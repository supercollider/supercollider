#pragma once

#include "sc_backend/emit.hpp"
#include "sc_backend/frames.hpp"
#include "sc_backend/inlining.hpp"
#include "sc_backend/lir.hpp"
#include "sc_backend/lower.hpp"
#include "sc_backend/members.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_source/text_info.hpp"
#include "sc_sema/compiler.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/module.hpp"
#include "sc_sema/source_location.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_util/indexed_vector.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace sc::compiler {

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

struct ClassFile;
struct ClassFileSource;
struct ClassLibraryTimes;
struct ClassLibrary;
struct Backend;
struct FramedGraph;
struct LoweredGraph;
struct EmittedGraph;

using ClassFiles = util::IndexedVector<ir::SourceEntryIndex, ClassFile>;

// Each path is a module, a directory's .sc files or one .sc file.
[[nodiscard]] ClassFiles
find_class_files(const util::IndexedVector<ir::ModuleIndex, std::filesystem::path>& module_paths);

[[nodiscard]] std::unique_ptr<ClassLibrary> compile_class_library(const ClassFiles& files, std::size_t num_threads,
                                                                  ClassLibraryTimes& times);
// Text that isn't in files, e.g., an editor's.
[[nodiscard]] std::unique_ptr<ClassLibrary>
compile_class_library(const util::IndexedVector<ir::SourceEntryIndex, ClassFileSource>& sources,
                      std::size_t num_threads, ClassLibraryTimes& times);

using CompilationUnit = ir::join<ir::SCClass_I, ir::SCInterpreted_I>;

// The backend's stages, each moving the last's result into its own. Lowering adds symbols, so the library isn't const.
[[nodiscard]] FramedGraph layout_frames(const ClassLibrary& library, const ir::IRGraph& g, CompilationUnit root);
[[nodiscard]] LoweredGraph lower(ClassLibrary& library, FramedGraph&& framed);
[[nodiscard]] EmittedGraph emit(const ClassLibrary& library, LoweredGraph&& lowered);

////////////////////////////////////////////////////////////////////////////////
// Class files
////////////////////////////////////////////////////////////////////////////////

struct ClassFile {
    std::filesystem::path path;
    ir::ModuleIndex module;
};

struct ClassFileSource {
    // Says it is a class file.
    std::shared_ptr<const src::TextInfo> text;
    ir::ModuleIndex module;
};

////////////////////////////////////////////////////////////////////////////////
// Class library times
////////////////////////////////////////////////////////////////////////////////

// In seconds. Parsing includes reading the files.
struct ClassLibraryTimes {
    double parse = 0;
    double sema = 0;
    double member_table = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Backend
////////////////////////////////////////////////////////////////////////////////

// What every graph's backend stages share, built once the class library is compiled.
struct Backend {
    Backend(const ir::IRCompiler& compiler, ir::SymbolTable& symbols, const ir::ConstantTable& constants);
    Backend(const Backend&) = delete;
    Backend& operator=(const Backend&) = delete;

    backend::MemberTable members;
    backend::LoweringSymbols selectors;
    backend::SpecialSymbols special_symbols;
    backend::InliningContext inlining;
};

////////////////////////////////////////////////////////////////////////////////
// Class library
////////////////////////////////////////////////////////////////////////////////

// Its members refer to each other, so it doesn't move.
struct ClassLibrary {
    explicit ClassLibrary(ir::ClassLibrarySources class_sources);
    ClassLibrary(const ClassLibrary&) = delete;
    ClassLibrary& operator=(const ClassLibrary&) = delete;

    ir::SymbolTable symbols {};
    ir::ModuleTable modules {};
    // By the index of the class file each was parsed from.
    ir::ClassLibrarySources sources;
    ir::SymSCClassName abstract_object;
    ir::Context ctx;
    ir::IRCompiler compiler;
    // Per file, in order: files that couldn't be opened or parsed.
    std::string parse_log {};
    std::vector<diag::Diagnostic> parse_diagnostics {};
    std::vector<diag::Diagnostic> diagnostics {};
    // Set by compile_class_library.
    std::optional<Backend> backend {};
};

////////////////////////////////////////////////////////////////////////////////
// Backend stages
////////////////////////////////////////////////////////////////////////////////

// Each stage keeps the earlier stages' results. Diagnostics stay in the result of the stage that reported them.
struct FramedGraph {
    const ir::IRGraph* graph;
    CompilationUnit root;
    backend::Frames frames;
};

struct LoweredGraph {
    const ir::IRGraph* graph;
    CompilationUnit root;
    backend::Frames frames;
    backend::lir::LoweredGraph lowered;
};

struct EmittedGraph {
    const ir::IRGraph* graph;
    CompilationUnit root;
    backend::Frames frames;
    backend::lir::LoweredGraph lowered;
    backend::EmittedGraph emitted;
};
}
