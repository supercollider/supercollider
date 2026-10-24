#include "sc_backend/lower.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_sema/versions.hpp"
#include "sc_util/overload.hpp"

#include <cassert>
#include <limits>

namespace sc::backend {

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
    super_perform_list(symbols.get_selector("superPerformList")) {}

namespace {

// A sub-region's instructions while it is being lowered. Sub-regions nest, e.g., a default argument's value inside a
// body, so each is collected on its own and appended to its function when it ends, keeping every sub-region contiguous.
struct SubRegionBuild {
    std::vector<lir::Instruction> instructions {};
    std::vector<lir::SourceSpan> spans {};
};

struct Lowerer {
    const LoweringContext& ctx;
    const ir::IRGraph& ir_graph;
    // Walks the graph in the same order as layout_frames, so its frames are found again.
    FrameCursor cursor;
    lir::LoweredGraph out {};

    // The functions being lowered, innermost last, each with the sub-regions it is building, innermost last.
    struct Building {
        lir::FunctionIndex function;
        bool is_method;
        std::vector<SubRegionBuild> regions {};
    };
    std::vector<Building> building {};
    // Set when the method or interpreted function being lowered can't be emitted, e.g., a frame over the VM's limit.
    bool failed { false };
    // Whether the code's version folds constants. The old compiler didn't, so legacy code only uses literals' values,
    // e.g., a read of a variable sema knows is 1 still reads the variable.
    bool fold_constants { false };

    ////////////////////////////////////////////////////////////////////////////////
    // Functions and sub-regions
    ////////////////////////////////////////////////////////////////////////////////

    lir::Function& function() { return out.functions[building.back().function]; }

    // Gets the location data and pushes the instruction.
    void emit(lir::Instruction instruction, ir::NodeIndex at) {
        auto& region = building.back().regions.back();
        const auto range = ir::IRGraph::range(ir_graph.location(at));
        region.instructions.push_back(instruction);
        region.spans.push_back({ range.begin.absolute, range.end.absolute });
    }

    void begin_sub_region() { building.back().regions.emplace_back(); }

    // Appends the innermost sub-region to its function, returning where it is.
    lir::FunctionSubRegion append_sub_region() {
        auto region = std::move(building.back().regions.back());
        building.back().regions.pop_back();
        auto& f = function();
        const lir::FunctionSubRegion placed { lir::InstructionIndex {
                                                  static_cast<std::uint32_t>(f.instructions.size()) },
                                              static_cast<std::uint32_t>(region.instructions.size()) };
        f.instructions.raw().insert(f.instructions.raw().end(), region.instructions.begin(), region.instructions.end());
        f.spans.raw().insert(f.spans.raw().end(), region.spans.begin(), region.spans.end());
        return placed;
    }

    lir::FunctionSubRegionIndex end_sub_region() {
        const auto placed = append_sub_region();
        return function().sub_regions.push_back(placed);
    }

    // The body is sub-region 0, so it is reserved first, though it ends last.
    lir::FunctionIndex begin_function(FrameIndex frame, ir::NodeIndex owner) {
        lir::Function f { frame, ir_graph.location(owner).source };
        f.sub_regions.push_back({ lir::InstructionIndex { 0 }, 0 });
        const auto index = out.functions.push_back(std::move(f));
        building.push_back({ index, ir_graph.is_a<ir::SCMethod_I>(owner) });
        begin_sub_region();
        return index;
    }

    void end_function() {
        assert(building.back().regions.size() == 1);
        const auto sub = append_sub_region();
        function().sub_regions[lir::body_sub_region] = sub;
        building.pop_back();
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Operands
    ////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] ir::SymSCSymbol symbol(ir::Symbol name) const { return ctx.symbols.get_symbol(ctx.symbols(name)); }

    // A local's slot in the frame of the function being lowered.
    [[nodiscard]] lir::SlotIndex own_slot(ir::LocalIndex l) { return addressable(ctx.frames.locals[l].slot); }

    // The VM addresses a slot with a byte.
    [[nodiscard]] lir::SlotIndex addressable(FrameSlotIndex slot) {
        if (*slot > std::numeric_limits<std::uint8_t>::max()) {
            failed = true; // Reported by layout_frames.
            return lir::SlotIndex { 0 };
        }
        return lir::SlotIndex { static_cast<std::uint8_t>(*slot) };
    }

    [[nodiscard]] lir::LocalRef local_ref(ir::LocalIndex l) {
        const auto& slot = ctx.frames.locals[l];
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

    // Literals are not copied.
    // Constants, which aren't literals, are copied.
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

        //(1; 2; 3; 4) here we push 4.
        if (const auto list = ir_graph.as_a<ir::SCExprList_I>(i)) {
            if (const auto last = ir_graph.last_child(ir::NodeIndex { *list })) {
                // Push the constant, but write its location as the final.
                push_constant(ir::NodeIndex { *last }, c);
                return;
            }
        }
        push_copy(c, i);
    }

    // Copies the element
    void push_copy(ir::ConstExprIndex c, ir::NodeIndex at) {
        emit(lir::PushConstant { c }, at);
        // Here is the copy.
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

        // Make mutable elements copy themselves.
        const auto& value = ctx.constants(c);
        if (const auto* array = value.get_if<ir::ConstExprArray>()) {
            for (std::size_t k { 0 }; k < array->size(); ++k)
                if (is_mutable((*array)[k]))
                    place_element([&] { emit(lir::PushInteger { static_cast<std::int32_t>(k) }, at); }, (*array)[k]);
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

    [[nodiscard]] bool has_environment_target(ir::SCPackTargetList_I targets) const {
        for (auto it = ir_graph.children(targets).iter(); it; ++it)
            if (ir_graph.payload(*it).kind == ir::SCAssign::Kind::Environment)
                return true;
        return false;
    }

    [[nodiscard]] bool lowered_yet(ir::NodeIndex i) const {
        if (ir_graph.is_a<ir::SCRoutineGenerator_I>(i) || ir_graph.is_a<ir::SCImmediateGenerator_I>(i)
            || ir_graph.is_a<ir::SCRangeGenerator_I>(i))
            return false;
        if (const auto pack = ir_graph.as_a<ir::SCPackAssign_I>(i)) {
            const auto [_, targets] = ir_graph.children(*pack);
            return !has_environment_target(targets);
        }
        if (const auto message = ir_graph.as_a<ir::SCMessage_I>(i)) {
            // performList takes one array, the old grammar only allowed one.
            const auto [r, p, k, variadic, a] = ir_graph.children(*message);
            if (num_variadic_args(ir::NodeIndex { *variadic }) > 1)
                return false;
        }
        return true;
    }

    // tail is whether the expression's value is what its function returns
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
                       [&](const ir::SCAssign&) { assign(*ir_graph.as_a<ir::SCAssign_I>(i), tail); },
                       [&](const ir::SCSpecial& s) {
                           // `super` is `this`, only the send it receives differs.
                           const auto kind = s.kind == ir::SCSpecial::Kind::Super ? ir::SCSpecial::Kind::This : s.kind;
                           emit(lir::PushSpecial { kind }, i);
                       },
                       [&](const ir::SCMessage& m) { message(*ir_graph.as_a<ir::SCMessage_I>(i), m, tail); },
                       [&](const ir::SCExprList&) { expression_list(*ir_graph.as_a<ir::SCExprList_I>(i), tail); },
                       [&](const ir::SCCollection& c) { collection(*ir_graph.as_a<ir::SCCollection_I>(i), c); },
                       [&](const ir::SCRange&) { range(*ir_graph.as_a<ir::SCRange_I>(i), tail); },
                       [&](const ir::SCPackAssign&) {
                           const auto [value, targets] = ir_graph.children(*ir_graph.as_a<ir::SCPackAssign_I>(i));
                           pack(ir::NodeIndex { *value }, targets, i);
                       },
                       [&](const ir::SCExplicitReturn&) {
                           const auto [value] = ir_graph.children(*ir_graph.as_a<ir::SCExplicitReturn_I>(i));
                           lower(ir::NodeIndex { *value }, lir::Tail::ReturnFromMethod);
                           if (building.back().is_method)
                               emit(lir::Return {}, i);
                           else
                               emit(lir::NonLocalReturn {}, i);
                       },
                       [&](const ir::SCFunction&) {
                           const auto f = *ir_graph.as_a<ir::SCFunction_I>(i);
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
            lower(ir::NodeIndex { *value });
            emit(lir::Send { ctx.selectors.envir_put, 1, 0, lir::SendKind::Normal, lir::SendForm::Call, tail }, i);
            return;
        }
        lower(ir::NodeIndex { *value });
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
        const ir::NodeIndex i { *m };
        const auto [receiver, positional, keyword, variadic, adverb] = ir_graph.children(m);
        const ir::NodeIndex receiver_i { *receiver };
        const auto special = ir_graph.as_a<ir::SCSpecial_I>(receiver_i);
        const bool super = special && ir_graph.payload(*special).kind == ir::SCSpecial::Kind::Super;
        const bool has_variadic = num_variadic_args(ir::NodeIndex { *variadic }) > 0;
        const bool has_adverb = !missing(ir::NodeIndex { *adverb });

        lower(receiver_i);
        std::uint32_t num_positional { 0 };
        if (has_variadic) {
            emit(lir::PushSymbol { symbol(*payload.selector) }, i);
            ++num_positional;
        }
        if (const auto list = ir_graph.as_a<ir::SCPosArgList_I>(ir::NodeIndex { *positional }))
            for (auto it = ir_graph.children(*list).iter(); it; ++it, ++num_positional)
                lower(ir::NodeIndex { **it });
        if (const auto list = ir_graph.as_a<ir::SCVariadicArgList_I>(ir::NodeIndex { *variadic }))
            for (auto it = ir_graph.children(*list).iter(); it; ++it, ++num_positional)
                lower(ir::NodeIndex { **it });
        // An adverb is passed as the last argument, `a foo.x: b` is `a.foo(b, \x)`, as the old compiler did.
        if (has_adverb) {
            lower(ir::NodeIndex { *adverb });
            ++num_positional;
        }
        std::uint32_t num_keyword { 0 };
        if (const auto list = ir_graph.as_a<ir::SCKwArgList_I>(ir::NodeIndex { *keyword }))
            for (auto it = ir_graph.children(*list).iter(); it; ++it, ++num_keyword) {
                const auto [value] = ir_graph.children(*it);
                emit(lir::PushSymbol { symbol(*ir_graph.payload(*it).name) }, ir::NodeIndex { **it });
                lower(ir::NodeIndex { *value });
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

    // Each expression's value is dropped but the last's.
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
                lower(ir::NodeIndex { **it });
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
            lower(ir::NodeIndex { **it });
            emit(lir::Send { ctx.selectors.add, 1, 0, lir::SendKind::Normal, lir::SendForm::Implicit, lir::Tail::None },
                 i);
        }
    }

    // `(first, second..last)` is `first.prSimpleNumberSeries(second, last)`, a missing part is nil, as the old grammar
    // did.
    void range(ir::SCRange_I r, lir::Tail tail) {
        const ir::NodeIndex i { *r };
        const auto [first, second, last] = ir_graph.children(r);
        lower(ir::NodeIndex { *first });
        for (const ir::NodeIndex part : { ir::NodeIndex { *second }, ir::NodeIndex { *last } }) {
            if (missing(part))
                emit(lir::PushSpecialValue { lir::SpecialValue::Nil }, i);
            else
                lower(part);
        }
        emit(lir::Send { ctx.selectors.series, 2, 0, lir::SendKind::Normal, lir::SendForm::Call, tail }, i);
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

    // A literal default is in the frame's prototype, so needs no code. Any other runs when the argument is nil.
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
            begin_sub_region();
            lower(value_i);
            const auto region = end_sub_region();
            emit(lir::DefaultArgument { own_slot(argument.local), region }, ir::NodeIndex { **it });
        }
        for (auto it = ir_graph.children(variadic).iter(); it; ++it)
            cursor.skip_over_children(ir::NodeIndex { **it });
        body(body_i, is_method);
    }

    // A variable with a literal initial value is in the frame's prototype, so needs no code.
    void declare(ir::SCVariableDeclare_I d) {
        const auto& declaration = ir_graph.payload(d);
        const auto [value] = ir_graph.children(d);
        const ir::NodeIndex value_i { *value };
        if (declaration.prototype || missing(value_i)) {
            cursor.skip_over_children(value_i);
            return;
        }
        lower(value_i);
        emit(lir::StoreLocal { lir::LocalRef { 0, own_slot(declaration.local) } }, ir::NodeIndex { *d });
        emit(lir::Drop {}, ir::NodeIndex { *d });
    }

    // A function returns its last expression's value, or nil. A method drops every value and returns this, unless it
    // ends with `^`, as the old grammar added `^this`.
    void body(ir::SCBody_I b, bool is_method) {
        const ir::NodeIndex body_i { *b };
        const auto last_item = ir_graph.last_child(body_i);
        bool has_value { false };
        bool returns { false };
        for (auto it = ir_graph.children(b).iter(); it; ++it) {
            const ir::NodeIndex item { **it };
            const bool last = *item == *last_item;
            has_value = false;
            if (const auto d = ir_graph.as_a<ir::SCVariableDeclare_I>(item)) {
                declare(*d);
            } else if (const auto p = ir_graph.as_a<ir::SCPackDeclare_I>(item)) {
                const auto [value, targets] = ir_graph.children(*p);
                pack(ir::NodeIndex { *value }, targets, item);
                emit(lir::Drop {}, item);
            } else if (ir_graph.is_a<ir::SCExplicitReturn_I>(item)) {
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

    // A root's functions are only kept when all of it can be emitted.
    template <typename Function> void root(Function f, bool is_method) {
        const ir::NodeIndex i { *f };
        const auto started = cursor.enter(i);
        failed = false;
        const auto index = begin_function(*started, i);
        contents(f, is_method);
        end_function();
        (void)cursor.exit(i);
        if (!failed)
            out.roots.push_back(index);
    }

    void lower_class(ir::SCClass_I c) {
        const auto [members, methods] = ir_graph.children(c);
        for (auto it = ir_graph.children(methods).iter(); it; ++it) {
            const auto m = *it;
            if (ir_graph.payload(m).poisoned) {
                cursor.skip_over_children(ir::NodeIndex { *m });
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
        const auto [code] = ir_graph.children(in);
        if (const auto f = ir_graph.as_a<ir::SCFunction_I>(ir::NodeIndex { *code })) {
            root(*f, false);
            return;
        }
        for (auto it = ir_graph.children(*ir_graph.as_a<ir::SCRegionList_I>(ir::NodeIndex { *code })).iter(); it;
             ++it) {
            const auto [f] = ir_graph.children(*it);
            root(f, false);
        }
    }
};

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

lir::LoweredGraph lower(const LoweringContext& ctx, ir::join<ir::SCClass_I, ir::SCInterpreted_I> root) {
    Lowerer lowerer { ctx, ctx.graph, FrameCursor { ctx.graph, ctx.frames } };
    if (const auto c = ctx.graph.as_a<ir::SCClass_I>(ir::NodeIndex { *root }))
        lowerer.lower_class(*c);
    else
        lowerer.lower_interpreted(*ctx.graph.as_a<ir::SCInterpreted_I>(ir::NodeIndex { *root }));
    assert(lowerer.building.empty());
    return std::move(lowerer.out);
}

void write_instruction(std::ostream& out, const lir::Instruction& instruction, const ir::IRGraph& graph,
                       const ir::SymbolTable& symbols, const ir::ConstantTable& constants) {
    const auto local = [&](lir::LocalRef l) { out << " depth " << l.depth << ", slot " << unsigned { *l.slot }; };
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
                       out << "DefaultArgument slot " << unsigned { *d.slot } << ", region " << *d.value;
                   },
                   [&](const lir::Unimplemented& u) {
                       out << "Unimplemented " << ir::IRGraph::node_name(graph.payload(u.node));
                   },
               },
               instruction);
}

}
