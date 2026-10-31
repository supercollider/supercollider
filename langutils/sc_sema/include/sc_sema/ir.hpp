#pragma once


#include "sc_util/indexed_vector.hpp"
#include "sc_util/typed_graph.hpp"

#include "sc_sema/details/indexes.hpp"
#include "sc_sema/details/node_collection.hpp"

#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace sc::src {
struct TextInfo;
}


namespace sc::util::typed_graph {

// Make the base tempalte extern to avoid creating it everytime.
// Helps with compile times.
extern template struct sc::util::typed_graph::GraphImplementation<sc::ir::OptNodeIndex, sc::ir::NodeIndex,
                                                                  sc::ir::NodeCollectionHelper>;
}


namespace sc::ir {

struct LocalVariable {
    SymSCIdentifier name;
    SCLocalOwner_I owner; ///< The function, method, or generator that declares it.
    SourceLocation declaration;
    bool assigned_to { false };
    bool captured { false };
};

struct MemberVariable {
    SymSCIdentifier name;
    SymSCClassName owner;
    SourceLocation declaration;
};

/// @brief The intermediate representation of one class, or of one piece of interpreted code.
///
/// A typed graph of nodes (see ir_nodes.hpp), each with the SourceLocation it came from, plus the tables of local
/// variables, member variables, sources, and constant nodes. It holds the text its locations refer to, so the ast is
/// not needed once it is lowered. Walk it type safely with children() and the typed indexes.
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
    /// @brief Adds a node to the graph.
    /// @param child_indexes The node's children, which must match what the node type accepts.
    template <typename Node, typename... ChildrenIndexes>
    [[nodiscard]] typename Node::Index create(Node&& node, SourceLocation location, ChildrenIndexes&&... child_indexes);

    ////////////////////////////////////////////////////////////////////////////////
    // Creating Edges
    ////////////////////////////////////////////////////////////////////////////////
    using Base::append;

    ////////////////////////////////////////////////////////////////////////////////
    // node type operations
    ////////////////////////////////////////////////////////////////////////////////
    using Base::is_a;

    template <typename TypedIndex> [[nodiscard]] static constexpr const char* node_name(TypedIndex);
    [[nodiscard]] static const char* node_name(const Variant& variant);

    /// @name Roots
    /// A graph has one root: a class, an expression, or interpreted code.
    /// @{

    void add_root(SCClass_I c);
    void add_root(SCAnyExpr_I c);
    void add_root(SCInterpreted_I c);

    std::optional<std::variant<SCClass_I, SCAnyExpr_I, SCInterpreted_I>> root() const;
    /// @}

    ////////////////////////////////////////////////////////////////////////////////
    // accessors
    ////////////////////////////////////////////////////////////////////////////////
    using Base::payload;
    /// @brief The first child of a node.
    /// To walk one node's children without its siblings, depth_first_traverse continues along the siblings.
    [[nodiscard]] OptNodeIndex first_child(NodeIndex i) const { return Base::edges(i).first_child; }
    using Base::last_child;
    [[nodiscard]] SourceLocation location(NodeIndex i) const { return m_node_locations[i]; }

    ////////////////////////////////////////////////////////////////////////////////
    // Sources
    // The text the locations refer to. Holding the text keeps it alive, the ast isn't needed once it is lowered.
    ////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] SourceIndex register_source(const std::shared_ptr<const src::TextInfo>& text);
    [[nodiscard]] SourceLocation make_location(const std::shared_ptr<const src::TextInfo>& text,
                                               src::SourceCodeRange range);
    [[nodiscard]] const std::shared_ptr<const src::TextInfo>& text_info(SourceIndex i) const;
    [[nodiscard]] const util::IndexedVector<SourceIndex, std::shared_ptr<const src::TextInfo>>& sources() const;
    [[nodiscard]] static src::SourceCodeRange range(SourceLocation l);
    [[nodiscard]] std::string_view read(SourceLocation l) const;


    ////////////////////////////////////////////////////////////////////////////////
    // Children. This is the type safe way to walk the graph.
    ////////////////////////////////////////////////////////////////////////////////
    using Base::as_a;
    using Base::assume_a;
    using Base::children;

    ////////////////////////////////////////////////////////////////////////////////
    // Traversal
    ////////////////////////////////////////////////////////////////////////////////

    struct Default {
        void operator()(const Edges&, const Variant&, const SourceLocation&, NodeIndex, size_t /**depth*/) {}
    };

    /// @brief Walks the tree from a node, depth first.
    /// All the callbacks have the signature
    /// `(const Edges&, const Variant&, const SourceLocation&, NodeIndex, size_t depth)`.
    /// Siblings after @p i are visited too, as a list is walked by looping rather than recursing.
    template <typename EnterNode, typename BeforeChildren = Default, typename AfterChildren = Default,
              typename ExitNode = Default>

    void depth_first_traverse(NodeIndex i, EnterNode enter_node, BeforeChildren before_children = {},
                              AfterChildren after_children = {}, ExitNode exit_node = {}) const;


    ////////////////////////////////////////////////////////////////////////////////
    // Local variables
    ////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] LocalIndex add_local(LocalVariable v) { return m_locals.push_back(v); }
    [[nodiscard]] const LocalVariable& local(LocalIndex i) const { return m_locals[i]; }
    [[nodiscard]] LocalVariable& local(LocalIndex i) { return m_locals[i]; }
    [[nodiscard]] const util::IndexedVector<LocalIndex, LocalVariable>& locals() const { return m_locals; }

    ////////////////////////////////////////////////////////////////////////////////
    // Member variables
    ////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] MemberIndex member_index(MemberVariable v);
    [[nodiscard]] const MemberVariable& member(MemberIndex i) const { return m_members[i]; }
    [[nodiscard]] const util::IndexedVector<MemberIndex, MemberVariable>& members() const { return m_members; }

    ////////////////////////////////////////////////////////////////////////////////
    // Constexprs
    ////////////////////////////////////////////////////////////////////////////////

    /// @brief Records that a node always evaluates to a constant.
    void register_node_as_consteval(NodeIndex node, ConstExprIndex expr);

    [[nodiscard]] std::optional<ConstExprIndex> node_as_constexpr(NodeIndex node) const;


private:
    util::IndexedVector<NodeIndex, SourceLocation> m_node_locations {};
    util::IndexedVector<SourceIndex, std::shared_ptr<const src::TextInfo>> m_sources {};
    util::IndexedVector<LocalIndex, LocalVariable> m_locals {};
    util::IndexedVector<MemberIndex, MemberVariable> m_members {};

    std::unordered_map<NodeIndex, ConstExprIndex> nodes_to_constants {};
    std::variant<std::monostate, SCClass_I, SCAnyExpr_I, SCInterpreted_I> m_roots;
};


////////////////////////////////////////////////////////////////////////////////

template <typename TypedIndex> [[nodiscard]] constexpr const char* IRGraph::node_name(TypedIndex) {
    using NodeType = NodeTypeFromIndex<TypedIndex>;
    return NodeType::type_name;
};

template <typename Node, typename... ChildrenIndexes>
[[nodiscard]] typename Node::Index IRGraph::create(Node&& node, SourceLocation location,
                                                   ChildrenIndexes&&... child_indexes) {
    const auto index = Base::create(std::forward<Node>(node), std::forward<ChildrenIndexes>(child_indexes)...);
    if (m_node_locations.contains(index)) {
        m_node_locations[index] = std::move(location);
    } else {
        assert(index == m_node_locations.next_index());
        (void)m_node_locations.push_back(std::move(location));
    }

    return index;
}

template <typename EnterNode, typename BeforeChildren, typename AfterChildren, typename ExitNode>
void IRGraph::depth_first_traverse(NodeIndex i, EnterNode enter_node, BeforeChildren before_children,
                                   AfterChildren after_children, ExitNode exit_node) const {
    const auto wrap = [&](auto& f) {
        return [&](const Edges& e, const Variant& v, NodeIndex index, size_t d) {
            f(e, v, m_node_locations[index], index, d);
        };
    };
    Base::depth_first_traverse(i, wrap(enter_node), wrap(before_children), wrap(after_children), wrap(exit_node));
};
}
