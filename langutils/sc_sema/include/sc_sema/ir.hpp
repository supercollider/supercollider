#pragma once


#include "sc_sema/ir_index.hpp"
#include "sc_sema/ir_nodes.hpp"
#include "sc_util/overload.hpp"
#include "sc_util/typed_graph_helper.hpp"
#include "sc_util/typed_graph.hpp"
#include <unordered_map>
#include <utility>


namespace sc::ir {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


using NodeCollectionHelper = sc::util::typed_graph::TypedGraphHelper<SCNodeType, TypedNodeIndex, //
                                                                     SCMissing, //
                                                                     SCLiteral, //
                                                                     SCKwArg, //
                                                                     SCPositionalArgs, //
                                                                     SCAssign, //
                                                                     SCRead, //
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
                                                                     SCRef,
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
    [[nodiscard]] static constexpr const char* node_name(const Variant& variant) {
        return std::visit(
            [](const auto& v) -> const char* {
                using V = std::remove_reference_t<std::remove_cv_t<decltype(v)>>;
                return V::type_name;
            },
            variant);
    };

    void add_root(SCClass_I c) {
        assert(std::get_if<std::monostate>(&m_roots) != nullptr);
        m_roots = c;
    }

    void add_root(SCAnyExpr_I c) {
        assert(std::get_if<std::monostate>(&m_roots) != nullptr);
        m_roots = c;
    }

    std::optional<std::variant<SCClass_I, SCAnyExpr_I>> root() const {
        using R = std::optional<std::variant<SCClass_I, SCAnyExpr_I>>;
        return std::visit(util::overload {
                              [&](SCClass_I i) -> R { return { i }; },
                              [&](SCAnyExpr_I i) -> R { return { i }; },
                              [&](std::monostate) -> R { return { }; },
                          },
                          m_roots);
    }

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
                              AfterChildren after_children = { }, ExitNode exit_node = { }) const;


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
    std::variant<std::monostate, SCClass_I, SCAnyExpr_I> m_roots;
};



////////////////////////////////////////////////////////////////////////////////

template <typename TypedIndex, typename> [[nodiscard]] constexpr const char* IRGraph::node_name(TypedIndex) {
    using NodeType = NodeTypeFromIndex<TypedIndex>;
    return NodeType::type_name;
};

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
                                   AfterChildren after_children, ExitNode exit_node) const {
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
