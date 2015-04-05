
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
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
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

void bcopy(void *src, void *dst, size_t size) ;
int yyparse();
extern bool compilingCmdLine;
extern LongStack generatorStack;




/* Line 189 of yacc.c  */
#line 95 "lang11d_tab.cpp"

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



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 170 "lang11d_tab.cpp"

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
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
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
#define YYLAST   1945

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  101
/* YYNRULES -- Number of rules.  */
#define YYNRULES  292
/* YYNRULES -- Number of states.  */
#define YYNSTATES  534

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
     155,   156,   158,   161,   166,   171,   178,   185,   194,   200,
     208,   213,   216,   221,   228,   235,   241,   247,   255,   264,
     272,   279,   286,   295,   303,   308,   309,   317,   318,   326,
     327,   330,   335,   341,   347,   350,   355,   360,   362,   364,
     366,   368,   370,   372,   376,   379,   383,   387,   392,   396,
     398,   403,   405,   411,   417,   424,   432,   440,   449,   456,
     463,   471,   480,   489,   499,   502,   505,   509,   515,   520,
     523,   526,   530,   535,   541,   543,   545,   547,   549,   555,
     558,   563,   567,   572,   578,   586,   591,   598,   606,   607,
     610,   613,   618,   620,   624,   627,   628,   631,   633,   637,
     640,   644,   649,   655,   657,   661,   664,   669,   671,   675,
     678,   680,   684,   686,   690,   692,   696,   698,   700,   702,
     704,   706,   708,   710,   712,   714,   716,   718,   720,   722,
     724,   726,   728,   730,   732,   734,   736,   738,   740,   742,
     744,   746,   748,   750,   752,   754,   756,   758,   764,   770,
     771,   774,   776,   779,   783,   784,   788,   794,   798,   804,
     806,   810,   815,   816,   818,   820,   824,   826,   830,   836,
     837,   839,   841,   845,   847,   851,   856,   860,   863,   865,
     869,   870,   873,   875,   879,   882,   887,   891,   895,   898,
     900,   904,   905,   908,   913,   919,   923,   928,   929,   932,
     934,   938,   939,   941,   943,   945,   946,   948,   950,   953,
     955,   958,   960,   963,   965,   967,   969,   972,   975,   977,
     980,   982,   984,   986,   988,   990,   992,   994,   996,   998,
    1000,  1002,  1004,  1006,  1008,  1010,  1012,  1014,  1016,  1018,
    1020,  1022,  1024
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      56,     0,    -1,    57,    -1,    58,    -1,    26,    71,    -1,
      -1,    57,    59,    -1,    60,    -1,    58,    60,    -1,   143,
      62,    44,    63,    65,    45,    -1,   143,    46,    61,    47,
      62,    44,    63,    65,    45,    -1,    36,   143,    44,    65,
      45,    -1,    -1,   142,    -1,    -1,    30,   143,    -1,    -1,
      63,    64,    -1,    15,   125,    48,    -1,    13,   125,    48,
      -1,    16,   114,    48,    -1,    -1,    65,    66,    -1,   142,
      44,   113,   110,    73,    72,    45,    -1,    35,   142,    44,
     113,   110,    73,    72,    45,    -1,   152,    44,   113,   110,
      73,    72,    45,    -1,    35,   152,    44,   113,   110,    73,
      72,    45,    -1,    -1,    48,    -1,    -1,    49,    -1,    -1,
      31,    -1,    75,    -1,    94,    75,    -1,    50,   111,    70,
      51,    -1,   111,    70,    -1,    70,    -1,    74,    -1,    94,
      74,    -1,    -1,   144,    67,    -1,    -1,    52,    91,    67,
      -1,    -1,    52,    91,    67,    -1,    77,    -1,    76,    77,
      -1,   105,    -1,    80,    -1,    -1,    76,    -1,   142,    76,
      -1,    50,   154,    51,    76,    -1,   142,    50,    51,    76,
      -1,   142,    50,    97,   101,    51,    78,    -1,    50,   154,
      51,    50,    51,    76,    -1,    50,   154,    51,    50,    97,
     101,    51,    78,    -1,   142,    50,    98,   101,    51,    -1,
      50,   154,    51,    50,    98,   101,    51,    -1,   143,    46,
      95,    47,    -1,   143,    76,    -1,   143,    50,    51,    78,
      -1,   143,    50,    99,    68,    51,    78,    -1,   143,    50,
      97,   101,    51,    78,    -1,   143,    50,    98,   101,    51,
      -1,    91,    41,    50,    51,    78,    -1,    91,    41,    50,
      99,    68,    51,    78,    -1,    91,    41,   142,    50,    99,
      68,    51,    78,    -1,    91,    41,    50,    97,   101,    51,
      78,    -1,    91,    41,    50,    98,   101,    51,    -1,    91,
      41,   142,    50,    51,    78,    -1,    91,    41,   142,    50,
      97,   101,    51,    78,    -1,    91,    41,   142,    50,    98,
     101,    51,    -1,    91,    41,   142,    78,    -1,    -1,    44,
      30,    94,    81,    49,    84,    45,    -1,    -1,    44,    48,
      94,    82,    49,    84,    45,    -1,    -1,    49,    84,    -1,
     142,    28,    94,    83,    -1,   142,   142,    28,    94,    83,
      -1,    13,   142,    31,    94,    83,    -1,    94,    83,    -1,
      30,    30,    94,    83,    -1,    30,    29,    94,    83,    -1,
     107,    -1,   105,    -1,    80,    -1,   106,    -1,   155,    -1,
      79,    -1,    50,    94,    51,    -1,    53,   142,    -1,    46,
      95,    47,    -1,    50,    89,    51,    -1,    50,    30,    90,
      51,    -1,    50,   124,    51,    -1,   151,    -1,    85,    46,
      97,    47,    -1,    86,    -1,    85,    46,    97,    22,    47,
      -1,    85,    46,    22,    94,    47,    -1,    85,    46,    97,
      22,    94,    47,    -1,    85,    46,    97,    22,    47,    31,
      91,    -1,    85,    46,    22,    94,    47,    31,    91,    -1,
      85,    46,    97,    22,    94,    47,    31,    91,    -1,    91,
      41,    46,    97,    22,    47,    -1,    91,    41,    46,    22,
      94,    47,    -1,    91,    41,    46,    97,    22,    94,    47,
      -1,    91,    41,    46,    97,    22,    47,    31,    91,    -1,
      91,    41,    46,    22,    94,    47,    31,    91,    -1,    91,
      41,    46,    97,    22,    94,    47,    31,    91,    -1,    94,
      22,    -1,    22,    94,    -1,    94,    22,    94,    -1,    94,
      49,    94,    22,    94,    -1,    94,    49,    94,    22,    -1,
      22,    94,    -1,    94,    22,    -1,    94,    22,    94,    -1,
      94,    49,    94,    22,    -1,    94,    49,    94,    22,    94,
      -1,    85,    -1,    88,    -1,    87,    -1,   143,    -1,    91,
      41,    46,    97,    47,    -1,    42,    91,    -1,    91,   154,
      92,    91,    -1,   142,    31,    91,    -1,    53,   142,    31,
      91,    -1,    91,    41,   142,    31,    91,    -1,   142,    50,
      97,   101,    51,    31,    91,    -1,    54,   102,    31,    91,
      -1,    85,    46,    97,    47,    31,    91,    -1,    91,    41,
      46,    97,    47,    31,    91,    -1,    -1,    41,   142,    -1,
      41,   137,    -1,    41,    50,    94,    51,    -1,    91,    -1,
      93,    48,    91,    -1,    93,    67,    -1,    -1,    96,    68,
      -1,    94,    -1,    94,    30,    94,    -1,   153,    94,    -1,
      96,    49,    94,    -1,    96,    49,   153,    94,    -1,    96,
      49,    94,    30,    94,    -1,    94,    -1,    97,    49,    94,
      -1,    35,    94,    -1,    97,    49,    35,    94,    -1,   100,
      -1,    99,    49,   100,    -1,   153,    94,    -1,    68,    -1,
      49,    99,    68,    -1,   103,    -1,   103,    21,   142,    -1,
     142,    -1,   103,    49,   142,    -1,   137,    -1,   141,    -1,
     148,    -1,   150,    -1,   149,    -1,   145,    -1,   146,    -1,
     147,    -1,   131,    -1,   109,    -1,   142,    -1,   137,    -1,
     141,    -1,   148,    -1,   150,    -1,   149,    -1,   145,    -1,
     146,    -1,   147,    -1,   131,    -1,   137,    -1,   141,    -1,
     148,    -1,   150,    -1,   149,    -1,   142,    -1,   145,    -1,
     146,    -1,   147,    -1,   132,    -1,   127,    -1,    44,   113,
     110,    70,    45,    -1,    24,   113,   110,    70,    45,    -1,
      -1,   110,   112,    -1,   112,    -1,   111,   112,    -1,    13,
     120,    48,    -1,    -1,    14,   120,    48,    -1,    14,   119,
      21,   142,    48,    -1,    37,   117,    37,    -1,    37,   116,
      21,   142,    37,    -1,   115,    -1,   114,    68,   115,    -1,
     136,   142,    31,   104,    -1,    -1,   117,    -1,   118,    -1,
     117,    68,   118,    -1,   142,    -1,   142,    69,   104,    -1,
     142,    69,    50,    94,    51,    -1,    -1,   120,    -1,   121,
      -1,   120,    49,   121,    -1,   142,    -1,   142,    31,    91,
      -1,   142,    50,    94,    51,    -1,    94,    30,    94,    -1,
     153,    94,    -1,   122,    -1,   123,    49,   122,    -1,    -1,
     123,    68,    -1,   126,    -1,   125,    49,   126,    -1,   135,
     142,    -1,   135,   142,    31,   104,    -1,    50,   130,    51,
      -1,   108,    30,   108,    -1,   153,   108,    -1,   128,    -1,
     129,    49,   128,    -1,    -1,   129,    68,    -1,    54,    46,
     133,    47,    -1,    54,   143,    46,   133,    47,    -1,    46,
     133,    47,    -1,   143,    46,   133,    47,    -1,    -1,   134,
      68,    -1,   108,    -1,   134,    49,   108,    -1,    -1,    33,
      -1,    38,    -1,    34,    -1,    -1,    33,    -1,     4,    -1,
      32,     4,    -1,     5,    -1,    32,     5,    -1,     6,    -1,
      32,     6,    -1,    23,    -1,   138,    -1,   139,    -1,   138,
     140,    -1,   137,   140,    -1,   140,    -1,    32,   140,    -1,
       3,    -1,    29,    -1,    11,    -1,    10,    -1,    18,    -1,
      19,    -1,    17,    -1,     9,    -1,     7,    -1,     8,    -1,
      20,    -1,    40,    -1,    38,    -1,    33,    -1,    34,    -1,
      32,    -1,    35,    -1,    36,    -1,    37,    -1,    39,    -1,
     152,    -1,   153,    -1,    12,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    39,    39,    41,    43,    47,    48,    52,    53,    57,
      61,    68,    74,    75,    78,    79,    83,    84,    88,    90,
      92,    96,    97,   101,   104,   107,   110,   115,   116,   119,
     120,   123,   124,   127,   128,   132,   134,   136,   140,   141,
     145,   146,   151,   152,   157,   158,   162,   163,   169,   170,
     173,   174,   177,   181,   185,   189,   194,   198,   203,   221,
     234,   236,   247,   258,   269,   282,   303,   312,   321,   326,
     339,   359,   363,   369,   387,   393,   393,   403,   403,   410,
     431,   435,   469,   507,   521,   532,   536,   561,   562,   563,
     564,   565,   566,   567,   573,   583,   585,   587,   589,   591,
     593,   606,   609,   636,   654,   681,   709,   728,   756,   783,
     801,   826,   854,   873,   901,   920,   939,   956,   970,   991,
    1010,  1028,  1045,  1061,  1077,  1078,  1079,  1080,  1081,  1094,
    1108,  1113,  1117,  1128,  1133,  1143,  1148,  1162,  1178,  1179,
    1180,  1181,  1184,  1185,  1191,  1194,  1195,  1199,  1200,  1202,
    1207,  1209,  1216,  1224,  1225,  1229,  1231,  1235,  1236,  1240,
    1244,  1245,  1248,  1250,  1254,  1255,  1260,  1261,  1262,  1263,
    1264,  1265,  1266,  1267,  1268,  1271,  1274,  1277,  1278,  1279,
    1280,  1281,  1282,  1283,  1284,  1285,  1288,  1289,  1290,  1291,
    1292,  1293,  1294,  1295,  1296,  1297,  1298,  1301,  1304,  1309,
    1310,  1314,  1315,  1319,  1323,  1324,  1328,  1332,  1336,  1342,
    1343,  1347,  1351,  1352,  1355,  1356,  1360,  1362,  1364,  1372,
    1373,  1376,  1377,  1381,  1383,  1385,  1393,  1395,  1402,  1403,
    1407,  1408,  1411,  1412,  1416,  1418,  1422,  1426,  1428,  1435,
    1436,  1440,  1441,  1446,  1448,  1452,  1454,  1458,  1459,  1462,
    1463,  1467,  1468,  1470,  1472,  1476,  1477,  1481,  1482,  1491,
    1492,  1501,  1502,  1513,  1516,  1517,  1518,  1524,  1532,  1539,
    1548,  1549,  1552,  1555,  1558,  1561,  1564,  1567,  1570,  1573,
    1576,  1579,  1580,  1581,  1582,  1583,  1584,  1585,  1586,  1589,
    1592,  1593,  1596
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
  "blocklist", "msgsend", "generator", "$@1", "$@2", "nextqual", "qual",
  "expr1", "valrangex1", "valrangeassign", "valrangexd", "valrange2",
  "valrange3", "expr", "adverb", "exprn", "exprseq", "arrayelems",
  "arrayelems1", "arglist1", "arglistv1", "keyarglist1", "keyarg",
  "optkeyarglist", "mavars", "mavarlist", "slotliteral", "blockliteral",
  "pushname", "pushliteral", "listliteral", "block", "funcvardecls",
  "funcvardecls1", "funcvardecl", "argdecls", "constdeflist", "constdef",
  "slotdeflist0", "slotdeflist", "slotdef", "vardeflist0", "vardeflist",
  "vardef", "dictslotdef", "dictslotlist1", "dictslotlist",
  "rwslotdeflist", "rwslotdef", "dictlit2", "litdictslotdef",
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
      79,    79,    79,    79,    79,    81,    80,    82,    80,    83,
      83,    84,    84,    84,    84,    84,    84,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    86,    86,    86,    87,    87,    87,    88,    88,
      88,    88,    88,    88,    89,    89,    89,    89,    89,    90,
      90,    90,    90,    90,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    91,    91,    91,    91,    91,    92,    92,
      92,    92,    93,    93,    94,    95,    95,    96,    96,    96,
      96,    96,    96,    97,    97,    98,    98,    99,    99,   100,
     101,   101,   102,   102,   103,   103,   104,   104,   104,   104,
     104,   104,   104,   104,   104,   105,   106,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   109,   109,   110,
     110,   111,   111,   112,   113,   113,   113,   113,   113,   114,
     114,   115,   116,   116,   117,   117,   118,   118,   118,   119,
     119,   120,   120,   121,   121,   121,   122,   122,   123,   123,
     124,   124,   125,   125,   126,   126,   127,   128,   128,   129,
     129,   130,   130,   131,   131,   132,   132,   133,   133,   134,
     134,   135,   135,   135,   135,   136,   136,   137,   137,   138,
     138,   139,   139,   140,   141,   141,   141,   141,   141,   141,
     142,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   152,   152,   152,   152,   152,   152,   152,   153,
     154,   154,   155
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     0,     2,     1,     2,     6,
       9,     5,     0,     1,     0,     2,     0,     2,     3,     3,
       3,     0,     2,     7,     8,     7,     8,     0,     1,     0,
       1,     0,     1,     1,     2,     4,     2,     1,     1,     2,
       0,     2,     0,     3,     0,     3,     1,     2,     1,     1,
       0,     1,     2,     4,     4,     6,     6,     8,     5,     7,
       4,     2,     4,     6,     6,     5,     5,     7,     8,     7,
       6,     6,     8,     7,     4,     0,     7,     0,     7,     0,
       2,     4,     5,     5,     2,     4,     4,     1,     1,     1,
       1,     1,     1,     3,     2,     3,     3,     4,     3,     1,
       4,     1,     5,     5,     6,     7,     7,     8,     6,     6,
       7,     8,     8,     9,     2,     2,     3,     5,     4,     2,
       2,     3,     4,     5,     1,     1,     1,     1,     5,     2,
       4,     3,     4,     5,     7,     4,     6,     7,     0,     2,
       2,     4,     1,     3,     2,     0,     2,     1,     3,     2,
       3,     4,     5,     1,     3,     2,     4,     1,     3,     2,
       1,     3,     1,     3,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     5,     5,     0,
       2,     1,     2,     3,     0,     3,     5,     3,     5,     1,
       3,     4,     0,     1,     1,     3,     1,     3,     5,     0,
       1,     1,     3,     1,     3,     4,     3,     2,     1,     3,
       0,     2,     1,     3,     2,     4,     3,     3,     2,     1,
       3,     0,     2,     4,     5,     3,     4,     0,     2,     1,
       3,     0,     1,     1,     1,     0,     1,     1,     2,     1,
       2,     1,     2,     1,     1,     1,     2,     2,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       5,    44,     0,     0,     2,     3,     7,   270,   257,   259,
     261,   278,   279,   277,   272,   292,     0,   276,   274,   275,
     280,   263,   204,   271,     0,     0,   204,   145,   230,     0,
       0,     0,    37,     4,    33,    92,    89,   124,   101,   126,
     125,   142,    27,    44,    88,    90,    87,   175,    44,   201,
     185,   177,   264,   265,   268,   178,   176,   127,   182,   183,
     184,   179,   181,   180,    99,    91,     0,     1,     6,    14,
       8,     0,   221,   223,   219,   212,   199,   258,   260,   262,
     269,   230,   129,     0,     0,   199,   289,   147,     0,    29,
       0,     0,     0,   285,   283,   284,   286,   287,   288,   282,
     281,     0,     0,    44,   228,    29,     0,   290,   291,     0,
      27,    94,   247,     0,   162,   164,     0,     0,   285,     0,
     291,   138,    28,   144,    34,    36,   202,   267,   266,     0,
       0,    52,    46,    49,    48,   145,     0,    61,    21,     0,
      12,     0,   203,     0,     0,     0,     0,   220,     0,    29,
     214,    31,    44,    75,    77,    44,     0,    95,    30,   146,
     149,   115,     0,     0,     0,    96,   114,     0,     0,    93,
       0,    30,   231,    98,   227,     0,    28,    45,     0,   247,
     241,   249,   196,   195,     0,    29,   186,   187,   191,     0,
     192,   193,   194,   188,   190,   189,     0,     0,     0,   247,
       0,   153,     0,     0,     0,    50,     0,     0,   143,   131,
       0,     0,    29,    29,    47,     0,    50,    29,    29,    29,
     157,     0,     0,    15,     0,    13,    16,   222,   224,     0,
       0,   205,     0,   207,    30,     0,    32,     0,     0,   200,
       0,     0,     0,   148,   150,     0,   119,    97,   120,     0,
     116,   226,     0,    35,     0,   229,     0,     0,    53,   132,
       0,     0,   239,    29,     0,     0,   243,    30,   248,   247,
     135,   163,   165,     0,     0,     0,   100,     0,     0,     0,
      50,    29,    29,    29,     0,     0,    51,    74,     0,     0,
     140,   139,   130,   155,    54,    30,   160,     0,    30,     0,
      60,    62,     0,     0,    30,     0,   159,   286,    11,    22,
       0,     0,    14,    21,   225,     0,     0,   215,     0,     0,
     217,   174,   166,   167,   171,   172,   173,   168,   170,   169,
     198,     0,     0,   197,     0,   151,   121,     0,   118,     0,
      29,    29,   245,     0,    30,   242,   236,   238,   250,     0,
     244,   103,   102,     0,     0,   154,     0,     0,   128,    66,
       0,     0,     0,   133,    50,    29,    29,    29,     0,     0,
      29,    50,    58,    50,    65,   158,    50,     0,     0,   204,
     204,     0,   251,   251,   255,    17,     0,   206,   208,     0,
       0,     0,     0,    79,   176,     0,   152,   122,   117,    56,
       0,     0,   237,   240,   246,     0,     0,   104,   136,   109,
     108,     0,     0,    50,    70,    50,    71,     0,     0,     0,
     141,   156,   161,     0,    55,    64,    63,   204,   204,   199,
     199,    16,   252,   254,   253,     0,   232,     0,     0,   256,
      29,   209,     0,     9,   218,     0,     0,     0,    76,     0,
      84,     0,     0,    78,   123,    50,    59,   106,   105,     0,
       0,     0,   110,   137,    69,    67,    50,    73,    50,   134,
     199,   199,    40,    40,    21,    19,   251,   234,    18,    20,
     255,     0,     0,    79,    79,    80,    79,     0,    57,   107,
     112,   111,     0,    72,    68,    40,    40,   273,    42,    27,
      42,     0,   233,     0,   210,     0,    79,    86,    85,    81,
      79,   113,    42,    42,     0,     0,    38,    42,    41,     0,
      10,   235,   211,    83,    82,     0,     0,    27,    23,    39,
      25,    24,    26,    43
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     3,     4,     5,    68,     6,   224,   141,   313,   385,
     222,   309,   123,   296,   237,    32,    33,   515,   498,   516,
      34,   286,   132,   287,    35,    36,   240,   241,   450,   392,
      37,    38,    39,    40,   101,   163,    41,   207,    42,   201,
      88,    89,   202,   213,   370,   220,   297,   113,   114,   320,
      44,    45,    46,   181,    47,   152,    48,   239,    76,   440,
     441,   148,   149,   150,   146,    71,    72,   104,   105,   106,
     435,   436,   182,   262,   263,   264,    50,   183,   184,   185,
     437,   442,    51,    52,    53,    54,    55,    56,    57,   499,
      58,    59,    60,    61,    62,    63,    64,   107,   120,   121,
      65
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -358
static const yytype_int16 yypact[] =
{
       6,  1013,    28,   131,    28,    57,  -358,  -358,  -358,  -358,
    -358,  -358,  -358,  -358,  -358,  -358,    17,  -358,  -358,  -358,
    -358,  -358,    64,  -358,   236,  1689,    40,  1325,   733,  1689,
      17,   147,  -358,  -358,  -358,  -358,  -358,    89,  -358,  -358,
    -358,  1904,    90,   104,  -358,  -358,  -358,  -358,  1065,  -358,
    -358,   118,   118,  -358,  -358,  -358,   163,    93,  -358,  -358,
    -358,  -358,  -358,  -358,  -358,  -358,   120,  -358,  -358,    38,
    -358,   -24,  -358,    68,    17,    17,  -358,  -358,  -358,  -358,
    -358,   857,  -358,  1689,  1689,  -358,  -358,   154,   150,   151,
    1689,  1689,  1377,   236,  -358,  -358,  -358,  -358,  -358,  -358,
    -358,   160,    15,  1065,  -358,   174,   183,  -358,  1689,   184,
    1887,   196,  1818,   221,    12,  -358,   197,  1429,  -358,    19,
    -358,   213,  1689,  -358,  -358,  -358,  -358,  -358,  -358,  1689,
    1117,    92,  -358,  -358,  -358,  1325,   793,    92,  -358,    28,
      17,   218,  -358,    17,  1689,  1689,   243,   -18,   247,    74,
    -358,   200,  1065,  -358,  -358,  1065,  1689,  -358,  1325,  -358,
    -358,  -358,  1689,   224,    18,  -358,  1689,  1689,  1689,  -358,
     229,  1325,  -358,  -358,  -358,   188,  -358,  -358,  1689,  1818,
    1770,  -358,  -358,  -358,   235,   246,   118,  -358,  -358,   252,
    -358,  -358,  -358,  -358,  -358,  -358,  1689,    17,    17,  1818,
    1689,  -358,    51,  1481,   909,   186,    47,  1689,  1904,  1904,
    1689,    92,   260,   261,  -358,   245,    92,   260,   261,   262,
    -358,  1689,  1825,  -358,   254,  -358,  -358,  -358,  1904,   264,
      17,  -358,    17,  -358,  -358,    17,  -358,  1740,   267,  -358,
     269,   270,   271,  -358,   290,  1689,  -358,  -358,  1689,  1689,
    -358,  -358,   299,  -358,   292,  -358,  1689,  1169,    92,  1904,
     278,   296,  -358,   279,   276,  1818,  -358,  1818,  -358,  1818,
    1904,  -358,  -358,   282,   285,  1533,   303,  1689,  1689,    83,
      92,   260,   261,   262,  1689,   961,    92,  -358,   335,  1689,
    -358,  -358,   307,  -358,    92,  1221,  -358,   293,   312,   313,
    -358,  -358,   314,   318,   312,   319,  -358,  1878,  -358,  -358,
     310,   311,   342,   176,  -358,   325,   338,  -358,  1689,     7,
    -358,  -358,   118,  -358,  -358,  -358,  -358,  -358,  -358,  -358,
    -358,  1273,  1273,  -358,  1689,  -358,  -358,   356,  1689,    92,
     260,   261,  -358,  1818,  1770,  -358,  -358,  -358,  -358,   336,
    -358,   351,   357,   337,  1689,  -358,   345,  1585,   358,  -358,
     343,   344,   346,  1904,    92,   260,   261,   262,   347,  1689,
     262,   202,  -358,    92,  -358,  -358,    92,   355,   363,    64,
      64,   364,   266,   266,   377,  -358,  1840,  -358,  -358,   350,
      17,   116,   367,   370,    31,   372,  -358,  1689,  -358,    92,
     369,   371,  -358,  -358,  -358,  1689,  1689,   392,  1904,   393,
     394,   379,  1689,    92,  -358,    92,  -358,   378,   380,   382,
    -358,  -358,  -358,  1689,  -358,  -358,  -358,    64,    64,  -358,
    -358,  -358,  -358,  -358,  -358,   212,  -358,    17,   230,  -358,
     257,  -358,    17,  -358,  -358,   397,  1689,  1689,  -358,  1273,
    -358,  1689,   408,  -358,  -358,    92,  -358,  1904,  1904,  1689,
    1689,  1689,   406,  1904,  -358,  -358,    92,  -358,    92,  1904,
    -358,  -358,    96,    96,   176,  -358,   266,   412,  -358,  -358,
     377,   413,  1689,   370,   370,  -358,   370,  1689,  -358,  1904,
    1904,  1904,  1689,  -358,  -358,    96,    96,  -358,  1637,   391,
    1637,  1863,  -358,   647,  -358,   647,   370,  -358,  -358,  -358,
     370,  1904,  1637,  1637,  1689,   402,  -358,   400,  -358,   404,
    -358,  -358,  -358,  -358,  -358,   411,   414,  1887,  -358,  -358,
    -358,  -358,  -358,  -358
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -358,  -358,  -358,  -358,  -358,   448,  -358,   145,    27,  -358,
    -311,  -358,  -109,   -77,  -358,   -40,  -358,  -255,  -271,   -57,
     418,   -42,   -96,   263,  -358,    91,  -358,  -358,  -357,  -327,
    -358,  -358,  -358,  -358,  -358,  -358,   -19,  -358,  -358,    -1,
     328,  -358,  -117,  -133,  -100,   165,  -161,  -358,  -358,  -289,
     216,  -358,  -358,  -171,  -358,   -68,   443,    10,   -22,  -358,
      -3,  -358,  -358,   237,  -358,   415,   339,   320,  -358,  -358,
      95,    22,  -358,   149,  -358,  -358,  -214,  -358,  -150,  -358,
    -358,  -358,     2,  -358,  -358,    -8,    16,   144,   511,  -358,
      71,   201,   255,   304,   353,   368,  -358,  -215,   523,    -7,
    -358
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -217
static const yytype_int16 yytable[] =
{
      43,   177,   386,   218,    85,   395,    82,   311,   125,   261,
     110,    49,   159,   212,   131,   137,    80,   155,    14,   217,
       7,   109,     7,   321,   142,   143,    87,   102,   172,   260,
     231,   143,     1,   197,     7,   214,   219,   166,    49,    14,
     248,   214,     2,   127,   128,   167,    23,    43,    23,   273,
       7,     8,   299,   112,    74,    22,   302,   303,   126,   451,
      23,   198,   129,   170,   168,   203,   169,   249,   139,   204,
      83,   282,   235,   275,   109,    26,    23,    75,    74,   288,
     102,   130,   153,   154,   140,    80,   279,   281,    84,   160,
     161,   164,   378,     2,   347,  -213,   348,   289,   276,   144,
     277,    75,    43,   208,   283,   357,   497,   174,   268,    16,
     209,   233,   238,   126,   186,   242,    22,    22,   145,   349,
     360,   361,   485,   234,   341,   228,   507,   508,   187,   509,
     358,    67,   277,   258,    87,   117,    26,    26,   122,   135,
     340,    21,   305,   136,   229,   446,   447,   133,   133,   523,
       7,    43,   366,   524,    43,   243,    29,   244,    14,   259,
      73,   246,   214,   501,   138,   250,   251,   252,   365,   294,
     254,   311,   402,   261,   111,   115,    23,   270,   127,   400,
     401,   186,   186,   190,   156,   367,   345,    22,   292,   382,
     214,   383,   384,   112,   129,   187,   187,   157,   214,   274,
     158,   186,   500,  -216,   417,   418,   362,    26,   290,   293,
      22,   165,    22,   130,   521,   187,   522,   284,    73,   151,
     306,  -216,   133,   171,   512,   513,    22,   178,   133,  -216,
      26,   236,    26,   423,   173,   175,   285,  -216,   257,   322,
      77,    78,    79,   199,   335,   519,    26,   336,   337,  -216,
     190,   190,   196,   323,   206,   174,   188,   525,   526,    21,
     475,   476,   226,   205,   230,   363,   133,   186,   232,   186,
     190,   186,   134,   134,   353,   247,   355,   356,   478,   476,
     253,   187,   266,   187,   225,   187,   311,    73,   368,   321,
     419,   321,   300,   422,   355,   267,   133,   399,   269,   432,
     433,   312,   133,   214,   434,   479,   234,   133,   324,   295,
     298,   304,   330,   191,   127,   314,   333,   389,   331,   332,
     334,   338,   167,   188,   188,   342,   343,   346,   344,   350,
     393,   393,   351,   396,   354,   408,   190,   398,   190,    77,
     190,   271,   272,   188,   371,   186,   186,   134,   119,   133,
     291,    86,   131,   134,   379,   380,   411,   429,   430,   187,
     187,   472,   473,   480,   372,   373,   310,   192,   421,   374,
     376,   133,   139,   387,   315,   388,   316,   133,   397,   151,
     191,   191,   405,   404,   407,   133,   457,   458,   406,   412,
     518,   134,   409,   463,   413,   414,   454,   415,   420,   427,
     191,   444,   495,   496,   469,   470,   471,   428,   431,   188,
     439,   188,   448,   188,   190,   190,   193,   453,   533,   449,
     455,   134,   456,   459,   460,   461,   462,   134,   482,   466,
     133,   467,   134,   468,   192,   192,   487,   492,   325,   176,
     489,   490,   491,   503,   505,   483,   484,   528,   393,   530,
     486,   377,   514,    70,   192,   133,   531,   381,   474,   532,
     529,   124,   133,   215,   133,   194,   191,   133,   191,   375,
     191,   103,   317,   511,   134,   394,   394,   504,   438,   301,
     195,   506,   227,   193,   193,   133,   510,   188,   188,   147,
     133,   255,   326,   403,     0,   527,   134,   517,   502,   517,
       0,     0,   134,   193,   133,   322,   133,   322,     0,     0,
     134,   517,   517,    66,     0,    69,     0,     0,     0,   323,
     192,   323,   192,     0,   192,     0,     0,     0,     0,     0,
     310,     0,   194,   194,   445,     0,     0,     0,   452,     0,
       0,   327,   116,   359,   191,   191,   133,   195,   195,     0,
      90,   108,   194,     0,     0,   134,     0,   133,     0,   133,
       0,     0,     0,     0,     0,     0,     0,   195,     0,   193,
       0,   193,     0,   193,   324,     0,   324,     0,     0,     0,
     134,   477,     0,     0,     0,     0,   481,   134,     0,   134,
     328,     0,   134,   394,     0,     0,     0,     0,   192,   192,
       0,     0,     0,     0,   108,   329,     0,     0,     0,     0,
     134,     0,     0,     0,     0,   134,     0,     0,   194,     0,
     194,     0,   194,   189,     0,     0,     0,   416,     0,   134,
       0,   134,     0,   195,   424,   195,   425,   195,     0,   426,
       0,     0,     0,     0,     0,   310,     0,   193,   193,     0,
     223,     8,     9,    10,    11,    12,    13,     0,    90,   221,
       0,     0,     0,     0,    17,    18,    19,     0,     0,     0,
      21,   134,     0,     0,     0,     0,   464,     0,   465,    24,
       0,   245,   134,     0,   134,     0,     0,     0,     0,     0,
     189,   189,     0,     0,   256,     0,   194,   194,     0,     0,
       0,   319,     0,   265,   325,     0,   325,     0,     0,     0,
     189,   195,   195,     0,     0,     0,     0,     0,   488,     0,
       0,     0,     0,     0,     0,     0,     0,   221,     0,   493,
       0,   494,     0,     0,     0,     0,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,    16,     0,     0,     0,
      17,    18,    19,    20,     0,    91,    21,    22,   326,     0,
     326,     0,    23,    92,     0,    93,    94,    95,    96,    97,
      98,    99,    86,   100,     0,    25,   189,    26,   189,    27,
     189,     0,     0,    81,     0,     0,    30,    31,     0,     0,
       0,     0,     0,     0,     0,     0,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,     0,   327,   221,   327,
      17,    18,    19,    20,     0,     0,    21,    22,   221,     0,
       0,   221,    23,     0,     0,    24,     0,   221,   210,     0,
     116,     0,    86,     0,     0,    25,     0,    26,     0,    27,
       0,     0,     0,    81,   216,     0,    30,    31,     0,     0,
       0,     0,     0,     0,   189,   189,   328,     0,   328,     0,
       7,     8,     9,    10,    11,    12,    13,   265,    14,    15,
       0,   329,     0,   329,    17,    18,    19,    20,     0,    91,
      21,    22,     0,     0,     0,     0,    23,    92,     0,    93,
      94,    95,    96,    97,    98,    99,    86,   100,     0,    25,
       0,    26,     0,    27,     0,     0,     0,    81,     0,     0,
      30,    31,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    17,    18,    19,    20,
       0,     0,    21,    22,     0,     0,     0,     0,    23,     0,
       0,    24,     0,     0,   210,     0,     0,     0,    86,     0,
       0,    25,     0,    26,     0,    27,     0,     0,     0,    81,
     280,     0,    30,    31,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    17,    18,
      19,    20,     0,     0,    21,    22,     0,     0,     0,     0,
      23,     0,     0,    24,     0,     0,   210,     0,     0,     0,
      86,     0,     0,    25,     0,    26,     0,    27,     0,     0,
       0,    81,   364,     0,    30,    31,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,    16,     0,     0,     0,
      17,    18,    19,    20,     0,     0,    21,    22,     0,     0,
       0,     0,    23,     0,     0,    24,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    25,     0,    26,     0,    27,
       0,     0,     0,    28,     0,    29,    30,    31,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,    16,     0,
       0,     0,    17,    18,    19,    20,     0,     0,    21,    22,
       0,     0,     0,     0,    23,     0,     0,    24,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    25,     0,    26,
       0,    27,     0,     0,     0,    81,     0,    29,    30,    31,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    17,    18,    19,    20,     0,     0,
      21,    22,     0,     0,     0,     0,    23,     0,     0,    24,
       0,     0,   210,     0,     0,     0,     0,     0,     0,    25,
       0,    26,     0,    27,     0,     0,     0,    81,   211,     0,
      30,    31,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    17,    18,    19,    20,
       0,     0,    21,    22,     0,     0,     0,     0,    23,     0,
       0,    24,     0,     0,   210,     0,     0,     0,     0,     0,
       0,    25,     0,    26,     0,    27,     0,     0,     0,    81,
     339,     0,    30,    31,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    17,    18,
      19,    20,     0,     0,    21,    22,     0,     0,     0,     0,
      23,     0,     0,    24,     0,     0,   369,     0,     0,     0,
      86,     0,     0,    25,     0,    26,     0,    27,     0,     0,
       0,    81,     0,     0,    30,    31,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,   390,     0,     0,     0,
      17,    18,    19,    20,     0,     0,    21,    22,     0,     0,
       0,     0,    23,   391,     0,    24,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    25,     0,    26,     0,    27,
       0,     0,     0,    81,     0,     0,    30,    31,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,     0,     0,
       0,     0,    17,    18,    19,    20,     0,     0,    21,    22,
       0,     0,     0,     0,    23,     0,     0,    24,     0,     0,
       0,     0,     0,     0,    86,     0,     0,    25,     0,    26,
       0,    27,     0,     0,     0,    81,     0,     0,    30,    31,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    17,    18,    19,    20,     0,   162,
      21,    22,     0,     0,     0,     0,    23,     0,     0,    24,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    25,
       0,    26,     0,    27,     0,     0,     0,    81,     0,     0,
      30,    31,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    17,    18,    19,    20,
       0,   200,    21,    22,     0,     0,     0,     0,    23,     0,
       0,    24,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    25,     0,    26,     0,    27,     0,     0,     0,    81,
       0,     0,    30,    31,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    17,    18,
      19,    20,     0,   278,    21,    22,     0,     0,     0,     0,
      23,     0,     0,    24,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    25,     0,    26,     0,    27,     0,     0,
       0,    81,     0,     0,    30,    31,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,     0,     0,     0,     0,
      17,    18,    19,    20,     0,     0,    21,    22,     0,     0,
       0,     0,    23,     0,     0,    24,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    25,     0,    26,     0,    27,
     352,     0,     0,    81,     0,     0,    30,    31,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,     0,     0,
       0,     0,    17,    18,    19,    20,     0,     0,    21,    22,
       0,     0,     0,     0,    23,     0,     0,    24,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    25,     0,    26,
       0,    27,   410,     0,     0,    81,     0,     0,    30,    31,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    17,    18,    19,    20,     0,     0,
      21,    22,     0,     0,     0,     0,    23,     0,     0,    24,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    25,
       0,    26,     0,    27,     0,     0,     0,    81,     0,   514,
      30,    31,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    17,    18,    19,    20,
       0,     0,    21,    22,     0,     0,     0,     0,    23,     0,
       0,    24,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    25,     0,    26,     0,    27,     0,     0,     0,    81,
       0,     0,    30,    31,     8,     9,    10,    11,    12,    13,
       0,     0,     0,     0,     0,     0,     0,    17,    18,    19,
       0,     0,     0,    21,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     7,     8,     9,    10,    11,    12,    13,
       0,    14,     0,     0,     0,     0,     0,    17,    18,    19,
     318,     0,     0,    21,   319,     0,     0,     0,     0,    23,
       0,     0,    24,     0,     0,     0,     0,     0,     0,    86,
       0,     0,     0,     0,     0,     0,   179,     0,     0,     0,
     180,     7,     8,     9,    10,    11,    12,    13,     7,    14,
       0,     0,     0,     0,     0,    17,    18,    19,     0,     0,
       0,    21,     0,     7,     0,     0,     0,    23,     0,     0,
      24,     0,     0,     0,    23,     0,     0,   118,    94,    95,
     307,    97,    98,    99,   179,   100,     7,     0,   180,    23,
     308,     0,   118,    94,    95,   307,    97,    98,    99,     0,
     100,     7,     0,     0,     0,   443,     0,     0,     0,     0,
       0,     0,    23,     0,     0,   118,    94,    95,   307,    97,
      98,    99,     0,   100,     0,     0,     0,    23,   520,     0,
     118,    94,    95,    96,    97,    98,    99,     0,   100,   118,
      94,    95,    96,    97,    98,    99,    86,   100,   119,     0,
       0,     0,     0,     0,     0,   176,   118,    94,    95,    96,
      97,    98,    99,    86,   100,   119
};

static const yytype_int16 yycheck[] =
{
       1,   110,   313,   136,    26,   332,    25,   222,    48,   180,
      29,     1,    89,   130,    56,    57,    24,    85,    11,   136,
       3,    28,     3,   237,    48,    49,    27,    28,   105,   179,
      48,    49,    26,    21,     3,   131,   136,    22,    28,    11,
      22,   137,    36,    51,    52,    30,    29,    48,    29,   199,
       3,     4,   213,    46,    14,    24,   217,   218,    48,    28,
      29,    49,    31,   103,    49,    46,    51,    49,    30,    50,
      30,   204,   149,    22,    81,    44,    29,    37,    14,    32,
      81,    50,    83,    84,    46,    93,   203,   204,    48,    90,
      91,    92,   307,    36,   265,    21,   267,    50,    47,    31,
      49,    37,   103,   122,   204,    22,    10,   108,   185,    13,
     129,    37,   152,   103,   112,   155,    24,    24,    50,   269,
     281,   282,   449,    49,   257,   144,   483,   484,   112,   486,
      47,     0,    49,   175,   135,    46,    44,    44,    48,    46,
     257,    23,   219,    50,   145,    29,    30,    56,    57,   506,
       3,   152,   285,   510,   155,   156,    52,   158,    11,   178,
      16,   162,   258,   474,    44,   166,   167,   168,   285,   211,
     171,   386,   343,   344,    30,    31,    29,   196,   186,   340,
     341,   179,   180,   112,    30,   285,   263,    24,   207,    13,
     286,    15,    16,    46,    31,   179,   180,    47,   294,   200,
      49,   199,   473,     3,   365,   366,   283,    44,   206,   210,
      24,    51,    24,    50,   503,   199,   505,    31,    74,    75,
     221,    21,   131,    49,   495,   496,    24,    31,   137,    29,
      44,    31,    44,    31,    51,    51,    50,    37,    50,   237,
       4,     5,     6,    46,   245,   500,    44,   248,   249,    49,
     179,   180,    31,   237,    41,   256,   112,   512,   513,    23,
      48,    49,    44,   119,    21,   284,   175,   265,    21,   267,
     199,   269,    56,    57,   275,    51,   277,   278,    48,    49,
      51,   265,    47,   267,   140,   269,   501,   143,   289,   503,
     367,   505,    47,   370,   295,    49,   205,   339,    46,    33,
      34,    47,   211,   399,    38,    48,    49,   216,   237,    49,
      49,    49,    45,   112,   322,    51,    45,   318,    49,    49,
      30,    22,    30,   179,   180,    47,    30,    51,    49,    47,
     331,   332,    47,   334,    31,   354,   265,   338,   267,     4,
     269,   197,   198,   199,    51,   343,   344,   131,    41,   258,
     206,    39,   394,   137,    44,    44,   357,   379,   380,   343,
     344,   429,   430,   440,    51,    51,   222,   112,   369,    51,
      51,   280,    30,    48,   230,    37,   232,   286,    22,   235,
     179,   180,    31,    47,    47,   294,   405,   406,    31,    31,
     499,   175,    47,   412,    51,    51,   397,    51,    51,    44,
     199,    51,   470,   471,   423,   427,   428,    44,    44,   265,
      33,   267,    45,   269,   343,   344,   112,    45,   527,    49,
      51,   205,    51,    31,    31,    31,    47,   211,    31,    51,
     339,    51,   216,    51,   179,   180,    28,    31,   237,    48,
     459,   460,   461,    31,    31,   446,   447,    45,   449,    45,
     451,   307,    52,     5,   199,   364,    45,   312,   431,    45,
     517,    43,   371,   135,   373,   112,   265,   376,   267,   304,
     269,    28,   235,   492,   258,   331,   332,   480,   383,   216,
     112,   482,   143,   179,   180,   394,   487,   343,   344,    74,
     399,   171,   237,   344,    -1,   514,   280,   498,   476,   500,
      -1,    -1,   286,   199,   413,   503,   415,   505,    -1,    -1,
     294,   512,   513,     2,    -1,     4,    -1,    -1,    -1,   503,
     265,   505,   267,    -1,   269,    -1,    -1,    -1,    -1,    -1,
     386,    -1,   179,   180,   390,    -1,    -1,    -1,   394,    -1,
      -1,   237,    31,   280,   343,   344,   455,   179,   180,    -1,
      27,    28,   199,    -1,    -1,   339,    -1,   466,    -1,   468,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   199,    -1,   265,
      -1,   267,    -1,   269,   503,    -1,   505,    -1,    -1,    -1,
     364,   437,    -1,    -1,    -1,    -1,   442,   371,    -1,   373,
     237,    -1,   376,   449,    -1,    -1,    -1,    -1,   343,   344,
      -1,    -1,    -1,    -1,    81,   237,    -1,    -1,    -1,    -1,
     394,    -1,    -1,    -1,    -1,   399,    -1,    -1,   265,    -1,
     267,    -1,   269,   112,    -1,    -1,    -1,   364,    -1,   413,
      -1,   415,    -1,   265,   371,   267,   373,   269,    -1,   376,
      -1,    -1,    -1,    -1,    -1,   501,    -1,   343,   344,    -1,
     139,     4,     5,     6,     7,     8,     9,    -1,   135,   136,
      -1,    -1,    -1,    -1,    17,    18,    19,    -1,    -1,    -1,
      23,   455,    -1,    -1,    -1,    -1,   413,    -1,   415,    32,
      -1,   158,   466,    -1,   468,    -1,    -1,    -1,    -1,    -1,
     179,   180,    -1,    -1,   171,    -1,   343,   344,    -1,    -1,
      -1,    54,    -1,   180,   503,    -1,   505,    -1,    -1,    -1,
     199,   343,   344,    -1,    -1,    -1,    -1,    -1,   455,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   204,    -1,   466,
      -1,   468,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    13,    -1,    -1,    -1,
      17,    18,    19,    20,    -1,    22,    23,    24,   503,    -1,
     505,    -1,    29,    30,    -1,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    -1,    42,   265,    44,   267,    46,
     269,    -1,    -1,    50,    -1,    -1,    53,    54,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    -1,   503,   285,   505,
      17,    18,    19,    20,    -1,    -1,    23,    24,   295,    -1,
      -1,   298,    29,    -1,    -1,    32,    -1,   304,    35,    -1,
     319,    -1,    39,    -1,    -1,    42,    -1,    44,    -1,    46,
      -1,    -1,    -1,    50,    51,    -1,    53,    54,    -1,    -1,
      -1,    -1,    -1,    -1,   343,   344,   503,    -1,   505,    -1,
       3,     4,     5,     6,     7,     8,     9,   344,    11,    12,
      -1,   503,    -1,   505,    17,    18,    19,    20,    -1,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    -1,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    42,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,
      53,    54,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,
      -1,    32,    -1,    -1,    35,    -1,    -1,    -1,    39,    -1,
      -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,
      51,    -1,    53,    54,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    32,    -1,    -1,    35,    -1,    -1,    -1,
      39,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,
      -1,    50,    51,    -1,    53,    54,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    13,    -1,    -1,    -1,
      17,    18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    32,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,
      -1,    -1,    -1,    50,    -1,    52,    53,    54,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    13,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,    24,
      -1,    -1,    -1,    -1,    29,    -1,    -1,    32,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,
      -1,    46,    -1,    -1,    -1,    50,    -1,    52,    53,    54,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,
      23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    32,
      -1,    -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,
      53,    54,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,
      -1,    32,    -1,    -1,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,
      51,    -1,    53,    54,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    32,    -1,    -1,    35,    -1,    -1,    -1,
      39,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,
      -1,    50,    -1,    -1,    53,    54,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    13,    -1,    -1,    -1,
      17,    18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    -1,    32,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,
      -1,    -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    -1,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,    24,
      -1,    -1,    -1,    -1,    29,    -1,    -1,    32,    -1,    -1,
      -1,    -1,    -1,    -1,    39,    -1,    -1,    42,    -1,    44,
      -1,    46,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    32,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,
      53,    54,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,
      -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,
      -1,    -1,    53,    54,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,
      -1,    50,    -1,    -1,    53,    54,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,
      17,    18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    32,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,
      47,    -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    -1,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,    24,
      -1,    -1,    -1,    -1,    29,    -1,    -1,    32,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,
      -1,    46,    47,    -1,    -1,    50,    -1,    -1,    53,    54,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,
      23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    32,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,    52,
      53,    54,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,
      -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,
      -1,    -1,    53,    54,     4,     5,     6,     7,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,    18,    19,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    -1,    -1,    -1,    -1,    -1,    17,    18,    19,
      50,    -1,    -1,    23,    54,    -1,    -1,    -1,    -1,    29,
      -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    -1,
      50,     3,     4,     5,     6,     7,     8,     9,     3,    11,
      -1,    -1,    -1,    -1,    -1,    17,    18,    19,    -1,    -1,
      -1,    23,    -1,     3,    -1,    -1,    -1,    29,    -1,    -1,
      32,    -1,    -1,    -1,    29,    -1,    -1,    32,    33,    34,
      35,    36,    37,    38,    46,    40,     3,    -1,    50,    29,
      45,    -1,    32,    33,    34,    35,    36,    37,    38,    -1,
      40,     3,    -1,    -1,    -1,    45,    -1,    -1,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    32,    33,    34,    35,    36,
      37,    38,    -1,    40,    -1,    -1,    -1,    29,    45,    -1,
      32,    33,    34,    35,    36,    37,    38,    -1,    40,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41
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
     131,   137,   138,   139,   140,   141,   142,   143,   145,   146,
     147,   148,   149,   150,   151,   155,   143,     0,    59,   143,
      60,   120,   121,   142,    14,    37,   113,     4,     5,     6,
     140,    50,    91,    30,    48,   113,    39,    94,    95,    96,
     153,    22,    30,    32,    33,    34,    35,    36,    37,    38,
      40,    89,    94,   111,   122,   123,   124,   152,   153,   154,
      91,   142,    46,   102,   103,   142,   143,    46,    32,    41,
     153,   154,    48,    67,    75,    70,   112,   140,   140,    31,
      50,    76,    77,    80,   105,    46,    50,    76,    44,    30,
      46,    62,    48,    49,    31,    50,   119,   120,   116,   117,
     118,   142,   110,    94,    94,   110,    30,    47,    49,    68,
      94,    94,    22,    90,    94,    51,    22,    30,    49,    51,
      70,    49,    68,    51,    94,    51,    48,    67,    31,    46,
      50,   108,   127,   132,   133,   134,   137,   141,   142,   143,
     145,   146,   147,   148,   149,   150,    31,    21,    49,    46,
      22,    94,    97,    46,    50,   142,    41,    92,    91,    91,
      35,    51,    97,    98,    77,    95,    51,    97,    98,    99,
     100,   153,    65,   143,    61,   142,    44,   121,    91,    94,
      21,    48,    21,    37,    49,    68,    31,    69,    70,   112,
      81,    82,    70,    94,    94,   153,    94,    51,    22,    49,
      94,    94,    94,    51,    94,   122,   153,    50,    76,    91,
     133,   108,   128,   129,   130,   153,    47,    49,    68,    46,
      91,   142,   142,   133,    94,    22,    47,    49,    22,    97,
      51,    97,    98,    99,    31,    50,    76,    78,    32,    50,
     137,   142,    91,    94,    76,    49,    68,   101,    49,   101,
      47,    78,   101,   101,    49,    68,    94,    35,    45,    66,
     142,   152,    47,    63,    51,   142,   142,   118,    50,    54,
     104,   131,   137,   141,   145,   146,   147,   148,   149,   150,
      45,    49,    49,    45,    30,    94,    94,    94,    22,    51,
      97,    98,    47,    30,    49,    68,    51,   108,   108,   133,
      47,    47,    47,    94,    31,    94,    94,    22,    47,    78,
     101,   101,    68,    91,    51,    97,    98,    99,    94,    35,
      99,    51,    51,    51,    51,   100,    51,   142,   152,    44,
      44,    62,    13,    15,    16,    64,    65,    48,    37,    94,
      13,    30,    84,    94,   142,    84,    94,    22,    94,    76,
     101,   101,   108,   128,    47,    31,    31,    47,    91,    47,
      47,    94,    31,    51,    51,    51,    78,   101,   101,    68,
      51,    94,    68,    31,    78,    78,    78,    44,    44,   113,
     113,    44,    33,    34,    38,   125,   126,   135,   125,    33,
     114,   115,   136,    45,    51,   142,    29,    30,    45,    49,
      83,    28,   142,    45,    94,    51,    51,    91,    91,    31,
      31,    31,    47,    91,    78,    78,    51,    51,    51,    91,
     113,   113,   110,   110,    63,    48,    49,   142,    48,    48,
      68,   142,    31,    94,    94,    84,    94,    28,    78,    91,
      91,    91,    31,    78,    78,   110,   110,    10,    73,   144,
      73,    65,   126,    31,   115,    31,    94,    83,    83,    83,
      94,    91,    73,    73,    52,    72,    74,    94,    67,    72,
      45,   104,   104,    83,    83,    72,    72,    91,    45,    74,
      45,    45,    45,    67
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

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

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

/* Line 1455 of yacc.c  */
#line 40 "lang11d"
    { gRootParseNode = (PyrParseNode*)(yyvsp[(1) - (1)]); gParserResult = 1; ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 42 "lang11d"
    { gRootParseNode = (PyrParseNode*)(yyvsp[(1) - (1)]); gParserResult = 1; ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 44 "lang11d"
    { gRootParseNode = (PyrParseNode*)(yyvsp[(2) - (2)]); gParserResult = 2; ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 47 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 49 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 54 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 58 "lang11d"
    { (yyval) = (intptr_t)newPyrClassNode((PyrSlotNode*)(yyvsp[(1) - (6)]), (PyrSlotNode*)(yyvsp[(2) - (6)]),
					(PyrVarListNode*)(yyvsp[(4) - (6)]), (PyrMethodNode*)(yyvsp[(5) - (6)]), 0);
				;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 62 "lang11d"
    { (yyval) = (intptr_t)newPyrClassNode((PyrSlotNode*)(yyvsp[(1) - (9)]), (PyrSlotNode*)(yyvsp[(5) - (9)]),
					(PyrVarListNode*)(yyvsp[(7) - (9)]), (PyrMethodNode*)(yyvsp[(8) - (9)]),
					(PyrSlotNode*)(yyvsp[(3) - (9)]));
				;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 69 "lang11d"
    {
					(yyval) = (intptr_t)newPyrClassExtNode((PyrSlotNode*)(yyvsp[(2) - (5)]), (PyrMethodNode*)(yyvsp[(4) - (5)]));
				;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 74 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 78 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 80 "lang11d"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 83 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 85 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 89 "lang11d"
    { (yyval) = (intptr_t)newPyrVarListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), varClass); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 91 "lang11d"
    { (yyval) = (intptr_t)newPyrVarListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), varInst); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 93 "lang11d"
    { (yyval) = (intptr_t)newPyrVarListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), varConst); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 96 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 98 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 102 "lang11d"
    { (yyval) = (intptr_t)newPyrMethodNode((PyrSlotNode*)(yyvsp[(1) - (7)]), (PyrSlotNode*)(yyvsp[(5) - (7)]),
					(PyrArgListNode*)(yyvsp[(3) - (7)]), (PyrVarListNode*)(yyvsp[(4) - (7)]), (PyrParseNode*)(yyvsp[(6) - (7)]), 0); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 105 "lang11d"
    { (yyval) = (intptr_t)newPyrMethodNode((PyrSlotNode*)(yyvsp[(2) - (8)]), (PyrSlotNode*)(yyvsp[(6) - (8)]),
					(PyrArgListNode*)(yyvsp[(4) - (8)]), (PyrVarListNode*)(yyvsp[(5) - (8)]), (PyrParseNode*)(yyvsp[(7) - (8)]), 1); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 108 "lang11d"
    { (yyval) = (intptr_t)newPyrMethodNode((PyrSlotNode*)(yyvsp[(1) - (7)]), (PyrSlotNode*)(yyvsp[(5) - (7)]),
					(PyrArgListNode*)(yyvsp[(3) - (7)]), (PyrVarListNode*)(yyvsp[(4) - (7)]), (PyrParseNode*)(yyvsp[(6) - (7)]), 0); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 111 "lang11d"
    { (yyval) = (intptr_t)newPyrMethodNode((PyrSlotNode*)(yyvsp[(2) - (8)]), (PyrSlotNode*)(yyvsp[(6) - (8)]),
					(PyrArgListNode*)(yyvsp[(4) - (8)]), (PyrVarListNode*)(yyvsp[(5) - (8)]), (PyrParseNode*)(yyvsp[(7) - (8)]), 1); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 129 "lang11d"
    { (yyval) = (intptr_t)newPyrDropNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 133 "lang11d"
    { (yyval) = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)(yyvsp[(2) - (4)]), (PyrParseNode*)(yyvsp[(3) - (4)]), false); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 135 "lang11d"
    { (yyval) = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)]), false); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 137 "lang11d"
    { (yyval) = (intptr_t)newPyrBlockNode(NULL, NULL, (PyrParseNode*)(yyvsp[(1) - (1)]), false); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 142 "lang11d"
    { (yyval) = (intptr_t)newPyrDropNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 145 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 147 "lang11d"
    { (yyval) = (yyvsp[(1) - (2)]); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 151 "lang11d"
    { (yyval) = (intptr_t)newPyrReturnNode(NULL); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 153 "lang11d"
    { (yyval) = (intptr_t)newPyrReturnNode((PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 157 "lang11d"
    { (yyval) = (intptr_t)newPyrBlockReturnNode(); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 159 "lang11d"
    { (yyval) = (intptr_t)newPyrReturnNode((PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 164 "lang11d"
    {
					(yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)]));
				;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 173 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 178 "lang11d"
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)]), 0, 0);
			;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 182 "lang11d"
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[(2) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)]), 0, 0);
			;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 186 "lang11d"
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[(1) - (4)]), NULL, NULL, (PyrParseNode*)(yyvsp[(4) - (4)]));
			;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 190 "lang11d"
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[(1) - (6)]), (PyrParseNode*)(yyvsp[(3) - (6)]),
						(PyrParseNode*)(yyvsp[(4) - (6)]), (PyrParseNode*)(yyvsp[(6) - (6)]));
			;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 195 "lang11d"
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[(2) - (6)]), NULL, NULL, (PyrParseNode*)(yyvsp[(6) - (6)]));
			;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 199 "lang11d"
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[(2) - (8)]), (PyrParseNode*)(yyvsp[(5) - (8)]),
						(PyrParseNode*)(yyvsp[(6) - (8)]), (PyrParseNode*)(yyvsp[(8) - (8)]));
			;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 204 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)(yyvsp[(3) - (5)]))) {
					SetRaw(&((PyrPushNameNode*)(yyvsp[(3) - (5)]))->mSlot, s_this);
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				selectornode = newPyrSlotNode(&slot);
				args = linkAfterHead(
					(PyrParseNode*)(yyvsp[(3) - (5)]),
					newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (5)]), NULL));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(4) - (5)]), 0);
			;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 222 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_performList);
				selectornode = newPyrSlotNode(&slot);
				args = linkAfterHead(
					(PyrParseNode*)(yyvsp[(5) - (7)]),
					newPyrPushLitNode((PyrSlotNode*)(yyvsp[(2) - (7)]), NULL));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(6) - (7)]), 0);
			;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 235 "lang11d"
    { (yyval) = (intptr_t)newPyrDynListNode((PyrParseNode*)(yyvsp[(1) - (4)]), (PyrParseNode*)(yyvsp[(3) - (4)])); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 237 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (2)]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, (PyrParseNode*)(yyvsp[(2) - (2)]));
			;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 248 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (4)]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, NULL, (PyrParseNode*)(yyvsp[(4) - (4)]));
			;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 259 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (6)]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(3) - (6)]), (PyrParseNode*)(yyvsp[(6) - (6)]));
			;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 270 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (6)])),
					(PyrParseNode*)(yyvsp[(3) - (6)]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(4) - (6)]), (PyrParseNode*)(yyvsp[(6) - (6)]));
			;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 283 "lang11d"
    {
				PyrSlotNode *selectornode, *selectornode2;
				PyrSlot slot, slot2;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)(yyvsp[(1) - (5)]))) {
					SetRaw(&((PyrPushNameNode*)(yyvsp[(1) - (5)]))->mSlot, s_this);
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(5) - (5)]), 0);
			;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 304 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)(yyvsp[(1) - (5)]), NULL, (PyrParseNode*)(yyvsp[(5) - (5)]));
			;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 313 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)(yyvsp[(1) - (7)]), (PyrParseNode*)(yyvsp[(4) - (7)]), (PyrParseNode*)(yyvsp[(7) - (7)]));
			;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 322 "lang11d"
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[(3) - (8)]), (PyrParseNode*)(yyvsp[(1) - (8)]),
					(PyrParseNode*)(yyvsp[(5) - (8)]), (PyrParseNode*)(yyvsp[(8) - (8)]));
			;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 327 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[(1) - (7)]),
					(PyrParseNode*)(yyvsp[(4) - (7)]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(5) - (7)]), (PyrParseNode*)(yyvsp[(7) - (7)]));
			;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 340 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot, slot2;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)(yyvsp[(1) - (6)]))) {
					SetRaw(&((PyrPushNameNode*)(yyvsp[(1) - (6)]))->mSlot, s_this);
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(5) - (6)]), 0);
			;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 360 "lang11d"
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[(3) - (6)]), (PyrParseNode*)(yyvsp[(1) - (6)]), NULL, (PyrParseNode*)(yyvsp[(6) - (6)]));
			;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 364 "lang11d"
    {
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (8)]), (PyrParseNode*)(yyvsp[(5) - (8)]));
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[(3) - (8)]), args, (PyrParseNode*)(yyvsp[(6) - (8)]), (PyrParseNode*)(yyvsp[(8) - (8)]));
			;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 370 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)(yyvsp[(1) - (7)]))) {
					SetRaw(&((PyrPushNameNode*)(yyvsp[(1) - (7)]))->mSlot, s_this);
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				selectornode = newPyrSlotNode(&slot);

				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (7)]), newPyrPushLitNode((PyrSlotNode*)(yyvsp[(3) - (7)]), NULL));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[(5) - (7)]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(6) - (7)]), 0);
			;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 388 "lang11d"
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[(3) - (4)]), (PyrParseNode*)(yyvsp[(1) - (4)]), 0, (PyrParseNode*)(yyvsp[(4) - (4)]));
			;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 393 "lang11d"
    { pushls(&generatorStack, (yyvsp[(3) - (3)])); pushls(&generatorStack, 1); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 394 "lang11d"
    {
				PyrSlot slot;
				SetSymbol(&slot, getsym("r"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);

				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)(yyvsp[(6) - (7)]), false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)blocklit, 0, 0);
			;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 403 "lang11d"
    { pushls(&generatorStack, (yyvsp[(3) - (3)])); pushls(&generatorStack, 2); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 404 "lang11d"
    {
				(yyval) = (yyvsp[(6) - (7)]);
			;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 410 "lang11d"
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

							(yyval) = (intptr_t)newPyrCallNode(selectornode, expr, 0, 0);
						} break;
						case 2 :
						{
							(yyval) = (intptr_t)expr;
						} break;
					}
				;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 432 "lang11d"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 436 "lang11d"
    {
				// later should check if exprseq is a series and optimize it to for loop
				PyrParseNode *exprseq = (PyrParseNode*)(yyvsp[(3) - (4)]);
				if (exprseq->mClassno == pn_CallNode) {
					PyrCallNode *callnode = (PyrCallNode*)exprseq;
					if (slotRawSymbol(&callnode->mSelector->mSlot) == s_series)
					{
						SetSymbol(&callnode->mSelector->mSlot, getsym("forSeries"));

						PyrVarDefNode* var = newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (4)]), NULL, 0);
						PyrArgListNode* args = newPyrArgListNode(var, NULL);
						PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)(yyvsp[(4) - (4)]), false);
						PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);

						callnode->mArglist = linkNextNode(callnode->mArglist, blocklit);
						(yyval) = (intptr_t)callnode;

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
					(yyval) = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
				}
			;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 470 "lang11d"
    {
				// later should check if exprseq is a series and optimize it to for loop
				PyrParseNode *exprseq = (PyrParseNode*)(yyvsp[(4) - (5)]);
				if (exprseq->mClassno == pn_CallNode) {
					PyrCallNode *callnode = (PyrCallNode*)exprseq;
					if (slotRawSymbol(&callnode->mSelector->mSlot) == s_series)
					{
						SetSymbol(&callnode->mSelector->mSlot, getsym("forSeries"));

						PyrVarDefNode* var1 = newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (5)]), NULL, 0);
						PyrVarDefNode* var2 = newPyrVarDefNode((PyrSlotNode*)(yyvsp[(2) - (5)]), NULL, 0);
						PyrVarDefNode* vars = (PyrVarDefNode*)linkNextNode(var1, var2);
						PyrArgListNode* args = newPyrArgListNode(vars, NULL);
						PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)(yyvsp[(5) - (5)]), false);
						PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);

						callnode->mArglist = linkNextNode(callnode->mArglist, blocklit);
						(yyval) = (intptr_t)callnode;

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
					(yyval) = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
				}
			;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 508 "lang11d"
    {
				PyrSlot slot;
				SetSymbol(&slot, s_value);
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);

				PyrVarDefNode* var = newPyrVarDefNode((PyrSlotNode*)(yyvsp[(2) - (5)]), NULL, 0);
				PyrArgListNode* args = newPyrArgListNode(var, NULL);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)(yyvsp[(5) - (5)]), false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode(blocklit, (PyrParseNode*)(yyvsp[(4) - (5)]));

				(yyval) = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
			;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 522 "lang11d"
    {
				PyrSlot slot;
				SetSymbol(&slot, getsym("if"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)(yyvsp[(2) - (2)]), false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), blocklit);

				(yyval) = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
			;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 533 "lang11d"
    {
				(yyval) = (intptr_t)newPyrDropNode((PyrParseNode*)(yyvsp[(3) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)]));
			;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 537 "lang11d"
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

				(yyval) = (intptr_t)newPyrCallNode(selectornode2, args3, 0, 0);
			;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 568 "lang11d"
    {
				PyrParseNode* node = (PyrParseNode*)(yyvsp[(2) - (3)]);
				node->mParens = 1;
				(yyval) = (yyvsp[(2) - (3)]);
			;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 574 "lang11d"
    {
				PyrParseNode* argnode;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(2) - (2)]), NULL);
				SetSymbol(&slot, s_envirGet);
				selectornode = newPyrSlotNode(&slot);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, argnode, 0, 0);
			;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 584 "lang11d"
    { (yyval) = (intptr_t)newPyrDynListNode(0, (PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 586 "lang11d"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 588 "lang11d"
    { (yyval) = (yyvsp[(3) - (4)]); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 590 "lang11d"
    { (yyval) = (intptr_t)newPyrDynDictNode((PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 592 "lang11d"
    { (yyval) = (intptr_t)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 594 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[(1) - (4)]),
					(PyrParseNode*)(yyvsp[(3) - (4)]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 610 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 637 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 655 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 682 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 710 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 729 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 757 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 784 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 802 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 827 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 855 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 874 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 902 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 921 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 940 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 957 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 971 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 992 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 1011 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 1029 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 1046 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 1062 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 1080 "lang11d"
    { (yyval) = (intptr_t)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 1082 "lang11d"
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[(1) - (5)]),
					(PyrParseNode*)(yyvsp[(4) - (5)]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 1095 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(slotnode, args, 0, 0);
			;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 1109 "lang11d"
    {
				(yyval) = (intptr_t)newPyrBinopCallNode((PyrSlotNode*)(yyvsp[(2) - (4)]),
						(PyrParseNode*)(yyvsp[(1) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)]), (PyrParseNode*)(yyvsp[(3) - (4)]));
			;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 1114 "lang11d"
    {
				(yyval) = (intptr_t)newPyrAssignNode((PyrSlotNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)]), 0);
			;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 1118 "lang11d"
    {
				PyrParseNode *argnode, *args;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(2) - (4)]), NULL);
				args = linkNextNode(argnode, (PyrParseNode*)(yyvsp[(4) - (4)]));
				SetSymbol(&slot, s_envirPut);
				selectornode = newPyrSlotNode(&slot);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 1129 "lang11d"
    {
				(yyval) = (intptr_t)newPyrSetterNode((PyrSlotNode*)(yyvsp[(3) - (5)]),
						(PyrParseNode*)(yyvsp[(1) - (5)]), (PyrParseNode*)(yyvsp[(5) - (5)]));
			;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 1134 "lang11d"
    {
				if ((yyvsp[(4) - (7)]) != 0) {
					error("Setter method called with keyword arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[(4) - (7)]));
					compileErrors++;
				}
				(yyval) = (intptr_t)newPyrSetterNode((PyrSlotNode*)(yyvsp[(1) - (7)]),
						(PyrParseNode*)(yyvsp[(3) - (7)]), (PyrParseNode*)(yyvsp[(7) - (7)]));
			;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 1144 "lang11d"
    {
				(yyval) = (intptr_t)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)(yyvsp[(2) - (4)]),
					(PyrParseNode*)(yyvsp[(4) - (4)]), 0);
			;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 1149 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 1163 "lang11d"
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
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 1178 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 1179 "lang11d"
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(2) - (2)]), NULL); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 1180 "lang11d"
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(2) - (2)]), NULL); ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 1181 "lang11d"
    { (yyval) = (yyvsp[(3) - (4)]); ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 1186 "lang11d"
    {
				(yyval) = (intptr_t)newPyrDropNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)]));
			;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 1194 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 1196 "lang11d"
    { (yyval) = (yyvsp[(1) - (2)]); ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 1201 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 1203 "lang11d"
    {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (2)]), NULL);
					(yyval) = (intptr_t)linkNextNode(key, (PyrParseNode*)(yyvsp[(2) - (2)]));
				;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 1208 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 1210 "lang11d"
    {
					PyrParseNode* elems;
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[(3) - (4)]), NULL);
					elems = (PyrParseNode*)linkNextNode(key, (PyrParseNode*)(yyvsp[(4) - (4)]));
					(yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (4)]), elems);
				;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 1217 "lang11d"
    {
					PyrParseNode* elems;
					elems = (PyrParseNode*)linkNextNode((PyrParseNode*)(yyvsp[(3) - (5)]), (PyrParseNode*)(yyvsp[(5) - (5)]));
					(yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (5)]), elems);
				;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 1226 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 1230 "lang11d"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 1232 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)])); ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 1237 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 1241 "lang11d"
    { (yyval) = (intptr_t)newPyrPushKeyArgNode((PyrSlotNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 1244 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 1245 "lang11d"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 1249 "lang11d"
    { (yyval) = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 1251 "lang11d"
    { (yyval) = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)(yyvsp[(1) - (3)]), (PyrSlotNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 1256 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 1260 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 1261 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 1262 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 1263 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 1264 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 1265 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 1266 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 1267 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 1268 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 1271 "lang11d"
    { (yyval) = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 1274 "lang11d"
    { (yyval) = (intptr_t)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 1277 "lang11d"
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 1278 "lang11d"
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 1279 "lang11d"
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 1280 "lang11d"
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 1281 "lang11d"
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 1282 "lang11d"
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 1283 "lang11d"
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 1284 "lang11d"
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 1285 "lang11d"
    { (yyval) = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 1288 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 1289 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 1290 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 1291 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 1292 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 1293 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 1294 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 1295 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 1296 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 1297 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 1298 "lang11d"
    { (yyval) = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 1302 "lang11d"
    { (yyval) = (intptr_t)newPyrBlockNode((PyrArgListNode*)(yyvsp[(2) - (5)]), (PyrVarListNode*)(yyvsp[(3) - (5)]),
					(PyrParseNode*)(yyvsp[(4) - (5)]), false); ;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 1305 "lang11d"
    { (yyval) = (intptr_t)newPyrBlockNode((PyrArgListNode*)(yyvsp[(2) - (5)]), (PyrVarListNode*)(yyvsp[(3) - (5)]),
					(PyrParseNode*)(yyvsp[(4) - (5)]), true); ;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 1309 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 1311 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 1316 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 1320 "lang11d"
    { (yyval) = (intptr_t)newPyrVarListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), varLocal); ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 1323 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 1325 "lang11d"
    {
					(yyval) = (intptr_t)newPyrArgListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), NULL);
				;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 1329 "lang11d"
    {
					(yyval) = (intptr_t)newPyrArgListNode((PyrVarDefNode*)(yyvsp[(2) - (5)]), (PyrSlotNode*)(yyvsp[(4) - (5)]));
				;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 1333 "lang11d"
    {
					(yyval) = (intptr_t)newPyrArgListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), NULL);
				;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 1337 "lang11d"
    {
					(yyval) = (intptr_t)newPyrArgListNode((PyrVarDefNode*)(yyvsp[(2) - (5)]), (PyrSlotNode*)(yyvsp[(4) - (5)]));
				;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 1344 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 1348 "lang11d"
    { (yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(2) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)]), (yyvsp[(1) - (4)])); ;}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 1351 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 1357 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 1361 "lang11d"
    { (yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL, 0); ;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 1363 "lang11d"
    { (yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)]), 0); ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 1365 "lang11d"
    {
					PyrParseNode* node = (PyrParseNode*)(yyvsp[(4) - (5)]);
					node->mParens = 1;
					(yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (5)]), node, 0);
				;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 1372 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 1378 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 1382 "lang11d"
    { (yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL, 0); ;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 1384 "lang11d"
    { (yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)]), 0); ;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 1386 "lang11d"
    {
									PyrParseNode* node = (PyrParseNode*)(yyvsp[(3) - (4)]);
									node->mParens = 1;
									(yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (4)]), node, 0);
								;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 1394 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 1396 "lang11d"
    {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (2)]), NULL);
					(yyval) = (intptr_t)linkNextNode(key, (PyrParseNode*)(yyvsp[(2) - (2)]));
				;}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 1404 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 1407 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 1413 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 1417 "lang11d"
    { (yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(2) - (2)]), NULL, (yyvsp[(1) - (2)])); ;}
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 1419 "lang11d"
    { (yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(2) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)]), (yyvsp[(1) - (4)])); ;}
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 1423 "lang11d"
    { (yyval) = (intptr_t)newPyrLitDictNode((PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 1427 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 1429 "lang11d"
    {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (2)]), NULL);
					(yyval) = (intptr_t)linkNextNode(key, (PyrParseNode*)(yyvsp[(2) - (2)]));
				;}
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 1437 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 1440 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 1447 "lang11d"
    { (yyval) = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)(yyvsp[(3) - (4)])); ;}
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 1449 "lang11d"
    { (yyval) = (intptr_t)newPyrLitListNode((PyrParseNode*)(yyvsp[(2) - (5)]), (PyrParseNode*)(yyvsp[(4) - (5)])); ;}
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 1453 "lang11d"
    { (yyval) = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 1455 "lang11d"
    { (yyval) = (intptr_t)newPyrLitListNode((PyrParseNode*)(yyvsp[(1) - (4)]), (PyrParseNode*)(yyvsp[(3) - (4)])); ;}
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 1458 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 1464 "lang11d"
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 1467 "lang11d"
    { (yyval) = rwPrivate; ;}
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 1469 "lang11d"
    { (yyval) = rwReadOnly; ;}
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 1471 "lang11d"
    { (yyval) = rwReadWrite; ;}
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 1473 "lang11d"
    { (yyval) = rwWriteOnly; ;}
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 1476 "lang11d"
    { (yyval) = rwPrivate; ;}
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 1478 "lang11d"
    { (yyval) = rwReadOnly; ;}
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 1481 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 1483 "lang11d"
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawInt(&node->mSlot));
				(yyval) = zzval;
			;}
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 1491 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 1493 "lang11d"
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawFloat(&node->mSlot));
				(yyval) = zzval;
			;}
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 1501 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 1503 "lang11d"
    {
					PyrSlotNode *node;
					double intval, fracval;
					node = (PyrSlotNode*)zzval;
					intval = floor(slotRawFloat(&node->mSlot) + 0.5);
					fracval = slotRawFloat(&node->mSlot) - intval;
					SetRaw(&node->mSlot, -intval + fracval);
					(yyval) = zzval;
				;}
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 1513 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 266:

/* Line 1455 of yacc.c  */
#line 1519 "lang11d"
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)(yyvsp[(1) - (2)]);
				SetRaw(&node->mSlot, slotRawFloat(&node->mSlot) * pi);
			;}
    break;

  case 267:

/* Line 1455 of yacc.c  */
#line 1525 "lang11d"
    {
				PyrSlotNode *node;
				double ival;
				node = (PyrSlotNode*)(yyvsp[(1) - (2)]);
				ival = slotRawInt(&node->mSlot);
				SetFloat(&node->mSlot, ival * pi);
			;}
    break;

  case 268:

/* Line 1455 of yacc.c  */
#line 1533 "lang11d"
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, pi);
				(yyval) = zzval;
			;}
    break;

  case 269:

/* Line 1455 of yacc.c  */
#line 1540 "lang11d"
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, -pi);
				(yyval) = zzval;
			;}
    break;

  case 270:

/* Line 1455 of yacc.c  */
#line 1548 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 271:

/* Line 1455 of yacc.c  */
#line 1549 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 272:

/* Line 1455 of yacc.c  */
#line 1552 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 273:

/* Line 1455 of yacc.c  */
#line 1555 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 274:

/* Line 1455 of yacc.c  */
#line 1558 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 275:

/* Line 1455 of yacc.c  */
#line 1561 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 276:

/* Line 1455 of yacc.c  */
#line 1564 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 277:

/* Line 1455 of yacc.c  */
#line 1567 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 278:

/* Line 1455 of yacc.c  */
#line 1570 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 279:

/* Line 1455 of yacc.c  */
#line 1573 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 280:

/* Line 1455 of yacc.c  */
#line 1576 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 281:

/* Line 1455 of yacc.c  */
#line 1579 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 282:

/* Line 1455 of yacc.c  */
#line 1580 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 283:

/* Line 1455 of yacc.c  */
#line 1581 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 284:

/* Line 1455 of yacc.c  */
#line 1582 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 285:

/* Line 1455 of yacc.c  */
#line 1583 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 286:

/* Line 1455 of yacc.c  */
#line 1584 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 287:

/* Line 1455 of yacc.c  */
#line 1585 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 288:

/* Line 1455 of yacc.c  */
#line 1586 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 289:

/* Line 1455 of yacc.c  */
#line 1589 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 292:

/* Line 1455 of yacc.c  */
#line 1596 "lang11d"
    { (yyval) = zzval; ;}
    break;



/* Line 1455 of yacc.c  */
#line 4778 "lang11d_tab.cpp"
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
		      yytoken, &yylval);
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



