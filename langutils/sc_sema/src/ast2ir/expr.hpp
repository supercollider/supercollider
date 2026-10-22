#pragma once

#include "ast2ir_base.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir_index.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace sc::ir::ast2ir {

struct ScopeEntry {
    enum struct Kind { Local, Instance, Class, Argument };
    enum struct State { CurrentlyInitalising, Initalised };
    // The graph the indexes below are in, which might not be the one being compiled.
    // This happens when we are accessing a meta class entry, or when inlining.
    // Null means the graph being compiled, e.g., for arguments, as Expr doesn't own its graph.
    std::shared_ptr<const IRGraph> graph;
    join<SCFunction_I, SCMethod_I, SCMemberList_I> scope_index;
    // A variable declared in a pack, `var #a, b = c`, is its SCPackTarget.
    join<SCVariableDeclare_I, SCPackTarget_I, SCMember_I, SCArgumentDeclare_I> declare_index;
    std::optional<ConstExprIndex> const_expr;
    std::optional<TypeInfo> type_info;
    Kind kind;
    State state;
};

using Scope = std::unordered_map<Symbol, ScopeEntry>;

extern template struct Return<SCAnyExpr_I>;

struct Expr : private Base {
    // Diagnostics are reported to the sink, so the owner (e.g., the method) can tell whether any were fatal.
    Expr(IRGraph& ir, std::shared_ptr<const ast::ASTGraph> ast, Context& cxt, DiagnosticSink& diagnostic_sink):
        Base(ir, std::move(ast), cxt),
        sink(diagnostic_sink) {}

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
    Return<SCAnyExpr_I> operator()(ast::GeneratorIndex);
    Return<SCAnyExpr_I> operator()(ast::GeneratorFakeIndex);
    Return<SCAnyExpr_I> operator()(ast::RangeIndex);
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
    Return<SCAnyExpr_I> operator()(ast::NamedIdentifierIndex);
    Return<SCAnyExpr_I> operator()(ast::CurryIndex);
    Return<SCAnyExpr_I> operator()(ast::ArrayIndex);
    Return<SCAnyExpr_I> operator()(ast::CollectionIndex);
    Return<SCAnyExpr_I> operator()(ast::DictionaryIndex);

    Return<SCAnyExpr_I> operator()(ast::PrefixMessageIndex);
    Return<SCAnyExpr_I> operator()(ast::PostfixMessageIndex);
    Return<SCAnyExpr_I> operator()(ast::BinaryMessageIndex);
    Return<SCAnyExpr_I> operator()(ast::ValueMessageIndex);
    Return<SCAnyExpr_I> operator()(ast::NewMessageIndex);
    Return<SCAnyExpr_I> operator()(ast::AtMessageIndex);

    Return<SCAnyExpr_I> operator()(ast::BlockIndex);

    Return<SCAnyExpr_I> operator()(ast::AnyExprIndex);

    std::vector<Scope> scope;
    DiagnosticSink& sink;

    [[nodiscard]] std::optional<ScopeEntry> resolve_symbol(Symbol i, bool get_uninitalised = false) const;
    void add_symbol(Symbol, ScopeEntry);
};


}