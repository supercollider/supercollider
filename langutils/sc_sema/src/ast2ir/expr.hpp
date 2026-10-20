#pragma once

#include "ast2ir_base.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir_index.hpp"

namespace sc::ir::ast2ir {

struct ScopeEntry {
    enum struct Kind { Local, Instance, Class, Argument };
    enum struct State { CurrentlyInitalising, Initalised };
    join<SCFunction_I, SCMemberList_I> scope_index;
    join<SCVariableDeclare_I, SCMember_I, SCArgumentDeclare_I> declare_index;
    std::optional<ConstExprIndex> const_expr;
    std::optional<TypeInfo> type_info;
    Kind kind;
    State state;
};

struct Expr : private Base {
    using Base::Base;
    Return<SCAnyExpr_I> operator()(ast::IntLitIndex i);
    Return<SCAnyExpr_I> operator()(ast::FloatLitIndex i);
    Return<SCAnyExpr_I> operator()(ast::ExprSeqIndex);
    Return<SCAnyExpr_I> operator()(ast::EnvIdentifierIndex);
    Return<SCAnyExpr_I> operator()(ast::NonLocalReturnExprIndex);

    Return<SCAnyExpr_I> operator()(ast::ReferenceIndex);
    Return<SCAnyExpr_I> operator()(ast::AssignmentIndex);
    Return<SCAnyExpr_I> operator()(ast::AssignmentAtIndex);
    Return<SCAnyExpr_I> operator()(ast::SetterIndex);
    Return<SCAnyExpr_I> operator()(ast::VariablePackAssignIndex);
    Return<SCAnyExpr_I> operator()(ast::GeneratorAnyIndex);
    Return<SCAnyExpr_I> operator()(ast::ImmediatelyEvaluatedBlockIndex);
    Return<SCAnyExpr_I> operator()(ast::RangeGeneratorIndex);
    Return<SCAnyExpr_I> operator()(ast::RangeAssignIndex);
    Return<SCAnyExpr_I> operator()(ast::ClassNameIdentifierIndex);
    Return<SCAnyExpr_I> operator()(ast::ASCIIIndex);
    Return<SCAnyExpr_I> operator()(ast::PiLitIndex);
    Return<SCAnyExpr_I> operator()(ast::AccidentalIndex);
    Return<SCAnyExpr_I> operator()(ast::StringLitIndex);
    Return<SCAnyExpr_I> operator()(ast::BooleanLitIndex);
    Return<SCAnyExpr_I> operator()(ast::NilLitIndex);
    Return<SCAnyExpr_I> operator()(ast::SymbolQuoteIndex);
    Return<SCAnyExpr_I> operator()(ast::SymbolSlashIndex);
    Return<SCAnyExpr_I> operator()(ast::AnyMessageIndex);
    Return<SCAnyExpr_I> operator()(ast::RangeIndex);
    Return<SCAnyExpr_I> operator()(ast::NamedIdentifierIndex);
    Return<SCAnyExpr_I> operator()(ast::CurryIndex);
    Return<SCAnyExpr_I> operator()(ast::ArrayIndex);
    Return<SCAnyExpr_I> operator()(ast::CollectionIndex);
    Return<SCAnyExpr_I> operator()(ast::DictionaryIndex);
    Return<SCAnyExpr_I> operator()(ast::BlockIndex);

    std::vector<std::unordered_map<Symbol, ScopeEntry>> scope;

    [[nodiscard]] std::optional<ScopeEntry> resolve_symbol(Symbol i, bool get_uninitalised = false) const;
};


}