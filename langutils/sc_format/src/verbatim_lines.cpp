// Copyright Jordan Henderson 2026
#include "verbatim_lines.hpp"

#include "sc_lexer/details/tokens.hpp"
#include "sc_parser/ast.hpp"

#include <algorithm>

namespace sc::format {

namespace {

using lex::TokenType;

// Where each line starts, lines ending at a `\n`, as an editor counts them.
using LineStarts = std::vector<TextOffset>;

LineStarts line_starts(std::string_view text) {
    LineStarts starts { TextOffset { 0 } };
    for (const auto offset : util::range(TextOffset { 0 }, TextOffset::from(text.size())))
        if (text[*offset] == '\n')
            starts.push_back(util::advance(offset));
    return starts;
}

std::uint32_t line_holding(const LineStarts& starts, TextOffset offset) {
    const auto first_after = std::upper_bound(starts.begin(), starts.end(), offset,
                                              [](TextOffset target, TextOffset start) { return *target < *start; });
    return static_cast<std::uint32_t>(first_after - starts.begin() - 1);
}

// Or the end of the text.
TextOffset next_line_start(const LineStarts& starts, std::string_view text, std::uint32_t line) {
    return line + 1 < starts.size() ? starts[line + 1] : TextOffset::from(text.size());
}

void add_lines_outside_selection(const LineStarts& starts, std::string_view text, LineRange selection,
                                 std::vector<TextRange>& verbatim_lines) {
    const auto first_selected = selection.first;
    const auto last_selected = std::max(selection.first, selection.last);
    const auto text_end = TextOffset::from(text.size());
    const auto selection_begin = first_selected < starts.size() ? starts[first_selected] : text_end;
    if (*selection_begin > 0)
        verbatim_lines.push_back({ TextOffset { 0 }, selection_begin });
    if (last_selected < starts.size()) {
        const auto after_selection = next_line_start(starts, text, last_selected);
        if (*after_selection < *text_end)
            verbatim_lines.push_back({ after_selection, text_end });
    }
}

// `sc-format off` or `sc-format on` in a `//` or `/* */` comment, spaces around it ignored.
std::optional<bool> formatting_turned_on_by(TokenType kind, std::string_view comment) {
    if (kind == TokenType::Comment)
        comment = trim_line_comment(comment.substr(2));
    else if (kind == TokenType::MultilineComment && comment.size() >= 4)
        comment = trim_line_comment(comment.substr(2, comment.size() - 4));
    else
        return std::nullopt;
    while (!comment.empty() && is_space_or_tab(std::get<0>(first_codepoint(comment))))
        comment.remove_prefix(std::get<1>(first_codepoint(comment)));
    if (comment == "sc-format off")
        return false;
    if (comment == "sc-format on")
        return true;
    return std::nullopt;
}

// From each `sc-format off` comment to the next `sc-format on` comment, or the end of the text.
void add_formatting_off_ranges(const ast::ASTGraph& graph, std::string_view text,
                               std::vector<TextRange>& verbatim_lines) {
    std::optional<TextOffset> off_start;
    for (const auto& comment : graph.source_tokens()->comments.raw()) {
        const auto turns_on = formatting_turned_on_by(
            comment.kind,
            text.substr(comment.range.begin.absolute, comment.range.end.absolute - comment.range.begin.absolute));
        if (turns_on == false && !off_start)
            off_start = TextOffset { comment.range.begin.absolute };
        else if (turns_on == true && off_start) {
            verbatim_lines.push_back({ *off_start, TextOffset { comment.range.end.absolute } });
            off_start.reset();
        }
    }
    if (off_start)
        verbatim_lines.push_back({ *off_start, TextOffset::from(text.size()) });
}

void merge_overlapping(std::vector<TextRange>& ranges) {
    std::sort(ranges.begin(), ranges.end(),
              [](const TextRange& left, const TextRange& right) { return *left.begin < *right.begin; });
    std::vector<TextRange> merged;
    for (const auto& range : ranges)
        if (!merged.empty() && *range.begin <= *merged.back().end)
            merged.back().end = TextOffset { std::max(*merged.back().end, *range.end) };
        else
            merged.push_back(range);
    ranges = std::move(merged);
}

}

VerbatimLines collect_verbatim_lines(const ast::ASTGraph& graph, std::string_view text,
                                     std::vector<TextRange> unparsed_ranges, std::optional<LineRange> selection) {
    VerbatimLines collected { std::move(unparsed_ranges), {} };
    auto& ranges = collected.ranges;
    const auto first_line = [](const LineStarts& starts, const TextRange& range) {
        return line_holding(starts, range.begin);
    };
    const auto last_line = [](const LineStarts& starts, const TextRange& range) {
        return line_holding(starts, *range.end > *range.begin ? util::retreat(range.end) : range.begin);
    };
    if (!ranges.empty()) {
        const auto starts = line_starts(text);
        for (const auto& range : ranges) {
            const auto range_first_line = first_line(starts, range);
            const auto range_last_line = last_line(starts, range);
            if (!collected.unparsed_lines.empty() && collected.unparsed_lines.back().last + 1 >= range_first_line)
                collected.unparsed_lines.back().last = std::max(collected.unparsed_lines.back().last, range_last_line);
            else
                collected.unparsed_lines.push_back({ range_first_line, range_last_line });
        }
    }
    add_formatting_off_ranges(graph, text, ranges);
    if (!ranges.empty() || selection) {
        const auto starts = line_starts(text);
        for (auto& range : ranges)
            range = { starts[first_line(starts, range)], next_line_start(starts, text, last_line(starts, range)) };
        if (selection)
            add_lines_outside_selection(starts, text, *selection, ranges);
        merge_overlapping(ranges);
    }
    return collected;
}
}
