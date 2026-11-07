#pragma once

/// @file
/// @brief For tools: the stages one by one, and compiling from text, rather than files. See sc_compiler.hpp.

#include "sc_backend/emit.hpp"
#include "sc_backend/details/frames.hpp"
#include "sc_backend/details/lir.hpp"

#include "sc_compiler/sc_compiler.hpp"

#include <cstddef>
#include <iosfwd>

namespace sc::compiler {

struct FramedGraph;
struct LoweredGraph;
struct EmittedGraph;

/// @brief A class or interpreted code: what the backend compiles.
using CompilationUnit = ir::join<ir::SCClass_I, ir::SCInterpreted_I>;

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

/// @brief Finds the class files in each module.
/// Each path is a module, a directory's .sc files or one .sc file.
[[nodiscard]] ClassFiles
find_class_files(const util::IndexedVector<ir::ModuleIndex, std::filesystem::path>& module_paths);

/// @brief The text of a class file, which is not in a file.
struct ClassFileSource {
    /// The text, which says it is a class file.
    std::shared_ptr<const src::TextInfo> text;
    ir::ModuleIndex module;
};

/// @brief Like compile_class_library() for files, but for text that isn't in files, e.g., an editor's.
[[nodiscard]] std::unique_ptr<ClassLibrary>
compile_class_library(const util::IndexedVector<ir::SourceEntryIndex, ClassFileSource>& sources,
                      std::size_t num_threads, const VMDefinitions& vm, ClassLibraryTimes* times = nullptr);

/// @name Backend stages
/// The backend's stages, each moving the last's result into its own, which compile_class_graphs() runs per graph.
/// @{

[[nodiscard]] FramedGraph layout_frames(const ClassLibrary& library, const ir::IRGraph& g, CompilationUnit root);
[[nodiscard]] LoweredGraph lower(ClassLibrary& library, FramedGraph&& framed);
[[nodiscard]] EmittedGraph emit(const ClassLibrary& library, LoweredGraph&& lowered);
[[nodiscard]] CompiledGraph finish(const ClassLibrary& library, EmittedGraph&& emitted);
/// @}

void write_compiled(std::ostream& out, const CompiledGraph& compiled, const ClassLibrary& library);

////////////////////////////////////////////////////////////////////////////////
// Backend stages
////////////////////////////////////////////////////////////////////////////////

/// @brief The result of layout_frames().
///
/// Each stage keeps the earlier stages' results. Diagnostics stay in the result of the stage that reported them.
/// The graph is not owned: the library's, or an InterpretedIR's (which holds it in a shared_ptr), must outlive every
/// stage made from it.
struct FramedGraph {
    const ir::IRGraph* graph;
    CompilationUnit root;
    backend::Frames frames;
};

/// @brief The result of lower().
struct LoweredGraph {
    const ir::IRGraph* graph;
    CompilationUnit root;
    backend::Frames frames;
    backend::lir::LoweredGraph lowered;
};

/// @brief The result of emit().
struct EmittedGraph {
    const ir::IRGraph* graph;
    CompilationUnit root;
    backend::Frames frames;
    backend::lir::LoweredGraph lowered;
    backend::EmittedGraph emitted;
};
}
