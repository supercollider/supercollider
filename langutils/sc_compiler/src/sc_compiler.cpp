#include "sc_compiler/sc_compiler.hpp"

#include "boost/asio/post.hpp"
#include "boost/asio/thread_pool.hpp"
#include "sc_source/normalise_source.hpp"
#include "sc_source/text_info.hpp"
#include "sc_source/text_location.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/sc_parser.hpp"
#include "sc_sema/intrinsic_constraint.hpp"

#include <cassert>
#include <chrono>
#include <fstream>
#include <optional>
#include <sstream>
#include <system_error>
#include <variant>

namespace sc::compiler {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Helpers
////////////////////////////////////////////////////////////////////////////////

using Clock = std::chrono::steady_clock;

double seconds_since(Clock::time_point start) { return std::chrono::duration<double>(Clock::now() - start).count(); }

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
        true //
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
            for (const auto& entry : std::filesystem::recursive_directory_iterator { path })
                if (entry.is_regular_file() && entry.path().extension() == ".sc")
                    (void)files.push_back({ entry.path(), module_index });
        } else if (std::filesystem::is_regular_file(path) && path.extension() == ".sc") {
            (void)files.push_back({ path, module_index });
        }
    }
    return files;
}

////////////////////////////////////////////////////////////////////////////////
// Class library
////////////////////////////////////////////////////////////////////////////////

ClassLibrary::ClassLibrary(ir::ClassLibrarySources class_sources):
    sources(std::move(class_sources)),
    abstract_object(symbols.get_class_name("AbstractObjectExperimental")),
    ctx(symbols, ir::symbol_table_and_common_classes(symbols, abstract_object), modules, sources),
    compiler(ctx) {}

namespace {

std::unique_ptr<ClassLibrary> compile_parsed(ParsedClassFiles parsed, std::size_t num_threads, ClassLibraryTimes& times,
                                             const VMDefinitions& vm) {
    auto library = std::make_unique<ClassLibrary>(std::move(parsed.sources));
    for (const auto& log : parsed.logs.raw())
        library->parse_log += log.str();
    for (auto& file : parsed.diagnostics.raw())
        for (auto& d : file)
            library->parse_diagnostics.push_back(std::move(d));

    for (const auto& p : vm.primitives)
        library->ctx.primitive_signatures.insert(
            { library->symbols.get_primitive(p.name),
              ir::PrimitiveSignature { p.num_arguments, p.variadic_arguments, p.keyword_arguments } });

    const auto sema_start = Clock::now();
    library->diagnostics =
        library->compiler.compile_class_library(intrinsic_constraints(library->symbols, vm), num_threads);
    times.sema = seconds_since(sema_start);

    const auto member_table_start = Clock::now();
    library->backend.emplace(library->compiler, library->symbols, library->ctx.constant_table);
    times.member_table = seconds_since(member_table_start);
    return library;
}
}

std::unique_ptr<ClassLibrary> compile_class_library(const ClassFiles& files, std::size_t num_threads,
                                                    ClassLibraryTimes& times, const VMDefinitions& vm) {
    const auto parse_start = Clock::now();
    ParsedClassFiles parsed { files.size() };
    {
        boost::asio::thread_pool pool { num_threads };
        for (const auto file : files.indices())
            boost::asio::post(pool, [&, file] {
                const auto& [path, module] = files[file];
                if (auto text = read_file(path))
                    parsed.parse(file, { make_class_file_text(std::move(*text), path), module });
                else
                    parsed.logs[file] << "Could not open file" << path << "\n";
            });
        pool.join();
    }
    times.parse = seconds_since(parse_start);
    return compile_parsed(std::move(parsed), num_threads, times, vm);
}

std::unique_ptr<ClassLibrary>
compile_class_library(const util::IndexedVector<ir::SourceEntryIndex, ClassFileSource>& sources,
                      std::size_t num_threads, ClassLibraryTimes& times, const VMDefinitions& vm) {
    const auto parse_start = Clock::now();
    ParsedClassFiles parsed { sources.size() };
    {
        boost::asio::thread_pool pool { num_threads };
        for (const auto i : sources.indices())
            boost::asio::post(pool, [&, i] { parsed.parse(i, sources[i]); });
        pool.join();
    }
    times.parse = seconds_since(parse_start);
    return compile_parsed(std::move(parsed), num_threads, times, vm);
}

////////////////////////////////////////////////////////////////////////////////
// Backend
////////////////////////////////////////////////////////////////////////////////

Backend::Backend(const ir::IRCompiler& compiler, ir::SymbolTable& symbols, const ir::ConstantTable& constants):
    members(compiler, symbols),
    selectors(symbols),
    special_symbols(symbols),
    inlining { special_symbols, constants } {}

FramedGraph layout_frames(const ClassLibrary& library, const ir::IRGraph& g, CompilationUnit root) {
    return { &g, root, backend::layout_frames(g, library.backend->inlining, root) };
}

LoweredGraph lower(ClassLibrary& library, FramedGraph&& framed) {
    const auto& ctx = library.ctx;
    const auto& shared = *library.backend;
    const backend::LoweringContext lowering {
        *framed.graph,    framed.frames,   shared.members,           ctx.constant_table,      library.symbols,
        shared.selectors, shared.inlining, ctx.common_classes.array, ctx.common_classes.event
    };
    auto lowered = backend::lower(lowering, framed.root);
    return { framed.graph, framed.root, std::move(framed.frames), std::move(lowered) };
}

EmittedGraph emit(const ClassLibrary& library, LoweredGraph&& lowered) {
    const auto& ctx = library.ctx;
    const auto& shared = *library.backend;
    const auto& g = *lowered.graph;
    const auto class_node = g.as_a<ir::SCClass_I>(lowered.root);
    const bool interpreter_members = !class_node || g.payload(*class_node).name == ctx.common_symbols.interpreter;
    const backend::EmitContext emitting {
        g, lowered.frames, shared.members, ctx.constant_table, shared.special_symbols, interpreter_members
    };
    auto emitted = backend::emit(emitting, lowered.lowered);
    return { lowered.graph, lowered.root, std::move(lowered.frames), std::move(lowered.lowered), std::move(emitted) };
}
}
