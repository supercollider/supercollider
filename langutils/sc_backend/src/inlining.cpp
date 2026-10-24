#include "sc_backend/inlining.hpp"
#include "sc_bytecode/SpecialNames.h"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_sema/versions.hpp"

#include <vector>

// This file is AI made with little human supervision.
// This is because the old behaviour is a mess, as long as it is byte identical (which can only be achieved through
// trial and error) who cares what it looks like. When compiling code for 3.16 and beyond, it is worth putting more
// thought into the code.

namespace sc::backend {

namespace {

// The old compiler sent anything with keyword arguments without looking at its selector. A variadic argument is a
// performList, and an adverb isn't inlined.
bool only_positional(const ir::IRGraph& g, ir::SCMessage_I m) {
    const auto [receiver, p, keyword, variadic, adverb] = g.children(m);
    const auto empty = [&](ir::NodeIndex i) { return g.is_a<ir::SCMissing_I>(i) || !g.first_child(i); };
    if (const auto special = g.as_a<ir::SCSpecial_I>(ir::NodeIndex { *receiver });
        special && g.payload(*special).kind == ir::SCSpecial::Kind::Super)
        return false;
    return empty(ir::NodeIndex { *keyword }) && empty(ir::NodeIndex { *variadic })
        && g.is_a<ir::SCMissing_I>(ir::NodeIndex { *adverb });
}

bool is_true(const InliningContext& ctx, const ir::IRGraph& g, ir::SCAnyExpr_I i) {
    const auto literal = g.as_a<ir::SCLiteral_I>(ir::NodeIndex { *i });
    if (!literal)
        return false;
    const auto* b = ctx.constants(g.payload(*literal).index).get_if<bool>();
    return b && *b;
}

bool all_inlineable(const ir::IRGraph& g, const std::vector<ir::SCAnyExpr_I>& functions, ast::LanguageVersion version) {
    for (const auto f : functions)
        if (!is_inlineable(g, f, version))
            return false;
    return true;
}

// Keys and functions, then an optional default function.
bool can_inline_switch(const InliningContext& ctx, const ir::IRGraph& g, const std::vector<ir::SCAnyExpr_I>& args,
                       ast::LanguageVersion version) {
    // The old compiler reported fewer as an error.
    if (args.size() < 2)
        return false;
    std::size_t k { 0 };
    for (; k + 1 < args.size(); k += 2)
        if (!switch_key(ctx, g, args[k], version) || !is_inlineable(g, args[k + 1], version))
            return false;
    return k == args.size() || is_inlineable(g, args[k], version);
}
}

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

std::optional<Inlined> inlining(const InliningContext& ctx, const ir::IRGraph& g, ir::SCMessage_I m,
                                ast::LanguageVersion version) {
    const auto& payload = g.payload(m);
    if (payload.form == ir::SCMessage::Form::Setter)
        return std::nullopt;
    const auto special = ctx.symbols.find(ir::Symbol { *payload.selector });
    const auto* selector = special ? std::get_if<OpSpecialSelectors>(&*special) : nullptr;
    if (!selector || !only_positional(g, m))
        return std::nullopt;
    const auto args = positional_arguments(g, m);
    const auto [receiver, p, k, v, a] = g.children(m);
    // The old compiler sent a call with more than 15 arguments without looking at its selector, unless switch or case.
    const bool switch_or_case = *selector == OpSpecialSelectors::Switch || *selector == OpSpecialSelectors::Case;
    if (payload.form == ir::SCMessage::Form::Call && args.size() + 1 > 15 && !switch_or_case)
        return std::nullopt;

    switch (*selector) {
    case OpSpecialSelectors::If:
        if ((args.size() != 1 && args.size() != 2) || !all_inlineable(g, args, version))
            return std::nullopt;
        if (const auto test = nil_test(ctx, g, receiver))
            return test->kind();
        return Inlined::If;
    case OpSpecialSelectors::And:
        return args.size() == 1 && is_inlineable(g, args[0], version) ? std::optional { Inlined::And } : std::nullopt;
    case OpSpecialSelectors::Or:
        return args.size() == 1 && is_inlineable(g, args[0], version) ? std::optional { Inlined::Or } : std::nullopt;
    case OpSpecialSelectors::DoubleQuestionMark:
        return args.size() == 1 && is_inlineable(g, args[0], version) ? std::optional { Inlined::OrIfNil }
                                                                      : std::nullopt;
    case OpSpecialSelectors::ExclamationQuestionMark:
        return args.size() == 1 && is_inlineable(g, args[0], version) ? std::optional { Inlined::AndIfNotNil }
                                                                      : std::nullopt;
    case OpSpecialSelectors::QuestionMark:
        return args.size() == 1 ? std::optional { Inlined::QuestionMark } : std::nullopt;
    case OpSpecialSelectors::While:
        if (args.empty())
            return is_inlineable(g, receiver, version) ? std::optional { Inlined::While } : std::nullopt;
        if (args.size() != 1 || !is_inlineable(g, args[0], version))
            return std::nullopt;
        // Only a literal `true`, the old compiler's test for `{ true }` never matched.
        if (is_true(ctx, g, receiver))
            return Inlined::WhileTrue;
        return is_inlineable(g, receiver, version) ? std::optional { Inlined::While } : std::nullopt;
    case OpSpecialSelectors::Loop:
        return args.empty() && is_inlineable(g, receiver, version) ? std::optional { Inlined::Loop } : std::nullopt;
    case OpSpecialSelectors::Case:
        return is_inlineable(g, receiver, version) && all_inlineable(g, args, version) ? std::optional { Inlined::Case }
                                                                                       : std::nullopt;
    case OpSpecialSelectors::Switch:
        return can_inline_switch(ctx, g, args, version) ? std::optional { Inlined::Switch } : std::nullopt;
    default:
        return std::nullopt;
    }
}

bool inlines_receiver(Inlined kind) { return kind == Inlined::While || kind == Inlined::Loop || kind == Inlined::Case; }

////////////////////////////////////////////////////////////////////////////////
// Inlined sends
////////////////////////////////////////////////////////////////////////////////

bool is_inlineable(const ir::IRGraph& g, ir::SCAnyExpr_I i, ast::LanguageVersion version) {
    const auto f = g.as_a<ir::SCFunction_I>(ir::NodeIndex { *i });
    // A closed function's names are looked up in the Interpreter, which an inlined body can't do.
    if (!f || g.payload(*f).closed)
        return false;
    const auto [positional, variadic, body] = g.children(*f);
    if (g.children(positional).iter() || g.children(variadic).iter())
        return false;
    const bool hoists = ir::hoists_inlined_variables(version);
    for (auto it = g.children(body).iter(); it; ++it) {
        const ir::NodeIndex item { **it };
        if (const auto declare = g.as_a<ir::SCVariableDeclare_I>(item)) {
            if (!hoists || g.local(g.payload(*declare).local).captured)
                return false;
        } else if (const auto pack = g.as_a<ir::SCPackDeclare_I>(item)) {
            if (!hoists)
                return false;
            const auto [_, targets] = g.children(*pack);
            for (auto t = g.children(targets).iter(); t; ++t)
                if (const auto& target = g.payload(*t);
                    target.declares && target.local && g.local(ir::LocalIndex { *target.local }).captured)
                    return false;
        }
    }
    return true;
}

std::vector<ir::SCAnyExpr_I> positional_arguments(const ir::IRGraph& g, ir::SCMessage_I m) {
    std::vector<ir::SCAnyExpr_I> out;
    const auto [r, positional, k, v, a] = g.children(m);
    if (const auto list = g.as_a<ir::SCPosArgList_I>(ir::NodeIndex { *positional }))
        for (auto it = g.children(*list).iter(); it; ++it)
            out.push_back(*it);
    return out;
}

////////////////////////////////////////////////////////////////////////////////
// Nil tests
////////////////////////////////////////////////////////////////////////////////

std::optional<NilTest> nil_test(const InliningContext& ctx, const ir::IRGraph& g, ir::SCAnyExpr_I i) {
    const auto m = g.as_a<ir::SCMessage_I>(ir::NodeIndex { *i });
    if (!m || g.payload(*m).form != ir::SCMessage::Form::Call || !only_positional(g, *m)
        || !positional_arguments(g, *m).empty())
        return std::nullopt;
    const auto special = ctx.symbols.find(ir::Symbol { *g.payload(*m).selector });
    const auto* unary = special ? std::get_if<OpUnaryMath>(&*special) : nullptr;
    if (unary && *unary == OpUnaryMath::IsNil)
        return NilTest { true, *m };
    if (unary && *unary == OpUnaryMath::NotNil)
        return NilTest { false, *m };
    return std::nullopt;
}

std::optional<NilTest> nil_test_body(const InliningContext& ctx, const ir::IRGraph& g, ir::SCFunction_I f) {
    const auto [p, v, body] = g.children(f);
    const auto it = g.children(body).iter();
    if (!it)
        return std::nullopt;
    const ir::NodeIndex item { **it };
    if (*g.last_child(ir::NodeIndex { *body }) != *item)
        return std::nullopt;
    if (const auto m = g.as_a<ir::SCMessage_I>(item))
        return nil_test(ctx, g, *m);
    return std::nullopt;
}

////////////////////////////////////////////////////////////////////////////////
// Switch keys
////////////////////////////////////////////////////////////////////////////////

std::optional<ir::ConstExprIndex> switch_key(const InliningContext& ctx, const ir::IRGraph& g, ir::SCAnyExpr_I i,
                                             ast::LanguageVersion version) {
    ir::NodeIndex key_node { *i };
    if (const auto f = g.as_a<ir::SCFunction_I>(key_node)) {
        if (!is_inlineable(g, i, version))
            return std::nullopt;
        const auto [p, v, body] = g.children(*f);
        const auto first = g.first_child(ir::NodeIndex { *body });
        if (!first || *g.last_child(ir::NodeIndex { *body }) != *first)
            return std::nullopt;
        key_node = ir::NodeIndex { *first };
    }
    const auto literal = g.as_a<ir::SCLiteral_I>(key_node);
    if (!literal)
        return std::nullopt;
    const auto key = g.payload(*literal).index;
    const auto& value = ctx.constants(key);
    if (value.get_if<std::int32_t>() || value.get_if<double>() || value.get_if<char>() || value.get_if<bool>()
        || value.get_if<ir::SymSCSymbol>())
        return key;
    return std::nullopt;
}

////////////////////////////////////////////////////////////////////////////////
// Special symbols
////////////////////////////////////////////////////////////////////////////////

SpecialSymbols::SpecialSymbols(ir::SymbolTable& symbols): series(symbols.get_selector("prSimpleNumberSeries")) {
    // The first table a name is in wins, as the old compiler looked in them in this order.
    const auto add = [&](const auto& names, auto table) {
        using Enum = decltype(table);
        for (std::size_t i { 0 }; i < names.size(); ++i)
            special_selectors.insert({ symbols.get(names[i]), SpecialSelector { static_cast<Enum>(i) } });
    };
    add(Opcode::names::special_selectors, OpSpecialSelectors {});
    add(Opcode::names::unary_math, OpUnaryMath {});
    add(Opcode::names::binary_math, OpBinaryMath {});
    for (std::size_t i { 0 }; i < Opcode::names::special_classes.size(); ++i)
        special_classes.insert({ symbols.get(Opcode::names::special_classes[i]), static_cast<std::uint8_t>(i) });
}

std::optional<SpecialSymbols::SpecialSelector> SpecialSymbols::find(ir::Symbol selector) const {
    if (const auto fnd = special_selectors.find(selector); fnd != special_selectors.end())
        return fnd->second;
    return std::nullopt;
}

}
