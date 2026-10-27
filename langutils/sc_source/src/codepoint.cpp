#include "sc_source/codepoint.hpp"
#include "utf8proc.h"

namespace sc::src {

[[nodiscard]] std::tuple<CodePoint, std::uint8_t> utf8_multibyte_sequence_to_codepoint(const char* utf8,
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
}
