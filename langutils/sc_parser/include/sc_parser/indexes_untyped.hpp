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

    // symbols
    KeybinOp,
    SymbolQuote,
    SymbolSlash,

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
    NameList, //
    NamePack, //

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

    Range,
    RangeGenerator,
    RangeAssign,

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
    VariablePackAssign,

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

    FileAsFunction,

    RegionList,

    COUNT
};


namespace details {
using Spec = sc::util::typed_index::Spec<std::uint32_t, NodeFlag, struct ASTGraph___>;
using Def = sc::util::typed_index::QuicklyDefineTypesFromSpec<Spec>;
}

using Index = details::Def::Index;
using OptionalIndex = details::Def::OptionalIndex;
}