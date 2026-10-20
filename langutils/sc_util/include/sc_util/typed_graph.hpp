#pragma once

#include "type_set_index.hpp"
#include <algorithm>
#include <functional>
#include <tuple>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

namespace sc::util::typed_graph {


struct IsTrivial {
    template <typename T> [[nodiscard]] constexpr bool operator()() const {
        return std::is_trivially_move_constructible_v<T> && std::is_trivially_destructible_v<T>
            && std::is_nothrow_move_constructible_v<T> && std::is_trivially_move_assignable_v<T>
            && std::is_nothrow_move_assignable_v<T>;
    }
};


template <typename OptNodeIndex, //
          typename Edges, //
          typename T //
          > //
struct Iter {
    // static_assert(std::is_integral_v<typename OptNodeIndex::Underlying>);
    // static_assert(std::is_constructible_v<bool, OptNodeIndex>);
    constexpr Iter(OptNodeIndex current, const std::vector<Edges>& edge): m_current(current), m_edges(edge) { }

    Iter& operator+=(int offset);
    [[nodiscard]] Iter operator++(int);
    Iter& operator++() { return this->operator+=(1); }
    [[nodiscard]] constexpr explicit operator bool() const { return static_cast<bool>(m_current); }

    [[nodiscard]] constexpr T operator*() const;

private:
    OptNodeIndex m_current;
    const std::vector<Edges>& m_edges;
};

template <typename OptNodeIndex, typename Edges, typename T> //
struct Container {
    constexpr Container(const std::vector<Edges>& edges, OptNodeIndex first): m_edges(edges), m_first(first) { }
    [[nodiscard]] Iter<OptNodeIndex, Edges, T> iter() const { return { m_first, m_edges }; }

private:
    const std::vector<Edges>& m_edges;
    OptNodeIndex m_first;
};

// Used when destructing children from a node.
struct NoChildren { };

template <typename OptIndex> struct EdgesTemplate { OptIndex first_child, parent, next_sibling, prev_sibling; };

/**
@brief
Essentially this is linked list but done with indicies.
Additionally, all the node construction is heavily typed.
Was this is setup, aside from the basic accessors, you only need 3 methods, create, visit, and children.
This is done in a struct of vectors approach.

This is designed to be inherited from so you can add your own implementation and expose protected methods with the using
declaration.

@param OptID the optional index type.
@param ID the index type.
@param A specialization of TypedGraphHelper.

*/


template <typename OptIndex, //
          typename Index, //
          typename HELPER //
          >
struct GraphImplementation {
public: // This is meant to be privately inherited from.
    ////////////////////////////////////////////////////////////////////////////////
    // static_assert(HELPER::template all_node_types<IsTrivial>(), "Nodes should not allocate or require destructors.");
    // static_assert(std::is_same_v<typename HELPER::UntypedStrongIndex, OptIndex>
    //                  || std::is_same_v<typename HELPER::UntypedStrongIndex, Index>,
    //             "Index types aren't the ones the nodes use.");

    template <typename OptNodeIndex, typename Edges, typename T> friend struct Container;
    template <typename OptNodeIndex, typename Edges, typename T> friend struct Iter;

    using Edges = EdgesTemplate<OptIndex>;

    ////////////////////////////////////////////////////////////////////////////////
    using UntypedNodeIndex = typename HELPER::UntypedStrongIndex;
    using Variant = typename HELPER::Variant;
    using Helper = HELPER;

    template <typename I>
    using NodeTypeFromIndex = typename decltype(Helper::template get_node_type_from_index_type<I>())::type;

    // Unfortunately, some times the node indexes need to be optional.
    static constexpr bool NodesAreOptional = std::is_same_v<UntypedNodeIndex, OptIndex>;

    // Additionally, the last_sibling is stored in a separate header as it is only needed when inserting, not when
    // iterating.


    ////////////////////////////////////////////////////////////////////////////////
    // The data.
    ////////////////////////////////////////////////////////////////////////////////

    std::vector<UntypedNodeIndex> m_dead_nodes {};
    std::vector<OptIndex> m_last_child { };
    std::vector<Index> m_orphans { };
    // One entry per node.
    std::vector<Edges> m_node_edges { };
    std::vector<typename Helper::Variant> m_node_payload { };

    ////////////////////////////////////////////////////////////////////////////////
    // Create
    ////////////////////////////////////////////////////////////////////////////////

    /**
    @brief How nodes are made. This will return the typed index. If you have extra data, use the index to figure out
    where to insert it.
    @tparam Node A node,
    @tparam  ...Children the typed children indexs
     */
    template <typename Node, typename... ChildrenIndexes> //
    [[nodiscard]] typename Node::Index create(Node&& node, ChildrenIndexes&&... child_indexes) {
        assert((not_dead(child_indexes) && ...));
        /*
        static_assert((typed_index::is_type_set_index<ChildrenIndexes>() && ...),
                      "You have not passed an index to the constructor of a node, you have probably passed the node "
                      "rather than its index.");
        static_assert(typed_index::is_type_set_index<typename Node::Index>());
        static_assert(Helper::template has_node<typename Node::Index>(),
                      "You have either called create on a graph with the wrong type of node, or have failed to add the "
                      "node to the template argument.");
        static constexpr auto expected_args = Node::number_expected_args;

        static_assert(!expected_args || sizeof...(child_indexes) <= *expected_args,
                      "Too many arguments have been passed to the constructor of a node.");
        static_assert(!expected_args || sizeof...(child_indexes) >= *expected_args,
                      "Too few arguments have been passed to the constructor of a node.");
        static_assert(Node::template constructible_from<ChildrenIndexes...>(),
                      "The children index types do not match the constructor");
                      */
        const typename Node::Index index = [&]() {
            if (m_dead_nodes.empty()) {
                const auto index_raw = m_node_payload.size();
                m_node_payload.push_back(std::move(node));
                m_node_edges.push_back({ });
                m_last_child.push_back({ });
                return index_raw;
            } else {
                const size_t index_raw = *m_dead_nodes.back();
                m_dead_nodes.pop_back();
                m_node_payload[index_raw] = std::move(node);
                m_node_edges[index_raw] = { };
                m_last_child[index_raw] = { };
                return index_raw;
            }
        }();

        m_orphans.push_back(Index { *index });

        // Just call the unchecked version, but if the child is invalid, and we are allowed invalid nodes, it will skip
        // it.
        const auto append_node = [&](auto c) {
            if constexpr (NodesAreOptional) {
                if constexpr (Node::is_list()) {
                    if (c) {
                        append_to_parent_unchecked(index, c);
                    }
                } else {
                    append_to_parent_unchecked(index, c);
                }
            } else {
                append_to_parent_unchecked(index, c);
            }
        };

        (append_node(std::forward<ChildrenIndexes>(child_indexes)), ...);
        (void)append_node; // warnings get triggered without this, it can't see this is variadic.

        return index;
    }

    /// Returns set of currently orphaned nodes.
    /// This does a copy
    [[nodiscard]] std::vector<Index> orphans() const { return m_orphans; }

    ////////////////////////////////////////////////////////////////////////////////
    // accessors
    ////////////////////////////////////////////////////////////////////////////////

    template <typename TypedIndex, typename = std::enable_if<typed_index::is_type_set_index<TypedIndex>()>>
    [[nodiscard]] constexpr NodeTypeFromIndex<TypedIndex>& payload(TypedIndex i) {
        // assert(not_dead(i));
        return std::get<NodeTypeFromIndex<TypedIndex>>(m_node_payload[*i]);
    }

    template <typename TypedIndex, typename = std::enable_if<typed_index::is_type_set_index<TypedIndex>()>>
    [[nodiscard]] constexpr const auto& payload(TypedIndex i) const {
        // assert(not_dead(i));
        using R = typename decltype(Helper::template get_node_type_from_index_type<TypedIndex>())::type;
        return std::get<R>(m_node_payload[*i]);
    }

    [[nodiscard]] constexpr typename Helper::Variant& payload(UntypedNodeIndex i) {
        // assert(not_dead(i));
        return m_node_payload[*i];
    };

    [[nodiscard]] constexpr const typename Helper::Variant& payload(UntypedNodeIndex i) const {
        // assert(not_dead(i));
        return m_node_payload[*i];
    }

    [[nodiscard]] constexpr Edges& edges(UntypedNodeIndex i) {
        // assert(not_dead(i));
        return m_node_edges[*i];
    }
    [[nodiscard]] constexpr const Edges& edges(UntypedNodeIndex i) const {
        // assert(not_dead(i));
        return m_node_edges[*i];
    }

    [[nodiscard]] constexpr auto last_child(UntypedNodeIndex i) const {
        // assert(not_dead(i));
        return m_last_child[*i];
    }

    [[nodiscard]] constexpr size_t num_children(UntypedNodeIndex i) const {
        const Edges& edges = m_node_edges[*i];
        size_t count { 0 };
        for (auto c = edges.first_child; c; c = m_node_edges[*c].next_sibling) {
            count += 1;
        }
        return count;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // Checks type of untyped node.
    ////////////////////////////////////////////////////////////////////////////////
    template <typename TypedIndex> [[nodiscard]] constexpr bool is_a(UntypedNodeIndex i) const {
        // assert(not_dead(i));
        return std::get_if<NodeTypeFromIndex<TypedIndex>>(&m_node_payload[*i]) != nullptr;
    }


    template <typename TypedIndex> [[nodiscard]] constexpr std::optional<TypedIndex> as_a(UntypedNodeIndex i) const {
        // assert(not_dead(i));


        if (std::get_if<NodeTypeFromIndex<TypedIndex>>(&m_node_payload[*i])) {
            return { TypedIndex { *i } };
        } else {
            return std::nullopt;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Return children of a node in a structured way.
    ////////////////////////////////////////////////////////////////////////////////
    template <typename TypedIndex> [[nodiscard]] constexpr auto children(TypedIndex i) const {
        // assert(not_dead(i));
        //  static_assert(TypedIndex::size_of_set == 1);
        using N = NodeTypeFromIndex<TypedIndex>;
        if constexpr (N::is_terminal())
            return NoChildren { };
        else if constexpr (N::is_node()) {
            using Tup = typename N::ChildrenIndexTupleType;
            return build_node_children_tuple<Tup>(i, std::make_index_sequence<std::tuple_size_v<Tup>>());
        } else {
            return Container<OptIndex, Edges, typename N::HeldType> { m_node_edges, m_node_edges[*i].first_child };
        }
    }

    template <typename TypedIndex> [[nodiscard]] constexpr auto index_to_variant(TypedIndex i) const {
        // assert(not_dead(i));
        return std::visit([&](const auto& node) { return GraphImplementation::as_variant(i, node); },
                          m_node_payload[*i]);
    }


    template <typename Action, typename TypedIndex> auto visit(Action action, TypedIndex i) const {
        // assert(not_dead(i));
        return std::visit(
            action,
            std::visit([&](const auto& node) { return GraphImplementation::as_variant(i, node); }, m_node_payload[*i]));
    }

    template <size_t N, typename TypedIndex> //
    [[nodiscard]] std::optional<std::array<
        typename decltype(Helper::template get_node_type_from_index_type<TypedIndex>())::type::HeldType, N>> //
    list_unpack(TypedIndex i) const {
        using NodeType = typename decltype(Helper::template get_node_type_from_index_type<TypedIndex>())::type;
        // static_assert(NodeType::is_list());
        using Held = typename decltype(Helper::template get_node_type_from_index_type<TypedIndex>())::type::HeldType;
        if (num_children(i) != N)
            return {};
        return list_unpack_impl<Held>(i, std::make_index_sequence<N> {});
    }


    ////////////////////////////////////////////////////////////////////////////////
    // List operations, all assume the parent is node list type.
    ////////////////////////////////////////////////////////////////////////////////
    template <typename ParentI, typename ChildI> //
    ParentI append(ParentI p, ChildI c) {
        // assert(not_dead(p));
        // assert(not_dead(c));
        //  static_assert(typed_index::is_type_set_index<ParentI>());
        //  static_assert(typed_index::is_type_set_index<ChildI>());
        using Parent = typename decltype(Helper::template get_node_type_from_index_type<ParentI>())::type;
        // static_assert(Parent::is_list());
        // static_assert(std::is_convertible_v<ChildI, typename Parent::HeldType>);

        append_to_parent_unchecked(p, c);
        return p;
    }

    template <typename ParentI, typename ChildI> //
    ParentI prepend(ParentI p, ChildI child) {
        // assert(not_dead(p));
        // assert(not_dead(child));
        //  static_assert(typed_index::is_type_set_index<ParentI>());
        //  static_assert(typed_index::is_type_set_index<ChildI>());
        using Parent = NodeTypeFromIndex<ParentI>;
        // static_assert(Parent::is_list());
        // static_assert(std::is_convertible_v<ChildI, typename Parent::HeldType>);

        auto& parent_edges = m_node_edges[*p];

        // No existing children, same as append.
        if (!parent_edges.first_child) {
            append_to_parent_unchecked(p, child);
            return p;
        }

        const auto old_first_child = parent_edges.first_child;

        // Re-parent new sub graph.
        UntypedNodeIndex last_valid = child;
        for (OptIndex c = last_valid; c; c = m_node_edges[*c].next_sibling) {
            m_node_edges[*c].parent = p;
            if (auto fnd = std::find(m_orphans.begin(), m_orphans.end(), Index(*c)); fnd != m_orphans.end())
                m_orphans.erase(fnd);
            last_valid = *c;
        }

        m_node_edges[*last_valid].next_sibling = old_first_child;
        m_node_edges[*old_first_child].prev_sibling = last_valid;
        parent_edges.first_child = child;

        return p;
    }

    /**
        @brief After this call [eaten] will no longer be alive.
        [eaten] cannot have a parent assigned already.
    */
    template <typename Surviving, typename Eaten> //
    Surviving merge(Surviving s, Eaten e) {
        // assert(s != e);
        // assert(not_dead(s));
        // assert(not_dead(e));
        //  static_assert(typed_index::is_type_set_index<Surviving>());
        //  static_assert(typed_index::is_type_set_index<Eaten>());

        using SurvivingType = NodeTypeFromIndex<Surviving>;
        using EatenType = NodeTypeFromIndex<Eaten>;
        // static_assert(SurvivingType::is_list());
        // static_assert(EatenType::is_list());

        static_assert(std::is_convertible_v<typename EatenType::ChildIndex, typename SurvivingType::ChildIndex>);


        auto& eaten_edges = m_node_edges[*e];
        // assert(!eaten_edges.parent);
        if (auto c = eaten_edges.first_child)
            append_to_parent_unchecked(s, c);

        eaten_edges = { };
        m_last_child[*e] = {};


        if (auto fnd = std::find(m_orphans.begin(), m_orphans.end(), Index(*e)); fnd != m_orphans.end())
            m_orphans.erase(fnd);

        m_dead_nodes.push_back(e);

        return s;
    }

    template <typename To, typename FromIndex, typename... ARGS>
    typename To::Index cast(FromIndex from_index, ARGS&&... args) {
        using ToIndex = typename To::Index;
        using From = NodeTypeFromIndex<FromIndex>;
        // static_assert(From::type == To::type);
        // static_assert(std::is_convertible_v<typename From::HeldType, typename To::HeldType>);
        payload(UntypedNodeIndex { *from_index }) = To { std::forward<ARGS>(args)... };
        return ToIndex { *from_index };
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Traversal
    ////////////////////////////////////////////////////////////////////////////////


    template <typename F> //
    [[nodiscard]] static constexpr bool signature() {
        return std::is_invocable_v<F, const Edges&, const Variant&, Index, size_t>;
    }


    template <typename F, typename = std::enable_if<signature<F>()>> //
    void flat_walk(F f) const {
        const auto sz = m_node_payload.size();
        for (size_t i { 0 }; i < sz; ++i) {
            std::invoke(f, m_node_edges[i], m_node_payload[i], i);
        }
    }

    template <typename F, typename = std::enable_if<signature<F>()>> //
    void traverse_only_children(F f, Index i) const {
        const auto& edge = m_node_edges[i];
        if (auto start = edge.first_child) {
            for (OptIndex c = start; c; c = m_node_edges[*c].next_sibling) {
                f(m_node_edges[*c], m_node_payload[*c], Index { *c });
            }
        }
    }

    struct Default {
        void operator()(const Edges&, const Variant&, Index, size_t) {}
    };

    template <typename EnterNode = Default, typename BeforeChildren = Default, typename AfterChildren = Default,
              typename ExitNode = Default,
              typename = std::enable_if<signature<EnterNode>()>, //
              typename = std::enable_if<signature<BeforeChildren>()>, //
              typename = std::enable_if<signature<AfterChildren>()>, //
              typename = std::enable_if<signature<ExitNode>()> //
              > //
    void depth_first_traverse(Index i, EnterNode enter_node = {}, BeforeChildren before_children = {},
                              AfterChildren after_children = {}, ExitNode exit_node = {}) const {
        size_t visited { 0 };
        depth_first_traverse_impl(enter_node, before_children, after_children, exit_node, *i, visited);
    }


    [[nodiscard]] std::optional<Index> debug_find_loop() const {
        std::unordered_set<Index> visited;

        try {
            depth_first_traverse(Index::from(m_node_edges.size() - 1),
                                 [&](const Edges&, const Variant&, Index i, size_t) {
                                     if (visited.count(i) != 0) {
                                         throw i;
                                     }
                                     visited.insert(i);
                                 });
        } catch (Index i) { return { i }; }
        return {};
    }

private:
    template <typename I, size_t... Ns>
    std::array<I, sizeof...(Ns)> list_unpack_impl(UntypedNodeIndex i, std::index_sequence<Ns...>) const {
        auto c = m_node_edges[*i].first_child;
        const auto next = [&](size_t) {
            assert(c);
            auto out = c;
            c = m_node_edges[*c].next_sibling;
            return out;
        };

        return { next(Ns)... };
    }

    [[nodiscard]] bool not_dead(UntypedNodeIndex i) const {
        return std::find(m_dead_nodes.begin(), m_dead_nodes.end(), i) == m_dead_nodes.end();
    }
    /// [child] can be either a single node, or a list.
    void append_to_parent_unchecked(UntypedNodeIndex parent_i, UntypedNodeIndex child_i) {
        assert(*parent_i != *child_i);
        auto& parent_edges = m_node_edges[*parent_i];

        if (auto final_child = m_last_child[*parent_i]) {
            auto& f_c_edges = m_node_edges[*final_child];
            f_c_edges.next_sibling = child_i;
            m_node_edges[*child_i].prev_sibling = final_child;
        } else {
            m_node_edges[*child_i].prev_sibling = {};
            parent_edges.first_child = child_i;
        }

        UntypedNodeIndex last_valid = child_i;
        for (OptIndex c = last_valid; c; c = m_node_edges[*c].next_sibling) {
            m_node_edges[*c].parent = parent_i;
            if (auto fnd = std::find(m_orphans.begin(), m_orphans.end(), Index(*c)); fnd != m_orphans.end())
                m_orphans.erase(fnd);
            last_valid = *c;
        }
        m_last_child[*parent_i] = last_valid;
    }

    template <typename Tup, size_t... IS>
    Tup build_node_children_tuple(UntypedNodeIndex parent, std::index_sequence<IS...>) const {
        std::array<UntypedNodeIndex, sizeof...(IS)> child_indexes;

        OptIndex rolling = m_node_edges[*parent].first_child;

        for (size_t i { 0 }; i < sizeof...(IS); ++i) {
            assert(rolling);
            child_indexes[i] = rolling;
            rolling = m_node_edges[*rolling].next_sibling;
        }

        return { std::tuple_element_t<IS, Tup>::from(child_indexes[IS])... };
    };

    template <typename EnterNode, typename BeforeChildren, typename AfterChildren, typename ExitNode,
              typename = std::enable_if<signature<EnterNode>()>, //
              typename = std::enable_if<signature<BeforeChildren>()>, //
              typename = std::enable_if<signature<AfterChildren>()>, //
              typename = std::enable_if<signature<ExitNode>()>>
    void depth_first_traverse_impl(EnterNode enter_node, BeforeChildren before_children, AfterChildren after_children,
                                   ExitNode exit_node, size_t i, size_t& visited, size_t depth = 0) const {
        assert(visited < 999'999'999);
        visited += 1;
        const auto call = [&](auto& f) { f(m_node_edges[i], m_node_payload[i], Index(i), depth); };

        const auto& edge = m_node_edges[i];

        call(enter_node);
        if (edge.first_child) {
            call(before_children);
            depth_first_traverse_impl(enter_node, before_children, after_children, exit_node, *edge.first_child,
                                      visited, depth + 1);
            call(after_children);
        }
        call(exit_node);

        if (edge.next_sibling)
            depth_first_traverse_impl(enter_node, before_children, after_children, exit_node, *edge.next_sibling,
                                      visited, depth);
    }

    template <typename TypedIndex, typename Node> [[nodiscard]] static auto as_variant(TypedIndex i, const Node&) {
        using NodeIndex = typename Node::Index;
        // static_assert(NodeIndex::size_of_set == 1);
        // static_assert(Helper::template has_node_with_enum<NodeIndex::Possible[0]>());
        return i.template as_variant<NodeIndex::Possible[0]>();
    }
};

////////////////////////////////////////////////////////////////////////////////
template <typename OptNodeIndex, typename Edges, typename T>
Iter<OptNodeIndex, Edges, T>& Iter<OptNodeIndex, Edges, T>::operator+=(int offset) {
    if (offset >= 0) {
        for (int i { 0 }; i < offset; ++i) {
            if (!m_current)
                break;
            m_current = m_edges[*m_current].next_sibling;
        }
    } else {
        for (int i { 0 }; i > offset; --i) {
            if (!m_current)
                break;
            m_current = m_edges[*m_current].prev_sibling;
        }
    }
    return *this;
}


template <typename OptNodeIndex, typename Edges, typename T>
[[nodiscard]] Iter<OptNodeIndex, Edges, T> Iter<OptNodeIndex, Edges, T>::operator++(int) {
    Iter copy = *this;
    if (m_current) {
        m_current = m_edges[*m_current].next_sibling;
    }
    return copy;
}

template <typename OptNodeIndex, typename Edges, typename T>
[[nodiscard]] constexpr T Iter<OptNodeIndex, Edges, T>::operator*() const {
    // assert(m_current);
    return T { *m_current };
}
}
