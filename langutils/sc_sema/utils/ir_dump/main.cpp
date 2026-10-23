
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
#include <charconv>
#include <thread>
#include <cstdint>
#include <map>
#include <optional>
#include <utility>

#include "describe/describe.hpp"
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
parse_file(std::filesystem::path dir_entry, std::vector<sc::diag::Diagnostic>& diagnostics) {
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

    auto [ast_graph, accepted] = sc::ast::parse(text_info);
    if (!accepted) {
#if DIAG_LOGGING
        std::cout << "Failed to parse " << dir_entry
                  << " diagnostic count = " << (ast_graph ? ast_graph->diagnostics().size() : 0) << "\n";
#endif
    }
    if (!ast_graph)
        return {};


    // Printed with the compiler's, see print_diagnostics.
    for (const auto& d : ast_graph->diagnostics())
        diagnostics.push_back(d);

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


// A release, e.g., `3.16`, as the version it compiles as. Nothing if it isn't `major.minor`, or is after the latest.
std::optional<sc::ast::LanguageVersion> parse_release(const std::string& release) {
    unsigned major { 0 };
    unsigned minor { 0 };
    const auto* end = release.data() + release.size();
    const auto [dot, major_error] = std::from_chars(release.data(), end, major);
    if (major_error != std::errc {} || dot == end || *dot != '.')
        return std::nullopt;
    const auto [last, minor_error] = std::from_chars(dot + 1, end, minor);
    if (minor_error != std::errc {} || last != end)
        return std::nullopt;
    return sc::ast::version_for_release(major, minor);
}

// An scd file, parsed as interpreted code. Null if it can't be read, or is a class file.
std::shared_ptr<const sc::ast::ASTGraph> parse_interpreted(const std::filesystem::path& path,
                                                           std::vector<sc::diag::Diagnostic>& diagnostics) {
    std::ifstream file_stream { path };
    if (!file_stream.is_open())
        return nullptr;
    std::stringstream ss {};
    ss << file_stream.rdbuf();
    auto text_info = std::shared_ptr<sc::lex::TextInfo>(new sc::lex::TextInfo {
        sc::lex::NormalisedSource { std::move(ss).str() }, //
        sc::lex::FileCodeLocation {}, //
        path, //
        false //
    });
    auto [ast_graph, _] = sc::ast::parse(text_info);
    if (!ast_graph)
        return nullptr;
    for (const auto& d : ast_graph->diagnostics())
        diagnostics.push_back(d);
    const auto root = ast_graph->root();
    if (!root || std::holds_alternative<sc::ast::ClassOrExtensionListIndex>(*root))
        return nullptr;
    return ast_graph;
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
                     "[--threads n] [--relative-to dir] [--interpreted-version x.y] [--prelude dir|.sc file]... "
                     "[dir|.sc file]... "
                  << std::endl;
        return EXIT_FAILURE;
    }

#ifdef _MSC_VER
    // need this in order to ensure console output encoding handles UTF-8 properly
    if (SetConsoleOutputCP(CP_UTF8) == 0) {
        std::cerr << "ERROR: can't set console output codepage to UTF-8." << std::endl;
    }
#endif // _MSC_VER

    // Each path is a module. A path after --prelude is compiled, but its classes aren't dumped, e.g., the class
    // stubs shared by the regression tests.
    // Diagnostics write their paths relative to the directory after --relative-to, rather than as absolute uris.
    // The output is in source order, so it is the same however many threads compile it, --threads sets how many, all
    // of them by default.
    // Interpreted code, the scd files, is compiled as the release after --interpreted-version, e.g., 3.16, as it has no
    // version comment, legacy by default.
    std::vector<std::filesystem::path> module_paths;
    std::vector<std::filesystem::path> prelude_paths;
    std::optional<std::filesystem::path> relative_to;
    size_t num_threads = std::max(1u, std::thread::hardware_concurrency());
    auto interpreted_version = sc::ast::LanguageVersion::Legacy;
    for (int i { 1 }; i < argc; ++i) {
        const auto arg = get_path(argv[i]);
        if (arg == "--threads" && i + 1 < argc) {
            const std::string count = get_path(argv[++i]).string();
            const auto [end, error] = std::from_chars(count.data(), count.data() + count.size(), num_threads);
            if (error != std::errc {} || end != count.data() + count.size() || num_threads == 0) {
                std::cout << "ERROR: --threads expects a positive number, not '" << count << "'" << std::endl;
                return EXIT_FAILURE;
            }
        } else if (arg == "--interpreted-version" && i + 1 < argc) {
            const std::string release = get_path(argv[++i]).string();
            const auto version = parse_release(release);
            if (!version) {
                std::cout << "ERROR: --interpreted-version expects a known release, e.g., 3.16, not '" << release << "'"
                          << std::endl;
                return EXIT_FAILURE;
            }
            interpreted_version = *version;
        } else if (arg == "--relative-to" && i + 1 < argc) {
            relative_to = get_path(argv[++i]);
        } else if (arg == "--prelude" && i + 1 < argc) {
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
    // Per file, so each thread only writes its own.
    std::vector<std::vector<sc::diag::Diagnostic>> parse_diagnostics;


    std::vector<std::pair<std::filesystem::path, sc::ir::ModuleIndex>> sc_files;
    // Compiled as interpreted code after the class library. Only scd files given as paths, not those in directories, as
    // a class library's directories have many.
    std::vector<std::filesystem::path> scd_files;

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
                } else if (dir_path.extension() == ".scd") {
                    scd_files.push_back(dir_path);
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
        parse_diagnostics.resize(sc_files.size());

        const auto process_files = [&]() {
            while (true) {
                const auto file_index = sc_file_i.fetch_add(1, std::memory_order_relaxed);
                if (file_index >= sc_files.size())
                    return;
                const auto& [path, module_index] = sc_files[file_index];
                if (auto r = parse_file(path, parse_diagnostics[file_index])) {
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

    auto diags = comp.compile_class_library(constraints, num_threads);

    // In path order, so the dump doesn't depend on how the directories are listed.
    std::sort(scd_files.begin(), scd_files.end());
    std::vector<std::shared_ptr<sc::ir::IRGraph>> interpreted_graphs;
    for (const auto& path : scd_files) {
        const auto ast_graph = parse_interpreted(path, diags);
        if (!ast_graph)
            continue;
        auto r = comp.compile_interpreted(ast_graph, interpreted_version);
        for (auto& d : r.diagnostics)
            diags.push_back(std::move(d));
        if (r.ir)
            interpreted_graphs.push_back(std::move(r.ir));
    }

#if DIAG_LOGGING
    std::vector<sc::diag::Diagnostic> all_diagnostics;
    for (auto& file : parse_diagnostics)
        for (auto& d : file)
            all_diagnostics.push_back(std::move(d));
    for (auto& d : diags)
        all_diagnostics.push_back(std::move(d));

    // Each line is prefixed by indent, so a diagnostic printed in the dump stays inside its node.
    const auto print_diagnostic = [&](const sc::diag::Diagnostic& d, const std::string& indent) {
        std::istringstream lines { relative_to ? d.to_string(*relative_to) : d.to_string() };
        for (std::string line; std::getline(lines, line);) {
            if (line.empty())
                std::cout << indent.substr(0, indent.find_last_not_of(' ') + 1) << "\n";
            else
                std::cout << indent << line << "\n";
        }
    };

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
        // The class, or interpreted code.
        sc::ir::NodeIndex index;
        std::optional<sc::ir::SymSCClassName> class_name;
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
        if (sc::ir::describe::repeats_class({ g, ctx }, *class_i))
            return;
        classes.push_back({ &g, sc::ir::NodeIndex { **class_i }, c.name, file ? file->string() : std::string {},
                            sc::ir::IRGraph::range(loc).begin, symbol_table.is_meta(c.name) });
    });
    for (const auto& g : interpreted_graphs) {
        const auto interpreted_i = std::get<sc::ir::SCInterpreted_I>(*g->root());
        const auto loc = g->location(sc::ir::NodeIndex { *interpreted_i });
        const auto& file = g->text_info(loc.source)->file_path;
        classes.push_back({ g.get(), sc::ir::NodeIndex { *interpreted_i }, std::nullopt,
                            file ? file->string() : std::string {}, sc::ir::IRGraph::range(loc).begin, false });
    }
    std::sort(classes.begin(), classes.end(), [](const DumpedClass& l, const DumpedClass& r) {
        return std::tie(l.file, l.begin, l.is_meta) < std::tie(r.file, r.begin, r.is_meta);
    });

    // Diagnostics are printed under the method whose source contains them, or else the class, so they are next to the
    // ir they affect. A diagnostic's last location is where the problem is, earlier ones are context, e.g., where a
    // duplicate was first declared. The rest, e.g., a missing class, are printed before the dump.
    struct Owner {
        const sc::ir::IRGraph* graph;
        sc::ir::NodeIndex node;
        std::shared_ptr<const sc::lex::TextInfo> text;
        sc::lex::SourceCodeRange range;
    };
    std::vector<Owner> method_owners;
    std::vector<Owner> class_owners;
    for (const auto& dumped : classes) {
        const auto& g = *dumped.graph;
        const auto owner = [&](sc::ir::NodeIndex node) {
            const auto loc = g.location(node);
            return Owner { &g, node, g.text_info(loc.source), sc::ir::IRGraph::range(loc) };
        };
        // Interpreted code is like a method.
        if (!dumped.class_name) {
            method_owners.push_back(owner(dumped.index));
            continue;
        }
        class_owners.push_back(owner(dumped.index));
        const auto [members, methods] = g.children(*g.as_a<sc::ir::SCClass_I>(dumped.index));
        for (auto it = g.children(methods).iter(); it; ++it)
            method_owners.push_back(owner(sc::ir::NodeIndex { **it }));
    }
    std::map<std::pair<const sc::ir::IRGraph*, std::uint32_t>, std::vector<const sc::diag::Diagnostic*>> owned;
    std::vector<const sc::diag::Diagnostic*> unowned;
    for (const auto& d : all_diagnostics) {
        const auto find_owner = [&](const std::vector<Owner>& owners) -> const Owner* {
            for (const auto& o : owners)
                if (d.within(o.text, o.range))
                    return &o;
            return nullptr;
        };
        const auto* o = find_owner(method_owners);
        if (!o)
            o = find_owner(class_owners);
        if (o)
            owned[{ o->graph, *o->node }].push_back(&d);
        else
            unowned.push_back(&d);
    }
    // In source order, by file then position, those without a location first.
    const auto source_order = [](const sc::diag::Diagnostic* l, const sc::diag::Diagnostic* r) {
        const auto key = [](const sc::diag::Diagnostic* d) {
            if (d->locations().empty())
                return std::tuple { false, std::string {}, std::uint32_t { 0 } };
            const auto& loc = d->locations().back();
            const auto& path = loc.text_info->file_path;
            return std::tuple { true, path ? path->string() : std::string {}, loc.range.begin.absolute };
        };
        return key(l) < key(r);
    };
    std::stable_sort(unowned.begin(), unowned.end(), source_order);
    for (auto& [_, ds] : owned)
        std::stable_sort(ds.begin(), ds.end(), source_order);

    for (const auto* d : unowned)
        print_diagnostic(*d, "");
    std::cout << "\n\n\nIR GRAPH DUMP \n\n\n";

    // Each node is `Name (line:column): details`, see describe::write_details.
    for (const auto& dumped : classes) {
        const auto& g = *dumped.graph;
        const sc::ir::describe::Describe describe { g, ctx, relative_to };
        g.depth_first_traverse(dumped.index,
                               [&](const sc::ir::IRGraph::Edges& edges, const sc::ir::IRGraph::Variant& payload,
                                   const sc::ir::SourceLocation& loc, sc::ir::NodeIndex i, size_t depth) {
                                   if (sc::ir::describe::hidden(describe, edges, payload, dumped.class_name))
                                       return;

                                   for (size_t di { 0 }; di < depth; ++di)
                                       std::cout << "|  ";
                                   std::cout << sc::ir::IRGraph::node_name(payload) << " ";
                                   sc::ir::describe::write_location(std::cout, loc);
                                   sc::ir::describe::write_details(describe, std::cout, payload, loc, i);
                                   std::cout << "\n";

                                   if (const auto fnd = owned.find({ &g, *i }); fnd != owned.end()) {
                                       std::string indent;
                                       for (size_t di { 0 }; di <= depth; ++di)
                                           indent += "|  ";
                                       for (const auto* d : fnd->second)
                                           print_diagnostic(*d, indent);
                                   }
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
