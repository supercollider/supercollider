#include "expr.hpp"
#include "sc_parser/ast.hpp"
#include "ast2ir/literal_synthesis.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"

#include "sc_parser/ast_untyped.hpp"
#include "sc_parser/ast_index_to_var.hpp"

namespace sc::ir::ast2ir {

template struct Return<SCAnyExpr_I>;

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

Return<SCAnyExpr_I> Expr::operator()(ast::ExprSeqIndex i) {
    const auto expr_list = ir.create(SCExprList {}, { ast_graph, { *i } });

    bool body_is_const_expr = true;
    std::optional<Return<SCAnyExpr_I>> last_index;
    for (auto expr_i = iter(*ast_graph, i); expr_i; ++expr_i) {
        last_index = std::visit(*this, to_variant(*ast_graph, *expr_i));
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
    const auto& t = get_text_info(*ast_graph);
    const auto loc = get_location(*ast_graph, i);
    const auto txt = get_string_view(*ast_graph, i);
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
    const auto expr_return = std::visit(*this, to_variant(*ast_graph, expr));

    if (auto n = ast::next_sibling(*ast_graph, expr)) {
        ctx.diagnostics(
            sc::diag::Diagnostic::non_terminating_return(get_text_info(*ast_graph), get_location(*ast_graph, n)));
    }

    return { ir.create(SCExplicitReturn {}, { ast_graph, { *i } }, expr_return.ir_node) };
}

Return<SCAnyExpr_I> Expr::operator()(ast::ReferenceIndex i) {
    const auto [expr] = ast_graph->children(i);
    const auto expr_r = std::visit(*this, to_variant(*ast_graph, expr));
    return { ir.create(SCRef {}, { ast_graph, { *i } }, expr_r.ir_node) };
}
}
