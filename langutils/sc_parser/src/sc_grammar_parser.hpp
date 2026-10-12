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
#line 16 "langutils/sc_parser/src/sc_grammar.y"


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

#line 7 "langutils/sc_parser/src/sc_grammar.y"
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
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class value_type
  {
  public:
    /// Type of *this.
    typedef value_type self_type;

    /// Empty construction.
    value_type () YY_NOEXCEPT
      : yyraw_ ()
    {}

    /// Construct and fill.
    template <typename T>
    value_type (YY_RVREF (T) t)
    {
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    value_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif

    /// Destruction, allowed only if empty.
    ~value_type () YY_NOEXCEPT
    {}

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
    }

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    value_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yyraw_;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yyraw_;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // ascii
      char dummy1[sizeof (ASCIIIndex)];

      // accidental.unsigned
      // accidental
      char dummy2[sizeof (AccidentalIndex)];

      // adverb
      char dummy3[sizeof (AdverbIndex)];

      // msgsend
      // expr.base
      // expr
      // expr_or_seq.base
      // expr_or_seq
      char dummy4[sizeof (AnyExprIndex)];

      // method
      char dummy5[sizeof(AnyMethodIndex)];

      // literal.terminal
      // literal
      char dummy6[sizeof(AnyPossiblyLiteralIndex)];

      // arguments.entries
      char dummy7[sizeof (ArgumentEntryIndex)];

      // arguments.no_trailing
      // arguments
      // arguments.paren
      // arguments.maybe_paren
      char dummy8[sizeof (ArgumentListIndex)];

      // literal.array.contents
      // literal.array
      char dummy9[sizeof (ArrayIndex)];

      // members
      // members.opt
      char dummy10[sizeof(BigMemberListIndex)];

      // block.contents
      char dummy11[sizeof(BlockContentsListIndex)];

      // block
      char dummy12[sizeof(BlockIndex)];

      // block.contents.item
      char dummy13[sizeof(BlockItemIndex)];

      // block.opt_list
      // block.list
      char dummy14[sizeof(BlockListIndex)];

      // boolean
      char dummy15[sizeof(BooleanLitIndex)];

      // class.extension
      char dummy16[sizeof(ClassExtensionIndex)];

      // class
      char dummy17[sizeof(ClassIndex)];

      // go
      char dummy18[sizeof(ClassListOrExprListIndex)];

      // classOrExtList.item
      char dummy19[sizeof(ClassOrExtensionIndex)];

      // classOrExtList.list
      char dummy20[sizeof(ClassOrExtensionListIndex)];

      // members.entry
      char dummy21[sizeof(DeclareAnyMemberListIndex)];

      // variable_declarations.list.item
      char dummy22[sizeof(DeclareAnyVariableIndex)];

      // argument_declarations.list
      // argument_declarations.pipelist
      // argument_declarations
      // argument_declarations.opt
      char dummy23[sizeof(DeclareArgumentListIndex)];

      // members.entry.item
      char dummy24[sizeof(DeclareMemberIndex)];

      // members.entry.list
      char dummy25[sizeof (DeclareMemberListIndex)];

      // variable_declarations.list
      // variable_declarations
      char dummy26[sizeof (DeclareVariableListIndex)];

      // literal.dictionary.entry
      char dummy27[sizeof (DictionaryEntryIndex)];

      // literal.dictionary.entries
      // literal.dictionary
      char dummy28[sizeof (DictionaryIndex)];

      // float.raw_unsigned
      // float.raw
      char dummy29[sizeof (FloatLitIndex)];

      // float
      char dummy30[sizeof (FloatProducingIndex)];

      // generator
      char dummy31[sizeof(GeneratorAnyIndex)];

      // generator.qual
      char dummy32[sizeof(GeneratorQualAnyIndex)];

      // generator.qual.list
      char dummy33[sizeof(GeneratorQualListIndex)];

      // integer
      char dummy34[sizeof(IntLitIndex)];

      // REGION_SEPARATOR
      // OPENCURLY
      // CLOSECURLY
      // OPENSQUARE
      // CLOSESQUARE
      // OPENPAREN
      // CLOSEPAREN
      // SEMICOLON
      // NONLOCALRETURN
      // COMMA
      // HASH
      // TILDE
      // NAME
      // INTEGER
      // INTEGER_RADIX
      // HEXADECIMAL
      // FLOAT
      // FLOAT_RADIX
      // FLOAT_EXPONENT
      // FLOAT_INF
      // ACCIDENTAL_STEPS
      // ACCIDENTAL_CENTS
      // SYMBOL_QUOTE
      // SYMBOL_SLASH
      // STRINGLINE
      // ASCII
      // PRIMITIVENAME
      // CLASSNAME
      // CURRYARG
      // VAR
      // ARG
      // CLASSVAR
      // CONST
      // NIL
      // TRUE
      // FALSE
      // PI
      // ELLIPSIS
      // DOTDOT
      // BEGINCLOSEDFUNC
      // BADTOKEN
      // INTERPRET
      // LEFTARROW
      // WHILE
      // LEXER_ERROR
      // COLON
      // EQUALSSIGN
      // BINOP
      // KEYBINOP
      // MINUS
      // LESSTHAN
      // GREATERTHAN
      // MULTIPLY
      // ADD
      // PIPE
      // READWRITEVAR
      // DOT
      // BACKTICK
      // UMINUS
      char dummy35[sizeof(LexerToken)];

      // method.base
      char dummy36[sizeof(MethodIndex)];

      // method.list
      // method.list.opt
      char dummy37[sizeof(MethodListIndex)];

      // name.list
      char dummy38[sizeof(NameListIndex)];

      // name
      char dummy39[sizeof(NamedIdentifierIndex)];

      // nil
      char dummy40[sizeof(NilLitIndex)];

      // accessor
      char dummy41[sizeof(RWAccessorIndex)];

      // region
      char dummy42[sizeof(RegionListIndex)];

      // method.name
      // binary_op.raw
      // binary_op.no_adverb
      char dummy43[sizeof(SelectorIndex)];

      // binary_op
      char dummy44[sizeof(SelectorMaybeAdverbIndex)];

      // string
      char dummy45[sizeof(StringLitIndex)];

      // symbol
      char dummy46[sizeof(SymbolLitIndex)];

      // region.item
      char dummy47[sizeof(error_index<AnyExprIndex>)];

      // class.super.opt
      char dummy48[sizeof(maybe<ClassNameIdentifierIndex>)];

      // class.slot.opt
      char dummy49[sizeof(maybe<NamedIdentifierIndex>)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me_;
      /// A buffer large enough to store any of the semantic values.
      char yyraw_[size];
    };
  };

#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef  sc::lex::SourceCodeRange  location_type;

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
        enum token_kind_type {
            TOKEN_YYEMPTY = -2,
            TOKEN_YYEOF = 0, // "end of file"
            TOKEN_YYerror = 256, // error
            TOKEN_YYUNDEF = 257, // "invalid token"
            TOKEN_REGION_SEPARATOR = 258, // REGION_SEPARATOR
            TOKEN_OPENCURLY = 259, // OPENCURLY
            TOKEN_CLOSECURLY = 260, // CLOSECURLY
            TOKEN_OPENSQUARE = 261, // OPENSQUARE
            TOKEN_CLOSESQUARE = 262, // CLOSESQUARE
            TOKEN_OPENPAREN = 263, // OPENPAREN
            TOKEN_CLOSEPAREN = 264, // CLOSEPAREN
            TOKEN_SEMICOLON = 265, // SEMICOLON
            TOKEN_NONLOCALRETURN = 266, // NONLOCALRETURN
            TOKEN_COMMA = 267, // COMMA
            TOKEN_HASH = 268, // HASH
            TOKEN_TILDE = 269, // TILDE
            TOKEN_NAME = 270, // NAME
            TOKEN_INTEGER = 271, // INTEGER
            TOKEN_INTEGER_RADIX = 272, // INTEGER_RADIX
            TOKEN_HEXADECIMAL = 273, // HEXADECIMAL
            TOKEN_FLOAT = 274, // FLOAT
            TOKEN_FLOAT_RADIX = 275, // FLOAT_RADIX
            TOKEN_FLOAT_EXPONENT = 276, // FLOAT_EXPONENT
            TOKEN_FLOAT_INF = 277, // FLOAT_INF
            TOKEN_ACCIDENTAL_STEPS = 278, // ACCIDENTAL_STEPS
            TOKEN_ACCIDENTAL_CENTS = 279, // ACCIDENTAL_CENTS
            TOKEN_SYMBOL_QUOTE = 280, // SYMBOL_QUOTE
            TOKEN_SYMBOL_SLASH = 281, // SYMBOL_SLASH
            TOKEN_STRINGLINE = 282, // STRINGLINE
            TOKEN_ASCII = 283, // ASCII
            TOKEN_PRIMITIVENAME = 284, // PRIMITIVENAME
            TOKEN_CLASSNAME = 285, // CLASSNAME
            TOKEN_CURRYARG = 286, // CURRYARG
            TOKEN_VAR = 287, // VAR
            TOKEN_ARG = 288, // ARG
            TOKEN_CLASSVAR = 289, // CLASSVAR
            TOKEN_CONST = 290, // CONST
            TOKEN_NIL = 291, // NIL
            TOKEN_TRUE = 292, // TRUE
            TOKEN_FALSE = 293, // FALSE
            TOKEN_PI = 294, // PI
            TOKEN_ELLIPSIS = 295, // ELLIPSIS
            TOKEN_DOTDOT = 296, // DOTDOT
            TOKEN_BEGINCLOSEDFUNC = 297, // BEGINCLOSEDFUNC
            TOKEN_BADTOKEN = 298, // BADTOKEN
            TOKEN_INTERPRET = 299, // INTERPRET
            TOKEN_LEFTARROW = 300, // LEFTARROW
            TOKEN_WHILE = 301, // WHILE
            TOKEN_LEXER_ERROR = 302, // LEXER_ERROR
            TOKEN_COLON = 303, // COLON
            TOKEN_EQUALSSIGN = 304, // EQUALSSIGN
            TOKEN_BINOP = 305, // BINOP
            TOKEN_KEYBINOP = 306, // KEYBINOP
            TOKEN_MINUS = 307, // MINUS
            TOKEN_LESSTHAN = 308, // LESSTHAN
            TOKEN_GREATERTHAN = 309, // GREATERTHAN
            TOKEN_MULTIPLY = 310, // MULTIPLY
            TOKEN_ADD = 311, // ADD
            TOKEN_PIPE = 312, // PIPE
            TOKEN_READWRITEVAR = 313, // READWRITEVAR
            TOKEN_DOT = 314, // DOT
            TOKEN_BACKTICK = 315, // BACKTICK
            TOKEN_UMINUS = 316 // UMINUS
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
        enum symbol_kind_type {
            YYNTOKENS = 62, ///< Number of tokens.
            S_YYEMPTY = -2,
            S_YYEOF = 0, // "end of file"
            S_YYerror = 1, // error
            S_YYUNDEF = 2, // "invalid token"
            S_REGION_SEPARATOR = 3, // REGION_SEPARATOR
            S_OPENCURLY = 4, // OPENCURLY
            S_CLOSECURLY = 5, // CLOSECURLY
            S_OPENSQUARE = 6, // OPENSQUARE
            S_CLOSESQUARE = 7, // CLOSESQUARE
            S_OPENPAREN = 8, // OPENPAREN
            S_CLOSEPAREN = 9, // CLOSEPAREN
            S_SEMICOLON = 10, // SEMICOLON
            S_NONLOCALRETURN = 11, // NONLOCALRETURN
            S_COMMA = 12, // COMMA
            S_HASH = 13, // HASH
            S_TILDE = 14, // TILDE
            S_NAME = 15, // NAME
            S_INTEGER = 16, // INTEGER
            S_INTEGER_RADIX = 17, // INTEGER_RADIX
            S_HEXADECIMAL = 18, // HEXADECIMAL
            S_FLOAT = 19, // FLOAT
            S_FLOAT_RADIX = 20, // FLOAT_RADIX
            S_FLOAT_EXPONENT = 21, // FLOAT_EXPONENT
            S_FLOAT_INF = 22, // FLOAT_INF
            S_ACCIDENTAL_STEPS = 23, // ACCIDENTAL_STEPS
            S_ACCIDENTAL_CENTS = 24, // ACCIDENTAL_CENTS
            S_SYMBOL_QUOTE = 25, // SYMBOL_QUOTE
            S_SYMBOL_SLASH = 26, // SYMBOL_SLASH
            S_STRINGLINE = 27, // STRINGLINE
            S_ASCII = 28, // ASCII
            S_PRIMITIVENAME = 29, // PRIMITIVENAME
            S_CLASSNAME = 30, // CLASSNAME
            S_CURRYARG = 31, // CURRYARG
            S_VAR = 32, // VAR
            S_ARG = 33, // ARG
            S_CLASSVAR = 34, // CLASSVAR
            S_CONST = 35, // CONST
            S_NIL = 36, // NIL
            S_TRUE = 37, // TRUE
            S_FALSE = 38, // FALSE
            S_PI = 39, // PI
            S_ELLIPSIS = 40, // ELLIPSIS
            S_DOTDOT = 41, // DOTDOT
            S_BEGINCLOSEDFUNC = 42, // BEGINCLOSEDFUNC
            S_BADTOKEN = 43, // BADTOKEN
            S_INTERPRET = 44, // INTERPRET
            S_LEFTARROW = 45, // LEFTARROW
            S_WHILE = 46, // WHILE
            S_LEXER_ERROR = 47, // LEXER_ERROR
            S_COLON = 48, // COLON
            S_EQUALSSIGN = 49, // EQUALSSIGN
            S_BINOP = 50, // BINOP
            S_KEYBINOP = 51, // KEYBINOP
            S_MINUS = 52, // MINUS
            S_LESSTHAN = 53, // LESSTHAN
            S_GREATERTHAN = 54, // GREATERTHAN
            S_MULTIPLY = 55, // MULTIPLY
            S_ADD = 56, // ADD
            S_PIPE = 57, // PIPE
            S_READWRITEVAR = 58, // READWRITEVAR
            S_DOT = 59, // DOT
            S_BACKTICK = 60, // BACKTICK
            S_UMINUS = 61, // UMINUS
            S_YYACCEPT = 62, // $accept
            S_go = 63, // go
            S_64_region_item = 64, // region.item
            S_region = 65, // region
            S_66_classOrExtList_list = 66, // classOrExtList.list
            S_67_classOrExtList_item = 67, // classOrExtList.item
            S_class = 68, // class
            S_69_class_super_opt = 69, // class.super.opt
            S_70_class_slot_opt = 70, // class.slot.opt
            S_71_class_extension = 71, // class.extension
            S_72_members_entry_item = 72, // members.entry.item
            S_73_members_entry_list = 73, // members.entry.list
            S_74_members_entry = 74, // members.entry
            S_members = 75, // members
            S_76_members_opt = 76, // members.opt
            S_77_method_name = 77, // method.name
            S_78_method_base = 78, // method.base
            S_method = 79, // method
            S_80_method_list = 80, // method.list
            S_81_method_list_opt = 81, // method.list.opt
            S_82_block_open = 82, // block.open
            S_block = 83, // block
            S_84_block_opt_list = 84, // block.opt_list
            S_85_block_list = 85, // block.list
            S_86_block_contents = 86, // block.contents
            S_87_block_contents_item = 87, // block.contents.item
            S_adverb = 88, // adverb
            S_89_binary_op_raw = 89, // binary_op.raw
            S_90_binary_op_no_adverb = 90, // binary_op.no_adverb
            S_binary_op = 91, // binary_op
            S_msgsend = 92, // msgsend
            S_93_expr_base = 93, // expr.base
            S_expr = 94, // expr
            S_95_expr_or_seq_base = 95, // expr_or_seq.base
            S_expr_or_seq = 96, // expr_or_seq
            S_generator = 97, // generator
            S_98_generator_qual_list = 98, // generator.qual.list
            S_99_generator_qual = 99, // generator.qual
            S_100_argument_declarations_list = 100, // argument_declarations.list
            S_101_argument_declarations_pipelist = 101, // argument_declarations.pipelist
            S_argument_declarations = 102, // argument_declarations
            S_103_argument_declarations_opt = 103, // argument_declarations.opt
            S_104_arguments_entries = 104, // arguments.entries
            S_105_arguments_no_trailing = 105, // arguments.no_trailing
            S_arguments = 106, // arguments
            S_107_arguments_paren = 107, // arguments.paren
            S_108_arguments_maybe_paren = 108, // arguments.maybe_paren
            S_109_variable_declarations_list_item = 109, // variable_declarations.list.item
            S_110_variable_declarations_list = 110, // variable_declarations.list
            S_variable_declarations = 111, // variable_declarations
            S_112_literal_terminal = 112, // literal.terminal
            S_113_literal_array_contents = 113, // literal.array.contents
            S_114_literal_dictionary_entry = 114, // literal.dictionary.entry
            S_115_literal_dictionary_entries = 115, // literal.dictionary.entries
            S_116_literal_dictionary = 116, // literal.dictionary
            S_117_literal_array = 117, // literal.array
            S_literal = 118, // literal
            S_name = 119, // name
            S_120_name_list = 120, // name.list
            S_ascii = 121, // ascii
            S_nil = 122, // nil
            S_boolean = 123, // boolean
            S_symbol = 124, // symbol
            S_string = 125, // string
            S_integer = 126, // integer
            S_127_float_raw_unsigned = 127, // float.raw_unsigned
            S_128_float_raw = 128, // float.raw
            S_129_accidental_unsigned = 129, // accidental.unsigned
            S_accidental = 130, // accidental
            S_float = 131, // float
            S_accessor = 132, // accessor
            S_133_semicolon_opt = 133, // semicolon.opt
            S_134_comma_opt = 134 // comma.opt
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
        , value ()
        , location (std::move (that.location))
      {
        switch (this->kind ())
    {
      case symbol_kind::S_ascii: // ascii
        value.move< ASCIIIndex > (std::move (that.value));
        break;

      case symbol_kind::S_129_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.move< AccidentalIndex > (std::move (that.value));
        break;

      case symbol_kind::S_adverb: // adverb
        value.move< AdverbIndex > (std::move (that.value));
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_93_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_95_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
          value.move<AnyExprIndex>(std::move(that.value));
          break;

      case symbol_kind::S_method: // method
        value.move< AnyMethodIndex > (std::move (that.value));
        break;

      case symbol_kind::S_112_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.move<AnyPossiblyLiteralIndex>(std::move(that.value));
          break;

      case symbol_kind::S_104_arguments_entries: // arguments.entries
          value.move<ArgumentEntryIndex>(std::move(that.value));
          break;

      case symbol_kind::S_105_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_107_arguments_paren: // arguments.paren
      case symbol_kind::S_108_arguments_maybe_paren: // arguments.maybe_paren
          value.move<ArgumentListIndex>(std::move(that.value));
          break;

      case symbol_kind::S_113_literal_array_contents: // literal.array.contents
      case symbol_kind::S_117_literal_array: // literal.array
          value.move<ArrayIndex>(std::move(that.value));
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_76_members_opt: // members.opt
          value.move<BigMemberListIndex>(std::move(that.value));
          break;

      case symbol_kind::S_86_block_contents: // block.contents
          value.move<BlockContentsListIndex>(std::move(that.value));
          break;

      case symbol_kind::S_block: // block
        value.move< BlockIndex > (std::move (that.value));
        break;

      case symbol_kind::S_87_block_contents_item: // block.contents.item
          value.move<BlockItemIndex>(std::move(that.value));
          break;

      case symbol_kind::S_84_block_opt_list: // block.opt_list
      case symbol_kind::S_85_block_list: // block.list
          value.move<BlockListIndex>(std::move(that.value));
          break;

      case symbol_kind::S_boolean: // boolean
        value.move< BooleanLitIndex > (std::move (that.value));
        break;

      case symbol_kind::S_71_class_extension: // class.extension
          value.move<ClassExtensionIndex>(std::move(that.value));
          break;

      case symbol_kind::S_class: // class
        value.move< ClassIndex > (std::move (that.value));
        break;

      case symbol_kind::S_go: // go
        value.move< ClassListOrExprListIndex > (std::move (that.value));
        break;

      case symbol_kind::S_67_classOrExtList_item: // classOrExtList.item
          value.move<ClassOrExtensionIndex>(std::move(that.value));
          break;

      case symbol_kind::S_66_classOrExtList_list: // classOrExtList.list
          value.move<ClassOrExtensionListIndex>(std::move(that.value));
          break;

      case symbol_kind::S_74_members_entry: // members.entry
          value.move<DeclareAnyMemberListIndex>(std::move(that.value));
          break;

      case symbol_kind::S_109_variable_declarations_list_item: // variable_declarations.list.item
          value.move<DeclareAnyVariableIndex>(std::move(that.value));
          break;

      case symbol_kind::S_100_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_101_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_103_argument_declarations_opt: // argument_declarations.opt
          value.move<DeclareArgumentListIndex>(std::move(that.value));
          break;

      case symbol_kind::S_72_members_entry_item: // members.entry.item
          value.move<DeclareMemberIndex>(std::move(that.value));
          break;

      case symbol_kind::S_73_members_entry_list: // members.entry.list
          value.move<DeclareMemberListIndex>(std::move(that.value));
          break;

      case symbol_kind::S_110_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.move< DeclareVariableListIndex > (std::move (that.value));
        break;

      case symbol_kind::S_114_literal_dictionary_entry: // literal.dictionary.entry
          value.move<DictionaryEntryIndex>(std::move(that.value));
          break;

      case symbol_kind::S_115_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_116_literal_dictionary: // literal.dictionary
          value.move<DictionaryIndex>(std::move(that.value));
          break;

      case symbol_kind::S_127_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_128_float_raw: // float.raw
          value.move<FloatLitIndex>(std::move(that.value));
          break;

      case symbol_kind::S_float: // float
        value.move< FloatProducingIndex > (std::move (that.value));
        break;

      case symbol_kind::S_generator: // generator
          value.move<GeneratorAnyIndex>(std::move(that.value));
          break;

      case symbol_kind::S_99_generator_qual: // generator.qual
          value.move<GeneratorQualAnyIndex>(std::move(that.value));
          break;

      case symbol_kind::S_98_generator_qual_list: // generator.qual.list
          value.move<GeneratorQualListIndex>(std::move(that.value));
          break;

      case symbol_kind::S_integer: // integer
        value.move< IntLitIndex > (std::move (that.value));
        break;

      case symbol_kind::S_REGION_SEPARATOR: // REGION_SEPARATOR
      case symbol_kind::S_OPENCURLY: // OPENCURLY
      case symbol_kind::S_CLOSECURLY: // CLOSECURLY
      case symbol_kind::S_OPENSQUARE: // OPENSQUARE
      case symbol_kind::S_CLOSESQUARE: // CLOSESQUARE
      case symbol_kind::S_OPENPAREN: // OPENPAREN
      case symbol_kind::S_CLOSEPAREN: // CLOSEPAREN
      case symbol_kind::S_SEMICOLON: // SEMICOLON
      case symbol_kind::S_NONLOCALRETURN: // NONLOCALRETURN
      case symbol_kind::S_COMMA: // COMMA
      case symbol_kind::S_HASH: // HASH
      case symbol_kind::S_TILDE: // TILDE
      case symbol_kind::S_NAME: // NAME
      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_INTEGER_RADIX: // INTEGER_RADIX
      case symbol_kind::S_HEXADECIMAL: // HEXADECIMAL
      case symbol_kind::S_FLOAT: // FLOAT
      case symbol_kind::S_FLOAT_RADIX: // FLOAT_RADIX
      case symbol_kind::S_FLOAT_EXPONENT: // FLOAT_EXPONENT
      case symbol_kind::S_FLOAT_INF: // FLOAT_INF
      case symbol_kind::S_ACCIDENTAL_STEPS: // ACCIDENTAL_STEPS
      case symbol_kind::S_ACCIDENTAL_CENTS: // ACCIDENTAL_CENTS
      case symbol_kind::S_SYMBOL_QUOTE: // SYMBOL_QUOTE
      case symbol_kind::S_SYMBOL_SLASH: // SYMBOL_SLASH
      case symbol_kind::S_STRINGLINE: // STRINGLINE
      case symbol_kind::S_ASCII: // ASCII
      case symbol_kind::S_PRIMITIVENAME: // PRIMITIVENAME
      case symbol_kind::S_CLASSNAME: // CLASSNAME
      case symbol_kind::S_CURRYARG: // CURRYARG
      case symbol_kind::S_VAR: // VAR
      case symbol_kind::S_ARG: // ARG
      case symbol_kind::S_CLASSVAR: // CLASSVAR
      case symbol_kind::S_CONST: // CONST
      case symbol_kind::S_NIL: // NIL
      case symbol_kind::S_TRUE: // TRUE
      case symbol_kind::S_FALSE: // FALSE
      case symbol_kind::S_PI: // PI
      case symbol_kind::S_ELLIPSIS: // ELLIPSIS
      case symbol_kind::S_DOTDOT: // DOTDOT
      case symbol_kind::S_BEGINCLOSEDFUNC: // BEGINCLOSEDFUNC
      case symbol_kind::S_BADTOKEN: // BADTOKEN
      case symbol_kind::S_INTERPRET: // INTERPRET
      case symbol_kind::S_LEFTARROW: // LEFTARROW
      case symbol_kind::S_WHILE: // WHILE
      case symbol_kind::S_LEXER_ERROR: // LEXER_ERROR
      case symbol_kind::S_COLON: // COLON
      case symbol_kind::S_EQUALSSIGN: // EQUALSSIGN
      case symbol_kind::S_BINOP: // BINOP
      case symbol_kind::S_KEYBINOP: // KEYBINOP
      case symbol_kind::S_MINUS: // MINUS
      case symbol_kind::S_LESSTHAN: // LESSTHAN
      case symbol_kind::S_GREATERTHAN: // GREATERTHAN
      case symbol_kind::S_MULTIPLY: // MULTIPLY
      case symbol_kind::S_ADD: // ADD
      case symbol_kind::S_PIPE: // PIPE
      case symbol_kind::S_READWRITEVAR: // READWRITEVAR
      case symbol_kind::S_DOT: // DOT
      case symbol_kind::S_BACKTICK: // BACKTICK
      case symbol_kind::S_UMINUS: // UMINUS
        value.move< LexerToken > (std::move (that.value));
        break;

      case symbol_kind::S_78_method_base: // method.base
          value.move<MethodIndex>(std::move(that.value));
          break;

      case symbol_kind::S_80_method_list: // method.list
      case symbol_kind::S_81_method_list_opt: // method.list.opt
          value.move<MethodListIndex>(std::move(that.value));
          break;

      case symbol_kind::S_120_name_list: // name.list
          value.move<NameListIndex>(std::move(that.value));
          break;

      case symbol_kind::S_name: // name
        value.move< NamedIdentifierIndex > (std::move (that.value));
        break;

      case symbol_kind::S_nil: // nil
        value.move< NilLitIndex > (std::move (that.value));
        break;

      case symbol_kind::S_accessor: // accessor
          value.move<RWAccessorIndex>(std::move(that.value));
          break;

      case symbol_kind::S_region: // region
        value.move< RegionListIndex > (std::move (that.value));
        break;

      case symbol_kind::S_77_method_name: // method.name
      case symbol_kind::S_89_binary_op_raw: // binary_op.raw
      case symbol_kind::S_90_binary_op_no_adverb: // binary_op.no_adverb
          value.move<SelectorIndex>(std::move(that.value));
          break;

      case symbol_kind::S_binary_op: // binary_op
        value.move< SelectorMaybeAdverbIndex > (std::move (that.value));
        break;

      case symbol_kind::S_string: // string
        value.move< StringLitIndex > (std::move (that.value));
        break;

      case symbol_kind::S_symbol: // symbol
        value.move< SymbolLitIndex > (std::move (that.value));
        break;

      case symbol_kind::S_64_region_item: // region.item
          value.move<error_index<AnyExprIndex>>(std::move(that.value));
          break;

      case symbol_kind::S_69_class_super_opt: // class.super.opt
          value.move<maybe<ClassNameIdentifierIndex>>(std::move(that.value));
          break;

      case symbol_kind::S_70_class_slot_opt: // class.slot.opt
          value.move<maybe<NamedIdentifierIndex>>(std::move(that.value));
          break;

      default:
        break;
    }

      }
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructors for typed symbols.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ASCIIIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ASCIIIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AccidentalIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AccidentalIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AdverbIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AdverbIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AnyExprIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AnyExprIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol(typename Base::kind_type t, AnyMethodIndex&& v, location_type&& l):
          Base(t),
          value(std::move(v)),
          location(std::move(l)) {}
#else
      basic_symbol(typename Base::kind_type t, const AnyMethodIndex& v, const location_type& l):
          Base(t),
          value(v),
          location(l) {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol(typename Base::kind_type t, AnyPossiblyLiteralIndex&& v, location_type&& l):
          Base(t),
          value(std::move(v)),
          location(std::move(l)) {}
#else
      basic_symbol(typename Base::kind_type t, const AnyPossiblyLiteralIndex& v, const location_type& l):
          Base(t),
          value(v),
          location(l) {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ArgumentEntryIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ArgumentEntryIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ArgumentListIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ArgumentListIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ArrayIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ArrayIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol(typename Base::kind_type t, BigMemberListIndex&& v, location_type&& l):
          Base(t),
          value(std::move(v)),
          location(std::move(l)) {}
#else
      basic_symbol(typename Base::kind_type t, const BigMemberListIndex& v, const location_type& l):
          Base(t),
          value(v),
          location(l) {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, BlockContentsListIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const BlockContentsListIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, BlockIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const BlockIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, BlockItemIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const BlockItemIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, BlockListIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const BlockListIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, BooleanLitIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const BooleanLitIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ClassExtensionIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ClassExtensionIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ClassIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ClassIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ClassListOrExprListIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ClassListOrExprListIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ClassOrExtensionIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ClassOrExtensionIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ClassOrExtensionListIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ClassOrExtensionListIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol(typename Base::kind_type t, DeclareAnyMemberListIndex&& v, location_type&& l):
          Base(t),
          value(std::move(v)),
          location(std::move(l)) {}
#else
      basic_symbol(typename Base::kind_type t, const DeclareAnyMemberListIndex& v, const location_type& l):
          Base(t),
          value(v),
          location(l) {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, DeclareAnyVariableIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const DeclareAnyVariableIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, DeclareArgumentListIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const DeclareArgumentListIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol(typename Base::kind_type t, DeclareMemberIndex&& v, location_type&& l):
          Base(t),
          value(std::move(v)),
          location(std::move(l)) {}
#else
      basic_symbol(typename Base::kind_type t, const DeclareMemberIndex& v, const location_type& l):
          Base(t),
          value(v),
          location(l) {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, DeclareMemberListIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const DeclareMemberListIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, DeclareVariableListIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const DeclareVariableListIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, DictionaryEntryIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const DictionaryEntryIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, DictionaryIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const DictionaryIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, FloatLitIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const FloatLitIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, FloatProducingIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const FloatProducingIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol(typename Base::kind_type t, GeneratorAnyIndex&& v, location_type&& l):
          Base(t),
          value(std::move(v)),
          location(std::move(l)) {}
#else
      basic_symbol(typename Base::kind_type t, const GeneratorAnyIndex& v, const location_type& l):
          Base(t),
          value(v),
          location(l) {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol(typename Base::kind_type t, GeneratorQualAnyIndex&& v, location_type&& l):
          Base(t),
          value(std::move(v)),
          location(std::move(l)) {}
#else
      basic_symbol(typename Base::kind_type t, const GeneratorQualAnyIndex& v, const location_type& l):
          Base(t),
          value(v),
          location(l) {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol(typename Base::kind_type t, GeneratorQualListIndex&& v, location_type&& l):
          Base(t),
          value(std::move(v)),
          location(std::move(l)) {}
#else
      basic_symbol(typename Base::kind_type t, const GeneratorQualListIndex& v, const location_type& l):
          Base(t),
          value(v),
          location(l) {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, IntLitIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const IntLitIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, LexerToken&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const LexerToken& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, MethodIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const MethodIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, MethodListIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const MethodListIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol(typename Base::kind_type t, NameListIndex&& v, location_type&& l):
          Base(t),
          value(std::move(v)),
          location(std::move(l)) {}
#else
      basic_symbol(typename Base::kind_type t, const NameListIndex& v, const location_type& l):
          Base(t),
          value(v),
          location(l) {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, NamedIdentifierIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const NamedIdentifierIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, NilLitIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const NilLitIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol(typename Base::kind_type t, RWAccessorIndex&& v, location_type&& l):
          Base(t),
          value(std::move(v)),
          location(std::move(l)) {}
#else
      basic_symbol(typename Base::kind_type t, const RWAccessorIndex& v, const location_type& l):
          Base(t),
          value(v),
          location(l) {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, RegionListIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const RegionListIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, SelectorIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const SelectorIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, SelectorMaybeAdverbIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const SelectorMaybeAdverbIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, StringLitIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const StringLitIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, SymbolLitIndex&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const SymbolLitIndex& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, error_index<AnyExprIndex>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const error_index<AnyExprIndex>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, maybe<ClassNameIdentifierIndex>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const maybe<ClassNameIdentifierIndex>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, maybe<NamedIdentifierIndex>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const maybe<NamedIdentifierIndex>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        // User destructor.
        symbol_kind_type yykind = this->kind ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yykind)
        {
       default:
          break;
        }

        // Value type destructor.
switch (yykind)
    {
      case symbol_kind::S_ascii: // ascii
        value.template destroy< ASCIIIndex > ();
        break;

      case symbol_kind::S_129_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.template destroy< AccidentalIndex > ();
        break;

      case symbol_kind::S_adverb: // adverb
        value.template destroy< AdverbIndex > ();
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_93_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_95_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
          value.template destroy<AnyExprIndex>();
          break;

      case symbol_kind::S_method: // method
        value.template destroy< AnyMethodIndex > ();
        break;

      case symbol_kind::S_112_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.template destroy<AnyPossiblyLiteralIndex>();
          break;

      case symbol_kind::S_104_arguments_entries: // arguments.entries
          value.template destroy<ArgumentEntryIndex>();
          break;

      case symbol_kind::S_105_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_107_arguments_paren: // arguments.paren
      case symbol_kind::S_108_arguments_maybe_paren: // arguments.maybe_paren
          value.template destroy<ArgumentListIndex>();
          break;

      case symbol_kind::S_113_literal_array_contents: // literal.array.contents
      case symbol_kind::S_117_literal_array: // literal.array
          value.template destroy<ArrayIndex>();
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_76_members_opt: // members.opt
          value.template destroy<BigMemberListIndex>();
          break;

      case symbol_kind::S_86_block_contents: // block.contents
          value.template destroy<BlockContentsListIndex>();
          break;

      case symbol_kind::S_block: // block
        value.template destroy< BlockIndex > ();
        break;

      case symbol_kind::S_87_block_contents_item: // block.contents.item
          value.template destroy<BlockItemIndex>();
          break;

      case symbol_kind::S_84_block_opt_list: // block.opt_list
      case symbol_kind::S_85_block_list: // block.list
          value.template destroy<BlockListIndex>();
          break;

      case symbol_kind::S_boolean: // boolean
        value.template destroy< BooleanLitIndex > ();
        break;

      case symbol_kind::S_71_class_extension: // class.extension
          value.template destroy<ClassExtensionIndex>();
          break;

      case symbol_kind::S_class: // class
        value.template destroy< ClassIndex > ();
        break;

      case symbol_kind::S_go: // go
        value.template destroy< ClassListOrExprListIndex > ();
        break;

      case symbol_kind::S_67_classOrExtList_item: // classOrExtList.item
          value.template destroy<ClassOrExtensionIndex>();
          break;

      case symbol_kind::S_66_classOrExtList_list: // classOrExtList.list
          value.template destroy<ClassOrExtensionListIndex>();
          break;

      case symbol_kind::S_74_members_entry: // members.entry
          value.template destroy<DeclareAnyMemberListIndex>();
          break;

      case symbol_kind::S_109_variable_declarations_list_item: // variable_declarations.list.item
          value.template destroy<DeclareAnyVariableIndex>();
          break;

      case symbol_kind::S_100_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_101_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_103_argument_declarations_opt: // argument_declarations.opt
          value.template destroy<DeclareArgumentListIndex>();
          break;

      case symbol_kind::S_72_members_entry_item: // members.entry.item
          value.template destroy<DeclareMemberIndex>();
          break;

      case symbol_kind::S_73_members_entry_list: // members.entry.list
          value.template destroy<DeclareMemberListIndex>();
          break;

      case symbol_kind::S_110_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.template destroy< DeclareVariableListIndex > ();
        break;

      case symbol_kind::S_114_literal_dictionary_entry: // literal.dictionary.entry
          value.template destroy<DictionaryEntryIndex>();
          break;

      case symbol_kind::S_115_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_116_literal_dictionary: // literal.dictionary
          value.template destroy<DictionaryIndex>();
          break;

      case symbol_kind::S_127_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_128_float_raw: // float.raw
          value.template destroy<FloatLitIndex>();
          break;

      case symbol_kind::S_float: // float
        value.template destroy< FloatProducingIndex > ();
        break;

      case symbol_kind::S_generator: // generator
          value.template destroy<GeneratorAnyIndex>();
          break;

      case symbol_kind::S_99_generator_qual: // generator.qual
          value.template destroy<GeneratorQualAnyIndex>();
          break;

      case symbol_kind::S_98_generator_qual_list: // generator.qual.list
          value.template destroy<GeneratorQualListIndex>();
          break;

      case symbol_kind::S_integer: // integer
        value.template destroy< IntLitIndex > ();
        break;

      case symbol_kind::S_REGION_SEPARATOR: // REGION_SEPARATOR
      case symbol_kind::S_OPENCURLY: // OPENCURLY
      case symbol_kind::S_CLOSECURLY: // CLOSECURLY
      case symbol_kind::S_OPENSQUARE: // OPENSQUARE
      case symbol_kind::S_CLOSESQUARE: // CLOSESQUARE
      case symbol_kind::S_OPENPAREN: // OPENPAREN
      case symbol_kind::S_CLOSEPAREN: // CLOSEPAREN
      case symbol_kind::S_SEMICOLON: // SEMICOLON
      case symbol_kind::S_NONLOCALRETURN: // NONLOCALRETURN
      case symbol_kind::S_COMMA: // COMMA
      case symbol_kind::S_HASH: // HASH
      case symbol_kind::S_TILDE: // TILDE
      case symbol_kind::S_NAME: // NAME
      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_INTEGER_RADIX: // INTEGER_RADIX
      case symbol_kind::S_HEXADECIMAL: // HEXADECIMAL
      case symbol_kind::S_FLOAT: // FLOAT
      case symbol_kind::S_FLOAT_RADIX: // FLOAT_RADIX
      case symbol_kind::S_FLOAT_EXPONENT: // FLOAT_EXPONENT
      case symbol_kind::S_FLOAT_INF: // FLOAT_INF
      case symbol_kind::S_ACCIDENTAL_STEPS: // ACCIDENTAL_STEPS
      case symbol_kind::S_ACCIDENTAL_CENTS: // ACCIDENTAL_CENTS
      case symbol_kind::S_SYMBOL_QUOTE: // SYMBOL_QUOTE
      case symbol_kind::S_SYMBOL_SLASH: // SYMBOL_SLASH
      case symbol_kind::S_STRINGLINE: // STRINGLINE
      case symbol_kind::S_ASCII: // ASCII
      case symbol_kind::S_PRIMITIVENAME: // PRIMITIVENAME
      case symbol_kind::S_CLASSNAME: // CLASSNAME
      case symbol_kind::S_CURRYARG: // CURRYARG
      case symbol_kind::S_VAR: // VAR
      case symbol_kind::S_ARG: // ARG
      case symbol_kind::S_CLASSVAR: // CLASSVAR
      case symbol_kind::S_CONST: // CONST
      case symbol_kind::S_NIL: // NIL
      case symbol_kind::S_TRUE: // TRUE
      case symbol_kind::S_FALSE: // FALSE
      case symbol_kind::S_PI: // PI
      case symbol_kind::S_ELLIPSIS: // ELLIPSIS
      case symbol_kind::S_DOTDOT: // DOTDOT
      case symbol_kind::S_BEGINCLOSEDFUNC: // BEGINCLOSEDFUNC
      case symbol_kind::S_BADTOKEN: // BADTOKEN
      case symbol_kind::S_INTERPRET: // INTERPRET
      case symbol_kind::S_LEFTARROW: // LEFTARROW
      case symbol_kind::S_WHILE: // WHILE
      case symbol_kind::S_LEXER_ERROR: // LEXER_ERROR
      case symbol_kind::S_COLON: // COLON
      case symbol_kind::S_EQUALSSIGN: // EQUALSSIGN
      case symbol_kind::S_BINOP: // BINOP
      case symbol_kind::S_KEYBINOP: // KEYBINOP
      case symbol_kind::S_MINUS: // MINUS
      case symbol_kind::S_LESSTHAN: // LESSTHAN
      case symbol_kind::S_GREATERTHAN: // GREATERTHAN
      case symbol_kind::S_MULTIPLY: // MULTIPLY
      case symbol_kind::S_ADD: // ADD
      case symbol_kind::S_PIPE: // PIPE
      case symbol_kind::S_READWRITEVAR: // READWRITEVAR
      case symbol_kind::S_DOT: // DOT
      case symbol_kind::S_BACKTICK: // BACKTICK
      case symbol_kind::S_UMINUS: // UMINUS
        value.template destroy< LexerToken > ();
        break;

      case symbol_kind::S_78_method_base: // method.base
          value.template destroy<MethodIndex>();
          break;

      case symbol_kind::S_80_method_list: // method.list
      case symbol_kind::S_81_method_list_opt: // method.list.opt
          value.template destroy<MethodListIndex>();
          break;

      case symbol_kind::S_120_name_list: // name.list
          value.template destroy<NameListIndex>();
          break;

      case symbol_kind::S_name: // name
        value.template destroy< NamedIdentifierIndex > ();
        break;

      case symbol_kind::S_nil: // nil
        value.template destroy< NilLitIndex > ();
        break;

      case symbol_kind::S_accessor: // accessor
          value.template destroy<RWAccessorIndex>();
          break;

      case symbol_kind::S_region: // region
        value.template destroy< RegionListIndex > ();
        break;

      case symbol_kind::S_77_method_name: // method.name
      case symbol_kind::S_89_binary_op_raw: // binary_op.raw
      case symbol_kind::S_90_binary_op_no_adverb: // binary_op.no_adverb
          value.template destroy<SelectorIndex>();
          break;

      case symbol_kind::S_binary_op: // binary_op
        value.template destroy< SelectorMaybeAdverbIndex > ();
        break;

      case symbol_kind::S_string: // string
        value.template destroy< StringLitIndex > ();
        break;

      case symbol_kind::S_symbol: // symbol
        value.template destroy< SymbolLitIndex > ();
        break;

      case symbol_kind::S_64_region_item: // region.item
          value.template destroy<error_index<AnyExprIndex>>();
          break;

      case symbol_kind::S_69_class_super_opt: // class.super.opt
          value.template destroy<maybe<ClassNameIdentifierIndex>>();
          break;

      case symbol_kind::S_70_class_slot_opt: // class.slot.opt
          value.template destroy<maybe<NamedIdentifierIndex>>();
          break;

      default:
        break;
    }

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
    {
      /// Superclass.
      typedef basic_symbol<by_kind> super_type;

      /// Empty symbol.
      symbol_type () YY_NOEXCEPT {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type (token_kind_type (tok), std::move (l))
#else
      symbol_type (int tok, const location_type& l)
        : super_type (token_kind_type (tok), l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, LexerToken v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const LexerToken& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
    };

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

    // Implementation of make_symbol for each token kind.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYEOF (location_type l)
      {
        return symbol_type (token::TOKEN_YYEOF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYEOF (const location_type& l)
      {
        return symbol_type (token::TOKEN_YYEOF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYerror (location_type l)
      {
        return symbol_type (token::TOKEN_YYerror, std::move (l));
      }
#else
      static
      symbol_type
      make_YYerror (const location_type& l)
      {
        return symbol_type (token::TOKEN_YYerror, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYUNDEF (location_type l)
      {
        return symbol_type (token::TOKEN_YYUNDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYUNDEF (const location_type& l)
      {
        return symbol_type (token::TOKEN_YYUNDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_REGION_SEPARATOR (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_REGION_SEPARATOR, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_REGION_SEPARATOR (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_REGION_SEPARATOR, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OPENCURLY (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_OPENCURLY, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_OPENCURLY (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_OPENCURLY, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLOSECURLY (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_CLOSECURLY, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CLOSECURLY (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_CLOSECURLY, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OPENSQUARE (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_OPENSQUARE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_OPENSQUARE (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_OPENSQUARE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLOSESQUARE (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_CLOSESQUARE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CLOSESQUARE (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_CLOSESQUARE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OPENPAREN (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_OPENPAREN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_OPENPAREN (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_OPENPAREN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLOSEPAREN (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_CLOSEPAREN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CLOSEPAREN (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_CLOSEPAREN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SEMICOLON (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_SEMICOLON, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_SEMICOLON (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_SEMICOLON, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NONLOCALRETURN (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_NONLOCALRETURN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_NONLOCALRETURN (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_NONLOCALRETURN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COMMA (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_COMMA, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_COMMA (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_COMMA, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HASH (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_HASH, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_HASH (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_HASH, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TILDE (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_TILDE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TILDE (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_TILDE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NAME (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_NAME, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_NAME (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_NAME, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INTEGER (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_INTEGER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_INTEGER (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_INTEGER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INTEGER_RADIX (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_INTEGER_RADIX, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_INTEGER_RADIX (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_INTEGER_RADIX, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HEXADECIMAL (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_HEXADECIMAL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_HEXADECIMAL (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_HEXADECIMAL, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FLOAT (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_FLOAT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_FLOAT (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_FLOAT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FLOAT_RADIX (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_FLOAT_RADIX, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_FLOAT_RADIX (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_FLOAT_RADIX, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FLOAT_EXPONENT (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_FLOAT_EXPONENT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_FLOAT_EXPONENT (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_FLOAT_EXPONENT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FLOAT_INF (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_FLOAT_INF, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_FLOAT_INF (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_FLOAT_INF, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACCIDENTAL_STEPS (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_ACCIDENTAL_STEPS, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ACCIDENTAL_STEPS (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_ACCIDENTAL_STEPS, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACCIDENTAL_CENTS (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_ACCIDENTAL_CENTS, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ACCIDENTAL_CENTS (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_ACCIDENTAL_CENTS, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SYMBOL_QUOTE (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_SYMBOL_QUOTE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_SYMBOL_QUOTE (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_SYMBOL_QUOTE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SYMBOL_SLASH (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_SYMBOL_SLASH, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_SYMBOL_SLASH (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_SYMBOL_SLASH, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRINGLINE (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_STRINGLINE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_STRINGLINE (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_STRINGLINE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ASCII (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_ASCII, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ASCII (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_ASCII, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PRIMITIVENAME (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_PRIMITIVENAME, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_PRIMITIVENAME (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_PRIMITIVENAME, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLASSNAME (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_CLASSNAME, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CLASSNAME (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_CLASSNAME, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CURRYARG (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_CURRYARG, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CURRYARG (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_CURRYARG, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VAR (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_VAR, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_VAR (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_VAR, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ARG (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_ARG, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ARG (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_ARG, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLASSVAR (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_CLASSVAR, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CLASSVAR (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_CLASSVAR, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONST (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_CONST, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CONST (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_CONST, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NIL (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_NIL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_NIL (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_NIL, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TRUE (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_TRUE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TRUE (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_TRUE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FALSE (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_FALSE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_FALSE (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_FALSE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PI (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_PI, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_PI (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_PI, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ELLIPSIS (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_ELLIPSIS, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ELLIPSIS (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_ELLIPSIS, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DOTDOT (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_DOTDOT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_DOTDOT (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_DOTDOT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BEGINCLOSEDFUNC (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_BEGINCLOSEDFUNC, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_BEGINCLOSEDFUNC (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_BEGINCLOSEDFUNC, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BADTOKEN (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_BADTOKEN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_BADTOKEN (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_BADTOKEN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INTERPRET (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_INTERPRET, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_INTERPRET (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_INTERPRET, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LEFTARROW (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_LEFTARROW, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_LEFTARROW (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_LEFTARROW, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static symbol_type make_WHILE(LexerToken v, location_type l) {
          return symbol_type(token::TOKEN_WHILE, std::move(v), std::move(l));
      }
#else
      static symbol_type make_WHILE(const LexerToken& v, const location_type& l) {
          return symbol_type(token::TOKEN_WHILE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LEXER_ERROR (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_LEXER_ERROR, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_LEXER_ERROR (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_LEXER_ERROR, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COLON (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_COLON, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_COLON (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_COLON, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EQUALSSIGN (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_EQUALSSIGN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_EQUALSSIGN (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_EQUALSSIGN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BINOP (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_BINOP, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_BINOP (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_BINOP, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_KEYBINOP (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_KEYBINOP, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_KEYBINOP (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_KEYBINOP, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MINUS (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_MINUS, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_MINUS (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_MINUS, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LESSTHAN (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_LESSTHAN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_LESSTHAN (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_LESSTHAN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_GREATERTHAN (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_GREATERTHAN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_GREATERTHAN (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_GREATERTHAN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MULTIPLY (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_MULTIPLY, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_MULTIPLY (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_MULTIPLY, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ADD (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_ADD, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ADD (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_ADD, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PIPE (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_PIPE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_PIPE (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_PIPE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_READWRITEVAR (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_READWRITEVAR, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_READWRITEVAR (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_READWRITEVAR, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DOT (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_DOT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_DOT (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_DOT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BACKTICK (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_BACKTICK, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_BACKTICK (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_BACKTICK, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_UMINUS (LexerToken v, location_type l)
      {
        return symbol_type (token::TOKEN_UMINUS, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_UMINUS (const LexerToken& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_UMINUS, v, l);
      }
#endif


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
    enum {
        yylast_ = 2211, ///< Last index in yytable_.
        yynnts_ = 73, ///< Number of nonterminal symbols.
        yyfinal_ = 63 ///< Termination state number.
    };


    // User arguments.
    ParserContext& cxt;

  };


#line 7 "langutils/sc_parser/src/sc_grammar.y"
} } } // sc::ast::parser
#line 3610 "langutils/sc_parser/src/sc_grammar_parser.hpp"


#endif // !YY_YY_LANGUTILS_SC_PARSER_SRC_SC_GRAMMAR_PARSER_HPP_INCLUDED
