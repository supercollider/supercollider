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
                       join<SCFunction_I, SCMethod_I> owner) {
    const auto declared = [&](SymSCIdentifier name, SCArgumentDeclare_I arg) {
        (void)expr.declared(name);
        if (ctx.common_symbols.is_special(name)) {
            const auto loc = ir.location(arg);
            expr.sink(sc::diag::Diagnostic::declared_special_variable(ir.text_info(loc.source), IRGraph::range(loc)));
            return;
        }
        ScopeEntry entry { nullptr, owner, arg, {}, {}, ScopeEntry::Kind::Argument, ScopeEntry::State::Initalised };
        entry.local = ir.payload(arg).local;
        expr.add_symbol(name, entry);
    };
    const auto local = [&](SymSCIdentifier name, ast::Index declaration) {
        return expr.local_for_declaration(name, location_of(ir, *ast_graph, declaration), owner);
    };

    for (auto arg_it = ast_graph->children(i).iter(); arg_it; ++arg_it) {
        std::visit(
            util::overload {
                [&](ast::NamedIdentifierIndex i) {
                    const auto sym = ctx.symbol_table.get_identifier(ast_graph->string(i));
                    const auto missing_default = ir.create(SCMissing {}, location_of(ir, *ast_graph, { *i }));
                    const auto arg = ir.create(SCArgumentDeclare { sym, local(sym, { *i }) },
                                               location_of(ir, *ast_graph, { *i }), missing_default);
                    ir.append(args, arg);
                    declared(sym, arg);
                },
                [&](ast::DeclareArgumentWithDefaultIndex i) {
                    const auto preseve_nil = ast_graph->payload(i).preserve_nil;
                    const auto [name, default_i] = ast_graph->children(i);
                    const auto sym = ctx.symbol_table.get_identifier(ast_graph->string(name));
                    // Compiled before this argument is declared, a function in it can still refer to it, see
                    // Expr::initialising.
                    const auto variable_local = local(sym, { *i });
                    Expr::Initialising variable { sym, expr.scope.size() - 1, location_of(ir, *ast_graph, { *i }),
                                                  variable_local };
                    variable.in_frame = true;
                    expr.initialising.push_back(variable);
                    // The parser records whether it is bracketed as preserve_nil, see
                    // Expr::in_unbracketed_argument_default.
                    const bool outer = expr.in_unbracketed_argument_default;
                    expr.in_unbracketed_argument_default = outer || preseve_nil;
                    const auto default_r = expr(ast::AnyExprIndex { default_i });
                    expr.in_unbracketed_argument_default = outer;
                    expr.initialising.pop_back();
                    const auto arg = ir.create(SCArgumentDeclare { sym, variable_local,
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
                    const auto arg = ir.create(SCArgumentDeclare { sym, local(sym, { *n }) },
                                               location_of(ir, *ast_graph, { *n }), missing_default);
                    ir.append(var_args, arg);
                    declared(sym, arg);
                },
            },
            ast_graph->index_to_variant(*arg_it));
    }
}
}
