#include "expr.hpp"
// #include "sc_parser/ast.hpp"
#include "ast2ir/literal_synthesis.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"

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

Return<SCAnyExpr_I> Expr::operator()(ast::ExprSeqIndex i) {
    const auto expr_list = ir.create(SCExprList {}, { ast_graph, { *i } });

    bool body_is_const_expr = true;
    std::optional<Return<SCAnyExpr_I>> last_index;
    for (auto expr_i = ast_graph->children(i).iter(); expr_i; ++expr_i) {
        last_index = std::visit(*this, ast_graph->index_to_variant(*expr_i));
        if (!last_index->const_index) {
            assert(last_index->type);
            body_is_const_expr = false;
        }
        ir.append(expr_list, last_index->ir_node);
    }

    if (body_is_const_expr && last_index) {
        return { expr_list, *last_index->const_index, *last_index->type };
    }
    if (last_index->type)
        return { expr_list, *last_index->type };
    return { expr_list };
}


Return<SCAnyExpr_I> Expr::operator()(ast::EnvIdentifierIndex i) {
    const auto& t = ast_graph->text_info();
    const auto loc = ast_graph->location(i);
    const auto txt = ast_graph->string_view(i);
    const auto name_sym = ctx.symbol_table.get_identifier(std::string { txt });
    const auto index = ir.create(SCRead { name_sym, SCRead::Kind::Environment }, { ast_graph, { *i } });
    return { index };
}

std::optional<ScopeEntry> Expr::resolve_symbol(Symbol i, bool get_uninitalised) const {
    for (auto it = scope.rbegin(); it != scope.rend(); ++it) {
        if (auto fnd = it->find(i); fnd != it->end()) {
            if (fnd->second.state == ScopeEntry::State::Initalised || get_uninitalised) {
                return { fnd->second };
            }
        }
    }
    return {};
}

Return<SCAnyExpr_I> Expr::operator()(ast::NonLocalReturnExprIndex i) {
    const auto [expr] = ast_graph->children(i);
    const auto expr_return = std::visit(*this, ast_graph->index_to_variant(expr));

    const auto& edges = ast_graph->edges(i);
    if (edges.next_sibling) {
        ctx.diagnostics(sc::diag::Diagnostic::non_terminating_return(ast_graph->text_info(),
                                                                     ast_graph->location({ *edges.next_sibling })));
    }

    return { ir.create(SCExplicitReturn {}, { ast_graph, { *i } }, expr_return.ir_node) };
}
}
