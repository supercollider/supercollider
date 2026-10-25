
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

#include "boost/asio/post.hpp"
#include "boost/asio/thread_pool.hpp"
#include "describe/describe.hpp"
#include "sc_backend/emit.hpp"
#include "sc_backend/frames.hpp"
#include "sc_backend/lower.hpp"
#include "sc_backend/members.hpp"
#include "sc_compiler/sc_compiler.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_source/normalise_source.hpp"
#include "sc_source/text_info.hpp"
#include "sc_source/text_location.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/sc_grammar_shared.hpp"
#include "sc_parser/sc_parser.hpp"
#include "sc_sema/compiler.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"


#define DIAG_LOGGING true

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

// How long each stage took, in seconds, see --time. The backend's are summed over its graphs.
struct StageTimes {
    double find_files = 0;
    sc::compiler::ClassLibraryTimes library {};
    double interpreted_parse = 0;
    double interpreted_sema = 0;
    double frames = 0;
    double lower = 0;
    double emit = 0;
};

using Clock = std::chrono::steady_clock;

double seconds_since(Clock::time_point start) { return std::chrono::duration<double>(Clock::now() - start).count(); }

using InterpretedFileIndex = sc::util::StrongIndex<std::uint32_t, struct InterpretedFileIndex__>;

// An scd file, parsed as interpreted code. Null if it can't be read, or is a class file.
std::shared_ptr<const sc::ast::ASTGraph> parse_interpreted(const std::filesystem::path& path,
                                                           std::vector<sc::diag::Diagnostic>& diagnostics) {
    std::ifstream file_stream { path };
    if (!file_stream.is_open())
        return nullptr;
    std::stringstream ss {};
    ss << file_stream.rdbuf();
    auto text_info = std::shared_ptr<sc::src::TextInfo>(new sc::src::TextInfo {
        sc::src::NormalisedSource { std::move(ss).str() }, //
        sc::src::FileCodeLocation {}, //
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

void write_times(std::ostream& out, const StageTimes& t, double total) {
    const auto row = [&](const char* name, double seconds) {
        out << "  " << name << std::string(20 - std::string_view { name }.size(), ' ') << std::fixed
            << std::setprecision(6) << seconds << " s\n";
    };
    out << "Stage times:\n";
    row("find files", t.find_files);
    row("parse", t.library.parse);
    row("sema", t.library.sema);
    row("interpreted parse", t.interpreted_parse);
    row("interpreted sema", t.interpreted_sema);
    row("member table", t.library.member_table);
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
    sc::util::IndexedVector<sc::ir::ModuleIndex, std::filesystem::path> module_paths;
    // Compiled as interpreted code after the class library, in path order.
    sc::util::IndexedVector<InterpretedFileIndex, std::filesystem::path> scd_files;
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
        } else if (arg.extension() == ".scd" && std::filesystem::is_regular_file(arg)) {
            scd_files.push_back(arg);
        } else {
            module_paths.push_back(arg);
        }
    }
    const auto total_start = Clock::now();
    StageTimes times;

    const auto find_start = Clock::now();
    const auto files = sc::compiler::find_class_files(module_paths);
    times.find_files = seconds_since(find_start);

    const auto library = sc::compiler::compile_class_library(files, num_threads, times.library);
    std::cout << library->parse_log;
    auto& ctx = library->ctx;
    auto& comp = library->compiler;
    auto& symbol_table = library->symbols;
    auto diags = std::move(library->diagnostics);

    // Each file is parsed, then compiled, on several threads, each writing only its own entry, and gathered in order.
    std::sort(scd_files.raw().begin(), scd_files.raw().end());
    sc::util::IndexedVector<InterpretedFileIndex, std::vector<sc::diag::Diagnostic>> scd_diagnostics;
    scd_diagnostics.resize(scd_files.size());
    sc::util::IndexedVector<InterpretedFileIndex, std::shared_ptr<const sc::ast::ASTGraph>> scd_asts;
    scd_asts.resize(scd_files.size());
    sc::util::IndexedVector<InterpretedFileIndex, std::shared_ptr<sc::ir::IRGraph>> scd_graphs;
    scd_graphs.resize(scd_files.size());
    const auto interpreted_parse_start = Clock::now();
    {
        boost::asio::thread_pool pool { num_threads };
        for (const auto i : scd_files.indices())
            boost::asio::post(pool, [&, i] { scd_asts[i] = parse_interpreted(scd_files[i], scd_diagnostics[i]); });
        pool.join();
    }
    times.interpreted_parse = seconds_since(interpreted_parse_start);
    const auto interpreted_sema_start = Clock::now();
    {
        boost::asio::thread_pool pool { num_threads };
        for (const auto i : scd_files.indices())
            boost::asio::post(pool, [&, i] {
                if (!scd_asts[i])
                    return;
                auto r = comp.compile_interpreted(scd_asts[i], interpreted_version);
                for (auto& d : r.diagnostics)
                    scd_diagnostics[i].push_back(std::move(d));
                scd_graphs[i] = std::move(r.ir);
            });
        pool.join();
    }
    times.interpreted_sema = seconds_since(interpreted_sema_start);
    std::vector<std::shared_ptr<sc::ir::IRGraph>> interpreted_graphs;
    for (const auto i : scd_files.indices()) {
        for (auto& d : scd_diagnostics[i])
            diags.push_back(std::move(d));
        if (scd_graphs[i])
            interpreted_graphs.push_back(std::move(scd_graphs[i]));
    }

    // The backend's frames, lowered ir and bytecode, by graph, and its diagnostics, printed with the others. Each
    // frame's owner is written when the owner is, which for a qualifier is before its frame starts, so they are found
    // by owner. Each stage only runs if it, or a later one, is shown.
    using CompilationUnit = sc::compiler::CompilationUnit;
    struct DumpFrames {
        CompilationUnit root;
        sc::backend::Frames layout;
        std::unordered_map<sc::ir::NodeIndex, sc::backend::FrameIndex> by_owner {};
        sc::backend::lir::LoweredGraph lowered {};
        sc::backend::EmittedGraph emitted {};
    };
    const bool show_backend = show_frames || show_lir || show_bytecode;
    const auto& backend = library->backend;
    const auto run_backend = [&](const sc::ir::IRGraph& g, CompilationUnit root,
                                 std::vector<sc::diag::Diagnostic>& diagnostics) {
        const auto frames_start = Clock::now();
        auto framed = sc::compiler::layout_frames(*library, g, root);
        times.frames += seconds_since(frames_start);
        for (auto& d : framed.frames.diagnostics)
            diagnostics.push_back(std::move(d));
        framed.frames.diagnostics.clear();
        const auto finish = [&](DumpFrames f) {
            for (const auto fi : f.layout.frames.indices())
                f.by_owner.insert({ sc::ir::NodeIndex { *f.layout.frames[fi].owner }, fi });
            return f;
        };
        if (!show_lir && !show_bytecode)
            return finish({ root, std::move(framed.frames) });

        const auto lower_start = Clock::now();
        auto lowered = sc::compiler::lower(*library, std::move(framed));
        times.lower += seconds_since(lower_start);
        if (!show_bytecode)
            return finish({ root, std::move(lowered.frames), {}, std::move(lowered.lowered) });

        const auto emit_start = Clock::now();
        auto emitted = sc::compiler::emit(*library, std::move(lowered));
        times.emit += seconds_since(emit_start);
        for (auto& d : emitted.emitted.diagnostics)
            diagnostics.push_back(std::move(d));
        emitted.emitted.diagnostics.clear();
        return finish({ root, std::move(emitted.frames), {}, std::move(emitted.lowered), std::move(emitted.emitted) });
    };

    if (!dump) {
        std::size_t count = library->parse_diagnostics.size() + diags.size();
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
    for (auto& d : library->parse_diagnostics)
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
        sc::src::SourceCodeLocation begin;
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
        classes.push_back({ &g, *class_i, c.name, file ? file->string() : std::string {},
                            sc::ir::IRGraph::range(loc).begin, symbol_table.is_meta(c.name) });
    });
    for (const auto& g : interpreted_graphs) {
        const auto interpreted_i = std::get<sc::ir::SCInterpreted_I>(*g->root());
        const auto loc = g->location(interpreted_i);
        const auto& file = g->text_info(loc.source)->file_path;
        classes.push_back({ g.get(), interpreted_i, std::nullopt, file ? file->string() : std::string {},
                            sc::ir::IRGraph::range(loc).begin, false });
    }
    std::sort(classes.begin(), classes.end(), [](const DumpedClass& l, const DumpedClass& r) {
        return std::tie(l.file, l.begin, l.is_meta) < std::tie(r.file, r.begin, r.is_meta);
    });

    std::map<const sc::ir::IRGraph*, DumpFrames> frames;
    if (show_backend) {
        for (const auto& dumped : classes) {
            const auto& g = *dumped.graph;
            const auto root = [&]() -> CompilationUnit {
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
            const auto owned = fnd->second.by_owner.find(sc::ir::NodeIndex { *o });
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
            const auto location = backend->members.find(variable.owner, variable.name);
            if (!location) {
                std::cout << " [member not found]";
                return;
            }
            // A class variable as its position among its class's own, so it doesn't depend on the other classes.
            std::visit(sc::util::overload {
                           [](sc::backend::InstanceSlotIndex slot) { std::cout << " [instance slot " << *slot << "]"; },
                           [&](sc::backend::ClassVariableIndex index) {
                               std::cout << " [class variable "
                                         << *index - *backend->members.class_variable_base(variable.owner) << " of "
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
        std::shared_ptr<const sc::src::TextInfo> text;
        sc::src::SourceCodeRange range;
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
            method_owners.push_back(owner(*it));
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
            cursor.emplace(g, backend->inlining, fnd->second.layout);
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
            for (const auto index : lowered.functions.indices()) {
                const auto& function = lowered.functions[index];
                const auto& frame = f.layout.frames[function.frame];
                const sc::ir::NodeIndex owner { *frame.owner };
                std::cout << "|  function " << *index << " ";
                sc::ir::describe::write_location(std::cout, g.location(owner));
                if (const auto method = g.as_a<sc::ir::SCMethod_I>(owner))
                    std::cout << ": method " << symbol_table(*g.payload(*method).selector);
                if (!frame.parent && !function.complete)
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
    // Each class's accessors, then each emitted function's tables and bytecode, see write_emitted. Functions of a
    // method or interpreted function that can't be emitted, or in a hard coded loop method, are left out.
    if (show_bytecode) {
        std::cout << "\n\n\nBYTECODE DUMP \n\n\n";
        for (const auto& dumped : classes) {
            const auto& g = *dumped.graph;
            const auto& f = frames.at(&g);
            std::cout << (dumped.class_name ? std::string { symbol_table(*dumped.class_name) } : "Interpreted") << "\n";
            for (const auto& accessor : f.emitted.accessors) {
                std::cout << "|  accessor ";
                sc::ir::describe::write_location(std::cout, g.location(accessor.member));
                std::cout << ": method " << symbol_table(accessor.selector) << "\n";
                sc::backend::write_accessor(std::cout, accessor, "|  |  ", symbol_table, ctx.constant_table);
            }
            std::optional<sc::backend::lir::FunctionIndex> root;
            for (const auto index : f.lowered.functions.indices()) {
                const auto& frame = f.layout.frames[f.lowered.functions[index].frame];
                if (!frame.parent)
                    root = index;
                const auto& roots = f.emitted.roots;
                if (std::find(roots.begin(), roots.end(), *root) == roots.end()
                    || f.emitted.functions[index].thrown_away)
                    continue;
                const sc::ir::NodeIndex owner { *frame.owner };
                std::cout << "|  function " << *index << " ";
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

    return 0;
}
