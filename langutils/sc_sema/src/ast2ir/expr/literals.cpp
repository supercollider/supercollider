// Literals, these are evaluated at compile time.
// Does not need the ast graph.
#include "ast2ir/consteval.hpp"
#include "ast2ir/expr.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"

namespace sc::ir::ast2ir {

namespace {
// ConstEval does the synthesis, so literals evaluate the same here and in constant expressions.
// Its dispatch is already compiled in consteval.cpp, so this doesn't instantiate another.
// The graph index is only needed when evaluating class members, not literals.
Return<SCAnyExpr_I> literal(Context& ctx, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph,
                            ast::LanguageVersion version, DiagnosticSink& sink, ast::AnyDefiniteConstantIndex i) {
    return ConstEval { ctx, ast_graph, ASTGraphIndex { 0 }, version }.eval(i).visit(
        [&](ConstEval::Success s) -> Return<SCAnyExpr_I> {
            const auto [expr_index, info] = s;
            const auto lit = ir.create(SCLiteral { expr_index }, location_of(ir, *ast_graph, { *i }));
            ir.register_node_as_consteval(lit, expr_index);
            return { lit, expr_index, info };
        },
        [&](sc::diag::Diagnostic&& d) -> Return<SCAnyExpr_I> {
            sink(std::move(d));
            return { ir.create(SCExprList {}, location_of(ir, *ast_graph, { *i })) };
        });
}
}

Return<SCAnyExpr_I> Expr::operator()(ast::IntLitIndex i) { return literal(ctx, ir, ast_graph, version, sink, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::FloatLitIndex i) { return literal(ctx, ir, ast_graph, version, sink, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::PiLitIndex i) { return literal(ctx, ir, ast_graph, version, sink, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::AccidentalIndex i) { return literal(ctx, ir, ast_graph, version, sink, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::ASCIIIndex i) { return literal(ctx, ir, ast_graph, version, sink, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::StringLitIndex i) { return literal(ctx, ir, ast_graph, version, sink, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::BooleanLitIndex i) { return literal(ctx, ir, ast_graph, version, sink, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::NilLitIndex i) { return literal(ctx, ir, ast_graph, version, sink, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::SymbolQuoteIndex i) { return literal(ctx, ir, ast_graph, version, sink, i); }
Return<SCAnyExpr_I> Expr::operator()(ast::SymbolSlashIndex i) { return literal(ctx, ir, ast_graph, version, sink, i); }

// The class, its type is the meta class. Legacy code doesn't know it is constant, as the old compiler didn't treat it
// as a literal.
Return<SCAnyExpr_I> Expr::operator()(ast::ClassNameIdentifierIndex i) {
    if (folds_constants(version))
        return literal(ctx, ir, ast_graph, version, sink, i);
    const auto r = ConstEval { ctx, ast_graph, ASTGraphIndex { 0 }, version }(i).success();
    assert(r);
    return { ir.create(SCLiteral { r->first }, location({ *i })), r->second };
}
}
