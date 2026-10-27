#include "SourceHighlight.hpp"
#include "sc_source/codepoint.hpp"
#include "sc_source/source_utils.hpp"
#include <iomanip>

std::ostream& streamSourceCodeWithHighlight(std::ostream& ss, const DiagnosticHighlight& highlight, bool printSource,
                                            const char* newlinePrefixPtr, size_t newlinePrefixSz) {
    const auto& [filepath, txt, txt_len, range, source_line, source_column, description] { highlight };

    const auto starting_line = source_line + range.begin.line_number;
    const auto start_column = source_column + range.end.column;

    if (filepath)
        ss << "file://" << filepath << ':' << starting_line + 1 << ':' << start_column + 1 << '\n';
    else
        ss << "unknown file:" << starting_line + 1 << ':' << start_column + 1 << '\n';

    if (!txt || txt_len == 0) {
        ss.write(newlinePrefixPtr, newlinePrefixSz);
        ss << description;
        return ss; // nothing left to do
    }
    if (!printSource)
        return ss;

    const auto selection_start = txt + range.begin.absolute;
    const auto selection_end = txt + range.end.absolute;

    auto maybe_iter = sc::src::utils::LineIter::make(txt, txt + txt_len, range.begin);
    if (!maybe_iter.has_value())
        return ss;

    auto iter = std::move(*maybe_iter);

    if (range.line_count() <= 4) {
        iter.backwards();
        iter.backwards();
    }

    const auto single_line_error = range.line_count() == 1;

    for (auto line = iter.forwards(); line; line = iter.forwards()) {
        const auto [ptr, sz, line_number, ends_in_newline_char] = *line;
        const auto real_line_number = line_number + 1 + source_line;

        // If is outside of the highlight range
        if (line_number < range.begin.line_number || line_number > range.end.line_number) {
            ss.write(newlinePrefixPtr, newlinePrefixSz);
            ss << std::setfill(' ') << std::setw(5) << real_line_number << " │ ";
            ss.write(ptr, sz);
            if (!ends_in_newline_char)
                ss << '\n';
            if (line_number > range.end.line_number + 2)
                break;
            else
                continue;
        }
        // Otherwise...

        if (single_line_error) {
            ss.write(newlinePrefixPtr, newlinePrefixSz);
            ss << std::setfill(' ') << std::setw(5) << real_line_number << " │ ";
            ss.write(ptr, sz);

            if (!ends_in_newline_char)
                ss << '\n';

            ss.write(newlinePrefixPtr, newlinePrefixSz);
            ss << std::setfill(' ') << std::setw(5) << ' ' << " ┆ ";
            auto cp_iter = sc::src::utils::CodePointIterator::make(ptr, ptr + sz, ptr).value();

            for (auto cp = cp_iter.forwards(); cp; cp = cp_iter.forwards()) {
                if (cp_iter.current_location() <= selection_start) {
                    ss << sc::src::codepoint_as_whitespace(*cp);
                } else if (cp_iter.current_location() <= selection_end) {
                    const auto w { sc::src::codepoint_width(*cp) };
                    for (size_t i { 0 }; i < w; ++i)
                        ss << '^';
                } else
                    break;
            }

            ss << " " << description << '\n';
        } else {
            ss.write(newlinePrefixPtr, newlinePrefixSz);
            ss << std::setfill(' ') << std::setw(5) << real_line_number << "*│ ";
            ss.write(ptr, sz);

            if (!ends_in_newline_char)
                ss << '\n';

            if (line_number == range.end.line_number) {
                ss.write(newlinePrefixPtr, newlinePrefixSz);
                ss << "     >│ ^ " << description << '\n';
            }
        }
    }
    return ss;
}
