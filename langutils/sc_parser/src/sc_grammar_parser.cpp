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
#line 96 "langutils/sc_parser/src/sc_grammar.y"


#include "sc_parser/details/indexes_typed.hpp"
#include "sc_parser/details/nodes.hpp"

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

#line 68 "langutils/sc_parser/src/sc_grammar.y"
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
#line 243 "langutils/sc_parser/src/sc_grammar.y"
                                     { (yylhs.value.v_GoIndex) = cxt.graph->assign_root((yystack_[2].value.v_RegionListIndex)); }
#line 638 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 3: // go: INTERPRET $end
#line 244 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_GoIndex) = cxt.graph->assign_root(cxt.create(RegionList{}, yylhs.location)); }
#line 644 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 4: // go: file_as_func
#line 245 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_GoIndex) = cxt.graph->assign_root((yystack_[0].value.v_FileAsFunctionIndex)); }
#line 650 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 5: // go: interpreted_function
#line 246 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_GoIndex) = cxt.graph->assign_root((yystack_[0].value.v_FileAsFunctionIndex)); }
#line 656 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 6: // go: classOrExtList.list $end
#line 247 "langutils/sc_parser/src/sc_grammar.y"
                                              { (yylhs.value.v_GoIndex) = cxt.graph->assign_root((yystack_[1].value.v_ClassOrExtensionListIndex)); }
#line 662 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 7: // go: %empty
#line 248 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_GoIndex) = cxt.graph->assign_root(cxt.create(ClassOrExtensionList{}, yylhs.location));}
#line 668 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 8: // file_as_func: INTERPRET argument_declarations block.contents semicolon.opt $end
#line 257 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 674 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 9: // file_as_func: INTERPRET variable_declarations SEMICOLON block.contents semicolon.opt $end
#line 260 "langutils/sc_parser/src/sc_grammar.y"
                {
			cxt.graph->location((yystack_[2].value.v_BlockContentsListIndex)) = range(yystack_[4].location, yystack_[2].location);
			(yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[5].location), cxt.graph->prepend((yystack_[2].value.v_BlockContentsListIndex), (yystack_[4].value.v_DeclareVariableListIndex)));
		}
#line 683 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 10: // file_as_func: INTERPRET variable_declarations semicolon.opt $end
#line 266 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[3].location), cxt.create(BlockContentsList{}, yystack_[2].location, (yystack_[2].value.v_DeclareVariableListIndex))); }
#line 689 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 11: // interpreted_function: INTERPRET_FUNCTION argument_declarations block.contents semicolon.opt $end
#line 277 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 695 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 12: // interpreted_function: INTERPRET_FUNCTION argument_declarations semicolon.opt $end
#line 280 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[2].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, empty_after(yystack_[2].location))); }
#line 701 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 13: // interpreted_function: INTERPRET_FUNCTION OPENPAREN argument_declarations block.contents semicolon.opt CLOSEPAREN $end
#line 283 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[4].value.v_DeclareArgumentListIndex), (yystack_[3].value.v_BlockContentsListIndex)); }
#line 707 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 14: // interpreted_function: INTERPRET_FUNCTION OPENPAREN argument_declarations semicolon.opt CLOSEPAREN $end
#line 286 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, empty_after(yystack_[3].location))); }
#line 713 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 15: // interpreted_function: INTERPRET_FUNCTION block.contents semicolon.opt $end
#line 289 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[3].location), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 719 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 16: // interpreted_function: INTERPRET_FUNCTION semicolon.opt $end
#line 292 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[2].location), cxt.create(BlockContentsList{}, empty_after(yystack_[2].location))); }
#line 725 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 17: // interpreted_function: INTERPRET_FUNCTION OPENPAREN variable_declarations SEMICOLON block.contents semicolon.opt CLOSEPAREN $end
#line 295 "langutils/sc_parser/src/sc_grammar.y"
                {
			cxt.graph->location((yystack_[3].value.v_BlockContentsListIndex)) = range(yystack_[5].location, yystack_[3].location);
			(yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, empty_after(yystack_[6].location)), cxt.graph->prepend((yystack_[3].value.v_BlockContentsListIndex), (yystack_[5].value.v_DeclareVariableListIndex)));
		}
#line 734 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 18: // interpreted_function: INTERPRET_FUNCTION OPENPAREN variable_declarations semicolon.opt CLOSEPAREN $end
#line 301 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, empty_after(yystack_[4].location)), cxt.create(BlockContentsList{}, yystack_[3].location, (yystack_[3].value.v_DeclareVariableListIndex))); }
#line 740 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 19: // region.item: expr
#line 310 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 746 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 20: // region.item: NONLOCALRETURN expr
#line 313 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 752 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 21: // region.item: OPENPAREN argument_declarations block.contents semicolon.opt CLOSEPAREN
#line 315 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 758 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 22: // region.item: OPENPAREN argument_declarations CLOSEPAREN
#line 318 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, empty_after(yystack_[1].location))); }
#line 764 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 23: // region.item: OPENPAREN variable_declarations SEMICOLON block.contents semicolon.opt CLOSEPAREN
#line 321 "langutils/sc_parser/src/sc_grammar.y"
                {
			cxt.graph->location((yystack_[2].value.v_BlockContentsListIndex)) = range(yystack_[4].location, yystack_[2].location);
			(yylhs.value.v_RegionItemIndex) = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, cxt.create(DeclareArgumentList{}, empty_after(yystack_[5].location)), cxt.graph->prepend((yystack_[2].value.v_BlockContentsListIndex), (yystack_[4].value.v_DeclareVariableListIndex)));
		}
#line 773 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 24: // region: INTERPRET region.item
#line 329 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionListIndex) = cxt.create(RegionList{}, yylhs.location, (yystack_[0].value.v_RegionItemIndex)); }
#line 779 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 25: // region: INTERPRET error
#line 332 "langutils/sc_parser/src/sc_grammar.y"
                {
			error_recovery::expr(cxt);
			(yylhs.value.v_RegionListIndex) = cxt.create(RegionList{}, yylhs.location, cxt.create_error(yylhs.location, yystack_[0].location));
		}
#line 788 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 26: // region: region SEMICOLON region.item
#line 338 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionListIndex) = cxt.graph->append((yystack_[2].value.v_RegionListIndex), yylhs.location, (yystack_[0].value.v_RegionItemIndex)); }
#line 794 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 27: // region: region REGION_SEPARATOR region.item
#line 341 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionListIndex) = cxt.graph->append((yystack_[2].value.v_RegionListIndex), yylhs.location, (yystack_[0].value.v_RegionItemIndex)); }
#line 800 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 28: // region: region error
#line 344 "langutils/sc_parser/src/sc_grammar.y"
                {
			// A missing semicolon between lines is inserted by the lexer, see yylex.
			error_recovery::expr(cxt);
			(yylhs.value.v_RegionListIndex) = cxt.graph->append((yystack_[1].value.v_RegionListIndex), cxt.create_error(yystack_[0].location, yystack_[0].location, Index { *(yystack_[1].value.v_RegionListIndex) }));
			yyclearin;
		}
#line 811 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 29: // classOrExtList.list: classOrExtList.item
#line 361 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassOrExtensionListIndex) = cxt.create(ClassOrExtensionList{}, yylhs.location, (yystack_[0].value.v_ClassOrExtensionIndex)); }
#line 817 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 30: // classOrExtList.list: classOrExtList.list classOrExtList.item
#line 363 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassOrExtensionListIndex) = cxt.graph->append((yystack_[1].value.v_ClassOrExtensionListIndex), yylhs.location, (yystack_[0].value.v_ClassOrExtensionIndex)); }
#line 823 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 31: // classOrExtList.item: class
#line 367 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassOrExtensionIndex) = (yystack_[0].value.v_ClassIndex); }
#line 829 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 32: // classOrExtList.item: class.extension
#line 368 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_ClassOrExtensionIndex) = (yystack_[0].value.v_ClassExtensionIndex); }
#line 835 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 33: // classOrExtList.item: error
#line 370 "langutils/sc_parser/src/sc_grammar.y"
                {
			// E.g., code outside a class. Skips to the next class or class extension.
			const auto dropped = error_recovery::class_list_item(cxt, yystack_[0].location);
			if (!dropped)
				YYABORT;
			yylhs.location = *dropped;
			(yylhs.value.v_ClassOrExtensionIndex) = cxt.create_error(*dropped, yystack_[0].location);
			yyclearin;
			yyerrok; // Recovered at the next class, so report its error.
		}
#line 850 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 34: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY members semicolon.opt method.list.opt CLOSECURLY
#line 384 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassIndex) = cxt.create(Class{ cxt.take_version(yystack_[4].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[7].location), (yystack_[6].value.v_maybe_NamedIdentifierIndex), (yystack_[5].value.v_maybe_ClassNameIdentifierIndex), (yystack_[3].value.v_BigMemberListIndex), (yystack_[1].value.v_MethodListIndex)); }
#line 856 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 35: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY method.list.opt CLOSECURLY
#line 387 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto no_members = cxt.create(BigMemberList{}, sc::src::SourceCodeRange { yystack_[2].location.end, yystack_[2].location.end });
			(yylhs.value.v_ClassIndex) = cxt.create(Class{ cxt.take_version(yystack_[2].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[5].location), (yystack_[4].value.v_maybe_NamedIdentifierIndex), (yystack_[3].value.v_maybe_ClassNameIdentifierIndex), no_members, (yystack_[1].value.v_MethodListIndex));
		}
#line 865 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 36: // class.super.opt: %empty
#line 394 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_maybe_ClassNameIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 871 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 37: // class.super.opt: COLON CLASSNAME
#line 395 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_maybe_ClassNameIdentifierIndex) = cxt.create(ClassNameIdentifier{}, yystack_[0].location); }
#line 877 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 38: // class.slot.opt: %empty
#line 399 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_maybe_NamedIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 883 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 39: // class.slot.opt: OPENSQUARE name CLOSESQUARE
#line 400 "langutils/sc_parser/src/sc_grammar.y"
                                      { (yylhs.value.v_maybe_NamedIdentifierIndex) = (yystack_[1].value.v_NamedIdentifierIndex); }
#line 889 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 40: // class.slot.opt: OPENSQUARE CLOSESQUARE
#line 401 "langutils/sc_parser/src/sc_grammar.y"
                                 { (yylhs.value.v_maybe_NamedIdentifierIndex) = cxt.create(Missing{}, empty_after(yystack_[1].location)); }
#line 895 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 41: // class.extension: ADD CLASSNAME OPENCURLY method.list.opt CLOSECURLY
#line 406 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassExtensionIndex) = cxt.create(ClassExtension{ cxt.take_version(yystack_[2].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[3].location), (yystack_[1].value.v_MethodListIndex)); }
#line 901 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 42: // members.entry.item: accessor variable_declarations.list.item
#line 415 "langutils/sc_parser/src/sc_grammar.y"
                {
			// An accessor that isn't written is empty, at the end of the keyword before it, so it moves to the name.
			if (yystack_[1].location.begin == yystack_[1].location.end) {
				yylhs.location = yystack_[0].location;
				cxt.graph->location((yystack_[1].value.v_RWAccessorIndex)) = empty_before(yystack_[0].location);
			}
			(yylhs.value.v_DeclareMemberIndex) = cxt.create(DeclareMember{}, yylhs.location, (yystack_[1].value.v_RWAccessorIndex), (yystack_[0].value.v_DeclareAnyVariableIndex));
		}
#line 914 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 43: // members.entry.list: members.entry.item
#line 427 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberListIndex) = cxt.create(DeclareMemberList{}, yylhs.location, (yystack_[0].value.v_DeclareMemberIndex)); }
#line 920 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 44: // members.entry.list: members.entry.list COMMA members.entry.item
#line 429 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberListIndex) = cxt.graph->append((yystack_[2].value.v_DeclareMemberListIndex), yylhs.location, (yystack_[0].value.v_DeclareMemberIndex)); }
#line 926 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 45: // members.entry: CLASSVAR members.entry.list
#line 434 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberEntryIndex) = cxt.graph->cast<DeclareClassMemberList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 932 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 46: // members.entry: VAR members.entry.list
#line 436 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberEntryIndex) = cxt.graph->cast<DeclareMemberList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 938 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 47: // members.entry: CONST members.entry.list
#line 438 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberEntryIndex) = cxt.graph->cast<DeclareConstList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 944 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 48: // members.entry: CLASSVAR error
#line 440 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips to the declaration's `;`, or the next method.
			const auto error = error_recovery::member(cxt, yystack_[1].location, yystack_[0].location);
			if (!error)
				YYABORT;
			(yylhs.value.v_BigMemberEntryIndex) = *error;
			yyclearin;
			yyerrok; // Recovered at the next item, so report its error.
		}
#line 958 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 49: // members.entry: VAR error
#line 450 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips to the declaration's `;`, or the next method.
			const auto error = error_recovery::member(cxt, yystack_[1].location, yystack_[0].location);
			if (!error)
				YYABORT;
			(yylhs.value.v_BigMemberEntryIndex) = *error;
			yyclearin;
			yyerrok; // Recovered at the next item, so report its error.
		}
#line 972 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 50: // members.entry: CONST error
#line 460 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips to the declaration's `;`, or the next method.
			const auto error = error_recovery::member(cxt, yystack_[1].location, yystack_[0].location);
			if (!error)
				YYABORT;
			(yylhs.value.v_BigMemberEntryIndex) = *error;
			yyclearin;
			yyerrok; // Recovered at the next item, so report its error.
		}
#line 986 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 51: // members: members.entry
#line 473 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberListIndex) = cxt.create(BigMemberList{}, yylhs.location, (yystack_[0].value.v_BigMemberEntryIndex));}
#line 992 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 52: // members: members SEMICOLON members.entry
#line 475 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberListIndex) = cxt.graph->append((yystack_[2].value.v_BigMemberListIndex), yylhs.location, (yystack_[0].value.v_BigMemberEntryIndex)); }
#line 998 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 53: // method.name: name
#line 484 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 1004 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 54: // method.name: binary_op.raw
#line 485 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 1010 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 55: // method.base: method.name OPENCURLY argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 490 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(Missing{}, empty_after(yystack_[3].location)), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 1016 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 56: // method.base: method.name OPENCURLY argument_declarations.opt CLOSECURLY
#line 493 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[3].value.v_SelectorIndex), (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(Missing{}, empty_after(yystack_[1].location)), cxt.create(BlockContentsList{}, empty_after(yystack_[1].location))); }
#line 1022 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 57: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt block.contents semicolon.opt CLOSECURLY
#line 496 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[7].value.v_SelectorIndex), (yystack_[5].value.v_DeclareArgumentListIndex), cxt.create(PrimitiveIdentifier{}, yystack_[4].location), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 1028 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 58: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt CLOSECURLY
#line 499 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(PrimitiveIdentifier{}, yystack_[2].location), cxt.create(BlockContentsList{}, empty_after(yystack_[1].location))); }
#line 1034 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 59: // method: method.base
#line 504 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_AnyMethodIndex) = (yystack_[0].value.v_AnyMethodIndex); }
#line 1040 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 60: // method: error
#line 506 "langutils/sc_parser/src/sc_grammar.y"
                {
			// E.g., a method name with no `{`, or a stray `;`. Skips to the next method, or the class's closing curly.
			const auto dropped = error_recovery::class_item(cxt, yystack_[0].location);
			if (!dropped)
				YYABORT;
			yylhs.location = *dropped;
			(yylhs.value.v_AnyMethodIndex) = cxt.create_error(*dropped, yystack_[0].location);
			yyclearin;
			yyerrok; // Recovered at the next item, so report its error.
		}
#line 1055 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 61: // method: MULTIPLY method.base
#line 517 "langutils/sc_parser/src/sc_grammar.y"
                { 
			if (auto c = cxt.graph->as_a<MethodIndex>((yystack_[0].value.v_AnyMethodIndex))) {
				(yylhs.value.v_AnyMethodIndex) = cxt.graph->cast<ClassMethod>(*c);
			} 
			// leaves as error
			cxt.graph->location((yystack_[0].value.v_AnyMethodIndex)) = yylhs.location; 
			(yylhs.value.v_AnyMethodIndex) = (yystack_[0].value.v_AnyMethodIndex); 
		}
#line 1068 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 62: // method.list: method
#line 528 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_MethodListIndex) = cxt.create(MethodList{}, yylhs.location, (yystack_[0].value.v_AnyMethodIndex)); }
#line 1074 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 63: // method.list: method.list method
#line 529 "langutils/sc_parser/src/sc_grammar.y"
                             { (yylhs.value.v_MethodListIndex) = cxt.graph->append((yystack_[1].value.v_MethodListIndex), yylhs.location, (yystack_[0].value.v_AnyMethodIndex)); }
#line 1080 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 64: // method.list.opt: %empty
#line 533 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_MethodListIndex) = cxt.create(MethodList{}, yylhs.location); }
#line 1086 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 65: // method.list.opt: method.list
#line 534 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_MethodListIndex) = (yystack_[0].value.v_MethodListIndex); }
#line 1092 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 66: // block.open: OPENCURLY
#line 543 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_bool) = false; }
#line 1098 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 67: // block.open: BEGINCLOSEDFUNC
#line 544 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_bool) = true; }
#line 1104 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 68: // block: block.open argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 549 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockIndex) = cxt.create(BlockNode{ (yystack_[4].value.v_bool) }, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 1110 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 69: // block: block.open argument_declarations.opt CLOSECURLY
#line 551 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockIndex) = cxt.create(BlockNode{ (yystack_[2].value.v_bool) }, yylhs.location, (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, empty_after(yystack_[1].location))); }
#line 1116 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 70: // block.opt_list: %empty
#line 555 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location); }
#line 1122 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 71: // block.opt_list: block.list
#line 556 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_BlockListIndex) = (yystack_[0].value.v_BlockListIndex); }
#line 1128 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 72: // block.list: block
#line 560 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location, (yystack_[0].value.v_BlockIndex)); }
#line 1134 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 73: // block.list: generator
#line 561 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location, (yystack_[0].value.v_GeneratorAnyIndex)); }
#line 1140 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 74: // block.list: block.list block
#line 562 "langutils/sc_parser/src/sc_grammar.y"
                           { (yylhs.value.v_BlockListIndex) = cxt.graph->append((yystack_[1].value.v_BlockListIndex), yylhs.location, (yystack_[0].value.v_BlockIndex)); }
#line 1146 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 75: // block.list: block.list generator
#line 563 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_BlockListIndex) = cxt.graph->append((yystack_[1].value.v_BlockListIndex), yylhs.location, (yystack_[0].value.v_GeneratorAnyIndex)); }
#line 1152 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 76: // block.contents: block.contents.item
#line 567 "langutils/sc_parser/src/sc_grammar.y"
                              { (yylhs.value.v_BlockContentsListIndex) = cxt.create(BlockContentsList{}, yylhs.location, (yystack_[0].value.v_BlockItemIndex)); }
#line 1158 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 77: // block.contents: block.contents SEMICOLON block.contents.item
#line 568 "langutils/sc_parser/src/sc_grammar.y"
                                                       { (yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[2].value.v_BlockContentsListIndex), yylhs.location, (yystack_[0].value.v_BlockItemIndex)); }
#line 1164 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 78: // block.contents: block.contents NONLOCALRETURN expr
#line 570 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[2].value.v_BlockContentsListIndex), yylhs.location, non_local); 
		}
#line 1173 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 79: // block.contents: block.contents SEMICOLON NONLOCALRETURN expr
#line 575 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[3].value.v_BlockContentsListIndex), yylhs.location, non_local); 
		}
#line 1182 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 80: // block.contents: NONLOCALRETURN expr
#line 580 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.create(BlockContentsList{}, yylhs.location, non_local);
		}
#line 1191 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 81: // block.contents.item: expr
#line 587 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_BlockItemIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1197 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 82: // block.contents.item: variable_declarations
#line 588 "langutils/sc_parser/src/sc_grammar.y"
                                { (yylhs.value.v_BlockItemIndex) = (yystack_[0].value.v_DeclareVariableListIndex); }
#line 1203 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 83: // block.contents.item: error
#line 590 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips to the statement's `;`, or the bracket closing the block, so the statements around it are kept.
			const auto dropped = error_recovery::statement(cxt, yystack_[0].location);
			if (!dropped)
				YYABORT;
			yylhs.location = *dropped;
			(yylhs.value.v_BlockItemIndex) = cxt.create_error(*dropped, yystack_[0].location);
			yyclearin;
			yyerrok; // Recovered at the next statement, so report its error.
		}
#line 1218 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 84: // adverb: DOT name
#line 608 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_AdverbIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 1224 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 85: // adverb: DOT integer
#line 609 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_AdverbIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 1230 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 86: // adverb: DOT OPENPAREN expr_or_seq CLOSEPAREN
#line 610 "langutils/sc_parser/src/sc_grammar.y"
                                               { (yylhs.value.v_AdverbIndex) = cxt.create(AdverbExprNode{}, yylhs.location, (yystack_[1].value.v_AnyExprIndex));  }
#line 1236 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 87: // binary_op.raw: BINOP
#line 614 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1242 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 88: // binary_op.raw: READWRITEVAR
#line 615 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1248 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 89: // binary_op.raw: LESSTHAN
#line 616 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1254 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 90: // binary_op.raw: GREATERTHAN
#line 617 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1260 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 91: // binary_op.raw: MINUS
#line 618 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1266 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 92: // binary_op.raw: MULTIPLY
#line 619 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1272 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 93: // binary_op.raw: ADD
#line 620 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1278 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 94: // binary_op.raw: PIPE
#line 621 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1284 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 95: // binary_op.no_adverb: binary_op.raw
#line 625 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 1290 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 96: // binary_op.no_adverb: KEYBINOP
#line 626 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ true }, yylhs.location); }
#line 1296 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 97: // binary_op: binary_op.no_adverb adverb
#line 631 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorMaybeAdverbIndex) = cxt.create(SelectorWAdverb{}, yylhs.location, (yystack_[1].value.v_SelectorIndex), (yystack_[0].value.v_AdverbIndex)); }
#line 1302 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 98: // binary_op: binary_op.no_adverb
#line 633 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorMaybeAdverbIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 1308 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 99: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN arguments CLOSEPAREN block.opt_list
#line 638 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[2].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1314 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 100: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN CLOSEPAREN block.list
#line 641 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[4].value.v_SelectorIndex), cxt.create(ArgumentList{}, empty_after(yystack_[2].location)), (yystack_[0].value.v_BlockListIndex)); }
#line 1320 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 101: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN block.list
#line 644 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[2].value.v_SelectorIndex), cxt.create(ArgumentList{}, empty_after(yystack_[1].location)), (yystack_[0].value.v_BlockListIndex)); }
#line 1326 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 102: // msgsend: name OPENPAREN arguments CLOSEPAREN block.opt_list
#line 648 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[2].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1332 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 103: // msgsend: name OPENPAREN CLOSEPAREN block.list
#line 651 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), cxt.create(ArgumentList{}, empty_after(yystack_[2].location)), (yystack_[0].value.v_BlockListIndex)); }
#line 1338 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 104: // msgsend: name block.list
#line 654 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[1].value.v_NamedIdentifierIndex), cxt.create(ArgumentList{}, empty_after(yystack_[1].location)), (yystack_[0].value.v_BlockListIndex)); }
#line 1344 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 105: // msgsend: expr DOT name arguments.maybe_paren block.opt_list
#line 657 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PostfixMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1350 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 106: // msgsend: expr DOT arguments.paren block.opt_list
#line 660 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(ValueMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1356 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 107: // msgsend: expr DOT OPENPAREN CLOSEPAREN block.opt_list
#line 663 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(ValueMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), cxt.create(ArgumentList{}, empty_after(yystack_[2].location)), (yystack_[0].value.v_BlockListIndex)); }
#line 1362 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 108: // msgsend: CLASSNAME OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 673 "langutils/sc_parser/src/sc_grammar.y"
                {
			// The array includes its brackets, as every other array does, see literal.array.
			cxt.graph->location(*(yystack_[2].value.v_ArrayIndex)) = range(yystack_[3].location, yystack_[0].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(CollectionNode{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[4].location), (yystack_[2].value.v_ArrayIndex));
		}
#line 1372 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 109: // msgsend: CLASSNAME block.list
#line 680 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[1].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, empty_after(yystack_[1].location)), (yystack_[0].value.v_BlockListIndex));
		}
#line 1381 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 110: // msgsend: CLASSNAME arguments.paren block.opt_list
#line 685 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[2].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex));
		}
#line 1390 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 111: // msgsend: CLASSNAME OPENPAREN CLOSEPAREN block.opt_list
#line 691 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[3].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, empty_after(yystack_[2].location)), (yystack_[0].value.v_BlockListIndex));
		}
#line 1399 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 112: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 706 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[3].value.v_ArgumentListIndex), (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r);
		}
#line 1409 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 113: // expr.dotrange: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 713 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, empty_after(yystack_[3].location)),  (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1419 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 114: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 720 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[2].value.v_ArgumentListIndex), cxt.create(Missing{}, empty_after(yystack_[1].location)));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1429 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 115: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 729 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[5].value.v_ArgumentListIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[8].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1439 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 116: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 736 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[4].value.v_ArgumentListIndex), cxt.create(Missing{}, empty_after(yystack_[3].location)));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1449 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 117: // expr.dotrange.assign: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 743 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, empty_after(yystack_[5].location)),  (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1459 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 118: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 753 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[3].value.v_ArgumentListIndex), (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1469 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 119: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 759 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[2].value.v_ArgumentListIndex), cxt.create(Missing{}, empty_after(yystack_[1].location)));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), r);	
		}
#line 1479 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 120: // expr.base.range: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 765 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, empty_after(yystack_[3].location)),  (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), r);	
		}
#line 1489 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 121: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 774 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[5].value.v_ArgumentListIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1499 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 122: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 780 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[4].value.v_ArgumentListIndex), cxt.create(Missing{}, empty_after(yystack_[3].location)));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1509 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 123: // expr.base.range.assign: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 786 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, empty_after(yystack_[5].location)),  (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1519 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 124: // range: OPENPAREN expr_or_seq DOTDOT CLOSEPAREN
#line 795 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, cxt.create(Missing{}, empty_after(yystack_[1].location)));
		}
#line 1528 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 125: // range: OPENPAREN expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 800 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[4].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1537 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 126: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 805 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[6].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1546 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 127: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 810 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[4].location, yystack_[2].location), (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[5].location, yystack_[0].location), args, cxt.create(Missing{}, empty_after(yystack_[1].location)));
		}
#line 1555 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 128: // range: OPENPAREN DOTDOT expr_or_seq CLOSEPAREN
#line 815 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, empty_after(yystack_[3].location));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1564 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 129: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 823 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[5].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1573 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 130: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 828 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[7].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1582 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 131: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 834 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[4].location, yystack_[2].location), (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[6].location, yystack_[0].location), args, cxt.create(Missing{}, empty_after(yystack_[1].location)));
		}
#line 1591 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 132: // range.generator: OPENPAREN COLON DOTDOT expr_or_seq CLOSEPAREN
#line 839 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, empty_after(yystack_[3].location));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1600 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 133: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT CLOSEPAREN
#line 844 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, cxt.create(Missing{}, empty_after(yystack_[1].location)));
		}
#line 1609 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 134: // expr.base: literal
#line 857 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyPossiblyLiteralIndex); }
#line 1615 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 135: // expr.base: generator
#line 860 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_GeneratorAnyIndex); }
#line 1621 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 136: // expr.base: name
#line 863 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 1627 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 137: // expr.base: CURRYARG
#line 866 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyExprIndex) = cxt.create(CurryNode{}, yystack_[0].location); }
#line 1633 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 138: // expr.base: msgsend
#line 869 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1639 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 139: // expr.base: OPENPAREN expr_or_seq CLOSEPAREN
#line 872 "langutils/sc_parser/src/sc_grammar.y"
                { 
			// A nasty hack because of nil assignment.
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AnyExprIndex) = (yystack_[1].value.v_AnyExprIndex);
		}
#line 1649 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 140: // expr.base: OPENPAREN error
#line 879 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto close = error_recovery::bracket(cxt, yystack_[1].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create_error(range(yystack_[1].location, close), yystack_[0].location);
			yyclearin; // The lookahead was skipped.
			yyerrok; // Recovered at the matching bracket, so report the next error.
		}
#line 1660 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 141: // expr.base: TILDE name
#line 887 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(EnvIdentifierNode{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1666 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 142: // expr.base: expr.base OPENSQUARE arguments CLOSESQUARE
#line 890 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex) ); }
#line 1672 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 143: // expr.base: expr.base OPENSQUARE CLOSESQUARE
#line 893 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), cxt.create(ArgumentList{}, empty_after(yystack_[1].location))); }
#line 1678 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 144: // expr.base: expr.base.range
#line 896 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AtMessageIndex); }
#line 1684 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 145: // expr.base: range
#line 899 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeIndex); }
#line 1690 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 146: // expr.base: range.generator
#line 902 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeGeneratorIndex); }
#line 1696 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 147: // expr: expr.base
#line 907 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1702 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 148: // expr: expr.base.range.assign
#line 908 "langutils/sc_parser/src/sc_grammar.y"
                                 { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeAssignIndex); }
#line 1708 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 149: // expr: expr.dotrange
#line 909 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AtMessageIndex); }
#line 1714 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 150: // expr: expr.dotrange.assign
#line 910 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeAssignIndex); }
#line 1720 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 151: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE
#line 913 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex) ); }
#line 1726 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 152: // expr: expr DOT OPENSQUARE CLOSESQUARE
#line 916 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), cxt.create(ArgumentList{}, empty_after(yystack_[1].location)) ); }
#line 1732 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 153: // expr: expr binary_op expr
#line 919 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(BinaryMessage{}, yylhs.location, (yystack_[1].value.v_SelectorMaybeAdverbIndex), (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1738 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 154: // expr: expr DOT name EQUALSSIGN expr
#line 922 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(SetterNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1744 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 155: // expr: CLASSNAME
#line 924 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_AnyExprIndex) = cxt.create(ClassNameIdentifier{}, yylhs.location); }
#line 1750 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 156: // expr: BACKTICK expr
#line 925 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyExprIndex) = cxt.create(ReferenceNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1756 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 157: // expr: name EQUALSSIGN expr
#line 927 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentNode{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1762 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 158: // expr: TILDE name EQUALSSIGN expr
#line 930 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentNode{AssignmentNode::Target::Environment}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1768 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 159: // expr: name OPENPAREN arguments CLOSEPAREN EQUALSSIGN expr
#line 933 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(SetterNode{}, yylhs.location, (yystack_[3].value.v_ArgumentListIndex), (yystack_[5].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1774 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 160: // expr: name.pack EQUALSSIGN expr
#line 936 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[2].value.v_NamePackIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1780 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 161: // expr: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 939 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[3].value.v_NamePackIndex), (yystack_[1].value.v_AnyExprIndex)); }
#line 1786 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 162: // expr: expr.base OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 945 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_ArgumentListIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1792 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 163: // expr: expr.base OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 948 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), cxt.create(ArgumentList{}, empty_after(yystack_[3].location)), (yystack_[0].value.v_AnyExprIndex)); }
#line 1798 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 164: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 951 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), (yystack_[3].value.v_ArgumentListIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1804 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 165: // expr: expr DOT OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 954 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), cxt.create(ArgumentList{}, empty_after(yystack_[3].location)), (yystack_[0].value.v_AnyExprIndex)); }
#line 1810 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 166: // expr_or_seq.base: expr
#line 961 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1816 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 167: // expr_or_seq.base: expr_or_seq.base SEMICOLON expr
#line 962 "langutils/sc_parser/src/sc_grammar.y"
                                          { (yylhs.value.v_AnyExprIndex) = cxt.create(ExprSeq{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1822 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 168: // expr_or_seq: expr_or_seq.base semicolon.opt
#line 966 "langutils/sc_parser/src/sc_grammar.y"
                                       { (yylhs.value.v_AnyExprIndex) = (yystack_[1].value.v_AnyExprIndex); }
#line 1828 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 169: // generator: OPENCURLY COLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 975 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorAnyIndex) = cxt.create(GeneratorNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_GeneratorQualListIndex)); }
#line 1834 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 170: // generator: OPENCURLY SEMICOLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 978 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorAnyIndex) = cxt.create(GeneratorFakeNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_GeneratorQualListIndex)); }
#line 1840 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 171: // generator.qual.list: generator.qual
#line 983 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualListIndex) = cxt.create(GeneratorQualList{}, yylhs.location, (yystack_[0].value.v_GeneratorQualAnyIndex)); }
#line 1846 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 172: // generator.qual.list: generator.qual.list COMMA generator.qual
#line 985 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualListIndex) = cxt.graph->append((yystack_[2].value.v_GeneratorQualListIndex), yylhs.location, (yystack_[0].value.v_GeneratorQualAnyIndex)); }
#line 1852 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 173: // generator.qual: name LEFTARROW expr_or_seq
#line 990 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualAssign{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), cxt.create(Missing{}, empty_after(yystack_[2].location)), (yystack_[0].value.v_AnyExprIndex)); }
#line 1858 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 174: // generator.qual: name name LEFTARROW expr_or_seq
#line 992 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualAssign{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1864 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 175: // generator.qual: VAR name EQUALSSIGN expr_or_seq
#line 994 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualDeclare{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1870 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 176: // generator.qual: VAR name OPENPAREN expr_or_seq CLOSEPAREN
#line 996 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualDeclare{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex)); }
#line 1876 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 177: // generator.qual: expr_or_seq
#line 998 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualIf{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1882 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 178: // generator.qual: COLON COLON expr_or_seq
#line 1000 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualSideEffect{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1888 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 179: // generator.qual: COLON WHILE expr_or_seq
#line 1002 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualGuard{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1894 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 180: // argument_declarations.list: name
#line 1015 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1900 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 181: // argument_declarations.list: name EQUALSSIGN expr
#line 1017 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed((yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{is_bracketed}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex))); 
		}
#line 1909 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 182: // argument_declarations.list: name OPENPAREN expr_or_seq CLOSEPAREN
#line 1022 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1915 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 183: // argument_declarations.list: argument_declarations.list COMMA name
#line 1025 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[2].value.v_DeclareArgumentListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1921 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 184: // argument_declarations.list: argument_declarations.list COMMA name EQUALSSIGN expr
#line 1028 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed((yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[4].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{is_bracketed}, range(yystack_[2].location, yystack_[0].location), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)));
		}
#line 1930 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 185: // argument_declarations.list: argument_declarations.list COMMA name OPENPAREN expr_or_seq CLOSEPAREN
#line 1033 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[3].location, yystack_[0].location), (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1936 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 186: // argument_declarations.pipelist: name literal
#line 1039 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, (yystack_[1].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1942 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 187: // argument_declarations.pipelist: name
#line 1041 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1948 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 188: // argument_declarations.pipelist: name EQUALSSIGN literal
#line 1043 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1954 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 189: // argument_declarations.pipelist: name OPENPAREN expr_or_seq CLOSEPAREN
#line 1045 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1960 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 190: // argument_declarations.pipelist: name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 1047 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1966 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 191: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name literal
#line 1049 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[3].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[1].location, yystack_[0].location), (yystack_[1].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1972 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 192: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name
#line 1051 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[2].value.v_DeclareArgumentListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1978 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 193: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN literal
#line 1053 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[4].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[2].location, yystack_[0].location), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1984 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 194: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name OPENPAREN expr_or_seq CLOSEPAREN
#line 1055 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[3].location, yystack_[0].location), (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1990 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 195: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 1057 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[6].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[4].location, yystack_[0].location), (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1996 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 196: // argument_declarations: ARG SEMICOLON
#line 1061 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, empty_after(yystack_[1].location)); }
#line 2002 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 197: // argument_declarations: ARG argument_declarations.list comma.opt SEMICOLON
#line 1062 "langutils/sc_parser/src/sc_grammar.y"
                                                             { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[2].value.v_DeclareArgumentListIndex); }
#line 2008 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 198: // argument_declarations: ARG argument_declarations.list ELLIPSIS name SEMICOLON
#line 1064 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[3].value.v_DeclareArgumentListIndex), range(yystack_[3].location, yystack_[1].location), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 2014 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 199: // argument_declarations: ARG argument_declarations.list ELLIPSIS name COMMA name SEMICOLON
#line 1066 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[5].value.v_DeclareArgumentListIndex), range(yystack_[5].location, yystack_[1].location), cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 2020 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 200: // argument_declarations: ARG ELLIPSIS name SEMICOLON
#line 1068 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, range(yystack_[2].location, yystack_[1].location));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(arg_list, range(yystack_[2].location, yystack_[1].location), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); 
		}
#line 2029 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 201: // argument_declarations: ARG ELLIPSIS name COMMA name SEMICOLON
#line 1073 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, range(yystack_[4].location, yystack_[1].location));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(
				arg_list, range(yystack_[4].location, yystack_[1].location),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))
			); 
		}
#line 2042 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 202: // argument_declarations: ARG error
#line 1082 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips past the `;` ending the list. The arguments are discarded with the error.
			const auto close = error_recovery::arguments(cxt, yystack_[0].location, sc::lex::TokenType::SemiColon);
			if (!close)
				YYABORT;
			// Empty, as what was in it went with the error. `arg` and `;` belong to the block.
			yylhs.location = range(yystack_[1].location, *close);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, empty_after(yystack_[1].location));
			yyclearin;
			yyerrok; // Recovered at the end of the list, so report the next error.
		}
#line 2058 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 203: // argument_declarations: PIPE error
#line 1094 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips past the `|` ending the list. The arguments are discarded with the error.
			const auto close = error_recovery::arguments(cxt, yystack_[0].location, sc::lex::TokenType::Pipe);
			if (!close)
				YYABORT;
			// Empty, as what was in it went with the error. The `|`s belong to the block.
			yylhs.location = range(yystack_[1].location, *close);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, empty_after(yystack_[1].location));
			yyclearin;
			yyerrok; // Recovered at the end of the list, so report the next error.
		}
#line 2074 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 204: // argument_declarations: PIPE PIPE
#line 1106 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, empty_after(yystack_[1].location)); }
#line 2080 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 205: // argument_declarations: PIPE argument_declarations.pipelist comma.opt PIPE
#line 1108 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[2].value.v_DeclareArgumentListIndex); }
#line 2086 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 206: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name PIPE
#line 1110 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[3].value.v_DeclareArgumentListIndex), range(yystack_[3].location, yystack_[1].location), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 2092 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 207: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name COMMA name PIPE
#line 1112 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[5].value.v_DeclareArgumentListIndex), range(yystack_[5].location, yystack_[1].location), cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 2098 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 208: // argument_declarations: PIPE ELLIPSIS name PIPE
#line 1114 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, range(yystack_[2].location, yystack_[1].location));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(arg_list, range(yystack_[2].location, yystack_[1].location), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); 
		}
#line 2107 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 209: // argument_declarations: PIPE ELLIPSIS name COMMA name PIPE
#line 1119 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, range(yystack_[4].location, yystack_[1].location));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(
				arg_list, range(yystack_[4].location, yystack_[1].location),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))
			); 
		}
#line 2120 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 210: // argument_declarations.opt: %empty
#line 1130 "langutils/sc_parser/src/sc_grammar.y"
                                       { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 2126 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 211: // argument_declarations.opt: argument_declarations
#line 1131 "langutils/sc_parser/src/sc_grammar.y"
                                { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[0].value.v_DeclareArgumentListIndex); }
#line 2132 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 212: // argument_declarations.opt: error
#line 1135 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Nothing is written, so it is empty, before the error.
			yylhs.location = empty_before(yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location);
		}
#line 2142 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 213: // arguments.entries: KEYBINOP expr_or_seq
#line 1144 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_ArgumentEntryIndex) = cxt.create(KwArgNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2148 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 214: // arguments.entries: MULTIPLY expr_or_seq
#line 1145 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_ArgumentEntryIndex) = cxt.create(VariadicArgNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2154 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 215: // arguments.entries: expr_or_seq
#line 1146 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_ArgumentEntryIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 2160 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 216: // arguments.no_trailing: arguments.entries
#line 1150 "langutils/sc_parser/src/sc_grammar.y"
                            {  (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location, (yystack_[0].value.v_ArgumentEntryIndex)); }
#line 2166 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 217: // arguments.no_trailing: arguments.no_trailing COMMA arguments.entries
#line 1151 "langutils/sc_parser/src/sc_grammar.y"
                                                        { (yylhs.value.v_ArgumentListIndex) = cxt.graph->append((yystack_[2].value.v_ArgumentListIndex), yylhs.location, (yystack_[0].value.v_ArgumentEntryIndex)); }
#line 2172 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 218: // arguments: arguments.no_trailing comma.opt
#line 1154 "langutils/sc_parser/src/sc_grammar.y"
                                            { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2178 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 219: // arguments.paren: OPENPAREN arguments CLOSEPAREN
#line 1156 "langutils/sc_parser/src/sc_grammar.y"
                                                 { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2184 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 220: // arguments.maybe_paren: %empty
#line 1159 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location); }
#line 2190 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 221: // arguments.maybe_paren: OPENPAREN CLOSEPAREN
#line 1161 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, empty_after(yystack_[1].location)); }
#line 2196 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 222: // arguments.maybe_paren: OPENPAREN arguments CLOSEPAREN
#line 1163 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2202 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 223: // variable_declarations.list.item: name
#line 1172 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 2208 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 224: // variable_declarations.list.item: name EQUALSSIGN expr
#line 1174 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2214 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 225: // variable_declarations.list.item: name OPENPAREN expr_or_seq CLOSEPAREN
#line 1176 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Bracketed, as in the old compiler, so it isn't a literal, see ConstEval::is_legacy_literal.
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex));
		}
#line 2224 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 226: // variable_declarations.list.item: name.pack EQUALSSIGN expr
#line 1182 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[2].value.v_NamePackIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2230 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 227: // variable_declarations.list.item: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 1184 "langutils/sc_parser/src/sc_grammar.y"
                {
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[3].value.v_NamePackIndex), (yystack_[1].value.v_AnyExprIndex));
		}
#line 2239 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 228: // variable_declarations.list: variable_declarations.list.item
#line 1192 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareVariableListIndex) = cxt.create(DeclareVariableList{}, yylhs.location, (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 2245 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 229: // variable_declarations.list: variable_declarations.list COMMA variable_declarations.list.item
#line 1194 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareVariableListIndex) = cxt.graph->append((yystack_[2].value.v_DeclareVariableListIndex), yylhs.location, (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 2251 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 230: // variable_declarations: VAR variable_declarations.list
#line 1197 "langutils/sc_parser/src/sc_grammar.y"
                                                       { (yylhs.value.v_DeclareVariableListIndex) = (yystack_[0].value.v_DeclareVariableListIndex); }
#line 2257 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 231: // literal.terminal: symbol
#line 1205 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_AnySymbolIndex); }
#line 2263 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 232: // literal.terminal: string
#line 1206 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_StringLitIndex); }
#line 2269 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 233: // literal.terminal: integer
#line 1207 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 2275 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 234: // literal.terminal: float
#line 1208 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_FloatProducingIndex); }
#line 2281 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 235: // literal.terminal: boolean
#line 1209 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_BooleanLitIndex); }
#line 2287 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 236: // literal.terminal: nil
#line 1210 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_NilLitIndex); }
#line 2293 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 237: // literal.terminal: ascii
#line 1211 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_ASCIIIndex); }
#line 2299 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 238: // literal.terminal: block
#line 1212 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_BlockIndex); }
#line 2305 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 239: // literal.array.contents: %empty
#line 1217 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location); }
#line 2311 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 240: // literal.array.contents: expr_or_seq
#line 1219 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2317 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 241: // literal.array.contents: expr_or_seq COLON expr_or_seq
#line 1221 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2323 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 242: // literal.array.contents: KEYBINOP expr_or_seq
#line 1223 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2329 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 243: // literal.array.contents: literal.array.contents COMMA expr_or_seq
#line 1225 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[2].value.v_ArrayIndex), yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2335 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 244: // literal.array.contents: literal.array.contents COMMA expr_or_seq COLON expr_or_seq
#line 1227 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[4].value.v_ArrayIndex), yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2341 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 245: // literal.array.contents: literal.array.contents COMMA KEYBINOP expr_or_seq
#line 1229 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[3].value.v_ArrayIndex), yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2347 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 246: // literal.dictionary.entry: expr COLON expr_or_seq
#line 1237 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryEntryIndex) = cxt.create(DictionaryEntryNode{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2353 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 247: // literal.dictionary.entry: KEYBINOP expr_or_seq
#line 1239 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryEntryIndex) = cxt.create(DictionaryEntryNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2359 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 248: // literal.dictionary.entries: %empty
#line 1244 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.create(DictionaryNode{}, yylhs.location); }
#line 2365 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 249: // literal.dictionary.entries: literal.dictionary.entry
#line 1246 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.create(DictionaryNode{}, yylhs.location, (yystack_[0].value.v_DictionaryEntryIndex)); }
#line 2371 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 250: // literal.dictionary.entries: literal.dictionary.entries COMMA literal.dictionary.entry
#line 1248 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.graph->append((yystack_[2].value.v_DictionaryIndex), yylhs.location,  (yystack_[0].value.v_DictionaryEntryIndex)); }
#line 2377 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 251: // literal.dictionary: OPENPAREN literal.dictionary.entries comma.opt CLOSEPAREN
#line 1253 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->location(*(yystack_[2].value.v_DictionaryIndex)) = yylhs.location; (yylhs.value.v_DictionaryIndex) = (yystack_[2].value.v_DictionaryIndex); }
#line 2383 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 252: // literal.array: OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1258 "langutils/sc_parser/src/sc_grammar.y"
                {  cxt.graph->location(*(yystack_[2].value.v_ArrayIndex)) = yylhs.location; (yylhs.value.v_ArrayIndex) = (yystack_[2].value.v_ArrayIndex); }
#line 2389 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 253: // literal.array: HASH OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1260 "langutils/sc_parser/src/sc_grammar.y"
                {  
			cxt.graph->payload((yystack_[2].value.v_ArrayIndex)).is_immutable = true;
			cxt.graph->location(*(yystack_[2].value.v_ArrayIndex)) = yylhs.location;
			(yylhs.value.v_ArrayIndex) = (yystack_[2].value.v_ArrayIndex);
		}
#line 2399 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 254: // literal: literal.terminal
#line 1268 "langutils/sc_parser/src/sc_grammar.y"
                           { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_AnyPossiblyLiteralIndex); }
#line 2405 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 255: // literal: literal.array
#line 1269 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_ArrayIndex); }
#line 2411 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 256: // literal: literal.dictionary
#line 1270 "langutils/sc_parser/src/sc_grammar.y"
                             { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_DictionaryIndex); }
#line 2417 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 257: // name: NAME
#line 1278 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_NamedIdentifierIndex) = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 2423 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 258: // name: WHILE
#line 1279 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamedIdentifierIndex) = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 2429 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 259: // name.list: HASH name
#line 1285 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.create(NameList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2435 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 260: // name.list: HASH TILDE name
#line 1287 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.create(NameList{}, yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2441 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 261: // name.list: name.list COMMA name
#line 1289 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.graph->append((yystack_[2].value.v_NameListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2447 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 262: // name.list: name.list COMMA TILDE name
#line 1291 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.graph->append((yystack_[3].value.v_NameListIndex), yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2453 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 263: // name.pack: name.list
#line 1295 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[0].value.v_NameListIndex), cxt.create(Missing{}, empty_after(yystack_[0].location))); }
#line 2459 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 264: // name.pack: name.list ELLIPSIS name
#line 1297 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[2].value.v_NameListIndex), (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2465 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 265: // name.pack: name.list ELLIPSIS TILDE name
#line 1299 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[3].value.v_NameListIndex), cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2471 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 266: // ascii: ASCII
#line 1309 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_ASCIIIndex) = cxt.create(ASCIINode{}, yylhs.location); }
#line 2477 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 267: // nil: NIL
#line 1311 "langutils/sc_parser/src/sc_grammar.y"
          { (yylhs.value.v_NilLitIndex) = cxt.create(NilNode{}, yylhs.location); }
#line 2483 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 268: // boolean: TRUE
#line 1314 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_BooleanLitIndex) = cxt.create(BooleanNode{true}, yylhs.location); }
#line 2489 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 269: // boolean: FALSE
#line 1315 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BooleanLitIndex) = cxt.create(BooleanNode{false}, yylhs.location); }
#line 2495 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 270: // symbol: SYMBOL_QUOTE
#line 1319 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_AnySymbolIndex) = cxt.create(SymbolQuote{}, yylhs.location); }
#line 2501 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 271: // symbol: SYMBOL_SLASH
#line 1320 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_AnySymbolIndex) = cxt.create(SymbolSlash{}, yylhs.location); }
#line 2507 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 272: // string: STRINGLINE
#line 1324 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_StringLitIndex) = cxt.create(StringLineList{}, yylhs.location, cxt.create(StringLineNode{}, yylhs.location)); }
#line 2513 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 273: // string: string STRINGLINE
#line 1325 "langutils/sc_parser/src/sc_grammar.y"
                            { (yylhs.value.v_StringLitIndex) = cxt.graph->append((yystack_[1].value.v_StringLitIndex), yylhs.location, cxt.create(StringLineNode{}, yystack_[0].location)); }
#line 2519 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 274: // integer.unsigned: INTEGER
#line 1329 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{}, yylhs.location); }
#line 2525 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 275: // integer.unsigned: INTEGER_RADIX
#line 1330 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{IntNode::Kind::Radix}, yylhs.location); }
#line 2531 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 276: // integer.unsigned: HEXADECIMAL
#line 1331 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{IntNode::Kind::Hexadecimal}, yylhs.location); }
#line 2537 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 277: // integer: integer.unsigned
#line 1335 "langutils/sc_parser/src/sc_grammar.y"
                           { (yylhs.value.v_IntLitIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 2543 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 278: // integer: MINUS integer.unsigned
#line 1337 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload((yystack_[0].value.v_IntLitIndex)).sign = IntNode::Sign::Negative; (yylhs.value.v_IntLitIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 2549 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 279: // float.raw_unsigned: FLOAT
#line 1341 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{}, yylhs.location); }
#line 2555 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 280: // float.raw_unsigned: FLOAT_RADIX
#line 1342 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Radix}, yylhs.location); }
#line 2561 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 281: // float.raw_unsigned: FLOAT_EXPONENT
#line 1343 "langutils/sc_parser/src/sc_grammar.y"
                         { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Exponent}, yylhs.location); }
#line 2567 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 282: // float.raw_unsigned: FLOAT_INF
#line 1344 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Inf}, yylhs.location); }
#line 2573 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 283: // float.raw: float.raw_unsigned
#line 1349 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FloatLitIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2579 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 284: // float.raw: MINUS float.raw_unsigned
#line 1351 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload((yystack_[0].value.v_FloatLitIndex)).sign = FloatNode::Sign::Negative; (yylhs.value.v_FloatLitIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2585 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 285: // accidental.unsigned: ACCIDENTAL_STEPS
#line 1356 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = cxt.create(AccidentalNode{AccidentalNode::Kind::Steps}, yylhs.location); }
#line 2591 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 286: // accidental.unsigned: ACCIDENTAL_CENTS
#line 1358 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = cxt.create(AccidentalNode{AccidentalNode::Kind::Cents}, yylhs.location); }
#line 2597 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 287: // accidental: accidental.unsigned
#line 1363 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2603 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 288: // accidental: MINUS accidental.unsigned
#line 1365 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload((yystack_[0].value.v_AccidentalIndex)).sign = AccidentalNode::Sign::Negative; (yylhs.value.v_AccidentalIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2609 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 289: // float: float.raw
#line 1369 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_FloatProducingIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2615 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 290: // float: accidental
#line 1370 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_FloatProducingIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2621 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 291: // float: float.raw PI
#line 1371 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, (yystack_[1].value.v_FloatLitIndex)); }
#line 2627 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 292: // float: integer PI
#line 1372 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, (yystack_[1].value.v_IntLitIndex)); }
#line 2633 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 293: // float: PI
#line 1373 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, cxt.create(Missing{}, empty_before(yystack_[0].location))); }
#line 2639 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 294: // float: MINUS PI
#line 1374 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{PiNode::Sign::Negative}, yylhs.location, cxt.create(Missing{}, empty_before(yystack_[0].location))); }
#line 2645 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 295: // accessor: %empty
#line 1378 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::Private}, yylhs.location); }
#line 2651 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 296: // accessor: LESSTHAN
#line 1379 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyReadable}, yylhs.location); }
#line 2657 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 297: // accessor: GREATERTHAN
#line 1380 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyWriteable}, yylhs.location); }
#line 2663 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 298: // accessor: READWRITEVAR
#line 1382 "langutils/sc_parser/src/sc_grammar.y"
                { 
			(yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{
				combine<ReadWriteAccessor::ExternallyReadable, ReadWriteAccessor::ExternallyWriteable>()
			}, yylhs.location); 
		}
#line 2673 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;


#line 2677 "langutils/sc_parser/src/sc_grammar_parser.cpp"

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
  "LEFTARROW", "WHILE", "LEXER_ERROR", "EMPTY_ARGUMENTS", "COLON",
  "EQUALSSIGN", "BINOP", "KEYBINOP", "MINUS", "LESSTHAN", "GREATERTHAN",
  "MULTIPLY", "ADD", "PIPE", "READWRITEVAR", "DOT", "BACKTICK", "$accept",
  "go", "file_as_func", "interpreted_function", "region.item", "region",
  "classOrExtList.list", "classOrExtList.item", "class", "class.super.opt",
  "class.slot.opt", "class.extension", "members.entry.item",
  "members.entry.list", "members.entry", "members", "method.name",
  "method.base", "method", "method.list", "method.list.opt", "block.open",
  "block", "block.opt_list", "block.list", "block.contents",
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
  "symbol", "string", "integer.unsigned", "integer", "float.raw_unsigned",
  "float.raw", "accidental.unsigned", "accidental", "float", "accessor",
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




  const short parser::yypact_ninf_ = -235;

  const short parser::yytable_ninf_ = -301;

  const short
  parser::yypact_[] =
  {
      24,  -235,    42,   835,   783,    -2,    53,  -235,  -235,   306,
      30,  -235,  -235,  -235,   200,   -17,  -235,  -235,    27,  2104,
    1045,  2666,   120,   121,  -235,  -235,  -235,  -235,  -235,  -235,
    -235,  -235,  -235,  -235,  -235,  -235,  -235,  -235,   296,  -235,
     136,   108,  -235,  -235,  -235,  -235,  -235,  -235,   615,    34,
    2666,  -235,  1217,  -235,  -235,  -235,  -235,  -235,  -235,  -235,
    -235,    57,   674,  -235,  1586,   103,  -235,  -235,  -235,  -235,
     152,   169,    19,  -235,  -235,  -235,  -235,   106,  -235,    99,
    -235,   113,  -235,  -235,  -235,  -235,  1045,  -235,  2666,    29,
    -235,   674,   942,  -235,   171,   175,  -235,  -235,  2156,  2156,
     195,  -235,  -235,  -235,   191,   180,   216,  2666,  2666,  1105,
    2666,   674,   214,   183,   227,  -235,  2666,  2207,  -235,  2666,
     615,  -235,  -235,  -235,  -235,   231,  -235,  -235,   241,   701,
     267,  1378,   225,  -235,   251,   674,  2104,   121,  -235,   229,
    2104,  1741,  -235,    61,  -235,    61,   222,  -235,   271,    21,
      47,  -235,  -235,   121,   204,    54,  -235,  -235,  -235,  -235,
    -235,   121,  -235,   280,  2713,  -235,  -235,  -235,  1430,  1637,
    -235,  -235,  -235,   223,   257,  2666,    29,   994,   303,  1793,
    2666,    61,   242,   252,  2666,  2666,  -235,  -235,  -235,  1274,
     311,   674,   887,  2666,   325,    29,   328,  -235,    85,  -235,
    -235,  -235,  -235,  -235,   690,   317,   318,  -235,  2666,  -235,
    2666,  2258,   326,   327,  2666,   112,  -235,   226,  2666,  -235,
    2666,  2309,  -235,    29,  1586,  2360,   330,   227,  -235,  2666,
     227,    61,  2666,  2666,  -235,  -235,   322,   331,  -235,  -235,
    -235,    61,   136,  2666,  2666,  2666,  2666,   300,   121,   121,
     333,  2666,  2666,    33,  -235,   121,   116,  -235,  2360,   338,
    2791,  -235,  -235,    29,   286,  2666,   155,  1689,  1845,    61,
      71,   244,  -235,   285,   345,    29,  -235,    61,   342,   674,
     121,  -235,   121,  -235,   344,   674,    29,   346,  1482,   348,
    2666,  -235,   674,  -235,   356,  -235,  -235,   570,   343,  -235,
    -235,   347,   354,  -235,  -235,    65,   146,   198,  -235,   350,
     358,  2001,  2001,   674,  -235,  2666,   314,  -235,  -235,   357,
    2666,  2411,  1897,    61,  -235,   329,  -235,   359,   360,    29,
    2666,   701,  -235,  -235,   365,   674,   366,  -235,  -235,  -235,
    2052,  -235,  -235,  -235,   368,   674,   369,   674,  -235,   121,
     119,   307,  -235,   371,   674,   121,  -235,    38,  -235,  2752,
     372,  2360,  -235,   377,  2666,   367,   337,  2462,   339,  2666,
     201,    61,  -235,  1949,  2666,    61,  2666,    41,  -235,  -235,
    -235,   384,    61,    22,  -235,  -235,  -235,   380,   392,    29,
     393,   674,  -235,  -235,  1165,  -235,  -235,  -235,  -235,  -235,
    -235,  -235,   388,   136,  -235,   388,  -235,   388,   261,    85,
    -235,   121,   107,  -235,   397,  -235,   172,   397,  -235,  2666,
    -235,   355,  -235,   402,    61,   404,  2513,  -235,  -235,   405,
    -235,  -235,  -235,  -235,  -235,   407,  2666,  2666,  -235,   121,
    -235,   361,   121,  -235,  2360,  2830,  -235,  -235,   406,  -235,
     674,   370,  2666,   373,   411,  2666,   412,   374,  2564,  -235,
    -235,   413,   674,  -235,   419,  -235,  2666,  -235,   431,  -235,
     423,  -235,  1326,    -4,  -235,  -235,   428,   122,  2666,  2666,
    2001,   430,  2666,   390,   432,  -235,  2615,  -235,    61,    61,
    -235,   434,  -235,  -235,   435,   674,   429,  -235,   386,   437,
    2360,  -235,  -235,  2666,   674,  2666,   399,   674,   400,  2666,
     408,   441,  -235,  -235,   674,  -235,   453,  -235,   445,    29,
    -235,  -235,  2666,  2666,  -235,  -235,  -235,  -235,  -235,  2666,
    -235,  -235,   447,  -235,  -235,  -235,  -235,  -235,  -235,   448,
     674,   674,  2666,  2666,   674,  2666,   409,  -235,  1534,   456,
     449,  -235,  -235,  -235,  -235,   674,   674,   674,  2666,  -235,
      29,  -235,  -235,   674,   459,  -235
  };

  const short
  parser::yydefact_[] =
  {
       0,    33,    38,     0,     0,     0,     0,     4,     5,     0,
       0,    29,    31,    32,     0,    36,     3,    25,    66,   239,
       0,     0,     0,     0,   257,   274,   275,   276,   279,   280,
     281,   282,   285,   286,   270,   271,   272,   266,   155,   137,
       0,     0,   267,   268,   269,   293,    67,   258,     0,     0,
       0,    24,     0,   238,   138,   149,   150,   144,   148,   145,
     146,   147,    19,   135,     0,   299,   254,   256,   255,   134,
     136,   263,     0,   237,   236,   235,   231,   232,   277,   233,
     283,   289,   287,   290,   234,    83,     0,   300,     0,   299,
      76,    81,     0,    82,     0,     0,     1,    28,     0,   300,
       0,     6,    30,    40,     0,     0,     0,     0,     0,     0,
       0,   166,   299,   240,   301,   140,     0,     0,    87,    96,
      91,    89,    90,    92,    93,     0,    88,    95,     0,   166,
       0,     0,     0,   249,   301,    20,   239,     0,   259,   141,
     239,     0,    72,   109,    73,    70,     0,   228,   230,   223,
       0,   202,   196,     0,   301,   180,   294,   278,   284,   288,
     203,     0,   204,   301,   187,   156,   212,   211,     0,     0,
      96,    91,    94,     0,    98,     0,   299,     0,     0,     0,
       0,   104,     0,     0,     0,     0,   273,   292,   291,     0,
     299,    80,     0,     0,     0,   299,     0,    16,     0,    27,
      26,     2,    39,    37,     0,     0,     0,   242,   300,   168,
       0,   302,     0,     0,     0,     0,   247,     0,     0,   139,
       0,     0,    22,   299,     0,   302,     0,   301,   260,     0,
     301,    70,     0,     0,   215,   216,   301,     0,    74,    75,
     110,    71,     0,     0,     0,     0,     0,     0,   302,     0,
       0,     0,     0,     0,   302,     0,     0,    66,   248,     0,
       0,   186,    69,   299,   143,     0,     0,     0,     0,    70,
     220,     0,    97,   153,     0,   299,    10,     0,     0,   157,
       0,   261,     0,   264,     0,   160,   299,     0,     0,     0,
       0,    77,    78,    15,     0,    12,    60,    92,     0,    59,
      62,     0,     0,    54,    53,     0,     0,     0,    51,   299,
       0,     0,     0,   167,   241,     0,   243,   252,   128,     0,
       0,     0,     0,   101,   246,     0,   124,     0,     0,   299,
       0,     0,   250,   251,     0,   158,     0,   111,   213,   214,
     302,   218,   219,   229,     0,   224,     0,   226,   200,     0,
     183,     0,   197,     0,   181,     0,   208,     0,   205,   192,
       0,   248,   188,     0,     0,     0,   142,     0,   152,     0,
       0,    70,   106,     0,     0,    70,     0,     0,    84,    85,
       8,     0,   103,    70,   262,   265,   161,     0,     0,   299,
       0,    79,    11,    61,     0,    63,    41,    49,   296,   297,
     298,    43,    46,     0,    48,    45,    50,    47,   300,     0,
      35,     0,     0,   177,   301,   171,   136,   301,   245,     0,
     132,     0,   133,     0,     0,     0,     0,   125,    21,     0,
     253,   108,   217,   225,   227,     0,     0,     0,   198,     0,
     182,     0,     0,   206,   248,     0,   191,   189,     0,    68,
     163,   120,     0,   119,     0,     0,     0,   151,     0,   107,
     221,     0,   154,   105,     0,     9,     0,   102,     0,    14,
       0,    18,     0,   295,    42,    52,     0,     0,     0,     0,
     302,     0,     0,     0,     0,   244,     0,   129,   100,    70,
     127,     0,    23,   201,     0,   184,     0,   209,     0,     0,
     248,   193,   190,     0,   162,     0,   118,   165,   113,     0,
     114,     0,   222,    86,   159,    13,     0,    56,   299,   299,
      44,    34,     0,     0,   179,   178,   172,   170,   173,     0,
     169,   131,     0,    99,   126,   185,   199,   207,   194,     0,
     123,   122,     0,     0,   164,     0,   112,    17,     0,     0,
       0,   175,   174,   130,   195,   121,   117,   116,     0,    58,
     299,    55,   176,   115,     0,    57
  };

  const short
  parser::yypgoto_[] =
  {
    -235,  -235,  -235,  -235,   215,  -235,  -235,   457,  -235,  -235,
    -235,  -235,    -7,    20,    60,  -235,  -235,   173,   170,  -235,
    -197,  -235,   284,  -225,   -29,   -60,   281,  -235,  -182,   -10,
    -235,  -235,  -235,  -235,  -235,  -235,  -235,  -235,  -235,    -3,
    -235,   165,   297,   160,    -5,  -235,  -235,    16,    87,   139,
    -235,  -156,   309,  -235,  -234,  -235,    18,  -235,   -94,   262,
    -235,  -235,  -235,  -159,   301,  -235,   -38,  -235,  -235,  -235,
    -235,  -235,   -45,   218,   -36,  -235,   -33,  -235,  -235,  -235,
       2,  -120
  };

  const short
  parser::yydefgoto_[] =
  {
       0,     6,     7,     8,    51,     9,    10,    11,    12,   106,
      15,    13,   401,   402,   308,   309,   298,   299,   300,   301,
     302,    52,    53,   240,   241,    89,    90,   272,   127,   174,
     175,    54,    55,    56,    57,    58,    59,    60,    61,   111,
     112,   234,    63,   414,   415,   154,   163,   167,   168,   235,
     236,   237,   145,   375,   147,   148,    93,    66,   114,   133,
     134,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,   403,
      94,   212
  };

  const short
  parser::yytable_[] =
  {
      62,    91,   150,   157,   176,   261,   337,   310,   343,   143,
     128,   100,   158,   266,   226,   159,   303,   129,   135,    64,
      92,    65,   303,   278,    -7,     1,    18,   184,    95,   243,
     101,     1,   195,   105,   250,   160,   131,   107,   132,   192,
     193,   181,   227,   256,   372,   355,   230,   165,    14,    24,
     442,   398,   399,    96,     2,   245,   400,    25,    26,    27,
       2,    91,   251,   169,    46,    18,   397,   178,     3,     4,
     185,   223,   244,   466,   161,   157,   128,   108,  -295,   373,
    -295,    47,     5,   129,   158,   191,   296,   159,     5,    91,
     -64,   194,   356,   162,   196,    62,    62,   443,   246,   128,
      24,   362,   189,    46,   190,   252,   129,   334,   263,   151,
     336,   370,  -295,   177,   209,   303,   341,   275,   152,   303,
     398,   399,   374,    24,   320,   400,   136,   436,    91,   286,
     522,    24,    47,   186,   137,    24,    24,   118,   187,   171,
     121,   122,   297,   124,   172,   126,   459,   404,   153,   146,
     463,    24,   188,   321,   478,    47,    18,   479,   467,  -295,
     179,  -295,   366,    47,   329,    91,   425,    47,    47,   474,
     437,   197,   273,   523,    91,   358,    18,   279,   274,   198,
     179,   182,   285,    47,   113,   130,    91,    24,   323,    91,
     292,   287,   289,  -295,    46,   201,   367,   294,   202,   406,
     446,   398,   399,   180,   150,   313,   400,   103,   457,   183,
     203,  -295,   476,  -295,    46,    24,   248,   461,   482,    47,
     204,    91,   331,   180,   208,   328,   335,   303,   389,   267,
      18,   268,   160,   210,   322,   224,   137,    24,    24,   211,
     -94,   345,   458,   347,   249,  -295,    24,    47,   382,   354,
     217,   130,   376,   398,   399,   129,   280,    24,   400,    24,
      25,    26,    27,   225,   533,   363,   282,    24,    46,    47,
      47,   161,   205,   206,   130,   207,   219,   381,    47,   220,
     229,   213,   215,   242,   216,    91,   501,   391,   387,    47,
     162,    47,   254,   305,   481,   306,   307,   484,   377,    47,
      18,   113,   140,   276,   141,   113,  -299,    97,   221,    98,
     348,   409,   349,   199,   200,   104,    99,   438,   271,   439,
     255,   288,   142,   138,   139,   293,   405,   407,   295,   311,
     312,   429,   157,   317,   340,   144,   318,   364,    46,   333,
     342,   149,   155,   352,   136,   380,   173,   394,   296,   284,
     164,   383,   -65,   386,   142,   388,   392,   390,   129,   396,
     408,   450,    24,   410,   419,   150,   420,   144,   427,   428,
     426,   462,   430,   431,   451,   314,   316,   433,   434,   319,
     440,   447,   449,   324,   465,   325,   327,   181,   452,   468,
     455,   470,   469,   471,    47,   488,   486,   338,   339,   118,
     473,   171,   121,   122,   297,   124,   172,   126,   344,   480,
     346,   487,   519,   489,   492,   502,   353,   493,   506,   508,
     497,   503,   512,   360,   505,   509,   164,   238,   513,   142,
     365,   515,   516,   521,   495,   527,   529,   530,   228,   536,
     239,   129,   144,   534,   535,   537,   538,   138,   546,   504,
     542,   543,   507,   547,   247,    87,   553,   554,   562,   545,
     558,   561,   253,   514,   565,   238,   520,   102,   475,    91,
     393,   395,   417,   291,   270,   526,   413,   413,   239,   432,
     418,   472,   269,   281,   283,   421,   423,   332,   560,   379,
       0,     0,     0,     0,     0,   216,     0,   129,     0,   304,
     540,   142,   541,     0,     0,   304,   544,     0,     0,     0,
       0,     0,     0,     0,   144,   142,     0,     0,     0,     0,
     548,   549,     0,     0,     0,   238,   448,     0,   144,     0,
       0,     0,   454,     0,   456,     0,     0,     0,   239,   555,
     556,   464,   557,   149,     0,    91,     0,     0,     0,   350,
     351,     0,     0,   142,     0,   563,   357,   359,     0,     0,
       0,   142,   564,     0,     0,     0,   144,     0,     0,     0,
       0,     0,   378,     0,   144,     0,     0,     0,     0,     0,
       0,   384,     0,   385,   485,    24,     0,     0,     0,     0,
       0,   491,     0,     0,     0,     0,     0,     0,   304,     0,
       0,   494,   304,     0,     0,     0,     0,   238,     0,   499,
       0,     0,   416,   416,     0,     0,     0,    47,     0,     0,
     239,     0,   118,   511,   171,   121,   122,   123,   124,   172,
     126,    25,    26,    27,    28,    29,    30,    31,    32,    33,
       0,     0,     0,   524,   525,   413,     0,   528,     0,     0,
     435,   532,     0,     0,   156,   142,   441,     0,     0,   142,
       0,     0,     0,     0,     0,   539,   238,   142,   144,     0,
       0,     0,   144,     0,     0,     0,     0,     0,     0,   239,
     144,     0,     0,     0,     0,     0,     0,   550,   551,     0,
       0,   296,     0,     0,   552,   -64,     0,     0,     0,     0,
     142,     0,     0,     0,   149,    24,     0,     0,   142,     0,
     304,     0,   477,   144,     0,     0,     0,   483,     0,     0,
       0,   144,   305,     0,   306,   307,   118,   170,   171,   121,
     122,   123,   124,   172,   126,   173,     0,    47,     0,     0,
     496,     0,   118,   498,   171,   121,   122,   297,   124,   172,
     126,   218,     0,   118,   170,   171,   121,   122,   123,   124,
     172,   126,   173,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   238,   142,     0,     0,     0,     0,     0,     0,
       0,   416,     0,  -299,    85,   239,   144,    18,     0,    19,
       0,    86,     0,    87,    88,     0,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,     0,    38,    39,    40,    41,     0,     0,    42,
      43,    44,    45,     0,     0,    46,     0,     0,     0,     0,
      47,     0,     0,     0,     0,    16,    17,    48,     0,    18,
       0,    19,    49,    20,     0,    50,    21,     0,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,     0,    38,    39,    40,    41,     0,
       0,    42,    43,    44,    45,     0,     0,    46,     0,     0,
       0,     0,    47,     0,     0,     0,     0,  -300,    85,    48,
       0,    18,  -300,    19,    49,   109,  -300,    50,   290,     0,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,     0,    38,    39,    40,
       0,     0,     0,    42,    43,    44,    45,     0,     0,    46,
       0,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,    48,  -299,    85,     0,     0,    18,     0,    19,    50,
     109,     0,    87,    88,     0,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,     0,    38,    39,    40,     0,     0,     0,    42,    43,
      44,    45,     0,     0,    46,     0,     0,     0,     0,    47,
       0,     0,     0,     0,  -300,    85,    48,     0,    18,     0,
      19,     0,   109,     0,    50,    88,     0,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,     0,    38,    39,    40,     0,     0,     0,
      42,    43,    44,    45,     0,     0,    46,     0,     0,     0,
       0,    47,     0,     0,     0,     0,   115,     0,    48,    18,
       0,    19,     0,   109,  -248,     0,    50,  -248,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,     0,    38,    39,    40,    41,     0,
       0,    42,    43,    44,    45,     0,   116,    46,     0,     0,
       0,     0,    47,     0,     0,   117,     0,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   115,    50,     0,    18,
       0,    19,     0,   109,  -248,     0,     0,  -248,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,     0,    38,    39,     0,     0,     0,
       0,    42,    43,    44,    45,     0,   116,    46,     0,     0,
       0,     0,    47,     0,     0,   117,     0,   118,   119,   120,
     121,   122,   123,   124,   172,   126,   166,    50,     0,  -210,
    -210,  -210,     0,  -210,     0,     0,  -210,     0,  -210,  -210,
    -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,
    -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,    41,     0,
       0,  -210,  -210,  -210,  -210,     0,     0,  -210,     0,     0,
       0,     0,  -210,     0,     0,     0,     0,     0,   166,  -210,
       0,  -210,  -210,  -210,    49,  -210,     0,  -210,  -210,     0,
    -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,
    -210,  -210,  -210,  -210,  -210,  -210,     0,  -210,  -210,  -210,
      41,     0,     0,  -210,  -210,  -210,  -210,     0,     0,  -210,
       0,     0,     0,     0,  -210,     0,     0,     0,     0,     0,
       0,  -210,     0,     0,     0,    85,    49,     0,    18,  -210,
      19,     0,   109,  -299,    87,    88,     0,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,     0,    38,    39,    40,     0,     0,     0,
      42,    43,    44,    45,     0,     0,    46,     0,     0,     0,
       0,    47,     0,     0,     0,     0,     0,    85,    48,     0,
      18,   517,    19,     0,   109,     0,    50,    88,     0,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,   518,    38,    39,    40,     0,
       0,     0,    42,    43,    44,    45,     0,     0,    46,     0,
       0,     0,     0,    47,     0,     0,     0,     0,     0,    85,
      48,     0,    18,     0,    19,     0,   109,   222,    50,    88,
       0,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,     0,    38,    39,
      40,     0,     0,     0,    42,    43,    44,    45,     0,     0,
      46,     0,     0,     0,     0,    47,     0,     0,     0,     0,
       0,    85,    48,     0,    18,   262,    19,     0,   109,     0,
      50,    88,     0,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,     0,
      38,    39,    40,     0,     0,     0,    42,    43,    44,    45,
       0,     0,    46,     0,     0,     0,     0,    47,     0,     0,
       0,     0,     0,    85,    48,     0,    18,     0,    19,     0,
     109,  -300,    50,    88,     0,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,     0,    38,    39,    40,     0,     0,     0,    42,    43,
      44,    45,     0,     0,    46,     0,     0,     0,     0,    47,
       0,     0,     0,     0,     0,    85,    48,     0,    18,   559,
      19,     0,   109,     0,    50,    88,     0,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,     0,    38,    39,    40,     0,     0,     0,
      42,    43,    44,    45,     0,     0,    46,     0,     0,     0,
       0,    47,     0,     0,     0,     0,     0,    85,    48,     0,
      18,     0,    19,     0,   109,     0,    50,    88,     0,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,     0,    38,    39,    40,     0,
       0,     0,    42,    43,    44,    45,     0,     0,    46,     0,
       0,     0,     0,    47,     0,     0,     0,     0,     0,     0,
      48,    18,     0,    19,   264,   109,     0,     0,    50,     0,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,     0,    38,    39,     0,
       0,     0,     0,    42,    43,    44,    45,     0,   265,    46,
       0,     0,     0,     0,    47,     0,     0,     0,     0,     0,
     232,    48,     0,    18,   233,    19,   368,   109,     0,    50,
       0,     0,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,     0,    38,
      39,     0,     0,     0,     0,    42,    43,    44,    45,     0,
     369,    46,     0,     0,     0,     0,    47,     0,     0,     0,
       0,     0,   232,    48,     0,    18,   233,    19,     0,   109,
     231,    50,     0,     0,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
       0,    38,    39,     0,     0,     0,     0,    42,    43,    44,
      45,     0,     0,    46,     0,     0,     0,     0,    47,     0,
       0,     0,     0,     0,   232,    48,     0,    18,   233,    19,
       0,   109,   277,    50,     0,     0,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,     0,    38,    39,     0,     0,     0,     0,    42,
      43,    44,    45,     0,     0,    46,     0,     0,     0,     0,
      47,     0,     0,     0,     0,     0,   232,    48,     0,    18,
     233,    19,     0,   109,   371,    50,     0,     0,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,     0,    38,    39,     0,     0,     0,
       0,    42,    43,    44,    45,     0,     0,    46,     0,     0,
       0,     0,    47,     0,     0,     0,     0,     0,   232,    48,
       0,    18,   233,    19,     0,   109,   424,    50,     0,     0,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,     0,    38,    39,     0,
       0,     0,     0,    42,    43,    44,    45,     0,     0,    46,
       0,     0,     0,     0,    47,     0,     0,     0,     0,     0,
     232,    48,     0,    18,   233,    19,     0,   109,   460,    50,
       0,     0,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,     0,    38,
      39,     0,     0,     0,     0,    42,    43,    44,    45,     0,
       0,    46,     0,     0,     0,     0,    47,     0,     0,     0,
       0,     0,   232,    48,     0,    18,   233,    19,     0,   109,
       0,    50,     0,     0,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
       0,    38,    39,   411,     0,     0,     0,    42,    43,    44,
      45,     0,     0,    46,     0,     0,     0,     0,    47,     0,
       0,   412,     0,     0,     0,    48,    18,     0,    19,     0,
     109,     0,     0,    50,     0,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,     0,    38,    39,     0,     0,     0,     0,    42,    43,
      44,    45,     0,     0,    46,     0,     0,     0,     0,    47,
       0,     0,     0,     0,     0,   232,    48,     0,    18,   233,
      19,     0,   109,     0,    50,     0,     0,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,     0,    38,    39,     0,     0,     0,     0,
      42,    43,    44,    45,     0,     0,    46,     0,     0,     0,
       0,    47,     0,     0,     0,     0,     0,   110,    48,     0,
      18,     0,    19,     0,    20,     0,    50,    21,     0,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,     0,    38,    39,     0,     0,
       0,     0,    42,    43,    44,    45,     0,     0,    46,     0,
       0,     0,     0,    47,     0,     0,     0,     0,     0,     0,
      48,    18,     0,    19,     0,   109,     0,     0,    50,     0,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,     0,    38,    39,     0,
       0,     0,     0,    42,    43,    44,    45,     0,   214,    46,
       0,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,    48,    18,     0,    19,     0,   109,     0,     0,    50,
       0,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,     0,    38,    39,
       0,     0,     0,     0,    42,    43,    44,    45,     0,     0,
      46,     0,     0,     0,     0,    47,     0,     0,     0,     0,
       0,   315,    48,    18,     0,    19,     0,   109,   326,     0,
      50,     0,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,     0,    38,
      39,     0,     0,     0,     0,    42,    43,    44,    45,     0,
       0,    46,     0,     0,     0,     0,    47,     0,     0,     0,
       0,     0,     0,    48,    18,     0,    19,     0,   109,     0,
       0,    50,     0,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,     0,
      38,    39,     0,     0,     0,     0,    42,    43,    44,    45,
       0,     0,    46,     0,     0,     0,     0,    47,     0,     0,
       0,     0,     0,   330,    48,    18,     0,    19,     0,   109,
     422,     0,    50,     0,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
       0,    38,    39,     0,     0,     0,     0,    42,    43,    44,
      45,     0,     0,    46,     0,     0,     0,     0,    47,     0,
       0,     0,     0,     0,     0,    48,    18,     0,    19,   453,
     109,     0,     0,    50,     0,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,     0,    38,    39,     0,     0,     0,     0,    42,    43,
      44,    45,     0,     0,    46,     0,     0,     0,     0,    47,
       0,     0,     0,     0,     0,     0,    48,    18,     0,    19,
       0,   109,   490,     0,    50,     0,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,     0,    38,    39,     0,     0,     0,     0,    42,
      43,    44,    45,     0,     0,    46,     0,     0,     0,     0,
      47,     0,     0,     0,     0,     0,     0,    48,    18,     0,
      19,   510,   109,     0,     0,    50,     0,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,     0,    38,    39,     0,     0,     0,     0,
      42,    43,    44,    45,     0,     0,    46,     0,     0,     0,
       0,    47,     0,     0,     0,     0,     0,     0,    48,    18,
       0,    19,     0,   109,   531,     0,    50,     0,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,     0,    38,    39,     0,     0,     0,
       0,    42,    43,    44,    45,     0,     0,    46,     0,     0,
       0,     0,    47,     0,     0,     0,     0,     0,     0,    48,
      18,     0,    19,     0,   109,     0,     0,    50,     0,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,     0,    38,    39,     0,     0,
       0,     0,    42,    43,    44,    45,     0,     0,    46,     0,
       0,     0,     0,    47,     0,     0,     0,   257,     0,    19,
      48,   258,     0,     0,     0,     0,   259,     0,    50,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,     0,     0,     0,     0,     0,     0,     0,    42,
      43,    44,    45,     0,     0,    46,   257,     0,    19,     0,
     444,     0,     0,     0,   260,   259,     0,    48,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,     0,     0,     0,     0,     0,     0,     0,    42,    43,
      44,    45,     0,     0,    46,   257,     0,    19,     0,   361,
       0,     0,     0,   445,   259,     0,    48,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
       0,     0,     0,     0,     0,     0,     0,    42,    43,    44,
      45,     0,     0,    46,   257,     0,    19,     0,   500,     0,
       0,     0,     0,   259,     0,    48,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,     0,
       0,     0,     0,     0,     0,     0,    42,    43,    44,    45,
       0,     0,    46,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48
  };

  const short
  parser::yycheck_[] =
  {
       3,     4,    40,    48,    64,   164,   231,   204,   242,    38,
      20,     9,    48,   169,   134,    48,   198,    20,    21,     3,
       4,     3,   204,   179,     0,     1,     4,     8,    30,     8,
       0,     1,    92,    50,   154,     1,    20,    10,    20,    10,
      11,    70,   136,   163,   269,    12,   140,    50,     6,    15,
      12,    55,    56,     0,    30,     8,    60,    16,    17,    18,
      30,    64,     8,     6,    42,     4,     1,    65,    44,    45,
      51,   131,    51,    51,    40,   120,    86,    50,    13,     8,
      15,    47,    58,    86,   120,    88,     1,   120,    58,    92,
       5,    89,    59,    59,    92,    98,    99,    59,    51,   109,
      15,   260,    86,    42,    86,    51,   109,   227,   168,     1,
     230,   267,    47,    10,   112,   297,   236,   177,    10,   301,
      55,    56,    51,    15,    12,    60,     6,     8,   131,   189,
       8,    15,    47,    27,    14,    15,    15,    52,    39,    54,
      55,    56,    57,    58,    59,    60,   371,     1,    40,    13,
     375,    15,    39,    41,    47,    47,     4,    50,   383,    13,
       8,    15,     7,    47,   224,   168,   322,    47,    47,   403,
      51,     0,   175,    51,   177,    59,     4,   180,   176,     4,
       8,    12,   185,    47,    19,    20,   189,    15,   217,   192,
     193,   189,   190,    47,    42,     0,    41,   195,     7,     1,
     359,    55,    56,    51,   242,   208,    60,     7,     7,    40,
      30,    13,   409,    15,    42,    15,    12,   373,    46,    47,
       4,   224,   225,    51,    10,   223,   229,   409,   288,     6,
       4,     8,     1,    50,     8,    10,    14,    15,    15,    12,
       9,   244,    41,   246,    40,    47,    15,    47,   277,   252,
       9,    86,     8,    55,    56,   258,    14,    15,    60,    15,
      16,    17,    18,    12,   489,   263,    14,    15,    42,    47,
      47,    40,   107,   108,   109,   110,     9,   275,    47,    12,
      51,   116,   117,    12,   119,   288,   445,   290,   286,    47,
      59,    47,    12,    32,   414,    34,    35,   417,    54,    47,
       4,   136,     6,     0,     8,   140,     0,     1,    41,     3,
      10,   309,    12,    98,    99,    14,    10,    10,    61,    12,
      40,    10,    38,    22,    23,     0,   306,   307,     0,    12,
      12,   329,   377,     7,    12,    38,     9,    51,    42,     9,
       9,    40,    41,    10,     6,     0,    61,     4,     1,   184,
      49,     9,     5,     9,    70,     9,     0,     9,   361,     5,
      10,   364,    15,     5,    50,   403,     9,    70,     9,     9,
      41,   374,     7,     7,     7,   210,   211,     9,     9,   214,
       9,     9,     5,   218,     0,   220,   221,   416,    51,     9,
      51,   389,     0,     0,    47,   424,    41,   232,   233,    52,
      12,    54,    55,    56,    57,    58,    59,    60,   243,    12,
     245,     9,   472,     9,     9,     9,   251,    10,     7,     7,
      59,    51,     9,   258,    51,    51,   125,   143,     9,   145,
     265,     0,     9,     5,   437,     5,    46,     5,   137,    10,
     143,   444,   145,     9,     9,    59,     9,   146,     7,   452,
      51,    51,   455,     0,   153,    10,     9,     9,     9,    51,
      51,     5,   161,   466,     5,   181,   473,    10,   408,   472,
     297,   301,   312,   192,   173,   480,   311,   312,   181,   340,
     315,   394,   173,   182,   183,   320,   321,   225,   548,   271,
      -1,    -1,    -1,    -1,    -1,   330,    -1,   500,    -1,   198,
     503,   217,   505,    -1,    -1,   204,   509,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   217,   231,    -1,    -1,    -1,    -1,
     518,   519,    -1,    -1,    -1,   241,   361,    -1,   231,    -1,
      -1,    -1,   367,    -1,   369,    -1,    -1,    -1,   241,   542,
     543,   376,   545,   242,    -1,   548,    -1,    -1,    -1,   248,
     249,    -1,    -1,   269,    -1,   558,   255,   256,    -1,    -1,
      -1,   277,   560,    -1,    -1,    -1,   269,    -1,    -1,    -1,
      -1,    -1,   271,    -1,   277,    -1,    -1,    -1,    -1,    -1,
      -1,   280,    -1,   282,   419,    15,    -1,    -1,    -1,    -1,
      -1,   426,    -1,    -1,    -1,    -1,    -1,    -1,   297,    -1,
      -1,   436,   301,    -1,    -1,    -1,    -1,   323,    -1,   444,
      -1,    -1,   311,   312,    -1,    -1,    -1,    47,    -1,    -1,
     323,    -1,    52,   458,    54,    55,    56,    57,    58,    59,
      60,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,   478,   479,   480,    -1,   482,    -1,    -1,
     349,   486,    -1,    -1,    39,   371,   355,    -1,    -1,   375,
      -1,    -1,    -1,    -1,    -1,   500,   382,   383,   371,    -1,
      -1,    -1,   375,    -1,    -1,    -1,    -1,    -1,    -1,   382,
     383,    -1,    -1,    -1,    -1,    -1,    -1,   522,   523,    -1,
      -1,     1,    -1,    -1,   529,     5,    -1,    -1,    -1,    -1,
     416,    -1,    -1,    -1,   403,    15,    -1,    -1,   424,    -1,
     409,    -1,   411,   416,    -1,    -1,    -1,   416,    -1,    -1,
      -1,   424,    32,    -1,    34,    35,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    -1,    47,    -1,    -1,
     439,    -1,    52,   442,    54,    55,    56,    57,    58,    59,
      60,    50,    -1,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   488,   489,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   480,    -1,     0,     1,   488,   489,     4,    -1,     6,
      -1,     8,    -1,    10,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    32,    33,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,
      47,    -1,    -1,    -1,    -1,     0,     1,    54,    -1,     4,
      -1,     6,    59,     8,    -1,    62,    11,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    32,    33,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,     0,     1,    54,
      -1,     4,     5,     6,    59,     8,     9,    62,    11,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    32,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,
      -1,    54,     0,     1,    -1,    -1,     4,    -1,     6,    62,
       8,    -1,    10,    11,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    32,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,     0,     1,    54,    -1,     4,    -1,
       6,    -1,     8,    -1,    62,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    32,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    47,    -1,    -1,    -1,    -1,     1,    -1,    54,     4,
      -1,     6,    -1,     8,     9,    -1,    62,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    32,    33,    -1,
      -1,    36,    37,    38,    39,    -1,    41,    42,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    50,    -1,    52,    53,    54,
      55,    56,    57,    58,    59,    60,     1,    62,    -1,     4,
      -1,     6,    -1,     8,     9,    -1,    -1,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    41,    42,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    50,    -1,    52,    53,    54,
      55,    56,    57,    58,    59,    60,     1,    62,    -1,     4,
       5,     6,    -1,     8,    -1,    -1,    11,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,     1,    54,
      -1,     4,     5,     6,    59,     8,    -1,    62,    11,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    32,
      33,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,
      -1,    54,    -1,    -1,    -1,     1,    59,    -1,     4,    62,
       6,    -1,     8,     9,    10,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    32,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    47,    -1,    -1,    -1,    -1,    -1,     1,    54,    -1,
       4,     5,     6,    -1,     8,    -1,    62,    11,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,     1,
      54,    -1,     4,    -1,     6,    -1,     8,     9,    62,    11,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      32,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,
      -1,     1,    54,    -1,     4,     5,     6,    -1,     8,    -1,
      62,    11,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      30,    31,    32,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,
      -1,    -1,    -1,     1,    54,    -1,     4,    -1,     6,    -1,
       8,     9,    62,    11,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    32,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,     1,    54,    -1,     4,     5,
       6,    -1,     8,    -1,    62,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    32,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    47,    -1,    -1,    -1,    -1,    -1,     1,    54,    -1,
       4,    -1,     6,    -1,     8,    -1,    62,    11,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    32,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    -1,
      54,     4,    -1,     6,     7,     8,    -1,    -1,    62,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    -1,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,    41,    42,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,
      53,    54,    -1,     4,    57,     6,     7,     8,    -1,    62,
      -1,    -1,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,    30,
      31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
      41,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,
      -1,    -1,    53,    54,    -1,     4,    57,     6,    -1,     8,
       9,    62,    -1,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    53,    54,    -1,     4,    57,     6,
      -1,     8,     9,    62,    -1,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,
      47,    -1,    -1,    -1,    -1,    -1,    53,    54,    -1,     4,
      57,     6,    -1,     8,     9,    62,    -1,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,    54,
      -1,     4,    57,     6,    -1,     8,     9,    62,    -1,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    -1,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,
      53,    54,    -1,     4,    57,     6,    -1,     8,     9,    62,
      -1,    -1,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,    30,
      31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
      -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,
      -1,    -1,    53,    54,    -1,     4,    57,     6,    -1,     8,
      -1,    62,    -1,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    32,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    50,    -1,    -1,    -1,    54,     4,    -1,     6,    -1,
       8,    -1,    -1,    62,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    53,    54,    -1,     4,    57,
       6,    -1,     8,    -1,    62,    -1,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    47,    -1,    -1,    -1,    -1,    -1,    53,    54,    -1,
       4,    -1,     6,    -1,     8,    -1,    62,    11,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    -1,
      54,     4,    -1,     6,    -1,     8,    -1,    -1,    62,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    -1,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,    41,    42,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,
      -1,    54,     4,    -1,     6,    -1,     8,    -1,    -1,    62,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,
      -1,    53,    54,     4,    -1,     6,    -1,     8,     9,    -1,
      62,    -1,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,    30,
      31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
      -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,
      -1,    -1,    -1,    54,     4,    -1,     6,    -1,     8,    -1,
      -1,    62,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,
      -1,    -1,    -1,    53,    54,     4,    -1,     6,    -1,     8,
       9,    -1,    62,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    -1,    54,     4,    -1,     6,     7,
       8,    -1,    -1,    62,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    -1,    54,     4,    -1,     6,
      -1,     8,     9,    -1,    62,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,
      47,    -1,    -1,    -1,    -1,    -1,    -1,    54,     4,    -1,
       6,     7,     8,    -1,    -1,    62,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    47,    -1,    -1,    -1,    -1,    -1,    -1,    54,     4,
      -1,     6,    -1,     8,     9,    -1,    62,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    -1,    54,
       4,    -1,     6,    -1,     8,    -1,    -1,    62,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,     4,    -1,     6,
      54,     8,    -1,    -1,    -1,    -1,    13,    -1,    62,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,     4,    -1,     6,    -1,
       8,    -1,    -1,    -1,    51,    13,    -1,    54,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,     4,    -1,     6,    -1,     8,
      -1,    -1,    -1,    51,    13,    -1,    54,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,     4,    -1,     6,    -1,     8,    -1,
      -1,    -1,    -1,    13,    -1,    54,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,     1,    30,    44,    45,    58,    64,    65,    66,    68,
      69,    70,    71,    74,     6,    73,     0,     1,     4,     6,
       8,    11,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    30,    31,
      32,    33,    36,    37,    38,    39,    42,    47,    54,    59,
      62,    67,    84,    85,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   105,   110,   119,   120,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,     1,     8,    10,    11,    88,
      89,   102,   110,   119,   143,    30,     0,     1,     3,    10,
     143,     0,    70,     7,   127,    50,    72,    10,    50,     8,
      53,   102,   103,   104,   121,     1,    41,    50,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    91,    92,   102,
     104,   110,   119,   122,   123,   102,     6,    14,   127,   127,
       6,     8,    85,    87,   105,   115,    13,   117,   118,   127,
     129,     1,    10,    40,   108,   127,    39,   135,   137,   139,
       1,    40,    59,   109,   127,   102,     1,   110,   111,     6,
      53,    54,    59,    61,    92,    93,    88,    10,   143,     8,
      51,    87,    12,    40,     8,    51,    27,    39,    39,   110,
     119,   102,    10,    11,   143,    88,   143,     0,     4,    67,
      67,     0,     7,    30,     4,   104,   104,   104,    10,   143,
      50,    12,   144,   104,    41,   104,   104,     9,    50,     9,
      12,    41,     9,    88,    10,    12,   144,   121,   127,    51,
     121,     9,    53,    57,   104,   112,   113,   114,    85,   105,
      86,    87,    12,     8,    51,     8,    51,   127,    12,    40,
     144,     8,    51,   127,    12,    40,   144,     4,     8,    13,
      51,   126,     5,    88,     7,    41,   114,     6,     8,   115,
     127,    61,    90,   102,   143,    88,     0,     9,   114,   102,
      14,   127,    14,   127,   104,   102,    88,   143,    10,   143,
      11,    89,   102,     0,   143,     0,     1,    57,    79,    80,
      81,    82,    83,    91,   127,    32,    34,    35,    77,    78,
      83,    12,    12,   102,   104,    53,   104,     7,     9,   104,
      12,    41,     8,    87,   104,   104,     9,   104,   143,    88,
      53,   102,   122,     9,   144,   102,   144,    86,   104,   104,
      12,   144,     9,   117,   104,   102,   104,   102,    10,    12,
     127,   127,    10,   104,   102,    12,    59,   127,    59,   127,
     104,     8,   126,   143,    51,   104,     7,    41,     7,    41,
     114,     9,    86,     8,    51,   116,     8,    54,   127,   136,
       0,   143,    87,     9,   127,   127,     9,   143,     9,    88,
       9,   102,     0,    80,     4,    81,     5,     1,    55,    56,
      60,    75,    76,   142,     1,    76,     1,    76,    10,   143,
       5,    32,    50,   104,   106,   107,   127,   106,   104,    50,
       9,   104,     9,   104,     9,   114,    41,     9,     9,   143,
       7,     7,   112,     9,     9,   127,     8,    51,    10,    12,
       9,   127,    12,    59,     8,    51,   126,     9,   104,     5,
     102,     7,    51,     7,   104,    51,   104,     7,    41,    86,
       9,   114,   102,    86,   104,     0,    51,    86,     9,     0,
     143,     0,   111,    12,   117,    77,    83,   127,    47,    50,
      12,   144,    46,   127,   144,   104,    41,     9,    87,     9,
       9,   104,     9,    10,   104,   102,   127,    59,   127,   104,
       8,   126,     9,    51,   102,    51,     7,   102,     7,    51,
       7,   104,     9,     9,   102,     0,     9,     5,    29,    88,
      75,     5,     8,    51,   104,   104,   107,     5,   104,    46,
       5,     9,   104,    86,     9,     9,    10,    59,     9,   104,
     102,   102,    51,    51,   102,    51,     7,     0,   143,   143,
     104,   104,   104,     9,     9,   102,   102,   102,    51,     5,
      88,     5,     9,   102,   143,     5
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    63,    64,    64,    64,    64,    64,    64,    65,    65,
      65,    66,    66,    66,    66,    66,    66,    66,    66,    67,
      67,    67,    67,    67,    68,    68,    68,    68,    68,    69,
      69,    70,    70,    70,    71,    71,    72,    72,    73,    73,
      73,    74,    75,    76,    76,    77,    77,    77,    77,    77,
      77,    78,    78,    79,    79,    80,    80,    80,    80,    81,
      81,    81,    82,    82,    83,    83,    84,    84,    85,    85,
      86,    86,    87,    87,    87,    87,    88,    88,    88,    88,
      88,    89,    89,    89,    90,    90,    90,    91,    91,    91,
      91,    91,    91,    91,    91,    92,    92,    93,    93,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    95,    95,    95,    96,    96,    96,    97,    97,
      97,    98,    98,    98,    99,    99,    99,    99,    99,   100,
     100,   100,   100,   100,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   103,   103,   104,   105,
     105,   106,   106,   107,   107,   107,   107,   107,   107,   107,
     108,   108,   108,   108,   108,   108,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     111,   111,   111,   112,   112,   112,   113,   113,   114,   115,
     116,   116,   116,   117,   117,   117,   117,   117,   118,   118,
     119,   120,   120,   120,   120,   120,   120,   120,   120,   121,
     121,   121,   121,   121,   121,   121,   122,   122,   123,   123,
     123,   124,   125,   125,   126,   126,   126,   127,   127,   128,
     128,   128,   128,   129,   129,   129,   130,   131,   132,   132,
     133,   133,   134,   134,   135,   135,   135,   136,   136,   137,
     137,   137,   137,   138,   138,   139,   139,   140,   140,   141,
     141,   141,   141,   141,   141,   142,   142,   142,   142,   143,
     143,   144,   144
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     3,     2,     1,     1,     2,     0,     5,     6,
       4,     5,     4,     7,     6,     4,     3,     8,     6,     1,
       2,     5,     3,     6,     2,     2,     3,     3,     2,     1,
       2,     1,     1,     1,     8,     6,     0,     2,     0,     3,
       2,     5,     2,     1,     3,     2,     2,     2,     2,     2,
       2,     1,     3,     1,     1,     6,     4,     8,     6,     1,
       1,     2,     1,     2,     0,     1,     1,     1,     5,     3,
       0,     1,     1,     1,     2,     2,     1,     3,     3,     4,
       2,     1,     1,     1,     2,     2,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     7,
       6,     4,     5,     4,     2,     5,     4,     5,     5,     2,
       3,     4,     7,     6,     6,     9,     8,     8,     6,     5,
       5,     8,     7,     7,     4,     5,     7,     6,     4,     6,
       8,     7,     5,     5,     1,     1,     1,     1,     1,     3,
       2,     2,     4,     3,     1,     1,     1,     1,     1,     1,
       1,     5,     4,     3,     5,     1,     2,     3,     4,     6,
       3,     4,     6,     5,     7,     6,     1,     3,     2,     7,
       7,     1,     3,     3,     4,     4,     5,     1,     3,     3,
       1,     3,     4,     3,     5,     6,     2,     1,     3,     4,
       5,     4,     3,     5,     6,     7,     2,     4,     5,     7,
       4,     6,     2,     2,     2,     4,     5,     7,     4,     6,
       0,     1,     1,     2,     2,     1,     1,     3,     2,     3,
       0,     2,     3,     1,     3,     4,     3,     4,     1,     3,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       1,     3,     2,     3,     5,     4,     3,     2,     0,     1,
       3,     4,     4,     5,     1,     1,     1,     1,     1,     2,
       3,     3,     4,     1,     3,     4,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     2,     1,
       1,     2,     2,     1,     2,     0,     1,     1,     1,     0,
       1,     0,     1
  };




#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   243,   243,   244,   245,   246,   247,   248,   256,   259,
     265,   276,   279,   282,   285,   288,   291,   294,   300,   309,
     312,   314,   317,   320,   328,   331,   337,   340,   343,   360,
     362,   367,   368,   369,   383,   386,   394,   395,   399,   400,
     401,   405,   414,   426,   428,   433,   435,   437,   439,   449,
     459,   472,   474,   484,   485,   489,   492,   495,   498,   504,
     505,   516,   528,   529,   533,   534,   543,   544,   548,   550,
     555,   556,   560,   561,   562,   563,   567,   568,   569,   574,
     579,   587,   588,   589,   608,   609,   610,   614,   615,   616,
     617,   618,   619,   620,   621,   625,   626,   630,   632,   637,
     640,   643,   647,   650,   653,   656,   659,   662,   672,   679,
     684,   690,   705,   712,   719,   728,   735,   742,   752,   758,
     764,   773,   779,   785,   794,   799,   804,   809,   814,   822,
     827,   833,   838,   843,   856,   859,   862,   865,   868,   871,
     878,   886,   889,   892,   895,   898,   901,   907,   908,   909,
     910,   912,   915,   918,   921,   924,   925,   927,   929,   932,
     935,   938,   944,   947,   950,   953,   961,   962,   966,   974,
     977,   982,   984,   989,   991,   993,   995,   997,   999,  1001,
    1014,  1016,  1021,  1024,  1027,  1032,  1038,  1040,  1042,  1044,
    1046,  1048,  1050,  1052,  1054,  1056,  1061,  1062,  1063,  1065,
    1067,  1072,  1081,  1093,  1105,  1107,  1109,  1111,  1113,  1118,
    1130,  1131,  1134,  1144,  1145,  1146,  1150,  1151,  1154,  1156,
    1159,  1160,  1162,  1171,  1173,  1175,  1181,  1183,  1191,  1193,
    1197,  1205,  1206,  1207,  1208,  1209,  1210,  1211,  1212,  1216,
    1218,  1220,  1222,  1224,  1226,  1228,  1236,  1238,  1243,  1245,
    1247,  1252,  1257,  1259,  1268,  1269,  1270,  1278,  1279,  1284,
    1286,  1288,  1290,  1294,  1296,  1298,  1309,  1311,  1314,  1315,
    1319,  1320,  1324,  1325,  1329,  1330,  1331,  1335,  1336,  1341,
    1342,  1343,  1344,  1348,  1350,  1355,  1357,  1362,  1364,  1369,
    1370,  1371,  1372,  1373,  1374,  1378,  1379,  1380,  1381,  1393,
    1393,  1394,  1394
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

#line 68 "langutils/sc_parser/src/sc_grammar.y"
} } } // sc::ast::parser
#line 4085 "langutils/sc_parser/src/sc_grammar_parser.cpp"

#line 1396 "langutils/sc_parser/src/sc_grammar.y"

