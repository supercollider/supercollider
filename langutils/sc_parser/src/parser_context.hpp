// Copyright Jordan Henderson 2026
#pragma once

#include "sc_lexer/codepoint_stream.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/sc_grammar_shared.hpp"

#include "sc_lexer/normalise_source.hpp"
#include "sc_lexer/text_info.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_lexer/tokens.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <utility>
#include <optional>
#include <vector>

namespace sc::ast::parser {


// Stores the token type, source code range.
// Secondarily, it store an index where the matching parenthesis is (if the token is a parenthesis).
// Additionally, the tokeniser is going to insert brackets when it thinks they are missing.
// This is stored as a single bit (inserted bit) which is set at the top of the match index (together stored in 'bits').
struct Token {
    static constexpr std::uint32_t inserted_bit = std::uint32_t { 1 } << 31;
    static constexpr std::uint32_t no_match = inserted_bit - 1;

    // The most tokens there can be, as the index of a match must fit below the inserted bit.
    // Since we have a file size limit of 2^32, this seems unlikely to ever be hit.
    static constexpr std::uint32_t max_tokens = no_match;

    Token(sc::lex::TokenType t, sc::lex::SourceCodeRange r): token(t), range(r) {}

    sc::lex::TokenType token;
    sc::lex::SourceCodeRange range;

    // The index of the bracket matching this one, every bracket has one, see tokenise.
    [[nodiscard]] std::optional<std::uint32_t> match() const {
        const auto m = bits & ~inserted_bit;
        return m == no_match ? std::nullopt : std::optional { m };
    }
    void set_match(std::uint32_t i) { bits = (bits & inserted_bit) | i; }

    // Inserted to close an unclosed bracket, so isn't in the source and has an empty range at the token before it.
    [[nodiscard]] bool inserted() const { return bits & inserted_bit; }
    void set_inserted() { bits |= inserted_bit; }

private:
    // The match's index, or no_match, with the inserted bit at the top.
    std::uint32_t bits { no_match };
};
static_assert(sizeof(Token) == 32);

// A `//# VERSION 3.16` comment, which the parser doesn't see, see ParserContext::take_version.
struct VersionDirective {
    sc::lex::SourceCodeRange range;
    // Nothing if it is malformed or unknown, which has been reported.
    std::optional<LanguageVersion> version;
    // The token before it, nothing at the start of the file.
    std::optional<sc::lex::SourceCodeRange> after;
    bool used { false };
};

struct ParserContext;

// Lexes all of the stream into the context's tokens and version directives, reporting the lexer's errors, then pairs
// the brackets. Attempt to repair any mismatched brackets.
void tokenise(ParserContext& cxt, sc::lex::CodePointStream cps);

struct UnexpectedToken {
    sc::lex::SourceCodeRange location;
    // These are type erased. To figure out what they are, you need the parser.
    // This is implemented in the file sc_grammar_impl.
    std::vector<int> expected;
    int got;
    // At a bracket inserted to close an unclosed one, which has been reported, see tokenise.
    bool at_inserted { false };
    // The token written before it, as a type erased parser symbol, for messages when we want to suggest somethign
    // between them.
    struct Before {
        int kind;
        std::uint32_t index;
    };
    std::optional<Before> before {};
};
class parser;

struct ParserContext {
    enum struct Mode { ClassLibrary, CommandInitial, CommandContinue };
    std::shared_ptr<const sc::lex::TextInfo> text_info;
    // Used to emit the specical interpret token.
    Mode mode;

    std::shared_ptr<sc::ast::ASTGraph> graph = std::make_shared<ASTGraph>(text_info);

    std::vector<Token> tokens {};

    // In source order, see tokenise.
    std::vector<VersionDirective> directives {};

    // The version of classes without their own, from the directives at the start of the file, see claim_file_version.
    std::optional<LanguageVersion> file_version {};

    // Takes the directives at the start of a class file as its version. In interpreted code, which is given its version
    // when compiled, they are all reported as ignored.
    void claim_file_version();

    // The version of the class or extension whose body opens with open_curly: the directive directly after it, else the
    // file's, else Legacy.
    [[nodiscard]] LanguageVersion take_version(sc::lex::SourceCodeRange open_curly);

    // Those not taken don't apply to anything.
    void report_misplaced_versions();

    // The next token to emit, see yylex.
    std::uint32_t next { 0 };

    // The token last emitted, which an error is at.
    std::optional<std::uint32_t> lookahead {};

    // A region separator was inserted before the next token
    bool separator_inserted { false };

    std::optional<UnexpectedToken> unexpected_token_error { };

    // Empties it, so it is reported once, see error_recovery::report.
    std::optional<UnexpectedToken> consume_error() { return std::exchange(unexpected_token_error, std::nullopt); }


    // The parser this is the context of, so the lexer can ask what it can accept, see yylex.
    // Must be initalised afterwards because the parser owns the context.
    // A little hacky, but useful.
    const parser* owner { nullptr };


    template <class... ARGS> [[nodiscard]] auto create(ARGS&&... args) {
        return graph->create(std::forward<ARGS>(args)...);
    }

    // When an error is created, it isn't assigned a parent, leaving it in the orphan list.
    // Here we use the error location to gather all nodes within the range.
    // Assumes that there is a non-empty rule before it.
    // The error rule's own value can still end inside it, e.g., a region that is only an error at the same token, so
    // the rule passes it as kept.
    [[nodiscard]] std::vector<Index> discarded(sc::lex::SourceCodeRange error, std::optional<Index> kept = {}) const {
        std::vector<Index> nodes;
        for (const auto i : graph->orphans()) {
            const auto& l = graph->location(i);
            // Error rules must have a non-empty symbol before the error, e.g., a token, otherwise an empty value left
            // on the stack, e.g., an empty list, could touch the error and be adopted.
            const bool inside = error.begin <= l.begin && l.end <= error.end;
            // Only the head of a sibling chain, as appending it takes the rest of the chain.
            if (inside && i != kept && !graph->edges(i).prev_sibling)
                nodes.push_back(i);
        }
        std::sort(nodes.begin(), nodes.end(),
                  [&](Index a, Index b) { return graph->location(a).begin < graph->location(b).begin; });
        return nodes;
    }

    // An error node that adopts what error recovery discarded, so it isn't left as orphans, see discarded.
    ErrorIndex create_error(sc::lex::SourceCodeRange location, sc::lex::SourceCodeRange error,
                            std::optional<Index> kept = {}) {
        const auto nodes = discarded(error, kept);
        const auto e = graph->create(Error {}, location);
        for (const auto i : nodes)
            graph->append(e, i);
        return e;
    }
};

}
