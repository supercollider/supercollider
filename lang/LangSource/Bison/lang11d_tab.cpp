
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
# define YYERROR_VERBOSE 0
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
#define YYFINAL  68
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1887

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  102
/* YYNRULES -- Number of rules.  */
#define YYNRULES  295
/* YYNRULES -- Number of states.  */
#define YYNSTATES  539

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
     744,   746,   748,   750,   752,   754,   756,   758,   760,   762,
     768,   774,   775,   778,   780,   783,   787,   788,   792,   798,
     802,   808,   810,   814,   819,   820,   822,   824,   828,   830,
     834,   840,   841,   843,   845,   849,   851,   855,   860,   864,
     867,   869,   873,   874,   877,   879,   883,   886,   891,   896,
     900,   904,   907,   909,   913,   914,   917,   922,   928,   932,
     937,   938,   941,   943,   947,   948,   950,   952,   954,   955,
     957,   959,   962,   964,   967,   969,   972,   974,   976,   978,
     981,   984,   986,   989,   991,   993,   995,   997,   999,  1001,
    1003,  1005,  1007,  1009,  1011,  1013,  1015,  1017,  1019,  1021,
    1023,  1025,  1027,  1029,  1031,  1033
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
      -1,    50,   155,    51,    76,    -1,   143,    50,    51,    76,
      -1,   143,    50,    97,   101,    51,    78,    -1,    50,   155,
      51,    50,    51,    76,    -1,    50,   155,    51,    50,    97,
     101,    51,    78,    -1,   143,    50,    98,   101,    51,    -1,
      50,   155,    51,    50,    98,   101,    51,    -1,   144,    46,
      95,    47,    -1,   144,    76,    -1,   144,    50,    51,    78,
      -1,   144,    50,    99,    68,    51,    78,    -1,   144,    50,
      97,   101,    51,    78,    -1,   144,    50,    98,   101,    51,
      -1,    91,    41,    50,    51,    78,    -1,    91,    41,    50,
      99,    68,    51,    78,    -1,    91,    41,   143,    50,    99,
      68,    51,    78,    -1,    91,    41,    50,    97,   101,    51,
      78,    -1,    91,    41,    50,    98,   101,    51,    -1,    91,
      41,   143,    50,    51,    78,    -1,    91,    41,   143,    50,
      97,   101,    51,    78,    -1,    91,    41,   143,    50,    98,
     101,    51,    -1,    91,    41,   143,    78,    -1,    -1,    44,
      30,    94,    81,    49,    84,    45,    -1,    -1,    44,    48,
      94,    82,    49,    84,    45,    -1,    -1,    49,    84,    -1,
     143,    28,    94,    83,    -1,   143,   143,    28,    94,    83,
      -1,    13,   143,    31,    94,    83,    -1,    94,    83,    -1,
      30,    30,    94,    83,    -1,    30,    29,    94,    83,    -1,
     107,    -1,   105,    -1,    80,    -1,   106,    -1,   156,    -1,
      79,    -1,    50,    94,    51,    -1,    53,   143,    -1,    46,
      95,    47,    -1,    50,    89,    51,    -1,    50,    30,    90,
      51,    -1,    50,   124,    51,    -1,   152,    -1,    85,    46,
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
      -1,    85,    -1,    88,    -1,    87,    -1,   144,    -1,    91,
      41,    46,    97,    47,    -1,    42,    91,    -1,    91,   155,
      92,    91,    -1,   143,    31,    91,    -1,    53,   143,    31,
      91,    -1,    91,    41,   143,    31,    91,    -1,   143,    50,
      97,   101,    51,    31,    91,    -1,    54,   102,    31,    91,
      -1,    85,    46,    97,    47,    31,    91,    -1,    91,    41,
      46,    97,    47,    31,    91,    -1,    -1,    41,   143,    -1,
      41,   138,    -1,    41,    50,    94,    51,    -1,    91,    -1,
      93,    48,    91,    -1,    93,    67,    -1,    -1,    96,    68,
      -1,    94,    -1,    94,    30,    94,    -1,   154,    94,    -1,
      96,    49,    94,    -1,    96,    49,   154,    94,    -1,    96,
      49,    94,    30,    94,    -1,    94,    -1,    97,    49,    94,
      -1,    35,    94,    -1,    97,    49,    35,    94,    -1,   100,
      -1,    99,    49,   100,    -1,   154,    94,    -1,    68,    -1,
      49,    99,    68,    -1,   103,    -1,   103,    21,   143,    -1,
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
      49,   122,    -1,    -1,   123,    68,    -1,   126,    -1,   125,
      49,   126,    -1,   136,   143,    -1,   136,   143,    31,   104,
      -1,    54,    50,   131,    51,    -1,    50,   131,    51,    -1,
     108,    30,   108,    -1,   154,   108,    -1,   129,    -1,   130,
      49,   129,    -1,    -1,   130,    68,    -1,    54,    46,   134,
      47,    -1,    54,   144,    46,   134,    47,    -1,    46,   134,
      47,    -1,   144,    46,   134,    47,    -1,    -1,   135,    68,
      -1,   108,    -1,   135,    49,   108,    -1,    -1,    33,    -1,
      38,    -1,    34,    -1,    -1,    33,    -1,     4,    -1,    32,
       4,    -1,     5,    -1,    32,     5,    -1,     6,    -1,    32,
       6,    -1,    23,    -1,   139,    -1,   140,    -1,   139,   141,
      -1,   138,   141,    -1,   141,    -1,    32,   141,    -1,     3,
      -1,    29,    -1,    11,    -1,    10,    -1,    18,    -1,    19,
      -1,    17,    -1,     9,    -1,     7,    -1,     8,    -1,    20,
      -1,    40,    -1,    38,    -1,    33,    -1,    34,    -1,    32,
      -1,    35,    -1,    36,    -1,    37,    -1,    39,    -1,   153,
      -1,   154,    -1,    12,    -1
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
    1264,  1265,  1266,  1267,  1268,  1269,  1272,  1275,  1278,  1279,
    1280,  1281,  1282,  1283,  1284,  1285,  1286,  1287,  1290,  1291,
    1292,  1293,  1294,  1295,  1296,  1297,  1298,  1299,  1300,  1303,
    1306,  1311,  1312,  1316,  1317,  1321,  1325,  1326,  1330,  1334,
    1338,  1344,  1345,  1349,  1353,  1354,  1357,  1358,  1362,  1364,
    1366,  1374,  1375,  1378,  1379,  1383,  1385,  1387,  1395,  1397,
    1404,  1405,  1409,  1410,  1413,  1414,  1418,  1420,  1424,  1428,
    1432,  1434,  1441,  1442,  1446,  1447,  1452,  1454,  1458,  1460,
    1464,  1465,  1468,  1469,  1473,  1474,  1476,  1478,  1482,  1483,
    1487,  1488,  1497,  1498,  1507,  1508,  1519,  1522,  1523,  1524,
    1530,  1538,  1545,  1554,  1555,  1558,  1561,  1564,  1567,  1570,
    1573,  1576,  1579,  1582,  1585,  1586,  1587,  1588,  1589,  1590,
    1591,  1592,  1595,  1598,  1599,  1602
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
     104,   104,   104,   104,   104,   104,   105,   106,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   109,
     109,   110,   110,   111,   111,   112,   113,   113,   113,   113,
     113,   114,   114,   115,   116,   116,   117,   117,   118,   118,
     118,   119,   119,   120,   120,   121,   121,   121,   122,   122,
     123,   123,   124,   124,   125,   125,   126,   126,   127,   128,
     129,   129,   130,   130,   131,   131,   132,   132,   133,   133,
     134,   134,   135,   135,   136,   136,   136,   136,   137,   137,
     138,   138,   139,   139,   140,   140,   141,   142,   142,   142,
     142,   142,   142,   143,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   153,   153,   153,   153,   153,
     153,   153,   154,   155,   155,   156
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
       1,     1,     1,     1,     1,     1,     1,     1,     1,     5,
       5,     0,     2,     1,     2,     3,     0,     3,     5,     3,
       5,     1,     3,     4,     0,     1,     1,     3,     1,     3,
       5,     0,     1,     1,     3,     1,     3,     4,     3,     2,
       1,     3,     0,     2,     1,     3,     2,     4,     4,     3,
       3,     2,     1,     3,     0,     2,     4,     5,     3,     4,
       0,     2,     1,     3,     0,     1,     1,     1,     0,     1,
       1,     2,     1,     2,     1,     2,     1,     1,     1,     2,
       2,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       5,    44,     0,     0,     2,     3,     7,   273,   260,   262,
     264,   281,   282,   280,   275,   295,     0,   279,   277,   278,
     283,   266,   206,   274,     0,     0,   206,   145,   232,     0,
       0,     0,    37,     4,    33,    92,    89,   124,   101,   126,
     125,   142,    27,    44,    88,    90,    87,   176,    44,   203,
     187,   186,   178,   267,   268,   271,   179,   177,   127,   183,
     184,   185,   180,   182,   181,    99,    91,     0,     1,     6,
      14,     8,     0,   223,   225,   221,   214,   201,   261,   263,
     265,   272,   232,   129,     0,     0,   201,   292,   147,     0,
      29,     0,     0,     0,   288,   286,   287,   289,   290,   291,
     285,   284,     0,     0,    44,   230,    29,     0,   293,   294,
       0,    27,    94,   250,   244,     0,   162,   164,     0,     0,
     288,     0,   294,   138,    28,   144,    34,    36,   204,   270,
     269,     0,     0,    52,    46,    49,    48,   145,     0,    61,
      21,     0,    12,     0,   205,     0,     0,     0,     0,   222,
       0,    29,   216,    31,    44,    75,    77,    44,     0,    95,
      30,   146,   149,   115,     0,     0,     0,    96,   114,     0,
       0,    93,     0,    30,   233,    98,   229,     0,    28,    45,
       0,   250,   244,   252,   198,   197,     0,    29,   188,   189,
     193,     0,   194,   195,   196,   190,   192,   191,     0,   242,
      29,     0,     0,     0,     0,     0,   250,     0,   153,     0,
       0,     0,    50,     0,     0,   143,   131,     0,     0,    29,
      29,    47,     0,    50,    29,    29,    29,   157,     0,     0,
      15,     0,    13,    16,   224,   226,     0,     0,   207,     0,
     209,    30,     0,    32,     0,     0,   202,     0,     0,     0,
     148,   150,     0,   119,    97,   120,     0,   116,   228,     0,
      35,     0,   231,     0,     0,    53,   132,     0,     0,   246,
      30,   251,   250,     0,    30,   245,   238,   241,   135,   163,
     165,     0,     0,     0,   100,     0,     0,     0,    50,    29,
      29,    29,     0,     0,    51,    74,     0,     0,   140,   139,
     130,   155,    54,    30,   160,     0,    30,     0,    60,    62,
       0,     0,    30,     0,   159,   289,    11,    22,     0,     0,
      14,    21,   227,     0,     0,   217,     0,     0,   219,   175,
     174,   166,   167,   171,   172,   173,   168,   170,   169,   200,
       0,     0,   199,     0,   151,   121,     0,   118,     0,    29,
      29,   248,   239,   253,     0,   240,   243,   247,   103,   102,
       0,     0,   154,     0,     0,   128,    66,     0,     0,     0,
     133,    50,    29,    29,    29,     0,     0,    29,    50,    58,
      50,    65,   158,    50,     0,     0,   206,   206,     0,   254,
     254,   258,    17,     0,   208,   210,     0,     0,     0,     0,
      79,   177,     0,   152,   122,   117,    56,     0,     0,   249,
       0,     0,   104,   136,   109,   108,     0,     0,    50,    70,
      50,    71,     0,     0,     0,   141,   156,   161,     0,    55,
      64,    63,   206,   206,   201,   201,    16,   255,   257,   256,
       0,   234,     0,     0,   259,    29,   211,     0,     9,   220,
       0,     0,     0,    76,     0,    84,     0,     0,    78,   123,
      50,    59,   106,   105,     0,     0,     0,   110,   137,    69,
      67,    50,    73,    50,   134,   201,   201,    40,    40,    21,
      19,   254,   236,    18,    20,   258,     0,     0,    79,    79,
      80,    79,     0,    57,   107,   112,   111,     0,    72,    68,
      40,    40,   276,    42,    27,    42,     0,   235,     0,   212,
       0,    79,    86,    85,    81,    79,   113,    42,    42,     0,
       0,    38,    42,    41,     0,    10,   237,   213,    83,    82,
       0,     0,    27,    23,    39,    25,    24,    26,    43
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     3,     4,     5,    69,     6,   231,   143,   321,   392,
     229,   317,   125,   304,   244,    32,    33,   520,   503,   521,
      34,   294,   134,   295,    35,    36,   247,   248,   455,   399,
      37,    38,    39,    40,   102,   165,    41,   214,    42,   208,
      89,    90,   209,   220,   377,   227,   305,   115,   116,   328,
      44,    45,    46,   183,    47,   154,    48,   246,    77,   445,
     446,   150,   151,   152,   148,    72,    73,   105,   106,   107,
     440,   441,    50,   184,   199,   200,   201,    51,   185,   186,
     187,   442,   447,    52,    53,    54,    55,    56,    57,    58,
     504,    59,    60,    61,    62,    63,    64,    65,   108,   122,
     123,    66
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -357
static const yytype_int16 yypact[] =
{
      43,   701,    31,    37,    31,    10,  -357,  -357,  -357,  -357,
    -357,  -357,  -357,  -357,  -357,  -357,    41,  -357,  -357,  -357,
    -357,  -357,    11,  -357,   339,  1720,    86,  1356,   888,  1720,
      41,    25,  -357,  -357,  -357,  -357,  -357,    40,  -357,  -357,
    -357,   810,    48,     6,  -357,  -357,  -357,  -357,  1096,  -357,
    -357,  -357,    89,    89,  -357,  -357,  -357,    97,    98,  -357,
    -357,  -357,  -357,  -357,  -357,  -357,  -357,    83,  -357,  -357,
      26,  -357,   198,  -357,   -16,    41,    41,  -357,  -357,  -357,
    -357,  -357,   940,  -357,  1720,  1720,  -357,  -357,   108,    84,
     133,  1720,  1720,  1408,   339,  -357,  -357,  -357,  -357,  -357,
    -357,  -357,   137,   265,  1096,  -357,   147,   152,  -357,  1720,
     154,   601,   183,  1820,  1772,   199,    12,  -357,   197,  1460,
    -357,    16,  -357,   192,  1720,  -357,  -357,  -357,  -357,  -357,
    -357,  1720,  1148,   101,  -357,  -357,  -357,  1356,   756,   101,
    -357,    31,    41,   205,  -357,    41,  1720,  1720,   220,   211,
     243,    18,  -357,   349,  1096,  -357,  -357,  1096,  1720,  -357,
    1356,  -357,  -357,  -357,  1720,   222,     2,  -357,  1720,  1720,
    1720,  -357,   224,  1356,  -357,  -357,  -357,   273,  -357,  -357,
    1720,  1820,  1772,  -357,  -357,  -357,   236,   241,    89,  -357,
    -357,   248,  -357,  -357,  -357,  -357,  -357,  -357,   268,  -357,
     251,   254,  1820,  1720,    41,    41,  1820,  1720,  -357,    38,
    1512,   992,   226,   249,  1720,   810,   810,  1720,   101,   258,
     264,  -357,   271,   101,   258,   264,   272,  -357,  1720,   838,
    -357,   280,  -357,  -357,  -357,   810,   279,    41,  -357,    41,
    -357,  -357,    41,  -357,   808,   311,  -357,   301,   312,   324,
    -357,   351,  1720,  -357,  -357,  1720,  1720,  -357,  -357,   335,
    -357,   352,  -357,  1720,  1200,   101,   810,   332,   333,  -357,
    1820,  -357,  1820,  1820,  1772,  -357,  -357,  -357,   810,  -357,
    -357,   336,   338,  1564,   356,  1720,  1720,    88,   101,   258,
     264,   272,  1720,  1044,   101,  -357,   384,  1720,  -357,  -357,
     348,  -357,   101,  1252,  -357,   342,   362,   353,  -357,  -357,
     367,   368,   362,   370,  -357,   175,  -357,  -357,   365,   381,
     372,   320,  -357,   376,   389,  -357,  1720,     7,  -357,  -357,
    -357,    89,  -357,  -357,  -357,  -357,  -357,  -357,  -357,  -357,
    1304,  1304,  -357,  1720,  -357,  -357,   406,  1720,   101,   258,
     264,  -357,  -357,  -357,   383,  -357,  -357,  -357,   402,   404,
     391,  1720,  -357,   392,  1616,   405,  -357,   396,   397,   398,
     810,   101,   258,   264,   272,   403,  1720,   272,   195,  -357,
     101,  -357,  -357,   101,   399,   413,    11,    11,   421,   202,
     202,   409,  -357,  1827,  -357,  -357,   416,    41,   308,   423,
     420,    49,   425,  -357,  1720,  -357,   101,   427,   428,  -357,
    1720,  1720,   443,   810,   445,   449,   435,  1720,   101,  -357,
     101,  -357,   433,   434,   436,  -357,  -357,  -357,  1720,  -357,
    -357,  -357,    11,    11,  -357,  -357,  -357,  -357,  -357,  -357,
     300,  -357,    41,   325,  -357,   328,  -357,    41,  -357,  -357,
     457,  1720,  1720,  -357,  1304,  -357,  1720,   462,  -357,  -357,
     101,  -357,   810,   810,  1720,  1720,  1720,   464,   810,  -357,
    -357,   101,  -357,   101,   810,  -357,  -357,   160,   160,   320,
    -357,   202,   467,  -357,  -357,   409,   469,  1720,   420,   420,
    -357,   420,  1720,  -357,   810,   810,   810,  1720,  -357,  -357,
     160,   160,  -357,  1668,   459,  1668,  1842,  -357,   669,  -357,
     669,   420,  -357,  -357,  -357,   420,   810,  1668,  1668,  1720,
     465,  -357,   468,  -357,   474,  -357,  -357,  -357,  -357,  -357,
     476,   477,   601,  -357,  -357,  -357,  -357,  -357,  -357
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -357,  -357,  -357,  -357,  -357,   509,  -357,   203,    92,  -357,
    -317,  -357,  -110,   305,  -357,   -40,  -357,  -320,  -335,     8,
     481,   -47,   -74,  -149,  -357,   135,  -357,  -357,  -160,  -339,
    -357,  -357,  -357,  -357,  -357,  -357,   -20,  -357,  -357,    -1,
     395,  -357,  -109,  -135,  -106,   225,   207,  -357,  -357,  -356,
     187,  -357,  -357,   -84,  -357,   -69,   506,    15,   -19,  -357,
      53,  -357,  -357,   303,  -357,   473,   414,   369,  -357,  -357,
     153,    68,  -231,  -357,   276,  -357,   386,  -222,  -357,  -175,
    -357,  -357,  -357,   -93,  -357,  -357,   -12,    -7,    19,   550,
    -357,    36,   190,   327,   345,   358,   380,  -357,  -191,   484,
     -14,  -357
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -219
static const yytype_int16 yytable[] =
{
      43,   179,   402,   225,   393,    83,   267,    86,   127,   111,
     133,   139,    81,   329,   110,   146,    49,   157,    14,     7,
     188,   188,   330,   219,   255,    75,    88,   103,     7,   224,
     198,   281,   226,   204,   147,    74,    14,    68,   319,  -215,
     129,   130,    14,    49,     7,    23,     2,    43,    76,   112,
     117,   256,     7,   113,    23,   240,   141,   114,    29,   221,
     283,   205,   210,   128,   172,   221,   211,   241,   110,     1,
      23,   113,   142,    22,   309,   114,   290,   456,    23,     2,
     131,   103,    81,   155,   156,   284,   119,   285,   188,   188,
     162,   163,   166,    26,    74,   153,   124,   354,   198,   132,
      75,   287,   289,    43,   215,   291,   189,   189,   176,   188,
     364,   216,    21,   188,   245,   490,    84,   249,   277,   128,
     298,    22,    22,    76,   385,    22,   235,   140,   131,   350,
     265,   159,   190,   190,    85,   365,    88,   285,   158,   366,
     212,    26,    26,   505,   137,    26,   236,   132,   138,   192,
     192,   331,   526,    43,   527,   349,    43,   250,   373,   251,
     266,   232,   506,   253,    74,   517,   518,   257,   258,   259,
     502,   302,   261,    16,   189,   189,   129,   188,     7,   188,
     188,   188,   160,   278,   372,   524,   353,   374,   167,   355,
     198,   221,   135,   135,   300,   189,   173,   530,   531,   189,
     190,   190,   319,   175,    23,   177,   282,   120,    95,    96,
      97,    98,    99,   100,   180,   101,   301,   192,   192,    22,
     221,   190,   421,   279,   280,   190,   428,   314,   221,   429,
     203,   430,   299,   213,   431,   437,   438,   332,   192,    26,
     439,   237,   192,   206,   136,   136,   144,   145,   318,   233,
      22,   344,     7,     8,   345,   346,   323,   292,   324,   238,
     145,   153,   176,   189,   239,   189,   189,   189,   135,   469,
      26,   470,   370,   254,   135,   260,   293,   329,    23,   329,
     333,   296,   360,   269,   362,   363,   330,   168,   330,   190,
     270,   190,   190,   190,   272,   169,   375,    22,   273,   297,
     274,   406,   362,   193,   193,   276,   192,   303,   192,   192,
     192,   493,   135,   306,   170,   319,   171,    26,   308,   129,
     136,   312,   498,   264,   499,   396,   136,   320,   512,   513,
     322,   514,   221,   389,   384,   390,   391,   451,   452,   400,
     400,   413,   403,    78,    79,    80,   405,   135,   480,   481,
     340,   528,  -218,   135,   133,   529,   339,   347,   135,   401,
     401,   341,    21,   416,   136,   477,   478,   434,   435,   342,
    -218,   193,   193,   483,   481,   426,   484,   241,  -218,   351,
     243,   343,   169,   357,   352,   358,  -218,   361,    78,   121,
     462,   463,   193,   378,   523,   161,   193,   468,  -218,   136,
     135,    87,   141,   459,   379,   136,   500,   501,   474,   386,
     136,   174,   318,   475,   476,   331,   450,   331,   380,   381,
     457,   383,   538,   135,   394,   387,   395,   307,   404,   135,
     409,   310,   311,   410,   334,   411,   417,   135,   412,   414,
     194,   194,   444,   432,   494,   495,   496,   418,   419,   420,
     488,   489,   136,   400,   425,   491,   242,   433,   195,   195,
     193,   482,   193,   193,   193,   436,   486,   449,   453,   454,
     458,   196,   196,   401,   464,   136,   465,   516,   460,   461,
     466,   136,   467,   135,   471,   472,   511,   473,   487,   136,
     492,   515,   271,   197,   197,   497,   367,   368,   508,   532,
     510,   332,   522,   332,   522,   275,   135,   178,   194,   194,
     533,    91,   109,   135,    71,   135,   522,   522,   135,   535,
     519,   536,   537,   388,   126,   318,   195,   195,   479,   194,
     534,   313,   222,   194,   104,   136,   135,   382,   509,   196,
     196,   135,   262,   443,   333,   325,   333,   195,   149,   507,
     356,   195,    67,   135,    70,   135,   407,   408,   136,   234,
     196,   197,   197,     0,   196,   136,   109,   136,   268,     0,
     136,   335,     0,     0,     0,     0,     0,     0,     0,   422,
     423,   118,   197,     0,     0,     0,   197,     0,   136,   336,
       0,     0,     0,   136,     0,   135,   369,   194,   202,   194,
     194,   194,   337,     0,     0,   136,   135,   136,   135,     0,
       0,     0,     0,     0,     0,   195,     0,   195,   195,   195,
       0,    91,   228,     0,   338,     0,     0,     0,   196,     0,
     196,   196,   196,   120,    95,    96,    97,    98,    99,   100,
      87,   101,   121,     0,   252,     0,     0,   136,     0,   178,
     197,     0,   197,   197,   197,     0,     0,   263,   136,     0,
     136,     0,     0,   191,   191,     0,   202,     0,     0,     0,
       0,     0,     0,     8,     9,    10,    11,    12,    13,   424,
       0,     0,   427,     0,     0,     0,    17,    18,    19,     0,
       0,   230,    21,     0,     0,   228,     0,     0,   334,     0,
     334,    24,     0,     0,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,    16,     0,     0,     0,    17,    18,
      19,    20,     0,   327,    21,    22,     0,     0,     0,     0,
      23,   191,   191,    24,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    25,     0,    26,     0,    27,     0,     0,
     485,    28,   191,    29,    30,    31,   191,     0,   202,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,     0,
       0,     0,     0,    17,    18,    19,    20,   228,     0,    21,
      22,     0,     0,     0,     0,    23,     0,   228,    24,     0,
     228,   217,     0,     0,     0,    87,   228,     0,    25,     0,
      26,     0,    27,     0,     0,     0,    82,   223,     0,    30,
      31,     0,     8,     9,    10,    11,    12,    13,     0,     0,
     191,     0,   191,   191,   191,    17,    18,    19,     0,     0,
       0,    21,     0,     0,     0,   335,     0,   335,     0,     0,
      24,     7,   120,    95,    96,    97,    98,    99,   100,    87,
     101,   121,     0,   336,     0,   336,     0,     0,   326,     0,
       0,     0,   327,     0,     0,     0,   337,    23,   337,     0,
     120,    95,    96,   315,    98,    99,   100,   118,   101,     0,
       0,     0,     0,   316,     0,     0,     0,     0,   338,     0,
     338,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,    16,     0,     0,     0,    17,    18,    19,    20,     0,
      92,    21,    22,     0,     0,     0,     0,    23,    93,     0,
      94,    95,    96,    97,    98,    99,   100,    87,   101,     0,
      25,     0,    26,     0,    27,     0,     0,     0,    82,     0,
       0,    30,    31,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,     0,     0,     0,     0,    17,    18,    19,
      20,     0,    92,    21,    22,     0,     0,     0,     0,    23,
      93,     0,    94,    95,    96,    97,    98,    99,   100,    87,
     101,     0,    25,     0,    26,     0,    27,     0,     0,     0,
      82,     0,     0,    30,    31,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,     0,     0,     0,     0,    17,
      18,    19,    20,     0,     0,    21,    22,     0,     0,     0,
       0,    23,     0,     0,    24,     0,     0,   217,     0,     0,
       0,    87,     0,     0,    25,     0,    26,     0,    27,     0,
       0,     0,    82,   288,     0,    30,    31,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,     0,     0,     0,
       0,    17,    18,    19,    20,     0,     0,    21,    22,     0,
       0,     0,     0,    23,     0,     0,    24,     0,     0,   217,
       0,     0,     0,    87,     0,     0,    25,     0,    26,     0,
      27,     0,     0,     0,    82,   371,     0,    30,    31,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,    16,
       0,     0,     0,    17,    18,    19,    20,     0,     0,    21,
      22,     0,     0,     0,     0,    23,     0,     0,    24,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    25,     0,
      26,     0,    27,     0,     0,     0,    82,     0,    29,    30,
      31,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,     0,     0,     0,     0,    17,    18,    19,    20,     0,
       0,    21,    22,     0,     0,     0,     0,    23,     0,     0,
      24,     0,     0,   217,     0,     0,     0,     0,     0,     0,
      25,     0,    26,     0,    27,     0,     0,     0,    82,   218,
       0,    30,    31,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,     0,     0,     0,     0,    17,    18,    19,
      20,     0,     0,    21,    22,     0,     0,     0,     0,    23,
       0,     0,    24,     0,     0,   217,     0,     0,     0,     0,
       0,     0,    25,     0,    26,     0,    27,     0,     0,     0,
      82,   348,     0,    30,    31,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,     0,     0,     0,     0,    17,
      18,    19,    20,     0,     0,    21,    22,     0,     0,     0,
       0,    23,     0,     0,    24,     0,     0,   376,     0,     0,
       0,    87,     0,     0,    25,     0,    26,     0,    27,     0,
       0,     0,    82,     0,     0,    30,    31,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,   397,     0,     0,
       0,    17,    18,    19,    20,     0,     0,    21,    22,     0,
       0,     0,     0,    23,   398,     0,    24,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    25,     0,    26,     0,
      27,     0,     0,     0,    82,     0,     0,    30,    31,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,     0,
       0,     0,     0,    17,    18,    19,    20,     0,     0,    21,
      22,     0,     0,     0,     0,    23,     0,     0,    24,     0,
       0,     0,     0,     0,     0,    87,     0,     0,    25,     0,
      26,     0,    27,     0,     0,     0,    82,     0,     0,    30,
      31,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,     0,     0,     0,     0,    17,    18,    19,    20,     0,
     164,    21,    22,     0,     0,     0,     0,    23,     0,     0,
      24,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      25,     0,    26,     0,    27,     0,     0,     0,    82,     0,
       0,    30,    31,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,     0,     0,     0,     0,    17,    18,    19,
      20,     0,   207,    21,    22,     0,     0,     0,     0,    23,
       0,     0,    24,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    25,     0,    26,     0,    27,     0,     0,     0,
      82,     0,     0,    30,    31,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,     0,     0,     0,     0,    17,
      18,    19,    20,     0,   286,    21,    22,     0,     0,     0,
       0,    23,     0,     0,    24,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    25,     0,    26,     0,    27,     0,
       0,     0,    82,     0,     0,    30,    31,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,     0,     0,     0,
       0,    17,    18,    19,    20,     0,     0,    21,    22,     0,
       0,     0,     0,    23,     0,     0,    24,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    25,     0,    26,     0,
      27,   359,     0,     0,    82,     0,     0,    30,    31,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,     0,
       0,     0,     0,    17,    18,    19,    20,     0,     0,    21,
      22,     0,     0,     0,     0,    23,     0,     0,    24,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    25,     0,
      26,     0,    27,   415,     0,     0,    82,     0,     0,    30,
      31,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,     0,     0,     0,     0,    17,    18,    19,    20,     0,
       0,    21,    22,     0,     0,     0,     0,    23,     0,     0,
      24,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      25,     0,    26,     0,    27,     0,     0,     0,    82,     0,
     519,    30,    31,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,     0,     0,     0,     0,    17,    18,    19,
      20,     0,     0,    21,    22,     0,     0,     0,     0,    23,
       0,     0,    24,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    25,     0,    26,     0,    27,     0,     0,     0,
      82,     0,     0,    30,    31,     7,     8,     9,    10,    11,
      12,    13,     0,    14,     0,     0,     0,     0,     0,    17,
      18,    19,     0,     0,     0,    21,     0,     0,     0,     0,
       0,    23,     0,     0,    24,     0,     0,     0,     0,     0,
       0,    87,     0,     0,     0,     0,     0,     0,   181,     0,
       0,     0,   182,     7,     8,     9,    10,    11,    12,    13,
       7,    14,     0,     0,     0,     0,     0,    17,    18,    19,
       0,     0,     0,    21,     0,     7,     0,     0,     0,    23,
       0,     0,    24,     0,     0,     0,    23,     0,     0,   120,
      95,    96,   315,    98,    99,   100,   181,   101,     0,     0,
     182,    23,   448,     0,   120,    95,    96,   315,    98,    99,
     100,     0,   101,     0,     0,     0,     0,   525
};

static const yytype_int16 yycheck[] =
{
       1,   111,   341,   138,   321,    25,   181,    26,    48,    29,
      57,    58,    24,   244,    28,    31,     1,    86,    11,     3,
     113,   114,   244,   132,    22,    14,    27,    28,     3,   138,
     114,   206,   138,    21,    50,    16,    11,     0,   229,    21,
      52,    53,    11,    28,     3,    29,    36,    48,    37,    30,
      31,    49,     3,    46,    29,    37,    30,    50,    52,   133,
      22,    49,    46,    48,   104,   139,    50,    49,    82,    26,
      29,    46,    46,    24,   223,    50,   211,    28,    29,    36,
      31,    82,    94,    84,    85,    47,    46,    49,   181,   182,
      91,    92,    93,    44,    75,    76,    48,   272,   182,    50,
      14,   210,   211,   104,   124,   211,   113,   114,   109,   202,
      22,   131,    23,   206,   154,   454,    30,   157,   202,   104,
     213,    24,    24,    37,   315,    24,   146,    44,    31,   264,
     177,    47,   113,   114,    48,    47,   137,    49,    30,   288,
     121,    44,    44,   478,    46,    44,   147,    50,    50,   113,
     114,   244,   508,   154,   510,   264,   157,   158,   293,   160,
     180,   142,   479,   164,   145,   500,   501,   168,   169,   170,
      10,   218,   173,    13,   181,   182,   188,   270,     3,   272,
     273,   274,    49,   203,   293,   505,   270,   293,    51,   273,
     274,   265,    57,    58,   214,   202,    49,   517,   518,   206,
     181,   182,   393,    51,    29,    51,   207,    32,    33,    34,
      35,    36,    37,    38,    31,    40,   217,   181,   182,    24,
     294,   202,   371,   204,   205,   206,    31,   228,   302,   378,
      31,   380,   213,    41,   383,    33,    34,   244,   202,    44,
      38,    21,   206,    46,    57,    58,    48,    49,   229,    44,
      24,   252,     3,     4,   255,   256,   237,    31,   239,    48,
      49,   242,   263,   270,    21,   272,   273,   274,   133,   418,
      44,   420,   292,    51,   139,    51,    50,   508,    29,   510,
     244,    32,   283,    47,   285,   286,   508,    22,   510,   270,
      49,   272,   273,   274,    46,    30,   297,    24,    30,    50,
      49,   348,   303,   113,   114,    51,   270,    49,   272,   273,
     274,   460,   177,    49,    49,   506,    51,    44,    47,   331,
     133,    49,   471,    50,   473,   326,   139,    47,   488,   489,
      51,   491,   406,    13,   315,    15,    16,    29,    30,   340,
     341,   361,   343,     4,     5,     6,   347,   212,    48,    49,
      49,   511,     3,   218,   401,   515,    45,    22,   223,   340,
     341,    49,    23,   364,   177,   434,   435,   386,   387,    45,
      21,   181,   182,    48,    49,   376,    48,    49,    29,    47,
      31,    30,    30,    47,    51,    47,    37,    31,     4,    41,
     410,   411,   202,    51,   504,    90,   206,   417,    49,   212,
     265,    39,    30,   404,    51,   218,   475,   476,   428,    44,
     223,   106,   393,   432,   433,   508,   397,   510,    51,    51,
     401,    51,   532,   288,    48,    44,    37,   220,    22,   294,
      47,   224,   225,    31,   244,    31,    31,   302,    47,    47,
     113,   114,    33,    44,   464,   465,   466,    51,    51,    51,
     451,   452,   265,   454,    51,   456,   151,    44,   113,   114,
     270,   442,   272,   273,   274,    44,   447,    51,    45,    49,
      45,   113,   114,   454,    31,   288,    31,   497,    51,    51,
      31,   294,    47,   348,    51,    51,   487,    51,    31,   302,
      28,   492,   187,   113,   114,    31,   289,   290,    31,   519,
      31,   508,   503,   510,   505,   200,   371,    48,   181,   182,
      45,    27,    28,   378,     5,   380,   517,   518,   383,    45,
      52,    45,    45,   320,    43,   506,   181,   182,   436,   202,
     522,   226,   137,   206,    28,   348,   401,   312,   485,   181,
     182,   406,   173,   390,   508,   242,   510,   202,    75,   481,
     274,   206,     2,   418,     4,   420,   349,   350,   371,   145,
     202,   181,   182,    -1,   206,   378,    82,   380,   182,    -1,
     383,   244,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   372,
     373,    31,   202,    -1,    -1,    -1,   206,    -1,   401,   244,
      -1,    -1,    -1,   406,    -1,   460,   291,   270,   114,   272,
     273,   274,   244,    -1,    -1,   418,   471,   420,   473,    -1,
      -1,    -1,    -1,    -1,    -1,   270,    -1,   272,   273,   274,
      -1,   137,   138,    -1,   244,    -1,    -1,    -1,   270,    -1,
     272,   273,   274,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    -1,   160,    -1,    -1,   460,    -1,    48,
     270,    -1,   272,   273,   274,    -1,    -1,   173,   471,    -1,
     473,    -1,    -1,   113,   114,    -1,   182,    -1,    -1,    -1,
      -1,    -1,    -1,     4,     5,     6,     7,     8,     9,   374,
      -1,    -1,   377,    -1,    -1,    -1,    17,    18,    19,    -1,
      -1,   141,    23,    -1,    -1,   211,    -1,    -1,   508,    -1,
     510,    32,    -1,    -1,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    13,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    54,    23,    24,    -1,    -1,    -1,    -1,
      29,   181,   182,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,
     445,    50,   202,    52,    53,    54,   206,    -1,   274,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    -1,
      -1,    -1,    -1,    17,    18,    19,    20,   293,    -1,    23,
      24,    -1,    -1,    -1,    -1,    29,    -1,   303,    32,    -1,
     306,    35,    -1,    -1,    -1,    39,   312,    -1,    42,    -1,
      44,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,    53,
      54,    -1,     4,     5,     6,     7,     8,     9,    -1,    -1,
     270,    -1,   272,   273,   274,    17,    18,    19,    -1,    -1,
      -1,    23,    -1,    -1,    -1,   508,    -1,   510,    -1,    -1,
      32,     3,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    -1,   508,    -1,   510,    -1,    -1,    50,    -1,
      -1,    -1,    54,    -1,    -1,    -1,   508,    29,   510,    -1,
      32,    33,    34,    35,    36,    37,    38,   327,    40,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    -1,    -1,   508,    -1,
     510,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      12,    13,    -1,    -1,    -1,    17,    18,    19,    20,    -1,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    -1,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    -1,
      42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,
      -1,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    -1,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,
      50,    -1,    -1,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,
      18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,
      -1,    29,    -1,    -1,    32,    -1,    -1,    35,    -1,    -1,
      -1,    39,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    51,    -1,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    -1,    -1,    35,
      -1,    -1,    -1,    39,    -1,    -1,    42,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    13,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,
      24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    32,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,
      44,    -1,    46,    -1,    -1,    -1,    50,    -1,    52,    53,
      54,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,
      -1,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,
      32,    -1,    -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    51,
      -1,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,
      -1,    -1,    32,    -1,    -1,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,
      50,    51,    -1,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,
      18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,
      -1,    29,    -1,    -1,    32,    -1,    -1,    35,    -1,    -1,
      -1,    39,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    13,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    -1,    32,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    -1,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,
      24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    32,    -1,
      -1,    -1,    -1,    -1,    -1,    39,    -1,    -1,    42,    -1,
      44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,    53,
      54,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,
      -1,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,
      50,    -1,    -1,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,
      18,    19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,
      46,    47,    -1,    -1,    50,    -1,    -1,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    -1,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,
      24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    32,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,
      44,    -1,    46,    47,    -1,    -1,    50,    -1,    -1,    53,
      54,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,
      -1,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,
      52,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,
      -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,
      50,    -1,    -1,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    -1,    -1,    -1,    -1,    -1,    17,
      18,    19,    -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,
      -1,    29,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,
      -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,
      -1,    -1,    50,     3,     4,     5,     6,     7,     8,     9,
       3,    11,    -1,    -1,    -1,    -1,    -1,    17,    18,    19,
      -1,    -1,    -1,    23,    -1,     3,    -1,    -1,    -1,    29,
      -1,    -1,    32,    -1,    -1,    -1,    29,    -1,    -1,    32,
      33,    34,    35,    36,    37,    38,    46,    40,    -1,    -1,
      50,    29,    45,    -1,    32,    33,    34,    35,    36,    37,
      38,    -1,    40,    -1,    -1,    -1,    -1,    45
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
      49,    51,    70,    49,    68,    51,    94,    51,    48,    67,
      31,    46,    50,   108,   128,   133,   134,   135,   138,   142,
     143,   144,   146,   147,   148,   149,   150,   151,   108,   129,
     130,   131,   154,    31,    21,    49,    46,    22,    94,    97,
      46,    50,   143,    41,    92,    91,    91,    35,    51,    97,
      98,    77,    95,    51,    97,    98,    99,   100,   154,    65,
     144,    61,   143,    44,   121,    91,    94,    21,    48,    21,
      37,    49,    68,    31,    69,    70,   112,    81,    82,    70,
      94,    94,   154,    94,    51,    22,    49,    94,    94,    94,
      51,    94,   122,   154,    50,    76,    91,   134,   131,    47,
      49,    68,    46,    30,    49,    68,    51,   108,    91,   143,
     143,   134,    94,    22,    47,    49,    22,    97,    51,    97,
      98,    99,    31,    50,    76,    78,    32,    50,   138,   143,
      91,    94,    76,    49,    68,   101,    49,   101,    47,    78,
     101,   101,    49,    68,    94,    35,    45,    66,   143,   153,
      47,    63,    51,   143,   143,   118,    50,    54,   104,   127,
     132,   138,   142,   146,   147,   148,   149,   150,   151,    45,
      49,    49,    45,    30,    94,    94,    94,    22,    51,    97,
      98,    47,    51,   108,   134,   108,   129,    47,    47,    47,
      94,    31,    94,    94,    22,    47,    78,   101,   101,    68,
      91,    51,    97,    98,    99,    94,    35,    99,    51,    51,
      51,    51,   100,    51,   143,   153,    44,    44,    62,    13,
      15,    16,    64,    65,    48,    37,    94,    13,    30,    84,
      94,   143,    84,    94,    22,    94,    76,   101,   101,    47,
      31,    31,    47,    91,    47,    47,    94,    31,    51,    51,
      51,    78,   101,   101,    68,    51,    94,    68,    31,    78,
      78,    78,    44,    44,   113,   113,    44,    33,    34,    38,
     125,   126,   136,   125,    33,   114,   115,   137,    45,    51,
     143,    29,    30,    45,    49,    83,    28,   143,    45,    94,
      51,    51,    91,    91,    31,    31,    31,    47,    91,    78,
      78,    51,    51,    51,    91,   113,   113,   110,   110,    63,
      48,    49,   143,    48,    48,    68,   143,    31,    94,    94,
      84,    94,    28,    78,    91,    91,    91,    31,    78,    78,
     110,   110,    10,    73,   145,    73,    65,   126,    31,   115,
      31,    94,    83,    83,    83,    94,    91,    73,    73,    52,
      72,    74,    94,    67,    72,    45,   104,   104,    83,    83,
      72,    72,    91,    45,    74,    45,    45,    45,    67
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
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 54 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 58 "lang11d"
    { (yyval) = (long)newPyrClassNode((PyrSlotNode*)(yyvsp[(1) - (6)]), (PyrSlotNode*)(yyvsp[(2) - (6)]),
					(PyrVarListNode*)(yyvsp[(4) - (6)]), (PyrMethodNode*)(yyvsp[(5) - (6)]), 0);
				;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 62 "lang11d"
    { (yyval) = (long)newPyrClassNode((PyrSlotNode*)(yyvsp[(1) - (9)]), (PyrSlotNode*)(yyvsp[(5) - (9)]),
					(PyrVarListNode*)(yyvsp[(7) - (9)]), (PyrMethodNode*)(yyvsp[(8) - (9)]),
					(PyrSlotNode*)(yyvsp[(3) - (9)]));
				;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 69 "lang11d"
    {
					(yyval) = (long)newPyrClassExtNode((PyrSlotNode*)(yyvsp[(2) - (5)]), (PyrMethodNode*)(yyvsp[(4) - (5)]));
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
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 89 "lang11d"
    { (yyval) = (long)newPyrVarListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), varClass); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 91 "lang11d"
    { (yyval) = (long)newPyrVarListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), varInst); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 93 "lang11d"
    { (yyval) = (long)newPyrVarListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), varConst); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 96 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 98 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 102 "lang11d"
    { (yyval) = (long)newPyrMethodNode((PyrSlotNode*)(yyvsp[(1) - (7)]), (PyrSlotNode*)(yyvsp[(5) - (7)]),
					(PyrArgListNode*)(yyvsp[(3) - (7)]), (PyrVarListNode*)(yyvsp[(4) - (7)]), (PyrParseNode*)(yyvsp[(6) - (7)]), 0); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 105 "lang11d"
    { (yyval) = (long)newPyrMethodNode((PyrSlotNode*)(yyvsp[(2) - (8)]), (PyrSlotNode*)(yyvsp[(6) - (8)]),
					(PyrArgListNode*)(yyvsp[(4) - (8)]), (PyrVarListNode*)(yyvsp[(5) - (8)]), (PyrParseNode*)(yyvsp[(7) - (8)]), 1); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 108 "lang11d"
    { (yyval) = (long)newPyrMethodNode((PyrSlotNode*)(yyvsp[(1) - (7)]), (PyrSlotNode*)(yyvsp[(5) - (7)]),
					(PyrArgListNode*)(yyvsp[(3) - (7)]), (PyrVarListNode*)(yyvsp[(4) - (7)]), (PyrParseNode*)(yyvsp[(6) - (7)]), 0); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 111 "lang11d"
    { (yyval) = (long)newPyrMethodNode((PyrSlotNode*)(yyvsp[(2) - (8)]), (PyrSlotNode*)(yyvsp[(6) - (8)]),
					(PyrArgListNode*)(yyvsp[(4) - (8)]), (PyrVarListNode*)(yyvsp[(5) - (8)]), (PyrParseNode*)(yyvsp[(7) - (8)]), 1); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 129 "lang11d"
    { (yyval) = (long)newPyrDropNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 133 "lang11d"
    { (yyval) = (long)newPyrBlockNode(NULL, (PyrVarListNode*)(yyvsp[(2) - (4)]), (PyrParseNode*)(yyvsp[(3) - (4)]), false); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 135 "lang11d"
    { (yyval) = (long)newPyrBlockNode(NULL, (PyrVarListNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)]), false); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 137 "lang11d"
    { (yyval) = (long)newPyrBlockNode(NULL, NULL, (PyrParseNode*)(yyvsp[(1) - (1)]), false); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 142 "lang11d"
    { (yyval) = (long)newPyrDropNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
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
    { (yyval) = (long)newPyrReturnNode(NULL); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 153 "lang11d"
    { (yyval) = (long)newPyrReturnNode((PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 157 "lang11d"
    { (yyval) = (long)newPyrBlockReturnNode(); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 159 "lang11d"
    { (yyval) = (long)newPyrReturnNode((PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 164 "lang11d"
    {
					(yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)]));
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
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)]), 0, 0);
			;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 182 "lang11d"
    {
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(2) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)]), 0, 0);
			;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 186 "lang11d"
    {
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(1) - (4)]), NULL, NULL, (PyrParseNode*)(yyvsp[(4) - (4)]));
			;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 190 "lang11d"
    {
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(1) - (6)]), (PyrParseNode*)(yyvsp[(3) - (6)]),
						(PyrParseNode*)(yyvsp[(4) - (6)]), (PyrParseNode*)(yyvsp[(6) - (6)]));
			;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 195 "lang11d"
    {
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(2) - (6)]), NULL, NULL, (PyrParseNode*)(yyvsp[(6) - (6)]));
			;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 199 "lang11d"
    {
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(2) - (8)]), (PyrParseNode*)(yyvsp[(5) - (8)]),
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
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(4) - (5)]), 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(6) - (7)]), 0);
			;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 235 "lang11d"
    { (yyval) = (long)newPyrDynListNode((PyrParseNode*)(yyvsp[(1) - (4)]), (PyrParseNode*)(yyvsp[(3) - (4)])); ;}
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, (PyrParseNode*)(yyvsp[(2) - (2)]));
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
				(yyval) = (long)newPyrCallNode(selectornode, args, NULL, (PyrParseNode*)(yyvsp[(4) - (4)]));
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
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(3) - (6)]), (PyrParseNode*)(yyvsp[(6) - (6)]));
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
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(4) - (6)]), (PyrParseNode*)(yyvsp[(6) - (6)]));
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
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(5) - (5)]), 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, (PyrParseNode*)(yyvsp[(1) - (5)]), NULL, (PyrParseNode*)(yyvsp[(5) - (5)]));
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
				(yyval) = (long)newPyrCallNode(selectornode, (PyrParseNode*)(yyvsp[(1) - (7)]), (PyrParseNode*)(yyvsp[(5) - (7)]), (PyrParseNode*)(yyvsp[(6) - (7)]));
			;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 322 "lang11d"
    {
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(3) - (8)]), (PyrParseNode*)(yyvsp[(1) - (8)]),
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
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(5) - (7)]), (PyrParseNode*)(yyvsp[(7) - (7)]));
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
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(5) - (6)]), 0);
			;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 360 "lang11d"
    {
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(3) - (6)]), (PyrParseNode*)(yyvsp[(1) - (6)]), NULL, (PyrParseNode*)(yyvsp[(6) - (6)]));
			;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 364 "lang11d"
    {
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)(yyvsp[(1) - (8)]), (PyrParseNode*)(yyvsp[(5) - (8)]));
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(3) - (8)]), args, (PyrParseNode*)(yyvsp[(6) - (8)]), (PyrParseNode*)(yyvsp[(8) - (8)]));
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
				(yyval) = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[(6) - (7)]), 0);
			;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 388 "lang11d"
    {
				(yyval) = (long)newPyrCallNode((PyrSlotNode*)(yyvsp[(3) - (4)]), (PyrParseNode*)(yyvsp[(1) - (4)]), 0, (PyrParseNode*)(yyvsp[(4) - (4)]));
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
				(yyval) = (long)newPyrCallNode(selectornode, (PyrParseNode*)blocklit, 0, 0);
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

							(yyval) = (long)newPyrCallNode(selectornode, expr, 0, 0);
						} break;
						case 2 :
						{
							(yyval) = (long)expr;
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

				(yyval) = (long)newPyrCallNode(selectornode, args2, 0, 0);
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

				(yyval) = (long)newPyrCallNode(selectornode, args2, 0, 0);
			;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 533 "lang11d"
    {
				(yyval) = (long)newPyrDropNode((PyrParseNode*)(yyvsp[(3) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)]));
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

				(yyval) = (long)newPyrCallNode(selectornode2, args3, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, argnode, 0, 0);
			;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 584 "lang11d"
    { (yyval) = (long)newPyrDynListNode(0, (PyrParseNode*)(yyvsp[(2) - (3)])); ;}
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
    { (yyval) = (long)newPyrDynDictNode((PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 592 "lang11d"
    { (yyval) = (long)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (1)])); ;}
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 1080 "lang11d"
    { (yyval) = (long)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (1)])); ;}
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(slotnode, args, 0, 0);
			;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 1109 "lang11d"
    {
				(yyval) = (long)newPyrBinopCallNode((PyrSlotNode*)(yyvsp[(2) - (4)]),
						(PyrParseNode*)(yyvsp[(1) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)]), (PyrParseNode*)(yyvsp[(3) - (4)]));
			;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 1114 "lang11d"
    {
				(yyval) = (long)newPyrAssignNode((PyrSlotNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)]), 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
			;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 1129 "lang11d"
    {
				(yyval) = (long)newPyrSetterNode((PyrSlotNode*)(yyvsp[(3) - (5)]),
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
				(yyval) = (long)newPyrSetterNode((PyrSlotNode*)(yyvsp[(1) - (7)]),
						(PyrParseNode*)(yyvsp[(3) - (7)]), (PyrParseNode*)(yyvsp[(7) - (7)]));
			;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 1144 "lang11d"
    {
				(yyval) = (long)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)(yyvsp[(2) - (4)]),
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
				(yyval) = (long)newPyrCallNode(selectornode, args, 0, 0);
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
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(2) - (2)]), NULL); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 1180 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(2) - (2)]), NULL); ;}
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
				(yyval) = (long)newPyrDropNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)]));
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
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 1203 "lang11d"
    {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (2)]), NULL);
					(yyval) = (long)linkNextNode(key, (PyrParseNode*)(yyvsp[(2) - (2)]));
				;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 1208 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 1210 "lang11d"
    {
					PyrParseNode* elems;
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[(3) - (4)]), NULL);
					elems = (PyrParseNode*)linkNextNode(key, (PyrParseNode*)(yyvsp[(4) - (4)]));
					(yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (4)]), elems);
				;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 1217 "lang11d"
    {
					PyrParseNode* elems;
					elems = (PyrParseNode*)linkNextNode((PyrParseNode*)(yyvsp[(3) - (5)]), (PyrParseNode*)(yyvsp[(5) - (5)]));
					(yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (5)]), elems);
				;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 1226 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 1230 "lang11d"
    { (yyval) = (yyvsp[(2) - (2)]); ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 1232 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)])); ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 1237 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 1241 "lang11d"
    { (yyval) = (long)newPyrPushKeyArgNode((PyrSlotNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
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
    { (yyval) = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 1251 "lang11d"
    { (yyval) = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)(yyvsp[(1) - (3)]), (PyrSlotNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 1256 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 1260 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 1261 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 1262 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 1263 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 1264 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 1265 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 1266 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 1267 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 1268 "lang11d"
    { (yyval) = (long)newPyrLiteralNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 1269 "lang11d"
    { (yyval) = (long)newPyrLiteralNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 1272 "lang11d"
    { (yyval) = (long)newPyrPushLitNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 1275 "lang11d"
    { (yyval) = (long)newPyrPushNameNode((PyrSlotNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 1278 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 1279 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 1280 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 1281 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 1282 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 1283 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 1284 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 1285 "lang11d"
    { (yyval) = (long)newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 1286 "lang11d"
    { (yyval) = (long)newPyrPushLitNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 1287 "lang11d"
    { (yyval) = (long)newPyrPushLitNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 1290 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 1291 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 1292 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 1293 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 1294 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 1295 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 1296 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 1297 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 1298 "lang11d"
    { (yyval) = (long)newPyrLiteralNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL); ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 1299 "lang11d"
    { (yyval) = (long)newPyrLiteralNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 1300 "lang11d"
    { (yyval) = (long)newPyrLiteralNode(NULL, (PyrParseNode*)(yyvsp[(1) - (1)])); ;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 1304 "lang11d"
    { (yyval) = (long)newPyrBlockNode((PyrArgListNode*)(yyvsp[(2) - (5)]), (PyrVarListNode*)(yyvsp[(3) - (5)]),
					(PyrParseNode*)(yyvsp[(4) - (5)]), false); ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 1307 "lang11d"
    { (yyval) = (long)newPyrBlockNode((PyrArgListNode*)(yyvsp[(2) - (5)]), (PyrVarListNode*)(yyvsp[(3) - (5)]),
					(PyrParseNode*)(yyvsp[(4) - (5)]), true); ;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 1311 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 1313 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 1318 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (2)]), (PyrParseNode*)(yyvsp[(2) - (2)])); ;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 1322 "lang11d"
    { (yyval) = (long)newPyrVarListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), varLocal); ;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 1325 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 1327 "lang11d"
    {
					(yyval) = (long)newPyrArgListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), NULL);
				;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 1331 "lang11d"
    {
					(yyval) = (long)newPyrArgListNode((PyrVarDefNode*)(yyvsp[(2) - (5)]), (PyrSlotNode*)(yyvsp[(4) - (5)]));
				;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 1335 "lang11d"
    {
					(yyval) = (long)newPyrArgListNode((PyrVarDefNode*)(yyvsp[(2) - (3)]), NULL);
				;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 1339 "lang11d"
    {
					(yyval) = (long)newPyrArgListNode((PyrVarDefNode*)(yyvsp[(2) - (5)]), (PyrSlotNode*)(yyvsp[(4) - (5)]));
				;}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 1346 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 1350 "lang11d"
    { (yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(2) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)]), (yyvsp[(1) - (4)])); ;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 1353 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 1359 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 1363 "lang11d"
    { (yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL, 0); ;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 1365 "lang11d"
    { (yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)]), 0); ;}
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 1367 "lang11d"
    {
					PyrParseNode* node = (PyrParseNode*)(yyvsp[(4) - (5)]);
					node->mParens = 1;
					(yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (5)]), node, 0);
				;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 1374 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 1380 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 1384 "lang11d"
    { (yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (1)]), NULL, 0); ;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 1386 "lang11d"
    { (yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)]), 0); ;}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 1388 "lang11d"
    {
									PyrParseNode* node = (PyrParseNode*)(yyvsp[(3) - (4)]);
									node->mParens = 1;
									(yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(1) - (4)]), node, 0);
								;}
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 1396 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 1398 "lang11d"
    {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (2)]), NULL);
					(yyval) = (long)linkNextNode(key, (PyrParseNode*)(yyvsp[(2) - (2)]));
				;}
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 1406 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 1409 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 1415 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 1419 "lang11d"
    { (yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(2) - (2)]), NULL, (yyvsp[(1) - (2)])); ;}
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 1421 "lang11d"
    { (yyval) = (long)newPyrVarDefNode((PyrSlotNode*)(yyvsp[(2) - (4)]), (PyrParseNode*)(yyvsp[(4) - (4)]), (yyvsp[(1) - (4)])); ;}
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 1425 "lang11d"
    { (yyval) = (long)newPyrLitDictNode((PyrParseNode*)(yyvsp[(3) - (4)])); ;}
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 1429 "lang11d"
    { (yyval) = (long)newPyrLitDictNode((PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 1433 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 1435 "lang11d"
    {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[(1) - (2)]), NULL);
					(yyval) = (long)linkNextNode(key, (PyrParseNode*)(yyvsp[(2) - (2)]));
				;}
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 1443 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 1446 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 1453 "lang11d"
    { (yyval) = (long)newPyrLitListNode(0, (PyrParseNode*)(yyvsp[(3) - (4)])); ;}
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 1455 "lang11d"
    { (yyval) = (long)newPyrLitListNode((PyrParseNode*)(yyvsp[(2) - (5)]), (PyrParseNode*)(yyvsp[(4) - (5)])); ;}
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 1459 "lang11d"
    { (yyval) = (long)newPyrLitListNode(0, (PyrParseNode*)(yyvsp[(2) - (3)])); ;}
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 1461 "lang11d"
    { (yyval) = (long)newPyrLitListNode((PyrParseNode*)(yyvsp[(1) - (4)]), (PyrParseNode*)(yyvsp[(3) - (4)])); ;}
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 1464 "lang11d"
    { (yyval) = 0; ;}
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 1470 "lang11d"
    { (yyval) = (long)linkNextNode((PyrParseNode*)(yyvsp[(1) - (3)]), (PyrParseNode*)(yyvsp[(3) - (3)])); ;}
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 1473 "lang11d"
    { (yyval) = rwPrivate; ;}
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 1475 "lang11d"
    { (yyval) = rwReadOnly; ;}
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 1477 "lang11d"
    { (yyval) = rwReadWrite; ;}
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 1479 "lang11d"
    { (yyval) = rwWriteOnly; ;}
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 1482 "lang11d"
    { (yyval) = rwPrivate; ;}
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 1484 "lang11d"
    { (yyval) = rwReadOnly; ;}
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 1487 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 1489 "lang11d"
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawInt(&node->mSlot));
				(yyval) = zzval;
			;}
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 1497 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 1499 "lang11d"
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawFloat(&node->mSlot));
				(yyval) = zzval;
			;}
    break;

  case 264:

/* Line 1455 of yacc.c  */
#line 1507 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 265:

/* Line 1455 of yacc.c  */
#line 1509 "lang11d"
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

  case 266:

/* Line 1455 of yacc.c  */
#line 1519 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 269:

/* Line 1455 of yacc.c  */
#line 1525 "lang11d"
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)(yyvsp[(1) - (2)]);
				SetRaw(&node->mSlot, slotRawFloat(&node->mSlot) * pi);
			;}
    break;

  case 270:

/* Line 1455 of yacc.c  */
#line 1531 "lang11d"
    {
				PyrSlotNode *node;
				double ival;
				node = (PyrSlotNode*)(yyvsp[(1) - (2)]);
				ival = slotRawInt(&node->mSlot);
				SetFloat(&node->mSlot, ival * pi);
			;}
    break;

  case 271:

/* Line 1455 of yacc.c  */
#line 1539 "lang11d"
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, pi);
				(yyval) = zzval;
			;}
    break;

  case 272:

/* Line 1455 of yacc.c  */
#line 1546 "lang11d"
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, -pi);
				(yyval) = zzval;
			;}
    break;

  case 273:

/* Line 1455 of yacc.c  */
#line 1554 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 274:

/* Line 1455 of yacc.c  */
#line 1555 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 275:

/* Line 1455 of yacc.c  */
#line 1558 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 276:

/* Line 1455 of yacc.c  */
#line 1561 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 277:

/* Line 1455 of yacc.c  */
#line 1564 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 278:

/* Line 1455 of yacc.c  */
#line 1567 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 279:

/* Line 1455 of yacc.c  */
#line 1570 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 280:

/* Line 1455 of yacc.c  */
#line 1573 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 281:

/* Line 1455 of yacc.c  */
#line 1576 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 282:

/* Line 1455 of yacc.c  */
#line 1579 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 283:

/* Line 1455 of yacc.c  */
#line 1582 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 284:

/* Line 1455 of yacc.c  */
#line 1585 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 285:

/* Line 1455 of yacc.c  */
#line 1586 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 286:

/* Line 1455 of yacc.c  */
#line 1587 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 287:

/* Line 1455 of yacc.c  */
#line 1588 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 288:

/* Line 1455 of yacc.c  */
#line 1589 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 289:

/* Line 1455 of yacc.c  */
#line 1590 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 290:

/* Line 1455 of yacc.c  */
#line 1591 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 291:

/* Line 1455 of yacc.c  */
#line 1592 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 292:

/* Line 1455 of yacc.c  */
#line 1595 "lang11d"
    { (yyval) = zzval; ;}
    break;

  case 295:

/* Line 1455 of yacc.c  */
#line 1602 "lang11d"
    { (yyval) = zzval; ;}
    break;



/* Line 1455 of yacc.c  */
#line 4788 "lang11d_tab.cpp"
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



