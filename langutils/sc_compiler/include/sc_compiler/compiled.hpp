#pragma once

#include "sc_compiler/details/compiled_function.hpp"

#include "sc_source/text_info.hpp"

#include <memory>
#include <optional>
#include <variant>
#include <vector>

namespace sc::diag {
class Diagnostic;
}

namespace sc::compiler {

// Plain data: names are the library's symbols and values its constants, read through the library.

/// @brief A variable or constant of a compiled class.
struct CompiledMember {
    ir::SymSCIdentifier name;
    ir::OptionalConstExprIndex value; ///< Its value, nil without a value.
};
static_assert(sizeof(CompiledMember) == 8);

/// @brief A compiled class: the data the VM needs to create it. Plain data: names are the library's symbols and values
/// its constants, read through the library.
struct CompiledClass {
    ir::SymSCClassName name;
    ir::OptionalSymSCClassName superclass; ///< Its superclass, none for the root.
    ir::MemoryLayout memory_layout;
    /// Begins at the class's name.
    ir::SourceLocation location;
    /// A class's, the inherited ones first. A meta class's are Class's, which the VM gives it.
    std::vector<CompiledMember> instance_variables;
    /// A meta class's own, which the VM keeps in its class, numbered from first_class_variable.
    std::optional<backend::ClassVariableIndex> first_class_variable;
    std::vector<CompiledMember> class_variables;
    std::vector<CompiledMember> constants;
};

/// @brief A compiled method.
struct CompiledMethod {
    ir::SymSCSelector selector;
    backend::lir::FunctionIndex function; ///< Its code, in CompiledGraph::functions.
    ir::OptionalSymSCPrimitive primitive;
};
static_assert(sizeof(CompiledMethod) == 12);

/// @brief A compiled getter or setter. Its location is the member's name.
struct CompiledAccessor {
    ir::SymSCSelector selector;
    ir::SourceLocation location;
    backend::OptionalMethodShortcut shortcut;
};
static_assert(sizeof(CompiledAccessor) == 44);

/// @brief Compiled interpreted code.
struct CompiledInterpreted {
    /// A function for each region, in order.
    std::vector<backend::lir::FunctionIndex> functions;
};

/// @brief The result of compiling one class, or one piece of interpreted code.
struct CompiledGraph {
    std::variant<CompiledClass, CompiledInterpreted> unit; ///< What was compiled.
    /// The text the locations refer to.
    util::IndexedVector<ir::SourceIndex, std::shared_ptr<const src::TextInfo>> sources;
    util::IndexedVector<backend::lir::FunctionIndex, CompiledFunction> functions;
    std::vector<CompiledMethod> methods;
    std::vector<CompiledAccessor> accessors;
    /// What was found by the backend: the frames', then the emitter's.
    std::vector<diag::Diagnostic> diagnostics;
};
}
