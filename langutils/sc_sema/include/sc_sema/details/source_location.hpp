#pragma once
#include "sc_sema/details/indexes.hpp"
#include "sc_source/text_location.hpp"
#include <optional>

namespace sc::ir {

// Where a node came from, as a range in one of the IRGraph's sources.
//
// This is plain data so the graph can be serialised, and so the ast can be freed once it has been lowered. It is only
// meaningful inside the IRGraph that owns it.
// Create and read these through the IRGraph, so the representation can change, e.g., to record macro expansions.
// The line and codepoint in line are stored rather than recomputed, as the lexer's counting is subtle and would be
// duplicated.
struct SourceLocation {
    SourceIndex source;
    src::SourceCodeRange range;
};

template <typename T> struct Locatable {
    SourceLocation location;
    T value;
    [[nodiscard]] constexpr T& operator->() { return value; }
    [[nodiscard]] constexpr const T& operator->() const { return value; }

    [[nodiscard]] constexpr T& operator*() { return value; }
    [[nodiscard]] constexpr const T& operator*() const { return value; }
};

// A value together with where it was written, if it was written, e.g., not for a generated value.
template <typename T> struct OptionallyLocatable {
    std::optional<SourceLocation> location;
    T value;

    [[nodiscard]] constexpr T& operator->() { return value; }
    [[nodiscard]] constexpr const T& operator->() const { return value; }

    [[nodiscard]] constexpr T& operator*() { return value; }
    [[nodiscard]] constexpr const T& operator*() const { return value; }
};

}
