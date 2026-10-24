#pragma once

#include "sc_backend/frames.hpp"
#include "sc_backend/members.hpp"
#include "sc_sema/const_expr_index.hpp"
#include "sc_sema/ir_nodes.hpp"
#include "sc_sema/source_location.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_util/indexed_vector.hpp"
#include "sc_util/strong_index.hpp"

#include <cstdint>
#include <limits>
#include <variant>
#include <vector>

// The lowered ir: what each method and function does, in the order the VM does it. The VM is a stack machine, so
// operands come before what uses them, and each instruction is one or a few bytecodes. How each is encoded, e.g., a
// nibble or an extended opcode, is the emitter's choice, the lir only says what happens.
// Operands are resolved: locals to frame depth and slot, members to where they are stored, nested functions to their
// index. Bodies are sub-regions, contiguous ranges of a function's instructions, so the emitter can measure them to lay
// out jumps.
namespace sc::backend::lir {

// Within one lowered graph, a class's methods and functions, or interpreted code.
using FunctionIndex = util::StrongIndex<std::uint32_t, struct FunctionIndex__>;
// Within one function.
using InstructionIndex = util::StrongIndex<std::uint32_t, struct InstructionIndex__>;
using FunctionSubRegionIndex = util::StrongIndex<std::uint32_t, struct FunctionSubRegionIndex__>;
using OptionalFunctionSubRegionIndex = util::StrongOptionalIndex<std::uint32_t, struct FunctionSubRegionIndex__,
                                                                 std::numeric_limits<std::uint32_t>::max()>;

// A local in the frame `depth` frames up from the current one, see Frames::depth. Frames have at most 255 slots, and
// code is nested at most max_nesting_depth deep, so both fit.
struct LocalRef {
    std::uint16_t depth;
    std::uint8_t slot;
};

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

// Stores keep the value on the stack, a Drop after one is folded into it by the emitter.
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

enum struct Tail : std::uint8_t { None, ReturnFromFunction, ReturnFromMethod };

enum struct SendKind : std::uint8_t { Normal, Super, WithAdverb };

struct Send {
    ir::SymSCSelector selector;
    std::uint8_t num_positional;
    std::uint8_t num_keyword;
    SendKind kind;
    Tail tail;
};

struct Return {}; // in methods

struct NonLocalReturn {}; // in functions

// Only applied to non-nil preserving arguments.
struct DefaultArgument {
    std::uint8_t slot;
    FunctionSubRegionIndex value; // code to call if slot is nil.
};

struct Unimplemented {
    ir::NodeIndex node;
};

using Instruction =
    std::variant<PushLocal, PushInstance, PushClassVariable, PushConstant, PushSpecialValue, PushInteger, PushSymbol,
                 PushClass, PushSpecial, PushFunction, StoreLocal, StoreInstance, StoreClassVariable, Dup, Drop, Send,
                 Return, NonLocalReturn, DefaultArgument, Unimplemented>;
static_assert(sizeof(Instruction) <= 12);

struct SourceSpan {
    std::uint32_t begin;
    std::uint32_t end;
};

// A part of a function
// Can also be the whole function if needed.
// This is used when computing jumps.
struct FunctionSubRegion {
    InstructionIndex begin;
    std::uint32_t size;
};

struct Function {
    FrameIndex frame;
    ir::SourceIndex source;

    util::IndexedVector<InstructionIndex, Instruction> instructions {};
    util::IndexedVector<InstructionIndex, SourceSpan> spans {};

    util::IndexedVector<FunctionSubRegionIndex, FunctionSubRegion> sub_regions {};
};

inline constexpr FunctionSubRegionIndex body_sub_region { 0 };

struct LoweredGraph {
    util::IndexedVector<FunctionIndex, Function> functions {};
    std::vector<FunctionIndex> roots {};
};

}
