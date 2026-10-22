// Ranges, e.g., `(1..5)` and `(1, 3..9)`.
// These become SCRange rather than a message, so later passes can choose how to lower them.
// Needs the ast graph for the children.
#include "ast2ir/consteval.hpp"
#include "ast2ir/expr.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include <cstdint>
#include <optional>

namespace sc::ir::ast2ir {

namespace {

std::optional<std::int32_t> integer_constant(const Context& ctx, const Return<SCAnyExpr_I>& r) {
    if (!r.const_index)
        return std::nullopt;
    if (const auto i = ctx.constant_table(*r.const_index).get_if<std::int32_t>())
        return *i;
    return std::nullopt;
}

}

// `(first..last)`, `(first, second..last)`, `(..last)` is `(0..last)`.
// Without a last, it is unbounded, which is an error here as it would be created. Contexts that only iterate it, e.g.,
// generators, should compile the range themselves.
Return<SCAnyExpr_I> Expr::operator()(ast::RangeIndex i) {
    const TypeInfo type { ctx.common_classes.array, TypeConfidence::Absolute };

    // Extra arguments are reported, and the first two are still compiled.
    auto parts = ConstEval::range_parts(*ast_graph, i);
    if (parts.error)
        sink(*std::move(parts.error));

    std::optional<Return<SCAnyExpr_I>> first;
    std::optional<Return<SCAnyExpr_I>> second;
    if (parts.first)
        first = (*this)(*parts.first);
    if (parts.second)
        second = (*this)(*parts.second);
    if (!first) {
        const auto zero = ctx.constant_table.register_constexpr({ std::int32_t { 0 } });
        const auto lit = ir.create(SCLiteral { zero }, location(i));
        ir.register_node_as_consteval(lit, zero);
        first = Return<SCAnyExpr_I> { lit, zero, TypeInfo { ctx.common_classes.int_, TypeConfidence::Absolute } };
    }

    std::optional<Return<SCAnyExpr_I>> last;
    if (parts.last)
        last = (*this)(*parts.last);
    else
        sink(sc::diag::Diagnostic::unbounded_range(get_text_info(*ast_graph), get_location(*ast_graph, i)));

    const auto missing = [&]() { return ir.create(SCMissing {}, location(i)); };
    const maybe<SCAnyExpr_I> second_i = second ? maybe<SCAnyExpr_I> { second->ir_node } : missing();
    const maybe<SCAnyExpr_I> last_i = last ? maybe<SCAnyExpr_I> { last->ir_node } : missing();
    const auto range = ir.create(SCRange {}, location(i), first->ir_node, second_i, last_i);

    const auto first_value = integer_constant(ctx, *first);
    const auto last_value = last ? integer_constant(ctx, *last) : std::nullopt;
    const auto second_value = second ? integer_constant(ctx, *second) : std::nullopt;
    if (!first_value || !last_value || (second && !second_value))
        return { range, type };
    // Folding assumes what series does.
    if (!ctx.known_methods.is_known(ctx.common_classes.int_, ctx.common_symbols.series))
        return { range, type };

    auto values = ConstEval::fold_series(*first_value, second_value, *last_value);
    if (!values)
        return { range, type };

    const auto const_i = ctx.constant_table.register_constexpr({ std::move(*values) });
    ir.register_node_as_consteval(range, const_i);
    return { range, const_i, type };
}

}
