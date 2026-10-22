// Blocks, which become functions. Their arguments are in a scope of their own, inside the enclosing scopes.
// Needs the ast graph for the children.
#include "ast2ir/arguments.hpp"
#include "ast2ir/body.hpp"
#include "ast2ir/expr.hpp"
#include "sc_parser/ast.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"

namespace sc::ir::ast2ir {

// A block captures its surroundings, so it is never constant.
Return<SCAnyExpr_I> compile_function(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph,
                                     Context& ctx, ast::Index whole, ast::DeclareArgumentListIndex arg_list,
                                     std::optional<ast::BlockContentsListIndex> contents) {
    const auto here = location_of(ir, *ast_graph, whole);

    // The function is created first, so its arguments can refer to it as their scope.
    const auto positional = ir.create(SCArgumentDeclareList {}, location_of(ir, *ast_graph, { *arg_list }));
    const auto variadic = ir.create(SCArgumentDeclareList {}, location_of(ir, *ast_graph, { *arg_list }));
    const auto body = ir.create(SCBody {}, here);
    const auto function = ir.create(SCFunction {}, here, positional, variadic, body);

    expr.scope.emplace_back();
    expr.scope.back().starts_function = true;
    declare_arguments(expr, ir, ast_graph, ctx, positional, variadic, arg_list, function);

    BodyResult result;
    if (contents)
        result = compile_body(expr, ir, ast_graph, ctx, function, body, *contents);
    else
        result.constant = ctx.constant_table.register_constexpr({ Nil {} });
    expr.scope.pop_back();

    // A return leaves the enclosing method, so calling the function doesn't produce a value.
    const bool has_arguments = ir.children(positional).iter() || ir.children(variadic).iter();
    if (result.constant && !result.explicit_return && !has_arguments && !expr.sink.fatal)
        ir.payload(function).constant_result = *result.constant;
    return { function };
}

Return<SCAnyExpr_I> Expr::operator()(ast::BlockIndex i) {
    const auto [arg_list, maybe_contents] = ast_graph->children(i);
    std::optional<ast::BlockContentsListIndex> contents;
    if (const auto c = ast_graph->as_a<ast::BlockContentsListIndex>(maybe_contents))
        contents = *c;
    return compile_function(*this, ir, ast_graph, ctx, { *i }, arg_list, contents);
}
}
