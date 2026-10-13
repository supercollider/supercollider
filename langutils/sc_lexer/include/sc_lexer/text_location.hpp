#pragma once
#include <cassert>
#include <cstddef>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Locations in source code snippets or files
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace sc::lex {

// A location point in a source code snippet, might not be the whole file.
struct SourceCodeLocation {
    [[nodiscard]] bool operator==(const SourceCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator!=(const SourceCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator<(const SourceCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator>(const SourceCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator<=(const SourceCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator>=(const SourceCodeLocation& o) const noexcept;

    // Offset as a byte index into the text.
    std::size_t absolute { 0 };
    // Zero indexed, first line is zero.
    std::size_t line_number { 0 };
    // Byte offset in line.. NOT the visual column.
    std::size_t column { 0 };
};

// A location point inside an entire file
struct FileCodeRange;
struct FileCodeLocation {
    [[nodiscard]] bool operator==(const FileCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator!=(const FileCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator<(const FileCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator>(const FileCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator<=(const FileCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator>=(const FileCodeLocation& o) const noexcept;

    // Offset as a byte index into the text.
    std::size_t absolute { 0 };
    // Zero indexed, first line is zero.
    std::size_t line_number { 0 };
    // Byte offset in line.. NOT the visual column.
    std::size_t column { 0 };
};


// A range within a source code snippet
struct SourceCodeRange {
    SourceCodeRange(SourceCodeLocation b, SourceCodeLocation e): begin(b), end(e) { assert(b <= e); }
    SourceCodeRange(): begin({}), end({}) {}
    [[nodiscard]] static SourceCodeRange range(SourceCodeRange left, SourceCodeRange right);
    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] std::size_t line_count() const;
    [[nodiscard]] bool operator==(const SourceCodeRange& o) const noexcept;
    [[nodiscard]] bool operator!=(const SourceCodeRange& o) const noexcept;
    [[nodiscard]] bool operator<(const SourceCodeRange& o) const noexcept;
    [[nodiscard]] bool operator>(const SourceCodeRange& o) const noexcept;
    [[nodiscard]] bool operator<=(const SourceCodeRange& o) const noexcept;
    [[nodiscard]] bool operator>=(const SourceCodeRange& o) const noexcept;

    [[nodiscard]] SourceCodeRange flatten() const noexcept {
        return { begin, { begin.absolute + 1, begin.line_number, begin.column } };
    }

    SourceCodeLocation begin {}, end {};
};

[[nodiscard]] inline SourceCodeRange range(SourceCodeRange from, SourceCodeRange to) { return { from.begin, to.end }; }

// A range within an entire file
struct FileCodeRange {
    FileCodeRange(FileCodeLocation b, FileCodeLocation e): begin(b), end(e) { assert(b <= e); }
    FileCodeRange(): begin({}), end({}) {}

    [[nodiscard]] static FileCodeRange range(FileCodeRange left, FileCodeRange right);
    [[nodiscard]] static FileCodeRange sourceToFile(FileCodeLocation start_of_source, SourceCodeRange src);
    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] std::size_t line_count() const;
    [[nodiscard]] bool operator==(const FileCodeRange& o) const noexcept;
    [[nodiscard]] bool operator!=(const FileCodeRange& o) const noexcept;
    [[nodiscard]] bool operator<(const FileCodeRange& o) const noexcept;
    [[nodiscard]] bool operator>(const FileCodeRange& o) const noexcept;
    [[nodiscard]] bool operator<=(const FileCodeRange& o) const noexcept;
    [[nodiscard]] bool operator>=(const FileCodeRange& o) const noexcept;

    [[nodiscard]] FileCodeRange flatten() const noexcept {
        return { begin, { begin.absolute + 1, begin.line_number, begin.column } };
    }

    FileCodeLocation begin {}, end {};
};

}
