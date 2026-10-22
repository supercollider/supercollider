// Reads source code from stdin and writes its ast, ir and diagnostics as json, for the explorer's web page, see
// serve.py.
//   sc_explorer [--class] [--prelude file.sc]...
// The source is interpreted code, or a class file with --class. The preludes are compiled first, as the class library,
// but aren't written out.
// A fresh process compiles each source, so an assertion on half typed code only loses that request.

#include "describe/describe.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_lexer/normalise_source.hpp"
#include "sc_lexer/text_info.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/sc_parser.hpp"
#include "sc_sema/compiler.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/intrinsic_constraint.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace {

using TextInfoPtr = std::shared_ptr<const sc::lex::TextInfo>;

std::string json_string(std::string_view s) {
    std::string out { "\"" };
    for (const char c : s) {
        switch (c) {
        case '"':
            out += "\\\"";
            break;
        case '\\':
            out += "\\\\";
            break;
        case '\n':
            out += "\\n";
            break;
        case '\t':
            out += "\\t";
            break;
        default:
            if (static_cast<unsigned char>(c) < 0x20) {
                char escaped[8];
                std::snprintf(escaped, sizeof escaped, "\\u%04x", static_cast<unsigned>(c));
                out += escaped;
            } else {
                out += c;
            }
        }
    }
    return out + "\"";
}

// Offsets are bytes into the normalised source, lines and columns count from one, as in the diagnostics.
std::string json_range(sc::lex::SourceCodeRange r) {
    std::ostringstream out;
    out << "{\"begin\":" << r.begin.absolute << ",\"end\":" << r.end.absolute << ",\"line\":" << r.begin.line_number + 1
        << ",\"column\":" << r.begin.column + 1 << "}";
    return out.str();
}

// A tree of either graph, flattened so a hidden node's children can be given to its parent.
struct Tree {
    struct Node {
        std::string name;
        std::string detail;
        // Only nodes from the source being explored, not the prelude's.
        std::optional<sc::lex::SourceCodeRange> range;
        std::vector<size_t> children;
    };
    std::vector<Node> nodes;
    std::vector<size_t> roots;
    // The visible ancestors of the node being added, with their depths.
    std::vector<std::pair<size_t, size_t>> open;

    // Adds a node seen at depth by a depth first traversal, which starts each root at depth 0.
    void add(size_t depth, Node node) {
        while (!open.empty() && open.back().first >= depth)
            open.pop_back();
        const auto i = nodes.size();
        nodes.push_back(std::move(node));
        (open.empty() ? roots : nodes[open.back().second].children).push_back(i);
        open.push_back({ depth, i });
    }
    // A new root, rather than a child of the last.
    void close() { open.clear(); }

    void write(std::ostream& out, size_t i) const {
        const auto& n = nodes[i];
        out << "{\"name\":" << json_string(n.name) << ",\"detail\":" << json_string(n.detail)
            << ",\"range\":" << (n.range ? json_range(*n.range) : "null") << ",\"children\":[";
        for (size_t c { 0 }; c < n.children.size(); ++c) {
            if (c)
                out << ",";
            write(out, n.children[c]);
        }
        out << "]}";
    }
    void write_roots(std::ostream& out, const std::vector<size_t>& which) const {
        out << "[";
        for (size_t r { 0 }; r < which.size(); ++r) {
            if (r)
                out << ",";
            write(out, which[r]);
        }
        out << "]";
    }
};

// Each node's source, on one line, as the ast's nodes only hold their location.
std::string one_line(std::string_view view) {
    std::string out;
    for (const char c : view)
        out += c == '\n' ? ' ' : c;
    return out;
}

// The ast's tree, and the nodes the parser left without a parent, e.g., ones error recovery didn't adopt.
struct AstTrees {
    Tree tree;
    std::vector<size_t> roots;
    std::vector<size_t> orphans;
};

AstTrees ast_trees(const sc::ast::ASTGraph& g) {
    AstTrees out;
    const auto add_tree = [&](sc::ast::Index root) {
        out.tree.close();
        g.depth_first_traverse(
            root,
            [&](const sc::ast::ASTGraph::Edges&, const sc::ast::ASTGraph::Variant& payload,
                const sc::lex::SourceCodeRange& loc, sc::ast::Index, size_t depth) {
                out.tree.add(depth,
                             { sc::ast::ASTGraph::node_name(payload), one_line(g.text_info()->read(loc)), loc, {} });
            });
        return out.tree.roots.back();
    };
    const auto root = g.root_any();
    if (root)
        out.roots.push_back(add_tree(*root));
    for (const auto i : g.orphans())
        if (i != root)
            out.orphans.push_back(add_tree(i));
    return out;
}

struct IrRoot {
    const sc::ir::IRGraph* graph;
    sc::ir::NodeIndex index;
    std::optional<sc::ir::SymSCClassName> class_name;
    sc::lex::SourceCodeLocation begin;
    bool is_meta;
};

void add_ir(Tree& tree, const sc::ir::Context& ctx, const IrRoot& r, const sc::lex::TextInfo* text) {
    const auto& g = *r.graph;
    const sc::ir::describe::Describe describe { g, ctx };
    tree.close();
    g.depth_first_traverse(r.index,
                           [&](const sc::ir::IRGraph::Edges& edges, const sc::ir::IRGraph::Variant& payload,
                               const sc::ir::SourceLocation& loc, sc::ir::NodeIndex i, size_t depth) {
                               if (sc::ir::describe::hidden(describe, edges, payload, r.class_name))
                                   return;
                               std::ostringstream detail;
                               sc::ir::describe::write_details(describe, detail, payload, loc, i);
                               auto range = g.text_info(loc.source).get() == text
                                   ? std::optional { sc::ir::IRGraph::range(loc) }
                                   : std::nullopt;
                               tree.add(depth, { sc::ir::IRGraph::node_name(payload), detail.str(), range, {} });
                           });
}

void write_diagnostic(std::ostream& out, const sc::diag::Diagnostic& d, const sc::lex::TextInfo* text) {
    out << "{\"name\":" << json_string(d.name())
        << ",\"severity\":" << (d.severity() == sc::diag::Severity::Error ? "\"error\"" : "\"warning\"")
        << ",\"text\":" << json_string(d.to_string()) << ",\"locations\":[";
    bool first = true;
    for (const auto& l : d.locations()) {
        if (l.text_info.get() != text)
            continue;
        if (!first)
            out << ",";
        first = false;
        out << "{\"range\":" << json_range(l.range) << ",\"message\":" << json_string(l.msg) << "}";
    }
    out << "]}";
}

TextInfoPtr make_text(std::string source, std::optional<std::filesystem::path> path, bool class_file) {
    return std::make_shared<const sc::lex::TextInfo>(sc::lex::NormalisedSource { source }, sc::lex::FileCodeLocation {},
                                                     std::move(path), class_file);
}

} // namespace

int main(int argc, char* argv[]) {
    bool class_mode = false;
    std::vector<std::filesystem::path> preludes;
    for (int i { 1 }; i < argc; ++i) {
        const std::string_view arg { argv[i] };
        if (arg == "--class") {
            class_mode = true;
        } else if (arg == "--prelude" && i + 1 < argc) {
            preludes.emplace_back(argv[++i]);
        } else {
            std::cerr << "usage: sc_explorer [--class] [--prelude file.sc]... < source\n";
            return EXIT_FAILURE;
        }
    }

    const auto text =
        make_text(std::string { std::istreambuf_iterator<char> { std::cin }, {} }, std::nullopt, class_mode);

    std::vector<sc::diag::Diagnostic> diagnostics;
    std::vector<std::optional<sc::ir::SourceEntry>> entries;
    std::shared_ptr<const sc::ast::ASTGraph> source_ast;

    // Each prelude is a module, and a class file the last.
    const auto add_class_file = [&](TextInfoPtr t, size_t module) {
        auto [ast, _] = sc::ast::parse(t);
        if (!ast)
            return std::shared_ptr<const sc::ast::ASTGraph> {};
        for (const auto& d : ast->diagnostics())
            diagnostics.push_back(d);
        const auto root = ast->root();
        if (root && std::holds_alternative<sc::ast::ClassOrExtensionListIndex>(*root))
            entries.push_back(sc::ir::SourceEntry { ast, std::get<sc::ast::ClassOrExtensionListIndex>(*root),
                                                    sc::ir::ModuleIndex::from(module) });
        return std::shared_ptr<const sc::ast::ASTGraph> { ast };
    };
    for (size_t i { 0 }; i < preludes.size(); ++i) {
        std::ifstream file { preludes[i] };
        if (!file.is_open()) {
            std::cerr << "Could not open prelude " << preludes[i] << "\n";
            return EXIT_FAILURE;
        }
        std::stringstream ss;
        ss << file.rdbuf();
        add_class_file(make_text(std::move(ss).str(), preludes[i], true), i);
    }
    if (class_mode)
        source_ast = add_class_file(text, preludes.size());

    // As ir_dump, these are only what the compiler needs, not the class library's constraints.
    sc::ir::SymbolTable symbol_table;
    sc::ir::ModuleTable module_table {};
    sc::ir::IntrinsicConstraints constraints {};
    const auto abstract_object = symbol_table.get_class_name("AbstractObjectExperimental");
    constraints.insert({ abstract_object, sc::ir::IntrinsicConstraint { abstract_object, {}, {}, {} } });
    const auto object = symbol_table.get_class_name("Object");
    constraints.insert({ object, sc::ir::IntrinsicConstraint { object, { abstract_object }, {}, {} } });
    const auto meta_object = symbol_table.get_meta_class_name(object);
    constraints.insert(
        { meta_object,
          sc::ir::IntrinsicConstraint { meta_object, { symbol_table.get_meta_class_name(abstract_object) }, {}, {} } });

    auto common_classes = sc::ir::symbol_table_and_common_classes(symbol_table, abstract_object);
    sc::ir::Context ctx { symbol_table, common_classes, module_table, entries };
    sc::ir::IRCompiler comp { ctx };
    for (auto& d : comp.compile_class_library(constraints, 1))
        diagnostics.push_back(std::move(d));

    std::vector<IrRoot> ir_roots;
    std::shared_ptr<sc::ir::IRGraph> interpreted;
    if (class_mode) {
        // In source order, each meta class after its class.
        comp.depth_first_do([&](const sc::ir::IRGraph& g) {
            const auto roots = g.root();
            if (!roots)
                return;
            const auto class_i = std::get<sc::ir::SCClass_I>(*roots);
            const auto loc = g.location(class_i);
            if (g.text_info(loc.source) != text || sc::ir::describe::repeats_class({ g, ctx }, class_i))
                return;
            const auto name = g.payload(class_i).name;
            ir_roots.push_back({ &g, sc::ir::NodeIndex { *class_i }, name, sc::ir::IRGraph::range(loc).begin,
                                 symbol_table.is_meta(name) });
        });
        std::sort(ir_roots.begin(), ir_roots.end(), [](const IrRoot& l, const IrRoot& r) {
            return std::tie(l.begin, l.is_meta) < std::tie(r.begin, r.is_meta);
        });
    } else {
        auto [ast, _] = sc::ast::parse(text);
        source_ast = ast;
        if (ast) {
            for (const auto& d : ast->diagnostics())
                diagnostics.push_back(d);
            const auto root = ast->root();
            if (root && !std::holds_alternative<sc::ast::ClassOrExtensionListIndex>(*root)) {
                auto r = comp.compile_interpreted(source_ast);
                for (auto& d : r.diagnostics)
                    diagnostics.push_back(std::move(d));
                interpreted = std::move(r.ir);
            }
        }
        if (interpreted) {
            const auto i = std::get<sc::ir::SCInterpreted_I>(*interpreted->root());
            ir_roots.push_back({ interpreted.get(), sc::ir::NodeIndex { *i }, std::nullopt, {}, false });
        }
    }

    Tree ir_tree;
    for (const auto& r : ir_roots)
        add_ir(ir_tree, ctx, r, text.get());

    std::ostream& out = std::cout;
    out << "{\"source\":" << json_string(text->source.as_string()) << ",\"ast\":{";
    if (source_ast) {
        const auto ast = ast_trees(*source_ast);
        out << "\"roots\":";
        ast.tree.write_roots(out, ast.roots);
        out << ",\"orphans\":";
        ast.tree.write_roots(out, ast.orphans);
    } else {
        out << "\"roots\":[],\"orphans\":[]";
    }
    out << "},\"ir\":{\"roots\":";
    ir_tree.write_roots(out, ir_tree.roots);
    out << "},\"diagnostics\":[";
    for (size_t i { 0 }; i < diagnostics.size(); ++i) {
        if (i)
            out << ",";
        write_diagnostic(out, diagnostics[i], text.get());
    }
    out << "]}\n";
    return 0;
}
