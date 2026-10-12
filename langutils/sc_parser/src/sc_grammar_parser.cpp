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


#include "sc_lexer/lexer.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/nodes.hpp"

#include "sc_grammar_parser.hpp"
#include "sc_grammar_impl.hpp"
#include "parser_context.hpp"


using namespace sc::ast;

template<typename... REJECTS>
auto create_error(sc::ast::parser::ParserContext& cxt, sc::lex::SourceCodeRange loc, REJECTS...rejects) {
	const auto orphans = cxt.graph.orphans();
	auto er = cxt.create(Error{}, loc);
	for(auto o : orphans){
		if (!((*o == *rejects) || ...))
			cxt.graph.append(er, sc::ast::AnyIndex{*o});
	}
	return er;
}


#line 65 "langutils/sc_parser/src/sc_grammar_parser.cpp"


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
#line 165 "langutils/sc_parser/src/sc_grammar_parser.cpp"

/// Build a parser object.
parser::parser(ParserContext& cxt_yyarg)
#if YYDEBUG
    :
    yydebug_(false),
    yycdebug_(&std::cerr),
#else
    :
#endif
    cxt(cxt_yyarg) {
}

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

      case symbol_kind::S_129_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.copy< AccidentalIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_adverb: // adverb
        value.copy< AdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_93_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_95_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
          value.copy<AnyExprIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_method: // method
        value.copy< AnyMethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_112_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.copy<AnyPossiblyLiteralIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_104_arguments_entries: // arguments.entries
          value.copy<ArgumentEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_105_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_107_arguments_paren: // arguments.paren
      case symbol_kind::S_108_arguments_maybe_paren: // arguments.maybe_paren
          value.copy<ArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_113_literal_array_contents: // literal.array.contents
      case symbol_kind::S_117_literal_array: // literal.array
          value.copy<ArrayIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_76_members_opt: // members.opt
          value.copy<BigMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_86_block_contents: // block.contents
          value.copy<BlockContentsListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_block: // block
        value.copy< BlockIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_87_block_contents_item: // block.contents.item
          value.copy<BlockItemIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_84_block_opt_list: // block.opt_list
      case symbol_kind::S_85_block_list: // block.list
          value.copy<BlockListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_boolean: // boolean
        value.copy< BooleanLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_71_class_extension: // class.extension
          value.copy<ClassExtensionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_class: // class
        value.copy< ClassIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_go: // go
        value.copy< ClassListOrExprListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_67_classOrExtList_item: // classOrExtList.item
          value.copy<ClassOrExtensionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_66_classOrExtList_list: // classOrExtList.list
          value.copy<ClassOrExtensionListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_74_members_entry: // members.entry
          value.copy<DeclareAnyMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_109_variable_declarations_list_item: // variable_declarations.list.item
          value.copy<DeclareAnyVariableIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_100_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_101_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_103_argument_declarations_opt: // argument_declarations.opt
          value.copy<DeclareArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_72_members_entry_item: // members.entry.item
          value.copy<DeclareMemberIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_73_members_entry_list: // members.entry.list
          value.copy<DeclareMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_110_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.copy< DeclareVariableListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_114_literal_dictionary_entry: // literal.dictionary.entry
          value.copy<DictionaryEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_115_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_116_literal_dictionary: // literal.dictionary
          value.copy<DictionaryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_127_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_128_float_raw: // float.raw
          value.copy<FloatLitIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_float: // float
        value.copy< FloatProducingIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_generator: // generator
          value.copy<GeneratorAnyIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_99_generator_qual: // generator.qual
          value.copy<GeneratorQualAnyIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_98_generator_qual_list: // generator.qual.list
          value.copy<GeneratorQualListIndex>(YY_MOVE(that.value));
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

      case symbol_kind::S_78_method_base: // method.base
          value.copy<MethodIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_80_method_list: // method.list
      case symbol_kind::S_81_method_list_opt: // method.list.opt
          value.copy<MethodListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_120_name_list: // name.list
          value.copy<NameListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_name: // name
        value.copy< NamedIdentifierIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_nil: // nil
        value.copy< NilLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_accessor: // accessor
          value.copy<RWAccessorIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_region: // region
        value.copy< RegionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_77_method_name: // method.name
      case symbol_kind::S_89_binary_op_raw: // binary_op.raw
      case symbol_kind::S_90_binary_op_no_adverb: // binary_op.no_adverb
          value.copy<SelectorIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_binary_op: // binary_op
        value.copy< SelectorMaybeAdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_string: // string
        value.copy< StringLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_symbol: // symbol
        value.copy< SymbolLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_64_region_item: // region.item
          value.copy<error_index<AnyExprIndex>>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_69_class_super_opt: // class.super.opt
          value.copy<maybe<ClassNameIdentifierIndex>>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_70_class_slot_opt: // class.slot.opt
          value.copy<maybe<NamedIdentifierIndex>>(YY_MOVE(that.value));
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

      case symbol_kind::S_129_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.move< AccidentalIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_adverb: // adverb
        value.move< AdverbIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_93_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_95_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
          value.move<AnyExprIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_method: // method
        value.move< AnyMethodIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_112_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.move<AnyPossiblyLiteralIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_104_arguments_entries: // arguments.entries
          value.move<ArgumentEntryIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_105_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_107_arguments_paren: // arguments.paren
      case symbol_kind::S_108_arguments_maybe_paren: // arguments.maybe_paren
          value.move<ArgumentListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_113_literal_array_contents: // literal.array.contents
      case symbol_kind::S_117_literal_array: // literal.array
          value.move<ArrayIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_76_members_opt: // members.opt
          value.move<BigMemberListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_86_block_contents: // block.contents
          value.move<BlockContentsListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_block: // block
        value.move< BlockIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_87_block_contents_item: // block.contents.item
          value.move<BlockItemIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_84_block_opt_list: // block.opt_list
      case symbol_kind::S_85_block_list: // block.list
          value.move<BlockListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_boolean: // boolean
        value.move< BooleanLitIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_71_class_extension: // class.extension
          value.move<ClassExtensionIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_class: // class
        value.move< ClassIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_go: // go
        value.move< ClassListOrExprListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_67_classOrExtList_item: // classOrExtList.item
          value.move<ClassOrExtensionIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_66_classOrExtList_list: // classOrExtList.list
          value.move<ClassOrExtensionListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_74_members_entry: // members.entry
          value.move<DeclareAnyMemberListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_109_variable_declarations_list_item: // variable_declarations.list.item
          value.move<DeclareAnyVariableIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_100_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_101_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_103_argument_declarations_opt: // argument_declarations.opt
          value.move<DeclareArgumentListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_72_members_entry_item: // members.entry.item
          value.move<DeclareMemberIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_73_members_entry_list: // members.entry.list
          value.move<DeclareMemberListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_110_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.move< DeclareVariableListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_114_literal_dictionary_entry: // literal.dictionary.entry
          value.move<DictionaryEntryIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_115_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_116_literal_dictionary: // literal.dictionary
          value.move<DictionaryIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_127_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_128_float_raw: // float.raw
          value.move<FloatLitIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_float: // float
        value.move< FloatProducingIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_generator: // generator
          value.move<GeneratorAnyIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_99_generator_qual: // generator.qual
          value.move<GeneratorQualAnyIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_98_generator_qual_list: // generator.qual.list
          value.move<GeneratorQualListIndex>(YY_MOVE(s.value));
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

      case symbol_kind::S_78_method_base: // method.base
          value.move<MethodIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_80_method_list: // method.list
      case symbol_kind::S_81_method_list_opt: // method.list.opt
          value.move<MethodListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_120_name_list: // name.list
          value.move<NameListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_name: // name
        value.move< NamedIdentifierIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_nil: // nil
        value.move< NilLitIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_accessor: // accessor
          value.move<RWAccessorIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_region: // region
        value.move< RegionListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_77_method_name: // method.name
      case symbol_kind::S_89_binary_op_raw: // binary_op.raw
      case symbol_kind::S_90_binary_op_no_adverb: // binary_op.no_adverb
          value.move<SelectorIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_binary_op: // binary_op
        value.move< SelectorMaybeAdverbIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_string: // string
        value.move< StringLitIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_symbol: // symbol
        value.move< SymbolLitIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_64_region_item: // region.item
          value.move<error_index<AnyExprIndex>>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_69_class_super_opt: // class.super.opt
          value.move<maybe<ClassNameIdentifierIndex>>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_70_class_slot_opt: // class.slot.opt
          value.move<maybe<NamedIdentifierIndex>>(YY_MOVE(s.value));
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

      case symbol_kind::S_129_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.YY_MOVE_OR_COPY< AccidentalIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_adverb: // adverb
        value.YY_MOVE_OR_COPY< AdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_93_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_95_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
          value.YY_MOVE_OR_COPY<AnyExprIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_method: // method
        value.YY_MOVE_OR_COPY< AnyMethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_112_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.YY_MOVE_OR_COPY<AnyPossiblyLiteralIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_104_arguments_entries: // arguments.entries
          value.YY_MOVE_OR_COPY<ArgumentEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_105_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_107_arguments_paren: // arguments.paren
      case symbol_kind::S_108_arguments_maybe_paren: // arguments.maybe_paren
          value.YY_MOVE_OR_COPY<ArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_113_literal_array_contents: // literal.array.contents
      case symbol_kind::S_117_literal_array: // literal.array
          value.YY_MOVE_OR_COPY<ArrayIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_76_members_opt: // members.opt
          value.YY_MOVE_OR_COPY<BigMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_86_block_contents: // block.contents
          value.YY_MOVE_OR_COPY<BlockContentsListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_block: // block
        value.YY_MOVE_OR_COPY< BlockIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_87_block_contents_item: // block.contents.item
          value.YY_MOVE_OR_COPY<BlockItemIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_84_block_opt_list: // block.opt_list
      case symbol_kind::S_85_block_list: // block.list
          value.YY_MOVE_OR_COPY<BlockListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_boolean: // boolean
        value.YY_MOVE_OR_COPY< BooleanLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_71_class_extension: // class.extension
          value.YY_MOVE_OR_COPY<ClassExtensionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_class: // class
        value.YY_MOVE_OR_COPY< ClassIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_go: // go
        value.YY_MOVE_OR_COPY< ClassListOrExprListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_67_classOrExtList_item: // classOrExtList.item
          value.YY_MOVE_OR_COPY<ClassOrExtensionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_66_classOrExtList_list: // classOrExtList.list
          value.YY_MOVE_OR_COPY<ClassOrExtensionListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_74_members_entry: // members.entry
          value.YY_MOVE_OR_COPY<DeclareAnyMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_109_variable_declarations_list_item: // variable_declarations.list.item
          value.YY_MOVE_OR_COPY<DeclareAnyVariableIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_100_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_101_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_103_argument_declarations_opt: // argument_declarations.opt
          value.YY_MOVE_OR_COPY<DeclareArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_72_members_entry_item: // members.entry.item
          value.YY_MOVE_OR_COPY<DeclareMemberIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_73_members_entry_list: // members.entry.list
          value.YY_MOVE_OR_COPY<DeclareMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_110_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.YY_MOVE_OR_COPY< DeclareVariableListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_114_literal_dictionary_entry: // literal.dictionary.entry
          value.YY_MOVE_OR_COPY<DictionaryEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_115_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_116_literal_dictionary: // literal.dictionary
          value.YY_MOVE_OR_COPY<DictionaryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_127_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_128_float_raw: // float.raw
          value.YY_MOVE_OR_COPY<FloatLitIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_float: // float
        value.YY_MOVE_OR_COPY< FloatProducingIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_generator: // generator
          value.YY_MOVE_OR_COPY<GeneratorAnyIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_99_generator_qual: // generator.qual
          value.YY_MOVE_OR_COPY<GeneratorQualAnyIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_98_generator_qual_list: // generator.qual.list
          value.YY_MOVE_OR_COPY<GeneratorQualListIndex>(YY_MOVE(that.value));
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

      case symbol_kind::S_78_method_base: // method.base
          value.YY_MOVE_OR_COPY<MethodIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_80_method_list: // method.list
      case symbol_kind::S_81_method_list_opt: // method.list.opt
          value.YY_MOVE_OR_COPY<MethodListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_120_name_list: // name.list
          value.YY_MOVE_OR_COPY<NameListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_name: // name
        value.YY_MOVE_OR_COPY< NamedIdentifierIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_nil: // nil
        value.YY_MOVE_OR_COPY< NilLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_accessor: // accessor
          value.YY_MOVE_OR_COPY<RWAccessorIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_region: // region
        value.YY_MOVE_OR_COPY< RegionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_77_method_name: // method.name
      case symbol_kind::S_89_binary_op_raw: // binary_op.raw
      case symbol_kind::S_90_binary_op_no_adverb: // binary_op.no_adverb
          value.YY_MOVE_OR_COPY<SelectorIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_binary_op: // binary_op
        value.YY_MOVE_OR_COPY< SelectorMaybeAdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_string: // string
        value.YY_MOVE_OR_COPY< StringLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_symbol: // symbol
        value.YY_MOVE_OR_COPY< SymbolLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_64_region_item: // region.item
          value.YY_MOVE_OR_COPY<error_index<AnyExprIndex>>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_69_class_super_opt: // class.super.opt
          value.YY_MOVE_OR_COPY<maybe<ClassNameIdentifierIndex>>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_70_class_slot_opt: // class.slot.opt
          value.YY_MOVE_OR_COPY<maybe<NamedIdentifierIndex>>(YY_MOVE(that.value));
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

      case symbol_kind::S_129_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.move< AccidentalIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_adverb: // adverb
        value.move< AdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_93_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_95_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
          value.move<AnyExprIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_method: // method
        value.move< AnyMethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_112_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.move<AnyPossiblyLiteralIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_104_arguments_entries: // arguments.entries
          value.move<ArgumentEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_105_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_107_arguments_paren: // arguments.paren
      case symbol_kind::S_108_arguments_maybe_paren: // arguments.maybe_paren
          value.move<ArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_113_literal_array_contents: // literal.array.contents
      case symbol_kind::S_117_literal_array: // literal.array
          value.move<ArrayIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_76_members_opt: // members.opt
          value.move<BigMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_86_block_contents: // block.contents
          value.move<BlockContentsListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_block: // block
        value.move< BlockIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_87_block_contents_item: // block.contents.item
          value.move<BlockItemIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_84_block_opt_list: // block.opt_list
      case symbol_kind::S_85_block_list: // block.list
          value.move<BlockListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_boolean: // boolean
        value.move< BooleanLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_71_class_extension: // class.extension
          value.move<ClassExtensionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_class: // class
        value.move< ClassIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_go: // go
        value.move< ClassListOrExprListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_67_classOrExtList_item: // classOrExtList.item
          value.move<ClassOrExtensionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_66_classOrExtList_list: // classOrExtList.list
          value.move<ClassOrExtensionListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_74_members_entry: // members.entry
          value.move<DeclareAnyMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_109_variable_declarations_list_item: // variable_declarations.list.item
          value.move<DeclareAnyVariableIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_100_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_101_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_103_argument_declarations_opt: // argument_declarations.opt
          value.move<DeclareArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_72_members_entry_item: // members.entry.item
          value.move<DeclareMemberIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_73_members_entry_list: // members.entry.list
          value.move<DeclareMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_110_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.move< DeclareVariableListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_114_literal_dictionary_entry: // literal.dictionary.entry
          value.move<DictionaryEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_115_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_116_literal_dictionary: // literal.dictionary
          value.move<DictionaryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_127_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_128_float_raw: // float.raw
          value.move<FloatLitIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_float: // float
        value.move< FloatProducingIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_generator: // generator
          value.move<GeneratorAnyIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_99_generator_qual: // generator.qual
          value.move<GeneratorQualAnyIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_98_generator_qual_list: // generator.qual.list
          value.move<GeneratorQualListIndex>(YY_MOVE(that.value));
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

      case symbol_kind::S_78_method_base: // method.base
          value.move<MethodIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_80_method_list: // method.list
      case symbol_kind::S_81_method_list_opt: // method.list.opt
          value.move<MethodListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_120_name_list: // name.list
          value.move<NameListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_name: // name
        value.move< NamedIdentifierIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_nil: // nil
        value.move< NilLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_accessor: // accessor
          value.move<RWAccessorIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_region: // region
        value.move< RegionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_77_method_name: // method.name
      case symbol_kind::S_89_binary_op_raw: // binary_op.raw
      case symbol_kind::S_90_binary_op_no_adverb: // binary_op.no_adverb
          value.move<SelectorIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_binary_op: // binary_op
        value.move< SelectorMaybeAdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_string: // string
        value.move< StringLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_symbol: // symbol
        value.move< SymbolLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_64_region_item: // region.item
          value.move<error_index<AnyExprIndex>>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_69_class_super_opt: // class.super.opt
          value.move<maybe<ClassNameIdentifierIndex>>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_70_class_slot_opt: // class.slot.opt
          value.move<maybe<NamedIdentifierIndex>>(YY_MOVE(that.value));
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

      case symbol_kind::S_129_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.copy< AccidentalIndex > (that.value);
        break;

      case symbol_kind::S_adverb: // adverb
        value.copy< AdverbIndex > (that.value);
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_93_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_95_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
          value.copy<AnyExprIndex>(that.value);
          break;

      case symbol_kind::S_method: // method
        value.copy< AnyMethodIndex > (that.value);
        break;

      case symbol_kind::S_112_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.copy<AnyPossiblyLiteralIndex>(that.value);
          break;

      case symbol_kind::S_104_arguments_entries: // arguments.entries
          value.copy<ArgumentEntryIndex>(that.value);
          break;

      case symbol_kind::S_105_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_107_arguments_paren: // arguments.paren
      case symbol_kind::S_108_arguments_maybe_paren: // arguments.maybe_paren
          value.copy<ArgumentListIndex>(that.value);
          break;

      case symbol_kind::S_113_literal_array_contents: // literal.array.contents
      case symbol_kind::S_117_literal_array: // literal.array
          value.copy<ArrayIndex>(that.value);
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_76_members_opt: // members.opt
          value.copy<BigMemberListIndex>(that.value);
          break;

      case symbol_kind::S_86_block_contents: // block.contents
          value.copy<BlockContentsListIndex>(that.value);
          break;

      case symbol_kind::S_block: // block
        value.copy< BlockIndex > (that.value);
        break;

      case symbol_kind::S_87_block_contents_item: // block.contents.item
          value.copy<BlockItemIndex>(that.value);
          break;

      case symbol_kind::S_84_block_opt_list: // block.opt_list
      case symbol_kind::S_85_block_list: // block.list
          value.copy<BlockListIndex>(that.value);
          break;

      case symbol_kind::S_boolean: // boolean
        value.copy< BooleanLitIndex > (that.value);
        break;

      case symbol_kind::S_71_class_extension: // class.extension
          value.copy<ClassExtensionIndex>(that.value);
          break;

      case symbol_kind::S_class: // class
        value.copy< ClassIndex > (that.value);
        break;

      case symbol_kind::S_go: // go
        value.copy< ClassListOrExprListIndex > (that.value);
        break;

      case symbol_kind::S_67_classOrExtList_item: // classOrExtList.item
          value.copy<ClassOrExtensionIndex>(that.value);
          break;

      case symbol_kind::S_66_classOrExtList_list: // classOrExtList.list
          value.copy<ClassOrExtensionListIndex>(that.value);
          break;

      case symbol_kind::S_74_members_entry: // members.entry
          value.copy<DeclareAnyMemberListIndex>(that.value);
          break;

      case symbol_kind::S_109_variable_declarations_list_item: // variable_declarations.list.item
          value.copy<DeclareAnyVariableIndex>(that.value);
          break;

      case symbol_kind::S_100_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_101_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_103_argument_declarations_opt: // argument_declarations.opt
          value.copy<DeclareArgumentListIndex>(that.value);
          break;

      case symbol_kind::S_72_members_entry_item: // members.entry.item
          value.copy<DeclareMemberIndex>(that.value);
          break;

      case symbol_kind::S_73_members_entry_list: // members.entry.list
          value.copy<DeclareMemberListIndex>(that.value);
          break;

      case symbol_kind::S_110_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.copy< DeclareVariableListIndex > (that.value);
        break;

      case symbol_kind::S_114_literal_dictionary_entry: // literal.dictionary.entry
          value.copy<DictionaryEntryIndex>(that.value);
          break;

      case symbol_kind::S_115_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_116_literal_dictionary: // literal.dictionary
          value.copy<DictionaryIndex>(that.value);
          break;

      case symbol_kind::S_127_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_128_float_raw: // float.raw
          value.copy<FloatLitIndex>(that.value);
          break;

      case symbol_kind::S_float: // float
        value.copy< FloatProducingIndex > (that.value);
        break;

      case symbol_kind::S_generator: // generator
          value.copy<GeneratorAnyIndex>(that.value);
          break;

      case symbol_kind::S_99_generator_qual: // generator.qual
          value.copy<GeneratorQualAnyIndex>(that.value);
          break;

      case symbol_kind::S_98_generator_qual_list: // generator.qual.list
          value.copy<GeneratorQualListIndex>(that.value);
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

      case symbol_kind::S_78_method_base: // method.base
          value.copy<MethodIndex>(that.value);
          break;

      case symbol_kind::S_80_method_list: // method.list
      case symbol_kind::S_81_method_list_opt: // method.list.opt
          value.copy<MethodListIndex>(that.value);
          break;

      case symbol_kind::S_120_name_list: // name.list
          value.copy<NameListIndex>(that.value);
          break;

      case symbol_kind::S_name: // name
        value.copy< NamedIdentifierIndex > (that.value);
        break;

      case symbol_kind::S_nil: // nil
        value.copy< NilLitIndex > (that.value);
        break;

      case symbol_kind::S_accessor: // accessor
          value.copy<RWAccessorIndex>(that.value);
          break;

      case symbol_kind::S_region: // region
        value.copy< RegionListIndex > (that.value);
        break;

      case symbol_kind::S_77_method_name: // method.name
      case symbol_kind::S_89_binary_op_raw: // binary_op.raw
      case symbol_kind::S_90_binary_op_no_adverb: // binary_op.no_adverb
          value.copy<SelectorIndex>(that.value);
          break;

      case symbol_kind::S_binary_op: // binary_op
        value.copy< SelectorMaybeAdverbIndex > (that.value);
        break;

      case symbol_kind::S_string: // string
        value.copy< StringLitIndex > (that.value);
        break;

      case symbol_kind::S_symbol: // symbol
        value.copy< SymbolLitIndex > (that.value);
        break;

      case symbol_kind::S_64_region_item: // region.item
          value.copy<error_index<AnyExprIndex>>(that.value);
          break;

      case symbol_kind::S_69_class_super_opt: // class.super.opt
          value.copy<maybe<ClassNameIdentifierIndex>>(that.value);
          break;

      case symbol_kind::S_70_class_slot_opt: // class.slot.opt
          value.copy<maybe<NamedIdentifierIndex>>(that.value);
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

      case symbol_kind::S_129_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.move< AccidentalIndex > (that.value);
        break;

      case symbol_kind::S_adverb: // adverb
        value.move< AdverbIndex > (that.value);
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_93_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_95_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
          value.move<AnyExprIndex>(that.value);
          break;

      case symbol_kind::S_method: // method
        value.move< AnyMethodIndex > (that.value);
        break;

      case symbol_kind::S_112_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.move<AnyPossiblyLiteralIndex>(that.value);
          break;

      case symbol_kind::S_104_arguments_entries: // arguments.entries
          value.move<ArgumentEntryIndex>(that.value);
          break;

      case symbol_kind::S_105_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_107_arguments_paren: // arguments.paren
      case symbol_kind::S_108_arguments_maybe_paren: // arguments.maybe_paren
          value.move<ArgumentListIndex>(that.value);
          break;

      case symbol_kind::S_113_literal_array_contents: // literal.array.contents
      case symbol_kind::S_117_literal_array: // literal.array
          value.move<ArrayIndex>(that.value);
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_76_members_opt: // members.opt
          value.move<BigMemberListIndex>(that.value);
          break;

      case symbol_kind::S_86_block_contents: // block.contents
          value.move<BlockContentsListIndex>(that.value);
          break;

      case symbol_kind::S_block: // block
        value.move< BlockIndex > (that.value);
        break;

      case symbol_kind::S_87_block_contents_item: // block.contents.item
          value.move<BlockItemIndex>(that.value);
          break;

      case symbol_kind::S_84_block_opt_list: // block.opt_list
      case symbol_kind::S_85_block_list: // block.list
          value.move<BlockListIndex>(that.value);
          break;

      case symbol_kind::S_boolean: // boolean
        value.move< BooleanLitIndex > (that.value);
        break;

      case symbol_kind::S_71_class_extension: // class.extension
          value.move<ClassExtensionIndex>(that.value);
          break;

      case symbol_kind::S_class: // class
        value.move< ClassIndex > (that.value);
        break;

      case symbol_kind::S_go: // go
        value.move< ClassListOrExprListIndex > (that.value);
        break;

      case symbol_kind::S_67_classOrExtList_item: // classOrExtList.item
          value.move<ClassOrExtensionIndex>(that.value);
          break;

      case symbol_kind::S_66_classOrExtList_list: // classOrExtList.list
          value.move<ClassOrExtensionListIndex>(that.value);
          break;

      case symbol_kind::S_74_members_entry: // members.entry
          value.move<DeclareAnyMemberListIndex>(that.value);
          break;

      case symbol_kind::S_109_variable_declarations_list_item: // variable_declarations.list.item
          value.move<DeclareAnyVariableIndex>(that.value);
          break;

      case symbol_kind::S_100_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_101_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_103_argument_declarations_opt: // argument_declarations.opt
          value.move<DeclareArgumentListIndex>(that.value);
          break;

      case symbol_kind::S_72_members_entry_item: // members.entry.item
          value.move<DeclareMemberIndex>(that.value);
          break;

      case symbol_kind::S_73_members_entry_list: // members.entry.list
          value.move<DeclareMemberListIndex>(that.value);
          break;

      case symbol_kind::S_110_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.move< DeclareVariableListIndex > (that.value);
        break;

      case symbol_kind::S_114_literal_dictionary_entry: // literal.dictionary.entry
          value.move<DictionaryEntryIndex>(that.value);
          break;

      case symbol_kind::S_115_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_116_literal_dictionary: // literal.dictionary
          value.move<DictionaryIndex>(that.value);
          break;

      case symbol_kind::S_127_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_128_float_raw: // float.raw
          value.move<FloatLitIndex>(that.value);
          break;

      case symbol_kind::S_float: // float
        value.move< FloatProducingIndex > (that.value);
        break;

      case symbol_kind::S_generator: // generator
          value.move<GeneratorAnyIndex>(that.value);
          break;

      case symbol_kind::S_99_generator_qual: // generator.qual
          value.move<GeneratorQualAnyIndex>(that.value);
          break;

      case symbol_kind::S_98_generator_qual_list: // generator.qual.list
          value.move<GeneratorQualListIndex>(that.value);
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

      case symbol_kind::S_78_method_base: // method.base
          value.move<MethodIndex>(that.value);
          break;

      case symbol_kind::S_80_method_list: // method.list
      case symbol_kind::S_81_method_list_opt: // method.list.opt
          value.move<MethodListIndex>(that.value);
          break;

      case symbol_kind::S_120_name_list: // name.list
          value.move<NameListIndex>(that.value);
          break;

      case symbol_kind::S_name: // name
        value.move< NamedIdentifierIndex > (that.value);
        break;

      case symbol_kind::S_nil: // nil
        value.move< NilLitIndex > (that.value);
        break;

      case symbol_kind::S_accessor: // accessor
          value.move<RWAccessorIndex>(that.value);
          break;

      case symbol_kind::S_region: // region
        value.move< RegionListIndex > (that.value);
        break;

      case symbol_kind::S_77_method_name: // method.name
      case symbol_kind::S_89_binary_op_raw: // binary_op.raw
      case symbol_kind::S_90_binary_op_no_adverb: // binary_op.no_adverb
          value.move<SelectorIndex>(that.value);
          break;

      case symbol_kind::S_binary_op: // binary_op
        value.move< SelectorMaybeAdverbIndex > (that.value);
        break;

      case symbol_kind::S_string: // string
        value.move< StringLitIndex > (that.value);
        break;

      case symbol_kind::S_symbol: // symbol
        value.move< SymbolLitIndex > (that.value);
        break;

      case symbol_kind::S_64_region_item: // region.item
          value.move<error_index<AnyExprIndex>>(that.value);
          break;

      case symbol_kind::S_69_class_super_opt: // class.super.opt
          value.move<maybe<ClassNameIdentifierIndex>>(that.value);
          break;

      case symbol_kind::S_70_class_slot_opt: // class.slot.opt
          value.move<maybe<NamedIdentifierIndex>>(that.value);
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

      case symbol_kind::S_129_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        yylhs.value.emplace< AccidentalIndex > ();
        break;

      case symbol_kind::S_adverb: // adverb
        yylhs.value.emplace< AdverbIndex > ();
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_93_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_95_expr_or_seq_base: // expr_or_seq.base
      case symbol_kind::S_expr_or_seq: // expr_or_seq
          yylhs.value.emplace<AnyExprIndex>();
          break;

      case symbol_kind::S_method: // method
        yylhs.value.emplace< AnyMethodIndex > ();
        break;

      case symbol_kind::S_112_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          yylhs.value.emplace<AnyPossiblyLiteralIndex>();
          break;

      case symbol_kind::S_104_arguments_entries: // arguments.entries
          yylhs.value.emplace<ArgumentEntryIndex>();
          break;

      case symbol_kind::S_105_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_107_arguments_paren: // arguments.paren
      case symbol_kind::S_108_arguments_maybe_paren: // arguments.maybe_paren
          yylhs.value.emplace<ArgumentListIndex>();
          break;

      case symbol_kind::S_113_literal_array_contents: // literal.array.contents
      case symbol_kind::S_117_literal_array: // literal.array
          yylhs.value.emplace<ArrayIndex>();
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_76_members_opt: // members.opt
          yylhs.value.emplace<BigMemberListIndex>();
          break;

      case symbol_kind::S_86_block_contents: // block.contents
          yylhs.value.emplace<BlockContentsListIndex>();
          break;

      case symbol_kind::S_block: // block
        yylhs.value.emplace< BlockIndex > ();
        break;

      case symbol_kind::S_87_block_contents_item: // block.contents.item
          yylhs.value.emplace<BlockItemIndex>();
          break;

      case symbol_kind::S_84_block_opt_list: // block.opt_list
      case symbol_kind::S_85_block_list: // block.list
          yylhs.value.emplace<BlockListIndex>();
          break;

      case symbol_kind::S_boolean: // boolean
        yylhs.value.emplace< BooleanLitIndex > ();
        break;

      case symbol_kind::S_71_class_extension: // class.extension
          yylhs.value.emplace<ClassExtensionIndex>();
          break;

      case symbol_kind::S_class: // class
        yylhs.value.emplace< ClassIndex > ();
        break;

      case symbol_kind::S_go: // go
        yylhs.value.emplace< ClassListOrExprListIndex > ();
        break;

      case symbol_kind::S_67_classOrExtList_item: // classOrExtList.item
          yylhs.value.emplace<ClassOrExtensionIndex>();
          break;

      case symbol_kind::S_66_classOrExtList_list: // classOrExtList.list
          yylhs.value.emplace<ClassOrExtensionListIndex>();
          break;

      case symbol_kind::S_74_members_entry: // members.entry
          yylhs.value.emplace<DeclareAnyMemberListIndex>();
          break;

      case symbol_kind::S_109_variable_declarations_list_item: // variable_declarations.list.item
          yylhs.value.emplace<DeclareAnyVariableIndex>();
          break;

      case symbol_kind::S_100_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_101_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_103_argument_declarations_opt: // argument_declarations.opt
          yylhs.value.emplace<DeclareArgumentListIndex>();
          break;

      case symbol_kind::S_72_members_entry_item: // members.entry.item
          yylhs.value.emplace<DeclareMemberIndex>();
          break;

      case symbol_kind::S_73_members_entry_list: // members.entry.list
          yylhs.value.emplace<DeclareMemberListIndex>();
          break;

      case symbol_kind::S_110_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        yylhs.value.emplace< DeclareVariableListIndex > ();
        break;

      case symbol_kind::S_114_literal_dictionary_entry: // literal.dictionary.entry
          yylhs.value.emplace<DictionaryEntryIndex>();
          break;

      case symbol_kind::S_115_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_116_literal_dictionary: // literal.dictionary
          yylhs.value.emplace<DictionaryIndex>();
          break;

      case symbol_kind::S_127_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_128_float_raw: // float.raw
          yylhs.value.emplace<FloatLitIndex>();
          break;

      case symbol_kind::S_float: // float
        yylhs.value.emplace< FloatProducingIndex > ();
        break;

      case symbol_kind::S_generator: // generator
          yylhs.value.emplace<GeneratorAnyIndex>();
          break;

      case symbol_kind::S_99_generator_qual: // generator.qual
          yylhs.value.emplace<GeneratorQualAnyIndex>();
          break;

      case symbol_kind::S_98_generator_qual_list: // generator.qual.list
          yylhs.value.emplace<GeneratorQualListIndex>();
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

      case symbol_kind::S_78_method_base: // method.base
          yylhs.value.emplace<MethodIndex>();
          break;

      case symbol_kind::S_80_method_list: // method.list
      case symbol_kind::S_81_method_list_opt: // method.list.opt
          yylhs.value.emplace<MethodListIndex>();
          break;

      case symbol_kind::S_120_name_list: // name.list
          yylhs.value.emplace<NameListIndex>();
          break;

      case symbol_kind::S_name: // name
        yylhs.value.emplace< NamedIdentifierIndex > ();
        break;

      case symbol_kind::S_nil: // nil
        yylhs.value.emplace< NilLitIndex > ();
        break;

      case symbol_kind::S_accessor: // accessor
          yylhs.value.emplace<RWAccessorIndex>();
          break;

      case symbol_kind::S_region: // region
        yylhs.value.emplace< RegionListIndex > ();
        break;

      case symbol_kind::S_77_method_name: // method.name
      case symbol_kind::S_89_binary_op_raw: // binary_op.raw
      case symbol_kind::S_90_binary_op_no_adverb: // binary_op.no_adverb
          yylhs.value.emplace<SelectorIndex>();
          break;

      case symbol_kind::S_binary_op: // binary_op
        yylhs.value.emplace< SelectorMaybeAdverbIndex > ();
        break;

      case symbol_kind::S_string: // string
        yylhs.value.emplace< StringLitIndex > ();
        break;

      case symbol_kind::S_symbol: // symbol
        yylhs.value.emplace< SymbolLitIndex > ();
        break;

      case symbol_kind::S_64_region_item: // region.item
          yylhs.value.emplace<error_index<AnyExprIndex>>();
          break;

      case symbol_kind::S_69_class_super_opt: // class.super.opt
          yylhs.value.emplace<maybe<ClassNameIdentifierIndex>>();
          break;

      case symbol_kind::S_70_class_slot_opt: // class.slot.opt
          yylhs.value.emplace<maybe<NamedIdentifierIndex>>();
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
#line 179 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassListOrExprListIndex>() =
                  cxt.graph.assign_root(yystack_[2].value.as<RegionListIndex>());
          }
#line 2579 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 3: // go: classOrExtList.list $end
#line 180 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassListOrExprListIndex>() =
                  cxt.graph.assign_root(yystack_[1].value.as<ClassOrExtensionListIndex>());
          }
#line 2585 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 4: // go: %empty
#line 181 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassListOrExprListIndex>() =
                  cxt.graph.assign_root(cxt.create(ClassOrExtensionList {}, yylhs.location));
          }
#line 2591 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 5: // region.item: expr
#line 190 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<error_index<AnyExprIndex>>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 2597 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 6: // region.item: OPENPAREN argument_declarations block.contents CLOSEPAREN
#line 192 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<error_index<AnyExprIndex>>() = cxt.create(ImmediatelyEvaluatedBlock {}, yylhs.location,
                                                                       yystack_[2].value.as<DeclareArgumentListIndex>(),
                                                                       yystack_[1].value.as<BlockContentsListIndex>());
          }
#line 2603 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 7: // region.item: OPENPAREN argument_declarations CLOSEPAREN
#line 194 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<error_index<AnyExprIndex>>() = cxt.create(
                  ImmediatelyEvaluatedBlock {}, yylhs.location, yystack_[1].value.as<DeclareArgumentListIndex>(),
                  cxt.create(BlockContentsList {}, yystack_[0].location));
          }
#line 2609 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 8: // region: INTERPRET expr
#line 200 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RegionListIndex>() =
                  cxt.create(RegionList {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 2615 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 9: // region: INTERPRET error
#line 202 "langutils/sc_parser/src/sc_grammar.y"
          {
              error_recovery::expr(cxt);
              yyclearin;
              yylhs.value.as<RegionListIndex>() =
                  cxt.create(RegionList {}, yylhs.location, create_error(cxt, yystack_[0].location));
          }
#line 2625 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 10: // region: INTERPRET OPENPAREN argument_declarations block.contents semicolon.opt CLOSEPAREN
#line 208 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto ieb = cxt.create(ImmediatelyEvaluatedBlock {}, yylhs.location,
                                          yystack_[3].value.as<DeclareArgumentListIndex>(),
                                          yystack_[2].value.as<BlockContentsListIndex>());
              yylhs.value.as<RegionListIndex>() = cxt.create(RegionList {}, yylhs.location, ieb);
          }
#line 2634 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 12: // region: INTERPRET OPENPAREN argument_declarations CLOSEPAREN
#line 215 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto ieb = cxt.create(ImmediatelyEvaluatedBlock {}, yylhs.location,
                                          yystack_[1].value.as<DeclareArgumentListIndex>(),
                                          cxt.create(BlockContentsList {}, yystack_[0].location));
              yylhs.value.as<RegionListIndex>() = cxt.create(RegionList {}, yylhs.location, ieb);
          }
#line 2643 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 13: // region: region SEMICOLON region.item
#line 221 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RegionListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<RegionListIndex>(), yylhs.location,
                                   yystack_[0].value.as<error_index<AnyExprIndex>>());
          }
#line 2649 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 14: // region: region REGION_SEPARATOR region.item
#line 224 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RegionListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<RegionListIndex>(), yylhs.location,
                                   yystack_[0].value.as<error_index<AnyExprIndex>>());
          }
#line 2655 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 15: // region: region error
#line 227 "langutils/sc_parser/src/sc_grammar.y"
          {
              if (yystack_[1].location.end.line_number != yystack_[0].location.begin.line_number) {
                  auto first_child = cxt.graph.edges(*yystack_[1].value.as<RegionListIndex>()).first_child.value();
                  auto last_child = cxt.graph.last_child(*yystack_[1].value.as<RegionListIndex>());
                  auto loc = cxt.graph.location(last_child ? Index { *last_child } : Index { first_child });
                  error_recovery::region_separator(cxt, loc);
                  cxt.region_recovery = sc::ast::parser::ParserContext::RegionRecovery::EmitRegionSeparator;
                  static_assert(std::is_same_v<decltype(yyerrstatus_), int>);
                  yyerrstatus_ = 0; // this is NOT in the api, but the only way to get errors to re-emit.
                  yyclearin;
                  yylhs.value.as<RegionListIndex>() = yystack_[1].value.as<RegionListIndex>();
              } else {
                  error_recovery::expr(cxt);
                  yylhs.value.as<RegionListIndex>() = cxt.graph.append(
                      yystack_[1].value.as<RegionListIndex>(),
                      create_error(cxt, yystack_[0].location, yystack_[1].value.as<RegionListIndex>()));
                  yyclearin;
              }
          }
#line 2677 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 16: // classOrExtList.list: classOrExtList.item
#line 259 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassOrExtensionListIndex>() =
                  cxt.create(ClassOrExtensionList {}, yylhs.location, yystack_[0].value.as<ClassOrExtensionIndex>());
          }
#line 2683 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 17: // classOrExtList.list: classOrExtList.list classOrExtList.item
#line 261 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassOrExtensionListIndex>() = cxt.graph.append(
                  yystack_[1].value.as<ClassOrExtensionListIndex>(), yystack_[0].value.as<ClassOrExtensionIndex>());
          }
#line 2689 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 18: // classOrExtList.item: class
#line 265 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassOrExtensionIndex>() = yystack_[0].value.as<ClassIndex>();
          }
#line 2695 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 19: // classOrExtList.item: class.extension
#line 266 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassOrExtensionIndex>() = yystack_[0].value.as<ClassExtensionIndex>();
          }
#line 2701 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 20: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY members.opt method.list.opt CLOSECURLY
#line 271 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassIndex>() =
                  cxt.create(Class {}, yylhs.location, cxt.create(ClassNameIdentifier {}, yystack_[6].location),
                             yystack_[5].value.as<maybe<NamedIdentifierIndex>>(),
                             yystack_[4].value.as<maybe<ClassNameIdentifierIndex>>(),
                             yystack_[2].value.as<BigMemberListIndex>(), yystack_[1].value.as<MethodListIndex>());
          }
#line 2707 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 21: // class.super.opt: %empty
#line 275 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<maybe<ClassNameIdentifierIndex>>() = cxt.create(Missing {}, yylhs.location);
          }
#line 2713 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 22: // class.super.opt: COLON CLASSNAME
#line 276 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<maybe<ClassNameIdentifierIndex>>() =
                  cxt.create(ClassNameIdentifier {}, yystack_[0].location);
          }
#line 2719 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 23: // class.slot.opt: %empty
#line 280 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<maybe<NamedIdentifierIndex>>() = cxt.create(Missing {}, yylhs.location);
          }
#line 2725 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 24: // class.slot.opt: OPENSQUARE name CLOSESQUARE
#line 281 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<maybe<NamedIdentifierIndex>>() = yystack_[1].value.as<NamedIdentifierIndex>();
          }
#line 2731 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 25: // class.extension: ADD CLASSNAME OPENCURLY method.list.opt CLOSECURLY
#line 286 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassExtensionIndex>() = cxt.create(
                  ClassExtension {}, yylhs.location, cxt.create(ClassNameIdentifier {}, yystack_[3].location),
                  yystack_[1].value.as<MethodListIndex>());
          }
#line 2737 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 26: // members.entry.item: accessor variable_declarations.list.item
#line 295 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareMemberIndex>() =
                  cxt.create(DeclareMember {}, yylhs.location, yystack_[1].value.as<RWAccessorIndex>(),
                             yystack_[0].value.as<DeclareAnyVariableIndex>());
          }
#line 2743 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 27: // members.entry.list: members.entry.item
#line 300 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareMemberListIndex>() =
                  cxt.create(DeclareMemberList {}, yylhs.location, yystack_[0].value.as<DeclareMemberIndex>());
          }
#line 2749 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 28: // members.entry.list: members.entry.list COMMA members.entry.item
#line 302 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareMemberListIndex>() = cxt.graph.append(
                  yystack_[2].value.as<DeclareMemberListIndex>(), yystack_[0].value.as<DeclareMemberIndex>());
          }
#line 2755 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 29: // members.entry: CLASSVAR members.entry.list
#line 307 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyMemberListIndex>() =
                  cxt.graph.cast<DeclareClassMemberList>(yystack_[0].value.as<DeclareMemberListIndex>());
          }
#line 2761 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 30: // members.entry: VAR members.entry.list
#line 309 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyMemberListIndex>() =
                  cxt.graph.cast<DeclareMemberList>(yystack_[0].value.as<DeclareMemberListIndex>());
          }
#line 2767 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 31: // members.entry: CONST members.entry.list
#line 311 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyMemberListIndex>() =
                  cxt.graph.cast<DeclareConstList>(yystack_[0].value.as<DeclareMemberListIndex>());
          }
#line 2773 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 32: // members: members.entry
#line 316 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BigMemberListIndex>() =
                  cxt.create(BigMemberList {}, yylhs.location, yystack_[0].value.as<DeclareAnyMemberListIndex>());
          }
#line 2779 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 33: // members: members SEMICOLON members.entry
#line 318 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BigMemberListIndex>() = cxt.graph.append(
                  yystack_[2].value.as<BigMemberListIndex>(), yystack_[0].value.as<DeclareAnyMemberListIndex>());
          }
#line 2785 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 34: // members.opt: %empty
#line 322 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BigMemberListIndex>() = cxt.create(BigMemberList {}, yylhs.location);
          }
#line 2791 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 35: // members.opt: members semicolon.opt
#line 323 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BigMemberListIndex>() = yystack_[1].value.as<BigMemberListIndex>();
          }
#line 2797 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 36: // method.name: name
#line 331 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = yystack_[0].value.as<NamedIdentifierIndex>();
          }
#line 2803 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 37: // method.name: binary_op.raw
#line 332 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = yystack_[0].value.as<SelectorIndex>();
          }
#line 2809 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 38: // method.base: method.name OPENCURLY argument_declarations.opt block.contents semicolon.opt
                   // CLOSECURLY
#line 337 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodIndex>() = cxt.create(
                  Method {}, yylhs.location, yystack_[5].value.as<SelectorIndex>(),
                  yystack_[3].value.as<DeclareArgumentListIndex>(), cxt.create(Missing {}, yystack_[4].location),
                  yystack_[2].value.as<BlockContentsListIndex>());
          }
#line 2815 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 39: // method.base: method.name OPENCURLY argument_declarations.opt CLOSECURLY
#line 339 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodIndex>() = cxt.create(
                  Method {}, yylhs.location, yystack_[3].value.as<SelectorIndex>(),
                  yystack_[1].value.as<DeclareArgumentListIndex>(), cxt.create(Missing {}, yystack_[2].location),
                  cxt.create(BlockList {}, yystack_[0].location));
          }
#line 2821 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 40: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt
                   // block.contents semicolon.opt CLOSECURLY
#line 341 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodIndex>() =
                  cxt.create(Method {}, yylhs.location, yystack_[7].value.as<SelectorIndex>(),
                             yystack_[5].value.as<DeclareArgumentListIndex>(),
                             cxt.create(PrimitiveIdentifier {}, yystack_[4].location),
                             yystack_[2].value.as<BlockContentsListIndex>());
          }
#line 2827 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 41: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt
                   // CLOSECURLY
#line 343 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodIndex>() =
                  cxt.create(Method {}, yylhs.location, yystack_[5].value.as<SelectorIndex>(),
                             yystack_[3].value.as<DeclareArgumentListIndex>(),
                             cxt.create(PrimitiveIdentifier {}, yystack_[2].location),
                             cxt.create(BlockList {}, yystack_[1].location));
          }
#line 2833 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 42: // method: method.base
#line 347 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyMethodIndex>() = yystack_[0].value.as<MethodIndex>();
          }
#line 2839 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 43: // method: MULTIPLY method.base
#line 349 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyMethodIndex>() = cxt.graph.cast<ClassMethod>(yystack_[0].value.as<MethodIndex>());
          }
#line 2845 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 44: // method.list: method
#line 353 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodListIndex>() =
                  cxt.create(MethodList {}, yylhs.location, yystack_[0].value.as<AnyMethodIndex>());
          }
#line 2851 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 45: // method.list: method.list method
#line 354 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodListIndex>() = cxt.graph.append(
                  yystack_[1].value.as<MethodListIndex>(), yylhs.location, yystack_[0].value.as<AnyMethodIndex>());
          }
#line 2857 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 46: // method.list.opt: %empty
#line 358 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodListIndex>() = cxt.create(MethodList {}, yylhs.location);
          }
#line 2863 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 47: // method.list.opt: method.list
#line 359 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodListIndex>() = yystack_[0].value.as<MethodListIndex>();
          }
#line 2869 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 50: // block: block.open argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 371 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockIndex>() =
                  cxt.create(BlockNode {}, yylhs.location, yystack_[3].value.as<DeclareArgumentListIndex>(),
                             yystack_[2].value.as<BlockContentsListIndex>());
          }
#line 2875 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 51: // block: block.open argument_declarations.opt CLOSECURLY
#line 373 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockIndex>() =
                  cxt.create(BlockNode {}, yylhs.location, yystack_[1].value.as<DeclareArgumentListIndex>(),
                             cxt.create(BlockContentsList {}, yylhs.location));
          }
#line 2881 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 52: // block.opt_list: %empty
#line 377 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockListIndex>() = cxt.create(BlockList {}, yylhs.location);
          }
#line 2887 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 53: // block.opt_list: block.list
#line 378 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockListIndex>() = yystack_[0].value.as<BlockListIndex>();
          }
#line 2893 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 54: // block.list: block
#line 382 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockListIndex>() =
                  cxt.create(BlockList {}, yylhs.location, yystack_[0].value.as<BlockIndex>());
          }
#line 2899 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 55: // block.list: block.list block
#line 383 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockListIndex>() = cxt.graph.append(yystack_[1].value.as<BlockListIndex>(),
                                                                  yylhs.location, yystack_[0].value.as<BlockIndex>());
          }
#line 2905 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 56: // block.contents: block.contents.item
#line 387 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockContentsListIndex>() =
                  cxt.create(BlockContentsList {}, yylhs.location, yystack_[0].value.as<BlockItemIndex>());
          }
#line 2911 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 57: // block.contents: block.contents SEMICOLON block.contents.item
#line 388 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockContentsListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<BlockContentsListIndex>(), yylhs.location,
                                   yystack_[0].value.as<BlockItemIndex>());
          }
#line 2917 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 58: // block.contents: block.contents NONLOCALRETURN expr
#line 390 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto non_local =
                  cxt.create(NonLocalReturnExpr {}, yystack_[0].location, yystack_[0].value.as<AnyExprIndex>());
              yylhs.value.as<BlockContentsListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<BlockContentsListIndex>(), yylhs.location, non_local);
          }
#line 2926 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 59: // block.contents: block.contents SEMICOLON NONLOCALRETURN expr
#line 395 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto non_local =
                  cxt.create(NonLocalReturnExpr {}, yystack_[0].location, yystack_[0].value.as<AnyExprIndex>());
              yylhs.value.as<BlockContentsListIndex>() =
                  cxt.graph.append(yystack_[3].value.as<BlockContentsListIndex>(), yylhs.location, non_local);
          }
#line 2935 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 60: // block.contents: NONLOCALRETURN expr
#line 400 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto non_local =
                  cxt.create(NonLocalReturnExpr {}, yystack_[0].location, yystack_[0].value.as<AnyExprIndex>());
              yylhs.value.as<BlockContentsListIndex>() = cxt.create(BlockContentsList {}, yylhs.location, non_local);
          }
#line 2944 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 61: // block.contents.item: expr
#line 407 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockItemIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 2950 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 62: // block.contents.item: variable_declarations
#line 408 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockItemIndex>() = yystack_[0].value.as<DeclareVariableListIndex>();
          }
#line 2956 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 63: // adverb: DOT name
#line 417 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AdverbIndex>() = yystack_[0].value.as<NamedIdentifierIndex>();
          }
#line 2962 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 64: // adverb: DOT integer
#line 418 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AdverbIndex>() = yystack_[0].value.as<IntLitIndex>();
          }
#line 2968 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 65: // adverb: DOT OPENPAREN expr_or_seq CLOSEPAREN
#line 419 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AdverbIndex>() =
                  cxt.create(AdverbExprNode {}, yylhs.location, yystack_[1].value.as<AnyExprIndex>());
          }
#line 2974 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 66: // binary_op.raw: BINOP
#line 423 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 2980 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 67: // binary_op.raw: READWRITEVAR
#line 424 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 2986 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 68: // binary_op.raw: LESSTHAN
#line 425 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 2992 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 69: // binary_op.raw: GREATERTHAN
#line 426 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 2998 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 70: // binary_op.raw: MINUS
#line 427 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 3004 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 71: // binary_op.raw: MULTIPLY
#line 428 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 3010 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 72: // binary_op.raw: ADD
#line 429 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 3016 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 73: // binary_op.raw: PIPE
#line 430 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 3022 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 74: // binary_op.no_adverb: binary_op.raw
#line 434 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = yystack_[0].value.as<SelectorIndex>();
          }
#line 3028 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 75: // binary_op.no_adverb: KEYBINOP
#line 435 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { true }, yylhs.location);
          }
#line 3034 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 76: // binary_op: binary_op.no_adverb adverb
#line 440 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorMaybeAdverbIndex>() =
                  cxt.create(SelectorWAdverb {}, yylhs.location, yystack_[1].value.as<SelectorIndex>(),
                             yystack_[0].value.as<AdverbIndex>());
          }
#line 3040 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 77: // binary_op: binary_op.no_adverb
#line 442 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorMaybeAdverbIndex>() = yystack_[0].value.as<SelectorIndex>();
          }
#line 3046 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 78: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN arguments CLOSEPAREN block.opt_list
#line 447 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(PrefixMessage {}, yylhs.location, yystack_[5].value.as<SelectorIndex>(),
                             yystack_[2].value.as<ArgumentListIndex>(), yystack_[0].value.as<BlockListIndex>());
          }
#line 3052 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 79: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN CLOSEPAREN block.list
#line 450 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(PrefixMessage {}, yylhs.location, yystack_[4].value.as<SelectorIndex>(),
                             cxt.create(ArgumentList {}, yystack_[2].location), yystack_[0].value.as<BlockListIndex>());
          }
#line 3058 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 80: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN block.list
#line 453 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(PrefixMessage {}, yylhs.location, yystack_[2].value.as<SelectorIndex>(),
                             cxt.create(ArgumentList {}, yystack_[0].location), yystack_[0].value.as<BlockListIndex>());
          }
#line 3064 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 81: // msgsend: name OPENPAREN arguments CLOSEPAREN block.opt_list
#line 457 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(PrefixMessage {}, yylhs.location, yystack_[4].value.as<NamedIdentifierIndex>(),
                             yystack_[2].value.as<ArgumentListIndex>(), yystack_[0].value.as<BlockListIndex>());
          }
#line 3070 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 82: // msgsend: name OPENPAREN CLOSEPAREN block.list
#line 460 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(PrefixMessage {}, yylhs.location, yystack_[3].value.as<NamedIdentifierIndex>(),
                             cxt.create(ArgumentList {}, yystack_[2].location), yystack_[0].value.as<BlockListIndex>());
          }
#line 3076 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 83: // msgsend: name block.list
#line 463 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(PrefixMessage {}, yylhs.location, yystack_[1].value.as<NamedIdentifierIndex>(),
                             cxt.create(ArgumentList {}, yystack_[0].location), yystack_[0].value.as<BlockListIndex>());
          }
#line 3082 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 84: // msgsend: expr DOT name arguments.maybe_paren block.opt_list
#line 466 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(PostfixMessage {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<ArgumentListIndex>(),
                             yystack_[0].value.as<BlockListIndex>());
          }
#line 3088 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 85: // msgsend: expr DOT arguments.paren block.opt_list
#line 469 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(ValueMessage {}, yylhs.location, yystack_[3].value.as<AnyExprIndex>(),
                             yystack_[1].value.as<ArgumentListIndex>(), yystack_[0].value.as<BlockListIndex>());
          }
#line 3094 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 86: // msgsend: expr DOT OPENPAREN CLOSEPAREN block.opt_list
#line 472 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(ValueMessage {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             cxt.create(ArgumentList {}, yystack_[2].location), yystack_[0].value.as<BlockListIndex>());
          }
#line 3100 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 87: // msgsend: CLASSNAME OPENSQUARE literal.array.contents CLOSESQUARE
#line 482 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = cxt.create(CollectionNode {}, yylhs.location,
                                                          cxt.create(ClassNameIdentifier {}, yystack_[3].location),
                                                          yystack_[1].value.as<ArrayIndex>());
          }
#line 3106 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 88: // msgsend: CLASSNAME block.list
#line 485 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto class_name = cxt.create(ClassNameIdentifier {}, yystack_[1].location);
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(NewMessage {}, yylhs.location, class_name,
                             cxt.create(ArgumentList {}, yystack_[1].location), yystack_[0].value.as<BlockListIndex>());
          }
#line 3115 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 89: // msgsend: CLASSNAME arguments.paren block.opt_list
#line 490 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto class_name = cxt.create(ClassNameIdentifier {}, yystack_[2].location);
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(NewMessage {}, yylhs.location, class_name, yystack_[1].value.as<ArgumentListIndex>(),
                             yystack_[0].value.as<BlockListIndex>());
          }
#line 3124 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 90: // msgsend: CLASSNAME OPENPAREN CLOSEPAREN block.opt_list
#line 496 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto class_name = cxt.create(ClassNameIdentifier {}, yystack_[3].location);
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(NewMessage {}, yylhs.location, class_name,
                             cxt.create(ArgumentList {}, yystack_[3].location), yystack_[0].value.as<BlockListIndex>());
          }
#line 3133 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 91: // expr.base: literal
#line 507 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<AnyPossiblyLiteralIndex>();
          }
#line 3139 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 92: // expr.base: generator
#line 508 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<GeneratorAnyIndex>();
          }
#line 3145 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 93: // expr.base: name
#line 509 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<NamedIdentifierIndex>();
          }
#line 3151 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 94: // expr.base: msgsend
#line 511 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 3157 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 95: // expr.base: OPENPAREN expr_or_seq CLOSEPAREN
#line 513 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(ImmediatelyEvaluatedBlock {}, yylhs.location,
                             cxt.create(ArgumentList {}, yystack_[2].location), yystack_[1].value.as<AnyExprIndex>());
          }
#line 3163 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 96: // expr.base: TILDE name
#line 514 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(EnvIdentifierNode {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3169 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 97: // expr.base: expr.base OPENSQUARE arguments CLOSESQUARE
#line 517 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AtMessage {}, yylhs.location, yystack_[3].value.as<AnyExprIndex>(),
                             yystack_[1].value.as<ArgumentListIndex>());
          }
#line 3175 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 98: // expr.base: expr.base OPENSQUARE CLOSESQUARE
#line 520 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AtMessage {}, yylhs.location, yystack_[2].value.as<AnyExprIndex>(),
                             cxt.create(ArgumentList {}, yystack_[1].location));
          }
#line 3181 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 99: // expr.base: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 523 "langutils/sc_parser/src/sc_grammar.y"
          {
          }
#line 3187 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 100: // expr.base: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 525 "langutils/sc_parser/src/sc_grammar.y"
          {
          }
#line 3193 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 101: // expr.base: OPENPAREN expr_or_seq DOTDOT CLOSEPAREN
#line 529 "langutils/sc_parser/src/sc_grammar.y"
          {
          }
#line 3199 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 102: // expr.base: OPENPAREN expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 531 "langutils/sc_parser/src/sc_grammar.y"
          {
          }
#line 3205 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 103: // expr.base: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 533 "langutils/sc_parser/src/sc_grammar.y"
          {
          }
#line 3211 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 104: // expr.base: OPENPAREN DOTDOT expr_or_seq CLOSEPAREN
#line 535 "langutils/sc_parser/src/sc_grammar.y"
          {
          }
#line 3217 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 105: // expr.base: OPENPAREN COLON expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 541 "langutils/sc_parser/src/sc_grammar.y"
          {
          }
#line 3223 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 106: // expr.base: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 543 "langutils/sc_parser/src/sc_grammar.y"
          {
          }
#line 3229 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 107: // expr.base: OPENPAREN COLON DOTDOT expr_or_seq CLOSEPAREN
#line 545 "langutils/sc_parser/src/sc_grammar.y"
          {
          }
#line 3235 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 108: // expr.base: OPENPAREN COLON expr_or_seq DOTDOT CLOSEPAREN
#line 548 "langutils/sc_parser/src/sc_grammar.y"
          {
          }
#line 3241 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 109: // expr: expr.base
#line 555 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 3247 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 110: // expr: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 558 "langutils/sc_parser/src/sc_grammar.y"
          {
          }
#line 3253 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 111: // expr: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 561 "langutils/sc_parser/src/sc_grammar.y"
          {
          }
#line 3259 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 112: // expr: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 564 "langutils/sc_parser/src/sc_grammar.y"
          {
          }
#line 3265 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 113: // expr: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 567 "langutils/sc_parser/src/sc_grammar.y"
          {
          }
#line 3271 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 114: // expr: CLASSNAME
#line 571 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = cxt.create(ClassNameIdentifier {}, yylhs.location);
          }
#line 3277 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 115: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE
#line 574 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AtMessage {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             yystack_[1].value.as<ArgumentListIndex>());
          }
#line 3283 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 116: // expr: expr DOT OPENSQUARE CLOSESQUARE
#line 577 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AtMessage {}, yylhs.location, yystack_[3].value.as<AnyExprIndex>(),
                             cxt.create(ArgumentList {}, yystack_[1].location));
          }
#line 3289 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 117: // expr: BACKTICK expr
#line 579 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(ReferenceNode {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 3295 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 118: // expr: expr binary_op expr
#line 582 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(BinaryMessage {}, yylhs.location, yystack_[1].value.as<SelectorMaybeAdverbIndex>(),
                             yystack_[2].value.as<AnyExprIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3301 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 119: // expr: name EQUALSSIGN expr
#line 585 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentNode {}, yylhs.location, yystack_[2].value.as<NamedIdentifierIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3307 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 120: // expr: TILDE name EQUALSSIGN expr
#line 588 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentNode { AssignmentNode::Target::Environment }, yylhs.location,
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3313 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 121: // expr: expr DOT name EQUALSSIGN expr
#line 591 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(SetterNode {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3319 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 122: // expr: name OPENPAREN arguments CLOSEPAREN EQUALSSIGN expr
#line 594 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(SetterNode {}, yylhs.location, yystack_[3].value.as<ArgumentListIndex>(),
                             yystack_[5].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3325 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 123: // expr: HASH name.list comma.opt EQUALSSIGN expr
#line 597 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(VariablePack {}, yylhs.location, yystack_[3].value.as<NameListIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3331 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 124: // expr: HASH name.list comma.opt OPENPAREN expr_or_seq CLOSEPAREN
#line 599 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(VariablePack {}, yylhs.location, yystack_[4].value.as<NameListIndex>(),
                             yystack_[1].value.as<AnyExprIndex>());
          }
#line 3337 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 125: // expr: expr.base OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 605 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentAtNode {}, yylhs.location, yystack_[5].value.as<AnyExprIndex>(),
                             yystack_[3].value.as<ArgumentListIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3343 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 126: // expr: expr.base OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 608 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentAtNode {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             cxt.create(ArgumentList {}, yystack_[3].location), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3349 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 127: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 611 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentAtNode {}, yylhs.location, yystack_[6].value.as<AnyExprIndex>(),
                             yystack_[3].value.as<ArgumentListIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3355 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 128: // expr: expr DOT OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 614 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentAtNode {}, yylhs.location, yystack_[5].value.as<AnyExprIndex>(),
                             cxt.create(ArgumentList {}, yystack_[3].location), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3361 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 129: // expr_or_seq.base: expr
#line 618 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 3367 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 130: // expr_or_seq.base: expr_or_seq.base SEMICOLON expr
#line 619 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(ExprSeq {}, yylhs.location, yystack_[2].value.as<AnyExprIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3373 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 131: // expr_or_seq: expr_or_seq.base semicolon.opt
#line 623 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[1].value.as<AnyExprIndex>();
          }
#line 3379 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 132: // generator: OPENCURLY COLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 633 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorAnyIndex>() =
                  cxt.create(GeneratorNode {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             yystack_[2].value.as<GeneratorQualListIndex>());
          }
#line 3385 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 133: // generator: OPENCURLY SEMICOLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 639 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorAnyIndex>() =
                  cxt.create(GeneratorFakeNode {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             yystack_[2].value.as<GeneratorQualListIndex>());
          }
#line 3391 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 134: // generator.qual.list: generator.qual
#line 644 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualListIndex>() =
                  cxt.create(GeneratorQualList {}, yylhs.location, yystack_[0].value.as<GeneratorQualAnyIndex>());
          }
#line 3397 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 135: // generator.qual.list: generator.qual.list COMMA generator.qual
#line 646 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<GeneratorQualListIndex>(), yystack_[2].location,
                                   yystack_[0].value.as<GeneratorQualAnyIndex>());
          }
#line 3403 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 136: // generator.qual: name LEFTARROW expr_or_seq
#line 651 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualAnyIndex>() =
                  cxt.create(GeneratorQualAssign {}, yylhs.location, yystack_[2].value.as<NamedIdentifierIndex>(),
                             cxt.create(Missing {}, yystack_[1].location), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3409 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 137: // generator.qual: name name LEFTARROW expr_or_seq
#line 653 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualAnyIndex>() =
                  cxt.create(GeneratorQualAssign {}, yylhs.location, yystack_[3].value.as<NamedIdentifierIndex>(),
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3415 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 138: // generator.qual: VAR name EQUALSSIGN expr_or_seq
#line 655 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualAnyIndex>() =
                  cxt.create(GeneratorQualDeclare {}, yylhs.location, yystack_[2].value.as<NamedIdentifierIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3421 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 139: // generator.qual: VAR name OPENPAREN expr_or_seq CLOSEPAREN
#line 657 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualAnyIndex>() =
                  cxt.create(GeneratorQualDeclare {}, yylhs.location, yystack_[3].value.as<NamedIdentifierIndex>(),
                             yystack_[1].value.as<AnyExprIndex>());
          }
#line 3427 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 140: // generator.qual: expr_or_seq
#line 659 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualAnyIndex>() =
                  cxt.create(GeneratorQualIf {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 3433 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 141: // generator.qual: COLON COLON expr_or_seq
#line 661 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualAnyIndex>() =
                  cxt.create(GeneratorQualSideEffect {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 3439 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 142: // generator.qual: COLON WHILE expr_or_seq
#line 663 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualAnyIndex>() =
                  cxt.create(GeneratorQualGuard {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 3445 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 143: // argument_declarations.list: name
#line 676 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.create(DeclareArgumentList {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3451 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 144: // argument_declarations.list: name EQUALSSIGN literal
#line 678 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.create(DeclareArgumentList {}, yylhs.location,
                             cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                        yystack_[2].value.as<NamedIdentifierIndex>(),
                                        yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3457 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 145: // argument_declarations.list: name OPENPAREN expr_or_seq CLOSEPAREN
#line 680 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(
                  DeclareArgumentList {}, yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[3].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3463 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 146: // argument_declarations.list: name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 682 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(
                  DeclareArgumentList {}, yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[4].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3469 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 147: // argument_declarations.list: argument_declarations.list COMMA name
#line 684 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3475 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 148: // argument_declarations.list: argument_declarations.list COMMA name EQUALSSIGN literal
#line 686 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[4].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                              yystack_[2].value.as<NamedIdentifierIndex>(),
                                              yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3481 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 149: // argument_declarations.list: argument_declarations.list COMMA name OPENPAREN expr_or_seq
                    // CLOSEPAREN
#line 688 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.graph.append(
                  yystack_[5].value.as<DeclareArgumentListIndex>(), yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[3].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3487 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 150: // argument_declarations.list: argument_declarations.list COMMA name EQUALSSIGN OPENPAREN
                    // expr_or_seq CLOSEPAREN
#line 690 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.graph.append(
                  yystack_[6].value.as<DeclareArgumentListIndex>(), yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[4].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3493 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 151: // argument_declarations.pipelist: name literal
#line 695 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.create(DeclareArgumentList {}, yylhs.location,
                             cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                        yystack_[1].value.as<NamedIdentifierIndex>(),
                                        yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3499 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 152: // argument_declarations.pipelist: name
#line 697 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.create(DeclareArgumentList {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3505 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 153: // argument_declarations.pipelist: name EQUALSSIGN literal
#line 699 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.create(DeclareArgumentList {}, yylhs.location,
                             cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                        yystack_[2].value.as<NamedIdentifierIndex>(),
                                        yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3511 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 154: // argument_declarations.pipelist: name OPENPAREN expr_or_seq CLOSEPAREN
#line 701 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(
                  DeclareArgumentList {}, yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[3].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3517 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 155: // argument_declarations.pipelist: name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 703 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(
                  DeclareArgumentList {}, yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[4].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3523 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 156: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name literal
#line 705 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[3].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                              yystack_[1].value.as<NamedIdentifierIndex>(),
                                              yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3529 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 157: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name
#line 707 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3535 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 158: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN literal
#line 709 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[4].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                              yystack_[2].value.as<NamedIdentifierIndex>(),
                                              yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3541 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 159: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name OPENPAREN
                    // expr_or_seq CLOSEPAREN
#line 711 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.graph.append(
                  yystack_[5].value.as<DeclareArgumentListIndex>(), yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[3].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3547 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 160: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN
                    // OPENPAREN expr_or_seq CLOSEPAREN
#line 713 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.graph.append(
                  yystack_[6].value.as<DeclareArgumentListIndex>(), yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[4].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3553 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 161: // argument_declarations: ARG SEMICOLON
#line 717 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(DeclareArgumentList {}, yylhs.location);
          }
#line 3559 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 162: // argument_declarations: ARG argument_declarations.list comma.opt SEMICOLON
#line 718 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = yystack_[2].value.as<DeclareArgumentListIndex>();
          }
#line 3565 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 163: // argument_declarations: ARG argument_declarations.list ELLIPSIS name SEMICOLON
#line 720 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[3].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3571 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 164: // argument_declarations: ARG argument_declarations.list ELLIPSIS name COMMA name SEMICOLON
#line 722 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[5].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[3].location,
                                              yystack_[3].value.as<NamedIdentifierIndex>()),
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3577 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 165: // argument_declarations: ARG ELLIPSIS name SEMICOLON
#line 724 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto arg_list = cxt.create(DeclareArgumentList {}, yylhs.location);
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(arg_list, yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3586 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 166: // argument_declarations: ARG ELLIPSIS name COMMA name SEMICOLON
#line 729 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto arg_list = cxt.create(DeclareArgumentList {}, yylhs.location);
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(arg_list, yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[3].location,
                                              yystack_[3].value.as<NamedIdentifierIndex>()),
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3599 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 167: // argument_declarations: PIPE PIPE
#line 738 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(DeclareArgumentList {}, yylhs.location);
          }
#line 3605 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 168: // argument_declarations: PIPE argument_declarations.pipelist comma.opt PIPE
#line 740 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = yystack_[2].value.as<DeclareArgumentListIndex>();
          }
#line 3611 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 169: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name PIPE
#line 742 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[3].value.as<DeclareArgumentListIndex>(), yystack_[3].location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3617 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 170: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name COMMA name PIPE
#line 744 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[5].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[3].location,
                                              yystack_[3].value.as<NamedIdentifierIndex>()),
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3623 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 171: // argument_declarations: PIPE ELLIPSIS name PIPE
#line 746 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto arg_list = cxt.create(DeclareArgumentList {}, yylhs.location);
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(arg_list, yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3632 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 172: // argument_declarations: PIPE ELLIPSIS name COMMA name PIPE
#line 751 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto arg_list = cxt.create(DeclareArgumentList {}, yylhs.location);
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(arg_list, yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[3].location,
                                              yystack_[3].value.as<NamedIdentifierIndex>()),
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3645 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 173: // argument_declarations.opt: %empty
#line 762 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(DeclareArgumentList {}, yylhs.location);
          }
#line 3651 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 174: // argument_declarations.opt: argument_declarations
#line 763 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = yystack_[0].value.as<DeclareArgumentListIndex>();
          }
#line 3657 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 175: // arguments.entries: KEYBINOP expr_or_seq
#line 767 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentEntryIndex>() =
                  cxt.create(KwArgNode {}, yylhs.location,
                             cxt.create(SymbolNode { SymbolNode::Kind::KeyBinOp }, yystack_[1].location),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3663 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 176: // arguments.entries: MULTIPLY expr_or_seq
#line 768 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentEntryIndex>() =
                  cxt.create(VariadicArgNode {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 3669 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 177: // arguments.entries: expr_or_seq
#line 769 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentEntryIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 3675 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 178: // arguments.no_trailing: arguments.entries
#line 773 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() =
                  cxt.create(ArgumentList {}, yylhs.location, yystack_[0].value.as<ArgumentEntryIndex>());
          }
#line 3681 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 179: // arguments.no_trailing: arguments.no_trailing COMMA arguments.entries
#line 774 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<ArgumentListIndex>(), yylhs.location,
                                   yystack_[0].value.as<ArgumentEntryIndex>());
          }
#line 3687 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 180: // arguments: arguments.no_trailing comma.opt
#line 777 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = yystack_[1].value.as<ArgumentListIndex>();
          }
#line 3693 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 181: // arguments.paren: OPENPAREN arguments CLOSEPAREN
#line 779 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = yystack_[1].value.as<ArgumentListIndex>();
          }
#line 3699 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 182: // arguments.maybe_paren: %empty
#line 782 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = cxt.create(ArgumentList {}, yylhs.location);
          }
#line 3705 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 183: // arguments.maybe_paren: OPENPAREN CLOSEPAREN
#line 784 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = cxt.create(ArgumentList {}, yylhs.location);
          }
#line 3711 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 184: // arguments.maybe_paren: OPENPAREN arguments CLOSEPAREN
#line 786 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = yystack_[1].value.as<ArgumentListIndex>();
          }
#line 3717 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 185: // variable_declarations.list.item: name
#line 795 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() = yystack_[0].value.as<NamedIdentifierIndex>();
          }
#line 3723 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 186: // variable_declarations.list.item: name EQUALSSIGN expr
#line 797 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() =
                  cxt.create(DeclareVariableWithDefaultNode {}, yylhs.location,
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3729 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 187: // variable_declarations.list.item: name OPENPAREN expr_or_seq CLOSEPAREN
#line 799 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() =
                  cxt.create(DeclareVariableWithDefaultNode {}, yylhs.location,
                             yystack_[3].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>());
          }
#line 3735 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 188: // variable_declarations.list.item: HASH name.list comma.opt EQUALSSIGN expr
#line 801 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() =
                  cxt.create(VariablePack {}, yylhs.location, yystack_[3].value.as<NameListIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3741 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 189: // variable_declarations.list.item: HASH name.list comma.opt OPENPAREN expr_or_seq CLOSEPAREN
#line 803 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() =
                  cxt.create(VariablePack {}, yylhs.location, yystack_[4].value.as<NameListIndex>(),
                             yystack_[1].value.as<AnyExprIndex>());
          }
#line 3747 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 190: // variable_declarations.list: variable_declarations.list.item
#line 808 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareVariableListIndex>() =
                  cxt.create(DeclareVariableList {}, yylhs.location, yystack_[0].value.as<DeclareAnyVariableIndex>());
          }
#line 3753 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 191: // variable_declarations.list: variable_declarations.list COMMA variable_declarations.list.item
#line 810 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareVariableListIndex>() = cxt.graph.append(
                  yystack_[2].value.as<DeclareVariableListIndex>(), yystack_[0].value.as<DeclareAnyVariableIndex>());
          }
#line 3759 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 192: // variable_declarations: VAR variable_declarations.list
#line 813 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareVariableListIndex>() = yystack_[0].value.as<DeclareVariableListIndex>();
          }
#line 3765 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 193: // literal.terminal: symbol
#line 821 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<SymbolLitIndex>();
          }
#line 3771 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 194: // literal.terminal: string
#line 822 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<StringLitIndex>();
          }
#line 3777 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 195: // literal.terminal: integer
#line 823 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<IntLitIndex>();
          }
#line 3783 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 196: // literal.terminal: float
#line 824 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<FloatProducingIndex>();
          }
#line 3789 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 197: // literal.terminal: boolean
#line 825 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<BooleanLitIndex>();
          }
#line 3795 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 198: // literal.terminal: nil
#line 826 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<NilLitIndex>();
          }
#line 3801 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 199: // literal.terminal: ascii
#line 827 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<ASCIIIndex>();
          }
#line 3807 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 200: // literal.terminal: block
#line 828 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<BlockIndex>();
          }
#line 3813 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 201: // literal.array.contents: %empty
#line 833 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() = cxt.create(ArrayNode {}, yylhs.location);
          }
#line 3819 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 202: // literal.array.contents: expr_or_seq
#line 835 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.create(ArrayNode {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 3825 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 203: // literal.array.contents: expr_or_seq COLON expr_or_seq
#line 837 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.create(ArrayNode {}, yylhs.location, yystack_[2].value.as<AnyExprIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3831 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 204: // literal.array.contents: KEYBINOP expr_or_seq
#line 839 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.create(ArrayNode {}, yylhs.location,
                             cxt.create(SymbolNode { SymbolNode::Kind::KeyBinOp }, yystack_[1].location),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3837 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 205: // literal.array.contents: literal.array.contents COMMA expr_or_seq
#line 841 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() = cxt.graph.append(yystack_[2].value.as<ArrayIndex>(), yylhs.location,
                                                              yystack_[0].value.as<AnyExprIndex>());
          }
#line 3843 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 206: // literal.array.contents: literal.array.contents COMMA expr_or_seq COLON expr_or_seq
#line 843 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.graph.append(yystack_[4].value.as<ArrayIndex>(), yylhs.location,
                                   yystack_[2].value.as<AnyExprIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3849 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 207: // literal.array.contents: literal.array.contents COMMA KEYBINOP expr_or_seq
#line 845 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.graph.append(yystack_[3].value.as<ArrayIndex>(), yylhs.location,
                                   cxt.create(SymbolNode { SymbolNode::Kind::KeyBinOp }, yystack_[1].location),
                                   yystack_[0].value.as<AnyExprIndex>());
          }
#line 3855 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 208: // literal.dictionary.entry: expr COLON expr_or_seq
#line 853 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryEntryIndex>() =
                  cxt.create(DictionaryEntryNode {}, yylhs.location, yystack_[2].value.as<AnyExprIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3861 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 209: // literal.dictionary.entry: KEYBINOP expr_or_seq
#line 855 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryEntryIndex>() =
                  cxt.create(DictionaryEntryNode {}, yylhs.location,
                             cxt.create(SymbolNode { SymbolNode::Kind::KeyBinOp }, yystack_[1].location),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3867 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 210: // literal.dictionary.entries: %empty
#line 860 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryIndex>() = cxt.create(DictionaryNode {}, yylhs.location);
          }
#line 3873 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 211: // literal.dictionary.entries: literal.dictionary.entry
#line 862 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryIndex>() =
                  cxt.create(DictionaryNode {}, yylhs.location, yystack_[0].value.as<DictionaryEntryIndex>());
          }
#line 3879 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 212: // literal.dictionary.entries: literal.dictionary.entries COMMA literal.dictionary.entry
#line 864 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryIndex>() =
                  cxt.graph.append(yystack_[2].value.as<DictionaryIndex>(), yylhs.location,
                                   yystack_[0].value.as<DictionaryEntryIndex>());
          }
#line 3885 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 213: // literal.dictionary: OPENPAREN literal.dictionary.entries comma.opt CLOSEPAREN
#line 869 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.location(*yystack_[2].value.as<DictionaryIndex>()) = yylhs.location;
              yylhs.value.as<DictionaryIndex>() = yystack_[2].value.as<DictionaryIndex>();
          }
#line 3891 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 214: // literal.array: OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 874 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.location(*yystack_[2].value.as<ArrayIndex>()) = yylhs.location;
              yylhs.value.as<ArrayIndex>() = yystack_[2].value.as<ArrayIndex>();
          }
#line 3897 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 215: // literal.array: HASH OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 876 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.payload(yystack_[2].value.as<ArrayIndex>()).is_immutable = true;
              cxt.graph.location(*yystack_[2].value.as<ArrayIndex>()) = yylhs.location;
              yylhs.value.as<ArrayIndex>() = yystack_[2].value.as<ArrayIndex>();
          }
#line 3907 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 216: // literal: literal.terminal
#line 884 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<AnyPossiblyLiteralIndex>();
          }
#line 3913 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 217: // literal: literal.array
#line 885 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<ArrayIndex>();
          }
#line 3919 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 218: // literal: literal.dictionary
#line 886 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<DictionaryIndex>();
          }
#line 3925 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 219: // name: NAME
#line 893 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NamedIdentifierIndex>() = cxt.create(NamedIdentifier {}, yylhs.location);
          }
#line 3931 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 220: // name.list: name
#line 898 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NameListIndex>() =
                  cxt.create(NameList {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3937 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 221: // name.list: TILDE name
#line 900 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NameListIndex>() = cxt.create(
                  NameList {}, yylhs.location,
                  cxt.create(EnvIdentifierNode {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>()));
          }
#line 3943 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 222: // name.list: name.list COMMA name
#line 902 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NameListIndex>() = cxt.graph.append(yystack_[2].value.as<NameListIndex>(), yylhs.location,
                                                                 yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3949 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 223: // name.list: name.list COMMA TILDE name
#line 904 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NameListIndex>() = cxt.graph.append(
                  yystack_[3].value.as<NameListIndex>(), yylhs.location,
                  cxt.create(EnvIdentifierNode {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>()));
          }
#line 3955 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 224: // ascii: ASCII
#line 914 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ASCIIIndex>() = cxt.create(ASCIINode {}, yylhs.location);
          }
#line 3961 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 225: // nil: NIL
#line 916 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NilLitIndex>() = cxt.create(NilNode {}, yylhs.location);
          }
#line 3967 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 226: // boolean: TRUE
#line 919 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BooleanLitIndex>() = cxt.create(BooleanNode { true }, yylhs.location);
          }
#line 3973 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 227: // boolean: FALSE
#line 920 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BooleanLitIndex>() = cxt.create(BooleanNode { false }, yylhs.location);
          }
#line 3979 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 228: // symbol: SYMBOL_QUOTE
#line 924 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SymbolLitIndex>() = cxt.create(SymbolNode { SymbolNode::Kind::Quote }, yylhs.location);
          }
#line 3985 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 229: // symbol: SYMBOL_SLASH
#line 925 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SymbolLitIndex>() = cxt.create(SymbolNode { SymbolNode::Kind::Slash }, yylhs.location);
          }
#line 3991 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 230: // string: STRINGLINE
#line 929 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<StringLitIndex>() =
                  cxt.create(StringLineList {}, yylhs.location, cxt.create(StringLineNode {}, yylhs.location));
          }
#line 3997 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 231: // string: string STRINGLINE
#line 930 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<StringLitIndex>() = cxt.graph.append(yystack_[1].value.as<StringLitIndex>(),
                                                                  cxt.create(StringLineNode {}, yystack_[0].location));
          }
#line 4003 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 232: // integer: INTEGER
#line 934 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<IntLitIndex>() = cxt.create(IntNode {}, yylhs.location);
          }
#line 4009 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 233: // integer: INTEGER_RADIX
#line 935 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<IntLitIndex>() = cxt.create(IntNode { IntNode::Kind::Radix }, yylhs.location);
          }
#line 4015 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 234: // integer: HEXADECIMAL
#line 936 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<IntLitIndex>() = cxt.create(IntNode { IntNode::Kind::Hexadecimal }, yylhs.location);
          }
#line 4021 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 235: // integer: MINUS integer
#line 938 "langutils/sc_parser/src/sc_grammar.y"
          {
              // Reaches into the previous integer and changes its sign.
              cxt.graph.payload(yystack_[0].value.as<IntLitIndex>()).sign = IntNode::Sign::Negative;
              yylhs.value.as<IntLitIndex>() = yystack_[0].value.as<IntLitIndex>();
          }
#line 4031 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 236: // float.raw_unsigned: FLOAT
#line 946 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = cxt.create(FloatNode {}, yylhs.location);
          }
#line 4037 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 237: // float.raw_unsigned: FLOAT_RADIX
#line 947 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = cxt.create(FloatNode { FloatNode::Kind::Radix }, yylhs.location);
          }
#line 4043 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 238: // float.raw_unsigned: FLOAT_EXPONENT
#line 948 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = cxt.create(FloatNode { FloatNode::Kind::Exponent }, yylhs.location);
          }
#line 4049 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 239: // float.raw_unsigned: FLOAT_INF
#line 949 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = cxt.create(FloatNode { FloatNode::Kind::Inf }, yylhs.location);
          }
#line 4055 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 240: // float.raw: float.raw_unsigned
#line 954 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = yystack_[0].value.as<FloatLitIndex>();
          }
#line 4061 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 241: // float.raw: MINUS float.raw_unsigned
#line 956 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.payload(yystack_[0].value.as<FloatLitIndex>()).sign = FloatNode::Sign::Negative;
              yylhs.value.as<FloatLitIndex>() = yystack_[0].value.as<FloatLitIndex>();
          }
#line 4067 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 242: // accidental.unsigned: ACCIDENTAL_STEPS
#line 961 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AccidentalIndex>() =
                  cxt.create(AccidentalNode { AccidentalNode::Kind::Steps }, yylhs.location);
          }
#line 4073 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 243: // accidental.unsigned: ACCIDENTAL_CENTS
#line 963 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AccidentalIndex>() =
                  cxt.create(AccidentalNode { AccidentalNode::Kind::Cents }, yylhs.location);
          }
#line 4079 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 244: // accidental: accidental.unsigned
#line 968 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AccidentalIndex>() = yystack_[0].value.as<AccidentalIndex>();
          }
#line 4085 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 245: // accidental: MINUS accidental.unsigned
#line 970 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.payload(yystack_[0].value.as<AccidentalIndex>()).sign = AccidentalNode::Sign::Negative;
              yylhs.value.as<AccidentalIndex>() = yystack_[0].value.as<AccidentalIndex>();
          }
#line 4091 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 246: // float: float.raw
#line 974 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() = yystack_[0].value.as<FloatLitIndex>();
          }
#line 4097 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 247: // float: accidental
#line 975 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() = yystack_[0].value.as<AccidentalIndex>();
          }
#line 4103 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 248: // float: float.raw PI
#line 976 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() =
                  cxt.create(PiNode {}, yylhs.location, yystack_[1].value.as<FloatLitIndex>());
          }
#line 4109 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 249: // float: integer PI
#line 977 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() =
                  cxt.create(PiNode {}, yylhs.location, yystack_[1].value.as<IntLitIndex>());
          }
#line 4115 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 250: // float: PI
#line 978 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() =
                  cxt.create(PiNode {}, yylhs.location, cxt.create(Missing {}, yylhs.location));
          }
#line 4121 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 251: // float: MINUS PI
#line 979 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() =
                  cxt.create(PiNode { PiNode::Sign::Negative }, yylhs.location, cxt.create(Missing {}, yylhs.location));
          }
#line 4127 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 252: // accessor: %empty
#line 983 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RWAccessorIndex>() = cxt.create(RWAccessor { ReadWriteAccessor::Private }, yylhs.location);
          }
#line 4133 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 253: // accessor: LESSTHAN
#line 984 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RWAccessorIndex>() =
                  cxt.create(RWAccessor { ReadWriteAccessor::ExternallyReadable }, yylhs.location);
          }
#line 4139 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 254: // accessor: GREATERTHAN
#line 985 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RWAccessorIndex>() =
                  cxt.create(RWAccessor { ReadWriteAccessor::ExternallyWriteable }, yylhs.location);
          }
#line 4145 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 255: // accessor: READWRITEVAR
#line 987 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RWAccessorIndex>() = cxt.create(
                  RWAccessor {
                      combine<ReadWriteAccessor::ExternallyReadable, ReadWriteAccessor::ExternallyWriteable>() },
                  yylhs.location);
          }
#line 4155 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;


#line 4159 "langutils/sc_parser/src/sc_grammar_parser.cpp"

          default:
              break;
          }
      }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc) {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error(yyexc);
          YYERROR;
      }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT("-> $$ =", yylhs);
      yypop_(yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_(YY_NULLPTR, YY_MOVE(yylhs));
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
      static const char* const yy_sname[] = { "end of file",
                                              "error",
                                              "invalid token",
                                              "REGION_SEPARATOR",
                                              "OPENCURLY",
                                              "CLOSECURLY",
                                              "OPENSQUARE",
                                              "CLOSESQUARE",
                                              "OPENPAREN",
                                              "CLOSEPAREN",
                                              "SEMICOLON",
                                              "NONLOCALRETURN",
                                              "COMMA",
                                              "HASH",
                                              "TILDE",
                                              "NAME",
                                              "INTEGER",
                                              "INTEGER_RADIX",
                                              "HEXADECIMAL",
                                              "FLOAT",
                                              "FLOAT_RADIX",
                                              "FLOAT_EXPONENT",
                                              "FLOAT_INF",
                                              "ACCIDENTAL_STEPS",
                                              "ACCIDENTAL_CENTS",
                                              "SYMBOL_QUOTE",
                                              "SYMBOL_SLASH",
                                              "STRINGLINE",
                                              "ASCII",
                                              "PRIMITIVENAME",
                                              "CLASSNAME",
                                              "CURRYARG",
                                              "VAR",
                                              "ARG",
                                              "CLASSVAR",
                                              "CONST",
                                              "NIL",
                                              "TRUE",
                                              "FALSE",
                                              "PI",
                                              "ELLIPSIS",
                                              "DOTDOT",
                                              "BEGINCLOSEDFUNC",
                                              "BADTOKEN",
                                              "INTERPRET",
                                              "LEFTARROW",
                                              "WHILE",
                                              "LEXER_ERROR",
                                              "COLON",
                                              "EQUALSSIGN",
                                              "BINOP",
                                              "KEYBINOP",
                                              "MINUS",
                                              "LESSTHAN",
                                              "GREATERTHAN",
                                              "MULTIPLY",
                                              "ADD",
                                              "PIPE",
                                              "READWRITEVAR",
                                              "DOT",
                                              "BACKTICK",
                                              "UMINUS",
                                              "$accept",
                                              "go",
                                              "region.item",
                                              "region",
                                              "classOrExtList.list",
                                              "classOrExtList.item",
                                              "class",
                                              "class.super.opt",
                                              "class.slot.opt",
                                              "class.extension",
                                              "members.entry.item",
                                              "members.entry.list",
                                              "members.entry",
                                              "members",
                                              "members.opt",
                                              "method.name",
                                              "method.base",
                                              "method",
                                              "method.list",
                                              "method.list.opt",
                                              "block.open",
                                              "block",
                                              "block.opt_list",
                                              "block.list",
                                              "block.contents",
                                              "block.contents.item",
                                              "adverb",
                                              "binary_op.raw",
                                              "binary_op.no_adverb",
                                              "binary_op",
                                              "msgsend",
                                              "expr.base",
                                              "expr",
                                              "expr_or_seq.base",
                                              "expr_or_seq",
                                              "generator",
                                              "generator.qual.list",
                                              "generator.qual",
                                              "argument_declarations.list",
                                              "argument_declarations.pipelist",
                                              "argument_declarations",
                                              "argument_declarations.opt",
                                              "arguments.entries",
                                              "arguments.no_trailing",
                                              "arguments",
                                              "arguments.paren",
                                              "arguments.maybe_paren",
                                              "variable_declarations.list.item",
                                              "variable_declarations.list",
                                              "variable_declarations",
                                              "literal.terminal",
                                              "literal.array.contents",
                                              "literal.dictionary.entry",
                                              "literal.dictionary.entries",
                                              "literal.dictionary",
                                              "literal.array",
                                              "literal",
                                              "name",
                                              "name.list",
                                              "ascii",
                                              "nil",
                                              "boolean",
                                              "symbol",
                                              "string",
                                              "integer",
                                              "float.raw_unsigned",
                                              "float.raw",
                                              "accidental.unsigned",
                                              "accidental",
                                              "float",
                                              "accessor",
                                              "semicolon.opt",
                                              "comma.opt",
                                              YY_NULLPTR };
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


  const short parser::yypact_ninf_ = -256;

  const short parser::yytable_ninf_ = -257;

  const short parser::yypact_[] = {
      177,  16,   521,  19,   36,   29,   10,   -256, -256, -256, 26,   63,   -256, 87,   1576, 595,  190,  26,   -256,
      -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, 39,   -256, -256, -256, -256, -256,
      293,  1870, 84,   -256, -256, 131,  462,  -256, -256, -256, -256, -256, 40,   -256, -256, -256, -256, 123,  7,
      -256, 137,  -256, -256, -256, 193,  -256, -256, 1919, 1919, 183,  -256, -256, 199,  202,  216,  1870, 1870, 717,
      1870, 462,  219,  205,  249,  105,  184,  1870, 1625, -256, 1870, 293,  -256, -256, -256, -256, 13,   -256, -256,
      257,  2152, 170,  925,  258,  -256, 255,  1576, 26,   -256, 261,  222,  -256, 1576, 974,  -256, 101,  101,  -256,
      171,  -256, -256, -256, -256, 13,   -256, 1027, 772,  -256, -256, -256, 194,  220,  1870, 1076, 1870, 101,  -256,
      -256, -256, 112,  662,  -256, 462,  -256, -256, -256, -256, 203,  266,  268,  -256, 1870, -256, 1870, 1674, 275,
      116,  -256, 271,  6,    -256, 26,   14,   34,   277,  1870, 103,  -256, 26,   -256, 102,  1964, 153,  1870, -256,
      1870, 1723, -256, 1870, 247,  -256, 462,  -256, 1378, 1772, 280,  249,  -256, 245,  42,   1870, 186,  101,  1870,
      1870, -256, -256, 272,  282,  -256, -256, 101,  -256, 247,  250,  1870, 11,   822,  1126, 101,  43,   138,  -256,
      239,  101,  287,  462,  160,  296,  -256, -256, 112,  297,  -256, -256, 1176, -20,  -20,  -20,  -256, 294,  112,
      1427, 1427, 462,  -256, 1870, 260,  -256, 261,  105,  1870, 1870, 148,  26,   26,   295,  1870, 2038, -256, 309,
      1870, 1821, 15,   -256, 26,   0,    1772, 300,  2075, -256, 1225, 101,  -256, 262,  -256, 310,  462,  1477, 1870,
      311,  233,  1870, 2152, -256, -256, 314,  26,   -256, 1870, 1870, 462,  -256, 1674, -256, -256, -256, 1526, -256,
      -256, 317,  1870, 316,  278,  1870, 279,  1870, 17,   101,  -256, 1275, 1870, 101,  1870, -256, -256, 101,  52,
      -256, 84,   -256, -256, -256, 237,  -256, -256, -256, -256, 312,  105,  312,  312,  203,  -256, 321,  26,   179,
      -256, 318,  -256, 91,   318,  -256, 1870, 51,   -256, 320,  462,  -256, 26,   53,   244,  -256, 324,  1772, -256,
      -256, 298,  -256, 325,  26,   -256, 23,   -256, 2001, 326,  1772, -256, 101,  327,  1870, -256, 1870, -256, 462,
      -256, -256, 1477, -256, -256, 328,  462,  -256, -256, 462,  -256, 1870, 336,  1870, 339,  299,  1870, -256, -256,
      338,  462,  -256, 340,  1870, -256, 875,  -256, -20,  -256, -256, -256, 54,   1870, 1870, 1427, 347,  1870, 308,
      352,  -256, 1870, 1870, -256, 353,  1870, 2112, -256, 26,   -256, 356,  1870, -256, 319,  26,   -256, 1772, 2149,
      -256, -256, 357,  101,  101,  358,  462,  -256, 462,  -256, 462,  322,  1870, 361,  -256, -256, 462,  -256, 359,
      247,  -256, 1870, 1870, -256, -256, -256, -256, -256, 1870, -256, 365,  462,  -256, 366,  1772, -256, 367,  -256,
      371,  -256, 330,  374,  1772, -256, -256, -256, -256, 1870, 462,  341,  -256, 1328, 383,  380,  -256, -256, -256,
      -256, 382,  -256, -256, -256, -256, 384,  462,  1870, -256, 247,  -256, -256, -256, -256, 462,  387,  -256
  };

  const short parser::yydefact_[] = {
      4,   23,  0,   0,   0,   0,   0,   16,  18,  19,  0,   21,  9,   48,  201, 210, 0,   0,   219, 232, 233, 234, 236,
      237, 238, 239, 242, 243, 228, 229, 230, 224, 114, 225, 226, 227, 250, 49,  0,   0,   173, 200, 94,  109, 8,   92,
      216, 218, 217, 91,  93,  199, 198, 197, 193, 194, 195, 240, 246, 244, 247, 196, 0,   1,   15,  0,   257, 0,   3,
      17,  0,   0,   0,   0,   0,   210, 0,   129, 256, 202, 258, 0,   0,   0,   0,   66,  75,  70,  68,  69,  71,  72,
      73,  67,  74,  0,   129, 0,   0,   0,   211, 258, 201, 0,   220, 258, 96,  48,  201, 0,   54,  88,  52,  251, 0,
      235, 241, 245, 117, 0,   174, 0,   0,   75,  70,  73,  0,   77,  0,   0,   0,   83,  231, 249, 248, 46,  210, 14,
      5,   13,  2,   24,  22,  34,  0,   0,   204, 257, 131, 0,   259, 0,   0,   190, 192, 185, 161, 0,   258, 143, 0,
      0,   0,   209, 0,   167, 258, 152, 0,   0,   95,  0,   0,   12,  0,   256, 56,  61,  62,  0,   259, 0,   258, 221,
      259, 0,   0,   0,   52,  0,   0,   177, 178, 258, 0,   55,  89,  53,  51,  256, 98,  0,   0,   0,   0,   52,  182,
      0,   76,  118, 0,   0,   119, 71,  0,   42,  44,  47,  0,   37,  36,  0,   252, 252, 252, 32,  256, 46,  0,   0,
      130, 203, 0,   205, 214, 258, 0,   0,   0,   0,   259, 0,   0,   0,   0,   104, 0,   0,   0,   0,   259, 0,   0,
      210, 0,   0,   151, 0,   80,  208, 0,   101, 0,   60,  257, 0,   0,   0,   0,   0,   212, 213, 0,   0,   222, 0,
      0,   120, 87,  0,   90,  175, 176, 259, 180, 181, 0,   0,   0,   97,  0,   116, 0,   0,   52,  85,  0,   0,   52,
      0,   63,  64,  82,  52,  43,  173, 45,  25,  7,   0,   253, 254, 255, 27,  30,  0,   29,  31,  257, 35,  0,   0,
      0,   140, 258, 134, 93,  258, 207, 0,   0,   191, 0,   186, 165, 0,   147, 0,   162, 0,   210, 144, 107, 0,   108,
      0,   0,   171, 0,   168, 157, 0,   210, 153, 0,   0,   0,   102, 0,   57,  58,  10,  11,  0,   215, 223, 0,   123,
      179, 50,  126, 100, 0,   0,   0,   0,   115, 0,   86,  183, 0,   121, 84,  0,   0,   81,  0,   6,   252, 26,  33,
      20,  0,   0,   0,   259, 0,   0,   0,   0,   206, 0,   0,   187, 0,   0,   0,   163, 0,   145, 0,   0,   105, 0,
      0,   169, 210, 0,   156, 154, 0,   79,  52,  0,   59,  124, 125, 99,  128, 111, 0,   0,   184, 65,  122, 39,  256,
      256, 28,  0,   0,   142, 141, 135, 133, 136, 0,   132, 0,   188, 166, 0,   210, 148, 0,   146, 0,   172, 0,   0,
      210, 158, 155, 78,  103, 0,   127, 110, 257, 0,   0,   0,   138, 137, 189, 149, 0,   164, 106, 170, 159, 0,   113,
      0,   41,  256, 38,  139, 150, 160, 112, 0,   40
  };

  const short parser::yypgoto_[] = { -256, -256, 332,  -256, -256, 395,  -256, -256, -256, -256, 18,   41,   86,
                                     -256, -256, -256, 189,  188,  -256, 182,  -256, 176,  -182, -29,  -114, -255,
                                     -256, -104, -7,   -256, -256, -256, -2,   -256, 2,    -256, 181,  20,   -256,
                                     -256, -11,  107,  134,  -256, -110, 301,  -256, -231, -256, 404,  -256, 120,
                                     240,  -256, -256, -256, -165, 259,  269,  -256, -256, -256, -256, -256, -27,
                                     -18,  -256, -13,  -256, -256, -256, -77,  -34 };

  const short parser::yydefgoto_[] = { 0,   4,   137, 5,   6,   7,  8,   72,  11,  9,   313, 314, 225, 226, 227,
                                       214, 215, 216, 217, 218, 40, 41,  196, 197, 175, 176, 208, 94,  127, 128,
                                       42,  43,  77,  78,  191, 45, 324, 325, 158, 166, 120, 121, 192, 193, 194,
                                       112, 298, 153, 154, 178, 46, 80,  100, 101, 47,  48,  49,  50,  105, 51,
                                       52,  53,  54,  55,  56,  57, 58,  59,  60,  61,  315, 67,  151 };

  const short parser::yytable_[] = {
      44,  148, 256, 111, 98,  331, 280, 199,  95,  359, 68,  115, 202, 96,  237, 18,  79,  97,  289, 211, 116, 131,
      10,  295, 376, 117, 240, 346, 18,  -256, 64,  219, 65,  310, 311, 414, 63,  118, 312, 66,  1,   18,  243, 107,
      107, 108, 133, 109, 129, 62,  275, 296,  290, 164, 241, 238, 107, 349, 377, 401, 115, 405, 439, 138, 138, 267,
      3,   181, 95,  116, 165, 185, 347, 96,   117, 144, 145, 97,  146, 341, 415, 37,  37,  244, 389, 160, 162, 115,
      163, 130, 353, 276, 297, 293, 37,  107,  177, 73,  266, 129, 402, 384, 406, 440, 79,  107, 18,  309, 359, 219,
      79,  71,  378, 219, 250, 247, 382, 82,   152, 177, 18,  385, 286, 219, 242, 221, 209, 18,  212, 95,  103, 18,
      252, 37,  96,  74,  397, 122, 97,  258,  130, 119, 251, 37,  248, 230, 299, 355, 272, 319, 132, 231, 233, 18,
      19,  20,  21,  107, 334, 284, 335, 257,  85,  246, 124, 88,  89,  213, 91,  125, 93,  259, 263, 260, 262, 18,
      134, 177, 269, 170, 301, 302, 171, 140,  277, 418, 380, 19,  20,  21,  114, 281, 282, 278, 156, 37,  102, 135,
      279, 18,  203, 330, 204, 288, 103, 18,   141, 1,   110, 18,  85,  172, 124, 88,  89,  90,  91,  125, 93,  177,
      143, 2,   182, 114, 157, 393, 110, 394,  187, 147, 323, 323, 142, 3,   328, 222, 333, 223, 224, 332, 463, 453,
      362, 363, 265, 339, 387, 363, 265, 343,  345, 96,  461, 149, 407, 351, 408, 264, 265, 273, 18,  150, 177, 360,
      316, 317, 168, 180, 179, 70,  163, 186,  437, 184, 367, 104, 106, 366, 228, 207, 229, 233, 234, 236, 283, 370,
      245, 195, 110, 271, 396, 285, 373, 399,  375, 381, 303, 131, 126, 287, 305, 383, 307, 356, 318, 338, 102, 195,
      329, 19,  20,  21,  22,  23,  24,  25,   26,  27,  342, 357, 361, 364, 369, 371, 388, 421, 391, 372, 374, 403,
      395, 400, 113, 409, 412, 419, 422, 425,  96,  411, 155, 159, 410, 427, 110, 114, 429, 432, 430, 433, 96,  167,
      444, 446, 420, 485, 424, 447, 423, 469,  470, 177, 183, 450, 110, 455, 462, 464, 467, 468, 426, 465, 428, 195,
      474, 475, 457, 477, 167, 431, 478, 110,  434, 480, 177, 206, 110, 479, 486, 487, 483, 488, 492, 489, 220, 441,
      442, 323, 139, 445, 449, 69,  304, 448,  390, 306, 438, 451, 491, 320, 327, 104, 386, 456, 96,  443, 239, 368,
      459, 99,  270, 235, 0,   249, 0,   0,    0,   205, 466, 0,   0,   0,   0,   0,   195, 0,   0,   0,   0,   0,
      0,   471, 472, 274, 0,   0,   0,   0,    473, 0,   96,  0,   0,   0,   476, 0,   0,   0,   96,  0,   0,   0,
      481, 482, 0,   0,   300, 177, 0,   0,    110, 0,   220, 0,   110, 0,   220, 0,   195, 110, 0,   490, 0,   0,
      0,   0,   220, 326, 326, 0,   0,   0,    0,   0,   0,   155, 0,   0,   0,   336, 337, 0,   110, 0,   0,   0,
      0,   0,   0,   0,   348, 350, 85,  123,  124, 88,  89,  90,  91,  125, 93,  126, 12,  0,   0,   13,  0,   14,
      0,   15,  110, 0,   365, 0,   16,  17,   18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
      0,   32,  0,   0,   0,   0,   0,   33,   34,  35,  36,  0,   0,   37,  0,   0,   0,   0,   0,   0,   0,   0,
      0,   38,  155, 0,   0,   0,   0,   0,    392, 39,  0,   0,   0,   398, 0,   0,   0,   0,   0,   0,   0,   0,
      404, 0,   0,   195, 110, 13,  0,   14,   0,   75,  0,   413, 0,   0,   16,  17,  18,  19,  20,  21,  22,  23,
      24,  25,  26,  27,  28,  29,  30,  31,   0,   32,  0,   81,  82,  0,   0,   33,  34,  35,  36,  0,   83,  37,
      0,   0,   0,   0,   0,   84,  0,   85,   86,  87,  88,  89,  90,  91,  92,  93,  326, 39,  0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   13,  454,  14,  0,   75,  0,   0,   458, 0,   16,  17,  18,  19,  20,  21,  22,
      23,  24,  25,  26,  27,  28,  29,  30,   31,  0,   32,  0,   0,   82,  0,   0,   33,  34,  35,  36,  0,   83,
      37,  0,   0,   0,   0,   0,   84,  0,    85,  86,  87,  88,  89,  90,  91,  92,  93,  13,  39,  14,  0,   75,
      0,   0,   0,   0,   16,  17,  18,  19,   20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  0,   32,
      0,   0,   0,   0,   0,   33,  34,  35,   36,  0,   83,  37,  0,   0,   0,   0,   0,   84,  0,   85,  86,  87,
      88,  89,  90,  91,  125, 93,  13,  39,   14,  200, 75,  0,   0,   0,   0,   16,  17,  18,  19,  20,  21,  22,
      23,  24,  25,  26,  27,  28,  29,  30,   31,  0,   32,  0,   0,   0,   0,   0,   33,  34,  35,  36,  0,   201,
      37,  0,   0,   0,   0,   0,   0,   0,    0,   189, 38,  0,   13,  190, 14,  291, 75,  0,   39,  0,   0,   16,
      17,  18,  19,  20,  21,  22,  23,  24,   25,  26,  27,  28,  29,  30,  31,  0,   32,  0,   0,   0,   0,   0,
      33,  34,  35,  36,  0,   292, 37,  0,    0,   0,   0,   0,   0,   0,   0,   189, 38,  0,   0,   190, 0,   13,
      435, 14,  39,  75,  0,   0,   174, 0,    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
      30,  31,  436, 32,  0,   81,  0,   0,    0,   33,  34,  35,  36,  0,   0,   37,  0,   0,   0,   0,   0,   0,
      0,   0,   0,   38,  0,   13,  0,   14,   0,   75,  173, 39,  174, 0,   16,  17,  18,  19,  20,  21,  22,  23,
      24,  25,  26,  27,  28,  29,  30,  31,   0,   32,  0,   81,  0,   0,   0,   33,  34,  35,  36,  0,   0,   37,
      0,   0,   0,   0,   0,   0,   0,   0,    0,   38,  13,  0,   14,  0,   75,  188, 0,   39,  0,   16,  17,  18,
      19,  20,  21,  22,  23,  24,  25,  26,   27,  28,  29,  30,  31,  0,   32,  0,   0,   0,   0,   0,   33,  34,
      35,  36,  0,   0,   37,  0,   0,   0,    0,   0,   0,   0,   0,   189, 38,  0,   0,   190, 0,   13,  198, 14,
      39,  75,  0,   0,   174, 0,   16,  17,   18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
      0,   32,  0,   81,  0,   0,   0,   33,   34,  35,  36,  0,   0,   37,  0,   0,   0,   0,   0,   0,   0,   0,
      0,   38,  13,  0,   14,  0,   75,  210,  0,   39,  0,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,
      27,  28,  29,  30,  31,  0,   32,  0,    0,   0,   0,   0,   33,  34,  35,  36,  0,   0,   37,  0,   0,   0,
      0,   0,   0,   0,   0,   189, 38,  0,    13,  190, 14,  0,   75,  294, 39,  0,   0,   16,  17,  18,  19,  20,
      21,  22,  23,  24,  25,  26,  27,  28,   29,  30,  31,  0,   32,  0,   0,   0,   0,   0,   33,  34,  35,  36,
      0,   0,   37,  0,   0,   0,   0,   0,    0,   0,   0,   189, 38,  0,   13,  190, 14,  0,   75,  308, 39,  174,
      0,   16,  17,  18,  19,  20,  21,  22,   23,  24,  25,  26,  27,  28,  29,  30,  31,  0,   32,  0,   81,  0,
      0,   0,   33,  34,  35,  36,  0,   0,    37,  0,   0,   0,   0,   0,   0,   0,   0,   0,   38,  13,  0,   14,
      0,   75,  354, 0,   39,  0,   16,  17,   18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
      0,   32,  0,   0,   0,   0,   0,   33,   34,  35,  36,  0,   0,   37,  0,   0,   0,   0,   0,   0,   0,   0,
      189, 38,  0,   13,  190, 14,  0,   75,   379, 39,  0,   0,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,
      26,  27,  28,  29,  30,  31,  0,   32,   0,   0,   0,   0,   0,   33,  34,  35,  36,  0,   0,   37,  0,   0,
      0,   0,   0,   0,   0,   0,   189, 38,   0,   0,   190, 0,   13,  484, 14,  39,  75,  0,   0,   174, 0,   16,
      17,  18,  19,  20,  21,  22,  23,  24,   25,  26,  27,  28,  29,  30,  31,  0,   32,  0,   81,  0,   0,   0,
      33,  34,  35,  36,  0,   0,   37,  0,    0,   0,   0,   0,   0,   0,   0,   0,   38,  0,   13,  0,   14,  0,
      75,  0,   39,  174, 0,   16,  17,  18,   19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  0,
      32,  0,   81,  0,   0,   0,   33,  34,   35,  36,  0,   0,   37,  0,   0,   0,   0,   0,   0,   0,   0,   0,
      38,  13,  0,   14,  0,   75,  0,   0,    39,  0,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
      28,  29,  30,  31,  0,   32,  0,   321,  0,   0,   0,   33,  34,  35,  36,  0,   0,   37,  0,   0,   0,   0,
      0,   322, 0,   0,   0,   38,  0,   13,   0,   14,  0,   75,  0,   39,  358, 0,   16,  17,  18,  19,  20,  21,
      22,  23,  24,  25,  26,  27,  28,  29,   30,  31,  0,   32,  0,   81,  0,   0,   0,   33,  34,  35,  36,  0,
      0,   37,  0,   0,   0,   0,   0,   0,    0,   0,   0,   38,  13,  0,   14,  0,   75,  0,   0,   39,  0,   16,
      17,  18,  19,  20,  21,  22,  23,  24,   25,  26,  27,  28,  29,  30,  31,  0,   32,  0,   0,   0,   0,   0,
      33,  34,  35,  36,  0,   0,   37,  0,    0,   0,   0,   0,   0,   0,   0,   189, 38,  0,   13,  190, 14,  0,
      75,  0,   39,  0,   0,   16,  17,  18,   19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  0,
      32,  0,   0,   0,   0,   0,   33,  34,   35,  36,  0,   0,   37,  0,   0,   0,   0,   0,   0,   0,   0,   76,
      38,  13,  0,   14,  0,   75,  0,   0,    39,  0,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
      28,  29,  30,  31,  0,   32,  0,   0,    0,   0,   0,   33,  34,  35,  36,  0,   161, 37,  0,   0,   0,   0,
      0,   0,   0,   0,   0,   38,  13,  0,    14,  0,   75,  0,   0,   39,  0,   16,  17,  18,  19,  20,  21,  22,
      23,  24,  25,  26,  27,  28,  29,  30,   31,  0,   32,  0,   0,   0,   0,   0,   33,  34,  35,  36,  0,   0,
      37,  0,   0,   0,   0,   0,   0,   0,    0,   232, 38,  13,  0,   14,  0,   75,  261, 0,   39,  0,   16,  17,
      18,  19,  20,  21,  22,  23,  24,  25,   26,  27,  28,  29,  30,  31,  0,   32,  0,   0,   0,   0,   0,   33,
      34,  35,  36,  0,   0,   37,  0,   0,    0,   0,   0,   0,   0,   0,   0,   38,  13,  0,   14,  0,   75,  0,
      0,   39,  0,   16,  17,  18,  19,  20,   21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  0,   32,  0,
      0,   0,   0,   0,   33,  34,  35,  36,   0,   0,   37,  0,   0,   0,   0,   0,   0,   0,   0,   268, 38,  13,
      0,   14,  0,   75,  344, 0,   39,  0,    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
      30,  31,  0,   32,  0,   0,   0,   0,    0,   33,  34,  35,  36,  0,   0,   37,  0,   0,   0,   0,   0,   0,
      0,   0,   0,   38,  13,  0,   14,  0,    75,  0,   0,   39,  0,   16,  17,  18,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  29,  30,  31,  0,    32,  0,   0,   0,   0,   0,   33,  34,  35,  36,  0,   0,   37,  0,
      0,   0,   0,   0,   0,   0,   0,   0,    38,  13,  0,   14,  0,   136, 0,   0,   39,  0,   16,  17,  18,  19,
      20,  21,  22,  23,  24,  25,  26,  27,   28,  29,  30,  31,  0,   32,  0,   0,   0,   0,   0,   33,  34,  35,
      36,  0,   0,   37,  0,   0,   0,   0,    0,   0,   107, 0,   14,  38,  253, 0,   0,   0,   0,   254, 0,   39,
      19,  20,  21,  22,  23,  24,  25,  26,   27,  28,  29,  30,  31,  0,   0,   0,   0,   0,   0,   0,   33,  34,
      35,  36,  0,   107, 37,  14,  0,   416,  0,   0,   0,   255, 254, 0,   38,  19,  20,  21,  22,  23,  24,  25,
      26,  27,  28,  29,  30,  31,  0,   0,    0,   0,   0,   0,   0,   33,  34,  35,  36,  0,   107, 37,  14,  0,
      340, 0,   0,   0,   417, 254, 0,   38,   19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  0,
      0,   0,   0,   0,   0,   0,   33,  34,   35,  36,  0,   107, 37,  14,  0,   352, 0,   0,   0,   0,   254, 0,
      38,  19,  20,  21,  22,  23,  24,  25,   26,  27,  28,  29,  30,  31,  0,   0,   0,   0,   0,   0,   0,   33,
      34,  35,  36,  0,   107, 37,  14,  0,    452, 0,   0,   0,   0,   254, 0,   38,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  29,  30,  31,  0,    0,   0,   0,   0,   0,   0,   33,  34,  35,  36,  0,   107, 37,  14,
      0,   460, 0,   0,   0,   0,   254, 0,    38,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
      0,   0,   0,   0,   0,   0,   0,   33,   34,  35,  36,  0,   0,   37,  0,   0,   0,   0,   0,   0,   0,   0,
      169, 38,  85,  123, 124, 88,  89,  90,   91,  125, 93,  126
  };

  const short parser::yycheck_[] = {
      2,   78,  167, 32,  15,  236, 188, 121, 15,  264, 0,   38,  122, 15,  8,   15,  14,  15,  7,   129, 38,  50,  6,
      205, 7,   38,  12,  12,  15,  0,   1,   135, 3,   53,  54,  12,  0,   39,  58,  10,  30,  15,  8,   4,   4,   6,
      39,  8,   8,   30,  8,   8,   41,  40,  40,  49,  4,   57,  41,  8,   87,  8,   8,   65,  66,  179, 56,  101, 75,
      87,  57,  105, 57,  75,  87,  73,  74,  75,  76,  244, 57,  42,  42,  49,  315, 83,  84,  114, 86,  49,  255, 49,
      49,  203, 42,  4,   98,  10,  175, 8,   49,  49,  49,  49,  102, 4,   15,  221, 363, 213, 108, 48,  294, 217, 12,
      12,  298, 33,  13,  121, 15,  303, 199, 227, 158, 136, 128, 15,  130, 136, 14,  15,  166, 42,  136, 48,  45,  6,
      136, 168, 49,  57,  40,  42,  41,  147, 8,   257, 182, 226, 27,  149, 150, 15,  16,  17,  18,  4,   10,  193, 12,
      8,   50,  161, 52,  53,  54,  55,  56,  57,  58,  169, 174, 171, 172, 15,  39,  179, 180, 9,   207, 210, 12,  0,
      186, 350, 296, 16,  17,  18,  52,  189, 190, 7,   10,  42,  6,   4,   12,  15,  6,   235, 8,   201, 14,  15,  7,
      30,  32,  15,  50,  41,  52,  53,  54,  55,  56,  57,  58,  221, 4,   44,  102, 52,  40,  46,  50,  48,  108, 10,
      228, 229, 30,  56,  232, 32,  238, 34,  35,  237, 422, 406, 9,   10,  11,  243, 9,   10,  11,  247, 248, 253, 417,
      48,  10,  253, 12,  10,  11,  14,  15,  12,  264, 265, 223, 224, 9,   12,  10,  10,  268, 49,  386, 12,  276, 16,
      17,  275, 12,  59,  12,  279, 7,   12,  12,  287, 9,   111, 112, 9,   324, 9,   290, 327, 292, 297, 9,   326, 59,
      49,  4,   299, 5,   41,  10,  10,  6,   131, 48,  16,  17,  18,  19,  20,  21,  22,  23,  24,  9,   9,   9,   7,
      5,   7,   12,  354, 5,   49,  49,  9,   12,  329, 39,  9,   9,   9,   9,   9,   340, 41,  81,  82,  340, 7,   168,
      52,  7,   9,   49,  9,   352, 92,  5,   45,  352, 469, 358, 5,   356, 436, 437, 363, 103, 10,  188, 9,   9,   9,
      7,   10,  372, 49,  374, 197, 9,   9,   57,  10,  119, 377, 9,   205, 384, 9,   386, 126, 210, 57,  5,   9,   49,
      9,   5,   9,   135, 393, 394, 395, 66,  397, 402, 6,   213, 401, 318, 217, 388, 405, 485, 227, 229, 152, 305, 411,
      416, 395, 157, 283, 416, 15,  180, 152, -1,  164, -1,  -1,  -1,  126, 430, -1,  -1,  -1,  -1,  -1,  258, -1,  -1,
      -1,  -1,  -1,  -1,  439, 440, 184, -1,  -1,  -1,  -1,  446, -1,  452, -1,  -1,  -1,  452, -1,  -1,  -1,  460, -1,
      -1,  -1,  460, 465, -1,  -1,  207, 469, -1,  -1,  294, -1,  213, -1,  298, -1,  217, -1,  302, 303, -1,  483, -1,
      -1,  -1,  -1,  227, 228, 229, -1,  -1,  -1,  -1,  -1,  -1,  236, -1,  -1,  -1,  240, 241, -1,  326, -1,  -1,  -1,
      -1,  -1,  -1,  -1,  251, 252, 50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  1,   -1,  -1,  4,   -1,  6,   -1,
      8,   354, -1,  273, -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,
      -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  52,  315,
      -1,  -1,  -1,  -1,  -1,  321, 60,  -1,  -1,  -1,  326, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  335, -1,  -1,  421,
      422, 4,   -1,  6,   -1,  8,   -1,  346, -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,
      26,  27,  28,  -1,  30,  -1,  32,  33,  -1,  -1,  36,  37,  38,  39,  -1,  41,  42,  -1,  -1,  -1,  -1,  -1,  48,
      -1,  50,  51,  52,  53,  54,  55,  56,  57,  58,  395, 60,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  4,
      408, 6,   -1,  8,   -1,  -1,  414, -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
      28,  -1,  30,  -1,  -1,  33,  -1,  -1,  36,  37,  38,  39,  -1,  41,  42,  -1,  -1,  -1,  -1,  -1,  48,  -1,  50,
      51,  52,  53,  54,  55,  56,  57,  58,  4,   60,  6,   -1,  8,   -1,  -1,  -1,  -1,  13,  14,  15,  16,  17,  18,
      19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  41,
      42,  -1,  -1,  -1,  -1,  -1,  48,  -1,  50,  51,  52,  53,  54,  55,  56,  57,  58,  4,   60,  6,   7,   8,   -1,
      -1,  -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,
      -1,  -1,  -1,  36,  37,  38,  39,  -1,  41,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  51,  52,  -1,  4,   55,
      6,   7,   8,   -1,  60,  -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,
      -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  41,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  51,
      52,  -1,  -1,  55,  -1,  4,   5,   6,   60,  8,   -1,  -1,  11,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,
      22,  23,  24,  25,  26,  27,  28,  29,  30,  -1,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  52,  -1,  4,   -1,  6,   -1,  8,   9,   60,  11,  -1,  13,  14,  15,  16,  17,
      18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,
      -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  52,  4,   -1,  6,   -1,  8,   9,   -1,  60,  -1,  13,  14,
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,
      38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  51,  52,  -1,  -1,  55,  -1,  4,   5,   6,   60,
      8,   -1,  -1,  11,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,
      -1,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  52,  4,
      -1,  6,   -1,  8,   9,   -1,  60,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
      28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      51,  52,  -1,  4,   55,  6,   -1,  8,   9,   60,  -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,
      24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  51,  52,  -1,  4,   55,  6,   -1,  8,   9,   60,  11,  -1,  13,  14,  15,  16,  17,  18,  19,
      20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  52,  4,   -1,  6,   -1,  8,   9,   -1,  60,  -1,  13,  14,  15,  16,
      17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,
      -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  51,  52,  -1,  4,   55,  6,   -1,  8,   9,   60,  -1,  -1,
      13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,
      36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  51,  52,  -1,  -1,  55,  -1,  4,   5,
      6,   60,  8,   -1,  -1,  11,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,
      -1,  30,  -1,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      52,  -1,  4,   -1,  6,   -1,  8,   -1,  60,  11,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  -1,  30,  -1,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  52,  4,   -1,  6,   -1,  8,   -1,  -1,  60,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,
      22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,
      -1,  -1,  -1,  48,  -1,  -1,  -1,  52,  -1,  4,   -1,  6,   -1,  8,   -1,  60,  11,  -1,  13,  14,  15,  16,  17,
      18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,
      -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  52,  4,   -1,  6,   -1,  8,   -1,  -1,  60,  -1,  13,  14,
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,
      38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  51,  52,  -1,  4,   55,  6,   -1,  8,   -1,  60,
      -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,
      -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  51,  52,  4,   -1,  6,   -1,
      8,   -1,  -1,  60,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,
      -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  41,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  52,  4,
      -1,  6,   -1,  8,   -1,  -1,  60,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
      28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      51,  52,  4,   -1,  6,   -1,  8,   9,   -1,  60,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  52,  4,   -1,  6,   -1,  8,   -1,  -1,  60,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,
      22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  51,  52,  4,   -1,  6,   -1,  8,   9,   -1,  60,  -1,  13,  14,  15,  16,  17,  18,
      19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,
      42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  52,  4,   -1,  6,   -1,  8,   -1,  -1,  60,  -1,  13,  14,  15,
      16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,
      39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  52,  4,   -1,  6,   -1,  8,   -1,  -1,  60,  -1,
      13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,
      36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  4,   -1,  6,   52,  8,   -1,  -1,  -1,  -1,  13,
      -1,  60,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  36,
      37,  38,  39,  -1,  4,   42,  6,   -1,  8,   -1,  -1,  -1,  49,  13,  -1,  52,  16,  17,  18,  19,  20,  21,  22,
      23,  24,  25,  26,  27,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  4,   42,  6,   -1,  8,
      -1,  -1,  -1,  49,  13,  -1,  52,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  4,   42,  6,   -1,  8,   -1,  -1,  -1,  -1,  13,  -1,  52,  16,  17,
      18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,
      4,   42,  6,   -1,  8,   -1,  -1,  -1,  -1,  13,  -1,  52,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,
      27,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  4,   42,  6,   -1,  8,   -1,  -1,  -1,  -1,
      13,  -1,  52,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  48,  52,  50,  51,  52,  53,  54,  55,
      56,  57,  58,  59
  };

  const unsigned char parser::yystos_[] = {
      0,   30,  44,  56,  63,  65,  66,  67,  68,  71,  6,   70,  1,   4,   6,   8,   13,  14,  15,  16,  17,  18,  19,
      20,  21,  22,  23,  24,  25,  26,  27,  28,  30,  36,  37,  38,  39,  42,  52,  60,  82,  83,  92,  93,  94,  97,
      112, 116, 117, 118, 119, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 30,  0,   1,   3,   10,  133, 0,
      67,  119, 48,  69,  10,  48,  8,   51,  94,  95,  96,  113, 32,  33,  41,  48,  50,  51,  52,  53,  54,  55,  56,
      57,  58,  89,  90,  94,  96,  102, 111, 114, 115, 6,   14,  119, 120, 119, 4,   6,   8,   83,  85,  107, 39,  52,
      126, 127, 129, 94,  57,  102, 103, 6,   51,  52,  57,  59,  90,  91,  8,   49,  85,  27,  39,  39,  4,   8,   64,
      94,  64,  0,   7,   30,  4,   96,  96,  96,  10,  133, 48,  12,  134, 13,  109, 110, 119, 10,  40,  100, 119, 96,
      41,  96,  96,  40,  57,  101, 119, 9,   48,  9,   12,  41,  9,   11,  86,  87,  94,  111, 10,  12,  134, 113, 119,
      12,  134, 49,  113, 9,   51,  55,  96,  104, 105, 106, 83,  84,  85,  5,   86,  7,   41,  106, 6,   8,   107, 119,
      59,  88,  94,  9,   106, 94,  55,  77,  78,  79,  80,  81,  89,  119, 102, 32,  34,  35,  74,  75,  76,  12,  12,
      94,  96,  51,  96,  7,   120, 12,  8,   49,  119, 12,  40,  134, 8,   49,  9,   96,  12,  41,  119, 12,  40,  134,
      8,   13,  49,  118, 8,   85,  96,  96,  9,   96,  94,  10,  11,  133, 86,  51,  94,  114, 9,   134, 14,  119, 8,
      49,  94,  7,   12,  84,  96,  96,  12,  134, 9,   133, 49,  96,  7,   41,  7,   41,  106, 9,   84,  8,   49,  108,
      8,   119, 126, 85,  9,   78,  4,   79,  5,   9,   86,  53,  54,  58,  72,  73,  132, 73,  73,  10,  133, 81,  32,
      48,  96,  98,  99,  119, 98,  96,  48,  134, 109, 96,  94,  10,  12,  119, 119, 10,  96,  8,   118, 9,   96,  9,
      96,  12,  57,  119, 57,  119, 96,  8,   118, 9,   106, 41,  9,   11,  87,  94,  9,   9,   10,  7,   119, 96,  94,
      104, 5,   94,  7,   49,  96,  49,  96,  7,   41,  84,  9,   106, 94,  84,  96,  49,  84,  103, 9,   12,  109, 74,
      5,   119, 46,  48,  12,  134, 45,  119, 134, 96,  8,   49,  9,   119, 8,   49,  10,  12,  9,   96,  41,  9,   119,
      12,  57,  8,   49,  118, 9,   96,  85,  9,   96,  94,  9,   94,  7,   94,  7,   49,  96,  9,   9,   94,  5,   29,
      86,  72,  8,   49,  96,  96,  99,  5,   96,  45,  5,   96,  94,  10,  96,  8,   118, 119, 9,   96,  57,  119, 96,
      8,   118, 9,   84,  9,   49,  94,  7,   10,  133, 133, 96,  96,  96,  9,   9,   96,  10,  9,   57,  9,   96,  94,
      49,  5,   86,  5,   9,   9,   9,   94,  133, 5
  };

  const unsigned char parser::yyr1_[] = {
      0,   62,  63,  63,  63,  64,  64,  64,  65,  65,  65,  65,  65,  65,  65,  65,  66,  66,  67,  67,  68,  69,
      69,  70,  70,  71,  72,  73,  73,  74,  74,  74,  75,  75,  76,  76,  77,  77,  78,  78,  78,  78,  79,  79,
      80,  80,  81,  81,  82,  82,  83,  83,  84,  84,  85,  85,  86,  86,  86,  86,  86,  87,  87,  88,  88,  88,
      89,  89,  89,  89,  89,  89,  89,  89,  90,  90,  91,  91,  92,  92,  92,  92,  92,  92,  92,  92,  92,  92,
      92,  92,  92,  93,  93,  93,  93,  93,  93,  93,  93,  93,  93,  93,  93,  93,  93,  93,  93,  93,  93,  94,
      94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  94,  95,  95,  96,
      97,  97,  98,  98,  99,  99,  99,  99,  99,  99,  99,  100, 100, 100, 100, 100, 100, 100, 100, 101, 101, 101,
      101, 101, 101, 101, 101, 101, 101, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 103, 103, 104,
      104, 104, 105, 105, 106, 107, 108, 108, 108, 109, 109, 109, 109, 109, 110, 110, 111, 112, 112, 112, 112, 112,
      112, 112, 112, 113, 113, 113, 113, 113, 113, 113, 114, 114, 115, 115, 115, 116, 117, 117, 118, 118, 118, 119,
      120, 120, 120, 120, 121, 122, 123, 123, 124, 124, 125, 125, 126, 126, 126, 126, 127, 127, 127, 127, 128, 128,
      129, 129, 130, 130, 131, 131, 131, 131, 131, 131, 132, 132, 132, 132, 133, 133, 134, 134
  };

  const signed char parser::yyr2_[] = {
      0, 2, 3, 2, 0, 1, 4, 3, 2, 2, 6, 6, 4, 3, 3, 2, 1, 2, 1, 1, 7, 0, 2, 0, 3, 5, 2, 1, 3, 2, 2, 2, 1, 3, 0, 2, 1, 1,
      6, 4, 8, 6, 1, 2, 1, 2, 0, 1, 1, 1, 5, 3, 0, 1, 1, 2, 1, 3, 3, 4, 2, 1, 1, 2, 2, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      2, 1, 7, 6, 4, 5, 4, 2, 5, 4, 5, 4, 2, 3, 4, 1, 1, 1, 1, 3, 2, 4, 3, 6, 5, 4, 5, 7, 4, 6, 8, 5, 5, 1, 7, 6, 9, 8,
      1, 5, 4, 2, 3, 3, 4, 5, 6, 5, 6, 6, 5, 7, 6, 1, 3, 2, 7, 7, 1, 3, 3, 4, 4, 5, 1, 3, 3, 1, 3, 4, 5, 3, 5, 6, 7, 2,
      1, 3, 4, 5, 4, 3, 5, 6, 7, 2, 4, 5, 7, 4, 6, 2, 4, 5, 7, 4, 6, 0, 1, 2, 2, 1, 1, 3, 2, 3, 0, 2, 3, 1, 3, 4, 5, 6,
      1, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 3, 2, 3, 5, 4, 3, 2, 0, 1, 3, 4, 4, 5, 1, 1, 1, 1, 1, 2, 3, 4, 1, 1, 1, 1,
      1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 2, 1, 2, 0, 1, 1, 1, 0, 1, 0, 1
  };


#if YYDEBUG
  const short parser::yyrline_[] = {
      0,   179, 179, 180, 181, 189, 191, 193, 199, 201, 207, 213, 214, 220, 223, 226, 258, 260, 265, 266, 270, 275,
      276, 280, 281, 285, 294, 299, 301, 306, 308, 310, 315, 317, 322, 323, 331, 332, 336, 338, 340, 342, 347, 348,
      353, 354, 358, 359, 367, 367, 370, 372, 377, 378, 382, 383, 387, 388, 389, 394, 399, 407, 408, 417, 418, 419,
      423, 424, 425, 426, 427, 428, 429, 430, 434, 435, 439, 441, 446, 449, 452, 456, 459, 462, 465, 468, 471, 481,
      484, 489, 495, 507, 508, 509, 511, 512, 514, 516, 519, 522, 524, 528, 530, 532, 534, 540, 542, 544, 547, 555,
      557, 560, 563, 566, 571, 573, 576, 579, 581, 584, 587, 590, 593, 596, 598, 604, 607, 610, 613, 618, 619, 623,
      632, 638, 643, 645, 650, 652, 654, 656, 658, 660, 662, 675, 677, 679, 681, 683, 685, 687, 689, 694, 696, 698,
      700, 702, 704, 706, 708, 710, 712, 717, 718, 719, 721, 723, 728, 737, 739, 741, 743, 745, 750, 762, 763, 767,
      768, 769, 773, 774, 777, 779, 782, 783, 785, 794, 796, 798, 800, 802, 807, 809, 813, 821, 822, 823, 824, 825,
      826, 827, 828, 832, 834, 836, 838, 840, 842, 844, 852, 854, 859, 861, 863, 868, 873, 875, 884, 885, 886, 893,
      897, 899, 901, 903, 914, 916, 919, 920, 924, 925, 929, 930, 934, 935, 936, 937, 946, 947, 948, 949, 953, 955,
      960, 962, 967, 969, 974, 975, 976, 977, 978, 979, 983, 984, 985, 986, 998, 998, 999, 999
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
    static const signed char translate_table[] = {
        0,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
        42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61
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
#line 5253 "langutils/sc_parser/src/sc_grammar_parser.cpp"

#line 1001 "langutils/sc_parser/src/sc_grammar.y"
