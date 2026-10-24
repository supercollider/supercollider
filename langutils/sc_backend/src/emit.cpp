#include "sc_backend/emit.hpp"
#include "sc_bytecode/Opcodes.h"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_util/overload.hpp"

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

// A value on the stack while a sub-region is read, as the instructions that start and finish computing it.
struct Value {
    lir::InstructionIndex start;
    lir::InstructionIndex producer;

    // A push, which can be left out on its own.
    [[nodiscard]] bool pushed_by_one_instruction() const { return start == producer; }
};
static_assert(sizeof(Value) == 8);

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
    UnaryAfterDrops,
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
    // UnaryAfterDrops' extra arguments, dropped before the operator.
    std::uint8_t drops { 0 };
    InstanceSlotIndex instance { 0 };
    SelectorIndex selector { 0 };
};
static_assert(sizeof(SendPlan) == 16);


struct RegionPlan {
    explicit RegionPlan(lir::FunctionSubRegion r): region(r), m_left_out(r.size, false) {}

    lir::FunctionSubRegion region;
    std::vector<SendPlan> sends {};
    std::vector<lir::InstructionIndex> send_instructions {};
    // The instruction each send's selector is added at, and the send, sorted by instruction. Sends added at the same
    // instruction are in the order the old compiler added them, the outer first.
    std::vector<std::pair<lir::InstructionIndex, std::size_t>> additions {};

    [[nodiscard]] bool contains(lir::InstructionIndex i) const {
        return *i >= *region.begin && *i < *region.begin + region.size;
    }
    [[nodiscard]] bool left_out(lir::InstructionIndex i) const { return m_left_out[offset(i)]; }
    void leave_out(lir::InstructionIndex i) { m_left_out[offset(i)] = true; }

private:
    [[nodiscard]] std::size_t offset(lir::InstructionIndex i) const {
        assert(contains(i));
        return *i - *region.begin;
    }
    std::vector<bool> m_left_out;
};

////////////////////////////////////////////////////////////////////////////////
// Emitter
////////////////////////////////////////////////////////////////////////////////

struct Emitter {
    const EmitContext& ctx;
    const lir::Function& function;
    const Frame& frame;
    EmittedFunction out {};
    std::vector<sc::diag::Diagnostic>& diagnostics;
    bool failed { false };
    bool reported_selectors { false };

    [[nodiscard]] const lir::Instruction& instruction(lir::InstructionIndex i) const {
        return function.instructions[i];
    }

    [[nodiscard]] sc::lex::SourceCodeRange location(lir::InstructionIndex i) const {
        const auto span = function.spans[i];
        return { { span.begin, 0, 0 }, { span.end, 0, 0 } };
    }

    [[nodiscard]] ir::SourceLocation owner_location() const {
        return ctx.graph.location(ir::NodeIndex { *frame.owner });
    }

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
            for (std::size_t i { 0 }; i < out.selectors.size(); ++i) {
                const SelectorIndex index { static_cast<std::uint8_t>(i) };
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
        const auto identity = identity_of(ctx.constants, entry);
        for (std::uint32_t i { 0 }; i < out.constants.size(); ++i)
            if (identity == identity_of(ctx.constants, out.constants[ConstantIndex { i }]))
                return ConstantIndex { i };
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

    [[nodiscard]] static lir::SlotIndex argument_slot(std::uint32_t k) {
        assert(k <= std::numeric_limits<std::uint8_t>::max());
        return lir::SlotIndex { static_cast<std::uint8_t>(k) };
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
    [[nodiscard]] PushAll push_all_args(const Value* args, std::uint32_t n) const {
        if (!is_name(args[0])) {
            if (n < 3)
                return PushAll::No;
            for (std::uint32_t k { 1 }; k < n; ++k)
                if (!is_argument(args[k], argument_slot(k)))
                    return PushAll::No;
            return PushAll::AllButFirst;
        }
        for (std::uint32_t k { 0 }; k < n; ++k)
            if (!is_argument(args[k], argument_slot(k)))
                return PushAll::No;
        return PushAll::All;
    }

    // checkPushAllButFirstTwoArgs, the arguments after the first two are the function's after its first.
    [[nodiscard]] bool push_all_but_first_two_args(const Value* args, std::uint32_t num_block) const {
        if (num_block < 2)
            return false;
        for (std::uint32_t k { 1 }; k < num_block; ++k)
            if (!is_argument(args[k + 1], argument_slot(k)))
                return false;
        return true;
    }

    // Decides one send, see PyrCallNode::compileCall, PyrBinopCallNode::compileCall and PyrSetterNode::compileCall.
    SendPlan decide(RegionPlan& plan, const lir::Send& s, const Value* args) {
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
            return binary && (*binary == OpBinaryMath::Add || *binary == OpBinaryMath::Sub) && is_integer_one(args[1]);
        };
        const auto leave_out_arg = [&](std::uint32_t a) { plan.leave_out(args[a].producer); };

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
            if (super || !special) {
                p.add = AddSelector::AtReceiver;
                p.op = super ? SendOp::Super : SendOp::Message;
            } else if (special_selector) {
                p.op = SendOp::Special;
            } else if (unary) {
                // The extra arguments are dropped.
                p.op = SendOp::UnaryAfterDrops;
                p.drops = static_cast<std::uint8_t>(n - 1);
            } else if (n == 3) {
                p.op = SendOp::BinaryWithAdverb;
            } else if (adds_or_subtracts_one()) {
                // The old compiler didn't emit a tail call before PushOneAndAddOne.
                const bool add = *binary == OpBinaryMath::Add;
                p.op = add ? SendOp::PushOneAndAddOne : SendOp::PushOneAndSubtract;
                p.tail = p.tail && !add;
                leave_out_arg(1);
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
                leave_out_arg(0);
            } else {
                p.op = SendOp::Super;
            }
            return p;
        }

        // A message, also an operator sent with the wrong number of arguments.
        const auto message = [&]() {
            p.add = AddSelector::AtReceiver;
            if (n == 1 && is_this(args[0])) {
                p.op = SendOp::MessageThisOpt;
                leave_out_arg(0);
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
                    for (std::uint32_t a { all == PushAll::All ? 0u : 1u }; a < n; ++a)
                        leave_out_arg(a);
                }
            } else if (n > 2 && n == num_block_args() + 1 && push_all_but_first_two_args(args, num_block_args())) {
                p.op = SendOp::PushAllButFirstTwoArgs;
                for (std::uint32_t a { 2 }; a < n; ++a)
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
                if (is_this(args[0])) {
                    p.op = SendOp::SpecialThisOpt;
                    leave_out_arg(0);
                } else if (args[0].pushed_by_one_instruction()) {
                    if (const auto* instance = std::get_if<lir::PushInstance>(&instruction(args[0].producer));
                        instance && *instance->slot <= std::numeric_limits<std::uint8_t>::max()) {
                        p.op = SendOp::PushInstVarAndSpecial;
                        p.instance = instance->slot;
                        leave_out_arg(0);
                    }
                }
            } else if (*special_selector == OpSpecialSelectors::Do) {
                // `(a..b).do(f)` is `a.forSeries(nil, b, f)`, the range's arguments are the first.
                const auto* series = std::get_if<lir::Send>(&instruction(args[0].producer));
                if (series && series->selector == ctx.special_symbols.series && series->form == lir::SendForm::Call
                    && series->kind == lir::SendKind::Normal && series->num_keyword == 0) {
                    plan.leave_out(args[0].producer);
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
            leave_out_arg(1);
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
    void special_push_all(SendPlan& p, const Value* args, std::uint32_t n, const LeaveOut& leave_out_arg) const {
        if (n <= 1)
            return;
        const auto all = push_all_args(args, n);
        if (all == PushAll::No)
            return;
        p.op = all == PushAll::All ? SendOp::PushAllArgsSpecial : SendOp::PushAllButFirstArgSpecial;
        for (std::uint32_t a { all == PushAll::All ? 0u : 1u }; a < n; ++a)
            leave_out_arg(a);
    }

    // Reads the sub-region's stack, then decides its sends from the last, so a send that absorbs the one computing its
    // receiver, e.g., forSeries, decides first, and the outer of two sends adding a selector at the same instruction
    // adds it first.
    RegionPlan plan_region(lir::FunctionSubRegion region) {
        RegionPlan plan { region };
        const std::uint32_t begin { *region.begin };
        std::vector<Value> stack;
        struct SendArgs {
            lir::InstructionIndex instruction;
            std::size_t first;
        };
        std::vector<SendArgs> sends;
        std::vector<Value> args;
        const auto pop = [&]() {
            assert(!stack.empty());
            const auto v = stack.back();
            stack.pop_back();
            return v;
        };
        const auto replace = [&](lir::InstructionIndex i, std::size_t count) {
            assert(stack.size() >= count);
            const auto start = stack[stack.size() - count].start;
            stack.erase(stack.end() - static_cast<std::ptrdiff_t>(count), stack.end());
            stack.push_back({ start, i });
        };
        for (std::uint32_t k { begin }; k < begin + region.size; ++k) {
            const lir::InstructionIndex i { k };
            std::visit(util::overload {
                           [&](const lir::Send& s) {
                               const std::size_t count = 1u + s.num_positional + 2u * s.num_keyword;
                               assert(stack.size() >= count);
                               sends.push_back({ i, args.size() });
                               args.insert(args.end(), stack.end() - static_cast<std::ptrdiff_t>(count), stack.end());
                               const auto receiver = stack[stack.size() - count];
                               stack.erase(stack.end() - static_cast<std::ptrdiff_t>(count), stack.end());
                               stack.push_back({ receiver.start, i });
                           },
                           [&](const lir::StoreLocal&) { stack.back().producer = i; },
                           [&](const lir::StoreInstance&) { stack.back().producer = i; },
                           [&](const lir::StoreClassVariable&) { stack.back().producer = i; },
                           [&](const lir::Drop&) { (void)pop(); },
                           [&](const lir::Return&) { return_value(plan, pop()); },
                           [&](const lir::NonLocalReturn&) { return_value(plan, pop()); },
                           [&](const lir::ReturnSelf&) {},
                           [&](const lir::DefaultArgument&) {},
                           [&](const lir::If&) { replace(i, 1); },
                           [&](const lir::IfNil&) { replace(i, 1); },
                           [&](const lir::IfNotNil&) { replace(i, 1); },
                           [&](const lir::And&) { replace(i, 1); },
                           [&](const lir::Or&) { replace(i, 1); },
                           [&](const lir::OrIfNil&) { replace(i, 1); },
                           [&](const lir::AndIfNotNil&) { replace(i, 1); },
                           [&](const lir::QuestionMark&) { replace(i, 2); },
                           [&](const lir::Switch&) { replace(i, 1); },
                           [&](const lir::While&) {
                               stack.push_back({ i, i });
                           },
                           [&](const lir::Loop&) {
                               stack.push_back({ i, i });
                           },
                           [&](const lir::Unimplemented&) {},
                           [&](const auto&) {
                               stack.push_back({ i, i });
                           },
                       },
                       instruction(i));
        }

        plan.sends.resize(sends.size());
        for (const auto& s : sends)
            plan.send_instructions.push_back(s.instruction);
        for (std::size_t s { sends.size() }; s-- > 0;) {
            const auto i = sends[s].instruction;
            // Absorbed by a later send, which encodes it.
            if (plan.left_out(i)) {
                plan.sends[s].op = SendOp::None;
                continue;
            }
            const auto& send = std::get<lir::Send>(instruction(i));
            plan.sends[s] = decide(plan, send, args.data() + sends[s].first);
            if (plan.sends[s].add == AddSelector::AtReceiver)
                plan.additions.push_back({ args[sends[s].first].start, s });
            else if (plan.sends[s].add == AddSelector::AtSend)
                plan.additions.push_back({ i, s });
        }
        std::stable_sort(plan.additions.begin(), plan.additions.end(),
                         [](const auto& l, const auto& r) { return *l.first < *r.first; });
        return plan;
    }

    // `^nil`, `^true` and `^false` are one bytecode, when the value is the literal.
    void return_value(RegionPlan& plan, Value v) const {
        if (v.pushed_by_one_instruction() && special_value(v.producer))
            plan.leave_out(v.producer);
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

    static void push_integer(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, std::int32_t value) {
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

    void push_constant_entry(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, ConstantContent entry) {
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

    void push_class(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, ir::SymSCClassName name) {
        if (const auto fnd = ctx.special_symbols.special_classes.find(ir::Symbol { *name });
            fnd != ctx.special_symbols.special_classes.end())
            PushSpecialClass.emit(bc, loc, Operands::SpecialClass::fromRaw(fnd->second));
        else
            PushClassX.emit(bc, loc, Operands::Class::fromRaw(*add_selector(name)));
    }

    // As PyrSlotNode::compilePushLit, the values with their own bytecodes first.
    void push_constant(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, ir::ConstExprIndex c) {
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

    void push_local(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, lir::LocalRef l) {
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

    // Whether it popped the value, which only the short bytecodes can.
    bool store(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, const lir::Instruction& i, bool drop) {
        return std::visit(util::overload {
                              [&](const lir::StoreLocal& s) {
                                  const auto [depth, slot] = s.local;
                                  assert(depth <= std::numeric_limits<std::uint8_t>::max());
                                  if (drop && *slot <= 15 && depth < 8) {
                                      StoreTempVar.emit(bc, loc, depth, Operands::Index::fromRaw(*slot));
                                      return true;
                                  }
                                  StoreTempVarX.emit(bc, loc, Operands::FrameOffset::fromRaw(static_cast<Byte>(depth)),
                                                     Operands::Index::fromRaw(*slot));
                                  return false;
                              },
                              [&](const lir::StoreInstance& s) {
                                  const auto slot = *s.slot;
                                  if (drop && slot <= 15) {
                                      StoreInstVar.emit(bc, loc, slot);
                                      return true;
                                  }
                                  StoreInstVarX.emit(bc, loc, Operands::Index::fromRaw(static_cast<int>(slot)));
                                  return false;
                              },
                              [&](const lir::StoreClassVariable& s) {
                                  const std::uint32_t index { *s.index };
                                  if (drop && index < 4096) {
                                      StoreClassVar.emit(bc, loc, index);
                                      return true;
                                  }
                                  StoreClassVarX.emit(bc, loc, Operands::UnsignedInt<16, 1>::fromFull(index),
                                                      Operands::UnsignedInt<16, 0>::fromFull(index));
                                  return false;
                              },
                              [](const auto&) {
                                  assert(false);
                                  return false;
                              },
                          },
                          i);
    }

    void send(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, const lir::Send& s, const SendPlan& p) {
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
        case SendOp::UnaryAfterDrops:
            for (std::uint8_t d { 0 }; d < p.drops; ++d)
                Drop.emit(bc, loc);
            // The old compiler always used the short form, which only has the first 16.
            if (static_cast<Byte>(unary()) < static_cast<Byte>(OpUnaryMathNibble::COUNT))
                SendSpecialUnaryArithMsg.emit(bc, loc, { static_cast<OpUnaryMathNibble>(unary()) });
            else
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

    void special(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, ir::SCSpecial::Kind kind) {
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
    void default_argument(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, const lir::DefaultArgument& d) {
        push_local(bc, loc, { 0, d.slot });
        auto value = encode(function.sub_regions[d.value]);
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

    CompilingBytecodes encode(lir::FunctionSubRegion region) {
        auto plan = plan_region(region);
        CompilingBytecodes bc;
        const std::uint32_t begin { *region.begin };
        const std::uint32_t end { begin + region.size };
        std::size_t next_send { 0 };
        std::size_t next_addition { 0 };
        bool popped_by_store { false };
        for (std::uint32_t k { begin }; k < end; ++k) {
            const lir::InstructionIndex i { k };
            while (next_addition < plan.additions.size() && plan.additions[next_addition].first == i) {
                const auto s = plan.additions[next_addition].second;
                // Added at the send, after its arguments, it is added when it is encoded.
                if (plan.sends[s].add == AddSelector::AtReceiver)
                    plan.sends[s].selector =
                        add_selector(std::get<lir::Send>(instruction(plan.send_instructions[s])).selector);
                ++next_addition;
            }
            const auto& in = instruction(i);
            const auto loc = location(i);
            const bool left_out = plan.left_out(i);
            if (const auto* s = std::get_if<lir::Send>(&in)) {
                auto& p = plan.sends[next_send++];
                if (!left_out) {
                    if (p.add == AddSelector::AtSend)
                        p.selector = add_selector(s->selector);
                    send(bc, loc, *s, p);
                }
                continue;
            }
            if (left_out)
                continue;
            if (popped_by_store) {
                popped_by_store = false;
                assert(std::holds_alternative<lir::Drop>(in));
                continue;
            }
            std::visit(
                util::overload {
                    [&](const lir::PushLocal& p) { push_local(bc, loc, p.local); },
                    [&](const lir::PushInstance& p) {
                        PushInstVarX.emit(bc, loc, Operands::Index::fromRaw(static_cast<int>(*p.slot)));
                    },
                    [&](const lir::PushClassVariable& p) {
                        const std::uint32_t index { *p.index };
                        if (PushClassVar.validNibble(index))
                            PushClassVar.emit(bc, loc, index);
                        else
                            PushClassVarX.emit(bc, loc, Operands::UnsignedInt<16, 1>::fromFull(index),
                                               Operands::UnsignedInt<16, 0>::fromFull(index));
                    },
                    [&](const lir::PushConstant& p) { push_constant(bc, loc, p.value); },
                    [&](const lir::PushClassConstant& p) {
                        const auto& member = ctx.graph.member(p.member);
                        push_constant_entry(bc, loc, ClassConstant { member.owner, member.name, p.value });
                    },
                    [&](const lir::PushSpecialValue& p) {
                        PushSpecialValue.emit(bc, loc,
                                              { p.value == lir::SpecialValue::Nil        ? OpSpecialValue::Nil_
                                                    : p.value == lir::SpecialValue::True ? OpSpecialValue::True
                                                                                         : OpSpecialValue::False });
                    },
                    [&](const lir::PushInteger& p) { push_integer(bc, loc, p.value); },
                    [&](const lir::PushSymbol& p) { push_constant_entry(bc, loc, p.value); },
                    [&](const lir::PushClass& p) { push_class(bc, loc, p.name); },
                    [&](const lir::PushSpecial& p) { special(bc, loc, p.kind); },
                    [&](const lir::PushFunction& p) {
                        PushLiteralX.emit(bc, loc, Operands::Index::fromRaw(*add_selector(p.function)));
                    },
                    [&](const lir::StoreLocal&) { popped_by_store = store(bc, loc, in, drop_follows(plan, i)); },
                    [&](const lir::StoreInstance&) { popped_by_store = store(bc, loc, in, drop_follows(plan, i)); },
                    [&](const lir::StoreClassVariable&) {
                        popped_by_store = store(bc, loc, in, drop_follows(plan, i));
                    },
                    [&](const lir::Dup&) { Dup.emit(bc, loc); },
                    [&](const lir::Drop&) { Drop.emit(bc, loc); },
                    [&](const lir::Send&) {},
                    [&](const lir::Return&) { emit_return(bc, loc, plan, i); },
                    [&](const lir::NonLocalReturn&) { emit_return(bc, loc, plan, i); },
                    [&](const lir::ReturnSelf&) { ReturnSelf.emit(bc, loc); },
                    [&](const lir::DefaultArgument& d) { default_argument(bc, loc, d); },
                    [&](const lir::If& b) {
                        branch(bc, loc, b.then_body, b.else_body, JumpIfFalsePushNil, JumpIfFalse);
                    },
                    [&](const lir::IfNil& b) {
                        branch(bc, loc, b.then_body, b.else_body, Extended::IfNotNilJumpPushNilElsePop.Jump,
                               Extended::IfNotNilJump.Jump);
                    },
                    [&](const lir::IfNotNil& b) {
                        branch(bc, loc, b.then_body, b.else_body, Extended::IfNilThenJumpElsePopNil.Jump,
                               Extended::IfNilJump.Jump);
                    },
                    [&](const lir::And& b) { jump_over(bc, loc, JumpIfFalsePushFalse, b.rhs); },
                    [&](const lir::Or& b) { jump_over(bc, loc, JumpIfTruePushTrue, b.rhs); },
                    [&](const lir::OrIfNil& b) { jump_over(bc, loc, Extended::DoubleQuestionMark.Jump, b.rhs); },
                    [&](const lir::AndIfNotNil& b) {
                        jump_over(bc, loc, Extended::IfNilThenJumpElsePopNil.Jump, b.rhs);
                    },
                    [&](const lir::QuestionMark&) { Extended::QuestionMark.emit(bc, loc); },
                    [&](const lir::While& w) { while_(bc, loc, w); },
                    [&](const lir::Loop& l) {
                        auto body = encode_value(l.body, loc);
                        const auto length = body.length();
                        bc.consume(std::move(body));
                        jump(bc, loc, JumpBack, length + 1);
                    },
                    [&](const lir::Switch& s) { switch_(bc, loc, s); },
                    [&](const lir::Unimplemented&) {
                        assert(false);
                        failed = true;
                    },
                },
                in);
        }
        return bc;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Inlined sends, laid out as compileIfMsg and its neighbours in PyrParseNode.cpp did
    ////////////////////////////////////////////////////////////////////////////////

    template <typename Op>
    void jump(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, const Op& op, std::size_t length) {
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
    CompilingBytecodes encode_value(lir::FunctionSubRegionIndex r, sc::lex::SourceCodeRange loc) {
        auto bc = encode(function.sub_regions[r]);
        if (bc.length() == 0)
            PushSpecialValue.emit(bc, loc, { OpSpecialValue::Nil_ });
        return bc;
    }

    // short_jump skips the then body leaving nil, long_jump goes to the else body. The else body is encoded first, as
    // the old compiler did, which orders the tables.
    template <typename Short, typename Long>
    void branch(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, lir::FunctionSubRegionIndex then_body,
                lir::OptionalFunctionSubRegionIndex else_body, const Short& short_jump, const Long& long_jump) {
        std::optional<CompilingBytecodes> else_bc;
        if (else_body)
            else_bc = encode(function.sub_regions[lir::FunctionSubRegionIndex { *else_body }]);
        auto then_bc = encode(function.sub_regions[then_body]);
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
    void jump_over(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, const Op& op,
                   lir::FunctionSubRegionIndex rhs) {
        auto rhs_bc = encode_value(rhs, loc);
        jump(bc, loc, op, rhs_bc.length());
        bc.consume(std::move(rhs_bc));
    }

    // The jump back drops the body's value. The lengths are the old compiler's.
    void while_(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, const lir::While& w) {
        auto condition = encode_value(w.condition, loc);
        CompilingBytecodes body;
        if (w.body)
            body = encode_value(lir::FunctionSubRegionIndex { *w.body }, loc);
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
    void switch_(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, const lir::Switch& s) {
        const auto table_index = out.switch_tables.push_back({});
        push_constant_entry(bc, loc, table_index);
        Extended::Switch.emit(bc, loc);

        std::vector<CompilingBytecodes> bodies;
        bool has_default { false };
        for (std::uint32_t c { 0 }; c < s.count; ++c) {
            const auto& sc = function.switch_cases[lir::switch_case(s, c)];
            bodies.push_back(encode_value(sc.body, loc));
            has_default = !sc.key;
        }
        if (!has_default) {
            bodies.emplace_back();
            PushSpecialValue.emit(bodies.back(), loc, { OpSpecialValue::Nil_ });
        }
        const std::size_t num_keys = bodies.size() - 1;
        constexpr std::size_t jump_size { 3 };
        std::size_t end { 0 };
        for (std::size_t b { 0 }; b < bodies.size(); ++b)
            end += bodies[b].length() + (b < num_keys ? jump_size : 0);

        // The receiver, then a key and a body each, then the default, as the old compiler sized it.
        const std::size_t num_args = 1 + 2 * num_keys + (has_default ? 1 : 0);
        auto& table = out.switch_tables[table_index];
        table.capacity = 1;
        while (table.capacity < 2 * num_args)
            table.capacity *= 2;
        std::size_t offset { 0 };
        for (std::size_t b { 0 }; b < num_keys; ++b) {
            const ir::ConstExprIndex key { *function.switch_cases[lir::switch_case(s, b)].key };
            // The first of equal keys is kept, as Object:switch tests them in order. The old compiler kept the last.
            const auto identity = identity_of(ctx.constants, key);
            if (std::none_of(table.offsets.begin(), table.offsets.end(),
                             [&](const auto& o) { return identity_of(ctx.constants, o.first) == identity; }))
                table.offsets.push_back({ key, CodeOffset::from(offset) });
            offset += bodies[b].length() + jump_size;
            jump(bodies[b], loc, JumpFwd, end - offset);
        }
        table.default_offset = CodeOffset::from(offset);
        for (auto& body : bodies)
            bc.consume(std::move(body));
    }

    [[nodiscard]] bool drop_follows(const RegionPlan& plan, lir::InstructionIndex i) const {
        const lir::InstructionIndex next { *i + 1 };
        return plan.contains(next) && !plan.left_out(next) && std::holds_alternative<lir::Drop>(instruction(next));
    }

    void emit_return(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, const RegionPlan& plan,
                     lir::InstructionIndex i) {
        const lir::InstructionIndex previous { *i - 1 };
        if (*i > 0 && plan.contains(previous) && plan.left_out(previous))
            if (const auto value = special_value(previous)) {
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

    // A function, not a method, ends with BlockReturn, after its body leaves its value on the stack.
    void run(bool is_method) {
        auto bc = encode(function.sub_regions[lir::body_sub_region]);
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
    if (const auto f = ctx.graph.as_a<ir::SCFunction_I>(ir::NodeIndex { *frame.owner }))
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
    for (auto fi = static_cast<std::uint32_t>(functions.size()); fi-- > 0;) {
        const lir::FunctionIndex index { fi };
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

        if (ctx.graph.is_a<ir::SCMethod_I>(ir::NodeIndex { *frame.owner }))
            continue;
        properties.is_closed = !used.cant_be_closed && used.outer_depth == 0;
        properties.has_context = !properties.is_closed && !explicitly_closed(ctx, frame);
    }
    return out;
}
}

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

EmittedGraph emit(const EmitContext& ctx, const lir::LoweredGraph& lowered) {
    EmittedGraph out;
    const auto properties = function_properties(ctx, lowered.functions);
    // Nested functions follow their root, a function whose frame has no parent.
    std::optional<lir::FunctionIndex> root;
    bool root_failed { false };
    const auto finish_root = [&]() {
        if (root && !root_failed)
            out.roots.push_back(*root);
    };
    for (std::uint32_t fi { 0 }; fi < lowered.functions.size(); ++fi) {
        const lir::FunctionIndex index { fi };
        const auto& function = lowered.functions[index];
        const auto& frame = ctx.frames.frames[function.frame];
        if (!frame.parent) {
            finish_root();
            root = index;
            root_failed = std::find(lowered.roots.begin(), lowered.roots.end(), index) == lowered.roots.end();
        }
        if (root_failed) {
            out.functions.push_back({});
            continue;
        }
        Emitter emitter { ctx, function, frame, {}, out.diagnostics };
        emitter.run(ctx.graph.is_a<ir::SCMethod_I>(ir::NodeIndex { *frame.owner }));
        root_failed = emitter.failed;
        emitter.out.properties = properties[index];
        out.functions.push_back(std::move(emitter.out));
    }
    finish_root();
    return out;
}

////////////////////////////////////////////////////////////////////////////////
// Dump
////////////////////////////////////////////////////////////////////////////////

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
    std::size_t offset { 0 };
    for (const auto size : code.sizeOfCodes) {
        out << indent << std::setw(4) << std::left << offset << std::right << ' ';
        std::string bytes;
        for (std::size_t b { 0 }; b < size; ++b) {
            static constexpr char digits[] = "0123456789ABCDEF";
            const auto byte = code.codes[offset + b];
            bytes += digits[byte >> 4];
            bytes += digits[byte & 15];
            bytes += ' ';
        }
        out << std::setw(16) << std::left << bytes << std::right << opcode_name(code.codes[offset]) << "\n";
        offset += size;
    }
}

}
