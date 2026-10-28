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
#line 95 "langutils/sc_parser/src/sc_grammar.y"


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
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[5].location), cxt.graph->prepend((yystack_[2].value.v_BlockContentsListIndex), (yystack_[4].value.v_DeclareVariableListIndex))); }
#line 680 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 10: // file_as_func: INTERPRET variable_declarations semicolon.opt $end
#line 263 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[3].location), cxt.create(BlockContentsList{}, yystack_[1].location, (yystack_[2].value.v_DeclareVariableListIndex))); }
#line 686 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 11: // interpreted_function: INTERPRET_FUNCTION argument_declarations block.contents semicolon.opt $end
#line 274 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 692 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 12: // interpreted_function: INTERPRET_FUNCTION argument_declarations semicolon.opt $end
#line 277 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[2].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, yylhs.location)); }
#line 698 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 13: // interpreted_function: INTERPRET_FUNCTION OPENPAREN argument_declarations block.contents semicolon.opt CLOSEPAREN $end
#line 280 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[4].value.v_DeclareArgumentListIndex), (yystack_[3].value.v_BlockContentsListIndex)); }
#line 704 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 14: // interpreted_function: INTERPRET_FUNCTION OPENPAREN argument_declarations semicolon.opt CLOSEPAREN $end
#line 283 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, yylhs.location)); }
#line 710 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 15: // interpreted_function: INTERPRET_FUNCTION block.contents semicolon.opt $end
#line 286 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[3].location), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 716 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 16: // interpreted_function: INTERPRET_FUNCTION semicolon.opt $end
#line 289 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[2].location), cxt.create(BlockContentsList{}, yylhs.location)); }
#line 722 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 17: // interpreted_function: INTERPRET_FUNCTION OPENPAREN variable_declarations SEMICOLON block.contents semicolon.opt CLOSEPAREN $end
#line 292 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[7].location), cxt.graph->prepend((yystack_[3].value.v_BlockContentsListIndex), (yystack_[5].value.v_DeclareVariableListIndex))); }
#line 728 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 18: // interpreted_function: INTERPRET_FUNCTION OPENPAREN variable_declarations semicolon.opt CLOSEPAREN $end
#line 295 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[5].location), cxt.create(BlockContentsList{}, yystack_[3].location, (yystack_[3].value.v_DeclareVariableListIndex))); }
#line 734 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 19: // region.item: expr
#line 304 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 740 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 20: // region.item: NONLOCALRETURN expr
#line 307 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 746 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 21: // region.item: OPENPAREN argument_declarations block.contents semicolon.opt CLOSEPAREN
#line 309 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 752 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 22: // region.item: OPENPAREN argument_declarations CLOSEPAREN
#line 312 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, yystack_[0].location)); }
#line 758 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 23: // region.item: OPENPAREN variable_declarations SEMICOLON block.contents semicolon.opt CLOSEPAREN
#line 315 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[5].location), cxt.graph->prepend((yystack_[2].value.v_BlockContentsListIndex), (yystack_[4].value.v_DeclareVariableListIndex))); }
#line 764 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 24: // region: INTERPRET region.item
#line 320 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionListIndex) = cxt.create(RegionList{}, yylhs.location, (yystack_[0].value.v_RegionItemIndex)); }
#line 770 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 25: // region: INTERPRET error
#line 323 "langutils/sc_parser/src/sc_grammar.y"
                {
			error_recovery::expr(cxt);
			(yylhs.value.v_RegionListIndex) = cxt.create(RegionList{}, yylhs.location, cxt.create_error(yylhs.location, yystack_[0].location));
		}
#line 779 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 26: // region: region SEMICOLON region.item
#line 329 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionListIndex) = cxt.graph->append((yystack_[2].value.v_RegionListIndex), yylhs.location, (yystack_[0].value.v_RegionItemIndex)); }
#line 785 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 27: // region: region REGION_SEPARATOR region.item
#line 332 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionListIndex) = cxt.graph->append((yystack_[2].value.v_RegionListIndex), yylhs.location, (yystack_[0].value.v_RegionItemIndex)); }
#line 791 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 28: // region: region error
#line 335 "langutils/sc_parser/src/sc_grammar.y"
                {
			// A missing semicolon between lines is inserted by the lexer, see yylex.
			error_recovery::expr(cxt);
			(yylhs.value.v_RegionListIndex) = cxt.graph->append((yystack_[1].value.v_RegionListIndex), cxt.create_error(yystack_[0].location, yystack_[0].location, Index { *(yystack_[1].value.v_RegionListIndex) }));
			yyclearin;
		}
#line 802 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 29: // classOrExtList.list: classOrExtList.item
#line 352 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassOrExtensionListIndex) = cxt.create(ClassOrExtensionList{}, yylhs.location, (yystack_[0].value.v_ClassOrExtensionIndex)); }
#line 808 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 30: // classOrExtList.list: classOrExtList.list classOrExtList.item
#line 354 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassOrExtensionListIndex) = cxt.graph->append((yystack_[1].value.v_ClassOrExtensionListIndex), (yystack_[0].value.v_ClassOrExtensionIndex)); }
#line 814 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 31: // classOrExtList.item: class
#line 358 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassOrExtensionIndex) = (yystack_[0].value.v_ClassIndex); }
#line 820 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 32: // classOrExtList.item: class.extension
#line 359 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_ClassOrExtensionIndex) = (yystack_[0].value.v_ClassExtensionIndex); }
#line 826 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 33: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY members semicolon.opt method.list.opt CLOSECURLY
#line 364 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassIndex) = cxt.create(Class{ cxt.take_version(yystack_[4].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[7].location), (yystack_[6].value.v_maybe_NamedIdentifierIndex), (yystack_[5].value.v_maybe_ClassNameIdentifierIndex), (yystack_[3].value.v_BigMemberListIndex), (yystack_[1].value.v_MethodListIndex)); }
#line 832 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 34: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY method.list.opt CLOSECURLY
#line 367 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto no_members = cxt.create(BigMemberList{}, sc::src::SourceCodeRange { yystack_[2].location.end, yystack_[2].location.end });
			(yylhs.value.v_ClassIndex) = cxt.create(Class{ cxt.take_version(yystack_[2].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[5].location), (yystack_[4].value.v_maybe_NamedIdentifierIndex), (yystack_[3].value.v_maybe_ClassNameIdentifierIndex), no_members, (yystack_[1].value.v_MethodListIndex));
		}
#line 841 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 35: // class.super.opt: %empty
#line 374 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_maybe_ClassNameIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 847 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 36: // class.super.opt: COLON CLASSNAME
#line 375 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_maybe_ClassNameIdentifierIndex) = cxt.create(ClassNameIdentifier{}, yystack_[0].location); }
#line 853 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 37: // class.slot.opt: %empty
#line 379 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_maybe_NamedIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 859 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 38: // class.slot.opt: OPENSQUARE name CLOSESQUARE
#line 380 "langutils/sc_parser/src/sc_grammar.y"
                                      { (yylhs.value.v_maybe_NamedIdentifierIndex) = (yystack_[1].value.v_NamedIdentifierIndex); }
#line 865 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 39: // class.slot.opt: OPENSQUARE CLOSESQUARE
#line 381 "langutils/sc_parser/src/sc_grammar.y"
                                 { (yylhs.value.v_maybe_NamedIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 871 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 40: // class.extension: ADD CLASSNAME OPENCURLY method.list.opt CLOSECURLY
#line 386 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassExtensionIndex) = cxt.create(ClassExtension{ cxt.take_version(yystack_[2].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[3].location), (yystack_[1].value.v_MethodListIndex)); }
#line 877 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 41: // members.entry.item: accessor variable_declarations.list.item
#line 395 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberIndex) = cxt.create(DeclareMember{}, yylhs.location, (yystack_[1].value.v_RWAccessorIndex), (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 883 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 42: // members.entry.list: members.entry.item
#line 400 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberListIndex) = cxt.create(DeclareMemberList{}, yylhs.location, (yystack_[0].value.v_DeclareMemberIndex)); }
#line 889 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 43: // members.entry.list: members.entry.list COMMA members.entry.item
#line 402 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberListIndex) = cxt.graph->append((yystack_[2].value.v_DeclareMemberListIndex), (yystack_[0].value.v_DeclareMemberIndex)); }
#line 895 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 44: // members.entry: CLASSVAR members.entry.list
#line 407 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberEntryIndex) = cxt.graph->cast<DeclareClassMemberList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 901 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 45: // members.entry: VAR members.entry.list
#line 409 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberEntryIndex) = cxt.graph->cast<DeclareMemberList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 907 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 46: // members.entry: CONST members.entry.list
#line 411 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberEntryIndex) = cxt.graph->cast<DeclareConstList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 913 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 47: // members.entry: CLASSVAR error
#line 413 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips to the declaration's `;`, or the next method.
			const auto dropped = error_recovery::member(cxt, yystack_[1].location, yystack_[0].location);
			if (!dropped)
				YYABORT;
			(yylhs.value.v_BigMemberEntryIndex) = cxt.create_error(*dropped, yystack_[0].location);
			yyclearin;
			yyerrok; // Recovered at the next item, so report its error.
		}
#line 927 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 48: // members.entry: VAR error
#line 423 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips to the declaration's `;`, or the next method.
			const auto dropped = error_recovery::member(cxt, yystack_[1].location, yystack_[0].location);
			if (!dropped)
				YYABORT;
			(yylhs.value.v_BigMemberEntryIndex) = cxt.create_error(*dropped, yystack_[0].location);
			yyclearin;
			yyerrok; // Recovered at the next item, so report its error.
		}
#line 941 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 49: // members.entry: CONST error
#line 433 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips to the declaration's `;`, or the next method.
			const auto dropped = error_recovery::member(cxt, yystack_[1].location, yystack_[0].location);
			if (!dropped)
				YYABORT;
			(yylhs.value.v_BigMemberEntryIndex) = cxt.create_error(*dropped, yystack_[0].location);
			yyclearin;
			yyerrok; // Recovered at the next item, so report its error.
		}
#line 955 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 50: // members: members.entry
#line 446 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberListIndex) = cxt.create(BigMemberList{}, yylhs.location, (yystack_[0].value.v_BigMemberEntryIndex));}
#line 961 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 51: // members: members SEMICOLON members.entry
#line 448 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberListIndex) = cxt.graph->append((yystack_[2].value.v_BigMemberListIndex), (yystack_[0].value.v_BigMemberEntryIndex)); }
#line 967 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 52: // method.name: name
#line 457 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 973 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 53: // method.name: binary_op.raw
#line 458 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 979 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 54: // method.base: method.name OPENCURLY argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 463 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(Missing{},yystack_[4].location), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 985 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 55: // method.base: method.name OPENCURLY argument_declarations.opt CLOSECURLY
#line 466 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[3].value.v_SelectorIndex), (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(Missing{},yystack_[2].location), cxt.create(BlockContentsList{}, yystack_[0].location)); }
#line 991 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 56: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt block.contents semicolon.opt CLOSECURLY
#line 469 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[7].value.v_SelectorIndex), (yystack_[5].value.v_DeclareArgumentListIndex), cxt.create(PrimitiveIdentifier{}, yystack_[4].location), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 997 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 57: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt CLOSECURLY
#line 472 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(PrimitiveIdentifier{}, yystack_[2].location), cxt.create(BlockContentsList{}, yystack_[1].location)); }
#line 1003 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 58: // method: method.base
#line 477 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_AnyMethodIndex) = (yystack_[0].value.v_AnyMethodIndex); }
#line 1009 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 59: // method: error
#line 479 "langutils/sc_parser/src/sc_grammar.y"
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
#line 1024 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 60: // method: MULTIPLY method.base
#line 490 "langutils/sc_parser/src/sc_grammar.y"
                { 
			if (auto c = cxt.graph->as_a<MethodIndex>((yystack_[0].value.v_AnyMethodIndex))) {
				(yylhs.value.v_AnyMethodIndex) = cxt.graph->cast<ClassMethod>(*c);
			} 
			// leaves as error
			cxt.graph->location((yystack_[0].value.v_AnyMethodIndex)) = yylhs.location; 
			(yylhs.value.v_AnyMethodIndex) = (yystack_[0].value.v_AnyMethodIndex); 
		}
#line 1037 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 61: // method.list: method
#line 501 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_MethodListIndex) = cxt.create(MethodList{}, yylhs.location, (yystack_[0].value.v_AnyMethodIndex)); }
#line 1043 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 62: // method.list: method.list method
#line 502 "langutils/sc_parser/src/sc_grammar.y"
                             { (yylhs.value.v_MethodListIndex) = cxt.graph->append((yystack_[1].value.v_MethodListIndex), yylhs.location, (yystack_[0].value.v_AnyMethodIndex)); }
#line 1049 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 63: // method.list.opt: %empty
#line 506 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_MethodListIndex) = cxt.create(MethodList{}, yylhs.location); }
#line 1055 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 64: // method.list.opt: method.list
#line 507 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_MethodListIndex) = (yystack_[0].value.v_MethodListIndex); }
#line 1061 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 65: // block.open: OPENCURLY
#line 516 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_bool) = false; }
#line 1067 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 66: // block.open: BEGINCLOSEDFUNC
#line 517 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_bool) = true; }
#line 1073 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 67: // block: block.open argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 522 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockIndex) = cxt.create(BlockNode{ (yystack_[4].value.v_bool) }, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 1079 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 68: // block: block.open argument_declarations.opt CLOSECURLY
#line 524 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockIndex) = cxt.create(BlockNode{ (yystack_[2].value.v_bool) }, yylhs.location, (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, yylhs.location)); }
#line 1085 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 69: // block.opt_list: %empty
#line 528 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location); }
#line 1091 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 70: // block.opt_list: block.list
#line 529 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_BlockListIndex) = (yystack_[0].value.v_BlockListIndex); }
#line 1097 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 71: // block.list: block
#line 533 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location, (yystack_[0].value.v_BlockIndex)); }
#line 1103 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 72: // block.list: generator
#line 534 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location, (yystack_[0].value.v_GeneratorAnyIndex)); }
#line 1109 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 73: // block.list: block.list block
#line 535 "langutils/sc_parser/src/sc_grammar.y"
                           { (yylhs.value.v_BlockListIndex) = cxt.graph->append((yystack_[1].value.v_BlockListIndex), yylhs.location, (yystack_[0].value.v_BlockIndex)); }
#line 1115 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 74: // block.list: block.list generator
#line 536 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_BlockListIndex) = cxt.graph->append((yystack_[1].value.v_BlockListIndex), yylhs.location, (yystack_[0].value.v_GeneratorAnyIndex)); }
#line 1121 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 75: // block.contents: block.contents.item
#line 540 "langutils/sc_parser/src/sc_grammar.y"
                              { (yylhs.value.v_BlockContentsListIndex) = cxt.create(BlockContentsList{}, yylhs.location, (yystack_[0].value.v_BlockItemIndex)); }
#line 1127 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 76: // block.contents: block.contents SEMICOLON block.contents.item
#line 541 "langutils/sc_parser/src/sc_grammar.y"
                                                       { (yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[2].value.v_BlockContentsListIndex), yylhs.location, (yystack_[0].value.v_BlockItemIndex)); }
#line 1133 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 77: // block.contents: block.contents NONLOCALRETURN expr
#line 543 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[2].value.v_BlockContentsListIndex), yylhs.location, non_local); 
		}
#line 1142 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 78: // block.contents: block.contents SEMICOLON NONLOCALRETURN expr
#line 548 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[3].value.v_BlockContentsListIndex), yylhs.location, non_local); 
		}
#line 1151 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 79: // block.contents: NONLOCALRETURN expr
#line 553 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.create(BlockContentsList{}, yylhs.location, non_local);
		}
#line 1160 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 80: // block.contents.item: expr
#line 560 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_BlockItemIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1166 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 81: // block.contents.item: variable_declarations
#line 561 "langutils/sc_parser/src/sc_grammar.y"
                                { (yylhs.value.v_BlockItemIndex) = (yystack_[0].value.v_DeclareVariableListIndex); }
#line 1172 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 82: // block.contents.item: error
#line 563 "langutils/sc_parser/src/sc_grammar.y"
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
#line 1187 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 83: // adverb: DOT name
#line 581 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_AdverbIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 1193 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 84: // adverb: DOT integer
#line 582 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_AdverbIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 1199 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 85: // adverb: DOT OPENPAREN expr_or_seq CLOSEPAREN
#line 583 "langutils/sc_parser/src/sc_grammar.y"
                                               { (yylhs.value.v_AdverbIndex) = cxt.create(AdverbExprNode{}, yylhs.location, (yystack_[1].value.v_AnyExprIndex));  }
#line 1205 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 86: // binary_op.raw: BINOP
#line 587 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1211 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 87: // binary_op.raw: READWRITEVAR
#line 588 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1217 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 88: // binary_op.raw: LESSTHAN
#line 589 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1223 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 89: // binary_op.raw: GREATERTHAN
#line 590 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1229 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 90: // binary_op.raw: MINUS
#line 591 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1235 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 91: // binary_op.raw: MULTIPLY
#line 592 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1241 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 92: // binary_op.raw: ADD
#line 593 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1247 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 93: // binary_op.raw: PIPE
#line 594 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1253 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 94: // binary_op.no_adverb: binary_op.raw
#line 598 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 1259 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 95: // binary_op.no_adverb: KEYBINOP
#line 599 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ true }, yylhs.location); }
#line 1265 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 96: // binary_op: binary_op.no_adverb adverb
#line 604 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorMaybeAdverbIndex) = cxt.create(SelectorWAdverb{}, yylhs.location, (yystack_[1].value.v_SelectorIndex), (yystack_[0].value.v_AdverbIndex)); }
#line 1271 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 97: // binary_op: binary_op.no_adverb
#line 606 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorMaybeAdverbIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 1277 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 98: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN arguments CLOSEPAREN block.opt_list
#line 611 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[2].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1283 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 99: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN CLOSEPAREN block.list
#line 614 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[4].value.v_SelectorIndex), cxt.create(ArgumentList{}, yystack_[2].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1289 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 100: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN block.list
#line 617 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[2].value.v_SelectorIndex), cxt.create(ArgumentList{}, yystack_[0].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1295 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 101: // msgsend: name OPENPAREN arguments CLOSEPAREN block.opt_list
#line 621 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[2].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1301 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 102: // msgsend: name OPENPAREN CLOSEPAREN block.list
#line 624 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), cxt.create(ArgumentList{}, yystack_[2].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1307 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 103: // msgsend: name block.list
#line 627 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[1].value.v_NamedIdentifierIndex), cxt.create(ArgumentList{}, yystack_[0].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1313 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 104: // msgsend: expr DOT name arguments.maybe_paren block.opt_list
#line 630 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PostfixMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1319 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 105: // msgsend: expr DOT arguments.paren block.opt_list
#line 633 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(ValueMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1325 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 106: // msgsend: expr DOT OPENPAREN CLOSEPAREN block.opt_list
#line 636 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(ValueMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[2].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1331 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 107: // msgsend: CLASSNAME OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 646 "langutils/sc_parser/src/sc_grammar.y"
                {  (yylhs.value.v_AnyExprIndex) = cxt.create(CollectionNode{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[4].location), (yystack_[2].value.v_ArrayIndex)); }
#line 1337 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 108: // msgsend: CLASSNAME block.list
#line 649 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[1].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, yystack_[1].location), (yystack_[0].value.v_BlockListIndex));
		}
#line 1346 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 109: // msgsend: CLASSNAME arguments.paren block.opt_list
#line 654 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[2].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex));
		}
#line 1355 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 110: // msgsend: CLASSNAME OPENPAREN CLOSEPAREN block.opt_list
#line 660 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[3].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, yystack_[3].location), (yystack_[0].value.v_BlockListIndex));
		}
#line 1364 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 111: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 675 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[3].value.v_ArgumentListIndex), (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r);
		}
#line 1374 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 112: // expr.dotrange: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 682 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[3].location),  (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1384 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 113: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 689 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[2].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[1].location));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1394 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 114: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 698 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[5].value.v_ArgumentListIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[8].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1404 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 115: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 705 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[4].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[3].location));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1414 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 116: // expr.dotrange.assign: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 712 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[5].location),  (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1424 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 117: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 722 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[3].value.v_ArgumentListIndex), (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1434 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 118: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 728 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[2].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[0].location));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), r);	
		}
#line 1444 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 119: // expr.base.range: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 734 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[3].location),  (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), r);	
		}
#line 1454 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 120: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 743 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[5].value.v_ArgumentListIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1464 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 121: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 749 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[4].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[3].location));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1474 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 122: // expr.base.range.assign: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 755 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[5].location),  (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1484 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 123: // range: OPENPAREN expr_or_seq DOTDOT CLOSEPAREN
#line 764 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1493 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 124: // range: OPENPAREN expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 769 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[4].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1502 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 125: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 774 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[6].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1511 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 126: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 779 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[4].location, yystack_[2].location), (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[5].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1520 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 127: // range: OPENPAREN DOTDOT expr_or_seq CLOSEPAREN
#line 784 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location);
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1529 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 128: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 792 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[5].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1538 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 129: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 797 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[7].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1547 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 130: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 803 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[4].location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[6].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1556 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 131: // range.generator: OPENPAREN COLON DOTDOT expr_or_seq CLOSEPAREN
#line 808 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location);
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1565 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 132: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT CLOSEPAREN
#line 813 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1574 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 133: // expr.base: literal
#line 826 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyPossiblyLiteralIndex); }
#line 1580 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 134: // expr.base: generator
#line 829 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_GeneratorAnyIndex); }
#line 1586 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 135: // expr.base: name
#line 832 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 1592 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 136: // expr.base: CURRYARG
#line 835 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyExprIndex) = cxt.create(CurryNode{}, yystack_[0].location); }
#line 1598 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 137: // expr.base: msgsend
#line 838 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1604 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 138: // expr.base: OPENPAREN expr_or_seq CLOSEPAREN
#line 841 "langutils/sc_parser/src/sc_grammar.y"
                { 
			// A nasty hack because of nil assignment.
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AnyExprIndex) = (yystack_[1].value.v_AnyExprIndex);
		}
#line 1614 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 139: // expr.base: OPENPAREN error
#line 848 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto close = error_recovery::bracket(cxt, yystack_[1].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create_error(range(yystack_[1].location, close), yystack_[0].location);
			yyclearin; // The lookahead was skipped.
			yyerrok; // Recovered at the matching bracket, so report the next error.
		}
#line 1625 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 140: // expr.base: TILDE name
#line 856 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(EnvIdentifierNode{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1631 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 141: // expr.base: expr.base OPENSQUARE arguments CLOSESQUARE
#line 859 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex) ); }
#line 1637 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 142: // expr.base: expr.base OPENSQUARE CLOSESQUARE
#line 862 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[1].location)); }
#line 1643 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 143: // expr.base: expr.base.range
#line 865 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AtMessageIndex); }
#line 1649 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 144: // expr.base: range
#line 868 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeIndex); }
#line 1655 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 145: // expr.base: range.generator
#line 871 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeGeneratorIndex); }
#line 1661 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 146: // expr: expr.base
#line 876 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1667 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 147: // expr: expr.base.range.assign
#line 877 "langutils/sc_parser/src/sc_grammar.y"
                                 { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeAssignIndex); }
#line 1673 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 148: // expr: expr.dotrange
#line 878 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AtMessageIndex); }
#line 1679 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 149: // expr: expr.dotrange.assign
#line 879 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeAssignIndex); }
#line 1685 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 150: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE
#line 882 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex) ); }
#line 1691 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 151: // expr: expr DOT OPENSQUARE CLOSESQUARE
#line 885 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[1].location) ); }
#line 1697 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 152: // expr: expr binary_op expr
#line 888 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(BinaryMessage{}, yylhs.location, (yystack_[1].value.v_SelectorMaybeAdverbIndex), (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1703 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 153: // expr: expr DOT name EQUALSSIGN expr
#line 891 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(SetterNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1709 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 154: // expr: CLASSNAME
#line 893 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_AnyExprIndex) = cxt.create(ClassNameIdentifier{}, yylhs.location); }
#line 1715 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 155: // expr: BACKTICK expr
#line 894 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyExprIndex) = cxt.create(ReferenceNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1721 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 156: // expr: name EQUALSSIGN expr
#line 896 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentNode{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1727 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 157: // expr: TILDE name EQUALSSIGN expr
#line 899 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentNode{AssignmentNode::Target::Environment}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1733 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 158: // expr: name OPENPAREN arguments CLOSEPAREN EQUALSSIGN expr
#line 902 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(SetterNode{}, yylhs.location, (yystack_[3].value.v_ArgumentListIndex), (yystack_[5].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1739 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 159: // expr: name.pack EQUALSSIGN expr
#line 905 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[2].value.v_NamePackIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1745 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 160: // expr: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 908 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[3].value.v_NamePackIndex), (yystack_[1].value.v_AnyExprIndex)); }
#line 1751 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 161: // expr: expr.base OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 914 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_ArgumentListIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1757 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 162: // expr: expr.base OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 917 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[3].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 1763 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 163: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 920 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), (yystack_[3].value.v_ArgumentListIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1769 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 164: // expr: expr DOT OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 923 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[3].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 1775 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 165: // expr_or_seq.base: expr
#line 930 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1781 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 166: // expr_or_seq.base: expr_or_seq.base SEMICOLON expr
#line 931 "langutils/sc_parser/src/sc_grammar.y"
                                          { (yylhs.value.v_AnyExprIndex) = cxt.create(ExprSeq{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1787 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 167: // expr_or_seq: expr_or_seq.base semicolon.opt
#line 935 "langutils/sc_parser/src/sc_grammar.y"
                                       { (yylhs.value.v_AnyExprIndex) = (yystack_[1].value.v_AnyExprIndex); }
#line 1793 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 168: // generator: OPENCURLY COLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 944 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorAnyIndex) = cxt.create(GeneratorNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_GeneratorQualListIndex)); }
#line 1799 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 169: // generator: OPENCURLY SEMICOLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 947 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorAnyIndex) = cxt.create(GeneratorFakeNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_GeneratorQualListIndex)); }
#line 1805 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 170: // generator.qual.list: generator.qual
#line 952 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualListIndex) = cxt.create(GeneratorQualList{}, yylhs.location, (yystack_[0].value.v_GeneratorQualAnyIndex)); }
#line 1811 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 171: // generator.qual.list: generator.qual.list COMMA generator.qual
#line 954 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualListIndex) = cxt.graph->append((yystack_[2].value.v_GeneratorQualListIndex), yylhs.location, (yystack_[0].value.v_GeneratorQualAnyIndex)); }
#line 1817 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 172: // generator.qual: name LEFTARROW expr_or_seq
#line 959 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualAssign{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), cxt.create(Missing{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 1823 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 173: // generator.qual: name name LEFTARROW expr_or_seq
#line 961 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualAssign{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1829 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 174: // generator.qual: VAR name EQUALSSIGN expr_or_seq
#line 963 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualDeclare{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1835 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 175: // generator.qual: VAR name OPENPAREN expr_or_seq CLOSEPAREN
#line 965 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualDeclare{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex)); }
#line 1841 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 176: // generator.qual: expr_or_seq
#line 967 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualIf{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1847 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 177: // generator.qual: COLON COLON expr_or_seq
#line 969 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualSideEffect{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1853 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 178: // generator.qual: COLON WHILE expr_or_seq
#line 971 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualGuard{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1859 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 179: // argument_declarations.list: name
#line 984 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1865 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 180: // argument_declarations.list: name EQUALSSIGN expr
#line 986 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed((yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{is_bracketed}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex))); 
		}
#line 1874 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 181: // argument_declarations.list: name OPENPAREN expr_or_seq CLOSEPAREN
#line 991 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1880 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 182: // argument_declarations.list: argument_declarations.list COMMA name
#line 994 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[2].value.v_DeclareArgumentListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1886 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 183: // argument_declarations.list: argument_declarations.list COMMA name EQUALSSIGN expr
#line 997 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed((yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[4].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{is_bracketed}, range(yystack_[2].location, yystack_[0].location), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)));
		}
#line 1895 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 184: // argument_declarations.list: argument_declarations.list COMMA name OPENPAREN expr_or_seq CLOSEPAREN
#line 1002 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[3].location, yystack_[0].location), (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1901 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 185: // argument_declarations.pipelist: name literal
#line 1008 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, (yystack_[1].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1907 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 186: // argument_declarations.pipelist: name
#line 1010 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1913 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 187: // argument_declarations.pipelist: name EQUALSSIGN literal
#line 1012 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1919 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 188: // argument_declarations.pipelist: name OPENPAREN expr_or_seq CLOSEPAREN
#line 1014 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1925 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 189: // argument_declarations.pipelist: name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 1016 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1931 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 190: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name literal
#line 1018 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[3].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[1].location, yystack_[0].location), (yystack_[1].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1937 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 191: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name
#line 1020 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[2].value.v_DeclareArgumentListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1943 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 192: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN literal
#line 1022 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[4].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[2].location, yystack_[0].location), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1949 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 193: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name OPENPAREN expr_or_seq CLOSEPAREN
#line 1024 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[3].location, yystack_[0].location), (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1955 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 194: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 1026 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[6].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[4].location, yystack_[0].location), (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1961 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 195: // argument_declarations: ARG SEMICOLON
#line 1030 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 1967 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 196: // argument_declarations: ARG argument_declarations.list comma.opt SEMICOLON
#line 1031 "langutils/sc_parser/src/sc_grammar.y"
                                                             { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[2].value.v_DeclareArgumentListIndex); }
#line 1973 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 197: // argument_declarations: ARG argument_declarations.list ELLIPSIS name SEMICOLON
#line 1033 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[3].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 1979 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 198: // argument_declarations: ARG argument_declarations.list ELLIPSIS name COMMA name SEMICOLON
#line 1035 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 1985 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 199: // argument_declarations: ARG ELLIPSIS name SEMICOLON
#line 1037 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(arg_list, yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); 
		}
#line 1994 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 200: // argument_declarations: ARG ELLIPSIS name COMMA name SEMICOLON
#line 1042 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(
				arg_list, yylhs.location,
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))
			); 
		}
#line 2007 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 201: // argument_declarations: ARG error
#line 1051 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips past the `;` ending the list. The arguments are discarded with the error.
			const auto close = error_recovery::arguments(cxt, yystack_[0].location, sc::lex::TokenType::SemiColon);
			if (!close)
				YYABORT;
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, range(yystack_[1].location, *close));
			yyclearin;
			yyerrok; // Recovered at the end of the list, so report the next error.
		}
#line 2021 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 202: // argument_declarations: PIPE error
#line 1061 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Skips past the `|` ending the list. The arguments are discarded with the error.
			const auto close = error_recovery::arguments(cxt, yystack_[0].location, sc::lex::TokenType::Pipe);
			if (!close)
				YYABORT;
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, range(yystack_[1].location, *close));
			yyclearin;
			yyerrok; // Recovered at the end of the list, so report the next error.
		}
#line 2035 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 203: // argument_declarations: PIPE PIPE
#line 1071 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 2041 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 204: // argument_declarations: PIPE argument_declarations.pipelist comma.opt PIPE
#line 1073 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[2].value.v_DeclareArgumentListIndex); }
#line 2047 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 205: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name PIPE
#line 1075 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[3].value.v_DeclareArgumentListIndex), yystack_[3].location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 2053 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 206: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name COMMA name PIPE
#line 1077 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 2059 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 207: // argument_declarations: PIPE ELLIPSIS name PIPE
#line 1079 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(arg_list, yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); 
		}
#line 2068 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 208: // argument_declarations: PIPE ELLIPSIS name COMMA name PIPE
#line 1084 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(
				arg_list, yylhs.location,
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))
			); 
		}
#line 2081 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 209: // argument_declarations.opt: %empty
#line 1095 "langutils/sc_parser/src/sc_grammar.y"
                                       { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 2087 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 210: // argument_declarations.opt: argument_declarations
#line 1096 "langutils/sc_parser/src/sc_grammar.y"
                                { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[0].value.v_DeclareArgumentListIndex); }
#line 2093 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 211: // argument_declarations.opt: error
#line 1099 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 2099 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 212: // arguments.entries: KEYBINOP expr_or_seq
#line 1104 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_ArgumentEntryIndex) = cxt.create(KwArgNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2105 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 213: // arguments.entries: MULTIPLY expr_or_seq
#line 1105 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_ArgumentEntryIndex) = cxt.create(VariadicArgNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2111 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 214: // arguments.entries: expr_or_seq
#line 1106 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_ArgumentEntryIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 2117 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 215: // arguments.no_trailing: arguments.entries
#line 1110 "langutils/sc_parser/src/sc_grammar.y"
                            {  (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location, (yystack_[0].value.v_ArgumentEntryIndex)); }
#line 2123 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 216: // arguments.no_trailing: arguments.no_trailing COMMA arguments.entries
#line 1111 "langutils/sc_parser/src/sc_grammar.y"
                                                        { (yylhs.value.v_ArgumentListIndex) = cxt.graph->append((yystack_[2].value.v_ArgumentListIndex), yylhs.location, (yystack_[0].value.v_ArgumentEntryIndex)); }
#line 2129 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 217: // arguments: arguments.no_trailing comma.opt
#line 1114 "langutils/sc_parser/src/sc_grammar.y"
                                            { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2135 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 218: // arguments.paren: OPENPAREN arguments CLOSEPAREN
#line 1116 "langutils/sc_parser/src/sc_grammar.y"
                                                 { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2141 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 219: // arguments.maybe_paren: %empty
#line 1119 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location); }
#line 2147 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 220: // arguments.maybe_paren: OPENPAREN CLOSEPAREN
#line 1121 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location); }
#line 2153 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 221: // arguments.maybe_paren: OPENPAREN arguments CLOSEPAREN
#line 1123 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2159 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 222: // variable_declarations.list.item: name
#line 1132 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 2165 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 223: // variable_declarations.list.item: name EQUALSSIGN expr
#line 1134 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2171 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 224: // variable_declarations.list.item: name OPENPAREN expr_or_seq CLOSEPAREN
#line 1136 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Bracketed, as in the old compiler, so it isn't a literal, see ConstEval::is_legacy_literal.
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex));
		}
#line 2181 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 225: // variable_declarations.list.item: name.pack EQUALSSIGN expr
#line 1142 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[2].value.v_NamePackIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2187 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 226: // variable_declarations.list.item: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 1144 "langutils/sc_parser/src/sc_grammar.y"
                {
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[3].value.v_NamePackIndex), (yystack_[1].value.v_AnyExprIndex));
		}
#line 2196 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 227: // variable_declarations.list: variable_declarations.list.item
#line 1152 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareVariableListIndex) = cxt.create(DeclareVariableList{}, yylhs.location, (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 2202 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 228: // variable_declarations.list: variable_declarations.list COMMA variable_declarations.list.item
#line 1154 "langutils/sc_parser/src/sc_grammar.y"
                {(yylhs.value.v_DeclareVariableListIndex) = cxt.graph->append((yystack_[2].value.v_DeclareVariableListIndex), (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 2208 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 229: // variable_declarations: VAR variable_declarations.list
#line 1157 "langutils/sc_parser/src/sc_grammar.y"
                                                       { (yylhs.value.v_DeclareVariableListIndex) = (yystack_[0].value.v_DeclareVariableListIndex); }
#line 2214 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 230: // literal.terminal: symbol
#line 1165 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_AnySymbolIndex); }
#line 2220 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 231: // literal.terminal: string
#line 1166 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_StringLitIndex); }
#line 2226 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 232: // literal.terminal: integer
#line 1167 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 2232 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 233: // literal.terminal: float
#line 1168 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_FloatProducingIndex); }
#line 2238 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 234: // literal.terminal: boolean
#line 1169 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_BooleanLitIndex); }
#line 2244 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 235: // literal.terminal: nil
#line 1170 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_NilLitIndex); }
#line 2250 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 236: // literal.terminal: ascii
#line 1171 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_ASCIIIndex); }
#line 2256 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 237: // literal.terminal: block
#line 1172 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_BlockIndex); }
#line 2262 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 238: // literal.array.contents: %empty
#line 1177 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location); }
#line 2268 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 239: // literal.array.contents: expr_or_seq
#line 1179 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2274 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 240: // literal.array.contents: expr_or_seq COLON expr_or_seq
#line 1181 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2280 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 241: // literal.array.contents: KEYBINOP expr_or_seq
#line 1183 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2286 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 242: // literal.array.contents: literal.array.contents COMMA expr_or_seq
#line 1185 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[2].value.v_ArrayIndex), yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2292 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 243: // literal.array.contents: literal.array.contents COMMA expr_or_seq COLON expr_or_seq
#line 1187 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[4].value.v_ArrayIndex), yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2298 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 244: // literal.array.contents: literal.array.contents COMMA KEYBINOP expr_or_seq
#line 1189 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[3].value.v_ArrayIndex), yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2304 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 245: // literal.dictionary.entry: expr COLON expr_or_seq
#line 1197 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryEntryIndex) = cxt.create(DictionaryEntryNode{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2310 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 246: // literal.dictionary.entry: KEYBINOP expr_or_seq
#line 1199 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryEntryIndex) = cxt.create(DictionaryEntryNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2316 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 247: // literal.dictionary.entries: %empty
#line 1204 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.create(DictionaryNode{}, yylhs.location); }
#line 2322 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 248: // literal.dictionary.entries: literal.dictionary.entry
#line 1206 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.create(DictionaryNode{}, yylhs.location, (yystack_[0].value.v_DictionaryEntryIndex)); }
#line 2328 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 249: // literal.dictionary.entries: literal.dictionary.entries COMMA literal.dictionary.entry
#line 1208 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.graph->append((yystack_[2].value.v_DictionaryIndex), yylhs.location,  (yystack_[0].value.v_DictionaryEntryIndex)); }
#line 2334 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 250: // literal.dictionary: OPENPAREN literal.dictionary.entries comma.opt CLOSEPAREN
#line 1213 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->location(*(yystack_[2].value.v_DictionaryIndex)) = yylhs.location; (yylhs.value.v_DictionaryIndex) = (yystack_[2].value.v_DictionaryIndex); }
#line 2340 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 251: // literal.array: OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1218 "langutils/sc_parser/src/sc_grammar.y"
                {  cxt.graph->location(*(yystack_[2].value.v_ArrayIndex)) = yylhs.location; (yylhs.value.v_ArrayIndex) = (yystack_[2].value.v_ArrayIndex); }
#line 2346 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 252: // literal.array: HASH OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1220 "langutils/sc_parser/src/sc_grammar.y"
                {  
			cxt.graph->payload((yystack_[2].value.v_ArrayIndex)).is_immutable = true;
			cxt.graph->location(*(yystack_[2].value.v_ArrayIndex)) = yylhs.location;
			(yylhs.value.v_ArrayIndex) = (yystack_[2].value.v_ArrayIndex);
		}
#line 2356 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 253: // literal: literal.terminal
#line 1228 "langutils/sc_parser/src/sc_grammar.y"
                           { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_AnyPossiblyLiteralIndex); }
#line 2362 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 254: // literal: literal.array
#line 1229 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_ArrayIndex); }
#line 2368 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 255: // literal: literal.dictionary
#line 1230 "langutils/sc_parser/src/sc_grammar.y"
                             { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_DictionaryIndex); }
#line 2374 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 256: // name: NAME
#line 1238 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_NamedIdentifierIndex) = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 2380 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 257: // name: WHILE
#line 1239 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamedIdentifierIndex) = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 2386 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 258: // name.list: HASH name
#line 1245 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.create(NameList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2392 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 259: // name.list: HASH TILDE name
#line 1247 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.create(NameList{}, yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2398 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 260: // name.list: name.list COMMA name
#line 1249 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.graph->append((yystack_[2].value.v_NameListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2404 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 261: // name.list: name.list COMMA TILDE name
#line 1251 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.graph->append((yystack_[3].value.v_NameListIndex), yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2410 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 262: // name.pack: name.list
#line 1255 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[0].value.v_NameListIndex), cxt.create(Missing{}, yystack_[0].location)); }
#line 2416 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 263: // name.pack: name.list ELLIPSIS name
#line 1257 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[2].value.v_NameListIndex), (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2422 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 264: // name.pack: name.list ELLIPSIS TILDE name
#line 1259 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[3].value.v_NameListIndex), cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2428 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 265: // ascii: ASCII
#line 1269 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_ASCIIIndex) = cxt.create(ASCIINode{}, yylhs.location); }
#line 2434 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 266: // nil: NIL
#line 1271 "langutils/sc_parser/src/sc_grammar.y"
          { (yylhs.value.v_NilLitIndex) = cxt.create(NilNode{}, yylhs.location); }
#line 2440 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 267: // boolean: TRUE
#line 1274 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_BooleanLitIndex) = cxt.create(BooleanNode{true}, yylhs.location); }
#line 2446 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 268: // boolean: FALSE
#line 1275 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BooleanLitIndex) = cxt.create(BooleanNode{false}, yylhs.location); }
#line 2452 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 269: // symbol: SYMBOL_QUOTE
#line 1279 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_AnySymbolIndex) = cxt.create(SymbolQuote{}, yylhs.location); }
#line 2458 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 270: // symbol: SYMBOL_SLASH
#line 1280 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_AnySymbolIndex) = cxt.create(SymbolSlash{}, yylhs.location); }
#line 2464 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 271: // string: STRINGLINE
#line 1284 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_StringLitIndex) = cxt.create(StringLineList{}, yylhs.location, cxt.create(StringLineNode{}, yylhs.location)); }
#line 2470 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 272: // string: string STRINGLINE
#line 1285 "langutils/sc_parser/src/sc_grammar.y"
                            { (yylhs.value.v_StringLitIndex) = cxt.graph->append((yystack_[1].value.v_StringLitIndex), cxt.create(StringLineNode{}, yystack_[0].location)); }
#line 2476 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 273: // integer: INTEGER
#line 1289 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{}, yylhs.location); }
#line 2482 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 274: // integer: INTEGER_RADIX
#line 1290 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{IntNode::Kind::Radix}, yylhs.location); }
#line 2488 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 275: // integer: HEXADECIMAL
#line 1291 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{IntNode::Kind::Hexadecimal}, yylhs.location); }
#line 2494 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 276: // integer: MINUS integer
#line 1293 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Reaches into the previous integer and changes its sign.
			cxt.graph->payload((yystack_[0].value.v_IntLitIndex)).sign = IntNode::Sign::Negative;
			(yylhs.value.v_IntLitIndex) = (yystack_[0].value.v_IntLitIndex);
		}
#line 2504 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 277: // float.raw_unsigned: FLOAT
#line 1301 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{}, yylhs.location); }
#line 2510 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 278: // float.raw_unsigned: FLOAT_RADIX
#line 1302 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Radix}, yylhs.location); }
#line 2516 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 279: // float.raw_unsigned: FLOAT_EXPONENT
#line 1303 "langutils/sc_parser/src/sc_grammar.y"
                         { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Exponent}, yylhs.location); }
#line 2522 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 280: // float.raw_unsigned: FLOAT_INF
#line 1304 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Inf}, yylhs.location); }
#line 2528 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 281: // float.raw: float.raw_unsigned
#line 1309 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FloatLitIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2534 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 282: // float.raw: MINUS float.raw_unsigned
#line 1311 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload((yystack_[0].value.v_FloatLitIndex)).sign = FloatNode::Sign::Negative; (yylhs.value.v_FloatLitIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2540 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 283: // accidental.unsigned: ACCIDENTAL_STEPS
#line 1316 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = cxt.create(AccidentalNode{AccidentalNode::Kind::Steps}, yylhs.location); }
#line 2546 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 284: // accidental.unsigned: ACCIDENTAL_CENTS
#line 1318 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = cxt.create(AccidentalNode{AccidentalNode::Kind::Cents}, yylhs.location); }
#line 2552 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 285: // accidental: accidental.unsigned
#line 1323 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2558 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 286: // accidental: MINUS accidental.unsigned
#line 1325 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload((yystack_[0].value.v_AccidentalIndex)).sign = AccidentalNode::Sign::Negative; (yylhs.value.v_AccidentalIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2564 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 287: // float: float.raw
#line 1329 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_FloatProducingIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2570 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 288: // float: accidental
#line 1330 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_FloatProducingIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2576 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 289: // float: float.raw PI
#line 1331 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, (yystack_[1].value.v_FloatLitIndex)); }
#line 2582 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 290: // float: integer PI
#line 1332 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, (yystack_[1].value.v_IntLitIndex)); }
#line 2588 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 291: // float: PI
#line 1333 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, cxt.create(Missing{}, yylhs.location)); }
#line 2594 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 292: // float: MINUS PI
#line 1334 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{PiNode::Sign::Negative}, yylhs.location, cxt.create(Missing{}, yylhs.location)); }
#line 2600 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 293: // accessor: %empty
#line 1338 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::Private}, yylhs.location); }
#line 2606 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 294: // accessor: LESSTHAN
#line 1339 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyReadable}, yylhs.location); }
#line 2612 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 295: // accessor: GREATERTHAN
#line 1340 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyWriteable}, yylhs.location); }
#line 2618 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 296: // accessor: READWRITEVAR
#line 1342 "langutils/sc_parser/src/sc_grammar.y"
                { 
			(yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{
				combine<ReadWriteAccessor::ExternallyReadable, ReadWriteAccessor::ExternallyWriteable>()
			}, yylhs.location); 
		}
#line 2628 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;


#line 2632 "langutils/sc_parser/src/sc_grammar_parser.cpp"

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
  "MULTIPLY", "ADD", "PIPE", "READWRITEVAR", "DOT", "BACKTICK", "UMINUS",
  "$accept", "go", "file_as_func", "interpreted_function", "region.item",
  "region", "classOrExtList.list", "classOrExtList.item", "class",
  "class.super.opt", "class.slot.opt", "class.extension",
  "members.entry.item", "members.entry.list", "members.entry", "members",
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




  const short parser::yypact_ninf_ = -234;

  const short parser::yytable_ninf_ = -299;

  const short
  parser::yypact_[] =
  {
     283,    27,   795,   743,    10,    52,  -234,  -234,   200,    50,
    -234,  -234,  -234,    57,    89,  -234,  -234,   144,  2064,  1005,
    2626,   117,   114,  -234,  -234,  -234,  -234,  -234,  -234,  -234,
    -234,  -234,  -234,  -234,  -234,  -234,  -234,   212,  -234,   140,
     213,  -234,  -234,  -234,  -234,  -234,  -234,   683,   100,  2626,
    -234,  1177,  -234,  -234,  -234,  -234,  -234,  -234,  -234,  -234,
     136,  2781,  -234,  1546,   141,  -234,  -234,  -234,  -234,   106,
     158,    66,  -234,  -234,  -234,  -234,   131,   107,  -234,   180,
    -234,  -234,  -234,  -234,  1005,  -234,  2626,    49,  -234,  2781,
     902,  -234,   250,   247,  -234,  -234,  2116,  2116,   256,  -234,
    -234,  -234,   257,   227,   262,  2626,  2626,  1065,  2626,  2781,
     251,   222,   264,  -234,  2626,  2167,  -234,  2626,   683,  -234,
    -234,  -234,  -234,    26,  -234,  -234,   266,   671,   288,  1338,
     285,  -234,   294,  2781,  2064,   114,  -234,   267,  2064,  1701,
    -234,   146,  -234,   146,   232,  -234,   295,   101,   111,  -234,
    -234,   114,   184,   122,  -234,   220,  -234,  -234,  -234,  -234,
     114,  -234,   303,  2673,  -234,  -234,  -234,  1390,  1597,  -234,
    -234,  -234,   187,   259,  2626,    49,   954,   321,  1753,  2626,
     146,   255,   279,  2626,  2626,  -234,  -234,  -234,  1234,   312,
    2781,   847,  2626,   324,    49,   330,  -234,   607,  -234,  -234,
    -234,  -234,  -234,   559,   320,   325,  -234,  2626,  -234,  2626,
    2218,   326,   327,  2626,   171,  -234,   244,  2626,  -234,  2626,
    2269,  -234,    49,  1546,  2320,   331,   264,  -234,  2626,   264,
     146,  2626,  2626,  -234,  -234,   334,   338,  -234,  -234,  -234,
     146,   140,  2626,  2626,  2626,  2626,   203,   114,   114,   329,
    2626,  2626,     4,  -234,   114,    11,  -234,  2320,   342,  2751,
    -234,  -234,    49,   298,  2626,    38,  1649,  1805,   146,   128,
     265,  -234,   290,   352,    49,  -234,   146,   344,  2781,   114,
    -234,   114,  -234,   345,  2781,    49,   346,  1442,   350,  2626,
    -234,  2781,  -234,   360,  -234,  -234,   359,   358,  -234,  -234,
     638,   361,  -234,  -234,    21,    31,   120,  -234,   353,   363,
    1961,  1961,  2781,  -234,  2626,   315,  -234,  -234,   364,  2626,
    2371,  1857,   146,  -234,   335,  -234,   368,   369,    49,  2626,
     671,  -234,  -234,   365,  2781,   372,  -234,  -234,  -234,  2012,
    -234,  -234,  -234,   371,  2781,   375,  2781,  -234,   114,   130,
     291,  -234,   376,  2781,   114,  -234,    30,  -234,  2712,   377,
    2320,  -234,   382,  2626,   374,   319,  2422,   337,  2626,   119,
     146,  -234,  1909,  2626,   146,  2626,  -234,  -234,  -234,   392,
     146,    20,  -234,  -234,  -234,   384,   394,    49,   396,  2781,
    -234,  -234,  1125,  -234,  -234,  -234,  -234,  -234,  -234,  -234,
     387,   140,  -234,   387,  -234,   387,    22,   607,  -234,   114,
      87,  -234,   389,  -234,   263,   389,  -234,  2626,  -234,   362,
    -234,   395,   146,   398,  2473,  -234,  -234,   411,  -234,  -234,
    -234,  -234,  -234,   412,  2626,  2626,  -234,   114,  -234,   366,
     114,  -234,  2320,  2790,  -234,  -234,   414,  -234,  2781,   354,
    2626,   373,   419,  2626,   420,   378,  2524,  -234,  -234,   421,
    2781,  -234,   422,  -234,  2626,  -234,   432,  -234,   429,  -234,
    1286,    -7,  -234,  -234,   436,   133,  2626,  2626,  1961,   438,
    2626,   400,   442,  -234,  2575,  -234,   146,   146,  -234,   443,
    -234,  -234,   444,  2781,   445,  -234,   397,   448,  2320,  -234,
    -234,  2626,  2781,  2626,   409,  2781,   410,  2626,   413,   456,
    -234,  -234,  2781,  -234,   465,  -234,   457,    49,  -234,  -234,
    2626,  2626,  -234,  -234,  -234,  -234,  -234,  2626,  -234,  -234,
     460,  -234,  -234,  -234,  -234,  -234,  -234,   462,  2781,  2781,
    2626,  2626,  2781,  2626,   415,  -234,  1494,   467,   464,  -234,
    -234,  -234,  -234,  2781,  2781,  2781,  2626,  -234,    49,  -234,
    -234,  2781,   470,  -234
  };

  const short
  parser::yydefact_[] =
  {
       7,    37,     0,     0,     0,     0,     4,     5,     0,     0,
      29,    31,    32,     0,    35,     3,    25,    65,   238,     0,
       0,     0,     0,   256,   273,   274,   275,   277,   278,   279,
     280,   283,   284,   269,   270,   271,   265,   154,   136,     0,
       0,   266,   267,   268,   291,    66,   257,     0,     0,     0,
      24,     0,   237,   137,   148,   149,   143,   147,   144,   145,
     146,    19,   134,     0,   297,   253,   255,   254,   133,   135,
     262,     0,   236,   235,   234,   230,   231,   232,   281,   287,
     285,   288,   233,    82,     0,   298,     0,   297,    75,    80,
       0,    81,     0,     0,     1,    28,     0,   298,     0,     6,
      30,    39,     0,     0,     0,     0,     0,     0,     0,   165,
     297,   239,   299,   139,     0,     0,    86,    95,    90,    88,
      89,    91,    92,     0,    87,    94,     0,   165,     0,     0,
       0,   248,   299,    20,   238,     0,   258,   140,   238,     0,
      71,   108,    72,    69,     0,   227,   229,   222,     0,   201,
     195,     0,   299,   179,   292,     0,   276,   282,   286,   202,
       0,   203,   299,   186,   155,   211,   210,     0,     0,    95,
      90,    93,     0,    97,     0,   297,     0,     0,     0,     0,
     103,     0,     0,     0,     0,   272,   290,   289,     0,   297,
      79,     0,     0,     0,   297,     0,    16,     0,    27,    26,
       2,    38,    36,     0,     0,     0,   241,   298,   167,     0,
     300,     0,     0,     0,     0,   246,     0,     0,   138,     0,
       0,    22,   297,     0,   300,     0,   299,   259,     0,   299,
      69,     0,     0,   214,   215,   299,     0,    73,    74,   109,
      70,     0,     0,     0,     0,     0,     0,   300,     0,     0,
       0,     0,     0,   300,     0,     0,    65,   247,     0,     0,
     185,    68,   297,   142,     0,     0,     0,     0,    69,   219,
       0,    96,   152,     0,   297,    10,     0,     0,   156,     0,
     260,     0,   263,     0,   159,   297,     0,     0,     0,     0,
      76,    77,    15,     0,    12,    59,    91,     0,    58,    61,
       0,     0,    53,    52,     0,     0,     0,    50,   297,     0,
       0,     0,   166,   240,     0,   242,   251,   127,     0,     0,
       0,     0,   100,   245,     0,   123,     0,     0,   297,     0,
       0,   249,   250,     0,   157,     0,   110,   212,   213,   300,
     217,   218,   228,     0,   223,     0,   225,   199,     0,   182,
       0,   196,     0,   180,     0,   207,     0,   204,   191,     0,
     247,   187,     0,     0,     0,   141,     0,   151,     0,     0,
      69,   105,     0,     0,    69,     0,    83,    84,     8,     0,
     102,    69,   261,   264,   160,     0,     0,   297,     0,    78,
      11,    60,     0,    62,    40,    48,   294,   295,   296,    42,
      45,     0,    47,    44,    49,    46,   298,     0,    34,     0,
       0,   176,   299,   170,   135,   299,   244,     0,   131,     0,
     132,     0,     0,     0,     0,   124,    21,     0,   252,   107,
     216,   224,   226,     0,     0,     0,   197,     0,   181,     0,
       0,   205,   247,     0,   190,   188,     0,    67,   162,   119,
       0,   118,     0,     0,     0,   150,     0,   106,   220,     0,
     153,   104,     0,     9,     0,   101,     0,    14,     0,    18,
       0,   293,    41,    51,     0,     0,     0,     0,   300,     0,
       0,     0,     0,   243,     0,   128,    99,    69,   126,     0,
      23,   200,     0,   183,     0,   208,     0,     0,   247,   192,
     189,     0,   161,     0,   117,   164,   112,     0,   113,     0,
     221,    85,   158,    13,     0,    55,   297,   297,    43,    33,
       0,     0,   178,   177,   171,   169,   172,     0,   168,   130,
       0,    98,   125,   184,   198,   206,   193,     0,   122,   121,
       0,     0,   163,     0,   111,    17,     0,     0,     0,   174,
     173,   129,   194,   120,   116,   115,     0,    57,   297,    54,
     175,   114,     0,    56
  };

  const short
  parser::yypgoto_[] =
  {
    -234,  -234,  -234,  -234,   110,  -234,  -234,   468,  -234,  -234,
    -234,  -234,     5,  -181,    72,  -234,  -234,   183,   181,  -234,
    -196,  -234,    28,  -225,   -31,   -60,   292,  -234,  -178,    -9,
    -234,  -234,  -234,  -234,  -234,  -234,  -234,  -234,  -234,    -2,
    -234,   125,   254,   176,    14,  -234,  -234,    12,    90,   149,
    -234,  -155,   317,  -234,  -233,  -234,    18,  -234,   -83,   271,
    -234,  -234,  -234,  -159,   277,  -234,   -37,  -234,  -234,  -234,
    -234,  -234,   -35,   -26,  -234,   -19,  -234,  -234,  -234,     3,
    -123
  };

  const short
  parser::yydefgoto_[] =
  {
       0,     5,     6,     7,    50,     8,     9,    10,    11,   104,
      14,    12,   399,   400,   307,   308,   297,   298,   299,   300,
     301,    51,    52,   239,   240,    87,    88,   271,   125,   173,
     174,    53,    54,    55,    56,    57,    58,    59,    60,   109,
     110,   233,    62,   412,   413,   152,   162,   166,   167,   234,
     235,   236,   143,   374,   145,   146,    91,    65,   112,   131,
     132,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,   401,    92,
     211
  };

  const short
  parser::yytable_[] =
  {
      61,    89,   148,   175,   260,   336,   141,   309,   342,   225,
     126,    98,   156,   265,    63,    90,   354,   127,   133,   302,
      64,   157,   395,   277,    17,   302,    23,   159,   158,   249,
     194,   129,   402,    13,  -293,   -93,  -293,   130,   180,   255,
      93,    23,   440,   371,  -293,   365,  -293,   164,   396,   397,
      99,   226,    94,   398,   304,   229,   305,   306,    46,   191,
     192,    89,    45,   355,   101,   140,   160,   177,  -293,   222,
     357,   464,    23,    46,   183,   126,   396,   397,  -293,   366,
       1,   398,   127,   156,   190,   161,   396,   397,    89,   441,
     193,   398,   157,   195,    61,    61,   188,   140,   126,   158,
     361,   159,   189,   333,    46,   127,   335,   262,     4,   242,
      17,   369,   340,   208,   178,    23,   274,   184,   302,   244,
     156,   404,   302,   134,   403,   405,   455,    89,   285,    23,
     250,   135,    23,  -293,   476,  -293,   372,   477,   434,   103,
     160,   520,   168,   111,   128,   457,   186,    46,    45,   461,
      17,   176,   243,   144,   105,    23,   465,   179,   185,   161,
     456,    46,   245,   328,    46,    89,   423,  -293,   472,   237,
     181,   140,   272,   251,    89,   396,   397,   278,   273,   373,
     398,   435,   284,   319,   521,   322,    89,    46,    45,    89,
     291,   286,   288,   266,   106,   267,   247,   293,   182,   444,
    -297,    95,    23,    96,   148,   312,   198,   199,   237,   128,
      97,   474,   320,   347,   149,   348,    17,   459,   138,   187,
     139,    89,   330,   150,   248,   327,   334,   387,    23,   302,
     204,   205,   128,   206,    46,   377,    24,    25,    26,   212,
     214,   344,   215,   346,   140,   380,   135,    23,    17,   353,
     196,   197,   321,   151,    45,   127,   200,   202,   140,   111,
      46,   207,   531,   111,   201,   362,   203,    17,   237,   279,
      23,   178,   209,   375,   155,   216,   210,   379,    23,    46,
      23,    24,    25,    26,   499,    89,    45,   389,   385,   479,
     102,   142,   482,   281,    23,   223,   140,   218,   136,   137,
     219,   436,    46,   437,   140,    45,   224,   241,   283,   480,
      46,   407,    46,     1,   179,   253,   147,   153,   228,   155,
     270,   275,   287,   142,   292,   163,    46,     2,     3,   220,
     294,   427,   310,   316,   313,   315,   317,   311,   318,   351,
     332,     4,   323,   254,   324,   326,   339,   341,   134,   363,
     237,   172,   378,   381,   384,   386,   337,   338,   127,   388,
     390,   448,   392,   406,   148,   417,   394,   343,   408,   345,
     450,   460,   428,   418,    23,   352,   424,   425,   426,   429,
     431,   449,   359,   180,   432,   438,   445,   447,   453,   364,
     468,   486,   463,   466,   467,   238,   469,   142,   140,   471,
     163,   478,   140,   484,   485,   501,    46,   487,   237,   140,
     517,   116,   227,   170,   119,   120,   121,   122,   171,   124,
     490,   136,   491,   500,   503,   495,   504,   506,   246,   507,
     510,   511,   513,   493,   238,   411,   411,   252,   514,   416,
     127,   519,   140,   525,   419,   421,   527,   528,   502,   269,
     140,   505,   532,   533,   215,   534,   535,   536,   280,   282,
     540,   541,   512,   544,   543,   545,   556,    85,    89,   551,
     142,   552,   559,   560,   303,   563,   518,   100,   473,   391,
     303,   393,   470,   290,   142,   446,   558,   415,   430,   268,
       0,   452,   524,   454,   238,   331,   127,     0,     0,   538,
     462,   539,     0,     0,     0,   542,     0,     0,     0,     0,
       0,     0,     0,     0,   237,   140,     0,     0,   147,   546,
     547,     0,   142,     0,   349,   350,     0,     0,     0,     0,
     142,   356,   358,     0,     0,     0,     0,     0,   553,   554,
       0,   555,   483,     0,    89,     0,     0,   376,     0,   489,
       0,     0,     0,     0,   561,     0,   382,     0,   383,   492,
     295,   562,     0,     0,   -63,     0,     0,   497,     0,     0,
       0,     0,     0,   303,    23,     0,   238,   303,     0,     0,
       0,   509,     0,     0,     0,     0,     0,   414,   414,     0,
       0,   304,     0,   305,   306,     0,     0,     0,     0,     0,
       0,   522,   523,   411,     0,   526,    46,     0,   295,   530,
       0,   116,   -63,   170,   119,   120,   296,   122,   171,   124,
       0,     0,    23,   537,   142,   433,     0,     0,   142,     0,
       0,   439,     0,     0,   238,   142,     0,     0,     0,   295,
       0,     0,     0,   -64,     0,   548,   549,     0,     0,     0,
       0,     0,   550,    23,    46,     0,     0,     0,     0,   116,
       0,   170,   119,   120,   296,   122,   171,   124,   142,     0,
       0,     0,     0,     0,     0,     0,   142,     0,   147,     0,
       0,     0,     0,     0,   303,    46,   475,     0,     0,     0,
     116,   481,   170,   119,   120,   296,   122,   171,   124,    24,
      25,    26,    27,    28,    29,    30,    31,    32,     0,     0,
       0,     0,     0,     0,   494,     0,     0,   496,     0,     0,
       0,   217,   154,   116,   169,   170,   119,   120,   121,   122,
     171,   124,   172,     0,     0,     0,     0,   155,     0,     0,
     238,   142,     0,  -297,    83,     0,     0,    17,     0,    18,
       0,    84,     0,    85,    86,   414,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,    37,    38,    39,    40,     0,     0,    41,
      42,    43,    44,     0,     0,    45,     0,     0,     0,     0,
      46,     0,     0,     0,     0,    15,    16,    47,     0,    17,
       0,    18,    48,    19,     0,    49,    20,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,    39,    40,     0,
       0,    41,    42,    43,    44,     0,     0,    45,     0,     0,
       0,     0,    46,     0,     0,     0,     0,  -298,    83,    47,
       0,    17,  -298,    18,    48,   107,  -298,    49,   289,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,    39,
       0,     0,     0,    41,    42,    43,    44,     0,     0,    45,
       0,     0,     0,     0,    46,     0,     0,     0,     0,     0,
       0,    47,  -297,    83,     0,     0,    17,     0,    18,    49,
     107,     0,    85,    86,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,    39,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,  -298,    83,    47,     0,    17,     0,
      18,     0,   107,     0,    49,    86,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,    39,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,   113,     0,    47,    17,
       0,    18,     0,   107,  -247,     0,    49,  -247,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,    39,    40,     0,
       0,    41,    42,    43,    44,     0,   114,    45,     0,     0,
       0,     0,    46,     0,     0,   115,     0,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   113,    49,     0,    17,
       0,    18,     0,   107,  -247,     0,     0,  -247,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,     0,     0,     0,
       0,    41,    42,    43,    44,     0,   114,    45,     0,     0,
       0,     0,    46,     0,     0,   115,     0,   116,   117,   118,
     119,   120,   121,   122,   171,   124,   165,    49,     0,  -209,
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
       0,  -209,     0,     0,     0,    83,    48,     0,    17,  -209,
      18,     0,   107,  -297,    85,    86,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,    39,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,     0,    83,    47,     0,
      17,   515,    18,     0,   107,     0,    49,    86,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,   516,    37,    38,    39,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,    83,
      47,     0,    17,     0,    18,     0,   107,   221,    49,    86,
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     0,    37,    38,
      39,     0,     0,     0,    41,    42,    43,    44,     0,     0,
      45,     0,     0,     0,     0,    46,     0,     0,     0,     0,
       0,    83,    47,     0,    17,   261,    18,     0,   107,     0,
      49,    86,     0,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
      37,    38,    39,     0,     0,     0,    41,    42,    43,    44,
       0,     0,    45,     0,     0,     0,     0,    46,     0,     0,
       0,     0,     0,    83,    47,     0,    17,     0,    18,     0,
     107,  -298,    49,    86,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,    39,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,    83,    47,     0,    17,   557,
      18,     0,   107,     0,    49,    86,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,    39,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,     0,    83,    47,     0,
      17,     0,    18,     0,   107,     0,    49,    86,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,    39,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,     0,
      47,    17,     0,    18,   263,   107,     0,     0,    49,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,     0,
       0,     0,     0,    41,    42,    43,    44,     0,   264,    45,
       0,     0,     0,     0,    46,     0,     0,     0,     0,     0,
     231,    47,     0,    17,   232,    18,   367,   107,     0,    49,
       0,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,     0,    37,
      38,     0,     0,     0,     0,    41,    42,    43,    44,     0,
     368,    45,     0,     0,     0,     0,    46,     0,     0,     0,
       0,     0,   231,    47,     0,    17,   232,    18,     0,   107,
     230,    49,     0,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,   231,    47,     0,    17,   232,    18,
       0,   107,   276,    49,     0,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,    37,    38,     0,     0,     0,     0,    41,
      42,    43,    44,     0,     0,    45,     0,     0,     0,     0,
      46,     0,     0,     0,     0,     0,   231,    47,     0,    17,
     232,    18,     0,   107,   370,    49,     0,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,     0,     0,     0,
       0,    41,    42,    43,    44,     0,     0,    45,     0,     0,
       0,     0,    46,     0,     0,     0,     0,     0,   231,    47,
       0,    17,   232,    18,     0,   107,   422,    49,     0,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,     0,
       0,     0,     0,    41,    42,    43,    44,     0,     0,    45,
       0,     0,     0,     0,    46,     0,     0,     0,     0,     0,
     231,    47,     0,    17,   232,    18,     0,   107,   458,    49,
       0,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,     0,    37,
      38,     0,     0,     0,     0,    41,    42,    43,    44,     0,
       0,    45,     0,     0,     0,     0,    46,     0,     0,     0,
       0,     0,   231,    47,     0,    17,   232,    18,     0,   107,
       0,    49,     0,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,   409,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,   410,     0,     0,     0,    47,    17,     0,    18,     0,
     107,     0,     0,    49,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,   231,    47,     0,    17,   232,
      18,     0,   107,     0,    49,     0,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,     0,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,     0,   108,    47,     0,
      17,     0,    18,     0,    19,     0,    49,    20,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,     0,
      47,    17,     0,    18,     0,   107,     0,     0,    49,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,     0,
       0,     0,     0,    41,    42,    43,    44,     0,   213,    45,
       0,     0,     0,     0,    46,     0,     0,     0,     0,     0,
       0,    47,    17,     0,    18,     0,   107,     0,     0,    49,
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     0,    37,    38,
       0,     0,     0,     0,    41,    42,    43,    44,     0,     0,
      45,     0,     0,     0,     0,    46,     0,     0,     0,     0,
       0,   314,    47,    17,     0,    18,     0,   107,   325,     0,
      49,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,     0,    37,
      38,     0,     0,     0,     0,    41,    42,    43,    44,     0,
       0,    45,     0,     0,     0,     0,    46,     0,     0,     0,
       0,     0,     0,    47,    17,     0,    18,     0,   107,     0,
       0,    49,     0,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
      37,    38,     0,     0,     0,     0,    41,    42,    43,    44,
       0,     0,    45,     0,     0,     0,     0,    46,     0,     0,
       0,     0,     0,   329,    47,    17,     0,    18,     0,   107,
     420,     0,    49,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,     0,    47,    17,     0,    18,   451,
     107,     0,     0,    49,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,     0,    47,    17,     0,    18,
       0,   107,   488,     0,    49,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,    37,    38,     0,     0,     0,     0,    41,
      42,    43,    44,     0,     0,    45,     0,     0,     0,     0,
      46,     0,     0,     0,     0,     0,     0,    47,    17,     0,
      18,   508,   107,     0,     0,    49,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,     0,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,     0,     0,    47,    17,
       0,    18,     0,   107,   529,     0,    49,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,     0,     0,     0,
       0,    41,    42,    43,    44,     0,     0,    45,     0,     0,
       0,     0,    46,     0,     0,     0,     0,     0,     0,    47,
      17,     0,    18,     0,   107,     0,     0,    49,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,   256,     0,    18,
      47,   257,     0,     0,     0,     0,   258,     0,    49,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,     0,     0,     0,     0,     0,     0,    41,
      42,    43,    44,     0,     0,    45,   256,     0,    18,     0,
     442,     0,     0,     0,   259,   258,     0,    47,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,     0,     0,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,   256,     0,    18,     0,   360,
       0,     0,     0,   443,   258,     0,    47,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,     0,     0,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,   256,     0,    18,     0,   498,     0,
       0,     0,     0,   258,     0,    47,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
       0,     0,     0,     0,     0,     0,    41,    42,    43,    44,
       0,     0,    45,   116,   169,   170,   119,   120,   121,   122,
     171,   124,   172,     0,    47
  };

  const short
  parser::yycheck_[] =
  {
       2,     3,    39,    63,   163,   230,    37,   203,   241,   132,
      19,     8,    47,   168,     2,     3,    12,    19,    20,   197,
       2,    47,     1,   178,     4,   203,    15,     1,    47,   152,
      90,    19,     1,     6,    13,     9,    15,    19,    69,   162,
      30,    15,    12,   268,    13,     7,    15,    49,    55,    56,
       0,   134,     0,    60,    32,   138,    34,    35,    47,    10,
      11,    63,    42,    59,     7,    37,    40,    64,    47,   129,
      59,    51,    15,    47,     8,    84,    55,    56,    47,    41,
      30,    60,    84,   118,    86,    59,    55,    56,    90,    59,
      87,    60,   118,    90,    96,    97,    84,    69,   107,   118,
     259,     1,    84,   226,    47,   107,   229,   167,    58,     8,
       4,   266,   235,   110,     8,    15,   176,    51,   296,     8,
     155,     1,   300,     6,   305,   306,     7,   129,   188,    15,
       8,    14,    15,    13,    47,    15,     8,    50,     8,    50,
      40,     8,     6,    18,    19,   370,    39,    47,    42,   374,
       4,    10,    51,    13,    10,    15,   381,    51,    27,    59,
      41,    47,    51,   223,    47,   167,   321,    47,   401,   141,
      12,   143,   174,    51,   176,    55,    56,   179,   175,    51,
      60,    51,   184,    12,    51,   216,   188,    47,    42,   191,
     192,   188,   189,     6,    50,     8,    12,   194,    40,   358,
       0,     1,    15,     3,   241,   207,    96,    97,   180,    84,
      10,   407,    41,    10,     1,    12,     4,   372,     6,    39,
       8,   223,   224,    10,    40,   222,   228,   287,    15,   407,
     105,   106,   107,   108,    47,   270,    16,    17,    18,   114,
     115,   243,   117,   245,   216,   276,    14,    15,     4,   251,
       0,     4,     8,    40,    42,   257,     0,    30,   230,   134,
      47,    10,   487,   138,     7,   262,     4,     4,   240,    14,
      15,     8,    50,     8,    54,     9,    12,   274,    15,    47,
      15,    16,    17,    18,   443,   287,    42,   289,   285,   412,
      13,    37,   415,    14,    15,    10,   268,     9,    21,    22,
      12,    10,    47,    12,   276,    42,    12,    12,   183,    46,
      47,   308,    47,    30,    51,    12,    39,    40,    51,    54,
      61,     0,    10,    69,     0,    48,    47,    44,    45,    41,
       0,   328,    12,     7,   209,   210,     9,    12,   213,    10,
       9,    58,   217,    40,   219,   220,    12,     9,     6,    51,
     322,    61,     0,     9,     9,     9,   231,   232,   360,     9,
       0,   363,     4,    10,   401,    50,     5,   242,     5,   244,
      51,   373,     7,     9,    15,   250,    41,     9,     9,     7,
       9,     7,   257,   414,     9,     9,     9,     5,    51,   264,
     387,   422,     0,     9,     0,   141,     0,   143,   370,    12,
     123,    12,   374,    41,     9,    51,    47,     9,   380,   381,
     470,    52,   135,    54,    55,    56,    57,    58,    59,    60,
       9,   144,    10,     9,    51,    59,     7,     7,   151,    51,
       9,     9,     0,   435,   180,   310,   311,   160,     9,   314,
     442,     5,   414,     5,   319,   320,    46,     5,   450,   172,
     422,   453,     9,     9,   329,    10,    59,     9,   181,   182,
      51,    51,   464,     7,    51,     0,    51,    10,   470,     9,
     216,     9,     5,     9,   197,     5,   471,     9,   406,   296,
     203,   300,   392,   191,   230,   360,   546,   311,   339,   172,
      -1,   366,   478,   368,   240,   224,   498,    -1,    -1,   501,
     375,   503,    -1,    -1,    -1,   507,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   486,   487,    -1,    -1,   241,   516,
     517,    -1,   268,    -1,   247,   248,    -1,    -1,    -1,    -1,
     276,   254,   255,    -1,    -1,    -1,    -1,    -1,   540,   541,
      -1,   543,   417,    -1,   546,    -1,    -1,   270,    -1,   424,
      -1,    -1,    -1,    -1,   556,    -1,   279,    -1,   281,   434,
       1,   558,    -1,    -1,     5,    -1,    -1,   442,    -1,    -1,
      -1,    -1,    -1,   296,    15,    -1,   322,   300,    -1,    -1,
      -1,   456,    -1,    -1,    -1,    -1,    -1,   310,   311,    -1,
      -1,    32,    -1,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,   476,   477,   478,    -1,   480,    47,    -1,     1,   484,
      -1,    52,     5,    54,    55,    56,    57,    58,    59,    60,
      -1,    -1,    15,   498,   370,   348,    -1,    -1,   374,    -1,
      -1,   354,    -1,    -1,   380,   381,    -1,    -1,    -1,     1,
      -1,    -1,    -1,     5,    -1,   520,   521,    -1,    -1,    -1,
      -1,    -1,   527,    15,    47,    -1,    -1,    -1,    -1,    52,
      -1,    54,    55,    56,    57,    58,    59,    60,   414,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   422,    -1,   401,    -1,
      -1,    -1,    -1,    -1,   407,    47,   409,    -1,    -1,    -1,
      52,   414,    54,    55,    56,    57,    58,    59,    60,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    -1,   437,    -1,    -1,   440,    -1,    -1,
      -1,    50,    39,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    -1,    -1,    -1,    -1,    54,    -1,    -1,
     486,   487,    -1,     0,     1,    -1,    -1,     4,    -1,     6,
      -1,     8,    -1,    10,    11,   478,    13,    14,    15,    16,
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
      -1,    -1,    42,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    -1,    54
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,    30,    44,    45,    58,    65,    66,    67,    69,    70,
      71,    72,    75,     6,    74,     0,     1,     4,     6,     8,
      11,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    30,    31,    32,
      33,    36,    37,    38,    39,    42,    47,    54,    59,    62,
      68,    85,    86,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   106,   111,   120,   121,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,     1,     8,    10,    11,    89,    90,   103,
     111,   120,   143,    30,     0,     1,     3,    10,   143,     0,
      71,     7,   128,    50,    73,    10,    50,     8,    53,   103,
     104,   105,   122,     1,    41,    50,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    92,    93,   103,   105,   111,
     120,   123,   124,   103,     6,    14,   128,   128,     6,     8,
      86,    88,   106,   116,    13,   118,   119,   128,   130,     1,
      10,    40,   109,   128,    39,    54,   136,   137,   139,     1,
      40,    59,   110,   128,   103,     1,   111,   112,     6,    53,
      54,    59,    61,    93,    94,    89,    10,   143,     8,    51,
      88,    12,    40,     8,    51,    27,    39,    39,   111,   120,
     103,    10,    11,   143,    89,   143,     0,     4,    68,    68,
       0,     7,    30,     4,   105,   105,   105,    10,   143,    50,
      12,   144,   105,    41,   105,   105,     9,    50,     9,    12,
      41,     9,    89,    10,    12,   144,   122,   128,    51,   122,
       9,    53,    57,   105,   113,   114,   115,    86,   106,    87,
      88,    12,     8,    51,     8,    51,   128,    12,    40,   144,
       8,    51,   128,    12,    40,   144,     4,     8,    13,    51,
     127,     5,    89,     7,    41,   115,     6,     8,   116,   128,
      61,    91,   103,   143,    89,     0,     9,   115,   103,    14,
     128,    14,   128,   105,   103,    89,   143,    10,   143,    11,
      90,   103,     0,   143,     0,     1,    57,    80,    81,    82,
      83,    84,    92,   128,    32,    34,    35,    78,    79,    84,
      12,    12,   103,   105,    53,   105,     7,     9,   105,    12,
      41,     8,    88,   105,   105,     9,   105,   143,    89,    53,
     103,   123,     9,   144,   103,   144,    87,   105,   105,    12,
     144,     9,   118,   105,   103,   105,   103,    10,    12,   128,
     128,    10,   105,   103,    12,    59,   128,    59,   128,   105,
       8,   127,   143,    51,   105,     7,    41,     7,    41,   115,
       9,    87,     8,    51,   117,     8,   128,   136,     0,   143,
      88,     9,   128,   128,     9,   143,     9,    89,     9,   103,
       0,    81,     4,    82,     5,     1,    55,    56,    60,    76,
      77,   142,     1,    77,     1,    77,    10,   143,     5,    32,
      50,   105,   107,   108,   128,   107,   105,    50,     9,   105,
       9,   105,     9,   115,    41,     9,     9,   143,     7,     7,
     113,     9,     9,   128,     8,    51,    10,    12,     9,   128,
      12,    59,     8,    51,   127,     9,   105,     5,   103,     7,
      51,     7,   105,    51,   105,     7,    41,    87,     9,   115,
     103,    87,   105,     0,    51,    87,     9,     0,   143,     0,
     112,    12,   118,    78,    84,   128,    47,    50,    12,   144,
      46,   128,   144,   105,    41,     9,    88,     9,     9,   105,
       9,    10,   105,   103,   128,    59,   128,   105,     8,   127,
       9,    51,   103,    51,     7,   103,     7,    51,     7,   105,
       9,     9,   103,     0,     9,     5,    29,    89,    76,     5,
       8,    51,   105,   105,   108,     5,   105,    46,     5,     9,
     105,    87,     9,     9,    10,    59,     9,   105,   103,   103,
      51,    51,   103,    51,     7,     0,   143,   143,   105,   105,
     105,     9,     9,   103,   103,   103,    51,     5,    89,     5,
       9,   103,   143,     5
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    64,    65,    65,    65,    65,    65,    65,    66,    66,
      66,    67,    67,    67,    67,    67,    67,    67,    67,    68,
      68,    68,    68,    68,    69,    69,    69,    69,    69,    70,
      70,    71,    71,    72,    72,    73,    73,    74,    74,    74,
      75,    76,    77,    77,    78,    78,    78,    78,    78,    78,
      79,    79,    80,    80,    81,    81,    81,    81,    82,    82,
      82,    83,    83,    84,    84,    85,    85,    86,    86,    87,
      87,    88,    88,    88,    88,    89,    89,    89,    89,    89,
      90,    90,    90,    91,    91,    91,    92,    92,    92,    92,
      92,    92,    92,    92,    93,    93,    94,    94,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    96,    96,    96,    97,    97,    97,    98,    98,    98,
      99,    99,    99,   100,   100,   100,   100,   100,   101,   101,
     101,   101,   101,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   104,   104,   105,   106,   106,
     107,   107,   108,   108,   108,   108,   108,   108,   108,   109,
     109,   109,   109,   109,   109,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   112,
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
       1,     1,     2,     1,     1,     1,     2,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     2,     1,     1,     2,
       2,     1,     2,     0,     1,     1,     1,     0,     1,     0,
       1
  };




#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   243,   243,   244,   245,   246,   247,   248,   256,   259,
     262,   273,   276,   279,   282,   285,   288,   291,   294,   303,
     306,   308,   311,   314,   319,   322,   328,   331,   334,   351,
     353,   358,   359,   363,   366,   374,   375,   379,   380,   381,
     385,   394,   399,   401,   406,   408,   410,   412,   422,   432,
     445,   447,   457,   458,   462,   465,   468,   471,   477,   478,
     489,   501,   502,   506,   507,   516,   517,   521,   523,   528,
     529,   533,   534,   535,   536,   540,   541,   542,   547,   552,
     560,   561,   562,   581,   582,   583,   587,   588,   589,   590,
     591,   592,   593,   594,   598,   599,   603,   605,   610,   613,
     616,   620,   623,   626,   629,   632,   635,   645,   648,   653,
     659,   674,   681,   688,   697,   704,   711,   721,   727,   733,
     742,   748,   754,   763,   768,   773,   778,   783,   791,   796,
     802,   807,   812,   825,   828,   831,   834,   837,   840,   847,
     855,   858,   861,   864,   867,   870,   876,   877,   878,   879,
     881,   884,   887,   890,   893,   894,   896,   898,   901,   904,
     907,   913,   916,   919,   922,   930,   931,   935,   943,   946,
     951,   953,   958,   960,   962,   964,   966,   968,   970,   983,
     985,   990,   993,   996,  1001,  1007,  1009,  1011,  1013,  1015,
    1017,  1019,  1021,  1023,  1025,  1030,  1031,  1032,  1034,  1036,
    1041,  1050,  1060,  1070,  1072,  1074,  1076,  1078,  1083,  1095,
    1096,  1099,  1104,  1105,  1106,  1110,  1111,  1114,  1116,  1119,
    1120,  1122,  1131,  1133,  1135,  1141,  1143,  1151,  1153,  1157,
    1165,  1166,  1167,  1168,  1169,  1170,  1171,  1172,  1176,  1178,
    1180,  1182,  1184,  1186,  1188,  1196,  1198,  1203,  1205,  1207,
    1212,  1217,  1219,  1228,  1229,  1230,  1238,  1239,  1244,  1246,
    1248,  1250,  1254,  1256,  1258,  1269,  1271,  1274,  1275,  1279,
    1280,  1284,  1285,  1289,  1290,  1291,  1292,  1301,  1302,  1303,
    1304,  1308,  1310,  1315,  1317,  1322,  1324,  1329,  1330,  1331,
    1332,  1333,  1334,  1338,  1339,  1340,  1341,  1353,  1353,  1354,
    1354
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
      55,    56,    57,    58,    59,    60,    61,    62,    63
    };
    // Last valid token kind.
    const int code_max = 318;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 68 "langutils/sc_parser/src/sc_grammar.y"
} } } // sc::ast::parser
#line 4032 "langutils/sc_parser/src/sc_grammar_parser.cpp"

#line 1356 "langutils/sc_parser/src/sc_grammar.y"

