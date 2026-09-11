/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com
    Copyright (c) 2017 Brian Heim (boost::filesystem additions)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <mutex>
#include "AllocPools.h"
#include "ClassLibraryInfo.hpp"
#include "CompilerContext.hpp"
#include "PyrKernel.h"
#include "PyrMessage.h"
#include "PyrObjectHdr.h"
#include "SCBase.h"
#include "PyrLexer.h"
#include "PyrSlot.h"
#include "PyrSymbol.h"
#include "SC_AllocPool.h"
#include "SC_Constants.h"
#include "SC_LanguageClient.h"
#include "SimpleStack.h"
#include "VMGlobals.h"

#include "BisonHeaderInclude.hpp"
#include "codepoint_stream.hpp"
#include "normalise_source.hpp"
#include "lang11d_tab.h"

#include "text_location.hpp"
#include "tokens.hpp"

#include <algorithm>
#include <exception>
#include <mutex>
#include <optional>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <new>
#include <stdlib.h>
#include <limits>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#ifdef _WIN32
#    include <direct.h>
#else
#    include <sys/param.h>
#endif

#include <filesystem>
#include <fstream>
#include <string.h>

#include "PyrParseNode.h"
#include "SCBase.h"
#include "PyrObject.h"
#include "PyrObjectProto.h"
#include "PyrSched.h"
#include "GC.h"

#include "PyrSymbolTable.h"
#include "PyrInterpreter.h"
#include "PyrObjectProto.h"
#include "PyrPrimitiveProto.h"
#include "PyrKernelProto.h"
#include "PredefinedSymbols.h"
#include "SC_LanguageConfig.hpp"

#include "SC_Filesystem.hpp" // getDirectory, resolveIfAlias, isStandalone
#include "SC_Codecvt.hpp" // path_to_utf8_str

#include <lexer.hpp>

extern ClassLibraryInfo gClassLibraryInfo;

int yyparse();
PyrSlot process_accidental_cents(const char* s);
PyrSlot process_accidental_steps(const char* s);

namespace fs = std::filesystem;
using DirName = SC_Filesystem::DirName;

int sc_strtoi(const char* str, int n, int base) {
    int z = 0;
    for (int i = 0; i < n; ++i) {
        int c = *str++;
        if (!c)
            break;
        if (c >= '0' && c <= '0' + sc_min(10, base) - 1)
            z = z * base + c - '0';
        else if (c >= 'a' && c <= 'a' + sc_min(36, base) - 11)
            z = z * base + c - 'a' + 10;
        else if (c >= 'A' && c <= 'A' + sc_min(36, base) - 11)
            z = z * base + c - 'A' + 10;
    }
    return z;
}

double sc_strtof(const char* str, int n, int base) {
    double z = 0.;
    int decptpos = 0;
    for (int i = 0; i < n; ++i) {
        int c = *str++;
        if (!c)
            break;
        if (c >= '0' && c <= '0' + sc_min(10, base) - 1)
            z = z * base + c - '0';
        else if (c >= 'a' && c <= 'a' + sc_min(36, base) - 11)
            z = z * base + c - 'a' + 10;
        else if (c >= 'A' && c <= 'A' + sc_min(36, base) - 11)
            z = z * base + c - 'A' + 10;
        else if (c == '.')
            decptpos = i;
    }
    // calculation previously included decimal point in count of columns (was n-decptpos); there are 1 less than n
    // characters which are columns in the number contribution
    z = z / pow((double)base, n - 1 - decptpos);
    return z;
}


namespace lex = sc::lex;

using TokenType = lex::TokenType;
using UnderlyingTokenType = std::underlying_type_t<TokenType>;

[[nodiscard]] constexpr std::optional<yytokentype> convert_to_bison_tokentype(TokenType t) {
    if (sc::lex::is_error(t))
        return BADTOKEN;

    switch (t) {
    case TokenType::EndOfFile:
        return YYEOF;
    case TokenType::Name:
        return NAME;
    case TokenType::ClassName:
        return CLASSNAME;
    case TokenType::PrimitiveName:
        return PRIMITIVENAME;
    case TokenType::Integer:
        return INTEGER;
    case TokenType::IntegerRadix:
        return INTEGER;
    case TokenType::Hexidecimal:
        return INTEGER;
    case TokenType::Float:
    case TokenType::FloatRadix:
    case TokenType::FloatExponent:
    case TokenType::Inf:
        return SC_FLOAT;
    case TokenType::Pi:
        return PIE;
    case TokenType::AccidentalSteps:
    case TokenType::AccidentalCents:
        return ACCIDENTAL;
    case TokenType::SymbolSlash:
    case TokenType::SymbolQuote:
        return SYMBOL;
    case TokenType::Ascii:
        return ASCII;
    case TokenType::True:
        return TRUEOBJ;
    case TokenType::False:
        return FALSEOBJ;
    case TokenType::Nil:
        return NILOBJ;
    case TokenType::StringLine:
        return STRING;
    case TokenType::While:
        return WHILE;
    case TokenType::Var:
        return VAR;
    case TokenType::Arg:
        return ARG;
    case TokenType::ClassVar:
        return CLASSVAR;
    case TokenType::Const:
        return SC_CONST;
    case TokenType::OpenParen:
        return OPENPAREN;
    case TokenType::CloseParen:
        return CLOSEPAREN;
    case TokenType::OpenSquare:
        return OPENSQUARE;
    case TokenType::CloseSquare:
        return CLOSESQUARE;
    case TokenType::OpenCurly:
        return OPENCURLY;
    case TokenType::CloseCurly:
        return CLOSECURLY;
    case TokenType::BeginClosedFunction:
        return BEGINCLOSEDFUNC;
    case TokenType::SemiColon:
        return SEMICOLON;
    case TokenType::Colon:
        return COLON;
    case TokenType::Comma:
        return COMMA;
    case TokenType::EqualsSign:
        return EQUALSSIGN;
    case TokenType::NonLocalReturn:
        return NONLOCALRETURN;
    case TokenType::BackTick:
        return BACKTICK;
    case TokenType::Tilde:
        return TILDE;
    case TokenType::Hash:
        return HASH;
    case TokenType::LeftArrow:
        return LEFTARROW;
    case TokenType::ReadWriteVar:
        return READWRITEVAR;
    case TokenType::Ellipsis:
        return ELLIPSIS;
    case TokenType::Dot:
        return DOT;
    case TokenType::DotDot:
        return DOTDOT;
    case TokenType::CurryArg:
        return CURRYARG;
    case TokenType::Pipe:
        return PIPE;
    case TokenType::Minus:
        return MINUS;
    case TokenType::Multiply:
        return MULTIPLY;
    case TokenType::Add:
        return ADD;
    case TokenType::LessThan:
        return LESSTHAN;
    case TokenType::GreaterThan:
        return GREATERTHAN;
    case TokenType::BinaryOperator:
        return BINOP;
    case TokenType::KeywordBinaryOperator:
        return KEYBINOP;
    default:
        assert(false);
        return YYerror;
    }
}

constexpr inline int str_to_int(const char* str, size_t n, int base) {
    int z = 0;
    for (int i = 0; i < n; ++i) {
        int c = *str++;
        if (!c)
            break;
        if (c >= '0' && c <= '0' + std::min(10, base) - 1)
            z = z * base + c - '0';
        else if (c >= 'a' && c <= 'a' + std::min(36, base) - 11)
            z = z * base + c - 'a' + 10;
        else if (c >= 'A' && c <= 'A' + std::min(36, base) - 11)
            z = z * base + c - 'A' + 10;
    }
    return z;
}


enum struct ExtendedErrors : std::underlying_type_t<TokenType> {
    ExtraClosingParenBracket = static_cast<UnderlyingTokenType>(TokenType::START_OF_USER_DEFINED_ERRORS),
    ExtraClosingSqaureBracket,
    ExtraClosingCurlyBracket,

    GotParenExpectedSquare,
    GotParenExpectedCurly,

    GotCurlyExpectedParen,
    GotCurlyExpectedSquare,

    GotSquareExpectedParen,
    GotSquareExpectedCurly,
};

struct BisonSemActionOutput {
    BisonSemActionOutput(ExtendedErrors e, lex::SourceCodeRange range):
        type(static_cast<TokenType>(e)),
        range(range),
        slot({}) {};

    BisonSemActionOutput(ExtendedErrors e, lex::SourceCodeRange range, lex::SourceCodeRange extra_range):
        type(static_cast<TokenType>(e)),
        range(range),
        slot({}),
        extra_range_of_error(extra_range) {};

    BisonSemActionOutput(TokenType t, lex::SourceCodeRange range, std::optional<PyrSlot> slot = {}):
        type(t),
        range(range),
        slot(slot) {};

    BisonSemActionOutput(TokenType t, lex::SourceCodeRange range, PyrSlot slot): type(t), range(range), slot(slot) {};

    BisonSemActionOutput() = default;
    BisonSemActionOutput(BisonSemActionOutput&&) noexcept = default;
    BisonSemActionOutput(const BisonSemActionOutput&) noexcept = default;
    BisonSemActionOutput& operator=(BisonSemActionOutput&&) noexcept = default;
    BisonSemActionOutput& operator=(const BisonSemActionOutput&) noexcept = default;

    [[nodiscard]] bool is_error() const { return sc::lex::is_error(type); }
    [[nodiscard]] bool is(TokenType t) const { return type == t; }
    [[nodiscard]] bool is(ExtendedErrors t) const { return static_cast<int>(type) == static_cast<int>(t); }

    TokenType type {}; // can also include the ExtendedErrors set. There is no nice way to extend an enum in c++.
    lex::SourceCodeRange range {};
    std::optional<PyrSlot> slot {};
    std::optional<lex::SourceCodeRange> extra_range_of_error {};
};
struct BisonLexerAction {
public:
    BisonLexerAction(std::shared_ptr<TextInfo> textInfo): textInfo(std::move(textInfo)) {};
    BisonLexerAction() = delete;
    BisonLexerAction(BisonLexerAction&&) noexcept = default;
    BisonLexerAction(const BisonLexerAction&) = default;
    BisonLexerAction& operator=(BisonLexerAction&&) noexcept = default;
    BisonLexerAction& operator=(const BisonLexerAction&) = default;

    std::shared_ptr<TextInfo> textInfo;
    std::vector<std::pair<TokenType, lex::SourceCodeRange>> closing_bracket_stack {};


    using Output = BisonSemActionOutput;

    template <TokenType T> std::optional<Output> process(lex::SourceCodeRange loc) {
        // Discard
        if constexpr (sc::lex::is_whitespace(T) || sc::lex::is_comment(T))
            return std::nullopt;

        // Convert these directly to a symbol.
        else if constexpr (sc::lex::is_identifier(T) || sc::lex::is_keyword(T) || T == TokenType::BinaryOperator
                           || sc::lex::is_ambiguous_punctuation(T))
            return { { T, loc, PyrSlot::make(text_to_symbol(loc)) } };

        // More complex symbols that drop part of the location and/or use escape characters.
        else if constexpr (T == TokenType::KeywordBinaryOperator)
            return { { T, loc, PyrSlot::make(text_to_symbol(loc, 0, 1)) } };

        else if constexpr (T == TokenType::SymbolSlash)
            return { { T, loc, PyrSlot::make(text_to_symbol(loc, 1, 0)) } };

        else if constexpr (T == TokenType::SymbolQuote)
            return { { T, loc, PyrSlot::make(text_to_symbol(loc, 1, 1, true)) } };

        // Constants
        else if constexpr (sc::lex::matches(T, TokenType::Pi, TokenType::Nil, TokenType::Inf, TokenType::True,
                                            TokenType::False))
            return { { T, loc, to_constant<T>() } };

        // Open brackets
        else if constexpr (sc::lex::is_open_bracket(T)) {
            closing_bracket_stack.push_back({ get_closing_bracket<T>(), loc });
            return { { T, loc } };
        }

        // Closing brackets
        else if constexpr (sc::lex::is_close_bracket(T)) {
            if (closing_bracket_stack.empty()) {
                if constexpr (T == TokenType::CloseParen)
                    return { { ExtendedErrors::ExtraClosingParenBracket, loc } };
                else if constexpr (T == TokenType::CloseSquare)
                    return { { ExtendedErrors::ExtraClosingSqaureBracket, loc } };
                else if constexpr (T == TokenType::CloseCurly)
                    return { { ExtendedErrors::ExtraClosingCurlyBracket, loc } };
                else {
                    // Should not happen, all cases should be dealt with. Return something nice just in case.
                    assert(false);
                    return { { TokenType::ErUnknown, loc } };
                }
            } else {
                const auto expected = closing_bracket_stack.back().first;
                if (expected == T) {
                    // This is pushed even if it isn't a closed function.
                    closing_bracket_stack.pop_back();
                    return { { T, loc } };
                } else if (expected == TokenType::CloseParen) {
                    if (T == TokenType::CloseSquare)
                        return { { ExtendedErrors::GotSquareExpectedParen, loc, closing_bracket_stack.back().second } };
                    if (T == TokenType::CloseCurly)
                        return { { ExtendedErrors::GotCurlyExpectedParen, loc, closing_bracket_stack.back().second } };
                } else if (expected == TokenType::CloseSquare) {
                    if (T == TokenType::CloseParen)
                        return { { ExtendedErrors::GotParenExpectedSquare, loc, closing_bracket_stack.back().second } };
                    if (T == TokenType::CloseCurly)
                        return { { ExtendedErrors::GotCurlyExpectedSquare, loc, closing_bracket_stack.back().second } };
                } else if (expected == TokenType::CloseCurly) {
                    if (T == TokenType::CloseParen)
                        return { { ExtendedErrors::GotParenExpectedCurly, loc, closing_bracket_stack.back().second } };
                    if (T == TokenType::CloseSquare)
                        return { { ExtendedErrors::GotSquareExpectedCurly, loc, closing_bracket_stack.back().second } };
                }
                // Should not happen, all cases should be dealt with. Return something nice just in case.
                assert(false);
                return { { TokenType::ErUnknown, loc } };
            }
        }

        // Floats
        else if constexpr (T == TokenType::Float)
            return { { T, loc, PyrSlot::make(atof(fill_temp_buf(loc))) } };
        else if constexpr (T == TokenType::FloatExponent)
            return { { T, loc, PyrSlot::make(atof(fill_temp_buf(loc))) } };

        // Radix, both int and float
        else if constexpr (T == TokenType::IntegerRadix || T == TokenType::FloatRadix) {
            const auto& str = textInfo->normalisedSource.as_string();
            const char* c_str = str.c_str();
            const char* start = c_str + loc.begin.absolute;
            // Looking for radix.
            const char* it = start;
            while (*it != 'r') // Potentially unsafe, but the lexer guarenteed this was found.
                ++it;
            const int radix = str_to_int(start, it - start, 10);
            ++it; // drop r
            if constexpr (T == TokenType::IntegerRadix) {
                const auto slot_value = sc_strtoi(it, (c_str + loc.end.absolute) - it, radix);
                return { { T, loc, PyrSlot::make(slot_value) } };
            } else {
                const double slot_value = sc_strtof(it, (c_str + loc.end.absolute) - it, radix);
                return { { T, loc, PyrSlot::make(slot_value) } };
            }
        }

        else if constexpr (T == TokenType::Integer)
            return { { T, loc, PyrSlot::make(atoi(fill_temp_buf(loc))) } };

        else if constexpr (T == TokenType::Hexidecimal) {
            const auto& str = textInfo->normalisedSource.as_string();
            const char* c_str = str.c_str();
            const char* c = c_str + loc.begin.absolute;
            const char* const end = c_str + loc.end.absolute;
            // BUG: this is probably a bug, we are ignoring everything before the 'x'
            while (*c != 'x' && *c != 'X' && *c != 0 && c < end)
                ++c;
            ++c;
            int val = 0;
            while (c < end) {
                if (*c >= '0' && *c <= '9')
                    val = val * 16 + *c - '0';
                else if (*c >= 'a' && *c <= 'z')
                    val = val * 16 + *c - 'a' + 10;
                else if (*c >= 'A' && *c <= 'Z')
                    val = val * 16 + *c - 'A' + 10;
                c++;
            }
            return { { T, loc, PyrSlot::make(val) } };
        }

        else if constexpr (T == TokenType::Ascii) {
            const auto& str = textInfo->normalisedSource.as_string();
            const char* c_str = str.c_str();
            assert(c_str[loc.begin.absolute] == '$');
            if (loc.size() == 2) {
                const char out = c_str[loc.begin.absolute + 1];
                if (out == 0)
                    // TODO: consider this change in more detail.
                    // This is a little bit odd, but appears to be the current behaviour.
                    // I believe this is because the language client places extra spaces after the code you evaluate.
                    // There might be discrepencies here between class files, other language clients, and even depending
                    // on how you evaluate a file, I've opted to make this consistent. If we need the null terminator
                    // character, we could use $\0, although that currently produces the same of $0.
                    return { { T, loc, PyrSlot::make(' ') } };
                return { { T, loc, std::optional<PyrSlot> { PyrSlot::make(out) } } };
            }
            assert(loc.size() == 3);
            assert(c_str[loc.begin.absolute] == '$');
            assert(c_str[loc.begin.absolute + 1] == '\\');
            // Three only occurs for the following escape characters.
            char out = c_str[loc.begin.absolute + 2];
            if (out == 'n')
                out = '\n';
            else if (out == 'r')
                out = '\r';
            else if (out == 't')
                out = '\t';
            else if (out == 'f')
                out = '\f';
            else if (out == 'v')
                out = '\v';

            return { Output { T, loc, std::optional<PyrSlot> { PyrSlot::make(out) } } };
        }

        else if constexpr (T == TokenType::AccidentalCents)
            return { { TokenType::AccidentalCents, loc, process_accidental_cents(fill_temp_buf(loc)) } };
        else if constexpr (T == TokenType::AccidentalSteps)
            return { { TokenType::AccidentalSteps, loc, process_accidental_steps(fill_temp_buf(loc)) } };

        else
            return { { T, loc } };
    }


private:
    std::string temp_buffer {};

    template <TokenType T> PyrSlot to_constant() {
        static_assert(
            sc::lex::matches(T, TokenType::Pi, TokenType::Nil, TokenType::Inf, TokenType::True, TokenType::False));
        if constexpr (T == TokenType::Pi)
            return PyrSlot::make(pi);
        else if constexpr (T == TokenType::Nil)
            return PyrSlot::make(PyrNil {});
        else if constexpr (T == TokenType::Inf)
            return PyrSlot::make(std::numeric_limits<double>::max());
        else if constexpr (T == TokenType::True)
            return PyrSlot::make(true);
        else if constexpr (T == TokenType::False)
            return PyrSlot::make(false);
    }

    const char* fill_temp_buf(lex::SourceCodeRange loc) {
        const auto& str = textInfo->normalisedSource.as_string();
        const char* c_str = str.c_str();
        temp_buffer.clear();
        temp_buffer.insert(temp_buffer.begin(), c_str + loc.begin.absolute, c_str + loc.end.absolute);
        return temp_buffer.c_str();
    }

    PyrSymbol* text_to_symbol(lex::SourceCodeRange loc, size_t drop_from_start = 0, size_t drop_from_end = 0,
                              bool needs_escaping = false) {
        temp_buffer.clear();

        const auto& str = textInfo->normalisedSource.as_string();
        const char* c_str = str.c_str();
        auto start = c_str + loc.begin.absolute + drop_from_start;
        const auto end = c_str + loc.end.absolute - drop_from_end;
        const size_t sz = end - start;
        if (!needs_escaping) {
            temp_buffer.insert(temp_buffer.begin(), start, end);
            return getsym(temp_buffer.c_str());
        }
        bool escaped = false;
        auto from_it = start;
        for (; from_it != end; from_it += 1) {
            if (*from_it == '\\' && !escaped) {
                escaped = true;
                // don't write
                continue;
            }

            // Bit odd, we actually only use the escape character to escape the delimiter.
            // This is very weird because the escape character does nothing, other than not print itself (occurs in
            // quotes symbols, '\n' == 'n').
            temp_buffer.push_back(*from_it);
            escaped = false;
        }
        return getsym(temp_buffer.c_str());
    }

    template <TokenType T> constexpr auto get_closing_bracket() -> decltype(auto) {
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

struct ParserState {
    // Lifetime is complex here due to the class library jumping around to different files, shared_ptr isn't the most
    // performant, but is the simplest to deal with.
    std::shared_ptr<const TextInfo> textInfo;
    // Converts lexer tokens in to parser tokens.
    BisonLexerAction action;
    // Iterates through the source code.
    lex::CodePointStream codePointStream;
    // cmd line code must emit a special token at the start, this is the state that implements that.
    enum struct Mode { CMDInitial, CMDContinue, ClassLibrary } mode;

    // Ugly cache used for turning string lines from the lexer into a single string.
    // TODO: refactor pyrparse nodes (slot node in particular) so we can have a them create the literals from the text
    // and token during compilation.
    std::optional<BisonLexerAction::Output> cached {};
};


std::optional<ParserState> gParserState {};

[[nodiscard]] std::string mkLexingError(const TextInfo& txtInfo, BisonSemActionOutput o) {
    if (o.is(ExtendedErrors::GotCurlyExpectedParen) || o.is(ExtendedErrors::GotSquareExpectedParen)) {
        if (o.extra_range_of_error) {
            const DiagnosticHighlight highlights[2] {
                txtInfo.createDiagnosticHighlight(*o.extra_range_of_error, "Parenthsises opened here..."),
                txtInfo.createDiagnosticHighlight(o.range, "...was expected to be closed here."),
            };
            return diagnosticToCompilerError(ErrorType::Error, "Parenthises mismatch.", highlights, 2);
        } else {
            // Should not happen.
            assert(false);
            const DiagnosticHighlight h =
                txtInfo.createDiagnosticHighlight(o.range, "Parenthises opened here was expected to be closed.");
            return diagnosticToCompilerError(ErrorType::Error, "Parenthises mismatch", &h, 1);
        }
    } else if (o.is(ExtendedErrors::GotCurlyExpectedSquare) || o.is(ExtendedErrors::GotParenExpectedSquare)) {
        if (o.extra_range_of_error) {
            const DiagnosticHighlight highlights[2] {
                txtInfo.createDiagnosticHighlight(*o.extra_range_of_error, "Square bracket opened here..."),
                txtInfo.createDiagnosticHighlight(o.range, "...was expected to be closed here."),
            };
            return diagnosticToCompilerError(ErrorType::Error, "Square bracket mismatch.", highlights, 2);
        } else {
            assert(false);
            const DiagnosticHighlight h =
                txtInfo.createDiagnosticHighlight(o.range, "Square bracket opened here was expected to be closed.");
            return diagnosticToCompilerError(ErrorType::Error, "Square bracket mismatch", &h, 1);
        }
    } else if (o.is(ExtendedErrors::GotParenExpectedCurly) || o.is(ExtendedErrors::GotSquareExpectedCurly)) {
        if (o.extra_range_of_error) {
            const DiagnosticHighlight highlights[2] {
                txtInfo.createDiagnosticHighlight(*o.extra_range_of_error, "Curly bracket opened here..."),
                txtInfo.createDiagnosticHighlight(o.range, "...was expected to be closed here."),
            };
            return diagnosticToCompilerError(ErrorType::Error, "Curly bracket mismatch.", highlights, 2);
        } else {
            assert(false);
            const DiagnosticHighlight h =
                txtInfo.createDiagnosticHighlight(o.range, "Curly bracket opened here was expected to be closed.");
            return diagnosticToCompilerError(ErrorType::Error, "Curly bracket mismatch", &h, 1);
        }
    } else if (o.is(ExtendedErrors::ExtraClosingCurlyBracket)) {
        const DiagnosticHighlight h = txtInfo.createDiagnosticHighlight(
            o.range, "Unexpected closing curly braket, could not find a matching opening one.");
        return diagnosticToCompilerError(ErrorType::Error, "Curly bracket mismatch", &h, 1);
    } else if (o.is(ExtendedErrors::ExtraClosingParenBracket)) {
        const DiagnosticHighlight h = txtInfo.createDiagnosticHighlight(
            o.range, "Unexpected closing parenthesis, could not find a matching opening one.");
        return diagnosticToCompilerError(ErrorType::Error, "Paranthesis mismatch", &h, 1);
    } else if (o.is(ExtendedErrors::ExtraClosingSqaureBracket)) {
        const DiagnosticHighlight h = txtInfo.createDiagnosticHighlight(
            o.range, "Unexpected closing square bracket, could not find a matching opening one.");
        return diagnosticToCompilerError(ErrorType::Error, "Square bracket mismatch", &h, 1);
    } else if (o.is(TokenType::ErMissingExponent)) {
        const auto [ptr, sz] = txtInfo.indexIntoSource(o.range);
        const std::string example { ptr, sz };
        auto desc = std::string { "Expected digits after the 'e', for example '" } + example + "10'.";

        const auto h = txtInfo.createDiagnosticHighlight(o.range, std::move(desc));
        return diagnosticToCompilerError(ErrorType::Error, "Invalid float exponent.", &h, 1);
    }

    else if (o.is(TokenType::ErSymbolQuoteUnclosed)) {
        const auto [ptr, sz] = txtInfo.indexIntoSource(o.range);
        size_t i { 0 };
        while (i < sz && ptr[i] != ' ' && ptr[i] != '\n')
            ++i;
        // TODO: we could look forward to see if the next token (discarding whitespace) is a '\'', in that case,
        // the user has a new line character in the wrong place.
        const std::string example { ptr, i };
        auto desc = std::string { "This quoted symbol does not have a matching closing quote, perhaps you meant "
                                  + example + "'?" };
        const auto h = txtInfo.createDiagnosticHighlight(o.range, std::move(desc));
        return diagnosticToCompilerError(ErrorType::Error, "Invalid symbol.", &h, 1);
    }

    else if (o.is(TokenType::ErInvalidUTF8)) {
        const auto h = txtInfo.createDiagnosticHighlight(o.range, "this is invalid utf8, please delete it.");
        return diagnosticToCompilerError(ErrorType::Error, "Invalid utf8", &h, 1);
    }

    else if (o.is(TokenType::ErInvalidToken)) {
        const auto h = txtInfo.createDiagnosticHighlight(o.range, "this token is invalid in this context.");
        return diagnosticToCompilerError(ErrorType::Error, "Invalid token.", &h, 1);
    }

    else if (o.is(TokenType::ErStringUnclosed)) {
        const auto [ptr, sz] = txtInfo.indexIntoSource(o.range);
        size_t i { 0 };
        while (i < sz && ptr[i] != '\n' && ptr[i] != ' ')
            ++i;
        const std::string example { ptr, i };

        const auto h = txtInfo.createDiagnosticHighlight(
            o.range, std::string { "This string does not have a closing '\"', perhaps you meant " + example + "\"?" });
        return diagnosticToCompilerError(ErrorType::Error, "Unclosed string.", &h, 1);
    } else if (o.is(TokenType::ErMultilineCommentUnclosed)) {
        const auto h = txtInfo.createDiagnosticHighlight(o.range, "this comment lacks a closing */.");
        return diagnosticToCompilerError(ErrorType::Error, "Unclosed string.", &h, 1);
    } else if (o.is(TokenType::ErASCIIInvalidWhitespace)) {
        const auto h =
            txtInfo.createDiagnosticHighlight(o.range, "did you mean either: '$ ' (missing space) or '$\\n'?");
        return diagnosticToCompilerError(ErrorType::Error, "Invalid whitespace in char", &h, 1);
    } else {
        const auto h = txtInfo.createDiagnosticHighlight(o.range, "an unknown error has occured right here!");
        return diagnosticToCompilerError(ErrorType::Error, "Unknown error.", &h, 1);
    }
}

// Only ever called from inside of yyparse.
// Right now, this allocates using the GC, this should be removed.
int yylex() {
    assert(gParserState);
    ParserState& s = *gParserState;
    if (s.mode == ParserState::Mode::CMDInitial) {
        s.mode = ParserState::Mode::CMDContinue;
        return INTERPRET;
    }

    const auto prepForOutput = [&](BisonSemActionOutput o) {
        yylval.empty = {};
        yylloc = o.range;
        if (o.slot)
            yylval.slotNode = bison_cxt->allocParseNode<PyrSlotNode>(o.range, *o.slot);
        if (o.is_error()) {
            auto str = mkLexingError(*bison_cxt->textInfo.get(), o);
            bison_cxt->postError(str);
            // These errors are swallowed, printed, but not returned.
            // In a future version this needs to be removed.
            if (o.type == TokenType::ErInvalidUTF8 || o.type == TokenType::ErUnexpectedUnicode) {
                return yylex();
            } else {
                return static_cast<int>(YYerror); // This suppresses the printing of the error message that the parse
                                                  // generates because we have already printed one.
            }
        }
        return static_cast<int>(*convert_to_bison_tokentype(o.type));
    };


    // If we have a cached out return it.
    // This is necessary for the string line bodge while we migrate, it can be remove in the future once the
    // parser & compiler know how to deal with string lines.
    if (s.cached) {
        const auto o = std::move(*s.cached);
        s.cached.reset();
        return prepForOutput(o);
    }

    BisonLexerAction::Output out = lex::lexer(s.codePointStream, s.action);

    if (out.type != TokenType::StringLine)
        return prepForOutput(out);

    sc::lex::SourceCodeLocation start { out.range.begin };
    std::string str {};
    str.reserve(128);

    auto prev = out;
    while (true) {
        // This is nasty, but in the future, this should move into the compiler making this unnecessary.
        if (out.type != TokenType::StringLine) {
            assert(!s.cached.has_value());

            // This is the one case in the whole lexer where we currently have to alloc using the GC.
            // This would be much better pushed into the compiler.
            const int flags = s.textInfo->isClassFile ? obj_permanent | obj_immutable : obj_immutable;
            auto sc_str = newPyrString(gMainVMGlobals->gc, str.c_str(), flags, false);
            yylval.slotNode = bison_cxt->allocParseNode<PyrSlotNode>(out.range, PyrSlot::make(sc_str));
            yylloc = out.range;

            s.cached = std::move(out); // save for next time.
            return STRING;
        }
        auto range = out.range;
        // This is dodgy, we are dropping the quotes here.
        // Again, once this is in the compilation phase, this becomes nice.
        range.begin.absolute += 1;
        range.end.absolute -= 1;

        bool escaped = false;
        const auto [bb, sz] = s.codePointStream.source_code_range_to_text(range);
        for (auto b = bb; b < (bb + sz); ++b) {
            if (*b == '\\' && !escaped) {
                escaped = true;
                continue;
            }

            if (escaped) {
                if (*b == 'n')
                    str += '\n';
                else if (*b == 'r')
                    str += '\r';
                else if (*b == 't')
                    str += '\t';
                else if (*b == 'f')
                    str += '\f';
                else if (*b == 'v')
                    str += '\v';
                else
                    str += *b;
                escaped = false;
            } else {
                str += *b;
            }
        }

        prev = out;
        out = lex::lexer(s.codePointStream, s.action);
    }
}

PyrSlot process_accidental_cents(const char* s) {
    const char* c = s;
    double degree = 0.;
    double cents = 0.;
    double centsdiv = 1000.;
    while (*c) {
        if (*c >= '0' && *c <= '9')
            degree = degree * 10. + *c - '0';
        else
            break;
        c++;
    }

    if (*c == 'b')
        centsdiv = -1000.;
    else if (*c == 's')
        centsdiv = 1000.;
    c++;

    while (*c) {
        if (*c >= '0' && *c <= '9') {
            cents = cents * 10. + *c - '0';
        } else
            break;
        c++;
    }

    if (cents > 499.)
        cents = 499.;

    return PyrSlot::make(degree + cents / centsdiv);
}

PyrSlot process_accidental_steps(const char* s) {
    const char* c = s;
    double degree = 0.;
    double semitones = 0.;
    while (*c) {
        if (*c >= '0' && *c <= '9')
            degree = degree * 10. + *c - '0';
        else
            break;
        c++;
    }

    while (*c) {
        if (*c == 'b')
            semitones -= 1.;
        else if (*c == 's')
            semitones += 1.;
        c++;
    }

    if (semitones > 4.)
        semitones = 4.;
    else if (semitones < -4.)
        semitones = -4.;

    return PyrSlot::make(degree + semitones / 10.);
}


struct ParseClassException {
    ~ParseClassException() = default;
    [[nodiscard]] virtual std::string getError() & = 0;
    [[nodiscard]] virtual std::string getError() && = 0;
};

struct ParseClassExceptionSimple : ParseClassException {
    ParseClassExceptionSimple() = delete;
    ParseClassExceptionSimple(std::shared_ptr<TextInfo> textInfo, sc::lex::SourceCodeRange range, const char* desc):
        textInfo(std::move(textInfo)),
        range(range),
        desc(desc) {}
    ~ParseClassExceptionSimple() = default;
    ParseClassExceptionSimple(ParseClassExceptionSimple&&) noexcept = default;
    ParseClassExceptionSimple(const ParseClassExceptionSimple&) noexcept = default;

    std::shared_ptr<TextInfo> textInfo;
    sc::lex::SourceCodeRange range;
    const char* desc;

    [[nodiscard]] std::string getError() & override {
        const auto highlight = textInfo->createDiagnosticHighlight(range, { desc });
        return diagnosticToCompilerError(ErrorType::Error, "Parsing error.", &highlight, 1);
    }
    [[nodiscard]] std::string getError() && override {
        const auto highlight = textInfo->createDiagnosticHighlight(range, { desc });
        return diagnosticToCompilerError(ErrorType::Error, "Parsing error.", &highlight, 1);
    }
};
struct ParseClassExceptionBracket : ParseClassException {
    ParseClassExceptionBracket() = delete;
    ParseClassExceptionBracket(TextInfo f_info, sc::lex::SourceCodeRange start, sc::lex::SourceCodeRange end,
                               std::string startDesc, std::string endDesc):
        f_info(f_info),
        start(start),
        end(end),
        startDesc(startDesc),
        endDesc(std::move(endDesc)) {}
    ~ParseClassExceptionBracket() = default;
    ParseClassExceptionBracket(ParseClassExceptionBracket&&) noexcept = default;
    ParseClassExceptionBracket(const ParseClassExceptionBracket&) noexcept = default;

    TextInfo f_info;

    sc::lex::SourceCodeRange start, end;
    std::string startDesc, endDesc;

    [[nodiscard]] std::string getError() & override {
        const DiagnosticHighlight highlight[2] {
            f_info.createDiagnosticHighlight(start, std::string { startDesc }),
            f_info.createDiagnosticHighlight(end, std::string { endDesc }),
        };
        return diagnosticToCompilerError(ErrorType::Error, "Parsing error.", highlight, 2);
    }
    [[nodiscard]] std::string getError() && override {
        const DiagnosticHighlight highlight[2] {
            f_info.createDiagnosticHighlight(start, std::move(startDesc)),
            f_info.createDiagnosticHighlight(end, std::move(endDesc)),
        };
        return diagnosticToCompilerError(ErrorType::Error, "Parsing error.", highlight, 2);
    }
};

struct OptionalIndex {
    static constexpr auto invalid = std::numeric_limits<std::size_t>::max();
    [[nodiscard]] constexpr static OptionalIndex valid(std::size_t v) {
        assert(v != invalid);
        return { v };
    }
    constexpr OptionalIndex(): v(invalid) {}
    constexpr OptionalIndex(OptionalIndex&&) noexcept = default;
    constexpr OptionalIndex(const OptionalIndex&) noexcept = default;
    constexpr OptionalIndex& operator=(OptionalIndex&&) noexcept = default;
    constexpr OptionalIndex& operator=(const OptionalIndex&) noexcept = default;
    ~OptionalIndex() = default;
    [[nodiscard]] explicit operator bool() const { return v != invalid; }
    [[nodiscard]] [[nodiscard]] std::size_t valueUnchecked() const { return v; }

private:
    constexpr OptionalIndex(std::size_t v): v(v) {}
    std::size_t v;
};
struct ClassDependency {
    ClassDependency(PyrSymbol* name, PyrSymbol* superClassName, std::shared_ptr<TextInfo>& textInfo,
                    sc::lex::SourceCodeRange range, sc::lex::SourceCodeRange rangeOfClassname,
                    std::optional<sc::lex::SourceCodeRange> rangeOfSuperClass):
        className(name),
        superClassName(superClassName),
        textInfo(textInfo),
        range(range),
        rangeOfClassName(rangeOfClassname),
        rangeOfSuperClass(rangeOfSuperClass) {}

    ClassDependency(ClassDependency&&) noexcept = default;
    ClassDependency(const ClassDependency&) = default;
    ClassDependency& operator=(ClassDependency&&) noexcept = default;
    ClassDependency& operator=(const ClassDependency&) = default;

    PyrSymbol* className;
    PyrSymbol* superClassName; // can be nullptr
    std::shared_ptr<TextInfo> textInfo;
    sc::lex::SourceCodeRange range;
    sc::lex::SourceCodeRange rangeOfClassName;
    std::optional<sc::lex::SourceCodeRange> rangeOfSuperClass;
    // Set in a second pass
    OptionalIndex parent {}, firstChild {}, lastChild {}, nextSibling {};
};

struct ClassDependencyList {
    std::unordered_map<PyrSymbol*, std::size_t> className2DepIndex;
    std::vector<ClassDependency> deps;

    // returns false if duplicate
    bool add(ClassDependency dep) {
        const auto name = dep.className;
        if (className2DepIndex.find(name) != std::end(className2DepIndex)) {
            return false;
        }
        const auto index = deps.size();
        deps.push_back(std::move(dep));
        className2DepIndex.emplace(name, index);
        return true;
    }


    // returns vector of dependencys whose super class is not defined.
    void linkReturnUndefinedSuperClasses() {
        const auto count = deps.size();
        for (std::size_t i { 0 }; i < count; ++i) {
            auto& dep = deps[i];
            if (!dep.superClassName)
                continue;

            auto fnd = className2DepIndex.find(dep.superClassName);
            if (fnd == className2DepIndex.end()) {
                continue;
            }
            auto& parent = deps[fnd->second];
            if (!parent.firstChild) {
                parent.firstChild = OptionalIndex::valid(i);
                parent.lastChild = OptionalIndex::valid(i);
            } else {
                assert(parent.lastChild);
                const auto last = parent.lastChild.valueUnchecked();
                assert(!deps[last].nextSibling);
                deps[last].nextSibling = OptionalIndex::valid(i);
                parent.lastChild = OptionalIndex::valid(i);
            }
        }
    }

    struct TopoResult {
        std::vector<ClassDependency> valid;
        std::vector<std::vector<ClassDependency>> loops;
        [[nodiscard]] explicit operator bool() const { return loops.empty(); }
    };

    [[nodiscard]] TopoResult getTopologicalOrdering() const {
        const auto count = deps.size();
        std::vector<Mark> marks(count, Mark::NotVisited);

        std::vector<ClassDependency> out, loop;
        out.reserve(count);

        const auto objIndex = className2DepIndex.at(s_abstract_object);
        std::unordered_set<std::size_t> toVisit;
        toVisit.reserve(deps.size());
        for (std::size_t i { 0 }; i < deps.size(); ++i) {
            toVisit.insert(i);
        }

        topoSortHasLoop(objIndex, marks, out, toVisit);
        std::reverse(out.begin(), out.end());

        std::vector<std::vector<ClassDependency>> loops;
        std::vector<std::size_t> toVisitCopy;
        toVisitCopy.reserve(toVisit.size());
        std::copy(toVisit.begin(), toVisit.end(), std::back_inserter(toVisitCopy));
        for (auto it : toVisitCopy) {
            if (toVisit.count(it) == 1) {
                std::vector<ClassDependency> chain;
                buildLoops(it, toVisit, chain);

                std::sort(chain.begin(), chain.end(), [](const ClassDependency& l, const ClassDependency& r) {
                    const auto lex = strcmp(l.textInfo->filePathDescription.name, r.textInfo->filePathDescription.name);
                    return lex == 0 ? l.rangeOfClassName < r.rangeOfClassName : lex;
                });


                loops.push_back(std::move(chain));
            }
        }
        return { out, loops };
    }

private:
    void buildLoops(std::size_t i, std::unordered_set<std::size_t>& toVisit,
                    std::vector<ClassDependency>& chain) const {
        toVisit.erase(i);
        chain.push_back(deps[i]);

        for (auto it = deps[i].firstChild; it; it = deps[it.valueUnchecked()].nextSibling) {
            if (toVisit.count(it.valueUnchecked()) == 1) {
                buildLoops(it.valueUnchecked(), toVisit, chain);
            }
        }
    }


    enum struct Mark { NotVisited, InProgress, Visited };
    void topoSortHasLoop(std::size_t i, std::vector<Mark>& marks, std::vector<ClassDependency>& sorted,
                         std::unordered_set<std::size_t>& visited) const {
        // DFS
        switch (marks[i]) {
        case Mark::InProgress:
        default:
            unreachable();
        case Mark::Visited:
            return;
        case Mark::NotVisited:
            marks[i] = Mark::InProgress;
            bool foundLoop { false };
            // if we find a loop, all classes added to sorted are invalidated
            const auto startingSize = sorted.size();
            for (auto it = deps[i].firstChild; it; it = deps[it.valueUnchecked()].nextSibling) {
                topoSortHasLoop(it.valueUnchecked(), marks, sorted, visited);
            }
            visited.erase(i);
            marks[i] = Mark::Visited;
            sorted.push_back(deps[i]);
            return;
        }
    }
};

struct ClassExtentionFile {
    std::shared_ptr<TextInfo> textInfo;
    sc::lex::SourceCodeLocation start;
};

bool compile(CompilerContext& cxt) {
    const auto on_parse_sucess = [&](PyrRootNode& root) {
        // Prints errors for us.
        // TODO: this would be nicer if it returned diagnostics
        compileNodeList(cxt, &root, true);
        return cxt.errors == 0;
    };

    const auto on_parse_failure = [&](const std::vector<CompilerContext::ParseErrorInCurFile>& errors, int error_code) {
        for (const auto& error : errors) {
            const auto highlight = cxt.textInfo->createDiagnosticHighlight(error.location, std::string { error.msg });
            const auto str = diagnosticToCompilerError(ErrorType::Error, "parse error", &highlight, 1);
            cxt.postError(str.c_str(), error.versionOfError);
        }
        return false;
    };


    return parse(cxt, on_parse_sucess, on_parse_failure);
}

bool compile(const ClassDependency& dep) {
    CompilerContext cxt { dep.textInfo, {}, dep.range, nullptr };
    return compile(cxt);
}

bool compile(const ClassExtentionFile& ext) {
    CompilerContext cxt { ext.textInfo, {}, ext.start, nullptr };
    return compile(cxt);
}


void findDiscrepancy();

struct ActionSkipWhitespace {
    struct Output {
        sc::lex::TokenType type;
        sc::lex::SourceCodeRange range;
    };
    template <sc::lex::TokenType type> std::optional<Output> process(sc::lex::SourceCodeRange loc) {
        if constexpr (sc::lex::is_whitespace(type) || sc::lex::is_comment(type))
            return std::nullopt;
        else
            return { { type, loc } };
    }
};

template <typename... TS>
ActionSkipWhitespace::Output match(std::shared_ptr<TextInfo>& f_info, sc::lex::CodePointStream& cps,
                                   ActionSkipWhitespace& action, const char* desc, TS... ts) {
    auto out = lex::lexer(cps, action);

    if (((out.type == ts) || ...))
        return out;

    throw ParseClassExceptionSimple { f_info, out.range, desc };
}

// Must have *just* consumed the opening bracket.
ActionSkipWhitespace::Output matchClosingBracket(std::shared_ptr<TextInfo>& f_info, sc::lex::CodePointStream& cps,
                                                 ActionSkipWhitespace& action, sc::lex::SourceCodeRange loc_of_open,
                                                 sc::lex::TokenType opening) {
    assert(sc::lex::is_open_bracket(opening));
    const auto closing = sc::lex::get_closing_bracket(opening);
    assert(sc::lex::is_close_bracket(closing));
    const auto [c_start, o_end] = sc::lex::get_opening_brackets(closing);

    auto out = lex::lexer(cps, action);

    std::size_t bracket_level { 1 };

    while (true) {
        if (out.type == sc::lex::TokenType::EndOfFile)
            throw ParseClassExceptionSimple { f_info, loc_of_open, "Could not find closing bracket opened here." };
        else if (c_start <= out.type && out.type < o_end)
            ++bracket_level;
        else if (out.type == closing) {
            --bracket_level;
            if (bracket_level == 0)
                return out;
        }

        out = lex::lexer(cps, action);
    }
}

// Called in a loop until it returns false.
bool initaliseClassDependencyListAndRegisterExtensions(std::shared_ptr<TextInfo>& textInfo,
                                                       sc::lex::CodePointStream& cps, ActionSkipWhitespace& action,
                                                       ClassDependencyList& depList,
                                                       std::vector<ClassExtentionFile>& extList) {
    // What we want here is an error resistant approach to parsing.
    // In future this should be rewritten so we can avoid manual checks, this will involve a more 'theoretical' and
    // consider approach to parsing. Right now, we just have some basic cases. It is very easy to end up with an
    // un-parsable file, which will result in valid class definitions being missed.

    const auto first =
        match(textInfo, cps, action, "Expected class name or '+' for extention class", TokenType::ClassName,
              TokenType::KeywordBinaryOperator, TokenType::Add, TokenType::EndOfFile);

    if (first.type == TokenType::EndOfFile)
        return false; // This is the main exit of the loop.
    else if (first.type == TokenType::KeywordBinaryOperator) {
        std::string msg = "Replace with '";
        const auto [ptr, sz] = textInfo->indexIntoSource(first.range);
        msg.append(ptr, sz - 1);
        msg += " :'";
        const auto hg = textInfo->createDiagnosticHighlight(first.range, msg);
        const auto err =
            diagnosticToCompilerError(ErrorType::Error, "Must have a space between class name and the colon.", &hg, 1);
        postText(err.c_str(), err.size());

        // Skip this class definition.
        const auto maybeSuper = match(textInfo, cps, action, "Expected superclass after ':'", TokenType::ClassName);
        const auto open_curly = match(textInfo, cps, action, "Expected open curly bracket '{'.", TokenType::OpenCurly);
        const auto close_curly = matchClosingBracket(textInfo, cps, action, open_curly.range, TokenType::OpenCurly);
        return true; // try to parse the remainder of the file.
    } else if (first.type == TokenType::Add) {
        const auto class_name = match(textInfo, cps, action, "Expected class name after '+'", TokenType::ClassName);
        const auto open_curly = match(textInfo, cps, action, "Expected open curly bracket '{'.", TokenType::OpenCurly);
        const auto close_curly = matchClosingBracket(textInfo, cps, action, open_curly.range, TokenType::OpenCurly);
        // This marks the rest of the file as an extention, do not continue.
        extList.push_back(ClassExtentionFile { textInfo, first.range.begin });
        return false;
    }

    assert(first.type == TokenType::ClassName);
    const auto [ptr, sz] = cps.source_code_range_to_text(first.range);
    const auto className = getsymlen(ptr, sz);

    // mutable variable, is updated as we step through the code.
    auto next = lex::lexer(cps, action);

    if (next.type == TokenType::OpenSquare) {
        // TODO: these checks can be moved to the compilation stage
        if (className == s_object)
            throw ParseClassExceptionSimple { textInfo, first.range, "Class 'Object' cannot have an index spec." };

        if (className == s_abstract_object)
            throw ParseClassExceptionSimple { textInfo, first.range,
                                              "Class 'AbstractObject' cannot have an index spec." };

        matchClosingBracket(textInfo, cps, action, next.range, TokenType::OpenSquare);
        next = lex::lexer(cps, action);
    }

    std::optional<sc::lex::SourceCodeRange> superloc {};
    PyrSymbol* superName;
    if (next.type != TokenType::Colon) {
        superName = className == s_object ? s_none : s_object;
    } else {
        if (className == s_abstract_object)
            throw ParseClassExceptionSimple { textInfo, first.range,
                                              "Class 'AbstractObject' cannot inherit from another class." };
        const auto super = match(textInfo, cps, action, "Expected a super class name", TokenType::ClassName);
        const auto [ptr, sz] = cps.source_code_range_to_text(super.range);

        superName = getsymlen(ptr, sz);
        superloc = super.range;

        next = lex::lexer(cps, action);
    }

    if (next.type != TokenType::OpenCurly) {
        throw ParseClassExceptionSimple { textInfo, next.range, "Expected open curly bracket '{'." };
    }
    matchClosingBracket(textInfo, cps, action, next.range, TokenType::OpenCurly);

    depList.add(ClassDependency {
        className,
        superName,
        textInfo,
        { first.range.begin, cps.end_token() },
        first.range,
        superloc,
    });
    return true; // keep going
}

// Returns whether there are more classes to parse in the file.
bool declareClassForDependencyTree(std::shared_ptr<TextInfo>& textInfo, sc::lex::CodePointStream& cps,
                                   ActionSkipWhitespace& action, ClassDependencyList& depList,
                                   std::vector<ClassExtentionFile>& extList) {
    try {
        return initaliseClassDependencyListAndRegisterExtensions(textInfo, cps, action, depList, extList);
    } catch (ParseClassException& p) {
        // TODO: consider implementing some for of error recovery here by skipping some classes.
        // As this only prints one error per file.
        // Alternatively, if we move towards a language server, this is unnecessary.
        const auto str = std::move(p).getError();
        postText(str.c_str(), str.size());
        return false;
    }
}

/**
 * \brief \c true if \c dir is one of the language config's default classlib directories
 */
static bool isDefaultClassLibraryDirectory(const fs::path& dir) {
    auto const& defaultDirs = gLanguageConfig->defaultClassLibraryDirectories();
    auto const iter = std::find(defaultDirs.begin(), defaultDirs.end(), dir);
    return iter != defaultDirs.end();
}

using ClassLibraryFileMap = std::unordered_map<PyrSymbol*, std::shared_ptr<TextInfo>>;

/**
 * \brief Handles a missing directory encountered during compilation.
 *
 * If the directory is one of the default directories traversed during compilation,
 * try to create it, silently ignoring failure (most likely from permissions failure).
 * Otherwise, warn the user to help catch mistyped/missing directory names. See #3468.
 */
static void passOne_HandleMissingDirectory(const fs::path& dir) {
    if (isDefaultClassLibraryDirectory(dir)) {
        std::error_code ec {};
        fs::create_directories(dir, ec);
    } else {
        post("WARNING: Could not open directory: '%s'\n"
             "\tTo resolve this, either create the directory or remove it from your compilation paths.\n\n",
             SC_Codecvt::path_to_utf8_str(dir).c_str());
    }
}


bool passOne_ProcessOneFile(const fs::path& path, ClassLibraryFileMap& files, struct PyrGC* gc,
                            ClassDependencyList& depList, std::vector<ClassExtentionFile>& extList);

/** \brief Determines whether the directory should be skipped during compilation.
 *
 * \param dir : The directory to check, as a `path` object
 * \returns `true` iff any of the following conditions is true:
 * - the directory has already been compiled
 * - the language configuration says this path is excluded
 * - SC_Filesystem::shouldNotCompileDirectory(dir) returns `true`
 */
static bool passOne_ShouldSkipDirectory(const fs::path& dir, const std::set<fs::path>& compiledDirs) {
    return (compiledDirs.find(dir) != compiledDirs.end()) || (gLanguageConfig && gLanguageConfig->pathIsExcluded(dir))
        || (SC_Filesystem::instance().shouldNotCompileDirectory(dir));
}

/** \brief Compile the contents of a single directory
 *
 * This method compiles any .sc files in a single directory, working
 * via depth-first recursion. This routine is designed to fail gracefully,
 * and only indicates failure if something truly unexpected happens. These
 * conditions are:
 * - an error occurred while trying to open a directory, other than the case
 *    the case that the object doesn't exist.
 * - an error occurred while calling `passOne_processOneFile` on a file
 * - an error occurred in a recursive call of this routine on a macOS alias
 * Otherwise, this method returns success, even if:
 * - `dir` does not exist
 * - Iterating to the next file fails for any reason at all
 *
 * This method returns with a success state immediately if the directory
 * should not be compiled according to the language configuration.
 *
 * \param dir : The directory to traverse, as a `path` object
 * \returns `true` if processing was successful, `false` if it failed.
 *   See above for what constitutes success and failure conditions.
 */
static bool passOne_ProcessDir(const fs::path& dir, std::set<fs::path>& compiledDirs, ClassLibraryFileMap& files,
                               PyrGC* gc, ClassDependencyList& deps, std::vector<ClassExtentionFile>& extList) {
    // Prefer non-throwing versions of filesystem functions, since they are actually not unexpected
    // and because it's faster to use error codes.
    std::error_code ec;

    // Perform tilde expansion on incoming dir.
    const fs::path expdir = SC_Filesystem::instance().expandTilde(dir);

    // Using a recursive_directory_iterator is much faster than actually calling this function
    // recursively. Speedup from the switch was about 1.5x. _Do_ recurse on symlinks.
    fs::recursive_directory_iterator rditer(expdir, fs::directory_options::follow_directory_symlink, ec);

    // Check preconditions: are we able to access the file, and should we compile it according to
    // the language configuration?
    if (ec) {
        // If we got an error, post a warning if it was because the target wasn't found, and return success.
        // Otherwise, post the error and fail.
        if (ec.default_error_condition() == std::errc::no_such_file_or_directory) {
            passOne_HandleMissingDirectory(expdir);
            return true;
        } else {
            error("Could not open directory '%s': (%d) %s\n", SC_Codecvt::path_to_utf8_str(expdir).c_str(), ec.value(),
                  ec.message().c_str());

            return false;
        }
    } else if (passOne_ShouldSkipDirectory(expdir, compiledDirs)) {
        // If we should skip the directory, just return success now.
        return true;
    } else {
        // Let the user know we are in fact compiling this directory.
        post("\tCompiling directory '%s'\n", SC_Codecvt::path_to_utf8_str(expdir).c_str());
    }

    // Record that we have touched this directory already.
    compiledDirs.insert(expdir);

    // Invariant: we have processed (or begun to process) every directory or file already
    // touched by the iterator.
    while (rditer != fs::end(rditer)) {
        const fs::path path = *rditer;

        // If the file is a directory, perform the same checks as above to see if we should
        // skip compilation on it.
        if (fs::is_directory(path)) {
            if (passOne_ShouldSkipDirectory(path, compiledDirs)) {
                rditer.disable_recursion_pending(); // don't "push" into the next level of the hierarchy
            } else {
                // Mark this directory as compiled.
                // By not calling no_push(), we allow the iterator to enter the directory
                compiledDirs.insert(path);
            }

        } else { // ordinary file
            // Try to resolve a potential alias. Possible outcomes:
            // - it was an alias & is also a directory: try to recurse on it
            // - resolution failed: returns empty path: let the user know
            // - it was not an alias, or was an alias that wasn't a directory: try to process it as a source file
            bool isAlias = false;
            const fs::path& respath = SC_Filesystem::resolveIfAlias(path, isAlias);
            if (isAlias && fs::is_directory(respath)) {
                // If the resolved alias is a directory, recurse on it.
                if (!passOne_ProcessDir(respath, compiledDirs, files, gc, deps, extList)) {
                    return false;
                }
            } else if (respath.empty()) {
                error("Could not resolve symlink: %s\n", SC_Codecvt::path_to_utf8_str(path).c_str());

            } else {
                if (passOne_ProcessOneFile(respath, files, gc, deps, extList)) {
                } else {
                    return false;
                }
            }
        }

        // Error-code version of `++`
        rditer.increment(ec);
        if (ec) {
            // If iteration failed, allow compilation to continue, but bail out of this directory.
            error("Could not iterate on '%s': %s\n", SC_Codecvt::path_to_utf8_str(path).c_str(), ec.message().c_str());
            return true;
        }
    }
    return true;
}

// Pass one build the class dependancy tree.
bool declareDependancyTreeLoadFiles(ClassLibraryFileMap& files, ClassDependencyList& deps,
                                    std::vector<ClassExtentionFile>& extList, PyrGC* gc) {
    return gLanguageConfig->forEachIncludedDirectory(
        [&, compiled_dirs = std::set<fs::path> {}](const fs::path& p) mutable {
            return passOne_ProcessDir(p, compiled_dirs, files, gc, deps, extList);
        });
    ;
}

/// True if file doesn't begin with '.', and ends with either '.sc' or '.rtf'
bool isValidSourceFileName(const fs::path& path) {
    const fs::path& ext = path.extension();
    return path.filename().c_str()[0] != '.' && // must not be hidden file
        ((ext == ".sc") || (ext == ".rtf" && path.stem().extension() == ".sc"));
}


/** \brief Attempt to parse a single SuperCollider source file
 *
 * Parsing is aborted if the file doesn't have a valid source file name,
 * or if the file can't be opened.
 * (Sekhar's replacement)
 *
 * \returns Whether parsing was successful. The only failure condition occurs
 * when the file can't be opened.
 */
bool passOne_ProcessOneFile(const fs::path& path, ClassLibraryFileMap& files, class PyrGC* gc,
                            ClassDependencyList& depList, std::vector<ClassExtentionFile>& extList) try {
    const std::string path_str = SC_Codecvt::path_to_utf8_str(path);
    const char* path_c_str = path_str.c_str();
    if (gLanguageConfig && gLanguageConfig->pathIsExcluded(path)) {
        post("\texcluding file: '%s'\n", path_c_str);
        return true;
    }
    if (!isValidSourceFileName(path))
        return true;

    PyrSymbol* fileSym = getsym(path_c_str);
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(path, std::ios::binary);

    std::stringstream ss;

    ss << file.rdbuf();

    sc::lex::NormalisedSource norm { ss.str() };
    auto pyrString =
        newPyrString(gc, static_cast<const std::string&>(norm).c_str(), obj_immutable | obj_permanent, false);

    auto textInfo = std::make_shared<TextInfo>(norm, *pyrString, *fileSym, sc::lex::FileCodeLocation {}, true);
    files.emplace(fileSym, textInfo);

    sc::lex::CodePointStream cps { textInfo->normalisedSource, textInfo->offsetInFile };
    ActionSkipWhitespace action {};

    while (declareClassForDependencyTree(textInfo, cps, action, depList, extList)) {};
    return true;

} catch (const std::exception& ex) {
    error("Could not read %s: %s.\n", SC_Codecvt::path_to_utf8_str(path).c_str(), ex.what());
    return false;
}


void schedRun();


static void runShutdown(bool wasCompiledPreviously) {
    // printf("->aboutToCompileLibrary\n");
    gLangMutex.lock();
    if (wasCompiledPreviously) {
        VMGlobals* g = gMainVMGlobals;

        g->canCallOS = true;

        ++g->sp;
        SetObject(g->sp, g->process);
        runInterpreter(g, s_shutdown, 1);

        g->canCallOS = false;
    }
    gLangMutex.unlock();
    // printf("<-aboutToCompileLibrary\n");
}

void closeAllGUIScreens();
void TempoClock_stopAll(void);
void closeAllCustomPorts();

void shutdownLibrary(bool wasCompiledPreviously) {
    gClassLibraryInfo.markShuttingDown();
    closeAllGUIScreens();

    schedStop();

    runShutdown(wasCompiledPreviously);

    TempoClock_stopAll();

    {
        auto lock = std::lock_guard<std::timed_mutex> { gLangMutex };
        closeAllCustomPorts();

        if (wasCompiledPreviously) {
            VMGlobals* g = gMainVMGlobals;
            g->canCallOS = true;
            g->gc->RunAllFinalizers();
            g->canCallOS = false;
        }

        pyr_pool_runtime->FreeAll();
    }

    gClassLibraryInfo.reset();
    deinitPrimitives();
}

SCLANG_DLLEXPORT_C bool compileLibrary(bool wasCompiledPreviously, bool standalone) try {
    // ensure buffer is flushed
    struct FlushOnExit {
        ~FlushOnExit() { flushPostBuf(); }
    } flush;

    shutdownLibrary(wasCompiledPreviously);

    auto lock = std::lock_guard<std::timed_mutex> { gLangMutex };

    // TODO: this should just use chrono directly.
    const auto startTime = elapsedTime();

    if (!gLanguageConfig)
        SC_LanguageConfig::readLibraryConfig(standalone);


    pyr_pool_runtime->FreeAllInternal();

    void* ptr = pyr_pool_runtime->Alloc(sizeof(SymbolTable));
    if (!ptr)
        throw FatalInterpreterError { "Out of memory" };

    gMainVMGlobals->symbolTable = new (ptr) SymbolTable(pyr_pool_runtime, 65536);

#ifdef NDEBUG
    postfl("Compiling class library...\n");
#else
    postfl("Compiling class library (debug build)...\n");
#endif

    gClassLibraryInfo.markCompilationInProgress();

    initSymbols();
    initSpecialSelectors();
    initSpecialClasses();
    initClasses();
    initPrimitives();

    ClassLibraryFileMap files;
    ClassDependencyList deps;
    std::vector<ClassExtentionFile> extList;

    std::size_t numFilesCompiled { 0 };

    declareDependancyTreeLoadFiles(files, deps, extList, nullptr);

    deps.linkReturnUndefinedSuperClasses();

    auto [topo, disconnected] = deps.getTopologicalOrdering();

    for (const std::vector<ClassDependency>& dis : disconnected) {
        if (dis.size() == 1) { // A chain of one, either inherits from self, or from an undefined class
            if (dis[0].superClassName == dis[0].className) {
                const auto h = dis[0].textInfo->createDiagnosticHighlight(dis[0].rangeOfSuperClass.value(),
                                                                          "Classes cannot inherit from themselves");
                const auto str = diagnosticToCompilerError(ErrorType::Error, "Self inheritance", &h, 1);
                ::postText(str.c_str(), str.size());
            } else {
                const auto h = dis[0].textInfo->createDiagnosticHighlight(dis[0].rangeOfSuperClass.value(),
                                                                          "This superclass does not exist.");
                const auto str = diagnosticToCompilerError(ErrorType::Error, "Undefined classes", &h, 1);
                ::postText(str.c_str(), str.size());
            }
        } else {
            // Long chain
            std::vector<DiagnosticHighlight> diags;
            const auto disSz = dis.size();
            for (std::size_t i { 0 }; i < disSz; ++i) {
                if (i == 0)
                    diags.push_back(dis[i].textInfo->createDiagnosticHighlight(dis[i].rangeOfClassName,
                                                                               "Inheritance loop started here..."));
                else if (i + 1 == disSz)
                    diags.push_back(dis[i].textInfo->createDiagnosticHighlight(dis[i].rangeOfClassName,
                                                                               "...inheritance loop ended here."));
                else
                    diags.push_back(dis[i].textInfo->createDiagnosticHighlight(dis[i].rangeOfClassName, "..."));
            }

            const auto str = diagnosticToCompilerError(ErrorType::Error, "Inheritance loop", &diags[0], diags.size());
            ::postText(str.c_str(), str.size());
        }
    }

    std::vector<ClassDependency> classesToRemove;
    for (const auto& d : topo)
        if (!compile(d))
            classesToRemove.push_back(d);

    for (const auto& d : extList) {
        compile(d);
    }

    if (!classesToRemove.empty())
        throw std::runtime_error { "There are classes that need removing, this is currently a compilation error but "
                                   "one day will only post a warning." };


    buildClassTree();
    gNumClasses = indexClassTree(class_abstract_object, 0);
    const auto numSelectors = setSelectorFlags();

    buildBigMethodMatrix(numSelectors);

    SymbolTable* symbolTable = gMainVMGlobals->symbolTable;
    post("\tNumber of Symbols %d\n", symbolTable->NumItems());
    const auto elapsed = elapsedTime() - startTime;
    post("\tcompiled in %.2f seconds\n", elapsed);
    const auto numOverwrites = gClassLibraryInfo.methodOverrideCount();
    if (numOverwrites == 1) {
        post("\nInfo: One method is currently overwritten by an extension. To see which, "
             "execute:\nMethodOverride.printAll\n\n");
    } else if (numOverwrites > 1) {
        post("\nInfo: %i methods are currently overwritten by extensions. To see which, "
             "execute:\nMethodOverride.printAll\n\n",
             numOverwrites);
    }

    pyr_pool_compile->FreeAll();

    PyrClass* class_main = s_main->u.classobj;
    if (!class_main)
        throw std::runtime_error { "Class 'Main' was not defined" };


    if (!isSubclassOf(class_main, class_process)) {
        throw std::runtime_error { "Class 'Main' is not a subclass of 'Process'" };
    }

    post("Compile done.\n");
    gClassLibraryInfo.markCompilationOkay();

    post("Initalising runtime.\n");

    initRuntime(gMainVMGlobals, 128 * 1024, pyr_pool_runtime);

    post("Executing 'Process.startup'...\n");
    VMGlobals* g = gMainVMGlobals;
    g->canCallOS = true;
    ++g->sp;
    SetObject(g->sp, g->process);

    // This will post the main greating.
    runInterpreter(g, s_startup, 1);
    g->canCallOS = false;
    schedRun();

    gClassLibraryInfo.markLibraryInitalised();

    return true;
} catch (const FatalInterpreterError& e) {
    gClassLibraryInfo.markCompilationFailed();
    throw;
} catch (const std::exception& e) {
    error("Exception thrown while booting: %s\n", e.what());
    flushPostBuf();
    gClassLibraryInfo.markCompilationFailed();
    return false;
} catch (...) {
    error("Unexpected throw object");
    gClassLibraryInfo.markCompilationFailed();
    return false;
}

void dumpByteCodes(PyrBlock* theBlock);

SCLANG_DLLEXPORT_C void runLibrary(PyrSymbol* selector) {
    VMGlobals* g = gMainVMGlobals;
    g->canCallOS = true;
    try {
        if (gClassLibraryInfo.acceptsInput()) {
            ++g->sp;
            SetObject(g->sp, g->process);
            runInterpreter(g, selector, 1);
        } else {
            postfl("Library has not been compiled successfully.\n");
        }
    } catch (const FatalInterpreterError& er) {
        error("A fatal interpreter error has occured. Reason: %s\n", er.what());
        throw;
    } catch (const std::exception& ex) {
        PyrMethod* meth = g->method;
        if (meth) {
            int ip = slotRawInt8Array(&meth->code) ? g->ip - slotRawInt8Array(&meth->code)->b : -1;
            post("caught exception in runLibrary %s:%s %3d\n",
                 slotRawSymbol(&slotRawClass(&meth->ownerclass)->name)->name, slotRawSymbol(&meth->name)->name, ip);
            dumpByteCodes(meth);
        } else {
            post("caught exception in runLibrary\n");
        }
        error(ex.what());
    } catch (...) { postfl("DANGER: OUT of MEMORY. Operation failed.\n"); }
    g->canCallOS = false;
}

SCLANG_DLLEXPORT_C void setCommandLine(const char* txt, size_t txtSize, const char* filePath, int lineNumber,
                                       int column) {
    VMGlobals* g = gMainVMGlobals;

    auto interpreter = g->process->interpreter.getPyrObjType<PyrInterpreter>();

    PyrString* strobj = newPyrStringN(g->gc, txtSize, 0, true);
    memcpy(strobj->s, txt, txtSize);
    interpreter->cmdLine = PyrSlot::make(strobj);

    interpreter->filePath = filePath ? PyrSlot::make(getsym(filePath)) : PyrSlot {};
    interpreter->lineNumber = PyrSlot::make(lineNumber);
    interpreter->column = PyrSlot::make(column);

    // we know strobj is white so we can use GCWriteNew
    g->gc->GCWriteNew(interpreter, strobj);
}

CompilerContext::CompilerContext(std::shared_ptr<TextInfo> t, sc::lex::FileCodeLocation fileLoc,
                                 sc::lex::SourceCodeLocation loc, struct VMGlobals* vm):
    textInfo(std::move(t)),
    vm_globals(vm) {
    parseNodePool.Init(pyr_pool_compile, 32000, 32000, 2000);
    assert(bison_cxt == nullptr);
    bison_cxt = this;
    assert(!gParserState);
    initLongStack(&generatorStack);
    parseNodePool.FreeAll();
    gParserState =
        ParserState { textInfo, BisonLexerAction { textInfo },
                      sc::lex::CodePointStream { textInfo->normalisedSource, fileLoc, loc },
                      textInfo->isClassFile ? ParserState::Mode::ClassLibrary : ParserState::Mode::CMDInitial };
}

CompilerContext::CompilerContext(std::shared_ptr<TextInfo> t, sc::lex::FileCodeLocation fileLoc,
                                 sc::lex::SourceCodeRange range, struct VMGlobals* vm):
    textInfo(std::move(t)),
    vm_globals(vm) {
    parseNodePool.Init(pyr_pool_compile, 32000, 32000, 2000);
    assert(bison_cxt == nullptr);
    bison_cxt = this;
    assert(!gParserState);
    initLongStack(&generatorStack);
    parseNodePool.FreeAll();
    gParserState =
        ParserState { textInfo, BisonLexerAction { textInfo },
                      sc::lex::CodePointStream { textInfo->normalisedSource, fileLoc, range },
                      textInfo->isClassFile ? ParserState::Mode::ClassLibrary : ParserState::Mode::CMDInitial };
}

CompilerContext::CompilerContext(std::shared_ptr<TextInfo> t, struct VMGlobals* vm):
    textInfo(std::move(t)),
    vm_globals(vm) {
    parseNodePool.Init(pyr_pool_compile, 32000, 32000, 2000);
    assert(bison_cxt == nullptr);
    bison_cxt = this;
    assert(!gParserState);
    initLongStack(&generatorStack);
    parseNodePool.FreeAll();

    gParserState =
        ParserState { textInfo, BisonLexerAction { textInfo }, sc::lex::CodePointStream { textInfo->normalisedSource },
                      textInfo->isClassFile ? ParserState::Mode::ClassLibrary : ParserState::Mode::CMDInitial };
}

CompilerContext::~CompilerContext() {
    gParserState.reset();
    parseNodePool.FreeAll();
    freeLongStack(&generatorStack);
    assert(bison_cxt == this);
    bison_cxt = nullptr;
}
