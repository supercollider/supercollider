
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string_view>
#include <string>
#include <tuple>
#include <vector>
#include <algorithm>
#include <charconv>
#include <thread>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <optional>
#include <utility>

#include "describe/describe.hpp"
#include "boost/asio/post.hpp"
#include "boost/asio/thread_pool.hpp"
#include "sc_backend/emit.hpp"
#include "sc_backend/frames.hpp"
#include "sc_backend/lower.hpp"
#include "sc_backend/members.hpp"
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
parse_file(std::filesystem::path dir_entry, std::vector<sc::diag::Diagnostic>& diagnostics, std::ostream& log) {
#if TIME_LOGGING
    const auto start = std::chrono::high_resolution_clock::now();
#endif
    std::stringstream ss { };

    std::ifstream file_stream { dir_entry };
    if (!file_stream.is_open()) {
#if DIAG_LOGGING
        log << "Could not open file" << dir_entry << "\n";
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
        log << "Failed to parse " << dir_entry
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
        log << "No roots  " << dir_entry << "\n";
#endif
        return { };
    }
    if (!std::get_if<sc::ast::ClassOrExtensionListIndex>(&*roots)) {
#if DIAG_LOGGING
        log << "Parser returned an scd file, not a class file.\n";
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

// How long each stage took, in seconds, see --time. The backend's are summed over its graphs.
struct StageTimes {
    double find_files = 0;
    double parse = 0;
    double sema = 0;
    double interpreted_parse = 0;
    double interpreted_sema = 0;
    double member_table = 0;
    double frames = 0;
    double lower = 0;
    double emit = 0;
};

using Clock = std::chrono::steady_clock;

double seconds_since(Clock::time_point start) { return std::chrono::duration<double>(Clock::now() - start).count(); }

void write_times(std::ostream& out, const StageTimes& t, double total) {
    const auto row = [&](const char* name, double seconds) {
        out << "  " << name << std::string(20 - std::string_view { name }.size(), ' ') << std::fixed
            << std::setprecision(6) << seconds << " s\n";
    };
    out << "Stage times:\n";
    row("find files", t.find_files);
    row("parse", t.parse);
    row("sema", t.sema);
    row("interpreted parse", t.interpreted_parse);
    row("interpreted sema", t.interpreted_sema);
    row("member table", t.member_table);
    row("frames", t.frames);
    row("lower", t.lower);
    row("emit", t.emit);
    row("total", total);
    out << std::defaultfloat;
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
        std::cout << "ERROR: incorrect number of arguments to sc_inspect, expected "
                     "[--threads n] [--relative-to dir] [--interpreted-version x.y] [--no-dump] [--frames] [--lir] "
                     "[--bytecode] [--time] [--prelude dir|.sc file]... [dir|.sc file]... "
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
    // --no-dump only compiles, printing how many diagnostics and classes there are, e.g., to time the compiler, as
    // writing the dump takes longer than compiling.
    // --frames adds the backend's frame layout, each frame's slots, and where each variable read or assigned is.
    // --lir adds the backend's lowered ir, after the dump, each class's functions and their instructions.
    // --bytecode adds the backend's bytecode after that, each function's tables and bytes.
    // With --no-dump, --frames, --lir or --bytecode run the backend up to that stage over every graph without printing
    // it, timing the backend on its own.
    // --time prints how long each stage took, after everything else, see StageTimes. The total is from after the
    // arguments are read until before the dump is written, so it leaves out printing.
    std::vector<std::filesystem::path> module_paths;
    std::vector<std::filesystem::path> prelude_paths;
    std::optional<std::filesystem::path> relative_to;
    size_t num_threads = std::max(1u, std::thread::hardware_concurrency());
    auto interpreted_version = sc::ast::LanguageVersion::Legacy;
    bool dump = true;
    bool show_frames = false;
    bool show_lir = false;
    bool show_bytecode = false;
    bool show_times = false;
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
        } else if (arg == "--no-dump") {
            dump = false;
        } else if (arg == "--frames") {
            show_frames = true;
        } else if (arg == "--lir") {
            show_lir = true;
        } else if (arg == "--bytecode") {
            show_bytecode = true;
        } else if (arg == "--time") {
            show_times = true;
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
    const auto total_start = Clock::now();
    StageTimes times;

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
        const auto find_start = Clock::now();
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

        times.find_files = seconds_since(find_start);
        const auto parse_start = Clock::now();
        std::atomic<size_t> sc_file_i { 0 };

        entries.resize(sc_files.size());
        parse_diagnostics.resize(sc_files.size());
        // Per file, printed in order once they are all parsed, so the output doesn't depend on the threads.
        std::vector<std::ostringstream> parse_logs(sc_files.size());

        const auto process_files = [&]() {
            while (true) {
                const auto file_index = sc_file_i.fetch_add(1, std::memory_order_relaxed);
                if (file_index >= sc_files.size())
                    return;
                const auto& [path, module_index] = sc_files[file_index];
                if (auto r = parse_file(path, parse_diagnostics[file_index], parse_logs[file_index])) {
                    assert(!entries[file_index].has_value());
                    entries[file_index] = sc::ir::SourceEntry { std::move(r->first), r->second, module_index };
                }
            }
        };


        for (size_t i { 0 }; i < num_threads; ++i) {
            boost::asio::post(pool, process_files);
        }

        pool.join();
        times.parse = seconds_since(parse_start);
        for (const auto& log : parse_logs)
            std::cout << log.str();

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

    const auto sema_start = Clock::now();
    auto diags = comp.compile_class_library(constraints, num_threads);
    times.sema = seconds_since(sema_start);

    // In path order, so the dump doesn't depend on how the directories are listed. Each file is parsed, then
    // compiled, on several threads, each writing only its own entry, and gathered in that order.
    std::sort(scd_files.begin(), scd_files.end());
    const auto for_each_scd_file = [&](auto&& f) {
        std::atomic<size_t> next { 0 };
        boost::asio::thread_pool pool { num_threads };
        for (size_t i { 0 }; i < num_threads; ++i)
            boost::asio::post(pool, [&] {
                for (size_t file_index; (file_index = next.fetch_add(1, std::memory_order_relaxed)) < scd_files.size();)
                    f(file_index);
            });
        pool.join();
    };
    std::vector<std::vector<sc::diag::Diagnostic>> scd_diagnostics(scd_files.size());
    std::vector<std::shared_ptr<const sc::ast::ASTGraph>> scd_asts(scd_files.size());
    std::vector<std::shared_ptr<sc::ir::IRGraph>> scd_graphs(scd_files.size());
    const auto interpreted_parse_start = Clock::now();
    for_each_scd_file([&](size_t i) { scd_asts[i] = parse_interpreted(scd_files[i], scd_diagnostics[i]); });
    times.interpreted_parse = seconds_since(interpreted_parse_start);
    const auto interpreted_sema_start = Clock::now();
    for_each_scd_file([&](size_t i) {
        if (!scd_asts[i])
            return;
        auto r = comp.compile_interpreted(scd_asts[i], interpreted_version);
        for (auto& d : r.diagnostics)
            scd_diagnostics[i].push_back(std::move(d));
        scd_graphs[i] = std::move(r.ir);
    });
    times.interpreted_sema = seconds_since(interpreted_sema_start);
    std::vector<std::shared_ptr<sc::ir::IRGraph>> interpreted_graphs;
    for (size_t i { 0 }; i < scd_files.size(); ++i) {
        for (auto& d : scd_diagnostics[i])
            diags.push_back(std::move(d));
        if (scd_graphs[i])
            interpreted_graphs.push_back(std::move(scd_graphs[i]));
    }

    // The backend's frames, lowered ir and bytecode, by graph, and its diagnostics, printed with the others. Each
    // frame's owner is written when the owner is, which for a qualifier is before its frame starts, so they are found
    // by owner. Each stage only runs if it, or a later one, is shown.
    using BackendRoot = sc::ir::join<sc::ir::SCClass_I, sc::ir::SCInterpreted_I>;
    struct DumpFrames {
        BackendRoot root;
        sc::backend::Frames layout;
        std::unordered_map<std::uint32_t, sc::backend::FrameIndex> by_owner;
        sc::backend::lir::LoweredGraph lowered {};
        sc::backend::EmittedGraph emitted {};
    };
    const bool show_backend = show_frames || show_lir || show_bytecode;
    const sc::backend::LoweringSymbols selectors { symbol_table };
    const sc::backend::SpecialSymbols special_symbols { symbol_table };
    const sc::backend::InliningContext inlining { special_symbols, ctx.constant_table };
    std::optional<sc::backend::MemberTable> member_table;
    if (show_backend) {
        const auto member_table_start = Clock::now();
        member_table.emplace(comp, symbol_table);
        times.member_table = seconds_since(member_table_start);
    }
    const auto run_backend = [&](const sc::ir::IRGraph& g, BackendRoot root,
                                 std::vector<sc::diag::Diagnostic>& diagnostics) {
        const auto frames_start = Clock::now();
        DumpFrames f { root, sc::backend::layout_frames(g, inlining, root) };
        for (auto& d : f.layout.diagnostics)
            diagnostics.push_back(std::move(d));
        for (std::uint32_t fi { 0 }; fi < f.layout.frames.size(); ++fi)
            f.by_owner.insert(
                { *f.layout.frames[sc::backend::FrameIndex { fi }].owner, sc::backend::FrameIndex { fi } });
        times.frames += seconds_since(frames_start);
        if (show_lir || show_bytecode) {
            const auto lower_start = Clock::now();
            const sc::backend::LoweringContext lowering { g,
                                                          f.layout,
                                                          *member_table,
                                                          ctx.constant_table,
                                                          symbol_table,
                                                          selectors,
                                                          inlining,
                                                          ctx.common_classes.array,
                                                          ctx.common_classes.event };
            f.lowered = sc::backend::lower(lowering, root);
            times.lower += seconds_since(lower_start);
        }
        if (show_bytecode) {
            const auto emit_start = Clock::now();
            const auto class_node = g.as_a<sc::ir::SCClass_I>(sc::ir::NodeIndex { *root });
            const bool interpreter_members =
                !class_node || g.payload(*class_node).name == ctx.common_symbols.interpreter;
            const sc::backend::EmitContext emitting { g, f.layout, ctx.constant_table, special_symbols,
                                                      interpreter_members };
            f.emitted = sc::backend::emit(emitting, f.lowered);
            times.emit += seconds_since(emit_start);
            for (auto& d : f.emitted.diagnostics)
                diagnostics.push_back(std::move(d));
        }
        return f;
    };

    if (!dump) {
        std::size_t count = diags.size();
        for (const auto& file : parse_diagnostics)
            count += file.size();
        // Every graph, prelude and empty meta classes too, each dropped once run, so it only times the backend.
        if (show_backend) {
            std::vector<sc::diag::Diagnostic> backend_diagnostics;
            comp.depth_first_do([&](const sc::ir::IRGraph& g) {
                if (const auto roots = g.root())
                    run_backend(g, std::get<sc::ir::SCClass_I>(*roots), backend_diagnostics);
            });
            for (const auto& g : interpreted_graphs)
                run_backend(*g, std::get<sc::ir::SCInterpreted_I>(*g->root()), backend_diagnostics);
            count += backend_diagnostics.size();
        }
        const auto total = seconds_since(total_start);
        std::cout << count << " diagnostics.\n";
        std::cout << "Successfully compiled " << ctx.class_declarations.num_not_poisoned() / 2 << " classes."
                  << std::endl;
        if (show_times)
            write_times(std::cout, times, total);
        return 0;
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

    std::map<const sc::ir::IRGraph*, DumpFrames> frames;
    if (show_backend) {
        for (const auto& dumped : classes) {
            const auto& g = *dumped.graph;
            const auto root = [&]() -> BackendRoot {
                if (const auto c = g.as_a<sc::ir::SCClass_I>(dumped.index))
                    return *c;
                return *g.as_a<sc::ir::SCInterpreted_I>(dumped.index);
            }();
            frames.insert({ dumped.graph, run_backend(g, root, all_diagnostics) });
        }
    }
    const auto total = seconds_since(total_start);

    // `[frame: this, a, ...b | c]` for a frame's slots, its arguments, variadic arguments, then variables.
    // `[depth 1, slot 2]` for a local, `[instance slot 3]`, `[class variable 0 of Meta_Foo]` or `[constant]` for a
    // member. current is the frame the node is evaluated in, see FrameCursor.
    const auto write_frame = [&](const sc::ir::IRGraph& g, const sc::ir::IRGraph::Variant& payload, sc::ir::NodeIndex i,
                                 sc::backend::OptionalFrameIndex current) {
        const auto fnd = frames.find(&g);
        if (!show_frames || fnd == frames.end())
            return;
        const auto& f = fnd->second.layout;
        const auto owner = [&](sc::backend::FrameOwner_I o) {
            const auto owned = fnd->second.by_owner.find(*o);
            if (owned == fnd->second.by_owner.end())
                return;
            const auto& frame = f.frames[owned->second];
            std::cout << " [frame";
            const char* separator = ": ";
            if (frame.has_this) {
                std::cout << separator << "this";
                separator = ", ";
            }
            const std::uint32_t first { frame.has_this ? 1u : 0u };
            for (std::uint32_t l { 0 }; l + first < frame.slots.size(); ++l) {
                if (l == frame.num_positional + frame.num_variadic())
                    separator = l == 0 && !frame.has_this ? ": | " : " | ";
                std::cout << separator;
                if (l >= frame.num_positional && l < frame.num_positional + frame.num_variadic())
                    std::cout << "...";
                const auto local = frame.slots[sc::backend::FrameSlotIndex { l + first }];
                std::cout << symbol_table(g.local(sc::ir::LocalIndex { *local }).name);
                separator = ", ";
            }
            std::cout << "]";
        };
        const auto local = [&](sc::ir::OptionalLocalIndex l) {
            const auto& slot = f.locations[sc::ir::LocalIndex { *l }];
            if (!current || !slot.frame)
                return;
            std::cout << " [depth " << f.depth(*current, *slot.frame) << ", slot " << *slot.slot << "]";
        };
        const auto member = [&](sc::ir::OptionalMemberIndex m) {
            const auto& variable = g.member(sc::ir::MemberIndex { *m });
            const auto location = member_table->find(variable.owner, variable.name);
            if (!location) {
                std::cout << " [member not found]";
                return;
            }
            // A class variable as its position among its class's own, so it doesn't depend on the other classes.
            std::visit(sc::util::overload {
                           [](sc::backend::InstanceSlotIndex slot) { std::cout << " [instance slot " << *slot << "]"; },
                           [&](sc::backend::ClassVariableIndex index) {
                               std::cout << " [class variable "
                                         << *index - *member_table->class_variable_base(variable.owner) << " of "
                                         << symbol_table(variable.owner) << "]";
                           },
                           [](sc::ir::OptionalConstExprIndex) { std::cout << " [constant]"; },
                       },
                       *location);
        };
        const auto variable = [&](sc::ir::OptionalLocalIndex l, sc::ir::OptionalMemberIndex m) {
            if (l)
                local(l);
            else if (m)
                member(m);
        };
        using namespace sc::ir;
        std::visit(sc::util::overload {
                       [](const auto&) {},
                       [&](const SCMethod&) { owner(*g.as_a<SCMethod_I>(i)); },
                       [&](const SCFunction&) { owner(*g.as_a<SCFunction_I>(i)); },
                       [&](const SCGenQualIterate&) { owner(*g.as_a<SCGenQualIterate_I>(i)); },
                       [&](const SCGenQualDeclare&) { owner(*g.as_a<SCGenQualDeclare_I>(i)); },
                       [&](const SCRoutineGenerator&) { owner(*g.as_a<SCRoutineGenerator_I>(i)); },
                       [&](const SCRead& r) { variable(r.local, r.member); },
                       [&](const SCAssign& a) { variable(a.local, a.member); },
                       [&](const SCPackTarget& t) { variable(t.local, t.member); },
                   },
                   payload);
    };

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
        // Every node is entered and exited, even those hidden, so it follows the frames.
        std::optional<sc::backend::FrameCursor> cursor;
        if (const auto fnd = frames.find(&g); show_frames && fnd != frames.end())
            cursor.emplace(g, inlining, fnd->second.layout);
        g.depth_first_traverse(
            dumped.index,
            [&](const sc::ir::IRGraph::Edges& edges, const sc::ir::IRGraph::Variant& payload,
                const sc::ir::SourceLocation& loc, sc::ir::NodeIndex i, size_t depth) {
                const auto current = cursor ? cursor->current() : sc::backend::OptionalFrameIndex {};
                if (cursor)
                    (void)cursor->enter(i);
                if (sc::ir::describe::hidden(describe, edges, payload, dumped.class_name))
                    return;

                for (size_t di { 0 }; di < depth; ++di)
                    std::cout << "|  ";
                std::cout << sc::ir::IRGraph::node_name(payload) << " ";
                sc::ir::describe::write_location(std::cout, loc);
                sc::ir::describe::write_details(describe, std::cout, payload, loc, i);
                write_frame(g, payload, i, current);
                std::cout << "\n";

                if (const auto fnd = owned.find({ &g, *i }); fnd != owned.end()) {
                    std::string indent;
                    for (size_t di { 0 }; di <= depth; ++di)
                        indent += "|  ";
                    for (const auto* d : fnd->second)
                        print_diagnostic(*d, indent);
                }
            },
            sc::ir::IRGraph::Default {}, sc::ir::IRGraph::Default {},
            [&](const sc::ir::IRGraph::Edges&, const sc::ir::IRGraph::Variant&, const sc::ir::SourceLocation&,
                sc::ir::NodeIndex i, size_t) {
                if (cursor)
                    (void)cursor->exit(i);
            });
    }
    // Each function's regions, the body first, each instruction on its own line. A method or interpreted function that
    // can't be emitted, e.g., one using something not lowered yet, is marked.
    if (show_lir) {
        std::cout << "\n\n\nLIR DUMP \n\n\n";
        for (const auto& dumped : classes) {
            const auto& g = *dumped.graph;
            const auto& f = frames.at(&g);
            const auto& lowered = f.lowered;
            std::cout << (dumped.class_name ? std::string { symbol_table(*dumped.class_name) } : "Interpreted") << "\n";
            for (std::uint32_t fi { 0 }; fi < lowered.functions.size(); ++fi) {
                const sc::backend::lir::FunctionIndex index { fi };
                const auto& function = lowered.functions[index];
                const auto& frame = f.layout.frames[function.frame];
                const sc::ir::NodeIndex owner { *frame.owner };
                std::cout << "|  function " << fi << " ";
                sc::ir::describe::write_location(std::cout, g.location(owner));
                if (const auto method = g.as_a<sc::ir::SCMethod_I>(owner))
                    std::cout << ": method " << symbol_table(*g.payload(*method).selector);
                const auto& roots = lowered.roots;
                if (!frame.parent && std::find(roots.begin(), roots.end(), index) == roots.end())
                    std::cout << " (not emitted)";
                std::cout << "\n";
                for (std::uint32_t ri { 0 }; ri < function.sub_regions.size(); ++ri) {
                    const auto& region = function.sub_regions[sc::backend::lir::FunctionSubRegionIndex { ri }];
                    std::cout << "|  |  region " << ri << "\n";
                    for (std::uint32_t k { *region.begin }; k < *region.begin + region.size; ++k) {
                        std::cout << "|  |  |  ";
                        sc::backend::write_instruction(std::cout, function,
                                                       function.instructions[sc::backend::lir::InstructionIndex { k }],
                                                       g, symbol_table, ctx.constant_table);
                        std::cout << "\n";
                    }
                }
            }
        }
    }
    // Each emitted function's tables and bytecode, see write_emitted. Functions of a method or interpreted function
    // that can't be emitted are left out.
    if (show_bytecode) {
        std::cout << "\n\n\nBYTECODE DUMP \n\n\n";
        for (const auto& dumped : classes) {
            const auto& g = *dumped.graph;
            const auto& f = frames.at(&g);
            std::cout << (dumped.class_name ? std::string { symbol_table(*dumped.class_name) } : "Interpreted") << "\n";
            std::optional<sc::backend::lir::FunctionIndex> root;
            for (std::uint32_t fi { 0 }; fi < f.lowered.functions.size(); ++fi) {
                const sc::backend::lir::FunctionIndex index { fi };
                const auto& frame = f.layout.frames[f.lowered.functions[index].frame];
                if (!frame.parent)
                    root = index;
                const auto& roots = f.emitted.roots;
                if (std::find(roots.begin(), roots.end(), *root) == roots.end())
                    continue;
                const sc::ir::NodeIndex owner { *frame.owner };
                std::cout << "|  function " << fi << " ";
                sc::ir::describe::write_location(std::cout, g.location(owner));
                if (const auto method = g.as_a<sc::ir::SCMethod_I>(owner))
                    std::cout << ": method " << symbol_table(*g.payload(*method).selector);
                std::cout << "\n";
                sc::backend::write_emitted(std::cout, f.emitted.functions[index], "|  |  ", symbol_table,
                                           ctx.constant_table);
            }
        }
    }
#endif

    std::cout << "Successfully compiled " << ctx.class_declarations.num_not_poisoned() / 2 << " classes." << std::endl;
#if DIAG_LOGGING
    if (show_times)
        write_times(std::cout, times, total);
#endif
#if TIME_LOGGING

    const auto finish_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(finish_time - start_time);
    const auto seconds = static_cast<double>(time_taken.count()) / 1000.0;

    std::cout << "TIME TAKEN = " << seconds;
#endif

    return 0;
}
