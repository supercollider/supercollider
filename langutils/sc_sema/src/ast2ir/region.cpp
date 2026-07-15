
#include "../ast2ir.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir.hpp"

namespace sc::ir::ast2ir {

Return Region::operator()(ast::ErrorIndex) {
    assert(false);
    return Return { NodeIndex { 0 } };
}

Return Region::operator()(ast::AnyExprIndex expr) {
    return std::visit(Expr { building_ir, ast_graph, ast_index, context }, ast_graph.index_to_variant(expr));
}

}

