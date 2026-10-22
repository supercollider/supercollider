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
          value.copy<RegionItemIndex>(YY_MOVE(that.value));
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
          value.move<RegionItemIndex>(YY_MOVE(s.value));
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
          value.YY_MOVE_OR_COPY<RegionItemIndex>(YY_MOVE(that.value));
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
          value.move<RegionItemIndex>(YY_MOVE(that.value));
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
          value.copy<RegionItemIndex>(that.value);
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
          value.move<RegionItemIndex>(that.value);
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
          yylhs.value.emplace<RegionItemIndex>();
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
  {
      yylhs.value.as<RegionItemIndex>() = yystack_[0].value.as<AnyExprIndex>();
  }
#line 2777 "langutils/sc_parser/src/sc_grammar_parser.cpp"
  break;

  case 11: // region.item: OPENPAREN argument_declarations block.contents semicolon.opt CLOSEPAREN
#line 204 "langutils/sc_parser/src/sc_grammar.y"
  {
      yylhs.value.as<RegionItemIndex>() =
          cxt.create(ImmediatelyEvaluatedBlock {}, yylhs.location, yystack_[3].value.as<DeclareArgumentListIndex>(),
                     yystack_[2].value.as<BlockContentsListIndex>());
  }
#line 2783 "langutils/sc_parser/src/sc_grammar_parser.cpp"
  break;

  case 12: // region.item: OPENPAREN argument_declarations CLOSEPAREN
#line 207 "langutils/sc_parser/src/sc_grammar.y"
  {
      yylhs.value.as<RegionItemIndex>() =
          cxt.create(ImmediatelyEvaluatedBlock {}, yylhs.location, yystack_[1].value.as<DeclareArgumentListIndex>(),
                     cxt.create(BlockContentsList {}, yystack_[0].location));
  }
#line 2789 "langutils/sc_parser/src/sc_grammar_parser.cpp"
  break;

  case 13: // region.item: OPENPAREN variable_declarations SEMICOLON block.contents semicolon.opt CLOSEPAREN
#line 210 "langutils/sc_parser/src/sc_grammar.y"
  {
      yylhs.value.as<RegionItemIndex>() = cxt.create(
          ImmediatelyEvaluatedBlock {}, yylhs.location, cxt.create(DeclareArgumentList {}, yystack_[5].location),
          cxt.graph->prepend(yystack_[2].value.as<BlockContentsListIndex>(),
                             yystack_[4].value.as<DeclareVariableListIndex>()));
  }
#line 2795 "langutils/sc_parser/src/sc_grammar_parser.cpp"
  break;

  case 14: // region: INTERPRET region.item
#line 215 "langutils/sc_parser/src/sc_grammar.y"
  {
      yylhs.value.as<RegionListIndex>() =
          cxt.create(RegionList {}, yylhs.location, yystack_[0].value.as<RegionItemIndex>());
  }
#line 2801 "langutils/sc_parser/src/sc_grammar_parser.cpp"
  break;

  case 15: // region: INTERPRET error
#line 218 "langutils/sc_parser/src/sc_grammar.y"
                {
			error_recovery::expr(cxt);
			yylhs.value.as < RegionListIndex > () = cxt.create(RegionList{}, yylhs.location, cxt.create(Error{}, yylhs.location));
		}
#line 2810 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

        case 16: // region: region SEMICOLON region.item
#line 224 "langutils/sc_parser/src/sc_grammar.y"
        {
            yylhs.value.as<RegionListIndex>() = cxt.graph->append(
                yystack_[2].value.as<RegionListIndex>(), yylhs.location, yystack_[0].value.as<RegionItemIndex>());
        }
#line 2816 "langutils/sc_parser/src/sc_grammar_parser.cpp"
        break;

        case 17: // region: region REGION_SEPARATOR region.item
#line 227 "langutils/sc_parser/src/sc_grammar.y"
        {
            yylhs.value.as<RegionListIndex>() = cxt.graph->append(
                yystack_[2].value.as<RegionListIndex>(), yylhs.location, yystack_[0].value.as<RegionItemIndex>());
        }
#line 2822 "langutils/sc_parser/src/sc_grammar_parser.cpp"
        break;

  case 18: // region: region error
#line 230 "langutils/sc_parser/src/sc_grammar.y"
                {
			if (yystack_[1].location.end.line_number != yystack_[0].location.begin.line_number){
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
#line 2844 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 19: // classOrExtList.list: classOrExtList.item
#line 258 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassOrExtensionListIndex > () = cxt.create(ClassOrExtensionList{}, yylhs.location, yystack_[0].value.as < ClassOrExtensionIndex > ()); }
#line 2850 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 20: // classOrExtList.list: classOrExtList.list classOrExtList.item
#line 260 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassOrExtensionListIndex > () = cxt.graph->append(yystack_[1].value.as < ClassOrExtensionListIndex > (), yystack_[0].value.as < ClassOrExtensionIndex > ()); }
#line 2856 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 21: // classOrExtList.item: class
#line 264 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassOrExtensionIndex > () = yystack_[0].value.as < ClassIndex > (); }
#line 2862 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 22: // classOrExtList.item: class.extension
#line 265 "langutils/sc_parser/src/sc_grammar.y"
                          { yylhs.value.as < ClassOrExtensionIndex > () = yystack_[0].value.as < ClassExtensionIndex > (); }
#line 2868 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 23: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY members.opt method.list.opt CLOSECURLY
#line 270 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassIndex > () = cxt.create(Class{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[6].location), yystack_[5].value.as < maybe<NamedIdentifierIndex> > (), yystack_[4].value.as < maybe<ClassNameIdentifierIndex> > (), yystack_[2].value.as < BigMemberListIndex > (), yystack_[1].value.as < MethodListIndex > ()); }
#line 2874 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 24: // class.super.opt: %empty
#line 274 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < maybe<ClassNameIdentifierIndex> > () = cxt.create(Missing{}, yylhs.location); }
#line 2880 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 25: // class.super.opt: COLON CLASSNAME
#line 275 "langutils/sc_parser/src/sc_grammar.y"
                          { yylhs.value.as < maybe<ClassNameIdentifierIndex> > () = cxt.create(ClassNameIdentifier{}, yystack_[0].location); }
#line 2886 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 26: // class.slot.opt: %empty
#line 279 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < maybe<NamedIdentifierIndex> > () = cxt.create(Missing{}, yylhs.location); }
#line 2892 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 27: // class.slot.opt: OPENSQUARE name CLOSESQUARE
#line 280 "langutils/sc_parser/src/sc_grammar.y"
                                      { yylhs.value.as < maybe<NamedIdentifierIndex> > () = yystack_[1].value.as < NamedIdentifierIndex > (); }
#line 2898 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 28: // class.slot.opt: OPENSQUARE CLOSESQUARE
#line 281 "langutils/sc_parser/src/sc_grammar.y"
                                 { yylhs.value.as < maybe<NamedIdentifierIndex> > () = cxt.create(Missing{}, yylhs.location); }
#line 2904 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 29: // class.extension: ADD CLASSNAME OPENCURLY method.list.opt CLOSECURLY
#line 286 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassExtensionIndex > () = cxt.create(ClassExtension{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[3].location), yystack_[1].value.as < MethodListIndex > ()); }
#line 2910 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 30: // members.entry.item: accessor variable_declarations.list.item
#line 295 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareMemberIndex > () = cxt.create(DeclareMember{}, yylhs.location, yystack_[1].value.as < RWAccessorIndex > (), yystack_[0].value.as < DeclareAnyVariableIndex > ()); }
#line 2916 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 31: // members.entry.list: members.entry.item
#line 300 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareMemberListIndex > () = cxt.create(DeclareMemberList{}, yylhs.location, yystack_[0].value.as < DeclareMemberIndex > ()); }
#line 2922 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 32: // members.entry.list: members.entry.list COMMA members.entry.item
#line 302 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareMemberListIndex > () = cxt.graph->append(yystack_[2].value.as < DeclareMemberListIndex > (), yystack_[0].value.as < DeclareMemberIndex > ()); }
#line 2928 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 33: // members.entry: CLASSVAR members.entry.list
#line 307 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyMemberListIndex > () = cxt.graph->cast<DeclareClassMemberList>(yystack_[0].value.as < DeclareMemberListIndex > ()); }
#line 2934 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 34: // members.entry: VAR members.entry.list
#line 309 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyMemberListIndex > () = cxt.graph->cast<DeclareMemberList>(yystack_[0].value.as < DeclareMemberListIndex > ()); }
#line 2940 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 35: // members.entry: CONST members.entry.list
#line 311 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyMemberListIndex > () = cxt.graph->cast<DeclareConstList>(yystack_[0].value.as < DeclareMemberListIndex > ()); }
#line 2946 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 36: // members: members.entry
#line 316 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BigMemberListIndex > () = cxt.create(BigMemberList{}, yylhs.location, yystack_[0].value.as < DeclareAnyMemberListIndex > ());}
#line 2952 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 37: // members: members SEMICOLON members.entry
#line 318 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BigMemberListIndex > () = cxt.graph->append(yystack_[2].value.as < BigMemberListIndex > (), yystack_[0].value.as < DeclareAnyMemberListIndex > ()); }
#line 2958 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 38: // members.opt: %empty
#line 322 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < BigMemberListIndex > () = cxt.create(BigMemberList{}, yylhs.location); }
#line 2964 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 39: // members.opt: members semicolon.opt
#line 323 "langutils/sc_parser/src/sc_grammar.y"
                                { yylhs.value.as < BigMemberListIndex > () = yystack_[1].value.as < BigMemberListIndex > (); }
#line 2970 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 40: // method.name: name
#line 331 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < SelectorIndex > () = yystack_[0].value.as < NamedIdentifierIndex > (); }
#line 2976 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 41: // method.name: binary_op.raw
#line 332 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < SelectorIndex > () = yystack_[0].value.as < SelectorIndex > (); }
#line 2982 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 42: // method.base: method.name OPENCURLY argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 337 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyMethodIndex > () = cxt.create(Method{}, yylhs.location, yystack_[5].value.as < SelectorIndex > (), yystack_[3].value.as < DeclareArgumentListIndex > (), cxt.create(Missing{},yystack_[4].location), yystack_[2].value.as < BlockContentsListIndex > ()); }
#line 2988 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 43: // method.base: method.name OPENCURLY argument_declarations.opt CLOSECURLY
#line 340 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyMethodIndex > () = cxt.create(Method{}, yylhs.location, yystack_[3].value.as < SelectorIndex > (), yystack_[1].value.as < DeclareArgumentListIndex > (), cxt.create(Missing{},yystack_[2].location), cxt.create(BlockContentsList{}, yystack_[0].location)); }
#line 2994 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 44: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt block.contents semicolon.opt CLOSECURLY
#line 343 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyMethodIndex > () = cxt.create(Method{}, yylhs.location, yystack_[7].value.as < SelectorIndex > (), yystack_[5].value.as < DeclareArgumentListIndex > (), cxt.create(PrimitiveIdentifier{}, yystack_[4].location), yystack_[2].value.as < BlockContentsListIndex > ()); }
#line 3000 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 45: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt CLOSECURLY
#line 346 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyMethodIndex > () = cxt.create(Method{}, yylhs.location, yystack_[5].value.as < SelectorIndex > (), yystack_[3].value.as < DeclareArgumentListIndex > (), cxt.create(PrimitiveIdentifier{}, yystack_[2].location), cxt.create(BlockContentsList{}, yystack_[1].location)); }
#line 3006 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 46: // method.base: method.name OPENCURLY error
#line 349 "langutils/sc_parser/src/sc_grammar.y"
                {
			error_recovery::method(cxt, yystack_[1].location); // advance over the closing curly of function.
			yylhs.value.as < AnyMethodIndex > () = cxt.create(Error{}, yylhs.location);
			yyclearin;
		}
#line 3016 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 47: // method: method.base
#line 357 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < AnyMethodIndex > () = yystack_[0].value.as < AnyMethodIndex > (); }
#line 3022 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 48: // method: MULTIPLY method.base
#line 359 "langutils/sc_parser/src/sc_grammar.y"
                { 
			if (auto c = cxt.graph->as_a<MethodIndex>(yystack_[0].value.as < AnyMethodIndex > ())) {
				yylhs.value.as < AnyMethodIndex > () = cxt.graph->cast<ClassMethod>(*c);
			} 
			// leaves as error
			cxt.graph->location(yystack_[0].value.as < AnyMethodIndex > ()) = yylhs.location; 
			yylhs.value.as < AnyMethodIndex > () = yystack_[0].value.as < AnyMethodIndex > (); 
		}
#line 3035 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 49: // method.list: method
#line 370 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < MethodListIndex > () = cxt.create(MethodList{}, yylhs.location, yystack_[0].value.as < AnyMethodIndex > ()); }
#line 3041 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 50: // method.list: method.list method
#line 371 "langutils/sc_parser/src/sc_grammar.y"
                             { yylhs.value.as < MethodListIndex > () = cxt.graph->append(yystack_[1].value.as < MethodListIndex > (), yylhs.location, yystack_[0].value.as < AnyMethodIndex > ()); }
#line 3047 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 51: // method.list.opt: %empty
#line 375 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < MethodListIndex > () = cxt.create(MethodList{}, yylhs.location); }
#line 3053 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 52: // method.list.opt: method.list
#line 376 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < MethodListIndex > () = yystack_[0].value.as < MethodListIndex > (); }
#line 3059 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 55: // block: block.open argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 388 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BlockIndex > () = cxt.create(BlockNode{}, yylhs.location, yystack_[3].value.as < DeclareArgumentListIndex > (), yystack_[2].value.as < BlockContentsListIndex > ()); }
#line 3065 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 56: // block: block.open argument_declarations.opt CLOSECURLY
#line 390 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BlockIndex > () = cxt.create(BlockNode{}, yylhs.location, yystack_[1].value.as < DeclareArgumentListIndex > (), cxt.create(BlockContentsList{}, yylhs.location)); }
#line 3071 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 57: // block.opt_list: %empty
#line 394 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < BlockListIndex > () = cxt.create(BlockList{}, yylhs.location); }
#line 3077 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 58: // block.opt_list: block.list
#line 395 "langutils/sc_parser/src/sc_grammar.y"
                     { yylhs.value.as < BlockListIndex > () = yystack_[0].value.as < BlockListIndex > (); }
#line 3083 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 59: // block.list: block
#line 399 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BlockListIndex > () = cxt.create(BlockList{}, yylhs.location, yystack_[0].value.as < BlockIndex > ()); }
#line 3089 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 60: // block.list: generator
#line 400 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < BlockListIndex > () = cxt.create(BlockList{}, yylhs.location, yystack_[0].value.as < GeneratorAnyIndex > ()); }
#line 3095 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 61: // block.list: block.list block
#line 401 "langutils/sc_parser/src/sc_grammar.y"
                           { yylhs.value.as < BlockListIndex > () = cxt.graph->append(yystack_[1].value.as < BlockListIndex > (), yylhs.location, yystack_[0].value.as < BlockIndex > ()); }
#line 3101 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 62: // block.list: block.list generator
#line 402 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < BlockListIndex > () = cxt.graph->append(yystack_[1].value.as < BlockListIndex > (), yylhs.location, yystack_[0].value.as < GeneratorAnyIndex > ()); }
#line 3107 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 63: // block.contents: block.contents.item
#line 406 "langutils/sc_parser/src/sc_grammar.y"
                              { yylhs.value.as < BlockContentsListIndex > () = cxt.create(BlockContentsList{}, yylhs.location, yystack_[0].value.as < BlockItemIndex > ()); }
#line 3113 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 64: // block.contents: block.contents SEMICOLON block.contents.item
#line 407 "langutils/sc_parser/src/sc_grammar.y"
                                                       { yylhs.value.as < BlockContentsListIndex > () = cxt.graph->append(yystack_[2].value.as < BlockContentsListIndex > (), yylhs.location, yystack_[0].value.as < BlockItemIndex > ()); }
#line 3119 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 65: // block.contents: block.contents NONLOCALRETURN expr
#line 409 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, yystack_[0].location, yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < BlockContentsListIndex > () = cxt.graph->append(yystack_[2].value.as < BlockContentsListIndex > (), yylhs.location, non_local); 
		}
#line 3128 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 66: // block.contents: block.contents SEMICOLON NONLOCALRETURN expr
#line 414 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, yystack_[0].location, yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < BlockContentsListIndex > () = cxt.graph->append(yystack_[3].value.as < BlockContentsListIndex > (), yylhs.location, non_local); 
		}
#line 3137 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 67: // block.contents: NONLOCALRETURN expr
#line 419 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, yystack_[0].location, yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < BlockContentsListIndex > () = cxt.create(BlockContentsList{}, yylhs.location, non_local);
		}
#line 3146 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 68: // block.contents.item: expr
#line 426 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < BlockItemIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 3152 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 69: // block.contents.item: variable_declarations
#line 427 "langutils/sc_parser/src/sc_grammar.y"
                                { yylhs.value.as < BlockItemIndex > () = yystack_[0].value.as < DeclareVariableListIndex > (); }
#line 3158 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 70: // adverb: DOT name
#line 436 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < AdverbIndex > () = yystack_[0].value.as < NamedIdentifierIndex > (); }
#line 3164 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 71: // adverb: DOT integer
#line 437 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < AdverbIndex > () = yystack_[0].value.as < IntLitIndex > (); }
#line 3170 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 72: // adverb: DOT OPENPAREN expr_or_seq CLOSEPAREN
#line 438 "langutils/sc_parser/src/sc_grammar.y"
                                               { yylhs.value.as < AdverbIndex > () = cxt.create(AdverbExprNode{}, yylhs.location, yystack_[1].value.as < AnyExprIndex > ());  }
#line 3176 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 73: // binary_op.raw: BINOP
#line 442 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3182 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 74: // binary_op.raw: READWRITEVAR
#line 443 "langutils/sc_parser/src/sc_grammar.y"
                       { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3188 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 75: // binary_op.raw: LESSTHAN
#line 444 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3194 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 76: // binary_op.raw: GREATERTHAN
#line 445 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3200 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 77: // binary_op.raw: MINUS
#line 446 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3206 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 78: // binary_op.raw: MULTIPLY
#line 447 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3212 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 79: // binary_op.raw: ADD
#line 448 "langutils/sc_parser/src/sc_grammar.y"
              { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3218 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 80: // binary_op.raw: PIPE
#line 449 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3224 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 81: // binary_op.no_adverb: binary_op.raw
#line 453 "langutils/sc_parser/src/sc_grammar.y"
                          { yylhs.value.as < SelectorIndex > () = yystack_[0].value.as < SelectorIndex > (); }
#line 3230 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 82: // binary_op.no_adverb: KEYBINOP
#line 454 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ true }, yylhs.location); }
#line 3236 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 83: // binary_op: binary_op.no_adverb adverb
#line 459 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < SelectorMaybeAdverbIndex > () = cxt.create(SelectorWAdverb{}, yylhs.location, yystack_[1].value.as < SelectorIndex > (), yystack_[0].value.as < AdverbIndex > ()); }
#line 3242 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 84: // binary_op: binary_op.no_adverb
#line 461 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < SelectorMaybeAdverbIndex > () = yystack_[0].value.as < SelectorIndex > (); }
#line 3248 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 85: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN arguments CLOSEPAREN block.opt_list
#line 466 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[5].value.as < SelectorIndex > (), yystack_[2].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ()); }
#line 3254 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 86: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN CLOSEPAREN block.list
#line 469 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[4].value.as < SelectorIndex > (), cxt.create(ArgumentList{}, yystack_[2].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3260 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 87: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN block.list
#line 472 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[2].value.as < SelectorIndex > (), cxt.create(ArgumentList{}, yystack_[0].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3266 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 88: // msgsend: name OPENPAREN arguments CLOSEPAREN block.opt_list
#line 476 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[4].value.as < NamedIdentifierIndex > (), yystack_[2].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ()); }
#line 3272 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 89: // msgsend: name OPENPAREN CLOSEPAREN block.list
#line 479 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), cxt.create(ArgumentList{}, yystack_[2].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3278 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 90: // msgsend: name block.list
#line 482 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[1].value.as < NamedIdentifierIndex > (), cxt.create(ArgumentList{}, yystack_[0].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3284 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 91: // msgsend: expr DOT name arguments.maybe_paren block.opt_list
#line 485 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PostfixMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ()); }
#line 3290 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 92: // msgsend: expr DOT arguments.paren block.opt_list
#line 488 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(ValueMessage{}, yylhs.location, yystack_[3].value.as < AnyExprIndex > (), yystack_[1].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ()); }
#line 3296 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 93: // msgsend: expr DOT OPENPAREN CLOSEPAREN block.opt_list
#line 491 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(ValueMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[2].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3302 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 94: // msgsend: CLASSNAME OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 501 "langutils/sc_parser/src/sc_grammar.y"
                {  yylhs.value.as < AnyExprIndex > () = cxt.create(CollectionNode{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[4].location), yystack_[2].value.as < ArrayIndex > ()); }
#line 3308 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 95: // msgsend: CLASSNAME block.list
#line 504 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[1].location);
			yylhs.value.as < AnyExprIndex > () = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, yystack_[1].location), yystack_[0].value.as < BlockListIndex > ());
		}
#line 3317 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 96: // msgsend: CLASSNAME arguments.paren block.opt_list
#line 509 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[2].location);
			yylhs.value.as < AnyExprIndex > () = cxt.create(NewMessage{}, yylhs.location, class_name, yystack_[1].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ());
		}
#line 3326 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 97: // msgsend: CLASSNAME OPENPAREN CLOSEPAREN block.opt_list
#line 515 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[3].location);
			yylhs.value.as < AnyExprIndex > () = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, yystack_[3].location), yystack_[0].value.as < BlockListIndex > ());
		}
#line 3335 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 98: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 530 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, yystack_[3].value.as < ArgumentListIndex > (), yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[6].value.as < AnyExprIndex > (), r);
		}
#line 3345 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 99: // expr.dotrange: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 537 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[3].location),  yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), r);
		}
#line 3355 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 100: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 544 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, yystack_[2].value.as < ArgumentListIndex > (), cxt.create(Missing{}, yystack_[1].location));
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), r);
		}
#line 3365 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 101: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 553 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, yystack_[5].value.as < ArgumentListIndex > (), yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[8].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3375 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 102: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 560 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, yystack_[4].value.as < ArgumentListIndex > (), cxt.create(Missing{}, yystack_[3].location));
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[7].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3385 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 103: // expr.dotrange.assign: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 567 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[5].location),  yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[7].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3395 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 104: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 577 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, yystack_[3].value.as < ArgumentListIndex > (), yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), r);
		}
#line 3405 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 105: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 583 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, yystack_[2].value.as < ArgumentListIndex > (), cxt.create(Missing{}, yystack_[0].location));
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), r);	
		}
#line 3415 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 106: // expr.base.range: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 589 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[3].location),  yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), r);	
		}
#line 3425 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 107: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 598 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, yystack_[5].value.as < ArgumentListIndex > (), yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[7].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3435 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 108: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 604 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, yystack_[4].value.as < ArgumentListIndex > (), cxt.create(Missing{}, yystack_[3].location));
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[6].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3445 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 109: // expr.base.range.assign: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 610 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[5].location),  yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[6].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3455 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 110: // range: OPENPAREN expr_or_seq DOTDOT CLOSEPAREN
#line 619 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, yystack_[2].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 3464 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 111: // range: OPENPAREN expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 624 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[4].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3473 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 112: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 629 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), yystack_[5].value.as < AnyExprIndex > (), yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[6].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3482 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 113: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 634 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[4].location, yystack_[2].location), yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[5].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 3491 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 114: // range: OPENPAREN DOTDOT expr_or_seq CLOSEPAREN
#line 639 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location);
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3500 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 115: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 647 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[5].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3509 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 116: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 652 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), yystack_[5].value.as < AnyExprIndex > (), yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[7].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3518 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 117: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 658 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[4].location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[6].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 3527 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 118: // range.generator: OPENPAREN COLON DOTDOT expr_or_seq CLOSEPAREN
#line 663 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location);
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3536 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 119: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT CLOSEPAREN
#line 668 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, yystack_[2].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 3545 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 120: // expr.base: literal
#line 681 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AnyPossiblyLiteralIndex > (); }
#line 3551 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 121: // expr.base: generator
#line 684 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < GeneratorAnyIndex > (); }
#line 3557 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 122: // expr.base: name
#line 687 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < NamedIdentifierIndex > (); }
#line 3563 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 123: // expr.base: CURRYARG
#line 690 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < AnyExprIndex > () = cxt.create(CurryNode{}, yystack_[0].location); }
#line 3569 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 124: // expr.base: msgsend
#line 693 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 3575 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 125: // expr.base: OPENPAREN expr_or_seq CLOSEPAREN
#line 696 "langutils/sc_parser/src/sc_grammar.y"
                { 
			// A nasty hack because of nil assignment.
			cxt.graph->register_bracketed_expr(yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AnyExprIndex > () = yystack_[1].value.as < AnyExprIndex > ();
		}
#line 3585 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 126: // expr.base: TILDE name
#line 703 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(EnvIdentifierNode{}, yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3591 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 127: // expr.base: expr.base OPENSQUARE arguments CLOSESQUARE
#line 706 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[3].value.as < AnyExprIndex > (), yystack_[1].value.as < ArgumentListIndex > () ); }
#line 3597 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 128: // expr.base: expr.base OPENSQUARE CLOSESQUARE
#line 709 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[2].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[1].location)); }
#line 3603 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 129: // expr.base: expr.base.range
#line 712 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AtMessageIndex > (); }
#line 3609 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 130: // expr.base: range
#line 715 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < RangeIndex > (); }
#line 3615 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 131: // expr.base: range.generator
#line 718 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < RangeGeneratorIndex > (); }
#line 3621 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 132: // expr: expr.base
#line 723 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 3627 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 133: // expr: expr.base.range.assign
#line 724 "langutils/sc_parser/src/sc_grammar.y"
                                 { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < RangeAssignIndex > (); }
#line 3633 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 134: // expr: expr.dotrange
#line 725 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AtMessageIndex > (); }
#line 3639 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 135: // expr: expr.dotrange.assign
#line 726 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < RangeAssignIndex > (); }
#line 3645 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 136: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE
#line 729 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[1].value.as < ArgumentListIndex > () ); }
#line 3651 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 137: // expr: expr DOT OPENSQUARE CLOSESQUARE
#line 732 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[3].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[1].location) ); }
#line 3657 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 138: // expr: expr binary_op expr
#line 735 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(BinaryMessage{}, yylhs.location, yystack_[1].value.as < SelectorMaybeAdverbIndex > (), yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3663 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 139: // expr: expr DOT name EQUALSSIGN expr
#line 738 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(SetterNode{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3669 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 140: // expr: CLASSNAME
#line 740 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < AnyExprIndex > () = cxt.create(ClassNameIdentifier{}, yylhs.location); }
#line 3675 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 141: // expr: BACKTICK expr
#line 741 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < AnyExprIndex > () = cxt.create(ReferenceNode{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 3681 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 142: // expr: name EQUALSSIGN expr
#line 743 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentNode{}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3687 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 143: // expr: TILDE name EQUALSSIGN expr
#line 746 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentNode{AssignmentNode::Target::Environment}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3693 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 144: // expr: name OPENPAREN arguments CLOSEPAREN EQUALSSIGN expr
#line 749 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(SetterNode{}, yylhs.location, yystack_[3].value.as < ArgumentListIndex > (), yystack_[5].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3699 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 145: // expr: name.pack EQUALSSIGN expr
#line 752 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(VariablePackAssign{}, yylhs.location, yystack_[2].value.as < NamePackIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3705 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 146: // expr: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 755 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(VariablePackAssign{}, yylhs.location, yystack_[3].value.as < NamePackIndex > (), yystack_[1].value.as < AnyExprIndex > ()); }
#line 3711 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 147: // expr: expr.base OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 761 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentAtNode{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), yystack_[3].value.as < ArgumentListIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3717 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 148: // expr: expr.base OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 764 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentAtNode{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[3].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3723 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 149: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 767 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentAtNode{}, yylhs.location, yystack_[6].value.as < AnyExprIndex > (), yystack_[3].value.as < ArgumentListIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3729 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 150: // expr: expr DOT OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 770 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentAtNode{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[3].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3735 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 151: // expr_or_seq.base: expr
#line 777 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 3741 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 152: // expr_or_seq.base: expr_or_seq.base SEMICOLON expr
#line 778 "langutils/sc_parser/src/sc_grammar.y"
                                          { yylhs.value.as < AnyExprIndex > () = cxt.create(ExprSeq{}, yylhs.location, yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3747 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 153: // expr_or_seq: expr_or_seq.base semicolon.opt
#line 782 "langutils/sc_parser/src/sc_grammar.y"
                                       { yylhs.value.as < AnyExprIndex > () = yystack_[1].value.as < AnyExprIndex > (); }
#line 3753 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 154: // generator: OPENCURLY COLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 791 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorAnyIndex > () = cxt.create(GeneratorNode{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < GeneratorQualListIndex > ()); }
#line 3759 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 155: // generator: OPENCURLY SEMICOLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 794 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorAnyIndex > () = cxt.create(GeneratorFakeNode{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < GeneratorQualListIndex > ()); }
#line 3765 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 156: // generator.qual.list: generator.qual
#line 799 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualListIndex > () = cxt.create(GeneratorQualList{}, yylhs.location, yystack_[0].value.as < GeneratorQualAnyIndex > ()); }
#line 3771 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 157: // generator.qual.list: generator.qual.list COMMA generator.qual
#line 801 "langutils/sc_parser/src/sc_grammar.y"
  {
      yylhs.value.as<GeneratorQualListIndex>() =
          cxt.graph->append(yystack_[2].value.as<GeneratorQualListIndex>(), yylhs.location,
                            yystack_[0].value.as<GeneratorQualAnyIndex>());
  }
#line 3777 "langutils/sc_parser/src/sc_grammar_parser.cpp"
  break;

  case 158: // generator.qual: name LEFTARROW expr_or_seq
#line 806 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualAssign{}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), cxt.create(Missing{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3783 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 159: // generator.qual: name name LEFTARROW expr_or_seq
#line 808 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualAssign{}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3789 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 160: // generator.qual: VAR name EQUALSSIGN expr_or_seq
#line 810 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualDeclare{}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3795 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 161: // generator.qual: VAR name OPENPAREN expr_or_seq CLOSEPAREN
#line 812 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualDeclare{}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ()); }
#line 3801 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 162: // generator.qual: expr_or_seq
#line 814 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualIf{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 3807 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 163: // generator.qual: COLON COLON expr_or_seq
#line 816 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualSideEffect{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 3813 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 164: // generator.qual: COLON WHILE expr_or_seq
#line 818 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualGuard{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 3819 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 165: // argument_declarations.list: name
#line 831 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3825 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 166: // argument_declarations.list: name EQUALSSIGN expr
#line 833 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed(yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{!is_bracketed}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ())); 
		}
#line 3834 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 167: // argument_declarations.list: name OPENPAREN expr_or_seq CLOSEPAREN
#line 838 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3840 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 168: // argument_declarations.list: argument_declarations.list COMMA name
#line 841 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[2].value.as < DeclareArgumentListIndex > (), yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3846 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 169: // argument_declarations.list: argument_declarations.list COMMA name EQUALSSIGN expr
#line 844 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed(yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[4].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{!is_bracketed}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()));
		}
#line 3855 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 170: // argument_declarations.list: argument_declarations.list COMMA name OPENPAREN expr_or_seq CLOSEPAREN
#line 849 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(yystack_[5].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3861 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 171: // argument_declarations.pipelist: name literal
#line 855 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, yystack_[1].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyPossiblyLiteralIndex > ())); }
#line 3867 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 172: // argument_declarations.pipelist: name
#line 857 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3873 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 173: // argument_declarations.pipelist: name EQUALSSIGN literal
#line 859 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyPossiblyLiteralIndex > ())); }
#line 3879 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 174: // argument_declarations.pipelist: name OPENPAREN expr_or_seq CLOSEPAREN
#line 861 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3885 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 175: // argument_declarations.pipelist: name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 863 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, yystack_[4].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3891 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 176: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name literal
#line 865 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[3].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, yystack_[1].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyPossiblyLiteralIndex > ())); }
#line 3897 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 177: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name
#line 867 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[2].value.as < DeclareArgumentListIndex > (), yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3903 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 178: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN literal
#line 869 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[4].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyPossiblyLiteralIndex > ())); }
#line 3909 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 179: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name OPENPAREN expr_or_seq CLOSEPAREN
#line 871 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[5].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3915 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 180: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 873 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[6].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, yystack_[4].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3921 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 181: // argument_declarations: ARG SEMICOLON
#line 877 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 3927 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 182: // argument_declarations: ARG argument_declarations.list comma.opt SEMICOLON
#line 878 "langutils/sc_parser/src/sc_grammar.y"
                                                             { yylhs.value.as < DeclareArgumentListIndex > () = yystack_[2].value.as < DeclareArgumentListIndex > (); }
#line 3933 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 183: // argument_declarations: ARG argument_declarations.list ELLIPSIS name SEMICOLON
#line 880 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[3].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); }
#line 3939 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 184: // argument_declarations: ARG argument_declarations.list ELLIPSIS name COMMA name SEMICOLON
#line 882 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[5].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, yystack_[3].value.as < NamedIdentifierIndex > ()), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); }
#line 3945 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 185: // argument_declarations: ARG ELLIPSIS name SEMICOLON
#line 884 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(arg_list, yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); 
		}
#line 3954 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 186: // argument_declarations: ARG ELLIPSIS name COMMA name SEMICOLON
#line 889 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(
				arg_list, yylhs.location,
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, yystack_[3].value.as < NamedIdentifierIndex > ()),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())
			); 
		}
#line 3967 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 187: // argument_declarations: PIPE PIPE
#line 898 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 3973 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 188: // argument_declarations: PIPE argument_declarations.pipelist comma.opt PIPE
#line 900 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = yystack_[2].value.as < DeclareArgumentListIndex > (); }
#line 3979 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 189: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name PIPE
#line 902 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(yystack_[3].value.as < DeclareArgumentListIndex > (), yystack_[3].location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); }
#line 3985 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 190: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name COMMA name PIPE
#line 904 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(yystack_[5].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, yystack_[3].value.as < NamedIdentifierIndex > ()), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); }
#line 3991 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 191: // argument_declarations: PIPE ELLIPSIS name PIPE
#line 906 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(arg_list, yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); 
		}
#line 4000 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 192: // argument_declarations: PIPE ELLIPSIS name COMMA name PIPE
#line 911 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(
				arg_list, yylhs.location,
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, yystack_[3].value.as < NamedIdentifierIndex > ()),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())
			); 
		}
#line 4013 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 193: // argument_declarations.opt: %empty
#line 922 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 4019 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 194: // argument_declarations.opt: argument_declarations
#line 923 "langutils/sc_parser/src/sc_grammar.y"
                                { yylhs.value.as < DeclareArgumentListIndex > () = yystack_[0].value.as < DeclareArgumentListIndex > (); }
#line 4025 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 195: // arguments.entries: KEYBINOP expr_or_seq
#line 928 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < ArgumentEntryIndex > () = cxt.create(KwArgNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4031 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 196: // arguments.entries: MULTIPLY expr_or_seq
#line 929 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < ArgumentEntryIndex > () = cxt.create(VariadicArgNode{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 4037 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 197: // arguments.entries: expr_or_seq
#line 930 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < ArgumentEntryIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 4043 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 198: // arguments.no_trailing: arguments.entries
#line 934 "langutils/sc_parser/src/sc_grammar.y"
                            {  yylhs.value.as < ArgumentListIndex > () = cxt.create(ArgumentList{}, yylhs.location, yystack_[0].value.as < ArgumentEntryIndex > ()); }
#line 4049 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 199: // arguments.no_trailing: arguments.no_trailing COMMA arguments.entries
#line 935 "langutils/sc_parser/src/sc_grammar.y"
                                                        { yylhs.value.as < ArgumentListIndex > () = cxt.graph->append(yystack_[2].value.as < ArgumentListIndex > (), yylhs.location, yystack_[0].value.as < ArgumentEntryIndex > ()); }
#line 4055 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 200: // arguments: arguments.no_trailing comma.opt
#line 938 "langutils/sc_parser/src/sc_grammar.y"
                                            { yylhs.value.as < ArgumentListIndex > () = yystack_[1].value.as < ArgumentListIndex > (); }
#line 4061 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 201: // arguments.paren: OPENPAREN arguments CLOSEPAREN
#line 940 "langutils/sc_parser/src/sc_grammar.y"
                                                 { yylhs.value.as < ArgumentListIndex > () = yystack_[1].value.as < ArgumentListIndex > (); }
#line 4067 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 202: // arguments.maybe_paren: %empty
#line 943 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < ArgumentListIndex > () = cxt.create(ArgumentList{}, yylhs.location); }
#line 4073 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 203: // arguments.maybe_paren: OPENPAREN CLOSEPAREN
#line 945 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArgumentListIndex > () = cxt.create(ArgumentList{}, yylhs.location); }
#line 4079 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 204: // arguments.maybe_paren: OPENPAREN arguments CLOSEPAREN
#line 947 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArgumentListIndex > () = yystack_[1].value.as < ArgumentListIndex > (); }
#line 4085 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 205: // variable_declarations.list.item: name
#line 956 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = yystack_[0].value.as < NamedIdentifierIndex > (); }
#line 4091 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 206: // variable_declarations.list.item: name EQUALSSIGN expr
#line 958 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4097 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 207: // variable_declarations.list.item: name OPENPAREN expr_or_seq CLOSEPAREN
#line 960 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ()); }
#line 4103 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 208: // variable_declarations.list.item: name.pack EQUALSSIGN expr
#line 962 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = cxt.create(VariablePackAssign{}, yylhs.location, yystack_[2].value.as < NamePackIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4109 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 209: // variable_declarations.list.item: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 964 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = cxt.create(VariablePackAssign{}, yylhs.location, yystack_[3].value.as < NamePackIndex > (), yystack_[1].value.as < AnyExprIndex > ()); }
#line 4115 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 210: // variable_declarations.list: variable_declarations.list.item
#line 969 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareVariableListIndex > () = cxt.create(DeclareVariableList{}, yylhs.location, yystack_[0].value.as < DeclareAnyVariableIndex > ()); }
#line 4121 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 211: // variable_declarations.list: variable_declarations.list COMMA variable_declarations.list.item
#line 971 "langutils/sc_parser/src/sc_grammar.y"
                {yylhs.value.as < DeclareVariableListIndex > () = cxt.graph->append(yystack_[2].value.as < DeclareVariableListIndex > (), yystack_[0].value.as < DeclareAnyVariableIndex > ()); }
#line 4127 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 212: // variable_declarations: VAR variable_declarations.list
#line 974 "langutils/sc_parser/src/sc_grammar.y"
                                                       { yylhs.value.as < DeclareVariableListIndex > () = yystack_[0].value.as < DeclareVariableListIndex > (); }
#line 4133 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 213: // literal.terminal: symbol
#line 982 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < AnySymbolIndex > (); }
#line 4139 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 214: // literal.terminal: string
#line 983 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < StringLitIndex > (); }
#line 4145 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 215: // literal.terminal: integer
#line 984 "langutils/sc_parser/src/sc_grammar.y"
                  { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < IntLitIndex > (); }
#line 4151 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 216: // literal.terminal: float
#line 985 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < FloatProducingIndex > (); }
#line 4157 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 217: // literal.terminal: boolean
#line 986 "langutils/sc_parser/src/sc_grammar.y"
                  { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < BooleanLitIndex > (); }
#line 4163 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 218: // literal.terminal: nil
#line 987 "langutils/sc_parser/src/sc_grammar.y"
              { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < NilLitIndex > (); }
#line 4169 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 219: // literal.terminal: ascii
#line 988 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < ASCIIIndex > (); }
#line 4175 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 220: // literal.terminal: block
#line 989 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < BlockIndex > (); }
#line 4181 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 221: // literal.array.contents: %empty
#line 994 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.create(ArrayNode{}, yylhs.location); }
#line 4187 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 222: // literal.array.contents: expr_or_seq
#line 996 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.create(ArrayNode{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 4193 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 223: // literal.array.contents: expr_or_seq COLON expr_or_seq
#line 998 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.create(ArrayNode{}, yylhs.location, yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4199 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 224: // literal.array.contents: KEYBINOP expr_or_seq
#line 1000 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.create(ArrayNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4205 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 225: // literal.array.contents: literal.array.contents COMMA expr_or_seq
#line 1002 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.graph->append(yystack_[2].value.as < ArrayIndex > (), yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 4211 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 226: // literal.array.contents: literal.array.contents COMMA expr_or_seq COLON expr_or_seq
#line 1004 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.graph->append(yystack_[4].value.as < ArrayIndex > (), yylhs.location, yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4217 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 227: // literal.array.contents: literal.array.contents COMMA KEYBINOP expr_or_seq
#line 1006 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.graph->append(yystack_[3].value.as < ArrayIndex > (), yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4223 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 228: // literal.dictionary.entry: expr COLON expr_or_seq
#line 1014 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryEntryIndex > () = cxt.create(DictionaryEntryNode{}, yylhs.location, yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4229 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 229: // literal.dictionary.entry: KEYBINOP expr_or_seq
#line 1016 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryEntryIndex > () = cxt.create(DictionaryEntryNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4235 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 230: // literal.dictionary.entries: %empty
#line 1021 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryIndex > () = cxt.create(DictionaryNode{}, yylhs.location); }
#line 4241 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 231: // literal.dictionary.entries: literal.dictionary.entry
#line 1023 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryIndex > () = cxt.create(DictionaryNode{}, yylhs.location, yystack_[0].value.as < DictionaryEntryIndex > ()); }
#line 4247 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 232: // literal.dictionary.entries: literal.dictionary.entries COMMA literal.dictionary.entry
#line 1025 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryIndex > () = cxt.graph->append(yystack_[2].value.as < DictionaryIndex > (), yylhs.location,  yystack_[0].value.as < DictionaryEntryIndex > ()); }
#line 4253 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 233: // literal.dictionary: OPENPAREN literal.dictionary.entries comma.opt CLOSEPAREN
#line 1030 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->location(*yystack_[2].value.as < DictionaryIndex > ()) = yylhs.location; yylhs.value.as < DictionaryIndex > () = yystack_[2].value.as < DictionaryIndex > (); }
#line 4259 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 234: // literal.array: OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1035 "langutils/sc_parser/src/sc_grammar.y"
                {  cxt.graph->location(*yystack_[2].value.as < ArrayIndex > ()) = yylhs.location; yylhs.value.as < ArrayIndex > () = yystack_[2].value.as < ArrayIndex > (); }
#line 4265 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 235: // literal.array: HASH OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1037 "langutils/sc_parser/src/sc_grammar.y"
                {  
			cxt.graph->payload(yystack_[2].value.as < ArrayIndex > ()).is_immutable = true;
			cxt.graph->location(*yystack_[2].value.as < ArrayIndex > ()) = yylhs.location;
			yylhs.value.as < ArrayIndex > () = yystack_[2].value.as < ArrayIndex > ();
		}
#line 4275 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 236: // literal: literal.terminal
#line 1045 "langutils/sc_parser/src/sc_grammar.y"
                           { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < AnyPossiblyLiteralIndex > (); }
#line 4281 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 237: // literal: literal.array
#line 1046 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < ArrayIndex > (); }
#line 4287 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 238: // literal: literal.dictionary
#line 1047 "langutils/sc_parser/src/sc_grammar.y"
                             { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < DictionaryIndex > (); }
#line 4293 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 239: // name: NAME
#line 1055 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < NamedIdentifierIndex > () = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 4299 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 240: // name: WHILE
#line 1056 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NamedIdentifierIndex > () = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 4305 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 241: // name.list: HASH name
#line 1062 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NameListIndex > () = cxt.create(NameList{}, yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 4311 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 242: // name.list: HASH TILDE name
#line 1064 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NameListIndex > () = cxt.create(NameList{}, yylhs.location, cxt.create(EnvIdentifierNode{}, yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ())); }
#line 4317 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 243: // name.list: name.list COMMA name
#line 1066 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NameListIndex > () = cxt.graph->append(yystack_[2].value.as < NameListIndex > (), yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 4323 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 244: // name.list: name.list COMMA TILDE name
#line 1068 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NameListIndex > () = cxt.graph->append(yystack_[3].value.as < NameListIndex > (), yylhs.location, cxt.create(EnvIdentifierNode{},yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ())); }
#line 4329 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 245: // name.pack: name.list
#line 1072 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NamePackIndex > () = cxt.create(NamePack{}, yylhs.location, yystack_[0].value.as < NameListIndex > (), cxt.create(Missing{}, yystack_[0].location)); }
#line 4335 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 246: // name.pack: name.list ELLIPSIS name
#line 1074 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NamePackIndex > () = cxt.create(NamePack{}, yylhs.location, yystack_[2].value.as < NameListIndex > (), yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 4341 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 247: // name.pack: name.list ELLIPSIS TILDE name
#line 1076 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NamePackIndex > () = cxt.create(NamePack{}, yylhs.location, yystack_[3].value.as < NameListIndex > (), cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), yystack_[0].value.as < NamedIdentifierIndex > ())); }
#line 4347 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 248: // ascii: ASCII
#line 1086 "langutils/sc_parser/src/sc_grammar.y"
              { yylhs.value.as < ASCIIIndex > () = cxt.create(ASCIINode{}, yylhs.location); }
#line 4353 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 249: // nil: NIL
#line 1088 "langutils/sc_parser/src/sc_grammar.y"
          { yylhs.value.as < NilLitIndex > () = cxt.create(NilNode{}, yylhs.location); }
#line 4359 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 250: // boolean: TRUE
#line 1091 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < BooleanLitIndex > () = cxt.create(BooleanNode{true}, yylhs.location); }
#line 4365 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 251: // boolean: FALSE
#line 1092 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BooleanLitIndex > () = cxt.create(BooleanNode{false}, yylhs.location); }
#line 4371 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 252: // symbol: SYMBOL_QUOTE
#line 1096 "langutils/sc_parser/src/sc_grammar.y"
                       { yylhs.value.as < AnySymbolIndex > () = cxt.create(SymbolQuote{}, yylhs.location); }
#line 4377 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 253: // symbol: SYMBOL_SLASH
#line 1097 "langutils/sc_parser/src/sc_grammar.y"
                       { yylhs.value.as < AnySymbolIndex > () = cxt.create(SymbolSlash{}, yylhs.location); }
#line 4383 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 254: // string: STRINGLINE
#line 1101 "langutils/sc_parser/src/sc_grammar.y"
                     { yylhs.value.as < StringLitIndex > () = cxt.create(StringLineList{}, yylhs.location, cxt.create(StringLineNode{}, yylhs.location)); }
#line 4389 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 255: // string: string STRINGLINE
#line 1102 "langutils/sc_parser/src/sc_grammar.y"
                            { yylhs.value.as < StringLitIndex > () = cxt.graph->append(yystack_[1].value.as < StringLitIndex > (), cxt.create(StringLineNode{}, yystack_[0].location)); }
#line 4395 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 256: // integer: INTEGER
#line 1106 "langutils/sc_parser/src/sc_grammar.y"
                  { yylhs.value.as < IntLitIndex > () = cxt.create(IntNode{}, yylhs.location); }
#line 4401 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 257: // integer: INTEGER_RADIX
#line 1107 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < IntLitIndex > () = cxt.create(IntNode{IntNode::Kind::Radix}, yylhs.location); }
#line 4407 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 258: // integer: HEXADECIMAL
#line 1108 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < IntLitIndex > () = cxt.create(IntNode{IntNode::Kind::Hexadecimal}, yylhs.location); }
#line 4413 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 259: // integer: MINUS integer
#line 1110 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Reaches into the previous integer and changes its sign.
			cxt.graph->payload(yystack_[0].value.as < IntLitIndex > ()).sign = IntNode::Sign::Negative;
			yylhs.value.as < IntLitIndex > () = yystack_[0].value.as < IntLitIndex > ();
		}
#line 4423 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 260: // float.raw_unsigned: FLOAT
#line 1118 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < FloatLitIndex > () = cxt.create(FloatNode{}, yylhs.location); }
#line 4429 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 261: // float.raw_unsigned: FLOAT_RADIX
#line 1119 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < FloatLitIndex > () = cxt.create(FloatNode{FloatNode::Kind::Radix}, yylhs.location); }
#line 4435 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 262: // float.raw_unsigned: FLOAT_EXPONENT
#line 1120 "langutils/sc_parser/src/sc_grammar.y"
                         { yylhs.value.as < FloatLitIndex > () = cxt.create(FloatNode{FloatNode::Kind::Exponent}, yylhs.location); }
#line 4441 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 263: // float.raw_unsigned: FLOAT_INF
#line 1121 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < FloatLitIndex > () = cxt.create(FloatNode{FloatNode::Kind::Inf}, yylhs.location); }
#line 4447 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 264: // float.raw: float.raw_unsigned
#line 1126 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < FloatLitIndex > () = yystack_[0].value.as < FloatLitIndex > (); }
#line 4453 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 265: // float.raw: MINUS float.raw_unsigned
#line 1128 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload(yystack_[0].value.as < FloatLitIndex > ()).sign = FloatNode::Sign::Negative; yylhs.value.as < FloatLitIndex > () = yystack_[0].value.as < FloatLitIndex > (); }
#line 4459 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 266: // accidental.unsigned: ACCIDENTAL_STEPS
#line 1133 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AccidentalIndex > () = cxt.create(AccidentalNode{AccidentalNode::Kind::Steps}, yylhs.location); }
#line 4465 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 267: // accidental.unsigned: ACCIDENTAL_CENTS
#line 1135 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AccidentalIndex > () = cxt.create(AccidentalNode{AccidentalNode::Kind::Cents}, yylhs.location); }
#line 4471 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 268: // accidental: accidental.unsigned
#line 1140 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AccidentalIndex > () = yystack_[0].value.as < AccidentalIndex > (); }
#line 4477 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 269: // accidental: MINUS accidental.unsigned
#line 1142 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload(yystack_[0].value.as < AccidentalIndex > ()).sign = AccidentalNode::Sign::Negative; yylhs.value.as < AccidentalIndex > () = yystack_[0].value.as < AccidentalIndex > (); }
#line 4483 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 270: // float: float.raw
#line 1146 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < FloatProducingIndex > () = yystack_[0].value.as < FloatLitIndex > (); }
#line 4489 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 271: // float: accidental
#line 1147 "langutils/sc_parser/src/sc_grammar.y"
                     { yylhs.value.as < FloatProducingIndex > () = yystack_[0].value.as < AccidentalIndex > (); }
#line 4495 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 272: // float: float.raw PI
#line 1148 "langutils/sc_parser/src/sc_grammar.y"
                       { yylhs.value.as < FloatProducingIndex > () = cxt.create(PiNode{}, yylhs.location, yystack_[1].value.as < FloatLitIndex > ()); }
#line 4501 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 273: // float: integer PI
#line 1149 "langutils/sc_parser/src/sc_grammar.y"
                     { yylhs.value.as < FloatProducingIndex > () = cxt.create(PiNode{}, yylhs.location, yystack_[1].value.as < IntLitIndex > ()); }
#line 4507 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 274: // float: PI
#line 1150 "langutils/sc_parser/src/sc_grammar.y"
              { yylhs.value.as < FloatProducingIndex > () = cxt.create(PiNode{}, yylhs.location, cxt.create(Missing{}, yylhs.location)); }
#line 4513 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 275: // float: MINUS PI
#line 1151 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < FloatProducingIndex > () = cxt.create(PiNode{PiNode::Sign::Negative}, yylhs.location, cxt.create(Missing{}, yylhs.location)); }
#line 4519 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 276: // accessor: %empty
#line 1155 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < RWAccessorIndex > () = cxt.create(RWAccessor{ReadWriteAccessor::Private}, yylhs.location); }
#line 4525 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 277: // accessor: LESSTHAN
#line 1156 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < RWAccessorIndex > () = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyReadable}, yylhs.location); }
#line 4531 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 278: // accessor: GREATERTHAN
#line 1157 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < RWAccessorIndex > () = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyWriteable}, yylhs.location); }
#line 4537 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 279: // accessor: READWRITEVAR
#line 1159 "langutils/sc_parser/src/sc_grammar.y"
                { 
			yylhs.value.as < RWAccessorIndex > () = cxt.create(RWAccessor{
				combine<ReadWriteAccessor::ExternallyReadable, ReadWriteAccessor::ExternallyWriteable>()
			}, yylhs.location); 
		}
#line 4547 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;


#line 4551 "langutils/sc_parser/src/sc_grammar_parser.cpp"

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








  const short parser::yypact_ninf_ = -211;

  const short parser::yytable_ninf_ = -281;

  const short
  parser::yypact_[] =
  {
      -3,    14,   738,     4,    38,  -211,   228,    16,  -211,  -211,
    -211,   137,     6,  -211,  -211,     9,  1653,   788,   105,   102,
    -211,  -211,  -211,  -211,  -211,  -211,  -211,  -211,  -211,  -211,
    -211,  -211,  -211,  -211,    43,  -211,    37,   147,  -211,  -211,
    -211,  -211,  -211,  -211,   255,    78,  2143,  -211,   190,  -211,
    -211,  -211,  -211,  -211,  -211,  -211,  -211,   100,   485,  -211,
    1455,   149,  -211,  -211,  -211,  -211,    13,   157,    10,  -211,
    -211,  -211,  -211,   167,    71,  -211,   135,  -211,  -211,  -211,
     111,  -211,  -211,  2192,  2192,   216,  -211,  -211,  -211,   199,
     204,   240,  2143,  2143,   843,  2143,   485,   242,   209,   249,
    2143,  1702,  -211,  2143,   255,  -211,  -211,  -211,  -211,    78,
    -211,  -211,   245,   380,   195,  1051,   253,  -211,   280,  1653,
     102,  -211,   248,  1653,  1100,  -211,    83,  -211,    83,   196,
    -211,   286,    48,    63,  -211,   102,   168,    88,  -211,   161,
    -211,  -211,  -211,   102,  -211,   177,  2237,  -211,  -211,  1153,
     898,  -211,  -211,  -211,   130,   241,  2143,  2143,   277,  -211,
     485,  -211,  1455,   265,  1202,  2143,    83,   200,   207,  2143,
    2143,  -211,  -211,  -211,    76,  -211,  -211,  -211,  -211,  -211,
     250,   289,   290,  -211,  2143,  -211,  2143,  1751,   296,   295,
    2143,   189,  -211,   182,  2143,  -211,  2143,  1800,  -211,   277,
    1455,  1849,   299,   249,  -211,  2143,   249,    83,  2143,  2143,
    -211,  -211,   293,   300,  -211,  -211,  -211,    83,    37,  2143,
    2143,  2143,  2143,   215,   102,   102,   301,  2143,  2143,    12,
    -211,   102,    27,  -211,  1849,   304,  2311,  -211,  -211,   277,
     266,  2143,   151,   948,  1252,    83,    91,    62,  -211,   257,
     485,  1505,  2143,   319,   277,  -211,    83,   320,   485,   102,
    -211,   102,  -211,   322,   485,   268,   324,  -211,  -211,    76,
     328,  -211,  -211,   202,   202,   202,  -211,   325,    76,  1554,
    1554,   485,  -211,  2143,   288,  -211,  -211,   330,  2143,  1898,
    1302,    83,  -211,   302,  -211,   332,   333,   277,  2143,   380,
    -211,  -211,   339,   485,   342,  -211,  -211,  -211,  1603,  -211,
    -211,  -211,   341,   485,   344,   485,  -211,   102,    97,   225,
    -211,   348,   485,   102,  -211,    19,  -211,  2274,   350,  1849,
    -211,   351,  2143,   356,   315,  1947,   318,  2143,   164,    83,
    -211,  1352,  2143,    83,  2143,  -211,  -211,  2143,  -211,   485,
    -211,   337,    83,    26,  -211,  -211,  -211,  -211,   670,  -211,
    -211,  -211,  -211,  -211,  -211,   357,    37,   357,   357,   250,
    -211,   363,   102,   197,  -211,   359,  -211,    18,   359,  -211,
    2143,  -211,   334,  -211,   364,    83,   365,  1996,  -211,  -211,
     367,  -211,  -211,  -211,  -211,  -211,   368,  2143,  2143,  -211,
     102,  -211,   329,   102,  -211,  1849,  2348,  -211,  -211,   370,
    -211,   485,   336,  2143,   338,   374,  2143,   375,   340,  2045,
    -211,  -211,   379,   485,  -211,   382,   485,  -211,  2143,  -211,
    -211,  1001,   202,  -211,  -211,  -211,   101,  2143,  2143,  1554,
     372,  2143,   349,   388,  -211,  2094,  -211,    83,    83,  -211,
     386,  -211,  -211,   389,   485,   387,  -211,   343,   390,  1849,
    -211,  -211,  2143,   485,  2143,   353,   485,   361,  2143,   366,
     398,  -211,  -211,   485,  -211,   402,   277,  -211,  2143,  2143,
    -211,  -211,  -211,  -211,  -211,  2143,  -211,  -211,   399,  -211,
    -211,  -211,  -211,  -211,  -211,   404,   485,   485,  2143,  2143,
     485,  2143,   371,  -211,  1405,   414,   412,  -211,  -211,  -211,
    -211,   485,   485,   485,  2143,  -211,   277,  -211,  -211,   485,
     417,  -211
  };

  const short
  parser::yydefact_[] =
  {
       6,    26,     0,     0,     0,     4,     0,     0,    19,    21,
      22,     0,    24,     3,    15,    53,   221,   230,     0,     0,
     239,   256,   257,   258,   260,   261,   262,   263,   266,   267,
     252,   253,   254,   248,   140,   123,     0,     0,   249,   250,
     251,   274,    54,   240,     0,     0,     0,    14,   193,   220,
     124,   134,   135,   129,   133,   130,   131,   132,    10,   121,
       0,   280,   236,   238,   237,   120,   122,   245,     0,   219,
     218,   217,   213,   214,   215,   264,   270,   268,   271,   216,
       0,     1,    18,     0,   281,     0,     5,    20,    28,     0,
       0,     0,     0,     0,   230,     0,   151,   280,   222,   282,
       0,     0,    73,    82,    77,    75,    76,    78,    79,    80,
      74,    81,     0,   151,     0,     0,     0,   231,   282,   221,
       0,   241,   126,   221,     0,    59,    95,    60,    57,     0,
     210,   212,   205,     0,   181,     0,   282,   165,   275,     0,
     259,   265,   269,     0,   187,   282,   172,   141,   194,     0,
       0,    82,    77,    80,     0,    84,     0,     0,   280,    63,
      68,    69,   281,     0,     0,     0,    90,     0,     0,     0,
       0,   255,   273,   272,    51,    17,    16,     2,    27,    25,
      38,     0,     0,   224,   281,   153,     0,   283,     0,     0,
       0,     0,   229,     0,     0,   125,     0,     0,    12,   280,
       0,   283,     0,   282,   242,     0,   282,    57,     0,     0,
     197,   198,   282,     0,    61,    62,    96,    58,     0,     0,
       0,     0,     0,     0,   283,     0,     0,     0,     0,     0,
     283,     0,     0,    53,   230,     0,     0,   171,    56,   280,
     128,     0,     0,     0,     0,    57,   202,     0,    83,   138,
      67,   281,     0,     0,   280,     9,     0,     0,   142,     0,
     243,     0,   246,     0,   145,    78,     0,    47,    49,    52,
       0,    41,    40,   276,   276,   276,    36,   280,    51,     0,
       0,   152,   223,     0,   225,   234,   114,     0,     0,     0,
       0,    87,   228,     0,   110,     0,     0,   280,     0,     0,
     232,   233,     0,   143,     0,    97,   195,   196,   283,   200,
     201,   211,     0,   206,     0,   208,   185,     0,   168,     0,
     182,     0,   166,     0,   191,     0,   188,   177,     0,   230,
     173,     0,     0,     0,   127,     0,   137,     0,     0,    57,
      92,     0,     0,    57,     0,    70,    71,     0,    64,    65,
       7,     0,    89,    57,   244,   247,   146,    48,     0,    50,
      29,   277,   278,   279,    31,    34,     0,    33,    35,   281,
      39,     0,     0,     0,   162,   282,   156,   122,   282,   227,
       0,   118,     0,   119,     0,     0,     0,     0,   111,    11,
       0,   235,    94,   199,   207,   209,     0,     0,     0,   183,
       0,   167,     0,     0,   189,   230,     0,   176,   174,     0,
      55,   148,   106,     0,   105,     0,     0,     0,   136,     0,
      93,   203,     0,   139,    91,     0,    66,     8,     0,    88,
      46,     0,   276,    30,    37,    23,     0,     0,     0,   283,
       0,     0,     0,     0,   226,     0,   115,    86,    57,   113,
       0,    13,   186,     0,   169,     0,   192,     0,     0,   230,
     178,   175,     0,   147,     0,   104,   150,    99,     0,   100,
       0,   204,    72,   144,    43,   280,   280,    32,     0,     0,
     164,   163,   157,   155,   158,     0,   154,   117,     0,    85,
     112,   170,   184,   190,   179,     0,   109,   108,     0,     0,
     149,     0,    98,   281,     0,     0,     0,   160,   159,   116,
     180,   107,   103,   102,     0,    45,   280,    42,   161,   101,
       0,    44
  };

  const short
  parser::yypgoto_[] =
  {
    -211,  -211,  -211,   206,  -211,  -211,   416,  -211,  -211,  -211,
    -211,    -8,    21,    56,  -211,  -211,  -211,   162,   171,  -211,
     163,  -211,    -5,  -200,   -23,  -114,   194,  -211,  -162,    -4,
    -211,  -211,  -211,  -211,  -211,  -211,  -211,  -211,  -211,    -2,
    -211,    72,   278,   166,     8,  -211,  -211,    23,    90,   141,
    -211,  -141,   297,  -211,  -210,  -211,   125,  -211,   -87,   252,
    -211,  -211,  -211,  -142,   347,  -211,   -34,  -211,  -211,  -211,
    -211,  -211,   -38,   -39,  -211,   -30,  -211,  -211,  -211,   -58,
    -108
  };

  const short
  parser::yydefgoto_[] =
  {
       0,     4,     5,    47,     6,     7,     8,     9,    91,    12,
      10,   364,   365,   276,   277,   278,   266,   267,   268,   269,
     270,    48,    49,   216,   217,   158,   159,   248,   111,   155,
     156,    50,    51,    52,    53,    54,    55,    56,    57,    96,
      97,   210,    59,   375,   376,   136,   145,   148,   149,   211,
     212,   213,   128,   343,   130,   131,   161,    62,    99,   117,
     118,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,   366,    85,
     188
  };

  const short
  parser::yytable_[] =
  {
      58,   199,   133,   163,   237,   141,   140,   305,   311,   242,
     202,   126,   271,   112,   142,   113,    86,    15,   169,    92,
      11,   164,    15,   257,   323,    60,   164,     1,   226,   125,
      15,   403,   203,    20,    80,   239,   206,   232,    81,   185,
     115,     2,    20,   166,   147,   340,     1,    15,   254,   123,
     129,   124,    20,     3,    90,    42,   219,    93,   160,   170,
      42,   125,   165,   441,    43,   141,   140,   165,    42,   324,
     344,   221,     3,    43,   142,   428,   404,    20,    21,    22,
      23,    58,    58,    43,   326,    42,   297,    15,    98,   114,
     112,    20,   113,    20,   330,   302,   227,   220,   304,   341,
     253,   140,   338,   271,   309,   397,   150,   271,    43,   478,
     172,   119,   222,   160,   139,   174,   271,    20,   143,   120,
      20,   214,    43,   125,    43,    42,   102,    61,   152,   105,
     106,   265,   108,   153,   110,   144,   243,   228,   244,   420,
     342,   296,   116,   424,    88,    20,   398,   160,    43,   386,
     479,    43,    20,   429,   249,   250,   433,   134,   334,   162,
     160,   214,    20,   258,   181,   182,   114,   183,   264,   167,
     291,   418,   189,   191,   173,   192,    43,    21,    22,    23,
     224,   331,   281,    43,   133,   407,    15,   135,   125,   230,
     290,    98,   335,    43,   171,    98,   351,   168,   160,   299,
     422,   288,   125,   303,   195,   419,   178,   196,   225,   346,
     120,    20,   214,   139,   259,    20,   177,   231,   313,   370,
     315,   261,    20,    37,    42,   316,   322,   317,  -280,    82,
     289,    83,   113,   352,   179,   399,   197,   400,    84,   390,
     125,   263,    43,   437,   180,   438,    43,    45,   489,   160,
     349,   125,   184,    43,   193,   361,   362,   186,   282,   284,
     363,   187,   287,   200,   460,   255,   292,   440,   293,   295,
     443,    21,    22,    23,    24,    25,    26,    27,    28,    29,
     306,   307,   273,    20,   274,   275,   214,   251,   252,   175,
     176,   312,   201,   314,   138,   367,   368,   205,   218,   321,
     247,   279,   280,   285,   286,   308,   328,   139,   301,   310,
     119,   320,   127,   333,    43,   332,   154,   476,   102,   350,
     152,   105,   106,   107,   108,   153,   110,   113,   358,   353,
     411,   356,   133,   360,   125,   369,   380,   427,   125,   381,
     423,   388,   389,   387,   127,   426,   391,   214,   125,   392,
     394,   374,   374,   395,   166,   379,   410,   401,    89,   408,
     382,   384,   447,   412,   413,   121,   122,   416,   435,   432,
     192,   439,   125,   446,   448,   445,   451,   483,   452,   461,
     125,   465,   467,   132,   137,   462,   456,   464,   471,   468,
     516,   472,   146,   486,   485,   490,   454,   492,   491,   494,
     493,   409,   498,   113,   215,   502,   127,   415,   509,   417,
     499,   463,   503,   510,   466,   501,   425,   504,   505,   517,
     514,   518,   521,    87,   477,   434,   473,   357,   194,   160,
     102,   151,   152,   105,   106,   107,   108,   153,   110,   154,
     359,   371,   214,   125,   215,   348,   378,   482,   431,   393,
       0,   245,   444,   300,     0,     0,   146,   113,   520,   450,
     496,     0,   497,     0,     0,     0,   500,   204,     0,   453,
       0,   127,     0,     0,     0,     0,   121,   458,     0,     0,
       0,     0,   223,     0,     0,   127,     0,     0,     0,     0,
     229,   470,     0,     0,     0,   215,   511,   512,     0,   513,
       0,   246,   160,     0,     0,     0,     0,     0,     0,   480,
     481,   374,   519,   484,   260,   262,     0,   488,     0,     0,
       0,   272,     0,   127,     0,     0,     0,     0,     0,     0,
       0,   495,     0,     0,   127,   102,   151,   152,   105,   106,
     107,   108,   153,   110,   154,     0,     0,     0,     0,     0,
     506,   507,     0,     0,     0,     0,     0,   508,     0,     0,
       0,     0,     0,     0,     0,   132,     0,     0,     0,   215,
       0,   318,   319,     0,     0,     0,     0,     0,   325,   327,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   345,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   354,     0,   355,     0,
       0,     0,   272,     0,     0,     0,   272,   127,     0,     0,
       0,   127,     0,     0,     0,   272,   377,   377,     0,     0,
     215,   127,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   127,     0,     0,     0,     0,
       0,     0,     0,   127,   396,     0,     0,     0,     0,     0,
     402,   430,     0,     0,  -193,  -193,  -193,     0,  -193,     0,
       0,  -193,     0,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,  -193,
    -193,  -193,  -193,    37,     0,     0,  -193,  -193,  -193,  -193,
       0,     0,  -193,   132,     0,     0,  -193,     0,     0,   436,
       0,     0,  -193,     0,   442,   215,   127,    45,     0,     0,
    -193,     0,     0,     0,     0,     0,     0,     0,    13,    14,
       0,     0,    15,     0,    16,     0,    17,   455,     0,     0,
     457,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,     0,    34,    35,
      36,    37,     0,     0,    38,    39,    40,    41,     0,     0,
      42,     0,     0,     0,    43,     0,   377,     0,     0,     0,
      44,     0,    15,     0,    16,    45,    94,     0,    46,     0,
       0,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,     0,    34,    35,
      36,    37,     0,     0,    38,    39,    40,    41,     0,   100,
      42,     0,     0,     0,    43,     0,   101,     0,   102,   103,
     104,   105,   106,   107,   108,   109,   110,    15,    46,    16,
       0,    94,     0,     0,     0,     0,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,     0,    34,    35,     0,     0,     0,     0,    38,
      39,    40,    41,     0,   100,    42,     0,     0,     0,    43,
       0,   101,     0,   102,   103,   104,   105,   106,   107,   108,
     153,   110,    15,    46,    16,   240,    94,     0,     0,     0,
       0,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,     0,    34,    35,
       0,     0,     0,     0,    38,    39,    40,    41,     0,   241,
      42,     0,     0,     0,    43,     0,     0,     0,     0,   208,
      44,     0,    15,   209,    16,   336,    94,     0,    46,     0,
       0,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,     0,    34,    35,
       0,     0,     0,     0,    38,    39,    40,    41,     0,   337,
      42,     0,     0,     0,    43,     0,     0,     0,     0,   208,
      44,     0,     0,   209,     0,    15,   474,    16,    46,    94,
       0,     0,   157,     0,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
     475,    34,    35,    36,     0,     0,     0,    38,    39,    40,
      41,     0,     0,    42,     0,     0,     0,    43,     0,     0,
       0,     0,     0,    44,     0,    15,     0,    16,     0,    94,
     198,    46,   157,     0,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
       0,    34,    35,    36,     0,     0,     0,    38,    39,    40,
      41,     0,     0,    42,     0,     0,     0,    43,     0,     0,
       0,     0,     0,    44,    15,     0,    16,     0,    94,   207,
       0,    46,     0,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,     0,
      34,    35,     0,     0,     0,     0,    38,    39,    40,    41,
       0,     0,    42,     0,     0,     0,    43,     0,     0,     0,
       0,   208,    44,     0,     0,   209,     0,    15,   238,    16,
      46,    94,     0,     0,   157,     0,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,     0,    34,    35,    36,     0,     0,     0,    38,
      39,    40,    41,     0,     0,    42,     0,     0,     0,    43,
       0,     0,     0,     0,     0,    44,    15,     0,    16,     0,
      94,   256,     0,    46,     0,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,     0,    34,    35,     0,     0,     0,     0,    38,    39,
      40,    41,     0,     0,    42,     0,     0,     0,    43,     0,
       0,     0,     0,   208,    44,     0,    15,   209,    16,     0,
      94,   339,    46,     0,     0,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,     0,    34,    35,     0,     0,     0,     0,    38,    39,
      40,    41,     0,     0,    42,     0,     0,     0,    43,     0,
       0,     0,     0,   208,    44,     0,    15,   209,    16,     0,
      94,   385,    46,     0,     0,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,     0,    34,    35,     0,     0,     0,     0,    38,    39,
      40,    41,     0,     0,    42,     0,     0,     0,    43,     0,
       0,     0,     0,   208,    44,     0,    15,   209,    16,     0,
      94,   421,    46,     0,     0,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,     0,    34,    35,     0,     0,     0,     0,    38,    39,
      40,    41,     0,     0,    42,     0,     0,     0,    43,     0,
       0,     0,     0,   208,    44,     0,     0,   209,     0,    15,
     515,    16,    46,    94,     0,     0,   157,     0,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,     0,    34,    35,    36,     0,     0,
       0,    38,    39,    40,    41,     0,     0,    42,     0,     0,
       0,    43,     0,     0,     0,     0,     0,    44,     0,    15,
       0,    16,     0,    94,     0,    46,   157,     0,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,     0,    34,    35,    36,     0,     0,
       0,    38,    39,    40,    41,     0,     0,    42,     0,     0,
       0,    43,     0,     0,     0,     0,     0,    44,     0,    15,
       0,    16,     0,    94,     0,    46,   347,     0,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,     0,    34,    35,    36,     0,     0,
       0,    38,    39,    40,    41,     0,     0,    42,     0,     0,
       0,    43,     0,     0,     0,     0,     0,    44,    15,     0,
      16,     0,    94,     0,     0,    46,     0,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,     0,    34,    35,   372,     0,     0,     0,
      38,    39,    40,    41,     0,     0,    42,     0,     0,     0,
      43,     0,   373,     0,     0,     0,    44,    15,     0,    16,
       0,    94,     0,     0,    46,     0,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,     0,    34,    35,     0,     0,     0,     0,    38,
      39,    40,    41,     0,     0,    42,     0,     0,     0,    43,
       0,     0,     0,     0,   208,    44,     0,    15,   209,    16,
       0,    94,     0,    46,     0,     0,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,     0,    34,    35,     0,     0,     0,     0,    38,
      39,    40,    41,     0,     0,    42,     0,     0,     0,    43,
       0,     0,     0,     0,    95,    44,    15,     0,    16,     0,
      94,     0,     0,    46,     0,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,     0,    34,    35,     0,     0,     0,     0,    38,    39,
      40,    41,     0,   190,    42,     0,     0,     0,    43,     0,
       0,     0,     0,     0,    44,    15,     0,    16,     0,    94,
       0,     0,    46,     0,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
       0,    34,    35,     0,     0,     0,     0,    38,    39,    40,
      41,     0,     0,    42,     0,     0,     0,    43,     0,     0,
       0,     0,   283,    44,    15,     0,    16,     0,    94,   294,
       0,    46,     0,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,     0,
      34,    35,     0,     0,     0,     0,    38,    39,    40,    41,
       0,     0,    42,     0,     0,     0,    43,     0,     0,     0,
       0,     0,    44,    15,     0,    16,     0,    94,     0,     0,
      46,     0,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,     0,    34,
      35,     0,     0,     0,     0,    38,    39,    40,    41,     0,
       0,    42,     0,     0,     0,    43,     0,     0,     0,     0,
     298,    44,    15,     0,    16,     0,    94,   383,     0,    46,
       0,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,     0,    34,    35,
       0,     0,     0,     0,    38,    39,    40,    41,     0,     0,
      42,     0,     0,     0,    43,     0,     0,     0,     0,     0,
      44,    15,     0,    16,   414,    94,     0,     0,    46,     0,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,     0,    34,    35,     0,
       0,     0,     0,    38,    39,    40,    41,     0,     0,    42,
       0,     0,     0,    43,     0,     0,     0,     0,     0,    44,
      15,     0,    16,     0,    94,   449,     0,    46,     0,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,     0,    34,    35,     0,     0,
       0,     0,    38,    39,    40,    41,     0,     0,    42,     0,
       0,     0,    43,     0,     0,     0,     0,     0,    44,    15,
       0,    16,   469,    94,     0,     0,    46,     0,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,     0,    34,    35,     0,     0,     0,
       0,    38,    39,    40,    41,     0,     0,    42,     0,     0,
       0,    43,     0,     0,     0,     0,     0,    44,    15,     0,
      16,     0,    94,   487,     0,    46,     0,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,     0,    34,    35,     0,     0,     0,     0,
      38,    39,    40,    41,     0,     0,    42,     0,     0,     0,
      43,     0,     0,     0,     0,     0,    44,    15,     0,    16,
       0,    94,     0,     0,    46,     0,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,     0,    34,    35,     0,     0,     0,     0,    38,
      39,    40,    41,     0,     0,    42,     0,     0,     0,    43,
       0,     0,     0,     0,     0,    44,    15,     0,    16,     0,
      17,     0,     0,    46,     0,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,     0,    34,    35,     0,     0,     0,     0,    38,    39,
      40,    41,     0,     0,    42,     0,     0,     0,    43,     0,
       0,   233,     0,    16,    44,   234,     0,     0,     0,     0,
     235,     0,    46,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,     0,     0,     0,     0,
       0,     0,     0,    38,    39,    40,    41,     0,   233,    42,
      16,     0,   405,     0,     0,     0,   236,   235,     0,    44,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,     0,     0,     0,     0,     0,     0,     0,
      38,    39,    40,    41,     0,   233,    42,    16,     0,   329,
       0,     0,     0,   406,   235,     0,    44,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
       0,     0,     0,     0,     0,     0,     0,    38,    39,    40,
      41,     0,   233,    42,    16,     0,   459,     0,     0,     0,
       0,   235,     0,    44,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,     0,     0,     0,
       0,     0,     0,     0,    38,    39,    40,    41,     0,     0,
      42,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      44
  };

  const short
  parser::yycheck_[] =
  {
       2,   115,    36,    61,   146,    44,    44,   207,   218,   150,
     118,    34,   174,    17,    44,    17,     0,     4,     8,    10,
       6,     8,     4,   164,    12,     2,     8,    30,   136,    34,
       4,    12,   119,    15,    30,   149,   123,   145,     0,    97,
      17,    44,    15,    66,    46,   245,    30,     4,   162,     6,
      13,     8,    15,    56,    48,    42,     8,    48,    60,    49,
      42,    66,    49,    45,    46,   104,   104,    49,    42,    57,
       8,     8,    56,    46,   104,    49,    57,    15,    16,    17,
      18,    83,    84,    46,    57,    42,   200,     4,    16,    17,
      94,    15,    94,    15,   236,   203,     8,    49,   206,     8,
     158,   139,   243,   265,   212,     8,     6,   269,    46,     8,
      39,     6,    49,   115,    52,     4,   278,    15,    40,    14,
      15,   126,    46,   128,    46,    42,    50,     2,    52,    53,
      54,    55,    56,    57,    58,    57,     6,    49,     8,   339,
      49,   199,    17,   343,     7,    15,    49,   149,    46,   290,
      49,    46,    15,   353,   156,   157,   366,    10,     7,    10,
     162,   166,    15,   165,    92,    93,    94,    95,   170,    12,
     193,     7,   100,   101,    39,   103,    46,    16,    17,    18,
      12,   239,   184,    46,   218,   327,     4,    40,   193,    12,
       8,   119,    41,    46,    27,   123,   254,    40,   200,   201,
     341,    12,   207,   205,     9,    41,     7,    12,    40,   247,
      14,    15,   217,    52,    14,    15,     0,    40,   220,   277,
     222,    14,    15,    33,    42,    10,   228,    12,     0,     1,
      41,     3,   234,   256,    30,    10,    41,    12,    10,   297,
     245,   169,    46,    46,     4,    48,    46,    57,   448,   251,
     252,   256,    10,    46,     9,    53,    54,    48,   186,   187,
      58,    12,   190,    10,   406,     0,   194,   375,   196,   197,
     378,    16,    17,    18,    19,    20,    21,    22,    23,    24,
     208,   209,    32,    15,    34,    35,   291,    10,    11,    83,
      84,   219,    12,   221,    39,   274,   275,    49,    12,   227,
      59,    12,    12,     7,     9,    12,   234,    52,     9,     9,
       6,    10,    34,   241,    46,    49,    59,   431,    50,     0,
      52,    53,    54,    55,    56,    57,    58,   329,     4,     9,
     332,     9,   366,     5,   339,    10,    48,     0,   343,     9,
     342,     9,     9,    41,    66,   347,     7,   352,   353,     7,
       9,   279,   280,     9,   377,   283,     5,     9,    11,     9,
     288,   289,   385,     7,    49,    18,    19,    49,     5,    12,
     298,    12,   377,     9,     9,    41,     9,     5,    10,     9,
     385,     7,     7,    36,    37,    49,    57,    49,     9,    49,
     504,     9,    45,     5,    45,     9,   398,    10,     9,     9,
      57,   329,    49,   405,   126,     7,   128,   335,     9,   337,
      49,   413,    10,     9,   416,    49,   344,   475,   476,     5,
      49,     9,     5,     7,   432,   369,   428,   265,    48,   431,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
     269,   278,   447,   448,   166,   251,   280,   439,   358,   308,
      -1,   154,   380,   201,    -1,    -1,   109,   459,   516,   387,
     462,    -1,   464,    -1,    -1,    -1,   468,   120,    -1,   397,
      -1,   193,    -1,    -1,    -1,    -1,   129,   405,    -1,    -1,
      -1,    -1,   135,    -1,    -1,   207,    -1,    -1,    -1,    -1,
     143,   419,    -1,    -1,    -1,   217,   498,   499,    -1,   501,
      -1,   154,   504,    -1,    -1,    -1,    -1,    -1,    -1,   437,
     438,   439,   514,   441,   167,   168,    -1,   445,    -1,    -1,
      -1,   174,    -1,   245,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   459,    -1,    -1,   256,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    -1,    -1,    -1,
     478,   479,    -1,    -1,    -1,    -1,    -1,   485,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   218,    -1,    -1,    -1,   291,
      -1,   224,   225,    -1,    -1,    -1,    -1,    -1,   231,   232,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   247,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   259,    -1,   261,    -1,
      -1,    -1,   265,    -1,    -1,    -1,   269,   339,    -1,    -1,
      -1,   343,    -1,    -1,    -1,   278,   279,   280,    -1,    -1,
     352,   353,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   377,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   385,   317,    -1,    -1,    -1,    -1,    -1,
     323,     1,    -1,    -1,     4,     5,     6,    -1,     8,    -1,
      -1,    11,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,   366,    -1,    -1,    46,    -1,    -1,   372,
      -1,    -1,    52,    -1,   377,   447,   448,    57,    -1,    -1,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,     1,
      -1,    -1,     4,    -1,     6,    -1,     8,   400,    -1,    -1,
     403,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      32,    33,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      42,    -1,    -1,    -1,    46,    -1,   439,    -1,    -1,    -1,
      52,    -1,     4,    -1,     6,    57,     8,    -1,    60,    -1,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      32,    33,    -1,    -1,    36,    37,    38,    39,    -1,    41,
      42,    -1,    -1,    -1,    46,    -1,    48,    -1,    50,    51,
      52,    53,    54,    55,    56,    57,    58,     4,    60,     6,
      -1,     8,    -1,    -1,    -1,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    41,    42,    -1,    -1,    -1,    46,
      -1,    48,    -1,    50,    51,    52,    53,    54,    55,    56,
      57,    58,     4,    60,     6,     7,     8,    -1,    -1,    -1,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    41,
      42,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    51,
      52,    -1,     4,    55,     6,     7,     8,    -1,    60,    -1,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    41,
      42,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    51,
      52,    -1,    -1,    55,    -1,     4,     5,     6,    60,     8,
      -1,    -1,    11,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,    -1,
      -1,    -1,    -1,    52,    -1,     4,    -1,     6,    -1,     8,
       9,    60,    11,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    32,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,    -1,
      -1,    -1,    -1,    52,     4,    -1,     6,    -1,     8,     9,
      -1,    60,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,    -1,    -1,    -1,    46,    -1,    -1,    -1,
      -1,    51,    52,    -1,    -1,    55,    -1,     4,     5,     6,
      60,     8,    -1,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    32,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,
      -1,    -1,    -1,    -1,    -1,    52,     4,    -1,     6,    -1,
       8,     9,    -1,    60,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,
      -1,    -1,    -1,    51,    52,    -1,     4,    55,     6,    -1,
       8,     9,    60,    -1,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,
      -1,    -1,    -1,    51,    52,    -1,     4,    55,     6,    -1,
       8,     9,    60,    -1,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,
      -1,    -1,    -1,    51,    52,    -1,     4,    55,     6,    -1,
       8,     9,    60,    -1,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,
      -1,    -1,    -1,    51,    52,    -1,    -1,    55,    -1,     4,
       5,     6,    60,     8,    -1,    -1,    11,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    32,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    46,    -1,    -1,    -1,    -1,    -1,    52,    -1,     4,
      -1,     6,    -1,     8,    -1,    60,    11,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    32,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    46,    -1,    -1,    -1,    -1,    -1,    52,    -1,     4,
      -1,     6,    -1,     8,    -1,    60,    11,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    32,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    46,    -1,    -1,    -1,    -1,    -1,    52,     4,    -1,
       6,    -1,     8,    -1,    -1,    60,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    32,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      46,    -1,    48,    -1,    -1,    -1,    52,     4,    -1,     6,
      -1,     8,    -1,    -1,    60,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,
      -1,    -1,    -1,    -1,    51,    52,    -1,     4,    55,     6,
      -1,     8,    -1,    60,    -1,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,
      -1,    -1,    -1,    -1,    51,    52,     4,    -1,     6,    -1,
       8,    -1,    -1,    60,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    41,    42,    -1,    -1,    -1,    46,    -1,
      -1,    -1,    -1,    -1,    52,     4,    -1,     6,    -1,     8,
      -1,    -1,    60,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,    -1,
      -1,    -1,    51,    52,     4,    -1,     6,    -1,     8,     9,
      -1,    60,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,    -1,    -1,    -1,    46,    -1,    -1,    -1,
      -1,    -1,    52,     4,    -1,     6,    -1,     8,    -1,    -1,
      60,    -1,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,    30,
      31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
      -1,    42,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,
      51,    52,     4,    -1,     6,    -1,     8,     9,    -1,    60,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      42,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,
      52,     4,    -1,     6,     7,     8,    -1,    -1,    60,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    -1,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
      -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,    52,
       4,    -1,     6,    -1,     8,     9,    -1,    60,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,    52,     4,
      -1,     6,     7,     8,    -1,    -1,    60,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    46,    -1,    -1,    -1,    -1,    -1,    52,     4,    -1,
       6,    -1,     8,     9,    -1,    60,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      46,    -1,    -1,    -1,    -1,    -1,    52,     4,    -1,     6,
      -1,     8,    -1,    -1,    60,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,
      -1,    -1,    -1,    -1,    -1,    52,     4,    -1,     6,    -1,
       8,    -1,    -1,    60,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,    -1,
      -1,     4,    -1,     6,    52,     8,    -1,    -1,    -1,    -1,
      13,    -1,    60,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,     4,    42,
       6,    -1,     8,    -1,    -1,    -1,    49,    13,    -1,    52,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,     4,    42,     6,    -1,     8,
      -1,    -1,    -1,    49,    13,    -1,    52,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,     4,    42,     6,    -1,     8,    -1,    -1,    -1,
      -1,    13,    -1,    52,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      52
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,    30,    44,    56,    63,    64,    66,    67,    68,    69,
      72,     6,    71,     0,     1,     4,     6,     8,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    30,    31,    32,    33,    36,    37,
      38,    39,    42,    46,    52,    57,    60,    65,    83,    84,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   104,
     109,   118,   119,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
      30,     0,     1,     3,    10,   141,     0,    68,     7,   126,
      48,    70,    10,    48,     8,    51,   101,   102,   103,   120,
      41,    48,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    90,    91,   101,   103,   109,   118,   121,   122,     6,
      14,   126,   126,     6,     8,    84,    86,   104,   114,    13,
     116,   117,   126,   128,    10,    40,   107,   126,    39,    52,
     134,   135,   137,    40,    57,   108,   126,   101,   109,   110,
       6,    51,    52,    57,    59,    91,    92,    11,    87,    88,
     101,   118,    10,   141,     8,    49,    86,    12,    40,     8,
      49,    27,    39,    39,     4,    65,    65,     0,     7,    30,
       4,   103,   103,   103,    10,   141,    48,    12,   142,   103,
      41,   103,   103,     9,    48,     9,    12,    41,     9,    87,
      10,    12,   142,   120,   126,    49,   120,     9,    51,    55,
     103,   111,   112,   113,    84,   104,    85,    86,    12,     8,
      49,     8,    49,   126,    12,    40,   142,     8,    49,   126,
      12,    40,   142,     4,     8,    13,    49,   125,     5,    87,
       7,    41,   113,     6,     8,   114,   126,    59,    89,   101,
     101,    10,    11,   141,    87,     0,     9,   113,   101,    14,
     126,    14,   126,   103,   101,    55,    78,    79,    80,    81,
      82,    90,   126,    32,    34,    35,    75,    76,    77,    12,
      12,   101,   103,    51,   103,     7,     9,   103,    12,    41,
       8,    86,   103,   103,     9,   103,   141,    87,    51,   101,
     121,     9,   142,   101,   142,    85,   103,   103,    12,   142,
       9,   116,   103,   101,   103,   101,    10,    12,   126,   126,
      10,   103,   101,    12,    57,   126,    57,   126,   103,     8,
     125,   141,    49,   103,     7,    41,     7,    41,   113,     9,
      85,     8,    49,   115,     8,   126,   134,    11,    88,   101,
       0,   141,    86,     9,   126,   126,     9,    79,     4,    80,
       5,    53,    54,    58,    73,    74,   140,    74,    74,    10,
     141,    82,    32,    48,   103,   105,   106,   126,   105,   103,
      48,     9,   103,     9,   103,     9,   113,    41,     9,     9,
     141,     7,     7,   111,     9,     9,   126,     8,    49,    10,
      12,     9,   126,    12,    57,     8,    49,   125,     9,   103,
       5,   101,     7,    49,     7,   103,    49,   103,     7,    41,
      85,     9,   113,   101,    85,   103,   101,     0,    49,    85,
       1,   110,    12,   116,    75,     5,   126,    46,    48,    12,
     142,    45,   126,   142,   103,    41,     9,    86,     9,     9,
     103,     9,    10,   103,   101,   126,    57,   126,   103,     8,
     125,     9,    49,   101,    49,     7,   101,     7,    49,     7,
     103,     9,     9,   101,     5,    29,    87,    73,     8,    49,
     103,   103,   106,     5,   103,    45,     5,     9,   103,    85,
       9,     9,    10,    57,     9,   103,   101,   101,    49,    49,
     101,    49,     7,    10,   141,   141,   103,   103,   103,     9,
       9,   101,   101,   101,    49,     5,    87,     5,     9,   101,
     141,     5
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    62,    63,    63,    63,    63,    63,    64,    64,    64,
      65,    65,    65,    65,    66,    66,    66,    66,    66,    67,
      67,    68,    68,    69,    70,    70,    71,    71,    71,    72,
      73,    74,    74,    75,    75,    75,    76,    76,    77,    77,
      78,    78,    79,    79,    79,    79,    79,    80,    80,    81,
      81,    82,    82,    83,    83,    84,    84,    85,    85,    86,
      86,    86,    86,    87,    87,    87,    87,    87,    88,    88,
      89,    89,    89,    90,    90,    90,    90,    90,    90,    90,
      90,    91,    91,    92,    92,    93,    93,    93,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    93,    94,    94,
      94,    95,    95,    95,    96,    96,    96,    97,    97,    97,
      98,    98,    98,    98,    98,    99,    99,    99,    99,    99,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   102,   102,   103,   104,   104,   105,   105,   106,   106,
     106,   106,   106,   106,   106,   107,   107,   107,   107,   107,
     107,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   110,   110,   111,   111,   111,   112,   112,
     113,   114,   115,   115,   115,   116,   116,   116,   116,   116,
     117,   117,   118,   119,   119,   119,   119,   119,   119,   119,
     119,   120,   120,   120,   120,   120,   120,   120,   121,   121,
     122,   122,   122,   123,   124,   124,   125,   125,   125,   126,
     126,   127,   127,   127,   127,   128,   128,   128,   129,   130,
     131,   131,   132,   132,   133,   133,   134,   134,   134,   134,
     135,   135,   135,   135,   136,   136,   137,   137,   138,   138,
     139,   139,   139,   139,   139,   139,   140,   140,   140,   140,
     141,   141,   142,   142
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     3,     2,     1,     2,     0,     5,     6,     4,
       1,     5,     3,     6,     2,     2,     3,     3,     2,     1,
       2,     1,     1,     7,     0,     2,     0,     3,     2,     5,
       2,     1,     3,     2,     2,     2,     1,     3,     0,     2,
       1,     1,     6,     4,     8,     6,     3,     1,     2,     1,
       2,     0,     1,     1,     1,     5,     3,     0,     1,     1,
       1,     2,     2,     1,     3,     3,     4,     2,     1,     1,
       2,     2,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     7,     6,     4,     5,     4,
       2,     5,     4,     5,     5,     2,     3,     4,     7,     6,
       6,     9,     8,     8,     6,     5,     5,     8,     7,     7,
       4,     5,     7,     6,     4,     6,     8,     7,     5,     5,
       1,     1,     1,     1,     1,     3,     2,     4,     3,     1,
       1,     1,     1,     1,     1,     1,     5,     4,     3,     5,
       1,     2,     3,     4,     6,     3,     4,     6,     5,     7,
       6,     1,     3,     2,     7,     7,     1,     3,     3,     4,
       4,     5,     1,     3,     3,     1,     3,     4,     3,     5,
       6,     2,     1,     3,     4,     5,     4,     3,     5,     6,
       7,     2,     4,     5,     7,     4,     6,     2,     4,     5,
       7,     4,     6,     0,     1,     2,     2,     1,     1,     3,
       2,     3,     0,     2,     3,     1,     3,     4,     3,     4,
       1,     3,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     1,     3,     2,     3,     5,     4,     3,     2,
       0,     1,     3,     4,     4,     5,     1,     1,     1,     1,
       1,     2,     3,     3,     4,     1,     3,     4,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     2,
       1,     1,     2,     2,     1,     2,     0,     1,     1,     1,
       0,     1,     0,     1
  };




#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   174,   174,   175,   176,   177,   178,   186,   189,   192,
     201,   203,   206,   209,   214,   217,   223,   226,   229,   257,
     259,   264,   265,   269,   274,   275,   279,   280,   281,   285,
     294,   299,   301,   306,   308,   310,   315,   317,   322,   323,
     331,   332,   336,   339,   342,   345,   348,   357,   358,   370,
     371,   375,   376,   384,   384,   387,   389,   394,   395,   399,
     400,   401,   402,   406,   407,   408,   413,   418,   426,   427,
     436,   437,   438,   442,   443,   444,   445,   446,   447,   448,
     449,   453,   454,   458,   460,   465,   468,   471,   475,   478,
     481,   484,   487,   490,   500,   503,   508,   514,   529,   536,
     543,   552,   559,   566,   576,   582,   588,   597,   603,   609,
     618,   623,   628,   633,   638,   646,   651,   657,   662,   667,
     680,   683,   686,   689,   692,   695,   702,   705,   708,   711,
     714,   717,   723,   724,   725,   726,   728,   731,   734,   737,
     740,   741,   743,   745,   748,   751,   754,   760,   763,   766,
     769,   777,   778,   782,   790,   793,   798,   800,   805,   807,
     809,   811,   813,   815,   817,   830,   832,   837,   840,   843,
     848,   854,   856,   858,   860,   862,   864,   866,   868,   870,
     872,   877,   878,   879,   881,   883,   888,   897,   899,   901,
     903,   905,   910,   922,   923,   928,   929,   930,   934,   935,
     938,   940,   943,   944,   946,   955,   957,   959,   961,   963,
     968,   970,   974,   982,   983,   984,   985,   986,   987,   988,
     989,   993,   995,   997,   999,  1001,  1003,  1005,  1013,  1015,
    1020,  1022,  1024,  1029,  1034,  1036,  1045,  1046,  1047,  1055,
    1056,  1061,  1063,  1065,  1067,  1071,  1073,  1075,  1086,  1088,
    1091,  1092,  1096,  1097,  1101,  1102,  1106,  1107,  1108,  1109,
    1118,  1119,  1120,  1121,  1125,  1127,  1132,  1134,  1139,  1141,
    1146,  1147,  1148,  1149,  1150,  1151,  1155,  1156,  1157,  1158,
    1170,  1170,  1171,  1171
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
#line 5705 "langutils/sc_parser/src/sc_grammar_parser.cpp"

#line 1173 "langutils/sc_parser/src/sc_grammar.y"

