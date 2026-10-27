#pragma once

#include <assert.h>
#include <cstdint>
#include <algorithm>
#include <array>
#include <cstddef>
#include <tuple>

#include "sc_source/text_location.hpp"
#include "sc_source/codepoint.hpp"
#include "sc_source/normalise_source.hpp"

namespace sc::lex {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Peek - used to look ahead into the codepoint stream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <std::size_t N> struct Peek {
    template <std::size_t M> [[nodiscard]] constexpr Peek<M> shrink_to() const noexcept {
        static_assert(M <= N);
        std::array<src::CodePoint, M> out;
        std::copy(characters.data(), characters.data() + characters.size(), out.data());
        return { out };
    }

    [[nodiscard]] constexpr src::CodePoint operator[](std::size_t i) const { return characters[i]; }

    template <std::size_t I> [[nodiscard]] constexpr src::CodePoint at() const noexcept {
        static_assert(I < N);
        return characters[I];
    }

    // Whether the peeked characters start with the string, e.g., `peek.starts_with("abc")`.
    template <std::size_t M> [[nodiscard]] constexpr bool starts_with(const char (&str)[M]) const noexcept {
        static_assert(M - 1 <= N, "string is longer than the peek");
        for (std::size_t i { 0 }; i < M - 1; ++i)
            if (characters[i] != static_cast<unsigned char>(str[i]))
                return false;
        return true;
    }

    std::array<src::CodePoint, N> characters;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CodePointStream - Walks through source code, returns codepoints. If we have reached the end of the text, returns a
// null terminator, even if there isn't one in the original text.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CodePointStream {
    // Mutable, gets updated as we advance through the code.
    struct State {
        std::uint32_t next_byte_offest { 0 };
        std::uint32_t current_line_number { 0 };
        // Byte offset in line, not visual column.
        std::uint32_t current_column_count { 0 };
        bool prev_was_newline { false };
        void update(src::CodePoint next, std::uint8_t sz) noexcept {
            if (prev_was_newline) {
                current_line_number += 1;
                current_column_count = 0;
                prev_was_newline = false;
            } else {
                current_column_count += 1;
            }
            prev_was_newline = src::is_newline(next);
            next_byte_offest += sz;
        }
    } state {};

public:
    // NOTE: NormalisedSource src must outlive this object.

    // Used to stream through a whole piece of text from beginning to end.
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

    // Used to stream through a snippet from a larger source.
    CodePointStream(const src::NormalisedSource& src, src::FileCodeLocation src_start_in_file,
                    src::SourceCodeRange snippet_in_src) noexcept:
        state({ snippet_in_src.begin.absolute, snippet_in_src.begin.line_number, snippet_in_src.begin.column }),
        source_start_in_file(src_start_in_file),
        source(static_cast<const std::string&>(src).c_str()),
        snippet(source + snippet_in_src.begin.absolute),
        snippet_len(snippet_in_src.size()) {}

    // Used to stream through a snippet from a larger source, from a starting point until the end of the file.
    CodePointStream(const src::NormalisedSource& src, src::FileCodeLocation src_start_in_file,
                    src::SourceCodeLocation snippet_start_in_src) noexcept:
        state({ snippet_start_in_src.absolute, snippet_start_in_src.line_number, snippet_start_in_src.column }),
        source_start_in_file(src_start_in_file),
        source(static_cast<const std::string&>(src).c_str()),
        snippet(source + snippet_start_in_src.absolute),
        snippet_len(static_cast<const std::string&>(src).size() - snippet_start_in_src.absolute) {}

    CodePointStream(CodePointStream&&) noexcept = default;
    CodePointStream(const CodePointStream&) = default;
    CodePointStream& operator=(CodePointStream&&) noexcept = default;
    CodePointStream& operator=(const CodePointStream&) = default;

    // Allows us to get the location in the file that the source came from.
    src::FileCodeLocation source_start_in_file;

    // Text, could be code snippet, or whole file, may not be null terminated.
    // This is used to resolve SourceCodeRanges
    const char* source;

    // This is the text we are actually moving through, it is a part of source
    const char* snippet;
    std::size_t snippet_len;


    // SourceCodeRange manipulation
    [[nodiscard]] src::FileCodeRange source_to_file(const src::SourceCodeRange& source) const;
    [[nodiscard]] src::FileCodeLocation source_to_file(const src::SourceCodeLocation& source) const;

    // returns begin and size
    [[nodiscard]] std::tuple<const char*, std::size_t>
    source_code_range_to_text(const src::SourceCodeRange& range) const;

    // Advances the stream, returns start location and codepoint.
    [[nodiscard]] std::tuple<src::SourceCodeLocation, src::CodePoint> start_token() {
        if (state.prev_was_newline) {
            state.current_line_number += 1;
            state.current_column_count = 0;
            state.prev_was_newline = false;
        }
        return { { state.next_byte_offest, state.current_line_number, state.current_column_count }, advance() };
    }

    // Does not advance the stream, returns the end of the token
    [[nodiscard]] src::SourceCodeLocation end_token() const {
        return { state.next_byte_offest, state.current_line_number, state.current_column_count };
    }


    // Peeking returns the next N CodePoints, but does not advance the state.
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

    // The lexer peeks and advances for every character, so these are inline, see utf8_sequence_to_codepoint.
    [[nodiscard]] src::CodePoint peek() const {
        if (source + state.next_byte_offest >= snippet + snippet_len)
            return 0;
        return std::get<0>(src::utf8_sequence_to_codepoint(
            source + state.next_byte_offest,
            static_cast<std::size_t>((snippet + snippet_len) - (source + state.next_byte_offest))));
    }

    // Returns next character, advancing the stream.
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

    // Advance followed by peek
    [[nodiscard]] src::CodePoint advance_and_peek() {
        advance();
        return peek();
    }

    // Commits peek to the current token, advancing it.
    template <std::size_t N> src::SourceCodeLocation advance_by_peek(Peek<N> peek) {
        for (std::size_t i { 0 }; i < N; ++i)
            state.update(peek[i], src::codepoint_size(peek[i]));
        return end_token();
    }

    // Peeks, if equal to one of c, then advances.
    // Usage: if (stream.peek_advance_if('.')) { ... consumed '.' ... };
    // Usage: if (stream.peek_advance_if('+', '-')) { ... consumed '-' or '+' ... };
    template <typename... C> bool peek_advance_if(C... cs) {
        if (const auto p = peek(); ((p == cs) || ...)) {
            advance();
            return true;
        }
        return false;
    }

    // Like peek_advance_if, but must match all in order rather than just one.
    // Advances stream by sizeof...(C)
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


    // Null terminator is NEVER accepted as a predicate.
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
