// Copyright Jordan Henderson 2026
#include "layout/passes.hpp"

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

namespace sc::format {

namespace {

using lex::TokenType;

class Printer {
public:
    Printer(const LayoutDraft& layout, const SyntaxMarks& marks, const Style& style):
        m_layout(layout),
        m_marks(marks),
        m_style(style),
        m_sets_indent(style.indent.kind != Indent::Kind::Keep) {}

    std::string run() {
        m_output.reserve(m_layout.normalised_text.size() + 1);
        for (const auto token : m_layout.whitespace_before.indices()) {
            print_whitespace_before(token);
            if (!m_layout.tokens.contains(token))
                break;
            m_output += m_layout.token_text(token);
            track_brackets(token);
        }
        return std::move(m_output);
    }

private:
    // A byte of the output.
    struct OutputOffsetTag;
    using OutputOffset = util::StrongIndex<std::uint32_t, OutputOffsetTag>;

    struct OutputRange {
        OutputOffset begin;
        OutputOffset end;
    };

    struct OpenBracketTag;
    using OpenBracketIndex = util::StrongIndex<std::uint32_t, OpenBracketTag>;

    struct OpenBracket {
        std::uint32_t opener_line_level;
        std::uint32_t inner_line_level;
        OutputRange opener_line_indent;
        std::optional<OutputRange> first_inner_line_indent;
        bool indents_inner_lines; // not a region that isn't indented
    };

    struct PrintedLine {
        TokenIndex first_token;
        std::uint32_t level;
        OutputRange indent;
    };

    void print_whitespace_before(TokenIndex token) {
        const auto written = m_layout.written_whitespace_before(token);
        if (!split_whitespace_lines(written, m_whitespace_lines)) {
            m_output += written;
            return;
        }
        const bool at_end_of_file = !m_layout.tokens.contains(token);
        // Only a version comment at the very start of the text is the file's, so the whitespace before one stays.
        const bool before_version_comment =
            *token == 0 && !at_end_of_file && m_layout.tokens[token].type == TokenType::VersionComment;
        if (m_layout.whitespace_before[token].touches_verbatim_line)
            print_whitespace_touching_verbatim_line(token, at_end_of_file);
        else if (before_version_comment)
            print_whitespace_as_written();
        else
            print_formatted_whitespace(token, at_end_of_file);
    }

    // A verbatim line's part is written as it is, the first part being the end of the line before.
    void print_whitespace_touching_verbatim_line(TokenIndex token, bool at_end_of_file) {
        for (const auto line : m_whitespace_lines.indices()) {
            const bool line_is_verbatim = *line == 0 && *token > 0
                ? m_layout.tokens[util::retreat(token)].on_verbatim_line
                : m_layout.on_verbatim_line(
                    TextOffset::from(m_whitespace_lines[line].data() - m_layout.normalised_text.data()));
            if (*line > 0)
                m_output += '\n';
            const bool before_next_token = !at_end_of_file && !m_whitespace_lines.contains(util::advance(line));
            if (before_next_token && (*line > 0 || *token == 0))
                start_line(token, m_whitespace_lines[line], line_is_verbatim);
            else if (line_is_verbatim)
                m_output += m_whitespace_lines[line];
            else if (before_next_token) // between two tokens
                write_whitespace(m_whitespace_lines[line]);
            else
                write_trailing_whitespace(m_whitespace_lines[line]);
        }
    }

    void print_whitespace_as_written() {
        for (const auto line : m_whitespace_lines.indices()) {
            if (*line > 0)
                m_output += '\n';
            write_whitespace(m_whitespace_lines[line]);
        }
    }

    // Every line but the last ends in a newline, and with the newline it ends a line of the text. At the end of the
    // text, a final newline ends the last line too.
    void print_formatted_whitespace(TokenIndex token, bool at_end_of_file) {
        const auto newlines = m_layout.whitespace_before[token].newlines;
        // A line break where the text has none.
        const bool new_line_break = newlines > 0 && m_whitespace_lines.size() == 1;
        for (const auto line : util::range(WhitespaceLineIndex { 0 }, WhitespaceLineIndex { newlines })) {
            if (m_whitespace_lines.contains(util::advance(line))
                || (at_end_of_file && m_whitespace_lines.contains(line)))
                write_trailing_whitespace(m_whitespace_lines[line]);
            m_output += '\n';
        }
        if (at_end_of_file) {
            if (m_style.final_newline != FinalNewline::One)
                write_trailing_whitespace(m_whitespace_lines.back());
        } else if (newlines == 0 && *token != 0) {
            if (const auto spaces = m_layout.whitespace_before[token].spaces; spaces != Whitespace::spaces_as_written)
                m_output.append(spaces, ' ');
            else if (m_whitespace_lines.size() > 1) // joined
                m_output += ' ';
            else
                write_whitespace(m_whitespace_lines.back());
        } else {
            start_line(token, new_line_break ? std::nullopt : std::optional { m_whitespace_lines.back() });
        }
    }

    void write_trailing_whitespace(std::string_view whitespace) {
        if (m_style.trailing_whitespace != TrailingWhitespace::Remove)
            write_whitespace(whitespace);
    }

    void write_whitespace(std::string_view whitespace) {
        if (m_style.unicode_spaces == UnicodeSpaces::Keep) {
            m_output += whitespace;
            return;
        }
        for (auto remaining = whitespace; !remaining.empty();) {
            const auto [code_point, size] = first_codepoint(remaining);
            const auto length = std::max<std::size_t>(size, 1);
            if (size > 1)
                m_output += ' ';
            else
                m_output += remaining.substr(0, length);
            remaining.remove_prefix(length);
        }
    }

    // With verbatim, the line keeps its written indentation, but its level is still worked out for the lines after.
    void start_line(TokenIndex first_token, std::optional<std::string_view> written_indent, bool is_verbatim = false) {
        const auto indent_begin = OutputOffset::from(m_output.size());
        if (m_sets_indent) {
            m_current_line_level = level_of_line_starting_with(first_token);
            if (is_verbatim)
                m_output += *written_indent;
            else if (m_style.indent.kind == Indent::Kind::Tab)
                m_output.append(m_current_line_level, '\t');
            else
                m_output.append(static_cast<std::size_t>(m_current_line_level) * m_style.indent.spaces, ' ');
        } else if (is_verbatim) {
            m_output += *written_indent;
        } else if (written_indent) {
            write_whitespace(*written_indent);
        } else {
            m_output += indent_for_new_line(first_token);
        }
        m_current_line_indent = { indent_begin, OutputOffset::from(m_output.size()) };
        m_printed_lines.push_back({ first_token, m_current_line_level, m_current_line_indent });
        const bool starts_with_closer =
            m_layout.tokens.contains(first_token) && lex::is_close_bracket(m_layout.tokens[first_token].type);
        if (m_open_brackets.empty() || starts_with_closer)
            return;
        auto& innermost = m_open_brackets.back();
        if (!innermost.first_inner_line_indent)
            innermost.first_inner_line_indent = m_current_line_indent;
    }

    std::string printed_text(OutputRange range) const {
        return m_output.substr(*range.begin, *range.end - *range.begin);
    }

    // The indentation of a line the formatter made, with every line's indentation kept: a closer's opener's line's,
    // else the first line inside the bracket's, else one more than the opener's line (the same for a region that
    // isn't indented), a tab unless that line is indented by spaces.
    // A `switch` or `case` pair's line is one more than the message's line.
    std::string indent_for_new_line(TokenIndex first_token) const {
        if (const auto* relative_indent = relative_indent_line_starting_with(first_token);
            relative_indent && relative_indent->kind == RelativeIndentLine::Kind::ControlFunction)
            return one_level_deeper(printed_text(printed_line_holding(relative_indent->expression_start).indent));
        if (m_open_brackets.empty())
            return {};
        const auto& innermost = m_open_brackets.back();
        const auto opener_line_indent = printed_text(innermost.opener_line_indent);
        if (m_layout.tokens.contains(first_token) && lex::is_close_bracket(m_layout.tokens[first_token].type))
            return opener_line_indent;
        if (innermost.first_inner_line_indent)
            return printed_text(*innermost.first_inner_line_indent);
        return innermost.indents_inner_lines ? one_level_deeper(opener_line_indent) : opener_line_indent;
    }

    static std::string one_level_deeper(const std::string& indent) {
        const bool spaces = !indent.empty() && indent.find('\t') == std::string::npos;
        return indent + (spaces ? "    " : "\t");
    }

    const RelativeIndentLine* relative_indent_line_starting_with(TokenIndex first_token) const {
        const auto& relative_lines = m_marks.relative_indent_lines;
        const auto relative_indent = std::lower_bound(
            relative_lines.begin(), relative_lines.end(), first_token,
            [](const RelativeIndentLine& line, TokenIndex token) { return *line.first_token < *token; });
        return relative_indent == relative_lines.end() || relative_indent->first_token != first_token
            ? nullptr
            : &*relative_indent;
    }

    // Level 0 before the first printed line, which only a version comment starting the text comes before, see
    // print_whitespace_before().
    PrintedLine printed_line_holding(TokenIndex token) const {
        const auto first_after =
            std::upper_bound(m_printed_lines.begin(), m_printed_lines.end(), token,
                             [](TokenIndex held, const PrintedLine& line) { return *held < *line.first_token; });
        const OutputRange no_indent { OutputOffset { 0 }, OutputOffset { 0 } };
        return first_after == m_printed_lines.begin() ? PrintedLine { TokenIndex { 0 }, 0, no_indent }
                                                      : *std::prev(first_after);
    }

    void track_brackets(TokenIndex token) {
        const auto type = m_layout.tokens[token].type;
        const auto& token_marks = m_marks.tokens[token];
        if (lex::is_open_bracket(type)) {
            const bool indented = indents_inner_lines(token_marks);
            (void)m_open_brackets.push_back({ m_current_line_level, m_current_line_level + (indented ? 1 : 0),
                                              m_current_line_indent, std::nullopt, indented });
        } else if (lex::is_close_bracket(type) && !m_open_brackets.empty()) {
            m_open_brackets.pop_back();
        }
    }

    // Only a top-level region can be left unindented.
    bool indents_inner_lines(const TokenMarks& opener) const {
        if (!opener.opens_region || !m_open_brackets.empty())
            return true;
        if (m_style.indent_regions == IndentRegions::Keep)
            return opener.region_indented_in_source;
        return m_style.indent_regions == IndentRegions::On;
    }

    std::uint32_t level_inside_innermost_bracket() const {
        return m_open_brackets.empty() ? 0 : m_open_brackets.back().inner_line_level;
    }

    std::uint32_t level_of_line_starting_with(TokenIndex first_token) const {
        const auto& tokens = m_layout.tokens;
        const auto type = tokens[first_token].type;
        if (lex::is_comment(type)) {
            // At the level of what comes next, inside its bracket if it is a closing one.
            const auto next_code = m_layout.first_non_comment_from(first_token);
            if (!tokens.contains(next_code) || lex::is_close_bracket(tokens[next_code].type))
                return level_inside_innermost_bracket();
            return level_of_code_line_starting_with(next_code);
        }
        if (lex::is_close_bracket(type)) {
            // Back to the line of the outermost bracket the line's first closing brackets close.
            auto outermost_closed = OpenBracketIndex::from(m_open_brackets.size());
            std::uint32_t level =
                *outermost_closed > 0 ? m_open_brackets[util::retreat(outermost_closed)].opener_line_level : 0;
            const auto is_leading_closer = [&](TokenIndex token) {
                if (!tokens.contains(token) || !lex::is_close_bracket(tokens[token].type))
                    return false;
                return token == first_token || m_layout.whitespace_before[token].newlines == 0;
            };
            auto closer = first_token;
            while (*outermost_closed > 0 && is_leading_closer(closer)) {
                outermost_closed = util::retreat(outermost_closed);
                level = std::min(level, m_open_brackets[outermost_closed].opener_line_level);
                closer = util::advance(closer);
            }
            return level;
        }
        return level_of_code_line_starting_with(first_token);
    }

    // Not a comment or a closing bracket.
    std::uint32_t level_of_code_line_starting_with(TokenIndex first_token) const {
        // Code at the top level of an scd file is often run a line at a time, so it is never indented.
        if (m_open_brackets.empty())
            return 0;
        if (const auto level = level_relative_to_expression(first_token))
            return *level;
        return level_inside_innermost_bracket() + (continues_previous_line(first_token) ? 1 : 0);
    }

    bool continues_previous_line(TokenIndex first_token) const {
        // A `{` starting a line stays at the level of what it belongs to, as most code puts it (Allman style).
        if (m_marks.tokens[first_token].starts_list_item || m_layout.is_open_brace(first_token))
            return false;
        const auto previous_code = m_layout.previous_non_comment(first_token);
        return previous_code && !lex::is_open_bracket(m_layout.tokens[*previous_code].type);
    }

    // A line placed relative to the line its expression starts on: one level more, or the same, but never less than
    // the lines inside its bracket. A chain's by the option, a string's the same if the string starts that line, and a
    // `switch` or `case` pair's one more.
    std::optional<std::uint32_t> level_relative_to_expression(TokenIndex first_token) const {
        const auto* relative_indent = relative_indent_line_starting_with(first_token);
        if (!relative_indent)
            return std::nullopt;
        const auto expression_line = printed_line_holding(relative_indent->expression_start);
        const auto chain_option = m_style.indent_method_chains;
        bool indent_more { true };
        if (relative_indent->kind == RelativeIndentLine::Kind::MethodChain)
            indent_more = chain_option == IndentMethodChains::On
                || (chain_option == IndentMethodChains::Keep && relative_indent->chain_indented_in_source);
        else if (relative_indent->kind == RelativeIndentLine::Kind::StringContinuation)
            indent_more = expression_line.first_token != relative_indent->expression_start;
        return std::max(level_inside_innermost_bracket(), expression_line.level + (indent_more ? 1 : 0));
    }

    const LayoutDraft& m_layout;
    const SyntaxMarks& m_marks;
    const Style& m_style;
    const bool m_sets_indent;
    std::string m_output {};
    WhitespaceLines m_whitespace_lines {};
    util::IndexedVector<OpenBracketIndex, OpenBracket> m_open_brackets {};
    std::vector<PrintedLine> m_printed_lines {};
    std::uint32_t m_current_line_level { 0 };
    OutputRange m_current_line_indent { OutputOffset { 0 }, OutputOffset { 0 } };
};

}

std::string print(const LayoutDraft& layout, const SyntaxMarks& marks, const Style& style) {
    return Printer { layout, marks, style }.run();
}
}
