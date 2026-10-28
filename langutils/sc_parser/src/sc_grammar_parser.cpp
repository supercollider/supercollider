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
#line 239 "langutils/sc_parser/src/sc_grammar.y"
                                     { (yylhs.value.v_GoIndex) = cxt.graph->assign_root((yystack_[2].value.v_RegionListIndex)); }
#line 638 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 3: // go: INTERPRET $end
#line 240 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_GoIndex) = cxt.graph->assign_root(cxt.create(RegionList{}, yylhs.location)); }
#line 644 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 4: // go: file_as_func
#line 241 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_GoIndex) = cxt.graph->assign_root((yystack_[0].value.v_FileAsFunctionIndex)); }
#line 650 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 5: // go: interpreted_function
#line 242 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_GoIndex) = cxt.graph->assign_root((yystack_[0].value.v_FileAsFunctionIndex)); }
#line 656 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 6: // go: classOrExtList.list $end
#line 243 "langutils/sc_parser/src/sc_grammar.y"
                                              { (yylhs.value.v_GoIndex) = cxt.graph->assign_root((yystack_[1].value.v_ClassOrExtensionListIndex)); }
#line 662 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 7: // go: %empty
#line 244 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_GoIndex) = cxt.graph->assign_root(cxt.create(ClassOrExtensionList{}, yylhs.location));}
#line 668 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 8: // file_as_func: INTERPRET argument_declarations block.contents semicolon.opt $end
#line 253 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 674 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 9: // file_as_func: INTERPRET variable_declarations SEMICOLON block.contents semicolon.opt $end
#line 256 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[5].location), cxt.graph->prepend((yystack_[2].value.v_BlockContentsListIndex), (yystack_[4].value.v_DeclareVariableListIndex))); }
#line 680 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 10: // file_as_func: INTERPRET variable_declarations semicolon.opt $end
#line 259 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[3].location), cxt.create(BlockContentsList{}, yystack_[1].location, (yystack_[2].value.v_DeclareVariableListIndex))); }
#line 686 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 11: // interpreted_function: INTERPRET_FUNCTION argument_declarations block.contents semicolon.opt $end
#line 270 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 692 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 12: // interpreted_function: INTERPRET_FUNCTION argument_declarations semicolon.opt $end
#line 273 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[2].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, yylhs.location)); }
#line 698 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 13: // interpreted_function: INTERPRET_FUNCTION OPENPAREN argument_declarations block.contents semicolon.opt CLOSEPAREN $end
#line 276 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[4].value.v_DeclareArgumentListIndex), (yystack_[3].value.v_BlockContentsListIndex)); }
#line 704 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 14: // interpreted_function: INTERPRET_FUNCTION OPENPAREN argument_declarations semicolon.opt CLOSEPAREN $end
#line 279 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, yylhs.location)); }
#line 710 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 15: // interpreted_function: INTERPRET_FUNCTION block.contents semicolon.opt $end
#line 282 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[3].location), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 716 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 16: // interpreted_function: INTERPRET_FUNCTION semicolon.opt $end
#line 285 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[2].location), cxt.create(BlockContentsList{}, yylhs.location)); }
#line 722 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 17: // interpreted_function: INTERPRET_FUNCTION OPENPAREN variable_declarations SEMICOLON block.contents semicolon.opt CLOSEPAREN $end
#line 288 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[7].location), cxt.graph->prepend((yystack_[3].value.v_BlockContentsListIndex), (yystack_[5].value.v_DeclareVariableListIndex))); }
#line 728 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 18: // interpreted_function: INTERPRET_FUNCTION OPENPAREN variable_declarations semicolon.opt CLOSEPAREN $end
#line 291 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FileAsFunctionIndex) = cxt.create(FileAsFunction{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[5].location), cxt.create(BlockContentsList{}, yystack_[3].location, (yystack_[3].value.v_DeclareVariableListIndex))); }
#line 734 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 19: // region.item: expr
#line 300 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 740 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 20: // region.item: NONLOCALRETURN expr
#line 303 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 746 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 21: // region.item: OPENPAREN argument_declarations block.contents semicolon.opt CLOSEPAREN
#line 305 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 752 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 22: // region.item: OPENPAREN argument_declarations CLOSEPAREN
#line 308 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, yystack_[0].location)); }
#line 758 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 23: // region.item: OPENPAREN variable_declarations SEMICOLON block.contents semicolon.opt CLOSEPAREN
#line 311 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionItemIndex) = cxt.create(ImmediatelyEvaluatedBlock{}, yylhs.location, cxt.create(DeclareArgumentList{}, yystack_[5].location), cxt.graph->prepend((yystack_[2].value.v_BlockContentsListIndex), (yystack_[4].value.v_DeclareVariableListIndex))); }
#line 764 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 24: // region: INTERPRET region.item
#line 316 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionListIndex) = cxt.create(RegionList{}, yylhs.location, (yystack_[0].value.v_RegionItemIndex)); }
#line 770 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 25: // region: INTERPRET error
#line 319 "langutils/sc_parser/src/sc_grammar.y"
                {
			error_recovery::expr(cxt);
			(yylhs.value.v_RegionListIndex) = cxt.create(RegionList{}, yylhs.location, cxt.create_error(yylhs.location, yystack_[0].location));
		}
#line 779 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 26: // region: region SEMICOLON region.item
#line 325 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionListIndex) = cxt.graph->append((yystack_[2].value.v_RegionListIndex), yylhs.location, (yystack_[0].value.v_RegionItemIndex)); }
#line 785 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 27: // region: region REGION_SEPARATOR region.item
#line 328 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_RegionListIndex) = cxt.graph->append((yystack_[2].value.v_RegionListIndex), yylhs.location, (yystack_[0].value.v_RegionItemIndex)); }
#line 791 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 28: // region: region error
#line 331 "langutils/sc_parser/src/sc_grammar.y"
                {
			// A missing semicolon between lines is inserted by the lexer, see yylex.
			error_recovery::expr(cxt);
			(yylhs.value.v_RegionListIndex) = cxt.graph->append((yystack_[1].value.v_RegionListIndex), cxt.create_error(yystack_[0].location, yystack_[0].location, Index { *(yystack_[1].value.v_RegionListIndex) }));
			yyclearin;
		}
#line 802 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 29: // classOrExtList.list: classOrExtList.item
#line 348 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassOrExtensionListIndex) = cxt.create(ClassOrExtensionList{}, yylhs.location, (yystack_[0].value.v_ClassOrExtensionIndex)); }
#line 808 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 30: // classOrExtList.list: classOrExtList.list classOrExtList.item
#line 350 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassOrExtensionListIndex) = cxt.graph->append((yystack_[1].value.v_ClassOrExtensionListIndex), (yystack_[0].value.v_ClassOrExtensionIndex)); }
#line 814 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 31: // classOrExtList.item: class
#line 354 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassOrExtensionIndex) = (yystack_[0].value.v_ClassIndex); }
#line 820 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 32: // classOrExtList.item: class.extension
#line 355 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_ClassOrExtensionIndex) = (yystack_[0].value.v_ClassExtensionIndex); }
#line 826 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 33: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY members semicolon.opt method.list.opt CLOSECURLY
#line 360 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassIndex) = cxt.create(Class{ cxt.take_version(yystack_[4].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[7].location), (yystack_[6].value.v_maybe_NamedIdentifierIndex), (yystack_[5].value.v_maybe_ClassNameIdentifierIndex), (yystack_[3].value.v_BigMemberListIndex), (yystack_[1].value.v_MethodListIndex)); }
#line 832 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 34: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY method.list.opt CLOSECURLY
#line 363 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto no_members = cxt.create(BigMemberList{}, sc::src::SourceCodeRange { yystack_[2].location.end, yystack_[2].location.end });
			(yylhs.value.v_ClassIndex) = cxt.create(Class{ cxt.take_version(yystack_[2].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[5].location), (yystack_[4].value.v_maybe_NamedIdentifierIndex), (yystack_[3].value.v_maybe_ClassNameIdentifierIndex), no_members, (yystack_[1].value.v_MethodListIndex));
		}
#line 841 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 35: // class.super.opt: %empty
#line 370 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_maybe_ClassNameIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 847 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 36: // class.super.opt: COLON CLASSNAME
#line 371 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_maybe_ClassNameIdentifierIndex) = cxt.create(ClassNameIdentifier{}, yystack_[0].location); }
#line 853 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 37: // class.slot.opt: %empty
#line 375 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_maybe_NamedIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 859 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 38: // class.slot.opt: OPENSQUARE name CLOSESQUARE
#line 376 "langutils/sc_parser/src/sc_grammar.y"
                                      { (yylhs.value.v_maybe_NamedIdentifierIndex) = (yystack_[1].value.v_NamedIdentifierIndex); }
#line 865 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 39: // class.slot.opt: OPENSQUARE CLOSESQUARE
#line 377 "langutils/sc_parser/src/sc_grammar.y"
                                 { (yylhs.value.v_maybe_NamedIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 871 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 40: // class.extension: ADD CLASSNAME OPENCURLY method.list.opt CLOSECURLY
#line 382 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassExtensionIndex) = cxt.create(ClassExtension{ cxt.take_version(yystack_[2].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[3].location), (yystack_[1].value.v_MethodListIndex)); }
#line 877 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 41: // members.entry.item: accessor variable_declarations.list.item
#line 391 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberIndex) = cxt.create(DeclareMember{}, yylhs.location, (yystack_[1].value.v_RWAccessorIndex), (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 883 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 42: // members.entry.list: members.entry.item
#line 396 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberListIndex) = cxt.create(DeclareMemberList{}, yylhs.location, (yystack_[0].value.v_DeclareMemberIndex)); }
#line 889 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 43: // members.entry.list: members.entry.list COMMA members.entry.item
#line 398 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberListIndex) = cxt.graph->append((yystack_[2].value.v_DeclareMemberListIndex), (yystack_[0].value.v_DeclareMemberIndex)); }
#line 895 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 44: // members.entry: CLASSVAR members.entry.list
#line 403 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberEntryIndex) = cxt.graph->cast<DeclareClassMemberList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 901 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 45: // members.entry: VAR members.entry.list
#line 405 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberEntryIndex) = cxt.graph->cast<DeclareMemberList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 907 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 46: // members.entry: CONST members.entry.list
#line 407 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberEntryIndex) = cxt.graph->cast<DeclareConstList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 913 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 47: // members.entry: CLASSVAR error
#line 409 "langutils/sc_parser/src/sc_grammar.y"
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
#line 419 "langutils/sc_parser/src/sc_grammar.y"
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
#line 429 "langutils/sc_parser/src/sc_grammar.y"
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
#line 442 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberListIndex) = cxt.create(BigMemberList{}, yylhs.location, (yystack_[0].value.v_BigMemberEntryIndex));}
#line 961 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 51: // members: members SEMICOLON members.entry
#line 444 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberListIndex) = cxt.graph->append((yystack_[2].value.v_BigMemberListIndex), (yystack_[0].value.v_BigMemberEntryIndex)); }
#line 967 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 52: // method.name: name
#line 453 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 973 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 53: // method.name: binary_op.raw
#line 454 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 979 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 54: // method.base: method.name OPENCURLY argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 459 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(Missing{},yystack_[4].location), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 985 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 55: // method.base: method.name OPENCURLY argument_declarations.opt CLOSECURLY
#line 462 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[3].value.v_SelectorIndex), (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(Missing{},yystack_[2].location), cxt.create(BlockContentsList{}, yystack_[0].location)); }
#line 991 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 56: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt block.contents semicolon.opt CLOSECURLY
#line 465 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[7].value.v_SelectorIndex), (yystack_[5].value.v_DeclareArgumentListIndex), cxt.create(PrimitiveIdentifier{}, yystack_[4].location), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 997 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 57: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt CLOSECURLY
#line 468 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(PrimitiveIdentifier{}, yystack_[2].location), cxt.create(BlockContentsList{}, yystack_[1].location)); }
#line 1003 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 58: // method.base: method.name OPENCURLY error
#line 471 "langutils/sc_parser/src/sc_grammar.y"
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
#line 1018 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 59: // method: method.base
#line 484 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_AnyMethodIndex) = (yystack_[0].value.v_AnyMethodIndex); }
#line 1024 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 60: // method: error
#line 486 "langutils/sc_parser/src/sc_grammar.y"
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
#line 1039 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 61: // method: MULTIPLY method.base
#line 497 "langutils/sc_parser/src/sc_grammar.y"
                { 
			if (auto c = cxt.graph->as_a<MethodIndex>((yystack_[0].value.v_AnyMethodIndex))) {
				(yylhs.value.v_AnyMethodIndex) = cxt.graph->cast<ClassMethod>(*c);
			} 
			// leaves as error
			cxt.graph->location((yystack_[0].value.v_AnyMethodIndex)) = yylhs.location; 
			(yylhs.value.v_AnyMethodIndex) = (yystack_[0].value.v_AnyMethodIndex); 
		}
#line 1052 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 62: // method.list: method
#line 508 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_MethodListIndex) = cxt.create(MethodList{}, yylhs.location, (yystack_[0].value.v_AnyMethodIndex)); }
#line 1058 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 63: // method.list: method.list method
#line 509 "langutils/sc_parser/src/sc_grammar.y"
                             { (yylhs.value.v_MethodListIndex) = cxt.graph->append((yystack_[1].value.v_MethodListIndex), yylhs.location, (yystack_[0].value.v_AnyMethodIndex)); }
#line 1064 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 64: // method.list.opt: %empty
#line 513 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_MethodListIndex) = cxt.create(MethodList{}, yylhs.location); }
#line 1070 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 65: // method.list.opt: method.list
#line 514 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_MethodListIndex) = (yystack_[0].value.v_MethodListIndex); }
#line 1076 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 66: // block.open: OPENCURLY
#line 523 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_bool) = false; }
#line 1082 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 67: // block.open: BEGINCLOSEDFUNC
#line 524 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_bool) = true; }
#line 1088 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 68: // block: block.open argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 529 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockIndex) = cxt.create(BlockNode{ (yystack_[4].value.v_bool) }, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 1094 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 69: // block: block.open argument_declarations.opt CLOSECURLY
#line 531 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockIndex) = cxt.create(BlockNode{ (yystack_[2].value.v_bool) }, yylhs.location, (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, yylhs.location)); }
#line 1100 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 70: // block.opt_list: %empty
#line 535 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location); }
#line 1106 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 71: // block.opt_list: block.list
#line 536 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_BlockListIndex) = (yystack_[0].value.v_BlockListIndex); }
#line 1112 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 72: // block.list: block
#line 540 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location, (yystack_[0].value.v_BlockIndex)); }
#line 1118 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 73: // block.list: generator
#line 541 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location, (yystack_[0].value.v_GeneratorAnyIndex)); }
#line 1124 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 74: // block.list: block.list block
#line 542 "langutils/sc_parser/src/sc_grammar.y"
                           { (yylhs.value.v_BlockListIndex) = cxt.graph->append((yystack_[1].value.v_BlockListIndex), yylhs.location, (yystack_[0].value.v_BlockIndex)); }
#line 1130 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 75: // block.list: block.list generator
#line 543 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_BlockListIndex) = cxt.graph->append((yystack_[1].value.v_BlockListIndex), yylhs.location, (yystack_[0].value.v_GeneratorAnyIndex)); }
#line 1136 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 76: // block.contents: block.contents.item
#line 547 "langutils/sc_parser/src/sc_grammar.y"
                              { (yylhs.value.v_BlockContentsListIndex) = cxt.create(BlockContentsList{}, yylhs.location, (yystack_[0].value.v_BlockItemIndex)); }
#line 1142 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 77: // block.contents: block.contents SEMICOLON block.contents.item
#line 548 "langutils/sc_parser/src/sc_grammar.y"
                                                       { (yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[2].value.v_BlockContentsListIndex), yylhs.location, (yystack_[0].value.v_BlockItemIndex)); }
#line 1148 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 78: // block.contents: block.contents NONLOCALRETURN expr
#line 550 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[2].value.v_BlockContentsListIndex), yylhs.location, non_local); 
		}
#line 1157 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 79: // block.contents: block.contents SEMICOLON NONLOCALRETURN expr
#line 555 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[3].value.v_BlockContentsListIndex), yylhs.location, non_local); 
		}
#line 1166 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 80: // block.contents: NONLOCALRETURN expr
#line 560 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.create(BlockContentsList{}, yylhs.location, non_local);
		}
#line 1175 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 81: // block.contents.item: expr
#line 567 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_BlockItemIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1181 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 82: // block.contents.item: variable_declarations
#line 568 "langutils/sc_parser/src/sc_grammar.y"
                                { (yylhs.value.v_BlockItemIndex) = (yystack_[0].value.v_DeclareVariableListIndex); }
#line 1187 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 83: // adverb: DOT name
#line 577 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_AdverbIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 1193 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 84: // adverb: DOT integer
#line 578 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_AdverbIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 1199 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 85: // adverb: DOT OPENPAREN expr_or_seq CLOSEPAREN
#line 579 "langutils/sc_parser/src/sc_grammar.y"
                                               { (yylhs.value.v_AdverbIndex) = cxt.create(AdverbExprNode{}, yylhs.location, (yystack_[1].value.v_AnyExprIndex));  }
#line 1205 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 86: // binary_op.raw: BINOP
#line 583 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1211 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 87: // binary_op.raw: READWRITEVAR
#line 584 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1217 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 88: // binary_op.raw: LESSTHAN
#line 585 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1223 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 89: // binary_op.raw: GREATERTHAN
#line 586 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1229 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 90: // binary_op.raw: MINUS
#line 587 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1235 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 91: // binary_op.raw: MULTIPLY
#line 588 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1241 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 92: // binary_op.raw: ADD
#line 589 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1247 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 93: // binary_op.raw: PIPE
#line 590 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1253 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 94: // binary_op.no_adverb: binary_op.raw
#line 594 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 1259 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 95: // binary_op.no_adverb: KEYBINOP
#line 595 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ true }, yylhs.location); }
#line 1265 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 96: // binary_op: binary_op.no_adverb adverb
#line 600 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorMaybeAdverbIndex) = cxt.create(SelectorWAdverb{}, yylhs.location, (yystack_[1].value.v_SelectorIndex), (yystack_[0].value.v_AdverbIndex)); }
#line 1271 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 97: // binary_op: binary_op.no_adverb
#line 602 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorMaybeAdverbIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 1277 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 98: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN arguments CLOSEPAREN block.opt_list
#line 607 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[2].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1283 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 99: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN CLOSEPAREN block.list
#line 610 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[4].value.v_SelectorIndex), cxt.create(ArgumentList{}, yystack_[2].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1289 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 100: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN block.list
#line 613 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[2].value.v_SelectorIndex), cxt.create(ArgumentList{}, yystack_[0].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1295 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 101: // msgsend: name OPENPAREN arguments CLOSEPAREN block.opt_list
#line 617 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[2].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1301 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 102: // msgsend: name OPENPAREN CLOSEPAREN block.list
#line 620 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), cxt.create(ArgumentList{}, yystack_[2].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1307 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 103: // msgsend: name block.list
#line 623 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[1].value.v_NamedIdentifierIndex), cxt.create(ArgumentList{}, yystack_[0].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1313 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 104: // msgsend: expr DOT name arguments.maybe_paren block.opt_list
#line 626 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PostfixMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1319 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 105: // msgsend: expr DOT arguments.paren block.opt_list
#line 629 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(ValueMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1325 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 106: // msgsend: expr DOT OPENPAREN CLOSEPAREN block.opt_list
#line 632 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(ValueMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[2].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1331 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 107: // msgsend: CLASSNAME OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 642 "langutils/sc_parser/src/sc_grammar.y"
                {  (yylhs.value.v_AnyExprIndex) = cxt.create(CollectionNode{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[4].location), (yystack_[2].value.v_ArrayIndex)); }
#line 1337 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 108: // msgsend: CLASSNAME block.list
#line 645 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[1].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, yystack_[1].location), (yystack_[0].value.v_BlockListIndex));
		}
#line 1346 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 109: // msgsend: CLASSNAME arguments.paren block.opt_list
#line 650 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[2].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex));
		}
#line 1355 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 110: // msgsend: CLASSNAME OPENPAREN CLOSEPAREN block.opt_list
#line 656 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[3].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, yystack_[3].location), (yystack_[0].value.v_BlockListIndex));
		}
#line 1364 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 111: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 671 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[3].value.v_ArgumentListIndex), (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r);
		}
#line 1374 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 112: // expr.dotrange: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 678 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[3].location),  (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1384 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 113: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 685 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[2].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[1].location));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1394 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 114: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 694 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[5].value.v_ArgumentListIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[8].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1404 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 115: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 701 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[4].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[3].location));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1414 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 116: // expr.dotrange.assign: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 708 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[5].location),  (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1424 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 117: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 718 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[3].value.v_ArgumentListIndex), (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1434 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 118: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 724 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[2].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[0].location));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), r);	
		}
#line 1444 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 119: // expr.base.range: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 730 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[3].location),  (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), r);	
		}
#line 1454 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 120: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 739 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[5].value.v_ArgumentListIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1464 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 121: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 745 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[4].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[3].location));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1474 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 122: // expr.base.range.assign: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 751 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[5].location),  (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1484 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 123: // range: OPENPAREN expr_or_seq DOTDOT CLOSEPAREN
#line 760 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1493 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 124: // range: OPENPAREN expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 765 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[4].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1502 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 125: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 770 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[6].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1511 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 126: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 775 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[4].location, yystack_[2].location), (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[5].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1520 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 127: // range: OPENPAREN DOTDOT expr_or_seq CLOSEPAREN
#line 780 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location);
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1529 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 128: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 788 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[5].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1538 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 129: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 793 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[7].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1547 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 130: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 799 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[4].location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[6].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1556 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 131: // range.generator: OPENPAREN COLON DOTDOT expr_or_seq CLOSEPAREN
#line 804 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location);
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1565 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 132: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT CLOSEPAREN
#line 809 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1574 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 133: // expr.base: literal
#line 822 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyPossiblyLiteralIndex); }
#line 1580 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 134: // expr.base: generator
#line 825 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_GeneratorAnyIndex); }
#line 1586 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 135: // expr.base: name
#line 828 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 1592 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 136: // expr.base: CURRYARG
#line 831 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyExprIndex) = cxt.create(CurryNode{}, yystack_[0].location); }
#line 1598 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 137: // expr.base: msgsend
#line 834 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1604 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 138: // expr.base: OPENPAREN expr_or_seq CLOSEPAREN
#line 837 "langutils/sc_parser/src/sc_grammar.y"
                { 
			// A nasty hack because of nil assignment.
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AnyExprIndex) = (yystack_[1].value.v_AnyExprIndex);
		}
#line 1614 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 139: // expr.base: OPENPAREN error
#line 844 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto close = error_recovery::bracket(cxt, yystack_[1].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create_error(range(yystack_[1].location, close), yystack_[0].location);
			yyclearin; // The lookahead was skipped.
			yyerrok; // Recovered at the matching bracket, so report the next error.
		}
#line 1625 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 140: // expr.base: TILDE name
#line 852 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(EnvIdentifierNode{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1631 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 141: // expr.base: expr.base OPENSQUARE arguments CLOSESQUARE
#line 855 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex) ); }
#line 1637 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 142: // expr.base: expr.base OPENSQUARE CLOSESQUARE
#line 858 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[1].location)); }
#line 1643 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 143: // expr.base: expr.base.range
#line 861 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AtMessageIndex); }
#line 1649 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 144: // expr.base: range
#line 864 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeIndex); }
#line 1655 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 145: // expr.base: range.generator
#line 867 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeGeneratorIndex); }
#line 1661 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 146: // expr: expr.base
#line 872 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1667 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 147: // expr: expr.base.range.assign
#line 873 "langutils/sc_parser/src/sc_grammar.y"
                                 { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeAssignIndex); }
#line 1673 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 148: // expr: expr.dotrange
#line 874 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AtMessageIndex); }
#line 1679 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 149: // expr: expr.dotrange.assign
#line 875 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeAssignIndex); }
#line 1685 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 150: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE
#line 878 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex) ); }
#line 1691 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 151: // expr: expr DOT OPENSQUARE CLOSESQUARE
#line 881 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[1].location) ); }
#line 1697 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 152: // expr: expr binary_op expr
#line 884 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(BinaryMessage{}, yylhs.location, (yystack_[1].value.v_SelectorMaybeAdverbIndex), (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1703 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 153: // expr: expr DOT name EQUALSSIGN expr
#line 887 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(SetterNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1709 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 154: // expr: CLASSNAME
#line 889 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_AnyExprIndex) = cxt.create(ClassNameIdentifier{}, yylhs.location); }
#line 1715 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 155: // expr: BACKTICK expr
#line 890 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyExprIndex) = cxt.create(ReferenceNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1721 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 156: // expr: name EQUALSSIGN expr
#line 892 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentNode{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1727 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 157: // expr: TILDE name EQUALSSIGN expr
#line 895 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentNode{AssignmentNode::Target::Environment}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1733 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 158: // expr: name OPENPAREN arguments CLOSEPAREN EQUALSSIGN expr
#line 898 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(SetterNode{}, yylhs.location, (yystack_[3].value.v_ArgumentListIndex), (yystack_[5].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1739 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 159: // expr: name.pack EQUALSSIGN expr
#line 901 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[2].value.v_NamePackIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1745 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 160: // expr: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 904 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[3].value.v_NamePackIndex), (yystack_[1].value.v_AnyExprIndex)); }
#line 1751 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 161: // expr: expr.base OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 910 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_ArgumentListIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1757 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 162: // expr: expr.base OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 913 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[3].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 1763 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 163: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 916 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), (yystack_[3].value.v_ArgumentListIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1769 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 164: // expr: expr DOT OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 919 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[3].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 1775 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 165: // expr_or_seq.base: expr
#line 926 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1781 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 166: // expr_or_seq.base: expr_or_seq.base SEMICOLON expr
#line 927 "langutils/sc_parser/src/sc_grammar.y"
                                          { (yylhs.value.v_AnyExprIndex) = cxt.create(ExprSeq{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1787 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 167: // expr_or_seq: expr_or_seq.base semicolon.opt
#line 931 "langutils/sc_parser/src/sc_grammar.y"
                                       { (yylhs.value.v_AnyExprIndex) = (yystack_[1].value.v_AnyExprIndex); }
#line 1793 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 168: // generator: OPENCURLY COLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 940 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorAnyIndex) = cxt.create(GeneratorNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_GeneratorQualListIndex)); }
#line 1799 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 169: // generator: OPENCURLY SEMICOLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 943 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorAnyIndex) = cxt.create(GeneratorFakeNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_GeneratorQualListIndex)); }
#line 1805 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 170: // generator.qual.list: generator.qual
#line 948 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualListIndex) = cxt.create(GeneratorQualList{}, yylhs.location, (yystack_[0].value.v_GeneratorQualAnyIndex)); }
#line 1811 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 171: // generator.qual.list: generator.qual.list COMMA generator.qual
#line 950 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualListIndex) = cxt.graph->append((yystack_[2].value.v_GeneratorQualListIndex), yylhs.location, (yystack_[0].value.v_GeneratorQualAnyIndex)); }
#line 1817 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 172: // generator.qual: name LEFTARROW expr_or_seq
#line 955 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualAssign{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), cxt.create(Missing{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 1823 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 173: // generator.qual: name name LEFTARROW expr_or_seq
#line 957 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualAssign{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1829 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 174: // generator.qual: VAR name EQUALSSIGN expr_or_seq
#line 959 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualDeclare{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1835 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 175: // generator.qual: VAR name OPENPAREN expr_or_seq CLOSEPAREN
#line 961 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualDeclare{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex)); }
#line 1841 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 176: // generator.qual: expr_or_seq
#line 963 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualIf{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1847 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 177: // generator.qual: COLON COLON expr_or_seq
#line 965 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualSideEffect{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1853 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 178: // generator.qual: COLON WHILE expr_or_seq
#line 967 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualGuard{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1859 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 179: // argument_declarations.list: name
#line 980 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1865 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 180: // argument_declarations.list: name EQUALSSIGN expr
#line 982 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed((yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{is_bracketed}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex))); 
		}
#line 1874 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 181: // argument_declarations.list: name OPENPAREN expr_or_seq CLOSEPAREN
#line 987 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1880 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 182: // argument_declarations.list: argument_declarations.list COMMA name
#line 990 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[2].value.v_DeclareArgumentListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1886 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 183: // argument_declarations.list: argument_declarations.list COMMA name EQUALSSIGN expr
#line 993 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed((yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[4].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{is_bracketed}, range(yystack_[2].location, yystack_[0].location), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)));
		}
#line 1895 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 184: // argument_declarations.list: argument_declarations.list COMMA name OPENPAREN expr_or_seq CLOSEPAREN
#line 998 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[3].location, yystack_[0].location), (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1901 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 185: // argument_declarations.pipelist: name literal
#line 1004 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, (yystack_[1].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1907 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 186: // argument_declarations.pipelist: name
#line 1006 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1913 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 187: // argument_declarations.pipelist: name EQUALSSIGN literal
#line 1008 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1919 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 188: // argument_declarations.pipelist: name OPENPAREN expr_or_seq CLOSEPAREN
#line 1010 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1925 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 189: // argument_declarations.pipelist: name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 1012 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1931 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 190: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name literal
#line 1014 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[3].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[1].location, yystack_[0].location), (yystack_[1].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1937 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 191: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name
#line 1016 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[2].value.v_DeclareArgumentListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1943 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 192: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN literal
#line 1018 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[4].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[2].location, yystack_[0].location), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1949 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 193: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name OPENPAREN expr_or_seq CLOSEPAREN
#line 1020 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[3].location, yystack_[0].location), (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1955 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 194: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 1022 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[6].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[4].location, yystack_[0].location), (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1961 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 195: // argument_declarations: ARG SEMICOLON
#line 1026 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 1967 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 196: // argument_declarations: ARG argument_declarations.list comma.opt SEMICOLON
#line 1027 "langutils/sc_parser/src/sc_grammar.y"
                                                             { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[2].value.v_DeclareArgumentListIndex); }
#line 1973 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 197: // argument_declarations: ARG argument_declarations.list ELLIPSIS name SEMICOLON
#line 1029 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[3].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 1979 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 198: // argument_declarations: ARG argument_declarations.list ELLIPSIS name COMMA name SEMICOLON
#line 1031 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 1985 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 199: // argument_declarations: ARG ELLIPSIS name SEMICOLON
#line 1033 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(arg_list, yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); 
		}
#line 1994 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 200: // argument_declarations: ARG ELLIPSIS name COMMA name SEMICOLON
#line 1038 "langutils/sc_parser/src/sc_grammar.y"
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

  case 201: // argument_declarations: PIPE PIPE
#line 1047 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 2013 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 202: // argument_declarations: PIPE argument_declarations.pipelist comma.opt PIPE
#line 1049 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[2].value.v_DeclareArgumentListIndex); }
#line 2019 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 203: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name PIPE
#line 1051 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[3].value.v_DeclareArgumentListIndex), yystack_[3].location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 2025 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 204: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name COMMA name PIPE
#line 1053 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 2031 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 205: // argument_declarations: PIPE ELLIPSIS name PIPE
#line 1055 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(arg_list, yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); 
		}
#line 2040 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 206: // argument_declarations: PIPE ELLIPSIS name COMMA name PIPE
#line 1060 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(
				arg_list, yylhs.location,
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))
			); 
		}
#line 2053 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 207: // argument_declarations.opt: %empty
#line 1071 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 2059 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 208: // argument_declarations.opt: argument_declarations
#line 1072 "langutils/sc_parser/src/sc_grammar.y"
                                { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[0].value.v_DeclareArgumentListIndex); }
#line 2065 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 209: // arguments.entries: KEYBINOP expr_or_seq
#line 1077 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_ArgumentEntryIndex) = cxt.create(KwArgNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2071 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 210: // arguments.entries: MULTIPLY expr_or_seq
#line 1078 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_ArgumentEntryIndex) = cxt.create(VariadicArgNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2077 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 211: // arguments.entries: expr_or_seq
#line 1079 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_ArgumentEntryIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 2083 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 212: // arguments.no_trailing: arguments.entries
#line 1083 "langutils/sc_parser/src/sc_grammar.y"
                            {  (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location, (yystack_[0].value.v_ArgumentEntryIndex)); }
#line 2089 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 213: // arguments.no_trailing: arguments.no_trailing COMMA arguments.entries
#line 1084 "langutils/sc_parser/src/sc_grammar.y"
                                                        { (yylhs.value.v_ArgumentListIndex) = cxt.graph->append((yystack_[2].value.v_ArgumentListIndex), yylhs.location, (yystack_[0].value.v_ArgumentEntryIndex)); }
#line 2095 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 214: // arguments: arguments.no_trailing comma.opt
#line 1087 "langutils/sc_parser/src/sc_grammar.y"
                                            { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2101 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 215: // arguments.paren: OPENPAREN arguments CLOSEPAREN
#line 1089 "langutils/sc_parser/src/sc_grammar.y"
                                                 { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2107 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 216: // arguments.maybe_paren: %empty
#line 1092 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location); }
#line 2113 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 217: // arguments.maybe_paren: OPENPAREN CLOSEPAREN
#line 1094 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location); }
#line 2119 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 218: // arguments.maybe_paren: OPENPAREN arguments CLOSEPAREN
#line 1096 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2125 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 219: // variable_declarations.list.item: name
#line 1105 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 2131 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 220: // variable_declarations.list.item: name EQUALSSIGN expr
#line 1107 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2137 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 221: // variable_declarations.list.item: name OPENPAREN expr_or_seq CLOSEPAREN
#line 1109 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Bracketed, as in the old compiler, so it isn't a literal, see ConstEval::is_legacy_literal.
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex));
		}
#line 2147 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 222: // variable_declarations.list.item: name.pack EQUALSSIGN expr
#line 1115 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[2].value.v_NamePackIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2153 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 223: // variable_declarations.list.item: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 1117 "langutils/sc_parser/src/sc_grammar.y"
                {
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[3].value.v_NamePackIndex), (yystack_[1].value.v_AnyExprIndex));
		}
#line 2162 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 224: // variable_declarations.list: variable_declarations.list.item
#line 1125 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareVariableListIndex) = cxt.create(DeclareVariableList{}, yylhs.location, (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 2168 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 225: // variable_declarations.list: variable_declarations.list COMMA variable_declarations.list.item
#line 1127 "langutils/sc_parser/src/sc_grammar.y"
                {(yylhs.value.v_DeclareVariableListIndex) = cxt.graph->append((yystack_[2].value.v_DeclareVariableListIndex), (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 2174 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 226: // variable_declarations: VAR variable_declarations.list
#line 1130 "langutils/sc_parser/src/sc_grammar.y"
                                                       { (yylhs.value.v_DeclareVariableListIndex) = (yystack_[0].value.v_DeclareVariableListIndex); }
#line 2180 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 227: // literal.terminal: symbol
#line 1138 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_AnySymbolIndex); }
#line 2186 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 228: // literal.terminal: string
#line 1139 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_StringLitIndex); }
#line 2192 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 229: // literal.terminal: integer
#line 1140 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 2198 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 230: // literal.terminal: float
#line 1141 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_FloatProducingIndex); }
#line 2204 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 231: // literal.terminal: boolean
#line 1142 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_BooleanLitIndex); }
#line 2210 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 232: // literal.terminal: nil
#line 1143 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_NilLitIndex); }
#line 2216 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 233: // literal.terminal: ascii
#line 1144 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_ASCIIIndex); }
#line 2222 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 234: // literal.terminal: block
#line 1145 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_BlockIndex); }
#line 2228 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 235: // literal.array.contents: %empty
#line 1150 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location); }
#line 2234 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 236: // literal.array.contents: expr_or_seq
#line 1152 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2240 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 237: // literal.array.contents: expr_or_seq COLON expr_or_seq
#line 1154 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2246 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 238: // literal.array.contents: KEYBINOP expr_or_seq
#line 1156 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2252 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 239: // literal.array.contents: literal.array.contents COMMA expr_or_seq
#line 1158 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[2].value.v_ArrayIndex), yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2258 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 240: // literal.array.contents: literal.array.contents COMMA expr_or_seq COLON expr_or_seq
#line 1160 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[4].value.v_ArrayIndex), yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2264 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 241: // literal.array.contents: literal.array.contents COMMA KEYBINOP expr_or_seq
#line 1162 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[3].value.v_ArrayIndex), yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2270 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 242: // literal.dictionary.entry: expr COLON expr_or_seq
#line 1170 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryEntryIndex) = cxt.create(DictionaryEntryNode{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2276 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 243: // literal.dictionary.entry: KEYBINOP expr_or_seq
#line 1172 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryEntryIndex) = cxt.create(DictionaryEntryNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2282 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 244: // literal.dictionary.entries: %empty
#line 1177 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.create(DictionaryNode{}, yylhs.location); }
#line 2288 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 245: // literal.dictionary.entries: literal.dictionary.entry
#line 1179 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.create(DictionaryNode{}, yylhs.location, (yystack_[0].value.v_DictionaryEntryIndex)); }
#line 2294 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 246: // literal.dictionary.entries: literal.dictionary.entries COMMA literal.dictionary.entry
#line 1181 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.graph->append((yystack_[2].value.v_DictionaryIndex), yylhs.location,  (yystack_[0].value.v_DictionaryEntryIndex)); }
#line 2300 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 247: // literal.dictionary: OPENPAREN literal.dictionary.entries comma.opt CLOSEPAREN
#line 1186 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->location(*(yystack_[2].value.v_DictionaryIndex)) = yylhs.location; (yylhs.value.v_DictionaryIndex) = (yystack_[2].value.v_DictionaryIndex); }
#line 2306 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 248: // literal.array: OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1191 "langutils/sc_parser/src/sc_grammar.y"
                {  cxt.graph->location(*(yystack_[2].value.v_ArrayIndex)) = yylhs.location; (yylhs.value.v_ArrayIndex) = (yystack_[2].value.v_ArrayIndex); }
#line 2312 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 249: // literal.array: HASH OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1193 "langutils/sc_parser/src/sc_grammar.y"
                {  
			cxt.graph->payload((yystack_[2].value.v_ArrayIndex)).is_immutable = true;
			cxt.graph->location(*(yystack_[2].value.v_ArrayIndex)) = yylhs.location;
			(yylhs.value.v_ArrayIndex) = (yystack_[2].value.v_ArrayIndex);
		}
#line 2322 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 250: // literal: literal.terminal
#line 1201 "langutils/sc_parser/src/sc_grammar.y"
                           { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_AnyPossiblyLiteralIndex); }
#line 2328 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 251: // literal: literal.array
#line 1202 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_ArrayIndex); }
#line 2334 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 252: // literal: literal.dictionary
#line 1203 "langutils/sc_parser/src/sc_grammar.y"
                             { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_DictionaryIndex); }
#line 2340 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 253: // name: NAME
#line 1211 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_NamedIdentifierIndex) = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 2346 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 254: // name: WHILE
#line 1212 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamedIdentifierIndex) = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 2352 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 255: // name.list: HASH name
#line 1218 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.create(NameList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2358 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 256: // name.list: HASH TILDE name
#line 1220 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.create(NameList{}, yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2364 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 257: // name.list: name.list COMMA name
#line 1222 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.graph->append((yystack_[2].value.v_NameListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2370 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 258: // name.list: name.list COMMA TILDE name
#line 1224 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.graph->append((yystack_[3].value.v_NameListIndex), yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2376 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 259: // name.pack: name.list
#line 1228 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[0].value.v_NameListIndex), cxt.create(Missing{}, yystack_[0].location)); }
#line 2382 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 260: // name.pack: name.list ELLIPSIS name
#line 1230 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[2].value.v_NameListIndex), (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2388 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 261: // name.pack: name.list ELLIPSIS TILDE name
#line 1232 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[3].value.v_NameListIndex), cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2394 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 262: // ascii: ASCII
#line 1242 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_ASCIIIndex) = cxt.create(ASCIINode{}, yylhs.location); }
#line 2400 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 263: // nil: NIL
#line 1244 "langutils/sc_parser/src/sc_grammar.y"
          { (yylhs.value.v_NilLitIndex) = cxt.create(NilNode{}, yylhs.location); }
#line 2406 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 264: // boolean: TRUE
#line 1247 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_BooleanLitIndex) = cxt.create(BooleanNode{true}, yylhs.location); }
#line 2412 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 265: // boolean: FALSE
#line 1248 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BooleanLitIndex) = cxt.create(BooleanNode{false}, yylhs.location); }
#line 2418 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 266: // symbol: SYMBOL_QUOTE
#line 1252 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_AnySymbolIndex) = cxt.create(SymbolQuote{}, yylhs.location); }
#line 2424 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 267: // symbol: SYMBOL_SLASH
#line 1253 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_AnySymbolIndex) = cxt.create(SymbolSlash{}, yylhs.location); }
#line 2430 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 268: // string: STRINGLINE
#line 1257 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_StringLitIndex) = cxt.create(StringLineList{}, yylhs.location, cxt.create(StringLineNode{}, yylhs.location)); }
#line 2436 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 269: // string: string STRINGLINE
#line 1258 "langutils/sc_parser/src/sc_grammar.y"
                            { (yylhs.value.v_StringLitIndex) = cxt.graph->append((yystack_[1].value.v_StringLitIndex), cxt.create(StringLineNode{}, yystack_[0].location)); }
#line 2442 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 270: // integer: INTEGER
#line 1262 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{}, yylhs.location); }
#line 2448 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 271: // integer: INTEGER_RADIX
#line 1263 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{IntNode::Kind::Radix}, yylhs.location); }
#line 2454 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 272: // integer: HEXADECIMAL
#line 1264 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{IntNode::Kind::Hexadecimal}, yylhs.location); }
#line 2460 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 273: // integer: MINUS integer
#line 1266 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Reaches into the previous integer and changes its sign.
			cxt.graph->payload((yystack_[0].value.v_IntLitIndex)).sign = IntNode::Sign::Negative;
			(yylhs.value.v_IntLitIndex) = (yystack_[0].value.v_IntLitIndex);
		}
#line 2470 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 274: // float.raw_unsigned: FLOAT
#line 1274 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{}, yylhs.location); }
#line 2476 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 275: // float.raw_unsigned: FLOAT_RADIX
#line 1275 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Radix}, yylhs.location); }
#line 2482 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 276: // float.raw_unsigned: FLOAT_EXPONENT
#line 1276 "langutils/sc_parser/src/sc_grammar.y"
                         { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Exponent}, yylhs.location); }
#line 2488 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 277: // float.raw_unsigned: FLOAT_INF
#line 1277 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Inf}, yylhs.location); }
#line 2494 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 278: // float.raw: float.raw_unsigned
#line 1282 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FloatLitIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2500 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 279: // float.raw: MINUS float.raw_unsigned
#line 1284 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload((yystack_[0].value.v_FloatLitIndex)).sign = FloatNode::Sign::Negative; (yylhs.value.v_FloatLitIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2506 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 280: // accidental.unsigned: ACCIDENTAL_STEPS
#line 1289 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = cxt.create(AccidentalNode{AccidentalNode::Kind::Steps}, yylhs.location); }
#line 2512 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 281: // accidental.unsigned: ACCIDENTAL_CENTS
#line 1291 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = cxt.create(AccidentalNode{AccidentalNode::Kind::Cents}, yylhs.location); }
#line 2518 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 282: // accidental: accidental.unsigned
#line 1296 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2524 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 283: // accidental: MINUS accidental.unsigned
#line 1298 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload((yystack_[0].value.v_AccidentalIndex)).sign = AccidentalNode::Sign::Negative; (yylhs.value.v_AccidentalIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2530 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 284: // float: float.raw
#line 1302 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_FloatProducingIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2536 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 285: // float: accidental
#line 1303 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_FloatProducingIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2542 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 286: // float: float.raw PI
#line 1304 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, (yystack_[1].value.v_FloatLitIndex)); }
#line 2548 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 287: // float: integer PI
#line 1305 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, (yystack_[1].value.v_IntLitIndex)); }
#line 2554 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 288: // float: PI
#line 1306 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, cxt.create(Missing{}, yylhs.location)); }
#line 2560 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 289: // float: MINUS PI
#line 1307 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{PiNode::Sign::Negative}, yylhs.location, cxt.create(Missing{}, yylhs.location)); }
#line 2566 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 290: // accessor: %empty
#line 1311 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::Private}, yylhs.location); }
#line 2572 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 291: // accessor: LESSTHAN
#line 1312 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyReadable}, yylhs.location); }
#line 2578 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 292: // accessor: GREATERTHAN
#line 1313 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyWriteable}, yylhs.location); }
#line 2584 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 293: // accessor: READWRITEVAR
#line 1315 "langutils/sc_parser/src/sc_grammar.y"
                { 
			(yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{
				combine<ReadWriteAccessor::ExternallyReadable, ReadWriteAccessor::ExternallyWriteable>()
			}, yylhs.location); 
		}
#line 2594 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;


#line 2598 "langutils/sc_parser/src/sc_grammar_parser.cpp"

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




  const short parser::yypact_ninf_ = -229;

  const short parser::yytable_ninf_ = -295;

  const short
  parser::yypact_[] =
  {
     301,    24,   722,   947,     6,    50,  -229,  -229,   322,    21,
    -229,  -229,  -229,    39,   -10,  -229,  -229,    31,  1825,   773,
    2376,   165,   130,  -229,  -229,  -229,  -229,  -229,  -229,  -229,
    -229,  -229,  -229,  -229,  -229,  -229,  -229,   123,  -229,   120,
     253,  -229,  -229,  -229,  -229,  -229,  -229,   461,    85,  2376,
    -229,    43,  -229,  -229,  -229,  -229,  -229,  -229,  -229,  -229,
      54,   609,  -229,  1623,    42,  -229,  -229,  -229,  -229,    60,
     164,     5,  -229,  -229,  -229,  -229,   119,   127,  -229,   133,
    -229,  -229,  -229,   773,  -229,  2376,    46,  -229,   609,  1160,
    -229,    66,   180,  -229,  -229,  1876,  1876,   199,  -229,  -229,
    -229,   207,   172,   243,  2376,  2376,   832,  2376,   609,   245,
     217,   247,  -229,  2376,  1926,  -229,  2376,   461,  -229,  -229,
    -229,  -229,    85,  -229,  -229,   265,   628,   267,  1211,   286,
    -229,   300,   609,  1825,   130,  -229,   271,  1825,  1261,  -229,
      61,  -229,    61,   107,  -229,   326,     8,    27,  -229,   130,
     220,    29,  -229,   298,  -229,  -229,  -229,   130,  -229,   279,
    2422,  -229,  -229,  1315,  1003,  -229,  -229,  -229,   238,   282,
    2376,    46,  1623,   339,  1365,  2376,    61,   205,   273,  2376,
    2376,  -229,  -229,  -229,  1160,   333,   609,  1674,  2376,   349,
      46,   350,  -229,    83,  -229,  -229,  -229,  -229,  -229,   565,
     340,   341,  -229,  2376,  -229,  2376,  1976,   348,   347,  2376,
     112,  -229,   148,  2376,  -229,  2376,  2026,  -229,    46,  1623,
    2076,   352,   247,  -229,  2376,   247,    61,  2376,  2376,  -229,
    -229,   351,   353,  -229,  -229,  -229,    61,   120,  2376,  2376,
    2376,  2376,   139,   130,   130,   354,  2376,  2376,    11,  -229,
     130,    38,  -229,  2076,   359,  2500,  -229,  -229,    46,   309,
    2376,   187,  1054,  1416,    61,    40,   142,  -229,   306,   368,
      46,  -229,    61,   360,   609,   130,  -229,   130,  -229,   361,
     609,    46,   362,  1623,   363,  2376,  -229,   609,  -229,   373,
    -229,  -229,   473,   370,  -229,  -229,   600,   372,  -229,  -229,
     195,   210,   223,  -229,   365,   375,  1724,  1724,   609,  -229,
    2376,   329,  -229,  -229,   374,  2376,  2126,  1467,    61,  -229,
     343,  -229,   376,   381,    46,  2376,   628,  -229,  -229,   384,
     609,   386,  -229,  -229,  -229,  1774,  -229,  -229,  -229,   385,
     609,   387,   609,  -229,   130,    55,   287,  -229,   388,   609,
     130,  -229,    13,  -229,  2461,   391,  2076,  -229,   377,  2376,
     394,   345,  2176,   355,  2376,   249,    61,  -229,  1518,  2376,
      61,  2376,  -229,  -229,  -229,   404,    61,    20,  -229,  -229,
    -229,   398,   408,    46,   409,   609,  -229,  -229,   891,  -229,
    -229,  -229,  -229,  -229,  -229,  -229,   402,   120,  -229,   402,
    -229,   402,   171,    83,  -229,   130,   277,  -229,   403,  -229,
     225,   403,  -229,  2376,  -229,   378,  -229,   407,    61,   411,
    2226,  -229,  -229,   412,  -229,  -229,  -229,  -229,  -229,   413,
    2376,  2376,  -229,   130,  -229,   364,   130,  -229,  2076,  2539,
    -229,  -229,   417,  -229,   609,   367,  2376,   383,   420,  2376,
     421,   392,  2276,  -229,  -229,   422,   609,  -229,   425,  -229,
    2376,  -229,   438,  -229,   432,  -229,  -229,  1108,   274,  -229,
    -229,   441,    67,  2376,  2376,  1724,   443,  2376,   405,   444,
    -229,  2326,  -229,    61,    61,  -229,   447,  -229,  -229,   450,
     609,   442,  -229,   410,   451,  2076,  -229,  -229,  2376,   609,
    2376,   416,   609,   419,  2376,   426,   454,  -229,  -229,   609,
    -229,   462,  -229,   453,    46,  -229,  -229,  2376,  2376,  -229,
    -229,  -229,  -229,  -229,  2376,  -229,  -229,   458,  -229,  -229,
    -229,  -229,  -229,  -229,   463,   609,   609,  2376,  2376,   609,
    2376,   436,  -229,  1572,   465,   466,  -229,  -229,  -229,  -229,
     609,   609,   609,  2376,  -229,    46,  -229,  -229,   609,   485,
    -229
  };

  const short
  parser::yydefact_[] =
  {
       7,    37,     0,   294,     0,     0,     4,     5,     0,     0,
      29,    31,    32,     0,    35,     3,    25,    66,   235,     0,
       0,     0,     0,   253,   270,   271,   272,   274,   275,   276,
     277,   280,   281,   266,   267,   268,   262,   154,   136,     0,
       0,   263,   264,   265,   288,    67,   254,     0,     0,     0,
      24,   207,   234,   137,   148,   149,   143,   147,   144,   145,
     146,    19,   134,     0,   294,   250,   252,   251,   133,   135,
     259,     0,   233,   232,   231,   227,   228,   229,   278,   284,
     282,   285,   230,     0,   295,     0,   294,    76,    81,   294,
      82,     0,     0,     1,    28,     0,   295,     0,     6,    30,
      39,     0,     0,     0,     0,     0,     0,     0,   165,   294,
     236,   296,   139,     0,     0,    86,    95,    90,    88,    89,
      91,    92,    93,    87,    94,     0,   165,     0,     0,     0,
     245,   296,    20,   235,     0,   255,   140,   235,     0,    72,
     108,    73,    70,     0,   224,   226,   219,     0,   195,     0,
     296,   179,   289,     0,   273,   279,   283,     0,   201,   296,
     186,   155,   208,     0,     0,    95,    90,    93,     0,    97,
       0,   294,   295,     0,     0,     0,   103,     0,     0,     0,
       0,   269,   287,   286,   294,   294,    80,   295,     0,     0,
     294,     0,    16,     0,    27,    26,     2,    38,    36,     0,
       0,     0,   238,   295,   167,     0,   297,     0,     0,     0,
       0,   243,     0,     0,   138,     0,     0,    22,   294,     0,
     297,     0,   296,   256,     0,   296,    70,     0,     0,   211,
     212,   296,     0,    74,    75,   109,    71,     0,     0,     0,
       0,     0,     0,   297,     0,     0,     0,     0,     0,   297,
       0,     0,    66,   244,     0,     0,   185,    69,   294,   142,
       0,     0,     0,     0,    70,   216,     0,    96,   152,     0,
     294,    10,     0,     0,   156,     0,   257,     0,   260,     0,
     159,   294,     0,   295,     0,     0,    77,    78,    15,     0,
      12,    60,    91,     0,    59,    62,     0,     0,    53,    52,
       0,     0,     0,    50,   294,     0,     0,     0,   166,   237,
       0,   239,   248,   127,     0,     0,     0,     0,   100,   242,
       0,   123,     0,     0,   294,     0,     0,   246,   247,     0,
     157,     0,   110,   209,   210,   297,   214,   215,   225,     0,
     220,     0,   222,   199,     0,   182,     0,   196,     0,   180,
       0,   205,     0,   202,   191,     0,   244,   187,     0,     0,
       0,   141,     0,   151,     0,     0,    70,   105,     0,     0,
      70,     0,    83,    84,     8,     0,   102,    70,   258,   261,
     160,     0,     0,   294,     0,    79,    11,    61,     0,    63,
      40,    48,   291,   292,   293,    42,    45,     0,    47,    44,
      49,    46,   295,     0,    34,     0,     0,   176,   296,   170,
     135,   296,   241,     0,   131,     0,   132,     0,     0,     0,
       0,   124,    21,     0,   249,   107,   213,   221,   223,     0,
       0,     0,   197,     0,   181,     0,     0,   203,   244,     0,
     190,   188,     0,    68,   162,   119,     0,   118,     0,     0,
       0,   150,     0,   106,   217,     0,   153,   104,     0,     9,
       0,   101,     0,    14,     0,    18,    58,     0,   290,    41,
      51,     0,     0,     0,     0,   297,     0,     0,     0,     0,
     240,     0,   128,    99,    70,   126,     0,    23,   200,     0,
     183,     0,   206,     0,     0,   244,   192,   189,     0,   161,
       0,   117,   164,   112,     0,   113,     0,   218,    85,   158,
      13,     0,    55,   294,   294,    43,    33,     0,     0,   178,
     177,   171,   169,   172,     0,   168,   130,     0,    98,   125,
     184,   198,   204,   193,     0,   122,   121,     0,     0,   163,
       0,   111,    17,     0,     0,     0,   174,   173,   129,   194,
     120,   116,   115,     0,    57,   294,    54,   175,   114,     0,
      56
  };

  const short
  parser::yypgoto_[] =
  {
    -229,  -229,  -229,  -229,    96,  -229,  -229,   482,  -229,  -229,
    -229,  -229,    26,     4,    69,  -229,  -229,   200,   201,  -229,
    -194,  -229,   -29,  -222,   -31,   -56,   312,  -229,  -173,    -9,
    -229,  -229,  -229,  -229,  -229,  -229,  -229,  -229,  -229,    -2,
    -229,   197,    22,   194,    33,  -229,  -229,    25,   121,   175,
    -229,  -142,   357,  -229,  -228,  -229,    12,  -229,   -88,   291,
    -229,  -229,  -229,  -141,   296,  -229,   -37,  -229,  -229,  -229,
    -229,  -229,   -35,   -44,  -229,   -18,  -229,  -229,  -229,     3,
    -116
  };

  const short
  parser::yydefgoto_[] =
  {
       0,     5,     6,     7,    50,     8,     9,    10,    11,   103,
      14,    12,   395,   396,   303,   304,   293,   294,   295,   296,
     297,    51,    52,   235,   236,    86,    87,   267,   124,   169,
     170,    53,    54,    55,    56,    57,    58,    59,    60,   108,
     109,   229,    62,   408,   409,   150,   159,   162,   163,   230,
     231,   232,   142,   370,   144,   145,    90,    65,   111,   130,
     131,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,   397,    91,
     207
  };

  const short
  parser::yytable_[] =
  {
      61,    88,   147,   155,   332,   305,   140,   171,   139,   338,
     125,    97,   154,   179,    64,   221,   238,   126,   132,   256,
     298,    98,   261,   350,    17,   436,   298,    63,    89,   156,
      13,   129,   273,   190,   245,   240,    92,   246,   176,   102,
     139,   104,   367,   251,   128,   222,   100,   161,   368,   225,
      93,     1,   172,    23,    23,   180,   187,   188,   239,   141,
     164,    88,    45,   430,    17,    17,   192,   173,   174,   351,
     460,   437,   218,   155,   125,   517,    40,   241,     4,   247,
     105,   126,   154,   186,   291,    46,    46,    88,   -64,   189,
     369,   141,   191,    61,    61,   185,   353,   125,    23,   156,
      23,    48,    45,    45,   126,   431,   329,   258,   184,   331,
     175,   233,   204,   139,   357,   336,   270,   518,   154,   298,
     365,   134,    23,   298,   315,   157,    88,    17,   281,   137,
      46,   138,    46,   143,   115,    23,   166,   118,   119,   292,
     121,   167,   123,   158,   453,    23,   181,   233,   457,   343,
     371,   344,    17,   316,    46,   461,   317,    23,    24,    25,
      26,    88,   234,   324,   141,    45,   182,    46,   268,   469,
      88,   133,   183,   274,   269,   419,   177,    46,   280,   134,
      23,   318,    88,   139,   193,    88,   287,   282,   284,    46,
      45,   194,   195,   289,   361,   153,   391,   139,   234,   196,
     147,   308,   198,   300,   178,   301,   302,   233,  -290,   471,
    -290,   398,    46,   440,   197,   110,   127,    88,   326,   275,
      23,   323,   330,  -290,   400,  -290,   455,   383,   362,    17,
     298,   373,   243,   174,   141,   139,  -290,   340,  -290,   342,
      23,   376,  -290,   139,   262,   349,   263,   199,   141,   392,
     393,   126,    46,    23,   394,   203,   451,  -290,   234,   206,
     244,   358,   528,   148,   392,   393,   205,    45,    23,   394,
    -290,   477,    46,   375,   212,   175,   214,   392,   393,   215,
     127,    88,   394,   385,   381,    46,   141,   277,    23,   233,
     452,   249,   476,   149,   141,   479,   219,   432,   496,   433,
      46,   200,   201,   127,   202,   399,   401,   403,   216,   101,
     208,   210,   220,   211,    24,    25,    26,   135,   136,   250,
      46,   224,  -294,    94,   473,    95,   474,   423,   392,   393,
     110,     1,    96,   394,   110,   146,   151,   139,   237,   271,
     234,   139,   266,   283,   160,     2,     3,   233,   139,   288,
     290,   153,   306,   307,   126,   312,   313,   444,     4,   359,
     147,   328,   337,   335,   347,   133,   168,   456,   374,   377,
     380,   382,   384,   386,   388,   402,   279,   390,   413,   176,
     404,   139,   443,   414,   420,   421,   464,   483,   141,   139,
     422,   424,   141,   425,   427,   446,   428,   434,   234,   141,
     441,   445,   309,   311,   459,   449,   314,   462,   463,   465,
     319,   514,   320,   322,   468,   475,   482,   498,   160,   481,
     484,   487,   492,   488,   333,   334,   497,   501,   503,   490,
     223,   507,   141,   500,   508,   339,   126,   341,   510,   135,
     141,   511,   504,   348,   499,   242,   516,   502,   522,   525,
     355,   524,   531,   248,   233,   139,   529,   360,   509,   530,
     533,   541,   542,    84,   265,    88,   537,   548,   532,   538,
     556,   470,   549,   276,   278,   557,   540,    24,    25,    26,
      27,    28,    29,    30,    31,    32,   553,   555,    23,   299,
     560,    99,   387,   126,   515,   299,   535,   389,   536,   286,
     152,   411,   539,   407,   407,   234,   141,   412,   521,   467,
     426,   327,   415,   417,   153,     0,   543,   544,     0,     0,
      46,     0,   211,     0,   115,   264,   166,   118,   119,   120,
     121,   167,   123,   146,     0,   550,   551,     0,   552,   345,
     346,    88,     0,     0,     0,     0,   352,   354,     0,     0,
       0,   558,     0,   442,     0,     0,     0,     0,   559,   448,
       0,   450,   372,     0,     0,     0,   291,     0,   458,     0,
     -64,   378,     0,   379,     0,     0,     0,     0,     0,     0,
      23,     0,     0,     0,     0,     0,     0,     0,   299,     0,
       0,     0,   299,     0,     0,     0,     0,   300,     0,   301,
     302,   291,   410,   410,     0,   -65,     0,     0,     0,     0,
     480,     0,    46,     0,     0,    23,   115,   486,   166,   118,
     119,   292,   121,   167,   123,     0,     0,   489,     0,     0,
       0,     0,     0,     0,     0,   494,     0,     0,     0,     0,
     429,     0,     0,     0,     0,     0,   435,    46,     0,   506,
       0,   115,     0,   166,   118,   119,   292,   121,   167,   123,
     115,   165,   166,   118,   119,   120,   121,   167,   123,   168,
     519,   520,   407,     0,   523,     0,     0,   213,   527,   115,
     165,   166,   118,   119,   120,   121,   167,   123,   168,     0,
       0,     0,   534,   146,     0,     0,     0,     0,     0,   299,
       0,   472,     0,     0,     0,     0,   478,     0,     0,     0,
       0,     0,     0,     0,   545,   546,     0,     0,     0,     0,
       0,   547,    15,    16,     0,     0,    17,     0,    18,   491,
      19,     0,   493,    20,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,    39,    40,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,   410,     0,     0,   112,    47,     0,    17,     0,    18,
      48,   106,  -244,    49,     0,  -244,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,    37,    38,    39,    40,     0,     0,    41,
      42,    43,    44,     0,   113,    45,     0,     0,     0,     0,
      46,     0,   114,     0,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   112,    49,     0,    17,     0,    18,     0,
     106,  -244,     0,     0,  -244,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,   113,    45,     0,     0,     0,     0,    46,
       0,   114,     0,   115,   116,   117,   118,   119,   120,   121,
     167,   123,   466,    49,     0,  -207,  -207,  -207,     0,  -207,
       0,     0,  -207,     0,  -207,  -207,  -207,  -207,  -207,  -207,
    -207,  -207,  -207,  -207,  -207,  -207,  -207,  -207,  -207,  -207,
    -207,  -207,  -207,  -207,    40,     0,     0,  -207,  -207,  -207,
    -207,     0,     0,  -207,     0,     0,     0,     0,  -207,     0,
       0,     0,     0,     0,  -207,     0,     0,     0,     0,    48,
       0,    17,  -207,    18,     0,    83,     0,    84,    85,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,    39,
      40,     0,     0,    41,    42,    43,    44,     0,     0,    45,
       0,     0,     0,     0,    46,     0,     0,     0,     0,     0,
      47,     0,     0,     0,     0,    48,     0,    17,    49,    18,
     259,   106,     0,     0,     0,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,    37,    38,     0,     0,     0,     0,    41,
      42,    43,    44,     0,   260,    45,     0,     0,     0,     0,
      46,     0,     0,     0,     0,   227,    47,     0,    17,   228,
      18,   363,   106,     0,    49,     0,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,     0,     0,     0,     0,
      41,    42,    43,    44,     0,   364,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,   227,    47,     0,     0,
     228,     0,    17,   512,    18,    49,   106,     0,     0,    85,
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,   513,    37,    38,
      39,     0,     0,     0,    41,    42,    43,    44,     0,     0,
      45,     0,     0,     0,     0,    46,     0,     0,     0,     0,
       0,    47,     0,     0,    17,     0,    18,     0,   106,    49,
      84,    85,     0,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
      37,    38,    39,     0,     0,     0,    41,    42,    43,    44,
       0,     0,    45,     0,     0,     0,     0,    46,     0,     0,
       0,     0,     0,    47,     0,    17,     0,    18,     0,   106,
     217,    49,    85,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,    39,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,    47,    17,     0,    18,     0,   106,
     226,     0,    49,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,   227,    47,     0,     0,   228,     0,    17,
     257,    18,    49,   106,     0,     0,    85,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,    39,     0,     0,
       0,    41,    42,    43,    44,     0,     0,    45,     0,     0,
       0,     0,    46,     0,     0,     0,     0,     0,    47,    17,
       0,    18,     0,   106,   272,     0,    49,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,     0,     0,     0,
       0,    41,    42,    43,    44,     0,     0,    45,     0,     0,
       0,     0,    46,     0,     0,     0,     0,   227,    47,     0,
      17,   228,    18,     0,   106,   366,    49,     0,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,   227,    47,
       0,    17,   228,    18,     0,   106,   418,    49,     0,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,     0,
       0,     0,     0,    41,    42,    43,    44,     0,     0,    45,
       0,     0,     0,     0,    46,     0,     0,     0,     0,   227,
      47,     0,    17,   228,    18,     0,   106,   454,    49,     0,
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     0,    37,    38,
       0,     0,     0,     0,    41,    42,    43,    44,     0,     0,
      45,     0,     0,     0,     0,    46,     0,     0,     0,     0,
     227,    47,     0,     0,   228,     0,    17,   554,    18,    49,
     106,     0,     0,    85,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,    39,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,    47,     0,    17,     0,    18,
       0,   106,     0,    49,    85,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,    37,    38,    39,     0,     0,     0,    41,
      42,    43,    44,     0,     0,    45,     0,     0,     0,     0,
      46,     0,     0,     0,     0,     0,    47,     0,    17,     0,
      18,     0,   106,     0,    49,   285,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,    39,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,     0,    47,    17,     0,
      18,     0,   106,     0,     0,    49,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,   405,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,   406,     0,     0,     0,    47,    17,     0,
      18,     0,   106,     0,     0,    49,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,    37,    38,     0,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,    46,     0,     0,     0,     0,   227,    47,     0,    17,
     228,    18,     0,   106,     0,    49,     0,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,     0,     0,     0,
       0,    41,    42,    43,    44,     0,     0,    45,     0,     0,
       0,     0,    46,     0,     0,     0,     0,   107,    47,     0,
      17,     0,    18,     0,    19,     0,    49,    20,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,    47,
      17,     0,    18,     0,   106,     0,     0,    49,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,   209,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,    47,
      17,     0,    18,     0,   106,     0,     0,    49,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,   310,    47,
      17,     0,    18,     0,   106,   321,     0,    49,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,    47,
      17,     0,    18,     0,   106,     0,     0,    49,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,   325,    47,
      17,     0,    18,     0,   106,   416,     0,    49,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,    47,
      17,     0,    18,   447,   106,     0,     0,    49,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,    47,
      17,     0,    18,     0,   106,   485,     0,    49,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,    47,
      17,     0,    18,   505,   106,     0,     0,    49,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,    47,
      17,     0,    18,     0,   106,   526,     0,    49,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,    47,
      17,     0,    18,     0,   106,     0,     0,    49,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,   252,     0,    18,    47,
     253,     0,     0,     0,     0,   254,     0,    49,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,     0,     0,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,   252,     0,    18,     0,   438,
       0,     0,   255,     0,   254,    47,     0,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,     0,     0,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,   252,     0,    18,     0,   356,     0,
       0,   439,     0,   254,    47,     0,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
       0,     0,     0,     0,     0,     0,    41,    42,    43,    44,
       0,     0,    45,   252,     0,    18,     0,   495,     0,     0,
       0,     0,   254,    47,     0,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,     0,     0,
       0,     0,     0,     0,     0,    41,    42,    43,    44,     0,
       0,    45,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    47
  };

  const short
  parser::yycheck_[] =
  {
       2,     3,    39,    47,   226,   199,    37,    63,    37,   237,
      19,     8,    47,     8,     2,   131,     8,    19,    20,   160,
     193,     0,   164,    12,     4,    12,   199,     2,     3,    47,
       6,    19,   174,    89,   150,     8,    30,     8,    69,    49,
      69,    10,   264,   159,    19,   133,     7,    49,     8,   137,
       0,    30,    10,    15,    15,    50,    10,    11,    50,    37,
       6,    63,    42,     8,     4,     4,     0,    64,     8,    58,
      50,    58,   128,   117,    83,     8,    33,    50,    57,    50,
      49,    83,   117,    85,     1,    47,    47,    89,     5,    86,
      50,    69,    89,    95,    96,    83,    58,   106,    15,   117,
      15,    58,    42,    42,   106,    50,   222,   163,    83,   225,
      50,   140,   109,   142,   255,   231,   172,    50,   153,   292,
     262,    14,    15,   296,    12,    40,   128,     4,   184,     6,
      47,     8,    47,    13,    51,    15,    53,    54,    55,    56,
      57,    58,    59,    58,   366,    15,    27,   176,   370,    10,
       8,    12,     4,    41,    47,   377,     8,    15,    16,    17,
      18,   163,   140,   219,   142,    42,    39,    47,   170,   397,
     172,     6,    39,   175,   171,   317,    12,    47,   180,    14,
      15,   212,   184,   212,     4,   187,   188,   184,   185,    47,
      42,    95,    96,   190,     7,    53,     1,   226,   176,     0,
     237,   203,    30,    32,    40,    34,    35,   236,    13,   403,
      15,     1,    47,   354,     7,    18,    19,   219,   220,    14,
      15,   218,   224,    13,     1,    15,   368,   283,    41,     4,
     403,   266,    12,     8,   212,   264,    13,   239,    15,   241,
      15,   272,    47,   272,     6,   247,     8,     4,   226,    54,
      55,   253,    47,    15,    59,    10,     7,    47,   236,    12,
      40,   258,   484,    10,    54,    55,    49,    42,    15,    59,
      47,    46,    47,   270,     9,    50,     9,    54,    55,    12,
      83,   283,    59,   285,   281,    47,   264,    14,    15,   318,
      41,    12,   408,    40,   272,   411,    10,    10,   439,    12,
      47,   104,   105,   106,   107,   301,   302,   304,    41,    13,
     113,   114,    12,   116,    16,    17,    18,    21,    22,    40,
      47,    50,     0,     1,    47,     3,    49,   324,    54,    55,
     133,    30,    10,    59,   137,    39,    40,   366,    12,     0,
     318,   370,    60,    10,    48,    44,    45,   376,   377,     0,
       0,    53,    12,    12,   356,     7,     9,   359,    57,    50,
     397,     9,     9,    12,    10,     6,    60,   369,     0,     9,
       9,     9,     9,     0,     4,    10,   179,     5,    49,   410,
       5,   410,     5,     9,    41,     9,   383,   418,   366,   418,
       9,     7,   370,     7,     9,    50,     9,     9,   376,   377,
       9,     7,   205,   206,     0,    50,   209,     9,     0,     0,
     213,   467,   215,   216,    12,    12,     9,    50,   122,    41,
       9,     9,    58,    10,   227,   228,     9,     7,     7,   431,
     134,     9,   410,    50,     9,   238,   438,   240,     0,   143,
     418,     9,    50,   246,   446,   149,     5,   449,     5,     5,
     253,    46,    10,   157,   483,   484,     9,   260,   460,     9,
       9,     7,     0,    10,   168,   467,    50,     9,    58,    50,
       5,   402,     9,   177,   178,     9,    50,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    50,   543,    15,   193,
       5,     9,   292,   495,   468,   199,   498,   296,   500,   187,
      39,   307,   504,   306,   307,   483,   484,   310,   475,   388,
     335,   220,   315,   316,    53,    -1,   513,   514,    -1,    -1,
      47,    -1,   325,    -1,    51,   168,    53,    54,    55,    56,
      57,    58,    59,   237,    -1,   537,   538,    -1,   540,   243,
     244,   543,    -1,    -1,    -1,    -1,   250,   251,    -1,    -1,
      -1,   553,    -1,   356,    -1,    -1,    -1,    -1,   555,   362,
      -1,   364,   266,    -1,    -1,    -1,     1,    -1,   371,    -1,
       5,   275,    -1,   277,    -1,    -1,    -1,    -1,    -1,    -1,
      15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   292,    -1,
      -1,    -1,   296,    -1,    -1,    -1,    -1,    32,    -1,    34,
      35,     1,   306,   307,    -1,     5,    -1,    -1,    -1,    -1,
     413,    -1,    47,    -1,    -1,    15,    51,   420,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,   430,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   438,    -1,    -1,    -1,    -1,
     344,    -1,    -1,    -1,    -1,    -1,   350,    47,    -1,   452,
      -1,    51,    -1,    53,    54,    55,    56,    57,    58,    59,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
     473,   474,   475,    -1,   477,    -1,    -1,    49,   481,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
      -1,    -1,   495,   397,    -1,    -1,    -1,    -1,    -1,   403,
      -1,   405,    -1,    -1,    -1,    -1,   410,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   517,   518,    -1,    -1,    -1,    -1,
      -1,   524,     0,     1,    -1,    -1,     4,    -1,     6,   433,
       8,    -1,   436,    11,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    32,    33,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,   475,    -1,    -1,     1,    53,    -1,     4,    -1,     6,
      58,     8,     9,    61,    -1,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    32,    33,    -1,    -1,    36,
      37,    38,    39,    -1,    41,    42,    -1,    -1,    -1,    -1,
      47,    -1,    49,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,     1,    61,    -1,     4,    -1,     6,    -1,
       8,     9,    -1,    -1,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    41,    42,    -1,    -1,    -1,    -1,    47,
      -1,    49,    -1,    51,    52,    53,    54,    55,    56,    57,
      58,    59,     1,    61,    -1,     4,     5,     6,    -1,     8,
      -1,    -1,    11,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    53,    -1,    -1,    -1,    -1,    58,
      -1,     4,    61,     6,    -1,     8,    -1,    10,    11,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    32,
      33,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,
      53,    -1,    -1,    -1,    -1,    58,    -1,     4,    61,     6,
       7,     8,    -1,    -1,    -1,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    41,    42,    -1,    -1,    -1,    -1,
      47,    -1,    -1,    -1,    -1,    52,    53,    -1,     4,    56,
       6,     7,     8,    -1,    61,    -1,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    41,    42,    -1,    -1,    -1,
      -1,    47,    -1,    -1,    -1,    -1,    52,    53,    -1,    -1,
      56,    -1,     4,     5,     6,    61,     8,    -1,    -1,    11,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,
      -1,    53,    -1,    -1,     4,    -1,     6,    -1,     8,    61,
      10,    11,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      30,    31,    32,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,
      -1,    -1,    -1,    53,    -1,     4,    -1,     6,    -1,     8,
       9,    61,    11,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    32,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    53,     4,    -1,     6,    -1,     8,
       9,    -1,    61,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    52,    53,    -1,    -1,    56,    -1,     4,
       5,     6,    61,     8,    -1,    -1,    11,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    32,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,     4,
      -1,     6,    -1,     8,     9,    -1,    61,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    52,    53,    -1,
       4,    56,     6,    -1,     8,     9,    61,    -1,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    52,    53,
      -1,     4,    56,     6,    -1,     8,     9,    61,    -1,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    -1,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    52,
      53,    -1,     4,    56,     6,    -1,     8,     9,    61,    -1,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,
      52,    53,    -1,    -1,    56,    -1,     4,     5,     6,    61,
       8,    -1,    -1,    11,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    32,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    53,    -1,     4,    -1,     6,
      -1,     8,    -1,    61,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    30,    31,    32,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,
      47,    -1,    -1,    -1,    -1,    -1,    53,    -1,     4,    -1,
       6,    -1,     8,    -1,    61,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    32,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    47,    -1,    -1,    -1,    -1,    -1,    53,     4,    -1,
       6,    -1,     8,    -1,    -1,    61,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    32,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    47,    -1,    49,    -1,    -1,    -1,    53,     4,    -1,
       6,    -1,     8,    -1,    -1,    61,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    47,    -1,    -1,    -1,    -1,    52,    53,    -1,     4,
      56,     6,    -1,     8,    -1,    61,    -1,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    52,    53,    -1,
       4,    -1,     6,    -1,     8,    -1,    61,    11,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,
       4,    -1,     6,    -1,     8,    -1,    -1,    61,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    41,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,
       4,    -1,     6,    -1,     8,    -1,    -1,    61,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    52,    53,
       4,    -1,     6,    -1,     8,     9,    -1,    61,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,
       4,    -1,     6,    -1,     8,    -1,    -1,    61,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    52,    53,
       4,    -1,     6,    -1,     8,     9,    -1,    61,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,
       4,    -1,     6,     7,     8,    -1,    -1,    61,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,
       4,    -1,     6,    -1,     8,     9,    -1,    61,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,
       4,    -1,     6,     7,     8,    -1,    -1,    61,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,
       4,    -1,     6,    -1,     8,     9,    -1,    61,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,
       4,    -1,     6,    -1,     8,    -1,    -1,    61,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,     4,    -1,     6,    53,
       8,    -1,    -1,    -1,    -1,    13,    -1,    61,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,     4,    -1,     6,    -1,     8,
      -1,    -1,    50,    -1,    13,    53,    -1,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,     4,    -1,     6,    -1,     8,    -1,
      -1,    50,    -1,    13,    53,    -1,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,     4,    -1,     6,    -1,     8,    -1,    -1,
      -1,    -1,    13,    53,    -1,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
      -1,    42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    53
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,    30,    44,    45,    57,    64,    65,    66,    68,    69,
      70,    71,    74,     6,    73,     0,     1,     4,     6,     8,
      11,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    30,    31,    32,
      33,    36,    37,    38,    39,    42,    47,    53,    58,    61,
      67,    84,    85,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   105,   110,   119,   120,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,     8,    10,    11,    88,    89,   102,   110,
     119,   142,    30,     0,     1,     3,    10,   142,     0,    70,
       7,   127,    49,    72,    10,    49,     8,    52,   102,   103,
     104,   121,     1,    41,    49,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    91,    92,   102,   104,   110,   119,
     122,   123,   102,     6,    14,   127,   127,     6,     8,    85,
      87,   105,   115,    13,   117,   118,   127,   129,    10,    40,
     108,   127,    39,    53,   135,   136,   138,    40,    58,   109,
     127,   102,   110,   111,     6,    52,    53,    58,    60,    92,
      93,    88,    10,   142,     8,    50,    87,    12,    40,     8,
      50,    27,    39,    39,   110,   119,   102,    10,    11,   142,
      88,   142,     0,     4,    67,    67,     0,     7,    30,     4,
     104,   104,   104,    10,   142,    49,    12,   143,   104,    41,
     104,   104,     9,    49,     9,    12,    41,     9,    88,    10,
      12,   143,   121,   127,    50,   121,     9,    52,    56,   104,
     112,   113,   114,    85,   105,    86,    87,    12,     8,    50,
       8,    50,   127,    12,    40,   143,     8,    50,   127,    12,
      40,   143,     4,     8,    13,    50,   126,     5,    88,     7,
      41,   114,     6,     8,   115,   127,    60,    90,   102,   142,
      88,     0,     9,   114,   102,    14,   127,    14,   127,   104,
     102,    88,   142,    10,   142,    11,    89,   102,     0,   142,
       0,     1,    56,    79,    80,    81,    82,    83,    91,   127,
      32,    34,    35,    77,    78,    83,    12,    12,   102,   104,
      52,   104,     7,     9,   104,    12,    41,     8,    87,   104,
     104,     9,   104,   142,    88,    52,   102,   122,     9,   143,
     102,   143,    86,   104,   104,    12,   143,     9,   117,   104,
     102,   104,   102,    10,    12,   127,   127,    10,   104,   102,
      12,    58,   127,    58,   127,   104,     8,   126,   142,    50,
     104,     7,    41,     7,    41,   114,     9,    86,     8,    50,
     116,     8,   127,   135,     0,   142,    87,     9,   127,   127,
       9,   142,     9,    88,     9,   102,     0,    80,     4,    81,
       5,     1,    54,    55,    59,    75,    76,   141,     1,    76,
       1,    76,    10,   142,     5,    32,    49,   104,   106,   107,
     127,   106,   104,    49,     9,   104,     9,   104,     9,   114,
      41,     9,     9,   142,     7,     7,   112,     9,     9,   127,
       8,    50,    10,    12,     9,   127,    12,    58,     8,    50,
     126,     9,   104,     5,   102,     7,    50,     7,   104,    50,
     104,     7,    41,    86,     9,   114,   102,    86,   104,     0,
      50,    86,     9,     0,   142,     0,     1,   111,    12,   117,
      77,    83,   127,    47,    49,    12,   143,    46,   127,   143,
     104,    41,     9,    87,     9,     9,   104,     9,    10,   104,
     102,   127,    58,   127,   104,     8,   126,     9,    50,   102,
      50,     7,   102,     7,    50,     7,   104,     9,     9,   102,
       0,     9,     5,    29,    88,    75,     5,     8,    50,   104,
     104,   107,     5,   104,    46,     5,     9,   104,    86,     9,
       9,    10,    58,     9,   104,   102,   102,    50,    50,   102,
      50,     7,     0,   142,   142,   104,   104,   104,     9,     9,
     102,   102,   102,    50,     5,    88,     5,     9,   102,   142,
       5
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    63,    64,    64,    64,    64,    64,    64,    65,    65,
      65,    66,    66,    66,    66,    66,    66,    66,    66,    67,
      67,    67,    67,    67,    68,    68,    68,    68,    68,    69,
      69,    70,    70,    71,    71,    72,    72,    73,    73,    73,
      74,    75,    76,    76,    77,    77,    77,    77,    77,    77,
      78,    78,    79,    79,    80,    80,    80,    80,    80,    81,
      81,    81,    82,    82,    83,    83,    84,    84,    85,    85,
      86,    86,    87,    87,    87,    87,    88,    88,    88,    88,
      88,    89,    89,    90,    90,    90,    91,    91,    91,    91,
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
     110,   110,   110,   110,   110,   110,   110,   111,   111,   112,
     112,   112,   113,   113,   114,   115,   116,   116,   116,   117,
     117,   117,   117,   117,   118,   118,   119,   120,   120,   120,
     120,   120,   120,   120,   120,   121,   121,   121,   121,   121,
     121,   121,   122,   122,   123,   123,   123,   124,   125,   125,
     126,   126,   126,   127,   127,   128,   128,   128,   128,   129,
     129,   129,   130,   131,   132,   132,   133,   133,   134,   134,
     135,   135,   135,   135,   136,   136,   136,   136,   137,   137,
     138,   138,   139,   139,   140,   140,   140,   140,   140,   140,
     141,   141,   141,   141,   142,   142,   143,   143
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     3,     2,     1,     1,     2,     0,     5,     6,
       4,     5,     4,     7,     6,     4,     3,     8,     6,     1,
       2,     5,     3,     6,     2,     2,     3,     3,     2,     1,
       2,     1,     1,     8,     6,     0,     2,     0,     3,     2,
       5,     2,     1,     3,     2,     2,     2,     2,     2,     2,
       1,     3,     1,     1,     6,     4,     8,     6,     3,     1,
       1,     2,     1,     2,     0,     1,     1,     1,     5,     3,
       0,     1,     1,     1,     2,     2,     1,     3,     3,     4,
       2,     1,     1,     2,     2,     4,     1,     1,     1,     1,
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
       6,     2,     4,     5,     7,     4,     6,     0,     1,     2,
       2,     1,     1,     3,     2,     3,     0,     2,     3,     1,
       3,     4,     3,     4,     1,     3,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     1,     3,     2,     3,
       5,     4,     3,     2,     0,     1,     3,     4,     4,     5,
       1,     1,     1,     1,     1,     2,     3,     3,     4,     1,
       3,     4,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     2,     1,     1,     2,     2,     1,     2,
       0,     1,     1,     1,     0,     1,     0,     1
  };




#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   239,   239,   240,   241,   242,   243,   244,   252,   255,
     258,   269,   272,   275,   278,   281,   284,   287,   290,   299,
     302,   304,   307,   310,   315,   318,   324,   327,   330,   347,
     349,   354,   355,   359,   362,   370,   371,   375,   376,   377,
     381,   390,   395,   397,   402,   404,   406,   408,   418,   428,
     441,   443,   453,   454,   458,   461,   464,   467,   470,   484,
     485,   496,   508,   509,   513,   514,   523,   524,   528,   530,
     535,   536,   540,   541,   542,   543,   547,   548,   549,   554,
     559,   567,   568,   577,   578,   579,   583,   584,   585,   586,
     587,   588,   589,   590,   594,   595,   599,   601,   606,   609,
     612,   616,   619,   622,   625,   628,   631,   641,   644,   649,
     655,   670,   677,   684,   693,   700,   707,   717,   723,   729,
     738,   744,   750,   759,   764,   769,   774,   779,   787,   792,
     798,   803,   808,   821,   824,   827,   830,   833,   836,   843,
     851,   854,   857,   860,   863,   866,   872,   873,   874,   875,
     877,   880,   883,   886,   889,   890,   892,   894,   897,   900,
     903,   909,   912,   915,   918,   926,   927,   931,   939,   942,
     947,   949,   954,   956,   958,   960,   962,   964,   966,   979,
     981,   986,   989,   992,   997,  1003,  1005,  1007,  1009,  1011,
    1013,  1015,  1017,  1019,  1021,  1026,  1027,  1028,  1030,  1032,
    1037,  1046,  1048,  1050,  1052,  1054,  1059,  1071,  1072,  1077,
    1078,  1079,  1083,  1084,  1087,  1089,  1092,  1093,  1095,  1104,
    1106,  1108,  1114,  1116,  1124,  1126,  1130,  1138,  1139,  1140,
    1141,  1142,  1143,  1144,  1145,  1149,  1151,  1153,  1155,  1157,
    1159,  1161,  1169,  1171,  1176,  1178,  1180,  1185,  1190,  1192,
    1201,  1202,  1203,  1211,  1212,  1217,  1219,  1221,  1223,  1227,
    1229,  1231,  1242,  1244,  1247,  1248,  1252,  1253,  1257,  1258,
    1262,  1263,  1264,  1265,  1274,  1275,  1276,  1277,  1281,  1283,
    1288,  1290,  1295,  1297,  1302,  1303,  1304,  1305,  1306,  1307,
    1311,  1312,  1313,  1314,  1326,  1326,  1327,  1327
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
#line 3945 "langutils/sc_parser/src/sc_grammar_parser.cpp"

#line 1329 "langutils/sc_parser/src/sc_grammar.y"

