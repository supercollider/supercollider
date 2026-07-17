#pragma once
#include <cstdint>
#include <cstddef>
#include <limits>
#include <tuple>

namespace sc::lex {

using CodePoint = std::int32_t;
inline constexpr CodePoint invalid_utf8_flag { std::numeric_limits<CodePoint>::max() };

// Returns codepoint and the number of bytes consumed.
// Returns {invalid_utf8_flag, 0} when going past the end.
[[nodiscard]] std::tuple<CodePoint, std::uint8_t> utf8_sequence_to_codepoint(const char* utf8,
                                                                             std::size_t len) noexcept;

// Will return {invalid_utf8_flag, 0} when going before the BEGINNING of the text.
[[nodiscard]] std::tuple<CodePoint, std::uint8_t> utf8_sequence_to_codepoint_backwards(const char* utf8_start,
                                                                                       const char* current) noexcept;

// Assumes codepoint is valid.
[[nodiscard]] std::uint8_t codepoint_size(CodePoint c) noexcept;

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
[[nodiscard]] bool is_newline(CodePoint c) noexcept;
[[nodiscard]] bool is_space(CodePoint c) noexcept;
[[nodiscard]] bool is_tab(CodePoint c) noexcept;
[[nodiscard]] bool is_whitespace(CodePoint c) noexcept;
[[nodiscard]] bool is_control_code(CodePoint c) noexcept;
[[nodiscard]] bool is_lower(CodePoint c) noexcept;
[[nodiscard]] bool is_upper(CodePoint c) noexcept;
[[nodiscard]] bool is_numeric(CodePoint c) noexcept;
[[nodiscard]] bool is_start_of_class(CodePoint c) noexcept;
[[nodiscard]] bool is_starting_identifier(CodePoint c) noexcept;
[[nodiscard]] bool is_continuing_identifier(CodePoint c) noexcept;
[[nodiscard]] bool is_binary_operator_character(CodePoint c) noexcept;

}
