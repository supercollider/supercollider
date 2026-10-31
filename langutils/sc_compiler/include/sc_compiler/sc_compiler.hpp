#pragma once

/// @file
/// @brief What sclang needs. See inspect.hpp for the stages one by one, and for compiling from text rather than files.

#include "sc_compiler/compiled.hpp"
#include "sc_compiler/vm_definitions.hpp"

#include "sc_source/text_info.hpp"

#include "sc_util/indexed_vector.hpp"
#include "sc_util/language_version.hpp"
#include "sc_util/strong_index.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace sc::diag {
class Diagnostic;
}
namespace sc::ir {
// Same as in sc_sema/details/class_library_sources.hpp, so only the users need SourceEntry.
struct SourceEntry;
using SourceEntryIndex = util::StrongIndex<std::uint32_t, struct SourceEntryIndex__>;
using ClassLibrarySources = util::IndexedVector<SourceEntryIndex, std::optional<SourceEntry>>;
}

namespace sc::ast {
struct ASTGraph;
}

namespace sc::compiler {

struct ClassLibrary;
struct ClassLibraryInternals;
struct InterpretedParsed;
struct InterpretedIR;
struct ClassLibraryTimes;

/// @brief A file of the class library.
struct ClassFile {
    std::filesystem::path path;
    ir::ModuleIndex module;
};
using ClassFiles = util::IndexedVector<ir::SourceEntryIndex, ClassFile>;


////////////////////////////////////////////////////////////////////////////////
// API Class library
////////////////////////////////////////////////////////////////////////////////

/// @brief Parses and compiles the class library from files, up to the IR of every class.
///
/// The result is the information that needs to be kept and passed to every compile invocation. The loader needs access
/// to it.
/// @param times If given, set to how long each stage took.
[[nodiscard]] std::unique_ptr<ClassLibrary> compile_class_library(const ClassFiles& files, std::size_t num_threads,
                                                                  const VMDefinitions& vm,
                                                                  ClassLibraryTimes* times = nullptr);

/// @brief A class graph's topological position: a superclass has a lower index than its subclasses.
using ClassGraphIndex = util::StrongIndex<std::uint32_t, struct ClassGraphIndex__>;

/// @brief Runs the backend's stages for every class of the library.
/// @return The compiled classes, a class graph for each class that is not poisoned.
[[nodiscard]] util::IndexedVector<ClassGraphIndex, CompiledGraph> compile_class_graphs(ClassLibrary& library,
                                                                                       std::size_t num_threads);


////////////////////////////////////////////////////////////////////////////////
// API Interpreted, requires class library
////////////////////////////////////////////////////////////////////////////////


/// @brief Parses interpreted code, e.g., an editor's text or an scd file, after the class library.
[[nodiscard]] InterpretedParsed parse_interpreted(std::shared_ptr<const src::TextInfo> text);

/// @brief Compiles parsed interpreted code to IR, as if written inside `Interpreter:functionCompileContext`.
///
/// Its version is the one given, else a version comment on its file's first line, else the fallback. A given
/// version that disagrees with the comment is an error.
///
/// In most cases, the fallback will be legacy, but is left as an option incase this changes.
///
/// @param given The version passed with the code, e.g., by an editor evaluating part of a file.
/// @param fallback The version of code that has neither.
[[nodiscard]] InterpretedIR compile_interpreted_ir(ClassLibrary& library, InterpretedParsed&& parsed,
                                                   std::optional<ast::LanguageVersion> given,
                                                   ast::LanguageVersion fallback);

/// @brief Runs the backend's stages for interpreted code.
/// @param ir The result of compile_interpreted_ir(), whose graph must not be null.
[[nodiscard]] CompiledGraph compile_interpreted(ClassLibrary& library, const InterpretedIR& ir);


////////////////////////////////////////////////////////////////////////////////
// Class library
////////////////////////////////////////////////////////////////////////////////

struct ClassLibraryTimes {
    // In seconds.
    std::chrono::duration<double> parse {}; ///< Parsing, which includes reading the files.
    std::chrono::duration<double> sema {};
    std::chrono::duration<double> member_table {};
};

/// @brief The compiled class library. Global per vm instance.
/// Its parts are behind `internals` as most places just need to keep it alive, see details/class_library_internals.hpp.
struct ClassLibrary {
    explicit ClassLibrary(ir::ClassLibrarySources class_sources);
    ClassLibrary(const ClassLibrary&) = delete;
    ClassLibrary& operator=(const ClassLibrary&) = delete;
    ~ClassLibrary();

    /// Per file, in order: files that couldn't be opened or parsed.
    std::string parse_log {};
    std::vector<diag::Diagnostic> parse_diagnostics {};
    std::vector<diag::Diagnostic> diagnostics {};

    [[nodiscard]] std::string_view symbol_text(ir::Symbol symbol) const;
    [[nodiscard]] bool is_meta(ir::SymSCClassName name) const;
    [[nodiscard]] const ir::ConstantTable& constants() const;
    [[nodiscard]] std::uint32_t num_class_variables() const;

    std::unique_ptr<ClassLibraryInternals> internals;
};

////////////////////////////////////////////////////////////////////////////////
// Interpreted code
////////////////////////////////////////////////////////////////////////////////

/// @brief Parsed interpreted code.
struct InterpretedParsed {
    std::shared_ptr<const ast::ASTGraph> ast; ///< The parsed code. Null if it can't be parsed, or is a class file.
    std::vector<diag::Diagnostic> diagnostics;
};

/// @brief Interpreted code as IR.
struct InterpretedIR {
    std::shared_ptr<const ir::IRGraph> graph;
    std::vector<diag::Diagnostic> diagnostics;
};
}