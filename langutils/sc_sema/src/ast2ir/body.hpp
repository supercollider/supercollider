#pragma once
#include "ast2ir/expr.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir_index.hpp"
#include <optional>

namespace sc::ir::ast2ir {


struct BodyResult {
    // Set when every item is constant, so evaluating the body has no effects. An empty
    // body is nil, otherwise it is set to the last expression.
    std::optional<ConstExprIndex> constant;

    // Whether the last item was `^value`.
    // In functions, this makes the body not constant, and the opposite in methods.
    bool explicit_return { false };
};

Return<SCAnyExpr_I> compile_function(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph,
                                     Context& ctx, ast::Index whole, ast::DeclareArgumentListIndex arg_list,
                                     std::optional<ast::BlockContentsListIndex> contents);

[[nodiscard]] OptionalConstExprIndex can_be_prototyped(const ast::ASTGraph& g, ast::AnyExprIndex value,
                                                       std::optional<ConstExprIndex> constant);

[[nodiscard]] OptionalConstExprIndex can_be_prototyped(const ast::ASTGraph& g,
                                                       ast::DeclareArgumentWithDefaultIndex argument,
                                                       std::optional<ConstExprIndex> constant);

BodyResult compile_body(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph, Context& ctx,
                        join<SCFunction_I, SCMethod_I> owner, SCBody_I body, ast::BlockContentsListIndex contents);
}
