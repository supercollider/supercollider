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
    OptionalLocalIndex local;
    OptionalMemberIndex member;
    SCRead(SymSCIdentifier n, Kind k, OptionalLocalIndex l = {}, OptionalMemberIndex m = {}):
        name(n),
        kind(k),
        local(l),
        member(m) {}
};

// `this`, `super`, and the pseudo variables, e.g., `thisProcess`. These aren't variables: the language sets them,
// nothing declares them, and they can't be assigned.
struct SCSpecial : priv::Terminal<SCSpecial_I> {
    static constexpr auto type_name { "SCSpecial" };
    enum struct Kind {
        This,
        // `this`, but a message sent to it is looked up from the superclass of the class the method is defined in.
        Super,
        Process,
        Thread,
        Method,
        FunctionDef,
        Function,
    };
    Kind kind;
    SCSpecial(Kind k): kind(k) {}
};

struct SCAssign : priv::Node<SCAssign_I(SCAnyExpr_I)> {
    static constexpr auto type_name { "SCAssign" };
    enum struct Kind { Local, Instance, Class, Environment };
    Locatable<SymSCIdentifier> name;
    Kind kind;
    OptionalLocalIndex local;
    OptionalMemberIndex member;
    SCAssign(Locatable<SymSCIdentifier> n, Kind k, OptionalLocalIndex l = {}, OptionalMemberIndex m = {}):
        name(n),
        kind(k),
        local(l),
        member(m) {}
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

// `var a = 1`, value is missing for `var a`, which is nil.
struct SCVariableDeclare : priv::Node<SCVariableDeclare_I(maybe<SCAnyExpr_I> value)> {
    static constexpr auto type_name { "SCVariableDeclare" };
    Locatable<SymSCIdentifier> name;
    LocalIndex local;
    SCVariableDeclare(Locatable<SymSCIdentifier> n, LocalIndex l): name(n), local(l) {}
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
    // Whether it declares a local in SCPackDeclare or assigns it.
    bool declares { false };
    OptionalLocalIndex local;
    OptionalMemberIndex member;
    SCPackTarget(Locatable<SymSCIdentifier> n, SCAssign::Kind k, std::uint32_t i, bool r, OptionalLocalIndex l = {},
                 OptionalMemberIndex m = {}):
        name(n),
        kind(k),
        index(i),
        rest(r),
        local(l),
        member(m) {}
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

////////////////////////////////////////////////////////////////////////////////
// Generators, `{: value, qualifiers }`. Each qualifier nests inside the previous one, as blocks did in the old
// compiler, but they are kept in a flat list in the order they are written. Lowering them, e.g., into a Routine, can
// rebuild the nesting.
// Each qualifier is a block. The variables it declares are a new binding EACH time it iterates.
////////////////////////////////////////////////////////////////////////////////

// `x <- source` or `x i <- source`, iterates source with do (forSeries for a range), declaring the element, and the
// index when given, each a new binding per element. source can be an unbounded range, e.g., `x <- (1..)`.
struct SCGenQualIterate : priv::Node<SCGenQualIterate_I(SCAnyExpr_I source)> {
    static constexpr auto type_name { "SCGenQualIterate" };
    Locatable<SymSCIdentifier> element;
    LocalIndex element_local;

    std::optional<Locatable<SymSCIdentifier>> index;
    OptionalLocalIndex index_local;

    SCGenQualIterate(Locatable<SymSCIdentifier> e, std::optional<Locatable<SymSCIdentifier>> i, LocalIndex el,
                     OptionalLocalIndex il):
        element(e),
        index(i),
        element_local(el),
        index_local(il) {}
};

// `var x = value`, declares x for the qualifiers after it, a new binding each time it runs.
struct SCGenQualDeclare : priv::Node<SCGenQualDeclare_I(SCAnyExpr_I value)> {
    static constexpr auto type_name { "SCGenQualDeclare" };
    Locatable<SymSCIdentifier> name;
    LocalIndex local;
    SCGenQualDeclare(Locatable<SymSCIdentifier> n, LocalIndex l): name(n), local(l) {}
};

// `condition`, the qualifiers after it only run when it is true.
struct SCGenQualIf : priv::Node<SCGenQualIf_I(SCAnyExpr_I condition)> {
    static constexpr auto type_name { "SCGenQualIf" };
};

// `:: expr`, evaluated for its effects, then the qualifiers after it run.
struct SCGenQualSideEffect : priv::Node<SCGenQualSideEffect_I(SCAnyExpr_I expr)> {
    static constexpr auto type_name { "SCGenQualSideEffect" };
};

// `:while condition`, ends the whole generator when it is false.
struct SCGenQualWhile : priv::Node<SCGenQualWhile_I(SCAnyExpr_I condition)> {
    static constexpr auto type_name { "SCGenQualWhile" };
};

struct SCGenQualList
    : priv::List<SCGenQualList_I,
                 join<SCGenQualIterate_I, SCGenQualDeclare_I, SCGenQualIf_I, SCGenQualSideEffect_I, SCGenQualWhile_I>> {
    static constexpr auto type_name { "SCGenQualList" };
};

// `{: value, qualifiers }` makes a Routine, which yields value each time the qualifiers reach it.
// `{; value, qualifiers }` is Immediate, the qualifiers run straight away, evaluating value for its effects. It isn't a
// Routine, and returns what the outermost qualifier does, e.g., the source of `x <- source`.
// value is compiled after the qualifiers, so it can use what they declare.
struct SCGenerator : priv::Node<SCGenerator_I(SCGenQualList_I qualifiers, SCAnyExpr_I value)> {
    static constexpr auto type_name { "SCGenerator" };
    enum struct Kind { Routine, Immediate };
    Kind kind;
    SCGenerator(Kind k): kind(k) {}
};

// `(: first, second..last)`, a Routine yielding the series, the old compiler's `first.seriesIter(second, last)`.
// Like SCRange, but a missing last is allowed, which means it never ends.
struct SCRangeGenerator
    : priv::Node<SCRangeGenerator_I(SCAnyExpr_I first, maybe<SCAnyExpr_I> second, maybe<SCAnyExpr_I> last)> {
    static constexpr auto type_name { "SCRangeGenerator" };
};

enum struct DefaultBehaviour { NoDefault, PreserveNil, DontPreserveNil };
struct SCArgumentDeclare : priv::Node<SCArgumentDeclare_I(maybe<SCAnyExpr_I>)> {
    static constexpr auto type_name { "SCArgumentDeclare" };
    SymSCIdentifier name;
    LocalIndex local;
    DefaultBehaviour default_behaviour;
    SCArgumentDeclare(SymSCIdentifier n, LocalIndex l, DefaultBehaviour db = DefaultBehaviour::NoDefault):
        name(n),
        local(l),
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
    // Can differ from its class's, when it is in an extension.
    ast::LanguageVersion version;
    // What calling the method returns, when it is known and has no effects. Only set without arguments, as their values
    // aren't known until the method is called. Callers can't rely on it unless they know which method is dispatched.
    OptionalConstExprIndex constant_result {};
    SCMethod(std::optional<Locatable<SymSCPrimitive>> p, ast::LanguageVersion v): primitive(p), version(v) {}
};
struct SCFunction : priv::Node<SCFunction_I(SCArgumentDeclareList_I positional_arguments,
                                            SCArgumentDeclareList_I variadic_arguments, SCBody_I body)> {
    static constexpr auto type_name { "SCFunction" };
    // What calling the function returns, when it is known and has no effects, e.g., `{ 1 }`. The function itself isn't
    // constant. Only set without arguments, as their values aren't known until the function is called.
    OptionalConstExprIndex constant_result {};
};
// One item of a region, e.g., `a = 1` or `( var b = 2; b )`, compiled into its own function, so each can be evaluated
// on its own.
struct SCRegion : priv::Node<SCRegion_I(SCFunction_I function)> {
    static constexpr auto type_name { "SCRegion" };
};
// The items of a region, e.g., `a = 1; ( var b = 2; b )`, evaluated in turn.
struct SCRegionList : priv::List<SCRegionList_I, SCRegion_I> {
    static constexpr auto type_name { "SCRegionList" };
    // What evaluating the regions returns, the last one's, when it is known.
    OptionalConstExprIndex constant_result {};
};
// Code that isn't in the class library, e.g., a line evaluated in the IDE or an scd file, compiled as if it were
// written inside `Interpreter:functionCompileContext`, so it can use the Interpreter's members. A file as a function,
// e.g., `|x| x + 1`, is a function, otherwise it is a list of regions.
struct SCInterpreted : priv::Node<SCInterpreted_I(join<SCFunction_I, SCRegionList_I> code)> {
    static constexpr auto type_name { "SCInterpreted" };
    bool poisoned { false };
    // Given when it is compiled, see IRCompiler::compile_interpreted.
    ast::LanguageVersion version;
    SCInterpreted(ast::LanguageVersion v): version(v) {}
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
    // How an instance's indexed slots, its elements, are stored, set by the class's slot type, e.g., `String[char]`.
    // A class without one, NotIndexed, only has its instance variables.
    enum struct MemoryLayout { NotIndexed, Slot, Double, Float, Int32, Int16, Int8, Char, Symbol };
    SymSCClassName name;
    MemoryLayout memory_layout { MemoryLayout::NotIndexed };
    std::optional<SymSCClassName> meta_class {};
    // Of its own declaration, its extension methods have their own, see SCMethod::version.
    ast::LanguageVersion version { ast::LanguageVersion::Legacy };
    SCClass(SymSCClassName n): name(n) {}
};


struct SCExprList : priv::List<TypedNodeIndex<SCNodeType::ExprList>, SCAnyExpr_I> {
    static constexpr auto type_name { "SCExprList" };
};


}