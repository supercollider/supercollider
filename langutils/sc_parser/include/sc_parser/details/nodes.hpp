// Copyright Jordan Henderson 2026
#pragma once

#include "indexes_typed.hpp"
#include "read_write_accessor.hpp"
#include "sc_util/language_version.hpp"

#include "sc_util/details/typed_graph_node.hpp"

#include <cstdint>


/*

This file should be read alongside the indexes_typed.hpp

The typed indexs define conversions. This allows integers to become expression, for example.

This file defines extra information ir nodes need, and what children they can have.
a

For example, say we have a basic language that has integers, float, expressions (nothing else), and functions which are
just a list of expressions.

We would then have the following typed indexes:

    using IntIndex = TypedIndex<IRType::IntLiteral>;
    using FloatIndex = TypedIndex<IRType::FloatLiteral>;
    using FunctionIndex = TypedIndex<IRType::FunctionLiteral>;

    // Expressions can either be ints, floats, or functions.
    using ExpressionIndex = join<IntIndex, FloatIndex, FunctionIndex>;

    using ExpressionListIndex = TypedIndex<IRType::ExpressionList>;
&,
Then the nodes would look like this:

    struct IntLiteral : public priv::TerminalNode<IntLiteral, IntIndex> {};
    struct FloatLiteral : public priv::TerminalNode<FloatLiteral, FloatIndex> {};

    struct FunctionLiteral : public priv::Node<
        FunctionIndex(ExpressionListIndex body),  <<<<<<< 1.
        FunctionIndex
    > {};

    struct ExpressionList : public priv::ListNode<
        ExpressionIndex,  <<<<<2
        ExpressionListIndex,
    > {};


    //      1. this line defines that the constructor of a function literal node requires an expression list be passed.
This is a function signature type.

    //      2. this line defines that the expression list can have any number of expressions appended to it.

Note how there is no IR node the corresponds to an expression. This is because ints, floats, and functions can all be
valid expressions in their own right.

If IR nodes need extra information this can be done by simply create data in the struct.

Was the struct is defined, you MUST add it to the template at the end of this file.
This way, the structure will automatically be added to the variant in the ir graph.

The reason why all this complexity is desirable, is because when it comes to compile the ir graph, we know exactly what
all the children are. Although the grammar/parser may actually produce a more restricted graph. By compiling a more
general graph, we end up being able to change the grammar without worrying about how to compile it, so long as we don't
change the definitions below.

*/

namespace sc::ast {
template <typename Index> using TerminalNode = sc::util::typed_graph::TerminalNode<Index>;

template <typename Index, typename ChildIndex> using ListNode = sc::util::typed_graph::ListNode<Index, ChildIndex>;

template <class ThisTypedIndex, typename... CHILD_INDEXES>
using Node = sc::util::typed_graph::Node<ThisTypedIndex, CHILD_INDEXES...>;


/// @brief Marks an optional child that was not written.
struct Missing : public TerminalNode<MissingIndex> {
    static constexpr auto name { "Missing" };
};

struct ExprSeq : ListNode<ExprSeqIndex, AnyExprIndex> {
    static constexpr auto name { "ExprSeq" };
};

struct IntNode : public TerminalNode<IntLitIndex> {
    static constexpr auto name { "IntNode" };
    enum struct Kind : std::uint8_t { Normal, Radix, Hexadecimal } kind;
    enum struct Sign { Positive, Negative } sign;
    constexpr IntNode(Kind i = Kind::Normal, Sign s = Sign::Positive): kind(i), sign(s) {}
};

struct FloatNode : public TerminalNode<FloatLitIndex> {
    static constexpr auto name { "FloatNode" };
    enum struct Kind : std::uint8_t { Normal, Radix, Exponent, Inf } kind;
    enum struct Sign { Positive, Negative } sign;
    constexpr FloatNode(Kind i = Kind::Normal, Sign s = Sign::Positive): kind(i), sign(s) {}
};

/// @brief The constant `pi`, with an optional multiplier, e.g., `2pi`.
struct PiNode : public Node<PiLitIndex(maybe<NumberIndex> multiplier)> {
    static constexpr auto name { "PiNode" };
    enum struct Sign { Positive, Negative } sign; // if the multiplier is signed, this is always positive.
    constexpr PiNode(Sign s = Sign::Positive): sign(s) {}
};

struct AccidentalNode : public TerminalNode<AccidentalIndex> {
    static constexpr auto name { "AccidentalNode" };
    enum struct Kind { Steps, Cents } kind;
    enum struct Sign { Positive, Negative } sign;
    constexpr AccidentalNode(Kind k, Sign s = Sign::Positive): kind(k), sign(s) {}
};

/// @brief One line of a string literal.
struct StringLineNode : public TerminalNode<StringLineLitIndex> {
    static constexpr auto name { "StringLineNode" };
};

/// The true string literal type, a list of its lines.
struct StringLineList : public ListNode<StringLitIndex, StringLineLitIndex> {
    static constexpr auto name { "StringLineList" };
};

/// @brief A keyword
struct KeyBinOp : public TerminalNode<KeyBinOpIndex> {
    static constexpr auto name { "KeyBinOp" };
};

/// @brief a keybinop and the value
struct KwArgNode : Node<KwArgIndex(KeyBinOpIndex keyword, AnyExprIndex value)> {
    static constexpr auto name { "KwArgNode" };
};

/// @brief A symbol written with a slash, e.g., `\symbol`.
struct SymbolSlash : public TerminalNode<SymbolSlashIndex> {
    static constexpr auto name { "SymbolSlash" };
};
/// @brief A symbol written in quotes, e.g., `'symbol'`.
struct SymbolQuote : public TerminalNode<SymbolQuoteIndex> {
    static constexpr auto name { "SymbolQuote" };
};

struct BooleanNode : public TerminalNode<BooleanLitIndex> {
    static constexpr auto name { "BooleanNode" };
    bool value;
    constexpr BooleanNode(bool v): value(v) {}
};

struct NilNode : public TerminalNode<NilLitIndex> {
    static constexpr auto name { "NilNode" };
};

struct ASCIINode : public TerminalNode<ASCIIIndex> {
    static constexpr auto name { "ASCIINode" };
};

struct CurryNode : public TerminalNode<CurryIndex> {
    static constexpr auto name { "CurryNode" };
};


struct ArrayNode : public ListNode<ArrayIndex, join<AnyExprIndex, KeyBinOpIndex>> {
    static constexpr auto name { "ArrayList" };
    // #[1, 2];
    // Previously, this would mean the compiler would create the array and you would just access it straight from the
    // constants slot. This is now not useful, because the new compiler can just look through all the elements of the
    // array and try to deduce them. Then, the new compiler, emits this into the constants slots, and COPIES THE OBJECT
    // to the stack when accessed. Hence the hash '#', means this is immutable, and you can therefore avoid a copy.
    bool is_immutable { false };
};

/// @brief A class name followed by an array, e.g., `Set[1, 2]`.
struct CollectionNode : public Node<CollectionIndex(ClassNameIdentifierIndex, ArrayIndex)> {
    static constexpr auto name { "CollectionNode" };
};

/// @brief One key and value pair of a dictionary.
struct DictionaryEntryNode
    : public Node<DictionaryEntryIndex(join<AnyExprIndex, KeyBinOpIndex> key, AnyExprIndex value)> {
    static constexpr auto name { "DictionaryEntryNode" };
};

struct DictionaryNode : public ListNode<DictionaryIndex, DictionaryEntryIndex> {
    static constexpr auto name { "DictionaryNode" };
};

/// @brief The functions and generators written after the arguments of a message.
struct BlockList : public ListNode<BlockListIndex, join<BlockIndex, GeneratorAnyIndex>> {
    static constexpr auto name { "BlockList" };
};

struct NamedIdentifier : TerminalNode<NamedIdentifierIndex> {
    static constexpr auto name { "NameNode" };
};

struct ClassNameIdentifier : TerminalNode<ClassNameIdentifierIndex> {
    static constexpr auto name { "ClassNameIdentitier" };
};

struct PrimitiveIdentifier : TerminalNode<PrimitiveIdentifierIndex> {
    static constexpr auto name { "PrimitiveIdentifier" };
};

struct EnvIdentifierNode : Node<EnvIdentifierIndex(NamedIdentifierIndex)> {
    static constexpr auto name { "EnvIdentifierNode" };
};


/// @brief A list of names or environment variables.
struct NameList : ListNode<NameListIndex, NameOrEnvNameIndex> {
    static constexpr auto name { "NameList" };
};

/// @brief Names unpacked from a value, with an optional name for the remaining elements.
struct NamePack : Node<NamePackIndex(NameListIndex, join<MissingIndex, NamedIdentifierIndex, EnvIdentifierIndex>)> {
    static constexpr auto name { "NamePack" };
};

struct SelectorNode : TerminalNode<TypedIndex<NodeFlag::SelectorLiteral>> {
    static constexpr auto name { "SelectorNode" };
    constexpr SelectorNode(bool infix = false) noexcept: is_infix_keyword(infix) {}
    bool is_infix_keyword;
};

struct SelectorWAdverb : Node<SelectorWAdverbIndex(SelectorIndex, AdverbIndex)> {
    static constexpr auto name { "SelectorNodeWithAdverb" };
};


/// @brief one or more variadic args
struct VariadicArgNode : Node<VariadicArgIndex(AnyExprIndex expr)> {
    static constexpr auto name { "VariadicArgNode" };
};

struct ArgumentList : ListNode<ArgumentListIndex, ArgumentEntryIndex> {
    static constexpr auto name { "ArgumentList" };
};


/// @brief An adverb that is an expression, e.g., `.(x)`.
struct AdverbExprNode : Node<AdverbExprIndex(AnyExprIndex)> {
    static constexpr auto name { "AdverbExprNode" };
};

////////////////////////////////////////////////////////////////////////////////
// Messages
////////////////////////////////////////////////////////////////////////////////
/// @brief A message written without a receiver, e.g., `f(1)`.
struct PrefixMessage : Node<PrefixMessageIndex(SelectorMaybeAdverbIndex, ArgumentListIndex, BlockListIndex blocks)> {
    static constexpr auto name { "PrefixMessage" };
};
/// @brief A binary operator, e.g., `a + b`.
struct BinaryMessage : Node<BinaryMessageIndex(SelectorMaybeAdverbIndex, AnyExprIndex lhs, AnyExprIndex rhs)> {
    static constexpr auto name { "BinaryMessage" };
};
/// @brief A message sent to a receiver, e.g., `a.f(1)`.
struct PostfixMessage : Node<PostfixMessageIndex(AnyExprIndex receiver, SelectorMaybeAdverbIndex, ArgumentListIndex,
                                                 BlockListIndex blocks)> {
    static constexpr auto name { "PostfixMessage" };
};
/// @brief A call of a value, e.g., `f.(1)`.
struct ValueMessage : Node<ValueMessageIndex(AnyExprIndex receiver, ArgumentListIndex, BlockListIndex)> {
    static constexpr auto name { "ValueMessage" };
};
/// @brief A constructor call, e.g., `Foo(1)`.
struct NewMessage : Node<NewMessageIndex(ClassNameIdentifierIndex receiver, ArgumentListIndex, BlockListIndex)> {
    static constexpr auto name { "NewMessage" };
};
/// @brief An index, e.g., `a[0]`.
struct AtMessage : Node<AtMessageIndex(AnyExprIndex receiver, AtMessageArgIndex)> {
    static constexpr auto name { "AtMessage" };
};


/// @brief A reference, e.g., `` `x ``.
struct ReferenceNode : Node<ReferenceIndex(AnyExprIndex)> {
    static constexpr auto name { "ReferenceNode" };
};


/// @brief An assignment to a name, `target` is whether it is an environment variable.
struct AssignmentNode : Node<AssignmentIndex(NamedIdentifierIndex name, AnyExprIndex expr)> {
    static constexpr auto name { "AssignmentNode" };
    enum struct Target { Normal, Environment } target;
    constexpr AssignmentNode(Target t = Target::Normal): target(t) {}
};

/// An indexed assignment, e.g., `a[0] = 1`.
/// This isn't a message node due to a nasty quirk in the grammar which was there in the original version.
// This isn't a message node due to a nasty quirk in the grammar which was there in the original version.
struct AssignmentAtNode : Node<AssignmentAtIndex(AnyExprIndex thing, ArgumentListIndex args, AnyExprIndex value)> {
    static constexpr auto name { "AssignmentAt" };
};

/// @brief A setter, e.g., `a.x = 1`.
struct SetterNode
    : Node<SetterIndex(join<ArgumentListIndex, AnyExprIndex> thing, NamedIdentifierIndex name, AnyExprIndex value)> {
    static constexpr auto name { "SetterNode" };
};


////////////////////////////////////////////////////////////////////////////////
// Generators
////////////////////////////////////////////////////////////////////////////////
/// @brief A list comprehension, e.g., `{: x * 2, x <- list }`.
struct GeneratorNode : Node<GeneratorIndex(AnyExprIndex expr, GeneratorQualListIndex qualifiers)> {
    static constexpr auto name { "GeneratorNode" };
};

/// @brief Something written like a generator but with `;` in place of `:`.
struct GeneratorFakeNode : Node<GeneratorFakeIndex(AnyExprIndex expr, GeneratorQualListIndex qualifiers)> {
    static constexpr auto name { "GeneratorFakeNode" };
};

/// @brief The qualifiers of a generator.
struct GeneratorQualList : ListNode<GeneratorQualListIndex, GeneratorQualAnyIndex> {
    static constexpr auto name { "GeneratorQualList" };
};

////////////////////////////////////////////////////////////////////////////////
// Generator Qualifiers
////////////////////////////////////////////////////////////////////////////////

/// @brief Draws values from a collection, e.g., `value <- list`, or `value index <- list` to also name the index.
struct GeneratorQualAssign
    : Node<GeneratorQualAssignIndex(NamedIdentifierIndex element, maybe<NamedIdentifierIndex> index, AnyExprIndex)> {
    static constexpr auto name { "GeneratorQualAssign" };
};

/// @brief Declares a name, e.g., `var y = x`.
struct GeneratorQualDeclare : Node<GeneratorQualDeclareIndex(NamedIdentifierIndex name, AnyExprIndex expr)> {
    static constexpr auto name { "GeneratorQualDeclare" };
};

/// @brief A bare expression, which filters, e.g., `x > 1`.
struct GeneratorQualIf : Node<GeneratorQualIfIndex(AnyExprIndex boolean_expr)> {
    static constexpr auto name { "GeneratorQualIf" };
};

/// @brief Runs an expression for its side effect, e.g., `:: x.postln`.
struct GeneratorQualSideEffect : Node<GeneratorQualSideEffectIndex(AnyExprIndex expr)> {
    static constexpr auto name { "GeneratorQualSideEffect" };
};
/// @brief Stops the generator, e.g., `:while x < 10`.
struct GeneratorQualGuard : Node<GeneratorQualGuardIndex(AnyExprIndex clause)> {
    static constexpr auto name { "GeneratorQualGuard" };
};

////////////////////////////////////////////////////////////////////////////////
// Ranges
////////////////////////////////////////////////////////////////////////////////

/// @brief A range in an index, e.g., `a[1..3]`.
struct Range : Node<RangeIndex(ArgumentListIndex args, maybe<AnyExprIndex> end)> {
    static constexpr auto name { "Range" };
};

/// @brief A series generator, e.g., `(1, 3..9)`.
struct RangeGenerator : Node<RangeGeneratorIndex(ArgumentListIndex args, maybe<AnyExprIndex> end)> {
    static constexpr auto name { "RangeGenerator" };
};
/// @brief Assigning to a range, e.g., `a[1..3] = x`.
struct RangeAssign : Node<RangeAssignIndex(AnyExprIndex receiver, RangeIndex, AnyExprIndex)> {
    static constexpr auto name { "RangeAssign" };
};


/// @brief An argument declaration with a default, e.g., `x = 1`.
struct DeclareArgumentWithDefaultNode : Node<DeclareArgumentWithDefaultIndex(NamedIdentifierIndex, AnyExprIndex)> {
    static constexpr auto name { "DeclareArgumentWithDefaultNode" };
    // Whether the default is in brackets, `a = (1)`, `a(1)` or `|a = (1)|`. The latter two's brackets are the
    // declaration's, so the default node itself isn't bracketed.
    bool bracketed { false };
    constexpr DeclareArgumentWithDefaultNode(bool b = false): bracketed(b) {}
};

/// @brief A variadic argument declaration, e.g., `*rest`.
struct DeclareArgumentVariadicNode : Node<DeclareArgumentVariadicIndex(NamedIdentifierIndex)> {
    static constexpr auto name { "DeclareArgumentVariadicNode" };
};

struct DeclareArgumentList : ListNode<DeclareArgumentListIndex, DeclareAnyArgumentIndex> {
    static constexpr auto name { "DeclareArgumentList" };
};

struct DeclareVariableWithDefaultNode : Node<DeclareVariableWithDefaultIndex(NamedIdentifierIndex, AnyExprIndex)> {
    static constexpr auto name { "DeclareVariableWithDefaultNode" };
};

/// @brief Declares several variables from a value, e.g., `var #a, b = x`.
struct VariablePackAssign : Node<VariablePackAssignIndex(NamePackIndex, AnyExprIndex)> {
    static constexpr auto name { "VariablePackAssign" };
};


/// @brief One `var` statement, holding one or more declarations.
struct DeclareVariableList : ListNode<DeclareVariableListIndex, DeclareAnyVariableIndex> {
    static constexpr auto name { "DeclareVariableList" };
};

////////////////////////////////////////////////////////////////////////////////
// Blocks
////////////////////////////////////////////////////////////////////////////////

struct BlockContentsList : ListNode<BlockContentsListIndex, BlockItemIndex> {
    static constexpr auto name { "BlockContentsList" };
};

struct BlockNode : Node<BlockIndex(DeclareArgumentListIndex, maybe<BlockContentsListIndex>)> {
    static constexpr auto name { "BlockNode" };
    bool closed; // Whether it was written with a `#{}`.
    constexpr BlockNode(bool c = false): closed(c) {}
};


/// @brief A non local return, e.g., `^1`.
struct NonLocalReturnExpr : Node<NonLocalReturnExprIndex(AnyExprIndex)> {
    static constexpr auto name { "NonLocalReturnExpr" };
};

struct Method : Node<MethodIndex(SelectorIndex, DeclareArgumentListIndex, maybe<PrimitiveIdentifierIndex>,
                                 BlockContentsListIndex)> {
    static constexpr auto name { "Method" };
};

struct ClassMethod : Node<ClassMethodIndex(SelectorIndex, DeclareArgumentListIndex, maybe<PrimitiveIdentifierIndex>,
                                           BlockContentsListIndex)> {
    static constexpr auto name { "ClassMethod" };
};


struct MethodList : ListNode<MethodListIndex, AnyMethodIndex> {
    static constexpr auto name { "MethodList" };
};


/// @brief The `<`, `>`, or `<>` before a declared member.
struct RWAccessor : TerminalNode<RWAccessorIndex> {
    static constexpr auto name { "RWAccessor" };
    ReadWriteAccessor accessor;
    constexpr RWAccessor(ReadWriteAccessor r = ReadWriteAccessor::Private): accessor(r) {}
};

struct DeclareMember : Node<DeclareMemberIndex(maybe<RWAccessorIndex>, DeclareAnyVariableIndex)> {
    static constexpr auto name { "DeclareMember" };
};

struct DeclareMemberList : ListNode<DeclareMemberListIndex, DeclareMemberIndex> {
    static constexpr auto name { "DeclareClassVarList" };
};

struct DeclareClassMemberList : ListNode<DeclareClassMemberListIndex, DeclareMemberIndex> {
    static constexpr auto name { "DeclareClassVarList" };
};

struct DeclareConstList : ListNode<DeclareConstListIndex, DeclareMemberIndex> {
    static constexpr auto name { "DeclareClassVarList" };
};

/// @brief All the variable and constant statements of a class.
struct BigMemberList : ListNode<BigMemberListIndex, BigMemberEntryIndex> {
    static constexpr auto name { "BigMemberList" };
};


/// @brief A class definition: name, optional slot, optional superclass, members, and methods.
/// `version` is the language version it is written in.
struct Class
    : Node<ClassIndex(ClassNameIdentifierIndex name, maybe<NamedIdentifierIndex> slot,
                      maybe<ClassNameIdentifierIndex> super, BigMemberListIndex members, MethodListIndex meths)> {
    static constexpr auto name { "Class" };
    LanguageVersion version;
    constexpr Class(LanguageVersion v = LanguageVersion::Legacy): version(v) {}
};

/// @brief A class extension, e.g., `+ Foo { ... }`.
/// Has its own version, as it is often in another file, by another author, than the class.
struct ClassExtension : Node<ClassExtensionIndex(ClassNameIdentifierIndex name, MethodListIndex meths)> {
    static constexpr auto name { "ClassExtension" };
    LanguageVersion version;
    constexpr ClassExtension(LanguageVersion v = LanguageVersion::Legacy): version(v) {}
};

/// @brief A file of classes and extensions.
struct ClassOrExtensionList : ListNode<ClassOrExtensionListIndex, ClassOrExtensionIndex> {
    static constexpr auto name { "ClassOrExtensionList" };
};

/// @brief An scd file written as a function, e.g., `|x| x + 1`, or the VM's interpreted code.
struct FileAsFunction : Node<FileAsFunctionIndex(DeclareArgumentListIndex, BlockContentsListIndex)> {
    static constexpr auto name { "FileAsFunction" };
};

/// @brief A parenthesised group of statements in interpreted code, e.g., `( var a = 1; a + 1 )`.
struct ImmediatelyEvaluatedBlock
    : Node<ImmediatelyEvaluatedBlockIndex(DeclareArgumentListIndex, BlockContentsListIndex)> {
    static constexpr auto name { "ImmediatelyEvaluatedBlock" };
};

/// @brief Interpreted code: a list of its items.
struct RegionList : ListNode<RegionListIndex, RegionItemIndex> {
    static constexpr auto name { "RegionList" };
};

/// @brief A syntax error the parser recovered from, holding what it skipped.
struct Error : ListNode<ErrorIndex, AnyIndex> {
    static constexpr auto name { "Error" };
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


} // sc::ast
