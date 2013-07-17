/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.7.12-4996"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


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
/* Line 371 of yacc.c  */
#line 1 "SCDoc.y"

/************************************************************************
*
* Copyright 2012 Jonatan Liljedahl <lijon@kymatica.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
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

static inline bool stringEqual(const char * a, const char * b)
{
    return strcmp(a, b) == 0;
}


/* Line 371 of yacc.c  */
#line 129 "SCDoc.tab.cpp"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "SCDoc.tab.hpp".  */
#ifndef YY_SCDOC_SCDOC_TAB_HPP_INCLUDED
# define YY_SCDOC_SCDOC_TAB_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int scdocdebug;
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
     STRONG = 292,
     EMPHASIS = 293,
     CODEBLOCK = 294,
     TELETYPEBLOCK = 295,
     TAGSYM = 296,
     BARS = 297,
     HASHES = 298,
     TEXT = 299,
     URL = 300,
     COMMA = 301,
     METHODNAME = 302,
     METHODARGS = 303,
     NEWLINE = 304,
     EMPTYLINES = 305,
     BAD_METHODNAME = 306,
     START_FULL = 307,
     START_PARTIAL = 308,
     START_METADATA = 309
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 55 "SCDoc.y"

    int i;
    const char *id;
    char *str;
    DocNode *doc_node;


/* Line 387 of yacc.c  */
#line 235 "SCDoc.tab.cpp"
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

extern YYSTYPE scdoclval;
extern YYLTYPE scdoclloc;
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int scdocparse (void *YYPARSE_PARAM);
#else
int scdocparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int scdocparse (void);
#else
int scdocparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_SCDOC_SCDOC_TAB_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */
/* Line 390 of yacc.c  */
#line 100 "SCDoc.y"

//int scdoclex (YYSTYPE * yylval_param, struct YYLTYPE * yylloc_param );
int scdoclex (void);

/* Line 390 of yacc.c  */
#line 281 "SCDoc.tab.cpp"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif


/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
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

# define YYCOPY_NEEDED 1

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

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  68
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   429

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  55
/* YYNRULES -- Number of rules.  */
#define YYNRULES  132
/* YYNRULES -- Number of states.  */
#define YYNSTATES  191

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   309

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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54
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
     261,   264,   267,   270,   272,   276,   278,   282,   285,   289,
     292,   294,   296,   298,   300,   302,   305,   307,   310,   312,
     314,   316,   318,   320,   322,   324,   327,   329,   332,   335,
     337,   339,   343
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      56,     0,    -1,    57,    -1,    57,     1,    -1,    52,    58,
      59,    63,    -1,    53,    64,    -1,    54,    59,    63,    -1,
     104,    -1,    -1,    59,    60,    -1,    60,    -1,    61,   103,
     104,    -1,     7,   109,   104,    -1,     6,   109,   104,    -1,
       3,    -1,     4,    -1,     5,    -1,     8,    -1,    18,    -1,
      19,    -1,    17,    -1,    20,    -1,    64,    -1,    -1,    64,
      65,    -1,    65,    -1,    71,    -1,    -1,    13,    66,   103,
     104,    67,    -1,    62,    67,    -1,    68,    -1,    -1,    68,
      69,    -1,    69,    -1,    71,    -1,    14,   103,   104,    70,
      -1,    71,    -1,    -1,    71,    72,    -1,    72,    -1,    83,
      -1,    15,    75,    73,   104,    76,    -1,    10,   102,   104,
      -1,    12,   109,   105,    -1,    -1,    48,    -1,    47,    -1,
      75,    46,    74,    -1,    74,    -1,    77,    78,    81,    82,
      -1,    83,    -1,    -1,    79,    -1,    -1,    79,    80,    -1,
      80,    -1,    16,   102,   104,    77,    -1,    16,   104,    83,
      -1,    21,    83,    -1,    -1,    22,    83,    -1,    -1,    84,
      -1,    85,    -1,    85,    86,    -1,    84,    86,    -1,    86,
      -1,    84,    87,    -1,    87,    -1,    92,    83,    41,    -1,
      91,    93,    41,    -1,    27,    95,    41,    -1,    26,    99,
      41,    -1,    90,   107,    41,    -1,     9,   102,   105,    -1,
      11,   109,   105,    -1,    50,    -1,    35,   103,    41,    -1,
      87,    88,    -1,    88,    -1,   101,    -1,    45,    -1,    89,
     102,    41,    -1,    28,    83,    41,    -1,    49,    -1,    32,
      -1,    37,    -1,    34,    -1,    38,    -1,    31,    -1,    36,
      -1,    33,    -1,    39,    -1,    40,    -1,    23,    -1,    24,
      -1,    25,    -1,    30,    -1,    29,    -1,    93,    43,    83,
      -1,    43,    83,    -1,    43,    96,    -1,    95,    94,    -1,
      94,    -1,    96,    42,    77,    -1,    77,    -1,    97,    43,
      83,    -1,    43,    83,    -1,    97,    42,    77,    -1,    99,
      98,    -1,    98,    -1,   101,    -1,    45,    -1,    44,    -1,
      46,    -1,   102,   101,    -1,   101,    -1,   103,   100,    -1,
     100,    -1,    49,    -1,    50,    -1,   104,    -1,     0,    -1,
     101,    -1,   104,    -1,   107,   106,    -1,   106,    -1,   108,
      44,    -1,   108,    45,    -1,    44,    -1,    45,    -1,   109,
      46,   108,    -1,   108,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   107,   107,   108,   111,   117,   121,   129,   130,   133,
     134,   137,   138,   139,   142,   143,   144,   145,   148,   149,
     150,   151,   154,   155,   158,   159,   160,   163,   163,   164,
     167,   168,   171,   172,   173,   176,   179,   180,   183,   184,
     185,   188,   195,   201,   206,   207,   218,   229,   230,   233,
     242,   243,   246,   247,   250,   251,   254,   255,   258,   259,
     262,   263,   271,   272,   275,   276,   277,   280,   281,   284,
     285,   286,   287,   288,   289,   290,   293,   294,   297,   298,
     301,   302,   303,   304,   305,   308,   309,   310,   311,   312,
     313,   314,   317,   318,   321,   322,   323,   326,   327,   330,
     331,   334,   337,   338,   341,   342,   345,   346,   349,   356,
     357,   360,   361,   364,   365,   368,   369,   372,   373,   376,
     377,   380,   381,   384,   385,   388,   389,   392,   393,   394,
     395,   398,   399
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
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
  "TELETYPE", "STRONG", "EMPHASIS", "\"CODE block\"", "\"TELETYPE block\"",
  "\"::\"", "\"||\"", "\"##\"", "\"text\"", "URL", "COMMA",
  "\"method name\"", "\"arguments string\"", "\"newline\"",
  "\"empty lines\"", "\"bad method name\"", "START_FULL", "START_PARTIAL",
  "START_METADATA", "$accept", "start", "document", "eateol", "dochead",
  "headline", "headtag", "sectiontag", "optsections", "sections",
  "section", "$@1", "optsubsections", "subsections", "subsection",
  "optsubsubsections", "subsubsections", "subsubsection", "optMETHODARGS",
  "methodname", "methnames", "methodbody", "optbody", "optargs", "args",
  "arg", "optreturns", "optdiscussion", "body", "blockA", "blockB",
  "bodyelem", "prose", "proseelem", "inlinetag", "blocktag", "listtag",
  "rangetag", "listbody", "tablerow", "tablebody", "tablecells",
  "defterms", "deflistrow", "deflistbody", "anywordurl", "anyword",
  "words", "words2", "eol", "eoleof", "anywordnl", "wordsnl",
  "nocommawords", "commalist", YY_NULL
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
     305,   306,   307,   308,   309
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    55,    56,    56,    57,    57,    57,    58,    58,    59,
      59,    60,    60,    60,    61,    61,    61,    61,    62,    62,
      62,    62,    63,    63,    64,    64,    64,    66,    65,    65,
      67,    67,    68,    68,    68,    69,    70,    70,    71,    71,
      71,    72,    72,    72,    73,    73,    74,    75,    75,    76,
      77,    77,    78,    78,    79,    79,    80,    80,    81,    81,
      82,    82,    83,    83,    84,    84,    84,    85,    85,    86,
      86,    86,    86,    86,    86,    86,    86,    86,    87,    87,
      88,    88,    88,    88,    88,    89,    89,    89,    89,    89,
      89,    89,    90,    90,    91,    91,    91,    92,    92,    93,
      93,    94,    95,    95,    96,    96,    97,    97,    98,    99,
      99,   100,   100,   101,   101,   102,   102,   103,   103,   104,
     104,   105,   105,   106,   106,   107,   107,   108,   108,   108,
     108,   109,   109
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
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       2,     2,     2,     1,     3,     1,     3,     2,     3,     2,
       1,     1,     1,     1,     1,     2,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     2,     2,     1,
       1,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     8,     0,     0,     0,     0,   119,   120,     0,     7,
       0,     0,     0,     0,    27,     0,    20,    18,    19,    21,
      94,    95,    96,     0,     0,     0,    98,    97,    89,    85,
      91,    87,     0,    90,    86,    88,    92,    93,   113,    81,
     114,    84,    76,    31,     5,    25,    26,    39,    40,    62,
      63,    66,    68,    79,     0,     0,     0,     0,    80,    14,
      15,    16,     0,     0,    17,    23,    10,     0,     1,     3,
      23,   116,     0,     0,   129,   130,   132,     0,     0,     0,
      46,    48,    44,     0,     0,   110,     0,    51,   103,     0,
       0,   112,   118,   111,     0,     0,    29,    30,    33,    34,
      24,    38,    65,    67,    64,    78,     0,   123,   124,   126,
       0,     0,     0,     0,     0,     0,     9,     6,    22,     0,
       4,   122,   115,   121,    74,    42,   127,   128,     0,    75,
      43,     0,     0,    45,     0,   107,    51,     0,    72,   109,
     105,    50,   101,    71,   102,    83,    77,   117,     0,    32,
      82,    73,   125,   100,    70,     0,    69,    13,    12,    11,
     131,    31,    47,    51,   108,   106,    51,    37,    99,    28,
      41,    53,   104,    35,    36,     0,    59,    52,    55,     0,
       0,     0,    61,    54,    51,    57,    58,     0,    49,    56,
      60
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,     8,    65,    66,    67,    43,   117,   118,
      45,    79,    96,    97,    98,   173,    46,    47,   134,    81,
      82,   170,   140,   176,   177,   178,   182,   188,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,   112,    88,
      89,   142,    84,    85,    86,    92,    58,    72,    94,   123,
     124,   109,   110,    76,    77
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -129
static const yytype_int16 yypact[] =
{
     -11,    42,   222,   137,    11,   111,  -129,  -129,   137,  -129,
     -17,   -17,    91,    91,  -129,    -8,  -129,  -129,  -129,  -129,
    -129,  -129,  -129,    14,    38,   338,  -129,  -129,  -129,  -129,
    -129,  -129,    71,  -129,  -129,  -129,  -129,  -129,  -129,  -129,
    -129,  -129,  -129,   264,    70,  -129,     7,  -129,  -129,   338,
     370,  -129,   380,  -129,   -17,    58,    41,   338,  -129,  -129,
    -129,  -129,    91,    91,  -129,   180,  -129,    71,  -129,  -129,
     180,  -129,    25,    58,  -129,  -129,    93,    16,    16,    71,
    -129,  -129,     3,   338,    85,  -129,    62,   338,  -129,    78,
      32,  -129,  -129,  -129,   -31,    71,  -129,    20,  -129,     7,
    -129,  -129,  -129,   380,  -129,  -129,     9,  -129,  -129,  -129,
     -13,   338,    83,    55,    30,    30,  -129,  -129,    70,    49,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,    91,  -129,
    -129,    49,    -8,  -129,    42,  -129,   338,   338,  -129,  -129,
    -129,  -129,   -12,  -129,  -129,  -129,  -129,  -129,    49,  -129,
    -129,  -129,  -129,  -129,  -129,   338,  -129,  -129,  -129,  -129,
      93,   264,  -129,   338,  -129,  -129,   338,   306,  -129,  -129,
    -129,    84,  -129,  -129,     7,    58,    27,    84,  -129,    58,
     338,   338,    98,  -129,   338,  -129,  -129,   338,  -129,  -129,
    -129
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -129,  -129,  -129,  -129,   117,   -44,  -129,  -129,    76,   151,
     -41,  -129,    -6,  -129,    57,  -129,   -38,   -40,  -129,    24,
    -129,  -129,  -128,  -129,  -129,   -20,  -129,  -129,    -5,  -129,
    -129,    99,   110,   -43,  -129,  -129,  -129,  -129,  -129,    73,
    -129,  -129,  -129,    77,  -129,   -87,    -9,    -7,   -55,    -1,
      74,    50,  -129,    36,     5
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -3
static const yytype_int16 yytable[] =
{
       9,    71,    71,   100,    73,    99,   101,   147,   164,   105,
     146,    68,   119,    38,    91,    40,   121,    11,    78,    13,
      90,   116,    15,    93,   131,   121,   116,    38,   151,    40,
     166,    38,   147,    40,    95,   171,     6,     7,   172,    80,
     148,     1,     2,     3,   147,    71,   107,   106,   181,   132,
     150,   133,   113,    38,   108,    40,   189,    83,    93,   101,
     105,   147,   128,   122,   122,     6,     7,   114,   115,    38,
      93,    40,   125,   145,     6,     7,   128,   100,   135,     6,
       7,    87,   141,    14,   111,    93,    93,    16,    17,    18,
      19,     6,     7,    38,    91,    40,   156,   122,     6,     7,
     175,   107,    38,   138,    40,    83,   153,     6,     7,   108,
      93,    -2,    69,   157,   158,    38,    91,    40,   159,   143,
     187,    87,    93,    99,   154,    70,   155,   136,   137,   174,
     161,   141,   165,   163,   101,    74,    75,   126,   127,    93,
      59,    60,    61,    62,    63,    64,   120,   167,   102,   104,
     168,   129,   130,    44,   149,   169,   162,   183,   141,   103,
     152,   141,   144,   139,   160,     0,    71,     0,   179,     0,
     122,     0,     0,     0,   180,   185,   186,     0,   184,   141,
       0,     0,   190,    59,    60,    61,    62,    63,    64,    10,
      11,    12,    13,    14,     0,    15,     0,    16,    17,    18,
      19,     0,     0,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,     0,     0,     0,    38,    39,    40,     0,     0,    41,
      42,    10,    11,    12,    13,    14,     0,    15,     0,    16,
      17,    18,    19,     0,     0,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,     0,     0,     0,    38,    39,    40,     0,
       0,    41,    42,    10,    11,    12,    13,     0,    95,    15,
       0,     0,     0,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,     0,     0,     0,    38,    39,
      40,     0,     0,    41,    42,    10,    11,    12,    13,     0,
       0,    15,     0,     0,     0,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    10,     0,    12,
      38,    39,    40,     0,     0,    41,    42,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    10,
       0,    12,    38,    39,    40,     0,     0,    41,    42,     0,
       0,     0,     0,    20,    21,    22,    23,    24,     0,    26,
      27,     0,     0,     0,     0,    32,     0,     0,    25,    36,
      37,    28,    29,    30,    31,     0,    33,    34,    35,     0,
      42,     0,     0,     0,    38,    39,    40,     0,     0,    41
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-129)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       1,    10,    11,    44,    11,    43,    46,    94,   136,    52,
      41,     0,    67,    44,    45,    46,     0,    10,    13,    12,
      25,    65,    15,    32,    79,     0,    70,    44,    41,    46,
      42,    44,   119,    46,    14,   163,    49,    50,   166,    47,
      95,    52,    53,    54,   131,    54,    55,    54,    21,    46,
      41,    48,    57,    44,    55,    46,   184,    43,    67,    99,
     103,   148,    46,    72,    73,    49,    50,    62,    63,    44,
      79,    46,    73,    41,    49,    50,    46,   118,    83,    49,
      50,    43,    87,    13,    43,    94,    95,    17,    18,    19,
      20,    49,    50,    44,    45,    46,    41,   106,    49,    50,
      16,   110,    44,    41,    46,    43,   111,    49,    50,   110,
     119,     0,     1,   114,   115,    44,    45,    46,   119,    41,
      22,    43,   131,   161,    41,     8,    43,    42,    43,   167,
     131,   136,   137,   134,   174,    44,    45,    44,    45,   148,
       3,     4,     5,     6,     7,     8,    70,   148,    49,    50,
     155,    77,    78,     2,    97,   161,   132,   177,   163,    49,
     110,   166,    89,    86,   128,    -1,   175,    -1,   175,    -1,
     179,    -1,    -1,    -1,   175,   180,   181,    -1,   179,   184,
      -1,    -1,   187,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    -1,    15,    -1,    17,    18,    19,
      20,    -1,    -1,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    -1,    44,    45,    46,    -1,    -1,    49,
      50,     9,    10,    11,    12,    13,    -1,    15,    -1,    17,
      18,    19,    20,    -1,    -1,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    -1,    -1,    -1,    44,    45,    46,    -1,
      -1,    49,    50,     9,    10,    11,    12,    -1,    14,    15,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    -1,    -1,    44,    45,
      46,    -1,    -1,    49,    50,     9,    10,    11,    12,    -1,
      -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     9,    -1,    11,
      44,    45,    46,    -1,    -1,    49,    50,    -1,    -1,    -1,
      -1,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     9,
      -1,    11,    44,    45,    46,    -1,    -1,    49,    50,    -1,
      -1,    -1,    -1,    23,    24,    25,    26,    27,    -1,    29,
      30,    -1,    -1,    -1,    -1,    35,    -1,    -1,    28,    39,
      40,    31,    32,    33,    34,    -1,    36,    37,    38,    -1,
      50,    -1,    -1,    -1,    44,    45,    46,    -1,    -1,    49
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    52,    53,    54,    56,    57,    49,    50,    58,   104,
       9,    10,    11,    12,    13,    15,    17,    18,    19,    20,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    44,    45,
      46,    49,    50,    62,    64,    65,    71,    72,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,   101,     3,
       4,     5,     6,     7,     8,    59,    60,    61,     0,     1,
      59,   101,   102,   102,    44,    45,   108,   109,   109,    66,
      47,    74,    75,    43,    97,    98,    99,    43,    94,    95,
      83,    45,   100,   101,   103,    14,    67,    68,    69,    71,
      65,    72,    86,    87,    86,    88,   102,   101,   104,   106,
     107,    43,    93,    83,   109,   109,    60,    63,    64,   103,
      63,     0,   101,   104,   105,   104,    44,    45,    46,   105,
     105,   103,    46,    48,    73,    83,    42,    43,    41,    98,
      77,    83,    96,    41,    94,    41,    41,   100,   103,    69,
      41,    41,   106,    83,    41,    43,    41,   104,   104,   104,
     108,   104,    74,   104,    77,    83,    42,   104,    83,    67,
      76,    77,    77,    70,    71,    16,    78,    79,    80,   102,
     104,    21,    81,    80,   104,    83,    83,    22,    82,    77,
      83
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (YYID (N))                                                     \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (YYID (0))
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

__attribute__((__unused__))
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
#else
static unsigned
yy_location_print_ (yyo, yylocp)
    FILE *yyo;
    YYLTYPE const * const yylocp;
#endif
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += fprintf (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += fprintf (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += fprintf (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += fprintf (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += fprintf (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

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
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  YYUSE (yytype);
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
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
      case 44: /* "text" */
/* Line 1393 of yacc.c  */
#line 98 "SCDoc.y"
        { free(((*yyvaluep).str)); };
/* Line 1393 of yacc.c  */
#line 1527 "SCDoc.tab.cpp"
        break;
      case 45: /* URL */
/* Line 1393 of yacc.c  */
#line 98 "SCDoc.y"
        { free(((*yyvaluep).str)); };
/* Line 1393 of yacc.c  */
#line 1534 "SCDoc.tab.cpp"
        break;
      case 46: /* COMMA */
/* Line 1393 of yacc.c  */
#line 98 "SCDoc.y"
        { free(((*yyvaluep).str)); };
/* Line 1393 of yacc.c  */
#line 1541 "SCDoc.tab.cpp"
        break;
      case 47: /* "method name" */
/* Line 1393 of yacc.c  */
#line 98 "SCDoc.y"
        { free(((*yyvaluep).str)); };
/* Line 1393 of yacc.c  */
#line 1548 "SCDoc.tab.cpp"
        break;
      case 48: /* "arguments string" */
/* Line 1393 of yacc.c  */
#line 98 "SCDoc.y"
        { free(((*yyvaluep).str)); };
/* Line 1393 of yacc.c  */
#line 1555 "SCDoc.tab.cpp"
        break;
      case 57: /* document */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1562 "SCDoc.tab.cpp"
        break;
      case 59: /* dochead */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1569 "SCDoc.tab.cpp"
        break;
      case 60: /* headline */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1576 "SCDoc.tab.cpp"
        break;
      case 63: /* optsections */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1583 "SCDoc.tab.cpp"
        break;
      case 64: /* sections */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1590 "SCDoc.tab.cpp"
        break;
      case 65: /* section */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1597 "SCDoc.tab.cpp"
        break;
      case 67: /* optsubsections */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1604 "SCDoc.tab.cpp"
        break;
      case 68: /* subsections */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1611 "SCDoc.tab.cpp"
        break;
      case 69: /* subsection */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1618 "SCDoc.tab.cpp"
        break;
      case 70: /* optsubsubsections */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1625 "SCDoc.tab.cpp"
        break;
      case 71: /* subsubsections */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1632 "SCDoc.tab.cpp"
        break;
      case 72: /* subsubsection */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1639 "SCDoc.tab.cpp"
        break;
      case 73: /* optMETHODARGS */
/* Line 1393 of yacc.c  */
#line 98 "SCDoc.y"
        { free(((*yyvaluep).str)); };
/* Line 1393 of yacc.c  */
#line 1646 "SCDoc.tab.cpp"
        break;
      case 74: /* methodname */
/* Line 1393 of yacc.c  */
#line 98 "SCDoc.y"
        { free(((*yyvaluep).str)); };
/* Line 1393 of yacc.c  */
#line 1653 "SCDoc.tab.cpp"
        break;
      case 75: /* methnames */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1660 "SCDoc.tab.cpp"
        break;
      case 76: /* methodbody */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1667 "SCDoc.tab.cpp"
        break;
      case 77: /* optbody */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1674 "SCDoc.tab.cpp"
        break;
      case 78: /* optargs */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1681 "SCDoc.tab.cpp"
        break;
      case 79: /* args */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1688 "SCDoc.tab.cpp"
        break;
      case 80: /* arg */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1695 "SCDoc.tab.cpp"
        break;
      case 81: /* optreturns */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1702 "SCDoc.tab.cpp"
        break;
      case 82: /* optdiscussion */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1709 "SCDoc.tab.cpp"
        break;
      case 83: /* body */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1716 "SCDoc.tab.cpp"
        break;
      case 84: /* blockA */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1723 "SCDoc.tab.cpp"
        break;
      case 85: /* blockB */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1730 "SCDoc.tab.cpp"
        break;
      case 86: /* bodyelem */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1737 "SCDoc.tab.cpp"
        break;
      case 87: /* prose */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1744 "SCDoc.tab.cpp"
        break;
      case 88: /* proseelem */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1751 "SCDoc.tab.cpp"
        break;
      case 93: /* listbody */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1758 "SCDoc.tab.cpp"
        break;
      case 94: /* tablerow */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1765 "SCDoc.tab.cpp"
        break;
      case 95: /* tablebody */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1772 "SCDoc.tab.cpp"
        break;
      case 96: /* tablecells */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1779 "SCDoc.tab.cpp"
        break;
      case 97: /* defterms */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1786 "SCDoc.tab.cpp"
        break;
      case 98: /* deflistrow */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1793 "SCDoc.tab.cpp"
        break;
      case 99: /* deflistbody */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1800 "SCDoc.tab.cpp"
        break;
      case 100: /* anywordurl */
/* Line 1393 of yacc.c  */
#line 98 "SCDoc.y"
        { free(((*yyvaluep).str)); };
/* Line 1393 of yacc.c  */
#line 1807 "SCDoc.tab.cpp"
        break;
      case 101: /* anyword */
/* Line 1393 of yacc.c  */
#line 98 "SCDoc.y"
        { free(((*yyvaluep).str)); };
/* Line 1393 of yacc.c  */
#line 1814 "SCDoc.tab.cpp"
        break;
      case 102: /* words */
/* Line 1393 of yacc.c  */
#line 98 "SCDoc.y"
        { free(((*yyvaluep).str)); };
/* Line 1393 of yacc.c  */
#line 1821 "SCDoc.tab.cpp"
        break;
      case 103: /* words2 */
/* Line 1393 of yacc.c  */
#line 98 "SCDoc.y"
        { free(((*yyvaluep).str)); };
/* Line 1393 of yacc.c  */
#line 1828 "SCDoc.tab.cpp"
        break;
      case 106: /* anywordnl */
/* Line 1393 of yacc.c  */
#line 98 "SCDoc.y"
        { free(((*yyvaluep).str)); };
/* Line 1393 of yacc.c  */
#line 1835 "SCDoc.tab.cpp"
        break;
      case 107: /* wordsnl */
/* Line 1393 of yacc.c  */
#line 98 "SCDoc.y"
        { free(((*yyvaluep).str)); };
/* Line 1393 of yacc.c  */
#line 1842 "SCDoc.tab.cpp"
        break;
      case 108: /* nocommawords */
/* Line 1393 of yacc.c  */
#line 98 "SCDoc.y"
        { free(((*yyvaluep).str)); };
/* Line 1393 of yacc.c  */
#line 1849 "SCDoc.tab.cpp"
        break;
      case 109: /* commalist */
/* Line 1393 of yacc.c  */
#line 97 "SCDoc.y"
        { doc_node_free_tree(((*yyvaluep).doc_node)); };
/* Line 1393 of yacc.c  */
#line 1856 "SCDoc.tab.cpp"
        break;

      default:
        break;
    }
}




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;


/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

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

       Refer to the stacks through separate pointers, to allow yyoverflow
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
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
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
  if (yypact_value_is_default (yyn))
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
      if (yytable_value_is_error (yyn))
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
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
/* Line 1787 of yacc.c  */
#line 107 "SCDoc.y"
    { topnode = (yyvsp[(1) - (1)].doc_node); }
    break;

  case 3:
/* Line 1787 of yacc.c  */
#line 108 "SCDoc.y"
    { topnode = NULL; doc_node_free_tree((yyvsp[(1) - (2)].doc_node)); }
    break;

  case 4:
/* Line 1787 of yacc.c  */
#line 112 "SCDoc.y"
    {
        (yyval.doc_node) = doc_node_create("DOCUMENT");
        doc_node_add_child((yyval.doc_node), (yyvsp[(3) - (4)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(4) - (4)].doc_node));
    }
    break;

  case 5:
/* Line 1787 of yacc.c  */
#line 118 "SCDoc.y"
    {
        (yyval.doc_node) = doc_node_make_take_children("BODY",NULL,(yyvsp[(2) - (2)].doc_node));
    }
    break;

  case 6:
/* Line 1787 of yacc.c  */
#line 122 "SCDoc.y"
    {
        (yyval.doc_node) = doc_node_create("DOCUMENT");
        doc_node_add_child((yyval.doc_node), (yyvsp[(2) - (3)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(3) - (3)].doc_node));
    }
    break;

  case 9:
/* Line 1787 of yacc.c  */
#line 133 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); }
    break;

  case 10:
/* Line 1787 of yacc.c  */
#line 134 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("HEADER",NULL,(yyvsp[(1) - (1)].doc_node)); }
    break;

  case 11:
/* Line 1787 of yacc.c  */
#line 137 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make((yyvsp[(1) - (3)].id),(yyvsp[(2) - (3)].str),NULL); }
    break;

  case 12:
/* Line 1787 of yacc.c  */
#line 138 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("CATEGORIES",NULL,(yyvsp[(2) - (3)].doc_node)); }
    break;

  case 13:
/* Line 1787 of yacc.c  */
#line 139 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("RELATED",NULL,(yyvsp[(2) - (3)].doc_node)); }
    break;

  case 14:
/* Line 1787 of yacc.c  */
#line 142 "SCDoc.y"
    { (yyval.id) = "TITLE"; }
    break;

  case 15:
/* Line 1787 of yacc.c  */
#line 143 "SCDoc.y"
    { (yyval.id) = "TITLE"; }
    break;

  case 16:
/* Line 1787 of yacc.c  */
#line 144 "SCDoc.y"
    { (yyval.id) = "SUMMARY"; }
    break;

  case 17:
/* Line 1787 of yacc.c  */
#line 145 "SCDoc.y"
    { (yyval.id) = "REDIRECT"; }
    break;

  case 18:
/* Line 1787 of yacc.c  */
#line 148 "SCDoc.y"
    { (yyval.id) = "CLASSMETHODS"; method_type = "CMETHOD"; }
    break;

  case 19:
/* Line 1787 of yacc.c  */
#line 149 "SCDoc.y"
    { (yyval.id) = "INSTANCEMETHODS"; method_type = "IMETHOD"; }
    break;

  case 20:
/* Line 1787 of yacc.c  */
#line 150 "SCDoc.y"
    { (yyval.id) = "DESCRIPTION"; method_type = "METHOD"; }
    break;

  case 21:
/* Line 1787 of yacc.c  */
#line 151 "SCDoc.y"
    { (yyval.id) = "EXAMPLES"; method_type = "METHOD"; }
    break;

  case 23:
/* Line 1787 of yacc.c  */
#line 155 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("BODY",NULL,NULL); }
    break;

  case 24:
/* Line 1787 of yacc.c  */
#line 158 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); }
    break;

  case 25:
/* Line 1787 of yacc.c  */
#line 159 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("BODY",NULL,(yyvsp[(1) - (1)].doc_node)); }
    break;

  case 26:
/* Line 1787 of yacc.c  */
#line 160 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("BODY",NULL,(yyvsp[(1) - (1)].doc_node)); }
    break;

  case 27:
/* Line 1787 of yacc.c  */
#line 163 "SCDoc.y"
    { method_type = "METHOD"; }
    break;

  case 28:
/* Line 1787 of yacc.c  */
#line 163 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("SECTION",(yyvsp[(3) - (5)].str),(yyvsp[(5) - (5)].doc_node)); }
    break;

  case 29:
/* Line 1787 of yacc.c  */
#line 164 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children((yyvsp[(1) - (2)].id), NULL,(yyvsp[(2) - (2)].doc_node)); }
    break;

  case 31:
/* Line 1787 of yacc.c  */
#line 168 "SCDoc.y"
    { (yyval.doc_node) = NULL; }
    break;

  case 32:
/* Line 1787 of yacc.c  */
#line 171 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); }
    break;

  case 33:
/* Line 1787 of yacc.c  */
#line 172 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(SUBSECTIONS)",NULL,(yyvsp[(1) - (1)].doc_node)); }
    break;

  case 35:
/* Line 1787 of yacc.c  */
#line 176 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("SUBSECTION", (yyvsp[(2) - (4)].str), (yyvsp[(4) - (4)].doc_node)); }
    break;

  case 37:
/* Line 1787 of yacc.c  */
#line 180 "SCDoc.y"
    { (yyval.doc_node) = NULL; }
    break;

  case 38:
/* Line 1787 of yacc.c  */
#line 183 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); }
    break;

  case 39:
/* Line 1787 of yacc.c  */
#line 184 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(SUBSUBSECTIONS)",NULL,(yyvsp[(1) - (1)].doc_node)); }
    break;

  case 40:
/* Line 1787 of yacc.c  */
#line 185 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("(SUBSUBSECTIONS)",NULL,(yyvsp[(1) - (1)].doc_node)); }
    break;

  case 41:
/* Line 1787 of yacc.c  */
#line 189 "SCDoc.y"
    {
        (yyvsp[(2) - (5)].doc_node)->id = "METHODNAMES";
        (yyval.doc_node) = doc_node_make(method_type,(yyvsp[(3) - (5)].str),(yyvsp[(2) - (5)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(5) - (5)].doc_node));
//        doc_node_add_child($2, $3);
    }
    break;

  case 42:
/* Line 1787 of yacc.c  */
#line 195 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make(
                stringEqual(method_type, "CMETHOD") ? "CCOPYMETHOD"
                                                    : (stringEqual(method_type, "IMETHOD") ? "ICOPYMETHOD"
                                                                                           : "COPYMETHOD"),
                (yyvsp[(2) - (3)].str), NULL
                ); }
    break;

  case 43:
/* Line 1787 of yacc.c  */
#line 201 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children( stringEqual(method_type, "CMETHOD") ? "CPRIVATE"
                                                                                                                : "IPRIVATE",
                NULL, (yyvsp[(2) - (3)].doc_node)); }
    break;

  case 44:
/* Line 1787 of yacc.c  */
#line 206 "SCDoc.y"
    { (yyval.str) = NULL; }
    break;

  case 45:
/* Line 1787 of yacc.c  */
#line 208 "SCDoc.y"
    {
//        $$ = doc_node_make("ARGSTRING",$1,NULL);
        (yyval.str) = (yyvsp[(1) - (1)].str);
        if(!stringEqual(method_type, "METHOD")) {
            yyerror("METHOD argument string is not allowed inside CLASSMETHODS or INSTANCEMETHODS");
            YYERROR;
        }
    }
    break;

  case 46:
/* Line 1787 of yacc.c  */
#line 219 "SCDoc.y"
    {
        char *p = (yyvsp[(1) - (1)].str)+strlen((yyvsp[(1) - (1)].str))-1;
        if(*p=='_') {
            post("WARNING: SCDoc: In %s\n  Property setter %s should be documented without underscore.\n", scdoc_current_file, (yyvsp[(1) - (1)].str));
            *p = '\0';
        };
        (yyval.str) = (yyvsp[(1) - (1)].str);
    }
    break;

  case 47:
/* Line 1787 of yacc.c  */
#line 229 "SCDoc.y"
    { free((yyvsp[(2) - (3)].str)); (yyvsp[(2) - (3)].str) = NULL; (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node), doc_node_make("STRING",(yyvsp[(3) - (3)].str),NULL)); }
    break;

  case 48:
/* Line 1787 of yacc.c  */
#line 230 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(METHODNAMES)",NULL,doc_node_make("STRING",(yyvsp[(1) - (1)].str),NULL)); }
    break;

  case 49:
/* Line 1787 of yacc.c  */
#line 234 "SCDoc.y"
    {
        (yyval.doc_node) = doc_node_make_take_children("METHODBODY",NULL,(yyvsp[(1) - (4)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(2) - (4)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(3) - (4)].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[(4) - (4)].doc_node));
    }
    break;

  case 51:
/* Line 1787 of yacc.c  */
#line 243 "SCDoc.y"
    { (yyval.doc_node) = NULL; }
    break;

  case 53:
/* Line 1787 of yacc.c  */
#line 247 "SCDoc.y"
    { (yyval.doc_node) = NULL; }
    break;

  case 54:
/* Line 1787 of yacc.c  */
#line 250 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); }
    break;

  case 55:
/* Line 1787 of yacc.c  */
#line 251 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("ARGUMENTS",NULL,(yyvsp[(1) - (1)].doc_node)); }
    break;

  case 56:
/* Line 1787 of yacc.c  */
#line 254 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("ARGUMENT", (yyvsp[(2) - (4)].str), (yyvsp[(4) - (4)].doc_node)); }
    break;

  case 57:
/* Line 1787 of yacc.c  */
#line 255 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("ARGUMENT", NULL, (yyvsp[(3) - (3)].doc_node)); }
    break;

  case 58:
/* Line 1787 of yacc.c  */
#line 258 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("RETURNS",NULL,(yyvsp[(2) - (2)].doc_node)); }
    break;

  case 59:
/* Line 1787 of yacc.c  */
#line 259 "SCDoc.y"
    { (yyval.doc_node) = NULL; }
    break;

  case 60:
/* Line 1787 of yacc.c  */
#line 262 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("DISCUSSION",NULL,(yyvsp[(2) - (2)].doc_node)); }
    break;

  case 61:
/* Line 1787 of yacc.c  */
#line 263 "SCDoc.y"
    { (yyval.doc_node) = NULL; }
    break;

  case 64:
/* Line 1787 of yacc.c  */
#line 275 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); }
    break;

  case 65:
/* Line 1787 of yacc.c  */
#line 276 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); }
    break;

  case 66:
/* Line 1787 of yacc.c  */
#line 277 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(SECTIONBODY)",NULL,(yyvsp[(1) - (1)].doc_node)); }
    break;

  case 67:
/* Line 1787 of yacc.c  */
#line 280 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); }
    break;

  case 68:
/* Line 1787 of yacc.c  */
#line 281 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(SECTIONBODY)",NULL,(yyvsp[(1) - (1)].doc_node)); }
    break;

  case 69:
/* Line 1787 of yacc.c  */
#line 284 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children((yyvsp[(1) - (3)].id),NULL,(yyvsp[(2) - (3)].doc_node)); }
    break;

  case 70:
/* Line 1787 of yacc.c  */
#line 285 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children((yyvsp[(1) - (3)].id),NULL,(yyvsp[(2) - (3)].doc_node)); }
    break;

  case 71:
/* Line 1787 of yacc.c  */
#line 286 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("TABLE",NULL,(yyvsp[(2) - (3)].doc_node)); }
    break;

  case 72:
/* Line 1787 of yacc.c  */
#line 287 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("DEFINITIONLIST",NULL,(yyvsp[(2) - (3)].doc_node)); }
    break;

  case 73:
/* Line 1787 of yacc.c  */
#line 288 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make((yyvsp[(1) - (3)].id),(yyvsp[(2) - (3)].str),NULL); }
    break;

  case 74:
/* Line 1787 of yacc.c  */
#line 289 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("CLASSTREE",(yyvsp[(2) - (3)].str),NULL); }
    break;

  case 75:
/* Line 1787 of yacc.c  */
#line 290 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("KEYWORD",NULL,(yyvsp[(2) - (3)].doc_node));
//            printf("keyword '%s'\n",$2->children[0]->text);
        }
    break;

  case 76:
/* Line 1787 of yacc.c  */
#line 293 "SCDoc.y"
    { (yyval.doc_node) = NULL; }
    break;

  case 77:
/* Line 1787 of yacc.c  */
#line 294 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("IMAGE",(yyvsp[(2) - (3)].str),NULL); }
    break;

  case 78:
/* Line 1787 of yacc.c  */
#line 297 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node), (yyvsp[(2) - (2)].doc_node)); }
    break;

  case 79:
/* Line 1787 of yacc.c  */
#line 298 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("PROSE",NULL,(yyvsp[(1) - (1)].doc_node)); }
    break;

  case 80:
/* Line 1787 of yacc.c  */
#line 301 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make(NODE_TEXT,(yyvsp[(1) - (1)].str),NULL); }
    break;

  case 81:
/* Line 1787 of yacc.c  */
#line 302 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("LINK",(yyvsp[(1) - (1)].str),NULL); }
    break;

  case 82:
/* Line 1787 of yacc.c  */
#line 303 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make((yyvsp[(1) - (3)].id),(yyvsp[(2) - (3)].str),NULL); }
    break;

  case 83:
/* Line 1787 of yacc.c  */
#line 304 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("FOOTNOTE",NULL,(yyvsp[(2) - (3)].doc_node)); }
    break;

  case 84:
/* Line 1787 of yacc.c  */
#line 305 "SCDoc.y"
    { (yyval.doc_node) = doc_node_create(NODE_NL); }
    break;

  case 85:
/* Line 1787 of yacc.c  */
#line 308 "SCDoc.y"
    { (yyval.id) = "LINK"; }
    break;

  case 86:
/* Line 1787 of yacc.c  */
#line 309 "SCDoc.y"
    { (yyval.id) = "STRONG"; }
    break;

  case 87:
/* Line 1787 of yacc.c  */
#line 310 "SCDoc.y"
    { (yyval.id) = "SOFT"; }
    break;

  case 88:
/* Line 1787 of yacc.c  */
#line 311 "SCDoc.y"
    { (yyval.id) = "EMPHASIS"; }
    break;

  case 89:
/* Line 1787 of yacc.c  */
#line 312 "SCDoc.y"
    { (yyval.id) = "CODE"; }
    break;

  case 90:
/* Line 1787 of yacc.c  */
#line 313 "SCDoc.y"
    { (yyval.id) = "TELETYPE"; }
    break;

  case 91:
/* Line 1787 of yacc.c  */
#line 314 "SCDoc.y"
    { (yyval.id) = "ANCHOR"; }
    break;

  case 92:
/* Line 1787 of yacc.c  */
#line 317 "SCDoc.y"
    { (yyval.id) = "CODEBLOCK"; }
    break;

  case 93:
/* Line 1787 of yacc.c  */
#line 318 "SCDoc.y"
    { (yyval.id) = "TELETYPEBLOCK"; }
    break;

  case 94:
/* Line 1787 of yacc.c  */
#line 321 "SCDoc.y"
    { (yyval.id) = "LIST"; }
    break;

  case 95:
/* Line 1787 of yacc.c  */
#line 322 "SCDoc.y"
    { (yyval.id) = "TREE"; }
    break;

  case 96:
/* Line 1787 of yacc.c  */
#line 323 "SCDoc.y"
    { (yyval.id) = "NUMBEREDLIST"; }
    break;

  case 97:
/* Line 1787 of yacc.c  */
#line 326 "SCDoc.y"
    { (yyval.id) = "WARNING"; }
    break;

  case 98:
/* Line 1787 of yacc.c  */
#line 327 "SCDoc.y"
    { (yyval.id) = "NOTE"; }
    break;

  case 99:
/* Line 1787 of yacc.c  */
#line 330 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node), doc_node_make_take_children("ITEM",NULL,(yyvsp[(3) - (3)].doc_node))); }
    break;

  case 100:
/* Line 1787 of yacc.c  */
#line 331 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(LISTBODY)",NULL, doc_node_make_take_children("ITEM",NULL,(yyvsp[(2) - (2)].doc_node))); }
    break;

  case 101:
/* Line 1787 of yacc.c  */
#line 334 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make_take_children("TABROW",NULL,(yyvsp[(2) - (2)].doc_node)); }
    break;

  case 102:
/* Line 1787 of yacc.c  */
#line 337 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); }
    break;

  case 103:
/* Line 1787 of yacc.c  */
#line 338 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(TABLEBODY)",NULL,(yyvsp[(1) - (1)].doc_node)); }
    break;

  case 104:
/* Line 1787 of yacc.c  */
#line 341 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node), doc_node_make_take_children("TABCOL",NULL,(yyvsp[(3) - (3)].doc_node))); }
    break;

  case 105:
/* Line 1787 of yacc.c  */
#line 342 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(TABLECELLS)",NULL, doc_node_make_take_children("TABCOL",NULL,(yyvsp[(1) - (1)].doc_node))); }
    break;

  case 106:
/* Line 1787 of yacc.c  */
#line 345 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node),doc_node_make_take_children("TERM",NULL,(yyvsp[(3) - (3)].doc_node))); }
    break;

  case 107:
/* Line 1787 of yacc.c  */
#line 346 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(TERMS)",NULL,doc_node_make_take_children("TERM",NULL,(yyvsp[(2) - (2)].doc_node))); }
    break;

  case 108:
/* Line 1787 of yacc.c  */
#line 350 "SCDoc.y"
    {
        (yyval.doc_node) = doc_node_make_take_children("DEFLISTITEM", NULL, (yyvsp[(1) - (3)].doc_node));
        doc_node_add_child((yyval.doc_node), doc_node_make_take_children("DEFINITION", NULL, (yyvsp[(3) - (3)].doc_node)));
    }
    break;

  case 109:
/* Line 1787 of yacc.c  */
#line 356 "SCDoc.y"
    { (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (2)].doc_node),(yyvsp[(2) - (2)].doc_node)); }
    break;

  case 110:
/* Line 1787 of yacc.c  */
#line 357 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(DEFLISTBODY)",NULL,(yyvsp[(1) - (1)].doc_node)); }
    break;

  case 115:
/* Line 1787 of yacc.c  */
#line 368 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); }
    break;

  case 117:
/* Line 1787 of yacc.c  */
#line 372 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); }
    break;

  case 124:
/* Line 1787 of yacc.c  */
#line 385 "SCDoc.y"
    { (yyval.str) = strdup("\n"); }
    break;

  case 125:
/* Line 1787 of yacc.c  */
#line 388 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); }
    break;

  case 127:
/* Line 1787 of yacc.c  */
#line 392 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); }
    break;

  case 128:
/* Line 1787 of yacc.c  */
#line 393 "SCDoc.y"
    { (yyval.str) = strmerge((yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str)); }
    break;

  case 131:
/* Line 1787 of yacc.c  */
#line 398 "SCDoc.y"
    { free((yyvsp[(2) - (3)].str)); (yyvsp[(2) - (3)].str)=NULL; (yyval.doc_node) = doc_node_add_child((yyvsp[(1) - (3)].doc_node),doc_node_make("STRING",(yyvsp[(3) - (3)].str),NULL)); }
    break;

  case 132:
/* Line 1787 of yacc.c  */
#line 399 "SCDoc.y"
    { (yyval.doc_node) = doc_node_make("(COMMALIST)",NULL,doc_node_make("STRING",(yyvsp[(1) - (1)].str),NULL)); }
    break;


/* Line 1787 of yacc.c  */
#line 2860 "SCDoc.tab.cpp"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
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
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

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

  yyerror_range[1] = yylsp[1-yylen];
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
      if (!yypact_value_is_default (yyn))
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

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
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

#if !defined yyoverflow || YYERROR_VERBOSE
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
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
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


/* Line 2050 of yacc.c  */
#line 402 "SCDoc.y"


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

