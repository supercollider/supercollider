#pragma once

#include "sc_backend/details/frames.hpp"
#include "sc_backend/details/inlining.hpp"
#include "sc_backend/details/lir.hpp"
#include "sc_backend/details/members.hpp"
#include "sc_bytecode/ByteCodeArray.h"

#include <cstdint>
#include <ostream>
#include <string_view>
#include <variant>
#include <vector>


////////////////////////////////////////////////////////////////////////////////
// Turns the lir into supercollider specific instructions.
////////////////////////////////////////////////////////////////////////////////


namespace sc::diag {
class Diagnostic;
}

namespace sc::ir {
struct ConstantTable;
struct SymbolTable;
}

namespace sc::backend {

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

struct EmittedGraph;
struct EmittedFunction;
struct Accessor;

/// @brief Everything emit() needs, all referred to rather than owned, so they must outlive the call.
struct EmitContext {
    const ir::IRGraph& graph;
    const Frames& frames;
    const MemberTable& members;
    const ir::ConstantTable& constants;
    const SpecialSymbols& special_symbols;
    /// Whether it needs access to the Interpreter's memebers.
    /// Is interpreter code, or a closed function.
    bool interpreter_members;
};

/// @brief Encodes lowered instructions as bytecode, with the selectors and constants each function refers to.
/// @return The emitted graph is parallel to the lowered graph.
[[nodiscard]] EmittedGraph emit(const EmitContext& ctx, const lir::LoweredGraph& lowered);

void write_emitted(std::ostream& out, const EmittedFunction& function, std::string_view indent,
                   const ir::SymbolTable& symbols, const ir::ConstantTable& constants);
void write_accessor(std::ostream& out, const Accessor& accessor, std::string_view indent,
                    const ir::SymbolTable& symbols, const ir::ConstantTable& constants);


////////////////////////////////////////////////////////////////////////////////
// Emitted function
////////////////////////////////////////////////////////////////////////////////

using SelectorIndex = util::StrongIndex<std::uint8_t, struct SelectorIndex__>;
using ConstantIndex = util::StrongIndex<std::uint32_t, struct ConstantIndex__>;

/// @brief A constant of a class, which the VM loads from the class itself.
struct ClassConstant {
    ir::SymSCClassName owner;
    ir::SymSCIdentifier name;
    ir::ConstExprIndex value;
};
static_assert(sizeof(ClassConstant) == 12);

/// @brief What an entry of a function's selectors table holds: a selector, a class name, or a nested function.
using SelectorContent = std::variant<ir::SymSCSelector, ir::SymSCClassName, lir::FunctionIndex>;
static_assert(sizeof(SelectorContent) == 8);

/// @brief A byte of a function's code, as an offset from a point the instruction using it defines.
using CodeOffset = util::StrongIndex<std::uint32_t, struct CodeOffset__>;

/// @brief The jump table of an inlined switch.
struct SwitchTable {
    std::uint32_t capacity { 0 };
    std::vector<std::pair<ir::ConstExprIndex, CodeOffset>> offsets {}; ///< Each key and where its case starts.
    CodeOffset default_offset { 0 };
};

using SwitchTableIndex = util::StrongIndex<std::uint32_t, struct SwitchTableIndex__>;

using ConstantContent = std::variant<ir::ConstExprIndex, ir::SymSCSymbol, ClassConstant, SwitchTableIndex>;
static_assert(sizeof(ConstantContent) == 16);


/// @brief Flags the VM reads from a function's definition.
struct FunctionProperties {
    bool has_context { false };
    bool is_closed { false };
    bool needs_heap_context { false };
};
static_assert(sizeof(FunctionProperties) == 3);

/// @brief The old compiler's methType shortcuts. The VM runs such a method without code.
namespace shortcut {

struct ReturnSelf {};

struct ReturnLiteral {
    ir::ConstExprIndex value;
};

struct ReturnArgument {
    lir::SlotIndex slot;
};

struct ReturnInstance {
    InstanceSlotIndex slot;
};

struct AssignInstance {
    InstanceSlotIndex slot;
};

/// `^this.sel(a, b)` or `^super.sel(a, b)`, passing all of the method's arguments in order.
struct Redirect {
    ir::SymSCSelector selector;
    bool super;
};

/// `^x.sel(a, b)` where `x` is an instance variable, as Redirect.
struct ForwardInstance {
    ir::SymSCSelector selector;
    InstanceSlotIndex slot;
};

/// `^x.sel(a, b)` where `x` is a class variable, as Redirect.
struct ForwardClassVariable {
    ir::SymSCSelector selector;
    ClassVariableIndex index;
};

struct ReturnClassVariable {
    ClassVariableIndex index;
};

struct AssignClassVariable {
    ClassVariableIndex index;
};
}

using OptionalMethodShortcut =
    std::variant<std::monostate, shortcut::ReturnSelf, shortcut::ReturnLiteral, shortcut::ReturnArgument,
                 shortcut::ReturnInstance, shortcut::AssignInstance, shortcut::Redirect, shortcut::ForwardInstance,
                 shortcut::ForwardClassVariable, shortcut::ReturnClassVariable, shortcut::AssignClassVariable>;
static_assert(sizeof(OptionalMethodShortcut) == 12);

void write_shortcut(std::ostream& out, const OptionalMethodShortcut& shortcut, const ir::SymbolTable& symbols,
                    const ir::ConstantTable& constants);

/// @brief A function as the VM loads it: bytecode, and the selectors, constants, and switch tables it refers to.
struct EmittedFunction {
    /// Shortcuts have no code, a function might be thrown away but have no shortcut if it is nested inside a
    /// shortcutted method.
    OptionalMethodShortcut shortcut {};
    bool thrown_away { false };

    FunctionProperties properties {};
    CompilingBytecodes::Data code {};
    util::IndexedVector<SelectorIndex, SelectorContent> selectors {};
    util::IndexedVector<ConstantIndex, ConstantContent> constants {};
    util::IndexedVector<SwitchTableIndex, SwitchTable> switch_tables {};
};

////////////////////////////////////////////////////////////////////////////////
// Accessor
////////////////////////////////////////////////////////////////////////////////

/// @brief A getter or setter of one of the class's own members, a method that is only a shortcut.
struct Accessor {
    ir::SCMember_I member;
    ir::SymSCSelector selector;
    OptionalMethodShortcut shortcut;
};
static_assert(sizeof(Accessor) == 20);

////////////////////////////////////////////////////////////////////////////////
// Emitted graph
////////////////////////////////////////////////////////////////////////////////

/// @brief The output of emit(), the input is lir::LoweredGraph.
struct EmittedGraph {
    util::IndexedVector<lir::FunctionIndex, EmittedFunction>
        functions {}; ///< Every function, in the same order as the lowered graph.

    /// One for interpreted, one per method in class code, one per region.
    std::vector<lir::FunctionIndex> roots {};
    std::vector<Accessor> accessors {};
    std::vector<sc::diag::Diagnostic> diagnostics {};
};
}
