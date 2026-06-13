#pragma once
#include <limits>
#include <type_traits>

namespace sc::lex {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TokenType
// An enum covering all the types of things the lexer sees.
// These are defined in range, DO NOT CHANGE THE RANGE DELIMITERS, just insert new cases inbetween them.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// clang-format off
enum struct TokenType : unsigned int {
    EndOfFile = 0,

    START_OF_IDENTIFIERS,
        Name = START_OF_IDENTIFIERS,
        ClassName,
        PrimitiveName,
    END_OF_IDENTIFIERS,

    START_OF_LITERALS = END_OF_IDENTIFIERS,
        START_OF_LITERAL_INTS = START_OF_LITERALS,
            Integer = START_OF_LITERAL_INTS,
            IntegerRadix,
            Hexidecimal,
        END_OF_LITERAL_INTS,

        START_OF_LITERAL_FLOATS = END_OF_LITERAL_INTS,
            Float = START_OF_LITERAL_FLOATS,
            FloatRadix,
            FloatExponent,
            Pi,
            Inf,
        END_OF_LITERAL_FLOATS,

        START_OF_LITERAL_ACCIDENTALS = END_OF_LITERAL_FLOATS,
            AccidentalSteps = START_OF_LITERAL_ACCIDENTALS,
            AccidentalCents,
        END_OF_LITERAL_ACCIDENTALS,

        START_OF_LITERAL_SYMBOLS = END_OF_LITERAL_ACCIDENTALS,
            SymbolSlash = START_OF_LITERAL_SYMBOLS, //   \symbol
            SymbolQuote, // 'symbol literal'
        END_OF_LITERAL_SYMBOLS,

        START_OF_LITERAL_ASCII = END_OF_LITERAL_SYMBOLS,
            Ascii = START_OF_LITERAL_ASCII, // character, $a, $B
        END_OF_LITERAL_ASCII,

        START_OF_LITERAL_BOOLEAN = END_OF_LITERAL_ASCII,
            True = START_OF_LITERAL_BOOLEAN,
            False,
        END_OF_LITERAL_BOOLEAN,

        START_OF_LITERAL_NIL = END_OF_LITERAL_BOOLEAN,
            Nil = START_OF_LITERAL_NIL,
        END_OF_LITERAL_NIL,

        START_OF_LITERAL_STRING = END_OF_LITERAL_NIL,
            StringLine = START_OF_LITERAL_STRING,
        END_OF_LITERAL_STRING,
    END_OF_LITERALS,

    START_OF_KEYWORDS = END_OF_LITERALS,
        While = START_OF_KEYWORDS,
        Var,
        Arg,
        ClassVar,
        Const,
    END_OF_KEYWORDS,

    START_OF_BRACKETS = END_OF_KEYWORDS,
        START_OF_OPEN_BRACKETS = START_OF_BRACKETS,
            OpenParen = START_OF_OPEN_BRACKETS,
            OpenSquare,
            OpenCurly,
            BeginClosedFunction,
        END_OF_OPEN_BRACKETS,
        START_OF_CLOSE_BRACKETS = END_OF_OPEN_BRACKETS,
            CloseParen = START_OF_CLOSE_BRACKETS,
            CloseSquare,
            CloseCurly,
        END_OF_CLOSE_BRACKETS,
    END_OF_BRACKETS = END_OF_CLOSE_BRACKETS,

    // Punctuation that cannot be mistaken for an operator
    START_OF_KNOWN_PUNCTUATION = END_OF_BRACKETS,
        SemiColon = START_OF_KNOWN_PUNCTUATION,
        Colon,
        Comma,
        EqualsSign,
        NonLocalReturn,
        BackTick,
        Tilde,
        Hash,
        LeftArrow ,
        Ellipsis,
        Dot,
        DotDot,
        CurryArg,
    END_OF_KNOWN_PUNCTUATION,

    START_OF_AMBIGUOUS_PUNCTUATION = END_OF_KNOWN_PUNCTUATION,
        Pipe = START_OF_AMBIGUOUS_PUNCTUATION,
        ReadWriteVar,
        Minus,
        Multiply,
        Add,
        LessThan,
        GreaterThan,
    END_OF_AMBIGUOUS_PUNCTUATION,

    START_OF_OPERATORS = END_OF_AMBIGUOUS_PUNCTUATION,
        BinaryOperator = START_OF_OPERATORS,
        KeywordBinaryOperator,
    END_OF_OPERATORS,


    START_OF_WHITESPACE = END_OF_OPERATORS,
        Space = START_OF_WHITESPACE,
        NewLine,
        Tab,
    END_OF_WHITESPACE,

    START_OF_COMMENTS = END_OF_WHITESPACE,
        Comment = START_OF_COMMENTS,
        MultilineComment,
    END_OF_COMMENTS,

    // errors
    START_OF_ERRORS = END_OF_COMMENTS,
        ErUnexpected = START_OF_ERRORS,
        ErSymbolQuoteUnclosed,
        ErMultilineCommentUnclosed,
        ErStringUnclosed,
        ErMissingExponent,
        ErInvalidToken,
        ErInvalidUTF8,
        ErUnknown,

        START_OF_USER_DEFINED_ERRORS,
            // Left empty so parsers can store their own error codes here.
        END_OF_USER_DEFINED_ERRORS = std::numeric_limits<int>::max(),
    END_OF_ERRORS = END_OF_USER_DEFINED_ERRORS,
};
// clang-format on

template <typename... ARGS> [[nodiscard]] constexpr inline bool matches(TokenType t, ARGS... args) {
    static_assert((std::is_same_v<TokenType, ARGS> && ...));
    return ((t == args) || ...);
}

[[nodiscard]] constexpr inline bool is_error(TokenType t) {
    return TokenType::START_OF_ERRORS <= t && t < TokenType::END_OF_ERRORS;
}

[[nodiscard]] constexpr inline bool is_literal(TokenType t) {
    return TokenType::START_OF_LITERALS <= t && t < TokenType::END_OF_LITERALS;
}

[[nodiscard]] constexpr inline bool is_whitespace(TokenType t) {
    return TokenType::START_OF_WHITESPACE <= t && t < TokenType::END_OF_WHITESPACE;
}

[[nodiscard]] constexpr inline bool is_comment(TokenType t) {
    return TokenType::START_OF_COMMENTS <= t && t < TokenType::END_OF_COMMENTS;
}

[[nodiscard]] constexpr inline bool is_identifier(TokenType t) {
    return TokenType::START_OF_IDENTIFIERS <= t && t < TokenType::END_OF_IDENTIFIERS;
}

[[nodiscard]] constexpr inline bool is_keyword(TokenType t) {
    return TokenType::START_OF_KEYWORDS <= t && t < TokenType::END_OF_KEYWORDS;
}

[[nodiscard]] constexpr inline bool is_integer(TokenType t) {
    return TokenType::START_OF_LITERAL_INTS <= t && t < TokenType::END_OF_LITERAL_INTS;
}

[[nodiscard]] constexpr inline bool is_ambiguous_punctuation(TokenType t) {
    return TokenType::START_OF_AMBIGUOUS_PUNCTUATION <= t && t < TokenType::END_OF_AMBIGUOUS_PUNCTUATION;
}

// Also includes constants pi and inf
[[nodiscard]] constexpr inline bool is_float(TokenType t) {
    return TokenType::START_OF_LITERAL_FLOATS <= t && t < TokenType::END_OF_LITERAL_FLOATS;
}

[[nodiscard]] constexpr inline bool is_accidental(TokenType t) {
    return TokenType::START_OF_LITERAL_ACCIDENTALS <= t && t < TokenType::END_OF_LITERAL_ACCIDENTALS;
}

[[nodiscard]] constexpr inline bool is_symbol(TokenType t) {
    return TokenType::START_OF_LITERAL_SYMBOLS <= t && t < TokenType::END_OF_LITERAL_SYMBOLS;
}

[[nodiscard]] constexpr inline bool is_boolean(TokenType t) {
    return TokenType::START_OF_LITERAL_BOOLEAN <= t && t < TokenType::END_OF_LITERAL_BOOLEAN;
}

[[nodiscard]] constexpr inline bool is_bracket(TokenType t) {
    return TokenType::START_OF_BRACKETS <= t && t < TokenType::END_OF_BRACKETS;
}

[[nodiscard]] constexpr inline bool is_open_bracket(TokenType t) {
    return TokenType::START_OF_OPEN_BRACKETS <= t && t < TokenType::END_OF_OPEN_BRACKETS;
}

[[nodiscard]] constexpr inline bool is_close_bracket(TokenType t) {
    return TokenType::START_OF_CLOSE_BRACKETS <= t && t < TokenType::END_OF_CLOSE_BRACKETS;
}


// Will return "Unknown token" if t is not known.
[[nodiscard]] const char* to_string(TokenType t) noexcept;
}
