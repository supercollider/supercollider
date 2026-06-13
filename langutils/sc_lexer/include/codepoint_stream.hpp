#pragma once

#include <assert.h>
#include <cstdint>
#include <array>

#include <text_location.hpp>
#include <codepoint.hpp>

namespace sc::lex {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Peek - used to look ahead into the codepoint stream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <std::size_t N> struct Peek {
    template <std::size_t M> [[nodiscard]] constexpr Peek<M> shrink_to() const noexcept {
        static_assert(M <= N);
        std::array<CodePoint, M> out;
        std::copy(characters.data(), characters.data() + characters.size(), out.data());
        return { out };
    }

    [[nodiscard]] constexpr CodePoint operator[](std::size_t i) const { return characters[i]; }

    template <std::size_t I> [[nodiscard]] constexpr CodePoint at() const noexcept {
        static_assert(I < N);
        return characters[I];
    }

    std::array<CodePoint, N> characters;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CodePointStream - Walks through source code, returns codepoints. If we have reached the end of the text, returns a
// null terminator, even if there isn't one in the original text.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CodePointStream {
    // Mutable, gets updated as we advance through the code.
    struct State {
        std::size_t next_byte_offest { 0 };
        std::size_t current_line_number { 0 };
        // Doesn't work with tabs, fix this once the main parser no longer needs this.
        std::size_t current_column_count { 0 };
        bool prev_was_newline { false };
        void update(CodePoint next, std::uint8_t sz) noexcept;
    } state {};

public:
    CodePointStream(const char* src, std::size_t src_len, FileCodeLocation src_start_in_file);
    CodePointStream() = delete;
    CodePointStream(CodePointStream&&) noexcept = default;
    CodePointStream(const CodePointStream&) = default;
    CodePointStream& operator=(CodePointStream&&) noexcept = delete;
    CodePointStream& operator=(const CodePointStream&) = delete;

    // Allows us to get the location in the file that the source came from.
    const FileCodeLocation source_start_in_file;
    // Text, could be code snippet, or whole file, may not be null terminated.
    const char* const source;
    const std::size_t source_length;

    // SourceCodeRange manipulation
    [[nodiscard]] FileCodeRange source_to_file(const SourceCodeRange& source) const;
    [[nodiscard]] FileCodeLocation source_to_file(const SourceCodeLocation& source) const;

    // returns begin and size
    [[nodiscard]] std::tuple<const char*, std::size_t> source_code_range_to_text(const SourceCodeRange& range) const;

    // Advances the stream, returns start location and codepoint.
    [[nodiscard]] std::tuple<SourceCodeLocation, CodePoint> start_token();

    // Does not advance the stream, returns the end of the token
    [[nodiscard]] SourceCodeLocation end_token() const;


    // Peeking returns the next N CodePoints, but does not advance the state.
    template <std::size_t N> [[nodiscard]] Peek<N> peek_n() const {
        static_assert(N > 0);
        std::array<CodePoint, N> out;

        // Copy state, don't update the main stream's state.
        State rolling_state { state };
        const auto get_next = [&]() {
            const auto [cp, cp_size] = utf8_sequence_to_codepoint(source + rolling_state.next_byte_offest,
                                                                  source_length - rolling_state.next_byte_offest);
            rolling_state.update(cp, cp_size);
            return cp;
        };

        for (std::size_t char_count { 0 }; char_count < N; ++char_count)
            out[char_count] = (rolling_state.next_byte_offest >= source_length) ? 0 : get_next();

        return { out };
    }

    [[nodiscard]] CodePoint peek() const;


    // Returns next character, advancing the stream.
    CodePoint advance();

    // Advance followed by peek
    [[nodiscard]] CodePoint advance_and_peek();

    // Commits peek to the current token, advancing it.
    template <std::size_t N> SourceCodeLocation advance_by_peek(Peek<N> peek) {
        for (std::size_t i { 0 }; i < N; ++i)
            state.update(peek[i], codepoint_size(peek[i]));
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
            return (c == 0 || state.next_byte_offest >= source_length) ? false : predicate(c);
        };
        std::size_t i { 0 };
        for (auto c = peek(); discard_null_then_predicate(c); c = advance_and_peek(), ++i) {}
        return i;
    }

    template <typename Predicate> SourceCodeLocation advance_while(Predicate&& predicate) {
        advance_while_count(std::forward<Predicate>(predicate));
        return end_token();
    }

    void reset();
};


}
