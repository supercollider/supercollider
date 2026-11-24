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

  case 33: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY members semicolon.opt method.list.opt CLOSECURLY
#line 373 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassIndex) = cxt.create(Class{ cxt.take_version(yystack_[4].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[7].location), (yystack_[6].value.v_maybe_NamedIdentifierIndex), (yystack_[5].value.v_maybe_ClassNameIdentifierIndex), (yystack_[3].value.v_BigMemberListIndex), (yystack_[1].value.v_MethodListIndex)); }
#line 841 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 34: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY method.list.opt CLOSECURLY
#line 376 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto no_members = cxt.create(BigMemberList{}, sc::src::SourceCodeRange { yystack_[2].location.end, yystack_[2].location.end });
			(yylhs.value.v_ClassIndex) = cxt.create(Class{ cxt.take_version(yystack_[2].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[5].location), (yystack_[4].value.v_maybe_NamedIdentifierIndex), (yystack_[3].value.v_maybe_ClassNameIdentifierIndex), no_members, (yystack_[1].value.v_MethodListIndex));
		}
#line 850 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 35: // class.super.opt: %empty
#line 383 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_maybe_ClassNameIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 856 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 36: // class.super.opt: COLON CLASSNAME
#line 384 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_maybe_ClassNameIdentifierIndex) = cxt.create(ClassNameIdentifier{}, yystack_[0].location); }
#line 862 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 37: // class.slot.opt: %empty
#line 388 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_maybe_NamedIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 868 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 38: // class.slot.opt: OPENSQUARE name CLOSESQUARE
#line 389 "langutils/sc_parser/src/sc_grammar.y"
                                      { (yylhs.value.v_maybe_NamedIdentifierIndex) = (yystack_[1].value.v_NamedIdentifierIndex); }
#line 874 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 39: // class.slot.opt: OPENSQUARE CLOSESQUARE
#line 390 "langutils/sc_parser/src/sc_grammar.y"
                                 { (yylhs.value.v_maybe_NamedIdentifierIndex) = cxt.create(Missing{}, empty_after(yystack_[1].location)); }
#line 880 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 40: // class.extension: ADD CLASSNAME OPENCURLY method.list.opt CLOSECURLY
#line 395 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassExtensionIndex) = cxt.create(ClassExtension{ cxt.take_version(yystack_[2].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[3].location), (yystack_[1].value.v_MethodListIndex)); }
#line 886 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 41: // members.entry.item: accessor variable_declarations.list.item
#line 404 "langutils/sc_parser/src/sc_grammar.y"
                {
			// An accessor that isn't written is empty, at the end of the keyword before it, so it moves to the name.
			if (yystack_[1].location.begin == yystack_[1].location.end) {
				yylhs.location = yystack_[0].location;
				cxt.graph->location((yystack_[1].value.v_RWAccessorIndex)) = empty_before(yystack_[0].location);
			}
			(yylhs.value.v_DeclareMemberIndex) = cxt.create(DeclareMember{}, yylhs.location, (yystack_[1].value.v_RWAccessorIndex), (yystack_[0].value.v_DeclareAnyVariableIndex));
		}
#line 899 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 42: // members.entry.list: members.entry.item
#line 416 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberListIndex) = cxt.create(DeclareMemberList{}, yylhs.location, (yystack_[0].value.v_DeclareMemberIndex)); }
#line 905 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 43: // members.entry.list: members.entry.list COMMA members.entry.item
#line 418 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberListIndex) = cxt.graph->append((yystack_[2].value.v_DeclareMemberListIndex), yylhs.location, (yystack_[0].value.v_DeclareMemberIndex)); }
#line 911 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 44: // members.entry: CLASSVAR members.entry.list
#line 423 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberEntryIndex) = cxt.graph->cast<DeclareClassMemberList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 917 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 45: // members.entry: VAR members.entry.list
#line 425 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberEntryIndex) = cxt.graph->cast<DeclareMemberList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 923 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 46: // members.entry: CONST members.entry.list
#line 427 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberEntryIndex) = cxt.graph->cast<DeclareConstList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 929 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 47: // members.entry: CLASSVAR error
#line 429 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips to the declaration's `;`, or the next method.
			const auto error = error_recovery::member(cxt, yystack_[1].location, yystack_[0].location);
			if (!error)
				YYABORT;
			(yylhs.value.v_BigMemberEntryIndex) = *error;
			yyclearin;
			yyerrok; // Recovered at the next item, so report its error.
		}
#line 943 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 48: // members.entry: VAR error
#line 439 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips to the declaration's `;`, or the next method.
			const auto error = error_recovery::member(cxt, yystack_[1].location, yystack_[0].location);
			if (!error)
				YYABORT;
			(yylhs.value.v_BigMemberEntryIndex) = *error;
			yyclearin;
			yyerrok; // Recovered at the next item, so report its error.
		}
#line 957 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 49: // members.entry: CONST error
#line 449 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips to the declaration's `;`, or the next method.
			const auto error = error_recovery::member(cxt, yystack_[1].location, yystack_[0].location);
			if (!error)
				YYABORT;
			(yylhs.value.v_BigMemberEntryIndex) = *error;
			yyclearin;
			yyerrok; // Recovered at the next item, so report its error.
		}
#line 971 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 50: // members: members.entry
#line 462 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberListIndex) = cxt.create(BigMemberList{}, yylhs.location, (yystack_[0].value.v_BigMemberEntryIndex));}
#line 977 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 51: // members: members SEMICOLON members.entry
#line 464 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberListIndex) = cxt.graph->append((yystack_[2].value.v_BigMemberListIndex), yylhs.location, (yystack_[0].value.v_BigMemberEntryIndex)); }
#line 983 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 52: // method.name: name
#line 473 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 989 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 53: // method.name: binary_op.raw
#line 474 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 995 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 54: // method.base: method.name OPENCURLY argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 479 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(Missing{}, empty_after(yystack_[3].location)), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 1001 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 55: // method.base: method.name OPENCURLY argument_declarations.opt CLOSECURLY
#line 482 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[3].value.v_SelectorIndex), (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(Missing{}, empty_after(yystack_[1].location)), cxt.create(BlockContentsList{}, empty_after(yystack_[1].location))); }
#line 1007 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 56: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt block.contents semicolon.opt CLOSECURLY
#line 485 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[7].value.v_SelectorIndex), (yystack_[5].value.v_DeclareArgumentListIndex), cxt.create(PrimitiveIdentifier{}, yystack_[4].location), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 1013 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 57: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt CLOSECURLY
#line 488 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(PrimitiveIdentifier{}, yystack_[2].location), cxt.create(BlockContentsList{}, empty_after(yystack_[1].location))); }
#line 1019 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 58: // method: method.base
#line 493 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_AnyMethodIndex) = (yystack_[0].value.v_AnyMethodIndex); }
#line 1025 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 59: // method: error
#line 495 "langutils/sc_parser/src/sc_grammar.y"
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
#line 1040 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 60: // method: MULTIPLY method.base
#line 506 "langutils/sc_parser/src/sc_grammar.y"
                { 
			if (auto c = cxt.graph->as_a<MethodIndex>((yystack_[0].value.v_AnyMethodIndex))) {
				(yylhs.value.v_AnyMethodIndex) = cxt.graph->cast<ClassMethod>(*c);
			} 
			// leaves as error
			cxt.graph->location((yystack_[0].value.v_AnyMethodIndex)) = yylhs.location; 
			(yylhs.value.v_AnyMethodIndex) = (yystack_[0].value.v_AnyMethodIndex); 
		}
#line 1053 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 61: // method.list: method
#line 517 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_MethodListIndex) = cxt.create(MethodList{}, yylhs.location, (yystack_[0].value.v_AnyMethodIndex)); }
#line 1059 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 62: // method.list: method.list method
#line 518 "langutils/sc_parser/src/sc_grammar.y"
                             { (yylhs.value.v_MethodListIndex) = cxt.graph->append((yystack_[1].value.v_MethodListIndex), yylhs.location, (yystack_[0].value.v_AnyMethodIndex)); }
#line 1065 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 63: // method.list.opt: %empty
#line 522 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_MethodListIndex) = cxt.create(MethodList{}, yylhs.location); }
#line 1071 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 64: // method.list.opt: method.list
#line 523 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_MethodListIndex) = (yystack_[0].value.v_MethodListIndex); }
#line 1077 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 65: // block.open: OPENCURLY
#line 532 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_bool) = false; }
#line 1083 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 66: // block.open: BEGINCLOSEDFUNC
#line 533 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_bool) = true; }
#line 1089 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 67: // block: block.open argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 538 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockIndex) = cxt.create(BlockNode{ (yystack_[4].value.v_bool) }, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 1095 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 68: // block: block.open argument_declarations.opt CLOSECURLY
#line 540 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockIndex) = cxt.create(BlockNode{ (yystack_[2].value.v_bool) }, yylhs.location, (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, empty_after(yystack_[1].location))); }
#line 1101 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 69: // block.opt_list: %empty
#line 544 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location); }
#line 1107 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 70: // block.opt_list: block.list
#line 545 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_BlockListIndex) = (yystack_[0].value.v_BlockListIndex); }
#line 1113 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 71: // block.list: block
#line 549 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location, (yystack_[0].value.v_BlockIndex)); }
#line 1119 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 72: // block.list: generator
#line 550 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location, (yystack_[0].value.v_GeneratorAnyIndex)); }
#line 1125 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 73: // block.list: block.list block
#line 551 "langutils/sc_parser/src/sc_grammar.y"
                           { (yylhs.value.v_BlockListIndex) = cxt.graph->append((yystack_[1].value.v_BlockListIndex), yylhs.location, (yystack_[0].value.v_BlockIndex)); }
#line 1131 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 74: // block.list: block.list generator
#line 552 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_BlockListIndex) = cxt.graph->append((yystack_[1].value.v_BlockListIndex), yylhs.location, (yystack_[0].value.v_GeneratorAnyIndex)); }
#line 1137 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 75: // block.contents: block.contents.item
#line 556 "langutils/sc_parser/src/sc_grammar.y"
                              { (yylhs.value.v_BlockContentsListIndex) = cxt.create(BlockContentsList{}, yylhs.location, (yystack_[0].value.v_BlockItemIndex)); }
#line 1143 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 76: // block.contents: block.contents SEMICOLON block.contents.item
#line 557 "langutils/sc_parser/src/sc_grammar.y"
                                                       { (yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[2].value.v_BlockContentsListIndex), yylhs.location, (yystack_[0].value.v_BlockItemIndex)); }
#line 1149 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 77: // block.contents: block.contents NONLOCALRETURN expr
#line 559 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[2].value.v_BlockContentsListIndex), yylhs.location, non_local); 
		}
#line 1158 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 78: // block.contents: block.contents SEMICOLON NONLOCALRETURN expr
#line 564 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[3].value.v_BlockContentsListIndex), yylhs.location, non_local); 
		}
#line 1167 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 79: // block.contents: NONLOCALRETURN expr
#line 569 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.create(BlockContentsList{}, yylhs.location, non_local);
		}
#line 1176 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 80: // block.contents.item: expr
#line 576 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_BlockItemIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1182 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 81: // block.contents.item: variable_declarations
#line 577 "langutils/sc_parser/src/sc_grammar.y"
                                { (yylhs.value.v_BlockItemIndex) = (yystack_[0].value.v_DeclareVariableListIndex); }
#line 1188 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 82: // block.contents.item: error
#line 579 "langutils/sc_parser/src/sc_grammar.y"
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
#line 1203 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 83: // adverb: DOT name
#line 597 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_AdverbIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 1209 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 84: // adverb: DOT integer
#line 598 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_AdverbIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 1215 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 85: // adverb: DOT OPENPAREN expr_or_seq CLOSEPAREN
#line 599 "langutils/sc_parser/src/sc_grammar.y"
                                               { (yylhs.value.v_AdverbIndex) = cxt.create(AdverbExprNode{}, yylhs.location, (yystack_[1].value.v_AnyExprIndex));  }
#line 1221 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 86: // binary_op.raw: BINOP
#line 603 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1227 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 87: // binary_op.raw: READWRITEVAR
#line 604 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1233 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 88: // binary_op.raw: LESSTHAN
#line 605 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1239 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 89: // binary_op.raw: GREATERTHAN
#line 606 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1245 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 90: // binary_op.raw: MINUS
#line 607 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1251 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 91: // binary_op.raw: MULTIPLY
#line 608 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1257 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 92: // binary_op.raw: ADD
#line 609 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1263 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 93: // binary_op.raw: PIPE
#line 610 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1269 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 94: // binary_op.no_adverb: binary_op.raw
#line 614 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 1275 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 95: // binary_op.no_adverb: KEYBINOP
#line 615 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ true }, yylhs.location); }
#line 1281 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 96: // binary_op: binary_op.no_adverb adverb
#line 620 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorMaybeAdverbIndex) = cxt.create(SelectorWAdverb{}, yylhs.location, (yystack_[1].value.v_SelectorIndex), (yystack_[0].value.v_AdverbIndex)); }
#line 1287 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 97: // binary_op: binary_op.no_adverb
#line 622 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorMaybeAdverbIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 1293 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 98: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN arguments CLOSEPAREN block.opt_list
#line 627 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[2].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1299 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 99: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN CLOSEPAREN block.list
#line 630 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[4].value.v_SelectorIndex), cxt.create(ArgumentList{}, empty_after(yystack_[2].location)), (yystack_[0].value.v_BlockListIndex)); }
#line 1305 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 100: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN block.list
#line 633 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[2].value.v_SelectorIndex), cxt.create(ArgumentList{}, empty_after(yystack_[1].location)), (yystack_[0].value.v_BlockListIndex)); }
#line 1311 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 101: // msgsend: name OPENPAREN arguments CLOSEPAREN block.opt_list
#line 637 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[2].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1317 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 102: // msgsend: name OPENPAREN CLOSEPAREN block.list
#line 640 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), cxt.create(ArgumentList{}, empty_after(yystack_[2].location)), (yystack_[0].value.v_BlockListIndex)); }
#line 1323 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 103: // msgsend: name block.list
#line 643 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[1].value.v_NamedIdentifierIndex), cxt.create(ArgumentList{}, empty_after(yystack_[1].location)), (yystack_[0].value.v_BlockListIndex)); }
#line 1329 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 104: // msgsend: expr DOT name arguments.maybe_paren block.opt_list
#line 646 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PostfixMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1335 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 105: // msgsend: expr DOT arguments.paren block.opt_list
#line 649 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(ValueMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1341 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 106: // msgsend: expr DOT OPENPAREN CLOSEPAREN block.opt_list
#line 652 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(ValueMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), cxt.create(ArgumentList{}, empty_after(yystack_[2].location)), (yystack_[0].value.v_BlockListIndex)); }
#line 1347 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 107: // msgsend: CLASSNAME OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 662 "langutils/sc_parser/src/sc_grammar.y"
                {
			// The array includes its brackets, as every other array does, see literal.array.
			cxt.graph->location(*(yystack_[2].value.v_ArrayIndex)) = range(yystack_[3].location, yystack_[0].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(CollectionNode{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[4].location), (yystack_[2].value.v_ArrayIndex));
		}
#line 1357 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 108: // msgsend: CLASSNAME block.list
#line 669 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[1].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, empty_after(yystack_[1].location)), (yystack_[0].value.v_BlockListIndex));
		}
#line 1366 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 109: // msgsend: CLASSNAME arguments.paren block.opt_list
#line 674 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[2].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex));
		}
#line 1375 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 110: // msgsend: CLASSNAME OPENPAREN CLOSEPAREN block.opt_list
#line 680 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[3].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, empty_after(yystack_[2].location)), (yystack_[0].value.v_BlockListIndex));
		}
#line 1384 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 111: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 695 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[3].value.v_ArgumentListIndex), (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r);
		}
#line 1394 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 112: // expr.dotrange: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 702 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, empty_after(yystack_[3].location)),  (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1404 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 113: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 709 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[2].value.v_ArgumentListIndex), cxt.create(Missing{}, empty_after(yystack_[1].location)));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1414 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 114: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 718 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[5].value.v_ArgumentListIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[8].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1424 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 115: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 725 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[4].value.v_ArgumentListIndex), cxt.create(Missing{}, empty_after(yystack_[3].location)));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1434 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 116: // expr.dotrange.assign: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 732 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, empty_after(yystack_[5].location)),  (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1444 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 117: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 742 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[3].value.v_ArgumentListIndex), (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1454 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 118: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 748 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[2].value.v_ArgumentListIndex), cxt.create(Missing{}, empty_after(yystack_[1].location)));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), r);	
		}
#line 1464 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 119: // expr.base.range: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 754 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, empty_after(yystack_[3].location)),  (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), r);	
		}
#line 1474 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 120: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 763 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[5].value.v_ArgumentListIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1484 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 121: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 769 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[4].value.v_ArgumentListIndex), cxt.create(Missing{}, empty_after(yystack_[3].location)));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1494 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 122: // expr.base.range.assign: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 775 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, empty_after(yystack_[5].location)),  (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1504 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 123: // range: OPENPAREN expr_or_seq DOTDOT CLOSEPAREN
#line 784 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, cxt.create(Missing{}, empty_after(yystack_[1].location)));
		}
#line 1513 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 124: // range: OPENPAREN expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 789 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[4].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1522 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 125: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 794 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[6].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1531 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 126: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 799 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[4].location, yystack_[2].location), (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[5].location, yystack_[0].location), args, cxt.create(Missing{}, empty_after(yystack_[1].location)));
		}
#line 1540 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 127: // range: OPENPAREN DOTDOT expr_or_seq CLOSEPAREN
#line 804 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, empty_after(yystack_[3].location));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1549 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 128: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 812 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[5].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1558 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 129: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 817 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[7].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1567 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 130: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 823 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[4].location, yystack_[2].location), (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[6].location, yystack_[0].location), args, cxt.create(Missing{}, empty_after(yystack_[1].location)));
		}
#line 1576 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 131: // range.generator: OPENPAREN COLON DOTDOT expr_or_seq CLOSEPAREN
#line 828 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, empty_after(yystack_[3].location));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1585 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 132: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT CLOSEPAREN
#line 833 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, cxt.create(Missing{}, empty_after(yystack_[1].location)));
		}
#line 1594 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 133: // expr.base: literal
#line 846 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyPossiblyLiteralIndex); }
#line 1600 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 134: // expr.base: generator
#line 849 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_GeneratorAnyIndex); }
#line 1606 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 135: // expr.base: name
#line 852 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 1612 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 136: // expr.base: CURRYARG
#line 855 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyExprIndex) = cxt.create(CurryNode{}, yystack_[0].location); }
#line 1618 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 137: // expr.base: msgsend
#line 858 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1624 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 138: // expr.base: OPENPAREN expr_or_seq CLOSEPAREN
#line 861 "langutils/sc_parser/src/sc_grammar.y"
                { 
			// A nasty hack because of nil assignment.
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AnyExprIndex) = (yystack_[1].value.v_AnyExprIndex);
		}
#line 1634 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 139: // expr.base: OPENPAREN error
#line 868 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto close = error_recovery::bracket(cxt, yystack_[1].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create_error(range(yystack_[1].location, close), yystack_[0].location);
			yyclearin; // The lookahead was skipped.
			yyerrok; // Recovered at the matching bracket, so report the next error.
		}
#line 1645 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 140: // expr.base: TILDE name
#line 876 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(EnvIdentifierNode{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1651 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 141: // expr.base: expr.base OPENSQUARE arguments CLOSESQUARE
#line 879 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex) ); }
#line 1657 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 142: // expr.base: expr.base OPENSQUARE CLOSESQUARE
#line 882 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), cxt.create(ArgumentList{}, empty_after(yystack_[1].location))); }
#line 1663 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 143: // expr.base: expr.base.range
#line 885 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AtMessageIndex); }
#line 1669 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 144: // expr.base: range
#line 888 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeIndex); }
#line 1675 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 145: // expr.base: range.generator
#line 891 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeGeneratorIndex); }
#line 1681 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 146: // expr: expr.base
#line 896 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1687 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 147: // expr: expr.base.range.assign
#line 897 "langutils/sc_parser/src/sc_grammar.y"
                                 { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeAssignIndex); }
#line 1693 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 148: // expr: expr.dotrange
#line 898 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AtMessageIndex); }
#line 1699 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 149: // expr: expr.dotrange.assign
#line 899 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeAssignIndex); }
#line 1705 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 150: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE
#line 902 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex) ); }
#line 1711 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 151: // expr: expr DOT OPENSQUARE CLOSESQUARE
#line 905 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), cxt.create(ArgumentList{}, empty_after(yystack_[1].location)) ); }
#line 1717 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 152: // expr: expr binary_op expr
#line 908 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(BinaryMessage{}, yylhs.location, (yystack_[1].value.v_SelectorMaybeAdverbIndex), (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1723 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 153: // expr: expr DOT name EQUALSSIGN expr
#line 911 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(SetterNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1729 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 154: // expr: CLASSNAME
#line 913 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_AnyExprIndex) = cxt.create(ClassNameIdentifier{}, yylhs.location); }
#line 1735 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 155: // expr: BACKTICK expr
#line 914 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyExprIndex) = cxt.create(ReferenceNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1741 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 156: // expr: name EQUALSSIGN expr
#line 916 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentNode{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1747 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 157: // expr: TILDE name EQUALSSIGN expr
#line 919 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentNode{AssignmentNode::Target::Environment}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1753 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 158: // expr: name OPENPAREN arguments CLOSEPAREN EQUALSSIGN expr
#line 922 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(SetterNode{}, yylhs.location, (yystack_[3].value.v_ArgumentListIndex), (yystack_[5].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1759 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 159: // expr: name.pack EQUALSSIGN expr
#line 925 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[2].value.v_NamePackIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1765 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 160: // expr: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 928 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[3].value.v_NamePackIndex), (yystack_[1].value.v_AnyExprIndex)); }
#line 1771 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 161: // expr: expr.base OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 934 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_ArgumentListIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1777 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 162: // expr: expr.base OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 937 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), cxt.create(ArgumentList{}, empty_after(yystack_[3].location)), (yystack_[0].value.v_AnyExprIndex)); }
#line 1783 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 163: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 940 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), (yystack_[3].value.v_ArgumentListIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1789 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 164: // expr: expr DOT OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 943 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), cxt.create(ArgumentList{}, empty_after(yystack_[3].location)), (yystack_[0].value.v_AnyExprIndex)); }
#line 1795 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 165: // expr_or_seq.base: expr
#line 950 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1801 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 166: // expr_or_seq.base: expr_or_seq.base SEMICOLON expr
#line 951 "langutils/sc_parser/src/sc_grammar.y"
                                          { (yylhs.value.v_AnyExprIndex) = cxt.create(ExprSeq{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1807 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 167: // expr_or_seq: expr_or_seq.base semicolon.opt
#line 955 "langutils/sc_parser/src/sc_grammar.y"
                                       { (yylhs.value.v_AnyExprIndex) = (yystack_[1].value.v_AnyExprIndex); }
#line 1813 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 168: // generator: OPENCURLY COLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 964 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorAnyIndex) = cxt.create(GeneratorNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_GeneratorQualListIndex)); }
#line 1819 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 169: // generator: OPENCURLY SEMICOLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 967 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorAnyIndex) = cxt.create(GeneratorFakeNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_GeneratorQualListIndex)); }
#line 1825 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 170: // generator.qual.list: generator.qual
#line 972 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualListIndex) = cxt.create(GeneratorQualList{}, yylhs.location, (yystack_[0].value.v_GeneratorQualAnyIndex)); }
#line 1831 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 171: // generator.qual.list: generator.qual.list COMMA generator.qual
#line 974 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualListIndex) = cxt.graph->append((yystack_[2].value.v_GeneratorQualListIndex), yylhs.location, (yystack_[0].value.v_GeneratorQualAnyIndex)); }
#line 1837 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 172: // generator.qual: name LEFTARROW expr_or_seq
#line 979 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualAssign{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), cxt.create(Missing{}, empty_after(yystack_[2].location)), (yystack_[0].value.v_AnyExprIndex)); }
#line 1843 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 173: // generator.qual: name name LEFTARROW expr_or_seq
#line 981 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualAssign{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1849 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 174: // generator.qual: VAR name EQUALSSIGN expr_or_seq
#line 983 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualDeclare{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1855 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 175: // generator.qual: VAR name OPENPAREN expr_or_seq CLOSEPAREN
#line 985 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualDeclare{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex)); }
#line 1861 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 176: // generator.qual: expr_or_seq
#line 987 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualIf{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1867 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 177: // generator.qual: COLON COLON expr_or_seq
#line 989 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualSideEffect{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1873 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 178: // generator.qual: COLON WHILE expr_or_seq
#line 991 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualGuard{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1879 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 179: // argument_declarations.list: name
#line 1004 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1885 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 180: // argument_declarations.list: name EQUALSSIGN expr
#line 1006 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed((yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{is_bracketed}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex))); 
		}
#line 1894 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 181: // argument_declarations.list: name OPENPAREN expr_or_seq CLOSEPAREN
#line 1011 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1900 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 182: // argument_declarations.list: argument_declarations.list COMMA name
#line 1014 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[2].value.v_DeclareArgumentListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1906 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 183: // argument_declarations.list: argument_declarations.list COMMA name EQUALSSIGN expr
#line 1017 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed((yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[4].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{is_bracketed}, range(yystack_[2].location, yystack_[0].location), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)));
		}
#line 1915 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 184: // argument_declarations.list: argument_declarations.list COMMA name OPENPAREN expr_or_seq CLOSEPAREN
#line 1022 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[3].location, yystack_[0].location), (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1921 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 185: // argument_declarations.pipelist: name literal
#line 1028 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, (yystack_[1].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1927 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 186: // argument_declarations.pipelist: name
#line 1030 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1933 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 187: // argument_declarations.pipelist: name EQUALSSIGN literal
#line 1032 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1939 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 188: // argument_declarations.pipelist: name OPENPAREN expr_or_seq CLOSEPAREN
#line 1034 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1945 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 189: // argument_declarations.pipelist: name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 1036 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1951 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 190: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name literal
#line 1038 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[3].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[1].location, yystack_[0].location), (yystack_[1].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1957 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 191: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name
#line 1040 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[2].value.v_DeclareArgumentListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1963 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 192: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN literal
#line 1042 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[4].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[2].location, yystack_[0].location), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1969 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 193: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name OPENPAREN expr_or_seq CLOSEPAREN
#line 1044 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[3].location, yystack_[0].location), (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1975 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 194: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 1046 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[6].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[4].location, yystack_[0].location), (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1981 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 195: // argument_declarations: ARG SEMICOLON
#line 1050 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, empty_after(yystack_[1].location)); }
#line 1987 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 196: // argument_declarations: ARG argument_declarations.list comma.opt SEMICOLON
#line 1051 "langutils/sc_parser/src/sc_grammar.y"
                                                             { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[2].value.v_DeclareArgumentListIndex); }
#line 1993 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 197: // argument_declarations: ARG argument_declarations.list ELLIPSIS name SEMICOLON
#line 1053 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[3].value.v_DeclareArgumentListIndex), range(yystack_[3].location, yystack_[1].location), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 1999 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 198: // argument_declarations: ARG argument_declarations.list ELLIPSIS name COMMA name SEMICOLON
#line 1055 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[5].value.v_DeclareArgumentListIndex), range(yystack_[5].location, yystack_[1].location), cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 2005 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 199: // argument_declarations: ARG ELLIPSIS name SEMICOLON
#line 1057 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, range(yystack_[2].location, yystack_[1].location));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(arg_list, range(yystack_[2].location, yystack_[1].location), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); 
		}
#line 2014 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 200: // argument_declarations: ARG ELLIPSIS name COMMA name SEMICOLON
#line 1062 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, range(yystack_[4].location, yystack_[1].location));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(
				arg_list, range(yystack_[4].location, yystack_[1].location),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))
			); 
		}
#line 2027 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 201: // argument_declarations: ARG error
#line 1071 "langutils/sc_parser/src/sc_grammar.y"
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
#line 2043 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 202: // argument_declarations: PIPE error
#line 1083 "langutils/sc_parser/src/sc_grammar.y"
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
#line 2059 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 203: // argument_declarations: PIPE PIPE
#line 1095 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, empty_after(yystack_[1].location)); }
#line 2065 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 204: // argument_declarations: PIPE argument_declarations.pipelist comma.opt PIPE
#line 1097 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[2].value.v_DeclareArgumentListIndex); }
#line 2071 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 205: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name PIPE
#line 1099 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[3].value.v_DeclareArgumentListIndex), range(yystack_[3].location, yystack_[1].location), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 2077 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 206: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name COMMA name PIPE
#line 1101 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[5].value.v_DeclareArgumentListIndex), range(yystack_[5].location, yystack_[1].location), cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 2083 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 207: // argument_declarations: PIPE ELLIPSIS name PIPE
#line 1103 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, range(yystack_[2].location, yystack_[1].location));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(arg_list, range(yystack_[2].location, yystack_[1].location), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); 
		}
#line 2092 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 208: // argument_declarations: PIPE ELLIPSIS name COMMA name PIPE
#line 1108 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, range(yystack_[4].location, yystack_[1].location));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(
				arg_list, range(yystack_[4].location, yystack_[1].location),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))
			); 
		}
#line 2105 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 209: // argument_declarations.opt: %empty
#line 1119 "langutils/sc_parser/src/sc_grammar.y"
                                       { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 2111 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 210: // argument_declarations.opt: argument_declarations
#line 1120 "langutils/sc_parser/src/sc_grammar.y"
                                { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[0].value.v_DeclareArgumentListIndex); }
#line 2117 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 211: // argument_declarations.opt: error
#line 1124 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Nothing is written, so it is empty, before the error.
			yylhs.location = empty_before(yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location);
		}
#line 2127 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 212: // arguments.entries: KEYBINOP expr_or_seq
#line 1133 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_ArgumentEntryIndex) = cxt.create(KwArgNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2133 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 213: // arguments.entries: MULTIPLY expr_or_seq
#line 1134 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_ArgumentEntryIndex) = cxt.create(VariadicArgNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2139 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 214: // arguments.entries: expr_or_seq
#line 1135 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_ArgumentEntryIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 2145 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 215: // arguments.no_trailing: arguments.entries
#line 1139 "langutils/sc_parser/src/sc_grammar.y"
                            {  (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location, (yystack_[0].value.v_ArgumentEntryIndex)); }
#line 2151 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 216: // arguments.no_trailing: arguments.no_trailing COMMA arguments.entries
#line 1140 "langutils/sc_parser/src/sc_grammar.y"
                                                        { (yylhs.value.v_ArgumentListIndex) = cxt.graph->append((yystack_[2].value.v_ArgumentListIndex), yylhs.location, (yystack_[0].value.v_ArgumentEntryIndex)); }
#line 2157 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 217: // arguments: arguments.no_trailing comma.opt
#line 1143 "langutils/sc_parser/src/sc_grammar.y"
                                            { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2163 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 218: // arguments.paren: OPENPAREN arguments CLOSEPAREN
#line 1145 "langutils/sc_parser/src/sc_grammar.y"
                                                 { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2169 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 219: // arguments.maybe_paren: %empty
#line 1148 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location); }
#line 2175 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 220: // arguments.maybe_paren: OPENPAREN CLOSEPAREN
#line 1150 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, empty_after(yystack_[1].location)); }
#line 2181 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 221: // arguments.maybe_paren: OPENPAREN arguments CLOSEPAREN
#line 1152 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2187 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 222: // variable_declarations.list.item: name
#line 1161 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 2193 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 223: // variable_declarations.list.item: name EQUALSSIGN expr
#line 1163 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2199 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 224: // variable_declarations.list.item: name OPENPAREN expr_or_seq CLOSEPAREN
#line 1165 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Bracketed, as in the old compiler, so it isn't a literal, see ConstEval::is_legacy_literal.
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex));
		}
#line 2209 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 225: // variable_declarations.list.item: name.pack EQUALSSIGN expr
#line 1171 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[2].value.v_NamePackIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2215 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 226: // variable_declarations.list.item: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 1173 "langutils/sc_parser/src/sc_grammar.y"
                {
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[3].value.v_NamePackIndex), (yystack_[1].value.v_AnyExprIndex));
		}
#line 2224 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 227: // variable_declarations.list: variable_declarations.list.item
#line 1181 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareVariableListIndex) = cxt.create(DeclareVariableList{}, yylhs.location, (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 2230 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 228: // variable_declarations.list: variable_declarations.list COMMA variable_declarations.list.item
#line 1183 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareVariableListIndex) = cxt.graph->append((yystack_[2].value.v_DeclareVariableListIndex), yylhs.location, (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 2236 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 229: // variable_declarations: VAR variable_declarations.list
#line 1186 "langutils/sc_parser/src/sc_grammar.y"
                                                       { (yylhs.value.v_DeclareVariableListIndex) = (yystack_[0].value.v_DeclareVariableListIndex); }
#line 2242 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 230: // literal.terminal: symbol
#line 1194 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_AnySymbolIndex); }
#line 2248 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 231: // literal.terminal: string
#line 1195 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_StringLitIndex); }
#line 2254 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 232: // literal.terminal: integer
#line 1196 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 2260 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 233: // literal.terminal: float
#line 1197 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_FloatProducingIndex); }
#line 2266 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 234: // literal.terminal: boolean
#line 1198 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_BooleanLitIndex); }
#line 2272 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 235: // literal.terminal: nil
#line 1199 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_NilLitIndex); }
#line 2278 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 236: // literal.terminal: ascii
#line 1200 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_ASCIIIndex); }
#line 2284 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 237: // literal.terminal: block
#line 1201 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_BlockIndex); }
#line 2290 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 238: // literal.array.contents: %empty
#line 1206 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location); }
#line 2296 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 239: // literal.array.contents: expr_or_seq
#line 1208 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2302 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 240: // literal.array.contents: expr_or_seq COLON expr_or_seq
#line 1210 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2308 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 241: // literal.array.contents: KEYBINOP expr_or_seq
#line 1212 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2314 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 242: // literal.array.contents: literal.array.contents COMMA expr_or_seq
#line 1214 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[2].value.v_ArrayIndex), yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2320 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 243: // literal.array.contents: literal.array.contents COMMA expr_or_seq COLON expr_or_seq
#line 1216 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[4].value.v_ArrayIndex), yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2326 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 244: // literal.array.contents: literal.array.contents COMMA KEYBINOP expr_or_seq
#line 1218 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[3].value.v_ArrayIndex), yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2332 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 245: // literal.dictionary.entry: expr COLON expr_or_seq
#line 1226 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryEntryIndex) = cxt.create(DictionaryEntryNode{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2338 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 246: // literal.dictionary.entry: KEYBINOP expr_or_seq
#line 1228 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryEntryIndex) = cxt.create(DictionaryEntryNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2344 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 247: // literal.dictionary.entries: %empty
#line 1233 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.create(DictionaryNode{}, yylhs.location); }
#line 2350 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 248: // literal.dictionary.entries: literal.dictionary.entry
#line 1235 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.create(DictionaryNode{}, yylhs.location, (yystack_[0].value.v_DictionaryEntryIndex)); }
#line 2356 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 249: // literal.dictionary.entries: literal.dictionary.entries COMMA literal.dictionary.entry
#line 1237 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.graph->append((yystack_[2].value.v_DictionaryIndex), yylhs.location,  (yystack_[0].value.v_DictionaryEntryIndex)); }
#line 2362 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 250: // literal.dictionary: OPENPAREN literal.dictionary.entries comma.opt CLOSEPAREN
#line 1242 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->location(*(yystack_[2].value.v_DictionaryIndex)) = yylhs.location; (yylhs.value.v_DictionaryIndex) = (yystack_[2].value.v_DictionaryIndex); }
#line 2368 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 251: // literal.array: OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1247 "langutils/sc_parser/src/sc_grammar.y"
                {  cxt.graph->location(*(yystack_[2].value.v_ArrayIndex)) = yylhs.location; (yylhs.value.v_ArrayIndex) = (yystack_[2].value.v_ArrayIndex); }
#line 2374 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 252: // literal.array: HASH OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1249 "langutils/sc_parser/src/sc_grammar.y"
                {  
			cxt.graph->payload((yystack_[2].value.v_ArrayIndex)).is_immutable = true;
			cxt.graph->location(*(yystack_[2].value.v_ArrayIndex)) = yylhs.location;
			(yylhs.value.v_ArrayIndex) = (yystack_[2].value.v_ArrayIndex);
		}
#line 2384 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 253: // literal: literal.terminal
#line 1257 "langutils/sc_parser/src/sc_grammar.y"
                           { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_AnyPossiblyLiteralIndex); }
#line 2390 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 254: // literal: literal.array
#line 1258 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_ArrayIndex); }
#line 2396 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 255: // literal: literal.dictionary
#line 1259 "langutils/sc_parser/src/sc_grammar.y"
                             { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_DictionaryIndex); }
#line 2402 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 256: // name: NAME
#line 1267 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_NamedIdentifierIndex) = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 2408 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 257: // name: WHILE
#line 1268 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamedIdentifierIndex) = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 2414 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 258: // name.list: HASH name
#line 1274 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.create(NameList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2420 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 259: // name.list: HASH TILDE name
#line 1276 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.create(NameList{}, yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2426 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 260: // name.list: name.list COMMA name
#line 1278 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.graph->append((yystack_[2].value.v_NameListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2432 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 261: // name.list: name.list COMMA TILDE name
#line 1280 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.graph->append((yystack_[3].value.v_NameListIndex), yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2438 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 262: // name.pack: name.list
#line 1284 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[0].value.v_NameListIndex), cxt.create(Missing{}, empty_after(yystack_[0].location))); }
#line 2444 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 263: // name.pack: name.list ELLIPSIS name
#line 1286 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[2].value.v_NameListIndex), (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2450 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 264: // name.pack: name.list ELLIPSIS TILDE name
#line 1288 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[3].value.v_NameListIndex), cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2456 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 265: // ascii: ASCII
#line 1298 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_ASCIIIndex) = cxt.create(ASCIINode{}, yylhs.location); }
#line 2462 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 266: // nil: NIL
#line 1300 "langutils/sc_parser/src/sc_grammar.y"
          { (yylhs.value.v_NilLitIndex) = cxt.create(NilNode{}, yylhs.location); }
#line 2468 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 267: // boolean: TRUE
#line 1303 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_BooleanLitIndex) = cxt.create(BooleanNode{true}, yylhs.location); }
#line 2474 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 268: // boolean: FALSE
#line 1304 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BooleanLitIndex) = cxt.create(BooleanNode{false}, yylhs.location); }
#line 2480 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 269: // symbol: SYMBOL_QUOTE
#line 1308 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_AnySymbolIndex) = cxt.create(SymbolQuote{}, yylhs.location); }
#line 2486 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 270: // symbol: SYMBOL_SLASH
#line 1309 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_AnySymbolIndex) = cxt.create(SymbolSlash{}, yylhs.location); }
#line 2492 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 271: // string: STRINGLINE
#line 1313 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_StringLitIndex) = cxt.create(StringLineList{}, yylhs.location, cxt.create(StringLineNode{}, yylhs.location)); }
#line 2498 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 272: // string: string STRINGLINE
#line 1314 "langutils/sc_parser/src/sc_grammar.y"
                            { (yylhs.value.v_StringLitIndex) = cxt.graph->append((yystack_[1].value.v_StringLitIndex), yylhs.location, cxt.create(StringLineNode{}, yystack_[0].location)); }
#line 2504 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 273: // integer.unsigned: INTEGER
#line 1318 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{}, yylhs.location); }
#line 2510 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 274: // integer.unsigned: INTEGER_RADIX
#line 1319 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{IntNode::Kind::Radix}, yylhs.location); }
#line 2516 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 275: // integer.unsigned: HEXADECIMAL
#line 1320 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{IntNode::Kind::Hexadecimal}, yylhs.location); }
#line 2522 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 276: // integer: integer.unsigned
#line 1324 "langutils/sc_parser/src/sc_grammar.y"
                           { (yylhs.value.v_IntLitIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 2528 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 277: // integer: MINUS integer.unsigned
#line 1326 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload((yystack_[0].value.v_IntLitIndex)).sign = IntNode::Sign::Negative; (yylhs.value.v_IntLitIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 2534 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 278: // float.raw_unsigned: FLOAT
#line 1330 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{}, yylhs.location); }
#line 2540 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 279: // float.raw_unsigned: FLOAT_RADIX
#line 1331 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Radix}, yylhs.location); }
#line 2546 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 280: // float.raw_unsigned: FLOAT_EXPONENT
#line 1332 "langutils/sc_parser/src/sc_grammar.y"
                         { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Exponent}, yylhs.location); }
#line 2552 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 281: // float.raw_unsigned: FLOAT_INF
#line 1333 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Inf}, yylhs.location); }
#line 2558 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 282: // float.raw: float.raw_unsigned
#line 1338 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FloatLitIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2564 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 283: // float.raw: MINUS float.raw_unsigned
#line 1340 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload((yystack_[0].value.v_FloatLitIndex)).sign = FloatNode::Sign::Negative; (yylhs.value.v_FloatLitIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2570 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 284: // accidental.unsigned: ACCIDENTAL_STEPS
#line 1345 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = cxt.create(AccidentalNode{AccidentalNode::Kind::Steps}, yylhs.location); }
#line 2576 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 285: // accidental.unsigned: ACCIDENTAL_CENTS
#line 1347 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = cxt.create(AccidentalNode{AccidentalNode::Kind::Cents}, yylhs.location); }
#line 2582 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 286: // accidental: accidental.unsigned
#line 1352 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2588 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 287: // accidental: MINUS accidental.unsigned
#line 1354 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload((yystack_[0].value.v_AccidentalIndex)).sign = AccidentalNode::Sign::Negative; (yylhs.value.v_AccidentalIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2594 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 288: // float: float.raw
#line 1358 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_FloatProducingIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2600 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 289: // float: accidental
#line 1359 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_FloatProducingIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2606 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 290: // float: float.raw PI
#line 1360 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, (yystack_[1].value.v_FloatLitIndex)); }
#line 2612 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 291: // float: integer PI
#line 1361 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, (yystack_[1].value.v_IntLitIndex)); }
#line 2618 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 292: // float: PI
#line 1362 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, cxt.create(Missing{}, empty_before(yystack_[0].location))); }
#line 2624 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 293: // float: MINUS PI
#line 1363 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{PiNode::Sign::Negative}, yylhs.location, cxt.create(Missing{}, empty_before(yystack_[0].location))); }
#line 2630 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 294: // accessor: %empty
#line 1367 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::Private}, yylhs.location); }
#line 2636 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 295: // accessor: LESSTHAN
#line 1368 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyReadable}, yylhs.location); }
#line 2642 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 296: // accessor: GREATERTHAN
#line 1369 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyWriteable}, yylhs.location); }
#line 2648 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 297: // accessor: READWRITEVAR
#line 1371 "langutils/sc_parser/src/sc_grammar.y"
                { 
			(yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{
				combine<ReadWriteAccessor::ExternallyReadable, ReadWriteAccessor::ExternallyWriteable>()
			}, yylhs.location); 
		}
#line 2658 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;


#line 2662 "langutils/sc_parser/src/sc_grammar_parser.cpp"

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




  const short parser::yypact_ninf_ = -234;

  const short parser::yytable_ninf_ = -300;

  const short
  parser::yypact_[] =
  {
     259,    25,   835,   783,    23,    48,  -234,  -234,   292,    24,
    -234,  -234,  -234,   161,     6,  -234,  -234,   121,  2104,  1045,
    2666,   148,    12,  -234,  -234,  -234,  -234,  -234,  -234,  -234,
    -234,  -234,  -234,  -234,  -234,  -234,  -234,   145,  -234,   168,
     108,  -234,  -234,  -234,  -234,  -234,  -234,   615,    34,  2666,
    -234,  1217,  -234,  -234,  -234,  -234,  -234,  -234,  -234,  -234,
     120,   674,  -234,  1586,   142,  -234,  -234,  -234,  -234,    21,
     167,    52,  -234,  -234,  -234,  -234,    31,  -234,   136,  -234,
     170,  -234,  -234,  -234,  -234,  1045,  -234,  2666,    29,  -234,
     674,   942,  -234,   220,   225,  -234,  -234,  2156,  2156,   224,
    -234,  -234,  -234,   249,   208,   264,  2666,  2666,  1105,  2666,
     674,   266,   233,   268,  -234,  2666,  2207,  -234,  2666,   615,
    -234,  -234,  -234,  -234,   231,  -234,  -234,   287,   701,   238,
    1378,   289,  -234,   288,   674,  2104,    12,  -234,   256,  2104,
    1741,  -234,   156,  -234,   156,   216,  -234,   307,    54,    62,
    -234,  -234,    12,   227,    71,  -234,  -234,  -234,  -234,  -234,
      12,  -234,   230,  2713,  -234,  -234,  -234,  1430,  1637,  -234,
    -234,  -234,   188,   260,  2666,    29,   994,   310,  1793,  2666,
     156,   219,   222,  2666,  2666,  -234,  -234,  -234,  1274,   315,
     674,   887,  2666,   320,    29,   326,  -234,    85,  -234,  -234,
    -234,  -234,  -234,   690,   316,   317,  -234,  2666,  -234,  2666,
    2258,   323,   318,  2666,    16,  -234,   215,  2666,  -234,  2666,
    2309,  -234,    29,  1586,  2360,   324,   268,  -234,  2666,   268,
     156,  2666,  2666,  -234,  -234,   322,   327,  -234,  -234,  -234,
     156,   168,  2666,  2666,  2666,  2666,   206,    12,    12,   328,
    2666,  2666,    33,  -234,    12,    18,  -234,  2360,   331,  2791,
    -234,  -234,    29,   293,  2666,   173,  1689,  1845,   156,   116,
     244,  -234,   278,   340,    29,  -234,   156,   334,   674,    12,
    -234,    12,  -234,   336,   674,    29,   337,  1482,   338,  2666,
    -234,   674,  -234,   351,  -234,  -234,   570,   349,  -234,  -234,
     347,   350,  -234,  -234,    65,   146,   198,  -234,   346,   352,
    2001,  2001,   674,  -234,  2666,   309,  -234,  -234,   354,  2666,
    2411,  1897,   156,  -234,   319,  -234,   355,   357,    29,  2666,
     701,  -234,  -234,   361,   674,   362,  -234,  -234,  -234,  2052,
    -234,  -234,  -234,   363,   674,   364,   674,  -234,    12,   119,
     306,  -234,   365,   674,    12,  -234,    39,  -234,  2752,   368,
    2360,  -234,   373,  2666,   374,   329,  2462,   333,  2666,   203,
     156,  -234,  1949,  2666,   156,  2666,   296,  -234,  -234,  -234,
     370,   156,    46,  -234,  -234,  -234,   379,   382,    29,   389,
     674,  -234,  -234,  1165,  -234,  -234,  -234,  -234,  -234,  -234,
    -234,   378,   168,  -234,   378,  -234,   378,   274,    85,  -234,
      12,     5,  -234,   380,  -234,    22,   380,  -234,  2666,  -234,
     359,  -234,   384,   156,   387,  2513,  -234,  -234,   400,  -234,
    -234,  -234,  -234,  -234,   401,  2666,  2666,  -234,    12,  -234,
     356,    12,  -234,  2360,  2830,  -234,  -234,   404,  -234,   674,
     366,  2666,   367,   407,  2666,   412,   369,  2564,  -234,  -234,
     413,   674,  -234,   415,  -234,  2666,  -234,   421,  -234,   416,
    -234,  1326,    78,  -234,  -234,   423,   122,  2666,  2666,  2001,
     426,  2666,   386,   428,  -234,  2615,  -234,   156,   156,  -234,
     427,  -234,  -234,   430,   674,   425,  -234,   385,   434,  2360,
    -234,  -234,  2666,   674,  2666,   394,   674,   395,  2666,   397,
     443,  -234,  -234,   674,  -234,   437,  -234,   441,    29,  -234,
    -234,  2666,  2666,  -234,  -234,  -234,  -234,  -234,  2666,  -234,
    -234,   444,  -234,  -234,  -234,  -234,  -234,  -234,   446,   674,
     674,  2666,  2666,   674,  2666,   405,  -234,  1534,   452,   449,
    -234,  -234,  -234,  -234,   674,   674,   674,  2666,  -234,    29,
    -234,  -234,   674,   454,  -234
  };

  const short
  parser::yydefact_[] =
  {
       7,    37,     0,     0,     0,     0,     4,     5,     0,     0,
      29,    31,    32,     0,    35,     3,    25,    65,   238,     0,
       0,     0,     0,   256,   273,   274,   275,   278,   279,   280,
     281,   284,   285,   269,   270,   271,   265,   154,   136,     0,
       0,   266,   267,   268,   292,    66,   257,     0,     0,     0,
      24,     0,   237,   137,   148,   149,   143,   147,   144,   145,
     146,    19,   134,     0,   298,   253,   255,   254,   133,   135,
     262,     0,   236,   235,   234,   230,   231,   276,   232,   282,
     288,   286,   289,   233,    82,     0,   299,     0,   298,    75,
      80,     0,    81,     0,     0,     1,    28,     0,   299,     0,
       6,    30,    39,     0,     0,     0,     0,     0,     0,     0,
     165,   298,   239,   300,   139,     0,     0,    86,    95,    90,
      88,    89,    91,    92,     0,    87,    94,     0,   165,     0,
       0,     0,   248,   300,    20,   238,     0,   258,   140,   238,
       0,    71,   108,    72,    69,     0,   227,   229,   222,     0,
     201,   195,     0,   300,   179,   293,   277,   283,   287,   202,
       0,   203,   300,   186,   155,   211,   210,     0,     0,    95,
      90,    93,     0,    97,     0,   298,     0,     0,     0,     0,
     103,     0,     0,     0,     0,   272,   291,   290,     0,   298,
      79,     0,     0,     0,   298,     0,    16,     0,    27,    26,
       2,    38,    36,     0,     0,     0,   241,   299,   167,     0,
     301,     0,     0,     0,     0,   246,     0,     0,   138,     0,
       0,    22,   298,     0,   301,     0,   300,   259,     0,   300,
      69,     0,     0,   214,   215,   300,     0,    73,    74,   109,
      70,     0,     0,     0,     0,     0,     0,   301,     0,     0,
       0,     0,     0,   301,     0,     0,    65,   247,     0,     0,
     185,    68,   298,   142,     0,     0,     0,     0,    69,   219,
       0,    96,   152,     0,   298,    10,     0,     0,   156,     0,
     260,     0,   263,     0,   159,   298,     0,     0,     0,     0,
      76,    77,    15,     0,    12,    59,    91,     0,    58,    61,
       0,     0,    53,    52,     0,     0,     0,    50,   298,     0,
       0,     0,   166,   240,     0,   242,   251,   127,     0,     0,
       0,     0,   100,   245,     0,   123,     0,     0,   298,     0,
       0,   249,   250,     0,   157,     0,   110,   212,   213,   301,
     217,   218,   228,     0,   223,     0,   225,   199,     0,   182,
       0,   196,     0,   180,     0,   207,     0,   204,   191,     0,
     247,   187,     0,     0,     0,   141,     0,   151,     0,     0,
      69,   105,     0,     0,    69,     0,     0,    83,    84,     8,
       0,   102,    69,   261,   264,   160,     0,     0,   298,     0,
      78,    11,    60,     0,    62,    40,    48,   295,   296,   297,
      42,    45,     0,    47,    44,    49,    46,   299,     0,    34,
       0,     0,   176,   300,   170,   135,   300,   244,     0,   131,
       0,   132,     0,     0,     0,     0,   124,    21,     0,   252,
     107,   216,   224,   226,     0,     0,     0,   197,     0,   181,
       0,     0,   205,   247,     0,   190,   188,     0,    67,   162,
     119,     0,   118,     0,     0,     0,   150,     0,   106,   220,
       0,   153,   104,     0,     9,     0,   101,     0,    14,     0,
      18,     0,   294,    41,    51,     0,     0,     0,     0,   301,
       0,     0,     0,     0,   243,     0,   128,    99,    69,   126,
       0,    23,   200,     0,   183,     0,   208,     0,     0,   247,
     192,   189,     0,   161,     0,   117,   164,   112,     0,   113,
       0,   221,    85,   158,    13,     0,    55,   298,   298,    43,
      33,     0,     0,   178,   177,   171,   169,   172,     0,   168,
     130,     0,    98,   125,   184,   198,   206,   193,     0,   122,
     121,     0,     0,   163,     0,   111,    17,     0,     0,     0,
     174,   173,   129,   194,   120,   116,   115,     0,    57,   298,
      54,   175,   114,     0,    56
  };

  const short
  parser::yypgoto_[] =
  {
    -234,  -234,  -234,  -234,    38,  -234,  -234,   451,  -234,  -234,
    -234,  -234,   -11,  -149,    57,  -234,  -234,   171,   172,  -234,
    -196,  -234,   285,  -224,   -28,   -59,   275,  -234,  -181,    -9,
    -234,  -234,  -234,  -234,  -234,  -234,  -234,  -234,  -234,    -2,
    -234,   166,   298,   157,    -8,  -234,  -234,    17,    77,   134,
    -234,  -155,   303,  -234,  -233,  -234,    19,  -234,   -93,   255,
    -234,  -234,  -234,  -158,   302,  -234,   -37,  -234,  -234,  -234,
    -234,  -234,   -44,   211,   -35,  -234,   -32,  -234,  -234,  -234,
       3,  -119
  };

  const short
  parser::yydefgoto_[] =
  {
       0,     5,     6,     7,    50,     8,     9,    10,    11,   105,
      14,    12,   400,   401,   307,   308,   297,   298,   299,   300,
     301,    51,    52,   239,   240,    88,    89,   271,   126,   173,
     174,    53,    54,    55,    56,    57,    58,    59,    60,   110,
     111,   233,    62,   413,   414,   153,   162,   166,   167,   234,
     235,   236,   144,   374,   146,   147,    92,    65,   113,   132,
     133,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,   402,
      93,   211
  };

  const short
  parser::yytable_[] =
  {
      61,    90,   149,   156,   175,   260,   336,   309,   342,   142,
     127,    99,   157,   265,   225,   158,   302,   128,   134,    63,
      91,    64,   302,   277,   100,    17,    17,    23,   319,   178,
     178,    13,   194,    23,   249,   159,   130,    23,   131,   191,
     192,   180,   226,   255,   371,   354,   229,   164,    95,    23,
      17,   441,   477,    94,     1,   478,   104,   320,   185,    46,
     183,    90,   242,    45,    45,    46,   396,   177,   481,    46,
     244,   222,   179,   179,   160,   156,   127,   357,  -294,   250,
    -294,    46,     4,   128,   157,   190,   295,   158,    45,    90,
     -63,   193,   355,   161,   195,    61,    61,   465,   442,   127,
      23,   361,   188,   184,   189,   243,   128,   333,   262,   150,
     335,   369,  -294,   245,   208,   302,   340,   274,   151,   302,
     397,   398,   251,    23,   372,   399,   168,   435,    90,   285,
     521,   106,    46,   397,   398,   198,   199,   117,   399,   170,
     120,   121,   296,   123,   171,   125,   458,   403,   152,    17,
     462,   139,   176,   140,   135,    46,   404,   406,   466,  -294,
      17,  -294,   136,    23,   328,    90,   424,   373,   102,   473,
     436,   107,   272,   522,    90,   186,    23,   278,   273,   181,
     365,   145,   284,    23,   112,   129,    90,    45,   322,    90,
     291,   286,   288,  -294,   266,    46,   267,   293,    45,   405,
     445,   397,   398,    23,   149,   312,   399,   182,    46,   187,
     456,  -294,   475,  -294,   366,    46,   347,   460,   348,    17,
     196,    90,   330,   321,   200,   327,   334,   302,   388,   197,
     136,    23,   159,   279,    23,    46,   281,    23,   202,   247,
     -93,   344,   253,   346,   457,  -294,    23,   218,   381,   353,
     219,   129,   375,   397,   398,   128,   201,    45,   399,    23,
      24,    25,    26,    46,   532,   362,    46,   248,   203,    46,
     254,   160,   204,   205,   129,   206,   207,   380,    46,   220,
     210,   212,   214,   209,   215,    90,   500,   390,   386,     1,
     161,    46,  -298,    96,   480,    97,   216,   483,   376,   223,
     224,   112,    98,     2,     3,   112,   304,   228,   305,   306,
     275,   408,    24,    25,    26,   103,   437,     4,   438,   241,
     292,   270,   141,   137,   138,   287,   294,   317,   310,   311,
     316,   428,   156,   332,   339,   143,   341,   135,   351,   172,
     379,   148,   154,   382,   363,   385,   387,   389,   295,   283,
     163,   391,   -64,   393,   141,   395,   407,   409,   128,   418,
     425,   449,    23,   419,   426,   149,   427,   143,   429,   430,
     464,   461,   432,   433,   439,   313,   315,   446,   448,   318,
     451,   450,   468,   323,   454,   324,   326,   180,   467,   470,
     472,   469,   479,   486,    46,   487,   488,   337,   338,   117,
     485,   170,   120,   121,   296,   123,   171,   125,   343,   491,
     345,   492,   518,   501,   505,   496,   352,   502,   504,   507,
     508,   514,   511,   359,   512,   515,   163,   237,   520,   141,
     364,   526,   528,   529,   494,   535,   533,   546,   227,   534,
     238,   128,   143,   537,   536,   541,   542,   137,   544,   503,
     545,    86,   506,   552,   246,   553,   557,   560,   561,   564,
     101,   519,   252,   513,   474,   237,   290,   392,   416,    90,
     471,   525,   394,   431,   269,   268,   412,   412,   238,   331,
     417,   378,     0,   280,   282,   420,   422,     0,   559,     0,
       0,     0,     0,     0,     0,   215,     0,   128,     0,   303,
     539,   141,   540,     0,     0,   303,   543,     0,     0,     0,
       0,     0,     0,     0,   143,   141,     0,     0,     0,     0,
     547,   548,     0,     0,     0,   237,   447,     0,   143,     0,
       0,     0,   453,     0,   455,     0,     0,     0,   238,   554,
     555,   463,   556,   148,     0,    90,     0,     0,     0,   349,
     350,     0,     0,   141,     0,   562,   356,   358,     0,     0,
       0,   141,   563,     0,     0,     0,   143,     0,     0,     0,
       0,     0,   377,     0,   143,     0,     0,     0,     0,     0,
       0,   383,     0,   384,   484,    23,     0,     0,     0,     0,
       0,   490,     0,     0,     0,     0,     0,     0,   303,     0,
       0,   493,   303,     0,     0,     0,     0,   237,     0,   498,
       0,     0,   415,   415,     0,     0,     0,    46,     0,     0,
     238,     0,   117,   510,   170,   120,   121,   122,   123,   171,
     125,    24,    25,    26,    27,    28,    29,    30,    31,    32,
       0,     0,     0,   523,   524,   412,     0,   527,     0,     0,
     434,   531,     0,     0,   155,   141,   440,     0,     0,   141,
       0,     0,     0,     0,     0,   538,   237,   141,   143,     0,
       0,     0,   143,     0,     0,     0,     0,     0,     0,   238,
     143,     0,     0,     0,     0,     0,     0,   549,   550,     0,
       0,   295,     0,     0,   551,   -63,     0,     0,     0,     0,
     141,     0,     0,     0,   148,    23,     0,     0,   141,     0,
     303,     0,   476,   143,     0,     0,     0,   482,     0,     0,
       0,   143,   304,     0,   305,   306,   117,   169,   170,   120,
     121,   122,   123,   171,   125,   172,     0,    46,     0,     0,
     495,     0,   117,   497,   170,   120,   121,   296,   123,   171,
     125,   217,     0,   117,   169,   170,   120,   121,   122,   123,
     171,   125,   172,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   237,   141,     0,     0,     0,     0,     0,     0,
       0,   415,     0,  -298,    84,   238,   143,    17,     0,    18,
       0,    85,     0,    86,    87,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,    37,    38,    39,    40,     0,     0,    41,
      42,    43,    44,     0,     0,    45,     0,     0,     0,     0,
      46,     0,     0,     0,     0,    15,    16,    47,     0,    17,
       0,    18,    48,    19,     0,    49,    20,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,    39,    40,     0,
       0,    41,    42,    43,    44,     0,     0,    45,     0,     0,
       0,     0,    46,     0,     0,     0,     0,  -299,    84,    47,
       0,    17,  -299,    18,    48,   108,  -299,    49,   289,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,    39,
       0,     0,     0,    41,    42,    43,    44,     0,     0,    45,
       0,     0,     0,     0,    46,     0,     0,     0,     0,     0,
       0,    47,  -298,    84,     0,     0,    17,     0,    18,    49,
     108,     0,    86,    87,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,    39,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,  -299,    84,    47,     0,    17,     0,
      18,     0,   108,     0,    49,    87,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,    39,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,   114,     0,    47,    17,
       0,    18,     0,   108,  -247,     0,    49,  -247,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,    39,    40,     0,
       0,    41,    42,    43,    44,     0,   115,    45,     0,     0,
       0,     0,    46,     0,     0,   116,     0,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   114,    49,     0,    17,
       0,    18,     0,   108,  -247,     0,     0,  -247,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,     0,     0,     0,
       0,    41,    42,    43,    44,     0,   115,    45,     0,     0,
       0,     0,    46,     0,     0,   116,     0,   117,   118,   119,
     120,   121,   122,   123,   171,   125,   165,    49,     0,  -209,
    -209,  -209,     0,  -209,     0,     0,  -209,     0,  -209,  -209,
    -209,  -209,  -209,  -209,  -209,  -209,  -209,  -209,  -209,  -209,
    -209,  -209,  -209,  -209,  -209,  -209,  -209,  -209,    40,     0,
       0,  -209,  -209,  -209,  -209,     0,     0,  -209,     0,     0,
       0,     0,  -209,     0,     0,     0,     0,     0,   165,  -209,
       0,  -209,  -209,  -209,    48,  -209,     0,  -209,  -209,     0,
    -209,  -209,  -209,  -209,  -209,  -209,  -209,  -209,  -209,  -209,
    -209,  -209,  -209,  -209,  -209,  -209,     0,  -209,  -209,  -209,
      40,     0,     0,  -209,  -209,  -209,  -209,     0,     0,  -209,
       0,     0,     0,     0,  -209,     0,     0,     0,     0,     0,
       0,  -209,     0,     0,     0,    84,    48,     0,    17,  -209,
      18,     0,   108,  -298,    86,    87,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,    39,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,     0,    84,    47,     0,
      17,   516,    18,     0,   108,     0,    49,    87,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,   517,    37,    38,    39,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,    84,
      47,     0,    17,     0,    18,     0,   108,   221,    49,    87,
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     0,    37,    38,
      39,     0,     0,     0,    41,    42,    43,    44,     0,     0,
      45,     0,     0,     0,     0,    46,     0,     0,     0,     0,
       0,    84,    47,     0,    17,   261,    18,     0,   108,     0,
      49,    87,     0,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
      37,    38,    39,     0,     0,     0,    41,    42,    43,    44,
       0,     0,    45,     0,     0,     0,     0,    46,     0,     0,
       0,     0,     0,    84,    47,     0,    17,     0,    18,     0,
     108,  -299,    49,    87,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,    39,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,    84,    47,     0,    17,   558,
      18,     0,   108,     0,    49,    87,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,    39,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,     0,    84,    47,     0,
      17,     0,    18,     0,   108,     0,    49,    87,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,    39,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,     0,
      47,    17,     0,    18,   263,   108,     0,     0,    49,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,     0,
       0,     0,     0,    41,    42,    43,    44,     0,   264,    45,
       0,     0,     0,     0,    46,     0,     0,     0,     0,     0,
     231,    47,     0,    17,   232,    18,   367,   108,     0,    49,
       0,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,     0,    37,
      38,     0,     0,     0,     0,    41,    42,    43,    44,     0,
     368,    45,     0,     0,     0,     0,    46,     0,     0,     0,
       0,     0,   231,    47,     0,    17,   232,    18,     0,   108,
     230,    49,     0,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,   231,    47,     0,    17,   232,    18,
       0,   108,   276,    49,     0,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,    37,    38,     0,     0,     0,     0,    41,
      42,    43,    44,     0,     0,    45,     0,     0,     0,     0,
      46,     0,     0,     0,     0,     0,   231,    47,     0,    17,
     232,    18,     0,   108,   370,    49,     0,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,     0,     0,     0,
       0,    41,    42,    43,    44,     0,     0,    45,     0,     0,
       0,     0,    46,     0,     0,     0,     0,     0,   231,    47,
       0,    17,   232,    18,     0,   108,   423,    49,     0,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,     0,
       0,     0,     0,    41,    42,    43,    44,     0,     0,    45,
       0,     0,     0,     0,    46,     0,     0,     0,     0,     0,
     231,    47,     0,    17,   232,    18,     0,   108,   459,    49,
       0,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,     0,    37,
      38,     0,     0,     0,     0,    41,    42,    43,    44,     0,
       0,    45,     0,     0,     0,     0,    46,     0,     0,     0,
       0,     0,   231,    47,     0,    17,   232,    18,     0,   108,
       0,    49,     0,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,   410,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,   411,     0,     0,     0,    47,    17,     0,    18,     0,
     108,     0,     0,    49,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,   231,    47,     0,    17,   232,
      18,     0,   108,     0,    49,     0,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,     0,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,     0,   109,    47,     0,
      17,     0,    18,     0,    19,     0,    49,    20,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,     0,
      47,    17,     0,    18,     0,   108,     0,     0,    49,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,     0,
       0,     0,     0,    41,    42,    43,    44,     0,   213,    45,
       0,     0,     0,     0,    46,     0,     0,     0,     0,     0,
       0,    47,    17,     0,    18,     0,   108,     0,     0,    49,
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     0,    37,    38,
       0,     0,     0,     0,    41,    42,    43,    44,     0,     0,
      45,     0,     0,     0,     0,    46,     0,     0,     0,     0,
       0,   314,    47,    17,     0,    18,     0,   108,   325,     0,
      49,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,     0,    37,
      38,     0,     0,     0,     0,    41,    42,    43,    44,     0,
       0,    45,     0,     0,     0,     0,    46,     0,     0,     0,
       0,     0,     0,    47,    17,     0,    18,     0,   108,     0,
       0,    49,     0,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
      37,    38,     0,     0,     0,     0,    41,    42,    43,    44,
       0,     0,    45,     0,     0,     0,     0,    46,     0,     0,
       0,     0,     0,   329,    47,    17,     0,    18,     0,   108,
     421,     0,    49,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,     0,    47,    17,     0,    18,   452,
     108,     0,     0,    49,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,     0,    47,    17,     0,    18,
       0,   108,   489,     0,    49,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,    37,    38,     0,     0,     0,     0,    41,
      42,    43,    44,     0,     0,    45,     0,     0,     0,     0,
      46,     0,     0,     0,     0,     0,     0,    47,    17,     0,
      18,   509,   108,     0,     0,    49,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,     0,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,     0,     0,    47,    17,
       0,    18,     0,   108,   530,     0,    49,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,     0,     0,     0,
       0,    41,    42,    43,    44,     0,     0,    45,     0,     0,
       0,     0,    46,     0,     0,     0,     0,     0,     0,    47,
      17,     0,    18,     0,   108,     0,     0,    49,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,   256,     0,    18,
      47,   257,     0,     0,     0,     0,   258,     0,    49,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,     0,     0,     0,     0,     0,     0,    41,
      42,    43,    44,     0,     0,    45,   256,     0,    18,     0,
     443,     0,     0,     0,   259,   258,     0,    47,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,     0,     0,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,   256,     0,    18,     0,   360,
       0,     0,     0,   444,   258,     0,    47,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,     0,     0,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,   256,     0,    18,     0,   499,     0,
       0,     0,     0,   258,     0,    47,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
       0,     0,     0,     0,     0,     0,    41,    42,    43,    44,
       0,     0,    45,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    47
  };

  const short
  parser::yycheck_[] =
  {
       2,     3,    39,    47,    63,   163,   230,   203,   241,    37,
      19,     8,    47,   168,   133,    47,   197,    19,    20,     2,
       3,     2,   203,   178,     0,     4,     4,    15,    12,     8,
       8,     6,    91,    15,   153,     1,    19,    15,    19,    10,
      11,    69,   135,   162,   268,    12,   139,    49,     0,    15,
       4,    12,    47,    30,    30,    50,    50,    41,    27,    47,
       8,    63,     8,    42,    42,    47,     1,    64,    46,    47,
       8,   130,    51,    51,    40,   119,    85,    59,    13,     8,
      15,    47,    58,    85,   119,    87,     1,   119,    42,    91,
       5,    88,    59,    59,    91,    97,    98,    51,    59,   108,
      15,   259,    85,    51,    85,    51,   108,   226,   167,     1,
     229,   266,    47,    51,   111,   296,   235,   176,    10,   300,
      55,    56,    51,    15,     8,    60,     6,     8,   130,   188,
       8,    10,    47,    55,    56,    97,    98,    52,    60,    54,
      55,    56,    57,    58,    59,    60,   370,     1,    40,     4,
     374,     6,    10,     8,     6,    47,   305,   306,   382,    13,
       4,    15,    14,    15,   223,   167,   321,    51,     7,   402,
      51,    50,   174,    51,   176,    39,    15,   179,   175,    12,
       7,    13,   184,    15,    18,    19,   188,    42,   216,   191,
     192,   188,   189,    47,     6,    47,     8,   194,    42,     1,
     358,    55,    56,    15,   241,   207,    60,    40,    47,    39,
       7,    13,   408,    15,    41,    47,    10,   372,    12,     4,
       0,   223,   224,     8,     0,   222,   228,   408,   287,     4,
      14,    15,     1,    14,    15,    47,    14,    15,    30,    12,
       9,   243,    12,   245,    41,    47,    15,     9,   276,   251,
      12,    85,     8,    55,    56,   257,     7,    42,    60,    15,
      16,    17,    18,    47,   488,   262,    47,    40,     4,    47,
      40,    40,   106,   107,   108,   109,    10,   274,    47,    41,
      12,   115,   116,    50,   118,   287,   444,   289,   285,    30,
      59,    47,     0,     1,   413,     3,     9,   416,    54,    10,
      12,   135,    10,    44,    45,   139,    32,    51,    34,    35,
       0,   308,    16,    17,    18,    13,    10,    58,    12,    12,
       0,    61,    37,    21,    22,    10,     0,     9,    12,    12,
       7,   328,   376,     9,    12,    37,     9,     6,    10,    61,
       0,    39,    40,     9,    51,     9,     9,     9,     1,   183,
      48,     0,     5,     4,    69,     5,    10,     5,   360,    50,
      41,   363,    15,     9,     9,   402,     9,    69,     7,     7,
       0,   373,     9,     9,     9,   209,   210,     9,     5,   213,
      51,     7,     0,   217,    51,   219,   220,   415,     9,     0,
      12,   388,    12,     9,    47,   423,     9,   231,   232,    52,
      41,    54,    55,    56,    57,    58,    59,    60,   242,     9,
     244,    10,   471,     9,     7,    59,   250,    51,    51,     7,
      51,     0,     9,   257,     9,     9,   124,   142,     5,   144,
     264,     5,    46,     5,   436,    10,     9,     0,   136,     9,
     142,   443,   144,     9,    59,    51,    51,   145,    51,   451,
       7,    10,   454,     9,   152,     9,    51,     5,     9,     5,
       9,   472,   160,   465,   407,   180,   191,   296,   311,   471,
     393,   479,   300,   339,   172,   172,   310,   311,   180,   224,
     314,   270,    -1,   181,   182,   319,   320,    -1,   547,    -1,
      -1,    -1,    -1,    -1,    -1,   329,    -1,   499,    -1,   197,
     502,   216,   504,    -1,    -1,   203,   508,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   216,   230,    -1,    -1,    -1,    -1,
     517,   518,    -1,    -1,    -1,   240,   360,    -1,   230,    -1,
      -1,    -1,   366,    -1,   368,    -1,    -1,    -1,   240,   541,
     542,   375,   544,   241,    -1,   547,    -1,    -1,    -1,   247,
     248,    -1,    -1,   268,    -1,   557,   254,   255,    -1,    -1,
      -1,   276,   559,    -1,    -1,    -1,   268,    -1,    -1,    -1,
      -1,    -1,   270,    -1,   276,    -1,    -1,    -1,    -1,    -1,
      -1,   279,    -1,   281,   418,    15,    -1,    -1,    -1,    -1,
      -1,   425,    -1,    -1,    -1,    -1,    -1,    -1,   296,    -1,
      -1,   435,   300,    -1,    -1,    -1,    -1,   322,    -1,   443,
      -1,    -1,   310,   311,    -1,    -1,    -1,    47,    -1,    -1,
     322,    -1,    52,   457,    54,    55,    56,    57,    58,    59,
      60,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,   477,   478,   479,    -1,   481,    -1,    -1,
     348,   485,    -1,    -1,    39,   370,   354,    -1,    -1,   374,
      -1,    -1,    -1,    -1,    -1,   499,   381,   382,   370,    -1,
      -1,    -1,   374,    -1,    -1,    -1,    -1,    -1,    -1,   381,
     382,    -1,    -1,    -1,    -1,    -1,    -1,   521,   522,    -1,
      -1,     1,    -1,    -1,   528,     5,    -1,    -1,    -1,    -1,
     415,    -1,    -1,    -1,   402,    15,    -1,    -1,   423,    -1,
     408,    -1,   410,   415,    -1,    -1,    -1,   415,    -1,    -1,
      -1,   423,    32,    -1,    34,    35,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    -1,    47,    -1,    -1,
     438,    -1,    52,   441,    54,    55,    56,    57,    58,    59,
      60,    50,    -1,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   487,   488,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   479,    -1,     0,     1,   487,   488,     4,    -1,     6,
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
       0,    30,    44,    45,    58,    64,    65,    66,    68,    69,
      70,    71,    74,     6,    73,     0,     1,     4,     6,     8,
      11,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    30,    31,    32,
      33,    36,    37,    38,    39,    42,    47,    54,    59,    62,
      67,    84,    85,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   105,   110,   119,   120,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,     1,     8,    10,    11,    88,    89,
     102,   110,   119,   143,    30,     0,     1,     3,    10,   143,
       0,    70,     7,   127,    50,    72,    10,    50,     8,    53,
     102,   103,   104,   121,     1,    41,    50,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    91,    92,   102,   104,
     110,   119,   122,   123,   102,     6,    14,   127,   127,     6,
       8,    85,    87,   105,   115,    13,   117,   118,   127,   129,
       1,    10,    40,   108,   127,    39,   135,   137,   139,     1,
      40,    59,   109,   127,   102,     1,   110,   111,     6,    53,
      54,    59,    61,    92,    93,    88,    10,   143,     8,    51,
      87,    12,    40,     8,    51,    27,    39,    39,   110,   119,
     102,    10,    11,   143,    88,   143,     0,     4,    67,    67,
       0,     7,    30,     4,   104,   104,   104,    10,   143,    50,
      12,   144,   104,    41,   104,   104,     9,    50,     9,    12,
      41,     9,    88,    10,    12,   144,   121,   127,    51,   121,
       9,    53,    57,   104,   112,   113,   114,    85,   105,    86,
      87,    12,     8,    51,     8,    51,   127,    12,    40,   144,
       8,    51,   127,    12,    40,   144,     4,     8,    13,    51,
     126,     5,    88,     7,    41,   114,     6,     8,   115,   127,
      61,    90,   102,   143,    88,     0,     9,   114,   102,    14,
     127,    14,   127,   104,   102,    88,   143,    10,   143,    11,
      89,   102,     0,   143,     0,     1,    57,    79,    80,    81,
      82,    83,    91,   127,    32,    34,    35,    77,    78,    83,
      12,    12,   102,   104,    53,   104,     7,     9,   104,    12,
      41,     8,    87,   104,   104,     9,   104,   143,    88,    53,
     102,   122,     9,   144,   102,   144,    86,   104,   104,    12,
     144,     9,   117,   104,   102,   104,   102,    10,    12,   127,
     127,    10,   104,   102,    12,    59,   127,    59,   127,   104,
       8,   126,   143,    51,   104,     7,    41,     7,    41,   114,
       9,    86,     8,    51,   116,     8,    54,   127,   136,     0,
     143,    87,     9,   127,   127,     9,   143,     9,    88,     9,
     102,     0,    80,     4,    81,     5,     1,    55,    56,    60,
      75,    76,   142,     1,    76,     1,    76,    10,   143,     5,
      32,    50,   104,   106,   107,   127,   106,   104,    50,     9,
     104,     9,   104,     9,   114,    41,     9,     9,   143,     7,
       7,   112,     9,     9,   127,     8,    51,    10,    12,     9,
     127,    12,    59,     8,    51,   126,     9,   104,     5,   102,
       7,    51,     7,   104,    51,   104,     7,    41,    86,     9,
     114,   102,    86,   104,     0,    51,    86,     9,     0,   143,
       0,   111,    12,   117,    77,    83,   127,    47,    50,    12,
     144,    46,   127,   144,   104,    41,     9,    87,     9,     9,
     104,     9,    10,   104,   102,   127,    59,   127,   104,     8,
     126,     9,    51,   102,    51,     7,   102,     7,    51,     7,
     104,     9,     9,   102,     0,     9,     5,    29,    88,    75,
       5,     8,    51,   104,   104,   107,     5,   104,    46,     5,
       9,   104,    86,     9,     9,    10,    59,     9,   104,   102,
     102,    51,    51,   102,    51,     7,     0,   143,   143,   104,
     104,   104,     9,     9,   102,   102,   102,    51,     5,    88,
       5,     9,   102,   143,     5
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    63,    64,    64,    64,    64,    64,    64,    65,    65,
      65,    66,    66,    66,    66,    66,    66,    66,    66,    67,
      67,    67,    67,    67,    68,    68,    68,    68,    68,    69,
      69,    70,    70,    71,    71,    72,    72,    73,    73,    73,
      74,    75,    76,    76,    77,    77,    77,    77,    77,    77,
      78,    78,    79,    79,    80,    80,    80,    80,    81,    81,
      81,    82,    82,    83,    83,    84,    84,    85,    85,    86,
      86,    87,    87,    87,    87,    88,    88,    88,    88,    88,
      89,    89,    89,    90,    90,    90,    91,    91,    91,    91,
      91,    91,    91,    91,    92,    92,    93,    93,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    95,    95,    95,    96,    96,    96,    97,    97,    97,
      98,    98,    98,    99,    99,    99,    99,    99,   100,   100,
     100,   100,   100,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   103,   103,   104,   105,   105,
     106,   106,   107,   107,   107,   107,   107,   107,   107,   108,
     108,   108,   108,   108,   108,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   111,
     111,   111,   112,   112,   112,   113,   113,   114,   115,   116,
     116,   116,   117,   117,   117,   117,   117,   118,   118,   119,
     120,   120,   120,   120,   120,   120,   120,   120,   121,   121,
     121,   121,   121,   121,   121,   122,   122,   123,   123,   123,
     124,   125,   125,   126,   126,   126,   127,   127,   128,   128,
     128,   128,   129,   129,   129,   130,   131,   132,   132,   133,
     133,   134,   134,   135,   135,   135,   136,   136,   137,   137,
     137,   137,   138,   138,   139,   139,   140,   140,   141,   141,
     141,   141,   141,   141,   142,   142,   142,   142,   143,   143,
     144,   144
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     3,     2,     1,     1,     2,     0,     5,     6,
       4,     5,     4,     7,     6,     4,     3,     8,     6,     1,
       2,     5,     3,     6,     2,     2,     3,     3,     2,     1,
       2,     1,     1,     8,     6,     0,     2,     0,     3,     2,
       5,     2,     1,     3,     2,     2,     2,     2,     2,     2,
       1,     3,     1,     1,     6,     4,     8,     6,     1,     1,
       2,     1,     2,     0,     1,     1,     1,     5,     3,     0,
       1,     1,     1,     2,     2,     1,     3,     3,     4,     2,
       1,     1,     1,     2,     2,     4,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     7,     6,
       4,     5,     4,     2,     5,     4,     5,     5,     2,     3,
       4,     7,     6,     6,     9,     8,     8,     6,     5,     5,
       8,     7,     7,     4,     5,     7,     6,     4,     6,     8,
       7,     5,     5,     1,     1,     1,     1,     1,     3,     2,
       2,     4,     3,     1,     1,     1,     1,     1,     1,     1,
       5,     4,     3,     5,     1,     2,     3,     4,     6,     3,
       4,     6,     5,     7,     6,     1,     3,     2,     7,     7,
       1,     3,     3,     4,     4,     5,     1,     3,     3,     1,
       3,     4,     3,     5,     6,     2,     1,     3,     4,     5,
       4,     3,     5,     6,     7,     2,     4,     5,     7,     4,
       6,     2,     2,     2,     4,     5,     7,     4,     6,     0,
       1,     1,     2,     2,     1,     1,     3,     2,     3,     0,
       2,     3,     1,     3,     4,     3,     4,     1,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     1,
       3,     2,     3,     5,     4,     3,     2,     0,     1,     3,
       4,     4,     5,     1,     1,     1,     1,     1,     2,     3,
       3,     4,     1,     3,     4,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     2,     1,     1,
       2,     2,     1,     2,     0,     1,     1,     1,     0,     1,
       0,     1
  };




#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   243,   243,   244,   245,   246,   247,   248,   256,   259,
     265,   276,   279,   282,   285,   288,   291,   294,   300,   309,
     312,   314,   317,   320,   328,   331,   337,   340,   343,   360,
     362,   367,   368,   372,   375,   383,   384,   388,   389,   390,
     394,   403,   415,   417,   422,   424,   426,   428,   438,   448,
     461,   463,   473,   474,   478,   481,   484,   487,   493,   494,
     505,   517,   518,   522,   523,   532,   533,   537,   539,   544,
     545,   549,   550,   551,   552,   556,   557,   558,   563,   568,
     576,   577,   578,   597,   598,   599,   603,   604,   605,   606,
     607,   608,   609,   610,   614,   615,   619,   621,   626,   629,
     632,   636,   639,   642,   645,   648,   651,   661,   668,   673,
     679,   694,   701,   708,   717,   724,   731,   741,   747,   753,
     762,   768,   774,   783,   788,   793,   798,   803,   811,   816,
     822,   827,   832,   845,   848,   851,   854,   857,   860,   867,
     875,   878,   881,   884,   887,   890,   896,   897,   898,   899,
     901,   904,   907,   910,   913,   914,   916,   918,   921,   924,
     927,   933,   936,   939,   942,   950,   951,   955,   963,   966,
     971,   973,   978,   980,   982,   984,   986,   988,   990,  1003,
    1005,  1010,  1013,  1016,  1021,  1027,  1029,  1031,  1033,  1035,
    1037,  1039,  1041,  1043,  1045,  1050,  1051,  1052,  1054,  1056,
    1061,  1070,  1082,  1094,  1096,  1098,  1100,  1102,  1107,  1119,
    1120,  1123,  1133,  1134,  1135,  1139,  1140,  1143,  1145,  1148,
    1149,  1151,  1160,  1162,  1164,  1170,  1172,  1180,  1182,  1186,
    1194,  1195,  1196,  1197,  1198,  1199,  1200,  1201,  1205,  1207,
    1209,  1211,  1213,  1215,  1217,  1225,  1227,  1232,  1234,  1236,
    1241,  1246,  1248,  1257,  1258,  1259,  1267,  1268,  1273,  1275,
    1277,  1279,  1283,  1285,  1287,  1298,  1300,  1303,  1304,  1308,
    1309,  1313,  1314,  1318,  1319,  1320,  1324,  1325,  1330,  1331,
    1332,  1333,  1337,  1339,  1344,  1346,  1351,  1353,  1358,  1359,
    1360,  1361,  1362,  1363,  1367,  1368,  1369,  1370,  1382,  1382,
    1383,  1383
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
#line 4070 "langutils/sc_parser/src/sc_grammar_parser.cpp"

#line 1385 "langutils/sc_parser/src/sc_grammar.y"

