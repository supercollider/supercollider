#pragma once

#include "sc_backend/frames.hpp"
#include "sc_backend/inlining.hpp"
#include "sc_backend/lir.hpp"
#include "sc_backend/members.hpp"
#include "sc_bytecode/ByteCodeArray.h"
#include "sc_diagnostic/sc_diagnostic.hpp"

#include <cstdint>
#include <ostream>
#include <string_view>
#include <variant>
#include <vector>

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

struct EmitContext {
    const ir::IRGraph& graph;
    const Frames& frames;
    const MemberTable& members;
    const ir::ConstantTable& constants;
    const SpecialSymbols& special_symbols;
    // Interpreted code or the Interpreter class, whose members don't stop a function being closed, as the old compiler
    // had it. A closed function's are the Interpreter's too.
    bool interpreter_members;
};

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

struct ClassConstant {
    ir::SymSCClassName owner;
    ir::SymSCIdentifier name;
    ir::ConstExprIndex value;
};
static_assert(sizeof(ClassConstant) == 12);

using SelectorContent = std::variant<ir::SymSCSelector, ir::SymSCClassName, lir::FunctionIndex>;
static_assert(sizeof(SelectorContent) == 8);

// A byte of a function's code, as an offset from a point the instruction using it defines.
using CodeOffset = util::StrongIndex<std::uint32_t, struct CodeOffset__>;

// Inlined switch table.
struct SwitchTable {
    std::uint32_t capacity { 0 };
    std::vector<std::pair<ir::ConstExprIndex, CodeOffset>> offsets {};
    CodeOffset default_offset { 0 };
};

using SwitchTableIndex = util::StrongIndex<std::uint32_t, struct SwitchTableIndex__>;

using ConstantContent = std::variant<ir::ConstExprIndex, ir::SymSCSymbol, ClassConstant, SwitchTableIndex>;
static_assert(sizeof(ConstantContent) == 16);


struct FunctionProperties {
    // The FunctionDef's contextDef is the frame it is created in. Never for a method, or a function that is closed,
    // explicitly (`#{ }`, interpreted code) or because it uses nothing of the frames around it.
    bool has_context { false };
    bool is_closed { false };
    // Sometimes a frame can be reference counted.
    bool needs_heap_context { false };
};
static_assert(sizeof(FunctionProperties) == 3);

// The old compiler's methType shortcuts. The VM runs such a method without code, so it has none.
namespace shortcut {
struct ReturnSelf {};
// Not a class name or a function.
struct ReturnLiteral {
    ir::ConstExprIndex value;
};
// Slot 0 is `this`.
struct ReturnArgument {
    lir::SlotIndex slot;
};
struct ReturnInstance {
    InstanceSlotIndex slot;
};
// `x = a`, the method's first argument, from 3.16. The old compiler's check never matched its own parse tree.
struct AssignInstance {
    InstanceSlotIndex slot;
};
// `^this.sel(a, b)` or `^super.sel(a, b)`, passing all of the method's arguments in order.
struct Redirect {
    ir::SymSCSelector selector;
    bool super;
};
// `^x.sel(a, b)`, as Redirect.
struct ForwardInstance {
    ir::SymSCSelector selector;
    InstanceSlotIndex slot;
};
struct ForwardClassVariable {
    ir::SymSCSelector selector;
    ClassVariableIndex index;
};
// Legacy only makes these for accessors.
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

struct EmittedFunction {
    // In the body of a method whose code is a hard coded loop, so never made.
    bool thrown_away { false };
    FunctionProperties properties {};
    OptionalMethodShortcut shortcut {};
    CompilingBytecodes::Data code {};
    util::IndexedVector<SelectorIndex, SelectorContent> selectors {};
    util::IndexedVector<ConstantIndex, ConstantContent> constants {};
    util::IndexedVector<SwitchTableIndex, SwitchTable> switch_tables {};
};

////////////////////////////////////////////////////////////////////////////////
// Accessor
////////////////////////////////////////////////////////////////////////////////

// A getter or setter of one of the class's own members, a method that is only a shortcut. Its source is the member's
// name, it takes `this` and, for a setter, the value.
struct Accessor {
    ir::SCMember_I member;
    ir::SymSCSelector selector;
    OptionalMethodShortcut shortcut;
};
static_assert(sizeof(Accessor) == 20);

////////////////////////////////////////////////////////////////////////////////
// Emitted graph
////////////////////////////////////////////////////////////////////////////////

// This is the output of the emit function, the input is LoweredGraph.
struct EmittedGraph {
    util::IndexedVector<lir::FunctionIndex, EmittedFunction> functions {};
    std::vector<lir::FunctionIndex> roots {};
    std::vector<Accessor> accessors {};
    std::vector<sc::diag::Diagnostic> diagnostics {};
};
}
