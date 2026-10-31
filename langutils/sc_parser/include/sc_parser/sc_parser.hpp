// Copyright Jordan Henderson 2026
#pragma once
#include "sc_source/text_location.hpp"
#include "sc_source/text_info.hpp"
#include <tuple>
#include <memory>

namespace sc::ast {

struct ASTGraph;

// The graph, and whether the parser accepted the input. Errors are recovered from, so input with errors is usually
// accepted, check the graph's diagnostics for them.
[[nodiscard]] std::tuple<std::shared_ptr<ASTGraph>, bool> parse(std::shared_ptr<const sc::src::TextInfo>,
                                                                sc::src::SourceCodeRange);
[[nodiscard]] std::tuple<std::shared_ptr<ASTGraph>, bool> parse(std::shared_ptr<const sc::src::TextInfo>,
                                                                sc::src::SourceCodeLocation);
[[nodiscard]] std::tuple<std::shared_ptr<ASTGraph>, bool> parse(std::shared_ptr<const sc::src::TextInfo>);
}
