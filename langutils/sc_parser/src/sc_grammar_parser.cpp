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

      case symbol_kind::S_125_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.copy< AccidentalIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_adverb: // adverb
        value.copy< AdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_92_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_94_expr_seq_base: // expr.seq.base
      case symbol_kind::S_95_expr_seq: // expr.seq
          value.copy<AnyExprIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_method: // method
        value.copy< AnyMethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_108_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.copy<AnyPossiblyLiteralIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_100_arguments_entries: // arguments.entries
          value.copy<ArgumentEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_101_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_103_arguments_paren: // arguments.paren
      case symbol_kind::S_104_arguments_maybe_paren: // arguments.maybe_paren
          value.copy<ArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_109_literal_array_contents: // literal.array.contents
      case symbol_kind::S_113_literal_array: // literal.array
          value.copy<ArrayIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_75_members_opt: // members.opt
          value.copy<BigMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_85_block_contents: // block.contents
          value.copy<BlockContentsListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_block: // block
        value.copy< BlockIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_86_block_contents_item: // block.contents.item
          value.copy<BlockItemIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_83_block_opt_list: // block.opt_list
      case symbol_kind::S_84_block_list: // block.list
        value.copy< BlockListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_boolean: // boolean
        value.copy< BooleanLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_70_class_extension: // class.extension
        value.copy< ClassExtensionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_class: // class
        value.copy< ClassIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_go: // go
        value.copy< ClassListOrExprListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_66_classOrExtList_item: // classOrExtList.item
        value.copy< ClassOrExtensionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_65_classOrExtList_list: // classOrExtList.list
        value.copy< ClassOrExtensionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_73_members_entry: // members.entry
          value.copy<DeclareAnyMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_105_variable_declarations_list_item: // variable_declarations.list.item
          value.copy<DeclareAnyVariableIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_96_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_97_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_99_argument_declarations_opt: // argument_declarations.opt
          value.copy<DeclareArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_71_members_entry_item: // members.entry.item
          value.copy<DeclareMemberIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_72_members_entry_list: // members.entry.list
          value.copy<DeclareMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_106_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.copy< DeclareVariableListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_110_literal_dictionary_entry: // literal.dictionary.entry
          value.copy<DictionaryEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_111_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_112_literal_dictionary: // literal.dictionary
          value.copy<DictionaryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_123_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_124_float_raw: // float.raw
          value.copy<FloatLitIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_float: // float
        value.copy< FloatProducingIndex > (YY_MOVE (that.value));
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

      case symbol_kind::S_77_method_base: // method.base
        value.copy< MethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_79_method_list: // method.list
      case symbol_kind::S_80_method_list_opt: // method.list.opt
        value.copy< MethodListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_116_name_list: // name.list
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

      case symbol_kind::S_76_method_name: // method.name
      case symbol_kind::S_88_binary_op_raw: // binary_op.raw
      case symbol_kind::S_89_binary_op_no_adverb: // binary_op.no_adverb
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

      case symbol_kind::S_63_region_item: // region.item
        value.copy< error_index<AnyExprIndex> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_68_class_super_opt: // class.super.opt
        value.copy< maybe<ClassNameIdentifierIndex> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_69_class_slot_opt: // class.slot.opt
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

      case symbol_kind::S_125_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.move< AccidentalIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_adverb: // adverb
        value.move< AdverbIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_92_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_94_expr_seq_base: // expr.seq.base
      case symbol_kind::S_95_expr_seq: // expr.seq
          value.move<AnyExprIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_method: // method
        value.move< AnyMethodIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_108_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.move<AnyPossiblyLiteralIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_100_arguments_entries: // arguments.entries
          value.move<ArgumentEntryIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_101_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_103_arguments_paren: // arguments.paren
      case symbol_kind::S_104_arguments_maybe_paren: // arguments.maybe_paren
          value.move<ArgumentListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_109_literal_array_contents: // literal.array.contents
      case symbol_kind::S_113_literal_array: // literal.array
          value.move<ArrayIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_75_members_opt: // members.opt
          value.move<BigMemberListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_85_block_contents: // block.contents
          value.move<BlockContentsListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_block: // block
        value.move< BlockIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_86_block_contents_item: // block.contents.item
          value.move<BlockItemIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_83_block_opt_list: // block.opt_list
      case symbol_kind::S_84_block_list: // block.list
        value.move< BlockListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_boolean: // boolean
        value.move< BooleanLitIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_70_class_extension: // class.extension
        value.move< ClassExtensionIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_class: // class
        value.move< ClassIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_go: // go
        value.move< ClassListOrExprListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_66_classOrExtList_item: // classOrExtList.item
        value.move< ClassOrExtensionIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_65_classOrExtList_list: // classOrExtList.list
        value.move< ClassOrExtensionListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_73_members_entry: // members.entry
          value.move<DeclareAnyMemberListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_105_variable_declarations_list_item: // variable_declarations.list.item
          value.move<DeclareAnyVariableIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_96_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_97_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_99_argument_declarations_opt: // argument_declarations.opt
          value.move<DeclareArgumentListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_71_members_entry_item: // members.entry.item
          value.move<DeclareMemberIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_72_members_entry_list: // members.entry.list
          value.move<DeclareMemberListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_106_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.move< DeclareVariableListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_110_literal_dictionary_entry: // literal.dictionary.entry
          value.move<DictionaryEntryIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_111_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_112_literal_dictionary: // literal.dictionary
          value.move<DictionaryIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_123_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_124_float_raw: // float.raw
          value.move<FloatLitIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_float: // float
        value.move< FloatProducingIndex > (YY_MOVE (s.value));
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

      case symbol_kind::S_77_method_base: // method.base
        value.move< MethodIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_79_method_list: // method.list
      case symbol_kind::S_80_method_list_opt: // method.list.opt
        value.move< MethodListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_116_name_list: // name.list
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

      case symbol_kind::S_76_method_name: // method.name
      case symbol_kind::S_88_binary_op_raw: // binary_op.raw
      case symbol_kind::S_89_binary_op_no_adverb: // binary_op.no_adverb
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

      case symbol_kind::S_63_region_item: // region.item
        value.move< error_index<AnyExprIndex> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_68_class_super_opt: // class.super.opt
        value.move< maybe<ClassNameIdentifierIndex> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_69_class_slot_opt: // class.slot.opt
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

      case symbol_kind::S_125_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.YY_MOVE_OR_COPY< AccidentalIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_adverb: // adverb
        value.YY_MOVE_OR_COPY< AdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_92_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_94_expr_seq_base: // expr.seq.base
      case symbol_kind::S_95_expr_seq: // expr.seq
          value.YY_MOVE_OR_COPY<AnyExprIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_method: // method
        value.YY_MOVE_OR_COPY< AnyMethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_108_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.YY_MOVE_OR_COPY<AnyPossiblyLiteralIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_100_arguments_entries: // arguments.entries
          value.YY_MOVE_OR_COPY<ArgumentEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_101_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_103_arguments_paren: // arguments.paren
      case symbol_kind::S_104_arguments_maybe_paren: // arguments.maybe_paren
          value.YY_MOVE_OR_COPY<ArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_109_literal_array_contents: // literal.array.contents
      case symbol_kind::S_113_literal_array: // literal.array
          value.YY_MOVE_OR_COPY<ArrayIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_75_members_opt: // members.opt
          value.YY_MOVE_OR_COPY<BigMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_85_block_contents: // block.contents
          value.YY_MOVE_OR_COPY<BlockContentsListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_block: // block
        value.YY_MOVE_OR_COPY< BlockIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_86_block_contents_item: // block.contents.item
          value.YY_MOVE_OR_COPY<BlockItemIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_83_block_opt_list: // block.opt_list
      case symbol_kind::S_84_block_list: // block.list
        value.YY_MOVE_OR_COPY< BlockListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_boolean: // boolean
        value.YY_MOVE_OR_COPY< BooleanLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_70_class_extension: // class.extension
        value.YY_MOVE_OR_COPY< ClassExtensionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_class: // class
        value.YY_MOVE_OR_COPY< ClassIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_go: // go
        value.YY_MOVE_OR_COPY< ClassListOrExprListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_66_classOrExtList_item: // classOrExtList.item
        value.YY_MOVE_OR_COPY< ClassOrExtensionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_65_classOrExtList_list: // classOrExtList.list
        value.YY_MOVE_OR_COPY< ClassOrExtensionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_73_members_entry: // members.entry
          value.YY_MOVE_OR_COPY<DeclareAnyMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_105_variable_declarations_list_item: // variable_declarations.list.item
          value.YY_MOVE_OR_COPY<DeclareAnyVariableIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_96_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_97_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_99_argument_declarations_opt: // argument_declarations.opt
          value.YY_MOVE_OR_COPY<DeclareArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_71_members_entry_item: // members.entry.item
          value.YY_MOVE_OR_COPY<DeclareMemberIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_72_members_entry_list: // members.entry.list
          value.YY_MOVE_OR_COPY<DeclareMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_106_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.YY_MOVE_OR_COPY< DeclareVariableListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_110_literal_dictionary_entry: // literal.dictionary.entry
          value.YY_MOVE_OR_COPY<DictionaryEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_111_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_112_literal_dictionary: // literal.dictionary
          value.YY_MOVE_OR_COPY<DictionaryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_123_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_124_float_raw: // float.raw
          value.YY_MOVE_OR_COPY<FloatLitIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_float: // float
        value.YY_MOVE_OR_COPY< FloatProducingIndex > (YY_MOVE (that.value));
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

      case symbol_kind::S_77_method_base: // method.base
        value.YY_MOVE_OR_COPY< MethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_79_method_list: // method.list
      case symbol_kind::S_80_method_list_opt: // method.list.opt
        value.YY_MOVE_OR_COPY< MethodListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_116_name_list: // name.list
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

      case symbol_kind::S_76_method_name: // method.name
      case symbol_kind::S_88_binary_op_raw: // binary_op.raw
      case symbol_kind::S_89_binary_op_no_adverb: // binary_op.no_adverb
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

      case symbol_kind::S_63_region_item: // region.item
        value.YY_MOVE_OR_COPY< error_index<AnyExprIndex> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_68_class_super_opt: // class.super.opt
        value.YY_MOVE_OR_COPY< maybe<ClassNameIdentifierIndex> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_69_class_slot_opt: // class.slot.opt
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

      case symbol_kind::S_125_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.move< AccidentalIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_adverb: // adverb
        value.move< AdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_92_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_94_expr_seq_base: // expr.seq.base
      case symbol_kind::S_95_expr_seq: // expr.seq
          value.move<AnyExprIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_method: // method
        value.move< AnyMethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_108_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.move<AnyPossiblyLiteralIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_100_arguments_entries: // arguments.entries
          value.move<ArgumentEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_101_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_103_arguments_paren: // arguments.paren
      case symbol_kind::S_104_arguments_maybe_paren: // arguments.maybe_paren
          value.move<ArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_109_literal_array_contents: // literal.array.contents
      case symbol_kind::S_113_literal_array: // literal.array
          value.move<ArrayIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_75_members_opt: // members.opt
          value.move<BigMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_85_block_contents: // block.contents
          value.move<BlockContentsListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_block: // block
        value.move< BlockIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_86_block_contents_item: // block.contents.item
          value.move<BlockItemIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_83_block_opt_list: // block.opt_list
      case symbol_kind::S_84_block_list: // block.list
        value.move< BlockListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_boolean: // boolean
        value.move< BooleanLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_70_class_extension: // class.extension
        value.move< ClassExtensionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_class: // class
        value.move< ClassIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_go: // go
        value.move< ClassListOrExprListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_66_classOrExtList_item: // classOrExtList.item
        value.move< ClassOrExtensionIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_65_classOrExtList_list: // classOrExtList.list
        value.move< ClassOrExtensionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_73_members_entry: // members.entry
          value.move<DeclareAnyMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_105_variable_declarations_list_item: // variable_declarations.list.item
          value.move<DeclareAnyVariableIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_96_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_97_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_99_argument_declarations_opt: // argument_declarations.opt
          value.move<DeclareArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_71_members_entry_item: // members.entry.item
          value.move<DeclareMemberIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_72_members_entry_list: // members.entry.list
          value.move<DeclareMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_106_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.move< DeclareVariableListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_110_literal_dictionary_entry: // literal.dictionary.entry
          value.move<DictionaryEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_111_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_112_literal_dictionary: // literal.dictionary
          value.move<DictionaryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_123_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_124_float_raw: // float.raw
          value.move<FloatLitIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_float: // float
        value.move< FloatProducingIndex > (YY_MOVE (that.value));
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

      case symbol_kind::S_77_method_base: // method.base
        value.move< MethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_79_method_list: // method.list
      case symbol_kind::S_80_method_list_opt: // method.list.opt
        value.move< MethodListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_116_name_list: // name.list
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

      case symbol_kind::S_76_method_name: // method.name
      case symbol_kind::S_88_binary_op_raw: // binary_op.raw
      case symbol_kind::S_89_binary_op_no_adverb: // binary_op.no_adverb
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

      case symbol_kind::S_63_region_item: // region.item
        value.move< error_index<AnyExprIndex> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_68_class_super_opt: // class.super.opt
        value.move< maybe<ClassNameIdentifierIndex> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_69_class_slot_opt: // class.slot.opt
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

      case symbol_kind::S_125_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.copy< AccidentalIndex > (that.value);
        break;

      case symbol_kind::S_adverb: // adverb
        value.copy< AdverbIndex > (that.value);
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_92_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_94_expr_seq_base: // expr.seq.base
      case symbol_kind::S_95_expr_seq: // expr.seq
          value.copy<AnyExprIndex>(that.value);
          break;

      case symbol_kind::S_method: // method
        value.copy< AnyMethodIndex > (that.value);
        break;

      case symbol_kind::S_108_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.copy<AnyPossiblyLiteralIndex>(that.value);
          break;

      case symbol_kind::S_100_arguments_entries: // arguments.entries
          value.copy<ArgumentEntryIndex>(that.value);
          break;

      case symbol_kind::S_101_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_103_arguments_paren: // arguments.paren
      case symbol_kind::S_104_arguments_maybe_paren: // arguments.maybe_paren
          value.copy<ArgumentListIndex>(that.value);
          break;

      case symbol_kind::S_109_literal_array_contents: // literal.array.contents
      case symbol_kind::S_113_literal_array: // literal.array
          value.copy<ArrayIndex>(that.value);
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_75_members_opt: // members.opt
          value.copy<BigMemberListIndex>(that.value);
          break;

      case symbol_kind::S_85_block_contents: // block.contents
          value.copy<BlockContentsListIndex>(that.value);
          break;

      case symbol_kind::S_block: // block
        value.copy< BlockIndex > (that.value);
        break;

      case symbol_kind::S_86_block_contents_item: // block.contents.item
          value.copy<BlockItemIndex>(that.value);
          break;

      case symbol_kind::S_83_block_opt_list: // block.opt_list
      case symbol_kind::S_84_block_list: // block.list
        value.copy< BlockListIndex > (that.value);
        break;

      case symbol_kind::S_boolean: // boolean
        value.copy< BooleanLitIndex > (that.value);
        break;

      case symbol_kind::S_70_class_extension: // class.extension
        value.copy< ClassExtensionIndex > (that.value);
        break;

      case symbol_kind::S_class: // class
        value.copy< ClassIndex > (that.value);
        break;

      case symbol_kind::S_go: // go
        value.copy< ClassListOrExprListIndex > (that.value);
        break;

      case symbol_kind::S_66_classOrExtList_item: // classOrExtList.item
        value.copy< ClassOrExtensionIndex > (that.value);
        break;

      case symbol_kind::S_65_classOrExtList_list: // classOrExtList.list
        value.copy< ClassOrExtensionListIndex > (that.value);
        break;

      case symbol_kind::S_73_members_entry: // members.entry
          value.copy<DeclareAnyMemberListIndex>(that.value);
          break;

      case symbol_kind::S_105_variable_declarations_list_item: // variable_declarations.list.item
          value.copy<DeclareAnyVariableIndex>(that.value);
          break;

      case symbol_kind::S_96_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_97_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_99_argument_declarations_opt: // argument_declarations.opt
          value.copy<DeclareArgumentListIndex>(that.value);
          break;

      case symbol_kind::S_71_members_entry_item: // members.entry.item
          value.copy<DeclareMemberIndex>(that.value);
          break;

      case symbol_kind::S_72_members_entry_list: // members.entry.list
          value.copy<DeclareMemberListIndex>(that.value);
          break;

      case symbol_kind::S_106_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.copy< DeclareVariableListIndex > (that.value);
        break;

      case symbol_kind::S_110_literal_dictionary_entry: // literal.dictionary.entry
          value.copy<DictionaryEntryIndex>(that.value);
          break;

      case symbol_kind::S_111_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_112_literal_dictionary: // literal.dictionary
          value.copy<DictionaryIndex>(that.value);
          break;

      case symbol_kind::S_123_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_124_float_raw: // float.raw
          value.copy<FloatLitIndex>(that.value);
          break;

      case symbol_kind::S_float: // float
        value.copy< FloatProducingIndex > (that.value);
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

      case symbol_kind::S_77_method_base: // method.base
        value.copy< MethodIndex > (that.value);
        break;

      case symbol_kind::S_79_method_list: // method.list
      case symbol_kind::S_80_method_list_opt: // method.list.opt
        value.copy< MethodListIndex > (that.value);
        break;

      case symbol_kind::S_116_name_list: // name.list
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

      case symbol_kind::S_76_method_name: // method.name
      case symbol_kind::S_88_binary_op_raw: // binary_op.raw
      case symbol_kind::S_89_binary_op_no_adverb: // binary_op.no_adverb
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

      case symbol_kind::S_63_region_item: // region.item
        value.copy< error_index<AnyExprIndex> > (that.value);
        break;

      case symbol_kind::S_68_class_super_opt: // class.super.opt
        value.copy< maybe<ClassNameIdentifierIndex> > (that.value);
        break;

      case symbol_kind::S_69_class_slot_opt: // class.slot.opt
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

      case symbol_kind::S_125_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        value.move< AccidentalIndex > (that.value);
        break;

      case symbol_kind::S_adverb: // adverb
        value.move< AdverbIndex > (that.value);
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_92_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_94_expr_seq_base: // expr.seq.base
      case symbol_kind::S_95_expr_seq: // expr.seq
          value.move<AnyExprIndex>(that.value);
          break;

      case symbol_kind::S_method: // method
        value.move< AnyMethodIndex > (that.value);
        break;

      case symbol_kind::S_108_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.move<AnyPossiblyLiteralIndex>(that.value);
          break;

      case symbol_kind::S_100_arguments_entries: // arguments.entries
          value.move<ArgumentEntryIndex>(that.value);
          break;

      case symbol_kind::S_101_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_103_arguments_paren: // arguments.paren
      case symbol_kind::S_104_arguments_maybe_paren: // arguments.maybe_paren
          value.move<ArgumentListIndex>(that.value);
          break;

      case symbol_kind::S_109_literal_array_contents: // literal.array.contents
      case symbol_kind::S_113_literal_array: // literal.array
          value.move<ArrayIndex>(that.value);
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_75_members_opt: // members.opt
          value.move<BigMemberListIndex>(that.value);
          break;

      case symbol_kind::S_85_block_contents: // block.contents
          value.move<BlockContentsListIndex>(that.value);
          break;

      case symbol_kind::S_block: // block
        value.move< BlockIndex > (that.value);
        break;

      case symbol_kind::S_86_block_contents_item: // block.contents.item
          value.move<BlockItemIndex>(that.value);
          break;

      case symbol_kind::S_83_block_opt_list: // block.opt_list
      case symbol_kind::S_84_block_list: // block.list
        value.move< BlockListIndex > (that.value);
        break;

      case symbol_kind::S_boolean: // boolean
        value.move< BooleanLitIndex > (that.value);
        break;

      case symbol_kind::S_70_class_extension: // class.extension
        value.move< ClassExtensionIndex > (that.value);
        break;

      case symbol_kind::S_class: // class
        value.move< ClassIndex > (that.value);
        break;

      case symbol_kind::S_go: // go
        value.move< ClassListOrExprListIndex > (that.value);
        break;

      case symbol_kind::S_66_classOrExtList_item: // classOrExtList.item
        value.move< ClassOrExtensionIndex > (that.value);
        break;

      case symbol_kind::S_65_classOrExtList_list: // classOrExtList.list
        value.move< ClassOrExtensionListIndex > (that.value);
        break;

      case symbol_kind::S_73_members_entry: // members.entry
          value.move<DeclareAnyMemberListIndex>(that.value);
          break;

      case symbol_kind::S_105_variable_declarations_list_item: // variable_declarations.list.item
          value.move<DeclareAnyVariableIndex>(that.value);
          break;

      case symbol_kind::S_96_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_97_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_99_argument_declarations_opt: // argument_declarations.opt
          value.move<DeclareArgumentListIndex>(that.value);
          break;

      case symbol_kind::S_71_members_entry_item: // members.entry.item
          value.move<DeclareMemberIndex>(that.value);
          break;

      case symbol_kind::S_72_members_entry_list: // members.entry.list
          value.move<DeclareMemberListIndex>(that.value);
          break;

      case symbol_kind::S_106_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.move< DeclareVariableListIndex > (that.value);
        break;

      case symbol_kind::S_110_literal_dictionary_entry: // literal.dictionary.entry
          value.move<DictionaryEntryIndex>(that.value);
          break;

      case symbol_kind::S_111_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_112_literal_dictionary: // literal.dictionary
          value.move<DictionaryIndex>(that.value);
          break;

      case symbol_kind::S_123_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_124_float_raw: // float.raw
          value.move<FloatLitIndex>(that.value);
          break;

      case symbol_kind::S_float: // float
        value.move< FloatProducingIndex > (that.value);
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

      case symbol_kind::S_77_method_base: // method.base
        value.move< MethodIndex > (that.value);
        break;

      case symbol_kind::S_79_method_list: // method.list
      case symbol_kind::S_80_method_list_opt: // method.list.opt
        value.move< MethodListIndex > (that.value);
        break;

      case symbol_kind::S_116_name_list: // name.list
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

      case symbol_kind::S_76_method_name: // method.name
      case symbol_kind::S_88_binary_op_raw: // binary_op.raw
      case symbol_kind::S_89_binary_op_no_adverb: // binary_op.no_adverb
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

      case symbol_kind::S_63_region_item: // region.item
        value.move< error_index<AnyExprIndex> > (that.value);
        break;

      case symbol_kind::S_68_class_super_opt: // class.super.opt
        value.move< maybe<ClassNameIdentifierIndex> > (that.value);
        break;

      case symbol_kind::S_69_class_slot_opt: // class.slot.opt
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

      case symbol_kind::S_125_accidental_unsigned: // accidental.unsigned
      case symbol_kind::S_accidental: // accidental
        yylhs.value.emplace< AccidentalIndex > ();
        break;

      case symbol_kind::S_adverb: // adverb
        yylhs.value.emplace< AdverbIndex > ();
        break;

      case symbol_kind::S_msgsend: // msgsend
      case symbol_kind::S_92_expr_base: // expr.base
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_94_expr_seq_base: // expr.seq.base
      case symbol_kind::S_95_expr_seq: // expr.seq
          yylhs.value.emplace<AnyExprIndex>();
          break;

      case symbol_kind::S_method: // method
        yylhs.value.emplace< AnyMethodIndex > ();
        break;

      case symbol_kind::S_108_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          yylhs.value.emplace<AnyPossiblyLiteralIndex>();
          break;

      case symbol_kind::S_100_arguments_entries: // arguments.entries
          yylhs.value.emplace<ArgumentEntryIndex>();
          break;

      case symbol_kind::S_101_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_103_arguments_paren: // arguments.paren
      case symbol_kind::S_104_arguments_maybe_paren: // arguments.maybe_paren
          yylhs.value.emplace<ArgumentListIndex>();
          break;

      case symbol_kind::S_109_literal_array_contents: // literal.array.contents
      case symbol_kind::S_113_literal_array: // literal.array
          yylhs.value.emplace<ArrayIndex>();
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_75_members_opt: // members.opt
          yylhs.value.emplace<BigMemberListIndex>();
          break;

      case symbol_kind::S_85_block_contents: // block.contents
          yylhs.value.emplace<BlockContentsListIndex>();
          break;

      case symbol_kind::S_block: // block
        yylhs.value.emplace< BlockIndex > ();
        break;

      case symbol_kind::S_86_block_contents_item: // block.contents.item
          yylhs.value.emplace<BlockItemIndex>();
          break;

      case symbol_kind::S_83_block_opt_list: // block.opt_list
      case symbol_kind::S_84_block_list: // block.list
        yylhs.value.emplace< BlockListIndex > ();
        break;

      case symbol_kind::S_boolean: // boolean
        yylhs.value.emplace< BooleanLitIndex > ();
        break;

      case symbol_kind::S_70_class_extension: // class.extension
        yylhs.value.emplace< ClassExtensionIndex > ();
        break;

      case symbol_kind::S_class: // class
        yylhs.value.emplace< ClassIndex > ();
        break;

      case symbol_kind::S_go: // go
        yylhs.value.emplace< ClassListOrExprListIndex > ();
        break;

      case symbol_kind::S_66_classOrExtList_item: // classOrExtList.item
        yylhs.value.emplace< ClassOrExtensionIndex > ();
        break;

      case symbol_kind::S_65_classOrExtList_list: // classOrExtList.list
        yylhs.value.emplace< ClassOrExtensionListIndex > ();
        break;

      case symbol_kind::S_73_members_entry: // members.entry
          yylhs.value.emplace<DeclareAnyMemberListIndex>();
          break;

      case symbol_kind::S_105_variable_declarations_list_item: // variable_declarations.list.item
          yylhs.value.emplace<DeclareAnyVariableIndex>();
          break;

      case symbol_kind::S_96_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_97_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_99_argument_declarations_opt: // argument_declarations.opt
          yylhs.value.emplace<DeclareArgumentListIndex>();
          break;

      case symbol_kind::S_71_members_entry_item: // members.entry.item
          yylhs.value.emplace<DeclareMemberIndex>();
          break;

      case symbol_kind::S_72_members_entry_list: // members.entry.list
          yylhs.value.emplace<DeclareMemberListIndex>();
          break;

      case symbol_kind::S_106_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        yylhs.value.emplace< DeclareVariableListIndex > ();
        break;

      case symbol_kind::S_110_literal_dictionary_entry: // literal.dictionary.entry
          yylhs.value.emplace<DictionaryEntryIndex>();
          break;

      case symbol_kind::S_111_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_112_literal_dictionary: // literal.dictionary
          yylhs.value.emplace<DictionaryIndex>();
          break;

      case symbol_kind::S_123_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_124_float_raw: // float.raw
          yylhs.value.emplace<FloatLitIndex>();
          break;

      case symbol_kind::S_float: // float
        yylhs.value.emplace< FloatProducingIndex > ();
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

      case symbol_kind::S_77_method_base: // method.base
        yylhs.value.emplace< MethodIndex > ();
        break;

      case symbol_kind::S_79_method_list: // method.list
      case symbol_kind::S_80_method_list_opt: // method.list.opt
        yylhs.value.emplace< MethodListIndex > ();
        break;

      case symbol_kind::S_116_name_list: // name.list
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

      case symbol_kind::S_76_method_name: // method.name
      case symbol_kind::S_88_binary_op_raw: // binary_op.raw
      case symbol_kind::S_89_binary_op_no_adverb: // binary_op.no_adverb
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

      case symbol_kind::S_63_region_item: // region.item
        yylhs.value.emplace< error_index<AnyExprIndex> > ();
        break;

      case symbol_kind::S_68_class_super_opt: // class.super.opt
        yylhs.value.emplace< maybe<ClassNameIdentifierIndex> > ();
        break;

      case symbol_kind::S_69_class_slot_opt: // class.slot.opt
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
          {
              yylhs.value.as<ClassListOrExprListIndex>() =
                  cxt.graph.assign_root(yystack_[2].value.as<RegionListIndex>());
          }
#line 2488 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 3: // go: classOrExtList.list $end
#line 175 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassListOrExprListIndex>() =
                  cxt.graph.assign_root(yystack_[1].value.as<ClassOrExtensionListIndex>());
          }
#line 2494 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 4: // go: %empty
#line 176 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassListOrExprListIndex>() =
                  cxt.graph.assign_root(cxt.create(ClassOrExtensionList {}, yylhs.location));
          }
#line 2500 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 5: // region.item: expr
#line 185 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<error_index<AnyExprIndex>>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 2506 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 6: // region.item: OPENPAREN argument_declarations block.contents CLOSEPAREN
#line 187 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto block = cxt.create(BlockNode {}, yylhs.location, yystack_[2].value.as<DeclareArgumentListIndex>(),
                                      yystack_[1].value.as<BlockContentsListIndex>());

              yylhs.value.as<error_index<AnyExprIndex>>() = cxt.create(
                  MessageNode { MessageNode::SelectorMode::Value }, yylhs.location,
                  cxt.create(Missing {}, yylhs.location), cxt.create(ArgumentList {}, yylhs.location, block));
          }
#line 2521 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 7: // region.item: OPENPAREN argument_declarations CLOSEPAREN
#line 198 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto block = cxt.create(BlockNode {}, yylhs.location, yystack_[1].value.as<DeclareArgumentListIndex>(),
                                      cxt.create(BlockContentsList {}, yylhs.location));

              yylhs.value.as<error_index<AnyExprIndex>>() = cxt.create(
                  MessageNode { MessageNode::SelectorMode::Value }, yylhs.location,
                  cxt.create(Missing {}, yylhs.location), cxt.create(ArgumentList {}, yylhs.location, block));
          }
#line 2536 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 8: // region: INTERPRET expr
#line 213 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RegionListIndex>() =
                  cxt.create(RegionList {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 2542 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 9: // region: INTERPRET error
#line 215 "langutils/sc_parser/src/sc_grammar.y"
          {
              error_recovery::expr(cxt);
              yyclearin;
              yylhs.value.as<RegionListIndex>() =
                  cxt.create(RegionList {}, yylhs.location, create_error(cxt, yystack_[0].location));
          }
#line 2552 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 10: // region: INTERPRET OPENPAREN argument_declarations block.contents semicolon.opt CLOSEPAREN
#line 221 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto block = cxt.create(BlockNode {}, yylhs.location, yystack_[3].value.as<DeclareArgumentListIndex>(),
                                      yystack_[2].value.as<BlockContentsListIndex>());

              auto msg = cxt.create(MessageNode { MessageNode::SelectorMode::Value }, yylhs.location,
                                    cxt.create(Missing {}, yylhs.location),
                                    cxt.create(ArgumentList {}, yylhs.location, block));
              yylhs.value.as<RegionListIndex>() = cxt.create(RegionList {}, yylhs.location, msg);
          }
#line 2568 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 11: // region: INTERPRET OPENPAREN argument_declarations CLOSEPAREN
#line 233 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto block = cxt.create(BlockNode {}, yylhs.location, yystack_[1].value.as<DeclareArgumentListIndex>(),
                                      cxt.create(BlockContentsList {}, yylhs.location));

              auto msg = cxt.create(MessageNode { MessageNode::SelectorMode::Value }, yylhs.location,
                                    cxt.create(Missing {}, yylhs.location),
                                    cxt.create(ArgumentList {}, yylhs.location, block));

              yylhs.value.as<RegionListIndex>() = cxt.create(RegionList {}, yylhs.location, msg);
          }
#line 2585 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 12: // region: region SEMICOLON region.item
#line 247 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RegionListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<RegionListIndex>(), yylhs.location,
                                   yystack_[0].value.as<error_index<AnyExprIndex>>());
          }
#line 2591 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 13: // region: region REGION_SEPARATOR region.item
#line 250 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RegionListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<RegionListIndex>(), yylhs.location,
                                   yystack_[0].value.as<error_index<AnyExprIndex>>());
          }
#line 2597 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 14: // region: region error
#line 253 "langutils/sc_parser/src/sc_grammar.y"
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
#line 2619 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 15: // classOrExtList.list: classOrExtList.item
#line 285 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassOrExtensionListIndex>() =
                  cxt.create(ClassOrExtensionList {}, yylhs.location, yystack_[0].value.as<ClassOrExtensionIndex>());
          }
#line 2625 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 16: // classOrExtList.list: classOrExtList.list classOrExtList.item
#line 287 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassOrExtensionListIndex>() = cxt.graph.append(
                  yystack_[1].value.as<ClassOrExtensionListIndex>(), yystack_[0].value.as<ClassOrExtensionIndex>());
          }
#line 2631 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 17: // classOrExtList.item: class
#line 291 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassOrExtensionIndex>() = yystack_[0].value.as<ClassIndex>();
          }
#line 2637 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 18: // classOrExtList.item: class.extension
#line 292 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassOrExtensionIndex>() = yystack_[0].value.as<ClassExtensionIndex>();
          }
#line 2643 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 19: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY members.opt method.list.opt CLOSECURLY
#line 297 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassIndex>() =
                  cxt.create(Class {}, yylhs.location, cxt.create(ClassNameIdentifier {}, yystack_[6].location),
                             yystack_[5].value.as<maybe<NamedIdentifierIndex>>(),
                             yystack_[4].value.as<maybe<ClassNameIdentifierIndex>>(),
                             yystack_[2].value.as<BigMemberListIndex>(), yystack_[1].value.as<MethodListIndex>());
          }
#line 2649 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 20: // class.super.opt: %empty
#line 301 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<maybe<ClassNameIdentifierIndex>>() = cxt.create(Missing {}, yylhs.location);
          }
#line 2655 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 21: // class.super.opt: COLON CLASSNAME
#line 302 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<maybe<ClassNameIdentifierIndex>>() =
                  cxt.create(ClassNameIdentifier {}, yystack_[0].location);
          }
#line 2661 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 22: // class.slot.opt: %empty
#line 306 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<maybe<NamedIdentifierIndex>>() = cxt.create(Missing {}, yylhs.location);
          }
#line 2667 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 23: // class.slot.opt: OPENSQUARE name CLOSESQUARE
#line 307 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<maybe<NamedIdentifierIndex>>() = yystack_[1].value.as<NamedIdentifierIndex>();
          }
#line 2673 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 24: // class.extension: ADD CLASSNAME OPENCURLY method.list.opt CLOSECURLY
#line 312 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassExtensionIndex>() = cxt.create(
                  ClassExtension {}, yylhs.location, cxt.create(ClassNameIdentifier {}, yystack_[3].location),
                  yystack_[1].value.as<MethodListIndex>());
          }
#line 2679 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 25: // members.entry.item: accessor variable_declarations.list.item
#line 321 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareMemberIndex>() =
                  cxt.create(DeclareMember {}, yylhs.location, yystack_[1].value.as<RWAccessorIndex>(),
                             yystack_[0].value.as<DeclareAnyVariableIndex>());
          }
#line 2685 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 26: // members.entry.list: members.entry.item
#line 326 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareMemberListIndex>() =
                  cxt.create(DeclareMemberList {}, yylhs.location, yystack_[0].value.as<DeclareMemberIndex>());
          }
#line 2691 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 27: // members.entry.list: members.entry.list COMMA members.entry.item
#line 328 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareMemberListIndex>() = cxt.graph.append(
                  yystack_[2].value.as<DeclareMemberListIndex>(), yystack_[0].value.as<DeclareMemberIndex>());
          }
#line 2697 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 28: // members.entry: CLASSVAR members.entry.list
#line 333 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyMemberListIndex>() =
                  cxt.graph.cast<DeclareClassMemberList>(yystack_[0].value.as<DeclareMemberListIndex>());
          }
#line 2703 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 29: // members.entry: VAR members.entry.list
#line 335 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyMemberListIndex>() =
                  cxt.graph.cast<DeclareMemberList>(yystack_[0].value.as<DeclareMemberListIndex>());
          }
#line 2709 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 30: // members.entry: CONST members.entry.list
#line 337 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyMemberListIndex>() =
                  cxt.graph.cast<DeclareConstList>(yystack_[0].value.as<DeclareMemberListIndex>());
          }
#line 2715 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 31: // members: members.entry
#line 342 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BigMemberListIndex>() =
                  cxt.create(BigMemberList {}, yylhs.location, yystack_[0].value.as<DeclareAnyMemberListIndex>());
          }
#line 2721 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 32: // members: members SEMICOLON members.entry
#line 344 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BigMemberListIndex>() = cxt.graph.append(
                  yystack_[2].value.as<BigMemberListIndex>(), yystack_[0].value.as<DeclareAnyMemberListIndex>());
          }
#line 2727 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 33: // members.opt: %empty
#line 348 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BigMemberListIndex>() = cxt.create(BigMemberList {}, yylhs.location);
          }
#line 2733 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 34: // members.opt: members semicolon.opt
#line 349 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BigMemberListIndex>() = yystack_[1].value.as<BigMemberListIndex>();
          }
#line 2739 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 35: // method.name: name
#line 357 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = yystack_[0].value.as<NamedIdentifierIndex>();
          }
#line 2745 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 36: // method.name: binary_op.raw
#line 358 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = yystack_[0].value.as<SelectorIndex>();
          }
#line 2751 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 37: // method.base: method.name OPENCURLY argument_declarations.opt block.contents semicolon.opt
                   // CLOSECURLY
#line 363 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodIndex>() = cxt.create(
                  Method {}, yylhs.location, yystack_[5].value.as<SelectorIndex>(),
                  yystack_[3].value.as<DeclareArgumentListIndex>(), cxt.create(Missing {}, yystack_[4].location),
                  yystack_[2].value.as<BlockContentsListIndex>());
          }
#line 2757 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 38: // method.base: method.name OPENCURLY argument_declarations.opt CLOSECURLY
#line 365 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodIndex>() = cxt.create(
                  Method {}, yylhs.location, yystack_[3].value.as<SelectorIndex>(),
                  yystack_[1].value.as<DeclareArgumentListIndex>(), cxt.create(Missing {}, yystack_[2].location),
                  cxt.create(BlockList {}, yystack_[0].location));
          }
#line 2763 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 39: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt
                   // block.contents semicolon.opt CLOSECURLY
#line 367 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodIndex>() =
                  cxt.create(Method {}, yylhs.location, yystack_[7].value.as<SelectorIndex>(),
                             yystack_[5].value.as<DeclareArgumentListIndex>(),
                             cxt.create(PrimitiveIdentifier {}, yystack_[4].location),
                             yystack_[2].value.as<BlockContentsListIndex>());
          }
#line 2769 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 40: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt
                   // CLOSECURLY
#line 369 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodIndex>() =
                  cxt.create(Method {}, yylhs.location, yystack_[5].value.as<SelectorIndex>(),
                             yystack_[3].value.as<DeclareArgumentListIndex>(),
                             cxt.create(PrimitiveIdentifier {}, yystack_[2].location),
                             cxt.create(BlockList {}, yystack_[1].location));
          }
#line 2775 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 41: // method: method.base
#line 373 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyMethodIndex>() = yystack_[0].value.as<MethodIndex>();
          }
#line 2781 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 42: // method: MULTIPLY method.base
#line 375 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyMethodIndex>() = cxt.graph.cast<ClassMethod>(yystack_[0].value.as<MethodIndex>());
          }
#line 2787 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 43: // method.list: method
#line 379 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodListIndex>() =
                  cxt.create(MethodList {}, yylhs.location, yystack_[0].value.as<AnyMethodIndex>());
          }
#line 2793 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 44: // method.list: method.list method
#line 380 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodListIndex>() = cxt.graph.append(
                  yystack_[1].value.as<MethodListIndex>(), yylhs.location, yystack_[0].value.as<AnyMethodIndex>());
          }
#line 2799 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 45: // method.list.opt: %empty
#line 384 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodListIndex>() = cxt.create(MethodList {}, yylhs.location);
          }
#line 2805 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 46: // method.list.opt: method.list
#line 385 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodListIndex>() = yystack_[0].value.as<MethodListIndex>();
          }
#line 2811 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 49: // block: block.open argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 397 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockIndex>() =
                  cxt.create(BlockNode {}, yylhs.location, yystack_[3].value.as<DeclareArgumentListIndex>(),
                             yystack_[2].value.as<BlockContentsListIndex>());
          }
#line 2817 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 50: // block: block.open argument_declarations.opt CLOSECURLY
#line 399 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockIndex>() =
                  cxt.create(BlockNode {}, yylhs.location, yystack_[1].value.as<DeclareArgumentListIndex>(),
                             cxt.create(BlockContentsList {}, yylhs.location));
          }
#line 2823 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 51: // block.opt_list: %empty
#line 403 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockListIndex>() = {};
          }
#line 2829 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 52: // block.opt_list: block.list
#line 404 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockListIndex>() = yystack_[0].value.as<BlockListIndex>();
          }
#line 2835 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 53: // block.list: block
#line 408 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockListIndex>() =
                  cxt.create(BlockList {}, yylhs.location, yystack_[0].value.as<BlockIndex>());
          }
#line 2841 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 54: // block.list: block.list block
#line 409 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockListIndex>() = cxt.graph.append(yystack_[1].value.as<BlockListIndex>(),
                                                                  yylhs.location, yystack_[0].value.as<BlockIndex>());
          }
#line 2847 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 55: // block.contents: block.contents.item
#line 413 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockContentsListIndex>() =
                  cxt.create(BlockContentsList {}, yylhs.location, yystack_[0].value.as<BlockItemIndex>());
          }
#line 2853 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 56: // block.contents: block.contents SEMICOLON block.contents.item
#line 414 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockContentsListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<BlockContentsListIndex>(), yylhs.location,
                                   yystack_[0].value.as<BlockItemIndex>());
          }
#line 2859 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 57: // block.contents: block.contents NONLOCALRETURN expr
#line 416 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto non_local =
                  cxt.create(NonLocalReturnExpr {}, yystack_[0].location, yystack_[0].value.as<AnyExprIndex>());
              yylhs.value.as<BlockContentsListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<BlockContentsListIndex>(), yylhs.location, non_local);
          }
#line 2868 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 58: // block.contents: block.contents SEMICOLON NONLOCALRETURN expr
#line 421 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto non_local =
                  cxt.create(NonLocalReturnExpr {}, yystack_[0].location, yystack_[0].value.as<AnyExprIndex>());
              yylhs.value.as<BlockContentsListIndex>() =
                  cxt.graph.append(yystack_[3].value.as<BlockContentsListIndex>(), yylhs.location, non_local);
          }
#line 2877 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 59: // block.contents: NONLOCALRETURN expr
#line 426 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto non_local =
                  cxt.create(NonLocalReturnExpr {}, yystack_[0].location, yystack_[0].value.as<AnyExprIndex>());
              yylhs.value.as<BlockContentsListIndex>() = cxt.create(BlockContentsList {}, yylhs.location, non_local);
          }
#line 2886 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 60: // block.contents.item: expr
#line 433 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockItemIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 2892 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 61: // block.contents.item: variable_declarations
#line 434 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockItemIndex>() = yystack_[0].value.as<DeclareVariableListIndex>();
          }
#line 2898 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 62: // adverb: DOT name
#line 443 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AdverbIndex>() = yystack_[0].value.as<NamedIdentifierIndex>();
          }
#line 2904 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 63: // adverb: DOT integer
#line 444 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AdverbIndex>() = yystack_[0].value.as<IntLitIndex>();
          }
#line 2910 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 64: // adverb: DOT OPENPAREN expr.seq CLOSEPAREN
#line 445 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AdverbIndex>() =
                  cxt.create(AdverbExprNode {}, yylhs.location, yystack_[1].value.as<AnyExprIndex>());
          }
#line 2916 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 65: // binary_op.raw: BINOP
#line 449 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 2922 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 66: // binary_op.raw: READWRITEVAR
#line 450 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 2928 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 67: // binary_op.raw: LESSTHAN
#line 451 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 2934 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 68: // binary_op.raw: GREATERTHAN
#line 452 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 2940 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 69: // binary_op.raw: MINUS
#line 453 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 2946 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 70: // binary_op.raw: MULTIPLY
#line 454 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 2952 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 71: // binary_op.raw: ADD
#line 455 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 2958 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 72: // binary_op.raw: PIPE
#line 456 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 2964 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 73: // binary_op.no_adverb: binary_op.raw
#line 460 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = yystack_[0].value.as<SelectorIndex>();
          }
#line 2970 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 74: // binary_op.no_adverb: KEYBINOP
#line 461 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { true }, yylhs.location);
          }
#line 2976 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 75: // binary_op: binary_op.no_adverb adverb
#line 466 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorMaybeAdverbIndex>() =
                  cxt.create(SelectorWAdverb {}, yylhs.location, yystack_[1].value.as<SelectorIndex>(),
                             yystack_[0].value.as<AdverbIndex>());
          }
#line 2982 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 76: // binary_op: binary_op.no_adverb
#line 468 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorMaybeAdverbIndex>() = yystack_[0].value.as<SelectorIndex>();
          }
#line 2988 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 77: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN arguments CLOSEPAREN block.opt_list
#line 473 "langutils/sc_parser/src/sc_grammar.y"
          {
              if (yystack_[0].value.as<BlockListIndex>()) {
                  cxt.graph.merge(yystack_[2].value.as<ArgumentListIndex>(), yystack_[0].value.as<BlockListIndex>());
                  cxt.graph.location(*yystack_[2].value.as<ArgumentListIndex>()) =
                      yylhs.location; // spans arguments and block list
              }
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode {}, yylhs.location, yystack_[5].value.as<SelectorIndex>(),
                             yystack_[2].value.as<ArgumentListIndex>());
          }
#line 3000 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 78: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN CLOSEPAREN block.list
#line 482 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode {}, yylhs.location, yystack_[4].value.as<SelectorIndex>(),
                             yystack_[0].value.as<BlockListIndex>());
          }
#line 3006 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 79: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN block.list
#line 485 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode {}, yylhs.location, yystack_[2].value.as<SelectorIndex>(),
                             cxt.create(ArgumentList {}, yylhs.location, yystack_[0].value.as<BlockListIndex>()));
          }
#line 3012 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 80: // msgsend: name OPENPAREN arguments CLOSEPAREN block.opt_list
#line 489 "langutils/sc_parser/src/sc_grammar.y"
          {
              if (yystack_[0].value.as<BlockListIndex>()) {
                  cxt.graph.merge(yystack_[2].value.as<ArgumentListIndex>(), yystack_[0].value.as<BlockListIndex>());
                  cxt.graph.location(*yystack_[2].value.as<ArgumentListIndex>()) = {
                      yystack_[2].location.begin, yystack_[0].location.end
                  }; // spans arguments and block list
              }
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode {}, yylhs.location, yystack_[4].value.as<NamedIdentifierIndex>(),
                             yystack_[2].value.as<ArgumentListIndex>());
          }
#line 3024 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 81: // msgsend: name OPENPAREN CLOSEPAREN block.list
#line 498 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode {}, yylhs.location, yystack_[3].value.as<NamedIdentifierIndex>(),
                             yystack_[0].value.as<BlockListIndex>());
          }
#line 3030 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 82: // msgsend: name block.list
#line 501 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode {}, yylhs.location, yystack_[1].value.as<NamedIdentifierIndex>(),
                             cxt.create(ArgumentList {}, yystack_[0].location, yystack_[0].value.as<BlockListIndex>()));
          }
#line 3036 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 83: // msgsend: expr DOT name arguments.maybe_paren block.opt_list
#line 504 "langutils/sc_parser/src/sc_grammar.y"
          {
              if (yystack_[0].value.as<BlockListIndex>()) {
                  cxt.graph.merge(yystack_[1].value.as<ArgumentListIndex>(), yystack_[0].value.as<BlockListIndex>());
                  cxt.graph.location(*yystack_[1].value.as<ArgumentListIndex>()) = {
                      yystack_[1].location.begin, yystack_[0].location.end
                  }; // spans arguments and block list
              }
              cxt.graph.prepend(yystack_[1].value.as<ArgumentListIndex>(),
                                yystack_[4].value.as<AnyExprIndex>()); // put the receiver in place
              cxt.graph.location(*yystack_[1].value.as<ArgumentListIndex>()) = {
                  yystack_[4].location.begin, yystack_[1].location.end
              }; // spans arguments and block list
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode {}, yylhs.location, yystack_[2].value.as<NamedIdentifierIndex>(),
                             yystack_[1].value.as<ArgumentListIndex>());
          }
#line 3050 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 84: // msgsend: expr DOT arguments.paren block.opt_list
#line 515 "langutils/sc_parser/src/sc_grammar.y"
          {
              if (yystack_[0].value.as<BlockListIndex>()) {
                  cxt.graph.merge(yystack_[1].value.as<ArgumentListIndex>(), yystack_[0].value.as<BlockListIndex>());
                  cxt.graph.location(*yystack_[1].value.as<ArgumentListIndex>()) = {
                      yystack_[1].location.begin, yystack_[0].location.end
                  }; // spans arguments and block list
              }
              cxt.graph.prepend(yystack_[1].value.as<ArgumentListIndex>(),
                                yystack_[3].value.as<AnyExprIndex>()); // put the receiver in place
              cxt.graph.location(*yystack_[1].value.as<ArgumentListIndex>()) = {
                  yystack_[3].location.begin, yystack_[1].location.end
              }; // spans arguments and block list
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode { MessageNode::SelectorMode::Value }, yylhs.location,
                             cxt.create(Missing {}, yystack_[2].location), yystack_[1].value.as<ArgumentListIndex>());
          }
#line 3064 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 85: // msgsend: expr DOT OPENPAREN CLOSEPAREN block.opt_list
#line 525 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto args = cxt.create(ArgumentList {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>());
              if (yystack_[0].value.as<BlockListIndex>())
                  cxt.graph.merge(args, yystack_[0].value.as<BlockListIndex>());
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode { MessageNode::SelectorMode::Value }, yylhs.location,
                             cxt.create(Missing {}, yystack_[3].location), args);
          }
#line 3074 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 86: // msgsend: CLASSNAME OPENSQUARE literal.array.contents CLOSESQUARE
#line 540 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = cxt.create(CollectionNode {}, yylhs.location,
                                                          cxt.create(ClassNameIdentifier {}, yystack_[3].location),
                                                          yystack_[1].value.as<ArrayIndex>());
          }
#line 3080 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 87: // msgsend: CLASSNAME block.list
#line 543 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto args =
                  cxt.create(ArgumentList {}, yylhs.location, cxt.create(NamedIdentifier {}, yystack_[1].location));
              cxt.graph.merge(args, yystack_[0].value.as<BlockListIndex>());
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode {}, yylhs.location, cxt.create(Missing {}, yystack_[1].location), args);
          }
#line 3090 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 88: // msgsend: CLASSNAME OPENPAREN arguments CLOSEPAREN block.opt_list
#line 549 "langutils/sc_parser/src/sc_grammar.y"
          {
              if (yystack_[0].value.as<BlockListIndex>()) {
                  cxt.graph.merge(yystack_[2].value.as<ArgumentListIndex>(), yystack_[0].value.as<BlockListIndex>());
                  cxt.graph.location(*yystack_[2].value.as<ArgumentListIndex>()) = {
                      yystack_[2].location.begin, yystack_[0].location.end
                  }; // spans arguments and block list
              }
              cxt.graph.prepend(yystack_[2].value.as<ArgumentListIndex>(),
                                cxt.create(ClassNameIdentifier {}, yystack_[4].location)); // put the receiver in place
              cxt.graph.location(*yystack_[2].value.as<ArgumentListIndex>()) = {
                  yystack_[2].location.begin, yystack_[0].location.end
              }; // spans arguments and block list
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode { MessageNode::SelectorMode::New }, yylhs.location,
                             cxt.create(Missing {}, yystack_[4].location), yystack_[2].value.as<ArgumentListIndex>());
          }
#line 3104 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 89: // msgsend: CLASSNAME OPENPAREN CLOSEPAREN block.opt_list
#line 560 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto args = cxt.create(ArgumentList {}, yylhs.location, yystack_[0].value.as<BlockListIndex>());
              cxt.graph.prepend(args,
                                cxt.create(ClassNameIdentifier {}, yystack_[3].location)); // put the receiver in place
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode { MessageNode::SelectorMode::New }, yylhs.location,
                             cxt.create(Missing {}, yystack_[3].location), args);
          }
#line 3114 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 90: // expr.base: literal
#line 572 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<AnyPossiblyLiteralIndex>();
          }
#line 3120 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 91: // expr.base: name
#line 574 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<NamedIdentifierIndex>();
          }
#line 3126 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 92: // expr.base: msgsend
#line 576 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 3132 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 93: // expr.base: OPENPAREN block.contents semicolon.opt CLOSEPAREN
#line 578 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto blk = cxt.create(BlockNode {}, yylhs.location, cxt.create(DeclareArgumentList {}, yylhs.location),
                                    yystack_[2].value.as<BlockContentsListIndex>());
              yylhs.value.as<AnyExprIndex>() = cxt.create(
                  MessageNode { MessageNode::SelectorMode::Value }, yylhs.location.flatten(),
                  cxt.create(Missing {}, yylhs.location.flatten()), cxt.create(ArgumentList {}, yylhs.location, blk));
          }
#line 3141 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 94: // expr.base: TILDE name
#line 582 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(EnvIdentifierNode {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3147 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 95: // expr.base: expr.base OPENSQUARE arguments CLOSESQUARE
#line 588 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.prepend(yystack_[1].value.as<ArgumentListIndex>(),
                                yystack_[3].value.as<AnyExprIndex>()); // put receiver in place.
              cxt.graph.location(*yystack_[1].value.as<ArgumentListIndex>()) = yylhs.location;
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode { MessageNode::SelectorMode::At }, yylhs.location,
                             cxt.create(Missing {}, yystack_[2].location), yystack_[1].value.as<ArgumentListIndex>());
          }
#line 3157 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 96: // expr.base: expr.base OPENSQUARE CLOSESQUARE
#line 595 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto args = cxt.create(ArgumentList {}, yylhs.location, yystack_[2].value.as<AnyExprIndex>());
              yylhs.value.as<AnyExprIndex>() = cxt.create(MessageNode { MessageNode::SelectorMode::At }, yylhs.location,
                                                          cxt.create(Missing {}, yystack_[1].location), args);
          }
#line 3166 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 97: // expr: expr.base
#line 604 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 3172 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 98: // expr: CLASSNAME
#line 608 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = cxt.create(ClassNameIdentifier {}, yylhs.location);
          }
#line 3178 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 99: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE
#line 611 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.prepend(yystack_[1].value.as<ArgumentListIndex>(),
                                yystack_[4].value.as<AnyExprIndex>()); // put receiver in place
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode { MessageNode::SelectorMode::At }, yylhs.location,
                             cxt.create(Missing {}, yylhs.location), yystack_[1].value.as<ArgumentListIndex>());
          }
#line 3187 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 100: // expr: expr DOT OPENSQUARE CLOSESQUARE
#line 616 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto args = cxt.create(ArgumentList {}, yylhs.location, yystack_[3].value.as<AnyExprIndex>());
              yylhs.value.as<AnyExprIndex>() = cxt.create(MessageNode { MessageNode::SelectorMode::At }, yylhs.location,
                                                          cxt.create(Missing {}, yystack_[1].location), args);
          }
#line 3196 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 101: // expr: BACKTICK expr
#line 621 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(ReferenceNode {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 3202 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 102: // expr: expr binary_op expr
#line 624 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto args = cxt.create(ArgumentList {}, yylhs.location, yystack_[2].value.as<AnyExprIndex>(),
                                     yystack_[0].value.as<AnyExprIndex>());
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(MessageNode {}, yylhs.location, yystack_[1].value.as<SelectorMaybeAdverbIndex>(), args);
          }
#line 3211 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 103: // expr: name EQUALSSIGN expr
#line 630 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentNode {}, yylhs.location, yystack_[2].value.as<NamedIdentifierIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3217 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 104: // expr: TILDE name EQUALSSIGN expr
#line 633 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentNode { AssignmentNode::Target::Environment }, yylhs.location,
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3223 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 105: // expr: expr DOT name EQUALSSIGN expr
#line 636 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(SetterNode {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3229 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 106: // expr: name OPENPAREN arguments CLOSEPAREN EQUALSSIGN expr
#line 639 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(SetterNode {}, yylhs.location, yystack_[3].value.as<ArgumentListIndex>(),
                             yystack_[5].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3235 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 107: // expr: HASH name.list comma.opt EQUALSSIGN expr
#line 642 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(VariablePack {}, yylhs.location, yystack_[3].value.as<NameListIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3241 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 108: // expr: HASH name.list comma.opt OPENPAREN expr.seq CLOSEPAREN
#line 644 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(VariablePack {}, yylhs.location, yystack_[4].value.as<NameListIndex>(),
                             yystack_[1].value.as<AnyExprIndex>());
          }
#line 3247 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 109: // expr: expr.base OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 650 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentAtNode {}, yylhs.location, yystack_[5].value.as<AnyExprIndex>(),
                             yystack_[3].value.as<ArgumentListIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3253 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 110: // expr: expr.base OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 652 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentAtNode {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             cxt.create(ArgumentList {}, yystack_[3].location), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3259 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 111: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 655 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentAtNode {}, yylhs.location, yystack_[6].value.as<AnyExprIndex>(),
                             yystack_[3].value.as<ArgumentListIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3265 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 112: // expr: expr DOT OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 658 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentAtNode {}, yylhs.location, yystack_[5].value.as<AnyExprIndex>(),
                             cxt.create(ArgumentList {}, yystack_[3].location), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3271 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 113: // expr.seq.base: expr
#line 662 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 3277 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 114: // expr.seq.base: expr.seq.base SEMICOLON expr
#line 664 "langutils/sc_parser/src/sc_grammar.y"
          {
              // This piece of logic is here because exprs can contain expr.seq, so we avoid creating the list node if
              // we can.
              if (auto expr_seq = cxt.graph.as_a<ExprSeqIndex>(*yystack_[2].value.as<AnyExprIndex>())) {
                  cxt.graph.append(*expr_seq, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
                  yylhs.value.as<AnyExprIndex>() = yystack_[2].value.as<AnyExprIndex>();
              } else {
                  yylhs.value.as<AnyExprIndex>() =
                      cxt.create(ExprSeq {}, yylhs.location, yystack_[2].value.as<AnyExprIndex>(),
                                 yystack_[0].value.as<AnyExprIndex>());
              }
          }
#line 3291 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 115: // expr.seq: expr.seq.base semicolon.opt
#line 675 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[1].value.as<AnyExprIndex>();
          }
#line 3297 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 116: // argument_declarations.list: name
#line 688 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.create(DeclareArgumentList {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3303 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 117: // argument_declarations.list: name EQUALSSIGN literal
#line 690 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.create(DeclareArgumentList {}, yylhs.location,
                             cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                        yystack_[2].value.as<NamedIdentifierIndex>(),
                                        yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3309 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 118: // argument_declarations.list: name OPENPAREN expr.seq CLOSEPAREN
#line 692 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(
                  DeclareArgumentList {}, yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[3].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3315 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 119: // argument_declarations.list: name EQUALSSIGN OPENPAREN expr.seq CLOSEPAREN
#line 694 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(
                  DeclareArgumentList {}, yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[4].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3321 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 120: // argument_declarations.list: argument_declarations.list COMMA name
#line 696 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3327 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 121: // argument_declarations.list: argument_declarations.list COMMA name EQUALSSIGN literal
#line 698 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[4].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                              yystack_[2].value.as<NamedIdentifierIndex>(),
                                              yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3333 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 122: // argument_declarations.list: argument_declarations.list COMMA name OPENPAREN expr.seq CLOSEPAREN
#line 700 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.graph.append(
                  yystack_[5].value.as<DeclareArgumentListIndex>(), yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[3].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3339 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 123: // argument_declarations.list: argument_declarations.list COMMA name EQUALSSIGN OPENPAREN expr.seq
                    // CLOSEPAREN
#line 702 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.graph.append(
                  yystack_[6].value.as<DeclareArgumentListIndex>(), yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[4].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3345 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 124: // argument_declarations.pipelist: name literal
#line 707 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.create(DeclareArgumentList {}, yylhs.location,
                             cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                        yystack_[1].value.as<NamedIdentifierIndex>(),
                                        yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3351 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 125: // argument_declarations.pipelist: name
#line 709 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.create(DeclareArgumentList {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3357 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 126: // argument_declarations.pipelist: name EQUALSSIGN literal
#line 711 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.create(DeclareArgumentList {}, yylhs.location,
                             cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                        yystack_[2].value.as<NamedIdentifierIndex>(),
                                        yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3363 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 127: // argument_declarations.pipelist: name OPENPAREN expr.seq CLOSEPAREN
#line 713 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(
                  DeclareArgumentList {}, yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[3].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3369 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 128: // argument_declarations.pipelist: name EQUALSSIGN OPENPAREN expr.seq CLOSEPAREN
#line 715 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(
                  DeclareArgumentList {}, yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[4].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3375 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 129: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name literal
#line 717 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[3].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                              yystack_[1].value.as<NamedIdentifierIndex>(),
                                              yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3381 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 130: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name
#line 719 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3387 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 131: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN literal
#line 721 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[4].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                              yystack_[2].value.as<NamedIdentifierIndex>(),
                                              yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3393 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 132: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name OPENPAREN expr.seq
                    // CLOSEPAREN
#line 723 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.graph.append(
                  yystack_[5].value.as<DeclareArgumentListIndex>(), yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[3].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3399 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 133: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN
                    // OPENPAREN expr.seq CLOSEPAREN
#line 725 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.graph.append(
                  yystack_[6].value.as<DeclareArgumentListIndex>(), yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[4].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3405 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 134: // argument_declarations: ARG SEMICOLON
#line 729 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(DeclareArgumentList {}, yylhs.location);
          }
#line 3411 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 135: // argument_declarations: ARG argument_declarations.list comma.opt SEMICOLON
#line 730 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = yystack_[2].value.as<DeclareArgumentListIndex>();
          }
#line 3417 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 136: // argument_declarations: ARG argument_declarations.list ELLIPSIS name SEMICOLON
#line 732 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[3].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3423 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 137: // argument_declarations: ARG argument_declarations.list ELLIPSIS name COMMA name SEMICOLON
#line 734 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[5].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[3].location,
                                              yystack_[3].value.as<NamedIdentifierIndex>()),
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3429 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 138: // argument_declarations: ARG ELLIPSIS name SEMICOLON
#line 736 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto arg_list = cxt.create(DeclareArgumentList {}, yylhs.location);
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(arg_list, yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3438 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 139: // argument_declarations: ARG ELLIPSIS name COMMA name SEMICOLON
#line 741 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto arg_list = cxt.create(DeclareArgumentList {}, yylhs.location);
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(arg_list, yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[3].location,
                                              yystack_[3].value.as<NamedIdentifierIndex>()),
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3451 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 140: // argument_declarations: PIPE PIPE
#line 750 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(DeclareArgumentList {}, yylhs.location);
          }
#line 3457 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 141: // argument_declarations: PIPE argument_declarations.pipelist comma.opt PIPE
#line 752 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = yystack_[2].value.as<DeclareArgumentListIndex>();
          }
#line 3463 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 142: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name PIPE
#line 754 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[3].value.as<DeclareArgumentListIndex>(), yystack_[3].location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3469 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 143: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name COMMA name PIPE
#line 756 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[5].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[3].location,
                                              yystack_[3].value.as<NamedIdentifierIndex>()),
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3475 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 144: // argument_declarations: PIPE ELLIPSIS name PIPE
#line 758 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto arg_list = cxt.create(DeclareArgumentList {}, yylhs.location);
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(arg_list, yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3484 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 145: // argument_declarations: PIPE ELLIPSIS name COMMA name PIPE
#line 763 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto arg_list = cxt.create(DeclareArgumentList {}, yylhs.location);
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(arg_list, yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[3].location,
                                              yystack_[3].value.as<NamedIdentifierIndex>()),
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3497 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 146: // argument_declarations.opt: %empty
#line 774 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(DeclareArgumentList {}, yylhs.location);
          }
#line 3503 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 147: // argument_declarations.opt: argument_declarations
#line 775 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = yystack_[0].value.as<DeclareArgumentListIndex>();
          }
#line 3509 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 148: // arguments.entries: KEYBINOP expr.seq
#line 779 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentEntryIndex>() =
                  cxt.create(KwArgNode {}, yylhs.location,
                             cxt.create(SymbolNode { SymbolNode::Kind::KeyBinOp }, yystack_[1].location),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3515 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 149: // arguments.entries: MULTIPLY expr.seq
#line 780 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentEntryIndex>() =
                  cxt.create(VariadicArgNode {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 3521 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 150: // arguments.entries: expr.seq
#line 781 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentEntryIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 3527 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 151: // arguments.no_trailing: arguments.entries
#line 785 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() =
                  cxt.create(ArgumentList {}, yylhs.location, yystack_[0].value.as<ArgumentEntryIndex>());
          }
#line 3533 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 152: // arguments.no_trailing: arguments.no_trailing COMMA arguments.entries
#line 786 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<ArgumentListIndex>(), yylhs.location,
                                   yystack_[0].value.as<ArgumentEntryIndex>());
          }
#line 3539 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 153: // arguments: arguments.no_trailing comma.opt
#line 789 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = yystack_[1].value.as<ArgumentListIndex>();
          }
#line 3545 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 154: // arguments.paren: OPENPAREN arguments CLOSEPAREN
#line 791 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = yystack_[1].value.as<ArgumentListIndex>();
          }
#line 3551 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 155: // arguments.maybe_paren: %empty
#line 794 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = cxt.create(ArgumentList {}, yylhs.location);
          }
#line 3557 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 156: // arguments.maybe_paren: OPENPAREN CLOSEPAREN
#line 796 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = cxt.create(ArgumentList {}, yylhs.location);
          }
#line 3563 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 157: // arguments.maybe_paren: arguments.paren
#line 797 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = yystack_[0].value.as<ArgumentListIndex>();
          }
#line 3569 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 158: // variable_declarations.list.item: name
#line 806 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() = yystack_[0].value.as<NamedIdentifierIndex>();
          }
#line 3575 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 159: // variable_declarations.list.item: name EQUALSSIGN expr
#line 808 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() =
                  cxt.create(DeclareVariableWithDefaultNode {}, yylhs.location,
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3581 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 160: // variable_declarations.list.item: name OPENPAREN expr.seq CLOSEPAREN
#line 810 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() =
                  cxt.create(DeclareVariableWithDefaultNode {}, yylhs.location,
                             yystack_[3].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>());
          }
#line 3587 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 161: // variable_declarations.list.item: HASH name.list comma.opt EQUALSSIGN expr
#line 814 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() =
                  cxt.create(VariablePack {}, yylhs.location, yystack_[3].value.as<NameListIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3593 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 162: // variable_declarations.list.item: HASH name.list comma.opt OPENPAREN expr.seq CLOSEPAREN
#line 816 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() =
                  cxt.create(VariablePack {}, yylhs.location, yystack_[4].value.as<NameListIndex>(),
                             yystack_[1].value.as<AnyExprIndex>());
          }
#line 3599 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 163: // variable_declarations.list: variable_declarations.list.item
#line 821 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareVariableListIndex>() =
                  cxt.create(DeclareVariableList {}, yylhs.location, yystack_[0].value.as<DeclareAnyVariableIndex>());
          }
#line 3605 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 164: // variable_declarations.list: variable_declarations.list COMMA variable_declarations.list.item
#line 823 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareVariableListIndex>() = cxt.graph.append(
                  yystack_[2].value.as<DeclareVariableListIndex>(), yystack_[0].value.as<DeclareAnyVariableIndex>());
          }
#line 3611 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 165: // variable_declarations: VAR variable_declarations.list
#line 826 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareVariableListIndex>() = yystack_[0].value.as<DeclareVariableListIndex>();
          }
#line 3617 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 166: // literal.terminal: symbol
#line 834 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<SymbolLitIndex>();
          }
#line 3623 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 167: // literal.terminal: string
#line 835 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<StringLitIndex>();
          }
#line 3629 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 168: // literal.terminal: integer
#line 836 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<IntLitIndex>();
          }
#line 3635 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 169: // literal.terminal: float
#line 837 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<FloatProducingIndex>();
          }
#line 3641 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 170: // literal.terminal: boolean
#line 838 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<BooleanLitIndex>();
          }
#line 3647 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 171: // literal.terminal: nil
#line 839 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<NilLitIndex>();
          }
#line 3653 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 172: // literal.terminal: ascii
#line 840 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<ASCIIIndex>();
          }
#line 3659 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 173: // literal.terminal: block
#line 841 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<BlockIndex>();
          }
#line 3665 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 174: // literal.array.contents: %empty
#line 846 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() = cxt.create(ArrayNode {}, yylhs.location);
          }
#line 3671 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 175: // literal.array.contents: expr.seq
#line 848 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.create(ArrayNode {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 3677 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 176: // literal.array.contents: expr.seq COLON expr.seq
#line 850 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.create(ArrayNode {}, yylhs.location, yystack_[2].value.as<AnyExprIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3683 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 177: // literal.array.contents: KEYBINOP expr.seq
#line 852 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.create(ArrayNode {}, yylhs.location,
                             cxt.create(SymbolNode { SymbolNode::Kind::KeyBinOp }, yystack_[1].location),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3689 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 178: // literal.array.contents: literal.array.contents COMMA expr.seq
#line 854 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() = cxt.graph.append(yystack_[2].value.as<ArrayIndex>(), yylhs.location,
                                                              yystack_[0].value.as<AnyExprIndex>());
          }
#line 3695 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 179: // literal.array.contents: literal.array.contents COMMA expr.seq COLON expr.seq
#line 856 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.graph.append(yystack_[4].value.as<ArrayIndex>(), yylhs.location,
                                   yystack_[2].value.as<AnyExprIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3701 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 180: // literal.array.contents: literal.array.contents COMMA KEYBINOP expr.seq
#line 858 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.graph.append(yystack_[3].value.as<ArrayIndex>(), yylhs.location,
                                   cxt.create(SymbolNode { SymbolNode::Kind::KeyBinOp }, yystack_[1].location),
                                   yystack_[0].value.as<AnyExprIndex>());
          }
#line 3707 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 181: // literal.dictionary.entry: expr COLON expr.seq
#line 866 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryEntryIndex>() =
                  cxt.create(DictionaryEntryNode {}, yylhs.location, yystack_[2].value.as<AnyExprIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3713 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 182: // literal.dictionary.entry: KEYBINOP expr.seq
#line 868 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryEntryIndex>() =
                  cxt.create(DictionaryEntryNode {}, yylhs.location,
                             cxt.create(SymbolNode { SymbolNode::Kind::KeyBinOp }, yystack_[1].location),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3719 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 183: // literal.dictionary.entries: %empty
#line 873 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryIndex>() = cxt.create(DictionaryNode {}, yylhs.location);
          }
#line 3725 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 184: // literal.dictionary.entries: literal.dictionary.entry
#line 875 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryIndex>() =
                  cxt.create(DictionaryNode {}, yylhs.location, yystack_[0].value.as<DictionaryEntryIndex>());
          }
#line 3731 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 185: // literal.dictionary.entries: literal.dictionary.entries COMMA literal.dictionary.entry
#line 877 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryIndex>() =
                  cxt.graph.append(yystack_[2].value.as<DictionaryIndex>(), yylhs.location,
                                   yystack_[0].value.as<DictionaryEntryIndex>());
          }
#line 3737 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 186: // literal.dictionary: OPENPAREN literal.dictionary.entries comma.opt CLOSEPAREN
#line 882 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.location(*yystack_[2].value.as<DictionaryIndex>()) = yylhs.location;
              yylhs.value.as<DictionaryIndex>() = yystack_[2].value.as<DictionaryIndex>();
          }
#line 3743 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 187: // literal.array: OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 887 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.location(*yystack_[2].value.as<ArrayIndex>()) = yylhs.location;
              yylhs.value.as<ArrayIndex>() = yystack_[2].value.as<ArrayIndex>();
          }
#line 3749 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 188: // literal.array: HASH OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 889 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.payload(yystack_[2].value.as<ArrayIndex>()).is_immutable = true;
              cxt.graph.location(*yystack_[2].value.as<ArrayIndex>()) = yylhs.location;
              yylhs.value.as<ArrayIndex>() = yystack_[2].value.as<ArrayIndex>();
          }
#line 3759 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 189: // literal: literal.terminal
#line 897 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<AnyPossiblyLiteralIndex>();
          }
#line 3765 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 190: // literal: literal.array
#line 898 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<ArrayIndex>();
          }
#line 3771 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 191: // literal: literal.dictionary
#line 899 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<DictionaryIndex>();
          }
#line 3777 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 192: // name: NAME
#line 906 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NamedIdentifierIndex>() = cxt.create(NamedIdentifier {}, yylhs.location);
          }
#line 3783 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 193: // name.list: name
#line 911 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NameListIndex>() =
                  cxt.create(NameList {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3789 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 194: // name.list: TILDE name
#line 913 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NameListIndex>() = cxt.create(
                  NameList {}, yylhs.location,
                  cxt.create(EnvIdentifierNode {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>()));
          }
#line 3795 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 195: // name.list: name.list COMMA name
#line 915 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NameListIndex>() = cxt.graph.append(yystack_[2].value.as<NameListIndex>(), yylhs.location,
                                                                 yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3801 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 196: // name.list: name.list COMMA TILDE name
#line 917 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NameListIndex>() = cxt.graph.append(
                  yystack_[3].value.as<NameListIndex>(), yylhs.location,
                  cxt.create(EnvIdentifierNode {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>()));
          }
#line 3807 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 197: // ascii: ASCII
#line 927 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ASCIIIndex>() = cxt.create(ASCIINode {}, yylhs.location);
          }
#line 3813 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 198: // nil: NIL
#line 929 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NilLitIndex>() = cxt.create(NilNode {}, yylhs.location);
          }
#line 3819 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 199: // boolean: TRUE
#line 932 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BooleanLitIndex>() = cxt.create(BooleanNode { true }, yylhs.location);
          }
#line 3825 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 200: // boolean: FALSE
#line 933 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BooleanLitIndex>() = cxt.create(BooleanNode { false }, yylhs.location);
          }
#line 3831 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 201: // symbol: SYMBOL_QUOTE
#line 937 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SymbolLitIndex>() = cxt.create(SymbolNode { SymbolNode::Kind::Quote }, yylhs.location);
          }
#line 3837 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 202: // symbol: SYMBOL_SLASH
#line 938 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SymbolLitIndex>() = cxt.create(SymbolNode { SymbolNode::Kind::Slash }, yylhs.location);
          }
#line 3843 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 203: // string: STRINGLINE
#line 942 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<StringLitIndex>() =
                  cxt.create(StringLineList {}, yylhs.location, cxt.create(StringLineNode {}, yylhs.location));
          }
#line 3849 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 204: // string: string STRINGLINE
#line 943 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<StringLitIndex>() = cxt.graph.append(yystack_[1].value.as<StringLitIndex>(),
                                                                  cxt.create(StringLineNode {}, yystack_[0].location));
          }
#line 3855 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 205: // integer: INTEGER
#line 947 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<IntLitIndex>() = cxt.create(IntNode {}, yylhs.location);
          }
#line 3861 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 206: // integer: INTEGER_RADIX
#line 948 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<IntLitIndex>() = cxt.create(IntNode { IntNode::Kind::Radix }, yylhs.location);
          }
#line 3867 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 207: // integer: HEXADECIMAL
#line 949 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<IntLitIndex>() = cxt.create(IntNode { IntNode::Kind::Hexadecimal }, yylhs.location);
          }
#line 3873 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 208: // integer: MINUS integer
#line 951 "langutils/sc_parser/src/sc_grammar.y"
          {
              // Reaches into the previous integer and changes its sign.
              cxt.graph.payload(yystack_[0].value.as<IntLitIndex>()).sign = IntNode::Sign::Negative;
              yylhs.value.as<IntLitIndex>() = yystack_[0].value.as<IntLitIndex>();
          }
#line 3883 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 209: // float.raw_unsigned: FLOAT
#line 959 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = cxt.create(FloatNode {}, yylhs.location);
          }
#line 3889 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 210: // float.raw_unsigned: FLOAT_RADIX
#line 960 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = cxt.create(FloatNode { FloatNode::Kind::Radix }, yylhs.location);
          }
#line 3895 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 211: // float.raw_unsigned: FLOAT_EXPONENT
#line 961 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = cxt.create(FloatNode { FloatNode::Kind::Exponent }, yylhs.location);
          }
#line 3901 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 212: // float.raw_unsigned: FLOAT_INF
#line 962 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = cxt.create(FloatNode { FloatNode::Kind::Inf }, yylhs.location);
          }
#line 3907 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 213: // float.raw: float.raw_unsigned
#line 967 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = yystack_[0].value.as<FloatLitIndex>();
          }
#line 3913 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 214: // float.raw: MINUS float.raw_unsigned
#line 969 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.payload(yystack_[0].value.as<FloatLitIndex>()).sign = FloatNode::Sign::Negative;
              yylhs.value.as<FloatLitIndex>() = yystack_[0].value.as<FloatLitIndex>();
          }
#line 3919 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 215: // accidental.unsigned: ACCIDENTAL_STEPS
#line 974 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AccidentalIndex>() =
                  cxt.create(AccidentalNode { AccidentalNode::Kind::Steps }, yylhs.location);
          }
#line 3925 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 216: // accidental.unsigned: ACCIDENTAL_CENTS
#line 976 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AccidentalIndex>() =
                  cxt.create(AccidentalNode { AccidentalNode::Kind::Cents }, yylhs.location);
          }
#line 3931 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 217: // accidental: accidental.unsigned
#line 981 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AccidentalIndex>() = yystack_[0].value.as<AccidentalIndex>();
          }
#line 3937 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 218: // accidental: MINUS accidental.unsigned
#line 983 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.payload(yystack_[0].value.as<AccidentalIndex>()).sign = AccidentalNode::Sign::Negative;
              yylhs.value.as<AccidentalIndex>() = yystack_[0].value.as<AccidentalIndex>();
          }
#line 3943 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 219: // float: float.raw
#line 987 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() = yystack_[0].value.as<FloatLitIndex>();
          }
#line 3949 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 220: // float: accidental
#line 988 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() = yystack_[0].value.as<AccidentalIndex>();
          }
#line 3955 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 221: // float: float.raw PI
#line 989 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() =
                  cxt.create(PiNode {}, yylhs.location, yystack_[1].value.as<FloatLitIndex>());
          }
#line 3961 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 222: // float: integer PI
#line 990 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() =
                  cxt.create(PiNode {}, yylhs.location, yystack_[1].value.as<IntLitIndex>());
          }
#line 3967 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 223: // float: PI
#line 991 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() =
                  cxt.create(PiNode {}, yylhs.location, cxt.create(Missing {}, yylhs.location));
          }
#line 3973 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 224: // float: MINUS PI
#line 992 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() =
                  cxt.create(PiNode { PiNode::Sign::Negative }, yylhs.location, cxt.create(Missing {}, yylhs.location));
          }
#line 3979 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 225: // accessor: %empty
#line 996 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RWAccessorIndex>() = cxt.create(RWAccessor { ReadWriteAccessor::Private }, yylhs.location);
          }
#line 3985 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 226: // accessor: LESSTHAN
#line 997 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RWAccessorIndex>() =
                  cxt.create(RWAccessor { ReadWriteAccessor::ExternallyReadable }, yylhs.location);
          }
#line 3991 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 227: // accessor: GREATERTHAN
#line 998 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RWAccessorIndex>() =
                  cxt.create(RWAccessor { ReadWriteAccessor::ExternallyWriteable }, yylhs.location);
          }
#line 3997 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 228: // accessor: READWRITEVAR
#line 1000 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RWAccessorIndex>() = cxt.create(
                  RWAccessor {
                      combine<ReadWriteAccessor::ExternallyReadable, ReadWriteAccessor::ExternallyWriteable>() },
                  yylhs.location);
          }
#line 4007 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;


#line 4011 "langutils/sc_parser/src/sc_grammar_parser.cpp"

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
                                              "expr.seq.base",
                                              "expr.seq",
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


  const short parser::yypact_ninf_ = -212;

  const short parser::yytable_ninf_ = -230;

  const short parser::yypact_[] = {
      73,   47,   399,  5,    45,   192,  12,   -212, -212, -212, 54,   49,   -212, -212, 1255, 508,  37,   54,   -212,
      -212, -212, -212, -212, -212, -212, -212, -212, -212, -212, -212, -212, -212, 32,   -212, -212, -212, -212, -212,
      1712, 1399, 83,   -212, -212, 100,  1703, -212, -212, -212, -212, 24,   -212, -212, -212, -212, 88,   102,  -212,
      112,  -212, -212, -212, 144,  -212, -212, 1447, 1447, 170,  -212, -212, 118,  164,  193,  562,  1399, 1703, 189,
      154,  196,  1399, 11,   109,  -212, 1399, 1712, -212, -212, -212, -212, 8,    -212, 150,  -212, -212, 204,  1690,
      667,  -212, -212, 203,  1255, 54,   -212, 208,  174,  1255, 456,  -212, 108,  -212, 138,  -212, -212, -212, -212,
      8,    -212, 716,  764,  -212, -212, -212, 190,  163,  1399, 813,  1399, 108,  -212, -212, -212, 129,  508,  -212,
      1703, -212, -212, -212, -212, 172,  -212, 1399, -212, 1399, 1303, 216,  1703, 153,  -212, 212,  39,   -212, 54,
      10,   60,   -212, 54,   -212, 90,   1491, 1158, 1399, 219,  13,   1399, -212, 150,  1703, 1351, 221,  196,  -212,
      173,  86,   1399, 115,  108,  1399, 1399, -212, -212, 220,  225,  -212, -212, 150,  191,  230,  862,  911,  108,
      97,   41,   -212, 180,  108,  231,  1703, 211,  237,  -212, -212, 129,  240,  -212, -212, 960,  122,  122,  122,
      -212, 232,  129,  1703, -212, 1399, 200,  -212, 208,  11,   1399, 1399, 121,  54,   54,   238,  1399, 1565, 21,
      -212, 54,   19,   1351, 245,  1602, -212, 1399, -212, 1703, -212, 1008, 108,  -212, 243,  1399, 1690, -212, -212,
      246,  54,   -212, 1399, 1399, 1703, -212, 1303, -212, 108,  -212, -212, 1206, -212, 108,  252,  1399, 210,  223,
      265,  108,  250,  -212, 1057, 1399, -212, 108,  1399, -212, -212, 108,  42,   -212, 83,   -212, -212, -212, 201,
      -212, -212, -212, -212, 261,  11,   261,  261,  172,  -212, 270,  -212, 1399, 99,   -212, 267,  1703, -212, 54,
      105,  125,  -212, 269,  1351, -212, 54,   -212, 29,   -212, 1528, 1690, 271,  1351, -212, 1703, 108,  272,  -212,
      -212, -212, 274,  1703, -212, -212, -212, 1703, 1399, 1399, 236,  -212, -212, -212, 1703, -212, 276,  1399, -212,
      618,  -212, 1158, 122,  -212, -212, -212, -212, 1399, 1399, -212, 277,  1399, 1639, -212, 54,   -212, 279,  233,
      54,   -212, 1351, 1676, -212, -212, 282,  108,  108,  -212, 1703, 1703, 1399, -212, 1703, -212, 283,  150,  -212,
      285,  1703, -212, 287,  1351, -212, 288,  -212, -212, 244,  290,  1351, -212, -212, -212, 1703, -212, 1109, 296,
      -212, -212, 303,  -212, -212, -212, 304,  -212, 150,  -212, -212, -212, 300,  -212
  };

  const unsigned char parser::yydefact_[] = {
      4,   22,  0,   0,   0,   0,   0,   15,  17,  18,  0,   20,  9,   47,  174, 183, 0,   0,   192, 205, 206, 207, 209,
      210, 211, 212, 215, 216, 201, 202, 203, 197, 98,  198, 199, 200, 223, 48,  0,   0,   146, 173, 92,  97,  8,   189,
      191, 190, 90,  91,  172, 171, 170, 166, 167, 168, 213, 219, 217, 220, 169, 0,   1,   14,  0,   230, 0,   3,   16,
      0,   0,   0,   183, 0,   113, 229, 175, 231, 0,   0,   0,   65,  74,  69,  67,  68,  70,  71,  72,  66,  229, 55,
      73,  0,   60,  0,   61,  184, 231, 174, 0,   193, 231, 94,  174, 0,   53,  87,  224, 0,   208, 214, 218, 101, 0,
      147, 0,   0,   74,  69,  72,  0,   76,  0,   0,   0,   82,  204, 222, 221, 45,  183, 13,  5,   12,  2,   23,  21,
      33,  177, 230, 115, 0,   232, 0,   59,  0,   163, 165, 158, 134, 0,   231, 116, 182, 0,   140, 231, 125, 230, 0,
      0,   0,   0,   11,  229, 60,  232, 0,   231, 194, 232, 0,   0,   0,   51,  0,   0,   150, 151, 231, 0,   54,  50,
      229, 96,  0,   0,   0,   51,  155, 0,   75,  102, 0,   0,   103, 70,  0,   41,  43,  46,  0,   36,  35,  0,   225,
      225, 225, 31,  229, 45,  114, 176, 0,   178, 187, 231, 0,   0,   0,   0,   232, 0,   0,   0,   0,   0,   232, 0,
      0,   183, 0,   0,   124, 0,   56,  57,  93,  0,   79,  181, 0,   0,   0,   185, 186, 0,   0,   195, 0,   0,   104,
      86,  0,   89,  52,  148, 149, 232, 153, 51,  0,   0,   95,  100, 0,   51,  0,   84,  0,   0,   157, 51,  0,   62,
      63,  81,  51,  42,  146, 44,  24,  7,   0,   226, 227, 228, 26,  29,  0,   28,  30,  230, 34,  0,   180, 0,   0,
      164, 0,   159, 138, 0,   120, 0,   135, 0,   183, 117, 0,   144, 0,   141, 130, 113, 0,   183, 126, 58,  0,   0,
      10,  188, 196, 0,   107, 152, 88,  49,  110, 0,   0,   99,  85,  154, 156, 105, 83,  0,   0,   80,  0,   6,   0,
      225, 25,  32,  19,  179, 0,   0,   160, 0,   0,   0,   136, 0,   118, 0,   0,   0,   142, 183, 0,   129, 127, 0,
      78,  51,  108, 109, 112, 0,   64,  106, 38,  229, 229, 27,  0,   161, 139, 0,   183, 121, 0,   119, 145, 0,   0,
      183, 131, 128, 77,  111, 230, 0,   0,   162, 122, 0,   137, 143, 132, 0,   40,  229, 37,  123, 133, 0,   39
  };

  const short parser::yypgoto_[] = {
      -212, -212, 214,  -212, -212, 286,  -212, -212, -212, -212, -27,  9,    23,   -212, -212, -212, 123,  124,
      -212, 110,  -212, 120,  -178, -30,  -87,  -154, -212, -100, -11,  -212, -212, -212, -2,   -212, 0,    -212,
      -212, -5,   43,   63,   -212, -99,  134,  -212, -211, -212, -212, -212, -60,  152,  -212, -212, -212, -155,
      156,  182,  -212, -212, -212, -212, -212, -29,  -23,  -212, -18,  -212, -212, -212, -74,  -71
  };

  const short parser::yydefgoto_[] = { 0,   4,   132, 5,   6,   7,   8,   71,  11,  9,   288, 289, 209, 210,
                                       211, 198, 199, 200, 201, 202, 40,  41,  255, 256, 90,  91,  192, 92,
                                       122, 123, 42,  43,  74,  75,  178, 152, 157, 115, 116, 179, 180, 268,
                                       189, 273, 147, 148, 96,  45,  77,  97,  98,  46,  47,  48,  49,  102,
                                       50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  290, 66,  144 };

  const short parser::yytable_[] = {
      44,  141, 107, 234, 93,  236, 181, 299, 165, 110, 95,  269, 67,  94,  76,  111, 161,  13,  186, 126, 112, 239,
      222, 18,  146, 195, 18,  168, 13,  184, 203, 172, 124, 310, 18,  61,  13,  113, 104,  169, 105, 361, 1,   99,
      174, 62,  13,  219, 155, 274, 223, 100, 18,  10,  110, 37,  18,  19,  20,  21,  111,  93,  133, 133, 156, 112,
      37,  3,   225, 18,  94,  309, 125, 139, 37,  313, 145, 311, 318, 346, 110, 224, 154,  328, 37,  362, 230, 220,
      266, 334, 340, 242, 109, 166, 250, 338, 70,  203, 247, 76,  341, 203, 228, 1,   76,   270, 117, 350, 226, 260,
      262, 203, 13,  354, 166, 127, 80,  2,   284, 150, 93,  193, 253, 196, 18,  136, 205,  254, 3,   94,  229, 302,
      240, 303, 251, 356, 294, 357, 212, 114, 321, 128, 213, 215, 18,  271, 298, 351, 130,  151, 37,  129, 106, 355,
      19,  20,  21,  166, 237, 365, 159, 160, 276, 241, 277, 244, 69,  100, 18,  106, 135,  252, 101, 103, 285, 286,
      257, 258, 81,  287, 119, 84,  85,  197, 87,  120, 89,  248, 18,  109, 236, 394, -229, 63,  137, 64,  187, 138,
      188, 140, 385, 142, 65,  166, 206, 18,  207, 208, 143, 392, 343, 344, 160, 162, 296,  167, 291, 292, 301, 300,
      171, 191, 173, 216, 218, 307, 18,  182, 238, 315, 246, 316, 259, 319, 261, 149, 153,  264, 121, 263, 278, 280,
      293, 154, 158, 282, 182, 297, 306, 326, 325, 99,  322, 323, 215, 378, 170, 329, 331,  335, 81,  330, 119, 84,
      85,  86,  87,  120, 89,  337, 158, 332, 333, 345, 339, 348, 352, 190, 358, 134, 366,  369, 106, 370, 373, 374,
      204, 382, 387, 388, 368, 393, 68,  396, 399, 106, 400, 349, 402, 404, 403, 408, 101,  397, 398, 412, 315, 221,
      359, 106, 407, 227, 409, 410, 106, 315, 347, 367, 379, 245, 279, 295, 327, 342, 272,  281, 0,   249, 217, 371,
      372, 0,   0,   411, 0,   0,   0,   0,   375, 0,   166, 0,   166, 0,   0,   0,   0,    275, 0,   381, 380, 0,
      0,   204, 383, 0,   0,   204, 0,   0,   182, 315, 0,   390, 0,   0,   0,   204, 0,    0,   0,   395, 0,   0,
      149, 0,   182, 0,   304, 305, 0,   106, 315, 0,   401, 312, 314, 106, 0,   315, 0,    405, 0,   106, 0,   166,
      0,   182, 106, 0,   12,  0,   0,   13,  324, 14,  0,   15,  0,   0,   0,   0,   16,   17,  18,  19,  20,  21,
      22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  0,   32,  0,   0,   0,   0,   0,    33,  34,  35,  36,  0,
      106, 37,  0,   0,   0,   0,   149, 0,   0,   0,   38,  0,   0,   0,   0,   0,   0,    0,   39,  353, 13,  0,
      14,  0,   72,  175, 360, 0,   0,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,   26,  27,  28,  29,  30,
      31,  0,   32,  0,   182, 106, 0,   0,   33,  34,  35,  36,  0,   0,   37,  0,   0,    0,   0,   0,   0,   0,
      176, 38,  0,   0,   177, 0,   13,  386, 14,  39,  72,  389, 0,   78,  0,   16,  17,   18,  19,  20,  21,  22,
      23,  24,  25,  26,  27,  28,  29,  30,  31,  0,   32,  0,   79,  80,  0,   0,   33,   34,  35,  36,  0,   0,
      37,  0,   0,   0,   0,   0,   0,   81,  82,  83,  84,  85,  86,  87,  88,  89,  13,   39,  14,  0,   72,  0,
      0,   78,  0,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,   30,  31,  0,   32,  0,
      79,  0,   0,   0,   33,  34,  35,  36,  0,   0,   37,  0,   0,   0,   0,   0,   0,    81,  82,  83,  84,  85,
      86,  87,  120, 89,  0,   39,  13,  376, 14,  0,   72,  0,   0,   78,  0,   16,  17,   18,  19,  20,  21,  22,
      23,  24,  25,  26,  27,  28,  29,  30,  31,  377, 32,  0,   79,  0,   0,   0,   33,   34,  35,  36,  0,   0,
      37,  0,   0,   0,   0,   0,   0,   0,   0,   38,  0,   13,  0,   14,  0,   72,  164,  39,  78,  0,   16,  17,
      18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  0,   32,  0,    79,  0,   0,   0,   33,
      34,  35,  36,  0,   0,   37,  0,   0,   0,   0,   0,   0,   0,   0,   38,  0,   13,   183, 14,  0,   72,  0,
      39,  78,  0,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,   30,  31,  0,   32,  0,
      79,  0,   0,   0,   33,  34,  35,  36,  0,   0,   37,  0,   0,   0,   0,   0,   0,    0,   0,   38,  13,  0,
      14,  185, 72,  0,   0,   39,  0,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,   26,  27,  28,  29,  30,
      31,  0,   32,  0,   0,   0,   0,   0,   33,  34,  35,  36,  0,   0,   37,  0,   0,    0,   0,   0,   0,   0,
      176, 38,  0,   13,  177, 14,  0,   72,  194, 39,  0,   0,   16,  17,  18,  19,  20,   21,  22,  23,  24,  25,
      26,  27,  28,  29,  30,  31,  0,   32,  0,   0,   0,   0,   0,   33,  34,  35,  36,   0,   0,   37,  0,   0,
      0,   0,   0,   0,   0,   176, 38,  0,   13,  177, 14,  265, 72,  0,   39,  0,   0,    16,  17,  18,  19,  20,
      21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  0,   32,  0,   0,   0,   0,    0,   33,  34,  35,  36,
      0,   0,   37,  0,   0,   0,   0,   0,   0,   0,   176, 38,  0,   13,  177, 14,  0,    72,  267, 39,  0,   0,
      16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  0,    32,  0,   0,   0,   0,
      0,   33,  34,  35,  36,  0,   0,   37,  0,   0,   0,   0,   0,   0,   0,   176, 38,   0,   13,  177, 14,  0,
      72,  283, 39,  78,  0,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,   28,  29,  30,  31,  0,
      32,  0,   79,  0,   0,   0,   33,  34,  35,  36,  0,   0,   37,  0,   0,   0,   0,    0,   0,   0,   0,   38,
      13,  0,   14,  0,   72,  320, 0,   39,  0,   16,  17,  18,  19,  20,  21,  22,  23,   24,  25,  26,  27,  28,
      29,  30,  31,  0,   32,  0,   0,   0,   0,   0,   33,  34,  35,  36,  0,   0,   37,   0,   0,   0,   0,   0,
      0,   0,   176, 38,  0,   13,  177, 14,  0,   72,  336, 39,  0,   0,   16,  17,  18,   19,  20,  21,  22,  23,
      24,  25,  26,  27,  28,  29,  30,  31,  0,   32,  0,   0,   0,   0,   0,   33,  34,   35,  36,  0,   0,   37,
      0,   0,   0,   0,   0,   0,   0,   176, 38,  0,   0,   177, 0,   13,  406, 14,  39,   72,  0,   0,   78,  0,
      16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  0,    32,  0,   79,  0,   0,
      0,   33,  34,  35,  36,  0,   0,   37,  0,   0,   0,   0,   0,   0,   0,   0,   38,   0,   13,  0,   14,  0,
      72,  0,   39,  235, 0,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,   28,  29,  30,  31,  0,
      32,  0,   79,  0,   0,   0,   33,  34,  35,  36,  0,   0,   37,  0,   0,   0,   0,    0,   0,   0,   0,   38,
      13,  0,   14,  0,   72,  0,   0,   39,  0,   16,  17,  18,  19,  20,  21,  22,  23,   24,  25,  26,  27,  28,
      29,  30,  31,  0,   32,  0,   0,   0,   0,   0,   33,  34,  35,  36,  0,   0,   37,   0,   0,   0,   0,   0,
      0,   0,   176, 38,  0,   13,  177, 14,  0,   72,  0,   39,  0,   0,   16,  17,  18,   19,  20,  21,  22,  23,
      24,  25,  26,  27,  28,  29,  30,  31,  0,   32,  0,   0,   0,   0,   0,   33,  34,   35,  36,  0,   0,   37,
      0,   0,   0,   0,   0,   0,   0,   73,  38,  13,  0,   14,  0,   72,  0,   0,   39,   0,   16,  17,  18,  19,
      20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  0,   32,  0,   0,   0,    0,   0,   33,  34,  35,
      36,  0,   0,   37,  0,   0,   0,   0,   0,   0,   0,   214, 38,  13,  0,   14,  0,    72,  0,   0,   39,  0,
      16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  0,    32,  0,   0,   0,   0,
      0,   33,  34,  35,  36,  0,   0,   37,  0,   0,   0,   0,   0,   0,   0,   243, 38,   13,  0,   14,  0,   72,
      0,   0,   39,  0,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,   29,  30,  31,  0,   32,
      0,   0,   0,   0,   0,   33,  34,  35,  36,  0,   0,   37,  0,   0,   0,   0,   0,    0,   0,   0,   38,  13,
      0,   14,  0,   131, 0,   0,   39,  0,   16,  17,  18,  19,  20,  21,  22,  23,  24,   25,  26,  27,  28,  29,
      30,  31,  0,   32,  0,   0,   0,   0,   0,   33,  34,  35,  36,  0,   0,   37,  0,    0,   0,   0,   0,   13,
      0,   14,  38,  231, 0,   0,   0,   0,   232, 0,   39,  19,  20,  21,  22,  23,  24,   25,  26,  27,  28,  29,
      30,  31,  0,   0,   0,   0,   0,   0,   0,   33,  34,  35,  36,  0,   13,  37,  14,   0,   363, 0,   0,   233,
      0,   232, 38,  0,   19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,   0,   0,   0,   0,   0,
      0,   0,   33,  34,  35,  36,  0,   13,  37,  14,  0,   308, 0,   0,   364, 0,   232,  38,  0,   19,  20,  21,
      22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  0,   0,   0,   0,   0,   0,   0,    33,  34,  35,  36,  0,
      13,  37,  14,  0,   317, 0,   0,   0,   0,   232, 38,  0,   19,  20,  21,  22,  23,   24,  25,  26,  27,  28,
      29,  30,  31,  0,   0,   0,   0,   0,   0,   0,   33,  34,  35,  36,  0,   13,  37,   14,  0,   384, 0,   0,
      0,   0,   232, 38,  0,   19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,   31,  0,   0,   0,   0,
      0,   0,   0,   33,  34,  35,  36,  0,   13,  37,  14,  0,   391, 0,   0,   0,   0,    232, 38,  0,   19,  20,
      21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  0,   0,   0,   0,   0,   0,    0,   33,  34,  35,  36,
      0,   0,   37,  0,   0,   0,   0,   0,   0,   0,   0,   38,  19,  20,  21,  22,  23,   24,  25,  26,  27,  163,
      0,   81,  118, 119, 84,  85,  86,  87,  120, 89,  121, 0,   0,   108, 81,  118, 119,  84,  85,  86,  87,  120,
      89,  121, 0,   109
  };

  const short parser::yycheck_[] = {
      2,   75,  32,  158, 15,  159, 105, 218, 95,  38,  15,  189, 0,   15,  14,  38,  90,  4,   117, 49,  38,  8,   12,
      15,  13,  124, 15,  98,  4,   116, 130, 102, 8,   12,  15,  30,  4,   39,  6,   99,  8,   12,  30,  6,   104, 0,
      4,   8,   40,  8,   40,  14,  15,  6,   83,  42,  15,  16,  17,  18,  83,  72,  64,  65,  56,  83,  42,  55,  8,
      15,  72,  226, 48,  73,  42,  56,  78,  56,  233, 290, 109, 152, 82,  261, 42,  56,  157, 48,  187, 267, 48,  165,
      51,  95,  8,   273, 47,  197, 169, 99,  278, 201, 12,  30,  104, 8,   6,   8,   48,  180, 184, 211, 4,   8,   116,
      27,  33,  44,  205, 10,  131, 123, 7,   125, 15,  7,   131, 12,  55,  131, 40,  10,  162, 12,  48,  10,  210, 12,
      140, 56,  239, 39,  142, 143, 15,  48,  217, 48,  4,   40,  42,  39,  32,  48,  16,  17,  18,  159, 160, 314, 10,
      11,  191, 163, 194, 167, 10,  14,  15,  49,  0,   173, 16,  17,  52,  53,  176, 177, 49,  57,  51,  52,  53,  54,
      55,  56,  57,  14,  15,  51,  344, 369, 0,   1,   30,  3,   6,   4,   8,   10,  355, 47,  10,  205, 32,  15,  34,
      35,  12,  364, 9,   10,  11,  9,   214, 12,  207, 208, 220, 219, 12,  58,  48,  7,   12,  225, 15,  107, 9,   231,
      9,   231, 12,  235, 9,   79,  80,  7,   58,  48,  9,   4,   10,  243, 88,  5,   126, 47,  10,  251, 250, 6,   9,
      7,   254, 342, 100, 5,   48,  9,   49,  263, 51,  52,  53,  54,  55,  56,  57,  271, 114, 48,  7,   12,  274, 5,
      9,   121, 9,   65,  9,   9,   162, 9,   48,  9,   130, 10,  9,   56,  320, 9,   6,   10,  9,   175, 9,   297, 10,
      9,   56,  5,   146, 377, 378, 5,   308, 151, 308, 189, 397, 155, 9,   9,   194, 317, 293, 317, 345, 167, 197, 211,
      259, 280, 190, 201, -1,  171, 146, 331, 332, -1,  -1,  407, -1,  -1,  -1,  -1,  340, -1,  342, -1,  344, -1,  -1,
      -1,  -1,  191, -1,  351, 350, -1,  -1,  197, 354, -1,  -1,  201, -1,  -1,  240, 363, -1,  363, -1,  -1,  -1,  211,
      -1,  -1,  -1,  373, -1,  -1,  218, -1,  256, -1,  222, 223, -1,  261, 384, -1,  384, 229, 230, 267, -1,  391, -1,
      391, -1,  273, -1,  397, -1,  277, 278, -1,  1,   -1,  -1,  4,   248, 6,   -1,  8,   -1,  -1,  -1,  -1,  13,  14,
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,
      38,  39,  -1,  320, 42,  -1,  -1,  -1,  -1,  290, -1,  -1,  -1,  51,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  59,  303,
      4,   -1,  6,   -1,  8,   9,   310, -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,
      27,  28,  -1,  30,  -1,  368, 369, -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      50,  51,  -1,  -1,  54,  -1,  4,   357, 6,   59,  8,   361, -1,  11,  -1,  13,  14,  15,  16,  17,  18,  19,  20,
      21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  32,  33,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,
      -1,  -1,  -1,  -1,  -1,  49,  50,  51,  52,  53,  54,  55,  56,  57,  4,   59,  6,   -1,  8,   -1,  -1,  11,  -1,
      13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  32,  -1,  -1,  -1,
      36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  49,  50,  51,  52,  53,  54,  55,  56,  57,  -1,
      59,  4,   5,   6,   -1,  8,   -1,  -1,  11,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,
      26,  27,  28,  29,  30,  -1,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  51,  -1,  4,   -1,  6,   -1,  8,   9,   59,  11,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,
      23,  24,  25,  26,  27,  28,  -1,  30,  -1,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  51,  -1,  4,   5,   6,   -1,  8,   -1,  59,  11,  -1,  13,  14,  15,  16,  17,  18,  19,
      20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  51,  4,   -1,  6,   7,   8,   -1,  -1,  59,  -1,  13,  14,  15,  16,  17,
      18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,
      -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  51,  -1,  4,   54,  6,   -1,  8,   9,   59,  -1,  -1,  13,  14,
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,
      38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  51,  -1,  4,   54,  6,   7,   8,   -1,  59,  -1,
      -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,
      -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  51,  -1,  4,   54,  6,   -1,  8,
      9,   59,  -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,
      -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  51,  -1,  4,   54,
      6,   -1,  8,   9,   59,  11,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,
      -1,  30,  -1,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  51,
      4,   -1,  6,   -1,  8,   9,   -1,  59,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,
      27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      50,  51,  -1,  4,   54,  6,   -1,  8,   9,   59,  -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,
      24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  50,  51,  -1,  -1,  54,  -1,  4,   5,   6,   59,  8,   -1,  -1,  11,  -1,  13,  14,  15,  16,  17,
      18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,
      -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  51,  -1,  4,   -1,  6,   -1,  8,   -1,  59,  11,  -1,  13,  14,
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  32,  -1,  -1,  -1,  36,  37,
      38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  51,  4,   -1,  6,   -1,  8,   -1,  -1,  59,  -1,
      13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,
      36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  51,  -1,  4,   54,  6,   -1,  8,   -1,
      59,  -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  -1,  -1,
      -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  51,  4,   -1,  6,   -1,
      8,   -1,  -1,  59,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,
      -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  51,  4,   -1,
      6,   -1,  8,   -1,  -1,  59,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,
      -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  51,
      4,   -1,  6,   -1,  8,   -1,  -1,  59,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,
      27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  51,  4,   -1,  6,   -1,  8,   -1,  -1,  59,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  -1,  30,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,
      4,   -1,  6,   51,  8,   -1,  -1,  -1,  -1,  13,  -1,  59,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,
      27,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  4,   42,  6,   -1,  8,   -1,  -1,  48,  -1,
      13,  51,  -1,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      36,  37,  38,  39,  -1,  4,   42,  6,   -1,  8,   -1,  -1,  48,  -1,  13,  51,  -1,  16,  17,  18,  19,  20,  21,
      22,  23,  24,  25,  26,  27,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  4,   42,  6,   -1,
      8,   -1,  -1,  -1,  -1,  13,  51,  -1,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  4,   42,  6,   -1,  8,   -1,  -1,  -1,  -1,  13,  51,  -1,  16,
      17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,
      -1,  4,   42,  6,   -1,  8,   -1,  -1,  -1,  -1,  13,  51,  -1,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,
      26,  27,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  51,  16,  17,  18,  19,  20,  21,  22,  23,  24,  47,  -1,  49,  50,  51,  52,  53,  54,  55,  56,  57,
      58,  -1,  -1,  39,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  -1,  51
  };

  const unsigned char parser::yystos_[] = {
      0,   30,  44,  55,  62,  64,  65,  66,  67,  70,  6,   69,  1,   4,   6,   8,   13, 14,  15,  16,  17,  18,  19,
      20,  21,  22,  23,  24,  25,  26,  27,  28,  30,  36,  37,  38,  39,  42,  51,  59, 81,  82,  91,  92,  93,  108,
      112, 113, 114, 115, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 30,  0,  1,   3,   10,  129, 0,   66,
      115, 47,  68,  8,   50,  93,  94,  95,  109, 11,  32,  33,  49,  50,  51,  52,  53, 54,  55,  56,  57,  85,  86,
      88,  89,  93,  98,  107, 110, 111, 6,   14,  115, 116, 115, 6,   8,   82,  84,  39, 51,  122, 123, 125, 93,  56,
      98,  99,  6,   50,  51,  56,  58,  89,  90,  8,   48,  84,  27,  39,  39,  4,   8,  63,  93,  63,  0,   7,   30,
      4,   95,  10,  129, 47,  12,  130, 93,  13,  105, 106, 115, 10,  40,  96,  115, 95, 40,  56,  97,  115, 10,  11,
      129, 9,   47,  9,   85,  93,  12,  130, 109, 115, 12,  130, 48,  109, 9,   50,  54, 95,  100, 101, 102, 82,  5,
      85,  7,   102, 6,   8,   103, 115, 58,  87,  93,  9,   102, 93,  54,  76,  77,  78, 79,  80,  88,  115, 98,  32,
      34,  35,  73,  74,  75,  93,  95,  50,  95,  7,   116, 12,  8,   48,  115, 12,  40, 130, 8,   48,  115, 12,  40,
      130, 8,   13,  48,  114, 11,  86,  93,  9,   8,   84,  95,  129, 50,  93,  110, 9,  130, 14,  115, 8,   48,  93,
      7,   12,  83,  84,  95,  95,  12,  130, 9,   129, 48,  7,   7,   102, 9,   102, 83, 8,   48,  103, 104, 8,   115,
      122, 84,  9,   77,  4,   78,  5,   9,   85,  52,  53,  57,  71,  72,  128, 72,  72, 10,  129, 80,  95,  47,  130,
      105, 95,  93,  10,  12,  115, 115, 10,  95,  8,   114, 12,  56,  115, 56,  115, 93, 95,  8,   114, 93,  9,   102,
      9,   7,   115, 95,  93,  100, 83,  5,   93,  48,  48,  7,   83,  9,   9,   93,  83, 95,  48,  83,  99,  9,   10,
      12,  105, 73,  5,   95,  8,   48,  9,   115, 8,   48,  10,  12,  9,   95,  115, 12, 56,  8,   48,  114, 9,   95,
      84,  9,   9,   93,  93,  48,  9,   93,  5,   29,  85,  71,  95,  93,  10,  95,  8,  114, 115, 9,   56,  115, 95,
      8,   114, 9,   83,  93,  10,  129, 129, 9,   9,   95,  10,  56,  9,   95,  5,   85, 5,   9,   9,   129, 5
  };

  const unsigned char parser::yyr1_[] = {
      0,   61,  62,  62,  62,  63,  63,  63,  64,  64,  64,  64,  64,  64,  64,  65,  65,  66,  66,  67,  68,  68,
      69,  69,  70,  71,  72,  72,  73,  73,  73,  74,  74,  75,  75,  76,  76,  77,  77,  77,  77,  78,  78,  79,
      79,  80,  80,  81,  81,  82,  82,  83,  83,  84,  84,  85,  85,  85,  85,  85,  86,  86,  87,  87,  87,  88,
      88,  88,  88,  88,  88,  88,  88,  89,  89,  90,  90,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,  91,
      91,  91,  92,  92,  92,  92,  92,  92,  92,  93,  93,  93,  93,  93,  93,  93,  93,  93,  93,  93,  93,  93,
      93,  93,  93,  94,  94,  95,  96,  96,  96,  96,  96,  96,  96,  96,  97,  97,  97,  97,  97,  97,  97,  97,
      97,  97,  98,  98,  98,  98,  98,  98,  98,  98,  98,  98,  98,  98,  99,  99,  100, 100, 100, 101, 101, 102,
      103, 104, 104, 104, 105, 105, 105, 105, 105, 106, 106, 107, 108, 108, 108, 108, 108, 108, 108, 108, 109, 109,
      109, 109, 109, 109, 109, 110, 110, 111, 111, 111, 112, 113, 113, 114, 114, 114, 115, 116, 116, 116, 116, 117,
      118, 119, 119, 120, 120, 121, 121, 122, 122, 122, 122, 123, 123, 123, 123, 124, 124, 125, 125, 126, 126, 127,
      127, 127, 127, 127, 127, 128, 128, 128, 128, 129, 129, 130, 130
  };

  const signed char parser::yyr2_[] = { 0, 2, 3, 2, 0, 1, 4, 3, 2, 2, 6, 4, 3, 3, 2, 1, 2, 1, 1, 7, 0, 2, 0, 3, 5, 2,
                                        1, 3, 2, 2, 2, 1, 3, 0, 2, 1, 1, 6, 4, 8, 6, 1, 2, 1, 2, 0, 1, 1, 1, 5, 3, 0,
                                        1, 1, 2, 1, 3, 3, 4, 2, 1, 1, 2, 2, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 7,
                                        6, 4, 5, 4, 2, 5, 4, 5, 4, 2, 5, 4, 1, 1, 1, 4, 2, 4, 3, 1, 1, 5, 4, 2, 3, 3,
                                        4, 5, 6, 5, 6, 6, 5, 7, 6, 1, 3, 2, 1, 3, 4, 5, 3, 5, 6, 7, 2, 1, 3, 4, 5, 4,
                                        3, 5, 6, 7, 2, 4, 5, 7, 4, 6, 2, 4, 5, 7, 4, 6, 0, 1, 2, 2, 1, 1, 3, 2, 3, 0,
                                        2, 1, 1, 3, 4, 5, 6, 1, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 3, 2, 3, 5, 4, 3,
                                        2, 0, 1, 3, 4, 4, 5, 1, 1, 1, 1, 1, 2, 3, 4, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1,
                                        2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 2, 1, 2, 0, 1, 1, 1, 0, 1, 0, 1 };


#if YYDEBUG
  const short parser::yyrline_[] = {
      0,   174, 174, 175, 176, 184, 186, 197, 212, 214,  220,  232,  246, 249, 252, 284, 286, 291, 292, 296, 301, 302,
      306, 307, 311, 320, 325, 327, 332, 334, 336, 341,  343,  348,  349, 357, 358, 362, 364, 366, 368, 373, 374, 379,
      380, 384, 385, 393, 393, 396, 398, 403, 404, 408,  409,  413,  414, 415, 420, 425, 433, 434, 443, 444, 445, 449,
      450, 451, 452, 453, 454, 455, 456, 460, 461, 465,  467,  472,  481, 484, 488, 497, 500, 503, 514, 524, 539, 542,
      548, 559, 572, 574, 576, 577, 582, 587, 594, 604,  608,  610,  615, 621, 623, 629, 632, 635, 638, 641, 643, 649,
      651, 654, 657, 662, 663, 675, 687, 689, 691, 693,  695,  697,  699, 701, 706, 708, 710, 712, 714, 716, 718, 720,
      722, 724, 729, 730, 731, 733, 735, 740, 749, 751,  753,  755,  757, 762, 774, 775, 779, 780, 781, 785, 786, 789,
      791, 794, 795, 797, 805, 807, 809, 813, 815, 820,  822,  826,  834, 835, 836, 837, 838, 839, 840, 841, 845, 847,
      849, 851, 853, 855, 857, 865, 867, 872, 874, 876,  881,  886,  888, 897, 898, 899, 906, 910, 912, 914, 916, 927,
      929, 932, 933, 937, 938, 942, 943, 947, 948, 949,  950,  959,  960, 961, 962, 966, 968, 973, 975, 980, 982, 987,
      988, 989, 990, 991, 992, 996, 997, 998, 999, 1011, 1011, 1012, 1012
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
      55,    56,    57,    58,    59,    60
    };
    // Last valid token kind.
    const int code_max = 315;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 7 "langutils/sc_parser/src/sc_grammar.y"
} } } // sc::ast::parser
#line 4982 "langutils/sc_parser/src/sc_grammar_parser.cpp"

#line 1014 "langutils/sc_parser/src/sc_grammar.y"
