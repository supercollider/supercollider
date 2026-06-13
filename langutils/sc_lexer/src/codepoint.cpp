#include <codepoint.hpp>
#include "utf8proc.h"
#include <array>

namespace sc::lex {

[[nodiscard]] std::tuple<CodePoint, std::uint8_t> utf8_sequence_to_codepoint(const char* utf8,
                                                                             std::size_t len) noexcept {
    CodePoint cp;
    const auto width_or_error =
        utf8proc_iterate(reinterpret_cast<const std::uint8_t*>(utf8), static_cast<utf8proc_ssize_t>(len), &cp);
    const auto width = static_cast<std::uint8_t>(width_or_error);
    if (width_or_error < 0)
        // Note: all these '1' need to be initalised like std::uint8_t{1} because MSVC thinks '1' is an int and can't
        // see the conversion is fine.
        return { invalid_utf8_flag, std::uint8_t { 1 } };
    else
        return { cp, width };
}


[[nodiscard]] std::tuple<CodePoint, std::uint8_t> utf8_sequence_to_codepoint_backwards(const char* utf8_start,
                                                                                       const char* current) noexcept {
    const auto* const start = reinterpret_cast<const std::uint8_t*>(utf8_start);
    auto c { reinterpret_cast<const std::uint8_t*>(current) };

    const auto is_cont = [](std::uint8_t ch) -> bool { return (ch & 0xC0) == 0x80; };

    utf8proc_ssize_t count { 1 };
    while (is_cont(*c)) {
        --c;
        ++count;
        if (c < start)
            return { invalid_utf8_flag, std::uint8_t { 1 } };
    }

    CodePoint cp;
    const auto width_or_error = utf8proc_iterate(c, count, &cp);
    const auto width { static_cast<std::uint8_t>(width_or_error) };
    if (width_or_error < 0)
        return { invalid_utf8_flag, std::uint8_t { 1 } };
    else
        return { cp, width };
}

[[nodiscard]] std::uint8_t codepoint_size(CodePoint uc) noexcept {
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

[[nodiscard]] std::uint8_t codepoint_width(CodePoint c) noexcept {
    return static_cast<std::uint8_t>(utf8proc_charwidth(c));
}


[[nodiscard]] const char* codepoint_as_whitespace(CodePoint c) noexcept {
    if (c == '\t')
        return "\t";
    else
        switch (codepoint_width(c)) {
        case 0:
            return "";
        case 1:
            return " ";
        case 2:
            return "  ";
        case 3:
            return "   ";
        default:
            return " ";
        }
}

[[nodiscard]] bool is_space(CodePoint c) noexcept {
    return c == ' ' || c == 0x00a0 // This is the Ux00a0, no-break-space, often occurs when copying code from the web
        || ((0x2000 <= c) && (c <= 0x200A)) // en/em space/quads and other spaces
        || c == 0x202F // narrow no break
        || c == 0x205F // medium mathematical space
        || c == 0x3000 // ideogrpahic space
        ;
}

[[nodiscard]] bool is_newline(CodePoint c) noexcept {
    return c == '\n' // line feed
        || c == '\v' // vertical tab (not classified as a tab)
        || c == '\f' // form feed
        || c == '\r' // carrage return
        || c == 0x0085 // next line
        || c == 0x2028 // line separator
        || c == 0x2029 // paragraph separator
        ;
}

[[nodiscard]] bool is_tab(CodePoint c) noexcept { return c == '\t'; }

[[nodiscard]] bool is_whitespace(CodePoint c) noexcept { return is_newline(c) || is_space(c) || is_tab(c); }

[[nodiscard]] bool is_control_code(CodePoint c) noexcept {
    return (1 <= c && c <= 8) || (14 <= c && c <= 31) || c == 127;
}

[[nodiscard]] bool is_lower(CodePoint c) noexcept { return 'a' <= c && c <= 'z'; }

[[nodiscard]] bool is_upper(CodePoint c) noexcept { return 'A' <= c && c <= 'Z'; }

[[nodiscard]] bool is_numeric(CodePoint c) noexcept { return '0' <= c && c <= '9'; }

[[nodiscard]] bool is_start_of_class(CodePoint c) noexcept { return is_upper(c); }

[[nodiscard]] bool is_starting_identifier(CodePoint c) noexcept { return is_lower(c) || is_upper(c); }

[[nodiscard]] bool is_continuing_identifier(CodePoint c) noexcept {
    return is_lower(c) || is_upper(c) || is_numeric(c) || c == '_';
}

inline constexpr std::array<CodePoint, 13> binary_operator_characters { '!', '@', '%', '&', '*', '-', '+',
                                                                        '=', '|', '<', '>', '?', '/' };

[[nodiscard]] bool is_binary_operator_character(CodePoint c) noexcept {
    for (CodePoint b : binary_operator_characters)
        if (b == c)
            return true;
    return false;
}

}
