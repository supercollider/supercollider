#pragma once
#include <cstdint>
#include <cstddef>
#include <limits>
#include <tuple>

namespace sc::src {

using CodePoint = std::int32_t;
inline constexpr CodePoint invalid_utf8_flag { std::numeric_limits<CodePoint>::max() };

// The multi-byte case of utf8_sequence_to_codepoint.
[[nodiscard]] std::tuple<CodePoint, std::uint8_t> utf8_multibyte_sequence_to_codepoint(const char* utf8,
                                                                                       std::size_t len) noexcept;

// Returns codepoint and the number of bytes consumed.
// Returns {invalid_utf8_flag, 0} when going past the end, and {-1, 0} for a length of 0, so check for the end first.
// Inline, as the lexer calls it for every character, which is nearly always ascii.
[[nodiscard]] inline std::tuple<CodePoint, std::uint8_t> utf8_sequence_to_codepoint(const char* utf8,
                                                                                    std::size_t len) noexcept {
    if (len > 0 && static_cast<unsigned char>(*utf8) < 0x80)
        return { static_cast<unsigned char>(*utf8), std::uint8_t { 1 } };
    return utf8_multibyte_sequence_to_codepoint(utf8, len);
}

// Will return {invalid_utf8_flag, 0} when going before the BEGINNING of the text.
[[nodiscard]] std::tuple<CodePoint, std::uint8_t> utf8_sequence_to_codepoint_backwards(const char* utf8_start,
                                                                                       const char* current) noexcept;

// Assumes codepoint is valid.
[[nodiscard]] inline std::uint8_t codepoint_size(CodePoint uc) noexcept {
    // Modified from the ut8proc library.
    if (uc < 0x80) {
        return 1;
    } else if (uc < 0x800) {
        return 2;
    } else if (uc < 0x10000) {
        return 3;
    } else
        return 4;
}

// Guess the display width. Not accurate in some cases, but usually good when using a monospaced font
//   Except for some asian characters, but probably good enough for supercollider's use case.
// Does not work with tabs as this depends on the editor.
[[nodiscard]] std::uint8_t codepoint_width(CodePoint c) noexcept;

// Converts a codepoint into whitespace that takes up the same amount of visual space (approximate).
// Will return either a string of zero or more spaces, or a single tab.
// Newline characters return "", these must be processed separately.
[[nodiscard]] const char* codepoint_as_whitespace(CodePoint c) noexcept;


// Note: these are supercollider's defintions, not unicode's.
// The only valid newline ascii character is \n. \r\n and \r are not allowed!!
// Supercollider requires that the source code be normalised before tokenising, see normalise_source.hpp
// Inline, as the lexer calls them for every character.
[[nodiscard]] inline bool is_newline(CodePoint c) noexcept {
    return c == '\n' // line feed
        || c == 0x0085 // next line
        || c == 0x2028 // line separator
        || c == 0x2029 // paragraph separator
        ;
}

[[nodiscard]] inline bool is_space(CodePoint c) noexcept {
    return c == ' ' || c == 0x00a0 // This is the Ux00a0, no-break-space, often occurs when copying code from the web
        || ((0x2000 <= c) && (c <= 0x200A)) // en/em space/quads and other spaces
        || c == 0x202F // narrow no break
        || c == 0x205F // medium mathematical space
        || c == 0x3000 // ideograpahic space
        ;
}

[[nodiscard]] inline bool is_tab(CodePoint c) noexcept { return c == '\t'; }

[[nodiscard]] inline bool is_whitespace(CodePoint c) noexcept { return is_newline(c) || is_space(c) || is_tab(c); }

[[nodiscard]] inline bool is_control_code(CodePoint c) noexcept {
    return (1 <= c && c <= 8) || (14 <= c && c <= 31) || c == 127;
}

[[nodiscard]] inline bool is_lower(CodePoint c) noexcept { return 'a' <= c && c <= 'z'; }

[[nodiscard]] inline bool is_upper(CodePoint c) noexcept { return 'A' <= c && c <= 'Z'; }

[[nodiscard]] inline bool is_numeric(CodePoint c) noexcept { return '0' <= c && c <= '9'; }

[[nodiscard]] inline bool is_start_of_class(CodePoint c) noexcept { return is_upper(c); }

[[nodiscard]] inline bool is_starting_identifier(CodePoint c) noexcept { return is_lower(c) || is_upper(c); }

[[nodiscard]] inline bool is_continuing_identifier(CodePoint c) noexcept {
    return is_lower(c) || is_upper(c) || is_numeric(c) || c == '_';
}

[[nodiscard]] inline bool is_binary_operator_character(CodePoint c) noexcept {
    switch (c) {
    case '!':
    case '@':
    case '%':
    case '&':
    case '*':
    case '-':
    case '+':
    case '=':
    case '|':
    case '<':
    case '>':
    case '?':
    case '/':
        return true;
    default:
        return false;
    }
}
}
