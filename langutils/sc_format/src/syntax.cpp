// Copyright Jordan Henderson 2026
#include "syntax.hpp"

#include "text.hpp"

#include "sc_lexer/lexer.hpp"
#include "sc_source/normalise_source.hpp"

#include <optional>
#include <string>

namespace sc::format {

namespace {

// Whether the first token, whitespace aside, ends where the left one does.
struct FirstTokenEndsAt {
    std::size_t left_length;
    using Output = bool;
    template <lex::TokenType T> [[nodiscard]] std::optional<Output> process(src::SourceCodeRange range) {
        if constexpr (lex::is_whitespace(T))
            return std::nullopt;
        else
            return range.end.absolute == left_length;
    }
};

struct KeepNonWhitespace {
    struct Output {
        lex::TokenType type;
        src::SourceCodeRange range;
    };
    template <lex::TokenType T> [[nodiscard]] std::optional<Output> process(src::SourceCodeRange range) {
        if constexpr (lex::is_whitespace(T))
            return std::nullopt;
        else
            return Output { T, range };
    }
};
}

bool is_control_flow(std::string_view selector) {
    return is_any_of(selector, "if", "while", "for", "forBy", "switch", "case", "try");
}

bool is_control_flow_or_iterator(std::string_view selector) {
    return is_control_flow(selector) || is_any_of(selector, "do", "collect");
}

bool lexes_as_one(std::string_view left, std::string_view right) {
    if (left.empty() || right.empty())
        return false;
    std::string text;
    text.reserve(left.size() + right.size());
    text += left;
    text += right;
    const src::NormalisedSource source { text };
    lex::CodePointStream stream { source };
    FirstTokenEndsAt action { left.size() };
    return !lex::lexer(stream, action);
}

TokenTexts non_whitespace_tokens(const src::NormalisedSource& source) {
    TokenTexts tokens;
    const std::string_view text = source.as_string();
    lex::CodePointStream stream { source, src::FileCodeLocation {} };
    KeepNonWhitespace action;
    while (true) {
        const auto token = lex::lexer(stream, action);
        if (token.type == lex::TokenType::EndOfFile)
            return tokens;
        const auto token_text =
            text.substr(token.range.begin.absolute, token.range.end.absolute - token.range.begin.absolute);
        tokens.emplace_back(token.type, is_line_comment(token.type) ? trim_line_comment(token_text) : token_text);
    }
}
}
