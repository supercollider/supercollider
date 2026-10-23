// Ranges, e.g., `(1..5)` and `(1, 3..9)`, and range generators, `(: 1..5)`.
// These become SCRange rather than a message, so later passes can choose how to lower them.
// Needs the ast graph for the children.
#include "ast2ir/consteval.hpp"
#include "ast2ir/expr.hpp"
#include "ast2ir/expr/ranges.hpp"
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

// The compiled parts of a range, a missing first is 0.
struct RangeNodes {
    Return<SCAnyExpr_I> first;
    std::optional<Return<SCAnyExpr_I>> second;
    std::optional<Return<SCAnyExpr_I>> last;
    maybe<SCAnyExpr_I> second_i;
    maybe<SCAnyExpr_I> last_i;
};

// Extra arguments are reported, and the first two are still compiled.
RangeNodes compile_parts(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph, Context& ctx,
                         ConstEval::RangeParts parts, ast::Index whole) {
    if (parts.error)
        expr.sink(*std::move(parts.error));

    const auto here = location_of(ir, *ast_graph, whole);
    std::optional<Return<SCAnyExpr_I>> first;
    std::optional<Return<SCAnyExpr_I>> second;
    std::optional<Return<SCAnyExpr_I>> last;
    if (parts.first)
        first = expr(*parts.first);
    if (parts.second)
        second = expr(*parts.second);
    if (!first) {
        const auto zero = ctx.constant_table.register_constexpr({ std::int32_t { 0 } });
        const auto lit = ir.create(SCLiteral { zero }, here);
        ir.register_node_as_consteval(lit, zero);
        first = Return<SCAnyExpr_I> { lit, zero, TypeInfo { ctx.common_classes.int_, TypeConfidence::Absolute } };
    }
    if (parts.last)
        last = expr(*parts.last);

    const auto missing = [&]() { return ir.create(SCMissing {}, here); };
    const maybe<SCAnyExpr_I> second_i = second ? maybe<SCAnyExpr_I> { second->ir_node } : missing();
    const maybe<SCAnyExpr_I> last_i = last ? maybe<SCAnyExpr_I> { last->ir_node } : missing();
    return { *first, second, last, second_i, last_i };
}

}

Return<SCAnyExpr_I> compile_range(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph,
                                  Context& ctx, ast::RangeIndex i, bool unbounded) {
    const TypeInfo type { ctx.common_classes.array, TypeConfidence::Absolute };
    const auto r = compile_parts(expr, ir, ast_graph, ctx, ConstEval::range_parts(*ast_graph, i), { *i });
    if (!r.last && !unbounded)
        expr.sink(sc::diag::Diagnostic::unbounded_range(get_text_info(*ast_graph), get_location(*ast_graph, i)));

    const auto range =
        ir.create(SCRange {}, location_of(ir, *ast_graph, { *i }), r.first.ir_node, r.second_i, r.last_i);

    const auto first_value = integer_constant(ctx, r.first);
    const auto last_value = r.last ? integer_constant(ctx, *r.last) : std::nullopt;
    const auto second_value = r.second ? integer_constant(ctx, *r.second) : std::nullopt;
    if (!folds_constants(expr.version) || !first_value || !last_value || (r.second && !second_value))
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

// `(first..last)`, `(first, second..last)`, `(..last)` is `(0..last)`.
// Without a last, it is unbounded, which is an error here as it would be created. Contexts that only iterate it, e.g.,
// generators, use compile_range directly.
Return<SCAnyExpr_I> Expr::operator()(ast::RangeIndex i) { return compile_range(*this, ir, ast_graph, ctx, i, false); }

// `(: first, second..last)`, a Routine, so it is never constant, and last can be missing.
Return<SCAnyExpr_I> Expr::operator()(ast::RangeGeneratorIndex i) {
    const auto r = compile_parts(*this, ir, ast_graph, ctx, ConstEval::range_parts(*ast_graph, i), { *i });
    return { ir.create(SCRangeGenerator {}, location({ *i }), r.first.ir_node, r.second_i, r.last_i) };
}
}
