#pragma once
#include "ast2ir_base.hpp"
#include "ast2ir/expr.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir_index.hpp"

namespace sc::ir::ast2ir {

// The members of a class, including those it inherits, as variables of its methods.
[[nodiscard]] Scope member_scope(std::shared_ptr<const IRGraph> class_ir, SCMemberList_I members);
// The members of a compiled class, whose root is the class.
[[nodiscard]] Scope member_scope(std::shared_ptr<const IRGraph> class_ir);

// Pushes the scopes a method of a class sees, its class variables (the meta class's members), then its instance
// variables, which shadow them. Meta classes don't have a meta class, so they have no meta_class_members.
// Interpreted code is compiled as if it were a method of Interpreter, so it uses these too.
void push_member_scopes(Expr& expr, const Scope* meta_class_members, const Scope& class_members);

struct Method : private Base {
    // See push_member_scopes.
    Method(IRGraph& ir, std::shared_ptr<const ast::ASTGraph> ast, Context& cxt, const Scope* meta_class_members,
           const Scope& class_members);

    Return<SCMethod_I> operator()(ast::AnyValidMethodIndex);

private:
    // Declared before expr, as expr refers to it.
    DiagnosticSink sink;
    Expr expr;
};

}
