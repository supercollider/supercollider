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
#include "sc_util/strong_index.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace sc::compiler {

struct ClassFile;
struct ClassFileSource;
struct VMDefinitions;
struct ClassLibraryTimes;
struct ClassLibrary;
struct Backend;
struct InterpretedParsed;
struct InterpretedIR;
struct FramedGraph;
struct LoweredGraph;
struct EmittedGraph;
struct CompiledGraph;

using ClassFiles = util::IndexedVector<ir::SourceEntryIndex, ClassFile>;

////////////////////////////////////////////////////////////////////////////////
// API: sclang
////////////////////////////////////////////////////////////////////////////////

// The VM's definitions, its intrinsic classes and primitives, are checked against the classes, none are without them.
[[nodiscard]] std::unique_ptr<ClassLibrary> compile_class_library(const ClassFiles& files, std::size_t num_threads,
                                                                  ClassLibraryTimes& times, const VMDefinitions& vm);

// A class graph's topological position.
using ClassGraphIndex = util::StrongIndex<std::uint32_t, struct ClassGraphIndex__>;

// Every class graph through the backend's stages, in parallel, in topological order. Only lowering writes to the
// library, to its symbols, which are thread safe.
[[nodiscard]] util::IndexedVector<ClassGraphIndex, CompiledGraph> compile_class_graphs(ClassLibrary& library,
                                                                                       std::size_t num_threads);

// Interpreted code, e.g., an editor's text or an scd file, after the class library, then the backend's stages as a
// class's are, from layout_frames.
[[nodiscard]] InterpretedParsed parse_interpreted(std::shared_ptr<const src::TextInfo> text);
[[nodiscard]] InterpretedIR compile_interpreted_ir(ClassLibrary& library, InterpretedParsed&& parsed,
                                                   ast::LanguageVersion version);

////////////////////////////////////////////////////////////////////////////////
// API: inspection and debugging
////////////////////////////////////////////////////////////////////////////////

// Each path is a module, a directory's .sc files or one .sc file.
[[nodiscard]] ClassFiles
find_class_files(const util::IndexedVector<ir::ModuleIndex, std::filesystem::path>& module_paths);

// Text that isn't in files, e.g., an editor's.
[[nodiscard]] std::unique_ptr<ClassLibrary>
compile_class_library(const util::IndexedVector<ir::SourceEntryIndex, ClassFileSource>& sources,
                      std::size_t num_threads, ClassLibraryTimes& times, const VMDefinitions& vm);

using CompilationUnit = ir::join<ir::SCClass_I, ir::SCInterpreted_I>;

// The backend's stages, each moving the last's result into its own, which compile_class_graphs runs per graph.
[[nodiscard]] FramedGraph layout_frames(const ClassLibrary& library, const ir::IRGraph& g, CompilationUnit root);
[[nodiscard]] LoweredGraph lower(ClassLibrary& library, FramedGraph&& framed);
[[nodiscard]] EmittedGraph emit(const ClassLibrary& library, LoweredGraph&& lowered);
[[nodiscard]] CompiledGraph finish(const ClassLibrary& library, EmittedGraph&& emitted);

void write_compiled(std::ostream& out, const CompiledGraph& compiled, const ClassLibrary& library);

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
// VM definitions
////////////////////////////////////////////////////////////////////////////////

// What the VM defines itself, as strings, as the library's symbols don't exist yet.

// A class whose layout the VM relies on, e.g., Array or Interpreter.
struct IntrinsicClass {
    std::string name;
    std::optional<std::string> superclass;
    // e.g., `slot` or `char`, none when it isn't indexed.
    std::optional<std::string> slot_type;
    // Its own, not those it inherits.
    std::vector<std::string> instance_variables;
};

// See ir::PrimitiveSignature.
struct Primitive {
    std::string name;
    std::uint8_t num_arguments;
    bool variadic_arguments;
    bool keyword_arguments;
};

struct VMDefinitions {
    std::vector<IntrinsicClass> classes {};
    std::vector<Primitive> primitives {};
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
    // The class a poisoned method throws, see backend::LoweringContext. Unset when the library doesn't define it.
    static constexpr const char* poisoned_method_error_name { "PoisonedMethodError" };
    ir::OptionalSymSCClassName poisoned_method_error {};
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
// Interpreted code
////////////////////////////////////////////////////////////////////////////////

// Null if it can't be parsed, or is a class file.
struct InterpretedParsed {
    std::shared_ptr<const ast::ASTGraph> ast;
    std::vector<diag::Diagnostic> diagnostics;
};

// Null if it wasn't parsed, or the library has no Interpreter to compile it in. Its root is an ir::SCInterpreted_I.
struct InterpretedIR {
    std::shared_ptr<const ir::IRGraph> graph;
    std::vector<diag::Diagnostic> diagnostics;
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

////////////////////////////////////////////////////////////////////////////////
// Compiled graph
////////////////////////////////////////////////////////////////////////////////

// Plain data: names are the library's symbols and values its constants, read through the library.

// Nil without a value.
struct CompiledMember {
    ir::SymSCIdentifier name;
    ir::OptionalConstExprIndex value;
};
static_assert(sizeof(CompiledMember) == 8);

struct CompiledClass {
    ir::SymSCClassName name;
    ir::OptionalSymSCClassName superclass;
    ir::SCClass::MemoryLayout memory_layout;
    // Begins at the class's name.
    ir::SourceLocation location;
    // A class's, the inherited ones first. A meta class's are Class's, which the VM gives it.
    std::vector<CompiledMember> instance_variables;
    // A meta class's own, which the VM keeps in its class, numbered from first_class_variable.
    std::optional<backend::ClassVariableIndex> first_class_variable;
    std::vector<CompiledMember> class_variables;
    std::vector<CompiledMember> constants;
};

// The prototype frame's value is nil unless the argument's default or the variable's initial value is a literal.
struct CompiledSlot {
    ir::SymSCIdentifier name;
    ir::OptionalConstExprIndex prototype;
};
static_assert(sizeof(CompiledSlot) == 8);

struct CompiledFunction {
    ir::SourceLocation location;
    ir::OptionalSymSCSymbol name;
    backend::lir::OptionalFunctionIndex parent;
    bool has_this;
    std::uint32_t num_positional;
    backend::Variadic variadic; // none, args, kwargs
    // As backend::Frame's.
    util::IndexedVector<backend::FrameSlotIndex, CompiledSlot> slots;
    backend::EmittedFunction emitted;
};

struct CompiledMethod {
    ir::SymSCSelector selector;
    backend::lir::FunctionIndex function;
    ir::OptionalSymSCPrimitive primitive;
};
static_assert(sizeof(CompiledMethod) == 12);

// Its location is the member's name.
struct CompiledAccessor {
    ir::SymSCSelector selector;
    ir::SourceLocation location;
    backend::OptionalMethodShortcut shortcut;
};
static_assert(sizeof(CompiledAccessor) == 44);

struct CompiledInterpreted {
    // A region's each, in order.
    std::vector<backend::lir::FunctionIndex> functions;
};

struct CompiledGraph {
    std::variant<CompiledClass, CompiledInterpreted> unit;
    util::IndexedVector<ir::SourceIndex, std::shared_ptr<const src::TextInfo>> sources;
    util::IndexedVector<backend::lir::FunctionIndex, CompiledFunction> functions;
    std::vector<CompiledMethod> methods;
    std::vector<CompiledAccessor> accessors;
    // The frames', then the emitter's.
    std::vector<diag::Diagnostic> diagnostics;
};
}
