// Copyright Jordan Henderson 2026
#pragma once

#include "syntax.hpp"
#include "text.hpp"

#include "sc_format/format.hpp"
#include "sc_lexer/details/tokens.hpp"
#include "sc_source/text_location.hpp"
#include "sc_util/indexed_vector.hpp"
#include "sc_util/strong_index.hpp"

#include <cstdint>
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

struct LayoutDraft;

// The tokens and the whitespace as written, with the tokens on the verbatim lines marked. The verbatim lines are
// whole lines, in order and apart. Throws InternalError if the parser's tokens overlap.
[[nodiscard]] LayoutDraft make_layout_draft(const ast::ASTGraph& graph, const std::vector<TextRange>& verbatim_lines);

// Runs the passes of layout/passes.hpp over the file. Throws InternalError if the parser's tokens overlap.
[[nodiscard]] std::string lay_out_and_print(const ast::ASTGraph& graph, const Style& style,
                                            const std::vector<TextRange>& verbatim_lines);

////////////////////////////////////////////////////////////////////////////////
// Layout draft
////////////////////////////////////////////////////////////////////////////////

struct TokenTag;
using TokenIndex = util::StrongIndex<std::uint32_t, TokenTag>;

// What the AST says a token is, where spacing and line breaking need it.
enum struct TokenRole : std::uint8_t {
    None,
    BinaryOperator,
    BinaryOperatorFirstPart, // an operator with an adverb, `+.x`, is several tokens
    BinaryOperatorLastPart,
    KeywordOperator,
    AssignmentEquals,
    ParameterDefaultEquals,
    ParameterListOpenPipe,
    ParameterListClosePipe,
    KeyColon, // a keyword argument's or a dictionary key's `:`, or the keyword `freq:` holding it
    ArrayOpenBracket,
    ArgumentListOpenParen,
    DictionaryOpenParen,
    ControlFunctionOpenBrace, // after the `)`, which K&R braces join to the line before
    TrailingClosureOpenBrace, // any other function after the arguments, `x.do { }`, `f(1) { }`
};

// A token as written. Comments and closing brackets that close nothing are tokens, brackets the parser inserted aren't.
struct Token {
    TextOffset begin;
    TextOffset end;
    lex::TokenType type;
    bool on_verbatim_line;
};

// What the AST says a token is.
struct TokenMarks {
    TokenRole role;
    bool starts_list_item : 1;
    bool opens_region : 1;
    bool region_indented_in_source : 1; // the first line inside is indented more than the line of the `(`
};

// Spaces are only set between two tokens on one line.
struct Whitespace {
    static constexpr std::uint16_t spaces_as_written { 0xFFFF };
    std::uint32_t newlines;
    std::uint16_t spaces { spaces_as_written };
    bool touches_verbatim_line { false };
};

// A line indented from the line its expression starts on, not from its brackets.
struct RelativeIndentLine {
    enum struct Kind : std::uint8_t { StringContinuation, MethodChain, ControlFunction };
    TokenIndex first_token;
    TokenIndex expression_start; // the string's, the chain's receiver's, or the control message's first token
    Kind kind;
    bool chain_indented_in_source;
};

// An element of a list with a trailing comma, and the comma after it.
/*
Each entry here is a cell.
[
    1,   22, 3,
    444, 5,  66,
]
*/
struct TableCell {
    TokenIndex first_token;
    TokenIndex comma;
    bool starts_row;
};

struct TableCellTag;
using TableCellIndex = util::StrongIndex<std::uint32_t, TableCellTag>;

// A list break_and_join_lines() expanded into rows of more than one element. `end` is excluded.
struct TableToAlign {
    TableCellIndex begin;
    TableCellIndex end;
};

// The functions after a `switch` or `case`, `switch(x) { 1 } { a } { 2 } { b } { c }`: test and action pairs, and a
// default if their number is odd. Each `{` after the first is found from the `}` before it.
struct ControlPairs {
    TokenIndex message_start;
    TokenIndex first_open_brace;
    std::uint32_t function_count;
};

// The whole file as tokens and the whitespace before each, which the passes change and print() writes.
// We mutate this.
struct LayoutDraft {
    std::string_view normalised_text;

    // Parallel: whitespace_before[i] is before tokens[i]. It has one more, the whitespace at the end of the file.
    util::IndexedVector<TokenIndex, Token> tokens;
    util::IndexedVector<TokenIndex, Whitespace> whitespace_before;

    // Whole lines printed as written. Their tokens are marked on_verbatim_line.
    std::vector<TextRange> verbatim_lines;
    bool any_token_on_verbatim_line { false };

    // Text.
    [[nodiscard]] std::string_view text_between(TextOffset begin, TextOffset end) const;
    [[nodiscard]] std::string_view token_text(TokenIndex token) const;
    [[nodiscard]] std::string_view written_whitespace_before(TokenIndex token) const;
    [[nodiscard]] bool on_verbatim_line(TextOffset offset) const;
    // E.g., `|a| -1` as `|a|-1`, where `|-` is an operator.
    [[nodiscard]] bool would_lex_as_one(TokenIndex left_token, TokenIndex right_token) const;

    // Finding tokens. Those returning a TokenIndex return the end of the file if there is none.
    [[nodiscard]] TokenIndex first_token_at_or_after(TextOffset offset) const;
    [[nodiscard]] std::optional<TokenIndex> token_at_start_of(const src::SourceCodeRange& range) const;
    [[nodiscard]] TokenIndex first_non_comment_from(TokenIndex token) const;
    [[nodiscard]] TokenIndex next_non_comment(TokenIndex token) const;
    [[nodiscard]] std::optional<TokenIndex> previous_non_comment(TokenIndex token) const;
    // The start of the file starts a line.
    [[nodiscard]] std::optional<TokenIndex> line_start_if_only_comments_before(TokenIndex token) const;

    // Line breaks.
    // Whether any token after from_after, up to and including through, has a newline before it.
    [[nodiscard]] bool line_break_between(TokenIndex from_after, TokenIndex through) const;
    // Comments between them count. There must be code before token.
    [[nodiscard]] bool line_break_since_previous_code(TokenIndex token) const;
    // Not the first token.
    void start_line_unless_after_comment(TokenIndex token);

    // What a token is. False at the end of the file.
    [[nodiscard]] bool is(TokenIndex token, lex::TokenType type) const;
    [[nodiscard]] bool is_comment(TokenIndex token) const;
    [[nodiscard]] bool is_open_brace(TokenIndex token) const;
};

// From the AST, by mark_syntax(). Only the marks an option reads are made.
struct SyntaxMarks {
    util::IndexedVector<TokenIndex, TokenMarks> tokens; // parallel to LayoutDraft::tokens
    //
    std::vector<RelativeIndentLine> relative_indent_lines; // in order of first_token
    std::vector<ControlPairs> control_pairs;
};

// From break_and_join_lines().
struct TablesToAlign {
    util::IndexedVector<TableCellIndex, TableCell> cells;
    std::vector<TableToAlign> tables; // each a range of cells
};

inline std::string_view LayoutDraft::text_between(TextOffset begin, TextOffset end) const {
    return normalised_text.substr(*begin, *end - *begin);
}

inline std::string_view LayoutDraft::token_text(TokenIndex token) const {
    return text_between(tokens[token].begin, tokens[token].end);
}

inline TokenIndex LayoutDraft::first_non_comment_from(TokenIndex token) const {
    while (is_comment(token))
        token = util::advance(token);
    return token;
}

inline TokenIndex LayoutDraft::next_non_comment(TokenIndex token) const {
    return first_non_comment_from(util::advance(token));
}

inline std::optional<TokenIndex> LayoutDraft::previous_non_comment(TokenIndex token) const {
    do {
        if (*token == 0)
            return std::nullopt;
        token = util::retreat(token);
    } while (is_comment(token));
    return token;
}

inline std::optional<TokenIndex> LayoutDraft::line_start_if_only_comments_before(TokenIndex token) const {
    while (whitespace_before[token].newlines == 0) {
        if (*token == 0)
            return token;
        if (!is_comment(util::retreat(token)))
            return std::nullopt;
        token = util::retreat(token);
    }
    return token;
}

inline bool LayoutDraft::line_break_between(TokenIndex from_after, TokenIndex through) const {
    for (auto token = util::advance(from_after); *token <= *through; token = util::advance(token))
        if (whitespace_before[token].newlines > 0)
            return true;
    return false;
}

inline bool LayoutDraft::line_break_since_previous_code(TokenIndex token) const {
    return line_break_between(*previous_non_comment(token), token);
}

inline void LayoutDraft::start_line_unless_after_comment(TokenIndex token) {
    if (whitespace_before[token].newlines == 0 && !is_comment(util::retreat(token)))
        whitespace_before[token].newlines = 1;
}

inline bool LayoutDraft::is(TokenIndex token, lex::TokenType type) const {
    return tokens.contains(token) && tokens[token].type == type;
}

inline bool LayoutDraft::is_comment(TokenIndex token) const {
    return tokens.contains(token) && lex::is_comment(tokens[token].type);
}

inline bool LayoutDraft::is_open_brace(TokenIndex token) const {
    return tokens.contains(token) && format::is_open_brace(tokens[token].type);
}
}
