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



#line 92 "lang11d_tab.cpp"

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
  YYSYMBOL_PRIMITIVENAME = 10,             /* PRIMITIVENAME  */
  YYSYMBOL_CLASSNAME = 11,                 /* CLASSNAME  */
  YYSYMBOL_CURRYARG = 12,                  /* CURRYARG  */
  YYSYMBOL_VAR = 13,                       /* VAR  */
  YYSYMBOL_ARG = 14,                       /* ARG  */
  YYSYMBOL_CLASSVAR = 15,                  /* CLASSVAR  */
  YYSYMBOL_SC_CONST = 16,                  /* SC_CONST  */
  YYSYMBOL_NILOBJ = 17,                    /* NILOBJ  */
  YYSYMBOL_TRUEOBJ = 18,                   /* TRUEOBJ  */
  YYSYMBOL_FALSEOBJ = 19,                  /* FALSEOBJ  */
  YYSYMBOL_PSEUDOVAR = 20,                 /* PSEUDOVAR  */
  YYSYMBOL_ELLIPSIS = 21,                  /* ELLIPSIS  */
  YYSYMBOL_DOTDOT = 22,                    /* DOTDOT  */
  YYSYMBOL_PIE = 23,                       /* PIE  */
  YYSYMBOL_BEGINCLOSEDFUNC = 24,           /* BEGINCLOSEDFUNC  */
  YYSYMBOL_BADTOKEN = 25,                  /* BADTOKEN  */
  YYSYMBOL_INTERPRET = 26,                 /* INTERPRET  */
  YYSYMBOL_BEGINGENERATOR = 27,            /* BEGINGENERATOR  */
  YYSYMBOL_LEFTARROW = 28,                 /* LEFTARROW  */
  YYSYMBOL_WHILE = 29,                     /* WHILE  */
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
  YYSYMBOL_optname = 61,                   /* optname  */
  YYSYMBOL_superclass = 62,                /* superclass  */
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
  YYSYMBOL_primitive = 73,                 /* primitive  */
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
  YYSYMBOL_mavarlist = 103,                /* mavarlist  */
  YYSYMBOL_slotliteral = 104,              /* slotliteral  */
  YYSYMBOL_blockliteral = 105,             /* blockliteral  */
  YYSYMBOL_pushname = 106,                 /* pushname  */
  YYSYMBOL_pushliteral = 107,              /* pushliteral  */
  YYSYMBOL_listliteral = 108,              /* listliteral  */
  YYSYMBOL_block = 109,                    /* block  */
  YYSYMBOL_funcvardecls = 110,             /* funcvardecls  */
  YYSYMBOL_funcvardecls1 = 111,            /* funcvardecls1  */
  YYSYMBOL_funcvardecl = 112,              /* funcvardecl  */
  YYSYMBOL_argdecls = 113,                 /* argdecls  */
  YYSYMBOL_argdecls1 = 114,                /* argdecls1  */
  YYSYMBOL_constdeflist = 115,             /* constdeflist  */
  YYSYMBOL_constdef = 116,                 /* constdef  */
  YYSYMBOL_slotdeflist0 = 117,             /* slotdeflist0  */
  YYSYMBOL_slotdeflist = 118,              /* slotdeflist  */
  YYSYMBOL_slotdef = 119,                  /* slotdef  */
  YYSYMBOL_vardeflist0 = 120,              /* vardeflist0  */
  YYSYMBOL_vardeflist = 121,               /* vardeflist  */
  YYSYMBOL_vardef = 122,                   /* vardef  */
  YYSYMBOL_dictslotdef = 123,              /* dictslotdef  */
  YYSYMBOL_dictslotlist1 = 124,            /* dictslotlist1  */
  YYSYMBOL_dictslotlist = 125,             /* dictslotlist  */
  YYSYMBOL_rwslotdeflist = 126,            /* rwslotdeflist  */
  YYSYMBOL_rwslotdef = 127,                /* rwslotdef  */
  YYSYMBOL_listlit = 128,                  /* listlit  */
  YYSYMBOL_listlit2 = 129,                 /* listlit2  */
  YYSYMBOL_literallistc = 130,             /* literallistc  */
  YYSYMBOL_literallist1 = 131,             /* literallist1  */
  YYSYMBOL_rwspec = 132,                   /* rwspec  */
  YYSYMBOL_rspec = 133,                    /* rspec  */
  YYSYMBOL_integer = 134,                  /* integer  */
  YYSYMBOL_floatr = 135,                   /* floatr  */
  YYSYMBOL_accidental = 136,               /* accidental  */
  YYSYMBOL_pie = 137,                      /* pie  */
  YYSYMBOL_floatp = 138,                   /* floatp  */
  YYSYMBOL_name = 139,                     /* name  */
  YYSYMBOL_classname = 140,                /* classname  */
  YYSYMBOL_primname = 141,                 /* primname  */
  YYSYMBOL_trueobj = 142,                  /* trueobj  */
  YYSYMBOL_falseobj = 143,                 /* falseobj  */
  YYSYMBOL_nilobj = 144,                   /* nilobj  */
  YYSYMBOL_ascii = 145,                    /* ascii  */
  YYSYMBOL_symbol = 146,                   /* symbol  */
  YYSYMBOL_string = 147,                   /* string  */
  YYSYMBOL_pseudovar = 148,                /* pseudovar  */
  YYSYMBOL_binop = 149,                    /* binop  */
  YYSYMBOL_keybinop = 150,                 /* keybinop  */
  YYSYMBOL_binop2 = 151,                   /* binop2  */
  YYSYMBOL_curryarg = 152                  /* curryarg  */
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
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
#define YYLAST   1828

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  98
/* YYNRULES -- Number of rules.  */
#define YYNRULES  294
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  551

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
       0,    39,    39,    41,    43,    47,    48,    52,    53,    57,
      61,    68,    74,    75,    78,    79,    83,    84,    88,    90,
      92,    96,    97,   101,   104,   107,   110,   115,   116,   119,
     120,   123,   124,   127,   128,   132,   134,   136,   138,   140,
     142,   144,   148,   149,   153,   154,   159,   160,   165,   166,
     170,   171,   177,   178,   181,   182,   185,   189,   193,   197,
     202,   206,   211,   229,   242,   244,   255,   266,   277,   290,
     311,   320,   329,   334,   348,   370,   374,   380,   398,   404,
     404,   414,   414,   421,   442,   446,   480,   518,   532,   543,
     547,   572,   573,   574,   575,   576,   577,   578,   584,   594,
     596,   598,   600,   602,   604,   617,   620,   647,   665,   692,
     720,   739,   767,   794,   812,   837,   865,   884,   912,   931,
     950,   967,   981,  1002,  1021,  1039,  1056,  1072,  1088,  1089,
    1090,  1091,  1092,  1105,  1119,  1124,  1128,  1139,  1144,  1154,
    1159,  1173,  1189,  1190,  1191,  1192,  1195,  1196,  1202,  1205,
    1206,  1210,  1211,  1213,  1218,  1220,  1227,  1235,  1236,  1240,
    1242,  1246,  1247,  1251,  1255,  1256,  1259,  1261,  1265,  1266,
    1271,  1272,  1273,  1274,  1275,  1276,  1277,  1278,  1279,  1282,
    1285,  1288,  1289,  1290,  1291,  1292,  1293,  1294,  1295,  1296,
    1299,  1300,  1301,  1302,  1303,  1304,  1305,  1306,  1307,  1308,
    1311,  1314,  1319,  1320,  1324,  1325,  1329,  1333,  1334,  1338,
    1342,  1346,  1350,  1356,  1360,  1364,  1368,  1372,  1379,  1380,
    1384,  1388,  1389,  1392,  1393,  1397,  1399,  1401,  1409,  1410,
    1413,  1414,  1418,  1420,  1422,  1430,  1432,  1439,  1440,  1444,
    1445,  1448,  1449,  1453,  1455,  1479,  1481,  1485,  1487,  1491,
    1492,  1495,  1496,  1500,  1501,  1503,  1505,  1509,  1510,  1514,
    1515,  1524,  1525,  1534,  1535,  1546,  1549,  1550,  1551,  1557,
    1565,  1572,  1581,  1582,  1585,  1588,  1591,  1594,  1597,  1600,
    1603,  1606,  1609,  1612,  1613,  1614,  1615,  1616,  1617,  1618,
    1619,  1622,  1625,  1626,  1629
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
  "SC_FLOAT", "ACCIDENTAL", "SYMBOL", "STRING", "ASCII", "PRIMITIVENAME",
  "CLASSNAME", "CURRYARG", "VAR", "ARG", "CLASSVAR", "SC_CONST", "NILOBJ",
  "TRUEOBJ", "FALSEOBJ", "PSEUDOVAR", "ELLIPSIS", "DOTDOT", "PIE",
  "BEGINCLOSEDFUNC", "BADTOKEN", "INTERPRET", "BEGINGENERATOR",
  "LEFTARROW", "WHILE", "':'", "'='", "BINOP", "KEYBINOP", "'-'", "'<'",
  "'>'", "'*'", "'+'", "'|'", "READWRITEVAR", "'.'", "'`'", "UMINUS",
  "'{'", "'}'", "'['", "']'", "';'", "','", "'('", "')'", "'^'", "'~'",
  "'#'", "$accept", "root", "classes", "classextensions", "classdef",
  "classextension", "optname", "superclass", "classvardecls",
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

#define YYPACT_NINF (-400)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-291)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     167,   852,    38,    76,    38,     5,  -400,  -400,  -400,  -400,
    -400,  -400,  -400,  -400,  -400,  -400,   147,   147,  -400,  -400,
    -400,  -400,  -400,    43,  -400,   295,   147,  1684,    56,  1320,
     748,  1684,   147,   179,  -400,  -400,  -400,  -400,  -400,    51,
    -400,  -400,  -400,  1787,    41,    85,  -400,  -400,  -400,  -400,
    1060,  -400,  1060,  -400,   121,   121,  -400,  -400,  -400,   331,
     236,  -400,  -400,  -400,  -400,  -400,  -400,  -400,  -400,   102,
    -400,  -400,     9,  -400,   201,  -400,    23,   132,   210,   147,
     147,  -400,  -400,  -400,  -400,  -400,   146,    -3,  -400,    59,
     800,  -400,  1684,  1684,  -400,  -400,   139,   150,   137,  1684,
    1684,  1372,  -400,   295,  -400,  -400,  -400,  -400,    32,  -400,
     143,   170,  1060,  1060,  -400,   153,   159,  -400,  1684,   165,
    1770,   199,  1736,   231,     3,  -400,   186,  1424,  -400,  -400,
      35,  -400,   225,  1684,  -400,  -400,  -400,  -400,  -400,  1060,
    -400,  -400,  1684,  1112,   115,  -400,  -400,  -400,  1320,   904,
     115,  -400,    38,   147,   237,  -400,   147,  1684,  1684,   147,
    -400,   258,   254,   263,    45,  1060,   147,  -400,  -400,   147,
    -400,   624,  -400,  -400,  1060,  1684,  -400,  1320,  -400,  -400,
    -400,  1684,   238,    19,  -400,  1684,  1684,  1684,  -400,   240,
     242,  1060,  1320,  -400,  -400,  -400,    21,  -400,  -400,  1684,
    1736,  -400,  -400,   249,   275,   121,  -400,  -400,   241,  -400,
    -400,  -400,  -400,  -400,  -400,  1684,   147,   147,  1736,  1684,
    -400,    94,  1476,   956,   439,   129,  1684,  1787,  -400,  1787,
    1684,   115,   280,   284,  -400,   291,   115,   280,   284,   292,
    -400,  1684,   498,  -400,   297,  -400,  -400,  -400,  1787,   296,
     301,   147,  -400,   147,  -400,   306,  -400,   184,  -400,  1684,
      14,  -400,  -400,   121,  -400,  -400,  -400,  -400,  -400,  -400,
    -400,   309,   310,   316,  -400,   334,  1684,  -400,  -400,  1684,
    1684,  -400,  -400,   346,  -400,  -400,   320,   342,  -400,  1684,
    1164,   115,  1787,   326,  -400,  1736,  -400,  1736,  1787,  -400,
    -400,   327,   337,  1528,   354,  1684,  1684,   113,   115,   280,
     284,   292,  1684,  1008,   115,  -400,   384,  1684,  -400,  -400,
     348,  -400,   115,  1216,  -400,   344,   368,   352,  -400,  -400,
     356,   362,   368,   364,  -400,  1761,  -400,  -400,   378,   381,
     399,   294,  -400,  -400,   386,   206,  -400,  -400,   147,   385,
    1268,  1268,  -400,  1684,  -400,  -400,   416,  1684,  -400,   115,
     280,   284,  -400,  -400,   392,  -400,   410,   412,   397,  1684,
    -400,   398,  1580,   424,  -400,   406,   407,   408,  1787,   115,
     280,   284,   292,   420,  1684,   292,    98,  -400,   115,  -400,
    -400,   115,   417,   430,    43,    43,   431,   234,   234,   445,
    -400,   565,  -400,  -400,   147,   452,  -400,   147,   282,   442,
     443,   244,   451,  -400,  1684,  -400,   115,   454,   455,  -400,
    1684,  1684,   462,  1787,   468,   472,   460,  1684,   115,  -400,
     115,  -400,   458,   461,   463,  -400,  -400,  -400,  1684,  -400,
    -400,  -400,    43,    43,  -400,  -400,  -400,  -400,  -400,  -400,
     271,  -400,   147,   273,  -400,   308,  -400,   147,  -400,   476,
    -400,   485,  1684,  1684,  -400,  1268,  -400,  1684,   489,  -400,
    -400,   115,  -400,  1787,  1787,  1684,  1684,  1684,   488,  1787,
    -400,  -400,   115,  -400,   115,  1787,  -400,  -400,   214,   214,
     294,  -400,   234,   490,  -400,  -400,   445,   494,  -400,  1684,
     443,   443,  -400,   443,  1684,  -400,  1787,  1787,  1787,  1684,
    -400,  -400,   214,   214,  -400,  1632,   478,  1632,  1749,  -400,
     709,  -400,   709,   443,  -400,  -400,  -400,   443,  1787,  1632,
    1632,  1684,   475,  -400,   477,  -400,   486,  -400,  -400,  -400,
    -400,  -400,   495,   496,  1770,  -400,  -400,  -400,  -400,  -400,
    -400
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       5,    48,     0,     0,     2,     3,     7,   272,   259,   261,
     263,   280,   281,   279,   274,   294,     0,   228,   278,   276,
     277,   282,   265,   207,   273,     0,   221,     0,   207,   149,
     239,     0,     0,     0,    41,     4,    33,    96,    93,   128,
     105,   130,   129,   146,    27,    48,    92,    94,    91,   179,
      48,   204,    48,   189,   181,   266,   267,   270,   182,   180,
     131,   186,   187,   188,   183,   185,   184,   103,    95,     0,
       1,     6,    14,     8,     0,   230,   232,     0,   229,   228,
     221,   202,   260,   262,   264,   271,     0,    29,   223,    31,
     239,   133,     0,     0,   202,   291,   151,     0,    29,     0,
       0,     0,   283,   287,   285,   286,   288,   289,   221,   284,
       0,     0,    48,    48,   237,    29,     0,   292,   293,     0,
      27,    98,   249,     0,   166,   168,     0,     0,   287,   290,
       0,   293,   142,    28,   148,    34,    40,   205,    39,    48,
     269,   268,     0,     0,    56,    50,    53,    52,   149,     0,
      65,    21,     0,    12,     0,   206,     0,     0,     0,     0,
     213,     0,   229,     0,    29,    48,     0,   215,    30,     0,
      32,     0,    79,    81,    48,     0,    99,    30,   150,   153,
     119,     0,     0,     0,   100,   118,     0,     0,    97,     0,
       0,    48,    30,   240,   102,   236,     0,    28,    49,     0,
     249,   251,   199,     0,    29,   190,   191,   195,     0,   196,
     197,   198,   192,   194,   193,     0,     0,     0,   249,     0,
     157,     0,     0,     0,    54,     0,     0,   147,    38,   135,
       0,     0,    29,    29,    51,     0,    54,    29,    29,    29,
     161,     0,     0,    15,     0,    13,    16,   231,   233,     0,
       0,     0,   208,     0,   210,     0,   203,     0,   224,     0,
       0,   226,   178,   170,   171,   175,   176,   177,   172,   174,
     173,     0,     0,     0,   152,   154,     0,   123,   101,   124,
       0,   120,   235,     0,    37,    36,     0,     0,   238,     0,
       0,    57,   136,     0,   245,    30,   250,   249,   139,   167,
     169,     0,     0,     0,   104,     0,     0,     0,    54,    29,
      29,    29,     0,     0,    55,    78,     0,     0,   144,   143,
     134,   159,    58,    30,   164,     0,    30,     0,    64,    66,
       0,     0,    30,     0,   163,   288,    11,    22,     0,     0,
      14,    21,   234,   214,     0,     0,   201,   216,     0,     0,
       0,     0,   200,     0,   155,   125,     0,   122,    35,     0,
      29,    29,   247,   252,     0,   246,   107,   106,     0,     0,
     158,     0,     0,   132,    70,     0,     0,     0,   137,    54,
      29,    29,    29,     0,     0,    29,    54,    62,    54,    69,
     162,    54,     0,     0,   207,   207,     0,   253,   253,   257,
      17,     0,   209,   211,     0,     0,   227,     0,     0,     0,
      83,   180,     0,   156,   126,   121,    60,     0,     0,   248,
       0,     0,   108,   140,   113,   112,     0,     0,    54,    74,
      54,    75,     0,     0,     0,   145,   160,   165,     0,    59,
      68,    67,   207,   207,   202,   202,    16,   254,   256,   255,
       0,   241,     0,     0,   258,    29,   218,     0,     9,     0,
     217,     0,     0,     0,    80,     0,    88,     0,     0,    82,
     127,    54,    63,   110,   109,     0,     0,     0,   114,   141,
      73,    71,    54,    77,    54,   138,   202,   202,    44,    44,
      21,    19,   253,   243,    18,    20,   257,     0,   212,     0,
      83,    83,    84,    83,     0,    61,   111,   116,   115,     0,
      76,    72,    44,    44,   275,    46,    27,    46,     0,   242,
       0,   219,     0,    83,    90,    89,    85,    83,   117,    46,
      46,     0,     0,    42,    46,    45,     0,    10,   244,   220,
      87,    86,     0,     0,    27,    23,    43,    25,    24,    26,
      47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -400,  -400,  -400,  -400,  -400,   534,  -400,   202,   100,  -400,
    -335,  -400,  -118,   -68,  -400,   448,  -400,  -253,  -306,    13,
     505,   -28,  -100,   -52,  -400,   -51,  -400,  -400,  -262,  -341,
    -400,  -400,  -400,  -400,  -400,  -400,   -27,  -400,  -400,   205,
     403,  -400,  -109,  -112,  -136,   221,   235,  -400,  -400,  -399,
     266,  -400,  -400,   259,  -400,   -78,    -4,   436,   -25,   529,
    -400,    67,   484,   491,   396,   487,   -12,   413,   380,  -400,
    -400,   172,    81,  -170,  -400,  -185,  -400,  -400,  -400,   -99,
    -400,  -400,     4,  -108,    -5,    18,  -400,   -80,   -69,   -66,
     -53,   -44,   -43,  -400,  -235,   375,   -13,  -400
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     3,     4,     5,    71,     6,   244,   154,   341,   400,
     242,   337,   134,   324,   171,    34,    35,   532,   515,   533,
      36,   314,   145,   315,    37,    38,   271,   272,   466,   409,
      39,    40,    41,    42,   110,   182,    43,   226,    44,    45,
      97,    98,   221,   233,   385,   240,   325,   123,   124,   261,
      46,    47,    48,   201,    49,   165,    50,   256,    81,    52,
     455,   456,    86,    87,    88,    77,    74,    75,   114,   115,
     116,   450,   451,    53,   202,   203,   204,   452,   457,    54,
      55,    56,    57,    58,    59,    60,   516,    61,    62,    63,
      64,    65,    66,    67,   117,   131,   132,    68
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      91,   262,   198,    94,   120,    78,   401,   339,   146,   146,
     412,    76,    76,   239,   206,   293,   174,   119,  -222,   169,
      69,    89,    72,   205,   216,    14,   112,   121,   125,    85,
     178,   144,   150,   301,   232,     7,   167,   238,     7,   152,
     237,   279,   209,     2,   234,    23,   168,   193,   139,    14,
     234,   126,   217,   210,   157,   153,   211,    79,   140,   141,
     122,    24,  -225,   264,    24,    28,  -222,   162,   280,   212,
      79,   290,   263,   158,    76,    89,    70,   119,   213,   214,
    -225,   222,    80,  -290,   254,   223,    92,   311,  -225,   133,
     170,   265,   206,   146,   168,    80,   169,   127,  -225,   146,
     393,   205,   266,    89,    93,   267,   227,    85,  -225,   191,
     206,   310,   364,   307,   309,   229,   303,   207,   268,   205,
     209,   538,    23,   539,   502,   224,   318,   269,   270,   438,
     248,   210,     7,     8,   211,   372,   296,    31,   209,    23,
     208,   304,    28,   305,    22,   146,   151,   212,   245,   210,
       7,    76,   211,   159,   250,   518,   213,   214,    24,    28,
     373,   257,   305,   316,    89,   212,   339,   166,   291,   175,
     243,   333,   292,   146,   213,   214,    24,   382,   361,   317,
     146,   360,     7,   517,   329,   146,   177,   206,   298,   206,
      14,   234,   185,     1,   184,   207,   205,   176,   205,   320,
     186,   381,   192,   322,   380,     2,   529,   530,    24,   140,
     194,   299,   300,   207,   234,   209,   196,   209,   208,   187,
     319,   188,   234,   347,   514,   122,   210,    16,   210,   211,
     199,   211,   218,   348,    96,   111,   208,   338,   524,   525,
     146,   526,   212,   377,   212,   403,   344,     7,   345,   155,
     156,   213,   214,   213,   214,   404,   374,   146,   160,   156,
      23,   540,   215,   146,   536,   541,   225,   140,    23,   447,
     448,   146,   467,    24,   449,   142,   542,   543,   126,   251,
      28,   246,   148,   339,   253,   378,   149,   297,    28,   278,
     207,   284,   207,   285,   143,   111,   294,   172,   173,    82,
      83,    84,   252,   156,   179,   180,   183,   397,   146,   398,
     399,   462,   463,   208,   434,   208,   234,   437,    22,   491,
     492,   494,   492,   195,   295,   147,   147,   431,   146,   323,
     392,   416,   220,   326,   439,   146,   440,   146,   328,   441,
     146,   332,   423,   405,   340,   411,   411,   342,   220,   343,
     262,   346,   262,    96,   220,    23,   495,   168,   350,   351,
     146,   352,   142,   249,   353,   146,   488,   489,   357,   444,
     445,   358,   186,   362,   365,    28,   480,   146,   481,   146,
     274,   143,   275,   144,   366,   369,   277,   496,    82,   130,
     281,   282,   283,   473,   474,   386,   338,   287,   535,   459,
     479,    95,   461,   387,    99,   118,   468,   388,   512,   513,
     147,   485,   264,   389,   264,   391,   147,   486,   487,   505,
     146,   263,   394,   263,   302,   395,   550,   220,   220,   152,
     510,   146,   511,   146,   402,   321,   406,    51,   414,   419,
     265,   420,   265,   421,   422,   424,   334,   493,   506,   507,
     508,   266,   497,   266,   267,   427,   267,   428,   429,   430,
     411,   442,   147,    23,   349,   118,    51,   268,   327,   268,
     312,   435,   330,   331,   443,   446,   269,   270,   269,   270,
     454,   354,   528,    28,   355,   356,   137,   464,    51,   313,
     147,   460,   465,   475,   195,   220,   469,   147,   136,   476,
     138,     7,   147,   477,   544,   471,   472,   478,   368,   482,
     370,   371,   483,   338,   484,   498,   499,   504,   220,   509,
     545,   520,   383,    99,   241,   522,   197,    24,   370,   531,
     102,   547,   128,   104,   105,   335,   107,   129,   109,    73,
     548,   549,   396,   336,   375,   376,   490,   546,   137,    51,
     135,   235,   276,   390,   363,   410,   410,   147,   413,   113,
     189,   190,   415,   521,   163,   258,   161,   289,     7,   247,
     453,   164,   288,   519,   147,   137,     0,   426,     0,     0,
     147,     0,     0,     0,     0,     0,     0,   228,   147,   436,
       0,     0,     0,     0,    24,   417,   418,   102,   241,   128,
     104,   105,   335,   107,   129,   109,     0,     0,     0,     0,
     458,     0,     0,   255,     0,   432,   433,     0,     0,   470,
       0,     0,   273,     0,     0,   147,     0,   137,     8,     9,
      10,    11,    12,    13,     0,     0,     0,     0,     0,   286,
       0,    18,    19,    20,     0,   147,     0,    22,     0,     0,
       0,     0,   147,     0,   147,     0,     0,   147,    25,     0,
       0,     0,     0,     0,     0,     0,     0,   500,   501,     0,
     410,     0,   503,     0,   259,     0,     0,   147,   260,     0,
       0,     0,   147,     0,     0,     0,     0,     0,   241,     0,
       0,     0,     0,     0,   147,     0,   147,     0,   241,     0,
       0,   241,     0,     0,   523,     0,     0,   241,     0,   527,
       0,     0,     0,     8,     9,    10,    11,    12,    13,     0,
     534,     0,   534,     0,     0,     0,    18,    19,    20,     0,
       0,     0,    22,     0,   534,   534,     0,   147,     0,     0,
       0,     0,     0,    25,     0,     0,     0,     0,   147,     0,
     147,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,    16,    17,   260,     0,    18,    19,    20,    21,     0,
     100,    22,    23,     0,     0,     0,     0,    24,   101,     0,
     102,    95,   103,   104,   105,   106,   107,   108,   109,     0,
      27,     0,    28,     0,    29,     0,     0,     0,    90,     0,
       0,    32,    33,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,     0,     0,     0,     0,    18,    19,    20,
      21,     0,   100,    22,    23,     0,     0,     0,     0,    24,
     101,     0,   102,    95,   103,   104,   105,   106,   107,   129,
     109,     0,    27,     0,    28,     0,    29,     0,     0,     0,
      90,     0,     0,    32,    33,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,    16,    17,     0,     0,    18,
      19,    20,    21,     0,     0,    22,    23,     0,     0,     0,
       0,    24,     0,     0,     0,     0,    25,     0,     0,     0,
       0,    26,     0,     0,    27,     0,    28,     0,    29,     0,
       0,     0,    30,     0,    31,    32,    33,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,     0,     0,     0,
       0,    18,    19,    20,    21,     0,     0,    22,    23,     0,
       0,     0,     0,    24,     0,     0,     0,    95,    25,     0,
       0,   230,     0,     0,     0,     0,    27,     0,    28,     0,
      29,     0,     0,     0,    90,   236,     0,    32,    33,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,     0,
       0,     0,     0,    18,    19,    20,    21,     0,     0,    22,
      23,     0,     0,     0,     0,    24,     0,     0,     0,    95,
      25,     0,     0,   230,     0,     0,     0,     0,    27,     0,
      28,     0,    29,     0,     0,     0,    90,   308,     0,    32,
      33,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,     0,     0,     0,     0,    18,    19,    20,    21,     0,
       0,    22,    23,     0,     0,     0,     0,    24,     0,     0,
       0,    95,    25,     0,     0,   230,     0,     0,     0,     0,
      27,     0,    28,     0,    29,     0,     0,     0,    90,   379,
       0,    32,    33,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,    16,     0,     0,     0,    18,    19,    20,
      21,     0,     0,    22,    23,     0,     0,     0,     0,    24,
       0,     0,     0,     0,    25,     0,     0,     0,     0,     0,
       0,     0,    27,     0,    28,     0,    29,     0,     0,     0,
      90,     0,    31,    32,    33,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,     0,     0,     0,     0,    18,
      19,    20,    21,     0,     0,    22,    23,     0,     0,     0,
       0,    24,     0,     0,     0,     0,    25,     0,     0,   230,
       0,     0,     0,     0,    27,     0,    28,     0,    29,     0,
       0,     0,    90,   231,     0,    32,    33,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,     0,     0,     0,
       0,    18,    19,    20,    21,     0,     0,    22,    23,     0,
       0,     0,     0,    24,     0,     0,     0,     0,    25,     0,
       0,   230,     0,     0,     0,     0,    27,     0,    28,     0,
      29,     0,     0,     0,    90,   359,     0,    32,    33,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,     0,
       0,     0,     0,    18,    19,    20,    21,     0,     0,    22,
      23,     0,     0,     0,     0,    24,     0,     0,     0,    95,
      25,     0,     0,   384,     0,     0,     0,     0,    27,     0,
      28,     0,    29,     0,     0,     0,    90,     0,     0,    32,
      33,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,   407,     0,     0,     0,    18,    19,    20,    21,     0,
       0,    22,    23,     0,     0,     0,     0,    24,   408,     0,
       0,     0,    25,     0,     0,     0,     0,     0,     0,     0,
      27,     0,    28,     0,    29,     0,     0,     0,    90,     0,
       0,    32,    33,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,     0,     0,     0,     0,    18,    19,    20,
      21,     0,     0,    22,    23,     0,     0,     0,     0,    24,
       0,     0,     0,    95,    25,     0,     0,     0,     0,     0,
       0,     0,    27,     0,    28,     0,    29,     0,     0,     0,
      90,     0,     0,    32,    33,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,     0,     0,     0,     0,    18,
      19,    20,    21,     0,   181,    22,    23,     0,     0,     0,
       0,    24,     0,     0,     0,     0,    25,     0,     0,     0,
       0,     0,     0,     0,    27,     0,    28,     0,    29,     0,
       0,     0,    90,     0,     0,    32,    33,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,     0,     0,     0,
       0,    18,    19,    20,    21,     0,   219,    22,    23,     0,
       0,     0,     0,    24,     0,     0,     0,     0,    25,     0,
       0,     0,     0,     0,     0,     0,    27,     0,    28,     0,
      29,     0,     0,     0,    90,     0,     0,    32,    33,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,     0,
       0,     0,     0,    18,    19,    20,    21,     0,   306,    22,
      23,     0,     0,     0,     0,    24,     0,     0,     0,     0,
      25,     0,     0,     0,     0,     0,     0,     0,    27,     0,
      28,     0,    29,     0,     0,     0,    90,     0,     0,    32,
      33,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,     0,     0,     0,     0,    18,    19,    20,    21,     0,
       0,    22,    23,     0,     0,     0,     0,    24,     0,     0,
       0,     0,    25,     0,     0,     0,     0,     0,     0,     0,
      27,     0,    28,     0,    29,   367,     0,     0,    90,     0,
       0,    32,    33,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,     0,     0,     0,     0,    18,    19,    20,
      21,     0,     0,    22,    23,     0,     0,     0,     0,    24,
       0,     0,     0,     0,    25,     0,     0,     0,     0,     0,
       0,     0,    27,     0,    28,     0,    29,   425,     0,     0,
      90,     0,     0,    32,    33,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,     0,     0,     0,     0,    18,
      19,    20,    21,     0,     0,    22,    23,     0,     0,     0,
       0,    24,     0,     0,     0,     0,    25,     0,     0,     0,
       0,     0,     0,     0,    27,     0,    28,     0,    29,     0,
       0,     0,    90,     0,   531,    32,    33,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,     0,     0,     0,
       0,    18,    19,    20,    21,     0,     0,    22,    23,     0,
       0,     0,     0,    24,     0,     0,     0,     0,    25,     0,
       0,     0,     0,     0,     0,     0,    27,     0,    28,     0,
      29,     0,     0,     0,    90,     0,     0,    32,    33,     7,
       8,     9,    10,    11,    12,    13,     0,    14,     0,     0,
       0,     0,     7,    18,    19,    20,     0,     0,     0,    22,
       0,     0,     0,     0,     7,    24,     0,     0,     0,     0,
      25,     0,     0,     0,     0,     0,     0,     0,    24,     0,
       0,   102,   200,   128,   104,   105,   335,   107,   129,   109,
      24,     0,     0,   102,   537,   128,   104,   105,   106,   107,
     129,   109,   102,    95,   128,   104,   105,   106,   107,   129,
     109,   130,     0,     0,     0,     0,     0,     0,   197,   102,
      95,   128,   104,   105,   106,   107,   129,   109,   130
};

static const yytype_int16 yycheck[] =
{
      27,   171,   120,    28,    31,    17,   341,   242,    59,    60,
     351,    16,    17,   149,   122,   200,    94,    30,    21,    87,
       2,    26,     4,   122,    21,    11,    30,    32,    33,    25,
      98,    59,    60,   218,   143,     3,    39,   149,     3,    30,
     149,    22,   122,    38,   144,    24,    49,   115,    52,    11,
     150,    33,    49,   122,    31,    46,   122,    14,    54,    55,
      46,    29,     3,   171,    29,    44,    21,    79,    49,   122,
      14,    50,   171,    50,    79,    80,     0,    90,   122,   122,
      21,    46,    39,    51,    39,    50,    30,   223,    29,    48,
      31,   171,   200,   144,    49,    39,   164,    46,    39,   150,
     335,   200,   171,   108,    48,   171,   133,   103,    49,   113,
     218,   223,   297,   222,   223,   142,    22,   122,   171,   218,
     200,   520,    24,   522,   465,   130,   225,   171,   171,    31,
     157,   200,     3,     4,   200,    22,   204,    52,   218,    24,
     122,    47,    44,    49,    23,   196,    44,   200,   153,   218,
       3,   156,   218,    21,   159,   490,   200,   200,    29,    44,
      47,   166,    49,    34,   169,   218,   401,    21,   196,    30,
     152,   239,   199,   224,   218,   218,    29,   313,   290,    50,
     231,   290,     3,   489,   236,   236,    49,   295,   215,   297,
      11,   291,    22,    26,    51,   200,   295,    47,   297,   226,
      30,   313,    49,   231,   313,    38,   512,   513,    29,   205,
      51,   216,   217,   218,   314,   295,    51,   297,   200,    49,
     225,    51,   322,    39,    10,    46,   295,    13,   297,   295,
      31,   297,    46,    49,    29,    30,   218,   242,   500,   501,
     291,   503,   295,   311,   297,    39,   251,     3,   253,    48,
      49,   295,   295,   297,   297,    49,   308,   308,    48,    49,
      24,   523,    31,   314,   517,   527,    41,   263,    24,    35,
      36,   322,    28,    29,    40,    31,   529,   530,   260,    21,
      44,    44,    46,   518,    21,   312,    50,    46,    44,    51,
     295,    51,   297,    51,    50,    90,    47,    92,    93,     4,
       5,     6,    48,    49,    99,   100,   101,    13,   359,    15,
      16,    29,    30,   295,   382,   297,   416,   385,    23,    48,
      49,    48,    49,   118,    49,    59,    60,   379,   379,    49,
     335,   359,   127,    49,   386,   386,   388,   388,    47,   391,
     391,    49,   369,   348,    47,   350,   351,    51,   143,    48,
     520,    45,   522,   148,   149,    24,    48,    49,    49,    49,
     411,    45,    31,   158,    30,   416,   444,   445,    22,   394,
     395,    51,    30,    47,    47,    44,   428,   428,   430,   430,
     175,    50,   177,   411,    47,    31,   181,   455,     4,    41,
     185,   186,   187,   420,   421,    51,   401,   192,   516,   404,
     427,    33,   407,    51,    29,    30,   411,    51,   486,   487,
     144,   438,   520,    51,   522,    51,   150,   442,   443,   471,
     471,   520,    44,   522,   219,    44,   544,   222,   223,    30,
     482,   482,   484,   484,    48,   230,    51,     1,    22,    47,
     520,    31,   522,    31,    47,    47,   241,   452,   475,   476,
     477,   520,   457,   522,   520,    31,   522,    51,    51,    51,
     465,    44,   196,    24,   259,    90,    30,   520,   233,   522,
      31,    51,   237,   238,    44,    44,   520,   520,   522,   522,
      35,   276,   509,    44,   279,   280,    50,    45,    52,    50,
     224,    39,    49,    31,   289,   290,    45,   231,    50,    31,
      52,     3,   236,    31,   531,    51,    51,    47,   303,    51,
     305,   306,    51,   518,    51,    39,    31,    28,   313,    31,
      45,    31,   317,   148,   149,    31,    48,    29,   323,    52,
      32,    45,    34,    35,    36,    37,    38,    39,    40,     5,
      45,    45,   340,    45,   309,   310,   446,   534,   112,   113,
      45,   148,   177,   332,   295,   350,   351,   291,   353,    30,
     112,   113,   357,   496,    80,   169,    79,   192,     3,   156,
     398,    80,   192,   492,   308,   139,    -1,   372,    -1,    -1,
     314,    -1,    -1,    -1,    -1,    -1,    -1,   139,   322,   384,
      -1,    -1,    -1,    -1,    29,   360,   361,    32,   223,    34,
      35,    36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,
      45,    -1,    -1,   165,    -1,   380,   381,    -1,    -1,   414,
      -1,    -1,   174,    -1,    -1,   359,    -1,   191,     4,     5,
       6,     7,     8,     9,    -1,    -1,    -1,    -1,    -1,   191,
      -1,    17,    18,    19,    -1,   379,    -1,    23,    -1,    -1,
      -1,    -1,   386,    -1,   388,    -1,    -1,   391,    34,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   462,   463,    -1,
     465,    -1,   467,    -1,    50,    -1,    -1,   411,    54,    -1,
      -1,    -1,   416,    -1,    -1,    -1,    -1,    -1,   313,    -1,
      -1,    -1,    -1,    -1,   428,    -1,   430,    -1,   323,    -1,
      -1,   326,    -1,    -1,   499,    -1,    -1,   332,    -1,   504,
      -1,    -1,    -1,     4,     5,     6,     7,     8,     9,    -1,
     515,    -1,   517,    -1,    -1,    -1,    17,    18,    19,    -1,
      -1,    -1,    23,    -1,   529,   530,    -1,   471,    -1,    -1,
      -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,   482,    -1,
     484,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      12,    13,    14,    54,    -1,    17,    18,    19,    20,    -1,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    -1,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    -1,
      42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,
      -1,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    -1,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,
      50,    -1,    -1,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    13,    14,    -1,    -1,    17,
      18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,
      -1,    29,    -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,
      -1,    39,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    -1,    52,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    -1,    33,    34,    -1,
      -1,    37,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    -1,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,
      24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,    33,
      34,    -1,    -1,    37,    -1,    -1,    -1,    -1,    42,    -1,
      44,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,    53,
      54,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,
      -1,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,
      -1,    33,    34,    -1,    -1,    37,    -1,    -1,    -1,    -1,
      42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    51,
      -1,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    13,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,
      -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,
      50,    -1,    52,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,
      18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,
      -1,    29,    -1,    -1,    -1,    -1,    34,    -1,    -1,    37,
      -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    51,    -1,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    34,    -1,
      -1,    37,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    -1,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,
      24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,    33,
      34,    -1,    -1,    37,    -1,    -1,    -1,    -1,    42,    -1,
      44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,    53,
      54,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      12,    13,    -1,    -1,    -1,    17,    18,    19,    20,    -1,
      -1,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    -1,
      -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,
      -1,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,
      -1,    -1,    -1,    33,    34,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,
      50,    -1,    -1,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,
      18,    19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    34,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    -1,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,
      34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,
      44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,    53,
      54,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,
      -1,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,
      -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      42,    -1,    44,    -1,    46,    47,    -1,    -1,    50,    -1,
      -1,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,
      -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    46,    47,    -1,    -1,
      50,    -1,    -1,    53,    54,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,
      18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,
      -1,    29,    -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    -1,    52,    53,    54,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    34,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    -1,    -1,
      -1,    -1,     3,    17,    18,    19,    -1,    -1,    -1,    23,
      -1,    -1,    -1,    -1,     3,    29,    -1,    -1,    -1,    -1,
      34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    -1,
      -1,    32,    46,    34,    35,    36,    37,    38,    39,    40,
      29,    -1,    -1,    32,    45,    34,    35,    36,    37,    38,
      39,    40,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    -1,    -1,    -1,    -1,    -1,    -1,    48,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    26,    38,    56,    57,    58,    60,     3,     4,     5,
       6,     7,     8,     9,    11,    12,    13,    14,    17,    18,
      19,    20,    23,    24,    29,    34,    39,    42,    44,    46,
      50,    52,    53,    54,    70,    71,    75,    79,    80,    85,
      86,    87,    88,    91,    93,    94,   105,   106,   107,   109,
     111,   112,   114,   128,   134,   135,   136,   137,   138,   139,
     140,   142,   143,   144,   145,   146,   147,   148,   152,   140,
       0,    59,   140,    60,   121,   122,   139,   120,   121,    14,
      39,   113,     4,     5,     6,   137,   117,   118,   119,   139,
      50,    91,    30,    48,   113,    33,    94,    95,    96,   150,
      22,    30,    32,    34,    35,    36,    37,    38,    39,    40,
      89,    94,   111,   114,   123,   124,   125,   149,   150,   151,
      91,   139,    46,   102,   103,   139,   140,    46,    34,    39,
      41,   150,   151,    48,    67,    75,    70,   112,    70,   111,
     137,   137,    31,    50,    76,    77,    80,   105,    46,    50,
      76,    44,    30,    46,    62,    48,    49,    31,    50,    21,
      48,   120,   121,   117,   118,   110,    21,    39,    49,    68,
      31,    69,    94,    94,   110,    30,    47,    49,    68,    94,
      94,    22,    90,    94,    51,    22,    30,    49,    51,    70,
      70,   111,    49,    68,    51,    94,    51,    48,    67,    31,
      46,   108,   129,   130,   131,   134,   138,   139,   140,   142,
     143,   144,   145,   146,   147,    31,    21,    49,    46,    22,
      94,    97,    46,    50,   139,    41,    92,    91,    70,    91,
      37,    51,    97,    98,    77,    95,    51,    97,    98,    99,
     100,   150,    65,   140,    61,   139,    44,   122,    91,    94,
     139,    21,    48,    21,    39,    70,   112,   139,   119,    50,
      54,   104,   128,   134,   138,   142,   143,   144,   145,   146,
     147,    81,    82,    70,    94,    94,   150,    94,    51,    22,
      49,    94,    94,    94,    51,    51,    70,    94,   123,   150,
      50,    76,    91,   130,    47,    49,    68,    46,    91,   139,
     139,   130,    94,    22,    47,    49,    22,    97,    51,    97,
      98,    99,    31,    50,    76,    78,    34,    50,   134,   139,
      91,    94,    76,    49,    68,   101,    49,   101,    47,    78,
     101,   101,    49,    68,    94,    37,    45,    66,   139,   149,
      47,    63,    51,    48,   139,   139,    45,    39,    49,    94,
      49,    49,    45,    30,    94,    94,    94,    22,    51,    51,
      97,    98,    47,   108,   130,    47,    47,    47,    94,    31,
      94,    94,    22,    47,    78,   101,   101,    68,    91,    51,
      97,    98,    99,    94,    37,    99,    51,    51,    51,    51,
     100,    51,   139,   149,    44,    44,    62,    13,    15,    16,
      64,    65,    48,    39,    49,   139,    51,    13,    30,    84,
      94,   139,    84,    94,    22,    94,    76,   101,   101,    47,
      31,    31,    47,    91,    47,    47,    94,    31,    51,    51,
      51,    78,   101,   101,    68,    51,    94,    68,    31,    78,
      78,    78,    44,    44,   113,   113,    44,    35,    36,    40,
     126,   127,   132,   126,    35,   115,   116,   133,    45,   139,
      39,   139,    29,    30,    45,    49,    83,    28,   139,    45,
      94,    51,    51,    91,    91,    31,    31,    31,    47,    91,
      78,    78,    51,    51,    51,    91,   113,   113,   110,   110,
      63,    48,    49,   139,    48,    48,    68,   139,    39,    31,
      94,    94,    84,    94,    28,    78,    91,    91,    91,    31,
      78,    78,   110,   110,    10,    73,   141,    73,    65,   127,
      31,   116,    31,    94,    83,    83,    83,    94,    91,    73,
      73,    52,    72,    74,    94,    67,    72,    45,   104,   104,
      83,    83,    72,    72,    91,    45,    74,    45,    45,    45,
      67
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
     109,   109,   110,   110,   111,   111,   112,   113,   113,   113,
     113,   113,   113,   114,   114,   114,   114,   114,   115,   115,
     116,   117,   117,   118,   118,   119,   119,   119,   120,   120,
     121,   121,   122,   122,   122,   123,   123,   124,   124,   125,
     125,   126,   126,   127,   127,   128,   128,   129,   129,   130,
     130,   131,   131,   132,   132,   132,   132,   133,   133,   134,
     134,   135,   135,   136,   136,   137,   138,   138,   138,   138,
     138,   138,   139,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   149,   149,   149,   149,   149,   149,
     149,   150,   151,   151,   152
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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
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
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
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
                       &yyvsp[(yyi + 1) - (yynrhs)]);
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
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
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

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

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

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
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
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* root: classes  */
#line 40 "lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; }
#line 2149 "lang11d_tab.cpp"
    break;

  case 3: /* root: classextensions  */
#line 42 "lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; }
#line 2155 "lang11d_tab.cpp"
    break;

  case 4: /* root: INTERPRET cmdlinecode  */
#line 44 "lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 2; }
#line 2161 "lang11d_tab.cpp"
    break;

  case 5: /* classes: %empty  */
#line 47 "lang11d"
          { yyval = 0; }
#line 2167 "lang11d_tab.cpp"
    break;

  case 6: /* classes: classes classdef  */
#line 49 "lang11d"
                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2173 "lang11d_tab.cpp"
    break;

  case 8: /* classextensions: classextensions classextension  */
#line 54 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2179 "lang11d_tab.cpp"
    break;

  case 9: /* classdef: classname superclass '{' classvardecls methods '}'  */
#line 58 "lang11d"
                                { yyval = (intptr_t)newPyrClassNode((PyrSlotNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1], 0);
				}
#line 2187 "lang11d_tab.cpp"
    break;

  case 10: /* classdef: classname '[' optname ']' superclass '{' classvardecls methods '}'  */
#line 62 "lang11d"
                                { yyval = (intptr_t)newPyrClassNode((PyrSlotNode*)yyvsp[-8], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1],
					(PyrSlotNode*)yyvsp[-6]);
				}
#line 2196 "lang11d_tab.cpp"
    break;

  case 11: /* classextension: '+' classname '{' methods '}'  */
#line 69 "lang11d"
                                {
					yyval = (intptr_t)newPyrClassExtNode((PyrSlotNode*)yyvsp[-3], (PyrMethodNode*)yyvsp[-1]);
				}
#line 2204 "lang11d_tab.cpp"
    break;

  case 12: /* optname: %empty  */
#line 74 "lang11d"
                  { yyval = 0; }
#line 2210 "lang11d_tab.cpp"
    break;

  case 14: /* superclass: %empty  */
#line 78 "lang11d"
                  { yyval = 0; }
#line 2216 "lang11d_tab.cpp"
    break;

  case 15: /* superclass: ':' classname  */
#line 80 "lang11d"
                                { yyval = yyvsp[0]; }
#line 2222 "lang11d_tab.cpp"
    break;

  case 16: /* classvardecls: %empty  */
#line 83 "lang11d"
                  { yyval = 0; }
#line 2228 "lang11d_tab.cpp"
    break;

  case 17: /* classvardecls: classvardecls classvardecl  */
#line 85 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2234 "lang11d_tab.cpp"
    break;

  case 18: /* classvardecl: CLASSVAR rwslotdeflist ';'  */
#line 89 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varClass); }
#line 2240 "lang11d_tab.cpp"
    break;

  case 19: /* classvardecl: VAR rwslotdeflist ';'  */
#line 91 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varInst); }
#line 2246 "lang11d_tab.cpp"
    break;

  case 20: /* classvardecl: SC_CONST constdeflist ';'  */
#line 93 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varConst); }
#line 2252 "lang11d_tab.cpp"
    break;

  case 21: /* methods: %empty  */
#line 96 "lang11d"
                  { yyval = 0; }
#line 2258 "lang11d_tab.cpp"
    break;

  case 22: /* methods: methods methoddef  */
#line 98 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2264 "lang11d_tab.cpp"
    break;

  case 23: /* methoddef: name '{' argdecls funcvardecls primitive methbody '}'  */
#line 102 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2271 "lang11d_tab.cpp"
    break;

  case 24: /* methoddef: '*' name '{' argdecls funcvardecls primitive methbody '}'  */
#line 105 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2278 "lang11d_tab.cpp"
    break;

  case 25: /* methoddef: binop '{' argdecls funcvardecls primitive methbody '}'  */
#line 108 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2285 "lang11d_tab.cpp"
    break;

  case 26: /* methoddef: '*' binop '{' argdecls funcvardecls primitive methbody '}'  */
#line 111 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2292 "lang11d_tab.cpp"
    break;

  case 34: /* funcbody: exprseq funretval  */
#line 129 "lang11d"
                                { yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2298 "lang11d_tab.cpp"
    break;

  case 35: /* cmdlinecode: '(' argdecls1 funcvardecls1 funcbody ')'  */
#line 133 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2304 "lang11d_tab.cpp"
    break;

  case 36: /* cmdlinecode: '(' argdecls1 funcbody ')'  */
#line 135 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-2], NULL, (PyrParseNode*)yyvsp[-1], false); }
#line 2310 "lang11d_tab.cpp"
    break;

  case 37: /* cmdlinecode: '(' funcvardecls1 funcbody ')'  */
#line 137 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2316 "lang11d_tab.cpp"
    break;

  case 38: /* cmdlinecode: argdecls1 funcvardecls1 funcbody  */
#line 139 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-2], (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2322 "lang11d_tab.cpp"
    break;

  case 39: /* cmdlinecode: argdecls1 funcbody  */
#line 141 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-1], NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2328 "lang11d_tab.cpp"
    break;

  case 40: /* cmdlinecode: funcvardecls1 funcbody  */
#line 143 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2334 "lang11d_tab.cpp"
    break;

  case 41: /* cmdlinecode: funcbody  */
#line 145 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2340 "lang11d_tab.cpp"
    break;

  case 43: /* methbody: exprseq retval  */
#line 150 "lang11d"
                                { yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2346 "lang11d_tab.cpp"
    break;

  case 44: /* primitive: %empty  */
#line 153 "lang11d"
                  { yyval = 0; }
#line 2352 "lang11d_tab.cpp"
    break;

  case 45: /* primitive: primname optsemi  */
#line 155 "lang11d"
                                { yyval = yyvsp[-1]; }
#line 2358 "lang11d_tab.cpp"
    break;

  case 46: /* retval: %empty  */
#line 159 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode(NULL); }
#line 2364 "lang11d_tab.cpp"
    break;

  case 47: /* retval: '^' expr optsemi  */
#line 161 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); }
#line 2370 "lang11d_tab.cpp"
    break;

  case 48: /* funretval: %empty  */
#line 165 "lang11d"
                        { yyval = (intptr_t)newPyrBlockReturnNode(); }
#line 2376 "lang11d_tab.cpp"
    break;

  case 49: /* funretval: '^' expr optsemi  */
#line 167 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); }
#line 2382 "lang11d_tab.cpp"
    break;

  case 51: /* blocklist1: blocklist1 blocklistitem  */
#line 172 "lang11d"
                                {
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
				}
#line 2390 "lang11d_tab.cpp"
    break;

  case 54: /* blocklist: %empty  */
#line 181 "lang11d"
                        { yyval = 0; }
#line 2396 "lang11d_tab.cpp"
    break;

  case 56: /* msgsend: name blocklist1  */
#line 186 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2404 "lang11d_tab.cpp"
    break;

  case 57: /* msgsend: '(' binop2 ')' blocklist1  */
#line 190 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2412 "lang11d_tab.cpp"
    break;

  case 58: /* msgsend: name '(' ')' blocklist1  */
#line 194 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-3], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2420 "lang11d_tab.cpp"
    break;

  case 59: /* msgsend: name '(' arglist1 optkeyarglist ')' blocklist  */
#line 198 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2429 "lang11d_tab.cpp"
    break;

  case 60: /* msgsend: '(' binop2 ')' '(' ')' blocklist1  */
#line 203 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-4], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2437 "lang11d_tab.cpp"
    break;

  case 61: /* msgsend: '(' binop2 ')' '(' arglist1 optkeyarglist ')' blocklist  */
#line 207 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2446 "lang11d_tab.cpp"
    break;

  case 62: /* msgsend: name '(' arglistv1 optkeyarglist ')'  */
#line 212 "lang11d"
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
				selectornode = newPyrSlotNode(&slot);
				args = linkAfterHead(
					(PyrParseNode*)yyvsp[-2],
					newPyrPushLitNode((PyrSlotNode*)yyvsp[-4], NULL));
				yyval = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)yyvsp[-1], 0);
			}
#line 2468 "lang11d_tab.cpp"
    break;

  case 63: /* msgsend: '(' binop2 ')' '(' arglistv1 optkeyarglist ')'  */
#line 230 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_performList);
				selectornode = newPyrSlotNode(&slot);
				args = linkAfterHead(
					(PyrParseNode*)yyvsp[-2],
					newPyrPushLitNode((PyrSlotNode*)yyvsp[-5], NULL));
				yyval = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)yyvsp[-1], 0);
			}
#line 2485 "lang11d_tab.cpp"
    break;

  case 64: /* msgsend: classname '[' arrayelems ']'  */
#line 243 "lang11d"
                        { yyval = (intptr_t)newPyrDynListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 2491 "lang11d_tab.cpp"
    break;

  case 65: /* msgsend: classname blocklist1  */
#line 245 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, (PyrParseNode*)yyvsp[0]);
			}
#line 2506 "lang11d_tab.cpp"
    break;

  case 66: /* msgsend: classname '(' ')' blocklist  */
#line 256 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2521 "lang11d_tab.cpp"
    break;

  case 67: /* msgsend: classname '(' keyarglist1 optcomma ')' blocklist  */
#line 267 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)yyvsp[-5]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2536 "lang11d_tab.cpp"
    break;

  case 68: /* msgsend: classname '(' arglist1 optkeyarglist ')' blocklist  */
#line 278 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)yyvsp[-5]),
					(PyrParseNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2553 "lang11d_tab.cpp"
    break;

  case 69: /* msgsend: classname '(' arglistv1 optkeyarglist ')'  */
#line 291 "lang11d"
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
				selectornode = newPyrSlotNode(&slot);
				selectornode2 = newPyrSlotNode(&slot2);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)yyvsp[-4]),
					newPyrPushLitNode(selectornode2, NULL));
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-2]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)yyvsp[0], 0);
			}
#line 2578 "lang11d_tab.cpp"
    break;

  case 70: /* msgsend: expr '.' '(' ')' blocklist  */
#line 312 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				yyval = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)yyvsp[-4], NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2591 "lang11d_tab.cpp"
    break;

  case 71: /* msgsend: expr '.' '(' keyarglist1 optcomma ')' blocklist  */
#line 321 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				yyval = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2604 "lang11d_tab.cpp"
    break;

  case 72: /* msgsend: expr '.' name '(' keyarglist1 optcomma ')' blocklist  */
#line 330 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-7],
					(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2613 "lang11d_tab.cpp"
    break;

  case 73: /* msgsend: expr '.' '(' arglist1 optkeyarglist ')' blocklist  */
#line 335 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-6],
					(PyrParseNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2630 "lang11d_tab.cpp"
    break;

  case 74: /* msgsend: expr '.' '(' arglistv1 optkeyarglist ')'  */
#line 349 "lang11d"
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
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-5],
					newPyrPushLitNode(newPyrSlotNode(&slot2), NULL));
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-2]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)yyvsp[-1], 0);
			}
#line 2654 "lang11d_tab.cpp"
    break;

  case 75: /* msgsend: expr '.' name '(' ')' blocklist  */
#line 371 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-5], NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2662 "lang11d_tab.cpp"
    break;

  case 76: /* msgsend: expr '.' name '(' arglist1 optkeyarglist ')' blocklist  */
#line 375 "lang11d"
                        {
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2672 "lang11d_tab.cpp"
    break;

  case 77: /* msgsend: expr '.' name '(' arglistv1 optkeyarglist ')'  */
#line 381 "lang11d"
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
				selectornode = newPyrSlotNode(&slot);

				args = linkNextNode((PyrParseNode*)yyvsp[-6], newPyrPushLitNode((PyrSlotNode*)yyvsp[-4], NULL));
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-2]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, (PyrParseNode*)yyvsp[-1], 0);
			}
#line 2694 "lang11d_tab.cpp"
    break;

  case 78: /* msgsend: expr '.' name blocklist  */
#line 399 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[-3], 0, (PyrParseNode*)yyvsp[0]);
			}
#line 2702 "lang11d_tab.cpp"
    break;

  case 79: /* $@1: %empty  */
#line 404 "lang11d"
                            { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 1); }
#line 2708 "lang11d_tab.cpp"
    break;

  case 80: /* generator: '{' ':' exprseq $@1 ',' qual '}'  */
#line 405 "lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, getsym("r"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);

				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)yyvsp[-1], false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				yyval = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)blocklit, 0, 0);
			}
#line 2722 "lang11d_tab.cpp"
    break;

  case 81: /* $@2: %empty  */
#line 414 "lang11d"
                                  { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 2); }
#line 2728 "lang11d_tab.cpp"
    break;

  case 82: /* generator: '{' ';' exprseq $@2 ',' qual '}'  */
#line 415 "lang11d"
                        {
				yyval = yyvsp[-1];
			}
#line 2736 "lang11d_tab.cpp"
    break;

  case 83: /* nextqual: %empty  */
#line 421 "lang11d"
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

							yyval = (intptr_t)newPyrCallNode(selectornode, expr, 0, 0);
						} break;
						case 2 :
						{
							yyval = (intptr_t)expr;
						} break;
					}
				}
#line 2762 "lang11d_tab.cpp"
    break;

  case 84: /* nextqual: ',' qual  */
#line 443 "lang11d"
                                { yyval = yyvsp[0]; }
#line 2768 "lang11d_tab.cpp"
    break;

  case 85: /* qual: name LEFTARROW exprseq nextqual  */
#line 447 "lang11d"
                        {
				// later should check if exprseq is a series and optimize it to for loop
				PyrParseNode *exprseq = (PyrParseNode*)yyvsp[-1];
				if (exprseq->mClassno == pn_CallNode) {
					PyrCallNode *callnode = (PyrCallNode*)exprseq;
					if (slotRawSymbol(&callnode->mSelector->mSlot) == s_series)
					{
						SetSymbol(&callnode->mSelector->mSlot, getsym("forSeries"));

						PyrVarDefNode* var = newPyrVarDefNode((PyrSlotNode*)yyvsp[-3], NULL, 0);
						PyrArgListNode* args = newPyrArgListNode(var, NULL, NULL);
						PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)yyvsp[0], false);
						PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);

						callnode->mArglist = linkNextNode(callnode->mArglist, blocklit);
						yyval = (intptr_t)callnode;

					} else goto notoptimized1;
				} else {
					notoptimized1:
					PyrSlot slot;
					SetSymbol(&slot, getsym("do"));
					PyrSlotNode* selectornode = newPyrSlotNode(&slot);

					PyrVarDefNode* var = newPyrVarDefNode((PyrSlotNode*)yyvsp[-3], NULL, 0);
					PyrArgListNode* args = newPyrArgListNode(var, NULL, NULL);
					PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)yyvsp[0], false);
					PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);

					PyrParseNode* args2 = linkNextNode(exprseq, blocklit);
					yyval = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
				}
			}
#line 2806 "lang11d_tab.cpp"
    break;

  case 86: /* qual: name name LEFTARROW exprseq nextqual  */
#line 481 "lang11d"
                        {
				// later should check if exprseq is a series and optimize it to for loop
				PyrParseNode *exprseq = (PyrParseNode*)yyvsp[-1];
				if (exprseq->mClassno == pn_CallNode) {
					PyrCallNode *callnode = (PyrCallNode*)exprseq;
					if (slotRawSymbol(&callnode->mSelector->mSlot) == s_series)
					{
						SetSymbol(&callnode->mSelector->mSlot, getsym("forSeries"));

						PyrVarDefNode* var1 = newPyrVarDefNode((PyrSlotNode*)yyvsp[-4], NULL, 0);
						PyrVarDefNode* var2 = newPyrVarDefNode((PyrSlotNode*)yyvsp[-3], NULL, 0);
						PyrVarDefNode* vars = (PyrVarDefNode*)linkNextNode(var1, var2);
						PyrArgListNode* args = newPyrArgListNode(vars, NULL, NULL);
						PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)yyvsp[0], false);
						PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);

						callnode->mArglist = linkNextNode(callnode->mArglist, blocklit);
						yyval = (intptr_t)callnode;

					} else goto notoptimized2;
				} else {
					notoptimized2:
					PyrSlot slot;
					SetSymbol(&slot, getsym("do"));
					PyrSlotNode* selectornode = newPyrSlotNode(&slot);

					PyrVarDefNode* var1 = newPyrVarDefNode((PyrSlotNode*)yyvsp[-4], NULL, 0);
					PyrVarDefNode* var2 = newPyrVarDefNode((PyrSlotNode*)yyvsp[-3], NULL, 0);
					PyrVarDefNode* vars = (PyrVarDefNode*)linkNextNode(var1, var2);
					PyrArgListNode* args = newPyrArgListNode(vars, NULL, NULL);
					PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)yyvsp[0], false);
					PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);

					PyrParseNode* args2 = linkNextNode(exprseq, blocklit);
					yyval = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
				}
			}
#line 2848 "lang11d_tab.cpp"
    break;

  case 87: /* qual: VAR name '=' exprseq nextqual  */
#line 519 "lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, s_value);
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);

				PyrVarDefNode* var = newPyrVarDefNode((PyrSlotNode*)yyvsp[-3], NULL, 0);
				PyrArgListNode* args = newPyrArgListNode(var, NULL, NULL);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(args, 0, (PyrParseNode*)yyvsp[0], false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode(blocklit, (PyrParseNode*)yyvsp[-1]);

				yyval = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
			}
#line 2866 "lang11d_tab.cpp"
    break;

  case 88: /* qual: exprseq nextqual  */
#line 533 "lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, getsym("if"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)yyvsp[0], false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-1], blocklit);

				yyval = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
			}
#line 2881 "lang11d_tab.cpp"
    break;

  case 89: /* qual: ':' ':' exprseq nextqual  */
#line 544 "lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
			}
#line 2889 "lang11d_tab.cpp"
    break;

  case 90: /* qual: ':' WHILE exprseq nextqual  */
#line 548 "lang11d"
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
				PyrParseNode *block2 = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)yyvsp[0], false);
				PyrParseNode *blocklit2 = (PyrParseNode*)newPyrPushLitNode(NULL, block2);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-1], blocklit2);
				PyrParseNode* args3 = (PyrParseNode*)linkNextNode(args2, blocklit1);

				yyval = (intptr_t)newPyrCallNode(selectornode2, args3, 0, 0);
			}
#line 2916 "lang11d_tab.cpp"
    break;

  case 97: /* expr1: '(' exprseq ')'  */
#line 579 "lang11d"
                        {
				PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
				node->mParens = 1;
				yyval = yyvsp[-1];
			}
#line 2926 "lang11d_tab.cpp"
    break;

  case 98: /* expr1: '~' name  */
#line 585 "lang11d"
                        {
				PyrParseNode* argnode;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL);
				SetSymbol(&slot, s_envirGet);
				selectornode = newPyrSlotNode(&slot);
				yyval = (intptr_t)newPyrCallNode(selectornode, argnode, 0, 0);
			}
#line 2940 "lang11d_tab.cpp"
    break;

  case 99: /* expr1: '[' arrayelems ']'  */
#line 595 "lang11d"
                        { yyval = (intptr_t)newPyrDynListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 2946 "lang11d_tab.cpp"
    break;

  case 100: /* expr1: '(' valrange2 ')'  */
#line 597 "lang11d"
                        { yyval = yyvsp[-1]; }
#line 2952 "lang11d_tab.cpp"
    break;

  case 101: /* expr1: '(' ':' valrange3 ')'  */
#line 599 "lang11d"
                        { yyval = yyvsp[-1]; }
#line 2958 "lang11d_tab.cpp"
    break;

  case 102: /* expr1: '(' dictslotlist ')'  */
#line 601 "lang11d"
                        { yyval = (intptr_t)newPyrDynDictNode((PyrParseNode*)yyvsp[-1]); }
#line 2964 "lang11d_tab.cpp"
    break;

  case 103: /* expr1: pseudovar  */
#line 603 "lang11d"
                        { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 2970 "lang11d_tab.cpp"
    break;

  case 104: /* expr1: expr1 '[' arglist1 ']'  */
#line 605 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-3],
					(PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 2987 "lang11d_tab.cpp"
    break;

  case 106: /* valrangex1: expr1 '[' arglist1 DOTDOT ']'  */
#line 621 "lang11d"
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
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[-2]);
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3018 "lang11d_tab.cpp"
    break;

  case 107: /* valrangex1: expr1 '[' DOTDOT exprseq ']'  */
#line 648 "lang11d"
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
				args = linkNextNode((PyrParseNode*)yyvsp[-4], nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3040 "lang11d_tab.cpp"
    break;

  case 108: /* valrangex1: expr1 '[' arglist1 DOTDOT exprseq ']'  */
#line 666 "lang11d"
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
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-3]);
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3069 "lang11d_tab.cpp"
    break;

  case 109: /* valrangeassign: expr1 '[' arglist1 DOTDOT ']' '=' expr  */
#line 693 "lang11d"
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
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-4]);
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3101 "lang11d_tab.cpp"
    break;

  case 110: /* valrangeassign: expr1 '[' DOTDOT exprseq ']' '=' expr  */
#line 721 "lang11d"
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
				args = linkNextNode((PyrParseNode*)yyvsp[-6], nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-3]);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3124 "lang11d_tab.cpp"
    break;

  case 111: /* valrangeassign: expr1 '[' arglist1 DOTDOT exprseq ']' '=' expr  */
#line 740 "lang11d"
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
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-5]);
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-3]);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3154 "lang11d_tab.cpp"
    break;

  case 112: /* valrangexd: expr '.' '[' arglist1 DOTDOT ']'  */
#line 768 "lang11d"
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
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_copyseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-2]);
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3185 "lang11d_tab.cpp"
    break;

  case 113: /* valrangexd: expr '.' '[' DOTDOT exprseq ']'  */
#line 795 "lang11d"
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
				args = linkNextNode((PyrParseNode*)yyvsp[-5], nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3207 "lang11d_tab.cpp"
    break;

  case 114: /* valrangexd: expr '.' '[' arglist1 DOTDOT exprseq ']'  */
#line 813 "lang11d"
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
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-3]);
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3236 "lang11d_tab.cpp"
    break;

  case 115: /* valrangexd: expr '.' '[' arglist1 DOTDOT ']' '=' expr  */
#line 838 "lang11d"
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
				nilnode2 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_putseries);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-4]);
				if (arglen < 2) {
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3268 "lang11d_tab.cpp"
    break;

  case 116: /* valrangexd: expr '.' '[' DOTDOT exprseq ']' '=' expr  */
#line 866 "lang11d"
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
				args = linkNextNode((PyrParseNode*)yyvsp[-7], nilnode1);
				args = linkNextNode(args, nilnode2);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-3]);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3291 "lang11d_tab.cpp"
    break;

  case 117: /* valrangexd: expr '.' '[' arglist1 DOTDOT exprseq ']' '=' expr  */
#line 885 "lang11d"
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
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-8], (PyrParseNode*)yyvsp[-5]);
				if (arglen < 2) {
					SetNil(&nilSlot);
					nilnode1 = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);
					args = linkNextNode(args, nilnode1);
				}
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-3]);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3321 "lang11d_tab.cpp"
    break;

  case 118: /* valrange2: exprseq DOTDOT  */
#line 913 "lang11d"
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
				args = linkNextNode((PyrParseNode*)yyvsp[-1], nilnode);
				args = linkNextNode(args, nilnode2);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3343 "lang11d_tab.cpp"
    break;

  case 119: /* valrange2: DOTDOT exprseq  */
#line 932 "lang11d"
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
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3365 "lang11d_tab.cpp"
    break;

  case 120: /* valrange2: exprseq DOTDOT exprseq  */
#line 951 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-2], nilnode);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3385 "lang11d_tab.cpp"
    break;

  case 121: /* valrange2: exprseq ',' exprseq DOTDOT exprseq  */
#line 968 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot selectorSlot;
				PyrParseNode* args;

				SetSymbol(&selectorSlot, s_series);
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-4],
					(PyrParseNode*)yyvsp[-2]);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3403 "lang11d_tab.cpp"
    break;

  case 122: /* valrange2: exprseq ',' exprseq DOTDOT  */
#line 982 "lang11d"
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
					(PyrParseNode*)yyvsp[-3],
					(PyrParseNode*)yyvsp[-1]);
				args = linkNextNode(args, nilnode);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3426 "lang11d_tab.cpp"
    break;

  case 123: /* valrange3: DOTDOT exprseq  */
#line 1003 "lang11d"
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
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3448 "lang11d_tab.cpp"
    break;

  case 124: /* valrange3: exprseq DOTDOT  */
#line 1022 "lang11d"
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
				args = linkNextNode((PyrParseNode*)yyvsp[-1], nilnode);
				args = linkNextNode(args, nilnode2);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3469 "lang11d_tab.cpp"
    break;

  case 125: /* valrange3: exprseq DOTDOT exprseq  */
#line 1040 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-2], nilnode);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3489 "lang11d_tab.cpp"
    break;

  case 126: /* valrange3: exprseq ',' exprseq DOTDOT  */
#line 1057 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrPushLitNode *nilnode;
				PyrSlot selectorSlot, nilSlot;
				PyrParseNode* args;

				SetNil(&nilSlot);
				nilnode = newPyrPushLitNode(newPyrSlotNode(&nilSlot), NULL);

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]);
				args = linkNextNode(args, nilnode);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3509 "lang11d_tab.cpp"
    break;

  case 127: /* valrange3: exprseq ',' exprseq DOTDOT exprseq  */
#line 1073 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot selectorSlot;
				PyrParseNode* args;

				SetSymbol(&selectorSlot, getsym("seriesIter"));
				selectornode = newPyrSlotNode(&selectorSlot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-4],
					(PyrParseNode*)yyvsp[-2]);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3527 "lang11d_tab.cpp"
    break;

  case 131: /* expr: classname  */
#line 1091 "lang11d"
                            { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3533 "lang11d_tab.cpp"
    break;

  case 132: /* expr: expr '.' '[' arglist1 ']'  */
#line 1093 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-4],
					(PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3550 "lang11d_tab.cpp"
    break;

  case 133: /* expr: '`' expr  */
#line 1106 "lang11d"
                        {
				PyrParseNode *node, *args;
				PyrSlotNode *slotnode;
				PyrSlot slot;

				SetSymbol(&slot, s_ref);
				slotnode = newPyrSlotNode(&slot);
				node = (PyrParseNode*)newPyrPushNameNode(slotnode);
				args = linkNextNode(node, (PyrParseNode*)yyvsp[0]);
				SetSymbol(&slot, s_new);
				slotnode = newPyrSlotNode(&slot);
				yyval = (intptr_t)newPyrCallNode(slotnode, args, 0, 0);
			}
#line 3568 "lang11d_tab.cpp"
    break;

  case 134: /* expr: expr binop2 adverb expr  */
#line 1120 "lang11d"
                        {
				yyval = (intptr_t)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0], (PyrParseNode*)yyvsp[-1]);
			}
#line 3577 "lang11d_tab.cpp"
    break;

  case 135: /* expr: name '=' expr  */
#line 1125 "lang11d"
                        {
				yyval = (intptr_t)newPyrAssignNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0);
			}
#line 3585 "lang11d_tab.cpp"
    break;

  case 136: /* expr: '~' name '=' expr  */
#line 1129 "lang11d"
                        {
				PyrParseNode *argnode, *args;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)yyvsp[-2], NULL);
				args = linkNextNode(argnode, (PyrParseNode*)yyvsp[0]);
				SetSymbol(&slot, s_envirPut);
				selectornode = newPyrSlotNode(&slot);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3600 "lang11d_tab.cpp"
    break;

  case 137: /* expr: expr '.' name '=' expr  */
#line 1140 "lang11d"
                        {
				yyval = (intptr_t)newPyrSetterNode((PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3609 "lang11d_tab.cpp"
    break;

  case 138: /* expr: name '(' arglist1 optkeyarglist ')' '=' expr  */
#line 1145 "lang11d"
                        {
				if (yyvsp[-3] != 0) {
					error("Setter method called with keyword arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-3]);
					compileErrors++;
				}
				yyval = (intptr_t)newPyrSetterNode((PyrSlotNode*)yyvsp[-6],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3623 "lang11d_tab.cpp"
    break;

  case 139: /* expr: '#' mavars '=' expr  */
#line 1155 "lang11d"
                        {
				yyval = (intptr_t)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[0], 0);
			}
#line 3632 "lang11d_tab.cpp"
    break;

  case 140: /* expr: expr1 '[' arglist1 ']' '=' expr  */
#line 1160 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_put);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-5],
					(PyrParseNode*)yyvsp[-3]);
				args = linkNextNode( args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3650 "lang11d_tab.cpp"
    break;

  case 141: /* expr: expr '.' '[' arglist1 ']' '=' expr  */
#line 1174 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;

				SetSymbol(&slot, s_put);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-6],
					(PyrParseNode*)yyvsp[-3]);
				args = linkNextNode( args, (PyrParseNode*)yyvsp[0]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, 0, 0);
			}
#line 3668 "lang11d_tab.cpp"
    break;

  case 142: /* adverb: %empty  */
#line 1189 "lang11d"
          { yyval = 0; }
#line 3674 "lang11d_tab.cpp"
    break;

  case 143: /* adverb: '.' name  */
#line 1190 "lang11d"
                           { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3680 "lang11d_tab.cpp"
    break;

  case 144: /* adverb: '.' integer  */
#line 1191 "lang11d"
                              { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3686 "lang11d_tab.cpp"
    break;

  case 145: /* adverb: '.' '(' exprseq ')'  */
#line 1192 "lang11d"
                                      { yyval = yyvsp[-1]; }
#line 3692 "lang11d_tab.cpp"
    break;

  case 147: /* exprn: exprn ';' expr  */
#line 1197 "lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 3700 "lang11d_tab.cpp"
    break;

  case 149: /* arrayelems: %empty  */
#line 1205 "lang11d"
                  { yyval = 0; }
#line 3706 "lang11d_tab.cpp"
    break;

  case 150: /* arrayelems: arrayelems1 optcomma  */
#line 1207 "lang11d"
                          { yyval = yyvsp[-1]; }
#line 3712 "lang11d_tab.cpp"
    break;

  case 152: /* arrayelems1: exprseq ':' exprseq  */
#line 1212 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3718 "lang11d_tab.cpp"
    break;

  case 153: /* arrayelems1: keybinop exprseq  */
#line 1214 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 3727 "lang11d_tab.cpp"
    break;

  case 154: /* arrayelems1: arrayelems1 ',' exprseq  */
#line 1219 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3733 "lang11d_tab.cpp"
    break;

  case 155: /* arrayelems1: arrayelems1 ',' keybinop exprseq  */
#line 1221 "lang11d"
                                {
					PyrParseNode* elems;
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					elems = (PyrParseNode*)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], elems);
				}
#line 3744 "lang11d_tab.cpp"
    break;

  case 156: /* arrayelems1: arrayelems1 ',' exprseq ':' exprseq  */
#line 1228 "lang11d"
                                {
					PyrParseNode* elems;
					elems = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-4], elems);
				}
#line 3754 "lang11d_tab.cpp"
    break;

  case 158: /* arglist1: arglist1 ',' exprseq  */
#line 1237 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3760 "lang11d_tab.cpp"
    break;

  case 159: /* arglistv1: '*' exprseq  */
#line 1241 "lang11d"
                                { yyval = yyvsp[0]; }
#line 3766 "lang11d_tab.cpp"
    break;

  case 160: /* arglistv1: arglist1 ',' '*' exprseq  */
#line 1243 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]); }
#line 3772 "lang11d_tab.cpp"
    break;

  case 162: /* keyarglist1: keyarglist1 ',' keyarg  */
#line 1248 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3778 "lang11d_tab.cpp"
    break;

  case 163: /* keyarg: keybinop exprseq  */
#line 1252 "lang11d"
                                { yyval = (intptr_t)newPyrPushKeyArgNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 3784 "lang11d_tab.cpp"
    break;

  case 164: /* optkeyarglist: optcomma  */
#line 1255 "lang11d"
                           { yyval = 0; }
#line 3790 "lang11d_tab.cpp"
    break;

  case 165: /* optkeyarglist: ',' keyarglist1 optcomma  */
#line 1256 "lang11d"
                                                           { yyval = yyvsp[-1]; }
#line 3796 "lang11d_tab.cpp"
    break;

  case 166: /* mavars: mavarlist  */
#line 1260 "lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3802 "lang11d_tab.cpp"
    break;

  case 167: /* mavars: mavarlist ELLIPSIS name  */
#line 1262 "lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); }
#line 3808 "lang11d_tab.cpp"
    break;

  case 169: /* mavarlist: mavarlist ',' name  */
#line 1267 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3814 "lang11d_tab.cpp"
    break;

  case 170: /* slotliteral: integer  */
#line 1271 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3820 "lang11d_tab.cpp"
    break;

  case 171: /* slotliteral: floatp  */
#line 1272 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3826 "lang11d_tab.cpp"
    break;

  case 172: /* slotliteral: ascii  */
#line 1273 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3832 "lang11d_tab.cpp"
    break;

  case 173: /* slotliteral: string  */
#line 1274 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3838 "lang11d_tab.cpp"
    break;

  case 174: /* slotliteral: symbol  */
#line 1275 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3844 "lang11d_tab.cpp"
    break;

  case 175: /* slotliteral: trueobj  */
#line 1276 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3850 "lang11d_tab.cpp"
    break;

  case 176: /* slotliteral: falseobj  */
#line 1277 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3856 "lang11d_tab.cpp"
    break;

  case 177: /* slotliteral: nilobj  */
#line 1278 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3862 "lang11d_tab.cpp"
    break;

  case 178: /* slotliteral: listlit  */
#line 1279 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3868 "lang11d_tab.cpp"
    break;

  case 179: /* blockliteral: block  */
#line 1282 "lang11d"
                        { yyval = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3874 "lang11d_tab.cpp"
    break;

  case 180: /* pushname: name  */
#line 1285 "lang11d"
                                { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3880 "lang11d_tab.cpp"
    break;

  case 181: /* pushliteral: integer  */
#line 1288 "lang11d"
                                { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3886 "lang11d_tab.cpp"
    break;

  case 182: /* pushliteral: floatp  */
#line 1289 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3892 "lang11d_tab.cpp"
    break;

  case 183: /* pushliteral: ascii  */
#line 1290 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3898 "lang11d_tab.cpp"
    break;

  case 184: /* pushliteral: string  */
#line 1291 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3904 "lang11d_tab.cpp"
    break;

  case 185: /* pushliteral: symbol  */
#line 1292 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3910 "lang11d_tab.cpp"
    break;

  case 186: /* pushliteral: trueobj  */
#line 1293 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3916 "lang11d_tab.cpp"
    break;

  case 187: /* pushliteral: falseobj  */
#line 1294 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3922 "lang11d_tab.cpp"
    break;

  case 188: /* pushliteral: nilobj  */
#line 1295 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3928 "lang11d_tab.cpp"
    break;

  case 189: /* pushliteral: listlit  */
#line 1296 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3934 "lang11d_tab.cpp"
    break;

  case 190: /* listliteral: integer  */
#line 1299 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3940 "lang11d_tab.cpp"
    break;

  case 191: /* listliteral: floatp  */
#line 1300 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3946 "lang11d_tab.cpp"
    break;

  case 192: /* listliteral: ascii  */
#line 1301 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3952 "lang11d_tab.cpp"
    break;

  case 193: /* listliteral: string  */
#line 1302 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3958 "lang11d_tab.cpp"
    break;

  case 194: /* listliteral: symbol  */
#line 1303 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3964 "lang11d_tab.cpp"
    break;

  case 195: /* listliteral: name  */
#line 1304 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3970 "lang11d_tab.cpp"
    break;

  case 196: /* listliteral: trueobj  */
#line 1305 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3976 "lang11d_tab.cpp"
    break;

  case 197: /* listliteral: falseobj  */
#line 1306 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3982 "lang11d_tab.cpp"
    break;

  case 198: /* listliteral: nilobj  */
#line 1307 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3988 "lang11d_tab.cpp"
    break;

  case 199: /* listliteral: listlit2  */
#line 1308 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3994 "lang11d_tab.cpp"
    break;

  case 200: /* block: '{' argdecls funcvardecls funcbody '}'  */
#line 1312 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], false); }
#line 4001 "lang11d_tab.cpp"
    break;

  case 201: /* block: BEGINCLOSEDFUNC argdecls funcvardecls funcbody '}'  */
#line 1315 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], true); }
#line 4008 "lang11d_tab.cpp"
    break;

  case 202: /* funcvardecls: %empty  */
#line 1319 "lang11d"
                  { yyval = 0; }
#line 4014 "lang11d_tab.cpp"
    break;

  case 203: /* funcvardecls: funcvardecls funcvardecl  */
#line 1321 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 4020 "lang11d_tab.cpp"
    break;

  case 205: /* funcvardecls1: funcvardecls1 funcvardecl  */
#line 1326 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 4026 "lang11d_tab.cpp"
    break;

  case 206: /* funcvardecl: VAR vardeflist ';'  */
#line 1330 "lang11d"
                                { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varLocal); }
#line 4032 "lang11d_tab.cpp"
    break;

  case 207: /* argdecls: %empty  */
#line 1333 "lang11d"
                  { yyval = 0; }
#line 4038 "lang11d_tab.cpp"
    break;

  case 208: /* argdecls: ARG vardeflist ';'  */
#line 1335 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4046 "lang11d_tab.cpp"
    break;

  case 209: /* argdecls: ARG vardeflist0 ELLIPSIS name ';'  */
#line 1339 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4054 "lang11d_tab.cpp"
    break;

  case 210: /* argdecls: '|' slotdeflist '|'  */
#line 1343 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4062 "lang11d_tab.cpp"
    break;

  case 211: /* argdecls: '|' slotdeflist0 ELLIPSIS name '|'  */
#line 1347 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4070 "lang11d_tab.cpp"
    break;

  case 212: /* argdecls: '|' slotdeflist0 ELLIPSIS name ',' name '|'  */
#line 1351 "lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 4078 "lang11d_tab.cpp"
    break;

  case 213: /* argdecls1: ARG vardeflist ';'  */
#line 1357 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4086 "lang11d_tab.cpp"
    break;

  case 214: /* argdecls1: ARG vardeflist0 ELLIPSIS name ';'  */
#line 1361 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4094 "lang11d_tab.cpp"
    break;

  case 215: /* argdecls1: '|' slotdeflist '|'  */
#line 1365 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4102 "lang11d_tab.cpp"
    break;

  case 216: /* argdecls1: '|' slotdeflist0 ELLIPSIS name '|'  */
#line 1369 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4110 "lang11d_tab.cpp"
    break;

  case 217: /* argdecls1: '|' slotdeflist0 ELLIPSIS name ',' name '|'  */
#line 1373 "lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 4118 "lang11d_tab.cpp"
    break;

  case 219: /* constdeflist: constdeflist optcomma constdef  */
#line 1381 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4124 "lang11d_tab.cpp"
    break;

  case 220: /* constdef: rspec name '=' slotliteral  */
#line 1385 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4130 "lang11d_tab.cpp"
    break;

  case 221: /* slotdeflist0: %empty  */
#line 1388 "lang11d"
                  { yyval = 0; }
#line 4136 "lang11d_tab.cpp"
    break;

  case 224: /* slotdeflist: slotdeflist optcomma slotdef  */
#line 1394 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4142 "lang11d_tab.cpp"
    break;

  case 225: /* slotdef: name  */
#line 1398 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4148 "lang11d_tab.cpp"
    break;

  case 226: /* slotdef: name optequal slotliteral  */
#line 1400 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4154 "lang11d_tab.cpp"
    break;

  case 227: /* slotdef: name optequal '(' exprseq ')'  */
#line 1402 "lang11d"
                                {
					PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
					node->mParens = 1;
					yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-4], node, 0);
				}
#line 4164 "lang11d_tab.cpp"
    break;

  case 228: /* vardeflist0: %empty  */
#line 1409 "lang11d"
                  { yyval = 0; }
#line 4170 "lang11d_tab.cpp"
    break;

  case 231: /* vardeflist: vardeflist ',' vardef  */
#line 1415 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4176 "lang11d_tab.cpp"
    break;

  case 232: /* vardef: name  */
#line 1419 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4182 "lang11d_tab.cpp"
    break;

  case 233: /* vardef: name '=' expr  */
#line 1421 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4188 "lang11d_tab.cpp"
    break;

  case 234: /* vardef: name '(' exprseq ')'  */
#line 1423 "lang11d"
                                {
									PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
									node->mParens = 1;
									yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-3], node, 0);
								}
#line 4198 "lang11d_tab.cpp"
    break;

  case 235: /* dictslotdef: exprseq ':' exprseq  */
#line 1431 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4204 "lang11d_tab.cpp"
    break;

  case 236: /* dictslotdef: keybinop exprseq  */
#line 1433 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 4213 "lang11d_tab.cpp"
    break;

  case 238: /* dictslotlist1: dictslotlist1 ',' dictslotdef  */
#line 1441 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4219 "lang11d_tab.cpp"
    break;

  case 239: /* dictslotlist: %empty  */
#line 1444 "lang11d"
                  { yyval = 0; }
#line 4225 "lang11d_tab.cpp"
    break;

  case 242: /* rwslotdeflist: rwslotdeflist ',' rwslotdef  */
#line 1450 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4231 "lang11d_tab.cpp"
    break;

  case 243: /* rwslotdef: rwspec name  */
#line 1454 "lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); }
#line 4237 "lang11d_tab.cpp"
    break;

  case 244: /* rwslotdef: rwspec name '=' slotliteral  */
#line 1456 "lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4243 "lang11d_tab.cpp"
    break;

  case 245: /* listlit: '#' '[' literallistc ']'  */
#line 1480 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 4249 "lang11d_tab.cpp"
    break;

  case 246: /* listlit: '#' classname '[' literallistc ']'  */
#line 1482 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4255 "lang11d_tab.cpp"
    break;

  case 247: /* listlit2: '[' literallistc ']'  */
#line 1486 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 4261 "lang11d_tab.cpp"
    break;

  case 248: /* listlit2: classname '[' literallistc ']'  */
#line 1488 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4267 "lang11d_tab.cpp"
    break;

  case 249: /* literallistc: %empty  */
#line 1491 "lang11d"
                  { yyval = 0; }
#line 4273 "lang11d_tab.cpp"
    break;

  case 252: /* literallist1: literallist1 ',' listliteral  */
#line 1497 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4279 "lang11d_tab.cpp"
    break;

  case 253: /* rwspec: %empty  */
#line 1500 "lang11d"
           { yyval = rwPrivate; }
#line 4285 "lang11d_tab.cpp"
    break;

  case 254: /* rwspec: '<'  */
#line 1502 "lang11d"
                        { yyval = rwReadOnly; }
#line 4291 "lang11d_tab.cpp"
    break;

  case 255: /* rwspec: READWRITEVAR  */
#line 1504 "lang11d"
                        { yyval = rwReadWrite; }
#line 4297 "lang11d_tab.cpp"
    break;

  case 256: /* rwspec: '>'  */
#line 1506 "lang11d"
                        { yyval = rwWriteOnly; }
#line 4303 "lang11d_tab.cpp"
    break;

  case 257: /* rspec: %empty  */
#line 1509 "lang11d"
           { yyval = rwPrivate; }
#line 4309 "lang11d_tab.cpp"
    break;

  case 258: /* rspec: '<'  */
#line 1511 "lang11d"
                        { yyval = rwReadOnly; }
#line 4315 "lang11d_tab.cpp"
    break;

  case 259: /* integer: INTEGER  */
#line 1514 "lang11d"
                  { yyval = zzval; }
#line 4321 "lang11d_tab.cpp"
    break;

  case 260: /* integer: '-' INTEGER  */
#line 1516 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawInt(&node->mSlot));
				yyval = zzval;
			}
#line 4332 "lang11d_tab.cpp"
    break;

  case 261: /* floatr: SC_FLOAT  */
#line 1524 "lang11d"
                   { yyval = zzval; }
#line 4338 "lang11d_tab.cpp"
    break;

  case 262: /* floatr: '-' SC_FLOAT  */
#line 1526 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawFloat(&node->mSlot));
				yyval = zzval;
			}
#line 4349 "lang11d_tab.cpp"
    break;

  case 263: /* accidental: ACCIDENTAL  */
#line 1534 "lang11d"
                        { yyval = zzval; }
#line 4355 "lang11d_tab.cpp"
    break;

  case 264: /* accidental: '-' ACCIDENTAL  */
#line 1536 "lang11d"
                                {
					PyrSlotNode *node;
					double intval, fracval;
					node = (PyrSlotNode*)zzval;
					intval = floor(slotRawFloat(&node->mSlot) + 0.5);
					fracval = slotRawFloat(&node->mSlot) - intval;
					SetRaw(&node->mSlot, -intval + fracval);
					yyval = zzval;
				}
#line 4369 "lang11d_tab.cpp"
    break;

  case 265: /* pie: PIE  */
#line 1546 "lang11d"
                      { yyval = zzval; }
#line 4375 "lang11d_tab.cpp"
    break;

  case 268: /* floatp: floatr pie  */
#line 1552 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)yyvsp[-1];
				SetRaw(&node->mSlot, slotRawFloat(&node->mSlot) * pi);
			}
#line 4385 "lang11d_tab.cpp"
    break;

  case 269: /* floatp: integer pie  */
#line 1558 "lang11d"
                        {
				PyrSlotNode *node;
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = slotRawInt(&node->mSlot);
				SetFloat(&node->mSlot, ival * pi);
			}
#line 4397 "lang11d_tab.cpp"
    break;

  case 270: /* floatp: pie  */
#line 1566 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, pi);
				yyval = zzval;
			}
#line 4408 "lang11d_tab.cpp"
    break;

  case 271: /* floatp: '-' pie  */
#line 1573 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, -pi);
				yyval = zzval;
			}
#line 4419 "lang11d_tab.cpp"
    break;

  case 272: /* name: NAME  */
#line 1581 "lang11d"
                       { yyval = zzval; }
#line 4425 "lang11d_tab.cpp"
    break;

  case 273: /* name: WHILE  */
#line 1582 "lang11d"
                                { yyval = zzval; }
#line 4431 "lang11d_tab.cpp"
    break;

  case 274: /* classname: CLASSNAME  */
#line 1585 "lang11d"
                                    { yyval = zzval; }
#line 4437 "lang11d_tab.cpp"
    break;

  case 275: /* primname: PRIMITIVENAME  */
#line 1588 "lang11d"
                                        { yyval = zzval; }
#line 4443 "lang11d_tab.cpp"
    break;

  case 276: /* trueobj: TRUEOBJ  */
#line 1591 "lang11d"
                          { yyval = zzval; }
#line 4449 "lang11d_tab.cpp"
    break;

  case 277: /* falseobj: FALSEOBJ  */
#line 1594 "lang11d"
                           { yyval = zzval; }
#line 4455 "lang11d_tab.cpp"
    break;

  case 278: /* nilobj: NILOBJ  */
#line 1597 "lang11d"
                         { yyval = zzval; }
#line 4461 "lang11d_tab.cpp"
    break;

  case 279: /* ascii: ASCII  */
#line 1600 "lang11d"
                        { yyval = zzval; }
#line 4467 "lang11d_tab.cpp"
    break;

  case 280: /* symbol: SYMBOL  */
#line 1603 "lang11d"
                         { yyval = zzval; }
#line 4473 "lang11d_tab.cpp"
    break;

  case 281: /* string: STRING  */
#line 1606 "lang11d"
                         { yyval = zzval; }
#line 4479 "lang11d_tab.cpp"
    break;

  case 282: /* pseudovar: PSEUDOVAR  */
#line 1609 "lang11d"
                            { yyval = zzval; }
#line 4485 "lang11d_tab.cpp"
    break;

  case 283: /* binop: BINOP  */
#line 1612 "lang11d"
                { yyval = zzval; }
#line 4491 "lang11d_tab.cpp"
    break;

  case 284: /* binop: READWRITEVAR  */
#line 1613 "lang11d"
                               { yyval = zzval; }
#line 4497 "lang11d_tab.cpp"
    break;

  case 285: /* binop: '<'  */
#line 1614 "lang11d"
                       { yyval = zzval; }
#line 4503 "lang11d_tab.cpp"
    break;

  case 286: /* binop: '>'  */
#line 1615 "lang11d"
                       { yyval = zzval; }
#line 4509 "lang11d_tab.cpp"
    break;

  case 287: /* binop: '-'  */
#line 1616 "lang11d"
                       { yyval = zzval; }
#line 4515 "lang11d_tab.cpp"
    break;

  case 288: /* binop: '*'  */
#line 1617 "lang11d"
                       { yyval = zzval; }
#line 4521 "lang11d_tab.cpp"
    break;

  case 289: /* binop: '+'  */
#line 1618 "lang11d"
                       { yyval = zzval; }
#line 4527 "lang11d_tab.cpp"
    break;

  case 290: /* binop: '|'  */
#line 1619 "lang11d"
                       { yyval = zzval; }
#line 4533 "lang11d_tab.cpp"
    break;

  case 291: /* keybinop: KEYBINOP  */
#line 1622 "lang11d"
                    { yyval = zzval; }
#line 4539 "lang11d_tab.cpp"
    break;

  case 294: /* curryarg: CURRYARG  */
#line 1629 "lang11d"
                    { yyval = zzval; }
#line 4545 "lang11d_tab.cpp"
    break;


#line 4549 "lang11d_tab.cpp"

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
          = {yyssp, yytoken};
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


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
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

