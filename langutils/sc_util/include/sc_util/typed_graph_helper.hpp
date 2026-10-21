#pragma once

#include <array>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <variant>
namespace sc::util::typed_graph {

template <class T> struct TypeWrapper { using type = T; };


struct TypedGraphHelperBase {};

template <typename ENUM, template <ENUM...> typename INDEXCREATOR, typename... Nodes>
struct TypedGraphHelper : TypedGraphHelperBase {
    /*
    static_assert((std::is_base_of_v<NodeBaseBase, Nodes> && ...));
    */
    using Variant = std::variant<Nodes...>;
    using Tuple = std::tuple<Nodes...>;
    using IndexSequence = std::make_index_sequence<sizeof...(Nodes)>;
    using UntypedStrongIndex = typename std::tuple_element_t<0, Tuple>::Index::UnderlyingIndex;

    // template <typename N> [[nodiscard]] static constexpr bool check_enum_appears_once() {
    //     return ((N::Index::Possible[0] == Nodes::Index::Possible[0] ? 1 : 0) + ...) == 1;
    // }

    // static_assert(((Nodes::Index::size_of_set == 1) && ...),
    //               "Each node must have an index type that contains a single enum entry.");
    //  This is really slow O(N^2), but should be true.
    //  static_assert((check_enum_appears_once<Nodes>() && ...), "Some nodes produce the same typed index.");

    using UnderlyingEnum = ENUM;
    template <UnderlyingEnum... Es> using IndexCreator = INDEXCREATOR<Es...>;

    // The enum of each node, in the same order as the variant, so Variant::index() can be turned into the enum.
    static constexpr std::array<ENUM, sizeof...(Nodes)> node_enums { Nodes::Index::Possible[0]... };

    template <typename E, E EValue> [[nodiscard]] static constexpr auto node_type_from_enum() {
        return node_type_at<position_of_enum(EValue)>();
    }

    template <class ToFind> [[nodiscard]] static constexpr bool has_node() {
        return position_of_index<std::remove_reference_t<std::remove_cv_t<ToFind>>>() < sizeof...(Nodes);
    }

    template <ENUM E> [[nodiscard]] static constexpr bool has_node_with_enum() {
        return position_of_index<IndexCreator<E>>() < sizeof...(Nodes);
    }

    template <class IndexT> constexpr static auto get_node_type_from_index_type() {
        // static_assert(IndexT::size_of_set == 1);
        return node_type_at<position_of_index<IndexT>()>();
    }

    template <typename F> constexpr static auto all_node_types() { return (F().template operator()<Nodes>() && ...); }

    template <typename F> constexpr static auto at_least_node_types() {
        return (F().template operator()<Nodes>() || ...);
    }

    // These are folds rather than recursion, as recursing over every node, for every lookup, is slow to compile.
    // The position of the node in the variant, returns sizeof...(Nodes) if not found.
    template <class Target> [[nodiscard]] static constexpr std::size_t position_of_index() {
        std::size_t position = 0;
        const bool found = ((std::is_same_v<typename Nodes::Index, Target> ? true : (++position, false)) || ...);
        return found ? position : sizeof...(Nodes);
    }

protected:
    [[nodiscard]] static constexpr std::size_t position_of_enum(ENUM target) {
        for (std::size_t i = 0; i < sizeof...(Nodes); ++i)
            if (node_enums[i] == target)
                return i;
        return sizeof...(Nodes);
    }

    // Returns void if not found, so callers fail by substitution, rather than a hard error.
    template <std::size_t Position> [[nodiscard]] static constexpr auto node_type_at() {
        if constexpr (Position < sizeof...(Nodes))
            return TypeWrapper<std::tuple_element_t<Position, Tuple>> {};
    }
};
}