#pragma once
#include "ast2ir_base.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir_index.hpp"

namespace sc::ir::ast2ir {

struct Expr;

// Shared by methods and blocks, they declare their arguments the same way.
// Defaults are compiled with expr, so a constant default is registered as one.
// Each argument is added to expr's current scope as it is declared, so later defaults can refer to earlier arguments.
// owner's frame holds them, see LocalVariable.
void declare_arguments(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph, Context& ctx,
                       SCArgumentDeclareList_I args, SCArgumentDeclareList_I var_args, ast::DeclareArgumentListIndex i,
                       join<SCFunction_I, SCMethod_I> owner);
}
