#include "sc_backend/lower.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_sema/versions.hpp"
#include "sc_util/overload.hpp"

#include <cassert>
#include <limits>
#include <vector>

namespace sc::backend {

////////////////////////////////////////////////////////////////////////////////
// Lowering symbols
////////////////////////////////////////////////////////////////////////////////

LoweringSymbols::LoweringSymbols(ir::SymbolTable& symbols):
    new_(symbols.get_selector("new")),
    add(symbols.get_selector("add")),
    put(symbols.get_selector("put")),
    at(symbols.get_selector("at")),
    copy(symbols.get_selector("copy")),
    copy_to_end(symbols.get_selector("copyToEnd")),
    series(symbols.get_selector("prSimpleNumberSeries")),
    envir_get(symbols.get_selector("envirGet")),
    envir_put(symbols.get_selector("envirPut")),
    perform_list(symbols.get_selector("performList")),
    super_perform_list(symbols.get_selector("superPerformList")),
    if_(symbols.get_selector("if")),
    r(symbols.get_selector("r")),
    do_(symbols.get_selector("do")),
    for_series(symbols.get_selector("forSeries")),
    value(symbols.get_selector("value")),
    yield(symbols.get_selector("yield")),
    always_yield(symbols.get_selector("alwaysYield")),
    series_iter(symbols.get_selector("seriesIter")) {}

////////////////////////////////////////////////////////////////////////////////
// Lowerer
////////////////////////////////////////////////////////////////////////////////

namespace {

// The functions of an inlined `case`, the receiver then the arguments.
using CaseFunctionIndex = util::StrongIndex<std::uint32_t, struct CaseFunctionIndex__>;
using CaseFunctions = util::IndexedVector<CaseFunctionIndex, ir::SCFunction_I>;

// A generator's qualifiers, in the order they are written.
using QualifierIndex = util::StrongIndex<std::uint32_t, struct QualifierIndex__>;

// Instructions lowered but not yet placed in their function.
using PendingIndex = util::StrongIndex<std::uint32_t, struct PendingIndex__>;
using PendingBodyIndex = util::StrongIndex<std::uint32_t, struct PendingBodyIndex__>;

struct BodyBuild {
    util::IndexedVector<PendingIndex, lir::Instruction> instructions {};
    util::IndexedVector<PendingIndex, lir::SourceSpan> spans {};
};

struct Lowerer {
    const LoweringContext& ctx;
    const ir::IRGraph& ir_graph;
    // Must walk in layout_frames' order.
    FrameCursor cursor;
    lir::LoweredGraph out {};

    // Innermost last.
    struct Building {
        lir::FunctionIndex function;
        bool is_method;
        // Its own body's start in body_starts.
        PendingBodyIndex first_body;
    };
    std::vector<Building> building {};
    // Bodies nest, e.g., a default argument's value in a function's body, and functions nest, so all are built on one
    // stack, each from its start, and appended to its function when it ends, keeping each contiguous.
    BodyBuild pending {};
    util::IndexedVector<PendingBodyIndex, PendingIndex> body_starts {};
    // The root being lowered can't be emitted, e.g., a frame is over the VM's limit.
    bool failed { false };
    // Legacy code only uses literals' values, a read of a variable sema knows is 1 still reads it.
    bool fold_constants { false };

    ////////////////////////////////////////////////////////////////////////////////
    // Functions and bodies
    ////////////////////////////////////////////////////////////////////////////////

    lir::Function& function() { return out.functions[building.back().function]; }

    void emit(lir::Instruction instruction, ir::NodeIndex at) {
        assert(!body_starts.empty());
        const auto range = ir::IRGraph::range(ir_graph.location(at));
        (void)pending.instructions.push_back(instruction);
        (void)pending.spans.push_back({ range.begin.absolute, range.end.absolute });
    }

    void begin_body() { (void)body_starts.push_back(pending.instructions.next_index()); }

    // Removes the innermost body from the stack.
    template <typename Take> void pop_body(const Take& take) {
        const auto start = static_cast<std::ptrdiff_t>(*body_starts.back());
        body_starts.pop_back();
        auto& instructions = pending.instructions.raw();
        auto& spans = pending.spans.raw();
        take(instructions.begin() + start, instructions.end(), spans.begin() + start, spans.end());
        instructions.erase(instructions.begin() + start, instructions.end());
        spans.erase(spans.begin() + start, spans.end());
    }

    lir::Body append_body() {
        auto& f = function();
        lir::Body placed { f.instructions.next_index(), 0 };
        pop_body([&](auto i_begin, auto i_end, auto s_begin, auto s_end) {
            placed.size = static_cast<std::uint32_t>(i_end - i_begin);
            f.instructions.raw().insert(f.instructions.raw().end(), i_begin, i_end);
            f.spans.raw().insert(f.spans.raw().end(), s_begin, s_end);
        });
        return placed;
    }

    lir::BodyIndex end_body() {
        const auto placed = append_body();
        return function().bodies.push_back(placed);
    }

    // Instructions lowered before they are placed, e.g., an operand pushed after a function lowered later. Any
    // bodies they hold are already placed.
    BodyBuild take_body() {
        BodyBuild taken;
        pop_body([&](auto i_begin, auto i_end, auto s_begin, auto s_end) {
            taken.instructions.raw().assign(i_begin, i_end);
            taken.spans.raw().assign(s_begin, s_end);
        });
        return taken;
    }

    void place(const BodyBuild& taken) {
        assert(!body_starts.empty());
        auto& instructions = pending.instructions.raw();
        auto& spans = pending.spans.raw();
        instructions.insert(instructions.end(), taken.instructions.raw().begin(), taken.instructions.raw().end());
        spans.insert(spans.end(), taken.spans.raw().begin(), taken.spans.raw().end());
    }

    // The function's own body is body 0, reserved first though it ends last.
    lir::FunctionIndex begin_function(FrameIndex frame, ir::NodeIndex owner) {
        lir::Function f { frame, ir_graph.location(owner).source };
        f.bodies.push_back({ lir::InstructionIndex { 0 }, 0 });
        const auto index = out.functions.push_back(std::move(f));
        building.push_back({ index, ir_graph.is_a<ir::SCMethod_I>(owner), body_starts.next_index() });
        begin_body();
        return index;
    }

    void end_function() {
        assert(body_starts.size() == *building.back().first_body + 1);
        const auto sub = append_body();
        function().bodies[lir::function_body] = sub;
        building.pop_back();
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Operands
    ////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] ir::SymSCSymbol symbol(ir::Symbol name) const { return ctx.symbols.get_symbol(ctx.symbols(name)); }

    [[nodiscard]] lir::SlotIndex own_slot(ir::LocalIndex l) { return addressable(ctx.frames.locations[l].slot); }

    [[nodiscard]] lir::SlotIndex addressable(FrameSlotIndex slot) {
        if (*slot > std::numeric_limits<std::uint8_t>::max()) {
            failed = true; // Reported by layout_frames.
            return lir::SlotIndex { 0 };
        }
        return lir::SlotIndex { static_cast<std::uint8_t>(*slot) };
    }

    [[nodiscard]] lir::LocalRef local_ref(ir::LocalIndex l) {
        const auto& slot = ctx.frames.locations[l];
        assert(slot.frame && cursor.current());
        const auto depth = ctx.frames.depth(*cursor.current(), FrameIndex { *slot.frame });
        assert(depth <= std::numeric_limits<std::uint16_t>::max());
        return { static_cast<std::uint16_t>(depth), addressable(slot.slot) };
    }

    void push_member(ir::MemberIndex m, ir::NodeIndex at) {
        const auto& variable = ir_graph.member(m);
        const auto location = ctx.members.find(variable.owner, variable.name);
        if (!location) {
            emit(lir::Unimplemented { at }, at);
            return;
        }
        std::visit(util::overload {
                       [&](InstanceSlotIndex slot) { emit(lir::PushInstance { slot }, at); },
                       [&](ClassVariableIndex index) { emit(lir::PushClassVariable { index }, at); },
                       // From 3.16 an immutable constant is pushed as a literal would be, a mutable one is still
                       // the class's object, so every use sees the same one.
                       [&](ir::OptionalConstExprIndex value) {
                           if (!value)
                               emit(lir::PushSpecialValue { lir::SpecialValue::Nil }, at);
                           else if (fold_constants && !is_mutable(ir::ConstExprIndex { *value }))
                               emit(lir::PushConstant { ir::ConstExprIndex { *value } }, at);
                           else
                               emit(lir::PushClassConstant { m, ir::ConstExprIndex { *value } }, at);
                       },
                   },
                   *location);
    }

    void store_member(ir::MemberIndex m, ir::NodeIndex at) {
        const auto& variable = ir_graph.member(m);
        const auto location = ctx.members.find(variable.owner, variable.name);
        if (!location) {
            emit(lir::Unimplemented { at }, at);
            return;
        }
        std::visit(util::overload {
                       [&](InstanceSlotIndex slot) { emit(lir::StoreInstance { slot }, at); },
                       [&](ClassVariableIndex index) { emit(lir::StoreClassVariable { index }, at); },
                       // Assigning a constant is an error, so its method is poisoned and never lowered.
                       [&](ir::OptionalConstExprIndex) { emit(lir::Unimplemented { at }, at); },
                   },
                   *location);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Constants
    ////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] bool is_mutable(ir::ConstExprIndex c) const { return ctx.constants(c).is_mutable(); }

    // A literal is shared, a folded mutable constant is a new copy each time.
    [[nodiscard]] bool is_literal(ir::NodeIndex i) const {
        if (ir_graph.is_a<ir::SCLiteral_I>(i))
            return true;
        const auto collection = ir_graph.as_a<ir::SCCollection_I>(i);
        return collection && ir_graph.payload(*collection).literal;
    }

    void push_constant(ir::NodeIndex i, ir::ConstExprIndex c) {
        if (is_literal(i) || !is_mutable(c)) {
            emit(lir::PushConstant { c }, i);
            return;
        }

        // `(1; "a")` is its last expression's value, so is shared if that is a literal.
        if (const auto list = ir_graph.as_a<ir::SCExprList_I>(i)) {
            if (const auto last = ir_graph.last_child(*list)) {
                push_constant(ir::NodeIndex { *last }, c);
                return;
            }
        }
        push_copy(c, i);
    }

    // `copy`, then each mutable element copied and put, as constants have no cycles to need deepCopy.
    void push_copy(ir::ConstExprIndex c, ir::NodeIndex at) {
        emit(lir::PushConstant { c }, at);
        emit(lir::Send { ctx.selectors.copy, 0, 0, lir::SendKind::Normal, lir::SendForm::Implicit, lir::Tail::None },
             at);

        const auto push_element = [&](ir::ConstExprIndex element) {
            if (is_mutable(element))
                push_copy(element, at);
            else
                emit(lir::PushConstant { element }, at);
        };

        const auto place_element = [&](auto push_key, ir::ConstExprIndex element) {
            emit(lir::Dup {}, at);
            push_key();
            push_element(element);
            emit(lir::Send { ctx.selectors.put, 2, 0, lir::SendKind::Normal, lir::SendForm::Implicit, lir::Tail::None },
                 at);
            emit(lir::Drop {}, at);
        };

        const auto& value = ctx.constants(c);
        if (const auto* array = value.get_if<ir::ConstExprArray>()) {
            for (const auto k : array->indices())
                if (is_mutable((*array)[k]))
                    place_element([&] { emit(lir::PushInteger { static_cast<std::int32_t>(*k) }, at); }, (*array)[k]);
        } else if (const auto* event = value.get_if<ir::ConstExprEvent>()) {
            for (const auto& [key, element] : *event)
                if (is_mutable(key) || is_mutable(element))
                    place_element([&, key = key] { push_element(key); }, element);
        }
    }


    ////////////////////////////////////////////////////////////////////////////////
    // Expressions
    ////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] bool missing(ir::NodeIndex i) const { return ir_graph.is_a<ir::SCMissing_I>(i); }

    [[nodiscard]] std::size_t num_variadic_args(ir::NodeIndex variadic) const {
        std::size_t count { 0 };
        if (const auto list = ir_graph.as_a<ir::SCVariadicArgList_I>(variadic))
            for (auto it = ir_graph.children(*list).iter(); it; ++it)
                ++count;
        return count;
    }

    [[nodiscard]] bool lowered_yet(ir::NodeIndex i) const {
        if (const auto message = ir_graph.as_a<ir::SCMessage_I>(i)) {
            // performList takes one array, the old grammar only allowed one.
            const auto [r, p, k, variadic, a] = ir_graph.children(*message);
            if (num_variadic_args(variadic) > 1)
                return false;
        }
        return true;
    }

    void lower(ir::NodeIndex i, lir::Tail tail = lir::Tail::None) {
        if (!lowered_yet(i)) {
            emit(lir::Unimplemented { i }, i);
            failed = true;
            cursor.skip_over_children(i);
            return;
        }
        if (const auto c = ir_graph.node_as_constexpr(i); c && (fold_constants || is_literal(i))) {
            push_constant(i, *c);
            cursor.skip_over_children(i);
            return;
        }
        const auto started = cursor.enter(i);
        std::visit(util::overload {
                       [&](const auto&) {
                           emit(lir::Unimplemented { i }, i);
                           failed = true;
                       },
                       [&](const ir::SCLiteral& l) { emit(lir::PushConstant { l.index }, i); },
                       [&](const ir::SCRead& r) { read(i, r, tail); },
                       [&](const ir::SCAssign&) { assign(ir_graph.assume_a<ir::SCAssign_I>(i), tail); },
                       [&](const ir::SCSpecial& s) {
                           // `super` is `this`, only the send it receives differs.
                           const auto kind = s.kind == ir::SCSpecial::Kind::Super ? ir::SCSpecial::Kind::This : s.kind;
                           emit(lir::PushSpecial { kind }, i);
                       },
                       [&](const ir::SCMessage& m) { message(ir_graph.assume_a<ir::SCMessage_I>(i), m, tail); },
                       [&](const ir::SCExprList&) { expression_list(ir_graph.assume_a<ir::SCExprList_I>(i), tail); },
                       [&](const ir::SCCollection& c) { collection(ir_graph.assume_a<ir::SCCollection_I>(i), c); },
                       [&](const ir::SCRange&) { range(ir_graph.assume_a<ir::SCRange_I>(i), tail); },
                       [&](const ir::SCRangeGenerator&) {
                           const auto r = ir_graph.assume_a<ir::SCRangeGenerator_I>(i);
                           series_operands(r);
                           emit(lir::Send { ctx.selectors.series_iter, 2, 0, lir::SendKind::Normal, lir::SendForm::Call,
                                            tail },
                                i);
                       },
                       [&](const ir::SCRoutineGenerator&) {
                           routine(ir_graph.assume_a<ir::SCRoutineGenerator_I>(i), *started, tail);
                       },
                       [&](const ir::SCImmediateGenerator&) {
                           const auto [qualifiers, value] =
                               ir_graph.children(ir_graph.assume_a<ir::SCImmediateGenerator_I>(i));
                           generator(qualifiers, value, false, tail);
                       },
                       [&](const ir::SCPackAssign&) {
                           const auto [value, targets] = ir_graph.children(ir_graph.assume_a<ir::SCPackAssign_I>(i));
                           pack(value, targets, i);
                       },
                       [&](const ir::SCExplicitReturn&) {
                           const auto [value] = ir_graph.children(ir_graph.assume_a<ir::SCExplicitReturn_I>(i));
                           lower(value, lir::Tail::ReturnFromMethod);
                           if (building.back().is_method)
                               emit(lir::Return {}, i);
                           else
                               emit(lir::NonLocalReturn {}, i);
                       },
                       [&](const ir::SCFunction&) {
                           const auto f = ir_graph.assume_a<ir::SCFunction_I>(i);
                           const auto index = begin_function(*started, i);
                           contents(f, false);
                           end_function();
                           emit(lir::PushFunction { index }, i);
                       },
                   },
                   ir_graph.payload(i));
        (void)cursor.exit(i);
    }

    void read(ir::NodeIndex i, const ir::SCRead& r, lir::Tail tail) {
        switch (r.kind) {
        case ir::SCRead::Kind::Local:
            emit(lir::PushLocal { local_ref(ir::LocalIndex { *r.local }) }, i);
            return;
        case ir::SCRead::Kind::Instance:
        case ir::SCRead::Kind::Class:
            push_member(ir::MemberIndex { *r.member }, i);
            return;
        // `~x` is `\x.envirGet`.
        case ir::SCRead::Kind::Environment:
            emit(lir::PushSymbol { symbol(r.name) }, i);
            emit(lir::Send { ctx.selectors.envir_get, 0, 0, lir::SendKind::Normal, lir::SendForm::Call, tail }, i);
            return;
        }
    }

    void assign(ir::SCAssign_I a, lir::Tail tail) {
        const ir::NodeIndex i { *a };
        const auto& payload = ir_graph.payload(a);
        const auto [value] = ir_graph.children(a);
        // `~x = v` is `\x.envirPut(v)`.
        if (payload.kind == ir::SCAssign::Kind::Environment) {
            emit(lir::PushSymbol { symbol(*payload.name) }, i);
            lower(value);
            emit(lir::Send { ctx.selectors.envir_put, 1, 0, lir::SendKind::Normal, lir::SendForm::Call, tail }, i);
            return;
        }
        lower(value);
        if (payload.kind == ir::SCAssign::Kind::Local)
            emit(lir::StoreLocal { local_ref(ir::LocalIndex { *payload.local }) }, i);
        else
            store_member(ir::MemberIndex { *payload.member }, i);
    }

    [[nodiscard]] std::uint8_t count(std::uint32_t n) {
        if (n > std::numeric_limits<std::uint8_t>::max()) {
            failed = true; // More arguments than the VM's argument count byte.
            return 0;
        }
        return static_cast<std::uint8_t>(n);
    }

    // `r.sel(a, k: v)`. With a variadic argument, `r.sel(a, *v)` is `r.performList(\sel, a, v)`, and `super` is
    // `this.superPerformList(\sel, ...)`, as the old grammar did. Keyword arguments are kept either way.
    void message(ir::SCMessage_I m, const ir::SCMessage& payload, lir::Tail tail) {
        if (const auto kind = inlining(ctx.inlining, ir_graph, m, cursor.version())) {
            inlined(m, *kind, tail);
            return;
        }
        const ir::NodeIndex i { *m };
        const auto [receiver, positional, keyword, variadic, adverb] = ir_graph.children(m);
        const ir::NodeIndex receiver_i { *receiver };
        const auto special = ir_graph.as_a<ir::SCSpecial_I>(receiver_i);
        const bool super = special && ir_graph.payload(*special).kind == ir::SCSpecial::Kind::Super;
        const bool has_variadic = num_variadic_args(variadic) > 0;
        const bool has_adverb = !missing(adverb);

        lower(receiver_i);
        std::uint32_t num_positional { 0 };
        if (has_variadic) {
            emit(lir::PushSymbol { symbol(*payload.selector) }, i);
            ++num_positional;
        }
        if (const auto list = ir_graph.as_a<ir::SCPosArgList_I>(positional))
            for (auto it = ir_graph.children(*list).iter(); it; ++it, ++num_positional)
                lower(*it);
        if (const auto list = ir_graph.as_a<ir::SCVariadicArgList_I>(variadic))
            for (auto it = ir_graph.children(*list).iter(); it; ++it, ++num_positional)
                lower(*it);
        // An adverb is passed as the last argument, `a foo.x: b` is `a.foo(b, \x)`, as the old compiler did.
        if (has_adverb) {
            lower(adverb);
            ++num_positional;
        }
        std::uint32_t num_keyword { 0 };
        if (const auto list = ir_graph.as_a<ir::SCKwArgList_I>(keyword))
            for (auto it = ir_graph.children(*list).iter(); it; ++it, ++num_keyword) {
                const auto [value] = ir_graph.children(*it);
                emit(lir::PushSymbol { symbol(*ir_graph.payload(*it).name) }, *it);
                lower(value);
            }

        auto selector = *payload.selector;
        auto kind = lir::SendKind::Normal;
        if (has_variadic)
            selector = super ? ctx.selectors.super_perform_list : ctx.selectors.perform_list;
        else if (super)
            kind = lir::SendKind::Super;
        else if (has_adverb)
            kind = lir::SendKind::WithAdverb;
        emit(lir::Send { selector, count(num_positional), count(num_keyword), kind, form(payload.form), tail }, i);
    }

    [[nodiscard]] static lir::SendForm form(ir::SCMessage::Form f) {
        switch (f) {
        case ir::SCMessage::Form::Call:
            return lir::SendForm::Call;
        case ir::SCMessage::Form::Binary:
            return lir::SendForm::Binary;
        case ir::SCMessage::Form::Setter:
            return lir::SendForm::Setter;
        }
        return lir::SendForm::Call;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Inlined sends
    ////////////////////////////////////////////////////////////////////////////////

    // Leaves nothing for an empty body. Its variables were hoisted into the frame, so they start again each time it
    // runs, a body ending with one is nil.
    template <typename Last> void inline_body(ir::SCFunction_I f, const Last& last) {
        const ir::NodeIndex function { *f };
        // Starts no frame, it was inlined when the send was entered.
        (void)cursor.enter(function);
        const auto [positional, variadic, body] = ir_graph.children(f);
        const auto last_item = ir_graph.last_child(body);
        for (auto it = ir_graph.children(body).iter(); it; ++it) {
            const ir::NodeIndex item { **it };
            if (declaration(item, true)) {
                if (*item == *last_item)
                    emit(lir::PushSpecialValue { lir::SpecialValue::Nil }, item);
            } else if (*item == *last_item) {
                last(item);
            } else {
                lower(item);
                emit(lir::Drop {}, item);
            }
        }
        (void)cursor.exit(function);
    }

    void inline_body(ir::SCFunction_I f, lir::Tail tail) {
        inline_body(f, [&](ir::NodeIndex item) { lower(item, tail); });
    }

    lir::BodyIndex inlined_body(ir::SCFunction_I f, lir::Tail tail) {
        begin_body();
        inline_body(f, tail);
        return end_body();
    }

    // Lowers x of `x.isNil`, without the send.
    void nil_tested(ir::SCMessage_I test) {
        const ir::NodeIndex i { *test };
        (void)cursor.enter(i);
        const auto [receiver, positional, keyword, variadic, adverb] = ir_graph.children(test);
        lower(receiver);
        for (const ir::NodeIndex rest : std::initializer_list<ir::NodeIndex> { positional, keyword, variadic, adverb })
            cursor.skip_over_children(rest);
        (void)cursor.exit(i);
    }

    // The tested value of an inlined `if (c)`, but x of `x.isNil` or `x.notNil`.
    Inlined condition(ir::SCAnyExpr_I c) {
        if (const auto test = nil_test(ctx.inlining, ir_graph, c)) {
            nil_tested(test->message);
            return test->kind();
        }
        lower(c);
        return Inlined::If;
    }

    template <typename Else>
    void branch(Inlined kind, ir::SCFunction_I then_body, const Else& else_body, lir::Tail tail, ir::NodeIndex at) {
        const auto then_lowered = inlined_body(then_body, tail);
        emit_branch(kind, then_lowered, else_body(), at);
    }

    void emit_branch(Inlined kind, lir::BodyIndex then_body, lir::OptionalBodyIndex else_body, ir::NodeIndex at) {
        switch (kind) {
        case Inlined::IfNil:
            emit(lir::IfNil { then_body, else_body }, at);
            return;
        case Inlined::IfNotNil:
            emit(lir::IfNotNil { then_body, else_body }, at);
            return;
        default:
            emit(lir::If { then_body, else_body }, at);
            return;
        }
    }

    // `case { c } { a } { d } { b } { e }` is `if (c) { a } { if (d) { b } { e } }`, as the old compiler rewrote it.
    // An empty condition is nil.
    void case_(const CaseFunctions& functions, CaseFunctionIndex k, lir::Tail tail, ir::NodeIndex at) {
        const auto condition = functions[k];
        // A condition of only `x.isNil` is tested as an inlined `if` would.
        const auto test = nil_test_body(ctx.inlining, ir_graph, condition);
        if (test)
            inline_body(condition, [&](ir::NodeIndex) { nil_tested(test->message); });
        else
            inline_body(condition, lir::Tail::None);
        const auto [p, v, body] = ir_graph.children(condition);
        if (!ir_graph.first_child(body))
            emit(lir::PushSpecialValue { lir::SpecialValue::Nil }, condition);

        const auto remaining = functions.size() - *k;
        if (remaining == 1) {
            // `case { c }` is `c.if`.
            emit(lir::Send { ctx.selectors.if_, 0, 0, lir::SendKind::Normal, lir::SendForm::Call, tail }, at);
            return;
        }
        branch(
            test ? test->kind() : Inlined::If, functions[util::advance(k)],
            [&]() -> lir::OptionalBodyIndex {
                if (remaining == 2)
                    return {};
                if (remaining == 3)
                    return inlined_body(functions[util::advance(k, 2)], tail);
                begin_body();
                case_(functions, util::advance(k, 2), tail, at);
                return end_body();
            },
            tail, at);
    }

    // A nested switch adds its cases while these are lowered, so they are collected first, to be consecutive.
    void switch_(const Arguments& cases, lir::Tail tail, ir::NodeIndex at) {
        std::vector<lir::SwitchCase> lowered;
        ArgumentIndex key_i { 0 };
        for (; cases.contains(util::advance(key_i)); key_i = util::advance(key_i, 2)) {
            const auto key = switch_key(ctx.inlining, ir_graph, cases[key_i], cursor.version());
            cursor.skip_over_children(cases[key_i]);
            const auto body = inlined_body(ir_graph.assume_a<ir::SCFunction_I>(cases[util::advance(key_i)]), tail);
            lowered.push_back({ ir::OptionalConstExprIndex { *key }, body });
        }
        if (cases.contains(key_i))
            lowered.push_back({ {}, inlined_body(ir_graph.assume_a<ir::SCFunction_I>(cases[key_i]), tail) });
        auto& switch_cases = function().switch_cases;
        const auto first = switch_cases.next_index();
        for (const auto& c : lowered)
            (void)switch_cases.push_back(c);
        emit(lir::Switch { first, static_cast<std::uint32_t>(lowered.size()) }, at);
    }

    // Bodies are in tail position when the send is, except a loop's.
    void inlined(ir::SCMessage_I m, Inlined kind, lir::Tail tail) {
        const ir::NodeIndex i { *m };
        const auto [receiver, p, k, v, a] = ir_graph.children(m);
        const auto args = positional_arguments(ir_graph, m);

        switch (kind) {
        case Inlined::If:
            lower(receiver);
            break;
        case Inlined::IfNil:
        case Inlined::IfNotNil:
            nil_tested(nil_test(ctx.inlining, ir_graph, receiver)->message);
            break;
        default:
            break;
        }

        switch (kind) {
        case Inlined::If:
        case Inlined::IfNil:
        case Inlined::IfNotNil:
            branch(
                kind, ir_graph.assume_a<ir::SCFunction_I>(args[{ 0 }]),
                [&]() {
                    if (!args.contains({ 1 }))
                        return lir::OptionalBodyIndex {};
                    return lir::OptionalBodyIndex { inlined_body(ir_graph.assume_a<ir::SCFunction_I>(args[{ 1 }]),
                                                                 tail) };
                },
                tail, i);
            return;
        case Inlined::And:
            lower(receiver);
            emit(lir::And { inlined_body(ir_graph.assume_a<ir::SCFunction_I>(args[{ 0 }]), tail) }, i);
            return;
        case Inlined::Or:
            lower(receiver);
            emit(lir::Or { inlined_body(ir_graph.assume_a<ir::SCFunction_I>(args[{ 0 }]), tail) }, i);
            return;
        case Inlined::OrIfNil:
            lower(receiver);
            emit(lir::OrIfNil { inlined_body(ir_graph.assume_a<ir::SCFunction_I>(args[{ 0 }]), tail) }, i);
            return;
        case Inlined::AndIfNotNil:
            lower(receiver);
            emit(lir::AndIfNotNil { inlined_body(ir_graph.assume_a<ir::SCFunction_I>(args[{ 0 }]), tail) }, i);
            return;
        case Inlined::QuestionMark:
            lower(receiver);
            lower(args[{ 0 }]);
            emit(lir::QuestionMark {}, i);
            return;
        case Inlined::While: {
            const auto condition = inlined_body(ir_graph.assume_a<ir::SCFunction_I>(receiver), lir::Tail::None);
            lir::OptionalBodyIndex body {};
            if (args.contains({ 0 }))
                body = inlined_body(ir_graph.assume_a<ir::SCFunction_I>(args[{ 0 }]), lir::Tail::None);
            emit(lir::While { condition, body }, i);
            return;
        }
        case Inlined::WhileTrue:
            // `true` isn't evaluated.
            cursor.skip_over_children(receiver);
            emit(lir::Loop { inlined_body(ir_graph.assume_a<ir::SCFunction_I>(args[{ 0 }]), lir::Tail::None) }, i);
            return;
        case Inlined::Loop:
            emit(lir::Loop { inlined_body(ir_graph.assume_a<ir::SCFunction_I>(receiver), lir::Tail::None) }, i);
            return;
        case Inlined::Case: {
            CaseFunctions functions;
            (void)functions.push_back(ir_graph.assume_a<ir::SCFunction_I>(receiver));
            for (const auto argument : args.raw())
                (void)functions.push_back(ir_graph.assume_a<ir::SCFunction_I>(argument));
            case_(functions, CaseFunctionIndex { 0 }, tail, i);
            return;
        }
        case Inlined::Switch:
            lower(receiver);
            switch_(args, tail, i);
            return;
        }
    }

    void expression_list(ir::SCExprList_I list, lir::Tail tail) {
        const ir::NodeIndex i { *list };
        const auto last = ir_graph.last_child(i);
        if (!last) {
            emit(lir::PushSpecialValue { lir::SpecialValue::Nil }, i);
            return;
        }
        for (auto it = ir_graph.children(list).iter(); it; ++it) {
            const ir::NodeIndex item { **it };
            if (*item == *last) {
                lower(item, tail);
            } else {
                lower(item, lir::Tail::None);
                emit(lir::Drop {}, item);
            }
        }
    }

    // `(a: 1)` is `Event.new(1, nil, nil, true).put(\a, 1)`, any other, e.g., `[1, 2]` or `Set[1, 2]`, is
    // `Class.new(2).add(1).add(2)`, as the old compiler did.
    void collection(ir::SCCollection_I c, const ir::SCCollection& payload) {
        const ir::NodeIndex i { *c };
        const auto collection_class = *payload.collection_class;
        const bool dictionary = !payload.collection_class.location && collection_class == ctx.event;
        emit(lir::PushClass { collection_class }, i);
        if (dictionary) {
            emit(lir::PushInteger { static_cast<std::int32_t>(payload.size / 2) }, i);
            emit(lir::PushSpecialValue { lir::SpecialValue::Nil }, i);
            emit(lir::PushSpecialValue { lir::SpecialValue::Nil }, i);
            emit(lir::PushSpecialValue { lir::SpecialValue::True }, i);
            emit(
                lir::Send { ctx.selectors.new_, 4, 0, lir::SendKind::Normal, lir::SendForm::Implicit, lir::Tail::None },
                i);
            std::uint32_t k { 0 };
            for (auto it = ir_graph.children(c).iter(); it; ++it, ++k) {
                lower(*it);
                if (k % 2 == 1)
                    emit(lir::Send { ctx.selectors.put, 2, 0, lir::SendKind::Normal, lir::SendForm::Implicit,
                                     lir::Tail::None },
                         i);
            }
            return;
        }
        emit(lir::PushInteger { static_cast<std::int32_t>(payload.size) }, i);
        emit(lir::Send { ctx.selectors.new_, 1, 0, lir::SendKind::Normal, lir::SendForm::Implicit, lir::Tail::None },
             i);
        for (auto it = ir_graph.children(c).iter(); it; ++it) {
            lower(*it);
            emit(lir::Send { ctx.selectors.add, 1, 0, lir::SendKind::Normal, lir::SendForm::Implicit, lir::Tail::None },
                 i);
        }
    }

    // first, second and last, a missing part is nil.
    template <typename Range> void series_operands(Range r) {
        const ir::NodeIndex i { *r };
        const auto [first, second, last] = ir_graph.children(r);
        lower(first);
        for (const ir::NodeIndex part : { second, last }) {
            if (missing(part))
                emit(lir::PushSpecialValue { lir::SpecialValue::Nil }, i);
            else
                lower(part);
        }
    }

    // `(first, second..last)` is `first.prSimpleNumberSeries(second, last)`, as the old grammar did.
    void range(ir::SCRange_I r, lir::Tail tail) {
        series_operands(r);
        emit(lir::Send { ctx.selectors.series, 2, 0, lir::SendKind::Normal, lir::SendForm::Call, tail }, r);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Generators
    ////////////////////////////////////////////////////////////////////////////////

    // Rewritten as the old grammar did, each qualifier nesting the ones after it, and the value innermost, in a
    // function or an inlined `if`.
    struct Generator {
        ir::SCGenQualList_I list;
        util::IndexedVector<QualifierIndex, ir::NodeIndex> qualifiers;
        ir::NodeIndex value;
        bool routine;
    };

    void send(ir::SymSCSelector selector, std::uint8_t num_positional, lir::Tail tail, ir::NodeIndex at) {
        emit(lir::Send { selector, num_positional, 0, lir::SendKind::Normal, lir::SendForm::Call, tail }, at);
    }

    // `{: value, qualifiers }` is `{ qualifiers }.r`, with `value.yield` innermost.
    void routine(ir::SCRoutineGenerator_I g, FrameIndex started, lir::Tail tail) {
        const ir::NodeIndex i { *g };
        const auto [qualifiers, value] = ir_graph.children(g);
        const auto index = begin_function(started, i);
        generator(qualifiers, value, true, lir::Tail::ReturnFromFunction);
        end_function();
        emit(lir::PushFunction { index }, i);
        send(ctx.selectors.r, 0, tail, i);
    }

    // `{; value, qualifiers }` is the qualifiers, with value innermost.
    void generator(ir::SCGenQualList_I list, ir::SCAnyExpr_I value, bool routine, lir::Tail tail) {
        Generator g { list, {}, value, routine };
        for (auto it = ir_graph.children(list).iter(); it; ++it)
            (void)g.qualifiers.push_back(*it);
        (void)cursor.enter(list);
        nest(g, QualifierIndex { 0 }, tail);
    }

    // Qualifier k nesting the rest.
    void nest(const Generator& g, QualifierIndex k, lir::Tail tail) {
        if (!g.qualifiers.contains(k)) {
            (void)cursor.exit(g.list);
            if (!g.routine) {
                lower(g.value, tail);
                return;
            }
            lower(g.value);
            send(ctx.selectors.yield, 0, tail, g.value);
            return;
        }

        const auto q = g.qualifiers[k];
        (void)cursor.enter(q);
        if (const auto iterate = ir_graph.as_a<ir::SCGenQualIterate_I>(q)) {
            // `x <- source` is `source.do { |x| ... }`, `x <- (first, second..last)` is
            // `first.forSeries(second, last) { |x| ... }`, so the range isn't made.
            const auto [source] = ir_graph.children(*iterate);
            const ir::NodeIndex source_i { *source };
            const auto range = ir_graph.as_a<ir::SCRange_I>(source_i);
            if (range) {
                (void)cursor.enter(source_i);
                series_operands(*range);
                (void)cursor.exit(source_i);
            } else {
                lower(source_i);
            }
            qualifier_function(g, k);
            if (range)
                send(ctx.selectors.for_series, 3, tail, q);
            else
                send(ctx.selectors.do_, 1, tail, q);
        } else if (const auto declare = ir_graph.as_a<ir::SCGenQualDeclare_I>(q)) {
            // `var x = value` is `{ |x| ... }.value(value)`, the function is pushed first.
            const auto [value] = ir_graph.children(*declare);
            begin_body();
            lower(value);
            const auto taken = take_body();
            qualifier_function(g, k);
            place(taken);
            send(ctx.selectors.value, 1, tail, q);
        } else if (const auto test = ir_graph.as_a<ir::SCGenQualIf_I>(q)) {
            // `if (condition) { ... }`, inlined.
            const auto [expr] = ir_graph.children(*test);
            const auto kind = condition(expr);
            (void)cursor.exit(q);
            begin_body();
            nest(g, util::advance(k), tail);
            emit_branch(kind, end_body(), {}, q);
        } else if (const auto effect = ir_graph.as_a<ir::SCGenQualSideEffect_I>(q)) {
            const auto [expr] = ir_graph.children(*effect);
            lower(expr);
            emit(lir::Drop {}, q);
            (void)cursor.exit(q);
            nest(g, util::advance(k), tail);
        } else if (const auto guard = ir_graph.as_a<ir::SCGenQualWhile_I>(q)) {
            // `if (condition) { ... } { nil.alwaysYield }`, inlined.
            const auto [expr] = ir_graph.children(*guard);
            const auto kind = condition(expr);
            (void)cursor.exit(q);
            begin_body();
            nest(g, util::advance(k), tail);
            const auto then_body = end_body();
            begin_body();
            emit(lir::PushSpecialValue { lir::SpecialValue::Nil }, q);
            send(ctx.selectors.always_yield, 0, tail, q);
            emit_branch(kind, then_body, end_body(), q);
        } else {
            assert(false && "unhandled generator qualifier");
        }
    }

    // The qualifier's frame starts as it exits, its function's arguments are what it declares.
    void qualifier_function(const Generator& g, QualifierIndex k) {
        const auto q = g.qualifiers[k];
        const auto started = cursor.exit(q);
        assert(started);
        const auto index = begin_function(*started, q);
        nest(g, util::advance(k), lir::Tail::ReturnFromFunction);
        end_function();
        emit(lir::PushFunction { index }, q);
    }

    // `#a, b ...c = value` keeps value on the stack, and assigns each target value.at(index), or value.copyToEnd(index)
    // for the rest.
    void pack(ir::NodeIndex value, ir::SCPackTargetList_I targets, ir::NodeIndex at) {
        lower(value);
        for (auto it = ir_graph.children(targets).iter(); it; ++it) {
            const ir::NodeIndex target_i { **it };
            const auto& target = ir_graph.payload(*it);
            emit(lir::Dup {}, target_i);
            emit(lir::PushInteger { static_cast<std::int32_t>(target.index) }, target_i);
            emit(lir::Send { target.rest ? ctx.selectors.copy_to_end : ctx.selectors.at, 1, 0, lir::SendKind::Normal,
                             lir::SendForm::Implicit, lir::Tail::None },
                 target_i);
            if (target.local)
                emit(lir::StoreLocal { local_ref(ir::LocalIndex { *target.local }) }, target_i);
            else
                store_member(ir::MemberIndex { *target.member }, target_i);
            emit(lir::Drop {}, target_i);
        }
        (void)at;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Methods and functions
    ////////////////////////////////////////////////////////////////////////////////

    // A literal default is in the frame's prototype, any other runs when the argument is nil.
    template <typename Function> void contents(Function f, bool is_method) {
        const auto [positional, variadic, body_i] = ir_graph.children(f);
        for (auto it = ir_graph.children(positional).iter(); it; ++it) {
            const auto& argument = ir_graph.payload(*it);
            const auto [value] = ir_graph.children(*it);
            const ir::NodeIndex value_i { *value };
            if (argument.prototype || missing(value_i)) {
                cursor.skip_over_children(value_i);
                continue;
            }
            begin_body();
            lower(value_i);
            const auto default_value = end_body();
            emit(lir::DefaultArgument { own_slot(argument.local), default_value }, *it);
        }
        for (auto it = ir_graph.children(variadic).iter(); it; ++it)
            cursor.skip_over_children(*it);
        body(body_i, is_method);
    }

    // A literal initial value is in the frame's prototype, unless hoisted from an inlined function, which sets each
    // variable, nil without an initial value.
    void declare(ir::SCVariableDeclare_I d, bool hoisted) {
        const auto& declaration = ir_graph.payload(d);
        const auto [value] = ir_graph.children(d);
        const ir::NodeIndex value_i { *value };
        const ir::NodeIndex at { *d };
        if (missing(value_i)) {
            cursor.skip_over_children(value_i);
            if (!hoisted)
                return;
            emit(lir::PushSpecialValue { lir::SpecialValue::Nil }, at);
        } else if (declaration.prototype && !hoisted) {
            cursor.skip_over_children(value_i);
            return;
        } else {
            lower(value_i);
        }
        emit(lir::StoreLocal { local_ref(declaration.local) }, at);
        emit(lir::Drop {}, at);
    }

    // Returns whether the item was a declaration.
    bool declaration(ir::NodeIndex item, bool hoisted) {
        if (const auto d = ir_graph.as_a<ir::SCVariableDeclare_I>(item)) {
            declare(*d, hoisted);
            return true;
        }
        if (const auto p = ir_graph.as_a<ir::SCPackDeclare_I>(item)) {
            const auto [value, targets] = ir_graph.children(*p);
            pack(value, targets, item);
            emit(lir::Drop {}, item);
            return true;
        }
        return false;
    }

    // A method returns this unless it ends with `^`, as the old grammar added `^this`.
    void body(ir::SCBody_I b, bool is_method) {
        const ir::NodeIndex body_i { *b };
        const auto last_item = ir_graph.last_child(body_i);
        bool has_value { false };
        bool returns { false };
        for (auto it = ir_graph.children(b).iter(); it; ++it) {
            const ir::NodeIndex item { **it };
            const bool last = *item == *last_item;
            has_value = false;
            if (declaration(item, false))
                continue;
            if (ir_graph.is_a<ir::SCExplicitReturn_I>(item)) {
                lower(item);
                returns = last;
                has_value = last;
            } else if (last && !is_method) {
                lower(item, lir::Tail::ReturnFromFunction);
                has_value = true;
            } else {
                lower(item);
                emit(lir::Drop {}, item);
            }
        }
        if (is_method) {
            if (!returns)
                emit(lir::ReturnSelf {}, body_i);
        } else if (!has_value) {
            emit(lir::PushSpecialValue { lir::SpecialValue::Nil }, body_i);
        }
    }

    // A root's functions are kept when it fails, for debugging, but it isn't complete.
    template <typename Function> void root(Function f, bool is_method) {
        const ir::NodeIndex i { *f };
        const auto started = cursor.enter(i);
        failed = false;
        const auto index = begin_function(*started, i);
        contents(f, is_method);
        end_function();
        (void)cursor.exit(i);
        out.functions[index].complete = !failed;
    }

    void lower_class(ir::SCClass_I c) {
        const auto [members, methods] = ir_graph.children(c);
        for (auto it = ir_graph.children(methods).iter(); it; ++it) {
            const auto m = *it;
            if (ir_graph.payload(m).poisoned) {
                cursor.skip_over_children(m);
                continue;
            }
            fold_constants = ir::can_fold_constants(ir_graph.payload(m).version);
            root(m, true);
        }
    }

    void lower_interpreted(ir::SCInterpreted_I in) {
        if (ir_graph.payload(in).poisoned)
            return;
        fold_constants = ir::can_fold_constants(ir_graph.payload(in).version);
        // Only sets the cursor's version.
        (void)cursor.enter(in);
        const auto [code] = ir_graph.children(in);
        if (const auto f = ir_graph.as_a<ir::SCFunction_I>(code)) {
            root(*f, false);
        } else {
            for (auto it = ir_graph.children(ir_graph.assume_a<ir::SCRegionList_I>(code)).iter(); it; ++it) {
                const auto [f] = ir_graph.children(*it);
                root(f, false);
            }
        }
        (void)cursor.exit(in);
    }
};

////////////////////////////////////////////////////////////////////////////////
// Dump
////////////////////////////////////////////////////////////////////////////////

const char* special_name(ir::SCSpecial::Kind k) {
    switch (k) {
    case ir::SCSpecial::Kind::This:
        return "this";
    case ir::SCSpecial::Kind::Super:
        return "super";
    case ir::SCSpecial::Kind::Process:
        return "thisProcess";
    case ir::SCSpecial::Kind::Thread:
        return "thisThread";
    case ir::SCSpecial::Kind::Method:
        return "thisMethod";
    case ir::SCSpecial::Kind::FunctionDef:
        return "thisFunctionDef";
    case ir::SCSpecial::Kind::Function:
        return "thisFunction";
    }
    return "?";
}

}

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

lir::LoweredGraph lower(const LoweringContext& ctx, ir::join<ir::SCClass_I, ir::SCInterpreted_I> root) {
    Lowerer lowerer { ctx, ctx.graph, FrameCursor { ctx.graph, ctx.inlining, ctx.frames } };
    if (const auto c = ctx.graph.as_a<ir::SCClass_I>(root))
        lowerer.lower_class(*c);
    else
        lowerer.lower_interpreted(ctx.graph.assume_a<ir::SCInterpreted_I>(root));
    assert(lowerer.building.empty());
    return std::move(lowerer.out);
}

void write_instruction(std::ostream& out, const lir::Function& function, const lir::Instruction& instruction,
                       const ir::IRGraph& graph, const ir::SymbolTable& symbols, const ir::ConstantTable& constants) {
    const auto local = [&](lir::LocalRef l) { out << " depth " << l.depth << ", slot " << unsigned { *l.slot }; };
    const auto branches = [&](const char* name, lir::BodyIndex then_body, lir::OptionalBodyIndex else_body) {
        out << name << " then body " << *then_body;
        if (else_body)
            out << ", else body " << *else_body;
    };
    std::visit(util::overload {
                   [&](const lir::PushLocal& p) {
                       out << "PushLocal";
                       local(p.local);
                   },
                   [&](const lir::PushInstance& p) { out << "PushInstance slot " << *p.slot; },
                   [&](const lir::PushClassVariable& p) { out << "PushClassVariable " << *p.index; },
                   [&](const lir::PushConstant& p) {
                       out << "PushConstant ";
                       constants(p.value).to_literal(out, symbols, constants);
                   },
                   [&](const lir::PushClassConstant& p) {
                       out << "PushClassConstant " << symbols(graph.member(p.member).name) << " = ";
                       constants(p.value).to_literal(out, symbols, constants);
                   },
                   [&](const lir::PushSpecialValue& p) {
                       out << "PushSpecialValue "
                           << (p.value == lir::SpecialValue::Nil        ? "nil"
                                   : p.value == lir::SpecialValue::True ? "true"
                                                                        : "false");
                   },
                   [&](const lir::PushInteger& p) { out << "PushInteger " << p.value; },
                   [&](const lir::PushSymbol& p) { out << "PushSymbol \\" << symbols(p.value); },
                   [&](const lir::PushClass& p) { out << "PushClass " << symbols(p.name); },
                   [&](const lir::PushSpecial& p) { out << "PushSpecial " << special_name(p.kind); },
                   [&](const lir::PushFunction& p) { out << "PushFunction " << *p.function; },
                   [&](const lir::StoreLocal& s) {
                       out << "StoreLocal";
                       local(s.local);
                   },
                   [&](const lir::StoreInstance& s) { out << "StoreInstance slot " << *s.slot; },
                   [&](const lir::StoreClassVariable& s) { out << "StoreClassVariable " << *s.index; },
                   [&](const lir::Dup&) { out << "Dup"; },
                   [&](const lir::Drop&) { out << "Drop"; },
                   [&](const lir::Send& s) {
                       out << "Send " << symbols(s.selector) << " (positional " << unsigned { s.num_positional };
                       if (s.num_keyword)
                           out << ", keyword " << unsigned { s.num_keyword };
                       if (s.kind == lir::SendKind::Super)
                           out << ", super";
                       else if (s.kind == lir::SendKind::WithAdverb)
                           out << ", adverb";
                       if (s.form == lir::SendForm::Binary)
                           out << ", binary";
                       else if (s.form == lir::SendForm::Setter)
                           out << ", setter";
                       else if (s.form == lir::SendForm::Implicit)
                           out << ", implicit";
                       if (s.tail == lir::Tail::ReturnFromFunction)
                           out << ", tail from function";
                       else if (s.tail == lir::Tail::ReturnFromMethod)
                           out << ", tail from method";
                       out << ")";
                   },
                   [&](const lir::Return&) { out << "Return"; },
                   [&](const lir::ReturnSelf&) { out << "ReturnSelf"; },
                   [&](const lir::NonLocalReturn&) { out << "NonLocalReturn"; },
                   [&](const lir::DefaultArgument& d) {
                       out << "DefaultArgument slot " << unsigned { *d.slot } << ", body " << *d.value;
                   },
                   [&](const lir::If& b) { branches("If", b.then_body, b.else_body); },
                   [&](const lir::IfNil& b) { branches("IfNil", b.then_body, b.else_body); },
                   [&](const lir::IfNotNil& b) { branches("IfNotNil", b.then_body, b.else_body); },
                   [&](const lir::And& b) { out << "And body " << *b.rhs; },
                   [&](const lir::Or& b) { out << "Or body " << *b.rhs; },
                   [&](const lir::OrIfNil& b) { out << "OrIfNil body " << *b.rhs; },
                   [&](const lir::AndIfNotNil& b) { out << "AndIfNotNil body " << *b.rhs; },
                   [&](const lir::QuestionMark&) { out << "QuestionMark"; },
                   [&](const lir::While& w) {
                       out << "While condition body " << *w.condition;
                       if (w.body)
                           out << ", body " << *w.body;
                   },
                   [&](const lir::Loop& l) { out << "Loop body " << *l.body; },
                   [&](const lir::Switch& s) {
                       out << "Switch";
                       const char* separator = " ";
                       for (const auto c : lir::cases(s)) {
                           const auto& sc = function.switch_cases[c];
                           out << separator;
                           if (sc.key)
                               constants(ir::ConstExprIndex { *sc.key }).to_literal(out, symbols, constants);
                           else
                               out << "default";
                           out << ": body " << *sc.body;
                           separator = ", ";
                       }
                   },
                   [&](const lir::Unimplemented& u) {
                       out << "Unimplemented " << ir::IRGraph::node_name(graph.payload(u.node));
                   },
               },
               instruction);
}
}
