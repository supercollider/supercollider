#pragma once

#include "sc_parser/sc_grammar_shared.hpp"
#include "sc_sema/source_location.hpp"
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
    SCKwArg(Locatable<SymSCIdentifier> n): name(n) {}
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
    std::uint32_t size {};
};

struct SCKwArgs : priv::List<SCKwArgList_I, SCKwArg_I> {
    static constexpr auto type_name { "SCKwArgs" };
};

// The expressions passed with `*`, whose elements are appended to the positional arguments.
struct SCVariadicArgs : priv::List<SCVariadicArgList_I, SCAnyExpr_I> {
    static constexpr auto type_name { "SCVariadicArgs" };
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

// `var a = 1`, value is missing for `var a`, which is nil.
struct SCVariableDeclare : priv::Node<SCVariableDeclare_I(maybe<SCAnyExpr_I> value)> {
    static constexpr auto type_name { "SCVariableDeclare" };
    Locatable<SymSCIdentifier> name;
    SCVariableDeclare(Locatable<SymSCIdentifier> n): name(n) {}
};

// The body of a method or function, its expressions and variable declarations in the order they are written.
struct SCBody : priv::List<SCBody_I, join<SCAnyExpr_I, SCVariableDeclare_I, SCPackDeclare_I>> {
    static constexpr auto type_name { "SCBody" };
};

struct SCExplicitReturn : priv::Node<SCExplicitReturn_I(SCAnyExpr_I value)> {
    static constexpr auto type_name { "SCExplicitReturn" };
};

struct SCCollection : priv::List<SCCollection_I, SCAnyExpr_I> {
    static constexpr auto type_name { "SCCollection" };
    OptionallyLocatable<SymSCClassName> collection_class;
    std::uint32_t size {};
    SCCollection(OptionallyLocatable<SymSCClassName> c): collection_class(c) {}
};


// `(first..last)` or `(first, second..last)`, a missing second means a step of one or minus one.
// A missing last means the range is unbounded (to inf), which is only allowed where the range is iterated rather than
// created, e.g., in a generator.
// This isn't a message, so later passes can choose how to lower it, e.g., iterating without creating the array.
struct SCRange : priv::Node<SCRange_I(SCAnyExpr_I first, maybe<SCAnyExpr_I> second, maybe<SCAnyExpr_I> last)> {
    static constexpr auto type_name { "SCRange" };
};

// One name in a pack assignment or declaration, `#a, b ...c = value`. It is assigned value.at(index), or
// value.copyToEnd(index) when it is the rest, `...c`. The node's location is the name.
struct SCPackTarget : priv::Terminal<SCPackTarget_I> {
    static constexpr auto type_name { "SCPackTarget" };
    Locatable<SymSCIdentifier> name;
    SCAssign::Kind kind;
    std::uint32_t index;
    bool rest;
    SCPackTarget(Locatable<SymSCIdentifier> n, SCAssign::Kind k, std::uint32_t i, bool r):
        name(n),
        kind(k),
        index(i),
        rest(r) {}
};

struct SCPackTargets : priv::List<SCPackTargetList_I, SCPackTarget_I> {
    static constexpr auto type_name { "SCPackTargets" };
};

// `#a, ~b ...c = value`, evaluates value once and assigns its parts to the targets, see SCPackTarget. Returns value.
// This isn't lowered to messages, as that needs a temporary to hold value, later passes can choose how to lower it.
struct SCPackAssign : priv::Node<SCPackAssign_I(SCAnyExpr_I value, SCPackTargetList_I targets)> {
    static constexpr auto type_name { "SCPackAssign" };
};

// `var #a, b ...c = value`, declares each target as a local variable, then assigns it like SCPackAssign.
// Every target's kind is Local. Like SCVariableDeclare, this isn't an expression.
struct SCPackDeclare : priv::Node<SCPackDeclare_I(SCAnyExpr_I value, SCPackTargetList_I targets)> {
    static constexpr auto type_name { "SCPackDeclare" };
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
                                        SCArgumentDeclareList_I variadic_arguments, SCBody_I body)> {
    static constexpr auto type_name { "SCMethod" };
    std::optional<Locatable<SymSCPrimitive>> primitive;
    bool poisoned { false };
    // What calling the method returns, when it is known and has no effects. Only set without arguments, as their values
    // aren't known until the method is called. Callers can't rely on it unless they know which method is dispatched.
    OptionalConstExprIndex constant_result {};
    SCMethod(std::optional<Locatable<SymSCPrimitive>> p): primitive(p) {}
};
struct SCFunction : priv::Node<SCFunction_I(SCArgumentDeclareList_I positional_arguments,
                                            SCArgumentDeclareList_I variadic_arguments, SCBody_I body)> {
    static constexpr auto type_name { "SCFunction" };
    // What calling the function returns, when it is known and has no effects, e.g., `{ 1 }`. The function itself isn't
    // constant. Only set without arguments, as their values aren't known until the function is called.
    OptionalConstExprIndex constant_result {};
};

struct SCMethodList : priv::List<SCMethodList_I, SCMethod_I> {
    static constexpr auto type_name { "SCMethodList" };
};

struct SCMember : priv::Node<SCMember_I()> {
    static constexpr auto type_name { "SCMember" };

    // The node's location is the member's name.
    SCMember(SymSCIdentifier name, SymSCClassName owning_class): name(name), owning_class(owning_class) {}

    SymSCIdentifier name;
    SymSCClassName owning_class;
    OptionalConstExprIndex expr {};
    TypeInfo expr_type_info {};

    ast::ReadWriteAccessor accessor {};
    bool is_const {};
};

struct SCMemberList : priv::List<SCMemberList_I, SCMember_I> {
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