#pragma once
#include "ast2ir_base.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir_index.hpp"

namespace sc::ir::ast2ir {

struct Method : private Base {
    using Base::Base;
    Return<SCMethod_I> operator()(ast::AnyValidMethodIndex);

private:
    void append_args(SCArgumentDeclareList_I args, SCArgumentDeclareList_I var_args, ast::DeclareArgumentListIndex);
};

}