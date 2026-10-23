#pragma once

#include <algorithm>
#include <array>
#include <cassert>

#include "sc_lexer/tokens.hpp"

#include "parser_context.hpp"
#include "sc_grammar_parser.hpp"
#include "sc_parser/sc_grammar_shared.hpp"
#include "sc_lexer/text_location.hpp"

inline sc::ast::parser::parser::token_kind_type to_parser_token(sc::lex::TokenType t) {
    using T = sc::ast::parser::parser::token_kind_type;
    using TokenType = sc::lex::TokenType;
    switch (t) {
    case TokenType::EndOfFile:
        return T::TOKEN_YYEOF;
    case TokenType::Name:
        return T::TOKEN_NAME;
    case TokenType::ClassName:
        return T::TOKEN_CLASSNAME;
    case TokenType::PrimitiveName:
        return T::TOKEN_PRIMITIVENAME;
    case TokenType::Integer:
        return T::TOKEN_INTEGER;
    case TokenType::IntegerRadix:
        return T::TOKEN_INTEGER_RADIX;
    case TokenType::Hexadecimal:
        return T::TOKEN_HEXADECIMAL;
    case TokenType::Float:
        return T::TOKEN_FLOAT;
    case TokenType::FloatRadix:
        return T::TOKEN_FLOAT_RADIX;
    case TokenType::FloatExponent:
        return T::TOKEN_FLOAT_EXPONENT;
    case TokenType::Pi:
        return T::TOKEN_PI;
    case TokenType::Inf:
        return T::TOKEN_FLOAT_INF;
    case TokenType::AccidentalSteps:
        return T::TOKEN_ACCIDENTAL_STEPS;
    case TokenType::AccidentalCents:
        return T::TOKEN_ACCIDENTAL_CENTS;
    case TokenType::SymbolSlash:
        return T::TOKEN_SYMBOL_SLASH;
    case TokenType::SymbolQuote:
        return T::TOKEN_SYMBOL_QUOTE;
    case TokenType::Ascii:
        return T::TOKEN_ASCII;
    case TokenType::True:
        return T::TOKEN_TRUE;
    case TokenType::False:
        return T::TOKEN_FALSE;
    case TokenType::Nil:
        return T::TOKEN_NIL;
    case TokenType::StringLine:
        return T::TOKEN_STRINGLINE;
    case TokenType::While:
        return T::TOKEN_WHILE; // There is no need to do this at this stage, inline it in the compiler.
    case TokenType::Var:
        return T::TOKEN_VAR;
    case TokenType::Arg:
        return T::TOKEN_ARG;
    case TokenType::ClassVar:
        return T::TOKEN_CLASSVAR;
    case TokenType::Const:
        return T::TOKEN_CONST;
    case TokenType::OpenParen:
        return T::TOKEN_OPENPAREN;
    case TokenType::OpenSquare:
        return T::TOKEN_OPENSQUARE;
    case TokenType::OpenCurly:
        return T::TOKEN_OPENCURLY;
    case TokenType::BeginClosedFunction:
        return T::TOKEN_BEGINCLOSEDFUNC;
    case TokenType::CloseParen:
        return T::TOKEN_CLOSEPAREN;
    case TokenType::CloseSquare:
        return T::TOKEN_CLOSESQUARE;
    case TokenType::CloseCurly:
        return T::TOKEN_CLOSECURLY;
    case TokenType::SemiColon:
        return T::TOKEN_SEMICOLON;
    case TokenType::Colon:
        return T::TOKEN_COLON;
    case TokenType::Comma:
        return T::TOKEN_COMMA;
    case TokenType::EqualsSign:
        return T::TOKEN_EQUALSSIGN;
    case TokenType::NonLocalReturn:
        return T::TOKEN_NONLOCALRETURN;
    case TokenType::BackTick:
        return T::TOKEN_BACKTICK;
    case TokenType::Tilde:
        return T::TOKEN_TILDE;
    case TokenType::Hash:
        return T::TOKEN_HASH;
    case TokenType::LeftArrow:
        return T::TOKEN_LEFTARROW;
    case TokenType::Ellipsis:
        return T::TOKEN_ELLIPSIS;
    case TokenType::Dot:
        return T::TOKEN_DOT;
    case TokenType::DotDot:
        return T::TOKEN_DOTDOT;
    case TokenType::CurryArg:
        return T::TOKEN_CURRYARG;
    case TokenType::Pipe:
        return T::TOKEN_PIPE;
    case TokenType::ReadWriteVar:
        return T::TOKEN_READWRITEVAR;
    case TokenType::Minus:
        return T::TOKEN_MINUS;
    case TokenType::Multiply:
        return T::TOKEN_MULTIPLY;
    case TokenType::Add:
        return T::TOKEN_ADD;
    case TokenType::LessThan:
        return T::TOKEN_LESSTHAN;
    case TokenType::GreaterThan:
        return T::TOKEN_GREATERTHAN;
    case TokenType::BinaryOperator:
        return T::TOKEN_BINOP;
    case TokenType::KeywordBinaryOperator:
        return T::TOKEN_KEYBINOP;
    case TokenType::Space:
        return T::TOKEN_YYUNDEF;
    case TokenType::NewLine:
        return T::TOKEN_YYUNDEF;
    case TokenType::Tab:
        return T::TOKEN_YYUNDEF;
    case TokenType::Comment:
        return T::TOKEN_YYUNDEF;
    case TokenType::MultilineComment:
        return T::TOKEN_YYUNDEF;
    default:
        return T::TOKEN_YYUNDEF;
    }
}

namespace sc::ast::parser::error_recovery {

// Reports the error the parser recorded, if it hasn't been, see report_syntax_error.
void report(ParserContext&);
// Reports a missing semicolon at the end of last_valid, see yylex.
void region_separator(ParserContext&, sc ::lex::SourceCodeRange last_valid);
void expr(ParserContext&);
// Reports the error and skips past the curly bracket closing the method, matching open_bracket, returning where it is.
sc::lex::SourceCodeRange method(ParserContext&, sc::lex::SourceCodeRange open_bracket);
// Reports the error and skips past the bracket matching open_bracket, returning where it is.
sc::lex::SourceCodeRange bracket(ParserContext&, sc::lex::SourceCodeRange open_bracket);
}

// Whether the parser can take token next. It uses lookahead correction, so is exact.
inline bool parser_accepts(const sc::ast::parser::parser& p, sc::ast::parser::parser::token_kind_type token) {
    using P = sc::ast::parser::parser;
    const auto kind = P::symbol_type { token, sc::ast::LexerToken {}, {} }.kind();
    std::array<P::symbol_kind_type, P::symbol_kind::YYNTOKENS> expected {};
    const auto count = P::context { p, P::symbol_type { P::token::TOKEN_YYEOF, {} } }.expected_tokens(
        expected.data(), static_cast<int>(expected.size()));
    return std::find(expected.begin(), expected.begin() + count, kind) != expected.begin() + count;
}

inline int yylex(sc::ast::parser::parser::value_type* v, sc::lex::SourceCodeRange* loc,
                 sc::ast::parser::ParserContext& cxt) {
    using T = sc::ast::parser::parser::token_kind_type;
    v->emplace<sc::ast::LexerToken>();

    if (cxt.mode == sc::ast::parser::ParserContext::Mode::CommandInitial) {
        cxt.mode = sc::ast::parser::ParserContext::Mode::CommandContinue;
        *loc = {};
        return T::TOKEN_INTERPRET;
    }

    const auto& current = cxt.tokens[cxt.next];
    *loc = current.range;

    // The lexer has reported it, see tokenise.
    if (sc::lex::is_error(current.token)) {
        cxt.lookahead = cxt.next++;
        return T::TOKEN_LEXER_ERROR;
    }

    const auto token = to_parser_token(current.token);

    // Region recovery. Since scd files aren't valid sclang code, this tries to guess where the semicolons should go.
    const auto* before = cxt.next > 0 ? &cxt.tokens[cxt.next - 1] : nullptr;
    const bool starts_line = before && before->range.end.line_number < current.range.begin.line_number;
    if (cxt.mode == sc::ast::parser::ParserContext::Mode::CommandContinue && starts_line && !cxt.separator_inserted
        && current.token != sc::lex::TokenType::EndOfFile && !parser_accepts(*cxt.owner, token)
        && parser_accepts(*cxt.owner, T::TOKEN_REGION_SEPARATOR)) {
        sc::ast::parser::error_recovery::region_separator(cxt, before->range);
        cxt.separator_inserted = true;
        *loc = { before->range.end, before->range.end };
        return T::TOKEN_REGION_SEPARATOR;
    }
    cxt.separator_inserted = false;

    // The end of file is emitted again if asked, e.g., by error recovery.
    cxt.lookahead = cxt.next;
    if (current.token != sc::lex::TokenType::EndOfFile)
        ++cxt.next;
    return static_cast<int>(token);
}
