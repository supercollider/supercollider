#pragma once

#include "ast2ir/ast2ir_base.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir_index.hpp"

namespace sc::ir::ast2ir {

struct Region : private Base {
    using Base::Base;
    Return<NodeIndex> operator()(ast::ErrorIndex);
    Return<NodeIndex> operator()(ast::AnyExprIndex expr);
    Return<NodeIndex> operator()(ast::ImmediatelyEvaluatedBlockIndex expr);
};

static_assert(callable_with<Region, ast::RegionItemIndex>());
}