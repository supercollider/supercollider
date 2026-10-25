// Reads source code from stdin and writes its ast, ir, lir, bytecode and diagnostics as json, for the explorer's web
// page, see serve.py.
//   sc_explorer [--class] [--prelude file.sc]...
// The source is interpreted code, or a class file with --class. The preludes are compiled first, as the class library,
// but aren't written out.
// A fresh process compiles each source, so an assertion on half typed code only loses that request.

#include "describe/describe.hpp"
#include "sc_backend/emit.hpp"
#include "sc_backend/frames.hpp"
#include "sc_backend/lower.hpp"
#include "sc_backend/members.hpp"
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
#include <cassert>
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

using BackendRoot = sc::ir::join<sc::ir::SCClass_I, sc::ir::SCInterpreted_I>;

struct IrRoot {
    const sc::ir::IRGraph* graph;
    BackendRoot root;
    std::optional<sc::ir::SymSCClassName> class_name;
    sc::lex::SourceCodeLocation begin;
    bool is_meta;
};

void add_ir(Tree& tree, const sc::ir::Context& ctx, const IrRoot& r, const sc::lex::TextInfo* text) {
    const auto& g = *r.graph;
    const sc::ir::describe::Describe describe { g, ctx };
    tree.close();
    g.depth_first_traverse(r.root,
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

struct Backend {
    const sc::ir::Context& ctx;
    const sc::backend::MemberTable& members;
    const sc::backend::LoweringSymbols& selectors;
    const sc::backend::SpecialSymbols& special_symbols;
    const sc::backend::InliningContext& inlining;
};

// A range from byte offsets into the text, as the lowered ir's spans only hold those.
sc::lex::SourceCodeRange range_from_offsets(const sc::lex::TextInfo& text, std::uint32_t begin, std::uint32_t end) {
    const auto source = std::string_view { text.source.as_string() };
    const auto at = [&](std::uint32_t offset) {
        const auto before = source.substr(0, offset);
        const auto line_start = before.rfind('\n');
        return sc::lex::SourceCodeLocation {
            offset, static_cast<std::uint32_t>(std::count(before.begin(), before.end(), '\n')),
            static_cast<std::uint32_t>(line_start == std::string_view::npos ? offset : offset - line_start - 1)
        };
    };
    return { at(begin), at(end) };
}

// As sc_inspect --lir and --bytecode, the root's class, then each function, starting with its frame. In the lir, each
// function's regions, the body first, then their instructions, all functions, marking those of a method that can't be
// emitted. In the bytecode, the lines write_emitted prints for each emitted function, each opcode's at its source.
void add_backend(Tree& lir_tree, Tree& bytecode_tree, const Backend& b, const IrRoot& r, const sc::lex::TextInfo* text,
                 std::vector<sc::diag::Diagnostic>& diagnostics) {
    const auto& g = *r.graph;
    const auto& ctx = b.ctx;
    const auto frames = sc::backend::layout_frames(g, b.inlining, r.root);
    const auto lowered = sc::backend::lower({ g, frames, b.members, ctx.constant_table, ctx.symbol_table, b.selectors,
                                              b.inlining, ctx.common_classes.array, ctx.common_classes.event },
                                            r.root);
    const auto class_node = g.as_a<sc::ir::SCClass_I>(r.root);
    const bool interpreter_members = !class_node || g.payload(*class_node).name == ctx.common_symbols.interpreter;
    const auto emitted = sc::backend::emit(
        { g, frames, b.members, ctx.constant_table, b.special_symbols, interpreter_members }, lowered);
    for (const auto& d : frames.diagnostics)
        diagnostics.push_back(d);
    for (const auto& d : emitted.diagnostics)
        diagnostics.push_back(d);

    const auto range_of = [&](sc::ir::NodeIndex i) {
        const auto loc = g.location(i);
        return g.text_info(loc.source).get() == text ? std::optional { sc::ir::IRGraph::range(loc) } : std::nullopt;
    };
    // The frame's slots, as `this`, arguments, variadic arguments, then variables, an inlined function's last. Slot
    // numbers are as the code addresses them, from this frame or, at a depth, an enclosing one.
    const auto add_frame = [&](Tree& tree, const sc::backend::Frame& frame) {
        tree.add(2,
                 { "frame",
                   "depth " + std::to_string(frame.depth) + ", " + std::to_string(frame.num_slots())
                       + (frame.num_slots() == 1 ? " slot" : " slots"),
                   std::nullopt,
                   {} });
        const std::uint32_t first { frame.has_this ? 1u : 0u };
        for (std::uint32_t si { 0 }; si < frame.slots.size(); ++si) {
            const auto slot = frame.slots[sc::backend::FrameSlotIndex { si }];
            const auto name = "slot " + std::to_string(si);
            if (!slot) {
                tree.add(3, { name, "this", std::nullopt, {} });
                continue;
            }
            const auto& local = g.local(sc::ir::LocalIndex { *slot });
            const sc::ir::NodeIndex local_owner { *local.owner };
            const auto l = si - first;
            std::string detail;
            if (l < frame.num_positional)
                detail = "argument ";
            else if (l < frame.num_positional + frame.num_variadic())
                detail = "variadic argument ...";
            else if (g.is_a<sc::ir::SCFunction_I>(local_owner) && local_owner != frame.owner)
                detail = "hoisted var ";
            else
                detail = "var ";
            detail += ctx.symbol_table(local.name);
            if (local.captured)
                detail += ", captured";
            if (local.assigned_to)
                detail += ", assigned";
            const auto range = g.text_info(local.declaration.source).get() == text
                ? std::optional { sc::ir::IRGraph::range(local.declaration) }
                : std::nullopt;
            tree.add(3, { name, detail, range, {} });
        }
    };
    const Tree::Node class_row {
        r.class_name ? std::string { ctx.symbol_table(*r.class_name) } : "Interpreted", "", range_of(r.root), {}
    };
    for (auto* tree : { &lir_tree, &bytecode_tree }) {
        tree->close();
        tree->add(0, class_row);
    }
    std::optional<sc::backend::lir::FunctionIndex> root;
    for (std::uint32_t fi { 0 }; fi < lowered.functions.size(); ++fi) {
        const sc::backend::lir::FunctionIndex index { fi };
        const auto& function = lowered.functions[index];
        const auto& frame = frames.frames[function.frame];
        if (!frame.parent)
            root = index;
        const sc::ir::NodeIndex owner { *frame.owner };
        std::string detail;
        if (const auto method = g.as_a<sc::ir::SCMethod_I>(owner))
            detail = "method " + std::string { ctx.symbol_table(*g.payload(*method).selector) };
        const bool is_emitted = std::find(emitted.roots.begin(), emitted.roots.end(), *root) != emitted.roots.end();
        const bool not_lowered = !frame.parent && !function.complete;

        lir_tree.add(
            1,
            { "function " + std::to_string(fi), detail + (not_lowered ? " (not emitted)" : ""), range_of(owner), {} });
        add_frame(lir_tree, frame);
        const bool in_text = g.text_info(function.source).get() == text;
        for (std::uint32_t ri { 0 }; ri < function.sub_regions.size(); ++ri) {
            const auto& region = function.sub_regions[sc::backend::lir::FunctionSubRegionIndex { ri }];
            lir_tree.add(2, { "region " + std::to_string(ri), "", std::nullopt, {} });
            for (std::uint32_t k { *region.begin }; k < *region.begin + region.size; ++k) {
                const sc::backend::lir::InstructionIndex i { k };
                std::ostringstream line;
                sc::backend::write_instruction(line, function, function.instructions[i], g, ctx.symbol_table,
                                               ctx.constant_table);
                const auto span = function.spans[i];
                lir_tree.add(
                    3,
                    { "",
                      line.str(),
                      in_text ? std::optional { range_from_offsets(*text, span.begin, span.end) } : std::nullopt,
                      {} });
            }
        }

        if (!is_emitted || emitted.functions[index].thrown_away)
            continue;
        bytecode_tree.add(1, { "function " + std::to_string(fi), detail, range_of(owner), {} });
        add_frame(bytecode_tree, frame);
        // write_emitted's lines are the properties, selectors and constants, then one per opcode, in order.
        const auto& emitted_function = emitted.functions[index];
        const auto& code = emitted_function.code;
        std::ostringstream lines;
        sc::backend::write_emitted(lines, emitted_function, "", ctx.symbol_table, ctx.constant_table);
        std::istringstream in { lines.str() };
        std::size_t line_index { 0 };
        for (std::string line; std::getline(in, line); ++line_index) {
            constexpr std::size_t table_lines = 3;
            std::optional<sc::lex::SourceCodeRange> range;
            if (in_text && line_index >= table_lines) {
                const auto op = CompilingBytecodes::OpcodeIndex::from(line_index - table_lines);
                assert(code.locations.contains(op));
                const auto location = code.locations[op];
                range = range_from_offsets(*text, static_cast<std::uint32_t>(location.begin),
                                           static_cast<std::uint32_t>(location.end));
            }
            bytecode_tree.add(2, { "", line, range, {} });
        }
    }
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
    sc::ir::ClassLibrarySources entries;
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
            (void)entries.push_back(sc::ir::SourceEntry { ast, std::get<sc::ast::ClassOrExtensionListIndex>(*root),
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

    // As sc_inspect, these are only what the compiler needs, not the class library's constraints.
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
            ir_roots.push_back({ &g, class_i, name, sc::ir::IRGraph::range(loc).begin, symbol_table.is_meta(name) });
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
            ir_roots.push_back({ interpreted.get(), i, std::nullopt, {}, false });
        }
    }

    Tree ir_tree;
    for (const auto& r : ir_roots)
        add_ir(ir_tree, ctx, r, text.get());

    const sc::backend::MemberTable members { comp, symbol_table };
    const sc::backend::LoweringSymbols selectors { symbol_table };
    const sc::backend::SpecialSymbols special_symbols { symbol_table };
    const sc::backend::InliningContext inlining { special_symbols, ctx.constant_table };
    const Backend backend { ctx, members, selectors, special_symbols, inlining };
    Tree lir_tree;
    Tree bytecode_tree;
    for (const auto& r : ir_roots)
        add_backend(lir_tree, bytecode_tree, backend, r, text.get(), diagnostics);

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
    out << "},\"lir\":{\"roots\":";
    lir_tree.write_roots(out, lir_tree.roots);
    out << "},\"bytecode\":{\"roots\":";
    bytecode_tree.write_roots(out, bytecode_tree.roots);
    out << "},\"diagnostics\":[";
    for (size_t i { 0 }; i < diagnostics.size(); ++i) {
        if (i)
            out << ",";
        write_diagnostic(out, diagnostics[i], text.get());
    }
    out << "]}\n";
    return 0;
}
