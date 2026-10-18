#pragma once

#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/sc_grammar_shared.hpp"
#include "sc_sema/ast_location.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir_index.hpp"
#include "sc_sema/type_info.hpp"
#include "sc_util/overload.hpp"
#include <type_traits>
#include <variant>

namespace sc::ir {
struct IRGraph;
}

namespace sc::ir::ast2ir {


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
    DeclareClass(Context& cx, std::shared_ptr<const ast::ASTGraph> ast, ASTGraphIndex ast_i, ModuleIndex mod):
        cxt(cx),
        ast_graph(ast),
        ast_index(ast_i),
        module(mod) {}
    struct Result {
        ClassDeclaration class_;
        ClassDeclaration meta_class_;
    };

    std::optional<Result> operator()(ast::ClassIndex);

private:
    void append_member_to_decl(ClassDeclaration&, ast::Index, ast::UntypedIndex accessor_i, ast::ReadWriteAccessor,
                               ast::NamedIdentifierIndex);
    void append_member_to_decl(ClassDeclaration&, ast::Index, ast::UntypedIndex accessor_i, ast::ReadWriteAccessor,
                               ast::DeclareVariableWithDefaultIndex);
    void append_member_to_decl(ClassDeclaration&, ast::Index, ast::UntypedIndex accessor_i, ast::ReadWriteAccessor,
                               ast::VariablePackAssignIndex);

    Context& cxt;
    std::shared_ptr<const ast::ASTGraph> ast_graph;
    ASTGraphIndex ast_index;
    ModuleIndex module;
};

void declare_class(const ast::ASTGraph&, ASTGraphIndex, Context&, const CommonClasses&, ast::ClassIndex);

////////////////////////////////////////////////////////////////////////////////
// Return type of most visitors
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Base class of most visitors
////////////////////////////////////////////////////////////////////////////////

struct Base {
    Base(IRGraph& ir, std::shared_ptr<const ast::ASTGraph> ast, Context& cxt):
        ctx(cxt),
        ir(ir),
        ast_graph(std::move(ast)) {}
    Context& ctx;
    IRGraph& ir;
    std::shared_ptr<const ast::ASTGraph> ast_graph;
};

////////////////////////////////////////////////////////////////////////////////
// Visitors
////////////////////////////////////////////////////////////////////////////////

/// ConstEval is special in that its return type is different as it is an error if we fail to produce a constexpr.
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


template <typename I> struct Return {
    constexpr Return(I i): ir_node(i) {}
    constexpr Return(I i, TypeInfo info): ir_node(i), type(info) {}
    constexpr Return(I i, ConstExprIndex expr_i, TypeInfo info): ir_node(i), const_index(expr_i), type(info) {}

    I ir_node;
    std::optional<ConstExprIndex> const_index {};
    std::optional<TypeInfo> type {};
};

struct Region : private Base {
    using Base::Base;
    Return<NodeIndex> operator()(ast::ErrorIndex);
    Return<NodeIndex> operator()(ast::AnyExprIndex expr);
};

static_assert(callable_with<Region, ast::AnyExprIndex>());
static_assert(callable_with<Region, ast::ErrorIndex>());

struct Expr : private Base {
    using Base::Base;
    Return<SCAnyExpr_I> operator()(ast::IntLitIndex i);
    Return<SCAnyExpr_I> operator()(ast::FloatLitIndex i);
    // TODO: implement all the types.
    template <typename T> Return<SCAnyExpr_I> operator()(T) {
        assert(false);
        return { SCAnyExpr_I { 0 } };
    };
};

static_assert(callable_with<Expr, ast::AnyExprIndex>());


struct Method : private Base {
    using Base::Base;
    Return<SCMethod_I> operator()(ast::AnyValidMethodIndex);

private:
    void append_args(SCArgumentDeclareList_I args, SCArgumentDeclareList_I var_args, ast::DeclareArgumentListIndex);
};
}
