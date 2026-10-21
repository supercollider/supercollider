// Reading and writing variables, and the scope used to resolve them.
// Needs the ast graph for the children and payloads, and the symbol table for the names.
#include "ast2ir/expr.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"

namespace sc::ir::ast2ir {

void Expr::add_symbol(Symbol n, ScopeEntry e) { scope.back().insert({ n, e }); }

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

Return<SCAnyExpr_I> Expr::operator()(ast::EnvIdentifierIndex i) {
    const auto& t = get_text_info(*ast_graph);
    const auto [child] = ast_graph->children(i);
    const auto loc = get_location(*ast_graph, child);
    const auto txt = get_string_view(*ast_graph, child);
    const auto name_sym = ctx.symbol_table.get_identifier(std::string { txt });
    const auto index = ir.create(SCRead { name_sym, SCRead::Kind::Environment }, { ast_graph, { *i } });
    return { index };
}

namespace {
SCRead::Kind to_read_kind(ScopeEntry::Kind k) {
    switch (k) {
    case ScopeEntry::Kind::Local:
    case ScopeEntry::Kind::Argument:
        return SCRead::Kind::Local;
    case ScopeEntry::Kind::Instance:
        return SCRead::Kind::Instance;
    case ScopeEntry::Kind::Class:
        return SCRead::Kind::Class;
    }
    assert(false && "unhandled ScopeEntry::Kind");
    return SCRead::Kind::Local;
}

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

// Reads are never constant, even when the variable's initial value is, as it might have been assigned since.
Return<SCAnyExpr_I> Expr::operator()(ast::NamedIdentifierIndex i) {
    const auto txt = get_string_view(*ast_graph, i);
    const auto name_sym = ctx.symbol_table.get_identifier(std::string { txt });
    if (const auto entry = resolve_symbol(name_sym))
        return { ir.create(SCRead { name_sym, to_read_kind(entry->kind) }, { ast_graph, { *i } }) };

    sink(sc::diag::Diagnostic::undefined_variable(get_text_info(*ast_graph), get_location(*ast_graph, i), txt));
    return { ir.create(SCExprList {}, { ast_graph, { *i } }) };
}

Return<SCAnyExpr_I> Expr::operator()(ast::AssignmentIndex i) {
    const auto [name, expr] = ast_graph->children(i);
    const auto expr_r = (*this)(ast::AnyExprIndex { expr });

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

    sink(sc::diag::Diagnostic::undefined_variable(get_text_info(*ast_graph), get_location(*ast_graph, name), txt));
    return expr_r;
}

}
