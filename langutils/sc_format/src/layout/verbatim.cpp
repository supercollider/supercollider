// Copyright Jordan Henderson 2026
#include "layout/passes.hpp"

namespace sc::format {

namespace {
// Whether a line that starts inside the written whitespace, e.g., a blank line, is verbatim.
bool starts_verbatim_line(const LayoutDraft& layout, std::string_view written) {
    const auto written_begin = written.data() - layout.normalised_text.data();
    auto newline = written.find('\n');
    while (newline != std::string_view::npos) {
        if (layout.on_verbatim_line(TextOffset::from(written_begin + newline + 1)))
            return true;
        newline = written.find('\n', newline + 1);
    }
    return false;
}
}

void mark_tokens_on_verbatim_lines(LayoutDraft& layout, const std::vector<TextRange>& verbatim_lines) {
    auto verbatim_line = verbatim_lines.begin();
    for (auto& token : layout.tokens.raw()) {
        while (verbatim_line != verbatim_lines.end() && *verbatim_line->end <= *token.begin)
            ++verbatim_line;
        token.on_verbatim_line = verbatim_line != verbatim_lines.end() && *verbatim_line->begin < *token.end;
        layout.any_token_on_verbatim_line = layout.any_token_on_verbatim_line || token.on_verbatim_line;
    }
    layout.verbatim_lines = verbatim_lines;
}

void restore_verbatim_whitespace(LayoutDraft& layout) {
    if (!layout.any_token_on_verbatim_line && layout.verbatim_lines.empty())
        return;
    const auto& tokens = layout.tokens;
    for (const auto token : layout.whitespace_before.indices()) {
        const auto written = layout.written_whitespace_before(token);
        const bool after_verbatim_token = *token > 0 && tokens[util::retreat(token)].on_verbatim_line;
        const bool before_verbatim_token = tokens.contains(token) && tokens[token].on_verbatim_line;
        const bool text_starts_verbatim = *token == 0 && layout.on_verbatim_line(TextOffset { 0 });
        const bool touches_verbatim = after_verbatim_token || before_verbatim_token || text_starts_verbatim
            || starts_verbatim_line(layout, written);
        if (!touches_verbatim)
            continue;
        auto& whitespace = layout.whitespace_before[token];
        whitespace.newlines = count_newlines(written);
        whitespace.spaces = Whitespace::spaces_as_written;
        whitespace.touches_verbatim_line = true;
    }
}
}
