// Copyright Jordan Henderson 2026
#pragma once

#include "sc_util/type_set_index.hpp"
#include <cstdint>
namespace sc::ast {

// Elements the indexs can represent.
// The types of node in the graph.
enum struct NodeFlag {
    // Misc
    Missing,
    Error,

    // parts of lits
    StringLineLiteral,

    // lits
    IntegerLiteral,
    FloatLiteral,
    PiLiteral,
    CurryLiteral,
    ASCIILiteral,
    AccidentalLiteral,
    StringLiteral,

    ArrayLiteral,
    SymbolLiteral,
    SelectorLiteral,
    SelectorWAdverb,
    BooleanLiteral,
    NilLiteral,
    BlockLiteral,
    BlockList,
    Array,
    Dictionary,
    DictionaryEntry,
    Collection,

    ImmediatelyEvaluatedBlock,

    //
    RWAccessor,

    // identifiers
    ClassNameIdentifier,
    EnvIdentifier,
    PrimitiveNameIdentifier,
    NameIdentifier, // args, vars, consts (anything lowercase)

    //
    NameList,

    // argument calling
    KwArg,
    VariadicArgument,

    // when calling argument
    ArgumentList,

    // This is the heart of the expr.
    ExprSeq,

    // expr stuff

    PrefixMessage,
    PostfixMessage,
    BinaryMessage,
    ValueMessage,
    NewMessage,
    AtMessage,


    AdverbExpr,
    Reference,
    Assignment,
    AssignmentAt,
    Setter,
    NonLocalReturnExpr,

    Generator,
    GeneratorFake,
    GeneratorQualList,
    GeneratorQualAssign,
    GeneratorQualDeclare,
    GeneratorQualIf,
    GeneratorQualSideEffect,
    GeneratorQualGuard,

    // defining functions/methods
    DeclareArgumentVariadic,
    DeclareArgumentWithDefault,
    DeclareArgumentList,
    //
    DeclareVariableWithDefault,
    VariablePack,

    DeclareVariableList,
    //
    BlockContentsList, // stuff that goes in a function.

    // methods
    Method,
    ClassMethod,
    MethodContentsList,
    MethodList,

    // class instances variables
    DeclareMember, // instance or member
    DeclareConst,

    DeclareMemberList,
    DeclareClassMemberList,
    DeclareConstList,


    BigMemberList,


    //
    Class,
    ClassExtension,
    //
    ClassOrExtensionList,

    RegionList,

    COUNT
};


namespace details {
using Spec = sc::util::typed_index::Spec<std::uint32_t, NodeFlag, struct ASTGraph>;
using Def = sc::util::typed_index::QuicklyDefineTypesFromSpec<Spec>;
}

using Index = details::Def::Index;
using OptionalIndex = details::Def::OptionalIndex;

// Because bison requires that the indexs be default constructable, we must use the optional index type here.
template <NodeFlag... Flags> using TypedIndex = details::Def::OptionalTypedIndex<Flags...>;

template <typename... Is> using join = sc::util::typed_index::join<Is...>;

template <typename T> using maybe = join<T, TypedIndex<NodeFlag::Missing>>;

template <typename T> using error_index = join<T, TypedIndex<NodeFlag::Error>>;


using MissingIndex = TypedIndex<NodeFlag::Missing>;
using ErrorIndex = TypedIndex<NodeFlag::Error>;

using StringLineLitIndex = TypedIndex<NodeFlag::StringLineLiteral>;

// Literal indexes
using IntLitIndex = TypedIndex<NodeFlag::IntegerLiteral>;
using FloatLitIndex = TypedIndex<NodeFlag::FloatLiteral>;

// A literal number, int or float
using NumberIndex = join<IntLitIndex, FloatLitIndex>;

using PiLitIndex = TypedIndex<NodeFlag::PiLiteral>;
using CurryIndex = TypedIndex<NodeFlag::CurryLiteral>;
using ASCIIIndex = TypedIndex<NodeFlag::ASCIILiteral>;
using AccidentalIndex = TypedIndex<NodeFlag::AccidentalLiteral>;

using FloatProducingIndex = join<FloatLitIndex, PiLitIndex, AccidentalIndex>;

using StringLitIndex = TypedIndex<NodeFlag::StringLiteral>;

using SymbolLitIndex = TypedIndex<NodeFlag::SymbolLiteral>;

using BooleanLitIndex = TypedIndex<NodeFlag::BooleanLiteral>;
using NilLitIndex = TypedIndex<NodeFlag::NilLiteral>;

using ArrayIndex = TypedIndex<NodeFlag::Array>;
using DictionaryEntryIndex = TypedIndex<NodeFlag::DictionaryEntry>;
using DictionaryIndex = TypedIndex<NodeFlag::Dictionary>;
using CollectionIndex = TypedIndex<NodeFlag::Collection>;

using BlockIndex = TypedIndex<NodeFlag::BlockLiteral>;
using BlockListIndex = TypedIndex<NodeFlag::BlockList>;

using ImmediatelyEvaluatedBlockIndex = TypedIndex<NodeFlag::ImmediatelyEvaluatedBlock>;
using AnyDefiniteLiteralIndex =
    join<ASCIIIndex, IntLitIndex, FloatProducingIndex, StringLitIndex, BooleanLitIndex, NilLitIndex, SymbolLitIndex>;

using AnyPossiblyLiteralIndex = join<AnyDefiniteLiteralIndex, BlockIndex, ArrayIndex, DictionaryIndex, CollectionIndex>;

using ClassNameIdentifierIndex = TypedIndex<NodeFlag::ClassNameIdentifier>;
using NamedIdentifierIndex = TypedIndex<NodeFlag::NameIdentifier>;
using EnvIdentifierIndex = TypedIndex<NodeFlag::EnvIdentifier>;
using PrimitiveIdentifierIndex = TypedIndex<NodeFlag::PrimitiveNameIdentifier>;

using NameOrEnvNameIndex = join<NamedIdentifierIndex, EnvIdentifierIndex>;

using NameListIndex = TypedIndex<NodeFlag::NameList>;

// TODO: remove SelectorLiteral
using SelectorIndex = join<TypedIndex<NodeFlag::SelectorLiteral>, NamedIdentifierIndex>;
using SelectorWAdverbIndex = TypedIndex<NodeFlag::SelectorWAdverb>;

using SelectorMaybeAdverbIndex = join<SelectorIndex, SelectorWAdverbIndex>;


using ReferenceIndex = TypedIndex<NodeFlag::Reference>;
using AssignmentIndex = TypedIndex<NodeFlag::Assignment>;
using AssignmentAtIndex = TypedIndex<NodeFlag::AssignmentAt>;
using SetterIndex = TypedIndex<NodeFlag::Setter>;

using NonLocalReturnExprIndex = TypedIndex<NodeFlag::NonLocalReturnExpr>;

// This is a little confusing, but an ExprSeqIndex can be a list of expression, or just a single expr.
// We use ExprSeqIndex rather than create an ExprSeq because from the compiler's point, they are the same, and we get to
//      save an extra node.
using ExprSeqIndex = TypedIndex<NodeFlag::ExprSeq>;


using GeneratorIndex = TypedIndex<NodeFlag::Generator>;
using GeneratorFakeIndex = TypedIndex<NodeFlag::GeneratorFake>;

using GeneratorAnyIndex = join<GeneratorIndex, GeneratorFakeIndex>;

using GeneratorQualAssignIndex = TypedIndex<NodeFlag::GeneratorQualAssign>;
using GeneratorQualDeclareIndex = TypedIndex<NodeFlag::GeneratorQualDeclare>;
using GeneratorQualIfIndex = TypedIndex<NodeFlag::GeneratorQualIf>;
using GeneratorQualSideEffectIndex = TypedIndex<NodeFlag::GeneratorQualSideEffect>;
using GeneratorQualGuardIndex = TypedIndex<NodeFlag::GeneratorQualGuard>;

using GeneratorQualAnyIndex = join< //
    GeneratorQualAssignIndex, //
    GeneratorQualDeclareIndex, //
    GeneratorQualIfIndex, //
    GeneratorQualSideEffectIndex, //
    GeneratorQualGuardIndex //
    >;

using GeneratorQualListIndex = TypedIndex<NodeFlag::GeneratorQualList>;


using PrefixMessageIndex = TypedIndex<NodeFlag::PrefixMessage>;
using PostfixMessageIndex = TypedIndex<NodeFlag::PostfixMessage>;
using BinaryMessageIndex = TypedIndex<NodeFlag::BinaryMessage>;
using ValueMessageIndex = TypedIndex<NodeFlag::ValueMessage>;
using NewMessageIndex = TypedIndex<NodeFlag::NewMessage>;
using AtMessageIndex = TypedIndex<NodeFlag::AtMessage>;

using AnyMessageIndex = join< //
    PrefixMessageIndex, //
    PostfixMessageIndex, //
    BinaryMessageIndex, //
    ValueMessageIndex, //
    NewMessageIndex, //
    AtMessageIndex //
    >;


using VariablePackIndex = TypedIndex<NodeFlag::VariablePack>;
using AnyExprIndex = join< //
    ExprSeqIndex, //
    AnyPossiblyLiteralIndex, //
    ClassNameIdentifierIndex, //
    EnvIdentifierIndex, //
    NamedIdentifierIndex, //
    AnyMessageIndex, //
    NonLocalReturnExprIndex, //
    ReferenceIndex, //
    AssignmentIndex, //
    AssignmentAtIndex, //
    SetterIndex, //
    VariablePackIndex, //
    GeneratorAnyIndex, //
    ImmediatelyEvaluatedBlockIndex //
    >;


using AdverbExprIndex = TypedIndex<NodeFlag::AdverbExpr>;
using AdverbIndex = TypedIndex<NodeFlag::AdverbExpr, NodeFlag::IntegerLiteral, NodeFlag::NameIdentifier>;

using KwArgIndex = TypedIndex<NodeFlag::KwArg>;
using VariadicArgIndex = TypedIndex<NodeFlag::VariadicArgument>;

using ArgumentEntryIndex = join<AnyExprIndex, KwArgIndex, VariadicArgIndex, BlockListIndex>;

using ArgumentListIndex = TypedIndex<NodeFlag::ArgumentList>;

// Declare arguments

using DeclareArgumentVariadicIndex = TypedIndex<NodeFlag::DeclareArgumentVariadic>;
using DeclareArgumentWithDefaultIndex = TypedIndex<NodeFlag::DeclareArgumentWithDefault>;

using DeclareAnyArgumentIndex =
    join<NamedIdentifierIndex, DeclareArgumentWithDefaultIndex, DeclareArgumentVariadicIndex>;

using DeclareArgumentListIndex = TypedIndex<NodeFlag::DeclareArgumentList>;

// Declare variables


using DeclareVariableWithDefaultIndex = TypedIndex<NodeFlag::DeclareVariableWithDefault>;

using DeclareAnyVariableIndex = join<NamedIdentifierIndex, DeclareVariableWithDefaultIndex, VariablePackIndex>;

using DeclareVariableListIndex = TypedIndex<NodeFlag::DeclareVariableList>;

using BlockItemIndex = join<AnyExprIndex, DeclareVariableListIndex>;
using BlockContentsListIndex = TypedIndex<NodeFlag::BlockContentsList>;

using MethodContentsListIndex = TypedIndex<NodeFlag::MethodContentsList>;
using MethodIndex = TypedIndex<NodeFlag::Method>;
using ClassMethodIndex = TypedIndex<NodeFlag::ClassMethod>;
using AnyMethodIndex = join<MethodIndex, ClassMethodIndex, TypedIndex<NodeFlag::Error>>;

using MethodListIndex = TypedIndex<NodeFlag::MethodList>;

using DeclareMemberIndex = TypedIndex<NodeFlag::DeclareMember>;
using DeclareConstMemberIndex = TypedIndex<NodeFlag::DeclareConst>;

using RWAccessorIndex = TypedIndex<NodeFlag::RWAccessor>;

using DeclareMemberListIndex = TypedIndex<NodeFlag::DeclareMemberList>;
using DeclareClassMemberListIndex = TypedIndex<NodeFlag::DeclareClassMemberList>;
using DeclareConstListIndex = TypedIndex<NodeFlag::DeclareConstList>;

using DeclareAnyMemberListIndex = join<DeclareMemberListIndex, DeclareClassMemberListIndex, DeclareConstListIndex>;

using BigMemberListIndex = TypedIndex<NodeFlag::BigMemberList>;

using ClassIndex = TypedIndex<NodeFlag::Class>;
using ClassExtensionIndex = TypedIndex<NodeFlag::ClassExtension>;
using ClassOrExtensionIndex = join<ClassIndex, ClassExtensionIndex>;

using ClassOrExtensionListIndex = TypedIndex<NodeFlag::ClassOrExtensionList>;

using RegionListIndex = TypedIndex<NodeFlag::RegionList>;
using ClassListOrExprListIndex = join<RegionListIndex, ExprSeqIndex, ClassOrExtensionListIndex>;

template <typename T> struct Wrapper {
    using Type = T;
};

template <std::size_t... I> auto any_index_builder(std::index_sequence<I...>) {
    using Type = TypedIndex<static_cast<NodeFlag>(I)...>;
    return Wrapper<Type> { };
};

// Accepts any index, without resorting to using Index or OptionalIndex directly.
using AnyIndex = typename decltype(any_index_builder(std::index_sequence<static_cast<int>(NodeFlag::COUNT)> { }))::Type;
}
