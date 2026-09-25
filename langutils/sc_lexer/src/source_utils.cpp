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
    if (txt_iter < txt_start)
        txt_iter = txt_start;
    const auto [cp, sz] = utf8_sequence_to_codepoint(txt_iter, static_cast<std::size_t>(txt_end - txt_iter));
    if (cp == invalid_utf8_flag)
        return std::nullopt;

    txt_iter += sz;
    return { cp };
}

std::optional<CodePoint> CodePointIterator::backwards() noexcept {
    if (txt_iter <= txt_start)
        return std::nullopt;
    if (txt_iter > txt_end)
        txt_iter = txt_end;
    txt_iter -= 1; // enters previous codepoint. We were sat at the start of one.
    const auto [cp, sz] = utf8_sequence_to_codepoint_backwards(txt_start, txt_iter);
    if (cp == invalid_utf8_flag)
        return std::nullopt;
    txt_iter -= (sz - 1);
    return { cp };
}

LineIter::LineIter(CodePointIterator it, std::size_t line_number) noexcept: cp_iter(it), current_line(line_number) {}

std::optional<LineIter> LineIter::make(const char* txt_start, const char* txt_end, SourceCodeLocation loc) {
    auto m_cp_it = CodePointIterator::make(txt_start, txt_end, txt_start + loc.absolute - loc.column);
    if (!m_cp_it)
        return std::nullopt;
    return { { std::move(*m_cp_it), loc.line_number } };
}

std::optional<LineIter::Result> LineIter::forwards() noexcept {
    const auto start = cp_iter.current_location();
    const auto [cur_cp, cur_sz] = cp_iter.current_codepoint();
    if (is_newline(cur_cp)) {
        // This means we have a line of only a new line
        cp_iter.forwards();
        current_line += 1;
        const auto end = cp_iter.current_location();
        return { { start, static_cast<size_t>(end - start), current_line - 1, true } };
    }

    for (auto r = cp_iter.forwards(); r; r = cp_iter.forwards()) {
        if (is_newline(*r)) {
            const auto end = cp_iter.current_location();
            // cp_iter.forwards();
            current_line += 1;
            return { { start, static_cast<size_t>(end - start), current_line - 1, true } };
        }
    }

    // hit the end of the file
    const auto end = cp_iter.current_location();
    const auto sz = static_cast<std::size_t>(end - start);
    if (sz == 0)
        return std::nullopt;
    return { { start, sz, current_line, false } };
}

std::optional<LineIter::Result> LineIter::backwards() noexcept {
    if (cp_iter.current_location() <= cp_iter.txt_start)
        return std::nullopt;

    const auto end = cp_iter.current_location();

    const auto new_line = cp_iter.backwards();
    if (!new_line) {
        if (current_line != 0)
            current_line -= 1;
        const auto start = cp_iter.current_location();
        return { { start, static_cast<size_t>(end - start), current_line, true } };
    }


    for (auto b = cp_iter.backwards(); b; b = cp_iter.backwards()) {
        if (is_newline(*b)) {
            cp_iter.forwards();
            current_line -= 1;
            const auto start = cp_iter.current_location();
            return { { start, static_cast<size_t>(end - start), current_line + 1, true } };
        }
    }

    // start of file
    const auto start = cp_iter.current_location();
    if (current_line != 0)
        current_line -= 1;
    return { { start, static_cast<size_t>(end - start), current_line, true } };
}


[[nodiscard]] std::tuple<CodePoint, std::uint8_t> CodePointIterator::current_codepoint() const noexcept {
    if (txt_iter >= txt_end)
        return { 0, std::uint8_t { 0 } };
    return utf8_sequence_to_codepoint(txt_iter, static_cast<std::size_t>(txt_end - txt_iter));
}
}
