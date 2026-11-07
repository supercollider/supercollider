#include "sc_lexer/details/codepoint_stream.hpp"

namespace sc::lex {

[[nodiscard]] src::FileCodeRange CodePointStream::source_to_file(const src::SourceCodeRange& src) const {
    return src::FileCodeRange { source_to_file(src.begin), source_to_file(src.end) };
}

[[nodiscard]] src::FileCodeLocation CodePointStream::source_to_file(const src::SourceCodeLocation& src) const {
    return { src.absolute + source_start_in_file.absolute, src.line_number + source_start_in_file.line_number,
             src.line_number == 0 ? src.codepoint_in_line + source_start_in_file.codepoint_in_line
                                  : src.codepoint_in_line };
}

[[nodiscard]] std::tuple<const char*, std::size_t>
CodePointStream::source_code_range_to_text(const src::SourceCodeRange& range) const {
    return { source + range.begin.absolute, range.end.absolute - range.begin.absolute };
}
}
