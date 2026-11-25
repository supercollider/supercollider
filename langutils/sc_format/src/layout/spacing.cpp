// Copyright Jordan Henderson 2026
#include "layout/passes.hpp"

#include "syntax.hpp"

#include <optional>
#include <vector>

namespace sc::format {

namespace {

using lex::TokenType;

using Spaces = std::optional<std::uint8_t>;

Spaces spaces(SpaceOne option) { return option == SpaceOne::One ? Spaces { 1 } : std::nullopt; }
Spaces spaces(SpaceNone option) { return option == SpaceNone::None ? Spaces { 0 } : std::nullopt; }
Spaces spaces(Space option) {
    if (option == Space::One)
        return 1;
    if (option == Space::None)
        return 0;
    return std::nullopt;
}

// Inside a bracket, by either of its brackets.
Spaces spaces_inside(const Style& style, TokenType bracket) {
    switch (bracket) {
    case TokenType::OpenParen:
    case TokenType::CloseParen:
        return spaces(style.space_inside_parens);
    case TokenType::OpenSquare:
    case TokenType::CloseSquare:
        return spaces(style.space_inside_brackets);
    default:
        return spaces(style.space_inside_braces);
    }
}

// The names of control structures, `if (x)`, when written without a receiver.
bool is_control_name(const LayoutDraft& layout, TokenIndex name) {
    const auto type = layout.tokens[name].type;
    if (type != TokenType::While) {
        if (type != TokenType::Name)
            return false;
        const auto name_text = layout.token_text(name);
        if (!is_any_of(name_text, "if", "for", "forBy", "switch", "case"))
            return false;
    }
    return *name == 0 || layout.tokens[util::retreat(name)].type != TokenType::Dot;
}

// A generator, `{: x, x <- y }`, keeps its spaces inside the braces.
bool is_generator(const LayoutDraft& layout, TokenIndex open_bracket) {
    if (!is_open_brace(layout.tokens[open_bracket].type))
        return false;
    const auto next_token = util::advance(open_bracket);
    return layout.is(next_token, TokenType::Colon) || layout.is(next_token, TokenType::SemiColon);
}

// The whitespace between two tokens on one line is covered by one spacing option at most, picked by the tokens on
// either side, the first that applies in this order.
Spaces spaces_between(const LayoutDraft& layout, const SyntaxMarks& marks, const Style& style, TokenIndex left_token,
                      TokenIndex right_token, std::optional<TokenIndex> opener_of_right) {
    const auto left_type = layout.tokens[left_token].type;
    const auto right_type = layout.tokens[right_token].type;
    const auto& left_marks = marks.tokens[left_token];
    const auto& right_marks = marks.tokens[right_token];
    if (lex::is_close_bracket(right_type)) {
        if (opener_of_right && is_generator(layout, *opener_of_right))
            return std::nullopt;
        if (lex::is_open_bracket(left_type))
            return is_open_brace(left_type) ? spaces(style.space_in_empty_braces) : spaces_inside(style, left_type);
        return spaces_inside(style, right_type);
    }
    if (lex::is_open_bracket(left_type)) {
        if (is_generator(layout, left_token))
            return std::nullopt;
        if (is_open_brace(left_type) && right_marks.role == TokenRole::ParameterListOpenPipe)
            return spaces(style.space_before_parameters);
        return spaces_inside(style, left_type);
    }
    // `| |` stays, as `||` is an operator.
    if (left_marks.role == TokenRole::ParameterListOpenPipe)
        return right_marks.role == TokenRole::ParameterListClosePipe ? std::nullopt : spaces(style.space_inside_pipes);
    if (right_marks.role == TokenRole::ParameterListClosePipe)
        return spaces(style.space_inside_pipes);
    if (left_marks.role == TokenRole::ParameterListClosePipe)
        return spaces(style.space_after_parameters);
    if (right_type == TokenType::Comma)
        return spaces(style.space_before_comma);
    if (right_type == TokenType::SemiColon)
        return spaces(style.space_before_semicolon);
    if (left_type == TokenType::Comma)
        return spaces(style.space_after_comma);
    if (left_type == TokenType::Dot || right_type == TokenType::Dot)
        return spaces(style.space_around_dot);
    const bool operator_starts_right =
        is_any_of(right_marks.role, TokenRole::BinaryOperator, TokenRole::BinaryOperatorFirstPart);
    const bool operator_ends_left =
        is_any_of(left_marks.role, TokenRole::BinaryOperator, TokenRole::BinaryOperatorLastPart);
    if (operator_starts_right || operator_ends_left)
        return spaces(style.space_around_binary_operators);
    if (left_marks.role == TokenRole::KeywordOperator || right_marks.role == TokenRole::KeywordOperator)
        return spaces(style.space_around_keyword_operators);
    if (left_marks.role == TokenRole::KeyColon)
        return spaces(style.space_after_key);
    if (left_marks.role == TokenRole::AssignmentEquals || right_marks.role == TokenRole::AssignmentEquals)
        return spaces(style.space_around_assignment);
    if (left_marks.role == TokenRole::ParameterDefaultEquals || right_marks.role == TokenRole::ParameterDefaultEquals)
        return spaces(style.space_around_parameter_equals);
    if (right_type == TokenType::OpenParen && is_control_name(layout, left_token))
        return spaces(style.space_before_control_paren);
    if (right_marks.role == TokenRole::TrailingClosureOpenBrace
        || right_marks.role == TokenRole::ControlFunctionOpenBrace)
        return spaces(style.space_before_closure);
    return std::nullopt;
}

// Only between two tokens on one line, neither a comment.
void set_spaces_before(LayoutDraft& layout, const SyntaxMarks& marks, const Style& style, TokenIndex token,
                       std::optional<TokenIndex> opener_of_token) {
    if (*token == 0 || layout.whitespace_before[token].newlines > 0)
        return;
    const auto previous = util::retreat(token);
    if (layout.is_comment(token) || layout.is_comment(previous))
        return;
    const auto chosen = spaces_between(layout, marks, style, previous, token, opener_of_token);
    if (chosen && (*chosen > 0 || !layout.would_lex_as_one(previous, token)))
        layout.whitespace_before[token].spaces = *chosen;
}
}

void set_spaces_between_tokens(LayoutDraft& layout, const SyntaxMarks& marks, const Style& style) {
    auto& tokens = layout.tokens;
    std::vector<TokenIndex> open_brackets;
    for (const auto token : tokens.indices()) {
        const auto type = tokens[token].type;
        std::optional<TokenIndex> opener_of_closing_bracket;
        if (lex::is_close_bracket(type) && !open_brackets.empty()) {
            opener_of_closing_bracket = open_brackets.back();
            open_brackets.pop_back();
        }
        set_spaces_before(layout, marks, style, token, opener_of_closing_bracket);
        if (lex::is_open_bracket(type))
            open_brackets.push_back(token);
    }
}
}
