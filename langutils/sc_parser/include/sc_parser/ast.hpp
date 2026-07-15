// Copyright Jordan Henderson 2026
#pragma once

#include "indexes_typed.hpp"
#include "sc_lexer/text_info.hpp"
#include "sc_lexer/text_location.hpp"
#include "nodes.hpp"
#include "sc_util/type_set_index.hpp"
#include "sc_util/typed_graph.hpp"

#include "sc_diagnostic/sc_diagnostic.hpp"

#include <functional>
#include <memory>
#include <type_traits>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>


namespace sc::ast {

namespace priv {
template <typename... Os> struct overload : Os... {
    using Os::operator()...;
};

template <typename... Os> overload(Os...) -> overload<Os...>;
}

////////////////////////////////////////////////////////////////////////////////
class ASTGraph : private sc::util::typed_graph::GraphImplementation<OptionalIndex, Index, NodeCollectionHelper> {
    using Base = sc::util::typed_graph::GraphImplementation<OptionalIndex, Index, NodeCollectionHelper>;
    using Helper = Base::Helper;

public:
    ASTGraph(std::shared_ptr<const lex::TextInfo> text_info): m_text_info(text_info) { }
    ASTGraph(const ASTGraph&) = default;
    ASTGraph(ASTGraph&&) noexcept = default;
    ASTGraph& operator=(const ASTGraph&) = delete;
    ASTGraph& operator=(ASTGraph&&) = delete;

    using Edges = Base::Edges;
    using Variant = Base::Variant;

    template <typename I> using NodeTypeFromIndex = Base::NodeTypeFromIndex<I>;

    ////////////////////////////////////////////////////////////////////////////////
    // Create
    ////////////////////////////////////////////////////////////////////////////////
    template <typename Node, typename... ChildrenIndexes>
    [[nodiscard]] typename Node::Index create(Node&& node, lex::SourceCodeRange location,
                                              ChildrenIndexes&&... child_indexes);

    ////////////////////////////////////////////////////////////////////////////////
    // Creating Edges
    ////////////////////////////////////////////////////////////////////////////////

    using Base::append;
    using Base::merge;
    using Base::prepend;

    template <typename I, typename... Children> //
    auto append(I p, lex::SourceCodeRange range, Children... children);

    ////////////////////////////////////////////////////////////////////////////////
    // node type operations
    ////////////////////////////////////////////////////////////////////////////////

    using Base::as_a;
    using Base::cast;
    using Base::is_a;

    template <typename TypedIndex, typename = std::enable_if<sc::util::typed_index::is_type_set_index<TypedIndex>()>>
    [[nodiscard]] static constexpr const char* node_name(TypedIndex);
    [[nodiscard]] static constexpr const char* node_name(const Variant& variant);

    /// Turns as missing<NODEINDEX> into a std::optional<NODEINDEX>
    template <typename TypedIndex, typename = std::enable_if<sc::util::typed_index::is_type_set_index<TypedIndex>()>>
    [[nodiscard]] constexpr auto present(TypedIndex i) const
        -> std::optional<typename decltype(TypedIndex::template remove_from_set<NodeFlag::Missing>())::type>;


    ////////////////////////////////////////////////////////////////////////////////
    // accessors
    ////////////////////////////////////////////////////////////////////////////////

    using Base::edges;
    using Base::last_child;
    using Base::orphans;
    using Base::payload;

    const sc::lex::SourceCodeRange& location(UntypedNodeIndex i) const { return m_node_locations[*i]; }
    sc::lex::SourceCodeRange& location(UntypedNodeIndex i) { return m_node_locations[*i]; }

    [[nodiscard]] std::shared_ptr<const lex::TextInfo> text_info() const { return m_text_info; }

    [[nodiscard]] std::string_view string_view(UntypedNodeIndex i) const { return m_text_info->read(location(i)); }

    ////////////////////////////////////////////////////////////////////////////////
    // Children. This is the type safe way to walk the graph.
    ////////////////////////////////////////////////////////////////////////////////

    using Base::children;
    using Base::index_to_variant;
    using Base::visit;

    ////////////////////////////////////////////////////////////////////////////////
    // Roots
    ////////////////////////////////////////////////////////////////////////////////

    RegionListIndex assign_root(RegionListIndex i);
    ClassOrExtensionListIndex assign_root(ClassOrExtensionListIndex i);

    [[nodiscard]] std::optional<Index> root_any() const;

    [[nodiscard]] std::optional<std::variant<RegionListIndex, ClassOrExtensionListIndex>> root() const;

    ////////////////////////////////////////////////////////////////////////////////
    // Diagnostics
    ////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] explicit operator bool() const;
    void add_diagnostic(sc::diag::Diagnostic d);
    [[nodiscard]] const std::vector<sc::diag::Diagnostic>& diagnostics() const&;
    [[nodiscard]] std::vector<sc::diag::Diagnostic> diagnostics() &&;

    ////////////////////////////////////////////////////////////////////////////////
    // Traversal
    ////////////////////////////////////////////////////////////////////////////////
    template <typename F> [[nodiscard]] static constexpr bool signature_check() {
        return std::is_invocable_v<F, const Edges&, const Variant&, const lex::SourceCodeRange&, Index>;
    }


    template <typename F, typename = std::enable_if<signature_check<F>()>> //
    void flat_walk(F f) const;

    template <typename F, typename = std::enable_if<signature_check<F>()>> //
    void traverse_only_children(F& f, Index i);

    struct Default {
        void operator()(const Edges&, const Variant&, const lex::SourceCodeRange&, Index, size_t /*depth*/) { }
    };


    /**
    @brief Expects functions of the signature:
        (const Edges& e, const Variant& v, const lex::SourceCodeRange&, Index i, size_t depth)
    */
    template <typename EnterNode, typename BeforeChildren = Default, typename AfterChildren = Default,
              typename ExitNode = Default,
              typename = std::enable_if<signature_check<EnterNode>()>, //
              typename = std::enable_if<signature_check<BeforeChildren>()>, //
              typename = std::enable_if<signature_check<AfterChildren>()>, //
              typename = std::enable_if<signature_check<ExitNode>()>>

    void depth_first_traverse(Index i, EnterNode enter_node, BeforeChildren before_children = {},
                              AfterChildren after_children = {}, ExitNode exit_node = {}) const;

    void debug_df_print(std::ostream&) const;
    void debug_flat_print(std::ostream&) const;
    using Base::debug_find_loop;


private:
    std::shared_ptr<const lex::TextInfo> m_text_info;
    std::vector<lex::SourceCodeRange> m_node_locations {};

    std::vector<sc::diag::Diagnostic> m_diagnostics { };

    bool m_has_fatal_diagnostic { false };

    std::variant<std::monostate, RegionListIndex, ClassOrExtensionListIndex> m_roots { };
};


////////////////////////////////////////////////////////////////////////////////


template <typename TypedIndex, typename>
[[nodiscard]] constexpr auto ASTGraph::present(TypedIndex i) const
    -> std::optional<typename decltype(TypedIndex::template remove_from_set<NodeFlag::Missing>())::type> //
{
    return is_a<MissingIndex>(i) ? std::nullopt : std::optional { *i };
}

template <typename F, typename> inline void ASTGraph::flat_walk(F f) const {
    Base::flat_walk([&](const Edges& e, const Variant& v, size_t i) { std::invoke(f, e, v, m_node_locations[i], i); });
}

template <typename TypedIndex, typename> //
[[nodiscard]] constexpr const char* ASTGraph::node_name(TypedIndex) {
    using NodeType = NodeTypeFromIndex<TypedIndex>;
    return NodeType::name;
}

template <typename I, typename... Children>
inline auto ASTGraph::append(I p, lex::SourceCodeRange range, Children... children) {
    (Base::append(p, children), ...);
    // At this point we know this is valid because the base does the checks
    m_node_locations[*p] = std::move(range);
    return p;
};

template <typename EnterNode, typename BeforeChildren, typename AfterChildren, typename ExitNode, typename, typename,
          typename, typename>
inline void ASTGraph::depth_first_traverse(Index i, EnterNode enter_node, BeforeChildren before_children,
                                           AfterChildren after_children, ExitNode exit_node) const {
    const auto wrap = [&](auto& f) {
        return [&](const Edges& e, const Variant& v, Index index, size_t d) {
            f(e, v, m_node_locations[*index], index, d);
        };
    };
    Base::depth_first_traverse(i, wrap(enter_node), wrap(before_children), wrap(after_children), wrap(exit_node));
}


template <typename Node, typename... ChildrenIndexes>
typename Node::Index ASTGraph::create(Node&& node, lex::SourceCodeRange location, ChildrenIndexes&&... child_indexes) {
    const auto index = Base::create(std::forward<Node>(node), std::forward<ChildrenIndexes>(child_indexes)...);

    if (*index < m_node_locations.size()) {
        m_node_locations[*index] = std::move(location);
    } else {
        assert(*index == m_node_locations.size());
        m_node_locations.push_back(std::move(location));
    }

    return index;
}

template <typename F, typename> inline void ASTGraph::traverse_only_children(F& f, Index i) {
    Base::traverse_only_children(
        [&](const Edges& e, const Variant& v, Index index) { f(e, v, m_node_locations[*index], index); }, i);
}

[[nodiscard]] constexpr const char* ASTGraph::node_name(const Variant& variant) {
    return std::visit(
        [](const auto& v) -> const char* {
            using V = std::remove_reference_t<std::remove_cv_t<decltype(v)>>;
            return V::name;
        },
        variant);
};


// class NodeGraph {
// public:
//     NodeGraph() = default;
//     NodeGraph(NodeGraph&&) noexcept = default;
//     //
//     NodeGraph(const NodeGraph&) = delete;
//     NodeGraph& operator=(NodeGraph&&) noexcept = delete;
//     NodeGraph& operator=(const NodeGraph&) = delete;

//     [[nodiscard]] std::vector<Index> orphans() const;

//     RegionListIndex assign_root(RegionListIndex i);
//     ClassOrExtensionListIndex assign_root(ClassOrExtensionListIndex i);

//     [[nodiscard]] std::optional<Index> root_any() const;
//     [[nodiscard]] std::variant<std::monostate, RegionListIndex, ClassOrExtensionListIndex> root() const;

//     // Hasn't encountered a fatal diagnostic (has no 'errors').
//     [[nodiscard]] explicit operator bool() const;

//     void add_diagnostic(Diagnostic d);

//     [[nodiscard]] const std::vector<Diagnostic>& diagnostics() const& { return m_diagnostics; }
//     [[nodiscard]] std::vector<Diagnostic> diagnostics() && { return std::move(m_diagnostics); }

//     template <typename Node, class... CHILDREN>
//     [[nodiscard]] auto create(Node n, sc::lex::SourceCodeRange loc, CHILDREN... children);

//     template <typename To, NodeFlag... Froms, typename... ARGS>
//     auto cast(TypedIndex<Froms...> from_index, ARGS&&... args) -> typename To::Index;

//     template <NodeFlag... Ps>
//     TypedIndex<Ps...> append_to_list(TypedIndex<Ps...> p, sc::lex::SourceCodeRange) =
//         delete; // !!!NOTE!!! If this has occurred, you have written '@n' instead of '$n' in the grammar file.

//     // Updates the location data of the parent.
//     template <NodeFlag... Ps, typename... Cs>
//     TypedIndex<Ps...> append_to_list(TypedIndex<Ps...> p, sc::lex::SourceCodeRange range, Cs... cs);

//     template <NodeFlag... Ps, NodeFlag... Cs>
//     TypedIndex<Ps...> append_to_list(TypedIndex<Ps...> p, TypedIndex<Cs...> c);

//     template <NodeFlag... Ps, NodeFlag... Cs>
//     TypedIndex<Ps...> prepend_to_list(TypedIndex<Ps...> list, TypedIndex<Cs...> item);

//     // After this, the child_list should be thought of as being in a moved from state, do not use it.
//     template <NodeFlag... Ps, NodeFlag... Cs>
//     TypedIndex<Ps...> merge_list(TypedIndex<Ps...> parent, TypedIndex<Cs...> child_list);

//     // Used to replace one subgraph with the other.
//     // This will replace the variant and the children (but no other edges).
//     // It will keep the old location unless specified
//     template <NodeFlag... OLDs, NodeFlag... NEWs>
//     TypedIndex<OLDs...> substitute(TypedIndex<OLDs...> old, TypedIndex<NEWs...> n);

//     template <NodeFlag... Ts> [[nodiscard]] auto& payload(TypedIndex<Ts...> index);
//     template <NodeFlag... Ts> [[nodiscard]] const auto& payload(TypedIndex<Ts...> index) const;

//     [[nodiscard]] nodes::NodeVariant& payload(Index index) { return m_payloads[*index]; };
//     [[nodiscard]] const nodes::NodeVariant& payload(Index index) const { return m_payloads[*index]; };

//     template <typename TypedIndexT> [[nodiscard]] bool is_a(Index i) const noexcept;

//     [[nodiscard]] const sc::lex::SourceCodeRange& location(Index index) const;
//     [[nodiscard]] sc::lex::SourceCodeRange& location(Index index);

//     [[nodiscard]] nodes::Edges& edges(Index i) noexcept;
//     [[nodiscard]] const nodes::Edges& edges(Index i) const noexcept;

//     // Mostly useful for debugging purposes.
//     template <typename F> void flat_walk(F&& f) const;

//     struct EmptyAction {
//         constexpr void operator()() const { }
//     };

//     template <class EnterNode, class BeforeChildren = EmptyAction, class AfterChildren = EmptyAction,
//               class ExitNode = EmptyAction>
//     void depth_first_traverse(Index i, EnterNode&& enter_node, BeforeChildren&& before_children = { },
//                               AfterChildren&& after_children = { }, ExitNode&& exit_node = { });

//     template <typename F> void traverse_only_children(F&& f, Index i);

// private:
//     std::vector<sc::lex::SourceCodeRange> m_locations { };
//     std::vector<nodes::Edges> m_edges { };
//     std::vector<nodes::NodeVariant> m_payloads { };

//     std::vector<Index::Underlying> m_dead_nodes { };

//     std::vector<Diagnostic> m_diagnostics { };
//     bool m_has_fatal_diagnostic { false };

//     std::variant<std::monostate, RegionListIndex, ClassOrExtensionListIndex> m_roots { };

//     void disconnect_sub_graph(Index i);

//     template <typename T>
//     [[nodiscard]] Index create_impl(sc::lex::SourceCodeRange range, nodes::Edges h, T&& maybe_var);

//     // Appends children to nodes at initialisation time.
//     // These must have already been type checked.
//     void append_unchecked(OptionalIndex parent, OptionalIndex child);

//     template <class EnterNode, class BeforeChildren, class AfterChildren, class ExitNode>
//     void depth_first_traverse_impl(EnterNode& enter_node, BeforeChildren& before_children,
//                                    AfterChildren& after_children, ExitNode& exit_node, size_t i, size_t depth = 0,
//                                    size_t visited = 0);
// };

// ////////////////////////////////////////////////////////////////////////////////
// //              Parser Graph implementations
// ////////////////////////////////////////////////////////////////////////////////


// template <typename To, NodeFlag... Froms, typename... ARGS>
// auto NodeGraph::cast(TypedIndex<Froms...> from_index, ARGS&&... args) -> typename To::Index {
//     static_assert(sizeof...(Froms) == 1, "Cannot deduce from type.");
//     using FromIndex = TypedIndex<Froms...>;
//     using ToIndex = typename To::Index;
//     using FromType = typename decltype(nodes::NodeCollection::get_node_type_from_index_type<FromIndex>())::type;
//     using ToType = To;

//     static_assert(ToType::type == FromType::type, "Types must be the same to do a node mutation.");

//     using FromTuple = typename FromType::HeldType;
//     using ToTuple = typename ToType::HeldType;
//     static_assert(std::is_convertible_v<FromTuple, ToTuple>, "Cannot convert children.");
//     payload(Index { *from_index }) = ToType { std::forward<ARGS>(args)... };
//     return ToIndex { *from_index };
// }


// template <NodeFlag... OLDs, NodeFlag... NEWs>
// TypedIndex<OLDs...> NodeGraph::substitute(TypedIndex<OLDs...> old, TypedIndex<NEWs...> n) {
//     using OldIndex = TypedIndex<OLDs...>;
//     using NewIndex = TypedIndex<NEWs...>;
//     static_assert(std::is_convertible_v<NewIndex, OldIndex>, "The new index cannot be used to replace the old.");

//     if (auto cc = m_edges[*old].first_child) {
//         std::vector<Index> to_delete;
//         depth_first_traverse(cc, [&](Index d) { to_delete.push_back(d); });
//         for (auto d : to_delete) {
//             m_edges[*d] = { };
//             m_dead_nodes.push_back(*d);
//         }
//     }

//     m_edges[*old].first_child = m_edges[*n].first_child;
//     m_payloads[*old] = std::move(m_payloads[*n]);
// }

// template <NodeFlag... Ps, NodeFlag... Cs>
// TypedIndex<Ps...> NodeGraph::merge_list(TypedIndex<Ps...> parent, TypedIndex<Cs...> child_list) {
//     using ParentIndex = TypedIndex<Ps...>;
//     using ChildIndex = TypedIndex<Cs...>;
//     using ParentType = typename decltype(nodes::NodeCollection::get_node_type_from_index_type<ParentIndex>())::type;
//     using ChildType = typename decltype(nodes::NodeCollection::get_node_type_from_index_type<ChildIndex>())::type;

//     static_assert(ChildType::is_list(), "Child must be a list type");
//     static_assert(ParentType::is_list(), "Parent must be a list type");

//     using ChildHeld = typename ChildType::ChildIndex;
//     using ParentHeld = typename ParentType::ChildIndex;

//     static_assert(std::is_convertible_v<ChildHeld, ParentHeld>, "The parent cannot hold all the possible child
//     types.");

//     auto& child_list_edges = m_edges[*child_list];

//     append_unchecked(parent, child_list_edges.first_child);

//     child_list_edges = { };
//     m_dead_nodes.push_back(*child_list);

//     return parent;
// }


// template <typename F> void NodeGraph::traverse_only_children(F&& f, Index i) {
//     const auto& edge = m_edges[*i];
//     if (edge.first_child) {
//         for (OptionalIndex c = edge.first_child; c; c = m_edges[*c].next_sibling) {
//             f(m_locations[*c], m_edges[*c], m_payloads[*c], Index { *c });
//         }
//     }
// }

// template <class EnterNode, class BeforeChildren, class AfterChildren, class ExitNode>
// void NodeGraph::depth_first_traverse(Index i, EnterNode&& enter_node, BeforeChildren&& before_children,
//                                      AfterChildren&& after_children, ExitNode&& exit_node) {
//     if (m_edges.empty())
//         return;
//     depth_first_traverse_impl(enter_node, before_children, after_children, exit_node, *i, 0);
// }

// // Mostly useful for debugging purposes.
// template <typename F> void NodeGraph::flat_walk(F&& f) const {
//     const auto sz = m_edges.size();
//     for (size_t i { 0 }; i < sz; ++i) {
//         f(m_locations[i], m_edges[i], m_payloads[i], i);
//     }
// }

// template <typename TypedIndexT> [[nodiscard]] bool NodeGraph::is_a(Index i) const noexcept {
//     using PayloadT = typename decltype(nodes::NodeCollection::get_node_type_from_index_type<TypedIndexT>())::type;
//     return std::get_if<PayloadT>(&m_payloads[*i]);
// }

// template <NodeFlag... Ts> [[nodiscard]] const auto& NodeGraph::payload(TypedIndex<Ts...> index) const {
//     using PayloadT = typename
//     decltype(nodes::NodeCollection::get_node_type_from_index_type<TypedIndex<Ts...>>())::type; return
//     std::get<PayloadT>(m_payloads[*index]);
// }

// template <NodeFlag... Ts> [[nodiscard]] auto& NodeGraph::payload(TypedIndex<Ts...> index) {
//     using PayloadT = typename
//     decltype(nodes::NodeCollection::get_node_type_from_index_type<TypedIndex<Ts...>>())::type; return
//     std::get<PayloadT>(m_payloads[*index]);
// }

// template <NodeFlag... Ps, NodeFlag... Cs>
// TypedIndex<Ps...> NodeGraph::prepend_to_list(TypedIndex<Ps...> list, TypedIndex<Cs...> item) {
//     using ParentIndex = TypedIndex<Ps...>;
//     using ChildIndex = TypedIndex<Cs...>;
//     using ParentType = typename decltype(nodes::NodeCollection::get_node_type_from_index_type<ParentIndex>())::type;
//     static_assert(std::is_constructible_v<typename ParentType::ChildIndex, ChildIndex>,
//                   "Attempting to add the wrong child type.");

//     auto& list_edges = m_edges[*list];
//     if (!list_edges.first_child) {
//         append_unchecked(*list, item);
//         return list;
//     }

//     const auto old_first_child = list_edges.first_child;
//     auto& first_child_edges = m_edges[*old_first_child];

//     // re-parent
//     for (OptionalIndex c = item; c; c = m_edges[*c].next_sibling) {
//         m_edges[*c].parent = list;
//     }

//     const auto last_index = first_child_edges.last_sibling;
//     first_child_edges.last_sibling = { }; // delete

//     const auto item_last = m_edges[*item].last_sibling ? m_edges[*item].last_sibling : item;
//     m_edges[*item].last_sibling = last_index;

//     m_edges[*item_last].next_sibling = old_first_child;
//     m_edges[*old_first_child].previous_sibling = item_last;

//     m_edges[*list].first_child = item;

//     return list;
// }

// template <NodeFlag... Ps, NodeFlag... Cs>
// TypedIndex<Ps...> NodeGraph::append_to_list(TypedIndex<Ps...> p, TypedIndex<Cs...> c) {
//     using ParentIndex = TypedIndex<Ps...>;
//     using ChildIndex = TypedIndex<Cs...>;
//     using ParentType = typename decltype(nodes::NodeCollection::get_node_type_from_index_type<ParentIndex>())::type;
//     static_assert(std::is_constructible_v<typename ParentType::ChildIndex, ChildIndex>,
//                   "Attempting to add the wrong child type.");

//     append_unchecked(*p, c);
//     return p;
// }

// template <NodeFlag... Ps, typename... Cs>
// TypedIndex<Ps...> NodeGraph::append_to_list(TypedIndex<Ps...> p, sc::lex::SourceCodeRange range, Cs... cs) {
//     m_locations[*p] = range;
//     (append_to_list(p, cs), ...);
//     return p;
// }

// template <typename Node, class... CHILDREN>
// [[nodiscard]] auto NodeGraph::create(Node n, sc::lex::SourceCodeRange loc, CHILDREN... children) {
//     static_assert(nodes::NodeCollection::has_node<typename Node::Index>(),
//                   "You have failed to add the node to the variant at the bottom of the file.");
//     static_assert(!(std::is_convertible_v<CHILDREN, LexerToken> || ...),
//                   "You have passed a lexer token instead of a grammar rule, you have used the wrong index to $N.");


//     static_assert(Node::template constructible_from<CHILDREN...>());
//     const auto i = typename Node::Index { create_impl(loc, { }, std::move(n)) };
//     (append_unchecked(i, children), ...);
//     return i;
// }

// // This is untyped. Instead use the IRNode's create method (it's a friend class).
// template <typename T> Index NodeGraph::create_impl(sc::lex::SourceCodeRange range, nodes::Edges h, T&& maybe_var) {
//     assert(m_locations.size() == m_edges.size());
//     assert(m_locations.size() == m_payloads.size());

//     if (m_dead_nodes.empty()) {
//         const auto i = m_edges.size();
//         m_locations.push_back(range);
//         m_edges.push_back(h);
//         m_payloads.push_back(nodes::NodeVariant { maybe_var });
//         return Index { static_cast<Index::Underlying>(i) };
//     } else {
//         const auto i = m_dead_nodes.back();
//         m_dead_nodes.pop_back();

//         m_locations[i] = range;
//         m_edges[i] = h;
//         m_payloads[i] = nodes::NodeVariant { maybe_var };
//         return Index { i };
//     }
// }

// template <class EnterNode, class BeforeChildren, class AfterChildren, class ExitNode>
// void NodeGraph::depth_first_traverse_impl(EnterNode& enter_node, BeforeChildren& before_children,
//                                           AfterChildren& after_children, ExitNode& exit_node, size_t i, size_t depth,
//                                           size_t visited) {
//     assert(visited < 999'999'999); // just a silly number to make sure we don't get stuck in a loop

//     const auto& edge = m_edges[i];

//     const auto call = [&](auto& f) {
//         using F = decltype(f);
//         if constexpr (std::is_invocable_v<F, const sc::lex::SourceCodeRange&, const nodes::NodeVariant&, size_t>) {
//             f(m_locations[i], m_payloads[i], depth);
//         } else {
//             f();
//         }
//     };

//     call(enter_node);

//     if (edge.first_child) {
//         call(before_children);
//         depth_first_traverse_impl(enter_node, before_children, after_children, exit_node, *edge.first_child, depth +
//         1,
//                                   visited + 1);
//         call(after_children);
//     }

//     call(exit_node);

//     // last sibling is only valid for the first node.

//     if (edge.next_sibling)
//         depth_first_traverse_impl(enter_node, before_children, after_children, exit_node, *edge.next_sibling, depth,
//                                   visited + 1);
// }
}
