#pragma once
#include "sc_lexer/text_location.hpp"
#include "sc_util/strong_index.hpp"
#include <cstdint>
#include <optional>

namespace sc::ir {

using ASTGraphIndex = util::StrongIndex<std::uint32_t, struct ASTGraphIndex__>;

// An index into an IRGraph's table of sources.
using SourceIndex = util::StrongIndex<std::uint32_t, struct SourceIndex__>;

// Where a node came from, as a range in one of the IRGraph's sources.
// This is plain data so the graph can be serialised, and so the ast can be freed once it has been lowered. It is only
// meaningful inside the IRGraph that owns it.
// Create and read these through the IRGraph, so the representation can change, e.g., to record macro expansions.
// The line and column are stored rather than recomputed, as the lexer's counting is subtle and would be duplicated.
struct SourceLocation {
    SourceIndex source;
    lex::SourceCodeRange range;
};

template <typename T> struct Locatable {
    SourceLocation location;
    T value;
    [[nodiscard]] constexpr T& operator->() { return value; }
    [[nodiscard]] constexpr const T& operator->() const { return value; }

    [[nodiscard]] constexpr T& operator*() { return value; }
    [[nodiscard]] constexpr const T& operator*() const { return value; }
};

template <typename T> struct OptionallyLocatable {
    std::optional<SourceLocation> location;
    T value;

    [[nodiscard]] constexpr T& operator->() { return value; }
    [[nodiscard]] constexpr const T& operator->() const { return value; }

    [[nodiscard]] constexpr T& operator*() { return value; }
    [[nodiscard]] constexpr const T& operator*() const { return value; }
};

}
