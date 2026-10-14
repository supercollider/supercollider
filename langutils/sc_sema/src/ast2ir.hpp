#pragma once

#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/sc_grammar_shared.hpp"
#include "sc_sema/ast_location.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/compiler.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/tables.hpp"
#include "sc_sema/type_info.hpp"
#include "sc_util/overload.hpp"
#include <type_traits>
#include <variant>

namespace sc::ir::ast2ir {

const static std::string meta_prefix = "Meta_";

template <typename T> struct TypeWrapper {};


template <typename T, typename... Is>
[[nodiscard]] static constexpr bool callable_with_impl(TypeWrapper<std::variant<Is...>>) {
    return (std::is_invocable_v<T, Is> && ...);
};

template <typename T, typename I> [[nodiscard]] static constexpr bool callable_with() {
    return callable_with_impl<T>(TypeWrapper<typename I::Variant> {});
};


////////////////////////////////////////////////////////////////////////////////
// Declaration step.
////////////////////////////////////////////////////////////////////////////////

struct DeclareClass {
    DeclareClass(Context& cx, const ast::ASTGraph& ast, ASTGraphIndex ast_i):
        cxt(cx),
        ast_graph(ast),
        ast_index(ast_i) {}
    struct Result {
        ClassDeclaration class_;
        ClassDeclaration meta_class_;
    };

    std::optional<Result> operator()(ast::ClassIndex);

private:
    void append_member_to_decl(ClassDeclaration&, ast::UntypedIndex accessor_i, ast::ReadWriteAccessor,
                               ast::NamedIdentifierIndex);
    void append_member_to_decl(ClassDeclaration&, ast::UntypedIndex accessor_i, ast::ReadWriteAccessor,
                               ast::DeclareVariableWithDefaultIndex);
    void append_member_to_decl(ClassDeclaration&, ast::UntypedIndex accessor_i, ast::ReadWriteAccessor,
                               ast::VariablePackAssignIndex);

    Context& cxt;
    const ast::ASTGraph& ast_graph;
    ASTGraphIndex ast_index;
};

void declare_class(const ast::ASTGraph&, ASTGraphIndex, Context&, const CommonClasses&, ast::ClassIndex);

////////////////////////////////////////////////////////////////////////////////
// Return type of most visitors
////////////////////////////////////////////////////////////////////////////////
struct Return {
    constexpr Return(NodeIndex i): ir_node(i) {}
    constexpr Return(NodeIndex i, TypeInfo info): ir_node(i), type(info) {}
    constexpr Return(NodeIndex i, ConstExprIndex expr_i, TypeInfo info): ir_node(i), const_index(expr_i), type(info) {}

    NodeIndex ir_node;
    std::optional<ConstExprIndex> const_index {};
    std::optional<TypeInfo> type {};
};


////////////////////////////////////////////////////////////////////////////////
// Base class of most visitors
////////////////////////////////////////////////////////////////////////////////

struct Base {
    Base(IRGraph& ir, const ast::ASTGraph& ast, ASTGraphIndex ast_i, Context& cxt):
        context(cxt),
        building_ir(ir),
        ast_graph(ast),
        ast_index(ast_i) {}
    Context& context;
    IRGraph& building_ir;
    const ast::ASTGraph& ast_graph;
    ASTGraphIndex ast_index;
};

////////////////////////////////////////////////////////////////////////////////
// Visitors
////////////////////////////////////////////////////////////////////////////////

/// ConstEval is special in that its return type is different as it is an error if we fail to produce a constexpr.
/// It is designed to be called...
//      ConstEval{...}(index_to_visit).visit([](ConstEval::Success){...}, [](Diagnostic&&){...});
struct ConstEval {
    ConstEval(Context& ct, const ast::ASTGraph& ast, ASTGraphIndex ast_i, ClassDeclaration* optional_class = nullptr):
        cxt(ct),
        ast_graph(ast),
        ast_index(ast_i),
        class_decl(optional_class) {}


    using Success = std::pair<ConstExprIndex, TypeInfo>;
    struct Result {
        Result(Success s): v(s) {}
        Result(Diagnostic d): v(std::move(d)) {}

        template <typename S, typename F> auto visit(S&& success, F&& failure) && {
            static_assert(std::is_invocable_v<S, Success>);
            static_assert(std::is_invocable_v<F, Diagnostic&&>);
            using SR = std::invoke_result_t<S, Success>;
            using FR = std::invoke_result_t<F, Diagnostic&&>;
            static_assert(std::is_same_v<SR, FR>);
            return std::visit(util::overload { std::forward<S>(success), std::forward<F>(failure) }, std::move(v));
        }

        [[nodiscard]] std::optional<Success> success() const;
        [[nodiscard]] std::optional<Diagnostic> diagnostic() const&;
        [[nodiscard]] std::optional<Diagnostic> diagnostic() &&;

    private:
        std::variant<Success, Diagnostic> v;
    };

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
    const ast::ASTGraph& ast_graph;
    ASTGraphIndex ast_index;
    ClassDeclaration* class_decl; // optional
    bool encountered_immutable_array { false };
};
static_assert(callable_with<ConstEval, ast::AnyExprIndex>());
static_assert(callable_with<ConstEval, ast::KeyBinOpIndex>());


struct Region : private Base {
    using Base::Base;
    Return operator()(ast::ErrorIndex);
    Return operator()(ast::AnyExprIndex expr);
};

static_assert(callable_with<Region, ast::AnyExprIndex>());
static_assert(callable_with<Region, ast::ErrorIndex>());

struct Expr : private Base {
    using Base::Base;
    Return operator()(ast::IntLitIndex i);
    Return operator()(ast::FloatLitIndex i);
    // TODO: implement all the types.
    template <typename T> Return operator()(T) {
        assert(false);
        return Return { NodeIndex { 0 } };
    };
};

static_assert(callable_with<Expr, ast::AnyExprIndex>());
}
