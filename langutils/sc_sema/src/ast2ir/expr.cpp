#include "../ast2ir.hpp"
#include "literal_synthesis.hpp"
#include "sc_sema/ast_location.hpp"
#include "sc_sema/ir.hpp"

namespace sc::ir::ast2ir {

Return<SCAnyExpr_I> Expr::operator()(ast::IntLitIndex i) {
    const auto& int_node = ast_graph->payload(i);
    const auto str = ast_graph->text_info()->read(ast_graph->location(i));
    const auto expr_index =
        ctx.constant_table.register_constexpr(ConstExpression { synthesize_literal(int_node, str) });
    const auto lit = ir.create(SCLiteral { expr_index }, ASTLocation { ast_graph, { *i } });
    ir.register_node_as_consteval(lit, expr_index);
    TypeInfo info { ctx.common_classes.int_, TypeConfidence::Absolute };
    return { lit, expr_index, info };
}

Return<SCAnyExpr_I> Expr::operator()(ast::FloatLitIndex i) {
    const ast::FloatNode& float_node = ast_graph->payload(i);
    const auto str = ast_graph->text_info()->read(ast_graph->location(i));
    const auto expr_index =
        ctx.constant_table.register_constexpr(ConstExpression { synthesize_literal(float_node, str) });
    const auto lit = ir.create(SCLiteral { expr_index }, ASTLocation { ast_graph, { *i } });
    ir.register_node_as_consteval(lit, expr_index);
    TypeInfo info { ctx.common_classes.float_, TypeConfidence::Absolute };
    return { lit, expr_index, info };
}
}
