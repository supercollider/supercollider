#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_compiler/inspect.hpp"
#include "sc_compiler/details/backend.hpp"
#include "sc_compiler/details/class_library_internals.hpp"

#include "boost/asio/post.hpp"
#include "boost/asio/thread_pool.hpp"
#include "sc_source/normalise_source.hpp"
#include "sc_source/text_info.hpp"
#include "sc_source/text_location.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/details/indexes_typed.hpp"
#include "sc_parser/sc_parser.hpp"
#include "sc_sema/details/class_library_sources.hpp"
#include "sc_sema/details/intrinsic_constraint.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <optional>
#include <sstream>
#include <system_error>
#include <variant>
#include <vector>

namespace sc::compiler {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Helpers
////////////////////////////////////////////////////////////////////////////////

using Clock = std::chrono::steady_clock;

std::chrono::duration<double> seconds_since(Clock::time_point start) { return Clock::now() - start; }

std::optional<std::string> read_file(const std::filesystem::path& path) {
    std::ifstream file_stream { path };
    if (!file_stream.is_open())
        return std::nullopt;
    std::stringstream ss {};
    ss << file_stream.rdbuf();
    return std::move(ss).str();
}

std::shared_ptr<sc::src::TextInfo> make_class_file_text(std::string source, const std::filesystem::path& path) {
    return std::shared_ptr<sc::src::TextInfo>(new sc::src::TextInfo {
        sc::src::NormalisedSource { std::move(source) }, //
        sc::src::FileCodeLocation {}, //
        path, //
        sc::src::TextKind::ClassFile //
    });
}

// Set if it parsed as a class file.
std::optional<ir::SourceEntry> parse_class_file(const ClassFileSource& source,
                                                std::vector<diag::Diagnostic>& diagnostics, std::ostream& log) {
    const auto& path = source.text->file_path;
    auto [ast_graph, accepted] = sc::ast::parse(source.text);
    if (!accepted)
        log << "Failed to parse " << path.value_or(std::filesystem::path {})
            << " diagnostic count = " << (ast_graph ? ast_graph->diagnostics().size() : 0) << "\n";
    if (!ast_graph)
        return {};

    for (const auto& d : ast_graph->diagnostics())
        diagnostics.push_back(d);

    const auto roots = ast_graph->root();
    if (!roots) {
        log << "No roots  " << path.value_or(std::filesystem::path {}) << "\n";
        return {};
    }
    if (!std::get_if<sc::ast::ClassOrExtensionListIndex>(&*roots)) {
        log << "Parser returned an scd file, not a class file.\n";
        return {};
    }

    const auto class_list_index = std::get<sc::ast::ClassOrExtensionListIndex>(*roots);
    return ir::SourceEntry { std::move(ast_graph), class_list_index, source.module };
}

// Per class file, by the same index as the library's sources.
struct ParsedClassFiles {
    ir::ClassLibrarySources sources {};
    util::IndexedVector<ir::SourceEntryIndex, std::vector<diag::Diagnostic>> diagnostics {};
    util::IndexedVector<ir::SourceEntryIndex, std::ostringstream> logs {};

    explicit ParsedClassFiles(std::size_t size) {
        sources.resize(size);
        diagnostics.resize(size);
        logs.resize(size);
    }

    void parse(ir::SourceEntryIndex i, const ClassFileSource& source) {
        // Null if the file couldn't be read, which is already in the log.
        if (!source.text)
            return;
        sources[i] = parse_class_file(source, diagnostics[i], logs[i]);
    }
};

// The VM's intrinsic classes, each once, the only constraints, as only the VM relies on a class's layout.
ir::IntrinsicConstraints intrinsic_constraints(ir::SymbolTable& symbols, const VMDefinitions& vm) {
    ir::IntrinsicConstraints constraints {};
    for (const auto& c : vm.classes) {
        const auto name = symbols.get_class_name(c.name);
        ir::IntrinsicConstraint constraint {
            name, ir::OptionalSymSCClassName {}, std::vector<ir::SymSCIdentifier> {}, {}
        };
        if (c.superclass)
            constraint.super_class = ir::OptionalSymSCClassName { symbols.get_class_name(*c.superclass) };
        if (c.slot_type)
            constraint.slot_def = symbols.get_identifier(*c.slot_type);
        for (const auto& v : c.instance_variables)
            constraint.member_names->push_back(symbols.get_identifier(v));
        const auto [_, inserted] = constraints.insert({ name, std::move(constraint) });
        assert(inserted && "the VM defines an intrinsic class twice");
        (void)inserted;
    }
    return constraints;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class files
////////////////////////////////////////////////////////////////////////////////

ClassFiles find_class_files(const util::IndexedVector<ir::ModuleIndex, std::filesystem::path>& module_paths) {
    ClassFiles files;
    for (const auto module_index : module_paths.indices()) {
        const auto& path = module_paths[module_index];
        std::error_code ec;
        if (std::filesystem::is_directory(path, ec)) {
            // Directory iteration order depends on the filesystem, sort so that dumps are the same everywhere.
            std::vector<std::filesystem::path> found;
            std::filesystem::recursive_directory_iterator it { path, ec }, end;
            for (; !ec && it != end; it.increment(ec))
                if (it->is_regular_file(ec) && it->path().extension() == ".sc"
                    && it->path().filename().string()[0] != '.')
                    found.push_back(it->path());
            std::sort(found.begin(), found.end());
            for (auto& file : found)
                (void)files.push_back({ std::move(file), module_index });
        } else if (std::filesystem::is_regular_file(path) && path.extension() == ".sc") {
            (void)files.push_back({ path, module_index });
        }
    }
    return files;
}

////////////////////////////////////////////////////////////////////////////////
// Class library
////////////////////////////////////////////////////////////////////////////////

ClassLibraryInternals::ClassLibraryInternals(ir::ClassLibrarySources class_sources):
    sources(std::move(class_sources)),
    abstract_object(symbols.get_class_name("AbstractObjectExperimental")),
    ctx(ir::make_context(symbols, abstract_object, modules, sources)),
    compiler(*ctx) {}

ClassLibraryInternals::~ClassLibraryInternals() = default;

ClassLibrary::ClassLibrary(ir::ClassLibrarySources class_sources):
    internals(std::make_unique<ClassLibraryInternals>(std::move(class_sources))) {}

ClassLibrary::~ClassLibrary() = default;

std::string_view ClassLibrary::symbol_text(ir::Symbol symbol) const { return internals->symbols(symbol); }

bool ClassLibrary::is_meta(ir::SymSCClassName name) const { return internals->symbols.is_meta(name); }

const ir::ConstantTable& ClassLibrary::constants() const { return ir::constant_table(*internals->ctx); }

std::uint32_t ClassLibrary::num_class_variables() const { return internals->backend->members.num_class_variables(); }

namespace {

std::unique_ptr<ClassLibrary> compile_parsed(ParsedClassFiles parsed, std::size_t num_threads, const VMDefinitions& vm,
                                             ClassLibraryTimes& times) {
    auto library = std::make_unique<ClassLibrary>(std::move(parsed.sources));
    for (const auto& log : parsed.logs.raw())
        library->parse_log += log.str();
    for (auto& file : parsed.diagnostics.raw())
        for (auto& d : file)
            library->parse_diagnostics.push_back(std::move(d));

    for (const auto& p : vm.primitives)
        ir::add_primitive_signature(*library->internals->ctx, library->internals->symbols.get_primitive(p.name),
                                    p.num_arguments, p.variadic_arguments, p.keyword_arguments);

    const auto sema_start = Clock::now();
    library->diagnostics = library->internals->compiler.compile_class_library(
        intrinsic_constraints(library->internals->symbols, vm), num_threads);
    times.sema = seconds_since(sema_start);

    const auto member_table_start = Clock::now();
    library->internals->backend = std::make_unique<Backend>(library->internals->compiler, library->internals->symbols,
                                                            ir::constant_table(*library->internals->ctx));
    times.member_table = seconds_since(member_table_start);
    return library;
}
}

namespace {

using ClassFileSources = util::IndexedVector<ir::SourceEntryIndex, ClassFileSource>;

// Per source, what went wrong before parsing, e.g., reading, goes first in its log.
std::unique_ptr<ClassLibrary> compile_sources(const ClassFileSources& sources, std::size_t num_threads,
                                              const VMDefinitions& vm, ClassLibraryTimes& times,
                                              const util::IndexedVector<ir::SourceEntryIndex, std::string>* before) {
    const auto parse_start = Clock::now();
    ParsedClassFiles parsed { sources.size() };
    if (before)
        for (const auto i : sources.indices())
            parsed.logs[i] << (*before)[i];
    {
        boost::asio::thread_pool pool { num_threads };
        for (const auto i : sources.indices())
            boost::asio::post(pool, [&, i] {
                // An exception leaving a worker thread would terminate the process.
                try {
                    parsed.parse(i, sources[i]);
                } catch (const std::exception& e) { parsed.logs[i] << "Failed to parse: " << e.what() << "\n"; }
            });
        pool.join();
    }
    times.parse = seconds_since(parse_start);
    return compile_parsed(std::move(parsed), num_threads, vm, times);
}
}

std::unique_ptr<ClassLibrary> compile_class_library(const ClassFileSources& sources, std::size_t num_threads,
                                                    const VMDefinitions& vm, ClassLibraryTimes* times) {
    ClassLibraryTimes ignored;
    return compile_sources(sources, num_threads, vm, times ? *times : ignored, nullptr);
}

std::unique_ptr<ClassLibrary> compile_class_library(const ClassFiles& files, std::size_t num_threads,
                                                    const VMDefinitions& vm, ClassLibraryTimes* times) {
    const auto read_start = Clock::now();
    util::IndexedVector<ir::SourceEntryIndex, std::shared_ptr<const src::TextInfo>> texts;
    texts.resize(files.size());
    util::IndexedVector<ir::SourceEntryIndex, std::string> read_errors;
    read_errors.resize(files.size());
    {
        boost::asio::thread_pool pool { num_threads };
        for (const auto file : files.indices())
            boost::asio::post(pool, [&, file] {
                const auto& [path, module] = files[file];
                try {
                    if (auto text = read_file(path))
                        texts[file] = make_class_file_text(std::move(*text), path);
                    else
                        read_errors[file] = (std::ostringstream {} << "Could not open file " << path << "\n").str();
                } catch (const std::exception& e) {
                    read_errors[file] =
                        (std::ostringstream {} << "Failed to read or parse " << path << ": " << e.what() << "\n").str();
                }
            });
        pool.join();
    }
    const auto read_time = seconds_since(read_start);

    ClassFileSources sources;
    for (const auto file : files.indices())
        (void)sources.push_back({ std::move(texts[file]), files[file].module });

    ClassLibraryTimes ignored;
    auto& t = times ? *times : ignored;
    auto library = compile_sources(sources, num_threads, vm, t, &read_errors);
    t.parse += read_time;
    return library;
}

////////////////////////////////////////////////////////////////////////////////
// Interpreted code
////////////////////////////////////////////////////////////////////////////////

InterpretedParsed parse_interpreted(std::shared_ptr<const src::TextInfo> text) {
    auto [ast_graph, _] = sc::ast::parse(std::move(text));
    if (!ast_graph)
        return {};
    InterpretedParsed parsed { nullptr, ast_graph->diagnostics() };
    const auto root = ast_graph->root();
    if (root && !std::holds_alternative<sc::ast::ClassOrExtensionListIndex>(*root))
        parsed.ast = std::move(ast_graph);
    return parsed;
}

InterpretedIR compile_interpreted_ir(ClassLibrary& library, InterpretedParsed&& parsed,
                                     std::optional<ast::LanguageVersion> given, ast::LanguageVersion fallback) {
    if (!parsed.ast)
        return {};
    auto r = library.internals->compiler.compile_interpreted(std::move(parsed.ast), given, fallback);
    return { std::move(r.ir), std::move(r.diagnostics) };
}

CompiledGraph compile_interpreted(ClassLibrary& library, const InterpretedIR& ir) {
    const auto& graph = *ir.graph;
    auto framed = layout_frames(library, graph, std::get<ir::SCInterpreted_I>(*graph.root()));
    return finish(library, emit(library, lower(library, std::move(framed))));
}

////////////////////////////////////////////////////////////////////////////////
// Backend
////////////////////////////////////////////////////////////////////////////////

Backend::Backend(const ir::IRCompiler& compiler, ir::SymbolTable& symbols, const ir::ConstantTable& constants):
    members(compiler, symbols),
    selectors(symbols),
    special_symbols(symbols),
    inlining { special_symbols, constants } {
    // Optional, a library without it skips its poisoned methods.
    if (compiler.class_graph(symbols.get_class_name(poisoned_method_error_name)))
        poisoned_method_error = ir::OptionalSymSCClassName { symbols.get_class_name(poisoned_method_error_name) };
}

FramedGraph layout_frames(const ClassLibrary& library, const ir::IRGraph& g, CompilationUnit root) {
    return { &g, root, backend::layout_frames(g, library.internals->backend->inlining, root) };
}

LoweredGraph lower(ClassLibrary& library, FramedGraph&& framed) {
    const auto& ctx = *library.internals->ctx;
    const auto& shared = *library.internals->backend;
    const backend::LoweringContext lowering {
        *framed.graph,    framed.frames,   shared.members,       ir::constant_table(ctx), library.internals->symbols,
        shared.selectors, shared.inlining, ir::array_class(ctx), ir::event_class(ctx),    shared.poisoned_method_error
    };
    auto lowered = backend::lower(lowering, framed.root);
    return { framed.graph, framed.root, std::move(framed.frames), std::move(lowered) };
}

EmittedGraph emit(const ClassLibrary& library, LoweredGraph&& lowered) {
    const auto& ctx = *library.internals->ctx;
    const auto& shared = *library.internals->backend;
    const auto& g = *lowered.graph;
    const auto class_node = g.as_a<ir::SCClass_I>(lowered.root);
    const bool interpreter_members = !class_node || g.payload(*class_node).name == ir::interpreter_class(ctx);
    const backend::EmitContext emitting {
        g, lowered.frames, shared.members, ir::constant_table(ctx), shared.special_symbols, interpreter_members
    };
    auto emitted = backend::emit(emitting, lowered.lowered);
    return { lowered.graph, lowered.root, std::move(lowered.frames), std::move(lowered.lowered), std::move(emitted) };
}

util::IndexedVector<ClassGraphIndex, CompiledGraph> compile_class_graphs(ClassLibrary& library,
                                                                         std::size_t num_threads) {
    util::IndexedVector<ClassGraphIndex, const ir::IRGraph*> graphs;
    library.internals->compiler.depth_first_do([&](const ir::IRGraph& g) { (void)graphs.push_back(&g); });
    util::IndexedVector<ClassGraphIndex, std::optional<CompiledGraph>> results;
    results.resize(graphs.size());
    {
        boost::asio::thread_pool pool { num_threads };
        for (const auto i : graphs.indices())
            boost::asio::post(pool, [&, i] {
                const auto& g = *graphs[i];
                const auto root = std::get<ir::SCClass_I>(*g.root());
                results[i] = finish(library, emit(library, lower(library, layout_frames(library, g, root))));
            });
        pool.join();
    }
    util::IndexedVector<ClassGraphIndex, CompiledGraph> compiled;
    compiled.raw().reserve(results.size());
    for (auto& r : results.raw())
        (void)compiled.push_back(std::move(*r));
    return compiled;
}
}
