#include "sc_backend/emit.hpp"
#include "sc_bytecode/Opcodes.h"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_sema/versions.hpp"
#include "sc_util/overload.hpp"
#include "sc_util/unreachable.hpp"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <limits>
#include <optional>

namespace sc::backend {

namespace {

using namespace Opcode;

// The old compiler reported a full table when adding to one of 255 entries.
constexpr std::size_t max_selectors { 255 };
constexpr std::size_t max_jump { 65535 };
constexpr std::size_t max_argument_count { 255 };

////////////////////////////////////////////////////////////////////////////////
// Slot identity
////////////////////////////////////////////////////////////////////////////////

// The old compiler shared table entries whose slots were equal.
struct LiteralObject {
    friend bool operator==(LiteralObject, LiteralObject) { return false; }
};
// A class constant's object is the class's own, so is equal to itself.
struct ClassObject {
    ir::SymSCClassName owner;
    ir::SymSCIdentifier name;
    friend bool operator==(const ClassObject& l, const ClassObject& r) {
        return l.owner == r.owner && l.name == r.name;
    }
};
using SlotIdentity =
    std::variant<double, std::int32_t, ir::SymSCSymbol, char, ir::Nil, bool, ClassObject, LiteralObject>;

[[nodiscard]] SlotIdentity identity_of(const ir::ConstantTable& constants, ir::ConstExprIndex c) {
    return constants(c).visit(util::overload {
        [](double d) -> SlotIdentity { return d; },
        [](std::int32_t i) -> SlotIdentity { return i; },
        [](ir::SymSCSymbol s) -> SlotIdentity { return s; },
        [](char c) -> SlotIdentity { return c; },
        [](ir::Nil n) -> SlotIdentity { return n; },
        [](bool b) -> SlotIdentity { return b; },
        [](const auto&) -> SlotIdentity { return LiteralObject {}; },
    });
}

[[nodiscard]] SlotIdentity identity_of(const ir::ConstantTable& constants, const ConstantContent& entry) {
    return std::visit(util::overload {
                          [&](ir::ConstExprIndex c) { return identity_of(constants, c); },
                          [&](ir::SymSCSymbol s) -> SlotIdentity { return s; },
                          [&](const ClassConstant& c) {
                              auto identity = identity_of(constants, c.value);
                              if (std::holds_alternative<LiteralObject>(identity))
                                  identity = ClassObject { c.owner, c.name };
                              return identity;
                          },
                          [&](SwitchTableIndex) -> SlotIdentity { return LiteralObject {}; },
                      },
                      entry);
}

////////////////////////////////////////////////////////////////////////////////
// Plans
////////////////////////////////////////////////////////////////////////////////

// A value on the stack while a body is read, as the instructions that start and finish computing it.
struct Value {
    lir::InstructionIndex start;
    lir::InstructionIndex producer;

    // A push, which can be left out on its own.
    [[nodiscard]] bool pushed_by_one_instruction() const { return start == producer; }
};
static_assert(sizeof(Value) == 8);

// A send's operand, the receiver first.
using OperandIndex = util::StrongIndex<std::uint32_t, struct OperandIndex__>;
inline constexpr OperandIndex receiver_operand { 0 };

// One send's operands, a part of its body's, see BodyPlan.
class SendOperands {
public:
    SendOperands(const Value* first, std::uint32_t count): m_first(first), m_count(count) {}

    [[nodiscard]] Value operator[](OperandIndex k) const {
        assert(*k < m_count);
        return m_first[*k];
    }

private:
    const Value* m_first;
    std::uint32_t m_count;
};

// Operands first to n, not including n, e.g., the receiver and positional arguments, before any keyword arguments.
[[nodiscard]] util::IndexRange<OperandIndex> operands(OperandIndex first, std::uint32_t n) {
    return { first, OperandIndex { n } };
}

// Each send of a body, in order.
using SendIndex = util::StrongIndex<std::uint32_t, struct SendIndex__>;

enum struct SendOp : std::uint8_t {
    None, // Not encoded, e.g., a send absorbed by another, `(a..b).do(f)` is one forSeries.
    Message,
    MessageX,
    MessageThisOpt,
    Super,
    SuperX,
    SuperThisOpt,
    Special,
    SpecialX,
    SpecialThisOpt,
    PushInstVarAndSpecial,
    PushAllArgs,
    PushAllButFirstArg,
    PushAllButFirstTwoArgs,
    PushAllArgsSpecial,
    PushAllButFirstArgSpecial,
    UnaryX,
    Binary,
    BinaryWithAdverb,
    PushOneAndAddOne,
    PushOneAndSubtract,
};

// When a send's selector is added to the selectors table. The old compiler added most before compiling the receiver
// and arguments, so they are added at the receiver's first instruction.
enum struct AddSelector : std::uint8_t { No, AtReceiver, AtSend };

struct SendPlan {
    SendOp op { SendOp::None };
    AddSelector add { AddSelector::No };
    bool tail { false };
    std::uint8_t num_args { 0 }; // include receiver, and 2x the keyword count
    std::uint8_t num_keyword { 0 }; // 1 per pair of key and value.
    SpecialSymbols::SpecialSelector special { OpSpecialSelectors::New };
    InstanceSlotIndex instance { 0 };
    SelectorIndex selector { 0 };
};
static_assert(sizeof(SendPlan) == 16);


struct BodyPlan {
    explicit BodyPlan(lir::Body b): body(b), m_left_out(b.size, false) {}

    lir::Body body;
    util::IndexedVector<SendIndex, SendPlan> sends {};
    util::IndexedVector<SendIndex, lir::InstructionIndex> send_instructions {};
    // The instruction each send's selector is added at, and the send, sorted by instruction. Sends added at the same
    // instruction are in the order the old compiler added them, the outer first.
    std::vector<std::pair<lir::InstructionIndex, SendIndex>> additions {};

    [[nodiscard]] bool contains(lir::InstructionIndex i) const {
        return *i >= *body.begin && *i < *body.begin + body.size;
    }
    [[nodiscard]] bool left_out(lir::InstructionIndex i) const { return m_left_out[offset(i)]; }
    void leave_out(lir::InstructionIndex i) { m_left_out[offset(i)] = true; }

private:
    // A vector<bool> by position in the body, packed, so not an IndexedVector.
    [[nodiscard]] std::size_t offset(lir::InstructionIndex i) const {
        assert(contains(i));
        return *i - *body.begin;
    }
    std::vector<bool> m_left_out;
};

////////////////////////////////////////////////////////////////////////////////
// Emitter
////////////////////////////////////////////////////////////////////////////////

// Operand k pushes the function's argument k, the first being `this` in a method. A frame with more slots than the VM
// can address isn't lowered.
[[nodiscard]] lir::SlotIndex argument_slot(OperandIndex k) {
    assert(*k <= std::numeric_limits<std::uint8_t>::max());
    return lir::SlotIndex { static_cast<std::uint8_t>(*k) };
}

struct Emitter {
    const EmitContext& ctx;
    const lir::Function& function;
    const Frame& frame;
    EmittedFunction out {};
    std::vector<sc::diag::Diagnostic>& diagnostics;
    bool failed { false };
    bool reported_selectors { false };
    // Each constant's identity, so adding one doesn't look up every entry again.
    util::IndexedVector<ConstantIndex, SlotIdentity> constant_identities {};

    [[nodiscard]] const lir::Instruction& instruction(lir::InstructionIndex i) const {
        return function.instructions[i];
    }

    [[nodiscard]] sc::src::SourceCodeRange location(lir::InstructionIndex i) const {
        const auto span = function.spans[i];
        return { { span.begin, 0, 0 }, { span.end, 0, 0 } };
    }

    [[nodiscard]] ir::SourceLocation owner_location() const { return ctx.graph.location(frame.owner); }

    void report(sc::diag::Diagnostic d) {
        diagnostics.push_back(std::move(d));
        failed = true;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Tables
    ////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] static std::uint32_t symbol_of(const SelectorContent& e) {
        return std::visit(
            util::overload { [](ir::SymSCSelector s) { return *s; }, [](ir::SymSCClassName s) { return *s; },
                             [](lir::FunctionIndex) { return std::numeric_limits<std::uint32_t>::max(); } },
            e);
    }

    // Symbols are shared, as the old compiler compared slots, every function is its own entry.
    SelectorIndex add_selector(SelectorContent entry) {
        if (!std::holds_alternative<lir::FunctionIndex>(entry))
            for (const auto index : out.selectors.indices()) {
                if (!std::holds_alternative<lir::FunctionIndex>(out.selectors[index])
                    && symbol_of(out.selectors[index]) == symbol_of(entry))
                    return index;
            }
        if (out.selectors.size() >= max_selectors) {
            if (!reported_selectors) {
                reported_selectors = true;
                const auto loc = owner_location();
                report(sc::diag::Diagnostic::selector_table_full(ctx.graph.text_info(loc.source),
                                                                 ir::IRGraph::range(loc), max_selectors));
            }
            return SelectorIndex { 0 };
        }
        return out.selectors.push_back(entry);
    }

    ConstantIndex add_constant(ConstantContent entry) {
        auto identity = identity_of(ctx.constants, entry);
        for (const auto i : constant_identities.indices())
            if (identity == constant_identities[i])
                return i;
        (void)constant_identities.push_back(std::move(identity));
        return out.constants.push_back(std::move(entry));
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Planning
    ////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] std::uint32_t num_block_args() const {
        return (frame.has_this ? 1u : 0u) + frame.num_positional + frame.num_variadic();
    }

    [[nodiscard]] bool is_this(Value v) const {
        if (!v.pushed_by_one_instruction())
            return false;
        const auto* s = std::get_if<lir::PushSpecial>(&instruction(v.producer));
        return s && s->kind == ir::SCSpecial::Kind::This;
    }

    // Written as a name, which the old compiler compared with the argument names, e.g., `a`, `this`, `Foo`.
    [[nodiscard]] bool is_name(Value v) const {
        if (!v.pushed_by_one_instruction())
            return false;
        const auto& i = instruction(v.producer);
        if (std::holds_alternative<lir::PushLocal>(i) || std::holds_alternative<lir::PushInstance>(i)
            || std::holds_alternative<lir::PushClassVariable>(i) || std::holds_alternative<lir::PushClassConstant>(i)
            || std::holds_alternative<lir::PushSpecial>(i) || std::holds_alternative<lir::PushClass>(i))
            return true;
        // `` `x `` is `Ref.new(x)`, whose class is a constant.
        if (const auto* c = std::get_if<lir::PushConstant>(&i))
            return ctx.constants(c->value).get_if<ir::SymSCClassName>() != nullptr;
        return false;
    }

    // The function's argument k, as the old compiler compared names with argNames, where a method's first is `this`.
    [[nodiscard]] bool is_argument(Value v, lir::SlotIndex k) const {
        if (!v.pushed_by_one_instruction())
            return false;
        if (frame.has_this && *k == 0)
            return is_this(v);
        const auto* local = std::get_if<lir::PushLocal>(&instruction(v.producer));
        return local && local->local.depth == 0 && local->local.slot == k;
    }

    [[nodiscard]] bool is_integer_one(Value v) const {
        if (!v.pushed_by_one_instruction())
            return false;
        const auto& i = instruction(v.producer);
        if (const auto* p = std::get_if<lir::PushInteger>(&i))
            return p->value == 1;
        if (const auto* c = std::get_if<lir::PushConstant>(&i))
            if (const auto* n = ctx.constants(c->value).get_if<std::int32_t>())
                return *n == 1;
        return false;
    }

    enum struct PushAll : std::uint8_t { No, All, AllButFirst };

    // checkPushAllArgs.
    [[nodiscard]] PushAll push_all_args(SendOperands args, std::uint32_t n) const {
        if (!is_name(args[receiver_operand])) {
            if (n < 3)
                return PushAll::No;
            for (const auto k : operands(util::advance(receiver_operand), n))
                if (!is_argument(args[k], argument_slot(k)))
                    return PushAll::No;
            return PushAll::AllButFirst;
        }
        for (const auto k : operands(receiver_operand, n))
            if (!is_argument(args[k], argument_slot(k)))
                return PushAll::No;
        return PushAll::All;
    }

    // checkPushAllButFirstTwoArgs, the arguments after the first two are the function's after its first.
    [[nodiscard]] bool push_all_but_first_two_args(SendOperands args, std::uint32_t num_block) const {
        if (num_block < 2)
            return false;
        for (const auto k : operands(util::advance(receiver_operand), num_block))
            if (!is_argument(args[util::advance(k)], argument_slot(k)))
                return false;
        return true;
    }

    // Decides one send, see PyrCallNode::compileCall, PyrBinopCallNode::compileCall and PyrSetterNode::compileCall.
    SendPlan decide(BodyPlan& plan, const lir::Send& s, SendOperands args) {
        SendPlan p;
        const std::uint32_t n = 1u + s.num_positional;
        const std::uint32_t k = s.num_keyword;
        if (n + 2 * k > max_argument_count) {
            const auto loc = owner_location();
            report(sc::diag::Diagnostic::too_many_arguments(ctx.graph.text_info(loc.source), ir::IRGraph::range(loc),
                                                            n + 2 * k, max_argument_count));
            // The function isn't emitted.
            p.op = SendOp::None;
            return p;
        }
        p.num_args = static_cast<std::uint8_t>(n);
        p.num_keyword = static_cast<std::uint8_t>(k);
        p.tail = s.tail != lir::Tail::None;
        const bool super = s.kind == lir::SendKind::Super;
        std::optional<SpecialSymbols::SpecialSelector> special;
        // A super send always looks its selector up.
        if (!super)
            if (const auto fnd = ctx.special_symbols.special_selectors.find(ir::Symbol { *s.selector });
                fnd != ctx.special_symbols.special_selectors.end())
                special = fnd->second;
        if (special)
            p.special = *special;
        const auto* special_selector = special ? std::get_if<OpSpecialSelectors>(&*special) : nullptr;
        const auto* unary = special ? std::get_if<OpUnaryMath>(&*special) : nullptr;
        const auto* binary = special ? std::get_if<OpBinaryMath>(&*special) : nullptr;
        const auto adds_or_subtracts_one = [&]() {
            return binary && (*binary == OpBinaryMath::Add || *binary == OpBinaryMath::Sub)
                && is_integer_one(args[util::advance(receiver_operand)]);
        };
        const auto leave_out_arg = [&](OperandIndex a) { plan.leave_out(args[a].producer); };

        switch (s.form) {
        case lir::SendForm::Implicit:
            p.tail = false;
            if (special_selector) {
                p.op = SendOp::Special;
            } else {
                p.op = SendOp::Message;
                p.add = AddSelector::AtReceiver;
            }
            return p;

        case lir::SendForm::Setter:
            p.add = AddSelector::AtSend;
            p.op = super ? SendOp::Super : SendOp::Message;
            return p;

        case lir::SendForm::Binary:
            // A unary operator written as a binary has an argument, so it is sent, `a reciprocal: b` is
            // `a.reciprocal(b)`.
            if (super || !special || unary) {
                p.add = AddSelector::AtReceiver;
                p.op = super ? SendOp::Super : SendOp::Message;
            } else if (special_selector) {
                p.op = SendOp::Special;
            } else if (n == 3) {
                p.op = SendOp::BinaryWithAdverb;
            } else if (adds_or_subtracts_one()) {
                // The old compiler didn't emit a tail call before PushOneAndAddOne.
                const bool add = *binary == OpBinaryMath::Add;
                p.op = add ? SendOp::PushOneAndAddOne : SendOp::PushOneAndSubtract;
                p.tail = p.tail && !add;
                leave_out_arg(util::advance(receiver_operand));
            } else {
                p.op = SendOp::Binary;
            }
            return p;

        case lir::SendForm::Call:
            break;
        }

        const bool is_switch_or_case = special_selector
            && (*special_selector == OpSpecialSelectors::Switch || *special_selector == OpSpecialSelectors::Case);
        if (k > 0 || (n > 15 && !is_switch_or_case)) {
            if (super || !special) {
                p.add = AddSelector::AtReceiver;
                p.op = super ? SendOp::SuperX : SendOp::MessageX;
            } else if (special_selector) {
                p.op = SendOp::SpecialX;
            } else {
                // An operator sent with keyword arguments is a message, its selector added after the arguments.
                p.add = AddSelector::AtSend;
                p.op = SendOp::MessageX;
            }
            return p;
        }

        if (super) {
            p.add = AddSelector::AtReceiver;
            if (n == 1) {
                p.op = SendOp::SuperThisOpt;
                leave_out_arg(receiver_operand);
            } else {
                p.op = SendOp::Super;
            }
            return p;
        }

        // A message, also an operator sent with the wrong number of arguments.
        const auto message = [&]() {
            p.add = AddSelector::AtReceiver;
            if (n == 1 && is_this(args[receiver_operand])) {
                p.op = SendOp::MessageThisOpt;
                leave_out_arg(receiver_operand);
            } else {
                p.op = SendOp::Message;
            }
        };

        if (!special) {
            message();
            if (n > 1 && n == num_block_args()) {
                const auto all = push_all_args(args, n);
                if (all != PushAll::No) {
                    p.op = all == PushAll::All ? SendOp::PushAllArgs : SendOp::PushAllButFirstArg;
                    for (const auto a :
                         operands(all == PushAll::All ? receiver_operand : util::advance(receiver_operand), n))
                        leave_out_arg(a);
                }
            } else if (n > 2 && n == num_block_args() + 1 && push_all_but_first_two_args(args, num_block_args())) {
                p.op = SendOp::PushAllButFirstTwoArgs;
                for (const auto a : operands(util::advance(receiver_operand, 2), n))
                    leave_out_arg(a);
            }
            return p;
        }

        if (special_selector) {
            p.op = SendOp::Special;
            // Sent by the old compiler's inlining when it couldn't inline them, without the peepholes.
            if (sent_plainly(*special_selector))
                return p;
            if (n == 1) {
                if (is_this(args[receiver_operand])) {
                    p.op = SendOp::SpecialThisOpt;
                    leave_out_arg(receiver_operand);
                } else if (args[receiver_operand].pushed_by_one_instruction()) {
                    if (const auto* instance =
                            std::get_if<lir::PushInstance>(&instruction(args[receiver_operand].producer));
                        instance && *instance->slot <= std::numeric_limits<std::uint8_t>::max()) {
                        p.op = SendOp::PushInstVarAndSpecial;
                        p.instance = instance->slot;
                        leave_out_arg(receiver_operand);
                    }
                }
            } else if (*special_selector == OpSpecialSelectors::Do) {
                // `(a..b).do(f)` is `a.forSeries(nil, b, f)`, the range's arguments are the first.
                const auto* series = std::get_if<lir::Send>(&instruction(args[receiver_operand].producer));
                if (series && series->selector == ctx.special_symbols.series && series->form == lir::SendForm::Call
                    && series->kind == lir::SendKind::Normal && series->num_keyword == 0) {
                    plan.leave_out(args[receiver_operand].producer);
                    p.special = OpSpecialSelectors::ForSeries;
                    p.num_args = static_cast<std::uint8_t>(1 + series->num_positional + n - 1);
                } else if (n == num_block_args()) {
                    special_push_all(p, args, n, leave_out_arg);
                }
            } else if (n == num_block_args()) {
                // The old compiler also tried the function's arguments after its first two here, but checked them as
                // all of its arguments, so it never matched.
                special_push_all(p, args, n, leave_out_arg);
            }
            return p;
        }

        if (unary) {
            if (n == 1)
                p.op = SendOp::UnaryX;
            else
                message();
            return p;
        }

        if (n != 2) {
            message();
        } else if (adds_or_subtracts_one()) {
            // Written as a call, the old compiler didn't emit a tail call before either.
            p.op = *binary == OpBinaryMath::Add ? SendOp::PushOneAndAddOne : SendOp::PushOneAndSubtract;
            p.tail = false;
            leave_out_arg(util::advance(receiver_operand));
        } else {
            p.op = SendOp::Binary;
        }
        return p;
    }

    // The selectors the old compiler could inline, see Inlined, which it otherwise sent with SendSpecialMsg.
    [[nodiscard]] static bool sent_plainly(OpSpecialSelectors s) {
        switch (s) {
        case OpSpecialSelectors::If:
        case OpSpecialSelectors::While:
        case OpSpecialSelectors::And:
        case OpSpecialSelectors::Or:
        case OpSpecialSelectors::Case:
        case OpSpecialSelectors::Switch:
        case OpSpecialSelectors::Loop:
        case OpSpecialSelectors::QuestionMark:
        case OpSpecialSelectors::DoubleQuestionMark:
        case OpSpecialSelectors::ExclamationQuestionMark:
            return true;
        default:
            return false;
        }
    }

    template <typename LeaveOut>
    void special_push_all(SendPlan& p, SendOperands args, std::uint32_t n, const LeaveOut& leave_out_arg) const {
        if (n <= 1)
            return;
        const auto all = push_all_args(args, n);
        if (all == PushAll::No)
            return;
        p.op = all == PushAll::All ? SendOp::PushAllArgsSpecial : SendOp::PushAllButFirstArgSpecial;
        for (const auto a : operands(all == PushAll::All ? receiver_operand : util::advance(receiver_operand), n))
            leave_out_arg(a);
    }

    // Reads a body's instructions in order: which instructions produce each value on the stack, and every send's
    // operands.
    struct StackReader {
        using BodyOperandIndex = util::StrongIndex<std::uint32_t, struct BodyOperandIndex__>;
        struct SendArgs {
            lir::InstructionIndex instruction;
            BodyOperandIndex first;
            std::uint32_t count;
        };

        const Emitter& emitter;
        BodyPlan& plan;
        lir::InstructionIndex at { 0 };
        std::vector<Value> stack {};
        // Every send's operands, one after another.
        util::IndexedVector<BodyOperandIndex, Value> operands {};
        util::IndexedVector<SendIndex, SendArgs> sends {};

        void read() {
            for (const auto i : lir::instructions(plan.body)) {
                at = i;
                std::visit(*this, emitter.instruction(i));
            }
        }

        void operator()(const lir::Send& s) {
            const std::uint32_t count = 1u + s.num_positional + 2u * s.num_keyword;
            assert(stack.size() >= count);
            (void)sends.push_back({ at, operands.next_index(), count });
            operands.raw().insert(operands.raw().end(), stack.end() - static_cast<std::ptrdiff_t>(count), stack.end());
            replace(count);
        }
        void operator()(const lir::StoreLocal&) { stack.back().producer = at; }
        void operator()(const lir::StoreInstance&) { stack.back().producer = at; }
        void operator()(const lir::StoreClassVariable&) { stack.back().producer = at; }
        void operator()(const lir::Drop&) { (void)pop(); }
        void operator()(const lir::Return&) { return_value(pop()); }
        void operator()(const lir::NonLocalReturn&) { return_value(pop()); }
        void operator()(const lir::ReturnSelf&) {}
        void operator()(const lir::DefaultArgument&) {}
        void operator()(const lir::If&) { replace(1); }
        void operator()(const lir::IfNil&) { replace(1); }
        void operator()(const lir::IfNotNil&) { replace(1); }
        void operator()(const lir::And&) { replace(1); }
        void operator()(const lir::Or&) { replace(1); }
        void operator()(const lir::OrIfNil&) { replace(1); }
        void operator()(const lir::AndIfNotNil&) { replace(1); }
        void operator()(const lir::QuestionMark&) { replace(2); }
        void operator()(const lir::Switch&) { replace(1); }
        void operator()(const lir::Unimplemented&) {}
        // The pushes, While and Loop.
        template <typename Push> void operator()(const Push&) { stack.push_back({ at, at }); }

        Value pop() {
            assert(!stack.empty());
            const auto v = stack.back();
            stack.pop_back();
            return v;
        }

        // The top count values become one, produced here.
        void replace(std::size_t count) {
            assert(stack.size() >= count);
            const auto start = stack[stack.size() - count].start;
            stack.erase(stack.end() - static_cast<std::ptrdiff_t>(count), stack.end());
            stack.push_back({ start, at });
        }

        // `^nil`, `^true` and `^false` are one bytecode, when the value is the literal.
        void return_value(Value v) {
            if (v.pushed_by_one_instruction() && emitter.special_value(v.producer))
                plan.leave_out(v.producer);
        }
    };

    // Reads the body's stack, then decides its sends from the last, so a send that absorbs the one computing its
    // receiver, e.g., forSeries, decides first, and the outer of two sends adding a selector at the same instruction
    // adds it first.
    BodyPlan plan_body(lir::Body body) {
        BodyPlan plan { body };
        StackReader reader { *this, plan };
        reader.read();
        const auto& sends = reader.sends;

        plan.sends.resize(sends.size());
        for (const auto& s : sends.raw())
            (void)plan.send_instructions.push_back(s.instruction);
        for (const auto s : sends.indices().reversed()) {
            const auto i = sends[s].instruction;
            // Absorbed by a later send, which encodes it.
            if (plan.left_out(i)) {
                plan.sends[s].op = SendOp::None;
                continue;
            }
            const auto& send = std::get<lir::Send>(instruction(i));
            const SendOperands args { &reader.operands[sends[s].first], sends[s].count };
            plan.sends[s] = decide(plan, send, args);
            if (plan.sends[s].add == AddSelector::AtReceiver)
                plan.additions.push_back({ args[receiver_operand].start, s });
            else if (plan.sends[s].add == AddSelector::AtSend)
                plan.additions.push_back({ i, s });
        }
        std::stable_sort(plan.additions.begin(), plan.additions.end(),
                         [](const auto& l, const auto& r) { return *l.first < *r.first; });
        return plan;
    }

    [[nodiscard]] std::optional<lir::SpecialValue> special_value(lir::InstructionIndex i) const {
        const auto& in = instruction(i);
        if (const auto* p = std::get_if<lir::PushSpecialValue>(&in))
            return p->value;
        if (const auto* c = std::get_if<lir::PushConstant>(&in)) {
            const auto& value = ctx.constants(c->value);
            if (value.get_if<ir::Nil>())
                return lir::SpecialValue::Nil;
            if (const auto* b = value.get_if<bool>())
                return *b ? lir::SpecialValue::True : lir::SpecialValue::False;
        }
        return std::nullopt;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Encoding
    ////////////////////////////////////////////////////////////////////////////////

    static void push_integer(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, std::int32_t value) {
        if (value == -1)
            PushSpecialNumber.emit(bc, loc, { OpSpecialNumbers::MinusOne });
        else if (value == 0)
            PushSpecialNumber.emit(bc, loc, { OpSpecialNumbers::Zero });
        else if (value == 1)
            PushSpecialNumber.emit(bc, loc, { OpSpecialNumbers::One });
        else if (value == 2)
            PushSpecialNumber.emit(bc, loc, { OpSpecialNumbers::Two });
        else if (value >= -(1 << 7) && value <= (1 << 7) - 1)
            PushInteger8.emit(bc, loc, Operands::Int<8, 0>::fromFull(value));
        else if (value >= -(1 << 15) && value <= (1 << 15) - 1)
            PushInteger16.emit(bc, loc, Operands::Int<16, 1>::fromFull(value), Operands::Int<16, 0>::fromFull(value));
        else if (value >= -(1 << 23) && value <= (1 << 23) - 1)
            PushInteger24.emit(bc, loc, Operands::Int<24, 2>::fromFull(value), Operands::Int<24, 1>::fromFull(value),
                               Operands::Int<24, 0>::fromFull(value));
        else
            PushInteger32.emit(bc, loc, Operands::Int<32, 3>::fromFull(value), Operands::Int<32, 2>::fromFull(value),
                               Operands::Int<32, 1>::fromFull(value), Operands::Int<32, 0>::fromFull(value));
    }

    void push_constant_entry(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, ConstantContent entry) {
        const std::uint32_t index { *add_constant(std::move(entry)) };
        if (index < (1u << 4))
            PushLiteral.emit(bc, loc, index);
        else if (index < (1u << 8))
            PushConstant8.emit(bc, loc, Operands::UnsignedInt<8, 0>::fromRaw(static_cast<Byte>(index)));
        else if (index < (1u << 16))
            PushConstant16.emit(bc, loc, Operands::UnsignedInt<16, 1>::fromFull(index),
                                Operands::UnsignedInt<16, 0>::fromFull(index));
        else if (index < (1u << 24))
            PushConstant24.emit(bc, loc, Operands::UnsignedInt<24, 2>::fromFull(index),
                                Operands::UnsignedInt<24, 1>::fromFull(index),
                                Operands::UnsignedInt<24, 0>::fromFull(index));
        else
            PushConstant32.emit(
                bc, loc, Operands::UnsignedInt<32, 3>::fromFull(index), Operands::UnsignedInt<32, 2>::fromFull(index),
                Operands::UnsignedInt<32, 1>::fromFull(index), Operands::UnsignedInt<32, 0>::fromFull(index));
    }

    void push_class(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, ir::SymSCClassName name) {
        if (const auto fnd = ctx.special_symbols.special_classes.find(ir::Symbol { *name });
            fnd != ctx.special_symbols.special_classes.end())
            PushSpecialClass.emit(bc, loc, Operands::SpecialClass { fnd->second });
        else
            PushClassX.emit(bc, loc, Operands::Class::fromRaw(*add_selector(name)));
    }

    // As PyrSlotNode::compilePushLit, the values with their own bytecodes first.
    void push_constant(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, ir::ConstExprIndex c) {
        const auto& value = ctx.constants(c);
        if (const auto* i = value.get_if<std::int32_t>()) {
            push_integer(bc, loc, *i);
        } else if (value.get_if<ir::Nil>()) {
            PushSpecialValue.emit(bc, loc, { OpSpecialValue::Nil_ });
        } else if (const auto* b = value.get_if<bool>()) {
            PushSpecialValue.emit(bc, loc, { *b ? OpSpecialValue::True : OpSpecialValue::False });
        } else if (const auto* d = value.get_if<double>()) {
            // Compared as numbers, as the old compiler did, so -0.0 is 0.0.
            if (*d == 0.5)
                PushSpecialNumber.emit(bc, loc, { OpSpecialNumbers::Half });
            else if (*d == -1.0)
                PushSpecialNumber.emit(bc, loc, { OpSpecialNumbers::MinusOneFloat });
            else if (*d == 0.0)
                PushSpecialNumber.emit(bc, loc, { OpSpecialNumbers::ZeroFloat });
            else if (*d == 1.0)
                PushSpecialNumber.emit(bc, loc, { OpSpecialNumbers::OneFloat });
            else if (*d == 2.0)
                PushSpecialNumber.emit(bc, loc, { OpSpecialNumbers::TwoFloat });
            else if (*d == std::numeric_limits<double>::infinity())
                PushSpecialValue.emit(bc, loc, { OpSpecialValue::Inf });
            else
                push_constant_entry(bc, loc, c);
        } else if (const auto* name = value.get_if<ir::SymSCClassName>()) {
            push_class(bc, loc, *name);
        } else {
            push_constant_entry(bc, loc, c);
        }
    }

    void push_local(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, lir::LocalRef l) {
        if (l.depth == 0) {
            if (PushTempZeroVar.validNibble(*l.slot))
                PushTempZeroVar.emit(bc, loc, *l.slot);
            else
                PushTempZeroVarX.emit(bc, loc, Operands::Index::fromRaw(*l.slot));
        } else if (PushTempVar.validNibble(l.depth)) {
            PushTempVar.emit(bc, loc, l.depth, Operands::Index::fromRaw(*l.slot));
        } else {
            // Nesting is limited well below 255, see max_nesting_depth.
            assert(l.depth <= std::numeric_limits<std::uint8_t>::max());
            PushTempVarX.emit(bc, loc, Operands::FrameOffset::fromRaw(static_cast<Byte>(l.depth)),
                              Operands::Index::fromRaw(*l.slot));
        }
    }

    // Each returns whether it popped the value, which only the short bytecodes can.
    bool store(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, const lir::StoreLocal& s, bool drop) {
        const auto [depth, slot] = s.local;
        assert(depth <= std::numeric_limits<std::uint8_t>::max());
        if (drop && *slot <= 15 && depth < 8) {
            StoreTempVar.emit(bc, loc, depth, Operands::Index::fromRaw(*slot));
            return true;
        }
        StoreTempVarX.emit(bc, loc, Operands::FrameOffset::fromRaw(static_cast<Byte>(depth)),
                           Operands::Index::fromRaw(*slot));
        return false;
    }

    bool store(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, const lir::StoreInstance& s, bool drop) {
        const auto slot = *s.slot;
        if (drop && slot <= 15) {
            StoreInstVar.emit(bc, loc, slot);
            return true;
        }
        StoreInstVarX.emit(bc, loc, Operands::Index::fromRaw(static_cast<int>(slot)));
        return false;
    }

    bool store(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, const lir::StoreClassVariable& s, bool drop) {
        const std::uint32_t index { *s.index };
        if (drop && index < 4096) {
            StoreClassVar.emit(bc, loc, index);
            return true;
        }
        StoreClassVarX.emit(bc, loc, Operands::UnsignedInt<16, 1>::fromFull(index),
                            Operands::UnsignedInt<16, 0>::fromFull(index));
        return false;
    }

    void send(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, const lir::Send& s, const SendPlan& p) {
        if (p.tail) {
            if (s.tail == lir::Tail::ReturnFromMethod)
                TailCallReturnFromMethod.emit(bc, loc);
            else
                TailCallReturnFromFunction.emit(bc, loc);
        }
        const auto n = p.num_args;
        const auto selector = Operands::SelectorIndex::fromRaw(*p.selector);
        const auto special_index = [&]() { return static_cast<Byte>(std::get<OpSpecialSelectors>(p.special)); };
        const auto unary = [&]() { return std::get<OpUnaryMath>(p.special); };
        const auto binary = [&]() { return std::get<OpBinaryMath>(p.special); };
        const auto with_keywords = Operands::ArgumentCount::fromRaw(p.num_args + 2 * p.num_keyword);
        const auto keywords = Operands::KwArgumentCount::fromRaw(p.num_keyword);
        switch (p.op) {
        case SendOp::None:
            return;
        case SendOp::Message:
            if (SendMsg.validNibble(n))
                SendMsg.emit(bc, loc, n, selector);
            else
                SendMsgX.emit(bc, loc, Operands::ArgumentCount::fromRaw(n), Operands::KwArgumentCount::fromRaw(0),
                              selector);
            return;
        case SendOp::MessageX:
            SendMsgX.emit(bc, loc, with_keywords, keywords, selector);
            return;
        case SendOp::MessageThisOpt:
            SendMsgThisOpt.emit(bc, loc, selector);
            return;
        case SendOp::Super:
            if (SendSuperMsg.validNibble(n))
                SendSuperMsg.emit(bc, loc, n, selector);
            else
                SendSuperMsgX.emit(bc, loc, Operands::ArgumentCount::fromRaw(n), Operands::KwArgumentCount::fromRaw(0),
                                   selector);
            return;
        case SendOp::SuperX:
            SendSuperMsgX.emit(bc, loc, with_keywords, keywords, selector);
            return;
        case SendOp::SuperThisOpt:
            SendSuperMsgThisOpt.emit(bc, loc, selector);
            return;
        case SendOp::Special:
            if (SendSpecialMsg.validNibble(n))
                SendSpecialMsg.emit(bc, loc, n, { std::get<OpSpecialSelectors>(p.special) });
            else
                SendSpecialMsgX.emit(bc, loc, Operands::ArgumentCount::fromRaw(n),
                                     Operands::KwArgumentCount::fromRaw(0), Operands::Index::fromRaw(special_index()));
            return;
        case SendOp::SpecialX:
            SendSpecialMsgX.emit(bc, loc, with_keywords, keywords, Operands::Index::fromRaw(special_index()));
            return;
        case SendOp::SpecialThisOpt:
            SendSpecialMsgThisOpt.emit(bc, loc, Operands::Index::fromRaw(special_index()));
            return;
        case SendOp::PushInstVarAndSpecial:
            PushInstVarAndSendSpecialMsg.emit(bc, loc, Operands::Index::fromRaw(static_cast<Byte>(*p.instance)),
                                              Operands::Index::fromRaw(special_index()));
            return;
        case SendOp::PushAllArgs:
            PushAllArgsAndSendMsg.emit(bc, loc, selector);
            return;
        case SendOp::PushAllButFirstArg:
            PushAllButFirstArgAndSendMsg.emit(bc, loc, selector);
            return;
        case SendOp::PushAllButFirstTwoArgs:
            PushAllButFirstTwoArgsAndSendMsg.emit(bc, loc, selector);
            return;
        case SendOp::PushAllArgsSpecial:
            PushAllArgsAndSendSpecialMsg.emit(bc, loc, Operands::Index::fromRaw(special_index()));
            return;
        case SendOp::PushAllButFirstArgSpecial:
            PushAllButFirstArgAndSendSpecialMsg.emit(bc, loc, Operands::Index::fromRaw(special_index()));
            return;
        case SendOp::UnaryX:
            SendSpecialUnaryArithMsgX.emit(bc, loc, { unary() });
            return;
        case SendOp::Binary:
            if (static_cast<Byte>(binary()) < static_cast<Byte>(OpBinaryMathNibble::COUNT))
                SendSpecialBinaryArithMsg.emit(bc, loc, { static_cast<OpBinaryMathNibble>(binary()) });
            else
                SendSpecialBinaryArithMsgX.emit(bc, loc, { binary() });
            return;
        case SendOp::BinaryWithAdverb:
            // The operand is the binary operator's index, not a trinary operator.
            bc.emit(loc, SpecialBinaryOpWithAdverb.code, static_cast<Byte>(binary()));
            return;
        case SendOp::PushOneAndAddOne:
            PushOneAndAddOne.emit(bc, loc);
            return;
        case SendOp::PushOneAndSubtract:
            PushOneAndSubtract.emit(bc, loc);
            return;
        }
    }

    void special(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, ir::SCSpecial::Kind kind) {
        switch (kind) {
        case ir::SCSpecial::Kind::This:
        case ir::SCSpecial::Kind::Super:
            PushSpecialValueThis.emit(bc, loc);
            return;
        case ir::SCSpecial::Kind::Process:
            SpecialOpcode.emit(bc, loc, { OpPseudoVarEnum::Process });
            return;
        case ir::SCSpecial::Kind::Thread:
            SpecialOpcode.emit(bc, loc, { OpPseudoVarEnum::Thread });
            return;
        case ir::SCSpecial::Kind::Method:
            SpecialOpcode.emit(bc, loc, { OpPseudoVarEnum::Method });
            return;
        case ir::SCSpecial::Kind::FunctionDef:
            SpecialOpcode.emit(bc, loc, { OpPseudoVarEnum::FunctionDef });
            return;
        case ir::SCSpecial::Kind::Function:
            SpecialOpcode.emit(bc, loc, { OpPseudoVarEnum::Function });
            return;
        }
    }

    // As PyrVarDefNode::compileArg: if the argument isn't nil, jump over the code computing its default.
    void default_argument(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, const lir::DefaultArgument& d) {
        push_local(bc, loc, { 0, d.slot });
        auto value = encode(function.bodies[d.value]);
        StoreTempVarX.emit(value, loc, Operands::FrameOffset::fromRaw(0), Operands::Index::fromRaw(*d.slot));
        const auto length = value.length();
        if (length > max_jump) {
            const auto owner = owner_location();
            report(sc::diag::Diagnostic::jump_too_long(ctx.graph.text_info(owner.source), ir::IRGraph::range(owner),
                                                       length, max_jump));
            return;
        }
        const auto jump = static_cast<std::uint32_t>(length);
        Extended::IfNotNilJumpPushNilElsePop.emit(
            bc, loc, { Operands::UnsignedInt<16, 1>::fromFull(jump), Operands::UnsignedInt<16, 0>::fromFull(jump) });
        bc.consume(std::move(value));
        Drop.emit(bc, loc);
    }

    // Encodes a body, an instruction at a time: first the selectors the old compiler added before compiling the
    // receivers starting there, then the instruction itself.
    struct BodyEncoder {
        Emitter& emitter;
        BodyPlan plan;
        CompilingBytecodes bc {};
        lir::InstructionIndex at { 0 };
        sc::src::SourceCodeRange loc {};
        SendIndex next_send { 0 };
        std::vector<std::pair<lir::InstructionIndex, SendIndex>>::const_iterator next_addition {
            plan.additions.begin()
        };
        // A short store popped its value, so the Drop following it is already encoded.
        bool dropped_by_store { false };

        CompilingBytecodes encode() {
            for (const auto i : lir::instructions(plan.body)) {
                at = i;
                loc = emitter.location(i);
                add_receiver_selectors();
                if (!encoded_elsewhere())
                    std::visit(*this, emitter.instruction(i));
            }
            return std::move(bc);
        }

        // Selectors added at the send, after its arguments, are added when it is encoded.
        void add_receiver_selectors() {
            for (; next_addition != plan.additions.end() && next_addition->first == at; ++next_addition) {
                const auto s = next_addition->second;
                if (plan.sends[s].add == AddSelector::AtReceiver)
                    plan.sends[s].selector = emitter.add_selector(
                        std::get<lir::Send>(emitter.instruction(plan.send_instructions[s])).selector);
            }
        }

        // Absorbed by another instruction, or the Drop a store popped.
        bool encoded_elsewhere() {
            if (plan.left_out(at))
                return true;
            if (!dropped_by_store)
                return false;
            dropped_by_store = false;
            assert(std::holds_alternative<lir::Drop>(emitter.instruction(at)));
            return true;
        }

        void operator()(const lir::PushLocal& p) { emitter.push_local(bc, loc, p.local); }
        void operator()(const lir::PushInstance& p) {
            PushInstVarX.emit(bc, loc, Operands::Index::fromRaw(static_cast<int>(*p.slot)));
        }
        void operator()(const lir::PushClassVariable& p) {
            const std::uint32_t index { *p.index };
            if (PushClassVar.validNibble(index))
                PushClassVar.emit(bc, loc, index);
            else
                PushClassVarX.emit(bc, loc, Operands::UnsignedInt<16, 1>::fromFull(index),
                                   Operands::UnsignedInt<16, 0>::fromFull(index));
        }
        void operator()(const lir::PushConstant& p) { emitter.push_constant(bc, loc, p.value); }
        void operator()(const lir::PushClassConstant& p) {
            const auto& member = emitter.ctx.graph.member(p.member);
            emitter.push_constant_entry(bc, loc, ClassConstant { member.owner, member.name, p.value });
        }
        void operator()(const lir::PushSpecialValue& p) {
            PushSpecialValue.emit(bc, loc,
                                  { p.value == lir::SpecialValue::Nil        ? OpSpecialValue::Nil_
                                        : p.value == lir::SpecialValue::True ? OpSpecialValue::True
                                                                             : OpSpecialValue::False });
        }
        void operator()(const lir::PushInteger& p) { emitter.push_integer(bc, loc, p.value); }
        void operator()(const lir::PushSymbol& p) { emitter.push_constant_entry(bc, loc, p.value); }
        void operator()(const lir::PushClass& p) { emitter.push_class(bc, loc, p.name); }
        void operator()(const lir::PushSpecial& p) { emitter.special(bc, loc, p.kind); }
        void operator()(const lir::PushFunction& p) {
            PushLiteralX.emit(bc, loc, Operands::Index::fromRaw(*emitter.add_selector(p.function)));
        }
        void operator()(const lir::StoreLocal& s) { dropped_by_store = emitter.store(bc, loc, s, drop_follows()); }
        void operator()(const lir::StoreInstance& s) { dropped_by_store = emitter.store(bc, loc, s, drop_follows()); }
        void operator()(const lir::StoreClassVariable& s) {
            dropped_by_store = emitter.store(bc, loc, s, drop_follows());
        }
        void operator()(const lir::Dup&) { Dup.emit(bc, loc); }
        void operator()(const lir::Drop&) { Drop.emit(bc, loc); }
        void operator()(const lir::Send& s) {
            // Sends are numbered in order, including those left out, which aren't visited.
            while (plan.send_instructions[next_send] != at)
                next_send = util::advance(next_send);
            auto& p = plan.sends[next_send];
            if (p.add == AddSelector::AtSend)
                p.selector = emitter.add_selector(s.selector);
            emitter.send(bc, loc, s, p);
        }
        void operator()(const lir::Return&) { encode_return(); }
        void operator()(const lir::NonLocalReturn&) { encode_return(); }
        void operator()(const lir::ReturnSelf&) { ReturnSelf.emit(bc, loc); }
        void operator()(const lir::DefaultArgument& d) { emitter.default_argument(bc, loc, d); }
        void operator()(const lir::If& b) {
            emitter.branch(bc, loc, b.then_body, b.else_body, JumpIfFalsePushNil, JumpIfFalse);
        }
        void operator()(const lir::IfNil& b) {
            emitter.branch(bc, loc, b.then_body, b.else_body, Extended::IfNotNilJumpPushNilElsePop.Jump,
                           Extended::IfNotNilJump.Jump);
        }
        void operator()(const lir::IfNotNil& b) {
            emitter.branch(bc, loc, b.then_body, b.else_body, Extended::IfNilThenJumpElsePopNil.Jump,
                           Extended::IfNilJump.Jump);
        }
        void operator()(const lir::And& b) { emitter.jump_over(bc, loc, JumpIfFalsePushFalse, b.rhs); }
        void operator()(const lir::Or& b) { emitter.jump_over(bc, loc, JumpIfTruePushTrue, b.rhs); }
        void operator()(const lir::OrIfNil& b) { emitter.jump_over(bc, loc, Extended::DoubleQuestionMark.Jump, b.rhs); }
        void operator()(const lir::AndIfNotNil& b) {
            emitter.jump_over(bc, loc, Extended::IfNilThenJumpElsePopNil.Jump, b.rhs);
        }
        void operator()(const lir::QuestionMark&) { Extended::QuestionMark.emit(bc, loc); }
        void operator()(const lir::While& w) { emitter.while_(bc, loc, w); }
        void operator()(const lir::Loop& l) {
            auto body = emitter.encode_value(l.body, loc);
            const auto length = body.length();
            bc.consume(std::move(body));
            emitter.jump(bc, loc, JumpBack, length + 1);
        }
        void operator()(const lir::Switch& s) { emitter.switch_(bc, loc, s); }
        void operator()(const lir::Unimplemented&) {
            assert(false);
            emitter.failed = true;
        }

        [[nodiscard]] bool drop_follows() const {
            const auto next = util::advance(at);
            return plan.contains(next) && !plan.left_out(next)
                && std::holds_alternative<lir::Drop>(emitter.instruction(next));
        }

        void encode_return() {
            if (at != plan.body.begin && plan.left_out(util::retreat(at)))
                if (const auto value = emitter.special_value(util::retreat(at))) {
                    switch (*value) {
                    case lir::SpecialValue::Nil:
                        ReturnNil.emit(bc, loc);
                        return;
                    case lir::SpecialValue::True:
                        ReturnTrue.emit(bc, loc);
                        return;
                    case lir::SpecialValue::False:
                        ReturnFalse.emit(bc, loc);
                        return;
                    }
                }
            Return.emit(bc, loc);
        }
    };

    CompilingBytecodes encode(lir::Body body) { return BodyEncoder { *this, plan_body(body) }.encode(); }

    ////////////////////////////////////////////////////////////////////////////////
    // Inlined sends, laid out as the old compiler's compileIfMsg and its neighbours did
    ////////////////////////////////////////////////////////////////////////////////

    template <typename Op>
    void jump(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, const Op& op, std::size_t length) {
        if (length > max_jump) {
            const auto owner = owner_location();
            report(sc::diag::Diagnostic::jump_too_long(ctx.graph.text_info(owner.source), ir::IRGraph::range(owner),
                                                       length, max_jump));
            length = 0;
        }
        const auto hi = Operands::UnsignedInt<16, 1>::fromFull(static_cast<std::uint32_t>(length));
        const auto lo = Operands::UnsignedInt<16, 0>::fromFull(static_cast<std::uint32_t>(length));
        op.emit(bc, loc, hi, lo);
    }

    // An empty body is nil. The old compiler left the stack without a value for some, `a and: { }`.
    CompilingBytecodes encode_value(lir::BodyIndex r, sc::src::SourceCodeRange loc) {
        auto bc = encode(function.bodies[r]);
        if (bc.length() == 0)
            PushSpecialValue.emit(bc, loc, { OpSpecialValue::Nil_ });
        return bc;
    }

    // short_jump skips the then body leaving nil, long_jump goes to the else body. The else body is encoded first, as
    // the old compiler did, which orders the tables.
    template <typename Short, typename Long>
    void branch(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, lir::BodyIndex then_body,
                lir::OptionalBodyIndex else_body, const Short& short_jump, const Long& long_jump) {
        std::optional<CompilingBytecodes> else_bc;
        if (else_body)
            else_bc = encode(function.bodies[lir::BodyIndex { *else_body }]);
        auto then_bc = encode(function.bodies[then_body]);
        if (else_bc && else_bc->length() > 0) {
            if (then_bc.length() == 0)
                PushSpecialValue.emit(then_bc, loc, { OpSpecialValue::Nil_ });
            jump(then_bc, loc, JumpFwd, else_bc->length());
            jump(bc, loc, long_jump, then_bc.length());
            bc.consume(std::move(then_bc));
            bc.consume(std::move(*else_bc));
        } else if (then_bc.length() > 0) {
            jump(bc, loc, short_jump, then_bc.length());
            bc.consume(std::move(then_bc));
        } else {
            Drop.emit(bc, loc);
            PushSpecialValue.emit(bc, loc, { OpSpecialValue::Nil_ });
        }
    }

    template <typename Op>
    void jump_over(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, const Op& op, lir::BodyIndex rhs) {
        auto rhs_bc = encode_value(rhs, loc);
        jump(bc, loc, op, rhs_bc.length());
        bc.consume(std::move(rhs_bc));
    }

    // The jump back drops the body's value. The lengths are the old compiler's.
    void while_(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, const lir::While& w) {
        auto condition = encode_value(w.condition, loc);
        CompilingBytecodes body;
        if (w.body)
            body = encode_value(lir::BodyIndex { *w.body }, loc);
        else
            PushSpecialValue.emit(body, loc, { OpSpecialValue::Nil_ });
        const auto condition_length = condition.length();
        const auto body_length = body.length();
        bc.consume(std::move(condition));
        jump(bc, loc, JumpIfFalsePushNil, body_length + 3);
        bc.consume(std::move(body));
        jump(bc, loc, JumpBack, body_length + condition_length + 4);
    }

    // Nil is the default if there isn't one. The table is numbered before the bodies' constants, as the old compiler
    // added it first.
    void switch_(CompilingBytecodes& bc, sc::src::SourceCodeRange loc, const lir::Switch& s) {
        const auto table_index = out.switch_tables.push_back({});
        push_constant_entry(bc, loc, table_index);
        Extended::Switch.emit(bc, loc);

        // Each key's body, then the default, the last without a key.
        struct EncodedBody {
            ir::OptionalConstExprIndex key;
            CompilingBytecodes code;
        };
        std::vector<EncodedBody> bodies;
        for (const auto c : lir::cases(s)) {
            const auto& sc = function.switch_cases[c];
            bodies.push_back({ sc.key, encode_value(sc.body, loc) });
        }
        const bool has_default = !bodies.back().key;
        if (!has_default) {
            bodies.push_back({ {}, {} });
            PushSpecialValue.emit(bodies.back().code, loc, { OpSpecialValue::Nil_ });
        }
        const auto keyed_end = std::prev(bodies.end());
        const std::size_t num_keys = bodies.size() - 1;
        // Each keyed body jumps to the end.
        constexpr std::size_t jump_size { 3 };
        std::size_t end { num_keys * jump_size };
        for (const auto& body : bodies)
            end += body.code.length();

        // The receiver, then a key and a body each, then the default, as the old compiler sized it.
        const std::size_t num_args = 1 + 2 * num_keys + (has_default ? 1 : 0);
        auto& table = out.switch_tables[table_index];
        table.capacity = 1;
        while (table.capacity < 2 * num_args)
            table.capacity *= 2;
        // Where each body starts, counted from after the Switch bytecode.
        CodeOffset offset { 0 };
        for (auto body = bodies.begin(); body != keyed_end; ++body) {
            const ir::ConstExprIndex key { *body->key };
            // The first of equal keys is kept, as Object:switch tests them in order. The old compiler kept the last.
            const auto identity = identity_of(ctx.constants, key);
            if (std::none_of(table.offsets.begin(), table.offsets.end(),
                             [&](const auto& o) { return identity_of(ctx.constants, o.first) == identity; }))
                table.offsets.push_back({ key, offset });
            offset = util::advance(offset, static_cast<std::uint32_t>(body->code.length() + jump_size));
            jump(body->code, loc, JumpFwd, end - *offset);
        }
        table.default_offset = offset;
        for (auto& body : bodies)
            bc.consume(std::move(body.code));
    }

    // A function, not a method, ends with BlockReturn, after its body leaves its value on the stack.
    void run(bool is_method) {
        auto bc = encode(function.bodies[lir::function_body]);
        if (!is_method) {
            const auto range = ir::IRGraph::range(owner_location());
            BlockReturn.emit(bc, { { range.end.absolute, 0, 0 }, { range.end.absolute, 0, 0 } });
        }
        out.code = std::move(bc).finish();
    }
};

////////////////////////////////////////////////////////////////////////////////
// Function properties
////////////////////////////////////////////////////////////////////////////////

// What a function and those nested in it use of the frames around it.
struct Uses {
    // How many frames out the furthest variable used is.
    std::uint32_t outer_depth { 0 };
    // `this`, `super`, `thisFunction`, `^` or a member.
    bool cant_be_closed { false };
};

bool explicitly_closed(const EmitContext& ctx, const Frame& frame) {
    if (const auto f = ctx.graph.as_a<ir::SCFunction_I>(frame.owner))
        return ctx.graph.payload(*f).closed || !frame.parent;
    return false;
}

bool members_close(const EmitContext& ctx, const Frame& frame) {
    if (ctx.interpreter_members)
        return true;
    for (const Frame* f = &frame;; f = &ctx.frames.frames[FrameIndex { *f->parent }]) {
        if (explicitly_closed(ctx, *f))
            return true;
        if (!f->parent)
            return false;
    }
}

util::IndexedVector<lir::FunctionIndex, FunctionProperties>
function_properties(const EmitContext& ctx, const util::IndexedVector<lir::FunctionIndex, lir::Function>& functions) {
    util::IndexedVector<lir::FunctionIndex, FunctionProperties> out;
    util::IndexedVector<lir::FunctionIndex, Uses> uses;
    out.resize(functions.size());
    uses.resize(functions.size());

    // A nested function is numbered after the function it is in, so is done first.
    for (const auto index : functions.indices().reversed()) {
        const auto& function = functions[index];
        const auto& frame = ctx.frames.frames[function.frame];
        const bool members_allowed = members_close(ctx, frame);
        auto& used = uses[index];
        auto& properties = out[index];

        const auto local = [&](lir::LocalRef l) {
            used.outer_depth = std::max<std::uint32_t>(used.outer_depth, l.depth);
        };
        const auto member = [&]() { used.cant_be_closed = used.cant_be_closed || !members_allowed; };
        for (const auto& instruction : function.instructions.raw())
            std::visit(util::overload {
                           [](const auto&) {},
                           [&](const lir::PushLocal& p) { local(p.local); },
                           [&](const lir::StoreLocal& s) { local(s.local); },
                           [&](const lir::PushInstance&) { member(); },
                           [&](const lir::StoreInstance&) { member(); },
                           [&](const lir::PushClassVariable&) { member(); },
                           [&](const lir::StoreClassVariable&) { member(); },
                           [&](const lir::PushSpecial& p) {
                               if (p.kind == ir::SCSpecial::Kind::This || p.kind == ir::SCSpecial::Kind::Function)
                                   used.cant_be_closed = true;
                           },
                           [&](const lir::NonLocalReturn&) { used.cant_be_closed = true; },
                           [&](const lir::PushFunction& p) {
                               const auto& nested = uses[p.function];
                               used.cant_be_closed = used.cant_be_closed || nested.cant_be_closed;
                               if (nested.outer_depth > 0)
                                   used.outer_depth = std::max(used.outer_depth, nested.outer_depth - 1);
                               properties.needs_heap_context =
                                   properties.needs_heap_context || out[p.function].has_context;
                           },
                       },
                       instruction);

        if (ctx.graph.is_a<ir::SCMethod_I>(frame.owner))
            continue;
        properties.is_closed = !used.cant_be_closed && used.outer_depth == 0;
        properties.has_context = !properties.is_closed && !explicitly_closed(ctx, frame);
    }
    return out;
}

////////////////////////////////////////////////////////////////////////////////
// Method shortcuts
////////////////////////////////////////////////////////////////////////////////

// Whether the instruction pushes the method's argument k, slot 0 being `this`.
bool pushes_argument(const lir::Instruction& i, lir::SlotIndex k) {
    const auto* local = std::get_if<lir::PushLocal>(&i);
    return local && local->local.depth == 0 && local->local.slot == k;
}

// Attempt to find a shortcut method.
OptionalMethodShortcut method_shortcut(const EmitContext& ctx, const lir::Function& function, const Frame& frame) {
    const auto method = ctx.graph.as_a<ir::SCMethod_I>(frame.owner);
    if (!method)
        return {};
    const auto& payload = ctx.graph.payload(*method);
    const auto selector = *payload.selector;
    if (payload.primitive || frame.variadic != Variadic::None || selector == ctx.special_symbols.does_not_understand
        || (ctx.interpreter_members && selector == ctx.special_symbols.function_compile_context))
        return {};

    const auto& code = function.instructions;
    // The method's code is short, so named by position.
    constexpr lir::InstructionIndex first { 0 };
    const auto second = util::advance(first);
    const auto third = util::advance(first, 2);
    const auto fourth = util::advance(first, 3);
    // Lowering ends every method with a return.
    assert(!code.empty());
    const auto last = util::retreat(code.next_index());
    if (code.size() == 1 && std::holds_alternative<lir::ReturnSelf>(code[first]))
        return shortcut::ReturnSelf {};
    if (ir::has_assign_shortcut(payload.version) && code.size() == 4 && frame.num_positional > 0
        && pushes_argument(code[first], lir::SlotIndex { 1 }) && std::holds_alternative<lir::Drop>(code[third])
        && std::holds_alternative<lir::ReturnSelf>(code[fourth])) {
        if (const auto* store = std::get_if<lir::StoreInstance>(&code[second]))
            return shortcut::AssignInstance { store->slot };
        if (const auto* store = std::get_if<lir::StoreClassVariable>(&code[second]);
            store && ir::has_class_variable_shortcuts(payload.version))
            return shortcut::AssignClassVariable { store->index };
    }
    if (!std::holds_alternative<lir::Return>(code[last]))
        return {};
    if (code.size() == 2) {
        const auto& value = code[first];
        if (const auto* s = std::get_if<lir::PushSpecial>(&value); s && s->kind == ir::SCSpecial::Kind::This)
            return shortcut::ReturnSelf {};
        if (const auto* c = std::get_if<lir::PushConstant>(&value);
            c && !ctx.constants(c->value).get_if<ir::SymSCClassName>())
            return shortcut::ReturnLiteral { c->value };
        if (const auto* l = std::get_if<lir::PushLocal>(&value);
            l && l->local.depth == 0 && *l->local.slot >= 1 && *l->local.slot <= frame.num_positional)
            return shortcut::ReturnArgument { l->local.slot };
        if (const auto* p = std::get_if<lir::PushInstance>(&value))
            return shortcut::ReturnInstance { p->slot };
        if (const auto* p = std::get_if<lir::PushClassVariable>(&value);
            p && ir::has_class_variable_shortcuts(payload.version))
            return shortcut::ReturnClassVariable { p->index };
    }

    // Shortcuts for message sends.
    constexpr std::uint32_t receiver_send_and_return = 3;
    if (code.size() != frame.num_positional + receiver_send_and_return)
        return {};
    const auto* send = std::get_if<lir::Send>(&code[util::retreat(last)]);
    if (!send || send->form != lir::SendForm::Call || send->kind == lir::SendKind::WithAdverb
        || send->num_positional != frame.num_positional || send->num_keyword != 0)
        return {};
    // The send's operands are the instructions before it, the receiver first.
    for (const auto k : operands(util::advance(receiver_operand), 1 + frame.num_positional))
        if (!pushes_argument(code[lir::InstructionIndex { *k }], argument_slot(k)))
            return {};
    const bool super = send->kind == lir::SendKind::Super;
    if (const auto* s = std::get_if<lir::PushSpecial>(&code[first]); s && s->kind == ir::SCSpecial::Kind::This)
        return shortcut::Redirect { send->selector, super };
    if (super)
        return {};
    if (const auto* p = std::get_if<lir::PushInstance>(&code[first]))
        return shortcut::ForwardInstance { send->selector, p->slot };
    if (const auto* p = std::get_if<lir::PushClassVariable>(&code[first]))
        return shortcut::ForwardClassVariable { send->selector, p->index };
    return {};
}

////////////////////////////////////////////////////////////////////////////////
// Hard coded loops
////////////////////////////////////////////////////////////////////////////////

// The opcodes use the frame's slots directly, `arguments` counting `this`.
struct LoopFrame {
    std::uint32_t arguments;
    std::uint32_t variables;
};

LoopFrame loop_frame(HardCodedLoop loop) {
    switch (loop) {
    case HardCodedLoop::IntegerDo:
    case HardCodedLoop::ArrayedCollectionDo:
    case HardCodedLoop::FloatDo:
        return { 2, 1 };
    case HardCodedLoop::IntegerReverseDo:
    case HardCodedLoop::ArrayedCollectionReverseDo:
    case HardCodedLoop::FloatReverseDo:
        return { 2, 2 };
    case HardCodedLoop::IntegerFor:
        return { 3, 3 };
    case HardCodedLoop::IntegerForBy:
    case HardCodedLoop::NumberForSeries:
        return { 4, 2 };
    case HardCodedLoop::DictionaryKeysValuesArrayDo:
        return { 3, 5 };
    }
    util::unreachable();
}

// From 3.16 an extension replacing one of these methods is compiled as written.
std::optional<HardCodedLoop> hard_coded_loop(const EmitContext& ctx, ir::SCMethod_I method) {
    const auto& payload = ctx.graph.payload(method);
    if (payload.extension && !ir::hard_codes_extension_loops(payload.version))
        return std::nullopt;
    const auto root = ctx.graph.root();
    const auto* class_i = root ? std::get_if<ir::SCClass_I>(&*root) : nullptr;
    if (!class_i)
        return std::nullopt;
    return ctx.special_symbols.find_hard_coded_loop(ctx.graph.payload(*class_i).name, *payload.selector);
}

// The old compiler's checks, as errors, though it emitted the loop anyway. Returns whether there were none.
bool check_loop_frame(const EmitContext& ctx, ir::SCMethod_I method, const Frame& frame, HardCodedLoop loop,
                      std::vector<sc::diag::Diagnostic>& diagnostics) {
    const auto& payload = ctx.graph.payload(method);
    const auto selector_loc = payload.selector.location;
    if (payload.extension)
        diagnostics.push_back(sc::diag::Diagnostic::hard_coded_loop_extension_ignored(
            ctx.graph.text_info(selector_loc.source), ir::IRGraph::range(selector_loc)));
    const auto num_diags_before = diagnostics.size();
    const auto expected = loop_frame(loop);
    const auto arguments = std::uint32_t { frame.has_this } + frame.num_positional;
    if (frame.variadic != Variadic::None || arguments != expected.arguments
        || frame.num_slots() - arguments != expected.variables)
        diagnostics.push_back(
            sc::diag::Diagnostic::hard_coded_loop_signature(ctx.graph.text_info(selector_loc.source), //
                                                            ir::IRGraph::range(selector_loc), //
                                                            expected.arguments, expected.variables));

    const auto computed_error = [&](ir::NodeIndex i) {
        const auto loc = ctx.graph.location(i);
        diagnostics.push_back(sc::diag::Diagnostic::hard_coded_loop_computed_value(ctx.graph.text_info(loc.source),
                                                                                   ir::IRGraph::range(loc)));
    };
    const auto [positional, _, body] = ctx.graph.children(method);
    for (auto it = ctx.graph.children(positional).iter(); it; ++it) {
        const auto [value] = ctx.graph.children(*it);
        if (!ctx.graph.payload(*it).prototype && !ctx.graph.is_a<ir::SCMissing_I>(value))
            computed_error(value);
    }
    for (auto it = ctx.graph.children(body).iter(); it; ++it) {
        const ir::NodeIndex i { **it };
        if (const auto declare = ctx.graph.as_a<ir::SCVariableDeclare_I>(i)) {
            const auto [value] = ctx.graph.children(*declare);
            if (!ctx.graph.payload(*declare).prototype && !ctx.graph.is_a<ir::SCMissing_I>(value))
                computed_error(value);
        } else if (ctx.graph.is_a<ir::SCPackDeclare_I>(i)) {
            computed_error(i);
        }
    }
    // error if we emitted a diagnostic.
    return diagnostics.size() == num_diags_before;
}

CompilingBytecodes::Data loop_code(HardCodedLoop loop, sc::src::SourceCodeRange loc) {
    CompilingBytecodes bc;
    switch (loop) {
    case HardCodedLoop::IntegerDo:
        Extended::IntegerDo.emit(bc, loc);
        break;
    case HardCodedLoop::IntegerReverseDo:
        Extended::IntegerReverseDo.emit(bc, loc);
        break;
    case HardCodedLoop::IntegerFor:
        Extended::IntegerFor.emit(bc, loc);
        break;
    case HardCodedLoop::IntegerForBy:
        Extended::IntegerForBy.emit(bc, loc);
        break;
    case HardCodedLoop::ArrayedCollectionDo:
        Extended::ArrayedCollectionDo.emit(bc, loc);
        break;
    case HardCodedLoop::ArrayedCollectionReverseDo:
        Extended::ArrayedCollectionReversedDo.emit(bc, loc);
        break;
    case HardCodedLoop::DictionaryKeysValuesArrayDo:
        Extended::DictionaryKeyValuesArrayDo.emit(bc, loc);
        break;
    case HardCodedLoop::NumberForSeries:
        Extended::NumberForSeries.emit(bc, loc);
        break;
    case HardCodedLoop::FloatDo:
        Extended::FloatDo.emit(bc, loc);
        break;
    case HardCodedLoop::FloatReverseDo:
        Extended::FloatDoReverse.emit(bc, loc);
        break;
    }
    return std::move(bc).finish();
}

// In the order the old compiler added them, by member, getter then setter.
std::vector<Accessor> accessors(const EmitContext& ctx) {
    std::vector<Accessor> out;
    const auto root = ctx.graph.root();
    const auto* class_i = root ? std::get_if<ir::SCClass_I>(&*root) : nullptr;
    if (!class_i)
        return out;
    const auto owner = ctx.graph.payload(*class_i).name;
    const auto [members, _] = ctx.graph.children(*class_i);
    for (auto it = ctx.graph.children(members).iter(); it; ++it) {
        const auto& member = ctx.graph.payload(*it);
        if (member.owning_class != owner || (!member.getter && !member.setter))
            continue;
        const auto location = ctx.members.find(owner, member.name);
        assert(location);
        const auto* slot = std::get_if<InstanceSlotIndex>(&*location);
        const auto* index = std::get_if<ClassVariableIndex>(&*location);
        const auto* value = std::get_if<ir::OptionalConstExprIndex>(&*location);
        if (member.getter) {
            OptionalMethodShortcut getter;
            if (slot)
                getter = shortcut::ReturnInstance { *slot };
            else if (index)
                getter = shortcut::ReturnClassVariable { *index };
            else
                getter = shortcut::ReturnLiteral { ir::ConstExprIndex { **value } };
            out.push_back({ *it, ir::SymSCSelector { *member.getter }, getter });
        }
        if (member.setter) {
            // A constant has no setter.
            assert(!value);
            out.push_back({ *it, ir::SymSCSelector { *member.setter },
                            slot ? OptionalMethodShortcut { shortcut::AssignInstance { *slot } }
                                 : shortcut::AssignClassVariable { *index } });
        }
    }
    return out;
}

////////////////////////////////////////////////////////////////////////////////
// Roots
////////////////////////////////////////////////////////////////////////////////

// A root and the functions nested in it, which follow it up to end. Returns whether all were emitted, stopping at the
// first that fails.
bool emit_root(const EmitContext& ctx, const lir::LoweredGraph& lowered,
               const util::IndexedVector<lir::FunctionIndex, FunctionProperties>& properties, lir::FunctionIndex root,
               lir::FunctionIndex end, EmittedGraph& out) {
    const auto& root_frame = ctx.frames.frames[lowered.functions[root].frame];
    // Check for shortcuts
    if (const auto found = method_shortcut(ctx, lowered.functions[root], root_frame);
        !std::holds_alternative<std::monostate>(found)) {
        // Its code is a single return, so creates no functions.
        assert(end == util::advance(root));
        EmittedFunction emitted {};
        emitted.shortcut = found;
        emitted.properties = properties[root];
        out.functions.push_back(std::move(emitted));
        return true;
    }
    // Some methods are hard coded so have a shorter exit path.
    if (const auto method = ctx.graph.as_a<ir::SCMethod_I>(root_frame.owner)) {
        if (const auto loop = hard_coded_loop(ctx, *method)) {
            if (!check_loop_frame(ctx, *method, root_frame, *loop, out.diagnostics))
                return false;
            // The body is thrown away, with the functions in it, and anything it would have made the method.
            EmittedFunction emitted {};
            emitted.code = loop_code(*loop, ir::IRGraph::range(ctx.graph.location(*method)));
            out.functions.push_back(std::move(emitted));
            while (out.functions.next_index() != end) {
                EmittedFunction thrown_away {};
                thrown_away.thrown_away = true;
                out.functions.push_back(std::move(thrown_away));
            }
            return true;
        }
    }
    for (const auto index : util::range(root, end)) {
        const auto& function = lowered.functions[index];
        const auto& frame = ctx.frames.frames[function.frame];
        Emitter emitter { ctx, function, frame, {}, out.diagnostics };
        emitter.run(ctx.graph.is_a<ir::SCMethod_I>(frame.owner));
        emitter.out.properties = properties[index];
        out.functions.push_back(std::move(emitter.out));
        if (emitter.failed)
            return false;
    }
    return true;
}
}

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

EmittedGraph emit(const EmitContext& ctx, const lir::LoweredGraph& lowered) {
    EmittedGraph out;
    out.accessors = accessors(ctx);
    const auto properties = function_properties(ctx, lowered.functions);
    // Nested functions follow their root, a function whose frame has no parent. A root that fails leaves out the rest.
    for (lir::FunctionIndex root { 0 }; lowered.functions.contains(root);) {
        // root to end spans the function and its nested functions.
        auto end = util::advance(root);
        while (lowered.functions.contains(end) && ctx.frames.frames[lowered.functions[end].frame].parent)
            end = util::advance(end);
        const auto num_diagnostics = out.diagnostics.size();
        if (lowered.functions[root].complete && emit_root(ctx, lowered, properties, root, end, out))
            out.roots.push_back(root);
        else if (out.diagnostics.size() == num_diagnostics) {
            // Failed without saying why, so the cause is something the backend can't do yet.
            const auto location = ctx.graph.location(ctx.frames.frames[lowered.functions[root].frame].owner);
            out.diagnostics.push_back(
                sc::diag::Diagnostic::not_compiled(ctx.graph.text_info(location.source), ir::IRGraph::range(location)));
        }

        out.functions.resize(*end);
        root = end;
    }
    return out;
}

////////////////////////////////////////////////////////////////////////////////
// Dump
////////////////////////////////////////////////////////////////////////////////

void write_shortcut(std::ostream& out, const OptionalMethodShortcut& shortcut, const ir::SymbolTable& symbols,
                    const ir::ConstantTable& constants) {
    out << "shortcut: ";
    std::visit(
        util::overload {
            [](std::monostate) {},
            [&](shortcut::ReturnSelf) { out << "return self"; },
            [&](shortcut::ReturnLiteral s) {
                out << "return literal ";
                constants(s.value).to_literal(out, symbols, constants);
            },
            [&](shortcut::ReturnArgument s) { out << "return argument " << +*s.slot; },
            [&](shortcut::ReturnInstance s) { out << "return instance variable " << *s.slot; },
            [&](shortcut::AssignInstance s) { out << "assign instance variable " << *s.slot; },
            [&](shortcut::Redirect s) { out << (s.super ? "redirect to super " : "redirect ") << symbols(s.selector); },
            [&](shortcut::ForwardInstance s) {
                out << "forward " << symbols(s.selector) << " to instance variable " << *s.slot;
            },
            [&](shortcut::ForwardClassVariable s) {
                out << "forward " << symbols(s.selector) << " to class variable " << *s.index;
            },
            [&](shortcut::ReturnClassVariable s) { out << "return class variable " << *s.index; },
            [&](shortcut::AssignClassVariable s) { out << "assign class variable " << *s.index; },
        },
        shortcut);
}

namespace {

const char* opcode_name(Byte b) {
    // Inclusive, as ints, `endCode - 1` isn't a Byte.
    struct Range {
        int begin;
        int end;
        const char* name;
    };
    static const Range ranges[] = {
        { PushClassX.code, PushClassX.code, PushClassX.name },
        { PushInstVarX.code, PushInstVarX.code, PushInstVarX.name },
        { PushTempVarX.code, PushTempVarX.code, PushTempVarX.name },
        { PushTempZeroVarX.code, PushTempZeroVarX.code, PushTempZeroVarX.name },
        { PushLiteralX.code, PushLiteralX.code, PushLiteralX.name },
        { PushClassVarX.code, PushClassVarX.code, PushClassVarX.name },
        { PushSpecialClass.code, PushSpecialClass.code, PushSpecialClass.name },
        { StoreInstVarX.code, StoreInstVarX.code, StoreInstVarX.name },
        { StoreTempVarX.code, StoreTempVarX.code, StoreTempVarX.name },
        { StoreClassVarX.code, StoreClassVarX.code, StoreClassVarX.name },
        { SendMsgX.code, SendMsgX.code, SendMsgX.name },
        { SendSuperMsgX.code, SendSuperMsgX.code, SendSuperMsgX.name },
        { SendSpecialMsgX.code, SendSpecialMsgX.code, SendSpecialMsgX.name },
        { SendSpecialUnaryArithMsgX.code, SendSpecialUnaryArithMsgX.code, SendSpecialUnaryArithMsgX.name },
        { SendSpecialBinaryArithMsgX.code, SendSpecialBinaryArithMsgX.code, SendSpecialBinaryArithMsgX.name },
        { SpecialOpcode.code, SpecialOpcode.code, SpecialOpcode.name },
        { PushInstVar.startCode, PushInstVar.endCode - 1, PushInstVar.name },
        { JumpIfTrue.code, JumpIfTrue.code, JumpIfTrue.name },
        { PushTempVar.startCode, PushTempVar.endCode - 1, PushTempVar.name },
        { PushConstant8.code, PushConstant8.code, PushConstant8.name },
        { PushConstant16.code, PushConstant16.code, PushConstant16.name },
        { PushConstant24.code, PushConstant24.code, PushConstant24.name },
        { PushConstant32.code, PushConstant32.code, PushConstant32.name },
        { PushInteger8.code, PushInteger8.code, PushInteger8.name },
        { PushInteger16.code, PushInteger16.code, PushInteger16.name },
        { PushInteger24.code, PushInteger24.code, PushInteger24.name },
        { PushInteger32.code, PushInteger32.code, PushInteger32.name },
        { PushTempZeroVar.startCode, PushTempZeroVar.endCode - 1, PushTempZeroVar.name },
        { PushLiteral.startCode, PushLiteral.endCode - 1, PushLiteral.name },
        { PushClassVar.startCode, PushClassVar.endCode - 1, PushClassVar.name },
        { PushSpecialValueThis.code, PushSpecialValueThis.code, PushSpecialValueThis.name },
        { PushOneAndSubtract.code, PushOneAndSubtract.code, PushOneAndSubtract.name },
        { PushSpecialNumber.startCode, PushSpecialNumber.endCode - 1, PushSpecialNumber.name },
        { PushOneAndAddOne.code, PushOneAndAddOne.code, PushOneAndAddOne.name },
        { PushSpecialValue.startCode, PushSpecialValue.endCode - 1, PushSpecialValue.name },
        { StoreInstVar.startCode, StoreInstVar.endCode - 1, StoreInstVar.name },
        { StoreTempVar.startCode, StoreTempVar.endCode - 1, StoreTempVar.name },
        { PushInstVarAndSendSpecialMsg.code, PushInstVarAndSendSpecialMsg.code, PushInstVarAndSendSpecialMsg.name },
        { PushAllArgsAndSendMsg.code, PushAllArgsAndSendMsg.code, PushAllArgsAndSendMsg.name },
        { PushAllButFirstArgAndSendMsg.code, PushAllButFirstArgAndSendMsg.code, PushAllButFirstArgAndSendMsg.name },
        { PushAllArgsAndSendSpecialMsg.code, PushAllArgsAndSendSpecialMsg.code, PushAllArgsAndSendSpecialMsg.name },
        { PushAllButFirstArgAndSendSpecialMsg.code, PushAllButFirstArgAndSendSpecialMsg.code,
          PushAllButFirstArgAndSendSpecialMsg.name },
        { PushAllButFirstTwoArgsAndSendMsg.code, PushAllButFirstTwoArgsAndSendMsg.code,
          PushAllButFirstTwoArgsAndSendMsg.name },
        { PushAllButFirstTwoArgsAndSendSpecialMsg.code, PushAllButFirstTwoArgsAndSendSpecialMsg.code,
          PushAllButFirstTwoArgsAndSendSpecialMsg.name },
        { ExtendedCodes.code, ExtendedCodes.code, ExtendedCodes.name },
        { StoreClassVar.startCode, StoreClassVar.endCode - 1, StoreClassVar.name },
        { SendMsgThisOpt.code, SendMsgThisOpt.code, SendMsgThisOpt.name },
        { SendMsg.startCode, SendMsg.endCode - 1, SendMsg.name },
        { TailCallReturnFromFunction.code, TailCallReturnFromFunction.code, TailCallReturnFromFunction.name },
        { SendSuperMsgThisOpt.code, SendSuperMsgThisOpt.code, SendSuperMsgThisOpt.name },
        { SendSuperMsg.startCode, SendSuperMsg.endCode - 1, SendSuperMsg.name },
        { SendSpecialMsgThisOpt.code, SendSpecialMsgThisOpt.code, SendSpecialMsgThisOpt.name },
        { SendSpecialMsg.startCode, SendSpecialMsg.endCode - 1, SendSpecialMsg.name },
        { SendSpecialUnaryArithMsg.startCode, SendSpecialUnaryArithMsg.endCode - 1, SendSpecialUnaryArithMsg.name },
        { SendSpecialBinaryArithMsg.startCode, SendSpecialBinaryArithMsg.endCode - 1, SendSpecialBinaryArithMsg.name },
        { Drop.code, Drop.code, Drop.name },
        { Dup.code, Dup.code, Dup.name },
        { BlockReturn.code, BlockReturn.code, BlockReturn.name },
        { Return.code, Return.code, Return.name },
        { ReturnSelf.code, ReturnSelf.code, ReturnSelf.name },
        { ReturnTrue.code, ReturnTrue.code, ReturnTrue.name },
        { ReturnFalse.code, ReturnFalse.code, ReturnFalse.name },
        { ReturnNil.code, ReturnNil.code, ReturnNil.name },
        { JumpIfFalse.code, JumpIfFalse.code, JumpIfFalse.name },
        { JumpIfFalsePushNil.code, JumpIfFalsePushNil.code, JumpIfFalsePushNil.name },
        { JumpIfFalsePushFalse.code, JumpIfFalsePushFalse.code, JumpIfFalsePushFalse.name },
        { JumpIfTruePushTrue.code, JumpIfTruePushTrue.code, JumpIfTruePushTrue.name },
        { JumpFwd.code, JumpFwd.code, JumpFwd.name },
        { JumpBack.code, JumpBack.code, JumpBack.name },
        { SpecialBinaryOpWithAdverb.code, SpecialBinaryOpWithAdverb.code, SpecialBinaryOpWithAdverb.name },
        { TailCallReturnFromMethod.code, TailCallReturnFromMethod.code, TailCallReturnFromMethod.name },
    };
    for (const auto& r : ranges)
        if (b >= r.begin && b <= r.end)
            return r.name;
    return "?";
}

}

void write_emitted(std::ostream& out, const EmittedFunction& function, std::string_view indent,
                   const ir::SymbolTable& symbols, const ir::ConstantTable& constants) {
    out << indent << "properties:";
    if (function.properties.has_context)
        out << " context,";
    if (function.properties.is_closed)
        out << " closed,";
    if (function.properties.needs_heap_context)
        out << " heap context,";
    if (!std::holds_alternative<std::monostate>(function.shortcut)) {
        out << "\n" << indent;
        write_shortcut(out, function.shortcut, symbols, constants);
    }
    out << "\n" << indent << "selectors:";
    for (const auto& entry : function.selectors.raw()) {
        std::visit(util::overload {
                       [&](ir::SymSCSelector s) { out << " " << symbols(s); },
                       [&](ir::SymSCClassName s) { out << " " << symbols(s); },
                       [&](lir::FunctionIndex f) { out << " function " << *f; },
                   },
                   entry);
        out << ",";
    }
    out << "\n" << indent << "constants:";
    for (const auto& entry : function.constants.raw()) {
        std::visit(util::overload {
                       [&](ir::ConstExprIndex c) {
                           out << " ";
                           constants(c).to_literal(out, symbols, constants);
                       },
                       [&](ir::SymSCSymbol s) { out << " \\" << symbols(s); },
                       [&](const ClassConstant& c) {
                           out << " ";
                           constants(c.value).to_literal(out, symbols, constants);
                       },
                       [&](SwitchTableIndex i) {
                           const auto& t = function.switch_tables[i];
                           out << " switch table (capacity " << t.capacity;
                           for (const auto& [key, offset] : t.offsets) {
                               out << ", ";
                               constants(key).to_literal(out, symbols, constants);
                               out << ": " << *offset;
                           }
                           out << ", default: " << *t.default_offset << ")";
                       },
                   },
                   entry);
        out << ",";
    }
    out << "\n";
    const auto& code = function.code;
    CodeOffset offset { 0 };
    auto byte = code.codes.raw().begin();
    for (const auto size : code.sizeOfCodes.raw()) {
        out << indent << std::setw(4) << std::left << *offset << std::right << ' ';
        const auto opcode = *byte;
        std::string bytes;
        for (const auto end = byte + size; byte != end; ++byte) {
            static constexpr char digits[] = "0123456789ABCDEF";
            bytes += digits[*byte >> 4];
            bytes += digits[*byte & 15];
            bytes += ' ';
        }
        out << std::setw(16) << std::left << bytes << std::right << opcode_name(opcode) << "\n";
        offset = util::advance(offset, static_cast<std::uint32_t>(size));
    }
}

void write_accessor(std::ostream& out, const Accessor& accessor, std::string_view indent,
                    const ir::SymbolTable& symbols, const ir::ConstantTable& constants) {
    out << indent;
    write_shortcut(out, accessor.shortcut, symbols, constants);
    out << "\n";
}
}
