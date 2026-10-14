// Copyright Jordan Henderson 2026
#pragma once

#include "sc_lexer/codepoint_stream.hpp"
#include "sc_parser/ast.hpp"

#include "sc_lexer/normalise_source.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_lexer/tokens.hpp"

#include <memory>
#include <utility>
#include <optional>
#include <vector>

namespace sc::ast::parser {


using UnderlyingTokenType = std::underlying_type_t<sc::lex::TokenType>;

enum struct ExtendedTokenType : std::underlying_type_t<sc::lex::TokenType> {
    ExtraClosingParenBracket = static_cast<UnderlyingTokenType>(sc::lex::TokenType::START_OF_USER_DEFINED_ERRORS),
    ExtraClosingSquareBracket,
    ExtraClosingCurlyBracket,

    GotParenExpectedSquare,
    GotParenExpectedCurly,

    GotCurlyExpectedParen,
    GotCurlyExpectedSquare,

    GotSquareExpectedParen,
    GotSquareExpectedCurly,
};

struct Action {
private:
    using TokenType = sc::lex::TokenType;
    using SourceCodeRange = sc::lex::SourceCodeRange;
    using NormalisedSource = sc::lex::NormalisedSource;

public:
    // Returned by sc::lex::lexer(...);
    struct Output {
        Output(ExtendedTokenType t, SourceCodeRange r, std::optional<SourceCodeRange> e = std::nullopt) noexcept:
            token(static_cast<TokenType>(t)),
            range(r),
            extra(e) {}
        Output(TokenType t, SourceCodeRange r, std::optional<SourceCodeRange> e = std::nullopt) noexcept:
            token(t),
            range(r),
            extra(e) {}

        TokenType token;
        SourceCodeRange range;
        std::optional<SourceCodeRange> extra { };
    };


    template <TokenType T> [[nodiscard]] std::optional<Output> process(SourceCodeRange loc) {
        if constexpr (sc::lex::is_whitespace(T) || sc::lex::is_comment(T))
            return std::nullopt;
        else if constexpr (sc::lex::is_open_bracket(T)) {
            closing_bracket_stack.push_back({ get_closing_bracket<T>(), loc });
            return { { T, loc } };
        } else if constexpr (sc::lex::is_close_bracket(T)) {
            if (closing_bracket_stack.empty()) {
                if constexpr (T == TokenType::CloseParen)
                    return { { ExtendedTokenType::ExtraClosingParenBracket, loc } };
                else if constexpr (T == TokenType::CloseSquare)
                    return { { ExtendedTokenType::ExtraClosingSquareBracket, loc } };
                else if constexpr (T == TokenType::CloseCurly)
                    return { { ExtendedTokenType::ExtraClosingCurlyBracket, loc } };
                else {
                    return { { TokenType::ErUnknown, loc } };
                }
            } else {
                if (const auto expected = closing_bracket_stack.back(); expected.first == T) {
                    closing_bracket_stack.pop_back();
                    return { { T, loc, { expected.second } } };
                } else if (expected.first == TokenType::CloseParen) {
                    if (T == TokenType::CloseSquare)
                        return { { ExtendedTokenType::GotSquareExpectedParen, loc,
                                   closing_bracket_stack.back().second } };
                    if (T == TokenType::CloseCurly)
                        return { { ExtendedTokenType::GotCurlyExpectedParen, loc,
                                   closing_bracket_stack.back().second } };
                } else if (expected.first == TokenType::CloseSquare) {
                    if (T == TokenType::CloseParen)
                        return { { ExtendedTokenType::GotParenExpectedSquare, loc,
                                   closing_bracket_stack.back().second } };
                    if (T == TokenType::CloseCurly)
                        return { { ExtendedTokenType::GotCurlyExpectedSquare, loc,
                                   closing_bracket_stack.back().second } };
                } else if (expected.first == TokenType::CloseCurly) {
                    if (T == TokenType::CloseParen)
                        return { { ExtendedTokenType::GotParenExpectedCurly, loc,
                                   closing_bracket_stack.back().second } };
                    if (T == TokenType::CloseSquare)
                        return { { ExtendedTokenType::GotSquareExpectedCurly, loc,
                                   closing_bracket_stack.back().second } };
                } else {
                    // This only happens if someone adds a new type of bracket and doesn't update the checks above.
                    return { { TokenType::ErUnknown, loc } };
                }
            }
        }

        return { { T, loc } };
    }

private:
    std::vector<std::pair<TokenType, SourceCodeRange>> closing_bracket_stack { };

    template <TokenType T> constexpr TokenType get_closing_bracket() const {
        static_assert(sc::lex::matches(T, TokenType::OpenParen, TokenType::OpenSquare, TokenType::OpenCurly,
                                       TokenType::BeginClosedFunction));
        if constexpr (T == TokenType::OpenParen)
            return TokenType::CloseParen;
        else if constexpr (T == TokenType::OpenSquare)
            return TokenType::CloseSquare;
        else
            return TokenType::CloseCurly;
    }
};

struct UnexpectedToken {
    sc::lex::SourceCodeRange location;
    // These are type erased. To figure out what they are, you need the parser.
    // This is implemented in the file sc_grammar_impl.
    std::vector<int> expected;
    int got;
};
struct ParserContext {
    enum struct Mode { ClassLibrary, CommandInitial, CommandContinue };
    std::shared_ptr<const sc::lex::TextInfo> text_info;
    sc::lex::CodePointStream cps;
    Action action;
    Mode mode;

    std::optional<Action::Output> previous { std::nullopt };
    sc::ast::ASTGraph graph { text_info };

    std::optional<UnexpectedToken> unexpected_token_error { };

    std::optional<UnexpectedToken> consume_error() { return std::move(unexpected_token_error); }


    enum struct RegionRecovery {
        EmitRegionSeparator,
        EmitPrevious,
        None,
    } region_recovery { ParserContext::RegionRecovery::None };


    template <class... ARGS> [[nodiscard]] auto create(ARGS&&... args) {
        return graph.create(std::forward<ARGS>(args)...);
    }
};

}
