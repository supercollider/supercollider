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
  YYSYMBOL_EXIT = 30,                      /* EXIT  */
  YYSYMBOL_31_ = 31,                       /* ':'  */
  YYSYMBOL_32_ = 32,                       /* '='  */
  YYSYMBOL_BINOP = 33,                     /* BINOP  */
  YYSYMBOL_KEYBINOP = 34,                  /* KEYBINOP  */
  YYSYMBOL_35_ = 35,                       /* '-'  */
  YYSYMBOL_36_ = 36,                       /* '<'  */
  YYSYMBOL_37_ = 37,                       /* '>'  */
  YYSYMBOL_38_ = 38,                       /* '*'  */
  YYSYMBOL_39_ = 39,                       /* '+'  */
  YYSYMBOL_40_ = 40,                       /* '|'  */
  YYSYMBOL_READWRITEVAR = 41,              /* READWRITEVAR  */
  YYSYMBOL_42_ = 42,                       /* '.'  */
  YYSYMBOL_43_ = 43,                       /* '`'  */
  YYSYMBOL_UMINUS = 44,                    /* UMINUS  */
  YYSYMBOL_45_ = 45,                       /* '{'  */
  YYSYMBOL_46_ = 46,                       /* '}'  */
  YYSYMBOL_47_ = 47,                       /* '['  */
  YYSYMBOL_48_ = 48,                       /* ']'  */
  YYSYMBOL_49_ = 49,                       /* ';'  */
  YYSYMBOL_50_ = 50,                       /* ','  */
  YYSYMBOL_51_ = 51,                       /* '('  */
  YYSYMBOL_52_ = 52,                       /* ')'  */
  YYSYMBOL_53_ = 53,                       /* '^'  */
  YYSYMBOL_54_ = 54,                       /* '~'  */
  YYSYMBOL_55_ = 55,                       /* '#'  */
  YYSYMBOL_YYACCEPT = 56,                  /* $accept  */
  YYSYMBOL_root = 57,                      /* root  */
  YYSYMBOL_classes = 58,                   /* classes  */
  YYSYMBOL_classextensions = 59,           /* classextensions  */
  YYSYMBOL_classdef = 60,                  /* classdef  */
  YYSYMBOL_classextension = 61,            /* classextension  */
  YYSYMBOL_optname = 62,                   /* optname  */
  YYSYMBOL_superclass = 63,                /* superclass  */
  YYSYMBOL_classvardecls = 64,             /* classvardecls  */
  YYSYMBOL_classvardecl = 65,              /* classvardecl  */
  YYSYMBOL_methods = 66,                   /* methods  */
  YYSYMBOL_methoddef = 67,                 /* methoddef  */
  YYSYMBOL_optsemi = 68,                   /* optsemi  */
  YYSYMBOL_optcomma = 69,                  /* optcomma  */
  YYSYMBOL_optequal = 70,                  /* optequal  */
  YYSYMBOL_funcbody = 71,                  /* funcbody  */
  YYSYMBOL_cmdlinecode = 72,               /* cmdlinecode  */
  YYSYMBOL_methbody = 73,                  /* methbody  */
  YYSYMBOL_primitive = 74,                 /* primitive  */
  YYSYMBOL_retval = 75,                    /* retval  */
  YYSYMBOL_funretval = 76,                 /* funretval  */
  YYSYMBOL_blocklist1 = 77,                /* blocklist1  */
  YYSYMBOL_blocklistitem = 78,             /* blocklistitem  */
  YYSYMBOL_blocklist = 79,                 /* blocklist  */
  YYSYMBOL_msgsend = 80,                   /* msgsend  */
  YYSYMBOL_generator = 81,                 /* generator  */
  YYSYMBOL_82_1 = 82,                      /* $@1  */
  YYSYMBOL_83_2 = 83,                      /* $@2  */
  YYSYMBOL_nextqual = 84,                  /* nextqual  */
  YYSYMBOL_qual = 85,                      /* qual  */
  YYSYMBOL_expr1 = 86,                     /* expr1  */
  YYSYMBOL_valrangex1 = 87,                /* valrangex1  */
  YYSYMBOL_valrangeassign = 88,            /* valrangeassign  */
  YYSYMBOL_valrangexd = 89,                /* valrangexd  */
  YYSYMBOL_valrange2 = 90,                 /* valrange2  */
  YYSYMBOL_valrange3 = 91,                 /* valrange3  */
  YYSYMBOL_expr = 92,                      /* expr  */
  YYSYMBOL_adverb = 93,                    /* adverb  */
  YYSYMBOL_exprn = 94,                     /* exprn  */
  YYSYMBOL_exprseq = 95,                   /* exprseq  */
  YYSYMBOL_arrayelems = 96,                /* arrayelems  */
  YYSYMBOL_arrayelems1 = 97,               /* arrayelems1  */
  YYSYMBOL_arglist1 = 98,                  /* arglist1  */
  YYSYMBOL_arglistv1 = 99,                 /* arglistv1  */
  YYSYMBOL_keyarglist1 = 100,              /* keyarglist1  */
  YYSYMBOL_keyarg = 101,                   /* keyarg  */
  YYSYMBOL_optkeyarglist = 102,            /* optkeyarglist  */
  YYSYMBOL_mavars = 103,                   /* mavars  */
  YYSYMBOL_mavarlist = 104,                /* mavarlist  */
  YYSYMBOL_slotliteral = 105,              /* slotliteral  */
  YYSYMBOL_blockliteral = 106,             /* blockliteral  */
  YYSYMBOL_pushname = 107,                 /* pushname  */
  YYSYMBOL_pushliteral = 108,              /* pushliteral  */
  YYSYMBOL_listliteral = 109,              /* listliteral  */
  YYSYMBOL_block = 110,                    /* block  */
  YYSYMBOL_funcvardecls = 111,             /* funcvardecls  */
  YYSYMBOL_funcvardecls1 = 112,            /* funcvardecls1  */
  YYSYMBOL_funcvardecl = 113,              /* funcvardecl  */
  YYSYMBOL_argdecls = 114,                 /* argdecls  */
  YYSYMBOL_argdecls1 = 115,                /* argdecls1  */
  YYSYMBOL_constdeflist = 116,             /* constdeflist  */
  YYSYMBOL_constdef = 117,                 /* constdef  */
  YYSYMBOL_slotdeflist0 = 118,             /* slotdeflist0  */
  YYSYMBOL_slotdeflist = 119,              /* slotdeflist  */
  YYSYMBOL_slotdef = 120,                  /* slotdef  */
  YYSYMBOL_vardeflist0 = 121,              /* vardeflist0  */
  YYSYMBOL_vardeflist = 122,               /* vardeflist  */
  YYSYMBOL_vardef = 123,                   /* vardef  */
  YYSYMBOL_dictslotdef = 124,              /* dictslotdef  */
  YYSYMBOL_dictslotlist1 = 125,            /* dictslotlist1  */
  YYSYMBOL_dictslotlist = 126,             /* dictslotlist  */
  YYSYMBOL_rwslotdeflist = 127,            /* rwslotdeflist  */
  YYSYMBOL_rwslotdef = 128,                /* rwslotdef  */
  YYSYMBOL_dictlit2 = 129,                 /* dictlit2  */
  YYSYMBOL_litdictslotdef = 130,           /* litdictslotdef  */
  YYSYMBOL_litdictslotlist1 = 131,         /* litdictslotlist1  */
  YYSYMBOL_litdictslotlist = 132,          /* litdictslotlist  */
  YYSYMBOL_listlit = 133,                  /* listlit  */
  YYSYMBOL_listlit2 = 134,                 /* listlit2  */
  YYSYMBOL_literallistc = 135,             /* literallistc  */
  YYSYMBOL_literallist1 = 136,             /* literallist1  */
  YYSYMBOL_rwspec = 137,                   /* rwspec  */
  YYSYMBOL_rspec = 138,                    /* rspec  */
  YYSYMBOL_integer = 139,                  /* integer  */
  YYSYMBOL_floatr = 140,                   /* floatr  */
  YYSYMBOL_accidental = 141,               /* accidental  */
  YYSYMBOL_pie = 142,                      /* pie  */
  YYSYMBOL_floatp = 143,                   /* floatp  */
  YYSYMBOL_name = 144,                     /* name  */
  YYSYMBOL_classname = 145,                /* classname  */
  YYSYMBOL_primname = 146,                 /* primname  */
  YYSYMBOL_trueobj = 147,                  /* trueobj  */
  YYSYMBOL_falseobj = 148,                 /* falseobj  */
  YYSYMBOL_nilobj = 149,                   /* nilobj  */
  YYSYMBOL_ascii = 150,                    /* ascii  */
  YYSYMBOL_symbol = 151,                   /* symbol  */
  YYSYMBOL_string = 152,                   /* string  */
  YYSYMBOL_pseudovar = 153,                /* pseudovar  */
  YYSYMBOL_binop = 154,                    /* binop  */
  YYSYMBOL_keybinop = 155,                 /* keybinop  */
  YYSYMBOL_binop2 = 156,                   /* binop2  */
  YYSYMBOL_curryarg = 157                  /* curryarg  */
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
#define YYFINAL  71
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2011

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  56
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  102
/* YYNRULES -- Number of rules.  */
#define YYNRULES  303
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  566

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   289


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
       2,     2,     2,     2,     2,    55,     2,     2,     2,     2,
      51,    52,    38,    39,    50,    35,    42,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    31,    49,
      36,    32,    37,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    47,     2,    48,    53,     2,    43,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    45,    40,    46,    54,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    33,    34,    41,    44
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
    1309,  1312,  1315,  1320,  1321,  1325,  1326,  1330,  1334,  1335,
    1339,  1343,  1347,  1351,  1357,  1361,  1365,  1369,  1373,  1380,
    1381,  1385,  1389,  1390,  1393,  1394,  1398,  1400,  1402,  1410,
    1411,  1414,  1415,  1419,  1421,  1423,  1431,  1433,  1440,  1441,
    1445,  1446,  1449,  1450,  1454,  1456,  1460,  1464,  1466,  1473,
    1474,  1478,  1479,  1484,  1486,  1490,  1492,  1496,  1497,  1500,
    1501,  1505,  1506,  1508,  1510,  1514,  1515,  1519,  1520,  1529,
    1530,  1539,  1540,  1551,  1554,  1555,  1556,  1562,  1570,  1577,
    1586,  1587,  1588,  1591,  1594,  1597,  1600,  1603,  1606,  1609,
    1612,  1615,  1618,  1619,  1620,  1621,  1622,  1623,  1624,  1625,
    1628,  1631,  1632,  1635
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
  "LEFTARROW", "WHILE", "EXIT", "':'", "'='", "BINOP", "KEYBINOP", "'-'",
  "'<'", "'>'", "'*'", "'+'", "'|'", "READWRITEVAR", "'.'", "'`'",
  "UMINUS", "'{'", "'}'", "'['", "']'", "';'", "','", "'('", "')'", "'^'",
  "'~'", "'#'", "$accept", "root", "classes", "classextensions",
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

#define YYPACT_NINF (-509)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-300)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      62,   952,    27,    86,    27,    51,  -509,  -509,  -509,  -509,
    -509,  -509,  -509,  -509,  -509,  -509,   241,   241,  -509,  -509,
    -509,  -509,  -509,     2,  -509,  -509,   280,   241,  1800,    67,
    1429,   846,  1800,   241,   206,  -509,  -509,  -509,  -509,  -509,
      -6,  -509,  -509,  -509,  1913,    42,    52,  -509,  -509,  -509,
    -509,  1164,  -509,  1164,  -509,    91,    91,  -509,  -509,  -509,
     249,   346,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,
      82,  -509,  -509,    77,  -509,   266,  -509,     3,   112,   276,
     241,   241,  -509,  -509,  -509,  -509,  -509,   118,     6,  -509,
     269,   899,  -509,  1800,  1800,  -509,  -509,   131,   101,   123,
    1800,  1800,  1482,  -509,   280,  -509,  -509,  -509,  -509,    28,
    -509,   132,   252,  1164,  1164,  -509,   139,   144,  -509,  1800,
     146,   783,   176,  1909,   181,    12,  -509,   168,  1535,  -509,
    -509,    31,  -509,   184,  1800,  -509,  -509,  -509,  -509,  -509,
    1164,  -509,  -509,  1800,  1217,    44,  -509,  -509,  -509,  1429,
    1005,    44,  -509,    27,   241,   179,  -509,   241,  1800,  1800,
     241,  -509,   207,   306,   211,   142,  1164,   241,  -509,  -509,
     241,  -509,   780,  -509,  -509,  1164,  1800,  -509,  1429,  -509,
    -509,  -509,  1800,   185,    61,  -509,  1800,  1800,  1800,  -509,
     194,   196,  1164,  1429,  -509,  -509,  -509,   215,  -509,  -509,
    1800,  1909,  1874,  -509,  -509,  -509,   208,   205,    91,  -509,
    -509,   221,  -509,  -509,  -509,  -509,  -509,  -509,  1800,   241,
     241,  1909,  1800,  -509,   133,  1588,  1058,   298,    41,  1800,
    1913,  -509,  1913,  1800,    44,   225,   230,  -509,   234,    44,
     225,   230,   238,  -509,  1800,  1928,  -509,   258,  -509,  -509,
    -509,  1913,   245,   209,   241,  -509,   241,  -509,   277,  -509,
      -1,  -509,  1800,    18,  -509,  -509,    91,  -509,  -509,  -509,
    -509,  -509,  -509,  -509,   281,   284,   286,  -509,   307,  1800,
    -509,  -509,  1800,  1800,  -509,  -509,   319,  -509,  -509,   300,
     340,  -509,  1800,  1270,    44,  1913,   324,   347,  -509,   329,
     328,  1909,  -509,  1909,  -509,  1909,  1913,  -509,  -509,   335,
     336,  1641,   355,  1800,  1800,   172,    44,   225,   230,   238,
    1800,  1111,    44,  -509,   384,  1800,  -509,  -509,   350,  -509,
      44,  1323,  -509,   337,   360,   349,  -509,  -509,   357,   361,
     360,   367,  -509,  1970,  -509,  -509,   354,   375,   371,   292,
    -509,  -509,   372,     0,  -509,  -509,   241,   374,  1376,  1376,
    -509,  1800,  -509,  -509,   401,  1800,  -509,    44,   225,   230,
    -509,  1909,  1874,  -509,  -509,  -509,  -509,   382,  -509,   405,
     406,   392,  1800,  -509,   394,  1694,   414,  -509,   395,   396,
     397,  1913,    44,   225,   230,   238,   399,  1800,   238,    78,
    -509,    44,  -509,  -509,    44,   410,   411,     2,     2,   415,
     182,   182,   431,  -509,  1942,  -509,  -509,   241,   429,  -509,
     241,   140,   424,   426,   521,   428,  -509,  1800,  -509,    44,
     433,   436,  -509,  -509,  -509,  1800,  1800,   446,  1913,   457,
     462,   447,  1800,    44,  -509,    44,  -509,   445,   448,   450,
    -509,  -509,  -509,  1800,  -509,  -509,  -509,     2,     2,  -509,
    -509,  -509,  -509,  -509,  -509,   310,  -509,   241,   312,  -509,
     318,  -509,   241,  -509,   458,  -509,   471,  1800,  1800,  -509,
    1376,  -509,  1800,   476,  -509,  -509,    44,  -509,  1913,  1913,
    1800,  1800,  1800,   473,  1913,  -509,  -509,    44,  -509,    44,
    1913,  -509,  -509,   109,   109,   292,  -509,   182,   474,  -509,
    -509,   431,   475,  -509,  1800,   426,   426,  -509,   426,  1800,
    -509,  1913,  1913,  1913,  1800,  -509,  -509,   109,   109,  -509,
    1747,   460,  1747,  1956,  -509,  1852,  -509,  1852,   426,  -509,
    -509,  -509,   426,  1913,  1747,  1747,  1800,   464,  -509,   461,
    -509,   470,  -509,  -509,  -509,  -509,  -509,   477,   480,   783,
    -509,  -509,  -509,  -509,  -509,  -509
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       5,    48,     0,     0,     2,     3,     7,   280,   267,   269,
     271,   289,   290,   288,   283,   303,     0,   229,   287,   285,
     286,   291,   273,   208,   281,   282,     0,   222,     0,   208,
     149,   240,     0,     0,     0,    41,     4,    33,    96,    93,
     128,   105,   130,   129,   146,    27,    48,    92,    94,    91,
     179,    48,   205,    48,   189,   181,   274,   275,   278,   182,
     180,   131,   186,   187,   188,   183,   185,   184,   103,    95,
       0,     1,     6,    14,     8,     0,   231,   233,     0,   230,
     229,   222,   203,   268,   270,   272,   279,     0,    29,   224,
      31,   240,   133,     0,     0,   203,   300,   151,     0,    29,
       0,     0,     0,   292,   296,   294,   295,   297,   298,   222,
     293,     0,     0,    48,    48,   238,    29,     0,   301,   302,
       0,    27,    98,   257,     0,   166,   168,     0,     0,   296,
     299,     0,   302,   142,    28,   148,    34,    40,   206,    39,
      48,   277,   276,     0,     0,    56,    50,    53,    52,   149,
       0,    65,    21,     0,    12,     0,   207,     0,     0,     0,
       0,   214,     0,   230,     0,    29,    48,     0,   216,    30,
       0,    32,     0,    79,    81,    48,     0,    99,    30,   150,
     153,   119,     0,     0,     0,   100,   118,     0,     0,    97,
       0,     0,    48,    30,   241,   102,   237,     0,    28,    49,
       0,   257,   251,   259,   200,   199,     0,    29,   190,   191,
     195,     0,   196,   197,   198,   192,   194,   193,     0,     0,
       0,   257,     0,   157,     0,     0,     0,    54,     0,     0,
     147,    38,   135,     0,     0,    29,    29,    51,     0,    54,
      29,    29,    29,   161,     0,     0,    15,     0,    13,    16,
     232,   234,     0,     0,     0,   209,     0,   211,     0,   204,
       0,   225,     0,     0,   227,   178,   170,   171,   175,   176,
     177,   172,   174,   173,     0,     0,     0,   152,   154,     0,
     123,   101,   124,     0,   120,   236,     0,    37,    36,     0,
       0,   239,     0,     0,    57,   136,     0,     0,   249,    29,
       0,     0,   253,    30,   258,   257,   139,   167,   169,     0,
       0,     0,   104,     0,     0,     0,    54,    29,    29,    29,
       0,     0,    55,    78,     0,     0,   144,   143,   134,   159,
      58,    30,   164,     0,    30,     0,    64,    66,     0,     0,
      30,     0,   163,   297,    11,    22,     0,     0,    14,    21,
     235,   215,     0,     0,   202,   217,     0,     0,     0,     0,
     201,     0,   155,   125,     0,   122,    35,     0,    29,    29,
     255,     0,    30,   252,   246,   248,   260,     0,   254,   107,
     106,     0,     0,   158,     0,     0,   132,    70,     0,     0,
       0,   137,    54,    29,    29,    29,     0,     0,    29,    54,
      62,    54,    69,   162,    54,     0,     0,   208,   208,     0,
     261,   261,   265,    17,     0,   210,   212,     0,     0,   228,
       0,     0,     0,    83,   180,     0,   156,   126,   121,    60,
       0,     0,   247,   250,   256,     0,     0,   108,   140,   113,
     112,     0,     0,    54,    74,    54,    75,     0,     0,     0,
     145,   160,   165,     0,    59,    68,    67,   208,   208,   203,
     203,    16,   262,   264,   263,     0,   242,     0,     0,   266,
      29,   219,     0,     9,     0,   218,     0,     0,     0,    80,
       0,    88,     0,     0,    82,   127,    54,    63,   110,   109,
       0,     0,     0,   114,   141,    73,    71,    54,    77,    54,
     138,   203,   203,    44,    44,    21,    19,   261,   244,    18,
      20,   265,     0,   213,     0,    83,    83,    84,    83,     0,
      61,   111,   116,   115,     0,    76,    72,    44,    44,   284,
      46,    27,    46,     0,   243,     0,   220,     0,    83,    90,
      89,    85,    83,   117,    46,    46,     0,     0,    42,    46,
      45,     0,    10,   245,   221,    87,    86,     0,     0,    27,
      23,    43,    25,    24,    26,    47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -509,  -509,  -509,  -509,  -509,   512,  -509,   174,    59,  -509,
    -337,  -509,  -115,   366,  -509,    46,  -509,  -508,  -294,   -19,
     481,   -43,  -119,   282,  -509,   218,  -509,  -509,  -165,  -336,
    -509,  -509,  -509,  -509,  -509,  -509,   -28,  -509,  -509,   246,
     383,  -509,   -91,  -141,  -142,   193,   389,  -509,  -509,  -359,
     253,  -509,  -509,  -172,  -509,   -84,   -10,    24,   -22,   504,
    -509,    30,   463,   465,   373,   472,    -7,   385,   362,  -509,
    -509,   143,    49,  -509,   186,  -509,  -509,  -171,  -509,  -173,
    -509,  -509,  -509,  -108,  -509,  -509,    -4,   -76,   -14,   439,
    -509,   -60,   -54,   -51,   -44,   -36,   210,  -509,  -240,   506,
     -17,  -509
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     3,     4,     5,    72,     6,   247,   155,   349,   413,
     245,   345,   135,   332,   172,    35,    36,   547,   530,   548,
      37,   322,   146,   323,    38,    39,   274,   275,   481,   422,
      40,    41,    42,    43,   111,   183,    44,   229,    45,    46,
      98,    99,   224,   236,   398,   243,   333,   124,   125,   264,
      47,    48,    49,   203,    50,   166,    51,   259,    82,    53,
     470,   471,    87,    88,    89,    78,    75,    76,   115,   116,
     117,   465,   466,   204,   298,   299,   300,    54,   205,   206,
     207,   467,   472,    55,    56,    57,    58,    59,    60,    61,
     531,    62,    63,    64,    65,    66,    67,    68,   118,   132,
     133,    69
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      92,   265,    77,    77,   121,   347,   199,    95,   242,   241,
      79,   175,   414,    90,   120,   208,    80,   145,   151,   122,
     126,   113,    86,   425,   551,    52,   237,  -223,   296,    14,
     297,     7,   237,   219,     7,   158,   557,   558,    14,   355,
     416,   128,    81,   140,     7,     8,   168,   209,   309,   356,
     417,   141,   142,   235,   159,    52,   169,    24,    25,   240,
      24,    25,   220,   212,   266,   123,    77,    90,    23,   213,
      24,    25,   214,   163,   120,   138,   324,    52,   225,   215,
    -299,    80,   226,   282,   319,   318,    71,   216,     1,    29,
       2,   134,   325,   208,   208,    90,   267,   137,    93,   139,
      86,     2,    23,   406,   192,    32,   230,    81,   153,   210,
     453,   283,   268,   208,    22,   232,    94,   227,   269,   529,
     326,   270,    16,    29,   154,   209,   209,   152,   271,   375,
     251,   376,   377,   160,   315,   317,   272,   138,    52,   167,
     248,   212,   212,    77,   517,   209,   253,   213,   213,   177,
     214,   214,   369,   260,   294,   311,    90,   215,   215,   190,
     191,   212,   176,  -223,   138,   216,   216,   213,   533,   477,
     214,   478,   295,   178,   347,   237,   553,   215,   554,   395,
     394,   312,   257,   313,   185,   216,   231,   210,   210,   193,
     306,   330,   169,   208,   385,   208,   195,   208,   197,   432,
     297,   328,   368,   237,   141,   307,   308,   210,   200,     7,
     532,   237,   258,   218,   327,   221,   138,    14,   462,   463,
     386,   276,   313,   464,   249,   209,   228,   209,   254,   209,
     393,   346,   256,   544,   545,    24,    25,   281,   289,    23,
     352,   212,   353,   212,     7,   212,   287,   213,   288,   213,
     214,   213,   214,   123,   214,   303,   302,   215,   351,   215,
      29,   215,   141,   208,   208,   216,   293,   216,   305,   216,
      24,    25,  -226,    23,   186,   331,    97,   112,   147,   147,
     334,   143,   336,   187,    83,    84,    85,   210,   340,   210,
    -226,   210,   391,   347,    29,   209,   209,   350,  -226,  -226,
     144,   171,   188,    22,   189,   410,   348,   411,   412,  -226,
     237,   212,   212,   148,   148,   156,   157,   213,   213,  -226,
     214,   214,    23,   354,   429,   161,   157,   215,   215,   405,
     320,   358,   360,   217,   359,   216,   216,   112,   361,   173,
     174,   365,   418,    29,   424,   424,   180,   181,   184,   321,
     539,   540,   366,   541,   438,   255,   157,   210,   210,   506,
     507,   509,   507,   147,   265,   196,   265,   510,   169,   147,
      23,   187,   370,   555,   223,   503,   504,   556,   371,   372,
     374,   145,   273,   378,   379,   459,   460,   382,    83,   399,
     223,    29,   131,   149,    96,    97,   223,   150,   148,   407,
     346,   400,   153,   474,   148,   252,   476,   488,   489,   401,
     483,   217,   217,   402,   494,   147,   550,   527,   528,   404,
     408,   415,   277,   427,   278,   500,   419,   266,   280,   266,
     434,   217,   284,   285,   286,   501,   502,   435,   436,   290,
     437,    70,   439,    73,   565,   147,   442,   443,   444,   445,
     148,   450,   147,   508,   170,   457,   458,   147,   512,   267,
     461,   267,   521,   522,   523,   179,   424,   469,   310,   475,
     479,   223,   223,   127,   484,   268,   480,   268,   490,   329,
     148,   269,   194,   269,   270,   486,   270,   148,   487,   491,
     342,   271,   148,   271,   492,   493,   543,   497,   513,   272,
     498,   272,   499,   514,   519,   524,   535,   537,   357,   198,
     560,   217,   147,   217,   546,   217,   562,    74,   559,   346,
     505,   337,   409,   563,     7,   362,   564,   136,   363,   364,
     561,   170,   238,   403,   147,   114,   100,   119,   196,   223,
     147,   536,   250,   261,   164,    23,   165,   148,   147,   482,
      24,    25,   162,   143,   468,   291,   534,   381,   433,   383,
     384,     0,   211,     0,     0,     0,    29,   223,     0,   148,
       0,   396,   144,   304,     0,   148,     0,   383,     0,     0,
       0,   217,   217,   148,     0,   147,     0,     0,     0,     0,
       0,     0,   246,     0,     0,     0,     0,   119,   387,     0,
       0,     0,     0,     0,   423,   423,     0,   426,   341,     0,
     147,   428,     0,     0,     0,     0,     0,   147,     0,   147,
     148,     0,   147,     0,     0,   335,     0,     0,     0,   338,
     339,   441,     0,     0,     0,     0,     0,     0,     0,     0,
     211,   211,   147,   451,     0,   148,     0,   147,     0,     0,
       0,     0,   148,     0,   148,   100,   244,   148,     0,     0,
     211,   147,     0,   147,     0,   373,     0,     0,     0,     0,
       0,     0,     0,   485,   446,     0,     0,   148,     0,     0,
       0,   454,   148,   455,   279,   390,   456,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   148,     0,   148,   292,
       0,     0,   127,     0,   147,     0,   388,   389,   301,     0,
       0,     0,     0,     0,     0,   147,     0,   147,     0,     0,
       0,     0,     0,   515,   516,   495,   423,   496,   518,     0,
       0,     0,   244,     0,     0,     0,     0,     0,     0,   148,
     211,     0,   211,     0,   211,   273,     0,   273,     0,     0,
     148,     0,   148,     0,     0,     0,     0,   430,   431,     0,
     538,   449,     0,     0,   452,   542,     0,     0,   520,     0,
       0,     0,     0,     0,     0,     0,   549,     0,   549,   525,
       0,   526,   447,   448,     8,     9,    10,    11,    12,    13,
     549,   549,     0,     0,     0,     0,     0,    18,    19,    20,
       0,     0,     0,    22,     0,     0,     0,     0,     0,     0,
     211,   211,     0,     0,     0,    26,   103,    96,   129,   105,
     106,   107,   108,   130,   110,   131,     0,   244,     0,     0,
       0,   262,   198,     0,     0,   263,   511,   244,     0,     0,
     244,     0,     0,     0,     0,     0,   244,     0,     0,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,    16,
      17,     0,     0,    18,    19,    20,    21,     0,   101,    22,
      23,     0,     0,     0,     0,    24,    25,   102,   301,   103,
      96,   104,   105,   106,   107,   108,   109,   110,     0,    28,
       0,    29,     0,    30,     0,     0,     0,    91,     0,     0,
      33,    34,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    18,    19,    20,    21,
       0,   101,    22,    23,     0,     0,     0,     0,    24,    25,
     102,     0,   103,    96,   104,   105,   106,   107,   108,   130,
     110,     0,    28,     0,    29,     0,    30,     0,     0,     0,
      91,     0,     0,    33,    34,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,    16,    17,     0,     0,    18,
      19,    20,    21,     0,     0,    22,    23,     0,     0,     0,
       0,    24,    25,     0,     0,     0,     0,    26,     0,     0,
       0,     0,    27,     0,     0,    28,     0,    29,     0,    30,
       0,     0,     0,    31,     0,    32,    33,    34,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,     0,     0,
       0,     0,    18,    19,    20,    21,     0,     0,    22,    23,
       0,     0,     0,     0,    24,    25,     0,     0,     0,    96,
      26,     0,     0,   233,     0,     0,     0,     0,    28,     0,
      29,     0,    30,     0,     0,     0,    91,   239,     0,    33,
      34,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,     0,     0,     0,     0,    18,    19,    20,    21,     0,
       0,    22,    23,     0,     0,     0,     0,    24,    25,     0,
       0,     0,    96,    26,     0,     0,   233,     0,     0,     0,
       0,    28,     0,    29,     0,    30,     0,     0,     0,    91,
     316,     0,    33,    34,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    18,    19,
      20,    21,     0,     0,    22,    23,     0,     0,     0,     0,
      24,    25,     0,     0,     0,    96,    26,     0,     0,   233,
       0,     0,     0,     0,    28,     0,    29,     0,    30,     0,
       0,     0,    91,   392,     0,    33,    34,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,    16,     0,     0,
       0,    18,    19,    20,    21,     0,     0,    22,    23,     0,
       0,     0,     0,    24,    25,     0,     0,     0,     0,    26,
       0,     0,     0,     0,     0,     0,     0,    28,     0,    29,
       0,    30,     0,     0,     0,    91,     0,    32,    33,    34,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    18,    19,    20,    21,     0,     0,
      22,    23,     0,     0,     0,     0,    24,    25,     0,     0,
       0,     0,    26,     0,     0,   233,     0,     0,     0,     0,
      28,     0,    29,     0,    30,     0,     0,     0,    91,   234,
       0,    33,    34,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,     0,     0,     0,     0,    18,    19,    20,
      21,     0,     0,    22,    23,     0,     0,     0,     0,    24,
      25,     0,     0,     0,     0,    26,     0,     0,   233,     0,
       0,     0,     0,    28,     0,    29,     0,    30,     0,     0,
       0,    91,   367,     0,    33,    34,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,     0,     0,     0,     0,
      18,    19,    20,    21,     0,     0,    22,    23,     0,     0,
       0,     0,    24,    25,     0,     0,     0,    96,    26,     0,
       0,   397,     0,     0,     0,     0,    28,     0,    29,     0,
      30,     0,     0,     0,    91,     0,     0,    33,    34,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,   420,
       0,     0,     0,    18,    19,    20,    21,     0,     0,    22,
      23,     0,     0,     0,     0,    24,    25,   421,     0,     0,
       0,    26,     0,     0,     0,     0,     0,     0,     0,    28,
       0,    29,     0,    30,     0,     0,     0,    91,     0,     0,
      33,    34,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    18,    19,    20,    21,
       0,     0,    22,    23,     0,     0,     0,     0,    24,    25,
       0,     0,     0,    96,    26,     0,     0,     0,     0,     0,
       0,     0,    28,     0,    29,     0,    30,     0,     0,     0,
      91,     0,     0,    33,    34,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,     0,     0,     0,     0,    18,
      19,    20,    21,     0,   182,    22,    23,     0,     0,     0,
       0,    24,    25,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,     0,     0,    28,     0,    29,     0,    30,
       0,     0,     0,    91,     0,     0,    33,    34,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,     0,     0,
       0,     0,    18,    19,    20,    21,     0,   222,    22,    23,
       0,     0,     0,     0,    24,    25,     0,     0,     0,     0,
      26,     0,     0,     0,     0,     0,     0,     0,    28,     0,
      29,     0,    30,     0,     0,     0,    91,     0,     0,    33,
      34,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,     0,     0,     0,     0,    18,    19,    20,    21,     0,
     314,    22,    23,     0,     0,     0,     0,    24,    25,     0,
       0,     0,     0,    26,     0,     0,     0,     0,     0,     0,
       0,    28,     0,    29,     0,    30,     0,     0,     0,    91,
       0,     0,    33,    34,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    18,    19,
      20,    21,     0,     0,    22,    23,     0,     0,     0,     0,
      24,    25,     0,     0,     0,     0,    26,     0,     0,     0,
       0,     0,     0,     0,    28,     0,    29,     0,    30,   380,
       0,     0,    91,     0,     0,    33,    34,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,     0,     0,     0,
       0,    18,    19,    20,    21,     0,     0,    22,    23,     0,
       0,     0,     0,    24,    25,     0,     0,     0,     0,    26,
       0,     0,     0,     0,     0,     0,     0,    28,     0,    29,
       0,    30,   440,     0,     0,    91,     0,     0,    33,    34,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    18,    19,    20,    21,     0,     0,
      22,    23,     0,     0,     0,     0,    24,    25,     0,     0,
       0,     0,    26,     0,     0,     0,     0,     0,     0,     0,
      28,     0,    29,     0,    30,     0,     0,     0,    91,     0,
     546,    33,    34,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,     0,     0,     0,     0,    18,    19,    20,
      21,     0,     0,    22,    23,     0,     0,     0,     0,    24,
      25,     0,     0,     0,     0,    26,     0,     0,     0,     0,
       0,     0,     0,    28,     0,    29,     0,    30,     0,     0,
       0,    91,     0,     0,    33,    34,     8,     9,    10,    11,
      12,    13,     0,     0,     0,     0,     0,     0,     0,    18,
      19,    20,     0,     0,     0,    22,     0,     7,     8,     9,
      10,    11,    12,    13,     0,    14,     0,    26,     0,     0,
       0,    18,    19,    20,     0,     0,     0,    22,     0,     0,
       0,     0,     0,    24,    25,     0,     0,   263,    96,    26,
       0,     0,     7,     8,     9,    10,    11,    12,    13,     0,
      14,   201,     0,     0,     0,   202,    18,    19,    20,     0,
       0,     7,    22,     0,     0,     0,     0,     0,    24,    25,
       0,     0,     0,     0,    26,     7,   103,    96,   129,   105,
     106,   107,   108,   130,   110,   131,   201,    24,    25,     7,
     202,   103,     0,   129,   105,   106,   343,   108,   130,   110,
       0,    24,    25,     7,   344,   103,     0,   129,   105,   106,
     343,   108,   130,   110,     0,    24,    25,     0,   473,   103,
       0,   129,   105,   106,   343,   108,   130,   110,     0,    24,
      25,     0,   552,   103,     0,   129,   105,   106,   107,   108,
     130,   110
};

static const yytype_int16 yycheck[] =
{
      28,   172,    16,    17,    32,   245,   121,    29,   150,   150,
      17,    95,   349,    27,    31,   123,    14,    60,    61,    33,
      34,    31,    26,   359,   532,     1,   145,    21,   201,    11,
     202,     3,   151,    21,     3,    32,   544,   545,    11,    40,
      40,    47,    40,    53,     3,     4,    40,   123,   221,    50,
      50,    55,    56,   144,    51,    31,    50,    29,    30,   150,
      29,    30,    50,   123,   172,    47,    80,    81,    24,   123,
      29,    30,   123,    80,    91,    51,    35,    53,    47,   123,
      52,    14,    51,    22,   226,   226,     0,   123,    26,    45,
      39,    49,    51,   201,   202,   109,   172,    51,    31,    53,
     104,    39,    24,   343,   114,    53,   134,    40,    31,   123,
      32,    50,   172,   221,    23,   143,    49,   131,   172,    10,
     228,   172,    13,    45,    47,   201,   202,    45,   172,   301,
     158,   303,   305,    21,   225,   226,   172,   113,   114,    21,
     154,   201,   202,   157,   480,   221,   160,   201,   202,    48,
     201,   202,   293,   167,   197,    22,   170,   201,   202,   113,
     114,   221,    31,    21,   140,   201,   202,   221,   505,    29,
     221,    31,   200,    50,   414,   294,   535,   221,   537,   321,
     321,    48,    40,    50,    52,   221,   140,   201,   202,    50,
     218,   234,    50,   301,    22,   303,    52,   305,    52,   371,
     372,   229,   293,   322,   208,   219,   220,   221,    32,     3,
     504,   330,   166,    32,   228,    47,   192,    11,    36,    37,
      48,   175,    50,    41,    45,   301,    42,   303,    21,   305,
     321,   245,    21,   527,   528,    29,    30,    52,   192,    24,
     254,   301,   256,   303,     3,   305,    52,   301,    52,   303,
     301,   305,   303,    47,   305,    50,    48,   301,    49,   303,
      45,   305,   266,   371,   372,   301,    51,   303,    47,   305,
      29,    30,     3,    24,    22,    50,    30,    31,    60,    61,
      50,    32,    48,    31,     4,     5,     6,   301,    50,   303,
      21,   305,   320,   533,    45,   371,   372,    52,    29,    30,
      51,    32,    50,    23,    52,    13,    48,    15,    16,    40,
     429,   371,   372,    60,    61,    49,    50,   371,   372,    50,
     371,   372,    24,    46,   367,    49,    50,   371,   372,   343,
      32,    50,    46,   123,    50,   371,   372,    91,    31,    93,
      94,    22,   356,    45,   358,   359,   100,   101,   102,    51,
     515,   516,    52,   518,   382,    49,    50,   371,   372,    49,
      50,    49,    50,   145,   535,   119,   537,    49,    50,   151,
      24,    31,    48,   538,   128,   459,   460,   542,    31,    50,
      52,   424,   172,    48,    48,   407,   408,    32,     4,    52,
     144,    45,    42,    47,    34,   149,   150,    51,   145,    45,
     414,    52,    31,   417,   151,   159,   420,   435,   436,    52,
     424,   201,   202,    52,   442,   197,   531,   501,   502,    52,
      45,    49,   176,    22,   178,   453,    52,   535,   182,   537,
      48,   221,   186,   187,   188,   457,   458,    32,    32,   193,
      48,     2,    48,     4,   559,   227,    32,    52,    52,    52,
     197,    52,   234,   467,    88,    45,    45,   239,   472,   535,
      45,   537,   490,   491,   492,    99,   480,    36,   222,    40,
      46,   225,   226,    34,    46,   535,    50,   537,    32,   233,
     227,   535,   116,   537,   535,    52,   537,   234,    52,    32,
     244,   535,   239,   537,    32,    48,   524,    52,    40,   535,
      52,   537,    52,    32,    28,    32,    32,    32,   262,    49,
      46,   301,   294,   303,    53,   305,    46,     5,   546,   533,
     461,   239,   348,    46,     3,   279,    46,    46,   282,   283,
     549,   165,   149,   340,   316,    31,    30,    31,   292,   293,
     322,   511,   157,   170,    81,    24,    81,   294,   330,    28,
      29,    30,    80,    32,   411,   193,   507,   311,   372,   313,
     314,    -1,   123,    -1,    -1,    -1,    45,   321,    -1,   316,
      -1,   325,    51,   207,    -1,   322,    -1,   331,    -1,    -1,
      -1,   371,   372,   330,    -1,   367,    -1,    -1,    -1,    -1,
      -1,    -1,   153,    -1,    -1,    -1,    -1,    91,   316,    -1,
      -1,    -1,    -1,    -1,   358,   359,    -1,   361,   242,    -1,
     392,   365,    -1,    -1,    -1,    -1,    -1,   399,    -1,   401,
     367,    -1,   404,    -1,    -1,   236,    -1,    -1,    -1,   240,
     241,   385,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     201,   202,   424,   397,    -1,   392,    -1,   429,    -1,    -1,
      -1,    -1,   399,    -1,   401,   149,   150,   404,    -1,    -1,
     221,   443,    -1,   445,    -1,   299,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   427,   392,    -1,    -1,   424,    -1,    -1,
      -1,   399,   429,   401,   178,   319,   404,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   443,    -1,   445,   193,
      -1,    -1,   263,    -1,   486,    -1,   317,   318,   202,    -1,
      -1,    -1,    -1,    -1,    -1,   497,    -1,   499,    -1,    -1,
      -1,    -1,    -1,   477,   478,   443,   480,   445,   482,    -1,
      -1,    -1,   226,    -1,    -1,    -1,    -1,    -1,    -1,   486,
     301,    -1,   303,    -1,   305,   535,    -1,   537,    -1,    -1,
     497,    -1,   499,    -1,    -1,    -1,    -1,   368,   369,    -1,
     514,   395,    -1,    -1,   398,   519,    -1,    -1,   486,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   530,    -1,   532,   497,
      -1,   499,   393,   394,     4,     5,     6,     7,     8,     9,
     544,   545,    -1,    -1,    -1,    -1,    -1,    17,    18,    19,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,
     371,   372,    -1,    -1,    -1,    35,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    -1,   321,    -1,    -1,
      -1,    51,    49,    -1,    -1,    55,   470,   331,    -1,    -1,
     334,    -1,    -1,    -1,    -1,    -1,   340,    -1,    -1,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    13,
      14,    -1,    -1,    17,    18,    19,    20,    -1,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,   372,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    -1,    43,
      -1,    45,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,
      54,    55,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    -1,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    -1,    43,    -1,    45,    -1,    47,    -1,    -1,    -1,
      51,    -1,    -1,    54,    55,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    13,    14,    -1,    -1,    17,
      18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    -1,    -1,    -1,    -1,    35,    -1,    -1,
      -1,    -1,    40,    -1,    -1,    43,    -1,    45,    -1,    47,
      -1,    -1,    -1,    51,    -1,    53,    54,    55,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    -1,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    -1,    -1,    -1,    34,
      35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    43,    -1,
      45,    -1,    47,    -1,    -1,    -1,    51,    52,    -1,    54,
      55,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,
      -1,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    -1,
      -1,    -1,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,
      -1,    43,    -1,    45,    -1,    47,    -1,    -1,    -1,    51,
      52,    -1,    54,    55,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    -1,    -1,    -1,    34,    35,    -1,    -1,    38,
      -1,    -1,    -1,    -1,    43,    -1,    45,    -1,    47,    -1,
      -1,    -1,    51,    52,    -1,    54,    55,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    13,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,    45,
      -1,    47,    -1,    -1,    -1,    51,    -1,    53,    54,    55,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    -1,    -1,
      -1,    -1,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      43,    -1,    45,    -1,    47,    -1,    -1,    -1,    51,    52,
      -1,    54,    55,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    -1,    -1,    -1,    -1,    35,    -1,    -1,    38,    -1,
      -1,    -1,    -1,    43,    -1,    45,    -1,    47,    -1,    -1,
      -1,    51,    52,    -1,    54,    55,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,
      17,    18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,
      -1,    -1,    29,    30,    -1,    -1,    -1,    34,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    43,    -1,    45,    -1,
      47,    -1,    -1,    -1,    51,    -1,    -1,    54,    55,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    13,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    -1,    -1,
      -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      -1,    45,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,
      54,    55,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,    30,
      -1,    -1,    -1,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    43,    -1,    45,    -1,    47,    -1,    -1,    -1,
      51,    -1,    -1,    54,    55,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,
      18,    19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,
      -1,    29,    30,    -1,    -1,    -1,    -1,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    -1,    45,    -1,    47,
      -1,    -1,    -1,    51,    -1,    -1,    54,    55,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    -1,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,
      45,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,    54,
      55,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    30,    -1,
      -1,    -1,    -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    -1,    45,    -1,    47,    -1,    -1,    -1,    51,
      -1,    -1,    54,    55,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,
      29,    30,    -1,    -1,    -1,    -1,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    -1,    45,    -1,    47,    48,
      -1,    -1,    51,    -1,    -1,    54,    55,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,    45,
      -1,    47,    48,    -1,    -1,    51,    -1,    -1,    54,    55,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,
      23,    24,    -1,    -1,    -1,    -1,    29,    30,    -1,    -1,
      -1,    -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    -1,    45,    -1,    47,    -1,    -1,    -1,    51,    -1,
      53,    54,    55,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,    29,
      30,    -1,    -1,    -1,    -1,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    -1,    45,    -1,    47,    -1,    -1,
      -1,    51,    -1,    -1,    54,    55,     4,     5,     6,     7,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,
      18,    19,    -1,    -1,    -1,    23,    -1,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    -1,    35,    -1,    -1,
      -1,    17,    18,    19,    -1,    -1,    -1,    23,    -1,    -1,
      -1,    -1,    -1,    29,    30,    -1,    -1,    55,    34,    35,
      -1,    -1,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    47,    -1,    -1,    -1,    51,    17,    18,    19,    -1,
      -1,     3,    23,    -1,    -1,    -1,    -1,    -1,    29,    30,
      -1,    -1,    -1,    -1,    35,     3,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    47,    29,    30,     3,
      51,    33,    -1,    35,    36,    37,    38,    39,    40,    41,
      -1,    29,    30,     3,    46,    33,    -1,    35,    36,    37,
      38,    39,    40,    41,    -1,    29,    30,    -1,    46,    33,
      -1,    35,    36,    37,    38,    39,    40,    41,    -1,    29,
      30,    -1,    46,    33,    -1,    35,    36,    37,    38,    39,
      40,    41
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    26,    39,    57,    58,    59,    61,     3,     4,     5,
       6,     7,     8,     9,    11,    12,    13,    14,    17,    18,
      19,    20,    23,    24,    29,    30,    35,    40,    43,    45,
      47,    51,    53,    54,    55,    71,    72,    76,    80,    81,
      86,    87,    88,    89,    92,    94,    95,   106,   107,   108,
     110,   112,   113,   115,   133,   139,   140,   141,   142,   143,
     144,   145,   147,   148,   149,   150,   151,   152,   153,   157,
     145,     0,    60,   145,    61,   122,   123,   144,   121,   122,
      14,    40,   114,     4,     5,     6,   142,   118,   119,   120,
     144,    51,    92,    31,    49,   114,    34,    95,    96,    97,
     155,    22,    31,    33,    35,    36,    37,    38,    39,    40,
      41,    90,    95,   112,   115,   124,   125,   126,   154,   155,
     156,    92,   144,    47,   103,   104,   144,   145,    47,    35,
      40,    42,   155,   156,    49,    68,    76,    71,   113,    71,
     112,   142,   142,    32,    51,    77,    78,    81,   106,    47,
      51,    77,    45,    31,    47,    63,    49,    50,    32,    51,
      21,    49,   121,   122,   118,   119,   111,    21,    40,    50,
      69,    32,    70,    95,    95,   111,    31,    48,    50,    69,
      95,    95,    22,    91,    95,    52,    22,    31,    50,    52,
      71,    71,   112,    50,    69,    52,    95,    52,    49,    68,
      32,    47,    51,   109,   129,   134,   135,   136,   139,   143,
     144,   145,   147,   148,   149,   150,   151,   152,    32,    21,
      50,    47,    22,    95,    98,    47,    51,   144,    42,    93,
      92,    71,    92,    38,    52,    98,    99,    78,    96,    52,
      98,    99,   100,   101,   155,    66,   145,    62,   144,    45,
     123,    92,    95,   144,    21,    49,    21,    40,    71,   113,
     144,   120,    51,    55,   105,   133,   139,   143,   147,   148,
     149,   150,   151,   152,    82,    83,    71,    95,    95,   155,
      95,    52,    22,    50,    95,    95,    95,    52,    52,    71,
      95,   124,   155,    51,    77,    92,   135,   109,   130,   131,
     132,   155,    48,    50,    69,    47,    92,   144,   144,   135,
      95,    22,    48,    50,    22,    98,    52,    98,    99,   100,
      32,    51,    77,    79,    35,    51,   139,   144,    92,    95,
      77,    50,    69,   102,    50,   102,    48,    79,   102,   102,
      50,    69,    95,    38,    46,    67,   144,   154,    48,    64,
      52,    49,   144,   144,    46,    40,    50,    95,    50,    50,
      46,    31,    95,    95,    95,    22,    52,    52,    98,    99,
      48,    31,    50,    69,    52,   109,   109,   135,    48,    48,
      48,    95,    32,    95,    95,    22,    48,    79,   102,   102,
      69,    92,    52,    98,    99,   100,    95,    38,   100,    52,
      52,    52,    52,   101,    52,   144,   154,    45,    45,    63,
      13,    15,    16,    65,    66,    49,    40,    50,   144,    52,
      13,    31,    85,    95,   144,    85,    95,    22,    95,    77,
     102,   102,   109,   130,    48,    32,    32,    48,    92,    48,
      48,    95,    32,    52,    52,    52,    79,   102,   102,    69,
      52,    95,    69,    32,    79,    79,    79,    45,    45,   114,
     114,    45,    36,    37,    41,   127,   128,   137,   127,    36,
     116,   117,   138,    46,   144,    40,   144,    29,    31,    46,
      50,    84,    28,   144,    46,    95,    52,    52,    92,    92,
      32,    32,    32,    48,    92,    79,    79,    52,    52,    52,
      92,   114,   114,   111,   111,    64,    49,    50,   144,    49,
      49,    69,   144,    40,    32,    95,    95,    85,    95,    28,
      79,    92,    92,    92,    32,    79,    79,   111,   111,    10,
      74,   146,    74,    66,   128,    32,   117,    32,    95,    84,
      84,    84,    95,    92,    74,    74,    53,    73,    75,    95,
      68,    73,    46,   105,   105,    84,    84,    73,    73,    92,
      46,    75,    46,    46,    46,    68
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    56,    57,    57,    57,    58,    58,    59,    59,    60,
      60,    61,    62,    62,    63,    63,    64,    64,    65,    65,
      65,    66,    66,    67,    67,    67,    67,    68,    68,    69,
      69,    70,    70,    71,    71,    72,    72,    72,    72,    72,
      72,    72,    73,    73,    74,    74,    75,    75,    76,    76,
      77,    77,    78,    78,    79,    79,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    82,
      81,    83,    81,    84,    84,    85,    85,    85,    85,    85,
      85,    86,    86,    86,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    86,    86,    86,    87,    87,    87,    88,
      88,    88,    89,    89,    89,    89,    89,    89,    90,    90,
      90,    90,    90,    91,    91,    91,    91,    91,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    93,    93,    93,    93,    94,    94,    95,    96,
      96,    97,    97,    97,    97,    97,    97,    98,    98,    99,
      99,   100,   100,   101,   102,   102,   103,   103,   104,   104,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   106,
     107,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   110,   110,   111,   111,   112,   112,   113,   114,   114,
     114,   114,   114,   114,   115,   115,   115,   115,   115,   116,
     116,   117,   118,   118,   119,   119,   120,   120,   120,   121,
     121,   122,   122,   123,   123,   123,   124,   124,   125,   125,
     126,   126,   127,   127,   128,   128,   129,   130,   130,   131,
     131,   132,   132,   133,   133,   134,   134,   135,   135,   136,
     136,   137,   137,   137,   137,   138,   138,   139,   139,   140,
     140,   141,   141,   142,   143,   143,   143,   143,   143,   143,
     144,   144,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   154,   154,   154,   154,   154,   154,   154,
     155,   156,   156,   157
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
       1,     5,     5,     0,     2,     1,     2,     3,     0,     3,
       5,     3,     5,     7,     3,     5,     3,     5,     7,     1,
       3,     4,     0,     1,     1,     3,     1,     3,     5,     0,
       1,     1,     3,     1,     3,     4,     3,     2,     1,     3,
       0,     2,     1,     3,     2,     4,     3,     3,     2,     1,
       3,     0,     2,     4,     5,     3,     4,     0,     2,     1,
       3,     0,     1,     1,     1,     0,     1,     1,     2,     1,
       2,     1,     2,     1,     1,     1,     2,     2,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
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
#line 2201 "lang11d_tab.cpp"
    break;

  case 3: /* root: classextensions  */
#line 42 "lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; }
#line 2207 "lang11d_tab.cpp"
    break;

  case 4: /* root: INTERPRET cmdlinecode  */
#line 44 "lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 2; }
#line 2213 "lang11d_tab.cpp"
    break;

  case 5: /* classes: %empty  */
#line 47 "lang11d"
          { yyval = 0; }
#line 2219 "lang11d_tab.cpp"
    break;

  case 6: /* classes: classes classdef  */
#line 49 "lang11d"
                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2225 "lang11d_tab.cpp"
    break;

  case 8: /* classextensions: classextensions classextension  */
#line 54 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2231 "lang11d_tab.cpp"
    break;

  case 9: /* classdef: classname superclass '{' classvardecls methods '}'  */
#line 58 "lang11d"
                                { yyval = (intptr_t)newPyrClassNode((PyrSlotNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1], 0);
				}
#line 2239 "lang11d_tab.cpp"
    break;

  case 10: /* classdef: classname '[' optname ']' superclass '{' classvardecls methods '}'  */
#line 62 "lang11d"
                                { yyval = (intptr_t)newPyrClassNode((PyrSlotNode*)yyvsp[-8], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1],
					(PyrSlotNode*)yyvsp[-6]);
				}
#line 2248 "lang11d_tab.cpp"
    break;

  case 11: /* classextension: '+' classname '{' methods '}'  */
#line 69 "lang11d"
                                {
					yyval = (intptr_t)newPyrClassExtNode((PyrSlotNode*)yyvsp[-3], (PyrMethodNode*)yyvsp[-1]);
				}
#line 2256 "lang11d_tab.cpp"
    break;

  case 12: /* optname: %empty  */
#line 74 "lang11d"
                  { yyval = 0; }
#line 2262 "lang11d_tab.cpp"
    break;

  case 14: /* superclass: %empty  */
#line 78 "lang11d"
                  { yyval = 0; }
#line 2268 "lang11d_tab.cpp"
    break;

  case 15: /* superclass: ':' classname  */
#line 80 "lang11d"
                                { yyval = yyvsp[0]; }
#line 2274 "lang11d_tab.cpp"
    break;

  case 16: /* classvardecls: %empty  */
#line 83 "lang11d"
                  { yyval = 0; }
#line 2280 "lang11d_tab.cpp"
    break;

  case 17: /* classvardecls: classvardecls classvardecl  */
#line 85 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2286 "lang11d_tab.cpp"
    break;

  case 18: /* classvardecl: CLASSVAR rwslotdeflist ';'  */
#line 89 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varClass); }
#line 2292 "lang11d_tab.cpp"
    break;

  case 19: /* classvardecl: VAR rwslotdeflist ';'  */
#line 91 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varInst); }
#line 2298 "lang11d_tab.cpp"
    break;

  case 20: /* classvardecl: SC_CONST constdeflist ';'  */
#line 93 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varConst); }
#line 2304 "lang11d_tab.cpp"
    break;

  case 21: /* methods: %empty  */
#line 96 "lang11d"
                  { yyval = 0; }
#line 2310 "lang11d_tab.cpp"
    break;

  case 22: /* methods: methods methoddef  */
#line 98 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2316 "lang11d_tab.cpp"
    break;

  case 23: /* methoddef: name '{' argdecls funcvardecls primitive methbody '}'  */
#line 102 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2323 "lang11d_tab.cpp"
    break;

  case 24: /* methoddef: '*' name '{' argdecls funcvardecls primitive methbody '}'  */
#line 105 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2330 "lang11d_tab.cpp"
    break;

  case 25: /* methoddef: binop '{' argdecls funcvardecls primitive methbody '}'  */
#line 108 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2337 "lang11d_tab.cpp"
    break;

  case 26: /* methoddef: '*' binop '{' argdecls funcvardecls primitive methbody '}'  */
#line 111 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2344 "lang11d_tab.cpp"
    break;

  case 34: /* funcbody: exprseq funretval  */
#line 129 "lang11d"
                                { yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2350 "lang11d_tab.cpp"
    break;

  case 35: /* cmdlinecode: '(' argdecls1 funcvardecls1 funcbody ')'  */
#line 133 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2356 "lang11d_tab.cpp"
    break;

  case 36: /* cmdlinecode: '(' argdecls1 funcbody ')'  */
#line 135 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-2], NULL, (PyrParseNode*)yyvsp[-1], false); }
#line 2362 "lang11d_tab.cpp"
    break;

  case 37: /* cmdlinecode: '(' funcvardecls1 funcbody ')'  */
#line 137 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2368 "lang11d_tab.cpp"
    break;

  case 38: /* cmdlinecode: argdecls1 funcvardecls1 funcbody  */
#line 139 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-2], (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2374 "lang11d_tab.cpp"
    break;

  case 39: /* cmdlinecode: argdecls1 funcbody  */
#line 141 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-1], NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2380 "lang11d_tab.cpp"
    break;

  case 40: /* cmdlinecode: funcvardecls1 funcbody  */
#line 143 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2386 "lang11d_tab.cpp"
    break;

  case 41: /* cmdlinecode: funcbody  */
#line 145 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2392 "lang11d_tab.cpp"
    break;

  case 43: /* methbody: exprseq retval  */
#line 150 "lang11d"
                                { yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2398 "lang11d_tab.cpp"
    break;

  case 44: /* primitive: %empty  */
#line 153 "lang11d"
                  { yyval = 0; }
#line 2404 "lang11d_tab.cpp"
    break;

  case 45: /* primitive: primname optsemi  */
#line 155 "lang11d"
                                { yyval = yyvsp[-1]; }
#line 2410 "lang11d_tab.cpp"
    break;

  case 46: /* retval: %empty  */
#line 159 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode(NULL); }
#line 2416 "lang11d_tab.cpp"
    break;

  case 47: /* retval: '^' expr optsemi  */
#line 161 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); }
#line 2422 "lang11d_tab.cpp"
    break;

  case 48: /* funretval: %empty  */
#line 165 "lang11d"
                        { yyval = (intptr_t)newPyrBlockReturnNode(); }
#line 2428 "lang11d_tab.cpp"
    break;

  case 49: /* funretval: '^' expr optsemi  */
#line 167 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); }
#line 2434 "lang11d_tab.cpp"
    break;

  case 51: /* blocklist1: blocklist1 blocklistitem  */
#line 172 "lang11d"
                                {
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
				}
#line 2442 "lang11d_tab.cpp"
    break;

  case 54: /* blocklist: %empty  */
#line 181 "lang11d"
                        { yyval = 0; }
#line 2448 "lang11d_tab.cpp"
    break;

  case 56: /* msgsend: name blocklist1  */
#line 186 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2456 "lang11d_tab.cpp"
    break;

  case 57: /* msgsend: '(' binop2 ')' blocklist1  */
#line 190 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2464 "lang11d_tab.cpp"
    break;

  case 58: /* msgsend: name '(' ')' blocklist1  */
#line 194 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-3], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2472 "lang11d_tab.cpp"
    break;

  case 59: /* msgsend: name '(' arglist1 optkeyarglist ')' blocklist  */
#line 198 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2481 "lang11d_tab.cpp"
    break;

  case 60: /* msgsend: '(' binop2 ')' '(' ')' blocklist1  */
#line 203 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-4], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2489 "lang11d_tab.cpp"
    break;

  case 61: /* msgsend: '(' binop2 ')' '(' arglist1 optkeyarglist ')' blocklist  */
#line 207 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2498 "lang11d_tab.cpp"
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
#line 2520 "lang11d_tab.cpp"
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
#line 2537 "lang11d_tab.cpp"
    break;

  case 64: /* msgsend: classname '[' arrayelems ']'  */
#line 243 "lang11d"
                        { yyval = (intptr_t)newPyrDynListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 2543 "lang11d_tab.cpp"
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
#line 2558 "lang11d_tab.cpp"
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
#line 2573 "lang11d_tab.cpp"
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
#line 2588 "lang11d_tab.cpp"
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
#line 2605 "lang11d_tab.cpp"
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
#line 2630 "lang11d_tab.cpp"
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
#line 2643 "lang11d_tab.cpp"
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
#line 2656 "lang11d_tab.cpp"
    break;

  case 72: /* msgsend: expr '.' name '(' keyarglist1 optcomma ')' blocklist  */
#line 330 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-7],
					(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2665 "lang11d_tab.cpp"
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
#line 2682 "lang11d_tab.cpp"
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
#line 2706 "lang11d_tab.cpp"
    break;

  case 75: /* msgsend: expr '.' name '(' ')' blocklist  */
#line 371 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-5], NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2714 "lang11d_tab.cpp"
    break;

  case 76: /* msgsend: expr '.' name '(' arglist1 optkeyarglist ')' blocklist  */
#line 375 "lang11d"
                        {
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2724 "lang11d_tab.cpp"
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
#line 2746 "lang11d_tab.cpp"
    break;

  case 78: /* msgsend: expr '.' name blocklist  */
#line 399 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[-3], 0, (PyrParseNode*)yyvsp[0]);
			}
#line 2754 "lang11d_tab.cpp"
    break;

  case 79: /* $@1: %empty  */
#line 404 "lang11d"
                            { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 1); }
#line 2760 "lang11d_tab.cpp"
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
#line 2774 "lang11d_tab.cpp"
    break;

  case 81: /* $@2: %empty  */
#line 414 "lang11d"
                                  { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 2); }
#line 2780 "lang11d_tab.cpp"
    break;

  case 82: /* generator: '{' ';' exprseq $@2 ',' qual '}'  */
#line 415 "lang11d"
                        {
				yyval = yyvsp[-1];
			}
#line 2788 "lang11d_tab.cpp"
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
#line 2814 "lang11d_tab.cpp"
    break;

  case 84: /* nextqual: ',' qual  */
#line 443 "lang11d"
                                { yyval = yyvsp[0]; }
#line 2820 "lang11d_tab.cpp"
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
#line 2858 "lang11d_tab.cpp"
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
#line 2900 "lang11d_tab.cpp"
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
#line 2918 "lang11d_tab.cpp"
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
#line 2933 "lang11d_tab.cpp"
    break;

  case 89: /* qual: ':' ':' exprseq nextqual  */
#line 544 "lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
			}
#line 2941 "lang11d_tab.cpp"
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
#line 2968 "lang11d_tab.cpp"
    break;

  case 97: /* expr1: '(' exprseq ')'  */
#line 579 "lang11d"
                        {
				PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
				node->mParens = 1;
				yyval = yyvsp[-1];
			}
#line 2978 "lang11d_tab.cpp"
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
#line 2992 "lang11d_tab.cpp"
    break;

  case 99: /* expr1: '[' arrayelems ']'  */
#line 595 "lang11d"
                        { yyval = (intptr_t)newPyrDynListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 2998 "lang11d_tab.cpp"
    break;

  case 100: /* expr1: '(' valrange2 ')'  */
#line 597 "lang11d"
                        { yyval = yyvsp[-1]; }
#line 3004 "lang11d_tab.cpp"
    break;

  case 101: /* expr1: '(' ':' valrange3 ')'  */
#line 599 "lang11d"
                        { yyval = yyvsp[-1]; }
#line 3010 "lang11d_tab.cpp"
    break;

  case 102: /* expr1: '(' dictslotlist ')'  */
#line 601 "lang11d"
                        { yyval = (intptr_t)newPyrDynDictNode((PyrParseNode*)yyvsp[-1]); }
#line 3016 "lang11d_tab.cpp"
    break;

  case 103: /* expr1: pseudovar  */
#line 603 "lang11d"
                        { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3022 "lang11d_tab.cpp"
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
#line 3039 "lang11d_tab.cpp"
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
#line 3070 "lang11d_tab.cpp"
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
#line 3092 "lang11d_tab.cpp"
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
#line 3121 "lang11d_tab.cpp"
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
#line 3153 "lang11d_tab.cpp"
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
#line 3176 "lang11d_tab.cpp"
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
#line 3206 "lang11d_tab.cpp"
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
#line 3237 "lang11d_tab.cpp"
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
#line 3259 "lang11d_tab.cpp"
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
#line 3288 "lang11d_tab.cpp"
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
#line 3320 "lang11d_tab.cpp"
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
#line 3343 "lang11d_tab.cpp"
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
#line 3373 "lang11d_tab.cpp"
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
#line 3395 "lang11d_tab.cpp"
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
#line 3417 "lang11d_tab.cpp"
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
#line 3437 "lang11d_tab.cpp"
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
#line 3455 "lang11d_tab.cpp"
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
#line 3478 "lang11d_tab.cpp"
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
#line 3500 "lang11d_tab.cpp"
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
#line 3521 "lang11d_tab.cpp"
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
#line 3541 "lang11d_tab.cpp"
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
#line 3561 "lang11d_tab.cpp"
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
#line 3579 "lang11d_tab.cpp"
    break;

  case 131: /* expr: classname  */
#line 1091 "lang11d"
                            { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3585 "lang11d_tab.cpp"
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
#line 3602 "lang11d_tab.cpp"
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
#line 3620 "lang11d_tab.cpp"
    break;

  case 134: /* expr: expr binop2 adverb expr  */
#line 1120 "lang11d"
                        {
				yyval = (intptr_t)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0], (PyrParseNode*)yyvsp[-1]);
			}
#line 3629 "lang11d_tab.cpp"
    break;

  case 135: /* expr: name '=' expr  */
#line 1125 "lang11d"
                        {
				yyval = (intptr_t)newPyrAssignNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0);
			}
#line 3637 "lang11d_tab.cpp"
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
#line 3652 "lang11d_tab.cpp"
    break;

  case 137: /* expr: expr '.' name '=' expr  */
#line 1140 "lang11d"
                        {
				yyval = (intptr_t)newPyrSetterNode((PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3661 "lang11d_tab.cpp"
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
#line 3675 "lang11d_tab.cpp"
    break;

  case 139: /* expr: '#' mavars '=' expr  */
#line 1155 "lang11d"
                        {
				yyval = (intptr_t)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[0], 0);
			}
#line 3684 "lang11d_tab.cpp"
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
#line 3702 "lang11d_tab.cpp"
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
#line 3720 "lang11d_tab.cpp"
    break;

  case 142: /* adverb: %empty  */
#line 1189 "lang11d"
          { yyval = 0; }
#line 3726 "lang11d_tab.cpp"
    break;

  case 143: /* adverb: '.' name  */
#line 1190 "lang11d"
                           { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3732 "lang11d_tab.cpp"
    break;

  case 144: /* adverb: '.' integer  */
#line 1191 "lang11d"
                              { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3738 "lang11d_tab.cpp"
    break;

  case 145: /* adverb: '.' '(' exprseq ')'  */
#line 1192 "lang11d"
                                      { yyval = yyvsp[-1]; }
#line 3744 "lang11d_tab.cpp"
    break;

  case 147: /* exprn: exprn ';' expr  */
#line 1197 "lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 3752 "lang11d_tab.cpp"
    break;

  case 149: /* arrayelems: %empty  */
#line 1205 "lang11d"
                  { yyval = 0; }
#line 3758 "lang11d_tab.cpp"
    break;

  case 150: /* arrayelems: arrayelems1 optcomma  */
#line 1207 "lang11d"
                          { yyval = yyvsp[-1]; }
#line 3764 "lang11d_tab.cpp"
    break;

  case 152: /* arrayelems1: exprseq ':' exprseq  */
#line 1212 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3770 "lang11d_tab.cpp"
    break;

  case 153: /* arrayelems1: keybinop exprseq  */
#line 1214 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 3779 "lang11d_tab.cpp"
    break;

  case 154: /* arrayelems1: arrayelems1 ',' exprseq  */
#line 1219 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3785 "lang11d_tab.cpp"
    break;

  case 155: /* arrayelems1: arrayelems1 ',' keybinop exprseq  */
#line 1221 "lang11d"
                                {
					PyrParseNode* elems;
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					elems = (PyrParseNode*)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], elems);
				}
#line 3796 "lang11d_tab.cpp"
    break;

  case 156: /* arrayelems1: arrayelems1 ',' exprseq ':' exprseq  */
#line 1228 "lang11d"
                                {
					PyrParseNode* elems;
					elems = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-4], elems);
				}
#line 3806 "lang11d_tab.cpp"
    break;

  case 158: /* arglist1: arglist1 ',' exprseq  */
#line 1237 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3812 "lang11d_tab.cpp"
    break;

  case 159: /* arglistv1: '*' exprseq  */
#line 1241 "lang11d"
                                { yyval = yyvsp[0]; }
#line 3818 "lang11d_tab.cpp"
    break;

  case 160: /* arglistv1: arglist1 ',' '*' exprseq  */
#line 1243 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]); }
#line 3824 "lang11d_tab.cpp"
    break;

  case 162: /* keyarglist1: keyarglist1 ',' keyarg  */
#line 1248 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3830 "lang11d_tab.cpp"
    break;

  case 163: /* keyarg: keybinop exprseq  */
#line 1252 "lang11d"
                                { yyval = (intptr_t)newPyrPushKeyArgNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 3836 "lang11d_tab.cpp"
    break;

  case 164: /* optkeyarglist: optcomma  */
#line 1255 "lang11d"
                           { yyval = 0; }
#line 3842 "lang11d_tab.cpp"
    break;

  case 165: /* optkeyarglist: ',' keyarglist1 optcomma  */
#line 1256 "lang11d"
                                                           { yyval = yyvsp[-1]; }
#line 3848 "lang11d_tab.cpp"
    break;

  case 166: /* mavars: mavarlist  */
#line 1260 "lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3854 "lang11d_tab.cpp"
    break;

  case 167: /* mavars: mavarlist ELLIPSIS name  */
#line 1262 "lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); }
#line 3860 "lang11d_tab.cpp"
    break;

  case 169: /* mavarlist: mavarlist ',' name  */
#line 1267 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3866 "lang11d_tab.cpp"
    break;

  case 170: /* slotliteral: integer  */
#line 1271 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3872 "lang11d_tab.cpp"
    break;

  case 171: /* slotliteral: floatp  */
#line 1272 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3878 "lang11d_tab.cpp"
    break;

  case 172: /* slotliteral: ascii  */
#line 1273 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3884 "lang11d_tab.cpp"
    break;

  case 173: /* slotliteral: string  */
#line 1274 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3890 "lang11d_tab.cpp"
    break;

  case 174: /* slotliteral: symbol  */
#line 1275 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3896 "lang11d_tab.cpp"
    break;

  case 175: /* slotliteral: trueobj  */
#line 1276 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3902 "lang11d_tab.cpp"
    break;

  case 176: /* slotliteral: falseobj  */
#line 1277 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3908 "lang11d_tab.cpp"
    break;

  case 177: /* slotliteral: nilobj  */
#line 1278 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3914 "lang11d_tab.cpp"
    break;

  case 178: /* slotliteral: listlit  */
#line 1279 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3920 "lang11d_tab.cpp"
    break;

  case 179: /* blockliteral: block  */
#line 1282 "lang11d"
                        { yyval = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3926 "lang11d_tab.cpp"
    break;

  case 180: /* pushname: name  */
#line 1285 "lang11d"
                                { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3932 "lang11d_tab.cpp"
    break;

  case 181: /* pushliteral: integer  */
#line 1288 "lang11d"
                                { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3938 "lang11d_tab.cpp"
    break;

  case 182: /* pushliteral: floatp  */
#line 1289 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3944 "lang11d_tab.cpp"
    break;

  case 183: /* pushliteral: ascii  */
#line 1290 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3950 "lang11d_tab.cpp"
    break;

  case 184: /* pushliteral: string  */
#line 1291 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3956 "lang11d_tab.cpp"
    break;

  case 185: /* pushliteral: symbol  */
#line 1292 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3962 "lang11d_tab.cpp"
    break;

  case 186: /* pushliteral: trueobj  */
#line 1293 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3968 "lang11d_tab.cpp"
    break;

  case 187: /* pushliteral: falseobj  */
#line 1294 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3974 "lang11d_tab.cpp"
    break;

  case 188: /* pushliteral: nilobj  */
#line 1295 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3980 "lang11d_tab.cpp"
    break;

  case 189: /* pushliteral: listlit  */
#line 1296 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 3986 "lang11d_tab.cpp"
    break;

  case 190: /* listliteral: integer  */
#line 1299 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3992 "lang11d_tab.cpp"
    break;

  case 191: /* listliteral: floatp  */
#line 1300 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3998 "lang11d_tab.cpp"
    break;

  case 192: /* listliteral: ascii  */
#line 1301 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4004 "lang11d_tab.cpp"
    break;

  case 193: /* listliteral: string  */
#line 1302 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4010 "lang11d_tab.cpp"
    break;

  case 194: /* listliteral: symbol  */
#line 1303 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4016 "lang11d_tab.cpp"
    break;

  case 195: /* listliteral: name  */
#line 1304 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4022 "lang11d_tab.cpp"
    break;

  case 196: /* listliteral: trueobj  */
#line 1305 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4028 "lang11d_tab.cpp"
    break;

  case 197: /* listliteral: falseobj  */
#line 1306 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4034 "lang11d_tab.cpp"
    break;

  case 198: /* listliteral: nilobj  */
#line 1307 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4040 "lang11d_tab.cpp"
    break;

  case 199: /* listliteral: listlit2  */
#line 1308 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 4046 "lang11d_tab.cpp"
    break;

  case 200: /* listliteral: dictlit2  */
#line 1309 "lang11d"
                                    { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 4052 "lang11d_tab.cpp"
    break;

  case 201: /* block: '{' argdecls funcvardecls funcbody '}'  */
#line 1313 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], false); }
#line 4059 "lang11d_tab.cpp"
    break;

  case 202: /* block: BEGINCLOSEDFUNC argdecls funcvardecls funcbody '}'  */
#line 1316 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], true); }
#line 4066 "lang11d_tab.cpp"
    break;

  case 203: /* funcvardecls: %empty  */
#line 1320 "lang11d"
                  { yyval = 0; }
#line 4072 "lang11d_tab.cpp"
    break;

  case 204: /* funcvardecls: funcvardecls funcvardecl  */
#line 1322 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 4078 "lang11d_tab.cpp"
    break;

  case 206: /* funcvardecls1: funcvardecls1 funcvardecl  */
#line 1327 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 4084 "lang11d_tab.cpp"
    break;

  case 207: /* funcvardecl: VAR vardeflist ';'  */
#line 1331 "lang11d"
                                { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varLocal); }
#line 4090 "lang11d_tab.cpp"
    break;

  case 208: /* argdecls: %empty  */
#line 1334 "lang11d"
                  { yyval = 0; }
#line 4096 "lang11d_tab.cpp"
    break;

  case 209: /* argdecls: ARG vardeflist ';'  */
#line 1336 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4104 "lang11d_tab.cpp"
    break;

  case 210: /* argdecls: ARG vardeflist0 ELLIPSIS name ';'  */
#line 1340 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4112 "lang11d_tab.cpp"
    break;

  case 211: /* argdecls: '|' slotdeflist '|'  */
#line 1344 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4120 "lang11d_tab.cpp"
    break;

  case 212: /* argdecls: '|' slotdeflist0 ELLIPSIS name '|'  */
#line 1348 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4128 "lang11d_tab.cpp"
    break;

  case 213: /* argdecls: '|' slotdeflist0 ELLIPSIS name ',' name '|'  */
#line 1352 "lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 4136 "lang11d_tab.cpp"
    break;

  case 214: /* argdecls1: ARG vardeflist ';'  */
#line 1358 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4144 "lang11d_tab.cpp"
    break;

  case 215: /* argdecls1: ARG vardeflist0 ELLIPSIS name ';'  */
#line 1362 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4152 "lang11d_tab.cpp"
    break;

  case 216: /* argdecls1: '|' slotdeflist '|'  */
#line 1366 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4160 "lang11d_tab.cpp"
    break;

  case 217: /* argdecls1: '|' slotdeflist0 ELLIPSIS name '|'  */
#line 1370 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4168 "lang11d_tab.cpp"
    break;

  case 218: /* argdecls1: '|' slotdeflist0 ELLIPSIS name ',' name '|'  */
#line 1374 "lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 4176 "lang11d_tab.cpp"
    break;

  case 220: /* constdeflist: constdeflist optcomma constdef  */
#line 1382 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4182 "lang11d_tab.cpp"
    break;

  case 221: /* constdef: rspec name '=' slotliteral  */
#line 1386 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4188 "lang11d_tab.cpp"
    break;

  case 222: /* slotdeflist0: %empty  */
#line 1389 "lang11d"
                  { yyval = 0; }
#line 4194 "lang11d_tab.cpp"
    break;

  case 225: /* slotdeflist: slotdeflist optcomma slotdef  */
#line 1395 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4200 "lang11d_tab.cpp"
    break;

  case 226: /* slotdef: name  */
#line 1399 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4206 "lang11d_tab.cpp"
    break;

  case 227: /* slotdef: name optequal slotliteral  */
#line 1401 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4212 "lang11d_tab.cpp"
    break;

  case 228: /* slotdef: name optequal '(' exprseq ')'  */
#line 1403 "lang11d"
                                {
					PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
					node->mParens = 1;
					yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-4], node, 0);
				}
#line 4222 "lang11d_tab.cpp"
    break;

  case 229: /* vardeflist0: %empty  */
#line 1410 "lang11d"
                  { yyval = 0; }
#line 4228 "lang11d_tab.cpp"
    break;

  case 232: /* vardeflist: vardeflist ',' vardef  */
#line 1416 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4234 "lang11d_tab.cpp"
    break;

  case 233: /* vardef: name  */
#line 1420 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4240 "lang11d_tab.cpp"
    break;

  case 234: /* vardef: name '=' expr  */
#line 1422 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4246 "lang11d_tab.cpp"
    break;

  case 235: /* vardef: name '(' exprseq ')'  */
#line 1424 "lang11d"
                                {
									PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
									node->mParens = 1;
									yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-3], node, 0);
								}
#line 4256 "lang11d_tab.cpp"
    break;

  case 236: /* dictslotdef: exprseq ':' exprseq  */
#line 1432 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4262 "lang11d_tab.cpp"
    break;

  case 237: /* dictslotdef: keybinop exprseq  */
#line 1434 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 4271 "lang11d_tab.cpp"
    break;

  case 239: /* dictslotlist1: dictslotlist1 ',' dictslotdef  */
#line 1442 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4277 "lang11d_tab.cpp"
    break;

  case 240: /* dictslotlist: %empty  */
#line 1445 "lang11d"
                  { yyval = 0; }
#line 4283 "lang11d_tab.cpp"
    break;

  case 243: /* rwslotdeflist: rwslotdeflist ',' rwslotdef  */
#line 1451 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4289 "lang11d_tab.cpp"
    break;

  case 244: /* rwslotdef: rwspec name  */
#line 1455 "lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); }
#line 4295 "lang11d_tab.cpp"
    break;

  case 245: /* rwslotdef: rwspec name '=' slotliteral  */
#line 1457 "lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4301 "lang11d_tab.cpp"
    break;

  case 246: /* dictlit2: '(' litdictslotlist ')'  */
#line 1461 "lang11d"
                                { yyval = (intptr_t)newPyrLitDictNode((PyrParseNode*)yyvsp[-1]); }
#line 4307 "lang11d_tab.cpp"
    break;

  case 247: /* litdictslotdef: listliteral ':' listliteral  */
#line 1465 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4313 "lang11d_tab.cpp"
    break;

  case 248: /* litdictslotdef: keybinop listliteral  */
#line 1467 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 4322 "lang11d_tab.cpp"
    break;

  case 250: /* litdictslotlist1: litdictslotlist1 ',' litdictslotdef  */
#line 1475 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4328 "lang11d_tab.cpp"
    break;

  case 251: /* litdictslotlist: %empty  */
#line 1478 "lang11d"
                  { yyval = 0; }
#line 4334 "lang11d_tab.cpp"
    break;

  case 253: /* listlit: '#' '[' literallistc ']'  */
#line 1485 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 4340 "lang11d_tab.cpp"
    break;

  case 254: /* listlit: '#' classname '[' literallistc ']'  */
#line 1487 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4346 "lang11d_tab.cpp"
    break;

  case 255: /* listlit2: '[' literallistc ']'  */
#line 1491 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 4352 "lang11d_tab.cpp"
    break;

  case 256: /* listlit2: classname '[' literallistc ']'  */
#line 1493 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4358 "lang11d_tab.cpp"
    break;

  case 257: /* literallistc: %empty  */
#line 1496 "lang11d"
                  { yyval = 0; }
#line 4364 "lang11d_tab.cpp"
    break;

  case 260: /* literallist1: literallist1 ',' listliteral  */
#line 1502 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4370 "lang11d_tab.cpp"
    break;

  case 261: /* rwspec: %empty  */
#line 1505 "lang11d"
           { yyval = rwPrivate; }
#line 4376 "lang11d_tab.cpp"
    break;

  case 262: /* rwspec: '<'  */
#line 1507 "lang11d"
                        { yyval = rwReadOnly; }
#line 4382 "lang11d_tab.cpp"
    break;

  case 263: /* rwspec: READWRITEVAR  */
#line 1509 "lang11d"
                        { yyval = rwReadWrite; }
#line 4388 "lang11d_tab.cpp"
    break;

  case 264: /* rwspec: '>'  */
#line 1511 "lang11d"
                        { yyval = rwWriteOnly; }
#line 4394 "lang11d_tab.cpp"
    break;

  case 265: /* rspec: %empty  */
#line 1514 "lang11d"
           { yyval = rwPrivate; }
#line 4400 "lang11d_tab.cpp"
    break;

  case 266: /* rspec: '<'  */
#line 1516 "lang11d"
                        { yyval = rwReadOnly; }
#line 4406 "lang11d_tab.cpp"
    break;

  case 267: /* integer: INTEGER  */
#line 1519 "lang11d"
                  { yyval = zzval; }
#line 4412 "lang11d_tab.cpp"
    break;

  case 268: /* integer: '-' INTEGER  */
#line 1521 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawInt(&node->mSlot));
				yyval = zzval;
			}
#line 4423 "lang11d_tab.cpp"
    break;

  case 269: /* floatr: SC_FLOAT  */
#line 1529 "lang11d"
                   { yyval = zzval; }
#line 4429 "lang11d_tab.cpp"
    break;

  case 270: /* floatr: '-' SC_FLOAT  */
#line 1531 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawFloat(&node->mSlot));
				yyval = zzval;
			}
#line 4440 "lang11d_tab.cpp"
    break;

  case 271: /* accidental: ACCIDENTAL  */
#line 1539 "lang11d"
                        { yyval = zzval; }
#line 4446 "lang11d_tab.cpp"
    break;

  case 272: /* accidental: '-' ACCIDENTAL  */
#line 1541 "lang11d"
                                {
					PyrSlotNode *node;
					double intval, fracval;
					node = (PyrSlotNode*)zzval;
					intval = floor(slotRawFloat(&node->mSlot) + 0.5);
					fracval = slotRawFloat(&node->mSlot) - intval;
					SetRaw(&node->mSlot, -intval + fracval);
					yyval = zzval;
				}
#line 4460 "lang11d_tab.cpp"
    break;

  case 273: /* pie: PIE  */
#line 1551 "lang11d"
                      { yyval = zzval; }
#line 4466 "lang11d_tab.cpp"
    break;

  case 276: /* floatp: floatr pie  */
#line 1557 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)yyvsp[-1];
				SetRaw(&node->mSlot, slotRawFloat(&node->mSlot) * pi);
			}
#line 4476 "lang11d_tab.cpp"
    break;

  case 277: /* floatp: integer pie  */
#line 1563 "lang11d"
                        {
				PyrSlotNode *node;
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = slotRawInt(&node->mSlot);
				SetFloat(&node->mSlot, ival * pi);
			}
#line 4488 "lang11d_tab.cpp"
    break;

  case 278: /* floatp: pie  */
#line 1571 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, pi);
				yyval = zzval;
			}
#line 4499 "lang11d_tab.cpp"
    break;

  case 279: /* floatp: '-' pie  */
#line 1578 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, -pi);
				yyval = zzval;
			}
#line 4510 "lang11d_tab.cpp"
    break;

  case 280: /* name: NAME  */
#line 1586 "lang11d"
                       { yyval = zzval; }
#line 4516 "lang11d_tab.cpp"
    break;

  case 281: /* name: WHILE  */
#line 1587 "lang11d"
                                { yyval = zzval; }
#line 4522 "lang11d_tab.cpp"
    break;

  case 282: /* name: EXIT  */
#line 1588 "lang11d"
                               { yyval = zzval; }
#line 4528 "lang11d_tab.cpp"
    break;

  case 283: /* classname: CLASSNAME  */
#line 1591 "lang11d"
                                    { yyval = zzval; }
#line 4534 "lang11d_tab.cpp"
    break;

  case 284: /* primname: PRIMITIVENAME  */
#line 1594 "lang11d"
                                        { yyval = zzval; }
#line 4540 "lang11d_tab.cpp"
    break;

  case 285: /* trueobj: TRUEOBJ  */
#line 1597 "lang11d"
                          { yyval = zzval; }
#line 4546 "lang11d_tab.cpp"
    break;

  case 286: /* falseobj: FALSEOBJ  */
#line 1600 "lang11d"
                           { yyval = zzval; }
#line 4552 "lang11d_tab.cpp"
    break;

  case 287: /* nilobj: NILOBJ  */
#line 1603 "lang11d"
                         { yyval = zzval; }
#line 4558 "lang11d_tab.cpp"
    break;

  case 288: /* ascii: ASCII  */
#line 1606 "lang11d"
                        { yyval = zzval; }
#line 4564 "lang11d_tab.cpp"
    break;

  case 289: /* symbol: SYMBOL  */
#line 1609 "lang11d"
                         { yyval = zzval; }
#line 4570 "lang11d_tab.cpp"
    break;

  case 290: /* string: STRING  */
#line 1612 "lang11d"
                         { yyval = zzval; }
#line 4576 "lang11d_tab.cpp"
    break;

  case 291: /* pseudovar: PSEUDOVAR  */
#line 1615 "lang11d"
                            { yyval = zzval; }
#line 4582 "lang11d_tab.cpp"
    break;

  case 292: /* binop: BINOP  */
#line 1618 "lang11d"
                { yyval = zzval; }
#line 4588 "lang11d_tab.cpp"
    break;

  case 293: /* binop: READWRITEVAR  */
#line 1619 "lang11d"
                               { yyval = zzval; }
#line 4594 "lang11d_tab.cpp"
    break;

  case 294: /* binop: '<'  */
#line 1620 "lang11d"
                       { yyval = zzval; }
#line 4600 "lang11d_tab.cpp"
    break;

  case 295: /* binop: '>'  */
#line 1621 "lang11d"
                       { yyval = zzval; }
#line 4606 "lang11d_tab.cpp"
    break;

  case 296: /* binop: '-'  */
#line 1622 "lang11d"
                       { yyval = zzval; }
#line 4612 "lang11d_tab.cpp"
    break;

  case 297: /* binop: '*'  */
#line 1623 "lang11d"
                       { yyval = zzval; }
#line 4618 "lang11d_tab.cpp"
    break;

  case 298: /* binop: '+'  */
#line 1624 "lang11d"
                       { yyval = zzval; }
#line 4624 "lang11d_tab.cpp"
    break;

  case 299: /* binop: '|'  */
#line 1625 "lang11d"
                       { yyval = zzval; }
#line 4630 "lang11d_tab.cpp"
    break;

  case 300: /* keybinop: KEYBINOP  */
#line 1628 "lang11d"
                    { yyval = zzval; }
#line 4636 "lang11d_tab.cpp"
    break;

  case 303: /* curryarg: CURRYARG  */
#line 1635 "lang11d"
                    { yyval = zzval; }
#line 4642 "lang11d_tab.cpp"
    break;


#line 4646 "lang11d_tab.cpp"

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

