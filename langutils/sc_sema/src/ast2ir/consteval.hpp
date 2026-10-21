#pragma once
#include "ast2ir_base.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ast_location.hpp"
#include "sc_util/overload.hpp"

namespace sc::ir {
struct ClassDeclaration;
}
namespace sc::ir::ast2ir {

struct ConstEval {
    ConstEval(Context& ct, std::shared_ptr<const ast::ASTGraph> ast, ASTGraphIndex ast_i,
              ClassDeclaration* optional_class = nullptr):
        cxt(ct),
        ast_graph(std::move(ast)),
        ast_index(ast_i),
        class_decl(optional_class) {}


    using Success = std::pair<ConstExprIndex, TypeInfo>;
    struct Result {
        Result(Success s): v(s) {}
        Result(sc::diag::Diagnostic d): v(std::move(d)) {}

        template <typename S, typename F> auto visit(S&& success, F&& failure) && {
            static_assert(std::is_invocable_v<S, Success>);
            static_assert(std::is_invocable_v<F, sc::diag::Diagnostic&&>);
            using SR = std::invoke_result_t<S, Success>;
            using FR = std::invoke_result_t<F, sc::diag::Diagnostic&&>;
            static_assert(std::is_same_v<SR, FR>);
            return std::visit(util::overload { std::forward<S>(success), std::forward<F>(failure) }, std::move(v));
        }

        [[nodiscard]] std::optional<Success> success() const;
        [[nodiscard]] std::optional<sc::diag::Diagnostic> diagnostic() const&;
        [[nodiscard]] std::optional<sc::diag::Diagnostic> diagnostic() &&;

    private:
        std::variant<Success, sc::diag::Diagnostic> v;
    };

    // Dispatches to the overloads below. The dispatch is only instantiated in consteval.cpp, so use this rather than
    // visiting yourself. Not an operator() overload, as that would make the conversions to the sets below ambiguous.
    Result eval(ast::AnyExprIndex);

    // Hard nos
    Result operator()(ast::ExprSeqIndex);
    Result operator()(ast::EnvIdentifierIndex);
    Result operator()(ast::NonLocalReturnExprIndex);
    Result operator()(ast::ReferenceIndex);
    Result operator()(ast::AssignmentIndex);
    Result operator()(ast::AssignmentAtIndex);
    Result operator()(ast::SetterIndex);
    Result operator()(ast::VariablePackAssignIndex);
    Result operator()(ast::GeneratorAnyIndex); // requires producing a routine
    Result operator()(ast::ImmediatelyEvaluatedBlockIndex);
    Result operator()(ast::RangeGeneratorIndex);
    Result operator()(ast::RangeAssignIndex);

    // Always possible
    Result operator()(ast::ClassNameIdentifierIndex);
    Result operator()(ast::ASCIIIndex);
    Result operator()(ast::IntLitIndex);
    Result operator()(ast::FloatLitIndex);
    Result operator()(ast::PiLitIndex);
    Result operator()(ast::AccidentalIndex);
    Result operator()(ast::StringLitIndex);
    Result operator()(ast::BooleanLitIndex);
    Result operator()(ast::NilLitIndex);
    Result operator()(ast::SymbolQuoteIndex);
    Result operator()(ast::SymbolSlashIndex);

    // Sometimes
    Result operator()(ast::AnyMessageIndex);
    Result operator()(ast::RangeIndex);
    Result operator()(ast::NamedIdentifierIndex);
    Result operator()(ast::CurryIndex);
    Result operator()(ast::ArrayIndex);
    Result operator()(ast::CollectionIndex);
    Result operator()(ast::DictionaryIndex);
    Result operator()(ast::BlockIndex);
    Result operator()(ast::KeyBinOpIndex);

    Context& cxt;
    std::shared_ptr<const ast::ASTGraph> ast_graph;
    ASTGraphIndex ast_index;
    ClassDeclaration* class_decl; // optional
    bool encountered_immutable_array { false };
};
static_assert(callable_with<ConstEval, ast::AnyExprIndex>());
static_assert(callable_with<ConstEval, ast::KeyBinOpIndex>());
}
