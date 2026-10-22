// The dispatch over every node type is only instantiated here, as it is the slowest part of Expr to compile.
// Other translation units, including the rest of Expr, should call these rather than visiting themselves.
#include "ast2ir/expr.hpp"
#include "ast2ir/expr/unimplemented.hpp"
#include "ast2ir/partial_application.hpp"
#include "sc_parser/ast_index_to_var.hpp"

namespace sc::ir::ast2ir {

template struct Return<SCAnyExpr_I>;

Return<SCAnyExpr_I> Expr::operator()(ast::AnyExprIndex i) {
    // A call with `_` is a function, e.g., `_ + 1` is `{ |_| _ + 1 }`, unless they are already being applied, which is
    // how the call itself is compiled.
    if (const auto curries = curry_arguments(*ast_graph, { *i }); !curries.empty() && curries != applied_curries)
        return partially_apply(*this, ir, ast_graph, ctx, i, curries);
    return std::visit(*this, to_variant(*ast_graph, i));
}
}
