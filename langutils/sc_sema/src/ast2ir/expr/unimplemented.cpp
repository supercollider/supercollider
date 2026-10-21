// Nodes that aren't compiled yet. When implementing one, move it to the file that matches the headers it needs.
#include "ast2ir/expr/unimplemented.hpp"
#include "sc_sema/ir.hpp"

namespace sc::ir::ast2ir {

Return<SCAnyExpr_I> unimplemented(IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph,
                                  ast::OptionalIndex i) {
    assert(false && "not implemented");
    return { ir.create(SCExprList {}, { ast_graph, { *i } }) };
}

Return<SCAnyExpr_I> Expr::operator()(ast::AssignmentAtIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::BlockIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::CollectionIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::CurryIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::GeneratorAnyIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::ImmediatelyEvaluatedBlockIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::RangeAssignIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::RangeGeneratorIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::RangeIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::SetterIndex i) { return unimplemented(ir, ast_graph, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::VariablePackAssignIndex i) { return unimplemented(ir, ast_graph, i); }
}
