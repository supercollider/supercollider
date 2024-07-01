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
  YYSYMBOL_dictlit2 = 128,                 /* dictlit2  */
  YYSYMBOL_litdictslotdef = 129,           /* litdictslotdef  */
  YYSYMBOL_litdictslotlist1 = 130,         /* litdictslotlist1  */
  YYSYMBOL_litdictslotlist = 131,          /* litdictslotlist  */
  YYSYMBOL_listlit = 132,                  /* listlit  */
  YYSYMBOL_listlit2 = 133,                 /* listlit2  */
  YYSYMBOL_literallistc = 134,             /* literallistc  */
  YYSYMBOL_literallist1 = 135,             /* literallist1  */
  YYSYMBOL_rwspec = 136,                   /* rwspec  */
  YYSYMBOL_rspec = 137,                    /* rspec  */
  YYSYMBOL_integer = 138,                  /* integer  */
  YYSYMBOL_floatr = 139,                   /* floatr  */
  YYSYMBOL_accidental = 140,               /* accidental  */
  YYSYMBOL_pie = 141,                      /* pie  */
  YYSYMBOL_floatp = 142,                   /* floatp  */
  YYSYMBOL_name = 143,                     /* name  */
  YYSYMBOL_classname = 144,                /* classname  */
  YYSYMBOL_primname = 145,                 /* primname  */
  YYSYMBOL_trueobj = 146,                  /* trueobj  */
  YYSYMBOL_falseobj = 147,                 /* falseobj  */
  YYSYMBOL_nilobj = 148,                   /* nilobj  */
  YYSYMBOL_ascii = 149,                    /* ascii  */
  YYSYMBOL_symbol = 150,                   /* symbol  */
  YYSYMBOL_string = 151,                   /* string  */
  YYSYMBOL_pseudovar = 152,                /* pseudovar  */
  YYSYMBOL_binop = 153,                    /* binop  */
  YYSYMBOL_keybinop = 154,                 /* keybinop  */
  YYSYMBOL_binop2 = 155,                   /* binop2  */
  YYSYMBOL_curryarg = 156                  /* curryarg  */
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
#define YYLAST   1965

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  102
/* YYNRULES -- Number of rules.  */
#define YYNRULES  303
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  571

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
     707,   735,   754,   782,   809,   827,   852,   880,   899,   927,
     946,   965,   982,   996,  1017,  1036,  1054,  1071,  1087,  1103,
    1104,  1105,  1106,  1107,  1120,  1134,  1139,  1143,  1154,  1159,
    1169,  1174,  1188,  1204,  1205,  1206,  1207,  1210,  1211,  1217,
    1220,  1221,  1225,  1226,  1228,  1233,  1235,  1242,  1250,  1251,
    1255,  1257,  1261,  1262,  1266,  1270,  1271,  1274,  1276,  1280,
    1281,  1286,  1287,  1288,  1289,  1290,  1291,  1292,  1293,  1294,
    1297,  1300,  1303,  1304,  1305,  1306,  1307,  1308,  1309,  1310,
    1311,  1314,  1315,  1316,  1317,  1318,  1319,  1320,  1321,  1322,
    1323,  1324,  1327,  1330,  1335,  1336,  1340,  1341,  1345,  1349,
    1350,  1354,  1358,  1362,  1366,  1372,  1376,  1380,  1384,  1388,
    1395,  1396,  1400,  1404,  1405,  1408,  1409,  1413,  1415,  1417,
    1425,  1426,  1429,  1430,  1434,  1436,  1438,  1446,  1448,  1455,
    1456,  1460,  1461,  1464,  1465,  1469,  1471,  1475,  1479,  1481,
    1488,  1489,  1493,  1494,  1499,  1501,  1505,  1507,  1511,  1512,
    1515,  1516,  1520,  1521,  1523,  1525,  1529,  1530,  1534,  1535,
    1544,  1545,  1554,  1555,  1566,  1569,  1570,  1571,  1577,  1585,
    1592,  1601,  1602,  1605,  1608,  1611,  1614,  1617,  1620,  1623,
    1626,  1629,  1632,  1633,  1634,  1635,  1636,  1637,  1638,  1639,
    1642,  1645,  1646,  1649
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
  "rwslotdef", "dictlit2", "litdictslotdef", "litdictslotlist1",
  "litdictslotlist", "listlit", "listlit2", "literallistc", "literallist1",
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

#define YYPACT_NINF (-477)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-300)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      23,   983,    60,    58,    60,    35,  -477,  -477,  -477,  -477,
    -477,  -477,  -477,  -477,  -477,  -477,    53,    53,  -477,  -477,
    -477,  -477,  -477,   127,  -477,   339,    53,  1815,   198,  1451,
     879,  1815,    53,    43,  -477,  -477,  -477,  -477,  -477,    40,
    -477,  -477,  -477,  1904,    59,    66,  -477,  -477,  -477,  -477,
    1191,  -477,  1191,  -477,   108,   108,  -477,  -477,  -477,   271,
     103,  -477,  -477,  -477,  -477,  -477,  -477,  -477,  -477,   102,
    -477,  -477,    67,  -477,   -23,  -477,   109,   131,   182,    53,
      53,  -477,  -477,  -477,  -477,  -477,   150,    42,  -477,   205,
     931,  -477,  1815,  1815,  -477,  -477,   159,   163,   164,  1815,
    1815,  1503,  -477,   339,  -477,  -477,  -477,  -477,    16,  -477,
     196,    99,  1191,  1191,  -477,   176,   212,  -477,  1815,   217,
     813,   243,  1901,   247,    21,  -477,   236,  1555,  -477,  -477,
      82,  -477,   249,  1815,  -477,  -477,  -477,  -477,  -477,  1191,
    -477,  -477,  1815,  1243,   174,  -477,  -477,  -477,  1451,  1035,
     174,  -477,    60,    53,   240,  -477,    53,  1815,  1815,    53,
    -477,   278,   276,   280,    54,  1191,    53,  -477,  -477,    53,
    -477,   661,  -477,  -477,  1191,  1815,  -477,  1451,  -477,  -477,
    -477,  1815,   241,    47,  -477,  1815,  1815,  1815,  -477,   252,
     255,  1191,  1451,  -477,  -477,  -477,   183,  -477,  -477,  1815,
    1901,  1867,  -477,  -477,  -477,   261,   265,   108,  -477,  -477,
     272,  -477,  -477,  -477,  -477,  -477,  -477,  1815,    53,    53,
    1901,  1815,  -477,    86,   282,  1607,  1087,   273,    28,  1815,
    1904,  -477,  1904,  1815,   174,   279,   306,  -477,   275,   174,
     279,   306,   308,  -477,  1815,   745,  -477,   303,  -477,  -477,
    -477,  1904,   309,   317,    53,  -477,    53,  -477,   321,  -477,
     162,  -477,  1815,    13,  -477,  -477,   108,  -477,  -477,  -477,
    -477,  -477,  -477,  -477,   319,   323,   322,  -477,   351,  1815,
    -477,  -477,  1815,  1815,  -477,  -477,   363,  -477,  -477,   335,
     357,  -477,  1815,  1295,   174,  1904,   350,   358,  -477,   349,
     348,  1901,  -477,  1901,  -477,  1901,  1904,  -477,  -477,   355,
     356,  1659,   373,  1815,  1815,  1815,   147,   174,   279,   306,
     308,  1815,  1139,   174,  -477,   401,  1815,  -477,  -477,   365,
    -477,   174,  1347,  -477,   364,   374,   370,  -477,  -477,   371,
     376,   374,   377,  -477,   604,  -477,  -477,   372,   381,   384,
     237,  -477,  -477,   383,   199,  -477,  -477,    53,   390,  1399,
    1399,  -477,  1815,  -477,  -477,   410,  1815,  -477,   174,   279,
     306,  -477,  1901,  1867,  -477,  -477,  -477,  -477,   388,  -477,
     411,   414,   399,  1815,  -477,   403,   408,  1711,   419,  -477,
     406,   409,   413,  1904,   174,   279,   306,   308,   430,  1815,
     308,   185,  -477,   174,  -477,  -477,   174,   439,   440,   127,
     127,   442,   251,   251,   453,  -477,   783,  -477,  -477,    53,
     451,  -477,    53,   301,   447,   444,    24,   454,  -477,  1815,
    -477,   174,   449,   450,  -477,  -477,  -477,  1815,  1815,   467,
    1904,  1815,   472,   473,   458,  1815,   174,  -477,   174,  -477,
     455,   456,   457,  -477,  -477,  -477,  1815,  -477,  -477,  -477,
     127,   127,  -477,  -477,  -477,  -477,  -477,  -477,   286,  -477,
      53,   292,  -477,   298,  -477,    53,  -477,   470,  -477,   479,
    1815,  1815,  -477,  1399,  -477,  1815,   484,  -477,  -477,   174,
    -477,  1904,  1904,  1815,   462,  1815,  1815,   483,  1904,  -477,
    -477,   174,  -477,   174,  1904,  -477,  -477,   154,   154,   237,
    -477,   251,   486,  -477,  -477,   453,   490,  -477,  1815,   444,
     444,  -477,   444,  1815,  -477,  1904,  -477,  1904,  1904,  1815,
    -477,  -477,   154,   154,  -477,  1763,   477,  1763,  1920,  -477,
     699,  -477,   699,   444,  -477,  -477,  -477,   444,  1904,  1763,
    1763,  1815,   482,  -477,   476,  -477,   491,  -477,  -477,  -477,
    -477,  -477,   495,   496,   813,  -477,  -477,  -477,  -477,  -477,
    -477
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       5,    48,     0,     0,     2,     3,     7,   281,   268,   270,
     272,   289,   290,   288,   283,   303,     0,   230,   287,   285,
     286,   291,   274,   209,   282,     0,   223,     0,   209,   150,
     241,     0,     0,     0,    41,     4,    33,    97,    94,   129,
     106,   131,   130,   147,    27,    48,    93,    95,    92,   180,
      48,   206,    48,   190,   182,   275,   276,   279,   183,   181,
     132,   187,   188,   189,   184,   186,   185,   104,    96,     0,
       1,     6,    14,     8,     0,   232,   234,     0,   231,   230,
     223,   204,   269,   271,   273,   280,     0,    29,   225,    31,
     241,   134,     0,     0,   204,   300,   152,     0,    29,     0,
       0,     0,   292,   296,   294,   295,   297,   298,   223,   293,
       0,     0,    48,    48,   239,    29,     0,   301,   302,     0,
      27,    99,   258,     0,   167,   169,     0,     0,   296,   299,
       0,   302,   143,    28,   149,    34,    40,   207,    39,    48,
     278,   277,     0,     0,    56,    50,    53,    52,   150,     0,
      65,    21,     0,    12,     0,   208,     0,     0,     0,     0,
     215,     0,   231,     0,    29,    48,     0,   217,    30,     0,
      32,     0,    80,    82,    48,     0,   100,    30,   151,   154,
     120,     0,     0,     0,   101,   119,     0,     0,    98,     0,
       0,    48,    30,   242,   103,   238,     0,    28,    49,     0,
     258,   252,   260,   201,   200,     0,    29,   191,   192,   196,
       0,   197,   198,   199,   193,   195,   194,     0,     0,     0,
     258,     0,   158,     0,     0,     0,     0,    54,     0,     0,
     148,    38,   136,     0,     0,    29,    29,    51,     0,    54,
      29,    29,    29,   162,     0,     0,    15,     0,    13,    16,
     233,   235,     0,     0,     0,   210,     0,   212,     0,   205,
       0,   226,     0,     0,   228,   179,   171,   172,   176,   177,
     178,   173,   175,   174,     0,     0,     0,   153,   155,     0,
     124,   102,   125,     0,   121,   237,     0,    37,    36,     0,
       0,   240,     0,     0,    57,   137,     0,     0,   250,    29,
       0,     0,   254,    30,   259,   258,   140,   168,   170,     0,
       0,     0,   105,     0,     0,     0,     0,    54,    29,    29,
      29,     0,     0,    55,    79,     0,     0,   145,   144,   135,
     160,    58,    30,   165,     0,    30,     0,    64,    66,     0,
       0,    30,     0,   164,   297,    11,    22,     0,     0,    14,
      21,   236,   216,     0,     0,   203,   218,     0,     0,     0,
       0,   202,     0,   156,   126,     0,   123,    35,     0,    29,
      29,   256,     0,    30,   253,   247,   249,   261,     0,   255,
     108,   107,     0,     0,   159,     0,     0,     0,   133,    70,
       0,     0,     0,   138,    54,    29,    29,    29,     0,     0,
      29,    54,    62,    54,    69,   163,    54,     0,     0,   209,
     209,     0,   262,   262,   266,    17,     0,   211,   213,     0,
       0,   229,     0,     0,     0,    84,   181,     0,   157,   127,
     122,    60,     0,     0,   248,   251,   257,     0,     0,   109,
     141,     0,   114,   113,     0,     0,    54,    75,    54,    76,
       0,     0,     0,   146,   161,   166,     0,    59,    68,    67,
     209,   209,   204,   204,    16,   263,   265,   264,     0,   243,
       0,     0,   267,    29,   220,     0,     9,     0,   219,     0,
       0,     0,    81,     0,    89,     0,     0,    83,   128,    54,
      63,   111,   110,     0,     0,     0,     0,   115,   142,    73,
      71,    54,    78,    54,   139,   204,   204,    44,    44,    21,
      19,   262,   245,    18,    20,   266,     0,   214,     0,    84,
      84,    85,    84,     0,    61,   112,    74,   117,   116,     0,
      77,    72,    44,    44,   284,    46,    27,    46,     0,   244,
       0,   221,     0,    84,    91,    90,    86,    84,   118,    46,
      46,     0,     0,    42,    46,    45,     0,    10,   246,   222,
      88,    87,     0,     0,    27,    23,    43,    25,    24,    26,
      47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -477,  -477,  -477,  -477,  -477,   524,  -477,   186,    79,  -477,
    -341,  -477,  -117,   -64,  -477,   420,  -477,  -273,  -263,   -10,
     500,   -31,  -138,   145,  -477,   -52,  -477,  -477,  -247,  -346,
    -477,  -477,  -477,  -477,  -477,  -477,   -27,  -477,  -477,   290,
     402,  -477,  -102,  -116,  -136,   206,   -57,  -477,  -477,  -476,
     341,  -477,  -477,  -158,  -477,   -93,    -8,    49,   -17,   519,
    -477,    38,   475,   480,   392,   478,     1,   407,   378,  -477,
    -477,   149,    63,  -477,   191,  -477,  -477,  -169,  -477,  -185,
    -477,  -477,  -477,  -106,  -477,  -477,   -15,   -84,     4,   493,
    -477,   -62,   -46,   -20,   158,   253,   318,  -477,  -240,   537,
     -13,  -477
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     3,     4,     5,    71,     6,   247,   154,   350,   415,
     245,   346,   134,   333,   171,    34,    35,   552,   535,   553,
      36,   323,   145,   324,    37,    38,   274,   275,   484,   424,
      39,    40,    41,    42,   110,   182,    43,   229,    44,    45,
      97,    98,   223,   236,   400,   243,   334,   123,   124,   264,
      46,    47,    48,   202,    49,   165,    50,   259,    81,    52,
     473,   474,    86,    87,    88,    77,    74,    75,   114,   115,
     116,   468,   469,   203,   298,   299,   300,    53,   204,   205,
     206,   470,   475,    54,    55,    56,    57,    58,    59,    60,
     536,    61,    62,    63,    64,    65,    66,    67,   117,   131,
     132,    68
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      91,   174,   265,   198,   120,   348,   237,   146,   146,   416,
      85,    94,   237,   242,   427,   296,   207,   119,    78,     7,
      76,    76,   112,   169,    14,   155,   156,     7,   144,   150,
      89,     7,     8,   241,   178,   309,   121,   125,   208,   140,
     141,   235,   218,   297,   139,    24,     7,   240,    23,     1,
      51,   193,   485,    24,    14,   142,     7,    24,    70,   122,
     211,     2,   325,  -224,   558,   266,   559,  -299,    28,   282,
     219,    14,    24,     2,   143,  -224,   212,   119,   326,    51,
     162,   167,    24,    76,    89,     7,   127,   267,    85,   122,
     320,   168,   146,   257,   207,   207,   283,   152,   146,   137,
     169,    51,   213,   168,   408,   191,   230,   133,   311,   268,
     319,    24,    89,   153,   207,   232,   208,   208,    31,   224,
     378,   185,   327,   316,   318,   269,   209,    23,   225,   186,
     251,    22,   226,   312,   227,   313,   208,   521,   211,   211,
     157,    79,   304,   376,   146,   377,   151,    28,   187,   148,
     188,   270,   159,   149,   212,   212,   237,   248,   211,   158,
      76,   137,    51,   253,   534,   294,    80,    16,   538,   387,
     260,   166,   295,    89,   212,   146,   348,   370,   342,   336,
     213,   213,   146,   339,   340,   237,   397,   146,   137,   175,
     306,   369,   140,   237,   388,   207,   313,   207,    23,   207,
     213,   356,   329,   331,   209,   209,   396,    23,  -227,    23,
     176,   357,    79,   177,   434,   297,   456,   208,    28,   208,
     395,   208,   307,   308,   209,   192,  -227,    28,    92,    28,
     160,   156,   328,   293,  -227,   374,   170,    80,   418,   211,
     137,   211,   146,   211,  -227,   537,    93,   184,   419,   347,
     412,   140,   413,   414,  -227,   212,   392,   212,   353,   212,
     354,   390,   391,   194,   556,   146,   207,   207,   196,   549,
     550,   146,   544,   545,   199,   546,   562,   563,   217,   146,
     214,   213,   220,   213,   249,   213,   465,   466,   208,   208,
     228,   467,   281,   237,   393,    23,   560,    23,   348,   254,
     561,   256,   142,   287,   321,   209,   288,   209,   302,   209,
     211,   211,   432,   433,   303,    28,   146,    28,   305,    96,
     111,   143,   337,   322,   255,   156,   212,   212,   332,   271,
     480,   481,   314,   452,   510,   511,   455,   431,   450,   451,
     513,   511,   146,    82,    83,    84,   514,   168,   407,   146,
     349,   146,   213,   213,   146,   335,   440,   341,   214,   214,
     351,   420,    22,   426,   426,   352,   355,   361,   359,   507,
     508,   265,   360,   265,   146,   215,   209,   209,   214,   146,
     111,   362,   172,   173,   338,   366,   367,   186,   372,   179,
     180,   183,   462,   463,   146,   144,   146,   371,   373,   375,
     147,   147,   379,   380,   383,    82,   130,    95,   195,   515,
     491,   492,   532,   533,   152,   401,   409,   222,   498,   555,
     347,   402,   403,   477,   272,   410,   479,   404,   406,   504,
     486,   417,   429,   222,   266,   436,   266,   146,    96,   222,
     216,   421,   437,   505,   506,   438,   439,   570,   252,   146,
     445,   146,   441,   215,   215,   442,   267,   446,   267,   214,
     447,   214,   389,   214,   448,   277,   525,   278,   527,   528,
     136,   280,   138,   215,   512,   284,   285,   286,   268,   516,
     268,   453,   290,   460,   461,   147,   464,   426,   472,   273,
     478,   147,   482,   483,   269,    69,   269,    72,   493,   487,
     489,   490,   548,   495,   496,   497,   501,   502,   503,   517,
     518,   310,   523,   526,   529,   222,   222,   540,   216,   216,
     270,   542,   270,   330,   564,   197,   126,   565,   551,    73,
     214,   214,   189,   190,   343,   411,   567,   147,   216,   449,
     568,   569,   347,   509,   566,   135,   457,   405,   458,   113,
     238,   459,   358,   541,   215,   163,   215,   161,   215,   231,
     164,   261,   471,   250,   435,     0,    99,   118,   147,   363,
     291,     0,   364,   365,   539,   147,     0,     0,     0,     0,
     147,     0,   195,   222,     0,   258,     0,     0,     0,     0,
       0,   499,     0,   500,   276,     0,     0,     0,     0,     0,
       0,   382,     0,   384,   385,   386,     0,     7,     0,     0,
       0,   289,   222,     0,     0,   210,   398,     0,     0,   216,
       0,   216,   384,   216,     0,   215,   215,   118,     0,     0,
       0,     0,     0,    24,   524,   147,   102,     0,   128,   104,
     105,   106,   107,   129,   109,   246,   530,     0,   531,   425,
     425,     0,   428,     0,     0,     0,   430,     0,   147,     0,
       0,     0,     0,     0,   147,     8,     9,    10,    11,    12,
      13,     0,   147,     0,     0,     0,     0,   444,    18,    19,
      20,     0,     0,     0,    22,    99,   244,     0,     0,   454,
     216,   216,     0,   210,   210,    25,     0,     0,   271,     0,
     271,     0,     0,     8,     9,    10,    11,    12,    13,   147,
       0,   262,     0,   210,   279,   263,    18,    19,    20,   488,
       0,     0,    22,     0,     0,     0,     0,     0,     0,   292,
       0,   494,     0,    25,     0,   147,     0,     0,   301,     0,
       0,     0,   147,     0,   147,     0,     0,   147,     7,     0,
       0,     0,     0,   263,     0,     0,   126,     0,     0,     0,
       0,     0,     0,   244,     0,     0,     0,   147,     0,     0,
     519,   520,   147,   425,    24,   522,     0,   102,     0,   128,
     104,   105,   344,   107,   129,   109,     7,   147,     0,   147,
     345,     0,     0,   272,   210,   272,   210,     0,   210,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   543,     0,
       0,     0,    24,   547,     0,   102,     0,   128,   104,   105,
     344,   107,   129,   109,     0,   554,     0,   554,   476,     0,
     147,     0,     0,     0,     0,     0,     0,     0,     0,   554,
     554,     0,   147,     0,   147,   102,    95,   128,   104,   105,
     106,   107,   129,   109,   130,     0,     0,     0,   273,   244,
     273,   197,     0,     0,     0,   210,   210,     0,     0,   244,
       0,     0,   244,     0,     0,     0,     0,     0,   244,     0,
       0,     0,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,    16,    17,     0,     0,    18,    19,    20,    21,
       0,   100,    22,    23,     0,     0,     0,     0,    24,   101,
     301,   102,    95,   103,   104,   105,   106,   107,   108,   109,
       0,    27,     0,    28,     0,    29,     0,     0,     0,    90,
       0,     0,    32,    33,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    18,    19,
      20,    21,     0,   100,    22,    23,     0,     0,     0,     0,
      24,   101,     0,   102,    95,   103,   104,   105,   106,   107,
     129,   109,     0,    27,     0,    28,     0,    29,     0,     0,
       0,    90,     0,     0,    32,    33,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,    16,    17,     0,     0,
      18,    19,    20,    21,     0,     0,    22,    23,     0,     0,
       0,     0,    24,     0,     0,     0,     0,    25,     0,     0,
       0,     0,    26,     0,     0,    27,     0,    28,     0,    29,
       0,     0,     0,    30,     0,    31,    32,    33,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,     0,     0,
       0,     0,    18,    19,    20,    21,     0,     0,    22,    23,
       0,     0,     0,     0,    24,     0,     0,     0,    95,    25,
       0,     0,   233,     0,     0,     0,     0,    27,     0,    28,
       0,    29,     0,     0,     0,    90,   239,     0,    32,    33,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    18,    19,    20,    21,     0,     0,
      22,    23,     0,     0,     0,     0,    24,     0,     0,     0,
      95,    25,     0,     0,   233,     0,     0,     0,     0,    27,
       0,    28,     0,    29,     0,     0,     0,    90,   317,     0,
      32,    33,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    18,    19,    20,    21,
       0,     0,    22,    23,     0,     0,     0,     0,    24,     0,
       0,     0,    95,    25,     0,     0,   233,     0,     0,     0,
       0,    27,     0,    28,     0,    29,     0,     0,     0,    90,
     394,     0,    32,    33,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,    16,     0,     0,     0,    18,    19,
      20,    21,     0,     0,    22,    23,     0,     0,     0,     0,
      24,     0,     0,     0,     0,    25,     0,     0,     0,     0,
       0,     0,     0,    27,     0,    28,     0,    29,     0,     0,
       0,    90,     0,    31,    32,    33,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,     0,     0,     0,     0,
      18,    19,    20,    21,     0,     0,    22,    23,     0,     0,
       0,     0,    24,     0,     0,     0,     0,    25,     0,     0,
     233,     0,     0,     0,     0,    27,     0,    28,     0,    29,
       0,     0,     0,    90,   234,     0,    32,    33,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,     0,     0,
       0,     0,    18,    19,    20,    21,     0,     0,    22,    23,
       0,     0,     0,     0,    24,     0,     0,     0,     0,    25,
       0,     0,   233,     0,     0,     0,     0,    27,     0,    28,
       0,    29,     0,     0,     0,    90,   368,     0,    32,    33,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    18,    19,    20,    21,     0,     0,
      22,    23,     0,     0,     0,     0,    24,     0,     0,     0,
      95,    25,     0,     0,   399,     0,     0,     0,     0,    27,
       0,    28,     0,    29,     0,     0,     0,    90,     0,     0,
      32,    33,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,   422,     0,     0,     0,    18,    19,    20,    21,
       0,     0,    22,    23,     0,     0,     0,     0,    24,   423,
       0,     0,     0,    25,     0,     0,     0,     0,     0,     0,
       0,    27,     0,    28,     0,    29,     0,     0,     0,    90,
       0,     0,    32,    33,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    18,    19,
      20,    21,     0,     0,    22,    23,     0,     0,     0,     0,
      24,     0,     0,     0,    95,    25,     0,     0,     0,     0,
       0,     0,     0,    27,     0,    28,     0,    29,     0,     0,
       0,    90,     0,     0,    32,    33,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,     0,     0,     0,     0,
      18,    19,    20,    21,     0,   181,    22,    23,     0,     0,
       0,     0,    24,     0,     0,     0,     0,    25,     0,     0,
       0,     0,     0,     0,     0,    27,     0,    28,     0,    29,
       0,     0,     0,    90,     0,     0,    32,    33,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,     0,     0,
       0,     0,    18,    19,    20,    21,     0,   221,    22,    23,
       0,     0,     0,     0,    24,     0,     0,     0,     0,    25,
       0,     0,     0,     0,     0,     0,     0,    27,     0,    28,
       0,    29,     0,     0,     0,    90,     0,     0,    32,    33,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    18,    19,    20,    21,     0,   315,
      22,    23,     0,     0,     0,     0,    24,     0,     0,     0,
       0,    25,     0,     0,     0,     0,     0,     0,     0,    27,
       0,    28,     0,    29,     0,     0,     0,    90,     0,     0,
      32,    33,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    18,    19,    20,    21,
       0,     0,    22,    23,     0,     0,     0,     0,    24,     0,
       0,     0,     0,    25,     0,     0,     0,     0,     0,     0,
       0,    27,     0,    28,     0,    29,   381,     0,     0,    90,
       0,     0,    32,    33,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    18,    19,
      20,    21,     0,     0,    22,    23,     0,     0,     0,     0,
      24,     0,     0,     0,     0,    25,     0,     0,     0,     0,
       0,     0,     0,    27,     0,    28,     0,    29,   443,     0,
       0,    90,     0,     0,    32,    33,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,     0,     0,     0,     0,
      18,    19,    20,    21,     0,     0,    22,    23,     0,     0,
       0,     0,    24,     0,     0,     0,     0,    25,     0,     0,
       0,     0,     0,     0,     0,    27,     0,    28,     0,    29,
       0,     0,     0,    90,     0,   551,    32,    33,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,     0,     0,
       0,     0,    18,    19,    20,    21,     0,     0,    22,    23,
       0,     0,     0,     0,    24,     0,     0,     0,     0,    25,
       0,     0,     0,     0,     0,     0,     0,    27,     0,    28,
       0,    29,     0,     0,     0,    90,     0,     0,    32,    33,
       7,     8,     9,    10,    11,    12,    13,     0,    14,     0,
       0,     0,     0,     0,    18,    19,    20,     0,     0,     0,
      22,     0,     0,     0,     0,     0,    24,     0,     0,     0,
      95,    25,     0,     0,     7,     8,     9,    10,    11,    12,
      13,     0,    14,   200,     0,     0,     0,   201,    18,    19,
      20,     0,     0,     7,    22,     0,     0,     0,     0,     0,
      24,     0,     0,     0,     0,    25,   102,    95,   128,   104,
     105,   106,   107,   129,   109,   130,     0,   200,     0,    24,
       0,   201,   102,     0,   128,   104,   105,   344,   107,   129,
     109,     0,     0,     0,     0,   557
};

static const yytype_int16 yycheck[] =
{
      27,    94,   171,   120,    31,   245,   144,    59,    60,   350,
      25,    28,   150,   149,   360,   200,   122,    30,    17,     3,
      16,    17,    30,    87,    11,    48,    49,     3,    59,    60,
      26,     3,     4,   149,    98,   220,    32,    33,   122,    54,
      55,   143,    21,   201,    52,    29,     3,   149,    24,    26,
       1,   115,    28,    29,    11,    31,     3,    29,     0,    46,
     122,    38,    34,    21,   540,   171,   542,    51,    44,    22,
      49,    11,    29,    38,    50,    21,   122,    90,    50,    30,
      79,    39,    29,    79,    80,     3,    46,   171,   103,    46,
     226,    49,   144,    39,   200,   201,    49,    30,   150,    50,
     164,    52,   122,    49,   344,   113,   133,    48,    22,   171,
     226,    29,   108,    46,   220,   142,   200,   201,    52,    37,
     305,    22,   228,   225,   226,   171,   122,    24,    46,    30,
     157,    23,    50,    47,   130,    49,   220,   483,   200,   201,
      31,    14,   206,   301,   196,   303,    44,    44,    49,    46,
      51,   171,    21,    50,   200,   201,   294,   153,   220,    50,
     156,   112,   113,   159,    10,   196,    39,    13,   509,    22,
     166,    21,   199,   169,   220,   227,   416,   293,   242,   236,
     200,   201,   234,   240,   241,   323,   322,   239,   139,    30,
     217,   293,   207,   331,    47,   301,    49,   303,    24,   305,
     220,    39,   229,   234,   200,   201,   322,    24,     3,    24,
      47,    49,    14,    49,   372,   373,    31,   301,    44,   303,
     322,   305,   218,   219,   220,    49,    21,    44,    30,    44,
      48,    49,   228,    50,    29,   299,    31,    39,    39,   301,
     191,   303,   294,   305,    39,   508,    48,    51,    49,   245,
      13,   266,    15,    16,    49,   301,   320,   303,   254,   305,
     256,   318,   319,    51,   537,   317,   372,   373,    51,   532,
     533,   323,   519,   520,    31,   522,   549,   550,    31,   331,
     122,   301,    46,   303,    44,   305,    35,    36,   372,   373,
      41,    40,    51,   431,   321,    24,   543,    24,   538,    21,
     547,    21,    31,    51,    31,   301,    51,   303,    47,   305,
     372,   373,   369,   370,    49,    44,   368,    44,    46,    29,
      30,    50,    47,    50,    48,    49,   372,   373,    49,   171,
      29,    30,    50,   397,    48,    49,   400,   368,   395,   396,
      48,    49,   394,     4,     5,     6,    48,    49,   344,   401,
      47,   403,   372,   373,   406,    49,   383,    49,   200,   201,
      51,   357,    23,   359,   360,    48,    45,    45,    49,   462,
     463,   540,    49,   542,   426,   122,   372,   373,   220,   431,
      90,    30,    92,    93,   239,    22,    51,    30,    30,    99,
     100,   101,   409,   410,   446,   426,   448,    47,    49,    51,
      59,    60,    47,    47,    31,     4,    41,    33,   118,   473,
     437,   438,   505,   506,    30,    51,    44,   127,   445,   536,
     416,    51,    51,   419,   171,    44,   422,    51,    51,   456,
     426,    48,    22,   143,   540,    47,   542,   489,   148,   149,
     122,    51,    31,   460,   461,    31,    47,   564,   158,   501,
      31,   503,    49,   200,   201,    47,   540,    51,   542,   301,
      51,   303,   317,   305,    51,   175,   493,   177,   495,   496,
      50,   181,    52,   220,   470,   185,   186,   187,   540,   475,
     542,    51,   192,    44,    44,   144,    44,   483,    35,   171,
      39,   150,    45,    49,   540,     2,   542,     4,    31,    45,
      51,    51,   529,    31,    31,    47,    51,    51,    51,    39,
      31,   221,    28,    51,    31,   225,   226,    31,   200,   201,
     540,    31,   542,   233,   551,    48,    33,    45,    52,     5,
     372,   373,   112,   113,   244,   349,    45,   196,   220,   394,
      45,    45,   538,   464,   554,    45,   401,   341,   403,    30,
     148,   406,   262,   515,   301,    80,   303,    79,   305,   139,
      80,   169,   413,   156,   373,    -1,    29,    30,   227,   279,
     192,    -1,   282,   283,   511,   234,    -1,    -1,    -1,    -1,
     239,    -1,   292,   293,    -1,   165,    -1,    -1,    -1,    -1,
      -1,   446,    -1,   448,   174,    -1,    -1,    -1,    -1,    -1,
      -1,   311,    -1,   313,   314,   315,    -1,     3,    -1,    -1,
      -1,   191,   322,    -1,    -1,   122,   326,    -1,    -1,   301,
      -1,   303,   332,   305,    -1,   372,   373,    90,    -1,    -1,
      -1,    -1,    -1,    29,   489,   294,    32,    -1,    34,    35,
      36,    37,    38,    39,    40,   152,   501,    -1,   503,   359,
     360,    -1,   362,    -1,    -1,    -1,   366,    -1,   317,    -1,
      -1,    -1,    -1,    -1,   323,     4,     5,     6,     7,     8,
       9,    -1,   331,    -1,    -1,    -1,    -1,   387,    17,    18,
      19,    -1,    -1,    -1,    23,   148,   149,    -1,    -1,   399,
     372,   373,    -1,   200,   201,    34,    -1,    -1,   540,    -1,
     542,    -1,    -1,     4,     5,     6,     7,     8,     9,   368,
      -1,    50,    -1,   220,   177,    54,    17,    18,    19,   429,
      -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,   192,
      -1,   441,    -1,    34,    -1,   394,    -1,    -1,   201,    -1,
      -1,    -1,   401,    -1,   403,    -1,    -1,   406,     3,    -1,
      -1,    -1,    -1,    54,    -1,    -1,   263,    -1,    -1,    -1,
      -1,    -1,    -1,   226,    -1,    -1,    -1,   426,    -1,    -1,
     480,   481,   431,   483,    29,   485,    -1,    32,    -1,    34,
      35,    36,    37,    38,    39,    40,     3,   446,    -1,   448,
      45,    -1,    -1,   540,   301,   542,   303,    -1,   305,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   518,    -1,
      -1,    -1,    29,   523,    -1,    32,    -1,    34,    35,    36,
      37,    38,    39,    40,    -1,   535,    -1,   537,    45,    -1,
     489,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   549,
     550,    -1,   501,    -1,   503,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    -1,    -1,    -1,   540,   322,
     542,    48,    -1,    -1,    -1,   372,   373,    -1,    -1,   332,
      -1,    -1,   335,    -1,    -1,    -1,    -1,    -1,   341,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    13,    14,    -1,    -1,    17,    18,    19,    20,
      -1,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
     373,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,
      -1,    -1,    53,    54,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    -1,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,
      -1,    50,    -1,    -1,    53,    54,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    13,    14,    -1,    -1,
      17,    18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    -1,    -1,    34,    -1,    -1,
      -1,    -1,    39,    -1,    -1,    42,    -1,    44,    -1,    46,
      -1,    -1,    -1,    50,    -1,    52,    53,    54,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    -1,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,    24,
      -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,    33,    34,
      -1,    -1,    37,    -1,    -1,    -1,    -1,    42,    -1,    44,
      -1,    46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,
      23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,
      33,    34,    -1,    -1,    37,    -1,    -1,    -1,    -1,    42,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,
      53,    54,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,
      -1,    -1,    33,    34,    -1,    -1,    37,    -1,    -1,    -1,
      -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,
      51,    -1,    53,    54,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    13,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,
      -1,    50,    -1,    52,    53,    54,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,
      17,    18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    -1,    -1,    34,    -1,    -1,
      37,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,
      -1,    -1,    -1,    50,    51,    -1,    53,    54,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    -1,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,    24,
      -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    34,
      -1,    -1,    37,    -1,    -1,    -1,    -1,    42,    -1,    44,
      -1,    46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,
      23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,
      33,    34,    -1,    -1,    37,    -1,    -1,    -1,    -1,    42,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,
      53,    54,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    13,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,
      -1,    -1,    53,    54,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    -1,    33,    34,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,
      -1,    50,    -1,    -1,    53,    54,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,
      17,    18,    19,    20,    -1,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    -1,    -1,    34,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,
      -1,    -1,    -1,    50,    -1,    -1,    53,    54,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    -1,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    34,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,
      -1,    46,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,
      -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,
      53,    54,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,
      -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    -1,    44,    -1,    46,    47,    -1,    -1,    50,
      -1,    -1,    53,    54,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    42,    -1,    44,    -1,    46,    47,    -1,
      -1,    50,    -1,    -1,    53,    54,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,
      17,    18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    -1,    -1,    34,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    46,
      -1,    -1,    -1,    50,    -1,    52,    53,    54,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    -1,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,    24,
      -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    34,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,
      -1,    46,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    -1,
      -1,    -1,    -1,    -1,    17,    18,    19,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,
      33,    34,    -1,    -1,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    46,    -1,    -1,    -1,    50,    17,    18,
      19,    -1,    -1,     3,    23,    -1,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    -1,    -1,    34,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    -1,    46,    -1,    29,
      -1,    50,    32,    -1,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    -1,    -1,    45
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
     111,   112,   114,   132,   138,   139,   140,   141,   142,   143,
     144,   146,   147,   148,   149,   150,   151,   152,   156,   144,
       0,    59,   144,    60,   121,   122,   143,   120,   121,    14,
      39,   113,     4,     5,     6,   141,   117,   118,   119,   143,
      50,    91,    30,    48,   113,    33,    94,    95,    96,   154,
      22,    30,    32,    34,    35,    36,    37,    38,    39,    40,
      89,    94,   111,   114,   123,   124,   125,   153,   154,   155,
      91,   143,    46,   102,   103,   143,   144,    46,    34,    39,
      41,   154,   155,    48,    67,    75,    70,   112,    70,   111,
     141,   141,    31,    50,    76,    77,    80,   105,    46,    50,
      76,    44,    30,    46,    62,    48,    49,    31,    50,    21,
      48,   120,   121,   117,   118,   110,    21,    39,    49,    68,
      31,    69,    94,    94,   110,    30,    47,    49,    68,    94,
      94,    22,    90,    94,    51,    22,    30,    49,    51,    70,
      70,   111,    49,    68,    51,    94,    51,    48,    67,    31,
      46,    50,   108,   128,   133,   134,   135,   138,   142,   143,
     144,   146,   147,   148,   149,   150,   151,    31,    21,    49,
      46,    22,    94,    97,    37,    46,    50,   143,    41,    92,
      91,    70,    91,    37,    51,    97,    98,    77,    95,    51,
      97,    98,    99,   100,   154,    65,   144,    61,   143,    44,
     122,    91,    94,   143,    21,    48,    21,    39,    70,   112,
     143,   119,    50,    54,   104,   132,   138,   142,   146,   147,
     148,   149,   150,   151,    81,    82,    70,    94,    94,   154,
      94,    51,    22,    49,    94,    94,    94,    51,    51,    70,
      94,   123,   154,    50,    76,    91,   134,   108,   129,   130,
     131,   154,    47,    49,    68,    46,    91,   143,   143,   134,
      94,    22,    47,    49,    50,    22,    97,    51,    97,    98,
      99,    31,    50,    76,    78,    34,    50,   138,   143,    91,
      94,    76,    49,    68,   101,    49,   101,    47,    78,   101,
     101,    49,    68,    94,    37,    45,    66,   143,   153,    47,
      63,    51,    48,   143,   143,    45,    39,    49,    94,    49,
      49,    45,    30,    94,    94,    94,    22,    51,    51,    97,
      98,    47,    30,    49,    68,    51,   108,   108,   134,    47,
      47,    47,    94,    31,    94,    94,    94,    22,    47,    78,
     101,   101,    68,    91,    51,    97,    98,    99,    94,    37,
      99,    51,    51,    51,    51,   100,    51,   143,   153,    44,
      44,    62,    13,    15,    16,    64,    65,    48,    39,    49,
     143,    51,    13,    30,    84,    94,   143,    84,    94,    22,
      94,    76,   101,   101,   108,   129,    47,    31,    31,    47,
      91,    49,    47,    47,    94,    31,    51,    51,    51,    78,
     101,   101,    68,    51,    94,    68,    31,    78,    78,    78,
      44,    44,   113,   113,    44,    35,    36,    40,   126,   127,
     136,   126,    35,   115,   116,   137,    45,   143,    39,   143,
      29,    30,    45,    49,    83,    28,   143,    45,    94,    51,
      51,    91,    91,    31,    94,    31,    31,    47,    91,    78,
      78,    51,    51,    51,    91,   113,   113,   110,   110,    63,
      48,    49,   143,    48,    48,    68,   143,    39,    31,    94,
      94,    84,    94,    28,    78,    91,    51,    91,    91,    31,
      78,    78,   110,   110,    10,    73,   145,    73,    65,   127,
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
      79,    79,    79,    79,    79,    79,    79,    79,    79,    79,
      81,    80,    82,    80,    83,    83,    84,    84,    84,    84,
      84,    84,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    86,    86,    86,
      87,    87,    87,    88,    88,    88,    88,    88,    88,    89,
      89,    89,    89,    89,    90,    90,    90,    90,    90,    91,
      91,    91,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    92,    92,    92,    92,    93,    93,    94,
      95,    95,    96,    96,    96,    96,    96,    96,    97,    97,
      98,    98,    99,    99,   100,   101,   101,   102,   102,   103,
     103,   104,   104,   104,   104,   104,   104,   104,   104,   104,
     105,   106,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   109,   109,   110,   110,   111,   111,   112,   113,
     113,   113,   113,   113,   113,   114,   114,   114,   114,   114,
     115,   115,   116,   117,   117,   118,   118,   119,   119,   119,
     120,   120,   121,   121,   122,   122,   122,   123,   123,   124,
     124,   125,   125,   126,   126,   127,   127,   128,   129,   129,
     130,   130,   131,   131,   132,   132,   133,   133,   134,   134,
     135,   135,   136,   136,   136,   136,   137,   137,   138,   138,
     139,   139,   140,   140,   141,   142,   142,   142,   142,   142,
     142,   143,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   153,   153,   153,   153,   153,   153,   153,
     154,   155,   155,   156
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
       7,     7,     8,     6,     6,     7,     8,     8,     9,     2,
       2,     3,     5,     4,     2,     2,     3,     4,     5,     1,
       1,     1,     1,     5,     2,     4,     3,     4,     5,     7,
       4,     6,     7,     0,     2,     2,     4,     1,     3,     2,
       0,     2,     1,     3,     2,     3,     4,     5,     1,     3,
       2,     4,     1,     3,     2,     1,     3,     1,     3,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     5,     5,     0,     2,     1,     2,     3,     0,
       3,     5,     3,     5,     7,     3,     5,     3,     5,     7,
       1,     3,     4,     0,     1,     1,     3,     1,     3,     5,
       0,     1,     1,     3,     1,     3,     4,     3,     2,     1,
       3,     0,     2,     1,     3,     2,     4,     3,     3,     2,
       1,     3,     0,     2,     4,     5,     3,     4,     0,     2,
       1,     3,     0,     1,     1,     1,     0,     1,     1,     2,
       1,     2,     1,     2,     1,     1,     1,     2,     2,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1
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
#line 2193 "lang11d_tab.cpp"
    break;

  case 3: /* root: classextensions  */
#line 42 "lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; }
#line 2199 "lang11d_tab.cpp"
    break;

  case 4: /* root: INTERPRET cmdlinecode  */
#line 44 "lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 2; }
#line 2205 "lang11d_tab.cpp"
    break;

  case 5: /* classes: %empty  */
#line 47 "lang11d"
          { yyval = 0; }
#line 2211 "lang11d_tab.cpp"
    break;

  case 6: /* classes: classes classdef  */
#line 49 "lang11d"
                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2217 "lang11d_tab.cpp"
    break;

  case 8: /* classextensions: classextensions classextension  */
#line 54 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2223 "lang11d_tab.cpp"
    break;

  case 9: /* classdef: classname superclass '{' classvardecls methods '}'  */
#line 58 "lang11d"
                                { yyval = (intptr_t)newPyrClassNode((PyrSlotNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1], 0);
				}
#line 2231 "lang11d_tab.cpp"
    break;

  case 10: /* classdef: classname '[' optname ']' superclass '{' classvardecls methods '}'  */
#line 62 "lang11d"
                                { yyval = (intptr_t)newPyrClassNode((PyrSlotNode*)yyvsp[-8], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1],
					(PyrSlotNode*)yyvsp[-6]);
				}
#line 2240 "lang11d_tab.cpp"
    break;

  case 11: /* classextension: '+' classname '{' methods '}'  */
#line 69 "lang11d"
                                {
					yyval = (intptr_t)newPyrClassExtNode((PyrSlotNode*)yyvsp[-3], (PyrMethodNode*)yyvsp[-1]);
				}
#line 2248 "lang11d_tab.cpp"
    break;

  case 12: /* optname: %empty  */
#line 74 "lang11d"
                  { yyval = 0; }
#line 2254 "lang11d_tab.cpp"
    break;

  case 14: /* superclass: %empty  */
#line 78 "lang11d"
                  { yyval = 0; }
#line 2260 "lang11d_tab.cpp"
    break;

  case 15: /* superclass: ':' classname  */
#line 80 "lang11d"
                                { yyval = yyvsp[0]; }
#line 2266 "lang11d_tab.cpp"
    break;

  case 16: /* classvardecls: %empty  */
#line 83 "lang11d"
                  { yyval = 0; }
#line 2272 "lang11d_tab.cpp"
    break;

  case 17: /* classvardecls: classvardecls classvardecl  */
#line 85 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2278 "lang11d_tab.cpp"
    break;

  case 18: /* classvardecl: CLASSVAR rwslotdeflist ';'  */
#line 89 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varClass); }
#line 2284 "lang11d_tab.cpp"
    break;

  case 19: /* classvardecl: VAR rwslotdeflist ';'  */
#line 91 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varInst); }
#line 2290 "lang11d_tab.cpp"
    break;

  case 20: /* classvardecl: SC_CONST constdeflist ';'  */
#line 93 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varConst); }
#line 2296 "lang11d_tab.cpp"
    break;

  case 21: /* methods: %empty  */
#line 96 "lang11d"
                  { yyval = 0; }
#line 2302 "lang11d_tab.cpp"
    break;

  case 22: /* methods: methods methoddef  */
#line 98 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2308 "lang11d_tab.cpp"
    break;

  case 23: /* methoddef: name '{' argdecls funcvardecls primitive methbody '}'  */
#line 102 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2315 "lang11d_tab.cpp"
    break;

  case 24: /* methoddef: '*' name '{' argdecls funcvardecls primitive methbody '}'  */
#line 105 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2322 "lang11d_tab.cpp"
    break;

  case 25: /* methoddef: binop '{' argdecls funcvardecls primitive methbody '}'  */
#line 108 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2329 "lang11d_tab.cpp"
    break;

  case 26: /* methoddef: '*' binop '{' argdecls funcvardecls primitive methbody '}'  */
#line 111 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2336 "lang11d_tab.cpp"
    break;

  case 34: /* funcbody: exprseq funretval  */
#line 129 "lang11d"
                                { yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2342 "lang11d_tab.cpp"
    break;

  case 35: /* cmdlinecode: '(' argdecls1 funcvardecls1 funcbody ')'  */
#line 133 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2348 "lang11d_tab.cpp"
    break;

  case 36: /* cmdlinecode: '(' argdecls1 funcbody ')'  */
#line 135 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-2], NULL, (PyrParseNode*)yyvsp[-1], false); }
#line 2354 "lang11d_tab.cpp"
    break;

  case 37: /* cmdlinecode: '(' funcvardecls1 funcbody ')'  */
#line 137 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2360 "lang11d_tab.cpp"
    break;

  case 38: /* cmdlinecode: argdecls1 funcvardecls1 funcbody  */
#line 139 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-2], (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2366 "lang11d_tab.cpp"
    break;

  case 39: /* cmdlinecode: argdecls1 funcbody  */
#line 141 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-1], NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2372 "lang11d_tab.cpp"
    break;

  case 40: /* cmdlinecode: funcvardecls1 funcbody  */
#line 143 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2378 "lang11d_tab.cpp"
    break;

  case 41: /* cmdlinecode: funcbody  */
#line 145 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2384 "lang11d_tab.cpp"
    break;

  case 43: /* methbody: exprseq retval  */
#line 150 "lang11d"
                                { yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2390 "lang11d_tab.cpp"
    break;

  case 44: /* primitive: %empty  */
#line 153 "lang11d"
                  { yyval = 0; }
#line 2396 "lang11d_tab.cpp"
    break;

  case 45: /* primitive: primname optsemi  */
#line 155 "lang11d"
                                { yyval = yyvsp[-1]; }
#line 2402 "lang11d_tab.cpp"
    break;

  case 46: /* retval: %empty  */
#line 159 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode(NULL); }
#line 2408 "lang11d_tab.cpp"
    break;

  case 47: /* retval: '^' expr optsemi  */
#line 161 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); }
#line 2414 "lang11d_tab.cpp"
    break;

  case 48: /* funretval: %empty  */
#line 165 "lang11d"
                        { yyval = (intptr_t)newPyrBlockReturnNode(); }
#line 2420 "lang11d_tab.cpp"
    break;

  case 49: /* funretval: '^' expr optsemi  */
#line 167 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); }
#line 2426 "lang11d_tab.cpp"
    break;

  case 51: /* blocklist1: blocklist1 blocklistitem  */
#line 172 "lang11d"
                                {
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
				}
#line 2434 "lang11d_tab.cpp"
    break;

  case 54: /* blocklist: %empty  */
#line 181 "lang11d"
                        { yyval = 0; }
#line 2440 "lang11d_tab.cpp"
    break;

  case 56: /* msgsend: name blocklist1  */
#line 186 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2448 "lang11d_tab.cpp"
    break;

  case 57: /* msgsend: '(' binop2 ')' blocklist1  */
#line 190 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2456 "lang11d_tab.cpp"
    break;

  case 58: /* msgsend: name '(' ')' blocklist1  */
#line 194 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-3], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2464 "lang11d_tab.cpp"
    break;

  case 59: /* msgsend: name '(' arglist1 optkeyarglist ')' blocklist  */
#line 198 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2473 "lang11d_tab.cpp"
    break;

  case 60: /* msgsend: '(' binop2 ')' '(' ')' blocklist1  */
#line 203 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-4], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2481 "lang11d_tab.cpp"
    break;

  case 61: /* msgsend: '(' binop2 ')' '(' arglist1 optkeyarglist ')' blocklist  */
#line 207 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2490 "lang11d_tab.cpp"
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
#line 2512 "lang11d_tab.cpp"
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
#line 2529 "lang11d_tab.cpp"
    break;

  case 64: /* msgsend: classname '[' arrayelems ']'  */
#line 243 "lang11d"
                        { yyval = (intptr_t)newPyrDynListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 2535 "lang11d_tab.cpp"
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
#line 2550 "lang11d_tab.cpp"
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
#line 2565 "lang11d_tab.cpp"
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
#line 2580 "lang11d_tab.cpp"
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
#line 2597 "lang11d_tab.cpp"
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
#line 2622 "lang11d_tab.cpp"
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
#line 2635 "lang11d_tab.cpp"
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
#line 2648 "lang11d_tab.cpp"
    break;

  case 72: /* msgsend: expr '.' name '(' keyarglist1 optcomma ')' blocklist  */
#line 330 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-7],
					(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2657 "lang11d_tab.cpp"
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
#line 2674 "lang11d_tab.cpp"
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
#line 2692 "lang11d_tab.cpp"
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
#line 2716 "lang11d_tab.cpp"
    break;

  case 76: /* msgsend: expr '.' name '(' ')' blocklist  */
#line 386 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-5], NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2724 "lang11d_tab.cpp"
    break;

  case 77: /* msgsend: expr '.' name '(' arglist1 optkeyarglist ')' blocklist  */
#line 390 "lang11d"
                        {
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2734 "lang11d_tab.cpp"
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
#line 2756 "lang11d_tab.cpp"
    break;

  case 79: /* msgsend: expr '.' name blocklist  */
#line 414 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[-3], 0, (PyrParseNode*)yyvsp[0]);
			}
#line 2764 "lang11d_tab.cpp"
    break;

  case 80: /* $@1: %empty  */
#line 419 "lang11d"
                            { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 1); }
#line 2770 "lang11d_tab.cpp"
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
#line 2784 "lang11d_tab.cpp"
    break;

  case 82: /* $@2: %empty  */
#line 429 "lang11d"
                                  { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 2); }
#line 2790 "lang11d_tab.cpp"
    break;

  case 83: /* generator: '{' ';' exprseq $@2 ',' qual '}'  */
#line 430 "lang11d"
                        {
				yyval = yyvsp[-1];
			}
#line 2798 "lang11d_tab.cpp"
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
#line 2824 "lang11d_tab.cpp"
    break;

  case 85: /* nextqual: ',' qual  */
#line 458 "lang11d"
                                { yyval = yyvsp[0]; }
#line 2830 "lang11d_tab.cpp"
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
#line 2868 "lang11d_tab.cpp"
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
#line 2910 "lang11d_tab.cpp"
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
#line 2928 "lang11d_tab.cpp"
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
#line 2943 "lang11d_tab.cpp"
    break;

  case 90: /* qual: ':' ':' exprseq nextqual  */
#line 559 "lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
			}
#line 2951 "lang11d_tab.cpp"
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
#line 2978 "lang11d_tab.cpp"
    break;

  case 98: /* expr1: '(' exprseq ')'  */
#line 594 "lang11d"
                        {
				PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
				node->mParens = 1;
				yyval = yyvsp[-1];
			}
#line 2988 "lang11d_tab.cpp"
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
#line 3002 "lang11d_tab.cpp"
    break;

  case 100: /* expr1: '[' arrayelems ']'  */
#line 610 "lang11d"
                        { yyval = (intptr_t)newPyrDynListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 3008 "lang11d_tab.cpp"
    break;

  case 101: /* expr1: '(' valrange2 ')'  */
#line 612 "lang11d"
                        { yyval = yyvsp[-1]; }
#line 3014 "lang11d_tab.cpp"
    break;

  case 102: /* expr1: '(' ':' valrange3 ')'  */
#line 614 "lang11d"
                        { yyval = yyvsp[-1]; }
#line 3020 "lang11d_tab.cpp"
    break;

  case 103: /* expr1: '(' dictslotlist ')'  */
#line 616 "lang11d"
                        { yyval = (intptr_t)newPyrDynDictNode((PyrParseNode*)yyvsp[-1]); }
#line 3026 "lang11d_tab.cpp"
    break;

  case 104: /* expr1: pseudovar  */
#line 618 "lang11d"
                        { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3032 "lang11d_tab.cpp"
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
#line 3049 "lang11d_tab.cpp"
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
#line 3080 "lang11d_tab.cpp"
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
#line 3102 "lang11d_tab.cpp"
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
#line 3131 "lang11d_tab.cpp"
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
#line 3163 "lang11d_tab.cpp"
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
#line 3186 "lang11d_tab.cpp"
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
#line 3216 "lang11d_tab.cpp"
    break;

  case 113: /* valrangexd: expr '.' '[' arglist1 DOTDOT ']'  */
#line 783 "lang11d"
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
#line 3247 "lang11d_tab.cpp"
    break;

  case 114: /* valrangexd: expr '.' '[' DOTDOT exprseq ']'  */
#line 810 "lang11d"
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
#line 3269 "lang11d_tab.cpp"
    break;

  case 115: /* valrangexd: expr '.' '[' arglist1 DOTDOT exprseq ']'  */
#line 828 "lang11d"
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
#line 3298 "lang11d_tab.cpp"
    break;

  case 116: /* valrangexd: expr '.' '[' arglist1 DOTDOT ']' '=' expr  */
#line 853 "lang11d"
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
#line 3330 "lang11d_tab.cpp"
    break;

  case 117: /* valrangexd: expr '.' '[' DOTDOT exprseq ']' '=' expr  */
#line 881 "lang11d"
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
#line 3353 "lang11d_tab.cpp"
    break;

  case 118: /* valrangexd: expr '.' '[' arglist1 DOTDOT exprseq ']' '=' expr  */
#line 900 "lang11d"
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
#line 3383 "lang11d_tab.cpp"
    break;

  case 119: /* valrange2: exprseq DOTDOT  */
#line 928 "lang11d"
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
#line 3405 "lang11d_tab.cpp"
    break;

  case 120: /* valrange2: DOTDOT exprseq  */
#line 947 "lang11d"
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
#line 3427 "lang11d_tab.cpp"
    break;

  case 121: /* valrange2: exprseq DOTDOT exprseq  */
#line 966 "lang11d"
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
#line 3447 "lang11d_tab.cpp"
    break;

  case 122: /* valrange2: exprseq ',' exprseq DOTDOT exprseq  */
#line 983 "lang11d"
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
#line 3465 "lang11d_tab.cpp"
    break;

  case 123: /* valrange2: exprseq ',' exprseq DOTDOT  */
#line 997 "lang11d"
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
#line 3488 "lang11d_tab.cpp"
    break;

  case 124: /* valrange3: DOTDOT exprseq  */
#line 1018 "lang11d"
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
#line 3510 "lang11d_tab.cpp"
    break;

  case 125: /* valrange3: exprseq DOTDOT  */
#line 1037 "lang11d"
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
#line 3531 "lang11d_tab.cpp"
    break;

  case 126: /* valrange3: exprseq DOTDOT exprseq  */
#line 1055 "lang11d"
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
#line 3551 "lang11d_tab.cpp"
    break;

  case 127: /* valrange3: exprseq ',' exprseq DOTDOT  */
#line 1072 "lang11d"
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
#line 3571 "lang11d_tab.cpp"
    break;

  case 128: /* valrange3: exprseq ',' exprseq DOTDOT exprseq  */
#line 1088 "lang11d"
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
#line 3589 "lang11d_tab.cpp"
    break;

  case 132: /* expr: classname  */
#line 1106 "lang11d"
                            { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3595 "lang11d_tab.cpp"
    break;

  case 133: /* expr: expr '.' '[' arglist1 ']'  */
#line 1108 "lang11d"
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
#line 3612 "lang11d_tab.cpp"
    break;

  case 134: /* expr: '`' expr  */
#line 1121 "lang11d"
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
#line 3630 "lang11d_tab.cpp"
    break;

  case 135: /* expr: expr binop2 adverb expr  */
#line 1135 "lang11d"
                        {
				yyval = (intptr_t)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0], (PyrParseNode*)yyvsp[-1]);
			}
#line 3639 "lang11d_tab.cpp"
    break;

  case 136: /* expr: name '=' expr  */
#line 1140 "lang11d"
                        {
				yyval = (intptr_t)newPyrAssignNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0);
			}
#line 3647 "lang11d_tab.cpp"
    break;

  case 137: /* expr: '~' name '=' expr  */
#line 1144 "lang11d"
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
#line 3662 "lang11d_tab.cpp"
    break;

  case 138: /* expr: expr '.' name '=' expr  */
#line 1155 "lang11d"
                        {
				yyval = (intptr_t)newPyrSetterNode((PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3671 "lang11d_tab.cpp"
    break;

  case 139: /* expr: name '(' arglist1 optkeyarglist ')' '=' expr  */
#line 1160 "lang11d"
                        {
				if (yyvsp[-3] != 0) {
					error("Setter method called with keyword arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-3]);
					compileErrors++;
				}
				yyval = (intptr_t)newPyrSetterNode((PyrSlotNode*)yyvsp[-6],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3685 "lang11d_tab.cpp"
    break;

  case 140: /* expr: '#' mavars '=' expr  */
#line 1170 "lang11d"
                        {
				yyval = (intptr_t)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[0], 0);
			}
#line 3694 "lang11d_tab.cpp"
    break;

  case 141: /* expr: expr1 '[' arglist1 ']' '=' expr  */
#line 1175 "lang11d"
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
#line 3712 "lang11d_tab.cpp"
    break;

  case 142: /* expr: expr '.' '[' arglist1 ']' '=' expr  */
#line 1189 "lang11d"
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
#line 3730 "lang11d_tab.cpp"
    break;

  case 143: /* adverb: %empty  */
#line 1204 "lang11d"
          { yyval = 0; }
#line 3736 "lang11d_tab.cpp"
    break;

  case 144: /* adverb: '.' name  */
#line 1205 "lang11d"
                           { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3742 "lang11d_tab.cpp"
    break;

  case 145: /* adverb: '.' integer  */
#line 1206 "lang11d"
                              { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3748 "lang11d_tab.cpp"
    break;

  case 146: /* adverb: '.' '(' exprseq ')'  */
#line 1207 "lang11d"
                                      { yyval = yyvsp[-1]; }
#line 3754 "lang11d_tab.cpp"
    break;

  case 148: /* exprn: exprn ';' expr  */
#line 1212 "lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 3762 "lang11d_tab.cpp"
    break;

  case 150: /* arrayelems: %empty  */
#line 1220 "lang11d"
                  { yyval = 0; }
#line 3768 "lang11d_tab.cpp"
    break;

  case 151: /* arrayelems: arrayelems1 optcomma  */
#line 1222 "lang11d"
                          { yyval = yyvsp[-1]; }
#line 3774 "lang11d_tab.cpp"
    break;

  case 153: /* arrayelems1: exprseq ':' exprseq  */
#line 1227 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3780 "lang11d_tab.cpp"
    break;

  case 154: /* arrayelems1: keybinop exprseq  */
#line 1229 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 3789 "lang11d_tab.cpp"
    break;

  case 155: /* arrayelems1: arrayelems1 ',' exprseq  */
#line 1234 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3795 "lang11d_tab.cpp"
    break;

  case 156: /* arrayelems1: arrayelems1 ',' keybinop exprseq  */
#line 1236 "lang11d"
                                {
					PyrParseNode* elems;
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					elems = (PyrParseNode*)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], elems);
				}
#line 3806 "lang11d_tab.cpp"
    break;

  case 157: /* arrayelems1: arrayelems1 ',' exprseq ':' exprseq  */
#line 1243 "lang11d"
                                {
					PyrParseNode* elems;
					elems = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-4], elems);
				}
#line 3816 "lang11d_tab.cpp"
    break;

  case 159: /* arglist1: arglist1 ',' exprseq  */
#line 1252 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3822 "lang11d_tab.cpp"
    break;

  case 160: /* arglistv1: '*' exprseq  */
#line 1256 "lang11d"
                                { yyval = yyvsp[0]; }
#line 3828 "lang11d_tab.cpp"
    break;

  case 161: /* arglistv1: arglist1 ',' '*' exprseq  */
#line 1258 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]); }
#line 3834 "lang11d_tab.cpp"
    break;

  case 163: /* keyarglist1: keyarglist1 ',' keyarg  */
#line 1263 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3840 "lang11d_tab.cpp"
    break;

  case 164: /* keyarg: keybinop exprseq  */
#line 1267 "lang11d"
                                { yyval = (intptr_t)newPyrPushKeyArgNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 3846 "lang11d_tab.cpp"
    break;

  case 165: /* optkeyarglist: optcomma  */
#line 1270 "lang11d"
                           { yyval = 0; }
#line 3852 "lang11d_tab.cpp"
    break;

  case 166: /* optkeyarglist: ',' keyarglist1 optcomma  */
#line 1271 "lang11d"
                                                           { yyval = yyvsp[-1]; }
#line 3858 "lang11d_tab.cpp"
    break;

  case 167: /* mavars: mavarlist  */
#line 1275 "lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3864 "lang11d_tab.cpp"
    break;

  case 168: /* mavars: mavarlist ELLIPSIS name  */
#line 1277 "lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); }
#line 3870 "lang11d_tab.cpp"
    break;

  case 170: /* mavarlist: mavarlist ',' name  */
#line 1282 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3876 "lang11d_tab.cpp"
    break;

  case 171: /* slotliteral: integer  */
#line 1286 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3882 "lang11d_tab.cpp"
    break;

  case 172: /* slotliteral: floatp  */
#line 1287 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3888 "lang11d_tab.cpp"
    break;

  case 173: /* slotliteral: ascii  */
#line 1288 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3894 "lang11d_tab.cpp"
    break;

  case 174: /* slotliteral: string  */
#line 1289 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3900 "lang11d_tab.cpp"
    break;

  case 175: /* slotliteral: symbol  */
#line 1290 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3906 "lang11d_tab.cpp"
    break;

  case 176: /* slotliteral: trueobj  */
#line 1291 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3912 "lang11d_tab.cpp"
    break;

  case 177: /* slotliteral: falseobj  */
#line 1292 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3918 "lang11d_tab.cpp"
    break;

  case 178: /* slotliteral: nilobj  */
#line 1293 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3924 "lang11d_tab.cpp"
    break;

  case 179: /* slotliteral: listlit  */
#line 1294 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3930 "lang11d_tab.cpp"
    break;

  case 180: /* blockliteral: block  */
#line 1297 "lang11d"
                        { yyval = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3936 "lang11d_tab.cpp"
    break;

  case 181: /* pushname: name  */
#line 1300 "lang11d"
                                { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3942 "lang11d_tab.cpp"
    break;

  case 182: /* pushliteral: integer  */
#line 1303 "lang11d"
                                { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3948 "lang11d_tab.cpp"
    break;

  case 183: /* pushliteral: floatp  */
#line 1304 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3954 "lang11d_tab.cpp"
    break;

  case 184: /* pushliteral: ascii  */
#line 1305 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3960 "lang11d_tab.cpp"
    break;

  case 185: /* pushliteral: string  */
#line 1306 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3966 "lang11d_tab.cpp"
    break;

  case 186: /* pushliteral: symbol  */
#line 1307 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3972 "lang11d_tab.cpp"
    break;

  case 187: /* pushliteral: trueobj  */
#line 1308 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3978 "lang11d_tab.cpp"
    break;

  case 188: /* pushliteral: falseobj  */
#line 1309 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3984 "lang11d_tab.cpp"
    break;

  case 189: /* pushliteral: nilobj  */
#line 1310 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3990 "lang11d_tab.cpp"
    break;

  case 190: /* pushliteral: listlit  */
#line 1311 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3996 "lang11d_tab.cpp"
    break;

  case 191: /* listliteral: integer  */
#line 1314 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4002 "lang11d_tab.cpp"
    break;

  case 192: /* listliteral: floatp  */
#line 1315 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4008 "lang11d_tab.cpp"
    break;

  case 193: /* listliteral: ascii  */
#line 1316 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4014 "lang11d_tab.cpp"
    break;

  case 194: /* listliteral: string  */
#line 1317 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4020 "lang11d_tab.cpp"
    break;

  case 195: /* listliteral: symbol  */
#line 1318 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4026 "lang11d_tab.cpp"
    break;

  case 196: /* listliteral: name  */
#line 1319 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4032 "lang11d_tab.cpp"
    break;

  case 197: /* listliteral: trueobj  */
#line 1320 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4038 "lang11d_tab.cpp"
    break;

  case 198: /* listliteral: falseobj  */
#line 1321 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4044 "lang11d_tab.cpp"
    break;

  case 199: /* listliteral: nilobj  */
#line 1322 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4050 "lang11d_tab.cpp"
    break;

  case 200: /* listliteral: listlit2  */
#line 1323 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 4056 "lang11d_tab.cpp"
    break;

  case 201: /* listliteral: dictlit2  */
#line 1324 "lang11d"
                                    { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 4062 "lang11d_tab.cpp"
    break;

  case 202: /* block: '{' argdecls funcvardecls funcbody '}'  */
#line 1328 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], false); }
#line 4069 "lang11d_tab.cpp"
    break;

  case 203: /* block: BEGINCLOSEDFUNC argdecls funcvardecls funcbody '}'  */
#line 1331 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], true); }
#line 4076 "lang11d_tab.cpp"
    break;

  case 204: /* funcvardecls: %empty  */
#line 1335 "lang11d"
                  { yyval = 0; }
#line 4082 "lang11d_tab.cpp"
    break;

  case 205: /* funcvardecls: funcvardecls funcvardecl  */
#line 1337 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 4088 "lang11d_tab.cpp"
    break;

  case 207: /* funcvardecls1: funcvardecls1 funcvardecl  */
#line 1342 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 4094 "lang11d_tab.cpp"
    break;

  case 208: /* funcvardecl: VAR vardeflist ';'  */
#line 1346 "lang11d"
                                { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varLocal); }
#line 4100 "lang11d_tab.cpp"
    break;

  case 209: /* argdecls: %empty  */
#line 1349 "lang11d"
                  { yyval = 0; }
#line 4106 "lang11d_tab.cpp"
    break;

  case 210: /* argdecls: ARG vardeflist ';'  */
#line 1351 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4114 "lang11d_tab.cpp"
    break;

  case 211: /* argdecls: ARG vardeflist0 ELLIPSIS name ';'  */
#line 1355 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4122 "lang11d_tab.cpp"
    break;

  case 212: /* argdecls: '|' slotdeflist '|'  */
#line 1359 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4130 "lang11d_tab.cpp"
    break;

  case 213: /* argdecls: '|' slotdeflist0 ELLIPSIS name '|'  */
#line 1363 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4138 "lang11d_tab.cpp"
    break;

  case 214: /* argdecls: '|' slotdeflist0 ELLIPSIS name ',' name '|'  */
#line 1367 "lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 4146 "lang11d_tab.cpp"
    break;

  case 215: /* argdecls1: ARG vardeflist ';'  */
#line 1373 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4154 "lang11d_tab.cpp"
    break;

  case 216: /* argdecls1: ARG vardeflist0 ELLIPSIS name ';'  */
#line 1377 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4162 "lang11d_tab.cpp"
    break;

  case 217: /* argdecls1: '|' slotdeflist '|'  */
#line 1381 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4170 "lang11d_tab.cpp"
    break;

  case 218: /* argdecls1: '|' slotdeflist0 ELLIPSIS name '|'  */
#line 1385 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4178 "lang11d_tab.cpp"
    break;

  case 219: /* argdecls1: '|' slotdeflist0 ELLIPSIS name ',' name '|'  */
#line 1389 "lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 4186 "lang11d_tab.cpp"
    break;

  case 221: /* constdeflist: constdeflist optcomma constdef  */
#line 1397 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4192 "lang11d_tab.cpp"
    break;

  case 222: /* constdef: rspec name '=' slotliteral  */
#line 1401 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4198 "lang11d_tab.cpp"
    break;

  case 223: /* slotdeflist0: %empty  */
#line 1404 "lang11d"
                  { yyval = 0; }
#line 4204 "lang11d_tab.cpp"
    break;

  case 226: /* slotdeflist: slotdeflist optcomma slotdef  */
#line 1410 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4210 "lang11d_tab.cpp"
    break;

  case 227: /* slotdef: name  */
#line 1414 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4216 "lang11d_tab.cpp"
    break;

  case 228: /* slotdef: name optequal slotliteral  */
#line 1416 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4222 "lang11d_tab.cpp"
    break;

  case 229: /* slotdef: name optequal '(' exprseq ')'  */
#line 1418 "lang11d"
                                {
					PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
					node->mParens = 1;
					yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-4], node, 0);
				}
#line 4232 "lang11d_tab.cpp"
    break;

  case 230: /* vardeflist0: %empty  */
#line 1425 "lang11d"
                  { yyval = 0; }
#line 4238 "lang11d_tab.cpp"
    break;

  case 233: /* vardeflist: vardeflist ',' vardef  */
#line 1431 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4244 "lang11d_tab.cpp"
    break;

  case 234: /* vardef: name  */
#line 1435 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4250 "lang11d_tab.cpp"
    break;

  case 235: /* vardef: name '=' expr  */
#line 1437 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4256 "lang11d_tab.cpp"
    break;

  case 236: /* vardef: name '(' exprseq ')'  */
#line 1439 "lang11d"
                                {
									PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
									node->mParens = 1;
									yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-3], node, 0);
								}
#line 4266 "lang11d_tab.cpp"
    break;

  case 237: /* dictslotdef: exprseq ':' exprseq  */
#line 1447 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4272 "lang11d_tab.cpp"
    break;

  case 238: /* dictslotdef: keybinop exprseq  */
#line 1449 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 4281 "lang11d_tab.cpp"
    break;

  case 240: /* dictslotlist1: dictslotlist1 ',' dictslotdef  */
#line 1457 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4287 "lang11d_tab.cpp"
    break;

  case 241: /* dictslotlist: %empty  */
#line 1460 "lang11d"
                  { yyval = 0; }
#line 4293 "lang11d_tab.cpp"
    break;

  case 244: /* rwslotdeflist: rwslotdeflist ',' rwslotdef  */
#line 1466 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4299 "lang11d_tab.cpp"
    break;

  case 245: /* rwslotdef: rwspec name  */
#line 1470 "lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); }
#line 4305 "lang11d_tab.cpp"
    break;

  case 246: /* rwslotdef: rwspec name '=' slotliteral  */
#line 1472 "lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4311 "lang11d_tab.cpp"
    break;

  case 247: /* dictlit2: '(' litdictslotlist ')'  */
#line 1476 "lang11d"
                                { yyval = (intptr_t)newPyrLitDictNode((PyrParseNode*)yyvsp[-1]); }
#line 4317 "lang11d_tab.cpp"
    break;

  case 248: /* litdictslotdef: listliteral ':' listliteral  */
#line 1480 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4323 "lang11d_tab.cpp"
    break;

  case 249: /* litdictslotdef: keybinop listliteral  */
#line 1482 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 4332 "lang11d_tab.cpp"
    break;

  case 251: /* litdictslotlist1: litdictslotlist1 ',' litdictslotdef  */
#line 1490 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4338 "lang11d_tab.cpp"
    break;

  case 252: /* litdictslotlist: %empty  */
#line 1493 "lang11d"
                  { yyval = 0; }
#line 4344 "lang11d_tab.cpp"
    break;

  case 254: /* listlit: '#' '[' literallistc ']'  */
#line 1500 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 4350 "lang11d_tab.cpp"
    break;

  case 255: /* listlit: '#' classname '[' literallistc ']'  */
#line 1502 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4356 "lang11d_tab.cpp"
    break;

  case 256: /* listlit2: '[' literallistc ']'  */
#line 1506 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 4362 "lang11d_tab.cpp"
    break;

  case 257: /* listlit2: classname '[' literallistc ']'  */
#line 1508 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4368 "lang11d_tab.cpp"
    break;

  case 258: /* literallistc: %empty  */
#line 1511 "lang11d"
                  { yyval = 0; }
#line 4374 "lang11d_tab.cpp"
    break;

  case 261: /* literallist1: literallist1 ',' listliteral  */
#line 1517 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4380 "lang11d_tab.cpp"
    break;

  case 262: /* rwspec: %empty  */
#line 1520 "lang11d"
           { yyval = rwPrivate; }
#line 4386 "lang11d_tab.cpp"
    break;

  case 263: /* rwspec: '<'  */
#line 1522 "lang11d"
                        { yyval = rwReadOnly; }
#line 4392 "lang11d_tab.cpp"
    break;

  case 264: /* rwspec: READWRITEVAR  */
#line 1524 "lang11d"
                        { yyval = rwReadWrite; }
#line 4398 "lang11d_tab.cpp"
    break;

  case 265: /* rwspec: '>'  */
#line 1526 "lang11d"
                        { yyval = rwWriteOnly; }
#line 4404 "lang11d_tab.cpp"
    break;

  case 266: /* rspec: %empty  */
#line 1529 "lang11d"
           { yyval = rwPrivate; }
#line 4410 "lang11d_tab.cpp"
    break;

  case 267: /* rspec: '<'  */
#line 1531 "lang11d"
                        { yyval = rwReadOnly; }
#line 4416 "lang11d_tab.cpp"
    break;

  case 268: /* integer: INTEGER  */
#line 1534 "lang11d"
                  { yyval = zzval; }
#line 4422 "lang11d_tab.cpp"
    break;

  case 269: /* integer: '-' INTEGER  */
#line 1536 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawInt(&node->mSlot));
				yyval = zzval;
			}
#line 4433 "lang11d_tab.cpp"
    break;

  case 270: /* floatr: SC_FLOAT  */
#line 1544 "lang11d"
                   { yyval = zzval; }
#line 4439 "lang11d_tab.cpp"
    break;

  case 271: /* floatr: '-' SC_FLOAT  */
#line 1546 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawFloat(&node->mSlot));
				yyval = zzval;
			}
#line 4450 "lang11d_tab.cpp"
    break;

  case 272: /* accidental: ACCIDENTAL  */
#line 1554 "lang11d"
                        { yyval = zzval; }
#line 4456 "lang11d_tab.cpp"
    break;

  case 273: /* accidental: '-' ACCIDENTAL  */
#line 1556 "lang11d"
                                {
					PyrSlotNode *node;
					double intval, fracval;
					node = (PyrSlotNode*)zzval;
					intval = floor(slotRawFloat(&node->mSlot) + 0.5);
					fracval = slotRawFloat(&node->mSlot) - intval;
					SetRaw(&node->mSlot, -intval + fracval);
					yyval = zzval;
				}
#line 4470 "lang11d_tab.cpp"
    break;

  case 274: /* pie: PIE  */
#line 1566 "lang11d"
                      { yyval = zzval; }
#line 4476 "lang11d_tab.cpp"
    break;

  case 277: /* floatp: floatr pie  */
#line 1572 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)yyvsp[-1];
				SetRaw(&node->mSlot, slotRawFloat(&node->mSlot) * pi);
			}
#line 4486 "lang11d_tab.cpp"
    break;

  case 278: /* floatp: integer pie  */
#line 1578 "lang11d"
                        {
				PyrSlotNode *node;
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = slotRawInt(&node->mSlot);
				SetFloat(&node->mSlot, ival * pi);
			}
#line 4498 "lang11d_tab.cpp"
    break;

  case 279: /* floatp: pie  */
#line 1586 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, pi);
				yyval = zzval;
			}
#line 4509 "lang11d_tab.cpp"
    break;

  case 280: /* floatp: '-' pie  */
#line 1593 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, -pi);
				yyval = zzval;
			}
#line 4520 "lang11d_tab.cpp"
    break;

  case 281: /* name: NAME  */
#line 1601 "lang11d"
                       { yyval = zzval; }
#line 4526 "lang11d_tab.cpp"
    break;

  case 282: /* name: WHILE  */
#line 1602 "lang11d"
                                { yyval = zzval; }
#line 4532 "lang11d_tab.cpp"
    break;

  case 283: /* classname: CLASSNAME  */
#line 1605 "lang11d"
                                    { yyval = zzval; }
#line 4538 "lang11d_tab.cpp"
    break;

  case 284: /* primname: PRIMITIVENAME  */
#line 1608 "lang11d"
                                        { yyval = zzval; }
#line 4544 "lang11d_tab.cpp"
    break;

  case 285: /* trueobj: TRUEOBJ  */
#line 1611 "lang11d"
                          { yyval = zzval; }
#line 4550 "lang11d_tab.cpp"
    break;

  case 286: /* falseobj: FALSEOBJ  */
#line 1614 "lang11d"
                           { yyval = zzval; }
#line 4556 "lang11d_tab.cpp"
    break;

  case 287: /* nilobj: NILOBJ  */
#line 1617 "lang11d"
                         { yyval = zzval; }
#line 4562 "lang11d_tab.cpp"
    break;

  case 288: /* ascii: ASCII  */
#line 1620 "lang11d"
                        { yyval = zzval; }
#line 4568 "lang11d_tab.cpp"
    break;

  case 289: /* symbol: SYMBOL  */
#line 1623 "lang11d"
                         { yyval = zzval; }
#line 4574 "lang11d_tab.cpp"
    break;

  case 290: /* string: STRING  */
#line 1626 "lang11d"
                         { yyval = zzval; }
#line 4580 "lang11d_tab.cpp"
    break;

  case 291: /* pseudovar: PSEUDOVAR  */
#line 1629 "lang11d"
                            { yyval = zzval; }
#line 4586 "lang11d_tab.cpp"
    break;

  case 292: /* binop: BINOP  */
#line 1632 "lang11d"
                { yyval = zzval; }
#line 4592 "lang11d_tab.cpp"
    break;

  case 293: /* binop: READWRITEVAR  */
#line 1633 "lang11d"
                               { yyval = zzval; }
#line 4598 "lang11d_tab.cpp"
    break;

  case 294: /* binop: '<'  */
#line 1634 "lang11d"
                       { yyval = zzval; }
#line 4604 "lang11d_tab.cpp"
    break;

  case 295: /* binop: '>'  */
#line 1635 "lang11d"
                       { yyval = zzval; }
#line 4610 "lang11d_tab.cpp"
    break;

  case 296: /* binop: '-'  */
#line 1636 "lang11d"
                       { yyval = zzval; }
#line 4616 "lang11d_tab.cpp"
    break;

  case 297: /* binop: '*'  */
#line 1637 "lang11d"
                       { yyval = zzval; }
#line 4622 "lang11d_tab.cpp"
    break;

  case 298: /* binop: '+'  */
#line 1638 "lang11d"
                       { yyval = zzval; }
#line 4628 "lang11d_tab.cpp"
    break;

  case 299: /* binop: '|'  */
#line 1639 "lang11d"
                       { yyval = zzval; }
#line 4634 "lang11d_tab.cpp"
    break;

  case 300: /* keybinop: KEYBINOP  */
#line 1642 "lang11d"
                    { yyval = zzval; }
#line 4640 "lang11d_tab.cpp"
    break;

  case 303: /* curryarg: CURRYARG  */
#line 1649 "lang11d"
                    { yyval = zzval; }
#line 4646 "lang11d_tab.cpp"
    break;


#line 4650 "lang11d_tab.cpp"

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

