#include "ast2ir/arguments.hpp"
#include "ast2ir/expr.hpp"
#include "sc_parser/ast.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_util/overload.hpp"

namespace sc::ir::ast2ir {

void declare_arguments(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph, Context& ctx,
                       SCArgumentDeclareList_I args, SCArgumentDeclareList_I var_args, ast::DeclareArgumentListIndex i,
                       std::optional<join<SCFunction_I, SCMethod_I>> scope_owner) {
    const auto declared = [&](SymSCIdentifier name, SCArgumentDeclare_I arg) {
        if (scope_owner)
            expr.add_symbol(
                name,
                ScopeEntry {
                    nullptr, *scope_owner, arg, {}, {}, ScopeEntry::Kind::Argument, ScopeEntry::State::Initalised });
    };

    for (auto arg_it = ast_graph->children(i).iter(); arg_it; ++arg_it) {
        std::visit(
            util::overload {
                [&](ast::NamedIdentifierIndex i) {
                    const auto sym = ctx.symbol_table.get_identifier(ast_graph->string(i));
                    const auto missing_default = ir.create(SCMissing {}, location_of(ir, *ast_graph, { *i }));
                    const auto arg =
                        ir.create(SCArgumentDeclare { sym }, location_of(ir, *ast_graph, { *i }), missing_default);
                    ir.append(args, arg);
                    declared(sym, arg);
                },
                [&](ast::DeclareArgumentWithDefaultIndex i) {
                    const auto preseve_nil = ast_graph->payload(i).preserve_nil;
                    const auto [name, default_i] = ast_graph->children(i);
                    const auto sym = ctx.symbol_table.get_identifier(ast_graph->string(name));
                    // Compiled before this argument is declared, so it can't refer to itself.
                    const auto default_r = expr(ast::AnyExprIndex { default_i });
                    const auto arg = ir.create(SCArgumentDeclare { sym,
                                                                   preseve_nil ? DefaultBehaviour::PreserveNil
                                                                               : DefaultBehaviour::DontPreserveNil },
                                               location_of(ir, *ast_graph, { *i }), default_r.ir_node);
                    ir.append(args, arg);
                    declared(sym, arg);
                },
                [&](ast::DeclareArgumentVariadicIndex i) {
                    const auto [n] = ast_graph->children(i);
                    const auto sym = ctx.symbol_table.get_identifier(ast_graph->string(n));
                    const auto missing_default = ir.create(SCMissing {}, location_of(ir, *ast_graph, { *n }));
                    const auto arg =
                        ir.create(SCArgumentDeclare { sym }, location_of(ir, *ast_graph, { *n }), missing_default);
                    ir.append(var_args, arg);
                    declared(sym, arg);
                },
            },
            ast_graph->index_to_variant(*arg_it));
    }
}
}
