#include "method.hpp"
#include "ast2ir/expr.hpp"
#include "sc_parser/ast.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"

namespace sc::ir::ast2ir {

Method::Method(IRGraph& ir, std::shared_ptr<const ast::ASTGraph> ast, Context& cxt,
               std::optional<SCMemberList_I> meta_class_members, SCMemberList_I class_members):
    Base(ir, ast, cxt),
    expr(ir, std::move(ast), cxt) {
    if (meta_class_members)
        push_member_scope(*meta_class_members, ScopeEntry::Kind::Class);
    push_member_scope(class_members, ScopeEntry::Kind::Instance);
}

void Method::push_member_scope(SCMemberList_I members, ScopeEntry::Kind kind) {
    expr.scope.emplace_back();
    for (auto it = ir.children(members).iter(); it; ++it) {
        const SCMember_I member_i = *it;
        const SCMember& member = ir.payload(member_i);
        std::optional<ConstExprIndex> const_expr;
        std::optional<TypeInfo> type_info;
        if (member.expr) {
            const_expr = *member.expr;
            type_info = member.expr_type_info;
        }
        expr.add_symbol(member.name,
                        ScopeEntry { members, member_i, const_expr, type_info, kind, ScopeEntry::State::Initalised });
    }
}

Return<SCMethod_I> Method::operator()(ast::AnyValidMethodIndex i) {
    const auto [name, arg_list, maybe_prim, body] =
        std::visit([&](auto i) { return ast_graph->children(i); }, ast_graph->index_to_variant(i));

    const auto argument_list = ir.create(SCArgumentDeclareList {}, { ast_graph, { *arg_list } });
    const auto variadic_args = ir.create(SCArgumentDeclareList {}, { ast_graph, { *arg_list } });

    using PrimT = std::optional<Locatable<SymSCPrimitive>>;
    const PrimT primitive = std::visit(util::overload {
                                           [](ast::MissingIndex) -> PrimT { return {}; },
                                           [&](ast::PrimitiveIdentifierIndex i) -> PrimT {
                                               return { {
                                                   { ast_graph, { *i } }, //
                                                   ctx.symbol_table.get_primitive(ast_graph->string(i)) //
                                               } };
                                           },
                                       },
                                       ast_graph->index_to_variant(maybe_prim));

    append_args(argument_list, variadic_args, arg_list);

    const auto body_i = ir.create(SCExprList {}, { ast_graph, { *body } });

    bool body_is_const_expr = true;
    std::optional<SCAnyExpr_I> last_index;

    // Methods have their own scope
    expr.scope.emplace_back();

    // If all the body is const expr, then we can freely ignore everything bar the final expr.
    for (auto it = ast_graph->children(body).iter(); it; ++it) {
        const auto r = expr(*it);
        body_is_const_expr = body_is_const_expr && static_cast<bool>(r.const_index);
        ir.append(body_i, r.ir_node);
        last_index = r.ir_node;
    }

    expr.scope.pop_back();

    const auto method = ir.create(SCMethod { primitive }, { ast_graph, { *i } }, argument_list, variadic_args, body_i);

    return { method };
}


void Method::append_args(SCArgumentDeclareList_I args, SCArgumentDeclareList_I var_args,
                         ast::DeclareArgumentListIndex i) {
    for (auto arg_it = ast_graph->children(i).iter(); arg_it; ++arg_it) {
        std::visit(
            util::overload {
                [&](ast::NamedIdentifierIndex i) {
                    const auto sym = ctx.symbol_table.get_identifier(ast_graph->string(i));
                    const auto missing_default = ir.create(SCMissing {}, { ast_graph, { *i } });
                    const auto arg = ir.create(SCArgumentDeclare { sym }, { ast_graph, { *i } }, missing_default);
                    ir.append(args, arg);
                },
                [&](ast::DeclareArgumentWithDefaultIndex i) {
                    const auto preseve_nil = ast_graph->payload(i).preserve_nil;
                    const auto [name, expr] = ast_graph->children(i);
                    const auto sym = ctx.symbol_table.get_identifier(ast_graph->string(name));
                    //     const auto r = std::visit(Expr { ir, ast_graph, ctx }, ast_graph->index_to_variant(expr));

                    // TODO: Reenable expr processing
                    const auto missing_default = ir.create(SCMissing {}, { ast_graph, { *i } });
                    const auto arg = ir.create(SCArgumentDeclare { sym,
                                                                   preseve_nil ? DefaultBehaviour::PreserveNil
                                                                               : DefaultBehaviour::DontPreserveNil },
                                               { ast_graph, { *i } }, missing_default);
                    ir.append(args, arg);
                },
                [&](ast::DeclareArgumentVariadicIndex i) {
                    const auto [n] = ast_graph->children(i);
                    const auto sym = ctx.symbol_table.get_identifier(ast_graph->string(n));
                    const auto missing_default = ir.create(SCMissing {}, { ast_graph, { *n } });
                    const auto arg = ir.create(SCArgumentDeclare { sym }, { ast_graph, { *n } }, missing_default);
                    ir.append(var_args, arg);
                },
            },
            ast_graph->index_to_variant(*arg_it));
    }
}

}