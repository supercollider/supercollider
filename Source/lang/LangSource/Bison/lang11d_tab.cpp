/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NAME = 258,
     INTEGER = 259,
     SC_FLOAT = 260,
     ACCIDENTAL = 261,
     SYMBOL = 262,
     STRING = 263,
     ASCII = 264,
     PRIMITIVENAME = 265,
     CLASSNAME = 266,
     CURRYARG = 267,
     VAR = 268,
     ARG = 269,
     CLASSVAR = 270,
     SC_CONST = 271,
     NILOBJ = 272,
     TRUEOBJ = 273,
     FALSEOBJ = 274,
     PSEUDOVAR = 275,
     ELLIPSIS = 276,
     DOTDOT = 277,
     PIE = 278,
     BEGINCLOSEDFUNC = 279,
     BADTOKEN = 280,
     INTERPRET = 281,
     BEGINGENERATOR = 282,
     LEFTARROW = 283,
     WHILE = 284,
     READWRITEVAR = 285,
     KEYBINOP = 286,
     BINOP = 287,
     UMINUS = 288
   };
#endif
/* Tokens.  */
#define NAME 258
#define INTEGER 259
#define SC_FLOAT 260
#define ACCIDENTAL 261
#define SYMBOL 262
#define STRING 263
#define ASCII 264
#define PRIMITIVENAME 265
#define CLASSNAME 266
#define CURRYARG 267
#define VAR 268
#define ARG 269
#define CLASSVAR 270
#define SC_CONST 271
#define NILOBJ 272
#define TRUEOBJ 273
#define FALSEOBJ 274
#define PSEUDOVAR 275
#define ELLIPSIS 276
#define DOTDOT 277
#define PIE 278
#define BEGINCLOSEDFUNC 279
#define BADTOKEN 280
#define INTERPRET 281
#define BEGINGENERATOR 282
#define LEFTARROW 283
#define WHILE 284
#define READWRITEVAR 285
#define KEYBINOP 286
#define BINOP 287
#define UMINUS 288




/* Copy the first part of user declarations.  */
#line 16 "lang11d"


#include <stdlib.h>
#include <string.h>
#include "PyrLexer.h"
#include "PyrParseNode.h"
#include "SC_Constants.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "InitAlloc.h"
#include "PredefinedSymbols.h"
#include "SimpleStack.h"

void *alloca(unsigned long size); 
void bcopy(void *src, void *dst, size_t size) ;
int yyparse();
extern bool compilingCmdLine;
extern LongStack generatorStack;




/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 194 "lang11d_tab.cpp"

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
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
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
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

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
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  68
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1908

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  102
/* YYNRULES -- Number of rules.  */
#define YYNRULES  294
/* YYNRULES -- Number of states.  */
#define YYNSTATES  530

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   288

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    54,     2,     2,     2,     2,
      50,    51,    35,    36,    49,    32,    41,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    30,    48,
      33,    31,    34,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    46,     2,    47,    52,     2,    42,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    44,    37,    45,    53,     2,     2,     2,
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
      25,    26,    27,    28,    29,    38,    39,    40,    43
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,    10,    11,    14,    16,    19,
      26,    36,    42,    43,    45,    46,    49,    50,    53,    57,
      61,    65,    66,    69,    77,    86,    94,   103,   104,   106,
     107,   109,   110,   112,   114,   117,   122,   125,   127,   129,
     132,   133,   136,   137,   141,   142,   146,   148,   151,   153,
     155,   156,   158,   161,   166,   173,   180,   189,   195,   203,
     208,   211,   216,   222,   229,   235,   241,   248,   256,   264,
     271,   278,   287,   295,   300,   301,   309,   310,   318,   319,
     322,   327,   333,   339,   342,   347,   352,   354,   356,   358,
     360,   362,   364,   368,   371,   375,   379,   384,   388,   390,
     395,   397,   403,   409,   416,   424,   432,   441,   448,   455,
     463,   472,   481,   491,   494,   497,   501,   507,   512,   515,
     518,   522,   527,   533,   535,   537,   539,   541,   547,   550,
     555,   559,   564,   570,   578,   583,   590,   598,   599,   602,
     605,   610,   612,   616,   619,   620,   623,   625,   629,   632,
     636,   641,   647,   649,   653,   656,   661,   663,   667,   670,
     671,   674,   676,   680,   682,   686,   688,   690,   692,   694,
     696,   698,   700,   702,   704,   706,   708,   710,   712,   714,
     716,   718,   720,   722,   724,   726,   728,   730,   732,   734,
     736,   738,   740,   742,   744,   746,   748,   750,   752,   758,
     764,   765,   768,   770,   773,   777,   778,   782,   788,   792,
     798,   800,   804,   809,   810,   812,   814,   818,   820,   824,
     830,   831,   833,   835,   839,   841,   845,   850,   854,   857,
     859,   863,   864,   866,   868,   872,   875,   880,   885,   889,
     893,   896,   898,   902,   903,   905,   910,   916,   920,   925,
     926,   929,   931,   935,   936,   938,   940,   942,   943,   945,
     947,   950,   952,   955,   957,   960,   962,   964,   966,   969,
     972,   974,   977,   979,   981,   983,   985,   987,   989,   991,
     993,   995,   997,   999,  1001,  1003,  1005,  1007,  1009,  1011,
    1013,  1015,  1017,  1019,  1021
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      56,     0,    -1,    57,    -1,    58,    -1,    26,    71,    -1,
      -1,    57,    59,    -1,    60,    -1,    58,    60,    -1,   144,
      62,    44,    63,    65,    45,    -1,   144,    46,    61,    47,
      62,    44,    63,    65,    45,    -1,    36,   144,    44,    65,
      45,    -1,    -1,   143,    -1,    -1,    30,   144,    -1,    -1,
      63,    64,    -1,    15,   125,    48,    -1,    13,   125,    48,
      -1,    16,   114,    48,    -1,    -1,    65,    66,    -1,   143,
      44,   113,   110,    73,    72,    45,    -1,    35,   143,    44,
     113,   110,    73,    72,    45,    -1,   153,    44,   113,   110,
      73,    72,    45,    -1,    35,   153,    44,   113,   110,    73,
      72,    45,    -1,    -1,    48,    -1,    -1,    49,    -1,    -1,
      31,    -1,    75,    -1,    94,    75,    -1,    50,   111,    70,
      51,    -1,   111,    70,    -1,    70,    -1,    74,    -1,    94,
      74,    -1,    -1,   145,    67,    -1,    -1,    52,    91,    67,
      -1,    -1,    52,    91,    67,    -1,    77,    -1,    76,    77,
      -1,   105,    -1,    80,    -1,    -1,    76,    -1,   143,    76,
      -1,    50,   155,    51,    76,    -1,   143,    50,    97,   101,
      51,    78,    -1,    50,   155,    51,    50,    51,    78,    -1,
      50,   155,    51,    50,    97,   101,    51,    78,    -1,   143,
      50,    98,   101,    51,    -1,    50,   155,    51,    50,    98,
     101,    51,    -1,   144,    46,    95,    47,    -1,   144,    76,
      -1,   144,    50,    51,    78,    -1,   144,    50,    99,    51,
      78,    -1,   144,    50,    97,   101,    51,    78,    -1,   144,
      50,    98,   101,    51,    -1,    91,    41,    50,    51,    78,
      -1,    91,    41,    50,    99,    51,    78,    -1,    91,    41,
     143,    50,    99,    51,    78,    -1,    91,    41,    50,    97,
     101,    51,    78,    -1,    91,    41,    50,    98,   101,    51,
      -1,    91,    41,   143,    50,    51,    78,    -1,    91,    41,
     143,    50,    97,   101,    51,    78,    -1,    91,    41,   143,
      50,    98,   101,    51,    -1,    91,    41,   143,    78,    -1,
      -1,    44,    30,    94,    81,    49,    84,    45,    -1,    -1,
      44,    48,    94,    82,    49,    84,    45,    -1,    -1,    49,
      84,    -1,   143,    28,    94,    83,    -1,   143,   143,    28,
      94,    83,    -1,    13,   143,    31,    94,    83,    -1,    94,
      83,    -1,    30,    30,    94,    83,    -1,    30,    29,    94,
      83,    -1,   107,    -1,   105,    -1,    80,    -1,   106,    -1,
     156,    -1,    79,    -1,    50,    94,    51,    -1,    53,   143,
      -1,    46,    95,    47,    -1,    50,    89,    51,    -1,    50,
      30,    90,    51,    -1,    50,   124,    51,    -1,   152,    -1,
      85,    46,    97,    47,    -1,    86,    -1,    85,    46,    97,
      22,    47,    -1,    85,    46,    22,    94,    47,    -1,    85,
      46,    97,    22,    94,    47,    -1,    85,    46,    97,    22,
      47,    31,    91,    -1,    85,    46,    22,    94,    47,    31,
      91,    -1,    85,    46,    97,    22,    94,    47,    31,    91,
      -1,    91,    41,    46,    97,    22,    47,    -1,    91,    41,
      46,    22,    94,    47,    -1,    91,    41,    46,    97,    22,
      94,    47,    -1,    91,    41,    46,    97,    22,    47,    31,
      91,    -1,    91,    41,    46,    22,    94,    47,    31,    91,
      -1,    91,    41,    46,    97,    22,    94,    47,    31,    91,
      -1,    94,    22,    -1,    22,    94,    -1,    94,    22,    94,
      -1,    94,    49,    94,    22,    94,    -1,    94,    49,    94,
      22,    -1,    22,    94,    -1,    94,    22,    -1,    94,    22,
      94,    -1,    94,    49,    94,    22,    -1,    94,    49,    94,
      22,    94,    -1,    85,    -1,    88,    -1,    87,    -1,   144,
      -1,    91,    41,    46,    97,    47,    -1,    42,    91,    -1,
      91,   155,    92,    91,    -1,   143,    31,    91,    -1,    53,
     143,    31,    91,    -1,    91,    41,   143,    31,    91,    -1,
     143,    50,    97,   101,    51,    31,    91,    -1,    54,   102,
      31,    91,    -1,    85,    46,    97,    47,    31,    91,    -1,
      91,    41,    46,    97,    47,    31,    91,    -1,    -1,    41,
     143,    -1,    41,   138,    -1,    41,    50,    94,    51,    -1,
      91,    -1,    93,    48,    91,    -1,    93,    67,    -1,    -1,
      96,    68,    -1,    94,    -1,    94,    30,    94,    -1,   154,
      94,    -1,    96,    49,    94,    -1,    96,    49,   154,    94,
      -1,    96,    49,    94,    30,    94,    -1,    94,    -1,    97,
      49,    94,    -1,    35,    94,    -1,    97,    49,    35,    94,
      -1,   100,    -1,    99,    49,   100,    -1,   154,    94,    -1,
      -1,    49,    99,    -1,   103,    -1,   103,    21,   143,    -1,
     143,    -1,   103,    49,   143,    -1,   138,    -1,   142,    -1,
     149,    -1,   151,    -1,   150,    -1,   146,    -1,   147,    -1,
     148,    -1,   132,    -1,   127,    -1,   109,    -1,   143,    -1,
     138,    -1,   142,    -1,   149,    -1,   151,    -1,   150,    -1,
     146,    -1,   147,    -1,   148,    -1,   132,    -1,   127,    -1,
     138,    -1,   142,    -1,   149,    -1,   151,    -1,   150,    -1,
     143,    -1,   146,    -1,   147,    -1,   148,    -1,   133,    -1,
     128,    -1,    44,   113,   110,    70,    45,    -1,    24,   113,
     110,    70,    45,    -1,    -1,   110,   112,    -1,   112,    -1,
     111,   112,    -1,    13,   120,    48,    -1,    -1,    14,   120,
      48,    -1,    14,   119,    21,   143,    48,    -1,    37,   117,
      37,    -1,    37,   116,    21,   143,    37,    -1,   115,    -1,
     114,    68,   115,    -1,   137,   143,    31,   104,    -1,    -1,
     117,    -1,   118,    -1,   117,    68,   118,    -1,   143,    -1,
     143,    69,   104,    -1,   143,    69,    50,    94,    51,    -1,
      -1,   120,    -1,   121,    -1,   120,    49,   121,    -1,   143,
      -1,   143,    31,    91,    -1,   143,    50,    94,    51,    -1,
      94,    30,    94,    -1,   154,    94,    -1,   122,    -1,   123,
      49,   122,    -1,    -1,   123,    -1,   126,    -1,   125,    49,
     126,    -1,   136,   143,    -1,   136,   143,    31,   104,    -1,
      54,    50,   131,    51,    -1,    50,   131,    51,    -1,   108,
      30,   108,    -1,   154,   108,    -1,   129,    -1,   130,    49,
     129,    -1,    -1,   130,    -1,    54,    46,   134,    47,    -1,
      54,   144,    46,   134,    47,    -1,    46,   134,    47,    -1,
     144,    46,   134,    47,    -1,    -1,   135,    68,    -1,   108,
      -1,   135,    49,   108,    -1,    -1,    33,    -1,    38,    -1,
      34,    -1,    -1,    33,    -1,     4,    -1,    32,     4,    -1,
       5,    -1,    32,     5,    -1,     6,    -1,    32,     6,    -1,
      23,    -1,   139,    -1,   140,    -1,   139,   141,    -1,   138,
     141,    -1,   141,    -1,    32,   141,    -1,     3,    -1,    29,
      -1,    11,    -1,    10,    -1,    18,    -1,    19,    -1,    17,
      -1,     9,    -1,     7,    -1,     8,    -1,    20,    -1,    40,
      -1,    38,    -1,    33,    -1,    34,    -1,    32,    -1,    35,
      -1,    36,    -1,    37,    -1,    39,    -1,   153,    -1,   154,
      -1,    12,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    40,    40,    42,    44,    48,    49,    53,    54,    58,
      62,    69,    75,    76,    79,    80,    84,    85,    89,    91,
      93,    97,    98,   102,   105,   108,   111,   116,   117,   120,
     121,   124,   125,   128,   129,   133,   135,   137,   141,   142,
     146,   147,   152,   153,   158,   159,   163,   164,   170,   171,
     174,   175,   178,   182,   186,   191,   195,   200,   218,   231,
     233,   244,   255,   266,   279,   300,   309,   318,   323,   336,
     356,   360,   366,   384,   390,   390,   400,   400,   407,   428,
     432,   466,   504,   518,   529,   533,   558,   559,   560,   561,
     562,   563,   564,   570,   580,   582,   584,   586,   588,   590,
     603,   606,   633,   651,   678,   706,   725,   753,   780,   798,
     823,   851,   870,   898,   917,   936,   953,   967,   988,  1007,
    1025,  1042,  1058,  1074,  1075,  1076,  1077,  1078,  1091,  1105,
    1110,  1114,  1125,  1130,  1140,  1145,  1159,  1175,  1176,  1177,
    1178,  1181,  1182,  1188,  1191,  1192,  1196,  1197,  1199,  1204,
    1206,  1213,  1221,  1222,  1226,  1228,  1232,  1233,  1237,  1241,
    1242,  1245,  1247,  1251,  1252,  1257,  1258,  1259,  1260,  1261,
    1262,  1263,  1264,  1265,  1266,  1269,  1272,  1275,  1276,  1277,
    1278,  1279,  1280,  1281,  1282,  1283,  1284,  1287,  1288,  1289,
    1290,  1291,  1292,  1293,  1294,  1295,  1296,  1297,  1300,  1303,
    1308,  1309,  1313,  1314,  1318,  1322,  1323,  1327,  1331,  1335,
    1341,  1342,  1346,  1350,  1351,  1354,  1355,  1359,  1361,  1363,
    1371,  1372,  1375,  1376,  1380,  1382,  1384,  1392,  1394,  1401,
    1402,  1406,  1407,  1410,  1411,  1415,  1417,  1421,  1425,  1429,
    1431,  1438,  1439,  1443,  1444,  1449,  1451,  1455,  1457,  1461,
    1462,  1465,  1466,  1470,  1471,  1473,  1475,  1479,  1480,  1484,
    1485,  1494,  1495,  1504,  1505,  1516,  1519,  1520,  1521,  1527,
    1535,  1542,  1551,  1552,  1555,  1558,  1561,  1564,  1567,  1570,
    1573,  1576,  1579,  1582,  1583,  1584,  1585,  1586,  1587,  1588,
    1589,  1592,  1595,  1596,  1599
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NAME", "INTEGER", "SC_FLOAT",
  "ACCIDENTAL", "SYMBOL", "STRING", "ASCII", "PRIMITIVENAME", "CLASSNAME",
  "CURRYARG", "VAR", "ARG", "CLASSVAR", "SC_CONST", "NILOBJ", "TRUEOBJ",
  "FALSEOBJ", "PSEUDOVAR", "ELLIPSIS", "DOTDOT", "PIE", "BEGINCLOSEDFUNC",
  "BADTOKEN", "INTERPRET", "BEGINGENERATOR", "LEFTARROW", "WHILE", "':'",
  "'='", "'-'", "'<'", "'>'", "'*'", "'+'", "'|'", "READWRITEVAR",
  "KEYBINOP", "BINOP", "'.'", "'`'", "UMINUS", "'{'", "'}'", "'['", "']'",
  "';'", "','", "'('", "')'", "'^'", "'~'", "'#'", "$accept", "root",
  "classes", "classextensions", "classdef", "classextension", "optname",
  "superclass", "classvardecls", "classvardecl", "methods", "methoddef",
  "optsemi", "optcomma", "optequal", "funcbody", "cmdlinecode", "methbody",
  "primitive", "retval", "funretval", "blocklist1", "blocklistitem",
  "blocklist", "msgsend", "generator", "@1", "@2", "nextqual", "qual",
  "expr1", "valrangex1", "valrangeassign", "valrangexd", "valrange2",
  "valrange3", "expr", "adverb", "exprn", "exprseq", "arrayelems",
  "arrayelems1", "arglist1", "arglistv1", "keyarglist1", "keyarg",
  "optkeyarglist", "mavars", "mavarlist", "slotliteral", "blockliteral",
  "pushname", "pushliteral", "listliteral", "block", "funcvardecls",
  "funcvardecls1", "funcvardecl", "argdecls", "constdeflist", "constdef",
  "slotdeflist0", "slotdeflist", "slotdef", "vardeflist0", "vardeflist",
  "vardef", "dictslotdef", "dictslotlist1", "dictslotlist",
  "rwslotdeflist", "rwslotdef", "dictlit", "dictlit2", "litdictslotdef",
  "litdictslotlist1", "litdictslotlist", "listlit", "listlit2",
  "literallistc", "literallist1", "rwspec", "rspec", "integer", "floatr",
  "accidental", "pie", "floatp", "name", "classname", "primname",
  "trueobj", "falseobj", "nilobj", "ascii", "symbol", "string",
  "pseudovar", "binop", "keybinop", "binop2", "curryarg", 0
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
      58,    61,    45,    60,    62,    42,    43,   124,   285,   286,
     287,    46,    96,   288,   123,   125,    91,    93,    59,    44,
      40,    41,    94,   126,    35
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    55,    56,    56,    56,    57,    57,    58,    58,    59,
      59,    60,    61,    61,    62,    62,    63,    63,    64,    64,
      64,    65,    65,    66,    66,    66,    66,    67,    67,    68,
      68,    69,    69,    70,    70,    71,    71,    71,    72,    72,
      73,    73,    74,    74,    75,    75,    76,    76,    77,    77,
      78,    78,    79,    79,    79,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    81,    80,    82,    80,    83,    83,
      84,    84,    84,    84,    84,    84,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    86,    86,    86,    87,    87,    87,    88,    88,    88,
      88,    88,    88,    89,    89,    89,    89,    89,    90,    90,
      90,    90,    90,    91,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    91,    91,    91,    91,    92,    92,    92,
      92,    93,    93,    94,    95,    95,    96,    96,    96,    96,
      96,    96,    97,    97,    98,    98,    99,    99,   100,   101,
     101,   102,   102,   103,   103,   104,   104,   104,   104,   104,
     104,   104,   104,   104,   104,   105,   106,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   109,   109,
     110,   110,   111,   111,   112,   113,   113,   113,   113,   113,
     114,   114,   115,   116,   116,   117,   117,   118,   118,   118,
     119,   119,   120,   120,   121,   121,   121,   122,   122,   123,
     123,   124,   124,   125,   125,   126,   126,   127,   128,   129,
     129,   130,   130,   131,   131,   132,   132,   133,   133,   134,
     134,   135,   135,   136,   136,   136,   136,   137,   137,   138,
     138,   139,   139,   140,   140,   141,   142,   142,   142,   142,
     142,   142,   143,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   153,   153,   153,   153,   153,   153,
     153,   154,   155,   155,   156
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     0,     2,     1,     2,     6,
       9,     5,     0,     1,     0,     2,     0,     2,     3,     3,
       3,     0,     2,     7,     8,     7,     8,     0,     1,     0,
       1,     0,     1,     1,     2,     4,     2,     1,     1,     2,
       0,     2,     0,     3,     0,     3,     1,     2,     1,     1,
       0,     1,     2,     4,     6,     6,     8,     5,     7,     4,
       2,     4,     5,     6,     5,     5,     6,     7,     7,     6,
       6,     8,     7,     4,     0,     7,     0,     7,     0,     2,
       4,     5,     5,     2,     4,     4,     1,     1,     1,     1,
       1,     1,     3,     2,     3,     3,     4,     3,     1,     4,
       1,     5,     5,     6,     7,     7,     8,     6,     6,     7,
       8,     8,     9,     2,     2,     3,     5,     4,     2,     2,
       3,     4,     5,     1,     1,     1,     1,     5,     2,     4,
       3,     4,     5,     7,     4,     6,     7,     0,     2,     2,
       4,     1,     3,     2,     0,     2,     1,     3,     2,     3,
       4,     5,     1,     3,     2,     4,     1,     3,     2,     0,
       2,     1,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     5,     5,
       0,     2,     1,     2,     3,     0,     3,     5,     3,     5,
       1,     3,     4,     0,     1,     1,     3,     1,     3,     5,
       0,     1,     1,     3,     1,     3,     4,     3,     2,     1,
       3,     0,     1,     1,     3,     2,     4,     4,     3,     3,
       2,     1,     3,     0,     1,     4,     5,     3,     4,     0,
       2,     1,     3,     0,     1,     1,     1,     0,     1,     1,
       2,     1,     2,     1,     2,     1,     1,     1,     2,     2,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       5,    44,     0,     0,     2,     3,     7,   272,   259,   261,
     263,   280,   281,   279,   274,   294,     0,   278,   276,   277,
     282,   265,   205,   273,     0,     0,   205,   144,   231,     0,
       0,     0,    37,     4,    33,    91,    88,   123,   100,   125,
     124,   141,    27,    44,    87,    89,    86,   175,    44,   202,
     186,   185,   177,   266,   267,   270,   178,   176,   126,   182,
     183,   184,   179,   181,   180,    98,    90,     0,     1,     6,
      14,     8,     0,   222,   224,   220,   213,   200,   260,   262,
     264,   271,   231,   128,     0,     0,   200,   291,   146,     0,
      29,     0,     0,     0,   287,   285,   286,   288,   289,   290,
     284,   283,     0,     0,    44,   229,   232,     0,   292,   293,
       0,    27,    93,   249,   243,     0,   161,   163,     0,     0,
     287,     0,   293,   137,    28,   143,    34,    36,   203,   269,
     268,     0,     0,    52,    46,    49,    48,   144,     0,    60,
      21,     0,    12,     0,   204,     0,     0,     0,     0,   221,
       0,    29,   215,    31,    44,    74,    76,    44,     0,    94,
      30,   145,   148,   114,     0,     0,     0,    95,   113,     0,
       0,    92,     0,     0,    97,   228,     0,    28,    45,     0,
     249,   243,   251,   197,   196,     0,    29,   187,   188,   192,
       0,   193,   194,   195,   189,   191,   190,     0,   241,   244,
       0,     0,     0,     0,     0,   249,     0,   152,     0,     0,
       0,    50,     0,     0,   142,   130,     0,   159,   159,    47,
       0,    50,   159,   159,     0,   156,     0,     0,    15,     0,
      13,    16,   223,   225,     0,     0,   206,     0,   208,    30,
       0,    32,     0,     0,   201,     0,     0,     0,   147,   149,
       0,   118,    96,   119,     0,   115,   227,     0,    35,     0,
     230,     0,     0,    53,   131,     0,     0,   245,    30,   250,
     249,     0,     0,   237,   240,   134,   162,   164,     0,     0,
       0,    99,     0,     0,     0,    50,   159,   159,     0,     0,
       0,    51,    73,     0,     0,   139,   138,   129,   154,     0,
       0,     0,     0,    59,    61,     0,     0,     0,    50,   158,
     288,    11,    22,     0,     0,    14,    21,   226,     0,     0,
     216,     0,     0,   218,   174,   173,   165,   166,   170,   171,
     172,   167,   169,   168,   199,     0,     0,   198,     0,   150,
     120,     0,   117,    50,   159,   159,   247,   238,   252,     0,
     239,   242,   246,   102,   101,     0,     0,   153,     0,     0,
     127,    65,     0,     0,    50,   132,    50,   159,   159,     0,
       0,     0,   160,    50,    57,    50,    64,   157,    62,     0,
       0,   205,   205,     0,   253,   253,   257,    17,     0,   207,
     209,     0,     0,     0,     0,    78,   176,     0,   151,   121,
     116,    55,     0,     0,   248,     0,     0,   103,   135,   108,
     107,     0,     0,    50,    69,    66,    70,     0,     0,    50,
     140,   155,     0,    54,    63,   205,   205,   200,   200,    16,
     254,   256,   255,     0,   233,     0,     0,   258,    29,   210,
       0,     9,   219,     0,     0,     0,    75,     0,    83,     0,
       0,    77,   122,    50,    58,   105,   104,     0,     0,     0,
     109,   136,    68,    50,    72,    67,   133,   200,   200,    40,
      40,    21,    19,   253,   235,    18,    20,   257,     0,     0,
      78,    78,    79,    78,     0,    56,   106,   111,   110,     0,
      71,    40,    40,   275,    42,    27,    42,     0,   234,     0,
     211,     0,    78,    85,    84,    80,    78,   112,    42,    42,
       0,     0,    38,    42,    41,     0,    10,   236,   212,    82,
      81,     0,     0,    27,    23,    39,    25,    24,    26,    43
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     3,     4,     5,    69,     6,   229,   143,   316,   387,
     227,   312,   125,   161,   242,    32,    33,   511,   494,   512,
      34,   291,   134,   292,    35,    36,   245,   246,   448,   394,
      37,    38,    39,    40,   102,   165,    41,   213,    42,   207,
      89,    90,   208,   218,   372,   225,   300,   115,   116,   323,
      44,    45,    46,   182,    47,   154,    48,   244,    77,   438,
     439,   150,   151,   152,   148,    72,    73,   105,   106,   107,
     433,   434,    50,   183,   198,   199,   200,    51,   184,   185,
     186,   435,   440,    52,    53,    54,    55,    56,    57,    58,
     495,    59,    60,    61,    62,    63,    64,    65,   108,   122,
     123,    66
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -361
static const yytype_int16 yypact[] =
{
     186,  1020,    22,    37,    22,    53,  -361,  -361,  -361,  -361,
    -361,  -361,  -361,  -361,  -361,  -361,    48,  -361,  -361,  -361,
    -361,  -361,    20,  -361,   313,  1696,     6,  1280,   752,  1696,
      48,    50,  -361,  -361,  -361,  -361,  -361,    -5,  -361,  -361,
    -361,  1867,    49,    14,  -361,  -361,  -361,  -361,  1072,  -361,
    -361,  -361,   112,   112,  -361,  -361,  -361,   211,   329,  -361,
    -361,  -361,  -361,  -361,  -361,  -361,  -361,   118,  -361,  -361,
      99,  -361,    24,  -361,    70,    48,    48,  -361,  -361,  -361,
    -361,  -361,   804,  -361,  1696,  1696,  -361,  -361,   125,   123,
     147,  1696,  1696,  1332,   313,  -361,  -361,  -361,  -361,  -361,
    -361,  -361,   166,    63,  1072,  -361,   178,   189,  -361,  1696,
     194,  1850,   206,  1796,  1748,   220,     3,  -361,   213,  1384,
    -361,    19,  -361,   221,  1696,  -361,  -361,  -361,  -361,  -361,
    -361,  1696,  1436,    43,  -361,  -361,  -361,  1280,   864,    43,
    -361,    22,    48,   233,  -361,    48,  1696,  1696,   267,   131,
     270,     7,  -361,   162,  1072,  -361,  -361,  1072,  1696,  -361,
    1280,  -361,  -361,  -361,  1696,   251,    66,  -361,  1696,  1696,
    1696,  -361,   252,  1280,  -361,  -361,   239,  -361,  -361,  1696,
    1796,  1748,  -361,  -361,  -361,   248,   255,   112,  -361,  -361,
     261,  -361,  -361,  -361,  -361,  -361,  -361,   281,  -361,   265,
     264,  1796,  1696,    48,    48,  1796,  1696,  -361,    60,  1488,
     916,   378,   244,  1696,  1867,  1867,  1696,   273,   274,  -361,
     269,    43,   273,   274,   136,  -361,  1696,   689,  -361,   280,
    -361,  -361,  -361,  1867,   277,    48,  -361,    48,  -361,  -361,
      48,  -361,   631,   284,  -361,   283,   291,   305,  -361,   317,
    1696,  -361,  -361,  1696,  1696,  -361,  -361,   332,  -361,   321,
    -361,  1696,  1124,    43,  1867,   308,   312,  -361,  1796,  -361,
    1796,  1796,  1748,  -361,  -361,  1867,  -361,  -361,   320,   325,
    1540,   337,  1696,  1696,   111,    43,   273,   274,   143,  1696,
     968,    43,  -361,   370,  1696,  -361,  -361,   335,  -361,  1176,
     326,   342,   331,  -361,  -361,   338,   339,   342,    43,  -361,
    1841,  -361,  -361,   347,   351,   366,   344,  -361,   349,   362,
    -361,  1696,    18,  -361,  -361,  -361,   112,  -361,  -361,  -361,
    -361,  -361,  -361,  -361,  -361,  1228,  1228,  -361,  1696,  -361,
    -361,   381,  1696,    43,   273,   274,  -361,  -361,  -361,   353,
    -361,  -361,  -361,   376,   379,   364,  1696,  -361,   380,  1592,
     395,  -361,   382,   383,    43,  1867,    43,   273,   274,   187,
     387,  1696,   393,    11,  -361,    43,  -361,  -361,  -361,   385,
     401,    20,    20,   403,    92,    92,   416,  -361,  1803,  -361,
    -361,   400,    48,   184,   405,   406,   302,   407,  -361,  1696,
    -361,  -361,   410,   411,  -361,  1696,  1696,   423,  1867,   425,
     426,   420,  1696,    43,  -361,  -361,  -361,   412,   419,    43,
    -361,  -361,  1696,  -361,  -361,    20,    20,  -361,  -361,  -361,
    -361,  -361,  -361,   175,  -361,    48,   226,  -361,   238,  -361,
      48,  -361,  -361,   441,  1696,  1696,  -361,  1228,  -361,  1696,
     449,  -361,  -361,    43,  -361,  1867,  1867,  1696,  1696,  1696,
     448,  1867,  -361,    43,  -361,  -361,  1867,  -361,  -361,   108,
     108,   344,  -361,    92,   450,  -361,  -361,   416,   451,  1696,
     406,   406,  -361,   406,  1696,  -361,  1867,  1867,  1867,  1696,
    -361,   108,   108,  -361,  1644,   437,  1644,  1818,  -361,   696,
    -361,   696,   406,  -361,  -361,  -361,   406,  1867,  1644,  1644,
    1696,   444,  -361,   434,  -361,   445,  -361,  -361,  -361,  -361,
    -361,   446,   452,  1850,  -361,  -361,  -361,  -361,  -361,  -361
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -361,  -361,  -361,  -361,  -361,   489,  -361,   181,    71,  -361,
    -307,  -361,  -110,  -146,  -361,   -30,  -361,  -308,  -360,    -8,
     458,   -54,  -114,   202,  -361,     5,  -361,  -361,  -252,  -324,
    -361,  -361,  -361,  -361,  -361,  -361,   -18,  -361,  -361,    -1,
     377,  -361,  -115,  -108,  -124,   208,  -147,  -361,  -361,  -255,
     145,  -361,  -361,   -82,  -361,   -84,   488,    30,   -20,  -361,
      40,  -361,  -361,   282,  -361,   453,   386,   354,  -361,  -361,
     139,    52,  -234,  -361,   262,  -361,   355,  -229,  -361,  -159,
    -361,  -361,  -361,   -64,  -361,  -361,   -14,    38,    29,   471,
    -361,   144,   279,   318,   352,   390,   399,  -361,  -212,   392,
     -12,  -361
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -218
static const yytype_int16 yytable[] =
{
      43,   178,   157,   133,   139,   240,    86,    83,   324,   388,
      81,   111,   397,   325,   224,   314,   110,   217,   127,   219,
      75,   265,     7,   222,   203,   219,    88,   103,  -214,    14,
     223,    49,   197,    14,    75,    22,    84,    68,   129,   130,
     269,   119,   422,    76,   238,    74,   278,    43,    23,   187,
     187,     7,   204,     7,    85,    26,   239,    76,    49,   112,
     117,    14,   135,   135,   113,   209,    29,    22,   114,   210,
     110,   302,   144,   145,   172,   305,   306,    23,   128,    23,
      81,   103,   280,   155,   156,   168,   288,    26,   253,     2,
     162,   163,   166,   169,   284,   286,   113,   124,   380,   197,
     114,   146,   287,    43,    74,   153,   214,   281,   175,   282,
     496,   349,   170,   215,   171,   254,   187,   187,   493,   274,
     147,    16,   263,   482,   243,   430,   431,   247,   233,   141,
     432,   508,   509,   359,   128,    21,    88,   187,   135,   362,
     363,   187,   189,   189,   135,   142,   234,   344,   295,   219,
     211,   188,   188,    43,   345,   158,    43,   248,   360,   249,
     282,   264,   140,   251,   497,  -217,   369,   255,   256,   257,
     159,   230,   259,   129,    74,   367,   314,   219,   326,   236,
     145,   135,   368,  -217,   275,   307,   348,   308,   515,   350,
     197,  -217,   307,   241,   364,   297,   160,   402,   403,  -217,
     521,   522,   136,   136,   187,   279,   187,   187,   187,   189,
     189,  -217,     1,   444,   445,   298,   135,   167,   188,   188,
     417,   418,     2,   472,   473,   309,   135,   173,   503,   504,
     189,   505,   276,   277,   189,    22,   307,   179,   419,   188,
     174,   296,   131,   188,   517,   176,   518,     7,     8,   339,
     519,   202,   340,   341,   520,    26,   313,   191,   191,   205,
     175,   132,   212,    22,   318,   324,   319,   324,   135,   153,
     325,   365,   325,    23,   475,   473,   293,   231,   136,   355,
     327,   357,   358,    26,   136,   314,   476,   239,   235,   262,
     135,   237,   477,   370,   294,   267,   135,   189,   357,   189,
     189,   189,   252,   258,   268,     7,   188,   270,   188,   188,
     188,   271,   129,   135,   272,   273,   303,    78,    79,    80,
     391,   136,   299,   301,   191,   191,    22,   315,   317,   334,
     449,    23,   335,   131,   395,   395,    21,   398,   408,   379,
     336,   400,   133,   469,   470,   191,    26,   338,   135,   191,
     337,   169,   132,    22,   342,   346,   136,   384,   411,   385,
     386,   427,   428,   347,   396,   396,   136,   352,   356,   135,
     421,   135,   353,    26,    78,   137,   121,   373,   135,   138,
     135,    87,   374,   491,   492,   514,   328,   455,   456,   375,
     376,   381,   192,   192,   461,   382,   141,   389,   452,   390,
     404,   135,    22,   399,   466,   467,   468,   405,   136,   289,
     406,   407,   191,   529,   191,   191,   191,   313,   135,    91,
     109,   443,    26,   304,   135,   450,   412,   409,   290,   425,
     136,   193,   193,   413,   414,   326,   136,   326,   420,   486,
     487,   488,   307,   480,   481,   426,   395,   429,   483,   437,
     446,   442,   451,   136,   457,   447,   458,   459,   135,   192,
     192,   453,   454,   463,   474,   194,   194,   460,   135,   478,
     464,   507,   479,    67,   109,    70,   396,   484,   502,   489,
     192,   499,   501,   506,   192,   177,   510,   361,   136,   524,
     526,   527,   523,   513,    71,   513,   383,   528,   193,   193,
     471,   126,   118,   195,   195,   525,   201,   513,   513,   136,
     378,   136,   196,   196,   220,   377,   104,   500,   136,   193,
     136,   329,   320,   193,   436,   498,   313,   260,   149,    91,
     226,   232,   194,   194,   351,     0,   266,   327,     0,   327,
       0,   136,     0,     0,     0,   401,     0,   192,     0,   192,
     192,   192,   250,   194,     0,     0,     0,   194,   136,     0,
     330,     0,     0,     0,   136,   261,   415,     0,   416,     0,
     195,   195,     0,   201,     0,   423,     0,   424,     0,   196,
     196,     0,     0,     0,   190,   190,   193,     0,   193,   193,
     193,   195,     0,     0,   331,   195,     0,     0,   136,     0,
     196,     0,   226,     0,   196,     0,     0,     0,   136,     0,
       0,     0,   228,     0,     0,   462,     0,     0,     0,     0,
     194,   465,   194,   194,   194,     0,     0,     0,     0,     0,
       0,     0,   332,     0,     0,     8,     9,    10,    11,    12,
      13,   333,     0,   328,     0,   328,     0,     0,    17,    18,
      19,   190,   190,     0,    21,   485,     0,     0,   195,     0,
     195,   195,   195,    24,   201,   490,     0,   196,     0,   196,
     196,   196,   190,     0,     0,     0,   190,     0,     0,     0,
       0,   321,   226,     0,     0,   322,     0,     0,     0,     0,
       0,   226,     7,   226,     0,     0,     0,     0,     0,   226,
       8,     9,    10,    11,    12,    13,     0,     0,     0,     0,
       0,     0,     0,    17,    18,    19,     0,     0,    23,    21,
       0,   120,    95,    96,   310,    98,    99,   100,    24,   101,
       0,     0,     0,     0,   311,     0,     0,     0,     0,   190,
       0,   190,   190,   190,     0,     0,     0,     0,     0,     0,
     322,     0,     0,     0,     0,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,    16,     0,     0,     0,    17,
      18,    19,    20,     0,    92,    21,    22,     0,   329,     0,
     329,    23,    93,     0,    94,    95,    96,    97,    98,    99,
     100,    87,   101,   118,    25,     0,    26,     0,    27,     0,
       0,     0,    82,     0,     0,    30,    31,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,   330,     0,   330,
       0,    17,    18,    19,    20,     0,    92,    21,    22,     0,
       0,     0,     0,    23,    93,     0,    94,    95,    96,    97,
      98,    99,   100,    87,   101,     0,    25,     0,    26,     0,
      27,   331,     0,   331,    82,     0,     0,    30,    31,     0,
       0,     0,     0,     0,     0,     0,     0,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,     0,     0,     0,
       0,    17,    18,    19,    20,     0,     0,    21,    22,   332,
       0,   332,     0,    23,     0,     0,    24,     0,   333,   216,
     333,     0,     0,    87,     0,     0,    25,     0,    26,     0,
      27,     0,     0,     0,    82,   221,     0,    30,    31,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,     0,
       0,     0,     0,    17,    18,    19,    20,     0,     0,    21,
      22,     0,     0,     0,     0,    23,     0,     0,    24,     0,
       0,   216,     0,     0,     0,    87,     0,     0,    25,     0,
      26,     0,    27,     0,     0,     0,    82,   285,     0,    30,
      31,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,     0,     0,     0,     0,    17,    18,    19,    20,     0,
       0,    21,    22,     0,     0,     0,     0,    23,     0,     0,
      24,     0,     0,   216,     0,     0,     0,    87,     0,     0,
      25,     0,    26,     0,    27,     0,     0,     0,    82,   366,
       0,    30,    31,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,    16,     0,     0,     0,    17,    18,    19,
      20,     0,     0,    21,    22,     0,     0,     0,     0,    23,
       0,     0,    24,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    25,     0,    26,     0,    27,     0,     0,     0,
      28,     0,    29,    30,    31,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,    16,     0,     0,     0,    17,
      18,    19,    20,     0,     0,    21,    22,     0,     0,     0,
       0,    23,     0,     0,    24,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    25,     0,    26,     0,    27,     0,
       0,     0,    82,     0,    29,    30,    31,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,     0,     0,     0,
       0,    17,    18,    19,    20,     0,     0,    21,    22,     0,
       0,     0,     0,    23,     0,     0,    24,     0,     0,   216,
       0,     0,     0,     0,     0,     0,    25,     0,    26,     0,
      27,     0,     0,     0,    82,   343,     0,    30,    31,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,     0,
       0,     0,     0,    17,    18,    19,    20,     0,     0,    21,
      22,     0,     0,     0,     0,    23,     0,     0,    24,     0,
       0,   371,     0,     0,     0,    87,     0,     0,    25,     0,
      26,     0,    27,     0,     0,     0,    82,     0,     0,    30,
      31,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,   392,     0,     0,     0,    17,    18,    19,    20,     0,
       0,    21,    22,     0,     0,     0,     0,    23,   393,     0,
      24,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      25,     0,    26,     0,    27,     0,     0,     0,    82,     0,
       0,    30,    31,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,     0,     0,     0,     0,    17,    18,    19,
      20,     0,     0,    21,    22,     0,     0,     0,     0,    23,
       0,     0,    24,     0,     0,     0,     0,     0,     0,    87,
       0,     0,    25,     0,    26,     0,    27,     0,     0,     0,
      82,     0,     0,    30,    31,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,     0,     0,     0,     0,    17,
      18,    19,    20,     0,   164,    21,    22,     0,     0,     0,
       0,    23,     0,     0,    24,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    25,     0,    26,     0,    27,     0,
       0,     0,    82,     0,     0,    30,    31,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,     0,     0,     0,
       0,    17,    18,    19,    20,     0,   206,    21,    22,     0,
       0,     0,     0,    23,     0,     0,    24,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    25,     0,    26,     0,
      27,     0,     0,     0,    82,     0,     0,    30,    31,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,     0,
       0,     0,     0,    17,    18,    19,    20,     0,     0,    21,
      22,     0,     0,     0,     0,    23,     0,     0,    24,     0,
       0,   216,     0,     0,     0,     0,     0,     0,    25,     0,
      26,     0,    27,     0,     0,     0,    82,     0,     0,    30,
      31,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,     0,     0,     0,     0,    17,    18,    19,    20,     0,
     283,    21,    22,     0,     0,     0,     0,    23,     0,     0,
      24,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      25,     0,    26,     0,    27,     0,     0,     0,    82,     0,
       0,    30,    31,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,     0,     0,     0,     0,    17,    18,    19,
      20,     0,     0,    21,    22,     0,     0,     0,     0,    23,
       0,     0,    24,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    25,     0,    26,     0,    27,   354,     0,     0,
      82,     0,     0,    30,    31,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,     0,     0,     0,     0,    17,
      18,    19,    20,     0,     0,    21,    22,     0,     0,     0,
       0,    23,     0,     0,    24,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    25,     0,    26,     0,    27,   410,
       0,     0,    82,     0,     0,    30,    31,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,     0,     0,     0,
       0,    17,    18,    19,    20,     0,     0,    21,    22,     0,
       0,     0,     0,    23,     0,     0,    24,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    25,     0,    26,     0,
      27,     0,     0,     0,    82,     0,   510,    30,    31,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,     0,
       0,     0,     0,    17,    18,    19,    20,     0,     0,    21,
      22,     0,     0,     0,     0,    23,     0,     0,    24,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    25,     0,
      26,     0,    27,     0,     0,     0,    82,     0,     0,    30,
      31,     7,     8,     9,    10,    11,    12,    13,     0,    14,
       0,     0,     0,     0,     0,    17,    18,    19,     0,     0,
       0,    21,     0,     0,     0,     0,     0,    23,     0,     0,
      24,     0,     0,     0,     0,     0,     0,    87,     0,     0,
       0,     0,     0,     0,   180,     0,     0,     0,   181,     7,
       8,     9,    10,    11,    12,    13,     7,    14,     0,     0,
       0,     0,     0,    17,    18,    19,     0,     0,     0,    21,
       0,     7,     0,     0,     0,    23,     0,     0,    24,     0,
       0,     0,    23,     0,     0,   120,    95,    96,   310,    98,
      99,   100,   180,   101,     7,     0,   181,    23,   441,     0,
     120,    95,    96,   310,    98,    99,   100,     0,   101,     0,
       0,     0,     0,   516,     0,     0,     0,     0,     0,     0,
      23,     0,     0,   120,    95,    96,    97,    98,    99,   100,
       0,   101,   120,    95,    96,    97,    98,    99,   100,    87,
     101,   121,     0,     0,     0,     0,     0,     0,   177,   120,
      95,    96,    97,    98,    99,   100,    87,   101,   121
};

static const yytype_int16 yycheck[] =
{
       1,   111,    86,    57,    58,   151,    26,    25,   242,   316,
      24,    29,   336,   242,   138,   227,    28,   132,    48,   133,
      14,   180,     3,   138,    21,   139,    27,    28,    21,    11,
     138,     1,   114,    11,    14,    24,    30,     0,    52,    53,
     186,    46,    31,    37,    37,    16,   205,    48,    29,   113,
     114,     3,    49,     3,    48,    44,    49,    37,    28,    30,
      31,    11,    57,    58,    46,    46,    52,    24,    50,    50,
      82,   218,    48,    49,   104,   222,   223,    29,    48,    29,
      94,    82,    22,    84,    85,    22,   210,    44,    22,    36,
      91,    92,    93,    30,   209,   210,    46,    48,   310,   181,
      50,    31,   210,   104,    75,    76,   124,    47,   109,    49,
     470,   270,    49,   131,    51,    49,   180,   181,    10,   201,
      50,    13,   176,   447,   154,    33,    34,   157,   146,    30,
      38,   491,   492,    22,   104,    23,   137,   201,   133,   286,
     287,   205,   113,   114,   139,    46,   147,   262,   212,   263,
     121,   113,   114,   154,   262,    30,   157,   158,    47,   160,
      49,   179,    44,   164,   471,     3,   290,   168,   169,   170,
      47,   142,   173,   187,   145,   290,   388,   291,   242,    48,
      49,   176,   290,    21,   202,    49,   268,    51,   496,   271,
     272,    29,    49,    31,    51,   213,    49,   344,   345,    37,
     508,   509,    57,    58,   268,   206,   270,   271,   272,   180,
     181,    49,    26,    29,    30,   216,   211,    51,   180,   181,
     367,   368,    36,    48,    49,   226,   221,    49,   480,   481,
     201,   483,   203,   204,   205,    24,    49,    31,    51,   201,
      51,   212,    31,   205,   499,    51,   501,     3,     4,   250,
     502,    31,   253,   254,   506,    44,   227,   113,   114,    46,
     261,    50,    41,    24,   235,   499,   237,   501,   263,   240,
     499,   289,   501,    29,    48,    49,    32,    44,   133,   280,
     242,   282,   283,    44,   139,   497,    48,    49,    21,    50,
     285,    21,   438,   294,    50,    47,   291,   268,   299,   270,
     271,   272,    51,    51,    49,     3,   268,    46,   270,   271,
     272,    30,   326,   308,    49,    51,    47,     4,     5,     6,
     321,   176,    49,    49,   180,   181,    24,    47,    51,    45,
      28,    29,    49,    31,   335,   336,    23,   338,   356,   310,
      49,   342,   396,   427,   428,   201,    44,    30,   343,   205,
      45,    30,    50,    24,    22,    47,   211,    13,   359,    15,
      16,   381,   382,    51,   335,   336,   221,    47,    31,   364,
     371,   366,    47,    44,     4,    46,    41,    51,   373,    50,
     375,    39,    51,   467,   468,   495,   242,   405,   406,    51,
      51,    44,   113,   114,   412,    44,    30,    48,   399,    37,
      47,   396,    24,    22,   422,   425,   426,    31,   263,    31,
      31,    47,   268,   523,   270,   271,   272,   388,   413,    27,
      28,   392,    44,   221,   419,   396,    31,    47,    50,    44,
     285,   113,   114,    51,    51,   499,   291,   501,    51,   457,
     458,   459,    49,   444,   445,    44,   447,    44,   449,    33,
      45,    51,    45,   308,    31,    49,    31,    31,   453,   180,
     181,    51,    51,    51,   435,   113,   114,    47,   463,   440,
      51,   489,    31,     2,    82,     4,   447,    28,   479,    31,
     201,    31,    31,   484,   205,    48,    52,   285,   343,    45,
      45,    45,   510,   494,     5,   496,   315,    45,   180,   181,
     429,    43,    31,   113,   114,   513,   114,   508,   509,   364,
     308,   366,   113,   114,   137,   307,    28,   477,   373,   201,
     375,   242,   240,   205,   385,   473,   497,   173,    75,   137,
     138,   145,   180,   181,   272,    -1,   181,   499,    -1,   501,
      -1,   396,    -1,    -1,    -1,   343,    -1,   268,    -1,   270,
     271,   272,   160,   201,    -1,    -1,    -1,   205,   413,    -1,
     242,    -1,    -1,    -1,   419,   173,   364,    -1,   366,    -1,
     180,   181,    -1,   181,    -1,   373,    -1,   375,    -1,   180,
     181,    -1,    -1,    -1,   113,   114,   268,    -1,   270,   271,
     272,   201,    -1,    -1,   242,   205,    -1,    -1,   453,    -1,
     201,    -1,   210,    -1,   205,    -1,    -1,    -1,   463,    -1,
      -1,    -1,   141,    -1,    -1,   413,    -1,    -1,    -1,    -1,
     268,   419,   270,   271,   272,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   242,    -1,    -1,     4,     5,     6,     7,     8,
       9,   242,    -1,   499,    -1,   501,    -1,    -1,    17,    18,
      19,   180,   181,    -1,    23,   453,    -1,    -1,   268,    -1,
     270,   271,   272,    32,   272,   463,    -1,   268,    -1,   270,
     271,   272,   201,    -1,    -1,    -1,   205,    -1,    -1,    -1,
      -1,    50,   290,    -1,    -1,    54,    -1,    -1,    -1,    -1,
      -1,   299,     3,   301,    -1,    -1,    -1,    -1,    -1,   307,
       4,     5,     6,     7,     8,     9,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    17,    18,    19,    -1,    -1,    29,    23,
      -1,    32,    33,    34,    35,    36,    37,    38,    32,    40,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    -1,    -1,   268,
      -1,   270,   271,   272,    -1,    -1,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    13,    -1,    -1,    -1,    17,
      18,    19,    20,    -1,    22,    23,    24,    -1,   499,    -1,
     501,    29,    30,    -1,    32,    33,    34,    35,    36,    37,
      38,    39,    40,   322,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,   499,    -1,   501,
      -1,    17,    18,    19,    20,    -1,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    -1,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    42,    -1,    44,    -1,
      46,   499,    -1,   501,    50,    -1,    -1,    53,    54,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    -1,    23,    24,   499,
      -1,   501,    -1,    29,    -1,    -1,    32,    -1,   499,    35,
     501,    -1,    -1,    39,    -1,    -1,    42,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    -1,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,
      24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    32,    -1,
      -1,    35,    -1,    -1,    -1,    39,    -1,    -1,    42,    -1,
      44,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,    53,
      54,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,
      -1,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,
      32,    -1,    -1,    35,    -1,    -1,    -1,    39,    -1,    -1,
      42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    51,
      -1,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    13,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,
      -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,
      50,    -1,    52,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    13,    -1,    -1,    -1,    17,
      18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,
      -1,    29,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    -1,    52,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    -1,    -1,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    -1,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,
      24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    32,    -1,
      -1,    35,    -1,    -1,    -1,    39,    -1,    -1,    42,    -1,
      44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,    53,
      54,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      12,    13,    -1,    -1,    -1,    17,    18,    19,    20,    -1,
      -1,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    -1,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,
      -1,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,
      -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    39,
      -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,
      50,    -1,    -1,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,
      18,    19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    -1,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,
      24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    32,    -1,
      -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,
      44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,    53,
      54,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,
      -1,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,
      -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    46,    47,    -1,    -1,
      50,    -1,    -1,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,
      18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,
      -1,    29,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,    47,
      -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    -1,    52,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    -1,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,
      24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    32,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,
      44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,    53,
      54,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      -1,    -1,    -1,    -1,    -1,    17,    18,    19,    -1,    -1,
      -1,    23,    -1,    -1,    -1,    -1,    -1,    29,    -1,    -1,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    -1,    50,     3,
       4,     5,     6,     7,     8,     9,     3,    11,    -1,    -1,
      -1,    -1,    -1,    17,    18,    19,    -1,    -1,    -1,    23,
      -1,     3,    -1,    -1,    -1,    29,    -1,    -1,    32,    -1,
      -1,    -1,    29,    -1,    -1,    32,    33,    34,    35,    36,
      37,    38,    46,    40,     3,    -1,    50,    29,    45,    -1,
      32,    33,    34,    35,    36,    37,    38,    -1,    40,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    32,    33,    34,    35,    36,    37,    38,
      -1,    40,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    -1,    -1,    -1,    -1,    -1,    -1,    48,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    26,    36,    56,    57,    58,    60,     3,     4,     5,
       6,     7,     8,     9,    11,    12,    13,    17,    18,    19,
      20,    23,    24,    29,    32,    42,    44,    46,    50,    52,
      53,    54,    70,    71,    75,    79,    80,    85,    86,    87,
      88,    91,    93,    94,   105,   106,   107,   109,   111,   112,
     127,   132,   138,   139,   140,   141,   142,   143,   144,   146,
     147,   148,   149,   150,   151,   152,   156,   144,     0,    59,
     144,    60,   120,   121,   143,    14,    37,   113,     4,     5,
       6,   141,    50,    91,    30,    48,   113,    39,    94,    95,
      96,   154,    22,    30,    32,    33,    34,    35,    36,    37,
      38,    40,    89,    94,   111,   122,   123,   124,   153,   154,
     155,    91,   143,    46,    50,   102,   103,   143,   144,    46,
      32,    41,   154,   155,    48,    67,    75,    70,   112,   141,
     141,    31,    50,    76,    77,    80,   105,    46,    50,    76,
      44,    30,    46,    62,    48,    49,    31,    50,   119,   120,
     116,   117,   118,   143,   110,    94,    94,   110,    30,    47,
      49,    68,    94,    94,    22,    90,    94,    51,    22,    30,
      49,    51,    70,    49,    51,    94,    51,    48,    67,    31,
      46,    50,   108,   128,   133,   134,   135,   138,   142,   143,
     144,   146,   147,   148,   149,   150,   151,   108,   129,   130,
     131,   154,    31,    21,    49,    46,    22,    94,    97,    46,
      50,   143,    41,    92,    91,    91,    35,    97,    98,    77,
      95,    51,    97,    98,    99,   100,   154,    65,   144,    61,
     143,    44,   121,    91,    94,    21,    48,    21,    37,    49,
      68,    31,    69,    70,   112,    81,    82,    70,    94,    94,
     154,    94,    51,    22,    49,    94,    94,    94,    51,    94,
     122,   154,    50,    76,    91,   134,   131,    47,    49,    68,
      46,    30,    49,    51,   108,    91,   143,   143,   134,    94,
      22,    47,    49,    22,    97,    51,    97,    98,    99,    31,
      50,    76,    78,    32,    50,   138,   143,    91,    94,    49,
     101,    49,   101,    47,    78,   101,   101,    49,    51,    94,
      35,    45,    66,   143,   153,    47,    63,    51,   143,   143,
     118,    50,    54,   104,   127,   132,   138,   142,   146,   147,
     148,   149,   150,   151,    45,    49,    49,    45,    30,    94,
      94,    94,    22,    51,    97,    98,    47,    51,   108,   134,
     108,   129,    47,    47,    47,    94,    31,    94,    94,    22,
      47,    78,   101,   101,    51,    91,    51,    97,    98,    99,
      94,    35,    99,    51,    51,    51,    51,   100,    78,   143,
     153,    44,    44,    62,    13,    15,    16,    64,    65,    48,
      37,    94,    13,    30,    84,    94,   143,    84,    94,    22,
      94,    78,   101,   101,    47,    31,    31,    47,    91,    47,
      47,    94,    31,    51,    51,    78,    78,   101,   101,    51,
      51,    94,    31,    78,    78,    44,    44,   113,   113,    44,
      33,    34,    38,   125,   126,   136,   125,    33,   114,   115,
     137,    45,    51,   143,    29,    30,    45,    49,    83,    28,
     143,    45,    94,    51,    51,    91,    91,    31,    31,    31,
      47,    91,    78,    51,    51,    78,    91,   113,   113,   110,
     110,    63,    48,    49,   143,    48,    48,    68,   143,    31,
      94,    94,    84,    94,    28,    78,    91,    91,    91,    31,
      78,   110,   110,    10,    73,   145,    73,    65,   126,    31,
     115,    31,    94,    83,    83,    83,    94,    91,    73,    73,
      52,    72,    74,    94,    67,    72,    45,   104,   104,    83,
      83,    72,    72,    91,    45,    74,    45,    45,    45,    67
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
		  Type, Value); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
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
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

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



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

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
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

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


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

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
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 41 "lang11d"
    { gRootParseNode = (PyrParseNode*)(yyvsp[(1) - (1)]); gParserResult = 1; ;}
    break;

  case 3:
#line 43 "lang11d"
    { gRootParseNode = (PyrParseNode*)(yyvsp[(1) - (1)]); gParserResult = 1; ;}
    break;

  case 4:
#line 45 "lang11d"
    { gRootParseNode = (PyrParseNode*)(yyvsp[(2) - (2)]); gParserResult = 2; ;}
    break;

  case 5:
#line 48 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 6:
#line 50 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 8:
#line 55 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 9:
#line 59 "lang11d"
    { (yyval) = (long)newPyrClassNode((PyrSlotNode*)(yyvsp[(1) - (6)]), (PyrSlotNode*)(yyvsp[(2) - (6)]), 
					(PyrVarListNode*)(yyvsp[(4) - (6)]), (PyrMethodNode*)(yyvsp[(5) - (6)]), 0); 
				;}
    break;

  case 10:
#line 63 "lang11d"
    { (yyval) = (long)newPyrClassNode((PyrSlotNode*)(yyvsp[(1) - (9)]), (PyrSlotNode*)(yyvsp[(5) - (9)]), 
					(PyrVarListNode*)(yyvsp[(7) - (9)]), (PyrMethodNode*)(yyvsp[(8) - (9)]), 
					(PyrSlotNode*)(yyvsp[(3) - (9)])); 
				;}
    break;

  case 11:
#line 70 "lang11d"
    { 
					(yyval) = (long)newPyrClassExtNode((PyrSlotNode*)(yyvsp[(2) - (5)]), (PyrMethodNode*)(yyvsp[(4) - (5)])); 
				;}
    break;

  case 12:
#line 75 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 14:
#line 79 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 15:
#line 81 "lang11d"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 16:
#line 84 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 17:
#line 86 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 18:
#line 90 "lang11d"
    { (yyval) = (long)newPyrVarListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), varClass); ;}
    break;

  case 19:
#line 92 "lang11d"
    { (yyval) = (long)newPyrVarListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), varInst); ;}
    break;

  case 20:
#line 94 "lang11d"
    { (yyval) = (long)newPyrVarListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), varConst); ;}
    break;

  case 21:
#line 97 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 22:
#line 99 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 23:
#line 103 "lang11d"
    { (yyval) = (long)newPyrMethodNode((PyrSlotNode*)(yyvsp[(1) - (7)]), (PyrSlotNode*)(yyvsp[(5) - (7)]), 
					(PyrArgListNode*)(yyvsp[(3) - (7)]), (PyrVarListNode*)(yyvsp[(4) - (7)]), (PyrParseNode*)(yyvsp[(6) - (7)]), 0); ;}
    break;

  case 24:
#line 106 "lang11d"
    { (yyval) = (long)newPyrMethodNode((PyrSlotNode*)(yyvsp[(2) - (8)]), (PyrSlotNode*)(yyvsp[(6) - (8)]), 
					(PyrArgListNode*)(yyvsp[(4) - (8)]), (PyrVarListNode*)(yyvsp[(5) - (8)]), (PyrParseNode*)(yyvsp[(7) - (8)]), 1); ;}
    break;

  case 25:
#line 109 "lang11d"
    { (yyval) = (long)newPyrMethodNode((PyrSlotNode*)(yyvsp[(1) - (7)]), (PyrSlotNode*)(yyvsp[(5) - (7)]), 
					(PyrArgListNode*)(yyvsp[(3) - (7)]), (PyrVarListNode*)(yyvsp[(4) - (7)]), (PyrParseNode*)(yyvsp[(6) - (7)]), 0); ;}
    break;

  case 26:
#line 112 "lang11d"
    { (yyval) = (long)newPyrMethodNode((PyrSlotNode*)(yyvsp[(2) - (8)]), (PyrSlotNode*)(yyvsp[(6) - (8)]), 
					(PyrArgListNode*)(yyvsp[(4) - (8)]), (PyrVarListNode*)(yyvsp[(5) - (8)]), (PyrParseNode*)(yyvsp[(7) - (8)]), 1); ;}
    break;

  case 34:
#line 130 "lang11d"
    { (yyval) = (long)newPyrDropNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 35:
#line 134 "lang11d"
    { (yyval) = (long)newPyrBlockNode(NULL, (PyrVarListNode*)(yyvsp[(2) - (4)]), (PyrParseNode*)(yyvsp[(3) - (4)]), false); ;}
    break;

  case 36:
#line 136 "lang11d"
    { (yyval) = (long)newPyrBlockNode(NULL, (PyrVarListNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)]), false); ;}
    break;

  case 37:
#line 138 "lang11d"
    { (yyval) = (long)newPyrBlockNode(NULL, NULL, (PyrParseNode*)(yyvsp[(1) - (1)]), false); ;}
    break;

  case 39:
#line 143 "lang11d"
    { (yyval) = (long)newPyrDropNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 40:
#line 146 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 41:
#line 148 "lang11d"
    { (yyval) = (yyvsp[(1) - (2)]); ;}
    break;

  case 42:
#line 152 "lang11d"
    { (yyval) = (long)newPyrReturnNode(NULL); ;}
    break;

  case 43:
#line 154 "lang11d"
    { (yyval) = (long)newPyrReturnNode((PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 44:
#line 158 "lang11d"
    { (yyval) = (long)newPyrBlockReturnNode(); ;}
    break;

  case 45:
#line 160 "lang11d"
    { (yyval) = (long)newPyrReturnNode((PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 47:
#line 165 "lang11d"
    { 
					(yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); 
				;}
    break;

  case 50:
#line 174 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 52:
#line 179 "lang11d"
    { 
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)]), 0, 0); 
			;}
    break;

  case 53:
#line 183 "lang11d"
    { 
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(2) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)]), 0, 0); 
			;}
    break;

  case 54:
#line 187 "lang11d"
    { 
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(1) - (6)]), (PyrParseNode*)(yyvsp[(3) - (6)]), 
						(PyrParseNode*)(yyvsp[(4) - (6)]), (PyrParseNode*)(yyvsp[(6) - (6)])); 
			;}
    break;

  case 55:
#line 192 "lang11d"
    {
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(2) - (6)]), NULL, NULL, (PyrParseNode*)(yyvsp[(6) - (6)])); 
			;}
    break;

  case 56:
#line 196 "lang11d"
    {
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(2) - (8)]), (PyrParseNode*)(yyvsp[(5) - (8)]), 
						(PyrParseNode*)(yyvsp[(6) - (8)]), (PyrParseNode*)(yyvsp[(8) - (8)])); 
			;}
    break;

  case 57:
#line 201 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				if (isSuperObjNode((PyrParseNode*)(yyvsp[(3) - (5)]))) {
					((PyrPushNameNode*)(yyvsp[(3) - (5)]))->mSlot.us = s_this;
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				selectornode = newPyrSlotNode(&slot);
				args = linkAfterHead(
					(PyrParseNode*)(yyvsp[(3) - (5)]),
					newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (5)]), NULL));
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(4) - (5)]), 0); 
			;}
    break;

  case 58:
#line 219 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_performList);
				selectornode = newPyrSlotNode(&slot);
				args = linkAfterHead(
					(PyrParseNode*)(yyvsp[(5) - (7)]),
					newPyrPushLitNode((PyrSlotNode*)(yyvsp[(2) - (7)]), NULL));
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(6) - (7)]), 0); 
			;}
    break;

  case 59:
#line 232 "lang11d"
    { (yyval) = (long)newPyrDynListNode((PyrParseNode*)(yyvsp[(1) - (4)]), (PyrParseNode*)(yyvsp[(3) - (4)])); ;}
    break;

  case 60:
#line 234 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (2)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, (PyrParseNode*)(yyvsp[(2) - (2)])); 
			;}
    break;

  case 61:
#line 245 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (4)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, NULL, (PyrParseNode*)(yyvsp[(4) - (4)])); 
			;}
    break;

  case 62:
#line 256 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (5)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(3) - (5)]), (PyrParseNode*)(yyvsp[(5) - (5)])); 
			;}
    break;

  case 63:
#line 267 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (6)])), 
					(PyrParseNode*)(yyvsp[(3) - (6)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(4) - (6)]), (PyrParseNode*)(yyvsp[(6) - (6)])); 
			;}
    break;

  case 64:
#line 280 "lang11d"
    {
				PyrSlotNode *selectornode, *selectornode2;
				PyrSlot slot, slot2;
				PyrParseNode* args;
				
				if (isSuperObjNode((PyrParseNode*)(yyvsp[(1) - (5)]))) {
					((PyrPushNameNode*)(yyvsp[(1) - (5)]))->mSlot.us = s_this;
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				SetSymbol(&slot2, s_new);
				selectornode = newPyrSlotNode(&slot);
				selectornode2 = newPyrSlotNode(&slot2);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (5)])),
					newPyrPushLitNode(selectornode2, NULL));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(3) - (5)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(5) - (5)]), 0); 
			;}
    break;

  case 65:
#line 301 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				
				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				(yyval) = (long)newPyrCallNode(selectornode, (PyrParseNode*)(yyvsp[(1) - (5)]), NULL, (PyrParseNode*)(yyvsp[(5) - (5)])); 
			;}
    break;

  case 66:
#line 310 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				
				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				(yyval) = (long)newPyrCallNode(selectornode, (PyrParseNode*)(yyvsp[(1) - (6)]), (PyrParseNode*)(yyvsp[(4) - (6)]), (PyrParseNode*)(yyvsp[(6) - (6)])); 
			;}
    break;

  case 67:
#line 319 "lang11d"
    { 
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(3) - (7)]), (PyrParseNode*)(yyvsp[(1) - (7)]), 
					(PyrParseNode*)(yyvsp[(5) - (7)]), (PyrParseNode*)(yyvsp[(7) - (7)])); 
			;}
    break;

  case 68:
#line 324 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[(1) - (7)]), 
					(PyrParseNode*)(yyvsp[(4) - (7)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(5) - (7)]), (PyrParseNode*)(yyvsp[(7) - (7)])); 
			;}
    break;

  case 69:
#line 337 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot, slot2;
				PyrParseNode* args;
				
				if (isSuperObjNode((PyrParseNode*)(yyvsp[(1) - (6)]))) {
					((PyrPushNameNode*)(yyvsp[(1) - (6)]))->mSlot.us = s_this;
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				SetSymbol(&slot2, s_value);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[(1) - (6)]),
					newPyrPushLitNode(newPyrSlotNode(&slot2), NULL));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(4) - (6)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(5) - (6)]), 0); 
			;}
    break;

  case 70:
#line 357 "lang11d"
    { 
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(3) - (6)]), (PyrParseNode*)(yyvsp[(1) - (6)]), NULL, (PyrParseNode*)(yyvsp[(6) - (6)])); 
			;}
    break;

  case 71:
#line 361 "lang11d"
    { 
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (8)]), (PyrParseNode*)(yyvsp[(5) - (8)]));
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(3) - (8)]), args, (PyrParseNode*)(yyvsp[(6) - (8)]), (PyrParseNode*)(yyvsp[(8) - (8)])); 
			;}
    break;

  case 72:
#line 367 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				if (isSuperObjNode((PyrParseNode*)(yyvsp[(1) - (7)]))) {
					((PyrPushNameNode*)(yyvsp[(1) - (7)]))->mSlot.us = s_this;
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				selectornode = newPyrSlotNode(&slot);

				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (7)]), newPyrPushLitNode((PyrSlotNode*)(yyvsp[(3) - (7)]), NULL));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(5) - (7)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(6) - (7)]), 0); 
			;}
    break;

  case 73:
#line 385 "lang11d"
    { 
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(3) - (4)]), (PyrParseNode*)(yyvsp[(1) - (4)]), 0, (PyrParseNode*)(yyvsp[(4) - (4)])); 
			;}
    break;

  case 74:
#line 390 "lang11d"
    { pushls(&generatorStack, (yyvsp[(3) - (3)])); pushls(&generatorStack, 1); ;}
    break;

  case 75:
#line 391 "lang11d"
    {
				PyrSlot slot;
				SetSymbol(&slot, getsym("r"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);
				
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)(yyvsp[(6) - (7)]), false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				(yyval) = (long)newPyrCallNode(selectornode, (PyrParseNode*)blocklit, 0, 0); 
			;}
    break;

  case 76:
#line 400 "lang11d"
    { pushls(&generatorStack, (yyvsp[(3) - (3)])); pushls(&generatorStack, 2); ;}
    break;

  case 77:
#line 401 "lang11d"
    {
				(yyval) = (yyvsp[(6) - (7)]);
			;}
    break;

  case 78:
#line 407 "lang11d"
    { 
					// innermost part
					int action = popls(&generatorStack); 
					PyrParseNode* expr = (PyrParseNode*)popls(&generatorStack);
					
					switch (action) 
					{
						case 1 :
						{
							PyrSlot slot;
							SetSymbol(&slot, getsym("yield"));
							PyrSlotNode* selectornode = newPyrSlotNode(&slot);
					
							(yyval) = (long)newPyrCallNode(selectornode, expr, 0, 0);
						} break;
						case 2 :
						{
							(yyval) = (long)expr;
						} break;
					}
				;}
    break;

  case 79:
#line 429 "lang11d"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 80:
#line 433 "lang11d"
    { 
				// later should check if exprseq is a series and optimize it to for loop
				PyrParseNode *exprseq = (PyrParseNode*)(yyvsp[(3) - (4)]);
				if (exprseq->mClassno == pn_CallNode) {
					PyrCallNode *callnode = (PyrCallNode*)exprseq;
					if (callnode->mSelector->mSlot.us == s_series)
					{
						SetSymbol(&callnode->mSelector->mSlot, getsym("forSeries"));

						PyrVarDefNode* var = newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (4)]), NULL, 0);
						PyrArgListNode* args = newPyrArgListNode(var, NULL);
						PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)(yyvsp[(4) - (4)]), false);
						PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
						
						callnode->mArglist = linkNextNode(callnode->mArglist, blocklit);
						(yyval) = (long)callnode; 
						
					} else goto notoptimized1;
				} else {
					notoptimized1:
					PyrSlot slot;
					SetSymbol(&slot, getsym("do"));
					PyrSlotNode* selectornode = newPyrSlotNode(&slot);

					PyrVarDefNode* var = newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (4)]), NULL, 0);
					PyrArgListNode* args = newPyrArgListNode(var, NULL);
					PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)(yyvsp[(4) - (4)]), false);
					PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
					
					PyrParseNode* args2 = linkNextNode(exprseq, blocklit);
					(yyval) = (long)newPyrCallNode(selectornode, args2, 0, 0); 
				}
			;}
    break;

  case 81:
#line 467 "lang11d"
    { 
				// later should check if exprseq is a series and optimize it to for loop
				PyrParseNode *exprseq = (PyrParseNode*)(yyvsp[(4) - (5)]);
				if (exprseq->mClassno == pn_CallNode) {
					PyrCallNode *callnode = (PyrCallNode*)exprseq;
					if (callnode->mSelector->mSlot.us == s_series)
					{
						SetSymbol(&callnode->mSelector->mSlot, getsym("forSeries"));

						PyrVarDefNode* var1 = newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (5)]), NULL, 0);
						PyrVarDefNode* var2 = newPyrVarDefNode((PyrSlotNode*)(yyvsp[(2) - (5)]), NULL, 0);
						PyrVarDefNode* vars = (PyrVarDefNode*)linkNextNode(var1, var2);
						PyrArgListNode* args = newPyrArgListNode(vars, NULL);
						PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)(yyvsp[(5) - (5)]), false);
						PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
						
						callnode->mArglist = linkNextNode(callnode->mArglist, blocklit);
						(yyval) = (long)callnode; 
						
					} else goto notoptimized2;
				} else {
					notoptimized2:
					PyrSlot slot;
					SetSymbol(&slot, getsym("do"));
					PyrSlotNode* selectornode = newPyrSlotNode(&slot);

					PyrVarDefNode* var1 = newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (5)]), NULL, 0);
					PyrVarDefNode* var2 = newPyrVarDefNode((PyrSlotNode*)(yyvsp[(2) - (5)]), NULL, 0);
					PyrVarDefNode* vars = (PyrVarDefNode*)linkNextNode(var1, var2);
					PyrArgListNode* args = newPyrArgListNode(vars, NULL);
					PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)(yyvsp[(5) - (5)]), false);
					PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
					
					PyrParseNode* args2 = linkNextNode(exprseq, blocklit);
					(yyval) = (long)newPyrCallNode(selectornode, args2, 0, 0); 
				}
			;}
    break;

  case 82:
#line 505 "lang11d"
    { 
				PyrSlot slot;
				SetSymbol(&slot, s_value);
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);

				PyrVarDefNode* var = newPyrVarDefNode((PyrSlotNode*)(yyvsp[(2) - (5)]), NULL, 0);
				PyrArgListNode* args = newPyrArgListNode(var, NULL);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)(yyvsp[(5) - (5)]), false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode(blocklit, (PyrParseNode*)(yyvsp[(4) - (5)]));
				
				(yyval) = (long)newPyrCallNode(selectornode, args2, 0, 0); 
			;}
    break;

  case 83:
#line 519 "lang11d"
    {
				PyrSlot slot;
				SetSymbol(&slot, getsym("if"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)(yyvsp[(2) - (2)]), false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), blocklit);

				(yyval) = (long)newPyrCallNode(selectornode, args2, 0, 0); 
			;}
    break;

  case 84:
#line 530 "lang11d"
    { 
				(yyval) = (long)newPyrDropNode((PyrParseNode*)(yyvsp[(3) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)])); 
			;}
    break;

  case 85:
#line 534 "lang11d"
    {
				PyrSlot slot;
				SetSymbol(&slot, getsym("alwaysYield"));
				PyrSlotNode* selectornode1 = newPyrSlotNode(&slot);
				
				SetSymbol(&slot, getsym("if"));
				PyrSlotNode* selectornode2 = newPyrSlotNode(&slot);
				
				SetNil(&slot);
				PyrParseNode *pushnil = (PyrParseNode*)newPyrPushLitNode(newPyrSlotNode(&slot), NULL);
				
				PyrParseNode *yieldNil = (PyrParseNode*)newPyrCallNode(selectornode1, pushnil, 0, 0); 
				
				PyrParseNode *block1 = (PyrParseNode*)newPyrBlockNode(0, 0, yieldNil, false);
				PyrParseNode *blocklit1 = (PyrParseNode*)newPyrPushLitNode(NULL, block1);
				PyrParseNode *block2 = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)(yyvsp[(4) - (4)]), false);
				PyrParseNode *blocklit2 = (PyrParseNode*)newPyrPushLitNode(NULL, block2);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)(yyvsp[(3) - (4)]), blocklit2);
				PyrParseNode* args3 = (PyrParseNode*)linkNextNode(args2, blocklit1);

				(yyval) = (long)newPyrCallNode(selectornode2, args3, 0, 0); 
			;}
    break;

  case 92:
#line 565 "lang11d"
    { 
				PyrParseNode* node = (PyrParseNode*)(yyvsp[(2) - (3)]); 
				node->mParens = 1;
				(yyval) = (yyvsp[(2) - (3)]); 
			;}
    break;

  case 93:
#line 571 "lang11d"
    { 
				PyrParseNode* argnode;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(2) - (2)]), NULL);
				SetSymbol(&slot, s_envirGet);
				selectornode = newPyrSlotNode(&slot);
				(yyval) = (long)newPyrCallNode(selectornode, argnode, 0, 0); 
			;}
    break;

  case 94:
#line 581 "lang11d"
    { (yyval) = (long)newPyrDynListNode(0, (PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 95:
#line 583 "lang11d"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 96:
#line 585 "lang11d"
    { (yyval) = (yyvsp[(3) - (4)]); ;}
    break;

  case 97:
#line 587 "lang11d"
    { (yyval) = (long)newPyrDynDictNode((PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 98:
#line 589 "lang11d"
    { (yyval) = (long)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 99:
#line 591 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[(1) - (4)]), 
					(PyrParseNode*)(yyvsp[(3) - (4)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 101:
#line 607 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)(yyvsp[(3) - (5)]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[(3) - (5)]));
					compileErrors++;
				}

				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				
				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (5)]), (PyrParseNode*)(yyvsp[(3) - (5)]));
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 102:
#line 634 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				
				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (5)]), nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(4) - (5)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 103:
#line 652 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)(yyvsp[(3) - (6)]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[(3) - (6)]));
					compileErrors++;
				}
				
				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (6)]), (PyrParseNode*)(yyvsp[(3) - (6)]));
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(5) - (6)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 104:
#line 679 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)(yyvsp[(3) - (7)]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[(3) - (7)]));
					compileErrors++;
				}

				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				
				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (7)]), (PyrParseNode*)(yyvsp[(3) - (7)]));
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(7) - (7)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 105:
#line 707 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				
				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (7)]), nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(4) - (7)]));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(7) - (7)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 106:
#line 726 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)(yyvsp[(3) - (8)]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[(3) - (8)]));
					compileErrors++;
				}
				
				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (8)]), (PyrParseNode*)(yyvsp[(3) - (8)]));
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(5) - (8)]));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(8) - (8)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 107:
#line 754 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;
				
				int arglen = nodeListLength((PyrParseNode*)(yyvsp[(4) - (6)]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[(3) - (6)]));
					compileErrors++;
				}
				
				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				
				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (6)]), (PyrParseNode*)(yyvsp[(4) - (6)]));
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 108:
#line 781 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				
				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (6)]), nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(5) - (6)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 109:
#line 799 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;
				
				int arglen = nodeListLength((PyrParseNode*)(yyvsp[(4) - (7)]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[(3) - (7)]));
					compileErrors++;
				}
				
				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (7)]), (PyrParseNode*)(yyvsp[(4) - (7)]));
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(6) - (7)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 110:
#line 824 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;
				
				int arglen = nodeListLength((PyrParseNode*)(yyvsp[(4) - (8)]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[(3) - (8)]));
					compileErrors++;
				}
				
				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				
				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (8)]), (PyrParseNode*)(yyvsp[(4) - (8)]));
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(8) - (8)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 111:
#line 852 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				
				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (8)]), nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(5) - (8)]));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(8) - (8)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 112:
#line 871 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;
				
				int arglen = nodeListLength((PyrParseNode*)(yyvsp[(4) - (9)]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[(3) - (9)]));
					compileErrors++;
				}
				
				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (9)]), (PyrParseNode*)(yyvsp[(4) - (9)]));
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(6) - (9)]));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(9) - (9)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 113:
#line 899 "lang11d"
    {
				// if this is not used in a 'do' or list comprehension, then should return an error.
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				
				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), nilnode);
				args = linkNextNode(args, nilnode2);
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 114:
#line 918 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode, *zeronode;
				PyrSlot selectorSlot, nilSlot, zeroSlot;
				PyrParseNode* args;

				SetInt(&zeroSlot, 0);
				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				zeronode = newPyrPushLitNode(newPyrSlotNode(&zeroSlot), NULL);
				
				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode(zeronode, nilnode);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(2) - (2)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 115:
#line 937 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				
				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), nilnode);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(3) - (3)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 116:
#line 954 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrSlot selectorSlot;
				PyrParseNode* args;
				
				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[(1) - (5)]), 
					(PyrParseNode*)(yyvsp[(3) - (5)]));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(5) - (5)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 117:
#line 968 "lang11d"
    { 
				// if this is not used in a 'do' or list comprehension, then should return an error.
				PyrSlotNode *selectornode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;
				PyrPushLitNode *nilnode;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				
				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[(1) - (4)]), 
					(PyrParseNode*)(yyvsp[(3) - (4)]));
				args = linkNextNode(args, nilnode);
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 118:
#line 989 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode, *zeronode;
				PyrSlot selectorSlot, nilSlot, zeroSlot;
				PyrParseNode* args;

				SetInt(&zeroSlot, 0);
				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				zeronode = newPyrPushLitNode(newPyrSlotNode(&zeroSlot), NULL);
				
				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode(zeronode, nilnode);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(2) - (2)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 119:
#line 1008 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				
				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), nilnode);
				args = linkNextNode(args, nilnode2);
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 120:
#line 1026 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				
				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), nilnode);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(3) - (3)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 121:
#line 1043 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
				
				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (4)]), (PyrParseNode*)(yyvsp[(3) - (4)]));
				args = linkNextNode(args, nilnode);
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 122:
#line 1059 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrSlot selectorSlot;
				PyrParseNode* args;
				
				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[(1) - (5)]), 
					(PyrParseNode*)(yyvsp[(3) - (5)]));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(5) - (5)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 126:
#line 1077 "lang11d"
    { (yyval) = (long)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 127:
#line 1079 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[(1) - (5)]), 
					(PyrParseNode*)(yyvsp[(4) - (5)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 128:
#line 1092 "lang11d"
    {
				PyrParseNode *node, *args;
				PyrSlotNode *slotnode;
				PyrSlot slot;
				
				SetSymbol(&slot, s_ref);
				slotnode = newPyrSlotNode(&slot);
				node = (PyrParseNode*)newPyrPushNameNode(slotnode);
				args = linkNextNode(node, (PyrParseNode*)(yyvsp[(2) - (2)]));
				SetSymbol(&slot, s_new);
				slotnode = newPyrSlotNode(&slot);
				(yyval) = (long)newPyrCallNode(slotnode, args, 0, 0); 
			;}
    break;

  case 129:
#line 1106 "lang11d"
    { 
				(yyval) = (long)newPyrBinopCallNode((PyrSlotNode*)(yyvsp[(2) - (4)]), 
						(PyrParseNode*)(yyvsp[(1) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)]), (PyrParseNode*)(yyvsp[(3) - (4)])); 
			;}
    break;

  case 130:
#line 1111 "lang11d"
    { 
				(yyval) = (long)newPyrAssignNode((PyrSlotNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)]), 0); 
			;}
    break;

  case 131:
#line 1115 "lang11d"
    { 
				PyrParseNode *argnode, *args;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(2) - (4)]), NULL);
				args = linkNextNode(argnode, (PyrParseNode*)(yyvsp[(4) - (4)]));
				SetSymbol(&slot, s_envirPut);
				selectornode = newPyrSlotNode(&slot);
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 132:
#line 1126 "lang11d"
    { 
				(yyval) = (long)newPyrSetterNode((PyrSlotNode*)(yyvsp[(3) - (5)]), 
						(PyrParseNode*)(yyvsp[(1) - (5)]), (PyrParseNode*)(yyvsp[(5) - (5)])); 
			;}
    break;

  case 133:
#line 1131 "lang11d"
    { 
				if ((yyvsp[(4) - (7)]) != 0) {
					error("Setter method called with keyword arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[(4) - (7)]));
					compileErrors++;
				}
				(yyval) = (long)newPyrSetterNode((PyrSlotNode*)(yyvsp[(1) - (7)]), 
						(PyrParseNode*)(yyvsp[(3) - (7)]), (PyrParseNode*)(yyvsp[(7) - (7)])); 
			;}
    break;

  case 134:
#line 1141 "lang11d"
    { 
				(yyval) = (long)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)(yyvsp[(2) - (4)]), 
					(PyrParseNode*)(yyvsp[(4) - (4)]), 0); 
			;}
    break;

  case 135:
#line 1146 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_put);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[(1) - (6)]), 
					(PyrParseNode*)(yyvsp[(3) - (6)]));
				args = linkNextNode( args, (PyrParseNode*)(yyvsp[(6) - (6)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 136:
#line 1160 "lang11d"
    { 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_put);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[(1) - (7)]), 
					(PyrParseNode*)(yyvsp[(4) - (7)]));
				args = linkNextNode( args, (PyrParseNode*)(yyvsp[(7) - (7)]));
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;}
    break;

  case 137:
#line 1175 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 138:
#line 1176 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(2) - (2)]), NULL); ;}
    break;

  case 139:
#line 1177 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(2) - (2)]), NULL); ;}
    break;

  case 140:
#line 1178 "lang11d"
    { (yyval) = (yyvsp[(3) - (4)]); ;}
    break;

  case 142:
#line 1183 "lang11d"
    { 
				(yyval) = (long)newPyrDropNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); 
			;}
    break;

  case 144:
#line 1191 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 145:
#line 1193 "lang11d"
    { (yyval) = (yyvsp[(1) - (2)]); ;}
    break;

  case 147:
#line 1198 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 148:
#line 1200 "lang11d"
    {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (2)]), NULL);
					(yyval) = (long)linkNextNode(key, (PyrParseNode*)(yyvsp[(2) - (2)])); 
				;}
    break;

  case 149:
#line 1205 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 150:
#line 1207 "lang11d"
    {
					PyrParseNode* elems;
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[(3) - (4)]), NULL);
					elems = (PyrParseNode*)linkNextNode(key, (PyrParseNode*)(yyvsp[(4) - (4)])); 
					(yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (4)]), elems); 
				;}
    break;

  case 151:
#line 1214 "lang11d"
    {
					PyrParseNode* elems;
					elems = (PyrParseNode*)linkNextNode((PyrParseNode*)(yyvsp[(3) - (5)]), (PyrParseNode*)(yyvsp[(5) - (5)])); 
					(yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (5)]), elems); 
				;}
    break;

  case 153:
#line 1223 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 154:
#line 1227 "lang11d"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 155:
#line 1229 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)])); ;}
    break;

  case 157:
#line 1234 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 158:
#line 1238 "lang11d"
    { (yyval) = (long)newPyrPushKeyArgNode((PyrSlotNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 159:
#line 1241 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 160:
#line 1242 "lang11d"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 161:
#line 1246 "lang11d"
    { (yyval) = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 162:
#line 1248 "lang11d"
    { (yyval) = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)(yyvsp[(1) - (3)]), (PyrSlotNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 164:
#line 1253 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 165:
#line 1257 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 166:
#line 1258 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 167:
#line 1259 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 168:
#line 1260 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 169:
#line 1261 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 170:
#line 1262 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 171:
#line 1263 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 172:
#line 1264 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 173:
#line 1265 "lang11d"
    { (yyval) = (long)newPyrLiteralNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 174:
#line 1266 "lang11d"
    { (yyval) = (long)newPyrLiteralNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 175:
#line 1269 "lang11d"
    { (yyval) = (long)newPyrPushLitNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 176:
#line 1272 "lang11d"
    { (yyval) = (long)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 177:
#line 1275 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 178:
#line 1276 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 179:
#line 1277 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 180:
#line 1278 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 181:
#line 1279 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 182:
#line 1280 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 183:
#line 1281 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 184:
#line 1282 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 185:
#line 1283 "lang11d"
    { (yyval) = (long)newPyrPushLitNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 186:
#line 1284 "lang11d"
    { (yyval) = (long)newPyrPushLitNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 187:
#line 1287 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 188:
#line 1288 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 189:
#line 1289 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 190:
#line 1290 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 191:
#line 1291 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 192:
#line 1292 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 193:
#line 1293 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 194:
#line 1294 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 195:
#line 1295 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 196:
#line 1296 "lang11d"
    { (yyval) = (long)newPyrLiteralNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 197:
#line 1297 "lang11d"
    { (yyval) = (long)newPyrLiteralNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 198:
#line 1301 "lang11d"
    { (yyval) = (long)newPyrBlockNode((PyrArgListNode*)(yyvsp[(2) - (5)]), (PyrVarListNode*)(yyvsp[(3) - (5)]), 
					(PyrParseNode*)(yyvsp[(4) - (5)]), false); ;}
    break;

  case 199:
#line 1304 "lang11d"
    { (yyval) = (long)newPyrBlockNode((PyrArgListNode*)(yyvsp[(2) - (5)]), (PyrVarListNode*)(yyvsp[(3) - (5)]), 
					(PyrParseNode*)(yyvsp[(4) - (5)]), true); ;}
    break;

  case 200:
#line 1308 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 201:
#line 1310 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 203:
#line 1315 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 204:
#line 1319 "lang11d"
    { (yyval) = (long)newPyrVarListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), varLocal); ;}
    break;

  case 205:
#line 1322 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 206:
#line 1324 "lang11d"
    {
					(yyval) = (long)newPyrArgListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), NULL); 
				;}
    break;

  case 207:
#line 1328 "lang11d"
    { 
					(yyval) = (long)newPyrArgListNode((PyrVarDefNode*)(yyvsp[(2) - (5)]), (PyrSlotNode*)(yyvsp[(4) - (5)])); 
				;}
    break;

  case 208:
#line 1332 "lang11d"
    {
					(yyval) = (long)newPyrArgListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), NULL); 
				;}
    break;

  case 209:
#line 1336 "lang11d"
    { 
					(yyval) = (long)newPyrArgListNode((PyrVarDefNode*)(yyvsp[(2) - (5)]), (PyrSlotNode*)(yyvsp[(4) - (5)])); 
				;}
    break;

  case 211:
#line 1343 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 212:
#line 1347 "lang11d"
    { (yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(2) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)]), (yyvsp[(1) - (4)])); ;}
    break;

  case 213:
#line 1350 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 216:
#line 1356 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 217:
#line 1360 "lang11d"
    { (yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL, 0); ;}
    break;

  case 218:
#line 1362 "lang11d"
    { (yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)]), 0); ;}
    break;

  case 219:
#line 1364 "lang11d"
    {
					PyrParseNode* node = (PyrParseNode*)(yyvsp[(4) - (5)]); 
					node->mParens = 1;
					(yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (5)]), node, 0);
				;}
    break;

  case 220:
#line 1371 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 223:
#line 1377 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 224:
#line 1381 "lang11d"
    { (yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL, 0); ;}
    break;

  case 225:
#line 1383 "lang11d"
    { (yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)]), 0); ;}
    break;

  case 226:
#line 1385 "lang11d"
    { 
									PyrParseNode* node = (PyrParseNode*)(yyvsp[(3) - (4)]); 
									node->mParens = 1;
									(yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (4)]), node, 0); 
								;}
    break;

  case 227:
#line 1393 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 228:
#line 1395 "lang11d"
    {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (2)]), NULL);
					(yyval) = (long)linkNextNode(key, (PyrParseNode*)(yyvsp[(2) - (2)])); 
				;}
    break;

  case 230:
#line 1403 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 231:
#line 1406 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 234:
#line 1412 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 235:
#line 1416 "lang11d"
    { (yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(2) - (2)]), NULL, (yyvsp[(1) - (2)])); ;}
    break;

  case 236:
#line 1418 "lang11d"
    { (yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(2) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)]), (yyvsp[(1) - (4)])); ;}
    break;

  case 237:
#line 1422 "lang11d"
    { (yyval) = (long)newPyrLitDictNode((PyrParseNode*)(yyvsp[(3) - (4)])); ;}
    break;

  case 238:
#line 1426 "lang11d"
    { (yyval) = (long)newPyrLitDictNode((PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 239:
#line 1430 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 240:
#line 1432 "lang11d"
    {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (2)]), NULL);
					(yyval) = (long)linkNextNode(key, (PyrParseNode*)(yyvsp[(2) - (2)])); 
				;}
    break;

  case 242:
#line 1440 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 243:
#line 1443 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 245:
#line 1450 "lang11d"
    { (yyval) = (long)newPyrLitListNode(0, (PyrParseNode*)(yyvsp[(3) - (4)])); ;}
    break;

  case 246:
#line 1452 "lang11d"
    { (yyval) = (long)newPyrLitListNode((PyrParseNode*)(yyvsp[(2) - (5)]), (PyrParseNode*)(yyvsp[(4) - (5)])); ;}
    break;

  case 247:
#line 1456 "lang11d"
    { (yyval) = (long)newPyrLitListNode(0, (PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 248:
#line 1458 "lang11d"
    { (yyval) = (long)newPyrLitListNode((PyrParseNode*)(yyvsp[(1) - (4)]), (PyrParseNode*)(yyvsp[(3) - (4)])); ;}
    break;

  case 249:
#line 1461 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 252:
#line 1467 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 253:
#line 1470 "lang11d"
    { (yyval) = rwPrivate; ;}
    break;

  case 254:
#line 1472 "lang11d"
    { (yyval) = rwReadOnly; ;}
    break;

  case 255:
#line 1474 "lang11d"
    { (yyval) = rwReadWrite; ;}
    break;

  case 256:
#line 1476 "lang11d"
    { (yyval) = rwWriteOnly; ;}
    break;

  case 257:
#line 1479 "lang11d"
    { (yyval) = rwPrivate; ;}
    break;

  case 258:
#line 1481 "lang11d"
    { (yyval) = rwReadOnly; ;}
    break;

  case 259:
#line 1484 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 260:
#line 1486 "lang11d"
    {
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->mSlot.ui = -node->mSlot.ui;
				(yyval) = zzval;
			;}
    break;

  case 261:
#line 1494 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 262:
#line 1496 "lang11d"
    {
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->mSlot.uf = -node->mSlot.uf;
				(yyval) = zzval;
			;}
    break;

  case 263:
#line 1504 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 264:
#line 1506 "lang11d"
    {
					PyrSlotNode *node; 
					double intval, fracval;
					node = (PyrSlotNode*)zzval;
					intval = floor(node->mSlot.uf + 0.5);
					fracval = node->mSlot.uf - intval;
					node->mSlot.uf = -intval + fracval;
					(yyval) = zzval;
				;}
    break;

  case 265:
#line 1516 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 268:
#line 1522 "lang11d"
    {
				PyrSlotNode *node; 
				node = (PyrSlotNode*)(yyvsp[(1) - (2)]);
				node->mSlot.uf *= pi;
			;}
    break;

  case 269:
#line 1528 "lang11d"
    {
				PyrSlotNode *node; 
				double ival;
				node = (PyrSlotNode*)(yyvsp[(1) - (2)]);
				ival = node->mSlot.ui;
				node->mSlot.uf = ival * pi;
			;}
    break;

  case 270:
#line 1536 "lang11d"
    {
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->mSlot.uf = pi;
				(yyval) = zzval;
			;}
    break;

  case 271:
#line 1543 "lang11d"
    {
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->mSlot.uf = -pi;
				(yyval) = zzval;
			;}
    break;

  case 272:
#line 1551 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 273:
#line 1552 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 274:
#line 1555 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 275:
#line 1558 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 276:
#line 1561 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 277:
#line 1564 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 278:
#line 1567 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 279:
#line 1570 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 280:
#line 1573 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 281:
#line 1576 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 282:
#line 1579 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 283:
#line 1582 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 284:
#line 1583 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 285:
#line 1584 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 286:
#line 1585 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 287:
#line 1586 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 288:
#line 1587 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 289:
#line 1588 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 290:
#line 1589 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 291:
#line 1592 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 294:
#line 1599 "lang11d"
    { (yyval) = zzval; ;}
    break;


/* Line 1267 of yacc.c.  */
#line 4313 "lang11d_tab.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


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



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
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



