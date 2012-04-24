
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
#define YYFINAL  70
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   398

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  57
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  54
/* YYNRULES -- Number of rules.  */
#define YYNRULES  132
/* YYNRULES -- Number of states.  */
#define YYNSTATES  191

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
       0,     0,     3,     5,     8,    13,    16,    20,    22,    23,
      26,    28,    32,    36,    40,    42,    44,    46,    48,    50,
      52,    54,    56,    58,    59,    62,    64,    66,    67,    73,
      76,    78,    79,    82,    84,    86,    91,    93,    94,    97,
      99,   101,   107,   111,   115,   116,   118,   120,   124,   126,
     131,   133,   134,   136,   137,   140,   142,   147,   151,   154,
     155,   158,   159,   161,   163,   166,   169,   171,   174,   176,
     180,   184,   188,   192,   196,   200,   204,   206,   210,   213,
     215,   217,   219,   223,   227,   229,   231,   233,   235,   237,
     239,   241,   243,   245,   247,   249,   251,   253,   255,   257,
     259,   261,   265,   268,   271,   274,   276,   280,   282,   286,
     289,   293,   296,   298,   300,   302,   304,   306,   309,   311,
     314,   316,   318,   320,   322,   324,   327,   329,   332,   335,
     337,   339,   343
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      58,     0,    -1,    59,    -1,    59,     1,    -1,    54,    60,
      61,    65,    -1,    55,    66,    -1,    56,    61,    65,    -1,
     106,    -1,    -1,    61,    62,    -1,    62,    -1,    63,   105,
     106,    -1,     7,   110,   106,    -1,     6,   110,   106,    -1,
       3,    -1,     4,    -1,     5,    -1,     8,    -1,    18,    -1,
      19,    -1,    17,    -1,    20,    -1,    66,    -1,    -1,    66,
      67,    -1,    67,    -1,    73,    -1,    -1,    13,    68,   105,
     106,    69,    -1,    64,    69,    -1,    70,    -1,    -1,    70,
      71,    -1,    71,    -1,    73,    -1,    14,   105,   106,    72,
      -1,    73,    -1,    -1,    73,    74,    -1,    74,    -1,    85,
      -1,    15,    77,    75,   106,    78,    -1,    10,   104,   106,
      -1,    12,   110,   106,    -1,    -1,    50,    -1,    49,    -1,
      77,    48,    76,    -1,    76,    -1,    79,    80,    83,    84,
      -1,    85,    -1,    -1,    81,    -1,    -1,    81,    82,    -1,
      82,    -1,    16,   104,   106,    79,    -1,    16,   106,    85,
      -1,    21,    85,    -1,    -1,    22,    85,    -1,    -1,    86,
      -1,    87,    -1,    87,    88,    -1,    86,    88,    -1,    88,
      -1,    86,    89,    -1,    89,    -1,    94,    85,    43,    -1,
      93,    95,    43,    -1,    27,    97,    43,    -1,    26,   101,
      43,    -1,    92,   108,    43,    -1,     9,   104,   106,    -1,
      11,   110,   106,    -1,    52,    -1,    35,   105,    43,    -1,
      89,    90,    -1,    90,    -1,   103,    -1,    47,    -1,    91,
     104,    43,    -1,    28,    85,    43,    -1,    51,    -1,    32,
      -1,    38,    -1,    34,    -1,    39,    -1,    31,    -1,    36,
      -1,    37,    -1,    33,    -1,    40,    -1,    41,    -1,    42,
      -1,    23,    -1,    24,    -1,    25,    -1,    30,    -1,    29,
      -1,    95,    45,    85,    -1,    45,    85,    -1,    45,    98,
      -1,    97,    96,    -1,    96,    -1,    98,    44,    79,    -1,
      79,    -1,    99,    45,    85,    -1,    45,    85,    -1,    99,
      44,    79,    -1,   101,   100,    -1,   100,    -1,   103,    -1,
      47,    -1,    46,    -1,    48,    -1,   104,   103,    -1,   103,
      -1,   105,   102,    -1,   102,    -1,    51,    -1,    52,    -1,
     103,    -1,   106,    -1,   108,   107,    -1,   107,    -1,   109,
      46,    -1,   109,    47,    -1,    46,    -1,    47,    -1,   110,
      48,   109,    -1,   109,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   102,   102,   103,   106,   112,   116,   124,   125,   128,
     129,   132,   133,   134,   137,   138,   139,   140,   143,   144,
     145,   146,   149,   150,   153,   154,   155,   158,   158,   159,
     162,   163,   166,   167,   168,   171,   174,   175,   178,   179,
     180,   183,   190,   194,   197,   198,   209,   220,   221,   224,
     233,   234,   237,   238,   241,   242,   245,   246,   249,   250,
     253,   254,   262,   263,   266,   267,   268,   271,   272,   275,
     276,   277,   278,   279,   280,   281,   284,   285,   288,   289,
     292,   293,   294,   295,   296,   299,   300,   301,   302,   303,
     304,   305,   306,   309,   310,   311,   314,   315,   316,   319,
     320,   323,   324,   327,   330,   331,   334,   335,   338,   339,
     342,   349,   350,   353,   354,   357,   358,   361,   362,   365,
     366,   369,   370,   373,   374,   377,   378,   381,   382,   383,
     384,   387,   388
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
  "eateol", "dochead", "headline", "headtag", "sectiontag", "optsections",
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
      61,    62,    62,    62,    63,    63,    63,    63,    64,    64,
      64,    64,    65,    65,    66,    66,    66,    68,    67,    67,
      69,    69,    70,    70,    70,    71,    72,    72,    73,    73,
      73,    74,    74,    74,    75,    75,    76,    77,    77,    78,
      79,    79,    80,    80,    81,    81,    82,    82,    83,    83,
      84,    84,    85,    85,    86,    86,    86,    87,    87,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    89,    89,
      90,    90,    90,    90,    90,    91,    91,    91,    91,    91,
      91,    91,    91,    92,    92,    92,    93,    93,    93,    94,
      94,    95,    95,    96,    97,    97,    98,    98,    99,    99,
     100,   101,   101,   102,   102,   103,   103,   104,   104,   105,
     105,   106,   106,   107,   107,   108,   108,   109,   109,   109,
     109,   110,   110
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     4,     2,     3,     1,     0,     2,
       1,     3,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     2,     1,     1,     0,     5,     2,
       1,     0,     2,     1,     1,     4,     1,     0,     2,     1,
       1,     5,     3,     3,     0,     1,     1,     3,     1,     4,
       1,     0,     1,     0,     2,     1,     4,     3,     2,     0,
       2,     0,     1,     1,     2,     2,     1,     2,     1,     3,
       3,     3,     3,     3,     3,     3,     1,     3,     2,     1,
       1,     1,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     2,     2,     2,     1,     3,     1,     3,     2,
       3,     2,     1,     1,     1,     1,     1,     2,     1,     2,
       1,     1,     1,     1,     1,     2,     1,     2,     2,     1,
       1,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     8,     0,     0,     0,     0,   121,   122,     0,     7,
       0,     0,     0,     0,    27,     0,    20,    18,    19,    21,
      96,    97,    98,     0,     0,     0,   100,    99,    89,    85,
      92,    87,     0,    90,    91,    86,    88,    93,    94,    95,
     115,    81,   116,    84,    76,    31,     5,    25,    26,    39,
      40,    62,    63,    66,    68,    79,     0,     0,     0,     0,
      80,    14,    15,    16,     0,     0,    17,    23,    10,     0,
       1,     3,    23,   118,     0,     0,   129,   130,   132,     0,
       0,     0,    46,    48,    44,     0,     0,   112,     0,    51,
     105,     0,     0,   114,   120,   113,     0,     0,    29,    30,
      33,    34,    24,    38,    65,    67,    64,    78,     0,   123,
     124,   126,     0,     0,     0,     0,     0,     0,     9,     6,
      22,     0,     4,   117,    74,    42,   127,   128,     0,    75,
      43,     0,     0,    45,     0,   109,    51,     0,    72,   111,
     107,    50,   103,    71,   104,    83,    77,   119,     0,    32,
      82,    73,   125,   102,    70,     0,    69,    13,    12,    11,
     131,    31,    47,    51,   110,   108,    51,    37,   101,    28,
      41,    53,   106,    35,    36,     0,    59,    52,    55,     0,
       0,     0,    61,    54,    51,    57,    58,     0,    49,    56,
      60
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,     8,    67,    68,    69,    45,   119,   120,
      47,    81,    98,    99,   100,   173,    48,    49,   134,    83,
      84,   170,   140,   176,   177,   178,   182,   188,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,   114,    90,
      91,   142,    86,    87,    88,    94,    60,    74,    96,   110,
     111,   112,    78,    79
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -128
static const yytype_int16 yypact[] =
{
      21,    73,   224,   148,    11,   101,  -128,  -128,   148,  -128,
     -31,   -31,   116,   116,  -128,    15,  -128,  -128,  -128,  -128,
    -128,  -128,  -128,     1,    26,   346,  -128,  -128,  -128,  -128,
    -128,  -128,     5,  -128,  -128,  -128,  -128,  -128,  -128,  -128,
    -128,  -128,  -128,  -128,  -128,   268,    50,  -128,    25,  -128,
    -128,   346,     3,  -128,    58,  -128,   -31,    97,    37,   346,
    -128,  -128,  -128,  -128,   116,   116,  -128,   180,  -128,     5,
    -128,  -128,   180,  -128,    97,    97,  -128,  -128,   118,    59,
      59,     5,  -128,  -128,    35,   346,   127,  -128,    55,   346,
    -128,    99,    70,  -128,  -128,  -128,    71,     5,  -128,    28,
    -128,    25,  -128,  -128,  -128,    58,  -128,  -128,    80,  -128,
    -128,  -128,   -27,   346,   114,    91,    59,    59,  -128,  -128,
      50,    89,  -128,  -128,  -128,  -128,  -128,  -128,   116,  -128,
    -128,    89,    15,  -128,    73,  -128,   346,   346,  -128,  -128,
    -128,  -128,    49,  -128,  -128,  -128,  -128,  -128,    89,  -128,
    -128,  -128,  -128,  -128,  -128,   346,  -128,  -128,  -128,  -128,
     118,   268,  -128,   346,  -128,  -128,   346,   312,  -128,  -128,
    -128,   122,  -128,  -128,    25,    97,   125,   122,  -128,    97,
     346,   346,   138,  -128,   346,  -128,  -128,   346,  -128,  -128,
    -128
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -128,  -128,  -128,  -128,   159,   -54,  -128,  -128,   105,   167,
     -39,  -128,    12,  -128,    95,  -128,   -40,   -45,  -128,    64,
    -128,  -128,  -127,  -128,  -128,    24,  -128,  -128,    -5,  -128,
    -128,   129,   151,   -46,  -128,  -128,  -128,  -128,  -128,   132,
    -128,  -128,  -128,   136,  -128,   -90,    -9,    -7,   -47,    -1,
     113,  -128,   102,    -3
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -3
static const yytype_int16 yytable[] =
{
       9,    73,    73,   103,    75,   101,   147,   102,   107,   164,
      80,    70,    10,   118,    12,    40,   151,    42,   118,    40,
      92,    42,   121,    95,     6,     7,    20,    21,    22,    23,
      24,   147,    26,    27,   131,    11,   171,    13,    32,   172,
      15,   147,    97,    37,    38,    39,    85,    73,   109,   108,
     148,    40,    93,    42,   115,    44,   103,   189,   147,   107,
      95,   116,   117,    14,    82,   123,   123,    16,    17,    18,
      19,    89,    95,   124,   125,     1,     2,     3,   129,   130,
     135,   102,   113,   132,   141,   133,    25,    95,    95,    28,
      29,    30,    31,   166,    33,    34,    35,    36,   138,   123,
      85,    -2,    71,   109,    40,    41,    42,   128,   153,    43,
       6,     7,    95,   145,   146,   157,   158,    40,    93,    42,
     159,   101,    95,   150,     6,     7,    40,   174,    42,   103,
     161,   141,   165,   163,   156,    40,    93,    42,   175,    95,
       6,     7,   143,    40,    89,    42,   181,   167,     6,     7,
     168,    61,    62,    63,    64,    65,    66,   154,   141,   155,
     187,   141,    76,    77,   126,   127,    73,    72,   179,    46,
     123,   136,   137,   169,   180,   185,   186,   122,   184,   141,
     104,   106,   190,    61,    62,    63,    64,    65,    66,    10,
      11,    12,    13,    14,   149,    15,   162,    16,    17,    18,
      19,   183,   105,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,   144,   139,   152,    40,    41,    42,     0,
     160,    43,    44,    10,    11,    12,    13,    14,     0,    15,
       0,    16,    17,    18,    19,     0,     0,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,     0,     0,     0,
      40,    41,    42,     0,     0,    43,    44,    10,    11,    12,
      13,     0,    97,    15,     0,     0,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,     0,     0,     0,    40,    41,    42,     0,     0,    43,
      44,    10,    11,    12,    13,     0,     0,    15,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    10,     0,    12,    40,    41,
      42,     0,     0,    43,    44,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,     0,
       0,     0,    40,    41,    42,     0,     0,    43,    44
};

static const yytype_int16 yycheck[] =
{
       1,    10,    11,    48,    11,    45,    96,    46,    54,   136,
      13,     0,     9,    67,    11,    46,    43,    48,    72,    46,
      25,    48,    69,    32,    51,    52,    23,    24,    25,    26,
      27,   121,    29,    30,    81,    10,   163,    12,    35,   166,
      15,   131,    14,    40,    41,    42,    45,    56,    57,    56,
      97,    46,    47,    48,    59,    52,   101,   184,   148,   105,
      69,    64,    65,    13,    49,    74,    75,    17,    18,    19,
      20,    45,    81,    74,    75,    54,    55,    56,    79,    80,
      85,   120,    45,    48,    89,    50,    28,    96,    97,    31,
      32,    33,    34,    44,    36,    37,    38,    39,    43,   108,
      45,     0,     1,   112,    46,    47,    48,    48,   113,    51,
      51,    52,   121,    43,    43,   116,   117,    46,    47,    48,
     121,   161,   131,    43,    51,    52,    46,   167,    48,   174,
     131,   136,   137,   134,    43,    46,    47,    48,    16,   148,
      51,    52,    43,    46,    45,    48,    21,   148,    51,    52,
     155,     3,     4,     5,     6,     7,     8,    43,   163,    45,
      22,   166,    46,    47,    46,    47,   175,     8,   175,     2,
     179,    44,    45,   161,   175,   180,   181,    72,   179,   184,
      51,    52,   187,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    99,    15,   132,    17,    18,    19,
      20,   177,    51,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    91,    88,   112,    46,    47,    48,    -1,
     128,    51,    52,     9,    10,    11,    12,    13,    -1,    15,
      -1,    17,    18,    19,    20,    -1,    -1,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    -1,    -1,    -1,
      46,    47,    48,    -1,    -1,    51,    52,     9,    10,    11,
      12,    -1,    14,    15,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    -1,    -1,    -1,    46,    47,    48,    -1,    -1,    51,
      52,     9,    10,    11,    12,    -1,    -1,    15,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     9,    -1,    11,    46,    47,
      48,    -1,    -1,    51,    52,    -1,    -1,    -1,    -1,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    -1,
      -1,    -1,    46,    47,    48,    -1,    -1,    51,    52
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    54,    55,    56,    58,    59,    51,    52,    60,   106,
       9,    10,    11,    12,    13,    15,    17,    18,    19,    20,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      46,    47,    48,    51,    52,    64,    66,    67,    73,    74,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
     103,     3,     4,     5,     6,     7,     8,    61,    62,    63,
       0,     1,    61,   103,   104,   104,    46,    47,   109,   110,
     110,    68,    49,    76,    77,    45,    99,   100,   101,    45,
      96,    97,    85,    47,   102,   103,   105,    14,    69,    70,
      71,    73,    67,    74,    88,    89,    88,    90,   104,   103,
     106,   107,   108,    45,    95,    85,   110,   110,    62,    65,
      66,   105,    65,   103,   106,   106,    46,    47,    48,   106,
     106,   105,    48,    50,    75,    85,    44,    45,    43,   100,
      79,    85,    98,    43,    96,    43,    43,   102,   105,    71,
      43,    43,   107,    85,    43,    45,    43,   106,   106,   106,
     109,   106,    76,   106,    79,    85,    44,   106,    85,    69,
      78,    79,    79,    72,    73,    16,    80,    81,    82,   104,
     106,    21,    83,    82,   106,    85,    85,    22,    84,    79,
      85
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
#line 1415 "SCDoc.tab.cpp"
	break;
      case 47: /* "URL" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1424 "SCDoc.tab.cpp"
	break;
      case 48: /* "COMMA" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1433 "SCDoc.tab.cpp"
	break;
      case 49: /* "\"method name\"" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1442 "SCDoc.tab.cpp"
	break;
      case 50: /* "\"arguments string\"" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1451 "SCDoc.tab.cpp"
	break;
      case 59: /* "document" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1460 "SCDoc.tab.cpp"
	break;
      case 61: /* "dochead" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1469 "SCDoc.tab.cpp"
	break;
      case 62: /* "headline" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1478 "SCDoc.tab.cpp"
	break;
      case 65: /* "optsections" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1487 "SCDoc.tab.cpp"
	break;
      case 66: /* "sections" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1496 "SCDoc.tab.cpp"
	break;
      case 67: /* "section" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1505 "SCDoc.tab.cpp"
	break;
      case 69: /* "optsubsections" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1514 "SCDoc.tab.cpp"
	break;
      case 70: /* "subsections" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1523 "SCDoc.tab.cpp"
	break;
      case 71: /* "subsection" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1532 "SCDoc.tab.cpp"
	break;
      case 72: /* "optsubsubsections" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1541 "SCDoc.tab.cpp"
	break;
      case 73: /* "subsubsections" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1550 "SCDoc.tab.cpp"
	break;
      case 74: /* "subsubsection" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1559 "SCDoc.tab.cpp"
	break;
      case 75: /* "optMETHODARGS" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1568 "SCDoc.tab.cpp"
	break;
      case 76: /* "methodname" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1577 "SCDoc.tab.cpp"
	break;
      case 77: /* "methnames" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1586 "SCDoc.tab.cpp"
	break;
      case 78: /* "methodbody" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1595 "SCDoc.tab.cpp"
	break;
      case 79: /* "optbody" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1604 "SCDoc.tab.cpp"
	break;
      case 80: /* "optargs" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1613 "SCDoc.tab.cpp"
	break;
      case 81: /* "args" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1622 "SCDoc.tab.cpp"
	break;
      case 82: /* "arg" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1631 "SCDoc.tab.cpp"
	break;
      case 83: /* "optreturns" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1640 "SCDoc.tab.cpp"
	break;
      case 84: /* "optdiscussion" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1649 "SCDoc.tab.cpp"
	break;
      case 85: /* "body" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1658 "SCDoc.tab.cpp"
	break;
      case 86: /* "blockA" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1667 "SCDoc.tab.cpp"
	break;
      case 87: /* "blockB" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1676 "SCDoc.tab.cpp"
	break;
      case 88: /* "bodyelem" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1685 "SCDoc.tab.cpp"
	break;
      case 89: /* "prose" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1694 "SCDoc.tab.cpp"
	break;
      case 90: /* "proseelem" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1703 "SCDoc.tab.cpp"
	break;
      case 95: /* "listbody" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1712 "SCDoc.tab.cpp"
	break;
      case 96: /* "tablerow" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1721 "SCDoc.tab.cpp"
	break;
      case 97: /* "tablebody" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1730 "SCDoc.tab.cpp"
	break;
      case 98: /* "tablecells" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1739 "SCDoc.tab.cpp"
	break;
      case 99: /* "defterms" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1748 "SCDoc.tab.cpp"
	break;
      case 100: /* "deflistrow" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1757 "SCDoc.tab.cpp"
	break;
      case 101: /* "deflistbody" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1766 "SCDoc.tab.cpp"
	break;
      case 102: /* "anywordurl" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1775 "SCDoc.tab.cpp"
	break;
      case 103: /* "anyword" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1784 "SCDoc.tab.cpp"
	break;
      case 104: /* "words" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1793 "SCDoc.tab.cpp"
	break;
      case 105: /* "words2" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1802 "SCDoc.tab.cpp"
	break;
      case 107: /* "anywordnl" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1811 "SCDoc.tab.cpp"
	break;
      case 108: /* "wordsnl" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1820 "SCDoc.tab.cpp"
	break;
      case 109: /* "nocommawords" */

/* Line 1000 of yacc.c  */
#line 93 "SCDoc.y"
	{ free((yyvaluep->str)); };

/* Line 1000 of yacc.c  */
#line 1829 "SCDoc.tab.cpp"
	break;
      case 110: /* "commalist" */

/* Line 1000 of yacc.c  */
#line 92 "SCDoc.y"
	{ doc_node_free_tree((yyvaluep->doc_node)); };

/* Line 1000 of yacc.c  */
#line 1838 "SCDoc.tab.cpp"
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
        doc_node_add_child((yyval.doc_node), (yyvsp[(3) - (4)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(4) - (4)].doc_node));
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

  case 9:

/* Line 1455 of yacc.c  */
#line 128 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 129 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("HEADER",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 132 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make((yyvsp[(1) - (3)].id),(yyvsp[(2) - (3)].str),NULL); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 133 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("CATEGORIES",NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 134 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("RELATED",NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 137 "SCDoc.y"
    { (yyval.id) = "TITLE"; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 138 "SCDoc.y"
    { (yyval.id) = "TITLE"; ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 139 "SCDoc.y"
    { (yyval.id) = "SUMMARY"; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 140 "SCDoc.y"
    { (yyval.id) = "REDIRECT"; ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 143 "SCDoc.y"
    { (yyval.id) = "CLASSMETHODS"; method_type = "CMETHOD"; ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 144 "SCDoc.y"
    { (yyval.id) = "INSTANCEMETHODS"; method_type = "IMETHOD"; ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 145 "SCDoc.y"
    { (yyval.id) = "DESCRIPTION"; method_type = "METHOD"; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 146 "SCDoc.y"
    { (yyval.id) = "EXAMPLES"; method_type = "METHOD"; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 150 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("BODY",NULL,NULL); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 153 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 154 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("BODY",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 155 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("BODY",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 158 "SCDoc.y"
    { method_type = "METHOD"; ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 158 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("SECTION",(yyvsp[(3) - (5)].str),(yyvsp[(5) - (5)].doc_node)); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 159 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children((yyvsp[(1) - (2)].id), NULL,(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 163 "SCDoc.y"
    { (yyval.doc_node) = NULL; ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 166 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 167 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(SUBSECTIONS)",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 171 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("SUBSECTION", (yyvsp[(2) - (4)].str), (yyvsp[(4) - (4)].doc_node)); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 175 "SCDoc.y"
    { (yyval.doc_node) = NULL; ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 178 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 179 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(SUBSUBSECTIONS)",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 180 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("(SUBSUBSECTIONS)",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 184 "SCDoc.y"
    {
        (yyvsp[(2) - (5)].doc_node)->id = "METHODNAMES";
        (yyval.doc_node) = doc_node_make(method_type,(yyvsp[(3) - (5)].str),(yyvsp[(2) - (5)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(5) - (5)].doc_node));
//        doc_node_add_child($2, $3);
    ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 190 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make(
                method_type=="CMETHOD"?"CCOPYMETHOD":(method_type=="IMETHOD"?"ICOPYMETHOD":"COPYMETHOD"),
                (yyvsp[(2) - (3)].str),NULL
                ); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 194 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children(method_type=="CMETHOD"?"CPRIVATE":"IPRIVATE",NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 197 "SCDoc.y"
    { (yyval.str) = NULL; ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 199 "SCDoc.y"
    {
//        $$ = doc_node_make("ARGSTRING",$1,NULL);
        (yyval.str) = (yyvsp[(1) - (1)].str);
        if(method_type!="METHOD") {
            yyerror("METHOD argument string is not allowed inside CLASSMETHODS or INSTANCEMETHODS");
            YYERROR;
        }
    ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 210 "SCDoc.y"
    {
        char *p = (yyvsp[(1) - (1)].str)+strlen((yyvsp[(1) - (1)].str))-1;
        if(*p=='_') {
            post("WARNING: SCDoc: In %s\n  Property setter %s should be documented without underscore.\n", scdoc_current_file, (yyvsp[(1) - (1)].str));
            *p = '\0';
        };
        (yyval.str) = (yyvsp[(1) - (1)].str);
    ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 220 "SCDoc.y"
    { free((yyvsp[(2) - (3)].str)); (yyvsp[(2) - (3)].str) = NULL; (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node), doc_node_make("STRING",(yyvsp[(3) - (3)].str),NULL)); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 221 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(METHODNAMES)",NULL,doc_node_make("STRING",(yyvsp[(1) - (1)].str),NULL)); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 225 "SCDoc.y"
    {
        (yyval.doc_node) = doc_node_make_take_children("METHODBODY",NULL,(yyvsp[(1) - (4)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(2) - (4)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(3) - (4)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(4) - (4)].doc_node));
    ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 234 "SCDoc.y"
    { (yyval.doc_node) = NULL; ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 238 "SCDoc.y"
    { (yyval.doc_node) = NULL; ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 241 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 242 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("ARGUMENTS",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 245 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("ARGUMENT", (yyvsp[(2) - (4)].str), (yyvsp[(4) - (4)].doc_node)); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 246 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("ARGUMENT", NULL, (yyvsp[(3) - (3)].doc_node)); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 249 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("RETURNS",NULL,(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 250 "SCDoc.y"
    { (yyval.doc_node) = NULL; ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 253 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("DISCUSSION",NULL,(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 254 "SCDoc.y"
    { (yyval.doc_node) = NULL; ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 266 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
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
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 272 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(SECTIONBODY)",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 275 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children((yyvsp[(1) - (3)].id),NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 276 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children((yyvsp[(1) - (3)].id),NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 277 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("TABLE",NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 278 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("DEFINITIONLIST",NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 279 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make((yyvsp[(1) - (3)].id),(yyvsp[(2) - (3)].str),NULL); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 280 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("CLASSTREE",(yyvsp[(2) - (3)].str),NULL); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 281 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("KEYWORD",NULL,(yyvsp[(2) - (3)].doc_node));
//            printf("keyword '%s'\n",$2->children[0]->text);
        ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 284 "SCDoc.y"
    { (yyval.doc_node) = NULL; ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 285 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("IMAGE",(yyvsp[(2) - (3)].str),NULL); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 288 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node), (yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 289 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("PROSE",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 292 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("TEXT",(yyvsp[(1) - (1)].str),NULL); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 293 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("LINK",(yyvsp[(1) - (1)].str),NULL); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 294 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make((yyvsp[(1) - (3)].id),(yyvsp[(2) - (3)].str),NULL); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 295 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("FOOTNOTE",NULL,(yyvsp[(2) - (3)].doc_node)); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 296 "SCDoc.y"
    { (yyval.doc_node) = doc_node_create("NL"); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 299 "SCDoc.y"
    { (yyval.id) = "LINK"; ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 300 "SCDoc.y"
    { (yyval.id) = "STRONG"; ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 301 "SCDoc.y"
    { (yyval.id) = "SOFT"; ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 302 "SCDoc.y"
    { (yyval.id) = "EMPHASIS"; ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 303 "SCDoc.y"
    { (yyval.id) = "CODE"; ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 304 "SCDoc.y"
    { (yyval.id) = "TELETYPE"; ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 305 "SCDoc.y"
    { (yyval.id) = "MATH"; ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 306 "SCDoc.y"
    { (yyval.id) = "ANCHOR"; ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 309 "SCDoc.y"
    { (yyval.id) = "CODEBLOCK"; ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 310 "SCDoc.y"
    { (yyval.id) = "TELETYPEBLOCK"; ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 311 "SCDoc.y"
    { (yyval.id) = "MATHBLOCK"; ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 314 "SCDoc.y"
    { (yyval.id) = "LIST"; ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 315 "SCDoc.y"
    { (yyval.id) = "TREE"; ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 316 "SCDoc.y"
    { (yyval.id) = "NUMBEREDLIST"; ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 319 "SCDoc.y"
    { (yyval.id) = "WARNING"; ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 320 "SCDoc.y"
    { (yyval.id) = "NOTE"; ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 323 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node), doc_node_make_take_children("ITEM",NULL,(yyvsp[(3) - (3)].doc_node))); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 324 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(LISTBODY)",NULL, doc_node_make_take_children("ITEM",NULL,(yyvsp[(2) - (2)].doc_node))); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 327 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("TABROW",NULL,(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 330 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 331 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(TABLEBODY)",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 334 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node), doc_node_make_take_children("TABCOL",NULL,(yyvsp[(3) - (3)].doc_node))); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 335 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(TABLECELLS)",NULL, doc_node_make_take_children("TABCOL",NULL,(yyvsp[(1) - (1)].doc_node))); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 338 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node),doc_node_make_take_children("TERM",NULL,(yyvsp[(3) - (3)].doc_node))); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 339 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(TERMS)",NULL,doc_node_make_take_children("TERM",NULL,(yyvsp[(2) - (2)].doc_node))); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 343 "SCDoc.y"
    {
        (yyval.doc_node) = doc_node_make_take_children("DEFLISTITEM", NULL, (yyvsp[(1) - (3)].doc_node));
        doc_node_add_child((yyval.doc_node), doc_node_make_take_children("DEFINITION", NULL, (yyvsp[(3) - (3)].doc_node)));
    ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 349 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 350 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(DEFLISTBODY)",NULL,(yyvsp[(1) - (1)].doc_node)); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 361 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 365 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 374 "SCDoc.y"
    { (yyval.str) = strdup("\n"); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 377 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 381 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 382 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 387 "SCDoc.y"
    { free((yyvsp[(2) - (3)].str)); (yyvsp[(2) - (3)].str)=NULL; (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node),doc_node_make("STRING",(yyvsp[(3) - (3)].str),NULL)); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 388 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(COMMALIST)",NULL,doc_node_make("STRING",(yyvsp[(1) - (1)].str),NULL)); ;}
    break;



/* Line 1455 of yacc.c  */
#line 2975 "SCDoc.tab.cpp"
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
#line 391 "SCDoc.y"


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


