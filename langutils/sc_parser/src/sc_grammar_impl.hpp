#pragma once

#include "sc_lexer/lexer.hpp"

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
        return T::TOKEN_ACCIDENTAL_STEPS;
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

inline int yylex(sc::ast::parser::parser::value_type* v, sc::lex::SourceCodeRange* loc, sc::ast::parser::ParserContext& cxt) {
    using T = sc::ast::parser::parser::token_kind_type;

    // Ugly hack for region detection recovery.
    if (cxt.region_recovery == sc::ast::parser::ParserContext::RegionRecovery::EmitRegionSeparator) {
        assert(cxt.previous);
        cxt.region_recovery = sc::ast::parser::ParserContext::RegionRecovery::EmitPrevious;
        return T::TOKEN_REGION_SEPARATOR;
    }

    else if (cxt.region_recovery == sc::ast::parser::ParserContext::RegionRecovery::EmitPrevious) {
        assert(cxt.previous);
        cxt.region_recovery = sc::ast::parser::ParserContext::RegionRecovery::None;

        const auto [lex_token, location, extra_location] = *cxt.previous;
        *loc = location;
        v->emplace<sc::ast::LexerToken>();
        return static_cast<int>(to_parser_token(lex_token));
    }

    if (cxt.mode == sc::ast::parser::ParserContext::Mode::CommandInitial) {
        cxt.mode = sc::ast::parser::ParserContext::Mode::CommandContinue;
        v->emplace<sc::ast::LexerToken>();
        *loc = {};
        return T::TOKEN_INTERPRET;
    }

    // Normal path
    cxt.previous = sc::lex::lexer(cxt.cps, cxt.action);
    const auto [lex_token, location, extra_location] = *cxt.previous;
    *loc = location;
    v->emplace<sc::ast::LexerToken>();

    if (sc::lex::is_error(lex_token)) {
        // cxt.error_handler->operator()(cxt.text_info, lex_token, location, extra_location);
        return T::TOKEN_LEXER_ERROR;
    }

    return static_cast<int>(to_parser_token(lex_token));
}

namespace sc::ast::parser::error_recovery {

// Reports the error the parser recorded, if it hasn't been, see report_syntax_error.
void report(ParserContext&);
// Reports a missing semicolon instead of the error.
void region_separator(ParserContext&, sc ::lex::SourceCodeRange last_valid);
void expr(ParserContext&);
void method(ParserContext&, sc::lex::SourceCodeRange open_bracket);
// Reports the error and skips to the bracket matching open_bracket, or the end of the file, returning where it stopped.
sc::lex::SourceCodeRange bracket(ParserContext&, sc::lex::SourceCodeRange open_bracket);
}
