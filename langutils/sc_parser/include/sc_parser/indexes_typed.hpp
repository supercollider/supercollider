// Copyright Jordan Henderson 2026
#pragma once

#include "sc_util/type_set_index.hpp"
#include "sc_parser/indexes_untyped.hpp"


namespace sc::ast {

using UntypedIndex = OptionalIndex;

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


using KeyBinOpIndex = TypedIndex<NodeFlag::KeybinOp>;
using SymbolQuoteIndex = TypedIndex<NodeFlag::SymbolQuote>;
using SymbolSlashIndex = TypedIndex<NodeFlag::SymbolSlash>;

using AnySymbolIndex = join<SymbolQuoteIndex, SymbolSlashIndex>;

using BooleanLitIndex = TypedIndex<NodeFlag::BooleanLiteral>;
using NilLitIndex = TypedIndex<NodeFlag::NilLiteral>;

using RangeIndex = TypedIndex<NodeFlag::Range>;
using RangeGeneratorIndex = TypedIndex<NodeFlag::RangeGenerator>;
using RangeAssignIndex = TypedIndex<NodeFlag::RangeAssign>;

using ArrayIndex = TypedIndex<NodeFlag::Array>;
using DictionaryEntryIndex = TypedIndex<NodeFlag::DictionaryEntry>;
using DictionaryIndex = TypedIndex<NodeFlag::Dictionary>;
using CollectionIndex = TypedIndex<NodeFlag::Collection>;

using BlockIndex = TypedIndex<NodeFlag::BlockLiteral>;
using BlockListIndex = TypedIndex<NodeFlag::BlockList>;

using ImmediatelyEvaluatedBlockIndex = TypedIndex<NodeFlag::ImmediatelyEvaluatedBlock>;
using AnyDefiniteLiteralIndex =
    join<ASCIIIndex, IntLitIndex, FloatProducingIndex, StringLitIndex, BooleanLitIndex, NilLitIndex, AnySymbolIndex>;

using AnyPossiblyLiteralIndex = join<AnyDefiniteLiteralIndex, BlockIndex, ArrayIndex, DictionaryIndex, CollectionIndex>;

using ClassNameIdentifierIndex = TypedIndex<NodeFlag::ClassNameIdentifier>;

// Expressions whose value is always known at compile time. Class names aren't literals, but are constants.
using AnyDefiniteConstantIndex = join<AnyDefiniteLiteralIndex, ClassNameIdentifierIndex>;

using NamedIdentifierIndex = TypedIndex<NodeFlag::NameIdentifier>;
using EnvIdentifierIndex = TypedIndex<NodeFlag::EnvIdentifier>;
using PrimitiveIdentifierIndex = TypedIndex<NodeFlag::PrimitiveNameIdentifier>;

using NameOrEnvNameIndex = join<NamedIdentifierIndex, EnvIdentifierIndex>;

using NameListIndex = TypedIndex<NodeFlag::NameList>;
using NamePackIndex = TypedIndex<NodeFlag::NamePack>;

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


using VariablePackAssignIndex = TypedIndex<NodeFlag::VariablePackAssign>;
// Includes errors, as the parser can recover from an error in an expression and carry on, e.g., `(1 +)`.
using AnyExprIndex = join< //
    ErrorIndex, //
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
    VariablePackAssignIndex, //
    GeneratorAnyIndex, //
    RangeIndex, //
    RangeGeneratorIndex, //
    RangeAssignIndex, //
    CurryIndex //
    >;

static_assert(!std::is_convertible_v<KeyBinOpIndex, AnyExprIndex>);

using AdverbExprIndex = TypedIndex<NodeFlag::AdverbExpr>;
using AdverbIndex = TypedIndex<NodeFlag::AdverbExpr, NodeFlag::IntegerLiteral, NodeFlag::NameIdentifier>;

using KwArgIndex = TypedIndex<NodeFlag::KwArg>;
using VariadicArgIndex = TypedIndex<NodeFlag::VariadicArgument>;

using ArgumentEntryIndex = join<AnyExprIndex, KwArgIndex, VariadicArgIndex, BlockListIndex>;

using ArgumentListIndex = TypedIndex<NodeFlag::ArgumentList>;


using AtMessageArgIndex = join<RangeIndex, ArgumentListIndex>;

// Declare arguments

using DeclareArgumentVariadicIndex = TypedIndex<NodeFlag::DeclareArgumentVariadic>;
using DeclareArgumentWithDefaultIndex = TypedIndex<NodeFlag::DeclareArgumentWithDefault>;

using DeclareAnyArgumentIndex =
    join<NamedIdentifierIndex, DeclareArgumentWithDefaultIndex, DeclareArgumentVariadicIndex>;

using DeclareArgumentListIndex = TypedIndex<NodeFlag::DeclareArgumentList>;

// Declare variables


using DeclareVariableWithDefaultIndex = TypedIndex<NodeFlag::DeclareVariableWithDefault>;

using DeclareAnyVariableIndex = join<NamedIdentifierIndex, DeclareVariableWithDefaultIndex, VariablePackAssignIndex>;

using DeclareVariableListIndex = TypedIndex<NodeFlag::DeclareVariableList>;

using BlockItemIndex = join<AnyExprIndex, DeclareVariableListIndex>;
using BlockContentsListIndex = TypedIndex<NodeFlag::BlockContentsList>;

using MethodContentsListIndex = TypedIndex<NodeFlag::MethodContentsList>;
using MethodIndex = TypedIndex<NodeFlag::Method>;
using ClassMethodIndex = TypedIndex<NodeFlag::ClassMethod>;
using AnyMethodIndex = join<MethodIndex, ClassMethodIndex, TypedIndex<NodeFlag::Error>>;
using AnyValidMethodIndex = join<MethodIndex, ClassMethodIndex>;

using MethodListIndex = TypedIndex<NodeFlag::MethodList>;

using DeclareMemberIndex = TypedIndex<NodeFlag::DeclareMember>;
using DeclareConstMemberIndex = TypedIndex<NodeFlag::DeclareConst>;

using RWAccessorIndex = TypedIndex<NodeFlag::RWAccessor>;
using MaybeRWAccessorIndex = maybe<RWAccessorIndex>;

using DeclareMemberListIndex = TypedIndex<NodeFlag::DeclareMemberList>;
using DeclareClassMemberListIndex = TypedIndex<NodeFlag::DeclareClassMemberList>;
using DeclareConstListIndex = TypedIndex<NodeFlag::DeclareConstList>;

using DeclareAnyMemberListIndex = join<DeclareMemberListIndex, DeclareClassMemberListIndex, DeclareConstListIndex>;

using BigMemberListIndex = TypedIndex<NodeFlag::BigMemberList>;

using ClassIndex = TypedIndex<NodeFlag::Class>;
using ClassExtensionIndex = TypedIndex<NodeFlag::ClassExtension>;
using ClassOrExtensionIndex = join<ClassIndex, ClassExtensionIndex>;

using ClassOrExtensionListIndex = TypedIndex<NodeFlag::ClassOrExtensionList>;

using FileAsFunctionIndex = TypedIndex<NodeFlag::FileAsFunction>;

using RegionItemIndex = join<ErrorIndex, AnyExprIndex, ImmediatelyEvaluatedBlockIndex>;
using RegionListIndex = TypedIndex<NodeFlag::RegionList>;

using GoIndex = join<RegionListIndex, ClassOrExtensionListIndex, FileAsFunctionIndex>;

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
