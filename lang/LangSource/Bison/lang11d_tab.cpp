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
  YYSYMBOL_KWEXPAND = 21,                  /* KWEXPAND  */
  YYSYMBOL_ELLIPSIS = 22,                  /* ELLIPSIS  */
  YYSYMBOL_DOTDOT = 23,                    /* DOTDOT  */
  YYSYMBOL_PIE = 24,                       /* PIE  */
  YYSYMBOL_BEGINCLOSEDFUNC = 25,           /* BEGINCLOSEDFUNC  */
  YYSYMBOL_BADTOKEN = 26,                  /* BADTOKEN  */
  YYSYMBOL_INTERPRET = 27,                 /* INTERPRET  */
  YYSYMBOL_BEGINGENERATOR = 28,            /* BEGINGENERATOR  */
  YYSYMBOL_LEFTARROW = 29,                 /* LEFTARROW  */
  YYSYMBOL_WHILE = 30,                     /* WHILE  */
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
  YYSYMBOL_argListWithPerformList = 99,    /* argListWithPerformList  */
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
#define YYFINAL  70
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2131

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  56
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  102
/* YYNRULES -- Number of rules.  */
#define YYNRULES  307
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  578

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
     314,   323,   332,   337,   351,   374,   395,   421,   445,   471,
     475,   481,   499,   505,   505,   515,   515,   522,   543,   547,
     581,   619,   633,   644,   648,   673,   674,   675,   676,   677,
     678,   679,   685,   695,   697,   699,   701,   703,   705,   718,
     721,   748,   766,   793,   821,   840,   868,   895,   913,   938,
     966,   985,  1013,  1032,  1051,  1068,  1082,  1103,  1122,  1140,
    1157,  1173,  1189,  1190,  1191,  1192,  1193,  1206,  1220,  1225,
    1229,  1240,  1245,  1255,  1260,  1274,  1290,  1291,  1292,  1293,
    1296,  1297,  1303,  1306,  1307,  1311,  1312,  1314,  1319,  1321,
    1328,  1336,  1337,  1341,  1343,  1347,  1348,  1352,  1356,  1357,
    1360,  1362,  1366,  1367,  1372,  1373,  1374,  1375,  1376,  1377,
    1378,  1379,  1380,  1383,  1386,  1389,  1390,  1391,  1392,  1393,
    1394,  1395,  1396,  1397,  1400,  1401,  1402,  1403,  1404,  1405,
    1406,  1407,  1408,  1409,  1410,  1413,  1416,  1421,  1422,  1426,
    1427,  1431,  1435,  1436,  1440,  1444,  1448,  1452,  1458,  1462,
    1466,  1470,  1474,  1481,  1482,  1486,  1490,  1491,  1494,  1495,
    1499,  1501,  1503,  1511,  1512,  1515,  1516,  1520,  1522,  1524,
    1532,  1534,  1541,  1542,  1546,  1547,  1550,  1551,  1555,  1557,
    1561,  1565,  1567,  1574,  1575,  1579,  1580,  1585,  1587,  1591,
    1593,  1597,  1598,  1601,  1602,  1606,  1607,  1609,  1611,  1615,
    1616,  1620,  1621,  1630,  1631,  1640,  1641,  1652,  1655,  1656,
    1657,  1663,  1671,  1678,  1687,  1688,  1691,  1694,  1697,  1700,
    1703,  1706,  1709,  1712,  1715,  1718,  1719,  1720,  1721,  1722,
    1723,  1724,  1725,  1726,  1729,  1732,  1733,  1736
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
  "TRUEOBJ", "FALSEOBJ", "PSEUDOVAR", "KWEXPAND", "ELLIPSIS", "DOTDOT",
  "PIE", "BEGINCLOSEDFUNC", "BADTOKEN", "INTERPRET", "BEGINGENERATOR",
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
  "argListWithPerformList", "keyarglist1", "keyarg", "optkeyarglist",
  "mavars", "mavarlist", "slotliteral", "blockliteral", "pushname",
  "pushliteral", "listliteral", "block", "funcvardecls", "funcvardecls1",
  "funcvardecl", "argdecls", "argdecls1", "constdeflist", "constdef",
  "slotdeflist0", "slotdeflist", "slotdef", "vardeflist0", "vardeflist",
  "vardef", "dictslotdef", "dictslotlist1", "dictslotlist",
  "rwslotdeflist", "rwslotdef", "dictlit2", "litdictslotdef",
  "litdictslotlist1", "litdictslotlist", "listlit", "listlit2",
  "literallistc", "literallist1", "rwspec", "rspec", "integer", "floatr",
  "accidental", "pie", "floatp", "name", "classname", "primname",
  "trueobj", "falseobj", "nilobj", "ascii", "symbol", "string",
  "pseudovar", "binop", "keybinop", "binop2", "curryarg", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-402)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-304)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     141,   951,     6,    47,     6,    16,  -402,  -402,  -402,  -402,
    -402,  -402,  -402,  -402,  -402,  -402,    30,    30,  -402,  -402,
    -402,  -402,  -402,   124,  -402,   282,    30,  1799,   121,  1428,
     837,  1799,    30,    97,  -402,  -402,  -402,  -402,  -402,    49,
    -402,  -402,  -402,  2089,    60,    65,  -402,  -402,  -402,  -402,
    1216,  -402,  1216,  -402,   108,   108,  -402,  -402,  -402,   206,
     120,  -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,   102,
    -402,  -402,    25,  -402,    52,  -402,   166,   128,   105,    30,
      30,  -402,  -402,  -402,  -402,  -402,   174,    26,  -402,   242,
     895,  -402,  1799,  1799,  -402,  -402,   176,   178,   192,  1799,
    -402,  1799,  1481,  -402,   282,  -402,  -402,  -402,  -402,    27,
    -402,   201,    15,  1216,  1216,  -402,   194,   209,  -402,  1799,
     224,  2067,   251,  1908,   253,    42,  -402,   248,  1534,  -402,
    -402,    59,  -402,   255,  1799,  -402,  -402,  -402,  -402,  -402,
    1216,  -402,  -402,  1799,  1163,    -3,  -402,  -402,  -402,  1428,
    1004,    -3,  -402,     6,    30,   257,  -402,    30,  1799,  1799,
      30,  -402,   289,   162,   296,    55,  1216,    30,  -402,  -402,
      30,  -402,   743,  -402,  -402,  1216,  1799,  -402,  1428,  -402,
    -402,  -402,  1799,   256,    89,  -402,  1799,  1799,  1799,  -402,
     270,   272,  1216,  1428,  -402,  -402,  -402,    68,  -402,  -402,
    1799,  1908,  1873,  -402,  -402,  -402,   281,   284,   108,  -402,
    -402,   294,  -402,  -402,  -402,  -402,  -402,  -402,  1799,    30,
      30,  1908,  1799,  -402,   126,  1587,  1057,   214,    48,  1799,
    2089,  -402,  2089,  1799,  1799,    -3,   299,   301,  -402,   298,
    1799,    -3,   299,   301,   302,  -402,  1799,   790,  -402,   305,
    -402,  -402,  -402,  2089,   306,   287,    30,  -402,    30,  -402,
     311,  -402,   -24,  -402,  1799,    39,  -402,  -402,   108,  -402,
    -402,  -402,  -402,  -402,  -402,  -402,   309,   312,   321,  -402,
     337,  1799,  -402,  -402,  1799,  1799,  -402,  -402,   346,  -402,
    -402,   318,   342,  -402,  1799,  1269,    -3,  2089,   326,   350,
    -402,   327,   336,  1908,  -402,  1908,  -402,  1908,  2089,  -402,
    -402,   341,   344,  1640,   361,  1799,  1799,   137,  1799,    -3,
     299,   301,   302,  1799,  1110,    -3,  -402,   390,  1799,  -402,
    -402,   353,  1979,  -402,    -3,  1322,  -402,   345,   362,   347,
    -402,  2001,  -402,   349,   351,   362,   356,  -402,  1969,  -402,
    -402,   357,   360,   367,   288,  -402,  -402,   364,   155,  -402,
    -402,    30,   375,  1375,  1375,  -402,  1799,  -402,  -402,   406,
    1799,  -402,    -3,   299,   301,  -402,  1908,  1873,  -402,  -402,
    -402,  -402,   384,  -402,   401,   402,   388,  1799,  -402,   393,
    1693,   413,  2023,  -402,   395,   397,   400,  2089,  1799,    -3,
     299,   301,   302,   404,  -402,  1799,   302,    72,  -402,  -402,
      -3,  -402,  -402,    -3,   408,   420,   124,   124,   421,   234,
     234,   431,  -402,  1927,  -402,  -402,    30,   430,  -402,    30,
     202,   426,   427,    29,   432,  -402,  1799,  -402,    -3,   434,
     437,  -402,  -402,  -402,  1799,  1799,   447,  2089,   450,   451,
     436,  1799,  -402,    -3,  -402,    -3,  2045,  -402,   438,   442,
     443,  -402,  -402,  -402,  1799,  -402,  -402,  -402,   124,   124,
    -402,  -402,  -402,  -402,  -402,  -402,   244,  -402,    30,   278,
    -402,   283,  -402,    30,  -402,   457,  -402,   468,  1799,  1799,
    -402,  1375,  -402,  1799,   472,  -402,  -402,    -3,  -402,  2089,
    2089,  1799,  1799,  1799,   470,  2089,  -402,  -402,  -402,    -3,
    -402,    -3,  2089,  -402,  -402,   165,   165,   288,  -402,   234,
     471,  -402,  -402,   431,   473,  -402,  1799,   427,   427,  -402,
     427,  1799,  -402,  2089,  2089,  2089,  1799,  -402,  -402,   165,
     165,  -402,  1746,   455,  1746,  1948,  -402,  1851,  -402,  1851,
     427,  -402,  -402,  -402,   427,  2089,  1746,  1746,  1799,   464,
    -402,   458,  -402,   467,  -402,  -402,  -402,  -402,  -402,   469,
     477,  2067,  -402,  -402,  -402,  -402,  -402,  -402
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       5,    48,     0,     0,     2,     3,     7,   284,   271,   273,
     275,   292,   293,   291,   286,   307,     0,   233,   290,   288,
     289,   294,   277,   212,   285,     0,   226,     0,   212,   153,
     244,     0,     0,     0,    41,     4,    33,   100,    97,   132,
     109,   134,   133,   150,    27,    48,    96,    98,    95,   183,
      48,   209,    48,   193,   185,   278,   279,   282,   186,   184,
     135,   190,   191,   192,   187,   189,   188,   107,    99,     0,
       1,     6,    14,     8,     0,   235,   237,     0,   234,   233,
     226,   207,   272,   274,   276,   283,     0,    29,   228,    31,
     244,   137,     0,     0,   207,   304,   155,     0,    29,     0,
     297,     0,     0,   295,   300,   298,   299,   301,   302,   226,
     296,     0,     0,    48,    48,   242,    29,     0,   305,   306,
       0,    27,   102,   261,     0,   170,   172,     0,     0,   300,
     303,     0,   306,   146,    28,   152,    34,    40,   210,    39,
      48,   281,   280,     0,     0,    56,    50,    53,    52,   153,
       0,    65,    21,     0,    12,     0,   211,     0,     0,     0,
       0,   218,     0,   234,     0,    29,    48,     0,   220,    30,
       0,    32,     0,    83,    85,    48,     0,   103,    30,   154,
     157,   123,     0,     0,     0,   104,   122,     0,     0,   101,
       0,     0,    48,    30,   245,   106,   241,     0,    28,    49,
       0,   261,   255,   263,   204,   203,     0,    29,   194,   195,
     199,     0,   200,   201,   202,   196,   198,   197,     0,     0,
       0,   261,     0,   161,     0,     0,     0,    54,     0,     0,
     151,    38,   139,     0,     0,     0,    29,    29,    51,     0,
       0,    54,    29,    29,    29,   165,     0,     0,    15,     0,
      13,    16,   236,   238,     0,     0,     0,   213,     0,   215,
       0,   208,     0,   229,     0,     0,   231,   182,   174,   175,
     179,   180,   181,   176,   178,   177,     0,     0,     0,   156,
     158,     0,   127,   105,   128,     0,   124,   240,     0,    37,
      36,     0,     0,   243,     0,     0,    57,   140,     0,     0,
     253,    29,     0,     0,   257,    30,   262,   261,   143,   171,
     173,     0,     0,     0,   108,     0,     0,     0,     0,    54,
      29,    29,    29,     0,     0,    55,    82,     0,     0,   148,
     147,   138,     0,   163,    58,    30,   168,     0,    30,     0,
      64,     0,    66,     0,     0,    30,     0,   167,   301,    11,
      22,     0,     0,    14,    21,   239,   219,     0,     0,   206,
     221,     0,     0,     0,     0,   205,     0,   159,   129,     0,
     126,    35,     0,    29,    29,   259,     0,    30,   256,   250,
     252,   264,     0,   258,   111,   110,     0,     0,   162,     0,
       0,   136,     0,    70,     0,     0,     0,   141,     0,    54,
      29,    29,    29,     0,    78,     0,    29,    54,    62,    77,
      54,    69,   166,    54,     0,     0,   212,   212,     0,   265,
     265,   269,    17,     0,   214,   216,     0,     0,   232,     0,
       0,     0,    87,   184,     0,   160,   130,   125,    60,     0,
       0,   251,   254,   260,     0,     0,   112,   144,   117,   116,
       0,     0,    76,    54,    74,    54,     0,    79,     0,     0,
       0,   149,   164,   169,     0,    59,    68,    67,   212,   212,
     207,   207,    16,   266,   268,   267,     0,   246,     0,     0,
     270,    29,   223,     0,     9,     0,   222,     0,     0,     0,
      84,     0,    92,     0,     0,    86,   131,    54,    63,   114,
     113,     0,     0,     0,   118,   145,    73,    71,    75,    54,
      81,    54,   142,   207,   207,    44,    44,    21,    19,   265,
     248,    18,    20,   269,     0,   217,     0,    87,    87,    88,
      87,     0,    61,   115,   120,   119,     0,    80,    72,    44,
      44,   287,    46,    27,    46,     0,   247,     0,   224,     0,
      87,    94,    93,    89,    87,   121,    46,    46,     0,     0,
      42,    46,    45,     0,    10,   249,   225,    91,    90,     0,
       0,    27,    23,    43,    25,    24,    26,    47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -402,  -402,  -402,  -402,  -402,   503,  -402,   161,    44,  -402,
    -351,  -402,  -120,   -67,  -402,   580,  -402,  -276,  -292,   -44,
     479,   -46,   -82,   232,  -402,   203,  -402,  -402,  -207,  -349,
    -402,  -402,  -402,  -402,  -402,  -402,   -27,  -402,  -402,    91,
     369,  -402,  -138,  -142,  -123,   180,   417,  -402,  -402,  -401,
     271,  -402,  -402,  -177,  -402,   -50,   -23,    23,   -26,   496,
    -402,     9,   449,   453,   365,   459,    -8,   373,   343,  -402,
    -402,   114,    20,  -402,   163,  -402,  -402,  -167,  -402,  -178,
    -402,  -402,  -402,   -87,  -402,  -402,   -14,   -78,     2,    35,
    -402,     7,   143,   238,   286,   398,   425,  -402,  -226,   462,
     -20,  -402
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     3,     4,     5,    71,     6,   249,   155,   354,   422,
     247,   350,   135,   336,   172,    34,    35,   559,   542,   560,
      36,   325,   146,   326,    37,    38,   276,   277,   492,   431,
      39,    40,    41,    42,   111,   183,    43,   229,    44,    45,
      97,    98,   224,   237,   406,   245,   337,   124,   125,   266,
      46,    47,    48,   203,    49,   166,    50,   261,    81,    52,
     481,   482,    86,    87,    88,    77,    74,    75,   115,   116,
     117,   476,   477,   204,   300,   301,   302,    53,   205,   206,
     207,   478,   483,    54,    55,    56,    57,    58,    59,    60,
     543,    61,    62,    63,    64,    65,    66,    67,   118,   132,
     133,    68
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      91,   199,    94,   423,   121,   267,   236,   113,   243,    78,
     120,    85,   242,   145,   151,   434,   360,    14,    76,    76,
     170,   352,    23,   298,    51,   299,   361,   244,    89,   140,
       7,   179,     7,     7,   122,   126,   208,    69,   186,    72,
     141,   142,    28,   311,   175,   209,   187,    70,  -227,   194,
      14,     7,     8,    51,    23,     2,   153,    24,   493,    24,
      24,   143,     7,   238,   219,   188,   168,   189,   127,   238,
     120,   163,   154,   138,    28,    51,   169,  -227,    24,  -303,
     144,    76,    89,   327,   321,   268,   123,   317,   320,    24,
      85,   192,   220,    23,   269,   259,   128,    23,   170,   328,
       7,   156,   157,   322,   464,   169,   225,   230,    14,   134,
     226,    89,   284,    28,   208,   208,   232,    28,    31,   295,
      96,   112,   415,   209,   209,   210,   380,    24,   381,   382,
     212,   253,    22,   227,   208,    79,   138,    51,    79,   285,
     306,   329,   529,   209,   123,    23,   565,   152,   566,   313,
     160,   296,    92,   374,   161,   157,   250,   373,   211,    76,
     390,    80,   255,   138,    80,    28,   545,   149,     1,   262,
      93,   150,    89,   297,   314,   541,   315,   346,    16,   270,
       2,   112,   401,   173,   174,   391,   400,   315,   248,   334,
     180,   308,   181,   184,   141,   425,   167,   352,   158,   441,
     299,   402,   331,   210,   210,   426,   332,   176,   212,   212,
     196,   257,   157,   341,   238,   138,   208,   159,   208,   223,
     208,   309,   310,   210,   544,   209,   177,   209,   212,   209,
     330,    23,   488,   489,   378,   223,   211,   211,   143,    23,
      96,   223,   178,   238,   193,  -230,   323,   556,   557,   351,
     254,    28,   238,   185,   141,   396,   211,   144,   357,    28,
     358,   195,   147,   147,  -230,   324,   213,   279,   563,   280,
     473,   474,  -230,   282,   171,   475,   197,   286,   287,   288,
     569,   570,  -230,   200,   292,   218,    82,    83,    84,   208,
     208,   392,  -230,   518,   519,   221,   397,   228,   209,   209,
     127,   419,   251,   420,   421,   210,    22,   210,   283,   210,
     212,   256,   212,   312,   212,   271,   223,   223,   258,   352,
     551,   552,   289,   553,   290,   333,   438,   521,   519,   304,
     148,   148,   522,   169,   305,   460,   356,   347,   211,   463,
     211,   307,   211,   567,   213,   213,   340,   568,   147,   335,
     414,   338,   345,   353,   147,   362,   238,   359,   355,   363,
     447,   214,   364,   427,   213,   433,   433,   365,   366,   370,
     371,   456,   367,   187,   375,   368,   369,   377,   210,   210,
     267,   376,   267,   212,   212,   196,   223,   145,   379,   383,
     470,   471,   384,   387,    82,   131,    95,   407,   153,   408,
     147,   410,   416,   411,   386,   417,   388,   389,   413,   215,
     272,   211,   211,   424,   523,   223,   148,   499,   500,   403,
     515,   516,   148,   562,   505,   351,   388,   428,   485,   436,
     147,   487,   443,   444,   445,   494,   446,   512,   147,   214,
     214,   448,   513,   514,   147,   451,   213,   453,   213,   454,
     213,   577,   455,   468,   432,   432,   461,   435,   273,   214,
     268,   437,   268,   539,   540,   469,   472,   480,   148,   269,
     486,   269,   490,   342,   533,   534,   535,   491,   495,   501,
     520,   450,   502,   503,   504,   524,   497,   215,   215,   498,
     509,    99,   119,   433,   510,   511,   462,   525,   148,   147,
     526,   531,   536,   547,   198,   549,   148,   215,    73,   555,
     572,   558,   148,   574,   418,   575,   517,   573,   239,   213,
     213,   216,   147,   576,   136,   412,   114,   496,   147,   164,
     252,   571,   548,   165,   479,   263,   293,   147,   162,   546,
     442,   214,     0,   214,     0,   214,     0,   351,   217,     0,
       0,   393,   119,     0,   270,     0,   270,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   148,     0,     0,
     274,     0,     0,     0,     0,   147,     0,     0,     0,   527,
     528,     0,   432,     0,   530,     0,     0,     0,     0,   215,
     148,   215,     0,   215,     0,     0,   148,   275,     0,   216,
     216,     0,   147,     0,     0,   148,     0,     0,     0,     0,
     147,    99,   246,   147,   214,   214,   147,   550,     0,   216,
       0,     0,   554,     0,     0,     0,   217,   217,     0,     0,
     137,   457,   139,   561,     0,   561,   147,     0,     0,   465,
     281,   147,   466,   148,     0,   467,   217,   561,   561,     0,
       0,     0,     0,     0,   339,   294,   147,     0,   147,   343,
     344,     0,   215,   215,   303,     0,     0,     0,     0,     0,
     148,     0,     0,     0,     0,     0,     0,     0,   148,     0,
       0,   148,     0,     0,   148,   506,     0,   507,   246,     0,
     271,     0,   271,   190,   191,     0,     0,     0,     0,     0,
     147,   216,     0,   216,   148,   216,     0,     0,     0,   148,
       0,     0,   147,     0,   147,     0,     0,     0,     0,     0,
     231,     0,     0,     0,   148,     0,   148,     0,   217,   532,
     217,     0,   217,     0,     0,     0,     0,   394,   395,     0,
       0,   537,     0,   538,     0,     0,   260,     8,     9,    10,
      11,    12,    13,     0,     0,   278,     0,     0,     0,     0,
      18,    19,    20,     0,     0,     0,     0,    22,   148,     0,
       0,     0,   291,     0,   216,   216,     0,     0,    25,     0,
     148,     0,   148,     0,     0,   272,   246,   272,     0,     0,
     439,   440,     0,     7,   264,     0,     0,   246,   265,     0,
     246,   217,   217,     0,     0,     0,     0,   246,     0,     0,
       0,   100,     0,     0,     0,     0,     0,   458,   459,     0,
      24,     0,     0,   103,     0,   129,   105,   106,   348,   108,
     130,   110,     0,   273,     0,   273,   349,     0,     0,   303,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
      16,    17,     0,     0,    18,    19,    20,    21,   100,     0,
     101,    22,    23,     0,     0,     0,     0,    24,   102,     0,
     103,    95,   104,   105,   106,   107,   108,   109,   110,     0,
      27,     0,    28,     0,    29,     0,     0,     0,    90,     0,
       0,    32,    33,     0,     0,     0,     0,     0,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,     0,     0,
       0,     0,    18,    19,    20,    21,   100,     0,   101,    22,
      23,     0,     0,     0,     0,    24,   102,     0,   103,    95,
     104,   105,   106,   107,   108,   130,   110,     0,    27,     0,
      28,     0,    29,     0,     0,   274,    90,   274,     0,    32,
      33,     0,     0,     0,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,    16,    17,     0,     0,    18,    19,
      20,    21,   275,     0,   275,    22,    23,     0,     0,     0,
       0,    24,     0,     0,     0,     0,    25,     0,     0,     0,
       0,    26,     0,     0,    27,     0,    28,     0,    29,     0,
       0,     0,    30,     0,    31,    32,    33,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,     0,     0,     0,
       0,    18,    19,    20,    21,   240,     0,     0,    22,    23,
       0,     0,     0,     0,    24,     0,     0,     0,    95,    25,
       0,     0,   234,     0,     0,     0,     0,    27,     0,    28,
       0,    29,     0,     0,     0,    90,   241,     0,    32,    33,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    18,    19,    20,    21,   318,     0,
       0,    22,    23,     0,     0,     0,     0,    24,     0,     0,
       0,    95,    25,     0,     0,   234,     0,     0,     0,     0,
      27,     0,    28,     0,    29,     0,     0,     0,    90,   319,
       0,    32,    33,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,     0,     0,     0,     0,    18,    19,    20,
      21,   398,     0,     0,    22,    23,     0,     0,     0,     0,
      24,     0,     0,     0,    95,    25,     0,     0,   234,     0,
       0,     0,     0,    27,     0,    28,     0,    29,     0,     0,
       0,    90,   399,     0,    32,    33,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,     0,     0,     0,     0,
      18,    19,    20,    21,   233,     0,     0,    22,    23,     0,
       0,     0,     0,    24,     0,     0,     0,     0,    25,     0,
       0,   234,     0,     0,     0,     0,    27,     0,    28,     0,
      29,     0,     0,     0,    90,   235,     0,    32,    33,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,    16,
       0,     0,     0,    18,    19,    20,    21,     0,     0,     0,
      22,    23,     0,     0,     0,     0,    24,     0,     0,     0,
       0,    25,     0,     0,     0,     0,     0,     0,     0,    27,
       0,    28,     0,    29,     0,     0,     0,    90,     0,    31,
      32,    33,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    18,    19,    20,    21,
       0,     0,     0,    22,    23,     0,     0,     0,     0,    24,
       0,     0,     0,     0,    25,     0,     0,   234,     0,     0,
       0,     0,    27,     0,    28,     0,    29,     0,     0,     0,
      90,   372,     0,    32,    33,     7,     8,     9,    10,    11,
      12,    13,     0,    14,    15,     0,     0,     0,     0,    18,
      19,    20,    21,     0,     0,     0,    22,    23,     0,     0,
       0,     0,    24,     0,     0,     0,    95,    25,     0,     0,
     405,     0,     0,     0,     0,    27,     0,    28,     0,    29,
       0,     0,     0,    90,     0,     0,    32,    33,     7,     8,
       9,    10,    11,    12,    13,     0,    14,    15,   429,     0,
       0,     0,    18,    19,    20,    21,     0,     0,     0,    22,
      23,     0,     0,     0,     0,    24,   430,     0,     0,     0,
      25,     0,     0,     0,     0,     0,     0,     0,    27,     0,
      28,     0,    29,     0,     0,     0,    90,     0,     0,    32,
      33,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,     0,     0,     0,     0,    18,    19,    20,    21,     0,
       0,     0,    22,    23,     0,     0,     0,     0,    24,     0,
       0,     0,    95,    25,     0,     0,     0,     0,     0,     0,
       0,    27,     0,    28,     0,    29,     0,     0,     0,    90,
       0,     0,    32,    33,     7,     8,     9,    10,    11,    12,
      13,     0,    14,    15,     0,     0,     0,     0,    18,    19,
      20,    21,     0,     0,   182,    22,    23,     0,     0,     0,
       0,    24,     0,     0,     0,     0,    25,     0,     0,     0,
       0,     0,     0,     0,    27,     0,    28,     0,    29,     0,
       0,     0,    90,     0,     0,    32,    33,     7,     8,     9,
      10,    11,    12,    13,     0,    14,    15,     0,     0,     0,
       0,    18,    19,    20,    21,     0,     0,   222,    22,    23,
       0,     0,     0,     0,    24,     0,     0,     0,     0,    25,
       0,     0,     0,     0,     0,     0,     0,    27,     0,    28,
       0,    29,     0,     0,     0,    90,     0,     0,    32,    33,
       7,     8,     9,    10,    11,    12,    13,     0,    14,    15,
       0,     0,     0,     0,    18,    19,    20,    21,     0,     0,
     316,    22,    23,     0,     0,     0,     0,    24,     0,     0,
       0,     0,    25,     0,     0,     0,     0,     0,     0,     0,
      27,     0,    28,     0,    29,     0,     0,     0,    90,     0,
       0,    32,    33,     7,     8,     9,    10,    11,    12,    13,
       0,    14,    15,     0,     0,     0,     0,    18,    19,    20,
      21,     0,     0,     0,    22,    23,     0,     0,     0,     0,
      24,     0,     0,     0,     0,    25,     0,     0,     0,     0,
       0,     0,     0,    27,     0,    28,     0,    29,   385,     0,
       0,    90,     0,     0,    32,    33,     7,     8,     9,    10,
      11,    12,    13,     0,    14,    15,     0,     0,     0,     0,
      18,    19,    20,    21,     0,     0,     0,    22,    23,     0,
       0,     0,     0,    24,     0,     0,     0,     0,    25,     0,
       0,     0,     0,     0,     0,     0,    27,     0,    28,     0,
      29,   449,     0,     0,    90,     0,     0,    32,    33,     7,
       8,     9,    10,    11,    12,    13,     0,    14,    15,     0,
       0,     0,     0,    18,    19,    20,    21,     0,     0,     0,
      22,    23,     0,     0,     0,     0,    24,     0,     0,     0,
       0,    25,     0,     0,     0,     0,     0,     0,     0,    27,
       0,    28,     0,    29,     0,     0,     0,    90,     0,   558,
      32,    33,     7,     8,     9,    10,    11,    12,    13,     0,
      14,    15,     0,     0,     0,     0,    18,    19,    20,    21,
       0,     0,     0,    22,    23,     0,     0,     0,     0,    24,
       0,     0,     0,     0,    25,     0,     0,     0,     0,     0,
       0,     0,    27,     0,    28,     0,    29,     0,     0,     0,
      90,     0,     0,    32,    33,     8,     9,    10,    11,    12,
      13,     0,     0,     0,     0,     0,     0,     0,    18,    19,
      20,     0,     0,     0,     0,    22,     7,     8,     9,    10,
      11,    12,    13,     0,    14,     0,    25,     0,     0,     0,
      18,    19,    20,     0,     0,     0,     0,    22,     0,     0,
       0,     0,     0,    24,     0,     0,   265,    95,    25,     0,
       0,     7,     8,     9,    10,    11,    12,    13,     0,    14,
     201,     0,     0,     0,   202,    18,    19,    20,     0,     0,
       7,     0,    22,     0,     0,     0,     0,     0,    24,     0,
       0,     0,     0,    25,     0,     0,     0,     0,   100,     0,
       0,     7,     0,     0,     0,   201,     0,    24,     0,   202,
     103,     0,   129,   105,   106,   348,   108,   130,   110,   100,
       0,     0,     7,   484,     0,     0,     0,     0,    24,     0,
       0,   103,     0,   129,   105,   106,   348,   108,   130,   110,
     100,     0,     0,     0,   564,     0,     0,     0,     0,    24,
     100,     0,   103,     0,   129,   105,   106,   107,   108,   130,
     110,     0,   103,    95,   129,   105,   106,   107,   108,   130,
     110,   131,   100,     0,     0,     0,     0,     0,     0,     0,
       0,   404,     0,     0,   103,    95,   129,   105,   106,   107,
     108,   130,   110,   131,   100,     0,     0,     0,     0,     0,
       0,     0,     0,   409,     0,     0,   103,    95,   129,   105,
     106,   107,   108,   130,   110,   131,   100,     0,     0,     0,
       0,     0,     0,     0,     0,   452,     0,     0,   103,    95,
     129,   105,   106,   107,   108,   130,   110,   131,   100,     0,
       0,     0,     0,     0,     0,     0,     0,   508,     0,     0,
     103,    95,   129,   105,   106,   107,   108,   130,   110,   131,
     100,     0,     0,     0,     0,     0,   198,     0,     0,     0,
       0,     0,   103,    95,   129,   105,   106,   107,   108,   130,
     110,   131
};

static const yytype_int16 yycheck[] =
{
      27,   121,    28,   354,    31,   172,   144,    30,   150,    17,
      30,    25,   150,    59,    60,   364,    40,    11,    16,    17,
      87,   247,    25,   201,     1,   202,    50,   150,    26,    52,
       3,    98,     3,     3,    32,    33,   123,     2,    23,     4,
      54,    55,    45,   221,    94,   123,    31,     0,    22,   116,
      11,     3,     4,    30,    25,    39,    31,    30,    29,    30,
      30,    32,     3,   145,    22,    50,    40,    52,    33,   151,
      90,    79,    47,    50,    45,    52,    50,    22,    30,    52,
      51,    79,    80,    35,   226,   172,    47,   225,   226,    30,
     104,   114,    50,    25,   172,    40,    47,    25,   165,    51,
       3,    49,    50,   226,    32,    50,    47,   134,    11,    49,
      51,   109,    23,    45,   201,   202,   143,    45,    53,    51,
      29,    30,   348,   201,   202,   123,   303,    30,   305,   307,
     123,   158,    24,   131,   221,    14,   113,   114,    14,    50,
     207,   228,   491,   221,    47,    25,   547,    45,   549,    23,
      22,   197,    31,   295,    49,    50,   154,   295,   123,   157,
      23,    40,   160,   140,    40,    45,   517,    47,    27,   167,
      49,    51,   170,   200,    48,    10,    50,   244,    13,   172,
      39,    90,   324,    92,    93,    48,   324,    50,   153,   235,
      99,   218,   101,   102,   208,    40,    22,   423,    32,   376,
     377,   324,   229,   201,   202,    50,   233,    31,   201,   202,
     119,    49,    50,   240,   296,   192,   303,    51,   305,   128,
     307,   219,   220,   221,   516,   303,    48,   305,   221,   307,
     228,    25,    30,    31,   301,   144,   201,   202,    32,    25,
     149,   150,    50,   325,    50,     3,    32,   539,   540,   247,
     159,    45,   334,    52,   268,   322,   221,    51,   256,    45,
     258,    52,    59,    60,    22,    51,   123,   176,   544,   178,
      36,    37,    30,   182,    32,    41,    52,   186,   187,   188,
     556,   557,    40,    32,   193,    32,     4,     5,     6,   376,
     377,   318,    50,    49,    50,    47,   323,    42,   376,   377,
     265,    13,    45,    15,    16,   303,    24,   305,    52,   307,
     303,    22,   305,   222,   307,   172,   225,   226,    22,   545,
     527,   528,    52,   530,    52,   234,   372,    49,    50,    48,
      59,    60,    49,    50,    50,   402,    49,   246,   303,   406,
     305,    47,   307,   550,   201,   202,    48,   554,   145,    50,
     348,    50,    50,    48,   151,   264,   438,    46,    52,    50,
     387,   123,    50,   361,   221,   363,   364,    46,    31,    23,
      52,   398,   281,    31,    48,   284,   285,    50,   376,   377,
     547,    31,   549,   376,   377,   294,   295,   433,    52,    48,
     416,   417,    48,    32,     4,    42,    34,    52,    31,    52,
     197,    52,    45,    52,   313,    45,   315,   316,    52,   123,
     172,   376,   377,    49,   481,   324,   145,   444,   445,   328,
     470,   471,   151,   543,   451,   423,   335,    52,   426,    23,
     227,   429,    48,    32,    32,   433,    48,   464,   235,   201,
     202,    48,   468,   469,   241,    32,   303,    52,   305,    52,
     307,   571,    52,    45,   363,   364,    52,   366,   172,   221,
     547,   370,   549,   513,   514,    45,    45,    36,   197,   547,
      40,   549,    46,   241,   501,   502,   503,    50,    46,    32,
     478,   390,    32,    32,    48,   483,    52,   201,   202,    52,
      52,    29,    30,   491,    52,    52,   405,    40,   227,   296,
      32,    29,    32,    32,    49,    32,   235,   221,     5,   536,
      46,    53,   241,    46,   353,    46,   472,   561,   149,   376,
     377,   123,   319,    46,    45,   345,    30,   436,   325,    80,
     157,   558,   523,    80,   420,   170,   193,   334,    79,   519,
     377,   303,    -1,   305,    -1,   307,    -1,   545,   123,    -1,
      -1,   319,    90,    -1,   547,    -1,   549,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   296,    -1,    -1,
     172,    -1,    -1,    -1,    -1,   372,    -1,    -1,    -1,   488,
     489,    -1,   491,    -1,   493,    -1,    -1,    -1,    -1,   303,
     319,   305,    -1,   307,    -1,    -1,   325,   172,    -1,   201,
     202,    -1,   399,    -1,    -1,   334,    -1,    -1,    -1,    -1,
     407,   149,   150,   410,   376,   377,   413,   526,    -1,   221,
      -1,    -1,   531,    -1,    -1,    -1,   201,   202,    -1,    -1,
      50,   399,    52,   542,    -1,   544,   433,    -1,    -1,   407,
     178,   438,   410,   372,    -1,   413,   221,   556,   557,    -1,
      -1,    -1,    -1,    -1,   237,   193,   453,    -1,   455,   242,
     243,    -1,   376,   377,   202,    -1,    -1,    -1,    -1,    -1,
     399,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   407,    -1,
      -1,   410,    -1,    -1,   413,   453,    -1,   455,   226,    -1,
     547,    -1,   549,   113,   114,    -1,    -1,    -1,    -1,    -1,
     497,   303,    -1,   305,   433,   307,    -1,    -1,    -1,   438,
      -1,    -1,   509,    -1,   511,    -1,    -1,    -1,    -1,    -1,
     140,    -1,    -1,    -1,   453,    -1,   455,    -1,   303,   497,
     305,    -1,   307,    -1,    -1,    -1,    -1,   320,   321,    -1,
      -1,   509,    -1,   511,    -1,    -1,   166,     4,     5,     6,
       7,     8,     9,    -1,    -1,   175,    -1,    -1,    -1,    -1,
      17,    18,    19,    -1,    -1,    -1,    -1,    24,   497,    -1,
      -1,    -1,   192,    -1,   376,   377,    -1,    -1,    35,    -1,
     509,    -1,   511,    -1,    -1,   547,   324,   549,    -1,    -1,
     373,   374,    -1,     3,    51,    -1,    -1,   335,    55,    -1,
     338,   376,   377,    -1,    -1,    -1,    -1,   345,    -1,    -1,
      -1,    21,    -1,    -1,    -1,    -1,    -1,   400,   401,    -1,
      30,    -1,    -1,    33,    -1,    35,    36,    37,    38,    39,
      40,    41,    -1,   547,    -1,   549,    46,    -1,    -1,   377,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      13,    14,    -1,    -1,    17,    18,    19,    20,    21,    -1,
      23,    24,    25,    -1,    -1,    -1,    -1,    30,    31,    -1,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    -1,
      43,    -1,    45,    -1,    47,    -1,    -1,    -1,    51,    -1,
      -1,    54,    55,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    -1,    -1,
      -1,    -1,    17,    18,    19,    20,    21,    -1,    23,    24,
      25,    -1,    -1,    -1,    -1,    30,    31,    -1,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    -1,    43,    -1,
      45,    -1,    47,    -1,    -1,   547,    51,   549,    -1,    54,
      55,    -1,    -1,    -1,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    13,    14,    -1,    -1,    17,    18,
      19,    20,   547,    -1,   549,    24,    25,    -1,    -1,    -1,
      -1,    30,    -1,    -1,    -1,    -1,    35,    -1,    -1,    -1,
      -1,    40,    -1,    -1,    43,    -1,    45,    -1,    47,    -1,
      -1,    -1,    51,    -1,    53,    54,    55,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,
      -1,    17,    18,    19,    20,    21,    -1,    -1,    24,    25,
      -1,    -1,    -1,    -1,    30,    -1,    -1,    -1,    34,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    43,    -1,    45,
      -1,    47,    -1,    -1,    -1,    51,    52,    -1,    54,    55,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    21,    -1,
      -1,    24,    25,    -1,    -1,    -1,    -1,    30,    -1,    -1,
      -1,    34,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      43,    -1,    45,    -1,    47,    -1,    -1,    -1,    51,    52,
      -1,    54,    55,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,
      20,    21,    -1,    -1,    24,    25,    -1,    -1,    -1,    -1,
      30,    -1,    -1,    -1,    34,    35,    -1,    -1,    38,    -1,
      -1,    -1,    -1,    43,    -1,    45,    -1,    47,    -1,    -1,
      -1,    51,    52,    -1,    54,    55,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,
      17,    18,    19,    20,    21,    -1,    -1,    24,    25,    -1,
      -1,    -1,    -1,    30,    -1,    -1,    -1,    -1,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    43,    -1,    45,    -1,
      47,    -1,    -1,    -1,    51,    52,    -1,    54,    55,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    13,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,    -1,
      24,    25,    -1,    -1,    -1,    -1,    30,    -1,    -1,    -1,
      -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      -1,    45,    -1,    47,    -1,    -1,    -1,    51,    -1,    53,
      54,    55,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    -1,    24,    25,    -1,    -1,    -1,    -1,    30,
      -1,    -1,    -1,    -1,    35,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    43,    -1,    45,    -1,    47,    -1,    -1,    -1,
      51,    52,    -1,    54,    55,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,
      18,    19,    20,    -1,    -1,    -1,    24,    25,    -1,    -1,
      -1,    -1,    30,    -1,    -1,    -1,    34,    35,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    43,    -1,    45,    -1,    47,
      -1,    -1,    -1,    51,    -1,    -1,    54,    55,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    12,    13,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    -1,    -1,    24,
      25,    -1,    -1,    -1,    -1,    30,    31,    -1,    -1,    -1,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,
      45,    -1,    47,    -1,    -1,    -1,    51,    -1,    -1,    54,
      55,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,
      -1,    -1,    24,    25,    -1,    -1,    -1,    -1,    30,    -1,
      -1,    -1,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    -1,    45,    -1,    47,    -1,    -1,    -1,    51,
      -1,    -1,    54,    55,     3,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    -1,    23,    24,    25,    -1,    -1,    -1,
      -1,    30,    -1,    -1,    -1,    -1,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    -1,    45,    -1,    47,    -1,
      -1,    -1,    51,    -1,    -1,    54,    55,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    -1,    23,    24,    25,
      -1,    -1,    -1,    -1,    30,    -1,    -1,    -1,    -1,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,    45,
      -1,    47,    -1,    -1,    -1,    51,    -1,    -1,    54,    55,
       3,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,
      23,    24,    25,    -1,    -1,    -1,    -1,    30,    -1,    -1,
      -1,    -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    -1,    45,    -1,    47,    -1,    -1,    -1,    51,    -1,
      -1,    54,    55,     3,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    -1,    -1,    24,    25,    -1,    -1,    -1,    -1,
      30,    -1,    -1,    -1,    -1,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    -1,    45,    -1,    47,    48,    -1,
      -1,    51,    -1,    -1,    54,    55,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,
      17,    18,    19,    20,    -1,    -1,    -1,    24,    25,    -1,
      -1,    -1,    -1,    30,    -1,    -1,    -1,    -1,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,    45,    -1,
      47,    48,    -1,    -1,    51,    -1,    -1,    54,    55,     3,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    -1,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,    -1,
      24,    25,    -1,    -1,    -1,    -1,    30,    -1,    -1,    -1,
      -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      -1,    45,    -1,    47,    -1,    -1,    -1,    51,    -1,    53,
      54,    55,     3,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    -1,    24,    25,    -1,    -1,    -1,    -1,    30,
      -1,    -1,    -1,    -1,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    43,    -1,    45,    -1,    47,    -1,    -1,    -1,
      51,    -1,    -1,    54,    55,     4,     5,     6,     7,     8,
       9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,    18,
      19,    -1,    -1,    -1,    -1,    24,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    -1,    35,    -1,    -1,    -1,
      17,    18,    19,    -1,    -1,    -1,    -1,    24,    -1,    -1,
      -1,    -1,    -1,    30,    -1,    -1,    55,    34,    35,    -1,
      -1,     3,     4,     5,     6,     7,     8,     9,    -1,    11,
      47,    -1,    -1,    -1,    51,    17,    18,    19,    -1,    -1,
       3,    -1,    24,    -1,    -1,    -1,    -1,    -1,    30,    -1,
      -1,    -1,    -1,    35,    -1,    -1,    -1,    -1,    21,    -1,
      -1,     3,    -1,    -1,    -1,    47,    -1,    30,    -1,    51,
      33,    -1,    35,    36,    37,    38,    39,    40,    41,    21,
      -1,    -1,     3,    46,    -1,    -1,    -1,    -1,    30,    -1,
      -1,    33,    -1,    35,    36,    37,    38,    39,    40,    41,
      21,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    30,
      21,    -1,    33,    -1,    35,    36,    37,    38,    39,    40,
      41,    -1,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    21,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    -1,    -1,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    21,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    21,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      21,    -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,
      -1,    -1,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    27,    39,    57,    58,    59,    61,     3,     4,     5,
       6,     7,     8,     9,    11,    12,    13,    14,    17,    18,
      19,    20,    24,    25,    30,    35,    40,    43,    45,    47,
      51,    53,    54,    55,    71,    72,    76,    80,    81,    86,
      87,    88,    89,    92,    94,    95,   106,   107,   108,   110,
     112,   113,   115,   133,   139,   140,   141,   142,   143,   144,
     145,   147,   148,   149,   150,   151,   152,   153,   157,   145,
       0,    60,   145,    61,   122,   123,   144,   121,   122,    14,
      40,   114,     4,     5,     6,   142,   118,   119,   120,   144,
      51,    92,    31,    49,   114,    34,    95,    96,    97,   155,
      21,    23,    31,    33,    35,    36,    37,    38,    39,    40,
      41,    90,    95,   112,   115,   124,   125,   126,   154,   155,
     156,    92,   144,    47,   103,   104,   144,   145,    47,    35,
      40,    42,   155,   156,    49,    68,    76,    71,   113,    71,
     112,   142,   142,    32,    51,    77,    78,    81,   106,    47,
      51,    77,    45,    31,    47,    63,    49,    50,    32,    51,
      22,    49,   121,   122,   118,   119,   111,    22,    40,    50,
      69,    32,    70,    95,    95,   111,    31,    48,    50,    69,
      95,    95,    23,    91,    95,    52,    23,    31,    50,    52,
      71,    71,   112,    50,    69,    52,    95,    52,    49,    68,
      32,    47,    51,   109,   129,   134,   135,   136,   139,   143,
     144,   145,   147,   148,   149,   150,   151,   152,    32,    22,
      50,    47,    23,    95,    98,    47,    51,   144,    42,    93,
      92,    71,    92,    21,    38,    52,    98,    99,    78,    96,
      21,    52,    98,    99,   100,   101,   155,    66,   145,    62,
     144,    45,   123,    92,    95,   144,    22,    49,    22,    40,
      71,   113,   144,   120,    51,    55,   105,   133,   139,   143,
     147,   148,   149,   150,   151,   152,    82,    83,    71,    95,
      95,   155,    95,    52,    23,    50,    95,    95,    95,    52,
      52,    71,    95,   124,   155,    51,    77,    92,   135,   109,
     130,   131,   132,   155,    48,    50,    69,    47,    92,   144,
     144,   135,    95,    23,    48,    50,    23,    98,    21,    52,
      98,    99,   100,    32,    51,    77,    79,    35,    51,   139,
     144,    92,    92,    95,    77,    50,    69,   102,    50,   102,
      48,    92,    79,   102,   102,    50,    69,    95,    38,    46,
      67,   144,   154,    48,    64,    52,    49,   144,   144,    46,
      40,    50,    95,    50,    50,    46,    31,    95,    95,    95,
      23,    52,    52,    98,    99,    48,    31,    50,    69,    52,
     109,   109,   135,    48,    48,    48,    95,    32,    95,    95,
      23,    48,    92,    79,   102,   102,    69,    92,    21,    52,
      98,    99,   100,    95,    52,    38,   100,    52,    52,    52,
      52,    52,   101,    52,   144,   154,    45,    45,    63,    13,
      15,    16,    65,    66,    49,    40,    50,   144,    52,    13,
      31,    85,    95,   144,    85,    95,    23,    95,    77,   102,
     102,   109,   130,    48,    32,    32,    48,    92,    48,    48,
      95,    32,    52,    52,    52,    52,    92,    79,   102,   102,
      69,    52,    95,    69,    32,    79,    79,    79,    45,    45,
     114,   114,    45,    36,    37,    41,   127,   128,   137,   127,
      36,   116,   117,   138,    46,   144,    40,   144,    30,    31,
      46,    50,    84,    29,   144,    46,    95,    52,    52,    92,
      92,    32,    32,    32,    48,    92,    79,    79,    52,    52,
      52,    52,    92,   114,   114,   111,   111,    64,    49,    50,
     144,    49,    49,    69,   144,    40,    32,    95,    95,    85,
      95,    29,    79,    92,    92,    92,    32,    79,    79,   111,
     111,    10,    74,   146,    74,    66,   128,    32,   117,    32,
      95,    84,    84,    84,    95,    92,    74,    74,    53,    73,
      75,    95,    68,    73,    46,   105,   105,    84,    84,    73,
      73,    92,    46,    75,    46,    46,    46,    68
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
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    82,    81,    83,    81,    84,    84,    85,
      85,    85,    85,    85,    85,    86,    86,    86,    86,    86,
      86,    86,    86,    86,    86,    86,    86,    86,    86,    86,
      87,    87,    87,    88,    88,    88,    89,    89,    89,    89,
      89,    89,    90,    90,    90,    90,    90,    91,    91,    91,
      91,    91,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    93,    93,    93,    93,
      94,    94,    95,    96,    96,    97,    97,    97,    97,    97,
      97,    98,    98,    99,    99,   100,   100,   101,   102,   102,
     103,   103,   104,   104,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   106,   107,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   110,   110,   111,   111,   112,
     112,   113,   114,   114,   114,   114,   114,   114,   115,   115,
     115,   115,   115,   116,   116,   117,   118,   118,   119,   119,
     120,   120,   120,   121,   121,   122,   122,   123,   123,   123,
     124,   124,   125,   125,   126,   126,   127,   127,   128,   128,
     129,   130,   130,   131,   131,   132,   132,   133,   133,   134,
     134,   135,   135,   136,   136,   137,   137,   137,   137,   138,
     138,   139,   139,   140,   140,   141,   141,   142,   143,   143,
     143,   143,   143,   143,   144,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   155,   156,   156,   157
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
       5,     7,     8,     7,     6,     7,     6,     5,     5,     6,
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
       2,     3,     0,     3,     5,     3,     5,     7,     3,     5,
       3,     5,     7,     1,     3,     4,     0,     1,     1,     3,
       1,     3,     5,     0,     1,     1,     3,     1,     3,     4,
       3,     2,     1,     3,     0,     2,     1,     3,     2,     4,
       3,     3,     2,     1,     3,     0,     2,     4,     5,     3,
       4,     0,     2,     1,     3,     0,     1,     1,     1,     0,
       1,     1,     2,     1,     2,     1,     2,     1,     1,     1,
       2,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1
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
#line 2228 "lang11d_tab.cpp"
    break;

  case 3: /* root: classextensions  */
#line 42 "lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; }
#line 2234 "lang11d_tab.cpp"
    break;

  case 4: /* root: INTERPRET cmdlinecode  */
#line 44 "lang11d"
                        { gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 2; }
#line 2240 "lang11d_tab.cpp"
    break;

  case 5: /* classes: %empty  */
#line 47 "lang11d"
          { yyval = 0; }
#line 2246 "lang11d_tab.cpp"
    break;

  case 6: /* classes: classes classdef  */
#line 49 "lang11d"
                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2252 "lang11d_tab.cpp"
    break;

  case 8: /* classextensions: classextensions classextension  */
#line 54 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2258 "lang11d_tab.cpp"
    break;

  case 9: /* classdef: classname superclass '{' classvardecls methods '}'  */
#line 58 "lang11d"
                                { yyval = (intptr_t)newPyrClassNode((PyrSlotNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1], 0);
				}
#line 2266 "lang11d_tab.cpp"
    break;

  case 10: /* classdef: classname '[' optname ']' superclass '{' classvardecls methods '}'  */
#line 62 "lang11d"
                                { yyval = (intptr_t)newPyrClassNode((PyrSlotNode*)yyvsp[-8], (PyrSlotNode*)yyvsp[-4],
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1],
					(PyrSlotNode*)yyvsp[-6]);
				}
#line 2275 "lang11d_tab.cpp"
    break;

  case 11: /* classextension: '+' classname '{' methods '}'  */
#line 69 "lang11d"
                                {
					yyval = (intptr_t)newPyrClassExtNode((PyrSlotNode*)yyvsp[-3], (PyrMethodNode*)yyvsp[-1]);
				}
#line 2283 "lang11d_tab.cpp"
    break;

  case 12: /* optname: %empty  */
#line 74 "lang11d"
                  { yyval = 0; }
#line 2289 "lang11d_tab.cpp"
    break;

  case 14: /* superclass: %empty  */
#line 78 "lang11d"
                  { yyval = 0; }
#line 2295 "lang11d_tab.cpp"
    break;

  case 15: /* superclass: ':' classname  */
#line 80 "lang11d"
                                { yyval = yyvsp[0]; }
#line 2301 "lang11d_tab.cpp"
    break;

  case 16: /* classvardecls: %empty  */
#line 83 "lang11d"
                  { yyval = 0; }
#line 2307 "lang11d_tab.cpp"
    break;

  case 17: /* classvardecls: classvardecls classvardecl  */
#line 85 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2313 "lang11d_tab.cpp"
    break;

  case 18: /* classvardecl: CLASSVAR rwslotdeflist ';'  */
#line 89 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varClass); }
#line 2319 "lang11d_tab.cpp"
    break;

  case 19: /* classvardecl: VAR rwslotdeflist ';'  */
#line 91 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varInst); }
#line 2325 "lang11d_tab.cpp"
    break;

  case 20: /* classvardecl: SC_CONST constdeflist ';'  */
#line 93 "lang11d"
                                        { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varConst); }
#line 2331 "lang11d_tab.cpp"
    break;

  case 21: /* methods: %empty  */
#line 96 "lang11d"
                  { yyval = 0; }
#line 2337 "lang11d_tab.cpp"
    break;

  case 22: /* methods: methods methoddef  */
#line 98 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2343 "lang11d_tab.cpp"
    break;

  case 23: /* methoddef: name '{' argdecls funcvardecls primitive methbody '}'  */
#line 102 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2350 "lang11d_tab.cpp"
    break;

  case 24: /* methoddef: '*' name '{' argdecls funcvardecls primitive methbody '}'  */
#line 105 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2357 "lang11d_tab.cpp"
    break;

  case 25: /* methoddef: binop '{' argdecls funcvardecls primitive methbody '}'  */
#line 108 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); }
#line 2364 "lang11d_tab.cpp"
    break;

  case 26: /* methoddef: '*' binop '{' argdecls funcvardecls primitive methbody '}'  */
#line 111 "lang11d"
                                { yyval = (intptr_t)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2],
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); }
#line 2371 "lang11d_tab.cpp"
    break;

  case 34: /* funcbody: exprseq funretval  */
#line 129 "lang11d"
                                { yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2377 "lang11d_tab.cpp"
    break;

  case 35: /* cmdlinecode: '(' argdecls1 funcvardecls1 funcbody ')'  */
#line 133 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2383 "lang11d_tab.cpp"
    break;

  case 36: /* cmdlinecode: '(' argdecls1 funcbody ')'  */
#line 135 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-2], NULL, (PyrParseNode*)yyvsp[-1], false); }
#line 2389 "lang11d_tab.cpp"
    break;

  case 37: /* cmdlinecode: '(' funcvardecls1 funcbody ')'  */
#line 137 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); }
#line 2395 "lang11d_tab.cpp"
    break;

  case 38: /* cmdlinecode: argdecls1 funcvardecls1 funcbody  */
#line 139 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-2], (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2401 "lang11d_tab.cpp"
    break;

  case 39: /* cmdlinecode: argdecls1 funcbody  */
#line 141 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-1], NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2407 "lang11d_tab.cpp"
    break;

  case 40: /* cmdlinecode: funcvardecls1 funcbody  */
#line 143 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); }
#line 2413 "lang11d_tab.cpp"
    break;

  case 41: /* cmdlinecode: funcbody  */
#line 145 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode(NULL, NULL, (PyrParseNode*)yyvsp[0], false); }
#line 2419 "lang11d_tab.cpp"
    break;

  case 43: /* methbody: exprseq retval  */
#line 150 "lang11d"
                                { yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 2425 "lang11d_tab.cpp"
    break;

  case 44: /* primitive: %empty  */
#line 153 "lang11d"
                  { yyval = 0; }
#line 2431 "lang11d_tab.cpp"
    break;

  case 45: /* primitive: primname optsemi  */
#line 155 "lang11d"
                                { yyval = yyvsp[-1]; }
#line 2437 "lang11d_tab.cpp"
    break;

  case 46: /* retval: %empty  */
#line 159 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode(NULL); }
#line 2443 "lang11d_tab.cpp"
    break;

  case 47: /* retval: '^' expr optsemi  */
#line 161 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); }
#line 2449 "lang11d_tab.cpp"
    break;

  case 48: /* funretval: %empty  */
#line 165 "lang11d"
                        { yyval = (intptr_t)newPyrBlockReturnNode(); }
#line 2455 "lang11d_tab.cpp"
    break;

  case 49: /* funretval: '^' expr optsemi  */
#line 167 "lang11d"
                        { yyval = (intptr_t)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); }
#line 2461 "lang11d_tab.cpp"
    break;

  case 51: /* blocklist1: blocklist1 blocklistitem  */
#line 172 "lang11d"
                                {
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
				}
#line 2469 "lang11d_tab.cpp"
    break;

  case 54: /* blocklist: %empty  */
#line 181 "lang11d"
                        { yyval = 0; }
#line 2475 "lang11d_tab.cpp"
    break;

  case 56: /* msgsend: name blocklist1  */
#line 186 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2483 "lang11d_tab.cpp"
    break;

  case 57: /* msgsend: '(' binop2 ')' blocklist1  */
#line 190 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0, 0);
			}
#line 2491 "lang11d_tab.cpp"
    break;

  case 58: /* msgsend: name '(' ')' blocklist1  */
#line 194 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-3], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2499 "lang11d_tab.cpp"
    break;

  case 59: /* msgsend: name '(' arglist1 optkeyarglist ')' blocklist  */
#line 198 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2508 "lang11d_tab.cpp"
    break;

  case 60: /* msgsend: '(' binop2 ')' '(' ')' blocklist1  */
#line 203 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-4], NULL, NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2516 "lang11d_tab.cpp"
    break;

  case 61: /* msgsend: '(' binop2 ')' '(' arglist1 optkeyarglist ')' blocklist  */
#line 207 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-3],
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2525 "lang11d_tab.cpp"
    break;

  case 62: /* msgsend: name '(' argListWithPerformList optkeyarglist ')'  */
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
#line 2547 "lang11d_tab.cpp"
    break;

  case 63: /* msgsend: '(' binop2 ')' '(' argListWithPerformList optkeyarglist ')'  */
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
#line 2564 "lang11d_tab.cpp"
    break;

  case 64: /* msgsend: classname '[' arrayelems ']'  */
#line 243 "lang11d"
                        { yyval = (intptr_t)newPyrDynListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 2570 "lang11d_tab.cpp"
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
#line 2585 "lang11d_tab.cpp"
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
#line 2600 "lang11d_tab.cpp"
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
#line 2615 "lang11d_tab.cpp"
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
#line 2632 "lang11d_tab.cpp"
    break;

  case 69: /* msgsend: classname '(' argListWithPerformList optkeyarglist ')'  */
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
#line 2657 "lang11d_tab.cpp"
    break;

  case 70: /* msgsend: expr '.' '(' ')' blocklist  */
#line 315 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				yyval = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)yyvsp[-4], NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2670 "lang11d_tab.cpp"
    break;

  case 71: /* msgsend: expr '.' '(' keyarglist1 optcomma ')' blocklist  */
#line 324 "lang11d"
                        {
				PyrSlotNode *selectornode;
				PyrSlot slot;

				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				yyval = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2683 "lang11d_tab.cpp"
    break;

  case 72: /* msgsend: expr '.' name '(' keyarglist1 optcomma ')' blocklist  */
#line 333 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-7],
					(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]);
			}
#line 2692 "lang11d_tab.cpp"
    break;

  case 73: /* msgsend: expr '.' '(' arglist1 optkeyarglist ')' blocklist  */
#line 338 "lang11d"
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
#line 2709 "lang11d_tab.cpp"
    break;

  case 74: /* msgsend: expr '.' '(' argListWithPerformList optkeyarglist ')'  */
#line 352 "lang11d"
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
#line 2733 "lang11d_tab.cpp"
    break;

  case 75: /* msgsend: expr '.' name '(' KWEXPAND expr ')'  */
#line 375 "lang11d"
                        {
				PyrSlot selectorSlot;
				if (isSuperObjNode((PyrParseNode*)yyvsp[-6])) {
					SetRaw(&((PyrPushNameNode*)yyvsp[-6])->mSlot, s_this);
					SetSymbol(&selectorSlot, s_superPerformArgs);
				} else {
					SetSymbol(&selectorSlot, s_performArgs);
				}
				PyrSlotNode* selectornode = newPyrSlotNode(&selectorSlot);

				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-6], newPyrPushLitNode((PyrSlotNode*)yyvsp[-4], NULL));
				PyrSlot nil_slot;
				SetNil(&nil_slot);
				PyrSlotNode* nil_slot_node = newPyrSlotNode(&nil_slot);
				args = linkNextNode(args, newPyrPushLitNode(nil_slot_node, NULL));
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, NULL, 0);
			}
#line 2757 "lang11d_tab.cpp"
    break;

  case 76: /* msgsend: expr '.' '(' KWEXPAND expr ')'  */
#line 396 "lang11d"
                        {
				PyrSlot selectorSlot;
				if (isSuperObjNode((PyrParseNode*)yyvsp[-5])) {
					SetRaw(&((PyrPushNameNode*)yyvsp[-5])->mSlot, s_this);
					SetSymbol(&selectorSlot, s_superPerformArgs);
				} else {
					SetSymbol(&selectorSlot, s_performArgs);
				}
				PyrSlotNode* selectornode = newPyrSlotNode(&selectorSlot);

				PyrParseNode* args;

				PyrSlot value_slot;
				SetSymbol(&value_slot, s_value);
				PyrSlotNode* value_slot_node = newPyrSlotNode(&value_slot);
				args = linkNextNode((PyrParseNode*)yyvsp[-5], newPyrPushLitNode(value_slot_node, NULL));

				PyrSlot nil_slot;
				SetNil(&nil_slot);
				PyrSlotNode* nil_slot_node = newPyrSlotNode(&nil_slot);
				args = linkNextNode(args, newPyrPushLitNode(nil_slot_node, NULL));

				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, NULL, 0);
			}
#line 2787 "lang11d_tab.cpp"
    break;

  case 77: /* msgsend: classname '(' KWEXPAND expr ')'  */
#line 422 "lang11d"
                        { 
				PyrSlot selectorSlot;
				if (isSuperObjNode((PyrParseNode*)yyvsp[-4])) {
					SetRaw(&((PyrPushNameNode*)yyvsp[-4])->mSlot, s_this);
					SetSymbol(&selectorSlot, s_superPerformArgs);
				} else {
					SetSymbol(&selectorSlot, s_performArgs);
				}
				PyrSlotNode* selectornode = newPyrSlotNode(&selectorSlot);

				PyrSlot value_slot;
				SetSymbol(&value_slot, s_value);
				PyrSlotNode* value_slot_node = newPyrSlotNode(&value_slot);

				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-4], newPyrPushLitNode(value_slot_node, NULL));
				PyrSlot nil_slot;
				SetNil(&nil_slot);
				PyrSlotNode* nil_slot_node = newPyrSlotNode(&nil_slot);
				args = linkNextNode(args, newPyrPushLitNode(nil_slot_node, NULL));
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, NULL, 0);
			}
#line 2815 "lang11d_tab.cpp"
    break;

  case 78: /* msgsend: name '(' KWEXPAND expr ')'  */
#line 446 "lang11d"
                        { 
				PyrSlot selectorSlot;
				if (isSuperObjNode((PyrParseNode*)yyvsp[-4])) {
					SetRaw(&((PyrPushNameNode*)yyvsp[-4])->mSlot, s_this);
					SetSymbol(&selectorSlot, s_superPerformArgs);
				} else {
					SetSymbol(&selectorSlot, s_performArgs);
				}
				PyrSlotNode* selectornode = newPyrSlotNode(&selectorSlot);

				PyrSlot value_slot;
				SetSymbol(&value_slot, s_value);
				PyrSlotNode* value_slot_node = newPyrSlotNode(&value_slot);

				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-4], newPyrPushLitNode(value_slot_node, NULL));
				PyrSlot nil_slot;
				SetNil(&nil_slot);
				PyrSlotNode* nil_slot_node = newPyrSlotNode(&nil_slot);
				args = linkNextNode(args, newPyrPushLitNode(nil_slot_node, NULL));
				args = linkNextNode(args, (PyrParseNode*)yyvsp[-1]);
				yyval = (intptr_t)newPyrCallNode(selectornode, args, NULL, 0);
			}
#line 2843 "lang11d_tab.cpp"
    break;

  case 79: /* msgsend: expr '.' name '(' ')' blocklist  */
#line 472 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-5], NULL, (PyrParseNode*)yyvsp[0]);
			}
#line 2851 "lang11d_tab.cpp"
    break;

  case 80: /* msgsend: expr '.' name '(' arglist1 optkeyarglist ')' blocklist  */
#line 476 "lang11d"
                        {
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-3]);
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-5], args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 2861 "lang11d_tab.cpp"
    break;

  case 81: /* msgsend: expr '.' name '(' argListWithPerformList optkeyarglist ')'  */
#line 482 "lang11d"
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
#line 2883 "lang11d_tab.cpp"
    break;

  case 82: /* msgsend: expr '.' name blocklist  */
#line 500 "lang11d"
                        {
				yyval = (intptr_t)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[-3], 0, (PyrParseNode*)yyvsp[0]);
			}
#line 2891 "lang11d_tab.cpp"
    break;

  case 83: /* $@1: %empty  */
#line 505 "lang11d"
                            { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 1); }
#line 2897 "lang11d_tab.cpp"
    break;

  case 84: /* generator: '{' ':' exprseq $@1 ',' qual '}'  */
#line 506 "lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, getsym("r"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);

				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)yyvsp[-1], false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				yyval = (intptr_t)newPyrCallNode(selectornode, (PyrParseNode*)blocklit, 0, 0);
			}
#line 2911 "lang11d_tab.cpp"
    break;

  case 85: /* $@2: %empty  */
#line 515 "lang11d"
                                  { pushls(&generatorStack, yyvsp[0]); pushls(&generatorStack, 2); }
#line 2917 "lang11d_tab.cpp"
    break;

  case 86: /* generator: '{' ';' exprseq $@2 ',' qual '}'  */
#line 516 "lang11d"
                        {
				yyval = yyvsp[-1];
			}
#line 2925 "lang11d_tab.cpp"
    break;

  case 87: /* nextqual: %empty  */
#line 522 "lang11d"
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
#line 2951 "lang11d_tab.cpp"
    break;

  case 88: /* nextqual: ',' qual  */
#line 544 "lang11d"
                                { yyval = yyvsp[0]; }
#line 2957 "lang11d_tab.cpp"
    break;

  case 89: /* qual: name LEFTARROW exprseq nextqual  */
#line 548 "lang11d"
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
#line 2995 "lang11d_tab.cpp"
    break;

  case 90: /* qual: name name LEFTARROW exprseq nextqual  */
#line 582 "lang11d"
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
#line 3037 "lang11d_tab.cpp"
    break;

  case 91: /* qual: VAR name '=' exprseq nextqual  */
#line 620 "lang11d"
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
#line 3055 "lang11d_tab.cpp"
    break;

  case 92: /* qual: exprseq nextqual  */
#line 634 "lang11d"
                        {
				PyrSlot slot;
				SetSymbol(&slot, getsym("if"));
				PyrSlotNode* selectornode = newPyrSlotNode(&slot);
				PyrParseNode *block = (PyrParseNode*)newPyrBlockNode(0, 0, (PyrParseNode*)yyvsp[0], false);
				PyrParseNode *blocklit = (PyrParseNode*)newPyrPushLitNode(NULL, block);
				PyrParseNode* args2 = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-1], blocklit);

				yyval = (intptr_t)newPyrCallNode(selectornode, args2, 0, 0);
			}
#line 3070 "lang11d_tab.cpp"
    break;

  case 93: /* qual: ':' ':' exprseq nextqual  */
#line 645 "lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]);
			}
#line 3078 "lang11d_tab.cpp"
    break;

  case 94: /* qual: ':' WHILE exprseq nextqual  */
#line 649 "lang11d"
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
#line 3105 "lang11d_tab.cpp"
    break;

  case 101: /* expr1: '(' exprseq ')'  */
#line 680 "lang11d"
                        {
				PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
				node->mParens = 1;
				yyval = yyvsp[-1];
			}
#line 3115 "lang11d_tab.cpp"
    break;

  case 102: /* expr1: '~' name  */
#line 686 "lang11d"
                        {
				PyrParseNode* argnode;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL);
				SetSymbol(&slot, s_envirGet);
				selectornode = newPyrSlotNode(&slot);
				yyval = (intptr_t)newPyrCallNode(selectornode, argnode, 0, 0);
			}
#line 3129 "lang11d_tab.cpp"
    break;

  case 103: /* expr1: '[' arrayelems ']'  */
#line 696 "lang11d"
                        { yyval = (intptr_t)newPyrDynListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 3135 "lang11d_tab.cpp"
    break;

  case 104: /* expr1: '(' valrange2 ')'  */
#line 698 "lang11d"
                        { yyval = yyvsp[-1]; }
#line 3141 "lang11d_tab.cpp"
    break;

  case 105: /* expr1: '(' ':' valrange3 ')'  */
#line 700 "lang11d"
                        { yyval = yyvsp[-1]; }
#line 3147 "lang11d_tab.cpp"
    break;

  case 106: /* expr1: '(' dictslotlist ')'  */
#line 702 "lang11d"
                        { yyval = (intptr_t)newPyrDynDictNode((PyrParseNode*)yyvsp[-1]); }
#line 3153 "lang11d_tab.cpp"
    break;

  case 107: /* expr1: pseudovar  */
#line 704 "lang11d"
                        { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3159 "lang11d_tab.cpp"
    break;

  case 108: /* expr1: expr1 '[' arglist1 ']'  */
#line 706 "lang11d"
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
#line 3176 "lang11d_tab.cpp"
    break;

  case 110: /* valrangex1: expr1 '[' arglist1 DOTDOT ']'  */
#line 722 "lang11d"
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
#line 3207 "lang11d_tab.cpp"
    break;

  case 111: /* valrangex1: expr1 '[' DOTDOT exprseq ']'  */
#line 749 "lang11d"
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
#line 3229 "lang11d_tab.cpp"
    break;

  case 112: /* valrangex1: expr1 '[' arglist1 DOTDOT exprseq ']'  */
#line 767 "lang11d"
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
#line 3258 "lang11d_tab.cpp"
    break;

  case 113: /* valrangeassign: expr1 '[' arglist1 DOTDOT ']' '=' expr  */
#line 794 "lang11d"
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
#line 3290 "lang11d_tab.cpp"
    break;

  case 114: /* valrangeassign: expr1 '[' DOTDOT exprseq ']' '=' expr  */
#line 822 "lang11d"
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
#line 3313 "lang11d_tab.cpp"
    break;

  case 115: /* valrangeassign: expr1 '[' arglist1 DOTDOT exprseq ']' '=' expr  */
#line 841 "lang11d"
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
#line 3343 "lang11d_tab.cpp"
    break;

  case 116: /* valrangexd: expr '.' '[' arglist1 DOTDOT ']'  */
#line 869 "lang11d"
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
#line 3374 "lang11d_tab.cpp"
    break;

  case 117: /* valrangexd: expr '.' '[' DOTDOT exprseq ']'  */
#line 896 "lang11d"
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
#line 3396 "lang11d_tab.cpp"
    break;

  case 118: /* valrangexd: expr '.' '[' arglist1 DOTDOT exprseq ']'  */
#line 914 "lang11d"
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
#line 3425 "lang11d_tab.cpp"
    break;

  case 119: /* valrangexd: expr '.' '[' arglist1 DOTDOT ']' '=' expr  */
#line 939 "lang11d"
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
#line 3457 "lang11d_tab.cpp"
    break;

  case 120: /* valrangexd: expr '.' '[' DOTDOT exprseq ']' '=' expr  */
#line 967 "lang11d"
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
#line 3480 "lang11d_tab.cpp"
    break;

  case 121: /* valrangexd: expr '.' '[' arglist1 DOTDOT exprseq ']' '=' expr  */
#line 986 "lang11d"
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
#line 3510 "lang11d_tab.cpp"
    break;

  case 122: /* valrange2: exprseq DOTDOT  */
#line 1014 "lang11d"
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
#line 3532 "lang11d_tab.cpp"
    break;

  case 123: /* valrange2: DOTDOT exprseq  */
#line 1033 "lang11d"
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
#line 3554 "lang11d_tab.cpp"
    break;

  case 124: /* valrange2: exprseq DOTDOT exprseq  */
#line 1052 "lang11d"
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
#line 3574 "lang11d_tab.cpp"
    break;

  case 125: /* valrange2: exprseq ',' exprseq DOTDOT exprseq  */
#line 1069 "lang11d"
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
#line 3592 "lang11d_tab.cpp"
    break;

  case 126: /* valrange2: exprseq ',' exprseq DOTDOT  */
#line 1083 "lang11d"
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
#line 3615 "lang11d_tab.cpp"
    break;

  case 127: /* valrange3: DOTDOT exprseq  */
#line 1104 "lang11d"
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
#line 3637 "lang11d_tab.cpp"
    break;

  case 128: /* valrange3: exprseq DOTDOT  */
#line 1123 "lang11d"
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
#line 3658 "lang11d_tab.cpp"
    break;

  case 129: /* valrange3: exprseq DOTDOT exprseq  */
#line 1141 "lang11d"
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
#line 3678 "lang11d_tab.cpp"
    break;

  case 130: /* valrange3: exprseq ',' exprseq DOTDOT  */
#line 1158 "lang11d"
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
#line 3698 "lang11d_tab.cpp"
    break;

  case 131: /* valrange3: exprseq ',' exprseq DOTDOT exprseq  */
#line 1174 "lang11d"
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
#line 3716 "lang11d_tab.cpp"
    break;

  case 135: /* expr: classname  */
#line 1192 "lang11d"
                            { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 3722 "lang11d_tab.cpp"
    break;

  case 136: /* expr: expr '.' '[' arglist1 ']'  */
#line 1194 "lang11d"
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
#line 3739 "lang11d_tab.cpp"
    break;

  case 137: /* expr: '`' expr  */
#line 1207 "lang11d"
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
#line 3757 "lang11d_tab.cpp"
    break;

  case 138: /* expr: expr binop2 adverb expr  */
#line 1221 "lang11d"
                        {
				yyval = (intptr_t)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0], (PyrParseNode*)yyvsp[-1]);
			}
#line 3766 "lang11d_tab.cpp"
    break;

  case 139: /* expr: name '=' expr  */
#line 1226 "lang11d"
                        {
				yyval = (intptr_t)newPyrAssignNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0);
			}
#line 3774 "lang11d_tab.cpp"
    break;

  case 140: /* expr: '~' name '=' expr  */
#line 1230 "lang11d"
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
#line 3789 "lang11d_tab.cpp"
    break;

  case 141: /* expr: expr '.' name '=' expr  */
#line 1241 "lang11d"
                        {
				yyval = (intptr_t)newPyrSetterNode((PyrSlotNode*)yyvsp[-2],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3798 "lang11d_tab.cpp"
    break;

  case 142: /* expr: name '(' arglist1 optkeyarglist ')' '=' expr  */
#line 1246 "lang11d"
                        {
				if (yyvsp[-3] != 0) {
					error("Setter method called with keyword arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-3]);
					compileErrors++;
				}
				yyval = (intptr_t)newPyrSetterNode((PyrSlotNode*)yyvsp[-6],
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]);
			}
#line 3812 "lang11d_tab.cpp"
    break;

  case 143: /* expr: '#' mavars '=' expr  */
#line 1256 "lang11d"
                        {
				yyval = (intptr_t)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[0], 0);
			}
#line 3821 "lang11d_tab.cpp"
    break;

  case 144: /* expr: expr1 '[' arglist1 ']' '=' expr  */
#line 1261 "lang11d"
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
#line 3839 "lang11d_tab.cpp"
    break;

  case 145: /* expr: expr '.' '[' arglist1 ']' '=' expr  */
#line 1275 "lang11d"
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
#line 3857 "lang11d_tab.cpp"
    break;

  case 146: /* adverb: %empty  */
#line 1290 "lang11d"
          { yyval = 0; }
#line 3863 "lang11d_tab.cpp"
    break;

  case 147: /* adverb: '.' name  */
#line 1291 "lang11d"
                           { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3869 "lang11d_tab.cpp"
    break;

  case 148: /* adverb: '.' integer  */
#line 1292 "lang11d"
                              { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3875 "lang11d_tab.cpp"
    break;

  case 149: /* adverb: '.' '(' exprseq ')'  */
#line 1293 "lang11d"
                                      { yyval = yyvsp[-1]; }
#line 3881 "lang11d_tab.cpp"
    break;

  case 151: /* exprn: exprn ';' expr  */
#line 1298 "lang11d"
                        {
				yyval = (intptr_t)newPyrDropNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
			}
#line 3889 "lang11d_tab.cpp"
    break;

  case 153: /* arrayelems: %empty  */
#line 1306 "lang11d"
                  { yyval = 0; }
#line 3895 "lang11d_tab.cpp"
    break;

  case 154: /* arrayelems: arrayelems1 optcomma  */
#line 1308 "lang11d"
                          { yyval = yyvsp[-1]; }
#line 3901 "lang11d_tab.cpp"
    break;

  case 156: /* arrayelems1: exprseq ':' exprseq  */
#line 1313 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3907 "lang11d_tab.cpp"
    break;

  case 157: /* arrayelems1: keybinop exprseq  */
#line 1315 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 3916 "lang11d_tab.cpp"
    break;

  case 158: /* arrayelems1: arrayelems1 ',' exprseq  */
#line 1320 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3922 "lang11d_tab.cpp"
    break;

  case 159: /* arrayelems1: arrayelems1 ',' keybinop exprseq  */
#line 1322 "lang11d"
                                {
					PyrParseNode* elems;
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					elems = (PyrParseNode*)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], elems);
				}
#line 3933 "lang11d_tab.cpp"
    break;

  case 160: /* arrayelems1: arrayelems1 ',' exprseq ':' exprseq  */
#line 1329 "lang11d"
                                {
					PyrParseNode* elems;
					elems = (PyrParseNode*)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]);
					yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-4], elems);
				}
#line 3943 "lang11d_tab.cpp"
    break;

  case 162: /* arglist1: arglist1 ',' exprseq  */
#line 1338 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3949 "lang11d_tab.cpp"
    break;

  case 163: /* argListWithPerformList: '*' exprseq  */
#line 1342 "lang11d"
                                { yyval = yyvsp[0]; }
#line 3955 "lang11d_tab.cpp"
    break;

  case 164: /* argListWithPerformList: arglist1 ',' '*' exprseq  */
#line 1344 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[0]); }
#line 3961 "lang11d_tab.cpp"
    break;

  case 166: /* keyarglist1: keyarglist1 ',' keyarg  */
#line 1349 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 3967 "lang11d_tab.cpp"
    break;

  case 167: /* keyarg: keybinop exprseq  */
#line 1353 "lang11d"
                                { yyval = (intptr_t)newPyrPushKeyArgNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 3973 "lang11d_tab.cpp"
    break;

  case 168: /* optkeyarglist: optcomma  */
#line 1356 "lang11d"
                           { yyval = 0; }
#line 3979 "lang11d_tab.cpp"
    break;

  case 169: /* optkeyarglist: ',' keyarglist1 optcomma  */
#line 1357 "lang11d"
                                                           { yyval = yyvsp[-1]; }
#line 3985 "lang11d_tab.cpp"
    break;

  case 170: /* mavars: mavarlist  */
#line 1361 "lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 3991 "lang11d_tab.cpp"
    break;

  case 171: /* mavars: mavarlist ELLIPSIS name  */
#line 1363 "lang11d"
                        { yyval = (intptr_t)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); }
#line 3997 "lang11d_tab.cpp"
    break;

  case 173: /* mavarlist: mavarlist ',' name  */
#line 1368 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4003 "lang11d_tab.cpp"
    break;

  case 174: /* slotliteral: integer  */
#line 1372 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4009 "lang11d_tab.cpp"
    break;

  case 175: /* slotliteral: floatp  */
#line 1373 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4015 "lang11d_tab.cpp"
    break;

  case 176: /* slotliteral: ascii  */
#line 1374 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4021 "lang11d_tab.cpp"
    break;

  case 177: /* slotliteral: string  */
#line 1375 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4027 "lang11d_tab.cpp"
    break;

  case 178: /* slotliteral: symbol  */
#line 1376 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4033 "lang11d_tab.cpp"
    break;

  case 179: /* slotliteral: trueobj  */
#line 1377 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4039 "lang11d_tab.cpp"
    break;

  case 180: /* slotliteral: falseobj  */
#line 1378 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4045 "lang11d_tab.cpp"
    break;

  case 181: /* slotliteral: nilobj  */
#line 1379 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4051 "lang11d_tab.cpp"
    break;

  case 182: /* slotliteral: listlit  */
#line 1380 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 4057 "lang11d_tab.cpp"
    break;

  case 183: /* blockliteral: block  */
#line 1383 "lang11d"
                        { yyval = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 4063 "lang11d_tab.cpp"
    break;

  case 184: /* pushname: name  */
#line 1386 "lang11d"
                                { yyval = (intptr_t)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); }
#line 4069 "lang11d_tab.cpp"
    break;

  case 185: /* pushliteral: integer  */
#line 1389 "lang11d"
                                { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4075 "lang11d_tab.cpp"
    break;

  case 186: /* pushliteral: floatp  */
#line 1390 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4081 "lang11d_tab.cpp"
    break;

  case 187: /* pushliteral: ascii  */
#line 1391 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4087 "lang11d_tab.cpp"
    break;

  case 188: /* pushliteral: string  */
#line 1392 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4093 "lang11d_tab.cpp"
    break;

  case 189: /* pushliteral: symbol  */
#line 1393 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4099 "lang11d_tab.cpp"
    break;

  case 190: /* pushliteral: trueobj  */
#line 1394 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4105 "lang11d_tab.cpp"
    break;

  case 191: /* pushliteral: falseobj  */
#line 1395 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4111 "lang11d_tab.cpp"
    break;

  case 192: /* pushliteral: nilobj  */
#line 1396 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4117 "lang11d_tab.cpp"
    break;

  case 193: /* pushliteral: listlit  */
#line 1397 "lang11d"
                                        { yyval = (intptr_t)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 4123 "lang11d_tab.cpp"
    break;

  case 194: /* listliteral: integer  */
#line 1400 "lang11d"
                                { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4129 "lang11d_tab.cpp"
    break;

  case 195: /* listliteral: floatp  */
#line 1401 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4135 "lang11d_tab.cpp"
    break;

  case 196: /* listliteral: ascii  */
#line 1402 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4141 "lang11d_tab.cpp"
    break;

  case 197: /* listliteral: string  */
#line 1403 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4147 "lang11d_tab.cpp"
    break;

  case 198: /* listliteral: symbol  */
#line 1404 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4153 "lang11d_tab.cpp"
    break;

  case 199: /* listliteral: name  */
#line 1405 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4159 "lang11d_tab.cpp"
    break;

  case 200: /* listliteral: trueobj  */
#line 1406 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4165 "lang11d_tab.cpp"
    break;

  case 201: /* listliteral: falseobj  */
#line 1407 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4171 "lang11d_tab.cpp"
    break;

  case 202: /* listliteral: nilobj  */
#line 1408 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); }
#line 4177 "lang11d_tab.cpp"
    break;

  case 203: /* listliteral: listlit2  */
#line 1409 "lang11d"
                                        { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 4183 "lang11d_tab.cpp"
    break;

  case 204: /* listliteral: dictlit2  */
#line 1410 "lang11d"
                                    { yyval = (intptr_t)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); }
#line 4189 "lang11d_tab.cpp"
    break;

  case 205: /* block: '{' argdecls funcvardecls funcbody '}'  */
#line 1414 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], false); }
#line 4196 "lang11d_tab.cpp"
    break;

  case 206: /* block: BEGINCLOSEDFUNC argdecls funcvardecls funcbody '}'  */
#line 1417 "lang11d"
                                { yyval = (intptr_t)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2],
					(PyrParseNode*)yyvsp[-1], true); }
#line 4203 "lang11d_tab.cpp"
    break;

  case 207: /* funcvardecls: %empty  */
#line 1421 "lang11d"
                  { yyval = 0; }
#line 4209 "lang11d_tab.cpp"
    break;

  case 208: /* funcvardecls: funcvardecls funcvardecl  */
#line 1423 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 4215 "lang11d_tab.cpp"
    break;

  case 210: /* funcvardecls1: funcvardecls1 funcvardecl  */
#line 1428 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); }
#line 4221 "lang11d_tab.cpp"
    break;

  case 211: /* funcvardecl: VAR vardeflist ';'  */
#line 1432 "lang11d"
                                { yyval = (intptr_t)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varLocal); }
#line 4227 "lang11d_tab.cpp"
    break;

  case 212: /* argdecls: %empty  */
#line 1435 "lang11d"
                  { yyval = 0; }
#line 4233 "lang11d_tab.cpp"
    break;

  case 213: /* argdecls: ARG vardeflist ';'  */
#line 1437 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4241 "lang11d_tab.cpp"
    break;

  case 214: /* argdecls: ARG vardeflist0 ELLIPSIS name ';'  */
#line 1441 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4249 "lang11d_tab.cpp"
    break;

  case 215: /* argdecls: '|' slotdeflist '|'  */
#line 1445 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4257 "lang11d_tab.cpp"
    break;

  case 216: /* argdecls: '|' slotdeflist0 ELLIPSIS name '|'  */
#line 1449 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4265 "lang11d_tab.cpp"
    break;

  case 217: /* argdecls: '|' slotdeflist0 ELLIPSIS name ',' name '|'  */
#line 1453 "lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 4273 "lang11d_tab.cpp"
    break;

  case 218: /* argdecls1: ARG vardeflist ';'  */
#line 1459 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4281 "lang11d_tab.cpp"
    break;

  case 219: /* argdecls1: ARG vardeflist0 ELLIPSIS name ';'  */
#line 1463 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4289 "lang11d_tab.cpp"
    break;

  case 220: /* argdecls1: '|' slotdeflist '|'  */
#line 1467 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL, NULL);
				}
#line 4297 "lang11d_tab.cpp"
    break;

  case 221: /* argdecls1: '|' slotdeflist0 ELLIPSIS name '|'  */
#line 1471 "lang11d"
                                {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1], NULL);
				}
#line 4305 "lang11d_tab.cpp"
    break;

  case 222: /* argdecls1: '|' slotdeflist0 ELLIPSIS name ',' name '|'  */
#line 1475 "lang11d"
                            {
					yyval = (intptr_t)newPyrArgListNode((PyrVarDefNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]);
			    }
#line 4313 "lang11d_tab.cpp"
    break;

  case 224: /* constdeflist: constdeflist optcomma constdef  */
#line 1483 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4319 "lang11d_tab.cpp"
    break;

  case 225: /* constdef: rspec name '=' slotliteral  */
#line 1487 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4325 "lang11d_tab.cpp"
    break;

  case 226: /* slotdeflist0: %empty  */
#line 1490 "lang11d"
                  { yyval = 0; }
#line 4331 "lang11d_tab.cpp"
    break;

  case 229: /* slotdeflist: slotdeflist optcomma slotdef  */
#line 1496 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4337 "lang11d_tab.cpp"
    break;

  case 230: /* slotdef: name  */
#line 1500 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4343 "lang11d_tab.cpp"
    break;

  case 231: /* slotdef: name optequal slotliteral  */
#line 1502 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4349 "lang11d_tab.cpp"
    break;

  case 232: /* slotdef: name optequal '(' exprseq ')'  */
#line 1504 "lang11d"
                                {
					PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
					node->mParens = 1;
					yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-4], node, 0);
				}
#line 4359 "lang11d_tab.cpp"
    break;

  case 233: /* vardeflist0: %empty  */
#line 1511 "lang11d"
                  { yyval = 0; }
#line 4365 "lang11d_tab.cpp"
    break;

  case 236: /* vardeflist: vardeflist ',' vardef  */
#line 1517 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4371 "lang11d_tab.cpp"
    break;

  case 237: /* vardef: name  */
#line 1521 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); }
#line 4377 "lang11d_tab.cpp"
    break;

  case 238: /* vardef: name '=' expr  */
#line 1523 "lang11d"
                                { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); }
#line 4383 "lang11d_tab.cpp"
    break;

  case 239: /* vardef: name '(' exprseq ')'  */
#line 1525 "lang11d"
                                {
									PyrParseNode* node = (PyrParseNode*)yyvsp[-1];
									node->mParens = 1;
									yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-3], node, 0);
								}
#line 4393 "lang11d_tab.cpp"
    break;

  case 240: /* dictslotdef: exprseq ':' exprseq  */
#line 1533 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4399 "lang11d_tab.cpp"
    break;

  case 241: /* dictslotdef: keybinop exprseq  */
#line 1535 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 4408 "lang11d_tab.cpp"
    break;

  case 243: /* dictslotlist1: dictslotlist1 ',' dictslotdef  */
#line 1543 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4414 "lang11d_tab.cpp"
    break;

  case 244: /* dictslotlist: %empty  */
#line 1546 "lang11d"
                  { yyval = 0; }
#line 4420 "lang11d_tab.cpp"
    break;

  case 247: /* rwslotdeflist: rwslotdeflist ',' rwslotdef  */
#line 1552 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4426 "lang11d_tab.cpp"
    break;

  case 248: /* rwslotdef: rwspec name  */
#line 1556 "lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); }
#line 4432 "lang11d_tab.cpp"
    break;

  case 249: /* rwslotdef: rwspec name '=' slotliteral  */
#line 1558 "lang11d"
                                        { yyval = (intptr_t)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], yyvsp[-3]); }
#line 4438 "lang11d_tab.cpp"
    break;

  case 250: /* dictlit2: '(' litdictslotlist ')'  */
#line 1562 "lang11d"
                                { yyval = (intptr_t)newPyrLitDictNode((PyrParseNode*)yyvsp[-1]); }
#line 4444 "lang11d_tab.cpp"
    break;

  case 251: /* litdictslotdef: listliteral ':' listliteral  */
#line 1566 "lang11d"
                                { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4450 "lang11d_tab.cpp"
    break;

  case 252: /* litdictslotdef: keybinop listliteral  */
#line 1568 "lang11d"
                                {
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (intptr_t)linkNextNode(key, (PyrParseNode*)yyvsp[0]);
				}
#line 4459 "lang11d_tab.cpp"
    break;

  case 254: /* litdictslotlist1: litdictslotlist1 ',' litdictslotdef  */
#line 1576 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4465 "lang11d_tab.cpp"
    break;

  case 255: /* litdictslotlist: %empty  */
#line 1579 "lang11d"
                  { yyval = 0; }
#line 4471 "lang11d_tab.cpp"
    break;

  case 257: /* listlit: '#' '[' literallistc ']'  */
#line 1586 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 4477 "lang11d_tab.cpp"
    break;

  case 258: /* listlit: '#' classname '[' literallistc ']'  */
#line 1588 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4483 "lang11d_tab.cpp"
    break;

  case 259: /* listlit2: '[' literallistc ']'  */
#line 1592 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); }
#line 4489 "lang11d_tab.cpp"
    break;

  case 260: /* listlit2: classname '[' literallistc ']'  */
#line 1594 "lang11d"
                                { yyval = (intptr_t)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); }
#line 4495 "lang11d_tab.cpp"
    break;

  case 261: /* literallistc: %empty  */
#line 1597 "lang11d"
                  { yyval = 0; }
#line 4501 "lang11d_tab.cpp"
    break;

  case 264: /* literallist1: literallist1 ',' listliteral  */
#line 1603 "lang11d"
                                        { yyval = (intptr_t)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); }
#line 4507 "lang11d_tab.cpp"
    break;

  case 265: /* rwspec: %empty  */
#line 1606 "lang11d"
           { yyval = rwPrivate; }
#line 4513 "lang11d_tab.cpp"
    break;

  case 266: /* rwspec: '<'  */
#line 1608 "lang11d"
                        { yyval = rwReadOnly; }
#line 4519 "lang11d_tab.cpp"
    break;

  case 267: /* rwspec: READWRITEVAR  */
#line 1610 "lang11d"
                        { yyval = rwReadWrite; }
#line 4525 "lang11d_tab.cpp"
    break;

  case 268: /* rwspec: '>'  */
#line 1612 "lang11d"
                        { yyval = rwWriteOnly; }
#line 4531 "lang11d_tab.cpp"
    break;

  case 269: /* rspec: %empty  */
#line 1615 "lang11d"
           { yyval = rwPrivate; }
#line 4537 "lang11d_tab.cpp"
    break;

  case 270: /* rspec: '<'  */
#line 1617 "lang11d"
                        { yyval = rwReadOnly; }
#line 4543 "lang11d_tab.cpp"
    break;

  case 271: /* integer: INTEGER  */
#line 1620 "lang11d"
                  { yyval = zzval; }
#line 4549 "lang11d_tab.cpp"
    break;

  case 272: /* integer: '-' INTEGER  */
#line 1622 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawInt(&node->mSlot));
				yyval = zzval;
			}
#line 4560 "lang11d_tab.cpp"
    break;

  case 273: /* floatr: SC_FLOAT  */
#line 1630 "lang11d"
                   { yyval = zzval; }
#line 4566 "lang11d_tab.cpp"
    break;

  case 274: /* floatr: '-' SC_FLOAT  */
#line 1632 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetRaw(&node->mSlot, -slotRawFloat(&node->mSlot));
				yyval = zzval;
			}
#line 4577 "lang11d_tab.cpp"
    break;

  case 275: /* accidental: ACCIDENTAL  */
#line 1640 "lang11d"
                        { yyval = zzval; }
#line 4583 "lang11d_tab.cpp"
    break;

  case 276: /* accidental: '-' ACCIDENTAL  */
#line 1642 "lang11d"
                                {
					PyrSlotNode *node;
					double intval, fracval;
					node = (PyrSlotNode*)zzval;
					intval = floor(slotRawFloat(&node->mSlot) + 0.5);
					fracval = slotRawFloat(&node->mSlot) - intval;
					SetRaw(&node->mSlot, -intval + fracval);
					yyval = zzval;
				}
#line 4597 "lang11d_tab.cpp"
    break;

  case 277: /* pie: PIE  */
#line 1652 "lang11d"
                      { yyval = zzval; }
#line 4603 "lang11d_tab.cpp"
    break;

  case 280: /* floatp: floatr pie  */
#line 1658 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)yyvsp[-1];
				SetRaw(&node->mSlot, slotRawFloat(&node->mSlot) * pi);
			}
#line 4613 "lang11d_tab.cpp"
    break;

  case 281: /* floatp: integer pie  */
#line 1664 "lang11d"
                        {
				PyrSlotNode *node;
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = slotRawInt(&node->mSlot);
				SetFloat(&node->mSlot, ival * pi);
			}
#line 4625 "lang11d_tab.cpp"
    break;

  case 282: /* floatp: pie  */
#line 1672 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, pi);
				yyval = zzval;
			}
#line 4636 "lang11d_tab.cpp"
    break;

  case 283: /* floatp: '-' pie  */
#line 1679 "lang11d"
                        {
				PyrSlotNode *node;
				node = (PyrSlotNode*)zzval;
				SetFloat(&node->mSlot, -pi);
				yyval = zzval;
			}
#line 4647 "lang11d_tab.cpp"
    break;

  case 284: /* name: NAME  */
#line 1687 "lang11d"
                       { yyval = zzval; }
#line 4653 "lang11d_tab.cpp"
    break;

  case 285: /* name: WHILE  */
#line 1688 "lang11d"
                                { yyval = zzval; }
#line 4659 "lang11d_tab.cpp"
    break;

  case 286: /* classname: CLASSNAME  */
#line 1691 "lang11d"
                                    { yyval = zzval; }
#line 4665 "lang11d_tab.cpp"
    break;

  case 287: /* primname: PRIMITIVENAME  */
#line 1694 "lang11d"
                                        { yyval = zzval; }
#line 4671 "lang11d_tab.cpp"
    break;

  case 288: /* trueobj: TRUEOBJ  */
#line 1697 "lang11d"
                          { yyval = zzval; }
#line 4677 "lang11d_tab.cpp"
    break;

  case 289: /* falseobj: FALSEOBJ  */
#line 1700 "lang11d"
                           { yyval = zzval; }
#line 4683 "lang11d_tab.cpp"
    break;

  case 290: /* nilobj: NILOBJ  */
#line 1703 "lang11d"
                         { yyval = zzval; }
#line 4689 "lang11d_tab.cpp"
    break;

  case 291: /* ascii: ASCII  */
#line 1706 "lang11d"
                        { yyval = zzval; }
#line 4695 "lang11d_tab.cpp"
    break;

  case 292: /* symbol: SYMBOL  */
#line 1709 "lang11d"
                         { yyval = zzval; }
#line 4701 "lang11d_tab.cpp"
    break;

  case 293: /* string: STRING  */
#line 1712 "lang11d"
                         { yyval = zzval; }
#line 4707 "lang11d_tab.cpp"
    break;

  case 294: /* pseudovar: PSEUDOVAR  */
#line 1715 "lang11d"
                            { yyval = zzval; }
#line 4713 "lang11d_tab.cpp"
    break;

  case 295: /* binop: BINOP  */
#line 1718 "lang11d"
                { yyval = zzval; }
#line 4719 "lang11d_tab.cpp"
    break;

  case 296: /* binop: READWRITEVAR  */
#line 1719 "lang11d"
                               { yyval = zzval; }
#line 4725 "lang11d_tab.cpp"
    break;

  case 297: /* binop: KWEXPAND  */
#line 1720 "lang11d"
                           { yyval = zzval; }
#line 4731 "lang11d_tab.cpp"
    break;

  case 298: /* binop: '<'  */
#line 1721 "lang11d"
                       { yyval = zzval; }
#line 4737 "lang11d_tab.cpp"
    break;

  case 299: /* binop: '>'  */
#line 1722 "lang11d"
                       { yyval = zzval; }
#line 4743 "lang11d_tab.cpp"
    break;

  case 300: /* binop: '-'  */
#line 1723 "lang11d"
                       { yyval = zzval; }
#line 4749 "lang11d_tab.cpp"
    break;

  case 301: /* binop: '*'  */
#line 1724 "lang11d"
                       { yyval = zzval; }
#line 4755 "lang11d_tab.cpp"
    break;

  case 302: /* binop: '+'  */
#line 1725 "lang11d"
                       { yyval = zzval; }
#line 4761 "lang11d_tab.cpp"
    break;

  case 303: /* binop: '|'  */
#line 1726 "lang11d"
                       { yyval = zzval; }
#line 4767 "lang11d_tab.cpp"
    break;

  case 304: /* keybinop: KEYBINOP  */
#line 1729 "lang11d"
                    { yyval = zzval; }
#line 4773 "lang11d_tab.cpp"
    break;

  case 307: /* curryarg: CURRYARG  */
#line 1736 "lang11d"
                    { yyval = zzval; }
#line 4779 "lang11d_tab.cpp"
    break;


#line 4783 "lang11d_tab.cpp"

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

