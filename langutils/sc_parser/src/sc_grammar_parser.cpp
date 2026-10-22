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
			yylhs.value.as < RegionListIndex > () = cxt.create(RegionList{}, yylhs.location, cxt.create_error(yylhs.location, yystack_[0].location));
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
				// Only the token is re-emitted, so anything popped is lost from the region.
				yylhs.value.as < RegionListIndex > () = !cxt.discarded(yystack_[0].location, Index { *yystack_[1].value.as < RegionListIndex > () }).empty() ? cxt.graph->append(yystack_[1].value.as < RegionListIndex > (), cxt.create_error(yystack_[0].location, yystack_[0].location, Index { *yystack_[1].value.as < RegionListIndex > () })) : yystack_[1].value.as < RegionListIndex > ();
			} else {
				error_recovery::expr(cxt);
				yylhs.value.as < RegionListIndex > () = cxt.graph->append(yystack_[1].value.as < RegionListIndex > (), cxt.create_error(yystack_[0].location, yystack_[0].location, Index { *yystack_[1].value.as < RegionListIndex > () }));
				yyclearin;
			} 
		}
#line 2854 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 20: // classOrExtList.list: classOrExtList.item
#line 265 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassOrExtensionListIndex > () = cxt.create(ClassOrExtensionList{}, yylhs.location, yystack_[0].value.as < ClassOrExtensionIndex > ()); }
#line 2860 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 21: // classOrExtList.list: classOrExtList.list classOrExtList.item
#line 267 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassOrExtensionListIndex > () = cxt.graph->append(yystack_[1].value.as < ClassOrExtensionListIndex > (), yystack_[0].value.as < ClassOrExtensionIndex > ()); }
#line 2866 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 22: // classOrExtList.item: class
#line 271 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassOrExtensionIndex > () = yystack_[0].value.as < ClassIndex > (); }
#line 2872 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 23: // classOrExtList.item: class.extension
#line 272 "langutils/sc_parser/src/sc_grammar.y"
                          { yylhs.value.as < ClassOrExtensionIndex > () = yystack_[0].value.as < ClassExtensionIndex > (); }
#line 2878 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 24: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY members.opt method.list.opt CLOSECURLY
#line 277 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassIndex > () = cxt.create(Class{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[6].location), yystack_[5].value.as < maybe<NamedIdentifierIndex> > (), yystack_[4].value.as < maybe<ClassNameIdentifierIndex> > (), yystack_[2].value.as < BigMemberListIndex > (), yystack_[1].value.as < MethodListIndex > ()); }
#line 2884 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 25: // class.super.opt: %empty
#line 281 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < maybe<ClassNameIdentifierIndex> > () = cxt.create(Missing{}, yylhs.location); }
#line 2890 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 26: // class.super.opt: COLON CLASSNAME
#line 282 "langutils/sc_parser/src/sc_grammar.y"
                          { yylhs.value.as < maybe<ClassNameIdentifierIndex> > () = cxt.create(ClassNameIdentifier{}, yystack_[0].location); }
#line 2896 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 27: // class.slot.opt: %empty
#line 286 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < maybe<NamedIdentifierIndex> > () = cxt.create(Missing{}, yylhs.location); }
#line 2902 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 28: // class.slot.opt: OPENSQUARE name CLOSESQUARE
#line 287 "langutils/sc_parser/src/sc_grammar.y"
                                      { yylhs.value.as < maybe<NamedIdentifierIndex> > () = yystack_[1].value.as < NamedIdentifierIndex > (); }
#line 2908 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 29: // class.slot.opt: OPENSQUARE CLOSESQUARE
#line 288 "langutils/sc_parser/src/sc_grammar.y"
                                 { yylhs.value.as < maybe<NamedIdentifierIndex> > () = cxt.create(Missing{}, yylhs.location); }
#line 2914 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 30: // class.extension: ADD CLASSNAME OPENCURLY method.list.opt CLOSECURLY
#line 293 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ClassExtensionIndex > () = cxt.create(ClassExtension{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[3].location), yystack_[1].value.as < MethodListIndex > ()); }
#line 2920 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 31: // members.entry.item: accessor variable_declarations.list.item
#line 302 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareMemberIndex > () = cxt.create(DeclareMember{}, yylhs.location, yystack_[1].value.as < RWAccessorIndex > (), yystack_[0].value.as < DeclareAnyVariableIndex > ()); }
#line 2926 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 32: // members.entry.list: members.entry.item
#line 307 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareMemberListIndex > () = cxt.create(DeclareMemberList{}, yylhs.location, yystack_[0].value.as < DeclareMemberIndex > ()); }
#line 2932 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 33: // members.entry.list: members.entry.list COMMA members.entry.item
#line 309 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareMemberListIndex > () = cxt.graph->append(yystack_[2].value.as < DeclareMemberListIndex > (), yystack_[0].value.as < DeclareMemberIndex > ()); }
#line 2938 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 34: // members.entry: CLASSVAR members.entry.list
#line 314 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyMemberListIndex > () = cxt.graph->cast<DeclareClassMemberList>(yystack_[0].value.as < DeclareMemberListIndex > ()); }
#line 2944 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 35: // members.entry: VAR members.entry.list
#line 316 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyMemberListIndex > () = cxt.graph->cast<DeclareMemberList>(yystack_[0].value.as < DeclareMemberListIndex > ()); }
#line 2950 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 36: // members.entry: CONST members.entry.list
#line 318 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyMemberListIndex > () = cxt.graph->cast<DeclareConstList>(yystack_[0].value.as < DeclareMemberListIndex > ()); }
#line 2956 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 37: // members: members.entry
#line 323 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BigMemberListIndex > () = cxt.create(BigMemberList{}, yylhs.location, yystack_[0].value.as < DeclareAnyMemberListIndex > ());}
#line 2962 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 38: // members: members SEMICOLON members.entry
#line 325 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BigMemberListIndex > () = cxt.graph->append(yystack_[2].value.as < BigMemberListIndex > (), yystack_[0].value.as < DeclareAnyMemberListIndex > ()); }
#line 2968 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 39: // members.opt: %empty
#line 329 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < BigMemberListIndex > () = cxt.create(BigMemberList{}, yylhs.location); }
#line 2974 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 40: // members.opt: members semicolon.opt
#line 330 "langutils/sc_parser/src/sc_grammar.y"
                                { yylhs.value.as < BigMemberListIndex > () = yystack_[1].value.as < BigMemberListIndex > (); }
#line 2980 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 41: // method.name: name
#line 338 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < SelectorIndex > () = yystack_[0].value.as < NamedIdentifierIndex > (); }
#line 2986 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 42: // method.name: binary_op.raw
#line 339 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < SelectorIndex > () = yystack_[0].value.as < SelectorIndex > (); }
#line 2992 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 43: // method.base: method.name OPENCURLY argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 344 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyMethodIndex > () = cxt.create(Method{}, yylhs.location, yystack_[5].value.as < SelectorIndex > (), yystack_[3].value.as < DeclareArgumentListIndex > (), cxt.create(Missing{},yystack_[4].location), yystack_[2].value.as < BlockContentsListIndex > ()); }
#line 2998 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 44: // method.base: method.name OPENCURLY argument_declarations.opt CLOSECURLY
#line 347 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyMethodIndex > () = cxt.create(Method{}, yylhs.location, yystack_[3].value.as < SelectorIndex > (), yystack_[1].value.as < DeclareArgumentListIndex > (), cxt.create(Missing{},yystack_[2].location), cxt.create(BlockContentsList{}, yystack_[0].location)); }
#line 3004 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 45: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt block.contents semicolon.opt CLOSECURLY
#line 350 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyMethodIndex > () = cxt.create(Method{}, yylhs.location, yystack_[7].value.as < SelectorIndex > (), yystack_[5].value.as < DeclareArgumentListIndex > (), cxt.create(PrimitiveIdentifier{}, yystack_[4].location), yystack_[2].value.as < BlockContentsListIndex > ()); }
#line 3010 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 46: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt CLOSECURLY
#line 353 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyMethodIndex > () = cxt.create(Method{}, yylhs.location, yystack_[5].value.as < SelectorIndex > (), yystack_[3].value.as < DeclareArgumentListIndex > (), cxt.create(PrimitiveIdentifier{}, yystack_[2].location), cxt.create(BlockContentsList{}, yystack_[1].location)); }
#line 3016 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 47: // method.base: method.name OPENCURLY error
#line 356 "langutils/sc_parser/src/sc_grammar.y"
                {
			error_recovery::method(cxt, yystack_[1].location); // advance over the closing curly of function.
			// The name is left on the stack, so isn't discarded.
			yylhs.value.as < AnyMethodIndex > () = cxt.graph->prepend(cxt.create_error(yylhs.location, yystack_[0].location), yystack_[2].value.as < SelectorIndex > ());
			yyclearin;
		}
#line 3027 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 48: // method: method.base
#line 365 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < AnyMethodIndex > () = yystack_[0].value.as < AnyMethodIndex > (); }
#line 3033 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 49: // method: MULTIPLY method.base
#line 367 "langutils/sc_parser/src/sc_grammar.y"
                { 
			if (auto c = cxt.graph->as_a<MethodIndex>(yystack_[0].value.as < AnyMethodIndex > ())) {
				yylhs.value.as < AnyMethodIndex > () = cxt.graph->cast<ClassMethod>(*c);
			} 
			// leaves as error
			cxt.graph->location(yystack_[0].value.as < AnyMethodIndex > ()) = yylhs.location; 
			yylhs.value.as < AnyMethodIndex > () = yystack_[0].value.as < AnyMethodIndex > (); 
		}
#line 3046 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 50: // method.list: method
#line 378 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < MethodListIndex > () = cxt.create(MethodList{}, yylhs.location, yystack_[0].value.as < AnyMethodIndex > ()); }
#line 3052 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 51: // method.list: method.list method
#line 379 "langutils/sc_parser/src/sc_grammar.y"
                             { yylhs.value.as < MethodListIndex > () = cxt.graph->append(yystack_[1].value.as < MethodListIndex > (), yylhs.location, yystack_[0].value.as < AnyMethodIndex > ()); }
#line 3058 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 52: // method.list.opt: %empty
#line 383 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < MethodListIndex > () = cxt.create(MethodList{}, yylhs.location); }
#line 3064 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 53: // method.list.opt: method.list
#line 384 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < MethodListIndex > () = yystack_[0].value.as < MethodListIndex > (); }
#line 3070 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 56: // block: block.open argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 396 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BlockIndex > () = cxt.create(BlockNode{}, yylhs.location, yystack_[3].value.as < DeclareArgumentListIndex > (), yystack_[2].value.as < BlockContentsListIndex > ()); }
#line 3076 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 57: // block: block.open argument_declarations.opt CLOSECURLY
#line 398 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BlockIndex > () = cxt.create(BlockNode{}, yylhs.location, yystack_[1].value.as < DeclareArgumentListIndex > (), cxt.create(BlockContentsList{}, yylhs.location)); }
#line 3082 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 58: // block.opt_list: %empty
#line 402 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < BlockListIndex > () = cxt.create(BlockList{}, yylhs.location); }
#line 3088 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 59: // block.opt_list: block.list
#line 403 "langutils/sc_parser/src/sc_grammar.y"
                     { yylhs.value.as < BlockListIndex > () = yystack_[0].value.as < BlockListIndex > (); }
#line 3094 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 60: // block.list: block
#line 407 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BlockListIndex > () = cxt.create(BlockList{}, yylhs.location, yystack_[0].value.as < BlockIndex > ()); }
#line 3100 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 61: // block.list: generator
#line 408 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < BlockListIndex > () = cxt.create(BlockList{}, yylhs.location, yystack_[0].value.as < GeneratorAnyIndex > ()); }
#line 3106 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 62: // block.list: block.list block
#line 409 "langutils/sc_parser/src/sc_grammar.y"
                           { yylhs.value.as < BlockListIndex > () = cxt.graph->append(yystack_[1].value.as < BlockListIndex > (), yylhs.location, yystack_[0].value.as < BlockIndex > ()); }
#line 3112 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 63: // block.list: block.list generator
#line 410 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < BlockListIndex > () = cxt.graph->append(yystack_[1].value.as < BlockListIndex > (), yylhs.location, yystack_[0].value.as < GeneratorAnyIndex > ()); }
#line 3118 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 64: // block.contents: block.contents.item
#line 414 "langutils/sc_parser/src/sc_grammar.y"
                              { yylhs.value.as < BlockContentsListIndex > () = cxt.create(BlockContentsList{}, yylhs.location, yystack_[0].value.as < BlockItemIndex > ()); }
#line 3124 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 65: // block.contents: block.contents SEMICOLON block.contents.item
#line 415 "langutils/sc_parser/src/sc_grammar.y"
                                                       { yylhs.value.as < BlockContentsListIndex > () = cxt.graph->append(yystack_[2].value.as < BlockContentsListIndex > (), yylhs.location, yystack_[0].value.as < BlockItemIndex > ()); }
#line 3130 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 66: // block.contents: block.contents NONLOCALRETURN expr
#line 417 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < BlockContentsListIndex > () = cxt.graph->append(yystack_[2].value.as < BlockContentsListIndex > (), yylhs.location, non_local); 
		}
#line 3139 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 67: // block.contents: block.contents SEMICOLON NONLOCALRETURN expr
#line 422 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < BlockContentsListIndex > () = cxt.graph->append(yystack_[3].value.as < BlockContentsListIndex > (), yylhs.location, non_local); 
		}
#line 3148 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 68: // block.contents: NONLOCALRETURN expr
#line 427 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < BlockContentsListIndex > () = cxt.create(BlockContentsList{}, yylhs.location, non_local);
		}
#line 3157 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 69: // block.contents.item: expr
#line 434 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < BlockItemIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 3163 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 70: // block.contents.item: variable_declarations
#line 435 "langutils/sc_parser/src/sc_grammar.y"
                                { yylhs.value.as < BlockItemIndex > () = yystack_[0].value.as < DeclareVariableListIndex > (); }
#line 3169 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 71: // adverb: DOT name
#line 444 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < AdverbIndex > () = yystack_[0].value.as < NamedIdentifierIndex > (); }
#line 3175 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 72: // adverb: DOT integer
#line 445 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < AdverbIndex > () = yystack_[0].value.as < IntLitIndex > (); }
#line 3181 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 73: // adverb: DOT OPENPAREN expr_or_seq CLOSEPAREN
#line 446 "langutils/sc_parser/src/sc_grammar.y"
                                               { yylhs.value.as < AdverbIndex > () = cxt.create(AdverbExprNode{}, yylhs.location, yystack_[1].value.as < AnyExprIndex > ());  }
#line 3187 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 74: // binary_op.raw: BINOP
#line 450 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3193 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 75: // binary_op.raw: READWRITEVAR
#line 451 "langutils/sc_parser/src/sc_grammar.y"
                       { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3199 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 76: // binary_op.raw: LESSTHAN
#line 452 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3205 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 77: // binary_op.raw: GREATERTHAN
#line 453 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3211 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 78: // binary_op.raw: MINUS
#line 454 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3217 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 79: // binary_op.raw: MULTIPLY
#line 455 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3223 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 80: // binary_op.raw: ADD
#line 456 "langutils/sc_parser/src/sc_grammar.y"
              { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3229 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 81: // binary_op.raw: PIPE
#line 457 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 3235 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 82: // binary_op.no_adverb: binary_op.raw
#line 461 "langutils/sc_parser/src/sc_grammar.y"
                          { yylhs.value.as < SelectorIndex > () = yystack_[0].value.as < SelectorIndex > (); }
#line 3241 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 83: // binary_op.no_adverb: KEYBINOP
#line 462 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < SelectorIndex > () = cxt.create(SelectorNode{ true }, yylhs.location); }
#line 3247 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 84: // binary_op: binary_op.no_adverb adverb
#line 467 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < SelectorMaybeAdverbIndex > () = cxt.create(SelectorWAdverb{}, yylhs.location, yystack_[1].value.as < SelectorIndex > (), yystack_[0].value.as < AdverbIndex > ()); }
#line 3253 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 85: // binary_op: binary_op.no_adverb
#line 469 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < SelectorMaybeAdverbIndex > () = yystack_[0].value.as < SelectorIndex > (); }
#line 3259 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 86: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN arguments CLOSEPAREN block.opt_list
#line 474 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[5].value.as < SelectorIndex > (), yystack_[2].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ()); }
#line 3265 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 87: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN CLOSEPAREN block.list
#line 477 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[4].value.as < SelectorIndex > (), cxt.create(ArgumentList{}, yystack_[2].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3271 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 88: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN block.list
#line 480 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[2].value.as < SelectorIndex > (), cxt.create(ArgumentList{}, yystack_[0].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3277 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 89: // msgsend: name OPENPAREN arguments CLOSEPAREN block.opt_list
#line 484 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[4].value.as < NamedIdentifierIndex > (), yystack_[2].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ()); }
#line 3283 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 90: // msgsend: name OPENPAREN CLOSEPAREN block.list
#line 487 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), cxt.create(ArgumentList{}, yystack_[2].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3289 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 91: // msgsend: name block.list
#line 490 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PrefixMessage{}, yylhs.location, yystack_[1].value.as < NamedIdentifierIndex > (), cxt.create(ArgumentList{}, yystack_[0].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3295 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 92: // msgsend: expr DOT name arguments.maybe_paren block.opt_list
#line 493 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(PostfixMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ()); }
#line 3301 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 93: // msgsend: expr DOT arguments.paren block.opt_list
#line 496 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(ValueMessage{}, yylhs.location, yystack_[3].value.as < AnyExprIndex > (), yystack_[1].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ()); }
#line 3307 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 94: // msgsend: expr DOT OPENPAREN CLOSEPAREN block.opt_list
#line 499 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(ValueMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[2].location), yystack_[0].value.as < BlockListIndex > ()); }
#line 3313 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 95: // msgsend: CLASSNAME OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 509 "langutils/sc_parser/src/sc_grammar.y"
                {  yylhs.value.as < AnyExprIndex > () = cxt.create(CollectionNode{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[4].location), yystack_[2].value.as < ArrayIndex > ()); }
#line 3319 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 96: // msgsend: CLASSNAME block.list
#line 512 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[1].location);
			yylhs.value.as < AnyExprIndex > () = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, yystack_[1].location), yystack_[0].value.as < BlockListIndex > ());
		}
#line 3328 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 97: // msgsend: CLASSNAME arguments.paren block.opt_list
#line 517 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[2].location);
			yylhs.value.as < AnyExprIndex > () = cxt.create(NewMessage{}, yylhs.location, class_name, yystack_[1].value.as < ArgumentListIndex > (), yystack_[0].value.as < BlockListIndex > ());
		}
#line 3337 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 98: // msgsend: CLASSNAME OPENPAREN CLOSEPAREN block.opt_list
#line 523 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[3].location);
			yylhs.value.as < AnyExprIndex > () = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, yystack_[3].location), yystack_[0].value.as < BlockListIndex > ());
		}
#line 3346 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 99: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 538 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, yystack_[3].value.as < ArgumentListIndex > (), yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[6].value.as < AnyExprIndex > (), r);
		}
#line 3356 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 100: // expr.dotrange: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 545 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[3].location),  yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), r);
		}
#line 3366 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 101: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 552 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, yystack_[2].value.as < ArgumentListIndex > (), cxt.create(Missing{}, yystack_[1].location));
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), r);
		}
#line 3376 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 102: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 561 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, yystack_[5].value.as < ArgumentListIndex > (), yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[8].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3386 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 103: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 568 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, yystack_[4].value.as < ArgumentListIndex > (), cxt.create(Missing{}, yystack_[3].location));
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[7].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3396 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 104: // expr.dotrange.assign: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 575 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[5].location),  yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[7].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3406 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 105: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 585 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, yystack_[3].value.as < ArgumentListIndex > (), yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), r);
		}
#line 3416 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 106: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 591 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, yystack_[2].value.as < ArgumentListIndex > (), cxt.create(Missing{}, yystack_[0].location));
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), r);	
		}
#line 3426 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 107: // expr.base.range: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 597 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[3].location),  yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AtMessageIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), r);	
		}
#line 3436 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 108: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 606 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, yystack_[5].value.as < ArgumentListIndex > (), yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[7].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3446 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 109: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 612 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, yystack_[4].value.as < ArgumentListIndex > (), cxt.create(Missing{}, yystack_[3].location));
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[6].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3456 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 110: // expr.base.range.assign: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 618 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[5].location),  yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeAssignIndex > () = cxt.create(RangeAssign{}, yylhs.location, yystack_[6].value.as < AnyExprIndex > (), r, yystack_[0].value.as < AnyExprIndex > ());
		}
#line 3466 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 111: // range: OPENPAREN expr_or_seq DOTDOT CLOSEPAREN
#line 627 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, yystack_[2].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 3475 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 112: // range: OPENPAREN expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 632 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[4].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3484 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 113: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 637 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), yystack_[5].value.as < AnyExprIndex > (), yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[6].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3493 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 114: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 642 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[4].location, yystack_[2].location), yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[5].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 3502 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 115: // range: OPENPAREN DOTDOT expr_or_seq CLOSEPAREN
#line 647 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location);
			yylhs.value.as < RangeIndex > () = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3511 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 116: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 655 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[5].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3520 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 117: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 660 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), yystack_[5].value.as < AnyExprIndex > (), yystack_[3].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[7].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3529 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 118: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 666 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[4].location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[6].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 3538 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 119: // range.generator: OPENPAREN COLON DOTDOT expr_or_seq CLOSEPAREN
#line 671 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location);
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, yystack_[1].value.as < AnyExprIndex > ());
		}
#line 3547 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 120: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT CLOSEPAREN
#line 676 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, yystack_[2].value.as < AnyExprIndex > ());
			yylhs.value.as < RangeGeneratorIndex > () = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 3556 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 121: // expr.base: literal
#line 689 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AnyPossiblyLiteralIndex > (); }
#line 3562 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 122: // expr.base: generator
#line 692 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < GeneratorAnyIndex > (); }
#line 3568 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 123: // expr.base: name
#line 695 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < NamedIdentifierIndex > (); }
#line 3574 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 124: // expr.base: CURRYARG
#line 698 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < AnyExprIndex > () = cxt.create(CurryNode{}, yystack_[0].location); }
#line 3580 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 125: // expr.base: msgsend
#line 701 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 3586 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 126: // expr.base: OPENPAREN expr_or_seq CLOSEPAREN
#line 704 "langutils/sc_parser/src/sc_grammar.y"
                { 
			// A nasty hack because of nil assignment.
			cxt.graph->register_bracketed_expr(yystack_[1].value.as < AnyExprIndex > ());
			yylhs.value.as < AnyExprIndex > () = yystack_[1].value.as < AnyExprIndex > ();
		}
#line 3596 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 127: // expr.base: OPENPAREN error
#line 711 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto close = error_recovery::bracket(cxt, yystack_[1].location);
			yylhs.value.as < AnyExprIndex > () = cxt.create_error(range(yystack_[1].location, close), yystack_[0].location);
			yyclearin; // The lookahead was skipped.
			yyerrok; // Recovered at the matching bracket, so report the next error.
		}
#line 3607 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 128: // expr.base: TILDE name
#line 719 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(EnvIdentifierNode{}, yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3613 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 129: // expr.base: expr.base OPENSQUARE arguments CLOSESQUARE
#line 722 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[3].value.as < AnyExprIndex > (), yystack_[1].value.as < ArgumentListIndex > () ); }
#line 3619 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 130: // expr.base: expr.base OPENSQUARE CLOSESQUARE
#line 725 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[2].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[1].location)); }
#line 3625 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 131: // expr.base: expr.base.range
#line 728 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AtMessageIndex > (); }
#line 3631 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 132: // expr.base: range
#line 731 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < RangeIndex > (); }
#line 3637 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 133: // expr.base: range.generator
#line 734 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < RangeGeneratorIndex > (); }
#line 3643 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 134: // expr: expr.base
#line 739 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 3649 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 135: // expr: expr.base.range.assign
#line 740 "langutils/sc_parser/src/sc_grammar.y"
                                 { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < RangeAssignIndex > (); }
#line 3655 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 136: // expr: expr.dotrange
#line 741 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AtMessageIndex > (); }
#line 3661 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 137: // expr: expr.dotrange.assign
#line 742 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < RangeAssignIndex > (); }
#line 3667 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 138: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE
#line 745 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[1].value.as < ArgumentListIndex > () ); }
#line 3673 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 139: // expr: expr DOT OPENSQUARE CLOSESQUARE
#line 748 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AtMessage{}, yylhs.location, yystack_[3].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[1].location) ); }
#line 3679 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 140: // expr: expr binary_op expr
#line 751 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(BinaryMessage{}, yylhs.location, yystack_[1].value.as < SelectorMaybeAdverbIndex > (), yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3685 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 141: // expr: expr DOT name EQUALSSIGN expr
#line 754 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(SetterNode{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3691 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 142: // expr: CLASSNAME
#line 756 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < AnyExprIndex > () = cxt.create(ClassNameIdentifier{}, yylhs.location); }
#line 3697 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 143: // expr: BACKTICK expr
#line 757 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < AnyExprIndex > () = cxt.create(ReferenceNode{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 3703 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 144: // expr: name EQUALSSIGN expr
#line 759 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentNode{}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3709 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 145: // expr: TILDE name EQUALSSIGN expr
#line 762 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentNode{AssignmentNode::Target::Environment}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3715 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 146: // expr: name OPENPAREN arguments CLOSEPAREN EQUALSSIGN expr
#line 765 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(SetterNode{}, yylhs.location, yystack_[3].value.as < ArgumentListIndex > (), yystack_[5].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3721 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 147: // expr: name.pack EQUALSSIGN expr
#line 768 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(VariablePackAssign{}, yylhs.location, yystack_[2].value.as < NamePackIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3727 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 148: // expr: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 771 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(VariablePackAssign{}, yylhs.location, yystack_[3].value.as < NamePackIndex > (), yystack_[1].value.as < AnyExprIndex > ()); }
#line 3733 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 149: // expr: expr.base OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 777 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentAtNode{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), yystack_[3].value.as < ArgumentListIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3739 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 150: // expr: expr.base OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 780 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentAtNode{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[3].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3745 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 151: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 783 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentAtNode{}, yylhs.location, yystack_[6].value.as < AnyExprIndex > (), yystack_[3].value.as < ArgumentListIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3751 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 152: // expr: expr DOT OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 786 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyExprIndex > () = cxt.create(AssignmentAtNode{}, yylhs.location, yystack_[5].value.as < AnyExprIndex > (), cxt.create(ArgumentList{}, yystack_[3].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3757 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 153: // expr_or_seq.base: expr
#line 793 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < AnyExprIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 3763 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 154: // expr_or_seq.base: expr_or_seq.base SEMICOLON expr
#line 794 "langutils/sc_parser/src/sc_grammar.y"
                                          { yylhs.value.as < AnyExprIndex > () = cxt.create(ExprSeq{}, yylhs.location, yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3769 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 155: // expr_or_seq: expr_or_seq.base semicolon.opt
#line 798 "langutils/sc_parser/src/sc_grammar.y"
                                       { yylhs.value.as < AnyExprIndex > () = yystack_[1].value.as < AnyExprIndex > (); }
#line 3775 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 156: // generator: OPENCURLY COLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 807 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorAnyIndex > () = cxt.create(GeneratorNode{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < GeneratorQualListIndex > ()); }
#line 3781 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 157: // generator: OPENCURLY SEMICOLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 810 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorAnyIndex > () = cxt.create(GeneratorFakeNode{}, yylhs.location, yystack_[4].value.as < AnyExprIndex > (), yystack_[2].value.as < GeneratorQualListIndex > ()); }
#line 3787 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 158: // generator.qual.list: generator.qual
#line 815 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualListIndex > () = cxt.create(GeneratorQualList{}, yylhs.location, yystack_[0].value.as < GeneratorQualAnyIndex > ()); }
#line 3793 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 159: // generator.qual.list: generator.qual.list COMMA generator.qual
#line 817 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualListIndex > () = cxt.graph->append(yystack_[2].value.as < GeneratorQualListIndex > (), yylhs.location, yystack_[0].value.as < GeneratorQualAnyIndex > ()); }
#line 3799 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 160: // generator.qual: name LEFTARROW expr_or_seq
#line 822 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualAssign{}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), cxt.create(Missing{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3805 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 161: // generator.qual: name name LEFTARROW expr_or_seq
#line 824 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualAssign{}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3811 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 162: // generator.qual: VAR name EQUALSSIGN expr_or_seq
#line 826 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualDeclare{}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 3817 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 163: // generator.qual: VAR name OPENPAREN expr_or_seq CLOSEPAREN
#line 828 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualDeclare{}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ()); }
#line 3823 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 164: // generator.qual: expr_or_seq
#line 830 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualIf{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 3829 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 165: // generator.qual: COLON COLON expr_or_seq
#line 832 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualSideEffect{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 3835 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 166: // generator.qual: COLON WHILE expr_or_seq
#line 834 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < GeneratorQualAnyIndex > () = cxt.create(GeneratorQualGuard{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 3841 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 167: // argument_declarations.list: name
#line 847 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3847 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 168: // argument_declarations.list: name EQUALSSIGN expr
#line 849 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed(yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{!is_bracketed}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ())); 
		}
#line 3856 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 169: // argument_declarations.list: name OPENPAREN expr_or_seq CLOSEPAREN
#line 854 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3862 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 170: // argument_declarations.list: argument_declarations.list COMMA name
#line 857 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[2].value.as < DeclareArgumentListIndex > (), yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3868 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 171: // argument_declarations.list: argument_declarations.list COMMA name EQUALSSIGN expr
#line 860 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed(yystack_[0].value.as < AnyExprIndex > ());
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[4].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{!is_bracketed}, range(yystack_[2].location, yystack_[0].location), yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()));
		}
#line 3877 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 172: // argument_declarations.list: argument_declarations.list COMMA name OPENPAREN expr_or_seq CLOSEPAREN
#line 865 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(yystack_[5].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[3].location, yystack_[0].location), yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3883 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 173: // argument_declarations.pipelist: name literal
#line 871 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, yystack_[1].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyPossiblyLiteralIndex > ())); }
#line 3889 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 174: // argument_declarations.pipelist: name
#line 873 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3895 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 175: // argument_declarations.pipelist: name EQUALSSIGN literal
#line 875 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyPossiblyLiteralIndex > ())); }
#line 3901 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 176: // argument_declarations.pipelist: name OPENPAREN expr_or_seq CLOSEPAREN
#line 877 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3907 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 177: // argument_declarations.pipelist: name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 879 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, yystack_[4].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3913 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 178: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name literal
#line 881 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[3].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[1].location, yystack_[0].location), yystack_[1].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyPossiblyLiteralIndex > ())); }
#line 3919 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 179: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name
#line 883 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[2].value.as < DeclareArgumentListIndex > (), yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 3925 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 180: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN literal
#line 885 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[4].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[2].location, yystack_[0].location), yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyPossiblyLiteralIndex > ())); }
#line 3931 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 181: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name OPENPAREN expr_or_seq CLOSEPAREN
#line 887 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[5].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[3].location, yystack_[0].location), yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3937 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 182: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 889 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[6].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[4].location, yystack_[0].location), yystack_[4].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ())); }
#line 3943 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 183: // argument_declarations: ARG SEMICOLON
#line 893 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 3949 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 184: // argument_declarations: ARG argument_declarations.list comma.opt SEMICOLON
#line 894 "langutils/sc_parser/src/sc_grammar.y"
                                                             { yylhs.value.as < DeclareArgumentListIndex > () = yystack_[2].value.as < DeclareArgumentListIndex > (); }
#line 3955 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 185: // argument_declarations: ARG argument_declarations.list ELLIPSIS name SEMICOLON
#line 896 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[3].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); }
#line 3961 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 186: // argument_declarations: ARG argument_declarations.list ELLIPSIS name COMMA name SEMICOLON
#line 898 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append( yystack_[5].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, yystack_[3].value.as < NamedIdentifierIndex > ()), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); }
#line 3967 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 187: // argument_declarations: ARG ELLIPSIS name SEMICOLON
#line 900 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(arg_list, yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); 
		}
#line 3976 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 188: // argument_declarations: ARG ELLIPSIS name COMMA name SEMICOLON
#line 905 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(
				arg_list, yylhs.location,
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, yystack_[3].value.as < NamedIdentifierIndex > ()),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())
			); 
		}
#line 3989 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 189: // argument_declarations: PIPE PIPE
#line 914 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 3995 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 190: // argument_declarations: PIPE argument_declarations.pipelist comma.opt PIPE
#line 916 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = yystack_[2].value.as < DeclareArgumentListIndex > (); }
#line 4001 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 191: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name PIPE
#line 918 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(yystack_[3].value.as < DeclareArgumentListIndex > (), yystack_[3].location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); }
#line 4007 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 192: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name COMMA name PIPE
#line 920 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(yystack_[5].value.as < DeclareArgumentListIndex > (), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, yystack_[3].value.as < NamedIdentifierIndex > ()), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); }
#line 4013 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 193: // argument_declarations: PIPE ELLIPSIS name PIPE
#line 922 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(arg_list, yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())); 
		}
#line 4022 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 194: // argument_declarations: PIPE ELLIPSIS name COMMA name PIPE
#line 927 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			yylhs.value.as < DeclareArgumentListIndex > () = cxt.graph->append(
				arg_list, yylhs.location,
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, yystack_[3].value.as < NamedIdentifierIndex > ()),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, yystack_[1].value.as < NamedIdentifierIndex > ())
			); 
		}
#line 4035 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 195: // argument_declarations.opt: %empty
#line 938 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < DeclareArgumentListIndex > () = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 4041 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 196: // argument_declarations.opt: argument_declarations
#line 939 "langutils/sc_parser/src/sc_grammar.y"
                                { yylhs.value.as < DeclareArgumentListIndex > () = yystack_[0].value.as < DeclareArgumentListIndex > (); }
#line 4047 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 197: // arguments.entries: KEYBINOP expr_or_seq
#line 944 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < ArgumentEntryIndex > () = cxt.create(KwArgNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4053 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 198: // arguments.entries: MULTIPLY expr_or_seq
#line 945 "langutils/sc_parser/src/sc_grammar.y"
                               { yylhs.value.as < ArgumentEntryIndex > () = cxt.create(VariadicArgNode{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 4059 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 199: // arguments.entries: expr_or_seq
#line 946 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < ArgumentEntryIndex > () = yystack_[0].value.as < AnyExprIndex > (); }
#line 4065 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 200: // arguments.no_trailing: arguments.entries
#line 950 "langutils/sc_parser/src/sc_grammar.y"
                            {  yylhs.value.as < ArgumentListIndex > () = cxt.create(ArgumentList{}, yylhs.location, yystack_[0].value.as < ArgumentEntryIndex > ()); }
#line 4071 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 201: // arguments.no_trailing: arguments.no_trailing COMMA arguments.entries
#line 951 "langutils/sc_parser/src/sc_grammar.y"
                                                        { yylhs.value.as < ArgumentListIndex > () = cxt.graph->append(yystack_[2].value.as < ArgumentListIndex > (), yylhs.location, yystack_[0].value.as < ArgumentEntryIndex > ()); }
#line 4077 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 202: // arguments: arguments.no_trailing comma.opt
#line 954 "langutils/sc_parser/src/sc_grammar.y"
                                            { yylhs.value.as < ArgumentListIndex > () = yystack_[1].value.as < ArgumentListIndex > (); }
#line 4083 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 203: // arguments.paren: OPENPAREN arguments CLOSEPAREN
#line 956 "langutils/sc_parser/src/sc_grammar.y"
                                                 { yylhs.value.as < ArgumentListIndex > () = yystack_[1].value.as < ArgumentListIndex > (); }
#line 4089 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 204: // arguments.maybe_paren: %empty
#line 959 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < ArgumentListIndex > () = cxt.create(ArgumentList{}, yylhs.location); }
#line 4095 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 205: // arguments.maybe_paren: OPENPAREN CLOSEPAREN
#line 961 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArgumentListIndex > () = cxt.create(ArgumentList{}, yylhs.location); }
#line 4101 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 206: // arguments.maybe_paren: OPENPAREN arguments CLOSEPAREN
#line 963 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArgumentListIndex > () = yystack_[1].value.as < ArgumentListIndex > (); }
#line 4107 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 207: // variable_declarations.list.item: name
#line 972 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = yystack_[0].value.as < NamedIdentifierIndex > (); }
#line 4113 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 208: // variable_declarations.list.item: name EQUALSSIGN expr
#line 974 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, yystack_[2].value.as < NamedIdentifierIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4119 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 209: // variable_declarations.list.item: name OPENPAREN expr_or_seq CLOSEPAREN
#line 976 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, yystack_[3].value.as < NamedIdentifierIndex > (), yystack_[1].value.as < AnyExprIndex > ()); }
#line 4125 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 210: // variable_declarations.list.item: name.pack EQUALSSIGN expr
#line 978 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = cxt.create(VariablePackAssign{}, yylhs.location, yystack_[2].value.as < NamePackIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4131 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 211: // variable_declarations.list.item: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 980 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareAnyVariableIndex > () = cxt.create(VariablePackAssign{}, yylhs.location, yystack_[3].value.as < NamePackIndex > (), yystack_[1].value.as < AnyExprIndex > ()); }
#line 4137 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 212: // variable_declarations.list: variable_declarations.list.item
#line 985 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DeclareVariableListIndex > () = cxt.create(DeclareVariableList{}, yylhs.location, yystack_[0].value.as < DeclareAnyVariableIndex > ()); }
#line 4143 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 213: // variable_declarations.list: variable_declarations.list COMMA variable_declarations.list.item
#line 987 "langutils/sc_parser/src/sc_grammar.y"
                {yylhs.value.as < DeclareVariableListIndex > () = cxt.graph->append(yystack_[2].value.as < DeclareVariableListIndex > (), yystack_[0].value.as < DeclareAnyVariableIndex > ()); }
#line 4149 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 214: // variable_declarations: VAR variable_declarations.list
#line 990 "langutils/sc_parser/src/sc_grammar.y"
                                                       { yylhs.value.as < DeclareVariableListIndex > () = yystack_[0].value.as < DeclareVariableListIndex > (); }
#line 4155 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 215: // literal.terminal: symbol
#line 998 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < AnySymbolIndex > (); }
#line 4161 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 216: // literal.terminal: string
#line 999 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < StringLitIndex > (); }
#line 4167 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 217: // literal.terminal: integer
#line 1000 "langutils/sc_parser/src/sc_grammar.y"
                  { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < IntLitIndex > (); }
#line 4173 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 218: // literal.terminal: float
#line 1001 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < FloatProducingIndex > (); }
#line 4179 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 219: // literal.terminal: boolean
#line 1002 "langutils/sc_parser/src/sc_grammar.y"
                  { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < BooleanLitIndex > (); }
#line 4185 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 220: // literal.terminal: nil
#line 1003 "langutils/sc_parser/src/sc_grammar.y"
              { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < NilLitIndex > (); }
#line 4191 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 221: // literal.terminal: ascii
#line 1004 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < ASCIIIndex > (); }
#line 4197 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 222: // literal.terminal: block
#line 1005 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < BlockIndex > (); }
#line 4203 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 223: // literal.array.contents: %empty
#line 1010 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.create(ArrayNode{}, yylhs.location); }
#line 4209 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 224: // literal.array.contents: expr_or_seq
#line 1012 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.create(ArrayNode{}, yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 4215 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 225: // literal.array.contents: expr_or_seq COLON expr_or_seq
#line 1014 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.create(ArrayNode{}, yylhs.location, yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4221 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 226: // literal.array.contents: KEYBINOP expr_or_seq
#line 1016 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.create(ArrayNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4227 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 227: // literal.array.contents: literal.array.contents COMMA expr_or_seq
#line 1018 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.graph->append(yystack_[2].value.as < ArrayIndex > (), yylhs.location, yystack_[0].value.as < AnyExprIndex > ()); }
#line 4233 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 228: // literal.array.contents: literal.array.contents COMMA expr_or_seq COLON expr_or_seq
#line 1020 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.graph->append(yystack_[4].value.as < ArrayIndex > (), yylhs.location, yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4239 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 229: // literal.array.contents: literal.array.contents COMMA KEYBINOP expr_or_seq
#line 1022 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < ArrayIndex > () = cxt.graph->append(yystack_[3].value.as < ArrayIndex > (), yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4245 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 230: // literal.dictionary.entry: expr COLON expr_or_seq
#line 1030 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryEntryIndex > () = cxt.create(DictionaryEntryNode{}, yylhs.location, yystack_[2].value.as < AnyExprIndex > (), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4251 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 231: // literal.dictionary.entry: KEYBINOP expr_or_seq
#line 1032 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryEntryIndex > () = cxt.create(DictionaryEntryNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), yystack_[0].value.as < AnyExprIndex > ()); }
#line 4257 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 232: // literal.dictionary.entries: %empty
#line 1037 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryIndex > () = cxt.create(DictionaryNode{}, yylhs.location); }
#line 4263 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 233: // literal.dictionary.entries: literal.dictionary.entry
#line 1039 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryIndex > () = cxt.create(DictionaryNode{}, yylhs.location, yystack_[0].value.as < DictionaryEntryIndex > ()); }
#line 4269 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 234: // literal.dictionary.entries: literal.dictionary.entries COMMA literal.dictionary.entry
#line 1041 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < DictionaryIndex > () = cxt.graph->append(yystack_[2].value.as < DictionaryIndex > (), yylhs.location,  yystack_[0].value.as < DictionaryEntryIndex > ()); }
#line 4275 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 235: // literal.dictionary: OPENPAREN literal.dictionary.entries comma.opt CLOSEPAREN
#line 1046 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->location(*yystack_[2].value.as < DictionaryIndex > ()) = yylhs.location; yylhs.value.as < DictionaryIndex > () = yystack_[2].value.as < DictionaryIndex > (); }
#line 4281 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 236: // literal.array: OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1051 "langutils/sc_parser/src/sc_grammar.y"
                {  cxt.graph->location(*yystack_[2].value.as < ArrayIndex > ()) = yylhs.location; yylhs.value.as < ArrayIndex > () = yystack_[2].value.as < ArrayIndex > (); }
#line 4287 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 237: // literal.array: HASH OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1053 "langutils/sc_parser/src/sc_grammar.y"
                {  
			cxt.graph->payload(yystack_[2].value.as < ArrayIndex > ()).is_immutable = true;
			cxt.graph->location(*yystack_[2].value.as < ArrayIndex > ()) = yylhs.location;
			yylhs.value.as < ArrayIndex > () = yystack_[2].value.as < ArrayIndex > ();
		}
#line 4297 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 238: // literal: literal.terminal
#line 1061 "langutils/sc_parser/src/sc_grammar.y"
                           { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < AnyPossiblyLiteralIndex > (); }
#line 4303 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 239: // literal: literal.array
#line 1062 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < ArrayIndex > (); }
#line 4309 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 240: // literal: literal.dictionary
#line 1063 "langutils/sc_parser/src/sc_grammar.y"
                             { yylhs.value.as < AnyPossiblyLiteralIndex > () = yystack_[0].value.as < DictionaryIndex > (); }
#line 4315 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 241: // name: NAME
#line 1071 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < NamedIdentifierIndex > () = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 4321 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 242: // name: WHILE
#line 1072 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NamedIdentifierIndex > () = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 4327 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 243: // name.list: HASH name
#line 1078 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NameListIndex > () = cxt.create(NameList{}, yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 4333 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 244: // name.list: HASH TILDE name
#line 1080 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NameListIndex > () = cxt.create(NameList{}, yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), yystack_[0].value.as < NamedIdentifierIndex > ())); }
#line 4339 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 245: // name.list: name.list COMMA name
#line 1082 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NameListIndex > () = cxt.graph->append(yystack_[2].value.as < NameListIndex > (), yylhs.location, yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 4345 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 246: // name.list: name.list COMMA TILDE name
#line 1084 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NameListIndex > () = cxt.graph->append(yystack_[3].value.as < NameListIndex > (), yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), yystack_[0].value.as < NamedIdentifierIndex > ())); }
#line 4351 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 247: // name.pack: name.list
#line 1088 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NamePackIndex > () = cxt.create(NamePack{}, yylhs.location, yystack_[0].value.as < NameListIndex > (), cxt.create(Missing{}, yystack_[0].location)); }
#line 4357 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 248: // name.pack: name.list ELLIPSIS name
#line 1090 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NamePackIndex > () = cxt.create(NamePack{}, yylhs.location, yystack_[2].value.as < NameListIndex > (), yystack_[0].value.as < NamedIdentifierIndex > ()); }
#line 4363 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 249: // name.pack: name.list ELLIPSIS TILDE name
#line 1092 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < NamePackIndex > () = cxt.create(NamePack{}, yylhs.location, yystack_[3].value.as < NameListIndex > (), cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), yystack_[0].value.as < NamedIdentifierIndex > ())); }
#line 4369 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 250: // ascii: ASCII
#line 1102 "langutils/sc_parser/src/sc_grammar.y"
              { yylhs.value.as < ASCIIIndex > () = cxt.create(ASCIINode{}, yylhs.location); }
#line 4375 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 251: // nil: NIL
#line 1104 "langutils/sc_parser/src/sc_grammar.y"
          { yylhs.value.as < NilLitIndex > () = cxt.create(NilNode{}, yylhs.location); }
#line 4381 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 252: // boolean: TRUE
#line 1107 "langutils/sc_parser/src/sc_grammar.y"
               { yylhs.value.as < BooleanLitIndex > () = cxt.create(BooleanNode{true}, yylhs.location); }
#line 4387 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 253: // boolean: FALSE
#line 1108 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < BooleanLitIndex > () = cxt.create(BooleanNode{false}, yylhs.location); }
#line 4393 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 254: // symbol: SYMBOL_QUOTE
#line 1112 "langutils/sc_parser/src/sc_grammar.y"
                       { yylhs.value.as < AnySymbolIndex > () = cxt.create(SymbolQuote{}, yylhs.location); }
#line 4399 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 255: // symbol: SYMBOL_SLASH
#line 1113 "langutils/sc_parser/src/sc_grammar.y"
                       { yylhs.value.as < AnySymbolIndex > () = cxt.create(SymbolSlash{}, yylhs.location); }
#line 4405 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 256: // string: STRINGLINE
#line 1117 "langutils/sc_parser/src/sc_grammar.y"
                     { yylhs.value.as < StringLitIndex > () = cxt.create(StringLineList{}, yylhs.location, cxt.create(StringLineNode{}, yylhs.location)); }
#line 4411 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 257: // string: string STRINGLINE
#line 1118 "langutils/sc_parser/src/sc_grammar.y"
                            { yylhs.value.as < StringLitIndex > () = cxt.graph->append(yystack_[1].value.as < StringLitIndex > (), cxt.create(StringLineNode{}, yystack_[0].location)); }
#line 4417 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 258: // integer: INTEGER
#line 1122 "langutils/sc_parser/src/sc_grammar.y"
                  { yylhs.value.as < IntLitIndex > () = cxt.create(IntNode{}, yylhs.location); }
#line 4423 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 259: // integer: INTEGER_RADIX
#line 1123 "langutils/sc_parser/src/sc_grammar.y"
                        { yylhs.value.as < IntLitIndex > () = cxt.create(IntNode{IntNode::Kind::Radix}, yylhs.location); }
#line 4429 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 260: // integer: HEXADECIMAL
#line 1124 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < IntLitIndex > () = cxt.create(IntNode{IntNode::Kind::Hexadecimal}, yylhs.location); }
#line 4435 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 261: // integer: MINUS integer
#line 1126 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Reaches into the previous integer and changes its sign.
			cxt.graph->payload(yystack_[0].value.as < IntLitIndex > ()).sign = IntNode::Sign::Negative;
			yylhs.value.as < IntLitIndex > () = yystack_[0].value.as < IntLitIndex > ();
		}
#line 4445 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 262: // float.raw_unsigned: FLOAT
#line 1134 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < FloatLitIndex > () = cxt.create(FloatNode{}, yylhs.location); }
#line 4451 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 263: // float.raw_unsigned: FLOAT_RADIX
#line 1135 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < FloatLitIndex > () = cxt.create(FloatNode{FloatNode::Kind::Radix}, yylhs.location); }
#line 4457 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 264: // float.raw_unsigned: FLOAT_EXPONENT
#line 1136 "langutils/sc_parser/src/sc_grammar.y"
                         { yylhs.value.as < FloatLitIndex > () = cxt.create(FloatNode{FloatNode::Kind::Exponent}, yylhs.location); }
#line 4463 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 265: // float.raw_unsigned: FLOAT_INF
#line 1137 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < FloatLitIndex > () = cxt.create(FloatNode{FloatNode::Kind::Inf}, yylhs.location); }
#line 4469 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 266: // float.raw: float.raw_unsigned
#line 1142 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < FloatLitIndex > () = yystack_[0].value.as < FloatLitIndex > (); }
#line 4475 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 267: // float.raw: MINUS float.raw_unsigned
#line 1144 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload(yystack_[0].value.as < FloatLitIndex > ()).sign = FloatNode::Sign::Negative; yylhs.value.as < FloatLitIndex > () = yystack_[0].value.as < FloatLitIndex > (); }
#line 4481 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 268: // accidental.unsigned: ACCIDENTAL_STEPS
#line 1149 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AccidentalIndex > () = cxt.create(AccidentalNode{AccidentalNode::Kind::Steps}, yylhs.location); }
#line 4487 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 269: // accidental.unsigned: ACCIDENTAL_CENTS
#line 1151 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AccidentalIndex > () = cxt.create(AccidentalNode{AccidentalNode::Kind::Cents}, yylhs.location); }
#line 4493 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 270: // accidental: accidental.unsigned
#line 1156 "langutils/sc_parser/src/sc_grammar.y"
                { yylhs.value.as < AccidentalIndex > () = yystack_[0].value.as < AccidentalIndex > (); }
#line 4499 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 271: // accidental: MINUS accidental.unsigned
#line 1158 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload(yystack_[0].value.as < AccidentalIndex > ()).sign = AccidentalNode::Sign::Negative; yylhs.value.as < AccidentalIndex > () = yystack_[0].value.as < AccidentalIndex > (); }
#line 4505 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 272: // float: float.raw
#line 1162 "langutils/sc_parser/src/sc_grammar.y"
                    { yylhs.value.as < FloatProducingIndex > () = yystack_[0].value.as < FloatLitIndex > (); }
#line 4511 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 273: // float: accidental
#line 1163 "langutils/sc_parser/src/sc_grammar.y"
                     { yylhs.value.as < FloatProducingIndex > () = yystack_[0].value.as < AccidentalIndex > (); }
#line 4517 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 274: // float: float.raw PI
#line 1164 "langutils/sc_parser/src/sc_grammar.y"
                       { yylhs.value.as < FloatProducingIndex > () = cxt.create(PiNode{}, yylhs.location, yystack_[1].value.as < FloatLitIndex > ()); }
#line 4523 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 275: // float: integer PI
#line 1165 "langutils/sc_parser/src/sc_grammar.y"
                     { yylhs.value.as < FloatProducingIndex > () = cxt.create(PiNode{}, yylhs.location, yystack_[1].value.as < IntLitIndex > ()); }
#line 4529 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 276: // float: PI
#line 1166 "langutils/sc_parser/src/sc_grammar.y"
              { yylhs.value.as < FloatProducingIndex > () = cxt.create(PiNode{}, yylhs.location, cxt.create(Missing{}, yylhs.location)); }
#line 4535 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 277: // float: MINUS PI
#line 1167 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < FloatProducingIndex > () = cxt.create(PiNode{PiNode::Sign::Negative}, yylhs.location, cxt.create(Missing{}, yylhs.location)); }
#line 4541 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 278: // accessor: %empty
#line 1171 "langutils/sc_parser/src/sc_grammar.y"
                 { yylhs.value.as < RWAccessorIndex > () = cxt.create(RWAccessor{ReadWriteAccessor::Private}, yylhs.location); }
#line 4547 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 279: // accessor: LESSTHAN
#line 1172 "langutils/sc_parser/src/sc_grammar.y"
                   { yylhs.value.as < RWAccessorIndex > () = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyReadable}, yylhs.location); }
#line 4553 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 280: // accessor: GREATERTHAN
#line 1173 "langutils/sc_parser/src/sc_grammar.y"
                      { yylhs.value.as < RWAccessorIndex > () = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyWriteable}, yylhs.location); }
#line 4559 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 281: // accessor: READWRITEVAR
#line 1175 "langutils/sc_parser/src/sc_grammar.y"
                { 
			yylhs.value.as < RWAccessorIndex > () = cxt.create(RWAccessor{
				combine<ReadWriteAccessor::ExternallyReadable, ReadWriteAccessor::ExternallyWriteable>()
			}, yylhs.location); 
		}
#line 4569 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;


#line 4573 "langutils/sc_parser/src/sc_grammar_parser.cpp"

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








  const short parser::yypact_ninf_ = -215;

  const short parser::yytable_ninf_ = -283;

  const short
  parser::yypact_[] =
  {
     144,    54,   727,    12,    64,  -215,   224,    11,  -215,  -215,
    -215,    82,    43,  -215,  -215,     4,  1703,   777,  2243,   105,
      92,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,  -215,   126,  -215,     8,    99,  -215,
    -215,  -215,  -215,  -215,  -215,   615,    33,  2243,  -215,   163,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,    90,  2402,
    -215,  1505,    89,  -215,  -215,  -215,  -215,    14,   137,    27,
    -215,  -215,  -215,  -215,    86,    78,  -215,   109,  -215,  -215,
    -215,   127,  -215,  -215,  1753,  1753,   153,  -215,  -215,  -215,
     157,   140,   174,  2243,  2243,   835,  2243,  2402,   176,   171,
     199,  -215,  2243,  1802,  -215,  2243,   615,  -215,  -215,  -215,
    -215,    33,  -215,  -215,   196,   633,   134,  1101,   226,  -215,
     232,  2402,  1703,    92,  -215,   216,  1703,  1150,  -215,    29,
    -215,    29,   147,  -215,   243,    32,    35,  -215,    92,   167,
      36,  -215,   124,  -215,  -215,  -215,    92,  -215,   172,  2288,
    -215,  -215,  1203,   948,  -215,  -215,  -215,   121,   207,  2243,
    2243,    18,  -215,  2402,  -215,  1505,   268,  1252,  2243,    29,
     158,   180,  2243,  2243,  -215,  -215,  -215,   193,  -215,  -215,
    -215,  -215,  -215,   206,   257,   259,  -215,  2243,  -215,  2243,
    1851,   265,   264,  2243,    37,  -215,   102,  2243,  -215,  2243,
    1900,  -215,    18,  1505,  1949,   266,   199,  -215,  2243,   199,
      29,  2243,  2243,  -215,  -215,   262,   269,  -215,  -215,  -215,
      29,     8,  2243,  2243,  2243,  2243,   246,    92,    92,   272,
    2243,  2243,    15,  -215,    92,    23,  -215,  1949,   273,  2362,
    -215,  -215,    18,   234,  2243,   115,   998,  1302,    29,    38,
     108,  -215,   222,  2402,  1555,  2243,   288,    18,  -215,    29,
     281,  2402,    92,  -215,    92,  -215,   291,  2402,   239,   297,
    -215,  -215,   193,   298,  -215,  -215,   164,   164,   164,  -215,
     292,   193,  1604,  1604,  2402,  -215,  2243,   256,  -215,  -215,
     296,  2243,  1998,  1352,    29,  -215,   267,  -215,   301,   303,
      18,  2243,   633,  -215,  -215,   300,  2402,   306,  -215,  -215,
    -215,  1653,  -215,  -215,  -215,   305,  2402,   307,  2402,  -215,
      92,    45,   250,  -215,   309,  2402,    92,  -215,    20,  -215,
    2325,   311,  1949,  -215,   310,  2243,   314,   274,  2047,   276,
    2243,   128,    29,  -215,  1402,  2243,    29,  2243,  -215,  -215,
    2243,  -215,  2402,  -215,   322,    29,    26,  -215,  -215,  -215,
    -215,   893,  -215,  -215,  -215,  -215,  -215,  -215,   315,     8,
     315,   315,   206,  -215,   321,    92,   215,  -215,   316,  -215,
      16,   316,  -215,  2243,  -215,   293,  -215,   320,    29,   323,
    2096,  -215,  -215,   327,  -215,  -215,  -215,  -215,  -215,   329,
    2243,  2243,  -215,    92,  -215,   283,    92,  -215,  1949,  2399,
    -215,  -215,   332,  -215,  2402,   295,  2243,   302,   335,  2243,
     340,   304,  2145,  -215,  -215,   341,  2402,  -215,   343,  2402,
    -215,  2243,  -215,  -215,  1051,   164,  -215,  -215,  -215,    49,
    2243,  2243,  1604,   349,  2243,   313,   350,  -215,  2194,  -215,
      29,    29,  -215,   347,  -215,  -215,   351,  2402,   352,  -215,
     308,   357,  1949,  -215,  -215,  2243,  2402,  2243,   312,  2402,
     318,  2243,   319,   364,  -215,  -215,  2402,  -215,   365,    18,
    -215,  2243,  2243,  -215,  -215,  -215,  -215,  -215,  2243,  -215,
    -215,   367,  -215,  -215,  -215,  -215,  -215,  -215,   368,  2402,
    2402,  2243,  2243,  2402,  2243,   325,  -215,  1455,   375,   372,
    -215,  -215,  -215,  -215,  2402,  2402,  2402,  2243,  -215,    18,
    -215,  -215,  2402,   377,  -215
  };

  const short
  parser::yydefact_[] =
  {
       6,    27,     0,     0,     0,     4,     0,     0,    20,    22,
      23,     0,    25,     3,    16,    54,   223,     0,     0,     0,
       0,   241,   258,   259,   260,   262,   263,   264,   265,   268,
     269,   254,   255,   256,   250,   142,   124,     0,     0,   251,
     252,   253,   276,    55,   242,     0,     0,     0,    15,   195,
     222,   125,   136,   137,   131,   135,   132,   133,   134,    10,
     122,     0,   282,   238,   240,   239,   121,   123,   247,     0,
     221,   220,   219,   215,   216,   217,   266,   272,   270,   273,
     218,     0,     1,    19,     0,   283,     0,     5,    21,    29,
       0,     0,     0,     0,     0,     0,     0,   153,   282,   224,
     284,   127,     0,     0,    74,    83,    78,    76,    77,    79,
      80,    81,    75,    82,     0,   153,     0,     0,     0,   233,
     284,    11,   223,     0,   243,   128,   223,     0,    60,    96,
      61,    58,     0,   212,   214,   207,     0,   183,     0,   284,
     167,   277,     0,   261,   267,   271,     0,   189,   284,   174,
     143,   196,     0,     0,    83,    78,    81,     0,    85,     0,
       0,   282,    64,    69,    70,   283,     0,     0,     0,    91,
       0,     0,     0,     0,   257,   275,   274,    52,    18,    17,
       2,    28,    26,    39,     0,     0,   226,   283,   155,     0,
     285,     0,     0,     0,     0,   231,     0,     0,   126,     0,
       0,    13,   282,     0,   285,     0,   284,   244,     0,   284,
      58,     0,     0,   199,   200,   284,     0,    62,    63,    97,
      59,     0,     0,     0,     0,     0,     0,   285,     0,     0,
       0,     0,     0,   285,     0,     0,    54,   232,     0,     0,
     173,    57,   282,   130,     0,     0,     0,     0,    58,   204,
       0,    84,   140,    68,   283,     0,     0,   282,     9,     0,
       0,   144,     0,   245,     0,   248,     0,   147,    79,     0,
      48,    50,    53,     0,    42,    41,   278,   278,   278,    37,
     282,    52,     0,     0,   154,   225,     0,   227,   236,   115,
       0,     0,     0,     0,    88,   230,     0,   111,     0,     0,
     282,     0,     0,   234,   235,     0,   145,     0,    98,   197,
     198,   285,   202,   203,   213,     0,   208,     0,   210,   187,
       0,   170,     0,   184,     0,   168,     0,   193,     0,   190,
     179,     0,   232,   175,     0,     0,     0,   129,     0,   139,
       0,     0,    58,    93,     0,     0,    58,     0,    71,    72,
       0,    65,    66,     7,     0,    90,    58,   246,   249,   148,
      49,     0,    51,    30,   279,   280,   281,    32,    35,     0,
      34,    36,   283,    40,     0,     0,     0,   164,   284,   158,
     123,   284,   229,     0,   119,     0,   120,     0,     0,     0,
       0,   112,    12,     0,   237,    95,   201,   209,   211,     0,
       0,     0,   185,     0,   169,     0,     0,   191,   232,     0,
     178,   176,     0,    56,   150,   107,     0,   106,     0,     0,
       0,   138,     0,    94,   205,     0,   141,    92,     0,    67,
       8,     0,    89,    47,     0,   278,    31,    38,    24,     0,
       0,     0,   285,     0,     0,     0,     0,   228,     0,   116,
      87,    58,   114,     0,    14,   188,     0,   171,     0,   194,
       0,     0,   232,   180,   177,     0,   149,     0,   105,   152,
     100,     0,   101,     0,   206,    73,   146,    44,   282,   282,
      33,     0,     0,   166,   165,   159,   157,   160,     0,   156,
     118,     0,    86,   113,   172,   186,   192,   181,     0,   110,
     109,     0,     0,   151,     0,    99,   283,     0,     0,     0,
     162,   161,   117,   182,   108,   104,   103,     0,    46,   282,
      43,   163,   102,     0,    45
  };

  const short
  parser::yypgoto_[] =
  {
    -215,  -215,  -215,   113,  -215,  -215,   376,  -215,  -215,  -215,
    -215,   -51,   -45,    13,  -215,  -215,  -215,   118,   117,  -215,
     106,  -215,   271,  -209,   -31,  -115,   136,  -215,  -160,    -9,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,    -2,
    -215,    87,   324,   110,   -48,  -215,  -215,   213,    34,    85,
    -215,  -141,   240,  -215,  -214,  -215,   214,  -215,   -75,   194,
    -215,  -215,  -215,  -139,   326,  -215,   -34,  -215,  -215,  -215,
    -215,  -215,   -40,   -36,  -215,   -32,  -215,  -215,  -215,   -43,
    -114
  };

  const short
  parser::yydefgoto_[] =
  {
       0,     4,     5,    48,     6,     7,     8,     9,    92,    12,
      10,   367,   368,   279,   280,   281,   269,   270,   271,   272,
     273,    49,    50,   219,   220,   161,   162,   251,   113,   158,
     159,    51,    52,    53,    54,    55,    56,    57,    58,    97,
      98,   213,    60,   378,   379,   139,   148,   151,   152,   214,
     215,   216,   131,   346,   133,   134,   164,    63,   100,   119,
     120,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,   369,    86,
     191
  };

  const short
  parser::yytable_[] =
  {
      59,   308,   202,   136,   129,   143,   205,   314,   114,   144,
     240,    87,   245,   145,    93,   115,   121,   274,    15,   166,
      15,   132,   167,    21,   167,   229,   260,   326,   254,   255,
      15,    21,   406,    15,   235,   172,   169,   242,    21,   343,
     222,     1,    81,   224,   230,   150,   344,   206,    21,   291,
     257,   209,    94,   400,    44,   188,    43,   481,    43,   163,
      11,   444,    44,   168,    82,   168,   143,     3,    43,    44,
     144,    43,   327,   146,   145,   431,   173,   407,   292,    44,
     329,   223,    59,    59,   225,   231,   114,   345,   300,    89,
     147,    91,   305,   115,   401,   307,   153,    21,   482,   165,
     333,   312,   143,    99,   116,   341,    15,    21,   274,   137,
     293,   122,   274,   174,    21,   163,   347,   175,   256,   123,
      21,   274,   337,    21,    22,    23,    24,   246,    44,   247,
      15,   177,   126,   423,   127,   421,    21,   427,    44,   138,
      22,    23,    24,   198,    43,    44,   199,   432,   176,   170,
     163,    44,   389,   180,    44,   436,   338,   252,   253,   299,
     142,   123,    21,   163,   181,   294,   261,    44,    43,   422,
     182,   267,   262,    21,     1,   200,   142,   171,   183,   227,
     184,   185,   116,   186,   233,   284,   187,   136,     2,   192,
     194,   410,   195,    44,   264,    21,    38,   178,   179,   334,
       3,   163,   302,   425,    44,   196,   306,   228,    21,    99,
     349,   190,   234,    99,   354,    61,    62,   364,   365,   189,
      46,   316,   366,   318,  -282,    83,    44,    84,   355,   325,
     117,   118,   370,   371,    85,   115,   203,   373,   276,    44,
     277,   278,   492,   104,   204,   155,   107,   108,   268,   110,
     156,   112,   163,   352,    21,   221,   319,   393,   320,   266,
     402,   440,   403,   441,   443,   208,   250,   446,   258,   282,
     463,   283,   288,   289,   311,   304,   285,   287,   313,   122,
     290,   157,   323,   335,   295,    44,   296,   298,   353,   104,
     356,   155,   107,   108,   109,   110,   156,   112,   309,   310,
     359,   361,   372,   363,   383,   384,   128,   394,   390,   315,
     391,   317,   392,   395,   397,   413,   398,   324,   404,   479,
     411,   415,   430,   416,   331,   419,   438,   435,   442,   449,
     115,   336,   451,   414,   448,   136,   454,    90,   128,   455,
     459,   464,   468,   426,   465,   124,   125,   470,   429,   169,
     474,   467,   475,   471,   486,   489,   493,   450,   488,   130,
     494,   501,   495,   135,   140,   496,   497,   502,   504,   377,
     377,   505,   149,   382,   517,   506,   512,   513,   385,   387,
     520,   521,   524,    88,   480,   437,   360,   374,   195,   362,
     351,   130,   519,   381,   485,   434,   396,   248,   303,   457,
     217,     0,   128,     0,     0,     0,   115,     0,     0,     0,
       0,     0,     0,     0,   466,     0,     0,   469,     0,   412,
       0,     0,     0,     0,     0,   418,     0,   420,     0,   476,
       0,     0,   163,     0,   428,   507,   508,   149,     0,     0,
     217,     0,     0,     0,     0,     0,     0,     0,     0,   207,
       0,     0,     0,   218,     0,   130,     0,     0,   124,     0,
     115,     0,     0,   499,   226,   500,     0,   128,     0,   503,
     447,     0,   232,     0,     0,     0,   523,   453,     0,     0,
       0,   128,     0,   249,     0,     0,     0,   456,     0,     0,
       0,   217,     0,   218,     0,   461,   263,   265,     0,   514,
     515,     0,   516,   275,     0,   163,     0,     0,     0,   473,
       0,     0,     0,     0,     0,   522,     0,     0,     0,   128,
     130,     0,     0,     0,     0,     0,     0,   483,   484,   377,
     128,   487,     0,     0,   130,   491,     0,     0,     0,     0,
       0,     0,     0,     0,   218,     0,     0,   135,     0,   498,
       0,     0,     0,   321,   322,     0,     0,     0,     0,     0,
     328,   330,     0,     0,     0,   217,     0,     0,   509,   510,
       0,     0,   130,     0,     0,   511,   348,     0,     0,     0,
       0,     0,     0,   130,     0,     0,     0,     0,   357,     0,
     358,     0,     0,     0,   275,     0,     0,     0,   275,     0,
       0,     0,     0,     0,     0,     0,     0,   275,   380,   380,
       0,     0,     0,   128,     0,     0,     0,   128,   218,     0,
       0,     0,     0,     0,     0,     0,   217,   128,     0,     0,
       0,    22,    23,    24,    25,    26,    27,    28,    29,    30,
       0,     0,     0,     0,     0,     0,   399,     0,     0,     0,
       0,   128,   405,     0,   141,     0,     0,     0,     0,   128,
       0,     0,     0,     0,     0,     0,   130,   142,     0,     0,
     130,     0,     0,     0,     0,     0,     0,     0,     0,   218,
     130,   197,     0,   104,   154,   155,   107,   108,   109,   110,
     156,   112,   157,     0,     0,   135,     0,     0,     0,     0,
       0,   439,     0,     0,   130,     0,   445,     0,     0,     0,
       0,     0,   130,     0,     0,     0,     0,     0,     0,     0,
       0,   217,   128,     0,     0,     0,     0,    13,    14,   458,
       0,    15,   460,    16,     0,    17,     0,     0,    18,     0,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,     0,    35,    36,    37,
      38,     0,     0,    39,    40,    41,    42,     0,   380,    43,
       0,     0,     0,    44,   218,   130,     0,     0,   101,    45,
       0,    15,     0,    16,    46,    95,  -232,    47,     0,  -232,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,     0,    35,    36,    37,
      38,     0,     0,    39,    40,    41,    42,     0,   102,    43,
       0,     0,     0,    44,     0,   103,     0,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   101,    47,     0,    15,
       0,    16,     0,    95,  -232,     0,     0,  -232,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,    35,    36,     0,     0,     0,
       0,    39,    40,    41,    42,     0,   102,    43,     0,     0,
       0,    44,     0,   103,     0,   104,   105,   106,   107,   108,
     109,   110,   156,   112,   433,    47,     0,  -195,  -195,  -195,
       0,  -195,     0,     0,  -195,     0,  -195,  -195,  -195,  -195,
    -195,  -195,  -195,  -195,  -195,  -195,  -195,  -195,  -195,  -195,
    -195,  -195,  -195,  -195,  -195,  -195,    38,     0,     0,  -195,
    -195,  -195,  -195,     0,     0,  -195,     0,     0,     0,  -195,
       0,     0,     0,     0,     0,  -195,     0,     0,     0,     0,
      46,     0,    15,  -195,    16,   243,    95,     0,     0,     0,
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,     0,    35,    36,
       0,     0,     0,     0,    39,    40,    41,    42,     0,   244,
      43,     0,     0,     0,    44,     0,     0,     0,     0,   211,
      45,     0,    15,   212,    16,   339,    95,     0,    47,     0,
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,     0,    35,    36,
       0,     0,     0,     0,    39,    40,    41,    42,     0,   340,
      43,     0,     0,     0,    44,     0,     0,     0,     0,   211,
      45,     0,     0,   212,     0,    15,   477,    16,    47,    95,
       0,     0,   160,     0,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
     478,    35,    36,    37,     0,     0,     0,    39,    40,    41,
      42,     0,     0,    43,     0,     0,     0,    44,     0,     0,
       0,     0,     0,    45,     0,    15,     0,    16,     0,    95,
     201,    47,   160,     0,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
       0,    35,    36,    37,     0,     0,     0,    39,    40,    41,
      42,     0,     0,    43,     0,     0,     0,    44,     0,     0,
       0,     0,     0,    45,    15,     0,    16,     0,    95,   210,
       0,    47,     0,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,     0,
      35,    36,     0,     0,     0,     0,    39,    40,    41,    42,
       0,     0,    43,     0,     0,     0,    44,     0,     0,     0,
       0,   211,    45,     0,     0,   212,     0,    15,   241,    16,
      47,    95,     0,     0,   160,     0,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,     0,    35,    36,    37,     0,     0,     0,    39,
      40,    41,    42,     0,     0,    43,     0,     0,     0,    44,
       0,     0,     0,     0,     0,    45,    15,     0,    16,     0,
      95,   259,     0,    47,     0,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,     0,    35,    36,     0,     0,     0,     0,    39,    40,
      41,    42,     0,     0,    43,     0,     0,     0,    44,     0,
       0,     0,     0,   211,    45,     0,    15,   212,    16,     0,
      95,   342,    47,     0,     0,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,     0,    35,    36,     0,     0,     0,     0,    39,    40,
      41,    42,     0,     0,    43,     0,     0,     0,    44,     0,
       0,     0,     0,   211,    45,     0,    15,   212,    16,     0,
      95,   388,    47,     0,     0,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,     0,    35,    36,     0,     0,     0,     0,    39,    40,
      41,    42,     0,     0,    43,     0,     0,     0,    44,     0,
       0,     0,     0,   211,    45,     0,    15,   212,    16,     0,
      95,   424,    47,     0,     0,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,     0,    35,    36,     0,     0,     0,     0,    39,    40,
      41,    42,     0,     0,    43,     0,     0,     0,    44,     0,
       0,     0,     0,   211,    45,     0,     0,   212,     0,    15,
     518,    16,    47,    95,     0,     0,   160,     0,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,    35,    36,    37,     0,     0,
       0,    39,    40,    41,    42,     0,     0,    43,     0,     0,
       0,    44,     0,     0,     0,     0,     0,    45,     0,    15,
       0,    16,     0,    95,     0,    47,   160,     0,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,    35,    36,    37,     0,     0,
       0,    39,    40,    41,    42,     0,     0,    43,     0,     0,
       0,    44,     0,     0,     0,     0,     0,    45,     0,    15,
       0,    16,     0,    95,     0,    47,   350,     0,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,    35,    36,    37,     0,     0,
       0,    39,    40,    41,    42,     0,     0,    43,     0,     0,
       0,    44,     0,     0,     0,     0,     0,    45,    15,     0,
      16,     0,    95,     0,     0,    47,     0,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,     0,    35,    36,   375,     0,     0,     0,
      39,    40,    41,    42,     0,     0,    43,     0,     0,     0,
      44,     0,   376,     0,     0,     0,    45,    15,     0,    16,
       0,    95,     0,     0,    47,     0,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,     0,    35,    36,     0,     0,     0,     0,    39,
      40,    41,    42,     0,     0,    43,     0,     0,     0,    44,
       0,     0,     0,     0,   211,    45,     0,    15,   212,    16,
       0,    95,     0,    47,     0,     0,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,     0,    35,    36,     0,     0,     0,     0,    39,
      40,    41,    42,     0,     0,    43,     0,     0,     0,    44,
       0,     0,     0,     0,    96,    45,     0,    15,     0,    16,
       0,    17,     0,    47,    18,     0,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,     0,    35,    36,     0,     0,     0,     0,    39,
      40,    41,    42,     0,     0,    43,     0,     0,     0,    44,
       0,     0,     0,     0,     0,    45,    15,     0,    16,     0,
      95,     0,     0,    47,     0,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,     0,    35,    36,     0,     0,     0,     0,    39,    40,
      41,    42,     0,   193,    43,     0,     0,     0,    44,     0,
       0,     0,     0,     0,    45,    15,     0,    16,     0,    95,
       0,     0,    47,     0,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
       0,    35,    36,     0,     0,     0,     0,    39,    40,    41,
      42,     0,     0,    43,     0,     0,     0,    44,     0,     0,
       0,     0,   286,    45,    15,     0,    16,     0,    95,   297,
       0,    47,     0,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,     0,
      35,    36,     0,     0,     0,     0,    39,    40,    41,    42,
       0,     0,    43,     0,     0,     0,    44,     0,     0,     0,
       0,     0,    45,    15,     0,    16,     0,    95,     0,     0,
      47,     0,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,     0,    35,
      36,     0,     0,     0,     0,    39,    40,    41,    42,     0,
       0,    43,     0,     0,     0,    44,     0,     0,     0,     0,
     301,    45,    15,     0,    16,     0,    95,   386,     0,    47,
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,     0,    35,    36,
       0,     0,     0,     0,    39,    40,    41,    42,     0,     0,
      43,     0,     0,     0,    44,     0,     0,     0,     0,     0,
      45,    15,     0,    16,   417,    95,     0,     0,    47,     0,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,     0,    35,    36,     0,
       0,     0,     0,    39,    40,    41,    42,     0,     0,    43,
       0,     0,     0,    44,     0,     0,     0,     0,     0,    45,
      15,     0,    16,     0,    95,   452,     0,    47,     0,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,     0,    35,    36,     0,     0,
       0,     0,    39,    40,    41,    42,     0,     0,    43,     0,
       0,     0,    44,     0,     0,     0,     0,     0,    45,    15,
       0,    16,   472,    95,     0,     0,    47,     0,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,    35,    36,     0,     0,     0,
       0,    39,    40,    41,    42,     0,     0,    43,     0,     0,
       0,    44,     0,     0,     0,     0,     0,    45,    15,     0,
      16,     0,    95,   490,     0,    47,     0,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,     0,    35,    36,     0,     0,     0,     0,
      39,    40,    41,    42,     0,     0,    43,     0,     0,     0,
      44,     0,     0,     0,     0,     0,    45,    15,     0,    16,
       0,    95,     0,     0,    47,     0,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,     0,    35,    36,     0,     0,     0,     0,    39,
      40,    41,    42,     0,     0,    43,     0,     0,     0,    44,
       0,     0,   236,     0,    16,    45,   237,     0,     0,     0,
       0,   238,     0,    47,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,     0,     0,     0,
       0,     0,     0,     0,    39,    40,    41,    42,     0,   236,
      43,    16,     0,   408,     0,     0,     0,   239,   238,     0,
      45,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,     0,     0,     0,     0,     0,
       0,    39,    40,    41,    42,     0,   236,    43,    16,     0,
     332,     0,     0,     0,   409,   238,     0,    45,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,     0,     0,     0,     0,     0,     0,     0,    39,    40,
      41,    42,     0,   236,    43,    16,     0,   462,     0,     0,
       0,     0,   238,     0,    45,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,     0,     0,
       0,     0,     0,     0,     0,    39,    40,    41,    42,     0,
       0,    43,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    45,   104,   154,   155,   107,   108,   109,   110,   156,
     112,   157
  };

  const short
  parser::yycheck_[] =
  {
       2,   210,   117,    37,    35,    45,   120,   221,    17,    45,
     149,     0,   153,    45,    10,    17,    18,   177,     4,    62,
       4,    13,     8,    15,     8,   139,   167,    12,    10,    11,
       4,    15,    12,     4,   148,     8,    67,   152,    15,   248,
       8,    30,    30,     8,     8,    47,     8,   122,    15,    12,
     165,   126,    48,     8,    46,    98,    42,     8,    42,    61,
       6,    45,    46,    49,     0,    49,   106,    56,    42,    46,
     106,    42,    57,    40,   106,    49,    49,    57,    41,    46,
      57,    49,    84,    85,    49,    49,    95,    49,   203,     7,
      57,    48,   206,    95,    49,   209,     6,    15,    49,    10,
     239,   215,   142,    16,    17,   246,     4,    15,   268,    10,
       8,     6,   272,    27,    15,   117,     8,    39,   161,    14,
      15,   281,     7,    15,    16,    17,    18,     6,    46,     8,
       4,     4,     6,   342,     8,     7,    15,   346,    46,    40,
      16,    17,    18,     9,    42,    46,    12,   356,    39,    12,
     152,    46,   293,     0,    46,   369,    41,   159,   160,   202,
      52,    14,    15,   165,     7,   196,   168,    46,    42,    41,
      30,   173,    14,    15,    30,    41,    52,    40,     4,    12,
      93,    94,    95,    96,    12,   187,    10,   221,    44,   102,
     103,   330,   105,    46,    14,    15,    33,    84,    85,   242,
      56,   203,   204,   344,    46,     9,   208,    40,    15,   122,
     250,    12,    40,   126,   257,     2,     2,    53,    54,    48,
      57,   223,    58,   225,     0,     1,    46,     3,   259,   231,
      17,    17,   277,   278,    10,   237,    10,   280,    32,    46,
      34,    35,   451,    50,    12,    52,    53,    54,    55,    56,
      57,    58,   254,   255,    15,    12,    10,   300,    12,   172,
      10,    46,    12,    48,   378,    49,    59,   381,     0,    12,
     409,    12,     7,     9,    12,     9,   189,   190,     9,     6,
     193,    59,    10,    49,   197,    46,   199,   200,     0,    50,
       9,    52,    53,    54,    55,    56,    57,    58,   211,   212,
       9,     4,    10,     5,    48,     9,    35,     7,    41,   222,
       9,   224,     9,     7,     9,     5,     9,   230,     9,   434,
       9,     7,     0,    49,   237,    49,     5,    12,    12,     9,
     332,   244,     9,   335,    41,   369,     9,    11,    67,    10,
      57,     9,     7,   345,    49,    19,    20,     7,   350,   380,
       9,    49,     9,    49,     5,     5,     9,   388,    45,    35,
       9,    49,    10,    37,    38,    57,     9,    49,    49,   282,
     283,     7,    46,   286,    49,    10,     9,     9,   291,   292,
       5,     9,     5,     7,   435,   372,   268,   281,   301,   272,
     254,    67,   507,   283,   442,   361,   311,   157,   204,   401,
     129,    -1,   131,    -1,    -1,    -1,   408,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   416,    -1,    -1,   419,    -1,   332,
      -1,    -1,    -1,    -1,    -1,   338,    -1,   340,    -1,   431,
      -1,    -1,   434,    -1,   347,   478,   479,   111,    -1,    -1,
     169,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   123,
      -1,    -1,    -1,   129,    -1,   131,    -1,    -1,   132,    -1,
     462,    -1,    -1,   465,   138,   467,    -1,   196,    -1,   471,
     383,    -1,   146,    -1,    -1,    -1,   519,   390,    -1,    -1,
      -1,   210,    -1,   157,    -1,    -1,    -1,   400,    -1,    -1,
      -1,   220,    -1,   169,    -1,   408,   170,   171,    -1,   501,
     502,    -1,   504,   177,    -1,   507,    -1,    -1,    -1,   422,
      -1,    -1,    -1,    -1,    -1,   517,    -1,    -1,    -1,   248,
     196,    -1,    -1,    -1,    -1,    -1,    -1,   440,   441,   442,
     259,   444,    -1,    -1,   210,   448,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   220,    -1,    -1,   221,    -1,   462,
      -1,    -1,    -1,   227,   228,    -1,    -1,    -1,    -1,    -1,
     234,   235,    -1,    -1,    -1,   294,    -1,    -1,   481,   482,
      -1,    -1,   248,    -1,    -1,   488,   250,    -1,    -1,    -1,
      -1,    -1,    -1,   259,    -1,    -1,    -1,    -1,   262,    -1,
     264,    -1,    -1,    -1,   268,    -1,    -1,    -1,   272,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   281,   282,   283,
      -1,    -1,    -1,   342,    -1,    -1,    -1,   346,   294,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   355,   356,    -1,    -1,
      -1,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    -1,   320,    -1,    -1,    -1,
      -1,   380,   326,    -1,    39,    -1,    -1,    -1,    -1,   388,
      -1,    -1,    -1,    -1,    -1,    -1,   342,    52,    -1,    -1,
     346,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   355,
     356,    48,    -1,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,   369,    -1,    -1,    -1,    -1,
      -1,   375,    -1,    -1,   380,    -1,   380,    -1,    -1,    -1,
      -1,    -1,   388,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   450,   451,    -1,    -1,    -1,    -1,     0,     1,   403,
      -1,     4,   406,     6,    -1,     8,    -1,    -1,    11,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    32,
      33,    -1,    -1,    36,    37,    38,    39,    -1,   442,    42,
      -1,    -1,    -1,    46,   450,   451,    -1,    -1,     1,    52,
      -1,     4,    -1,     6,    57,     8,     9,    60,    -1,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    32,
      33,    -1,    -1,    36,    37,    38,    39,    -1,    41,    42,
      -1,    -1,    -1,    46,    -1,    48,    -1,    50,    51,    52,
      53,    54,    55,    56,    57,    58,     1,    60,    -1,     4,
      -1,     6,    -1,     8,     9,    -1,    -1,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    41,    42,    -1,    -1,
      -1,    46,    -1,    48,    -1,    50,    51,    52,    53,    54,
      55,    56,    57,    58,     1,    60,    -1,     4,     5,     6,
      -1,     8,    -1,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,
      -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,    -1,    -1,
      57,    -1,     4,    60,     6,     7,     8,    -1,    -1,    -1,
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
      -1,    -1,    -1,    -1,    51,    52,    -1,     4,    -1,     6,
      -1,     8,    -1,    60,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    46,
      -1,    -1,    -1,    -1,    -1,    52,     4,    -1,     6,    -1,
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
      -1,    -1,     4,    -1,     6,    52,     8,    -1,    -1,    -1,
      -1,    13,    -1,    60,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,     4,
      42,     6,    -1,     8,    -1,    -1,    -1,    49,    13,    -1,
      52,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,     4,    42,     6,    -1,
       8,    -1,    -1,    -1,    49,    13,    -1,    52,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,     4,    42,     6,    -1,     8,    -1,    -1,
      -1,    -1,    13,    -1,    52,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
      -1,    42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59
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
     120,     1,    41,    48,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    90,    91,   101,   103,   109,   118,   121,
     122,   101,     6,    14,   126,   126,     6,     8,    84,    86,
     104,   114,    13,   116,   117,   126,   128,    10,    40,   107,
     126,    39,    52,   134,   135,   137,    40,    57,   108,   126,
     101,   109,   110,     6,    51,    52,    57,    59,    91,    92,
      11,    87,    88,   101,   118,    10,   141,     8,    49,    86,
      12,    40,     8,    49,    27,    39,    39,     4,    65,    65,
       0,     7,    30,     4,   103,   103,   103,    10,   141,    48,
      12,   142,   103,    41,   103,   103,     9,    48,     9,    12,
      41,     9,    87,    10,    12,   142,   120,   126,    49,   120,
       9,    51,    55,   103,   111,   112,   113,    84,   104,    85,
      86,    12,     8,    49,     8,    49,   126,    12,    40,   142,
       8,    49,   126,    12,    40,   142,     4,     8,    13,    49,
     125,     5,    87,     7,    41,   113,     6,     8,   114,   126,
      59,    89,   101,   101,    10,    11,   141,    87,     0,     9,
     113,   101,    14,   126,    14,   126,   103,   101,    55,    78,
      79,    80,    81,    82,    90,   126,    32,    34,    35,    75,
      76,    77,    12,    12,   101,   103,    51,   103,     7,     9,
     103,    12,    41,     8,    86,   103,   103,     9,   103,   141,
      87,    51,   101,   121,     9,   142,   101,   142,    85,   103,
     103,    12,   142,     9,   116,   103,   101,   103,   101,    10,
      12,   126,   126,    10,   103,   101,    12,    57,   126,    57,
     126,   103,     8,   125,   141,    49,   103,     7,    41,     7,
      41,   113,     9,    85,     8,    49,   115,     8,   126,   134,
      11,    88,   101,     0,   141,    86,     9,   126,   126,     9,
      79,     4,    80,     5,    53,    54,    58,    73,    74,   140,
      74,    74,    10,   141,    82,    32,    48,   103,   105,   106,
     126,   105,   103,    48,     9,   103,     9,   103,     9,   113,
      41,     9,     9,   141,     7,     7,   111,     9,     9,   126,
       8,    49,    10,    12,     9,   126,    12,    57,     8,    49,
     125,     9,   103,     5,   101,     7,    49,     7,   103,    49,
     103,     7,    41,    85,     9,   113,   101,    85,   103,   101,
       0,    49,    85,     1,   110,    12,   116,    75,     5,   126,
      46,    48,    12,   142,    45,   126,   142,   103,    41,     9,
      86,     9,     9,   103,     9,    10,   103,   101,   126,    57,
     126,   103,     8,   125,     9,    49,   101,    49,     7,   101,
       7,    49,     7,   103,     9,     9,   101,     5,    29,    87,
      73,     8,    49,   103,   103,   106,     5,   103,    45,     5,
       9,   103,    85,     9,     9,    10,    57,     9,   103,   101,
     101,    49,    49,   101,    49,     7,    10,   141,   141,   103,
     103,   103,     9,     9,   101,   101,   101,    49,     5,    87,
       5,     9,   101,   141,     5
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
     100,   100,   100,   100,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   102,   102,   103,   104,   104,   105,   105,
     106,   106,   106,   106,   106,   106,   106,   107,   107,   107,
     107,   107,   107,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   110,   110,   111,   111,   111,
     112,   112,   113,   114,   115,   115,   115,   116,   116,   116,
     116,   116,   117,   117,   118,   119,   119,   119,   119,   119,
     119,   119,   119,   120,   120,   120,   120,   120,   120,   120,
     121,   121,   122,   122,   122,   123,   124,   124,   125,   125,
     125,   126,   126,   127,   127,   127,   127,   128,   128,   128,
     129,   130,   131,   131,   132,   132,   133,   133,   134,   134,
     134,   134,   135,   135,   135,   135,   136,   136,   137,   137,
     138,   138,   139,   139,   139,   139,   139,   139,   140,   140,
     140,   140,   141,   141,   142,   142
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
       5,     1,     1,     1,     1,     1,     3,     2,     2,     4,
       3,     1,     1,     1,     1,     1,     1,     1,     5,     4,
       3,     5,     1,     2,     3,     4,     6,     3,     4,     6,
       5,     7,     6,     1,     3,     2,     7,     7,     1,     3,
       3,     4,     4,     5,     1,     3,     3,     1,     3,     4,
       3,     5,     6,     2,     1,     3,     4,     5,     4,     3,
       5,     6,     7,     2,     4,     5,     7,     4,     6,     2,
       4,     5,     7,     4,     6,     0,     1,     2,     2,     1,
       1,     3,     2,     3,     0,     2,     3,     1,     3,     4,
       3,     4,     1,     3,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     1,     3,     2,     3,     5,     4,
       3,     2,     0,     1,     3,     4,     4,     5,     1,     1,
       1,     1,     1,     2,     3,     3,     4,     1,     3,     4,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     2,     1,     1,     2,     2,     1,     2,     0,     1,
       1,     1,     0,     1,     0,     1
  };




#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   174,   174,   175,   176,   177,   178,   186,   189,   192,
     201,   204,   206,   209,   212,   217,   220,   226,   229,   232,
     264,   266,   271,   272,   276,   281,   282,   286,   287,   288,
     292,   301,   306,   308,   313,   315,   317,   322,   324,   329,
     330,   338,   339,   343,   346,   349,   352,   355,   365,   366,
     378,   379,   383,   384,   392,   392,   395,   397,   402,   403,
     407,   408,   409,   410,   414,   415,   416,   421,   426,   434,
     435,   444,   445,   446,   450,   451,   452,   453,   454,   455,
     456,   457,   461,   462,   466,   468,   473,   476,   479,   483,
     486,   489,   492,   495,   498,   508,   511,   516,   522,   537,
     544,   551,   560,   567,   574,   584,   590,   596,   605,   611,
     617,   626,   631,   636,   641,   646,   654,   659,   665,   670,
     675,   688,   691,   694,   697,   700,   703,   710,   718,   721,
     724,   727,   730,   733,   739,   740,   741,   742,   744,   747,
     750,   753,   756,   757,   759,   761,   764,   767,   770,   776,
     779,   782,   785,   793,   794,   798,   806,   809,   814,   816,
     821,   823,   825,   827,   829,   831,   833,   846,   848,   853,
     856,   859,   864,   870,   872,   874,   876,   878,   880,   882,
     884,   886,   888,   893,   894,   895,   897,   899,   904,   913,
     915,   917,   919,   921,   926,   938,   939,   944,   945,   946,
     950,   951,   954,   956,   959,   960,   962,   971,   973,   975,
     977,   979,   984,   986,   990,   998,   999,  1000,  1001,  1002,
    1003,  1004,  1005,  1009,  1011,  1013,  1015,  1017,  1019,  1021,
    1029,  1031,  1036,  1038,  1040,  1045,  1050,  1052,  1061,  1062,
    1063,  1071,  1072,  1077,  1079,  1081,  1083,  1087,  1089,  1091,
    1102,  1104,  1107,  1108,  1112,  1113,  1117,  1118,  1122,  1123,
    1124,  1125,  1134,  1135,  1136,  1137,  1141,  1143,  1148,  1150,
    1155,  1157,  1162,  1163,  1164,  1165,  1166,  1167,  1171,  1172,
    1173,  1174,  1186,  1186,  1187,  1187
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
#line 5739 "langutils/sc_parser/src/sc_grammar_parser.cpp"

#line 1189 "langutils/sc_parser/src/sc_grammar.y"

