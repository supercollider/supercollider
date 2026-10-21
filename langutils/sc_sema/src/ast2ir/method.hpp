#pragma once
#include "ast2ir_base.hpp"
#include "ast2ir/expr.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir_index.hpp"

namespace sc::ir::ast2ir {

[[nodiscard]] Scope member_scope(std::shared_ptr<const IRGraph> class_ir, SCMemberList_I members);

struct Method : private Base {
    // The meta class's members (class variables) are put in the outer scope, and the class's own members in a child
    // scope so they shadow them.
    // The scope contains meta class membres (class vars), then the class instance variables.
    // Meta classes don't have a meta class, so they have no meta_class_members.
    Method(IRGraph& ir, std::shared_ptr<const ast::ASTGraph> ast, Context& cxt, const Scope* meta_class_members,
           const Scope& class_members);

    Return<SCMethod_I> operator()(ast::AnyValidMethodIndex);

private:
    // Declared before expr, as expr refers to it.
    DiagnosticSink sink;
    Expr expr;
};

}
