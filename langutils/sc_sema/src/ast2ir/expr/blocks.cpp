// Blocks, which become functions. Their arguments are in a scope of their own, inside the enclosing scopes.
// Needs the ast graph for the children.
#include "ast2ir/arguments.hpp"
#include "ast2ir/expr.hpp"
#include "sc_parser/ast.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"

namespace sc::ir::ast2ir {

// A block captures its surroundings, so it is never constant.
Return<SCAnyExpr_I> Expr::operator()(ast::BlockIndex i) {
    const auto [arg_list, maybe_contents] = ast_graph->children(i);
    const ASTLocation location { ast_graph, { *i } };

    // The function is created first, so its arguments can refer to it as their scope.
    const auto positional = ir.create(SCArgumentDeclareList {}, { ast_graph, { *arg_list } });
    const auto variadic = ir.create(SCArgumentDeclareList {}, { ast_graph, { *arg_list } });
    const auto body = ir.create(SCExprList {}, location);
    const auto function = ir.create(SCFunction {}, location, positional, variadic, body);

    scope.emplace_back();
    declare_arguments(*this, ir, ast_graph, ctx, positional, variadic, arg_list, function);

    if (const auto contents = ast_graph->as_a<ast::BlockContentsListIndex>(maybe_contents)) {
        for (auto it = ast_graph->children(*contents).iter(); it; ++it)
            ir.append(body, (*this)(ast::BlockItemIndex { *it }).ir_node);
    }

    scope.pop_back();
    return { function };
}

}
