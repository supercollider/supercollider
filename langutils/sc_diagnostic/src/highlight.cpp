#include "sc_diagnostic/highlight.hpp"
#include "sc_source/codepoint.hpp"
#include "sc_source/source_utils.hpp"
#include <iomanip>

namespace sc::diag {

std::ostream& streamHighlight(std::ostream& ss, const Highlight& highlight, bool print_source,
                              std::string_view line_prefix, const std::filesystem::path* relative_to) {
    const auto& [filepath, source, range, txt_start, description] { highlight };
    const auto source_line = txt_start.line_number;
    const auto source_column = txt_start.column;

    const auto starting_line = source_line + range.begin.line_number;
    // Where the range starts, counting from zero. The text's start column only applies to its first line.
    const auto start_column = (range.begin.line_number == 0 ? source_column : 0) + range.begin.column;

    if (filepath && relative_to) {
        const auto relative =
            std::filesystem::absolute(*filepath).lexically_relative(std::filesystem::absolute(*relative_to));
        // Empty when there is no relative path, e.g., on another drive on Windows.
        ss << (relative.empty() ? std::filesystem::absolute(*filepath) : relative).generic_string() << ':'
           << starting_line + 1 << ':' << start_column + 1 << '\n';
    } else if (filepath) {
        const auto abs = std::filesystem::absolute(*filepath).generic_string();
        // A URI path starts with a slash, which a Windows drive letter doesn't, i.e., `file:///C:/...`.
        ss << "file://" << (abs.front() == '/' ? "" : "/") << abs << ':' << starting_line + 1 << ':' << start_column + 1
           << '\n';
    } else
        ss << "unknown file:" << starting_line + 1 << ':' << start_column + 1 << '\n';

    if (source.empty()) {
        ss << line_prefix << description;
        return ss; // nothing to show
    }
    if (!print_source)
        return ss;

    const auto txt = source.data();
    const auto selection_start = txt + range.begin.absolute;
    const auto selection_end = txt + range.end.absolute;

    auto maybe_iter = sc::src::utils::LineIter::make(txt, txt + source.size(), range.begin);
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
            ss << line_prefix << std::setfill(' ') << std::setw(5) << real_line_number << " │ ";
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
            ss << line_prefix << std::setfill(' ') << std::setw(5) << real_line_number << " │ ";
            ss.write(ptr, sz);

            if (!ends_in_newline_char)
                ss << '\n';

            ss << line_prefix << "      ┆ ";
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
            ss << line_prefix << std::setfill(' ') << std::setw(5) << real_line_number << "*│ ";
            ss.write(ptr, sz);

            if (!ends_in_newline_char)
                ss << '\n';

            if (line_number == range.end.line_number)
                ss << line_prefix << "     >│ ^ " << description << '\n';
        }
    }
    return ss;
}

} // namespace sc::diag
