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


/* Substitute the variable and function names.  */
#define yyparse         scdocparse
#define yylex           scdoclex
#define yyerror         scdocerror
#define yydebug         scdocdebug
#define yynerrs         scdocnerrs

#define yylval          scdoclval
#define yychar          scdocchar
#define yylloc          scdoclloc

/* Copy the first part of user declarations.  */
#line 1 "SCDoc.y" /* yacc.c:339  */

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


#line 128 "SCDoc.tab.cpp" /* yacc.c:339  */

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
   by #include "SCDoc.tab.hpp".  */
#ifndef YY_SCDOC_SCDOC_TAB_HPP_INCLUDED
# define YY_SCDOC_SCDOC_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int scdocdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 55 "SCDoc.y" /* yacc.c:355  */

    intptr_t i;
    const char *id;
    char *str;
    DocNode *doc_node;

#line 231 "SCDoc.tab.cpp" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE scdoclval;
extern YYLTYPE scdoclloc;
int scdocparse (void);

#endif /* !YY_SCDOC_SCDOC_TAB_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */
#line 100 "SCDoc.y" /* yacc.c:358  */

//int scdoclex (YYSTYPE * yylval_param, struct YYLTYPE * yylloc_param );
int scdoclex (void);

#line 266 "SCDoc.tab.cpp" /* yacc.c:358  */

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
#define YYFINAL  68
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   429

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  55
/* YYNRULES -- Number of rules.  */
#define YYNRULES  132
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  191

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   309

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
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
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   107,   107,   108,   111,   117,   121,   129,   130,   133,
     134,   137,   138,   139,   142,   143,   144,   145,   148,   149,
     150,   151,   154,   155,   158,   159,   160,   163,   163,   164,
     167,   168,   171,   172,   173,   176,   179,   180,   183,   184,
     185,   188,   195,   207,   212,   213,   224,   235,   236,   239,
     248,   249,   252,   253,   256,   257,   260,   261,   264,   265,
     268,   269,   277,   278,   281,   282,   283,   286,   287,   290,
     291,   292,   293,   294,   295,   296,   299,   300,   303,   304,
     307,   308,   309,   310,   311,   314,   315,   316,   317,   318,
     319,   320,   323,   324,   327,   328,   329,   332,   333,   336,
     337,   340,   343,   344,   347,   348,   351,   352,   355,   362,
     363,   366,   367,   370,   371,   374,   375,   378,   379,   382,
     383,   386,   387,   390,   391,   394,   395,   398,   399,   400,
     401,   404,   405
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
  "nocommawords", "commalist", YY_NULLPTR
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
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309
};
# endif

#define YYPACT_NINF -129

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-129)))

#define YYTABLE_NINF -3

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
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

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
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

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
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

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
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


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
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
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


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


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
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
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
          case 44: /* "text"  */
#line 98 "SCDoc.y" /* yacc.c:1257  */
      { free(((*yyvaluep).str)); }
#line 1368 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 45: /* URL  */
#line 98 "SCDoc.y" /* yacc.c:1257  */
      { free(((*yyvaluep).str)); }
#line 1374 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 46: /* COMMA  */
#line 98 "SCDoc.y" /* yacc.c:1257  */
      { free(((*yyvaluep).str)); }
#line 1380 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 47: /* "method name"  */
#line 98 "SCDoc.y" /* yacc.c:1257  */
      { free(((*yyvaluep).str)); }
#line 1386 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 48: /* "arguments string"  */
#line 98 "SCDoc.y" /* yacc.c:1257  */
      { free(((*yyvaluep).str)); }
#line 1392 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 57: /* document  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1398 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 59: /* dochead  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1404 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 60: /* headline  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1410 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 63: /* optsections  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1416 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 64: /* sections  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1422 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 65: /* section  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1428 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 67: /* optsubsections  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1434 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 68: /* subsections  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1440 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 69: /* subsection  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1446 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 70: /* optsubsubsections  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1452 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 71: /* subsubsections  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1458 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 72: /* subsubsection  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1464 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 73: /* optMETHODARGS  */
#line 98 "SCDoc.y" /* yacc.c:1257  */
      { free(((*yyvaluep).str)); }
#line 1470 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 74: /* methodname  */
#line 98 "SCDoc.y" /* yacc.c:1257  */
      { free(((*yyvaluep).str)); }
#line 1476 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 75: /* methnames  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1482 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 76: /* methodbody  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1488 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 77: /* optbody  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1494 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 78: /* optargs  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1500 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 79: /* args  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1506 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 80: /* arg  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1512 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 81: /* optreturns  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1518 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 82: /* optdiscussion  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1524 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 83: /* body  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1530 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 84: /* blockA  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1536 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 85: /* blockB  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1542 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 86: /* bodyelem  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1548 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 87: /* prose  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1554 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 88: /* proseelem  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1560 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 93: /* listbody  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1566 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 94: /* tablerow  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1572 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 95: /* tablebody  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1578 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 96: /* tablecells  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1584 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 97: /* defterms  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1590 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 98: /* deflistrow  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1596 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 99: /* deflistbody  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1602 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 100: /* anywordurl  */
#line 98 "SCDoc.y" /* yacc.c:1257  */
      { free(((*yyvaluep).str)); }
#line 1608 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 101: /* anyword  */
#line 98 "SCDoc.y" /* yacc.c:1257  */
      { free(((*yyvaluep).str)); }
#line 1614 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 102: /* words  */
#line 98 "SCDoc.y" /* yacc.c:1257  */
      { free(((*yyvaluep).str)); }
#line 1620 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 103: /* words2  */
#line 98 "SCDoc.y" /* yacc.c:1257  */
      { free(((*yyvaluep).str)); }
#line 1626 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 106: /* anywordnl  */
#line 98 "SCDoc.y" /* yacc.c:1257  */
      { free(((*yyvaluep).str)); }
#line 1632 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 107: /* wordsnl  */
#line 98 "SCDoc.y" /* yacc.c:1257  */
      { free(((*yyvaluep).str)); }
#line 1638 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 108: /* nocommawords  */
#line 98 "SCDoc.y" /* yacc.c:1257  */
      { free(((*yyvaluep).str)); }
#line 1644 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;

    case 109: /* commalist  */
#line 97 "SCDoc.y" /* yacc.c:1257  */
      { doc_node_free_tree(((*yyvaluep).doc_node)); }
#line 1650 "SCDoc.tab.cpp" /* yacc.c:1257  */
        break;


      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
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

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

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
     '$$ = $1'.

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
#line 107 "SCDoc.y" /* yacc.c:1661  */
    { topnode = (yyvsp[0].doc_node); }
#line 1938 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 3:
#line 108 "SCDoc.y" /* yacc.c:1661  */
    { topnode = NULL; doc_node_free_tree((yyvsp[-1].doc_node)); }
#line 1944 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 4:
#line 112 "SCDoc.y" /* yacc.c:1661  */
    {
        (yyval.doc_node) = doc_node_create("DOCUMENT");
        doc_node_add_child((yyval.doc_node), (yyvsp[-1].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[0].doc_node));
    }
#line 1954 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 5:
#line 118 "SCDoc.y" /* yacc.c:1661  */
    {
        (yyval.doc_node) = doc_node_make_take_children("BODY",NULL,(yyvsp[0].doc_node));
    }
#line 1962 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 6:
#line 122 "SCDoc.y" /* yacc.c:1661  */
    {
        (yyval.doc_node) = doc_node_create("DOCUMENT");
        doc_node_add_child((yyval.doc_node), (yyvsp[-1].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[0].doc_node));
    }
#line 1972 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 9:
#line 133 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_add_child((yyvsp[-1].doc_node),(yyvsp[0].doc_node)); }
#line 1978 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 10:
#line 134 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("HEADER",NULL,(yyvsp[0].doc_node)); }
#line 1984 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 11:
#line 137 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make((yyvsp[-2].id),(yyvsp[-1].str),NULL); }
#line 1990 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 12:
#line 138 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("CATEGORIES",NULL,(yyvsp[-1].doc_node)); }
#line 1996 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 13:
#line 139 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("RELATED",NULL,(yyvsp[-1].doc_node)); }
#line 2002 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 14:
#line 142 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "TITLE"; }
#line 2008 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 15:
#line 143 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "TITLE"; }
#line 2014 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 16:
#line 144 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "SUMMARY"; }
#line 2020 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 17:
#line 145 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "REDIRECT"; }
#line 2026 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 18:
#line 148 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "CLASSMETHODS"; method_type = "CMETHOD"; }
#line 2032 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 19:
#line 149 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "INSTANCEMETHODS"; method_type = "IMETHOD"; }
#line 2038 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 20:
#line 150 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "DESCRIPTION"; method_type = "METHOD"; }
#line 2044 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 21:
#line 151 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "EXAMPLES"; method_type = "METHOD"; }
#line 2050 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 23:
#line 155 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("BODY",NULL,NULL); }
#line 2056 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 24:
#line 158 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_add_child((yyvsp[-1].doc_node),(yyvsp[0].doc_node)); }
#line 2062 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 25:
#line 159 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("BODY",NULL,(yyvsp[0].doc_node)); }
#line 2068 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 26:
#line 160 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("BODY",NULL,(yyvsp[0].doc_node)); }
#line 2074 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 27:
#line 163 "SCDoc.y" /* yacc.c:1661  */
    { method_type = "METHOD"; }
#line 2080 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 28:
#line 163 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("SECTION",(yyvsp[-2].str),(yyvsp[0].doc_node)); }
#line 2086 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 29:
#line 164 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children((yyvsp[-1].id), NULL,(yyvsp[0].doc_node)); }
#line 2092 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 31:
#line 168 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = NULL; }
#line 2098 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 32:
#line 171 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_add_child((yyvsp[-1].doc_node),(yyvsp[0].doc_node)); }
#line 2104 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 33:
#line 172 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("(SUBSECTIONS)",NULL,(yyvsp[0].doc_node)); }
#line 2110 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 35:
#line 176 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("SUBSECTION", (yyvsp[-2].str), (yyvsp[0].doc_node)); }
#line 2116 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 37:
#line 180 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = NULL; }
#line 2122 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 38:
#line 183 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_add_child((yyvsp[-1].doc_node),(yyvsp[0].doc_node)); }
#line 2128 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 39:
#line 184 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("(SUBSUBSECTIONS)",NULL,(yyvsp[0].doc_node)); }
#line 2134 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 40:
#line 185 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("(SUBSUBSECTIONS)",NULL,(yyvsp[0].doc_node)); }
#line 2140 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 41:
#line 189 "SCDoc.y" /* yacc.c:1661  */
    {
        (yyvsp[-3].doc_node)->id = "METHODNAMES";
        (yyval.doc_node) = doc_node_make(method_type,(yyvsp[-2].str),(yyvsp[-3].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[0].doc_node));
//        doc_node_add_child($2, $3);
    }
#line 2151 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 42:
#line 195 "SCDoc.y" /* yacc.c:1661  */
    { 
                if ( !(strchr((yyvsp[-1].str), ' ')) ) {
                  yyerror("COPYMETHOD requires 2 arguments (class name and method name)");
                  YYERROR;
                }

                (yyval.doc_node) = doc_node_make(
                stringEqual(method_type, "CMETHOD") ? "CCOPYMETHOD"
                                                    : (stringEqual(method_type, "IMETHOD") ? "ICOPYMETHOD"
                                                                                           : "COPYMETHOD"),
                (yyvsp[-1].str), NULL
                ); }
#line 2168 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 43:
#line 207 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children( stringEqual(method_type, "CMETHOD") ? "CPRIVATE"
                                                                                                                : "IPRIVATE",
                NULL, (yyvsp[-1].doc_node)); }
#line 2176 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 44:
#line 212 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.str) = NULL; }
#line 2182 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 45:
#line 214 "SCDoc.y" /* yacc.c:1661  */
    {
//        $$ = doc_node_make("ARGSTRING",$1,NULL);
        (yyval.str) = (yyvsp[0].str);
        if(!stringEqual(method_type, "METHOD")) {
            yyerror("METHOD argument string is not allowed inside CLASSMETHODS or INSTANCEMETHODS");
            YYERROR;
        }
    }
#line 2195 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 46:
#line 225 "SCDoc.y" /* yacc.c:1661  */
    {
        char *p = (yyvsp[0].str)+strlen((yyvsp[0].str))-1;
        if(*p=='_') {
            post("WARNING: SCDoc: In %s\n  Property setter %s should be documented without underscore.\n", scdoc_current_file, (yyvsp[0].str));
            *p = '\0';
        };
        (yyval.str) = (yyvsp[0].str);
    }
#line 2208 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 47:
#line 235 "SCDoc.y" /* yacc.c:1661  */
    { free((yyvsp[-1].str)); (yyvsp[-1].str) = NULL; (yyval.doc_node) = doc_node_add_child((yyvsp[-2].doc_node), doc_node_make("STRING",(yyvsp[0].str),NULL)); }
#line 2214 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 48:
#line 236 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("(METHODNAMES)",NULL,doc_node_make("STRING",(yyvsp[0].str),NULL)); }
#line 2220 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 49:
#line 240 "SCDoc.y" /* yacc.c:1661  */
    {
        (yyval.doc_node) = doc_node_make_take_children("METHODBODY",NULL,(yyvsp[-3].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[-2].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[-1].doc_node));
        doc_node_add_child((yyval.doc_node), (yyvsp[0].doc_node));
    }
#line 2231 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 51:
#line 249 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = NULL; }
#line 2237 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 53:
#line 253 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = NULL; }
#line 2243 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 54:
#line 256 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_add_child((yyvsp[-1].doc_node),(yyvsp[0].doc_node)); }
#line 2249 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 55:
#line 257 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("ARGUMENTS",NULL,(yyvsp[0].doc_node)); }
#line 2255 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 56:
#line 260 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("ARGUMENT", (yyvsp[-2].str), (yyvsp[0].doc_node)); }
#line 2261 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 57:
#line 261 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("ARGUMENT", NULL, (yyvsp[0].doc_node)); }
#line 2267 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 58:
#line 264 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("RETURNS",NULL,(yyvsp[0].doc_node)); }
#line 2273 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 59:
#line 265 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = NULL; }
#line 2279 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 60:
#line 268 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("DISCUSSION",NULL,(yyvsp[0].doc_node)); }
#line 2285 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 61:
#line 269 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = NULL; }
#line 2291 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 64:
#line 281 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_add_child((yyvsp[-1].doc_node),(yyvsp[0].doc_node)); }
#line 2297 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 65:
#line 282 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_add_child((yyvsp[-1].doc_node),(yyvsp[0].doc_node)); }
#line 2303 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 66:
#line 283 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("(SECTIONBODY)",NULL,(yyvsp[0].doc_node)); }
#line 2309 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 67:
#line 286 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_add_child((yyvsp[-1].doc_node),(yyvsp[0].doc_node)); }
#line 2315 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 68:
#line 287 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("(SECTIONBODY)",NULL,(yyvsp[0].doc_node)); }
#line 2321 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 69:
#line 290 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children((yyvsp[-2].id),NULL,(yyvsp[-1].doc_node)); }
#line 2327 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 70:
#line 291 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children((yyvsp[-2].id),NULL,(yyvsp[-1].doc_node)); }
#line 2333 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 71:
#line 292 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("TABLE",NULL,(yyvsp[-1].doc_node)); }
#line 2339 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 72:
#line 293 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("DEFINITIONLIST",NULL,(yyvsp[-1].doc_node)); }
#line 2345 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 73:
#line 294 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make((yyvsp[-2].id),(yyvsp[-1].str),NULL); }
#line 2351 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 74:
#line 295 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("CLASSTREE",(yyvsp[-1].str),NULL); }
#line 2357 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 75:
#line 296 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("KEYWORD",NULL,(yyvsp[-1].doc_node));
//            printf("keyword '%s'\n",$2->children[0]->text);
        }
#line 2365 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 76:
#line 299 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = NULL; }
#line 2371 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 77:
#line 300 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("IMAGE",(yyvsp[-1].str),NULL); }
#line 2377 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 78:
#line 303 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_add_child((yyvsp[-1].doc_node), (yyvsp[0].doc_node)); }
#line 2383 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 79:
#line 304 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("PROSE",NULL,(yyvsp[0].doc_node)); }
#line 2389 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 80:
#line 307 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make(NODE_TEXT,(yyvsp[0].str),NULL); }
#line 2395 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 81:
#line 308 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("LINK",(yyvsp[0].str),NULL); }
#line 2401 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 82:
#line 309 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make((yyvsp[-2].id),(yyvsp[-1].str),NULL); }
#line 2407 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 83:
#line 310 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("FOOTNOTE",NULL,(yyvsp[-1].doc_node)); }
#line 2413 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 84:
#line 311 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_create(NODE_NL); }
#line 2419 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 85:
#line 314 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "LINK"; }
#line 2425 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 86:
#line 315 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "STRONG"; }
#line 2431 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 87:
#line 316 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "SOFT"; }
#line 2437 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 88:
#line 317 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "EMPHASIS"; }
#line 2443 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 89:
#line 318 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "CODE"; }
#line 2449 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 90:
#line 319 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "TELETYPE"; }
#line 2455 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 91:
#line 320 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "ANCHOR"; }
#line 2461 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 92:
#line 323 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "CODEBLOCK"; }
#line 2467 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 93:
#line 324 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "TELETYPEBLOCK"; }
#line 2473 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 94:
#line 327 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "LIST"; }
#line 2479 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 95:
#line 328 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "TREE"; }
#line 2485 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 96:
#line 329 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "NUMBEREDLIST"; }
#line 2491 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 97:
#line 332 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "WARNING"; }
#line 2497 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 98:
#line 333 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.id) = "NOTE"; }
#line 2503 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 99:
#line 336 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_add_child((yyvsp[-2].doc_node), doc_node_make_take_children("ITEM",NULL,(yyvsp[0].doc_node))); }
#line 2509 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 100:
#line 337 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("(LISTBODY)",NULL, doc_node_make_take_children("ITEM",NULL,(yyvsp[0].doc_node))); }
#line 2515 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 101:
#line 340 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make_take_children("TABROW",NULL,(yyvsp[0].doc_node)); }
#line 2521 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 102:
#line 343 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_add_child((yyvsp[-1].doc_node),(yyvsp[0].doc_node)); }
#line 2527 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 103:
#line 344 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("(TABLEBODY)",NULL,(yyvsp[0].doc_node)); }
#line 2533 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 104:
#line 347 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_add_child((yyvsp[-2].doc_node), doc_node_make_take_children("TABCOL",NULL,(yyvsp[0].doc_node))); }
#line 2539 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 105:
#line 348 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("(TABLECELLS)",NULL, doc_node_make_take_children("TABCOL",NULL,(yyvsp[0].doc_node))); }
#line 2545 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 106:
#line 351 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_add_child((yyvsp[-2].doc_node),doc_node_make_take_children("TERM",NULL,(yyvsp[0].doc_node))); }
#line 2551 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 107:
#line 352 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("(TERMS)",NULL,doc_node_make_take_children("TERM",NULL,(yyvsp[0].doc_node))); }
#line 2557 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 108:
#line 356 "SCDoc.y" /* yacc.c:1661  */
    {
        (yyval.doc_node) = doc_node_make_take_children("DEFLISTITEM", NULL, (yyvsp[-2].doc_node));
        doc_node_add_child((yyval.doc_node), doc_node_make_take_children("DEFINITION", NULL, (yyvsp[0].doc_node)));
    }
#line 2566 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 109:
#line 362 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_add_child((yyvsp[-1].doc_node),(yyvsp[0].doc_node)); }
#line 2572 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 110:
#line 363 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("(DEFLISTBODY)",NULL,(yyvsp[0].doc_node)); }
#line 2578 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 115:
#line 374 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.str) = strmerge((yyvsp[-1].str),(yyvsp[0].str)); }
#line 2584 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 117:
#line 378 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.str) = strmerge((yyvsp[-1].str),(yyvsp[0].str)); }
#line 2590 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 124:
#line 391 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.str) = strdup("\n"); }
#line 2596 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 125:
#line 394 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.str) = strmerge((yyvsp[-1].str),(yyvsp[0].str)); }
#line 2602 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 127:
#line 398 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.str) = strmerge((yyvsp[-1].str),(yyvsp[0].str)); }
#line 2608 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 128:
#line 399 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.str) = strmerge((yyvsp[-1].str),(yyvsp[0].str)); }
#line 2614 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 131:
#line 404 "SCDoc.y" /* yacc.c:1661  */
    { free((yyvsp[-1].str)); (yyvsp[-1].str)=NULL; (yyval.doc_node) = doc_node_add_child((yyvsp[-2].doc_node),doc_node_make("STRING",(yyvsp[0].str),NULL)); }
#line 2620 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;

  case 132:
#line 405 "SCDoc.y" /* yacc.c:1661  */
    { (yyval.doc_node) = doc_node_make("(COMMALIST)",NULL,doc_node_make("STRING",(yyvsp[0].str),NULL)); }
#line 2626 "SCDoc.tab.cpp" /* yacc.c:1661  */
    break;


#line 2630 "SCDoc.tab.cpp" /* yacc.c:1661  */
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
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 408 "SCDoc.y" /* yacc.c:1906  */


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

