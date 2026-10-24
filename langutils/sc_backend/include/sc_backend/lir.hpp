#pragma once

#include "sc_backend/frames.hpp"
#include "sc_backend/members.hpp"
#include "sc_sema/const_expr_index.hpp"
#include "sc_sema/ir_nodes.hpp"
#include "sc_sema/source_location.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_util/indexed_vector.hpp"
#include "sc_util/strong_index.hpp"

#include <cassert>
#include <cstdint>
#include <limits>
#include <variant>
#include <vector>

// The lowered ir: each function's instructions in the order the VM runs them, operands first, with locals, members and
// nested functions resolved. How each is encoded is the emitter's choice. Bodies are sub-regions, contiguous ranges of
// a function's instructions, so the emitter can measure them to lay out jumps.
namespace sc::backend::lir {

////////////////////////////////////////////////////////////////////////////////
// Indexes
////////////////////////////////////////////////////////////////////////////////

// Within one lowered graph.
using FunctionIndex = util::StrongIndex<std::uint32_t, struct FunctionIndex__>;
// The rest are within one function.
using InstructionIndex = util::StrongIndex<std::uint32_t, struct InstructionIndex__>;
using FunctionSubRegionIndex = util::StrongIndex<std::uint32_t, struct FunctionSubRegionIndex__>;
using OptionalFunctionSubRegionIndex = util::StrongOptionalIndex<std::uint32_t, struct FunctionSubRegionIndex__,
                                                                 std::numeric_limits<std::uint32_t>::max()>;
using SwitchCaseIndex = util::StrongIndex<std::uint32_t, struct SwitchCaseIndex__>;

// A slot the VM can address, see FrameSlotIndex.
using SlotIndex = util::StrongIndex<std::uint8_t, struct SlotIndex__>;

// depth is the number of frames up from the current one.
struct LocalRef {
    std::uint16_t depth;
    SlotIndex slot;
};
static_assert(sizeof(LocalRef) == 4);

////////////////////////////////////////////////////////////////////////////////
// Pushes and stores
////////////////////////////////////////////////////////////////////////////////

enum struct SpecialValue : std::uint8_t { Nil, True, False };

struct PushLocal {
    LocalRef local;
};
struct PushInstance {
    InstanceSlotIndex slot;
};
struct PushClassVariable {
    ClassVariableIndex index;
};
struct PushConstant {
    ir::ConstExprIndex value;
};
// The class's own object, as the old compiler pushed it, kept for byte parity. 3.16 uses it only for mutable
// constants.
struct PushClassConstant {
    ir::MemberIndex member;
    ir::ConstExprIndex value;
};
struct PushSpecialValue {
    SpecialValue value;
};
struct PushInteger {
    std::int32_t value;
};
struct PushSymbol {
    ir::SymSCSymbol value;
};
struct PushClass {
    ir::SymSCClassName name;
};
struct PushSpecial {
    ir::SCSpecial::Kind kind;
};
struct PushFunction {
    FunctionIndex function;
};

// Stores keep the value on the stack, the emitter folds a following Drop into them.
struct StoreLocal {
    LocalRef local;
};
struct StoreInstance {
    InstanceSlotIndex slot;
};
struct StoreClassVariable {
    ClassVariableIndex index;
};

struct Dup {};
struct Drop {};

////////////////////////////////////////////////////////////////////////////////
// Sends and returns
////////////////////////////////////////////////////////////////////////////////

enum struct Tail : std::uint8_t { None, ReturnFromFunction, ReturnFromMethod };

enum struct SendKind : std::uint8_t { Normal, Super, WithAdverb };

// How the send was written, which the old compiler chose bytecodes by. Implicit is a send lowering adds, e.g., `new`.
enum struct SendForm : std::uint8_t { Call, Binary, Setter, Implicit };

// An adverb is counted as the last positional argument.
struct Send {
    ir::SymSCSelector selector;
    std::uint8_t num_positional;
    std::uint8_t num_keyword;
    SendKind kind : 2;
    SendForm form : 2;
    Tail tail : 2;
};

struct Return {};
// A method's end when it has no `^`.
struct ReturnSelf {};
// `^` in a function, but only a non-inlined function. `Class { meth { if (a) { ^1 }}}` this is not non-local.
struct NonLocalReturn {};

// Computes the argument when it is nil, its prototype doesn't hold its default.
struct DefaultArgument {
    SlotIndex slot;
    FunctionSubRegionIndex value;
};

////////////////////////////////////////////////////////////////////////////////
// Inlined sends
////////////////////////////////////////////////////////////////////////////////

// See Inlined. The tested value is on the stack. Each body leaves its value, but an empty one leaves nothing, as the
// old compiler had special forms for empty `if` bodies.

struct If {
    FunctionSubRegionIndex then_body;
    OptionalFunctionSubRegionIndex else_body;
};
struct IfNil {
    FunctionSubRegionIndex then_body;
    OptionalFunctionSubRegionIndex else_body;
};
struct IfNotNil {
    FunctionSubRegionIndex then_body;
    OptionalFunctionSubRegionIndex else_body;
};
struct And {
    FunctionSubRegionIndex rhs;
};
struct Or {
    FunctionSubRegionIndex rhs;
};
struct OrIfNil {
    FunctionSubRegionIndex rhs;
};
struct AndIfNotNil {
    FunctionSubRegionIndex rhs;
};
// Both operands are on the stack.
struct QuestionMark {};
// Pushes nil when it ends.
struct While {
    FunctionSubRegionIndex condition;
    OptionalFunctionSubRegionIndex body;
};
// Never ends, but counts as pushing a value.
struct Loop {
    FunctionSubRegionIndex body;
};

// The default has no key.
struct SwitchCase {
    ir::OptionalConstExprIndex key;
    FunctionSubRegionIndex body;
};
static_assert(sizeof(SwitchCase) == 8);

// Its cases are consecutive in Function::switch_cases, the default last.
struct Switch {
    SwitchCaseIndex first;
    std::uint32_t count;
};

[[nodiscard]] inline SwitchCaseIndex switch_case(const Switch& s, std::size_t k) {
    assert(k < s.count);
    return SwitchCaseIndex { *s.first + static_cast<std::uint32_t>(k) };
}

// Also used for malformed instructions.
struct Unimplemented {
    ir::NodeIndex node;
};

using Instruction =
    std::variant<PushLocal, PushInstance, PushClassVariable, PushConstant, PushClassConstant, PushSpecialValue,
                 PushInteger, PushSymbol, PushClass, PushSpecial, PushFunction, StoreLocal, StoreInstance,
                 StoreClassVariable, Dup, Drop, Send, Return, ReturnSelf, NonLocalReturn, DefaultArgument, If, IfNil,
                 IfNotNil, And, Or, OrIfNil, AndIfNotNil, QuestionMark, While, Loop, Switch, Unimplemented>;
static_assert(sizeof(Instruction) == 12);

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

struct SourceSpan {
    std::uint32_t begin;
    std::uint32_t end;
};
static_assert(sizeof(SourceSpan) == 8);

struct FunctionSubRegion {
    InstructionIndex begin;
    std::uint32_t size;
};
static_assert(sizeof(FunctionSubRegion) == 8);

struct Function {
    FrameIndex frame;
    ir::SourceIndex source;
    util::IndexedVector<InstructionIndex, Instruction> instructions {};
    util::IndexedVector<InstructionIndex, SourceSpan> spans {};
    util::IndexedVector<FunctionSubRegionIndex, FunctionSubRegion> sub_regions {};
    util::IndexedVector<SwitchCaseIndex, SwitchCase> switch_cases {};
};

inline constexpr FunctionSubRegionIndex body_sub_region { 0 };

struct LoweredGraph {
    util::IndexedVector<FunctionIndex, Function> functions {};
    std::vector<FunctionIndex> roots {};
};

}
