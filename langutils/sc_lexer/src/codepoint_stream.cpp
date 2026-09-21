#include "codepoint_stream.hpp"
#include <cstddef>

namespace sc::lex {

[[nodiscard]] FileCodeRange CodePointStream::source_to_file(const SourceCodeRange& src) const {
    return FileCodeRange { source_to_file(src.begin), source_to_file(src.end) };
}

[[nodiscard]] FileCodeLocation CodePointStream::source_to_file(const SourceCodeLocation& src) const {
    return { src.absolute + source_start_in_file.absolute, src.line_number + source_start_in_file.line_number,
             src.line_number == 0 ? src.column + source_start_in_file.column : src.column };
}

[[nodiscard]] std::tuple<SourceCodeLocation, CodePoint> CodePointStream::start_token() {
    if (state.prev_was_newline) {
        state.current_line_number += 1;
        state.current_column_count = 0;
        state.prev_was_newline = false;
    }
    return { { state.next_byte_offest, state.current_line_number, state.current_column_count }, advance() };
}

[[nodiscard]] SourceCodeLocation CodePointStream::end_token() const {
    return { state.next_byte_offest, state.current_line_number, state.current_column_count };
}

void CodePointStream::State::update(CodePoint next, std::uint8_t sz) noexcept {
    if (prev_was_newline) {
        current_line_number += 1;
        current_column_count = 0;
        prev_was_newline = false;
    } else {
        // TODO: this is wrong and should be removed.
        current_column_count += 1; // codepoint_width(next);
    }
    prev_was_newline = is_newline(next);
    next_byte_offest += sz;
}

CodePoint CodePointStream::advance() {
    // outside snippet
    if (source + state.next_byte_offest >= snippet + snippet_len)
        return 0;

    const auto remaining = std::max<ptrdiff_t>(0, (snippet + snippet_len) - (source + state.next_byte_offest));

    const auto [c, sz] =
        utf8_sequence_to_codepoint(source + state.next_byte_offest, static_cast<std::size_t>(remaining));

    state.update(c, sz);

    return c;
}

[[nodiscard]] CodePoint CodePointStream::advance_and_peek() {
    advance();
    return peek();
}

[[nodiscard]] std::tuple<const char*, std::size_t>
CodePointStream::source_code_range_to_text(const SourceCodeRange& range) const {
    return { source + range.begin.absolute, range.end.absolute - range.begin.absolute };
}

[[nodiscard]] CodePoint CodePointStream::peek() const { return peek_n<1>()[0]; }

}
