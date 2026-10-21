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

void Expr::add_symbol(Symbol n, ScopeEgntry e) { scope.back().insert({ n, e }); }

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
    const auto [child] = ast_graph->children(i);
    const auto loc = get_location(*ast_graph, child);
    const auto txt = get_string_view(*ast_graph, child);
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

namespace {
SCAssign::Kind to_assign_kind(ScopeEntry::Kind k) {
    switch (k) {
    case ScopeEntry::Kind::Local:
    case ScopeEntry::Kind::Argument:
        return SCAssign::Kind::Local;
    case ScopeEntry::Kind::Instance:
        return SCAssign::Kind::Instance;
    case ScopeEntry::Kind::Class:
        return SCAssign::Kind::Class;
    }
    assert(false && "unhandled ScopeEntry::Kind");
    return SCAssign::Kind::Local;
}
}

Return<SCAnyExpr_I> Expr::operator()(ast::AssignmentIndex i) {
    const auto [name, expr] = ast_graph->children(i);
    const auto expr_r = std::visit(*this, to_variant(*ast_graph, expr));

    const auto txt = get_string_view(*ast_graph, name);
    const auto name_sym = ctx.symbol_table.get_identifier(std::string { txt });
    const Locatable<SymSCIdentifier> located_name { { ast_graph, { *name } }, name_sym };

    const auto make_return = [&](SCAssign::Kind kind) -> Return<SCAnyExpr_I> {
        // Assignment returns the assigned value, but is never a const expression as it has a side effect.
        const auto index = ir.create(SCAssign { located_name, kind }, { ast_graph, { *i } }, expr_r.ir_node);
        if (expr_r.type)
            return { index, *expr_r.type };
        return { index };
    };

    // Environment vars are not registered in the scope.
    if (ast_graph->payload(i).target == ast::AssignmentNode::Target::Environment) {
        return make_return(SCAssign::Kind::Environment);
    }

    // TODO: error when assigning to a const once ScopeEntry records const-ness.
    if (const auto entry = resolve_symbol(name_sym))
        return make_return(to_assign_kind(entry->kind));

    ctx.diagnostics(
        sc::diag::Diagnostic::undefined_variable(get_text_info(*ast_graph), get_location(*ast_graph, name), txt));
    return expr_r;
}
}
