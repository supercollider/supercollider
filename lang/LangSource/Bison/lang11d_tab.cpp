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
#define YYLAST   1948

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  102
/* YYNRULES -- Number of rules.  */
#define YYNRULES  304
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  573

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
     311,   320,   329,   334,   348,   362,   376,   398,   402,   408,
     426,   432,   432,   442,   442,   449,   470,   474,   508,   546,
     560,   571,   575,   600,   601,   602,   603,   604,   605,   606,
     612,   622,   624,   626,   628,   630,   632,   645,   648,   675,
     693,   720,   748,   767,   795,   822,   840,   865,   893,   912,
     940,   959,   978,   995,  1009,  1030,  1049,  1067,  1084,  1100,
    1116,  1117,  1118,  1119,  1120,  1133,  1147,  1152,  1156,  1167,
    1172,  1182,  1187,  1201,  1217,  1218,  1219,  1220,  1223,  1224,
    1230,  1233,  1234,  1238,  1239,  1241,  1246,  1248,  1255,  1263,
    1264,  1268,  1270,  1274,  1275,  1279,  1283,  1284,  1287,  1289,
    1293,  1294,  1299,  1300,  1301,  1302,  1303,  1304,  1305,  1306,
    1307,  1310,  1313,  1316,  1317,  1318,  1319,  1320,  1321,  1322,
    1323,  1324,  1327,  1328,  1329,  1330,  1331,  1332,  1333,  1334,
    1335,  1336,  1337,  1340,  1343,  1348,  1349,  1353,  1354,  1358,
    1362,  1363,  1367,  1371,  1375,  1379,  1385,  1389,  1393,  1397,
    1401,  1408,  1409,  1413,  1417,  1418,  1421,  1422,  1426,  1428,
    1430,  1438,  1439,  1442,  1443,  1447,  1449,  1451,  1459,  1461,
    1468,  1469,  1473,  1474,  1477,  1478,  1482,  1484,  1488,  1492,
    1494,  1501,  1502,  1506,  1507,  1512,  1514,  1518,  1520,  1524,
    1525,  1528,  1529,  1533,  1534,  1536,  1538,  1542,  1543,  1547,
    1548,  1557,  1558,  1567,  1568,  1579,  1582,  1583,  1584,  1590,
    1598,  1605,  1614,  1615,  1618,  1621,  1624,  1627,  1630,  1633,
    1636,  1639,  1642,  1645,  1646,  1647,  1648,  1649,  1650,  1651,
    1652,  1655,  1658,  1659,  1662
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

#define YYPACT_NINF (-370)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-301)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     107,   969,    19,    81,    19,    58,  -370,  -370,  -370,  -370,
    -370,  -370,  -370,  -370,  -370,  -370,    51,    51,  -370,  -370,
    -370,  -370,  -370,    52,  -370,   338,    51,  1801,    68,  1437,
     862,  1801,    51,    54,  -370,  -370,  -370,  -370,  -370,    57,
    -370,  -370,  -370,  1907,    77,   103,  -370,  -370,  -370,  -370,
    1177,  -370,  1177,  -370,   106,   106,  -370,  -370,  -370,   195,
     290,  -370,  -370,  -370,  -370,  -370,  -370,  -370,  -370,    95,
    -370,  -370,    65,  -370,    -5,  -370,    36,   149,   127,    51,
      51,  -370,  -370,  -370,  -370,  -370,   183,     3,  -370,   310,
     917,  -370,  1801,  1801,  -370,  -370,   180,   184,   178,  1801,
    1801,  1489,  -370,   338,  -370,  -370,  -370,  -370,    20,  -370,
     189,   166,  1177,  1177,  -370,   186,   192,  -370,  1801,   200,
    1890,   231,  1887,   233,    25,  -370,   222,  1541,  -370,  -370,
      22,  -370,   240,  1801,  -370,  -370,  -370,  -370,  -370,  1177,
    -370,  -370,  1801,  1229,    90,  -370,  -370,  -370,  1437,  1021,
      90,  -370,    19,    51,   230,  -370,    51,  1801,  1801,    51,
    -370,   249,   229,   262,    87,  1177,    51,  -370,  -370,    51,
    -370,   758,  -370,  -370,  1177,  1801,  -370,  1437,  -370,  -370,
    -370,  1801,   248,    12,  -370,  1801,  1801,  1801,  -370,   256,
     266,  1177,  1437,  -370,  -370,  -370,    26,  -370,  -370,  1801,
    1887,  1853,  -370,  -370,  -370,   247,   274,   106,  -370,  -370,
     250,  -370,  -370,  -370,  -370,  -370,  -370,  1801,    51,    51,
    1887,  1801,  -370,    97,    29,  1593,  1073,   304,    44,  1801,
    1907,  -370,  1907,  1801,    90,   289,   296,  -370,   300,    90,
     289,   296,   302,  -370,  1801,   661,  -370,   309,  -370,  -370,
    -370,  1907,   307,   312,    51,  -370,    51,  -370,   319,  -370,
      -4,  -370,  1801,    18,  -370,  -370,   106,  -370,  -370,  -370,
    -370,  -370,  -370,  -370,   306,   320,   326,  -370,   342,  1801,
    -370,  -370,  1801,  1801,  -370,  -370,   351,  -370,  -370,   330,
     353,  -370,  1801,  1281,    90,  1907,   337,   359,  -370,   341,
     340,  1887,  -370,  1887,  -370,  1887,  1907,  -370,  -370,   345,
     348,  1645,   366,  1801,  1801,   349,  1801,   181,    90,   289,
     296,   302,  1801,  1125,    90,  -370,   394,  1801,  -370,  -370,
     360,  -370,    90,  1333,  -370,   352,   369,   356,  -370,  -370,
     357,   363,   369,   365,  -370,   819,  -370,  -370,   376,   380,
     395,   317,  -370,  -370,   362,    89,  -370,  -370,    51,   377,
    1385,  1385,  -370,  1801,  -370,  -370,   405,  1801,  -370,    90,
     289,   296,  -370,  1887,  1853,  -370,  -370,  -370,  -370,   384,
    -370,   404,   410,   396,  1801,  -370,   132,  1801,   399,  1697,
     411,  -370,   397,   398,   400,  1907,    90,   289,   296,   302,
     406,  1801,   302,   245,  -370,    90,  -370,  -370,    90,   418,
     422,    52,    52,   423,   112,   112,   415,  -370,   688,  -370,
    -370,    51,   416,  -370,    51,   258,   429,   419,   213,   435,
    -370,  1801,  -370,    90,   433,   438,  -370,  -370,  -370,  1801,
    1801,   455,  1907,  -370,   140,   459,   461,   449,  1801,    90,
    -370,    90,  -370,   448,   454,   463,  -370,  -370,  -370,  1801,
    -370,  -370,  -370,    52,    52,  -370,  -370,  -370,  -370,  -370,
    -370,   244,  -370,    51,   267,  -370,   273,  -370,    51,  -370,
     464,  -370,   469,  1801,  1801,  -370,  1385,  -370,  1801,   478,
    -370,  -370,    90,  -370,  1907,  1907,  1801,  -370,  1801,  1801,
     479,  1907,  -370,  -370,    90,  -370,    90,  1907,  -370,  -370,
     172,   172,   317,  -370,   112,   484,  -370,  -370,   415,   485,
    -370,  1801,   419,   419,  -370,   419,  1801,  -370,  1907,  1907,
    1907,  1801,  -370,  -370,   172,   172,  -370,  1749,   471,  1749,
     804,  -370,   780,  -370,   780,   419,  -370,  -370,  -370,   419,
    1907,  1749,  1749,  1801,   477,  -370,   472,  -370,   480,  -370,
    -370,  -370,  -370,  -370,   482,   486,  1890,  -370,  -370,  -370,
    -370,  -370,  -370
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       5,    48,     0,     0,     2,     3,     7,   282,   269,   271,
     273,   290,   291,   289,   284,   304,     0,   231,   288,   286,
     287,   292,   275,   210,   283,     0,   224,     0,   210,   151,
     242,     0,     0,     0,    41,     4,    33,    98,    95,   130,
     107,   132,   131,   148,    27,    48,    94,    96,    93,   181,
      48,   207,    48,   191,   183,   276,   277,   280,   184,   182,
     133,   188,   189,   190,   185,   187,   186,   105,    97,     0,
       1,     6,    14,     8,     0,   233,   235,     0,   232,   231,
     224,   205,   270,   272,   274,   281,     0,    29,   226,    31,
     242,   135,     0,     0,   205,   301,   153,     0,    29,     0,
       0,     0,   293,   297,   295,   296,   298,   299,   224,   294,
       0,     0,    48,    48,   240,    29,     0,   302,   303,     0,
      27,   100,   259,     0,   168,   170,     0,     0,   297,   300,
       0,   303,   144,    28,   150,    34,    40,   208,    39,    48,
     279,   278,     0,     0,    56,    50,    53,    52,   151,     0,
      65,    21,     0,    12,     0,   209,     0,     0,     0,     0,
     216,     0,   232,     0,    29,    48,     0,   218,    30,     0,
      32,     0,    81,    83,    48,     0,   101,    30,   152,   155,
     121,     0,     0,     0,   102,   120,     0,     0,    99,     0,
       0,    48,    30,   243,   104,   239,     0,    28,    49,     0,
     259,   253,   261,   202,   201,     0,    29,   192,   193,   197,
       0,   198,   199,   200,   194,   196,   195,     0,     0,     0,
     259,     0,   159,     0,     0,     0,     0,    54,     0,     0,
     149,    38,   137,     0,     0,    29,    29,    51,     0,    54,
      29,    29,    29,   163,     0,     0,    15,     0,    13,    16,
     234,   236,     0,     0,     0,   211,     0,   213,     0,   206,
       0,   227,     0,     0,   229,   180,   172,   173,   177,   178,
     179,   174,   176,   175,     0,     0,     0,   154,   156,     0,
     125,   103,   126,     0,   122,   238,     0,    37,    36,     0,
       0,   241,     0,     0,    57,   138,     0,     0,   251,    29,
       0,     0,   255,    30,   260,   259,   141,   169,   171,     0,
       0,     0,   106,     0,     0,     0,     0,     0,    54,    29,
      29,    29,     0,     0,    55,    80,     0,     0,   146,   145,
     136,   161,    58,    30,   166,     0,    30,     0,    64,    66,
       0,     0,    30,     0,   165,   298,    11,    22,     0,     0,
      14,    21,   237,   217,     0,     0,   204,   219,     0,     0,
       0,     0,   203,     0,   157,   127,     0,   124,    35,     0,
      29,    29,   257,     0,    30,   254,   248,   250,   262,     0,
     256,   109,   108,     0,     0,   160,     0,     0,     0,     0,
     134,    70,     0,     0,     0,   139,    54,    29,    29,    29,
       0,     0,    29,    54,    62,    54,    69,   164,    54,     0,
       0,   210,   210,     0,   263,   263,   267,    17,     0,   212,
     214,     0,     0,   230,     0,     0,     0,    85,   182,     0,
     158,   128,   123,    60,     0,     0,   249,   252,   258,     0,
       0,   110,   142,    75,     0,   115,   114,     0,     0,    54,
      76,    54,    77,     0,     0,     0,   147,   162,   167,     0,
      59,    68,    67,   210,   210,   205,   205,    16,   264,   266,
     265,     0,   244,     0,     0,   268,    29,   221,     0,     9,
       0,   220,     0,     0,     0,    82,     0,    90,     0,     0,
      84,   129,    54,    63,   112,   111,     0,    74,     0,     0,
     116,   143,    73,    71,    54,    79,    54,   140,   205,   205,
      44,    44,    21,    19,   263,   246,    18,    20,   267,     0,
     215,     0,    85,    85,    86,    85,     0,    61,   113,   118,
     117,     0,    78,    72,    44,    44,   285,    46,    27,    46,
       0,   245,     0,   222,     0,    85,    92,    91,    87,    85,
     119,    46,    46,     0,     0,    42,    46,    45,     0,    10,
     247,   223,    89,    88,     0,     0,    27,    23,    43,    25,
      24,    26,    47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -370,  -370,  -370,  -370,  -370,   524,  -370,   188,    67,  -370,
    -343,  -370,  -119,   462,  -370,    47,  -370,  -279,  -369,   -20,
     494,   -40,  -132,   237,  -370,   226,  -370,  -370,  -270,  -346,
    -370,  -370,  -370,  -370,  -370,  -370,   -27,  -370,  -370,   219,
     392,  -370,  -116,  -136,  -139,   199,   -73,  -370,  -370,  -333,
     243,  -370,  -370,  -160,  -370,   -91,   -24,    10,   -26,   513,
    -370,    30,   467,   473,   382,   475,   -10,   401,   364,  -370,
    -370,   144,    48,  -370,   190,  -370,  -370,  -162,  -370,  -184,
    -370,  -370,  -370,  -108,  -370,  -370,     1,   -83,     5,   452,
    -370,   -69,   -47,   -21,   287,   293,   308,  -370,  -240,   488,
     -13,  -370
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     3,     4,     5,    71,     6,   247,   154,   351,   417,
     245,   347,   134,   334,   171,    34,    35,   554,   537,   555,
      36,   324,   145,   325,    37,    38,   274,   275,   487,   426,
      39,    40,    41,    42,   110,   182,    43,   229,    44,    45,
      97,    98,   223,   236,   402,   243,   335,   123,   124,   264,
      46,    47,    48,   202,    49,   165,    50,   259,    81,    52,
     476,   477,    86,    87,    88,    77,    74,    75,   114,   115,
     116,   471,   472,   203,   298,   299,   300,    53,   204,   205,
     206,   473,   478,    54,    55,    56,    57,    58,    59,    60,
     538,    61,    62,    63,    64,    65,    66,    67,   117,   131,
     132,    68
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      91,   198,    94,   174,   120,   349,   112,    78,   418,   265,
     242,    51,   237,   241,   207,   429,   296,   119,   237,   144,
     150,    76,    76,     7,  -225,     7,    85,   235,   139,    14,
      14,    89,     7,   240,   282,   357,   309,   121,   125,   208,
      51,   297,   167,   155,   156,   358,   218,     7,     8,    24,
      23,    24,   168,   211,     7,   140,   141,     7,    24,   224,
     137,   283,    51,   266,   122,    14,    79,   157,   225,   162,
      28,  -300,   226,    24,   219,   212,   293,   119,   326,   314,
      24,    70,    79,    24,    76,    89,   158,   321,   267,   191,
     320,    80,   207,   207,   327,   152,     2,   136,    92,   138,
     122,   213,   268,   127,    85,   410,   230,    80,  -225,   317,
     319,   153,   207,    89,    23,   232,    93,   208,   208,   311,
     328,   379,   137,    51,   269,   133,   257,   209,   420,    22,
     251,   211,   211,     1,    28,   227,   168,   208,   421,   151,
     524,   377,   539,   378,   312,     2,   313,   468,   469,   137,
     270,   211,   470,   212,   212,    31,   294,   371,   248,   189,
     190,    76,   237,   337,   253,   551,   552,   340,   341,   540,
     159,   260,   295,   212,    89,   160,   156,   370,   349,   213,
     213,   313,   536,   443,   399,    16,   231,   398,   185,   313,
     306,   497,   237,   207,   332,   207,   186,   207,   386,   213,
     237,   137,   330,   389,   166,   209,   209,   397,   140,   560,
     175,   561,   258,   436,   297,   187,     7,   188,   208,    23,
     208,   276,   208,   307,   308,   209,   142,   177,   390,   315,
     313,   176,   211,   329,   211,   192,   211,    23,   289,    28,
     184,   488,    24,   194,   142,   143,   392,   393,    96,   111,
     348,   196,   546,   547,   212,   548,   212,    28,   212,   354,
     558,   355,   199,   143,   217,   207,   207,   140,   220,    23,
     254,   444,   564,   565,   249,   562,   459,   255,   156,   563,
     213,   228,   213,   256,   213,   146,   146,   483,   484,    28,
     208,   208,   513,   514,   302,   395,   305,   434,   435,   281,
     349,   237,   147,   147,   211,   211,   209,   287,   209,   111,
     209,   172,   173,  -228,    23,   516,   514,   288,   179,   180,
     183,   517,   168,   303,   453,   454,   212,   212,    23,   433,
     414,  -228,   415,   416,    28,   322,   148,   195,   333,  -228,
     149,   170,    82,    83,    84,   336,   222,   338,    28,  -228,
     409,   342,   213,   213,   323,   360,   350,   442,   352,  -228,
     353,    22,   222,   422,   356,   428,   428,    96,   222,   361,
     146,   362,   363,   367,   510,   511,   146,   252,   209,   209,
     265,   368,   265,   186,   372,   465,   466,   147,   144,   373,
     374,   376,   380,   147,   277,   381,   278,   384,    82,   387,
     280,   130,    95,   403,   284,   285,   286,   404,   405,   214,
     419,   290,   494,   495,   406,   215,   408,   534,   535,   557,
     411,   501,   146,   348,   412,   152,   480,   431,   423,   482,
     216,   438,   507,   489,   266,   439,   266,   508,   509,   147,
     310,   440,   448,   441,   222,   222,   445,   572,   449,   450,
     475,   451,   331,   146,    69,   481,    72,   456,   271,   267,
     146,   267,   463,   344,   272,   146,   464,   467,   486,   528,
     147,   529,   530,   268,   485,   268,   339,   147,   515,   273,
     490,   359,   147,   519,   492,   126,   496,   214,   214,   493,
     498,   428,   499,   215,   215,   269,   500,   269,   364,   504,
     521,   365,   366,   520,   550,   505,   526,   214,   216,   216,
     531,   195,   222,   215,   506,   542,   544,    99,   118,   197,
     146,   270,   567,   270,   553,   569,   566,   570,   216,    73,
     383,   571,   385,   222,   512,   388,   568,   147,   413,   135,
     238,   407,   222,   113,   146,   348,   400,   163,   543,   169,
     146,   261,   385,   164,   161,   391,   291,   250,   146,   474,
     178,   147,   541,     0,   437,     0,     0,   147,     0,     0,
       0,     0,     0,     0,   210,   147,     0,   193,   118,   427,
     427,     0,   430,     0,     0,     0,   432,     0,   214,     0,
     214,     0,   214,     0,   215,   146,   215,     0,   215,     0,
       0,     0,     0,     0,   246,     0,   222,     0,   447,   216,
       0,   216,   147,   216,     0,     0,     0,     0,     0,     0,
     457,     0,   146,     0,     0,     0,   169,     0,     0,   146,
       0,   146,     0,   452,   146,     0,    99,   244,     0,   147,
     460,     0,   461,     0,     0,   462,   147,     0,   147,     0,
     491,   147,   210,   210,   146,     0,     0,     0,     0,   146,
     214,   214,     0,     0,     7,   279,   215,   215,   304,     0,
       0,   147,   210,     0,     0,   146,   147,   146,     0,     0,
     292,   216,   216,     0,     0,     0,   502,     0,   503,   301,
      24,     7,   147,   102,   147,   128,   104,   105,   345,   107,
     129,   109,   522,   523,   343,   427,   346,   525,     0,     0,
       0,     0,     0,     0,   244,   126,     0,    24,   146,     0,
     102,     0,   128,   104,   105,   345,   107,   129,   109,   527,
     146,     0,   146,   479,     0,   147,     0,     0,     0,     0,
     545,   532,     0,   533,     0,   549,     0,   147,     0,   147,
       0,     0,     0,   210,     0,   210,   556,   210,   556,     0,
       0,   375,     8,     9,    10,    11,    12,    13,     0,     0,
     556,   556,     0,     0,     0,    18,    19,    20,     0,     0,
       0,    22,     0,   394,     8,     9,    10,    11,    12,    13,
       0,     0,    25,     0,     0,     0,     0,    18,    19,    20,
       0,     0,     0,    22,     0,     0,     0,     7,   262,     0,
       0,   244,   263,     0,    25,     0,     0,     0,     0,     0,
       0,   244,     7,     0,   244,   210,   210,     0,     0,   271,
     244,   271,     0,    24,   263,   272,   102,   272,   128,   104,
     105,   345,   107,   129,   109,     0,     0,     0,    24,   559,
     273,   102,   273,   128,   104,   105,   106,   107,   129,   109,
       0,   455,   301,     0,   458,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,    16,    17,     0,     0,    18,
      19,    20,    21,     0,   100,    22,    23,     0,     0,     0,
       0,    24,   101,     0,   102,    95,   103,   104,   105,   106,
     107,   108,   109,     0,    27,     0,    28,     0,    29,     0,
       0,     0,    90,     0,     0,    32,    33,     0,     0,     0,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    18,    19,    20,    21,   518,   100,
      22,    23,     0,     0,     0,     0,    24,   101,     0,   102,
      95,   103,   104,   105,   106,   107,   129,   109,     0,    27,
       0,    28,     0,    29,     0,     0,     0,    90,     0,     0,
      32,    33,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,    16,    17,     0,     0,    18,    19,    20,    21,
       0,     0,    22,    23,     0,     0,     0,     0,    24,     0,
       0,     0,     0,    25,     0,     0,     0,     0,    26,     0,
       0,    27,     0,    28,     0,    29,     0,     0,     0,    30,
       0,    31,    32,    33,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    18,    19,
      20,    21,     0,     0,    22,    23,     0,     0,     0,     0,
      24,     0,     0,     0,    95,    25,     0,     0,   233,     0,
       0,     0,     0,    27,     0,    28,     0,    29,     0,     0,
       0,    90,   239,     0,    32,    33,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,     0,     0,     0,     0,
      18,    19,    20,    21,     0,     0,    22,    23,     0,     0,
       0,     0,    24,     0,     0,     0,    95,    25,     0,     0,
     233,     0,     0,     0,     0,    27,     0,    28,     0,    29,
       0,     0,     0,    90,   318,     0,    32,    33,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,     0,     0,
       0,     0,    18,    19,    20,    21,     0,     0,    22,    23,
       0,     0,     0,     0,    24,     0,     0,     0,    95,    25,
       0,     0,   233,     0,     0,     0,     0,    27,     0,    28,
       0,    29,     0,     0,     0,    90,   396,     0,    32,    33,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
      16,     0,     0,     0,    18,    19,    20,    21,     0,     0,
      22,    23,     0,     0,     0,     0,    24,     0,     0,     0,
       0,    25,     0,     0,     0,     0,     0,     0,     0,    27,
       0,    28,     0,    29,     0,     0,     0,    90,     0,    31,
      32,    33,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    18,    19,    20,    21,
       0,     0,    22,    23,     0,     0,     0,     0,    24,     0,
       0,     0,     0,    25,     0,     0,   233,     0,     0,     0,
       0,    27,     0,    28,     0,    29,     0,     0,     0,    90,
     234,     0,    32,    33,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    18,    19,
      20,    21,     0,     0,    22,    23,     0,     0,     0,     0,
      24,     0,     0,     0,     0,    25,     0,     0,   233,     0,
       0,     0,     0,    27,     0,    28,     0,    29,     0,     0,
       0,    90,   369,     0,    32,    33,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,     0,     0,     0,     0,
      18,    19,    20,    21,     0,     0,    22,    23,     0,     0,
       0,     0,    24,     0,     0,     0,    95,    25,     0,     0,
     401,     0,     0,     0,     0,    27,     0,    28,     0,    29,
       0,     0,     0,    90,     0,     0,    32,    33,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,   424,     0,
       0,     0,    18,    19,    20,    21,     0,     0,    22,    23,
       0,     0,     0,     0,    24,   425,     0,     0,     0,    25,
       0,     0,     0,     0,     0,     0,     0,    27,     0,    28,
       0,    29,     0,     0,     0,    90,     0,     0,    32,    33,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    18,    19,    20,    21,     0,     0,
      22,    23,     0,     0,     0,     0,    24,     0,     0,     0,
      95,    25,     0,     0,     0,     0,     0,     0,     0,    27,
       0,    28,     0,    29,     0,     0,     0,    90,     0,     0,
      32,    33,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    18,    19,    20,    21,
       0,   181,    22,    23,     0,     0,     0,     0,    24,     0,
       0,     0,     0,    25,     0,     0,     0,     0,     0,     0,
       0,    27,     0,    28,     0,    29,     0,     0,     0,    90,
       0,     0,    32,    33,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    18,    19,
      20,    21,     0,   221,    22,    23,     0,     0,     0,     0,
      24,     0,     0,     0,     0,    25,     0,     0,     0,     0,
       0,     0,     0,    27,     0,    28,     0,    29,     0,     0,
       0,    90,     0,     0,    32,    33,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,     0,     0,     0,     0,
      18,    19,    20,    21,     0,   316,    22,    23,     0,     0,
       0,     0,    24,     0,     0,     0,     0,    25,     0,     0,
       0,     0,     0,     0,     0,    27,     0,    28,     0,    29,
       0,     0,     0,    90,     0,     0,    32,    33,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,     0,     0,
       0,     0,    18,    19,    20,    21,     0,     0,    22,    23,
       0,     0,     0,     0,    24,     0,     0,     0,     0,    25,
       0,     0,     0,     0,     0,     0,     0,    27,     0,    28,
       0,    29,   382,     0,     0,    90,     0,     0,    32,    33,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    18,    19,    20,    21,     0,     0,
      22,    23,     0,     0,     0,     0,    24,     0,     0,     0,
       0,    25,     0,     0,     0,     0,     0,     0,     0,    27,
       0,    28,     0,    29,   446,     0,     0,    90,     0,     0,
      32,    33,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    18,    19,    20,    21,
       0,     0,    22,    23,     0,     0,     0,     0,    24,     0,
       0,     0,     0,    25,     0,     0,     0,     0,     0,     0,
       0,    27,     0,    28,     0,    29,     0,     0,     0,    90,
       0,   553,    32,    33,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    18,    19,
      20,    21,     0,     0,    22,    23,     0,     0,     0,     0,
      24,     0,     0,     0,     0,    25,     0,     0,     0,     0,
       0,     0,     0,    27,     0,    28,     0,    29,     0,     0,
       0,    90,     0,     0,    32,    33,     7,     8,     9,    10,
      11,    12,    13,     0,    14,     0,     0,     0,     0,     0,
      18,    19,    20,     0,     0,     0,    22,     0,     0,     0,
       0,     0,    24,     0,     0,     0,    95,    25,     0,     0,
       7,     8,     9,    10,    11,    12,    13,     0,    14,   200,
       0,     0,     0,   201,    18,    19,    20,     0,     0,     0,
      22,     0,     0,     0,     0,     0,    24,     0,     0,     0,
       0,    25,   102,    95,   128,   104,   105,   106,   107,   129,
     109,   130,     0,   200,     0,     0,     0,   201,   197,   102,
      95,   128,   104,   105,   106,   107,   129,   109,   130
};

static const yytype_int16 yycheck[] =
{
      27,   120,    28,    94,    31,   245,    30,    17,   351,   171,
     149,     1,   144,   149,   122,   361,   200,    30,   150,    59,
      60,    16,    17,     3,    21,     3,    25,   143,    52,    11,
      11,    26,     3,   149,    22,    39,   220,    32,    33,   122,
      30,   201,    39,    48,    49,    49,    21,     3,     4,    29,
      24,    29,    49,   122,     3,    54,    55,     3,    29,    37,
      50,    49,    52,   171,    46,    11,    14,    31,    46,    79,
      44,    51,    50,    29,    49,   122,    50,    90,    34,    50,
      29,     0,    14,    29,    79,    80,    50,   226,   171,   113,
     226,    39,   200,   201,    50,    30,    38,    50,    30,    52,
      46,   122,   171,    46,   103,   345,   133,    39,    21,   225,
     226,    46,   220,   108,    24,   142,    48,   200,   201,    22,
     228,   305,   112,   113,   171,    48,    39,   122,    39,    23,
     157,   200,   201,    26,    44,   130,    49,   220,    49,    44,
     486,   301,   511,   303,    47,    38,    49,    35,    36,   139,
     171,   220,    40,   200,   201,    52,   196,   293,   153,   112,
     113,   156,   294,   236,   159,   534,   535,   240,   241,   512,
      21,   166,   199,   220,   169,    48,    49,   293,   418,   200,
     201,    49,    10,    51,   323,    13,   139,   323,    22,    49,
     217,    51,   324,   301,   234,   303,    30,   305,   314,   220,
     332,   191,   229,    22,    21,   200,   201,   323,   207,   542,
      30,   544,   165,   373,   374,    49,     3,    51,   301,    24,
     303,   174,   305,   218,   219,   220,    31,    49,    47,   224,
      49,    47,   301,   228,   303,    49,   305,    24,   191,    44,
      51,    28,    29,    51,    31,    50,   319,   320,    29,    30,
     245,    51,   522,   523,   301,   525,   303,    44,   305,   254,
     539,   256,    31,    50,    31,   373,   374,   266,    46,    24,
      21,   387,   551,   552,    44,   545,    31,    48,    49,   549,
     301,    41,   303,    21,   305,    59,    60,    29,    30,    44,
     373,   374,    48,    49,    47,   322,    46,   370,   371,    51,
     540,   433,    59,    60,   373,   374,   301,    51,   303,    90,
     305,    92,    93,     3,    24,    48,    49,    51,    99,   100,
     101,    48,    49,    49,   397,   398,   373,   374,    24,   369,
      13,    21,    15,    16,    44,    31,    46,   118,    49,    29,
      50,    31,     4,     5,     6,    49,   127,    47,    44,    39,
     345,    49,   373,   374,    50,    49,    47,   384,    51,    49,
      48,    23,   143,   358,    45,   360,   361,   148,   149,    49,
     144,    45,    30,    22,   465,   466,   150,   158,   373,   374,
     542,    51,   544,    30,    47,   411,   412,   144,   428,    30,
      49,    51,    47,   150,   175,    47,   177,    31,     4,    50,
     181,    41,    33,    51,   185,   186,   187,    51,    51,   122,
      48,   192,   439,   440,    51,   122,    51,   508,   509,   538,
      44,   448,   196,   418,    44,    30,   421,    22,    51,   424,
     122,    47,   459,   428,   542,    31,   544,   463,   464,   196,
     221,    31,    31,    47,   225,   226,    47,   566,    51,    51,
      35,    51,   233,   227,     2,    39,     4,    51,   171,   542,
     234,   544,    44,   244,   171,   239,    44,    44,    49,   496,
     227,   498,   499,   542,    45,   544,   239,   234,   473,   171,
      45,   262,   239,   478,    51,    33,    31,   200,   201,    51,
      31,   486,    31,   200,   201,   542,    47,   544,   279,    51,
      31,   282,   283,    39,   531,    51,    28,   220,   200,   201,
      31,   292,   293,   220,    51,    31,    31,    29,    30,    48,
     294,   542,    45,   544,    52,    45,   553,    45,   220,     5,
     311,    45,   313,   314,   467,   316,   556,   294,   350,    45,
     148,   342,   323,    30,   318,   540,   327,    80,   518,    87,
     324,   169,   333,    80,    79,   318,   192,   156,   332,   415,
      98,   318,   514,    -1,   374,    -1,    -1,   324,    -1,    -1,
      -1,    -1,    -1,    -1,   122,   332,    -1,   115,    90,   360,
     361,    -1,   363,    -1,    -1,    -1,   367,    -1,   301,    -1,
     303,    -1,   305,    -1,   301,   369,   303,    -1,   305,    -1,
      -1,    -1,    -1,    -1,   152,    -1,   387,    -1,   389,   301,
      -1,   303,   369,   305,    -1,    -1,    -1,    -1,    -1,    -1,
     401,    -1,   396,    -1,    -1,    -1,   164,    -1,    -1,   403,
      -1,   405,    -1,   396,   408,    -1,   148,   149,    -1,   396,
     403,    -1,   405,    -1,    -1,   408,   403,    -1,   405,    -1,
     431,   408,   200,   201,   428,    -1,    -1,    -1,    -1,   433,
     373,   374,    -1,    -1,     3,   177,   373,   374,   206,    -1,
      -1,   428,   220,    -1,    -1,   449,   433,   451,    -1,    -1,
     192,   373,   374,    -1,    -1,    -1,   449,    -1,   451,   201,
      29,     3,   449,    32,   451,    34,    35,    36,    37,    38,
      39,    40,   483,   484,   242,   486,    45,   488,    -1,    -1,
      -1,    -1,    -1,    -1,   226,   263,    -1,    29,   492,    -1,
      32,    -1,    34,    35,    36,    37,    38,    39,    40,   492,
     504,    -1,   506,    45,    -1,   492,    -1,    -1,    -1,    -1,
     521,   504,    -1,   506,    -1,   526,    -1,   504,    -1,   506,
      -1,    -1,    -1,   301,    -1,   303,   537,   305,   539,    -1,
      -1,   299,     4,     5,     6,     7,     8,     9,    -1,    -1,
     551,   552,    -1,    -1,    -1,    17,    18,    19,    -1,    -1,
      -1,    23,    -1,   321,     4,     5,     6,     7,     8,     9,
      -1,    -1,    34,    -1,    -1,    -1,    -1,    17,    18,    19,
      -1,    -1,    -1,    23,    -1,    -1,    -1,     3,    50,    -1,
      -1,   323,    54,    -1,    34,    -1,    -1,    -1,    -1,    -1,
      -1,   333,     3,    -1,   336,   373,   374,    -1,    -1,   542,
     342,   544,    -1,    29,    54,   542,    32,   544,    34,    35,
      36,    37,    38,    39,    40,    -1,    -1,    -1,    29,    45,
     542,    32,   544,    34,    35,    36,    37,    38,    39,    40,
      -1,   399,   374,    -1,   402,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    13,    14,    -1,    -1,    17,
      18,    19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    -1,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    -1,    42,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    -1,    -1,    53,    54,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,   476,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    -1,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    42,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    -1,    -1,
      53,    54,     3,     4,     5,     6,     7,     8,     9,    -1,
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
      -1,    -1,    -1,    50,    17,    18,    19,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,
      -1,    34,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    -1,    46,    -1,    -1,    -1,    50,    48,    32,
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
      94,    22,    47,    49,    50,   143,    22,    97,    51,    97,
      98,    99,    31,    50,    76,    78,    34,    50,   138,   143,
      91,    94,    76,    49,    68,   101,    49,   101,    47,    78,
     101,   101,    49,    68,    94,    37,    45,    66,   143,   153,
      47,    63,    51,    48,   143,   143,    45,    39,    49,    94,
      49,    49,    45,    30,    94,    94,    94,    22,    51,    51,
      97,    98,    47,    30,    49,    68,    51,   108,   108,   134,
      47,    47,    47,    94,    31,    94,    97,    50,    94,    22,
      47,    78,   101,   101,    68,    91,    51,    97,    98,    99,
      94,    37,    99,    51,    51,    51,    51,   100,    51,   143,
     153,    44,    44,    62,    13,    15,    16,    64,    65,    48,
      39,    49,   143,    51,    13,    30,    84,    94,   143,    84,
      94,    22,    94,    76,   101,   101,   108,   129,    47,    31,
      31,    47,    91,    51,    97,    47,    47,    94,    31,    51,
      51,    51,    78,   101,   101,    68,    51,    94,    68,    31,
      78,    78,    78,    44,    44,   113,   113,    44,    35,    36,
      40,   126,   127,   136,   126,    35,   115,   116,   137,    45,
     143,    39,   143,    29,    30,    45,    49,    83,    28,   143,
      45,    94,    51,    51,    91,    91,    31,    51,    31,    31,
      47,    91,    78,    78,    51,    51,    51,    91,   113,   113,
     110,   110,    63,    48,    49,   143,    48,    48,    68,   143,
      39,    31,    94,    94,    84,    94,    28,    78,    91,    91,
      91,    31,    78,    78,   110,   110,    10,    73,   145,    73,
      65,   127,    31,   116,    31,    94,    83,    83,    83,    94,
      91,    73,    73,    52,    72,    74,    94,    67,    72,    45,
     104,   104,    83,    83,    72,    72,    91,    45,    74,    45,
      45,    45,    67
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
      79,    81,    80,    82,    80,    83,    83,    84,    84,    84,
      84,    84,    84,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    86,    86,
      86,    87,    87,    87,    88,    88,    88,    88,    88,    88,
      89,    89,    89,    89,    89,    90,    90,    90,    90,    90,
      91,    91,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    91,    92,    92,    92,    92,    93,    93,
      94,    95,    95,    96,    96,    96,    96,    96,    96,    97,
      97,    98,    98,    99,    99,   100,   101,   101,   102,   102,
     103,   103,   104,   104,   104,   104,   104,   104,   104,   104,
     104,   105,   106,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   109,   109,   110,   110,   111,   111,   112,
     113,   113,   113,   113,   113,   113,   114,   114,   114,   114,
     114,   115,   115,   116,   117,   117,   118,   118,   119,   119,
     119,   120,   120,   121,   121,   122,   122,   122,   123,   123,
     124,   124,   125,   125,   126,   126,   127,   127,   128,   129,
     129,   130,   130,   131,   131,   132,   132,   133,   133,   134,
     134,   135,   135,   136,   136,   136,   136,   137,   137,   138,
     138,   139,   139,   140,   140,   141,   142,   142,   142,   142,
     142,   142,   143,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   153,   153,   153,   153,   153,   153,
     153,   154,   155,   155,   156
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
       5,     7,     8,     7,     7,     6,     6,     6,     8,     7,
       4,     0,     7,     0,     7,     0,     2,     4,     5,     5,
       2,     4,     4,     1,     1,     1,     1,     1,     1,     3,
       2,     3,     3,     4,     3,     1,     4,     1,     5,     5,
       6,     7,     7,     8,     6,     6,     7,     8,     8,     9,
       2,     2,     3,     5,     4,     2,     2,     3,     4,     5,
       1,     1,     1,     1,     5,     2,     4,     3,     4,     5,
       7,     4,     6,     7,     0,     2,     2,     4,     1,     3,
       2,     0,     2,     1,     3,     2,     3,     4,     5,     1,
       3,     2,     4,     1,     3,     2,     1,     3,     1,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     5,     5,     0,     2,     1,     2,     3,
       0,     3,     5,     3,     5,     7,     3,     5,     3,     5,
       7,     1,     3,     4,     0,     1,     1,     3,     1,     3,
       5,     0,     1,     1,     3,     1,     3,     4,     3,     2,
       1,     3,     0,     2,     1,     3,     2,     4,     3,     3,
       2,     1,     3,     0,     2,     4,     5,     3,     4,     0,
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
#line 2189 "lang11d_tab.cpp"
    break;

  case 3: /* root: classextensions  */
#line 42 "lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; }
#line 2195 "lang11d_tab.cpp"
    break;

  case 4: /* root: INTERPRET cmdlinecode  */
#line 44 "lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 2; }
#line 2201 "lang11d_tab.cpp"
    break;

  case 5: /* classes: %empty  */
#line 47 "lang11d"
          { yyval = 0; }
#line 2207 "lang11d_tab.cpp"
    break;

  case 6: /* classes: classes classdef  */
#line 49 "lang11d"
                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2213 "lang11d_tab.cpp"
    break;

  case 8: /* classextensions: classextensions classextension  */
#line 54 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2219 "lang11d_tab.cpp"
    break;

  case 9: /* classdef: classname superclass '{' classvardecls methods '}'  */
#line 58 "lang11d"
                                { yyval = (intptr_t)newPyrClassNode((PyrSlotNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1], 0);
				}
#line 2227 "lang11d_tab.cpp"
    break;

  case 10: /* classdef: classname '[' optname ']' superclass '{' classvardecls methods '}'  */
#line 62 "lang11d"
                                { yyval = (intptr_t)newPyrClassNode((PyrSlotNode*)yyvsp[-8], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1],
					(PyrSlotNode*)yyvsp[-6]);
				}
#line 2236 "lang11d_tab.cpp"
    break;

  case 11: /* classextension: '+' classname '{' methods '}'  */
#line 69 "lang11d"
                                {
					yyval = (intptr_t)newPyrClassExtNode((PyrSlotNode*)yyvsp[-3], (PyrMethodNode*)yyvsp[-1]);
				}
#line 2244 "lang11d_tab.cpp"
    break;

  case 12: /* optname: %empty  */
#line 74 "lang11d"
                  { yyval = 0; }
#line 2250 "lang11d_tab.cpp"
    break;

  case 14: /* superclass: %empty  */
#line 78 "lang11d"
                  { yyval = 0; }
#line 2256 "lang11d_tab.cpp"
    break;

  case 15: /* superclass: ':' classname  */
#line 80 "lang11d"
                                { yyval = yyvsp[0]; }
#line 2262 "lang11d_tab.cpp"
    break;

  case 16: /* classvardecls: %empty  */
#line 83 "lang11d"
                  { yyval = 0; }
#line 2268 "lang11d_tab.cpp"
    break;

  case 17: /* classvardecls: classvardecls classvardecl  */
#line 85 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2274 "lang11d_tab.cpp"
    break;

  case 18: /* classvardecl: CLASSVAR rwslotdeflist ';'  */
#line 89 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varClass); }
#line 2280 "lang11d_tab.cpp"
    break;

  case 19: /* classvardecl: VAR rwslotdeflist ';'  */
#line 91 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varInst); }
#line 2286 "lang11d_tab.cpp"
    break;

  case 20: /* classvardecl: SC_CONST constdeflist ';'  */
#line 93 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varConst); }
#line 2292 "lang11d_tab.cpp"
    break;

  case 21: /* methods: %empty  */
#line 96 "lang11d"
                  { yyval = 0; }
#line 2298 "lang11d_tab.cpp"
    break;

  case 22: /* methods: methods methoddef  */
#line 98 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2304 "lang11d_tab.cpp"
    break;

  case 23: /* methoddef: name '{' argdecls funcvardecls primitive methbody '}'  */
#line 102 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2311 "lang11d_tab.cpp"
    break;

  case 24: /* methoddef: '*' name '{' argdecls funcvardecls primitive methbody '}'  */
#line 105 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2318 "lang11d_tab.cpp"
    break;

  case 25: /* methoddef: binop '{' argdecls funcvardecls primitive methbody '}'  */
#line 108 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2325 "lang11d_tab.cpp"
    break;

  case 26: /* methoddef: '*' binop '{' argdecls funcvardecls primitive methbody '}'  */
#line 111 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2332 "lang11d_tab.cpp"
    break;

  case 34: /* funcbody: exprseq funretval  */
#line 129 "lang11d"
                                { yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2338 "lang11d_tab.cpp"
    break;

  case 35: /* cmdlinecode: '(' argdecls1 funcvardecls1 funcbody ')'  */
#line 133 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2344 "lang11d_tab.cpp"
    break;

  case 36: /* cmdlinecode: '(' argdecls1 funcbody ')'  */
#line 135 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-2], NULL, (PyrParseNode*)yyvsp[-1], false); }
#line 2350 "lang11d_tab.cpp"
    break;

  case 37: /* cmdlinecode: '(' funcvardecls1 funcbody ')'  */
#line 137 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2356 "lang11d_tab.cpp"
    break;

  case 38: /* cmdlinecode: argdecls1 funcvardecls1 funcbody  */
#line 139 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-2], (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2362 "lang11d_tab.cpp"
    break;

  case 39: /* cmdlinecode: argdecls1 funcbody  */
#line 141 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-1], NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2368 "lang11d_tab.cpp"
    break;

  case 40: /* cmdlinecode: funcvardecls1 funcbody  */
#line 143 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2374 "lang11d_tab.cpp"
    break;

  case 41: /* cmdlinecode: funcbody  */
#line 145 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2380 "lang11d_tab.cpp"
    break;

  case 43: /* methbody: exprseq retval  */
#line 150 "lang11d"
                                { yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2386 "lang11d_tab.cpp"
    break;

  case 44: /* primitive: %empty  */
#line 153 "lang11d"
                  { yyval = 0; }
#line 2392 "lang11d_tab.cpp"
    break;

  case 45: /* primitive: primname optsemi  */
#line 155 "lang11d"
                                { yyval = yyvsp[-1]; }
#line 2398 "lang11d_tab.cpp"
    break;

  case 46: /* retval: %empty  */
#line 159 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode(NULL); }
#line 2404 "lang11d_tab.cpp"
    break;

  case 47: /* retval: '^' expr optsemi  */
#line 161 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); }
#line 2410 "lang11d_tab.cpp"
    break;

  case 48: /* funretval: %empty  */
#line 165 "lang11d"
                        { yyval = (intptr_t)newPyrBlockReturnNode(); }
#line 2416 "lang11d_tab.cpp"
    break;

  case 49: /* funretval: '^' expr optsemi  */
#line 167 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); }
#line 2422 "lang11d_tab.cpp"
    break;

  case 51: /* blocklist1: blocklist1 blocklistitem  */
#line 172 "lang11d"
                                {
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
				}
#line 2430 "lang11d_tab.cpp"
    break;

  case 54: /* blocklist: %empty  */
#line 181 "lang11d"
                        { yyval = 0; }
#line 2436 "lang11d_tab.cpp"
    break;

  case 56: /* msgsend: name blocklist1  */
#line 186 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2444 "lang11d_tab.cpp"
    break;

  case 57: /* msgsend: '(' binop2 ')' blocklist1  */
#line 190 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2452 "lang11d_tab.cpp"
    break;

  case 58: /* msgsend: name '(' ')' blocklist1  */
#line 194 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-3], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2460 "lang11d_tab.cpp"
    break;

  case 59: /* msgsend: name '(' arglist1 optkeyarglist ')' blocklist  */
#line 198 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2469 "lang11d_tab.cpp"
    break;

  case 60: /* msgsend: '(' binop2 ')' '(' ')' blocklist1  */
#line 203 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-4], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2477 "lang11d_tab.cpp"
    break;

  case 61: /* msgsend: '(' binop2 ')' '(' arglist1 optkeyarglist ')' blocklist  */
#line 207 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2486 "lang11d_tab.cpp"
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
#line 2508 "lang11d_tab.cpp"
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
#line 2525 "lang11d_tab.cpp"
    break;

  case 64: /* msgsend: classname '[' arrayelems ']'  */
#line 243 "lang11d"
                        { yyval = (intptr_t)newPyrDynListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 2531 "lang11d_tab.cpp"
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
#line 2546 "lang11d_tab.cpp"
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
#line 2561 "lang11d_tab.cpp"
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
#line 2576 "lang11d_tab.cpp"
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
#line 2593 "lang11d_tab.cpp"
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
#line 2618 "lang11d_tab.cpp"
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
#line 2631 "lang11d_tab.cpp"
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
#line 2644 "lang11d_tab.cpp"
    break;

  case 72: /* msgsend: expr '.' name '(' keyarglist1 optcomma ')' blocklist  */
#line 330 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-7],
					(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2653 "lang11d_tab.cpp"
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
#line 2670 "lang11d_tab.cpp"
    break;

  case 74: /* msgsend: expr '.' '*' name '(' arglist1 ')'  */
#line 349 "lang11d"
                    {
		        PyrSlot slot;

		        SetSymbol(&slot, s_performArgs);
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);

				PyrParseNode* args = linkNextNode(
					(PyrParseNode*)yyvsp[-6],
					newPyrPushLitNode((PyrSlotNode*)yyvsp[-3], NULL)
                );
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, NULL, 0);
            }
#line 2688 "lang11d_tab.cpp"
    break;

  case 75: /* msgsend: expr '.' '*' '(' arglist1 ')'  */
#line 363 "lang11d"
                    {
		        PyrSlot slot, slot2;
		        SetSymbol(&slot, s_performArgs);
		        SetSymbol(&slot2, s_value);
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);
				PyrParseNode* args = linkNextNode(
					(PyrParseNode*)yyvsp[-5],
					newPyrPushLitNode(newPyrSlotNode(&slot2), NULL)
                );
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, NULL, 0);
            }
#line 2705 "lang11d_tab.cpp"
    break;

  case 76: /* msgsend: expr '.' '(' arglistv1 optkeyarglist ')'  */
#line 377 "lang11d"
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
#line 2729 "lang11d_tab.cpp"
    break;

  case 77: /* msgsend: expr '.' name '(' ')' blocklist  */
#line 399 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-5], NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2737 "lang11d_tab.cpp"
    break;

  case 78: /* msgsend: expr '.' name '(' arglist1 optkeyarglist ')' blocklist  */
#line 403 "lang11d"
                        {
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2747 "lang11d_tab.cpp"
    break;

  case 79: /* msgsend: expr '.' name '(' arglistv1 optkeyarglist ')'  */
#line 409 "lang11d"
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
#line 2769 "lang11d_tab.cpp"
    break;

  case 80: /* msgsend: expr '.' name blocklist  */
#line 427 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[-3], 0, (PyrParseNode*)yyvsp[0]);
			}
#line 2777 "lang11d_tab.cpp"
    break;

  case 81: /* $@1: %empty  */
#line 432 "lang11d"
                            { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 1); }
#line 2783 "lang11d_tab.cpp"
    break;

  case 82: /* generator: '{' ':' exprseq $@1 ',' qual '}'  */
#line 433 "lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, getsym("r"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);

				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)yyvsp[-1], false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				yyval = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)blocklit, 0, 0);
			}
#line 2797 "lang11d_tab.cpp"
    break;

  case 83: /* $@2: %empty  */
#line 442 "lang11d"
                                  { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 2); }
#line 2803 "lang11d_tab.cpp"
    break;

  case 84: /* generator: '{' ';' exprseq $@2 ',' qual '}'  */
#line 443 "lang11d"
                        {
				yyval = yyvsp[-1];
			}
#line 2811 "lang11d_tab.cpp"
    break;

  case 85: /* nextqual: %empty  */
#line 449 "lang11d"
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
#line 2837 "lang11d_tab.cpp"
    break;

  case 86: /* nextqual: ',' qual  */
#line 471 "lang11d"
                                { yyval = yyvsp[0]; }
#line 2843 "lang11d_tab.cpp"
    break;

  case 87: /* qual: name LEFTARROW exprseq nextqual  */
#line 475 "lang11d"
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
#line 2881 "lang11d_tab.cpp"
    break;

  case 88: /* qual: name name LEFTARROW exprseq nextqual  */
#line 509 "lang11d"
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
#line 2923 "lang11d_tab.cpp"
    break;

  case 89: /* qual: VAR name '=' exprseq nextqual  */
#line 547 "lang11d"
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
#line 2941 "lang11d_tab.cpp"
    break;

  case 90: /* qual: exprseq nextqual  */
#line 561 "lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, getsym("if"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)yyvsp[0], false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-1], blocklit);

				yyval = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
			}
#line 2956 "lang11d_tab.cpp"
    break;

  case 91: /* qual: ':' ':' exprseq nextqual  */
#line 572 "lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
			}
#line 2964 "lang11d_tab.cpp"
    break;

  case 92: /* qual: ':' WHILE exprseq nextqual  */
#line 576 "lang11d"
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
#line 2991 "lang11d_tab.cpp"
    break;

  case 99: /* expr1: '(' exprseq ')'  */
#line 607 "lang11d"
                        {
				PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
				node->mParens = 1;
				yyval = yyvsp[-1];
			}
#line 3001 "lang11d_tab.cpp"
    break;

  case 100: /* expr1: '~' name  */
#line 613 "lang11d"
                        {
				PyrParseNode* argnode;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL);
				SetSymbol(&slot, s_envirGet);
				selectornode = newPyrSlotNode(&slot);
				yyval = (intptr_t)newPyrCallNode(selectornode, argnode, 0, 0);
			}
#line 3015 "lang11d_tab.cpp"
    break;

  case 101: /* expr1: '[' arrayelems ']'  */
#line 623 "lang11d"
                        { yyval = (intptr_t)newPyrDynListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 3021 "lang11d_tab.cpp"
    break;

  case 102: /* expr1: '(' valrange2 ')'  */
#line 625 "lang11d"
                        { yyval = yyvsp[-1]; }
#line 3027 "lang11d_tab.cpp"
    break;

  case 103: /* expr1: '(' ':' valrange3 ')'  */
#line 627 "lang11d"
                        { yyval = yyvsp[-1]; }
#line 3033 "lang11d_tab.cpp"
    break;

  case 104: /* expr1: '(' dictslotlist ')'  */
#line 629 "lang11d"
                        { yyval = (intptr_t)newPyrDynDictNode((PyrParseNode*)yyvsp[-1]); }
#line 3039 "lang11d_tab.cpp"
    break;

  case 105: /* expr1: pseudovar  */
#line 631 "lang11d"
                        { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3045 "lang11d_tab.cpp"
    break;

  case 106: /* expr1: expr1 '[' arglist1 ']'  */
#line 633 "lang11d"
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
#line 3062 "lang11d_tab.cpp"
    break;

  case 108: /* valrangex1: expr1 '[' arglist1 DOTDOT ']'  */
#line 649 "lang11d"
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
#line 3093 "lang11d_tab.cpp"
    break;

  case 109: /* valrangex1: expr1 '[' DOTDOT exprseq ']'  */
#line 676 "lang11d"
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
#line 3115 "lang11d_tab.cpp"
    break;

  case 110: /* valrangex1: expr1 '[' arglist1 DOTDOT exprseq ']'  */
#line 694 "lang11d"
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
#line 3144 "lang11d_tab.cpp"
    break;

  case 111: /* valrangeassign: expr1 '[' arglist1 DOTDOT ']' '=' expr  */
#line 721 "lang11d"
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
#line 3176 "lang11d_tab.cpp"
    break;

  case 112: /* valrangeassign: expr1 '[' DOTDOT exprseq ']' '=' expr  */
#line 749 "lang11d"
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
#line 3199 "lang11d_tab.cpp"
    break;

  case 113: /* valrangeassign: expr1 '[' arglist1 DOTDOT exprseq ']' '=' expr  */
#line 768 "lang11d"
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
#line 3229 "lang11d_tab.cpp"
    break;

  case 114: /* valrangexd: expr '.' '[' arglist1 DOTDOT ']'  */
#line 796 "lang11d"
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
#line 3260 "lang11d_tab.cpp"
    break;

  case 115: /* valrangexd: expr '.' '[' DOTDOT exprseq ']'  */
#line 823 "lang11d"
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
#line 3282 "lang11d_tab.cpp"
    break;

  case 116: /* valrangexd: expr '.' '[' arglist1 DOTDOT exprseq ']'  */
#line 841 "lang11d"
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
#line 3311 "lang11d_tab.cpp"
    break;

  case 117: /* valrangexd: expr '.' '[' arglist1 DOTDOT ']' '=' expr  */
#line 866 "lang11d"
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
#line 3343 "lang11d_tab.cpp"
    break;

  case 118: /* valrangexd: expr '.' '[' DOTDOT exprseq ']' '=' expr  */
#line 894 "lang11d"
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
#line 3366 "lang11d_tab.cpp"
    break;

  case 119: /* valrangexd: expr '.' '[' arglist1 DOTDOT exprseq ']' '=' expr  */
#line 913 "lang11d"
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
#line 3396 "lang11d_tab.cpp"
    break;

  case 120: /* valrange2: exprseq DOTDOT  */
#line 941 "lang11d"
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
#line 3418 "lang11d_tab.cpp"
    break;

  case 121: /* valrange2: DOTDOT exprseq  */
#line 960 "lang11d"
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
#line 3440 "lang11d_tab.cpp"
    break;

  case 122: /* valrange2: exprseq DOTDOT exprseq  */
#line 979 "lang11d"
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
#line 3460 "lang11d_tab.cpp"
    break;

  case 123: /* valrange2: exprseq ',' exprseq DOTDOT exprseq  */
#line 996 "lang11d"
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
#line 3478 "lang11d_tab.cpp"
    break;

  case 124: /* valrange2: exprseq ',' exprseq DOTDOT  */
#line 1010 "lang11d"
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
#line 3501 "lang11d_tab.cpp"
    break;

  case 125: /* valrange3: DOTDOT exprseq  */
#line 1031 "lang11d"
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
#line 3523 "lang11d_tab.cpp"
    break;

  case 126: /* valrange3: exprseq DOTDOT  */
#line 1050 "lang11d"
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
#line 3544 "lang11d_tab.cpp"
    break;

  case 127: /* valrange3: exprseq DOTDOT exprseq  */
#line 1068 "lang11d"
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
#line 3564 "lang11d_tab.cpp"
    break;

  case 128: /* valrange3: exprseq ',' exprseq DOTDOT  */
#line 1085 "lang11d"
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
#line 3584 "lang11d_tab.cpp"
    break;

  case 129: /* valrange3: exprseq ',' exprseq DOTDOT exprseq  */
#line 1101 "lang11d"
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
#line 3602 "lang11d_tab.cpp"
    break;

  case 133: /* expr: classname  */
#line 1119 "lang11d"
                            { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3608 "lang11d_tab.cpp"
    break;

  case 134: /* expr: expr '.' '[' arglist1 ']'  */
#line 1121 "lang11d"
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
#line 3625 "lang11d_tab.cpp"
    break;

  case 135: /* expr: '`' expr  */
#line 1134 "lang11d"
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
#line 3643 "lang11d_tab.cpp"
    break;

  case 136: /* expr: expr binop2 adverb expr  */
#line 1148 "lang11d"
                        {
				yyval = (intptr_t)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0], (PyrParseNode*)yyvsp[-1]);
			}
#line 3652 "lang11d_tab.cpp"
    break;

  case 137: /* expr: name '=' expr  */
#line 1153 "lang11d"
                        {
				yyval = (intptr_t)newPyrAssignNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0);
			}
#line 3660 "lang11d_tab.cpp"
    break;

  case 138: /* expr: '~' name '=' expr  */
#line 1157 "lang11d"
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
#line 3675 "lang11d_tab.cpp"
    break;

  case 139: /* expr: expr '.' name '=' expr  */
#line 1168 "lang11d"
                        {
				yyval = (intptr_t)newPyrSetterNode((PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3684 "lang11d_tab.cpp"
    break;

  case 140: /* expr: name '(' arglist1 optkeyarglist ')' '=' expr  */
#line 1173 "lang11d"
                        {
				if (yyvsp[-3] != 0) {
					error("Setter method called with keyword arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-3]);
					compileErrors++;
				}
				yyval = (intptr_t)newPyrSetterNode((PyrSlotNode*)yyvsp[-6],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3698 "lang11d_tab.cpp"
    break;

  case 141: /* expr: '#' mavars '=' expr  */
#line 1183 "lang11d"
                        {
				yyval = (intptr_t)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[0], 0);
			}
#line 3707 "lang11d_tab.cpp"
    break;

  case 142: /* expr: expr1 '[' arglist1 ']' '=' expr  */
#line 1188 "lang11d"
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
#line 3725 "lang11d_tab.cpp"
    break;

  case 143: /* expr: expr '.' '[' arglist1 ']' '=' expr  */
#line 1202 "lang11d"
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
#line 3743 "lang11d_tab.cpp"
    break;

  case 144: /* adverb: %empty  */
#line 1217 "lang11d"
          { yyval = 0; }
#line 3749 "lang11d_tab.cpp"
    break;

  case 145: /* adverb: '.' name  */
#line 1218 "lang11d"
                           { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3755 "lang11d_tab.cpp"
    break;

  case 146: /* adverb: '.' integer  */
#line 1219 "lang11d"
                              { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3761 "lang11d_tab.cpp"
    break;

  case 147: /* adverb: '.' '(' exprseq ')'  */
#line 1220 "lang11d"
                                      { yyval = yyvsp[-1]; }
#line 3767 "lang11d_tab.cpp"
    break;

  case 149: /* exprn: exprn ';' expr  */
#line 1225 "lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 3775 "lang11d_tab.cpp"
    break;

  case 151: /* arrayelems: %empty  */
#line 1233 "lang11d"
                  { yyval = 0; }
#line 3781 "lang11d_tab.cpp"
    break;

  case 152: /* arrayelems: arrayelems1 optcomma  */
#line 1235 "lang11d"
                          { yyval = yyvsp[-1]; }
#line 3787 "lang11d_tab.cpp"
    break;

  case 154: /* arrayelems1: exprseq ':' exprseq  */
#line 1240 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3793 "lang11d_tab.cpp"
    break;

  case 155: /* arrayelems1: keybinop exprseq  */
#line 1242 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 3802 "lang11d_tab.cpp"
    break;

  case 156: /* arrayelems1: arrayelems1 ',' exprseq  */
#line 1247 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3808 "lang11d_tab.cpp"
    break;

  case 157: /* arrayelems1: arrayelems1 ',' keybinop exprseq  */
#line 1249 "lang11d"
                                {
					PyrParseNode* elems;
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					elems = (PyrParseNode*)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], elems);
				}
#line 3819 "lang11d_tab.cpp"
    break;

  case 158: /* arrayelems1: arrayelems1 ',' exprseq ':' exprseq  */
#line 1256 "lang11d"
                                {
					PyrParseNode* elems;
					elems = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-4], elems);
				}
#line 3829 "lang11d_tab.cpp"
    break;

  case 160: /* arglist1: arglist1 ',' exprseq  */
#line 1265 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3835 "lang11d_tab.cpp"
    break;

  case 161: /* arglistv1: '*' exprseq  */
#line 1269 "lang11d"
                                { yyval = yyvsp[0]; }
#line 3841 "lang11d_tab.cpp"
    break;

  case 162: /* arglistv1: arglist1 ',' '*' exprseq  */
#line 1271 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]); }
#line 3847 "lang11d_tab.cpp"
    break;

  case 164: /* keyarglist1: keyarglist1 ',' keyarg  */
#line 1276 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3853 "lang11d_tab.cpp"
    break;

  case 165: /* keyarg: keybinop exprseq  */
#line 1280 "lang11d"
                                { yyval = (intptr_t)newPyrPushKeyArgNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 3859 "lang11d_tab.cpp"
    break;

  case 166: /* optkeyarglist: optcomma  */
#line 1283 "lang11d"
                           { yyval = 0; }
#line 3865 "lang11d_tab.cpp"
    break;

  case 167: /* optkeyarglist: ',' keyarglist1 optcomma  */
#line 1284 "lang11d"
                                                           { yyval = yyvsp[-1]; }
#line 3871 "lang11d_tab.cpp"
    break;

  case 168: /* mavars: mavarlist  */
#line 1288 "lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3877 "lang11d_tab.cpp"
    break;

  case 169: /* mavars: mavarlist ELLIPSIS name  */
#line 1290 "lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); }
#line 3883 "lang11d_tab.cpp"
    break;

  case 171: /* mavarlist: mavarlist ',' name  */
#line 1295 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3889 "lang11d_tab.cpp"
    break;

  case 172: /* slotliteral: integer  */
#line 1299 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3895 "lang11d_tab.cpp"
    break;

  case 173: /* slotliteral: floatp  */
#line 1300 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3901 "lang11d_tab.cpp"
    break;

  case 174: /* slotliteral: ascii  */
#line 1301 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3907 "lang11d_tab.cpp"
    break;

  case 175: /* slotliteral: string  */
#line 1302 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3913 "lang11d_tab.cpp"
    break;

  case 176: /* slotliteral: symbol  */
#line 1303 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3919 "lang11d_tab.cpp"
    break;

  case 177: /* slotliteral: trueobj  */
#line 1304 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3925 "lang11d_tab.cpp"
    break;

  case 178: /* slotliteral: falseobj  */
#line 1305 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3931 "lang11d_tab.cpp"
    break;

  case 179: /* slotliteral: nilobj  */
#line 1306 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3937 "lang11d_tab.cpp"
    break;

  case 180: /* slotliteral: listlit  */
#line 1307 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3943 "lang11d_tab.cpp"
    break;

  case 181: /* blockliteral: block  */
#line 1310 "lang11d"
                        { yyval = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3949 "lang11d_tab.cpp"
    break;

  case 182: /* pushname: name  */
#line 1313 "lang11d"
                                { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3955 "lang11d_tab.cpp"
    break;

  case 183: /* pushliteral: integer  */
#line 1316 "lang11d"
                                { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3961 "lang11d_tab.cpp"
    break;

  case 184: /* pushliteral: floatp  */
#line 1317 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3967 "lang11d_tab.cpp"
    break;

  case 185: /* pushliteral: ascii  */
#line 1318 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3973 "lang11d_tab.cpp"
    break;

  case 186: /* pushliteral: string  */
#line 1319 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3979 "lang11d_tab.cpp"
    break;

  case 187: /* pushliteral: symbol  */
#line 1320 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3985 "lang11d_tab.cpp"
    break;

  case 188: /* pushliteral: trueobj  */
#line 1321 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3991 "lang11d_tab.cpp"
    break;

  case 189: /* pushliteral: falseobj  */
#line 1322 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3997 "lang11d_tab.cpp"
    break;

  case 190: /* pushliteral: nilobj  */
#line 1323 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4003 "lang11d_tab.cpp"
    break;

  case 191: /* pushliteral: listlit  */
#line 1324 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 4009 "lang11d_tab.cpp"
    break;

  case 192: /* listliteral: integer  */
#line 1327 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4015 "lang11d_tab.cpp"
    break;

  case 193: /* listliteral: floatp  */
#line 1328 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4021 "lang11d_tab.cpp"
    break;

  case 194: /* listliteral: ascii  */
#line 1329 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4027 "lang11d_tab.cpp"
    break;

  case 195: /* listliteral: string  */
#line 1330 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4033 "lang11d_tab.cpp"
    break;

  case 196: /* listliteral: symbol  */
#line 1331 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4039 "lang11d_tab.cpp"
    break;

  case 197: /* listliteral: name  */
#line 1332 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4045 "lang11d_tab.cpp"
    break;

  case 198: /* listliteral: trueobj  */
#line 1333 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4051 "lang11d_tab.cpp"
    break;

  case 199: /* listliteral: falseobj  */
#line 1334 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4057 "lang11d_tab.cpp"
    break;

  case 200: /* listliteral: nilobj  */
#line 1335 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4063 "lang11d_tab.cpp"
    break;

  case 201: /* listliteral: listlit2  */
#line 1336 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 4069 "lang11d_tab.cpp"
    break;

  case 202: /* listliteral: dictlit2  */
#line 1337 "lang11d"
                                    { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 4075 "lang11d_tab.cpp"
    break;

  case 203: /* block: '{' argdecls funcvardecls funcbody '}'  */
#line 1341 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], false); }
#line 4082 "lang11d_tab.cpp"
    break;

  case 204: /* block: BEGINCLOSEDFUNC argdecls funcvardecls funcbody '}'  */
#line 1344 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], true); }
#line 4089 "lang11d_tab.cpp"
    break;

  case 205: /* funcvardecls: %empty  */
#line 1348 "lang11d"
                  { yyval = 0; }
#line 4095 "lang11d_tab.cpp"
    break;

  case 206: /* funcvardecls: funcvardecls funcvardecl  */
#line 1350 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 4101 "lang11d_tab.cpp"
    break;

  case 208: /* funcvardecls1: funcvardecls1 funcvardecl  */
#line 1355 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 4107 "lang11d_tab.cpp"
    break;

  case 209: /* funcvardecl: VAR vardeflist ';'  */
#line 1359 "lang11d"
                                { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varLocal); }
#line 4113 "lang11d_tab.cpp"
    break;

  case 210: /* argdecls: %empty  */
#line 1362 "lang11d"
                  { yyval = 0; }
#line 4119 "lang11d_tab.cpp"
    break;

  case 211: /* argdecls: ARG vardeflist ';'  */
#line 1364 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4127 "lang11d_tab.cpp"
    break;

  case 212: /* argdecls: ARG vardeflist0 ELLIPSIS name ';'  */
#line 1368 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4135 "lang11d_tab.cpp"
    break;

  case 213: /* argdecls: '|' slotdeflist '|'  */
#line 1372 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4143 "lang11d_tab.cpp"
    break;

  case 214: /* argdecls: '|' slotdeflist0 ELLIPSIS name '|'  */
#line 1376 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4151 "lang11d_tab.cpp"
    break;

  case 215: /* argdecls: '|' slotdeflist0 ELLIPSIS name ',' name '|'  */
#line 1380 "lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 4159 "lang11d_tab.cpp"
    break;

  case 216: /* argdecls1: ARG vardeflist ';'  */
#line 1386 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4167 "lang11d_tab.cpp"
    break;

  case 217: /* argdecls1: ARG vardeflist0 ELLIPSIS name ';'  */
#line 1390 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4175 "lang11d_tab.cpp"
    break;

  case 218: /* argdecls1: '|' slotdeflist '|'  */
#line 1394 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4183 "lang11d_tab.cpp"
    break;

  case 219: /* argdecls1: '|' slotdeflist0 ELLIPSIS name '|'  */
#line 1398 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4191 "lang11d_tab.cpp"
    break;

  case 220: /* argdecls1: '|' slotdeflist0 ELLIPSIS name ',' name '|'  */
#line 1402 "lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 4199 "lang11d_tab.cpp"
    break;

  case 222: /* constdeflist: constdeflist optcomma constdef  */
#line 1410 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4205 "lang11d_tab.cpp"
    break;

  case 223: /* constdef: rspec name '=' slotliteral  */
#line 1414 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4211 "lang11d_tab.cpp"
    break;

  case 224: /* slotdeflist0: %empty  */
#line 1417 "lang11d"
                  { yyval = 0; }
#line 4217 "lang11d_tab.cpp"
    break;

  case 227: /* slotdeflist: slotdeflist optcomma slotdef  */
#line 1423 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4223 "lang11d_tab.cpp"
    break;

  case 228: /* slotdef: name  */
#line 1427 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4229 "lang11d_tab.cpp"
    break;

  case 229: /* slotdef: name optequal slotliteral  */
#line 1429 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4235 "lang11d_tab.cpp"
    break;

  case 230: /* slotdef: name optequal '(' exprseq ')'  */
#line 1431 "lang11d"
                                {
					PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
					node->mParens = 1;
					yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-4], node, 0);
				}
#line 4245 "lang11d_tab.cpp"
    break;

  case 231: /* vardeflist0: %empty  */
#line 1438 "lang11d"
                  { yyval = 0; }
#line 4251 "lang11d_tab.cpp"
    break;

  case 234: /* vardeflist: vardeflist ',' vardef  */
#line 1444 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4257 "lang11d_tab.cpp"
    break;

  case 235: /* vardef: name  */
#line 1448 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4263 "lang11d_tab.cpp"
    break;

  case 236: /* vardef: name '=' expr  */
#line 1450 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4269 "lang11d_tab.cpp"
    break;

  case 237: /* vardef: name '(' exprseq ')'  */
#line 1452 "lang11d"
                                {
									PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
									node->mParens = 1;
									yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-3], node, 0);
								}
#line 4279 "lang11d_tab.cpp"
    break;

  case 238: /* dictslotdef: exprseq ':' exprseq  */
#line 1460 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4285 "lang11d_tab.cpp"
    break;

  case 239: /* dictslotdef: keybinop exprseq  */
#line 1462 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 4294 "lang11d_tab.cpp"
    break;

  case 241: /* dictslotlist1: dictslotlist1 ',' dictslotdef  */
#line 1470 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4300 "lang11d_tab.cpp"
    break;

  case 242: /* dictslotlist: %empty  */
#line 1473 "lang11d"
                  { yyval = 0; }
#line 4306 "lang11d_tab.cpp"
    break;

  case 245: /* rwslotdeflist: rwslotdeflist ',' rwslotdef  */
#line 1479 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4312 "lang11d_tab.cpp"
    break;

  case 246: /* rwslotdef: rwspec name  */
#line 1483 "lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); }
#line 4318 "lang11d_tab.cpp"
    break;

  case 247: /* rwslotdef: rwspec name '=' slotliteral  */
#line 1485 "lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4324 "lang11d_tab.cpp"
    break;

  case 248: /* dictlit2: '(' litdictslotlist ')'  */
#line 1489 "lang11d"
                                { yyval = (intptr_t)newPyrLitDictNode((PyrParseNode*)yyvsp[-1]); }
#line 4330 "lang11d_tab.cpp"
    break;

  case 249: /* litdictslotdef: listliteral ':' listliteral  */
#line 1493 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4336 "lang11d_tab.cpp"
    break;

  case 250: /* litdictslotdef: keybinop listliteral  */
#line 1495 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 4345 "lang11d_tab.cpp"
    break;

  case 252: /* litdictslotlist1: litdictslotlist1 ',' litdictslotdef  */
#line 1503 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4351 "lang11d_tab.cpp"
    break;

  case 253: /* litdictslotlist: %empty  */
#line 1506 "lang11d"
                  { yyval = 0; }
#line 4357 "lang11d_tab.cpp"
    break;

  case 255: /* listlit: '#' '[' literallistc ']'  */
#line 1513 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 4363 "lang11d_tab.cpp"
    break;

  case 256: /* listlit: '#' classname '[' literallistc ']'  */
#line 1515 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4369 "lang11d_tab.cpp"
    break;

  case 257: /* listlit2: '[' literallistc ']'  */
#line 1519 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 4375 "lang11d_tab.cpp"
    break;

  case 258: /* listlit2: classname '[' literallistc ']'  */
#line 1521 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4381 "lang11d_tab.cpp"
    break;

  case 259: /* literallistc: %empty  */
#line 1524 "lang11d"
                  { yyval = 0; }
#line 4387 "lang11d_tab.cpp"
    break;

  case 262: /* literallist1: literallist1 ',' listliteral  */
#line 1530 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4393 "lang11d_tab.cpp"
    break;

  case 263: /* rwspec: %empty  */
#line 1533 "lang11d"
           { yyval = rwPrivate; }
#line 4399 "lang11d_tab.cpp"
    break;

  case 264: /* rwspec: '<'  */
#line 1535 "lang11d"
                        { yyval = rwReadOnly; }
#line 4405 "lang11d_tab.cpp"
    break;

  case 265: /* rwspec: READWRITEVAR  */
#line 1537 "lang11d"
                        { yyval = rwReadWrite; }
#line 4411 "lang11d_tab.cpp"
    break;

  case 266: /* rwspec: '>'  */
#line 1539 "lang11d"
                        { yyval = rwWriteOnly; }
#line 4417 "lang11d_tab.cpp"
    break;

  case 267: /* rspec: %empty  */
#line 1542 "lang11d"
           { yyval = rwPrivate; }
#line 4423 "lang11d_tab.cpp"
    break;

  case 268: /* rspec: '<'  */
#line 1544 "lang11d"
                        { yyval = rwReadOnly; }
#line 4429 "lang11d_tab.cpp"
    break;

  case 269: /* integer: INTEGER  */
#line 1547 "lang11d"
                  { yyval = zzval; }
#line 4435 "lang11d_tab.cpp"
    break;

  case 270: /* integer: '-' INTEGER  */
#line 1549 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawInt(&node->mSlot));
				yyval = zzval;
			}
#line 4446 "lang11d_tab.cpp"
    break;

  case 271: /* floatr: SC_FLOAT  */
#line 1557 "lang11d"
                   { yyval = zzval; }
#line 4452 "lang11d_tab.cpp"
    break;

  case 272: /* floatr: '-' SC_FLOAT  */
#line 1559 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawFloat(&node->mSlot));
				yyval = zzval;
			}
#line 4463 "lang11d_tab.cpp"
    break;

  case 273: /* accidental: ACCIDENTAL  */
#line 1567 "lang11d"
                        { yyval = zzval; }
#line 4469 "lang11d_tab.cpp"
    break;

  case 274: /* accidental: '-' ACCIDENTAL  */
#line 1569 "lang11d"
                                {
					PyrSlotNode *node;
					double intval, fracval;
					node = (PyrSlotNode*)zzval;
					intval = floor(slotRawFloat(&node->mSlot) + 0.5);
					fracval = slotRawFloat(&node->mSlot) - intval;
					SetRaw(&node->mSlot, -intval + fracval);
					yyval = zzval;
				}
#line 4483 "lang11d_tab.cpp"
    break;

  case 275: /* pie: PIE  */
#line 1579 "lang11d"
                      { yyval = zzval; }
#line 4489 "lang11d_tab.cpp"
    break;

  case 278: /* floatp: floatr pie  */
#line 1585 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)yyvsp[-1];
				SetRaw(&node->mSlot, slotRawFloat(&node->mSlot) * pi);
			}
#line 4499 "lang11d_tab.cpp"
    break;

  case 279: /* floatp: integer pie  */
#line 1591 "lang11d"
                        {
				PyrSlotNode *node;
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = slotRawInt(&node->mSlot);
				SetFloat(&node->mSlot, ival * pi);
			}
#line 4511 "lang11d_tab.cpp"
    break;

  case 280: /* floatp: pie  */
#line 1599 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, pi);
				yyval = zzval;
			}
#line 4522 "lang11d_tab.cpp"
    break;

  case 281: /* floatp: '-' pie  */
#line 1606 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, -pi);
				yyval = zzval;
			}
#line 4533 "lang11d_tab.cpp"
    break;

  case 282: /* name: NAME  */
#line 1614 "lang11d"
                       { yyval = zzval; }
#line 4539 "lang11d_tab.cpp"
    break;

  case 283: /* name: WHILE  */
#line 1615 "lang11d"
                                { yyval = zzval; }
#line 4545 "lang11d_tab.cpp"
    break;

  case 284: /* classname: CLASSNAME  */
#line 1618 "lang11d"
                                    { yyval = zzval; }
#line 4551 "lang11d_tab.cpp"
    break;

  case 285: /* primname: PRIMITIVENAME  */
#line 1621 "lang11d"
                                        { yyval = zzval; }
#line 4557 "lang11d_tab.cpp"
    break;

  case 286: /* trueobj: TRUEOBJ  */
#line 1624 "lang11d"
                          { yyval = zzval; }
#line 4563 "lang11d_tab.cpp"
    break;

  case 287: /* falseobj: FALSEOBJ  */
#line 1627 "lang11d"
                           { yyval = zzval; }
#line 4569 "lang11d_tab.cpp"
    break;

  case 288: /* nilobj: NILOBJ  */
#line 1630 "lang11d"
                         { yyval = zzval; }
#line 4575 "lang11d_tab.cpp"
    break;

  case 289: /* ascii: ASCII  */
#line 1633 "lang11d"
                        { yyval = zzval; }
#line 4581 "lang11d_tab.cpp"
    break;

  case 290: /* symbol: SYMBOL  */
#line 1636 "lang11d"
                         { yyval = zzval; }
#line 4587 "lang11d_tab.cpp"
    break;

  case 291: /* string: STRING  */
#line 1639 "lang11d"
                         { yyval = zzval; }
#line 4593 "lang11d_tab.cpp"
    break;

  case 292: /* pseudovar: PSEUDOVAR  */
#line 1642 "lang11d"
                            { yyval = zzval; }
#line 4599 "lang11d_tab.cpp"
    break;

  case 293: /* binop: BINOP  */
#line 1645 "lang11d"
                { yyval = zzval; }
#line 4605 "lang11d_tab.cpp"
    break;

  case 294: /* binop: READWRITEVAR  */
#line 1646 "lang11d"
                               { yyval = zzval; }
#line 4611 "lang11d_tab.cpp"
    break;

  case 295: /* binop: '<'  */
#line 1647 "lang11d"
                       { yyval = zzval; }
#line 4617 "lang11d_tab.cpp"
    break;

  case 296: /* binop: '>'  */
#line 1648 "lang11d"
                       { yyval = zzval; }
#line 4623 "lang11d_tab.cpp"
    break;

  case 297: /* binop: '-'  */
#line 1649 "lang11d"
                       { yyval = zzval; }
#line 4629 "lang11d_tab.cpp"
    break;

  case 298: /* binop: '*'  */
#line 1650 "lang11d"
                       { yyval = zzval; }
#line 4635 "lang11d_tab.cpp"
    break;

  case 299: /* binop: '+'  */
#line 1651 "lang11d"
                       { yyval = zzval; }
#line 4641 "lang11d_tab.cpp"
    break;

  case 300: /* binop: '|'  */
#line 1652 "lang11d"
                       { yyval = zzval; }
#line 4647 "lang11d_tab.cpp"
    break;

  case 301: /* keybinop: KEYBINOP  */
#line 1655 "lang11d"
                    { yyval = zzval; }
#line 4653 "lang11d_tab.cpp"
    break;

  case 304: /* curryarg: CURRYARG  */
#line 1662 "lang11d"
                    { yyval = zzval; }
#line 4659 "lang11d_tab.cpp"
    break;


#line 4663 "lang11d_tab.cpp"

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

