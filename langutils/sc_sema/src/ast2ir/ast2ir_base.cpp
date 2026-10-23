#include "ast2ir/ast2ir_base.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/ir.hpp"
#include <utility>
#include <vector>

namespace sc::ir::ast2ir {

SourceLocation location_of(IRGraph& ir, const ast::ASTGraph& ast_graph, ast::Index i) {
    return ir.make_location(get_text_info(ast_graph), get_location(ast_graph, i));
}

std::optional<ast::Index> nested_too_deeply(const ast::ASTGraph& ast, ast::Index i) {
    // The nodes still to visit and their depth, an explicit stack, as recursing is what this guards against. Kept
    // between calls, one per thread, as every method is checked, so it is only allocated once.
    thread_local std::vector<std::pair<ast::Index, std::size_t>> pending;
    pending.clear();
    pending.push_back({ i, 0 });
    while (!pending.empty()) {
        auto [node, depth] = pending.back();
        pending.pop_back();
        if (depth > max_nesting_depth) {
            // Its innermost node, which is small, e.g., a name, so the diagnostic doesn't show all of what is nested.
            while (const auto child = ast.edges(node).first_child)
                node = ast::Index { *child };
            return node;
        }
        for (auto child = ast.edges(node).first_child; child; child = ast.edges(*child).next_sibling)
            pending.push_back({ ast::Index { *child }, depth + 1 });
    }
    return std::nullopt;
}
}
