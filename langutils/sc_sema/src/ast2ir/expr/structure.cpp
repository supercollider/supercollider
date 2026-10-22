// Expressions that only contain other expressions: sequences and returns.
// Needs the ast graph for the children.
#include "ast2ir/expr.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/ast_index_to_var.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"

namespace sc::ir::ast2ir {

// The parser has reported it and recovered. The code can't run, so it is fatal, but has no diagnostic of its own.
Return<SCAnyExpr_I> Expr::operator()(ast::ErrorIndex i) {
    sink.fatal = true;
    return { ir.create(SCExprList {}, location({ *i })) };
}

Return<SCAnyExpr_I> Expr::operator()(ast::ExprSeqIndex i) {
    const auto expr_list = ir.create(SCExprList {}, location({ *i }));

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
    if (!returns_allowed) {
        sink(sc::diag::Diagnostic::return_in_interpreted_code(get_text_info(*ast_graph), get_location(*ast_graph, i)));
        return expr_return;
    }

    if (auto n = ast::next_sibling(*ast_graph, expr)) {
        sink(sc::diag::Diagnostic::non_terminating_return(get_text_info(*ast_graph), get_location(*ast_graph, n)));
    }

    return { ir.create(SCExplicitReturn {}, location({ *i }), expr_return.ir_node) };
}
}
