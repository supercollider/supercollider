// Copyright Jordan Henderson 2026
#pragma once
#include "ast.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_lexer/text_info.hpp"
#include <tuple>
#include <memory>

namespace sc::ast {

[[nodiscard]] std::tuple<ASTGraph, bool> parse(std::shared_ptr<const sc::lex::TextInfo>, sc::lex::SourceCodeRange);
[[nodiscard]] std::tuple<ASTGraph, bool> parse(std::shared_ptr<const sc::lex::TextInfo>, sc::lex::SourceCodeLocation);
[[nodiscard]] std::tuple<ASTGraph, bool> parse(std::shared_ptr<const sc::lex::TextInfo>);

}
