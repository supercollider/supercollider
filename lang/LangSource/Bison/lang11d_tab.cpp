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
#line 18 "lang/LangSource/Bison/lang11d"


#include "BisonHeaderInclude.hpp"
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



#line 93 "lang/LangSource/Bison/lang11d_tab.cpp"

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
  YYSYMBOL_OPENCURLY = 3,                  /* OPENCURLY  */
  YYSYMBOL_CLOSECURLY = 4,                 /* CLOSECURLY  */
  YYSYMBOL_OPENSQUARE = 5,                 /* OPENSQUARE  */
  YYSYMBOL_CLOSESQUARE = 6,                /* CLOSESQUARE  */
  YYSYMBOL_OPENPAREN = 7,                  /* OPENPAREN  */
  YYSYMBOL_CLOSEPAREN = 8,                 /* CLOSEPAREN  */
  YYSYMBOL_SEMICOLON = 9,                  /* SEMICOLON  */
  YYSYMBOL_NONLOCALRETURN = 10,            /* NONLOCALRETURN  */
  YYSYMBOL_COMMA = 11,                     /* COMMA  */
  YYSYMBOL_HASH = 12,                      /* HASH  */
  YYSYMBOL_TILDE = 13,                     /* TILDE  */
  YYSYMBOL_NAME = 14,                      /* NAME  */
  YYSYMBOL_INTEGER = 15,                   /* INTEGER  */
  YYSYMBOL_SC_FLOAT = 16,                  /* SC_FLOAT  */
  YYSYMBOL_ACCIDENTAL = 17,                /* ACCIDENTAL  */
  YYSYMBOL_SYMBOL = 18,                    /* SYMBOL  */
  YYSYMBOL_STRING = 19,                    /* STRING  */
  YYSYMBOL_ASCII = 20,                     /* ASCII  */
  YYSYMBOL_PRIMITIVENAME = 21,             /* PRIMITIVENAME  */
  YYSYMBOL_CLASSNAME = 22,                 /* CLASSNAME  */
  YYSYMBOL_CURRYARG = 23,                  /* CURRYARG  */
  YYSYMBOL_VAR = 24,                       /* VAR  */
  YYSYMBOL_ARG = 25,                       /* ARG  */
  YYSYMBOL_CLASSVAR = 26,                  /* CLASSVAR  */
  YYSYMBOL_SC_CONST = 27,                  /* SC_CONST  */
  YYSYMBOL_NILOBJ = 28,                    /* NILOBJ  */
  YYSYMBOL_TRUEOBJ = 29,                   /* TRUEOBJ  */
  YYSYMBOL_FALSEOBJ = 30,                  /* FALSEOBJ  */
  YYSYMBOL_PSEUDOVAR = 31,                 /* PSEUDOVAR  */
  YYSYMBOL_ELLIPSIS = 32,                  /* ELLIPSIS  */
  YYSYMBOL_DOTDOT = 33,                    /* DOTDOT  */
  YYSYMBOL_PIE = 34,                       /* PIE  */
  YYSYMBOL_BEGINCLOSEDFUNC = 35,           /* BEGINCLOSEDFUNC  */
  YYSYMBOL_BADTOKEN = 36,                  /* BADTOKEN  */
  YYSYMBOL_INTERPRET = 37,                 /* INTERPRET  */
  YYSYMBOL_LEFTARROW = 38,                 /* LEFTARROW  */
  YYSYMBOL_WHILE = 39,                     /* WHILE  */
  YYSYMBOL_COLON = 40,                     /* COLON  */
  YYSYMBOL_EQUALSSIGN = 41,                /* EQUALSSIGN  */
  YYSYMBOL_BINOP = 42,                     /* BINOP  */
  YYSYMBOL_KEYBINOP = 43,                  /* KEYBINOP  */
  YYSYMBOL_MINUS = 44,                     /* MINUS  */
  YYSYMBOL_LESSTHAN = 45,                  /* LESSTHAN  */
  YYSYMBOL_GREATERTHAN = 46,               /* GREATERTHAN  */
  YYSYMBOL_MULTIPLY = 47,                  /* MULTIPLY  */
  YYSYMBOL_ADD = 48,                       /* ADD  */
  YYSYMBOL_PIPE = 49,                      /* PIPE  */
  YYSYMBOL_READWRITEVAR = 50,              /* READWRITEVAR  */
  YYSYMBOL_DOT = 51,                       /* DOT  */
  YYSYMBOL_BACKTICK = 52,                  /* BACKTICK  */
  YYSYMBOL_UMINUS = 53,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 54,                  /* $accept  */
  YYSYMBOL_root = 55,                      /* root  */
  YYSYMBOL_classes = 56,                   /* classes  */
  YYSYMBOL_classextensions = 57,           /* classextensions  */
  YYSYMBOL_classdef = 58,                  /* classdef  */
  YYSYMBOL_classextension = 59,            /* classextension  */
  YYSYMBOL_optname = 60,                   /* optname  */
  YYSYMBOL_superclass = 61,                /* superclass  */
  YYSYMBOL_classvardecls = 62,             /* classvardecls  */
  YYSYMBOL_classvardecl = 63,              /* classvardecl  */
  YYSYMBOL_methods = 64,                   /* methods  */
  YYSYMBOL_methoddef = 65,                 /* methoddef  */
  YYSYMBOL_optsemi = 66,                   /* optsemi  */
  YYSYMBOL_optcomma = 67,                  /* optcomma  */
  YYSYMBOL_optequal = 68,                  /* optequal  */
  YYSYMBOL_funcbody = 69,                  /* funcbody  */
  YYSYMBOL_cmdlinecode = 70,               /* cmdlinecode  */
  YYSYMBOL_methbody = 71,                  /* methbody  */
  YYSYMBOL_primitive = 72,                 /* primitive  */
  YYSYMBOL_retval = 73,                    /* retval  */
  YYSYMBOL_funretval = 74,                 /* funretval  */
  YYSYMBOL_blocklist1 = 75,                /* blocklist1  */
  YYSYMBOL_blocklistitem = 76,             /* blocklistitem  */
  YYSYMBOL_blocklist = 77,                 /* blocklist  */
  YYSYMBOL_msgsend = 78,                   /* msgsend  */
  YYSYMBOL_generator = 79,                 /* generator  */
  YYSYMBOL_80_1 = 80,                      /* $@1  */
  YYSYMBOL_81_2 = 81,                      /* $@2  */
  YYSYMBOL_nextqual = 82,                  /* nextqual  */
  YYSYMBOL_qual = 83,                      /* qual  */
  YYSYMBOL_expr1 = 84,                     /* expr1  */
  YYSYMBOL_valrangex1 = 85,                /* valrangex1  */
  YYSYMBOL_valrangeassign = 86,            /* valrangeassign  */
  YYSYMBOL_valrangexd = 87,                /* valrangexd  */
  YYSYMBOL_valrange2 = 88,                 /* valrange2  */
  YYSYMBOL_valrange3 = 89,                 /* valrange3  */
  YYSYMBOL_expr = 90,                      /* expr  */
  YYSYMBOL_adverb = 91,                    /* adverb  */
  YYSYMBOL_exprn = 92,                     /* exprn  */
  YYSYMBOL_exprseq = 93,                   /* exprseq  */
  YYSYMBOL_arrayelems = 94,                /* arrayelems  */
  YYSYMBOL_arrayelems1 = 95,               /* arrayelems1  */
  YYSYMBOL_arglist1 = 96,                  /* arglist1  */
  YYSYMBOL_arglistv1 = 97,                 /* arglistv1  */
  YYSYMBOL_keyarglist1 = 98,               /* keyarglist1  */
  YYSYMBOL_keyarg = 99,                    /* keyarg  */
  YYSYMBOL_optkeyarglist = 100,            /* optkeyarglist  */
  YYSYMBOL_mavars = 101,                   /* mavars  */
  YYSYMBOL_mavarlist = 102,                /* mavarlist  */
  YYSYMBOL_slotliteral = 103,              /* slotliteral  */
  YYSYMBOL_blockliteral = 104,             /* blockliteral  */
  YYSYMBOL_pushname = 105,                 /* pushname  */
  YYSYMBOL_pushliteral = 106,              /* pushliteral  */
  YYSYMBOL_listliteral = 107,              /* listliteral  */
  YYSYMBOL_block = 108,                    /* block  */
  YYSYMBOL_funcvardecls = 109,             /* funcvardecls  */
  YYSYMBOL_funcvardecls1 = 110,            /* funcvardecls1  */
  YYSYMBOL_funcvardecl = 111,              /* funcvardecl  */
  YYSYMBOL_argdecls = 112,                 /* argdecls  */
  YYSYMBOL_argdecls1 = 113,                /* argdecls1  */
  YYSYMBOL_constdeflist = 114,             /* constdeflist  */
  YYSYMBOL_constdef = 115,                 /* constdef  */
  YYSYMBOL_slotdeflist0 = 116,             /* slotdeflist0  */
  YYSYMBOL_slotdeflist = 117,              /* slotdeflist  */
  YYSYMBOL_slotdef = 118,                  /* slotdef  */
  YYSYMBOL_vardeflist0 = 119,              /* vardeflist0  */
  YYSYMBOL_vardeflist = 120,               /* vardeflist  */
  YYSYMBOL_vardef = 121,                   /* vardef  */
  YYSYMBOL_dictslotdef = 122,              /* dictslotdef  */
  YYSYMBOL_dictslotlist1 = 123,            /* dictslotlist1  */
  YYSYMBOL_dictslotlist = 124,             /* dictslotlist  */
  YYSYMBOL_rwslotdeflist = 125,            /* rwslotdeflist  */
  YYSYMBOL_rwslotdef = 126,                /* rwslotdef  */
  YYSYMBOL_listlit = 127,                  /* listlit  */
  YYSYMBOL_listlit2 = 128,                 /* listlit2  */
  YYSYMBOL_literallistc = 129,             /* literallistc  */
  YYSYMBOL_literallist1 = 130,             /* literallist1  */
  YYSYMBOL_rwspec = 131,                   /* rwspec  */
  YYSYMBOL_rspec = 132,                    /* rspec  */
  YYSYMBOL_integer = 133,                  /* integer  */
  YYSYMBOL_floatr = 134,                   /* floatr  */
  YYSYMBOL_accidental = 135,               /* accidental  */
  YYSYMBOL_pie = 136,                      /* pie  */
  YYSYMBOL_floatp = 137,                   /* floatp  */
  YYSYMBOL_name = 138,                     /* name  */
  YYSYMBOL_classname = 139,                /* classname  */
  YYSYMBOL_primname = 140,                 /* primname  */
  YYSYMBOL_trueobj = 141,                  /* trueobj  */
  YYSYMBOL_falseobj = 142,                 /* falseobj  */
  YYSYMBOL_nilobj = 143,                   /* nilobj  */
  YYSYMBOL_ascii = 144,                    /* ascii  */
  YYSYMBOL_symbol = 145,                   /* symbol  */
  YYSYMBOL_string = 146,                   /* string  */
  YYSYMBOL_pseudovar = 147,                /* pseudovar  */
  YYSYMBOL_binop = 148,                    /* binop  */
  YYSYMBOL_keybinop = 149,                 /* keybinop  */
  YYSYMBOL_binop2 = 150,                   /* binop2  */
  YYSYMBOL_curryarg = 151                  /* curryarg  */
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
#define YYFINAL  70
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1677

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  98
/* YYNRULES -- Number of rules.  */
#define YYNRULES  294
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  551

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   308


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
      45,    46,    47,    48,    49,    50,    51,    52,    53
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    48,    48,    50,    52,    56,    57,    61,    62,    66,
      70,    77,    83,    84,    87,    88,    92,    93,    97,    99,
     101,   105,   106,   110,   113,   116,   119,   124,   125,   128,
     129,   132,   133,   136,   137,   141,   143,   145,   147,   149,
     151,   153,   157,   158,   162,   163,   168,   169,   174,   175,
     179,   180,   186,   187,   190,   191,   194,   198,   202,   206,
     211,   215,   220,   238,   251,   253,   264,   275,   286,   299,
     320,   329,   338,   343,   357,   379,   383,   389,   407,   413,
     413,   423,   423,   430,   451,   455,   489,   527,   541,   552,
     556,   581,   582,   583,   584,   585,   586,   587,   594,   604,
     606,   608,   610,   612,   614,   627,   630,   657,   675,   702,
     730,   749,   777,   804,   822,   847,   875,   894,   922,   941,
     960,   977,   991,  1012,  1031,  1049,  1066,  1082,  1098,  1099,
    1100,  1101,  1102,  1115,  1129,  1134,  1138,  1149,  1154,  1164,
    1169,  1183,  1199,  1200,  1201,  1202,  1205,  1206,  1212,  1215,
    1216,  1220,  1221,  1223,  1228,  1230,  1237,  1245,  1246,  1250,
    1252,  1256,  1257,  1261,  1265,  1266,  1269,  1271,  1275,  1276,
    1281,  1282,  1283,  1284,  1285,  1286,  1287,  1288,  1289,  1292,
    1295,  1298,  1299,  1300,  1301,  1302,  1303,  1304,  1305,  1306,
    1309,  1310,  1311,  1312,  1313,  1314,  1315,  1316,  1317,  1318,
    1321,  1324,  1329,  1330,  1334,  1335,  1339,  1343,  1344,  1348,
    1352,  1356,  1360,  1366,  1370,  1374,  1378,  1382,  1389,  1390,
    1394,  1398,  1399,  1402,  1403,  1407,  1409,  1411,  1419,  1420,
    1423,  1424,  1428,  1430,  1432,  1440,  1442,  1449,  1450,  1454,
    1455,  1458,  1459,  1463,  1465,  1469,  1471,  1475,  1477,  1481,
    1482,  1485,  1486,  1490,  1491,  1493,  1495,  1499,  1500,  1504,
    1505,  1514,  1515,  1524,  1525,  1536,  1539,  1540,  1541,  1547,
    1555,  1562,  1571,  1572,  1575,  1578,  1581,  1584,  1587,  1590,
    1593,  1596,  1599,  1602,  1603,  1604,  1605,  1606,  1607,  1608,
    1609,  1612,  1615,  1616,  1619
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
  "\"end of file\"", "error", "\"invalid token\"", "OPENCURLY",
  "CLOSECURLY", "OPENSQUARE", "CLOSESQUARE", "OPENPAREN", "CLOSEPAREN",
  "SEMICOLON", "NONLOCALRETURN", "COMMA", "HASH", "TILDE", "NAME",
  "INTEGER", "SC_FLOAT", "ACCIDENTAL", "SYMBOL", "STRING", "ASCII",
  "PRIMITIVENAME", "CLASSNAME", "CURRYARG", "VAR", "ARG", "CLASSVAR",
  "SC_CONST", "NILOBJ", "TRUEOBJ", "FALSEOBJ", "PSEUDOVAR", "ELLIPSIS",
  "DOTDOT", "PIE", "BEGINCLOSEDFUNC", "BADTOKEN", "INTERPRET", "LEFTARROW",
  "WHILE", "COLON", "EQUALSSIGN", "BINOP", "KEYBINOP", "MINUS", "LESSTHAN",
  "GREATERTHAN", "MULTIPLY", "ADD", "PIPE", "READWRITEVAR", "DOT",
  "BACKTICK", "UMINUS", "$accept", "root", "classes", "classextensions",
  "classdef", "classextension", "optname", "superclass", "classvardecls",
  "classvardecl", "methods", "methoddef", "optsemi", "optcomma",
  "optequal", "funcbody", "cmdlinecode", "methbody", "primitive", "retval",
  "funretval", "blocklist1", "blocklistitem", "blocklist", "msgsend",
  "generator", "$@1", "$@2", "nextqual", "qual", "expr1", "valrangex1",
  "valrangeassign", "valrangexd", "valrange2", "valrange3", "expr",
  "adverb", "exprn", "exprseq", "arrayelems", "arrayelems1", "arglist1",
  "arglistv1", "keyarglist1", "keyarg", "optkeyarglist", "mavars",
  "mavarlist", "slotliteral", "blockliteral", "pushname", "pushliteral",
  "listliteral", "block", "funcvardecls", "funcvardecls1", "funcvardecl",
  "argdecls", "argdecls1", "constdeflist", "constdef", "slotdeflist0",
  "slotdeflist", "slotdef", "vardeflist0", "vardeflist", "vardef",
  "dictslotdef", "dictslotlist1", "dictslotlist", "rwslotdeflist",
  "rwslotdef", "listlit", "listlit2", "literallistc", "literallist1",
  "rwspec", "rspec", "integer", "floatr", "accidental", "pie", "floatp",
  "name", "classname", "primname", "trueobj", "falseobj", "nilobj",
  "ascii", "symbol", "string", "pseudovar", "binop", "keybinop", "binop2",
  "curryarg", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-473)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-291)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      60,   806,    94,    38,    94,    50,  -473,    16,  1149,   710,
    1444,   202,    46,  -473,  -473,  -473,  -473,  -473,  -473,  -473,
    -473,  -473,    46,    46,  -473,  -473,  -473,  -473,  -473,    15,
    -473,   211,    46,  1444,  -473,  -473,  -473,  -473,  -473,    99,
    -473,  -473,  -473,  1626,   160,   162,  -473,  -473,  -473,  -473,
     666,  -473,   666,  -473,   171,   171,  -473,  -473,  -473,    20,
     241,  -473,  -473,  -473,  -473,  -473,  -473,  -473,  -473,   218,
    -473,  -473,    13,  -473,  1444,    46,  1444,    46,  -473,   758,
    -473,   190,   233,   220,  1444,  1444,  1191,  -473,   211,  -473,
    -473,  -473,  -473,   134,  -473,   234,   261,   666,   666,  -473,
     239,   252,  -473,  1444,   258,  1500,  1547,   229,   111,  -473,
     270,   242,   141,  -473,    21,   257,   225,  -473,  -473,  -473,
    -473,  -473,   260,    25,  -473,    31,  -473,  1233,  -473,  -473,
     139,  -473,   262,  1444,  -473,  -473,  -473,  -473,  -473,   666,
    -473,  -473,   978,  1444,    51,  -473,  -473,  -473,  1149,   849,
      51,  -473,    46,    94,   281,  -473,   271,   256,  -473,   275,
      26,   666,  1444,  -473,  1149,  -473,  -473,  -473,  1444,   280,
      19,  -473,  -473,  1444,  1444,  1444,   285,   313,   666,  1149,
    -473,  -473,  -473,   205,  -473,  -473,  1547,  -473,  -473,   318,
     324,   171,  -473,  -473,   336,  -473,  -473,  -473,  -473,  -473,
    -473,  1444,    46,    46,  1547,  1444,  -473,    46,  1444,  1444,
      46,  -473,   666,    46,  -473,  -473,    46,  -473,  1577,  1444,
    -473,    40,  1275,   892,    76,   338,  1444,  1626,  -473,    51,
    1444,   343,   344,  1626,  -473,   341,    51,   343,   344,   347,
    -473,  1444,  1466,   360,  -473,  -473,  -473,   357,    46,  -473,
     358,    46,  -473,   345,  -473,  -473,   330,  1444,  -473,  -473,
    1444,  1444,   342,  -473,  -473,  -473,  -473,   372,   346,  -473,
    1444,  1021,    51,   375,  -473,  1547,  -473,  1547,  1626,  -473,
    -473,   381,  1626,  -473,   382,  1626,   380,   388,    32,  -473,
    1444,    85,  -473,  -473,   171,  -473,  -473,  -473,  -473,  -473,
    -473,  -473,   387,   359,  1444,  1317,  1444,   184,    51,   343,
     344,   347,   935,  1444,    51,  -473,  1444,   383,  -473,  -473,
     354,    51,  -473,  1064,  -473,   401,   367,   403,  -473,  -473,
     404,   413,   367,   422,  -473,  -473,  1584,  -473,   430,   435,
     399,   254,  1107,   431,  1107,    33,  -473,  1444,  -473,   409,
    -473,  1444,  -473,    51,   343,   344,  -473,  -473,   437,  -473,
    -473,  -473,  -473,    46,  -473,   443,   419,  1444,  -473,   420,
     439,   448,   421,  1359,  -473,   456,   458,   459,    51,   343,
     344,   347,  1626,   464,  1444,   347,    90,  -473,    51,  -473,
    -473,    51,   472,   476,    15,    15,   478,   209,   209,   438,
    -473,  1478,    46,   119,   480,   471,   288,  -473,   481,    46,
    -473,  -473,  1444,  -473,    51,   479,   483,  -473,   440,  -473,
    1444,  1626,  1444,   447,   453,  1444,   455,   491,    51,  -473,
      51,  -473,   492,   493,   498,  -473,  -473,  -473,  1444,  -473,
    -473,  -473,    15,    15,  -473,  -473,  -473,  -473,  -473,  -473,
     331,  -473,    46,   335,  -473,   353,  -473,    46,  -473,   470,
    1444,  1444,  -473,  1107,  -473,  1444,   474,  -473,   465,  -473,
      51,  -473,  -473,  1626,  1626,  1444,  1444,  1626,  1444,   475,
    -473,  -473,    51,  -473,    51,  1626,  -473,  -473,    78,    78,
     254,  -473,   209,   485,  -473,  -473,   438,   486,  1444,   471,
     471,  -473,   471,  1444,  -473,  -473,  1626,  1626,  1626,  1444,
    -473,  -473,    78,    78,  -473,  1402,   510,  1402,  1490,  -473,
    1623,  -473,  1623,   471,  -473,  -473,  -473,   471,  1626,  1402,
    1402,  1444,   516,  -473,   518,  -473,   525,  -473,  -473,  -473,
    -473,  -473,   526,   528,  1500,  -473,  -473,  -473,  -473,  -473,
    -473
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       5,    48,     0,     0,     2,     3,     7,   207,   149,   239,
       0,     0,     0,   272,   259,   261,   263,   280,   281,   279,
     274,   294,     0,   228,   278,   276,   277,   282,   265,   207,
     273,     0,   221,     0,    41,     4,    33,    96,    93,   128,
     105,   130,   129,   146,    27,    48,    92,    94,    91,   179,
      48,   204,    48,   189,   181,   266,   267,   270,   182,   180,
     131,   186,   187,   188,   183,   185,   184,   103,    95,     0,
       1,     6,    14,     8,     0,   228,     0,   221,   202,   239,
     291,   151,     0,    29,     0,     0,     0,   283,   287,   285,
     286,   288,   289,   221,   284,     0,     0,    48,    48,   237,
      29,     0,   292,   293,     0,    27,   249,     0,   166,   168,
       0,    98,     0,   230,   232,     0,   229,   202,   260,   262,
     264,   271,     0,    29,   223,    31,   133,     0,   287,   290,
       0,   293,   142,    28,   148,    34,    40,   205,    39,    48,
     269,   268,     0,     0,    56,    50,    53,    52,   149,     0,
      65,    21,    12,     0,     0,    81,     0,   229,    79,     0,
      29,    48,     0,    99,    30,   150,   153,   119,     0,     0,
       0,   100,    97,     0,   118,     0,     0,     0,    48,    30,
     240,   102,   236,     0,    28,    49,   249,   251,   199,     0,
      29,   190,   191,   195,     0,   196,   197,   198,   192,   194,
     193,     0,     0,     0,   249,     0,   206,     0,     0,     0,
       0,   213,    48,     0,    30,   215,     0,    32,     0,     0,
     157,     0,     0,     0,    54,     0,     0,   147,    38,     0,
       0,    29,    29,   135,    51,     0,    54,    29,    29,    29,
     161,     0,     0,     0,    13,    15,    16,     0,     0,   208,
       0,     0,   210,     0,   203,   152,   154,     0,   123,   101,
       0,   124,     0,   120,   235,    37,    36,     0,     0,   238,
       0,     0,    57,     0,   245,    30,   250,   249,   139,   169,
     167,     0,   136,   231,     0,   233,     0,     0,     0,   224,
       0,     0,   226,   178,   170,   171,   175,   176,   177,   172,
     174,   173,     0,   104,     0,     0,     0,     0,    54,    29,
      29,    29,     0,     0,    55,    78,     0,     0,   144,   143,
     134,    58,   159,    30,   164,     0,    30,     0,    64,    66,
       0,     0,    30,     0,   163,    11,   288,    22,     0,     0,
      14,    21,     0,     0,     0,     0,   200,     0,   155,     0,
     125,   122,    35,     0,    29,    29,   247,   252,     0,   246,
     234,   214,   201,     0,   216,     0,   107,     0,   158,   106,
       0,     0,   132,     0,    70,     0,     0,     0,    54,    29,
      29,    29,   137,     0,     0,    29,    54,    62,    54,    69,
     162,    54,     0,     0,   207,   207,     0,   253,   253,   257,
      17,     0,     0,     0,     0,    83,   180,   209,     0,     0,
     211,   156,   126,   121,    60,     0,     0,   248,     0,   227,
       0,   140,     0,   108,   113,     0,   112,     0,    54,    74,
      54,    75,     0,     0,     0,   145,   160,   165,     0,    59,
      68,    67,   207,   207,   202,   202,    16,   254,   256,   255,
       0,   241,     0,     0,   258,    29,   218,     0,     9,     0,
       0,     0,    82,     0,    88,     0,     0,    80,     0,   127,
      54,    63,   217,   110,   109,     0,     0,   141,     0,   114,
      73,    71,    54,    77,    54,   138,   202,   202,    44,    44,
      21,    19,   253,   243,    18,    20,   257,     0,     0,    83,
      83,    84,    83,     0,   212,    61,   111,   116,   115,     0,
      76,    72,    44,    44,   275,    46,    27,    46,     0,   242,
       0,   219,     0,    83,    90,    89,    85,    83,   117,    46,
      46,     0,     0,    42,    46,    45,     0,    10,   244,   220,
      87,    86,     0,     0,    27,    23,    43,    25,    24,    26,
      47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -473,  -473,  -473,  -473,  -473,   529,  -473,   195,    91,  -473,
    -339,  -473,  -100,   -76,  -473,   298,  -473,  -333,  -374,     2,
     494,   -28,  -115,   237,  -473,   127,  -473,  -473,   -53,  -330,
    -473,  -473,  -473,  -473,  -473,  -473,    -7,  -473,  -473,    -8,
     390,  -473,   -48,  -136,  -127,   208,   266,  -473,  -473,  -472,
     178,  -473,  -473,   267,  -473,  -113,     7,   112,   -23,   534,
    -473,    52,   469,   473,   333,   487,    -6,   340,   374,  -473,
    -473,   153,    62,  -203,  -473,  -165,  -473,  -473,  -473,   -98,
    -473,  -473,   -21,   -95,   181,   126,  -473,   -94,   -86,   -57,
      39,    43,    82,  -473,  -233,   329,    10,  -473
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     3,     4,     5,    71,     6,   243,   154,   341,   400,
     242,   337,   134,   324,   218,    34,    35,   532,   515,   533,
      36,   314,   145,   315,    37,    38,   250,   247,   464,   404,
      39,    40,    41,    42,    95,   169,    43,   226,    44,    45,
      82,    83,   221,   232,   385,   240,   325,   107,   108,   292,
      46,    47,    48,   187,    49,   161,    50,   254,    78,    52,
     455,   456,   122,   123,   124,   115,   112,   113,    99,   100,
     101,   450,   451,    53,   188,   189,   190,   452,   457,    54,
      55,    56,    57,    58,    59,    60,   516,    61,    62,    63,
      64,    65,    66,    67,   102,   131,   132,    68
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      81,    96,   401,   105,   212,   185,   117,   165,   191,   339,
     121,   192,   195,   238,   408,   293,    97,   116,   152,   104,
     196,   273,   239,     7,   180,    74,   126,   142,   208,   234,
     260,   144,   150,   140,   141,   234,   214,   214,    70,   281,
      75,    75,  -225,   363,   409,  -225,   303,   216,   538,   197,
     539,   304,   261,   153,     7,    29,    76,  -222,  -222,   139,
      13,   143,   209,  -225,    77,    77,   155,   121,   158,   157,
    -225,    96,   217,   305,   215,   252,   166,   167,   170,     7,
    -225,   364,   410,   312,   216,    30,    29,   310,   191,   104,
     106,   192,   195,     7,   231,   182,   311,     1,     2,   514,
     196,   237,    22,   393,   127,   178,   191,    20,     2,   192,
     195,    29,   358,    51,   276,   517,    20,   313,   196,   220,
     294,    51,   202,   295,   296,    29,   227,   318,    69,   197,
      72,   438,   297,   501,   220,   355,   233,   110,   529,   530,
      81,   220,  -290,   203,   222,   198,   223,   197,    13,   199,
     206,   518,   207,    13,   255,   272,   256,   234,   460,   461,
     258,   298,   137,   333,    51,   262,   263,   264,   339,   133,
     140,   268,    10,    30,   307,   309,   380,   191,    30,   191,
     192,   195,   192,   195,   536,   381,   146,   146,   200,   196,
     372,   196,   109,   111,   278,   304,   542,   543,   282,   234,
     284,   321,   285,   114,   114,    28,   234,   106,     7,   137,
      51,   302,   271,   125,   220,   220,    13,   373,   197,   320,
     197,   151,   322,   354,    20,   198,   118,   119,   120,   199,
     162,   164,   194,   334,   211,   377,   207,   147,   147,   163,
      29,    30,   171,   198,     7,    28,   148,   199,   149,   348,
     179,   137,   349,   350,   447,   448,   114,   299,   125,   449,
     181,   300,   182,   220,   379,   249,   183,   207,   200,   172,
     201,   146,   173,   140,   125,   204,    29,   146,   397,   245,
     398,   399,   365,   205,   246,   339,   200,   193,   259,   210,
     137,     7,   213,   265,   174,   142,   368,   370,   371,   234,
     301,   175,    13,   248,   220,   434,   382,   251,   383,   437,
     146,   224,   194,   225,   198,   368,   198,   293,   199,   293,
     199,   266,   147,    29,   274,   414,   465,    30,   147,   143,
     194,   488,   489,   244,   405,   275,   405,    84,   103,   411,
     491,   277,   492,   413,   494,   316,   492,   328,   136,   346,
     138,   146,    13,    14,   323,   326,   146,   200,   332,   200,
     421,   147,   495,   146,   214,   427,   340,   193,   342,   344,
     347,   444,   445,   512,   513,   351,   436,    30,   144,   496,
     352,   356,   317,   279,   280,   193,   175,   359,   114,   361,
     360,   286,   362,   366,   288,   176,   177,   125,   118,   146,
     367,   194,   147,   194,   469,   130,   319,   147,   103,   386,
      80,   387,   388,   473,   147,   474,   535,   110,   477,   486,
     487,   389,   294,   338,   294,   295,   296,   295,   296,   343,
     391,   485,   345,   394,   297,   146,   297,   228,   395,   153,
     407,   146,   412,   417,   550,   423,   524,   525,   146,   526,
     147,   419,   499,   500,   424,   405,   193,   502,   193,   253,
     420,   422,   425,   298,   428,   298,   429,   430,   506,   507,
     540,   508,   435,   329,   541,   442,   267,    84,   241,   443,
     146,   446,   463,   454,   462,   467,   147,   470,   475,   472,
     523,   471,   147,   257,   476,   527,   478,   479,   327,   147,
     482,   483,   528,   330,   331,   146,   484,   534,   270,   534,
     287,   498,   503,   146,   504,   146,   509,   392,   146,   184,
     545,   534,   534,   406,   544,   406,   520,   522,   531,   547,
     548,   147,   549,   146,    73,   396,   546,   490,   235,   135,
     390,   146,   357,    98,   418,   374,   159,   283,   521,   289,
     160,   453,   241,   269,   519,   146,   147,   146,     0,   299,
       0,   299,   156,   300,   147,   300,   147,     0,     0,   147,
       0,     0,     0,     0,     0,   375,   376,     0,     0,     0,
       0,     0,   338,   459,   147,     0,     0,   466,     0,     0,
     468,     0,   147,     0,     0,     0,     0,   146,     0,     0,
       0,     0,   301,     0,   301,     0,   147,     0,   147,   146,
       0,   146,     0,     0,     0,   431,     0,     0,     0,     0,
     415,   416,     0,   439,     0,   440,     0,     0,   441,     0,
       0,     0,     0,   493,     0,     0,     0,     0,   497,     0,
       0,   241,     0,     0,   406,   432,   433,     0,   147,     0,
       0,     0,   241,     0,     0,   241,     0,     0,     0,     0,
     147,   241,   147,     0,     0,   480,     0,   481,     0,     7,
       0,     8,     0,    79,     0,     0,    10,     0,    11,    12,
      13,    14,    15,    16,    17,    18,    19,     0,    20,    21,
      22,     0,     0,     0,    24,    25,    26,    27,     0,   338,
      28,    29,     0,     0,     0,    30,     0,   505,     0,     0,
      31,     0,     0,     7,     0,     8,     0,    79,    33,   510,
       0,   511,    11,    12,    13,    14,    15,    16,    17,    18,
      19,     0,    20,    21,    22,    23,     0,     0,    24,    25,
      26,    27,     0,    85,    28,    29,     0,     0,     0,    30,
      86,     0,    87,    80,    88,    89,    90,    91,    92,    93,
      94,     7,    33,     8,     0,    79,     0,     0,     0,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    19,     0,
      20,    21,     0,     0,     0,     0,    24,    25,    26,    27,
       0,    85,    28,    29,     0,     0,     0,    30,    86,     0,
      87,    80,    88,    89,    90,    91,    92,   129,    94,     7,
      33,     8,     0,     9,     0,     0,    10,     0,    11,    12,
      13,    14,    15,    16,    17,    18,    19,     0,    20,    21,
      22,    23,     0,     0,    24,    25,    26,    27,     0,     0,
      28,    29,     0,     0,     0,    30,     0,     0,     0,     0,
      31,     0,     7,     0,     8,    32,    79,   236,    33,     0,
       0,    11,    12,    13,    14,    15,    16,    17,    18,    19,
       0,    20,    21,     0,     0,     0,     0,    24,    25,    26,
      27,     0,     0,    28,    29,     0,     0,     0,    30,     0,
       0,     0,    80,    31,     0,     7,   230,     8,     0,    79,
     308,    33,     0,     0,    11,    12,    13,    14,    15,    16,
      17,    18,    19,     0,    20,    21,     0,     0,     0,     0,
      24,    25,    26,    27,     0,     0,    28,    29,     0,     0,
       0,    30,     0,     0,     0,    80,    31,     0,     7,   230,
       8,     0,    79,   378,    33,     0,     0,    11,    12,    13,
      14,    15,    16,    17,    18,    19,     0,    20,    21,     0,
       0,     0,     0,    24,    25,    26,    27,     0,     0,    28,
      29,     0,     0,     0,    30,     0,     0,     0,    80,    31,
       0,     7,   230,     8,     0,    79,   229,    33,     0,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    19,     0,
      20,    21,     0,     0,     0,     0,    24,    25,    26,    27,
       0,     0,    28,    29,     0,     0,     0,    30,     0,     0,
       0,     0,    31,     0,     7,   230,     8,     0,    79,   353,
      33,     0,     0,    11,    12,    13,    14,    15,    16,    17,
      18,    19,     0,    20,    21,     0,     0,     0,     0,    24,
      25,    26,    27,     0,     0,    28,    29,     0,     0,     0,
      30,     0,     0,     0,     0,    31,     0,     7,   230,     8,
       0,    79,     0,    33,     0,     0,    11,    12,    13,    14,
      15,    16,    17,    18,    19,     0,    20,    21,     0,     0,
       0,     0,    24,    25,    26,    27,     0,     0,    28,    29,
       0,     0,     0,    30,     0,     0,     0,    80,    31,     0,
       7,   384,     8,     0,    79,     0,    33,     0,     0,    11,
      12,    13,    14,    15,    16,    17,    18,    19,     0,    20,
      21,   402,     0,     0,     0,    24,    25,    26,    27,     0,
       0,    28,    29,     0,     0,     0,    30,   403,     0,     0,
       0,    31,     7,     0,     8,     0,    79,     0,     0,    33,
       0,    11,    12,    13,    14,    15,    16,    17,    18,    19,
       0,    20,    21,     0,     0,     0,     0,    24,    25,    26,
      27,     0,     0,    28,    29,     0,     0,     0,    30,     0,
       0,     0,    80,    31,     7,     0,     8,     0,    79,     0,
       0,    33,     0,    11,    12,    13,    14,    15,    16,    17,
      18,    19,     0,    20,    21,     0,     0,     0,     0,    24,
      25,    26,    27,     0,   168,    28,    29,     0,     0,     0,
      30,     0,     0,     0,     0,    31,     7,     0,     8,     0,
      79,     0,     0,    33,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    19,     0,    20,    21,     0,     0,     0,
       0,    24,    25,    26,    27,     0,   219,    28,    29,     0,
       0,     0,    30,     0,     0,     0,     0,    31,     7,     0,
       8,     0,    79,     0,     0,    33,     0,    11,    12,    13,
      14,    15,    16,    17,    18,    19,     0,    20,    21,     0,
       0,     0,     0,    24,    25,    26,    27,     0,   306,    28,
      29,     0,     0,     0,    30,     0,     0,     0,     0,    31,
       7,     0,     8,   369,    79,     0,     0,    33,     0,    11,
      12,    13,    14,    15,    16,    17,    18,    19,     0,    20,
      21,     0,     0,     0,     0,    24,    25,    26,    27,     0,
       0,    28,    29,     0,     0,     0,    30,     0,     0,     0,
       0,    31,     7,     0,     8,   426,    79,     0,     0,    33,
       0,    11,    12,    13,    14,    15,    16,    17,    18,    19,
       0,    20,    21,     0,     0,     0,     0,    24,    25,    26,
      27,     0,     0,    28,    29,     0,     0,     0,    30,     0,
       0,     0,     0,    31,     0,     7,     0,     8,     0,    79,
       0,    33,   531,     0,    11,    12,    13,    14,    15,    16,
      17,    18,    19,     0,    20,    21,     0,     0,     0,     0,
      24,    25,    26,    27,     0,     0,    28,    29,     0,     0,
       0,    30,     0,     0,     0,     0,    31,     7,     0,     8,
       0,    79,     0,     0,    33,     0,    11,    12,    13,    14,
      15,    16,    17,    18,    19,     0,    20,    21,     0,     0,
     335,     0,    24,    25,    26,    27,     0,     0,    28,    29,
      13,     0,   458,    30,     0,     0,     0,     0,    31,     0,
       0,     0,    13,     0,   537,     0,    33,     0,     0,     0,
       0,     0,     0,     0,    13,    30,     0,     0,    87,   184,
     128,    89,    90,   336,    92,   129,    94,    30,     0,     0,
      87,     0,   128,    89,    90,   336,    92,   129,    94,    30,
       0,     0,    87,     0,   128,    89,    90,   336,    92,   129,
      94,     0,    87,    80,   128,    89,    90,    91,    92,   129,
      94,   130,   186,     0,     0,     0,     0,     0,     0,     0,
       0,    13,    14,    15,    16,    17,    18,    19,     0,    20,
       0,     0,     0,     0,     0,    24,    25,    26,     0,     0,
       0,    28,     0,     0,   290,     0,    30,     0,     0,   291,
       0,    31,    14,    15,    16,    17,    18,    19,    13,     0,
       0,     0,     0,     0,     0,    24,    25,    26,     0,     0,
       0,    28,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    31,     0,    30,     0,     0,    87,     0,   128,    89,
      90,    91,    92,   129,    94,   291,     0,     0,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,     0,     0,
       0,    24,    25,    26,     0,     0,     0,    28,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    31,    87,    80,
     128,    89,    90,    91,    92,   129,    94,   130
};

static const yytype_int16 yycheck[] =
{
       8,     9,   341,    10,   117,   105,    29,    83,   106,   242,
      31,   106,   106,   149,   344,   218,     9,    23,     5,     9,
     106,   186,   149,     3,   100,     9,    33,     7,     7,   144,
      11,    59,    60,    54,    55,   150,    11,    11,     0,   204,
      25,    25,    11,    11,    11,    14,     6,   123,   520,   106,
     522,    11,    33,    40,     3,    35,    40,    32,    32,    52,
      14,    41,    41,    32,    49,    49,    74,    88,    76,    75,
      39,    79,    41,    33,    49,    49,    84,    85,    86,     3,
      49,    49,    49,     7,   160,    39,    35,   223,   186,    79,
       5,   186,   186,     3,   142,   103,   223,    37,    48,    21,
     186,   149,    24,   336,     5,    98,   204,    22,    48,   204,
     204,    35,   277,     1,   190,   489,    22,    41,   204,   127,
     218,     9,    11,   218,   218,    35,   133,   225,     2,   186,
       4,    41,   218,   463,   142,   271,   143,    11,   512,   513,
     148,   149,     8,    32,     5,   106,     7,   204,    14,   106,
       9,   490,    11,    14,   162,   183,   164,   272,    39,    40,
     168,   218,    50,   239,    52,   173,   174,   175,   401,     9,
     191,   179,    10,    39,   222,   223,   312,   275,    39,   277,
     275,   275,   277,   277,   517,   312,    59,    60,   106,   275,
       6,   277,    11,    12,   201,    11,   529,   530,   205,   314,
     208,   229,   209,    22,    23,    34,   321,     5,     3,    97,
      98,   219,     7,    32,   222,   223,    14,    33,   275,   226,
     277,     3,   230,   271,    22,   186,    15,    16,    17,   186,
      40,    11,   106,   241,     9,   311,    11,    59,    60,     6,
      35,    39,     8,   204,     3,    34,     5,   204,     7,   257,
      11,   139,   260,   261,    45,    46,    75,   218,    77,    50,
       8,   218,   270,   271,   312,     9,     8,    11,   186,     8,
      41,   144,    11,   294,    93,     5,    35,   150,    24,   153,
      26,    27,   290,    41,     3,   518,   204,   106,     8,    32,
     178,     3,    32,     8,    33,     7,   304,   305,   306,   414,
     218,    40,    14,    32,   312,   381,   313,    32,   316,   385,
     183,   130,   186,    51,   275,   323,   277,   520,   275,   522,
     277,     8,   144,    35,     6,   353,    38,    39,   150,    41,
     204,   444,   445,   152,   342,    11,   344,     8,     9,   347,
       9,     5,    11,   351,     9,     7,    11,     6,    50,     4,
      52,   224,    14,    15,    11,    11,   229,   275,    11,   277,
     367,   183,     9,   236,    11,   373,     6,   186,    11,    11,
      40,   394,   395,   486,   487,    33,   384,    39,   406,   455,
       8,     6,    44,   202,   203,   204,    40,     6,   207,     9,
       8,   210,     4,     6,   213,    97,    98,   216,    15,   272,
      41,   275,   224,   277,   412,    51,   225,   229,    79,     8,
      43,     8,     8,   420,   236,   422,   516,   291,   425,   442,
     443,     8,   520,   242,   522,   520,   520,   522,   522,   248,
       8,   438,   251,     3,   520,   308,   522,   139,     3,    40,
       9,   314,    33,     6,   544,     6,   499,   500,   321,   502,
     272,     8,   460,   461,     6,   463,   275,   465,   277,   161,
      41,    41,    41,   520,     8,   522,     8,     8,   475,   476,
     523,   478,     8,   236,   527,     3,   178,   148,   149,     3,
     353,     3,    11,    45,     4,     4,   308,     8,    41,    49,
     498,     8,   314,   164,    41,   503,    41,     6,   232,   321,
       8,     8,   509,   237,   238,   378,     8,   515,   179,   517,
     212,    41,    38,   386,    49,   388,    41,   336,   391,     9,
       4,   529,   530,   342,   531,   344,    41,    41,    10,     4,
       4,   353,     4,   406,     5,   340,   534,   446,   148,    45,
     332,   414,   275,     9,   363,   308,    77,   207,   496,   216,
      77,   398,   223,   179,   492,   428,   378,   430,    -1,   520,
      -1,   522,    75,   520,   386,   522,   388,    -1,    -1,   391,
      -1,    -1,    -1,    -1,    -1,   309,   310,    -1,    -1,    -1,
      -1,    -1,   401,   402,   406,    -1,    -1,   406,    -1,    -1,
     409,    -1,   414,    -1,    -1,    -1,    -1,   470,    -1,    -1,
      -1,    -1,   520,    -1,   522,    -1,   428,    -1,   430,   482,
      -1,   484,    -1,    -1,    -1,   378,    -1,    -1,    -1,    -1,
     354,   355,    -1,   386,    -1,   388,    -1,    -1,   391,    -1,
      -1,    -1,    -1,   452,    -1,    -1,    -1,    -1,   457,    -1,
      -1,   312,    -1,    -1,   463,   379,   380,    -1,   470,    -1,
      -1,    -1,   323,    -1,    -1,   326,    -1,    -1,    -1,    -1,
     482,   332,   484,    -1,    -1,   428,    -1,   430,    -1,     3,
      -1,     5,    -1,     7,    -1,    -1,    10,    -1,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    22,    23,
      24,    -1,    -1,    -1,    28,    29,    30,    31,    -1,   518,
      34,    35,    -1,    -1,    -1,    39,    -1,   470,    -1,    -1,
      44,    -1,    -1,     3,    -1,     5,    -1,     7,    52,   482,
      -1,   484,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    -1,    22,    23,    24,    25,    -1,    -1,    28,    29,
      30,    31,    -1,    33,    34,    35,    -1,    -1,    -1,    39,
      40,    -1,    42,    43,    44,    45,    46,    47,    48,    49,
      50,     3,    52,     5,    -1,     7,    -1,    -1,    -1,    -1,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
      22,    23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,
      -1,    33,    34,    35,    -1,    -1,    -1,    39,    40,    -1,
      42,    43,    44,    45,    46,    47,    48,    49,    50,     3,
      52,     5,    -1,     7,    -1,    -1,    10,    -1,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    22,    23,
      24,    25,    -1,    -1,    28,    29,    30,    31,    -1,    -1,
      34,    35,    -1,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      44,    -1,     3,    -1,     5,    49,     7,     8,    52,    -1,
      -1,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      -1,    22,    23,    -1,    -1,    -1,    -1,    28,    29,    30,
      31,    -1,    -1,    34,    35,    -1,    -1,    -1,    39,    -1,
      -1,    -1,    43,    44,    -1,     3,    47,     5,    -1,     7,
       8,    52,    -1,    -1,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    -1,    22,    23,    -1,    -1,    -1,    -1,
      28,    29,    30,    31,    -1,    -1,    34,    35,    -1,    -1,
      -1,    39,    -1,    -1,    -1,    43,    44,    -1,     3,    47,
       5,    -1,     7,     8,    52,    -1,    -1,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    -1,    22,    23,    -1,
      -1,    -1,    -1,    28,    29,    30,    31,    -1,    -1,    34,
      35,    -1,    -1,    -1,    39,    -1,    -1,    -1,    43,    44,
      -1,     3,    47,     5,    -1,     7,     8,    52,    -1,    -1,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
      22,    23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,
      -1,    -1,    34,    35,    -1,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    44,    -1,     3,    47,     5,    -1,     7,     8,
      52,    -1,    -1,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    -1,    22,    23,    -1,    -1,    -1,    -1,    28,
      29,    30,    31,    -1,    -1,    34,    35,    -1,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    44,    -1,     3,    47,     5,
      -1,     7,    -1,    52,    -1,    -1,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    22,    23,    -1,    -1,
      -1,    -1,    28,    29,    30,    31,    -1,    -1,    34,    35,
      -1,    -1,    -1,    39,    -1,    -1,    -1,    43,    44,    -1,
       3,    47,     5,    -1,     7,    -1,    52,    -1,    -1,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    -1,    22,
      23,    24,    -1,    -1,    -1,    28,    29,    30,    31,    -1,
      -1,    34,    35,    -1,    -1,    -1,    39,    40,    -1,    -1,
      -1,    44,     3,    -1,     5,    -1,     7,    -1,    -1,    52,
      -1,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      -1,    22,    23,    -1,    -1,    -1,    -1,    28,    29,    30,
      31,    -1,    -1,    34,    35,    -1,    -1,    -1,    39,    -1,
      -1,    -1,    43,    44,     3,    -1,     5,    -1,     7,    -1,
      -1,    52,    -1,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    -1,    22,    23,    -1,    -1,    -1,    -1,    28,
      29,    30,    31,    -1,    33,    34,    35,    -1,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    44,     3,    -1,     5,    -1,
       7,    -1,    -1,    52,    -1,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    -1,    22,    23,    -1,    -1,    -1,
      -1,    28,    29,    30,    31,    -1,    33,    34,    35,    -1,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    44,     3,    -1,
       5,    -1,     7,    -1,    -1,    52,    -1,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    -1,    22,    23,    -1,
      -1,    -1,    -1,    28,    29,    30,    31,    -1,    33,    34,
      35,    -1,    -1,    -1,    39,    -1,    -1,    -1,    -1,    44,
       3,    -1,     5,     6,     7,    -1,    -1,    52,    -1,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    -1,    22,
      23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,    -1,
      -1,    34,    35,    -1,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    44,     3,    -1,     5,     6,     7,    -1,    -1,    52,
      -1,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      -1,    22,    23,    -1,    -1,    -1,    -1,    28,    29,    30,
      31,    -1,    -1,    34,    35,    -1,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    44,    -1,     3,    -1,     5,    -1,     7,
      -1,    52,    10,    -1,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    -1,    22,    23,    -1,    -1,    -1,    -1,
      28,    29,    30,    31,    -1,    -1,    34,    35,    -1,    -1,
      -1,    39,    -1,    -1,    -1,    -1,    44,     3,    -1,     5,
      -1,     7,    -1,    -1,    52,    -1,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    22,    23,    -1,    -1,
       4,    -1,    28,    29,    30,    31,    -1,    -1,    34,    35,
      14,    -1,     4,    39,    -1,    -1,    -1,    -1,    44,    -1,
      -1,    -1,    14,    -1,     4,    -1,    52,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    14,    39,    -1,    -1,    42,     9,
      44,    45,    46,    47,    48,    49,    50,    39,    -1,    -1,
      42,    -1,    44,    45,    46,    47,    48,    49,    50,    39,
      -1,    -1,    42,    -1,    44,    45,    46,    47,    48,    49,
      50,    -1,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,     5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    14,    15,    16,    17,    18,    19,    20,    -1,    22,
      -1,    -1,    -1,    -1,    -1,    28,    29,    30,    -1,    -1,
      -1,    34,    -1,    -1,     7,    -1,    39,    -1,    -1,    12,
      -1,    44,    15,    16,    17,    18,    19,    20,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    28,    29,    30,    -1,    -1,
      -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    44,    -1,    39,    -1,    -1,    42,    -1,    44,    45,
      46,    47,    48,    49,    50,    12,    -1,    -1,    15,    16,
      17,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    28,    29,    30,    -1,    -1,    -1,    34,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    37,    48,    55,    56,    57,    59,     3,     5,     7,
      10,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      22,    23,    24,    25,    28,    29,    30,    31,    34,    35,
      39,    44,    49,    52,    69,    70,    74,    78,    79,    84,
      85,    86,    87,    90,    92,    93,   104,   105,   106,   108,
     110,   111,   113,   127,   133,   134,   135,   136,   137,   138,
     139,   141,   142,   143,   144,   145,   146,   147,   151,   139,
       0,    58,   139,    59,     9,    25,    40,    49,   112,     7,
      43,    93,    94,    95,   149,    33,    40,    42,    44,    45,
      46,    47,    48,    49,    50,    88,    93,   110,   113,   122,
     123,   124,   148,   149,   150,    90,     5,   101,   102,   138,
     139,   138,   120,   121,   138,   119,   120,   112,    15,    16,
      17,   136,   116,   117,   118,   138,    90,     5,    44,    49,
      51,   149,   150,     9,    66,    74,    69,   111,    69,   110,
     136,   136,     7,    41,    75,    76,    79,   104,     5,     7,
      75,     3,     5,    40,    61,    93,   119,   120,    93,   116,
     117,   109,    40,     6,    11,    67,    93,    93,    33,    89,
      93,     8,     8,    11,    33,    40,    69,    69,   110,    11,
      67,     8,    93,     8,     9,    66,     5,   107,   128,   129,
     130,   133,   137,   138,   139,   141,   142,   143,   144,   145,
     146,    41,    11,    32,     5,    41,     9,    11,     7,    41,
      32,     9,   109,    32,    11,    49,    67,    41,    68,    33,
      93,    96,     5,     7,   138,    51,    91,    90,    69,     8,
      47,    96,    97,    90,    76,    94,     8,    96,    97,    98,
      99,   149,    64,    60,   138,   139,     3,    81,    32,     9,
      80,    32,    49,    69,   111,    93,    93,   149,    93,     8,
      11,    33,    93,    93,    93,     8,     8,    69,    93,   122,
     149,     7,    75,   129,     6,    11,    67,     5,    90,   138,
     138,   129,    90,   121,    93,    90,   138,    69,   138,   118,
       7,    12,   103,   127,   133,   137,   141,   142,   143,   144,
     145,   146,    93,     6,    11,    33,    33,    96,     8,    96,
      97,    98,     7,    41,    75,    77,     7,    44,   133,   138,
      90,    75,    93,    11,    67,   100,    11,   100,     6,    77,
     100,   100,    11,    67,    93,     4,    47,    65,   138,   148,
       6,    62,    11,   138,    11,   138,     4,    40,    93,    93,
      93,    33,     8,     8,    96,    97,     6,   107,   129,     6,
       8,     9,     4,    11,    49,    93,     6,    41,    93,     6,
      93,    93,     6,    33,    77,   100,   100,    67,     8,    96,
      97,    98,    90,    93,    47,    98,     8,     8,     8,     8,
      99,     8,   138,   148,     3,     3,    61,    24,    26,    27,
      63,    64,    24,    40,    83,    93,   138,     9,    83,    11,
      49,    93,    33,    93,    75,   100,   100,     6,   138,     8,
      41,    90,    41,     6,     6,    41,     6,    93,     8,     8,
       8,    77,   100,   100,    67,     8,    93,    67,    41,    77,
      77,    77,     3,     3,   112,   112,     3,    45,    46,    50,
     125,   126,   131,   125,    45,   114,   115,   132,     4,   138,
      39,    40,     4,    11,    82,    38,   138,     4,   138,    93,
       8,     8,    49,    90,    90,    41,    41,    90,    41,     6,
      77,    77,     8,     8,     8,    90,   112,   112,   109,   109,
      62,     9,    11,   138,     9,     9,    67,   138,    41,    93,
      93,    83,    93,    38,    49,    77,    90,    90,    90,    41,
      77,    77,   109,   109,    21,    72,   140,    72,    64,   126,
      41,   115,    41,    93,    82,    82,    82,    93,    90,    72,
      72,    10,    71,    73,    93,    66,    71,     4,   103,   103,
      82,    82,    71,    71,    90,     4,    73,     4,     4,     4,
      66
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    54,    55,    55,    55,    56,    56,    57,    57,    58,
      58,    59,    60,    60,    61,    61,    62,    62,    63,    63,
      63,    64,    64,    65,    65,    65,    65,    66,    66,    67,
      67,    68,    68,    69,    69,    70,    70,    70,    70,    70,
      70,    70,    71,    71,    72,    72,    73,    73,    74,    74,
      75,    75,    76,    76,    77,    77,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    80,
      79,    81,    79,    82,    82,    83,    83,    83,    83,    83,
      83,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    85,    85,    85,    86,
      86,    86,    87,    87,    87,    87,    87,    87,    88,    88,
      88,    88,    88,    89,    89,    89,    89,    89,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    91,    91,    91,    91,    92,    92,    93,    94,
      94,    95,    95,    95,    95,    95,    95,    96,    96,    97,
      97,    98,    98,    99,   100,   100,   101,   101,   102,   102,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   104,
     105,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     108,   108,   109,   109,   110,   110,   111,   112,   112,   112,
     112,   112,   112,   113,   113,   113,   113,   113,   114,   114,
     115,   116,   116,   117,   117,   118,   118,   118,   119,   119,
     120,   120,   121,   121,   121,   122,   122,   123,   123,   124,
     124,   125,   125,   126,   126,   127,   127,   128,   128,   129,
     129,   130,   130,   131,   131,   131,   131,   132,   132,   133,
     133,   134,   134,   135,   135,   136,   137,   137,   137,   137,
     137,   137,   138,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   148,   148,   148,   148,   148,   148,
     148,   149,   150,   150,   151
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
       4,     1,     1,     1,     1,     1,     1,     3,     2,     3,
       3,     4,     3,     1,     4,     1,     5,     5,     6,     7,
       7,     8,     6,     6,     7,     8,     8,     9,     2,     2,
       3,     5,     4,     2,     2,     3,     4,     5,     1,     1,
       1,     1,     5,     2,     4,     3,     4,     5,     7,     4,
       6,     7,     0,     2,     2,     4,     1,     3,     2,     0,
       2,     1,     3,     2,     3,     4,     5,     1,     3,     2,
       4,     1,     3,     2,     1,     3,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       5,     5,     0,     2,     1,     2,     3,     0,     3,     5,
       3,     5,     7,     3,     5,     3,     5,     7,     1,     3,
       4,     0,     1,     1,     3,     1,     3,     5,     0,     1,
       1,     3,     1,     3,     4,     3,     2,     1,     3,     0,
       2,     1,     3,     2,     4,     4,     5,     3,     4,     0,
       2,     1,     3,     0,     1,     1,     1,     0,     1,     1,
       2,     1,     2,     1,     2,     1,     1,     1,     2,     2,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1
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
#line 49 "lang/LangSource/Bison/lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; }
#line 2239 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 3: /* root: classextensions  */
#line 51 "lang/LangSource/Bison/lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; }
#line 2245 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 4: /* root: INTERPRET cmdlinecode  */
#line 53 "lang/LangSource/Bison/lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 2; }
#line 2251 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 5: /* classes: %empty  */
#line 56 "lang/LangSource/Bison/lang11d"
          { yyval = 0; }
#line 2257 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 6: /* classes: classes classdef  */
#line 58 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2263 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 8: /* classextensions: classextensions classextension  */
#line 63 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2269 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 9: /* classdef: classname superclass OPENCURLY classvardecls methods CLOSECURLY  */
#line 67 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrClassNode((yyloc), (PyrSlotNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1], 0);
				}
#line 2277 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 10: /* classdef: classname OPENSQUARE optname CLOSESQUARE superclass OPENCURLY classvardecls methods CLOSECURLY  */
#line 71 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrClassNode((yyloc),(PyrSlotNode*)yyvsp[-8], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1],
					(PyrSlotNode*)yyvsp[-6]);
				}
#line 2286 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 11: /* classextension: ADD classname OPENCURLY methods CLOSECURLY  */
#line 78 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrClassExtNode((yyloc),(PyrSlotNode*)yyvsp[-3], (PyrMethodNode*)yyvsp[-1]);
				}
#line 2294 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 12: /* optname: %empty  */
#line 83 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 2300 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 14: /* superclass: %empty  */
#line 87 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 2306 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 15: /* superclass: COLON classname  */
#line 89 "lang/LangSource/Bison/lang11d"
                                { yyval = yyvsp[0]; }
#line 2312 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 16: /* classvardecls: %empty  */
#line 92 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 2318 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 17: /* classvardecls: classvardecls classvardecl  */
#line 94 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2324 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 18: /* classvardecl: CLASSVAR rwslotdeflist SEMICOLON  */
#line 98 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], varClass); }
#line 2330 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 19: /* classvardecl: VAR rwslotdeflist SEMICOLON  */
#line 100 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], varInst); }
#line 2336 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 20: /* classvardecl: SC_CONST constdeflist SEMICOLON  */
#line 102 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], varConst); }
#line 2342 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 21: /* methods: %empty  */
#line 105 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 2348 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 22: /* methods: methods methoddef  */
#line 107 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2354 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 23: /* methoddef: name OPENCURLY argdecls funcvardecls primitive methbody CLOSECURLY  */
#line 111 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((yyloc), (PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2361 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 24: /* methoddef: MULTIPLY name OPENCURLY argdecls funcvardecls primitive methbody CLOSECURLY  */
#line 114 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((yyloc), (PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2368 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 25: /* methoddef: binop OPENCURLY argdecls funcvardecls primitive methbody CLOSECURLY  */
#line 117 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((yyloc), (PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2375 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 26: /* methoddef: MULTIPLY binop OPENCURLY argdecls funcvardecls primitive methbody CLOSECURLY  */
#line 120 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((yyloc), (PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2382 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 34: /* funcbody: exprseq funretval  */
#line 138 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrDropNode((yyloc), (PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2388 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 35: /* cmdlinecode: OPENPAREN argdecls1 funcvardecls1 funcbody CLOSEPAREN  */
#line 142 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), (PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2394 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 36: /* cmdlinecode: OPENPAREN argdecls1 funcbody CLOSEPAREN  */
#line 144 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), (PyrArgListNode*)yyvsp[-2], NULL, (PyrParseNode*)yyvsp[-1], false); }
#line 2400 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 37: /* cmdlinecode: OPENPAREN funcvardecls1 funcbody CLOSEPAREN  */
#line 146 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), NULL, (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2406 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 38: /* cmdlinecode: argdecls1 funcvardecls1 funcbody  */
#line 148 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), (PyrArgListNode*)yyvsp[-2], (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2412 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 39: /* cmdlinecode: argdecls1 funcbody  */
#line 150 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), (PyrArgListNode*)yyvsp[-1], NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2418 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 40: /* cmdlinecode: funcvardecls1 funcbody  */
#line 152 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), NULL, (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2424 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 41: /* cmdlinecode: funcbody  */
#line 154 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), NULL, NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2430 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 43: /* methbody: exprseq retval  */
#line 159 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrDropNode((yyloc), (PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2436 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 44: /* primitive: %empty  */
#line 162 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 2442 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 45: /* primitive: primname optsemi  */
#line 164 "lang/LangSource/Bison/lang11d"
                                { yyval = yyvsp[-1]; }
#line 2448 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 46: /* retval: %empty  */
#line 168 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((yyloc), NULL); }
#line 2454 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 47: /* retval: NONLOCALRETURN expr optsemi  */
#line 170 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((yyloc), (PyrParseNode*)yyvsp[-1]); }
#line 2460 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 48: /* funretval: %empty  */
#line 174 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrBlockReturnNode((yyloc)); }
#line 2466 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 49: /* funretval: NONLOCALRETURN expr optsemi  */
#line 176 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((yyloc), (PyrParseNode*)yyvsp[-1]); }
#line 2472 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 51: /* blocklist1: blocklist1 blocklistitem  */
#line 181 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
				}
#line 2480 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 54: /* blocklist: %empty  */
#line 190 "lang/LangSource/Bison/lang11d"
                        { yyval = 0; }
#line 2486 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 56: /* msgsend: name blocklist1  */
#line 195 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2494 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 57: /* msgsend: OPENPAREN binop2 CLOSEPAREN blocklist1  */
#line 199 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2502 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 58: /* msgsend: name OPENPAREN CLOSEPAREN blocklist1  */
#line 203 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-3], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2510 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 59: /* msgsend: name OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 207 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2519 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 60: /* msgsend: OPENPAREN binop2 CLOSEPAREN OPENPAREN CLOSEPAREN blocklist1  */
#line 212 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-4], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2527 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 61: /* msgsend: OPENPAREN binop2 CLOSEPAREN OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 216 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2536 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 62: /* msgsend: name OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 221 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)yyvsp[-2])) {
					SetRaw(&((PyrPushNameNode*)yyvsp[-2])->mSlot, s_this);
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = linkAfterHead(
					(PyrParseNode*)yyvsp[-2],
					newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[-4], NULL));
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, (PyrParseNode*)yyvsp[-1], 0);
			}
#line 2558 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 63: /* msgsend: OPENPAREN binop2 CLOSEPAREN OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 239 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_performList);
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = linkAfterHead(
					(PyrParseNode*)yyvsp[-2],
					newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[-5], NULL));
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, (PyrParseNode*)yyvsp[-1], 0);
			}
#line 2575 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 64: /* msgsend: classname OPENSQUARE arrayelems CLOSESQUARE  */
#line 252 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrDynListNode((yyloc), (PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 2581 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 65: /* msgsend: classname blocklist1  */
#line 254 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = (PyrParseNode*)newPyrPushNameNode((yyloc), (PyrSlotNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, (PyrParseNode*)yyvsp[0]);
			}
#line 2596 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 66: /* msgsend: classname OPENPAREN CLOSEPAREN blocklist  */
#line 265 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = (PyrParseNode*)newPyrPushNameNode((yyloc), (PyrSlotNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2611 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 67: /* msgsend: classname OPENPAREN keyarglist1 optcomma CLOSEPAREN blocklist  */
#line 276 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = (PyrParseNode*)newPyrPushNameNode((yyloc), (PyrSlotNode*)yyvsp[-5]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, (PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2626 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 68: /* msgsend: classname OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 287 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((yyloc), (PyrSlotNode*)yyvsp[-5]),
					(PyrParseNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2643 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 69: /* msgsend: classname OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 300 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode, *selectornode2;
				PyrSlot slot, slot2;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)yyvsp[-4])) {
					SetRaw(&((PyrPushNameNode*)yyvsp[-4])->mSlot, s_this);
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				SetSymbol(&slot2, s_new);
				selectornode = newPyrSlotNode((yyloc), &slot);
				selectornode2 = newPyrSlotNode((yyloc), &slot2);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((yyloc), (PyrSlotNode*)yyvsp[-4]),
					newPyrPushLitNode((yyloc), selectornode2, NULL));
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-2]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, (PyrParseNode*)yyvsp[0], 0);
			}
#line 2668 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 70: /* msgsend: expr DOT OPENPAREN CLOSEPAREN blocklist  */
#line 321 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode((yyloc), &slot);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, (PyrParseNode*)yyvsp[-4], NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2681 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 71: /* msgsend: expr DOT OPENPAREN keyarglist1 optcomma CLOSEPAREN blocklist  */
#line 330 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode((yyloc), &slot);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, (PyrParseNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2694 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 72: /* msgsend: expr DOT name OPENPAREN keyarglist1 optcomma CLOSEPAREN blocklist  */
#line 339 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-7],
					(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2703 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 73: /* msgsend: expr DOT OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 344 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-6],
					(PyrParseNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2720 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 74: /* msgsend: expr DOT OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 358 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot, slot2;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)yyvsp[-5])) {
					SetRaw(&((PyrPushNameNode*)yyvsp[-5])->mSlot, s_this);
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				SetSymbol(&slot2, s_value);
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-5],
					newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &slot2), NULL));
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-2]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, (PyrParseNode*)yyvsp[-1], 0);
			}
#line 2744 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 75: /* msgsend: expr DOT name OPENPAREN CLOSEPAREN blocklist  */
#line 380 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-5], NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2752 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 76: /* msgsend: expr DOT name OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 384 "lang/LangSource/Bison/lang11d"
                        {
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-5], args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2762 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 77: /* msgsend: expr DOT name OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 390 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				if (isSuperObjNode((PyrParseNode*)yyvsp[-6])) {
					SetRaw(&((PyrPushNameNode*)yyvsp[-6])->mSlot, s_this);
					SetSymbol(&slot, s_superPerformList);
				} else {
					SetSymbol(&slot, s_performList);
				}
				selectornode = newPyrSlotNode((yyloc), &slot);

				args = linkNextNode((PyrParseNode*)yyvsp[-6], newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[-4], NULL));
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-2]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, (PyrParseNode*)yyvsp[-1], 0);
			}
#line 2784 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 78: /* msgsend: expr DOT name blocklist  */
#line 408 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[-3], 0, (PyrParseNode*)yyvsp[0]);
			}
#line 2792 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 79: /* $@1: %empty  */
#line 413 "lang/LangSource/Bison/lang11d"
                                    { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 1); }
#line 2798 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 80: /* generator: OPENCURLY COLON exprseq $@1 COMMA qual CLOSECURLY  */
#line 414 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, getsym("r"));
				PyrSlotNode* selectornode = newPyrSlotNode((yyloc), &slot);

				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode((yyloc), 0, 0, (PyrParseNode*)yyvsp[-1], false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode((yyloc), NULL, block);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, (PyrParseNode*)blocklit, 0, 0);
			}
#line 2812 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 81: /* $@2: %empty  */
#line 423 "lang/LangSource/Bison/lang11d"
                                              { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 2); }
#line 2818 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 82: /* generator: OPENCURLY SEMICOLON exprseq $@2 COMMA qual CLOSECURLY  */
#line 424 "lang/LangSource/Bison/lang11d"
                        {
				yyval = yyvsp[-1];
			}
#line 2826 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 83: /* nextqual: %empty  */
#line 430 "lang/LangSource/Bison/lang11d"
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
							PyrSlotNode* selectornode = newPyrSlotNode((yyloc), &slot);

							yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, expr, 0, 0);
						} break;
						case 2 :
						{
							yyval = (intptr_t)expr;
						} break;
					}
				}
#line 2852 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 84: /* nextqual: COMMA qual  */
#line 452 "lang/LangSource/Bison/lang11d"
                                { yyval = yyvsp[0]; }
#line 2858 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 85: /* qual: name LEFTARROW exprseq nextqual  */
#line 456 "lang/LangSource/Bison/lang11d"
                        {
				// later should check if exprseq is a series and optimize it to for loop
				PyrParseNode *exprseq = (PyrParseNode*)yyvsp[-1];
				if (exprseq->mClassno == pn_CallNode) {
					PyrCallNode *callnode = (PyrCallNode*)exprseq;
					if (slotRawSymbol(&callnode->mSelector->mSlot) == s_series)
					{
						SetSymbol(&callnode->mSelector->mSlot, getsym("forSeries"));

						PyrVarDefNode* var = newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-3], NULL, 0);
						PyrArgListNode* args = newPyrArgListNode((yyloc), var, NULL, NULL);
						PyrParseNode *block = (PyrParseNode*)newPyrBlockNode((yyloc), args, 0, (PyrParseNode*)yyvsp[0], false);
						PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode((yyloc), NULL, block);

						callnode->mArglist = linkNextNode(callnode->mArglist, blocklit);
						yyval = (intptr_t)callnode;

					} else goto notoptimized1;
				} else {
					notoptimized1:
					PyrSlot slot;
					SetSymbol(&slot, getsym("do"));
					PyrSlotNode* selectornode = newPyrSlotNode((yyloc), &slot);

					PyrVarDefNode* var = newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-3], NULL, 0);
					PyrArgListNode* args = newPyrArgListNode((yyloc), var, NULL, NULL);
					PyrParseNode *block = (PyrParseNode*)newPyrBlockNode((yyloc), args, 0, (PyrParseNode*)yyvsp[0], false);
					PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode((yyloc), NULL, block);

					PyrParseNode* args2 = linkNextNode(exprseq, blocklit);
					yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args2, 0, 0);
				}
			}
#line 2896 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 86: /* qual: name name LEFTARROW exprseq nextqual  */
#line 490 "lang/LangSource/Bison/lang11d"
                        {
				// later should check if exprseq is a series and optimize it to for loop
				PyrParseNode *exprseq = (PyrParseNode*)yyvsp[-1];
				if (exprseq->mClassno == pn_CallNode) {
					PyrCallNode *callnode = (PyrCallNode*)exprseq;
					if (slotRawSymbol(&callnode->mSelector->mSlot) == s_series)
					{
						SetSymbol(&callnode->mSelector->mSlot, getsym("forSeries"));

						PyrVarDefNode* var1 = newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-4], NULL, 0);
						PyrVarDefNode* var2 = newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-3], NULL, 0);
						PyrVarDefNode* vars = (PyrVarDefNode*)linkNextNode(var1, var2);
						PyrArgListNode* args = newPyrArgListNode((yyloc), vars, NULL, NULL);
						PyrParseNode *block = (PyrParseNode*)newPyrBlockNode((yyloc), args, 0, (PyrParseNode*)yyvsp[0], false);
						PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode((yyloc), NULL, block);

						callnode->mArglist = linkNextNode(callnode->mArglist, blocklit);
						yyval = (intptr_t)callnode;

					} else goto notoptimized2;
				} else {
					notoptimized2:
					PyrSlot slot;
					SetSymbol(&slot, getsym("do"));
					PyrSlotNode* selectornode = newPyrSlotNode((yyloc), &slot);

					PyrVarDefNode* var1 = newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-4], NULL, 0);
					PyrVarDefNode* var2 = newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-3], NULL, 0);
					PyrVarDefNode* vars = (PyrVarDefNode*)linkNextNode(var1, var2);
					PyrArgListNode* args = newPyrArgListNode((yyloc), vars, NULL, NULL);
					PyrParseNode *block = (PyrParseNode*)newPyrBlockNode((yyloc), args, 0, (PyrParseNode*)yyvsp[0], false);
					PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode((yyloc), NULL, block);

					PyrParseNode* args2 = linkNextNode(exprseq, blocklit);
					yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args2, 0, 0);
				}
			}
#line 2938 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 87: /* qual: VAR name EQUALSSIGN exprseq nextqual  */
#line 528 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, s_value);
				PyrSlotNode* selectornode = newPyrSlotNode((yyloc), &slot);

				PyrVarDefNode* var = newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-3], NULL, 0);
				PyrArgListNode* args = newPyrArgListNode((yyloc), var, NULL, NULL);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode((yyloc), args, 0, (PyrParseNode*)yyvsp[0], false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode((yyloc), NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode(blocklit, (PyrParseNode*)yyvsp[-1]);

				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args2, 0, 0);
			}
#line 2956 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 88: /* qual: exprseq nextqual  */
#line 542 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, getsym("if"));
				PyrSlotNode* selectornode = newPyrSlotNode((yyloc), &slot);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode((yyloc), 0, 0, (PyrParseNode*)yyvsp[0], false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode((yyloc), NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-1], blocklit);

				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args2, 0, 0);
			}
#line 2971 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 89: /* qual: COLON COLON exprseq nextqual  */
#line 553 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((yyloc), (PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
			}
#line 2979 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 90: /* qual: COLON WHILE exprseq nextqual  */
#line 557 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, getsym("alwaysYield"));
				PyrSlotNode* selectornode1 = newPyrSlotNode((yyloc), &slot);

				SetSymbol(&slot, getsym("if"));
				PyrSlotNode* selectornode2 = newPyrSlotNode((yyloc), &slot);

				SetNil(&slot);
				PyrParseNode *pushnil = (PyrParseNode*)newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &slot), NULL);

				PyrParseNode *yieldNil = (PyrParseNode*)newPyrCallNode((yyloc), selectornode1, pushnil, 0, 0);

				PyrParseNode *block1 = (PyrParseNode*)newPyrBlockNode((yyloc), 0, 0, yieldNil, false);
				PyrParseNode *blocklit1 = (PyrParseNode*)newPyrPushLitNode((yyloc), NULL, block1);
				PyrParseNode *block2 = (PyrParseNode*)newPyrBlockNode((yyloc), 0, 0, (PyrParseNode*)yyvsp[0], false);
				PyrParseNode *blocklit2 = (PyrParseNode*)newPyrPushLitNode((yyloc), NULL, block2);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-1], blocklit2);
				PyrParseNode* args3 = (PyrParseNode*)linkNextNode(args2, blocklit1);

				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode2, args3, 0, 0);
			}
#line 3006 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 97: /* expr1: OPENPAREN exprseq CLOSEPAREN  */
#line 588 "lang/LangSource/Bison/lang11d"
                        {
				PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
				node->mParens = 1;
				yyval = yyvsp[-1];
				node->location = (yyloc);
			}
#line 3017 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 98: /* expr1: TILDE name  */
#line 595 "lang/LangSource/Bison/lang11d"
                        {
				PyrParseNode* argnode;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL);
				SetSymbol(&slot, s_envirGet);
				selectornode = newPyrSlotNode((yyloc), &slot);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, argnode, 0, 0);
			}
#line 3031 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 99: /* expr1: OPENSQUARE arrayelems CLOSESQUARE  */
#line 605 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrDynListNode((yyloc), 0, (PyrParseNode*)yyvsp[-1]); }
#line 3037 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 100: /* expr1: OPENPAREN valrange2 CLOSEPAREN  */
#line 607 "lang/LangSource/Bison/lang11d"
                        { yyval = yyvsp[-1]; }
#line 3043 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 101: /* expr1: OPENPAREN COLON valrange3 CLOSEPAREN  */
#line 609 "lang/LangSource/Bison/lang11d"
                        { yyval = yyvsp[-1]; }
#line 3049 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 102: /* expr1: OPENPAREN dictslotlist CLOSEPAREN  */
#line 611 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrDynDictNode((yyloc), (PyrParseNode*)yyvsp[-1]); }
#line 3055 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 103: /* expr1: pseudovar  */
#line 613 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrPushNameNode((yyloc), (PyrSlotNode*)yyvsp[0]); }
#line 3061 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 104: /* expr1: expr1 OPENSQUARE arglist1 CLOSESQUARE  */
#line 615 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-3],
					(PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3078 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 106: /* valrangex1: expr1 OPENSQUARE arglist1 DOTDOT CLOSESQUARE  */
#line 631 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)yyvsp[-2]);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-2]);
					compileErrors++;
				}

				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[-2]);
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3109 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 107: /* valrangex1: expr1 OPENSQUARE DOTDOT exprseq CLOSESQUARE  */
#line 658 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode1 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
				nilnode2 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-4], nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3131 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 108: /* valrangex1: expr1 OPENSQUARE arglist1 DOTDOT exprseq CLOSESQUARE  */
#line 676 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)yyvsp[-3]);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-3]);
					compileErrors++;
				}

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-3]);
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3160 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 109: /* valrangeassign: expr1 OPENSQUARE arglist1 DOTDOT CLOSESQUARE EQUALSSIGN expr  */
#line 703 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)yyvsp[-4]);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-4]);
					compileErrors++;
				}

				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-4]);
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3192 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 110: /* valrangeassign: expr1 OPENSQUARE DOTDOT exprseq CLOSESQUARE EQUALSSIGN expr  */
#line 731 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode1 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
				nilnode2 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-6], nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-3]);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3215 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 111: /* valrangeassign: expr1 OPENSQUARE arglist1 DOTDOT exprseq CLOSESQUARE EQUALSSIGN expr  */
#line 750 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)yyvsp[-5]);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-5]);
					compileErrors++;
				}

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-5]);
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-3]);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3245 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 112: /* valrangexd: expr DOT OPENSQUARE arglist1 DOTDOT CLOSESQUARE  */
#line 778 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)yyvsp[-2]);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-3]);
					compileErrors++;
				}

				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-2]);
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3276 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 113: /* valrangexd: expr DOT OPENSQUARE DOTDOT exprseq CLOSESQUARE  */
#line 805 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode1 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
				nilnode2 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-5], nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3298 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 114: /* valrangexd: expr DOT OPENSQUARE arglist1 DOTDOT exprseq CLOSESQUARE  */
#line 823 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)yyvsp[-3]);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-4]);
					compileErrors++;
				}

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-3]);
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3327 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 115: /* valrangexd: expr DOT OPENSQUARE arglist1 DOTDOT CLOSESQUARE EQUALSSIGN expr  */
#line 848 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)yyvsp[-4]);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-5]);
					compileErrors++;
				}

				SetNil(&nilSlot);
				nilnode2 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-4]);
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3359 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 116: /* valrangexd: expr DOT OPENSQUARE DOTDOT exprseq CLOSESQUARE EQUALSSIGN expr  */
#line 876 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode1 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
				nilnode2 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-7], nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-3]);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3382 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 117: /* valrangexd: expr DOT OPENSQUARE arglist1 DOTDOT exprseq CLOSESQUARE EQUALSSIGN expr  */
#line 895 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode1;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				int arglen = nodeListLength((PyrParseNode*)yyvsp[-5]);
				if (arglen > 2) {
					error("ArrayedCollection subrange has too many arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-6]);
					compileErrors++;
				}

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-8], (PyrParseNode*)yyvsp[-5]);
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-3]);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3412 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 118: /* valrange2: exprseq DOTDOT  */
#line 923 "lang/LangSource/Bison/lang11d"
                        {
				// if this is not used in a 'do' or list comprehension, then should return an error.
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
				nilnode2 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);

				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-1], nilnode);
				args = linkNextNode(args, nilnode2);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3434 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 119: /* valrange2: DOTDOT exprseq  */
#line 942 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode, *zeronode;
				PyrSlot selectorSlot, nilSlot, zeroSlot;
				PyrParseNode* args;

				SetInt(&zeroSlot, 0);
				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
				zeronode = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &zeroSlot), NULL);

				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode(zeronode, nilnode);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3456 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 120: /* valrange2: exprseq DOTDOT exprseq  */
#line 961 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);

				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-2], nilnode);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3476 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 121: /* valrange2: exprseq COMMA exprseq DOTDOT exprseq  */
#line 978 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot selectorSlot;
				PyrParseNode* args;

				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-4],
					(PyrParseNode*)yyvsp[-2]);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3494 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 122: /* valrange2: exprseq COMMA exprseq DOTDOT  */
#line 992 "lang/LangSource/Bison/lang11d"
                        {
				// if this is not used in a 'do' or list comprehension, then should return an error.
				PyrSlotNode *selectornode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;
				PyrPushLitNode *nilnode;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);

				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-3],
					(PyrParseNode*)yyvsp[-1]);
				args = linkNextNode(args, nilnode);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3517 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 123: /* valrange3: DOTDOT exprseq  */
#line 1013 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode, *zeronode;
				PyrSlot selectorSlot, nilSlot, zeroSlot;
				PyrParseNode* args;

				SetInt(&zeroSlot, 0);
				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
				zeronode = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &zeroSlot), NULL);

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode(zeronode, nilnode);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3539 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 124: /* valrange3: exprseq DOTDOT  */
#line 1032 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode, *nilnode2;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);
				nilnode2 = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-1], nilnode);
				args = linkNextNode(args, nilnode2);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3560 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 125: /* valrange3: exprseq DOTDOT exprseq  */
#line 1050 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-2], nilnode);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3580 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 126: /* valrange3: exprseq COMMA exprseq DOTDOT  */
#line 1067 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode((yyloc), newPyrSlotNode((yyloc), &nilSlot), NULL);

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]);
				args = linkNextNode(args, nilnode);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3600 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 127: /* valrange3: exprseq COMMA exprseq DOTDOT exprseq  */
#line 1083 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot selectorSlot;
				PyrParseNode* args;

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode((yyloc), &selectorSlot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-4],
					(PyrParseNode*)yyvsp[-2]);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3618 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 131: /* expr: classname  */
#line 1101 "lang/LangSource/Bison/lang11d"
                            { yyval = (intptr_t)newPyrPushNameNode((yyloc), (PyrSlotNode*)yyvsp[0]); }
#line 3624 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 132: /* expr: expr DOT OPENSQUARE arglist1 CLOSESQUARE  */
#line 1103 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-4],
					(PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3641 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 133: /* expr: BACKTICK expr  */
#line 1116 "lang/LangSource/Bison/lang11d"
                        {
				PyrParseNode *node, *args;
				PyrSlotNode *slotnode;
				PyrSlot slot;

				SetSymbol(&slot, s_ref);
				slotnode = newPyrSlotNode((yyloc), &slot);
				node = (PyrParseNode*)newPyrPushNameNode((yyloc), slotnode);
				args = linkNextNode(node, (PyrParseNode*)yyvsp[0]);
				SetSymbol(&slot, s_new);
				slotnode = newPyrSlotNode((yyloc), &slot);
				yyval = (intptr_t)newPyrCallNode((yyloc), slotnode, args, 0, 0);
			}
#line 3659 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 134: /* expr: expr binop2 adverb expr  */
#line 1130 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrBinopCallNode((yyloc), (PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0], (PyrParseNode*)yyvsp[-1]);
			}
#line 3668 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 135: /* expr: name EQUALSSIGN expr  */
#line 1135 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrAssignNode((yyloc), (PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0);
			}
#line 3676 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 136: /* expr: TILDE name EQUALSSIGN expr  */
#line 1139 "lang/LangSource/Bison/lang11d"
                        {
				PyrParseNode *argnode, *args;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[-2], NULL);
				args = linkNextNode(argnode, (PyrParseNode*)yyvsp[0]);
				SetSymbol(&slot, s_envirPut);
				selectornode = newPyrSlotNode((yyloc), &slot);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3691 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 137: /* expr: expr DOT name EQUALSSIGN expr  */
#line 1150 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrSetterNode((yyloc), (PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3700 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 138: /* expr: name OPENPAREN arglist1 optkeyarglist CLOSEPAREN EQUALSSIGN expr  */
#line 1155 "lang/LangSource/Bison/lang11d"
                        {
				if (yyvsp[-3] != 0) {
					error("Setter method called with keyword arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-3]);
					compileErrors++;
				}
				yyval = (intptr_t)newPyrSetterNode((yyloc), (PyrSlotNode*)yyvsp[-6],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3714 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 139: /* expr: HASH mavars EQUALSSIGN expr  */
#line 1165 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrMultiAssignNode((yyloc), (PyrMultiAssignVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[0], 0);
			}
#line 3723 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 140: /* expr: expr1 OPENSQUARE arglist1 CLOSESQUARE EQUALSSIGN expr  */
#line 1170 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_put);
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-5],
					(PyrParseNode*)yyvsp[-3]);
				args = linkNextNode( args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3741 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 141: /* expr: expr DOT OPENSQUARE arglist1 CLOSESQUARE EQUALSSIGN expr  */
#line 1184 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_put);
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-6],
					(PyrParseNode*)yyvsp[-3]);
				args = linkNextNode( args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, 0);
			}
#line 3759 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 142: /* adverb: %empty  */
#line 1199 "lang/LangSource/Bison/lang11d"
          { yyval = 0; }
#line 3765 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 143: /* adverb: DOT name  */
#line 1200 "lang/LangSource/Bison/lang11d"
                           { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3771 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 144: /* adverb: DOT integer  */
#line 1201 "lang/LangSource/Bison/lang11d"
                              { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3777 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 145: /* adverb: DOT OPENPAREN exprseq CLOSEPAREN  */
#line 1202 "lang/LangSource/Bison/lang11d"
                                                   { yyval = yyvsp[-1]; }
#line 3783 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 147: /* exprn: exprn SEMICOLON expr  */
#line 1207 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((yyloc), (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 3791 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 149: /* arrayelems: %empty  */
#line 1215 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 3797 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 150: /* arrayelems: arrayelems1 optcomma  */
#line 1217 "lang/LangSource/Bison/lang11d"
                          { yyval = yyvsp[-1]; }
#line 3803 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 152: /* arrayelems1: exprseq COLON exprseq  */
#line 1222 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3809 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 153: /* arrayelems1: keybinop exprseq  */
#line 1224 "lang/LangSource/Bison/lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 3818 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 154: /* arrayelems1: arrayelems1 COMMA exprseq  */
#line 1229 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3824 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 155: /* arrayelems1: arrayelems1 COMMA keybinop exprseq  */
#line 1231 "lang/LangSource/Bison/lang11d"
                                {
					PyrParseNode* elems;
					PyrParseNode* key = newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[-1], NULL);
					elems = (PyrParseNode*)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], elems);
				}
#line 3835 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 156: /* arrayelems1: arrayelems1 COMMA exprseq COLON exprseq  */
#line 1238 "lang/LangSource/Bison/lang11d"
                                {
					PyrParseNode* elems;
					elems = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-4], elems);
				}
#line 3845 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 158: /* arglist1: arglist1 COMMA exprseq  */
#line 1247 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3851 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 159: /* arglistv1: MULTIPLY exprseq  */
#line 1251 "lang/LangSource/Bison/lang11d"
                                { yyval = yyvsp[0]; }
#line 3857 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 160: /* arglistv1: arglist1 COMMA MULTIPLY exprseq  */
#line 1253 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]); }
#line 3863 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 162: /* keyarglist1: keyarglist1 COMMA keyarg  */
#line 1258 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3869 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 163: /* keyarg: keybinop exprseq  */
#line 1262 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrPushKeyArgNode((yyloc), (PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 3875 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 164: /* optkeyarglist: optcomma  */
#line 1265 "lang/LangSource/Bison/lang11d"
                           { yyval = 0; }
#line 3881 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 165: /* optkeyarglist: COMMA keyarglist1 optcomma  */
#line 1266 "lang/LangSource/Bison/lang11d"
                                                             { yyval = yyvsp[-1]; }
#line 3887 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 166: /* mavars: mavarlist  */
#line 1270 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3893 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 167: /* mavars: mavarlist ELLIPSIS name  */
#line 1272 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((yyloc), (PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); }
#line 3899 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 169: /* mavarlist: mavarlist COMMA name  */
#line 1277 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3905 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 170: /* slotliteral: integer  */
#line 1281 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3911 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 171: /* slotliteral: floatp  */
#line 1282 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3917 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 172: /* slotliteral: ascii  */
#line 1283 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3923 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 173: /* slotliteral: string  */
#line 1284 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3929 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 174: /* slotliteral: symbol  */
#line 1285 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3935 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 175: /* slotliteral: trueobj  */
#line 1286 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3941 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 176: /* slotliteral: falseobj  */
#line 1287 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3947 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 177: /* slotliteral: nilobj  */
#line 1288 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3953 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 178: /* slotliteral: listlit  */
#line 1289 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), NULL, (PyrParseNode*)yyvsp[0]); }
#line 3959 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 179: /* blockliteral: block  */
#line 1292 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), NULL, (PyrParseNode*)yyvsp[0]); }
#line 3965 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 180: /* pushname: name  */
#line 1295 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrPushNameNode((yyloc), (PyrSlotNode*)yyvsp[0]); }
#line 3971 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 181: /* pushliteral: integer  */
#line 1298 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3977 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 182: /* pushliteral: floatp  */
#line 1299 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3983 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 183: /* pushliteral: ascii  */
#line 1300 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3989 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 184: /* pushliteral: string  */
#line 1301 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3995 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 185: /* pushliteral: symbol  */
#line 1302 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4001 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 186: /* pushliteral: trueobj  */
#line 1303 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4007 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 187: /* pushliteral: falseobj  */
#line 1304 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4013 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 188: /* pushliteral: nilobj  */
#line 1305 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4019 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 189: /* pushliteral: listlit  */
#line 1306 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), NULL, (PyrParseNode*)yyvsp[0]); }
#line 4025 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 190: /* listliteral: integer  */
#line 1309 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4031 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 191: /* listliteral: floatp  */
#line 1310 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4037 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 192: /* listliteral: ascii  */
#line 1311 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4043 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 193: /* listliteral: string  */
#line 1312 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4049 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 194: /* listliteral: symbol  */
#line 1313 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4055 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 195: /* listliteral: name  */
#line 1314 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4061 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 196: /* listliteral: trueobj  */
#line 1315 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4067 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 197: /* listliteral: falseobj  */
#line 1316 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4073 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 198: /* listliteral: nilobj  */
#line 1317 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4079 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 199: /* listliteral: listlit2  */
#line 1318 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), NULL, (PyrParseNode*)yyvsp[0]); }
#line 4085 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 200: /* block: OPENCURLY argdecls funcvardecls funcbody CLOSECURLY  */
#line 1322 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), (PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], false); }
#line 4092 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 201: /* block: BEGINCLOSEDFUNC argdecls funcvardecls funcbody CLOSECURLY  */
#line 1325 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), (PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], true); }
#line 4099 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 202: /* funcvardecls: %empty  */
#line 1329 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 4105 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 203: /* funcvardecls: funcvardecls funcvardecl  */
#line 1331 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 4111 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 205: /* funcvardecls1: funcvardecls1 funcvardecl  */
#line 1336 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 4117 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 206: /* funcvardecl: VAR vardeflist SEMICOLON  */
#line 1340 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrVarListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], varLocal); }
#line 4123 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 207: /* argdecls: %empty  */
#line 1343 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 4129 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 208: /* argdecls: ARG vardeflist SEMICOLON  */
#line 1345 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4137 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 209: /* argdecls: ARG vardeflist0 ELLIPSIS name SEMICOLON  */
#line 1349 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4145 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 210: /* argdecls: PIPE slotdeflist PIPE  */
#line 1353 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4153 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 211: /* argdecls: PIPE slotdeflist0 ELLIPSIS name PIPE  */
#line 1357 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4161 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 212: /* argdecls: PIPE slotdeflist0 ELLIPSIS name COMMA name PIPE  */
#line 1361 "lang/LangSource/Bison/lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 4169 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 213: /* argdecls1: ARG vardeflist SEMICOLON  */
#line 1367 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4177 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 214: /* argdecls1: ARG vardeflist0 ELLIPSIS name SEMICOLON  */
#line 1371 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4185 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 215: /* argdecls1: PIPE slotdeflist PIPE  */
#line 1375 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4193 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 216: /* argdecls1: PIPE slotdeflist0 ELLIPSIS name PIPE  */
#line 1379 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4201 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 217: /* argdecls1: PIPE slotdeflist0 ELLIPSIS name COMMA name PIPE  */
#line 1383 "lang/LangSource/Bison/lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 4209 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 219: /* constdeflist: constdeflist optcomma constdef  */
#line 1391 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4215 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 220: /* constdef: rspec name EQUALSSIGN slotliteral  */
#line 1395 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4221 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 221: /* slotdeflist0: %empty  */
#line 1398 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 4227 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 224: /* slotdeflist: slotdeflist optcomma slotdef  */
#line 1404 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4233 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 225: /* slotdef: name  */
#line 1408 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4239 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 226: /* slotdef: name optequal slotliteral  */
#line 1410 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4245 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 227: /* slotdef: name optequal OPENPAREN exprseq CLOSEPAREN  */
#line 1412 "lang/LangSource/Bison/lang11d"
                                {
					PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
					node->mParens = 1;
					yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-4], node, 0);
				}
#line 4255 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 228: /* vardeflist0: %empty  */
#line 1419 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 4261 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 231: /* vardeflist: vardeflist COMMA vardef  */
#line 1425 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4267 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 232: /* vardef: name  */
#line 1429 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4273 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 233: /* vardef: name EQUALSSIGN expr  */
#line 1431 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4279 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 234: /* vardef: name OPENPAREN exprseq CLOSEPAREN  */
#line 1433 "lang/LangSource/Bison/lang11d"
                                {
									PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
									node->mParens = 1;
									yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-3], node, 0);
								}
#line 4289 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 235: /* dictslotdef: exprseq COLON exprseq  */
#line 1441 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4295 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 236: /* dictslotdef: keybinop exprseq  */
#line 1443 "lang/LangSource/Bison/lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 4304 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 238: /* dictslotlist1: dictslotlist1 COMMA dictslotdef  */
#line 1451 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4310 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 239: /* dictslotlist: %empty  */
#line 1454 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 4316 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 242: /* rwslotdeflist: rwslotdeflist COMMA rwslotdef  */
#line 1460 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4322 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 243: /* rwslotdef: rwspec name  */
#line 1464 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); }
#line 4328 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 244: /* rwslotdef: rwspec name EQUALSSIGN slotliteral  */
#line 1466 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4334 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 245: /* listlit: HASH OPENSQUARE literallistc CLOSESQUARE  */
#line 1470 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((yyloc), 0, (PyrParseNode*)yyvsp[-1]); }
#line 4340 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 246: /* listlit: HASH classname OPENSQUARE literallistc CLOSESQUARE  */
#line 1472 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((yyloc), (PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4346 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 247: /* listlit2: OPENSQUARE literallistc CLOSESQUARE  */
#line 1476 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((yyloc), 0, (PyrParseNode*)yyvsp[-1]); }
#line 4352 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 248: /* listlit2: classname OPENSQUARE literallistc CLOSESQUARE  */
#line 1478 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((yyloc), (PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4358 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 249: /* literallistc: %empty  */
#line 1481 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 4364 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 252: /* literallist1: literallist1 COMMA listliteral  */
#line 1487 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4370 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 253: /* rwspec: %empty  */
#line 1490 "lang/LangSource/Bison/lang11d"
           { yyval = rwPrivate; }
#line 4376 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 254: /* rwspec: LESSTHAN  */
#line 1492 "lang/LangSource/Bison/lang11d"
                        { yyval = rwReadOnly; }
#line 4382 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 255: /* rwspec: READWRITEVAR  */
#line 1494 "lang/LangSource/Bison/lang11d"
                        { yyval = rwReadWrite; }
#line 4388 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 256: /* rwspec: GREATERTHAN  */
#line 1496 "lang/LangSource/Bison/lang11d"
                        { yyval = rwWriteOnly; }
#line 4394 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 257: /* rspec: %empty  */
#line 1499 "lang/LangSource/Bison/lang11d"
           { yyval = rwPrivate; }
#line 4400 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 258: /* rspec: LESSTHAN  */
#line 1501 "lang/LangSource/Bison/lang11d"
                        { yyval = rwReadOnly; }
#line 4406 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 259: /* integer: INTEGER  */
#line 1504 "lang/LangSource/Bison/lang11d"
                  { yyval = zzval; }
#line 4412 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 260: /* integer: MINUS INTEGER  */
#line 1506 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawInt(&node->mSlot));
				yyval = zzval;
			}
#line 4423 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 261: /* floatr: SC_FLOAT  */
#line 1514 "lang/LangSource/Bison/lang11d"
                   { yyval = zzval; }
#line 4429 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 262: /* floatr: MINUS SC_FLOAT  */
#line 1516 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawFloat(&node->mSlot));
				yyval = zzval;
			}
#line 4440 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 263: /* accidental: ACCIDENTAL  */
#line 1524 "lang/LangSource/Bison/lang11d"
                        { yyval = zzval; }
#line 4446 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 264: /* accidental: MINUS ACCIDENTAL  */
#line 1526 "lang/LangSource/Bison/lang11d"
                                {
					PyrSlotNode *node;
					double intval, fracval;
					node = (PyrSlotNode*)zzval;
					intval = floor(slotRawFloat(&node->mSlot) + 0.5);
					fracval = slotRawFloat(&node->mSlot) - intval;
					SetRaw(&node->mSlot, -intval + fracval);
					yyval = zzval;
				}
#line 4460 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 265: /* pie: PIE  */
#line 1536 "lang/LangSource/Bison/lang11d"
                      { yyval = zzval; }
#line 4466 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 268: /* floatp: floatr pie  */
#line 1542 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)yyvsp[-1];
				SetRaw(&node->mSlot, slotRawFloat(&node->mSlot) * pi);
			}
#line 4476 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 269: /* floatp: integer pie  */
#line 1548 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *node;
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = slotRawInt(&node->mSlot);
				SetFloat(&node->mSlot, ival * pi);
			}
#line 4488 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 270: /* floatp: pie  */
#line 1556 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, pi);
				yyval = zzval;
			}
#line 4499 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 271: /* floatp: MINUS pie  */
#line 1563 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, -pi);
				yyval = zzval;
			}
#line 4510 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 272: /* name: NAME  */
#line 1571 "lang/LangSource/Bison/lang11d"
                       { yyval = zzval; }
#line 4516 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 273: /* name: WHILE  */
#line 1572 "lang/LangSource/Bison/lang11d"
                                { yyval = zzval; }
#line 4522 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 274: /* classname: CLASSNAME  */
#line 1575 "lang/LangSource/Bison/lang11d"
                                    { yyval = zzval; }
#line 4528 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 275: /* primname: PRIMITIVENAME  */
#line 1578 "lang/LangSource/Bison/lang11d"
                                        { yyval = zzval; }
#line 4534 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 276: /* trueobj: TRUEOBJ  */
#line 1581 "lang/LangSource/Bison/lang11d"
                          { yyval = zzval; }
#line 4540 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 277: /* falseobj: FALSEOBJ  */
#line 1584 "lang/LangSource/Bison/lang11d"
                           { yyval = zzval; }
#line 4546 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 278: /* nilobj: NILOBJ  */
#line 1587 "lang/LangSource/Bison/lang11d"
                         { yyval = zzval; }
#line 4552 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 279: /* ascii: ASCII  */
#line 1590 "lang/LangSource/Bison/lang11d"
                        { yyval = zzval; }
#line 4558 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 280: /* symbol: SYMBOL  */
#line 1593 "lang/LangSource/Bison/lang11d"
                         { yyval = zzval; }
#line 4564 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 281: /* string: STRING  */
#line 1596 "lang/LangSource/Bison/lang11d"
                         { yyval = zzval; }
#line 4570 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 282: /* pseudovar: PSEUDOVAR  */
#line 1599 "lang/LangSource/Bison/lang11d"
                            { yyval = zzval; }
#line 4576 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 283: /* binop: BINOP  */
#line 1602 "lang/LangSource/Bison/lang11d"
                { yyval = zzval; }
#line 4582 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 284: /* binop: READWRITEVAR  */
#line 1603 "lang/LangSource/Bison/lang11d"
                               { yyval = zzval; }
#line 4588 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 285: /* binop: LESSTHAN  */
#line 1604 "lang/LangSource/Bison/lang11d"
                            { yyval = zzval; }
#line 4594 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 286: /* binop: GREATERTHAN  */
#line 1605 "lang/LangSource/Bison/lang11d"
                               { yyval = zzval; }
#line 4600 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 287: /* binop: MINUS  */
#line 1606 "lang/LangSource/Bison/lang11d"
                         { yyval = zzval; }
#line 4606 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 288: /* binop: MULTIPLY  */
#line 1607 "lang/LangSource/Bison/lang11d"
                            { yyval = zzval; }
#line 4612 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 289: /* binop: ADD  */
#line 1608 "lang/LangSource/Bison/lang11d"
                       { yyval = zzval; }
#line 4618 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 290: /* binop: PIPE  */
#line 1609 "lang/LangSource/Bison/lang11d"
                        { yyval = zzval; }
#line 4624 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 291: /* keybinop: KEYBINOP  */
#line 1612 "lang/LangSource/Bison/lang11d"
                    { yyval = zzval; }
#line 4630 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 294: /* curryarg: CURRYARG  */
#line 1619 "lang/LangSource/Bison/lang11d"
                    { yyval =(intptr_t)newPyrCurryArgNode((yyloc)); }
#line 4636 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;


#line 4640 "lang/LangSource/Bison/lang11d_tab.cpp"

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

