// Copyright Jordan Henderson 2026
#pragma once

#include "sc_lexer/details/tokens.hpp"

#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

namespace sc::src {
struct NormalisedSource;
}

namespace sc::format {

template <typename Value, typename... Candidates>
[[nodiscard]] constexpr bool is_any_of(const Value& value, const Candidates&... candidates) {
    return ((value == candidates) || ...);
}

template <typename... Alternatives, typename Variant> [[nodiscard]] bool holds_any(const Variant& variant) {
    return (std::holds_alternative<Alternatives>(variant) || ...);
}

// For style options, which are all off when `Keep`.
template <typename... Options> [[nodiscard]] constexpr bool any_not_keep(Options... options) {
    return ((options != Options::Keep) || ...);
}

[[nodiscard]] inline bool is_line_comment(lex::TokenType type) {
    return lex::matches(type, lex::TokenType::Comment, lex::TokenType::VersionComment);
}

// `var`, `arg`, `classvar`, `const`.
[[nodiscard]] inline bool is_declaration(lex::TokenType type) {
    return lex::matches(type, lex::TokenType::Var, lex::TokenType::Arg, lex::TokenType::ClassVar,
                        lex::TokenType::Const);
}

// `{` or `#{`.
[[nodiscard]] inline bool is_open_brace(lex::TokenType type) {
    return lex::matches(type, lex::TokenType::OpenCurly, lex::TokenType::BeginClosedFunction);
}

// `if`, `while`, `for`, `forBy`, `switch`, `case`, `try`.
[[nodiscard]] bool is_control_flow(std::string_view selector);

// Control flow and `do` and `collect`, whose functions trailing closures and K&R braces handle.
[[nodiscard]] bool is_control_flow_or_iterator(std::string_view selector);

using TokenTexts = std::vector<std::tuple<lex::TokenType, std::string_view>>;

// Every token's type and text, whitespace aside, with a `//` comment's text trimmed. The texts are views into source.
[[nodiscard]] TokenTexts non_whitespace_tokens(const src::NormalisedSource& source);

// E.g., `|` and `-1` as `|-` and `1`.
[[nodiscard]] bool lexes_as_one(std::string_view left, std::string_view right);

}
