// Copyright Jordan Henderson 2026
#pragma once

#include "text.hpp"

#include "sc_format/format.hpp"

#include <optional>
#include <string_view>
#include <vector>

namespace sc::ast {
struct ASTGraph;
}

namespace sc::format {

struct VerbatimLines {
    std::vector<TextRange> ranges; // whole lines, in order and apart
    std::vector<LineRange> unparsed_lines; // around parse errors, see Text
};

// The lines of what the parser recovered from, the lines formatting is turned off for, and the lines outside the
// selection.
[[nodiscard]] VerbatimLines collect_verbatim_lines(const ast::ASTGraph& graph, std::string_view text,
                                                   std::vector<TextRange> unparsed_ranges,
                                                   std::optional<LineRange> selection);
}
