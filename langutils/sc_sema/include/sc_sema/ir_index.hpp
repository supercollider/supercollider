#pragma once
#include "sc_util/type_set_index.hpp"
#include <cstdint>
namespace sc::ir {
enum struct SCNodeType {
    Missing,
    Message,

    Assign,
    Read,

    Ref,

    KwArg,
    VariableDeclare,
    ExplicitReturn,

    KwArgList,
    PosArgList,
    VariadicArgList,
    Literal,
    Collection, // a collection that can't be turned into a literal.
    Range,
    PackAssign,
    PackDeclare,
    PackTarget,
    PackTargetList,
    Generator,
    GenQualIterate,
    GenQualDeclare,
    GenQualIf,
    GenQualSideEffect,
    GenQualWhile,
    GenQualList,
    RangeGenerator,

    ArgumentDeclare,
    ArgumentDeclareList,
    Method,
    Function,
    MethodList,
    Member,
    MemberList,
    Class,
    Extention,

    ExprList,
    Body,
    ClassOrExtentionList,
};

////////////////////////////////////////////////////////////////////////////////

using NodeSpec = sc::util::typed_index::Spec<std::uint32_t, SCNodeType, struct Graph__>;
using NodeDef = util::typed_index::QuicklyDefineTypesFromSpec<NodeSpec>;
using OptNodeIndex = NodeDef::OptionalIndex;
using NodeIndex = NodeDef::Index;

template <SCNodeType... ts> using TypedNodeIndex = NodeDef::TypedIndex<ts...>;

template <typename... Ts> using join = sc::util::typed_index::join<Ts...>;
template <typename T> using maybe = join<T, TypedNodeIndex<SCNodeType::Missing>>;

////////////////////////////////////////////////////////////////////////////////

using SCMissing_I = TypedNodeIndex<SCNodeType::Missing>;
using SCLiteral_I = TypedNodeIndex<SCNodeType::Literal>;

using SCRef_I = TypedNodeIndex<SCNodeType::Ref>;

using SCMessage_I = TypedNodeIndex<SCNodeType::Message>;
using SCAssign_I = TypedNodeIndex<SCNodeType::Assign>;
using SCRead_I = TypedNodeIndex<SCNodeType::Read>;

using SCKwArgList_I = TypedNodeIndex<SCNodeType::KwArgList>;
using SCPosArgList_I = TypedNodeIndex<SCNodeType::PosArgList>;
using SCVariadicArgList_I = TypedNodeIndex<SCNodeType::VariadicArgList>;

using SCKwArg_I = TypedNodeIndex<SCNodeType::KwArg>;
using SCExplicitReturn_I = TypedNodeIndex<SCNodeType::ExplicitReturn>;

using SCVariableDeclare_I = TypedNodeIndex<SCNodeType::VariableDeclare>;

using SCCollection_I = TypedNodeIndex<SCNodeType::Collection>;
using SCRange_I = TypedNodeIndex<SCNodeType::Range>;
using SCPackAssign_I = TypedNodeIndex<SCNodeType::PackAssign>;
using SCPackDeclare_I = TypedNodeIndex<SCNodeType::PackDeclare>;
using SCPackTarget_I = TypedNodeIndex<SCNodeType::PackTarget>;
using SCPackTargetList_I = TypedNodeIndex<SCNodeType::PackTargetList>;
using SCGenerator_I = TypedNodeIndex<SCNodeType::Generator>;
using SCGenQualIterate_I = TypedNodeIndex<SCNodeType::GenQualIterate>;
using SCGenQualDeclare_I = TypedNodeIndex<SCNodeType::GenQualDeclare>;
using SCGenQualIf_I = TypedNodeIndex<SCNodeType::GenQualIf>;
using SCGenQualSideEffect_I = TypedNodeIndex<SCNodeType::GenQualSideEffect>;
using SCGenQualWhile_I = TypedNodeIndex<SCNodeType::GenQualWhile>;
using SCGenQualList_I = TypedNodeIndex<SCNodeType::GenQualList>;
using SCRangeGenerator_I = TypedNodeIndex<SCNodeType::RangeGenerator>;
using SCArgumentDeclare_I = TypedNodeIndex<SCNodeType::ArgumentDeclare>;
using SCArgumentDeclareList_I = TypedNodeIndex<SCNodeType::ArgumentDeclareList>;
using SCMethod_I = TypedNodeIndex<SCNodeType::Method>;
using SCFunction_I = TypedNodeIndex<SCNodeType::Function>;
using SCMethodList_I = TypedNodeIndex<SCNodeType::MethodList>;
using SCMember_I = TypedNodeIndex<SCNodeType::Member>;
using SCMemberList_I = TypedNodeIndex<SCNodeType::MemberList>;
using SCClass_I = TypedNodeIndex<SCNodeType::Class>;
using SCExtention_I = TypedNodeIndex<SCNodeType::Extention>;

using SCExprList_I = TypedNodeIndex<SCNodeType::ExprList>;
using SCBody_I = TypedNodeIndex<SCNodeType::Body>;

using SCAnyExpr_I = join<SCExprList_I, SCFunction_I, SCAssign_I, SCRead_I, SCExplicitReturn_I, SCLiteral_I, SCRef_I,
                         SCMessage_I, SCCollection_I, SCRange_I, SCPackAssign_I, SCGenerator_I, SCRangeGenerator_I>;

using SCClassList_I = TypedNodeIndex<SCNodeType::ClassOrExtentionList>;


}