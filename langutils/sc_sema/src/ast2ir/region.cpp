
#include "../ast2ir.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir.hpp"

namespace sc::ir::ast2ir {

Return<NodeIndex> Region::operator()(ast::ErrorIndex) {
    assert(false);
    return Return { NodeIndex { 0 } };
}

Return<NodeIndex> Region::operator()(ast::AnyExprIndex expr) {
    assert(false);
    return { 0 }; // std::visit(Expr { ir, ast_graph, ctx }, ast_graph->index_to_variant(expr));
}
}

