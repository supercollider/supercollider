// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

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

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

// "%code top" blocks.
#line 31 "langutils/sc_parser/src/sc_grammar.y"


#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/nodes.hpp"

#include "sc_grammar_parser.hpp"
#include "sc_grammar_impl.hpp"
#include "parser_context.hpp"


using namespace sc::ast;



#line 54 "langutils/sc_parser/src/sc_grammar_parser.cpp"




#include "sc_grammar_parser.hpp"




#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 7 "langutils/sc_parser/src/sc_grammar.y"
namespace sc { namespace ast { namespace parser {
#line 154 "langutils/sc_parser/src/sc_grammar_parser.cpp"

  /// Build a parser object.
  parser::parser (ParserContext& cxt_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      cxt (cxt_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/

  // basic_symbol.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->kind ())
    {
      case symbol_kind::S_ascii: // ascii
        value.copy< ASCIIIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_137_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.copy< AccidentalIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_adverb: // adverb
        value.copy< AdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_100_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_102_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
        value.copy< AnyExprIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_79_method_base: // method.base
      case symbol_kind::S_method: // method
        value.copy< AnyMethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_119_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
        value.copy< AnyPossiblyLiteralIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_symbol: // symbol
        value.copy< AnySymbolIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_111_arguments_entries: // arguments.entries
        value.copy< ArgumentEntryIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_112_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_114_arguments_paren: // arguments.paren
      case symbol_kind::S_115_arguments_maybe_paren: // arguments.maybe_paren
        value.copy< ArgumentListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_120_literal_array_contents: // literal.array.contents
      case symbol_kind::S_124_literal_array: // literal.array
        value.copy< ArrayIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_94_expr_dotrange: // expr.dotrange
      case symbol_kind::S_96_expr_base_range: // expr.base.range
        value.copy< AtMessageIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_77_members_opt: // members.opt
        value.copy< BigMemberListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_87_block_contents: // block.contents
        value.copy< BlockContentsListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_block: // block
        value.copy< BlockIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_88_block_contents_item: // block.contents.item
        value.copy< BlockItemIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_85_block_opt_list: // block.opt_list
      case symbol_kind::S_86_block_list: // block.list
        value.copy< BlockListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_boolean: // boolean
        value.copy< BooleanLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_72_class_extension: // class.extension
        value.copy< ClassExtensionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_class: // class
        value.copy< ClassIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_68_classOrExtList_item: // classOrExtList.item
        value.copy< ClassOrExtensionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_67_classOrExtList_list: // classOrExtList.list
        value.copy< ClassOrExtensionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_75_members_entry: // members.entry
        value.copy< DeclareAnyMemberListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_116_variable_declarations_list_item: // variable_declarations.list.item
        value.copy< DeclareAnyVariableIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_107_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_108_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_110_argument_declarations_opt: // argument_declarations.opt
        value.copy< DeclareArgumentListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_73_members_entry_item: // members.entry.item
        value.copy< DeclareMemberIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_74_members_entry_list: // members.entry.list
        value.copy< DeclareMemberListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_117_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.copy< DeclareVariableListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_121_literal_dictionary_entry: // literal.dictionary.entry
        value.copy< DictionaryEntryIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_122_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_123_literal_dictionary: // literal.dictionary
        value.copy< DictionaryIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_file_as_func: // file_as_func
        value.copy< FileAsFunctionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_135_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_136_float_raw: // float.raw
        value.copy< FloatLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_float: // float
        value.copy< FloatProducingIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_generator: // generator
        value.copy< GeneratorAnyIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_106_generator_qual: // generator.qual
        value.copy< GeneratorQualAnyIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_105_generator_qual_list: // generator.qual.list
        value.copy< GeneratorQualListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_go: // go
        value.copy< GoIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_integer: // integer
        value.copy< IntLitIndex > (YY_MOVE (that.value));
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
        value.copy< LexerToken > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_81_method_list: // method.list
      case symbol_kind::S_82_method_list_opt: // method.list.opt
        value.copy< MethodListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_127_name_list: // name.list
        value.copy< NameListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_128_name_pack: // name.pack
        value.copy< NamePackIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_name: // name
        value.copy< NamedIdentifierIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_nil: // nil
        value.copy< NilLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_accessor: // accessor
        value.copy< RWAccessorIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_95_expr_dotrange_assign: // expr.dotrange.assign
      case symbol_kind::S_97_expr_base_range_assign: // expr.base.range.assign
        value.copy< RangeAssignIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_99_range_generator: // range.generator
        value.copy< RangeGeneratorIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_range: // range
        value.copy< RangeIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_65_region_item: // region.item
        value.copy< RegionItemIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_region: // region
        value.copy< RegionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_78_method_name: // method.name
      case symbol_kind::S_90_binary_op_raw: // binary_op.raw
      case symbol_kind::S_91_binary_op_no_adverb: // binary_op.no_adverb
        value.copy< SelectorIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_binary_op: // binary_op
        value.copy< SelectorMaybeAdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_string: // string
        value.copy< StringLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_70_class_super_opt: // class.super.opt
        value.copy< maybe<ClassNameIdentifierIndex> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_71_class_slot_opt: // class.slot.opt
        value.copy< maybe<NamedIdentifierIndex> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  parser::symbol_kind_type
  parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_ascii: // ascii
        value.move< ASCIIIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_137_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.move< AccidentalIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_adverb: // adverb
        value.move< AdverbIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_100_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_102_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
        value.move< AnyExprIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_79_method_base: // method.base
      case symbol_kind::S_method: // method
        value.move< AnyMethodIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_119_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
        value.move< AnyPossiblyLiteralIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_symbol: // symbol
        value.move< AnySymbolIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_111_arguments_entries: // arguments.entries
        value.move< ArgumentEntryIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_112_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_114_arguments_paren: // arguments.paren
      case symbol_kind::S_115_arguments_maybe_paren: // arguments.maybe_paren
        value.move< ArgumentListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_120_literal_array_contents: // literal.array.contents
      case symbol_kind::S_124_literal_array: // literal.array
        value.move< ArrayIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_94_expr_dotrange: // expr.dotrange
      case symbol_kind::S_96_expr_base_range: // expr.base.range
        value.move< AtMessageIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_77_members_opt: // members.opt
        value.move< BigMemberListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_87_block_contents: // block.contents
        value.move< BlockContentsListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_block: // block
        value.move< BlockIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_88_block_contents_item: // block.contents.item
        value.move< BlockItemIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_85_block_opt_list: // block.opt_list
      case symbol_kind::S_86_block_list: // block.list
        value.move< BlockListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_boolean: // boolean
        value.move< BooleanLitIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_72_class_extension: // class.extension
        value.move< ClassExtensionIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_class: // class
        value.move< ClassIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_68_classOrExtList_item: // classOrExtList.item
        value.move< ClassOrExtensionIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_67_classOrExtList_list: // classOrExtList.list
        value.move< ClassOrExtensionListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_75_members_entry: // members.entry
        value.move< DeclareAnyMemberListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_116_variable_declarations_list_item: // variable_declarations.list.item
        value.move< DeclareAnyVariableIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_107_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_108_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_110_argument_declarations_opt: // argument_declarations.opt
        value.move< DeclareArgumentListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_73_members_entry_item: // members.entry.item
        value.move< DeclareMemberIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_74_members_entry_list: // members.entry.list
        value.move< DeclareMemberListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_117_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.move< DeclareVariableListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_121_literal_dictionary_entry: // literal.dictionary.entry
        value.move< DictionaryEntryIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_122_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_123_literal_dictionary: // literal.dictionary
        value.move< DictionaryIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_file_as_func: // file_as_func
        value.move< FileAsFunctionIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_135_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_136_float_raw: // float.raw
        value.move< FloatLitIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_float: // float
        value.move< FloatProducingIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_generator: // generator
        value.move< GeneratorAnyIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_106_generator_qual: // generator.qual
        value.move< GeneratorQualAnyIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_105_generator_qual_list: // generator.qual.list
        value.move< GeneratorQualListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_go: // go
        value.move< GoIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_integer: // integer
        value.move< IntLitIndex > (YY_MOVE (s.value));
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
        value.move< LexerToken > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_81_method_list: // method.list
      case symbol_kind::S_82_method_list_opt: // method.list.opt
        value.move< MethodListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_127_name_list: // name.list
        value.move< NameListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_128_name_pack: // name.pack
        value.move< NamePackIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_name: // name
        value.move< NamedIdentifierIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_nil: // nil
        value.move< NilLitIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_accessor: // accessor
        value.move< RWAccessorIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_95_expr_dotrange_assign: // expr.dotrange.assign
      case symbol_kind::S_97_expr_base_range_assign: // expr.base.range.assign
        value.move< RangeAssignIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_99_range_generator: // range.generator
        value.move< RangeGeneratorIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_range: // range
        value.move< RangeIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_65_region_item: // region.item
        value.move< RegionItemIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_region: // region
        value.move< RegionListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_78_method_name: // method.name
      case symbol_kind::S_90_binary_op_raw: // binary_op.raw
      case symbol_kind::S_91_binary_op_no_adverb: // binary_op.no_adverb
        value.move< SelectorIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_binary_op: // binary_op
        value.move< SelectorMaybeAdverbIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_string: // string
        value.move< StringLitIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_70_class_super_opt: // class.super.opt
        value.move< maybe<ClassNameIdentifierIndex> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_71_class_slot_opt: // class.slot.opt
        value.move< maybe<NamedIdentifierIndex> > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_kind.
  parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  void
  parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  parser::symbol_kind_type
  parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  parser::symbol_kind_type
  parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_ascii: // ascii
        value.YY_MOVE_OR_COPY< ASCIIIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_137_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.YY_MOVE_OR_COPY< AccidentalIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_adverb: // adverb
        value.YY_MOVE_OR_COPY< AdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_100_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_102_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
        value.YY_MOVE_OR_COPY< AnyExprIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_79_method_base: // method.base
      case symbol_kind::S_method: // method
        value.YY_MOVE_OR_COPY< AnyMethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_119_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
        value.YY_MOVE_OR_COPY< AnyPossiblyLiteralIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_symbol: // symbol
        value.YY_MOVE_OR_COPY< AnySymbolIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_111_arguments_entries: // arguments.entries
        value.YY_MOVE_OR_COPY< ArgumentEntryIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_112_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_114_arguments_paren: // arguments.paren
      case symbol_kind::S_115_arguments_maybe_paren: // arguments.maybe_paren
        value.YY_MOVE_OR_COPY< ArgumentListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_120_literal_array_contents: // literal.array.contents
      case symbol_kind::S_124_literal_array: // literal.array
        value.YY_MOVE_OR_COPY< ArrayIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_94_expr_dotrange: // expr.dotrange
      case symbol_kind::S_96_expr_base_range: // expr.base.range
        value.YY_MOVE_OR_COPY< AtMessageIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_77_members_opt: // members.opt
        value.YY_MOVE_OR_COPY< BigMemberListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_87_block_contents: // block.contents
        value.YY_MOVE_OR_COPY< BlockContentsListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_block: // block
        value.YY_MOVE_OR_COPY< BlockIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_88_block_contents_item: // block.contents.item
        value.YY_MOVE_OR_COPY< BlockItemIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_85_block_opt_list: // block.opt_list
      case symbol_kind::S_86_block_list: // block.list
        value.YY_MOVE_OR_COPY< BlockListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_boolean: // boolean
        value.YY_MOVE_OR_COPY< BooleanLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_72_class_extension: // class.extension
        value.YY_MOVE_OR_COPY< ClassExtensionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_class: // class
        value.YY_MOVE_OR_COPY< ClassIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_68_classOrExtList_item: // classOrExtList.item
        value.YY_MOVE_OR_COPY< ClassOrExtensionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_67_classOrExtList_list: // classOrExtList.list
        value.YY_MOVE_OR_COPY< ClassOrExtensionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_75_members_entry: // members.entry
        value.YY_MOVE_OR_COPY< DeclareAnyMemberListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_116_variable_declarations_list_item: // variable_declarations.list.item
        value.YY_MOVE_OR_COPY< DeclareAnyVariableIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_107_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_108_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_110_argument_declarations_opt: // argument_declarations.opt
        value.YY_MOVE_OR_COPY< DeclareArgumentListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_73_members_entry_item: // members.entry.item
        value.YY_MOVE_OR_COPY< DeclareMemberIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_74_members_entry_list: // members.entry.list
        value.YY_MOVE_OR_COPY< DeclareMemberListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_117_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.YY_MOVE_OR_COPY< DeclareVariableListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_121_literal_dictionary_entry: // literal.dictionary.entry
        value.YY_MOVE_OR_COPY< DictionaryEntryIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_122_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_123_literal_dictionary: // literal.dictionary
        value.YY_MOVE_OR_COPY< DictionaryIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_file_as_func: // file_as_func
        value.YY_MOVE_OR_COPY< FileAsFunctionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_135_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_136_float_raw: // float.raw
        value.YY_MOVE_OR_COPY< FloatLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_float: // float
        value.YY_MOVE_OR_COPY< FloatProducingIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_generator: // generator
        value.YY_MOVE_OR_COPY< GeneratorAnyIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_106_generator_qual: // generator.qual
        value.YY_MOVE_OR_COPY< GeneratorQualAnyIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_105_generator_qual_list: // generator.qual.list
        value.YY_MOVE_OR_COPY< GeneratorQualListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_go: // go
        value.YY_MOVE_OR_COPY< GoIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_integer: // integer
        value.YY_MOVE_OR_COPY< IntLitIndex > (YY_MOVE (that.value));
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
        value.YY_MOVE_OR_COPY< LexerToken > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_81_method_list: // method.list
      case symbol_kind::S_82_method_list_opt: // method.list.opt
        value.YY_MOVE_OR_COPY< MethodListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_127_name_list: // name.list
        value.YY_MOVE_OR_COPY< NameListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_128_name_pack: // name.pack
        value.YY_MOVE_OR_COPY< NamePackIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_name: // name
        value.YY_MOVE_OR_COPY< NamedIdentifierIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_nil: // nil
        value.YY_MOVE_OR_COPY< NilLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_accessor: // accessor
        value.YY_MOVE_OR_COPY< RWAccessorIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_95_expr_dotrange_assign: // expr.dotrange.assign
      case symbol_kind::S_97_expr_base_range_assign: // expr.base.range.assign
        value.YY_MOVE_OR_COPY< RangeAssignIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_99_range_generator: // range.generator
        value.YY_MOVE_OR_COPY< RangeGeneratorIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_range: // range
        value.YY_MOVE_OR_COPY< RangeIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_65_region_item: // region.item
        value.YY_MOVE_OR_COPY< RegionItemIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_region: // region
        value.YY_MOVE_OR_COPY< RegionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_78_method_name: // method.name
      case symbol_kind::S_90_binary_op_raw: // binary_op.raw
      case symbol_kind::S_91_binary_op_no_adverb: // binary_op.no_adverb
        value.YY_MOVE_OR_COPY< SelectorIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_binary_op: // binary_op
        value.YY_MOVE_OR_COPY< SelectorMaybeAdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_string: // string
        value.YY_MOVE_OR_COPY< StringLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_70_class_super_opt: // class.super.opt
        value.YY_MOVE_OR_COPY< maybe<ClassNameIdentifierIndex> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_71_class_slot_opt: // class.slot.opt
        value.YY_MOVE_OR_COPY< maybe<NamedIdentifierIndex> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_ascii: // ascii
        value.move< ASCIIIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_137_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.move< AccidentalIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_adverb: // adverb
        value.move< AdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_100_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_102_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
        value.move< AnyExprIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_79_method_base: // method.base
      case symbol_kind::S_method: // method
        value.move< AnyMethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_119_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
        value.move< AnyPossiblyLiteralIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_symbol: // symbol
        value.move< AnySymbolIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_111_arguments_entries: // arguments.entries
        value.move< ArgumentEntryIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_112_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_114_arguments_paren: // arguments.paren
      case symbol_kind::S_115_arguments_maybe_paren: // arguments.maybe_paren
        value.move< ArgumentListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_120_literal_array_contents: // literal.array.contents
      case symbol_kind::S_124_literal_array: // literal.array
        value.move< ArrayIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_94_expr_dotrange: // expr.dotrange
      case symbol_kind::S_96_expr_base_range: // expr.base.range
        value.move< AtMessageIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_77_members_opt: // members.opt
        value.move< BigMemberListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_87_block_contents: // block.contents
        value.move< BlockContentsListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_block: // block
        value.move< BlockIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_88_block_contents_item: // block.contents.item
        value.move< BlockItemIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_85_block_opt_list: // block.opt_list
      case symbol_kind::S_86_block_list: // block.list
        value.move< BlockListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_boolean: // boolean
        value.move< BooleanLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_72_class_extension: // class.extension
        value.move< ClassExtensionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_class: // class
        value.move< ClassIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_68_classOrExtList_item: // classOrExtList.item
        value.move< ClassOrExtensionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_67_classOrExtList_list: // classOrExtList.list
        value.move< ClassOrExtensionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_75_members_entry: // members.entry
        value.move< DeclareAnyMemberListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_116_variable_declarations_list_item: // variable_declarations.list.item
        value.move< DeclareAnyVariableIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_107_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_108_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_110_argument_declarations_opt: // argument_declarations.opt
        value.move< DeclareArgumentListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_73_members_entry_item: // members.entry.item
        value.move< DeclareMemberIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_74_members_entry_list: // members.entry.list
        value.move< DeclareMemberListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_117_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.move< DeclareVariableListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_121_literal_dictionary_entry: // literal.dictionary.entry
        value.move< DictionaryEntryIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_122_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_123_literal_dictionary: // literal.dictionary
        value.move< DictionaryIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_file_as_func: // file_as_func
        value.move< FileAsFunctionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_135_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_136_float_raw: // float.raw
        value.move< FloatLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_float: // float
        value.move< FloatProducingIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_generator: // generator
        value.move< GeneratorAnyIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_106_generator_qual: // generator.qual
        value.move< GeneratorQualAnyIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_105_generator_qual_list: // generator.qual.list
        value.move< GeneratorQualListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_go: // go
        value.move< GoIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_integer: // integer
        value.move< IntLitIndex > (YY_MOVE (that.value));
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
        value.move< LexerToken > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_81_method_list: // method.list
      case symbol_kind::S_82_method_list_opt: // method.list.opt
        value.move< MethodListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_127_name_list: // name.list
        value.move< NameListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_128_name_pack: // name.pack
        value.move< NamePackIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_name: // name
        value.move< NamedIdentifierIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_nil: // nil
        value.move< NilLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_accessor: // accessor
        value.move< RWAccessorIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_95_expr_dotrange_assign: // expr.dotrange.assign
      case symbol_kind::S_97_expr_base_range_assign: // expr.base.range.assign
        value.move< RangeAssignIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_99_range_generator: // range.generator
        value.move< RangeGeneratorIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_range: // range
        value.move< RangeIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_65_region_item: // region.item
        value.move< RegionItemIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_region: // region
        value.move< RegionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_78_method_name: // method.name
      case symbol_kind::S_90_binary_op_raw: // binary_op.raw
      case symbol_kind::S_91_binary_op_no_adverb: // binary_op.no_adverb
        value.move< SelectorIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_binary_op: // binary_op
        value.move< SelectorMaybeAdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_string: // string
        value.move< StringLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_70_class_super_opt: // class.super.opt
        value.move< maybe<ClassNameIdentifierIndex> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_71_class_slot_opt: // class.slot.opt
        value.move< maybe<NamedIdentifierIndex> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_ascii: // ascii
        value.copy< ASCIIIndex > (that.value);
        break;

      case symbol_kind::S_137_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.copy< AccidentalIndex > (that.value);
        break;

      case symbol_kind::S_adverb: // adverb
        value.copy< AdverbIndex > (that.value);
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_100_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_102_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
        value.copy< AnyExprIndex > (that.value);
        break;

      case symbol_kind::S_79_method_base: // method.base
      case symbol_kind::S_method: // method
        value.copy< AnyMethodIndex > (that.value);
        break;

      case symbol_kind::S_119_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
        value.copy< AnyPossiblyLiteralIndex > (that.value);
        break;

      case symbol_kind::S_symbol: // symbol
        value.copy< AnySymbolIndex > (that.value);
        break;

      case symbol_kind::S_111_arguments_entries: // arguments.entries
        value.copy< ArgumentEntryIndex > (that.value);
        break;

      case symbol_kind::S_112_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_114_arguments_paren: // arguments.paren
      case symbol_kind::S_115_arguments_maybe_paren: // arguments.maybe_paren
        value.copy< ArgumentListIndex > (that.value);
        break;

      case symbol_kind::S_120_literal_array_contents: // literal.array.contents
      case symbol_kind::S_124_literal_array: // literal.array
        value.copy< ArrayIndex > (that.value);
        break;

      case symbol_kind::S_94_expr_dotrange: // expr.dotrange
      case symbol_kind::S_96_expr_base_range: // expr.base.range
        value.copy< AtMessageIndex > (that.value);
        break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_77_members_opt: // members.opt
        value.copy< BigMemberListIndex > (that.value);
        break;

      case symbol_kind::S_87_block_contents: // block.contents
        value.copy< BlockContentsListIndex > (that.value);
        break;

      case symbol_kind::S_block: // block
        value.copy< BlockIndex > (that.value);
        break;

      case symbol_kind::S_88_block_contents_item: // block.contents.item
        value.copy< BlockItemIndex > (that.value);
        break;

      case symbol_kind::S_85_block_opt_list: // block.opt_list
      case symbol_kind::S_86_block_list: // block.list
        value.copy< BlockListIndex > (that.value);
        break;

      case symbol_kind::S_boolean: // boolean
        value.copy< BooleanLitIndex > (that.value);
        break;

      case symbol_kind::S_72_class_extension: // class.extension
        value.copy< ClassExtensionIndex > (that.value);
        break;

      case symbol_kind::S_class: // class
        value.copy< ClassIndex > (that.value);
        break;

      case symbol_kind::S_68_classOrExtList_item: // classOrExtList.item
        value.copy< ClassOrExtensionIndex > (that.value);
        break;

      case symbol_kind::S_67_classOrExtList_list: // classOrExtList.list
        value.copy< ClassOrExtensionListIndex > (that.value);
        break;

      case symbol_kind::S_75_members_entry: // members.entry
        value.copy< DeclareAnyMemberListIndex > (that.value);
        break;

      case symbol_kind::S_116_variable_declarations_list_item: // variable_declarations.list.item
        value.copy< DeclareAnyVariableIndex > (that.value);
        break;

      case symbol_kind::S_107_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_108_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_110_argument_declarations_opt: // argument_declarations.opt
        value.copy< DeclareArgumentListIndex > (that.value);
        break;

      case symbol_kind::S_73_members_entry_item: // members.entry.item
        value.copy< DeclareMemberIndex > (that.value);
        break;

      case symbol_kind::S_74_members_entry_list: // members.entry.list
        value.copy< DeclareMemberListIndex > (that.value);
        break;

      case symbol_kind::S_117_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.copy< DeclareVariableListIndex > (that.value);
        break;

      case symbol_kind::S_121_literal_dictionary_entry: // literal.dictionary.entry
        value.copy< DictionaryEntryIndex > (that.value);
        break;

      case symbol_kind::S_122_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_123_literal_dictionary: // literal.dictionary
        value.copy< DictionaryIndex > (that.value);
        break;

      case symbol_kind::S_file_as_func: // file_as_func
        value.copy< FileAsFunctionIndex > (that.value);
        break;

      case symbol_kind::S_135_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_136_float_raw: // float.raw
        value.copy< FloatLitIndex > (that.value);
        break;

      case symbol_kind::S_float: // float
        value.copy< FloatProducingIndex > (that.value);
        break;

      case symbol_kind::S_generator: // generator
        value.copy< GeneratorAnyIndex > (that.value);
        break;

      case symbol_kind::S_106_generator_qual: // generator.qual
        value.copy< GeneratorQualAnyIndex > (that.value);
        break;

      case symbol_kind::S_105_generator_qual_list: // generator.qual.list
        value.copy< GeneratorQualListIndex > (that.value);
        break;

      case symbol_kind::S_go: // go
        value.copy< GoIndex > (that.value);
        break;

      case symbol_kind::S_integer: // integer
        value.copy< IntLitIndex > (that.value);
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
        value.copy< LexerToken > (that.value);
        break;

      case symbol_kind::S_81_method_list: // method.list
      case symbol_kind::S_82_method_list_opt: // method.list.opt
        value.copy< MethodListIndex > (that.value);
        break;

      case symbol_kind::S_127_name_list: // name.list
        value.copy< NameListIndex > (that.value);
        break;

      case symbol_kind::S_128_name_pack: // name.pack
        value.copy< NamePackIndex > (that.value);
        break;

      case symbol_kind::S_name: // name
        value.copy< NamedIdentifierIndex > (that.value);
        break;

      case symbol_kind::S_nil: // nil
        value.copy< NilLitIndex > (that.value);
        break;

      case symbol_kind::S_accessor: // accessor
        value.copy< RWAccessorIndex > (that.value);
        break;

      case symbol_kind::S_95_expr_dotrange_assign: // expr.dotrange.assign
      case symbol_kind::S_97_expr_base_range_assign: // expr.base.range.assign
        value.copy< RangeAssignIndex > (that.value);
        break;

      case symbol_kind::S_99_range_generator: // range.generator
        value.copy< RangeGeneratorIndex > (that.value);
        break;

      case symbol_kind::S_range: // range
        value.copy< RangeIndex > (that.value);
        break;

      case symbol_kind::S_65_region_item: // region.item
        value.copy< RegionItemIndex > (that.value);
        break;

      case symbol_kind::S_region: // region
        value.copy< RegionListIndex > (that.value);
        break;

      case symbol_kind::S_78_method_name: // method.name
      case symbol_kind::S_90_binary_op_raw: // binary_op.raw
      case symbol_kind::S_91_binary_op_no_adverb: // binary_op.no_adverb
        value.copy< SelectorIndex > (that.value);
        break;

      case symbol_kind::S_binary_op: // binary_op
        value.copy< SelectorMaybeAdverbIndex > (that.value);
        break;

      case symbol_kind::S_string: // string
        value.copy< StringLitIndex > (that.value);
        break;

      case symbol_kind::S_70_class_super_opt: // class.super.opt
        value.copy< maybe<ClassNameIdentifierIndex> > (that.value);
        break;

      case symbol_kind::S_71_class_slot_opt: // class.slot.opt
        value.copy< maybe<NamedIdentifierIndex> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_ascii: // ascii
        value.move< ASCIIIndex > (that.value);
        break;

      case symbol_kind::S_137_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.move< AccidentalIndex > (that.value);
        break;

      case symbol_kind::S_adverb: // adverb
        value.move< AdverbIndex > (that.value);
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_100_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_102_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
        value.move< AnyExprIndex > (that.value);
        break;

      case symbol_kind::S_79_method_base: // method.base
      case symbol_kind::S_method: // method
        value.move< AnyMethodIndex > (that.value);
        break;

      case symbol_kind::S_119_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
        value.move< AnyPossiblyLiteralIndex > (that.value);
        break;

      case symbol_kind::S_symbol: // symbol
        value.move< AnySymbolIndex > (that.value);
        break;

      case symbol_kind::S_111_arguments_entries: // arguments.entries
        value.move< ArgumentEntryIndex > (that.value);
        break;

      case symbol_kind::S_112_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_114_arguments_paren: // arguments.paren
      case symbol_kind::S_115_arguments_maybe_paren: // arguments.maybe_paren
        value.move< ArgumentListIndex > (that.value);
        break;

      case symbol_kind::S_120_literal_array_contents: // literal.array.contents
      case symbol_kind::S_124_literal_array: // literal.array
        value.move< ArrayIndex > (that.value);
        break;

      case symbol_kind::S_94_expr_dotrange: // expr.dotrange
      case symbol_kind::S_96_expr_base_range: // expr.base.range
        value.move< AtMessageIndex > (that.value);
        break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_77_members_opt: // members.opt
        value.move< BigMemberListIndex > (that.value);
        break;

      case symbol_kind::S_87_block_contents: // block.contents
        value.move< BlockContentsListIndex > (that.value);
        break;

      case symbol_kind::S_block: // block
        value.move< BlockIndex > (that.value);
        break;

      case symbol_kind::S_88_block_contents_item: // block.contents.item
        value.move< BlockItemIndex > (that.value);
        break;

      case symbol_kind::S_85_block_opt_list: // block.opt_list
      case symbol_kind::S_86_block_list: // block.list
        value.move< BlockListIndex > (that.value);
        break;

      case symbol_kind::S_boolean: // boolean
        value.move< BooleanLitIndex > (that.value);
        break;

      case symbol_kind::S_72_class_extension: // class.extension
        value.move< ClassExtensionIndex > (that.value);
        break;

      case symbol_kind::S_class: // class
        value.move< ClassIndex > (that.value);
        break;

      case symbol_kind::S_68_classOrExtList_item: // classOrExtList.item
        value.move< ClassOrExtensionIndex > (that.value);
        break;

      case symbol_kind::S_67_classOrExtList_list: // classOrExtList.list
        value.move< ClassOrExtensionListIndex > (that.value);
        break;

      case symbol_kind::S_75_members_entry: // members.entry
        value.move< DeclareAnyMemberListIndex > (that.value);
        break;

      case symbol_kind::S_116_variable_declarations_list_item: // variable_declarations.list.item
        value.move< DeclareAnyVariableIndex > (that.value);
        break;

      case symbol_kind::S_107_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_108_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_110_argument_declarations_opt: // argument_declarations.opt
        value.move< DeclareArgumentListIndex > (that.value);
        break;

      case symbol_kind::S_73_members_entry_item: // members.entry.item
        value.move< DeclareMemberIndex > (that.value);
        break;

      case symbol_kind::S_74_members_entry_list: // members.entry.list
        value.move< DeclareMemberListIndex > (that.value);
        break;

      case symbol_kind::S_117_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.move< DeclareVariableListIndex > (that.value);
        break;

      case symbol_kind::S_121_literal_dictionary_entry: // literal.dictionary.entry
        value.move< DictionaryEntryIndex > (that.value);
        break;

      case symbol_kind::S_122_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_123_literal_dictionary: // literal.dictionary
        value.move< DictionaryIndex > (that.value);
        break;

      case symbol_kind::S_file_as_func: // file_as_func
        value.move< FileAsFunctionIndex > (that.value);
        break;

      case symbol_kind::S_135_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_136_float_raw: // float.raw
        value.move< FloatLitIndex > (that.value);
        break;

      case symbol_kind::S_float: // float
        value.move< FloatProducingIndex > (that.value);
        break;

      case symbol_kind::S_generator: // generator
        value.move< GeneratorAnyIndex > (that.value);
        break;

      case symbol_kind::S_106_generator_qual: // generator.qual
        value.move< GeneratorQualAnyIndex > (that.value);
        break;

      case symbol_kind::S_105_generator_qual_list: // generator.qual.list
        value.move< GeneratorQualListIndex > (that.value);
        break;

      case symbol_kind::S_go: // go
        value.move< GoIndex > (that.value);
        break;

      case symbol_kind::S_integer: // integer
        value.move< IntLitIndex > (that.value);
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
        value.move< LexerToken > (that.value);
        break;

      case symbol_kind::S_81_method_list: // method.list
      case symbol_kind::S_82_method_list_opt: // method.list.opt
        value.move< MethodListIndex > (that.value);
        break;

      case symbol_kind::S_127_name_list: // name.list
        value.move< NameListIndex > (that.value);
        break;

      case symbol_kind::S_128_name_pack: // name.pack
        value.move< NamePackIndex > (that.value);
        break;

      case symbol_kind::S_name: // name
        value.move< NamedIdentifierIndex > (that.value);
        break;

      case symbol_kind::S_nil: // nil
        value.move< NilLitIndex > (that.value);
        break;

      case symbol_kind::S_accessor: // accessor
        value.move< RWAccessorIndex > (that.value);
        break;

      case symbol_kind::S_95_expr_dotrange_assign: // expr.dotrange.assign
      case symbol_kind::S_97_expr_base_range_assign: // expr.base.range.assign
        value.move< RangeAssignIndex > (that.value);
        break;

      case symbol_kind::S_99_range_generator: // range.generator
        value.move< RangeGeneratorIndex > (that.value);
        break;

      case symbol_kind::S_range: // range
        value.move< RangeIndex > (that.value);
        break;

      case symbol_kind::S_65_region_item: // region.item
        value.move< RegionItemIndex > (that.value);
        break;

      case symbol_kind::S_region: // region
        value.move< RegionListIndex > (that.value);
        break;

      case symbol_kind::S_78_method_name: // method.name
      case symbol_kind::S_90_binary_op_raw: // binary_op.raw
      case symbol_kind::S_91_binary_op_no_adverb: // binary_op.no_adverb
        value.move< SelectorIndex > (that.value);
        break;

      case symbol_kind::S_binary_op: // binary_op
        value.move< SelectorMaybeAdverbIndex > (that.value);
        break;

      case symbol_kind::S_string: // string
        value.move< StringLitIndex > (that.value);
        break;

      case symbol_kind::S_70_class_super_opt: // class.super.opt
        value.move< maybe<ClassNameIdentifierIndex> > (that.value);
        break;

      case symbol_kind::S_71_class_slot_opt: // class.slot.opt
        value.move< maybe<NamedIdentifierIndex> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value, &yyla.location, cxt));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_ascii: // ascii
        yylhs.value.emplace< ASCIIIndex > ();
        break;

      case symbol_kind::S_137_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        yylhs.value.emplace< AccidentalIndex > ();
        break;

      case symbol_kind::S_adverb: // adverb
        yylhs.value.emplace< AdverbIndex > ();
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_100_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_102_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
        yylhs.value.emplace< AnyExprIndex > ();
        break;

      case symbol_kind::S_79_method_base: // method.base
      case symbol_kind::S_method: // method
        yylhs.value.emplace< AnyMethodIndex > ();
        break;

      case symbol_kind::S_119_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
        yylhs.value.emplace< AnyPossiblyLiteralIndex > ();
        break;

      case symbol_kind::S_symbol: // symbol
        yylhs.value.emplace< AnySymbolIndex > ();
        break;

      case symbol_kind::S_111_arguments_entries: // arguments.entries
        yylhs.value.emplace< ArgumentEntryIndex > ();
        break;

      case symbol_kind::S_112_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_114_arguments_paren: // arguments.paren
      case symbol_kind::S_115_arguments_maybe_paren: // arguments.maybe_paren
        yylhs.value.emplace< ArgumentListIndex > ();
        break;

      case symbol_kind::S_120_literal_array_contents: // literal.array.contents
      case symbol_kind::S_124_literal_array: // literal.array
        yylhs.value.emplace< ArrayIndex > ();
        break;

      case symbol_kind::S_94_expr_dotrange: // expr.dotrange
      case symbol_kind::S_96_expr_base_range: // expr.base.range
        yylhs.value.emplace< AtMessageIndex > ();
        break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_77_members_opt: // members.opt
        yylhs.value.emplace< BigMemberListIndex > ();
        break;

      case symbol_kind::S_87_block_contents: // block.contents
        yylhs.value.emplace< BlockContentsListIndex > ();
        break;

      case symbol_kind::S_block: // block
        yylhs.value.emplace< BlockIndex > ();
        break;

      case symbol_kind::S_88_block_contents_item: // block.contents.item
        yylhs.value.emplace< BlockItemIndex > ();
        break;

      case symbol_kind::S_85_block_opt_list: // block.opt_list
      case symbol_kind::S_86_block_list: // block.list
        yylhs.value.emplace< BlockListIndex > ();
        break;

      case symbol_kind::S_boolean: // boolean
        yylhs.value.emplace< BooleanLitIndex > ();
        break;

      case symbol_kind::S_72_class_extension: // class.extension
        yylhs.value.emplace< ClassExtensionIndex > ();
        break;

      case symbol_kind::S_class: // class
        yylhs.value.emplace< ClassIndex > ();
        break;

      case symbol_kind::S_68_classOrExtList_item: // classOrExtList.item
        yylhs.value.emplace< ClassOrExtensionIndex > ();
        break;

      case symbol_kind::S_67_classOrExtList_list: // classOrExtList.list
        yylhs.value.emplace< ClassOrExtensionListIndex > ();
        break;

      case symbol_kind::S_75_members_entry: // members.entry
        yylhs.value.emplace< DeclareAnyMemberListIndex > ();
        break;

      case symbol_kind::S_116_variable_declarations_list_item: // variable_declarations.list.item
        yylhs.value.emplace< DeclareAnyVariableIndex > ();
        break;

      case symbol_kind::S_107_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_108_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_110_argument_declarations_opt: // argument_declarations.opt
        yylhs.value.emplace< DeclareArgumentListIndex > ();
        break;

      case symbol_kind::S_73_members_entry_item: // members.entry.item
        yylhs.value.emplace< DeclareMemberIndex > ();
        break;

      case symbol_kind::S_74_members_entry_list: // members.entry.list
        yylhs.value.emplace< DeclareMemberListIndex > ();
        break;

      case symbol_kind::S_117_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        yylhs.value.emplace< DeclareVariableListIndex > ();
        break;

      case symbol_kind::S_121_literal_dictionary_entry: // literal.dictionary.entry
        yylhs.value.emplace< DictionaryEntryIndex > ();
        break;

      case symbol_kind::S_122_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_123_literal_dictionary: // literal.dictionary
        yylhs.value.emplace< DictionaryIndex > ();
        break;

      case symbol_kind::S_file_as_func: // file_as_func
        yylhs.value.emplace< FileAsFunctionIndex > ();
        break;

      case symbol_kind::S_135_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_136_float_raw: // float.raw
        yylhs.value.emplace< FloatLitIndex > ();
        break;

      case symbol_kind::S_float: // float
        yylhs.value.emplace< FloatProducingIndex > ();
        break;

      case symbol_kind::S_generator: // generator
        yylhs.value.emplace< GeneratorAnyIndex > ();
        break;

      case symbol_kind::S_106_generator_qual: // generator.qual
        yylhs.value.emplace< GeneratorQualAnyIndex > ();
        break;

      case symbol_kind::S_105_generator_qual_list: // generator.qual.list
        yylhs.value.emplace< GeneratorQualListIndex > ();
        break;

      case symbol_kind::S_go: // go
        yylhs.value.emplace< GoIndex > ();
        break;

      case symbol_kind::S_integer: // integer
        yylhs.value.emplace< IntLitIndex > ();
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
        yylhs.value.emplace< LexerToken > ();
        break;

      case symbol_kind::S_81_method_list: // method.list
      case symbol_kind::S_82_method_list_opt: // method.list.opt
        yylhs.value.emplace< MethodListIndex > ();
        break;

      case symbol_kind::S_127_name_list: // name.list
        yylhs.value.emplace< NameListIndex > ();
        break;

      case symbol_kind::S_128_name_pack: // name.pack
        yylhs.value.emplace< NamePackIndex > ();
        break;

      case symbol_kind::S_name: // name
        yylhs.value.emplace< NamedIdentifierIndex > ();
        break;

      case symbol_kind::S_nil: // nil
        yylhs.value.emplace< NilLitIndex > ();
        break;

      case symbol_kind::S_accessor: // accessor
        yylhs.value.emplace< RWAccessorIndex > ();
        break;

      case symbol_kind::S_95_expr_dotrange_assign: // expr.dotrange.assign
      case symbol_kind::S_97_expr_base_range_assign: // expr.base.range.assign
        yylhs.value.emplace< RangeAssignIndex > ();
        break;

      case symbol_kind::S_99_range_generator: // range.generator
        yylhs.value.emplace< RangeGeneratorIndex > ();
        break;

      case symbol_kind::S_range: // range
        yylhs.value.emplace< RangeIndex > ();
        break;

      case symbol_kind::S_65_region_item: // region.item
        yylhs.value.emplace< RegionItemIndex > ();
        break;

      case symbol_kind::S_region: // region
        yylhs.value.emplace< RegionListIndex > ();
        break;

      case symbol_kind::S_78_method_name: // method.name
      case symbol_kind::S_90_binary_op_raw: // binary_op.raw
      case symbol_kind::S_91_binary_op_no_adverb: // binary_op.no_adverb
        yylhs.value.emplace< SelectorIndex > ();
        break;

      case symbol_kind::S_binary_op: // binary_op
        yylhs.value.emplace< SelectorMaybeAdverbIndex > ();
        break;

      case symbol_kind::S_string: // string
        yylhs.value.emplace< StringLitIndex > ();
        break;

      case symbol_kind::S_70_class_super_opt: // class.super.opt
        yylhs.value.emplace< maybe<ClassNameIdentifierIndex> > ();
        break;

      case symbol_kind::S_71_class_slot_opt: // class.slot.opt
        yylhs.value.emplace< maybe<NamedIdentifierIndex> > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // go: region semicolon.opt $end
#line 174 "langutils/sc_parser/src/sc_grammar.y"
                                     { yylhs.value.as < GoIndex > () = cxt.graph->assign_root(yystack_[2].value.as < RegionListIndex > ()); }
#line 2729 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 3: // go: INTERPRET $end
#line 175 "langutils/sc_parser/src/sc_grammar.y"
                          { yylhs.value.as < GoIndex > () = cxt.graph->assign_root(cxt.create(RegionList{}, yylhs.location)); }
#line 2735 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 4: // go: file_as_func
#line 176 "langutils/sc_parser/src/sc_grammar.y"
                       { yylhs.value.as < GoIndex > () = cxt.graph->assign_root(yystack_[0].value.as < FileAsFunctionIndex > ()); }
#line 2741 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 5: // go: classOrExtList.list $end
#line 177 "langutils/sc_parser/src/sc_grammar.y"
                                              { yylhs.value.as < GoIndex > () = cxt.graph->assign_root(yystack_[1].value.as < ClassOrExtensionListIndex > ()); }
#line 2747 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 6: // go: %empty
#line 178 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < GoIndex > () = cxt.graph->assign_root(cxt.create(ClassOrExtensionList{}, yylhs.location));}
#line 2753 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 7: // file_as_func: INTERPRET argument_declarations block.contents semicolon.opt $end
#line 187 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < FileAsFunctionIndex > () = cxt.create(FileAsFunction{}, yylhs.location, yystack_[3].value.as < DeclareArgumentListIndex > (), yystack_[2].value.as < BlockContentsListIndex > ()); }
#line 2759 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 8: // file_as_func: INTERPRET variable_declarations SEMICOLON block.contents semicolon.opt $end
#line 190 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < FileAsFunctionIndex > () = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[5].location), cxt.graph->prepend(yystack_[2].value.as < BlockContentsListIndex > (), yystack_[4].value.as < DeclareVariableListIndex > ())); }
#line 2765 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 9: // file_as_func: INTERPRET variable_declarations semicolon.opt $end
#line 193 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < FileAsFunctionIndex > () = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[3].location), cxt.create(BlockContentsList{}, yystack_[1].location, yystack_[2].value.as < DeclareVariableListIndex > ())); }
#line 2771 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 10: // region.item: expr
#line 202 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < RegionItemIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 2777 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 11: // region.item: NONLOCALRETURN expr
#line 205 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < RegionItemIndex > () = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 2783 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 12: // region.item: OPENPAREN argument_declarations block.contents semicolon.opt CLOSEPAREN
#line 207 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < RegionItemIndex > () = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, yystack_[3].value.as < DeclareArgumentListIndex > (), yystack_[2].value.as < BlockContentsListIndex > ()); }
#line 2789 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 13: // region.item: OPENPAREN argument_declarations CLOSEPAREN
#line 210 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < RegionItemIndex > () = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, yystack_[1].value.as < DeclareArgumentListIndex > (), cxt.create(BlockContentsList{}, yystack_[0].location)); }
#line 2795 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 14: // region.item: OPENPAREN variable_declarations SEMICOLON block.contents semicolon.opt CLOSEPAREN
#line 213 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < RegionItemIndex > () = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[5].location), cxt.graph->prepend(yystack_[2].value.as < BlockContentsListIndex > (), yystack_[4].value.as < DeclareVariableListIndex > ())); }
#line 2801 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 15: // region: INTERPRET region.item
#line 218 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < RegionListIndex > () = cxt.create(RegionList{}, yylhs.location, yystack_[0].value.as < RegionItemIndex > ()); }
#line 2807 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 16: // region: INTERPRET error
#line 221 "langutils/sc_parser/src/sc_grammar.y"
                {
			error_recovery::expr(cxt);
			yylhs.value.as < RegionListIndex > () = cxt.create(RegionList{}, yylhs.location, cxt.create(Error{}, yylhs.location));
		}
#line 2816 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 17: // region: region SEMICOLON region.item
#line 227 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < RegionListIndex > () = cxt.graph->append(yystack_[2].value.as < RegionListIndex > (), yylhs.location, yystack_[0].value.as < RegionItemIndex > ()); }
#line 2822 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 18: // region: region REGION_SEPARATOR region.item
#line 230 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < RegionListIndex > () = cxt.graph->append(yystack_[2].value.as < RegionListIndex > (), yylhs.location, yystack_[0].value.as < RegionItemIndex > ()); }
#line 2828 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 19: // region: region error
#line 233 "langutils/sc_parser/src/sc_grammar.y"
                {
			// A missing semicolon between lines is recovered by re-emitting the token after a separator. A lexer error,
			// e.g., an unclosed comment, can't start an item, so re-emitting it would fail the same way forever.
			const bool previous_is_lexer_error = cxt.previous && sc::lex::is_error(cxt.previous->token);
			if (yystack_[1].location.end.line_number != yystack_[0].location.begin.line_number && !previous_is_lexer_error){
				auto first_child = cxt.graph->edges(*yystack_[1].value.as < RegionListIndex > ()).first_child.value();
				auto last_child = cxt.graph->last_child(*yystack_[1].value.as < RegionListIndex > ());
				auto loc = cxt.graph->location(last_child ? Index{*last_child} : Index{first_child});
				error_recovery::region_separator(cxt, loc);
				cxt.region_recovery = sc::ast::parser::ParserContext::RegionRecovery::EmitRegionSeparator;
				static_assert(std::is_same_v<decltype(yyerrstatus_), int>);
				yyerrstatus_ = 0; // this is NOT in the api, but the only way to get errors to re-emit.
				yyclearin;
				yylhs.value.as < RegionListIndex > () = yystack_[1].value.as < RegionListIndex > ();
			} else {
				error_recovery::expr(cxt);
				yylhs.value.as < RegionListIndex > () = cxt.graph->append(yystack_[1].value.as < RegionListIndex > (), cxt.create(Error{}, yystack_[0].location));
				yyclearin;
			} 
		}
#line 2853 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 20: // classOrExtList.list: classOrExtList.item
#line 264 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassOrExtensionListIndex > () = cxt.create(ClassOrExtensionList{}, yylhs.location, yystack_[0].value.as < ClassOrExtensionIndex > ()); }
#line 2859 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 21: // classOrExtList.list: classOrExtList.list classOrExtList.item
#line 266 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassOrExtensionListIndex > () = cxt.graph->append(yystack_[1].value.as < ClassOrExtensionListIndex > (), yystack_[0].value.as < ClassOrExtensionIndex > ()); }
#line 2865 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 22: // classOrExtList.item: class
#line 270 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassOrExtensionIndex > () = yystack_[0].value.as < ClassIndex > (); }
#line 2871 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 23: // classOrExtList.item: class.extension
#line 271 "langutils/sc_parser/src/sc_grammar.y"
                          { yylhs.value.as < ClassOrExtensionIndex > () = yystack_[0].value.as < ClassExtensionIndex > (); }
#line 2877 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 24: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY members.opt method.list.opt CLOSECURLY
#line 276 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassIndex > () = cxt.create(Class{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[6].location), yystack_[5].value.as < maybe<NamedIdentifierIndex> > (), yystack_[4].value.as < maybe<ClassNameIdentifierIndex> > (), yystack_[2].value.as < BigMemberListIndex > (), yystack_[1].value.as < MethodListIndex > ()); }
#line 2883 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 25: // class.super.opt: %empty
#line 280 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < maybe<ClassNameIdentifierIndex> > () = cxt.create(Missing{}, yylhs.location); }
#line 2889 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 26: // class.super.opt: COLON CLASSNAME
#line 281 "langutils/sc_parser/src/sc_grammar.y"
                          { yylhs.value.as < maybe<ClassNameIdentifierIndex> > () = cxt.create(ClassNameIdentifier{}, yystack_[0].location); }
#line 2895 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 27: // class.slot.opt: %empty
#line 285 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < maybe<NamedIdentifierIndex> > () = cxt.create(Missing{}, yylhs.location); }
#line 2901 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 28: // class.slot.opt: OPENSQUARE name CLOSESQUARE
#line 286 "langutils/sc_parser/src/sc_grammar.y"
                                      { yylhs.value.as < maybe<NamedIdentifierIndex> > () = yystack_[1].value.as < NamedIdentifierIndex > (); }
#line 2907 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 29: // class.slot.opt: OPENSQUARE CLOSESQUARE
#line 287 "langutils/sc_parser/src/sc_grammar.y"
                                 { yylhs.value.as < maybe<NamedIdentifierIndex> > () = cxt.create(Missing{}, yylhs.location); }
#line 2913 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 30: // class.extension: ADD CLASSNAME OPENCURLY method.list.opt CLOSECURLY
#line 292 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassExtensionIndex > () = cxt.create(ClassExtension{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[3].location), yystack_[1].value.as < MethodListIndex > ()); }
#line 2919 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 31: // members.entry.item: accessor variable_declarations.list.item
#line 301 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareMemberIndex > () = cxt.create(DeclareMember{}, yylhs.location, yystack_[1].value.as < RWAccessorIndex > (), yystack_[0].value.as < DeclareAnyVariableIndex > ()); }
#line 2925 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 32: // members.entry.list: members.entry.item
#line 306 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareMemberListIndex > () = cxt.create(DeclareMemberList{}, yylhs.location, yystack_[0].value.as < DeclareMemberIndex > ()); }
#line 2931 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 33: // members.entry.list: members.entry.list COMMA members.entry.item
#line 308 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareMemberListIndex > () = cxt.graph->append(yystack_[2].value.as < DeclareMemberListIndex > (), yystack_[0].value.as < DeclareMemberIndex > ()); }
#line 2937 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 34: // members.entry: CLASSVAR members.entry.list
#line 313 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyMemberListIndex > () = cxt.graph->cast<DeclareClassMemberList>(yystack_[0].value.as < DeclareMemberListIndex > ()); }
#line 2943 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 35: // members.entry: VAR members.entry.list
#line 315 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyMemberListIndex > () = cxt.graph->cast<DeclareMemberList>(yystack_[0].value.as < DeclareMemberListIndex > ()); }
#line 2949 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 36: // members.entry: CONST members.entry.list
#line 317 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyMemberListIndex > () = cxt.graph->cast<DeclareConstList>(yystack_[0].value.as < DeclareMemberListIndex > ()); }
#line 2955 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 37: // members: members.entry
#line 322 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BigMemberListIndex > () = cxt.create(BigMemberList{}, yylhs.location, yystack_[0].value.as < DeclareAnyMemberListIndex > ());}
#line 2961 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 38: // members: members SEMICOLON members.entry
#line 324 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BigMemberListIndex > () = cxt.graph->append(yystack_[2].value.as < BigMemberListIndex > (), yystack_[0].value.as < DeclareAnyMemberListIndex > ()); }
#line 2967 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 39: // members.opt: %empty
#line 328 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < BigMemberListIndex > () = cxt.create(BigMemberList{}, yylhs.location); }
#line 2973 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 40: // members.opt: members semicolon.opt
#line 329 "langutils/sc_parser/src/sc_grammar.y"
                                { yylhs.value.as < BigMemberListIndex > () = yystack_[1].value.as < BigMemberListIndex > (); }
#line 2979 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 41: // method.name: name
#line 337 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < SelectorIndex > () = yystack_[0].value.as < NamedIdentifierIndex > (); }
#line 2985 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 42: // method.name: binary_op.raw
#line 338 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < SelectorIndex > () = yystack_[0].value.as < SelectorIndex > (); }
#line 2991 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 43: // method.base: method.name OPENCURLY argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 343 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyMethodIndex > () = cxt.create(Method{}, yylhs.location, yystack_[5].value.as < SelectorIndex > (), yystack_[3].value.as < DeclareArgumentListIndex > (), cxt.create(Missing{},yystack_[4].location), yystack_[2].value.as < BlockContentsListIndex > ()); }
#line 2997 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 44: // method.base: method.name OPENCURLY argument_declarations.opt CLOSECURLY
#line 346 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyMethodIndex > () = cxt.create(Method{}, yylhs.location, yystack_[3].value.as < SelectorIndex > (), yystack_[1].value.as < DeclareArgumentListIndex > (), cxt.create(Missing{},yystack_[2].location), cxt.create(BlockContentsList{}, yystack_[0].location)); }
#line 3003 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 45: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt block.contents semicolon.opt CLOSECURLY
#line 349 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyMethodIndex > () = cxt.create(Method{}, yylhs.location, yystack_[7].value.as < SelectorIndex > (), yystack_[5].value.as < DeclareArgumentListIndex > (), cxt.create(PrimitiveIdentifier{}, yystack_[4].location), yystack_[2].value.as < BlockContentsListIndex > ()); }
#line 3009 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 46: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt CLOSECURLY
#line 352 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyMethodIndex > () = cxt.create(Method{}, yylhs.location, yystack_[5].value.as < SelectorIndex > (), yystack_[3].value.as < DeclareArgumentListIndex > (), cxt.create(PrimitiveIdentifier{}, yystack_[2].location), cxt.create(BlockContentsList{}, yystack_[1].location)); }
#line 3015 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 47: // method.base: method.name OPENCURLY error
#line 355 "langutils/sc_parser/src/sc_grammar.y"
                {
			error_recovery::method(cxt, yystack_[1].location); // advance over the closing curly of function.
			yylhs.value.as < AnyMethodIndex > () = cxt.create(Error{}, yylhs.location);
			yyclearin;
		}
#line 3025 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 48: // method: method.base
#line 363 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < AnyMethodIndex > () = yystack_[0].value.as < AnyMethodIndex > (); }
#line 3031 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 49: // method: MULTIPLY method.base
#line 365 "langutils/sc_parser/src/sc_grammar.y"
                { 
			if (auto c = cxt.graph->as_a<MethodIndex>(yystack_[0].value.as < AnyMethodIndex > ())) {
				yylhs.value.as < AnyMethodIndex > () = cxt.graph->cast<ClassMethod>(*c);
			} 
			// leaves as error
			cxt.graph->location(yystack_[0].value.as < AnyMethodIndex > ()) = yylhs.location; 
			yylhs.value.as < AnyMethodIndex > () = yystack_[0].value.as < AnyMethodIndex > (); 
		}
#line 3044 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 50: // method.list: method
#line 376 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < MethodListIndex > () = cxt.create(MethodList{}, yylhs.location, yystack_[0].value.as < AnyMethodIndex > ()); }
#line 3050 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 51: // method.list: method.list method
#line 377 "langutils/sc_parser/src/sc_grammar.y"
                             { yylhs.value.as < MethodListIndex > () = cxt.graph->append(yystack_[1].value.as < MethodListIndex > (), yylhs.location, yystack_[0].value.as < AnyMethodIndex > ()); }
#line 3056 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 52: // method.list.opt: %empty
#line 381 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < MethodListIndex > () = cxt.create(MethodList{}, yylhs.location); }
#line 3062 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 53: // method.list.opt: method.list
#line 382 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < MethodListIndex > () = yystack_[0].value.as < MethodListIndex > (); }
#line 3068 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 56: // block: block.open argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 394 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BlockIndex > () = cxt.create(BlockNode{}, yylhs.location, yystack_[3].value.as < DeclareArgumentListIndex > (), yystack_[2].value.as < BlockContentsListIndex > ()); }
#line 3074 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 57: // block: block.open argument_declarations.opt CLOSECURLY
#line 396 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BlockIndex > () = cxt.create(BlockNode{}, yylhs.location, yystack_[1].value.as < DeclareArgumentListIndex > (), cxt.create(BlockContentsList{}, yylhs.location)); }
#line 3080 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 58: // block.opt_list: %empty
#line 400 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < BlockListIndex > () = cxt.create(BlockList{}, yylhs.location); }
#line 3086 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 59: // block.opt_list: block.list
#line 401 "langutils/sc_parser/src/sc_grammar.y"
                     { yylhs.value.as < BlockListIndex > () = yystack_[0].value.as < BlockListIndex > (); }
#line 3092 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 60: // block.list: block
#line 405 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BlockListIndex > () = cxt.create(BlockList{}, yylhs.location, yystack_[0].value.as < BlockIndex > ()); }
#line 3098 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 61: // block.list: generator
#line 406 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < BlockListIndex > () = cxt.create(BlockList{}, yylhs.location, yystack_[0].value.as < GeneratorAnyIndex > ()); }
#line 3104 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 62: // block.list: block.list block
#line 407 "langutils/sc_parser/src/sc_grammar.y"
                           { yylhs.value.as < BlockListIndex > () = cxt.graph->append(yystack_[1].value.as < BlockListIndex > (), yylhs.location, yystack_[0].value.as < BlockIndex > ()); }
#line 3110 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 63: // block.list: block.list generator
#line 408 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < BlockListIndex > () = cxt.graph->append(yystack_[1].value.as < BlockListIndex > (), yylhs.location, yystack_[0].value.as < GeneratorAnyIndex > ()); }
#line 3116 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 64: // block.contents: block.contents.item
#line 412 "langutils/sc_parser/src/sc_grammar.y"
                              { yylhs.value.as < BlockContentsListIndex > () = cxt.create(BlockContentsList{}, yylhs.location, yystack_[0].value.as < BlockItemIndex > ()); }
#line 3122 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 65: // block.contents: block.contents SEMICOLON block.contents.item
#line 413 "langutils/sc_parser/src/sc_grammar.y"
                                                       { yylhs.value.as < BlockContentsListIndex > () = cxt.graph->append(yystack_[2].value.as < BlockContentsListIndex > (), yylhs.location, yystack_[0].value.as < BlockItemIndex > ()); }
#line 3128 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 66: // block.contents: block.contents NONLOCALRETURN expr
#line 415 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < BlockContentsListIndex > () = cxt.graph->append(yystack_[2].value.as < BlockContentsListIndex > (), yylhs.location, non_local); 
		}
#line 3137 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 67: // block.contents: block.contents SEMICOLON NONLOCALRETURN expr
#line 420 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < BlockContentsListIndex > () = cxt.graph->append(yystack_[3].value.as < BlockContentsListIndex > (), yylhs.location, non_local); 
		}
#line 3146 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 68: // block.contents: NONLOCALRETURN expr
#line 425 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < BlockContentsListIndex > () = cxt.create(BlockContentsList{}, yylhs.location, non_local);
		}
#line 3155 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 69: // block.contents.item: expr
#line 432 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < BlockItemIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 3161 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 70: // block.contents.item: variable_declarations
#line 433 "langutils/sc_parser/src/sc_grammar.y"
                                { yylhs.value.as < BlockItemIndex > () = yystack_[0].value.as < DeclareVariableListIndex > (); }
#line 3167 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 71: // adverb: DOT name
#line 442 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < AdverbIndex > () = yystack_[0].value.as < NamedIdentifierIndex > (); }
#line 3173 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 72: // adverb: DOT integer
#line 443 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < AdverbIndex > () = yystack_[0].value.as < IntLitIndex > (); }
#line 3179 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 73: // adverb: DOT OPENPAREN expr_or_seq CLOSEPAREN
#line 444 "langutils/sc_parser/src/sc_grammar.y"
                                               { yylhs.value.as < AdverbIndex > () = cxt.create(AdverbExprNode{}, yylhs.location, yystack_[1].value.as < AnyExprIndex > ());  }
#line 3185 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 74: // binary_op.raw: BINOP
#line 448 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3191 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 75: // binary_op.raw: READWRITEVAR
#line 449 "langutils/sc_parser/src/sc_grammar.y"
                       { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3197 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 76: // binary_op.raw: LESSTHAN
#line 450 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3203 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 77: // binary_op.raw: GREATERTHAN
#line 451 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3209 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 78: // binary_op.raw: MINUS
#line 452 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3215 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 79: // binary_op.raw: MULTIPLY
#line 453 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3221 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 80: // binary_op.raw: ADD
#line 454 "langutils/sc_parser/src/sc_grammar.y"
              { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3227 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 81: // binary_op.raw: PIPE
#line 455 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3233 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 82: // binary_op.no_adverb: binary_op.raw
#line 459 "langutils/sc_parser/src/sc_grammar.y"
                          { yylhs.value.as < SelectorIndex > () = yystack_[0].value.as < SelectorIndex > (); }
#line 3239 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 83: // binary_op.no_adverb: KEYBINOP
#line 460 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ true }, yylhs.location); }
#line 3245 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 84: // binary_op: binary_op.no_adverb adverb
#line 465 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < SelectorMaybeAdverbIndex > () = cxt.create(SelectorWAdverb{}, yylhs.location, yystack_[1].value.as < SelectorIndex > (), yystack_[0].value.as < AdverbIndex > ()); }
#line 3251 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 85: // binary_op: binary_op.no_adverb
#line 467 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < SelectorMaybeAdverbIndex > () = yystack_[0].value.as < SelectorIndex > (); }
#line 3257 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 86: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN arguments CLOSEPAREN block.opt_list
#line 472 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[5].value.as < SelectorIndex > (), yystack_[2].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ()); }
#line 3263 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 87: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN CLOSEPAREN block.list
#line 475 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[4].value.as < SelectorIndex > (), cxt.create(ArgumentList{}, yystack_[2].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3269 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 88: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN block.list
#line 478 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[2].value.as < SelectorIndex > (), cxt.create(ArgumentList{}, yystack_[0].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3275 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 89: // msgsend: name OPENPAREN arguments CLOSEPAREN block.opt_list
#line 482 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[4].value.as < NamedIdentifierIndex > (), yystack_[2].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ()); }
#line 3281 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 90: // msgsend: name OPENPAREN CLOSEPAREN block.list
#line 485 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), cxt.create(ArgumentList{}, yystack_[2].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3287 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 91: // msgsend: name block.list
#line 488 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[1].value.as < NamedIdentifierIndex > (), cxt.create(ArgumentList{}, yystack_[0].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3293 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 92: // msgsend: expr DOT name arguments.maybe_paren block.opt_list
#line 491 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PostfixMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ()); }
#line 3299 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 93: // msgsend: expr DOT arguments.paren block.opt_list
#line 494 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(ValueMessage{}, yylhs.location, yystack_[3].value.as < AnyExprIndex > (), yystack_[1].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ()); }
#line 3305 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 94: // msgsend: expr DOT OPENPAREN CLOSEPAREN block.opt_list
#line 497 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(ValueMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[2].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3311 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 95: // msgsend: CLASSNAME OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 507 "langutils/sc_parser/src/sc_grammar.y"
                {  yylhs.value.as < AnyExprIndex > () = cxt.create(CollectionNode{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[4].location), yystack_[2].value.as < ArrayIndex > ()); }
#line 3317 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 96: // msgsend: CLASSNAME block.list
#line 510 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[1].location);
			yylhs.value.as < AnyExprIndex > () = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, yystack_[1].location), yystack_[0].value.as < BlockListIndex > ());
		}
#line 3326 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 97: // msgsend: CLASSNAME arguments.paren block.opt_list
#line 515 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[2].location);
			yylhs.value.as < AnyExprIndex > () = cxt.create(NewMessage{}, yylhs.location, class_name, yystack_[1].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ());
		}
#line 3335 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 98: // msgsend: CLASSNAME OPENPAREN CLOSEPAREN block.opt_list
#line 521 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[3].location);
			yylhs.value.as < AnyExprIndex > () = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, yystack_[3].location), yystack_[0].value.as < BlockListIndex > ());
		}
#line 3344 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 99: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 536 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, yystack_[3].value.as < ArgumentListIndex > (), yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[6].value.as < AnyExprIndex > (), r);
		}
#line 3354 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 100: // expr.dotrange: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 543 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[3].location),  yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), r);
		}
#line 3364 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 101: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 550 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, yystack_[2].value.as < ArgumentListIndex > (), cxt.create(Missing{}, yystack_[1].location));
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), r);
		}
#line 3374 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 102: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 559 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, yystack_[5].value.as < ArgumentListIndex > (), yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[8].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3384 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 103: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 566 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, yystack_[4].value.as < ArgumentListIndex > (), cxt.create(Missing{}, yystack_[3].location));
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[7].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3394 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 104: // expr.dotrange.assign: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 573 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[5].location),  yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[7].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3404 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 105: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 583 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, yystack_[3].value.as < ArgumentListIndex > (), yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), r);
		}
#line 3414 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 106: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 589 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, yystack_[2].value.as < ArgumentListIndex > (), cxt.create(Missing{}, yystack_[0].location));
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), r);	
		}
#line 3424 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 107: // expr.base.range: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 595 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[3].location),  yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), r);	
		}
#line 3434 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 108: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 604 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, yystack_[5].value.as < ArgumentListIndex > (), yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[7].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3444 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 109: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 610 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, yystack_[4].value.as < ArgumentListIndex > (), cxt.create(Missing{}, yystack_[3].location));
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[6].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3454 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 110: // expr.base.range.assign: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 616 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[5].location),  yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[6].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3464 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 111: // range: OPENPAREN expr_or_seq DOTDOT CLOSEPAREN
#line 625 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, yystack_[2].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 3473 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 112: // range: OPENPAREN expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 630 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[4].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3482 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 113: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 635 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), yystack_[5].value.as < AnyExprIndex > (), yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[6].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3491 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 114: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 640 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[4].location, yystack_[2].location), yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[5].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 3500 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 115: // range: OPENPAREN DOTDOT expr_or_seq CLOSEPAREN
#line 645 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location);
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3509 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 116: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 653 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[5].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3518 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 117: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 658 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), yystack_[5].value.as < AnyExprIndex > (), yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[7].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3527 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 118: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 664 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[4].location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[6].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 3536 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 119: // range.generator: OPENPAREN COLON DOTDOT expr_or_seq CLOSEPAREN
#line 669 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location);
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3545 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 120: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT CLOSEPAREN
#line 674 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, yystack_[2].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 3554 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 121: // expr.base: literal
#line 687 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AnyPossiblyLiteralIndex > (); }
#line 3560 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 122: // expr.base: generator
#line 690 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < GeneratorAnyIndex > (); }
#line 3566 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 123: // expr.base: name
#line 693 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < NamedIdentifierIndex > (); }
#line 3572 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 124: // expr.base: CURRYARG
#line 696 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < AnyExprIndex > () = cxt.create(CurryNode{}, yystack_[0].location); }
#line 3578 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 125: // expr.base: msgsend
#line 699 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 3584 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 126: // expr.base: OPENPAREN expr_or_seq CLOSEPAREN
#line 702 "langutils/sc_parser/src/sc_grammar.y"
                { 
			// A nasty hack because of nil assignment.
			cxt.graph->register_bracketed_expr(yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AnyExprIndex > () = yystack_[1].value.as < AnyExprIndex > ();
		}
#line 3594 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 127: // expr.base: TILDE name
#line 709 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(EnvIdentifierNode{}, yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3600 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 128: // expr.base: expr.base OPENSQUARE arguments CLOSESQUARE
#line 712 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[3].value.as < AnyExprIndex > (), yystack_[1].value.as < ArgumentListIndex > () ); }
#line 3606 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 129: // expr.base: expr.base OPENSQUARE CLOSESQUARE
#line 715 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[2].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[1].location)); }
#line 3612 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 130: // expr.base: expr.base.range
#line 718 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AtMessageIndex > (); }
#line 3618 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 131: // expr.base: range
#line 721 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < RangeIndex > (); }
#line 3624 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 132: // expr.base: range.generator
#line 724 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < RangeGeneratorIndex > (); }
#line 3630 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 133: // expr: expr.base
#line 729 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 3636 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 134: // expr: expr.base.range.assign
#line 730 "langutils/sc_parser/src/sc_grammar.y"
                                 { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < RangeAssignIndex > (); }
#line 3642 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 135: // expr: expr.dotrange
#line 731 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AtMessageIndex > (); }
#line 3648 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 136: // expr: expr.dotrange.assign
#line 732 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < RangeAssignIndex > (); }
#line 3654 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 137: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE
#line 735 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[1].value.as < ArgumentListIndex > () ); }
#line 3660 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 138: // expr: expr DOT OPENSQUARE CLOSESQUARE
#line 738 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[3].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[1].location) ); }
#line 3666 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 139: // expr: expr binary_op expr
#line 741 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(BinaryMessage{}, yylhs.location, yystack_[1].value.as < SelectorMaybeAdverbIndex > (), yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3672 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 140: // expr: expr DOT name EQUALSSIGN expr
#line 744 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(SetterNode{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3678 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 141: // expr: CLASSNAME
#line 746 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < AnyExprIndex > () = cxt.create(ClassNameIdentifier{}, yylhs.location); }
#line 3684 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 142: // expr: BACKTICK expr
#line 747 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < AnyExprIndex > () = cxt.create(ReferenceNode{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 3690 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 143: // expr: name EQUALSSIGN expr
#line 749 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentNode{}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3696 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 144: // expr: TILDE name EQUALSSIGN expr
#line 752 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentNode{AssignmentNode::Target::Environment}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3702 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 145: // expr: name OPENPAREN arguments CLOSEPAREN EQUALSSIGN expr
#line 755 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(SetterNode{}, yylhs.location, yystack_[3].value.as < ArgumentListIndex > (), yystack_[5].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3708 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 146: // expr: name.pack EQUALSSIGN expr
#line 758 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(VariablePackAssign{}, yylhs.location, yystack_[2].value.as < NamePackIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3714 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 147: // expr: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 761 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(VariablePackAssign{}, yylhs.location, yystack_[3].value.as < NamePackIndex > (), yystack_[1].value.as < AnyExprIndex > ()); }
#line 3720 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 148: // expr: expr.base OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 767 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentAtNode{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), yystack_[3].value.as < ArgumentListIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3726 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 149: // expr: expr.base OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 770 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentAtNode{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[3].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3732 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 150: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 773 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentAtNode{}, yylhs.location, yystack_[6].value.as < AnyExprIndex > (), yystack_[3].value.as < ArgumentListIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3738 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 151: // expr: expr DOT OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 776 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentAtNode{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[3].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3744 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 152: // expr_or_seq.base: expr
#line 783 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 3750 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 153: // expr_or_seq.base: expr_or_seq.base SEMICOLON expr
#line 784 "langutils/sc_parser/src/sc_grammar.y"
                                          { yylhs.value.as < AnyExprIndex > () = cxt.create(ExprSeq{}, yylhs.location, yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3756 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 154: // expr_or_seq: expr_or_seq.base semicolon.opt
#line 788 "langutils/sc_parser/src/sc_grammar.y"
                                       { yylhs.value.as < AnyExprIndex > () = yystack_[1].value.as < AnyExprIndex > (); }
#line 3762 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 155: // generator: OPENCURLY COLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 797 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorAnyIndex > () = cxt.create(GeneratorNode{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < GeneratorQualListIndex > ()); }
#line 3768 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 156: // generator: OPENCURLY SEMICOLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 800 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorAnyIndex > () = cxt.create(GeneratorFakeNode{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < GeneratorQualListIndex > ()); }
#line 3774 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 157: // generator.qual.list: generator.qual
#line 805 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualListIndex > () = cxt.create(GeneratorQualList{}, yylhs.location, yystack_[0].value.as < GeneratorQualAnyIndex > ()); }
#line 3780 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 158: // generator.qual.list: generator.qual.list COMMA generator.qual
#line 807 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualListIndex > () = cxt.graph->append(yystack_[2].value.as < GeneratorQualListIndex > (), yylhs.location, yystack_[0].value.as < GeneratorQualAnyIndex > ()); }
#line 3786 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 159: // generator.qual: name LEFTARROW expr_or_seq
#line 812 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualAssign{}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), cxt.create(Missing{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3792 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 160: // generator.qual: name name LEFTARROW expr_or_seq
#line 814 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualAssign{}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3798 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 161: // generator.qual: VAR name EQUALSSIGN expr_or_seq
#line 816 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualDeclare{}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3804 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 162: // generator.qual: VAR name OPENPAREN expr_or_seq CLOSEPAREN
#line 818 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualDeclare{}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ()); }
#line 3810 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 163: // generator.qual: expr_or_seq
#line 820 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualIf{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 3816 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 164: // generator.qual: COLON COLON expr_or_seq
#line 822 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualSideEffect{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 3822 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 165: // generator.qual: COLON WHILE expr_or_seq
#line 824 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualGuard{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 3828 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 166: // argument_declarations.list: name
#line 837 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3834 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 167: // argument_declarations.list: name EQUALSSIGN expr
#line 839 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed(yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{!is_bracketed}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ())); 
		}
#line 3843 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 168: // argument_declarations.list: name OPENPAREN expr_or_seq CLOSEPAREN
#line 844 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3849 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 169: // argument_declarations.list: argument_declarations.list COMMA name
#line 847 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[2].value.as < DeclareArgumentListIndex > (), yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3855 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 170: // argument_declarations.list: argument_declarations.list COMMA name EQUALSSIGN expr
#line 850 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed(yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[4].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{!is_bracketed}, range(yystack_[2].location, yystack_[0].location), yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()));
		}
#line 3864 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 171: // argument_declarations.list: argument_declarations.list COMMA name OPENPAREN expr_or_seq CLOSEPAREN
#line 855 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(yystack_[5].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[3].location, yystack_[0].location), yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3870 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 172: // argument_declarations.pipelist: name literal
#line 861 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, yystack_[1].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyPossiblyLiteralIndex > ())); }
#line 3876 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 173: // argument_declarations.pipelist: name
#line 863 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3882 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 174: // argument_declarations.pipelist: name EQUALSSIGN literal
#line 865 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyPossiblyLiteralIndex > ())); }
#line 3888 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 175: // argument_declarations.pipelist: name OPENPAREN expr_or_seq CLOSEPAREN
#line 867 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3894 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 176: // argument_declarations.pipelist: name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 869 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, yystack_[4].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3900 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 177: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name literal
#line 871 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[3].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[1].location, yystack_[0].location), yystack_[1].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyPossiblyLiteralIndex > ())); }
#line 3906 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 178: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name
#line 873 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[2].value.as < DeclareArgumentListIndex > (), yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3912 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 179: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN literal
#line 875 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[4].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[2].location, yystack_[0].location), yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyPossiblyLiteralIndex > ())); }
#line 3918 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 180: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name OPENPAREN expr_or_seq CLOSEPAREN
#line 877 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[5].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[3].location, yystack_[0].location), yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3924 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 181: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 879 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[6].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[4].location, yystack_[0].location), yystack_[4].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3930 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 182: // argument_declarations: ARG SEMICOLON
#line 883 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 3936 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 183: // argument_declarations: ARG argument_declarations.list comma.opt SEMICOLON
#line 884 "langutils/sc_parser/src/sc_grammar.y"
                                                             { yylhs.value.as < DeclareArgumentListIndex > () = yystack_[2].value.as < DeclareArgumentListIndex > (); }
#line 3942 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 184: // argument_declarations: ARG argument_declarations.list ELLIPSIS name SEMICOLON
#line 886 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[3].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); }
#line 3948 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 185: // argument_declarations: ARG argument_declarations.list ELLIPSIS name COMMA name SEMICOLON
#line 888 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[5].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, yystack_[3].value.as < NamedIdentifierIndex > ()), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); }
#line 3954 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 186: // argument_declarations: ARG ELLIPSIS name SEMICOLON
#line 890 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(arg_list, yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); 
		}
#line 3963 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 187: // argument_declarations: ARG ELLIPSIS name COMMA name SEMICOLON
#line 895 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(
				arg_list, yylhs.location,
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, yystack_[3].value.as < NamedIdentifierIndex > ()),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())
			); 
		}
#line 3976 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 188: // argument_declarations: PIPE PIPE
#line 904 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 3982 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 189: // argument_declarations: PIPE argument_declarations.pipelist comma.opt PIPE
#line 906 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = yystack_[2].value.as < DeclareArgumentListIndex > (); }
#line 3988 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 190: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name PIPE
#line 908 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(yystack_[3].value.as < DeclareArgumentListIndex > (), yystack_[3].location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); }
#line 3994 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 191: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name COMMA name PIPE
#line 910 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(yystack_[5].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, yystack_[3].value.as < NamedIdentifierIndex > ()), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); }
#line 4000 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 192: // argument_declarations: PIPE ELLIPSIS name PIPE
#line 912 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(arg_list, yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); 
		}
#line 4009 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 193: // argument_declarations: PIPE ELLIPSIS name COMMA name PIPE
#line 917 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(
				arg_list, yylhs.location,
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, yystack_[3].value.as < NamedIdentifierIndex > ()),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())
			); 
		}
#line 4022 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 194: // argument_declarations.opt: %empty
#line 928 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 4028 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 195: // argument_declarations.opt: argument_declarations
#line 929 "langutils/sc_parser/src/sc_grammar.y"
                                { yylhs.value.as < DeclareArgumentListIndex > () = yystack_[0].value.as < DeclareArgumentListIndex > (); }
#line 4034 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 196: // arguments.entries: KEYBINOP expr_or_seq
#line 934 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < ArgumentEntryIndex > () = cxt.create(KwArgNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4040 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 197: // arguments.entries: MULTIPLY expr_or_seq
#line 935 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < ArgumentEntryIndex > () = cxt.create(VariadicArgNode{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 4046 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 198: // arguments.entries: expr_or_seq
#line 936 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < ArgumentEntryIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 4052 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 199: // arguments.no_trailing: arguments.entries
#line 940 "langutils/sc_parser/src/sc_grammar.y"
                            {  yylhs.value.as < ArgumentListIndex > () = cxt.create(ArgumentList{}, yylhs.location, yystack_[0].value.as < ArgumentEntryIndex > ()); }
#line 4058 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 200: // arguments.no_trailing: arguments.no_trailing COMMA arguments.entries
#line 941 "langutils/sc_parser/src/sc_grammar.y"
                                                        { yylhs.value.as < ArgumentListIndex > () = cxt.graph->append(yystack_[2].value.as < ArgumentListIndex > (), yylhs.location, yystack_[0].value.as < ArgumentEntryIndex > ()); }
#line 4064 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 201: // arguments: arguments.no_trailing comma.opt
#line 944 "langutils/sc_parser/src/sc_grammar.y"
                                            { yylhs.value.as < ArgumentListIndex > () = yystack_[1].value.as < ArgumentListIndex > (); }
#line 4070 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 202: // arguments.paren: OPENPAREN arguments CLOSEPAREN
#line 946 "langutils/sc_parser/src/sc_grammar.y"
                                                 { yylhs.value.as < ArgumentListIndex > () = yystack_[1].value.as < ArgumentListIndex > (); }
#line 4076 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 203: // arguments.maybe_paren: %empty
#line 949 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < ArgumentListIndex > () = cxt.create(ArgumentList{}, yylhs.location); }
#line 4082 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 204: // arguments.maybe_paren: OPENPAREN CLOSEPAREN
#line 951 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArgumentListIndex > () = cxt.create(ArgumentList{}, yylhs.location); }
#line 4088 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 205: // arguments.maybe_paren: OPENPAREN arguments CLOSEPAREN
#line 953 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArgumentListIndex > () = yystack_[1].value.as < ArgumentListIndex > (); }
#line 4094 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 206: // variable_declarations.list.item: name
#line 962 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = yystack_[0].value.as < NamedIdentifierIndex > (); }
#line 4100 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 207: // variable_declarations.list.item: name EQUALSSIGN expr
#line 964 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4106 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 208: // variable_declarations.list.item: name OPENPAREN expr_or_seq CLOSEPAREN
#line 966 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ()); }
#line 4112 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 209: // variable_declarations.list.item: name.pack EQUALSSIGN expr
#line 968 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = cxt.create(VariablePackAssign{}, yylhs.location, yystack_[2].value.as < NamePackIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4118 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 210: // variable_declarations.list.item: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 970 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = cxt.create(VariablePackAssign{}, yylhs.location, yystack_[3].value.as < NamePackIndex > (), yystack_[1].value.as < AnyExprIndex > ()); }
#line 4124 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 211: // variable_declarations.list: variable_declarations.list.item
#line 975 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareVariableListIndex > () = cxt.create(DeclareVariableList{}, yylhs.location, yystack_[0].value.as < DeclareAnyVariableIndex > ()); }
#line 4130 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 212: // variable_declarations.list: variable_declarations.list COMMA variable_declarations.list.item
#line 977 "langutils/sc_parser/src/sc_grammar.y"
                {yylhs.value.as < DeclareVariableListIndex > () = cxt.graph->append(yystack_[2].value.as < DeclareVariableListIndex > (), yystack_[0].value.as < DeclareAnyVariableIndex > ()); }
#line 4136 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 213: // variable_declarations: VAR variable_declarations.list
#line 980 "langutils/sc_parser/src/sc_grammar.y"
                                                       { yylhs.value.as < DeclareVariableListIndex > () = yystack_[0].value.as < DeclareVariableListIndex > (); }
#line 4142 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 214: // literal.terminal: symbol
#line 988 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < AnySymbolIndex > (); }
#line 4148 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 215: // literal.terminal: string
#line 989 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < StringLitIndex > (); }
#line 4154 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 216: // literal.terminal: integer
#line 990 "langutils/sc_parser/src/sc_grammar.y"
                  { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < IntLitIndex > (); }
#line 4160 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 217: // literal.terminal: float
#line 991 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < FloatProducingIndex > (); }
#line 4166 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 218: // literal.terminal: boolean
#line 992 "langutils/sc_parser/src/sc_grammar.y"
                  { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < BooleanLitIndex > (); }
#line 4172 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 219: // literal.terminal: nil
#line 993 "langutils/sc_parser/src/sc_grammar.y"
              { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < NilLitIndex > (); }
#line 4178 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 220: // literal.terminal: ascii
#line 994 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < ASCIIIndex > (); }
#line 4184 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 221: // literal.terminal: block
#line 995 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < BlockIndex > (); }
#line 4190 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 222: // literal.array.contents: %empty
#line 1000 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.create(ArrayNode{}, yylhs.location); }
#line 4196 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 223: // literal.array.contents: expr_or_seq
#line 1002 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.create(ArrayNode{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 4202 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 224: // literal.array.contents: expr_or_seq COLON expr_or_seq
#line 1004 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.create(ArrayNode{}, yylhs.location, yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4208 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 225: // literal.array.contents: KEYBINOP expr_or_seq
#line 1006 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.create(ArrayNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4214 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 226: // literal.array.contents: literal.array.contents COMMA expr_or_seq
#line 1008 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.graph->append(yystack_[2].value.as < ArrayIndex > (), yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 4220 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 227: // literal.array.contents: literal.array.contents COMMA expr_or_seq COLON expr_or_seq
#line 1010 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.graph->append(yystack_[4].value.as < ArrayIndex > (), yylhs.location, yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4226 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 228: // literal.array.contents: literal.array.contents COMMA KEYBINOP expr_or_seq
#line 1012 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.graph->append(yystack_[3].value.as < ArrayIndex > (), yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4232 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 229: // literal.dictionary.entry: expr COLON expr_or_seq
#line 1020 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryEntryIndex > () = cxt.create(DictionaryEntryNode{}, yylhs.location, yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4238 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 230: // literal.dictionary.entry: KEYBINOP expr_or_seq
#line 1022 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryEntryIndex > () = cxt.create(DictionaryEntryNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4244 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 231: // literal.dictionary.entries: %empty
#line 1027 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryIndex > () = cxt.create(DictionaryNode{}, yylhs.location); }
#line 4250 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 232: // literal.dictionary.entries: literal.dictionary.entry
#line 1029 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryIndex > () = cxt.create(DictionaryNode{}, yylhs.location, yystack_[0].value.as < DictionaryEntryIndex > ()); }
#line 4256 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 233: // literal.dictionary.entries: literal.dictionary.entries COMMA literal.dictionary.entry
#line 1031 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryIndex > () = cxt.graph->append(yystack_[2].value.as < DictionaryIndex > (), yylhs.location,  yystack_[0].value.as < DictionaryEntryIndex > ()); }
#line 4262 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 234: // literal.dictionary: OPENPAREN literal.dictionary.entries comma.opt CLOSEPAREN
#line 1036 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->location(*yystack_[2].value.as < DictionaryIndex > ()) = yylhs.location; yylhs.value.as < DictionaryIndex > () = yystack_[2].value.as < DictionaryIndex > (); }
#line 4268 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 235: // literal.array: OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1041 "langutils/sc_parser/src/sc_grammar.y"
                {  cxt.graph->location(*yystack_[2].value.as < ArrayIndex > ()) = yylhs.location; yylhs.value.as < ArrayIndex > () = yystack_[2].value.as < ArrayIndex > (); }
#line 4274 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 236: // literal.array: HASH OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1043 "langutils/sc_parser/src/sc_grammar.y"
                {  
			cxt.graph->payload(yystack_[2].value.as < ArrayIndex > ()).is_immutable = true;
			cxt.graph->location(*yystack_[2].value.as < ArrayIndex > ()) = yylhs.location;
			yylhs.value.as < ArrayIndex > () = yystack_[2].value.as < ArrayIndex > ();
		}
#line 4284 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 237: // literal: literal.terminal
#line 1051 "langutils/sc_parser/src/sc_grammar.y"
                           { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < AnyPossiblyLiteralIndex > (); }
#line 4290 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 238: // literal: literal.array
#line 1052 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < ArrayIndex > (); }
#line 4296 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 239: // literal: literal.dictionary
#line 1053 "langutils/sc_parser/src/sc_grammar.y"
                             { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < DictionaryIndex > (); }
#line 4302 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 240: // name: NAME
#line 1061 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < NamedIdentifierIndex > () = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 4308 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 241: // name: WHILE
#line 1062 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NamedIdentifierIndex > () = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 4314 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 242: // name.list: HASH name
#line 1068 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NameListIndex > () = cxt.create(NameList{}, yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 4320 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 243: // name.list: HASH TILDE name
#line 1070 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NameListIndex > () = cxt.create(NameList{}, yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), yystack_[0].value.as < NamedIdentifierIndex > ())); }
#line 4326 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 244: // name.list: name.list COMMA name
#line 1072 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NameListIndex > () = cxt.graph->append(yystack_[2].value.as < NameListIndex > (), yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 4332 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 245: // name.list: name.list COMMA TILDE name
#line 1074 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NameListIndex > () = cxt.graph->append(yystack_[3].value.as < NameListIndex > (), yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), yystack_[0].value.as < NamedIdentifierIndex > ())); }
#line 4338 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 246: // name.pack: name.list
#line 1078 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NamePackIndex > () = cxt.create(NamePack{}, yylhs.location, yystack_[0].value.as < NameListIndex > (), cxt.create(Missing{}, yystack_[0].location)); }
#line 4344 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 247: // name.pack: name.list ELLIPSIS name
#line 1080 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NamePackIndex > () = cxt.create(NamePack{}, yylhs.location, yystack_[2].value.as < NameListIndex > (), yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 4350 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 248: // name.pack: name.list ELLIPSIS TILDE name
#line 1082 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NamePackIndex > () = cxt.create(NamePack{}, yylhs.location, yystack_[3].value.as < NameListIndex > (), cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), yystack_[0].value.as < NamedIdentifierIndex > ())); }
#line 4356 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 249: // ascii: ASCII
#line 1092 "langutils/sc_parser/src/sc_grammar.y"
              { yylhs.value.as < ASCIIIndex > () = cxt.create(ASCIINode{}, yylhs.location); }
#line 4362 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 250: // nil: NIL
#line 1094 "langutils/sc_parser/src/sc_grammar.y"
          { yylhs.value.as < NilLitIndex > () = cxt.create(NilNode{}, yylhs.location); }
#line 4368 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 251: // boolean: TRUE
#line 1097 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < BooleanLitIndex > () = cxt.create(BooleanNode{true}, yylhs.location); }
#line 4374 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 252: // boolean: FALSE
#line 1098 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BooleanLitIndex > () = cxt.create(BooleanNode{false}, yylhs.location); }
#line 4380 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 253: // symbol: SYMBOL_QUOTE
#line 1102 "langutils/sc_parser/src/sc_grammar.y"
                       { yylhs.value.as < AnySymbolIndex > () = cxt.create(SymbolQuote{}, yylhs.location); }
#line 4386 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 254: // symbol: SYMBOL_SLASH
#line 1103 "langutils/sc_parser/src/sc_grammar.y"
                       { yylhs.value.as < AnySymbolIndex > () = cxt.create(SymbolSlash{}, yylhs.location); }
#line 4392 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 255: // string: STRINGLINE
#line 1107 "langutils/sc_parser/src/sc_grammar.y"
                     { yylhs.value.as < StringLitIndex > () = cxt.create(StringLineList{}, yylhs.location, cxt.create(StringLineNode{}, yylhs.location)); }
#line 4398 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 256: // string: string STRINGLINE
#line 1108 "langutils/sc_parser/src/sc_grammar.y"
                            { yylhs.value.as < StringLitIndex > () = cxt.graph->append(yystack_[1].value.as < StringLitIndex > (), cxt.create(StringLineNode{}, yystack_[0].location)); }
#line 4404 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 257: // integer: INTEGER
#line 1112 "langutils/sc_parser/src/sc_grammar.y"
                  { yylhs.value.as < IntLitIndex > () = cxt.create(IntNode{}, yylhs.location); }
#line 4410 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 258: // integer: INTEGER_RADIX
#line 1113 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < IntLitIndex > () = cxt.create(IntNode{IntNode::Kind::Radix}, yylhs.location); }
#line 4416 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 259: // integer: HEXADECIMAL
#line 1114 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < IntLitIndex > () = cxt.create(IntNode{IntNode::Kind::Hexadecimal}, yylhs.location); }
#line 4422 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 260: // integer: MINUS integer
#line 1116 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Reaches into the previous integer and changes its sign.
			cxt.graph->payload(yystack_[0].value.as < IntLitIndex > ()).sign = IntNode::Sign::Negative;
			yylhs.value.as < IntLitIndex > () = yystack_[0].value.as < IntLitIndex > ();
		}
#line 4432 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 261: // float.raw_unsigned: FLOAT
#line 1124 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < FloatLitIndex > () = cxt.create(FloatNode{}, yylhs.location); }
#line 4438 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 262: // float.raw_unsigned: FLOAT_RADIX
#line 1125 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < FloatLitIndex > () = cxt.create(FloatNode{FloatNode::Kind::Radix}, yylhs.location); }
#line 4444 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 263: // float.raw_unsigned: FLOAT_EXPONENT
#line 1126 "langutils/sc_parser/src/sc_grammar.y"
                         { yylhs.value.as < FloatLitIndex > () = cxt.create(FloatNode{FloatNode::Kind::Exponent}, yylhs.location); }
#line 4450 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 264: // float.raw_unsigned: FLOAT_INF
#line 1127 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < FloatLitIndex > () = cxt.create(FloatNode{FloatNode::Kind::Inf}, yylhs.location); }
#line 4456 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 265: // float.raw: float.raw_unsigned
#line 1132 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < FloatLitIndex > () = yystack_[0].value.as < FloatLitIndex > (); }
#line 4462 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 266: // float.raw: MINUS float.raw_unsigned
#line 1134 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload(yystack_[0].value.as < FloatLitIndex > ()).sign = FloatNode::Sign::Negative; yylhs.value.as < FloatLitIndex > () = yystack_[0].value.as < FloatLitIndex > (); }
#line 4468 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 267: // accidental.unsigned: ACCIDENTAL_STEPS
#line 1139 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AccidentalIndex > () = cxt.create(AccidentalNode{AccidentalNode::Kind::Steps}, yylhs.location); }
#line 4474 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 268: // accidental.unsigned: ACCIDENTAL_CENTS
#line 1141 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AccidentalIndex > () = cxt.create(AccidentalNode{AccidentalNode::Kind::Cents}, yylhs.location); }
#line 4480 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 269: // accidental: accidental.unsigned
#line 1146 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AccidentalIndex > () = yystack_[0].value.as < AccidentalIndex > (); }
#line 4486 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 270: // accidental: MINUS accidental.unsigned
#line 1148 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload(yystack_[0].value.as < AccidentalIndex > ()).sign = AccidentalNode::Sign::Negative; yylhs.value.as < AccidentalIndex > () = yystack_[0].value.as < AccidentalIndex > (); }
#line 4492 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 271: // float: float.raw
#line 1152 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < FloatProducingIndex > () = yystack_[0].value.as < FloatLitIndex > (); }
#line 4498 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 272: // float: accidental
#line 1153 "langutils/sc_parser/src/sc_grammar.y"
                     { yylhs.value.as < FloatProducingIndex > () = yystack_[0].value.as < AccidentalIndex > (); }
#line 4504 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 273: // float: float.raw PI
#line 1154 "langutils/sc_parser/src/sc_grammar.y"
                       { yylhs.value.as < FloatProducingIndex > () = cxt.create(PiNode{}, yylhs.location, yystack_[1].value.as < FloatLitIndex > ()); }
#line 4510 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 274: // float: integer PI
#line 1155 "langutils/sc_parser/src/sc_grammar.y"
                     { yylhs.value.as < FloatProducingIndex > () = cxt.create(PiNode{}, yylhs.location, yystack_[1].value.as < IntLitIndex > ()); }
#line 4516 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 275: // float: PI
#line 1156 "langutils/sc_parser/src/sc_grammar.y"
              { yylhs.value.as < FloatProducingIndex > () = cxt.create(PiNode{}, yylhs.location, cxt.create(Missing{}, yylhs.location)); }
#line 4522 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 276: // float: MINUS PI
#line 1157 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < FloatProducingIndex > () = cxt.create(PiNode{PiNode::Sign::Negative}, yylhs.location, cxt.create(Missing{}, yylhs.location)); }
#line 4528 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 277: // accessor: %empty
#line 1161 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < RWAccessorIndex > () = cxt.create(RWAccessor{ReadWriteAccessor::Private}, yylhs.location); }
#line 4534 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 278: // accessor: LESSTHAN
#line 1162 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < RWAccessorIndex > () = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyReadable}, yylhs.location); }
#line 4540 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 279: // accessor: GREATERTHAN
#line 1163 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < RWAccessorIndex > () = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyWriteable}, yylhs.location); }
#line 4546 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 280: // accessor: READWRITEVAR
#line 1165 "langutils/sc_parser/src/sc_grammar.y"
                { 
			yylhs.value.as < RWAccessorIndex > () = cxt.create(RWAccessor{
				combine<ReadWriteAccessor::ExternallyReadable, ReadWriteAccessor::ExternallyWriteable>()
			}, yylhs.location); 
		}
#line 4556 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;


#line 4560 "langutils/sc_parser/src/sc_grammar_parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        report_syntax_error (yyctx);
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  const char *
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    static const char *const yy_sname[] =
    {
    "end of file", "error", "invalid token", "REGION_SEPARATOR",
  "OPENCURLY", "CLOSECURLY", "OPENSQUARE", "CLOSESQUARE", "OPENPAREN",
  "CLOSEPAREN", "SEMICOLON", "NONLOCALRETURN", "COMMA", "HASH", "TILDE",
  "NAME", "INTEGER", "INTEGER_RADIX", "HEXADECIMAL", "FLOAT",
  "FLOAT_RADIX", "FLOAT_EXPONENT", "FLOAT_INF", "ACCIDENTAL_STEPS",
  "ACCIDENTAL_CENTS", "SYMBOL_QUOTE", "SYMBOL_SLASH", "STRINGLINE",
  "ASCII", "PRIMITIVENAME", "CLASSNAME", "CURRYARG", "VAR", "ARG",
  "CLASSVAR", "CONST", "NIL", "TRUE", "FALSE", "PI", "ELLIPSIS", "DOTDOT",
  "BEGINCLOSEDFUNC", "BADTOKEN", "INTERPRET", "LEFTARROW", "WHILE",
  "LEXER_ERROR", "COLON", "EQUALSSIGN", "BINOP", "KEYBINOP", "MINUS",
  "LESSTHAN", "GREATERTHAN", "MULTIPLY", "ADD", "PIPE", "READWRITEVAR",
  "DOT", "BACKTICK", "UMINUS", "$accept", "go", "file_as_func",
  "region.item", "region", "classOrExtList.list", "classOrExtList.item",
  "class", "class.super.opt", "class.slot.opt", "class.extension",
  "members.entry.item", "members.entry.list", "members.entry", "members",
  "members.opt", "method.name", "method.base", "method", "method.list",
  "method.list.opt", "block.open", "block", "block.opt_list", "block.list",
  "block.contents", "block.contents.item", "adverb", "binary_op.raw",
  "binary_op.no_adverb", "binary_op", "msgsend", "expr.dotrange",
  "expr.dotrange.assign", "expr.base.range", "expr.base.range.assign",
  "range", "range.generator", "expr.base", "expr", "expr_or_seq.base",
  "expr_or_seq", "generator", "generator.qual.list", "generator.qual",
  "argument_declarations.list", "argument_declarations.pipelist",
  "argument_declarations", "argument_declarations.opt",
  "arguments.entries", "arguments.no_trailing", "arguments",
  "arguments.paren", "arguments.maybe_paren",
  "variable_declarations.list.item", "variable_declarations.list",
  "variable_declarations", "literal.terminal", "literal.array.contents",
  "literal.dictionary.entry", "literal.dictionary.entries",
  "literal.dictionary", "literal.array", "literal", "name", "name.list",
  "name.pack", "ascii", "nil", "boolean", "symbol", "string", "integer",
  "float.raw_unsigned", "float.raw", "accidental.unsigned", "accidental",
  "float", "accessor", "semicolon.opt", "comma.opt", YY_NULLPTR
    };
    return yy_sname[yysymbol];
  }



  // parser::context.
  parser::context::context (const parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }








  const short parser::yypact_ninf_ = -210;

  const short parser::yytable_ninf_ = -282;

  const short
  parser::yypact_[] =
  {
      -3,    11,   571,    -1,    54,  -210,   259,    14,  -210,  -210,
    -210,   104,   -16,  -210,  -210,    38,  1616,   734,  2156,   101,
     146,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,
    -210,  -210,  -210,  -210,  -210,   130,  -210,   140,   133,  -210,
    -210,  -210,  -210,  -210,  -210,   620,    10,  2156,  -210,    94,
    -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,    74,  2315,
    -210,  1418,    28,  -210,  -210,  -210,  -210,    29,    88,    12,
    -210,  -210,  -210,  -210,    67,   103,  -210,   106,  -210,  -210,
    -210,   167,  -210,  -210,  1666,  1666,   176,  -210,  -210,  -210,
     199,   181,   253,  2156,  2156,   806,  2156,  2315,   211,   179,
     258,  2156,  1715,  -210,  2156,   620,  -210,  -210,  -210,  -210,
      10,  -210,  -210,   262,   507,   198,  1014,   263,  -210,   265,
    2315,  1616,   146,  -210,   225,  1616,  1063,  -210,   135,  -210,
     135,   204,  -210,   266,    15,    26,  -210,   146,   129,    27,
    -210,   177,  -210,  -210,  -210,   146,  -210,   156,  2201,  -210,
    -210,  1116,   861,  -210,  -210,  -210,   118,   221,  2156,  2156,
     110,  -210,  2315,  -210,  1418,   281,  1165,  2156,   135,   209,
     217,  2156,  2156,  -210,  -210,  -210,   397,  -210,  -210,  -210,
    -210,  -210,   203,   270,   272,  -210,  2156,  -210,  2156,  1764,
     280,   282,  2156,    16,  -210,    43,  2156,  -210,  2156,  1813,
    -210,   110,  1418,  1862,   283,   258,  -210,  2156,   258,   135,
    2156,  2156,  -210,  -210,   277,   284,  -210,  -210,  -210,   135,
     140,  2156,  2156,  2156,  2156,   187,   146,   146,   285,  2156,
    2156,     6,  -210,   146,    45,  -210,  1862,   288,  2275,  -210,
    -210,   110,   241,  2156,    17,   911,  1215,   135,    32,   114,
    -210,   237,  2315,  1468,  2156,   299,   110,  -210,   135,   292,
    2315,   146,  -210,   146,  -210,   293,  2315,   427,   300,  -210,
    -210,   397,   298,  -210,  -210,   214,   214,   214,  -210,   295,
     397,  1517,  1517,  2315,  -210,  2156,   261,  -210,  -210,   297,
    2156,  1911,  1265,   135,  -210,   271,  -210,   302,   304,   110,
    2156,   507,  -210,  -210,   307,  2315,   308,  -210,  -210,  -210,
    1566,  -210,  -210,  -210,   310,  2315,   311,  2315,  -210,   146,
      35,   205,  -210,   312,  2315,   146,  -210,     9,  -210,  2238,
     313,  1862,  -210,   319,  2156,   318,   268,  1960,   278,  2156,
     105,   135,  -210,  1315,  2156,   135,  2156,  -210,  -210,  2156,
    -210,  2315,  -210,   326,   135,    48,  -210,  -210,  -210,  -210,
     684,  -210,  -210,  -210,  -210,  -210,  -210,   316,   140,   316,
     316,   203,  -210,   329,   146,   195,  -210,   323,  -210,    64,
     323,  -210,  2156,  -210,   290,  -210,   328,   135,   330,  2009,
    -210,  -210,   332,  -210,  -210,  -210,  -210,  -210,   333,  2156,
    2156,  -210,   146,  -210,   250,   146,  -210,  1862,  2312,  -210,
    -210,   335,  -210,  2315,   289,  2156,   296,   339,  2156,   346,
     305,  2058,  -210,  -210,   347,  2315,  -210,   348,  2315,  -210,
    2156,  -210,  -210,   964,   214,  -210,  -210,  -210,    69,  2156,
    2156,  1517,   350,  2156,   315,   353,  -210,  2107,  -210,   135,
     135,  -210,   354,  -210,  -210,   355,  2315,   356,  -210,   314,
     358,  1862,  -210,  -210,  2156,  2315,  2156,   321,  2315,   324,
    2156,   327,   368,  -210,  -210,  2315,  -210,   369,   110,  -210,
    2156,  2156,  -210,  -210,  -210,  -210,  -210,  2156,  -210,  -210,
     371,  -210,  -210,  -210,  -210,  -210,  -210,   372,  2315,  2315,
    2156,  2156,  2315,  2156,   334,  -210,  1368,   360,   375,  -210,
    -210,  -210,  -210,  2315,  2315,  2315,  2156,  -210,   110,  -210,
    -210,  2315,   380,  -210
  };

  const short
  parser::yydefact_[] =
  {
       6,    27,     0,     0,     0,     4,     0,     0,    20,    22,
      23,     0,    25,     3,    16,    54,   222,   231,     0,     0,
       0,   240,   257,   258,   259,   261,   262,   263,   264,   267,
     268,   253,   254,   255,   249,   141,   124,     0,     0,   250,
     251,   252,   275,    55,   241,     0,     0,     0,    15,   194,
     221,   125,   135,   136,   130,   134,   131,   132,   133,    10,
     122,     0,   281,   237,   239,   238,   121,   123,   246,     0,
     220,   219,   218,   214,   215,   216,   265,   271,   269,   272,
     217,     0,     1,    19,     0,   282,     0,     5,    21,    29,
       0,     0,     0,     0,     0,   231,     0,   152,   281,   223,
     283,     0,     0,    74,    83,    78,    76,    77,    79,    80,
      81,    75,    82,     0,   152,     0,     0,     0,   232,   283,
      11,   222,     0,   242,   127,   222,     0,    60,    96,    61,
      58,     0,   211,   213,   206,     0,   182,     0,   283,   166,
     276,     0,   260,   266,   270,     0,   188,   283,   173,   142,
     195,     0,     0,    83,    78,    81,     0,    85,     0,     0,
     281,    64,    69,    70,   282,     0,     0,     0,    91,     0,
       0,     0,     0,   256,   274,   273,    52,    18,    17,     2,
      28,    26,    39,     0,     0,   225,   282,   154,     0,   284,
       0,     0,     0,     0,   230,     0,     0,   126,     0,     0,
      13,   281,     0,   284,     0,   283,   243,     0,   283,    58,
       0,     0,   198,   199,   283,     0,    62,    63,    97,    59,
       0,     0,     0,     0,     0,     0,   284,     0,     0,     0,
       0,     0,   284,     0,     0,    54,   231,     0,     0,   172,
      57,   281,   129,     0,     0,     0,     0,    58,   203,     0,
      84,   139,    68,   282,     0,     0,   281,     9,     0,     0,
     143,     0,   244,     0,   247,     0,   146,    79,     0,    48,
      50,    53,     0,    42,    41,   277,   277,   277,    37,   281,
      52,     0,     0,   153,   224,     0,   226,   235,   115,     0,
       0,     0,     0,    88,   229,     0,   111,     0,     0,   281,
       0,     0,   233,   234,     0,   144,     0,    98,   196,   197,
     284,   201,   202,   212,     0,   207,     0,   209,   186,     0,
     169,     0,   183,     0,   167,     0,   192,     0,   189,   178,
       0,   231,   174,     0,     0,     0,   128,     0,   138,     0,
       0,    58,    93,     0,     0,    58,     0,    71,    72,     0,
      65,    66,     7,     0,    90,    58,   245,   248,   147,    49,
       0,    51,    30,   278,   279,   280,    32,    35,     0,    34,
      36,   282,    40,     0,     0,     0,   163,   283,   157,   123,
     283,   228,     0,   119,     0,   120,     0,     0,     0,     0,
     112,    12,     0,   236,    95,   200,   208,   210,     0,     0,
       0,   184,     0,   168,     0,     0,   190,   231,     0,   177,
     175,     0,    56,   149,   107,     0,   106,     0,     0,     0,
     137,     0,    94,   204,     0,   140,    92,     0,    67,     8,
       0,    89,    47,     0,   277,    31,    38,    24,     0,     0,
       0,   284,     0,     0,     0,     0,   227,     0,   116,    87,
      58,   114,     0,    14,   187,     0,   170,     0,   193,     0,
       0,   231,   179,   176,     0,   148,     0,   105,   151,   100,
       0,   101,     0,   205,    73,   145,    44,   281,   281,    33,
       0,     0,   165,   164,   158,   156,   159,     0,   155,   118,
       0,    86,   113,   171,   185,   191,   180,     0,   110,   109,
       0,     0,   150,     0,    99,   282,     0,     0,     0,   161,
     160,   117,   181,   108,   104,   103,     0,    46,   281,    43,
     162,   102,     0,    45
  };

  const short
  parser::yypgoto_[] =
  {
    -210,  -210,  -210,   160,  -210,  -210,   381,  -210,  -210,  -210,
    -210,   -45,   -30,    19,  -210,  -210,  -210,   125,   122,  -210,
     115,  -210,    -5,  -201,   -28,  -115,   143,  -210,  -172,    -7,
    -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,    -2,
    -210,    87,     7,   117,   -44,  -210,  -210,   223,    40,    91,
    -210,  -140,   246,  -210,  -209,  -210,   231,  -210,    53,   200,
    -210,  -210,  -210,  -142,   400,  -210,   -35,  -210,  -210,  -210,
    -210,  -210,   -40,   -36,  -210,   -32,  -210,  -210,  -210,   -43,
    -116
  };

  const short
  parser::yydefgoto_[] =
  {
       0,     4,     5,    48,     6,     7,     8,     9,    92,    12,
      10,   366,   367,   278,   279,   280,   268,   269,   270,   271,
     272,    49,    50,   218,   219,   160,   161,   250,   112,   157,
     158,    51,    52,    53,    54,    55,    56,    57,    58,    97,
      98,   212,    60,   377,   378,   138,   147,   150,   151,   213,
     214,   215,   130,   345,   132,   133,   163,    63,   100,   118,
     119,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,   368,    86,
     190
  };

  const short
  parser::yytable_[] =
  {
      59,   201,   135,   204,   273,   142,   239,   128,   307,   143,
     113,   313,   244,   144,    87,   114,   120,    11,   325,   165,
     171,   405,   228,   221,   336,    21,   259,     1,   290,    81,
     127,   234,    91,    15,   223,   229,   241,   166,   164,   168,
     343,     2,   129,   399,     1,   149,   342,    15,    93,   256,
     145,   292,    15,     3,    82,   187,    44,   291,   337,   162,
      21,   172,   127,   326,   222,   142,   406,   146,    15,   143,
       3,    43,   166,   144,   129,   224,   230,   480,   167,    21,
     152,   344,    59,    59,   400,    43,    94,   299,   113,   304,
      43,    44,   306,   114,   173,   273,   332,   430,   311,   273,
     169,   142,   328,    99,   115,   340,    43,   121,   273,   443,
      44,    89,   420,   167,   162,   122,    21,   255,   481,    21,
     253,   254,   346,   216,   245,   127,   246,    38,   170,    21,
      22,    23,    24,    21,    15,   217,   125,   129,   126,    15,
     422,   226,   174,   136,   426,   175,   421,    44,    21,   162,
      44,    46,   388,   131,   431,    21,   251,   252,   298,   435,
      44,    21,   162,   216,    44,   260,   141,   293,   232,   227,
     266,   176,    43,   137,   205,   217,   179,    43,   208,    44,
     183,   184,   115,   185,   283,   135,    44,   409,   191,   193,
     127,   194,    44,    22,    23,    24,   233,   318,   333,   319,
     162,   301,   129,   424,   127,   305,   180,   197,    99,   348,
     198,   181,    99,   353,   216,   401,   129,   402,   122,    21,
     315,   186,   317,   261,    21,    61,   217,   188,   324,   141,
     354,   263,    21,    62,   114,   275,   372,   276,   277,   199,
     116,   439,   127,   440,   177,   178,   369,   370,   117,   491,
      44,   162,   351,   127,   129,    44,   392,   182,   265,  -281,
      83,   442,    84,    44,   445,   129,   462,   363,   364,    85,
     189,   195,   365,   202,   207,   284,   286,   203,   220,   289,
     249,   257,   281,   294,   282,   295,   297,   287,   216,   310,
     334,   288,   303,   312,   121,   322,   156,   308,   309,   352,
     217,   355,   358,   362,   360,   371,   383,   458,   314,   382,
     316,   390,   389,   391,   393,   394,   323,   415,   478,   396,
     397,   403,   410,   330,   412,   414,   429,   418,   434,   114,
     335,   447,   413,   135,   437,   441,   127,   448,   464,   450,
     127,   453,   425,   454,   463,   466,   467,   428,   129,   216,
     127,   168,   129,   469,   470,   485,   473,   474,   488,   449,
     487,   217,   129,   492,   493,   519,   494,   496,   376,   376,
     500,   495,   381,   501,   127,   504,   503,   384,   386,   505,
     511,   512,   127,   516,   520,   523,   129,   194,    88,   479,
     436,   518,   359,   361,   129,   373,   350,   484,   456,   380,
     433,   395,   247,   302,     0,   114,     0,     0,     0,     0,
       0,    90,    21,   465,     0,     0,   468,     0,   411,   123,
     124,     0,     0,     0,   417,     0,   419,     0,   475,     0,
       0,   162,     0,   427,   506,   507,     0,   134,   139,     0,
       0,     0,    21,    44,   216,   127,   148,   103,     0,   154,
     106,   107,   267,   109,   155,   111,   217,   129,     0,   114,
       0,     0,   498,     0,   499,     0,     0,     0,   502,   446,
       0,     0,     0,    44,     0,   522,   452,   103,     0,   154,
     106,   107,   108,   109,   155,   111,   455,     0,     0,     0,
       0,     0,     0,     0,   460,     0,     0,     0,   513,   514,
       0,   515,     0,     0,   162,     0,     0,     0,   472,     0,
     148,     0,     0,     0,   521,     0,     0,     0,     0,     0,
       0,     0,   206,     0,     0,     0,   482,   483,   376,     0,
     486,   123,     0,     0,   490,     0,     0,   225,     0,     0,
       0,     0,     0,     0,     0,   231,     0,     0,   497,     0,
       0,     0,     0,     0,     0,   196,   248,   103,   153,   154,
     106,   107,   108,   109,   155,   111,   156,   508,   509,   262,
     264,    13,    14,     0,   510,    15,   274,    16,     0,    17,
       0,     0,    18,     0,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
       0,    35,    36,    37,    38,     0,     0,    39,    40,    41,
      42,     0,     0,    43,     0,     0,     0,    44,     0,     0,
     134,     0,     0,    45,     0,     0,   320,   321,    46,     0,
       0,    47,     0,   327,   329,     0,    22,    23,    24,    25,
      26,    27,    28,    29,    30,     0,     0,     0,     0,   347,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   140,
       0,   356,     0,   357,     0,     0,     0,   274,     0,     0,
       0,   274,   141,     0,     0,     0,     0,     0,     0,     0,
     274,   379,   379,     0,     0,   432,     0,     0,  -194,  -194,
    -194,     0,  -194,     0,     0,  -194,     0,  -194,  -194,  -194,
    -194,  -194,  -194,  -194,  -194,  -194,  -194,  -194,  -194,  -194,
    -194,  -194,  -194,  -194,  -194,  -194,  -194,    38,     0,   398,
    -194,  -194,  -194,  -194,     0,   404,  -194,     0,     0,     0,
    -194,     0,     0,     0,     0,     0,  -194,     0,    15,     0,
      16,    46,    95,     0,  -194,     0,     0,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,     0,    35,    36,    37,    38,   134,     0,
      39,    40,    41,    42,   438,   101,    43,     0,     0,   444,
      44,     0,   102,     0,   103,   104,   105,   106,   107,   108,
     109,   110,   111,     0,    47,     0,     0,     0,     0,     0,
       0,     0,   457,     0,     0,   459,     0,     0,     0,     0,
      15,     0,    16,     0,    95,     0,     0,     0,     0,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,     0,    35,    36,     0,     0,
       0,   379,    39,    40,    41,    42,     0,   101,    43,     0,
       0,     0,    44,     0,   102,     0,   103,   104,   105,   106,
     107,   108,   109,   155,   111,    15,    47,    16,   242,    95,
       0,     0,     0,     0,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
       0,    35,    36,     0,     0,     0,     0,    39,    40,    41,
      42,     0,   243,    43,     0,     0,     0,    44,     0,     0,
       0,     0,   210,    45,     0,    15,   211,    16,   338,    95,
       0,    47,     0,     0,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
       0,    35,    36,     0,     0,     0,     0,    39,    40,    41,
      42,     0,   339,    43,     0,     0,     0,    44,     0,     0,
       0,     0,   210,    45,     0,     0,   211,     0,    15,   476,
      16,    47,    95,     0,     0,   159,     0,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,   477,    35,    36,    37,     0,     0,     0,
      39,    40,    41,    42,     0,     0,    43,     0,     0,     0,
      44,     0,     0,     0,     0,     0,    45,     0,    15,     0,
      16,     0,    95,   200,    47,   159,     0,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,     0,    35,    36,    37,     0,     0,     0,
      39,    40,    41,    42,     0,     0,    43,     0,     0,     0,
      44,     0,     0,     0,     0,     0,    45,    15,     0,    16,
       0,    95,   209,     0,    47,     0,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,     0,    35,    36,     0,     0,     0,     0,    39,
      40,    41,    42,     0,     0,    43,     0,     0,     0,    44,
       0,     0,     0,     0,   210,    45,     0,     0,   211,     0,
      15,   240,    16,    47,    95,     0,     0,   159,     0,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,     0,    35,    36,    37,     0,
       0,     0,    39,    40,    41,    42,     0,     0,    43,     0,
       0,     0,    44,     0,     0,     0,     0,     0,    45,    15,
       0,    16,     0,    95,   258,     0,    47,     0,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,    35,    36,     0,     0,     0,
       0,    39,    40,    41,    42,     0,     0,    43,     0,     0,
       0,    44,     0,     0,     0,     0,   210,    45,     0,    15,
     211,    16,     0,    95,   341,    47,     0,     0,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,    35,    36,     0,     0,     0,
       0,    39,    40,    41,    42,     0,     0,    43,     0,     0,
       0,    44,     0,     0,     0,     0,   210,    45,     0,    15,
     211,    16,     0,    95,   387,    47,     0,     0,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,    35,    36,     0,     0,     0,
       0,    39,    40,    41,    42,     0,     0,    43,     0,     0,
       0,    44,     0,     0,     0,     0,   210,    45,     0,    15,
     211,    16,     0,    95,   423,    47,     0,     0,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,    35,    36,     0,     0,     0,
       0,    39,    40,    41,    42,     0,     0,    43,     0,     0,
       0,    44,     0,     0,     0,     0,   210,    45,     0,     0,
     211,     0,    15,   517,    16,    47,    95,     0,     0,   159,
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,     0,    35,    36,
      37,     0,     0,     0,    39,    40,    41,    42,     0,     0,
      43,     0,     0,     0,    44,     0,     0,     0,     0,     0,
      45,     0,    15,     0,    16,     0,    95,     0,    47,   159,
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,     0,    35,    36,
      37,     0,     0,     0,    39,    40,    41,    42,     0,     0,
      43,     0,     0,     0,    44,     0,     0,     0,     0,     0,
      45,     0,    15,     0,    16,     0,    95,     0,    47,   349,
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,     0,    35,    36,
      37,     0,     0,     0,    39,    40,    41,    42,     0,     0,
      43,     0,     0,     0,    44,     0,     0,     0,     0,     0,
      45,    15,     0,    16,     0,    95,     0,     0,    47,     0,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,     0,    35,    36,   374,
       0,     0,     0,    39,    40,    41,    42,     0,     0,    43,
       0,     0,     0,    44,     0,   375,     0,     0,     0,    45,
      15,     0,    16,     0,    95,     0,     0,    47,     0,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,     0,    35,    36,     0,     0,
       0,     0,    39,    40,    41,    42,     0,     0,    43,     0,
       0,     0,    44,     0,     0,     0,     0,   210,    45,     0,
      15,   211,    16,     0,    95,     0,    47,     0,     0,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,     0,    35,    36,     0,     0,
       0,     0,    39,    40,    41,    42,     0,     0,    43,     0,
       0,     0,    44,     0,     0,     0,     0,    96,    45,     0,
      15,     0,    16,     0,    17,     0,    47,    18,     0,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,     0,    35,    36,     0,     0,
       0,     0,    39,    40,    41,    42,     0,     0,    43,     0,
       0,     0,    44,     0,     0,     0,     0,     0,    45,    15,
       0,    16,     0,    95,     0,     0,    47,     0,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,    35,    36,     0,     0,     0,
       0,    39,    40,    41,    42,     0,   192,    43,     0,     0,
       0,    44,     0,     0,     0,     0,     0,    45,    15,     0,
      16,     0,    95,     0,     0,    47,     0,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,     0,    35,    36,     0,     0,     0,     0,
      39,    40,    41,    42,     0,     0,    43,     0,     0,     0,
      44,     0,     0,     0,     0,   285,    45,    15,     0,    16,
       0,    95,   296,     0,    47,     0,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,     0,    35,    36,     0,     0,     0,     0,    39,
      40,    41,    42,     0,     0,    43,     0,     0,     0,    44,
       0,     0,     0,     0,     0,    45,    15,     0,    16,     0,
      95,     0,     0,    47,     0,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,     0,    35,    36,     0,     0,     0,     0,    39,    40,
      41,    42,     0,     0,    43,     0,     0,     0,    44,     0,
       0,     0,     0,   300,    45,    15,     0,    16,     0,    95,
     385,     0,    47,     0,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
       0,    35,    36,     0,     0,     0,     0,    39,    40,    41,
      42,     0,     0,    43,     0,     0,     0,    44,     0,     0,
       0,     0,     0,    45,    15,     0,    16,   416,    95,     0,
       0,    47,     0,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,     0,
      35,    36,     0,     0,     0,     0,    39,    40,    41,    42,
       0,     0,    43,     0,     0,     0,    44,     0,     0,     0,
       0,     0,    45,    15,     0,    16,     0,    95,   451,     0,
      47,     0,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,     0,    35,
      36,     0,     0,     0,     0,    39,    40,    41,    42,     0,
       0,    43,     0,     0,     0,    44,     0,     0,     0,     0,
       0,    45,    15,     0,    16,   471,    95,     0,     0,    47,
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,     0,    35,    36,
       0,     0,     0,     0,    39,    40,    41,    42,     0,     0,
      43,     0,     0,     0,    44,     0,     0,     0,     0,     0,
      45,    15,     0,    16,     0,    95,   489,     0,    47,     0,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,     0,    35,    36,     0,
       0,     0,     0,    39,    40,    41,    42,     0,     0,    43,
       0,     0,     0,    44,     0,     0,     0,     0,     0,    45,
      15,     0,    16,     0,    95,     0,     0,    47,     0,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,     0,    35,    36,     0,     0,
       0,     0,    39,    40,    41,    42,     0,     0,    43,     0,
       0,     0,    44,     0,     0,   235,     0,    16,    45,   236,
       0,     0,     0,     0,   237,     0,    47,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
       0,     0,     0,     0,     0,     0,     0,    39,    40,    41,
      42,     0,   235,    43,    16,     0,   407,     0,     0,     0,
     238,   237,     0,    45,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,     0,     0,     0,
       0,     0,     0,     0,    39,    40,    41,    42,     0,   235,
      43,    16,     0,   331,     0,     0,     0,   408,   237,     0,
      45,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,     0,     0,     0,     0,     0,
       0,    39,    40,    41,    42,     0,   235,    43,    16,     0,
     461,     0,     0,     0,     0,   237,     0,    45,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,     0,     0,     0,     0,     0,     0,     0,    39,    40,
      41,    42,     0,     0,    43,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    45,   103,   153,   154,   106,   107,
     108,   109,   155,   111,   156
  };

  const short
  parser::yycheck_[] =
  {
       2,   116,    37,   119,   176,    45,   148,    35,   209,    45,
      17,   220,   152,    45,     0,    17,    18,     6,    12,    62,
       8,    12,   138,     8,     7,    15,   166,    30,    12,    30,
      35,   147,    48,     4,     8,     8,   151,     8,    10,    67,
       8,    44,    35,     8,    30,    47,   247,     4,    10,   164,
      40,     8,     4,    56,     0,    98,    46,    41,    41,    61,
      15,    49,    67,    57,    49,   105,    57,    57,     4,   105,
      56,    42,     8,   105,    67,    49,    49,     8,    49,    15,
       6,    49,    84,    85,    49,    42,    48,   202,    95,   205,
      42,    46,   208,    95,    27,   267,   238,    49,   214,   271,
      12,   141,    57,    16,    17,   245,    42,     6,   280,    45,
      46,     7,     7,    49,   116,    14,    15,   160,    49,    15,
      10,    11,     8,   128,     6,   130,     8,    33,    40,    15,
      16,    17,    18,    15,     4,   128,     6,   130,     8,     4,
     341,    12,    39,    10,   345,    39,    41,    46,    15,   151,
      46,    57,   292,    13,   355,    15,   158,   159,   201,   368,
      46,    15,   164,   168,    46,   167,    52,   195,    12,    40,
     172,     4,    42,    40,   121,   168,     0,    42,   125,    46,
      93,    94,    95,    96,   186,   220,    46,   329,   101,   102,
     195,   104,    46,    16,    17,    18,    40,    10,   241,    12,
     202,   203,   195,   343,   209,   207,     7,     9,   121,   249,
      12,    30,   125,   256,   219,    10,   209,    12,    14,    15,
     222,    10,   224,    14,    15,     2,   219,    48,   230,    52,
     258,    14,    15,     2,   236,    32,   279,    34,    35,    41,
      17,    46,   247,    48,    84,    85,   276,   277,    17,   450,
      46,   253,   254,   258,   247,    46,   299,     4,   171,     0,
       1,   377,     3,    46,   380,   258,   408,    53,    54,    10,
      12,     9,    58,    10,    49,   188,   189,    12,    12,   192,
      59,     0,    12,   196,    12,   198,   199,     7,   293,    12,
      49,     9,     9,     9,     6,    10,    59,   210,   211,     0,
     293,     9,     9,     5,     4,    10,     9,    57,   221,    48,
     223,     9,    41,     9,     7,     7,   229,    49,   433,     9,
       9,     9,     9,   236,     5,     7,     0,    49,    12,   331,
     243,    41,   334,   368,     5,    12,   341,     9,    49,     9,
     345,     9,   344,    10,     9,    49,     7,   349,   341,   354,
     355,   379,   345,     7,    49,     5,     9,     9,     5,   387,
      45,   354,   355,     9,     9,     5,    10,     9,   281,   282,
      49,    57,   285,    49,   379,     7,    49,   290,   291,    10,
       9,     9,   387,    49,     9,     5,   379,   300,     7,   434,
     371,   506,   267,   271,   387,   280,   253,   441,   400,   282,
     360,   310,   156,   203,    -1,   407,    -1,    -1,    -1,    -1,
      -1,    11,    15,   415,    -1,    -1,   418,    -1,   331,    19,
      20,    -1,    -1,    -1,   337,    -1,   339,    -1,   430,    -1,
      -1,   433,    -1,   346,   477,   478,    -1,    37,    38,    -1,
      -1,    -1,    15,    46,   449,   450,    46,    50,    -1,    52,
      53,    54,    55,    56,    57,    58,   449,   450,    -1,   461,
      -1,    -1,   464,    -1,   466,    -1,    -1,    -1,   470,   382,
      -1,    -1,    -1,    46,    -1,   518,   389,    50,    -1,    52,
      53,    54,    55,    56,    57,    58,   399,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   407,    -1,    -1,    -1,   500,   501,
      -1,   503,    -1,    -1,   506,    -1,    -1,    -1,   421,    -1,
     110,    -1,    -1,    -1,   516,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   122,    -1,    -1,    -1,   439,   440,   441,    -1,
     443,   131,    -1,    -1,   447,    -1,    -1,   137,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   145,    -1,    -1,   461,    -1,
      -1,    -1,    -1,    -1,    -1,    48,   156,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,   480,   481,   169,
     170,     0,     1,    -1,   487,     4,   176,     6,    -1,     8,
      -1,    -1,    11,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    32,    33,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,    -1,
     220,    -1,    -1,    52,    -1,    -1,   226,   227,    57,    -1,
      -1,    60,    -1,   233,   234,    -1,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,   249,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    39,
      -1,   261,    -1,   263,    -1,    -1,    -1,   267,    -1,    -1,
      -1,   271,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     280,   281,   282,    -1,    -1,     1,    -1,    -1,     4,     5,
       6,    -1,     8,    -1,    -1,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    -1,   319,
      36,    37,    38,    39,    -1,   325,    42,    -1,    -1,    -1,
      46,    -1,    -1,    -1,    -1,    -1,    52,    -1,     4,    -1,
       6,    57,     8,    -1,    60,    -1,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    32,    33,   368,    -1,
      36,    37,    38,    39,   374,    41,    42,    -1,    -1,   379,
      46,    -1,    48,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   402,    -1,    -1,   405,    -1,    -1,    -1,    -1,
       4,    -1,     6,    -1,     8,    -1,    -1,    -1,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,   441,    36,    37,    38,    39,    -1,    41,    42,    -1,
      -1,    -1,    46,    -1,    48,    -1,    50,    51,    52,    53,
      54,    55,    56,    57,    58,     4,    60,     6,     7,     8,
      -1,    -1,    -1,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    41,    42,    -1,    -1,    -1,    46,    -1,    -1,
      -1,    -1,    51,    52,    -1,     4,    55,     6,     7,     8,
      -1,    60,    -1,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    41,    42,    -1,    -1,    -1,    46,    -1,    -1,
      -1,    -1,    51,    52,    -1,    -1,    55,    -1,     4,     5,
       6,    60,     8,    -1,    -1,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      46,    -1,    -1,    -1,    -1,    -1,    52,    -1,     4,    -1,
       6,    -1,     8,     9,    60,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    32,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      46,    -1,    -1,    -1,    -1,    -1,    52,     4,    -1,     6,
      -1,     8,     9,    -1,    60,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,
      -1,    -1,    -1,    -1,    51,    52,    -1,    -1,    55,    -1,
       4,     5,     6,    60,     8,    -1,    -1,    11,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    32,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,    52,     4,
      -1,     6,    -1,     8,     9,    -1,    60,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    46,    -1,    -1,    -1,    -1,    51,    52,    -1,     4,
      55,     6,    -1,     8,     9,    60,    -1,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    46,    -1,    -1,    -1,    -1,    51,    52,    -1,     4,
      55,     6,    -1,     8,     9,    60,    -1,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    46,    -1,    -1,    -1,    -1,    51,    52,    -1,     4,
      55,     6,    -1,     8,     9,    60,    -1,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    46,    -1,    -1,    -1,    -1,    51,    52,    -1,    -1,
      55,    -1,     4,     5,     6,    60,     8,    -1,    -1,    11,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      32,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      42,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,
      52,    -1,     4,    -1,     6,    -1,     8,    -1,    60,    11,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      32,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      42,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,
      52,    -1,     4,    -1,     6,    -1,     8,    -1,    60,    11,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      32,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      42,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,
      52,     4,    -1,     6,    -1,     8,    -1,    -1,    60,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    32,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
      -1,    -1,    -1,    46,    -1,    48,    -1,    -1,    -1,    52,
       4,    -1,     6,    -1,     8,    -1,    -1,    60,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    46,    -1,    -1,    -1,    -1,    51,    52,    -1,
       4,    55,     6,    -1,     8,    -1,    60,    -1,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    46,    -1,    -1,    -1,    -1,    51,    52,    -1,
       4,    -1,     6,    -1,     8,    -1,    60,    11,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,    52,     4,
      -1,     6,    -1,     8,    -1,    -1,    60,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    41,    42,    -1,    -1,
      -1,    46,    -1,    -1,    -1,    -1,    -1,    52,     4,    -1,
       6,    -1,     8,    -1,    -1,    60,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      46,    -1,    -1,    -1,    -1,    51,    52,     4,    -1,     6,
      -1,     8,     9,    -1,    60,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,
      -1,    -1,    -1,    -1,    -1,    52,     4,    -1,     6,    -1,
       8,    -1,    -1,    60,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,
      -1,    -1,    -1,    51,    52,     4,    -1,     6,    -1,     8,
       9,    -1,    60,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,    -1,
      -1,    -1,    -1,    52,     4,    -1,     6,     7,     8,    -1,
      -1,    60,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,    -1,    -1,    -1,    46,    -1,    -1,    -1,
      -1,    -1,    52,     4,    -1,     6,    -1,     8,     9,    -1,
      60,    -1,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,    30,
      31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
      -1,    42,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,
      -1,    52,     4,    -1,     6,     7,     8,    -1,    -1,    60,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      42,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,
      52,     4,    -1,     6,    -1,     8,     9,    -1,    60,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    -1,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
      -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,    52,
       4,    -1,     6,    -1,     8,    -1,    -1,    60,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    46,    -1,    -1,     4,    -1,     6,    52,     8,
      -1,    -1,    -1,    -1,    13,    -1,    60,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,     4,    42,     6,    -1,     8,    -1,    -1,    -1,
      49,    13,    -1,    52,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,     4,
      42,     6,    -1,     8,    -1,    -1,    -1,    49,    13,    -1,
      52,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,     4,    42,     6,    -1,
       8,    -1,    -1,    -1,    -1,    13,    -1,    52,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    52,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,    30,    44,    56,    63,    64,    66,    67,    68,    69,
      72,     6,    71,     0,     1,     4,     6,     8,    11,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    30,    31,    32,    33,    36,
      37,    38,    39,    42,    46,    52,    57,    60,    65,    83,
      84,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     104,   109,   118,   119,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,    30,     0,     1,     3,    10,   141,     0,    68,     7,
     126,    48,    70,    10,    48,     8,    51,   101,   102,   103,
     120,    41,    48,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    90,    91,   101,   103,   109,   118,   121,   122,
     101,     6,    14,   126,   126,     6,     8,    84,    86,   104,
     114,    13,   116,   117,   126,   128,    10,    40,   107,   126,
      39,    52,   134,   135,   137,    40,    57,   108,   126,   101,
     109,   110,     6,    51,    52,    57,    59,    91,    92,    11,
      87,    88,   101,   118,    10,   141,     8,    49,    86,    12,
      40,     8,    49,    27,    39,    39,     4,    65,    65,     0,
       7,    30,     4,   103,   103,   103,    10,   141,    48,    12,
     142,   103,    41,   103,   103,     9,    48,     9,    12,    41,
       9,    87,    10,    12,   142,   120,   126,    49,   120,     9,
      51,    55,   103,   111,   112,   113,    84,   104,    85,    86,
      12,     8,    49,     8,    49,   126,    12,    40,   142,     8,
      49,   126,    12,    40,   142,     4,     8,    13,    49,   125,
       5,    87,     7,    41,   113,     6,     8,   114,   126,    59,
      89,   101,   101,    10,    11,   141,    87,     0,     9,   113,
     101,    14,   126,    14,   126,   103,   101,    55,    78,    79,
      80,    81,    82,    90,   126,    32,    34,    35,    75,    76,
      77,    12,    12,   101,   103,    51,   103,     7,     9,   103,
      12,    41,     8,    86,   103,   103,     9,   103,   141,    87,
      51,   101,   121,     9,   142,   101,   142,    85,   103,   103,
      12,   142,     9,   116,   103,   101,   103,   101,    10,    12,
     126,   126,    10,   103,   101,    12,    57,   126,    57,   126,
     103,     8,   125,   141,    49,   103,     7,    41,     7,    41,
     113,     9,    85,     8,    49,   115,     8,   126,   134,    11,
      88,   101,     0,   141,    86,     9,   126,   126,     9,    79,
       4,    80,     5,    53,    54,    58,    73,    74,   140,    74,
      74,    10,   141,    82,    32,    48,   103,   105,   106,   126,
     105,   103,    48,     9,   103,     9,   103,     9,   113,    41,
       9,     9,   141,     7,     7,   111,     9,     9,   126,     8,
      49,    10,    12,     9,   126,    12,    57,     8,    49,   125,
       9,   103,     5,   101,     7,    49,     7,   103,    49,   103,
       7,    41,    85,     9,   113,   101,    85,   103,   101,     0,
      49,    85,     1,   110,    12,   116,    75,     5,   126,    46,
      48,    12,   142,    45,   126,   142,   103,    41,     9,    86,
       9,     9,   103,     9,    10,   103,   101,   126,    57,   126,
     103,     8,   125,     9,    49,   101,    49,     7,   101,     7,
      49,     7,   103,     9,     9,   101,     5,    29,    87,    73,
       8,    49,   103,   103,   106,     5,   103,    45,     5,     9,
     103,    85,     9,     9,    10,    57,     9,   103,   101,   101,
      49,    49,   101,    49,     7,    10,   141,   141,   103,   103,
     103,     9,     9,   101,   101,   101,    49,     5,    87,     5,
       9,   101,   141,     5
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    62,    63,    63,    63,    63,    63,    64,    64,    64,
      65,    65,    65,    65,    65,    66,    66,    66,    66,    66,
      67,    67,    68,    68,    69,    70,    70,    71,    71,    71,
      72,    73,    74,    74,    75,    75,    75,    76,    76,    77,
      77,    78,    78,    79,    79,    79,    79,    79,    80,    80,
      81,    81,    82,    82,    83,    83,    84,    84,    85,    85,
      86,    86,    86,    86,    87,    87,    87,    87,    87,    88,
      88,    89,    89,    89,    90,    90,    90,    90,    90,    90,
      90,    90,    91,    91,    92,    92,    93,    93,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    93,    93,    94,
      94,    94,    95,    95,    95,    96,    96,    96,    97,    97,
      97,    98,    98,    98,    98,    98,    99,    99,    99,    99,
      99,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   102,   102,   103,   104,   104,   105,   105,   106,
     106,   106,   106,   106,   106,   106,   107,   107,   107,   107,
     107,   107,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   110,   110,   111,   111,   111,   112,
     112,   113,   114,   115,   115,   115,   116,   116,   116,   116,
     116,   117,   117,   118,   119,   119,   119,   119,   119,   119,
     119,   119,   120,   120,   120,   120,   120,   120,   120,   121,
     121,   122,   122,   122,   123,   124,   124,   125,   125,   125,
     126,   126,   127,   127,   127,   127,   128,   128,   128,   129,
     130,   131,   131,   132,   132,   133,   133,   134,   134,   134,
     134,   135,   135,   135,   135,   136,   136,   137,   137,   138,
     138,   139,   139,   139,   139,   139,   139,   140,   140,   140,
     140,   141,   141,   142,   142
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     3,     2,     1,     2,     0,     5,     6,     4,
       1,     2,     5,     3,     6,     2,     2,     3,     3,     2,
       1,     2,     1,     1,     7,     0,     2,     0,     3,     2,
       5,     2,     1,     3,     2,     2,     2,     1,     3,     0,
       2,     1,     1,     6,     4,     8,     6,     3,     1,     2,
       1,     2,     0,     1,     1,     1,     5,     3,     0,     1,
       1,     1,     2,     2,     1,     3,     3,     4,     2,     1,
       1,     2,     2,     4,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     7,     6,     4,     5,
       4,     2,     5,     4,     5,     5,     2,     3,     4,     7,
       6,     6,     9,     8,     8,     6,     5,     5,     8,     7,
       7,     4,     5,     7,     6,     4,     6,     8,     7,     5,
       5,     1,     1,     1,     1,     1,     3,     2,     4,     3,
       1,     1,     1,     1,     1,     1,     1,     5,     4,     3,
       5,     1,     2,     3,     4,     6,     3,     4,     6,     5,
       7,     6,     1,     3,     2,     7,     7,     1,     3,     3,
       4,     4,     5,     1,     3,     3,     1,     3,     4,     3,
       5,     6,     2,     1,     3,     4,     5,     4,     3,     5,
       6,     7,     2,     4,     5,     7,     4,     6,     2,     4,
       5,     7,     4,     6,     0,     1,     2,     2,     1,     1,
       3,     2,     3,     0,     2,     3,     1,     3,     4,     3,
       4,     1,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     3,     2,     3,     5,     4,     3,
       2,     0,     1,     3,     4,     4,     5,     1,     1,     1,
       1,     1,     2,     3,     3,     4,     1,     3,     4,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       2,     1,     1,     2,     2,     1,     2,     0,     1,     1,
       1,     0,     1,     0,     1
  };




#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   174,   174,   175,   176,   177,   178,   186,   189,   192,
     201,   204,   206,   209,   212,   217,   220,   226,   229,   232,
     263,   265,   270,   271,   275,   280,   281,   285,   286,   287,
     291,   300,   305,   307,   312,   314,   316,   321,   323,   328,
     329,   337,   338,   342,   345,   348,   351,   354,   363,   364,
     376,   377,   381,   382,   390,   390,   393,   395,   400,   401,
     405,   406,   407,   408,   412,   413,   414,   419,   424,   432,
     433,   442,   443,   444,   448,   449,   450,   451,   452,   453,
     454,   455,   459,   460,   464,   466,   471,   474,   477,   481,
     484,   487,   490,   493,   496,   506,   509,   514,   520,   535,
     542,   549,   558,   565,   572,   582,   588,   594,   603,   609,
     615,   624,   629,   634,   639,   644,   652,   657,   663,   668,
     673,   686,   689,   692,   695,   698,   701,   708,   711,   714,
     717,   720,   723,   729,   730,   731,   732,   734,   737,   740,
     743,   746,   747,   749,   751,   754,   757,   760,   766,   769,
     772,   775,   783,   784,   788,   796,   799,   804,   806,   811,
     813,   815,   817,   819,   821,   823,   836,   838,   843,   846,
     849,   854,   860,   862,   864,   866,   868,   870,   872,   874,
     876,   878,   883,   884,   885,   887,   889,   894,   903,   905,
     907,   909,   911,   916,   928,   929,   934,   935,   936,   940,
     941,   944,   946,   949,   950,   952,   961,   963,   965,   967,
     969,   974,   976,   980,   988,   989,   990,   991,   992,   993,
     994,   995,   999,  1001,  1003,  1005,  1007,  1009,  1011,  1019,
    1021,  1026,  1028,  1030,  1035,  1040,  1042,  1051,  1052,  1053,
    1061,  1062,  1067,  1069,  1071,  1073,  1077,  1079,  1081,  1092,
    1094,  1097,  1098,  1102,  1103,  1107,  1108,  1112,  1113,  1114,
    1115,  1124,  1125,  1126,  1127,  1131,  1133,  1138,  1140,  1145,
    1147,  1152,  1153,  1154,  1155,  1156,  1157,  1161,  1162,  1163,
    1164,  1176,  1176,  1177,  1177
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  parser::symbol_kind_type
  parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61
    };
    // Last valid token kind.
    const int code_max = 316;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 7 "langutils/sc_parser/src/sc_grammar.y"
} } } // sc::ast::parser
#line 5708 "langutils/sc_parser/src/sc_grammar_parser.cpp"

#line 1179 "langutils/sc_parser/src/sc_grammar.y"

