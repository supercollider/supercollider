#pragma once
#include "sc_parser/indexes_untyped.hpp"
#include "sc_util/strong_index.hpp"
#include <cstdint>
#include <memory>
#include <optional>
namespace sc::ast {
class ASTGraph;
}

namespace sc::ir {

using ASTGraphIndex = util::StrongIndex<std::uint32_t, struct ASTGraphIndex__>;
struct ASTLocation {
    // Why not an index? That would work for the class library, but interpreted code might want to not store the ast
    // graph forever. That means we need a way to keep the graph alive while the ir graph is alive.
    // Why not store on ptr in the ir graph, we store all the information needed for a class in one ir graph, but
    // extention methods might be stored in separate ast graphs as they might be in different files.
    std::shared_ptr<const ast::ASTGraph> graph_index;
    ast::Index node_index;
};

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

}
