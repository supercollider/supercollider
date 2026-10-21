// Literals, these are evaluated at compile time.
// Does not need the ast graph.
#include "ast2ir/expr.hpp"
#include "ast2ir/literal_synthesis.hpp"
#include "sc_parser/ast_index_to_var.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"

namespace sc::ir::ast2ir {

Return<SCAnyExpr_I> Expr::operator()(ast::IntLitIndex i) {
    const auto& int_node = payload(*ast_graph, i);
    const auto str = get_text_info(*ast_graph)->read(get_location(*ast_graph, i));
    const auto expr_index =
        ctx.constant_table.register_constexpr(ConstExpression { synthesize_literal(int_node, str) });
    const auto lit = ir.create(SCLiteral { expr_index }, ASTLocation { ast_graph, { *i } });
    ir.register_node_as_consteval(lit, expr_index);
    TypeInfo info { ctx.common_classes.int_, TypeConfidence::Absolute };
    return { lit, expr_index, info };
}

Return<SCAnyExpr_I> Expr::operator()(ast::FloatLitIndex i) {
    const ast::FloatNode& float_node = payload(*ast_graph, i);
    const auto str = get_text_info(*ast_graph)->read(get_location(*ast_graph, i));
    const auto expr_index =
        ctx.constant_table.register_constexpr(ConstExpression { synthesize_literal(float_node, str) });
    const auto lit = ir.create(SCLiteral { expr_index }, ASTLocation { ast_graph, { *i } });
    ir.register_node_as_consteval(lit, expr_index);
    TypeInfo info { ctx.common_classes.float_, TypeConfidence::Absolute };
    return { lit, expr_index, info };
}

}
