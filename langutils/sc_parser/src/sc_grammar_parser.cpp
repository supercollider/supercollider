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

  case 33: // class: CLASSNAME class.slot.opt class.super.opt OPENCURLY members.opt method.list.opt CLOSECURLY
#line 360 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassIndex) = cxt.create(Class{ cxt.take_version(yystack_[3].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[6].location), (yystack_[5].value.v_maybe_NamedIdentifierIndex), (yystack_[4].value.v_maybe_ClassNameIdentifierIndex), (yystack_[2].value.v_BigMemberListIndex), (yystack_[1].value.v_MethodListIndex)); }
#line 832 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 34: // class.super.opt: %empty
#line 364 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_maybe_ClassNameIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 838 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 35: // class.super.opt: COLON CLASSNAME
#line 365 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_maybe_ClassNameIdentifierIndex) = cxt.create(ClassNameIdentifier{}, yystack_[0].location); }
#line 844 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 36: // class.slot.opt: %empty
#line 369 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_maybe_NamedIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 850 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 37: // class.slot.opt: OPENSQUARE name CLOSESQUARE
#line 370 "langutils/sc_parser/src/sc_grammar.y"
                                      { (yylhs.value.v_maybe_NamedIdentifierIndex) = (yystack_[1].value.v_NamedIdentifierIndex); }
#line 856 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 38: // class.slot.opt: OPENSQUARE CLOSESQUARE
#line 371 "langutils/sc_parser/src/sc_grammar.y"
                                 { (yylhs.value.v_maybe_NamedIdentifierIndex) = cxt.create(Missing{}, yylhs.location); }
#line 862 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 39: // class.extension: ADD CLASSNAME OPENCURLY method.list.opt CLOSECURLY
#line 376 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ClassExtensionIndex) = cxt.create(ClassExtension{ cxt.take_version(yystack_[2].location) }, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[3].location), (yystack_[1].value.v_MethodListIndex)); }
#line 868 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 40: // members.entry.item: accessor variable_declarations.list.item
#line 385 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberIndex) = cxt.create(DeclareMember{}, yylhs.location, (yystack_[1].value.v_RWAccessorIndex), (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 874 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 41: // members.entry.list: members.entry.item
#line 390 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberListIndex) = cxt.create(DeclareMemberList{}, yylhs.location, (yystack_[0].value.v_DeclareMemberIndex)); }
#line 880 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 42: // members.entry.list: members.entry.list COMMA members.entry.item
#line 392 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareMemberListIndex) = cxt.graph->append((yystack_[2].value.v_DeclareMemberListIndex), (yystack_[0].value.v_DeclareMemberIndex)); }
#line 886 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 43: // members.entry: CLASSVAR members.entry.list
#line 397 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyMemberListIndex) = cxt.graph->cast<DeclareClassMemberList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 892 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 44: // members.entry: VAR members.entry.list
#line 399 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyMemberListIndex) = cxt.graph->cast<DeclareMemberList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 898 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 45: // members.entry: CONST members.entry.list
#line 401 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyMemberListIndex) = cxt.graph->cast<DeclareConstList>((yystack_[0].value.v_DeclareMemberListIndex)); }
#line 904 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 46: // members: members.entry
#line 406 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberListIndex) = cxt.create(BigMemberList{}, yylhs.location, (yystack_[0].value.v_DeclareAnyMemberListIndex));}
#line 910 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 47: // members: members SEMICOLON members.entry
#line 408 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BigMemberListIndex) = cxt.graph->append((yystack_[2].value.v_BigMemberListIndex), (yystack_[0].value.v_DeclareAnyMemberListIndex)); }
#line 916 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 48: // members.opt: %empty
#line 412 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_BigMemberListIndex) = cxt.create(BigMemberList{}, yylhs.location); }
#line 922 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 49: // members.opt: members semicolon.opt
#line 413 "langutils/sc_parser/src/sc_grammar.y"
                                { (yylhs.value.v_BigMemberListIndex) = (yystack_[1].value.v_BigMemberListIndex); }
#line 928 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 50: // method.name: name
#line 421 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 934 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 51: // method.name: binary_op.raw
#line 422 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 940 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 52: // method.base: method.name OPENCURLY argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 427 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(Missing{},yystack_[4].location), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 946 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 53: // method.base: method.name OPENCURLY argument_declarations.opt CLOSECURLY
#line 430 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[3].value.v_SelectorIndex), (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(Missing{},yystack_[2].location), cxt.create(BlockContentsList{}, yystack_[0].location)); }
#line 952 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 54: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt block.contents semicolon.opt CLOSECURLY
#line 433 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[7].value.v_SelectorIndex), (yystack_[5].value.v_DeclareArgumentListIndex), cxt.create(PrimitiveIdentifier{}, yystack_[4].location), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 958 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 55: // method.base: method.name OPENCURLY argument_declarations.opt PRIMITIVENAME semicolon.opt CLOSECURLY
#line 436 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyMethodIndex) = cxt.create(Method{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[3].value.v_DeclareArgumentListIndex), cxt.create(PrimitiveIdentifier{}, yystack_[2].location), cxt.create(BlockContentsList{}, yystack_[1].location)); }
#line 964 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 56: // method.base: method.name OPENCURLY error
#line 439 "langutils/sc_parser/src/sc_grammar.y"
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
#line 979 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 57: // method: method.base
#line 452 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_AnyMethodIndex) = (yystack_[0].value.v_AnyMethodIndex); }
#line 985 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 58: // method: MULTIPLY method.base
#line 454 "langutils/sc_parser/src/sc_grammar.y"
                { 
			if (auto c = cxt.graph->as_a<MethodIndex>((yystack_[0].value.v_AnyMethodIndex))) {
				(yylhs.value.v_AnyMethodIndex) = cxt.graph->cast<ClassMethod>(*c);
			} 
			// leaves as error
			cxt.graph->location((yystack_[0].value.v_AnyMethodIndex)) = yylhs.location; 
			(yylhs.value.v_AnyMethodIndex) = (yystack_[0].value.v_AnyMethodIndex); 
		}
#line 998 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 59: // method.list: method
#line 465 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_MethodListIndex) = cxt.create(MethodList{}, yylhs.location, (yystack_[0].value.v_AnyMethodIndex)); }
#line 1004 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 60: // method.list: method.list method
#line 466 "langutils/sc_parser/src/sc_grammar.y"
                             { (yylhs.value.v_MethodListIndex) = cxt.graph->append((yystack_[1].value.v_MethodListIndex), yylhs.location, (yystack_[0].value.v_AnyMethodIndex)); }
#line 1010 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 61: // method.list.opt: %empty
#line 470 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_MethodListIndex) = cxt.create(MethodList{}, yylhs.location); }
#line 1016 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 62: // method.list.opt: method.list
#line 471 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_MethodListIndex) = (yystack_[0].value.v_MethodListIndex); }
#line 1022 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 63: // block.open: OPENCURLY
#line 480 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_bool) = false; }
#line 1028 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 64: // block.open: BEGINCLOSEDFUNC
#line 481 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_bool) = true; }
#line 1034 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 65: // block: block.open argument_declarations.opt block.contents semicolon.opt CLOSECURLY
#line 486 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockIndex) = cxt.create(BlockNode{ (yystack_[4].value.v_bool) }, yylhs.location, (yystack_[3].value.v_DeclareArgumentListIndex), (yystack_[2].value.v_BlockContentsListIndex)); }
#line 1040 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 66: // block: block.open argument_declarations.opt CLOSECURLY
#line 488 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockIndex) = cxt.create(BlockNode{ (yystack_[2].value.v_bool) }, yylhs.location, (yystack_[1].value.v_DeclareArgumentListIndex), cxt.create(BlockContentsList{}, yylhs.location)); }
#line 1046 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 67: // block.opt_list: %empty
#line 492 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location); }
#line 1052 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 68: // block.opt_list: block.list
#line 493 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_BlockListIndex) = (yystack_[0].value.v_BlockListIndex); }
#line 1058 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 69: // block.list: block
#line 497 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location, (yystack_[0].value.v_BlockIndex)); }
#line 1064 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 70: // block.list: generator
#line 498 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_BlockListIndex) = cxt.create(BlockList{}, yylhs.location, (yystack_[0].value.v_GeneratorAnyIndex)); }
#line 1070 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 71: // block.list: block.list block
#line 499 "langutils/sc_parser/src/sc_grammar.y"
                           { (yylhs.value.v_BlockListIndex) = cxt.graph->append((yystack_[1].value.v_BlockListIndex), yylhs.location, (yystack_[0].value.v_BlockIndex)); }
#line 1076 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 72: // block.list: block.list generator
#line 500 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_BlockListIndex) = cxt.graph->append((yystack_[1].value.v_BlockListIndex), yylhs.location, (yystack_[0].value.v_GeneratorAnyIndex)); }
#line 1082 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 73: // block.contents: block.contents.item
#line 504 "langutils/sc_parser/src/sc_grammar.y"
                              { (yylhs.value.v_BlockContentsListIndex) = cxt.create(BlockContentsList{}, yylhs.location, (yystack_[0].value.v_BlockItemIndex)); }
#line 1088 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 74: // block.contents: block.contents SEMICOLON block.contents.item
#line 505 "langutils/sc_parser/src/sc_grammar.y"
                                                       { (yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[2].value.v_BlockContentsListIndex), yylhs.location, (yystack_[0].value.v_BlockItemIndex)); }
#line 1094 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 75: // block.contents: block.contents NONLOCALRETURN expr
#line 507 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[2].value.v_BlockContentsListIndex), yylhs.location, non_local); 
		}
#line 1103 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 76: // block.contents: block.contents SEMICOLON NONLOCALRETURN expr
#line 512 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.graph->append((yystack_[3].value.v_BlockContentsListIndex), yylhs.location, non_local); 
		}
#line 1112 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 77: // block.contents: NONLOCALRETURN expr
#line 517 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto non_local = cxt.create(NonLocalReturnExpr{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_BlockContentsListIndex) = cxt.create(BlockContentsList{}, yylhs.location, non_local);
		}
#line 1121 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 78: // block.contents.item: expr
#line 524 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_BlockItemIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1127 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 79: // block.contents.item: variable_declarations
#line 525 "langutils/sc_parser/src/sc_grammar.y"
                                { (yylhs.value.v_BlockItemIndex) = (yystack_[0].value.v_DeclareVariableListIndex); }
#line 1133 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 80: // adverb: DOT name
#line 534 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_AdverbIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 1139 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 81: // adverb: DOT integer
#line 535 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_AdverbIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 1145 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 82: // adverb: DOT OPENPAREN expr_or_seq CLOSEPAREN
#line 536 "langutils/sc_parser/src/sc_grammar.y"
                                               { (yylhs.value.v_AdverbIndex) = cxt.create(AdverbExprNode{}, yylhs.location, (yystack_[1].value.v_AnyExprIndex));  }
#line 1151 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 83: // binary_op.raw: BINOP
#line 540 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1157 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 84: // binary_op.raw: READWRITEVAR
#line 541 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1163 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 85: // binary_op.raw: LESSTHAN
#line 542 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1169 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 86: // binary_op.raw: GREATERTHAN
#line 543 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1175 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 87: // binary_op.raw: MINUS
#line 544 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1181 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 88: // binary_op.raw: MULTIPLY
#line 545 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1187 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 89: // binary_op.raw: ADD
#line 546 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1193 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 90: // binary_op.raw: PIPE
#line 547 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ false }, yylhs.location); }
#line 1199 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 91: // binary_op.no_adverb: binary_op.raw
#line 551 "langutils/sc_parser/src/sc_grammar.y"
                          { (yylhs.value.v_SelectorIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 1205 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 92: // binary_op.no_adverb: KEYBINOP
#line 552 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_SelectorIndex) = cxt.create(SelectorNode{ true }, yylhs.location); }
#line 1211 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 93: // binary_op: binary_op.no_adverb adverb
#line 557 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorMaybeAdverbIndex) = cxt.create(SelectorWAdverb{}, yylhs.location, (yystack_[1].value.v_SelectorIndex), (yystack_[0].value.v_AdverbIndex)); }
#line 1217 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 94: // binary_op: binary_op.no_adverb
#line 559 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_SelectorMaybeAdverbIndex) = (yystack_[0].value.v_SelectorIndex); }
#line 1223 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 95: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN arguments CLOSEPAREN block.opt_list
#line 564 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[5].value.v_SelectorIndex), (yystack_[2].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1229 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 96: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN OPENPAREN CLOSEPAREN block.list
#line 567 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[4].value.v_SelectorIndex), cxt.create(ArgumentList{}, yystack_[2].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1235 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 97: // msgsend: OPENPAREN binary_op.no_adverb CLOSEPAREN block.list
#line 570 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[2].value.v_SelectorIndex), cxt.create(ArgumentList{}, yystack_[0].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1241 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 98: // msgsend: name OPENPAREN arguments CLOSEPAREN block.opt_list
#line 574 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[2].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1247 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 99: // msgsend: name OPENPAREN CLOSEPAREN block.list
#line 577 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), cxt.create(ArgumentList{}, yystack_[2].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1253 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 100: // msgsend: name block.list
#line 580 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PrefixMessage{}, yylhs.location, (yystack_[1].value.v_NamedIdentifierIndex), cxt.create(ArgumentList{}, yystack_[0].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1259 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 101: // msgsend: expr DOT name arguments.maybe_paren block.opt_list
#line 583 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(PostfixMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1265 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 102: // msgsend: expr DOT arguments.paren block.opt_list
#line 586 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(ValueMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex)); }
#line 1271 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 103: // msgsend: expr DOT OPENPAREN CLOSEPAREN block.opt_list
#line 589 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(ValueMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[2].location), (yystack_[0].value.v_BlockListIndex)); }
#line 1277 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 104: // msgsend: CLASSNAME OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 599 "langutils/sc_parser/src/sc_grammar.y"
                {  (yylhs.value.v_AnyExprIndex) = cxt.create(CollectionNode{}, yylhs.location, cxt.create(ClassNameIdentifier{}, yystack_[4].location), (yystack_[2].value.v_ArrayIndex)); }
#line 1283 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 105: // msgsend: CLASSNAME block.list
#line 602 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[1].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, yystack_[1].location), (yystack_[0].value.v_BlockListIndex));
		}
#line 1292 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 106: // msgsend: CLASSNAME arguments.paren block.opt_list
#line 607 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[2].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, (yystack_[1].value.v_ArgumentListIndex), (yystack_[0].value.v_BlockListIndex));
		}
#line 1301 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 107: // msgsend: CLASSNAME OPENPAREN CLOSEPAREN block.opt_list
#line 613 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto class_name = cxt.create(ClassNameIdentifier{}, yystack_[3].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create(NewMessage{}, yylhs.location, class_name, cxt.create(ArgumentList{}, yystack_[3].location), (yystack_[0].value.v_BlockListIndex));
		}
#line 1310 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 108: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 628 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[3].value.v_ArgumentListIndex), (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r);
		}
#line 1320 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 109: // expr.dotrange: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 635 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[3].location),  (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1330 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 110: // expr.dotrange: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 642 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[2].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[1].location));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1340 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 111: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 651 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[5].value.v_ArgumentListIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[8].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1350 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 112: // expr.dotrange.assign: expr DOT OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 658 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[4].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[3].location));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1360 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 113: // expr.dotrange.assign: expr DOT OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 665 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[5].location),  (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1370 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 114: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE
#line 675 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[4].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[3].value.v_ArgumentListIndex), (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), r);
		}
#line 1380 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 115: // expr.base.range: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE
#line 681 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[2].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[0].location));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), r);	
		}
#line 1390 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 116: // expr.base.range: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE
#line 687 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[3].location, yystack_[0].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[3].location),  (yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AtMessageIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), r);	
		}
#line 1400 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 117: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 696 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[6].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[5].value.v_ArgumentListIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[7].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1410 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 118: // expr.base.range.assign: expr.base OPENSQUARE arguments DOTDOT CLOSESQUARE EQUALSSIGN expr
#line 702 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, (yystack_[4].value.v_ArgumentListIndex), cxt.create(Missing{}, yystack_[3].location));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1420 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 119: // expr.base.range.assign: expr.base OPENSQUARE DOTDOT expr_or_seq CLOSESQUARE EQUALSSIGN expr
#line 708 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto loc = range(yystack_[5].location, yystack_[2].location);
			const auto r = cxt.create(Range{}, loc, cxt.create(ArgumentList{}, yystack_[5].location),  (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeAssignIndex) = cxt.create(RangeAssign{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), r, (yystack_[0].value.v_AnyExprIndex));
		}
#line 1430 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 120: // range: OPENPAREN expr_or_seq DOTDOT CLOSEPAREN
#line 717 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1439 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 121: // range: OPENPAREN expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 722 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[4].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1448 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 122: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 727 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[6].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1457 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 123: // range: OPENPAREN expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 732 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[4].location, yystack_[2].location), (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[5].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1466 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 124: // range: OPENPAREN DOTDOT expr_or_seq CLOSEPAREN
#line 737 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location);
			(yylhs.value.v_RangeIndex) = cxt.create(Range{}, range(yystack_[3].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1475 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 125: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 745 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[3].location, (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[5].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1484 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 126: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT expr_or_seq CLOSEPAREN
#line 750 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, range(yystack_[5].location, yystack_[3].location), (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[7].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1493 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 127: // range.generator: OPENPAREN COLON expr_or_seq COMMA expr_or_seq DOTDOT CLOSEPAREN
#line 756 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[4].location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[6].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1502 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 128: // range.generator: OPENPAREN COLON DOTDOT expr_or_seq CLOSEPAREN
#line 761 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location);
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, (yystack_[1].value.v_AnyExprIndex));
		}
#line 1511 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 129: // range.generator: OPENPAREN COLON expr_or_seq DOTDOT CLOSEPAREN
#line 766 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto args = cxt.create(ArgumentList{}, yystack_[2].location, (yystack_[2].value.v_AnyExprIndex));
			(yylhs.value.v_RangeGeneratorIndex) = cxt.create(RangeGenerator{}, range(yystack_[4].location, yystack_[0].location), args, cxt.create(Missing{}, yystack_[1].location));
		}
#line 1520 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 130: // expr.base: literal
#line 779 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyPossiblyLiteralIndex); }
#line 1526 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 131: // expr.base: generator
#line 782 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_GeneratorAnyIndex); }
#line 1532 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 132: // expr.base: name
#line 785 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 1538 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 133: // expr.base: CURRYARG
#line 788 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyExprIndex) = cxt.create(CurryNode{}, yystack_[0].location); }
#line 1544 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 134: // expr.base: msgsend
#line 791 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1550 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 135: // expr.base: OPENPAREN expr_or_seq CLOSEPAREN
#line 794 "langutils/sc_parser/src/sc_grammar.y"
                { 
			// A nasty hack because of nil assignment.
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_AnyExprIndex) = (yystack_[1].value.v_AnyExprIndex);
		}
#line 1560 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 136: // expr.base: OPENPAREN error
#line 801 "langutils/sc_parser/src/sc_grammar.y"
                {
			const auto close = error_recovery::bracket(cxt, yystack_[1].location);
			(yylhs.value.v_AnyExprIndex) = cxt.create_error(range(yystack_[1].location, close), yystack_[0].location);
			yyclearin; // The lookahead was skipped.
			yyerrok; // Recovered at the matching bracket, so report the next error.
		}
#line 1571 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 137: // expr.base: TILDE name
#line 809 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(EnvIdentifierNode{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1577 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 138: // expr.base: expr.base OPENSQUARE arguments CLOSESQUARE
#line 812 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex) ); }
#line 1583 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 139: // expr.base: expr.base OPENSQUARE CLOSESQUARE
#line 815 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[1].location)); }
#line 1589 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 140: // expr.base: expr.base.range
#line 818 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AtMessageIndex); }
#line 1595 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 141: // expr.base: range
#line 821 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeIndex); }
#line 1601 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 142: // expr.base: range.generator
#line 824 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeGeneratorIndex); }
#line 1607 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 143: // expr: expr.base
#line 829 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1613 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 144: // expr: expr.base.range.assign
#line 830 "langutils/sc_parser/src/sc_grammar.y"
                                 { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeAssignIndex); }
#line 1619 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 145: // expr: expr.dotrange
#line 831 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AtMessageIndex); }
#line 1625 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 146: // expr: expr.dotrange.assign
#line 832 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_RangeAssignIndex); }
#line 1631 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 147: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE
#line 835 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[1].value.v_ArgumentListIndex) ); }
#line 1637 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 148: // expr: expr DOT OPENSQUARE CLOSESQUARE
#line 838 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AtMessage{}, yylhs.location, (yystack_[3].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[1].location) ); }
#line 1643 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 149: // expr: expr binary_op expr
#line 841 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(BinaryMessage{}, yylhs.location, (yystack_[1].value.v_SelectorMaybeAdverbIndex), (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1649 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 150: // expr: expr DOT name EQUALSSIGN expr
#line 844 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(SetterNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1655 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 151: // expr: CLASSNAME
#line 846 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_AnyExprIndex) = cxt.create(ClassNameIdentifier{}, yylhs.location); }
#line 1661 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 152: // expr: BACKTICK expr
#line 847 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyExprIndex) = cxt.create(ReferenceNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1667 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 153: // expr: name EQUALSSIGN expr
#line 849 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentNode{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1673 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 154: // expr: TILDE name EQUALSSIGN expr
#line 852 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentNode{AssignmentNode::Target::Environment}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1679 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 155: // expr: name OPENPAREN arguments CLOSEPAREN EQUALSSIGN expr
#line 855 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(SetterNode{}, yylhs.location, (yystack_[3].value.v_ArgumentListIndex), (yystack_[5].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1685 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 156: // expr: name.pack EQUALSSIGN expr
#line 858 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[2].value.v_NamePackIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1691 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 157: // expr: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 861 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[3].value.v_NamePackIndex), (yystack_[1].value.v_AnyExprIndex)); }
#line 1697 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 158: // expr: expr.base OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 867 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), (yystack_[3].value.v_ArgumentListIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1703 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 159: // expr: expr.base OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 870 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[3].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 1709 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 160: // expr: expr DOT OPENSQUARE arguments CLOSESQUARE EQUALSSIGN expr
#line 873 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[6].value.v_AnyExprIndex), (yystack_[3].value.v_ArgumentListIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1715 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 161: // expr: expr DOT OPENSQUARE CLOSESQUARE EQUALSSIGN expr
#line 876 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyExprIndex) = cxt.create(AssignmentAtNode{}, yylhs.location, (yystack_[5].value.v_AnyExprIndex), cxt.create(ArgumentList{}, yystack_[3].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 1721 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 162: // expr_or_seq.base: expr
#line 883 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_AnyExprIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 1727 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 163: // expr_or_seq.base: expr_or_seq.base SEMICOLON expr
#line 884 "langutils/sc_parser/src/sc_grammar.y"
                                          { (yylhs.value.v_AnyExprIndex) = cxt.create(ExprSeq{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1733 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 164: // expr_or_seq: expr_or_seq.base semicolon.opt
#line 888 "langutils/sc_parser/src/sc_grammar.y"
                                       { (yylhs.value.v_AnyExprIndex) = (yystack_[1].value.v_AnyExprIndex); }
#line 1739 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 165: // generator: OPENCURLY COLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 897 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorAnyIndex) = cxt.create(GeneratorNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_GeneratorQualListIndex)); }
#line 1745 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 166: // generator: OPENCURLY SEMICOLON expr_or_seq COMMA generator.qual.list comma.opt CLOSECURLY
#line 900 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorAnyIndex) = cxt.create(GeneratorFakeNode{}, yylhs.location, (yystack_[4].value.v_AnyExprIndex), (yystack_[2].value.v_GeneratorQualListIndex)); }
#line 1751 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 167: // generator.qual.list: generator.qual
#line 905 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualListIndex) = cxt.create(GeneratorQualList{}, yylhs.location, (yystack_[0].value.v_GeneratorQualAnyIndex)); }
#line 1757 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 168: // generator.qual.list: generator.qual.list COMMA generator.qual
#line 907 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualListIndex) = cxt.graph->append((yystack_[2].value.v_GeneratorQualListIndex), yylhs.location, (yystack_[0].value.v_GeneratorQualAnyIndex)); }
#line 1763 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 169: // generator.qual: name LEFTARROW expr_or_seq
#line 912 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualAssign{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), cxt.create(Missing{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 1769 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 170: // generator.qual: name name LEFTARROW expr_or_seq
#line 914 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualAssign{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1775 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 171: // generator.qual: VAR name EQUALSSIGN expr_or_seq
#line 916 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualDeclare{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 1781 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 172: // generator.qual: VAR name OPENPAREN expr_or_seq CLOSEPAREN
#line 918 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualDeclare{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex)); }
#line 1787 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 173: // generator.qual: expr_or_seq
#line 920 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualIf{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1793 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 174: // generator.qual: COLON COLON expr_or_seq
#line 922 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualSideEffect{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1799 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 175: // generator.qual: COLON WHILE expr_or_seq
#line 924 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_GeneratorQualAnyIndex) = cxt.create(GeneratorQualGuard{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 1805 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 176: // argument_declarations.list: name
#line 937 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1811 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 177: // argument_declarations.list: name EQUALSSIGN expr
#line 939 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed((yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{is_bracketed}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex))); 
		}
#line 1820 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 178: // argument_declarations.list: name OPENPAREN expr_or_seq CLOSEPAREN
#line 944 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1826 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 179: // argument_declarations.list: argument_declarations.list COMMA name
#line 947 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[2].value.v_DeclareArgumentListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1832 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 180: // argument_declarations.list: argument_declarations.list COMMA name EQUALSSIGN expr
#line 950 "langutils/sc_parser/src/sc_grammar.y"
                { 
			const auto is_bracketed = cxt.graph->is_bracketed((yystack_[0].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[4].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{is_bracketed}, range(yystack_[2].location, yystack_[0].location), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)));
		}
#line 1841 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 181: // argument_declarations.list: argument_declarations.list COMMA name OPENPAREN expr_or_seq CLOSEPAREN
#line 955 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[3].location, yystack_[0].location), (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1847 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 182: // argument_declarations.pipelist: name literal
#line 961 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, (yystack_[1].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1853 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 183: // argument_declarations.pipelist: name
#line 963 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1859 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 184: // argument_declarations.pipelist: name EQUALSSIGN literal
#line 965 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1865 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 185: // argument_declarations.pipelist: name OPENPAREN expr_or_seq CLOSEPAREN
#line 967 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1871 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 186: // argument_declarations.pipelist: name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 969 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, yylhs.location, (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1877 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 187: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name literal
#line 971 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[3].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[1].location, yystack_[0].location), (yystack_[1].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1883 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 188: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name
#line 973 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[2].value.v_DeclareArgumentListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 1889 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 189: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN literal
#line 975 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[4].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{false}, range(yystack_[2].location, yystack_[0].location), (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyPossiblyLiteralIndex))); }
#line 1895 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 190: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name OPENPAREN expr_or_seq CLOSEPAREN
#line 977 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[3].location, yystack_[0].location), (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1901 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 191: // argument_declarations.pipelist: argument_declarations.pipelist comma.opt name EQUALSSIGN OPENPAREN expr_or_seq CLOSEPAREN
#line 979 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[6].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentWithDefaultNode{true}, range(yystack_[4].location, yystack_[0].location), (yystack_[4].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex))); }
#line 1907 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 192: // argument_declarations: ARG SEMICOLON
#line 983 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 1913 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 193: // argument_declarations: ARG argument_declarations.list comma.opt SEMICOLON
#line 984 "langutils/sc_parser/src/sc_grammar.y"
                                                             { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[2].value.v_DeclareArgumentListIndex); }
#line 1919 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 194: // argument_declarations: ARG argument_declarations.list ELLIPSIS name SEMICOLON
#line 986 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[3].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 1925 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 195: // argument_declarations: ARG argument_declarations.list ELLIPSIS name COMMA name SEMICOLON
#line 988 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append( (yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 1931 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 196: // argument_declarations: ARG ELLIPSIS name SEMICOLON
#line 990 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(arg_list, yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); 
		}
#line 1940 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 197: // argument_declarations: ARG ELLIPSIS name COMMA name SEMICOLON
#line 995 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(
				arg_list, yylhs.location,
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))
			); 
		}
#line 1953 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 198: // argument_declarations: PIPE PIPE
#line 1004 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 1959 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 199: // argument_declarations: PIPE argument_declarations.pipelist comma.opt PIPE
#line 1006 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[2].value.v_DeclareArgumentListIndex); }
#line 1965 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 200: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name PIPE
#line 1008 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[3].value.v_DeclareArgumentListIndex), yystack_[3].location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 1971 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 201: // argument_declarations: PIPE argument_declarations.pipelist ELLIPSIS name COMMA name PIPE
#line 1010 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append((yystack_[5].value.v_DeclareArgumentListIndex), yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)), cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); }
#line 1977 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 202: // argument_declarations: PIPE ELLIPSIS name PIPE
#line 1012 "langutils/sc_parser/src/sc_grammar.y"
                { 
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(arg_list, yylhs.location, cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))); 
		}
#line 1986 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 203: // argument_declarations: PIPE ELLIPSIS name COMMA name PIPE
#line 1017 "langutils/sc_parser/src/sc_grammar.y"
                {
			auto arg_list = cxt.create(DeclareArgumentList{}, yylhs.location);
			(yylhs.value.v_DeclareArgumentListIndex) = cxt.graph->append(
				arg_list, yylhs.location,
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[3].location, (yystack_[3].value.v_NamedIdentifierIndex)),
				cxt.create(DeclareArgumentVariadicNode{}, yystack_[1].location, (yystack_[1].value.v_NamedIdentifierIndex))
			); 
		}
#line 1999 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 204: // argument_declarations.opt: %empty
#line 1028 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_DeclareArgumentListIndex) = cxt.create(DeclareArgumentList{}, yylhs.location); }
#line 2005 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 205: // argument_declarations.opt: argument_declarations
#line 1029 "langutils/sc_parser/src/sc_grammar.y"
                                { (yylhs.value.v_DeclareArgumentListIndex) = (yystack_[0].value.v_DeclareArgumentListIndex); }
#line 2011 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 206: // arguments.entries: KEYBINOP expr_or_seq
#line 1034 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_ArgumentEntryIndex) = cxt.create(KwArgNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2017 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 207: // arguments.entries: MULTIPLY expr_or_seq
#line 1035 "langutils/sc_parser/src/sc_grammar.y"
                               { (yylhs.value.v_ArgumentEntryIndex) = cxt.create(VariadicArgNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2023 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 208: // arguments.entries: expr_or_seq
#line 1036 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_ArgumentEntryIndex) = (yystack_[0].value.v_AnyExprIndex); }
#line 2029 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 209: // arguments.no_trailing: arguments.entries
#line 1040 "langutils/sc_parser/src/sc_grammar.y"
                            {  (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location, (yystack_[0].value.v_ArgumentEntryIndex)); }
#line 2035 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 210: // arguments.no_trailing: arguments.no_trailing COMMA arguments.entries
#line 1041 "langutils/sc_parser/src/sc_grammar.y"
                                                        { (yylhs.value.v_ArgumentListIndex) = cxt.graph->append((yystack_[2].value.v_ArgumentListIndex), yylhs.location, (yystack_[0].value.v_ArgumentEntryIndex)); }
#line 2041 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 211: // arguments: arguments.no_trailing comma.opt
#line 1044 "langutils/sc_parser/src/sc_grammar.y"
                                            { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2047 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 212: // arguments.paren: OPENPAREN arguments CLOSEPAREN
#line 1046 "langutils/sc_parser/src/sc_grammar.y"
                                                 { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2053 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 213: // arguments.maybe_paren: %empty
#line 1049 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location); }
#line 2059 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 214: // arguments.maybe_paren: OPENPAREN CLOSEPAREN
#line 1051 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArgumentListIndex) = cxt.create(ArgumentList{}, yylhs.location); }
#line 2065 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 215: // arguments.maybe_paren: OPENPAREN arguments CLOSEPAREN
#line 1053 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArgumentListIndex) = (yystack_[1].value.v_ArgumentListIndex); }
#line 2071 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 216: // variable_declarations.list.item: name
#line 1062 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = (yystack_[0].value.v_NamedIdentifierIndex); }
#line 2077 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 217: // variable_declarations.list.item: name EQUALSSIGN expr
#line 1064 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, (yystack_[2].value.v_NamedIdentifierIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2083 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 218: // variable_declarations.list.item: name OPENPAREN expr_or_seq CLOSEPAREN
#line 1066 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Bracketed, as in the old compiler, so it isn't a literal, see ConstEval::is_legacy_literal.
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(DeclareVariableWithDefaultNode{}, yylhs.location, (yystack_[3].value.v_NamedIdentifierIndex), (yystack_[1].value.v_AnyExprIndex));
		}
#line 2093 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 219: // variable_declarations.list.item: name.pack EQUALSSIGN expr
#line 1072 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[2].value.v_NamePackIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2099 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 220: // variable_declarations.list.item: name.pack OPENPAREN expr_or_seq CLOSEPAREN
#line 1074 "langutils/sc_parser/src/sc_grammar.y"
                {
			cxt.graph->register_bracketed_expr((yystack_[1].value.v_AnyExprIndex));
			(yylhs.value.v_DeclareAnyVariableIndex) = cxt.create(VariablePackAssign{}, yylhs.location, (yystack_[3].value.v_NamePackIndex), (yystack_[1].value.v_AnyExprIndex));
		}
#line 2108 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 221: // variable_declarations.list: variable_declarations.list.item
#line 1082 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DeclareVariableListIndex) = cxt.create(DeclareVariableList{}, yylhs.location, (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 2114 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 222: // variable_declarations.list: variable_declarations.list COMMA variable_declarations.list.item
#line 1084 "langutils/sc_parser/src/sc_grammar.y"
                {(yylhs.value.v_DeclareVariableListIndex) = cxt.graph->append((yystack_[2].value.v_DeclareVariableListIndex), (yystack_[0].value.v_DeclareAnyVariableIndex)); }
#line 2120 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 223: // variable_declarations: VAR variable_declarations.list
#line 1087 "langutils/sc_parser/src/sc_grammar.y"
                                                       { (yylhs.value.v_DeclareVariableListIndex) = (yystack_[0].value.v_DeclareVariableListIndex); }
#line 2126 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 224: // literal.terminal: symbol
#line 1095 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_AnySymbolIndex); }
#line 2132 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 225: // literal.terminal: string
#line 1096 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_StringLitIndex); }
#line 2138 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 226: // literal.terminal: integer
#line 1097 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_IntLitIndex); }
#line 2144 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 227: // literal.terminal: float
#line 1098 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_FloatProducingIndex); }
#line 2150 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 228: // literal.terminal: boolean
#line 1099 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_BooleanLitIndex); }
#line 2156 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 229: // literal.terminal: nil
#line 1100 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_NilLitIndex); }
#line 2162 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 230: // literal.terminal: ascii
#line 1101 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_ASCIIIndex); }
#line 2168 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 231: // literal.terminal: block
#line 1102 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_BlockIndex); }
#line 2174 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 232: // literal.array.contents: %empty
#line 1107 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location); }
#line 2180 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 233: // literal.array.contents: expr_or_seq
#line 1109 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2186 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 234: // literal.array.contents: expr_or_seq COLON expr_or_seq
#line 1111 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2192 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 235: // literal.array.contents: KEYBINOP expr_or_seq
#line 1113 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.create(ArrayNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2198 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 236: // literal.array.contents: literal.array.contents COMMA expr_or_seq
#line 1115 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[2].value.v_ArrayIndex), yylhs.location, (yystack_[0].value.v_AnyExprIndex)); }
#line 2204 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 237: // literal.array.contents: literal.array.contents COMMA expr_or_seq COLON expr_or_seq
#line 1117 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[4].value.v_ArrayIndex), yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2210 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 238: // literal.array.contents: literal.array.contents COMMA KEYBINOP expr_or_seq
#line 1119 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_ArrayIndex) = cxt.graph->append((yystack_[3].value.v_ArrayIndex), yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2216 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 239: // literal.dictionary.entry: expr COLON expr_or_seq
#line 1127 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryEntryIndex) = cxt.create(DictionaryEntryNode{}, yylhs.location, (yystack_[2].value.v_AnyExprIndex), (yystack_[0].value.v_AnyExprIndex)); }
#line 2222 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 240: // literal.dictionary.entry: KEYBINOP expr_or_seq
#line 1129 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryEntryIndex) = cxt.create(DictionaryEntryNode{}, yylhs.location, cxt.create(KeyBinOp{}, yystack_[1].location), (yystack_[0].value.v_AnyExprIndex)); }
#line 2228 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 241: // literal.dictionary.entries: %empty
#line 1134 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.create(DictionaryNode{}, yylhs.location); }
#line 2234 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 242: // literal.dictionary.entries: literal.dictionary.entry
#line 1136 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.create(DictionaryNode{}, yylhs.location, (yystack_[0].value.v_DictionaryEntryIndex)); }
#line 2240 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 243: // literal.dictionary.entries: literal.dictionary.entries COMMA literal.dictionary.entry
#line 1138 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_DictionaryIndex) = cxt.graph->append((yystack_[2].value.v_DictionaryIndex), yylhs.location,  (yystack_[0].value.v_DictionaryEntryIndex)); }
#line 2246 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 244: // literal.dictionary: OPENPAREN literal.dictionary.entries comma.opt CLOSEPAREN
#line 1143 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->location(*(yystack_[2].value.v_DictionaryIndex)) = yylhs.location; (yylhs.value.v_DictionaryIndex) = (yystack_[2].value.v_DictionaryIndex); }
#line 2252 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 245: // literal.array: OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1148 "langutils/sc_parser/src/sc_grammar.y"
                {  cxt.graph->location(*(yystack_[2].value.v_ArrayIndex)) = yylhs.location; (yylhs.value.v_ArrayIndex) = (yystack_[2].value.v_ArrayIndex); }
#line 2258 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 246: // literal.array: HASH OPENSQUARE literal.array.contents comma.opt CLOSESQUARE
#line 1150 "langutils/sc_parser/src/sc_grammar.y"
                {  
			cxt.graph->payload((yystack_[2].value.v_ArrayIndex)).is_immutable = true;
			cxt.graph->location(*(yystack_[2].value.v_ArrayIndex)) = yylhs.location;
			(yylhs.value.v_ArrayIndex) = (yystack_[2].value.v_ArrayIndex);
		}
#line 2268 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 247: // literal: literal.terminal
#line 1158 "langutils/sc_parser/src/sc_grammar.y"
                           { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_AnyPossiblyLiteralIndex); }
#line 2274 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 248: // literal: literal.array
#line 1159 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_ArrayIndex); }
#line 2280 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 249: // literal: literal.dictionary
#line 1160 "langutils/sc_parser/src/sc_grammar.y"
                             { (yylhs.value.v_AnyPossiblyLiteralIndex) = (yystack_[0].value.v_DictionaryIndex); }
#line 2286 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 250: // name: NAME
#line 1168 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_NamedIdentifierIndex) = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 2292 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 251: // name: WHILE
#line 1169 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamedIdentifierIndex) = cxt.create(NamedIdentifier{}, yylhs.location); }
#line 2298 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 252: // name.list: HASH name
#line 1175 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.create(NameList{}, yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2304 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 253: // name.list: HASH TILDE name
#line 1177 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.create(NameList{}, yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2310 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 254: // name.list: name.list COMMA name
#line 1179 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.graph->append((yystack_[2].value.v_NameListIndex), yylhs.location, (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2316 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 255: // name.list: name.list COMMA TILDE name
#line 1181 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NameListIndex) = cxt.graph->append((yystack_[3].value.v_NameListIndex), yylhs.location, cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2322 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 256: // name.pack: name.list
#line 1185 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[0].value.v_NameListIndex), cxt.create(Missing{}, yystack_[0].location)); }
#line 2328 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 257: // name.pack: name.list ELLIPSIS name
#line 1187 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[2].value.v_NameListIndex), (yystack_[0].value.v_NamedIdentifierIndex)); }
#line 2334 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 258: // name.pack: name.list ELLIPSIS TILDE name
#line 1189 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_NamePackIndex) = cxt.create(NamePack{}, yylhs.location, (yystack_[3].value.v_NameListIndex), cxt.create(EnvIdentifierNode{}, range(yystack_[1].location, yystack_[0].location), (yystack_[0].value.v_NamedIdentifierIndex))); }
#line 2340 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 259: // ascii: ASCII
#line 1199 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_ASCIIIndex) = cxt.create(ASCIINode{}, yylhs.location); }
#line 2346 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 260: // nil: NIL
#line 1201 "langutils/sc_parser/src/sc_grammar.y"
          { (yylhs.value.v_NilLitIndex) = cxt.create(NilNode{}, yylhs.location); }
#line 2352 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 261: // boolean: TRUE
#line 1204 "langutils/sc_parser/src/sc_grammar.y"
               { (yylhs.value.v_BooleanLitIndex) = cxt.create(BooleanNode{true}, yylhs.location); }
#line 2358 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 262: // boolean: FALSE
#line 1205 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_BooleanLitIndex) = cxt.create(BooleanNode{false}, yylhs.location); }
#line 2364 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 263: // symbol: SYMBOL_QUOTE
#line 1209 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_AnySymbolIndex) = cxt.create(SymbolQuote{}, yylhs.location); }
#line 2370 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 264: // symbol: SYMBOL_SLASH
#line 1210 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_AnySymbolIndex) = cxt.create(SymbolSlash{}, yylhs.location); }
#line 2376 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 265: // string: STRINGLINE
#line 1214 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_StringLitIndex) = cxt.create(StringLineList{}, yylhs.location, cxt.create(StringLineNode{}, yylhs.location)); }
#line 2382 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 266: // string: string STRINGLINE
#line 1215 "langutils/sc_parser/src/sc_grammar.y"
                            { (yylhs.value.v_StringLitIndex) = cxt.graph->append((yystack_[1].value.v_StringLitIndex), cxt.create(StringLineNode{}, yystack_[0].location)); }
#line 2388 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 267: // integer: INTEGER
#line 1219 "langutils/sc_parser/src/sc_grammar.y"
                  { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{}, yylhs.location); }
#line 2394 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 268: // integer: INTEGER_RADIX
#line 1220 "langutils/sc_parser/src/sc_grammar.y"
                        { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{IntNode::Kind::Radix}, yylhs.location); }
#line 2400 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 269: // integer: HEXADECIMAL
#line 1221 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_IntLitIndex) = cxt.create(IntNode{IntNode::Kind::Hexadecimal}, yylhs.location); }
#line 2406 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 270: // integer: MINUS integer
#line 1223 "langutils/sc_parser/src/sc_grammar.y"
                {
			// Reaches into the previous integer and changes its sign.
			cxt.graph->payload((yystack_[0].value.v_IntLitIndex)).sign = IntNode::Sign::Negative;
			(yylhs.value.v_IntLitIndex) = (yystack_[0].value.v_IntLitIndex);
		}
#line 2416 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 271: // float.raw_unsigned: FLOAT
#line 1231 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{}, yylhs.location); }
#line 2422 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 272: // float.raw_unsigned: FLOAT_RADIX
#line 1232 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Radix}, yylhs.location); }
#line 2428 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 273: // float.raw_unsigned: FLOAT_EXPONENT
#line 1233 "langutils/sc_parser/src/sc_grammar.y"
                         { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Exponent}, yylhs.location); }
#line 2434 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 274: // float.raw_unsigned: FLOAT_INF
#line 1234 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_FloatLitIndex) = cxt.create(FloatNode{FloatNode::Kind::Inf}, yylhs.location); }
#line 2440 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 275: // float.raw: float.raw_unsigned
#line 1239 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_FloatLitIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2446 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 276: // float.raw: MINUS float.raw_unsigned
#line 1241 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload((yystack_[0].value.v_FloatLitIndex)).sign = FloatNode::Sign::Negative; (yylhs.value.v_FloatLitIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2452 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 277: // accidental.unsigned: ACCIDENTAL_STEPS
#line 1246 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = cxt.create(AccidentalNode{AccidentalNode::Kind::Steps}, yylhs.location); }
#line 2458 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 278: // accidental.unsigned: ACCIDENTAL_CENTS
#line 1248 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = cxt.create(AccidentalNode{AccidentalNode::Kind::Cents}, yylhs.location); }
#line 2464 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 279: // accidental: accidental.unsigned
#line 1253 "langutils/sc_parser/src/sc_grammar.y"
                { (yylhs.value.v_AccidentalIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2470 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 280: // accidental: MINUS accidental.unsigned
#line 1255 "langutils/sc_parser/src/sc_grammar.y"
                { cxt.graph->payload((yystack_[0].value.v_AccidentalIndex)).sign = AccidentalNode::Sign::Negative; (yylhs.value.v_AccidentalIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2476 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 281: // float: float.raw
#line 1259 "langutils/sc_parser/src/sc_grammar.y"
                    { (yylhs.value.v_FloatProducingIndex) = (yystack_[0].value.v_FloatLitIndex); }
#line 2482 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 282: // float: accidental
#line 1260 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_FloatProducingIndex) = (yystack_[0].value.v_AccidentalIndex); }
#line 2488 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 283: // float: float.raw PI
#line 1261 "langutils/sc_parser/src/sc_grammar.y"
                       { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, (yystack_[1].value.v_FloatLitIndex)); }
#line 2494 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 284: // float: integer PI
#line 1262 "langutils/sc_parser/src/sc_grammar.y"
                     { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, (yystack_[1].value.v_IntLitIndex)); }
#line 2500 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 285: // float: PI
#line 1263 "langutils/sc_parser/src/sc_grammar.y"
              { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{}, yylhs.location, cxt.create(Missing{}, yylhs.location)); }
#line 2506 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 286: // float: MINUS PI
#line 1264 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_FloatProducingIndex) = cxt.create(PiNode{PiNode::Sign::Negative}, yylhs.location, cxt.create(Missing{}, yylhs.location)); }
#line 2512 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 287: // accessor: %empty
#line 1268 "langutils/sc_parser/src/sc_grammar.y"
                 { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::Private}, yylhs.location); }
#line 2518 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 288: // accessor: LESSTHAN
#line 1269 "langutils/sc_parser/src/sc_grammar.y"
                   { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyReadable}, yylhs.location); }
#line 2524 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 289: // accessor: GREATERTHAN
#line 1270 "langutils/sc_parser/src/sc_grammar.y"
                      { (yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{ReadWriteAccessor::ExternallyWriteable}, yylhs.location); }
#line 2530 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;

  case 290: // accessor: READWRITEVAR
#line 1272 "langutils/sc_parser/src/sc_grammar.y"
                { 
			(yylhs.value.v_RWAccessorIndex) = cxt.create(RWAccessor{
				combine<ReadWriteAccessor::ExternallyReadable, ReadWriteAccessor::ExternallyWriteable>()
			}, yylhs.location); 
		}
#line 2540 "langutils/sc_parser/src/sc_grammar_parser.cpp"
    break;


#line 2544 "langutils/sc_parser/src/sc_grammar_parser.cpp"

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




  const short parser::yypact_ninf_ = -233;

  const short parser::yytable_ninf_ = -292;

  const short
  parser::yypact_[] =
  {
     272,    33,   815,   992,     6,   124,  -233,  -233,   311,    22,
    -233,  -233,  -233,    98,   108,  -233,  -233,    17,  1820,   877,
    2371,   119,   129,  -233,  -233,  -233,  -233,  -233,  -233,  -233,
    -233,  -233,  -233,  -233,  -233,  -233,  -233,   135,  -233,   223,
     125,  -233,  -233,  -233,  -233,  -233,  -233,   270,    91,  2371,
    -233,     2,  -233,  -233,  -233,  -233,  -233,  -233,  -233,  -233,
     141,   543,  -233,  1668,   150,  -233,  -233,  -233,  -233,    36,
     155,    18,  -233,  -233,  -233,  -233,   144,   153,  -233,   164,
    -233,  -233,  -233,   877,  -233,  2371,    52,  -233,   543,  1205,
    -233,   183,   210,  -233,  -233,  1871,  1871,   233,  -233,  -233,
    -233,   241,   205,   236,  2371,  2371,   936,  2371,   543,   240,
     206,   257,  -233,  2371,  1921,  -233,  2371,   270,  -233,  -233,
    -233,  -233,    91,  -233,  -233,   287,   414,   237,  1256,   269,
    -233,   288,   543,  1820,   129,  -233,   249,  1820,  1306,  -233,
     110,  -233,   110,   176,  -233,   291,    21,    25,  -233,   129,
     192,    30,  -233,   163,  -233,  -233,  -233,   129,  -233,   218,
    2417,  -233,  -233,  1360,  1048,  -233,  -233,  -233,   122,   244,
    2371,    52,  1668,   310,  1410,  2371,   110,   212,   228,  2371,
    2371,  -233,  -233,  -233,  1205,   303,   543,  1719,  2371,   315,
      52,   318,  -233,   209,  -233,  -233,  -233,  -233,  -233,   242,
     312,   316,  -233,  2371,  -233,  2371,  1971,   320,   313,  2371,
      95,  -233,   147,  2371,  -233,  2371,  2021,  -233,    52,  1668,
    2071,   322,   257,  -233,  2371,   257,   110,  2371,  2371,  -233,
    -233,   321,   325,  -233,  -233,  -233,   110,   223,  2371,  2371,
    2371,  2371,    45,   129,   129,   326,  2371,  2371,     1,  -233,
     129,    43,  -233,  2071,   329,  2495,  -233,  -233,    52,   289,
    2371,    23,  1099,  1461,   110,    34,   194,  -233,   277,   340,
      52,  -233,   110,   332,   543,   129,  -233,   129,  -233,   333,
     543,    52,   334,  1668,   335,  2371,  -233,   543,  -233,   346,
    -233,   380,   343,  -233,  -233,   209,   344,  -233,  -233,    -5,
      -5,    -5,  -233,   338,   209,   637,   637,   543,  -233,  2371,
     301,  -233,  -233,   345,  2371,  2121,  1512,   110,  -233,   319,
    -233,   348,   350,    52,  2371,   414,  -233,  -233,   354,   543,
     355,  -233,  -233,  -233,  1769,  -233,  -233,  -233,   356,   543,
     358,   543,  -233,   129,    35,    60,  -233,   359,   543,   129,
    -233,    11,  -233,  2456,   360,  2071,  -233,   365,  2371,   364,
     324,  2171,   328,  2371,    24,   110,  -233,  1563,  2371,   110,
    2371,  -233,  -233,  -233,   351,   110,    49,  -233,  -233,  -233,
     363,   375,    52,   376,   543,  -233,  -233,   691,  -233,  -233,
    -233,  -233,  -233,  -233,   367,   223,   367,   367,   242,  -233,
     381,   129,    -1,  -233,   368,  -233,   112,   368,  -233,  2371,
    -233,   352,  -233,   378,   110,   382,  2221,  -233,  -233,   383,
    -233,  -233,  -233,  -233,  -233,   379,  2371,  2371,  -233,   129,
    -233,   336,   129,  -233,  2071,  2534,  -233,  -233,   390,  -233,
     543,   353,  2371,   361,   393,  2371,   395,   362,  2271,  -233,
    -233,   396,   543,  -233,   400,  -233,  2371,  -233,   404,  -233,
     401,  -233,  -233,  1153,    -5,  -233,  -233,  -233,    48,  2371,
    2371,   637,   410,  2371,   371,   413,  -233,  2321,  -233,   110,
     110,  -233,   411,  -233,  -233,   412,   543,   403,  -233,   366,
     417,  2071,  -233,  -233,  2371,   543,  2371,   369,   543,   373,
    2371,   391,   421,  -233,  -233,   543,  -233,   429,  -233,   420,
      52,  -233,  2371,  2371,  -233,  -233,  -233,  -233,  -233,  2371,
    -233,  -233,   433,  -233,  -233,  -233,  -233,  -233,  -233,   435,
     543,   543,  2371,  2371,   543,  2371,   398,  -233,  1617,   441,
     440,  -233,  -233,  -233,  -233,   543,   543,   543,  2371,  -233,
      52,  -233,  -233,   543,   445,  -233
  };

  const short
  parser::yydefact_[] =
  {
       7,    36,     0,   291,     0,     0,     4,     5,     0,     0,
      29,    31,    32,     0,    34,     3,    25,    63,   232,     0,
       0,     0,     0,   250,   267,   268,   269,   271,   272,   273,
     274,   277,   278,   263,   264,   265,   259,   151,   133,     0,
       0,   260,   261,   262,   285,    64,   251,     0,     0,     0,
      24,   204,   231,   134,   145,   146,   140,   144,   141,   142,
     143,    19,   131,     0,   291,   247,   249,   248,   130,   132,
     256,     0,   230,   229,   228,   224,   225,   226,   275,   281,
     279,   282,   227,     0,   292,     0,   291,    73,    78,   291,
      79,     0,     0,     1,    28,     0,   292,     0,     6,    30,
      38,     0,     0,     0,     0,     0,     0,     0,   162,   291,
     233,   293,   136,     0,     0,    83,    92,    87,    85,    86,
      88,    89,    90,    84,    91,     0,   162,     0,     0,     0,
     242,   293,    20,   232,     0,   252,   137,   232,     0,    69,
     105,    70,    67,     0,   221,   223,   216,     0,   192,     0,
     293,   176,   286,     0,   270,   276,   280,     0,   198,   293,
     183,   152,   205,     0,     0,    92,    87,    90,     0,    94,
       0,   291,   292,     0,     0,     0,   100,     0,     0,     0,
       0,   266,   284,   283,   291,   291,    77,   292,     0,     0,
     291,     0,    16,    61,    27,    26,     2,    37,    35,    48,
       0,     0,   235,   292,   164,     0,   294,     0,     0,     0,
       0,   240,     0,     0,   135,     0,     0,    22,   291,     0,
     294,     0,   293,   253,     0,   293,    67,     0,     0,   208,
     209,   293,     0,    71,    72,   106,    68,     0,     0,     0,
       0,     0,     0,   294,     0,     0,     0,     0,     0,   294,
       0,     0,    63,   241,     0,     0,   182,    66,   291,   139,
       0,     0,     0,     0,    67,   213,     0,    93,   149,     0,
     291,    10,     0,     0,   153,     0,   254,     0,   257,     0,
     156,   291,     0,   292,     0,     0,    74,    75,    15,     0,
      12,    88,     0,    57,    59,    62,     0,    51,    50,   287,
     287,   287,    46,   291,    61,     0,     0,   163,   234,     0,
     236,   245,   124,     0,     0,     0,     0,    97,   239,     0,
     120,     0,     0,   291,     0,     0,   243,   244,     0,   154,
       0,   107,   206,   207,   294,   211,   212,   222,     0,   217,
       0,   219,   196,     0,   179,     0,   193,     0,   177,     0,
     202,     0,   199,   188,     0,   241,   184,     0,     0,     0,
     138,     0,   148,     0,     0,    67,   102,     0,     0,    67,
       0,    80,    81,     8,     0,    99,    67,   255,   258,   157,
       0,     0,   291,     0,    76,    11,    58,     0,    60,    39,
     288,   289,   290,    41,    44,     0,    43,    45,   292,    49,
       0,     0,     0,   173,   293,   167,   132,   293,   238,     0,
     128,     0,   129,     0,     0,     0,     0,   121,    21,     0,
     246,   104,   210,   218,   220,     0,     0,     0,   194,     0,
     178,     0,     0,   200,   241,     0,   187,   185,     0,    65,
     159,   116,     0,   115,     0,     0,     0,   147,     0,   103,
     214,     0,   150,   101,     0,     9,     0,    98,     0,    14,
       0,    18,    56,     0,   287,    40,    47,    33,     0,     0,
       0,   294,     0,     0,     0,     0,   237,     0,   125,    96,
      67,   123,     0,    23,   197,     0,   180,     0,   203,     0,
       0,   241,   189,   186,     0,   158,     0,   114,   161,   109,
       0,   110,     0,   215,    82,   155,    13,     0,    53,   291,
     291,    42,     0,     0,   175,   174,   168,   166,   169,     0,
     165,   127,     0,    95,   122,   181,   195,   201,   190,     0,
     119,   118,     0,     0,   160,     0,   108,    17,     0,     0,
       0,   171,   170,   126,   191,   117,   113,   112,     0,    55,
     291,    52,   172,   111,     0,    54
  };

  const short
  parser::yypgoto_[] =
  {
    -233,  -233,  -233,  -233,    26,  -233,  -233,   442,  -233,  -233,
    -233,  -233,   -12,   -48,    58,  -233,  -233,  -233,   166,   165,
    -233,   154,  -233,     8,  -223,   -18,   -55,   292,  -233,  -172,
      -9,  -233,  -233,  -233,  -233,  -233,  -233,  -233,  -233,  -233,
      -2,  -233,    92,   408,   169,     7,  -233,  -233,    13,    72,
     146,  -233,  -160,   314,  -233,  -232,  -233,     5,  -233,    82,
     261,  -233,  -233,  -233,  -140,   342,  -233,   -37,  -233,  -233,
    -233,  -233,  -233,   -35,   -41,  -233,   -22,  -233,  -233,  -233,
       3,  -122
  };

  const short
  parser::yydefgoto_[] =
  {
       0,     5,     6,     7,    50,     8,     9,    10,    11,   103,
      14,    12,   393,   394,   302,   303,   304,   292,   293,   294,
     295,   296,    51,    52,   235,   236,    86,    87,   267,   124,
     169,   170,    53,    54,    55,    56,    57,    58,    59,    60,
     108,   109,   229,    62,   404,   405,   150,   159,   162,   163,
     230,   231,   232,   142,   369,   144,   145,    90,    65,   111,
     130,   131,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,   395,
      91,   207
  };

  const short
  parser::yytable_[] =
  {
      61,    88,   147,   331,   261,   337,   155,    64,   171,   221,
     125,    97,   154,   349,   273,    63,    89,   126,   132,   140,
     256,   297,    98,   432,   129,   156,   179,   104,   245,   238,
     360,   447,   128,   240,   190,    40,    92,   251,   246,    13,
      17,   366,   367,   426,   174,   139,   469,   161,   470,   390,
     391,   176,     1,    17,   392,   342,   512,   343,    23,   350,
      48,    88,   187,   188,   361,   448,   105,   173,   180,   433,
     428,   239,   429,   218,   125,   241,   155,   139,    45,     4,
     247,   126,   154,   186,   368,   427,   175,    88,   185,   189,
      46,    45,   191,    61,    61,   156,   184,   125,   513,   456,
     328,   352,   364,   330,   126,   100,    23,   314,   258,   335,
     110,   127,   204,    23,    17,   356,    17,   270,   154,   297,
     174,   194,   195,   297,    93,   133,    88,    23,   262,   281,
     263,   157,   297,   134,    23,   148,   315,    23,    46,    17,
      23,   137,   449,   138,    23,    46,   453,   164,   233,   158,
     139,    17,    45,   457,    45,   316,   415,   102,   473,    46,
     172,    88,   175,   465,   323,   149,    46,   177,   268,    46,
      88,   181,    46,   274,   269,   127,    46,    45,   280,    24,
      25,    26,    88,   192,   233,    88,   287,   282,   284,    45,
     134,    23,   182,   289,   317,   178,   200,   201,   127,   202,
     147,   307,   370,   183,   243,   208,   210,   451,   211,    23,
      24,    25,    26,   436,   193,   222,   153,    88,   325,   225,
     139,   322,   329,    46,    23,   110,   275,    23,   382,   110,
     249,   372,   244,   196,   139,   198,   143,   339,    23,   341,
     199,    46,   277,    23,   233,   348,   214,   153,   197,   215,
     203,   126,   396,   397,   375,   205,    46,   523,   250,    46,
     115,   357,   166,   118,   119,   291,   121,   167,   123,   206,
      46,   279,   139,   374,   299,    46,   300,   301,   216,   219,
     139,    88,   472,   384,   380,   475,    24,    25,    26,    27,
      28,    29,    30,    31,    32,   492,   212,   308,   310,   224,
     220,   313,     1,   237,   266,   318,   399,   319,   321,   152,
     271,  -291,    94,   283,    95,   288,     2,     3,   290,   332,
     333,    96,   312,   153,   305,   233,   419,   311,   306,     4,
     338,   327,   340,   334,   336,   133,   346,   168,   347,   358,
     373,   376,   379,   381,   383,   354,   385,   387,   398,   389,
     409,   455,   359,   126,   410,   101,   440,   417,   147,   418,
     416,   420,   421,   135,   136,   423,   452,   424,   430,   437,
     439,   441,   458,   139,   442,   459,   461,   139,   445,   464,
     471,   146,   151,   233,   139,   460,   467,   478,   176,   484,
     160,   480,   483,   477,   488,    23,   479,   403,   403,   493,
     497,   408,   499,   494,   506,   503,   411,   413,   510,   504,
     507,   496,   500,   526,   139,   517,   211,   519,   520,   532,
     524,   525,   139,   533,   527,   486,   528,    46,   536,   537,
      84,   115,   126,   166,   118,   119,   120,   121,   167,   123,
     495,   535,   543,   498,   544,   141,   551,   438,   548,   552,
     555,    99,   511,   444,   505,   446,   466,   386,   400,   463,
     388,    88,   454,   213,   160,   115,   165,   166,   118,   119,
     120,   121,   167,   123,   168,   407,   223,   141,   516,   286,
     422,   326,   264,   550,     0,   135,     0,   233,   139,   126,
       0,   242,   530,     0,   531,     0,     0,     0,   534,   248,
       0,   476,     0,     0,     0,     0,     0,     0,   482,     0,
     265,     0,   538,   539,     0,     0,     0,     0,   485,   276,
     278,     0,     0,     0,     0,     0,   490,     0,     0,     0,
     545,   546,     0,   547,     0,   298,    88,     0,     0,     0,
     502,     0,     0,     0,     0,     0,   553,     0,   234,     0,
     141,     0,     0,   554,     0,     0,     0,     0,     0,     0,
       0,   514,   515,   403,     0,   518,     0,     0,     0,   522,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   146,
       0,     0,     0,   529,   234,   344,   345,     0,     0,     0,
       0,     0,   351,   353,   115,   165,   166,   118,   119,   120,
     121,   167,   123,   168,   540,   541,     0,     0,   371,     0,
       0,   542,     0,     0,     0,     0,     0,   377,     0,   378,
     141,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   298,   141,     0,     0,   298,     0,     0,
       0,    17,     0,    18,   234,   106,   298,   406,   406,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,   401,
       0,     0,   141,    41,    42,    43,    44,     0,     0,    45,
     141,     0,     0,     0,    46,   425,   402,     0,     0,     0,
      47,   431,   462,     0,     0,  -204,  -204,  -204,    49,  -204,
       0,     0,  -204,     0,  -204,  -204,  -204,  -204,  -204,  -204,
    -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,
    -204,  -204,  -204,  -204,    40,   234,     0,  -204,  -204,  -204,
    -204,     0,     0,  -204,     0,     0,     0,   146,  -204,     0,
       0,     0,     0,   468,  -204,     0,     0,     0,   474,    48,
       0,     0,  -204,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   487,     0,   141,   489,     0,     0,   141,     0,     0,
       0,     0,     0,   234,   141,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   406,   141,    15,    16,     0,     0,    17,
       0,    18,   141,    19,     0,     0,    20,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,    39,    40,     0,
       0,    41,    42,    43,    44,     0,     0,    45,     0,     0,
       0,     0,    46,     0,     0,     0,     0,     0,    47,     0,
       0,     0,     0,    48,     0,     0,    49,     0,   112,     0,
       0,    17,     0,    18,     0,   106,  -241,   234,   141,  -241,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,    39,
      40,     0,     0,    41,    42,    43,    44,     0,   113,    45,
       0,     0,     0,     0,    46,     0,   114,     0,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   112,    49,     0,
      17,     0,    18,     0,   106,  -241,     0,     0,  -241,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,   113,    45,     0,
       0,     0,     0,    46,     0,   114,     0,   115,   116,   117,
     118,   119,   120,   121,   167,   123,    17,    49,    18,     0,
      83,     0,    84,    85,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,    39,    40,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,     0,    47,     0,     0,     0,     0,
      48,     0,    17,    49,    18,   259,   106,     0,     0,     0,
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     0,    37,    38,
       0,     0,     0,     0,    41,    42,    43,    44,     0,   260,
      45,     0,     0,     0,     0,    46,     0,     0,     0,     0,
     227,    47,     0,    17,   228,    18,   362,   106,     0,    49,
       0,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,     0,    37,
      38,     0,     0,     0,     0,    41,    42,    43,    44,     0,
     363,    45,     0,     0,     0,     0,    46,     0,     0,     0,
       0,   227,    47,     0,     0,   228,     0,    17,   508,    18,
      49,   106,     0,     0,    85,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,   509,    37,    38,    39,     0,     0,     0,    41,
      42,    43,    44,     0,     0,    45,     0,     0,     0,     0,
      46,     0,     0,     0,     0,     0,    47,     0,     0,    17,
       0,    18,     0,   106,    49,    84,    85,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,    39,     0,     0,
       0,    41,    42,    43,    44,     0,     0,    45,     0,     0,
       0,     0,    46,     0,     0,     0,     0,     0,    47,     0,
      17,     0,    18,     0,   106,   217,    49,    85,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,    39,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,    47,
      17,     0,    18,     0,   106,   226,     0,    49,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,    37,    38,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,     0,
       0,     0,     0,    46,     0,     0,     0,     0,   227,    47,
       0,     0,   228,     0,    17,   257,    18,    49,   106,     0,
       0,    85,     0,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
      37,    38,    39,     0,     0,     0,    41,    42,    43,    44,
       0,     0,    45,     0,     0,     0,     0,    46,     0,     0,
       0,     0,     0,    47,    17,     0,    18,     0,   106,   272,
       0,    49,     0,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
      37,    38,     0,     0,     0,     0,    41,    42,    43,    44,
       0,     0,    45,     0,     0,     0,     0,    46,     0,     0,
       0,     0,   227,    47,     0,    17,   228,    18,     0,   106,
     365,    49,     0,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,   227,    47,     0,    17,   228,    18,     0,
     106,   414,    49,     0,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     0,    37,    38,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,    45,     0,     0,     0,     0,    46,
       0,     0,     0,     0,   227,    47,     0,    17,   228,    18,
       0,   106,   450,    49,     0,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,    37,    38,     0,     0,     0,     0,    41,
      42,    43,    44,     0,     0,    45,     0,     0,     0,     0,
      46,     0,     0,     0,     0,   227,    47,     0,     0,   228,
       0,    17,   549,    18,    49,   106,     0,     0,    85,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,    37,    38,    39,
       0,     0,     0,    41,    42,    43,    44,     0,     0,    45,
       0,     0,     0,     0,    46,     0,     0,     0,     0,     0,
      47,     0,    17,     0,    18,     0,   106,     0,    49,    85,
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     0,    37,    38,
      39,     0,     0,     0,    41,    42,    43,    44,     0,     0,
      45,     0,     0,     0,     0,    46,     0,     0,     0,     0,
       0,    47,     0,    17,     0,    18,     0,   106,     0,    49,
     285,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,     0,    37,
      38,    39,     0,     0,     0,    41,    42,    43,    44,     0,
       0,    45,     0,     0,     0,     0,    46,     0,     0,     0,
       0,     0,    47,    17,     0,    18,     0,   106,     0,     0,
      49,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,     0,    37,
      38,     0,     0,     0,     0,    41,    42,    43,    44,     0,
       0,    45,     0,     0,     0,     0,    46,     0,     0,     0,
       0,   227,    47,     0,    17,   228,    18,     0,   106,     0,
      49,     0,     0,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,     0,
      37,    38,     0,     0,     0,     0,    41,    42,    43,    44,
       0,     0,    45,     0,     0,     0,     0,    46,     0,     0,
       0,     0,   107,    47,     0,    17,     0,    18,     0,    19,
       0,    49,    20,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,    47,    17,     0,    18,     0,   106,
       0,     0,    49,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,   209,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,    47,    17,     0,    18,     0,   106,
       0,     0,    49,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,   309,    47,    17,     0,    18,     0,   106,
     320,     0,    49,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,    47,    17,     0,    18,     0,   106,
       0,     0,    49,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,   324,    47,    17,     0,    18,     0,   106,
     412,     0,    49,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,    47,    17,     0,    18,   443,   106,
       0,     0,    49,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,    47,    17,     0,    18,     0,   106,
     481,     0,    49,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,    47,    17,     0,    18,   501,   106,
       0,     0,    49,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,    47,    17,     0,    18,     0,   106,
     521,     0,    49,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,    47,    17,     0,    18,     0,   106,
       0,     0,    49,     0,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,    37,    38,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,    45,     0,     0,     0,     0,    46,     0,
       0,   252,     0,    18,    47,   253,     0,     0,     0,     0,
     254,     0,    49,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,     0,     0,     0,
       0,     0,     0,    41,    42,    43,    44,     0,     0,    45,
     252,     0,    18,     0,   434,     0,     0,   255,     0,   254,
      47,     0,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     0,     0,     0,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,    45,   252,
       0,    18,     0,   355,     0,     0,   435,     0,   254,    47,
       0,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,     0,     0,     0,     0,     0,
       0,    41,    42,    43,    44,     0,     0,    45,   252,     0,
      18,     0,   491,     0,     0,     0,     0,   254,    47,     0,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,     0,     0,     0,     0,     0,     0,
      41,    42,    43,    44,     0,     0,    45,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    47
  };

  const short
  parser::yycheck_[] =
  {
       2,     3,    39,   226,   164,   237,    47,     2,    63,   131,
      19,     8,    47,    12,   174,     2,     3,    19,    20,    37,
     160,   193,     0,    12,    19,    47,     8,    10,   150,     8,
       7,     7,    19,     8,    89,    33,    30,   159,     8,     6,
       4,   264,     8,     8,     8,    37,    47,    49,    49,    54,
      55,    69,    30,     4,    59,    10,     8,    12,    15,    58,
      58,    63,    10,    11,    41,    41,    49,    64,    50,    58,
      10,    50,    12,   128,    83,    50,   117,    69,    42,    57,
      50,    83,   117,    85,    50,    50,    50,    89,    83,    86,
      47,    42,    89,    95,    96,   117,    83,   106,    50,    50,
     222,    58,   262,   225,   106,     7,    15,    12,   163,   231,
      18,    19,   109,    15,     4,   255,     4,   172,   153,   291,
       8,    95,    96,   295,     0,     6,   128,    15,     6,   184,
       8,    40,   304,    14,    15,    10,    41,    15,    47,     4,
      15,     6,   365,     8,    15,    47,   369,     6,   140,    58,
     142,     4,    42,   376,    42,     8,   316,    49,    46,    47,
      10,   163,    50,   395,   219,    40,    47,    12,   170,    47,
     172,    27,    47,   175,   171,    83,    47,    42,   180,    16,
      17,    18,   184,     0,   176,   187,   188,   184,   185,    42,
      14,    15,    39,   190,   212,    40,   104,   105,   106,   107,
     237,   203,     8,    39,    12,   113,   114,   367,   116,    15,
      16,    17,    18,   353,     4,   133,    53,   219,   220,   137,
     212,   218,   224,    47,    15,   133,    14,    15,   283,   137,
      12,   266,    40,     0,   226,    30,    13,   239,    15,   241,
       4,    47,    14,    15,   236,   247,     9,    53,     7,    12,
      10,   253,   300,   301,   272,    49,    47,   480,    40,    47,
      51,   258,    53,    54,    55,    56,    57,    58,    59,    12,
      47,   179,   264,   270,    32,    47,    34,    35,    41,    10,
     272,   283,   404,   285,   281,   407,    16,    17,    18,    19,
      20,    21,    22,    23,    24,   435,     9,   205,   206,    50,
      12,   209,    30,    12,    60,   213,   303,   215,   216,    39,
       0,     0,     1,    10,     3,     0,    44,    45,     0,   227,
     228,    10,     9,    53,    12,   317,   323,     7,    12,    57,
     238,     9,   240,    12,     9,     6,    10,    60,   246,    50,
       0,     9,     9,     9,     9,   253,     0,     4,    10,     5,
      49,     0,   260,   355,     9,    13,   358,     9,   395,     9,
      41,     7,     7,    21,    22,     9,   368,     9,     9,     9,
       5,     7,     9,   365,    50,     0,     0,   369,    50,    12,
      12,    39,    40,   375,   376,   382,     5,     9,   406,    10,
      48,     9,     9,    41,    58,    15,   414,   305,   306,     9,
       7,   309,     7,    50,     0,     9,   314,   315,   463,     9,
       9,    50,    50,    10,   406,     5,   324,    46,     5,    50,
       9,     9,   414,    50,    58,   427,     9,    47,     7,     0,
      10,    51,   434,    53,    54,    55,    56,    57,    58,    59,
     442,    50,     9,   445,     9,    37,     5,   355,    50,     9,
       5,     9,   464,   361,   456,   363,   398,   291,   304,   387,
     295,   463,   370,    49,   122,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,   306,   134,    69,   471,   187,
     334,   220,   168,   538,    -1,   143,    -1,   479,   480,   491,
      -1,   149,   494,    -1,   496,    -1,    -1,    -1,   500,   157,
      -1,   409,    -1,    -1,    -1,    -1,    -1,    -1,   416,    -1,
     168,    -1,   509,   510,    -1,    -1,    -1,    -1,   426,   177,
     178,    -1,    -1,    -1,    -1,    -1,   434,    -1,    -1,    -1,
     532,   533,    -1,   535,    -1,   193,   538,    -1,    -1,    -1,
     448,    -1,    -1,    -1,    -1,    -1,   548,    -1,   140,    -1,
     142,    -1,    -1,   550,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   469,   470,   471,    -1,   473,    -1,    -1,    -1,   477,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   237,
      -1,    -1,    -1,   491,   176,   243,   244,    -1,    -1,    -1,
      -1,    -1,   250,   251,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,   512,   513,    -1,    -1,   266,    -1,
      -1,   519,    -1,    -1,    -1,    -1,    -1,   275,    -1,   277,
     212,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   291,   226,    -1,    -1,   295,    -1,    -1,
      -1,     4,    -1,     6,   236,     8,   304,   305,   306,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    32,
      -1,    -1,   264,    36,    37,    38,    39,    -1,    -1,    42,
     272,    -1,    -1,    -1,    47,   343,    49,    -1,    -1,    -1,
      53,   349,     1,    -1,    -1,     4,     5,     6,    61,     8,
      -1,    -1,    11,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,   317,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,   395,    47,    -1,
      -1,    -1,    -1,   401,    53,    -1,    -1,    -1,   406,    58,
      -1,    -1,    61,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   429,    -1,   365,   432,    -1,    -1,   369,    -1,    -1,
      -1,    -1,    -1,   375,   376,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   471,   406,     0,     1,    -1,    -1,     4,
      -1,     6,   414,     8,    -1,    -1,    11,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    32,    33,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,    -1,
      -1,    -1,    -1,    58,    -1,    -1,    61,    -1,     1,    -1,
      -1,     4,    -1,     6,    -1,     8,     9,   479,   480,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    32,
      33,    -1,    -1,    36,    37,    38,    39,    -1,    41,    42,
      -1,    -1,    -1,    -1,    47,    -1,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,     1,    61,    -1,
       4,    -1,     6,    -1,     8,     9,    -1,    -1,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    41,    42,    -1,
      -1,    -1,    -1,    47,    -1,    49,    -1,    51,    52,    53,
      54,    55,    56,    57,    58,    59,     4,    61,     6,    -1,
       8,    -1,    10,    11,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,    31,    32,    33,    -1,    -1,    36,    37,
      38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,    -1,    -1,
      58,    -1,     4,    61,     6,     7,     8,    -1,    -1,    -1,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    41,
      42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,
      52,    53,    -1,     4,    56,     6,     7,     8,    -1,    61,
      -1,    -1,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,    30,
      31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
      41,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,
      -1,    52,    53,    -1,    -1,    56,    -1,     4,     5,     6,
      61,     8,    -1,    -1,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    -1,    -1,    -1,    36,
      37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,    -1,
      47,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,     4,
      -1,     6,    -1,     8,    61,    10,    11,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    32,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,    -1,
       4,    -1,     6,    -1,     8,     9,    61,    11,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    32,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,
       4,    -1,     6,    -1,     8,     9,    -1,    61,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    52,    53,
      -1,    -1,    56,    -1,     4,     5,     6,    61,     8,    -1,
      -1,    11,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      30,    31,    32,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,
      -1,    -1,    -1,    53,     4,    -1,     6,    -1,     8,     9,
      -1,    61,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,
      -1,    -1,    52,    53,    -1,     4,    56,     6,    -1,     8,
       9,    61,    -1,    -1,    13,    14,    15,    16,    17,    18,
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
      47,    -1,    -1,    -1,    -1,    52,    53,    -1,    -1,    56,
      -1,     4,     5,     6,    61,     8,    -1,    -1,    11,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    30,    31,    32,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,
      53,    -1,     4,    -1,     6,    -1,     8,    -1,    61,    11,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,    31,
      32,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,
      -1,    53,    -1,     4,    -1,     6,    -1,     8,    -1,    61,
      11,    -1,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,    30,
      31,    32,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
      -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,
      -1,    -1,    53,     4,    -1,     6,    -1,     8,    -1,    -1,
      61,    -1,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    -1,    30,
      31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,    -1,
      -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,
      -1,    52,    53,    -1,     4,    56,     6,    -1,     8,    -1,
      61,    -1,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,
      -1,    -1,    52,    53,    -1,     4,    -1,     6,    -1,     8,
      -1,    61,    11,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    53,     4,    -1,     6,    -1,     8,
      -1,    -1,    61,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    41,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    53,     4,    -1,     6,    -1,     8,
      -1,    -1,    61,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    52,    53,     4,    -1,     6,    -1,     8,
       9,    -1,    61,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    53,     4,    -1,     6,    -1,     8,
      -1,    -1,    61,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    52,    53,     4,    -1,     6,    -1,     8,
       9,    -1,    61,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    53,     4,    -1,     6,     7,     8,
      -1,    -1,    61,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    53,     4,    -1,     6,    -1,     8,
       9,    -1,    61,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    53,     4,    -1,     6,     7,     8,
      -1,    -1,    61,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    53,     4,    -1,     6,    -1,     8,
       9,    -1,    61,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    53,     4,    -1,     6,    -1,     8,
      -1,    -1,    61,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      -1,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    38,
      39,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      -1,     4,    -1,     6,    53,     8,    -1,    -1,    -1,    -1,
      13,    -1,    61,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,
       4,    -1,     6,    -1,     8,    -1,    -1,    50,    -1,    13,
      53,    -1,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    -1,    -1,    42,     4,
      -1,     6,    -1,     8,    -1,    -1,    50,    -1,    13,    53,
      -1,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    42,     4,    -1,
       6,    -1,     8,    -1,    -1,    -1,    -1,    13,    53,    -1,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    53
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
     139,   140,   141,     8,    10,    11,    89,    90,   103,   111,
     120,   143,    30,     0,     1,     3,    10,   143,     0,    70,
       7,   128,    49,    72,    10,    49,     8,    52,   103,   104,
     105,   122,     1,    41,    49,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    92,    93,   103,   105,   111,   120,
     123,   124,   103,     6,    14,   128,   128,     6,     8,    86,
      88,   106,   116,    13,   118,   119,   128,   130,    10,    40,
     109,   128,    39,    53,   136,   137,   139,    40,    58,   110,
     128,   103,   111,   112,     6,    52,    53,    58,    60,    93,
      94,    89,    10,   143,     8,    50,    88,    12,    40,     8,
      50,    27,    39,    39,   111,   120,   103,    10,    11,   143,
      89,   143,     0,     4,    67,    67,     0,     7,    30,     4,
     105,   105,   105,    10,   143,    49,    12,   144,   105,    41,
     105,   105,     9,    49,     9,    12,    41,     9,    89,    10,
      12,   144,   122,   128,    50,   122,     9,    52,    56,   105,
     113,   114,   115,    86,   106,    87,    88,    12,     8,    50,
       8,    50,   128,    12,    40,   144,     8,    50,   128,    12,
      40,   144,     4,     8,    13,    50,   127,     5,    89,     7,
      41,   115,     6,     8,   116,   128,    60,    91,   103,   143,
      89,     0,     9,   115,   103,    14,   128,    14,   128,   105,
     103,    89,   143,    10,   143,    11,    90,   103,     0,   143,
       0,    56,    80,    81,    82,    83,    84,    92,   128,    32,
      34,    35,    77,    78,    79,    12,    12,   103,   105,    52,
     105,     7,     9,   105,    12,    41,     8,    88,   105,   105,
       9,   105,   143,    89,    52,   103,   123,     9,   144,   103,
     144,    87,   105,   105,    12,   144,     9,   118,   105,   103,
     105,   103,    10,    12,   128,   128,    10,   105,   103,    12,
      58,   128,    58,   128,   105,     8,   127,   143,    50,   105,
       7,    41,     7,    41,   115,     9,    87,     8,    50,   117,
       8,   128,   136,     0,   143,    88,     9,   128,   128,     9,
     143,     9,    89,     9,   103,     0,    81,     4,    82,     5,
      54,    55,    59,    75,    76,   142,    76,    76,    10,   143,
      84,    32,    49,   105,   107,   108,   128,   107,   105,    49,
       9,   105,     9,   105,     9,   115,    41,     9,     9,   143,
       7,     7,   113,     9,     9,   128,     8,    50,    10,    12,
       9,   128,    12,    58,     8,    50,   127,     9,   105,     5,
     103,     7,    50,     7,   105,    50,   105,     7,    41,    87,
       9,   115,   103,    87,   105,     0,    50,    87,     9,     0,
     143,     0,     1,   112,    12,   118,    77,     5,   128,    47,
      49,    12,   144,    46,   128,   144,   105,    41,     9,    88,
       9,     9,   105,     9,    10,   105,   103,   128,    58,   128,
     105,     8,   127,     9,    50,   103,    50,     7,   103,     7,
      50,     7,   105,     9,     9,   103,     0,     9,     5,    29,
      89,    75,     8,    50,   105,   105,   108,     5,   105,    46,
       5,     9,   105,    87,     9,     9,    10,    58,     9,   105,
     103,   103,    50,    50,   103,    50,     7,     0,   143,   143,
     105,   105,   105,     9,     9,   103,   103,   103,    50,     5,
      89,     5,     9,   103,   143,     5
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    63,    64,    64,    64,    64,    64,    64,    65,    65,
      65,    66,    66,    66,    66,    66,    66,    66,    66,    67,
      67,    67,    67,    67,    68,    68,    68,    68,    68,    69,
      69,    70,    70,    71,    72,    72,    73,    73,    73,    74,
      75,    76,    76,    77,    77,    77,    78,    78,    79,    79,
      80,    80,    81,    81,    81,    81,    81,    82,    82,    83,
      83,    84,    84,    85,    85,    86,    86,    87,    87,    88,
      88,    88,    88,    89,    89,    89,    89,    89,    90,    90,
      91,    91,    91,    92,    92,    92,    92,    92,    92,    92,
      92,    93,    93,    94,    94,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    96,    96,
      96,    97,    97,    97,    98,    98,    98,    99,    99,    99,
     100,   100,   100,   100,   100,   101,   101,   101,   101,   101,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   104,   104,   105,   106,   106,   107,   107,   108,
     108,   108,   108,   108,   108,   108,   109,   109,   109,   109,
     109,   109,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   112,   112,   113,   113,   113,   114,
     114,   115,   116,   117,   117,   117,   118,   118,   118,   118,
     118,   119,   119,   120,   121,   121,   121,   121,   121,   121,
     121,   121,   122,   122,   122,   122,   122,   122,   122,   123,
     123,   124,   124,   124,   125,   126,   126,   127,   127,   127,
     128,   128,   129,   129,   129,   129,   130,   130,   130,   131,
     132,   133,   133,   134,   134,   135,   135,   136,   136,   136,
     136,   137,   137,   137,   137,   138,   138,   139,   139,   140,
     140,   141,   141,   141,   141,   141,   141,   142,   142,   142,
     142,   143,   143,   144,   144
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     3,     2,     1,     1,     2,     0,     5,     6,
       4,     5,     4,     7,     6,     4,     3,     8,     6,     1,
       2,     5,     3,     6,     2,     2,     3,     3,     2,     1,
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
       1,     1,     1,     1,     1,     3,     2,     2,     4,     3,
       1,     1,     1,     1,     1,     1,     1,     5,     4,     3,
       5,     1,     2,     3,     4,     6,     3,     4,     6,     5,
       7,     6,     1,     3,     2,     7,     7,     1,     3,     3,
       4,     4,     5,     1,     3,     3,     1,     3,     4,     3,
       5,     6,     2,     1,     3,     4,     5,     4,     3,     5,
       6,     7,     2,     4,     5,     7,     4,     6,     2,     4,
       5,     7,     4,     6,     0,     1,     2,     2,     1,     1,
       3,     2,     3,     0,     2,     3,     1,     3,     4,     3,
       4,     1,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     3,     2,     3,     5,     4,     3,
       2,     0,     1,     3,     4,     4,     5,     1,     1,     1,
       1,     1,     2,     3,     3,     4,     1,     3,     4,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       2,     1,     1,     2,     2,     1,     2,     0,     1,     1,
       1,     0,     1,     0,     1
  };




#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   239,   239,   240,   241,   242,   243,   244,   252,   255,
     258,   269,   272,   275,   278,   281,   284,   287,   290,   299,
     302,   304,   307,   310,   315,   318,   324,   327,   330,   347,
     349,   354,   355,   359,   364,   365,   369,   370,   371,   375,
     384,   389,   391,   396,   398,   400,   405,   407,   412,   413,
     421,   422,   426,   429,   432,   435,   438,   452,   453,   465,
     466,   470,   471,   480,   481,   485,   487,   492,   493,   497,
     498,   499,   500,   504,   505,   506,   511,   516,   524,   525,
     534,   535,   536,   540,   541,   542,   543,   544,   545,   546,
     547,   551,   552,   556,   558,   563,   566,   569,   573,   576,
     579,   582,   585,   588,   598,   601,   606,   612,   627,   634,
     641,   650,   657,   664,   674,   680,   686,   695,   701,   707,
     716,   721,   726,   731,   736,   744,   749,   755,   760,   765,
     778,   781,   784,   787,   790,   793,   800,   808,   811,   814,
     817,   820,   823,   829,   830,   831,   832,   834,   837,   840,
     843,   846,   847,   849,   851,   854,   857,   860,   866,   869,
     872,   875,   883,   884,   888,   896,   899,   904,   906,   911,
     913,   915,   917,   919,   921,   923,   936,   938,   943,   946,
     949,   954,   960,   962,   964,   966,   968,   970,   972,   974,
     976,   978,   983,   984,   985,   987,   989,   994,  1003,  1005,
    1007,  1009,  1011,  1016,  1028,  1029,  1034,  1035,  1036,  1040,
    1041,  1044,  1046,  1049,  1050,  1052,  1061,  1063,  1065,  1071,
    1073,  1081,  1083,  1087,  1095,  1096,  1097,  1098,  1099,  1100,
    1101,  1102,  1106,  1108,  1110,  1112,  1114,  1116,  1118,  1126,
    1128,  1133,  1135,  1137,  1142,  1147,  1149,  1158,  1159,  1160,
    1168,  1169,  1174,  1176,  1178,  1180,  1184,  1186,  1188,  1199,
    1201,  1204,  1205,  1209,  1210,  1214,  1215,  1219,  1220,  1221,
    1222,  1231,  1232,  1233,  1234,  1238,  1240,  1245,  1247,  1252,
    1254,  1259,  1260,  1261,  1262,  1263,  1264,  1268,  1269,  1270,
    1271,  1283,  1283,  1284,  1284
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
#line 3886 "langutils/sc_parser/src/sc_grammar_parser.cpp"

#line 1286 "langutils/sc_parser/src/sc_grammar.y"

