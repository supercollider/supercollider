#pragma once

#include <assert.h>
#include <cstdint>
#include <array>

#include <text_location.hpp>
#include <codepoint.hpp>
#include <normalise_source.hpp>

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
        // Byte offset in line, not visual column.
        std::size_t current_column_count { 0 };
        bool prev_was_newline { false };
        void update(CodePoint next, std::uint8_t sz) noexcept;
    } state {};

public:
    // NOTE: NormalisedSource src must outlive this object.

    // Used to stream through a whole piece of text from beginning to end.
    CodePointStream(const NormalisedSource& src, FileCodeLocation src_start_in_file) noexcept:
        source_start_in_file(src_start_in_file),
        source(static_cast<const std::string&>(src).c_str()),
        snippet(source),
        snippet_len(static_cast<const std::string&>(src).size()) {}

    CodePointStream(const NormalisedSource& src) noexcept:
        source_start_in_file(),
        source(static_cast<const std::string&>(src).c_str()),
        snippet(source),
        snippet_len(static_cast<const std::string&>(src).size()) {}

    // Used to stream through a snippet from a larger source.
    CodePointStream(const NormalisedSource& src, FileCodeLocation src_start_in_file,
                    SourceCodeRange snippet_in_src) noexcept:
        state({ snippet_in_src.begin.absolute, snippet_in_src.begin.line_number, snippet_in_src.begin.column }),
        source_start_in_file(src_start_in_file),
        source(static_cast<const std::string&>(src).c_str()),
        snippet(source + snippet_in_src.begin.absolute),
        snippet_len(snippet_in_src.size()) {}

    // Used to stream through a snippet from a larger source, from a starting point until the end of the file.
    CodePointStream(const NormalisedSource& src, FileCodeLocation src_start_in_file,
                    SourceCodeLocation snippet_start_in_src) noexcept:
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
    FileCodeLocation source_start_in_file;

    // Text, could be code snippet, or whole file, may not be null terminated.
    // This is used to resolve SourceCodeRanges
    const char* source;

    // This is the text we are actually moving through, it is a part of source
    const char* snippet;
    std::size_t snippet_len;


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
            const auto remaining = std::max<ptrdiff_t>(0, (snippet + snippet_len) - (source + state.next_byte_offest));
            const auto [cp, cp_size] = utf8_sequence_to_codepoint(source + rolling_state.next_byte_offest,
                                                                  static_cast<std::size_t>(remaining));
            rolling_state.update(cp, cp_size);
            return cp;
        };

        for (std::size_t char_count { 0 }; char_count < N; ++char_count)
            out[char_count] = (source + rolling_state.next_byte_offest >= snippet + snippet_len) ? 0 : get_next();

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
            return (c == 0 || source + state.next_byte_offest >= snippet + snippet_len) ? false : predicate(c);
        };
        std::size_t i { 0 };
        for (auto c = peek(); discard_null_then_predicate(c); c = advance_and_peek(), ++i) {}
        return i;
    }

    template <typename Predicate> SourceCodeLocation advance_while(Predicate&& predicate) {
        advance_while_count(std::forward<Predicate>(predicate));
        return end_token();
    }
};


}
