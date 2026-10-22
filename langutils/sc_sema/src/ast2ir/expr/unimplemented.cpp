// Nodes that aren't compiled yet. When implementing one, move it to the file that matches the headers it needs.
#include "ast2ir/expr/unimplemented.hpp"
#include "sc_sema/ir.hpp"

namespace sc::ir::ast2ir {

Return<SCAnyExpr_I> unimplemented(IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph,
                                  ast::OptionalIndex i) {
    assert(false && "not implemented");
    return { ir.create(SCExprList {}, location_of(ir, *ast_graph, i)) };
}

Return<SCAnyExpr_I> Expr::operator()(ast::CurryIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::GeneratorIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::GeneratorFakeIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::ImmediatelyEvaluatedBlockIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::RangeGeneratorIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::VariablePackAssignIndex i) { return unimplemented(ir, ast_graph, i); }
}
