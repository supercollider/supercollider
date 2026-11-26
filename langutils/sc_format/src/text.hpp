// Copyright Jordan Henderson 2026
#pragma once

#include "sc_source/details/codepoint.hpp"
#include "sc_util/indexed_vector.hpp"
#include "sc_util/strong_index.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <tuple>
#include <vector>

namespace sc::format {

// A byte of the normalised text.
struct TextOffsetTag;
using TextOffset = util::StrongIndex<std::uint32_t, TextOffsetTag>;

struct TextRange {
    TextOffset begin;
    TextOffset end;
};

// What the lexer counts as a space.
inline bool is_space_or_tab(src::CodePoint code_point) { return src::is_space(code_point) || src::is_tab(code_point); }

// The code point at the start of s, and its size in bytes, zero if it isn't valid UTF-8.
inline std::tuple<src::CodePoint, std::uint8_t> first_codepoint(std::string_view text) {
    const auto [code_point, size] = src::utf8_sequence_to_codepoint(text.data(), text.size());
    return { code_point, code_point == src::invalid_utf8_flag ? std::uint8_t { 0 } : size };
}

// The text from the start of a view up to the start of the rest of it.
inline std::string_view prefix_before(std::string_view whole, std::string_view suffix) {
    return whole.substr(0, whole.size() - suffix.size());
}

// A `//` comment runs to the newline, so it holds the line's trailing whitespace. That isn't the comment's text: it
// belongs to the whitespace after it.
inline std::string_view trim_line_comment(std::string_view comment) {
    auto trimmed = comment.substr(0, 0);
    for (auto remaining = comment; !remaining.empty();) {
        const auto [code_point, size] = first_codepoint(remaining);
        remaining.remove_prefix(std::max<std::size_t>(size, 1));
        if (size == 0 || !is_space_or_tab(code_point))
            trimmed = prefix_before(comment, remaining);
    }
    return trimmed;
}

struct WhitespaceLineTag;
using WhitespaceLineIndex = util::StrongIndex<std::uint32_t, WhitespaceLineTag>;
// One more than the newlines.
using WhitespaceLines = util::IndexedVector<WhitespaceLineIndex, std::string_view>;

// Splits whitespace into lines at newlines. False if it isn't all whitespace, e.g., the text after a NUL,
// where the lexer stops.
inline bool split_whitespace_lines(std::string_view whitespace, WhitespaceLines& lines) {
    lines.raw().clear();
    auto line = whitespace;
    for (auto remaining = whitespace; !remaining.empty();) {
        const auto [code_point, size] = first_codepoint(remaining);
        if (size == 0 || !(is_space_or_tab(code_point) || src::is_newline(code_point)))
            return false;
        if (src::is_newline(code_point)) {
            (void)lines.push_back(prefix_before(line, remaining));
            line = remaining.substr(size);
        }
        remaining.remove_prefix(size);
    }
    (void)lines.push_back(line);
    return true;
}

inline std::uint32_t count_newlines(std::string_view whitespace) {
    std::uint32_t newlines { 0 };
    for (auto remaining = whitespace; !remaining.empty();) {
        const auto [code_point, size] = first_codepoint(remaining);
        if (size == 0 || !(is_space_or_tab(code_point) || src::is_newline(code_point)))
            break;
        newlines += src::is_newline(code_point) ? 1 : 0;
        remaining.remove_prefix(size);
    }
    return newlines;
}
}
