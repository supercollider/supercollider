// Copyright Jordan Henderson 2026
#include "layout/layout_draft.hpp"

#include "layout/passes.hpp"

#include "sc_parser/ast.hpp"
#include "sc_source/normalise_source.hpp"

#include <algorithm>

namespace sc::format {

namespace {

using lex::TokenType;

Token make_token(const src::SourceCodeRange& range, TokenType type) {
    return { TextOffset { range.begin.absolute }, TextOffset { range.end.absolute }, type, false };
}

TokenType close_bracket_type(std::string_view bracket) {
    if (bracket == ")")
        return TokenType::CloseParen;
    if (bracket == "]")
        return TokenType::CloseSquare;
    return TokenType::CloseCurly;
}

// With the whitespace between them, they are the whole text.
std::vector<Token> tokens_in_order(const ast::SourceTokens& source_tokens, std::string_view text) {
    std::vector<Token> ordered;
    ordered.reserve(source_tokens.tokens.size() + source_tokens.comments.size()
                    + source_tokens.unmatched_brackets.size());
    for (const auto& source_token : source_tokens.tokens.raw())
        if (!source_token.inserted() && source_token.token != TokenType::EndOfFile
            && source_token.range.begin.absolute != source_token.range.end.absolute)
            ordered.push_back(make_token(source_token.range, source_token.token));
    for (const auto& comment : source_tokens.comments.raw()) {
        auto token = make_token(comment.range, comment.kind);
        if (is_line_comment(comment.kind)) {
            const auto length = trim_line_comment(text.substr(*token.begin, *token.end - *token.begin)).size();
            token.end = util::advance(token.begin, static_cast<std::uint32_t>(length));
        }
        ordered.push_back(token);
    }
    for (const auto& bracket_range : source_tokens.unmatched_brackets)
        ordered.push_back(make_token(bracket_range, close_bracket_type(text.substr(bracket_range.begin.absolute, 1))));
    std::sort(ordered.begin(), ordered.end(),
              [](const Token& left, const Token& right) { return *left.begin < *right.begin; });
    return ordered;
}

}

TokenIndex LayoutDraft::first_token_at_or_after(TextOffset offset) const {
    const auto& all_tokens = tokens.raw();
    const auto first_at_or_after =
        std::lower_bound(all_tokens.begin(), all_tokens.end(), offset,
                         [](const Token& token, TextOffset target) { return *token.begin < *target; });
    return TokenIndex::from(first_at_or_after - all_tokens.begin());
}

std::optional<TokenIndex> LayoutDraft::token_at_start_of(const src::SourceCodeRange& range) const {
    if (range.begin.absolute == range.end.absolute)
        return std::nullopt;
    const TextOffset range_begin { range.begin.absolute };
    const auto token = first_token_at_or_after(range_begin);
    if (tokens.contains(token) && tokens[token].begin == range_begin)
        return token;
    return std::nullopt;
}

bool LayoutDraft::on_verbatim_line(TextOffset offset) const {
    const auto first_after =
        std::upper_bound(verbatim_lines.begin(), verbatim_lines.end(), offset,
                         [](TextOffset target, const TextRange& line) { return *target < *line.begin; });
    return first_after != verbatim_lines.begin() && *offset < *std::prev(first_after)->end;
}

std::string_view LayoutDraft::written_whitespace_before(TokenIndex token) const {
    const auto whitespace_begin = *token == 0 ? TextOffset { 0 } : tokens[util::retreat(token)].end;
    const auto whitespace_end = tokens.contains(token) ? tokens[token].begin : TextOffset::from(normalised_text.size());
    return text_between(whitespace_begin, whitespace_end);
}

bool LayoutDraft::would_lex_as_one(TokenIndex left_token, TokenIndex right_token) const {
    return lexes_as_one(token_text(left_token), token_text(right_token));
}

LayoutDraft make_layout_draft(const ast::ASTGraph& graph, const std::vector<TextRange>& verbatim_lines) {
    LayoutDraft layout;
    layout.normalised_text = graph.text_info()->source.as_string();
    layout.tokens.raw() = tokens_in_order(*graph.source_tokens(), layout.normalised_text);
    if (!verbatim_lines.empty())
        mark_tokens_on_verbatim_lines(layout, verbatim_lines);
    TextOffset previous_end { 0 };
    for (const auto& token : layout.tokens.raw()) {
        if (*token.begin < *previous_end)
            throw InternalError { "the parser's tokens overlap, please report this" };
        previous_end = token.end;
    }
    layout.whitespace_before.resize(layout.tokens.size() + 1);
    for (const auto token : layout.whitespace_before.indices())
        layout.whitespace_before[token].newlines = count_newlines(layout.written_whitespace_before(token));
    return layout;
}

std::string lay_out_and_print(const ast::ASTGraph& graph, const Style& style,
                              const std::vector<TextRange>& verbatim_lines) {
    auto layout = make_layout_draft(graph, verbatim_lines);
    const auto marks = mark_syntax(layout, graph, style);
    limit_blank_lines_and_add_final_newline(layout, style);
    const auto tables = break_and_join_lines(layout, marks, style);
    set_spaces_between_tokens(layout, marks, style);
    // After spacing, as it measures the printed width.
    align_table_columns(layout, tables, style);
    // Last, as the passes before change the whitespace of verbatim lines too.
    restore_verbatim_whitespace(layout);
    return print(layout, marks, style);
}

}
