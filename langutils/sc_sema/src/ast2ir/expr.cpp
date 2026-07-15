#include "../ast2ir.hpp"
#include "literal_synthesis.hpp"

namespace sc::ir::ast2ir {

Return Expr::operator()(ast::IntLitIndex i) {
        const auto& int_node = ast_graph.payload(i);
        const auto str = ast_graph.text_info()->read(ast_graph.location(i));
        const auto expr_index =
            context.constant_table.register_constexpr(ConstExpression { synthesize_literal(int_node, str) });
        const auto lit = building_ir.create(SCLiteral { expr_index }, ASTLocation { ast_index, { *i } });
        building_ir.register_node_as_consteval(lit, expr_index);
        return lit;
    }

 Return Expr::operator()(ast::FloatLitIndex i) {
        const ast::FloatNode& float_node = ast_graph.payload(i);
        const auto str = ast_graph.text_info()->read(ast_graph.location(i));
        const auto const_expr_index =
            context.constant_table.register_constexpr(ConstExpression { synthesize_literal(float_node, str) });
        const auto lit = building_ir.create(SCLiteral { const_expr_index }, ASTLocation { ast_index, { *i } });
        building_ir.register_node_as_consteval(lit, const_expr_index);
        return lit;
    }

}
