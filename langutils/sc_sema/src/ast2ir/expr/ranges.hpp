#pragma once
#include "ast2ir/ast2ir_base.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir_index.hpp"
#include <memory>

namespace sc::ir::ast2ir {

struct Expr;

// Whether a range needs its last. Optional for ranges that are only iterated, e.g., `x <- (1..)` in a generator, as
// nothing is created. Deprecated for `(1..).do(f)` before 3.16, a warning.
enum class RangeEnd { Required, Optional, Deprecated };

// `(first, second..last)` as an SCRange, folded into a constant array when it can be.
Return<SCAnyExpr_I> compile_range(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph,
                                  Context& ctx, ast::RangeIndex i, RangeEnd end);
}
