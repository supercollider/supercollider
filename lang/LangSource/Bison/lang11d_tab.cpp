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
#define YYLAST   1643

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  98
/* YYNRULES -- Number of rules.  */
#define YYNRULES  295
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  555

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
     211,   216,   220,   225,   243,   256,   258,   269,   280,   291,
     304,   325,   334,   343,   348,   362,   384,   388,   394,   412,
     418,   418,   428,   428,   435,   456,   460,   494,   532,   546,
     557,   561,   586,   587,   588,   589,   590,   591,   592,   599,
     609,   611,   613,   615,   617,   619,   632,   635,   662,   680,
     707,   735,   754,   782,   809,   827,   852,   880,   899,   927,
     946,   965,   982,   996,  1017,  1036,  1054,  1071,  1087,  1103,
    1104,  1105,  1106,  1107,  1120,  1134,  1139,  1143,  1154,  1159,
    1169,  1174,  1188,  1204,  1205,  1206,  1207,  1210,  1211,  1217,
    1220,  1221,  1225,  1226,  1228,  1233,  1235,  1242,  1250,  1251,
    1255,  1257,  1261,  1262,  1266,  1270,  1271,  1274,  1276,  1280,
    1281,  1286,  1287,  1288,  1289,  1290,  1291,  1292,  1293,  1294,
    1297,  1300,  1303,  1304,  1305,  1306,  1307,  1308,  1309,  1310,
    1311,  1314,  1315,  1316,  1317,  1318,  1319,  1320,  1321,  1322,
    1323,  1326,  1329,  1334,  1335,  1339,  1340,  1344,  1348,  1349,
    1353,  1357,  1361,  1365,  1371,  1375,  1379,  1383,  1387,  1394,
    1395,  1399,  1403,  1404,  1407,  1408,  1412,  1414,  1416,  1424,
    1425,  1428,  1429,  1433,  1435,  1437,  1445,  1447,  1454,  1455,
    1459,  1460,  1463,  1464,  1468,  1470,  1474,  1476,  1480,  1482,
    1486,  1487,  1490,  1491,  1495,  1496,  1498,  1500,  1504,  1505,
    1509,  1510,  1519,  1520,  1529,  1530,  1541,  1544,  1545,  1546,
    1552,  1560,  1567,  1576,  1577,  1580,  1583,  1586,  1589,  1592,
    1595,  1598,  1601,  1604,  1607,  1608,  1609,  1610,  1611,  1612,
    1613,  1614,  1617,  1620,  1621,  1624
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

#define YYPACT_NINF (-334)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-292)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      53,   828,    -1,    44,    -1,    25,  -334,    21,  1213,   708,
    1508,   137,    90,  -334,  -334,  -334,  -334,  -334,  -334,  -334,
    -334,  -334,    90,    90,  -334,  -334,  -334,  -334,  -334,    60,
    -334,   209,    90,  1508,  -334,  -334,  -334,  -334,  -334,    81,
    -334,  -334,  -334,  1592,   113,    89,  -334,  -334,  -334,  -334,
    1043,  -334,  1043,  -334,   104,   104,  -334,  -334,  -334,    56,
     213,  -334,  -334,  -334,  -334,  -334,  -334,  -334,  -334,   149,
    -334,  -334,    22,  -334,  1508,    90,  1508,    90,  -334,   780,
    -334,   122,   177,   180,  1508,  1508,  1255,  -334,   209,  -334,
    -334,  -334,  -334,   238,  -334,   201,   362,  1043,  1043,  -334,
     202,   221,  -334,  1508,   227,     5,  1539,   195,    26,  -334,
     234,   208,   258,  -334,    38,   226,   295,  -334,  -334,  -334,
    -334,  -334,   244,    49,  -334,   254,  -334,  1297,  -334,  -334,
     357,  -334,   243,  1508,  -334,  -334,  -334,  -334,  -334,  1043,
    -334,  -334,   871,  1508,    29,  -334,  -334,  -334,  1213,   914,
      29,  -334,    90,    -1,   297,  -334,   287,   306,  -334,   290,
      76,  1043,  1508,  -334,  1213,  -334,  -334,  -334,  1508,   317,
     152,  -334,  -334,  1508,  1508,  1508,   332,   335,  1043,  1213,
    -334,  -334,  -334,   205,  -334,  -334,  1539,  -334,  -334,   342,
     341,   104,  -334,  -334,   356,  -334,  -334,  -334,  -334,  -334,
    -334,  1508,    90,    90,  1539,  1508,  -334,    90,  1508,  1508,
      90,  -334,  1043,    90,  -334,  -334,    90,  -334,  1569,  1508,
    -334,   124,  1339,   957,   109,   321,  1508,  1592,  -334,    29,
    1508,   355,   372,    20,  -334,  1508,  1592,  -334,   384,    29,
     355,   372,   380,   590,   388,  -334,  -334,  -334,   386,    90,
    -334,   393,    90,  -334,   395,  -334,  -334,   366,  1508,  -334,
    -334,  1508,  1508,   376,  -334,  -334,  -334,  -334,   403,   375,
    -334,  1508,  1085,    29,   406,  -334,  1539,  -334,  1539,  1592,
    -334,  -334,   415,  1592,  -334,   425,  1592,   414,   437,    18,
    -334,  1508,    35,  -334,  -334,   104,  -334,  -334,  -334,  -334,
    -334,  -334,  -334,   436,   408,  1508,  1381,  1508,   255,    29,
     355,   372,   380,  1000,  1508,    29,  -334,  1508,   435,  -334,
    -334,   401,    29,  -334,  1128,  -334,   457,   423,   459,    29,
     423,  -334,  -334,  -334,   460,   466,   423,   470,  -334,   726,
    -334,   476,   477,   445,   329,  1171,   478,  1171,    31,  -334,
    1508,  -334,   455,  -334,  1508,  -334,    29,   355,   372,  -334,
    -334,   484,  -334,  -334,  -334,  -334,    90,  -334,   483,   451,
    1508,  -334,   456,   495,   496,   464,  1423,  -334,   499,   500,
     501,    29,   355,   372,   380,  1592,   502,  1508,   380,   196,
    -334,  -334,  -334,    29,  -334,    29,   509,   511,    60,    60,
     512,   288,   288,   473,  -334,   639,    90,   349,   515,   510,
     203,  -334,   516,    90,  -334,  -334,  1508,  -334,    29,   514,
     523,  -334,   474,  -334,  1508,  1592,  1508,   493,   494,  1508,
     498,   518,    29,  -334,    29,  -334,   528,   529,   533,  -334,
    -334,  -334,  1508,  -334,  -334,  -334,    60,    60,  -334,  -334,
    -334,  -334,  -334,  -334,   358,  -334,    90,   363,  -334,   369,
    -334,    90,  -334,   503,  1508,  1508,  -334,  1171,  -334,  1508,
     504,  -334,   508,  -334,    29,  -334,  -334,  1592,  1592,  1508,
    1508,  1592,  1508,   506,  -334,  -334,    29,  -334,    29,  1592,
    -334,  -334,   148,   148,   329,  -334,   288,   507,  -334,  -334,
     473,   520,  1508,   510,   510,  -334,   510,  1508,  -334,  -334,
    1592,  1592,  1592,  1508,  -334,  -334,   148,   148,  -334,  1466,
     536,  1466,   653,  -334,  1589,  -334,  1589,   510,  -334,  -334,
    -334,   510,  1592,  1466,  1466,  1508,   545,  -334,   540,  -334,
     555,  -334,  -334,  -334,  -334,  -334,   558,   560,     5,  -334,
    -334,  -334,  -334,  -334,  -334
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       5,    48,     0,     0,     2,     3,     7,   208,   150,   240,
       0,     0,     0,   273,   260,   262,   264,   281,   282,   280,
     275,   295,     0,   229,   279,   277,   278,   283,   266,   208,
     274,     0,   222,     0,    41,     4,    33,    97,    94,   129,
     106,   131,   130,   147,    27,    48,    93,    95,    92,   180,
      48,   205,    48,   190,   182,   267,   268,   271,   183,   181,
     132,   187,   188,   189,   184,   186,   185,   104,    96,     0,
       1,     6,    14,     8,     0,   229,     0,   222,   203,   240,
     292,   152,     0,    29,     0,     0,     0,   284,   288,   286,
     287,   289,   290,   222,   285,     0,     0,    48,    48,   238,
      29,     0,   293,   294,     0,    27,   250,     0,   167,   169,
       0,    99,     0,   231,   233,     0,   230,   203,   261,   263,
     265,   272,     0,    29,   224,    31,   134,     0,   288,   291,
       0,   294,   143,    28,   149,    34,    40,   206,    39,    48,
     270,   269,     0,     0,    56,    50,    53,    52,   150,     0,
      66,    21,    12,     0,     0,    82,     0,   230,    80,     0,
      29,    48,     0,   100,    30,   151,   154,   120,     0,     0,
       0,   101,    98,     0,   119,     0,     0,     0,    48,    30,
     241,   103,   237,     0,    28,    49,   250,   252,   200,     0,
      29,   191,   192,   196,     0,   197,   198,   199,   193,   195,
     194,     0,     0,     0,   250,     0,   207,     0,     0,     0,
       0,   214,    48,     0,    30,   216,     0,    32,     0,     0,
     158,     0,     0,     0,    54,     0,     0,   148,    38,     0,
       0,    29,    29,     0,   162,     0,   136,    51,     0,    54,
      29,    29,    29,     0,     0,    13,    15,    16,     0,     0,
     209,     0,     0,   211,     0,   204,   153,   155,     0,   124,
     102,     0,   125,     0,   121,   236,    37,    36,     0,     0,
     239,     0,     0,    57,     0,   246,    30,   251,   250,   140,
     170,   168,     0,   137,   232,     0,   234,     0,     0,     0,
     225,     0,     0,   227,   179,   171,   172,   176,   177,   178,
     173,   175,   174,     0,   105,     0,     0,     0,     0,    54,
      29,    29,    29,     0,     0,    55,    79,     0,     0,   145,
     144,   135,    58,   160,    30,   165,     0,    30,     0,    54,
       0,   164,    65,    67,     0,     0,    30,     0,    11,   289,
      22,     0,     0,    14,    21,     0,     0,     0,     0,   201,
       0,   156,     0,   126,   123,    35,     0,    29,    29,   248,
     253,     0,   247,   235,   215,   202,     0,   217,     0,   108,
       0,   159,   107,     0,     0,   133,     0,    71,     0,     0,
       0,    54,    29,    29,    29,   138,     0,     0,    29,    54,
      63,    60,   163,    54,    70,    54,     0,     0,   208,   208,
       0,   254,   254,   258,    17,     0,     0,     0,     0,    84,
     181,   210,     0,     0,   212,   157,   127,   122,    61,     0,
       0,   249,     0,   228,     0,   141,     0,   109,   114,     0,
     113,     0,    54,    75,    54,    76,     0,     0,     0,   146,
     161,   166,     0,    59,    69,    68,   208,   208,   203,   203,
      16,   255,   257,   256,     0,   242,     0,     0,   259,    29,
     219,     0,     9,     0,     0,     0,    83,     0,    89,     0,
       0,    81,     0,   128,    54,    64,   218,   111,   110,     0,
       0,   142,     0,   115,    74,    72,    54,    78,    54,   139,
     203,   203,    44,    44,    21,    19,   254,   244,    18,    20,
     258,     0,     0,    84,    84,    85,    84,     0,   213,    62,
     112,   117,   116,     0,    77,    73,    44,    44,   276,    46,
      27,    46,     0,   243,     0,   220,     0,    84,    91,    90,
      86,    84,   118,    46,    46,     0,     0,    42,    46,    45,
       0,    10,   245,   221,    88,    87,     0,     0,    27,    23,
      43,    25,    24,    26,    47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -334,  -334,  -334,  -334,  -334,   561,  -334,   222,   118,  -334,
    -333,  -334,  -100,    70,  -334,   419,  -334,  -262,  -261,    33,
     527,   -25,  -126,    12,  -334,   174,  -334,  -334,  -177,  -328,
    -334,  -334,  -334,  -334,  -334,  -334,    -7,  -334,  -334,    -8,
     426,  -334,    79,  -139,  -106,  -187,   311,  -334,  -334,  -194,
     231,  -334,  -334,   299,  -334,  -104,    13,   261,   -22,   564,
    -334,    77,   505,   513,   360,   524,    -6,   371,   400,  -334,
    -334,   179,    87,  -216,  -334,  -163,  -334,  -334,  -334,   -98,
    -334,  -334,   -16,   -97,    71,   278,  -334,   -94,   -90,   -86,
     -81,   -73,   -31,  -334,  -237,   454,    -5,  -334
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     3,     4,     5,    71,     6,   244,   154,   344,   404,
     243,   340,   134,   325,   218,    34,    35,   536,   519,   537,
      36,   315,   145,   316,    37,    38,   251,   248,   468,   408,
      39,    40,    41,    42,    95,   169,    43,   226,    44,    45,
      82,    83,   221,   232,   388,   234,   326,   107,   108,   293,
      46,    47,    48,   187,    49,   161,    50,   255,    78,    52,
     459,   460,   122,   123,   124,   115,   112,   113,    99,   100,
     101,   454,   455,    53,   188,   189,   190,   456,   461,    54,
      55,    56,    57,    58,    59,    60,   520,    61,    62,    63,
      64,    65,    66,    67,   102,   131,   132,    68
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      81,    96,   294,   105,   104,   185,   342,   117,   191,   192,
     241,   405,   195,   212,   184,   121,   196,   116,   237,   412,
     197,    20,    97,   274,   237,   198,   126,   152,   329,   366,
      74,   330,     7,   199,   144,   150,   233,   202,   140,   141,
     106,   282,   413,   242,    70,   208,    75,    87,    80,   128,
      89,    90,    91,    92,   129,    94,   130,    20,   203,     7,
     214,    76,   153,   142,    29,   139,   155,   367,   158,   157,
      77,    96,   121,     2,   104,   200,   166,   167,   170,   209,
     414,  -223,   109,   111,   311,    75,   127,   214,   191,   192,
       1,    29,   195,   114,   114,   182,   196,   143,   215,    10,
     197,     2,   397,   125,    13,   198,   191,   192,  -223,    77,
     195,   178,     7,   199,   196,   361,   313,   312,   197,   220,
     295,   296,   133,   198,   297,   253,   227,   319,   298,    30,
     304,   199,   299,   358,   220,   305,   236,   300,    28,   505,
      81,   220,   106,   392,    29,   301,   114,   237,   125,   392,
     314,    13,   151,   165,   256,   200,   257,   306,   273,    20,
     259,   522,   162,   261,   125,   263,   264,   265,   342,   518,
     180,   269,    22,   200,   383,   140,    30,   193,   191,   192,
     191,   192,   195,   163,   195,   262,   196,   302,   196,   237,
     197,   164,   197,   216,   279,   198,   237,   198,   283,     7,
     285,   224,   286,   199,   322,   199,     7,   384,     7,   171,
     142,   303,   272,   179,   220,   220,     7,    13,   148,   321,
     149,   231,   323,   245,   118,   119,   120,   331,   240,   181,
     216,    29,   521,   146,   146,   183,   201,   442,    29,   204,
      29,   469,    30,    28,   143,   200,  -291,   200,    29,   205,
     351,   333,    13,   352,   353,   533,   534,   193,   210,   540,
     277,   375,    51,   182,   220,  -226,   305,   206,  -226,   207,
      51,   546,   547,   280,   281,   193,   213,    30,   114,   140,
      69,   287,    72,   368,   289,   342,  -226,   125,   376,   110,
     147,   147,   237,  -226,   225,   217,   320,   371,   373,   374,
     247,   308,   310,  -226,   211,   220,   207,   385,   294,   386,
     294,   137,   337,    51,   341,   250,   371,   207,   146,   249,
     346,   377,   252,   348,   146,   260,   528,   529,   317,   530,
     542,   418,   543,   451,   452,    13,    14,   409,   453,   409,
     266,   391,   415,   267,   492,   493,   417,   193,   275,   193,
     544,   357,   276,   401,   545,   402,   403,   146,   137,    51,
      30,   278,   222,   425,   223,   318,   324,   495,   431,   496,
     172,    13,   498,   173,   496,   147,   448,   449,   499,   440,
     214,   147,   380,   327,   194,   144,   516,   517,   464,   465,
     332,   336,   382,   435,   343,   174,    30,   345,   146,   349,
     137,   443,   175,   146,   347,   444,   350,   445,   473,   354,
     396,   355,   359,   146,   147,   175,   410,   477,   410,   478,
     539,   362,   481,   364,   490,   491,   295,   296,   295,   296,
     297,   246,   297,   363,   298,   489,   298,   422,   299,   137,
     299,   365,   369,   300,   484,   300,   485,   146,   554,   370,
     118,   301,   130,   301,   438,   147,   503,   504,   441,   409,
     147,   506,    84,   103,   194,   389,    80,   390,   393,   136,
     147,   138,   510,   511,   394,   512,   341,   463,   395,   398,
     399,   470,   194,   146,   472,   153,   509,   411,   416,   146,
     421,   423,   424,   302,   527,   302,   146,   426,   514,   531,
     515,   427,   428,   146,   147,   429,   532,   432,   433,   434,
     439,   538,   446,   538,   447,   450,   176,   177,   458,   466,
     471,   467,   474,   476,   483,   538,   538,   497,   548,   500,
     146,   475,   501,   103,   479,   480,   486,   487,   410,   482,
     147,   488,   507,   328,   502,   184,   147,   513,   524,   549,
     535,   334,   335,   147,   194,   146,   194,   508,   228,   551,
     147,   526,   552,   146,   553,   400,    73,   146,   494,   146,
     110,   550,   135,    98,   238,   360,   290,   525,   284,   270,
     254,   457,   159,   523,   146,     0,     0,   147,     0,     0,
     160,     0,   146,   341,   338,     0,   235,   268,     0,   156,
       0,     0,    84,   235,    13,     0,   146,     0,   146,     0,
       0,     0,   147,     0,     0,     0,     0,     0,   258,     0,
     147,   378,   379,     0,   147,     0,   147,     0,     0,    30,
       0,   288,    87,   271,   128,    89,    90,   339,    92,   129,
      94,   147,     0,   462,     0,     0,     0,     0,   146,   147,
       0,     0,     0,    13,     0,     0,     0,   541,     0,     0,
     146,     0,   146,   147,     0,   147,     0,    13,   419,   420,
       0,     0,     0,     0,     0,     0,     0,   235,    30,     0,
       0,    87,     0,   128,    89,    90,   339,    92,   129,    94,
       0,     0,    30,   436,   437,    87,     0,   128,    89,    90,
     339,    92,   129,    94,     0,   147,     0,     0,     0,     0,
       0,     7,     0,     8,     0,    79,     0,   147,     0,   147,
      11,    12,    13,    14,    15,    16,    17,    18,    19,     0,
      20,    21,    22,    23,     0,     0,    24,    25,    26,    27,
      13,    85,    28,    29,     0,     0,     0,    30,    86,     0,
      87,    80,    88,    89,    90,    91,    92,    93,    94,     0,
      33,     0,     0,     0,     0,    30,     0,   235,    87,     0,
     128,    89,    90,    91,    92,   129,    94,     0,   235,     0,
       0,   235,     0,     7,   235,     8,     0,    79,     0,     0,
     235,     0,    11,    12,    13,    14,    15,    16,    17,    18,
      19,     0,    20,    21,     0,     0,     0,     0,    24,    25,
      26,    27,     0,    85,    28,    29,     0,     0,     0,    30,
      86,     0,    87,    80,    88,    89,    90,    91,    92,   129,
      94,     7,    33,     8,     0,     9,     0,     0,    10,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    19,     0,
      20,    21,    22,    23,     0,     0,    24,    25,    26,    27,
       0,     0,    28,    29,     0,     0,     0,    30,     0,     0,
       0,     0,    31,     0,     7,     0,     8,    32,    79,   229,
      33,     0,     0,    11,    12,    13,    14,    15,    16,    17,
      18,    19,     0,    20,    21,     0,     0,     0,     0,    24,
      25,    26,    27,     0,     0,    28,    29,     0,     0,     0,
      30,     0,     0,     0,    80,    31,     0,     7,   230,     8,
       0,    79,   239,    33,     0,     0,    11,    12,    13,    14,
      15,    16,    17,    18,    19,     0,    20,    21,     0,     0,
       0,     0,    24,    25,    26,    27,     0,     0,    28,    29,
       0,     0,     0,    30,     0,     0,     0,    80,    31,     0,
       7,   230,     8,     0,    79,   309,    33,     0,     0,    11,
      12,    13,    14,    15,    16,    17,    18,    19,     0,    20,
      21,     0,     0,     0,     0,    24,    25,    26,    27,     0,
       0,    28,    29,     0,     0,     0,    30,     0,     0,     0,
      80,    31,     0,     7,   230,     8,     0,    79,   381,    33,
       0,     0,    11,    12,    13,    14,    15,    16,    17,    18,
      19,     0,    20,    21,     0,     0,     0,     0,    24,    25,
      26,    27,     0,     0,    28,    29,     0,     0,     0,    30,
       0,     0,     0,    80,    31,     0,     7,   230,     8,     0,
      79,     0,    33,    10,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    19,     0,    20,    21,    22,     0,     0,
       0,    24,    25,    26,    27,     0,     0,    28,    29,     0,
       0,     0,    30,     0,     0,     0,     0,    31,     7,     0,
       8,     0,    79,   356,     0,    33,     0,    11,    12,    13,
      14,    15,    16,    17,    18,    19,     0,    20,    21,     0,
       0,     0,     0,    24,    25,    26,    27,     0,     0,    28,
      29,     0,     0,     0,    30,     0,     0,     0,     0,    31,
       0,     7,   230,     8,     0,    79,     0,    33,     0,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    19,     0,
      20,    21,     0,     0,     0,     0,    24,    25,    26,    27,
       0,     0,    28,    29,     0,     0,     0,    30,     0,     0,
       0,    80,    31,     0,     7,   387,     8,     0,    79,     0,
      33,     0,     0,    11,    12,    13,    14,    15,    16,    17,
      18,    19,     0,    20,    21,   406,     0,     0,     0,    24,
      25,    26,    27,     0,     0,    28,    29,     0,     0,     0,
      30,   407,     0,     0,     0,    31,     7,     0,     8,     0,
      79,     0,     0,    33,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    19,     0,    20,    21,     0,     0,     0,
       0,    24,    25,    26,    27,     0,     0,    28,    29,     0,
       0,     0,    30,     0,     0,     0,    80,    31,     7,     0,
       8,     0,    79,     0,     0,    33,     0,    11,    12,    13,
      14,    15,    16,    17,    18,    19,     0,    20,    21,     0,
       0,     0,     0,    24,    25,    26,    27,     0,   168,    28,
      29,     0,     0,     0,    30,     0,     0,     0,     0,    31,
       7,     0,     8,     0,    79,     0,     0,    33,     0,    11,
      12,    13,    14,    15,    16,    17,    18,    19,     0,    20,
      21,     0,     0,     0,     0,    24,    25,    26,    27,     0,
     219,    28,    29,     0,     0,     0,    30,     0,     0,     0,
       0,    31,     7,     0,     8,     0,    79,     0,     0,    33,
       0,    11,    12,    13,    14,    15,    16,    17,    18,    19,
       0,    20,    21,     0,     0,     0,     0,    24,    25,    26,
      27,     0,   307,    28,    29,     0,     0,     0,    30,     0,
       0,     0,     0,    31,     7,     0,     8,   372,    79,     0,
       0,    33,     0,    11,    12,    13,    14,    15,    16,    17,
      18,    19,     0,    20,    21,     0,     0,     0,     0,    24,
      25,    26,    27,     0,     0,    28,    29,     0,     0,     0,
      30,     0,     0,     0,     0,    31,     7,     0,     8,   430,
      79,     0,     0,    33,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    19,     0,    20,    21,     0,     0,     0,
       0,    24,    25,    26,    27,     0,     0,    28,    29,     0,
       0,     0,    30,     0,     0,     0,     0,    31,     0,     7,
       0,     8,     0,    79,     0,    33,   535,     0,    11,    12,
      13,    14,    15,    16,    17,    18,    19,     0,    20,    21,
       0,     0,     0,     0,    24,    25,    26,    27,     0,     0,
      28,    29,     0,     0,     0,    30,     0,     0,     0,     0,
      31,     7,     0,     8,     0,    79,     0,     0,    33,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    19,     0,
      20,    21,     0,     0,     0,     0,    24,    25,    26,    27,
       0,     0,    28,    29,   186,     0,     0,    30,     0,     0,
       0,     0,    31,    13,    14,    15,    16,    17,    18,    19,
      33,    20,     0,     0,     0,     0,     0,    24,    25,    26,
       0,     0,     0,    28,     0,     0,   291,     0,    30,     0,
       0,   292,     0,    31,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,     0,     0,     0,    24,    25,    26,
       0,   292,     0,    28,    14,    15,    16,    17,    18,    19,
       0,     0,     0,    31,     0,     0,     0,    24,    25,    26,
       0,     0,     0,    28,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    87,    80,   128,    89,    90,    91,
      92,   129,    94,   130
};

static const yytype_int16 yycheck[] =
{
       8,     9,   218,    10,     9,   105,   243,    29,   106,   106,
     149,   344,   106,   117,     9,    31,   106,    23,   144,   347,
     106,    22,     9,   186,   150,   106,    33,     5,     8,    11,
       9,    11,     3,   106,    59,    60,   142,    11,    54,    55,
       5,   204,    11,   149,     0,     7,    25,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    22,    32,     3,
      11,    40,    40,     7,    35,    52,    74,    49,    76,    75,
      49,    79,    88,    48,    79,   106,    84,    85,    86,    41,
      49,    32,    11,    12,   223,    25,     5,    11,   186,   186,
      37,    35,   186,    22,    23,   103,   186,    41,    49,    10,
     186,    48,   339,    32,    14,   186,   204,   204,    32,    49,
     204,    98,     3,   186,   204,   278,     7,   223,   204,   127,
     218,   218,     9,   204,   218,    49,   133,   225,   218,    39,
       6,   204,   218,   272,   142,    11,   143,   218,    34,   467,
     148,   149,     5,   330,    35,   218,    75,   273,    77,   336,
      41,    14,     3,    83,   162,   186,   164,    33,   183,    22,
     168,   494,    40,    11,    93,   173,   174,   175,   405,    21,
     100,   179,    24,   204,   313,   191,    39,   106,   276,   276,
     278,   278,   276,     6,   278,    33,   276,   218,   278,   315,
     276,    11,   278,   123,   201,   276,   322,   278,   205,     3,
     208,   130,   209,   276,   229,   278,     3,   313,     3,     8,
       7,   219,     7,    11,   222,   223,     3,    14,     5,   226,
       7,   142,   230,   152,    15,    16,    17,   235,   149,     8,
     160,    35,   493,    59,    60,     8,    41,    41,    35,     5,
      35,    38,    39,    34,    41,   276,     8,   278,    35,    41,
     258,   239,    14,   261,   262,   516,   517,   186,    32,   521,
     190,     6,     1,   271,   272,    11,    11,     9,    14,    11,
       9,   533,   534,   202,   203,   204,    32,    39,   207,   295,
       2,   210,     4,   291,   213,   522,    32,   216,    33,    11,
      59,    60,   418,    39,    51,    41,   225,   305,   306,   307,
       3,   222,   223,    49,     9,   313,    11,   314,   524,   317,
     526,    50,   242,    52,   243,     9,   324,    11,   144,    32,
     249,   309,    32,   252,   150,     8,   503,   504,     7,   506,
     524,   356,   526,    45,    46,    14,    15,   345,    50,   347,
       8,   329,   350,     8,   448,   449,   354,   276,     6,   278,
     527,   272,    11,    24,   531,    26,    27,   183,    97,    98,
      39,     5,     5,   370,     7,    44,    11,     9,   376,    11,
       8,    14,     9,    11,    11,   144,   398,   399,     9,   387,
      11,   150,   312,    11,   106,   410,   490,   491,    39,    40,
       6,    11,   313,   381,     6,    33,    39,    11,   224,     4,
     139,   389,    40,   229,    11,   393,    40,   395,   416,    33,
     339,     8,     6,   239,   183,    40,   345,   424,   347,   426,
     520,     6,   429,     9,   446,   447,   524,   524,   526,   526,
     524,   153,   526,     8,   524,   442,   526,   366,   524,   178,
     526,     4,     6,   524,   432,   526,   434,   273,   548,    41,
      15,   524,    51,   526,   384,   224,   464,   465,   388,   467,
     229,   469,     8,     9,   186,     8,    43,     8,     8,    50,
     239,    52,   479,   480,     8,   482,   405,   406,     8,     3,
       3,   410,   204,   309,   413,    40,   474,     9,    33,   315,
       6,     8,    41,   524,   502,   526,   322,    41,   486,   507,
     488,     6,     6,   329,   273,    41,   513,     8,     8,     8,
       8,   519,     3,   521,     3,     3,    97,    98,    45,     4,
       4,    11,     8,    49,     6,   533,   534,   456,   535,   459,
     356,     8,   461,    79,    41,    41,     8,     8,   467,    41,
     309,     8,    38,   232,    41,     9,   315,    41,    41,     4,
      10,   240,   241,   322,   276,   381,   278,    49,   139,     4,
     329,    41,     4,   389,     4,   343,     5,   393,   450,   395,
     292,   538,    45,     9,   148,   276,   216,   500,   207,   179,
     161,   402,    77,   496,   410,    -1,    -1,   356,    -1,    -1,
      77,    -1,   418,   522,     4,    -1,   142,   178,    -1,    75,
      -1,    -1,   148,   149,    14,    -1,   432,    -1,   434,    -1,
      -1,    -1,   381,    -1,    -1,    -1,    -1,    -1,   164,    -1,
     389,   310,   311,    -1,   393,    -1,   395,    -1,    -1,    39,
      -1,   212,    42,   179,    44,    45,    46,    47,    48,    49,
      50,   410,    -1,     4,    -1,    -1,    -1,    -1,   474,   418,
      -1,    -1,    -1,    14,    -1,    -1,    -1,     4,    -1,    -1,
     486,    -1,   488,   432,    -1,   434,    -1,    14,   357,   358,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   223,    39,    -1,
      -1,    42,    -1,    44,    45,    46,    47,    48,    49,    50,
      -1,    -1,    39,   382,   383,    42,    -1,    44,    45,    46,
      47,    48,    49,    50,    -1,   474,    -1,    -1,    -1,    -1,
      -1,     3,    -1,     5,    -1,     7,    -1,   486,    -1,   488,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
      22,    23,    24,    25,    -1,    -1,    28,    29,    30,    31,
      14,    33,    34,    35,    -1,    -1,    -1,    39,    40,    -1,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    -1,
      52,    -1,    -1,    -1,    -1,    39,    -1,   313,    42,    -1,
      44,    45,    46,    47,    48,    49,    50,    -1,   324,    -1,
      -1,   327,    -1,     3,   330,     5,    -1,     7,    -1,    -1,
     336,    -1,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    -1,    22,    23,    -1,    -1,    -1,    -1,    28,    29,
      30,    31,    -1,    33,    34,    35,    -1,    -1,    -1,    39,
      40,    -1,    42,    43,    44,    45,    46,    47,    48,    49,
      50,     3,    52,     5,    -1,     7,    -1,    -1,    10,    -1,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
      22,    23,    24,    25,    -1,    -1,    28,    29,    30,    31,
      -1,    -1,    34,    35,    -1,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    44,    -1,     3,    -1,     5,    49,     7,     8,
      52,    -1,    -1,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    -1,    22,    23,    -1,    -1,    -1,    -1,    28,
      29,    30,    31,    -1,    -1,    34,    35,    -1,    -1,    -1,
      39,    -1,    -1,    -1,    43,    44,    -1,     3,    47,     5,
      -1,     7,     8,    52,    -1,    -1,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    22,    23,    -1,    -1,
      -1,    -1,    28,    29,    30,    31,    -1,    -1,    34,    35,
      -1,    -1,    -1,    39,    -1,    -1,    -1,    43,    44,    -1,
       3,    47,     5,    -1,     7,     8,    52,    -1,    -1,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    -1,    22,
      23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,    -1,
      -1,    34,    35,    -1,    -1,    -1,    39,    -1,    -1,    -1,
      43,    44,    -1,     3,    47,     5,    -1,     7,     8,    52,
      -1,    -1,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    -1,    22,    23,    -1,    -1,    -1,    -1,    28,    29,
      30,    31,    -1,    -1,    34,    35,    -1,    -1,    -1,    39,
      -1,    -1,    -1,    43,    44,    -1,     3,    47,     5,    -1,
       7,    -1,    52,    10,    -1,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    -1,    22,    23,    24,    -1,    -1,
      -1,    28,    29,    30,    31,    -1,    -1,    34,    35,    -1,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    44,     3,    -1,
       5,    -1,     7,     8,    -1,    52,    -1,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    -1,    22,    23,    -1,
      -1,    -1,    -1,    28,    29,    30,    31,    -1,    -1,    34,
      35,    -1,    -1,    -1,    39,    -1,    -1,    -1,    -1,    44,
      -1,     3,    47,     5,    -1,     7,    -1,    52,    -1,    -1,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
      22,    23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,
      -1,    -1,    34,    35,    -1,    -1,    -1,    39,    -1,    -1,
      -1,    43,    44,    -1,     3,    47,     5,    -1,     7,    -1,
      52,    -1,    -1,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,    28,
      29,    30,    31,    -1,    -1,    34,    35,    -1,    -1,    -1,
      39,    40,    -1,    -1,    -1,    44,     3,    -1,     5,    -1,
       7,    -1,    -1,    52,    -1,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    -1,    22,    23,    -1,    -1,    -1,
      -1,    28,    29,    30,    31,    -1,    -1,    34,    35,    -1,
      -1,    -1,    39,    -1,    -1,    -1,    43,    44,     3,    -1,
       5,    -1,     7,    -1,    -1,    52,    -1,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    -1,    22,    23,    -1,
      -1,    -1,    -1,    28,    29,    30,    31,    -1,    33,    34,
      35,    -1,    -1,    -1,    39,    -1,    -1,    -1,    -1,    44,
       3,    -1,     5,    -1,     7,    -1,    -1,    52,    -1,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    -1,    22,
      23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,    -1,
      33,    34,    35,    -1,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    44,     3,    -1,     5,    -1,     7,    -1,    -1,    52,
      -1,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      -1,    22,    23,    -1,    -1,    -1,    -1,    28,    29,    30,
      31,    -1,    33,    34,    35,    -1,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    44,     3,    -1,     5,     6,     7,    -1,
      -1,    52,    -1,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    -1,    22,    23,    -1,    -1,    -1,    -1,    28,
      29,    30,    31,    -1,    -1,    34,    35,    -1,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    44,     3,    -1,     5,     6,
       7,    -1,    -1,    52,    -1,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    -1,    22,    23,    -1,    -1,    -1,
      -1,    28,    29,    30,    31,    -1,    -1,    34,    35,    -1,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    44,    -1,     3,
      -1,     5,    -1,     7,    -1,    52,    10,    -1,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    22,    23,
      -1,    -1,    -1,    -1,    28,    29,    30,    31,    -1,    -1,
      34,    35,    -1,    -1,    -1,    39,    -1,    -1,    -1,    -1,
      44,     3,    -1,     5,    -1,     7,    -1,    -1,    52,    -1,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
      22,    23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,
      -1,    -1,    34,    35,     5,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    44,    14,    15,    16,    17,    18,    19,    20,
      52,    22,    -1,    -1,    -1,    -1,    -1,    28,    29,    30,
      -1,    -1,    -1,    34,    -1,    -1,     7,    -1,    39,    -1,
      -1,    12,    -1,    44,    15,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    29,    30,
      -1,    12,    -1,    34,    15,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    44,    -1,    -1,    -1,    28,    29,    30,
      -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    44,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51
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
      47,    96,    97,    98,    99,   149,    90,    76,    94,     8,
      96,    97,    98,    64,    60,   138,   139,     3,    81,    32,
       9,    80,    32,    49,    69,   111,    93,    93,   149,    93,
       8,    11,    33,    93,    93,    93,     8,     8,    69,    93,
     122,   149,     7,    75,   129,     6,    11,    67,     5,    90,
     138,   138,   129,    90,   121,    93,    90,   138,    69,   138,
     118,     7,    12,   103,   127,   133,   137,   141,   142,   143,
     144,   145,   146,    93,     6,    11,    33,    33,    96,     8,
      96,    97,    98,     7,    41,    75,    77,     7,    44,   133,
     138,    90,    75,    93,    11,    67,   100,    11,   100,     8,
      11,    93,     6,    77,   100,   100,    11,    67,     4,    47,
      65,   138,   148,     6,    62,    11,   138,    11,   138,     4,
      40,    93,    93,    93,    33,     8,     8,    96,    97,     6,
     107,   129,     6,     8,     9,     4,    11,    49,    93,     6,
      41,    93,     6,    93,    93,     6,    33,    77,   100,   100,
      67,     8,    96,    97,    98,    90,    93,    47,    98,     8,
       8,    77,    99,     8,     8,     8,   138,   148,     3,     3,
      61,    24,    26,    27,    63,    64,    24,    40,    83,    93,
     138,     9,    83,    11,    49,    93,    33,    93,    75,   100,
     100,     6,   138,     8,    41,    90,    41,     6,     6,    41,
       6,    93,     8,     8,     8,    77,   100,   100,    67,     8,
      93,    67,    41,    77,    77,    77,     3,     3,   112,   112,
       3,    45,    46,    50,   125,   126,   131,   125,    45,   114,
     115,   132,     4,   138,    39,    40,     4,    11,    82,    38,
     138,     4,   138,    93,     8,     8,    49,    90,    90,    41,
      41,    90,    41,     6,    77,    77,     8,     8,     8,    90,
     112,   112,   109,   109,    62,     9,    11,   138,     9,     9,
      67,   138,    41,    93,    93,    83,    93,    38,    49,    77,
      90,    90,    90,    41,    77,    77,   109,   109,    21,    72,
     140,    72,    64,   126,    41,   115,    41,    93,    82,    82,
      82,    93,    90,    72,    72,    10,    71,    73,    93,    66,
      71,     4,   103,   103,    82,    82,    71,    71,    90,     4,
      73,     4,     4,     4,    66
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
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      80,    79,    81,    79,    82,    82,    83,    83,    83,    83,
      83,    83,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    85,    85,    85,
      86,    86,    86,    87,    87,    87,    87,    87,    87,    88,
      88,    88,    88,    88,    89,    89,    89,    89,    89,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    91,    91,    91,    91,    92,    92,    93,
      94,    94,    95,    95,    95,    95,    95,    95,    96,    96,
      97,    97,    98,    98,    99,   100,   100,   101,   101,   102,
     102,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     104,   105,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   108,   108,   109,   109,   110,   110,   111,   112,   112,
     112,   112,   112,   112,   113,   113,   113,   113,   113,   114,
     114,   115,   116,   116,   117,   117,   118,   118,   118,   119,
     119,   120,   120,   121,   121,   121,   122,   122,   123,   123,
     124,   124,   125,   125,   126,   126,   127,   127,   128,   128,
     129,   129,   130,   130,   131,   131,   131,   131,   132,   132,
     133,   133,   134,   134,   135,   135,   136,   137,   137,   137,
     137,   137,   137,   138,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   148,   148,   148,   148,   148,
     148,   148,   149,   150,   150,   151
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
       5,     6,     8,     5,     7,     4,     2,     4,     6,     6,
       5,     5,     7,     8,     7,     6,     6,     8,     7,     4,
       0,     7,     0,     7,     0,     2,     4,     5,     5,     2,
       4,     4,     1,     1,     1,     1,     1,     1,     3,     2,
       3,     3,     4,     3,     1,     4,     1,     5,     5,     6,
       7,     7,     8,     6,     6,     7,     8,     8,     9,     2,
       2,     3,     5,     4,     2,     2,     3,     4,     5,     1,
       1,     1,     1,     5,     2,     4,     3,     4,     5,     7,
       4,     6,     7,     0,     2,     2,     4,     1,     3,     2,
       0,     2,     1,     3,     2,     3,     4,     5,     1,     3,
       2,     4,     1,     3,     2,     1,     3,     1,     3,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     5,     5,     0,     2,     1,     2,     3,     0,     3,
       5,     3,     5,     7,     3,     5,     3,     5,     7,     1,
       3,     4,     0,     1,     1,     3,     1,     3,     5,     0,
       1,     1,     3,     1,     3,     4,     3,     2,     1,     3,
       0,     2,     1,     3,     2,     4,     4,     5,     3,     4,
       0,     2,     1,     3,     0,     1,     1,     1,     0,     1,
       1,     2,     1,     2,     1,     2,     1,     1,     1,     2,
       2,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1
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
#line 2233 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 3: /* root: classextensions  */
#line 51 "lang/LangSource/Bison/lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; }
#line 2239 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 4: /* root: INTERPRET cmdlinecode  */
#line 53 "lang/LangSource/Bison/lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 2; }
#line 2245 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 5: /* classes: %empty  */
#line 56 "lang/LangSource/Bison/lang11d"
          { yyval = 0; }
#line 2251 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 6: /* classes: classes classdef  */
#line 58 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2257 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 8: /* classextensions: classextensions classextension  */
#line 63 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2263 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 9: /* classdef: classname superclass OPENCURLY classvardecls methods CLOSECURLY  */
#line 67 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrClassNode((yyloc), (PyrSlotNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1], 0);
				}
#line 2271 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 10: /* classdef: classname OPENSQUARE optname CLOSESQUARE superclass OPENCURLY classvardecls methods CLOSECURLY  */
#line 71 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrClassNode((yyloc),(PyrSlotNode*)yyvsp[-8], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1],
					(PyrSlotNode*)yyvsp[-6]);
				}
#line 2280 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 11: /* classextension: ADD classname OPENCURLY methods CLOSECURLY  */
#line 78 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrClassExtNode((yyloc),(PyrSlotNode*)yyvsp[-3], (PyrMethodNode*)yyvsp[-1]);
				}
#line 2288 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 12: /* optname: %empty  */
#line 83 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 2294 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 14: /* superclass: %empty  */
#line 87 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 2300 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 15: /* superclass: COLON classname  */
#line 89 "lang/LangSource/Bison/lang11d"
                                { yyval = yyvsp[0]; }
#line 2306 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 16: /* classvardecls: %empty  */
#line 92 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 2312 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 17: /* classvardecls: classvardecls classvardecl  */
#line 94 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2318 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 18: /* classvardecl: CLASSVAR rwslotdeflist SEMICOLON  */
#line 98 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], varClass); }
#line 2324 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 19: /* classvardecl: VAR rwslotdeflist SEMICOLON  */
#line 100 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], varInst); }
#line 2330 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 20: /* classvardecl: SC_CONST constdeflist SEMICOLON  */
#line 102 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], varConst); }
#line 2336 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 21: /* methods: %empty  */
#line 105 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 2342 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 22: /* methods: methods methoddef  */
#line 107 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2348 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 23: /* methoddef: name OPENCURLY argdecls funcvardecls primitive methbody CLOSECURLY  */
#line 111 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((yyloc), (PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2355 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 24: /* methoddef: MULTIPLY name OPENCURLY argdecls funcvardecls primitive methbody CLOSECURLY  */
#line 114 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((yyloc), (PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2362 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 25: /* methoddef: binop OPENCURLY argdecls funcvardecls primitive methbody CLOSECURLY  */
#line 117 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((yyloc), (PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2369 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 26: /* methoddef: MULTIPLY binop OPENCURLY argdecls funcvardecls primitive methbody CLOSECURLY  */
#line 120 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((yyloc), (PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2376 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 34: /* funcbody: exprseq funretval  */
#line 138 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrDropNode((yyloc), (PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2382 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 35: /* cmdlinecode: OPENPAREN argdecls1 funcvardecls1 funcbody CLOSEPAREN  */
#line 142 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), (PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2388 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 36: /* cmdlinecode: OPENPAREN argdecls1 funcbody CLOSEPAREN  */
#line 144 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), (PyrArgListNode*)yyvsp[-2], NULL, (PyrParseNode*)yyvsp[-1], false); }
#line 2394 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 37: /* cmdlinecode: OPENPAREN funcvardecls1 funcbody CLOSEPAREN  */
#line 146 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), NULL, (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2400 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 38: /* cmdlinecode: argdecls1 funcvardecls1 funcbody  */
#line 148 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), (PyrArgListNode*)yyvsp[-2], (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2406 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 39: /* cmdlinecode: argdecls1 funcbody  */
#line 150 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), (PyrArgListNode*)yyvsp[-1], NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2412 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 40: /* cmdlinecode: funcvardecls1 funcbody  */
#line 152 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), NULL, (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2418 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 41: /* cmdlinecode: funcbody  */
#line 154 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), NULL, NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2424 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 43: /* methbody: exprseq retval  */
#line 159 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrDropNode((yyloc), (PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2430 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 44: /* primitive: %empty  */
#line 162 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 2436 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 45: /* primitive: primname optsemi  */
#line 164 "lang/LangSource/Bison/lang11d"
                                { yyval = yyvsp[-1]; }
#line 2442 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 46: /* retval: %empty  */
#line 168 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((yyloc), NULL); }
#line 2448 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 47: /* retval: NONLOCALRETURN expr optsemi  */
#line 170 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((yyloc), (PyrParseNode*)yyvsp[-1]); }
#line 2454 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 48: /* funretval: %empty  */
#line 174 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrBlockReturnNode((yyloc)); }
#line 2460 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 49: /* funretval: NONLOCALRETURN expr optsemi  */
#line 176 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((yyloc), (PyrParseNode*)yyvsp[-1]); }
#line 2466 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 51: /* blocklist1: blocklist1 blocklistitem  */
#line 181 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
				}
#line 2474 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 54: /* blocklist: %empty  */
#line 190 "lang/LangSource/Bison/lang11d"
                        { yyval = 0; }
#line 2480 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 56: /* msgsend: name blocklist1  */
#line 195 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2488 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 57: /* msgsend: OPENPAREN binop2 CLOSEPAREN blocklist1  */
#line 199 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2496 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 58: /* msgsend: name OPENPAREN CLOSEPAREN blocklist1  */
#line 203 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-3], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2504 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 59: /* msgsend: name OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 207 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2513 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 60: /* msgsend: name OPENPAREN keyarglist1 CLOSEPAREN blocklist  */
#line 212 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-4], nullptr,
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2522 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 61: /* msgsend: OPENPAREN binop2 CLOSEPAREN OPENPAREN CLOSEPAREN blocklist1  */
#line 217 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-4], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2530 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 62: /* msgsend: OPENPAREN binop2 CLOSEPAREN OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 221 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2539 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 63: /* msgsend: name OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 226 "lang/LangSource/Bison/lang11d"
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
#line 2561 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 64: /* msgsend: OPENPAREN binop2 CLOSEPAREN OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 244 "lang/LangSource/Bison/lang11d"
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
#line 2578 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 65: /* msgsend: classname OPENSQUARE arrayelems CLOSESQUARE  */
#line 257 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrDynListNode((yyloc), (PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 2584 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 66: /* msgsend: classname blocklist1  */
#line 259 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = (PyrParseNode*)newPyrPushNameNode((yyloc), (PyrSlotNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, 0, (PyrParseNode*)yyvsp[0]);
			}
#line 2599 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 67: /* msgsend: classname OPENPAREN CLOSEPAREN blocklist  */
#line 270 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = (PyrParseNode*)newPyrPushNameNode((yyloc), (PyrSlotNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2614 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 68: /* msgsend: classname OPENPAREN keyarglist1 optcomma CLOSEPAREN blocklist  */
#line 281 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode((yyloc), &slot);
				args = (PyrParseNode*)newPyrPushNameNode((yyloc), (PyrSlotNode*)yyvsp[-5]);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args, (PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2629 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 69: /* msgsend: classname OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 292 "lang/LangSource/Bison/lang11d"
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
#line 2646 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 70: /* msgsend: classname OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 305 "lang/LangSource/Bison/lang11d"
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
#line 2671 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 71: /* msgsend: expr DOT OPENPAREN CLOSEPAREN blocklist  */
#line 326 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode((yyloc), &slot);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, (PyrParseNode*)yyvsp[-4], NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2684 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 72: /* msgsend: expr DOT OPENPAREN keyarglist1 optcomma CLOSEPAREN blocklist  */
#line 335 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode((yyloc), &slot);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, (PyrParseNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2697 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 73: /* msgsend: expr DOT name OPENPAREN keyarglist1 optcomma CLOSEPAREN blocklist  */
#line 344 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-7],
					(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2706 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 74: /* msgsend: expr DOT OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 349 "lang/LangSource/Bison/lang11d"
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
#line 2723 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 75: /* msgsend: expr DOT OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 363 "lang/LangSource/Bison/lang11d"
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
#line 2747 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 76: /* msgsend: expr DOT name OPENPAREN CLOSEPAREN blocklist  */
#line 385 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-5], NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2755 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 77: /* msgsend: expr DOT name OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 389 "lang/LangSource/Bison/lang11d"
                        {
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-5], args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2765 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 78: /* msgsend: expr DOT name OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 395 "lang/LangSource/Bison/lang11d"
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
#line 2787 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 79: /* msgsend: expr DOT name blocklist  */
#line 413 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((yyloc), (PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[-3], 0, (PyrParseNode*)yyvsp[0]);
			}
#line 2795 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 80: /* $@1: %empty  */
#line 418 "lang/LangSource/Bison/lang11d"
                                    { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 1); }
#line 2801 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 81: /* generator: OPENCURLY COLON exprseq $@1 COMMA qual CLOSECURLY  */
#line 419 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, getsym("r"));
				PyrSlotNode* selectornode = newPyrSlotNode((yyloc), &slot);

				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode((yyloc), 0, 0, (PyrParseNode*)yyvsp[-1], false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode((yyloc), NULL, block);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, (PyrParseNode*)blocklit, 0, 0);
			}
#line 2815 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 82: /* $@2: %empty  */
#line 428 "lang/LangSource/Bison/lang11d"
                                              { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 2); }
#line 2821 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 83: /* generator: OPENCURLY SEMICOLON exprseq $@2 COMMA qual CLOSECURLY  */
#line 429 "lang/LangSource/Bison/lang11d"
                        {
				yyval = yyvsp[-1];
			}
#line 2829 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 84: /* nextqual: %empty  */
#line 435 "lang/LangSource/Bison/lang11d"
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
#line 2855 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 85: /* nextqual: COMMA qual  */
#line 457 "lang/LangSource/Bison/lang11d"
                                { yyval = yyvsp[0]; }
#line 2861 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 86: /* qual: name LEFTARROW exprseq nextqual  */
#line 461 "lang/LangSource/Bison/lang11d"
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
#line 2899 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 87: /* qual: name name LEFTARROW exprseq nextqual  */
#line 495 "lang/LangSource/Bison/lang11d"
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
#line 2941 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 88: /* qual: VAR name EQUALSSIGN exprseq nextqual  */
#line 533 "lang/LangSource/Bison/lang11d"
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
#line 2959 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 89: /* qual: exprseq nextqual  */
#line 547 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, getsym("if"));
				PyrSlotNode* selectornode = newPyrSlotNode((yyloc), &slot);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode((yyloc), 0, 0, (PyrParseNode*)yyvsp[0], false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode((yyloc), NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-1], blocklit);

				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, args2, 0, 0);
			}
#line 2974 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 90: /* qual: COLON COLON exprseq nextqual  */
#line 558 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((yyloc), (PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
			}
#line 2982 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 91: /* qual: COLON WHILE exprseq nextqual  */
#line 562 "lang/LangSource/Bison/lang11d"
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
#line 3009 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 98: /* expr1: OPENPAREN exprseq CLOSEPAREN  */
#line 593 "lang/LangSource/Bison/lang11d"
                        {
				PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
				node->mParens = 1;
				yyval = yyvsp[-1];
				node->location = (yyloc);
			}
#line 3020 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 99: /* expr1: TILDE name  */
#line 600 "lang/LangSource/Bison/lang11d"
                        {
				PyrParseNode* argnode;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL);
				SetSymbol(&slot, s_envirGet);
				selectornode = newPyrSlotNode((yyloc), &slot);
				yyval = (intptr_t)newPyrCallNode((yyloc), selectornode, argnode, 0, 0);
			}
#line 3034 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 100: /* expr1: OPENSQUARE arrayelems CLOSESQUARE  */
#line 610 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrDynListNode((yyloc), 0, (PyrParseNode*)yyvsp[-1]); }
#line 3040 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 101: /* expr1: OPENPAREN valrange2 CLOSEPAREN  */
#line 612 "lang/LangSource/Bison/lang11d"
                        { yyval = yyvsp[-1]; }
#line 3046 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 102: /* expr1: OPENPAREN COLON valrange3 CLOSEPAREN  */
#line 614 "lang/LangSource/Bison/lang11d"
                        { yyval = yyvsp[-1]; }
#line 3052 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 103: /* expr1: OPENPAREN dictslotlist CLOSEPAREN  */
#line 616 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrDynDictNode((yyloc), (PyrParseNode*)yyvsp[-1]); }
#line 3058 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 104: /* expr1: pseudovar  */
#line 618 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrPushNameNode((yyloc), (PyrSlotNode*)yyvsp[0]); }
#line 3064 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 105: /* expr1: expr1 OPENSQUARE arglist1 CLOSESQUARE  */
#line 620 "lang/LangSource/Bison/lang11d"
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
#line 3081 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 107: /* valrangex1: expr1 OPENSQUARE arglist1 DOTDOT CLOSESQUARE  */
#line 636 "lang/LangSource/Bison/lang11d"
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
#line 3112 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 108: /* valrangex1: expr1 OPENSQUARE DOTDOT exprseq CLOSESQUARE  */
#line 663 "lang/LangSource/Bison/lang11d"
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
#line 3134 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 109: /* valrangex1: expr1 OPENSQUARE arglist1 DOTDOT exprseq CLOSESQUARE  */
#line 681 "lang/LangSource/Bison/lang11d"
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
#line 3163 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 110: /* valrangeassign: expr1 OPENSQUARE arglist1 DOTDOT CLOSESQUARE EQUALSSIGN expr  */
#line 708 "lang/LangSource/Bison/lang11d"
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
#line 3195 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 111: /* valrangeassign: expr1 OPENSQUARE DOTDOT exprseq CLOSESQUARE EQUALSSIGN expr  */
#line 736 "lang/LangSource/Bison/lang11d"
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
#line 3218 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 112: /* valrangeassign: expr1 OPENSQUARE arglist1 DOTDOT exprseq CLOSESQUARE EQUALSSIGN expr  */
#line 755 "lang/LangSource/Bison/lang11d"
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
#line 3248 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 113: /* valrangexd: expr DOT OPENSQUARE arglist1 DOTDOT CLOSESQUARE  */
#line 783 "lang/LangSource/Bison/lang11d"
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
#line 3279 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 114: /* valrangexd: expr DOT OPENSQUARE DOTDOT exprseq CLOSESQUARE  */
#line 810 "lang/LangSource/Bison/lang11d"
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
#line 3301 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 115: /* valrangexd: expr DOT OPENSQUARE arglist1 DOTDOT exprseq CLOSESQUARE  */
#line 828 "lang/LangSource/Bison/lang11d"
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
#line 3330 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 116: /* valrangexd: expr DOT OPENSQUARE arglist1 DOTDOT CLOSESQUARE EQUALSSIGN expr  */
#line 853 "lang/LangSource/Bison/lang11d"
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
#line 3362 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 117: /* valrangexd: expr DOT OPENSQUARE DOTDOT exprseq CLOSESQUARE EQUALSSIGN expr  */
#line 881 "lang/LangSource/Bison/lang11d"
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
#line 3385 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 118: /* valrangexd: expr DOT OPENSQUARE arglist1 DOTDOT exprseq CLOSESQUARE EQUALSSIGN expr  */
#line 900 "lang/LangSource/Bison/lang11d"
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
#line 3415 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 119: /* valrange2: exprseq DOTDOT  */
#line 928 "lang/LangSource/Bison/lang11d"
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
#line 3437 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 120: /* valrange2: DOTDOT exprseq  */
#line 947 "lang/LangSource/Bison/lang11d"
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
#line 3459 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 121: /* valrange2: exprseq DOTDOT exprseq  */
#line 966 "lang/LangSource/Bison/lang11d"
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
#line 3479 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 122: /* valrange2: exprseq COMMA exprseq DOTDOT exprseq  */
#line 983 "lang/LangSource/Bison/lang11d"
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
#line 3497 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 123: /* valrange2: exprseq COMMA exprseq DOTDOT  */
#line 997 "lang/LangSource/Bison/lang11d"
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
#line 3520 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 124: /* valrange3: DOTDOT exprseq  */
#line 1018 "lang/LangSource/Bison/lang11d"
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
#line 3542 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 125: /* valrange3: exprseq DOTDOT  */
#line 1037 "lang/LangSource/Bison/lang11d"
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
#line 3563 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 126: /* valrange3: exprseq DOTDOT exprseq  */
#line 1055 "lang/LangSource/Bison/lang11d"
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
#line 3583 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 127: /* valrange3: exprseq COMMA exprseq DOTDOT  */
#line 1072 "lang/LangSource/Bison/lang11d"
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
#line 3603 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 128: /* valrange3: exprseq COMMA exprseq DOTDOT exprseq  */
#line 1088 "lang/LangSource/Bison/lang11d"
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
#line 3621 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 132: /* expr: classname  */
#line 1106 "lang/LangSource/Bison/lang11d"
                            { yyval = (intptr_t)newPyrPushNameNode((yyloc), (PyrSlotNode*)yyvsp[0]); }
#line 3627 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 133: /* expr: expr DOT OPENSQUARE arglist1 CLOSESQUARE  */
#line 1108 "lang/LangSource/Bison/lang11d"
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
#line 3644 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 134: /* expr: BACKTICK expr  */
#line 1121 "lang/LangSource/Bison/lang11d"
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
#line 3662 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 135: /* expr: expr binop2 adverb expr  */
#line 1135 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrBinopCallNode((yyloc), (PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0], (PyrParseNode*)yyvsp[-1]);
			}
#line 3671 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 136: /* expr: name EQUALSSIGN expr  */
#line 1140 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrAssignNode((yyloc), (PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0);
			}
#line 3679 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 137: /* expr: TILDE name EQUALSSIGN expr  */
#line 1144 "lang/LangSource/Bison/lang11d"
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
#line 3694 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 138: /* expr: expr DOT name EQUALSSIGN expr  */
#line 1155 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrSetterNode((yyloc), (PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3703 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 139: /* expr: name OPENPAREN arglist1 optkeyarglist CLOSEPAREN EQUALSSIGN expr  */
#line 1160 "lang/LangSource/Bison/lang11d"
                        {
				if (yyvsp[-3] != 0) {
					error("Setter method called with keyword arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-3]);
					compileErrors++;
				}
				yyval = (intptr_t)newPyrSetterNode((yyloc), (PyrSlotNode*)yyvsp[-6],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3717 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 140: /* expr: HASH mavars EQUALSSIGN expr  */
#line 1170 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrMultiAssignNode((yyloc), (PyrMultiAssignVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[0], 0);
			}
#line 3726 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 141: /* expr: expr1 OPENSQUARE arglist1 CLOSESQUARE EQUALSSIGN expr  */
#line 1175 "lang/LangSource/Bison/lang11d"
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
#line 3744 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 142: /* expr: expr DOT OPENSQUARE arglist1 CLOSESQUARE EQUALSSIGN expr  */
#line 1189 "lang/LangSource/Bison/lang11d"
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
#line 3762 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 143: /* adverb: %empty  */
#line 1204 "lang/LangSource/Bison/lang11d"
          { yyval = 0; }
#line 3768 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 144: /* adverb: DOT name  */
#line 1205 "lang/LangSource/Bison/lang11d"
                           { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3774 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 145: /* adverb: DOT integer  */
#line 1206 "lang/LangSource/Bison/lang11d"
                              { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3780 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 146: /* adverb: DOT OPENPAREN exprseq CLOSEPAREN  */
#line 1207 "lang/LangSource/Bison/lang11d"
                                                   { yyval = yyvsp[-1]; }
#line 3786 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 148: /* exprn: exprn SEMICOLON expr  */
#line 1212 "lang/LangSource/Bison/lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((yyloc), (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 3794 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 150: /* arrayelems: %empty  */
#line 1220 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 3800 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 151: /* arrayelems: arrayelems1 optcomma  */
#line 1222 "lang/LangSource/Bison/lang11d"
                          { yyval = yyvsp[-1]; }
#line 3806 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 153: /* arrayelems1: exprseq COLON exprseq  */
#line 1227 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3812 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 154: /* arrayelems1: keybinop exprseq  */
#line 1229 "lang/LangSource/Bison/lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 3821 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 155: /* arrayelems1: arrayelems1 COMMA exprseq  */
#line 1234 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3827 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 156: /* arrayelems1: arrayelems1 COMMA keybinop exprseq  */
#line 1236 "lang/LangSource/Bison/lang11d"
                                {
					PyrParseNode* elems;
					PyrParseNode* key = newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[-1], NULL);
					elems = (PyrParseNode*)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], elems);
				}
#line 3838 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 157: /* arrayelems1: arrayelems1 COMMA exprseq COLON exprseq  */
#line 1243 "lang/LangSource/Bison/lang11d"
                                {
					PyrParseNode* elems;
					elems = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-4], elems);
				}
#line 3848 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 159: /* arglist1: arglist1 COMMA exprseq  */
#line 1252 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3854 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 160: /* arglistv1: MULTIPLY exprseq  */
#line 1256 "lang/LangSource/Bison/lang11d"
                                { yyval = yyvsp[0]; }
#line 3860 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 161: /* arglistv1: arglist1 COMMA MULTIPLY exprseq  */
#line 1258 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]); }
#line 3866 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 163: /* keyarglist1: keyarglist1 COMMA keyarg  */
#line 1263 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3872 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 164: /* keyarg: keybinop exprseq  */
#line 1267 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrPushKeyArgNode((yyloc), (PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 3878 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 165: /* optkeyarglist: optcomma  */
#line 1270 "lang/LangSource/Bison/lang11d"
                           { yyval = 0; }
#line 3884 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 166: /* optkeyarglist: COMMA keyarglist1 optcomma  */
#line 1271 "lang/LangSource/Bison/lang11d"
                                                             { yyval = yyvsp[-1]; }
#line 3890 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 167: /* mavars: mavarlist  */
#line 1275 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3896 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 168: /* mavars: mavarlist ELLIPSIS name  */
#line 1277 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((yyloc), (PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); }
#line 3902 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 170: /* mavarlist: mavarlist COMMA name  */
#line 1282 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3908 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 171: /* slotliteral: integer  */
#line 1286 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3914 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 172: /* slotliteral: floatp  */
#line 1287 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3920 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 173: /* slotliteral: ascii  */
#line 1288 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3926 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 174: /* slotliteral: string  */
#line 1289 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3932 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 175: /* slotliteral: symbol  */
#line 1290 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3938 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 176: /* slotliteral: trueobj  */
#line 1291 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3944 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 177: /* slotliteral: falseobj  */
#line 1292 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3950 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 178: /* slotliteral: nilobj  */
#line 1293 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3956 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 179: /* slotliteral: listlit  */
#line 1294 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), NULL, (PyrParseNode*)yyvsp[0]); }
#line 3962 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 180: /* blockliteral: block  */
#line 1297 "lang/LangSource/Bison/lang11d"
                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), NULL, (PyrParseNode*)yyvsp[0]); }
#line 3968 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 181: /* pushname: name  */
#line 1300 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrPushNameNode((yyloc), (PyrSlotNode*)yyvsp[0]); }
#line 3974 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 182: /* pushliteral: integer  */
#line 1303 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3980 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 183: /* pushliteral: floatp  */
#line 1304 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3986 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 184: /* pushliteral: ascii  */
#line 1305 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3992 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 185: /* pushliteral: string  */
#line 1306 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 3998 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 186: /* pushliteral: symbol  */
#line 1307 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4004 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 187: /* pushliteral: trueobj  */
#line 1308 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4010 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 188: /* pushliteral: falseobj  */
#line 1309 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4016 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 189: /* pushliteral: nilobj  */
#line 1310 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4022 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 190: /* pushliteral: listlit  */
#line 1311 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((yyloc), NULL, (PyrParseNode*)yyvsp[0]); }
#line 4028 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 191: /* listliteral: integer  */
#line 1314 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4034 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 192: /* listliteral: floatp  */
#line 1315 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4040 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 193: /* listliteral: ascii  */
#line 1316 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4046 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 194: /* listliteral: string  */
#line 1317 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4052 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 195: /* listliteral: symbol  */
#line 1318 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4058 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 196: /* listliteral: name  */
#line 1319 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4064 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 197: /* listliteral: trueobj  */
#line 1320 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4070 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 198: /* listliteral: falseobj  */
#line 1321 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4076 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 199: /* listliteral: nilobj  */
#line 1322 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL); }
#line 4082 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 200: /* listliteral: listlit2  */
#line 1323 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((yyloc), NULL, (PyrParseNode*)yyvsp[0]); }
#line 4088 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 201: /* block: OPENCURLY argdecls funcvardecls funcbody CLOSECURLY  */
#line 1327 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), (PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], false); }
#line 4095 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 202: /* block: BEGINCLOSEDFUNC argdecls funcvardecls funcbody CLOSECURLY  */
#line 1330 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((yyloc), (PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], true); }
#line 4102 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 203: /* funcvardecls: %empty  */
#line 1334 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 4108 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 204: /* funcvardecls: funcvardecls funcvardecl  */
#line 1336 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 4114 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 206: /* funcvardecls1: funcvardecls1 funcvardecl  */
#line 1341 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 4120 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 207: /* funcvardecl: VAR vardeflist SEMICOLON  */
#line 1345 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrVarListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], varLocal); }
#line 4126 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 208: /* argdecls: %empty  */
#line 1348 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 4132 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 209: /* argdecls: ARG vardeflist SEMICOLON  */
#line 1350 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4140 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 210: /* argdecls: ARG vardeflist0 ELLIPSIS name SEMICOLON  */
#line 1354 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4148 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 211: /* argdecls: PIPE slotdeflist PIPE  */
#line 1358 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4156 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 212: /* argdecls: PIPE slotdeflist0 ELLIPSIS name PIPE  */
#line 1362 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4164 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 213: /* argdecls: PIPE slotdeflist0 ELLIPSIS name COMMA name PIPE  */
#line 1366 "lang/LangSource/Bison/lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 4172 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 214: /* argdecls1: ARG vardeflist SEMICOLON  */
#line 1372 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4180 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 215: /* argdecls1: ARG vardeflist0 ELLIPSIS name SEMICOLON  */
#line 1376 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4188 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 216: /* argdecls1: PIPE slotdeflist PIPE  */
#line 1380 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4196 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 217: /* argdecls1: PIPE slotdeflist0 ELLIPSIS name PIPE  */
#line 1384 "lang/LangSource/Bison/lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4204 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 218: /* argdecls1: PIPE slotdeflist0 ELLIPSIS name COMMA name PIPE  */
#line 1388 "lang/LangSource/Bison/lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((yyloc), (PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 4212 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 220: /* constdeflist: constdeflist optcomma constdef  */
#line 1396 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4218 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 221: /* constdef: rspec name EQUALSSIGN slotliteral  */
#line 1400 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4224 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 222: /* slotdeflist0: %empty  */
#line 1403 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 4230 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 225: /* slotdeflist: slotdeflist optcomma slotdef  */
#line 1409 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4236 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 226: /* slotdef: name  */
#line 1413 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4242 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 227: /* slotdef: name optequal slotliteral  */
#line 1415 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4248 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 228: /* slotdef: name optequal OPENPAREN exprseq CLOSEPAREN  */
#line 1417 "lang/LangSource/Bison/lang11d"
                                {
					PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
					node->mParens = 1;
					yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-4], node, 0);
				}
#line 4258 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 229: /* vardeflist0: %empty  */
#line 1424 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 4264 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 232: /* vardeflist: vardeflist COMMA vardef  */
#line 1430 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4270 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 233: /* vardef: name  */
#line 1434 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4276 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 234: /* vardef: name EQUALSSIGN expr  */
#line 1436 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4282 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 235: /* vardef: name OPENPAREN exprseq CLOSEPAREN  */
#line 1438 "lang/LangSource/Bison/lang11d"
                                {
									PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
									node->mParens = 1;
									yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-3], node, 0);
								}
#line 4292 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 236: /* dictslotdef: exprseq COLON exprseq  */
#line 1446 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4298 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 237: /* dictslotdef: keybinop exprseq  */
#line 1448 "lang/LangSource/Bison/lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((yyloc), (PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 4307 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 239: /* dictslotlist1: dictslotlist1 COMMA dictslotdef  */
#line 1456 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4313 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 240: /* dictslotlist: %empty  */
#line 1459 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 4319 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 243: /* rwslotdeflist: rwslotdeflist COMMA rwslotdef  */
#line 1465 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4325 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 244: /* rwslotdef: rwspec name  */
#line 1469 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); }
#line 4331 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 245: /* rwslotdef: rwspec name EQUALSSIGN slotliteral  */
#line 1471 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((yyloc), (PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4337 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 246: /* listlit: HASH OPENSQUARE literallistc CLOSESQUARE  */
#line 1475 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((yyloc), 0, (PyrParseNode*)yyvsp[-1]); }
#line 4343 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 247: /* listlit: HASH classname OPENSQUARE literallistc CLOSESQUARE  */
#line 1477 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((yyloc), (PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4349 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 248: /* listlit2: OPENSQUARE literallistc CLOSESQUARE  */
#line 1481 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((yyloc), 0, (PyrParseNode*)yyvsp[-1]); }
#line 4355 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 249: /* listlit2: classname OPENSQUARE literallistc CLOSESQUARE  */
#line 1483 "lang/LangSource/Bison/lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((yyloc), (PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4361 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 250: /* literallistc: %empty  */
#line 1486 "lang/LangSource/Bison/lang11d"
                  { yyval = 0; }
#line 4367 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 253: /* literallist1: literallist1 COMMA listliteral  */
#line 1492 "lang/LangSource/Bison/lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4373 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 254: /* rwspec: %empty  */
#line 1495 "lang/LangSource/Bison/lang11d"
           { yyval = rwPrivate; }
#line 4379 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 255: /* rwspec: LESSTHAN  */
#line 1497 "lang/LangSource/Bison/lang11d"
                        { yyval = rwReadOnly; }
#line 4385 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 256: /* rwspec: READWRITEVAR  */
#line 1499 "lang/LangSource/Bison/lang11d"
                        { yyval = rwReadWrite; }
#line 4391 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 257: /* rwspec: GREATERTHAN  */
#line 1501 "lang/LangSource/Bison/lang11d"
                        { yyval = rwWriteOnly; }
#line 4397 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 258: /* rspec: %empty  */
#line 1504 "lang/LangSource/Bison/lang11d"
           { yyval = rwPrivate; }
#line 4403 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 259: /* rspec: LESSTHAN  */
#line 1506 "lang/LangSource/Bison/lang11d"
                        { yyval = rwReadOnly; }
#line 4409 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 260: /* integer: INTEGER  */
#line 1509 "lang/LangSource/Bison/lang11d"
                  { yyval = zzval; }
#line 4415 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 261: /* integer: MINUS INTEGER  */
#line 1511 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawInt(&node->mSlot));
				yyval = zzval;
			}
#line 4426 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 262: /* floatr: SC_FLOAT  */
#line 1519 "lang/LangSource/Bison/lang11d"
                   { yyval = zzval; }
#line 4432 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 263: /* floatr: MINUS SC_FLOAT  */
#line 1521 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawFloat(&node->mSlot));
				yyval = zzval;
			}
#line 4443 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 264: /* accidental: ACCIDENTAL  */
#line 1529 "lang/LangSource/Bison/lang11d"
                        { yyval = zzval; }
#line 4449 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 265: /* accidental: MINUS ACCIDENTAL  */
#line 1531 "lang/LangSource/Bison/lang11d"
                                {
					PyrSlotNode *node;
					double intval, fracval;
					node = (PyrSlotNode*)zzval;
					intval = floor(slotRawFloat(&node->mSlot) + 0.5);
					fracval = slotRawFloat(&node->mSlot) - intval;
					SetRaw(&node->mSlot, -intval + fracval);
					yyval = zzval;
				}
#line 4463 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 266: /* pie: PIE  */
#line 1541 "lang/LangSource/Bison/lang11d"
                      { yyval = zzval; }
#line 4469 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 269: /* floatp: floatr pie  */
#line 1547 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)yyvsp[-1];
				SetRaw(&node->mSlot, slotRawFloat(&node->mSlot) * pi);
			}
#line 4479 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 270: /* floatp: integer pie  */
#line 1553 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *node;
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = slotRawInt(&node->mSlot);
				SetFloat(&node->mSlot, ival * pi);
			}
#line 4491 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 271: /* floatp: pie  */
#line 1561 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, pi);
				yyval = zzval;
			}
#line 4502 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 272: /* floatp: MINUS pie  */
#line 1568 "lang/LangSource/Bison/lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, -pi);
				yyval = zzval;
			}
#line 4513 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 273: /* name: NAME  */
#line 1576 "lang/LangSource/Bison/lang11d"
                       { yyval = zzval; }
#line 4519 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 274: /* name: WHILE  */
#line 1577 "lang/LangSource/Bison/lang11d"
                                { yyval = zzval; }
#line 4525 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 275: /* classname: CLASSNAME  */
#line 1580 "lang/LangSource/Bison/lang11d"
                                    { yyval = zzval; }
#line 4531 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 276: /* primname: PRIMITIVENAME  */
#line 1583 "lang/LangSource/Bison/lang11d"
                                        { yyval = zzval; }
#line 4537 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 277: /* trueobj: TRUEOBJ  */
#line 1586 "lang/LangSource/Bison/lang11d"
                          { yyval = zzval; }
#line 4543 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 278: /* falseobj: FALSEOBJ  */
#line 1589 "lang/LangSource/Bison/lang11d"
                           { yyval = zzval; }
#line 4549 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 279: /* nilobj: NILOBJ  */
#line 1592 "lang/LangSource/Bison/lang11d"
                         { yyval = zzval; }
#line 4555 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 280: /* ascii: ASCII  */
#line 1595 "lang/LangSource/Bison/lang11d"
                        { yyval = zzval; }
#line 4561 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 281: /* symbol: SYMBOL  */
#line 1598 "lang/LangSource/Bison/lang11d"
                         { yyval = zzval; }
#line 4567 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 282: /* string: STRING  */
#line 1601 "lang/LangSource/Bison/lang11d"
                         { yyval = zzval; }
#line 4573 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 283: /* pseudovar: PSEUDOVAR  */
#line 1604 "lang/LangSource/Bison/lang11d"
                            { yyval = zzval; }
#line 4579 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 284: /* binop: BINOP  */
#line 1607 "lang/LangSource/Bison/lang11d"
                { yyval = zzval; }
#line 4585 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 285: /* binop: READWRITEVAR  */
#line 1608 "lang/LangSource/Bison/lang11d"
                               { yyval = zzval; }
#line 4591 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 286: /* binop: LESSTHAN  */
#line 1609 "lang/LangSource/Bison/lang11d"
                            { yyval = zzval; }
#line 4597 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 287: /* binop: GREATERTHAN  */
#line 1610 "lang/LangSource/Bison/lang11d"
                               { yyval = zzval; }
#line 4603 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 288: /* binop: MINUS  */
#line 1611 "lang/LangSource/Bison/lang11d"
                         { yyval = zzval; }
#line 4609 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 289: /* binop: MULTIPLY  */
#line 1612 "lang/LangSource/Bison/lang11d"
                            { yyval = zzval; }
#line 4615 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 290: /* binop: ADD  */
#line 1613 "lang/LangSource/Bison/lang11d"
                       { yyval = zzval; }
#line 4621 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 291: /* binop: PIPE  */
#line 1614 "lang/LangSource/Bison/lang11d"
                        { yyval = zzval; }
#line 4627 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 292: /* keybinop: KEYBINOP  */
#line 1617 "lang/LangSource/Bison/lang11d"
                    { yyval = zzval; }
#line 4633 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 295: /* curryarg: CURRYARG  */
#line 1624 "lang/LangSource/Bison/lang11d"
                    { yyval =(intptr_t)newPyrCurryArgNode((yyloc)); }
#line 4639 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;


#line 4643 "lang/LangSource/Bison/lang11d_tab.cpp"

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

