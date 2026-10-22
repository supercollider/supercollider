#include "ast2ir/body.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"

namespace sc::ir::ast2ir {

namespace {

struct BodyContext {
    Expr& expr;
    IRGraph& ir;
    Context& ctx;
    const std::shared_ptr<const ast::ASTGraph>& ast_graph;
    join<SCFunction_I, SCMethod_I> owner;
    SCBody_I body;
};

// `var a = 1`, or `var a` when value is missing.
void declare_variable(BodyContext& b, ast::NamedIdentifierIndex name_i, std::optional<ast::AnyExprIndex> value,
                      ast::OptionalIndex whole) {
    const auto& g = *b.ast_graph;
    // Compiled before the variable is declared, so it can't refer to itself.
    const maybe<SCAnyExpr_I> value_i = value
        ? maybe<SCAnyExpr_I> { b.expr(*value).ir_node }
        : maybe<SCAnyExpr_I> { b.ir.create(SCMissing {}, location_of(b.ir, g, whole)) };

    const auto name = b.ctx.symbol_table.get_identifier(std::string { get_string_view(g, name_i) });
    const auto declare =
        b.ir.create(SCVariableDeclare { { location_of(b.ir, g, name_i), name } }, location_of(b.ir, g, whole), value_i);
    b.ir.append(b.body, declare);

    // Arguments and variables of the same function share a scope, so a name can't be declared twice.
    auto& current = b.expr.scope.back();
    if (const auto fnd = current.find(name); fnd != current.end()) {
        const auto previous = b.ir.location(fnd->second.declare_index);
        b.expr.sink(sc::diag::Diagnostic::duplicate_variable(b.ir.text_info(previous.source), IRGraph::range(previous),
                                                             get_text_info(g), get_location(g, name_i)));
        return;
    }
    b.expr.add_symbol(
        name, ScopeEntry { nullptr, b.owner, declare, {}, {}, ScopeEntry::Kind::Local, ScopeEntry::State::Initalised });
}

void declare_variables(BodyContext& b, ast::DeclareVariableListIndex list) {
    const auto& g = *b.ast_graph;
    for (auto it = g.children(list).iter(); it; ++it) {
        const ast::DeclareAnyVariableIndex item = *it;
        if (const auto name = g.as_a<ast::NamedIdentifierIndex>(item)) {
            declare_variable(b, *name, std::nullopt, *name);
        } else if (const auto with_default = g.as_a<ast::DeclareVariableWithDefaultIndex>(item)) {
            const auto [name_i, value] = g.children(*with_default);
            declare_variable(b, name_i, value, *with_default);
        } else {
            // `var #a, b = c`, not implemented yet.
            b.expr.sink(sc::diag::Diagnostic::todo(get_text_info(g), get_location(g, item)));
        }
    }
}

}

std::optional<Return<SCAnyExpr_I>> compile_body(Expr& expr, IRGraph& ir,
                                                const std::shared_ptr<const ast::ASTGraph>& ast_graph, Context& ctx,
                                                join<SCFunction_I, SCMethod_I> owner, SCBody_I body,
                                                ast::BlockContentsListIndex contents) {
    BodyContext b { expr, ir, ctx, ast_graph, owner, body };
    std::optional<Return<SCAnyExpr_I>> last;
    for (auto it = ast_graph->children(contents).iter(); it; ++it) {
        const ast::BlockItemIndex item = *it;
        if (const auto declarations = ast_graph->as_a<ast::DeclareVariableListIndex>(item)) {
            declare_variables(b, *declarations);
            last = std::nullopt;
        } else {
            last = expr(ast::AnyExprIndex { *item });
            ir.append(body, last->ir_node);
        }
    }
    return last;
}

}
