#pragma once

#include <assert.h>
#include <cstdint>
#include <algorithm>
#include <array>
#include <cstddef>
#include <tuple>

#include "sc_source/text_location.hpp"
#include "sc_source/details/codepoint.hpp"
#include "sc_source/normalise_source.hpp"

namespace sc::lex {

// The next N code points of a stream, used to look ahead. Padded with null terminators past the end.
template <std::size_t N> struct Peek {
    [[nodiscard]] constexpr src::CodePoint operator[](std::size_t i) const { return characters[i]; }

    template <std::size_t I> [[nodiscard]] constexpr src::CodePoint at() const noexcept {
        static_assert(I < N);
        return characters[I];
    }

    template <std::size_t M> [[nodiscard]] constexpr bool starts_with(const char (&str)[M]) const noexcept {
        static_assert(M - 1 <= N, "string is longer than the peek");
        for (std::size_t i { 0 }; i < M - 1; ++i)
            if (characters[i] != static_cast<unsigned char>(str[i]))
                return false;
        return true;
    }

    std::array<src::CodePoint, N> characters;
};

// Walks through source code one code point at a time, tracking the line and column.
//
// At the end of the text it returns a null terminator, even if there isn't one in the original text.
// It keeps a pointer to the NormalisedSource, which must outlive the stream. Temporaries are rejected.
class CodePointStream {
    // Mutable, gets updated as we advance through the code.
    struct State {
        std::uint32_t next_byte_offest { 0 };
        std::uint32_t current_line_number { 0 };
        // Code points since the start of the line, not bytes and not the visual column.
        std::uint32_t current_codepoint_in_line { 0 };
        bool prev_was_newline { false };
        void update(src::CodePoint next, std::uint8_t sz) noexcept {
            if (prev_was_newline) {
                current_line_number += 1;
                current_codepoint_in_line = 0;
                prev_was_newline = false;
            } else {
                current_codepoint_in_line += 1;
            }
            prev_was_newline = src::is_newline(next);
            next_byte_offest += sz;
        }
    } state {};

public:
    CodePointStream(const src::NormalisedSource& src, src::FileCodeLocation src_start_in_file) noexcept:
        source_start_in_file(src_start_in_file),
        source(static_cast<const std::string&>(src).c_str()),
        snippet(source),
        snippet_len(static_cast<const std::string&>(src).size()) {}

    CodePointStream(const src::NormalisedSource& src) noexcept:
        source_start_in_file(),
        source(static_cast<const std::string&>(src).c_str()),
        snippet(source),
        snippet_len(static_cast<const std::string&>(src).size()) {}

    CodePointStream(const src::NormalisedSource& src, src::FileCodeLocation src_start_in_file,
                    src::SourceCodeRange snippet_in_src) noexcept:
        state({ snippet_in_src.begin.absolute, snippet_in_src.begin.line_number,
                snippet_in_src.begin.codepoint_in_line }),
        source_start_in_file(src_start_in_file),
        source(static_cast<const std::string&>(src).c_str()),
        snippet(source + snippet_in_src.begin.absolute),
        snippet_len(snippet_in_src.size()) {}

    CodePointStream(const src::NormalisedSource& src, src::FileCodeLocation src_start_in_file,
                    src::SourceCodeLocation snippet_start_in_src) noexcept:
        state({ snippet_start_in_src.absolute, snippet_start_in_src.line_number,
                snippet_start_in_src.codepoint_in_line }),
        source_start_in_file(src_start_in_file),
        source(static_cast<const std::string&>(src).c_str()),
        snippet(source + snippet_start_in_src.absolute),
        snippet_len(static_cast<const std::string&>(src).size() - snippet_start_in_src.absolute) {}

    // Only a pointer into the source is kept, so a temporary would dangle.
    CodePointStream(const src::NormalisedSource&&, src::FileCodeLocation) = delete;
    CodePointStream(const src::NormalisedSource&&) = delete;
    CodePointStream(const src::NormalisedSource&&, src::FileCodeLocation, src::SourceCodeRange) = delete;
    CodePointStream(const src::NormalisedSource&&, src::FileCodeLocation, src::SourceCodeLocation) = delete;

    CodePointStream(CodePointStream&&) noexcept = default;
    CodePointStream(const CodePointStream&) = default;
    CodePointStream& operator=(CodePointStream&&) noexcept = default;
    CodePointStream& operator=(const CodePointStream&) = default;

    // Where the source begins in its file, so locations in the source can be mapped to the file.
    src::FileCodeLocation source_start_in_file;

    // The whole text, which may not be null terminated. SourceCodeRanges are offsets into this.
    const char* source;

    // The part of source the stream moves through, and its length in bytes.
    const char* snippet;
    std::size_t snippet_len;


    [[nodiscard]] src::FileCodeRange source_to_file(const src::SourceCodeRange& source) const;
    [[nodiscard]] src::FileCodeLocation source_to_file(const src::SourceCodeLocation& source) const;

    [[nodiscard]] std::tuple<const char*, std::size_t>
    source_code_range_to_text(const src::SourceCodeRange& range) const;

    // Begins a token: advances the stream by one code point.
    [[nodiscard]] std::tuple<src::SourceCodeLocation, src::CodePoint> start_token() {
        if (state.prev_was_newline) {
            state.current_line_number += 1;
            state.current_codepoint_in_line = 0;
            state.prev_was_newline = false;
        }
        return { { state.next_byte_offest, state.current_line_number, state.current_codepoint_in_line }, advance() };
    }

    // The current location, which is the end of the token so far. Does not advance the stream.
    [[nodiscard]] src::SourceCodeLocation end_token() const {
        return { state.next_byte_offest, state.current_line_number, state.current_codepoint_in_line };
    }


    template <std::size_t N> [[nodiscard]] Peek<N> peek_n() const {
        static_assert(N > 0);
        std::array<src::CodePoint, N> out;

        // Copy state, don't update the main stream's state.
        State rolling_state { state };
        const auto get_next = [&]() {
            const auto remaining =
                std::max<ptrdiff_t>(0, (snippet + snippet_len) - (source + rolling_state.next_byte_offest));
            const auto [cp, cp_size] = src::utf8_sequence_to_codepoint(source + rolling_state.next_byte_offest,
                                                                       static_cast<std::size_t>(remaining));
            rolling_state.update(cp, cp_size);
            return cp;
        };

        for (std::size_t char_count { 0 }; char_count < N; ++char_count)
            out[char_count] = (source + rolling_state.next_byte_offest >= snippet + snippet_len) ? 0 : get_next();

        return { out };
    }

    [[nodiscard]] src::CodePoint peek() const {
        if (source + state.next_byte_offest >= snippet + snippet_len)
            return 0;
        return std::get<0>(src::utf8_sequence_to_codepoint(
            source + state.next_byte_offest,
            static_cast<std::size_t>((snippet + snippet_len) - (source + state.next_byte_offest))));
    }

    src::CodePoint advance() {
        // outside snippet
        if (source + state.next_byte_offest >= snippet + snippet_len)
            return 0;

        const auto remaining = std::max<ptrdiff_t>(0, (snippet + snippet_len) - (source + state.next_byte_offest));

        const auto [c, sz] =
            src::utf8_sequence_to_codepoint(source + state.next_byte_offest, static_cast<std::size_t>(remaining));

        state.update(c, sz);

        return c;
    }

    [[nodiscard]] src::CodePoint advance_and_peek() {
        advance();
        return peek();
    }

    template <typename... C> bool peek_advance_if(C... cs) {
        if (const auto p = peek(); ((p == cs) || ...)) {
            advance();
            return true;
        }
        return false;
    }

    template <typename... C> bool peek_advance_if_sequence(C... cs) {
        const auto size { sizeof...(C) };
        const auto peek { peek_n<size>() };

        std::size_t i { 0 };
        const auto valid = ((peek[i++] == cs) && ...);
        if (!valid)
            return false;

        // Cast to void to indicate we are discarding the value.
        ((advance(), void(cs)), ...);
        return true;
    }


    // Advances while the predicate holds. The null terminator is never accepted.
    template <typename Predicate> std::size_t advance_while_count(Predicate&& predicate) {
        auto discard_null_then_predicate = [&](auto c) {
            return (c == 0 || source + state.next_byte_offest >= snippet + snippet_len) ? false : predicate(c);
        };
        std::size_t i { 0 };
        for (auto c = peek(); discard_null_then_predicate(c); c = advance_and_peek(), ++i) {}
        return i;
    }

    template <typename Predicate> src::SourceCodeLocation advance_while(Predicate&& predicate) {
        advance_while_count(std::forward<Predicate>(predicate));
        return end_token();
    }
};


}
