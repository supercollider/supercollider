#pragma once
#include "ast2ir_base.hpp"
#include "ast2ir/expr.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir_index.hpp"

namespace sc::ir::ast2ir {

struct Method : private Base {
    Method(IRGraph& ir, std::shared_ptr<const ast::ASTGraph> ast, Context& cxt,
           std::optional<SCMemberList_I> meta_class_members, SCMemberList_I class_members);

    Return<SCMethod_I> operator()(ast::AnyValidMethodIndex);

private:
    void append_args(SCArgumentDeclareList_I args, SCArgumentDeclareList_I var_args, ast::DeclareArgumentListIndex);
    void push_member_scope(SCMemberList_I members, ScopeEntry::Kind kind);

    Expr expr;
};

}
