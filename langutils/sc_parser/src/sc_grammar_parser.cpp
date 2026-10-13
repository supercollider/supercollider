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
          value.copy<AnyExprIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_method: // method
        value.copy< AnyMethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_119_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.copy<AnyPossiblyLiteralIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_symbol: // symbol
          value.copy<AnySymbolIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_111_arguments_entries: // arguments.entries
          value.copy<ArgumentEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_112_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_114_arguments_paren: // arguments.paren
      case symbol_kind::S_115_arguments_maybe_paren: // arguments.maybe_paren
          value.copy<ArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_120_literal_array_contents: // literal.array.contents
      case symbol_kind::S_124_literal_array: // literal.array
          value.copy<ArrayIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_94_expr_dotrange: // expr.dotrange
      case symbol_kind::S_96_expr_base_range: // expr.base.range
          value.copy<AtMessageIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_77_members_opt: // members.opt
          value.copy<BigMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_87_block_contents: // block.contents
          value.copy<BlockContentsListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_block: // block
        value.copy< BlockIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_88_block_contents_item: // block.contents.item
          value.copy<BlockItemIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_85_block_opt_list: // block.opt_list
      case symbol_kind::S_86_block_list: // block.list
          value.copy<BlockListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_boolean: // boolean
        value.copy< BooleanLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_72_class_extension: // class.extension
          value.copy<ClassExtensionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_class: // class
        value.copy< ClassIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_68_classOrExtList_item: // classOrExtList.item
          value.copy<ClassOrExtensionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_67_classOrExtList_list: // classOrExtList.list
          value.copy<ClassOrExtensionListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_75_members_entry: // members.entry
          value.copy<DeclareAnyMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_116_variable_declarations_list_item: // variable_declarations.list.item
          value.copy<DeclareAnyVariableIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_107_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_108_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_110_argument_declarations_opt: // argument_declarations.opt
          value.copy<DeclareArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_73_members_entry_item: // members.entry.item
          value.copy<DeclareMemberIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_74_members_entry_list: // members.entry.list
          value.copy<DeclareMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_117_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.copy< DeclareVariableListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_121_literal_dictionary_entry: // literal.dictionary.entry
          value.copy<DictionaryEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_122_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_123_literal_dictionary: // literal.dictionary
          value.copy<DictionaryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_file_as_func: // file_as_func
          value.copy<FileAsFunctionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_135_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_136_float_raw: // float.raw
          value.copy<FloatLitIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_float: // float
        value.copy< FloatProducingIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_generator: // generator
          value.copy<GeneratorAnyIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_106_generator_qual: // generator.qual
          value.copy<GeneratorQualAnyIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_105_generator_qual_list: // generator.qual.list
          value.copy<GeneratorQualListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_go: // go
          value.copy<GoIndex>(YY_MOVE(that.value));
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

      case symbol_kind::S_79_method_base: // method.base
          value.copy<MethodIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_81_method_list: // method.list
      case symbol_kind::S_82_method_list_opt: // method.list.opt
          value.copy<MethodListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_127_name_list: // name.list
          value.copy<NameListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_128_name_pack: // name.pack
          value.copy<NamePackIndex>(YY_MOVE(that.value));
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

      case symbol_kind::S_95_expr_dotrange_assign: // expr.dotrange.assign
      case symbol_kind::S_97_expr_base_range_assign: // expr.base.range.assign
          value.copy<RangeAssignIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_99_range_generator: // range.generator
          value.copy<RangeGeneratorIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_range: // range
          value.copy<RangeIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_region: // region
        value.copy< RegionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_78_method_name: // method.name
      case symbol_kind::S_90_binary_op_raw: // binary_op.raw
      case symbol_kind::S_91_binary_op_no_adverb: // binary_op.no_adverb
          value.copy<SelectorIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_binary_op: // binary_op
        value.copy< SelectorMaybeAdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_string: // string
        value.copy< StringLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_65_region_item: // region.item
          value.copy<error_index<AnyExprIndex>>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_70_class_super_opt: // class.super.opt
          value.copy<maybe<ClassNameIdentifierIndex>>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_71_class_slot_opt: // class.slot.opt
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
          value.move<AnyExprIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_method: // method
        value.move< AnyMethodIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_119_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.move<AnyPossiblyLiteralIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_symbol: // symbol
          value.move<AnySymbolIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_111_arguments_entries: // arguments.entries
          value.move<ArgumentEntryIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_112_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_114_arguments_paren: // arguments.paren
      case symbol_kind::S_115_arguments_maybe_paren: // arguments.maybe_paren
          value.move<ArgumentListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_120_literal_array_contents: // literal.array.contents
      case symbol_kind::S_124_literal_array: // literal.array
          value.move<ArrayIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_94_expr_dotrange: // expr.dotrange
      case symbol_kind::S_96_expr_base_range: // expr.base.range
          value.move<AtMessageIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_77_members_opt: // members.opt
          value.move<BigMemberListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_87_block_contents: // block.contents
          value.move<BlockContentsListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_block: // block
        value.move< BlockIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_88_block_contents_item: // block.contents.item
          value.move<BlockItemIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_85_block_opt_list: // block.opt_list
      case symbol_kind::S_86_block_list: // block.list
          value.move<BlockListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_boolean: // boolean
        value.move< BooleanLitIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_72_class_extension: // class.extension
          value.move<ClassExtensionIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_class: // class
        value.move< ClassIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_68_classOrExtList_item: // classOrExtList.item
          value.move<ClassOrExtensionIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_67_classOrExtList_list: // classOrExtList.list
          value.move<ClassOrExtensionListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_75_members_entry: // members.entry
          value.move<DeclareAnyMemberListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_116_variable_declarations_list_item: // variable_declarations.list.item
          value.move<DeclareAnyVariableIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_107_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_108_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_110_argument_declarations_opt: // argument_declarations.opt
          value.move<DeclareArgumentListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_73_members_entry_item: // members.entry.item
          value.move<DeclareMemberIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_74_members_entry_list: // members.entry.list
          value.move<DeclareMemberListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_117_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.move< DeclareVariableListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_121_literal_dictionary_entry: // literal.dictionary.entry
          value.move<DictionaryEntryIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_122_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_123_literal_dictionary: // literal.dictionary
          value.move<DictionaryIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_file_as_func: // file_as_func
          value.move<FileAsFunctionIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_135_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_136_float_raw: // float.raw
          value.move<FloatLitIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_float: // float
        value.move< FloatProducingIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_generator: // generator
          value.move<GeneratorAnyIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_106_generator_qual: // generator.qual
          value.move<GeneratorQualAnyIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_105_generator_qual_list: // generator.qual.list
          value.move<GeneratorQualListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_go: // go
          value.move<GoIndex>(YY_MOVE(s.value));
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

      case symbol_kind::S_79_method_base: // method.base
          value.move<MethodIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_81_method_list: // method.list
      case symbol_kind::S_82_method_list_opt: // method.list.opt
          value.move<MethodListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_127_name_list: // name.list
          value.move<NameListIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_128_name_pack: // name.pack
          value.move<NamePackIndex>(YY_MOVE(s.value));
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

      case symbol_kind::S_95_expr_dotrange_assign: // expr.dotrange.assign
      case symbol_kind::S_97_expr_base_range_assign: // expr.base.range.assign
          value.move<RangeAssignIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_99_range_generator: // range.generator
          value.move<RangeGeneratorIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_range: // range
          value.move<RangeIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_region: // region
        value.move< RegionListIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_78_method_name: // method.name
      case symbol_kind::S_90_binary_op_raw: // binary_op.raw
      case symbol_kind::S_91_binary_op_no_adverb: // binary_op.no_adverb
          value.move<SelectorIndex>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_binary_op: // binary_op
        value.move< SelectorMaybeAdverbIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_string: // string
        value.move< StringLitIndex > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_65_region_item: // region.item
          value.move<error_index<AnyExprIndex>>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_70_class_super_opt: // class.super.opt
          value.move<maybe<ClassNameIdentifierIndex>>(YY_MOVE(s.value));
          break;

      case symbol_kind::S_71_class_slot_opt: // class.slot.opt
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
          value.YY_MOVE_OR_COPY<AnyExprIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_method: // method
        value.YY_MOVE_OR_COPY< AnyMethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_119_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.YY_MOVE_OR_COPY<AnyPossiblyLiteralIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_symbol: // symbol
          value.YY_MOVE_OR_COPY<AnySymbolIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_111_arguments_entries: // arguments.entries
          value.YY_MOVE_OR_COPY<ArgumentEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_112_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_114_arguments_paren: // arguments.paren
      case symbol_kind::S_115_arguments_maybe_paren: // arguments.maybe_paren
          value.YY_MOVE_OR_COPY<ArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_120_literal_array_contents: // literal.array.contents
      case symbol_kind::S_124_literal_array: // literal.array
          value.YY_MOVE_OR_COPY<ArrayIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_94_expr_dotrange: // expr.dotrange
      case symbol_kind::S_96_expr_base_range: // expr.base.range
          value.YY_MOVE_OR_COPY<AtMessageIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_77_members_opt: // members.opt
          value.YY_MOVE_OR_COPY<BigMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_87_block_contents: // block.contents
          value.YY_MOVE_OR_COPY<BlockContentsListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_block: // block
        value.YY_MOVE_OR_COPY< BlockIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_88_block_contents_item: // block.contents.item
          value.YY_MOVE_OR_COPY<BlockItemIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_85_block_opt_list: // block.opt_list
      case symbol_kind::S_86_block_list: // block.list
          value.YY_MOVE_OR_COPY<BlockListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_boolean: // boolean
        value.YY_MOVE_OR_COPY< BooleanLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_72_class_extension: // class.extension
          value.YY_MOVE_OR_COPY<ClassExtensionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_class: // class
        value.YY_MOVE_OR_COPY< ClassIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_68_classOrExtList_item: // classOrExtList.item
          value.YY_MOVE_OR_COPY<ClassOrExtensionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_67_classOrExtList_list: // classOrExtList.list
          value.YY_MOVE_OR_COPY<ClassOrExtensionListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_75_members_entry: // members.entry
          value.YY_MOVE_OR_COPY<DeclareAnyMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_116_variable_declarations_list_item: // variable_declarations.list.item
          value.YY_MOVE_OR_COPY<DeclareAnyVariableIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_107_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_108_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_110_argument_declarations_opt: // argument_declarations.opt
          value.YY_MOVE_OR_COPY<DeclareArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_73_members_entry_item: // members.entry.item
          value.YY_MOVE_OR_COPY<DeclareMemberIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_74_members_entry_list: // members.entry.list
          value.YY_MOVE_OR_COPY<DeclareMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_117_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.YY_MOVE_OR_COPY< DeclareVariableListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_121_literal_dictionary_entry: // literal.dictionary.entry
          value.YY_MOVE_OR_COPY<DictionaryEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_122_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_123_literal_dictionary: // literal.dictionary
          value.YY_MOVE_OR_COPY<DictionaryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_file_as_func: // file_as_func
          value.YY_MOVE_OR_COPY<FileAsFunctionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_135_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_136_float_raw: // float.raw
          value.YY_MOVE_OR_COPY<FloatLitIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_float: // float
        value.YY_MOVE_OR_COPY< FloatProducingIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_generator: // generator
          value.YY_MOVE_OR_COPY<GeneratorAnyIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_106_generator_qual: // generator.qual
          value.YY_MOVE_OR_COPY<GeneratorQualAnyIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_105_generator_qual_list: // generator.qual.list
          value.YY_MOVE_OR_COPY<GeneratorQualListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_go: // go
          value.YY_MOVE_OR_COPY<GoIndex>(YY_MOVE(that.value));
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

      case symbol_kind::S_79_method_base: // method.base
          value.YY_MOVE_OR_COPY<MethodIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_81_method_list: // method.list
      case symbol_kind::S_82_method_list_opt: // method.list.opt
          value.YY_MOVE_OR_COPY<MethodListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_127_name_list: // name.list
          value.YY_MOVE_OR_COPY<NameListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_128_name_pack: // name.pack
          value.YY_MOVE_OR_COPY<NamePackIndex>(YY_MOVE(that.value));
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

      case symbol_kind::S_95_expr_dotrange_assign: // expr.dotrange.assign
      case symbol_kind::S_97_expr_base_range_assign: // expr.base.range.assign
          value.YY_MOVE_OR_COPY<RangeAssignIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_99_range_generator: // range.generator
          value.YY_MOVE_OR_COPY<RangeGeneratorIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_range: // range
          value.YY_MOVE_OR_COPY<RangeIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_region: // region
        value.YY_MOVE_OR_COPY< RegionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_78_method_name: // method.name
      case symbol_kind::S_90_binary_op_raw: // binary_op.raw
      case symbol_kind::S_91_binary_op_no_adverb: // binary_op.no_adverb
          value.YY_MOVE_OR_COPY<SelectorIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_binary_op: // binary_op
        value.YY_MOVE_OR_COPY< SelectorMaybeAdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_string: // string
        value.YY_MOVE_OR_COPY< StringLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_65_region_item: // region.item
          value.YY_MOVE_OR_COPY<error_index<AnyExprIndex>>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_70_class_super_opt: // class.super.opt
          value.YY_MOVE_OR_COPY<maybe<ClassNameIdentifierIndex>>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_71_class_slot_opt: // class.slot.opt
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
          value.move<AnyExprIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_method: // method
        value.move< AnyMethodIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_119_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.move<AnyPossiblyLiteralIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_symbol: // symbol
          value.move<AnySymbolIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_111_arguments_entries: // arguments.entries
          value.move<ArgumentEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_112_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_114_arguments_paren: // arguments.paren
      case symbol_kind::S_115_arguments_maybe_paren: // arguments.maybe_paren
          value.move<ArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_120_literal_array_contents: // literal.array.contents
      case symbol_kind::S_124_literal_array: // literal.array
          value.move<ArrayIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_94_expr_dotrange: // expr.dotrange
      case symbol_kind::S_96_expr_base_range: // expr.base.range
          value.move<AtMessageIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_77_members_opt: // members.opt
          value.move<BigMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_87_block_contents: // block.contents
          value.move<BlockContentsListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_block: // block
        value.move< BlockIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_88_block_contents_item: // block.contents.item
          value.move<BlockItemIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_85_block_opt_list: // block.opt_list
      case symbol_kind::S_86_block_list: // block.list
          value.move<BlockListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_boolean: // boolean
        value.move< BooleanLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_72_class_extension: // class.extension
          value.move<ClassExtensionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_class: // class
        value.move< ClassIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_68_classOrExtList_item: // classOrExtList.item
          value.move<ClassOrExtensionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_67_classOrExtList_list: // classOrExtList.list
          value.move<ClassOrExtensionListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_75_members_entry: // members.entry
          value.move<DeclareAnyMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_116_variable_declarations_list_item: // variable_declarations.list.item
          value.move<DeclareAnyVariableIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_107_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_108_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_110_argument_declarations_opt: // argument_declarations.opt
          value.move<DeclareArgumentListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_73_members_entry_item: // members.entry.item
          value.move<DeclareMemberIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_74_members_entry_list: // members.entry.list
          value.move<DeclareMemberListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_117_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.move< DeclareVariableListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_121_literal_dictionary_entry: // literal.dictionary.entry
          value.move<DictionaryEntryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_122_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_123_literal_dictionary: // literal.dictionary
          value.move<DictionaryIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_file_as_func: // file_as_func
          value.move<FileAsFunctionIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_135_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_136_float_raw: // float.raw
          value.move<FloatLitIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_float: // float
        value.move< FloatProducingIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_generator: // generator
          value.move<GeneratorAnyIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_106_generator_qual: // generator.qual
          value.move<GeneratorQualAnyIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_105_generator_qual_list: // generator.qual.list
          value.move<GeneratorQualListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_go: // go
          value.move<GoIndex>(YY_MOVE(that.value));
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

      case symbol_kind::S_79_method_base: // method.base
          value.move<MethodIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_81_method_list: // method.list
      case symbol_kind::S_82_method_list_opt: // method.list.opt
          value.move<MethodListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_127_name_list: // name.list
          value.move<NameListIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_128_name_pack: // name.pack
          value.move<NamePackIndex>(YY_MOVE(that.value));
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

      case symbol_kind::S_95_expr_dotrange_assign: // expr.dotrange.assign
      case symbol_kind::S_97_expr_base_range_assign: // expr.base.range.assign
          value.move<RangeAssignIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_99_range_generator: // range.generator
          value.move<RangeGeneratorIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_range: // range
          value.move<RangeIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_region: // region
        value.move< RegionListIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_78_method_name: // method.name
      case symbol_kind::S_90_binary_op_raw: // binary_op.raw
      case symbol_kind::S_91_binary_op_no_adverb: // binary_op.no_adverb
          value.move<SelectorIndex>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_binary_op: // binary_op
        value.move< SelectorMaybeAdverbIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_string: // string
        value.move< StringLitIndex > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_65_region_item: // region.item
          value.move<error_index<AnyExprIndex>>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_70_class_super_opt: // class.super.opt
          value.move<maybe<ClassNameIdentifierIndex>>(YY_MOVE(that.value));
          break;

      case symbol_kind::S_71_class_slot_opt: // class.slot.opt
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
          value.copy<AnyExprIndex>(that.value);
          break;

      case symbol_kind::S_method: // method
        value.copy< AnyMethodIndex > (that.value);
        break;

      case symbol_kind::S_119_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.copy<AnyPossiblyLiteralIndex>(that.value);
          break;

      case symbol_kind::S_symbol: // symbol
          value.copy<AnySymbolIndex>(that.value);
          break;

      case symbol_kind::S_111_arguments_entries: // arguments.entries
          value.copy<ArgumentEntryIndex>(that.value);
          break;

      case symbol_kind::S_112_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_114_arguments_paren: // arguments.paren
      case symbol_kind::S_115_arguments_maybe_paren: // arguments.maybe_paren
          value.copy<ArgumentListIndex>(that.value);
          break;

      case symbol_kind::S_120_literal_array_contents: // literal.array.contents
      case symbol_kind::S_124_literal_array: // literal.array
          value.copy<ArrayIndex>(that.value);
          break;

      case symbol_kind::S_94_expr_dotrange: // expr.dotrange
      case symbol_kind::S_96_expr_base_range: // expr.base.range
          value.copy<AtMessageIndex>(that.value);
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_77_members_opt: // members.opt
          value.copy<BigMemberListIndex>(that.value);
          break;

      case symbol_kind::S_87_block_contents: // block.contents
          value.copy<BlockContentsListIndex>(that.value);
          break;

      case symbol_kind::S_block: // block
        value.copy< BlockIndex > (that.value);
        break;

      case symbol_kind::S_88_block_contents_item: // block.contents.item
          value.copy<BlockItemIndex>(that.value);
          break;

      case symbol_kind::S_85_block_opt_list: // block.opt_list
      case symbol_kind::S_86_block_list: // block.list
          value.copy<BlockListIndex>(that.value);
          break;

      case symbol_kind::S_boolean: // boolean
        value.copy< BooleanLitIndex > (that.value);
        break;

      case symbol_kind::S_72_class_extension: // class.extension
          value.copy<ClassExtensionIndex>(that.value);
          break;

      case symbol_kind::S_class: // class
        value.copy< ClassIndex > (that.value);
        break;

      case symbol_kind::S_68_classOrExtList_item: // classOrExtList.item
          value.copy<ClassOrExtensionIndex>(that.value);
          break;

      case symbol_kind::S_67_classOrExtList_list: // classOrExtList.list
          value.copy<ClassOrExtensionListIndex>(that.value);
          break;

      case symbol_kind::S_75_members_entry: // members.entry
          value.copy<DeclareAnyMemberListIndex>(that.value);
          break;

      case symbol_kind::S_116_variable_declarations_list_item: // variable_declarations.list.item
          value.copy<DeclareAnyVariableIndex>(that.value);
          break;

      case symbol_kind::S_107_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_108_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_110_argument_declarations_opt: // argument_declarations.opt
          value.copy<DeclareArgumentListIndex>(that.value);
          break;

      case symbol_kind::S_73_members_entry_item: // members.entry.item
          value.copy<DeclareMemberIndex>(that.value);
          break;

      case symbol_kind::S_74_members_entry_list: // members.entry.list
          value.copy<DeclareMemberListIndex>(that.value);
          break;

      case symbol_kind::S_117_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.copy< DeclareVariableListIndex > (that.value);
        break;

      case symbol_kind::S_121_literal_dictionary_entry: // literal.dictionary.entry
          value.copy<DictionaryEntryIndex>(that.value);
          break;

      case symbol_kind::S_122_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_123_literal_dictionary: // literal.dictionary
          value.copy<DictionaryIndex>(that.value);
          break;

      case symbol_kind::S_file_as_func: // file_as_func
          value.copy<FileAsFunctionIndex>(that.value);
          break;

      case symbol_kind::S_135_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_136_float_raw: // float.raw
          value.copy<FloatLitIndex>(that.value);
          break;

      case symbol_kind::S_float: // float
        value.copy< FloatProducingIndex > (that.value);
        break;

      case symbol_kind::S_generator: // generator
          value.copy<GeneratorAnyIndex>(that.value);
          break;

      case symbol_kind::S_106_generator_qual: // generator.qual
          value.copy<GeneratorQualAnyIndex>(that.value);
          break;

      case symbol_kind::S_105_generator_qual_list: // generator.qual.list
          value.copy<GeneratorQualListIndex>(that.value);
          break;

      case symbol_kind::S_go: // go
          value.copy<GoIndex>(that.value);
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

      case symbol_kind::S_79_method_base: // method.base
          value.copy<MethodIndex>(that.value);
          break;

      case symbol_kind::S_81_method_list: // method.list
      case symbol_kind::S_82_method_list_opt: // method.list.opt
          value.copy<MethodListIndex>(that.value);
          break;

      case symbol_kind::S_127_name_list: // name.list
          value.copy<NameListIndex>(that.value);
          break;

      case symbol_kind::S_128_name_pack: // name.pack
          value.copy<NamePackIndex>(that.value);
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

      case symbol_kind::S_95_expr_dotrange_assign: // expr.dotrange.assign
      case symbol_kind::S_97_expr_base_range_assign: // expr.base.range.assign
          value.copy<RangeAssignIndex>(that.value);
          break;

      case symbol_kind::S_99_range_generator: // range.generator
          value.copy<RangeGeneratorIndex>(that.value);
          break;

      case symbol_kind::S_range: // range
          value.copy<RangeIndex>(that.value);
          break;

      case symbol_kind::S_region: // region
        value.copy< RegionListIndex > (that.value);
        break;

      case symbol_kind::S_78_method_name: // method.name
      case symbol_kind::S_90_binary_op_raw: // binary_op.raw
      case symbol_kind::S_91_binary_op_no_adverb: // binary_op.no_adverb
          value.copy<SelectorIndex>(that.value);
          break;

      case symbol_kind::S_binary_op: // binary_op
        value.copy< SelectorMaybeAdverbIndex > (that.value);
        break;

      case symbol_kind::S_string: // string
        value.copy< StringLitIndex > (that.value);
        break;

      case symbol_kind::S_65_region_item: // region.item
          value.copy<error_index<AnyExprIndex>>(that.value);
          break;

      case symbol_kind::S_70_class_super_opt: // class.super.opt
          value.copy<maybe<ClassNameIdentifierIndex>>(that.value);
          break;

      case symbol_kind::S_71_class_slot_opt: // class.slot.opt
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
          value.move<AnyExprIndex>(that.value);
          break;

      case symbol_kind::S_method: // method
        value.move< AnyMethodIndex > (that.value);
        break;

      case symbol_kind::S_119_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          value.move<AnyPossiblyLiteralIndex>(that.value);
          break;

      case symbol_kind::S_symbol: // symbol
          value.move<AnySymbolIndex>(that.value);
          break;

      case symbol_kind::S_111_arguments_entries: // arguments.entries
          value.move<ArgumentEntryIndex>(that.value);
          break;

      case symbol_kind::S_112_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_114_arguments_paren: // arguments.paren
      case symbol_kind::S_115_arguments_maybe_paren: // arguments.maybe_paren
          value.move<ArgumentListIndex>(that.value);
          break;

      case symbol_kind::S_120_literal_array_contents: // literal.array.contents
      case symbol_kind::S_124_literal_array: // literal.array
          value.move<ArrayIndex>(that.value);
          break;

      case symbol_kind::S_94_expr_dotrange: // expr.dotrange
      case symbol_kind::S_96_expr_base_range: // expr.base.range
          value.move<AtMessageIndex>(that.value);
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_77_members_opt: // members.opt
          value.move<BigMemberListIndex>(that.value);
          break;

      case symbol_kind::S_87_block_contents: // block.contents
          value.move<BlockContentsListIndex>(that.value);
          break;

      case symbol_kind::S_block: // block
        value.move< BlockIndex > (that.value);
        break;

      case symbol_kind::S_88_block_contents_item: // block.contents.item
          value.move<BlockItemIndex>(that.value);
          break;

      case symbol_kind::S_85_block_opt_list: // block.opt_list
      case symbol_kind::S_86_block_list: // block.list
          value.move<BlockListIndex>(that.value);
          break;

      case symbol_kind::S_boolean: // boolean
        value.move< BooleanLitIndex > (that.value);
        break;

      case symbol_kind::S_72_class_extension: // class.extension
          value.move<ClassExtensionIndex>(that.value);
          break;

      case symbol_kind::S_class: // class
        value.move< ClassIndex > (that.value);
        break;

      case symbol_kind::S_68_classOrExtList_item: // classOrExtList.item
          value.move<ClassOrExtensionIndex>(that.value);
          break;

      case symbol_kind::S_67_classOrExtList_list: // classOrExtList.list
          value.move<ClassOrExtensionListIndex>(that.value);
          break;

      case symbol_kind::S_75_members_entry: // members.entry
          value.move<DeclareAnyMemberListIndex>(that.value);
          break;

      case symbol_kind::S_116_variable_declarations_list_item: // variable_declarations.list.item
          value.move<DeclareAnyVariableIndex>(that.value);
          break;

      case symbol_kind::S_107_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_108_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_110_argument_declarations_opt: // argument_declarations.opt
          value.move<DeclareArgumentListIndex>(that.value);
          break;

      case symbol_kind::S_73_members_entry_item: // members.entry.item
          value.move<DeclareMemberIndex>(that.value);
          break;

      case symbol_kind::S_74_members_entry_list: // members.entry.list
          value.move<DeclareMemberListIndex>(that.value);
          break;

      case symbol_kind::S_117_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        value.move< DeclareVariableListIndex > (that.value);
        break;

      case symbol_kind::S_121_literal_dictionary_entry: // literal.dictionary.entry
          value.move<DictionaryEntryIndex>(that.value);
          break;

      case symbol_kind::S_122_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_123_literal_dictionary: // literal.dictionary
          value.move<DictionaryIndex>(that.value);
          break;

      case symbol_kind::S_file_as_func: // file_as_func
          value.move<FileAsFunctionIndex>(that.value);
          break;

      case symbol_kind::S_135_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_136_float_raw: // float.raw
          value.move<FloatLitIndex>(that.value);
          break;

      case symbol_kind::S_float: // float
        value.move< FloatProducingIndex > (that.value);
        break;

      case symbol_kind::S_generator: // generator
          value.move<GeneratorAnyIndex>(that.value);
          break;

      case symbol_kind::S_106_generator_qual: // generator.qual
          value.move<GeneratorQualAnyIndex>(that.value);
          break;

      case symbol_kind::S_105_generator_qual_list: // generator.qual.list
          value.move<GeneratorQualListIndex>(that.value);
          break;

      case symbol_kind::S_go: // go
          value.move<GoIndex>(that.value);
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

      case symbol_kind::S_79_method_base: // method.base
          value.move<MethodIndex>(that.value);
          break;

      case symbol_kind::S_81_method_list: // method.list
      case symbol_kind::S_82_method_list_opt: // method.list.opt
          value.move<MethodListIndex>(that.value);
          break;

      case symbol_kind::S_127_name_list: // name.list
          value.move<NameListIndex>(that.value);
          break;

      case symbol_kind::S_128_name_pack: // name.pack
          value.move<NamePackIndex>(that.value);
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

      case symbol_kind::S_95_expr_dotrange_assign: // expr.dotrange.assign
      case symbol_kind::S_97_expr_base_range_assign: // expr.base.range.assign
          value.move<RangeAssignIndex>(that.value);
          break;

      case symbol_kind::S_99_range_generator: // range.generator
          value.move<RangeGeneratorIndex>(that.value);
          break;

      case symbol_kind::S_range: // range
          value.move<RangeIndex>(that.value);
          break;

      case symbol_kind::S_region: // region
        value.move< RegionListIndex > (that.value);
        break;

      case symbol_kind::S_78_method_name: // method.name
      case symbol_kind::S_90_binary_op_raw: // binary_op.raw
      case symbol_kind::S_91_binary_op_no_adverb: // binary_op.no_adverb
          value.move<SelectorIndex>(that.value);
          break;

      case symbol_kind::S_binary_op: // binary_op
        value.move< SelectorMaybeAdverbIndex > (that.value);
        break;

      case symbol_kind::S_string: // string
        value.move< StringLitIndex > (that.value);
        break;

      case symbol_kind::S_65_region_item: // region.item
          value.move<error_index<AnyExprIndex>>(that.value);
          break;

      case symbol_kind::S_70_class_super_opt: // class.super.opt
          value.move<maybe<ClassNameIdentifierIndex>>(that.value);
          break;

      case symbol_kind::S_71_class_slot_opt: // class.slot.opt
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
          yylhs.value.emplace<AnyExprIndex>();
          break;

      case symbol_kind::S_method: // method
        yylhs.value.emplace< AnyMethodIndex > ();
        break;

      case symbol_kind::S_119_literal_terminal: // literal.terminal
      case symbol_kind::S_literal: // literal
          yylhs.value.emplace<AnyPossiblyLiteralIndex>();
          break;

      case symbol_kind::S_symbol: // symbol
          yylhs.value.emplace<AnySymbolIndex>();
          break;

      case symbol_kind::S_111_arguments_entries: // arguments.entries
          yylhs.value.emplace<ArgumentEntryIndex>();
          break;

      case symbol_kind::S_112_arguments_no_trailing: // arguments.no_trailing
      case symbol_kind::S_arguments: // arguments
      case symbol_kind::S_114_arguments_paren: // arguments.paren
      case symbol_kind::S_115_arguments_maybe_paren: // arguments.maybe_paren
          yylhs.value.emplace<ArgumentListIndex>();
          break;

      case symbol_kind::S_120_literal_array_contents: // literal.array.contents
      case symbol_kind::S_124_literal_array: // literal.array
          yylhs.value.emplace<ArrayIndex>();
          break;

      case symbol_kind::S_94_expr_dotrange: // expr.dotrange
      case symbol_kind::S_96_expr_base_range: // expr.base.range
          yylhs.value.emplace<AtMessageIndex>();
          break;

      case symbol_kind::S_members: // members
      case symbol_kind::S_77_members_opt: // members.opt
          yylhs.value.emplace<BigMemberListIndex>();
          break;

      case symbol_kind::S_87_block_contents: // block.contents
          yylhs.value.emplace<BlockContentsListIndex>();
          break;

      case symbol_kind::S_block: // block
        yylhs.value.emplace< BlockIndex > ();
        break;

      case symbol_kind::S_88_block_contents_item: // block.contents.item
          yylhs.value.emplace<BlockItemIndex>();
          break;

      case symbol_kind::S_85_block_opt_list: // block.opt_list
      case symbol_kind::S_86_block_list: // block.list
          yylhs.value.emplace<BlockListIndex>();
          break;

      case symbol_kind::S_boolean: // boolean
        yylhs.value.emplace< BooleanLitIndex > ();
        break;

      case symbol_kind::S_72_class_extension: // class.extension
          yylhs.value.emplace<ClassExtensionIndex>();
          break;

      case symbol_kind::S_class: // class
        yylhs.value.emplace< ClassIndex > ();
        break;

      case symbol_kind::S_68_classOrExtList_item: // classOrExtList.item
          yylhs.value.emplace<ClassOrExtensionIndex>();
          break;

      case symbol_kind::S_67_classOrExtList_list: // classOrExtList.list
          yylhs.value.emplace<ClassOrExtensionListIndex>();
          break;

      case symbol_kind::S_75_members_entry: // members.entry
          yylhs.value.emplace<DeclareAnyMemberListIndex>();
          break;

      case symbol_kind::S_116_variable_declarations_list_item: // variable_declarations.list.item
          yylhs.value.emplace<DeclareAnyVariableIndex>();
          break;

      case symbol_kind::S_107_argument_declarations_list: // argument_declarations.list
      case symbol_kind::S_108_argument_declarations_pipelist: // argument_declarations.pipelist
      case symbol_kind::S_argument_declarations: // argument_declarations
      case symbol_kind::S_110_argument_declarations_opt: // argument_declarations.opt
          yylhs.value.emplace<DeclareArgumentListIndex>();
          break;

      case symbol_kind::S_73_members_entry_item: // members.entry.item
          yylhs.value.emplace<DeclareMemberIndex>();
          break;

      case symbol_kind::S_74_members_entry_list: // members.entry.list
          yylhs.value.emplace<DeclareMemberListIndex>();
          break;

      case symbol_kind::S_117_variable_declarations_list: // variable_declarations.list
      case symbol_kind::S_variable_declarations: // variable_declarations
        yylhs.value.emplace< DeclareVariableListIndex > ();
        break;

      case symbol_kind::S_121_literal_dictionary_entry: // literal.dictionary.entry
          yylhs.value.emplace<DictionaryEntryIndex>();
          break;

      case symbol_kind::S_122_literal_dictionary_entries: // literal.dictionary.entries
      case symbol_kind::S_123_literal_dictionary: // literal.dictionary
          yylhs.value.emplace<DictionaryIndex>();
          break;

      case symbol_kind::S_file_as_func: // file_as_func
          yylhs.value.emplace<FileAsFunctionIndex>();
          break;

      case symbol_kind::S_135_float_raw_unsigned: // float.raw_unsigned
      case symbol_kind::S_136_float_raw: // float.raw
          yylhs.value.emplace<FloatLitIndex>();
          break;

      case symbol_kind::S_float: // float
        yylhs.value.emplace< FloatProducingIndex > ();
        break;

      case symbol_kind::S_generator: // generator
          yylhs.value.emplace<GeneratorAnyIndex>();
          break;

      case symbol_kind::S_106_generator_qual: // generator.qual
          yylhs.value.emplace<GeneratorQualAnyIndex>();
          break;

      case symbol_kind::S_105_generator_qual_list: // generator.qual.list
          yylhs.value.emplace<GeneratorQualListIndex>();
          break;

      case symbol_kind::S_go: // go
          yylhs.value.emplace<GoIndex>();
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

      case symbol_kind::S_79_method_base: // method.base
          yylhs.value.emplace<MethodIndex>();
          break;

      case symbol_kind::S_81_method_list: // method.list
      case symbol_kind::S_82_method_list_opt: // method.list.opt
          yylhs.value.emplace<MethodListIndex>();
          break;

      case symbol_kind::S_127_name_list: // name.list
          yylhs.value.emplace<NameListIndex>();
          break;

      case symbol_kind::S_128_name_pack: // name.pack
          yylhs.value.emplace<NamePackIndex>();
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

      case symbol_kind::S_95_expr_dotrange_assign: // expr.dotrange.assign
      case symbol_kind::S_97_expr_base_range_assign: // expr.base.range.assign
          yylhs.value.emplace<RangeAssignIndex>();
          break;

      case symbol_kind::S_99_range_generator: // range.generator
          yylhs.value.emplace<RangeGeneratorIndex>();
          break;

      case symbol_kind::S_range: // range
          yylhs.value.emplace<RangeIndex>();
          break;

      case symbol_kind::S_region: // region
        yylhs.value.emplace< RegionListIndex > ();
        break;

      case symbol_kind::S_78_method_name: // method.name
      case symbol_kind::S_90_binary_op_raw: // binary_op.raw
      case symbol_kind::S_91_binary_op_no_adverb: // binary_op.no_adverb
          yylhs.value.emplace<SelectorIndex>();
          break;

      case symbol_kind::S_binary_op: // binary_op
        yylhs.value.emplace< SelectorMaybeAdverbIndex > ();
        break;

      case symbol_kind::S_string: // string
        yylhs.value.emplace< StringLitIndex > ();
        break;

      case symbol_kind::S_65_region_item: // region.item
          yylhs.value.emplace<error_index<AnyExprIndex>>();
          break;

      case symbol_kind::S_70_class_super_opt: // class.super.opt
          yylhs.value.emplace<maybe<ClassNameIdentifierIndex>>();
          break;

      case symbol_kind::S_71_class_slot_opt: // class.slot.opt
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
#line 188 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GoIndex>() = cxt.graph.assign_root(yystack_[2].value.as<RegionListIndex>());
          }
#line 2761 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 3: // go: INTERPRET $end
#line 189 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GoIndex>() = cxt.graph.assign_root(cxt.create(RegionList {}, yylhs.location));
          }
#line 2767 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 4: // go: file_as_func
#line 190 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GoIndex>() = cxt.graph.assign_root(yystack_[0].value.as<FileAsFunctionIndex>());
          }
#line 2773 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 5: // go: classOrExtList.list $end
#line 191 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GoIndex>() = cxt.graph.assign_root(yystack_[1].value.as<ClassOrExtensionListIndex>());
          }
#line 2779 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 6: // go: %empty
#line 192 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GoIndex>() = cxt.graph.assign_root(cxt.create(ClassOrExtensionList {}, yylhs.location));
          }
#line 2785 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 7: // file_as_func: INTERPRET argument_declarations block.contents semicolon.opt $end
#line 201 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FileAsFunctionIndex>() =
                  cxt.create(FileAsFunction {}, yylhs.location, yystack_[3].value.as<DeclareArgumentListIndex>(),
                             yystack_[2].value.as<BlockContentsListIndex>());
          }
#line 2791 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 8: // file_as_func: INTERPRET variable_declarations SEMICOLON block.contents semicolon.opt $end
#line 204 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FileAsFunctionIndex>() =
                  cxt.create(FileAsFunction {}, yylhs.location, cxt.create(ArgumentList {}, yystack_[5].location),
                             cxt.graph.prepend(yystack_[2].value.as<BlockContentsListIndex>(),
                                               yystack_[4].value.as<DeclareVariableListIndex>()));
          }
#line 2797 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 9: // file_as_func: INTERPRET variable_declarations semicolon.opt $end
#line 207 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FileAsFunctionIndex>() =
                  cxt.create(FileAsFunction {}, yylhs.location, cxt.create(ArgumentList {}, yystack_[3].location),
                             cxt.create(BlockContentsList {}, yystack_[1].location,
                                        yystack_[2].value.as<DeclareVariableListIndex>()));
          }
#line 2803 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 10: // region.item: expr
#line 216 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<error_index<AnyExprIndex>>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 2809 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 11: // region.item: OPENPAREN argument_declarations block.contents semicolon.opt CLOSEPAREN
#line 218 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<error_index<AnyExprIndex>>() = cxt.create(ImmediatelyEvaluatedBlock {}, yylhs.location,
                                                                       yystack_[3].value.as<DeclareArgumentListIndex>(),
                                                                       yystack_[2].value.as<BlockContentsListIndex>());
          }
#line 2815 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 12: // region.item: OPENPAREN argument_declarations CLOSEPAREN
#line 221 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<error_index<AnyExprIndex>>() = cxt.create(
                  ImmediatelyEvaluatedBlock {}, yylhs.location, yystack_[1].value.as<DeclareArgumentListIndex>(),
                  cxt.create(BlockContentsList {}, yystack_[0].location));
          }
#line 2821 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 13: // region.item: OPENPAREN variable_declarations SEMICOLON block.contents semicolon.opt CLOSEPAREN
#line 224 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<error_index<AnyExprIndex>>() = cxt.create(
                  ImmediatelyEvaluatedBlock {}, yylhs.location, cxt.create(ArgumentList {}, yystack_[5].location),
                  cxt.graph.prepend(yystack_[2].value.as<BlockContentsListIndex>(),
                                    yystack_[4].value.as<DeclareVariableListIndex>()));
          }
#line 2827 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 14: // region: INTERPRET region.item
#line 229 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RegionListIndex>() =
                  cxt.create(RegionList {}, yylhs.location, yystack_[0].value.as<error_index<AnyExprIndex>>());
          }
#line 2833 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 15: // region: INTERPRET error
#line 232 "langutils/sc_parser/src/sc_grammar.y"
          {
              error_recovery::expr(cxt);
              yylhs.value.as<RegionListIndex>() =
                  cxt.create(RegionList {}, yylhs.location, create_error(cxt, yystack_[0].location));
          }
#line 2842 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 16: // region: region SEMICOLON region.item
#line 238 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RegionListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<RegionListIndex>(), yylhs.location,
                                   yystack_[0].value.as<error_index<AnyExprIndex>>());
          }
#line 2848 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 17: // region: region REGION_SEPARATOR region.item
#line 241 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RegionListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<RegionListIndex>(), yylhs.location,
                                   yystack_[0].value.as<error_index<AnyExprIndex>>());
          }
#line 2854 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 18: // region: region error
#line 244 "langutils/sc_parser/src/sc_grammar.y"
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
#line 2876 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 19: // classOrExtList.list: classOrExtList.item
#line 276 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassOrExtensionListIndex>() =
                  cxt.create(ClassOrExtensionList {}, yylhs.location, yystack_[0].value.as<ClassOrExtensionIndex>());
          }
#line 2882 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 20: // classOrExtList.list: classOrExtList.list classOrExtList.item
#line 278 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassOrExtensionListIndex>() = cxt.graph.append(
                  yystack_[1].value.as<ClassOrExtensionListIndex>(), yystack_[0].value.as<ClassOrExtensionIndex>());
          }
#line 2888 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 21: // classOrExtList.item: class
#line 282 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassOrExtensionIndex>() = yystack_[0].value.as<ClassIndex>();
          }
#line 2894 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 22: // classOrExtList.item: class.extension
#line 283 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassOrExtensionIndex>() = yystack_[0].value.as<ClassExtensionIndex>();
          }
#line 2900 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 23: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY members.opt method.list.opt CLOSECURLY
#line 288 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassIndex>() =
                  cxt.create(Class {}, yylhs.location, cxt.create(ClassNameIdentifier {}, yystack_[6].location),
                             yystack_[5].value.as<maybe<NamedIdentifierIndex>>(),
                             yystack_[4].value.as<maybe<ClassNameIdentifierIndex>>(),
                             yystack_[2].value.as<BigMemberListIndex>(), yystack_[1].value.as<MethodListIndex>());
          }
#line 2906 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 24: // class.super.opt: %empty
#line 292 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<maybe<ClassNameIdentifierIndex>>() = cxt.create(Missing {}, yylhs.location);
          }
#line 2912 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 25: // class.super.opt: COLON CLASSNAME
#line 293 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<maybe<ClassNameIdentifierIndex>>() =
                  cxt.create(ClassNameIdentifier {}, yystack_[0].location);
          }
#line 2918 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 26: // class.slot.opt: %empty
#line 297 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<maybe<NamedIdentifierIndex>>() = cxt.create(Missing {}, yylhs.location);
          }
#line 2924 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 27: // class.slot.opt: OPENSQUARE name CLOSESQUARE
#line 298 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<maybe<NamedIdentifierIndex>>() = yystack_[1].value.as<NamedIdentifierIndex>();
          }
#line 2930 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 28: // class.slot.opt: OPENSQUARE CLOSESQUARE
#line 299 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<maybe<NamedIdentifierIndex>>() = cxt.create(Missing {}, yylhs.location);
          }
#line 2936 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 29: // class.extension: ADD CLASSNAME OPENCURLY method.list.opt CLOSECURLY
#line 304 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ClassExtensionIndex>() = cxt.create(
                  ClassExtension {}, yylhs.location, cxt.create(ClassNameIdentifier {}, yystack_[3].location),
                  yystack_[1].value.as<MethodListIndex>());
          }
#line 2942 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 30: // members.entry.item: accessor variable_declarations.list.item
#line 313 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareMemberIndex>() =
                  cxt.create(DeclareMember {}, yylhs.location, yystack_[1].value.as<RWAccessorIndex>(),
                             yystack_[0].value.as<DeclareAnyVariableIndex>());
          }
#line 2948 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 31: // members.entry.list: members.entry.item
#line 318 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareMemberListIndex>() =
                  cxt.create(DeclareMemberList {}, yylhs.location, yystack_[0].value.as<DeclareMemberIndex>());
          }
#line 2954 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 32: // members.entry.list: members.entry.list COMMA members.entry.item
#line 320 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareMemberListIndex>() = cxt.graph.append(
                  yystack_[2].value.as<DeclareMemberListIndex>(), yystack_[0].value.as<DeclareMemberIndex>());
          }
#line 2960 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 33: // members.entry: CLASSVAR members.entry.list
#line 325 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyMemberListIndex>() =
                  cxt.graph.cast<DeclareClassMemberList>(yystack_[0].value.as<DeclareMemberListIndex>());
          }
#line 2966 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 34: // members.entry: VAR members.entry.list
#line 327 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyMemberListIndex>() =
                  cxt.graph.cast<DeclareMemberList>(yystack_[0].value.as<DeclareMemberListIndex>());
          }
#line 2972 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 35: // members.entry: CONST members.entry.list
#line 329 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyMemberListIndex>() =
                  cxt.graph.cast<DeclareConstList>(yystack_[0].value.as<DeclareMemberListIndex>());
          }
#line 2978 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 36: // members: members.entry
#line 334 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BigMemberListIndex>() =
                  cxt.create(BigMemberList {}, yylhs.location, yystack_[0].value.as<DeclareAnyMemberListIndex>());
          }
#line 2984 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 37: // members: members SEMICOLON members.entry
#line 336 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BigMemberListIndex>() = cxt.graph.append(
                  yystack_[2].value.as<BigMemberListIndex>(), yystack_[0].value.as<DeclareAnyMemberListIndex>());
          }
#line 2990 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 38: // members.opt: %empty
#line 340 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BigMemberListIndex>() = cxt.create(BigMemberList {}, yylhs.location);
          }
#line 2996 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 39: // members.opt: members semicolon.opt
#line 341 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BigMemberListIndex>() = yystack_[1].value.as<BigMemberListIndex>();
          }
#line 3002 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 40: // method.name: name
#line 349 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = yystack_[0].value.as<NamedIdentifierIndex>();
          }
#line 3008 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 41: // method.name: binary_op.raw
#line 350 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = yystack_[0].value.as<SelectorIndex>();
          }
#line 3014 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 42: // method.base: method.name OPENCURLY argument_declarations.opt block.contents semicolon.opt
                   // CLOSECURLY
#line 355 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodIndex>() = cxt.create(
                  Method {}, yylhs.location, yystack_[5].value.as<SelectorIndex>(),
                  yystack_[3].value.as<DeclareArgumentListIndex>(), cxt.create(Missing {}, yystack_[4].location),
                  yystack_[2].value.as<BlockContentsListIndex>());
          }
#line 3020 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 43: // method.base: method.name OPENCURLY argument_declarations.opt CLOSECURLY
#line 358 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodIndex>() = cxt.create(
                  Method {}, yylhs.location, yystack_[3].value.as<SelectorIndex>(),
                  yystack_[1].value.as<DeclareArgumentListIndex>(), cxt.create(Missing {}, yystack_[2].location),
                  cxt.create(BlockList {}, yystack_[0].location));
          }
#line 3026 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 44: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt
                   // block.contents semicolon.opt CLOSECURLY
#line 361 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodIndex>() =
                  cxt.create(Method {}, yylhs.location, yystack_[7].value.as<SelectorIndex>(),
                             yystack_[5].value.as<DeclareArgumentListIndex>(),
                             cxt.create(PrimitiveIdentifier {}, yystack_[4].location),
                             yystack_[2].value.as<BlockContentsListIndex>());
          }
#line 3032 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 45: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt
                   // CLOSECURLY
#line 364 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodIndex>() =
                  cxt.create(Method {}, yylhs.location, yystack_[5].value.as<SelectorIndex>(),
                             yystack_[3].value.as<DeclareArgumentListIndex>(),
                             cxt.create(PrimitiveIdentifier {}, yystack_[2].location),
                             cxt.create(BlockList {}, yystack_[1].location));
          }
#line 3038 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 46: // method: method.base
#line 368 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyMethodIndex>() = yystack_[0].value.as<MethodIndex>();
          }
#line 3044 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 47: // method: MULTIPLY method.base
#line 370 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyMethodIndex>() = cxt.graph.cast<ClassMethod>(yystack_[0].value.as<MethodIndex>());
          }
#line 3050 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 48: // method.list: method
#line 374 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodListIndex>() =
                  cxt.create(MethodList {}, yylhs.location, yystack_[0].value.as<AnyMethodIndex>());
          }
#line 3056 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 49: // method.list: method.list method
#line 375 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodListIndex>() = cxt.graph.append(
                  yystack_[1].value.as<MethodListIndex>(), yylhs.location, yystack_[0].value.as<AnyMethodIndex>());
          }
#line 3062 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 50: // method.list.opt: %empty
#line 379 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodListIndex>() = cxt.create(MethodList {}, yylhs.location);
          }
#line 3068 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 51: // method.list.opt: method.list
#line 380 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<MethodListIndex>() = yystack_[0].value.as<MethodListIndex>();
          }
#line 3074 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 54: // block: block.open argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 392 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockIndex>() =
                  cxt.create(BlockNode {}, yylhs.location, yystack_[3].value.as<DeclareArgumentListIndex>(),
                             yystack_[2].value.as<BlockContentsListIndex>());
          }
#line 3080 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 55: // block: block.open argument_declarations.opt CLOSECURLY
#line 394 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockIndex>() =
                  cxt.create(BlockNode {}, yylhs.location, yystack_[1].value.as<DeclareArgumentListIndex>(),
                             cxt.create(BlockContentsList {}, yylhs.location));
          }
#line 3086 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 56: // block.opt_list: %empty
#line 398 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockListIndex>() = cxt.create(BlockList {}, yylhs.location);
          }
#line 3092 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 57: // block.opt_list: block.list
#line 399 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockListIndex>() = yystack_[0].value.as<BlockListIndex>();
          }
#line 3098 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 58: // block.list: block
#line 403 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockListIndex>() =
                  cxt.create(BlockList {}, yylhs.location, yystack_[0].value.as<BlockIndex>());
          }
#line 3104 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 59: // block.list: generator
#line 404 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockListIndex>() =
                  cxt.create(BlockList {}, yylhs.location, yystack_[0].value.as<GeneratorAnyIndex>());
          }
#line 3110 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 60: // block.list: block.list block
#line 405 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockListIndex>() = cxt.graph.append(yystack_[1].value.as<BlockListIndex>(),
                                                                  yylhs.location, yystack_[0].value.as<BlockIndex>());
          }
#line 3116 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 61: // block.list: block.list generator
#line 406 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockListIndex>() = cxt.graph.append(
                  yystack_[1].value.as<BlockListIndex>(), yylhs.location, yystack_[0].value.as<GeneratorAnyIndex>());
          }
#line 3122 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 62: // block.contents: block.contents.item
#line 410 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockContentsListIndex>() =
                  cxt.create(BlockContentsList {}, yylhs.location, yystack_[0].value.as<BlockItemIndex>());
          }
#line 3128 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 63: // block.contents: block.contents SEMICOLON block.contents.item
#line 411 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockContentsListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<BlockContentsListIndex>(), yylhs.location,
                                   yystack_[0].value.as<BlockItemIndex>());
          }
#line 3134 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 64: // block.contents: block.contents NONLOCALRETURN expr
#line 413 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto non_local =
                  cxt.create(NonLocalReturnExpr {}, yystack_[0].location, yystack_[0].value.as<AnyExprIndex>());
              yylhs.value.as<BlockContentsListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<BlockContentsListIndex>(), yylhs.location, non_local);
          }
#line 3143 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 65: // block.contents: block.contents SEMICOLON NONLOCALRETURN expr
#line 418 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto non_local =
                  cxt.create(NonLocalReturnExpr {}, yystack_[0].location, yystack_[0].value.as<AnyExprIndex>());
              yylhs.value.as<BlockContentsListIndex>() =
                  cxt.graph.append(yystack_[3].value.as<BlockContentsListIndex>(), yylhs.location, non_local);
          }
#line 3152 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 66: // block.contents: NONLOCALRETURN expr
#line 423 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto non_local =
                  cxt.create(NonLocalReturnExpr {}, yystack_[0].location, yystack_[0].value.as<AnyExprIndex>());
              yylhs.value.as<BlockContentsListIndex>() = cxt.create(BlockContentsList {}, yylhs.location, non_local);
          }
#line 3161 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 67: // block.contents.item: expr
#line 430 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockItemIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 3167 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 68: // block.contents.item: variable_declarations
#line 431 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BlockItemIndex>() = yystack_[0].value.as<DeclareVariableListIndex>();
          }
#line 3173 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 69: // adverb: DOT name
#line 440 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AdverbIndex>() = yystack_[0].value.as<NamedIdentifierIndex>();
          }
#line 3179 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 70: // adverb: DOT integer
#line 441 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AdverbIndex>() = yystack_[0].value.as<IntLitIndex>();
          }
#line 3185 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 71: // adverb: DOT OPENPAREN expr_or_seq CLOSEPAREN
#line 442 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AdverbIndex>() =
                  cxt.create(AdverbExprNode {}, yylhs.location, yystack_[1].value.as<AnyExprIndex>());
          }
#line 3191 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 72: // binary_op.raw: BINOP
#line 446 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 3197 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 73: // binary_op.raw: READWRITEVAR
#line 447 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 3203 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 74: // binary_op.raw: LESSTHAN
#line 448 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 3209 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 75: // binary_op.raw: GREATERTHAN
#line 449 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 3215 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 76: // binary_op.raw: MINUS
#line 450 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 3221 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 77: // binary_op.raw: MULTIPLY
#line 451 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 3227 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 78: // binary_op.raw: ADD
#line 452 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 3233 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 79: // binary_op.raw: PIPE
#line 453 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { false }, yylhs.location);
          }
#line 3239 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 80: // binary_op.no_adverb: binary_op.raw
#line 457 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = yystack_[0].value.as<SelectorIndex>();
          }
#line 3245 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 81: // binary_op.no_adverb: KEYBINOP
#line 458 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorIndex>() = cxt.create(SelectorNode { true }, yylhs.location);
          }
#line 3251 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 82: // binary_op: binary_op.no_adverb adverb
#line 463 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorMaybeAdverbIndex>() =
                  cxt.create(SelectorWAdverb {}, yylhs.location, yystack_[1].value.as<SelectorIndex>(),
                             yystack_[0].value.as<AdverbIndex>());
          }
#line 3257 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 83: // binary_op: binary_op.no_adverb
#line 465 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<SelectorMaybeAdverbIndex>() = yystack_[0].value.as<SelectorIndex>();
          }
#line 3263 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 84: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN arguments CLOSEPAREN block.opt_list
#line 470 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(PrefixMessage {}, yylhs.location, yystack_[5].value.as<SelectorIndex>(),
                             yystack_[2].value.as<ArgumentListIndex>(), yystack_[0].value.as<BlockListIndex>());
          }
#line 3269 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 85: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN CLOSEPAREN block.list
#line 473 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(PrefixMessage {}, yylhs.location, yystack_[4].value.as<SelectorIndex>(),
                             cxt.create(ArgumentList {}, yystack_[2].location), yystack_[0].value.as<BlockListIndex>());
          }
#line 3275 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 86: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN block.list
#line 476 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(PrefixMessage {}, yylhs.location, yystack_[2].value.as<SelectorIndex>(),
                             cxt.create(ArgumentList {}, yystack_[0].location), yystack_[0].value.as<BlockListIndex>());
          }
#line 3281 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 87: // msgsend: name OPENPAREN arguments CLOSEPAREN block.opt_list
#line 480 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(PrefixMessage {}, yylhs.location, yystack_[4].value.as<NamedIdentifierIndex>(),
                             yystack_[2].value.as<ArgumentListIndex>(), yystack_[0].value.as<BlockListIndex>());
          }
#line 3287 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 88: // msgsend: name OPENPAREN CLOSEPAREN block.list
#line 483 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(PrefixMessage {}, yylhs.location, yystack_[3].value.as<NamedIdentifierIndex>(),
                             cxt.create(ArgumentList {}, yystack_[2].location), yystack_[0].value.as<BlockListIndex>());
          }
#line 3293 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 89: // msgsend: name block.list
#line 486 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(PrefixMessage {}, yylhs.location, yystack_[1].value.as<NamedIdentifierIndex>(),
                             cxt.create(ArgumentList {}, yystack_[0].location), yystack_[0].value.as<BlockListIndex>());
          }
#line 3299 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 90: // msgsend: expr DOT name arguments.maybe_paren block.opt_list
#line 489 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(PostfixMessage {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<ArgumentListIndex>(),
                             yystack_[0].value.as<BlockListIndex>());
          }
#line 3305 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 91: // msgsend: expr DOT arguments.paren block.opt_list
#line 492 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(ValueMessage {}, yylhs.location, yystack_[3].value.as<AnyExprIndex>(),
                             yystack_[1].value.as<ArgumentListIndex>(), yystack_[0].value.as<BlockListIndex>());
          }
#line 3311 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 92: // msgsend: expr DOT OPENPAREN CLOSEPAREN block.opt_list
#line 495 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(ValueMessage {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             cxt.create(ArgumentList {}, yystack_[2].location), yystack_[0].value.as<BlockListIndex>());
          }
#line 3317 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 93: // msgsend: CLASSNAME OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 505 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = cxt.create(CollectionNode {}, yylhs.location,
                                                          cxt.create(ClassNameIdentifier {}, yystack_[4].location),
                                                          yystack_[2].value.as<ArrayIndex>());
          }
#line 3323 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 94: // msgsend: CLASSNAME block.list
#line 508 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto class_name = cxt.create(ClassNameIdentifier {}, yystack_[1].location);
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(NewMessage {}, yylhs.location, class_name,
                             cxt.create(ArgumentList {}, yystack_[1].location), yystack_[0].value.as<BlockListIndex>());
          }
#line 3332 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 95: // msgsend: CLASSNAME arguments.paren block.opt_list
#line 513 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto class_name = cxt.create(ClassNameIdentifier {}, yystack_[2].location);
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(NewMessage {}, yylhs.location, class_name, yystack_[1].value.as<ArgumentListIndex>(),
                             yystack_[0].value.as<BlockListIndex>());
          }
#line 3341 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 96: // msgsend: CLASSNAME OPENPAREN CLOSEPAREN block.opt_list
#line 519 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto class_name = cxt.create(ClassNameIdentifier {}, yystack_[3].location);
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(NewMessage {}, yylhs.location, class_name,
                             cxt.create(ArgumentList {}, yystack_[3].location), yystack_[0].value.as<BlockListIndex>());
          }
#line 3350 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 97: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 534 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto loc = range(yystack_[4].location, yystack_[0].location);
              const auto r = cxt.create(Range {}, loc, yystack_[3].value.as<ArgumentListIndex>(),
                                        yystack_[1].value.as<AnyExprIndex>());
              yylhs.value.as<AtMessageIndex>() =
                  cxt.create(AtMessage {}, yylhs.location, yystack_[6].value.as<AnyExprIndex>(), r);
          }
#line 3360 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 98: // expr.dotrange: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 541 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto loc = range(yystack_[3].location, yystack_[0].location);
              const auto r = cxt.create(Range {}, loc, cxt.create(ArgumentList {}, yystack_[3].location),
                                        yystack_[1].value.as<AnyExprIndex>());
              yylhs.value.as<AtMessageIndex>() =
                  cxt.create(AtMessage {}, yylhs.location, yystack_[5].value.as<AnyExprIndex>(), r);
          }
#line 3370 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 99: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 548 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto loc = range(yystack_[3].location, yystack_[0].location);
              const auto r = cxt.create(Range {}, loc, yystack_[2].value.as<ArgumentListIndex>(),
                                        cxt.create(Missing {}, yystack_[1].location));
              yylhs.value.as<AtMessageIndex>() =
                  cxt.create(AtMessage {}, yylhs.location, yystack_[5].value.as<AnyExprIndex>(), r);
          }
#line 3380 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 100: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN
                    // expr
#line 557 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto loc = range(yystack_[6].location, yystack_[2].location);
              const auto r = cxt.create(Range {}, loc, yystack_[5].value.as<ArgumentListIndex>(),
                                        yystack_[3].value.as<AnyExprIndex>());
              yylhs.value.as<RangeAssignIndex>() =
                  cxt.create(RangeAssign {}, yylhs.location, yystack_[8].value.as<AnyExprIndex>(), r,
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3390 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 101: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 564 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto loc = range(yystack_[5].location, yystack_[2].location);
              const auto r = cxt.create(Range {}, loc, yystack_[4].value.as<ArgumentListIndex>(),
                                        cxt.create(Missing {}, yystack_[3].location));
              yylhs.value.as<RangeAssignIndex>() =
                  cxt.create(RangeAssign {}, yylhs.location, yystack_[7].value.as<AnyExprIndex>(), r,
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3400 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 102: // expr.dotrange.assign: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 571 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto loc = range(yystack_[5].location, yystack_[2].location);
              const auto r = cxt.create(Range {}, loc, cxt.create(ArgumentList {}, yystack_[5].location),
                                        yystack_[3].value.as<AnyExprIndex>());
              yylhs.value.as<RangeAssignIndex>() =
                  cxt.create(RangeAssign {}, yylhs.location, yystack_[7].value.as<AnyExprIndex>(), r,
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3410 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 103: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 581 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto loc = range(yystack_[4].location, yystack_[0].location);
              const auto r = cxt.create(Range {}, loc, yystack_[3].value.as<ArgumentListIndex>(),
                                        yystack_[1].value.as<AnyExprIndex>());
              yylhs.value.as<AtMessageIndex>() =
                  cxt.create(AtMessage {}, yylhs.location, yystack_[5].value.as<AnyExprIndex>(), r);
          }
#line 3420 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 104: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 587 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto loc = range(yystack_[3].location, yystack_[0].location);
              const auto r = cxt.create(Range {}, loc, yystack_[2].value.as<ArgumentListIndex>(),
                                        cxt.create(Missing {}, yystack_[0].location));
              yylhs.value.as<AtMessageIndex>() =
                  cxt.create(AtMessage {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(), r);
          }
#line 3430 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 105: // expr.base.range: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 593 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto loc = range(yystack_[3].location, yystack_[0].location);
              const auto r = cxt.create(Range {}, loc, cxt.create(ArgumentList {}, yystack_[3].location),
                                        yystack_[1].value.as<AnyExprIndex>());
              yylhs.value.as<AtMessageIndex>() =
                  cxt.create(AtMessage {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(), r);
          }
#line 3440 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 106: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN
                    // expr
#line 602 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto loc = range(yystack_[6].location, yystack_[2].location);
              const auto r = cxt.create(Range {}, loc, yystack_[5].value.as<ArgumentListIndex>(),
                                        yystack_[3].value.as<AnyExprIndex>());
              yylhs.value.as<RangeAssignIndex>() =
                  cxt.create(RangeAssign {}, yylhs.location, yystack_[7].value.as<AnyExprIndex>(), r,
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3450 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 107: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 608 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto loc = range(yystack_[5].location, yystack_[2].location);
              const auto r = cxt.create(Range {}, loc, yystack_[4].value.as<ArgumentListIndex>(),
                                        cxt.create(Missing {}, yystack_[3].location));
              yylhs.value.as<RangeAssignIndex>() =
                  cxt.create(RangeAssign {}, yylhs.location, yystack_[6].value.as<AnyExprIndex>(), r,
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3460 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 108: // expr.base.range.assign: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 614 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto loc = range(yystack_[5].location, yystack_[2].location);
              const auto r = cxt.create(Range {}, loc, cxt.create(ArgumentList {}, yystack_[5].location),
                                        yystack_[3].value.as<AnyExprIndex>());
              yylhs.value.as<RangeAssignIndex>() =
                  cxt.create(RangeAssign {}, yylhs.location, yystack_[6].value.as<AnyExprIndex>(), r,
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3470 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 109: // range: OPENPAREN expr_or_seq DOTDOT CLOSEPAREN
#line 623 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto args = cxt.create(ArgumentList {}, yystack_[2].location, yystack_[2].value.as<AnyExprIndex>());
              yylhs.value.as<RangeIndex>() = cxt.create(Range {}, range(yystack_[3].location, yystack_[0].location),
                                                        args, cxt.create(Missing {}, yystack_[1].location));
          }
#line 3479 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 110: // range: OPENPAREN expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 628 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto args = cxt.create(ArgumentList {}, yystack_[3].location, yystack_[3].value.as<AnyExprIndex>());
              yylhs.value.as<RangeIndex>() = cxt.create(Range {}, range(yystack_[4].location, yystack_[0].location),
                                                        args, yystack_[1].value.as<AnyExprIndex>());
          }
#line 3488 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 111: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 633 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto args = cxt.create(ArgumentList {}, range(yystack_[5].location, yystack_[3].location),
                                           yystack_[5].value.as<AnyExprIndex>(), yystack_[3].value.as<AnyExprIndex>());
              yylhs.value.as<RangeIndex>() = cxt.create(Range {}, range(yystack_[6].location, yystack_[0].location),
                                                        args, yystack_[1].value.as<AnyExprIndex>());
          }
#line 3497 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 112: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 638 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto args = cxt.create(ArgumentList {}, range(yystack_[4].location, yystack_[2].location),
                                           yystack_[4].value.as<AnyExprIndex>(), yystack_[2].value.as<AnyExprIndex>());
              yylhs.value.as<RangeIndex>() = cxt.create(Range {}, range(yystack_[5].location, yystack_[0].location),
                                                        args, cxt.create(Missing {}, yystack_[1].location));
          }
#line 3506 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 113: // range: OPENPAREN DOTDOT expr_or_seq CLOSEPAREN
#line 643 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto args = cxt.create(ArgumentList {}, yystack_[2].location);
              yylhs.value.as<RangeIndex>() = cxt.create(Range {}, range(yystack_[3].location, yystack_[0].location),
                                                        args, yystack_[1].value.as<AnyExprIndex>());
          }
#line 3515 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 114: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 651 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto args = cxt.create(ArgumentList {}, yystack_[3].location, yystack_[3].value.as<AnyExprIndex>());
              yylhs.value.as<RangeGeneratorIndex>() =
                  cxt.create(RangeGenerator {}, range(yystack_[5].location, yystack_[0].location), args,
                             yystack_[1].value.as<AnyExprIndex>());
          }
#line 3524 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 115: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 656 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto args = cxt.create(ArgumentList {}, range(yystack_[5].location, yystack_[3].location),
                                           yystack_[5].value.as<AnyExprIndex>(), yystack_[3].value.as<AnyExprIndex>());
              yylhs.value.as<RangeGeneratorIndex>() =
                  cxt.create(RangeGenerator {}, range(yystack_[7].location, yystack_[0].location), args,
                             yystack_[1].value.as<AnyExprIndex>());
          }
#line 3533 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 116: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 662 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto args = cxt.create(ArgumentList {}, yystack_[4].location, yystack_[4].value.as<AnyExprIndex>(),
                                           yystack_[2].value.as<AnyExprIndex>());
              yylhs.value.as<RangeGeneratorIndex>() =
                  cxt.create(RangeGenerator {}, range(yystack_[6].location, yystack_[0].location), args,
                             cxt.create(Missing {}, yystack_[1].location));
          }
#line 3542 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 117: // range.generator: OPENPAREN COLON DOTDOT expr_or_seq CLOSEPAREN
#line 667 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto args = cxt.create(ArgumentList {}, yystack_[2].location);
              yylhs.value.as<RangeGeneratorIndex>() =
                  cxt.create(RangeGenerator {}, range(yystack_[4].location, yystack_[0].location), args,
                             yystack_[1].value.as<AnyExprIndex>());
          }
#line 3551 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 118: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT CLOSEPAREN
#line 672 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto args = cxt.create(ArgumentList {}, yystack_[2].location, yystack_[2].value.as<AnyExprIndex>());
              yylhs.value.as<RangeGeneratorIndex>() =
                  cxt.create(RangeGenerator {}, range(yystack_[4].location, yystack_[0].location), args,
                             cxt.create(Missing {}, yystack_[1].location));
          }
#line 3560 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 119: // expr.base: literal
#line 685 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<AnyPossiblyLiteralIndex>();
          }
#line 3566 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 120: // expr.base: generator
#line 688 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<GeneratorAnyIndex>();
          }
#line 3572 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 121: // expr.base: name
#line 691 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<NamedIdentifierIndex>();
          }
#line 3578 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 122: // expr.base: CURRYARG
#line 694 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = cxt.create(CurryNode {}, yystack_[0].location);
          }
#line 3584 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 123: // expr.base: msgsend
#line 697 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 3590 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 124: // expr.base: OPENPAREN expr_or_seq CLOSEPAREN
#line 700 "langutils/sc_parser/src/sc_grammar.y"
          {
              // A nasty hack because of nil assignment.
              cxt.graph.register_bracketed_expr(yystack_[1].value.as<AnyExprIndex>());
              yylhs.value.as<AnyExprIndex>() = yystack_[1].value.as<AnyExprIndex>();
          }
#line 3600 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 125: // expr.base: TILDE name
#line 707 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(EnvIdentifierNode {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3606 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 126: // expr.base: expr.base OPENSQUARE arguments CLOSESQUARE
#line 710 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AtMessage {}, yylhs.location, yystack_[3].value.as<AnyExprIndex>(),
                             yystack_[1].value.as<ArgumentListIndex>());
          }
#line 3612 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 127: // expr.base: expr.base OPENSQUARE CLOSESQUARE
#line 713 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AtMessage {}, yylhs.location, yystack_[2].value.as<AnyExprIndex>(),
                             cxt.create(ArgumentList {}, yystack_[1].location));
          }
#line 3618 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 128: // expr.base: expr.base.range
#line 716 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<AtMessageIndex>();
          }
#line 3624 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 129: // expr.base: range
#line 719 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<RangeIndex>();
          }
#line 3630 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 130: // expr.base: range.generator
#line 722 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<RangeGeneratorIndex>();
          }
#line 3636 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 131: // expr: expr.base
#line 727 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 3642 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 132: // expr: expr.base.range.assign
#line 728 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<RangeAssignIndex>();
          }
#line 3648 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 133: // expr: expr.dotrange
#line 729 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<AtMessageIndex>();
          }
#line 3654 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 134: // expr: expr.dotrange.assign
#line 730 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<RangeAssignIndex>();
          }
#line 3660 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 135: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE
#line 733 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AtMessage {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             yystack_[1].value.as<ArgumentListIndex>());
          }
#line 3666 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 136: // expr: expr DOT OPENSQUARE CLOSESQUARE
#line 736 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AtMessage {}, yylhs.location, yystack_[3].value.as<AnyExprIndex>(),
                             cxt.create(ArgumentList {}, yystack_[1].location));
          }
#line 3672 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 137: // expr: expr binary_op expr
#line 739 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(BinaryMessage {}, yylhs.location, yystack_[1].value.as<SelectorMaybeAdverbIndex>(),
                             yystack_[2].value.as<AnyExprIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3678 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 138: // expr: expr DOT name EQUALSSIGN expr
#line 742 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(SetterNode {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3684 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 139: // expr: CLASSNAME
#line 744 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = cxt.create(ClassNameIdentifier {}, yylhs.location);
          }
#line 3690 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 140: // expr: BACKTICK expr
#line 745 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(ReferenceNode {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 3696 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 141: // expr: name EQUALSSIGN expr
#line 747 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentNode {}, yylhs.location, yystack_[2].value.as<NamedIdentifierIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3702 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 142: // expr: TILDE name EQUALSSIGN expr
#line 750 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentNode { AssignmentNode::Target::Environment }, yylhs.location,
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3708 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 143: // expr: name OPENPAREN arguments CLOSEPAREN EQUALSSIGN expr
#line 753 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(SetterNode {}, yylhs.location, yystack_[3].value.as<ArgumentListIndex>(),
                             yystack_[5].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3714 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 144: // expr: name.pack EQUALSSIGN expr
#line 756 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(VariablePackAssign {}, yylhs.location, yystack_[2].value.as<NamePackIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3720 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 145: // expr: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 759 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(VariablePackAssign {}, yylhs.location, yystack_[3].value.as<NamePackIndex>(),
                             yystack_[1].value.as<AnyExprIndex>());
          }
#line 3726 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 146: // expr: expr.base OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 765 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentAtNode {}, yylhs.location, yystack_[5].value.as<AnyExprIndex>(),
                             yystack_[3].value.as<ArgumentListIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3732 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 147: // expr: expr.base OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 768 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentAtNode {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             cxt.create(ArgumentList {}, yystack_[3].location), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3738 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 148: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 771 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentAtNode {}, yylhs.location, yystack_[6].value.as<AnyExprIndex>(),
                             yystack_[3].value.as<ArgumentListIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3744 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 149: // expr: expr DOT OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 774 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(AssignmentAtNode {}, yylhs.location, yystack_[5].value.as<AnyExprIndex>(),
                             cxt.create(ArgumentList {}, yystack_[3].location), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3750 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 150: // expr_or_seq.base: expr
#line 781 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 3756 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 151: // expr_or_seq.base: expr_or_seq.base SEMICOLON expr
#line 782 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() =
                  cxt.create(ExprSeq {}, yylhs.location, yystack_[2].value.as<AnyExprIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3762 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 152: // expr_or_seq: expr_or_seq.base semicolon.opt
#line 786 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyExprIndex>() = yystack_[1].value.as<AnyExprIndex>();
          }
#line 3768 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 153: // generator: OPENCURLY COLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 795 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorAnyIndex>() =
                  cxt.create(GeneratorNode {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             yystack_[2].value.as<GeneratorQualListIndex>());
          }
#line 3774 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 154: // generator: OPENCURLY SEMICOLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 798 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorAnyIndex>() =
                  cxt.create(GeneratorFakeNode {}, yylhs.location, yystack_[4].value.as<AnyExprIndex>(),
                             yystack_[2].value.as<GeneratorQualListIndex>());
          }
#line 3780 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 155: // generator.qual.list: generator.qual
#line 803 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualListIndex>() =
                  cxt.create(GeneratorQualList {}, yylhs.location, yystack_[0].value.as<GeneratorQualAnyIndex>());
          }
#line 3786 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 156: // generator.qual.list: generator.qual.list COMMA generator.qual
#line 805 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<GeneratorQualListIndex>(), yystack_[2].location,
                                   yystack_[0].value.as<GeneratorQualAnyIndex>());
          }
#line 3792 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 157: // generator.qual: name LEFTARROW expr_or_seq
#line 810 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualAnyIndex>() =
                  cxt.create(GeneratorQualAssign {}, yylhs.location, yystack_[2].value.as<NamedIdentifierIndex>(),
                             cxt.create(Missing {}, yystack_[1].location), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3798 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 158: // generator.qual: name name LEFTARROW expr_or_seq
#line 812 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualAnyIndex>() =
                  cxt.create(GeneratorQualAssign {}, yylhs.location, yystack_[3].value.as<NamedIdentifierIndex>(),
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 3804 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 159: // generator.qual: VAR name EQUALSSIGN expr_or_seq
#line 814 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualAnyIndex>() =
                  cxt.create(GeneratorQualDeclare {}, yylhs.location, yystack_[2].value.as<NamedIdentifierIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 3810 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 160: // generator.qual: VAR name OPENPAREN expr_or_seq CLOSEPAREN
#line 816 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualAnyIndex>() =
                  cxt.create(GeneratorQualDeclare {}, yylhs.location, yystack_[3].value.as<NamedIdentifierIndex>(),
                             yystack_[1].value.as<AnyExprIndex>());
          }
#line 3816 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 161: // generator.qual: expr_or_seq
#line 818 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualAnyIndex>() =
                  cxt.create(GeneratorQualIf {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 3822 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 162: // generator.qual: COLON COLON expr_or_seq
#line 820 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualAnyIndex>() =
                  cxt.create(GeneratorQualSideEffect {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 3828 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 163: // generator.qual: COLON WHILE expr_or_seq
#line 822 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<GeneratorQualAnyIndex>() =
                  cxt.create(GeneratorQualGuard {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 3834 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 164: // argument_declarations.list: name
#line 835 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.create(DeclareArgumentList {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3840 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 165: // argument_declarations.list: name EQUALSSIGN expr
#line 837 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto is_bracketed = cxt.graph.is_bracketed(yystack_[0].value.as<AnyExprIndex>());
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(
                  DeclareArgumentList {}, yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { !is_bracketed }, yylhs.location,
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>()));
          }
#line 3849 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 166: // argument_declarations.list: name OPENPAREN expr_or_seq CLOSEPAREN
#line 842 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(
                  DeclareArgumentList {}, yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[3].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3855 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 167: // argument_declarations.list: argument_declarations.list COMMA name
#line 845 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3861 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 168: // argument_declarations.list: argument_declarations.list COMMA name EQUALSSIGN expr
#line 848 "langutils/sc_parser/src/sc_grammar.y"
          {
              const auto is_bracketed = cxt.graph.is_bracketed(yystack_[0].value.as<AnyExprIndex>());
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.graph.append(
                  yystack_[4].value.as<DeclareArgumentListIndex>(), yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { !is_bracketed }, yylhs.location,
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>()));
          }
#line 3870 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 169: // argument_declarations.list: argument_declarations.list COMMA name OPENPAREN expr_or_seq
                    // CLOSEPAREN
#line 853 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.graph.append(
                  yystack_[5].value.as<DeclareArgumentListIndex>(), yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[1].value.as<AnyExprIndex>(), yystack_[3].value.as<NamedIdentifierIndex>()));
          }
#line 3876 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 170: // argument_declarations.pipelist: name literal
#line 859 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.create(DeclareArgumentList {}, yylhs.location,
                             cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                        yystack_[1].value.as<NamedIdentifierIndex>(),
                                        yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3882 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 171: // argument_declarations.pipelist: name
#line 861 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.create(DeclareArgumentList {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3888 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 172: // argument_declarations.pipelist: name EQUALSSIGN literal
#line 863 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.create(DeclareArgumentList {}, yylhs.location,
                             cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                        yystack_[2].value.as<NamedIdentifierIndex>(),
                                        yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3894 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 173: // argument_declarations.pipelist: name OPENPAREN expr_or_seq CLOSEPAREN
#line 865 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(
                  DeclareArgumentList {}, yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[3].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3900 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 174: // argument_declarations.pipelist: name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 867 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(
                  DeclareArgumentList {}, yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[4].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3906 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 175: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name literal
#line 869 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[3].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                              yystack_[1].value.as<NamedIdentifierIndex>(),
                                              yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3912 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 176: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name
#line 871 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 3918 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 177: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN literal
#line 873 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[4].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentWithDefaultNode { true }, yylhs.location,
                                              yystack_[2].value.as<NamedIdentifierIndex>(),
                                              yystack_[0].value.as<AnyPossiblyLiteralIndex>()));
          }
#line 3924 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 178: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name OPENPAREN
                    // expr_or_seq CLOSEPAREN
#line 875 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.graph.append(
                  yystack_[5].value.as<DeclareArgumentListIndex>(), yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[3].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3930 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 179: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN
                    // OPENPAREN expr_or_seq CLOSEPAREN
#line 877 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.graph.append(
                  yystack_[6].value.as<DeclareArgumentListIndex>(), yylhs.location,
                  cxt.create(DeclareArgumentWithDefaultNode { false }, yylhs.location,
                             yystack_[4].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>()));
          }
#line 3936 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 180: // argument_declarations: ARG SEMICOLON
#line 881 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(DeclareArgumentList {}, yylhs.location);
          }
#line 3942 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 181: // argument_declarations: ARG argument_declarations.list comma.opt SEMICOLON
#line 882 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = yystack_[2].value.as<DeclareArgumentListIndex>();
          }
#line 3948 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 182: // argument_declarations: ARG argument_declarations.list ELLIPSIS name SEMICOLON
#line 884 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[3].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3954 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 183: // argument_declarations: ARG argument_declarations.list ELLIPSIS name COMMA name SEMICOLON
#line 886 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[5].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[3].location,
                                              yystack_[3].value.as<NamedIdentifierIndex>()),
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3960 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 184: // argument_declarations: ARG ELLIPSIS name SEMICOLON
#line 888 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto arg_list = cxt.create(DeclareArgumentList {}, yylhs.location);
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(arg_list, yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3969 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 185: // argument_declarations: ARG ELLIPSIS name COMMA name SEMICOLON
#line 893 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto arg_list = cxt.create(DeclareArgumentList {}, yylhs.location);
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(arg_list, yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[3].location,
                                              yystack_[3].value.as<NamedIdentifierIndex>()),
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 3982 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 186: // argument_declarations: PIPE PIPE
#line 902 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(DeclareArgumentList {}, yylhs.location);
          }
#line 3988 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 187: // argument_declarations: PIPE argument_declarations.pipelist comma.opt PIPE
#line 904 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = yystack_[2].value.as<DeclareArgumentListIndex>();
          }
#line 3994 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 188: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name PIPE
#line 906 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[3].value.as<DeclareArgumentListIndex>(), yystack_[3].location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 4000 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 189: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name COMMA name PIPE
#line 908 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(yystack_[5].value.as<DeclareArgumentListIndex>(), yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[3].location,
                                              yystack_[3].value.as<NamedIdentifierIndex>()),
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 4006 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 190: // argument_declarations: PIPE ELLIPSIS name PIPE
#line 910 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto arg_list = cxt.create(DeclareArgumentList {}, yylhs.location);
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(arg_list, yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 4015 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 191: // argument_declarations: PIPE ELLIPSIS name COMMA name PIPE
#line 915 "langutils/sc_parser/src/sc_grammar.y"
          {
              auto arg_list = cxt.create(DeclareArgumentList {}, yylhs.location);
              yylhs.value.as<DeclareArgumentListIndex>() =
                  cxt.graph.append(arg_list, yylhs.location,
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[3].location,
                                              yystack_[3].value.as<NamedIdentifierIndex>()),
                                   cxt.create(DeclareArgumentVariadicNode {}, yystack_[1].location,
                                              yystack_[1].value.as<NamedIdentifierIndex>()));
          }
#line 4028 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 192: // argument_declarations.opt: %empty
#line 926 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = cxt.create(DeclareArgumentList {}, yylhs.location);
          }
#line 4034 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 193: // argument_declarations.opt: argument_declarations
#line 927 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareArgumentListIndex>() = yystack_[0].value.as<DeclareArgumentListIndex>();
          }
#line 4040 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 194: // arguments.entries: KEYBINOP expr_or_seq
#line 932 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentEntryIndex>() =
                  cxt.create(KwArgNode {}, yylhs.location, cxt.create(KeyBinOp {}, yystack_[1].location),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 4046 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 195: // arguments.entries: MULTIPLY expr_or_seq
#line 933 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentEntryIndex>() =
                  cxt.create(VariadicArgNode {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 4052 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 196: // arguments.entries: expr_or_seq
#line 934 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentEntryIndex>() = yystack_[0].value.as<AnyExprIndex>();
          }
#line 4058 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 197: // arguments.no_trailing: arguments.entries
#line 938 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() =
                  cxt.create(ArgumentList {}, yylhs.location, yystack_[0].value.as<ArgumentEntryIndex>());
          }
#line 4064 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 198: // arguments.no_trailing: arguments.no_trailing COMMA arguments.entries
#line 939 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() =
                  cxt.graph.append(yystack_[2].value.as<ArgumentListIndex>(), yylhs.location,
                                   yystack_[0].value.as<ArgumentEntryIndex>());
          }
#line 4070 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 199: // arguments: arguments.no_trailing comma.opt
#line 942 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = yystack_[1].value.as<ArgumentListIndex>();
          }
#line 4076 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 200: // arguments.paren: OPENPAREN arguments CLOSEPAREN
#line 944 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = yystack_[1].value.as<ArgumentListIndex>();
          }
#line 4082 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 201: // arguments.maybe_paren: %empty
#line 947 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = cxt.create(ArgumentList {}, yylhs.location);
          }
#line 4088 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 202: // arguments.maybe_paren: OPENPAREN CLOSEPAREN
#line 949 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = cxt.create(ArgumentList {}, yylhs.location);
          }
#line 4094 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 203: // arguments.maybe_paren: OPENPAREN arguments CLOSEPAREN
#line 951 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArgumentListIndex>() = yystack_[1].value.as<ArgumentListIndex>();
          }
#line 4100 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 204: // variable_declarations.list.item: name
#line 960 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() = yystack_[0].value.as<NamedIdentifierIndex>();
          }
#line 4106 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 205: // variable_declarations.list.item: name EQUALSSIGN expr
#line 962 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() =
                  cxt.create(DeclareVariableWithDefaultNode {}, yylhs.location,
                             yystack_[2].value.as<NamedIdentifierIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 4112 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 206: // variable_declarations.list.item: name OPENPAREN expr_or_seq CLOSEPAREN
#line 964 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() =
                  cxt.create(DeclareVariableWithDefaultNode {}, yylhs.location,
                             yystack_[3].value.as<NamedIdentifierIndex>(), yystack_[1].value.as<AnyExprIndex>());
          }
#line 4118 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 207: // variable_declarations.list.item: name.pack EQUALSSIGN expr
#line 966 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() =
                  cxt.create(VariablePackAssign {}, yylhs.location, yystack_[2].value.as<NamePackIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 4124 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 208: // variable_declarations.list.item: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 968 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareAnyVariableIndex>() =
                  cxt.create(VariablePackAssign {}, yylhs.location, yystack_[3].value.as<NamePackIndex>(),
                             yystack_[1].value.as<AnyExprIndex>());
          }
#line 4130 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 209: // variable_declarations.list: variable_declarations.list.item
#line 973 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareVariableListIndex>() =
                  cxt.create(DeclareVariableList {}, yylhs.location, yystack_[0].value.as<DeclareAnyVariableIndex>());
          }
#line 4136 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 210: // variable_declarations.list: variable_declarations.list COMMA variable_declarations.list.item
#line 975 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareVariableListIndex>() = cxt.graph.append(
                  yystack_[2].value.as<DeclareVariableListIndex>(), yystack_[0].value.as<DeclareAnyVariableIndex>());
          }
#line 4142 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 211: // variable_declarations: VAR variable_declarations.list
#line 978 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DeclareVariableListIndex>() = yystack_[0].value.as<DeclareVariableListIndex>();
          }
#line 4148 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 212: // literal.terminal: symbol
#line 986 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<AnySymbolIndex>();
          }
#line 4154 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 213: // literal.terminal: string
#line 987 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<StringLitIndex>();
          }
#line 4160 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 214: // literal.terminal: integer
#line 988 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<IntLitIndex>();
          }
#line 4166 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 215: // literal.terminal: float
#line 989 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<FloatProducingIndex>();
          }
#line 4172 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 216: // literal.terminal: boolean
#line 990 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<BooleanLitIndex>();
          }
#line 4178 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 217: // literal.terminal: nil
#line 991 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<NilLitIndex>();
          }
#line 4184 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 218: // literal.terminal: ascii
#line 992 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<ASCIIIndex>();
          }
#line 4190 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 219: // literal.terminal: block
#line 993 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<BlockIndex>();
          }
#line 4196 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 220: // literal.array.contents: %empty
#line 998 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() = cxt.create(ArrayNode {}, yylhs.location);
          }
#line 4202 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 221: // literal.array.contents: expr_or_seq
#line 1000 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.create(ArrayNode {}, yylhs.location, yystack_[0].value.as<AnyExprIndex>());
          }
#line 4208 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 222: // literal.array.contents: expr_or_seq COLON expr_or_seq
#line 1002 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.create(ArrayNode {}, yylhs.location, yystack_[2].value.as<AnyExprIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 4214 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 223: // literal.array.contents: KEYBINOP expr_or_seq
#line 1004 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.create(ArrayNode {}, yylhs.location, cxt.create(KeyBinOp {}, yystack_[1].location),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 4220 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 224: // literal.array.contents: literal.array.contents COMMA expr_or_seq
#line 1006 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() = cxt.graph.append(yystack_[2].value.as<ArrayIndex>(), yylhs.location,
                                                              yystack_[0].value.as<AnyExprIndex>());
          }
#line 4226 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 225: // literal.array.contents: literal.array.contents COMMA expr_or_seq COLON expr_or_seq
#line 1008 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.graph.append(yystack_[4].value.as<ArrayIndex>(), yylhs.location,
                                   yystack_[2].value.as<AnyExprIndex>(), yystack_[0].value.as<AnyExprIndex>());
          }
#line 4232 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 226: // literal.array.contents: literal.array.contents COMMA KEYBINOP expr_or_seq
#line 1010 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ArrayIndex>() =
                  cxt.graph.append(yystack_[3].value.as<ArrayIndex>(), yylhs.location,
                                   cxt.create(KeyBinOp {}, yystack_[1].location), yystack_[0].value.as<AnyExprIndex>());
          }
#line 4238 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 227: // literal.dictionary.entry: expr COLON expr_or_seq
#line 1018 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryEntryIndex>() =
                  cxt.create(DictionaryEntryNode {}, yylhs.location, yystack_[2].value.as<AnyExprIndex>(),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 4244 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 228: // literal.dictionary.entry: KEYBINOP expr_or_seq
#line 1020 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryEntryIndex>() =
                  cxt.create(DictionaryEntryNode {}, yylhs.location, cxt.create(KeyBinOp {}, yystack_[1].location),
                             yystack_[0].value.as<AnyExprIndex>());
          }
#line 4250 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 229: // literal.dictionary.entries: %empty
#line 1025 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryIndex>() = cxt.create(DictionaryNode {}, yylhs.location);
          }
#line 4256 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 230: // literal.dictionary.entries: literal.dictionary.entry
#line 1027 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryIndex>() =
                  cxt.create(DictionaryNode {}, yylhs.location, yystack_[0].value.as<DictionaryEntryIndex>());
          }
#line 4262 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 231: // literal.dictionary.entries: literal.dictionary.entries COMMA literal.dictionary.entry
#line 1029 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<DictionaryIndex>() =
                  cxt.graph.append(yystack_[2].value.as<DictionaryIndex>(), yylhs.location,
                                   yystack_[0].value.as<DictionaryEntryIndex>());
          }
#line 4268 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 232: // literal.dictionary: OPENPAREN literal.dictionary.entries comma.opt CLOSEPAREN
#line 1034 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.location(*yystack_[2].value.as<DictionaryIndex>()) = yylhs.location;
              yylhs.value.as<DictionaryIndex>() = yystack_[2].value.as<DictionaryIndex>();
          }
#line 4274 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 233: // literal.array: OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1039 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.location(*yystack_[2].value.as<ArrayIndex>()) = yylhs.location;
              yylhs.value.as<ArrayIndex>() = yystack_[2].value.as<ArrayIndex>();
          }
#line 4280 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 234: // literal.array: HASH OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1041 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.payload(yystack_[2].value.as<ArrayIndex>()).is_immutable = true;
              cxt.graph.location(*yystack_[2].value.as<ArrayIndex>()) = yylhs.location;
              yylhs.value.as<ArrayIndex>() = yystack_[2].value.as<ArrayIndex>();
          }
#line 4290 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 235: // literal: literal.terminal
#line 1049 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<AnyPossiblyLiteralIndex>();
          }
#line 4296 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 236: // literal: literal.array
#line 1050 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<ArrayIndex>();
          }
#line 4302 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 237: // literal: literal.dictionary
#line 1051 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnyPossiblyLiteralIndex>() = yystack_[0].value.as<DictionaryIndex>();
          }
#line 4308 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 238: // name: NAME
#line 1059 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NamedIdentifierIndex>() = cxt.create(NamedIdentifier {}, yylhs.location);
          }
#line 4314 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 239: // name: WHILE
#line 1060 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NamedIdentifierIndex>() = cxt.create(NamedIdentifier {}, yylhs.location);
          }
#line 4320 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 240: // name.list: HASH name
#line 1066 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NameListIndex>() =
                  cxt.create(NameList {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 4326 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 241: // name.list: HASH TILDE name
#line 1068 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NameListIndex>() = cxt.create(
                  NameList {}, yylhs.location,
                  cxt.create(EnvIdentifierNode {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>()));
          }
#line 4332 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 242: // name.list: name.list COMMA name
#line 1070 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NameListIndex>() = cxt.graph.append(yystack_[2].value.as<NameListIndex>(), yylhs.location,
                                                                 yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 4338 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 243: // name.list: name.list COMMA TILDE name
#line 1072 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NameListIndex>() = cxt.graph.append(
                  yystack_[3].value.as<NameListIndex>(), yylhs.location,
                  cxt.create(EnvIdentifierNode {}, yylhs.location, yystack_[0].value.as<NamedIdentifierIndex>()));
          }
#line 4344 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 244: // name.pack: name.list
#line 1076 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NamePackIndex>() =
                  cxt.create(NamePack {}, yylhs.location, yystack_[0].value.as<NameListIndex>(),
                             cxt.create(Missing {}, yystack_[0].location));
          }
#line 4350 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 245: // name.pack: name.list ELLIPSIS name
#line 1078 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NamePackIndex>() =
                  cxt.create(NamePack {}, yylhs.location, yystack_[2].value.as<NameListIndex>(),
                             yystack_[0].value.as<NamedIdentifierIndex>());
          }
#line 4356 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 246: // name.pack: name.list ELLIPSIS TILDE name
#line 1080 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NamePackIndex>() =
                  cxt.create(NamePack {}, yylhs.location, yystack_[3].value.as<NameListIndex>(),
                             cxt.create(EnvIdentifierNode {}, range(yystack_[1].location, yystack_[0].location),
                                        yystack_[0].value.as<NamedIdentifierIndex>()));
          }
#line 4362 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 247: // ascii: ASCII
#line 1090 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<ASCIIIndex>() = cxt.create(ASCIINode {}, yylhs.location);
          }
#line 4368 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 248: // nil: NIL
#line 1092 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<NilLitIndex>() = cxt.create(NilNode {}, yylhs.location);
          }
#line 4374 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 249: // boolean: TRUE
#line 1095 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BooleanLitIndex>() = cxt.create(BooleanNode { true }, yylhs.location);
          }
#line 4380 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 250: // boolean: FALSE
#line 1096 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<BooleanLitIndex>() = cxt.create(BooleanNode { false }, yylhs.location);
          }
#line 4386 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 251: // symbol: SYMBOL_QUOTE
#line 1100 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnySymbolIndex>() = cxt.create(SymbolQuote {}, yylhs.location);
          }
#line 4392 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 252: // symbol: SYMBOL_SLASH
#line 1101 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AnySymbolIndex>() = cxt.create(SymbolSlash {}, yylhs.location);
          }
#line 4398 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 253: // string: STRINGLINE
#line 1105 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<StringLitIndex>() =
                  cxt.create(StringLineList {}, yylhs.location, cxt.create(StringLineNode {}, yylhs.location));
          }
#line 4404 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 254: // string: string STRINGLINE
#line 1106 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<StringLitIndex>() = cxt.graph.append(yystack_[1].value.as<StringLitIndex>(),
                                                                  cxt.create(StringLineNode {}, yystack_[0].location));
          }
#line 4410 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 255: // integer: INTEGER
#line 1110 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<IntLitIndex>() = cxt.create(IntNode {}, yylhs.location);
          }
#line 4416 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 256: // integer: INTEGER_RADIX
#line 1111 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<IntLitIndex>() = cxt.create(IntNode { IntNode::Kind::Radix }, yylhs.location);
          }
#line 4422 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 257: // integer: HEXADECIMAL
#line 1112 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<IntLitIndex>() = cxt.create(IntNode { IntNode::Kind::Hexadecimal }, yylhs.location);
          }
#line 4428 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 258: // integer: MINUS integer
#line 1114 "langutils/sc_parser/src/sc_grammar.y"
          {
              // Reaches into the previous integer and changes its sign.
              cxt.graph.payload(yystack_[0].value.as<IntLitIndex>()).sign = IntNode::Sign::Negative;
              yylhs.value.as<IntLitIndex>() = yystack_[0].value.as<IntLitIndex>();
          }
#line 4438 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 259: // float.raw_unsigned: FLOAT
#line 1122 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = cxt.create(FloatNode {}, yylhs.location);
          }
#line 4444 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 260: // float.raw_unsigned: FLOAT_RADIX
#line 1123 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = cxt.create(FloatNode { FloatNode::Kind::Radix }, yylhs.location);
          }
#line 4450 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 261: // float.raw_unsigned: FLOAT_EXPONENT
#line 1124 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = cxt.create(FloatNode { FloatNode::Kind::Exponent }, yylhs.location);
          }
#line 4456 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 262: // float.raw_unsigned: FLOAT_INF
#line 1125 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = cxt.create(FloatNode { FloatNode::Kind::Inf }, yylhs.location);
          }
#line 4462 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 263: // float.raw: float.raw_unsigned
#line 1130 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatLitIndex>() = yystack_[0].value.as<FloatLitIndex>();
          }
#line 4468 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 264: // float.raw: MINUS float.raw_unsigned
#line 1132 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.payload(yystack_[0].value.as<FloatLitIndex>()).sign = FloatNode::Sign::Negative;
              yylhs.value.as<FloatLitIndex>() = yystack_[0].value.as<FloatLitIndex>();
          }
#line 4474 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 265: // accidental.unsigned: ACCIDENTAL_STEPS
#line 1137 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AccidentalIndex>() =
                  cxt.create(AccidentalNode { AccidentalNode::Kind::Steps }, yylhs.location);
          }
#line 4480 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 266: // accidental.unsigned: ACCIDENTAL_CENTS
#line 1139 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AccidentalIndex>() =
                  cxt.create(AccidentalNode { AccidentalNode::Kind::Cents }, yylhs.location);
          }
#line 4486 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 267: // accidental: accidental.unsigned
#line 1144 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<AccidentalIndex>() = yystack_[0].value.as<AccidentalIndex>();
          }
#line 4492 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 268: // accidental: MINUS accidental.unsigned
#line 1146 "langutils/sc_parser/src/sc_grammar.y"
          {
              cxt.graph.payload(yystack_[0].value.as<AccidentalIndex>()).sign = AccidentalNode::Sign::Negative;
              yylhs.value.as<AccidentalIndex>() = yystack_[0].value.as<AccidentalIndex>();
          }
#line 4498 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 269: // float: float.raw
#line 1150 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() = yystack_[0].value.as<FloatLitIndex>();
          }
#line 4504 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 270: // float: accidental
#line 1151 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() = yystack_[0].value.as<AccidentalIndex>();
          }
#line 4510 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 271: // float: float.raw PI
#line 1152 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() =
                  cxt.create(PiNode {}, yylhs.location, yystack_[1].value.as<FloatLitIndex>());
          }
#line 4516 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 272: // float: integer PI
#line 1153 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() =
                  cxt.create(PiNode {}, yylhs.location, yystack_[1].value.as<IntLitIndex>());
          }
#line 4522 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 273: // float: PI
#line 1154 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() =
                  cxt.create(PiNode {}, yylhs.location, cxt.create(Missing {}, yylhs.location));
          }
#line 4528 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 274: // float: MINUS PI
#line 1155 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<FloatProducingIndex>() =
                  cxt.create(PiNode { PiNode::Sign::Negative }, yylhs.location, cxt.create(Missing {}, yylhs.location));
          }
#line 4534 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 275: // accessor: %empty
#line 1159 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RWAccessorIndex>() = cxt.create(RWAccessor { ReadWriteAccessor::Private }, yylhs.location);
          }
#line 4540 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 276: // accessor: LESSTHAN
#line 1160 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RWAccessorIndex>() =
                  cxt.create(RWAccessor { ReadWriteAccessor::ExternallyReadable }, yylhs.location);
          }
#line 4546 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 277: // accessor: GREATERTHAN
#line 1161 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RWAccessorIndex>() =
                  cxt.create(RWAccessor { ReadWriteAccessor::ExternallyWriteable }, yylhs.location);
          }
#line 4552 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;

          case 278: // accessor: READWRITEVAR
#line 1163 "langutils/sc_parser/src/sc_grammar.y"
          {
              yylhs.value.as<RWAccessorIndex>() = cxt.create(
                  RWAccessor {
                      combine<ReadWriteAccessor::ExternallyReadable, ReadWriteAccessor::ExternallyWriteable>() },
                  yylhs.location);
          }
#line 4562 "langutils/sc_parser/src/sc_grammar_parser.cpp"
          break;


#line 4566 "langutils/sc_parser/src/sc_grammar_parser.cpp"

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
                                              "file_as_func",
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
                                              "expr.dotrange",
                                              "expr.dotrange.assign",
                                              "expr.base.range",
                                              "expr.base.range.assign",
                                              "range",
                                              "range.generator",
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
                                              "name.pack",
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


  const short parser::yypact_ninf_ = -206;

  const short parser::yytable_ninf_ = -280;

  const short parser::yypact_[] = {
      251,  33,   633,  2,    46,   -206, 296,  12,   -206, -206, -206, 214,  3,    -206, -206, 40,   1519, 706,  159,
      7,    -206, -206, -206, -206, -206, -206, -206, -206, -206, -206, -206, -206, -206, -206, 186,  -206, 166,  215,
      -206, -206, -206, -206, -206, -206, 2241, 32,   2009, -206, -17,  -206, -206, -206, -206, -206, -206, -206, -206,
      50,   644,  -206, 1321, 49,   -206, -206, -206, -206, 13,   155,  25,   -206, -206, -206, -206, 68,   59,   -206,
      62,   -206, -206, -206, 112,  -206, -206, 2058, 2058, 162,  -206, -206, -206, 182,  179,  209,  2009, 2009, 761,
      2009, 644,  223,  171,  270,  2009, 1568, -206, 2009, 2241, -206, -206, -206, -206, 32,   -206, -206, 235,  565,
      244,  969,  274,  -206, 276,  1519, 7,    -206, 237,  1519, 1018, -206, 41,   -206, 41,   226,  -206, 278,  26,
      28,   -206, 7,    227,  30,   -206, 219,  -206, -206, -206, 7,    -206, 254,  2103, -206, -206, 106,  816,  -206,
      -206, -206, 185,  239,  2009, 2009, 212,  -206, 644,  -206, 1321, 303,  1068, 2009, 41,   232,  243,  2009, 2009,
      -206, -206, -206, 384,  -206, -206, -206, -206, -206, 277,  301,  302,  -206, 2009, -206, 2009, 1617, 308,  299,
      2009, 129,  -206, 200,  2009, -206, 2009, 1666, -206, 212,  1321, 1715, 309,  270,  -206, 2009, 270,  41,   2009,
      2009, -206, -206, 307,  311,  -206, -206, -206, 41,   166,  2009, 2009, 2009, 2009, 141,  7,    7,    313,  2009,
      2009, 16,   -206, 7,    34,   -206, 1715, 322,  2177, -206, -206, 212,  272,  2009, 99,   866,  1118, 41,   35,
      199,  -206, 273,  644,  1371, 2009, 329,  212,  -206, 41,   326,  644,  7,    -206, 7,    -206, 328,  644,  499,
      327,  -206, -206, 384,  333,  -206, -206, 247,  247,  247,  -206, 331,  384,  1420, 1420, 644,  -206, 2009, 291,
      -206, -206, 334,  2009, 1764, 1168, 41,   -206, 305,  -206, 335,  338,  212,  2009, 565,  -206, -206, 342,  644,
      343,  -206, -206, -206, 1469, -206, -206, -206, 344,  644,  345,  644,  -206, 7,    44,   253,  -206, 346,  644,
      7,    -206, 19,   -206, 2140, 348,  1715, -206, 337,  2009, 351,  310,  1813, 312,  2009, 139,  41,   -206, 1218,
      2009, 41,   2009, -206, -206, 2009, -206, 644,  -206, 364,  41,   21,   -206, -206, -206, -206, -17,  -206, -206,
      -206, -206, -206, -206, 353,  166,  353,  353,  277,  -206, 362,  7,    123,  -206, 356,  -206, 15,   356,  -206,
      2009, -206, 332,  -206, 360,  41,   366,  1862, -206, -206, 367,  -206, -206, -206, -206, -206, 368,  2009, 2009,
      -206, 7,    -206, 320,  7,    -206, 1715, 2214, -206, -206, 370,  -206, 644,  339,  2009, 341,  373,  2009, 375,
      349,  1911, -206, -206, 377,  644,  -206, 382,  644,  -206, 2009, -206, 919,  247,  -206, -206, -206, 58,   2009,
      2009, 1420, 387,  2009, 350,  388,  -206, 1960, -206, 41,   41,   -206, 385,  -206, -206, 391,  644,  392,  -206,
      340,  395,  1715, -206, -206, 2009, 644,  2009, 352,  644,  357,  2009, 358,  398,  -206, -206, 644,  -206, 399,
      212,  -206, 2009, 2009, -206, -206, -206, -206, -206, 2009, -206, -206, 401,  -206, -206, -206, -206, -206, -206,
      404,  644,  644,  2009, 2009, 644,  2009, 372,  -206, 1271, 410,  408,  -206, -206, -206, -206, 644,  644,  644,
      2009, -206, 212,  -206, -206, 644,  414,  -206
  };

  const short parser::yydefact_[] = {
      6,   26,  0,   0,   0,   4,   0,   0,   19,  21,  22,  0,   24,  3,   15,  52,  220, 229, 0,   0,   238, 255, 256,
      257, 259, 260, 261, 262, 265, 266, 251, 252, 253, 247, 139, 122, 0,   0,   248, 249, 250, 273, 53,  239, 0,   0,
      0,   14,  192, 219, 123, 133, 134, 128, 132, 129, 130, 131, 10,  120, 0,   279, 235, 237, 236, 119, 121, 244, 0,
      218, 217, 216, 212, 213, 214, 263, 269, 267, 270, 215, 0,   1,   18,  0,   280, 0,   5,   20,  28,  0,   0,   0,
      0,   0,   229, 0,   150, 279, 221, 281, 0,   0,   72,  81,  76,  74,  75,  77,  78,  79,  73,  80,  0,   150, 0,
      0,   0,   230, 281, 220, 0,   240, 125, 220, 0,   58,  94,  59,  56,  0,   209, 211, 204, 0,   180, 0,   281, 164,
      274, 0,   258, 264, 268, 0,   186, 281, 171, 140, 193, 0,   0,   81,  76,  79,  0,   83,  0,   0,   279, 62,  67,
      68,  280, 0,   0,   0,   89,  0,   0,   0,   0,   254, 272, 271, 50,  17,  16,  2,   27,  25,  38,  0,   0,   223,
      280, 152, 0,   282, 0,   0,   0,   0,   228, 0,   0,   124, 0,   0,   12,  279, 0,   282, 0,   281, 241, 0,   281,
      56,  0,   0,   196, 197, 281, 0,   60,  61,  95,  57,  0,   0,   0,   0,   0,   0,   282, 0,   0,   0,   0,   0,
      282, 0,   0,   52,  229, 0,   0,   170, 55,  279, 127, 0,   0,   0,   0,   56,  201, 0,   82,  137, 66,  280, 0,
      0,   279, 9,   0,   0,   141, 0,   242, 0,   245, 0,   144, 77,  0,   46,  48,  51,  0,   41,  40,  275, 275, 275,
      36,  279, 50,  0,   0,   151, 222, 0,   224, 233, 113, 0,   0,   0,   0,   86,  227, 0,   109, 0,   0,   279, 0,
      0,   231, 232, 0,   142, 0,   96,  194, 195, 282, 199, 200, 210, 0,   205, 0,   207, 184, 0,   167, 0,   181, 0,
      165, 0,   190, 0,   187, 176, 0,   229, 172, 0,   0,   0,   126, 0,   136, 0,   0,   56,  91,  0,   0,   56,  0,
      69,  70,  0,   63,  64,  7,   0,   88,  56,  243, 246, 145, 47,  192, 49,  29,  276, 277, 278, 31,  34,  0,   33,
      35,  280, 39,  0,   0,   0,   161, 281, 155, 121, 281, 226, 0,   117, 0,   118, 0,   0,   0,   0,   110, 11,  0,
      234, 93,  198, 206, 208, 0,   0,   0,   182, 0,   166, 0,   0,   188, 229, 0,   175, 173, 0,   54,  147, 105, 0,
      104, 0,   0,   0,   135, 0,   92,  202, 0,   138, 90,  0,   65,  8,   0,   87,  0,   275, 30,  37,  23,  0,   0,
      0,   282, 0,   0,   0,   0,   225, 0,   114, 85,  56,  112, 0,   13,  185, 0,   168, 0,   191, 0,   0,   229, 177,
      174, 0,   146, 0,   103, 149, 98,  0,   99,  0,   203, 71,  143, 43,  279, 279, 32,  0,   0,   163, 162, 156, 154,
      157, 0,   153, 116, 0,   84,  111, 169, 183, 189, 178, 0,   108, 107, 0,   0,   148, 0,   97,  280, 0,   0,   0,
      159, 158, 115, 179, 106, 102, 101, 0,   45,  279, 42,  160, 100, 0,   44
  };

  const short parser::yypgoto_[] = { -206, -206, -206, -57,  -206, -206, 415,  -206, -206, -206, -206, -8,   0,    60,
                                     -206, -206, -206, 168,  174,  -206, 167,  -206, 31,   -204, -29,  -114, 195,  -206,
                                     -160, -9,   -206, -206, -206, -206, -206, -206, -206, -206, -206, -2,   -206, 83,
                                     259,  169,  9,    -206, -206, 170,  90,   143,  -206, -140, 300,  -206, -205, -206,
                                     266,  -206, -15,  252,  -206, -206, -206, -142, 315,  -206, -30,  -206, -206, -206,
                                     -206, -206, -37,  -35,  -206, -33,  -206, -206, -206, -43,  -116 };

  const short parser::yydefgoto_[] = { 0,   4,   5,   47,  6,   7,   8,   9,   91,  12,  10,  364, 365, 276,
                                       277, 278, 266, 267, 268, 269, 270, 48,  49,  216, 217, 158, 159, 248,
                                       111, 155, 156, 50,  51,  52,  53,  54,  55,  56,  57,  96,  97,  210,
                                       59,  375, 376, 136, 145, 148, 149, 211, 212, 213, 128, 343, 130, 131,
                                       161, 62,  99,  117, 118, 63,  64,  65,  66,  67,  68,  69,  70,  71,
                                       72,  73,  74,  75,  76,  77,  78,  79,  366, 85,  188 };

  const short parser::yytable_[] = {
      58,  199, 202, 305, 237, 126, 133, 140, 112, 141, 242,  142, 86,  311, 271, 113, 37,  15,  163, 15,  226, 164,
      20,  164, 257, 15,  175, 176, 323, 232, 20,  403, 80,   169, 219, 239, 221, 166, 227, 11,  45,  340, 1,   341,
      147, 15,  81,  20,  254, 20,  92,  90,  397, 43,  185,  42,  150, 42,  160, 162, 440, 43,  165, 42,  165, 125,
      477, 140, 3,   141, 428, 142, 143, 324, 170, 220, 404,  222, 43,  228, 43,  58,  58,  42,  342, 112, 297, 302,
      93,  144, 304, 326, 113, 398, 330, 171, 309, 125, 172,  98,  114, 173, 140, 338, 203, 271, 334, 478, 206, 271,
      15,  238, 16,  160, 94,  253, 174, 157, 271, 18,  19,   20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,
      31,  32,  33,  420, 34,  35,  36,  424, 335, 288, 38,   39,  40,  41,  418, 160, 42,  429, 386, 316, 43,  317,
      249, 250, 296, 214, 44,  125, 160, 432, 177, 258, 291,  119, 46,  167, 264, 436, 289, 437, 60,  120, 20,  181,
      182, 114, 183, 129, 419, 20,  281, 189, 191, 407, 192,  115, 133, 178, 15,  243, 123, 244, 124, 168, 331, 214,
      160, 299, 20,  422, 98,  303, 15,  43,  98,  344, 290,  179, 346, 351, 43,  180, 20,  21,  22,  23,  313, 186,
      315, 88,  251, 252, 125, 134, 322, 352, 42,  20,  20,   43,  113, 184, 370, 21,  22,  23,  125, 224, 120, 20,
      42,  488, 193, 43,  259, 20,  214, 160, 349, 139, 263,  195, 390, 135, 196, 261, 20,  439, 43,  43,  442, 399,
      459, 400, 230, 225, 61,  282, 284, 139, 43,  287, 367,  368, 125, 292, 43,  293, 295, 1,   187, 116, 200, 197,
      205, 125, 201, 43,  218, 306, 307, 127, 231, 2,   -279, 82,  247, 83,  361, 362, 312, 255, 314, 363, 84,  3,
      286, 273, 321, 274, 275, 279, 280, 285, 475, 328, 301,  308, 310, 332, 214, 320, 333, 127, 89,  113, 119, 350,
      411, 358, 154, 121, 122, 353, 133, 356, 360, 380, 423,  369, 410, 381, 388, 426, 387, 389, 166, 391, 392, 132,
      137, 394, 395, 401, 446, 408, 412, 413, 146, 416, 374,  374, 427, 431, 379, 434, 438, 445, 125, 382, 384, 444,
      125, 447, 450, 455, 451, 460, 464, 192, 466, 214, 125,  215, 470, 127, 461, 515, 463, 471, 482, 485, 489, 484,
      453, 492, 467, 20,  490, 497, 491, 113, 493, 501, 498,  500, 125, 502, 508, 462, 409, 509, 465, 516, 125, 517,
      415, 520, 417, 513, 87,  476, 146, 215, 472, 425, 160,  433, 43,  503, 504, 357, 102, 204, 152, 105, 106, 265,
      108, 153, 110, 359, 121, 371, 348, 481, 430, 378, 223,  393, 127, 300, 245, 0,   113, 0,   229, 495, 0,   496,
      0,   443, 0,   499, 127, 0,   0,   246, 449, 0,   519,  0,   0,   0,   215, 214, 125, 0,   452, 0,   260, 262,
      0,   0,   0,   0,   457, 272, 0,   0,   0,   0,   0,    510, 511, 0,   512, 0,   0,   160, 469, 0,   127, 0,
      0,   0,   0,   0,   0,   518, 0,   0,   20,  127, 0,    0,   0,   479, 480, 374, 0,   483, 0,   0,   0,   487,
      0,   0,   0,   0,   0,   132, 0,   0,   0,   0,   0,    318, 319, 494, 0,   0,   0,   43,  325, 327, 0,   102,
      215, 152, 105, 106, 107, 108, 153, 110, 0,   0,   505,  506, 345, 0,   0,   0,   0,   507, 0,   0,   0,   0,
      0,   0,   354, 0,   355, 0,   0,   0,   272, 0,   0,    0,   272, 0,   0,   0,   0,   0,   0,   0,   0,   272,
      377, 377, 0,   0,   127, 0,   0,   0,   127, 0,   0,    0,   0,   0,   0,   0,   0,   215, 127, 194, 0,   102,
      151, 152, 105, 106, 107, 108, 153, 110, 154, 0,   0,    0,   0,   0,   0,   0,   396, 13,  14,  0,   127, 15,
      402, 16,  0,   17,  0,   0,   127, 0,   18,  19,  20,   21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
      32,  33,  0,   34,  35,  36,  37,  0,   0,   38,  39,   40,  41,  0,   0,   42,  0,   0,   0,   43,  0,   132,
      0,   0,   0,   44,  0,   435, 0,   0,   45,  0,   441,  46,  102, 151, 152, 105, 106, 107, 108, 153, 110, 154,
      0,   215, 127, 0,   0,   0,   15,  0,   16,  0,   94,   454, 0,   0,   456, 18,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  29,  30,  31,  32,  33,  0,   34,   35,  36,  37,  0,   0,   38,  39,  40,  41,  0,   100,
      42,  0,   0,   0,   43,  377, 101, 0,   102, 103, 104,  105, 106, 107, 108, 109, 110, 15,  46,  16,  0,   94,
      0,   0,   0,   0,   18,  19,  20,  21,  22,  23,  24,   25,  26,  27,  28,  29,  30,  31,  32,  33,  0,   34,
      35,  0,   0,   0,   0,   38,  39,  40,  41,  0,   100,  42,  0,   0,   0,   43,  0,   101, 0,   102, 103, 104,
      105, 106, 107, 108, 153, 110, 15,  46,  16,  240, 94,   0,   0,   0,   0,   18,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  29,  30,  31,  32,  33,  0,   34,   35,  0,   0,   0,   0,   38,  39,  40,  41,  0,   241,
      42,  0,   0,   0,   43,  0,   0,   0,   0,   208, 44,   0,   15,  209, 16,  336, 94,  0,   46,  0,   0,   18,
      19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,   30,  31,  32,  33,  0,   34,  35,  0,   0,   0,   0,
      38,  39,  40,  41,  0,   337, 42,  0,   0,   0,   43,   0,   0,   0,   0,   208, 44,  0,   0,   209, 0,   15,
      473, 16,  46,  94,  0,   0,   157, 0,   18,  19,  20,   21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
      32,  33,  474, 34,  35,  36,  0,   0,   0,   38,  39,   40,  41,  0,   0,   42,  0,   0,   0,   43,  0,   0,
      0,   0,   0,   44,  0,   15,  0,   16,  0,   94,  198,  46,  157, 0,   18,  19,  20,  21,  22,  23,  24,  25,
      26,  27,  28,  29,  30,  31,  32,  33,  0,   34,  35,   36,  0,   0,   0,   38,  39,  40,  41,  0,   0,   42,
      0,   0,   0,   43,  0,   0,   0,   0,   0,   44,  15,   0,   16,  0,   94,  207, 0,   46,  0,   18,  19,  20,
      21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,   32,  33,  0,   34,  35,  0,   0,   0,   0,   38,  39,
      40,  41,  0,   0,   42,  0,   0,   0,   43,  0,   0,    0,   0,   208, 44,  0,   15,  209, 16,  0,   94,  256,
      46,  0,   0,   18,  19,  20,  21,  22,  23,  24,  25,   26,  27,  28,  29,  30,  31,  32,  33,  0,   34,  35,
      0,   0,   0,   0,   38,  39,  40,  41,  0,   0,   42,   0,   0,   0,   43,  0,   0,   0,   0,   208, 44,  0,
      15,  209, 16,  0,   94,  339, 46,  0,   0,   18,  19,   20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,
      31,  32,  33,  0,   34,  35,  0,   0,   0,   0,   38,   39,  40,  41,  0,   0,   42,  0,   0,   0,   43,  0,
      0,   0,   0,   208, 44,  0,   15,  209, 16,  0,   94,   385, 46,  0,   0,   18,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  29,  30,  31,  32,  33,  0,   34,   35,  0,   0,   0,   0,   38,  39,  40,  41,  0,   0,
      42,  0,   0,   0,   43,  0,   0,   0,   0,   208, 44,   0,   15,  209, 16,  0,   94,  421, 46,  0,   0,   18,
      19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,   30,  31,  32,  33,  0,   34,  35,  0,   0,   0,   0,
      38,  39,  40,  41,  0,   0,   42,  0,   0,   0,   43,   0,   0,   0,   0,   208, 44,  0,   0,   209, 0,   15,
      514, 16,  46,  94,  0,   0,   157, 0,   18,  19,  20,   21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
      32,  33,  0,   34,  35,  36,  0,   0,   0,   38,  39,   40,  41,  0,   0,   42,  0,   0,   0,   43,  0,   0,
      0,   0,   0,   44,  0,   15,  0,   16,  0,   94,  0,    46,  157, 0,   18,  19,  20,  21,  22,  23,  24,  25,
      26,  27,  28,  29,  30,  31,  32,  33,  0,   34,  35,   36,  0,   0,   0,   38,  39,  40,  41,  0,   0,   42,
      0,   0,   0,   43,  0,   0,   0,   0,   0,   44,  0,    15,  0,   16,  0,   94,  0,   46,  347, 0,   18,  19,
      20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,   31,  32,  33,  0,   34,  35,  36,  0,   0,   0,   38,
      39,  40,  41,  0,   0,   42,  0,   0,   0,   43,  0,    0,   0,   0,   0,   44,  15,  0,   16,  0,   94,  0,
      0,   46,  0,   18,  19,  20,  21,  22,  23,  24,  25,   26,  27,  28,  29,  30,  31,  32,  33,  0,   34,  35,
      372, 0,   0,   0,   38,  39,  40,  41,  0,   0,   42,   0,   0,   0,   43,  0,   373, 0,   0,   0,   44,  15,
      0,   16,  0,   94,  0,   0,   46,  0,   18,  19,  20,   21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
      32,  33,  0,   34,  35,  0,   0,   0,   0,   38,  39,   40,  41,  0,   0,   42,  0,   0,   0,   43,  0,   0,
      0,   0,   208, 44,  0,   15,  209, 16,  0,   94,  0,    46,  0,   0,   18,  19,  20,  21,  22,  23,  24,  25,
      26,  27,  28,  29,  30,  31,  32,  33,  0,   34,  35,   0,   0,   0,   0,   38,  39,  40,  41,  0,   0,   42,
      0,   0,   0,   43,  0,   0,   0,   0,   95,  44,  15,   0,   16,  0,   94,  0,   0,   46,  0,   18,  19,  20,
      21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,   32,  33,  0,   34,  35,  0,   0,   0,   0,   38,  39,
      40,  41,  0,   190, 42,  0,   0,   0,   43,  0,   0,    0,   0,   0,   44,  15,  0,   16,  0,   94,  0,   0,
      46,  0,   18,  19,  20,  21,  22,  23,  24,  25,  26,   27,  28,  29,  30,  31,  32,  33,  0,   34,  35,  0,
      0,   0,   0,   38,  39,  40,  41,  0,   0,   42,  0,    0,   0,   43,  0,   0,   0,   0,   283, 44,  15,  0,
      16,  0,   94,  294, 0,   46,  0,   18,  19,  20,  21,   22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,
      33,  0,   34,  35,  0,   0,   0,   0,   38,  39,  40,   41,  0,   0,   42,  0,   0,   0,   43,  0,   0,   0,
      0,   0,   44,  15,  0,   16,  0,   94,  0,   0,   46,   0,   18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
      28,  29,  30,  31,  32,  33,  0,   34,  35,  0,   0,    0,   0,   38,  39,  40,  41,  0,   0,   42,  0,   0,
      0,   43,  0,   0,   0,   0,   298, 44,  15,  0,   16,   0,   94,  383, 0,   46,  0,   18,  19,  20,  21,  22,
      23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,   0,   34,  35,  0,   0,   0,   0,   38,  39,  40,  41,
      0,   0,   42,  0,   0,   0,   43,  0,   0,   0,   0,    0,   44,  15,  0,   16,  414, 94,  0,   0,   46,  0,
      18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,   29,  30,  31,  32,  33,  0,   34,  35,  0,   0,   0,
      0,   38,  39,  40,  41,  0,   0,   42,  0,   0,   0,    43,  0,   0,   0,   0,   0,   44,  15,  0,   16,  0,
      94,  448, 0,   46,  0,   18,  19,  20,  21,  22,  23,   24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  0,
      34,  35,  0,   0,   0,   0,   38,  39,  40,  41,  0,    0,   42,  0,   0,   0,   43,  0,   0,   0,   0,   0,
      44,  15,  0,   16,  468, 94,  0,   0,   46,  0,   18,   19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
      30,  31,  32,  33,  0,   34,  35,  0,   0,   0,   0,    38,  39,  40,  41,  0,   0,   42,  0,   0,   0,   43,
      0,   0,   0,   0,   0,   44,  15,  0,   16,  0,   94,   486, 0,   46,  0,   18,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  29,  30,  31,  32,  33,  0,   34,   35,  0,   0,   0,   0,   38,  39,  40,  41,  0,   0,
      42,  0,   0,   0,   43,  0,   0,   0,   0,   0,   44,   15,  0,   16,  0,   94,  0,   0,   46,  0,   18,  19,
      20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,   31,  32,  33,  0,   34,  35,  0,   0,   0,   0,   38,
      39,  40,  41,  0,   0,   42,  0,   0,   0,   43,  0,    0,   0,   0,   0,   44,  15,  0,   16,  0,   17,  0,
      0,   46,  0,   18,  19,  20,  21,  22,  23,  24,  25,   26,  27,  28,  29,  30,  31,  32,  33,  0,   34,  35,
      0,   0,   0,   0,   38,  39,  40,  41,  0,   0,   42,   0,   0,   0,   43,  0,   0,   233, 0,   16,  44,  234,
      0,   0,   0,   0,   235, 0,   46,  21,  22,  23,  24,   25,  26,  27,  28,  29,  30,  31,  32,  33,  0,   0,
      0,   0,   0,   0,   0,   38,  39,  40,  41,  0,   233,  42,  16,  0,   405, 0,   0,   0,   236, 235, 0,   44,
      21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,   32,  33,  0,   0,   0,   0,   0,   0,   0,   38,  39,
      40,  41,  0,   233, 42,  16,  0,   329, 0,   0,   0,    406, 235, 0,   44,  21,  22,  23,  24,  25,  26,  27,
      28,  29,  30,  31,  32,  33,  0,   0,   0,   0,   0,    0,   0,   38,  39,  40,  41,  0,   233, 42,  16,  0,
      458, 0,   0,   0,   0,   235, 0,   44,  21,  22,  23,   24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  0,
      0,   0,   0,   0,   0,   0,   38,  39,  40,  41,  0,    0,   42,  21,  22,  23,  24,  25,  26,  27,  28,  29,
      44,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   138, 0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   139
  };

  const short parser::yycheck_[] = {
      2,   115, 118, 207, 146, 34,  36,  44,  17,  44,  150, 44,  0,   218, 174, 17,  33,  4,   61,  4,   136, 8,   15,
      8,   164, 4,   83,  84,  12,  145, 15,  12,  30,  8,   8,   149, 8,   66,  8,   6,   57,  245, 30,  8,   46,  4,
      0,   15,  162, 15,  10,  48,  8,   46,  97,  42,  6,   42,  60,  10,  45,  46,  49,  42,  49,  34,  8,   104, 56,
      104, 49,  104, 40,  57,  49,  49,  57,  49,  46,  49,  46,  83,  84,  42,  49,  94,  200, 203, 48,  57,  206, 57,
      94,  49,  236, 27,  212, 66,  39,  16,  17,  39,  139, 243, 119, 265, 7,   49,  123, 269, 4,   5,   6,   115, 8,
      158, 4,   11,  278, 13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  339, 30,  31,
      32,  343, 41,  12,  36,  37,  38,  39,  7,   149, 42,  353, 290, 10,  46,  12,  156, 157, 199, 126, 52,  128, 162,
      366, 0,   165, 193, 6,   60,  12,  170, 46,  41,  48,  2,   14,  15,  92,  93,  94,  95,  13,  41,  15,  184, 100,
      101, 327, 103, 17,  218, 7,   4,   6,   6,   8,   8,   40,  239, 166, 200, 201, 15,  341, 119, 205, 4,   46,  123,
      8,   8,   30,  247, 254, 46,  4,   15,  16,  17,  18,  220, 48,  222, 7,   10,  11,  193, 10,  228, 256, 42,  15,
      15,  46,  234, 10,  277, 16,  17,  18,  207, 12,  14,  15,  42,  447, 9,   46,  14,  15,  217, 251, 252, 52,  169,
      9,   297, 40,  12,  14,  15,  375, 46,  46,  378, 10,  406, 12,  12,  40,  2,   186, 187, 52,  46,  190, 274, 275,
      245, 194, 46,  196, 197, 30,  12,  17,  10,  41,  49,  256, 12,  46,  12,  208, 209, 34,  40,  44,  0,   1,   59,
      3,   53,  54,  219, 0,   221, 58,  10,  56,  9,   32,  227, 34,  35,  12,  12,  7,   430, 234, 9,   12,  9,   49,
      291, 10,  241, 66,  11,  329, 6,   0,   332, 4,   59,  18,  19,  9,   366, 9,   5,   48,  342, 10,  5,   9,   9,
      347, 41,  9,   377, 7,   7,   36,  37,  9,   9,   9,   385, 9,   7,   49,  45,  49,  279, 280, 0,   12,  283, 5,
      12,  9,   339, 288, 289, 41,  343, 9,   9,   57,  10,  9,   7,   298, 7,   352, 353, 126, 9,   128, 49,  503, 49,
      9,   5,   5,   9,   45,  398, 57,  49,  15,  9,   49,  10,  405, 9,   7,   49,  49,  377, 10,  9,   413, 329, 9,
      416, 5,   385, 9,   335, 5,   337, 49,  7,   431, 109, 166, 428, 344, 430, 369, 46,  474, 475, 265, 50,  120, 52,
      53,  54,  55,  56,  57,  58,  269, 129, 278, 251, 438, 358, 280, 135, 308, 193, 201, 154, -1,  458, -1,  143, 461,
      -1,  463, -1,  380, -1,  467, 207, -1,  -1,  154, 387, -1,  515, -1,  -1,  -1,  217, 446, 447, -1,  397, -1,  167,
      168, -1,  -1,  -1,  -1,  405, 174, -1,  -1,  -1,  -1,  -1,  497, 498, -1,  500, -1,  -1,  503, 419, -1,  245, -1,
      -1,  -1,  -1,  -1,  -1,  513, -1,  -1,  15,  256, -1,  -1,  -1,  436, 437, 438, -1,  440, -1,  -1,  -1,  444, -1,
      -1,  -1,  -1,  -1,  218, -1,  -1,  -1,  -1,  -1,  224, 225, 458, -1,  -1,  -1,  46,  231, 232, -1,  50,  291, 52,
      53,  54,  55,  56,  57,  58,  -1,  -1,  477, 478, 247, -1,  -1,  -1,  -1,  484, -1,  -1,  -1,  -1,  -1,  -1,  259,
      -1,  261, -1,  -1,  -1,  265, -1,  -1,  -1,  269, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  278, 279, 280, -1,  -1,
      339, -1,  -1,  -1,  343, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  352, 353, 48,  -1,  50,  51,  52,  53,  54,  55,
      56,  57,  58,  59,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  317, 0,   1,   -1,  377, 4,   323, 6,   -1,  8,   -1,  -1,
      385, -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  32,  33,
      -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  366, -1,  -1,  -1,  52,  -1,  372, -1,  -1,
      57,  -1,  377, 60,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  -1,  446, 447, -1,  -1,  -1,  4,   -1,  6,
      -1,  8,   400, -1,  -1,  403, 13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,
      30,  31,  32,  33,  -1,  -1,  36,  37,  38,  39,  -1,  41,  42,  -1,  -1,  -1,  46,  438, 48,  -1,  50,  51,  52,
      53,  54,  55,  56,  57,  58,  4,   60,  6,   -1,  8,   -1,  -1,  -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,
      21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  41,  42,  -1,
      -1,  -1,  46,  -1,  48,  -1,  50,  51,  52,  53,  54,  55,  56,  57,  58,  4,   60,  6,   7,   8,   -1,  -1,  -1,
      -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,
      -1,  36,  37,  38,  39,  -1,  41,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  51,  52,  -1,  4,   55,  6,   7,
      8,   -1,  60,  -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,
      31,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  41,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  51,  52,  -1,
      -1,  55,  -1,  4,   5,   6,   60,  8,   -1,  -1,  11,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,
      24,  25,  26,  27,  28,  29,  30,  31,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,
      -1,  -1,  -1,  -1,  -1,  52,  -1,  4,   -1,  6,   -1,  8,   9,   60,  11,  -1,  13,  14,  15,  16,  17,  18,  19,
      20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,
      -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  -1,  52,  4,   -1,  6,   -1,  8,   9,   -1,  60,  -1,  13,  14,  15,  16,
      17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,  -1,  36,  37,  38,  39,
      -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  51,  52,  -1,  4,   55,  6,   -1,  8,   9,   60,  -1,  -1,
      13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,  -1,
      36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  51,  52,  -1,  4,   55,  6,   -1,  8,
      9,   60,  -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,
      -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  51,  52,  -1,  4,
      55,  6,   -1,  8,   9,   60,  -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
      28,  -1,  30,  31,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,
      51,  52,  -1,  4,   55,  6,   -1,  8,   9,   60,  -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,
      24,  25,  26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,
      -1,  -1,  -1,  -1,  51,  52,  -1,  -1,  55,  -1,  4,   5,   6,   60,  8,   -1,  -1,  11,  -1,  13,  14,  15,  16,
      17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  32,  -1,  -1,  -1,  36,  37,  38,  39,
      -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  -1,  52,  -1,  4,   -1,  6,   -1,  8,   -1,  60,  11,  -1,
      13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  32,  -1,  -1,  -1,
      36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  -1,  52,  -1,  4,   -1,  6,   -1,  8,
      -1,  60,  11,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,
      32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  -1,  52,  4,   -1,
      6,   -1,  8,   -1,  -1,  60,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,
      -1,  30,  31,  32,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  48,  -1,  -1,  -1,
      52,  4,   -1,  6,   -1,  8,   -1,  -1,  60,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,
      26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,
      -1,  -1,  51,  52,  -1,  4,   55,  6,   -1,  8,   -1,  60,  -1,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,
      22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,
      -1,  46,  -1,  -1,  -1,  -1,  51,  52,  4,   -1,  6,   -1,  8,   -1,  -1,  60,  -1,  13,  14,  15,  16,  17,  18,
      19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  41,
      42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  -1,  52,  4,   -1,  6,   -1,  8,   -1,  -1,  60,  -1,  13,  14,  15,
      16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,  -1,  36,  37,  38,
      39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  51,  52,  4,   -1,  6,   -1,  8,   9,   -1,  60,  -1,
      13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,  -1,
      36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  -1,  52,  4,   -1,  6,   -1,  8,   -1,
      -1,  60,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  -1,
      -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  51,  52,  4,   -1,  6,
      -1,  8,   9,   -1,  60,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,
      30,  31,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  -1,  52,
      4,   -1,  6,   7,   8,   -1,  -1,  60,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,
      27,  28,  -1,  30,  31,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,
      -1,  -1,  52,  4,   -1,  6,   -1,  8,   9,   -1,  60,  -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,
      24,  25,  26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,
      -1,  -1,  -1,  -1,  -1,  52,  4,   -1,  6,   7,   8,   -1,  -1,  60,  -1,  13,  14,  15,  16,  17,  18,  19,  20,
      21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,
      -1,  -1,  46,  -1,  -1,  -1,  -1,  -1,  52,  4,   -1,  6,   -1,  8,   9,   -1,  60,  -1,  13,  14,  15,  16,  17,
      18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,
      -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  -1,  52,  4,   -1,  6,   -1,  8,   -1,  -1,  60,  -1,  13,  14,
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,  -1,  36,  37,
      38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  -1,  -1,  -1,  52,  4,   -1,  6,   -1,  8,   -1,  -1,  60,
      -1,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  30,  31,  -1,  -1,  -1,
      -1,  36,  37,  38,  39,  -1,  -1,  42,  -1,  -1,  -1,  46,  -1,  -1,  4,   -1,  6,   52,  8,   -1,  -1,  -1,  -1,
      13,  -1,  60,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      36,  37,  38,  39,  -1,  4,   42,  6,   -1,  8,   -1,  -1,  -1,  49,  13,  -1,  52,  16,  17,  18,  19,  20,  21,
      22,  23,  24,  25,  26,  27,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  4,   42,  6,   -1,
      8,   -1,  -1,  -1,  49,  13,  -1,  52,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,  -1,  4,   42,  6,   -1,  8,   -1,  -1,  -1,  -1,  13,  -1,  52,  16,
      17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  36,  37,  38,  39,
      -1,  -1,  42,  16,  17,  18,  19,  20,  21,  22,  23,  24,  52,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  39,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  52
  };

  const unsigned char parser::yystos_[] = {
      0,   30,  44,  56,  63,  64,  66,  67,  68,  69,  72,  6,   71,  0,   1,   4,   6,   8,   13,  14,  15,  16,  17,
      18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  30,  31,  32,  33,  36,  37,  38,  39,  42,  46,  52,  57,
      60,  65,  83,  84,  93,  94,  95,  96,  97,  98,  99,  100, 101, 104, 109, 118, 119, 123, 124, 125, 126, 127, 128,
      129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 30,  0,   1,   3,   10,  141, 0,   68,  7,   126, 48,  70,
      10,  48,  8,   51,  101, 102, 103, 120, 41,  48,  50,  51,  52,  53,  54,  55,  56,  57,  58,  90,  91,  101, 103,
      109, 118, 121, 122, 6,   14,  126, 126, 6,   8,   84,  86,  104, 114, 13,  116, 117, 126, 128, 10,  40,  107, 126,
      39,  52,  134, 135, 137, 40,  57,  108, 126, 101, 109, 110, 6,   51,  52,  57,  59,  91,  92,  11,  87,  88,  101,
      118, 10,  141, 8,   49,  86,  12,  40,  8,   49,  27,  39,  39,  4,   65,  65,  0,   7,   30,  4,   103, 103, 103,
      10,  141, 48,  12,  142, 103, 41,  103, 103, 9,   48,  9,   12,  41,  9,   87,  10,  12,  142, 120, 126, 49,  120,
      9,   51,  55,  103, 111, 112, 113, 84,  104, 85,  86,  12,  8,   49,  8,   49,  126, 12,  40,  142, 8,   49,  126,
      12,  40,  142, 4,   8,   13,  49,  125, 5,   87,  7,   41,  113, 6,   8,   114, 126, 59,  89,  101, 101, 10,  11,
      141, 87,  0,   9,   113, 101, 14,  126, 14,  126, 103, 101, 55,  78,  79,  80,  81,  82,  90,  126, 32,  34,  35,
      75,  76,  77,  12,  12,  101, 103, 51,  103, 7,   9,   103, 12,  41,  8,   86,  103, 103, 9,   103, 141, 87,  51,
      101, 121, 9,   142, 101, 142, 85,  103, 103, 12,  142, 9,   116, 103, 101, 103, 101, 10,  12,  126, 126, 10,  103,
      101, 12,  57,  126, 57,  126, 103, 8,   125, 141, 49,  103, 7,   41,  7,   41,  113, 9,   85,  8,   49,  115, 8,
      126, 134, 11,  88,  101, 0,   141, 86,  9,   126, 126, 9,   79,  4,   80,  5,   53,  54,  58,  73,  74,  140, 74,
      74,  10,  141, 82,  32,  48,  103, 105, 106, 126, 105, 103, 48,  9,   103, 9,   103, 9,   113, 41,  9,   9,   141,
      7,   7,   111, 9,   9,   126, 8,   49,  10,  12,  9,   126, 12,  57,  8,   49,  125, 9,   103, 5,   101, 7,   49,
      7,   103, 49,  103, 7,   41,  85,  9,   113, 101, 85,  103, 101, 0,   49,  85,  110, 12,  116, 75,  5,   126, 46,
      48,  12,  142, 45,  126, 142, 103, 41,  9,   86,  9,   9,   103, 9,   10,  103, 101, 126, 57,  126, 103, 8,   125,
      9,   49,  101, 49,  7,   101, 7,   49,  7,   103, 9,   9,   101, 5,   29,  87,  73,  8,   49,  103, 103, 106, 5,
      103, 45,  5,   9,   103, 85,  9,   9,   10,  57,  9,   103, 101, 101, 49,  49,  101, 49,  7,   10,  141, 141, 103,
      103, 103, 9,   9,   101, 101, 101, 49,  5,   87,  5,   9,   101, 141, 5
  };

  const unsigned char parser::yyr1_[] = {
      0,   62,  63,  63,  63,  63,  63,  64,  64,  64,  65,  65,  65,  65,  66,  66,  66,  66,  66,  67,  67,  68,
      68,  69,  70,  70,  71,  71,  71,  72,  73,  74,  74,  75,  75,  75,  76,  76,  77,  77,  78,  78,  79,  79,
      79,  79,  80,  80,  81,  81,  82,  82,  83,  83,  84,  84,  85,  85,  86,  86,  86,  86,  87,  87,  87,  87,
      87,  88,  88,  89,  89,  89,  90,  90,  90,  90,  90,  90,  90,  90,  91,  91,  92,  92,  93,  93,  93,  93,
      93,  93,  93,  93,  93,  93,  93,  93,  93,  94,  94,  94,  95,  95,  95,  96,  96,  96,  97,  97,  97,  98,
      98,  98,  98,  98,  99,  99,  99,  99,  99,  100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 101,
      101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 102, 102, 103, 104,
      104, 105, 105, 106, 106, 106, 106, 106, 106, 106, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108,
      108, 108, 108, 108, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 109, 110, 110, 111, 111, 111, 112,
      112, 113, 114, 115, 115, 115, 116, 116, 116, 116, 116, 117, 117, 118, 119, 119, 119, 119, 119, 119, 119, 119,
      120, 120, 120, 120, 120, 120, 120, 121, 121, 122, 122, 122, 123, 124, 124, 125, 125, 125, 126, 126, 127, 127,
      127, 127, 128, 128, 128, 129, 130, 131, 131, 132, 132, 133, 133, 134, 134, 134, 134, 135, 135, 135, 135, 136,
      136, 137, 137, 138, 138, 139, 139, 139, 139, 139, 139, 140, 140, 140, 140, 141, 141, 142, 142
  };

  const signed char parser::yyr2_[] = {
      0, 2, 3, 2, 1, 2, 0, 5, 6, 4, 1, 5, 3, 6, 2, 2, 3, 3, 2, 1, 2, 1, 1, 7, 0, 2, 0, 3, 2, 5, 2, 1, 3, 2, 2, 2,
      1, 3, 0, 2, 1, 1, 6, 4, 8, 6, 1, 2, 1, 2, 0, 1, 1, 1, 5, 3, 0, 1, 1, 1, 2, 2, 1, 3, 3, 4, 2, 1, 1, 2, 2, 4,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 7, 6, 4, 5, 4, 2, 5, 4, 5, 5, 2, 3, 4, 7, 6, 6, 9, 8, 8, 6, 5, 5, 8, 7,
      7, 4, 5, 7, 6, 4, 6, 8, 7, 5, 5, 1, 1, 1, 1, 1, 3, 2, 4, 3, 1, 1, 1, 1, 1, 1, 1, 5, 4, 3, 5, 1, 2, 3, 4, 6,
      3, 4, 6, 5, 7, 6, 1, 3, 2, 7, 7, 1, 3, 3, 4, 4, 5, 1, 3, 3, 1, 3, 4, 3, 5, 6, 2, 1, 3, 4, 5, 4, 3, 5, 6, 7,
      2, 4, 5, 7, 4, 6, 2, 4, 5, 7, 4, 6, 0, 1, 2, 2, 1, 1, 3, 2, 3, 0, 2, 3, 1, 3, 4, 3, 4, 1, 3, 2, 1, 1, 1, 1,
      1, 1, 1, 1, 0, 1, 3, 2, 3, 5, 4, 3, 2, 0, 1, 3, 4, 4, 5, 1, 1, 1, 1, 1, 2, 3, 3, 4, 1, 3, 4, 1, 1, 1, 1, 1,
      1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 2, 1, 2, 0, 1, 1, 1, 0, 1, 0, 1
  };


#if YYDEBUG
  const short parser::yyrline_[] = {
      0,    188,  188,  189,  190,  191,  192,  200,  203,  206,  215,  217,  220,  223,  228,  231,  237,  240,  243,
      275,  277,  282,  283,  287,  292,  293,  297,  298,  299,  303,  312,  317,  319,  324,  326,  328,  333,  335,
      340,  341,  349,  350,  354,  357,  360,  363,  368,  369,  374,  375,  379,  380,  388,  388,  391,  393,  398,
      399,  403,  404,  405,  406,  410,  411,  412,  417,  422,  430,  431,  440,  441,  442,  446,  447,  448,  449,
      450,  451,  452,  453,  457,  458,  462,  464,  469,  472,  475,  479,  482,  485,  488,  491,  494,  504,  507,
      512,  518,  533,  540,  547,  556,  563,  570,  580,  586,  592,  601,  607,  613,  622,  627,  632,  637,  642,
      650,  655,  661,  666,  671,  684,  687,  690,  693,  696,  699,  706,  709,  712,  715,  718,  721,  727,  728,
      729,  730,  732,  735,  738,  741,  744,  745,  747,  749,  752,  755,  758,  764,  767,  770,  773,  781,  782,
      786,  794,  797,  802,  804,  809,  811,  813,  815,  817,  819,  821,  834,  836,  841,  844,  847,  852,  858,
      860,  862,  864,  866,  868,  870,  872,  874,  876,  881,  882,  883,  885,  887,  892,  901,  903,  905,  907,
      909,  914,  926,  927,  932,  933,  934,  938,  939,  942,  944,  947,  948,  950,  959,  961,  963,  965,  967,
      972,  974,  978,  986,  987,  988,  989,  990,  991,  992,  993,  997,  999,  1001, 1003, 1005, 1007, 1009, 1017,
      1019, 1024, 1026, 1028, 1033, 1038, 1040, 1049, 1050, 1051, 1059, 1060, 1065, 1067, 1069, 1071, 1075, 1077, 1079,
      1090, 1092, 1095, 1096, 1100, 1101, 1105, 1106, 1110, 1111, 1112, 1113, 1122, 1123, 1124, 1125, 1129, 1131, 1136,
      1138, 1143, 1145, 1150, 1151, 1152, 1153, 1154, 1155, 1159, 1160, 1161, 1162, 1174, 1174, 1175, 1175
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
#line 5698 "langutils/sc_parser/src/sc_grammar_parser.cpp"

#line 1177 "langutils/sc_parser/src/sc_grammar.y"
