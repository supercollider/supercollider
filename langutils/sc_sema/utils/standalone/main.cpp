
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

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
        std::cout << "ERROR: incorrect number of arguments to sc_sema_standalone, expected [dir|.sc file] "
                  << std::endl;
        return EXIT_FAILURE;
    }

#ifdef _MSC_VER
    // need this in order to ensure console output encoding handles UTF-8 properly
    if (SetConsoleOutputCP(CP_UTF8) == 0) {
        std::cerr << "ERROR: can't set console output codepage to UTF-8." << std::endl;
    }
#endif // _MSC_VER

    const auto num_threads = 6; // std::thread::hardware_concurrency();

    assert(argc > 0);
    const size_t num_mods = argc - 1;

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

            std::filesystem::path dir_path { get_path(argv[i + 1]) };

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

    comp.depth_first_do([&](const sc::ir::IRGraph& g) {
        if (auto roots = g.root()) {
            const auto class_i = std::get_if<sc::ir::SCClass_I>(&*roots);
            assert(class_i);
            g.depth_first_traverse(
                *class_i,
                [&](const sc::ir::IRGraph::Edges& edges, const sc::ir::IRGraph::Variant& payload,
                    const sc::ir::ASTLocation& loc, sc::ir::NodeIndex i, size_t depth) {
                    for (size_t di { 0 }; di < depth; ++di)
                        std::cout << "|  ";
                    std::cout << sc::ir::IRGraph::node_name(payload) << ": ";
                    std::visit(sc::util::overload {
                                   [](const auto&) {},
                                   [&](const sc::ir::SCClass& c) { std::cout << symbol_table(c.name) << ": "; },
                                   [&](const sc::ir::SCMessage& m) { std::cout << symbol_table(*m.selector) << ": "; },
                                   [&](const sc::ir::SCKwArg& k) { std::cout << symbol_table(*k.name) << ": "; },
                                   [&](const sc::ir::SCCollection& c) {
                                       std::cout << symbol_table(*c.collection_class) << ": ";
                                   } },
                               payload);
                    const auto view = loc.graph_index->text_info()->read(loc.graph_index->location(loc.node_index));
                    for (size_t i { 0 }; i < view.size(); ++i) {
                        if (view[i] == '\n') {
                            std::cout << "'\\n'";
                        } else {
                            std::cout.write(view.begin() + i, 1);
                        }
                    }
                    if (const auto const_i = g.node_as_constexpr(i)) {
                        std::cout << " = ";
                        ctx.constant_table(*const_i).to_json(std::cout, symbol_table);
                    }
                    std::cout << "\n";
                });
        }
    });
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
