#pragma once

#include "sc_sema/symbols.hpp"
#include "sc_util/strong_index.hpp"
#include "sc_util/type_set_index.hpp"
#include "sc_util/typed_graph.hpp"
#include "sc_util/typed_graph_node.hpp"

#include "ast_location.hpp"
#include "const_expr.hpp"

#include <unordered_map>
#include <utility>

namespace sc::ir {

template <typename T> struct Locatable {
    ASTLocation location;
    T value;
    [[nodiscard]] constexpr T& operator->() { return value; }
    [[nodiscard]] constexpr const T& operator->() const { return value; }

    [[nodiscard]] constexpr T& operator*() { return value; }
    [[nodiscard]] constexpr const T& operator*() const { return value; }
};

template <typename T> struct OptionallyLocatable {
    std::optional<ASTLocation> location;
    T value;

    [[nodiscard]] constexpr T& operator->() { return value; }
    [[nodiscard]] constexpr const T& operator->() const { return value; }

    [[nodiscard]] constexpr T& operator*() { return value; }
    [[nodiscard]] constexpr const T& operator*() const { return value; }
};

////////////////////////////////////////////////////////////////////////////////

enum struct SCNodeType {
    Missing,
    Message,
    Assign,

    KwArg,
    VariableDeclare,
    ExplicitReturn,

    KwArgList,
    PosArgList,
    VariadicArgList,
    Literal,
    Collection, // a collection that can't be turned into a literal.

    ArgumentDeclare,
    Function,
    Method,
    MethodList,
    Member,
    MemberList,
    Class,
    Extention,

    ExprList,
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

using SCLiteral_I = TypedNodeIndex<SCNodeType::Literal>;

using SCMessage_I = TypedNodeIndex<SCNodeType::Message>;
using SCAssign_I = TypedNodeIndex<SCNodeType::Assign>;

using SCKwArgList_I = TypedNodeIndex<SCNodeType::KwArgList>;
using SCPosArgList_I = TypedNodeIndex<SCNodeType::PosArgList>;
using SCVariadicArgList_I = TypedNodeIndex<SCNodeType::VariadicArgList>;

using SCKwArg_I = TypedNodeIndex<SCNodeType::KwArg>;
using SCExplicitReturn_I = TypedNodeIndex<SCNodeType::ExplicitReturn>;

using SCVariableDeclare_I = TypedNodeIndex<SCNodeType::VariableDeclare>;

using SCCollection_I = TypedNodeIndex<SCNodeType::Collection>;
using SCArgumentDeclare_I = TypedNodeIndex<SCNodeType::ArgumentDeclare>;
using SCFunction_I = TypedNodeIndex<SCNodeType::Function>;
using SCMethod_I = TypedNodeIndex<SCNodeType::Method>;
using SCMethodList_I = TypedNodeIndex<SCNodeType::MethodList>;
using SCMember_I = TypedNodeIndex<SCNodeType::Member>;
using SCMemberList_I = TypedNodeIndex<SCNodeType::MemberList>;
using SCClass_I = TypedNodeIndex<SCNodeType::Class>;
using SCExtention_I = TypedNodeIndex<SCNodeType::Extention>;

using SCExprList_I =
    join<TypedNodeIndex<SCNodeType::ExprList>, SCMethod_I, SCVariableDeclare_I, SCAssign_I, SCExplicitReturn_I>;

using SCClassOrExtentionList_I = TypedNodeIndex<SCNodeType::ClassOrExtentionList>;


////////////////////////////////////////////////////////////////////////////////

namespace priv {
template <typename Index> using Terminal = sc::util::typed_graph::TerminalNode<Index>;
template <typename This, typename... ARGS> using Node = sc::util::typed_graph::Node<This, ARGS...>;
template <typename This, typename Child> using List = sc::util::typed_graph::ListNode<This, Child>;
}

////////////////////////////////////////////////////////////////////////////////

// This is just a place holder, you need to look it up in the
struct SCLiteral : priv::Terminal<SCLiteral_I> {
    static constexpr auto type_name { "SCLiteral" };
    ConstExprIndex index;
    constexpr SCLiteral(ConstExprIndex i): index(i) { }
};

struct SCKwArg : priv::Node<SCKwArg_I(SCExprList_I value)> {
    static constexpr auto type_name { "SCKwArg" };
    Locatable<SymSCIdentifier> name;
};

struct SCPositionalArgs : priv::List<SCPosArgList_I, SCExprList_I> {
    static constexpr auto type_name { "SCPositionalArgs" };
    std::uint32_t size;
};

// This is a variable or a member assignment, it does not do `x.foo = 10`, which is the message `foo_(x, 10)`.
struct SCAssign : priv::Node<SCAssign_I(SCExprList_I value)> {
    static constexpr auto type_name { "SCAssign" };
    Locatable<SymSCIdentifier> name;
};

// clang-format off
struct SCMessage : priv::Node<
    SCMessage_I(
        SCExprList_I receiver, 
        maybe<SCPosArgList_I> positional_arguments,
        maybe<SCKwArgList_I> keyword_arguments,
        maybe<SCVariadicArgList_I> variadic_arguments, 
        maybe<SCExprList_I> adverb
    )
> {
    static constexpr auto type_name { "SCMessage" };
    // clang-format on
    Locatable<SymSCSelector> selector;
};

struct SCVariableDeclare : priv::Node<SCVariableDeclare_I(SCExprList_I value)> {
    static constexpr auto type_name { "SCVariableDeclare" };
    Locatable<SymSCIdentifier> name;
};

struct SCExplicitReturn : priv::Node<SCExplicitReturn_I(SCExprList_I value)> {
    static constexpr auto type_name { "SCExplicitReturn" };
};

struct SCCollection : priv::List<SCCollection_I, SCExprList_I> {
    static constexpr auto type_name { "SCCollection" };
    OptionallyLocatable<SymSCClassName> collection_class;
    std::uint32_t size;
};


struct SCArgumentDeclare : priv::Node<SCArgumentDeclare_I(maybe<SCExprList_I>)> {
    static constexpr auto type_name { "SCArgumentDeclare" };
    SymSCIdentifier name;
    bool preserve_nil;
};

// clang-format off
struct SCFunction : priv::Node<
    SCFunction_I(
        maybe<SCArgumentDeclare_I> positional_arguments,
        maybe<SCArgumentDeclare_I> variadic_arguments, 
        maybe<SCExprList_I> body
    )
> {
    // clang-format on
    static constexpr auto type_name { "SCFunction" };
    std::optional<Locatable<SymSCIdentifier>> function_name;
};


// clang-format off
struct SCMethod : priv::Node<
    SCMethod_I(
        maybe<SCArgumentDeclare_I> positional_arguments,
        maybe<SCArgumentDeclare_I> variadic_arguments, 
        maybe<SCExprList_I> body
    )
> {
    // clang-format on
    static constexpr auto type_name { "SCMethod" };
    Locatable<SymSCClassName> owning_class;
    std::optional<Locatable<SymSCPrimitive>> primitive;
};

struct SCMethodList : priv::List<SCMethodList_I, SCMethod_I> {
    static constexpr auto type_name { "SCMethodList" };
};


struct SCMember : priv::Node<SCMember_I(maybe<SCExprList_I> default_value)> {
    static constexpr auto type_name { "SCMember" };
    Locatable<SymSCIdentifier> name;
    bool read;
    bool write;
    bool constant;
};

struct SCMemberList : priv::List<SCMemberList_I, SCMember> {
    static constexpr auto type_name { "SCMemberList" };
};

// clang-format off
struct SCClass : priv::Node<
    SCClass_I(
        SCMemberList_I class_members, SCMemberList_I instance_members, 
        SCMethodList_I class_methods, SCMethodList_I instance_methods
    )
> {
    // clang-format on
    static constexpr auto type_name { "SCClass" };
    enum struct MemoryLayout { Float, Slot, Int };
    Locatable<SymSCClassName> name;
    std::optional<Locatable<MemoryLayout>> MemoryLayout;
};

struct SCExtention : priv::Node<SCExtention_I(SCMethodList_I methods)> {
    static constexpr auto type_name { "SCExtention" };
    Locatable<SymSCClassName> name;
};

struct SCClassOrExtentionList : priv::List<SCClassOrExtentionList_I, join<SCClass_I, SCExtention_I>> {
    static constexpr auto type_name { "SCClassOrExtentionList" };
};

struct SCExprList : priv::List<TypedNodeIndex<SCNodeType::ExprList>, SCExprList_I> {
    static constexpr auto type_name { "SCExprList" };
};

////////////////////////////////////////////////////////////////////////////////


using NodeCollectionHelper = sc::util::typed_graph::TypedGraphHelper<SCNodeType, TypedNodeIndex, //
                                                                     SCLiteral, //
                                                                     SCKwArg, //
                                                                     SCPositionalArgs, //
                                                                     SCAssign, //
                                                                     SCMessage, //
                                                                     SCVariableDeclare, //
                                                                     SCExplicitReturn, //
                                                                     SCCollection, //
                                                                     SCArgumentDeclare, //
                                                                     SCFunction, //
                                                                     SCMethod, //
                                                                     SCMethodList, //
                                                                     SCMember, //
                                                                     SCMemberList, //
                                                                     SCClass, //
                                                                     SCExtention, //
                                                                     SCClassOrExtentionList, //
                                                                     SCExprList //
                                                                     >;


////////////////////////////////////////////////////////////////////////////////

struct IRGraph : private sc::util::typed_graph::GraphImplementation<OptNodeIndex, NodeIndex, NodeCollectionHelper> {
    using Base = sc::util::typed_graph::GraphImplementation<OptNodeIndex, NodeIndex, NodeCollectionHelper>;

public:
    IRGraph() = default;
    IRGraph(const IRGraph&) = default;
    IRGraph(IRGraph&&) noexcept = default;
    IRGraph& operator=(const IRGraph&) = delete;
    IRGraph& operator=(IRGraph&&) = delete;

    using Edges = Base::Edges;
    using Variant = Base::Variant;

    template <typename I> using NodeTypeFromIndex = Base::NodeTypeFromIndex<I>;

    ////////////////////////////////////////////////////////////////////////////////
    // Create
    ////////////////////////////////////////////////////////////////////////////////
    template <typename Node, typename... ChildrenIndexes>
    [[nodiscard]] typename Node::Index create(Node&& node, ASTLocation location, ChildrenIndexes&&... child_indexes);

    ////////////////////////////////////////////////////////////////////////////////
    // Creating Edges
    ////////////////////////////////////////////////////////////////////////////////
    using Base::append;

    ////////////////////////////////////////////////////////////////////////////////
    // node type operations
    ////////////////////////////////////////////////////////////////////////////////
    using Base::is_a;

    template <typename TypedIndex, typename = std::enable_if<sc::util::typed_index::is_type_set_index<TypedIndex>()>>
    [[nodiscard]] static constexpr const char* node_name(TypedIndex);
    [[nodiscard]] static constexpr const char* node_name(const Variant& variant);

    ////////////////////////////////////////////////////////////////////////////////
    // accessors
    ////////////////////////////////////////////////////////////////////////////////
    using Base::payload;
    const ASTLocation location(NodeIndex i) const { return m_node_locations[*i]; }
    ASTLocation& location(NodeIndex i) { return m_node_locations[*i]; }

    ////////////////////////////////////////////////////////////////////////////////
    // Children. This is the type safe way to walk the graph.
    ////////////////////////////////////////////////////////////////////////////////
    using Base::children;

    ////////////////////////////////////////////////////////////////////////////////
    // Traversal
    ////////////////////////////////////////////////////////////////////////////////

    template <typename F> [[nodiscard]] static constexpr bool signature_check() {
        return std::is_invocable_v<F, const Edges&, const Variant&, const ASTLocation&, NodeIndex>;
    }

    template <typename F, typename = std::enable_if<signature_check<F>()>> //
    void flat_walk(F f);

    struct Default {
        void operator()(const Edges&, const Variant&, const ASTLocation&, NodeIndex, size_t /**depth*/) { }
    };

    /**
       @brief Expects functions of the signature:
        (const Edges& e, const Variant& v, const ASTLocation&, NodeIndex i, size_t depth)
    */
    template <typename EnterNode, typename BeforeChildren = Default, typename AfterChildren = Default,
              typename ExitNode = Default,
              typename = std::enable_if<signature_check<EnterNode>()>, //
              typename = std::enable_if<signature_check<BeforeChildren>()>, //
              typename = std::enable_if<signature_check<AfterChildren>()>, //
              typename = std::enable_if<signature_check<ExitNode>()>>

    void depth_first_traverse(NodeIndex i, EnterNode enter_node, BeforeChildren before_children = { },
                              AfterChildren after_children = { }, ExitNode exit_node = { });


    ////////////////////////////////////////////////////////////////////////////////
    // Constexprs
    ////////////////////////////////////////////////////////////////////////////////

    void register_node_as_consteval(NodeIndex node, ConstExprIndex expr) { nodes_to_constants.insert({ node, expr }); }
    std::optional<ConstExprIndex> node_as_constexpr(NodeIndex node) {
        if (const auto fnd = nodes_to_constants.find(node); fnd != nodes_to_constants.end())
            return { fnd->second };
        else
            return { };
    }

private:
    std::vector<ASTLocation> m_node_locations { };

    std::unordered_map<NodeIndex, ConstExprIndex> nodes_to_constants { };
};


////////////////////////////////////////////////////////////////////////////////

template <typename Node, typename... ChildrenIndexes>
[[nodiscard]] typename Node::Index IRGraph::create(Node&& node, ASTLocation location,
                                                   ChildrenIndexes&&... child_indexes) {
    const auto index = Base::create(std::forward<Node>(node), std::forward<ChildrenIndexes>(child_indexes)...);
    if (*index < m_node_locations.size()) {
        m_node_locations[*index] = std::move(location);
    } else {
        assert(*index == m_node_locations.size());
        m_node_locations.push_back(std::move(location));
    }
    return index;
}

template <typename EnterNode, typename BeforeChildren, typename AfterChildren, typename ExitNode, typename, typename,
          typename, typename>
void IRGraph::depth_first_traverse(NodeIndex i, EnterNode enter_node, BeforeChildren before_children,
                                   AfterChildren after_children, ExitNode exit_node) {
    const auto wrap = [&](auto& f) {
        return [&](const Edges& e, const Variant& v, NodeIndex index, size_t d) {
            f(e, v, m_node_locations[*index], index, d);
        };
    };
    Base::depth_first_traverse(i, wrap(enter_node), wrap(before_children), wrap(after_children), wrap(exit_node));
};

template <typename F, typename> void IRGraph::flat_walk(F f) {
    Base::flat_walk([&](const Edges& e, const Variant& v, size_t i) { std::invoke(f, e, v, m_node_locations[i], i); });
};


}
