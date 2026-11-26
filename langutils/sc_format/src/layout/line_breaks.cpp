// Copyright Jordan Henderson 2026
#include "layout/passes.hpp"

#include "syntax.hpp"

#include <algorithm>
#include <iterator>
#include <vector>

namespace sc::format {

namespace {

using lex::TokenType;

using CloserOfOpener = util::IndexedVector<TokenIndex, std::optional<TokenIndex>>;

CloserOfOpener match_brackets(const LayoutDraft& layout) {
    CloserOfOpener closer_of_opener;
    closer_of_opener.raw().assign(layout.tokens.size(), std::nullopt);
    std::vector<TokenIndex> open_brackets;
    for (const auto token : layout.tokens.indices()) {
        const auto type = layout.tokens[token].type;
        if (lex::is_close_bracket(type) && !open_brackets.empty()) {
            closer_of_opener[open_brackets.back()] = token;
            open_brackets.pop_back();
        }
        if (lex::is_open_bracket(type))
            open_brackets.push_back(token);
    }
    return closer_of_opener;
}

// Including the whitespace before each token.
util::IndexedVector<TokenIndex, std::uint32_t> count_cumulative_line_breaks(const LayoutDraft& layout) {
    util::IndexedVector<TokenIndex, std::uint32_t> cumulative_line_breaks;
    cumulative_line_breaks.raw().assign(layout.whitespace_before.size(), 0);
    std::uint32_t line_breaks { 0 };
    for (const auto token : layout.whitespace_before.indices()) {
        line_breaks += layout.whitespace_before[token].newlines > 0 ? 1 : 0;
        cumulative_line_breaks[token] = line_breaks;
    }
    return cumulative_line_breaks;
}

// A join is never next to a comment, and its spaces are set here, as the spacing options may leave that whitespace
// alone.
void join_to_previous_line(LayoutDraft& layout, TokenIndex token, std::uint16_t spaces) {
    if (layout.is_comment(util::retreat(token)))
        return;
    layout.whitespace_before[token].newlines = 0;
    layout.whitespace_before[token].spaces = spaces;
}

Switch kr_option_for(const Style& style, TokenType opener) {
    if (is_open_brace(opener))
        return style.kr_braces;
    if (opener == TokenType::OpenParen)
        return style.kr_parens;
    if (opener == TokenType::OpenSquare)
        return style.kr_brackets;
    return Switch::Keep;
}

// An opener starting a line that belongs to what the line before ends with, e.g., `x =\n{`, not one starting a
// list item, e.g., a statement or an array's element. Only one whose brackets spanned lines before any joins.
bool kr_joins_opener(const LayoutDraft& layout, const SyntaxMarks& marks, const Style& style,
                     const CloserOfOpener& closer_of_opener,
                     const util::IndexedVector<TokenIndex, std::uint32_t>& cumulative_line_breaks, TokenIndex opener) {
    if (kr_option_for(style, layout.tokens[opener].type) != Switch::On)
        return false;
    const auto& opener_marks = marks.tokens[opener];
    if (opener_marks.starts_list_item || opener_marks.opens_region)
        return false;
    const auto close_bracket = closer_of_opener[opener];
    if (!close_bracket || cumulative_line_breaks[*close_bracket] == cumulative_line_breaks[opener])
        return false;
    const auto previous_type = layout.tokens[util::retreat(opener)].type;
    if (lex::matches(previous_type, TokenType::SemiColon, TokenType::Comma) || lex::is_open_bracket(previous_type))
        return false;
    // After an operator, a bracket is an operand, `a +\n(b * c)`, where the line was broken for its length.
    const auto previous_role = marks.tokens[util::retreat(opener)].role;
    if (is_any_of(previous_role, TokenRole::BinaryOperator, TokenRole::BinaryOperatorLastPart,
                  TokenRole::KeywordOperator))
        return false;
    // After a block, only the `{` of an `else` after a multi-line block, `}\n{`, not a `case`'s next pair.
    return previous_type != TokenType::CloseCurly || layout.line_start_if_only_comments_before(util::retreat(opener));
}

// `if(x)\n{ 1 }\n{ 2 }`: a control message's functions join if each is a literal or spans lines, see
// mark_control_functions().
bool kr_joins_control_function(const SyntaxMarks& marks, const Style& style, TokenIndex brace) {
    if (style.kr_braces != Switch::On)
        return false;
    const auto& brace_marks = marks.tokens[brace];
    return brace_marks.role == TokenRole::ControlFunctionOpenBrace && !brace_marks.opens_region;
}

// `{\n|a|` or `{\narg a;`.
bool joins_parameters_to_brace(const LayoutDraft& layout, const SyntaxMarks& marks, const Style& style,
                               TokenIndex token) {
    if (style.parameters_on_brace_line != Switch::On || !layout.is_open_brace(util::retreat(token)))
        return false;
    return marks.tokens[token].role == TokenRole::ParameterListOpenPipe || layout.is(token, TokenType::Arg);
}

// None between a name and its arguments or index, `f(1)`, `a[0]`, one before anything else.
std::uint8_t spaces_before_joined_opener(const LayoutDraft& layout, TokenIndex opener) {
    const auto previous_type = layout.tokens[util::retreat(opener)].type;
    const bool after_name_or_closer =
        lex::matches(previous_type, TokenType::Name, TokenType::ClassName) || lex::is_close_bracket(previous_type);
    const bool attached = layout.tokens[opener].type != TokenType::OpenCurly && after_name_or_closer;
    return attached && !layout.would_lex_as_one(util::retreat(opener), opener) ? 0 : 1;
}

// K&R: a `{` whose braces span lines ends its line, `{ a;\n b }` becomes `{\n a;\n b }`. Its parameters stay
// on its line, as does a comment. Not a generator, `{: x, x <- y }`.
void break_line_after_open_brace(LayoutDraft& layout, const SyntaxMarks& marks, const CloserOfOpener& closer_of_opener,
                                 TokenIndex open_brace) {
    const auto close_brace = closer_of_opener[open_brace];
    if (!close_brace || !layout.line_break_between(open_brace, *close_brace))
        return;
    auto first_inner = util::advance(open_brace);
    if (first_inner == *close_brace
        || lex::matches(layout.tokens[first_inner].type, TokenType::Colon, TokenType::SemiColon))
        return;
    // Past `|a, b|` or `arg a, b;`.
    const auto parameters_end = [&](TokenIndex first_token) -> std::optional<TokenIndex> {
        const bool pipes = marks.tokens[first_token].role == TokenRole::ParameterListOpenPipe;
        if (!pipes && !layout.is(first_token, TokenType::Arg))
            return std::nullopt;
        for (auto token = util::advance(first_token); token != *close_brace; token = util::advance(token))
            if (pipes ? marks.tokens[token].role == TokenRole::ParameterListClosePipe
                      : layout.tokens[token].type == TokenType::SemiColon)
                return token;
        return std::nullopt;
    };
    if (layout.whitespace_before[first_inner].newlines == 0 && !layout.is_comment(first_inner))
        if (const auto parameters_close = parameters_end(first_inner))
            first_inner = util::advance(*parameters_close);
    const bool already_starts_line = layout.whitespace_before[first_inner].newlines > 0;
    const bool next_to_comment = layout.is_comment(first_inner) || layout.is_comment(util::retreat(first_inner));
    if (first_inner == *close_brace || already_starts_line || next_to_comment)
        return;
    layout.whitespace_before[first_inner].newlines = 1;
}

// A region's `(` and `)` each on a line of their own: `( a ); ( b );` becomes `(\na\n);\n(\nb\n);`. A comment
// after the `(` stays on its line, and a block comment before it keeps the `(` on its line.
void put_region_parens_on_own_lines(LayoutDraft& layout, const CloserOfOpener& closer_of_opener,
                                    TokenIndex open_paren) {
    const auto close_paren = closer_of_opener[open_paren];
    if (!close_paren)
        return;
    if (*open_paren > 0)
        layout.start_line_unless_after_comment(open_paren);
    const auto first_inner = util::advance(open_paren);
    if (first_inner == *close_paren)
        return;
    if (!layout.is_comment(first_inner))
        layout.start_line_unless_after_comment(first_inner);
    layout.start_line_unless_after_comment(*close_paren);
}

// A `switch` or `case` over lines: each test starts a line with its action after it, and the default its own.
void put_control_pairs_on_lines(LayoutDraft& layout, const CloserOfOpener& closer_of_opener,
                                const ControlPairs& pairs) {
    std::vector<TokenIndex> openers { pairs.first_open_brace };
    while (openers.size() < pairs.function_count) {
        const auto close_brace = closer_of_opener[openers.back()];
        if (!close_brace)
            return;
        const auto next_open_brace = layout.next_non_comment(*close_brace);
        if (!layout.tokens.contains(next_open_brace))
            return;
        openers.push_back(next_open_brace);
    }
    const auto last_close_brace = closer_of_opener[openers.back()];
    if (!last_close_brace || !layout.line_break_between(util::retreat(pairs.first_open_brace), *last_close_brace))
        return;
    bool is_test { true }; // else an action
    for (const auto open_brace : openers) {
        if (is_test && layout.whitespace_before[open_brace].newlines == 0)
            layout.whitespace_before[open_brace].newlines = 1;
        else if (!is_test && layout.whitespace_before[open_brace].newlines > 0)
            join_to_previous_line(layout, open_brace, 1);
        is_test = !is_test;
    }
}

bool opens_list(TokenRole role) {
    return is_any_of(role, TokenRole::ArrayOpenBracket, TokenRole::ArgumentListOpenParen,
                     TokenRole::DictionaryOpenParen);
}

bool one_element_per_line(const Style& style, TokenRole role) {
    if (role == TokenRole::ArrayOpenBracket)
        return style.multiline_arrays == PerLine::One;
    if (role == TokenRole::ArgumentListOpenParen)
        return style.multiline_arguments == PerLine::One;
    return false;
}

// The elements directly inside a list, each with the comma after it.
struct ListElements {
    std::vector<TableCell> cells;
    bool comment_between_elements_on_one_line { false }; // `a, /* x */ b`, which columns can't place
    bool any_verbatim { false };
};

// None if an element is missing, `[a, , b]`.
std::optional<ListElements> collect_list_elements(const LayoutDraft& layout, TokenIndex open_bracket,
                                                  TokenIndex close_bracket) {
    ListElements elements;
    std::optional<TokenIndex> element_start;
    std::uint32_t bracket_depth { 0 };
    for (auto token = util::advance(open_bracket); token != close_bracket; token = util::advance(token)) {
        const auto type = layout.tokens[token].type;
        elements.any_verbatim = elements.any_verbatim || layout.tokens[token].on_verbatim_line;
        if (bracket_depth == 0 && type == TokenType::Comma) {
            if (!element_start)
                return std::nullopt;
            elements.cells.push_back({ *element_start, token, false });
            element_start.reset();
            continue;
        }
        if (bracket_depth == 0 && !element_start) {
            if (!layout.is_comment(token))
                element_start = token;
            else if (layout.whitespace_before[token].newlines == 0
                     && layout.whitespace_before[util::advance(token)].newlines == 0)
                elements.comment_between_elements_on_one_line = true;
        }
        if (lex::is_open_bracket(type))
            ++bracket_depth;
        else if (lex::is_close_bracket(type))
            --bracket_depth;
    }
    return elements;
}

// A comma starting a line, `a\n, b`, joins its element's line, unless a comment ends that line.
void join_commas_to_their_elements(LayoutDraft& layout, const std::vector<TableCell>& cells) {
    for (const auto& cell : cells) {
        auto& whitespace = layout.whitespace_before[cell.comma];
        if (whitespace.newlines > 0 && !layout.is_comment(util::retreat(cell.comma))) {
            whitespace.newlines = 0;
            whitespace.spaces = 0;
        }
    }
}

struct RowsAsWritten {
    std::vector<std::uint32_t> elements_in_row;
    bool any_element_spans_lines { false };
    bool row_continues_after_multiline_element { false };
};

// Sets each cell's starts_row from the line breaks as written.
RowsAsWritten find_rows_as_written(const LayoutDraft& layout, std::vector<TableCell>& cells, TokenIndex open_bracket) {
    RowsAsWritten rows;
    auto previous_comma = open_bracket;
    bool previous_element_spans_lines { false };
    for (auto& cell : cells) {
        cell.starts_row = rows.elements_in_row.empty() || layout.line_break_between(previous_comma, cell.first_token);
        if (cell.starts_row)
            rows.elements_in_row.push_back(0);
        else if (previous_element_spans_lines)
            rows.row_continues_after_multiline_element = true;
        ++rows.elements_in_row.back();
        const auto before_comma = util::retreat(cell.comma);
        previous_element_spans_lines =
            (*cell.first_token < *before_comma && layout.line_break_between(cell.first_token, before_comma))
            || layout.whitespace_before[cell.comma].newlines > 0;
        rows.any_element_spans_lines = rows.any_element_spans_lines || previous_element_spans_lines;
        previous_comma = cell.comma;
    }
    return rows;
}

// The elements in each full row, if the rows form a table. The first and last rows may be shorter than the rest.
std::optional<std::uint32_t> table_row_length(const std::vector<std::uint32_t>& elements_in_row) {
    if (elements_in_row.size() < 2)
        return std::nullopt;
    const auto first_row = elements_in_row.front();
    const auto last_row = elements_in_row.back();
    const auto row_length = elements_in_row.size() > 2 ? elements_in_row[1] : std::max(first_row, last_row);
    if (first_row > row_length || last_row > row_length)
        return std::nullopt;
    const bool middle_rows_full = std::all_of(elements_in_row.begin() + 1, elements_in_row.end() - 1,
                                              [&](std::uint32_t elements) { return elements == row_length; });
    if (!middle_rows_full)
        return std::nullopt;
    return row_length;
}

// Each row, or each element if the rows aren't kept, and the closer start a line.
void start_row_lines(LayoutDraft& layout, std::vector<TableCell>& cells, bool rows_kept, TokenIndex open_bracket,
                     TokenIndex close_bracket) {
    auto previous_comma = open_bracket;
    for (auto& cell : cells) {
        cell.starts_row = cell.starts_row || !rows_kept;
        if (cell.starts_row && !layout.line_break_between(previous_comma, cell.first_token))
            layout.whitespace_before[cell.first_token].newlines = 1;
        previous_comma = cell.comma;
    }
    if (!layout.line_break_between(previous_comma, close_bracket))
        layout.whitespace_before[close_bracket].newlines = 1;
}

void add_table_to_align(TablesToAlign& tables, const std::vector<TableCell>& cells) {
    const auto first_cell = TableCellIndex::from(tables.cells.size());
    for (const auto& cell : cells)
        (void)tables.cells.push_back(cell);
    tables.tables.push_back({ first_cell, TableCellIndex::from(tables.cells.size()) });
}

void expand_trailing_comma_list(LayoutDraft& layout, const SyntaxMarks& marks, const Style& style,
                                const CloserOfOpener& closer_of_opener, TablesToAlign& tables,
                                TokenIndex open_bracket) {
    const auto close_bracket = *closer_of_opener[open_bracket];
    const auto last_comma = *layout.previous_non_comment(close_bracket);
    if (*last_comma <= *open_bracket || layout.tokens[last_comma].type != TokenType::Comma)
        return;
    auto elements = collect_list_elements(layout, open_bracket, close_bracket);
    if (!elements)
        return;
    join_commas_to_their_elements(layout, elements->cells);
    const auto rows = find_rows_as_written(layout, elements->cells, open_bracket);
    const auto row_length = table_row_length(rows.elements_in_row);
    const bool rows_kept = row_length && !rows.row_continues_after_multiline_element
        && !one_element_per_line(style, marks.tokens[open_bracket].role);
    start_row_lines(layout, elements->cells, rows_kept, open_bracket, close_bracket);
    const bool columns_can_align =
        !rows.any_element_spans_lines && !elements->comment_between_elements_on_one_line && !elements->any_verbatim;
    if (style.align_columns == Switch::On && rows_kept && *row_length >= 2 && columns_can_align)
        add_table_to_align(tables, elements->cells);
}

// If a list has a line break before one of its elements, every element and its closer start a line.
void split_one_element_per_line(LayoutDraft& layout, const SyntaxMarks& marks, const CloserOfOpener& closer_of_opener,
                                TokenIndex open_bracket) {
    const auto close_bracket = closer_of_opener[open_bracket];
    if (!close_bracket)
        return;
    // Where a line can start: each element, and the closer. Elements are the items directly inside.
    std::vector<TokenIndex> line_starts;
    std::uint32_t bracket_depth { 0 };
    for (auto token = util::advance(open_bracket); token != *close_bracket; token = util::advance(token)) {
        const auto type = layout.tokens[token].type;
        if (bracket_depth == 0 && marks.tokens[token].starts_list_item)
            line_starts.push_back(token);
        if (lex::is_open_bracket(type))
            ++bracket_depth;
        else if (lex::is_close_bracket(type))
            --bracket_depth;
    }
    if (line_starts.empty())
        return;
    line_starts.push_back(*close_bracket);
    // A closer alone on its line, `f(a, {\n}\n)`, doesn't make the list multi-line.
    if (std::none_of(line_starts.begin(), std::prev(line_starts.end()),
                     [&](TokenIndex line_start) { return layout.line_break_since_previous_code(line_start); }))
        return;
    for (const auto line_start : line_starts)
        if (!layout.line_break_since_previous_code(line_start))
            layout.whitespace_before[line_start].newlines = 1;
}

}

void limit_blank_lines_and_add_final_newline(LayoutDraft& layout, const Style& style) {
    if (const auto max_blank_lines = style.max_blank_lines)
        for (const auto token : layout.whitespace_before.indices()) {
            // The first newline ends the line of the token before, so isn't a blank line, but at the start of the
            // text it is.
            const std::uint32_t max_newlines = *token == 0 ? *max_blank_lines : *max_blank_lines + 1;
            layout.whitespace_before[token].newlines = std::min(layout.whitespace_before[token].newlines, max_newlines);
        }
    if (style.final_newline == FinalNewline::One)
        layout.whitespace_before.back().newlines = 1;
}

TablesToAlign break_and_join_lines(LayoutDraft& layout, const SyntaxMarks& marks, const Style& style) {
    const auto& tokens = layout.tokens;
    const auto closer_of_opener = match_brackets(layout);
    TablesToAlign tables;
    if (style.trailing_comma_lists == TrailingCommaLists::Expand) {
        // Inner lists first, as one that expands makes the element holding it span lines.
        std::vector<TokenIndex> list_openers;
        for (const auto token : tokens.indices())
            if (opens_list(marks.tokens[token].role) && closer_of_opener[token])
                list_openers.push_back(token);
        std::sort(list_openers.begin(), list_openers.end(), [&](TokenIndex first_list, TokenIndex second_list) {
            return **closer_of_opener[first_list] < **closer_of_opener[second_list];
        });
        for (const auto list_opener : list_openers)
            expand_trailing_comma_list(layout, marks, style, closer_of_opener, tables, list_opener);
    }
    for (const auto token : tokens.indices())
        if (one_element_per_line(style, marks.tokens[token].role))
            split_one_element_per_line(layout, marks, closer_of_opener, token);
    // Splits first, as they can make a bracket multi-line.
    const auto cumulative_line_breaks = count_cumulative_line_breaks(layout);
    for (const auto token : tokens.indices()) {
        if (*token == 0 || layout.whitespace_before[token].newlines == 0)
            continue;
        if (kr_joins_opener(layout, marks, style, closer_of_opener, cumulative_line_breaks, token)
            || kr_joins_control_function(marks, style, token))
            join_to_previous_line(layout, token, spaces_before_joined_opener(layout, token));
        else if (joins_parameters_to_brace(layout, marks, style, token))
            join_to_previous_line(layout, token, 1);
    }
    if (style.multiline_control_pairs == PerLine::One)
        for (const auto& pairs : marks.control_pairs)
            put_control_pairs_on_lines(layout, closer_of_opener, pairs);
    // After the joins, which decide which braces span lines.
    if (style.kr_braces == Switch::On)
        for (const auto token : tokens.indices())
            if (is_open_brace(tokens[token].type))
                break_line_after_open_brace(layout, marks, closer_of_opener, token);
    if (style.region_brackets == RegionBrackets::OwnLines)
        for (const auto token : tokens.indices())
            if (marks.tokens[token].opens_region)
                put_region_parens_on_own_lines(layout, closer_of_opener, token);
    return tables;
}
}
