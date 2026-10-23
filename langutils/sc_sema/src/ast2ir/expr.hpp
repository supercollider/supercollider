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
    // A generator's qualifiers declare its variables, see SCGenerator.
    join<SCFunction_I, SCMethod_I, SCMemberList_I, SCGenQualList_I> scope_index;

    // All of these can declare variables.
    join<SCVariableDeclare_I, SCPackTarget_I, SCMember_I, SCArgumentDeclare_I, SCGenQualIterate_I, SCGenQualDeclare_I>
        declare_index;
    std::optional<ConstExprIndex> const_expr;
    std::optional<TypeInfo> type_info;
    Kind kind;
    State state;
    // Where the declaration is, when it is in another graph, read when the scope is made, as that graph can be changing
    // on another thread while this one compiles. See declaration_location.
    std::shared_ptr<const lex::TextInfo> declaration_text {};
    lex::SourceCodeRange declaration_range {};
    OptionalLocalIndex local {};
    std::optional<SymSCClassName> owning_class {};
    // A member declared `const`, which can't be assigned.
    bool is_const { false };
};

// Where entry is declared, as a location in ir.
[[nodiscard]] SourceLocation declaration_location(IRGraph& ir, const ScopeEntry& entry);

struct Scope : std::unordered_map<Symbol, ScopeEntry> {
    // The first scope of a function, e.g., a block. Code in a function runs when it is called, so it can refer to a
    // variable whose initial value it is in, see Expr::initialising.
    bool starts_function { false };
};

extern template struct Return<SCAnyExpr_I>;

struct Expr : private Base {
    // Diagnostics are reported to the sink, so the owner (e.g., the method) can tell whether any were fatal.
    Expr(IRGraph& ir, std::shared_ptr<const ast::ASTGraph> ast, Context& cxt, ast::LanguageVersion v,
         DiagnosticSink& diagnostic_sink):
        Base(ir, std::move(ast), cxt, v),
        sink(diagnostic_sink) {}

    // So the free functions given an Expr can check it.
    using Base::version;

    Return<SCAnyExpr_I> operator()(ast::ErrorIndex);
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

    // A variable or argument whose initial value is being compiled, it is only added to its scope afterwards. A
    // function in the initial value can refer to it, as the function is called after the variable is assigned, e.g.,
    // `var f = { f.value }`. Elsewhere in the initial value its name refers to what it hides, e.g., `var x = x + 1`
    // reads the outer `x`.
    // Legacy code's frame also holds the function's arguments and leading variables that aren't declared yet, so they
    // can be used before their declarations, see predeclare_legacy_frame.
    struct Initialising {
        Symbol name;
        std::size_t scope;
        SourceLocation declaration;
        LocalIndex local;
        // Assigned, or used in a function, before it is declared, so it can't be constant.
        bool escaped { false };

        // Everything down here referes to legacy look up rules. These are complicated, beware!

        // A function's argument or variable, which legacy code's frame holds, rather than, e.g., a generator's.
        bool in_frame { false };
        // In legacy code's frame, but its initial value isn't being compiled, it is declared later.
        bool later { false };
        // A legacy generator qualifier's, whose value the old compiler put outside the block declaring it, so the value
        // can't refer to it, even in a function, see find_initialising.
        bool outside_value { false };
    };

    std::vector<Initialising> initialising;
    std::vector<ast::CurryIndex> applied_curries;

    // Interpreted code should not use the '^'.
    bool returns_allowed { true };

    bool in_unbracketed_argument_default { false };

    // Legacy code does not do constant folding.
    // We never fold when in an unbracketed argument default.
    [[nodiscard]] bool can_fold_constants() const {
        return ast2ir::can_fold_constants(version) && !in_unbracketed_argument_default;
    }

    [[nodiscard]] std::optional<ScopeEntry> resolve_symbol(Symbol i, bool get_uninitalised = false) const;
    // Records a use of the local name resolves to, see LocalVariable::assigned_to and captured.
    void record_local_use(Symbol name, LocalIndex local, bool assigns);
    void add_symbol(Symbol, ScopeEntry);

    // Removes name from legacy code's frame, as it is declared in the current scope. Returns whether it escaped.
    bool declared(Symbol name);


    // Old code has unique rules regarding lookup.
    void predeclare_legacy_frame(join<SCFunction_I, SCMethod_I> owner, ast::DeclareArgumentListIndex args,
                                 std::optional<ast::BlockContentsListIndex> contents);
    // Removes what is left of the current scope's frame, e.g., a name declared twice.
    void close_legacy_frame();

    // Always adds a new variable to the table.
    [[nodiscard]] LocalIndex create_local(SymSCIdentifier name, SourceLocation declaration, SCLocalOwner_I owner);

    // The variable a declaration in the current scope declares. In legacy code predeclare_legacy_frame has already
    // created it, so uses before the declaration refer to it, and that one is returned, otherwise it is created.
    [[nodiscard]] LocalIndex local_for_declaration(SymSCIdentifier name, SourceLocation declaration,
                                                   SCLocalOwner_I owner);
};
}