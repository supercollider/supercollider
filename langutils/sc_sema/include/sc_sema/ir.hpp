#pragma once

#include "sc_parser/sc_grammar_shared.hpp"
#include "sc_sema/type_info.hpp"
#include "sc_sema/ir_index.hpp"


#include "sc_util/type_set_index.hpp"

#include "sc_util/typed_graph_helper.hpp"
#include "sc_util/typed_graph_node.hpp"
#include "sc_util/typed_graph.hpp"

#include "symbols.hpp"

#include "ast_location.hpp"
#include "const_expr_index.hpp"

#include <unordered_map>
#include <utility>
namespace sc::ir {


////////////////////////////////////////////////////////////////////////////////

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
    constexpr SCLiteral(ConstExprIndex i): index(i) { }
};

struct SCKwArg : priv::Node<SCKwArg_I(SCAnyExpr_I value)> {
    static constexpr auto type_name { "SCKwArg" };
    Locatable<SymSCIdentifier> name;
};

struct SCPositionalArgs : priv::List<SCPosArgList_I, SCAnyExpr_I> {
    static constexpr auto type_name { "SCPositionalArgs" };
    std::uint32_t size;
};

// This is a variable or a member assignment, it does not do `x.foo = 10`, which is the message `foo_(x, 10)`.
struct SCAssign : priv::Node<SCAssign_I(SCAnyExpr_I value)> {
    static constexpr auto type_name { "SCAssign" };
    Locatable<SymSCIdentifier> name;
};

struct SCMessage : priv::Node<SCMessage_I(SCAnyExpr_I receiver, //
                                          maybe<SCPosArgList_I> positional_arguments, //
                                          maybe<SCKwArgList_I> keyword_arguments, //
                                          maybe<SCVariadicArgList_I> variadic_arguments, //
                                          maybe<SCAnyExpr_I> adverb //
                                          )> {
    static constexpr auto type_name { "SCMessage" };
    Locatable<SymSCSelector> selector;
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
    MemoryLayout memory_layout { MemoryLayout::Slot };
    std::optional<SymSCClassName> meta_class {};
};


struct SCExprList : priv::List<TypedNodeIndex<SCNodeType::ExprList>, SCAnyExpr_I> {
    static constexpr auto type_name { "SCExprList" };
};

////////////////////////////////////////////////////////////////////////////////


using NodeCollectionHelper = sc::util::typed_graph::TypedGraphHelper<SCNodeType, TypedNodeIndex, //
                                                                     SCMissing, //
                                                                     SCLiteral, //
                                                                     SCKwArg, //
                                                                     SCPositionalArgs, //
                                                                     SCAssign, //
                                                                     SCMessage, //
                                                                     SCVariableDeclare, //
                                                                     SCExplicitReturn, //
                                                                     SCCollection, //
                                                                     SCArgumentDeclare, //
                                                                     SCArgumentDeclareList, //
                                                                     SCMethod, //
                                                                     SCFunction,
                                                                     SCMethodList, //
                                                                     SCMember, //
                                                                     SCMemberList, //
                                                                     SCClass, //
                                                                     SCExprList //
                                                                     >;


////////////////////////////////////////////////////////////////////////////////

}

namespace sc::util::typed_graph {

extern template struct sc::util::typed_graph::GraphImplementation<sc::ir::OptNodeIndex, sc::ir::NodeIndex,
                                                                  sc::ir::NodeCollectionHelper>;
}


namespace sc::ir {
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
