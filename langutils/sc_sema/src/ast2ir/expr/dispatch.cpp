// The dispatch over every node type is only instantiated here, as it is the slowest part of Expr to compile.
// Other translation units, including the rest of Expr, should call these rather than visiting themselves.
#include "ast2ir/expr.hpp"
#include "sc_parser/ast_index_to_var.hpp"

namespace sc::ir::ast2ir {

template struct Return<SCAnyExpr_I>;

Return<SCAnyExpr_I> Expr::operator()(ast::AnyExprIndex i) { return std::visit(*this, to_variant(*ast_graph, i)); }
template <typename T> Return<SCAnyExpr_I> Expr::operator()(T i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::BlockItemIndex i) { return std::visit(*this, to_variant(*ast_graph, i)); }

}
