/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 85 "lang11d"


// Preamble for the generated file.

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


#line 93 "lang11d_tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "lang11d_tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NAME = 3,                       /* NAME  */
  YYSYMBOL_INTEGER = 4,                    /* INTEGER  */
  YYSYMBOL_SC_FLOAT = 5,                   /* SC_FLOAT  */
  YYSYMBOL_ACCIDENTAL = 6,                 /* ACCIDENTAL  */
  YYSYMBOL_SYMBOL = 7,                     /* SYMBOL  */
  YYSYMBOL_STRING = 8,                     /* STRING  */
  YYSYMBOL_ASCII = 9,                      /* ASCII  */
  YYSYMBOL_CLASSNAME = 10,                 /* CLASSNAME  */
  YYSYMBOL_PIE = 11,                       /* PIE  */
  YYSYMBOL_WHILE = 12,                     /* WHILE  */
  YYSYMBOL_PRIMITIVENAME = 13,             /* PRIMITIVENAME  */
  YYSYMBOL_LEFTARROW = 14,                 /* LEFTARROW  */
  YYSYMBOL_TRUEOBJ = 15,                   /* TRUEOBJ  */
  YYSYMBOL_FALSEOBJ = 16,                  /* FALSEOBJ  */
  YYSYMBOL_NILOBJ = 17,                    /* NILOBJ  */
  YYSYMBOL_PSEUDOVAR = 18,                 /* PSEUDOVAR  */
  YYSYMBOL_VAR = 19,                       /* VAR  */
  YYSYMBOL_ARG = 20,                       /* ARG  */
  YYSYMBOL_CLASSVAR = 21,                  /* CLASSVAR  */
  YYSYMBOL_SC_CONST = 22,                  /* SC_CONST  */
  YYSYMBOL_ELLIPSIS = 23,                  /* ELLIPSIS  */
  YYSYMBOL_DOTDOT = 24,                    /* DOTDOT  */
  YYSYMBOL_BEGINCLOSEDFUNC = 25,           /* BEGINCLOSEDFUNC  */
  YYSYMBOL_BADTOKEN = 26,                  /* BADTOKEN  */
  YYSYMBOL_INTERPRET = 27,                 /* INTERPRET  */
  YYSYMBOL_BEGINGENERATOR = 28,            /* BEGINGENERATOR  */
  YYSYMBOL_CURRYARG = 29,                  /* CURRYARG  */
  YYSYMBOL_30_ = 30,                       /* ':'  */
  YYSYMBOL_31_ = 31,                       /* '='  */
  YYSYMBOL_BINOP = 32,                     /* BINOP  */
  YYSYMBOL_KEYBINOP = 33,                  /* KEYBINOP  */
  YYSYMBOL_34_ = 34,                       /* '-'  */
  YYSYMBOL_35_ = 35,                       /* '<'  */
  YYSYMBOL_36_ = 36,                       /* '>'  */
  YYSYMBOL_37_ = 37,                       /* '*'  */
  YYSYMBOL_38_ = 38,                       /* '+'  */
  YYSYMBOL_39_ = 39,                       /* '|'  */
  YYSYMBOL_READWRITEVAR = 40,              /* READWRITEVAR  */
  YYSYMBOL_41_ = 41,                       /* '.'  */
  YYSYMBOL_42_ = 42,                       /* '`'  */
  YYSYMBOL_UMINUS = 43,                    /* UMINUS  */
  YYSYMBOL_44_ = 44,                       /* '{'  */
  YYSYMBOL_45_ = 45,                       /* '}'  */
  YYSYMBOL_46_ = 46,                       /* '['  */
  YYSYMBOL_47_ = 47,                       /* ']'  */
  YYSYMBOL_48_ = 48,                       /* ';'  */
  YYSYMBOL_49_ = 49,                       /* ','  */
  YYSYMBOL_50_ = 50,                       /* '('  */
  YYSYMBOL_51_ = 51,                       /* ')'  */
  YYSYMBOL_52_ = 52,                       /* '^'  */
  YYSYMBOL_53_ = 53,                       /* '~'  */
  YYSYMBOL_54_ = 54,                       /* '#'  */
  YYSYMBOL_YYACCEPT = 55,                  /* $accept  */
  YYSYMBOL_root = 56,                      /* root  */
  YYSYMBOL_classes = 57,                   /* classes  */
  YYSYMBOL_classextensions = 58,           /* classextensions  */
  YYSYMBOL_classdef = 59,                  /* classdef  */
  YYSYMBOL_classextension = 60,            /* classextension  */
  YYSYMBOL_optName = 61,                   /* optName  */
  YYSYMBOL_optSuperName = 62,              /* optSuperName  */
  YYSYMBOL_classvardecls = 63,             /* classvardecls  */
  YYSYMBOL_classvardecl = 64,              /* classvardecl  */
  YYSYMBOL_methods = 65,                   /* methods  */
  YYSYMBOL_methoddef = 66,                 /* methoddef  */
  YYSYMBOL_optsemi = 67,                   /* optsemi  */
  YYSYMBOL_optcomma = 68,                  /* optcomma  */
  YYSYMBOL_optequal = 69,                  /* optequal  */
  YYSYMBOL_funcbody = 70,                  /* funcbody  */
  YYSYMBOL_cmdlinecode = 71,               /* cmdlinecode  */
  YYSYMBOL_methbody = 72,                  /* methbody  */
  YYSYMBOL_optPrim = 73,                   /* optPrim  */
  YYSYMBOL_retval = 74,                    /* retval  */
  YYSYMBOL_funretval = 75,                 /* funretval  */
  YYSYMBOL_blocklist1 = 76,                /* blocklist1  */
  YYSYMBOL_blocklistitem = 77,             /* blocklistitem  */
  YYSYMBOL_blocklist = 78,                 /* blocklist  */
  YYSYMBOL_msgsend = 79,                   /* msgsend  */
  YYSYMBOL_generator = 80,                 /* generator  */
  YYSYMBOL_81_1 = 81,                      /* $@1  */
  YYSYMBOL_82_2 = 82,                      /* $@2  */
  YYSYMBOL_nextqual = 83,                  /* nextqual  */
  YYSYMBOL_qual = 84,                      /* qual  */
  YYSYMBOL_expr1 = 85,                     /* expr1  */
  YYSYMBOL_valrangex1 = 86,                /* valrangex1  */
  YYSYMBOL_valrangeassign = 87,            /* valrangeassign  */
  YYSYMBOL_valrangexd = 88,                /* valrangexd  */
  YYSYMBOL_valrange2 = 89,                 /* valrange2  */
  YYSYMBOL_valrange3 = 90,                 /* valrange3  */
  YYSYMBOL_expr = 91,                      /* expr  */
  YYSYMBOL_adverb = 92,                    /* adverb  */
  YYSYMBOL_exprn = 93,                     /* exprn  */
  YYSYMBOL_exprseq = 94,                   /* exprseq  */
  YYSYMBOL_arrayelems = 95,                /* arrayelems  */
  YYSYMBOL_arrayelems1 = 96,               /* arrayelems1  */
  YYSYMBOL_arglist1 = 97,                  /* arglist1  */
  YYSYMBOL_arglistv1 = 98,                 /* arglistv1  */
  YYSYMBOL_keyarglist1 = 99,               /* keyarglist1  */
  YYSYMBOL_keyarg = 100,                   /* keyarg  */
  YYSYMBOL_optkeyarglist = 101,            /* optkeyarglist  */
  YYSYMBOL_mavars = 102,                   /* mavars  */
  YYSYMBOL_nameList = 103,                 /* nameList  */
  YYSYMBOL_slotliteral = 104,              /* slotliteral  */
  YYSYMBOL_blockliteral = 105,             /* blockliteral  */
  YYSYMBOL_pushname = 106,                 /* pushname  */
  YYSYMBOL_pushliteral = 107,              /* pushliteral  */
  YYSYMBOL_listliteral = 108,              /* listliteral  */
  YYSYMBOL_block = 109,                    /* block  */
  YYSYMBOL_funcvardecls = 110,             /* funcvardecls  */
  YYSYMBOL_funcvardecls1 = 111,            /* funcvardecls1  */
  YYSYMBOL_funcvardecl = 112,              /* funcvardecl  */
  YYSYMBOL_newBlockLiteral = 113,          /* newBlockLiteral  */
  YYSYMBOL_newBlock = 114,                 /* newBlock  */
  YYSYMBOL_newBodyOpt = 115,               /* newBodyOpt  */
  YYSYMBOL_newBody = 116,                  /* newBody  */
  YYSYMBOL_newBodyItem = 117,              /* newBodyItem  */
  YYSYMBOL_argdecls = 118,                 /* argdecls  */
  YYSYMBOL_argdecls1 = 119,                /* argdecls1  */
  YYSYMBOL_constdeflist = 120,             /* constdeflist  */
  YYSYMBOL_constdef = 121,                 /* constdef  */
  YYSYMBOL_slotdeflist0 = 122,             /* slotdeflist0  */
  YYSYMBOL_slotdeflist = 123,              /* slotdeflist  */
  YYSYMBOL_slotdef = 124,                  /* slotdef  */
  YYSYMBOL_vardeflist0 = 125,              /* vardeflist0  */
  YYSYMBOL_vardeflist = 126,               /* vardeflist  */
  YYSYMBOL_vardef = 127,                   /* vardef  */
  YYSYMBOL_dictslotdef = 128,              /* dictslotdef  */
  YYSYMBOL_dictslotlist1 = 129,            /* dictslotlist1  */
  YYSYMBOL_dictslotlist = 130,             /* dictslotlist  */
  YYSYMBOL_rwslotdeflist = 131,            /* rwslotdeflist  */
  YYSYMBOL_rwslotdef = 132,                /* rwslotdef  */
  YYSYMBOL_dictlit2 = 133,                 /* dictlit2  */
  YYSYMBOL_litdictslotdef = 134,           /* litdictslotdef  */
  YYSYMBOL_litdictslotlist1 = 135,         /* litdictslotlist1  */
  YYSYMBOL_litdictslotlist = 136,          /* litdictslotlist  */
  YYSYMBOL_listlit = 137,                  /* listlit  */
  YYSYMBOL_listlit2 = 138,                 /* listlit2  */
  YYSYMBOL_literallistc = 139,             /* literallistc  */
  YYSYMBOL_literallist1 = 140,             /* literallist1  */
  YYSYMBOL_rwspec = 141,                   /* rwspec  */
  YYSYMBOL_rspec = 142,                    /* rspec  */
  YYSYMBOL_integer = 143,                  /* integer  */
  YYSYMBOL_floatr = 144,                   /* floatr  */
  YYSYMBOL_accidental = 145,               /* accidental  */
  YYSYMBOL_floatp = 146,                   /* floatp  */
  YYSYMBOL_name = 147,                     /* name  */
  YYSYMBOL_binop = 148,                    /* binop  */
  YYSYMBOL_binop2 = 149,                   /* binop2  */
  YYSYMBOL_curryArg = 150                  /* curryArg  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

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
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  62
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1923

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  96
/* YYNRULES -- Number of rules.  */
#define YYNRULES  301
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  574

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   288


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
static const yytype_int16 yyrline[] =
{
       0,   181,   181,   186,   191,   197,   198,   201,   202,   205,
     207,   210,   213,   213,   216,   216,   218,   219,   222,   224,
     226,   229,   230,   233,   235,   237,   239,   242,   242,   243,
     243,   244,   244,   246,   247,   250,   252,   254,   256,   258,
     260,   262,   265,   266,   269,   270,   272,   273,   276,   278,
     281,   282,   285,   285,   287,   287,   289,   292,   295,   298,
     301,   304,   307,   320,   326,   329,   335,   341,   347,   356,
     374,   379,   384,   386,   391,   407,   409,   411,   424,   433,
     433,   440,   440,   443,   457,   459,   492,   529,   540,   548,
     550,   567,   568,   569,   570,   571,   572,   573,   574,   581,
     587,   589,   591,   593,   595,   601,   603,   619,   629,   647,
     664,   675,   698,   720,   730,   747,   764,   773,   791,   799,
     809,   815,   820,   828,   838,   849,   859,   869,   875,   876,
     877,   878,   879,   884,   893,   899,   901,   907,   909,   918,
     920,   925,   931,   932,   933,   934,   936,   937,   940,   942,
     943,   945,   946,   948,   950,   952,   954,   957,   958,   961,
     963,   966,   967,   970,   973,   974,   976,   978,   981,   982,
     985,   986,   987,   988,   989,   990,   991,   992,   993,   995,
     997,   999,  1000,  1001,  1002,  1003,  1004,  1005,  1006,  1007,
    1009,  1010,  1011,  1012,  1013,  1014,  1015,  1016,  1017,  1018,
    1019,  1021,  1023,  1028,  1029,  1032,  1033,  1036,  1039,  1041,
    1044,  1045,  1047,  1048,  1051,  1053,  1055,  1056,  1058,  1060,
    1062,  1064,  1066,  1069,  1071,  1073,  1075,  1077,  1079,  1083,
    1084,  1087,  1090,  1091,  1093,  1094,  1097,  1099,  1101,  1108,
    1109,  1111,  1112,  1115,  1117,  1119,  1126,  1128,  1132,  1133,
    1136,  1137,  1139,  1140,  1143,  1145,  1148,  1151,  1153,  1156,
    1157,  1160,  1161,  1165,  1167,  1170,  1172,  1175,  1176,  1178,
    1179,  1182,  1183,  1184,  1185,  1187,  1188,  1190,  1191,  1198,
    1199,  1206,  1207,  1217,  1218,  1219,  1225,  1231,  1232,  1239,
    1239,  1240,  1240,  1240,  1240,  1240,  1240,  1240,  1240,  1241,
    1241,  1242
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NAME", "INTEGER",
  "SC_FLOAT", "ACCIDENTAL", "SYMBOL", "STRING", "ASCII", "CLASSNAME",
  "PIE", "WHILE", "PRIMITIVENAME", "LEFTARROW", "TRUEOBJ", "FALSEOBJ",
  "NILOBJ", "PSEUDOVAR", "VAR", "ARG", "CLASSVAR", "SC_CONST", "ELLIPSIS",
  "DOTDOT", "BEGINCLOSEDFUNC", "BADTOKEN", "INTERPRET", "BEGINGENERATOR",
  "CURRYARG", "':'", "'='", "BINOP", "KEYBINOP", "'-'", "'<'", "'>'",
  "'*'", "'+'", "'|'", "READWRITEVAR", "'.'", "'`'", "UMINUS", "'{'",
  "'}'", "'['", "']'", "';'", "','", "'('", "')'", "'^'", "'~'", "'#'",
  "$accept", "root", "classes", "classextensions", "classdef",
  "classextension", "optName", "optSuperName", "classvardecls",
  "classvardecl", "methods", "methoddef", "optsemi", "optcomma",
  "optequal", "funcbody", "cmdlinecode", "methbody", "optPrim", "retval",
  "funretval", "blocklist1", "blocklistitem", "blocklist", "msgsend",
  "generator", "$@1", "$@2", "nextqual", "qual", "expr1", "valrangex1",
  "valrangeassign", "valrangexd", "valrange2", "valrange3", "expr",
  "adverb", "exprn", "exprseq", "arrayelems", "arrayelems1", "arglist1",
  "arglistv1", "keyarglist1", "keyarg", "optkeyarglist", "mavars",
  "nameList", "slotliteral", "blockliteral", "pushname", "pushliteral",
  "listliteral", "block", "funcvardecls", "funcvardecls1", "funcvardecl",
  "newBlockLiteral", "newBlock", "newBodyOpt", "newBody", "newBodyItem",
  "argdecls", "argdecls1", "constdeflist", "constdef", "slotdeflist0",
  "slotdeflist", "slotdef", "vardeflist0", "vardeflist", "vardef",
  "dictslotdef", "dictslotlist1", "dictslotlist", "rwslotdeflist",
  "rwslotdef", "dictlit2", "litdictslotdef", "litdictslotlist1",
  "litdictslotlist", "listlit", "listlit2", "literallistc", "literallist1",
  "rwspec", "rspec", "integer", "floatr", "accidental", "floatp", "name",
  "binop", "binop2", "curryArg", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-395)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-299)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      -5,   654,    46,    59,    82,    66,  -395,  -395,  -395,  -395,
    -395,  -395,  -395,  -395,   209,  -395,  -395,  -395,  -395,  -395,
      86,    86,    93,  -395,   317,    86,  1792,    16,  1272,   752,
    1792,    86,    74,  -395,  -395,  -395,  -395,  -395,    51,  -395,
    -395,  -395,  1847,    91,    94,  -395,  -395,  -395,  -395,  1012,
    -395,  -395,  -395,  1012,  -395,   152,   165,  -395,  -395,   213,
    -395,   136,  -395,    23,  -395,  -395,  1272,   856,   111,  -395,
    -395,  -395,   203,  -395,   147,   160,   217,    86,    86,  -395,
    -395,  -395,  -395,  -395,   167,     5,  -395,   135,   804,  -395,
    1792,  1792,  -395,  1792,   162,   155,   163,  1792,  1324,  -395,
    1792,   317,  -395,  -395,  -395,  -395,    17,  -395,   171,   170,
    1012,  1012,  -395,   188,   177,  -395,   194,  1875,   212,   216,
     602,   205,   227,     1,  -395,  1376,  -395,  -395,  -395,    35,
     226,  1792,  -395,  -395,  -395,  -395,  -395,  1012,  -395,  -395,
    1792,  1064,   111,  -395,   265,    86,   238,   224,  1792,  1792,
     111,  -395,   246,   269,   275,  -395,  -395,  -395,    86,  1792,
    1792,    86,  -395,   273,   232,   279,    12,  1012,    86,  -395,
    -395,    86,  -395,   482,  -395,  -395,  1012,  -395,  1792,  -395,
    1428,  -395,  -395,  1792,   263,    85,  -395,  -395,  1792,  1792,
    1792,  -395,   278,   284,  1012,  1480,  -395,  -395,   157,  -395,
    -395,  1792,   602,  -395,  -395,  -395,   291,  -395,  -395,  -395,
     602,  1844,  -395,  -395,  -395,   293,   289,   152,  -395,  -395,
      93,  1792,    86,    86,  1792,    18,  1532,   908,   253,    28,
    1792,  1847,  -395,  1847,   111,   246,   269,   677,  -395,   296,
    -395,  -395,  -395,  -395,  -395,   111,  -395,  1116,  -395,   295,
     311,   298,   311,   299,  -395,  1847,   300,   256,    86,  -395,
      86,  -395,   307,  -395,    44,  -395,  -395,  -395,  -395,  -395,
    -395,  -395,  1792,     6,  -395,  -395,   152,  -395,   304,   305,
     312,  -395,  1792,   326,  -395,  -395,  1792,  1792,  -395,  -395,
     334,  -395,  -395,   308,  1792,   330,  -395,  1168,   111,  1847,
     316,   602,   318,   602,   337,  -395,   315,   324,  -395,   602,
    -395,  1584,  1847,  -395,  -395,   322,  1636,   345,  1792,  1792,
      52,   111,   246,   269,   275,  1792,   960,  -395,   376,  1792,
    -395,  -395,   342,   111,   335,   336,   707,  -395,  -395,   344,
     349,   364,   192,  1792,  -395,   275,   111,  -395,  -395,   111,
    -395,  -395,    86,   261,    57,  -395,  -395,    86,   348,  1220,
    1220,  -395,  -395,  1792,  -395,   372,  1792,  -395,   111,   246,
     269,  -395,   353,  -395,  -395,   602,  1844,  -395,  -395,  -395,
      86,  1847,   356,  -395,   357,   373,   375,   361,  1792,   362,
    1688,   379,  -395,   360,   371,   377,  1847,   111,   246,   269,
     275,   378,   125,  -395,   369,   386,    93,    93,   387,   105,
     105,   380,  -395,   691,  -395,  -395,  -395,  -395,   389,  -395,
      86,  -395,    86,   384,  -395,    86,     9,   393,   394,   123,
     397,  -395,  1792,  -395,   111,   398,   399,  -395,  -395,  -395,
     403,  -395,  1584,  -395,  1792,  1792,   417,  1847,   423,   424,
     409,  1792,   111,  -395,   111,  -395,   407,   410,   412,  -395,
    1792,  -395,    93,    93,  -395,  -395,  -395,  -395,  -395,  -395,
     264,  -395,    86,   283,  -395,   285,  -395,    86,  -395,  -395,
     416,   437,  -395,   446,  1792,  1792,  -395,  1220,  -395,  1792,
     465,  -395,  -395,   111,  -395,  -395,  1847,  1847,  1792,  1792,
    1792,   449,  1847,  -395,  -395,   111,  -395,   111,  1847,  -395,
    -395,    47,    47,   192,  -395,   105,   451,  -395,  -395,   380,
     453,  -395,  -395,  1792,   394,   394,  -395,   394,  1792,  -395,
    1847,  1847,  1847,  1792,  -395,  -395,    47,    47,   447,  1740,
    1740,  1861,  -395,   544,  -395,   544,   394,  -395,  -395,  -395,
     394,  1847,  1740,  1740,  -395,  1792,   440,  -395,   442,   457,
    -395,  -395,  -395,  -395,  -395,   459,   461,  1875,  -395,  -395,
    -395,  -395,  -395,  -395
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       5,    48,     0,     0,     2,     3,     7,   289,   277,   279,
     281,   185,   184,   183,   131,   287,   290,   186,   187,   188,
       0,   239,   216,   301,     0,   232,     0,   216,   149,   250,
       0,     0,     0,    41,     4,    33,    97,    94,   128,   105,
     130,   129,   146,    27,    48,    92,    95,    91,   179,    48,
     205,    93,   208,    48,   189,   181,   283,   284,   182,   180,
      96,     0,     1,    14,     6,     8,   149,     0,    65,    50,
      53,    52,     0,   241,   243,     0,   240,   239,   232,   203,
     278,   280,   282,   288,     0,    29,   234,    31,   250,   133,
       0,     0,   203,     0,   151,     0,    29,     0,     0,   291,
     300,   295,   293,   294,   296,   297,   232,   292,     0,     0,
      48,    48,   248,    29,     0,   299,     0,    27,    99,     0,
     267,     0,     0,   166,   168,     0,   300,   295,   298,     0,
     142,    28,   148,    34,    40,   206,    39,    48,   286,   285,
       0,     0,    56,    21,     0,    12,     0,     0,     0,     0,
      54,   157,    29,    29,    29,   161,    51,   207,     0,     0,
       0,     0,   223,     0,   240,     0,    29,    48,     0,   226,
      30,     0,    32,     0,    79,    81,    48,   153,     0,   100,
      30,   150,   119,     0,     0,     0,   247,   101,   118,     0,
       0,    98,     0,     0,    48,    30,   251,   103,     0,    28,
      49,     0,   267,   194,   193,   192,     0,   196,   197,   198,
     267,   261,   269,   200,   199,     0,    29,   190,   191,   195,
     216,     0,     0,     0,     0,     0,     0,     0,    54,     0,
       0,   147,    38,   135,     0,    29,    29,     0,    15,     0,
      13,    16,    64,   163,   159,    55,    66,    30,   164,     0,
      30,     0,    30,     0,   242,   244,     0,     0,     0,   217,
       0,   220,     0,   204,     0,   235,   174,   173,   172,   175,
     176,   177,     0,     0,   237,   178,   170,   171,     0,     0,
       0,   152,     0,   154,   123,   102,   124,     0,   120,   246,
       0,    37,    36,     0,     0,     0,   249,     0,    57,   136,
       0,   267,     0,     0,     0,   259,    29,     0,   263,    30,
     268,   210,   139,   167,   169,     0,     0,   104,     0,     0,
       0,    54,    29,    29,    29,     0,     0,    78,     0,     0,
     144,   143,   134,    58,     0,     0,   296,    11,    22,     0,
       0,    14,    21,     0,   158,    29,    54,    69,   162,    54,
     245,   224,     0,     0,     0,   202,   227,     0,     0,     0,
       0,   201,   155,     0,   125,     0,   122,    35,     0,    29,
      29,   264,     0,   265,   258,     0,    30,   262,   256,   270,
       0,   215,     0,   211,    27,   107,   106,     0,     0,     0,
       0,   132,    70,     0,     0,     0,   137,    54,    29,    29,
      29,     0,    54,    62,     0,     0,   216,   216,     0,   271,
     271,   275,    17,     0,   160,   165,    68,    67,     0,   218,
       0,   221,     0,     0,   238,     0,     0,     0,    83,   180,
       0,   156,   126,   121,    60,     0,     0,   266,   257,   260,
     214,   209,    28,   212,     0,     0,   108,   140,   113,   112,
       0,     0,    54,    74,    54,    75,     0,     0,     0,   145,
       0,    59,   216,   216,   203,   203,    16,   272,   274,   273,
       0,   252,     0,     0,   276,    29,   229,     0,     9,   225,
       0,     0,   228,     0,     0,     0,    80,     0,    88,     0,
       0,    82,   127,    54,    63,   213,   110,   109,     0,     0,
       0,   114,   141,    73,    71,    54,    77,    54,   138,   203,
     203,    44,    44,    21,    19,   271,   254,    18,    20,   275,
       0,   219,   222,     0,    83,    83,    84,    83,     0,    61,
     111,   116,   115,     0,    76,    72,    44,    44,    27,    46,
      46,     0,   253,     0,   230,     0,    83,    90,    89,    85,
      83,   117,    46,    46,    45,     0,     0,    42,    46,     0,
      10,   255,   231,    87,    86,     0,     0,    27,    23,    43,
      25,    24,    26,    47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -395,  -395,  -395,  -395,  -395,   491,  -395,   168,    48,  -395,
    -328,  -395,  -114,   120,  -395,    -8,  -395,  -233,  -264,   -50,
     468,    -2,   -42,   179,  -395,    49,  -395,  -395,  -300,  -343,
    -395,  -395,  -395,  -395,  -395,  -395,   -26,  -395,  -395,   -18,
     452,  -395,  -116,  -133,   -66,   271,     4,  -395,  -395,  -394,
     247,  -395,  -395,  -188,  -395,   -74,   -23,    42,  -395,  -395,
    -395,    73,  -395,   -22,   488,  -395,     7,   455,   460,   359,
     450,   -19,   366,   346,  -395,  -395,   121,    22,  -395,   164,
    -395,  -395,  -166,  -395,  -183,  -395,  -395,  -395,   -86,  -395,
    -395,   -83,   210,  -224,   -14,  -395
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     3,     4,     5,    64,     6,   239,   146,   342,   412,
     237,   338,   132,   248,   173,    33,    34,   556,   539,   557,
      35,   245,    69,   246,    36,    37,   278,   279,   488,   427,
      38,    39,    40,    41,   108,   184,    42,   230,    43,    44,
      95,    96,   152,   153,   345,   155,   249,   122,   123,   274,
      45,    46,    47,   212,    48,   167,    49,   263,    51,    52,
     382,   383,   384,    79,    53,   475,   476,    84,    85,    86,
      75,    72,    73,   112,   113,   114,   470,   471,   213,   305,
     306,   307,    54,   214,   215,   216,   472,   477,    55,    56,
      57,    58,    59,   115,   130,    60
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      89,   154,    76,   200,   117,    92,   110,   275,   236,   225,
      94,   109,    68,   340,   413,   116,   119,   430,   176,   300,
       7,   484,     1,   304,   222,   235,   156,   302,  -233,    16,
     137,     7,     8,     2,   217,  -233,    77,   218,     7,   485,
      16,   134,   316,    50,   169,   136,    90,    16,    94,   151,
     223,   261,   120,   144,   170,    78,    61,   142,   164,    62,
     538,   170,   328,    70,    91,   317,    20,   318,  -298,   145,
     109,    50,   174,   175,   116,   177,   390,     7,   329,   182,
     185,   226,   186,   356,   119,   227,    16,   276,   194,     7,
     277,   135,    63,   357,   323,    50,   421,   125,    16,   391,
     156,   318,   192,   193,     2,   231,   422,   151,    70,   286,
     320,   322,   405,    77,   233,   374,   217,    70,   372,   218,
     120,   379,   121,   151,   217,   217,     7,   218,   218,   232,
     243,   244,    78,   255,   287,    16,    22,   489,  -236,   131,
     467,   468,   256,   330,   526,   469,    30,  -236,    22,   561,
      22,   562,   135,    50,   140,    27,   460,   251,  -236,   262,
     281,   324,   283,   138,   370,   284,   172,    27,   280,    27,
     288,   289,   290,   141,  -236,   299,   139,   295,   159,   135,
     143,   369,    22,   161,  -236,   541,   293,   438,   304,   340,
     168,    70,   178,   399,   188,   312,   298,   160,   311,    70,
     189,    27,   179,   156,   332,   171,   315,   297,   151,   151,
     398,   409,   180,   410,   411,   217,   181,   217,   218,   190,
     218,   191,   187,   217,   547,   548,   218,   549,   197,   344,
      74,    74,   333,   196,    22,    87,   135,   195,    22,   334,
     335,   118,   124,   201,   140,   198,   563,    70,   540,   220,
     564,   157,   158,    27,   358,    66,   156,    27,   221,    67,
     400,    71,   202,   141,   362,   162,   158,   229,   364,   365,
     443,   242,   552,   553,   253,   238,   186,    70,    22,   151,
     259,   158,   241,    70,   325,   381,   171,    74,    87,   217,
     217,   156,   218,   218,    70,   247,   258,    27,   387,   396,
     344,   389,   260,   326,   351,   352,    71,   559,   151,   419,
     420,   401,   514,   515,   285,    71,    87,   340,   250,   565,
     566,    80,    81,    82,   252,   414,   393,   394,    83,   291,
     219,   517,   515,   518,   170,   292,   310,   301,   309,   228,
     308,   428,   428,   341,   148,   431,   346,    70,   433,   347,
     349,   350,   355,   359,   360,   240,   363,   361,   366,   367,
     189,   440,   447,   371,   376,   373,   434,   375,    74,   385,
      70,   257,   450,   435,   436,   378,   388,   275,   264,   275,
      80,    87,    70,   129,   464,   465,   402,   403,   406,    71,
     511,   512,   156,   407,   144,    70,   432,    71,    70,   424,
     437,   441,   456,   457,   444,   442,   445,   327,   446,   448,
     451,   452,   219,   462,   492,   474,   381,    70,   496,   497,
     219,   219,   453,   482,   554,   502,   377,   142,   454,   459,
     463,   466,   313,   314,   508,   536,   537,   479,   486,   331,
     509,   510,   491,   487,   395,    71,    70,   339,   498,   493,
     494,    70,   158,   573,   499,   500,   501,   276,   505,   276,
     277,   506,   277,   507,   521,   415,   524,   525,   353,   428,
     354,   527,   530,   531,   532,    71,   522,   523,    70,   528,
     533,    71,   543,    70,   545,   568,     8,     9,    10,   266,
     267,   268,    71,    15,   555,   199,    65,   269,   270,   271,
     392,    70,   570,    70,   571,   546,   572,   551,   569,   408,
     550,   219,   133,   219,   513,   495,    24,   111,   147,   219,
     458,   558,   558,   348,   254,   416,   544,   163,   417,   567,
     265,   473,   272,   165,   558,   558,   273,   542,   166,     0,
     439,   296,    70,     0,     0,    71,   404,     0,     8,     9,
      10,   266,   267,   268,    70,    15,    70,     0,     0,   269,
     270,   271,   418,     0,     0,     0,     0,   423,    71,   429,
     429,     0,     0,     0,     0,     0,   455,     0,    24,     0,
      71,   461,     0,     0,     0,   219,   219,     0,     0,     0,
      74,     0,     0,    71,     0,   519,    71,     0,   273,     0,
       0,     0,     0,     0,     0,     7,     8,     9,    10,   203,
     204,   205,   206,    15,    16,    71,     0,   207,   208,   209,
       0,     0,     0,   339,     0,     0,     0,     0,     0,     0,
     480,   503,   481,   504,     0,   483,    24,     0,     0,   490,
       0,     0,     0,     0,    71,     0,     0,     0,   210,    71,
       0,     0,   211,     0,     0,     0,     0,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,     0,     0,    17,
      18,    19,   529,    20,    21,     0,    71,     0,     0,    22,
       7,    71,   516,    23,   534,     0,   535,   520,    24,    16,
       0,     0,     0,    25,     7,     0,    26,   429,    27,    71,
      28,    71,     0,    16,    29,     0,    30,    31,    32,    99,
       7,   127,   102,   103,   336,   105,   128,   107,     0,    16,
       0,     0,   337,    99,     0,   127,   102,   103,   336,   105,
     128,   107,     0,     0,     0,     0,   478,     0,     0,    99,
      71,   127,   102,   103,   104,   105,   128,   107,     0,     0,
       0,   339,    71,     0,    71,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,     0,     0,    17,    18,    19,
       0,    20,    21,     0,     0,     0,    97,    22,     0,     0,
       0,    23,    98,     0,    99,   100,   101,   102,   103,   104,
     105,   106,   107,     0,    26,     0,    27,     0,    28,     0,
       0,     0,    88,     0,     0,    31,    32,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,     0,     0,    17,
      18,    19,     0,     0,     0,     0,     0,     0,    97,    22,
       0,     0,     0,    23,    98,     0,    99,   100,   101,   102,
     103,   104,   105,   128,   107,     0,    26,     0,    27,     0,
      28,     0,     0,     0,    88,     0,     0,    31,    32,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,     0,
       0,    17,    18,    19,     0,     0,     0,     0,     0,     0,
       0,    22,     0,     0,     0,    23,     0,     0,     0,   148,
      24,     0,     0,   149,     0,     0,     0,     0,    26,     0,
      27,     0,    28,     0,     0,     0,    88,   150,     0,    31,
      32,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,     0,     0,    17,    18,    19,     0,     0,     0,     0,
       0,     0,     0,    22,     0,     0,     0,    23,     0,     0,
       0,   148,    24,     0,     0,   149,     0,     0,     0,     0,
      26,     0,    27,     0,    28,     0,     0,     0,    88,   321,
       0,    31,    32,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,     0,     0,    17,    18,    19,     0,     0,
       0,     0,     0,     0,     0,    22,     0,     0,     0,    23,
       0,     0,     0,   148,    24,     0,     0,   149,     0,     0,
       0,     0,    26,     0,    27,     0,    28,     0,     0,     0,
      88,   397,     0,    31,    32,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,     0,     0,    17,    18,    19,
       0,    20,     0,     0,     0,     0,     0,    22,     0,     0,
       0,    23,     0,     0,     0,     0,    24,     0,     0,     0,
       0,     0,     0,     0,    26,     0,    27,     0,    28,     0,
       0,     0,    88,     0,    30,    31,    32,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,     0,     0,    17,
      18,    19,     0,     0,     0,     0,     0,     0,     0,    22,
       0,     0,     0,    23,     0,     0,     0,     0,    24,     0,
       0,   149,     0,     0,     0,     0,    26,     0,    27,     0,
      28,     0,     0,     0,    88,   234,     0,    31,    32,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,     0,
       0,    17,    18,    19,     0,     0,     0,     0,     0,     0,
       0,    22,     0,     0,     0,    23,     0,     0,     0,   148,
      24,     0,     0,   343,     0,     0,     0,     0,    26,     0,
      27,     0,    28,     0,     0,     0,    88,     0,     0,    31,
      32,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,     0,     0,    17,    18,    19,     0,     0,     0,     0,
       0,     0,     0,    22,     0,     0,     0,    23,     0,     0,
       0,     0,    24,     0,     0,   149,     0,     0,     0,     0,
      26,     0,    27,     0,    28,     0,     0,     0,    88,   368,
       0,    31,    32,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,     0,     0,    17,    18,    19,     0,   425,
       0,     0,     0,     0,     0,    22,     0,     0,     0,    23,
     426,     0,     0,     0,    24,     0,     0,     0,     0,     0,
       0,     0,    26,     0,    27,     0,    28,     0,     0,     0,
      88,     0,     0,    31,    32,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,     0,     0,    17,    18,    19,
       0,     0,     0,     0,     0,     0,     0,    22,     0,     0,
       0,    23,     0,     0,     0,    93,    24,     0,     0,     0,
       0,     0,     0,     0,    26,     0,    27,     0,    28,     0,
       0,     0,    88,     0,     0,    31,    32,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,     0,     0,    17,
      18,    19,     0,     0,     0,     0,     0,     0,   183,    22,
       0,     0,     0,    23,     0,     0,     0,     0,    24,     0,
       0,     0,     0,     0,     0,     0,    26,     0,    27,     0,
      28,     0,     0,     0,    88,     0,     0,    31,    32,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,     0,
       0,    17,    18,    19,     0,     0,     0,     0,     0,     0,
     224,    22,     0,     0,     0,    23,     0,     0,     0,     0,
      24,     0,     0,     0,     0,     0,     0,     0,    26,     0,
      27,     0,    28,     0,     0,     0,    88,     0,     0,    31,
      32,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,     0,     0,    17,    18,    19,     0,     0,     0,     0,
       0,     0,     0,    22,     0,     0,     0,    23,     0,     0,
       0,   282,    24,     0,     0,     0,     0,     0,     0,     0,
      26,     0,    27,     0,    28,     0,     0,     0,    88,     0,
       0,    31,    32,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,     0,     0,    17,    18,    19,     0,     0,
       0,     0,     0,     0,     0,    22,     0,     0,     0,    23,
       0,     0,     0,   294,    24,     0,     0,     0,     0,     0,
       0,     0,    26,     0,    27,     0,    28,     0,     0,     0,
      88,     0,     0,    31,    32,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,     0,     0,    17,    18,    19,
       0,     0,     0,     0,     0,     0,   319,    22,     0,     0,
       0,    23,     0,     0,     0,     0,    24,     0,     0,     0,
       0,     0,     0,     0,    26,     0,    27,     0,    28,     0,
       0,     0,    88,     0,     0,    31,    32,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,     0,     0,    17,
      18,    19,     0,   380,     0,     0,     0,     0,     0,    22,
       0,     0,     0,    23,     0,     0,     0,     0,    24,     0,
       0,     0,     0,     0,     0,     0,    26,     0,    27,     0,
      28,     0,     0,     0,    88,     0,     0,    31,    32,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,     0,
       0,    17,    18,    19,     0,     0,     0,     0,     0,     0,
       0,    22,     0,     0,     0,    23,     0,     0,     0,     0,
      24,     0,     0,     0,     0,     0,     0,     0,    26,     0,
      27,     0,    28,   386,     0,     0,    88,     0,     0,    31,
      32,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,     0,     0,    17,    18,    19,     0,     0,     0,     0,
       0,     0,     0,    22,     0,     0,     0,    23,     0,     0,
       0,     0,    24,     0,     0,     0,     0,     0,     0,     0,
      26,     0,    27,     0,    28,   449,     0,     0,    88,     0,
       0,    31,    32,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,     0,     0,    17,    18,    19,     0,     0,
       0,     0,     0,     0,     0,    22,     0,     0,     0,    23,
       0,     0,     0,     0,    24,     0,     0,     0,     0,     0,
       0,     0,    26,     0,    27,     0,    28,     0,     0,     0,
      88,     0,   555,    31,    32,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,     0,     0,    17,    18,    19,
       0,     0,     0,     0,     0,     0,     0,    22,     0,     0,
       0,    23,     0,     0,     0,     0,    24,     0,     0,     0,
       0,     0,     0,     0,    26,     0,    27,     0,    28,     0,
       0,     0,    88,     0,     0,    31,    32,     7,     8,     9,
      10,   203,   204,   205,   206,    15,    16,     0,     0,   207,
     208,   209,     0,     0,     7,     0,     0,     0,     0,     0,
       0,     0,     0,    16,     0,     0,     0,   303,    24,    99,
     126,   127,   102,   103,   104,   105,   128,   107,   129,     0,
     210,     0,     0,    99,   211,   127,   102,   103,   336,   105,
     128,   107,     0,     0,     0,     0,   560,    99,   126,   127,
     102,   103,   104,   105,   128,   107,   129,     0,     0,     0,
       0,     0,     0,   199
};

static const yytype_int16 yycheck[] =
{
      26,    67,    21,   117,    30,    27,    29,   173,   141,   125,
      28,    29,    14,   237,   342,    29,    10,   360,    92,   202,
       3,    12,    27,   211,    23,   141,    68,   210,    23,    12,
      53,     3,     4,    38,   120,    23,    20,   120,     3,    30,
      12,    49,    24,     1,    39,    53,    30,    12,    66,    67,
      49,    39,    46,    30,    49,    39,    10,    59,    77,     0,
      13,    49,    34,    14,    48,    47,    19,    49,    51,    46,
      88,    29,    90,    91,    88,    93,    24,     3,    50,    97,
      98,    46,   100,    39,    10,    50,    12,   173,   111,     3,
     173,    49,    10,    49,   227,    53,    39,    46,    12,    47,
     142,    49,   110,   111,    38,   131,    49,   125,    59,    24,
     226,   227,   336,    20,   140,   303,   202,    68,   301,   202,
      46,   309,    48,   141,   210,   211,     3,   210,   211,   137,
     148,   149,    39,   159,    49,    12,    25,    14,     3,    48,
      35,    36,   160,   229,   487,    40,    52,    12,    25,   543,
      25,   545,   110,   111,    31,    44,    31,   153,    23,   167,
     178,   227,   180,    11,   297,   183,    31,    44,   176,    44,
     188,   189,   190,    50,    39,   201,    11,   195,    31,   137,
      44,   297,    25,    23,    49,   513,   194,   375,   376,   413,
      23,   142,    30,   326,    24,   221,   198,    50,   220,   150,
      30,    44,    47,   245,   230,    85,   224,    50,   226,   227,
     326,    19,    49,    21,    22,   301,    96,   303,   301,    49,
     303,    51,    51,   309,   524,   525,   309,   527,    51,   247,
      20,    21,   234,   113,    25,    25,   194,    49,    25,   235,
     236,    31,    32,    31,    31,    51,   546,   198,   512,    44,
     550,    48,    49,    44,   272,    46,   298,    44,    31,    50,
     326,    14,    46,    50,   282,    48,    49,    41,   286,   287,
     384,    47,   536,   537,   154,    10,   294,   228,    25,   297,
      48,    49,    44,   234,    31,   311,   166,    77,    78,   375,
     376,   333,   375,   376,   245,    49,    23,    44,   316,   325,
     318,   319,    23,    50,    48,    49,    59,   540,   326,    48,
      49,   329,    48,    49,    51,    68,   106,   541,    49,   552,
     553,     4,     5,     6,    49,   343,   322,   323,    11,    51,
     120,    48,    49,    48,    49,    51,   216,    46,    49,   129,
      47,   359,   360,    47,    33,   363,    51,   298,   366,    51,
      51,    51,    45,    49,    49,   145,    30,    45,    24,    51,
      30,   380,   388,    47,    49,    47,   368,    30,   158,    47,
     321,   161,   390,   369,   370,    51,    31,   543,   168,   545,
       4,   171,   333,    41,   406,   407,    51,    51,    44,   142,
     464,   465,   434,    44,    30,   346,    24,   150,   349,    51,
      47,    45,   398,   399,    31,    48,    31,   228,    47,    47,
      31,    51,   202,    44,   432,    35,   442,   368,   444,   445,
     210,   211,    51,    39,   538,   451,   306,   429,    51,    51,
      44,    44,   222,   223,   460,   509,   510,    48,    45,   229,
     462,   463,    45,    49,   324,   198,   397,   237,    31,    51,
      51,   402,    49,   567,    31,    31,    47,   543,    51,   545,
     543,    51,   545,    51,    48,   345,   484,   485,   258,   487,
     260,   489,   498,   499,   500,   228,    39,    31,   429,    14,
      31,   234,    31,   434,    31,    45,     4,     5,     6,     7,
       8,     9,   245,    11,    52,    48,     5,    15,    16,    17,
     321,   452,    45,   454,    45,   523,    45,   533,   558,   341,
     528,   301,    44,   303,   466,   442,    34,    29,    66,   309,
     400,   539,   540,   252,   158,   346,   519,    77,   349,   555,
     171,   410,    50,    78,   552,   553,    54,   515,    78,    -1,
     376,   195,   493,    -1,    -1,   298,   336,    -1,     4,     5,
       6,     7,     8,     9,   505,    11,   507,    -1,    -1,    15,
      16,    17,   352,    -1,    -1,    -1,    -1,   357,   321,   359,
     360,    -1,    -1,    -1,    -1,    -1,   397,    -1,    34,    -1,
     333,   402,    -1,    -1,    -1,   375,   376,    -1,    -1,    -1,
     380,    -1,    -1,   346,    -1,   475,   349,    -1,    54,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,   368,    -1,    15,    16,    17,
      -1,    -1,    -1,   413,    -1,    -1,    -1,    -1,    -1,    -1,
     420,   452,   422,   454,    -1,   425,    34,    -1,    -1,   429,
      -1,    -1,    -1,    -1,   397,    -1,    -1,    -1,    46,   402,
      -1,    -1,    50,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    -1,    -1,    15,
      16,    17,   493,    19,    20,    -1,   429,    -1,    -1,    25,
       3,   434,   472,    29,   505,    -1,   507,   477,    34,    12,
      -1,    -1,    -1,    39,     3,    -1,    42,   487,    44,   452,
      46,   454,    -1,    12,    50,    -1,    52,    53,    54,    32,
       3,    34,    35,    36,    37,    38,    39,    40,    -1,    12,
      -1,    -1,    45,    32,    -1,    34,    35,    36,    37,    38,
      39,    40,    -1,    -1,    -1,    -1,    45,    -1,    -1,    32,
     493,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
      -1,   541,   505,    -1,   507,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    -1,    -1,    15,    16,    17,
      -1,    19,    20,    -1,    -1,    -1,    24,    25,    -1,    -1,
      -1,    29,    30,    -1,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    -1,    -1,    15,
      16,    17,    -1,    -1,    -1,    -1,    -1,    -1,    24,    25,
      -1,    -1,    -1,    29,    30,    -1,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    42,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    -1,
      -1,    15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    25,    -1,    -1,    -1,    29,    -1,    -1,    -1,    33,
      34,    -1,    -1,    37,    -1,    -1,    -1,    -1,    42,    -1,
      44,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,    53,
      54,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    -1,    -1,    15,    16,    17,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    25,    -1,    -1,    -1,    29,    -1,    -1,
      -1,    33,    34,    -1,    -1,    37,    -1,    -1,    -1,    -1,
      42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    51,
      -1,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    -1,    -1,    15,    16,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    25,    -1,    -1,    -1,    29,
      -1,    -1,    -1,    33,    34,    -1,    -1,    37,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,
      50,    51,    -1,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    -1,    -1,    15,    16,    17,
      -1,    19,    -1,    -1,    -1,    -1,    -1,    25,    -1,    -1,
      -1,    29,    -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    -1,    52,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    -1,    -1,    15,
      16,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,
      -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    34,    -1,
      -1,    37,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    -1,
      -1,    15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    25,    -1,    -1,    -1,    29,    -1,    -1,    -1,    33,
      34,    -1,    -1,    37,    -1,    -1,    -1,    -1,    42,    -1,
      44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,    53,
      54,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    -1,    -1,    15,    16,    17,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    25,    -1,    -1,    -1,    29,    -1,    -1,
      -1,    -1,    34,    -1,    -1,    37,    -1,    -1,    -1,    -1,
      42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    51,
      -1,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    -1,    -1,    15,    16,    17,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    25,    -1,    -1,    -1,    29,
      30,    -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,
      50,    -1,    -1,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    -1,    -1,    15,    16,    17,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,    -1,    -1,
      -1,    29,    -1,    -1,    -1,    33,    34,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    -1,    -1,    15,
      16,    17,    -1,    -1,    -1,    -1,    -1,    -1,    24,    25,
      -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    34,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    -1,
      -1,    15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      24,    25,    -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,
      34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,
      44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,    53,
      54,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    -1,    -1,    15,    16,    17,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    25,    -1,    -1,    -1,    29,    -1,    -1,
      -1,    33,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,
      -1,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    -1,    -1,    15,    16,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    25,    -1,    -1,    -1,    29,
      -1,    -1,    -1,    33,    34,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,
      50,    -1,    -1,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    -1,    -1,    15,    16,    17,
      -1,    -1,    -1,    -1,    -1,    -1,    24,    25,    -1,    -1,
      -1,    29,    -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    -1,    -1,    15,
      16,    17,    -1,    19,    -1,    -1,    -1,    -1,    -1,    25,
      -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    34,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    -1,
      -1,    15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    25,    -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,
      34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,
      44,    -1,    46,    47,    -1,    -1,    50,    -1,    -1,    53,
      54,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    -1,    -1,    15,    16,    17,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    25,    -1,    -1,    -1,    29,    -1,    -1,
      -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      42,    -1,    44,    -1,    46,    47,    -1,    -1,    50,    -1,
      -1,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    -1,    -1,    15,    16,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    25,    -1,    -1,    -1,    29,
      -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,
      50,    -1,    52,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    -1,    -1,    15,    16,    17,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,    -1,    -1,
      -1,    29,    -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    -1,    -1,    15,
      16,    17,    -1,    -1,     3,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    12,    -1,    -1,    -1,    33,    34,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    -1,
      46,    -1,    -1,    32,    50,    34,    35,    36,    37,    38,
      39,    40,    -1,    -1,    -1,    -1,    45,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    -1,    -1,    -1,
      -1,    -1,    -1,    48
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    27,    38,    56,    57,    58,    60,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    15,    16,    17,
      19,    20,    25,    29,    34,    39,    42,    44,    46,    50,
      52,    53,    54,    70,    71,    75,    79,    80,    85,    86,
      87,    88,    91,    93,    94,   105,   106,   107,   109,   111,
     112,   113,   114,   119,   137,   143,   144,   145,   146,   147,
     150,    10,     0,    10,    59,    60,    46,    50,    76,    77,
      80,   105,   126,   127,   147,   125,   126,    20,    39,   118,
       4,     5,     6,    11,   122,   123,   124,   147,    50,    91,
      30,    48,   118,    33,    94,    95,    96,    24,    30,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    89,    94,
     111,   119,   128,   129,   130,   148,   149,    91,   147,    10,
      46,    48,   102,   103,   147,    46,    33,    34,    39,    41,
     149,    48,    67,    75,    70,   112,    70,   111,    11,    11,
      31,    50,    76,    44,    30,    46,    62,    95,    33,    37,
      51,    94,    97,    98,    99,   100,    77,    48,    49,    31,
      50,    23,    48,   125,   126,   122,   123,   110,    23,    39,
      49,    68,    31,    69,    94,    94,   110,    94,    30,    47,
      49,    68,    94,    24,    90,    94,    94,    51,    24,    30,
      49,    51,    70,    70,   111,    49,    68,    51,    51,    48,
      67,    31,    46,     7,     8,     9,    10,    15,    16,    17,
      46,    50,   108,   133,   138,   139,   140,   143,   146,   147,
      44,    31,    23,    49,    24,    97,    46,    50,   147,    41,
      92,    91,    70,    91,    51,    97,    98,    65,    10,    61,
     147,    44,    47,    94,    94,    76,    78,    49,    68,   101,
      49,   101,    49,    68,   127,    91,    94,   147,    23,    48,
      23,    39,    70,   112,   147,   124,     7,     8,     9,    15,
      16,    17,    50,    54,   104,   137,   143,   146,    81,    82,
      70,    94,    33,    94,    94,    51,    24,    49,    94,    94,
      94,    51,    51,    70,    33,    94,   128,    50,    76,    91,
     139,    46,   139,    33,   108,   134,   135,   136,    47,    49,
      68,   118,    91,   147,   147,    94,    24,    47,    49,    24,
      97,    51,    97,    98,    99,    31,    50,    78,    34,    50,
     143,   147,    91,    76,   101,   101,    37,    45,    66,   147,
     148,    47,    63,    37,    94,    99,    51,    51,   100,    51,
      51,    48,    49,   147,   147,    45,    39,    49,    94,    49,
      49,    45,    94,    30,    94,    94,    24,    51,    51,    97,
      98,    47,   139,    47,   108,    30,    49,    68,    51,   108,
      19,    91,   115,   116,   117,    47,    47,    94,    31,    94,
      24,    47,    78,   101,   101,    68,    91,    51,    97,    98,
      99,    94,    51,    51,   147,   148,    44,    44,    62,    19,
      21,    22,    64,    65,    94,    68,    78,    78,   147,    48,
      49,    39,    49,   147,    51,    19,    30,    84,    94,   147,
      84,    94,    24,    94,    76,   101,   101,    47,   108,   134,
     126,    45,    48,    67,    31,    31,    47,    91,    47,    47,
      94,    31,    51,    51,    51,    78,   101,   101,    68,    51,
      31,    78,    44,    44,   118,   118,    44,    35,    36,    40,
     131,   132,   141,   131,    35,   120,   121,   142,    45,    48,
     147,   147,    39,   147,    12,    30,    45,    49,    83,    14,
     147,    45,    94,    51,    51,   116,    91,    91,    31,    31,
      31,    47,    91,    78,    78,    51,    51,    51,    91,   118,
     118,   110,   110,    63,    48,    49,   147,    48,    48,    68,
     147,    48,    39,    31,    94,    94,    84,    94,    14,    78,
      91,    91,    91,    31,    78,    78,   110,   110,    13,    73,
      73,    65,   132,    31,   121,    31,    94,    83,    83,    83,
      94,    91,    73,    73,    67,    52,    72,    74,    94,    72,
      45,   104,   104,    83,    83,    72,    72,    91,    45,    74,
      45,    45,    45,    67
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    55,    56,    56,    56,    57,    57,    58,    58,    59,
      59,    60,    61,    61,    62,    62,    63,    63,    64,    64,
      64,    65,    65,    66,    66,    66,    66,    67,    67,    68,
      68,    69,    69,    70,    70,    71,    71,    71,    71,    71,
      71,    71,    72,    72,    73,    73,    74,    74,    75,    75,
      76,    76,    77,    77,    78,    78,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    79,    79,    79,    81,
      80,    82,    80,    83,    83,    84,    84,    84,    84,    84,
      84,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    86,    86,    86,    87,
      87,    87,    88,    88,    88,    88,    88,    88,    89,    89,
      89,    89,    89,    90,    90,    90,    90,    90,    91,    91,
      91,    91,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    91,    92,    92,    92,    92,    93,    93,    94,    95,
      95,    96,    96,    96,    96,    96,    96,    97,    97,    98,
      98,    99,    99,   100,   101,   101,   102,   102,   103,   103,
     104,   104,   104,   104,   104,   104,   104,   104,   104,   105,
     106,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   109,   109,   110,   110,   111,   111,   112,   113,   114,
     115,   115,   116,   116,   117,   117,   118,   118,   118,   118,
     118,   118,   118,   119,   119,   119,   119,   119,   119,   120,
     120,   121,   122,   122,   123,   123,   124,   124,   124,   125,
     125,   126,   126,   127,   127,   127,   128,   128,   129,   129,
     130,   130,   131,   131,   132,   132,   133,   134,   134,   135,
     135,   136,   136,   137,   137,   138,   138,   139,   139,   140,
     140,   141,   141,   141,   141,   142,   142,   143,   143,   144,
     144,   145,   145,   146,   146,   146,   146,   146,   146,   147,
     147,   148,   148,   148,   148,   148,   148,   148,   148,   149,
     149,   150
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     0,     2,     1,     2,     6,
       9,     5,     0,     1,     0,     2,     0,     2,     3,     3,
       3,     0,     2,     7,     8,     7,     8,     0,     1,     0,
       1,     0,     1,     1,     2,     5,     4,     4,     3,     2,
       2,     1,     1,     2,     0,     2,     0,     3,     0,     3,
       1,     2,     1,     1,     0,     1,     2,     4,     4,     6,
       6,     8,     5,     7,     4,     2,     4,     6,     6,     5,
       5,     7,     8,     7,     6,     6,     8,     7,     4,     0,
       7,     0,     7,     0,     2,     4,     5,     5,     2,     4,
       4,     1,     1,     1,     1,     1,     1,     1,     3,     2,
       3,     3,     4,     3,     4,     1,     5,     5,     6,     7,
       7,     8,     6,     6,     7,     8,     8,     9,     2,     2,
       3,     5,     4,     2,     2,     3,     4,     5,     1,     1,
       1,     1,     5,     2,     4,     3,     4,     5,     7,     4,
       6,     7,     0,     2,     2,     4,     1,     3,     2,     0,
       2,     1,     3,     2,     3,     4,     5,     1,     3,     2,
       4,     1,     3,     2,     1,     3,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     5,     5,     0,     2,     1,     2,     3,     1,     6,
       0,     1,     2,     3,     2,     1,     0,     3,     5,     7,
       3,     5,     7,     3,     5,     7,     3,     5,     7,     1,
       3,     4,     0,     1,     1,     3,     1,     3,     5,     0,
       1,     1,     3,     1,     3,     4,     3,     2,     1,     3,
       0,     2,     1,     3,     2,     4,     3,     3,     2,     1,
       3,     0,     2,     4,     5,     3,     4,     0,     2,     1,
       3,     0,     1,     1,     1,     0,     1,     1,     2,     1,
       2,     1,     2,     1,     1,     2,     2,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

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


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
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

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
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
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
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
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
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
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
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
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
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
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
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
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
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

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* root: classes  */
#line 182 "lang11d"
                                        { 
						(yyval.node) = gRootParseNode = (yyvsp[0].node);
						gParserResult = 1;
					}
#line 2297 "lang11d_tab.cpp"
    break;

  case 3: /* root: classextensions  */
#line 187 "lang11d"
                                        { 
						(yyval.node) = gRootParseNode = (yyvsp[0].node);
						gParserResult = 1;
					}
#line 2306 "lang11d_tab.cpp"
    break;

  case 4: /* root: INTERPRET cmdlinecode  */
#line 192 "lang11d"
                                        { 
						(yyval.node) = gRootParseNode = (yyvsp[0].node);
						gParserResult = 2; 
					}
#line 2315 "lang11d_tab.cpp"
    break;

  case 5: /* classes: %empty  */
#line 197 "lang11d"
                                 { (yyval.node) = nullptr; }
#line 2321 "lang11d_tab.cpp"
    break;

  case 6: /* classes: classes classdef  */
#line 199 "lang11d"
                                        { (yyval.node) = linkNextNode((yyvsp[-1].node), (yyvsp[0].node)); }
#line 2327 "lang11d_tab.cpp"
    break;

  case 8: /* classextensions: classextensions classextension  */
#line 203 "lang11d"
                                        { (yyval.node) = linkNextNode((yyvsp[-1].node), (yyvsp[0].node)); }
#line 2333 "lang11d_tab.cpp"
    break;

  case 9: /* classdef: CLASSNAME optSuperName '{' classvardecls methods '}'  */
#line 206 "lang11d"
                                        { (yyval.node) = allocNode<PyrClassNode>((yyloc), (yyvsp[-5].slotNode), (yyvsp[-4].slotNode), nullptr, (yyvsp[-2].varListNode), (yyvsp[-1].methodNode)); }
#line 2339 "lang11d_tab.cpp"
    break;

  case 10: /* classdef: CLASSNAME '[' optName ']' optSuperName '{' classvardecls methods '}'  */
#line 208 "lang11d"
                                        { (yyval.node) = allocNode<PyrClassNode>((yyloc), (yyvsp[-8].slotNode), (yyvsp[-4].slotNode), (yyvsp[-6].slotNode), (yyvsp[-2].varListNode), (yyvsp[-1].methodNode)); }
#line 2345 "lang11d_tab.cpp"
    break;

  case 11: /* classextension: '+' CLASSNAME '{' methods '}'  */
#line 211 "lang11d"
                                        { (yyval.node) = allocNode<PyrClassExtNode>((yyloc), (yyvsp[-3].slotNode), (yyvsp[-1].methodNode)); }
#line 2351 "lang11d_tab.cpp"
    break;

  case 12: /* optName: %empty  */
#line 213 "lang11d"
                                 { (yyval.slotNode) = nullptr; }
#line 2357 "lang11d_tab.cpp"
    break;

  case 14: /* optSuperName: %empty  */
#line 216 "lang11d"
                         { (yyval.slotNode) = nullptr; }
#line 2363 "lang11d_tab.cpp"
    break;

  case 15: /* optSuperName: ':' CLASSNAME  */
#line 216 "lang11d"
                                                           { (yyval.slotNode) = (yyvsp[0].slotNode); }
#line 2369 "lang11d_tab.cpp"
    break;

  case 16: /* classvardecls: %empty  */
#line 218 "lang11d"
                         { (yyval.varListNode) = nullptr; }
#line 2375 "lang11d_tab.cpp"
    break;

  case 17: /* classvardecls: classvardecls classvardecl  */
#line 220 "lang11d"
                                        { (yyval.varListNode) = linkNextNode((yyvsp[-1].varListNode), (yyvsp[0].varListNode)); }
#line 2381 "lang11d_tab.cpp"
    break;

  case 18: /* classvardecl: CLASSVAR rwslotdeflist ';'  */
#line 223 "lang11d"
                                        { (yyval.varListNode) = allocNode<PyrVarListNode>((yyloc), (yyvsp[-1].varDefNode), varClass); }
#line 2387 "lang11d_tab.cpp"
    break;

  case 19: /* classvardecl: VAR rwslotdeflist ';'  */
#line 225 "lang11d"
                                        { (yyval.varListNode) = allocNode<PyrVarListNode>((yyloc), (yyvsp[-1].varDefNode), varInst); }
#line 2393 "lang11d_tab.cpp"
    break;

  case 20: /* classvardecl: SC_CONST constdeflist ';'  */
#line 227 "lang11d"
                                        { (yyval.varListNode) = allocNode<PyrVarListNode>((yyloc), (yyvsp[-1].varDefNode), varConst); }
#line 2399 "lang11d_tab.cpp"
    break;

  case 21: /* methods: %empty  */
#line 229 "lang11d"
                                 { (yyval.methodNode) = nullptr; }
#line 2405 "lang11d_tab.cpp"
    break;

  case 22: /* methods: methods methoddef  */
#line 231 "lang11d"
                                        { (yyval.methodNode) = linkNextNode((yyvsp[-1].methodNode), (yyvsp[0].methodNode)); }
#line 2411 "lang11d_tab.cpp"
    break;

  case 23: /* methoddef: name '{' argdecls funcvardecls optPrim methbody '}'  */
#line 234 "lang11d"
                                        { (yyval.methodNode) = allocNode<PyrMethodNode>((yyloc), (yyvsp[-6].slotNode), (yyvsp[-2].slotNode), (yyvsp[-4].argListNode), (yyvsp[-3].varListNode), (yyvsp[-1].node), false); }
#line 2417 "lang11d_tab.cpp"
    break;

  case 24: /* methoddef: '*' name '{' argdecls funcvardecls optPrim methbody '}'  */
#line 236 "lang11d"
                                        { (yyval.methodNode) = allocNode<PyrMethodNode>((yyloc), (yyvsp[-6].slotNode), (yyvsp[-2].slotNode), (yyvsp[-4].argListNode), (yyvsp[-3].varListNode), (yyvsp[-1].node), true); }
#line 2423 "lang11d_tab.cpp"
    break;

  case 25: /* methoddef: binop '{' argdecls funcvardecls optPrim methbody '}'  */
#line 238 "lang11d"
                                        { (yyval.methodNode) = allocNode<PyrMethodNode>((yyloc), (yyvsp[-6].slotNode), (yyvsp[-2].slotNode), (yyvsp[-4].argListNode), (yyvsp[-3].varListNode), (yyvsp[-1].node), false); }
#line 2429 "lang11d_tab.cpp"
    break;

  case 26: /* methoddef: '*' binop '{' argdecls funcvardecls optPrim methbody '}'  */
#line 240 "lang11d"
                                        { (yyval.methodNode) = allocNode<PyrMethodNode>((yyloc), (yyvsp[-6].slotNode), (yyvsp[-2].slotNode), (yyvsp[-4].argListNode), (yyvsp[-3].varListNode), (yyvsp[-1].node), true); }
#line 2435 "lang11d_tab.cpp"
    break;

  case 34: /* funcbody: exprseq funretval  */
#line 248 "lang11d"
                                        { (yyval.node) = allocNode<PyrDropNode>((yyloc), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2441 "lang11d_tab.cpp"
    break;

  case 35: /* cmdlinecode: '(' argdecls1 funcvardecls1 funcbody ')'  */
#line 251 "lang11d"
                                        { (yyval.node) = allocNode<PyrBlockNode>((yyloc), (yyvsp[-3].argListNode), (yyvsp[-2].varListNode), (yyvsp[-1].node), false); }
#line 2447 "lang11d_tab.cpp"
    break;

  case 36: /* cmdlinecode: '(' argdecls1 funcbody ')'  */
#line 253 "lang11d"
                                        { (yyval.node) = allocNode<PyrBlockNode>((yyloc), (yyvsp[-2].argListNode), nullptr, (yyvsp[-1].node), false); }
#line 2453 "lang11d_tab.cpp"
    break;

  case 37: /* cmdlinecode: '(' funcvardecls1 funcbody ')'  */
#line 255 "lang11d"
                                        { (yyval.node) = allocNode<PyrBlockNode>((yyloc), nullptr, (yyvsp[-2].varListNode), (yyvsp[-1].node), false); }
#line 2459 "lang11d_tab.cpp"
    break;

  case 38: /* cmdlinecode: argdecls1 funcvardecls1 funcbody  */
#line 257 "lang11d"
                                        { (yyval.node) = allocNode<PyrBlockNode>((yyloc), (yyvsp[-2].argListNode), (yyvsp[-1].varListNode), (yyvsp[0].node), false); }
#line 2465 "lang11d_tab.cpp"
    break;

  case 39: /* cmdlinecode: argdecls1 funcbody  */
#line 259 "lang11d"
                                        { (yyval.node) = allocNode<PyrBlockNode>((yyloc), (yyvsp[-1].argListNode), nullptr, (yyvsp[0].node), false); }
#line 2471 "lang11d_tab.cpp"
    break;

  case 40: /* cmdlinecode: funcvardecls1 funcbody  */
#line 261 "lang11d"
                                        { (yyval.node) = allocNode<PyrBlockNode>((yyloc), nullptr, (yyvsp[-1].varListNode), (yyvsp[0].node), false); }
#line 2477 "lang11d_tab.cpp"
    break;

  case 41: /* cmdlinecode: funcbody  */
#line 263 "lang11d"
                                        { (yyval.node) = allocNode<PyrBlockNode>((yyloc), nullptr, nullptr, (yyvsp[0].node), false); }
#line 2483 "lang11d_tab.cpp"
    break;

  case 43: /* methbody: exprseq retval  */
#line 267 "lang11d"
                                        { (yyval.node) = allocNode<PyrDropNode>((yyloc), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2489 "lang11d_tab.cpp"
    break;

  case 44: /* optPrim: %empty  */
#line 269 "lang11d"
                                { (yyval.slotNode) = nullptr; }
#line 2495 "lang11d_tab.cpp"
    break;

  case 45: /* optPrim: PRIMITIVENAME optsemi  */
#line 270 "lang11d"
                                                        { (yyval.slotNode) = (yyvsp[-1].slotNode); }
#line 2501 "lang11d_tab.cpp"
    break;

  case 46: /* retval: %empty  */
#line 272 "lang11d"
                                 { (yyval.node) = allocNode<PyrReturnNode>((yyloc), nullptr); }
#line 2507 "lang11d_tab.cpp"
    break;

  case 47: /* retval: '^' expr optsemi  */
#line 274 "lang11d"
                                        { (yyval.node) = allocNode<PyrReturnNode>((yyloc), (yyvsp[-1].node)); }
#line 2513 "lang11d_tab.cpp"
    break;

  case 48: /* funretval: %empty  */
#line 277 "lang11d"
                                        { (yyval.node) = allocNode<PyrBlockReturnNode>((yyloc), nullptr); }
#line 2519 "lang11d_tab.cpp"
    break;

  case 49: /* funretval: '^' expr optsemi  */
#line 279 "lang11d"
                                        { (yyval.node) = allocNode<PyrReturnNode>((yyloc), (yyvsp[-1].node)); }
#line 2525 "lang11d_tab.cpp"
    break;

  case 51: /* blocklist1: blocklist1 blocklistitem  */
#line 283 "lang11d"
                                        { (yyval.node) = linkNextNode((yyvsp[-1].node), (yyvsp[0].node)); }
#line 2531 "lang11d_tab.cpp"
    break;

  case 54: /* blocklist: %empty  */
#line 287 "lang11d"
                                 { (yyval.node) = nullptr; }
#line 2537 "lang11d_tab.cpp"
    break;

  case 56: /* msgsend: name blocklist1  */
#line 290 "lang11d"
                                        { (yyval.node) = allocNode<PyrCallNode>((yyloc), (yyvsp[-1].slotNode), (yyvsp[0].node), nullptr); }
#line 2543 "lang11d_tab.cpp"
    break;

  case 57: /* msgsend: '(' binop2 ')' blocklist1  */
#line 293 "lang11d"
                                        { (yyval.node) = allocNode<PyrCallNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[0].node), nullptr); }
#line 2549 "lang11d_tab.cpp"
    break;

  case 58: /* msgsend: name '(' ')' blocklist1  */
#line 296 "lang11d"
                                        { (yyval.node) = allocNode<PyrCallNode>((yyloc), (yyvsp[-3].slotNode), (yyvsp[0].node), nullptr); }
#line 2555 "lang11d_tab.cpp"
    break;

  case 59: /* msgsend: name '(' arglist1 optkeyarglist ')' blocklist  */
#line 299 "lang11d"
                                        { (yyval.node) = allocNode<PyrCallNode>((yyloc), (yyvsp[-5].slotNode), linkNextNode((yyvsp[-3].node), (yyvsp[0].node)), (yyvsp[-2].node)); }
#line 2561 "lang11d_tab.cpp"
    break;

  case 60: /* msgsend: '(' binop2 ')' '(' ')' blocklist1  */
#line 302 "lang11d"
                                        { (yyval.node) = allocNode<PyrCallNode>((yyloc), (yyvsp[-4].slotNode), (yyvsp[0].node), nullptr); }
#line 2567 "lang11d_tab.cpp"
    break;

  case 61: /* msgsend: '(' binop2 ')' '(' arglist1 optkeyarglist ')' blocklist  */
#line 305 "lang11d"
                                        { (yyval.node) = allocNode<PyrCallNode>((yyloc), (yyvsp[-6].slotNode), linkNextNode((yyvsp[-3].node), (yyvsp[0].node)), (yyvsp[-2].node)); }
#line 2573 "lang11d_tab.cpp"
    break;

  case 62: /* msgsend: name '(' arglistv1 optkeyarglist ')'  */
#line 308 "lang11d"
                                        {
						PyrSlot slot;
						if (isSuperObjNode((yyvsp[-2].node))) {
							(yyvsp[-2].node)->assertCast<PyrSlotNode>()->mSlot = PyrSlot::make(s_this);
							slot = PyrSlot::make(s_superPerformList);
						} else {
							slot = PyrSlot::make(s_performList);
						}
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), slot);
						auto* args = linkAfterHead((yyvsp[-2].node), (yyvsp[-4].slotNode)->changeLiteralType(pn_PushLitNode));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, (yyvsp[-1].node));
					}
#line 2590 "lang11d_tab.cpp"
    break;

  case 63: /* msgsend: '(' binop2 ')' '(' arglistv1 optkeyarglist ')'  */
#line 321 "lang11d"
                                        {
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_performList));
						auto* args = linkAfterHead((yyvsp[-2].node), (yyvsp[-5].slotNode)->changeLiteralType(pn_PushLitNode));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, (yyvsp[-1].node));
					}
#line 2600 "lang11d_tab.cpp"
    break;

  case 64: /* msgsend: CLASSNAME '[' arrayelems ']'  */
#line 327 "lang11d"
                                        { (yyval.node) = allocNode<PyrDynListNode>((yyloc), (yyvsp[-3].slotNode), (yyvsp[-1].node)); }
#line 2606 "lang11d_tab.cpp"
    break;

  case 65: /* msgsend: CLASSNAME blocklist1  */
#line 330 "lang11d"
                                        {
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_new));
						auto* args = linkNextNode((yyvsp[-1].slotNode)->changeLiteralType(pn_PushNameNode), (yyvsp[0].node));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 2616 "lang11d_tab.cpp"
    break;

  case 66: /* msgsend: CLASSNAME '(' ')' blocklist  */
#line 336 "lang11d"
                                        {
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_new));
						auto* args = linkNextNode((yyvsp[-3].slotNode)->changeLiteralType(pn_PushNameNode), (yyvsp[0].node));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 2626 "lang11d_tab.cpp"
    break;

  case 67: /* msgsend: CLASSNAME '(' keyarglist1 optcomma ')' blocklist  */
#line 342 "lang11d"
                                        {
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_new));
						auto* args = linkNextNode((yyvsp[-5].slotNode)->changeLiteralType(pn_PushNameNode), (yyvsp[0].node));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, (yyvsp[-3].node));
					}
#line 2636 "lang11d_tab.cpp"
    break;

  case 68: /* msgsend: CLASSNAME '(' arglist1 optkeyarglist ')' blocklist  */
#line 348 "lang11d"
                                        {
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_new));
						auto* args = linkAllNodes(
							(yyvsp[-5].slotNode)->changeLiteralType(pn_PushNameNode), 
							(yyvsp[-3].node),
							(yyvsp[0].node));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, (yyvsp[-2].node));
					}
#line 2649 "lang11d_tab.cpp"
    break;

  case 69: /* msgsend: CLASSNAME '(' arglistv1 optkeyarglist ')'  */
#line 357 "lang11d"
                                        {
						PyrSlot slot;
						if (isSuperObjNode((yyvsp[-4].slotNode))) {
							(yyvsp[-4].slotNode)->assertCast<PyrSlotNode>()->mSlot = PyrSlot::make(s_this);
							slot = PyrSlot::make(s_superPerformList);
						} else {
							slot = PyrSlot::make(s_performList);
						}

						auto* new_selector_push_lit = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_new), pn_PushLitNode);
						auto* args = linkAllNodes(
							(yyvsp[-4].slotNode)->changeLiteralType(pn_PushNameNode),
							new_selector_push_lit,
							(yyvsp[-2].node));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), slot);
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, (yyvsp[-1].node));
					}
#line 2671 "lang11d_tab.cpp"
    break;

  case 70: /* msgsend: expr '.' '(' ')' blocklist  */
#line 375 "lang11d"
                                        {
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_value));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, linkNextNode((yyvsp[-4].node), (yyvsp[0].node)), nullptr);
					}
#line 2680 "lang11d_tab.cpp"
    break;

  case 71: /* msgsend: expr '.' '(' keyarglist1 optcomma ')' blocklist  */
#line 380 "lang11d"
                                        {
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_value));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, linkNextNode((yyvsp[-6].node), (yyvsp[0].node)), (yyvsp[-3].node));
					}
#line 2689 "lang11d_tab.cpp"
    break;

  case 72: /* msgsend: expr '.' name '(' keyarglist1 optcomma ')' blocklist  */
#line 385 "lang11d"
                                        { (yyval.node) = allocNode<PyrCallNode>((yyloc), (yyvsp[-5].slotNode), linkNextNode((yyvsp[-7].node), (yyvsp[0].node)), (yyvsp[-3].node)); }
#line 2695 "lang11d_tab.cpp"
    break;

  case 73: /* msgsend: expr '.' '(' arglist1 optkeyarglist ')' blocklist  */
#line 387 "lang11d"
                                        {
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_value));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, linkAllNodes((yyvsp[-6].node), (yyvsp[-3].node), (yyvsp[0].node)), (yyvsp[-2].node));
					}
#line 2704 "lang11d_tab.cpp"
    break;

  case 74: /* msgsend: expr '.' '(' arglistv1 optkeyarglist ')'  */
#line 392 "lang11d"
                                        {
						PyrSlot selectorSlot;
						if (isSuperObjNode((yyvsp[-5].node))) {
							(yyvsp[-5].node)->assertCast<PyrSlotNode>()->mSlot = PyrSlot::make(s_this);
							selectorSlot = PyrSlot::make(s_superPerformList);
						} else {
							selectorSlot = PyrSlot::make(s_performList);
						}
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), selectorSlot);
						auto* args = linkAllNodes(
							(yyvsp[-5].node), 
							allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_value), pn_PushLitNode),
							(yyvsp[-2].node));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, (yyvsp[-1].node));
					}
#line 2724 "lang11d_tab.cpp"
    break;

  case 75: /* msgsend: expr '.' name '(' ')' blocklist  */
#line 408 "lang11d"
                                        { (yyval.node) = allocNode<PyrCallNode>((yyloc), (yyvsp[-3].slotNode), linkNextNode((yyvsp[-5].node), (yyvsp[0].node)), nullptr); }
#line 2730 "lang11d_tab.cpp"
    break;

  case 76: /* msgsend: expr '.' name '(' arglist1 optkeyarglist ')' blocklist  */
#line 410 "lang11d"
                                        { (yyval.node) = allocNode<PyrCallNode>((yyloc), (yyvsp[-5].slotNode), linkAllNodes((yyvsp[-7].node), (yyvsp[-3].node), (yyvsp[0].node)), (yyvsp[-2].node)); }
#line 2736 "lang11d_tab.cpp"
    break;

  case 77: /* msgsend: expr '.' name '(' arglistv1 optkeyarglist ')'  */
#line 412 "lang11d"
                                        {
						PyrSlot slot;
						if (isSuperObjNode((yyvsp[-6].node))) {
							(yyvsp[-6].node)->assertCast<PyrSlotNode>()->mSlot = PyrSlot::make(s_this);
							slot = PyrSlot::make(s_superPerformList);
						} else {
							slot = PyrSlot::make(s_performList);
						}
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), slot);
						auto* args = linkAllNodes((yyvsp[-6].node), (yyvsp[-4].slotNode)->changeLiteralType(pn_PushLitNode), (yyvsp[-2].node));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, (yyvsp[-1].node));
					}
#line 2753 "lang11d_tab.cpp"
    break;

  case 78: /* msgsend: expr '.' name blocklist  */
#line 425 "lang11d"
                                        { (yyval.node) = allocNode<PyrCallNode>((yyloc), (yyvsp[-1].slotNode), linkNextNode((yyvsp[-3].node), (yyvsp[0].node)), nullptr); }
#line 2759 "lang11d_tab.cpp"
    break;

  case 79: /* $@1: %empty  */
#line 433 "lang11d"
                                          { pushls(&generatorStack, (intptr_t)(yyvsp[0].node)); pushls(&generatorStack, 1); }
#line 2765 "lang11d_tab.cpp"
    break;

  case 80: /* generator: '{' ':' exprseq $@1 ',' qual '}'  */
#line 434 "lang11d"
                                        {
						PyrSlotNode* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("r")));
						PyrParseNode *block = allocNode<PyrBlockNode>((yyloc), nullptr, nullptr, (yyvsp[-1].node), false);
						PyrParseNode *blocklit = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>(block), pn_PushLitNode);
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, blocklit, nullptr);
					}
#line 2776 "lang11d_tab.cpp"
    break;

  case 81: /* $@2: %empty  */
#line 440 "lang11d"
                                                  { pushls(&generatorStack, (intptr_t)(yyvsp[0].node)); pushls(&generatorStack, 2); }
#line 2782 "lang11d_tab.cpp"
    break;

  case 82: /* generator: '{' ';' exprseq $@2 ',' qual '}'  */
#line 441 "lang11d"
                                        { (yyval.node) = (yyvsp[-1].node); }
#line 2788 "lang11d_tab.cpp"
    break;

  case 83: /* nextqual: %empty  */
#line 444 "lang11d"
                                        {
						// innermost part
						const int action = popls(&generatorStack);
						PyrParseNode* expr = (PyrParseNode*)popls(&generatorStack);
						switch (action) {
							case 1 : 
								(yyval.node) = allocNode<PyrCallNode>((yyloc), allocNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("yield"))), expr, nullptr);
								break;
							case 2 : 
								(yyval.node) = expr; 
								break;
						}
					}
#line 2806 "lang11d_tab.cpp"
    break;

  case 84: /* nextqual: ',' qual  */
#line 457 "lang11d"
                                           { (yyval.node) = (yyvsp[0].node); }
#line 2812 "lang11d_tab.cpp"
    break;

  case 85: /* qual: name LEFTARROW exprseq nextqual  */
#line 460 "lang11d"
                                        {
						// later should check if exprseq is a series and optimize it to for loop
						PyrParseNode *exprseq = (yyvsp[-1].node);
						if (exprseq->mClassno == pn_CallNode) {
							PyrCallNode *callnode = exprseq->assertCast<PyrCallNode>();
							if (slotRawSymbol(&callnode->mSelector->mSlot) == s_series) {
								SetSymbol(&callnode->mSelector->mSlot, getsym("forSeries"));

								auto* var = allocNode<PyrVarDefNode>((yyloc), (yyvsp[-3].slotNode), nullptr, ReadWriteAccessor::Private);
								auto* args = allocNode<PyrArgListNode>((yyloc), var, nullptr, nullptr);
								auto *block = allocNode<PyrBlockNode>((yyloc), args, nullptr, (yyvsp[0].node), false);
								auto *blocklit = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>(block), pn_PushLitNode);

								callnode->mArglist = linkNextNode(callnode->mArglist, blocklit);
								(yyval.node) = callnode;

							} else goto notoptimized1;
						} else {
							notoptimized1:
							PyrSlot slot;
							SetSymbol(&slot, getsym("do"));
							auto* selectornode = allocNode<PyrSlotNode>((yyloc), slot);

							auto* var = allocNode<PyrVarDefNode>((yyloc), (yyvsp[-3].slotNode), nullptr, ReadWriteAccessor::Private);
							auto* args = allocNode<PyrArgListNode>((yyloc), var, nullptr, nullptr);
							auto *block = allocNode<PyrBlockNode>((yyloc), args, nullptr, (yyvsp[0].node), false);
							auto *blocklit = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>(block), pn_PushLitNode);

							PyrParseNode* args2 = linkNextNode(exprseq, blocklit);
							(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args2, nullptr);
						}
					}
#line 2849 "lang11d_tab.cpp"
    break;

  case 86: /* qual: name name LEFTARROW exprseq nextqual  */
#line 493 "lang11d"
                                        {
						// later should check if exprseq is a series and optimize it to for loop
						PyrParseNode *exprseq = (yyvsp[-1].node);
						if (exprseq->mClassno == pn_CallNode) {
							PyrCallNode *callnode = exprseq->assertCast<PyrCallNode>();
							if (slotRawSymbol(&callnode->mSelector->mSlot) == s_series) {
								SetSymbol(&callnode->mSelector->mSlot, getsym("forSeries"));

								auto* var1 = allocNode<PyrVarDefNode>((yyloc), (yyvsp[-4].slotNode), nullptr, ReadWriteAccessor::Private);
								auto* var2 = allocNode<PyrVarDefNode>((yyloc), (yyvsp[-3].slotNode), nullptr, ReadWriteAccessor::Private);
								auto* vars = linkNextNode(var1, var2);
								auto* args = allocNode<PyrArgListNode>((yyloc), vars, nullptr, nullptr);
								auto *block = allocNode<PyrBlockNode>((yyloc), args, nullptr, (yyvsp[0].node), false);
								auto *blocklit = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>(block), pn_PushLitNode);

								callnode->mArglist = linkNextNode(callnode->mArglist, blocklit);
								(yyval.node) = callnode;

							} else goto notoptimized2;
						} else {
							notoptimized2:
							PyrSlot slot;
							SetSymbol(&slot, getsym("do"));
							PyrSlotNode* selectornode = allocNode<PyrSlotNode>((yyloc), slot);

							auto* var1 = allocNode<PyrVarDefNode>((yyloc), (yyvsp[-4].slotNode), nullptr, ReadWriteAccessor::Private);
							auto* var2 = allocNode<PyrVarDefNode>((yyloc), (yyvsp[-3].slotNode), nullptr, ReadWriteAccessor::Private);
							auto* vars = linkNextNode(var1, var2);
							auto* args = allocNode<PyrArgListNode>((yyloc), vars, nullptr, nullptr);
							auto *block = allocNode<PyrBlockNode>((yyloc), args, nullptr, (yyvsp[0].node), false);
							auto *blocklit = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>(block), pn_PushLitNode);

							PyrParseNode* args2 = linkNextNode(exprseq, blocklit);
							(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args2, nullptr);
						}
					}
#line 2890 "lang11d_tab.cpp"
    break;

  case 87: /* qual: VAR name '=' exprseq nextqual  */
#line 530 "lang11d"
                                        {
						PyrSlot slot = PyrSlot::make(s_value);
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), slot);
						auto* var = allocNode<PyrVarDefNode>((yyloc), (yyvsp[-3].slotNode), nullptr, ReadWriteAccessor::Private);
						auto* args = allocNode<PyrArgListNode>((yyloc), var, nullptr, nullptr);
						auto *block = allocNode<PyrBlockNode>((yyloc), args, nullptr, (yyvsp[0].node), false);
						auto *blocklit = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>(block), pn_PushLitNode);
						auto* args2 = linkNextNode(blocklit, (yyvsp[-1].node));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args2, nullptr);
					}
#line 2905 "lang11d_tab.cpp"
    break;

  case 88: /* qual: exprseq nextqual  */
#line 541 "lang11d"
                                        {
						PyrSlotNode* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("if")));
						PyrParseNode* block = allocNode<PyrBlockNode>((yyloc), nullptr, nullptr, (yyvsp[0].node), false);
						PyrParseNode* blocklit = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>(block), pn_PushLitNode);
						PyrParseNode* args2 = linkNextNode((yyvsp[-1].node), blocklit);
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args2, nullptr);
					}
#line 2917 "lang11d_tab.cpp"
    break;

  case 89: /* qual: ':' ':' exprseq nextqual  */
#line 549 "lang11d"
                                        { (yyval.node) = allocNode<PyrDropNode>((yyloc), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2923 "lang11d_tab.cpp"
    break;

  case 90: /* qual: ':' WHILE exprseq nextqual  */
#line 551 "lang11d"
                                        {
						PyrSlotNode* selectornode1 = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("alwaysYield")));
						PyrParseNode* pushnil = allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode);
						PyrParseNode* yieldNil = allocNode<PyrCallNode>((yyloc), selectornode1, pushnil, nullptr);
						PyrParseNode* block1 = allocNode<PyrBlockNode>((yyloc), nullptr, nullptr, yieldNil, false);
						PyrParseNode* blocklit1 = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>(block1), pn_PushLitNode);
						PyrParseNode* block2 = allocNode<PyrBlockNode>((yyloc), nullptr, nullptr, (yyvsp[0].node), false);
						PyrParseNode* blocklit2 = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>(block2), pn_PushLitNode);
						PyrParseNode* args2 = linkNextNode((yyvsp[-1].node), blocklit2);
						PyrParseNode* args3 = linkNextNode(args2, blocklit1);

						PyrSlotNode* selectornode2 = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("if")));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode2, args3, nullptr);
					}
#line 2942 "lang11d_tab.cpp"
    break;

  case 98: /* expr1: '(' exprseq ')'  */
#line 575 "lang11d"
                                        {
						PyrParseNode* node = (yyvsp[-1].node);
						node->mParens = 1;
						node->mLocation = (yyloc); // make the location include the brackets.
						(yyval.node) = (yyvsp[-1].node);
					}
#line 2953 "lang11d_tab.cpp"
    break;

  case 99: /* expr1: '~' name  */
#line 582 "lang11d"
                                        {
						auto* argnode = (yyvsp[0].slotNode)->changeLiteralType(pn_PushLitNode);
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_envirGet));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, argnode, nullptr);
					}
#line 2963 "lang11d_tab.cpp"
    break;

  case 100: /* expr1: '[' arrayelems ']'  */
#line 588 "lang11d"
                                        { (yyval.node) = allocNode<PyrDynListNode>((yyloc), nullptr, (yyvsp[-1].node)); }
#line 2969 "lang11d_tab.cpp"
    break;

  case 101: /* expr1: '(' valrange2 ')'  */
#line 590 "lang11d"
                                        { (yyval.node) = (yyvsp[-1].node); }
#line 2975 "lang11d_tab.cpp"
    break;

  case 102: /* expr1: '(' ':' valrange3 ')'  */
#line 592 "lang11d"
                                        { (yyval.node) = (yyvsp[-1].node); }
#line 2981 "lang11d_tab.cpp"
    break;

  case 103: /* expr1: '(' dictslotlist ')'  */
#line 594 "lang11d"
                                        { (yyval.node) = allocNode<PyrDynDictNode>((yyloc), (yyvsp[-1].node)); }
#line 2987 "lang11d_tab.cpp"
    break;

  case 104: /* expr1: expr1 '[' arglist1 ']'  */
#line 596 "lang11d"
                                        {
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_at));
						auto* args = linkNextNode((yyvsp[-3].node), (yyvsp[-1].node));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 2997 "lang11d_tab.cpp"
    break;

  case 106: /* valrangex1: expr1 '[' arglist1 DOTDOT ']'  */
#line 604 "lang11d"
                                        {
						const int arglen = nodeListLength((yyvsp[-2].node));
						if (arglen > 2) {
							error("ArrayedCollection subrange has too many arguments.\n");
							nodePostErrorLine((yyvsp[-2].node));
							compileErrors++;
						}
						auto* args = linkNextNode((yyvsp[-4].node), (yyvsp[-2].node));
						if (arglen < 2) 
							args = linkNextNode(args, allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode));

						args = linkNextNode(args, allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_copyseries));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3017 "lang11d_tab.cpp"
    break;

  case 107: /* valrangex1: expr1 '[' DOTDOT exprseq ']'  */
#line 620 "lang11d"
                                        {
						auto* nilnode1 = allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode);
						auto* args = linkNextNode((yyvsp[-4].node), nilnode1);
						auto* nilnode2 = allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode);
						args = linkNextNode(args, nilnode2);
						args = linkNextNode(args, (yyvsp[-1].node));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_copyseries));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3031 "lang11d_tab.cpp"
    break;

  case 108: /* valrangex1: expr1 '[' arglist1 DOTDOT exprseq ']'  */
#line 630 "lang11d"
                                        {
						const int arglen = nodeListLength((yyvsp[-3].node));
						if (arglen > 2) {
							error("ArrayedCollection subrange has too many arguments.\n");
							nodePostErrorLine((yyvsp[-3].node));
							compileErrors++;
						}

						auto* args = linkNextNode((yyvsp[-5].node), (yyvsp[-3].node));
						if (arglen < 2) 
							args = linkNextNode(args, allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode));
						
						args = linkNextNode(args, (yyvsp[-1].node));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_copyseries));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3052 "lang11d_tab.cpp"
    break;

  case 109: /* valrangeassign: expr1 '[' arglist1 DOTDOT ']' '=' expr  */
#line 648 "lang11d"
                                        {
						const int arglen = nodeListLength((yyvsp[-4].node));
						if (arglen > 2) {
							error("ArrayedCollection subrange has too many arguments.\n");
							nodePostErrorLine((yyvsp[-4].node));
							compileErrors++;
						}

						auto* args = linkNextNode((yyvsp[-6].node), (yyvsp[-4].node));
						if (arglen < 2) 
							args = linkNextNode(args, allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode));
						
						args = linkAllNodes(args, allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode), (yyvsp[0].node));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_putseries));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3073 "lang11d_tab.cpp"
    break;

  case 110: /* valrangeassign: expr1 '[' DOTDOT exprseq ']' '=' expr  */
#line 665 "lang11d"
                                        {
						auto* args = linkAllNodes(
							(yyvsp[-6].node),
							allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode),
							allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode),
							(yyvsp[-3].node), 
							(yyvsp[0].node));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_putseries));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3088 "lang11d_tab.cpp"
    break;

  case 111: /* valrangeassign: expr1 '[' arglist1 DOTDOT exprseq ']' '=' expr  */
#line 676 "lang11d"
                                        {
						const int arglen = nodeListLength((yyvsp[-5].node));
						if (arglen > 2) {
							error("ArrayedCollection subrange has too many arguments.\n");
							nodePostErrorLine((yyvsp[-5].node));
							compileErrors++;
						}

						auto* args = linkNextNode((yyvsp[-7].node), (yyvsp[-5].node));
						if (arglen < 2) 
							args = linkNextNode(args, allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode));
						
						args = linkAllNodes(args, (yyvsp[-3].node), (yyvsp[0].node));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_putseries));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3109 "lang11d_tab.cpp"
    break;

  case 112: /* valrangexd: expr '.' '[' arglist1 DOTDOT ']'  */
#line 699 "lang11d"
                                        {
						PyrSlotNode* nilnode1, *nilnode2;
						PyrSlot selectorSlot, nilSlot;
						PyrParseNode* args;

						const int arglen = nodeListLength((yyvsp[-2].node));
						if (arglen > 2) {
							error("ArrayedCollection subrange has too many arguments.\n");
							nodePostErrorLine((yyvsp[-2].node));
							compileErrors++;
						}

						args = linkNextNode((yyvsp[-5].node), (yyvsp[-2].node));
						if (arglen < 2) 
							args = linkNextNode(args, allocNode<PyrSlotNode>((yyloc), nilSlot, pn_PushLitNode));

						args = linkNextNode(args, allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode));

						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_copyseries));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3135 "lang11d_tab.cpp"
    break;

  case 113: /* valrangexd: expr '.' '[' DOTDOT exprseq ']'  */
#line 721 "lang11d"
                                        {
						auto* args = linkAllNodes(
							(yyvsp[-5].node),
							allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode),
							allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode),
							(yyvsp[-1].node));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_copyseries));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3149 "lang11d_tab.cpp"
    break;

  case 114: /* valrangexd: expr '.' '[' arglist1 DOTDOT exprseq ']'  */
#line 731 "lang11d"
                                        {
						const int arglen = nodeListLength((yyvsp[-3].node));
						if (arglen > 2) {
							error("ArrayedCollection subrange has too many arguments.\n");
							nodePostErrorLine((yyvsp[-3].node));
							compileErrors++;
						}

						auto* args = linkNextNode((yyvsp[-6].node), (yyvsp[-3].node));
						if (arglen < 2) 
							args = linkNextNode(args, allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode));
						
						args = linkNextNode(args, (yyvsp[-1].node));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_copyseries));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3170 "lang11d_tab.cpp"
    break;

  case 115: /* valrangexd: expr '.' '[' arglist1 DOTDOT ']' '=' expr  */
#line 748 "lang11d"
                                        {
						const int arglen = nodeListLength((yyvsp[-4].node));
						if (arglen > 2) {
							error("ArrayedCollection subrange has too many arguments.\n");
							nodePostErrorLine((yyvsp[-4].node));
							compileErrors++;
						}

						auto* args = linkNextNode((yyvsp[-7].node), (yyvsp[-4].node));
						if (arglen < 2) 
							args = linkNextNode(args, allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode));
						
						args = linkAllNodes(args, allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode), (yyvsp[0].node));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_putseries));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3191 "lang11d_tab.cpp"
    break;

  case 116: /* valrangexd: expr '.' '[' DOTDOT exprseq ']' '=' expr  */
#line 765 "lang11d"
                                        {
						auto* args = linkNextNode((yyvsp[-7].node), allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode));
						args = linkNextNode(args, allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode));
						args = linkNextNode(args, (yyvsp[-3].node));
						args = linkNextNode(args, (yyvsp[0].node));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_putseries));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3204 "lang11d_tab.cpp"
    break;

  case 117: /* valrangexd: expr '.' '[' arglist1 DOTDOT exprseq ']' '=' expr  */
#line 774 "lang11d"
                                        {
						const int arglen = nodeListLength((yyvsp[-5].node));
						if (arglen > 2) {
							error("ArrayedCollection subrange has too many arguments.\n");
							nodePostErrorLine((yyvsp[-5].node));
							compileErrors++;
						}

						auto* args = linkNextNode((yyvsp[-8].node), (yyvsp[-5].node));
						if (arglen < 2) 
							args = linkNextNode(args, allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode));
						
						args = linkAllNodes(args, (yyvsp[-3].node), (yyvsp[0].node));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_putseries));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3225 "lang11d_tab.cpp"
    break;

  case 118: /* valrange2: exprseq DOTDOT  */
#line 792 "lang11d"
                                        {
						// if this is not used in a 'do' or list comprehension, then should return an error.
						auto* args = linkNextNode((yyvsp[-1].node), allocNode<PyrSlotNode>((yylsp[0]), PyrSlot{}, pn_PushLitNode));
						args = linkNextNode(args, allocNode<PyrSlotNode>((yylsp[0]), PyrSlot{}, pn_PushLitNode));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_series));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3237 "lang11d_tab.cpp"
    break;

  case 119: /* valrange2: DOTDOT exprseq  */
#line 800 "lang11d"
                                        {
						auto* args = linkAllNodes(
							allocNode<PyrSlotNode>((yylsp[-1]), PyrSlot::make<int>(0), pn_PushLitNode),
							allocNode<PyrSlotNode>((yylsp[-1]), PyrSlot{}, pn_PushLitNode),
							(yyvsp[0].node)
						);
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_series));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3251 "lang11d_tab.cpp"
    break;

  case 120: /* valrange2: exprseq DOTDOT exprseq  */
#line 810 "lang11d"
                                        {
						auto* args = linkAllNodes((yyvsp[-2].node), allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode), (yyvsp[0].node));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_series));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3261 "lang11d_tab.cpp"
    break;

  case 121: /* valrange2: exprseq ',' exprseq DOTDOT exprseq  */
#line 816 "lang11d"
                                        {
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_series));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, linkAllNodes((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)), nullptr);
					}
#line 3270 "lang11d_tab.cpp"
    break;

  case 122: /* valrange2: exprseq ',' exprseq DOTDOT  */
#line 821 "lang11d"
                                        {
						// if this is not used in a 'do' or list comprehension, then should return an error.
						auto* args = linkAllNodes((yyvsp[-3].node), (yyvsp[-1].node), allocNode<PyrSlotNode>((yyloc), PyrSlot{}, pn_PushLitNode));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_series));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3281 "lang11d_tab.cpp"
    break;

  case 123: /* valrange3: DOTDOT exprseq  */
#line 829 "lang11d"
                                        {
						auto* args = linkAllNodes(
							allocNode<PyrSlotNode>((yylsp[-1]), PyrSlot::make<int>(0), pn_PushLitNode),
							allocNode<PyrSlotNode>((yylsp[-1]), PyrSlot{}, pn_PushLitNode),
							(yyvsp[0].node)
						);
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("seriesIter")));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3295 "lang11d_tab.cpp"
    break;

  case 124: /* valrange3: exprseq DOTDOT  */
#line 839 "lang11d"
                                        {
						auto* args = linkAllNodes(
							(yyvsp[-1].node),
							allocNode<PyrSlotNode>((yylsp[-1]), PyrSlot{}, pn_PushLitNode),
							allocNode<PyrSlotNode>((yylsp[-1]), PyrSlot{}, pn_PushLitNode)
						);
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("seriesIter")));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3309 "lang11d_tab.cpp"
    break;

  case 125: /* valrange3: exprseq DOTDOT exprseq  */
#line 850 "lang11d"
                                        {
						auto* args = linkAllNodes(
							(yyvsp[-2].node),
							allocNode<PyrSlotNode>((yylsp[-1]), PyrSlot{}, pn_PushLitNode),
							(yyvsp[0].node)
						);
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("seriesIter")));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3323 "lang11d_tab.cpp"
    break;

  case 126: /* valrange3: exprseq ',' exprseq DOTDOT  */
#line 860 "lang11d"
                                        {
						auto* args = linkAllNodes(
							(yyvsp[-3].node),
							(yyvsp[-1].node),
							allocNode<PyrSlotNode>((yylsp[-2]), PyrSlot{}, pn_PushLitNode)
						);
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("seriesIter")));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3337 "lang11d_tab.cpp"
    break;

  case 127: /* valrange3: exprseq ',' exprseq DOTDOT exprseq  */
#line 870 "lang11d"
                                        {
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("seriesIter")));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, linkAllNodes((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)), nullptr);
					}
#line 3346 "lang11d_tab.cpp"
    break;

  case 131: /* expr: CLASSNAME  */
#line 878 "lang11d"
                                            { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_PushNameNode); }
#line 3352 "lang11d_tab.cpp"
    break;

  case 132: /* expr: expr '.' '[' arglist1 ']'  */
#line 880 "lang11d"
                                        {
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_at));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, linkNextNode((yyvsp[-4].node), (yyvsp[-1].node)), nullptr);
					}
#line 3361 "lang11d_tab.cpp"
    break;

  case 133: /* expr: '`' expr  */
#line 885 "lang11d"
                                        {
						auto* args = linkNextNode(
							allocNode<PyrSlotNode>((yylsp[-1]), PyrSlot::make(s_ref), pn_PushNameNode),
							(yyvsp[0].node)
						);
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_new));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3374 "lang11d_tab.cpp"
    break;

  case 134: /* expr: expr binop2 adverb expr  */
#line 894 "lang11d"
                                        {
						(yyvsp[-3].node)->mNext = (yyvsp[0].node);
						(yyvsp[0].node)->mNext = (yyvsp[-1].node);
						(yyval.node) = allocNode<PyrBinopCallNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[-3].node));
					}
#line 3384 "lang11d_tab.cpp"
    break;

  case 135: /* expr: name '=' expr  */
#line 900 "lang11d"
                                        { (yyval.node) = allocNode<PyrAssignNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[0].node)); }
#line 3390 "lang11d_tab.cpp"
    break;

  case 136: /* expr: '~' name '=' expr  */
#line 902 "lang11d"
                                        {
						auto* args = linkNextNode((yyvsp[-2].slotNode)->changeLiteralType(pn_PushLitNode), (yyvsp[0].node));
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_envirPut));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3400 "lang11d_tab.cpp"
    break;

  case 137: /* expr: expr '.' name '=' expr  */
#line 908 "lang11d"
                                        { (yyval.node) = allocNode<PyrSetterNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[-4].node), (yyvsp[0].node)); }
#line 3406 "lang11d_tab.cpp"
    break;

  case 138: /* expr: name '(' arglist1 optkeyarglist ')' '=' expr  */
#line 910 "lang11d"
                                        {
						if ((yyvsp[-3].node) != nullptr) {
							error("Setter method called with keyword arguments.\n");
							nodePostErrorLine((yyvsp[-3].node));
							compileErrors++;
						}
						(yyval.node) = allocNode<PyrSetterNode>((yyloc), (yyvsp[-6].slotNode), (yyvsp[-4].node), (yyvsp[0].node));
					}
#line 3419 "lang11d_tab.cpp"
    break;

  case 139: /* expr: '#' mavars '=' expr  */
#line 919 "lang11d"
                                        { (yyval.node) = allocNode<PyrMultiAssignNode>((yyloc), (yyvsp[-2].multiAssignListNode), (yyvsp[0].node)); }
#line 3425 "lang11d_tab.cpp"
    break;

  case 140: /* expr: expr1 '[' arglist1 ']' '=' expr  */
#line 921 "lang11d"
                                        {
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_put));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, linkAllNodes((yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[0].node)), nullptr);
					}
#line 3434 "lang11d_tab.cpp"
    break;

  case 141: /* expr: expr '.' '[' arglist1 ']' '=' expr  */
#line 926 "lang11d"
                                        {
						auto* selectornode = allocNode<PyrSlotNode>((yyloc), PyrSlot::make(s_put));
						(yyval.node) = allocNode<PyrCallNode>((yyloc), selectornode, linkAllNodes((yyvsp[-6].node), (yyvsp[-3].node), (yyvsp[0].node)), nullptr);
					}
#line 3443 "lang11d_tab.cpp"
    break;

  case 142: /* adverb: %empty  */
#line 931 "lang11d"
                                  { (yyval.node) = nullptr; }
#line 3449 "lang11d_tab.cpp"
    break;

  case 143: /* adverb: '.' name  */
#line 932 "lang11d"
                                           { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_PushLitNode); }
#line 3455 "lang11d_tab.cpp"
    break;

  case 144: /* adverb: '.' integer  */
#line 933 "lang11d"
                                              { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_PushLitNode); }
#line 3461 "lang11d_tab.cpp"
    break;

  case 145: /* adverb: '.' '(' exprseq ')'  */
#line 934 "lang11d"
                                                      { (yyval.node) = (yyvsp[-1].node); }
#line 3467 "lang11d_tab.cpp"
    break;

  case 147: /* exprn: exprn ';' expr  */
#line 938 "lang11d"
                                        { (yyval.node) = allocNode<PyrDropNode>((yyloc), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3473 "lang11d_tab.cpp"
    break;

  case 149: /* arrayelems: %empty  */
#line 942 "lang11d"
                                 { (yyval.node) = nullptr; }
#line 3479 "lang11d_tab.cpp"
    break;

  case 150: /* arrayelems: arrayelems1 optcomma  */
#line 943 "lang11d"
                                                       { (yyval.node) = (yyvsp[-1].node); }
#line 3485 "lang11d_tab.cpp"
    break;

  case 152: /* arrayelems1: exprseq ':' exprseq  */
#line 947 "lang11d"
                                        { (yyval.node) = linkNextNode((yyvsp[-2].node), (yyvsp[0].node)); }
#line 3491 "lang11d_tab.cpp"
    break;

  case 153: /* arrayelems1: KEYBINOP exprseq  */
#line 949 "lang11d"
                                        { (yyval.node) = linkNextNode( (yyvsp[-1].slotNode)->changeLiteralType(pn_PushLitNode), (yyvsp[0].node)); }
#line 3497 "lang11d_tab.cpp"
    break;

  case 154: /* arrayelems1: arrayelems1 ',' exprseq  */
#line 951 "lang11d"
                                                { (yyval.node) = linkNextNode((yyvsp[-2].node), (yyvsp[0].node)); }
#line 3503 "lang11d_tab.cpp"
    break;

  case 155: /* arrayelems1: arrayelems1 ',' KEYBINOP exprseq  */
#line 953 "lang11d"
                                        { (yyval.node) = linkAllNodes((yyvsp[-3].node), (yyvsp[-1].slotNode)->changeLiteralType(pn_PushLitNode), (yyvsp[0].node)); }
#line 3509 "lang11d_tab.cpp"
    break;

  case 156: /* arrayelems1: arrayelems1 ',' exprseq ':' exprseq  */
#line 955 "lang11d"
                                        { (yyval.node) = linkAllNodes((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3515 "lang11d_tab.cpp"
    break;

  case 158: /* arglist1: arglist1 ',' exprseq  */
#line 959 "lang11d"
                                        { (yyval.node) = linkNextNode((yyvsp[-2].node), (yyvsp[0].node)); }
#line 3521 "lang11d_tab.cpp"
    break;

  case 159: /* arglistv1: '*' exprseq  */
#line 962 "lang11d"
                                        { (yylsp[0]) = (yyloc); (yyval.node) = (yyvsp[0].node); }
#line 3527 "lang11d_tab.cpp"
    break;

  case 160: /* arglistv1: arglist1 ',' '*' exprseq  */
#line 964 "lang11d"
                                                { (yyval.node) = linkNextNode((yyvsp[-3].node), (yyvsp[0].node)); }
#line 3533 "lang11d_tab.cpp"
    break;

  case 162: /* keyarglist1: keyarglist1 ',' keyarg  */
#line 968 "lang11d"
                                                { (yyval.node) = linkNextNode((yyvsp[-2].node), (yyvsp[0].node)); }
#line 3539 "lang11d_tab.cpp"
    break;

  case 163: /* keyarg: KEYBINOP exprseq  */
#line 971 "lang11d"
                                        { (yyval.node) = allocNode<PyrPushKeyArgNode>((yyloc), (yyvsp[-1].slotNode), (yyvsp[0].node)); }
#line 3545 "lang11d_tab.cpp"
    break;

  case 164: /* optkeyarglist: optcomma  */
#line 973 "lang11d"
                           { (yyval.node) = nullptr; }
#line 3551 "lang11d_tab.cpp"
    break;

  case 165: /* optkeyarglist: ',' keyarglist1 optcomma  */
#line 974 "lang11d"
                                                           { (yyval.node) = (yyvsp[-1].node); }
#line 3557 "lang11d_tab.cpp"
    break;

  case 166: /* mavars: nameList  */
#line 977 "lang11d"
                                        { (yyval.multiAssignListNode) = allocNode<PyrMultiAssignVarListNode>((yyloc), (yyvsp[0].slotNode), nullptr); }
#line 3563 "lang11d_tab.cpp"
    break;

  case 167: /* mavars: nameList ELLIPSIS name  */
#line 979 "lang11d"
                                        { (yyval.multiAssignListNode) = allocNode<PyrMultiAssignVarListNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[0].slotNode)); }
#line 3569 "lang11d_tab.cpp"
    break;

  case 169: /* nameList: nameList ',' name  */
#line 983 "lang11d"
                                        { (yyval.slotNode) = linkNextNode((yyvsp[-2].slotNode), (yyvsp[0].slotNode)); }
#line 3575 "lang11d_tab.cpp"
    break;

  case 170: /* slotliteral: integer  */
#line 985 "lang11d"
                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3581 "lang11d_tab.cpp"
    break;

  case 171: /* slotliteral: floatp  */
#line 986 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3587 "lang11d_tab.cpp"
    break;

  case 172: /* slotliteral: ASCII  */
#line 987 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3593 "lang11d_tab.cpp"
    break;

  case 173: /* slotliteral: STRING  */
#line 988 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3599 "lang11d_tab.cpp"
    break;

  case 174: /* slotliteral: SYMBOL  */
#line 989 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3605 "lang11d_tab.cpp"
    break;

  case 175: /* slotliteral: TRUEOBJ  */
#line 990 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3611 "lang11d_tab.cpp"
    break;

  case 176: /* slotliteral: FALSEOBJ  */
#line 991 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3617 "lang11d_tab.cpp"
    break;

  case 177: /* slotliteral: NILOBJ  */
#line 992 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3623 "lang11d_tab.cpp"
    break;

  case 178: /* slotliteral: listlit  */
#line 993 "lang11d"
                                                { (yyval.node) = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>((yyvsp[0].node)), pn_LiteralNode); }
#line 3629 "lang11d_tab.cpp"
    break;

  case 179: /* blockliteral: block  */
#line 995 "lang11d"
                        { (yyval.node) = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>((yyvsp[0].node)), pn_PushLitNode); }
#line 3635 "lang11d_tab.cpp"
    break;

  case 180: /* pushname: name  */
#line 997 "lang11d"
                               { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_PushNameNode); }
#line 3641 "lang11d_tab.cpp"
    break;

  case 181: /* pushliteral: integer  */
#line 999 "lang11d"
                                        { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_PushLitNode); }
#line 3647 "lang11d_tab.cpp"
    break;

  case 182: /* pushliteral: floatp  */
#line 1000 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_PushLitNode); }
#line 3653 "lang11d_tab.cpp"
    break;

  case 183: /* pushliteral: ASCII  */
#line 1001 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_PushLitNode); }
#line 3659 "lang11d_tab.cpp"
    break;

  case 184: /* pushliteral: STRING  */
#line 1002 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_PushLitNode); }
#line 3665 "lang11d_tab.cpp"
    break;

  case 185: /* pushliteral: SYMBOL  */
#line 1003 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_PushLitNode); }
#line 3671 "lang11d_tab.cpp"
    break;

  case 186: /* pushliteral: TRUEOBJ  */
#line 1004 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_PushLitNode); }
#line 3677 "lang11d_tab.cpp"
    break;

  case 187: /* pushliteral: FALSEOBJ  */
#line 1005 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_PushLitNode); }
#line 3683 "lang11d_tab.cpp"
    break;

  case 188: /* pushliteral: NILOBJ  */
#line 1006 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_PushLitNode); }
#line 3689 "lang11d_tab.cpp"
    break;

  case 189: /* pushliteral: listlit  */
#line 1007 "lang11d"
                                                { (yyval.node) = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>((yyvsp[0].node)), pn_PushLitNode); }
#line 3695 "lang11d_tab.cpp"
    break;

  case 190: /* listliteral: integer  */
#line 1009 "lang11d"
                                        { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode);}
#line 3701 "lang11d_tab.cpp"
    break;

  case 191: /* listliteral: floatp  */
#line 1010 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3707 "lang11d_tab.cpp"
    break;

  case 192: /* listliteral: ASCII  */
#line 1011 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3713 "lang11d_tab.cpp"
    break;

  case 193: /* listliteral: STRING  */
#line 1012 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3719 "lang11d_tab.cpp"
    break;

  case 194: /* listliteral: SYMBOL  */
#line 1013 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3725 "lang11d_tab.cpp"
    break;

  case 195: /* listliteral: name  */
#line 1014 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3731 "lang11d_tab.cpp"
    break;

  case 196: /* listliteral: TRUEOBJ  */
#line 1015 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3737 "lang11d_tab.cpp"
    break;

  case 197: /* listliteral: FALSEOBJ  */
#line 1016 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3743 "lang11d_tab.cpp"
    break;

  case 198: /* listliteral: NILOBJ  */
#line 1017 "lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType(pn_LiteralNode); }
#line 3749 "lang11d_tab.cpp"
    break;

  case 199: /* listliteral: listlit2  */
#line 1018 "lang11d"
                                                { (yyval.node) = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>((yyvsp[0].node)), pn_LiteralNode); }
#line 3755 "lang11d_tab.cpp"
    break;

  case 200: /* listliteral: dictlit2  */
#line 1019 "lang11d"
                                            { (yyval.node) = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>((yyvsp[0].node)), pn_LiteralNode); }
#line 3761 "lang11d_tab.cpp"
    break;

  case 201: /* block: '{' argdecls funcvardecls funcbody '}'  */
#line 1022 "lang11d"
                                        { (yyval.node) = allocNode<PyrBlockNode>((yyloc), (yyvsp[-3].argListNode), (yyvsp[-2].varListNode), (yyvsp[-1].node), false); }
#line 3767 "lang11d_tab.cpp"
    break;

  case 202: /* block: BEGINCLOSEDFUNC argdecls funcvardecls funcbody '}'  */
#line 1024 "lang11d"
                                        { (yyval.node) = allocNode<PyrBlockNode>((yyloc), (yyvsp[-3].argListNode), (yyvsp[-2].varListNode), (yyvsp[-1].node), true); }
#line 3773 "lang11d_tab.cpp"
    break;

  case 203: /* funcvardecls: %empty  */
#line 1028 "lang11d"
                         { (yyval.varListNode) = nullptr; }
#line 3779 "lang11d_tab.cpp"
    break;

  case 204: /* funcvardecls: funcvardecls funcvardecl  */
#line 1030 "lang11d"
                                        { (yyval.varListNode) = linkNextNode((yyvsp[-1].varListNode), (yyvsp[0].varListNode)); }
#line 3785 "lang11d_tab.cpp"
    break;

  case 206: /* funcvardecls1: funcvardecls1 funcvardecl  */
#line 1034 "lang11d"
                                        { (yyval.varListNode) = linkNextNode((yyvsp[-1].varListNode), (yyvsp[0].varListNode)); }
#line 3791 "lang11d_tab.cpp"
    break;

  case 207: /* funcvardecl: VAR vardeflist ';'  */
#line 1037 "lang11d"
                                        { (yyval.varListNode) = allocNode<PyrVarListNode>((yyloc), (yyvsp[-1].varDefNode), varLocal); }
#line 3797 "lang11d_tab.cpp"
    break;

  case 208: /* newBlockLiteral: newBlock  */
#line 1039 "lang11d"
                           { (yyval.node) = allocNode<PyrSlotNode>((yyloc), PyrSlot::make<void*>((yyvsp[0].node)), pn_PushLitNode); }
#line 3803 "lang11d_tab.cpp"
    break;

  case 209: /* newBlock: '#' ';' '{' argdecls newBodyOpt '}'  */
#line 1042 "lang11d"
                                        { (yyval.node) = allocNode<PyrNewBlockNode>((yyloc), (yyvsp[-2].argListNode), (yyvsp[-1].node), false); }
#line 3809 "lang11d_tab.cpp"
    break;

  case 210: /* newBodyOpt: %empty  */
#line 1044 "lang11d"
                                 { (yyval.node) = nullptr; }
#line 3815 "lang11d_tab.cpp"
    break;

  case 212: /* newBody: newBodyItem optsemi  */
#line 1047 "lang11d"
                                              { (yyval.node) = (yyvsp[-1].node); }
#line 3821 "lang11d_tab.cpp"
    break;

  case 213: /* newBody: newBodyItem ';' newBody  */
#line 1049 "lang11d"
                                        { (yyval.node) = linkNextNode((yyvsp[-2].node), (yyvsp[0].node)); }
#line 3827 "lang11d_tab.cpp"
    break;

  case 214: /* newBodyItem: VAR vardeflist  */
#line 1052 "lang11d"
                                        {  (yyval.node) = allocNode<PyrVarListNode>((yyloc), (yyvsp[0].varDefNode), varLocal); }
#line 3833 "lang11d_tab.cpp"
    break;

  case 216: /* argdecls: %empty  */
#line 1055 "lang11d"
                                 { (yyval.argListNode) = nullptr; }
#line 3839 "lang11d_tab.cpp"
    break;

  case 217: /* argdecls: ARG vardeflist ';'  */
#line 1057 "lang11d"
                                        { (yyval.argListNode) = allocNode<PyrArgListNode>((yyloc), (yyvsp[-1].varDefNode), nullptr, nullptr); }
#line 3845 "lang11d_tab.cpp"
    break;

  case 218: /* argdecls: ARG vardeflist0 ELLIPSIS name ';'  */
#line 1059 "lang11d"
                                        { (yyval.argListNode) = allocNode<PyrArgListNode>((yyloc), (yyvsp[-3].varDefNode), (yyvsp[-1].slotNode), nullptr); }
#line 3851 "lang11d_tab.cpp"
    break;

  case 219: /* argdecls: ARG vardeflist0 ELLIPSIS name ',' name ';'  */
#line 1061 "lang11d"
                                        { (yyval.argListNode) = allocNode<PyrArgListNode>((yyloc), (yyvsp[-5].varDefNode), (yyvsp[-3].slotNode), (yyvsp[-1].slotNode)); }
#line 3857 "lang11d_tab.cpp"
    break;

  case 220: /* argdecls: '|' slotdeflist '|'  */
#line 1063 "lang11d"
                                        { (yyval.argListNode) = allocNode<PyrArgListNode>((yyloc), (yyvsp[-1].varDefNode), nullptr, nullptr); }
#line 3863 "lang11d_tab.cpp"
    break;

  case 221: /* argdecls: '|' slotdeflist0 ELLIPSIS name '|'  */
#line 1065 "lang11d"
                                        { (yyval.argListNode) = allocNode<PyrArgListNode>((yyloc), (yyvsp[-3].varDefNode), (yyvsp[-1].slotNode), nullptr); }
#line 3869 "lang11d_tab.cpp"
    break;

  case 222: /* argdecls: '|' slotdeflist0 ELLIPSIS name ',' name '|'  */
#line 1067 "lang11d"
                                    { (yyval.argListNode) = allocNode<PyrArgListNode>((yyloc), (yyvsp[-5].varDefNode), (yyvsp[-3].slotNode), (yyvsp[-1].slotNode)); }
#line 3875 "lang11d_tab.cpp"
    break;

  case 223: /* argdecls1: ARG vardeflist ';'  */
#line 1070 "lang11d"
                                        { (yyval.argListNode) = allocNode<PyrArgListNode>((yyloc), (yyvsp[-1].varDefNode), nullptr, nullptr); }
#line 3881 "lang11d_tab.cpp"
    break;

  case 224: /* argdecls1: ARG vardeflist0 ELLIPSIS name ';'  */
#line 1072 "lang11d"
                                        { (yyval.argListNode) = allocNode<PyrArgListNode>((yyloc), (yyvsp[-3].varDefNode), (yyvsp[-1].slotNode), nullptr); }
#line 3887 "lang11d_tab.cpp"
    break;

  case 225: /* argdecls1: ARG vardeflist0 ELLIPSIS name ',' name ';'  */
#line 1074 "lang11d"
                                        { (yyval.argListNode) = allocNode<PyrArgListNode>((yyloc), (yyvsp[-5].varDefNode), (yyvsp[-3].slotNode), (yyvsp[-1].slotNode)); }
#line 3893 "lang11d_tab.cpp"
    break;

  case 226: /* argdecls1: '|' slotdeflist '|'  */
#line 1076 "lang11d"
                                        { (yyval.argListNode) = allocNode<PyrArgListNode>((yyloc), (yyvsp[-1].varDefNode), nullptr, nullptr); }
#line 3899 "lang11d_tab.cpp"
    break;

  case 227: /* argdecls1: '|' slotdeflist0 ELLIPSIS name '|'  */
#line 1078 "lang11d"
                                        { (yyval.argListNode) = allocNode<PyrArgListNode>((yyloc), (yyvsp[-3].varDefNode), (yyvsp[-1].slotNode), nullptr); }
#line 3905 "lang11d_tab.cpp"
    break;

  case 228: /* argdecls1: '|' slotdeflist0 ELLIPSIS name ',' name '|'  */
#line 1080 "lang11d"
                                    { (yyval.argListNode) = allocNode<PyrArgListNode>((yyloc), (yyvsp[-5].varDefNode), (yyvsp[-3].slotNode), (yyvsp[-1].slotNode)); }
#line 3911 "lang11d_tab.cpp"
    break;

  case 230: /* constdeflist: constdeflist optcomma constdef  */
#line 1085 "lang11d"
                                        { (yyval.varDefNode) = linkNextNode((yyvsp[-2].varDefNode), (yyvsp[0].varDefNode)); }
#line 3917 "lang11d_tab.cpp"
    break;

  case 231: /* constdef: rspec name '=' slotliteral  */
#line 1088 "lang11d"
                                        { (yyval.varDefNode) = allocNode<PyrVarDefNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[0].node), (yyvsp[-3].rwAccessor)); }
#line 3923 "lang11d_tab.cpp"
    break;

  case 232: /* slotdeflist0: %empty  */
#line 1090 "lang11d"
                         { (yyval.varDefNode) = nullptr; }
#line 3929 "lang11d_tab.cpp"
    break;

  case 235: /* slotdeflist: slotdeflist optcomma slotdef  */
#line 1095 "lang11d"
                                        { (yyval.varDefNode) = linkNextNode((yyvsp[-2].varDefNode), (yyvsp[0].varDefNode)); }
#line 3935 "lang11d_tab.cpp"
    break;

  case 236: /* slotdef: name  */
#line 1098 "lang11d"
                                        { (yyval.varDefNode) = allocNode<PyrVarDefNode>((yyloc), (yyvsp[0].slotNode), nullptr, ReadWriteAccessor::Private); }
#line 3941 "lang11d_tab.cpp"
    break;

  case 237: /* slotdef: name optequal slotliteral  */
#line 1100 "lang11d"
                                        { (yyval.varDefNode) = allocNode<PyrVarDefNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[0].node), ReadWriteAccessor::Private); }
#line 3947 "lang11d_tab.cpp"
    break;

  case 238: /* slotdef: name optequal '(' exprseq ')'  */
#line 1102 "lang11d"
                                        {
						PyrParseNode* node = (yyvsp[-1].node);
						node->mParens = 1;
						(yyval.varDefNode) = allocNode<PyrVarDefNode>((yyloc), (yyvsp[-4].slotNode), node, ReadWriteAccessor::Private);
					}
#line 3957 "lang11d_tab.cpp"
    break;

  case 239: /* vardeflist0: %empty  */
#line 1108 "lang11d"
                          { (yyval.varDefNode) = nullptr; }
#line 3963 "lang11d_tab.cpp"
    break;

  case 242: /* vardeflist: vardeflist ',' vardef  */
#line 1113 "lang11d"
                                        { (yyval.varDefNode) = linkNextNode((yyvsp[-2].varDefNode), (yyvsp[0].varDefNode)); }
#line 3969 "lang11d_tab.cpp"
    break;

  case 243: /* vardef: name  */
#line 1116 "lang11d"
                                        { (yyval.varDefNode) = allocNode<PyrVarDefNode>((yyloc), (yyvsp[0].slotNode), nullptr, ReadWriteAccessor::Private); }
#line 3975 "lang11d_tab.cpp"
    break;

  case 244: /* vardef: name '=' expr  */
#line 1118 "lang11d"
                                        { (yyval.varDefNode) = allocNode<PyrVarDefNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[0].node), ReadWriteAccessor::Private); }
#line 3981 "lang11d_tab.cpp"
    break;

  case 245: /* vardef: name '(' exprseq ')'  */
#line 1120 "lang11d"
                                        {
						PyrParseNode* node = (yyvsp[-1].node);
						node->mParens = 1;
						(yyval.varDefNode) = allocNode<PyrVarDefNode>((yyloc), (yyvsp[-3].slotNode), node, ReadWriteAccessor::Private);
					}
#line 3991 "lang11d_tab.cpp"
    break;

  case 246: /* dictslotdef: exprseq ':' exprseq  */
#line 1127 "lang11d"
                                        { (yyval.node) = linkNextNode((yyvsp[-2].node), (yyvsp[0].node)); }
#line 3997 "lang11d_tab.cpp"
    break;

  case 247: /* dictslotdef: KEYBINOP exprseq  */
#line 1129 "lang11d"
                                        { (yyval.node) = linkNextNode((yyvsp[-1].slotNode)->changeLiteralType(pn_PushLitNode), (yyvsp[0].node)); }
#line 4003 "lang11d_tab.cpp"
    break;

  case 249: /* dictslotlist1: dictslotlist1 ',' dictslotdef  */
#line 1134 "lang11d"
                                        { (yyval.node) = linkNextNode((yyvsp[-2].node), (yyvsp[0].node)); }
#line 4009 "lang11d_tab.cpp"
    break;

  case 250: /* dictslotlist: %empty  */
#line 1136 "lang11d"
                         { (yyval.node) = nullptr; }
#line 4015 "lang11d_tab.cpp"
    break;

  case 253: /* rwslotdeflist: rwslotdeflist ',' rwslotdef  */
#line 1141 "lang11d"
                                        { (yyval.varDefNode) = linkNextNode((yyvsp[-2].varDefNode), (yyvsp[0].varDefNode)); }
#line 4021 "lang11d_tab.cpp"
    break;

  case 254: /* rwslotdef: rwspec name  */
#line 1144 "lang11d"
                                        { (yyval.varDefNode) = allocNode<PyrVarDefNode>((yyloc), (yyvsp[0].slotNode), nullptr, (yyvsp[-1].rwAccessor)); }
#line 4027 "lang11d_tab.cpp"
    break;

  case 255: /* rwslotdef: rwspec name '=' slotliteral  */
#line 1146 "lang11d"
                                        { (yyval.varDefNode) = allocNode<PyrVarDefNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[0].node), (yyvsp[-3].rwAccessor)); }
#line 4033 "lang11d_tab.cpp"
    break;

  case 256: /* dictlit2: '(' litdictslotlist ')'  */
#line 1149 "lang11d"
                                        { (yyval.node) = allocNode<PyrLitDictNode>((yyloc), (yyvsp[-1].node)); }
#line 4039 "lang11d_tab.cpp"
    break;

  case 257: /* litdictslotdef: listliteral ':' listliteral  */
#line 1152 "lang11d"
                                        { (yyval.node) = linkNextNode((yyvsp[-2].node), (yyvsp[0].node)); }
#line 4045 "lang11d_tab.cpp"
    break;

  case 258: /* litdictslotdef: KEYBINOP listliteral  */
#line 1154 "lang11d"
                                        { (yyval.node) = linkNextNode((yyvsp[-1].slotNode)->changeLiteralType(pn_PushLitNode), (yyvsp[0].node)); }
#line 4051 "lang11d_tab.cpp"
    break;

  case 260: /* litdictslotlist1: litdictslotlist1 ',' litdictslotdef  */
#line 1158 "lang11d"
                                        { (yyval.node) = linkNextNode((yyvsp[-2].node), (yyvsp[0].node)); }
#line 4057 "lang11d_tab.cpp"
    break;

  case 261: /* litdictslotlist: %empty  */
#line 1160 "lang11d"
                         { (yyval.node) = nullptr; }
#line 4063 "lang11d_tab.cpp"
    break;

  case 263: /* listlit: '#' '[' literallistc ']'  */
#line 1166 "lang11d"
                                        { (yyval.node) = allocNode<PyrLitListNode>((yyloc), nullptr, (yyvsp[-1].node)); }
#line 4069 "lang11d_tab.cpp"
    break;

  case 264: /* listlit: '#' CLASSNAME '[' literallistc ']'  */
#line 1168 "lang11d"
                                                { (yyval.node) = allocNode<PyrLitListNode>((yyloc), (yyvsp[-3].slotNode), (yyvsp[-1].node)); }
#line 4075 "lang11d_tab.cpp"
    break;

  case 265: /* listlit2: '[' literallistc ']'  */
#line 1171 "lang11d"
                                        { (yyval.node) = allocNode<PyrLitListNode>((yyloc), nullptr, (yyvsp[-1].node)); }
#line 4081 "lang11d_tab.cpp"
    break;

  case 266: /* listlit2: CLASSNAME '[' literallistc ']'  */
#line 1173 "lang11d"
                                        { (yyval.node) = allocNode<PyrLitListNode>((yyloc), (yyvsp[-3].slotNode), (yyvsp[-1].node)); }
#line 4087 "lang11d_tab.cpp"
    break;

  case 267: /* literallistc: %empty  */
#line 1175 "lang11d"
                         { (yyval.node) = nullptr; }
#line 4093 "lang11d_tab.cpp"
    break;

  case 270: /* literallist1: literallist1 ',' listliteral  */
#line 1180 "lang11d"
                                        { (yyval.node) = linkNextNode((yyvsp[-2].node), (yyvsp[0].node)); }
#line 4099 "lang11d_tab.cpp"
    break;

  case 271: /* rwspec: %empty  */
#line 1182 "lang11d"
                                  { (yyval.rwAccessor) = ReadWriteAccessor::Private; }
#line 4105 "lang11d_tab.cpp"
    break;

  case 272: /* rwspec: '<'  */
#line 1183 "lang11d"
                                      { (yyval.rwAccessor) = ReadWriteAccessor::Read; }
#line 4111 "lang11d_tab.cpp"
    break;

  case 273: /* rwspec: READWRITEVAR  */
#line 1184 "lang11d"
                                               { (yyval.rwAccessor) = ReadWriteAccessor::ReadWrite; }
#line 4117 "lang11d_tab.cpp"
    break;

  case 274: /* rwspec: '>'  */
#line 1185 "lang11d"
                                      { (yyval.rwAccessor) = ReadWriteAccessor::Write; }
#line 4123 "lang11d_tab.cpp"
    break;

  case 275: /* rspec: %empty  */
#line 1187 "lang11d"
                                 { (yyval.rwAccessor) = ReadWriteAccessor::Private; }
#line 4129 "lang11d_tab.cpp"
    break;

  case 276: /* rspec: '<'  */
#line 1188 "lang11d"
                                      { (yyval.rwAccessor) = ReadWriteAccessor::Read; }
#line 4135 "lang11d_tab.cpp"
    break;

  case 278: /* integer: '-' INTEGER  */
#line 1192 "lang11d"
                                        {
						const auto v = (yyvsp[0].slotNode)->mSlot.getInt();
						(yyvsp[0].slotNode)->mSlot = PyrSlot::make(-v);
						(yyval.slotNode) = (yyvsp[0].slotNode);
					}
#line 4145 "lang11d_tab.cpp"
    break;

  case 280: /* floatr: '-' SC_FLOAT  */
#line 1200 "lang11d"
                                        {
						const double v = (yyvsp[0].slotNode)->mSlot.getDouble();
						(yyvsp[0].slotNode)->mSlot = PyrSlot::make(-v);
						(yyval.slotNode) = (yyvsp[0].slotNode);
					}
#line 4155 "lang11d_tab.cpp"
    break;

  case 282: /* accidental: '-' ACCIDENTAL  */
#line 1208 "lang11d"
                                        {
						const double in = (yyvsp[0].slotNode)->mSlot.getDouble();
						const double intval = floor(in + 0.5);
						const double fracval = in - intval;
						(yyvsp[0].slotNode)->mSlot = PyrSlot::make(-intval + fracval);
						(yyval.slotNode) = (yyvsp[0].slotNode);
					}
#line 4167 "lang11d_tab.cpp"
    break;

  case 285: /* floatp: floatr PIE  */
#line 1220 "lang11d"
                                        {
						(yyvsp[-1].slotNode)->mSlot = PyrSlot::make((yyvsp[-1].slotNode)->mSlot.getDouble() * pi);
						(yyvsp[-1].slotNode)->mLocation = (yyloc);
						(yyval.slotNode) = (yyvsp[-1].slotNode);
					}
#line 4177 "lang11d_tab.cpp"
    break;

  case 286: /* floatp: integer PIE  */
#line 1226 "lang11d"
                                        {
						(yyvsp[-1].slotNode)->mSlot = PyrSlot::make((yyvsp[-1].slotNode)->mSlot.getInt() * pi);
						(yyvsp[-1].slotNode)->mLocation = (yyloc);
						(yyval.slotNode) = (yyvsp[-1].slotNode);
					}
#line 4187 "lang11d_tab.cpp"
    break;

  case 288: /* floatp: '-' PIE  */
#line 1233 "lang11d"
                                        {
						(yyvsp[0].slotNode)->mSlot = PyrSlot::make(-pi);
						(yyvsp[0].slotNode)->mLocation = (yyloc);
						(yyval.slotNode) = (yyvsp[0].slotNode);
					}
#line 4197 "lang11d_tab.cpp"
    break;

  case 301: /* curryArg: CURRYARG  */
#line 1242 "lang11d"
                                   { (yyval.node) = allocNode<PyrCurryArgNode>((yyloc)); }
#line 4203 "lang11d_tab.cpp"
    break;


#line 4207 "lang11d_tab.cpp"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
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
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

