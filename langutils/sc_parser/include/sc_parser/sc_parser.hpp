// Copyright Jordan Henderson 2026
#pragma once
#include "sc_lexer/text_location.hpp"
#include "sc_lexer/text_info.hpp"
#include <tuple>
#include <memory>

namespace sc::ast {

struct ASTGraph;

// The graph, and whether the parser accepted the input. Errors are recovered from, so input with errors is usually
// accepted, check the graph's diagnostics for them.
[[nodiscard]] std::tuple<std::shared_ptr<ASTGraph>, bool> parse(std::shared_ptr<const sc::lex::TextInfo>, sc::lex::SourceCodeRange);
[[nodiscard]] std::tuple<std::shared_ptr<ASTGraph>, bool> parse(std::shared_ptr<const sc::lex::TextInfo>, sc::lex::SourceCodeLocation);
[[nodiscard]] std::tuple<std::shared_ptr<ASTGraph>, bool> parse(std::shared_ptr<const sc::lex::TextInfo>);

}
