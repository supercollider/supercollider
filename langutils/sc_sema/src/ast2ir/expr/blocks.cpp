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
Return<SCAnyExpr_I> Expr::operator()(ast::BlockIndex i) {
    const auto [arg_list, maybe_contents] = ast_graph->children(i);
    const auto here = location({ *i });

    // The function is created first, so its arguments can refer to it as their scope.
    const auto positional = ir.create(SCArgumentDeclareList {}, location({ *arg_list }));
    const auto variadic = ir.create(SCArgumentDeclareList {}, location({ *arg_list }));
    const auto body = ir.create(SCBody {}, here);
    const auto function = ir.create(SCFunction {}, here, positional, variadic, body);

    scope.emplace_back();
    declare_arguments(*this, ir, ast_graph, ctx, positional, variadic, arg_list, function);

    BodyResult result;
    if (const auto contents = ast_graph->as_a<ast::BlockContentsListIndex>(maybe_contents))
        result = compile_body(*this, ir, ast_graph, ctx, function, body, *contents);
    else
        result.constant = ctx.constant_table.register_constexpr({ Nil {} });
    scope.pop_back();

    // A return leaves the enclosing method, so calling the function doesn't produce a value.
    const bool has_arguments = ir.children(positional).iter() || ir.children(variadic).iter();
    if (result.constant && !result.explicit_return && !has_arguments && !sink.fatal)
        ir.payload(function).constant_result = *result.constant;
    return { function };
}

}
