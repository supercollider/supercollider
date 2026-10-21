#pragma once

#include "sc_parser/sc_grammar_shared.hpp"
#include "sc_sema/ast_location.hpp"
#include "sc_sema/const_expr_index.hpp"
#include "sc_sema/ir_index.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_sema/type_info.hpp"
#include "sc_util/typed_graph_node.hpp"

namespace sc::ir {

namespace priv {
template <typename Index> using Terminal = sc::util::typed_graph::TerminalNode<Index>;
template <typename This, typename... ARGS> using Node = sc::util::typed_graph::Node<This, ARGS...>;
template <typename This, typename Child> using List = sc::util::typed_graph::ListNode<This, Child>;
}

////////////////////////////////////////////////////////////////////////////////

struct SCMissing : priv::Terminal<SCMissing_I> {
    static constexpr auto type_name { "SCMissing" };
};

// This is just a place holder, you need to look it up in the
struct SCLiteral : priv::Terminal<SCLiteral_I> {
    static constexpr auto type_name { "SCLiteral" };
    ConstExprIndex index;
    constexpr SCLiteral(ConstExprIndex i): index(i) {}
};

struct SCKwArg : priv::Node<SCKwArg_I(SCAnyExpr_I value)> {
    static constexpr auto type_name { "SCKwArg" };
    Locatable<SymSCIdentifier> name;
};

struct SCRead : priv::Terminal<SCRead_I> {
    static constexpr auto type_name { "SCRead" };
    enum struct Kind { Local, Instance, Class, Environment };
    SymSCIdentifier name;
    Kind kind;
    SCRead(SymSCIdentifier n, Kind k): name(n), kind(k) {}
};

struct SCAssign : priv::Node<SCAssign_I(SCAnyExpr_I)> {
    static constexpr auto type_name { "SCAssign" };
    enum struct Kind { Local, Instance, Class, Environment };
    Locatable<SymSCIdentifier> name;
    Kind kind;
    SCAssign(Locatable<SymSCIdentifier> n, Kind k): name(n), kind(k) {}
};


struct SCPositionalArgs : priv::List<SCPosArgList_I, SCAnyExpr_I> {
    static constexpr auto type_name { "SCPositionalArgs" };
    std::uint32_t size;
};

struct SCMessage : priv::Node<SCMessage_I(SCAnyExpr_I receiver, //
                                          maybe<SCPosArgList_I> positional_arguments, //
                                          maybe<SCKwArgList_I> keyword_arguments, //
                                          maybe<SCVariadicArgList_I> variadic_arguments, //
                                          maybe<SCAnyExpr_I> adverb //
                                          )> {
    static constexpr auto type_name { "SCMessage" };
    OptionallyLocatable<SymSCSelector> selector;
    SCMessage(OptionallyLocatable<SymSCSelector> sel): selector(sel) {}
};

struct SCRef : priv::Node<SCRef_I(SCAnyExpr_I)> {
    static constexpr auto type_name { "SCRef" };
};

struct SCVariableDeclare : priv::Node<SCVariableDeclare_I(SCAnyExpr_I value)> {
    static constexpr auto type_name { "SCVariableDeclare" };
    Locatable<SymSCIdentifier> name;
};

struct SCExplicitReturn : priv::Node<SCExplicitReturn_I(SCAnyExpr_I value)> {
    static constexpr auto type_name { "SCExplicitReturn" };
};

struct SCCollection : priv::List<SCCollection_I, SCAnyExpr_I> {
    static constexpr auto type_name { "SCCollection" };
    OptionallyLocatable<SymSCClassName> collection_class;
    std::uint32_t size;
};


enum struct DefaultBehaviour { NoDefault, PreserveNil, DontPreserveNil };
struct SCArgumentDeclare : priv::Node<SCArgumentDeclare_I(maybe<SCAnyExpr_I>)> {
    static constexpr auto type_name { "SCArgumentDeclare" };
    SymSCIdentifier name;
    DefaultBehaviour default_behaviour;
    SCArgumentDeclare(SymSCIdentifier n, DefaultBehaviour db = DefaultBehaviour::NoDefault):
        name(n),
        default_behaviour(db) {}
};


struct SCArgumentDeclareList : priv::List<SCArgumentDeclareList_I, SCArgumentDeclare_I> {
    static constexpr auto type_name { "SCArgumentDeclareList" };
};

struct SCMethod : priv::Node<SCMethod_I(SCArgumentDeclareList_I positional_arguments,
                                        SCArgumentDeclareList_I variadic_arguments, SCExprList_I body)> {
    static constexpr auto type_name { "SCMethod" };
    std::optional<Locatable<SymSCPrimitive>> primitive;
    SCMethod(std::optional<Locatable<SymSCPrimitive>> p): primitive(p) {}
};
struct SCFunction : priv::Node<SCFunction_I(SCArgumentDeclareList_I positional_arguments,
                                            SCArgumentDeclareList_I variadic_arguments, SCExprList_I body)> {
    static constexpr auto type_name { "SCFunction" };
};

struct SCMethodList : priv::List<SCMethodList_I, SCMethod_I> {
    static constexpr auto type_name { "SCMethodList" };
};

struct SCMember : priv::Node<SCMember_I()> {
    static constexpr auto type_name { "SCMember" };

    SCMember(SymSCIdentifier name, ast::Index name_index, SymSCClassName owning_class):
        name(name),
        name_index(name_index),
        owning_class(owning_class) {}

    SymSCIdentifier name;
    ast::Index name_index;
    SymSCClassName owning_class;
    OptionalConstExprIndex expr {};
    ast::OptionalIndex expr_i {};
    TypeInfo expr_type_info {};

    ast::ReadWriteAccessor accessor {};
    bool is_const {};
};

struct SCMemberList : priv::List<SCMemberList_I, SCMember> {
    static constexpr auto type_name { "SCMemberList" };
};

struct SCClass : priv::Node<SCClass_I(SCMemberList_I members, SCMethodList_I methods)> {
    static constexpr auto type_name { "SCClass" };
    enum struct MemoryLayout { Float, Slot, Int };
    SymSCClassName name;
    MemoryLayout memory_layout { MemoryLayout::Slot };
    std::optional<SymSCClassName> meta_class {};
    SCClass(SymSCClassName n): name(n) {}
};


struct SCExprList : priv::List<TypedNodeIndex<SCNodeType::ExprList>, SCAnyExpr_I> {
    static constexpr auto type_name { "SCExprList" };
};


}