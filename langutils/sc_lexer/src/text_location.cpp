#include <text_location.hpp>

using namespace sc::lex;

// assuming the line number and column are correct and that the source text is the same means we only need to comare
// the absolute byte offset.
[[nodiscard]] bool SourceCodeLocation::operator==(const SourceCodeLocation& o) const noexcept {
    return absolute == o.absolute && line_number == o.line_number && column == o.column;
}

[[nodiscard]] bool SourceCodeLocation::operator!=(const SourceCodeLocation& o) const noexcept {
    return absolute != o.absolute;
}

[[nodiscard]] bool SourceCodeLocation::operator<(const SourceCodeLocation& o) const noexcept {
    return absolute < o.absolute;
}

[[nodiscard]] bool SourceCodeLocation::operator>(const SourceCodeLocation& o) const noexcept {
    return absolute > o.absolute;
}

[[nodiscard]] bool SourceCodeLocation::operator<=(const SourceCodeLocation& o) const noexcept {
    return absolute <= o.absolute;
}

[[nodiscard]] bool SourceCodeLocation::operator>=(const SourceCodeLocation& o) const noexcept {
    return absolute >= o.absolute;
}

[[nodiscard]] bool FileCodeLocation::operator==(const FileCodeLocation& o) const noexcept {
    return absolute == o.absolute && line_number == o.line_number && column == o.column;
}

[[nodiscard]] bool FileCodeLocation::operator!=(const FileCodeLocation& o) const noexcept {
    return absolute != o.absolute;
}

[[nodiscard]] bool FileCodeLocation::operator<(const FileCodeLocation& o) const noexcept {
    return absolute < o.absolute;
}

[[nodiscard]] bool FileCodeLocation::operator>(const FileCodeLocation& o) const noexcept {
    return absolute > o.absolute;
}

[[nodiscard]] bool FileCodeLocation::operator<=(const FileCodeLocation& o) const noexcept {
    return absolute <= o.absolute;
}

[[nodiscard]] bool FileCodeLocation::operator>=(const FileCodeLocation& o) const noexcept {
    return absolute >= o.absolute;
}

[[nodiscard]] SourceCodeRange SourceCodeRange::range(SourceCodeRange left, SourceCodeRange right) {
    return { left.begin, right.end };
}

[[nodiscard]] std::size_t sc::lex::SourceCodeRange::size() const { return end.absolute - begin.absolute; }

[[nodiscard]] std::size_t sc::lex::SourceCodeRange::line_count() const {
    return (end.line_number - begin.line_number) + 1;
}

[[nodiscard]] bool sc::lex::SourceCodeRange::operator==(const SourceCodeRange& o) const noexcept {
    return begin == o.begin && end == o.end;
}
[[nodiscard]] bool sc::lex::SourceCodeRange::operator!=(const SourceCodeRange& o) const noexcept {
    return !(*this == o);
}
[[nodiscard]] bool sc::lex::SourceCodeRange::operator<(const SourceCodeRange& o) const noexcept {
    return begin < o.begin && end < o.begin;
}
[[nodiscard]] bool sc::lex::SourceCodeRange::operator>(const SourceCodeRange& o) const noexcept {
    return begin < o.end && end > o.end;
}


[[nodiscard]] sc::lex::FileCodeRange sc::lex::FileCodeRange::range(FileCodeRange left, FileCodeRange right) {
    return { left.begin, right.end };
}

[[nodiscard]] std::size_t sc::lex::FileCodeRange::size() const { return end.absolute - begin.absolute; }


[[nodiscard]] bool sc::lex::FileCodeRange::operator==(const FileCodeRange& o) const noexcept {
    return begin == o.begin && end == o.end;
}
[[nodiscard]] bool sc::lex::FileCodeRange::operator!=(const FileCodeRange& o) const noexcept { return !(*this == o); }
[[nodiscard]] bool sc::lex::FileCodeRange::operator<(const FileCodeRange& o) const noexcept {
    return begin < o.begin && end < o.begin;
}
[[nodiscard]] bool sc::lex::FileCodeRange::operator>(const FileCodeRange& o) const noexcept {
    return begin < o.end && end > o.end;
}
[[nodiscard]] std::size_t sc::lex::FileCodeRange::line_count() const {
    return (end.line_number - begin.line_number) + 1;
}

[[nodiscard]] FileCodeRange FileCodeRange::sourceToFile(FileCodeLocation start_of_source, SourceCodeRange src) {
    const auto [ab, line, column] = start_of_source;
    return { { src.begin.absolute + ab, src.begin.line_number + line,
               src.begin.line_number == 0 ? src.begin.column + column : src.begin.column },
             { src.end.absolute + ab, src.end.line_number + line,
               src.end.column == 0 ? src.end.column + column : src.end.column } };
}
