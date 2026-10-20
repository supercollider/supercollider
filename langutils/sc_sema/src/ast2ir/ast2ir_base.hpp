#pragma once
#include "sc_sema/const_expr_index.hpp"
#include "sc_sema/type_info.hpp"
#include <memory>

namespace sc::ir {
struct IRGraph;
struct Context;
}

namespace sc::ast {
struct ASTGraph;
}

namespace sc::ir::ast2ir {

struct Base {
    Base(IRGraph& ir, std::shared_ptr<const ast::ASTGraph> ast, Context& cxt):
        ctx(cxt),
        ir(ir),
        ast_graph(std::move(ast)) {}
    Context& ctx;
    IRGraph& ir;
    std::shared_ptr<const ast::ASTGraph> ast_graph;
};

template <typename I> struct Return {
    constexpr Return(I i): ir_node(i) {}
    constexpr Return(I i, TypeInfo info): ir_node(i), type(info) {}
    constexpr Return(I i, ConstExprIndex expr_i, TypeInfo info): ir_node(i), const_index(expr_i), type(info) {}

    I ir_node;
    std::optional<ConstExprIndex> const_index {};
    std::optional<TypeInfo> type {};
};


namespace priv {
template <typename T> struct TypeWrapper {};
}

template <typename T, typename... Is>
[[nodiscard]] static constexpr bool callable_with_impl(priv::TypeWrapper<std::variant<Is...>>) {
    return (std::is_invocable_v<T, Is> && ...);
};

template <typename T, typename I> [[nodiscard]] static constexpr bool callable_with() {
    return callable_with_impl<T>(priv::TypeWrapper<typename I::Variant> {});
};

}