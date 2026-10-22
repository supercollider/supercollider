#pragma once
#include "ast2ir/ast2ir_base.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir_index.hpp"
#include <memory>
#include <vector>

namespace sc::ir::ast2ir {

struct Expr;

// Partial application, a `_` in a call makes a function whose arguments are the call's `_`, e.g., `_ + 1` is
// `{ |_| _ + 1 }`, and `a.foo(_, _)` is `{ |_1, _2| a.foo(_1, _2) }`.
// Only a `_` that is the receiver, an argument (including keyword and variadic arguments), or an element of the call
// counts, so `(_ + 1) * 2` multiplies a function. The calls are messages, including `a[_]`, `a[_] = v`, setters,
// slices and references (`` `_ `` is `Ref.new(_)`), ranges, arrays, collections and dictionaries.

// The `_` of node, in the order they are written, none if it isn't a call.
[[nodiscard]] std::vector<ast::CurryIndex> curry_arguments(const ast::ASTGraph& ast_graph, ast::Index node);

// The name of the function's argument for the nth `_` of count, from zero. `_` when there is one, otherwise `_1`,
// `_2`, ..., these can't be written as identifiers, so can't hide a variable.
[[nodiscard]] SymSCIdentifier curry_argument_name(Context& ctx, std::size_t n, std::size_t count);

// Compiles node, a call with curry_arguments, into a function with an argument for each.
Return<SCAnyExpr_I> partially_apply(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph,
                                    Context& ctx, ast::AnyExprIndex node, const std::vector<ast::CurryIndex>& curries);

}
