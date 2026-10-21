// Expressions that only contain other expressions: sequences, returns and references.
// Needs the ast graph for the children.
#include "ast2ir/expr.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/ast_index_to_var.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"

namespace sc::ir::ast2ir {

Return<SCAnyExpr_I> Expr::operator()(ast::ExprSeqIndex i) {
    const auto expr_list = ir.create(SCExprList {}, { ast_graph, { *i } });

    bool body_is_const_expr = true;
    std::optional<Return<SCAnyExpr_I>> last_index;
    for (auto expr_i = iter(*ast_graph, i); expr_i; ++expr_i) {
        last_index = (*this)(ast::AnyExprIndex { *expr_i });
        if (!last_index->const_index)
            body_is_const_expr = false;
        ir.append(expr_list, last_index->ir_node);
    }

    if (body_is_const_expr && last_index) {
        return { expr_list, *last_index->const_index, *last_index->type };
    }
    if (last_index->type)
        return { expr_list, *last_index->type };
    return { expr_list };
}

Return<SCAnyExpr_I> Expr::operator()(ast::NonLocalReturnExprIndex i) {
    const auto [expr] = ast_graph->children(i);
    const auto expr_return = (*this)(ast::AnyExprIndex { expr });

    if (auto n = ast::next_sibling(*ast_graph, expr)) {
        ctx.diagnostics(
            sc::diag::Diagnostic::non_terminating_return(get_text_info(*ast_graph), get_location(*ast_graph, n)));
    }

    return { ir.create(SCExplicitReturn {}, { ast_graph, { *i } }, expr_return.ir_node) };
}

Return<SCAnyExpr_I> Expr::operator()(ast::ReferenceIndex i) {
    const auto [expr] = ast_graph->children(i);
    const auto expr_r = (*this)(ast::AnyExprIndex { expr });
    return { ir.create(SCRef {}, { ast_graph, { *i } }, expr_r.ir_node) };
}

}
