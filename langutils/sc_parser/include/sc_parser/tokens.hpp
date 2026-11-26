// Copyright Jordan Henderson 2026
#pragma once

#include "sc_lexer/details/tokens.hpp"
#include "sc_source/text_location.hpp"
#include "sc_util/indexed_vector.hpp"
#include "sc_util/strong_index.hpp"

#include <cstdint>
#include <optional>
#include <vector>

namespace sc::ast {

struct TokenTag;
using TokenIndex = sc::util::StrongIndex<std::uint32_t, TokenTag>;

/// @brief A token the parser reads, with the matching bracket if it is one.
///
/// The tokeniser inserts a closing bracket where one is missing. It is marked as inserted, with an empty range at the
/// end of the token before it. The match and the inserted mark share one 32 bit field.
struct Token {
    static constexpr std::uint32_t inserted_bit = std::uint32_t { 1 } << 31;
    static constexpr std::uint32_t no_match = inserted_bit - 1;

    // The most tokens there can be, as the index of a match must fit below the inserted bit.
    // Since we have a file size limit of 2^32, this seems unlikely to ever be hit.
    static constexpr std::uint32_t max_tokens = no_match;

    Token(sc::lex::TokenType t, sc::src::SourceCodeRange r): token(t), range(r) {}

    sc::lex::TokenType token;
    sc::src::SourceCodeRange range;

    // When brackets, matching return the other one in a pair.
    [[nodiscard]] std::optional<TokenIndex> matching() const {
        const auto m = bits & ~inserted_bit;
        return m == no_match ? std::nullopt : std::optional { TokenIndex { m } };
    }

    // Sets the other one in a pair.
    void set_matching_index(TokenIndex i) { bits = (bits & inserted_bit) | *i; }

    // Inserted to close an unclosed bracket, so isn't in the source and has an empty range at the token before it.
    [[nodiscard]] bool inserted() const { return bits & inserted_bit; }
    void set_inserted() { bits |= inserted_bit; }

private:
    // The match's index, or no_match, with the inserted bit at the top.
    std::uint32_t bits { no_match };
};
static_assert(sizeof(Token) == 32);

struct CommentTag;
using CommentIndex = sc::util::StrongIndex<std::uint32_t, CommentTag>;

/// @brief A comment, which the parser skips: `//`, `/* */`, or `//# VERSION`.
struct Comment {
    sc::lex::TokenType kind;
    sc::src::SourceCodeRange range;
};
static_assert(sizeof(Comment) == 28);

/// @brief The tokens and comments of a parse, kept when parse() is given KeepTokens::Yes, e.g., for a formatter.
///
/// Together they are every token the lexer finds but whitespace, which is the text between them. Each list is in
/// source order, so a node's tokens can be found from its range by binary search.
struct SourceTokens {
    /// The tokens the parser reads, ending with `EndOfFile`. Inserted brackets aren't in the text, see
    /// Token::inserted. A closing bracket of the wrong kind has the kind that closes its opener.
    sc::util::IndexedVector<TokenIndex, Token> tokens;
    sc::util::IndexedVector<CommentIndex, Comment> comments;
    /// Closing brackets that close nothing. They are reported, and the parser doesn't read them.
    std::vector<sc::src::SourceCodeRange> unmatched_brackets;
};

/// @brief Whether parse() keeps the tokens and comments on the graph, see ASTGraph::source_tokens.
enum struct KeepTokens : bool { No, Yes };

}
