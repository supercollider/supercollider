#pragma once

#include "codepoint.hpp"
#include "text_location.hpp"
#include "codepoint_stream.hpp"
#include "tokens.hpp"
#include <optional>

namespace sc::lex {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main lexer function
// Takes in state and mutates it.
// Looks to the action to deduce its return type.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename Action> auto lexer(CodePointStream& stream, Action& action) noexcept -> typename Action::Output;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Some example actions.
// Must define an Output type.
// Must define a process method that returns a std::optional<Output> accepting a TokenType as a template arg and a
// SourceCodeRange as the location.
// Return std::nullopt to discard the token, this is common to skip whitespace or comments.
// Use if constexpr to switch on the token type, not template specialisation (as this isn't valid C++).
// NOTE: it is INVALID to return std::nullopt when receiveing TokenType::EndOfFile.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace actions {
struct TypeAndLocationAction {
    struct Output {
        TokenType type;
        SourceCodeRange range;
    };
    template <TokenType type> std::optional<Output> process(SourceCodeRange loc) { return { { type, loc } }; }
};

struct TypeOnlyAction {
    using Output = TokenType;
    template <TokenType type> std::optional<Output> process(SourceCodeRange) { return { type }; }
};

}


namespace details {

constexpr inline int str_to_int_for_radix(const char* str, std::size_t n, int base) {
    // TODO: in future it would be nice to remove this from the lexer, it means changing the language to accept
    // invalid radixs at the lexing stage, but makes things context independant.
    int out { 0 };
    for (std::size_t i { 0 }; i < n; ++i) {
        const char c = *str++;
        if (c >= '0' && c <= '0' + std::min(10, base) - 1)
            out = out * base + c - '0';
        else if (c >= 'a' && c <= 'a' + std::min(36, base) - 11)
            out = out * base + c - 'a' + 10;
        else if (c >= 'A' && c <= 'A' + std::min(36, base) - 11)
            out = out * base + c - 'A' + 10;
    }
    return out;
}


template <typename Action>
decltype(auto) lexer_binary_operator(CodePointStream& stream, Action& action, SourceCodeLocation token_start) {
    const auto end = stream.advance_while(is_binary_operator_character);
    const auto [str_b, sz] = stream.source_code_range_to_text({ token_start, end });
    assert(sz > 0);
    if (sz == 1) {
        const auto c = str_b[0];
        switch (c) {
        case '<':
            return action.template process<TokenType::LessThan>({ token_start, end });
        case '>':
            return action.template process<TokenType::GreaterThan>({ token_start, end });
        case '.':
            return action.template process<TokenType::Dot>({ token_start, end });
        case '-':
            return action.template process<TokenType::Minus>({ token_start, end });
        case '*':
            return action.template process<TokenType::Multiply>({ token_start, end });
        case '+':
            return action.template process<TokenType::Add>({ token_start, end });
        case '|':
            return action.template process<TokenType::Pipe>({ token_start, end });
        case '=':
            return action.template process<TokenType::EqualsSign>({ token_start, end });
        }
    } else if (sz == 2) {
        const auto c1 = str_b[0];
        const auto c2 = str_b[1];
        if (c1 == '<' && c2 == '-')
            return action.template process<TokenType::LeftArrow>({ token_start, end });
        if (c1 == '<' && c2 == '>')
            return action.template process<TokenType::ReadWriteVar>({ token_start, end });
    }
    return action.template process<TokenType::BinaryOperator>({ token_start, end });
}
struct StringMatcher {
    const char* start;
    template <std::size_t N> bool match(const char array[N]) const {
        for (std::size_t i { 0 }; i < N; ++i) {
            if (start[i] != array[i])
                return false;
        }
        return true;
    }
};

template <typename Action>
decltype(auto) lexer_identifier_keybinop_curry_kw_etc(CodePointStream& stream, Action& action,
                                                      SourceCodeLocation token_start) {
    const auto end = stream.advance_while([](auto c) { return is_continuing_identifier(c); });

    // Note: this logic is a little odd, as it mean '_:' and '_asdf:' are keybinops, as is 'Foo:'.
    // This explains why you need a space between child and parent class in class definitions.
    // Also means keywords can be keybinops, 'var:', 'pi:'.
    // The only place these can be used is in Event: `( _: {|self, other| other } ) _: 1`. This makes no sense for
    // '_', but 'true:' and 'pi:' could be used somwhere.
    if (stream.peek_advance_if(':'))
        return action.template process<TokenType::KeywordBinaryOperator>({ token_start, stream.end_token() });

    const SourceCodeRange range { token_start, end };
    const auto [t_b, sz] = stream.source_code_range_to_text(range);

    if (t_b[0] == '_' && range.size() == 1)
        return action.template process<TokenType::CurryArg>(range);

    if (t_b[0] == '_')
        return action.template process<TokenType::PrimitiveName>(range);

    if (is_start_of_class(*t_b))
        return action.template process<TokenType::ClassName>(range);

    const auto txt = StringMatcher { t_b };
    if (sz == 2) {
        if (txt.match<2>("pi"))
            return action.template process<TokenType::Pi>(range);
    } else if (sz == 3) {
        if (txt.match<3>("var"))
            return action.template process<TokenType::Var>(range);
        else if (txt.match<3>("arg"))
            return action.template process<TokenType::Arg>(range);
        else if (txt.match<3>("nil"))
            return action.template process<TokenType::Nil>(range);
        else if (txt.match<3>("inf"))
            return action.template process<TokenType::Float>(range);
    } else if (sz == 4) {
        if (txt.match<4>("true"))
            return action.template process<TokenType::True>(range);
    } else if (sz == 5) {
        if (txt.match<5>("const"))
            return action.template process<TokenType::Const>(range);
        else if (txt.match<5>("while"))
            return action.template process<TokenType::While>(range);
        else if (txt.match<5>("false"))
            return action.template process<TokenType::False>(range);
    } else if (sz == 8) {
        if (txt.match<8>("classvar"))
            return action.template process<TokenType::ClassVar>(range);
    }
    return action.template process<TokenType::Name>(range);
}

template <typename Action>
decltype(auto) lexer_digits(CodePointStream& stream, Action& action, SourceCodeLocation token_start) {
    const auto end_of_pre = stream.advance_while([](auto c) { return is_numeric(c); });

    const auto peek = stream.peek_n<2>();

    switch (peek[0]) {
    case 'r': {
        const auto [radix_str_b, sz] = stream.source_code_range_to_text({ token_start, end_of_pre });
        stream.advance(); // drop 'r'
        const int radix = str_to_int_for_radix(radix_str_b, sz, 10);
        const auto offset10 = std::max<int>(0, std::min<int>(10, radix)) - 1;
        const auto offset36 = std::max<int>(0, std::min<int>(36, radix)) - 11;

        // TODO: I'd like to change this part of the language.
        // We are only accepting valid radixs, that means what happens after can become another token..
        //
        //     |**| ---- integer '2345'
        // 2r012345
        // |^^|------------- intradix '2r01'
        //
        //      |***| ---- keybinop def: 'cdef:'
        // 12rabcdef:
        // |^^^|------------- intradix '12rab'
        stream.advance_while([=](auto c) {
            return ('0' <= c && c <= '0' + offset10) || ('a' <= c && c <= 'a' + offset36)
                || ('A' <= c && c <= 'A' + offset36);
        });
        if (stream.peek_advance_if('.')) {
            stream.advance_while(
                [=](auto c) { return ('0' <= c && c <= '0' + offset10) || ('A' <= c && c <= 'A' + offset36); });
            return action.template process<TokenType::FloatRadix>({ token_start, stream.end_token() });
        } else {
            return action.template process<TokenType::IntegerRadix>({ token_start, stream.end_token() });
        }
    }

    case 'e':
        [[fallthrough]];
    case 'E': {
        stream.advance(); // drop e
        stream.peek_advance_if('+', '-');
        const auto count = stream.advance_while_count([](auto c) { return is_numeric(c); });
        if (count == 0)
            return action.template process<TokenType::ErMissingExponent>({ token_start, stream.end_token() });

        return action.template process<TokenType::FloatExponent>({ token_start, stream.end_token() });
    }

    case '.': {
        if (!is_numeric(peek[1]))
            return action.template process<TokenType::Integer>({ token_start, stream.end_token() });
        stream.advance(); // drop '.'
        stream.advance_while(is_numeric);
        if (const auto e = stream.peek(); e == 'e' || e == 'E') {
            stream.advance(); // drop e
            stream.peek_advance_if('+', '-');
            const auto count = stream.advance_while_count([](auto c) { return is_numeric(c); });
            if (count == 0)
                return action.template process<TokenType::ErMissingExponent>({ token_start, stream.end_token() });

            return action.template process<TokenType::FloatExponent>({ token_start, stream.end_token() });
        }

        return action.template process<TokenType::Float>({ token_start, stream.end_token() });
    }

    case 'b':
        [[fallthrough]];
    case 's': {
        const auto num_acc = stream.advance_while_count([acc = peek[0]](auto c) { return c == acc; });
        assert(num_acc > 0);
        if (num_acc == 1) {
            // Lets you specify a cent value after a *single* accidental.
            // 123s40
            const auto num_cent_chars = stream.advance_while_count(is_numeric);
            if (num_cent_chars == 0)
                return action.template process<TokenType::AccidentalSteps>({ token_start, stream.end_token() });
            else
                return action.template process<TokenType::AccidentalCents>({ token_start, stream.end_token() });
        }
        return action.template process<TokenType::AccidentalSteps>({ token_start, stream.end_token() });
    }

    case 'x': {
        stream.advance();
        // Ignores the preceding digits and the 'x'.
        // BUG: this means 89702347890234589xAA == 0xAA. Probably not intended.
        const auto end = stream.advance_while(
            [](auto c) { return is_numeric(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); });
        return action.template process<TokenType::Hexidecimal>({ token_start, end });
    }

    default:
        return action.template process<TokenType::Integer>({ token_start, stream.end_token() });
    }
}

}

template <typename Action> auto lexer(CodePointStream& stream, Action& action) noexcept -> typename Action::Output {
    using namespace details;

// A simple loop, I've used a label rather than a while-continue so it is clear exactly where we are continuing to.
next_token : {
    // A commonly used pattern. If the optional is valid, return it unwrapped, else discard it and continue the loop.
    // Other progamming languages have this as a keyword, e.g. 'orelse' in zig `return (call) orelse continue.
#define return_orelse_next_token(call)                                                                                 \
    do {                                                                                                               \
        if (auto r = (call)) {                                                                                         \
            return *r;                                                                                                 \
        } else {                                                                                                       \
            goto next_token;                                                                                           \
        }                                                                                                              \
    } while (false)


    // Grab next token and location, advancing the stream.
    const auto [token_start, code_point] = stream.start_token();

    if (code_point == 0) {
        // It is invalid for an action to return an empty optional upon receiving the end of the file token.
        // It is impossible to exit this function if we don't stop at the end of the file.
        const auto r = action.template process<TokenType::EndOfFile>({ token_start, stream.end_token() });
        assert(r);
        return *r;
    }

    if (is_newline(code_point)) {
        stream.advance_while(is_newline);
        return_orelse_next_token(action.template process<TokenType::NewLine>({ token_start, stream.end_token() }));
    }


    if (code_point == '\t') {
        stream.advance_while(is_tab);
        return_orelse_next_token(action.template process<TokenType::Tab>({ token_start, stream.end_token() }));
    }

    if (is_space(code_point)) {
        stream.advance_while(is_space);
        return_orelse_next_token(action.template process<TokenType::Space>({ token_start, stream.end_token() }));
    }

    // ASCII tokens. Their ASCII value is their token type value.
    switch (code_point) {
    case '^':
        return_orelse_next_token(
            action.template process<TokenType::NonLocalReturn>({ token_start, stream.end_token() }));
    case '~':
        return_orelse_next_token(action.template process<TokenType::Tilde>({ token_start, stream.end_token() }));
    case ';':
        return_orelse_next_token(action.template process<TokenType::SemiColon>({ token_start, stream.end_token() }));
    case ':':
        return_orelse_next_token(action.template process<TokenType::Colon>({ token_start, stream.end_token() }));
    case '`':
        return_orelse_next_token(action.template process<TokenType::BackTick>({ token_start, stream.end_token() }));
    case ',':
        return_orelse_next_token(action.template process<TokenType::Comma>({ token_start, stream.end_token() }));
    case '(':
        return_orelse_next_token(action.template process<TokenType::OpenParen>({ token_start, stream.end_token() }));
    case '[':
        return_orelse_next_token(action.template process<TokenType::OpenSquare>({ token_start, stream.end_token() }));
    case '{':
        return_orelse_next_token(action.template process<TokenType::OpenCurly>({ token_start, stream.end_token() }));
    case ')':
        return_orelse_next_token(action.template process<TokenType::CloseParen>({ token_start, stream.end_token() }));
    case ']':
        return_orelse_next_token(action.template process<TokenType::CloseSquare>({ token_start, stream.end_token() }));
    case '}':
        return_orelse_next_token(action.template process<TokenType::CloseCurly>({ token_start, stream.end_token() }));
    }

    if (code_point == '#') {
        if (stream.peek_advance_if('{')) {
            return_orelse_next_token(
                action.template process<TokenType::BeginClosedFunction>({ token_start, stream.end_token() }));
        } else {
            return_orelse_next_token(action.template process<TokenType::Hash>({ token_start, stream.end_token() }));
        }
    }


    // Comments or binary op
    if (code_point == '/') {
        const auto p = stream.peek();

        // '/*'
        if (p == '*') {
            stream.advance();
            CodePoint it_1 = 0, it = 0;
            size_t level = 1;
            stream.advance_while([&](auto c) {
                it_1 = it;
                it = c;
                if (it_1 == '/' && it == '*') {
                    level += 1;
                    it = 0; // consume
                    return true;
                }
                if (it_1 == '*' && it == '/') {
                    level -= 1;
                    it = 0; // consume
                    return level != 0;
                }
                return true;
            });
            const auto delimit = stream.advance();
            if (delimit == 0) {
                return_orelse_next_token(action.template process<TokenType::ErMultilineCommentUnclosed>(
                    { token_start, stream.end_token() }));
            }
            return_orelse_next_token(
                action.template process<TokenType::MultilineComment>({ token_start, stream.end_token() }));
        }

        // '//'
        else if (p == '/') {
            stream.advance_while([](auto c) { return !is_newline(c); });
            return_orelse_next_token(action.template process<TokenType::Comment>({ token_start, stream.end_token() }));
        }

        // Its a binary operator.
        return_orelse_next_token(lexer_binary_operator(stream, action, token_start));
    }

    if (code_point == '.') {
        if (stream.peek_advance_if_sequence('.', '.'))
            return_orelse_next_token(action.template process<TokenType::Ellipsis>({ token_start, stream.end_token() }));
        else if (stream.peek_advance_if('.'))
            return_orelse_next_token(action.template process<TokenType::DotDot>({ token_start, stream.end_token() }));
        else
            return_orelse_next_token(action.template process<TokenType::Dot>({ token_start, stream.end_token() }));
    }

    if (code_point == '$') {
        if (stream.peek() == '\\') {
            stream.advance(); // consume '\'
            stream.advance(); // consume whatever happens after it.
            return_orelse_next_token(action.template process<TokenType::Ascii>({ token_start, stream.end_token() }));
        }

        stream.advance(); // consume whatever character comes after the '$'

        return_orelse_next_token(action.template process<TokenType::Ascii>({ token_start, stream.end_token() }));
    }

    if (is_starting_identifier(code_point) || code_point == '_')
        return_orelse_next_token(lexer_identifier_keybinop_curry_kw_etc(stream, action, token_start));


    if (is_binary_operator_character(code_point))
        return_orelse_next_token(lexer_binary_operator(stream, action, token_start));


    if (is_numeric(code_point))
        return_orelse_next_token(lexer_digits(stream, action, token_start));

    // Strings
    if (code_point == '"') {
        const auto end = stream.advance_while([escaped = false](auto c) mutable {
            if (c == '\\' && !escaped) {
                escaped = true;
                return true;
            }
            if (c == '"' && !escaped)
                return false;
            escaped = false;
            return true;
        });

        if (stream.advance() != '"')
            return_orelse_next_token(action.template process<TokenType::ErStringUnclosed>({ token_start, end }));

        return_orelse_next_token(action.template process<TokenType::StringLine>({ token_start, stream.end_token() }));
    }

    // Symbol that begin with a '\'. Note: first character is used to alter what is acceptable in the following.
    if (code_point == '\\') {
        const auto first = stream.peek();
        const SourceCodeLocation end = [&]() {
            if (is_lower(first) || is_upper(first) || first == '_')
                // Symbol began with alpha or underscore, therefore, can contain alpha, underscore, and numbers
                return stream.advance_while(
                    [](auto c) { return is_lower(c) || is_upper(c) || is_numeric(c) || c == '_'; });
            if (is_numeric(first))
                // Symbol began with a digit, therefore can only contain digits... why??
                return stream.advance_while(is_numeric);

            // It isn't either of the above, emit an empty symbol.
            return stream.end_token();
        }();
        return_orelse_next_token(action.template process<TokenType::SymbolSlash>({ token_start, end }));
    }

    // Symbol quotes 'asdf'
    if (code_point == '\'') {
        const auto content_end = stream.advance_while([escape = false](auto c) mutable {
            if (is_newline(c) && !escape) // you can escape the new line characters
                return false;
            if (c == '\'' && !escape)
                return false;
            if (c == '\\' && !escape) {
                escape = true;
                return true;
            }
            escape = false;
            return true;
        });
        if (stream.peek() != '\'')
            return_orelse_next_token(
                action.template process<TokenType::ErSymbolQuoteUnclosed>({ token_start, content_end }));

        stream.advance();
        return_orelse_next_token(action.template process<TokenType::SymbolQuote>({ token_start, stream.end_token() }));
    }

    // These are the control codes, throw them away!
    // We might consider generating an error here, as this really ought not to occur.
    if (is_control_code(code_point)) {
        stream.advance_while(is_control_code);
        goto next_token;
    }

    if (code_point == sc::lex::invalid_utf8_flag)
        return_orelse_next_token(
            action.template process<TokenType::ErInvalidUTF8>({ token_start, stream.end_token() }));

    return_orelse_next_token(action.template process<TokenType::ErUnexpected>({ token_start, stream.end_token() }));
}

#undef return_orelse_next_token
}


}
