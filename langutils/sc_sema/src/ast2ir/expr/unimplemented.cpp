// Nodes that aren't compiled yet. When implementing one, move it to the file that matches the headers it needs.
#include "ast2ir/expr.hpp"
#include "sc_sema/ir.hpp"

namespace sc::ir::ast2ir {

Return<SCAnyExpr_I> Expr::unimplemented(ast::OptionalIndex i) {
    assert(false && "not implemented");
    return { ir.create(SCExprList {}, { ast_graph, { *i } }) };
}

Return<SCAnyExpr_I> Expr::operator()(ast::AnyMessageIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::ArrayIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::AssignmentAtIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::AtMessageIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::BinaryMessageIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::BlockIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::ClassNameIdentifierIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::CollectionIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::CurryIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::DictionaryIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::GeneratorAnyIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::ImmediatelyEvaluatedBlockIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::NamedIdentifierIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::NewMessageIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::PostfixMessageIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::PrefixMessageIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::RangeAssignIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::RangeGeneratorIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::RangeIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::SetterIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::ValueMessageIndex i) { return unimplemented(i); }
Return<SCAnyExpr_I> Expr::operator()(ast::VariablePackAssignIndex i) { return unimplemented(i); }

}
