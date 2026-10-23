#pragma once

#include "ast2ir/ast2ir_base.hpp"
#include "ast2ir/expr.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir_index.hpp"

namespace sc::ir::ast2ir {

// Interpreted code, a region, e.g., `a = 1; a + 1`, compiled into a list of regions, or a file as a function, e.g.,
// `|x| x + 1`, compiled into a function, as if it were written inside `Interpreter:functionCompileContext`. The scopes
// are the Interpreter's, see push_member_scopes.
struct Interpreted : private Base {
    Interpreted(IRGraph& ir, std::shared_ptr<const ast::ASTGraph> ast, Context& cxt, ast::LanguageVersion version,
                const Scope* meta_class_members, const Scope& class_members);

    Return<SCInterpreted_I> operator()(ast::RegionListIndex);
    Return<SCInterpreted_I> operator()(ast::FileAsFunctionIndex);

private:
    // The function a region's item is compiled into.
    SCFunction_I expression(ast::AnyExprIndex);
    SCFunction_I immediately_evaluated(ast::ImmediatelyEvaluatedBlockIndex);
    // Puts the code in an SCInterpreted, and passes on the diagnostics.
    Return<SCInterpreted_I> finish(join<SCFunction_I, SCRegionList_I> code);

    // Declared before expr, as expr refers to it.
    DiagnosticSink sink;
    Expr expr;
};

}
