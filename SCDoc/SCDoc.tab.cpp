
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse         scdocparse
#define yylex           scdoclex
#define yyerror         scdocerror
#define yylval          scdoclval
#define yychar          scdocchar
#define yydebug         scdocdebug
#define yynerrs         scdocnerrs
#define yylloc          scdoclloc

/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "SCDoc.y"

/************************************************************************
*
* Copyright 2012 Jonatan Liljedahl <lijon@kymatica.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SCDoc.h"

//#define YYLEX_PARAM &yylval, &yylloc

int scdocparse();

extern int scdoclineno;
extern char *scdoctext;
extern int scdoc_start_token;
extern FILE *scdocin;
//extern struct YYLTYPE scdoclloc;

//int scdoc_metadata_mode;

static const char * method_type = NULL;

static DocNode * topnode;

void scdocerror(const char *str);

extern void error(const char *fmt, ...);
extern void post(const char *fmt, ...);



/* Line 189 of yacc.c  */
#line 130 "SCDoc.tab.cpp"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     END = 0,
     CLASS = 258,
     TITLE = 259,
     SUMMARY = 260,
     RELATED = 261,
     CATEGORIES = 262,
     REDIRECT = 263,
     CLASSTREE = 264,
     COPYMETHOD = 265,
     KEYWORD = 266,
     PRIVATE = 267,
     SECTION = 268,
     SUBSECTION = 269,
     METHOD = 270,
     ARGUMENT = 271,
     DESCRIPTION = 272,
     CLASSMETHODS = 273,
     INSTANCEMETHODS = 274,
     EXAMPLES = 275,
     RETURNS = 276,
     DISCUSSION = 277,
     LIST = 278,
     TREE = 279,
     NUMBEREDLIST = 280,
     DEFINITIONLIST = 281,
     TABLE = 282,
     FOOTNOTE = 283,
     NOTE = 284,
     WARNING = 285,
     CODE = 286,
     LINK = 287,
     ANCHOR = 288,
     SOFT = 289,
     IMAGE = 290,
     TELETYPE = 291,
     MATH = 292,
     STRONG = 293,
     EMPHASIS = 294,
     CODEBLOCK = 295,
     TELETYPEBLOCK = 296,
     MATHBLOCK = 297,
     TAGSYM = 298,
     BARS = 299,
     HASHES = 300,
     TEXT = 301,
     URL = 302,
     COMMA = 303,
     METHODNAME = 304,
     METHODARGS = 305,
     NEWLINE = 306,
     EMPTYLINES = 307,
     BAD_METHODNAME = 308,
     START_FULL = 309,
     START_PARTIAL = 310,
     START_METADATA = 311
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 50 "SCDoc.y"

    int i;
    const char *id;
    char *str;
    DocNode *doc_node;



/* Line 214 of yacc.c  */
#line 232 "SCDoc.tab.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */

/* Line 264 of yacc.c  */
#line 95 "SCDoc.y"

//int scdoclex (YYSTYPE * yylval_param, struct YYLTYPE * yylloc_param );
int scdoclex (void);


/* Line 264 of yacc.c  */
#line 263 "SCDoc.tab.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  67
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   430

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  57
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  53
/* YYNRULES -- Number of rules.  */
#define YYNRULES  131
/* YYNRULES -- Number of states.  */
#define YYNSTATES  190

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   311

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
      55,    56
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    12,    15,    19,    22,    24,
      28,    32,    36,    38,    40,    42,    44,    46,    48,    50,
      52,    54,    55,    58,    60,    62,    63,    69,    72,    74,
      75,    78,    80,    82,    87,    89,    90,    93,    95,    97,
     103,   107,   111,   112,   114,   116,   120,   122,   127,   129,
     130,   132,   133,   136,   138,   143,   147,   150,   151,   154,
     155,   157,   159,   162,   165,   167,   170,   172,   176,   180,
     184,   188,   192,   196,   200,   202,   206,   209,   211,   213,
     215,   219,   223,   225,   227,   229,   231,   233,   235,   237,
     239,   241,   243,   245,   247,   249,   251,   253,   255,   257,
     261,   264,   267,   270,   272,   276,   278,   282,   285,   289,
     292,   294,   296,   298,   300,   302,   305,   307,   310,   312,
     314,   316,   318,   320,   322,   325,   327,   330,   333,   335,
     337,   341
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      58,     0,    -1,    59,    -1,    59,     1,    -1,    54,    60,
      64,    -1,    55,    65,    -1,    56,    60,    64,    -1,    60,
      61,    -1,    61,    -1,    62,   104,   105,    -1,     7,   109,
     105,    -1,     6,   109,   105,    -1,     3,    -1,     4,    -1,
       5,    -1,     8,    -1,    18,    -1,    19,    -1,    17,    -1,
      20,    -1,    65,    -1,    -1,    65,    66,    -1,    66,    -1,
      72,    -1,    -1,    13,    67,   104,   105,    68,    -1,    63,
      68,    -1,    69,    -1,    -1,    69,    70,    -1,    70,    -1,
      72,    -1,    14,   104,   105,    71,    -1,    72,    -1,    -1,
      72,    73,    -1,    73,    -1,    84,    -1,    15,    76,    74,
     105,    77,    -1,    10,   103,   105,    -1,    12,   109,   105,
      -1,    -1,    50,    -1,    49,    -1,    76,    48,    75,    -1,
      75,    -1,    78,    79,    82,    83,    -1,    84,    -1,    -1,
      80,    -1,    -1,    80,    81,    -1,    81,    -1,    16,   103,
     105,    78,    -1,    16,   105,    84,    -1,    21,    84,    -1,
      -1,    22,    84,    -1,    -1,    85,    -1,    86,    -1,    86,
      87,    -1,    85,    87,    -1,    87,    -1,    85,    88,    -1,
      88,    -1,    93,    84,    43,    -1,    92,    94,    43,    -1,
      27,    96,    43,    -1,    26,   100,    43,    -1,    91,   107,
      43,    -1,     9,   103,   105,    -1,    11,   109,   105,    -1,
      52,    -1,    35,   104,    43,    -1,    88,    89,    -1,    89,
      -1,   102,    -1,    47,    -1,    90,   103,    43,    -1,    28,
      84,    43,    -1,    51,    -1,    32,    -1,    38,    -1,    34,
      -1,    39,    -1,    31,    -1,    36,    -1,    37,    -1,    33,
      -1,    40,    -1,    41,    -1,    42,    -1,    23,    -1,    24,
      -1,    25,    -1,    30,    -1,    29,    -1,    94,    45,    84,
      -1,    45,    84,    -1,    45,    97,    -1,    96,    95,    -1,
      95,    -1,    97,    44,    78,    -1,    78,    -1,    98,    45,
      84,    -1,    45,    84,    -1,    98,    44,    78,    -1,   100,
      99,    -1,    99,    -1,   102,    -1,    47,    -1,    46,    -1,
      48,    -1,   103,   102,    -1,   102,    -1,   104,   101,    -1,
     101,    -1,    51,    -1,    52,    -1,     0,    -1,   102,    -1,
     105,    -1,   107,   106,    -1,   106,    -1,   108,    46,    -1,
     108,    47,    -1,    46,    -1,    47,    -1,   109,    48,   108,
      -1,   108,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   102,   102,   103,   106,   112,   116,   124,   125,   128,
     129,   130,   133,   134,   135,   136,   139,   140,   141,   142,
     145,   146,   149,   150,   151,   154,   154,   155,   158,   159,
     162,   163,   164,   167,   170,   171,   174,   175,   176,   179,
     186,   190,   193,   194,   205,   216,   217,   220,   229,   230,
     233,   234,   237,   238,   241,   242,   245,   246,   249,   250,
     258,   259,   262,   263,   264,   267,   268,   271,   272,   273,
     274,   275,   276,   277,   280,   281,   284,   285,   288,   289,
     290,   291,   292,   295,   296,   297,   298,   299,   300,   301,
     302,   305,   306,   307,   310,   311,   312,   315,   316,   319,
     320,   323,   326,   327,   330,   331,   334,   335,   338,   345,
     346,   349,   350,   353,   354,   357,   358,   361,   362,   365,
     366,   367,   370,   371,   374,   375,   378,   379,   380,   381,
     384,   385
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "$undefined", "CLASS", "TITLE", "SUMMARY",
  "RELATED", "CATEGORIES", "REDIRECT", "CLASSTREE", "COPYMETHOD",
  "KEYWORD", "PRIVATE", "SECTION", "SUBSECTION", "METHOD", "ARGUMENT",
  "DESCRIPTION", "CLASSMETHODS", "INSTANCEMETHODS", "EXAMPLES", "RETURNS",
  "DISCUSSION", "LIST", "TREE", "NUMBEREDLIST", "DEFINITIONLIST", "TABLE",
  "FOOTNOTE", "NOTE", "WARNING", "CODE", "LINK", "ANCHOR", "SOFT", "IMAGE",
  "TELETYPE", "MATH", "STRONG", "EMPHASIS", "\"CODE block\"",
  "\"TELETYPE block\"", "\"MATH block\"", "\"::\"", "\"||\"", "\"##\"",
  "\"text\"", "URL", "COMMA", "\"method name\"", "\"arguments string\"",
  "\"newline\"", "\"empty lines\"", "\"bad method name\"", "START_FULL",
  "START_PARTIAL", "START_METADATA", "$accept", "start", "document",
  "dochead", "headline", "headtag", "sectiontag", "optsections",
  "sections", "section", "$@1", "optsubsections", "subsections",
  "subsection", "optsubsubsections", "subsubsections", "subsubsection",
  "optMETHODARGS", "methodname", "methnames", "methodbody", "optbody",
  "optargs", "args", "arg", "optreturns", "optdiscussion", "body",
  "blockA", "blockB", "bodyelem", "prose", "proseelem", "inlinetag",
  "blocktag", "listtag", "rangetag", "listbody", "tablerow", "tablebody",
  "tablecells", "defterms", "deflistrow", "deflistbody", "anywordurl",
  "anyword", "words", "words2", "eol", "anywordnl", "wordsnl",
  "nocommawords", "commalist", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    57,    58,    58,    59,    59,    59,    60,    60,    61,
      61,    61,    62,    62,    62,    62,    63,    63,    63,    63,
      64,    64,    65,    65,    65,    67,    66,    66,    68,    68,
      69,    69,    69,    70,    71,    71,    72,    72,    72,    73,
      73,    73,    74,    74,    75,    76,    76,    77,    78,    78,
      79,    79,    80,    80,    81,    81,    82,    82,    83,    83,
      84,    84,    85,    85,    85,    86,    86,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    88,    88,    89,    89,
      89,    89,    89,    90,    90,    90,    90,    90,    90,    90,
      90,    91,    91,    91,    92,    92,    92,    93,    93,    94,
      94,    95,    96,    96,    97,    97,    98,    98,    99,   100,
     100,   101,   101,   102,   102,   103,   103,   104,   104,   105,
     105,   105,   106,   106,   107,   107,   108,   108,   108,   108,
     109,   109
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     3,     2,     3,     2,     1,     3,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     2,     1,     1,     0,     5,     2,     1,     0,
       2,     1,     1,     4,     1,     0,     2,     1,     1,     5,
       3,     3,     0,     1,     1,     3,     1,     4,     1,     0,
       1,     0,     2,     1,     4,     3,     2,     0,     2,     0,
       1,     1,     2,     2,     1,     2,     1,     3,     3,     3,
       3,     3,     3,     3,     1,     3,     2,     1,     1,     1,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       2,     2,     2,     1,     3,     1,     3,     2,     3,     2,
       1,     1,     1,     1,     1,     2,     1,     2,     1,     1,
       1,     1,     1,     1,     2,     1,     2,     2,     1,     1,
       3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,    12,    13,    14,     0,
       0,    15,    21,     8,     0,     0,     0,     0,     0,    25,
       0,    18,    16,    17,    19,    94,    95,    96,     0,     0,
       0,    98,    97,    87,    83,    90,    85,     0,    88,    89,
      84,    86,    91,    92,    93,   113,    79,   114,    82,    74,
      29,     5,    23,    24,    37,    38,    60,    61,    64,    66,
      77,     0,     0,     0,     0,    78,    21,     1,     3,   128,
     129,   131,     0,     0,     7,     4,    20,   112,   118,   111,
       0,   116,     0,     0,     0,     0,     0,    44,    46,    42,
       0,     0,   110,     0,    49,   103,     0,     0,     0,     0,
      27,    28,    31,    32,    22,    36,    63,    65,    62,    76,
       0,   121,   119,   120,   122,   123,   125,     0,     0,     0,
       0,     6,   126,   127,     0,    11,    10,   117,     9,   115,
      72,    40,    73,    41,     0,     0,    43,     0,   107,    49,
       0,    70,   109,   105,    48,   101,    69,   102,    81,    75,
       0,    30,    80,    71,   124,   100,    68,     0,    67,   130,
      29,    45,    49,   108,   106,    49,    35,    99,    26,    39,
      51,   104,    33,    34,     0,    57,    50,    53,     0,     0,
       0,    59,    52,    49,    55,    56,     0,    47,    54,    58
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,    12,    13,    14,    50,    75,    76,    52,
      86,   100,   101,   102,   172,    53,    54,   137,    88,    89,
     169,   143,   175,   176,   177,   181,   187,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,   119,    95,    96,
     145,    91,    92,    93,    78,    65,    82,    80,   115,   116,
     117,    71,    72
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -74
static const yytype_int16 yypact[] =
{
     -10,   136,   206,   136,    22,    33,   -74,   -74,   -74,     4,
       4,   -74,   162,   -74,    85,   -31,   -31,     4,     4,   -74,
     -35,   -74,   -74,   -74,   -74,   -74,   -74,   -74,   -19,    -3,
     328,   -74,   -74,   -74,   -74,   -74,   -74,    85,   -74,   -74,
     -74,   -74,   -74,   -74,   -74,   -74,   -74,   -74,   -74,   -74,
     250,    39,   -74,   109,   -74,   -74,   328,   362,   -74,   379,
     -74,   -31,    53,     9,   328,   -74,   162,   -74,   -74,   -74,
     -74,    61,    30,    30,   -74,   -74,    39,   -74,   -74,   -74,
      41,   -74,    53,    53,    30,    30,    85,   -74,   -74,   -21,
     328,   104,   -74,    72,   328,   -74,    83,    47,    -8,    85,
     -74,    51,   -74,   109,   -74,   -74,   -74,   379,   -74,   -74,
      54,   -74,   -74,   -74,   -74,   -74,   -74,    28,   328,   102,
      55,   -74,   -74,   -74,     4,   -74,   -74,   -74,   -74,   -74,
     -74,   -74,   -74,   -74,    41,   -35,   -74,    43,   -74,   328,
     328,   -74,   -74,   -74,   -74,    67,   -74,   -74,   -74,   -74,
      41,   -74,   -74,   -74,   -74,   -74,   -74,   328,   -74,    61,
     250,   -74,   328,   -74,   -74,   328,   294,   -74,   -74,   -74,
     107,   -74,   -74,   109,    53,    97,   107,   -74,    53,   328,
     328,   112,   -74,   328,   -74,   -74,   328,   -74,   -74,   -74
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -74,   -74,   -74,   126,    -2,   -74,   -74,    71,   144,   -40,
     -74,    -6,   -74,    56,   -74,   -44,   -48,   -74,    20,   -74,
     -74,   -56,   -74,   -74,   -18,   -74,   -74,   -27,   -74,   -74,
      94,   105,   -47,   -74,   -74,   -74,   -74,   -74,    80,   -74,
     -74,   -74,    70,   -74,   -73,   -14,   -12,   -24,   -64,    66,
     -74,    60,    14
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -3
static const yytype_int16 yytable[] =
{
      79,    81,    81,    97,    83,   105,   103,   127,   125,   126,
      74,   104,   109,    98,    87,    45,   128,    47,   130,   131,
     132,   133,    67,    79,    73,   127,    90,   135,   111,   136,
     111,    84,    85,    -2,    68,   149,   104,   120,    45,    77,
      47,   111,    94,   111,     1,     2,     3,    81,   114,   110,
      69,    70,    19,   111,   118,   105,    21,    22,    23,    24,
     109,   127,   134,   138,    74,    99,    79,   144,   129,   129,
     160,   153,    79,   162,    45,   150,    47,   127,   124,   112,
     113,   112,   113,   163,    79,    79,   166,    45,    77,    47,
     148,   155,   112,   113,   112,   113,   129,   152,   158,    45,
      45,    47,    47,   114,   112,   113,   170,   122,   123,   171,
     179,   165,   144,   164,   183,   141,   103,    90,   180,    16,
      79,    18,   173,   174,    20,   105,   146,   188,    94,    66,
     167,    45,    77,    47,   186,   144,    79,   121,   144,     6,
       7,     8,     9,    10,    11,   156,    51,   157,   139,   140,
     106,   108,   184,   185,   168,   161,   144,   151,   182,   189,
      81,   107,   178,   142,   129,     6,     7,     8,     9,    10,
      11,    15,    16,    17,    18,    19,   147,    20,     0,    21,
      22,    23,    24,   154,   159,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,     0,     0,     0,    45,    46,
      47,     0,     0,    48,    49,    15,    16,    17,    18,    19,
       0,    20,     0,    21,    22,    23,    24,     0,     0,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,     0,
       0,     0,    45,    46,    47,     0,     0,    48,    49,    15,
      16,    17,    18,     0,    99,    20,     0,     0,     0,     0,
       0,     0,     0,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,     0,     0,     0,    45,    46,    47,     0,
       0,    48,    49,    15,    16,    17,    18,     0,     0,    20,
       0,     0,     0,     0,     0,     0,     0,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    15,     0,    17,
      45,    46,    47,     0,     0,    48,    49,     0,     0,     0,
       0,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    15,     0,    17,    45,    46,    47,     0,     0,    48,
      49,     0,     0,     0,     0,    25,    26,    27,    28,    29,
       0,    31,    32,     0,     0,     0,     0,    37,     0,     0,
       0,     0,    42,    43,    44,     0,     0,    30,     0,     0,
      33,    34,    35,    36,    49,    38,    39,    40,    41,     0,
       0,     0,     0,     0,     0,    45,    46,    47,     0,     0,
      48
};

static const yytype_int16 yycheck[] =
{
      14,    15,    16,    30,    16,    53,    50,    80,    72,    73,
      12,    51,    59,    37,    49,    46,    80,    48,    82,    83,
      84,    85,     0,    37,    10,    98,    45,    48,     0,    50,
       0,    17,    18,     0,     1,    43,    76,    64,    46,    47,
      48,     0,    45,     0,    54,    55,    56,    61,    62,    61,
      46,    47,    13,     0,    45,   103,    17,    18,    19,    20,
     107,   134,    86,    90,    66,    14,    80,    94,    82,    83,
     134,    43,    86,   137,    46,    99,    48,   150,    48,    51,
      52,    51,    52,   139,    98,    99,   150,    46,    47,    48,
      43,   118,    51,    52,    51,    52,   110,    43,    43,    46,
      46,    48,    48,   117,    51,    52,   162,    46,    47,   165,
     174,    44,   139,   140,   178,    43,   160,    45,    21,    10,
     134,    12,   166,    16,    15,   173,    43,   183,    45,     3,
     157,    46,    47,    48,    22,   162,   150,    66,   165,     3,
       4,     5,     6,     7,     8,    43,     2,    45,    44,    45,
      56,    57,   179,   180,   160,   135,   183,   101,   176,   186,
     174,    56,   174,    93,   178,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    96,    15,    -1,    17,
      18,    19,    20,   117,   124,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    -1,    -1,    -1,    46,    47,
      48,    -1,    -1,    51,    52,     9,    10,    11,    12,    13,
      -1,    15,    -1,    17,    18,    19,    20,    -1,    -1,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    -1,
      -1,    -1,    46,    47,    48,    -1,    -1,    51,    52,     9,
      10,    11,    12,    -1,    14,    15,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    -1,    -1,    -1,    46,    47,    48,    -1,
      -1,    51,    52,     9,    10,    11,    12,    -1,    -1,    15,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     9,    -1,    11,
      46,    47,    48,    -1,    -1,    51,    52,    -1,    -1,    -1,
      -1,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     9,    -1,    11,    46,    47,    48,    -1,    -1,    51,
      52,    -1,    -1,    -1,    -1,    23,    24,    25,    26,    27,
      -1,    29,    30,    -1,    -1,    -1,    -1,    35,    -1,    -1,
      -1,    -1,    40,    41,    42,    -1,    -1,    28,    -1,    -1,
      31,    32,    33,    34,    52,    36,    37,    38,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    -1,    -1,
      51
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    54,    55,    56,    58,    59,     3,     4,     5,     6,
       7,     8,    60,    61,    62,     9,    10,    11,    12,    13,
      15,    17,    18,    19,    20,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    46,    47,    48,    51,    52,
      63,    65,    66,    72,    73,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,   102,    60,     0,     1,    46,
      47,   108,   109,   109,    61,    64,    65,    47,   101,   102,
     104,   102,   103,   103,   109,   109,    67,    49,    75,    76,
      45,    98,    99,   100,    45,    95,    96,    84,   104,    14,
      68,    69,    70,    72,    66,    73,    87,    88,    87,    89,
     103,     0,    51,    52,   102,   105,   106,   107,    45,    94,
      84,    64,    46,    47,    48,   105,   105,   101,   105,   102,
     105,   105,   105,   105,   104,    48,    50,    74,    84,    44,
      45,    43,    99,    78,    84,    97,    43,    95,    43,    43,
     104,    70,    43,    43,   106,    84,    43,    45,    43,   108,
     105,    75,   105,    78,    84,    44,   105,    84,    68,    77,
      78,    78,    71,    72,    16,    79,    80,    81,   103,   105,
      21,    82,    81,   105,    84,    84,    22,    83,    78,    84
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 46: /* "\"text\"" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1420 "SCDoc.tab.cpp"
	break;
      case 47: /* "URL" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1429 "SCDoc.tab.cpp"
	break;
      case 48: /* "COMMA" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1438 "SCDoc.tab.cpp"
	break;
      case 49: /* "\"method name\"" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1447 "SCDoc.tab.cpp"
	break;
      case 50: /* "\"arguments string\"" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1456 "SCDoc.tab.cpp"
	break;
      case 59: /* "document" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1465 "SCDoc.tab.cpp"
	break;
      case 60: /* "dochead" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1474 "SCDoc.tab.cpp"
	break;
      case 61: /* "headline" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1483 "SCDoc.tab.cpp"
	break;
      case 64: /* "optsections" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1492 "SCDoc.tab.cpp"
	break;
      case 65: /* "sections" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1501 "SCDoc.tab.cpp"
	break;
      case 66: /* "section" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1510 "SCDoc.tab.cpp"
	break;
      case 68: /* "optsubsections" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1519 "SCDoc.tab.cpp"
	break;
      case 69: /* "subsections" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1528 "SCDoc.tab.cpp"
	break;
      case 70: /* "subsection" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1537 "SCDoc.tab.cpp"
	break;
      case 71: /* "optsubsubsections" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1546 "SCDoc.tab.cpp"
	break;
      case 72: /* "subsubsections" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1555 "SCDoc.tab.cpp"
	break;
      case 73: /* "subsubsection" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1564 "SCDoc.tab.cpp"
	break;
      case 74: /* "optMETHODARGS" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1573 "SCDoc.tab.cpp"
	break;
      case 75: /* "methodname" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1582 "SCDoc.tab.cpp"
	break;
      case 76: /* "methnames" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1591 "SCDoc.tab.cpp"
	break;
      case 77: /* "methodbody" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1600 "SCDoc.tab.cpp"
	break;
      case 78: /* "optbody" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1609 "SCDoc.tab.cpp"
	break;
      case 79: /* "optargs" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1618 "SCDoc.tab.cpp"
	break;
      case 80: /* "args" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1627 "SCDoc.tab.cpp"
	break;
      case 81: /* "arg" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1636 "SCDoc.tab.cpp"
	break;
      case 82: /* "optreturns" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1645 "SCDoc.tab.cpp"
	break;
      case 83: /* "optdiscussion" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1654 "SCDoc.tab.cpp"
	break;
      case 84: /* "body" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1663 "SCDoc.tab.cpp"
	break;
      case 85: /* "blockA" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1672 "SCDoc.tab.cpp"
	break;
      case 86: /* "blockB" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1681 "SCDoc.tab.cpp"
	break;
      case 87: /* "bodyelem" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1690 "SCDoc.tab.cpp"
	break;
      case 88: /* "prose" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1699 "SCDoc.tab.cpp"
	break;
      case 89: /* "proseelem" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1708 "SCDoc.tab.cpp"
	break;
      case 94: /* "listbody" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1717 "SCDoc.tab.cpp"
	break;
      case 95: /* "tablerow" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1726 "SCDoc.tab.cpp"
	break;
      case 96: /* "tablebody" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1735 "SCDoc.tab.cpp"
	break;
      case 97: /* "tablecells" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1744 "SCDoc.tab.cpp"
	break;
      case 98: /* "defterms" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1753 "SCDoc.tab.cpp"
	break;
      case 99: /* "deflistrow" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1762 "SCDoc.tab.cpp"
	break;
      case 100: /* "deflistbody" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1771 "SCDoc.tab.cpp"
	break;
      case 101: /* "anywordurl" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1780 "SCDoc.tab.cpp"
	break;
      case 102: /* "anyword" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1789 "SCDoc.tab.cpp"
	break;
      case 103: /* "words" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1798 "SCDoc.tab.cpp"
	break;
      case 104: /* "words2" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1807 "SCDoc.tab.cpp"
	break;
      case 106: /* "anywordnl" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1816 "SCDoc.tab.cpp"
	break;
      case 107: /* "wordsnl" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1825 "SCDoc.tab.cpp"
	break;
      case 108: /* "nocommawords" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1834 "SCDoc.tab.cpp"
	break;
      case 109: /* "commalist" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1843 "SCDoc.tab.cpp"
	break;

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 102 "SCDoc.y"
    { topnode = (yyvsp[(1) - (1)].doc_node); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 103 "SCDoc.y"
    { topnode = NULL; doc_node_free_tree((yyvsp[(1) - (2)].doc_node)); ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 107 "SCDoc.y"
    {
        (yyval.doc_node) = doc_node_create("DOCUMENT");
        doc_node_add_child((yyval.doc_node), (yyvsp[(2) - (3)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(3) - (3)].doc_node));
    ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 113 "SCDoc.y"
    {
        (yyval.doc_node) = doc_node_make_take_children("BODY",NULL,(yyvsp[(2) - (2)].doc_node));
    ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 117 "SCDoc.y"
    {
        (yyval.doc_node) = doc_node_create("DOCUMENT");
        doc_node_add_child((yyval.doc_node), (yyvsp[(2) - (3)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(3) - (3)].doc_node));
    ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 124 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 125 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("HEADER",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 128 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make((yyvsp[(1) - (3)].id),(yyvsp[(2) - (3)].str),NULL); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 129 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("CATEGORIES",NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 130 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("RELATED",NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 133 "SCDoc.y"
    { (yyval.id) = "TITLE"; ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 134 "SCDoc.y"
    { (yyval.id) = "TITLE"; ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 135 "SCDoc.y"
    { (yyval.id) = "SUMMARY"; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 136 "SCDoc.y"
    { (yyval.id) = "REDIRECT"; ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 139 "SCDoc.y"
    { (yyval.id) = "CLASSMETHODS"; method_type = "CMETHOD"; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 140 "SCDoc.y"
    { (yyval.id) = "INSTANCEMETHODS"; method_type = "IMETHOD"; ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 141 "SCDoc.y"
    { (yyval.id) = "DESCRIPTION"; method_type = "METHOD"; ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 142 "SCDoc.y"
    { (yyval.id) = "EXAMPLES"; method_type = "METHOD"; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 146 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("BODY",NULL,NULL); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 149 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 150 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("BODY",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 151 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("BODY",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 154 "SCDoc.y"
    { method_type = "METHOD"; ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 154 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("SECTION",(yyvsp[(3) - (5)].str),(yyvsp[(5) - (5)].doc_node)); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 155 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children((yyvsp[(1) - (2)].id), NULL,(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 159 "SCDoc.y"
    { (yyval.doc_node) = NULL; ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 162 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 163 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(SUBSECTIONS)",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 167 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("SUBSECTION", (yyvsp[(2) - (4)].str), (yyvsp[(4) - (4)].doc_node)); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 171 "SCDoc.y"
    { (yyval.doc_node) = NULL; ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 174 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 175 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(SUBSUBSECTIONS)",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 176 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("(SUBSUBSECTIONS)",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 180 "SCDoc.y"
    {
        (yyvsp[(2) - (5)].doc_node)->id = "METHODNAMES";
        (yyval.doc_node) = doc_node_make(method_type,(yyvsp[(3) - (5)].str),(yyvsp[(2) - (5)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(5) - (5)].doc_node));
//        doc_node_add_child($2, $3);
    ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 186 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make(
                method_type=="CMETHOD"?"CCOPYMETHOD":(method_type=="IMETHOD"?"ICOPYMETHOD":"COPYMETHOD"),
                (yyvsp[(2) - (3)].str),NULL
                ); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 190 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children(method_type=="CMETHOD"?"CPRIVATE":"IPRIVATE",NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 193 "SCDoc.y"
    { (yyval.str) = NULL; ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 195 "SCDoc.y"
    {
//        $$ = doc_node_make("ARGSTRING",$1,NULL);
        (yyval.str) = (yyvsp[(1) - (1)].str);
        if(method_type!="METHOD") {
            yyerror("METHOD argument string is not allowed inside CLASSMETHODS or INSTANCEMETHODS");
            YYERROR;
        }
    ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 206 "SCDoc.y"
    {
        char *p = (yyvsp[(1) - (1)].str)+strlen((yyvsp[(1) - (1)].str))-1;
        if(*p=='_') {
            post("WARNING: SCDoc: In %s\n  Property setter %s should be documented without underscore.\n", scdoc_current_file, (yyvsp[(1) - (1)].str));
            *p = '\0';
        };
        (yyval.str) = (yyvsp[(1) - (1)].str);
    ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 216 "SCDoc.y"
    { free((yyvsp[(2) - (3)].str)); (yyvsp[(2) - (3)].str) = NULL; (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node), doc_node_make("STRING",(yyvsp[(3) - (3)].str),NULL)); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 217 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(METHODNAMES)",NULL,doc_node_make("STRING",(yyvsp[(1) - (1)].str),NULL)); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 221 "SCDoc.y"
    {
        (yyval.doc_node) = doc_node_make_take_children("METHODBODY",NULL,(yyvsp[(1) - (4)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(2) - (4)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(3) - (4)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(4) - (4)].doc_node));
    ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 230 "SCDoc.y"
    { (yyval.doc_node) = NULL; ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 234 "SCDoc.y"
    { (yyval.doc_node) = NULL; ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 237 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 238 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("ARGUMENTS",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 241 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("ARGUMENT", (yyvsp[(2) - (4)].str), (yyvsp[(4) - (4)].doc_node)); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 242 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("ARGUMENT", NULL, (yyvsp[(3) - (3)].doc_node)); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 245 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("RETURNS",NULL,(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 246 "SCDoc.y"
    { (yyval.doc_node) = NULL; ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 249 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("DISCUSSION",NULL,(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 250 "SCDoc.y"
    { (yyval.doc_node) = NULL; ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 262 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 263 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 264 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(SECTIONBODY)",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 267 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 268 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(SECTIONBODY)",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 271 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children((yyvsp[(1) - (3)].id),NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 272 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children((yyvsp[(1) - (3)].id),NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 273 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("TABLE",NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 274 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("DEFINITIONLIST",NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 275 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make((yyvsp[(1) - (3)].id),(yyvsp[(2) - (3)].str),NULL); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 276 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("CLASSTREE",(yyvsp[(2) - (3)].str),NULL); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 277 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("KEYWORD",NULL,(yyvsp[(2) - (3)].doc_node));
//            printf("keyword '%s'\n",$2->children[0]->text);
        ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 280 "SCDoc.y"
    { (yyval.doc_node) = NULL; ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 281 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("IMAGE",(yyvsp[(2) - (3)].str),NULL); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 284 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node), (yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 285 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("PROSE",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 288 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("TEXT",(yyvsp[(1) - (1)].str),NULL); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 289 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("LINK",(yyvsp[(1) - (1)].str),NULL); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 290 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make((yyvsp[(1) - (3)].id),(yyvsp[(2) - (3)].str),NULL); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 291 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("FOOTNOTE",NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 292 "SCDoc.y"
    { (yyval.doc_node) = doc_node_create("NL"); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 295 "SCDoc.y"
    { (yyval.id) = "LINK"; ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 296 "SCDoc.y"
    { (yyval.id) = "STRONG"; ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 297 "SCDoc.y"
    { (yyval.id) = "SOFT"; ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 298 "SCDoc.y"
    { (yyval.id) = "EMPHASIS"; ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 299 "SCDoc.y"
    { (yyval.id) = "CODE"; ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 300 "SCDoc.y"
    { (yyval.id) = "TELETYPE"; ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 301 "SCDoc.y"
    { (yyval.id) = "MATH"; ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 302 "SCDoc.y"
    { (yyval.id) = "ANCHOR"; ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 305 "SCDoc.y"
    { (yyval.id) = "CODEBLOCK"; ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 306 "SCDoc.y"
    { (yyval.id) = "TELETYPEBLOCK"; ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 307 "SCDoc.y"
    { (yyval.id) = "MATHBLOCK"; ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 310 "SCDoc.y"
    { (yyval.id) = "LIST"; ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 311 "SCDoc.y"
    { (yyval.id) = "TREE"; ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 312 "SCDoc.y"
    { (yyval.id) = "NUMBEREDLIST"; ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 315 "SCDoc.y"
    { (yyval.id) = "WARNING"; ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 316 "SCDoc.y"
    { (yyval.id) = "NOTE"; ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 319 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node), doc_node_make_take_children("ITEM",NULL,(yyvsp[(3) - (3)].doc_node))); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 320 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(LISTBODY)",NULL, doc_node_make_take_children("ITEM",NULL,(yyvsp[(2) - (2)].doc_node))); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 323 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("TABROW",NULL,(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 326 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 327 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(TABLEBODY)",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 330 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node), doc_node_make_take_children("TABCOL",NULL,(yyvsp[(3) - (3)].doc_node))); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 331 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(TABLECELLS)",NULL, doc_node_make_take_children("TABCOL",NULL,(yyvsp[(1) - (1)].doc_node))); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 334 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node),doc_node_make_take_children("TERM",NULL,(yyvsp[(3) - (3)].doc_node))); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 335 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(TERMS)",NULL,doc_node_make_take_children("TERM",NULL,(yyvsp[(2) - (2)].doc_node))); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 339 "SCDoc.y"
    {
        (yyval.doc_node) = doc_node_make_take_children("DEFLISTITEM", NULL, (yyvsp[(1) - (3)].doc_node));
        doc_node_add_child((yyval.doc_node), doc_node_make_take_children("DEFINITION", NULL, (yyvsp[(3) - (3)].doc_node)));
    ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 345 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 346 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(DEFLISTBODY)",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 357 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 361 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 371 "SCDoc.y"
    { (yyval.str) = strdup("\n"); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 374 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 378 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 379 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 384 "SCDoc.y"
    { free((yyvsp[(2) - (3)].str)); (yyvsp[(2) - (3)].str)=NULL; (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node),doc_node_make("STRING",(yyvsp[(3) - (3)].str),NULL)); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 385 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(COMMALIST)",NULL,doc_node_make("STRING",(yyvsp[(1) - (1)].str),NULL)); ;}
    break;



/* Line 1455 of yacc.c  */
#line 2980 "SCDoc.tab.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 388 "SCDoc.y"


DocNode * scdoc_parse_run(int mode) {
    int modes[] = {START_FULL, START_PARTIAL, START_METADATA};
    if(mode<0 || mode>=sizeof(modes)) {
        error("scdoc_parse_run(): unknown mode: %d\n",mode);
    }
    scdoc_start_token = modes[mode];
/*    scdoc_start_token = START_FULL;
    scdoc_metadata_mode = 0;
    if(mode==SCDOC_PARSE_PARTIAL) {
        scdoc_start_token = START_PARTIAL;
    } else
    if(mode==SCDOC_PARSE_METADATA) {
        scdoc_metadata_mode = 1;
    }*/
    topnode = NULL;
    method_type = "METHOD";
    if(scdocparse()!=0) {
        return NULL;
    }
    return topnode;
}

void scdocerror(const char *str)
{
    error("In %s:\n  At line %d: %s\n\n",scdoc_current_file,scdoclineno,str);

/*  FIXME: this does not work well, since the reported linenumber is often *after* the actual error line
    fseek(scdocin, 0, SEEK_SET);
    int line = 1;
    char buf[256],*txt;
    while(line!=scdoclineno && !feof(scdocin)) {
        int c = fgetc(scdocin);
        if(c=='\n') line++;
    }
    txt = fgets(buf, 256, scdocin);
    if(txt)
        fprintf(stderr,"  %s\n-------------------\n", txt);
*/
}


