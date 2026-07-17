#pragma once

#include "codepoint.hpp"
#include "text_location.hpp"
#include <optional>

namespace sc::lex::utils {

// Iterate through utf8 text.
// Unlike CodePointStream, this is very simple and lightweight, only providing forwards and backwards methods.
// It does not return the source code location.
class CodePointIterator {
public:
    [[nodiscard]] static std::optional<CodePointIterator> make(const char* txt_start, const char* txt_end,
                                                               const char* current_location = nullptr);

    CodePointIterator(CodePointIterator&&) noexcept = default;
    CodePointIterator(const CodePointIterator&) noexcept = default;

    CodePointIterator() = delete;
    CodePointIterator& operator=(CodePointIterator&&) = delete;
    CodePointIterator& operator=(const CodePointIterator&) = delete;

    std::optional<CodePoint> forwards() noexcept;
    std::optional<CodePoint> backwards() noexcept;

    void jump(std::int64_t m) noexcept { txt_iter += m; }
    void jump_forward(CodePoint cp) noexcept { txt_iter += codepoint_size(cp); }
    void jump_backward(CodePoint cp) noexcept { txt_iter -= codepoint_size(cp); }

    [[nodiscard]] auto current_codepoint() const noexcept {
        return utf8_sequence_to_codepoint(txt_iter, static_cast<std::size_t>(txt_end - txt_iter));
    }
    [[nodiscard]] const char* current_location() const noexcept { return txt_iter; }

private:
    CodePointIterator(const char* text_start, const char* text_end, const char* cur_location) noexcept;

    const char* const txt_start;
    const char* const txt_end;

    const char* txt_iter;
};

// Breaks text into lines.
class LineIter {
public:
    // Will rewind current_location to the start of the line (or txt_start).
    [[nodiscard]] static std::optional<LineIter> make(const char* txt_start, const char* txt_end,
                                                      SourceCodeLocation loc = {});

    LineIter(LineIter&&) noexcept = default;
    LineIter(const LineIter&) noexcept = default;

    LineIter() = delete;
    LineIter& operator=(LineIter&&) = delete;
    LineIter& operator=(const LineIter&) = delete;

    struct Result {
        const char* ptr;
        std::size_t sz;
        std::size_t line_number;
        bool ends_in_newline_char;
    };

    std::optional<Result> forwards() noexcept;
    std::optional<Result> backwards() noexcept;

private:
    LineIter(CodePointIterator it, std::size_t line_number) noexcept;

    CodePointIterator cp_iter;
    std::size_t current_line;
    enum Dir { None, Backward, Forwards } previous_dir { None };
};

}
