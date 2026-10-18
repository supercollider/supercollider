#pragma once

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

    template <typename N> [[nodiscard]] static constexpr bool check_enum_appears_once() {
        return ((N::Index::Possible[0] == Nodes::Index::Possible[0] ? 1 : 0) + ...) == 1;
    }

    // static_assert(((Nodes::Index::size_of_set == 1) && ...),
    //               "Each node must have an index type that contains a single enum entry.");
    //  This is really slow O(N^2), but should be true.
    //  static_assert((check_enum_appears_once<Nodes>() && ...), "Some nodes produce the same typed index.");

    using UnderlyingEnum = ENUM;
    template <UnderlyingEnum... Es> using IndexCreator = INDEXCREATOR<Es...>;

    template <typename E, E EValue> [[nodiscard]] static constexpr auto node_type_from_enum() {
        return node_type_from_enum_impl<0, E, EValue>();
    }

    template <class ToFind> [[nodiscard]] static constexpr bool has_node() {
        return has_node_impl<0, std::remove_reference_t<std::remove_cv_t<ToFind>>>();
    }

    template <ENUM E> [[nodiscard]] static constexpr bool has_node_with_enum() {
        return has_node_impl<0, IndexCreator<E>>();
    }

    template <class IndexT> constexpr static auto get_node_type_from_index_type() {
        // static_assert(IndexT::size_of_set == 1);
        return get_node_type_from_index_type_impl<0, IndexT>();
    }

    template <typename F> constexpr static auto all_node_types() { return (F().template operator()<Nodes>() && ...); }

    template <typename F> constexpr static auto at_least_node_types() {
        return (F().template operator()<Nodes>() || ...);
    }

protected:
    // For some reason clang-format confuses clang-d (funny since they are both clang!)
    // clang-format off
    template <std::size_t CurrentI, class Target, typename = std::enable_if<CurrentI < std::tuple_size_v<Tuple>>>
    [[nodiscard]] constexpr static auto get_node_type_from_index_type_impl()  {
        // clang-format on
        // static_assert(CurrentI < sizeof...(Nodes));
        if constexpr (CurrentI < sizeof...(Nodes)) {
            using T = std::tuple_element_t<CurrentI, Tuple>;
            if constexpr (std::is_same_v<typename T::Index, Target>) {
                return TypeWrapper<T> {};
            } else {
                return get_node_type_from_index_type_impl<CurrentI + 1, Target>();
            }
        }
    }

    // clang-format off
    template <std::size_t CurrentI, class Enum, Enum Target, typename = std::enable_if<CurrentI < sizeof ... (Nodes)>>
    [[nodiscard]] constexpr static auto node_type_from_enum_impl()  {
        // clang-format on
        // static_assert(CurrentI < sizeof...(Nodes));
        if constexpr (CurrentI < sizeof...(Nodes)) {
            using CurrentT = std::tuple_element_t<CurrentI, Tuple>;
            if constexpr (CurrentT::Index::Possible[0] == Target) {
                return TypeWrapper<CurrentT> {};
            } else {
                return node_type_from_enum_impl<CurrentI + 1, Enum, Target>();
            }
        }
    }

    // clang-format off
    template <std::size_t CurrentI, class ToFind, typename = std::enable_if<CurrentI<sizeof...(Nodes)>> 
    [[nodiscard]] constexpr static bool has_node_impl()  {
        // clang-format on
        // static_assert(CurrentI < sizeof...(Nodes));
        if constexpr (CurrentI < sizeof...(Nodes)) {
            using CurrentT = std::tuple_element_t<CurrentI, Tuple>;
            if constexpr (std::is_same_v<typename CurrentT::Index, ToFind>) {
                return true;
            } else if constexpr (CurrentI + 1 < sizeof...(Nodes)) {
                return has_node_impl<CurrentI + 1, ToFind>();
            } else {
                return false;
            }
        }
    }
};
}