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
#line 93 "langutils/sc_parser/src/sc_grammar.y"


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

#line 66 "langutils/sc_parser/src/sc_grammar.y"
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
      yy_lac_established_ (false),
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
    , value (that.value)
    , location (that.location)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_MOVE_REF (location_type) l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (value_type) v, YY_RVREF (location_type) l)
    : Base (t)
    , value (YY_MOVE (v))
    , location (YY_MOVE (l))
  {}


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
    value = YY_MOVE (s.value);
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
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value), YY_MOVE (that.location))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value), YY_MOVE (that.location))
  {
    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
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

    // User destructor.
    YY_USE (yysym.kind ());
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

    // Discard the LAC context in case there still is one left from a
    // previous invocation.
    yy_lac_discard_ ("init");

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
        if (!yy_lac_establish_ (yyla.kind ()))
          goto yyerrlab;
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        if (!yy_lac_establish_ (yyla.kind ()))
          goto yyerrlab;

        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    yy_lac_discard_ ("shift");
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
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

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
#line 237 "langutils/sc_parser/src/sc_grammar.y"
                                     { (yylhs.value.v_GoIndex) = cxt.graph->assign_root((yystack_[2].value.v_RegionListIndex)); }
#line 638 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 3: // go: INTERPRET $end
#line 238 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_GoIndex) = cxt.graph->assign_root(cxt.create(RegionList{}, yylhs.location)); }
#line 644 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 4: // go: file_as_func
#line 239 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_GoIndex) = cxt.graph->assign_root((yystack_[0].value.v_FileAsFunctionIndex)); }
#line 650 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 5: // go: interpreted_function
#line 240 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_GoIndex) = cxt.graph->assign_root((yystack_[0].value.v_FileAsFunctionIndex)); }
#line 656 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 6: // go: classOrExtList.list $end
#line 241 "langutils/sc_parser/src/sc_grammar.y"
                                              { (yylhs.value.v_GoIndex) = cxt.graph->assign_root((yystack_[1].value.v_ClassOrExtensionListIndex)); }
#line 662 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 7: // go: %empty
#line 242 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_GoIndex) = cxt.graph->assign_root(cxt.create(ClassOrExtensionList{}, yylhs.location));}
#line 668 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 8: // file_as_func: INTERPRET argument_declarations block.contents semicolon.opt $end
#line 251 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 674 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 9: // file_as_func: INTERPRET variable_declarations SEMICOLON block.contents semicolon.opt $end
#line 254 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[5].location), cxt.graph->prepend((yystack_[2].value.v_BlockContentsListIndex), (yystack_[4].value.v_DeclareVariableListIndex))); }
#line 680 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 10: // file_as_func: INTERPRET variable_declarations semicolon.opt $end
#line 257 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[3].location), cxt.create(BlockContentsList{}, yystack_[1].location, (yystack_[2].value.v_DeclareVariableListIndex))); }
#line 686 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 11: // interpreted_function: INTERPRET_FUNCTION block.contents semicolon.opt $end
#line 268 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[3].location), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 692 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 12: // interpreted_function: INTERPRET_FUNCTION semicolon.opt $end
#line 271 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[2].location), cxt.create(BlockContentsList{}, yylhs.location)); }
#line 698 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 13: // interpreted_function: INTERPRET_FUNCTION OPENPAREN variable_declarations SEMICOLON block.contents semicolon.opt CLOSEPAREN $end
#line 274 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[7].location), cxt.graph->prepend((yystack_[3].value.v_BlockContentsListIndex), (yystack_[5].value.v_DeclareVariableListIndex))); }
#line 704 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 14: // interpreted_function: INTERPRET_FUNCTION OPENPAREN variable_declarations semicolon.opt CLOSEPAREN $end
#line 277 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[5].location), cxt.create(BlockContentsList{}, yystack_[3].location, (yystack_[3].value.v_DeclareVariableListIndex))); }
#line 710 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 15: // region.item: expr
#line 286 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 716 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 16: // region.item: NONLOCALRETURN expr
#line 289 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 722 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 17: // region.item: OPENPAREN argument_declarations block.contents semicolon.opt CLOSEPAREN
#line 291 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 728 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 18: // region.item: OPENPAREN argument_declarations CLOSEPAREN
#line 294 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, yystack_[0].location)); }
#line 734 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 19: // region.item: OPENPAREN variable_declarations SEMICOLON block.contents semicolon.opt CLOSEPAREN
#line 297 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[5].location), cxt.graph->prepend((yystack_[2].value.v_BlockContentsListIndex), (yystack_[4].value.v_DeclareVariableListIndex))); }
#line 740 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 20: // region: INTERPRET region.item
#line 302 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionListIndex) = cxt.create(RegionList{}, yylhs.location, (yystack_[0].value.v_RegionItemIndex)); }
#line 746 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 21: // region: INTERPRET error
#line 305 "langutils/sc_parser/src/sc_grammar.y"
                {
			error_recovery::expr(cxt);
			(yylhs.value.v_RegionListIndex) = cxt.create(RegionList{}, yylhs.location, cxt.create_error(yylhs.location, yystack_[0].location));
		}
#line 755 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 22: // region: region SEMICOLON region.item
#line 311 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionListIndex) = cxt.graph->append((yystack_[2].value.v_RegionListIndex), yylhs.location, (yystack_[0].value.v_RegionItemIndex)); }
#line 761 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 23: // region: region REGION_SEPARATOR region.item
#line 314 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionListIndex) = cxt.graph->append((yystack_[2].value.v_RegionListIndex), yylhs.location, (yystack_[0].value.v_RegionItemIndex)); }
#line 767 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 24: // region: region error
#line 317 "langutils/sc_parser/src/sc_grammar.y"
                {
			// A missing semicolon between lines is inserted by the lexer, see yylex.
			error_recovery::expr(cxt);
			(yylhs.value.v_RegionListIndex) = cxt.graph->append((yystack_[1].value.v_RegionListIndex), cxt.create_error(yystack_[0].location, yystack_[0].location, Index { *(yystack_[1].value.v_RegionListIndex) }));
			yyclearin;
		}
#line 778 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 25: // classOrExtList.list: classOrExtList.item
#line 334 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassOrExtensionListIndex) = cxt.create(ClassOrExtensionList{}, yylhs.location, (yystack_[0].value.v_ClassOrExtensionIndex)); }
#line 784 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 26: // classOrExtList.list: classOrExtList.list classOrExtList.item
#line 336 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassOrExtensionListIndex) = cxt.graph->append((yystack_[1].value.v_ClassOrExtensionListIndex), (yystack_[0].value.v_ClassOrExtensionIndex)); }
#line 790 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 27: // classOrExtList.item: class
#line 340 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassOrExtensionIndex) = (yystack_[0].value.v_ClassIndex); }
#line 796 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 28: // classOrExtList.item: class.extension
#line 341 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_ClassOrExtensionIndex) = (yystack_[0].value.v_ClassExtensionIndex); }
#line 802 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 29: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY members.opt method.list.opt CLOSECURLY
#line 346 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassIndex) = cxt.create(Class{ cxt.take_version(yystack_[3].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[6].location), (yystack_[5].value.v_maybe_NamedIdentifierIndex), (yystack_[4].value.v_maybe_ClassNameIdentifierIndex), (yystack_[2].value.v_BigMemberListIndex), (yystack_[1].value.v_MethodListIndex)); }
#line 808 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 30: // class.super.opt: %empty
#line 350 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_maybe_ClassNameIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 814 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 31: // class.super.opt: COLON CLASSNAME
#line 351 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_maybe_ClassNameIdentifierIndex) = cxt.create(ClassNameIdentifier{}, yystack_[0].location); }
#line 820 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 32: // class.slot.opt: %empty
#line 355 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_maybe_NamedIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 826 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 33: // class.slot.opt: OPENSQUARE name CLOSESQUARE
#line 356 "langutils/sc_parser/src/sc_grammar.y"
                                      { (yylhs.value.v_maybe_NamedIdentifierIndex) = (yystack_[1].value.v_NamedIdentifierIndex); }
#line 832 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 34: // class.slot.opt: OPENSQUARE CLOSESQUARE
#line 357 "langutils/sc_parser/src/sc_grammar.y"
                                 { (yylhs.value.v_maybe_NamedIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 838 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 35: // class.extension: ADD CLASSNAME OPENCURLY method.list.opt CLOSECURLY
#line 362 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassExtensionIndex) = cxt.create(ClassExtension{ cxt.take_version(yystack_[2].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[3].location), (yystack_[1].value.v_MethodListIndex)); }
#line 844 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 36: // members.entry.item: accessor variable_declarations.list.item
#line 371 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberIndex) = cxt.create(DeclareMember{}, yylhs.location, (yystack_[1].value.v_RWAccessorIndex), (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 850 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 37: // members.entry.list: members.entry.item
#line 376 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberListIndex) = cxt.create(DeclareMemberList{}, yylhs.location, (yystack_[0].value.v_DeclareMemberIndex)); }
#line 856 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 38: // members.entry.list: members.entry.list COMMA members.entry.item
#line 378 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberListIndex) = cxt.graph->append((yystack_[2].value.v_DeclareMemberListIndex), (yystack_[0].value.v_DeclareMemberIndex)); }
#line 862 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 39: // members.entry: CLASSVAR members.entry.list
#line 383 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyMemberListIndex) = cxt.graph->cast<DeclareClassMemberList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 868 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 40: // members.entry: VAR members.entry.list
#line 385 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyMemberListIndex) = cxt.graph->cast<DeclareMemberList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 874 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 41: // members.entry: CONST members.entry.list
#line 387 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyMemberListIndex) = cxt.graph->cast<DeclareConstList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 880 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 42: // members: members.entry
#line 392 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberListIndex) = cxt.create(BigMemberList{}, yylhs.location, (yystack_[0].value.v_DeclareAnyMemberListIndex));}
#line 886 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 43: // members: members SEMICOLON members.entry
#line 394 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberListIndex) = cxt.graph->append((yystack_[2].value.v_BigMemberListIndex), (yystack_[0].value.v_DeclareAnyMemberListIndex)); }
#line 892 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 44: // members.opt: %empty
#line 398 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_BigMemberListIndex) = cxt.create(BigMemberList{}, yylhs.location); }
#line 898 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 45: // members.opt: members semicolon.opt
#line 399 "langutils/sc_parser/src/sc_grammar.y"
                                { (yylhs.value.v_BigMemberListIndex) = (yystack_[1].value.v_BigMemberListIndex); }
#line 904 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 46: // method.name: name
#line 407 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 910 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 47: // method.name: binary_op.raw
#line 408 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 916 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 48: // method.base: method.name OPENCURLY argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 413 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(Missing{},yystack_[4].location), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 922 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 49: // method.base: method.name OPENCURLY argument_declarations.opt CLOSECURLY
#line 416 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[3].value.v_SelectorIndex), (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(Missing{},yystack_[2].location), cxt.create(BlockContentsList{}, yystack_[0].location)); }
#line 928 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 50: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt block.contents semicolon.opt CLOSECURLY
#line 419 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[7].value.v_SelectorIndex), (yystack_[5].value.v_DeclareArgumentListIndex), cxt.create(PrimitiveIdentifier{}, yystack_[4].location), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 934 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 51: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt CLOSECURLY
#line 422 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(PrimitiveIdentifier{}, yystack_[2].location), cxt.create(BlockContentsList{}, yystack_[1].location)); }
#line 940 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 52: // method.base: method.name OPENCURLY error
#line 425 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips past the method's closing curly. The method is kept, with the error as its body, so it is
			// poisoned rather than lost. Its arguments, if any, were discarded with the error.
			const auto close = error_recovery::method(cxt, yystack_[1].location);
			yylhs.location = range(yystack_[2].location, close);
			const auto body = cxt.create(BlockContentsList{}, range(yystack_[1].location, close), cxt.create_error(range(yystack_[1].location, close), yystack_[0].location));
			(yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[2].value.v_SelectorIndex), cxt.create(DeclareArgumentList{}, yystack_[1].location), cxt.create(Missing{}, yystack_[1].location), body);
			yyclearin;
			yyerrok; // Recovered at the closing curly, so report the next error.
		}
#line 955 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 53: // method: method.base
#line 438 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_AnyMethodIndex) = (yystack_[0].value.v_AnyMethodIndex); }
#line 961 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 54: // method: MULTIPLY method.base
#line 440 "langutils/sc_parser/src/sc_grammar.y"
                { 
			if (auto c = cxt.graph->as_a<MethodIndex>((yystack_[0].value.v_AnyMethodIndex))) {
				(yylhs.value.v_AnyMethodIndex) = cxt.graph->cast<ClassMethod>(*c);
			} 
			// leaves as error
			cxt.graph->location((yystack_[0].value.v_AnyMethodIndex)) = yylhs.location; 
			(yylhs.value.v_AnyMethodIndex) = (yystack_[0].value.v_AnyMethodIndex); 
		}
#line 974 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 55: // method.list: method
#line 451 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_MethodListIndex) = cxt.create(MethodList{}, yylhs.location, (yystack_[0].value.v_AnyMethodIndex)); }
#line 980 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 56: // method.list: method.list method
#line 452 "langutils/sc_parser/src/sc_grammar.y"
                             { (yylhs.value.v_MethodListIndex) = cxt.graph->append((yystack_[1].value.v_MethodListIndex), yylhs.location, (yystack_[0].value.v_AnyMethodIndex)); }
#line 986 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 57: // method.list.opt: %empty
#line 456 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_MethodListIndex) = cxt.create(MethodList{}, yylhs.location); }
#line 992 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 58: // method.list.opt: method.list
#line 457 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_MethodListIndex) = (yystack_[0].value.v_MethodListIndex); }
#line 998 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 59: // block.open: OPENCURLY
#line 466 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_bool) = false; }
#line 1004 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 60: // block.open: BEGINCLOSEDFUNC
#line 467 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_bool) = true; }
#line 1010 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 61: // block: block.open argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 472 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockIndex) = cxt.create(BlockNode{ (yystack_[4].value.v_bool) }, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 1016 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 62: // block: block.open argument_declarations.opt CLOSECURLY
#line 474 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockIndex) = cxt.create(BlockNode{ (yystack_[2].value.v_bool) }, yylhs.location, (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, yylhs.location)); }
#line 1022 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 63: // block.opt_list: %empty
#line 478 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location); }
#line 1028 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 64: // block.opt_list: block.list
#line 479 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_BlockListIndex) = (yystack_[0].value.v_BlockListIndex); }
#line 1034 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 65: // block.list: block
#line 483 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location, (yystack_[0].value.v_BlockIndex)); }
#line 1040 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 66: // block.list: generator
#line 484 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location, (yystack_[0].value.v_GeneratorAnyIndex)); }
#line 1046 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 67: // block.list: block.list block
#line 485 "langutils/sc_parser/src/sc_grammar.y"
                           { (yylhs.value.v_BlockListIndex) = cxt.graph->append((yystack_[1].value.v_BlockListIndex), yylhs.location, (yystack_[0].value.v_BlockIndex)); }
#line 1052 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 68: // block.list: block.list generator
#line 486 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_BlockListIndex) = cxt.graph->append((yystack_[1].value.v_BlockListIndex), yylhs.location, (yystack_[0].value.v_GeneratorAnyIndex)); }
#line 1058 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 69: // block.contents: block.contents.item
#line 490 "langutils/sc_parser/src/sc_grammar.y"
                              { (yylhs.value.v_BlockContentsListIndex) = cxt.create(BlockContentsList{}, yylhs.location, (yystack_[0].value.v_BlockItemIndex)); }
#line 1064 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 70: // block.contents: block.contents SEMICOLON block.contents.item
#line 491 "langutils/sc_parser/src/sc_grammar.y"
                                                       { (yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[2].value.v_BlockContentsListIndex), yylhs.location, (yystack_[0].value.v_BlockItemIndex)); }
#line 1070 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 71: // block.contents: block.contents NONLOCALRETURN expr
#line 493 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[2].value.v_BlockContentsListIndex), yylhs.location, non_local); 
		}
#line 1079 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 72: // block.contents: block.contents SEMICOLON NONLOCALRETURN expr
#line 498 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[3].value.v_BlockContentsListIndex), yylhs.location, non_local); 
		}
#line 1088 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 73: // block.contents: NONLOCALRETURN expr
#line 503 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.create(BlockContentsList{}, yylhs.location, non_local);
		}
#line 1097 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 74: // block.contents.item: expr
#line 510 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_BlockItemIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1103 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 75: // block.contents.item: variable_declarations
#line 511 "langutils/sc_parser/src/sc_grammar.y"
                                { (yylhs.value.v_BlockItemIndex) = (yystack_[0].value.v_DeclareVariableListIndex); }
#line 1109 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 76: // adverb: DOT name
#line 520 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_AdverbIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 1115 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 77: // adverb: DOT integer
#line 521 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_AdverbIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 1121 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 78: // adverb: DOT OPENPAREN expr_or_seq CLOSEPAREN
#line 522 "langutils/sc_parser/src/sc_grammar.y"
                                               { (yylhs.value.v_AdverbIndex) = cxt.create(AdverbExprNode{}, yylhs.location, (yystack_[1].value.v_AnyExprIndex));  }
#line 1127 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 79: // binary_op.raw: BINOP
#line 526 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1133 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 80: // binary_op.raw: READWRITEVAR
#line 527 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1139 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 81: // binary_op.raw: LESSTHAN
#line 528 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1145 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 82: // binary_op.raw: GREATERTHAN
#line 529 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1151 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 83: // binary_op.raw: MINUS
#line 530 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1157 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 84: // binary_op.raw: MULTIPLY
#line 531 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1163 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 85: // binary_op.raw: ADD
#line 532 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1169 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 86: // binary_op.raw: PIPE
#line 533 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1175 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 87: // binary_op.no_adverb: binary_op.raw
#line 537 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 1181 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 88: // binary_op.no_adverb: KEYBINOP
#line 538 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ true }, yylhs.location); }
#line 1187 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 89: // binary_op: binary_op.no_adverb adverb
#line 543 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorMaybeAdverbIndex) = cxt.create(SelectorWAdverb{}, yylhs.location, (yystack_[1].value.v_SelectorIndex), (yystack_[0].value.v_AdverbIndex)); }
#line 1193 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 90: // binary_op: binary_op.no_adverb
#line 545 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorMaybeAdverbIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 1199 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 91: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN arguments CLOSEPAREN block.opt_list
#line 550 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[2].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1205 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 92: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN CLOSEPAREN block.list
#line 553 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[4].value.v_SelectorIndex), cxt.create(ArgumentList{}, yystack_[2].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1211 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 93: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN block.list
#line 556 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[2].value.v_SelectorIndex), cxt.create(ArgumentList{}, yystack_[0].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1217 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 94: // msgsend: name OPENPAREN arguments CLOSEPAREN block.opt_list
#line 560 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[2].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1223 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 95: // msgsend: name OPENPAREN CLOSEPAREN block.list
#line 563 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), cxt.create(ArgumentList{}, yystack_[2].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1229 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 96: // msgsend: name block.list
#line 566 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[1].value.v_NamedIdentifierIndex), cxt.create(ArgumentList{}, yystack_[0].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1235 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 97: // msgsend: expr DOT name arguments.maybe_paren block.opt_list
#line 569 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PostfixMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1241 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 98: // msgsend: expr DOT arguments.paren block.opt_list
#line 572 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(ValueMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1247 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 99: // msgsend: expr DOT OPENPAREN CLOSEPAREN block.opt_list
#line 575 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(ValueMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[2].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1253 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 100: // msgsend: CLASSNAME OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 585 "langutils/sc_parser/src/sc_grammar.y"
                {  (yylhs.value.v_AnyExprIndex) = cxt.create(CollectionNode{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[4].location), (yystack_[2].value.v_ArrayIndex)); }
#line 1259 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 101: // msgsend: CLASSNAME block.list
#line 588 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[1].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, yystack_[1].location), (yystack_[0].value.v_BlockListIndex));
		}
#line 1268 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 102: // msgsend: CLASSNAME arguments.paren block.opt_list
#line 593 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[2].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex));
		}
#line 1277 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 103: // msgsend: CLASSNAME OPENPAREN CLOSEPAREN block.opt_list
#line 599 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[3].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, yystack_[3].location), (yystack_[0].value.v_BlockListIndex));
		}
#line 1286 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 104: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 614 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[3].value.v_ArgumentListIndex), (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r);
		}
#line 1296 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 105: // expr.dotrange: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 621 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[3].location),  (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1306 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 106: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 628 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[2].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[1].location));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1316 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 107: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 637 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[5].value.v_ArgumentListIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[8].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1326 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 108: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 644 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[4].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[3].location));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1336 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 109: // expr.dotrange.assign: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 651 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[5].location),  (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1346 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 110: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 661 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[3].value.v_ArgumentListIndex), (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1356 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 111: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 667 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[2].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[0].location));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), r);	
		}
#line 1366 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 112: // expr.base.range: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 673 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[3].location),  (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), r);	
		}
#line 1376 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 113: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 682 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[5].value.v_ArgumentListIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1386 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 114: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 688 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[4].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[3].location));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1396 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 115: // expr.base.range.assign: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 694 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[5].location),  (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1406 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 116: // range: OPENPAREN expr_or_seq DOTDOT CLOSEPAREN
#line 703 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1415 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 117: // range: OPENPAREN expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 708 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[4].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1424 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 118: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 713 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[6].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1433 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 119: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 718 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[4].location, yystack_[2].location), (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[5].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1442 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 120: // range: OPENPAREN DOTDOT expr_or_seq CLOSEPAREN
#line 723 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location);
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1451 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 121: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 731 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[5].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1460 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 122: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 736 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[7].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1469 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 123: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 742 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[4].location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[6].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1478 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 124: // range.generator: OPENPAREN COLON DOTDOT expr_or_seq CLOSEPAREN
#line 747 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location);
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1487 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 125: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT CLOSEPAREN
#line 752 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1496 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 126: // expr.base: literal
#line 765 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyPossiblyLiteralIndex); }
#line 1502 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 127: // expr.base: generator
#line 768 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_GeneratorAnyIndex); }
#line 1508 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 128: // expr.base: name
#line 771 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 1514 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 129: // expr.base: CURRYARG
#line 774 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyExprIndex) = cxt.create(CurryNode{}, yystack_[0].location); }
#line 1520 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 130: // expr.base: msgsend
#line 777 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1526 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 131: // expr.base: OPENPAREN expr_or_seq CLOSEPAREN
#line 780 "langutils/sc_parser/src/sc_grammar.y"
                { 
			// A nasty hack because of nil assignment.
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AnyExprIndex) = (yystack_[1].value.v_AnyExprIndex);
		}
#line 1536 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 132: // expr.base: OPENPAREN error
#line 787 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto close = error_recovery::bracket(cxt, yystack_[1].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create_error(range(yystack_[1].location, close), yystack_[0].location);
			yyclearin; // The lookahead was skipped.
			yyerrok; // Recovered at the matching bracket, so report the next error.
		}
#line 1547 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 133: // expr.base: TILDE name
#line 795 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(EnvIdentifierNode{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1553 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 134: // expr.base: expr.base OPENSQUARE arguments CLOSESQUARE
#line 798 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex) ); }
#line 1559 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 135: // expr.base: expr.base OPENSQUARE CLOSESQUARE
#line 801 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[1].location)); }
#line 1565 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 136: // expr.base: expr.base.range
#line 804 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AtMessageIndex); }
#line 1571 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 137: // expr.base: range
#line 807 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeIndex); }
#line 1577 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 138: // expr.base: range.generator
#line 810 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeGeneratorIndex); }
#line 1583 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 139: // expr: expr.base
#line 815 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1589 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 140: // expr: expr.base.range.assign
#line 816 "langutils/sc_parser/src/sc_grammar.y"
                                 { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeAssignIndex); }
#line 1595 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 141: // expr: expr.dotrange
#line 817 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AtMessageIndex); }
#line 1601 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 142: // expr: expr.dotrange.assign
#line 818 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeAssignIndex); }
#line 1607 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 143: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE
#line 821 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex) ); }
#line 1613 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 144: // expr: expr DOT OPENSQUARE CLOSESQUARE
#line 824 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[1].location) ); }
#line 1619 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 145: // expr: expr binary_op expr
#line 827 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(BinaryMessage{}, yylhs.location, (yystack_[1].value.v_SelectorMaybeAdverbIndex), (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1625 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 146: // expr: expr DOT name EQUALSSIGN expr
#line 830 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(SetterNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1631 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 147: // expr: CLASSNAME
#line 832 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_AnyExprIndex) = cxt.create(ClassNameIdentifier{}, yylhs.location); }
#line 1637 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 148: // expr: BACKTICK expr
#line 833 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyExprIndex) = cxt.create(ReferenceNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1643 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 149: // expr: name EQUALSSIGN expr
#line 835 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentNode{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1649 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 150: // expr: TILDE name EQUALSSIGN expr
#line 838 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentNode{AssignmentNode::Target::Environment}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1655 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 151: // expr: name OPENPAREN arguments CLOSEPAREN EQUALSSIGN expr
#line 841 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(SetterNode{}, yylhs.location, (yystack_[3].value.v_ArgumentListIndex), (yystack_[5].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1661 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 152: // expr: name.pack EQUALSSIGN expr
#line 844 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[2].value.v_NamePackIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1667 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 153: // expr: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 847 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[3].value.v_NamePackIndex), (yystack_[1].value.v_AnyExprIndex)); }
#line 1673 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 154: // expr: expr.base OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 853 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_ArgumentListIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1679 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 155: // expr: expr.base OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 856 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[3].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 1685 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 156: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 859 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), (yystack_[3].value.v_ArgumentListIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1691 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 157: // expr: expr DOT OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 862 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[3].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 1697 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 158: // expr_or_seq.base: expr
#line 869 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1703 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 159: // expr_or_seq.base: expr_or_seq.base SEMICOLON expr
#line 870 "langutils/sc_parser/src/sc_grammar.y"
                                          { (yylhs.value.v_AnyExprIndex) = cxt.create(ExprSeq{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1709 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 160: // expr_or_seq: expr_or_seq.base semicolon.opt
#line 874 "langutils/sc_parser/src/sc_grammar.y"
                                       { (yylhs.value.v_AnyExprIndex) = (yystack_[1].value.v_AnyExprIndex); }
#line 1715 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 161: // generator: OPENCURLY COLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 883 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorAnyIndex) = cxt.create(GeneratorNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_GeneratorQualListIndex)); }
#line 1721 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 162: // generator: OPENCURLY SEMICOLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 886 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorAnyIndex) = cxt.create(GeneratorFakeNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_GeneratorQualListIndex)); }
#line 1727 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 163: // generator.qual.list: generator.qual
#line 891 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualListIndex) = cxt.create(GeneratorQualList{}, yylhs.location, (yystack_[0].value.v_GeneratorQualAnyIndex)); }
#line 1733 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 164: // generator.qual.list: generator.qual.list COMMA generator.qual
#line 893 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualListIndex) = cxt.graph->append((yystack_[2].value.v_GeneratorQualListIndex), yylhs.location, (yystack_[0].value.v_GeneratorQualAnyIndex)); }
#line 1739 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 165: // generator.qual: name LEFTARROW expr_or_seq
#line 898 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualAssign{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), cxt.create(Missing{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 1745 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 166: // generator.qual: name name LEFTARROW expr_or_seq
#line 900 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualAssign{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1751 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 167: // generator.qual: VAR name EQUALSSIGN expr_or_seq
#line 902 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualDeclare{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1757 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 168: // generator.qual: VAR name OPENPAREN expr_or_seq CLOSEPAREN
#line 904 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualDeclare{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex)); }
#line 1763 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 169: // generator.qual: expr_or_seq
#line 906 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualIf{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1769 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 170: // generator.qual: COLON COLON expr_or_seq
#line 908 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualSideEffect{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1775 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 171: // generator.qual: COLON WHILE expr_or_seq
#line 910 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualGuard{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1781 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 172: // argument_declarations.list: name
#line 923 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1787 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 173: // argument_declarations.list: name EQUALSSIGN expr
#line 925 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed((yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{is_bracketed}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex))); 
		}
#line 1796 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 174: // argument_declarations.list: name OPENPAREN expr_or_seq CLOSEPAREN
#line 930 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1802 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 175: // argument_declarations.list: argument_declarations.list COMMA name
#line 933 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[2].value.v_DeclareArgumentListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1808 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 176: // argument_declarations.list: argument_declarations.list COMMA name EQUALSSIGN expr
#line 936 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed((yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[4].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{is_bracketed}, range(yystack_[2].location, yystack_[0].location), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)));
		}
#line 1817 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 177: // argument_declarations.list: argument_declarations.list COMMA name OPENPAREN expr_or_seq CLOSEPAREN
#line 941 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[3].location, yystack_[0].location), (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1823 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 178: // argument_declarations.pipelist: name literal
#line 947 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, (yystack_[1].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1829 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 179: // argument_declarations.pipelist: name
#line 949 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1835 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 180: // argument_declarations.pipelist: name EQUALSSIGN literal
#line 951 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1841 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 181: // argument_declarations.pipelist: name OPENPAREN expr_or_seq CLOSEPAREN
#line 953 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1847 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 182: // argument_declarations.pipelist: name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 955 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1853 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 183: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name literal
#line 957 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[3].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[1].location, yystack_[0].location), (yystack_[1].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1859 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 184: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name
#line 959 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[2].value.v_DeclareArgumentListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1865 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 185: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN literal
#line 961 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[4].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[2].location, yystack_[0].location), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1871 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 186: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name OPENPAREN expr_or_seq CLOSEPAREN
#line 963 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[3].location, yystack_[0].location), (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1877 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 187: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 965 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[6].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[4].location, yystack_[0].location), (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1883 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 188: // argument_declarations: ARG SEMICOLON
#line 969 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 1889 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 189: // argument_declarations: ARG argument_declarations.list comma.opt SEMICOLON
#line 970 "langutils/sc_parser/src/sc_grammar.y"
                                                             { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[2].value.v_DeclareArgumentListIndex); }
#line 1895 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 190: // argument_declarations: ARG argument_declarations.list ELLIPSIS name SEMICOLON
#line 972 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[3].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 1901 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 191: // argument_declarations: ARG argument_declarations.list ELLIPSIS name COMMA name SEMICOLON
#line 974 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 1907 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 192: // argument_declarations: ARG ELLIPSIS name SEMICOLON
#line 976 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(arg_list, yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); 
		}
#line 1916 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 193: // argument_declarations: ARG ELLIPSIS name COMMA name SEMICOLON
#line 981 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(
				arg_list, yylhs.location,
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))
			); 
		}
#line 1929 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 194: // argument_declarations: PIPE PIPE
#line 990 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 1935 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 195: // argument_declarations: PIPE argument_declarations.pipelist comma.opt PIPE
#line 992 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[2].value.v_DeclareArgumentListIndex); }
#line 1941 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 196: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name PIPE
#line 994 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[3].value.v_DeclareArgumentListIndex), yystack_[3].location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 1947 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 197: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name COMMA name PIPE
#line 996 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 1953 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 198: // argument_declarations: PIPE ELLIPSIS name PIPE
#line 998 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(arg_list, yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); 
		}
#line 1962 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 199: // argument_declarations: PIPE ELLIPSIS name COMMA name PIPE
#line 1003 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(
				arg_list, yylhs.location,
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))
			); 
		}
#line 1975 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 200: // argument_declarations.opt: %empty
#line 1014 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 1981 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 201: // argument_declarations.opt: argument_declarations
#line 1015 "langutils/sc_parser/src/sc_grammar.y"
                                { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[0].value.v_DeclareArgumentListIndex); }
#line 1987 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 202: // arguments.entries: KEYBINOP expr_or_seq
#line 1020 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_ArgumentEntryIndex) = cxt.create(KwArgNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 1993 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 203: // arguments.entries: MULTIPLY expr_or_seq
#line 1021 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_ArgumentEntryIndex) = cxt.create(VariadicArgNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1999 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 204: // arguments.entries: expr_or_seq
#line 1022 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_ArgumentEntryIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 2005 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 205: // arguments.no_trailing: arguments.entries
#line 1026 "langutils/sc_parser/src/sc_grammar.y"
                            {  (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location, (yystack_[0].value.v_ArgumentEntryIndex)); }
#line 2011 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 206: // arguments.no_trailing: arguments.no_trailing COMMA arguments.entries
#line 1027 "langutils/sc_parser/src/sc_grammar.y"
                                                        { (yylhs.value.v_ArgumentListIndex) = cxt.graph->append((yystack_[2].value.v_ArgumentListIndex), yylhs.location, (yystack_[0].value.v_ArgumentEntryIndex)); }
#line 2017 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 207: // arguments: arguments.no_trailing comma.opt
#line 1030 "langutils/sc_parser/src/sc_grammar.y"
                                            { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2023 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 208: // arguments.paren: OPENPAREN arguments CLOSEPAREN
#line 1032 "langutils/sc_parser/src/sc_grammar.y"
                                                 { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2029 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 209: // arguments.maybe_paren: %empty
#line 1035 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location); }
#line 2035 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 210: // arguments.maybe_paren: OPENPAREN CLOSEPAREN
#line 1037 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location); }
#line 2041 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 211: // arguments.maybe_paren: OPENPAREN arguments CLOSEPAREN
#line 1039 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2047 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 212: // variable_declarations.list.item: name
#line 1048 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 2053 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 213: // variable_declarations.list.item: name EQUALSSIGN expr
#line 1050 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2059 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 214: // variable_declarations.list.item: name OPENPAREN expr_or_seq CLOSEPAREN
#line 1052 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Bracketed, as in the old compiler, so it isn't a literal, see ConstEval::is_legacy_literal.
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex));
		}
#line 2069 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 215: // variable_declarations.list.item: name.pack EQUALSSIGN expr
#line 1058 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[2].value.v_NamePackIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2075 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 216: // variable_declarations.list.item: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 1060 "langutils/sc_parser/src/sc_grammar.y"
                {
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[3].value.v_NamePackIndex), (yystack_[1].value.v_AnyExprIndex));
		}
#line 2084 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 217: // variable_declarations.list: variable_declarations.list.item
#line 1068 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareVariableListIndex) = cxt.create(DeclareVariableList{}, yylhs.location, (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 2090 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 218: // variable_declarations.list: variable_declarations.list COMMA variable_declarations.list.item
#line 1070 "langutils/sc_parser/src/sc_grammar.y"
                {(yylhs.value.v_DeclareVariableListIndex) = cxt.graph->append((yystack_[2].value.v_DeclareVariableListIndex), (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 2096 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 219: // variable_declarations: VAR variable_declarations.list
#line 1073 "langutils/sc_parser/src/sc_grammar.y"
                                                       { (yylhs.value.v_DeclareVariableListIndex) = (yystack_[0].value.v_DeclareVariableListIndex); }
#line 2102 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 220: // literal.terminal: symbol
#line 1081 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_AnySymbolIndex); }
#line 2108 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 221: // literal.terminal: string
#line 1082 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_StringLitIndex); }
#line 2114 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 222: // literal.terminal: integer
#line 1083 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 2120 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 223: // literal.terminal: float
#line 1084 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_FloatProducingIndex); }
#line 2126 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 224: // literal.terminal: boolean
#line 1085 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_BooleanLitIndex); }
#line 2132 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 225: // literal.terminal: nil
#line 1086 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_NilLitIndex); }
#line 2138 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 226: // literal.terminal: ascii
#line 1087 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_ASCIIIndex); }
#line 2144 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 227: // literal.terminal: block
#line 1088 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_BlockIndex); }
#line 2150 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 228: // literal.array.contents: %empty
#line 1093 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location); }
#line 2156 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 229: // literal.array.contents: expr_or_seq
#line 1095 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2162 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 230: // literal.array.contents: expr_or_seq COLON expr_or_seq
#line 1097 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2168 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 231: // literal.array.contents: KEYBINOP expr_or_seq
#line 1099 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2174 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 232: // literal.array.contents: literal.array.contents COMMA expr_or_seq
#line 1101 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[2].value.v_ArrayIndex), yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2180 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 233: // literal.array.contents: literal.array.contents COMMA expr_or_seq COLON expr_or_seq
#line 1103 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[4].value.v_ArrayIndex), yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2186 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 234: // literal.array.contents: literal.array.contents COMMA KEYBINOP expr_or_seq
#line 1105 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[3].value.v_ArrayIndex), yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2192 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 235: // literal.dictionary.entry: expr COLON expr_or_seq
#line 1113 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryEntryIndex) = cxt.create(DictionaryEntryNode{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2198 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 236: // literal.dictionary.entry: KEYBINOP expr_or_seq
#line 1115 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryEntryIndex) = cxt.create(DictionaryEntryNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2204 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 237: // literal.dictionary.entries: %empty
#line 1120 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.create(DictionaryNode{}, yylhs.location); }
#line 2210 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 238: // literal.dictionary.entries: literal.dictionary.entry
#line 1122 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.create(DictionaryNode{}, yylhs.location, (yystack_[0].value.v_DictionaryEntryIndex)); }
#line 2216 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 239: // literal.dictionary.entries: literal.dictionary.entries COMMA literal.dictionary.entry
#line 1124 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.graph->append((yystack_[2].value.v_DictionaryIndex), yylhs.location,  (yystack_[0].value.v_DictionaryEntryIndex)); }
#line 2222 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 240: // literal.dictionary: OPENPAREN literal.dictionary.entries comma.opt CLOSEPAREN
#line 1129 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->location(*(yystack_[2].value.v_DictionaryIndex)) = yylhs.location; (yylhs.value.v_DictionaryIndex) = (yystack_[2].value.v_DictionaryIndex); }
#line 2228 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 241: // literal.array: OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1134 "langutils/sc_parser/src/sc_grammar.y"
                {  cxt.graph->location(*(yystack_[2].value.v_ArrayIndex)) = yylhs.location; (yylhs.value.v_ArrayIndex) = (yystack_[2].value.v_ArrayIndex); }
#line 2234 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 242: // literal.array: HASH OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1136 "langutils/sc_parser/src/sc_grammar.y"
                {  
			cxt.graph->payload((yystack_[2].value.v_ArrayIndex)).is_immutable = true;
			cxt.graph->location(*(yystack_[2].value.v_ArrayIndex)) = yylhs.location;
			(yylhs.value.v_ArrayIndex) = (yystack_[2].value.v_ArrayIndex);
		}
#line 2244 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 243: // literal: literal.terminal
#line 1144 "langutils/sc_parser/src/sc_grammar.y"
                           { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_AnyPossiblyLiteralIndex); }
#line 2250 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 244: // literal: literal.array
#line 1145 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_ArrayIndex); }
#line 2256 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 245: // literal: literal.dictionary
#line 1146 "langutils/sc_parser/src/sc_grammar.y"
                             { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_DictionaryIndex); }
#line 2262 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 246: // name: NAME
#line 1154 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_NamedIdentifierIndex) = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 2268 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 247: // name: WHILE
#line 1155 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamedIdentifierIndex) = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 2274 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 248: // name.list: HASH name
#line 1161 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.create(NameList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2280 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 249: // name.list: HASH TILDE name
#line 1163 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.create(NameList{}, yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2286 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 250: // name.list: name.list COMMA name
#line 1165 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.graph->append((yystack_[2].value.v_NameListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2292 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 251: // name.list: name.list COMMA TILDE name
#line 1167 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.graph->append((yystack_[3].value.v_NameListIndex), yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2298 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 252: // name.pack: name.list
#line 1171 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[0].value.v_NameListIndex), cxt.create(Missing{}, yystack_[0].location)); }
#line 2304 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 253: // name.pack: name.list ELLIPSIS name
#line 1173 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[2].value.v_NameListIndex), (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2310 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 254: // name.pack: name.list ELLIPSIS TILDE name
#line 1175 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[3].value.v_NameListIndex), cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2316 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 255: // ascii: ASCII
#line 1185 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_ASCIIIndex) = cxt.create(ASCIINode{}, yylhs.location); }
#line 2322 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 256: // nil: NIL
#line 1187 "langutils/sc_parser/src/sc_grammar.y"
          { (yylhs.value.v_NilLitIndex) = cxt.create(NilNode{}, yylhs.location); }
#line 2328 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 257: // boolean: TRUE
#line 1190 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_BooleanLitIndex) = cxt.create(BooleanNode{true}, yylhs.location); }
#line 2334 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 258: // boolean: FALSE
#line 1191 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BooleanLitIndex) = cxt.create(BooleanNode{false}, yylhs.location); }
#line 2340 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 259: // symbol: SYMBOL_QUOTE
#line 1195 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_AnySymbolIndex) = cxt.create(SymbolQuote{}, yylhs.location); }
#line 2346 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 260: // symbol: SYMBOL_SLASH
#line 1196 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_AnySymbolIndex) = cxt.create(SymbolSlash{}, yylhs.location); }
#line 2352 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 261: // string: STRINGLINE
#line 1200 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_StringLitIndex) = cxt.create(StringLineList{}, yylhs.location, cxt.create(StringLineNode{}, yylhs.location)); }
#line 2358 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 262: // string: string STRINGLINE
#line 1201 "langutils/sc_parser/src/sc_grammar.y"
                            { (yylhs.value.v_StringLitIndex) = cxt.graph->append((yystack_[1].value.v_StringLitIndex), cxt.create(StringLineNode{}, yystack_[0].location)); }
#line 2364 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 263: // integer: INTEGER
#line 1205 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{}, yylhs.location); }
#line 2370 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 264: // integer: INTEGER_RADIX
#line 1206 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{IntNode::Kind::Radix}, yylhs.location); }
#line 2376 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 265: // integer: HEXADECIMAL
#line 1207 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{IntNode::Kind::Hexadecimal}, yylhs.location); }
#line 2382 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 266: // integer: MINUS integer
#line 1209 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Reaches into the previous integer and changes its sign.
			cxt.graph->payload((yystack_[0].value.v_IntLitIndex)).sign = IntNode::Sign::Negative;
			(yylhs.value.v_IntLitIndex) = (yystack_[0].value.v_IntLitIndex);
		}
#line 2392 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 267: // float.raw_unsigned: FLOAT
#line 1217 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{}, yylhs.location); }
#line 2398 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 268: // float.raw_unsigned: FLOAT_RADIX
#line 1218 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Radix}, yylhs.location); }
#line 2404 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 269: // float.raw_unsigned: FLOAT_EXPONENT
#line 1219 "langutils/sc_parser/src/sc_grammar.y"
                         { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Exponent}, yylhs.location); }
#line 2410 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 270: // float.raw_unsigned: FLOAT_INF
#line 1220 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Inf}, yylhs.location); }
#line 2416 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 271: // float.raw: float.raw_unsigned
#line 1225 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FloatLitIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2422 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 272: // float.raw: MINUS float.raw_unsigned
#line 1227 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload((yystack_[0].value.v_FloatLitIndex)).sign = FloatNode::Sign::Negative; (yylhs.value.v_FloatLitIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2428 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 273: // accidental.unsigned: ACCIDENTAL_STEPS
#line 1232 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = cxt.create(AccidentalNode{AccidentalNode::Kind::Steps}, yylhs.location); }
#line 2434 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 274: // accidental.unsigned: ACCIDENTAL_CENTS
#line 1234 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = cxt.create(AccidentalNode{AccidentalNode::Kind::Cents}, yylhs.location); }
#line 2440 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 275: // accidental: accidental.unsigned
#line 1239 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2446 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 276: // accidental: MINUS accidental.unsigned
#line 1241 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload((yystack_[0].value.v_AccidentalIndex)).sign = AccidentalNode::Sign::Negative; (yylhs.value.v_AccidentalIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2452 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 277: // float: float.raw
#line 1245 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_FloatProducingIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2458 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 278: // float: accidental
#line 1246 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_FloatProducingIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2464 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 279: // float: float.raw PI
#line 1247 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, (yystack_[1].value.v_FloatLitIndex)); }
#line 2470 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 280: // float: integer PI
#line 1248 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, (yystack_[1].value.v_IntLitIndex)); }
#line 2476 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 281: // float: PI
#line 1249 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, cxt.create(Missing{}, yylhs.location)); }
#line 2482 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 282: // float: MINUS PI
#line 1250 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{PiNode::Sign::Negative}, yylhs.location, cxt.create(Missing{}, yylhs.location)); }
#line 2488 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 283: // accessor: %empty
#line 1254 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::Private}, yylhs.location); }
#line 2494 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 284: // accessor: LESSTHAN
#line 1255 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyReadable}, yylhs.location); }
#line 2500 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 285: // accessor: GREATERTHAN
#line 1256 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyWriteable}, yylhs.location); }
#line 2506 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 286: // accessor: READWRITEVAR
#line 1258 "langutils/sc_parser/src/sc_grammar.y"
                { 
			(yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{
				combine<ReadWriteAccessor::ExternallyReadable, ReadWriteAccessor::ExternallyWriteable>()
			}, yylhs.location); 
		}
#line 2516 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;


#line 2520 "langutils/sc_parser/src/sc_grammar_parser.cpp"

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
      yy_lac_discard_ ("error recovery");
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
  "BEGINCLOSEDFUNC", "BADTOKEN", "INTERPRET", "INTERPRET_FUNCTION",
  "LEFTARROW", "WHILE", "LEXER_ERROR", "COLON", "EQUALSSIGN", "BINOP",
  "KEYBINOP", "MINUS", "LESSTHAN", "GREATERTHAN", "MULTIPLY", "ADD",
  "PIPE", "READWRITEVAR", "DOT", "BACKTICK", "UMINUS", "$accept", "go",
  "file_as_func", "interpreted_function", "region.item", "region",
  "classOrExtList.list", "classOrExtList.item", "class", "class.super.opt",
  "class.slot.opt", "class.extension", "members.entry.item",
  "members.entry.list", "members.entry", "members", "members.opt",
  "method.name", "method.base", "method", "method.list", "method.list.opt",
  "block.open", "block", "block.opt_list", "block.list", "block.contents",
  "block.contents.item", "adverb", "binary_op.raw", "binary_op.no_adverb",
  "binary_op", "msgsend", "expr.dotrange", "expr.dotrange.assign",
  "expr.base.range", "expr.base.range.assign", "range", "range.generator",
  "expr.base", "expr", "expr_or_seq.base", "expr_or_seq", "generator",
  "generator.qual.list", "generator.qual", "argument_declarations.list",
  "argument_declarations.pipelist", "argument_declarations",
  "argument_declarations.opt", "arguments.entries",
  "arguments.no_trailing", "arguments", "arguments.paren",
  "arguments.maybe_paren", "variable_declarations.list.item",
  "variable_declarations.list", "variable_declarations",
  "literal.terminal", "literal.array.contents", "literal.dictionary.entry",
  "literal.dictionary.entries", "literal.dictionary", "literal.array",
  "literal", "name", "name.list", "name.pack", "ascii", "nil", "boolean",
  "symbol", "string", "integer", "float.raw_unsigned", "float.raw",
  "accidental.unsigned", "accidental", "float", "accessor",
  "semicolon.opt", "comma.opt", YY_NULLPTR
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

#if YYDEBUG
    // Execute LAC once. We don't care if it is successful, we
    // only do it for the sake of debugging output.
    if (!yyparser_.yy_lac_established_)
      yyparser_.yy_lac_check_ (yyla_.kind ());
#endif

    for (int yyx = 0; yyx < YYNTOKENS; ++yyx)
      {
        symbol_kind_type yysym = YY_CAST (symbol_kind_type, yyx);
        if (yysym != symbol_kind::S_YYerror
            && yysym != symbol_kind::S_YYUNDEF
            && yyparser_.yy_lac_check_ (yysym))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = yysym;
          }
      }
    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }




  bool
  parser::yy_lac_check_ (symbol_kind_type yytoken) const
  {
    // Logically, the yylac_stack's lifetime is confined to this function.
    // Clear it, to get rid of potential left-overs from previous call.
    yylac_stack_.clear ();
    // Reduce until we encounter a shift and thereby accept the token.
#if YYDEBUG
    YYCDEBUG << "LAC: checking lookahead " << symbol_name (yytoken) << ':';
#endif
    std::ptrdiff_t lac_top = 0;
    while (true)
      {
        state_type top_state = (yylac_stack_.empty ()
                                ? yystack_[lac_top].state
                                : yylac_stack_.back ());
        int yyrule = yypact_[+top_state];
        if (yy_pact_value_is_default_ (yyrule)
            || (yyrule += yytoken) < 0 || yylast_ < yyrule
            || yycheck_[yyrule] != yytoken)
          {
            // Use the default action.
            yyrule = yydefact_[+top_state];
            if (yyrule == 0)
              {
                YYCDEBUG << " Err\n";
                return false;
              }
          }
        else
          {
            // Use the action from yytable.
            yyrule = yytable_[yyrule];
            if (yy_table_value_is_error_ (yyrule))
              {
                YYCDEBUG << " Err\n";
                return false;
              }
            if (0 < yyrule)
              {
                YYCDEBUG << " S" << yyrule << '\n';
                return true;
              }
            yyrule = -yyrule;
          }
        // By now we know we have to simulate a reduce.
        YYCDEBUG << " R" << yyrule - 1;
        // Pop the corresponding number of values from the stack.
        {
          std::ptrdiff_t yylen = yyr2_[yyrule];
          // First pop from the LAC stack as many tokens as possible.
          std::ptrdiff_t lac_size = std::ptrdiff_t (yylac_stack_.size ());
          if (yylen < lac_size)
            {
              yylac_stack_.resize (std::size_t (lac_size - yylen));
              yylen = 0;
            }
          else if (lac_size)
            {
              yylac_stack_.clear ();
              yylen -= lac_size;
            }
          // Only afterwards look at the main stack.
          // We simulate popping elements by incrementing lac_top.
          lac_top += yylen;
        }
        // Keep top_state in sync with the updated stack.
        top_state = (yylac_stack_.empty ()
                     ? yystack_[lac_top].state
                     : yylac_stack_.back ());
        // Push the resulting state of the reduction.
        state_type state = yy_lr_goto_state_ (top_state, yyr1_[yyrule]);
        YYCDEBUG << " G" << int (state);
        yylac_stack_.push_back (state);
      }
  }

  // Establish the initial context if no initial context currently exists.
  bool
  parser::yy_lac_establish_ (symbol_kind_type yytoken)
  {
    /* Establish the initial context for the current lookahead if no initial
       context is currently established.

       We define a context as a snapshot of the parser stacks.  We define
       the initial context for a lookahead as the context in which the
       parser initially examines that lookahead in order to select a
       syntactic action.  Thus, if the lookahead eventually proves
       syntactically unacceptable (possibly in a later context reached via a
       series of reductions), the initial context can be used to determine
       the exact set of tokens that would be syntactically acceptable in the
       lookahead's place.  Moreover, it is the context after which any
       further semantic actions would be erroneous because they would be
       determined by a syntactically unacceptable token.

       yy_lac_establish_ should be invoked when a reduction is about to be
       performed in an inconsistent state (which, for the purposes of LAC,
       includes consistent states that don't know they're consistent because
       their default reductions have been disabled).

       For parse.lac=full, the implementation of yy_lac_establish_ is as
       follows.  If no initial context is currently established for the
       current lookahead, then check if that lookahead can eventually be
       shifted if syntactic actions continue from the current context.  */
    if (yy_lac_established_)
      return true;
    else
      {
#if YYDEBUG
        YYCDEBUG << "LAC: initial context established for "
                 << symbol_name (yytoken) << '\n';
#endif
        yy_lac_established_ = true;
        return yy_lac_check_ (yytoken);
      }
  }

  // Discard any previous initial lookahead context.
  void
  parser::yy_lac_discard_ (const char* event)
  {
   /* Discard any previous initial lookahead context because of Event,
      which may be a lookahead change or an invalidation of the currently
      established initial context for the current lookahead.

      The most common example of a lookahead change is a shift.  An example
      of both cases is syntax error recovery.  That is, a syntax error
      occurs when the lookahead is syntactically erroneous for the
      currently established initial context, so error recovery manipulates
      the parser stacks to try to find a new initial context in which the
      current lookahead is syntactically acceptable.  If it fails to find
      such a context, it discards the lookahead.  */
    if (yy_lac_established_)
      {
        YYCDEBUG << "LAC: initial context discarded due to "
                 << event << '\n';
        yy_lac_established_ = false;
      }
  }




  const short parser::yypact_ninf_ = -224;

  const short parser::yytable_ninf_ = -288;

  const short
  parser::yypact_[] =
  {
     266,    35,   743,  1256,    77,   123,  -224,  -224,   326,    12,
    -224,  -224,  -224,    79,   116,  -224,  -224,    10,  1921,   861,
    2472,   156,   128,  -224,  -224,  -224,  -224,  -224,  -224,  -224,
    -224,  -224,  -224,  -224,  -224,  -224,  -224,   136,  -224,   177,
     196,  -224,  -224,  -224,  -224,  -224,  -224,   264,    -1,  2472,
    -224,   182,  -224,  -224,  -224,  -224,  -224,  -224,  -224,  -224,
     162,   400,  -224,  1719,   178,  -224,  -224,  -224,  -224,    20,
     139,    24,  -224,  -224,  -224,  -224,   185,   160,  -224,   166,
    -224,  -224,  -224,   925,  -224,  2472,   244,  -224,   400,  -224,
     239,   228,  -224,  -224,  1972,  1972,   250,  -224,  -224,  -224,
     258,   238,   269,  2472,  2472,   984,  2472,   400,   281,   227,
     267,  -224,  2472,  2022,  -224,  2472,   264,  -224,  -224,  -224,
    -224,    -1,  -224,  -224,   288,   328,   154,  1307,   290,  -224,
     293,   400,  1921,   128,  -224,   259,  1921,  1357,  -224,   114,
    -224,   114,   231,  -224,   300,    29,    32,  -224,   128,   164,
      41,  -224,   184,  -224,  -224,  -224,   128,  -224,   198,  2518,
    -224,  -224,  1411,  1099,  -224,  -224,  -224,   174,   253,  2472,
     244,  1719,   314,  1461,  2472,   114,   252,   275,  2472,  2472,
    -224,  -224,  -224,   318,   400,  1770,  2472,   324,  -224,   172,
    -224,  -224,  -224,  -224,  -224,    16,   319,   320,  -224,  2472,
    -224,  2472,  2072,   323,   329,  2472,    75,  -224,   151,  2472,
    -224,  2472,  2122,  -224,   244,  1719,  2172,   330,   267,  -224,
    2472,   267,   114,  2472,  2472,  -224,  -224,   325,   331,  -224,
    -224,  -224,   114,   177,  2472,  2472,  2472,  2472,   232,   128,
     128,   333,  2472,  2472,    17,  -224,   128,    99,  -224,  2172,
     335,   111,  -224,  -224,   244,   285,  2472,    36,  1150,  1512,
     114,    48,    37,  -224,   284,   346,   244,  -224,   114,   338,
     400,   128,  -224,   128,  -224,   341,   400,  1719,   342,  2472,
    -224,   400,  -224,   205,   348,  -224,  -224,   172,   349,  -224,
    -224,   194,   194,   194,  -224,   343,   172,  1820,  1820,   400,
    -224,  2472,   306,  -224,  -224,   347,  2472,  2222,  1563,   114,
    -224,   316,  -224,   350,   351,   244,  2472,   328,  -224,  -224,
     355,   400,   356,  -224,  -224,  -224,  1870,  -224,  -224,  -224,
     357,   400,   359,   400,  -224,   128,    50,   296,  -224,   361,
     400,   128,  -224,    22,  -224,  2557,   362,  2172,  -224,   360,
    2472,   365,   345,  2272,   352,  2472,    65,   114,  -224,  1614,
    2472,   114,  2472,  -224,  -224,  -224,   364,   114,    21,  -224,
    -224,  -224,   244,   373,   400,  -224,  1043,  -224,  -224,  -224,
    -224,  -224,  -224,   366,   177,   366,   366,    16,  -224,   369,
     128,   147,  -224,   380,  -224,    18,   380,  -224,  2472,  -224,
     358,  -224,   384,   114,   387,  2322,  -224,  -224,   388,  -224,
    -224,  -224,  -224,  -224,   393,  2472,  2472,  -224,   128,  -224,
     353,   128,  -224,  2172,  2596,  -224,  -224,   395,  -224,   400,
     367,  2472,   368,   398,  2472,   399,   370,  2372,  -224,  -224,
     401,   400,  -224,   407,  -224,  2472,  -224,   410,  -224,  -224,
    1204,   194,  -224,  -224,  -224,    70,  2472,  2472,  1820,   402,
    2472,   363,   417,  -224,  2422,  -224,   114,   114,  -224,   415,
    -224,  -224,   419,   400,   416,  -224,   372,   424,  2172,  -224,
    -224,  2472,   400,  2472,   385,   400,   389,  2472,   390,   427,
    -224,  -224,   400,   437,  -224,   431,   244,  -224,  2472,  2472,
    -224,  -224,  -224,  -224,  -224,  2472,  -224,  -224,   435,  -224,
    -224,  -224,  -224,  -224,  -224,   438,   400,   400,  2472,  2472,
     400,  2472,   396,  -224,  1668,   444,   453,  -224,  -224,  -224,
    -224,   400,   400,   400,  2472,  -224,   244,  -224,  -224,   400,
     459,  -224
  };

  const short
  parser::yydefact_[] =
  {
       7,    32,     0,   287,     0,     0,     4,     5,     0,     0,
      25,    27,    28,     0,    30,     3,    21,    59,   228,     0,
       0,     0,     0,   246,   263,   264,   265,   267,   268,   269,
     270,   273,   274,   259,   260,   261,   255,   147,   129,     0,
       0,   256,   257,   258,   281,    60,   247,     0,     0,     0,
      20,   200,   227,   130,   141,   142,   136,   140,   137,   138,
     139,    15,   127,     0,   287,   243,   245,   244,   126,   128,
     252,     0,   226,   225,   224,   220,   221,   222,   271,   277,
     275,   278,   223,     0,   288,     0,   287,    69,    74,    75,
       0,     0,     1,    24,     0,   288,     0,     6,    26,    34,
       0,     0,     0,     0,     0,     0,     0,   158,   287,   229,
     289,   132,     0,     0,    79,    88,    83,    81,    82,    84,
      85,    86,    80,    87,     0,   158,     0,     0,     0,   238,
     289,    16,   228,     0,   248,   133,   228,     0,    65,   101,
      66,    63,     0,   217,   219,   212,     0,   188,     0,   289,
     172,   282,     0,   266,   272,   276,     0,   194,   289,   179,
     148,   201,     0,     0,    88,    83,    86,     0,    90,     0,
     287,   288,     0,     0,     0,    96,     0,     0,     0,     0,
     262,   280,   279,   287,    73,   288,     0,     0,    12,    57,
      23,    22,     2,    33,    31,    44,     0,     0,   231,   288,
     160,     0,   290,     0,     0,     0,     0,   236,     0,     0,
     131,     0,     0,    18,   287,     0,   290,     0,   289,   249,
       0,   289,    63,     0,     0,   204,   205,   289,     0,    67,
      68,   102,    64,     0,     0,     0,     0,     0,     0,   290,
       0,     0,     0,     0,     0,   290,     0,     0,    59,   237,
       0,     0,   178,    62,   287,   135,     0,     0,     0,     0,
      63,   209,     0,    89,   145,     0,   287,    10,     0,     0,
     149,     0,   250,     0,   253,     0,   152,   288,     0,     0,
      70,    71,    11,    84,     0,    53,    55,    58,     0,    47,
      46,   283,   283,   283,    42,   287,    57,     0,     0,   159,
     230,     0,   232,   241,   120,     0,     0,     0,     0,    93,
     235,     0,   116,     0,     0,   287,     0,     0,   239,   240,
       0,   150,     0,   103,   202,   203,   290,   207,   208,   218,
       0,   213,     0,   215,   192,     0,   175,     0,   189,     0,
     173,     0,   198,     0,   195,   184,     0,   237,   180,     0,
       0,     0,   134,     0,   144,     0,     0,    63,    98,     0,
       0,    63,     0,    76,    77,     8,     0,    95,    63,   251,
     254,   153,   287,     0,    72,    54,     0,    56,    35,   284,
     285,   286,    37,    40,     0,    39,    41,   288,    45,     0,
       0,     0,   169,   289,   163,   128,   289,   234,     0,   124,
       0,   125,     0,     0,     0,     0,   117,    17,     0,   242,
     100,   206,   214,   216,     0,     0,     0,   190,     0,   174,
       0,     0,   196,   237,     0,   183,   181,     0,    61,   155,
     112,     0,   111,     0,     0,     0,   143,     0,    99,   210,
       0,   146,    97,     0,     9,     0,    94,     0,    14,    52,
       0,   283,    36,    43,    29,     0,     0,     0,   290,     0,
       0,     0,     0,   233,     0,   121,    92,    63,   119,     0,
      19,   193,     0,   176,     0,   199,     0,     0,   237,   185,
     182,     0,   154,     0,   110,   157,   105,     0,   106,     0,
     211,    78,   151,     0,    49,   287,   287,    38,     0,     0,
     171,   170,   164,   162,   165,     0,   161,   123,     0,    91,
     118,   177,   191,   197,   186,     0,   115,   114,     0,     0,
     156,     0,   104,    13,     0,     0,     0,   167,   166,   122,
     187,   113,   109,   108,     0,    51,   287,    48,   168,   107,
       0,    50
  };

  const short
  parser::yypgoto_[] =
  {
    -224,  -224,  -224,  -224,   -64,  -224,  -224,   456,  -224,  -224,
    -224,  -224,    15,    27,    80,  -224,  -224,  -224,   186,   181,
    -224,   176,  -224,   339,  -216,   -34,   -61,   289,  -224,  -174,
     -10,  -224,  -224,  -224,  -224,  -224,  -224,  -224,  -224,  -224,
      -2,  -224,   189,   469,   175,    26,  -224,  -224,   102,   101,
     149,  -224,  -150,   315,  -224,  -223,  -224,    19,  -224,    49,
     272,  -224,  -224,  -224,  -154,   294,  -224,   -35,  -224,  -224,
    -224,  -224,  -224,   -40,   -31,  -224,   -28,  -224,  -224,  -224,
       3,  -122
  };

  const short
  parser::yydefgoto_[] =
  {
       0,     5,     6,     7,    50,     8,     9,    10,    11,   102,
      14,    12,   382,   383,   294,   295,   296,   284,   285,   286,
     287,   288,    51,    52,   231,   232,    86,    87,   263,   123,
     168,   169,    53,    54,    55,    56,    57,    58,    59,    60,
     107,   108,   225,    62,   393,   394,   149,   158,   161,   162,
     226,   227,   228,   141,   361,   143,   144,    89,    65,   110,
     129,   130,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,   384,
      90,   203
  };

  const short
  parser::yytable_[] =
  {
      61,    88,   170,   139,   146,   252,   323,   153,   217,   124,
     329,    96,    97,   257,    23,   289,   154,   125,   131,   155,
     103,    64,    17,   269,    17,    17,   173,   241,   173,   341,
     190,   191,   178,    23,   421,   175,   247,   234,   128,   156,
     236,    13,     1,   352,   358,   362,    46,   160,   291,   242,
     292,   293,    23,    24,    25,    26,   359,   157,   415,   104,
      45,    88,    45,    45,   460,    46,   214,   172,   174,     4,
     174,   445,   436,   124,   179,   342,   153,   353,   498,   235,
     422,   125,   237,   184,    46,   154,    99,   306,   155,   187,
     152,   243,    61,    61,    23,   124,   320,   348,   360,   322,
     416,   254,   183,   125,    63,   327,   437,    91,   356,   289,
     266,   200,   153,   289,    23,   248,   307,    18,    17,   347,
     499,   127,   289,    92,   250,    88,    46,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      17,   438,   136,    23,   137,   442,    46,    41,    42,    43,
      44,   176,   446,    45,   315,    17,    45,   344,   404,   308,
      88,   452,   132,   210,    47,   101,   211,   264,   163,    88,
     133,    23,   270,   265,   309,    46,   239,   276,    45,   177,
     258,   218,   259,    88,   281,   221,   278,    23,   171,    23,
     142,   425,    23,    45,   456,   212,   457,   299,   146,   181,
      24,    25,    26,    46,   240,   182,   147,   109,   126,   440,
     245,    23,   180,    88,   317,    40,   372,   314,   321,    46,
      23,    46,   364,   114,    46,   165,   117,   118,   283,   120,
     166,   122,   189,   331,   367,   333,   148,   152,   246,   188,
      48,   340,   334,    46,   335,   133,    23,   125,   379,   380,
     192,   509,    46,   381,   185,   186,   114,   349,   165,   117,
     118,   119,   120,   166,   122,   193,   271,    23,   194,   366,
     479,   459,   126,   195,   462,    88,   201,   374,    46,   202,
      24,    25,    26,    27,    28,    29,    30,    31,    32,   273,
      23,   199,   196,   197,   126,   198,     1,   208,   388,    46,
     215,   204,   206,   151,   207,   216,   417,   100,   418,   220,
       2,     3,   233,   262,   267,   134,   135,   152,   408,   385,
     386,   109,    46,     4,   282,   109,  -287,    93,   277,    94,
     303,   297,   298,   145,   150,   350,    95,   326,   304,   319,
     328,   132,   159,   338,   167,   125,   365,   368,   429,   146,
     371,   373,   376,   387,   378,   398,   399,   405,   441,   406,
     407,   175,   409,   410,   444,   428,   412,   275,   413,   466,
     419,   426,   430,   448,   454,   447,   138,   209,   451,   114,
     164,   165,   117,   118,   119,   120,   166,   122,   167,   496,
     300,   302,   458,   465,   305,   431,   467,   470,   310,   464,
     311,   313,   434,   471,   480,   484,   486,   503,   138,   505,
     490,   475,   324,   325,   473,   159,   491,   481,   483,   493,
     487,   125,   506,   330,   510,   332,   512,   219,   511,   482,
     513,   339,   485,   514,   522,   518,   134,   523,   346,   519,
     521,    84,   238,   492,   529,   351,   534,   530,    88,   537,
     244,   114,   164,   165,   117,   118,   119,   120,   166,   122,
     167,   261,   538,   536,   541,    98,   497,   453,   377,   375,
     272,   274,   389,   396,   280,   411,   125,   450,   229,   516,
     138,   517,   260,   290,   502,   520,   392,   392,   318,     0,
     397,     0,     0,     0,     0,   400,   402,     0,   524,   525,
       0,     0,     0,     0,     0,   207,   140,     0,     0,     0,
       0,     0,     0,     0,   229,     0,   531,   532,     0,   533,
       0,     0,    88,     0,     0,     0,     0,   145,     0,     0,
       0,     0,   539,   336,   337,     0,   427,     0,   140,   540,
     343,   345,   433,     0,   435,     0,     0,   138,     0,     0,
       0,   443,     0,     0,     0,     0,   363,     0,     0,     0,
       0,   138,     0,     0,     0,   369,     0,   370,     0,     0,
       0,   229,     0,     0,     0,     0,     0,   290,     0,     0,
       0,   290,     0,     0,     0,     0,     0,   463,     0,     0,
     290,   395,   395,     0,   469,     0,     0,     0,     0,   138,
       0,     0,     0,     0,   472,     0,     0,   138,   230,     0,
     140,     0,   477,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   489,     0,     0,   414,
       0,     0,     0,     0,     0,   420,     0,     0,     0,     0,
       0,     0,     0,     0,   230,   500,   501,   392,   229,   504,
       0,     0,     0,   508,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   515,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   140,   145,     0,
       0,     0,     0,     0,   455,     0,     0,   526,   527,   461,
       0,   140,     0,     0,   528,     0,   138,     0,     0,     0,
     138,   230,     0,     0,     0,     0,   229,   138,     0,     0,
       0,     0,   474,     0,     0,   476,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   140,
       0,     0,     0,     0,   138,     0,     0,   140,     0,     0,
       0,     0,   138,    15,    16,     0,     0,    17,     0,    18,
       0,    19,   395,     0,    20,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,    37,    38,    39,    40,     0,   230,    41,
      42,    43,    44,     0,     0,    45,     0,     0,     0,     0,
      46,     0,     0,     0,     0,     0,    47,     0,     0,     0,
       0,    48,     0,     0,    49,   229,   138,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   140,     0,     0,     0,
     140,     0,     0,     0,     0,     0,   230,   140,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   111,     0,   140,    17,     0,    18,     0,   105,
    -237,     0,   140,  -237,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,    39,    40,     0,     0,    41,    42,    43,
      44,     0,   112,    45,     0,     0,     0,     0,    46,     0,
     113,     0,   114,   115,   116,   117,   118,   119,   120,   121,
     122,     0,    49,     0,     0,     0,   111,     0,     0,    17,
       0,    18,     0,   105,  -237,   230,   140,  -237,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,    39,     0,     0,
       0,    41,    42,    43,    44,     0,   112,    45,     0,     0,
       0,     0,    46,     0,   113,     0,   114,   115,   116,   117,
     118,   119,   120,   166,   122,   111,    49,     0,    17,     0,
      18,     0,   105,  -237,     0,     0,  -237,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,     0,     0,     0,     0,
      41,    42,    43,    44,     0,   112,    45,     0,     0,     0,
       0,    46,     0,   113,     0,   114,   115,   116,   117,   118,
     119,   120,   166,   122,   449,    49,     0,  -200,  -200,  -200,
       0,  -200,     0,     0,  -200,     0,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,  -200,  -200,    40,     0,     0,  -200,
    -200,  -200,  -200,     0,     0,  -200,     0,     0,     0,     0,
    -200,     0,     0,     0,     0,     0,  -200,     0,     0,     0,
       0,    48,     0,    17,  -200,    18,   255,   105,     0,     0,
       0,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,     0,    37,
      38,     0,     0,     0,     0,    41,    42,    43,    44,     0,
     256,    45,     0,     0,     0,     0,    46,     0,     0,     0,
       0,   223,    47,     0,    17,   224,    18,   354,   105,     0,
      49,     0,     0,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
      37,    38,     0,     0,     0,     0,    41,    42,    43,    44,
       0,   355,    45,     0,     0,     0,     0,    46,     0,     0,
       0,     0,   223,    47,     0,     0,   224,     0,    17,   494,
      18,    49,   105,     0,     0,    85,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,   495,    37,    38,    39,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,     0,    47,     0,     0,
      17,     0,    18,     0,    83,    49,    84,    85,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,    39,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,    47,
       0,    17,     0,    18,     0,   105,   213,    49,    85,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,    39,
       0,     0,     0,    41,    42,    43,    44,     0,     0,    45,
       0,     0,     0,     0,    46,     0,     0,     0,     0,     0,
      47,    17,     0,    18,     0,   105,   222,     0,    49,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,     0,
       0,     0,     0,    41,    42,    43,    44,     0,     0,    45,
       0,     0,     0,     0,    46,     0,     0,     0,     0,   223,
      47,     0,     0,   224,     0,    17,   253,    18,    49,   105,
       0,     0,    85,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,    39,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,    47,    17,     0,    18,     0,   105,
     268,     0,    49,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,   223,    47,     0,    17,   224,    18,     0,
     105,   357,    49,     0,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,   223,    47,     0,    17,   224,    18,
       0,   105,   403,    49,     0,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,    37,    38,     0,     0,     0,     0,    41,
      42,    43,    44,     0,     0,    45,     0,     0,     0,     0,
      46,     0,     0,     0,     0,   223,    47,     0,    17,   224,
      18,     0,   105,   439,    49,     0,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,     0,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,   223,    47,     0,     0,
     224,     0,    17,   535,    18,    49,   105,     0,     0,    85,
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     0,    37,    38,
      39,     0,     0,     0,    41,    42,    43,    44,     0,     0,
      45,     0,     0,     0,     0,    46,     0,     0,     0,     0,
       0,    47,     0,    17,     0,    18,     0,   105,     0,    49,
      85,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,     0,    37,
      38,    39,     0,     0,     0,    41,    42,    43,    44,     0,
       0,    45,     0,     0,     0,     0,    46,     0,     0,     0,
       0,     0,    47,     0,    17,     0,    18,     0,   105,     0,
      49,   279,     0,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
      37,    38,    39,     0,     0,     0,    41,    42,    43,    44,
       0,     0,    45,     0,     0,     0,     0,    46,     0,     0,
       0,     0,     0,    47,    17,     0,    18,     0,   105,     0,
       0,    49,     0,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
      37,    38,   390,     0,     0,     0,    41,    42,    43,    44,
       0,     0,    45,     0,     0,     0,     0,    46,     0,   391,
       0,     0,     0,    47,    17,     0,    18,     0,   105,     0,
       0,    49,     0,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
      37,    38,     0,     0,     0,     0,    41,    42,    43,    44,
       0,     0,    45,     0,     0,     0,     0,    46,     0,     0,
       0,     0,   223,    47,     0,    17,   224,    18,     0,   105,
       0,    49,     0,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,   106,    47,     0,    17,     0,    18,     0,
      19,     0,    49,    20,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,    47,    17,     0,    18,     0,
     105,     0,     0,    49,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,   205,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,    47,    17,     0,    18,     0,
     105,     0,     0,    49,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,   301,    47,    17,     0,    18,     0,
     105,   312,     0,    49,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,    47,    17,     0,    18,     0,
     105,     0,     0,    49,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,   316,    47,    17,     0,    18,     0,
     105,   401,     0,    49,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,    47,    17,     0,    18,   432,
     105,     0,     0,    49,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,    47,    17,     0,    18,     0,
     105,   468,     0,    49,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,    47,    17,     0,    18,   488,
     105,     0,     0,    49,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,    47,    17,     0,    18,     0,
     105,   507,     0,    49,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,    47,    17,     0,    18,     0,
     105,     0,     0,    49,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,   248,     0,    18,    47,   249,     0,     0,     0,
       0,   250,     0,    49,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     0,     0,     0,
       0,     0,     0,     0,    41,    42,    43,    44,     0,     0,
      45,   248,     0,    18,     0,   423,     0,     0,   251,     0,
     250,    47,     0,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,     0,     0,     0,
       0,     0,     0,    41,    42,    43,    44,     0,     0,    45,
     248,     0,    18,     0,   478,     0,     0,   424,     0,   250,
      47,     0,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,     0,     0,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    47
  };

  const short
  parser::yycheck_[] =
  {
       2,     3,    63,    37,    39,   159,   222,    47,   130,    19,
     233,     8,     0,   163,    15,   189,    47,    19,    20,    47,
      10,     2,     4,   173,     4,     4,     8,   149,     8,    12,
      94,    95,     8,    15,    12,    69,   158,     8,    19,    40,
       8,     6,    30,     7,   260,     8,    47,    49,    32,     8,
      34,    35,    15,    16,    17,    18,     8,    58,     8,    49,
      42,    63,    42,    42,    46,    47,   127,    64,    50,    57,
      50,    50,     7,    83,    50,    58,   116,    41,     8,    50,
      58,    83,    50,    85,    47,   116,     7,    12,   116,    86,
      53,    50,    94,    95,    15,   105,   218,   251,    50,   221,
      50,   162,    83,   105,     2,   227,    41,    30,   258,   283,
     171,   108,   152,   287,    15,     4,    41,     6,     4,     8,
      50,    19,   296,     0,    13,   127,    47,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
       4,   357,     6,    15,     8,   361,    47,    36,    37,    38,
      39,    12,   368,    42,   215,     4,    42,    58,   308,     8,
     162,   384,     6,     9,    53,    49,    12,   169,     6,   171,
      14,    15,   174,   170,   208,    47,    12,   179,    42,    40,
       6,   132,     8,   185,   186,   136,   183,    15,    10,    15,
      13,   345,    15,    42,    47,    41,    49,   199,   233,    39,
      16,    17,    18,    47,    40,    39,    10,    18,    19,   359,
      12,    15,    27,   215,   216,    33,   277,   214,   220,    47,
      15,    47,   262,    51,    47,    53,    54,    55,    56,    57,
      58,    59,     4,   235,   268,   237,    40,    53,    40,     0,
      58,   243,    10,    47,    12,    14,    15,   249,    54,    55,
       0,   467,    47,    59,    10,    11,    51,   254,    53,    54,
      55,    56,    57,    58,    59,     7,    14,    15,    30,   266,
     424,   393,    83,     4,   396,   277,    49,   279,    47,    12,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    14,
      15,    10,   103,   104,   105,   106,    30,     9,   295,    47,
      10,   112,   113,    39,   115,    12,    10,    13,    12,    50,
      44,    45,    12,    60,     0,    21,    22,    53,   315,   292,
     293,   132,    47,    57,     0,   136,     0,     1,    10,     3,
       7,    12,    12,    39,    40,    50,    10,    12,     9,     9,
       9,     6,    48,    10,    60,   347,     0,     9,   350,   384,
       9,     9,     4,    10,     5,    49,     9,    41,   360,     9,
       9,   395,     7,     7,     0,     5,     9,   178,     9,   403,
       9,     9,     7,     0,     5,   372,    37,    49,    12,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,   450,
     201,   202,    12,     9,   205,    50,     9,     9,   209,    41,
     211,   212,    50,    10,     9,     7,     7,     5,    69,    46,
       9,    58,   223,   224,   416,   121,     9,    50,    50,     9,
      50,   423,     5,   234,     9,   236,    10,   133,     9,   431,
      58,   242,   434,     9,     7,    50,   142,     0,   249,    50,
      50,    10,   148,   445,     9,   256,    50,     9,   450,     5,
     156,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,   167,     9,   524,     5,     9,   451,   387,   287,   283,
     176,   177,   296,   298,   185,   326,   478,   376,   139,   481,
     141,   483,   167,   189,   458,   487,   297,   298,   216,    -1,
     301,    -1,    -1,    -1,    -1,   306,   307,    -1,   495,   496,
      -1,    -1,    -1,    -1,    -1,   316,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   175,    -1,   518,   519,    -1,   521,
      -1,    -1,   524,    -1,    -1,    -1,    -1,   233,    -1,    -1,
      -1,    -1,   534,   239,   240,    -1,   347,    -1,    69,   536,
     246,   247,   353,    -1,   355,    -1,    -1,   208,    -1,    -1,
      -1,   362,    -1,    -1,    -1,    -1,   262,    -1,    -1,    -1,
      -1,   222,    -1,    -1,    -1,   271,    -1,   273,    -1,    -1,
      -1,   232,    -1,    -1,    -1,    -1,    -1,   283,    -1,    -1,
      -1,   287,    -1,    -1,    -1,    -1,    -1,   398,    -1,    -1,
     296,   297,   298,    -1,   405,    -1,    -1,    -1,    -1,   260,
      -1,    -1,    -1,    -1,   415,    -1,    -1,   268,   139,    -1,
     141,    -1,   423,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   437,    -1,    -1,   335,
      -1,    -1,    -1,    -1,    -1,   341,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   175,   456,   457,   458,   309,   460,
      -1,    -1,    -1,   464,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   478,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   208,   384,    -1,
      -1,    -1,    -1,    -1,   390,    -1,    -1,   498,   499,   395,
      -1,   222,    -1,    -1,   505,    -1,   357,    -1,    -1,    -1,
     361,   232,    -1,    -1,    -1,    -1,   367,   368,    -1,    -1,
      -1,    -1,   418,    -1,    -1,   421,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   260,
      -1,    -1,    -1,    -1,   395,    -1,    -1,   268,    -1,    -1,
      -1,    -1,   403,     0,     1,    -1,    -1,     4,    -1,     6,
      -1,     8,   458,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    32,    33,    -1,   309,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,
      47,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,    -1,
      -1,    58,    -1,    -1,    61,   466,   467,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   357,    -1,    -1,    -1,
     361,    -1,    -1,    -1,    -1,    -1,   367,   368,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     1,    -1,   395,     4,    -1,     6,    -1,     8,
       9,    -1,   403,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    32,    33,    -1,    -1,    36,    37,    38,
      39,    -1,    41,    42,    -1,    -1,    -1,    -1,    47,    -1,
      49,    -1,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    -1,    61,    -1,    -1,    -1,     1,    -1,    -1,     4,
      -1,     6,    -1,     8,     9,   466,   467,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    32,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    41,    42,    -1,    -1,
      -1,    -1,    47,    -1,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,     1,    61,    -1,     4,    -1,
       6,    -1,     8,     9,    -1,    -1,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    41,    42,    -1,    -1,    -1,
      -1,    47,    -1,    49,    -1,    51,    52,    53,    54,    55,
      56,    57,    58,    59,     1,    61,    -1,     4,     5,     6,
      -1,     8,    -1,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,
      47,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,    -1,
      -1,    58,    -1,     4,    61,     6,     7,     8,    -1,    -1,
      -1,    -1,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,    30,
      31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
      41,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,
      -1,    52,    53,    -1,     4,    56,     6,     7,     8,    -1,
      61,    -1,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    41,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,
      -1,    -1,    52,    53,    -1,    -1,    56,    -1,     4,     5,
       6,    61,     8,    -1,    -1,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    47,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,
       4,    -1,     6,    -1,     8,    61,    10,    11,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    32,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,
      -1,     4,    -1,     6,    -1,     8,     9,    61,    11,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    32,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,
      53,     4,    -1,     6,    -1,     8,     9,    -1,    61,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    -1,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    52,
      53,    -1,    -1,    56,    -1,     4,     5,     6,    61,     8,
      -1,    -1,    11,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    32,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    53,     4,    -1,     6,    -1,     8,
       9,    -1,    61,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    52,    53,    -1,     4,    56,     6,    -1,
       8,     9,    61,    -1,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    52,    53,    -1,     4,    56,     6,
      -1,     8,     9,    61,    -1,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,
      47,    -1,    -1,    -1,    -1,    52,    53,    -1,     4,    56,
       6,    -1,     8,     9,    61,    -1,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    47,    -1,    -1,    -1,    -1,    52,    53,    -1,    -1,
      56,    -1,     4,     5,     6,    61,     8,    -1,    -1,    11,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      32,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,
      -1,    53,    -1,     4,    -1,     6,    -1,     8,    -1,    61,
      11,    -1,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,    30,
      31,    32,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
      -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,
      -1,    -1,    53,    -1,     4,    -1,     6,    -1,     8,    -1,
      61,    11,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      30,    31,    32,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,
      -1,    -1,    -1,    53,     4,    -1,     6,    -1,     8,    -1,
      -1,    61,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      30,    31,    32,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    49,
      -1,    -1,    -1,    53,     4,    -1,     6,    -1,     8,    -1,
      -1,    61,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,
      -1,    -1,    52,    53,    -1,     4,    56,     6,    -1,     8,
      -1,    61,    -1,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    52,    53,    -1,     4,    -1,     6,    -1,
       8,    -1,    61,    11,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    53,     4,    -1,     6,    -1,
       8,    -1,    -1,    61,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    41,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    53,     4,    -1,     6,    -1,
       8,    -1,    -1,    61,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    52,    53,     4,    -1,     6,    -1,
       8,     9,    -1,    61,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    53,     4,    -1,     6,    -1,
       8,    -1,    -1,    61,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    52,    53,     4,    -1,     6,    -1,
       8,     9,    -1,    61,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    53,     4,    -1,     6,     7,
       8,    -1,    -1,    61,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    53,     4,    -1,     6,    -1,
       8,     9,    -1,    61,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    53,     4,    -1,     6,     7,
       8,    -1,    -1,    61,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    53,     4,    -1,     6,    -1,
       8,     9,    -1,    61,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    53,     4,    -1,     6,    -1,
       8,    -1,    -1,    61,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,     4,    -1,     6,    53,     8,    -1,    -1,    -1,
      -1,    13,    -1,    61,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      42,     4,    -1,     6,    -1,     8,    -1,    -1,    50,    -1,
      13,    53,    -1,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
       4,    -1,     6,    -1,     8,    -1,    -1,    50,    -1,    13,
      53,    -1,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,    30,    44,    45,    57,    64,    65,    66,    68,    69,
      70,    71,    74,     6,    73,     0,     1,     4,     6,     8,
      11,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    30,    31,    32,
      33,    36,    37,    38,    39,    42,    47,    53,    58,    61,
      67,    85,    86,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   106,   111,   120,   121,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,     8,    10,    11,    89,    90,   103,   120,
     143,    30,     0,     1,     3,    10,   143,     0,    70,     7,
     128,    49,    72,    10,    49,     8,    52,   103,   104,   105,
     122,     1,    41,    49,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    92,    93,   103,   105,   111,   120,   123,
     124,   103,     6,    14,   128,   128,     6,     8,    86,    88,
     106,   116,    13,   118,   119,   128,   130,    10,    40,   109,
     128,    39,    53,   136,   137,   139,    40,    58,   110,   128,
     103,   111,   112,     6,    52,    53,    58,    60,    93,    94,
      89,    10,   143,     8,    50,    88,    12,    40,     8,    50,
      27,    39,    39,   120,   103,    10,    11,   143,     0,     4,
      67,    67,     0,     7,    30,     4,   105,   105,   105,    10,
     143,    49,    12,   144,   105,    41,   105,   105,     9,    49,
       9,    12,    41,     9,    89,    10,    12,   144,   122,   128,
      50,   122,     9,    52,    56,   105,   113,   114,   115,    86,
     106,    87,    88,    12,     8,    50,     8,    50,   128,    12,
      40,   144,     8,    50,   128,    12,    40,   144,     4,     8,
      13,    50,   127,     5,    89,     7,    41,   115,     6,     8,
     116,   128,    60,    91,   103,   143,    89,     0,     9,   115,
     103,    14,   128,    14,   128,   105,   103,    10,   143,    11,
      90,   103,     0,    56,    80,    81,    82,    83,    84,    92,
     128,    32,    34,    35,    77,    78,    79,    12,    12,   103,
     105,    52,   105,     7,     9,   105,    12,    41,     8,    88,
     105,   105,     9,   105,   143,    89,    52,   103,   123,     9,
     144,   103,   144,    87,   105,   105,    12,   144,     9,   118,
     105,   103,   105,   103,    10,    12,   128,   128,    10,   105,
     103,    12,    58,   128,    58,   128,   105,     8,   127,   143,
      50,   105,     7,    41,     7,    41,   115,     9,    87,     8,
      50,   117,     8,   128,   136,     0,   143,    88,     9,   128,
     128,     9,    89,     9,   103,    81,     4,    82,     5,    54,
      55,    59,    75,    76,   142,    76,    76,    10,   143,    84,
      32,    49,   105,   107,   108,   128,   107,   105,    49,     9,
     105,     9,   105,     9,   115,    41,     9,     9,   143,     7,
       7,   113,     9,     9,   128,     8,    50,    10,    12,     9,
     128,    12,    58,     8,    50,   127,     9,   105,     5,   103,
       7,    50,     7,   105,    50,   105,     7,    41,    87,     9,
     115,   103,    87,   105,     0,    50,    87,   143,     0,     1,
     112,    12,   118,    77,     5,   128,    47,    49,    12,   144,
      46,   128,   144,   105,    41,     9,    88,     9,     9,   105,
       9,    10,   105,   103,   128,    58,   128,   105,     8,   127,
       9,    50,   103,    50,     7,   103,     7,    50,     7,   105,
       9,     9,   103,     9,     5,    29,    89,    75,     8,    50,
     105,   105,   108,     5,   105,    46,     5,     9,   105,    87,
       9,     9,    10,    58,     9,   105,   103,   103,    50,    50,
     103,    50,     7,     0,   143,   143,   105,   105,   105,     9,
       9,   103,   103,   103,    50,     5,    89,     5,     9,   103,
     143,     5
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    63,    64,    64,    64,    64,    64,    64,    65,    65,
      65,    66,    66,    66,    66,    67,    67,    67,    67,    67,
      68,    68,    68,    68,    68,    69,    69,    70,    70,    71,
      72,    72,    73,    73,    73,    74,    75,    76,    76,    77,
      77,    77,    78,    78,    79,    79,    80,    80,    81,    81,
      81,    81,    81,    82,    82,    83,    83,    84,    84,    85,
      85,    86,    86,    87,    87,    88,    88,    88,    88,    89,
      89,    89,    89,    89,    90,    90,    91,    91,    91,    92,
      92,    92,    92,    92,    92,    92,    92,    93,    93,    94,
      94,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    96,    96,    96,    97,    97,    97,
      98,    98,    98,    99,    99,    99,   100,   100,   100,   100,
     100,   101,   101,   101,   101,   101,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   104,   104,
     105,   106,   106,   107,   107,   108,   108,   108,   108,   108,
     108,   108,   109,   109,   109,   109,   109,   109,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     112,   112,   113,   113,   113,   114,   114,   115,   116,   117,
     117,   117,   118,   118,   118,   118,   118,   119,   119,   120,
     121,   121,   121,   121,   121,   121,   121,   121,   122,   122,
     122,   122,   122,   122,   122,   123,   123,   124,   124,   124,
     125,   126,   126,   127,   127,   127,   128,   128,   129,   129,
     129,   129,   130,   130,   130,   131,   132,   133,   133,   134,
     134,   135,   135,   136,   136,   136,   136,   137,   137,   137,
     137,   138,   138,   139,   139,   140,   140,   141,   141,   141,
     141,   141,   141,   142,   142,   142,   142,   143,   143,   144,
     144
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     3,     2,     1,     1,     2,     0,     5,     6,
       4,     4,     3,     8,     6,     1,     2,     5,     3,     6,
       2,     2,     3,     3,     2,     1,     2,     1,     1,     7,
       0,     2,     0,     3,     2,     5,     2,     1,     3,     2,
       2,     2,     1,     3,     0,     2,     1,     1,     6,     4,
       8,     6,     3,     1,     2,     1,     2,     0,     1,     1,
       1,     5,     3,     0,     1,     1,     1,     2,     2,     1,
       3,     3,     4,     2,     1,     1,     2,     2,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     7,     6,     4,     5,     4,     2,     5,     4,     5,
       5,     2,     3,     4,     7,     6,     6,     9,     8,     8,
       6,     5,     5,     8,     7,     7,     4,     5,     7,     6,
       4,     6,     8,     7,     5,     5,     1,     1,     1,     1,
       1,     3,     2,     2,     4,     3,     1,     1,     1,     1,
       1,     1,     1,     5,     4,     3,     5,     1,     2,     3,
       4,     6,     3,     4,     6,     5,     7,     6,     1,     3,
       2,     7,     7,     1,     3,     3,     4,     4,     5,     1,
       3,     3,     1,     3,     4,     3,     5,     6,     2,     1,
       3,     4,     5,     4,     3,     5,     6,     7,     2,     4,
       5,     7,     4,     6,     2,     4,     5,     7,     4,     6,
       0,     1,     2,     2,     1,     1,     3,     2,     3,     0,
       2,     3,     1,     3,     4,     3,     4,     1,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     1,
       3,     2,     3,     5,     4,     3,     2,     0,     1,     3,
       4,     4,     5,     1,     1,     1,     1,     1,     2,     3,
       3,     4,     1,     3,     4,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     2,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     2,     1,     1,     2,
       2,     1,     2,     0,     1,     1,     1,     0,     1,     0,
       1
  };




#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   237,   237,   238,   239,   240,   241,   242,   250,   253,
     256,   267,   270,   273,   276,   285,   288,   290,   293,   296,
     301,   304,   310,   313,   316,   333,   335,   340,   341,   345,
     350,   351,   355,   356,   357,   361,   370,   375,   377,   382,
     384,   386,   391,   393,   398,   399,   407,   408,   412,   415,
     418,   421,   424,   438,   439,   451,   452,   456,   457,   466,
     467,   471,   473,   478,   479,   483,   484,   485,   486,   490,
     491,   492,   497,   502,   510,   511,   520,   521,   522,   526,
     527,   528,   529,   530,   531,   532,   533,   537,   538,   542,
     544,   549,   552,   555,   559,   562,   565,   568,   571,   574,
     584,   587,   592,   598,   613,   620,   627,   636,   643,   650,
     660,   666,   672,   681,   687,   693,   702,   707,   712,   717,
     722,   730,   735,   741,   746,   751,   764,   767,   770,   773,
     776,   779,   786,   794,   797,   800,   803,   806,   809,   815,
     816,   817,   818,   820,   823,   826,   829,   832,   833,   835,
     837,   840,   843,   846,   852,   855,   858,   861,   869,   870,
     874,   882,   885,   890,   892,   897,   899,   901,   903,   905,
     907,   909,   922,   924,   929,   932,   935,   940,   946,   948,
     950,   952,   954,   956,   958,   960,   962,   964,   969,   970,
     971,   973,   975,   980,   989,   991,   993,   995,   997,  1002,
    1014,  1015,  1020,  1021,  1022,  1026,  1027,  1030,  1032,  1035,
    1036,  1038,  1047,  1049,  1051,  1057,  1059,  1067,  1069,  1073,
    1081,  1082,  1083,  1084,  1085,  1086,  1087,  1088,  1092,  1094,
    1096,  1098,  1100,  1102,  1104,  1112,  1114,  1119,  1121,  1123,
    1128,  1133,  1135,  1144,  1145,  1146,  1154,  1155,  1160,  1162,
    1164,  1166,  1170,  1172,  1174,  1185,  1187,  1190,  1191,  1195,
    1196,  1200,  1201,  1205,  1206,  1207,  1208,  1217,  1218,  1219,
    1220,  1224,  1226,  1231,  1233,  1238,  1240,  1245,  1246,  1247,
    1248,  1249,  1250,  1254,  1255,  1256,  1257,  1269,  1269,  1270,
    1270
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
      55,    56,    57,    58,    59,    60,    61,    62
    };
    // Last valid token kind.
    const int code_max = 317;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 66 "langutils/sc_parser/src/sc_grammar.y"
} } } // sc::ast::parser
#line 3871 "langutils/sc_parser/src/sc_grammar_parser.cpp"

#line 1272 "langutils/sc_parser/src/sc_grammar.y"

