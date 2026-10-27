// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton interface for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


/**
 ** \file langutils/sc_parser/src/sc_grammar_parser.hpp
 ** Define the sc::ast::parser::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_YY_LANGUTILS_SC_PARSER_SRC_SC_GRAMMAR_PARSER_HPP_INCLUDED
# define YY_YY_LANGUTILS_SC_PARSER_SRC_SC_GRAMMAR_PARSER_HPP_INCLUDED
// "%code requires" blocks.
#line 78 "langutils/sc_parser/src/sc_grammar.y"


#include "parser_context.hpp"
#include "sc_parser/sc_grammar_shared.hpp"


#line 56 "langutils/sc_parser/src/sc_grammar_parser.hpp"


# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif



#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

#line 66 "langutils/sc_parser/src/sc_grammar.y"
namespace sc { namespace ast { namespace parser {
#line 192 "langutils/sc_parser/src/sc_grammar_parser.hpp"




  /// A Bison parser.
  class parser
  {
  public:
#ifdef YYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define YYSTYPE in C++, use %define api.value.type"
# endif
    typedef YYSTYPE value_type;
#else
    /// Symbol semantic values.
    union value_type
    {
#line 7 "langutils/sc_parser/src/sc_grammar.y"

    ASCIIIndex v_ASCIIIndex;
    AccidentalIndex v_AccidentalIndex;
    AdverbIndex v_AdverbIndex;
    AnyExprIndex v_AnyExprIndex;
    AnyMethodIndex v_AnyMethodIndex;
    AnyPossiblyLiteralIndex v_AnyPossiblyLiteralIndex;
    AnySymbolIndex v_AnySymbolIndex;
    ArgumentEntryIndex v_ArgumentEntryIndex;
    ArgumentListIndex v_ArgumentListIndex;
    ArrayIndex v_ArrayIndex;
    AtMessageIndex v_AtMessageIndex;
    BigMemberListIndex v_BigMemberListIndex;
    BlockContentsListIndex v_BlockContentsListIndex;
    BlockIndex v_BlockIndex;
    BlockItemIndex v_BlockItemIndex;
    BlockListIndex v_BlockListIndex;
    BooleanLitIndex v_BooleanLitIndex;
    ClassExtensionIndex v_ClassExtensionIndex;
    ClassIndex v_ClassIndex;
    ClassOrExtensionIndex v_ClassOrExtensionIndex;
    ClassOrExtensionListIndex v_ClassOrExtensionListIndex;
    DeclareAnyMemberListIndex v_DeclareAnyMemberListIndex;
    DeclareAnyVariableIndex v_DeclareAnyVariableIndex;
    DeclareArgumentListIndex v_DeclareArgumentListIndex;
    DeclareMemberIndex v_DeclareMemberIndex;
    DeclareMemberListIndex v_DeclareMemberListIndex;
    DeclareVariableListIndex v_DeclareVariableListIndex;
    DictionaryEntryIndex v_DictionaryEntryIndex;
    DictionaryIndex v_DictionaryIndex;
    FileAsFunctionIndex v_FileAsFunctionIndex;
    FloatLitIndex v_FloatLitIndex;
    FloatProducingIndex v_FloatProducingIndex;
    GeneratorAnyIndex v_GeneratorAnyIndex;
    GeneratorQualAnyIndex v_GeneratorQualAnyIndex;
    GeneratorQualListIndex v_GeneratorQualListIndex;
    GoIndex v_GoIndex;
    IntLitIndex v_IntLitIndex;
    LexerToken v_LexerToken;
    MethodListIndex v_MethodListIndex;
    NameListIndex v_NameListIndex;
    NamePackIndex v_NamePackIndex;
    NamedIdentifierIndex v_NamedIdentifierIndex;
    NilLitIndex v_NilLitIndex;
    RWAccessorIndex v_RWAccessorIndex;
    RangeAssignIndex v_RangeAssignIndex;
    RangeGeneratorIndex v_RangeGeneratorIndex;
    RangeIndex v_RangeIndex;
    RegionItemIndex v_RegionItemIndex;
    RegionListIndex v_RegionListIndex;
    SelectorIndex v_SelectorIndex;
    SelectorMaybeAdverbIndex v_SelectorMaybeAdverbIndex;
    StringLitIndex v_StringLitIndex;
    bool v_bool;
    maybe<ClassNameIdentifierIndex> v_maybe_ClassNameIdentifierIndex;
    maybe<NamedIdentifierIndex> v_maybe_NamedIdentifierIndex;
    value_type() {}

#line 269 "langutils/sc_parser/src/sc_grammar_parser.hpp"

    };
#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef  sc::src::SourceCodeRange  location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        TOKEN_YYEMPTY = -2,
    TOKEN_YYEOF = 0,               // "end of file"
    TOKEN_YYerror = 256,           // error
    TOKEN_YYUNDEF = 257,           // "invalid token"
    TOKEN_REGION_SEPARATOR = 258,  // REGION_SEPARATOR
    TOKEN_OPENCURLY = 259,         // OPENCURLY
    TOKEN_CLOSECURLY = 260,        // CLOSECURLY
    TOKEN_OPENSQUARE = 261,        // OPENSQUARE
    TOKEN_CLOSESQUARE = 262,       // CLOSESQUARE
    TOKEN_OPENPAREN = 263,         // OPENPAREN
    TOKEN_CLOSEPAREN = 264,        // CLOSEPAREN
    TOKEN_SEMICOLON = 265,         // SEMICOLON
    TOKEN_NONLOCALRETURN = 266,    // NONLOCALRETURN
    TOKEN_COMMA = 267,             // COMMA
    TOKEN_HASH = 268,              // HASH
    TOKEN_TILDE = 269,             // TILDE
    TOKEN_NAME = 270,              // NAME
    TOKEN_INTEGER = 271,           // INTEGER
    TOKEN_INTEGER_RADIX = 272,     // INTEGER_RADIX
    TOKEN_HEXADECIMAL = 273,       // HEXADECIMAL
    TOKEN_FLOAT = 274,             // FLOAT
    TOKEN_FLOAT_RADIX = 275,       // FLOAT_RADIX
    TOKEN_FLOAT_EXPONENT = 276,    // FLOAT_EXPONENT
    TOKEN_FLOAT_INF = 277,         // FLOAT_INF
    TOKEN_ACCIDENTAL_STEPS = 278,  // ACCIDENTAL_STEPS
    TOKEN_ACCIDENTAL_CENTS = 279,  // ACCIDENTAL_CENTS
    TOKEN_SYMBOL_QUOTE = 280,      // SYMBOL_QUOTE
    TOKEN_SYMBOL_SLASH = 281,      // SYMBOL_SLASH
    TOKEN_STRINGLINE = 282,        // STRINGLINE
    TOKEN_ASCII = 283,             // ASCII
    TOKEN_PRIMITIVENAME = 284,     // PRIMITIVENAME
    TOKEN_CLASSNAME = 285,         // CLASSNAME
    TOKEN_CURRYARG = 286,          // CURRYARG
    TOKEN_VAR = 287,               // VAR
    TOKEN_ARG = 288,               // ARG
    TOKEN_CLASSVAR = 289,          // CLASSVAR
    TOKEN_CONST = 290,             // CONST
    TOKEN_NIL = 291,               // NIL
    TOKEN_TRUE = 292,              // TRUE
    TOKEN_FALSE = 293,             // FALSE
    TOKEN_PI = 294,                // PI
    TOKEN_ELLIPSIS = 295,          // ELLIPSIS
    TOKEN_DOTDOT = 296,            // DOTDOT
    TOKEN_BEGINCLOSEDFUNC = 297,   // BEGINCLOSEDFUNC
    TOKEN_BADTOKEN = 298,          // BADTOKEN
    TOKEN_INTERPRET = 299,         // INTERPRET
    TOKEN_INTERPRET_FUNCTION = 300, // INTERPRET_FUNCTION
    TOKEN_LEFTARROW = 301,         // LEFTARROW
    TOKEN_WHILE = 302,             // WHILE
    TOKEN_LEXER_ERROR = 303,       // LEXER_ERROR
    TOKEN_COLON = 304,             // COLON
    TOKEN_EQUALSSIGN = 305,        // EQUALSSIGN
    TOKEN_BINOP = 306,             // BINOP
    TOKEN_KEYBINOP = 307,          // KEYBINOP
    TOKEN_MINUS = 308,             // MINUS
    TOKEN_LESSTHAN = 309,          // LESSTHAN
    TOKEN_GREATERTHAN = 310,       // GREATERTHAN
    TOKEN_MULTIPLY = 311,          // MULTIPLY
    TOKEN_ADD = 312,               // ADD
    TOKEN_PIPE = 313,              // PIPE
    TOKEN_READWRITEVAR = 314,      // READWRITEVAR
    TOKEN_DOT = 315,               // DOT
    TOKEN_BACKTICK = 316,          // BACKTICK
    TOKEN_UMINUS = 317             // UMINUS
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 63, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // "end of file"
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_REGION_SEPARATOR = 3,                  // REGION_SEPARATOR
        S_OPENCURLY = 4,                         // OPENCURLY
        S_CLOSECURLY = 5,                        // CLOSECURLY
        S_OPENSQUARE = 6,                        // OPENSQUARE
        S_CLOSESQUARE = 7,                       // CLOSESQUARE
        S_OPENPAREN = 8,                         // OPENPAREN
        S_CLOSEPAREN = 9,                        // CLOSEPAREN
        S_SEMICOLON = 10,                        // SEMICOLON
        S_NONLOCALRETURN = 11,                   // NONLOCALRETURN
        S_COMMA = 12,                            // COMMA
        S_HASH = 13,                             // HASH
        S_TILDE = 14,                            // TILDE
        S_NAME = 15,                             // NAME
        S_INTEGER = 16,                          // INTEGER
        S_INTEGER_RADIX = 17,                    // INTEGER_RADIX
        S_HEXADECIMAL = 18,                      // HEXADECIMAL
        S_FLOAT = 19,                            // FLOAT
        S_FLOAT_RADIX = 20,                      // FLOAT_RADIX
        S_FLOAT_EXPONENT = 21,                   // FLOAT_EXPONENT
        S_FLOAT_INF = 22,                        // FLOAT_INF
        S_ACCIDENTAL_STEPS = 23,                 // ACCIDENTAL_STEPS
        S_ACCIDENTAL_CENTS = 24,                 // ACCIDENTAL_CENTS
        S_SYMBOL_QUOTE = 25,                     // SYMBOL_QUOTE
        S_SYMBOL_SLASH = 26,                     // SYMBOL_SLASH
        S_STRINGLINE = 27,                       // STRINGLINE
        S_ASCII = 28,                            // ASCII
        S_PRIMITIVENAME = 29,                    // PRIMITIVENAME
        S_CLASSNAME = 30,                        // CLASSNAME
        S_CURRYARG = 31,                         // CURRYARG
        S_VAR = 32,                              // VAR
        S_ARG = 33,                              // ARG
        S_CLASSVAR = 34,                         // CLASSVAR
        S_CONST = 35,                            // CONST
        S_NIL = 36,                              // NIL
        S_TRUE = 37,                             // TRUE
        S_FALSE = 38,                            // FALSE
        S_PI = 39,                               // PI
        S_ELLIPSIS = 40,                         // ELLIPSIS
        S_DOTDOT = 41,                           // DOTDOT
        S_BEGINCLOSEDFUNC = 42,                  // BEGINCLOSEDFUNC
        S_BADTOKEN = 43,                         // BADTOKEN
        S_INTERPRET = 44,                        // INTERPRET
        S_INTERPRET_FUNCTION = 45,               // INTERPRET_FUNCTION
        S_LEFTARROW = 46,                        // LEFTARROW
        S_WHILE = 47,                            // WHILE
        S_LEXER_ERROR = 48,                      // LEXER_ERROR
        S_COLON = 49,                            // COLON
        S_EQUALSSIGN = 50,                       // EQUALSSIGN
        S_BINOP = 51,                            // BINOP
        S_KEYBINOP = 52,                         // KEYBINOP
        S_MINUS = 53,                            // MINUS
        S_LESSTHAN = 54,                         // LESSTHAN
        S_GREATERTHAN = 55,                      // GREATERTHAN
        S_MULTIPLY = 56,                         // MULTIPLY
        S_ADD = 57,                              // ADD
        S_PIPE = 58,                             // PIPE
        S_READWRITEVAR = 59,                     // READWRITEVAR
        S_DOT = 60,                              // DOT
        S_BACKTICK = 61,                         // BACKTICK
        S_UMINUS = 62,                           // UMINUS
        S_YYACCEPT = 63,                         // $accept
        S_go = 64,                               // go
        S_file_as_func = 65,                     // file_as_func
        S_interpreted_function = 66,             // interpreted_function
        S_67_region_item = 67,                   // region.item
        S_region = 68,                           // region
        S_69_classOrExtList_list = 69,           // classOrExtList.list
        S_70_classOrExtList_item = 70,           // classOrExtList.item
        S_class = 71,                            // class
        S_72_class_super_opt = 72,               // class.super.opt
        S_73_class_slot_opt = 73,                // class.slot.opt
        S_74_class_extension = 74,               // class.extension
        S_75_members_entry_item = 75,            // members.entry.item
        S_76_members_entry_list = 76,            // members.entry.list
        S_77_members_entry = 77,                 // members.entry
        S_members = 78,                          // members
        S_79_members_opt = 79,                   // members.opt
        S_80_method_name = 80,                   // method.name
        S_81_method_base = 81,                   // method.base
        S_method = 82,                           // method
        S_83_method_list = 83,                   // method.list
        S_84_method_list_opt = 84,               // method.list.opt
        S_85_block_open = 85,                    // block.open
        S_block = 86,                            // block
        S_87_block_opt_list = 87,                // block.opt_list
        S_88_block_list = 88,                    // block.list
        S_89_block_contents = 89,                // block.contents
        S_90_block_contents_item = 90,           // block.contents.item
        S_adverb = 91,                           // adverb
        S_92_binary_op_raw = 92,                 // binary_op.raw
        S_93_binary_op_no_adverb = 93,           // binary_op.no_adverb
        S_binary_op = 94,                        // binary_op
        S_msgsend = 95,                          // msgsend
        S_96_expr_dotrange = 96,                 // expr.dotrange
        S_97_expr_dotrange_assign = 97,          // expr.dotrange.assign
        S_98_expr_base_range = 98,               // expr.base.range
        S_99_expr_base_range_assign = 99,        // expr.base.range.assign
        S_range = 100,                           // range
        S_101_range_generator = 101,             // range.generator
        S_102_expr_base = 102,                   // expr.base
        S_expr = 103,                            // expr
        S_104_expr_or_seq_base = 104,            // expr_or_seq.base
        S_expr_or_seq = 105,                     // expr_or_seq
        S_generator = 106,                       // generator
        S_107_generator_qual_list = 107,         // generator.qual.list
        S_108_generator_qual = 108,              // generator.qual
        S_109_argument_declarations_list = 109,  // argument_declarations.list
        S_110_argument_declarations_pipelist = 110, // argument_declarations.pipelist
        S_argument_declarations = 111,           // argument_declarations
        S_112_argument_declarations_opt = 112,   // argument_declarations.opt
        S_113_arguments_entries = 113,           // arguments.entries
        S_114_arguments_no_trailing = 114,       // arguments.no_trailing
        S_arguments = 115,                       // arguments
        S_116_arguments_paren = 116,             // arguments.paren
        S_117_arguments_maybe_paren = 117,       // arguments.maybe_paren
        S_118_variable_declarations_list_item = 118, // variable_declarations.list.item
        S_119_variable_declarations_list = 119,  // variable_declarations.list
        S_variable_declarations = 120,           // variable_declarations
        S_121_literal_terminal = 121,            // literal.terminal
        S_122_literal_array_contents = 122,      // literal.array.contents
        S_123_literal_dictionary_entry = 123,    // literal.dictionary.entry
        S_124_literal_dictionary_entries = 124,  // literal.dictionary.entries
        S_125_literal_dictionary = 125,          // literal.dictionary
        S_126_literal_array = 126,               // literal.array
        S_literal = 127,                         // literal
        S_name = 128,                            // name
        S_129_name_list = 129,                   // name.list
        S_130_name_pack = 130,                   // name.pack
        S_ascii = 131,                           // ascii
        S_nil = 132,                             // nil
        S_boolean = 133,                         // boolean
        S_symbol = 134,                          // symbol
        S_string = 135,                          // string
        S_integer = 136,                         // integer
        S_137_float_raw_unsigned = 137,          // float.raw_unsigned
        S_138_float_raw = 138,                   // float.raw
        S_139_accidental_unsigned = 139,         // accidental.unsigned
        S_accidental = 140,                      // accidental
        S_float = 141,                           // float
        S_accessor = 142,                        // accessor
        S_143_semicolon_opt = 143,               // semicolon.opt
        S_144_comma_opt = 144                    // comma.opt
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value (std::move (that.value))
        , location (std::move (that.location))
      {}
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);
      /// Constructor for valueless symbols.
      basic_symbol (typename Base::kind_type t,
                    YY_MOVE_REF (location_type) l);

      /// Constructor for symbols with semantic value.
      basic_symbol (typename Base::kind_type t,
                    YY_RVREF (value_type) v,
                    YY_RVREF (location_type) l);

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        Base::clear ();
      }

      /// The user-facing name of this symbol.
      const char *name () const YY_NOEXCEPT
      {
        return parser::symbol_name (this->kind ());
      }

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;



      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {};

    /// Build a parser object.
    parser (ParserContext& cxt_yyarg);
    virtual ~parser ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    parser (const parser&) = delete;
    /// Non copyable.
    parser& operator= (const parser&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static const char *symbol_name (symbol_kind_type yysymbol);



    class context
    {
    public:
      context (const parser& yyparser, const symbol_type& yyla);
      const symbol_type& lookahead () const YY_NOEXCEPT { return yyla_; }
      symbol_kind_type token () const YY_NOEXCEPT { return yyla_.kind (); }
      const location_type& location () const YY_NOEXCEPT { return yyla_.location; }

      /// Put in YYARG at most YYARGN of the expected tokens, and return the
      /// number of tokens stored in YYARG.  If YYARG is null, return the
      /// number of expected tokens (guaranteed to be less than YYNTOKENS).
      int expected_tokens (symbol_kind_type yyarg[], int yyargn) const;

    private:
      const parser& yyparser_;
      const symbol_type& yyla_;
    };

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    parser (const parser&);
    /// Non copyable.
    parser& operator= (const parser&);
#endif

    /// Check the lookahead yytoken.
    /// \returns  true iff the token will be eventually shifted.
    bool yy_lac_check_ (symbol_kind_type yytoken) const;
    /// Establish the initial context if no initial context currently exists.
    /// \returns  true iff the token will be eventually shifted.
    bool yy_lac_establish_ (symbol_kind_type yytoken);
    /// Discard any previous initial lookahead context because of event.
    /// \param event  the event which caused the lookahead to be discarded.
    ///               Only used for debbuging output.
    void yy_lac_discard_ (const char* event);

    /// Stored state numbers (used for stacks).
    typedef short state_type;

    /// Report a syntax error
    /// \param yyctx     the context in which the error occurred.
    void report_syntax_error (const context& yyctx) const;
    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const short yypact_ninf_;
    static const short yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;



    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const short yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const short yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const short yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const unsigned char yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const unsigned char yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;
    /// The stack for LAC.
    /// Logically, the yy_lac_stack's lifetime is confined to the function
    /// yy_lac_check_. We just store it as a member of this class to hold
    /// on to the memory and to avoid frequent reallocations.
    /// Since yy_lac_check_ is const, this member must be mutable.
    mutable std::vector<state_type> yylac_stack_;
    /// Whether an initial LAC context was established.
    bool yy_lac_established_;


    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 2649,     ///< Last index in yytable_.
      yynnts_ = 82,  ///< Number of nonterminal symbols.
      yyfinal_ = 92 ///< Termination state number.
    };


    // User arguments.
    ParserContext& cxt;

  };


#line 66 "langutils/sc_parser/src/sc_grammar.y"
} } } // sc::ast::parser
#line 1064 "langutils/sc_parser/src/sc_grammar_parser.hpp"




#endif // !YY_YY_LANGUTILS_SC_PARSER_SRC_SC_GRAMMAR_PARSER_HPP_INCLUDED
