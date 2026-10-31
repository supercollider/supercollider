// Copyright Jordan Henderson 2026
#pragma once
#include "sc_source/text_location.hpp"
#include "sc_source/text_info.hpp"
#include <tuple>
#include <memory>

namespace sc::ast {

struct ASTGraph;

/// @name Parsing
/// Each overload returns the graph, and whether the parser accepted the input. Errors are recovered from, so input
/// with errors is usually accepted, check the graph's diagnostics (ASTGraph::diagnostics) for them.
/// @{

/// @brief Parses only the given range of the text, e.g., one snippet of a larger file.
[[nodiscard]] std::tuple<std::shared_ptr<ASTGraph>, bool> parse(std::shared_ptr<const sc::src::TextInfo> text,
                                                                sc::src::SourceCodeRange range);
/// @brief Parses from a location until the end of the text.
[[nodiscard]] std::tuple<std::shared_ptr<ASTGraph>, bool> parse(std::shared_ptr<const sc::src::TextInfo> text,
                                                                sc::src::SourceCodeLocation start);
/// @brief Parses the whole text.
[[nodiscard]] std::tuple<std::shared_ptr<ASTGraph>, bool> parse(std::shared_ptr<const sc::src::TextInfo> text);
/// @}
}
