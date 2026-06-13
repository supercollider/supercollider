#include "source_utils.hpp"
#include "codepoint.hpp"
#include "text_location.hpp"
#include <optional>
namespace sc::lex::utils {

CodePointIterator::CodePointIterator(const char* text_start, const char* text_end, const char* cur_location) noexcept:
    txt_start(text_start),
    txt_end(text_end),
    txt_iter(cur_location) {}

std::optional<CodePointIterator> CodePointIterator::make(const char* txt_start, const char* txt_end,
                                                         const char* current_location) {
    if (txt_start > txt_end)
        return std::nullopt;
    if (current_location == nullptr)
        return { { txt_start, txt_end, txt_start } };
    if (txt_start <= current_location && current_location <= txt_end)
        return { { txt_start, txt_end, current_location } };
    return std::nullopt;
}

std::optional<CodePoint> CodePointIterator::forwards() noexcept {
    if (txt_iter >= txt_end)
        return std::nullopt;
    const auto [cp, sz] = utf8_sequence_to_codepoint(txt_iter, static_cast<std::size_t>(txt_end - txt_iter));
    if (cp == invalid_utf8_flag)
        return std::nullopt;

    txt_iter += sz;
    return { cp };
}

std::optional<CodePoint> CodePointIterator::backwards() noexcept {
    if (txt_iter <= txt_start)
        return std::nullopt;
    txt_iter -= 1; // enters previous codepoint. We were sat at the start of one.
    const auto [cp, sz] = utf8_sequence_to_codepoint_backwards(txt_start, txt_iter);
    if (cp == invalid_utf8_flag)
        return std::nullopt;
    txt_iter -= (sz - 1);
    return { cp };
}

LineIter::LineIter(CodePointIterator it, std::size_t line_number) noexcept: cp_iter(it), current_line(line_number) {}

std::optional<LineIter> LineIter::make(const char* txt_start, const char* txt_end, SourceCodeLocation loc) {
    auto m_cp_it = CodePointIterator::make(txt_start, txt_end, txt_start + loc.absolute);
    if (!m_cp_it)
        return std::nullopt;
    return { { std::move(*m_cp_it), loc.lineNumber } };
}

std::optional<LineIter::Result> LineIter::forwards() noexcept {
    if (previous_dir == Backward) {
        const auto [cp, sz] = cp_iter.current_codepoint();
        if (is_newline(cp)) {
            cp_iter.forwards();
            current_line += 1;
        }
    }
    previous_dir = Forwards;

    const auto start = cp_iter.current_location();

    bool ends_in_newline_char = false;
    for (auto r = cp_iter.forwards(); r; r = cp_iter.forwards()) {
        if (is_newline(*r)) {
            ends_in_newline_char = true;
            break;
        }
    }

    const auto end = cp_iter.current_location();
    const auto sz = static_cast<std::size_t>(end - start);
    if (sz == 0)
        return std::nullopt;
    else {
        current_line += 1; // assumming we don't overflow.
        return { { start, sz, current_line - 1, ends_in_newline_char } };
    }
}

std::optional<LineIter::Result> LineIter::backwards() noexcept {
    if (previous_dir == Forwards) {}
    previous_dir = Backward;
    const auto end = cp_iter.current_location();
    bool ends_in_newline_char = is_newline(std::get<0>(cp_iter.current_codepoint()));
    for (auto r = cp_iter.backwards(); r; r = cp_iter.backwards()) {
        if (is_newline(*r)) {
            cp_iter.forwards(); // undo
            break;
        }
    }
    const auto start = cp_iter.current_location();
    cp_iter.backwards();
    const auto sz = static_cast<std::size_t>(end - start);
    if (sz == 0)
        return std::nullopt;
    else {
        if (current_line != 0)
            current_line -= 1; // avoid underflow if we are on line zero already.
        return { { start, sz, current_line + 1, ends_in_newline_char } };
    }
}
}
