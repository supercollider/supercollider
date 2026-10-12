#pragma once

#include "type_set_index.hpp"
#include <tuple>
#include <type_traits>


/*

The classes defined here are used to define the children nodes have.
Because we don't have concepts, the template arguments are constrained by static_asserts and sfinae.

*/

namespace sc::util::typed_graph {


enum struct NodeType { Node, Terminal, List };

// This is just used so you can check you have nodes in template parameters.
struct NodeBaseBase {};

// clang-format off
template <
    typename ThisTypedIndex, 
    NodeType T, 
    typename = std::enable_if<typed_index::is_type_set_index<ThisTypedIndex>()>
>
// clang-format on
struct NodeBase : NodeBaseBase {
    static_assert(ThisTypedIndex::size_of_set == 1, "Nodes must be represented by indexes that have exactly one enum.");
    using Index = ThisTypedIndex;
    static constexpr NodeType type { T };
    [[nodiscard]] static constexpr bool is_list() { return type == NodeType::List; }
    [[nodiscard]] static constexpr bool is_node() { return type == NodeType::Node; }
    [[nodiscard]] static constexpr bool is_terminal() { return type == NodeType::Terminal; }
};

/**
@brief A node with no children.
@tparam ThisTypedIndex must be the type_set_index this node produces.
*/
template <typename ThisTypedIndex> struct TerminalNode : public NodeBase<ThisTypedIndex, NodeType::Terminal> {
    static constexpr auto number_of_children { 0 };
    using HeldType = void;
    static constexpr std::optional<size_t> number_expected_args {};

    template <typename... ARGS> [[nodiscard]] static constexpr bool constructible_from() {
        return sizeof...(ARGS) == 0;
    }
};

template <class ThisTypedIndex, typename... CHILD_INDEXES> struct Node;

/**
@brief A node with a specific number of children each with a specified type.
@tparam ThisTypedIndex must be the type_set_index this node produces.
@tparam CHILD_INDEXES... is all the typed indexs of the children.

This is defined in function signature syntax.

struct MyNode : Node<MyNodeIndex(FirstChildIndex, SecondChildIndex)> {};

*/
template <class ThisTypedIndex, typename... CHILD_INDEXES>
struct Node<ThisTypedIndex(CHILD_INDEXES...)> : public NodeBase<ThisTypedIndex, NodeType::Node> {
    static_assert(sizeof...(CHILD_INDEXES) > 0, "Use TerminalNode instead.");
    static_assert((typed_index::is_type_set_index<CHILD_INDEXES>() && ...),
                  "TypedIndexs can only have constructors with other indexes in them, you have probably passed the "
                  "node type, not its index.");
    static constexpr auto number_of_children { sizeof...(CHILD_INDEXES) };
    static constexpr std::optional<size_t> number_expected_args { sizeof...(CHILD_INDEXES) };
    using ChildrenIndexTupleType = std::tuple<CHILD_INDEXES...>;
    using HeldType = ChildrenIndexTupleType;

    template <typename... ARGS> [[nodiscard]] static constexpr bool constructible_from() {
        if (sizeof...(ARGS) != number_of_children)
            return false;
        return std::is_constructible_v<ChildrenIndexTupleType, ARGS...>;
    }
};

/**
@brief A node with an unspecified number of children each with the same type.
@tparam ThisTypedIndex must be the type_set_index this node produces.
@tparam ChildIndexT is the typed indexs of the child.
*/
template <class ThisTypedIndex, typename ChildIndexT>
struct ListNode : public NodeBase<ThisTypedIndex, NodeType::List> {
    using ChildIndex = ChildIndexT;
    using HeldType = ChildIndex;
    static constexpr std::optional<size_t> number_expected_args {};

    template <typename... ARGS> [[nodiscard]] static constexpr bool constructible_from() {
        return sizeof...(ARGS) == 0 || (std::is_convertible_v<ARGS, ChildIndexT> && ...);
    }
};


}
