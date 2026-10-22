#pragma once
#include "ast2ir/ast2ir_base.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir_index.hpp"
#include <memory>

namespace sc::ir::ast2ir {

struct Expr;

// `(first, second..last)` as an SCRange, folded into a constant array when it can be.
// A missing last is an error, unless unbounded is true, for ranges that are only iterated, e.g., `x <- (1..)` in a
// generator, as nothing is created.
Return<SCAnyExpr_I> compile_range(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph,
                                  Context& ctx, ast::RangeIndex i, bool unbounded);

}
