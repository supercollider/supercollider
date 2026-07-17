#pragma once
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
    std::size_t lineNumber { 0 };
    // Codepoint count, note, this is currently broken and doesn't work with multicodepoint graphemes.
    std::size_t column { 0 };
};

// A location point inside an entire file
struct FileCodeLocation {
    [[nodiscard]] bool operator==(const SourceCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator!=(const SourceCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator<(const SourceCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator>(const SourceCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator<=(const SourceCodeLocation& o) const noexcept;
    [[nodiscard]] bool operator>=(const SourceCodeLocation& o) const noexcept;

    // Offset as a byte index into the text.
    std::size_t absolute { 0 };
    // Zero indexed, first line is zero.
    std::size_t lineNumber { 0 };
    // Codepoint count, note, this is currently broken and doesn't work with multicodepoint graphemes.
    std::size_t column { 0 };
};


// A range within a source code snippet
struct SourceCodeRange {
    [[nodiscard]] static SourceCodeRange range(SourceCodeRange left, SourceCodeRange right);
    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] std::size_t line_count() const;

    SourceCodeLocation begin, end;
};

// A range within an entire file
struct FileCodeRange {
    [[nodiscard]] static FileCodeRange range(FileCodeRange left, FileCodeRange right);
    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] std::size_t line_count() const;

    FileCodeLocation begin, end;
};

}
