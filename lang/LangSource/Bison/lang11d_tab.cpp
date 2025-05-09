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
  YYSYMBOL_dictlit2 = 127,                 /* dictlit2  */
  YYSYMBOL_litdictslotdef = 128,           /* litdictslotdef  */
  YYSYMBOL_litdictslotlist1 = 129,         /* litdictslotlist1  */
  YYSYMBOL_litdictslotlist = 130,          /* litdictslotlist  */
  YYSYMBOL_listlit = 131,                  /* listlit  */
  YYSYMBOL_listlit2 = 132,                 /* listlit2  */
  YYSYMBOL_literallistc = 133,             /* literallistc  */
  YYSYMBOL_literallist1 = 134,             /* literallist1  */
  YYSYMBOL_rwspec = 135,                   /* rwspec  */
  YYSYMBOL_rspec = 136,                    /* rspec  */
  YYSYMBOL_integer = 137,                  /* integer  */
  YYSYMBOL_floatr = 138,                   /* floatr  */
  YYSYMBOL_accidental = 139,               /* accidental  */
  YYSYMBOL_pie = 140,                      /* pie  */
  YYSYMBOL_floatp = 141,                   /* floatp  */
  YYSYMBOL_name = 142,                     /* name  */
  YYSYMBOL_classname = 143,                /* classname  */
  YYSYMBOL_primname = 144,                 /* primname  */
  YYSYMBOL_trueobj = 145,                  /* trueobj  */
  YYSYMBOL_falseobj = 146,                 /* falseobj  */
  YYSYMBOL_nilobj = 147,                   /* nilobj  */
  YYSYMBOL_ascii = 148,                    /* ascii  */
  YYSYMBOL_symbol = 149,                   /* symbol  */
  YYSYMBOL_string = 150,                   /* string  */
  YYSYMBOL_pseudovar = 151,                /* pseudovar  */
  YYSYMBOL_binop = 152,                    /* binop  */
  YYSYMBOL_keybinop = 153,                 /* keybinop  */
  YYSYMBOL_binop2 = 154,                   /* binop2  */
  YYSYMBOL_curryarg = 155                  /* curryarg  */
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
#define YYFINAL  69
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1814

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  101
/* YYNRULES -- Number of rules.  */
#define YYNRULES  295
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  557

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
     311,   320,   329,   334,   348,   363,   385,   389,   395,   413,
     419,   419,   429,   429,   436,   457,   461,   495,   533,   547,
     558,   562,   587,   588,   589,   590,   591,   592,   593,   599,
     609,   611,   613,   615,   617,   619,   632,   635,   662,   680,
     707,   735,   754,   783,   802,   821,   838,   852,   873,   892,
     910,   927,   943,   959,   960,   961,   962,   976,   981,   985,
     996,  1001,  1011,  1016,  1030,  1046,  1047,  1048,  1049,  1052,
    1053,  1059,  1062,  1063,  1067,  1068,  1070,  1075,  1077,  1084,
    1092,  1093,  1097,  1099,  1103,  1104,  1108,  1112,  1113,  1116,
    1118,  1122,  1123,  1128,  1129,  1130,  1131,  1132,  1133,  1134,
    1135,  1136,  1139,  1142,  1145,  1146,  1147,  1148,  1149,  1150,
    1151,  1152,  1153,  1156,  1157,  1158,  1159,  1160,  1161,  1162,
    1163,  1164,  1165,  1166,  1169,  1172,  1177,  1178,  1182,  1183,
    1187,  1191,  1192,  1196,  1200,  1204,  1208,  1214,  1218,  1222,
    1226,  1230,  1237,  1238,  1242,  1246,  1247,  1250,  1251,  1255,
    1257,  1259,  1267,  1268,  1271,  1272,  1276,  1278,  1280,  1288,
    1290,  1297,  1298,  1302,  1303,  1306,  1307,  1311,  1313,  1317,
    1321,  1323,  1330,  1331,  1335,  1336,  1341,  1343,  1347,  1349,
    1353,  1354,  1357,  1358,  1362,  1363,  1365,  1367,  1371,  1372,
    1376,  1377,  1386,  1387,  1396,  1397,  1408,  1411,  1412,  1413,
    1419,  1427,  1434,  1443,  1444,  1447,  1450,  1453,  1456,  1459,
    1462,  1465,  1468,  1471,  1474,  1475,  1476,  1477,  1478,  1479,
    1480,  1481,  1484,  1487,  1488,  1491
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
  "valrangeassign", "valrange2", "valrange3", "expr", "adverb", "exprn",
  "exprseq", "arrayelems", "arrayelems1", "arglist1", "arglistv1",
  "keyarglist1", "keyarg", "optkeyarglist", "mavars", "mavarlist",
  "slotliteral", "blockliteral", "pushname", "pushliteral", "listliteral",
  "block", "funcvardecls", "funcvardecls1", "funcvardecl", "argdecls",
  "argdecls1", "constdeflist", "constdef", "slotdeflist0", "slotdeflist",
  "slotdef", "vardeflist0", "vardeflist", "vardef", "dictslotdef",
  "dictslotlist1", "dictslotlist", "rwslotdeflist", "rwslotdef",
  "dictlit2", "litdictslotdef", "litdictslotlist1", "litdictslotlist",
  "listlit", "listlit2", "literallistc", "literallist1", "rwspec", "rspec",
  "integer", "floatr", "accidental", "pie", "floatp", "name", "classname",
  "primname", "trueobj", "falseobj", "nilobj", "ascii", "symbol", "string",
  "pseudovar", "binop", "keybinop", "binop2", "curryarg", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-345)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-292)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      84,   879,    16,    87,    16,    75,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,  -345,  -345,  -345,    72,    72,  -345,  -345,
    -345,  -345,  -345,    67,  -345,   410,    72,  1607,   230,  1347,
     750,  1607,    72,   144,  -345,  -345,  -345,  -345,  -345,     2,
    -345,  -345,  1773,   102,   115,  -345,  -345,  -345,  -345,  1087,
    -345,  1087,  -345,   151,   151,  -345,  -345,  -345,    59,   208,
    -345,  -345,  -345,  -345,  -345,  -345,  -345,  -345,   147,  -345,
    -345,   135,  -345,    79,  -345,   126,   199,   163,    72,    72,
    -345,  -345,  -345,  -345,  -345,   210,    35,  -345,   222,   807,
    -345,  1607,  1607,  -345,  -345,   205,   200,   201,  1607,  1607,
    1399,  -345,   410,  -345,  -345,  -345,  -345,    29,  -345,   206,
     267,  1087,  1087,  -345,   218,   219,  -345,  1607,   224,  1756,
     228,  1714,   248,    12,  -345,   236,  1451,  -345,  -345,   132,
    -345,   246,  1607,  -345,  -345,  -345,  -345,  -345,  1087,  -345,
    -345,  1607,  1139,    93,  -345,  -345,  -345,  1347,   931,    93,
    -345,    16,    72,   247,  -345,    72,  1607,  1607,    72,  -345,
     272,   260,   275,    50,  1087,    72,  -345,  -345,    72,  -345,
     678,  -345,  -345,  1087,  1607,  -345,  1347,  -345,  -345,  -345,
    1607,   262,    48,  -345,  1607,  1607,  1607,  -345,   264,   271,
    1087,  1347,  -345,  -345,  -345,   109,  -345,  -345,  1607,  1714,
    1680,  -345,  -345,  -345,   277,   282,   151,  -345,  -345,   280,
    -345,  -345,  -345,  -345,  -345,  -345,  1607,    72,    72,  1714,
    1607,  -345,    51,   289,  1607,   983,   324,    26,  1607,  1773,
    -345,  1773,  1607,    93,   296,   304,  -345,   317,    93,   296,
     304,   323,  -345,  1607,   636,  -345,   330,  -345,  -345,  -345,
    1773,   328,   337,    72,  -345,    72,  -345,   343,  -345,     1,
    -345,  1607,    23,  -345,  -345,   151,  -345,  -345,  -345,  -345,
    -345,  -345,  -345,   344,   345,   350,  -345,   366,  1607,  -345,
    -345,  1607,  1607,  -345,  -345,   377,  -345,  -345,   349,   372,
    -345,  1607,  1191,    93,  1773,   356,   388,  -345,   375,   378,
    1714,  -345,  1714,  -345,  1714,  1773,  -345,  -345,   387,   391,
    1503,   411,  1607,  1607,   113,    93,   296,   304,   323,  1607,
    1035,    93,  -345,   437,  1607,  -345,  -345,   402,  -345,    93,
    1243,  -345,   393,   412,   396,  -345,  -345,   401,   404,   412,
     405,  -345,  1747,  -345,  -345,   413,   414,   430,   334,  -345,
    -345,   416,   103,  -345,  -345,    72,   420,  1295,  1295,  -345,
    1607,  -345,  -345,   439,  1607,  -345,    93,   296,   304,  -345,
    1714,  1680,  -345,  -345,  -345,  -345,   418,  -345,   443,   444,
     431,  1607,  -345,   433,   452,  -345,   434,   435,   436,  1773,
      93,   296,   304,   323,   441,  1607,   323,    15,  -345,    93,
    -345,  -345,    93,   450,   456,    67,    67,   457,   263,   263,
     453,  -345,   830,  -345,  -345,    72,   464,  -345,    72,   308,
     459,   458,    13,   461,  -345,  1607,  -345,    93,   462,   463,
    -345,  -345,  -345,  1607,  1607,   488,  1773,  1607,  1607,    93,
    -345,    93,  -345,   469,   473,   474,  -345,  -345,  -345,  1607,
    -345,  -345,  -345,    67,    67,  -345,  -345,  -345,  -345,  -345,
    -345,   303,  -345,    72,   314,  -345,   318,  -345,    72,  -345,
     491,  -345,   496,  1607,  1607,  -345,  1295,  -345,  1607,   503,
    -345,  -345,    93,  -345,  1773,  1773,  1607,   481,  1773,  -345,
    -345,    93,  -345,    93,  1773,  -345,  -345,   189,   189,   334,
    -345,   263,   502,  -345,  -345,   453,   504,  -345,  1607,   458,
     458,  -345,   458,  1607,  -345,  1773,  -345,  -345,  -345,   189,
     189,  -345,  1555,   486,  1555,  1733,  -345,  1658,  -345,  1658,
     458,  -345,  -345,  -345,   458,  1555,  1555,  1607,   493,  -345,
     484,  -345,   495,  -345,  -345,  -345,  -345,  -345,   499,   505,
    1756,  -345,  -345,  -345,  -345,  -345,  -345
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       5,    48,     0,     0,     2,     3,     7,   273,   260,   262,
     264,   281,   282,   280,   275,   295,     0,   222,   279,   277,
     278,   283,   266,   201,   274,     0,   215,     0,   201,   142,
     233,     0,     0,     0,    41,     4,    33,    97,    94,   123,
     106,   124,   139,    27,    48,    93,    95,    92,   172,    48,
     198,    48,   182,   174,   267,   268,   271,   175,   173,   125,
     179,   180,   181,   176,   178,   177,   104,    96,     0,     1,
       6,    14,     8,     0,   224,   226,     0,   223,   222,   215,
     196,   261,   263,   265,   272,     0,    29,   217,    31,   233,
     126,     0,     0,   196,   292,   144,     0,    29,     0,     0,
       0,   284,   288,   286,   287,   289,   290,   215,   285,     0,
       0,    48,    48,   231,    29,     0,   293,   294,     0,    27,
      99,   250,     0,   159,   161,     0,     0,   288,   291,     0,
     294,   135,    28,   141,    34,    40,   199,    39,    48,   270,
     269,     0,     0,    56,    50,    53,    52,   142,     0,    65,
      21,     0,    12,     0,   200,     0,     0,     0,     0,   207,
       0,   223,     0,    29,    48,     0,   209,    30,     0,    32,
       0,    80,    82,    48,     0,   100,    30,   143,   146,   114,
       0,     0,     0,   101,   113,     0,     0,    98,     0,     0,
      48,    30,   234,   103,   230,     0,    28,    49,     0,   250,
     244,   252,   193,   192,     0,    29,   183,   184,   188,     0,
     189,   190,   191,   185,   187,   186,     0,     0,     0,   250,
       0,   150,     0,     0,     0,     0,    54,     0,     0,   140,
      38,   128,     0,     0,    29,    29,    51,     0,    54,    29,
      29,    29,   154,     0,     0,    15,     0,    13,    16,   225,
     227,     0,     0,     0,   202,     0,   204,     0,   197,     0,
     218,     0,     0,   220,   171,   163,   164,   168,   169,   170,
     165,   167,   166,     0,     0,     0,   145,   147,     0,   118,
     102,   119,     0,   115,   229,     0,    37,    36,     0,     0,
     232,     0,     0,    57,   129,     0,     0,   242,    29,     0,
       0,   246,    30,   251,   250,   132,   160,   162,     0,     0,
       0,   105,     0,     0,     0,    54,    29,    29,    29,     0,
       0,    55,    79,     0,     0,   137,   136,   127,   152,    58,
      30,   157,     0,    30,     0,    64,    66,     0,     0,    30,
       0,   156,   289,    11,    22,     0,     0,    14,    21,   228,
     208,     0,     0,   195,   210,     0,     0,     0,     0,   194,
       0,   148,   120,     0,   117,    35,     0,    29,    29,   248,
       0,    30,   245,   239,   241,   253,     0,   247,   108,   107,
       0,     0,   151,     0,     0,    70,     0,     0,     0,   130,
      54,    29,    29,    29,     0,     0,    29,    54,    62,    54,
      69,   155,    54,     0,     0,   201,   201,     0,   254,   254,
     258,    17,     0,   203,   205,     0,     0,   221,     0,     0,
       0,    84,   173,     0,   149,   121,   116,    60,     0,     0,
     240,   243,   249,     0,     0,   109,   133,     0,     0,    54,
      75,    54,    76,     0,     0,     0,   138,   153,   158,     0,
      59,    68,    67,   201,   201,   196,   196,    16,   255,   257,
     256,     0,   235,     0,     0,   259,    29,   212,     0,     9,
       0,   211,     0,     0,     0,    81,     0,    89,     0,     0,
      83,   122,    54,    63,   111,   110,     0,     0,   134,    73,
      71,    54,    78,    54,   131,   196,   196,    44,    44,    21,
      19,   254,   237,    18,    20,   258,     0,   206,     0,    84,
      84,    85,    84,     0,    61,   112,    74,    77,    72,    44,
      44,   276,    46,    27,    46,     0,   236,     0,   213,     0,
      84,    91,    90,    86,    84,    46,    46,     0,     0,    42,
      46,    45,     0,    10,   238,   214,    88,    87,     0,     0,
      27,    23,    43,    25,    24,    26,    47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -345,  -345,  -345,  -345,  -345,   536,  -345,   209,   100,  -345,
    -341,  -345,  -111,   -18,  -345,   -34,  -345,  -302,  -303,    18,
     515,   -49,  -121,    82,  -345,   187,  -345,  -345,  -205,  -344,
    -345,  -345,  -345,  -345,  -345,   -27,  -345,  -345,   185,   415,
    -345,   -99,  -113,  -143,   221,  -119,  -345,  -345,  -287,   197,
    -345,  -345,  -134,  -345,   -69,    -4,    37,   -22,   531,  -345,
      60,   489,   490,   399,   498,    -6,   423,   390,  -345,  -345,
     170,    81,  -345,   214,  -345,  -345,  -169,  -345,  -168,  -345,
    -345,  -345,  -108,  -345,  -345,     0,   -76,   -14,    19,  -345,
     -36,    10,   270,   329,   347,   352,  -345,  -224,   407,     6,
    -345
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     3,     4,     5,    70,     6,   246,   153,   348,   411,
     244,   344,   133,   331,   170,    34,    35,   538,   522,   539,
      36,   321,   144,   322,    37,    38,   273,   274,   477,   420,
      39,    40,    41,   109,   181,    42,   228,    43,    44,    96,
      97,   222,   235,   396,   242,   332,   122,   123,   263,    45,
      46,    47,   201,    48,   164,    49,   258,    80,    51,   466,
     467,    85,    86,    87,    76,    73,    74,   113,   114,   115,
     461,   462,   202,   297,   298,   299,    52,   203,   204,   205,
     463,   468,    53,    54,    55,    56,    57,    58,    59,   523,
      60,    61,    62,    63,    64,    65,    66,   116,   130,   131,
      67
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      90,   264,    75,    75,   119,   241,    93,   412,   197,   143,
     149,    77,    88,   206,   423,   135,     7,   137,   120,   124,
     346,    68,   236,    71,   173,    84,   111,    14,   236,     7,
       8,   295,     7,   217,    14,   240,   118,    23,    50,    23,
     354,   478,    24,   234,   141,   207,   449,   138,   126,   239,
     355,   308,   125,   139,   140,    24,  -216,    28,    24,    28,
     323,   218,   265,   142,    75,    88,   296,    50,   168,   121,
     281,  -216,   161,   310,   166,     7,   324,   188,   189,   177,
    -291,    78,   318,    23,   167,   210,   136,    69,    50,   256,
     141,   206,   206,    88,   266,   118,   192,   282,   311,   167,
     312,    24,    84,    28,   230,   229,    79,   208,   190,   142,
       1,   206,   317,     2,   231,   226,   334,    23,   404,   325,
     337,   338,     2,   207,   207,   314,   316,   154,   155,   250,
     257,   211,   511,    23,   267,     7,   376,    28,   247,   275,
     209,    75,   414,   207,   252,   168,   293,     7,   136,    50,
     132,   259,   415,    28,    88,    14,   288,   156,   525,   292,
     384,    24,   312,   210,   210,   151,   374,    31,   375,   223,
     245,   294,   236,    24,    22,   136,   157,   393,   224,   368,
     268,   152,   225,   210,   329,   208,   208,   303,   346,   305,
     121,   150,   206,   367,   206,   524,   206,   386,   387,   521,
     236,   327,    16,   306,   307,   208,   139,   392,   236,   211,
     211,   159,   155,   326,    95,   110,   535,   536,   209,   209,
     158,   391,   542,   340,   207,  -219,   207,   136,   207,   211,
     345,   165,    23,   548,   549,   174,   430,   296,   209,   351,
     544,   352,   545,  -219,    78,   145,   145,   175,   428,   429,
     176,  -219,    28,   169,   147,   146,   146,   183,   148,   198,
      91,  -219,   206,   206,   210,   139,   210,   191,   210,    79,
     193,  -219,   443,   444,   110,   195,   171,   172,    92,   216,
     372,   125,   219,   178,   179,   182,   208,   227,   208,   184,
     208,   248,   389,   253,   207,   207,   255,   185,   458,   459,
     388,   346,   194,   460,   531,   532,   236,   533,   254,   155,
     211,   221,   211,   280,   211,   286,   186,   427,   187,   209,
     336,   209,   287,   209,   301,   546,   304,   221,   403,   547,
     145,   302,    95,   221,   210,   210,   145,   473,   474,   313,
     146,   416,   251,   422,   422,   330,   146,   408,    23,   409,
     410,   500,   501,   333,   436,   319,   208,   208,   264,   276,
     264,   277,   503,   501,   335,   279,   504,   167,    28,   283,
     284,   285,   339,   143,   320,   445,   289,   347,   448,   349,
     211,   211,   145,   455,   456,   350,   497,   498,   353,   209,
     209,   212,   146,   357,   358,   359,   360,   385,   345,   364,
     365,   470,   185,   369,   472,   309,   484,   485,   479,   221,
     221,   488,   541,   145,    81,    82,    83,   328,   370,   265,
     145,   265,   494,   146,   371,   145,   519,   520,   341,   373,
     146,   495,   496,    22,   377,   146,    98,   117,   378,   556,
     269,    81,   381,   129,   397,    94,   356,   398,   505,   502,
     213,   266,   399,   266,   506,   400,   402,   405,   406,   515,
     151,   425,   422,   361,   413,   432,   362,   363,   214,   212,
     212,   417,   442,   215,   433,   434,   194,   221,   435,   450,
     145,   451,   437,   438,   452,   439,   440,   441,   465,   212,
     146,   267,   446,   267,   453,   380,   117,   382,   383,   270,
     454,   457,   145,   471,   475,   221,   480,   476,   145,   394,
     550,   345,   146,   482,   483,   382,   145,   271,   146,   486,
     491,   489,   272,   490,   492,   493,   146,   508,   213,   213,
     507,   513,   516,   527,   196,   529,   537,   268,   551,   268,
     553,    72,   421,   421,   554,   424,   214,   214,   213,   426,
     555,   215,   215,   145,    98,   243,   407,   499,   552,   134,
     401,   112,   237,   146,   514,   528,   214,   260,   162,   163,
     212,   215,   212,   517,   212,   518,   160,   145,   249,   464,
     447,   290,   526,   278,   145,   431,   145,   146,     0,   145,
       0,     0,     0,     0,   146,     0,   146,     0,   291,   146,
       0,     0,     0,     0,     0,     0,     0,   300,     0,   145,
     481,     0,     0,     0,   145,     0,     0,     0,     0,   146,
       0,     0,   487,     0,   146,     0,   145,     0,   145,   213,
       0,   213,   243,   213,     0,     0,   146,     0,   146,     7,
     212,   212,     0,     0,     0,     0,     0,   214,     0,   214,
       0,   214,   215,     0,   215,     0,   215,     0,   509,   510,
       0,   421,     0,   512,     0,    24,     0,     0,   101,   145,
     127,   103,   104,   342,   106,   128,   108,     0,   145,   146,
     145,   343,     8,     9,    10,    11,    12,    13,   146,     0,
     146,     0,     0,   530,     0,    18,    19,    20,   534,   213,
     213,    22,     0,     0,     0,     0,     0,   540,     0,   540,
       0,     0,    25,     0,     0,     0,     0,   214,   214,     0,
     540,   540,   215,   215,     0,     0,     0,   243,   261,     0,
       0,     0,   262,     0,     0,     0,     0,   243,     0,     0,
     243,     0,     0,     0,     0,     0,   243,     0,     0,     0,
       0,     0,     0,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,    16,    17,     0,     0,    18,    19,    20,
      21,     0,    99,    22,    23,     0,     0,     0,   300,    24,
     100,     0,   101,    94,   102,   103,   104,   105,   106,   107,
     108,     0,    27,     0,    28,     0,    29,   269,     0,   269,
      89,     0,     0,    32,    33,     0,     0,     0,     0,     0,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    18,    19,    20,    21,     0,    99,
      22,    23,     0,     7,     0,     0,    24,   100,     0,   101,
      94,   102,   103,   104,   105,   106,   128,   108,     0,    27,
       0,    28,     0,    29,     0,     0,   270,    89,   270,    24,
      32,    33,   101,     0,   127,   103,   104,   342,   106,   128,
     108,     0,     0,     0,   271,   469,   271,     0,     0,   272,
       0,   272,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,    16,    17,     0,     0,    18,    19,    20,    21,
       0,     0,    22,    23,     0,     0,     0,     0,    24,     0,
       0,     0,     0,    25,     0,     0,     0,     0,    26,     0,
       0,    27,     0,    28,     0,    29,     0,     0,     0,    30,
       0,    31,    32,    33,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    18,    19,
      20,    21,     0,     0,    22,    23,     0,     0,     0,     0,
      24,     0,     0,     0,    94,    25,     0,     0,   232,     0,
       0,     0,     0,    27,     0,    28,     0,    29,     0,     0,
       0,    89,   238,     0,    32,    33,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,     0,     0,     0,     0,
      18,    19,    20,    21,     0,     0,    22,    23,     0,     0,
       0,     0,    24,     0,     0,     0,    94,    25,     0,     0,
     232,     0,     0,     0,     0,    27,     0,    28,     0,    29,
       0,     0,     0,    89,   315,     0,    32,    33,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,     0,     0,
       0,     0,    18,    19,    20,    21,     0,     0,    22,    23,
       0,     0,     0,     0,    24,     0,     0,     0,    94,    25,
       0,     0,   232,     0,     0,     0,     0,    27,     0,    28,
       0,    29,     0,     0,     0,    89,   390,     0,    32,    33,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
      16,     0,     0,     0,    18,    19,    20,    21,     0,     0,
      22,    23,     0,     0,     0,     0,    24,     0,     0,     0,
       0,    25,     0,     0,     0,     0,     0,     0,     0,    27,
       0,    28,     0,    29,     0,     0,     0,    89,     0,    31,
      32,    33,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    18,    19,    20,    21,
       0,     0,    22,    23,     0,     0,     0,     0,    24,     0,
       0,     0,     0,    25,     0,     0,   232,     0,     0,     0,
       0,    27,     0,    28,     0,    29,     0,     0,     0,    89,
     233,     0,    32,    33,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    18,    19,
      20,    21,     0,     0,    22,    23,     0,     0,     0,     0,
      24,     0,     0,     0,     0,    25,     0,     0,   232,     0,
       0,     0,     0,    27,     0,    28,     0,    29,     0,     0,
       0,    89,   366,     0,    32,    33,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,     0,     0,     0,     0,
      18,    19,    20,    21,     0,     0,    22,    23,     0,     0,
       0,     0,    24,     0,     0,     0,    94,    25,     0,     0,
     395,     0,     0,     0,     0,    27,     0,    28,     0,    29,
       0,     0,     0,    89,     0,     0,    32,    33,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,   418,     0,
       0,     0,    18,    19,    20,    21,     0,     0,    22,    23,
       0,     0,     0,     0,    24,   419,     0,     0,     0,    25,
       0,     0,     0,     0,     0,     0,     0,    27,     0,    28,
       0,    29,     0,     0,     0,    89,     0,     0,    32,    33,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    18,    19,    20,    21,     0,     0,
      22,    23,     0,     0,     0,     0,    24,     0,     0,     0,
      94,    25,     0,     0,     0,     0,     0,     0,     0,    27,
       0,    28,     0,    29,     0,     0,     0,    89,     0,     0,
      32,    33,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    18,    19,    20,    21,
       0,   180,    22,    23,     0,     0,     0,     0,    24,     0,
       0,     0,     0,    25,     0,     0,     0,     0,     0,     0,
       0,    27,     0,    28,     0,    29,     0,     0,     0,    89,
       0,     0,    32,    33,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    18,    19,
      20,    21,     0,   220,    22,    23,     0,     0,     0,     0,
      24,     0,     0,     0,     0,    25,     0,     0,     0,     0,
       0,     0,     0,    27,     0,    28,     0,    29,     0,     0,
       0,    89,     0,     0,    32,    33,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,     0,     0,     0,     0,
      18,    19,    20,    21,     0,     0,    22,    23,     0,     0,
       0,     0,    24,     0,     0,     0,     0,    25,     0,     0,
       0,     0,     0,     0,     0,    27,     0,    28,     0,    29,
     379,     0,     0,    89,     0,     0,    32,    33,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,     0,     0,
       0,     0,    18,    19,    20,    21,     0,     0,    22,    23,
       0,     0,     0,     0,    24,     0,     0,     0,     0,    25,
       0,     0,     0,     0,     0,     0,     0,    27,     0,    28,
       0,    29,     0,     0,     0,    89,     0,   537,    32,    33,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    18,    19,    20,    21,     0,     0,
      22,    23,     0,     0,     0,     0,    24,     0,     0,     0,
       0,    25,     0,     0,     0,     0,     0,     0,     0,    27,
       0,    28,     0,    29,     0,     0,     0,    89,     0,     0,
      32,    33,     8,     9,    10,    11,    12,    13,     0,     0,
       0,     0,     0,     0,     0,    18,    19,    20,     0,     0,
       0,    22,     0,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    25,     0,     0,     0,     0,    18,    19,    20,
       0,     0,     0,    22,     0,     0,     0,     0,     0,    24,
       0,     0,   262,    94,    25,     0,     0,     7,     8,     9,
      10,    11,    12,    13,     0,    14,   199,     0,     0,     0,
     200,    18,    19,    20,     0,     0,     7,    22,     0,     0,
       0,     0,     0,    24,     0,     0,     0,     0,    25,     0,
       7,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     199,     0,    24,     0,   200,   101,     0,   127,   103,   104,
     342,   106,   128,   108,     0,     0,    24,     0,   543,   101,
       0,   127,   103,   104,   105,   106,   128,   108,   101,    94,
     127,   103,   104,   105,   106,   128,   108,   129,     0,     0,
       0,     0,     0,     0,   196,   101,    94,   127,   103,   104,
     105,   106,   128,   108,   129
};

static const yytype_int16 yycheck[] =
{
      27,   170,    16,    17,    31,   148,    28,   348,   119,    58,
      59,    17,    26,   121,   358,    49,     3,    51,    32,    33,
     244,     2,   143,     4,    93,    25,    30,    11,   149,     3,
       4,   199,     3,    21,    11,   148,    30,    24,     1,    24,
      39,    28,    29,   142,    31,   121,    31,    51,    46,   148,
      49,   219,    33,    53,    54,    29,    21,    44,    29,    44,
      34,    49,   170,    50,    78,    79,   200,    30,    86,    46,
      22,    21,    78,    22,    39,     3,    50,   111,   112,    97,
      51,    14,   225,    24,    49,   121,    49,     0,    51,    39,
      31,   199,   200,   107,   170,    89,   114,    49,    47,    49,
      49,    29,   102,    44,   138,   132,    39,   121,   112,    50,
      26,   219,   225,    38,   141,   129,   235,    24,   342,   227,
     239,   240,    38,   199,   200,   224,   225,    48,    49,   156,
     164,   121,   476,    24,   170,     3,   304,    44,   152,   173,
     121,   155,    39,   219,   158,   163,   195,     3,   111,   112,
      48,   165,    49,    44,   168,    11,   190,    31,   499,    50,
      47,    29,    49,   199,   200,    30,   300,    52,   302,    37,
     151,   198,   293,    29,    23,   138,    50,   320,    46,   292,
     170,    46,    50,   219,   233,   199,   200,   205,   412,   216,
      46,    44,   300,   292,   302,   498,   304,   316,   317,    10,
     321,   228,    13,   217,   218,   219,   206,   320,   329,   199,
     200,    48,    49,   227,    29,    30,   519,   520,   199,   200,
      21,   320,   524,   241,   300,     3,   302,   190,   304,   219,
     244,    21,    24,   535,   536,    30,   370,   371,   219,   253,
     527,   255,   529,    21,    14,    58,    59,    47,   367,   368,
      49,    29,    44,    31,    46,    58,    59,    51,    50,    31,
      30,    39,   370,   371,   300,   265,   302,    49,   304,    39,
      51,    49,   391,   392,    89,    51,    91,    92,    48,    31,
     298,   262,    46,    98,    99,   100,   300,    41,   302,    22,
     304,    44,   319,    21,   370,   371,    21,    30,    35,    36,
     318,   525,   117,    40,   509,   510,   427,   512,    48,    49,
     300,   126,   302,    51,   304,    51,    49,   366,    51,   300,
     238,   302,    51,   304,    47,   530,    46,   142,   342,   534,
     143,    49,   147,   148,   370,   371,   149,    29,    30,    50,
     143,   355,   157,   357,   358,    49,   149,    13,    24,    15,
      16,    48,    49,    49,   381,    31,   370,   371,   527,   174,
     529,   176,    48,    49,    47,   180,    48,    49,    44,   184,
     185,   186,    49,   422,    50,   393,   191,    47,   396,    51,
     370,   371,   195,   405,   406,    48,   455,   456,    45,   370,
     371,   121,   195,    49,    49,    45,    30,   315,   412,    22,
      51,   415,    30,    47,   418,   220,   433,   434,   422,   224,
     225,   438,   523,   226,     4,     5,     6,   232,    30,   527,
     233,   529,   449,   226,    49,   238,   495,   496,   243,    51,
     233,   453,   454,    23,    47,   238,    29,    30,    47,   550,
     170,     4,    31,    41,    51,    33,   261,    51,   466,   463,
     121,   527,    51,   529,   468,    51,    51,    44,    44,   486,
      30,    22,   476,   278,    48,    47,   281,   282,   121,   199,
     200,    51,   390,   121,    31,    31,   291,   292,    47,   397,
     293,   399,    49,    31,   402,    51,    51,    51,    35,   219,
     293,   527,    51,   529,    44,   310,    89,   312,   313,   170,
      44,    44,   315,    39,    45,   320,    45,    49,   321,   324,
     537,   525,   315,    51,    51,   330,   329,   170,   321,    31,
      51,   439,   170,   441,    51,    51,   329,    31,   199,   200,
      39,    28,    51,    31,    48,    31,    52,   527,    45,   529,
      45,     5,   357,   358,    45,   360,   199,   200,   219,   364,
      45,   199,   200,   366,   147,   148,   347,   457,   540,    44,
     339,    30,   147,   366,   482,   505,   219,   168,    79,    79,
     300,   219,   302,   491,   304,   493,    78,   390,   155,   409,
     395,   191,   501,   176,   397,   371,   399,   390,    -1,   402,
      -1,    -1,    -1,    -1,   397,    -1,   399,    -1,   191,   402,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   200,    -1,   422,
     425,    -1,    -1,    -1,   427,    -1,    -1,    -1,    -1,   422,
      -1,    -1,   437,    -1,   427,    -1,   439,    -1,   441,   300,
      -1,   302,   225,   304,    -1,    -1,   439,    -1,   441,     3,
     370,   371,    -1,    -1,    -1,    -1,    -1,   300,    -1,   302,
      -1,   304,   300,    -1,   302,    -1,   304,    -1,   473,   474,
      -1,   476,    -1,   478,    -1,    29,    -1,    -1,    32,   482,
      34,    35,    36,    37,    38,    39,    40,    -1,   491,   482,
     493,    45,     4,     5,     6,     7,     8,     9,   491,    -1,
     493,    -1,    -1,   508,    -1,    17,    18,    19,   513,   370,
     371,    23,    -1,    -1,    -1,    -1,    -1,   522,    -1,   524,
      -1,    -1,    34,    -1,    -1,    -1,    -1,   370,   371,    -1,
     535,   536,   370,   371,    -1,    -1,    -1,   320,    50,    -1,
      -1,    -1,    54,    -1,    -1,    -1,    -1,   330,    -1,    -1,
     333,    -1,    -1,    -1,    -1,    -1,   339,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    13,    14,    -1,    -1,    17,    18,    19,
      20,    -1,    22,    23,    24,    -1,    -1,    -1,   371,    29,
      30,    -1,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    42,    -1,    44,    -1,    46,   527,    -1,   529,
      50,    -1,    -1,    53,    54,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    22,
      23,    24,    -1,     3,    -1,    -1,    29,    30,    -1,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    42,
      -1,    44,    -1,    46,    -1,    -1,   527,    50,   529,    29,
      53,    54,    32,    -1,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    -1,   527,    45,   529,    -1,    -1,   527,
      -1,   529,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    13,    14,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,
      -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,    39,    -1,
      -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,
      -1,    52,    53,    54,     3,     4,     5,     6,     7,     8,
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
       5,     6,     7,     8,     9,    -1,    11,    12,    -1,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,    24,
      -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,    33,    34,
      -1,    -1,    37,    -1,    -1,    -1,    -1,    42,    -1,    44,
      -1,    46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,
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
      17,    18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    -1,    -1,    34,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,
      47,    -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    -1,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,    24,
      -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    34,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,
      -1,    46,    -1,    -1,    -1,    50,    -1,    52,    53,    54,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,
      23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,
      -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,
      53,    54,     4,     5,     6,     7,     8,     9,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    17,    18,    19,    -1,    -1,
      -1,    23,    -1,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    34,    -1,    -1,    -1,    -1,    17,    18,    19,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,    29,
      -1,    -1,    54,    33,    34,    -1,    -1,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    46,    -1,    -1,    -1,
      50,    17,    18,    19,    -1,    -1,     3,    23,    -1,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    34,    -1,
       3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    29,    -1,    50,    32,    -1,    34,    35,    36,
      37,    38,    39,    40,    -1,    -1,    29,    -1,    45,    32,
      -1,    34,    35,    36,    37,    38,    39,    40,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    -1,    -1,
      -1,    -1,    -1,    -1,    48,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    26,    38,    56,    57,    58,    60,     3,     4,     5,
       6,     7,     8,     9,    11,    12,    13,    14,    17,    18,
      19,    20,    23,    24,    29,    34,    39,    42,    44,    46,
      50,    52,    53,    54,    70,    71,    75,    79,    80,    85,
      86,    87,    90,    92,    93,   104,   105,   106,   108,   110,
     111,   113,   131,   137,   138,   139,   140,   141,   142,   143,
     145,   146,   147,   148,   149,   150,   151,   155,   143,     0,
      59,   143,    60,   120,   121,   142,   119,   120,    14,    39,
     112,     4,     5,     6,   140,   116,   117,   118,   142,    50,
      90,    30,    48,   112,    33,    93,    94,    95,   153,    22,
      30,    32,    34,    35,    36,    37,    38,    39,    40,    88,
      93,   110,   113,   122,   123,   124,   152,   153,   154,    90,
     142,    46,   101,   102,   142,   143,    46,    34,    39,    41,
     153,   154,    48,    67,    75,    70,   111,    70,   110,   140,
     140,    31,    50,    76,    77,    80,   104,    46,    50,    76,
      44,    30,    46,    62,    48,    49,    31,    50,    21,    48,
     119,   120,   116,   117,   109,    21,    39,    49,    68,    31,
      69,    93,    93,   109,    30,    47,    49,    68,    93,    93,
      22,    89,    93,    51,    22,    30,    49,    51,    70,    70,
     110,    49,    68,    51,    93,    51,    48,    67,    31,    46,
      50,   107,   127,   132,   133,   134,   137,   141,   142,   143,
     145,   146,   147,   148,   149,   150,    31,    21,    49,    46,
      22,    93,    96,    37,    46,    50,   142,    41,    91,    90,
      70,    90,    37,    51,    96,    97,    77,    94,    51,    96,
      97,    98,    99,   153,    65,   143,    61,   142,    44,   121,
      90,    93,   142,    21,    48,    21,    39,    70,   111,   142,
     118,    50,    54,   103,   131,   137,   141,   145,   146,   147,
     148,   149,   150,    81,    82,    70,    93,    93,   153,    93,
      51,    22,    49,    93,    93,    93,    51,    51,    70,    93,
     122,   153,    50,    76,    90,   133,   107,   128,   129,   130,
     153,    47,    49,    68,    46,    90,   142,   142,   133,    93,
      22,    47,    49,    50,    96,    51,    96,    97,    98,    31,
      50,    76,    78,    34,    50,   137,   142,    90,    93,    76,
      49,    68,   100,    49,   100,    47,    78,   100,   100,    49,
      68,    93,    37,    45,    66,   142,   152,    47,    63,    51,
      48,   142,   142,    45,    39,    49,    93,    49,    49,    45,
      30,    93,    93,    93,    22,    51,    51,    96,    97,    47,
      30,    49,    68,    51,   107,   107,   133,    47,    47,    47,
      93,    31,    93,    93,    47,    78,   100,   100,    68,    90,
      51,    96,    97,    98,    93,    37,    98,    51,    51,    51,
      51,    99,    51,   142,   152,    44,    44,    62,    13,    15,
      16,    64,    65,    48,    39,    49,   142,    51,    13,    30,
      84,    93,   142,    84,    93,    22,    93,    76,   100,   100,
     107,   128,    47,    31,    31,    47,    90,    49,    31,    51,
      51,    51,    78,   100,   100,    68,    51,    93,    68,    31,
      78,    78,    78,    44,    44,   112,   112,    44,    35,    36,
      40,   125,   126,   135,   125,    35,   114,   115,   136,    45,
     142,    39,   142,    29,    30,    45,    49,    83,    28,   142,
      45,    93,    51,    51,    90,    90,    31,    93,    90,    78,
      78,    51,    51,    51,    90,   112,   112,   109,   109,    63,
      48,    49,   142,    48,    48,    68,   142,    39,    31,    93,
      93,    84,    93,    28,    78,    90,    51,    78,    78,   109,
     109,    10,    73,   144,    73,    65,   126,    31,   115,    31,
      93,    83,    83,    83,    93,    73,    73,    52,    72,    74,
      93,    67,    72,    45,   103,   103,    83,    83,    72,    72,
      90,    45,    74,    45,    45,    45,    67
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
      79,    79,    79,    79,    79,    79,    79,    79,    79,    79,
      81,    80,    82,    80,    83,    83,    84,    84,    84,    84,
      84,    84,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    86,    86,    86,
      87,    87,    87,    88,    88,    88,    88,    88,    89,    89,
      89,    89,    89,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    91,    91,    91,    91,    92,
      92,    93,    94,    94,    95,    95,    95,    95,    95,    95,
      96,    96,    97,    97,    98,    98,    99,   100,   100,   101,
     101,   102,   102,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   104,   105,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   108,   108,   109,   109,   110,   110,
     111,   112,   112,   112,   112,   112,   112,   113,   113,   113,
     113,   113,   114,   114,   115,   116,   116,   117,   117,   118,
     118,   118,   119,   119,   120,   120,   121,   121,   121,   122,
     122,   123,   123,   124,   124,   125,   125,   126,   126,   127,
     128,   128,   129,   129,   130,   130,   131,   131,   132,   132,
     133,   133,   134,   134,   135,   135,   135,   135,   136,   136,
     137,   137,   138,   138,   139,   139,   140,   141,   141,   141,
     141,   141,   141,   142,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   152,   152,   152,   152,   152,
     152,   152,   153,   154,   154,   155
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
       5,     7,     8,     7,     8,     6,     6,     8,     7,     4,
       0,     7,     0,     7,     0,     2,     4,     5,     5,     2,
       4,     4,     1,     1,     1,     1,     1,     1,     3,     2,
       3,     3,     4,     3,     1,     4,     1,     5,     5,     6,
       7,     7,     8,     2,     2,     3,     5,     4,     2,     2,
       3,     4,     5,     1,     1,     1,     2,     4,     3,     4,
       5,     7,     4,     6,     7,     0,     2,     2,     4,     1,
       3,     2,     0,     2,     1,     3,     2,     3,     4,     5,
       1,     3,     2,     4,     1,     3,     2,     1,     3,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     5,     5,     0,     2,     1,     2,
       3,     0,     3,     5,     3,     5,     7,     3,     5,     3,
       5,     7,     1,     3,     4,     0,     1,     1,     3,     1,
       3,     5,     0,     1,     1,     3,     1,     3,     4,     3,
       2,     1,     3,     0,     2,     1,     3,     2,     4,     3,
       3,     2,     1,     3,     0,     2,     4,     5,     3,     4,
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
#line 2152 "lang11d_tab.cpp"
    break;

  case 3: /* root: classextensions  */
#line 42 "lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; }
#line 2158 "lang11d_tab.cpp"
    break;

  case 4: /* root: INTERPRET cmdlinecode  */
#line 44 "lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 2; }
#line 2164 "lang11d_tab.cpp"
    break;

  case 5: /* classes: %empty  */
#line 47 "lang11d"
          { yyval = 0; }
#line 2170 "lang11d_tab.cpp"
    break;

  case 6: /* classes: classes classdef  */
#line 49 "lang11d"
                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2176 "lang11d_tab.cpp"
    break;

  case 8: /* classextensions: classextensions classextension  */
#line 54 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2182 "lang11d_tab.cpp"
    break;

  case 9: /* classdef: classname superclass '{' classvardecls methods '}'  */
#line 58 "lang11d"
                                { yyval = (intptr_t)newPyrClassNode((PyrSlotNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1], 0);
				}
#line 2190 "lang11d_tab.cpp"
    break;

  case 10: /* classdef: classname '[' optname ']' superclass '{' classvardecls methods '}'  */
#line 62 "lang11d"
                                { yyval = (intptr_t)newPyrClassNode((PyrSlotNode*)yyvsp[-8], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1],
					(PyrSlotNode*)yyvsp[-6]);
				}
#line 2199 "lang11d_tab.cpp"
    break;

  case 11: /* classextension: '+' classname '{' methods '}'  */
#line 69 "lang11d"
                                {
					yyval = (intptr_t)newPyrClassExtNode((PyrSlotNode*)yyvsp[-3], (PyrMethodNode*)yyvsp[-1]);
				}
#line 2207 "lang11d_tab.cpp"
    break;

  case 12: /* optname: %empty  */
#line 74 "lang11d"
                  { yyval = 0; }
#line 2213 "lang11d_tab.cpp"
    break;

  case 14: /* superclass: %empty  */
#line 78 "lang11d"
                  { yyval = 0; }
#line 2219 "lang11d_tab.cpp"
    break;

  case 15: /* superclass: ':' classname  */
#line 80 "lang11d"
                                { yyval = yyvsp[0]; }
#line 2225 "lang11d_tab.cpp"
    break;

  case 16: /* classvardecls: %empty  */
#line 83 "lang11d"
                  { yyval = 0; }
#line 2231 "lang11d_tab.cpp"
    break;

  case 17: /* classvardecls: classvardecls classvardecl  */
#line 85 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2237 "lang11d_tab.cpp"
    break;

  case 18: /* classvardecl: CLASSVAR rwslotdeflist ';'  */
#line 89 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varClass); }
#line 2243 "lang11d_tab.cpp"
    break;

  case 19: /* classvardecl: VAR rwslotdeflist ';'  */
#line 91 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varInst); }
#line 2249 "lang11d_tab.cpp"
    break;

  case 20: /* classvardecl: SC_CONST constdeflist ';'  */
#line 93 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varConst); }
#line 2255 "lang11d_tab.cpp"
    break;

  case 21: /* methods: %empty  */
#line 96 "lang11d"
                  { yyval = 0; }
#line 2261 "lang11d_tab.cpp"
    break;

  case 22: /* methods: methods methoddef  */
#line 98 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2267 "lang11d_tab.cpp"
    break;

  case 23: /* methoddef: name '{' argdecls funcvardecls primitive methbody '}'  */
#line 102 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2274 "lang11d_tab.cpp"
    break;

  case 24: /* methoddef: '*' name '{' argdecls funcvardecls primitive methbody '}'  */
#line 105 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2281 "lang11d_tab.cpp"
    break;

  case 25: /* methoddef: binop '{' argdecls funcvardecls primitive methbody '}'  */
#line 108 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2288 "lang11d_tab.cpp"
    break;

  case 26: /* methoddef: '*' binop '{' argdecls funcvardecls primitive methbody '}'  */
#line 111 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2295 "lang11d_tab.cpp"
    break;

  case 34: /* funcbody: exprseq funretval  */
#line 129 "lang11d"
                                { yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2301 "lang11d_tab.cpp"
    break;

  case 35: /* cmdlinecode: '(' argdecls1 funcvardecls1 funcbody ')'  */
#line 133 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2307 "lang11d_tab.cpp"
    break;

  case 36: /* cmdlinecode: '(' argdecls1 funcbody ')'  */
#line 135 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-2], NULL, (PyrParseNode*)yyvsp[-1], false); }
#line 2313 "lang11d_tab.cpp"
    break;

  case 37: /* cmdlinecode: '(' funcvardecls1 funcbody ')'  */
#line 137 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2319 "lang11d_tab.cpp"
    break;

  case 38: /* cmdlinecode: argdecls1 funcvardecls1 funcbody  */
#line 139 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-2], (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2325 "lang11d_tab.cpp"
    break;

  case 39: /* cmdlinecode: argdecls1 funcbody  */
#line 141 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-1], NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2331 "lang11d_tab.cpp"
    break;

  case 40: /* cmdlinecode: funcvardecls1 funcbody  */
#line 143 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2337 "lang11d_tab.cpp"
    break;

  case 41: /* cmdlinecode: funcbody  */
#line 145 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2343 "lang11d_tab.cpp"
    break;

  case 43: /* methbody: exprseq retval  */
#line 150 "lang11d"
                                { yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2349 "lang11d_tab.cpp"
    break;

  case 44: /* primitive: %empty  */
#line 153 "lang11d"
                  { yyval = 0; }
#line 2355 "lang11d_tab.cpp"
    break;

  case 45: /* primitive: primname optsemi  */
#line 155 "lang11d"
                                { yyval = yyvsp[-1]; }
#line 2361 "lang11d_tab.cpp"
    break;

  case 46: /* retval: %empty  */
#line 159 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode(NULL); }
#line 2367 "lang11d_tab.cpp"
    break;

  case 47: /* retval: '^' expr optsemi  */
#line 161 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); }
#line 2373 "lang11d_tab.cpp"
    break;

  case 48: /* funretval: %empty  */
#line 165 "lang11d"
                        { yyval = (intptr_t)newPyrBlockReturnNode(); }
#line 2379 "lang11d_tab.cpp"
    break;

  case 49: /* funretval: '^' expr optsemi  */
#line 167 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); }
#line 2385 "lang11d_tab.cpp"
    break;

  case 51: /* blocklist1: blocklist1 blocklistitem  */
#line 172 "lang11d"
                                {
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
				}
#line 2393 "lang11d_tab.cpp"
    break;

  case 54: /* blocklist: %empty  */
#line 181 "lang11d"
                        { yyval = 0; }
#line 2399 "lang11d_tab.cpp"
    break;

  case 56: /* msgsend: name blocklist1  */
#line 186 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2407 "lang11d_tab.cpp"
    break;

  case 57: /* msgsend: '(' binop2 ')' blocklist1  */
#line 190 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2415 "lang11d_tab.cpp"
    break;

  case 58: /* msgsend: name '(' ')' blocklist1  */
#line 194 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-3], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2423 "lang11d_tab.cpp"
    break;

  case 59: /* msgsend: name '(' arglist1 optkeyarglist ')' blocklist  */
#line 198 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2432 "lang11d_tab.cpp"
    break;

  case 60: /* msgsend: '(' binop2 ')' '(' ')' blocklist1  */
#line 203 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-4], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2440 "lang11d_tab.cpp"
    break;

  case 61: /* msgsend: '(' binop2 ')' '(' arglist1 optkeyarglist ')' blocklist  */
#line 207 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2449 "lang11d_tab.cpp"
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
#line 2471 "lang11d_tab.cpp"
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
#line 2488 "lang11d_tab.cpp"
    break;

  case 64: /* msgsend: classname '[' arrayelems ']'  */
#line 243 "lang11d"
                        { yyval = (intptr_t)newPyrDynListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 2494 "lang11d_tab.cpp"
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
#line 2509 "lang11d_tab.cpp"
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
#line 2524 "lang11d_tab.cpp"
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
#line 2539 "lang11d_tab.cpp"
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
#line 2556 "lang11d_tab.cpp"
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
#line 2581 "lang11d_tab.cpp"
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
#line 2594 "lang11d_tab.cpp"
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
#line 2607 "lang11d_tab.cpp"
    break;

  case 72: /* msgsend: expr '.' name '(' keyarglist1 optcomma ')' blocklist  */
#line 330 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-7],
					(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2616 "lang11d_tab.cpp"
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
#line 2633 "lang11d_tab.cpp"
    break;

  case 74: /* msgsend: expr '.' '*' '(' exprseq ',' exprseq ')'  */
#line 349 "lang11d"
                    {
		        PyrSlot slot, slot2;
		        SetSymbol(&slot, s_performArgs);
		        SetSymbol(&slot2, s_value);
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);
				PyrParseNode* args = linkNextNode(
					(PyrParseNode*)yyvsp[-7],
					newPyrPushLitNode(newPyrSlotNode(&slot2), NULL)
                );
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-3]);
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, NULL, 0);
            }
#line 2651 "lang11d_tab.cpp"
    break;

  case 75: /* msgsend: expr '.' '(' arglistv1 optkeyarglist ')'  */
#line 364 "lang11d"
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
#line 2675 "lang11d_tab.cpp"
    break;

  case 76: /* msgsend: expr '.' name '(' ')' blocklist  */
#line 386 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-5], NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2683 "lang11d_tab.cpp"
    break;

  case 77: /* msgsend: expr '.' name '(' arglist1 optkeyarglist ')' blocklist  */
#line 390 "lang11d"
                        {
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2693 "lang11d_tab.cpp"
    break;

  case 78: /* msgsend: expr '.' name '(' arglistv1 optkeyarglist ')'  */
#line 396 "lang11d"
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
#line 2715 "lang11d_tab.cpp"
    break;

  case 79: /* msgsend: expr '.' name blocklist  */
#line 414 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[-3], 0, (PyrParseNode*)yyvsp[0]);
			}
#line 2723 "lang11d_tab.cpp"
    break;

  case 80: /* $@1: %empty  */
#line 419 "lang11d"
                            { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 1); }
#line 2729 "lang11d_tab.cpp"
    break;

  case 81: /* generator: '{' ':' exprseq $@1 ',' qual '}'  */
#line 420 "lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, getsym("r"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);

				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)yyvsp[-1], false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				yyval = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)blocklit, 0, 0);
			}
#line 2743 "lang11d_tab.cpp"
    break;

  case 82: /* $@2: %empty  */
#line 429 "lang11d"
                                  { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 2); }
#line 2749 "lang11d_tab.cpp"
    break;

  case 83: /* generator: '{' ';' exprseq $@2 ',' qual '}'  */
#line 430 "lang11d"
                        {
				yyval = yyvsp[-1];
			}
#line 2757 "lang11d_tab.cpp"
    break;

  case 84: /* nextqual: %empty  */
#line 436 "lang11d"
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
#line 2783 "lang11d_tab.cpp"
    break;

  case 85: /* nextqual: ',' qual  */
#line 458 "lang11d"
                                { yyval = yyvsp[0]; }
#line 2789 "lang11d_tab.cpp"
    break;

  case 86: /* qual: name LEFTARROW exprseq nextqual  */
#line 462 "lang11d"
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
#line 2827 "lang11d_tab.cpp"
    break;

  case 87: /* qual: name name LEFTARROW exprseq nextqual  */
#line 496 "lang11d"
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
#line 2869 "lang11d_tab.cpp"
    break;

  case 88: /* qual: VAR name '=' exprseq nextqual  */
#line 534 "lang11d"
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
#line 2887 "lang11d_tab.cpp"
    break;

  case 89: /* qual: exprseq nextqual  */
#line 548 "lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, getsym("if"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)yyvsp[0], false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-1], blocklit);

				yyval = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
			}
#line 2902 "lang11d_tab.cpp"
    break;

  case 90: /* qual: ':' ':' exprseq nextqual  */
#line 559 "lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
			}
#line 2910 "lang11d_tab.cpp"
    break;

  case 91: /* qual: ':' WHILE exprseq nextqual  */
#line 563 "lang11d"
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
#line 2937 "lang11d_tab.cpp"
    break;

  case 98: /* expr1: '(' exprseq ')'  */
#line 594 "lang11d"
                        {
				PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
				node->mParens = 1;
				yyval = yyvsp[-1];
			}
#line 2947 "lang11d_tab.cpp"
    break;

  case 99: /* expr1: '~' name  */
#line 600 "lang11d"
                        {
				PyrParseNode* argnode;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL);
				SetSymbol(&slot, s_envirGet);
				selectornode = newPyrSlotNode(&slot);
				yyval = (intptr_t)newPyrCallNode(selectornode, argnode, 0, 0);
			}
#line 2961 "lang11d_tab.cpp"
    break;

  case 100: /* expr1: '[' arrayelems ']'  */
#line 610 "lang11d"
                        { yyval = (intptr_t)newPyrDynListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 2967 "lang11d_tab.cpp"
    break;

  case 101: /* expr1: '(' valrange2 ')'  */
#line 612 "lang11d"
                        { yyval = yyvsp[-1]; }
#line 2973 "lang11d_tab.cpp"
    break;

  case 102: /* expr1: '(' ':' valrange3 ')'  */
#line 614 "lang11d"
                        { yyval = yyvsp[-1]; }
#line 2979 "lang11d_tab.cpp"
    break;

  case 103: /* expr1: '(' dictslotlist ')'  */
#line 616 "lang11d"
                        { yyval = (intptr_t)newPyrDynDictNode((PyrParseNode*)yyvsp[-1]); }
#line 2985 "lang11d_tab.cpp"
    break;

  case 104: /* expr1: pseudovar  */
#line 618 "lang11d"
                        { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 2991 "lang11d_tab.cpp"
    break;

  case 105: /* expr1: expr1 '[' arglist1 ']'  */
#line 620 "lang11d"
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
#line 3008 "lang11d_tab.cpp"
    break;

  case 107: /* valrangex1: expr1 '[' arglist1 DOTDOT ']'  */
#line 636 "lang11d"
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
#line 3039 "lang11d_tab.cpp"
    break;

  case 108: /* valrangex1: expr1 '[' DOTDOT exprseq ']'  */
#line 663 "lang11d"
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
#line 3061 "lang11d_tab.cpp"
    break;

  case 109: /* valrangex1: expr1 '[' arglist1 DOTDOT exprseq ']'  */
#line 681 "lang11d"
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
#line 3090 "lang11d_tab.cpp"
    break;

  case 110: /* valrangeassign: expr1 '[' arglist1 DOTDOT ']' '=' expr  */
#line 708 "lang11d"
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
#line 3122 "lang11d_tab.cpp"
    break;

  case 111: /* valrangeassign: expr1 '[' DOTDOT exprseq ']' '=' expr  */
#line 736 "lang11d"
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
#line 3145 "lang11d_tab.cpp"
    break;

  case 112: /* valrangeassign: expr1 '[' arglist1 DOTDOT exprseq ']' '=' expr  */
#line 755 "lang11d"
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
#line 3175 "lang11d_tab.cpp"
    break;

  case 113: /* valrange2: exprseq DOTDOT  */
#line 784 "lang11d"
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
#line 3197 "lang11d_tab.cpp"
    break;

  case 114: /* valrange2: DOTDOT exprseq  */
#line 803 "lang11d"
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
#line 3219 "lang11d_tab.cpp"
    break;

  case 115: /* valrange2: exprseq DOTDOT exprseq  */
#line 822 "lang11d"
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
#line 3239 "lang11d_tab.cpp"
    break;

  case 116: /* valrange2: exprseq ',' exprseq DOTDOT exprseq  */
#line 839 "lang11d"
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
#line 3257 "lang11d_tab.cpp"
    break;

  case 117: /* valrange2: exprseq ',' exprseq DOTDOT  */
#line 853 "lang11d"
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
#line 3280 "lang11d_tab.cpp"
    break;

  case 118: /* valrange3: DOTDOT exprseq  */
#line 874 "lang11d"
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
#line 3302 "lang11d_tab.cpp"
    break;

  case 119: /* valrange3: exprseq DOTDOT  */
#line 893 "lang11d"
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
#line 3323 "lang11d_tab.cpp"
    break;

  case 120: /* valrange3: exprseq DOTDOT exprseq  */
#line 911 "lang11d"
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
#line 3343 "lang11d_tab.cpp"
    break;

  case 121: /* valrange3: exprseq ',' exprseq DOTDOT  */
#line 928 "lang11d"
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
#line 3363 "lang11d_tab.cpp"
    break;

  case 122: /* valrange3: exprseq ',' exprseq DOTDOT exprseq  */
#line 944 "lang11d"
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
#line 3381 "lang11d_tab.cpp"
    break;

  case 125: /* expr: classname  */
#line 961 "lang11d"
                            { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3387 "lang11d_tab.cpp"
    break;

  case 126: /* expr: '`' expr  */
#line 963 "lang11d"
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
#line 3405 "lang11d_tab.cpp"
    break;

  case 127: /* expr: expr binop2 adverb expr  */
#line 977 "lang11d"
                        {
				yyval = (intptr_t)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0], (PyrParseNode*)yyvsp[-1]);
			}
#line 3414 "lang11d_tab.cpp"
    break;

  case 128: /* expr: name '=' expr  */
#line 982 "lang11d"
                        {
				yyval = (intptr_t)newPyrAssignNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0);
			}
#line 3422 "lang11d_tab.cpp"
    break;

  case 129: /* expr: '~' name '=' expr  */
#line 986 "lang11d"
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
#line 3437 "lang11d_tab.cpp"
    break;

  case 130: /* expr: expr '.' name '=' expr  */
#line 997 "lang11d"
                        {
				yyval = (intptr_t)newPyrSetterNode((PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3446 "lang11d_tab.cpp"
    break;

  case 131: /* expr: name '(' arglist1 optkeyarglist ')' '=' expr  */
#line 1002 "lang11d"
                        {
				if (yyvsp[-3] != 0) {
					error("Setter method called with keyword arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-3]);
					compileErrors++;
				}
				yyval = (intptr_t)newPyrSetterNode((PyrSlotNode*)yyvsp[-6],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3460 "lang11d_tab.cpp"
    break;

  case 132: /* expr: '#' mavars '=' expr  */
#line 1012 "lang11d"
                        {
				yyval = (intptr_t)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[0], 0);
			}
#line 3469 "lang11d_tab.cpp"
    break;

  case 133: /* expr: expr1 '[' arglist1 ']' '=' expr  */
#line 1017 "lang11d"
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
#line 3487 "lang11d_tab.cpp"
    break;

  case 134: /* expr: expr '.' '[' arglist1 ']' '=' expr  */
#line 1031 "lang11d"
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
#line 3505 "lang11d_tab.cpp"
    break;

  case 135: /* adverb: %empty  */
#line 1046 "lang11d"
          { yyval = 0; }
#line 3511 "lang11d_tab.cpp"
    break;

  case 136: /* adverb: '.' name  */
#line 1047 "lang11d"
                           { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3517 "lang11d_tab.cpp"
    break;

  case 137: /* adverb: '.' integer  */
#line 1048 "lang11d"
                              { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3523 "lang11d_tab.cpp"
    break;

  case 138: /* adverb: '.' '(' exprseq ')'  */
#line 1049 "lang11d"
                                      { yyval = yyvsp[-1]; }
#line 3529 "lang11d_tab.cpp"
    break;

  case 140: /* exprn: exprn ';' expr  */
#line 1054 "lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 3537 "lang11d_tab.cpp"
    break;

  case 142: /* arrayelems: %empty  */
#line 1062 "lang11d"
                  { yyval = 0; }
#line 3543 "lang11d_tab.cpp"
    break;

  case 143: /* arrayelems: arrayelems1 optcomma  */
#line 1064 "lang11d"
                          { yyval = yyvsp[-1]; }
#line 3549 "lang11d_tab.cpp"
    break;

  case 145: /* arrayelems1: exprseq ':' exprseq  */
#line 1069 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3555 "lang11d_tab.cpp"
    break;

  case 146: /* arrayelems1: keybinop exprseq  */
#line 1071 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 3564 "lang11d_tab.cpp"
    break;

  case 147: /* arrayelems1: arrayelems1 ',' exprseq  */
#line 1076 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3570 "lang11d_tab.cpp"
    break;

  case 148: /* arrayelems1: arrayelems1 ',' keybinop exprseq  */
#line 1078 "lang11d"
                                {
					PyrParseNode* elems;
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					elems = (PyrParseNode*)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], elems);
				}
#line 3581 "lang11d_tab.cpp"
    break;

  case 149: /* arrayelems1: arrayelems1 ',' exprseq ':' exprseq  */
#line 1085 "lang11d"
                                {
					PyrParseNode* elems;
					elems = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-4], elems);
				}
#line 3591 "lang11d_tab.cpp"
    break;

  case 151: /* arglist1: arglist1 ',' exprseq  */
#line 1094 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3597 "lang11d_tab.cpp"
    break;

  case 152: /* arglistv1: '*' exprseq  */
#line 1098 "lang11d"
                                { yyval = yyvsp[0]; }
#line 3603 "lang11d_tab.cpp"
    break;

  case 153: /* arglistv1: arglist1 ',' '*' exprseq  */
#line 1100 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]); }
#line 3609 "lang11d_tab.cpp"
    break;

  case 155: /* keyarglist1: keyarglist1 ',' keyarg  */
#line 1105 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3615 "lang11d_tab.cpp"
    break;

  case 156: /* keyarg: keybinop exprseq  */
#line 1109 "lang11d"
                                { yyval = (intptr_t)newPyrPushKeyArgNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 3621 "lang11d_tab.cpp"
    break;

  case 157: /* optkeyarglist: optcomma  */
#line 1112 "lang11d"
                           { yyval = 0; }
#line 3627 "lang11d_tab.cpp"
    break;

  case 158: /* optkeyarglist: ',' keyarglist1 optcomma  */
#line 1113 "lang11d"
                                                           { yyval = yyvsp[-1]; }
#line 3633 "lang11d_tab.cpp"
    break;

  case 159: /* mavars: mavarlist  */
#line 1117 "lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3639 "lang11d_tab.cpp"
    break;

  case 160: /* mavars: mavarlist ELLIPSIS name  */
#line 1119 "lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); }
#line 3645 "lang11d_tab.cpp"
    break;

  case 162: /* mavarlist: mavarlist ',' name  */
#line 1124 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3651 "lang11d_tab.cpp"
    break;

  case 163: /* slotliteral: integer  */
#line 1128 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3657 "lang11d_tab.cpp"
    break;

  case 164: /* slotliteral: floatp  */
#line 1129 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3663 "lang11d_tab.cpp"
    break;

  case 165: /* slotliteral: ascii  */
#line 1130 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3669 "lang11d_tab.cpp"
    break;

  case 166: /* slotliteral: string  */
#line 1131 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3675 "lang11d_tab.cpp"
    break;

  case 167: /* slotliteral: symbol  */
#line 1132 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3681 "lang11d_tab.cpp"
    break;

  case 168: /* slotliteral: trueobj  */
#line 1133 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3687 "lang11d_tab.cpp"
    break;

  case 169: /* slotliteral: falseobj  */
#line 1134 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3693 "lang11d_tab.cpp"
    break;

  case 170: /* slotliteral: nilobj  */
#line 1135 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3699 "lang11d_tab.cpp"
    break;

  case 171: /* slotliteral: listlit  */
#line 1136 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3705 "lang11d_tab.cpp"
    break;

  case 172: /* blockliteral: block  */
#line 1139 "lang11d"
                        { yyval = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3711 "lang11d_tab.cpp"
    break;

  case 173: /* pushname: name  */
#line 1142 "lang11d"
                                { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3717 "lang11d_tab.cpp"
    break;

  case 174: /* pushliteral: integer  */
#line 1145 "lang11d"
                                { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3723 "lang11d_tab.cpp"
    break;

  case 175: /* pushliteral: floatp  */
#line 1146 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3729 "lang11d_tab.cpp"
    break;

  case 176: /* pushliteral: ascii  */
#line 1147 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3735 "lang11d_tab.cpp"
    break;

  case 177: /* pushliteral: string  */
#line 1148 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3741 "lang11d_tab.cpp"
    break;

  case 178: /* pushliteral: symbol  */
#line 1149 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3747 "lang11d_tab.cpp"
    break;

  case 179: /* pushliteral: trueobj  */
#line 1150 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3753 "lang11d_tab.cpp"
    break;

  case 180: /* pushliteral: falseobj  */
#line 1151 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3759 "lang11d_tab.cpp"
    break;

  case 181: /* pushliteral: nilobj  */
#line 1152 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3765 "lang11d_tab.cpp"
    break;

  case 182: /* pushliteral: listlit  */
#line 1153 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3771 "lang11d_tab.cpp"
    break;

  case 183: /* listliteral: integer  */
#line 1156 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3777 "lang11d_tab.cpp"
    break;

  case 184: /* listliteral: floatp  */
#line 1157 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3783 "lang11d_tab.cpp"
    break;

  case 185: /* listliteral: ascii  */
#line 1158 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3789 "lang11d_tab.cpp"
    break;

  case 186: /* listliteral: string  */
#line 1159 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3795 "lang11d_tab.cpp"
    break;

  case 187: /* listliteral: symbol  */
#line 1160 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3801 "lang11d_tab.cpp"
    break;

  case 188: /* listliteral: name  */
#line 1161 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3807 "lang11d_tab.cpp"
    break;

  case 189: /* listliteral: trueobj  */
#line 1162 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3813 "lang11d_tab.cpp"
    break;

  case 190: /* listliteral: falseobj  */
#line 1163 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3819 "lang11d_tab.cpp"
    break;

  case 191: /* listliteral: nilobj  */
#line 1164 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3825 "lang11d_tab.cpp"
    break;

  case 192: /* listliteral: listlit2  */
#line 1165 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3831 "lang11d_tab.cpp"
    break;

  case 193: /* listliteral: dictlit2  */
#line 1166 "lang11d"
                                    { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3837 "lang11d_tab.cpp"
    break;

  case 194: /* block: '{' argdecls funcvardecls funcbody '}'  */
#line 1170 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], false); }
#line 3844 "lang11d_tab.cpp"
    break;

  case 195: /* block: BEGINCLOSEDFUNC argdecls funcvardecls funcbody '}'  */
#line 1173 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], true); }
#line 3851 "lang11d_tab.cpp"
    break;

  case 196: /* funcvardecls: %empty  */
#line 1177 "lang11d"
                  { yyval = 0; }
#line 3857 "lang11d_tab.cpp"
    break;

  case 197: /* funcvardecls: funcvardecls funcvardecl  */
#line 1179 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 3863 "lang11d_tab.cpp"
    break;

  case 199: /* funcvardecls1: funcvardecls1 funcvardecl  */
#line 1184 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 3869 "lang11d_tab.cpp"
    break;

  case 200: /* funcvardecl: VAR vardeflist ';'  */
#line 1188 "lang11d"
                                { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varLocal); }
#line 3875 "lang11d_tab.cpp"
    break;

  case 201: /* argdecls: %empty  */
#line 1191 "lang11d"
                  { yyval = 0; }
#line 3881 "lang11d_tab.cpp"
    break;

  case 202: /* argdecls: ARG vardeflist ';'  */
#line 1193 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 3889 "lang11d_tab.cpp"
    break;

  case 203: /* argdecls: ARG vardeflist0 ELLIPSIS name ';'  */
#line 1197 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 3897 "lang11d_tab.cpp"
    break;

  case 204: /* argdecls: '|' slotdeflist '|'  */
#line 1201 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 3905 "lang11d_tab.cpp"
    break;

  case 205: /* argdecls: '|' slotdeflist0 ELLIPSIS name '|'  */
#line 1205 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 3913 "lang11d_tab.cpp"
    break;

  case 206: /* argdecls: '|' slotdeflist0 ELLIPSIS name ',' name '|'  */
#line 1209 "lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 3921 "lang11d_tab.cpp"
    break;

  case 207: /* argdecls1: ARG vardeflist ';'  */
#line 1215 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 3929 "lang11d_tab.cpp"
    break;

  case 208: /* argdecls1: ARG vardeflist0 ELLIPSIS name ';'  */
#line 1219 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 3937 "lang11d_tab.cpp"
    break;

  case 209: /* argdecls1: '|' slotdeflist '|'  */
#line 1223 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 3945 "lang11d_tab.cpp"
    break;

  case 210: /* argdecls1: '|' slotdeflist0 ELLIPSIS name '|'  */
#line 1227 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 3953 "lang11d_tab.cpp"
    break;

  case 211: /* argdecls1: '|' slotdeflist0 ELLIPSIS name ',' name '|'  */
#line 1231 "lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 3961 "lang11d_tab.cpp"
    break;

  case 213: /* constdeflist: constdeflist optcomma constdef  */
#line 1239 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3967 "lang11d_tab.cpp"
    break;

  case 214: /* constdef: rspec name '=' slotliteral  */
#line 1243 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 3973 "lang11d_tab.cpp"
    break;

  case 215: /* slotdeflist0: %empty  */
#line 1246 "lang11d"
                  { yyval = 0; }
#line 3979 "lang11d_tab.cpp"
    break;

  case 218: /* slotdeflist: slotdeflist optcomma slotdef  */
#line 1252 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3985 "lang11d_tab.cpp"
    break;

  case 219: /* slotdef: name  */
#line 1256 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 3991 "lang11d_tab.cpp"
    break;

  case 220: /* slotdef: name optequal slotliteral  */
#line 1258 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 3997 "lang11d_tab.cpp"
    break;

  case 221: /* slotdef: name optequal '(' exprseq ')'  */
#line 1260 "lang11d"
                                {
					PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
					node->mParens = 1;
					yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-4], node, 0);
				}
#line 4007 "lang11d_tab.cpp"
    break;

  case 222: /* vardeflist0: %empty  */
#line 1267 "lang11d"
                  { yyval = 0; }
#line 4013 "lang11d_tab.cpp"
    break;

  case 225: /* vardeflist: vardeflist ',' vardef  */
#line 1273 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4019 "lang11d_tab.cpp"
    break;

  case 226: /* vardef: name  */
#line 1277 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4025 "lang11d_tab.cpp"
    break;

  case 227: /* vardef: name '=' expr  */
#line 1279 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4031 "lang11d_tab.cpp"
    break;

  case 228: /* vardef: name '(' exprseq ')'  */
#line 1281 "lang11d"
                                {
									PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
									node->mParens = 1;
									yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-3], node, 0);
								}
#line 4041 "lang11d_tab.cpp"
    break;

  case 229: /* dictslotdef: exprseq ':' exprseq  */
#line 1289 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4047 "lang11d_tab.cpp"
    break;

  case 230: /* dictslotdef: keybinop exprseq  */
#line 1291 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 4056 "lang11d_tab.cpp"
    break;

  case 232: /* dictslotlist1: dictslotlist1 ',' dictslotdef  */
#line 1299 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4062 "lang11d_tab.cpp"
    break;

  case 233: /* dictslotlist: %empty  */
#line 1302 "lang11d"
                  { yyval = 0; }
#line 4068 "lang11d_tab.cpp"
    break;

  case 236: /* rwslotdeflist: rwslotdeflist ',' rwslotdef  */
#line 1308 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4074 "lang11d_tab.cpp"
    break;

  case 237: /* rwslotdef: rwspec name  */
#line 1312 "lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); }
#line 4080 "lang11d_tab.cpp"
    break;

  case 238: /* rwslotdef: rwspec name '=' slotliteral  */
#line 1314 "lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4086 "lang11d_tab.cpp"
    break;

  case 239: /* dictlit2: '(' litdictslotlist ')'  */
#line 1318 "lang11d"
                                { yyval = (intptr_t)newPyrLitDictNode((PyrParseNode*)yyvsp[-1]); }
#line 4092 "lang11d_tab.cpp"
    break;

  case 240: /* litdictslotdef: listliteral ':' listliteral  */
#line 1322 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4098 "lang11d_tab.cpp"
    break;

  case 241: /* litdictslotdef: keybinop listliteral  */
#line 1324 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 4107 "lang11d_tab.cpp"
    break;

  case 243: /* litdictslotlist1: litdictslotlist1 ',' litdictslotdef  */
#line 1332 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4113 "lang11d_tab.cpp"
    break;

  case 244: /* litdictslotlist: %empty  */
#line 1335 "lang11d"
                  { yyval = 0; }
#line 4119 "lang11d_tab.cpp"
    break;

  case 246: /* listlit: '#' '[' literallistc ']'  */
#line 1342 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 4125 "lang11d_tab.cpp"
    break;

  case 247: /* listlit: '#' classname '[' literallistc ']'  */
#line 1344 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4131 "lang11d_tab.cpp"
    break;

  case 248: /* listlit2: '[' literallistc ']'  */
#line 1348 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 4137 "lang11d_tab.cpp"
    break;

  case 249: /* listlit2: classname '[' literallistc ']'  */
#line 1350 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4143 "lang11d_tab.cpp"
    break;

  case 250: /* literallistc: %empty  */
#line 1353 "lang11d"
                  { yyval = 0; }
#line 4149 "lang11d_tab.cpp"
    break;

  case 253: /* literallist1: literallist1 ',' listliteral  */
#line 1359 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4155 "lang11d_tab.cpp"
    break;

  case 254: /* rwspec: %empty  */
#line 1362 "lang11d"
           { yyval = rwPrivate; }
#line 4161 "lang11d_tab.cpp"
    break;

  case 255: /* rwspec: '<'  */
#line 1364 "lang11d"
                        { yyval = rwReadOnly; }
#line 4167 "lang11d_tab.cpp"
    break;

  case 256: /* rwspec: READWRITEVAR  */
#line 1366 "lang11d"
                        { yyval = rwReadWrite; }
#line 4173 "lang11d_tab.cpp"
    break;

  case 257: /* rwspec: '>'  */
#line 1368 "lang11d"
                        { yyval = rwWriteOnly; }
#line 4179 "lang11d_tab.cpp"
    break;

  case 258: /* rspec: %empty  */
#line 1371 "lang11d"
           { yyval = rwPrivate; }
#line 4185 "lang11d_tab.cpp"
    break;

  case 259: /* rspec: '<'  */
#line 1373 "lang11d"
                        { yyval = rwReadOnly; }
#line 4191 "lang11d_tab.cpp"
    break;

  case 260: /* integer: INTEGER  */
#line 1376 "lang11d"
                  { yyval = zzval; }
#line 4197 "lang11d_tab.cpp"
    break;

  case 261: /* integer: '-' INTEGER  */
#line 1378 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawInt(&node->mSlot));
				yyval = zzval;
			}
#line 4208 "lang11d_tab.cpp"
    break;

  case 262: /* floatr: SC_FLOAT  */
#line 1386 "lang11d"
                   { yyval = zzval; }
#line 4214 "lang11d_tab.cpp"
    break;

  case 263: /* floatr: '-' SC_FLOAT  */
#line 1388 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawFloat(&node->mSlot));
				yyval = zzval;
			}
#line 4225 "lang11d_tab.cpp"
    break;

  case 264: /* accidental: ACCIDENTAL  */
#line 1396 "lang11d"
                        { yyval = zzval; }
#line 4231 "lang11d_tab.cpp"
    break;

  case 265: /* accidental: '-' ACCIDENTAL  */
#line 1398 "lang11d"
                                {
					PyrSlotNode *node;
					double intval, fracval;
					node = (PyrSlotNode*)zzval;
					intval = floor(slotRawFloat(&node->mSlot) + 0.5);
					fracval = slotRawFloat(&node->mSlot) - intval;
					SetRaw(&node->mSlot, -intval + fracval);
					yyval = zzval;
				}
#line 4245 "lang11d_tab.cpp"
    break;

  case 266: /* pie: PIE  */
#line 1408 "lang11d"
                      { yyval = zzval; }
#line 4251 "lang11d_tab.cpp"
    break;

  case 269: /* floatp: floatr pie  */
#line 1414 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)yyvsp[-1];
				SetRaw(&node->mSlot, slotRawFloat(&node->mSlot) * pi);
			}
#line 4261 "lang11d_tab.cpp"
    break;

  case 270: /* floatp: integer pie  */
#line 1420 "lang11d"
                        {
				PyrSlotNode *node;
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = slotRawInt(&node->mSlot);
				SetFloat(&node->mSlot, ival * pi);
			}
#line 4273 "lang11d_tab.cpp"
    break;

  case 271: /* floatp: pie  */
#line 1428 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, pi);
				yyval = zzval;
			}
#line 4284 "lang11d_tab.cpp"
    break;

  case 272: /* floatp: '-' pie  */
#line 1435 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, -pi);
				yyval = zzval;
			}
#line 4295 "lang11d_tab.cpp"
    break;

  case 273: /* name: NAME  */
#line 1443 "lang11d"
                       { yyval = zzval; }
#line 4301 "lang11d_tab.cpp"
    break;

  case 274: /* name: WHILE  */
#line 1444 "lang11d"
                                { yyval = zzval; }
#line 4307 "lang11d_tab.cpp"
    break;

  case 275: /* classname: CLASSNAME  */
#line 1447 "lang11d"
                                    { yyval = zzval; }
#line 4313 "lang11d_tab.cpp"
    break;

  case 276: /* primname: PRIMITIVENAME  */
#line 1450 "lang11d"
                                        { yyval = zzval; }
#line 4319 "lang11d_tab.cpp"
    break;

  case 277: /* trueobj: TRUEOBJ  */
#line 1453 "lang11d"
                          { yyval = zzval; }
#line 4325 "lang11d_tab.cpp"
    break;

  case 278: /* falseobj: FALSEOBJ  */
#line 1456 "lang11d"
                           { yyval = zzval; }
#line 4331 "lang11d_tab.cpp"
    break;

  case 279: /* nilobj: NILOBJ  */
#line 1459 "lang11d"
                         { yyval = zzval; }
#line 4337 "lang11d_tab.cpp"
    break;

  case 280: /* ascii: ASCII  */
#line 1462 "lang11d"
                        { yyval = zzval; }
#line 4343 "lang11d_tab.cpp"
    break;

  case 281: /* symbol: SYMBOL  */
#line 1465 "lang11d"
                         { yyval = zzval; }
#line 4349 "lang11d_tab.cpp"
    break;

  case 282: /* string: STRING  */
#line 1468 "lang11d"
                         { yyval = zzval; }
#line 4355 "lang11d_tab.cpp"
    break;

  case 283: /* pseudovar: PSEUDOVAR  */
#line 1471 "lang11d"
                            { yyval = zzval; }
#line 4361 "lang11d_tab.cpp"
    break;

  case 284: /* binop: BINOP  */
#line 1474 "lang11d"
                { yyval = zzval; }
#line 4367 "lang11d_tab.cpp"
    break;

  case 285: /* binop: READWRITEVAR  */
#line 1475 "lang11d"
                               { yyval = zzval; }
#line 4373 "lang11d_tab.cpp"
    break;

  case 286: /* binop: '<'  */
#line 1476 "lang11d"
                       { yyval = zzval; }
#line 4379 "lang11d_tab.cpp"
    break;

  case 287: /* binop: '>'  */
#line 1477 "lang11d"
                       { yyval = zzval; }
#line 4385 "lang11d_tab.cpp"
    break;

  case 288: /* binop: '-'  */
#line 1478 "lang11d"
                       { yyval = zzval; }
#line 4391 "lang11d_tab.cpp"
    break;

  case 289: /* binop: '*'  */
#line 1479 "lang11d"
                       { yyval = zzval; }
#line 4397 "lang11d_tab.cpp"
    break;

  case 290: /* binop: '+'  */
#line 1480 "lang11d"
                       { yyval = zzval; }
#line 4403 "lang11d_tab.cpp"
    break;

  case 291: /* binop: '|'  */
#line 1481 "lang11d"
                       { yyval = zzval; }
#line 4409 "lang11d_tab.cpp"
    break;

  case 292: /* keybinop: KEYBINOP  */
#line 1484 "lang11d"
                    { yyval = zzval; }
#line 4415 "lang11d_tab.cpp"
    break;

  case 295: /* curryarg: CURRYARG  */
#line 1491 "lang11d"
                    { yyval = zzval; }
#line 4421 "lang11d_tab.cpp"
    break;


#line 4425 "lang11d_tab.cpp"

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

