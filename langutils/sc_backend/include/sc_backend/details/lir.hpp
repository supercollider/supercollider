#pragma once

#include "sc_backend/details/frames.hpp"
#include "sc_backend/details/members.hpp"

#include "sc_sema/details/indexes.hpp"
#include "sc_sema/details/symbols.hpp"
#include "sc_sema/details/ir_nodes_details.hpp"

#include "sc_util/indexed_vector.hpp"
#include "sc_util/strong_index.hpp"

#include <cassert>
#include <cstdint>
#include <limits>
#include <variant>

// The lowered ir: each function's instructions in the order the VM runs them, operands first, with locals,
// members and nested functions resolved. How each is encoded is the emitter's choice. Each body, the function's own
// and those inlined into it, is a contiguous range of its instructions, so the emitter can measure them to lay out
// jumps.
namespace sc::backend::lir {

////////////////////////////////////////////////////////////////////////////////
// Indexes
////////////////////////////////////////////////////////////////////////////////

using FunctionIndex = util::StrongIndex<std::uint32_t, struct FunctionIndex__>;
using OptionalFunctionIndex =
    util::StrongOptionalIndex<std::uint32_t, struct FunctionIndex__, std::numeric_limits<std::uint32_t>::max()>;
using InstructionIndex = util::StrongIndex<std::uint32_t, struct InstructionIndex__>;
using BodyIndex = util::StrongIndex<std::uint32_t, struct BodyIndex__>;
using OptionalBodyIndex =
    util::StrongOptionalIndex<std::uint32_t, struct BodyIndex__, std::numeric_limits<std::uint32_t>::max()>;
using SwitchCaseIndex = util::StrongIndex<std::uint32_t, struct SwitchCaseIndex__>;

// A slot the VM can address, see FrameSlotIndex.
using SlotIndex = util::StrongIndex<std::uint8_t, struct SlotIndex__>;

struct LocalRef {
    std::uint16_t depth; // The number of frames up from the current one.
    SlotIndex slot;
};
static_assert(sizeof(LocalRef) == 4);

////////////////////////////////////////////////////////////////////////////////
// Pushes and stores
////////////////////////////////////////////////////////////////////////////////

enum struct SpecialValue : std::uint8_t { Nil, True, False };

// Instructions that push a value on the stack.

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
// Pushes a constant through the class's own object, as the old compiler did, kept for byte parity.
// 3.16 uses it only for mutable constants.
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

// Whether a send is the last thing a function does, so its result is returned straight away.
enum struct Tail : std::uint8_t { None, ReturnFromFunction, ReturnFromMethod };

enum struct SendKind : std::uint8_t { Normal, Super, WithAdverb };

// How the send was written, which the old compiler chose bytecodes by.
// Implicit is a send lowering adds, e.g., `new`.
enum struct SendForm : std::uint8_t { Call, Binary, Setter, Implicit };

// Sends a message. The receiver and arguments are on the stack.
struct Send {
    ir::SymSCSelector selector;
    std::uint8_t num_positional; // The number of positional arguments. An adverb is counted as the last one.
    std::uint8_t num_keyword;
    SendKind kind : 2;
    SendForm form : 2;
    Tail tail : 2;
};

struct Return {};
// Returns `this`. A method's end when it has no `^`.
struct ReturnSelf {};
// Returns from the method that contains the function.
// `^` in a function, but only a non-inlined function. `Class { meth { if (a) { ^1 }}}` this is not non-local.
struct NonLocalReturn {};

// Computes the argument when it is nil, its prototype doesn't hold its default.
struct DefaultArgument {
    SlotIndex slot;
    BodyIndex value;
};

////////////////////////////////////////////////////////////////////////////////
// Inlined sends
////////////////////////////////////////////////////////////////////////////////

// See Inlined. The tested value is on the stack. Each body leaves its value, but an empty one leaves nothing, as the
// old compiler had special forms for empty `if` bodies.

struct If {
    BodyIndex then_body;
    OptionalBodyIndex else_body;
};
// `if (x.isNil)`.
struct IfNil {
    BodyIndex then_body;
    OptionalBodyIndex else_body;
};
// `if (x.notNil)`.
struct IfNotNil {
    BodyIndex then_body;
    OptionalBodyIndex else_body;
};
// `a and: { b }`.
struct And {
    BodyIndex rhs;
};
// `a or: { b }`.
struct Or {
    BodyIndex rhs;
};
// `a ?? { b }`.
struct OrIfNil {
    BodyIndex rhs;
};
// `a !? { b }`.
struct AndIfNotNil {
    BodyIndex rhs;
};
// `a ? b`. Both operands are on the stack.
struct QuestionMark {};
// `{ c }.while { b }`. Pushes nil when it ends.
struct While {
    BodyIndex condition;
    OptionalBodyIndex body;
};
// `{ b }.loop`. Never ends, but counts as pushing a value.
struct Loop {
    BodyIndex body;
};

// One case of a Switch. The default has no key.
struct SwitchCase {
    ir::OptionalConstExprIndex key; // The value to match, none for the default.
    BodyIndex body;
};
static_assert(sizeof(SwitchCase) == 8);

// `switch`. Its cases are consecutive in Function::switch_cases, the default last.
struct Switch {
    SwitchCaseIndex first;
    std::uint32_t count;
};

[[nodiscard]] inline util::IndexRange<SwitchCaseIndex> cases(const Switch& s) {
    return { s.first, util::advance(s.first, s.count) };
}

// Something that can't be lowered yet. Also used for malformed instructions.
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

// A contiguous range of a function's instructions: the function's own, or one inlined into it.
struct Body {
    InstructionIndex begin;
    std::uint32_t size;
};
static_assert(sizeof(Body) == 8);

[[nodiscard]] inline util::IndexRange<InstructionIndex> instructions(const Body& b) {
    return { b.begin, util::advance(b.begin, b.size) };
}

struct Function {
    FrameIndex frame;
    ir::SourceIndex source;
    // A root whose code, and its nested functions', all lowered, so can be emitted. Unset for nested functions.
    bool complete { false };
    util::IndexedVector<InstructionIndex, Instruction> instructions {};
    util::IndexedVector<InstructionIndex, SourceSpan> spans {};
    util::IndexedVector<BodyIndex, Body> bodies {}; // The bodies, the function's own is function_body.
    util::IndexedVector<SwitchCaseIndex, SwitchCase> switch_cases {};
};

inline constexpr BodyIndex function_body { 0 };

// The output of lower(). A root's nested functions follow it.
struct LoweredGraph {
    util::IndexedVector<FunctionIndex, Function> functions {};
};

}
