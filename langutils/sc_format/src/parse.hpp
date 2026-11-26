// Copyright Jordan Henderson 2026
#pragma once

#include "text.hpp"

#include "sc_format/format.hpp"

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace sc::ast {
struct ASTGraph;
}

namespace sc::format {

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

struct Parsed;

// Parses text, keeping the tokens.
[[nodiscard]] Parsed parse(std::string_view text, Kind kind);

////////////////////////////////////////////////////////////////////////////////
// Results
////////////////////////////////////////////////////////////////////////////////

struct Parsed {
    std::shared_ptr<ast::ASTGraph> graph; // none if the parser couldn't start
    bool accepted; // whether the parser reached the end, recovering from any errors
    std::optional<std::string> first_error;
    std::vector<TextRange> unparsed_ranges; // where the graph is a guess: Error nodes, added brackets, error locations
};

}
