#pragma once
#include "ast2ir/expr.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir_index.hpp"
#include <optional>

namespace sc::ir::ast2ir {

// Compiles the items of a method's or function's body, its expressions and variable declarations, into body.
// Variables are added to expr's current scope, which the caller opens, as it also holds the arguments. Each is only in
// scope for the items after it.
struct BodyResult {
    // Set when every item is constant, so evaluating the body has no effects, to the last expression's value. An empty
    // body is nil.
    std::optional<ConstExprIndex> constant;
    // Whether the last item was `^value`, constant is then the value.
    bool explicit_return { false };
};

BodyResult compile_body(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph, Context& ctx,
                        join<SCFunction_I, SCMethod_I> owner, SCBody_I body, ast::BlockContentsListIndex contents);
}
