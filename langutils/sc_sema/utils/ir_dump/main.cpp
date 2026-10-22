
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <algorithm>

#include "boost/asio/post.hpp"
#include "boost/asio/thread_pool.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_lexer/normalise_source.hpp"
#include "sc_lexer/text_info.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/sc_parser.hpp"
#include "sc_sema/compiler.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/intrinsic_constraint.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"


#define DIAG_LOGGING true
#define TIME_LOGGING fasle

#if TIME_LOGGING
// milli
std::atomic<size_t> time_loading { 0 };
std::atomic<size_t> time_parsing { 0 };
#endif

std::optional<std::pair<std::shared_ptr<const sc::ast::ASTGraph>, sc::ast::ClassOrExtensionListIndex>>
parse_file(std::filesystem::path dir_entry) {
#if TIME_LOGGING
    const auto start = std::chrono::high_resolution_clock::now();
#endif
    std::stringstream ss { };

    std::ifstream file_stream { dir_entry };
    if (!file_stream.is_open()) {
#if DIAG_LOGGING
        std::cout << "Could not open file" << dir_entry << "\n";
#endif
        return { };
    }
    ss << file_stream.rdbuf();
    // std::cout << ss.rdbuf() << "\n";
    auto text_info = std::shared_ptr<sc::lex::TextInfo>(new sc::lex::TextInfo {
        sc::lex::NormalisedSource { std::move(ss).str() }, //
        sc::lex::FileCodeLocation { }, //
        dir_entry, //
        true //
    });

    const auto after_loading = std::chrono::high_resolution_clock::now();

#if TIME_LOGGING
    {
        const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(after_loading - start);
        time_loading.fetch_add(time_taken.count(), std::memory_order_relaxed);
    }
#endif

    auto [ast_graph, result] = sc::ast::parse(text_info);
    if (result != 0 || !ast_graph) {
#if DIAG_LOGGING
        std::cout << "Failed to parse " << dir_entry << " diagnostic count = " << ast_graph->diagnostics().size()
                  << "\n";
#endif
    }


#if DIAG_LOGGING
    for (const auto& d : ast_graph->diagnostics()) {
        std::cout << d;
    }
#endif

#if TIME_LOGGING
    {
        const auto after_parsing = std::chrono::high_resolution_clock::now();
        const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(after_parsing - after_loading);
        time_parsing.fetch_add(time_taken.count(), std::memory_order_relaxed);
    }
#endif

    const auto roots = ast_graph->root();
    if (!roots) {
#if DIAG_LOGGING
        std::cout << "No roots  " << dir_entry << "\n";
#endif
        return { };
    }
    if (!std::get_if<sc::ast::ClassOrExtensionListIndex>(&*roots)) {
#if DIAG_LOGGING
        std::cout << "Parser returned an scd file, not a class file.";
#endif
        return { };
    }

    const auto class_list_index = std::get<sc::ast::ClassOrExtensionListIndex>(*roots);
    return { { std::move(ast_graph), class_list_index } };
}


template <typename T> std::filesystem::path get_path(const T* c) {
#ifdef _MSC_VER
    const std::string source_str = utf8_encode(std::wstring(c));
    const auto* dir = source_str.c_str();
#else
    const char* const dir = c;
#endif // _MSC_VER

    return { dir };
}

#ifdef _MSC_VER
// utility function to convert default Windows stdin encoding to UTF-8
// https://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte
std::string utf8_encode(const std::wstring& wstr) {
    if (wstr.empty())
        return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);
    std::string strTo(static_cast<size_t>(size_needed), 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), strTo.data(), size_needed, NULL, NULL);
    return strTo;
}

// need wmain for Windows default stdin encoding
int wmain(int argc, wchar_t* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif // _MSC_VER
    if (argc <= 1) {
        std::cout << "ERROR: incorrect number of arguments to ir_dump, expected "
                     "[--prelude dir|.sc file]... [dir|.sc file]... "
                  << std::endl;
        return EXIT_FAILURE;
    }

#ifdef _MSC_VER
    // need this in order to ensure console output encoding handles UTF-8 properly
    if (SetConsoleOutputCP(CP_UTF8) == 0) {
        std::cerr << "ERROR: can't set console output codepage to UTF-8." << std::endl;
    }
#endif // _MSC_VER

    const auto num_threads = 1; // std::thread::hardware_concurrency();

    // Each path is a module. A path after --prelude is compiled, but its classes aren't dumped, e.g., the class
    // stubs shared by the regression tests.
    std::vector<std::filesystem::path> module_paths;
    std::vector<std::filesystem::path> prelude_paths;
    for (int i { 1 }; i < argc; ++i) {
        const auto arg = get_path(argv[i]);
        if (arg == "--prelude" && i + 1 < argc) {
            prelude_paths.push_back(get_path(argv[++i]));
            module_paths.push_back(prelude_paths.back());
        } else {
            module_paths.push_back(arg);
        }
    }
    const size_t num_mods = module_paths.size();

    const auto start_time = std::chrono::high_resolution_clock::now();

    std::mutex entries_lock;
    std::vector<std::optional<sc::ir::SourceEntry>> entries { };

    sc::ir::ModuleTable module_table { };


    std::vector<std::pair<std::filesystem::path, sc::ir::ModuleIndex>> sc_files;

    {
        boost::asio::thread_pool pool { num_threads };
        const auto start_time = std::chrono::high_resolution_clock::now();
        for (size_t i { 0 }; i < num_mods; ++i) {
            const auto module_index = sc::ir::ModuleIndex::from(i);

            const std::filesystem::path& dir_path = module_paths[i];

            std::error_code ec;
            if (std::filesystem::is_directory(dir_path, ec)) {
                for (auto const& dir_entry : std::filesystem::recursive_directory_iterator { dir_path })
                    if (dir_entry.is_regular_file() && dir_entry.path().extension() == ".sc") {
                        sc_files.push_back({ dir_entry.path(), module_index });
                    }
            } else if (std::filesystem::is_regular_file(dir_path)) {
                if (dir_path.extension() == ".sc") {
                    sc_files.push_back({ dir_path, module_index });
                }
            }
        }

#if TIME_LOGGING
        {
            const auto stop_time = std::chrono::high_resolution_clock::now();
            const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time);
            std::cout << "Time spent building sc_files = " << time_taken.count() / 1000.0 << "\n";
        }
#endif

        std::atomic<size_t> sc_file_i { 0 };

        entries.resize(sc_files.size());

        const auto process_files = [&]() {
            while (true) {
                const auto file_index = sc_file_i.fetch_add(1, std::memory_order_relaxed);
                if (file_index >= sc_files.size())
                    return;
                const auto& [path, module_index] = sc_files[file_index];
                if (auto r = parse_file(path)) {
                    assert(!entries[file_index].has_value());
                    entries[file_index] = sc::ir::SourceEntry { std::move(r->first), r->second, module_index };
                }
            }
        };


        for (size_t i { 0 }; i < num_threads; ++i) {
            boost::asio::post(pool, process_files);
        }

        pool.join();

#if TIME_LOGGING

        const auto finish_time = std::chrono::high_resolution_clock::now();
        const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(finish_time - start_time);

        const auto loading = time_loading / 1000.0;
        const auto parsing = time_parsing / 1000.0;

        std::cout << "cpu time spent loading = " << loading << " parsing = " << parsing
                  << " ratio = 1:" << parsing / loading << "\n";
        std::cout << " real time spend loading & parsing  = " << time_taken.count() / 1000.0 << "\n";
#endif
    }


    sc::ir::SymbolTable symbol_table;


    // These aren't the constraints of the class library.
    // They are just what is needed to get the compiler to work, and then to test the intrinsic system.
    sc::ir::IntrinsicConstraints constraints { };
    const auto abstract_object = symbol_table.get_class_name("AbstractObjectExperimental");
    constraints.insert({ abstract_object,
                         sc::ir::IntrinsicConstraint {
                             abstract_object, //
                             { }, //
                             { }, //
                             { } //
                         } });

    {
        const auto object = symbol_table.get_class_name("Object");
        constraints.insert({ object,
                             sc::ir::IntrinsicConstraint {
                                 object, //
                                 { abstract_object }, //
                                 { }, //
                                 { } //
                             } });
        const auto meta_obj = symbol_table.get_meta_class_name(object);
        constraints.insert({ meta_obj,
                             sc::ir::IntrinsicConstraint {
                                 meta_obj, { symbol_table.get_meta_class_name(abstract_object) }, { }, { } } });
    }
    { }


    auto common_c = sc::ir::symbol_table_and_common_classes(symbol_table, abstract_object);

    sc::ir::Context ctx { symbol_table, common_c, module_table, entries };

    sc::ir::IRCompiler comp { ctx };

    const auto diags = comp.compile_class_library(constraints, num_threads);

#if DIAG_LOGGING
    if (!diags.empty()) {
        for (const auto& d : diags)
            std::cout << d;
    }

    std::cout << "\n\n\nIR GRAPH DUMP \n\n\n";

    // A class is in a prelude if its file is one of the prelude paths, or is inside one of the prelude directories.
    const auto in_prelude = [&](const std::optional<std::filesystem::path>& file) {
        if (!file)
            return false;
        const auto canonical = std::filesystem::weakly_canonical(*file);
        for (const auto& prelude : prelude_paths) {
            const auto p = std::filesystem::weakly_canonical(prelude);
            const auto [end, _] = std::mismatch(p.begin(), p.end(), canonical.begin(), canonical.end());
            if (end == p.end())
                return true;
        }
        return false;
    };

    // Classes are dumped in the order they are written, by file then position, each meta class after its class, so
    // the dump doesn't depend on how the classes are hashed.
    struct DumpedClass {
        // Uses pointers because we are passed a reference and vectors don't like const refs.
        const sc::ir::IRGraph* graph;
        sc::ir::SCClass_I index;
        std::string file;
        sc::lex::SourceCodeLocation begin;
        bool is_meta;
    };
    std::vector<DumpedClass> classes;
    comp.depth_first_do([&](const sc::ir::IRGraph& g) {
        const auto roots = g.root();
        if (!roots)
            return;
        const auto class_i = std::get_if<sc::ir::SCClass_I>(&*roots);
        assert(class_i);
        const auto loc = g.location(*class_i);
        const auto& file = g.text_info(loc.source)->file_path;
        if (in_prelude(file))
            return;
        const auto& c = g.payload(*class_i);
        // A meta class with nothing of its own, no class variables or class methods, only repeats its class.
        if (symbol_table.is_meta(c.name)) {
            const auto [members, methods] = g.children(*class_i);
            bool has_own_member = false;
            for (auto it = g.children(members).iter(); it; ++it)
                has_own_member = has_own_member || g.payload(*it).owning_class == c.name;
            if (!has_own_member && !g.children(methods).iter())
                return;
        }
        classes.push_back({ &g, *class_i, file ? file->string() : std::string {}, sc::ir::IRGraph::range(loc).begin,
                            symbol_table.is_meta(c.name) });
    });
    std::sort(classes.begin(), classes.end(), [](const DumpedClass& l, const DumpedClass& r) {
        return std::tie(l.file, l.begin, l.is_meta) < std::tie(r.file, r.begin, r.is_meta);
    });

    // Lines and columns count from one, as in the diagnostics.
    const auto write_location = [](const sc::ir::SourceLocation& l) {
        const auto r = sc::ir::IRGraph::range(l);
        std::cout << "(" << r.begin.line_number + 1 << ":" << r.begin.column + 1 << ")";
    };
    const auto write_text = [](std::string_view view) {
        for (const char c : view) {
            if (c == '\n')
                std::cout << "'\\n'";
            else
                std::cout << c;
        }
    };
    const auto write_constant = [&](sc::ir::ConstExprIndex i) {
        ctx.constant_table(i).to_literal(std::cout, symbol_table);
    };
    const auto kind_name = [](sc::ir::SCAssign::Kind k) {
        switch (k) {
        case sc::ir::SCAssign::Kind::Local:
            return "local";
        case sc::ir::SCAssign::Kind::Instance:
            return "instance";
        case sc::ir::SCAssign::Kind::Class:
            return "class";
        case sc::ir::SCAssign::Kind::Environment:
            return "environment";
        }
        return "";
    };
    const auto read_kind_name = [&](sc::ir::SCRead::Kind k) {
        return kind_name(static_cast<sc::ir::SCAssign::Kind>(static_cast<int>(k)));
    };

    // Each node is `Name (line:column): details`, followed by its constant value, ` = value`, and, for methods and
    // functions, what calling them returns, ` -> value`. Only a method shows its source, the rest is found by location.
    for (const auto& dumped : classes) {
        const auto& g = *dumped.graph;
        const auto class_name = g.payload(dumped.index).name;
        // Where the variable is declared, shown by its source, so a read can be matched to its declaration.
        const auto write_declaration = [&](const std::optional<sc::ir::SourceLocation>& declaration) {
            if (!declaration)
                return;
            std::cout << ", declared ";
            write_location(*declaration);
            std::cout << ": ";
            write_text(g.read(*declaration));
        };
        g.depth_first_traverse(
            dumped.index,
            [&](const sc::ir::IRGraph::Edges& edges, const sc::ir::IRGraph::Variant& payload,
                const sc::ir::SourceLocation& loc, sc::ir::NodeIndex i, size_t depth) {
                // Inherited members are dumped with the class that declares them.
                if (const auto member = std::get_if<sc::ir::SCMember>(&payload);
                    member && member->owning_class != class_name)
                    return;
                // Empty argument lists add nothing.
                const bool is_argument_list = std::holds_alternative<sc::ir::SCArgumentDeclareList>(payload)
                    || std::holds_alternative<sc::ir::SCPositionalArgs>(payload)
                    || std::holds_alternative<sc::ir::SCKwArgs>(payload)
                    || std::holds_alternative<sc::ir::SCVariadicArgs>(payload);
                if (is_argument_list && !edges.first_child)
                    return;
                // A missing part only matters in a range, where it says which part is missing, e.g., `(1..)`.
                if (std::holds_alternative<sc::ir::SCMissing>(payload)) {
                    const sc::ir::NodeIndex parent { *edges.parent };
                    if (!g.as_a<sc::ir::SCRange_I>(parent) && !g.as_a<sc::ir::SCRangeGenerator_I>(parent))
                        return;
                }

                for (size_t di { 0 }; di < depth; ++di)
                    std::cout << "|  ";
                std::cout << sc::ir::IRGraph::node_name(payload) << " ";
                write_location(loc);
                std::visit(
                    sc::util::overload {
                        [](const auto&) {},
                        [&](const sc::ir::SCClass& c) { std::cout << ": " << symbol_table(c.name); },
                        [&](const sc::ir::SCMethod& m) {
                            std::cout << ": ";
                            if (m.poisoned)
                                std::cout << "poisoned: ";
                            write_text(g.read(loc));
                        },
                        [&](const sc::ir::SCMessage& m) { std::cout << ": " << symbol_table(*m.selector); },
                        [&](const sc::ir::SCKwArg& k) { std::cout << ": " << symbol_table(*k.name); },
                        [&](const sc::ir::SCVariableDeclare& v) { std::cout << ": " << symbol_table(*v.name); },
                        [&](const sc::ir::SCArgumentDeclare& a) { std::cout << ": " << symbol_table(a.name); },
                        [&](const sc::ir::SCMember& m) { std::cout << ": " << symbol_table(m.name); },
                        [&](const sc::ir::SCRead& r) {
                            std::cout << ": " << read_kind_name(r.kind) << " " << symbol_table(r.name);
                        },
                        [&](const sc::ir::SCAssign& a) {
                            std::cout << ": " << kind_name(a.kind) << " " << symbol_table(*a.name);
                        },
                        [&](const sc::ir::SCPackTarget& t) {
                            std::cout << ": " << kind_name(t.kind) << " " << symbol_table(*t.name)
                                      << (t.rest ? " rest from " : " at ") << t.index;
                        },
                        [&](const sc::ir::SCCollection& c) { std::cout << ": " << symbol_table(*c.collection_class); },
                        [&](const sc::ir::SCGenerator& gen) {
                            std::cout << (gen.kind == sc::ir::SCGenerator::Kind::Routine ? ": routine" : ": immediate");
                        },
                        [&](const sc::ir::SCGenQualIterate& q) {
                            std::cout << ": " << symbol_table(*q.element);
                            if (q.index)
                                std::cout << ", " << symbol_table(**q.index);
                        },
                        [&](const sc::ir::SCGenQualDeclare& q) { std::cout << ": " << symbol_table(*q.name); } },
                    payload);

                if (const auto const_i = g.node_as_constexpr(i)) {
                    std::cout << " = ";
                    write_constant(*const_i);
                }
                // A member's initial value, the member itself isn't constant.
                if (const auto member = std::get_if<sc::ir::SCMember>(&payload); member && member->expr) {
                    std::cout << " = ";
                    write_constant(*member->expr);
                }
                // What calling a method or function returns.
                const auto constant_result =
                    std::visit(sc::util::overload { [](const auto&) -> sc::ir::OptionalConstExprIndex { return {}; },
                                                    [](const sc::ir::SCMethod& m) { return m.constant_result; },
                                                    [](const sc::ir::SCFunction& f) { return f.constant_result; } },
                               payload);
                if (constant_result) {
                    std::cout << " -> ";
                    write_constant(*constant_result);
                }
                std::visit(sc::util::overload {
                               [](const auto&) {}, [&](const sc::ir::SCRead& r) { write_declaration(r.declaration); },
                               [&](const sc::ir::SCAssign& a) { write_declaration(a.declaration); },
                               [&](const sc::ir::SCPackTarget& t) { write_declaration(t.declaration); } },
                           payload);
                std::cout << "\n";
            });
    }
#endif

    std::cout << "Successfully compiled " << ctx.class_declarations.num_not_poisoned() / 2 << " classes." << std::endl;
#if TIME_LOGGING

    const auto finish_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(finish_time - start_time);
    const auto seconds = static_cast<double>(time_taken.count()) / 1000.0;

    std::cout << "TIME TAKEN = " << seconds;
#endif

    return 0;
}
