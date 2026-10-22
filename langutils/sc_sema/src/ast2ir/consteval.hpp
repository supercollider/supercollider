#pragma once
#include "ast2ir_base.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/source_location.hpp"
#include "sc_util/overload.hpp"
#include <optional>
#include <cstdint>

namespace sc::ir {
struct ClassDeclaration;
struct SymbolTable;
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

    // The parts of a range, those that are missing are nullopt. Some forms in the grammar allow any number of arguments
    // before the `..`, but only two are meaningful, so error holds the reason when there are more.
    struct RangeParts {
        std::optional<ast::AnyExprIndex> first;
        std::optional<ast::AnyExprIndex> second;
        std::optional<ast::AnyExprIndex> last;
        std::optional<sc::diag::Diagnostic> error;
    };
    [[nodiscard]] static RangeParts range_parts(const ast::ASTGraph& ast_graph, ast::RangeIndex i);
    // `(: first, second..last)`, which has the same parts.
    [[nodiscard]] static RangeParts range_parts(const ast::ASTGraph& ast_graph, ast::RangeGeneratorIndex i);

    // The largest range that is folded into a constant array, larger ones are created when the code runs.
    static constexpr std::size_t max_constant_range_size = 1024;

    // `first.series(second, last)` for integers, if it can be folded.
    // Only folds when the step isn't zero and goes towards last, as those cases are unambiguous.
    [[nodiscard]] static std::optional<ConstExprArray>
    fold_series(std::int32_t first, std::optional<std::int32_t> second, std::int32_t last);

    // `receiver.copySeries(first, second, last)` for a constant array or string, if it can be folded. Missing first and
    // last are the ends, and indices outside the receiver are clipped. Like fold_series, it is only folded when the
    // step isn't zero and goes towards last, and when clipping is unambiguous. Doesn't check the method is known.
    [[nodiscard]] static std::optional<ConstExpression>
    fold_copy_series(const ConstExpression& receiver, std::optional<std::int32_t> first,
                     std::optional<std::int32_t> second, std::optional<std::int32_t> last, SymbolTable& symbols);

    // `lhs selector rhs` for a constant Integer or Float, if selector is `+`, `-`, `*` or `/` and the receiver's method
    // is known. Integers wrap like the 32 bit integers they are, and `/` always returns a Float. Division by zero isn't
    // folded.
    [[nodiscard]] static std::optional<std::pair<ConstExpression, TypeInfo>>
    fold_arithmetic(Context& cxt, SymSCSelector selector, const ConstExpression& lhs, const ConstExpression& rhs);

    // Dispatches to the overloads below. The dispatch is only instantiated in consteval.cpp, so use this rather than
    // visiting yourself. Not an operator() overload, as that would make the conversions to the sets below ambiguous.
    Result eval(ast::AnyExprIndex);

    // Hard nos
    // The parser has reported it, but where a constant is required it isn't one.
    Result operator()(ast::ErrorIndex);
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
    // Slices, `a[1..3]`, can be folded.
    Result operator()(ast::AtMessageIndex);
    // Arithmetic, `1 + 2`, can be folded.
    Result operator()(ast::BinaryMessageIndex);
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
