// Copyright Jordan Henderson 2026
#pragma once

#include "sc_lexer/details/codepoint_stream.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/tokens.hpp"
#include "sc_util/language_version.hpp"

#include "sc_source/text_info.hpp"
#include "sc_source/text_location.hpp"
#include "sc_util/indexed_vector.hpp"

#include <algorithm>
#include <memory>
#include <utility>
#include <optional>
#include <vector>

namespace sc::ast::parser {

// A `//# VERSION 3.16` comment, which the parser doesn't see, see ParserContext::take_version.
struct VersionDirective {
    sc::src::SourceCodeRange range;
    // Nothing if it is malformed or unknown, which has been reported.
    std::optional<LanguageVersion> version;
    // The token before it, nothing at the start of the file.
    std::optional<sc::src::SourceCodeRange> after;
    bool used { false };
};

struct ParserContext;

// Lexes all of the stream into the context's tokens and version directives, reporting the lexer's errors, then pairs
// the brackets. Attempt to repair any mismatched brackets. Comments are only collected when kept.
void tokenise(ParserContext& cxt, sc::lex::CodePointStream cps, KeepTokens keep);

struct UnexpectedToken {
    sc::src::SourceCodeRange location;
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
        TokenIndex index;
    };
    std::optional<Before> before {};
    // Where to report it, when that isn't where it was found, e.g., the end of the junk before a method, see
    // error_recovery::class_item.
    std::optional<sc::src::SourceCodeRange> report_at {};
};
class parser;

struct ParserContext {
    enum struct Mode { ClassLibrary, CommandInitial, CommandContinue, FunctionInitial, FunctionContinue };
    std::shared_ptr<const sc::src::TextInfo> text_info;
    // Used to emit the specical interpret token.
    Mode mode;

    std::shared_ptr<sc::ast::ASTGraph> graph = std::make_shared<ASTGraph>(text_info);

    sc::util::IndexedVector<TokenIndex, Token> tokens {};

    // Only filled when the tokens are kept, see tokenise.
    sc::util::IndexedVector<CommentIndex, Comment> comments {};

    // Closing brackets that close nothing, which the parser doesn't see, see tokenise.
    std::vector<sc::src::SourceCodeRange> unmatched_brackets {};

    // In source order, see tokenise.
    std::vector<VersionDirective> directives {};

    // The version of classes without their own, from the directive on the file's first line, see claim_file_version.
    std::optional<LanguageVersion> file_version {};

    // Takes a directive on the first line of the file as its version, in class files and interpreted code alike, and
    // gives it to the graph. Directives anywhere else are misplaced, other than a class's own, see take_version.
    void claim_file_version();

    // The version of the class or extension whose body opens with open_curly: the directive directly after it, else the
    // file's, else Legacy.
    [[nodiscard]] LanguageVersion take_version(sc::src::SourceCodeRange open_curly);

    // Those not taken don't apply to anything.
    void report_misplaced_versions();

    // The next token to emit, see yylex.
    TokenIndex next { 0 };

    // The token last emitted, which an error is at.
    std::optional<TokenIndex> lookahead {};

    // The token that class body or statement recovery last stopped at, so recovering again at it skips it, see
    // error_recovery.
    std::optional<TokenIndex> recovered_at {};

    // A region separator was inserted before the next token
    bool separator_inserted { false };

    std::optional<UnexpectedToken> unexpected_token_error {};

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
    [[nodiscard]] std::vector<Index> discarded(sc::src::SourceCodeRange error, std::optional<Index> kept = {}) const {
        std::vector<Index> nodes;
        for (const auto i : graph->orphans()) {
            const auto& l = graph->location(i);
            // Not an empty node: one left on the stack, e.g., an empty list or parameter list before a body's first
            // statement, touches the error and would be adopted by the error its own rule then holds, a cycle. An
            // empty node holds nothing skipped, so stays an orphan.
            const bool inside = l.begin < l.end && error.begin <= l.begin && l.end <= error.end;
            // Only the head of a sibling chain, as appending it takes the rest of the chain.
            if (inside && i != kept && !graph->edges(i).prev_sibling)
                nodes.push_back(i);
        }
        std::sort(nodes.begin(), nodes.end(),
                  [&](Index a, Index b) { return graph->location(a).begin < graph->location(b).begin; });
        return nodes;
    }

    // An error node that adopts what error recovery discarded, so it isn't left as orphans, see discarded.
    ErrorIndex create_error(sc::src::SourceCodeRange location, sc::src::SourceCodeRange error,
                            std::optional<Index> kept = {}) {
        const auto nodes = discarded(error, kept);
        const auto e = graph->create(Error {}, location);
        for (const auto i : nodes)
            graph->append(e, i);
        return e;
    }
};

}
