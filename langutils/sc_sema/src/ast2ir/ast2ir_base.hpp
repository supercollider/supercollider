#pragma once
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_parser/indexes_untyped.hpp"
#include "sc_sema/const_expr_index.hpp"
#include "sc_sema/source_location.hpp"
#include "sc_sema/type_info.hpp"
#include <memory>
#include <vector>

namespace sc::ir {
struct IRGraph;
struct Context;
}

namespace sc::ast {
struct ASTGraph;
}

namespace sc::ir::ast2ir {

// Where an ast node came from, in the IRGraph's own sources.
[[nodiscard]] SourceLocation location_of(IRGraph& ir, const ast::ASTGraph& ast_graph, ast::Index i);

struct Base {
    Base(IRGraph& ir, std::shared_ptr<const ast::ASTGraph> ast, Context& cxt):
        ctx(cxt),
        ir(ir),
        ast_graph(std::move(ast)) {}
    Context& ctx;
    IRGraph& ir;
    std::shared_ptr<const ast::ASTGraph> ast_graph;

    [[nodiscard]] SourceLocation location(ast::Index i) const { return location_of(ir, *ast_graph, i); }
};

// Collects the diagnostics of one unit of compilation, e.g., a method, so it knows whether it should be poisoned.
// This is NOT thread safe.
struct DiagnosticSink {
    void operator()(sc::diag::Diagnostic d) {
        fatal = fatal || d.fatal();
        diagnostics.push_back(std::move(d));
    }
    std::vector<sc::diag::Diagnostic> diagnostics;
    bool fatal { false };
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