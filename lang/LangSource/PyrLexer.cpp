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

#include "SCBase.h"
#include "PyrLexer.h"
#include "PyrSlot.h"
#include "PyrSymbol.h"
#include "SC_Constants.h"
#include "SC_LanguageClient.h"
#include "VMGlobals.h"
#include "codepoint.hpp"
#include "source_utils.hpp"
#include "tokens.hpp"

#include <algorithm>
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

#ifdef _WIN32
#    include <direct.h>
#else
#    include <sys/param.h>
#endif

#include <filesystem>
#include <fstream>
#include <string.h>

#include "PyrParseNode.h"
#include "Bison/lang11d_tab.h"
#include "SCBase.h"
#include "PyrObject.h"
#include "PyrObjectProto.h"
#include "PyrSched.h"
#include "GC.h"
#include "SimpleStack.h"

#include "PyrSymbolTable.h"
#include "PyrInterpreter.h"
#include "PyrObjectProto.h"
#include "PyrPrimitiveProto.h"
#include "PyrKernelProto.h"
#include "InitAlloc.h"
#include "PredefinedSymbols.h"
#include "SC_LanguageConfig.hpp"

#include "SC_Filesystem.hpp" // getDirectory, resolveIfAlias, isStandalone
#include "SC_Codecvt.hpp" // path_to_utf8_str
#include "SC_TextUtils.hpp"


#include <lexer.hpp>

struct ClassExtFile {
    struct ClassExtFile* next;
    PyrSymbol* fileSym;
    int startPos, endPos, lineOffset;
};

typedef struct classdep {
    struct classdep* next;
    struct classdep* superClassDep;
    struct classdep* subclasses;
    PyrSymbol* className;
    PyrSymbol* superClassName;
    PyrSymbol* fileSym;
    int startPos, endPos, lineOffset;
} ClassDependancy;

int yyparse();
PyrSlot process_accidental_cents(const char* s);
PyrSlot process_accidental_steps(const char* s);

double compileStartTime;
int gNumCompiledFiles { 0 };

extern PyrClass* gClassList;
ClassDependancy** gClassCompileOrder;
int gClassCompileOrderNum { 0 };
int gClassCompileOrderSize = 1000;

namespace fs = std::filesystem;
using DirName = SC_Filesystem::DirName;

PyrSymbol* gCompilingFileSym = nullptr;
VMGlobals* gCompilingVMGlobals = nullptr;
static fs::path gCompileDir;


bool gShowWarnings = false;
LongStack generatorStack;
int lastClosedFuncCharNo = 0;

fs::path currfilename;
std::string printingCurrfilename; // for error reporting

bool gCompilingCmdLine = false;

// TODO: replace with yylval
intptr_t zzval;

// TODO: replace with yyloc
int lineno, charno, linepos;
int* linestarts;
int maxlinestarts { 0 };

// This is the text of the source file currently being tokenized.
char* gCompilingText { nullptr };
int gCompilinTextLen { 0 };
int textpos { 0 };
// I don't know what these do.
int errLineOffset, errCharPosOffset;
int gParseFailed = 0;
bool gCompiledOK = false;
std::set<fs::path> compiledDirectories;


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

bool startLexer(PyrSymbol* fileSym, const fs::path& p, int startPos, int endPos, int lineOffset);
void startLexerCmdLine(char* textbuf, int textbuflen);
void finiLexer();

void initLexer() {
    // strcpy(binopchars, "!@%&*-+=|:<>?/");
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
    [[nodiscard]] BisonSemActionOutput(ExtendedErrors e, lex::SourceCodeRange range):
        type(static_cast<TokenType>(e)),
        range(range),
        slot({}) {};

    [[nodiscard]] BisonSemActionOutput(ExtendedErrors e, lex::SourceCodeRange range, lex::SourceCodeRange extra_range):
        type(static_cast<TokenType>(e)),
        range(range),
        slot({}),
        extra_range_of_error(extra_range) {};

    [[nodiscard]] BisonSemActionOutput(TokenType t, lex::SourceCodeRange range, std::optional<PyrSlot> slot = {}):
        type(t),
        range(range),
        slot(slot) {};

    [[nodiscard]] BisonSemActionOutput(TokenType t, lex::SourceCodeRange range, PyrSlot slot):
        type(t),
        range(range),
        slot(slot) {};

    [[nodiscard]] BisonSemActionOutput() = default;
    [[nodiscard]] BisonSemActionOutput(BisonSemActionOutput&&) noexcept = default;
    [[nodiscard]] BisonSemActionOutput(const BisonSemActionOutput&) noexcept = default;
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
    BisonLexerAction(const char* source): source(source) {};
    BisonLexerAction() = delete;
    BisonLexerAction(BisonLexerAction&&) noexcept = default;
    BisonLexerAction(const BisonLexerAction&) = default;
    BisonLexerAction& operator=(BisonLexerAction&&) noexcept = default;
    BisonLexerAction& operator=(const BisonLexerAction&) = default;

    const char* source;
    std::vector<std::pair<TokenType, lex::SourceCodeRange>> closing_bracket_stack {};

    // TODO: once parser/compiler has been updated, remove this.
    std::vector<std::size_t> line_starts {};

    using Output = BisonSemActionOutput;

    template <TokenType T> std::optional<Output> process(lex::SourceCodeRange loc) {
        // TODO: once parser/compiler has been updated, remove this.
        while (line_starts.size() <= loc.begin.lineNumber) {
            line_starts.push_back(loc.begin.absolute);
        }
        while (line_starts.size() < loc.end.lineNumber) {
            line_starts.push_back(loc.begin.absolute);
        }
        if (line_starts.size() <= loc.end.lineNumber) {
            line_starts.push_back(loc.end.absolute);
        }

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
                    lastClosedFuncCharNo = closing_bracket_stack.back().second.begin.absolute;
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
            const char* start = source + loc.begin.absolute;
            // Looking for radix.
            const char* it = start;
            while (*it != 'r') // Potentially unsafe, but the lexer guarenteed this was found.
                ++it;
            const int radix = str_to_int(start, it - start, 10);
            ++it; // drop r
            if constexpr (T == TokenType::IntegerRadix) {
                const auto slot_value = sc_strtoi(it, (source + loc.end.absolute) - it, radix);
                return { { T, loc, PyrSlot::make(slot_value) } };
            } else {
                const double slot_value = sc_strtof(it, (source + loc.end.absolute) - it, radix);
                return { { T, loc, PyrSlot::make(slot_value) } };
            }
        }

        else if constexpr (T == TokenType::Integer)
            return { { T, loc, PyrSlot::make(atoi(fill_temp_buf(loc))) } };

        else if constexpr (T == TokenType::Hexidecimal) {
            const char* c = source + loc.begin.absolute;
            const char* const end = source + loc.end.absolute;
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
            assert(source[loc.begin.absolute] == '$');
            if (loc.size() == 2) {
                const char out = source[loc.begin.absolute + 1];
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
            assert(source[loc.begin.absolute] == '$');
            assert(source[loc.begin.absolute + 1] == '\\');
            // Three only occurs for the following escape characters.
            char out = source[loc.begin.absolute + 2];
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
        temp_buffer.clear();
        temp_buffer.insert(temp_buffer.begin(), source + loc.begin.absolute, source + loc.end.absolute);
        return temp_buffer.c_str();
    }

    PyrSymbol* text_to_symbol(lex::SourceCodeRange loc, size_t drop_from_start = 0, size_t drop_from_end = 0,
                              bool needs_escaping = false) {
        temp_buffer.clear();
        auto start = source + loc.begin.absolute + drop_from_start;
        const auto end = source + loc.end.absolute - drop_from_end;
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


void print_error_line(const char* filepath, const char* txt, size_t txt_len, sc::lex::SourceCodeRange r,
                      const char* short_description = nullptr) {
    const auto short_desc_len = short_description == nullptr ? 0 : strlen(short_description);
    std::stringstream ss;
    ss << "Error: ";

    if (filepath)
        ss << filepath << ':' << r.begin.lineNumber + 1 << ':' << r.begin.column + 1;

    ss << "\n──────────────────────────────────────────────────────────────────────────────────\n";
    const auto selection_start = txt + r.begin.absolute;
    const auto selection_end = txt + r.end.absolute;

    auto maybe_iter = sc::lex::utils::LineIter::make(txt, txt + txt_len, r.begin);
    if (!maybe_iter.has_value())
        return;
    auto iter = std::move(*maybe_iter);

    iter.backwards(); // go to start of line.
    if (r.line_count() <= 4) {
        iter.backwards();
        iter.backwards();
    }

    const auto single_line_error = r.line_count() == 1;


    for (auto line = iter.forwards(); line; line = iter.forwards()) {
        const auto [ptr, sz, line_number, ends_in_newline_char] = *line;

        if (line_number < r.begin.lineNumber || line_number > r.end.lineNumber) {
            ss << std::setfill(' ') << std::setw(5) << line_number + 1 << " │ ";
            ss.write(ptr, sz);
            if (!ends_in_newline_char)
                ss << '\n';
            if (line_number > r.end.lineNumber + 2)
                break;
            else
                continue;
        }

        if (single_line_error) {
            ss << std::setfill(' ') << std::setw(5) << line_number + 1 << " │ ";
            ss.write(ptr, sz);

            if (!ends_in_newline_char)
                ss << '\n';

            ss << "      ┆ ";
            auto cp_iter = sc::lex::utils::CodePointIterator::make(ptr, ptr + sz, ptr).value();

            for (auto cp = cp_iter.forwards(); cp; cp = cp_iter.forwards()) {
                if (cp_iter.current_location() <= selection_start) {
                    ss << sc::lex::codepoint_as_whitespace(*cp);
                } else if (cp_iter.current_location() <= selection_end) {
                    const auto w { sc::lex::codepoint_width(*cp) };
                    for (size_t i { 0 }; i < w; ++i)
                        ss << '^';
                } else
                    break;
            }

            if (short_description) {
                ss << " ";
                ss.write(short_description, short_desc_len);
                ss << '\n';
            }
        } else {
            ss << std::setfill(' ') << std::setw(5) << line_number + 1 << "*│ ";
            ss.write(ptr, sz);

            if (!ends_in_newline_char)
                ss << '\n';

            if (line_number == r.end.lineNumber) {
                if (short_description) {
                    ss << "     >│ "
                       << "^ ";
                    ss.write(short_description, short_desc_len);
                    ss << '\n';
                }
            }
        }
    }

    ss << "\n──────────────────────────────────────────────────────────────────────────────────\n";
    const auto str = ss.str();

    ::postText(str.c_str(), str.size());
}

struct GlobalBisonLexerState {
    enum struct Mode { CMDInitial, CMDContinuing, ClassLibrary };
    GlobalBisonLexerState(Mode m, BisonLexerAction a, lex::CodePointStream s):
        mode(m),
        action(std::move(a)),
        char_stream(std::move(s)) {}

    Mode mode;

    BisonLexerAction action;
    lex::CodePointStream char_stream;
    std::optional<BisonLexerAction::Output> cached {};

    int mutate_global_state_for_return(const BisonLexerAction::Output& o) {
        // If you set this to 0 when not in use, the parse will segfault.
        // TODO: use yylval.
        if (o.slot && !o.is_error())
            zzval = (intptr_t)newPyrSlotNode(o.range, *o.slot);

        yylloc = o.range;

        // Yes it reads from the end point only. Very odd. Causes many issues.
        textpos = o.range.end.absolute;
        lineno = o.range.end.lineNumber + 1; // zero indexed to 1
        linepos = action.line_starts[o.range.end.lineNumber];
        charno = o.range.end.column;
        if (maxlinestarts < o.range.end.lineNumber) {
            maxlinestarts += maxlinestarts;
            linestarts = (int*)pyr_pool_compile->Realloc(linestarts, maxlinestarts * sizeof(int*));
        }
        linestarts[lineno] = linepos;

        if (o.is_error()) {
            zzval = 0; // stop anything from continuing.

            post("\nLexing "
                 "Error:\n──────────────────────────────────────────────────────────────────────────────────\n");
            if (o.is(ExtendedErrors::GotCurlyExpectedParen) || o.is(ExtendedErrors::GotSquareExpectedParen)) {
                if (o.extra_range_of_error) {
                    print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length,
                                     *o.extra_range_of_error, "Parenthises opened here...");
                    print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length, o.range,
                                     "...was expected to be closed here with a ')'.");
                }
            } else if (o.is(ExtendedErrors::GotCurlyExpectedSquare) || o.is(ExtendedErrors::GotParenExpectedSquare)) {
                if (o.extra_range_of_error) {
                    print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length,
                                     *o.extra_range_of_error, "Square bracket opened here...");
                    print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length, o.range,
                                     "...was expected to be closed here with a ']'.");
                }
            } else if (o.is(ExtendedErrors::GotParenExpectedCurly) || o.is(ExtendedErrors::GotSquareExpectedCurly)) {
                if (o.extra_range_of_error) {
                    print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length,
                                     *o.extra_range_of_error, "Curly bracket opened here...");
                    print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length, o.range,
                                     "...was expected to be closed here with a '}'.");
                }
            } else if (o.is(ExtendedErrors::ExtraClosingCurlyBracket)) {
                print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length, o.range,
                                 "Unexpected closing curly brace, could not find a matching opening one.");
            } else if (o.is(ExtendedErrors::ExtraClosingParenBracket)) {
                print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length, o.range,
                                 "Unexpected closing parenthesis, could not find a matching opening one.");
            } else if (o.is(ExtendedErrors::ExtraClosingSqaureBracket)) {
                print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length, o.range,
                                 "Unexpected closing square bracket, could not find a matching opening one.");
            } else if (o.is(TokenType::ErMissingExponent)) {
                const auto [ptr, sz] = char_stream.source_code_range_to_text(o.range);
                const std::string example { ptr, sz };
                const auto desc = std::string { "Expected digits after the 'e', for example '" } + example + "10'.";
                print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length, o.range,
                                 desc.c_str());
            }

            else if (o.is(TokenType::ErSymbolQuoteUnclosed)) {
                const auto [ptr, sz] = char_stream.source_code_range_to_text(o.range);
                size_t i { 0 };
                while (i < sz && ptr[i] != ' ' && ptr[i] != '\n')
                    ++i;
                // TODO: we could look forward to see if the next token (discarding whitespace) is a '\'', in that case,
                // the user has a new line character in the wrong place.
                const std::string example { ptr, i };
                const auto desc =
                    std::string { "This quoted symbol does not have a matching closing quote, perhaps you meant "
                                  + example + "'?" };
                print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length, o.range,
                                 desc.c_str());
            }

            else if (o.is(TokenType::ErInvalidUTF8)) {
                print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length, o.range,
                                 "Invalid UTF8 encountered here, you probably want to delete this.");
            } else if (o.is(TokenType::ErInvalidToken)) {
                print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length, o.range,
                                 "Invalid token encountered, supercollider does not know how to handle this.");
            }

            else if (o.is(TokenType::ErStringUnclosed)) {
                const auto [ptr, sz] = char_stream.source_code_range_to_text(o.range);
                size_t i { 0 };
                while (i < sz && ptr[i] != '\n' && ptr[i] != ' ')
                    ++i;
                const std::string example { ptr, i };
                const auto desc =
                    std::string { "This string does not have a closing '\"', perhaps you meant " + example + "\"?" };
                print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length, o.range,
                                 desc.c_str());
            } else if (o.is(TokenType::ErMultilineCommentUnclosed)) {
                const auto desc = std::string { "This multiline comment does not have a closing */." };
                print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length, o.range,
                                 desc.c_str());
            }

            else {
                print_error_line(gCompilingFileSym->name, char_stream.source, char_stream.source_length, o.range);
            }
        }
        gParseFailed = o.is_error() ? 1 : 0;

        return *convert_to_bison_tokentype(o.type);
    }
};

std::optional<GlobalBisonLexerState> global_bison_lexer_state {};

// Must also advance through global state.
bool scanForClosingBracket(TokenType to_find) {
    assert(global_bison_lexer_state);
    GlobalBisonLexerState& s = *global_bison_lexer_state;
    const auto tok = to_find;
    assert(!s.action.closing_bracket_stack.empty());
    const auto target_depth = s.action.closing_bracket_stack.size() - 1; // we have just pushed a bracket.

    BisonLexerAction::Output out {};
    while (true) {
        out = lex::lexer(s.char_stream, s.action);

        if (out.type == TokenType::EndOfFile || out.is_error()) {
            s.mutate_global_state_for_return(out);
            return false;
        }
        if (out.type == tok && s.action.closing_bracket_stack.size() == target_depth) {
            s.mutate_global_state_for_return(out);
            return true;
        }
    }
}

void scan_for_end() {
    assert(global_bison_lexer_state);
    GlobalBisonLexerState& s = *global_bison_lexer_state;

    BisonLexerAction::Output out;
    do {
        out = lex::lexer(s.char_stream, s.action);
    } while (out.type != TokenType::EndOfFile && !out.is_error());

    s.mutate_global_state_for_return(out);
}

int yylex() {
    assert(global_bison_lexer_state);
    GlobalBisonLexerState& s = *global_bison_lexer_state;
    if (s.mode == GlobalBisonLexerState::Mode::CMDInitial) {
        s.mode = GlobalBisonLexerState::Mode::CMDContinuing;
        textpos = 0;
        lineno = 0;
        linepos = 0;
        charno = 0;
        linestarts[0] = 0;
        zzval = 0;
        gParseFailed = 0;
        return INTERPRET;
    }


    // If we have a cached out return it.
    // This is necessary for the string line bodge while we migrate, it can be remove in the future once the
    // parser & compiler know how to deal with string lines.
    if (s.cached) {
        const auto o = std::move(*s.cached);
        s.cached.reset();
        return s.mutate_global_state_for_return(o);
    }

    BisonLexerAction::Output out = lex::lexer(s.char_stream, s.action);

    if (out.type != TokenType::StringLine)
        return s.mutate_global_state_for_return(out);

    sc::lex::SourceCodeLocation start { out.range.begin };
    std::string str {};
    str.reserve(128);

    auto prev = out;
    while (true) {
        // This is nasty, but in the future, this should move into the compiler making this unnecessary.
        if (out.type != TokenType::StringLine) {
            assert(!s.cached.has_value());

            // Yes it reads from the end point only. Very odd. Causes many issues.
            textpos = prev.range.end.absolute;
            lineno = prev.range.end.lineNumber + 1; // zero indexed to 1

            linepos = s.action.line_starts[prev.range.end.lineNumber];
            charno = prev.range.end.column;
            if (maxlinestarts < prev.range.end.lineNumber) {
                maxlinestarts += maxlinestarts;
                linestarts = (int*)pyr_pool_compile->Realloc(linestarts, maxlinestarts * sizeof(int*));
            }
            linestarts[lineno] = linepos;

            // This is the one case in the whole lexer where we currently have to alloc using the GC.
            // This would be much better pushed into the compiler.
            const int flags = gCompilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
            auto sc_str = newPyrString(gMainVMGlobals->gc, str.c_str(), flags, false);
            zzval = (intptr_t)newPyrSlotNode(out.range, PyrSlot::make(sc_str));
            yylloc = out.range;
            gParseFailed = prev.is_error() ? 1 : 0;

            s.cached = std::move(out); // save for next time.
            return STRING;
        }
        auto range = out.range;
        // This is dodgy, we are dropping the quotes here.
        // Again, once this is in the compilation phase, this becomes nice.
        range.begin.absolute += 1;
        range.end.absolute -= 1;

        bool escaped = false;
        const auto [bb, sz] = s.char_stream.source_code_range_to_text(range);
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
        out = lex::lexer(s.char_stream, s.action);
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

void yyerror(const char* s) {
    gParseFailed = 1;
    error("%s\n", s);
    postErrorLine(lineno, linepos, charno);
    //  Debugger();
}

void fatal() {
    gParseFailed = 1;
    error("Parse error\n");
    postErrorLine(lineno, linepos, charno);
    //  Debugger();
}

void postErrorLine(int linenum, int start, int charpos) {
    int i, j, end, pos;
    char str[256];

    // post("start %d\n", start);
    // parseFailed = true;
    post("  in %s\n", printingCurrfilename.c_str());
    post("  line %d char %d:\n\n", linenum + errLineOffset, charpos);
    // nice: postfl previous line for context

    // postfl("text '%s' %d\n", text, text);

    // postfl error line for context
    pos = start + charpos;
    for (i = pos; i < gCompilinTextLen; ++i) {
        if (gCompilingText[i] == 0 || gCompilingText[i] == '\r' || gCompilingText[i] == '\n')
            break;
    }
    end = i;
    for (i = start, j = 0; i < end && j < 255; ++i) {
        str[j++] = gCompilingText[i];
    }
    str[j] = 0;

    i = end + 1;
    if (i < gCompilinTextLen) {
        // postfl following line for context
        for (j = 0; j < 255 && i < gCompilinTextLen; ++i) {
            if (gCompilingText[i] == 0 || gCompilingText[i] == '\r' || gCompilingText[i] == '\n')
                break;
            str[j++] = gCompilingText[i];
        }
        str[j] = 0;
        post("  %s\n", str);
    }
    post("-----------------------------------\n", str);
}

void pstrncpy(unsigned char* s1, unsigned char* s2, int n);
void pstrncpy(unsigned char* s1, unsigned char* s2, int n) {
    int i, m;
    m = *s2++;
    n = (n < m) ? n : m;
    *s1 = n;
    s1++;
    for (i = 0; i < n; ++i) {
        *s1 = *s2;
        s1++;
        s2++;
    }
}

int pstrcmp(unsigned char* s1, unsigned char* s2);
int pstrcmp(unsigned char* s1, unsigned char* s2) {
    int i, len1, len2, len;
    len1 = *s1++;
    len2 = *s2++;
    len = sc_min(len1, len2);
    for (i = 0; i < len; ++i) {
        if (s1[i] < s2[i])
            return -1;
        if (s1[i] > s2[i])
            return 1;
    }
    if (len1 < len2)
        return -1;
    if (len1 > len2)
        return 1;
    return 0;
}


int numClassDeps;
static ClassExtFile* sClassExtFiles;
static ClassExtFile* eClassExtFiles;

ClassExtFile* newClassExtFile(PyrSymbol* fileSym, int startPos, int endPos);
ClassExtFile* newClassExtFile(PyrSymbol* fileSym, int startPos, int endPos) {
    ClassExtFile* classext;
    classext = (ClassExtFile*)pyr_pool_compile->Alloc(sizeof(ClassExtFile));
    MEMFAIL(classext);
    classext->fileSym = fileSym;
    classext->next = nullptr;
    classext->startPos = startPos;
    classext->endPos = endPos;
    if (!sClassExtFiles)
        sClassExtFiles = classext;
    else
        eClassExtFiles->next = classext;
    eClassExtFiles = classext;
    return classext;
}


ClassDependancy* newClassDependancy(PyrSymbol* className, PyrSymbol* superClassName, PyrSymbol* fileSym, int startPos,
                                    int endPos, int lineOffset) {
    ClassDependancy* classdep;

    // post("classdep '%s' '%s' '%s' %d %d\n", className->name, superClassName->name,
    //	fileSym->name, className, superClassName);
    // pyrmalloc:
    // lifetime: kill after compile.
    numClassDeps++;
    if (className->classdep) {
        error("duplicate Class found: '%s' \n", className->name);
        post("%s\n", className->classdep->fileSym->name);
        postfl("%s\n\n", fileSym->name);
        return className->classdep;
    }
    classdep = (ClassDependancy*)pyr_pool_compile->Alloc(sizeof(ClassDependancy));
    MEMFAIL(classdep);
    classdep->className = className;
    classdep->superClassName = superClassName;
    classdep->fileSym = fileSym;
    classdep->superClassDep = nullptr;
    classdep->next = nullptr;
    classdep->subclasses = nullptr;

    classdep->startPos = startPos;
    classdep->endPos = endPos;
    classdep->lineOffset = lineOffset;

    className->classdep = classdep;
    return classdep;
}

void buildDepTree() {
    ClassDependancy* next;
    SymbolTable* symbolTable = gMainVMGlobals->symbolTable;

    // postfl("->buildDepTree\n"); fflush(stdout);
    for (int i = 0; i < symbolTable->TableSize(); ++i) {
        PyrSymbol* sym = symbolTable->Get(i);
        if (sym && (sym->flags & sym_Class)) {
            if (sym->classdep) {
                if (sym->classdep->superClassName->classdep) {
                    next = sym->classdep->superClassName->classdep->subclasses;
                    sym->classdep->superClassName->classdep->subclasses = sym->classdep;
                    sym->classdep->next = next;
                } else if (sym->classdep->superClassName != s_none) {
                    error("Superclass '%s' of class '%s' is not defined in any file.\n%s\n",
                          sym->classdep->superClassName->name, sym->classdep->className->name,
                          sym->classdep->fileSym->name);
                }
            }
        }
    }
    // postfl("<-buildDepTree\n"); fflush(stdout);
}


void compileDepTree();
void traverseDepTree(ClassDependancy* classdep, int level);
void compileClassExtensions();

void traverseFullDepTree() {
    // postfl("->traverseFullDepTree\n"); fflush(stdout);
    gClassCompileOrderNum = 0;
    gClassCompileOrder = (ClassDependancy**)pyr_pool_compile->Alloc(gClassCompileOrderSize * sizeof(ClassDependancy));
    MEMFAIL(gClassCompileOrder);

    // parse and compile all files
    initParser(); // sets compiler errors to 0
    gParserResult = -1;

    traverseDepTree(s_object->classdep, 0);
    compileDepTree(); // compiles backwards using the order defined in gClassCompileOrder
    compileClassExtensions();

    pyr_pool_compile->Free(gClassCompileOrder);

    finiParser();
    // postfl("<-traverseFullDepTree\n"); fflush(stdout);
}


void traverseDepTree(ClassDependancy* classdep, int level) {
    ClassDependancy* subclassdep;

    if (!classdep)
        return;

    subclassdep = classdep->subclasses;
    for (; subclassdep; subclassdep = subclassdep->next) {
        traverseDepTree(subclassdep, level + 1);
    }
    if (gClassCompileOrderNum > gClassCompileOrderSize) {
        gClassCompileOrderSize *= 2;
        gClassCompileOrder = (ClassDependancy**)pyr_pool_compile->Realloc(
            gClassCompileOrder, gClassCompileOrderSize * sizeof(ClassDependancy));
        MEMFAIL(gClassCompileOrder);
    }

    /*	postfl("traverse level:%d, gClassCompileOrderNum:%d, '%s' '%s' '%s'\n", level, gClassCompileOrderNum,
       classdep->className->name, classdep->superClassName->name, classdep->fileSym->name); fflush(stdout);
    */

    gClassCompileOrder[gClassCompileOrderNum++] = classdep;
}


void compileClass(PyrSymbol* fileSym, int startPos, int endPos, int lineOffset) {
    // fprintf(stderr, "compileClass: %d\n", fileSym->u.index);

    gCompilingFileSym = fileSym;
    gCompilingVMGlobals = nullptr;
    gRootParseNode = nullptr;
    initParserPool();
    if (startLexer(fileSym, fs::path(), startPos, endPos, lineOffset)) {
        // postfl("->Parsing %s\n", fileSym->name); fflush(stdout);
        gParseFailed = yyparse();
        // postfl("<-Parsing %s %d\n", fileSym->name, parseFailed); fflush(stdout);
        // post("parseFailed %d\n", parseFailed); fflush(stdout);
        if (!gParseFailed && gRootParseNode) {
            // postfl("Compiling nodes %p\n", gRootParseNode);fflush(stdout);
            gCompilingCmdLine = false;
            compileNodeList(gRootParseNode, true);
            // postfl("done compiling\n");fflush(stdout);
        } else {
            compileErrors++;
            fs::path pathname(fileSym->name);
            error("file '%s' parse failed\n", SC_Codecvt::path_to_utf8_str(pathname).c_str());
            postfl("error parsing\n");
        }
        finiLexer();
    } else {
        error("file '%s' open failed\n", fileSym->name);
    }
    freeParserPool();
}

void compileDepTree() {
    ClassDependancy* classdep;
    int i;

    for (i = gClassCompileOrderNum - 1; i >= 0; --i) {
        classdep = gClassCompileOrder[i];
        /*postfl("compile %d '%s' '%s' '%s'...%d/%d/%d\n", i, classdep->className->name, classdep->superClassName->name,
            classdep->fileSym->name, classdep->startLine, classdep->endLine, classDep->lineOffset);*/
        compileClass(classdep->fileSym, classdep->startPos, classdep->endPos, classdep->lineOffset);
    }
    // postfl("<compile\n");
}

void compileClassExtensions() {
    if (sClassExtFiles) {
        ClassExtFile* classext = sClassExtFiles;
        do {
            // postfl("compile class ext: %d/%d\n", classext->startPos, classext->endPos);
            compileClass(classext->fileSym, classext->startPos, classext->endPos, -1);
            classext = classext->next;
        } while (classext);
    }
}

void findDiscrepancy();

void traverseFullDepTree2() {
    // assign a class index to all classes
    if (!gParseFailed && !compileErrors) {
        buildClassTree();
        gNumClasses = 0;

        // now I index them during pass one
        indexClassTree(class_object, 0);
        setSelectorFlags();
        if (2 * numClassDeps != gNumClasses) {
            error("There is a discrepancy.\n");
            /* not always correct
                    if(2*numClassDeps < gNumClasses) {
                        post("Duplicate files may exist in the directory structure.\n");
                    } else {
                        post("Some class files may be missing.\n");
                    }
                    */
            post("numClassDeps %d   gNumClasses %d\n", numClassDeps, gNumClasses);
            findDiscrepancy();
            compileErrors++;
        } else {
            double elapsed;
            buildBigMethodMatrix();
            SymbolTable* symbolTable = gMainVMGlobals->symbolTable;
            post("\tNumber of Symbols %d\n", symbolTable->NumItems());
            post("\tByte Code Size %d\n", totalByteCodes);
            // elapsed = TickCount() - compileStartTime;
            // elapsed = 0;
            elapsed = elapsedTime() - compileStartTime;
            post("\tcompiled %d files in %.2f seconds\n", gNumCompiledFiles, elapsed);
            if (numOverwrites == 1) {
                post("\nInfo: One method is currently overwritten by an extension. To see which, "
                     "execute:\nMethodOverride.printAll\n\n");
            } else if (numOverwrites > 1) {
                post("\nInfo: %i methods are currently overwritten by extensions. To see which, "
                     "execute:\nMethodOverride.printAll\n\n",
                     numOverwrites);
            }
            post("compile done\n");
        }
    }
}

bool parseOneClass(PyrSymbol* fileSym) {
    PyrSymbol *className, *superClassName;
    ClassDependancy* classdep;
    bool res;

    int startPos, startLineOffset;

    res = true;

    startPos = textpos;
    startLineOffset = lineno - 1;

    GlobalBisonLexerState& s = *global_bison_lexer_state;
    BisonLexerAction::Output out = lex::lexer(s.char_stream, s.action);

    if (out.type == TokenType::ClassName) {
        const auto [ptr, sz] = s.char_stream.source_code_range_to_text(out.range);
        className = getsymlen(ptr, sz);

        out = lex::lexer(s.char_stream, s.action);
        if (out.type == TokenType::EndOfFile)
            return false;
        if (out.type == TokenType::OpenSquare) {
            scanForClosingBracket(TokenType::CloseSquare); // eat indexing spec
            out = lex::lexer(s.char_stream, s.action);
            if (out.type == TokenType::EndOfFile)
                return false;
        }
        if (out.type == TokenType::Colon) {
            out = lex::lexer(s.char_stream, s.action);
            if (out.type == TokenType::EndOfFile)
                return false;
            if (out.type == TokenType::ClassName) {
                const auto [ptr, sz] = s.char_stream.source_code_range_to_text(out.range);
                superClassName = getsymlen(ptr, sz);

                out = lex::lexer(s.char_stream, s.action);
                if (out.type == TokenType::EndOfFile)
                    return false;
                if (out.type == TokenType::OpenCurly) {
                    scanForClosingBracket(TokenType::CloseCurly); // eat class body
                    classdep =
                        newClassDependancy(className, superClassName, fileSym, startPos, textpos, startLineOffset);
                } else {
                    compileErrors++;
                    std::string expected = std::string { "Expected an open curly bracket '{' but received a " }
                        + sc::lex::to_string(out.type);
                    print_error_line(fileSym->name, gCompilingText, gCompilinTextLen, out.range, expected.c_str());
                    return false;
                }
            } else {
                compileErrors++;
                std::string expected =
                    std::string { "Expected a superclass name but received a " } + sc::lex::to_string(out.type);
                print_error_line(fileSym->name, gCompilingText, gCompilinTextLen, out.range, expected.c_str());
                return false;
            }
        } else if (out.type == TokenType::OpenCurly) {
            if (className == s_object)
                superClassName = s_none;
            else
                superClassName = s_object;
            scanForClosingBracket(TokenType::CloseCurly); // eat class body
            classdep = newClassDependancy(className, superClassName, fileSym, startPos, textpos, startLineOffset);
        } else {
            compileErrors++;
            std::string expected = std::string {
                "Expected an open curly bracket '{', or a colon ':' followed by a superclass, but received a "
            } + sc::lex::to_string(out.type);
            print_error_line(fileSym->name, gCompilingText, gCompilinTextLen, out.range, expected.c_str());
            return false;
        }
    } else if (out.type == TokenType::Add) {
        out = lex::lexer(s.char_stream, s.action);
        if (out.type == TokenType::EndOfFile)
            return false;

        scan_for_end();

        newClassExtFile(fileSym, startPos, textpos);
        return false;
    } else {
        if (out.type != TokenType::EndOfFile) {
            compileErrors++;

            std::string expected =
                std::string { "Expected a class name but received a " } + sc::lex::to_string(out.type);
            print_error_line(fileSym->name, gCompilingText, gCompilinTextLen, out.range, expected.c_str());
            return false;
        } else {
            res = false;
        }
    }
    return res;
}

void initPassOne() {
    // dump_pool_histo(pyr_pool_runtime);
    pyr_pool_runtime->FreeAllInternal();
    // dump_pool_histo(pyr_pool_runtime);
    // gPermanentObjPool.Init(pyr_pool_runtime, PERMOBJCHUNK);
    sClassExtFiles = nullptr;

    void* ptr = pyr_pool_runtime->Alloc(sizeof(SymbolTable));
    MEMFAIL(ptr);
    gMainVMGlobals->symbolTable = new (ptr) SymbolTable(pyr_pool_runtime, 65536);

    initSymbols(); // initialize symbol globals
    initSpecialSelectors();
    initSpecialClasses();
    initClasses();
    initParserPool();
    initParseNodes();
    initPrimitives();

    initLexer();

    compileErrors = 0;
    numClassDeps = 0;
    gCompiledOK = false;
    compiledDirectories.clear();

    // main class library folder: only used for relative path resolution
    gCompileDir = SC_Filesystem::instance().getDirectory(DirName::Resource) / "SCClassLibrary";
}

void finiPassOne() {
    // postfl("->finiPassOne\n");
    freeParserPool();
    // postfl("<-finiPassOne\n");
}

/**
 * \brief \c true if \c dir is one of the language config's default classlib directories
 */
static bool isDefaultClassLibraryDirectory(const fs::path& dir) {
    auto const& defaultDirs = gLanguageConfig->defaultClassLibraryDirectories();
    auto const iter = std::find(defaultDirs.begin(), defaultDirs.end(), dir);
    return iter != defaultDirs.end();
}

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

fs::path relativeToCompileDir(const fs::path& p) { return fs::relative(p, gCompileDir); }

bool passOne_ProcessOneFile(const fs::path& path);

/** \brief Determines whether the directory should be skipped during compilation.
 *
 * \param dir : The directory to check, as a `path` object
 * \returns `true` iff any of the following conditions is true:
 * - the directory has already been compiled
 * - the language configuration says this path is excluded
 * - SC_Filesystem::shouldNotCompileDirectory(dir) returns `true`
 */
static bool passOne_ShouldSkipDirectory(const fs::path& dir) {
    return (compiledDirectories.find(dir) != compiledDirectories.end())
        || (gLanguageConfig && gLanguageConfig->pathIsExcluded(dir))
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
static bool passOne_ProcessDir(const fs::path& dir) {
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
    } else if (passOne_ShouldSkipDirectory(expdir)) {
        // If we should skip the directory, just return success now.
        return true;
    } else {
        // Let the user know we are in fact compiling this directory.
        post("\tCompiling directory '%s'\n", SC_Codecvt::path_to_utf8_str(expdir).c_str());
    }

    // Record that we have touched this directory already.
    compiledDirectories.insert(expdir);

    // Invariant: we have processed (or begun to process) every directory or file already
    // touched by the iterator.
    while (rditer != fs::end(rditer)) {
        const fs::path path = *rditer;

        // If the file is a directory, perform the same checks as above to see if we should
        // skip compilation on it.
        if (fs::is_directory(path)) {
            if (passOne_ShouldSkipDirectory(path)) {
                rditer.disable_recursion_pending(); // don't "push" into the next level of the hierarchy
            } else {
                // Mark this directory as compiled.
                // By not calling no_push(), we allow the iterator to enter the directory
                compiledDirectories.insert(path);
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
                if (!passOne_ProcessDir(respath)) {
                    return false;
                }
            } else if (respath.empty()) {
                error("Could not resolve symlink: %s\n", SC_Codecvt::path_to_utf8_str(path).c_str());
            } else if (!passOne_ProcessOneFile(respath)) {
                return false;
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

bool passOne() {
    initPassOne();
    bool success = gLanguageConfig->forEachIncludedDirectory(passOne_ProcessDir);
    finiPassOne();

    return success;
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
bool passOne_ProcessOneFile(const fs::path& path) {
    bool success = true;

    const std::string path_str = SC_Codecvt::path_to_utf8_str(path);
    const char* path_c_str = path_str.c_str();
    if (gLanguageConfig && gLanguageConfig->pathIsExcluded(path)) {
        post("\texcluding file: '%s'\n", path_c_str);
        return success;
    }

    if (isValidSourceFileName(path)) {
        gNumCompiledFiles++;
        PyrSymbol* fileSym = getsym(path_c_str);
        fileSym->u.source = nullptr;
        if (startLexer(fileSym, path, -1, -1, -1)) {
            while (parseOneClass(fileSym)) {};
            finiLexer();
        } else {
            error("file '%s' open failed\n", path_c_str);
            success = false;
        }
    } else {
        // wasn't a valid source file; ignore
    }
    return success;
}

void schedRun();

void compileSucceeded();
void compileSucceeded() {
    gCompiledOK = !(gParseFailed || compileErrors);
    if (gCompiledOK) {
        gCompiledOK = true;

        gCompiledOK = initRuntime(gMainVMGlobals, 128 * 1024, pyr_pool_runtime);

        if (gCompiledOK) {
            VMGlobals* g = gMainVMGlobals;

            g->canCallOS = true;

            ++g->sp;
            SetObject(g->sp, g->process);
            runInterpreter(g, s_startup, 1);
            g->canCallOS = false;

            schedRun();
        }
        flushPostBuf();
    }
}

static void runShutdown() {
    // printf("->aboutToCompileLibrary\n");
    gLangMutex.lock();
    if (gCompiledOK) {
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

void shutdownLibrary() {
    closeAllGUIScreens();

    schedStop();

    runShutdown();

    TempoClock_stopAll();

    gLangMutex.lock();
    closeAllCustomPorts();

    if (gCompiledOK) {
        VMGlobals* g = gMainVMGlobals;
        g->canCallOS = true;
        g->gc->RunAllFinalizers();
        g->canCallOS = false;
    }

    pyr_pool_runtime->FreeAll();

    gCompiledOK = false;

    gLangMutex.unlock();
    deinitPrimitives();
}

SCLANG_DLLEXPORT_C bool compileLibrary(bool standalone) {
    // printf("->compileLibrary\n");
    shutdownLibrary();

    gLangMutex.lock();
    gNumCompiledFiles = 0;
    gCompiledOK = false;

    if (!gLanguageConfig) {
        SC_LanguageConfig::readLibraryConfig(standalone);
    }

    compileStartTime = elapsedTime();

    totalByteCodes = 0;

#ifdef NDEBUG
    postfl("compiling class library...\n");
#else
    postfl("compiling class library (debug build)...\n");
#endif

    bool res = passOne();
    if (res) {
        if (!compileErrors) {
            buildDepTree();
            traverseFullDepTree();
            traverseFullDepTree2();
            flushPostBuf();

            if (!compileErrors && gShowWarnings) {
                SymbolTable* symbolTable = gMainVMGlobals->symbolTable;
                symbolTable->CheckSymbols();
            }
        }
        pyr_pool_compile->FreeAll();
        flushPostBuf();
        compileSucceeded();
    } else {
        gCompiledOK = false;
    }

    gLangMutex.unlock();
    // printf("<-compileLibrary\n");
    return gCompiledOK;
}

void dumpByteCodes(PyrBlock* theBlock);

SCLANG_DLLEXPORT_C void runLibrary(PyrSymbol* selector) {
    VMGlobals* g = gMainVMGlobals;
    g->canCallOS = true;
    try {
        if (gCompiledOK) {
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

bool startLexer(PyrSymbol* fileSym, const fs::path& p, int startPos, int endPos, int lineOffset) {
    const char* filename = fileSym->name;

    gCompilinTextLen = -1;

    if (!fileSym->u.source) {
        try {
            std::ifstream file;
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            file.open(p, std::ios_base::binary);
            size_t sz = fs::file_size(p);

            gCompilingText = (char*)pyr_pool_compile->Alloc((sz + 1) * sizeof(char));
            MEMFAIL(gCompilingText);
            file.read(gCompilingText, sz);
            gCompilingText[sz] = '\0';
            fileSym->u.source = gCompilingText;
            rtf2txt(gCompilingText);
        } catch (const std::exception& ex) {
            error("Could not read %s: %s.\n", SC_Codecvt::path_to_utf8_str(p).c_str(), ex.what());
            return false;
        }
    } else
        gCompilingText = fileSym->u.source;

    if ((startPos >= 0) && (endPos > 0)) {
        gCompilinTextLen = endPos - startPos;
        gCompilingText += startPos;
    } else if (gCompilinTextLen == -1)
        gCompilinTextLen = strlen(gCompilingText);

    if (lineOffset > 0)
        errLineOffset = lineOffset;
    else
        errLineOffset = 0;

    if (startPos > 0)
        errCharPosOffset = startPos;
    else
        errCharPosOffset = 0;

    initLongStack(&generatorStack);
    lastClosedFuncCharNo = 0;
    textpos = 0;
    linepos = 0;
    lineno = 1;
    charno = 0;

    zzval = 0;
    gParseFailed = 0;
    currfilename = fs::path(filename);
    printingCurrfilename = "file '" + SC_Codecvt::path_to_utf8_str(currfilename) + "'";
    maxlinestarts = 1000;
    linestarts = (int*)pyr_pool_compile->Alloc(maxlinestarts * sizeof(int*));
    MEMFAIL(linestarts);
    linestarts[0] = 0;
    linestarts[1] = 0;
    gCompilingCmdLine = false;

    global_bison_lexer_state.emplace(
        GlobalBisonLexerState::Mode::ClassLibrary, std::move(BisonLexerAction { gCompilingText }),
        std::move(sc::lex::CodePointStream { gCompilingText, static_cast<size_t>(gCompilinTextLen), {} }));

    return true;
}

void startLexerCmdLine(char* textbuf, int textbuflen) {
    // pyrmalloc:
    // lifetime: kill after compile. (this one gets killed anyway)
    gCompilingText = (char*)pyr_pool_compile->Alloc((textbuflen + 2) * sizeof(char));
    MEMFAIL(gCompilingText);
    memcpy(gCompilingText, textbuf, textbuflen);
    gCompilingText[textbuflen] = ' ';
    gCompilingText[textbuflen + 1] = 0;
    gCompilinTextLen = textbuflen + 1;

    rtf2txt(gCompilingText);

    initLongStack(&generatorStack);
    lastClosedFuncCharNo = 0;
    textpos = 0;
    linepos = 0;
    lineno = 1;
    charno = 0;

    gCompilingCmdLine = true;
    zzval = 0;
    gParseFailed = 0;
    currfilename = fs::path("interpreted text");
    printingCurrfilename = currfilename.string();
    maxlinestarts = 1000;
    linestarts = (int*)pyr_pool_compile->Alloc(maxlinestarts * sizeof(int*));
    MEMFAIL(linestarts);
    linestarts[0] = 0;
    linestarts[1] = 0;

    errLineOffset = 0;
    errCharPosOffset = 0;

    global_bison_lexer_state.emplace(
        GlobalBisonLexerState::Mode::CMDInitial, std::move(BisonLexerAction { gCompilingText }),
        std::move(lex::CodePointStream { gCompilingText, static_cast<size_t>(gCompilinTextLen), {} }));
}

void finiLexer() {
    global_bison_lexer_state.reset();
    pyr_pool_compile->Free(linestarts);
    freeLongStack(&generatorStack);
}
