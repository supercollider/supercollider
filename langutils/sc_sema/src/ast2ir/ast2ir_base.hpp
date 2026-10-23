#pragma once
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/indexes_untyped.hpp"
#include "sc_parser/sc_grammar_shared.hpp"
#include "sc_sema/const_expr_index.hpp"
#include "sc_sema/source_location.hpp"
#include "sc_sema/type_info.hpp"
#include <algorithm>
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
    Base(IRGraph& ir, std::shared_ptr<const ast::ASTGraph> ast, Context& cxt, ast::LanguageVersion v):
        ctx(cxt),
        ir(ir),
        ast_graph(std::move(ast)),
        version(v) {}
    Context& ctx;
    IRGraph& ir;
    std::shared_ptr<const ast::ASTGraph> ast_graph;
    // The version of the language the code is written in, which decides the rules that break compatibility with the
    // old compiler.
    ast::LanguageVersion version;

    [[nodiscard]] SourceLocation location(ast::Index i) const { return location_of(ir, *ast_graph, i); }
};

// Old legacy behaviour.
[[nodiscard]] constexpr bool can_fold_constants(ast::LanguageVersion v) { return v >= ast::LanguageVersion::V3_16; }

[[nodiscard]] constexpr bool is_frame_scoped(ast::LanguageVersion v) { return v < ast::LanguageVersion::V3_16; }

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

// The passes over the ast, e.g., Expr and ConstEval, recurse once for each level of nesting, so code nested deeper than
// this isn't compiled, as it could overflow the stack. Real code is rarely more than a hundred deep, but a long chain
// of binary operators, e.g., generated `a + b + c ...`, is as deep as it is long.
inline constexpr std::size_t max_nesting_depth { 4096 };

// The innermost node of i nested deeper than max_nesting_depth, if any is, found without recursing. Checked before
// compiling a method, a region of interpreted code, or a member's initial value, so the passes after can recurse.
[[nodiscard]] std::optional<ast::Index> nested_too_deeply(const ast::ASTGraph& ast, ast::Index i);

// Whether the parser reported an error inside the range, e.g., a bracket it repaired, so what was parsed isn't what was
// written, even without an error node.
[[nodiscard]] inline bool parse_error_inside(const ast::ASTGraph& ast, sc::lex::SourceCodeRange r) {
    return std::any_of(ast.diagnostics().begin(), ast.diagnostics().end(),
                       [&](const sc::diag::Diagnostic& d) { return d.fatal() && d.within(ast.text_info(), r); });
}

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