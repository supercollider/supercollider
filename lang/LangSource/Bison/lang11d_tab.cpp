/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 16 "lang11d" /* yacc.c:339  */


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



#line 87 "lang11d_tab.cpp" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
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
   by #include "lang11d_tab.hpp".  */
#ifndef YY_YY_LANG11D_TAB_HPP_INCLUDED
# define YY_YY_LANG11D_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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
    BINOP = 285,
    KEYBINOP = 286,
    READWRITEVAR = 287,
    UMINUS = 288
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_LANG11D_TAB_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 172 "lang11d_tab.cpp" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
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
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

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
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  63
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1911

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  100
/* YYNRULES -- Number of rules.  */
#define YYNRULES  290
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  532

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   288

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    54,     2,     2,     2,     2,
      50,    51,    37,    38,    49,    34,    41,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    30,    48,
      35,    31,    36,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    46,     2,    47,    52,     2,    42,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    44,    39,    45,    53,     2,     2,     2,
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
      25,    26,    27,    28,    29,    32,    33,    40,    43
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    39,    39,    41,    45,    46,    48,    52,    56,    63,
      69,    70,    73,    74,    78,    79,    83,    85,    87,    91,
      92,    96,    99,   102,   105,   110,   111,   114,   115,   118,
     119,   122,   123,   127,   129,   131,   135,   136,   140,   141,
     146,   147,   152,   153,   157,   158,   164,   165,   168,   169,
     172,   176,   180,   184,   189,   193,   198,   216,   229,   231,
     242,   253,   264,   277,   298,   307,   316,   321,   334,   354,
     358,   364,   382,   388,   388,   398,   398,   405,   426,   430,
     464,   502,   516,   527,   531,   556,   557,   558,   559,   560,
     561,   562,   568,   578,   580,   582,   584,   586,   588,   601,
     604,   631,   649,   676,   704,   723,   751,   778,   796,   821,
     849,   868,   896,   915,   934,   951,   965,   986,  1005,  1023,
    1040,  1056,  1072,  1073,  1074,  1075,  1076,  1089,  1103,  1108,
    1112,  1123,  1128,  1138,  1143,  1157,  1173,  1174,  1175,  1176,
    1179,  1180,  1186,  1189,  1190,  1194,  1195,  1197,  1202,  1204,
    1211,  1219,  1220,  1224,  1226,  1230,  1231,  1235,  1239,  1240,
    1243,  1245,  1249,  1250,  1255,  1256,  1257,  1258,  1259,  1260,
    1261,  1262,  1263,  1266,  1269,  1272,  1273,  1274,  1275,  1276,
    1277,  1278,  1279,  1280,  1283,  1284,  1285,  1286,  1287,  1288,
    1289,  1290,  1291,  1292,  1293,  1296,  1299,  1304,  1305,  1309,
    1310,  1314,  1318,  1319,  1323,  1327,  1331,  1337,  1338,  1342,
    1346,  1347,  1350,  1351,  1355,  1357,  1359,  1367,  1368,  1371,
    1372,  1376,  1378,  1380,  1388,  1390,  1397,  1398,  1402,  1403,
    1406,  1407,  1411,  1413,  1417,  1421,  1423,  1430,  1431,  1435,
    1436,  1441,  1443,  1447,  1449,  1453,  1454,  1457,  1458,  1462,
    1463,  1465,  1467,  1471,  1472,  1476,  1477,  1486,  1487,  1496,
    1497,  1508,  1511,  1512,  1513,  1519,  1527,  1534,  1543,  1544,
    1547,  1550,  1553,  1556,  1559,  1562,  1565,  1568,  1571,  1574,
    1575,  1576,  1577,  1578,  1579,  1580,  1581,  1584,  1587,  1588,
    1591
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NAME", "INTEGER", "SC_FLOAT",
  "ACCIDENTAL", "SYMBOL", "STRING", "ASCII", "PRIMITIVENAME", "CLASSNAME",
  "CURRYARG", "VAR", "ARG", "CLASSVAR", "SC_CONST", "NILOBJ", "TRUEOBJ",
  "FALSEOBJ", "PSEUDOVAR", "ELLIPSIS", "DOTDOT", "PIE", "BEGINCLOSEDFUNC",
  "BADTOKEN", "INTERPRET", "BEGINGENERATOR", "LEFTARROW", "WHILE", "':'",
  "'='", "BINOP", "KEYBINOP", "'-'", "'<'", "'>'", "'*'", "'+'", "'|'",
  "READWRITEVAR", "'.'", "'`'", "UMINUS", "'{'", "'}'", "'['", "']'",
  "';'", "','", "'('", "')'", "'^'", "'~'", "'#'", "$accept", "root",
  "classes", "classdef", "classextension", "optname", "superclass",
  "classvardecls", "classvardecl", "methods", "methoddef", "optsemi",
  "optcomma", "optequal", "funcbody", "cmdlinecode", "methbody",
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
  "pseudovar", "binop", "keybinop", "binop2", "curryarg", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
      58,    61,   285,   286,    45,    60,    62,    42,    43,   124,
     287,    46,    96,   288,   123,   125,    91,    93,    59,    44,
      40,    41,    94,   126,    35
};
# endif

#define YYPACT_NINF -343

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-343)))

#define YYTABLE_NINF -215

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      -9,  1075,    25,     9,  -343,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,    27,  -343,  -343,  -343,  -343,  -343,    51,
    -343,   231,  1751,    36,  1387,   752,  1751,    27,    23,  -343,
    -343,  -343,  -343,  -343,    11,  -343,  -343,  -343,  1870,   -15,
      24,  -343,  -343,  -343,  -343,  1127,  -343,  -343,    50,    50,
    -343,  -343,  -343,   171,   209,  -343,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,   105,  -343,  -343,    91,    14,  -343,
      63,    27,    27,  -343,  -343,  -343,  -343,  -343,   867,  -343,
    1751,  1751,  -343,  -343,    55,    83,    95,  1751,  1751,  1439,
    -343,   231,  -343,  -343,  -343,  -343,  -343,  -343,    99,    84,
    1127,  -343,   108,   120,  -343,  1751,   122,  1841,   157,  1837,
     167,    19,  -343,   154,  1491,  -343,     8,  -343,   166,  1751,
    -343,  -343,  -343,  -343,  -343,  -343,  1751,  1179,   103,  -343,
    -343,  -343,  1387,   919,   103,   165,   105,    27,   172,  -343,
      27,  1751,  1751,   199,   164,   207,     0,  -343,    43,  1127,
    -343,  -343,  1127,  1751,  -343,  1387,  -343,  -343,  -343,  1751,
     181,    21,  -343,  1751,  1751,  1751,  -343,   183,  1387,  -343,
    -343,  -343,    73,  -343,  -343,  1751,  1837,  1803,  -343,  -343,
    -343,   211,   194,    50,  -343,  -343,   200,  -343,  -343,  -343,
    -343,  -343,  -343,  1751,    27,    27,  1837,  1751,  -343,    61,
    1543,   971,   216,   291,  1751,  1870,  1870,  1751,   103,   229,
     233,  -343,   237,   103,   229,   233,   244,  -343,  1751,  -343,
    -343,   239,  -343,  -343,  -343,  1870,   258,    27,  -343,    27,
    -343,  -343,    27,  -343,   804,   235,  -343,   262,   264,   272,
    -343,   297,  1751,  -343,  -343,  1751,  1751,  -343,  -343,   307,
    -343,   301,  -343,  1751,  1231,   103,  1870,   285,   313,  -343,
     288,   294,  1837,  -343,  1837,  -343,  1837,  1870,  -343,  -343,
     300,   309,  1595,   319,  1751,  1751,    89,   103,   229,   233,
     244,  1751,  1023,   103,  -343,   353,  1751,  -343,  -343,   317,
    -343,   103,  1283,  -343,   308,   327,   314,  -343,  -343,   315,
     316,   327,   318,  -343,   625,   338,   303,  -343,   323,   334,
    -343,  1751,     7,  -343,  -343,    50,  -343,  -343,  -343,  -343,
    -343,  -343,  -343,  -343,  1335,  1335,  -343,  1751,  -343,  -343,
     352,  1751,   103,   229,   233,  -343,  1837,  1803,  -343,  -343,
    -343,  -343,   329,  -343,   347,   348,   335,  1751,  -343,   336,
    1647,   354,  -343,   333,   341,   342,  1870,   103,   229,   233,
     244,   344,  1751,   244,    74,  -343,   103,  -343,  -343,   103,
     640,  -343,  -343,   345,   356,   357,   125,   125,   361,  -343,
     796,  -343,  -343,   346,    27,   195,   362,   363,   198,   366,
    -343,  1751,  -343,   103,   359,   367,  -343,  -343,  -343,  1751,
    1751,   389,  1870,   391,   393,   381,  1751,   103,  -343,   103,
    -343,   378,   379,   382,  -343,  -343,  -343,  1751,  -343,  -343,
    -343,   394,   397,    51,    51,  -343,  -343,  -343,  -343,   202,
    -343,    27,   252,  -343,   273,  -343,    27,  -343,  -343,   406,
    1751,  1751,  -343,  1335,  -343,  1751,   415,  -343,  -343,   103,
    -343,  1870,  1870,  1751,  1751,  1751,   414,  1870,  -343,  -343,
     103,  -343,   103,  1870,    51,    51,  -343,  -343,   303,  -343,
     125,   417,  -343,  -343,   361,   418,  1751,   363,   363,  -343,
     363,  1751,  -343,  1870,  1870,  1870,  1751,  -343,  -343,  -343,
    -343,   133,   133,  1856,  -343,   714,  -343,   714,   363,  -343,
    -343,  -343,   363,  1870,   133,   133,  -343,  1699,   402,  1699,
    -343,  -343,  -343,  -343,  -343,  1699,  1699,  1751,   408,  -343,
     404,  -343,   409,   412,   416,  1841,  -343,  -343,  -343,  -343,
    -343,  -343
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       4,    42,     0,     2,   268,   255,   257,   259,   276,   277,
     275,   270,   290,     0,   274,   272,   273,   278,   261,   202,
     269,     0,     0,   202,   143,   228,     0,     0,     0,    35,
       3,    31,    90,    87,   122,    99,   124,   123,   140,    25,
      42,    86,    88,    85,   173,    42,   199,   183,   175,   262,
     263,   266,   176,   174,   125,   180,   181,   182,   177,   179,
     178,    97,    89,     1,     0,     6,     5,    12,     0,   219,
     221,   217,   210,   197,   256,   258,   260,   267,   228,   127,
       0,     0,   197,   287,   145,     0,    27,     0,     0,     0,
     279,   283,   281,   282,   284,   285,   286,   280,     0,     0,
      42,   226,    27,     0,   288,   289,     0,    25,    92,   245,
       0,   160,   162,     0,     0,   283,     0,   289,   136,    26,
     142,    32,    34,   200,   265,   264,     0,     0,    50,    44,
      47,    46,   143,     0,    59,     0,     0,    10,     0,   201,
       0,     0,     0,     0,   218,     0,    27,   212,    29,    42,
      73,    75,    42,     0,    93,    28,   144,   147,   113,     0,
       0,     0,    94,   112,     0,     0,    91,     0,    28,   229,
      96,   225,     0,    26,    43,     0,   245,   239,   247,   194,
     193,     0,    27,   184,   185,   189,     0,   190,   191,   192,
     186,   188,   187,     0,     0,     0,   245,     0,   151,     0,
       0,     0,    48,     0,     0,   141,   129,     0,     0,    27,
      27,    45,     0,    48,    27,    27,    27,   155,     0,    19,
      13,     0,    11,    14,   220,   222,     0,     0,   203,     0,
     205,    28,     0,    30,     0,     0,   198,     0,     0,     0,
     146,   148,     0,   117,    95,   118,     0,   114,   224,     0,
      33,     0,   227,     0,     0,    51,   130,     0,     0,   237,
      27,     0,     0,   241,    28,   246,   245,   133,   161,   163,
       0,     0,     0,    98,     0,     0,     0,    48,    27,    27,
      27,     0,     0,    49,    72,     0,     0,   138,   137,   128,
     153,    52,    28,   158,     0,    28,     0,    58,    60,     0,
       0,    28,     0,   157,     0,    12,    19,   223,     0,     0,
     213,     0,     0,   215,   172,   164,   165,   169,   170,   171,
     166,   168,   167,   196,     0,     0,   195,     0,   149,   119,
       0,   116,     0,    27,    27,   243,     0,    28,   240,   234,
     236,   248,     0,   242,   101,   100,     0,     0,   152,     0,
       0,   126,    64,     0,     0,     0,   131,    48,    27,    27,
      27,     0,     0,    27,    48,    56,    48,    63,   156,    48,
     284,     9,    20,     0,     0,     0,   249,   249,   253,    15,
       0,   204,   206,     0,     0,     0,     0,    77,   174,     0,
     150,   120,   115,    54,     0,     0,   235,   238,   244,     0,
       0,   102,   134,   107,   106,     0,     0,    48,    68,    48,
      69,     0,     0,     0,   139,   154,   159,     0,    53,    62,
      61,     0,     0,   202,   202,    14,   250,   252,   251,     0,
     230,     0,     0,   254,    27,   207,     0,     7,   216,     0,
       0,     0,    74,     0,    82,     0,     0,    76,   121,    48,
      57,   104,   103,     0,     0,     0,   108,   135,    67,    65,
      48,    71,    48,   132,   202,   202,   197,   197,    19,    17,
     249,   232,    16,    18,   253,     0,     0,    77,    77,    78,
      77,     0,    55,   105,   110,   109,     0,    70,    66,   197,
     197,    38,    38,     0,   231,     0,   208,     0,    77,    84,
      83,    79,    77,   111,    38,    38,   271,    40,    25,    40,
       8,   233,   209,    81,    80,    40,    40,     0,     0,    36,
      40,    39,     0,     0,     0,    25,    21,    37,    23,    22,
      24,    41
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -343,  -343,  -343,  -343,  -343,  -343,   155,    34,  -343,  -302,
    -343,  -106,   -57,  -343,    -7,  -343,  -211,  -311,   -58,   425,
     -40,   -86,   196,  -343,     6,  -343,  -343,  -190,  -315,  -343,
    -343,  -343,  -343,  -343,  -343,   -19,  -343,  -343,    -1,   340,
    -343,  -105,   -72,   -92,   175,   -95,  -343,  -343,  -342,   280,
    -343,  -343,  -161,  -343,   -63,   452,    26,   -18,  -343,     4,
    -343,  -343,   247,  -343,   419,   351,   324,  -343,  -343,   109,
      17,  -343,   148,  -343,  -343,  -225,  -343,  -164,  -343,  -343,
    -343,    15,  -343,  -343,   -13,    80,   159,     3,  -343,   255,
     349,   355,   360,   365,   372,  -343,  -289,   446,   -23,  -343
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,    65,    66,   221,   138,   306,   379,   304,
     372,   120,   293,   234,    29,    30,   518,   507,   519,    31,
     283,   129,   284,    32,    33,   237,   238,   444,   386,    34,
      35,    36,    37,    98,   160,    38,   204,    39,   198,    85,
      86,   199,   210,   363,   217,   294,   110,   111,   313,    41,
      42,    43,   178,    44,   149,    45,   236,    73,   434,   435,
     145,   146,   147,   143,    68,    69,   101,   102,   103,   429,
     430,   179,   259,   260,   261,    47,   180,   181,   182,   431,
     436,    48,    49,    50,    51,    52,    53,    54,   508,    55,
      56,    57,    58,    59,    60,    61,   104,   117,   118,    62
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      40,   174,   106,    79,   380,    82,    67,   107,    77,   314,
     389,     4,   257,   128,   134,   374,   258,     1,    11,   152,
      11,  -211,   209,    84,    99,    63,     4,    46,   214,   156,
       4,   113,   270,   119,    11,   124,   125,    20,   122,   230,
     194,   216,   211,   245,    40,   169,  -214,    64,   211,   231,
      71,    46,    20,   109,   200,   106,    20,   114,   201,   130,
     130,   215,   139,   140,  -214,    71,    80,   135,   195,   109,
     246,   123,  -214,    18,   233,    72,    26,    99,    77,   150,
     151,   422,  -214,   272,    81,   153,   157,   158,   161,   232,
      72,   374,  -214,   167,   141,   276,   278,    19,    19,    40,
     205,   340,   342,   341,   171,   417,   163,   206,   273,   280,
     274,   350,   186,   142,   164,   296,    11,    23,    23,   299,
     300,   136,   225,   254,   183,   265,   123,    19,   479,   279,
     154,    84,   255,   165,   130,   166,   351,   137,   274,   220,
     130,   226,   235,   506,   155,   239,    13,    23,    40,   333,
     162,    40,   240,   511,   241,   512,   256,   168,   243,   302,
     426,   427,   247,   248,   249,   428,   493,   251,   291,   211,
     124,   170,    70,   172,   267,   396,   258,   358,   130,   186,
     186,   509,   334,   353,   354,   289,   108,   112,   175,   184,
     360,   183,   183,   515,   516,    19,   271,   211,   193,   186,
     196,     4,   126,   338,   374,   211,   290,   203,   130,   219,
     359,   183,   228,   140,   130,    23,   223,   303,   287,   130,
     227,   127,    19,   355,   440,   441,   445,    20,   229,   126,
      70,   148,   244,    19,   250,    74,    75,    76,   394,   395,
      19,   328,    23,   264,   329,   330,   266,   281,   127,   315,
     469,   470,   171,    23,    18,   132,   184,   184,   263,   133,
      23,   130,   356,   411,   412,   186,   282,   186,   185,   186,
     314,   346,   314,   348,   349,   202,   184,   183,   292,   183,
     323,   183,   295,   130,   297,   361,   305,   499,   500,   130,
     501,   348,   393,   301,     4,     5,   222,   130,   522,    70,
     472,   470,   124,   413,   523,   524,   416,   211,   513,   307,
     383,   324,   514,   325,   316,   113,   376,   326,   377,   378,
      20,   473,   231,   387,   387,   285,   390,   327,   402,   331,
     392,   164,   335,   131,   131,   185,   185,   337,   130,   186,
     186,   286,   184,   336,   184,   339,   184,   343,   128,   405,
     347,   183,   183,   268,   269,   185,   344,    74,   116,   364,
      83,   415,   288,   130,   187,   365,   366,   367,   136,   369,
     130,   381,   130,   382,   391,   130,   398,   474,   399,   400,
     451,   452,   401,   403,   407,   406,   308,   457,   309,   423,
     448,   148,   408,   409,   130,   414,   433,   438,   463,   130,
     424,   425,   521,   491,   492,   466,   467,   442,   131,   298,
     449,   447,   443,   130,   131,   130,   184,   184,   450,   531,
     453,   185,   454,   185,   455,   185,   504,   505,   456,   460,
     461,   187,   187,   462,   483,   484,   485,   476,   464,   477,
     478,   465,   387,   481,   480,   486,   489,   490,   495,   497,
     173,   187,   131,   526,   528,   130,   517,   529,   188,   468,
     375,   530,   527,   373,   189,   121,   130,   503,   130,   190,
      87,   105,   212,   352,   191,   498,   368,   100,   496,   310,
     502,   192,   131,   388,   388,   397,   432,   494,   131,   317,
     144,   224,   252,   131,     0,   185,   185,     0,   525,     0,
       0,     0,     0,     0,     0,     0,   520,     0,   520,     0,
     315,     0,   315,     0,   520,   520,     0,   187,     0,   187,
       0,   187,     0,     0,   105,   188,   188,     0,     0,   421,
       0,   189,   189,     0,     0,   131,   190,   190,     0,   373,
       0,   191,   191,   439,     0,   188,     0,   446,   192,   192,
       0,   189,     0,   410,     0,     0,   190,   131,     0,     0,
     418,   191,   419,   131,     0,   420,     0,     0,   192,     0,
       0,   131,     0,     0,     0,   316,     0,   316,    87,   218,
       0,     0,     0,   318,     0,     0,     0,     0,     0,   319,
     471,   187,   187,     0,   320,   475,     0,     0,     0,   321,
       0,   242,   388,   458,     0,   459,   322,     0,     0,     0,
       0,   188,   131,   188,   253,   188,     0,   189,     0,   189,
       0,   189,   190,   262,   190,     0,   190,   191,     4,   191,
       0,   191,     0,     0,   192,     0,   192,   131,   192,     0,
       0,     0,     0,     4,   131,   482,   131,   218,     0,   131,
       0,     0,   373,     0,    20,     0,   487,    90,   488,   115,
      92,    93,   370,    95,    96,    97,     0,     0,   131,    20,
     371,     0,    90,   131,   115,    92,    93,    94,    95,    96,
      97,     0,     0,     0,     0,   188,   188,   131,     0,   131,
       0,   189,   189,     0,     0,     0,   190,   190,     0,     0,
       0,   191,   191,     0,     0,     0,     0,     0,   192,   192,
       0,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     8,     9,    10,     0,     0,     0,     0,   218,   131,
       0,    14,    15,    16,     0,     0,     0,    18,   218,     0,
     131,   218,   131,     0,     0,     0,     0,   218,    21,     0,
     317,     0,   317,     0,     0,     4,     5,     6,     7,     8,
       9,    10,     0,    11,    12,    13,     0,     0,   312,    14,
      15,    16,    17,     0,    88,    18,    19,     0,     0,     0,
       0,    20,    89,   262,    90,    83,    91,    92,    93,    94,
      95,    96,    97,     0,    22,     0,    23,     0,    24,     4,
       0,     0,    78,     0,     0,    27,    28,     0,     5,     6,
       7,     8,     9,    10,     0,     0,     0,     0,     0,     0,
       0,    14,    15,    16,     0,    20,     0,    18,    90,     0,
     115,    92,    93,   370,    95,    96,    97,     0,    21,     0,
       0,   437,     0,     0,   318,     0,   318,     0,     0,     0,
     319,     0,   319,     0,   311,   320,     0,   320,   312,     0,
     321,     0,   321,     0,     0,     0,     0,   322,     0,   322,
       4,     5,     6,     7,     8,     9,    10,     0,    11,    12,
       0,     0,     0,     0,    14,    15,    16,    17,     0,    88,
      18,    19,     0,     0,     0,     0,    20,    89,     0,    90,
      83,    91,    92,    93,    94,    95,    96,    97,     0,    22,
       0,    23,     0,    24,     0,     0,     0,    78,     0,     0,
      27,    28,     4,     5,     6,     7,     8,     9,    10,     0,
      11,    12,     0,     0,     0,     0,    14,    15,    16,    17,
       0,     0,    18,    19,     0,     0,     0,     0,    20,     0,
       0,     0,    83,    21,     0,     0,   207,     0,     0,     0,
       0,    22,     0,    23,     0,    24,     0,     0,     0,    78,
     213,     0,    27,    28,     4,     5,     6,     7,     8,     9,
      10,     0,    11,    12,     0,     0,     0,     0,    14,    15,
      16,    17,     0,     0,    18,    19,     0,     0,     0,     0,
      20,     0,     0,     0,    83,    21,     0,     0,   207,     0,
       0,     0,     0,    22,     0,    23,     0,    24,     0,     0,
       0,    78,   277,     0,    27,    28,     4,     5,     6,     7,
       8,     9,    10,     0,    11,    12,     0,     0,     0,     0,
      14,    15,    16,    17,     0,     0,    18,    19,     0,     0,
       0,     0,    20,     0,     0,     0,    83,    21,     0,     0,
     207,     0,     0,     0,     0,    22,     0,    23,     0,    24,
       0,     0,     0,    78,   357,     0,    27,    28,     4,     5,
       6,     7,     8,     9,    10,     0,    11,    12,    13,     0,
       0,     0,    14,    15,    16,    17,     0,     0,    18,    19,
       0,     0,     0,     0,    20,     0,     0,     0,     0,    21,
       0,     0,     0,     0,     0,     0,     0,    22,     0,    23,
       0,    24,     0,     0,     0,    25,     0,    26,    27,    28,
       4,     5,     6,     7,     8,     9,    10,     0,    11,    12,
      13,     0,     0,     0,    14,    15,    16,    17,     0,     0,
      18,    19,     0,     0,     0,     0,    20,     0,     0,     0,
       0,    21,     0,     0,     0,     0,     0,     0,     0,    22,
       0,    23,     0,    24,     0,     0,     0,    78,     0,    26,
      27,    28,     4,     5,     6,     7,     8,     9,    10,     0,
      11,    12,     0,     0,     0,     0,    14,    15,    16,    17,
       0,     0,    18,    19,     0,     0,     0,     0,    20,     0,
       0,     0,     0,    21,     0,     0,   207,     0,     0,     0,
       0,    22,     0,    23,     0,    24,     0,     0,     0,    78,
     208,     0,    27,    28,     4,     5,     6,     7,     8,     9,
      10,     0,    11,    12,     0,     0,     0,     0,    14,    15,
      16,    17,     0,     0,    18,    19,     0,     0,     0,     0,
      20,     0,     0,     0,     0,    21,     0,     0,   207,     0,
       0,     0,     0,    22,     0,    23,     0,    24,     0,     0,
       0,    78,   332,     0,    27,    28,     4,     5,     6,     7,
       8,     9,    10,     0,    11,    12,     0,     0,     0,     0,
      14,    15,    16,    17,     0,     0,    18,    19,     0,     0,
       0,     0,    20,     0,     0,     0,    83,    21,     0,     0,
     362,     0,     0,     0,     0,    22,     0,    23,     0,    24,
       0,     0,     0,    78,     0,     0,    27,    28,     4,     5,
       6,     7,     8,     9,    10,     0,    11,    12,   384,     0,
       0,     0,    14,    15,    16,    17,     0,     0,    18,    19,
       0,     0,     0,     0,    20,   385,     0,     0,     0,    21,
       0,     0,     0,     0,     0,     0,     0,    22,     0,    23,
       0,    24,     0,     0,     0,    78,     0,     0,    27,    28,
       4,     5,     6,     7,     8,     9,    10,     0,    11,    12,
       0,     0,     0,     0,    14,    15,    16,    17,     0,     0,
      18,    19,     0,     0,     0,     0,    20,     0,     0,     0,
      83,    21,     0,     0,     0,     0,     0,     0,     0,    22,
       0,    23,     0,    24,     0,     0,     0,    78,     0,     0,
      27,    28,     4,     5,     6,     7,     8,     9,    10,     0,
      11,    12,     0,     0,     0,     0,    14,    15,    16,    17,
       0,   159,    18,    19,     0,     0,     0,     0,    20,     0,
       0,     0,     0,    21,     0,     0,     0,     0,     0,     0,
       0,    22,     0,    23,     0,    24,     0,     0,     0,    78,
       0,     0,    27,    28,     4,     5,     6,     7,     8,     9,
      10,     0,    11,    12,     0,     0,     0,     0,    14,    15,
      16,    17,     0,   197,    18,    19,     0,     0,     0,     0,
      20,     0,     0,     0,     0,    21,     0,     0,     0,     0,
       0,     0,     0,    22,     0,    23,     0,    24,     0,     0,
       0,    78,     0,     0,    27,    28,     4,     5,     6,     7,
       8,     9,    10,     0,    11,    12,     0,     0,     0,     0,
      14,    15,    16,    17,     0,   275,    18,    19,     0,     0,
       0,     0,    20,     0,     0,     0,     0,    21,     0,     0,
       0,     0,     0,     0,     0,    22,     0,    23,     0,    24,
       0,     0,     0,    78,     0,     0,    27,    28,     4,     5,
       6,     7,     8,     9,    10,     0,    11,    12,     0,     0,
       0,     0,    14,    15,    16,    17,     0,     0,    18,    19,
       0,     0,     0,     0,    20,     0,     0,     0,     0,    21,
       0,     0,     0,     0,     0,     0,     0,    22,     0,    23,
       0,    24,   345,     0,     0,    78,     0,     0,    27,    28,
       4,     5,     6,     7,     8,     9,    10,     0,    11,    12,
       0,     0,     0,     0,    14,    15,    16,    17,     0,     0,
      18,    19,     0,     0,     0,     0,    20,     0,     0,     0,
       0,    21,     0,     0,     0,     0,     0,     0,     0,    22,
       0,    23,     0,    24,   404,     0,     0,    78,     0,     0,
      27,    28,     4,     5,     6,     7,     8,     9,    10,     0,
      11,    12,     0,     0,     0,     0,    14,    15,    16,    17,
       0,     0,    18,    19,     0,     0,     0,     0,    20,     0,
       0,     0,     0,    21,     0,     0,     0,     0,     0,     0,
       0,    22,     0,    23,     0,    24,     0,     0,     0,    78,
       0,   517,    27,    28,     4,     5,     6,     7,     8,     9,
      10,     0,    11,    12,     0,     0,     0,     0,    14,    15,
      16,    17,     0,     0,    18,    19,     0,     0,     0,     0,
      20,     0,     0,     0,     0,    21,     0,     0,     0,     0,
       0,     0,     0,    22,     0,    23,     0,    24,     0,     0,
       0,    78,     0,     0,    27,    28,     4,     5,     6,     7,
       8,     9,    10,     0,    11,     0,     0,     0,     0,     0,
      14,    15,    16,     0,     0,     0,    18,     0,     0,     0,
       0,     0,    20,     0,     0,     0,    83,    21,     0,     0,
       4,     5,     6,     7,     8,     9,    10,     0,    11,   176,
       0,     0,     0,   177,    14,    15,    16,     0,     0,     4,
      18,     0,     0,     0,     0,     0,    20,     0,     0,     0,
       0,    21,     0,    90,    83,   115,    92,    93,    94,    95,
      96,    97,   116,   176,     0,    20,     0,   177,    90,   173,
     115,    92,    93,   370,    95,    96,    97,     0,     0,     0,
       0,   510,    90,    83,   115,    92,    93,    94,    95,    96,
      97,   116
};

static const yytype_int16 yycheck[] =
{
       1,   107,    25,    22,   306,    23,     3,    26,    21,   234,
     325,     3,   176,    53,    54,   304,   177,    26,    11,    82,
      11,    21,   127,    24,    25,     0,     3,     1,   133,    86,
       3,    28,   196,    48,    11,    48,    49,    29,    45,    39,
      21,   133,   128,    22,    45,   102,     3,    38,   134,    49,
      14,    25,    29,    46,    46,    78,    29,    46,    50,    53,
      54,   133,    48,    49,    21,    14,    30,    64,    49,    46,
      49,    45,    29,    23,    31,    39,    52,    78,    91,    80,
      81,   370,    39,    22,    48,    30,    87,    88,    89,   146,
      39,   380,    49,   100,    31,   200,   201,    24,    24,   100,
     119,   262,   266,   264,   105,    31,    22,   126,    47,   201,
      49,    22,   109,    50,    30,   210,    11,    44,    44,   214,
     215,    30,   141,    50,   109,   182,   100,    24,   443,   201,
      47,   132,   172,    49,   128,    51,    47,    46,    49,   136,
     134,   142,   149,    10,    49,   152,    13,    44,   149,   254,
      51,   152,   153,   495,   155,   497,   175,    49,   159,   216,
      35,    36,   163,   164,   165,    40,   468,   168,   208,   255,
     183,    51,    13,    51,   193,   336,   337,   282,   172,   176,
     177,   492,   254,   278,   279,   204,    27,    28,    31,   109,
     282,   176,   177,   504,   505,    24,   197,   283,    31,   196,
      46,     3,    31,   260,   493,   291,   207,    41,   202,    44,
     282,   196,    48,    49,   208,    44,    44,   218,   203,   213,
      21,    50,    24,   280,    29,    30,    28,    29,    21,    31,
      71,    72,    51,    24,    51,     4,     5,     6,   333,   334,
      24,   242,    44,    49,   245,   246,    46,    31,    50,   234,
      48,    49,   253,    44,    23,    46,   176,   177,    47,    50,
      44,   255,   281,   358,   359,   262,    50,   264,   109,   266,
     495,   272,   497,   274,   275,   116,   196,   262,    49,   264,
      45,   266,    49,   277,    47,   286,    47,   477,   478,   283,
     480,   292,   332,    49,     3,     4,   137,   291,   509,   140,
      48,    49,   315,   360,   515,   516,   363,   393,   498,    51,
     311,    49,   502,    49,   234,   312,    13,    45,    15,    16,
      29,    48,    49,   324,   325,    34,   327,    30,   347,    22,
     331,    30,    47,    53,    54,   176,   177,    49,   332,   336,
     337,    50,   262,    30,   264,    51,   266,    47,   388,   350,
      31,   336,   337,   194,   195,   196,    47,     4,    41,    51,
      33,   362,   203,   357,   109,    51,    51,    51,    30,    51,
     364,    48,   366,    39,    22,   369,    47,   434,    31,    31,
     399,   400,    47,    47,    51,    31,   227,   406,   229,    44,
     391,   232,    51,    51,   388,    51,    35,    51,   417,   393,
      44,    44,   508,   466,   467,   423,   424,    45,   128,   213,
      51,    45,    49,   407,   134,   409,   336,   337,    51,   525,
      31,   262,    31,   264,    31,   266,   489,   490,    47,    51,
      51,   176,   177,    51,   453,   454,   455,    31,    44,   440,
     441,    44,   443,    28,   445,    31,   464,   465,    31,    31,
      48,   196,   172,    45,    45,   449,    52,    45,   109,   425,
     305,    45,   520,   304,   109,    40,   460,   486,   462,   109,
      24,    25,   132,   277,   109,   476,   301,    25,   474,   232,
     481,   109,   202,   324,   325,   337,   377,   470,   208,   234,
      71,   140,   168,   213,    -1,   336,   337,    -1,   517,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   507,    -1,   509,    -1,
     495,    -1,   497,    -1,   515,   516,    -1,   262,    -1,   264,
      -1,   266,    -1,    -1,    78,   176,   177,    -1,    -1,   370,
      -1,   176,   177,    -1,    -1,   255,   176,   177,    -1,   380,
      -1,   176,   177,   384,    -1,   196,    -1,   388,   176,   177,
      -1,   196,    -1,   357,    -1,    -1,   196,   277,    -1,    -1,
     364,   196,   366,   283,    -1,   369,    -1,    -1,   196,    -1,
      -1,   291,    -1,    -1,    -1,   495,    -1,   497,   132,   133,
      -1,    -1,    -1,   234,    -1,    -1,    -1,    -1,    -1,   234,
     431,   336,   337,    -1,   234,   436,    -1,    -1,    -1,   234,
      -1,   155,   443,   407,    -1,   409,   234,    -1,    -1,    -1,
      -1,   262,   332,   264,   168,   266,    -1,   262,    -1,   264,
      -1,   266,   262,   177,   264,    -1,   266,   262,     3,   264,
      -1,   266,    -1,    -1,   262,    -1,   264,   357,   266,    -1,
      -1,    -1,    -1,     3,   364,   449,   366,   201,    -1,   369,
      -1,    -1,   493,    -1,    29,    -1,   460,    32,   462,    34,
      35,    36,    37,    38,    39,    40,    -1,    -1,   388,    29,
      45,    -1,    32,   393,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    -1,    -1,   336,   337,   407,    -1,   409,
      -1,   336,   337,    -1,    -1,    -1,   336,   337,    -1,    -1,
      -1,   336,   337,    -1,    -1,    -1,    -1,    -1,   336,   337,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,
       6,     7,     8,     9,    -1,    -1,    -1,    -1,   282,   449,
      -1,    17,    18,    19,    -1,    -1,    -1,    23,   292,    -1,
     460,   295,   462,    -1,    -1,    -1,    -1,   301,    34,    -1,
     495,    -1,   497,    -1,    -1,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    13,    -1,    -1,    54,    17,
      18,    19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,   337,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    -1,    42,    -1,    44,    -1,    46,     3,
      -1,    -1,    50,    -1,    -1,    53,    54,    -1,     4,     5,
       6,     7,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    17,    18,    19,    -1,    29,    -1,    23,    32,    -1,
      34,    35,    36,    37,    38,    39,    40,    -1,    34,    -1,
      -1,    45,    -1,    -1,   495,    -1,   497,    -1,    -1,    -1,
     495,    -1,   497,    -1,    50,   495,    -1,   497,    54,    -1,
     495,    -1,   497,    -1,    -1,    -1,    -1,   495,    -1,   497,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    -1,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    42,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,
      53,    54,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,
      -1,    -1,    33,    34,    -1,    -1,    37,    -1,    -1,    -1,
      -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,
      51,    -1,    53,    54,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    -1,    33,    34,    -1,    -1,    37,    -1,
      -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,
      -1,    50,    51,    -1,    53,    54,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,
      17,    18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    -1,    33,    34,    -1,    -1,
      37,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,
      -1,    -1,    -1,    50,    51,    -1,    53,    54,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    13,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,    24,
      -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    34,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,
      -1,    46,    -1,    -1,    -1,    50,    -1,    52,    53,    54,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      13,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,
      23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,
      -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,    52,
      53,    54,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,
      -1,    -1,    -1,    34,    -1,    -1,    37,    -1,    -1,    -1,
      -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,
      51,    -1,    53,    54,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    -1,    -1,    34,    -1,    -1,    37,    -1,
      -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,
      -1,    50,    51,    -1,    53,    54,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,
      17,    18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    -1,    33,    34,    -1,    -1,
      37,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,
      -1,    -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    13,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    -1,    -1,    -1,    34,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,
      -1,    46,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,
      23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,
      33,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,
      53,    54,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,
      -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,
      -1,    -1,    53,    54,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,
      -1,    50,    -1,    -1,    53,    54,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,
      17,    18,    19,    20,    -1,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    -1,    -1,    34,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,
      -1,    -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    -1,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,    24,
      -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    34,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,
      -1,    46,    47,    -1,    -1,    50,    -1,    -1,    53,    54,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,
      23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,
      -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    44,    -1,    46,    47,    -1,    -1,    50,    -1,    -1,
      53,    54,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,
      -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,
      -1,    52,    53,    54,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,
      -1,    50,    -1,    -1,    53,    54,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    -1,    -1,    -1,    -1,    -1,
      17,    18,    19,    -1,    -1,    -1,    23,    -1,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    -1,    33,    34,    -1,    -1,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    46,
      -1,    -1,    -1,    50,    17,    18,    19,    -1,    -1,     3,
      23,    -1,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,
      -1,    34,    -1,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    46,    -1,    29,    -1,    50,    32,    48,
      34,    35,    36,    37,    38,    39,    40,    -1,    -1,    -1,
      -1,    45,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    26,    56,    57,     3,     4,     5,     6,     7,     8,
       9,    11,    12,    13,    17,    18,    19,    20,    23,    24,
      29,    34,    42,    44,    46,    50,    52,    53,    54,    69,
      70,    74,    78,    79,    84,    85,    86,    87,    90,    92,
      93,   104,   105,   106,   108,   110,   111,   130,   136,   137,
     138,   139,   140,   141,   142,   144,   145,   146,   147,   148,
     149,   150,   154,     0,    38,    58,    59,   142,   119,   120,
     141,    14,    39,   112,     4,     5,     6,   139,    50,    90,
      30,    48,   112,    33,    93,    94,    95,   152,    22,    30,
      32,    34,    35,    36,    37,    38,    39,    40,    88,    93,
     110,   121,   122,   123,   151,   152,   153,    90,   141,    46,
     101,   102,   141,   142,    46,    34,    41,   152,   153,    48,
      66,    74,    69,   111,   139,   139,    31,    50,    75,    76,
      79,   104,    46,    50,    75,   142,    30,    46,    61,    48,
      49,    31,    50,   118,   119,   115,   116,   117,   141,   109,
      93,    93,   109,    30,    47,    49,    67,    93,    93,    22,
      89,    93,    51,    22,    30,    49,    51,    69,    49,    67,
      51,    93,    51,    48,    66,    31,    46,    50,   107,   126,
     131,   132,   133,   136,   140,   141,   142,   144,   145,   146,
     147,   148,   149,    31,    21,    49,    46,    22,    93,    96,
      46,    50,   141,    41,    91,    90,    90,    37,    51,    96,
      97,    76,    94,    51,    96,    97,    98,    99,   152,    44,
     142,    60,   141,    44,   120,    90,    93,    21,    48,    21,
      39,    49,    67,    31,    68,    69,   111,    80,    81,    69,
      93,    93,   152,    93,    51,    22,    49,    93,    93,    93,
      51,    93,   121,   152,    50,    75,    90,   132,   107,   127,
     128,   129,   152,    47,    49,    67,    46,    90,   141,   141,
     132,    93,    22,    47,    49,    22,    96,    51,    96,    97,
      98,    31,    50,    75,    77,    34,    50,   136,   141,    90,
      93,    75,    49,    67,   100,    49,   100,    47,    77,   100,
     100,    49,    67,    93,    64,    47,    62,    51,   141,   141,
     117,    50,    54,   103,   130,   136,   140,   144,   145,   146,
     147,   148,   149,    45,    49,    49,    45,    30,    93,    93,
      93,    22,    51,    96,    97,    47,    30,    49,    67,    51,
     107,   107,   132,    47,    47,    47,    93,    31,    93,    93,
      22,    47,    77,   100,   100,    67,    90,    51,    96,    97,
      98,    93,    37,    98,    51,    51,    51,    51,    99,    51,
      37,    45,    65,   141,   151,    61,    13,    15,    16,    63,
      64,    48,    39,    93,    13,    30,    83,    93,   141,    83,
      93,    22,    93,    75,   100,   100,   107,   127,    47,    31,
      31,    47,    90,    47,    47,    93,    31,    51,    51,    51,
      77,   100,   100,    67,    51,    93,    67,    31,    77,    77,
      77,   141,   151,    44,    44,    44,    35,    36,    40,   124,
     125,   134,   124,    35,   113,   114,   135,    45,    51,   141,
      29,    30,    45,    49,    82,    28,   141,    45,    93,    51,
      51,    90,    90,    31,    31,    31,    47,    90,    77,    77,
      51,    51,    51,    90,    44,    44,   112,   112,    62,    48,
      49,   141,    48,    48,    67,   141,    31,    93,    93,    83,
      93,    28,    77,    90,    90,    90,    31,    77,    77,   112,
     112,   109,   109,    64,   125,    31,   114,    31,    93,    82,
      82,    82,    93,    90,   109,   109,    10,    72,   143,    72,
      45,   103,   103,    82,    82,    72,    72,    52,    71,    73,
      93,    66,    71,    71,    71,    90,    45,    73,    45,    45,
      45,    66
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    55,    56,    56,    57,    57,    57,    58,    58,    59,
      60,    60,    61,    61,    62,    62,    63,    63,    63,    64,
      64,    65,    65,    65,    65,    66,    66,    67,    67,    68,
      68,    69,    69,    70,    70,    70,    71,    71,    72,    72,
      73,    73,    74,    74,    75,    75,    76,    76,    77,    77,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    80,    79,    81,    79,    82,    82,    83,
      83,    83,    83,    83,    83,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      85,    85,    85,    86,    86,    86,    87,    87,    87,    87,
      87,    87,    88,    88,    88,    88,    88,    89,    89,    89,
      89,    89,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    91,    91,    91,    91,
      92,    92,    93,    94,    94,    95,    95,    95,    95,    95,
      95,    96,    96,    97,    97,    98,    98,    99,   100,   100,
     101,   101,   102,   102,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   104,   105,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   108,   108,   109,   109,   110,
     110,   111,   112,   112,   112,   112,   112,   113,   113,   114,
     115,   115,   116,   116,   117,   117,   117,   118,   118,   119,
     119,   120,   120,   120,   121,   121,   122,   122,   123,   123,
     124,   124,   125,   125,   126,   127,   127,   128,   128,   129,
     129,   130,   130,   131,   131,   132,   132,   133,   133,   134,
     134,   134,   134,   135,   135,   136,   136,   137,   137,   138,
     138,   139,   140,   140,   140,   140,   140,   140,   141,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     151,   151,   151,   151,   151,   151,   151,   152,   153,   153,
     154
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     0,     2,     2,     6,     9,     5,
       0,     1,     0,     2,     0,     2,     3,     3,     3,     0,
       2,     7,     8,     7,     8,     0,     1,     0,     1,     0,
       1,     1,     2,     4,     2,     1,     1,     2,     0,     2,
       0,     3,     0,     3,     1,     2,     1,     1,     0,     1,
       2,     4,     4,     6,     6,     8,     5,     7,     4,     2,
       4,     6,     6,     5,     5,     7,     8,     7,     6,     6,
       8,     7,     4,     0,     7,     0,     7,     0,     2,     4,
       5,     5,     2,     4,     4,     1,     1,     1,     1,     1,
       1,     3,     2,     3,     3,     4,     3,     1,     4,     1,
       5,     5,     6,     7,     7,     8,     6,     6,     7,     8,
       8,     9,     2,     2,     3,     5,     4,     2,     2,     3,
       4,     5,     1,     1,     1,     1,     5,     2,     4,     3,
       4,     5,     7,     4,     6,     7,     0,     2,     2,     4,
       1,     3,     2,     0,     2,     1,     3,     2,     3,     4,
       5,     1,     3,     2,     4,     1,     3,     2,     1,     3,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     5,     5,     0,     2,     1,
       2,     3,     0,     3,     5,     3,     5,     1,     3,     4,
       0,     1,     1,     3,     1,     3,     5,     0,     1,     1,
       3,     1,     3,     4,     3,     2,     1,     3,     0,     2,
       1,     3,     2,     4,     3,     3,     2,     1,     3,     0,
       2,     4,     5,     3,     4,     0,     2,     1,     3,     0,
       1,     1,     1,     0,     1,     1,     2,     1,     2,     1,
       2,     1,     1,     1,     2,     2,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


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
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
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
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
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

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

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

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
     '$$ = $1'.

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
#line 40 "lang11d" /* yacc.c:1646  */
    { gRootParseNode = (PyrParseNode*)(yyvsp[0]); gParserResult = 1; }
#line 1916 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 3:
#line 42 "lang11d" /* yacc.c:1646  */
    { gRootParseNode = (PyrParseNode*)(yyvsp[0]); gParserResult = 2; }
#line 1922 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 45 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 1928 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 5:
#line 47 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-1]), (PyrParseNode*)(yyvsp[0])); }
#line 1934 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 49 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-1]), (PyrParseNode*)(yyvsp[0])); }
#line 1940 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 53 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrClassNode((PyrSlotNode*)(yyvsp[-5]), (PyrSlotNode*)(yyvsp[-4]),
					(PyrVarListNode*)(yyvsp[-2]), (PyrMethodNode*)(yyvsp[-1]), 0);
				}
#line 1948 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 57 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrClassNode((PyrSlotNode*)(yyvsp[-8]), (PyrSlotNode*)(yyvsp[-4]),
					(PyrVarListNode*)(yyvsp[-2]), (PyrMethodNode*)(yyvsp[-1]),
					(PyrSlotNode*)(yyvsp[-6]));
				}
#line 1957 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 64 "lang11d" /* yacc.c:1646  */
    {
					(yyval) = (intptr_t)newPyrClassExtNode((PyrSlotNode*)(yyvsp[-3]), (PyrMethodNode*)(yyvsp[-1]));
				}
#line 1965 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 69 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 1971 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 73 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 1977 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 75 "lang11d" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 1983 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 78 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 1989 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 80 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-1]), (PyrParseNode*)(yyvsp[0])); }
#line 1995 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 84 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrVarListNode((PyrVarDefNode*)(yyvsp[-1]), varClass); }
#line 2001 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 86 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrVarListNode((PyrVarDefNode*)(yyvsp[-1]), varInst); }
#line 2007 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 88 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrVarListNode((PyrVarDefNode*)(yyvsp[-1]), varConst); }
#line 2013 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 91 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 2019 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 93 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-1]), (PyrParseNode*)(yyvsp[0])); }
#line 2025 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 97 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrMethodNode((PyrSlotNode*)(yyvsp[-6]), (PyrSlotNode*)(yyvsp[-2]),
					(PyrArgListNode*)(yyvsp[-4]), (PyrVarListNode*)(yyvsp[-3]), (PyrParseNode*)(yyvsp[-1]), 0); }
#line 2032 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 100 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrMethodNode((PyrSlotNode*)(yyvsp[-6]), (PyrSlotNode*)(yyvsp[-2]),
					(PyrArgListNode*)(yyvsp[-4]), (PyrVarListNode*)(yyvsp[-3]), (PyrParseNode*)(yyvsp[-1]), 1); }
#line 2039 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 103 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrMethodNode((PyrSlotNode*)(yyvsp[-6]), (PyrSlotNode*)(yyvsp[-2]),
					(PyrArgListNode*)(yyvsp[-4]), (PyrVarListNode*)(yyvsp[-3]), (PyrParseNode*)(yyvsp[-1]), 0); }
#line 2046 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 106 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrMethodNode((PyrSlotNode*)(yyvsp[-6]), (PyrSlotNode*)(yyvsp[-2]),
					(PyrArgListNode*)(yyvsp[-4]), (PyrVarListNode*)(yyvsp[-3]), (PyrParseNode*)(yyvsp[-1]), 1); }
#line 2053 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 124 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrDropNode((PyrParseNode*)(yyvsp[-1]), (PyrParseNode*)(yyvsp[0])); }
#line 2059 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 128 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[-1]), false); }
#line 2065 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 130 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)(yyvsp[-1]), (PyrParseNode*)(yyvsp[0]), false); }
#line 2071 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 132 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrBlockNode(NULL, NULL, (PyrParseNode*)(yyvsp[0]), false); }
#line 2077 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 137 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrDropNode((PyrParseNode*)(yyvsp[-1]), (PyrParseNode*)(yyvsp[0])); }
#line 2083 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 140 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 2089 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 142 "lang11d" /* yacc.c:1646  */
    { (yyval) = (yyvsp[-1]); }
#line 2095 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 146 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrReturnNode(NULL); }
#line 2101 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 148 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrReturnNode((PyrParseNode*)(yyvsp[-1])); }
#line 2107 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 152 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrBlockReturnNode(); }
#line 2113 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 154 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrReturnNode((PyrParseNode*)(yyvsp[-1])); }
#line 2119 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 159 "lang11d" /* yacc.c:1646  */
    {
					(yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-1]), (PyrParseNode*)(yyvsp[0]));
				}
#line 2127 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 168 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 2133 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 173 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[-1]), (PyrParseNode*)(yyvsp[0]), 0, 0);
			}
#line 2141 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 177 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0]), 0, 0);
			}
#line 2149 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 181 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[-3]), NULL, NULL, (PyrParseNode*)(yyvsp[0]));
			}
#line 2157 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 185 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[-5]), (PyrParseNode*)(yyvsp[-3]),
						(PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0]));
			}
#line 2166 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 190 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[-4]), NULL, NULL, (PyrParseNode*)(yyvsp[0]));
			}
#line 2174 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 194 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[-6]), (PyrParseNode*)(yyvsp[-3]),
						(PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0]));
			}
#line 2183 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 199 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)(yyvsp[-2]))) {
					SetRaw(&((PyrPushNameNode*)(yyvsp[-2]))->mSlot, s_this);
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				selectornode = newPyrSlotNode(&slot);
				args = linkAfterHead(
					(PyrParseNode*)(yyvsp[-2]),
					newPyrPushLitNode((PyrSlotNode*)(yyvsp[-4]), NULL));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[-1]), 0);
			}
#line 2205 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 217 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_performList);
				selectornode = newPyrSlotNode(&slot);
				args = linkAfterHead(
					(PyrParseNode*)(yyvsp[-2]),
					newPyrPushLitNode((PyrSlotNode*)(yyvsp[-5]), NULL));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[-1]), 0);
			}
#line 2222 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 230 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrDynListNode((PyrParseNode*)(yyvsp[-3]), (PyrParseNode*)(yyvsp[-1])); }
#line 2228 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 59:
#line 232 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)(yyvsp[-1]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, (PyrParseNode*)(yyvsp[0]));
			}
#line 2243 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 243 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)(yyvsp[-3]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, NULL, (PyrParseNode*)(yyvsp[0]));
			}
#line 2258 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 254 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)(yyvsp[-5]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[-3]), (PyrParseNode*)(yyvsp[0]));
			}
#line 2273 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 265 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)(yyvsp[-5])),
					(PyrParseNode*)(yyvsp[-3]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0]));
			}
#line 2290 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 278 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode, *selectornode2;
				PyrSlot slot, slot2;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)(yyvsp[-4]))) {
					SetRaw(&((PyrPushNameNode*)(yyvsp[-4]))->mSlot, s_this);
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				SetSymbol(&slot2, s_new);
				selectornode = newPyrSlotNode(&slot);
				selectornode2 = newPyrSlotNode(&slot2);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)(yyvsp[-4])),
					newPyrPushLitNode(selectornode2, NULL));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[-2]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[0]), 0);
			}
#line 2315 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 299 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)(yyvsp[-4]), NULL, (PyrParseNode*)(yyvsp[0]));
			}
#line 2328 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 308 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)(yyvsp[-6]), (PyrParseNode*)(yyvsp[-3]), (PyrParseNode*)(yyvsp[0]));
			}
#line 2341 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 317 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[-5]), (PyrParseNode*)(yyvsp[-7]),
					(PyrParseNode*)(yyvsp[-3]), (PyrParseNode*)(yyvsp[0]));
			}
#line 2350 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 322 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[-6]),
					(PyrParseNode*)(yyvsp[-3]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0]));
			}
#line 2367 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 335 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot, slot2;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)(yyvsp[-5]))) {
					SetRaw(&((PyrPushNameNode*)(yyvsp[-5]))->mSlot, s_this);
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				SetSymbol(&slot2, s_value);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[-5]),
					newPyrPushLitNode(newPyrSlotNode(&slot2), NULL));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[-2]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[-1]), 0);
			}
#line 2391 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 355 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[-3]), (PyrParseNode*)(yyvsp[-5]), NULL, (PyrParseNode*)(yyvsp[0]));
			}
#line 2399 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 359 "lang11d" /* yacc.c:1646  */
    {
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)(yyvsp[-7]), (PyrParseNode*)(yyvsp[-3]));
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[-5]), args, (PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0]));
			}
#line 2409 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 365 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)(yyvsp[-6]))) {
					SetRaw(&((PyrPushNameNode*)(yyvsp[-6]))->mSlot, s_this);
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				selectornode = newPyrSlotNode(&slot);

				args = linkNextNode((PyrParseNode*)(yyvsp[-6]), newPyrPushLitNode((PyrSlotNode*)(yyvsp[-4]), NULL));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[-2]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)(yyvsp[-1]), 0);
			}
#line 2431 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 383 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrCallNode((PyrSlotNode*)(yyvsp[-1]), (PyrParseNode*)(yyvsp[-3]), 0, (PyrParseNode*)(yyvsp[0]));
			}
#line 2439 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 388 "lang11d" /* yacc.c:1646  */
    { pushls(&generatorStack, (yyvsp[0])); pushls(&generatorStack, 1); }
#line 2445 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 389 "lang11d" /* yacc.c:1646  */
    {
				PyrSlot slot;
				SetSymbol(&slot, getsym("r"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);

				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)(yyvsp[-1]), false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)blocklit, 0, 0);
			}
#line 2459 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 398 "lang11d" /* yacc.c:1646  */
    { pushls(&generatorStack, (yyvsp[0])); pushls(&generatorStack, 2); }
#line 2465 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 399 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (yyvsp[-1]);
			}
#line 2473 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 405 "lang11d" /* yacc.c:1646  */
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
				}
#line 2499 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 427 "lang11d" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 2505 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 431 "lang11d" /* yacc.c:1646  */
    {
				// later should check if exprseq is a series and optimize it to for loop
				PyrParseNode *exprseq = (PyrParseNode*)(yyvsp[-1]);
				if (exprseq->mClassno == pn_CallNode) {
					PyrCallNode *callnode = (PyrCallNode*)exprseq;
					if (slotRawSymbol(&callnode->mSelector->mSlot) == s_series)
					{
						SetSymbol(&callnode->mSelector->mSlot, getsym("forSeries"));

						PyrVarDefNode* var = newPyrVarDefNode((PyrSlotNode*)(yyvsp[-3]), NULL, 0);
						PyrArgListNode* args = newPyrArgListNode(var, NULL);
						PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)(yyvsp[0]), false);
						PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);

						callnode->mArglist = linkNextNode(callnode->mArglist, blocklit);
						(yyval) = (intptr_t)callnode;

					} else goto notoptimized1;
				} else {
					notoptimized1:
					PyrSlot slot;
					SetSymbol(&slot, getsym("do"));
					PyrSlotNode* selectornode = newPyrSlotNode(&slot);

					PyrVarDefNode* var = newPyrVarDefNode((PyrSlotNode*)(yyvsp[-3]), NULL, 0);
					PyrArgListNode* args = newPyrArgListNode(var, NULL);
					PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)(yyvsp[0]), false);
					PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);

					PyrParseNode* args2 = linkNextNode(exprseq, blocklit);
					(yyval) = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
				}
			}
#line 2543 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 465 "lang11d" /* yacc.c:1646  */
    {
				// later should check if exprseq is a series and optimize it to for loop
				PyrParseNode *exprseq = (PyrParseNode*)(yyvsp[-1]);
				if (exprseq->mClassno == pn_CallNode) {
					PyrCallNode *callnode = (PyrCallNode*)exprseq;
					if (slotRawSymbol(&callnode->mSelector->mSlot) == s_series)
					{
						SetSymbol(&callnode->mSelector->mSlot, getsym("forSeries"));

						PyrVarDefNode* var1 = newPyrVarDefNode((PyrSlotNode*)(yyvsp[-4]), NULL, 0);
						PyrVarDefNode* var2 = newPyrVarDefNode((PyrSlotNode*)(yyvsp[-3]), NULL, 0);
						PyrVarDefNode* vars = (PyrVarDefNode*)linkNextNode(var1, var2);
						PyrArgListNode* args = newPyrArgListNode(vars, NULL);
						PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)(yyvsp[0]), false);
						PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);

						callnode->mArglist = linkNextNode(callnode->mArglist, blocklit);
						(yyval) = (intptr_t)callnode;

					} else goto notoptimized2;
				} else {
					notoptimized2:
					PyrSlot slot;
					SetSymbol(&slot, getsym("do"));
					PyrSlotNode* selectornode = newPyrSlotNode(&slot);

					PyrVarDefNode* var1 = newPyrVarDefNode((PyrSlotNode*)(yyvsp[-4]), NULL, 0);
					PyrVarDefNode* var2 = newPyrVarDefNode((PyrSlotNode*)(yyvsp[-3]), NULL, 0);
					PyrVarDefNode* vars = (PyrVarDefNode*)linkNextNode(var1, var2);
					PyrArgListNode* args = newPyrArgListNode(vars, NULL);
					PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)(yyvsp[0]), false);
					PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);

					PyrParseNode* args2 = linkNextNode(exprseq, blocklit);
					(yyval) = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
				}
			}
#line 2585 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 503 "lang11d" /* yacc.c:1646  */
    {
				PyrSlot slot;
				SetSymbol(&slot, s_value);
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);

				PyrVarDefNode* var = newPyrVarDefNode((PyrSlotNode*)(yyvsp[-3]), NULL, 0);
				PyrArgListNode* args = newPyrArgListNode(var, NULL);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)(yyvsp[0]), false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode(blocklit, (PyrParseNode*)(yyvsp[-1]));

				(yyval) = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
			}
#line 2603 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 517 "lang11d" /* yacc.c:1646  */
    {
				PyrSlot slot;
				SetSymbol(&slot, getsym("if"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)(yyvsp[0]), false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)(yyvsp[-1]), blocklit);

				(yyval) = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
			}
#line 2618 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 528 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrDropNode((PyrParseNode*)(yyvsp[-1]), (PyrParseNode*)(yyvsp[0]));
			}
#line 2626 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 532 "lang11d" /* yacc.c:1646  */
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
				PyrParseNode *block2 = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)(yyvsp[0]), false);
				PyrParseNode *blocklit2 = (PyrParseNode*)newPyrPushLitNode(NULL, block2);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)(yyvsp[-1]), blocklit2);
				PyrParseNode* args3 = (PyrParseNode*)linkNextNode(args2, blocklit1);

				(yyval) = (intptr_t)newPyrCallNode(selectornode2, args3, 0, 0);
			}
#line 2653 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 563 "lang11d" /* yacc.c:1646  */
    {
				PyrParseNode* node = (PyrParseNode*)(yyvsp[-1]);
				node->mParens = 1;
				(yyval) = (yyvsp[-1]);
			}
#line 2663 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 569 "lang11d" /* yacc.c:1646  */
    {
				PyrParseNode* argnode;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)(yyvsp[0]), NULL);
				SetSymbol(&slot, s_envirGet);
				selectornode = newPyrSlotNode(&slot);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, argnode, 0, 0);
			}
#line 2677 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 579 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrDynListNode(0, (PyrParseNode*)(yyvsp[-1])); }
#line 2683 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 581 "lang11d" /* yacc.c:1646  */
    { (yyval) = (yyvsp[-1]); }
#line 2689 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 583 "lang11d" /* yacc.c:1646  */
    { (yyval) = (yyvsp[-1]); }
#line 2695 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 585 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrDynDictNode((PyrParseNode*)(yyvsp[-1])); }
#line 2701 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 587 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushNameNode((PyrSlotNode*)(yyvsp[0])); }
#line 2707 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 589 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[-3]),
					(PyrParseNode*)(yyvsp[-1]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 2724 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 605 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)(yyvsp[-2]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[-2]));
					compileErrors++;
				}

				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[-4]), (PyrParseNode*)(yyvsp[-2]));
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 2755 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 101:
#line 632 "lang11d" /* yacc.c:1646  */
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
				args = linkNextNode((PyrParseNode*)(yyvsp[-4]), nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[-1]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 2777 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 102:
#line 650 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)(yyvsp[-3]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[-3]));
					compileErrors++;
				}

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[-5]), (PyrParseNode*)(yyvsp[-3]));
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[-1]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 2806 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 103:
#line 677 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)(yyvsp[-4]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[-4]));
					compileErrors++;
				}

				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[-6]), (PyrParseNode*)(yyvsp[-4]));
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[0]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 2838 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 104:
#line 705 "lang11d" /* yacc.c:1646  */
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
				args = linkNextNode((PyrParseNode*)(yyvsp[-6]), nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[-3]));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[0]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 2861 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 105:
#line 724 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)(yyvsp[-5]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[-5]));
					compileErrors++;
				}

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[-7]), (PyrParseNode*)(yyvsp[-5]));
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[-3]));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[0]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 2891 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 106:
#line 752 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)(yyvsp[-2]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[-3]));
					compileErrors++;
				}

				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[-5]), (PyrParseNode*)(yyvsp[-2]));
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 2922 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 107:
#line 779 "lang11d" /* yacc.c:1646  */
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
				args = linkNextNode((PyrParseNode*)(yyvsp[-5]), nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[-1]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 2944 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 108:
#line 797 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)(yyvsp[-3]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[-4]));
					compileErrors++;
				}

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[-6]), (PyrParseNode*)(yyvsp[-3]));
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[-1]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 2973 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 109:
#line 822 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)(yyvsp[-4]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[-5]));
					compileErrors++;
				}

				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[-7]), (PyrParseNode*)(yyvsp[-4]));
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[0]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3005 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 110:
#line 850 "lang11d" /* yacc.c:1646  */
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
				args = linkNextNode((PyrParseNode*)(yyvsp[-7]), nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[-3]));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[0]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3028 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 111:
#line 869 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)(yyvsp[-5]));
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[-6]));
					compileErrors++;
				}

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[-8]), (PyrParseNode*)(yyvsp[-5]));
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[-3]));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[0]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3058 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 112:
#line 897 "lang11d" /* yacc.c:1646  */
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
				args = linkNextNode((PyrParseNode*)(yyvsp[-1]), nilnode);
				args = linkNextNode(args, nilnode2);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3080 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 113:
#line 916 "lang11d" /* yacc.c:1646  */
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
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[0]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3102 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 114:
#line 935 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[-2]), nilnode);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[0]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3122 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 115:
#line 952 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot selectorSlot;
				PyrParseNode* args;

				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[-4]),
					(PyrParseNode*)(yyvsp[-2]));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[0]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3140 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 116:
#line 966 "lang11d" /* yacc.c:1646  */
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
					(PyrParseNode*)(yyvsp[-3]),
					(PyrParseNode*)(yyvsp[-1]));
				args = linkNextNode(args, nilnode);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3163 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 117:
#line 987 "lang11d" /* yacc.c:1646  */
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
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[0]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3185 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 118:
#line 1006 "lang11d" /* yacc.c:1646  */
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
				args = linkNextNode((PyrParseNode*)(yyvsp[-1]), nilnode);
				args = linkNextNode(args, nilnode2);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3206 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 119:
#line 1024 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[-2]), nilnode);
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[0]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3226 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 120:
#line 1041 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)(yyvsp[-3]), (PyrParseNode*)(yyvsp[-1]));
				args = linkNextNode(args, nilnode);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3246 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 121:
#line 1057 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot selectorSlot;
				PyrParseNode* args;

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[-4]),
					(PyrParseNode*)(yyvsp[-2]));
				args = linkNextNode(args, (PyrParseNode*)(yyvsp[0]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3264 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 125:
#line 1075 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushNameNode((PyrSlotNode*)(yyvsp[0])); }
#line 3270 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 126:
#line 1077 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[-4]),
					(PyrParseNode*)(yyvsp[-1]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3287 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 127:
#line 1090 "lang11d" /* yacc.c:1646  */
    {
				PyrParseNode *node, *args;
				PyrSlotNode *slotnode;
				PyrSlot slot;

				SetSymbol(&slot, s_ref);
				slotnode = newPyrSlotNode(&slot);
				node = (PyrParseNode*)newPyrPushNameNode(slotnode);
				args = linkNextNode(node, (PyrParseNode*)(yyvsp[0]));
				SetSymbol(&slot, s_new);
				slotnode = newPyrSlotNode(&slot);
				(yyval) = (intptr_t)newPyrCallNode(slotnode, args, 0, 0);
			}
#line 3305 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 128:
#line 1104 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrBinopCallNode((PyrSlotNode*)(yyvsp[-2]),
						(PyrParseNode*)(yyvsp[-3]), (PyrParseNode*)(yyvsp[0]), (PyrParseNode*)(yyvsp[-1]));
			}
#line 3314 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 129:
#line 1109 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrAssignNode((PyrSlotNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0]), 0);
			}
#line 3322 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 130:
#line 1113 "lang11d" /* yacc.c:1646  */
    {
				PyrParseNode *argnode, *args;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)(yyvsp[-2]), NULL);
				args = linkNextNode(argnode, (PyrParseNode*)(yyvsp[0]));
				SetSymbol(&slot, s_envirPut);
				selectornode = newPyrSlotNode(&slot);
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3337 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 131:
#line 1124 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrSetterNode((PyrSlotNode*)(yyvsp[-2]),
						(PyrParseNode*)(yyvsp[-4]), (PyrParseNode*)(yyvsp[0]));
			}
#line 3346 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 132:
#line 1129 "lang11d" /* yacc.c:1646  */
    {
				if ((yyvsp[-3]) != 0) {
					error("Setter method called with keyword arguments.\n");
					nodePostErrorLine((PyrParseNode*)(yyvsp[-3]));
					compileErrors++;
				}
				(yyval) = (intptr_t)newPyrSetterNode((PyrSlotNode*)(yyvsp[-6]),
						(PyrParseNode*)(yyvsp[-4]), (PyrParseNode*)(yyvsp[0]));
			}
#line 3360 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 133:
#line 1139 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)(yyvsp[-2]),
					(PyrParseNode*)(yyvsp[0]), 0);
			}
#line 3369 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 134:
#line 1144 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_put);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[-5]),
					(PyrParseNode*)(yyvsp[-3]));
				args = linkNextNode( args, (PyrParseNode*)(yyvsp[0]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3387 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 135:
#line 1158 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_put);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)(yyvsp[-6]),
					(PyrParseNode*)(yyvsp[-3]));
				args = linkNextNode( args, (PyrParseNode*)(yyvsp[0]));
				(yyval) = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3405 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 136:
#line 1173 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 3411 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 137:
#line 1174 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3417 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 138:
#line 1175 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3423 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 139:
#line 1176 "lang11d" /* yacc.c:1646  */
    { (yyval) = (yyvsp[-1]); }
#line 3429 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 141:
#line 1181 "lang11d" /* yacc.c:1646  */
    {
				(yyval) = (intptr_t)newPyrDropNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0]));
			}
#line 3437 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 143:
#line 1189 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 3443 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 144:
#line 1191 "lang11d" /* yacc.c:1646  */
    { (yyval) = (yyvsp[-1]); }
#line 3449 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 146:
#line 1196 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0])); }
#line 3455 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 147:
#line 1198 "lang11d" /* yacc.c:1646  */
    {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[-1]), NULL);
					(yyval) = (intptr_t)linkNextNode(key, (PyrParseNode*)(yyvsp[0]));
				}
#line 3464 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 148:
#line 1203 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0])); }
#line 3470 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 149:
#line 1205 "lang11d" /* yacc.c:1646  */
    {
					PyrParseNode* elems;
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[-1]), NULL);
					elems = (PyrParseNode*)linkNextNode(key, (PyrParseNode*)(yyvsp[0]));
					(yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-3]), elems);
				}
#line 3481 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 150:
#line 1212 "lang11d" /* yacc.c:1646  */
    {
					PyrParseNode* elems;
					elems = (PyrParseNode*)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0]));
					(yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-4]), elems);
				}
#line 3491 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 152:
#line 1221 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0])); }
#line 3497 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 153:
#line 1225 "lang11d" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 3503 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 154:
#line 1227 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-3]), (PyrParseNode*)(yyvsp[0])); }
#line 3509 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 156:
#line 1232 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0])); }
#line 3515 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 157:
#line 1236 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushKeyArgNode((PyrSlotNode*)(yyvsp[-1]), (PyrParseNode*)(yyvsp[0])); }
#line 3521 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 158:
#line 1239 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 3527 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 159:
#line 1240 "lang11d" /* yacc.c:1646  */
    { (yyval) = (yyvsp[-1]); }
#line 3533 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 160:
#line 1244 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3539 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 161:
#line 1246 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)(yyvsp[-2]), (PyrSlotNode*)(yyvsp[0])); }
#line 3545 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 163:
#line 1251 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0])); }
#line 3551 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 164:
#line 1255 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3557 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 165:
#line 1256 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3563 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 166:
#line 1257 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3569 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 167:
#line 1258 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3575 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 168:
#line 1259 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3581 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 169:
#line 1260 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3587 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 170:
#line 1261 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3593 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 171:
#line 1262 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3599 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 172:
#line 1263 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)(yyvsp[0])); }
#line 3605 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 173:
#line 1266 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)(yyvsp[0])); }
#line 3611 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 174:
#line 1269 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushNameNode((PyrSlotNode*)(yyvsp[0])); }
#line 3617 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 175:
#line 1272 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3623 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 176:
#line 1273 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3629 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 177:
#line 1274 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3635 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 178:
#line 1275 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3641 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 179:
#line 1276 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3647 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 180:
#line 1277 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3653 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 181:
#line 1278 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3659 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 182:
#line 1279 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushLitNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3665 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 183:
#line 1280 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)(yyvsp[0])); }
#line 3671 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 184:
#line 1283 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3677 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 185:
#line 1284 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3683 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 186:
#line 1285 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3689 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 187:
#line 1286 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3695 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 188:
#line 1287 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3701 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 189:
#line 1288 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3707 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 190:
#line 1289 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3713 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 191:
#line 1290 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3719 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 192:
#line 1291 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode((PyrSlotNode*)(yyvsp[0]), NULL); }
#line 3725 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 193:
#line 1292 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)(yyvsp[0])); }
#line 3731 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 194:
#line 1293 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)(yyvsp[0])); }
#line 3737 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 195:
#line 1297 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrBlockNode((PyrArgListNode*)(yyvsp[-3]), (PyrVarListNode*)(yyvsp[-2]),
					(PyrParseNode*)(yyvsp[-1]), false); }
#line 3744 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 196:
#line 1300 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrBlockNode((PyrArgListNode*)(yyvsp[-3]), (PyrVarListNode*)(yyvsp[-2]),
					(PyrParseNode*)(yyvsp[-1]), true); }
#line 3751 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 197:
#line 1304 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 3757 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 198:
#line 1306 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-1]), (PyrParseNode*)(yyvsp[0])); }
#line 3763 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 200:
#line 1311 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-1]), (PyrParseNode*)(yyvsp[0])); }
#line 3769 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 201:
#line 1315 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrVarListNode((PyrVarDefNode*)(yyvsp[-1]), varLocal); }
#line 3775 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 202:
#line 1318 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 3781 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 203:
#line 1320 "lang11d" /* yacc.c:1646  */
    {
					(yyval) = (intptr_t)newPyrArgListNode((PyrVarDefNode*)(yyvsp[-1]), NULL);
				}
#line 3789 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 204:
#line 1324 "lang11d" /* yacc.c:1646  */
    {
					(yyval) = (intptr_t)newPyrArgListNode((PyrVarDefNode*)(yyvsp[-3]), (PyrSlotNode*)(yyvsp[-1]));
				}
#line 3797 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 205:
#line 1328 "lang11d" /* yacc.c:1646  */
    {
					(yyval) = (intptr_t)newPyrArgListNode((PyrVarDefNode*)(yyvsp[-1]), NULL);
				}
#line 3805 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 206:
#line 1332 "lang11d" /* yacc.c:1646  */
    {
					(yyval) = (intptr_t)newPyrArgListNode((PyrVarDefNode*)(yyvsp[-3]), (PyrSlotNode*)(yyvsp[-1]));
				}
#line 3813 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 208:
#line 1339 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0])); }
#line 3819 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 209:
#line 1343 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0]), (yyvsp[-3])); }
#line 3825 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 210:
#line 1346 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 3831 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 213:
#line 1352 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0])); }
#line 3837 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 214:
#line 1356 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[0]), NULL, 0); }
#line 3843 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 215:
#line 1358 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0]), 0); }
#line 3849 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 216:
#line 1360 "lang11d" /* yacc.c:1646  */
    {
					PyrParseNode* node = (PyrParseNode*)(yyvsp[-1]);
					node->mParens = 1;
					(yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[-4]), node, 0);
				}
#line 3859 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 217:
#line 1367 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 3865 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 220:
#line 1373 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0])); }
#line 3871 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 221:
#line 1377 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[0]), NULL, 0); }
#line 3877 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 222:
#line 1379 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0]), 0); }
#line 3883 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 223:
#line 1381 "lang11d" /* yacc.c:1646  */
    {
									PyrParseNode* node = (PyrParseNode*)(yyvsp[-1]);
									node->mParens = 1;
									(yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[-3]), node, 0);
								}
#line 3893 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 224:
#line 1389 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0])); }
#line 3899 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 225:
#line 1391 "lang11d" /* yacc.c:1646  */
    {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[-1]), NULL);
					(yyval) = (intptr_t)linkNextNode(key, (PyrParseNode*)(yyvsp[0]));
				}
#line 3908 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 227:
#line 1399 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0])); }
#line 3914 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 228:
#line 1402 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 3920 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 231:
#line 1408 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0])); }
#line 3926 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 232:
#line 1412 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[0]), NULL, (yyvsp[-1])); }
#line 3932 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 233:
#line 1414 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrVarDefNode((PyrSlotNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0]), (yyvsp[-3])); }
#line 3938 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 234:
#line 1418 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLitDictNode((PyrParseNode*)(yyvsp[-1])); }
#line 3944 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 235:
#line 1422 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0])); }
#line 3950 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 236:
#line 1424 "lang11d" /* yacc.c:1646  */
    {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)(yyvsp[-1]), NULL);
					(yyval) = (intptr_t)linkNextNode(key, (PyrParseNode*)(yyvsp[0]));
				}
#line 3959 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 238:
#line 1432 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0])); }
#line 3965 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 239:
#line 1435 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 3971 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 241:
#line 1442 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)(yyvsp[-1])); }
#line 3977 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 242:
#line 1444 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLitListNode((PyrParseNode*)(yyvsp[-3]), (PyrParseNode*)(yyvsp[-1])); }
#line 3983 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 243:
#line 1448 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)(yyvsp[-1])); }
#line 3989 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 244:
#line 1450 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)newPyrLitListNode((PyrParseNode*)(yyvsp[-3]), (PyrParseNode*)(yyvsp[-1])); }
#line 3995 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 245:
#line 1453 "lang11d" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 4001 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 248:
#line 1459 "lang11d" /* yacc.c:1646  */
    { (yyval) = (intptr_t)linkNextNode((PyrParseNode*)(yyvsp[-2]), (PyrParseNode*)(yyvsp[0])); }
#line 4007 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 249:
#line 1462 "lang11d" /* yacc.c:1646  */
    { (yyval) = rwPrivate; }
#line 4013 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 250:
#line 1464 "lang11d" /* yacc.c:1646  */
    { (yyval) = rwReadOnly; }
#line 4019 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 251:
#line 1466 "lang11d" /* yacc.c:1646  */
    { (yyval) = rwReadWrite; }
#line 4025 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 252:
#line 1468 "lang11d" /* yacc.c:1646  */
    { (yyval) = rwWriteOnly; }
#line 4031 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 253:
#line 1471 "lang11d" /* yacc.c:1646  */
    { (yyval) = rwPrivate; }
#line 4037 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 254:
#line 1473 "lang11d" /* yacc.c:1646  */
    { (yyval) = rwReadOnly; }
#line 4043 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 255:
#line 1476 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4049 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 256:
#line 1478 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawInt(&node->mSlot));
				(yyval) = zzval;
			}
#line 4060 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 257:
#line 1486 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4066 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 258:
#line 1488 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawFloat(&node->mSlot));
				(yyval) = zzval;
			}
#line 4077 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 259:
#line 1496 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4083 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 260:
#line 1498 "lang11d" /* yacc.c:1646  */
    {
					PyrSlotNode *node;
					double intval, fracval;
					node = (PyrSlotNode*)zzval;
					intval = floor(slotRawFloat(&node->mSlot) + 0.5);
					fracval = slotRawFloat(&node->mSlot) - intval;
					SetRaw(&node->mSlot, -intval + fracval);
					(yyval) = zzval;
				}
#line 4097 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 261:
#line 1508 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4103 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 264:
#line 1514 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)(yyvsp[-1]);
				SetRaw(&node->mSlot, slotRawFloat(&node->mSlot) * pi);
			}
#line 4113 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 265:
#line 1520 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *node;
				double ival;
				node = (PyrSlotNode*)(yyvsp[-1]);
				ival = slotRawInt(&node->mSlot);
				SetFloat(&node->mSlot, ival * pi);
			}
#line 4125 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 266:
#line 1528 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, pi);
				(yyval) = zzval;
			}
#line 4136 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 267:
#line 1535 "lang11d" /* yacc.c:1646  */
    {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, -pi);
				(yyval) = zzval;
			}
#line 4147 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 268:
#line 1543 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4153 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 269:
#line 1544 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4159 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 270:
#line 1547 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4165 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 271:
#line 1550 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4171 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 272:
#line 1553 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4177 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 273:
#line 1556 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4183 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 274:
#line 1559 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4189 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 275:
#line 1562 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4195 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 276:
#line 1565 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4201 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 277:
#line 1568 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4207 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 278:
#line 1571 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4213 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 279:
#line 1574 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4219 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 280:
#line 1575 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4225 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 281:
#line 1576 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4231 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 282:
#line 1577 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4237 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 283:
#line 1578 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4243 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 284:
#line 1579 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4249 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 285:
#line 1580 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4255 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 286:
#line 1581 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4261 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 287:
#line 1584 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4267 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;

  case 290:
#line 1591 "lang11d" /* yacc.c:1646  */
    { (yyval) = zzval; }
#line 4273 "lang11d_tab.cpp" /* yacc.c:1646  */
    break;


#line 4277 "lang11d_tab.cpp" /* yacc.c:1646  */
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

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

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


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
