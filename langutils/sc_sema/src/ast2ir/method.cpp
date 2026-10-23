#include "method.hpp"
#include "ast2ir/arguments.hpp"
#include "ast2ir/body.hpp"
#include "ast2ir/expr.hpp"
#include "sc_parser/ast.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"

namespace sc::ir::ast2ir {

Scope member_scope(std::shared_ptr<const IRGraph> class_ir, SCMemberList_I members) {
    Scope scope;
    for (auto it = class_ir->children(members).iter(); it; ++it) {
        const SCMember_I member_i = *it;
        const SCMember& member = class_ir->payload(member_i);
        std::optional<ConstExprIndex> const_expr;
        std::optional<TypeInfo> type_info;
        if (member.expr) {
            const_expr = *member.expr;
            type_info = member.expr_type_info;
        }
        const auto loc = class_ir->location(member_i);
        scope.insert(
            { member.name,
              ScopeEntry { class_ir, members, member_i, const_expr, type_info, ScopeEntry::Kind::Instance,
                           ScopeEntry::State::Initalised, class_ir->text_info(loc.source), IRGraph::range(loc) } });
    }
    return scope;
}

Scope member_scope(std::shared_ptr<const IRGraph> class_ir) {
    const auto root = class_ir->root();
    assert(root);
    const auto [members, _] = class_ir->children(std::get<SCClass_I>(*root));
    return member_scope(std::move(class_ir), members);
}

void push_member_scopes(Expr& expr, const Scope* meta_class_members, const Scope& class_members) {
    if (meta_class_members) {
        // The meta class's instance variables are this class's class variables.
        Scope class_vars = *meta_class_members;
        for (auto& [_, entry] : class_vars)
            entry.kind = ScopeEntry::Kind::Class;
        expr.scope.push_back(std::move(class_vars));
    }
    expr.scope.push_back(class_members);
}

Method::Method(IRGraph& ir, std::shared_ptr<const ast::ASTGraph> ast, Context& cxt, ast::LanguageVersion version,
               const Scope* meta_class_members, const Scope& class_members):
    Base(ir, ast, cxt, version),
    expr(ir, std::move(ast), cxt, version, sink) {
    push_member_scopes(expr, meta_class_members, class_members);
}

Return<SCMethod_I> Method::operator()(ast::AnyValidMethodIndex i) {
    const auto [name, arg_list, maybe_prim, body] =
        std::visit([&](auto i) { return ast_graph->children(i); }, ast_graph->index_to_variant(i));

    const auto argument_list = ir.create(SCArgumentDeclareList {}, location({ *arg_list }));
    const auto variadic_args = ir.create(SCArgumentDeclareList {}, location({ *arg_list }));

    using PrimT = std::optional<Locatable<SymSCPrimitive>>;
    const PrimT primitive = std::visit(util::overload {
                                           [](ast::MissingIndex) -> PrimT { return {}; },
                                           [&](ast::PrimitiveIdentifierIndex i) -> PrimT {
                                               return { {
                                                   location({ *i }), //
                                                   ctx.symbol_table.get_primitive(ast_graph->string(i)) //
                                               } };
                                           },
                                       },
                                       ast_graph->index_to_variant(maybe_prim));

    const auto body_i = ir.create(SCBody {}, location({ *body }));
    // The method is created first, so its arguments can refer to it as their scope.
    const auto method =
        ir.create(SCMethod { primitive, version }, location({ *i }), argument_list, variadic_args, body_i);

    // Methods have their own scope, holding the arguments and variables, inside the member scopes.
    expr.scope.emplace_back();
    declare_frame(expr, ir, ast_graph, ctx, arg_list, body);
    declare_arguments(expr, ir, ast_graph, ctx, argument_list, variadic_args, arg_list, method);
    const auto result = compile_body(expr, ir, ast_graph, ctx, method, body_i, body);
    expr.close_frame();
    expr.scope.pop_back();

    // A fatal error anywhere in the method poisons all of it.
    sink.fatal = sink.fatal || parse_error_inside(*ast_graph, ast_graph->location({ *i }));
    ir.payload(method).poisoned = sink.fatal;

    // Without `^` a method returns `this`, which isn't constant.
    // A primitive's body only runs when the primitive fails, so is never constant.
    const bool has_arguments = ir.children(argument_list).iter() || ir.children(variadic_args).iter();
    if (result.constant && result.explicit_return && !primitive && !has_arguments && !sink.fatal)
        ir.payload(method).constant_result = *result.constant;
    ctx.diagnostics(std::move(sink.diagnostics));
    sink = {};

    return { method };
}
}