#pragma once
#include "ast2ir/expr.hpp"

namespace sc::ir::ast2ir {

// Placeholder for nodes that aren't compiled yet, asserts and returns an empty expression list.
[[nodiscard]] Return<SCAnyExpr_I> unimplemented(IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph,
                                                ast::Index i);
}
