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
  YYSYMBOL_PIE = 31,                       /* PIE  */
  YYSYMBOL_ELLIPSIS = 32,                  /* ELLIPSIS  */
  YYSYMBOL_DOTDOT = 33,                    /* DOTDOT  */
  YYSYMBOL_BEGINCLOSEDFUNC = 34,           /* BEGINCLOSEDFUNC  */
  YYSYMBOL_BADTOKEN = 35,                  /* BADTOKEN  */
  YYSYMBOL_INTERPRET = 36,                 /* INTERPRET  */
  YYSYMBOL_LEFTARROW = 37,                 /* LEFTARROW  */
  YYSYMBOL_WHILE = 38,                     /* WHILE  */
  YYSYMBOL_COLON = 39,                     /* COLON  */
  YYSYMBOL_EQUALSSIGN = 40,                /* EQUALSSIGN  */
  YYSYMBOL_BINOP = 41,                     /* BINOP  */
  YYSYMBOL_KEYBINOP = 42,                  /* KEYBINOP  */
  YYSYMBOL_MINUS = 43,                     /* MINUS  */
  YYSYMBOL_LESSTHAN = 44,                  /* LESSTHAN  */
  YYSYMBOL_GREATERTHAN = 45,               /* GREATERTHAN  */
  YYSYMBOL_MULTIPLY = 46,                  /* MULTIPLY  */
  YYSYMBOL_ADD = 47,                       /* ADD  */
  YYSYMBOL_PIPE = 48,                      /* PIPE  */
  YYSYMBOL_READWRITEVAR = 49,              /* READWRITEVAR  */
  YYSYMBOL_DOT = 50,                       /* DOT  */
  YYSYMBOL_BACKTICK = 51,                  /* BACKTICK  */
  YYSYMBOL_UMINUS = 52,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 53,                  /* $accept  */
  YYSYMBOL_root = 54,                      /* root  */
  YYSYMBOL_classes = 55,                   /* classes  */
  YYSYMBOL_classextensions = 56,           /* classextensions  */
  YYSYMBOL_classdef = 57,                  /* classdef  */
  YYSYMBOL_classextension = 58,            /* classextension  */
  YYSYMBOL_optName = 59,                   /* optName  */
  YYSYMBOL_optSuperName = 60,              /* optSuperName  */
  YYSYMBOL_classvardecls = 61,             /* classvardecls  */
  YYSYMBOL_classvardecl = 62,              /* classvardecl  */
  YYSYMBOL_methods = 63,                   /* methods  */
  YYSYMBOL_methoddef = 64,                 /* methoddef  */
  YYSYMBOL_optsemi = 65,                   /* optsemi  */
  YYSYMBOL_optcomma = 66,                  /* optcomma  */
  YYSYMBOL_optequal = 67,                  /* optequal  */
  YYSYMBOL_funcbody = 68,                  /* funcbody  */
  YYSYMBOL_cmdlinecode = 69,               /* cmdlinecode  */
  YYSYMBOL_methbody = 70,                  /* methbody  */
  YYSYMBOL_optPrim = 71,                   /* optPrim  */
  YYSYMBOL_retval = 72,                    /* retval  */
  YYSYMBOL_funretval = 73,                 /* funretval  */
  YYSYMBOL_blocklist1 = 74,                /* blocklist1  */
  YYSYMBOL_blocklistitem = 75,             /* blocklistitem  */
  YYSYMBOL_blocklist = 76,                 /* blocklist  */
  YYSYMBOL_msgsend = 77,                   /* msgsend  */
  YYSYMBOL_generator = 78,                 /* generator  */
  YYSYMBOL_79_1 = 79,                      /* $@1  */
  YYSYMBOL_80_2 = 80,                      /* $@2  */
  YYSYMBOL_nextqual = 81,                  /* nextqual  */
  YYSYMBOL_qual = 82,                      /* qual  */
  YYSYMBOL_expr1 = 83,                     /* expr1  */
  YYSYMBOL_valrangex1 = 84,                /* valrangex1  */
  YYSYMBOL_valrangeassign = 85,            /* valrangeassign  */
  YYSYMBOL_valrangexd = 86,                /* valrangexd  */
  YYSYMBOL_valrange2 = 87,                 /* valrange2  */
  YYSYMBOL_valrange3 = 88,                 /* valrange3  */
  YYSYMBOL_expr = 89,                      /* expr  */
  YYSYMBOL_adverb = 90,                    /* adverb  */
  YYSYMBOL_exprn = 91,                     /* exprn  */
  YYSYMBOL_exprseq = 92,                   /* exprseq  */
  YYSYMBOL_arrayelems = 93,                /* arrayelems  */
  YYSYMBOL_arrayelems1 = 94,               /* arrayelems1  */
  YYSYMBOL_arglist1 = 95,                  /* arglist1  */
  YYSYMBOL_arglistv1 = 96,                 /* arglistv1  */
  YYSYMBOL_keyarglist1 = 97,               /* keyarglist1  */
  YYSYMBOL_keyarg = 98,                    /* keyarg  */
  YYSYMBOL_optkeyarglist = 99,             /* optkeyarglist  */
  YYSYMBOL_mavars = 100,                   /* mavars  */
  YYSYMBOL_nameList = 101,                 /* nameList  */
  YYSYMBOL_slotliteral = 102,              /* slotliteral  */
  YYSYMBOL_blockliteral = 103,             /* blockliteral  */
  YYSYMBOL_pushname = 104,                 /* pushname  */
  YYSYMBOL_pushliteral = 105,              /* pushliteral  */
  YYSYMBOL_listliteral = 106,              /* listliteral  */
  YYSYMBOL_block = 107,                    /* block  */
  YYSYMBOL_funcvardecls = 108,             /* funcvardecls  */
  YYSYMBOL_funcvardecls1 = 109,            /* funcvardecls1  */
  YYSYMBOL_funcvardecl = 110,              /* funcvardecl  */
  YYSYMBOL_argdecls = 111,                 /* argdecls  */
  YYSYMBOL_argdecls1 = 112,                /* argdecls1  */
  YYSYMBOL_constdeflist = 113,             /* constdeflist  */
  YYSYMBOL_constdef = 114,                 /* constdef  */
  YYSYMBOL_slotdeflist0 = 115,             /* slotdeflist0  */
  YYSYMBOL_slotdeflist = 116,              /* slotdeflist  */
  YYSYMBOL_slotdef = 117,                  /* slotdef  */
  YYSYMBOL_vardeflist0 = 118,              /* vardeflist0  */
  YYSYMBOL_vardeflist = 119,               /* vardeflist  */
  YYSYMBOL_vardef = 120,                   /* vardef  */
  YYSYMBOL_dictslotdef = 121,              /* dictslotdef  */
  YYSYMBOL_dictslotlist1 = 122,            /* dictslotlist1  */
  YYSYMBOL_dictslotlist = 123,             /* dictslotlist  */
  YYSYMBOL_rwslotdeflist = 124,            /* rwslotdeflist  */
  YYSYMBOL_rwslotdef = 125,                /* rwslotdef  */
  YYSYMBOL_listlit = 126,                  /* listlit  */
  YYSYMBOL_listlit2 = 127,                 /* listlit2  */
  YYSYMBOL_literallistc = 128,             /* literallistc  */
  YYSYMBOL_literallist1 = 129,             /* literallist1  */
  YYSYMBOL_rwspec = 130,                   /* rwspec  */
  YYSYMBOL_rspec = 131,                    /* rspec  */
  YYSYMBOL_integer = 132,                  /* integer  */
  YYSYMBOL_floatr = 133,                   /* floatr  */
  YYSYMBOL_accidental = 134,               /* accidental  */
  YYSYMBOL_floatp = 135,                   /* floatp  */
  YYSYMBOL_name = 136,                     /* name  */
  YYSYMBOL_binop = 137,                    /* binop  */
  YYSYMBOL_binop2 = 138,                   /* binop2  */
  YYSYMBOL_curryArg = 139                  /* curryArg  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;


/* Second part of user prologue.  */
#line 134 "lang/LangSource/Bison/lang11d"


#include <stdlib.h>
#include <string.h>
#include "PyrLexer.h"
#include "PyrParseNode.h"
#include "SC_Constants.h"
#include "PredefinedSymbols.h"
#include "SimpleStack.h"


void yyerror(const char* s) { bison_cxt->logErrorInCurFile(yylloc, s); }

inline bool checkArraySubrangeSyntax(PyrParseNode* args, const size_t argLen) {
	if(argLen <= 2) 
		return true;
	const auto first = args->mNext->mNext;
	auto last = first;
	while(true) {
		auto next = last->mNext;
		if (next) {
			last = next;
		} else 
			break;
	}
	const auto range = sc::lex::SourceCodeRange::range(first->location, last->location);
	bison_cxt->postErrorInCurrentFile(range, "Invalid subrange", "expected 2 or less arguments, delete these.");
	return false;
}


#line 274 "lang/LangSource/Bison/lang11d_tab.cpp"


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
#define YYFINAL  60
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1536

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  53
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  87
/* YYNRULES -- Number of rules.  */
#define YYNRULES  284
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  545

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   307


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
      45,    46,    47,    48,    49,    50,    51,    52
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   175,   175,   180,   185,   191,   192,   195,   196,   199,
     201,   204,   207,   207,   210,   210,   212,   213,   216,   218,
     220,   223,   224,   227,   229,   231,   233,   236,   236,   237,
     237,   238,   238,   240,   241,   244,   246,   248,   250,   252,
     254,   256,   259,   260,   263,   264,   266,   267,   270,   272,
     275,   276,   279,   279,   281,   281,   283,   286,   289,   292,
     295,   298,   301,   316,   322,   325,   331,   337,   343,   352,
     372,   377,   382,   384,   389,   405,   407,   409,   422,   431,
     431,   438,   438,   441,   451,   453,   470,   490,   500,   507,
     509,   523,   524,   525,   526,   527,   528,   529,   536,   542,
     544,   546,   548,   550,   556,   558,   570,   580,   594,   607,
     619,   636,   655,   666,   680,   693,   702,   716,   724,   734,
     740,   745,   753,   763,   774,   784,   794,   800,   801,   802,
     803,   805,   810,   819,   825,   827,   833,   837,   844,   846,
     851,   857,   858,   859,   860,   862,   863,   866,   868,   869,
     871,   872,   874,   876,   878,   880,   883,   884,   887,   889,
     892,   893,   896,   899,   900,   902,   904,   907,   908,   911,
     912,   913,   914,   915,   916,   917,   918,   919,   921,   923,
     925,   926,   927,   928,   929,   930,   931,   932,   933,   935,
     936,   937,   938,   939,   940,   941,   942,   943,   944,   946,
     948,   951,   952,   955,   956,   959,   962,   963,   965,   967,
     969,   971,   973,   976,   978,   980,   982,   984,   986,   989,
     990,   993,   996,   997,   999,  1000,  1003,  1005,  1007,  1014,
    1015,  1017,  1018,  1021,  1023,  1025,  1032,  1034,  1037,  1038,
    1041,  1042,  1044,  1045,  1048,  1050,  1053,  1055,  1058,  1060,
    1063,  1064,  1066,  1067,  1070,  1071,  1072,  1073,  1075,  1076,
    1078,  1079,  1086,  1087,  1094,  1095,  1104,  1105,  1106,  1112,
    1118,  1119,  1126,  1126,  1127,  1127,  1127,  1127,  1127,  1127,
    1127,  1127,  1128,  1128,  1129
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
  "SC_CONST", "NILOBJ", "TRUEOBJ", "FALSEOBJ", "PIE", "ELLIPSIS", "DOTDOT",
  "BEGINCLOSEDFUNC", "BADTOKEN", "INTERPRET", "LEFTARROW", "WHILE",
  "COLON", "EQUALSSIGN", "BINOP", "KEYBINOP", "MINUS", "LESSTHAN",
  "GREATERTHAN", "MULTIPLY", "ADD", "PIPE", "READWRITEVAR", "DOT",
  "BACKTICK", "UMINUS", "$accept", "root", "classes", "classextensions",
  "classdef", "classextension", "optName", "optSuperName", "classvardecls",
  "classvardecl", "methods", "methoddef", "optsemi", "optcomma",
  "optequal", "funcbody", "cmdlinecode", "methbody", "optPrim", "retval",
  "funretval", "blocklist1", "blocklistitem", "blocklist", "msgsend",
  "generator", "$@1", "$@2", "nextqual", "qual", "expr1", "valrangex1",
  "valrangeassign", "valrangexd", "valrange2", "valrange3", "expr",
  "adverb", "exprn", "exprseq", "arrayelems", "arrayelems1", "arglist1",
  "arglistv1", "keyarglist1", "keyarg", "optkeyarglist", "mavars",
  "nameList", "slotliteral", "blockliteral", "pushname", "pushliteral",
  "listliteral", "block", "funcvardecls", "funcvardecls1", "funcvardecl",
  "argdecls", "argdecls1", "constdeflist", "constdef", "slotdeflist0",
  "slotdeflist", "slotdef", "vardeflist0", "vardeflist", "vardef",
  "dictslotdef", "dictslotlist1", "dictslotlist", "rwslotdeflist",
  "rwslotdef", "listlit", "listlit2", "literallistc", "literallist1",
  "rwspec", "rspec", "integer", "floatr", "accidental", "floatp", "name",
  "binop", "binop2", "curryArg", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-489)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-282)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      98,   544,     4,    32,    76,    52,  -489,    78,  1044,   604,
    1414,   297,    13,  -489,  -489,  -489,  -489,  -489,  -489,  -489,
     201,  -489,    13,    13,  -489,  -489,  -489,  -489,    95,  -489,
     352,    13,  1414,  -489,  -489,  -489,  -489,  -489,    85,  -489,
    -489,  -489,  1484,   107,    94,  -489,  -489,  -489,  -489,   836,
    -489,   836,  -489,    99,   109,  -489,  -489,    34,  -489,   145,
    -489,    16,  -489,  -489,  1414,    13,  1414,    13,  -489,   660,
    1414,   115,   173,   169,  1414,  1085,  -489,  1414,   352,  -489,
    -489,  -489,  -489,   307,  -489,   178,   232,   836,   836,  -489,
     180,   184,  -489,   192,   556,  1444,   213,   179,    77,  -489,
     186,  1044,   710,    46,  -489,  -489,  -489,   249,  -489,    17,
     204,   313,  -489,  -489,  -489,  -489,  -489,   207,    91,  -489,
     460,  -489,  1126,  -489,  -489,  -489,   200,   195,  1414,  -489,
    -489,  -489,  -489,  -489,   836,  -489,  -489,   877,  1414,    46,
    -489,    13,   224,   254,  -489,   243,   327,  -489,   257,   163,
     836,  -489,  1414,  -489,  1167,  -489,  -489,  1414,   258,    11,
    -489,  -489,  -489,  1414,  1414,  1414,   278,   292,   836,  1208,
    -489,  -489,   159,  -489,  -489,  1444,  -489,  -489,  -489,   308,
    -489,  -489,  -489,  -489,  -489,   311,   303,    99,  -489,  -489,
    1444,  1414,    13,    13,  1414,   312,    46,  1414,  1414,  -489,
     315,   316,   320,  -489,  -489,  -489,    13,  1414,  1414,    13,
    -489,   836,    13,  -489,  -489,    13,  -489,  1473,  1414,   170,
    1249,   752,    43,   104,  1414,  1484,  -489,    46,   315,   316,
    1484,   306,   333,  -489,  -489,  -489,   335,    13,  -489,   345,
      13,  -489,   355,  -489,  -489,  1414,   321,  -489,  -489,  1414,
    1414,   332,  -489,  -489,  -489,  -489,   362,  1414,   336,  -489,
     919,    46,   365,  1444,  -489,  1444,  -489,   380,  1484,  -489,
    -489,  1484,  -489,    46,  -489,  -489,  -489,   961,  -489,   386,
     366,   407,   366,   411,  -489,   412,  1484,   331,   417,    22,
    -489,  1414,    26,  -489,  -489,  -489,  -489,  -489,  -489,  -489,
    -489,    99,  -489,   416,   383,  1414,  1290,  1414,   262,    46,
     315,   316,   320,   794,  1414,  -489,  1414,   413,  -489,  -489,
     377,    46,   423,   430,  -489,   491,  -489,   437,   439,   404,
     261,  1003,   370,  1003,    36,  -489,  -489,  1414,   415,  -489,
    1414,  -489,    46,   315,   316,  -489,   444,  -489,  -489,  1414,
    -489,   320,    46,  -489,  -489,    46,  -489,  -489,    13,  -489,
      13,  -489,   445,   414,  1414,   418,   446,   449,   419,  1331,
    -489,   456,   458,   459,    46,   315,   316,   320,  1484,   461,
      88,  -489,   454,   465,    95,    95,   467,   -15,   -15,   428,
    -489,   344,    13,     0,   469,   464,   256,  -489,    13,   472,
      13,  -489,  -489,  1414,  -489,    46,   470,   474,  -489,  -489,
    -489,  -489,  -489,   468,   432,  -489,  1414,  1484,  1414,   447,
     448,  1414,   451,   480,    46,  -489,    46,  -489,   486,   493,
     496,  -489,  1414,  -489,    95,    95,  -489,  -489,  -489,  -489,
    -489,  -489,   389,  -489,    13,   393,  -489,   394,  -489,    13,
    -489,   471,  1414,  1414,  -489,  1003,  -489,  1414,   462,   500,
    -489,   466,  -489,    46,  -489,  -489,  -489,  1484,  1484,  1414,
    1414,  1484,  1414,   473,  -489,  -489,    46,  -489,    46,  1484,
    -489,  -489,    68,    68,   261,  -489,   -15,   481,  -489,  -489,
     428,   490,  1414,   464,   464,  -489,   464,  1414,  -489,  -489,
    -489,  1484,  1484,  1484,  1414,  -489,  -489,    68,    68,   501,
    1373,  1373,   479,  -489,  1493,  -489,  1493,   464,  -489,  -489,
    -489,   464,  1484,  1373,  1373,  -489,  1414,   508,  -489,   531,
     538,  -489,  -489,  -489,  -489,  -489,   540,   542,   556,  -489,
    -489,  -489,  -489,  -489,  -489
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       5,    48,     0,     0,     2,     3,     7,   206,   148,   240,
       0,     0,     0,   272,   260,   262,   264,   184,   183,   182,
     130,   284,     0,   229,   187,   185,   186,   270,   206,   273,
       0,   222,     0,    41,     4,    33,    96,    93,   127,   104,
     129,   128,   145,    27,    48,    92,    94,    91,   178,    48,
     203,    48,   188,   180,   266,   267,   181,   179,    95,     0,
       1,    14,     6,     8,     0,   229,     0,   222,   201,   240,
       0,   150,     0,    29,     0,     0,   274,   283,   278,   276,
     277,   279,   280,   222,   275,     0,     0,    48,    48,   238,
      29,     0,   282,     0,    27,   250,     0,     0,   165,   167,
      98,   148,     0,    65,    50,    53,    52,     0,   231,   233,
       0,   230,   201,   261,   263,   265,   271,     0,    29,   224,
      31,   132,     0,   283,   278,   281,     0,   141,    28,   147,
      34,    40,   204,    39,    48,   269,   268,     0,     0,    56,
      21,    12,     0,     0,    81,     0,   230,    79,     0,    29,
      48,   152,     0,    99,    30,   149,   118,     0,     0,     0,
     237,   100,    97,     0,   117,     0,     0,     0,    48,    30,
     241,   102,     0,    28,    49,   250,   193,   192,   191,     0,
     197,   195,   196,   252,   198,     0,    29,   189,   190,   194,
     250,     0,     0,     0,     0,     0,    54,     0,     0,   156,
      29,    29,    29,   160,    51,   205,     0,     0,     0,     0,
     213,    48,     0,    30,   216,     0,    32,     0,     0,     0,
       0,     0,    54,     0,     0,   146,    38,     0,    29,    29,
     134,     0,     0,    13,    15,    16,     0,     0,   207,     0,
       0,   210,     0,   202,   151,     0,   153,   122,   101,     0,
     123,     0,   119,   236,    37,    36,     0,     0,     0,   239,
       0,    57,     0,   250,   246,    30,   251,     0,   138,   168,
     166,   135,    64,    55,    66,   162,   158,    30,   163,     0,
      30,     0,    30,     0,   232,     0,   234,     0,     0,     0,
     225,     0,     0,   173,   172,   171,   176,   174,   175,   227,
     177,   169,   170,     0,   103,     0,     0,     0,     0,    54,
      29,    29,    29,     0,     0,    78,     0,     0,   143,   142,
     133,    58,     0,     0,    11,   279,    22,     0,     0,    14,
      21,     0,     0,     0,     0,   199,   154,     0,     0,   124,
     121,    35,     0,    29,    29,   248,     0,   253,   247,     0,
     157,    29,    54,    69,   161,    54,   235,   214,     0,   200,
       0,   217,     0,   106,     0,   105,     0,     0,   131,     0,
      70,     0,     0,     0,    54,    29,    29,    29,   136,     0,
      54,    62,     0,     0,   206,   206,     0,   254,   254,   258,
      17,     0,     0,     0,     0,    83,   179,   208,     0,     0,
       0,   211,   155,   125,   120,    60,     0,     0,   249,   159,
     164,    68,    67,     0,     0,   228,     0,   139,     0,   107,
     112,     0,   111,     0,    54,    74,    54,    75,     0,     0,
       0,   144,     0,    59,   206,   206,   201,   201,    16,   255,
     257,   256,     0,   242,     0,     0,   259,    29,   219,     0,
       9,     0,     0,     0,    82,     0,    88,     0,     0,     0,
      80,     0,   126,    54,    63,   215,   218,   109,   108,     0,
       0,   140,     0,   113,    73,    71,    54,    77,    54,   137,
     201,   201,    44,    44,    21,    19,   254,   244,    18,    20,
     258,     0,     0,    83,    83,    84,    83,     0,   209,   212,
      61,   110,   115,   114,     0,    76,    72,    44,    44,    27,
      46,    46,     0,   243,     0,   220,     0,    83,    90,    89,
      85,    83,   116,    46,    46,    45,     0,     0,    42,    46,
       0,    10,   245,   221,    87,    86,     0,     0,    27,    23,
      43,    25,    24,    26,    47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -489,  -489,  -489,  -489,  -489,   543,  -489,   221,   114,  -489,
    -316,  -489,   -92,   194,  -489,    -9,  -489,  -488,  -228,    48,
     526,   -12,   -96,  -202,  -489,    55,  -489,  -489,  -273,  -323,
    -489,  -489,  -489,  -489,  -489,  -489,    -7,  -489,  -489,    -8,
     478,  -489,   -51,  -125,   -83,   298,  -147,  -489,  -489,   -98,
     234,  -489,  -489,   318,  -489,  -103,     9,    64,   -22,   572,
    -489,   100,   517,   518,   373,   528,    -6,   385,   425,  -489,
    -489,   208,   124,  -213,  -489,  -162,  -489,  -489,  -489,   -90,
    -489,  -489,   -80,    41,  -220,     7,  -489
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     3,     4,     5,    62,     6,   232,   143,   330,   390,
     231,   326,   129,   278,   217,    33,    34,   527,   510,   528,
      35,   273,   104,   274,    36,    37,   239,   236,   456,   394,
      38,    39,    40,    41,    85,   158,    42,   224,    43,    44,
      72,    73,   200,   201,   351,   203,   279,    97,    98,   299,
      45,    46,    47,   183,    48,   150,    49,   243,    68,    51,
     447,   448,   117,   118,   119,   110,   107,   108,    89,    90,
      91,   442,   443,    52,   184,   185,   186,   444,   449,    53,
      54,    55,    56,    57,    92,   127,    58
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      71,    86,   174,    94,   300,   187,   112,   204,   103,   211,
     399,   328,   229,   262,   391,   188,    93,   111,    87,   202,
     315,   141,   249,   530,   207,   121,    59,    13,   267,   439,
     440,    95,    60,   360,   441,   536,   537,     7,   452,   453,
     131,   137,   133,   204,   250,   139,     7,   400,    96,     7,
     313,    29,    99,   100,   281,   142,   144,   208,   147,   146,
     134,    86,   151,   109,   109,    50,   156,   159,    28,   160,
     361,   219,   120,    50,   138,   105,    93,    28,   166,   167,
      28,   322,   323,   314,   401,   187,   228,    64,   192,   509,
     122,     7,    22,    71,   199,   188,   311,   168,    61,     2,
     187,   346,   213,    65,    10,   383,   109,   370,   120,   193,
     188,   316,   105,   132,   199,    50,   128,    66,    13,    14,
      65,   225,    28,  -223,   120,   226,    67,   301,   432,   199,
     135,   230,   495,   318,     1,   344,   189,   302,   312,   214,
     136,   242,    29,    67,   244,     2,   246,   317,   140,   247,
     411,   132,    50,   412,   152,   251,   252,   253,   105,   256,
     261,   258,     7,   371,   372,   204,   260,   222,   512,   308,
     310,   328,   427,   187,   213,   187,   304,   204,   433,   153,
     154,   305,   233,   188,   268,   188,   161,   271,   376,   275,
     276,   169,   171,    28,   105,  -223,   406,   407,   132,   285,
     172,   286,   288,   306,     7,   220,   101,   221,   102,   343,
     303,   241,   199,   199,    13,   321,   189,   320,   190,   191,
     518,   519,   474,   520,   475,   204,   194,   105,   428,   429,
     377,   189,   132,   269,   270,    28,   209,   336,    29,   212,
     162,   338,   339,   163,   534,   223,   234,   109,   535,   160,
     287,   105,   199,   289,   106,   511,   120,   235,   205,     7,
     206,   500,   375,   137,   319,   164,   248,   155,   368,   350,
      13,   165,   327,   305,   505,   237,   506,   105,   332,   523,
     524,   334,   105,   362,   170,   387,   254,   388,   389,   240,
      28,   106,   328,   457,    29,   369,   138,   350,   366,   367,
     255,   300,    95,   300,   189,   199,   189,   378,   379,   204,
     324,    13,   215,   263,   265,  -281,   105,   264,   272,    96,
      13,    13,   210,   395,   206,   395,   277,   280,   105,   402,
     405,   282,   404,   482,   483,    29,   238,   106,   206,   329,
     357,   409,   358,   215,    29,    29,   331,    76,   450,   124,
      79,    80,   325,    82,   125,    84,   333,   417,    13,   335,
     337,   423,   436,   437,   105,   340,   382,   113,   114,   115,
     341,   345,   396,   106,   396,   165,   105,   507,   508,   397,
     266,   398,    29,   116,   139,    76,   348,   124,    79,    80,
     325,    82,   125,    84,   352,   462,   283,   105,   485,   413,
     486,   414,   488,   489,   486,   213,   106,   105,   197,   467,
     105,   468,   480,   481,   471,   353,   532,   525,   533,   355,
     356,   359,   363,   364,   301,   479,   301,   126,   113,   105,
     106,   380,   327,   451,   302,   105,   302,   458,   381,   459,
     384,   461,   385,   142,   493,   494,   544,   395,   403,   496,
     408,   105,   419,   415,   416,   420,   106,   434,   418,   421,
     105,   106,   501,   502,   424,   503,   425,   426,   435,   431,
     438,  -226,   446,   454,  -226,   455,   460,   465,   463,   105,
     466,   105,   464,   531,   517,   487,   473,   469,   470,   521,
     491,   472,  -226,    13,   476,   106,   396,   522,  -226,   497,
     216,   477,   529,   529,   478,    13,   373,   106,  -226,   498,
     173,   492,   539,   504,   499,   529,   529,    29,   105,   538,
      76,   514,   124,    79,    80,   325,    82,   125,    84,    29,
     516,   105,    76,   105,   124,    79,    80,    81,    82,   125,
      84,   526,   541,   106,   542,   410,   543,     7,    63,     8,
     386,     9,   484,   327,    10,   106,    11,    12,    13,    14,
      15,    16,    17,    18,    19,   173,    20,    21,    22,    23,
     130,   430,    24,    25,    26,    27,   106,   540,    28,   195,
     354,    88,    29,   347,   148,   149,   106,    30,   290,   106,
     515,   284,    31,   145,   259,    32,   445,    76,   123,   124,
      79,    80,    81,    82,   125,    84,   126,     7,   106,     8,
     513,    69,     0,     0,   106,     0,    11,    12,    13,    14,
      15,    16,    17,    18,    19,     0,    20,    21,    22,    23,
     106,     0,    24,    25,    26,    27,     0,    74,    28,   106,
       0,   490,    29,    75,     0,    76,    77,    78,    79,    80,
      81,    82,    83,    84,     0,    32,     0,     0,   106,     0,
     106,     0,     0,     7,     0,     8,     0,    69,     0,     0,
       0,     0,    11,    12,    13,    14,    15,    16,    17,    18,
      19,     0,    20,    21,     0,     0,     0,     0,    24,    25,
      26,    27,     0,    74,    28,     0,     0,   106,    29,    75,
       0,    76,    77,    78,    79,    80,    81,    82,   125,    84,
     106,    32,   106,     7,     0,     8,     0,    69,   196,     0,
       0,     0,    11,    12,    13,    14,    15,    16,    17,    18,
      19,     0,    20,    21,     0,     0,     0,     0,    24,    25,
      26,    27,     0,     0,    28,     0,     0,     0,    29,     0,
       0,     0,   197,    30,     0,     7,   198,     8,     0,    69,
     309,    32,     0,     0,    11,    12,    13,    14,    15,    16,
      17,    18,    19,     0,    20,    21,     0,     0,     0,     0,
      24,    25,    26,    27,     0,     0,    28,     0,     0,     0,
      29,     0,     0,     0,   197,    30,     0,     7,   198,     8,
       0,    69,   374,    32,     0,     0,    11,    12,    13,    14,
      15,    16,    17,    18,    19,     0,    20,    21,     0,     0,
       0,     0,    24,    25,    26,    27,     0,     0,    28,     0,
       0,     0,    29,     0,     0,     0,   197,    30,     0,     7,
     198,     8,     0,    69,     0,    32,    10,     0,    11,    12,
      13,    14,    15,    16,    17,    18,    19,     0,    20,    21,
      22,     0,     0,     0,    24,    25,    26,    27,     0,     0,
      28,     0,     0,     0,    29,     0,     0,     0,     0,    30,
       7,     0,     8,     0,    69,   227,     0,    32,     0,    11,
      12,    13,    14,    15,    16,    17,    18,    19,     0,    20,
      21,     0,     0,     0,     0,    24,    25,    26,    27,     0,
       0,    28,     0,     0,     0,    29,     0,     0,     0,     0,
      30,     0,     7,   198,     8,     0,    69,   342,    32,     0,
       0,    11,    12,    13,    14,    15,    16,    17,    18,    19,
       0,    20,    21,     0,     0,     0,     0,    24,    25,    26,
      27,     0,     0,    28,     0,     0,     0,    29,     0,     0,
       0,     0,    30,     0,     7,   198,     8,     0,    69,     0,
      32,     0,     0,    11,    12,    13,    14,    15,    16,    17,
      18,    19,     0,    20,    21,     0,     0,     0,     0,    24,
      25,    26,    27,     0,     0,    28,     0,     0,     0,    29,
       0,     0,     0,   197,    30,     0,     7,   349,     8,     0,
      69,     0,    32,     0,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    19,     0,    20,    21,   392,     0,     0,
       0,    24,    25,    26,    27,     0,     0,    28,     0,     0,
       0,    29,   393,     0,     0,     0,    30,     7,     0,     8,
       0,    69,     0,     0,    32,     0,    11,    12,    13,    14,
      15,    16,    17,    18,    19,     0,    20,    21,     0,     0,
       0,     0,    24,    25,    26,    27,     0,     0,    28,     0,
       0,     0,    29,     0,     0,     0,    70,    30,     7,     0,
       8,     0,    69,     0,     0,    32,     0,    11,    12,    13,
      14,    15,    16,    17,    18,    19,     0,    20,    21,     0,
       0,     0,     0,    24,    25,    26,    27,     0,   157,    28,
       0,     0,     0,    29,     0,     0,     0,     0,    30,     7,
       0,     8,     0,    69,     0,     0,    32,     0,    11,    12,
      13,    14,    15,    16,    17,    18,    19,     0,    20,    21,
       0,     0,     0,     0,    24,    25,    26,    27,     0,   218,
      28,     0,     0,     0,    29,     0,     0,     0,     0,    30,
       7,     0,     8,     0,    69,     0,     0,    32,     0,    11,
      12,    13,    14,    15,    16,    17,    18,    19,     0,    20,
      21,     0,     0,     0,     0,    24,    25,    26,    27,     0,
       0,    28,     0,     0,     0,    29,     0,     0,     0,   245,
      30,     7,     0,     8,     0,    69,     0,     0,    32,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    19,     0,
      20,    21,     0,     0,     0,     0,    24,    25,    26,    27,
       0,     0,    28,     0,     0,     0,    29,     0,     0,     0,
     257,    30,     7,     0,     8,     0,    69,     0,     0,    32,
       0,    11,    12,    13,    14,    15,    16,    17,    18,    19,
       0,    20,    21,     0,     0,     0,     0,    24,    25,    26,
      27,     0,   307,    28,     0,     0,     0,    29,     0,     0,
       0,     0,    30,     7,     0,     8,   365,    69,     0,     0,
      32,     0,    11,    12,    13,    14,    15,    16,    17,    18,
      19,     0,    20,    21,     0,     0,     0,     0,    24,    25,
      26,    27,     0,     0,    28,     0,     0,     0,    29,     0,
       0,     0,     0,    30,     7,     0,     8,   422,    69,     0,
       0,    32,     0,    11,    12,    13,    14,    15,    16,    17,
      18,    19,     0,    20,    21,     0,     0,     0,     0,    24,
      25,    26,    27,     0,     0,    28,     0,     0,     0,    29,
       0,     0,     0,     0,    30,     0,     7,     0,     8,     0,
      69,     0,    32,   526,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    19,     0,    20,    21,     0,     0,     0,
       0,    24,    25,    26,    27,     0,     0,    28,     0,     0,
       0,    29,     0,     0,     0,     0,    30,     7,     0,     8,
       0,    69,     0,     0,    32,     0,    11,    12,    13,    14,
      15,    16,    17,    18,    19,     0,    20,    21,     0,     0,
       0,     0,    24,    25,    26,    27,     0,     0,    28,   175,
       0,     0,    29,     0,     0,     0,     0,    30,    13,    14,
      15,    16,   176,   177,   178,    32,   179,     0,     0,     0,
       0,     0,   180,   181,   182,    27,     0,     0,     0,     0,
     291,     0,    29,     0,     0,   292,     0,    30,    14,    15,
      16,   293,   294,   295,     0,     0,     0,     0,     0,     0,
       0,   296,   297,   298,    27,   292,     0,     0,    14,    15,
      16,   293,   294,   295,     0,     0,    30,     0,     0,     0,
       0,   296,   297,   298,    27,    76,   123,   124,    79,    80,
      81,    82,   125,    84,   126,     0,    30
};

static const yytype_int16 yycheck[] =
{
       8,     9,    94,    10,   217,    95,    28,   103,    20,   112,
     333,   231,   137,   175,   330,    95,     9,    23,     9,   102,
     222,     5,    11,   511,     7,    32,    22,    14,   190,    44,
      45,     5,     0,    11,    49,   523,   524,     3,    38,    39,
      49,     7,    51,   139,    33,    57,     3,    11,    22,     3,
       7,    38,    11,    12,   201,    39,    64,    40,    66,    65,
      51,    69,    70,    22,    23,     1,    74,    75,    34,    77,
      48,   122,    31,     9,    40,    20,    69,    34,    87,    88,
      34,   228,   229,    40,    48,   175,   137,     9,    11,    21,
       5,     3,    24,   101,   102,   175,   221,    88,    22,    47,
     190,   263,    11,    25,    10,   325,    65,   309,    67,    32,
     190,     7,    57,    49,   122,    51,     9,    39,    14,    15,
      25,   128,    34,    32,    83,   134,    48,   217,    40,   137,
      31,   138,   455,   223,    36,   260,    95,   217,   221,    48,
      31,   150,    38,    48,   152,    47,   154,    43,     3,   157,
     352,    87,    88,   355,    39,   163,   164,   165,   103,   168,
     172,   169,     3,   310,   311,   261,     7,   126,   484,   220,
     221,   391,   374,   263,    11,   265,     6,   273,   380,     6,
      11,    11,   141,   263,   191,   265,     8,   194,   313,   197,
     198,    11,     8,    34,   139,    32,   343,   344,   134,   207,
       8,   208,   211,    33,     3,     5,     5,     7,     7,   260,
     218,    48,   220,   221,    14,   227,   175,   224,     5,    40,
     493,   494,   424,   496,   426,   321,    40,   172,   375,   376,
     313,   190,   168,   192,   193,    34,    32,   245,    38,    32,
       8,   249,   250,    11,   517,    50,    22,   206,   521,   257,
     209,   196,   260,   212,    20,   483,   215,     3,     9,     3,
      11,   463,   313,     7,   223,    33,     8,    73,     6,   277,
      14,    39,   231,    11,   476,    32,   478,   222,   237,   507,
     508,   240,   227,   291,    90,    24,     8,    26,    27,    32,
      34,    57,   512,    37,    38,    33,    40,   305,   306,   307,
       8,   514,     5,   516,   263,   313,   265,   314,   316,   405,
       4,    14,   118,     5,    11,     8,   261,     6,     6,    22,
      14,    14,     9,   331,    11,   333,    11,    11,   273,   337,
     342,    11,   340,   436,   437,    38,     9,   103,    11,     6,
       9,   349,    11,   149,    38,    38,    11,    41,     4,    43,
      44,    45,    46,    47,    48,    49,    11,   364,    14,     4,
      39,   369,   384,   385,   309,    33,   325,    15,    16,    17,
       8,     6,   331,   139,   333,    39,   321,   480,   481,     9,
     186,    11,    38,    31,   396,    41,     6,    43,    44,    45,
      46,    47,    48,    49,     8,   403,   202,   342,     9,   358,
      11,   360,     9,     9,    11,    11,   172,   352,    42,   416,
     355,   418,   434,   435,   421,     8,   514,   509,   516,     8,
       8,     4,     6,    40,   514,   432,   516,    50,    15,   374,
     196,     8,   391,   392,   514,   380,   516,   396,     8,   398,
       3,   400,     3,    39,   452,   453,   538,   455,    33,   457,
       6,   396,     6,     8,    40,     6,   222,     3,    40,    40,
     405,   227,   469,   470,     8,   472,     8,     8,     3,     8,
       3,    11,    44,     4,    14,    11,     4,     9,     8,   424,
      48,   426,     8,     4,   492,   444,     6,    40,    40,   497,
     449,    40,    32,    14,     8,   261,   455,   504,    38,    37,
      40,     8,   510,   511,     8,    14,   312,   273,    48,     9,
       9,    40,     4,    40,    48,   523,   524,    38,   463,   526,
      41,    40,    43,    44,    45,    46,    47,    48,    49,    38,
      40,   476,    41,   478,    43,    44,    45,    46,    47,    48,
      49,    10,     4,   309,     4,   351,     4,     3,     5,     5,
     329,     7,   438,   512,    10,   321,    12,    13,    14,    15,
      16,    17,    18,    19,    20,     9,    22,    23,    24,    25,
      44,   377,    28,    29,    30,    31,   342,   529,    34,   101,
     282,     9,    38,   265,    67,    67,   352,    43,   215,   355,
     490,   206,    48,    65,   169,    51,   388,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,     3,   374,     5,
     486,     7,    -1,    -1,   380,    -1,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    22,    23,    24,    25,
     396,    -1,    28,    29,    30,    31,    -1,    33,    34,   405,
      -1,   447,    38,    39,    -1,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    -1,    51,    -1,    -1,   424,    -1,
     426,    -1,    -1,     3,    -1,     5,    -1,     7,    -1,    -1,
      -1,    -1,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    -1,    22,    23,    -1,    -1,    -1,    -1,    28,    29,
      30,    31,    -1,    33,    34,    -1,    -1,   463,    38,    39,
      -1,    41,    42,    43,    44,    45,    46,    47,    48,    49,
     476,    51,   478,     3,    -1,     5,    -1,     7,     8,    -1,
      -1,    -1,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    -1,    22,    23,    -1,    -1,    -1,    -1,    28,    29,
      30,    31,    -1,    -1,    34,    -1,    -1,    -1,    38,    -1,
      -1,    -1,    42,    43,    -1,     3,    46,     5,    -1,     7,
       8,    51,    -1,    -1,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    -1,    22,    23,    -1,    -1,    -1,    -1,
      28,    29,    30,    31,    -1,    -1,    34,    -1,    -1,    -1,
      38,    -1,    -1,    -1,    42,    43,    -1,     3,    46,     5,
      -1,     7,     8,    51,    -1,    -1,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    22,    23,    -1,    -1,
      -1,    -1,    28,    29,    30,    31,    -1,    -1,    34,    -1,
      -1,    -1,    38,    -1,    -1,    -1,    42,    43,    -1,     3,
      46,     5,    -1,     7,    -1,    51,    10,    -1,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    22,    23,
      24,    -1,    -1,    -1,    28,    29,    30,    31,    -1,    -1,
      34,    -1,    -1,    -1,    38,    -1,    -1,    -1,    -1,    43,
       3,    -1,     5,    -1,     7,     8,    -1,    51,    -1,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    -1,    22,
      23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,    -1,
      -1,    34,    -1,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      43,    -1,     3,    46,     5,    -1,     7,     8,    51,    -1,
      -1,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      -1,    22,    23,    -1,    -1,    -1,    -1,    28,    29,    30,
      31,    -1,    -1,    34,    -1,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    43,    -1,     3,    46,     5,    -1,     7,    -1,
      51,    -1,    -1,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    -1,    22,    23,    -1,    -1,    -1,    -1,    28,
      29,    30,    31,    -1,    -1,    34,    -1,    -1,    -1,    38,
      -1,    -1,    -1,    42,    43,    -1,     3,    46,     5,    -1,
       7,    -1,    51,    -1,    -1,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    -1,    22,    23,    24,    -1,    -1,
      -1,    28,    29,    30,    31,    -1,    -1,    34,    -1,    -1,
      -1,    38,    39,    -1,    -1,    -1,    43,     3,    -1,     5,
      -1,     7,    -1,    -1,    51,    -1,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    22,    23,    -1,    -1,
      -1,    -1,    28,    29,    30,    31,    -1,    -1,    34,    -1,
      -1,    -1,    38,    -1,    -1,    -1,    42,    43,     3,    -1,
       5,    -1,     7,    -1,    -1,    51,    -1,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    -1,    22,    23,    -1,
      -1,    -1,    -1,    28,    29,    30,    31,    -1,    33,    34,
      -1,    -1,    -1,    38,    -1,    -1,    -1,    -1,    43,     3,
      -1,     5,    -1,     7,    -1,    -1,    51,    -1,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    22,    23,
      -1,    -1,    -1,    -1,    28,    29,    30,    31,    -1,    33,
      34,    -1,    -1,    -1,    38,    -1,    -1,    -1,    -1,    43,
       3,    -1,     5,    -1,     7,    -1,    -1,    51,    -1,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    -1,    22,
      23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,    -1,
      -1,    34,    -1,    -1,    -1,    38,    -1,    -1,    -1,    42,
      43,     3,    -1,     5,    -1,     7,    -1,    -1,    51,    -1,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
      22,    23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,
      -1,    -1,    34,    -1,    -1,    -1,    38,    -1,    -1,    -1,
      42,    43,     3,    -1,     5,    -1,     7,    -1,    -1,    51,
      -1,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      -1,    22,    23,    -1,    -1,    -1,    -1,    28,    29,    30,
      31,    -1,    33,    34,    -1,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    43,     3,    -1,     5,     6,     7,    -1,    -1,
      51,    -1,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    -1,    22,    23,    -1,    -1,    -1,    -1,    28,    29,
      30,    31,    -1,    -1,    34,    -1,    -1,    -1,    38,    -1,
      -1,    -1,    -1,    43,     3,    -1,     5,     6,     7,    -1,
      -1,    51,    -1,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    -1,    22,    23,    -1,    -1,    -1,    -1,    28,
      29,    30,    31,    -1,    -1,    34,    -1,    -1,    -1,    38,
      -1,    -1,    -1,    -1,    43,    -1,     3,    -1,     5,    -1,
       7,    -1,    51,    10,    -1,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    -1,    22,    23,    -1,    -1,    -1,
      -1,    28,    29,    30,    31,    -1,    -1,    34,    -1,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    43,     3,    -1,     5,
      -1,     7,    -1,    -1,    51,    -1,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    22,    23,    -1,    -1,
      -1,    -1,    28,    29,    30,    31,    -1,    -1,    34,     5,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    43,    14,    15,
      16,    17,    18,    19,    20,    51,    22,    -1,    -1,    -1,
      -1,    -1,    28,    29,    30,    31,    -1,    -1,    -1,    -1,
       7,    -1,    38,    -1,    -1,    12,    -1,    43,    15,    16,
      17,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    28,    29,    30,    31,    12,    -1,    -1,    15,    16,
      17,    18,    19,    20,    -1,    -1,    43,    -1,    -1,    -1,
      -1,    28,    29,    30,    31,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    -1,    43
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    36,    47,    54,    55,    56,    58,     3,     5,     7,
      10,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      22,    23,    24,    25,    28,    29,    30,    31,    34,    38,
      43,    48,    51,    68,    69,    73,    77,    78,    83,    84,
      85,    86,    89,    91,    92,   103,   104,   105,   107,   109,
     110,   112,   126,   132,   133,   134,   135,   136,   139,    22,
       0,    22,    57,    58,     9,    25,    39,    48,   111,     7,
      42,    92,    93,    94,    33,    39,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    87,    92,   109,   112,   121,
     122,   123,   137,   138,    89,     5,    22,   100,   101,   136,
     136,     5,     7,    74,    75,    78,   103,   119,   120,   136,
     118,   119,   111,    15,    16,    17,    31,   115,   116,   117,
     136,    89,     5,    42,    43,    48,    50,   138,     9,    65,
      73,    68,   110,    68,   109,    31,    31,     7,    40,    74,
       3,     5,    39,    60,    92,   118,   119,    92,   115,   116,
     108,    92,    39,     6,    11,    66,    92,    33,    88,    92,
      92,     8,     8,    11,    33,    39,    68,    68,   109,    11,
      66,     8,     8,     9,    65,     5,    18,    19,    20,    22,
      28,    29,    30,   106,   127,   128,   129,   132,   135,   136,
       5,    40,    11,    32,    40,    93,     8,    42,    46,    92,
      95,    96,    97,    98,    75,     9,    11,     7,    40,    32,
       9,   108,    32,    11,    48,    66,    40,    67,    33,    95,
       5,     7,   136,    50,    90,    89,    68,     8,    95,    96,
      89,    63,    59,   136,    22,     3,    80,    32,     9,    79,
      32,    48,    68,   110,    92,    42,    92,    92,     8,    11,
      33,    92,    92,    92,     8,     8,    68,    42,    92,   121,
       7,    74,   128,     5,     6,    11,    66,   128,    89,   136,
     136,    89,     6,    74,    76,    92,    92,    11,    66,    99,
      11,    99,    11,    66,   120,    92,    89,   136,    68,   136,
     117,     7,    12,    18,    19,    20,    28,    29,    30,   102,
     126,   132,   135,    92,     6,    11,    33,    33,    95,     8,
      95,    96,    97,     7,    40,    76,     7,    43,   132,   136,
      89,    74,    99,    99,     4,    46,    64,   136,   137,     6,
      61,    11,   136,    11,   136,     4,    92,    39,    92,    92,
      33,     8,     8,    95,    96,     6,   128,   106,     6,    46,
      92,    97,     8,     8,    98,     8,     8,     9,    11,     4,
      11,    48,    92,     6,    40,     6,    92,    92,     6,    33,
      76,    99,    99,    66,     8,    95,    96,    97,    89,    92,
       8,     8,   136,   137,     3,     3,    60,    24,    26,    27,
      62,    63,    24,    39,    82,    92,   136,     9,    11,    82,
      11,    48,    92,    33,    92,    74,    99,    99,     6,    92,
      66,    76,    76,   136,   136,     8,    40,    89,    40,     6,
       6,    40,     6,    92,     8,     8,     8,    76,    99,    99,
      66,     8,    40,    76,     3,     3,   111,   111,     3,    44,
      45,    49,   124,   125,   130,   124,    44,   113,   114,   131,
       4,   136,    38,    39,     4,    11,    81,    37,   136,   136,
       4,   136,    92,     8,     8,     9,    48,    89,    89,    40,
      40,    89,    40,     6,    76,    76,     8,     8,     8,    89,
     111,   111,   108,   108,    61,     9,    11,   136,     9,     9,
      66,   136,    40,    92,    92,    82,    92,    37,     9,    48,
      76,    89,    89,    89,    40,    76,    76,   108,   108,    21,
      71,    71,    63,   125,    40,   114,    40,    92,    81,    81,
      81,    92,    89,    71,    71,    65,    10,    70,    72,    92,
      70,     4,   102,   102,    81,    81,    70,    70,    89,     4,
      72,     4,     4,     4,    65
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    53,    54,    54,    54,    55,    55,    56,    56,    57,
      57,    58,    59,    59,    60,    60,    61,    61,    62,    62,
      62,    63,    63,    64,    64,    64,    64,    65,    65,    66,
      66,    67,    67,    68,    68,    69,    69,    69,    69,    69,
      69,    69,    70,    70,    71,    71,    72,    72,    73,    73,
      74,    74,    75,    75,    76,    76,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    79,
      78,    80,    78,    81,    81,    82,    82,    82,    82,    82,
      82,    83,    83,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    84,    84,    84,    85,    85,
      85,    86,    86,    86,    86,    86,    86,    87,    87,    87,
      87,    87,    88,    88,    88,    88,    88,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      89,    90,    90,    90,    90,    91,    91,    92,    93,    93,
      94,    94,    94,    94,    94,    94,    95,    95,    96,    96,
      97,    97,    98,    99,    99,   100,   100,   101,   101,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   103,   104,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   107,
     107,   108,   108,   109,   109,   110,   111,   111,   111,   111,
     111,   111,   111,   112,   112,   112,   112,   112,   112,   113,
     113,   114,   115,   115,   116,   116,   117,   117,   117,   118,
     118,   119,   119,   120,   120,   120,   121,   121,   122,   122,
     123,   123,   124,   124,   125,   125,   126,   126,   127,   127,
     128,   128,   129,   129,   130,   130,   130,   130,   131,   131,
     132,   132,   133,   133,   134,   134,   135,   135,   135,   135,
     135,   135,   136,   136,   137,   137,   137,   137,   137,   137,
     137,   137,   138,   138,   139
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
       3,     4,     3,     4,     1,     5,     5,     6,     7,     7,
       8,     6,     6,     7,     8,     8,     9,     2,     2,     3,
       5,     4,     2,     2,     3,     4,     5,     1,     1,     1,
       1,     5,     2,     4,     3,     4,     5,     7,     4,     6,
       7,     0,     2,     2,     4,     1,     3,     2,     0,     2,
       1,     3,     2,     3,     4,     5,     1,     3,     2,     4,
       1,     3,     2,     1,     3,     1,     3,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     5,
       5,     0,     2,     1,     2,     3,     0,     3,     5,     7,
       3,     5,     7,     3,     5,     7,     3,     5,     7,     1,
       3,     4,     0,     1,     1,     3,     1,     3,     5,     0,
       1,     1,     3,     1,     3,     4,     3,     2,     1,     3,
       0,     2,     1,     3,     2,     4,     4,     5,     3,     4,
       0,     2,     1,     3,     0,     1,     1,     1,     0,     1,
       1,     2,     1,     2,     1,     2,     1,     1,     2,     2,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
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
#line 176 "lang/LangSource/Bison/lang11d"
                                        { 
						(yyval.root) = bison_cxt->allocParseNode<PyrRootNode>((yyloc), (yyvsp[0].node));
						bison_cxt->assignRoot(*(yyval.root));
					}
#line 2202 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 3: /* root: classextensions  */
#line 181 "lang/LangSource/Bison/lang11d"
                                        { 
						(yyval.root) = bison_cxt->allocParseNode<PyrRootNode>((yyloc), (yyvsp[0].node));
						bison_cxt->assignRoot(*(yyval.root));
					}
#line 2211 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 4: /* root: INTERPRET cmdlinecode  */
#line 186 "lang/LangSource/Bison/lang11d"
                                        { 
						(yyval.root) = bison_cxt->allocParseNode<PyrRootNode>((yyloc), (yyvsp[0].blockNode));
						bison_cxt->assignRoot(*(yyval.root));
					}
#line 2220 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 5: /* classes: %empty  */
#line 191 "lang/LangSource/Bison/lang11d"
                                 { (yyval.node) = nullptr; }
#line 2226 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 6: /* classes: classes classdef  */
#line 193 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = linkNodes((yyvsp[-1].node), (yyvsp[0].node)); }
#line 2232 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 8: /* classextensions: classextensions classextension  */
#line 197 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = linkNodes((yyvsp[-1].node), (yyvsp[0].node)); }
#line 2238 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 9: /* classdef: CLASSNAME optSuperName OPENCURLY classvardecls methods CLOSECURLY  */
#line 200 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrClassNode>((yyloc), (yyvsp[-5].slotNode), (yyvsp[-4].slotNode), nullptr, (yyvsp[-2].varListNode), (yyvsp[-1].methodNode)); }
#line 2244 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 10: /* classdef: CLASSNAME OPENSQUARE optName CLOSESQUARE optSuperName OPENCURLY classvardecls methods CLOSECURLY  */
#line 202 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrClassNode>((yyloc), (yyvsp[-8].slotNode), (yyvsp[-4].slotNode), (yyvsp[-6].slotNode), (yyvsp[-2].varListNode), (yyvsp[-1].methodNode)); }
#line 2250 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 11: /* classextension: ADD CLASSNAME OPENCURLY methods CLOSECURLY  */
#line 205 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrClassExtNode>((yyloc), (yyvsp[-3].slotNode), (yyvsp[-1].methodNode)); }
#line 2256 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 12: /* optName: %empty  */
#line 207 "lang/LangSource/Bison/lang11d"
                                 { (yyval.slotNode) = nullptr; }
#line 2262 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 14: /* optSuperName: %empty  */
#line 210 "lang/LangSource/Bison/lang11d"
                         { (yyval.slotNode) = nullptr; }
#line 2268 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 15: /* optSuperName: COLON CLASSNAME  */
#line 210 "lang/LangSource/Bison/lang11d"
                                                             { (yyval.slotNode) = (yyvsp[0].slotNode); }
#line 2274 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 16: /* classvardecls: %empty  */
#line 212 "lang/LangSource/Bison/lang11d"
                         { (yyval.varListNode) = nullptr; }
#line 2280 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 17: /* classvardecls: classvardecls classvardecl  */
#line 214 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varListNode) = linkNodes((yyvsp[-1].varListNode), (yyvsp[0].varListNode)); }
#line 2286 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 18: /* classvardecl: CLASSVAR rwslotdeflist SEMICOLON  */
#line 217 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varListNode) = bison_cxt->allocParseNode<PyrVarListNode>((yyloc), (yyvsp[-1].varDefNode), varClass); }
#line 2292 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 19: /* classvardecl: VAR rwslotdeflist SEMICOLON  */
#line 219 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varListNode) = bison_cxt->allocParseNode<PyrVarListNode>((yyloc), (yyvsp[-1].varDefNode), varInst); }
#line 2298 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 20: /* classvardecl: SC_CONST constdeflist SEMICOLON  */
#line 221 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varListNode) = bison_cxt->allocParseNode<PyrVarListNode>((yyloc), (yyvsp[-1].varDefNode), varConst); }
#line 2304 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 21: /* methods: %empty  */
#line 223 "lang/LangSource/Bison/lang11d"
                                 { (yyval.methodNode) = nullptr; }
#line 2310 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 22: /* methods: methods methoddef  */
#line 225 "lang/LangSource/Bison/lang11d"
                                        { (yyval.methodNode) = linkNodes((yyvsp[-1].methodNode), (yyvsp[0].methodNode)); }
#line 2316 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 23: /* methoddef: name OPENCURLY argdecls funcvardecls optPrim methbody CLOSECURLY  */
#line 228 "lang/LangSource/Bison/lang11d"
                                        { (yyval.methodNode) = bison_cxt->allocParseNode<PyrMethodNode>((yyloc), (yyvsp[-6].slotNode), (yyvsp[-2].slotNode), (yyvsp[-4].argListNode), (yyvsp[-3].varListNode), (yyvsp[-1].node), false); }
#line 2322 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 24: /* methoddef: MULTIPLY name OPENCURLY argdecls funcvardecls optPrim methbody CLOSECURLY  */
#line 230 "lang/LangSource/Bison/lang11d"
                                        { (yyval.methodNode) = bison_cxt->allocParseNode<PyrMethodNode>((yyloc), (yyvsp[-6].slotNode), (yyvsp[-2].slotNode), (yyvsp[-4].argListNode), (yyvsp[-3].varListNode), (yyvsp[-1].node), true); }
#line 2328 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 25: /* methoddef: binop OPENCURLY argdecls funcvardecls optPrim methbody CLOSECURLY  */
#line 232 "lang/LangSource/Bison/lang11d"
                                        { (yyval.methodNode) = bison_cxt->allocParseNode<PyrMethodNode>((yyloc), (yyvsp[-6].slotNode), (yyvsp[-2].slotNode), (yyvsp[-4].argListNode), (yyvsp[-3].varListNode), (yyvsp[-1].node), false); }
#line 2334 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 26: /* methoddef: MULTIPLY binop OPENCURLY argdecls funcvardecls optPrim methbody CLOSECURLY  */
#line 234 "lang/LangSource/Bison/lang11d"
                                        { (yyval.methodNode) = bison_cxt->allocParseNode<PyrMethodNode>((yyloc), (yyvsp[-6].slotNode), (yyvsp[-2].slotNode), (yyvsp[-4].argListNode), (yyvsp[-3].varListNode), (yyvsp[-1].node), true); }
#line 2340 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 34: /* funcbody: exprseq funretval  */
#line 242 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrDropNode>((yyloc), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2346 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 35: /* cmdlinecode: OPENPAREN argdecls1 funcvardecls1 funcbody CLOSEPAREN  */
#line 245 "lang/LangSource/Bison/lang11d"
                                        { (yyval.blockNode) = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), (yyvsp[-3].argListNode), (yyvsp[-2].varListNode), (yyvsp[-1].node), false); }
#line 2352 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 36: /* cmdlinecode: OPENPAREN argdecls1 funcbody CLOSEPAREN  */
#line 247 "lang/LangSource/Bison/lang11d"
                                        { (yyval.blockNode) = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), (yyvsp[-2].argListNode), nullptr, (yyvsp[-1].node), false); }
#line 2358 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 37: /* cmdlinecode: OPENPAREN funcvardecls1 funcbody CLOSEPAREN  */
#line 249 "lang/LangSource/Bison/lang11d"
                                        { (yyval.blockNode) = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), nullptr, (yyvsp[-2].varListNode), (yyvsp[-1].node), false); }
#line 2364 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 38: /* cmdlinecode: argdecls1 funcvardecls1 funcbody  */
#line 251 "lang/LangSource/Bison/lang11d"
                                        { (yyval.blockNode) = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), (yyvsp[-2].argListNode), (yyvsp[-1].varListNode), (yyvsp[0].node), false); }
#line 2370 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 39: /* cmdlinecode: argdecls1 funcbody  */
#line 253 "lang/LangSource/Bison/lang11d"
                                        { (yyval.blockNode) = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), (yyvsp[-1].argListNode), nullptr, (yyvsp[0].node), false); }
#line 2376 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 40: /* cmdlinecode: funcvardecls1 funcbody  */
#line 255 "lang/LangSource/Bison/lang11d"
                                        { (yyval.blockNode) = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), nullptr, (yyvsp[-1].varListNode), (yyvsp[0].node), false); }
#line 2382 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 41: /* cmdlinecode: funcbody  */
#line 257 "lang/LangSource/Bison/lang11d"
                                        { (yyval.blockNode) = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), nullptr, nullptr, (yyvsp[0].node), false); }
#line 2388 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 43: /* methbody: exprseq retval  */
#line 261 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrDropNode>((yyloc), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2394 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 44: /* optPrim: %empty  */
#line 263 "lang/LangSource/Bison/lang11d"
                                { (yyval.slotNode) = nullptr; }
#line 2400 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 45: /* optPrim: PRIMITIVENAME optsemi  */
#line 264 "lang/LangSource/Bison/lang11d"
                                                        { (yyval.slotNode) = (yyvsp[-1].slotNode); }
#line 2406 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 46: /* retval: %empty  */
#line 266 "lang/LangSource/Bison/lang11d"
                                 { (yyval.node) = bison_cxt->allocParseNode<PyrReturnNode>((yyloc), nullptr); }
#line 2412 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 47: /* retval: NONLOCALRETURN expr optsemi  */
#line 268 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrReturnNode>((yyloc), (yyvsp[-1].node)); }
#line 2418 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 48: /* funretval: %empty  */
#line 271 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrBlockReturnNode>((yyloc), nullptr); }
#line 2424 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 49: /* funretval: NONLOCALRETURN expr optsemi  */
#line 273 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrReturnNode>((yyloc), (yyvsp[-1].node)); }
#line 2430 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 51: /* blocklist1: blocklist1 blocklistitem  */
#line 277 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = linkNodes((yyvsp[-1].node), (yyvsp[0].node)); }
#line 2436 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 54: /* blocklist: %empty  */
#line 281 "lang/LangSource/Bison/lang11d"
                                 { (yyval.node) = nullptr; }
#line 2442 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 56: /* msgsend: name blocklist1  */
#line 284 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), (yyvsp[-1].slotNode), (yyvsp[0].node), nullptr); }
#line 2448 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 57: /* msgsend: OPENPAREN binop2 CLOSEPAREN blocklist1  */
#line 287 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[0].node), nullptr); }
#line 2454 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 58: /* msgsend: name OPENPAREN CLOSEPAREN blocklist1  */
#line 290 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), (yyvsp[-3].slotNode), (yyvsp[0].node), nullptr); }
#line 2460 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 59: /* msgsend: name OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 293 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), (yyvsp[-5].slotNode), linkNodes((yyvsp[-3].node), (yyvsp[0].node)), (yyvsp[-2].node)); }
#line 2466 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 60: /* msgsend: OPENPAREN binop2 CLOSEPAREN OPENPAREN CLOSEPAREN blocklist1  */
#line 296 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), (yyvsp[-4].slotNode), (yyvsp[0].node), nullptr); }
#line 2472 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 61: /* msgsend: OPENPAREN binop2 CLOSEPAREN OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 299 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), (yyvsp[-6].slotNode), linkNodes((yyvsp[-3].node), (yyvsp[0].node)), (yyvsp[-2].node)); }
#line 2478 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 62: /* msgsend: name OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 302 "lang/LangSource/Bison/lang11d"
                                        {
						PyrSlot selector;
						// Is the keyword 'super' in 'msg(super, *[])'.
						if (isSuperObjNode((yyvsp[-2].node))) {
							auto* slot_node = nodeCast<PyrSlotNode>((yyvsp[-2].node));
							slot_node->mSlot = PyrSlot::make(s_this);
							selector = PyrSlot::make(s_superPerformList);
						} else {
							selector = PyrSlot::make(s_performList);
						}
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), selector);
						auto* args = linkAfterHead((yyvsp[-2].node), (yyvsp[-4].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>());
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, (yyvsp[-1].node));
					}
#line 2497 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 63: /* msgsend: OPENPAREN binop2 CLOSEPAREN OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 317 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_performList));
						auto* args = linkAfterHead((yyvsp[-2].node), (yyvsp[-5].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>());
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, (yyvsp[-1].node));
					}
#line 2507 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 64: /* msgsend: CLASSNAME OPENSQUARE arrayelems CLOSESQUARE  */
#line 323 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrDynListNode>((yyloc), (yyvsp[-3].slotNode), (yyvsp[-1].node)); }
#line 2513 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 65: /* msgsend: CLASSNAME blocklist1  */
#line 326 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_new));
						auto* args = linkNodes((yyvsp[-1].slotNode)->changeLiteralType<PyrParseNodeType::PushNameNode>(), (yyvsp[0].node));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 2523 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 66: /* msgsend: CLASSNAME OPENPAREN CLOSEPAREN blocklist  */
#line 332 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_new));
						auto* args = linkNodes((yyvsp[-3].slotNode)->changeLiteralType<PyrParseNodeType::PushNameNode>(), (yyvsp[0].node));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 2533 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 67: /* msgsend: CLASSNAME OPENPAREN keyarglist1 optcomma CLOSEPAREN blocklist  */
#line 338 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_new));
						auto* args = linkNodes((yyvsp[-5].slotNode)->changeLiteralType<PyrParseNodeType::PushNameNode>(), (yyvsp[0].node));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, (yyvsp[-3].node));
					}
#line 2543 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 68: /* msgsend: CLASSNAME OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 344 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_new));
						auto* args = linkNodes(
							(yyvsp[-5].slotNode)->changeLiteralType<PyrParseNodeType::PushNameNode>(), 
							(yyvsp[-3].node),
							(yyvsp[0].node));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, (yyvsp[-2].node));
					}
#line 2556 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 69: /* msgsend: CLASSNAME OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 353 "lang/LangSource/Bison/lang11d"
                                        {
						PyrSlot selector;
						if (isSuperObjNode((yyvsp[-4].slotNode))) {
							// Ehh? this is not possible. CLASSNAME cannot be 'super'.
							(yyvsp[-4].slotNode)->mSlot = PyrSlot::make(s_this);
							selector = PyrSlot::make(s_superPerformList);
						} else {
							selector = PyrSlot::make(s_performList);
						}

						auto* new_selector_push_lit = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_new), PyrParseNodeType::PushLitNode);
						auto* args = linkNodes(
							(yyvsp[-4].slotNode)->changeLiteralType<PyrParseNodeType::PushNameNode>(),
							new_selector_push_lit,
							(yyvsp[-2].node)
						);
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), selector);
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, (yyvsp[-1].node));
					}
#line 2580 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 70: /* msgsend: expr DOT OPENPAREN CLOSEPAREN blocklist  */
#line 373 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_value));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, linkNodes((yyvsp[-4].node), (yyvsp[0].node)), nullptr);
					}
#line 2589 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 71: /* msgsend: expr DOT OPENPAREN keyarglist1 optcomma CLOSEPAREN blocklist  */
#line 378 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_value));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, linkNodes((yyvsp[-6].node), (yyvsp[0].node)), (yyvsp[-3].node));
					}
#line 2598 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 72: /* msgsend: expr DOT name OPENPAREN keyarglist1 optcomma CLOSEPAREN blocklist  */
#line 383 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), (yyvsp[-5].slotNode), linkNodes((yyvsp[-7].node), (yyvsp[0].node)), (yyvsp[-3].node)); }
#line 2604 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 73: /* msgsend: expr DOT OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 385 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_value));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, linkNodes((yyvsp[-6].node), (yyvsp[-3].node), (yyvsp[0].node)), (yyvsp[-2].node));
					}
#line 2613 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 74: /* msgsend: expr DOT OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 390 "lang/LangSource/Bison/lang11d"
                                        {
						PyrSlot selectorSlot;
						if (isSuperObjNode((yyvsp[-5].node))) {
							nodeCast<PyrSlotNode>((yyvsp[-5].node))->mSlot = PyrSlot::make(s_this);
							selectorSlot = PyrSlot::make(s_superPerformList);
						} else {
							selectorSlot = PyrSlot::make(s_performList);
						}
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), selectorSlot);
						auto* args = linkNodes(
							(yyvsp[-5].node), 
							bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_value), PyrParseNodeType::PushLitNode),
							(yyvsp[-2].node));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, (yyvsp[-1].node));
					}
#line 2633 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 75: /* msgsend: expr DOT name OPENPAREN CLOSEPAREN blocklist  */
#line 406 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), (yyvsp[-3].slotNode), linkNodes((yyvsp[-5].node), (yyvsp[0].node)), nullptr); }
#line 2639 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 76: /* msgsend: expr DOT name OPENPAREN arglist1 optkeyarglist CLOSEPAREN blocklist  */
#line 408 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), (yyvsp[-5].slotNode), linkNodes((yyvsp[-7].node), (yyvsp[-3].node), (yyvsp[0].node)), (yyvsp[-2].node)); }
#line 2645 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 77: /* msgsend: expr DOT name OPENPAREN arglistv1 optkeyarglist CLOSEPAREN  */
#line 410 "lang/LangSource/Bison/lang11d"
                                        {
						PyrSlot slot;
						if (isSuperObjNode((yyvsp[-6].node))) {
							nodeCast<PyrSlotNode>((yyvsp[-6].node))->mSlot = PyrSlot::make(s_this);
							slot = PyrSlot::make(s_superPerformList);
						} else {
							slot = PyrSlot::make(s_performList);
						}
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), slot);
						auto* args = linkNodes((yyvsp[-6].node), (yyvsp[-4].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(), (yyvsp[-2].node));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, (yyvsp[-1].node));
					}
#line 2662 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 78: /* msgsend: expr DOT name blocklist  */
#line 423 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), (yyvsp[-1].slotNode), linkNodes((yyvsp[-3].node), (yyvsp[0].node)), nullptr); }
#line 2668 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 79: /* $@1: %empty  */
#line 431 "lang/LangSource/Bison/lang11d"
                                                  { bison_cxt->pushToGenerator((yyvsp[0].node), 1); }
#line 2674 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 80: /* generator: OPENCURLY COLON exprseq $@1 COMMA qual CLOSECURLY  */
#line 432 "lang/LangSource/Bison/lang11d"
                                        {
						PyrSlotNode* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("r")));
						PyrParseNode *block = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), nullptr, nullptr, (yyvsp[-1].node), false);
						PyrParseNode *blocklit = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(static_cast<void*>(block)), PyrParseNodeType::PushLitNode);
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, blocklit, nullptr);
					}
#line 2685 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 81: /* $@2: %empty  */
#line 438 "lang/LangSource/Bison/lang11d"
                                                              { bison_cxt->pushToGenerator((yyvsp[0].node), 2); }
#line 2691 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 82: /* generator: OPENCURLY SEMICOLON exprseq $@2 COMMA qual CLOSECURLY  */
#line 439 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = (yyvsp[-1].node); }
#line 2697 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 83: /* nextqual: %empty  */
#line 442 "lang/LangSource/Bison/lang11d"
                                        {
						// innermost part
						const auto [expr, action] = bison_cxt->popFromGenerator();
						if (action == 1) 
							(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("yield"))), expr, nullptr);
						else if (action == 2)
							(yyval.node) = expr; 
						// if action is neither, then what?
					}
#line 2711 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 84: /* nextqual: COMMA qual  */
#line 451 "lang/LangSource/Bison/lang11d"
                                             { (yyval.node) = (yyvsp[0].node); }
#line 2717 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 85: /* qual: name LEFTARROW exprseq nextqual  */
#line 454 "lang/LangSource/Bison/lang11d"
                                        {
						// later should check if exprseq is a series and optimize it to for loop
						
						auto* var = bison_cxt->allocParseNode<PyrVarDefNode>((yylsp[-3]), (yyvsp[-3].slotNode), nullptr, ReadWriteAccessor::Private);
						auto* args = bison_cxt->allocParseNode<PyrArgListNode>((yylsp[-3]), var, nullptr, nullptr);
						auto *block = bison_cxt->allocParseNode<PyrBlockNode>((yylsp[-3]), args, nullptr, (yyvsp[0].node), false);
						auto *blocklit = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(static_cast<void*>(block)), PyrParseNodeType::PushLitNode);
						if (auto* call = nodeCast<PyrCallNode>((yyvsp[-1].node)); call && call->mSelector->mSlot.getSymbol() == s_series) {
								call->mSelector->mSlot = PyrSlot::make(getsym("forSeries"));
								call->mArglist = linkNodes(call->mArglist, blocklit);
								(yyval.node) = call;
						} else {
							auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yylsp[-1]), PyrSlot::make(getsym("do")));
							(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, linkNodes((yyvsp[-1].node), blocklit), nullptr);
						}
					}
#line 2738 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 86: /* qual: name name LEFTARROW exprseq nextqual  */
#line 471 "lang/LangSource/Bison/lang11d"
                                        {
						auto* call = nodeCast<PyrCallNode>((yyvsp[-1].node));
						const auto is_series = call ? (call->mSelector->mSlot.getSymbol() == s_series) : false;

						auto* var1 = bison_cxt->allocParseNode<PyrVarDefNode>((yyloc), (yyvsp[-4].slotNode), nullptr, ReadWriteAccessor::Private);
						auto* var2 = bison_cxt->allocParseNode<PyrVarDefNode>((yyloc), (yyvsp[-3].slotNode), nullptr, ReadWriteAccessor::Private);
						auto* args = bison_cxt->allocParseNode<PyrArgListNode>((yyloc), linkNodes(var1, var2), nullptr, nullptr);
						auto *block = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), args, nullptr, (yyvsp[0].node), false);
						auto *blocklit = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(static_cast<void*>(block)), PyrParseNodeType::PushLitNode);

						if (auto* call = nodeCast<PyrCallNode>((yyvsp[-2].slotNode)); call && call->mSelector->mSlot.getSymbol() == s_series) {
							call->mSelector->mSlot = PyrSlot::make(getsym("forSeries"));
							call->mArglist = linkNodes(call->mArglist, blocklit);
							(yyval.node) = call;
						} else {
							auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yylsp[-2]), PyrSlot::make(getsym("do")));
							(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, linkNodes((yyvsp[-1].node), blocklit), nullptr);
						}
					}
#line 2762 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 87: /* qual: VAR name EQUALSSIGN exprseq nextqual  */
#line 491 "lang/LangSource/Bison/lang11d"
                                        {
						PyrSlot slot = PyrSlot::make(s_value);
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), slot);
						auto* var = bison_cxt->allocParseNode<PyrVarDefNode>((yyloc), (yyvsp[-3].slotNode), nullptr, ReadWriteAccessor::Private);
						auto* args = bison_cxt->allocParseNode<PyrArgListNode>((yyloc), var, nullptr, nullptr);
						auto *block = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), args, nullptr, (yyvsp[0].node), false);
						auto *blocklit = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(static_cast<void*>(block)), PyrParseNodeType::PushLitNode);
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, linkNodes(blocklit, (yyvsp[-1].node)), nullptr);
					}
#line 2776 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 88: /* qual: exprseq nextqual  */
#line 501 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("if")));
						auto* block = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), nullptr, nullptr, (yyvsp[0].node), false);
						auto* blocklit = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(static_cast<void*>(block)), PyrParseNodeType::PushLitNode);
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, linkNodes((yyvsp[-1].node), blocklit), nullptr);
					}
#line 2787 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 89: /* qual: COLON COLON exprseq nextqual  */
#line 508 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrDropNode>((yyloc), (yyvsp[-1].node), (yyvsp[0].node)); }
#line 2793 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 90: /* qual: COLON WHILE exprseq nextqual  */
#line 510 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode1 = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("alwaysYield")));
						auto* pushnil = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode);
						auto* yieldNil = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode1, pushnil, nullptr);
						auto* block1 = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), nullptr, nullptr, yieldNil, false);
						auto* blocklit1 = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(static_cast<void*>(block1)), PyrParseNodeType::PushLitNode);
						auto* block2 = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), nullptr, nullptr, (yyvsp[0].node), false);
						auto* blocklit2 = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(static_cast<void*>(block2)), PyrParseNodeType::PushLitNode);

						auto* selectornode2 = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("if")));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode2, linkNodes((yyvsp[-1].node), blocklit2, blocklit1), nullptr);
					}
#line 2810 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 97: /* expr1: OPENPAREN exprseq CLOSEPAREN  */
#line 530 "lang/LangSource/Bison/lang11d"
                                        {
						PyrParseNode* node = (yyvsp[-1].node);
						node->mParens = true; // This means  (((((1))))), will not count the brackets, but just indicates they exist.
						node->location = (yyloc); // make the location include the brackets.
						(yyval.node) = (yyvsp[-1].node);
					}
#line 2821 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 98: /* expr1: TILDE name  */
#line 537 "lang/LangSource/Bison/lang11d"
                                        {
						auto* argnode = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>();
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_envirGet));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, argnode, nullptr);
					}
#line 2831 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 99: /* expr1: OPENSQUARE arrayelems CLOSESQUARE  */
#line 543 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrDynListNode>((yyloc), nullptr, (yyvsp[-1].node)); }
#line 2837 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 100: /* expr1: OPENPAREN valrange2 CLOSEPAREN  */
#line 545 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = (yyvsp[-1].node); }
#line 2843 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 101: /* expr1: OPENPAREN COLON valrange3 CLOSEPAREN  */
#line 547 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = (yyvsp[-1].node); }
#line 2849 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 102: /* expr1: OPENPAREN dictslotlist CLOSEPAREN  */
#line 549 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrDynDictNode>((yyloc), (yyvsp[-1].node)); }
#line 2855 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 103: /* expr1: expr1 OPENSQUARE arglist1 CLOSESQUARE  */
#line 551 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_at));
						auto* args = linkNodes((yyvsp[-3].node), (yyvsp[-1].node));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 2865 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 105: /* valrangex1: expr1 OPENSQUARE arglist1 DOTDOT CLOSESQUARE  */
#line 559 "lang/LangSource/Bison/lang11d"
                                        {
						const auto arglen = nodeListLength((yyvsp[-2].node));
						checkArraySubrangeSyntax((yyvsp[-2].node), arglen);
						auto* args = linkNodes((yyvsp[-4].node), (yyvsp[-2].node));
						if (arglen < 2) 
							args = linkNodes(args, bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode));

						args = linkNodes(args, bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode));
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_copyseries));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 2881 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 106: /* valrangex1: expr1 OPENSQUARE DOTDOT exprseq CLOSESQUARE  */
#line 571 "lang/LangSource/Bison/lang11d"
                                        {
						auto* nilnode1 = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode);
						auto* args = linkNodes((yyvsp[-4].node), nilnode1);
						auto* nilnode2 = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode);
						args = linkNodes(args, nilnode2);
						args = linkNodes(args, (yyvsp[-1].node));
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_copyseries));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 2895 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 107: /* valrangex1: expr1 OPENSQUARE arglist1 DOTDOT exprseq CLOSESQUARE  */
#line 581 "lang/LangSource/Bison/lang11d"
                                        {
						const int arglen = nodeListLength((yyvsp[-3].node));
						checkArraySubrangeSyntax((yyvsp[-3].node), arglen);

						auto* args = linkNodes((yyvsp[-5].node), (yyvsp[-3].node));
						if (arglen < 2) 
							args = linkNodes(args, bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode));
						
						args = linkNodes(args, (yyvsp[-1].node));
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_copyseries));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 2912 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 108: /* valrangeassign: expr1 OPENSQUARE arglist1 DOTDOT CLOSESQUARE EQUALSSIGN expr  */
#line 595 "lang/LangSource/Bison/lang11d"
                                        {
						const int arglen = nodeListLength((yyvsp[-4].node));
						checkArraySubrangeSyntax((yyvsp[-4].node), arglen);
						
						auto* args = linkNodes((yyvsp[-6].node), (yyvsp[-4].node));
						if (arglen < 2) 
							args = linkNodes(args, bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode));
						
						args = linkNodes(args, bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode), (yyvsp[0].node));
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_putseries));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 2929 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 109: /* valrangeassign: expr1 OPENSQUARE DOTDOT exprseq CLOSESQUARE EQUALSSIGN expr  */
#line 608 "lang/LangSource/Bison/lang11d"
                                        {
						auto* args = linkNodes(
							(yyvsp[-6].node),
							bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode),
							bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode),
							(yyvsp[-3].node), 
							(yyvsp[0].node)
						);
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_putseries));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 2945 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 110: /* valrangeassign: expr1 OPENSQUARE arglist1 DOTDOT exprseq CLOSESQUARE EQUALSSIGN expr  */
#line 620 "lang/LangSource/Bison/lang11d"
                                        {
						const int arglen = nodeListLength((yyvsp[-5].node));
						auto* args = linkNodes((yyvsp[-7].node), (yyvsp[-5].node));
						if (arglen < 2) 
							args = linkNodes(args, bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode));
						
						args = linkNodes(args, (yyvsp[-3].node), (yyvsp[0].node));
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_putseries));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 2960 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 111: /* valrangexd: expr DOT OPENSQUARE arglist1 DOTDOT CLOSESQUARE  */
#line 637 "lang/LangSource/Bison/lang11d"
                                        {
						PyrSlotNode* nilnode1, *nilnode2;
						PyrSlot selectorSlot, nilSlot;
						PyrParseNode* args;

						const int arglen = nodeListLength((yyvsp[-2].node));

						checkArraySubrangeSyntax((yyvsp[-2].node), arglen);

						args = linkNodes((yyvsp[-5].node), (yyvsp[-2].node));
						if (arglen < 2) 
							args = linkNodes(args, bison_cxt->allocParseNode<PyrSlotNode>((yyloc), nilSlot, PyrParseNodeType::PushLitNode));

						args = linkNodes(args, bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode));

						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_copyseries));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 2983 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 112: /* valrangexd: expr DOT OPENSQUARE DOTDOT exprseq CLOSESQUARE  */
#line 656 "lang/LangSource/Bison/lang11d"
                                        {
						auto* args = linkNodes(
							(yyvsp[-5].node),
							bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode),
							bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode),
							(yyvsp[-1].node)
						);
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_copyseries));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 2998 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 113: /* valrangexd: expr DOT OPENSQUARE arglist1 DOTDOT exprseq CLOSESQUARE  */
#line 667 "lang/LangSource/Bison/lang11d"
                                        {
						const int arglen = nodeListLength((yyvsp[-3].node));

						checkArraySubrangeSyntax((yyvsp[-3].node), arglen);

						auto* args = linkNodes((yyvsp[-6].node), (yyvsp[-3].node));
						if (arglen < 2) 
							args = linkNodes(args, bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode));
						
						args = linkNodes(args, (yyvsp[-1].node));
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_copyseries));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3016 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 114: /* valrangexd: expr DOT OPENSQUARE arglist1 DOTDOT CLOSESQUARE EQUALSSIGN expr  */
#line 681 "lang/LangSource/Bison/lang11d"
                                        {
						const int arglen = nodeListLength((yyvsp[-4].node));
						checkArraySubrangeSyntax((yyvsp[-4].node), arglen);

						auto* args = linkNodes((yyvsp[-7].node), (yyvsp[-4].node));
						if (arglen < 2) 
							args = linkNodes(args, bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode));
						
						args = linkNodes(args, bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode), (yyvsp[0].node));
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_putseries));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3033 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 115: /* valrangexd: expr DOT OPENSQUARE DOTDOT exprseq CLOSESQUARE EQUALSSIGN expr  */
#line 694 "lang/LangSource/Bison/lang11d"
                                        {
						auto* args = linkNodes((yyvsp[-7].node), bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode));
						args = linkNodes(args, bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode));
						args = linkNodes(args, (yyvsp[-3].node));
						args = linkNodes(args, (yyvsp[0].node));
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_putseries));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3046 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 116: /* valrangexd: expr DOT OPENSQUARE arglist1 DOTDOT exprseq CLOSESQUARE EQUALSSIGN expr  */
#line 703 "lang/LangSource/Bison/lang11d"
                                        {
						const int arglen = nodeListLength((yyvsp[-5].node));
						checkArraySubrangeSyntax((yyvsp[-5].node), arglen);

						auto* args = linkNodes((yyvsp[-8].node), (yyvsp[-5].node));
						if (arglen < 2) 
							args = linkNodes(args, bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode));
						
						args = linkNodes(args, (yyvsp[-3].node), (yyvsp[0].node));
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_putseries));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3063 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 117: /* valrange2: exprseq DOTDOT  */
#line 717 "lang/LangSource/Bison/lang11d"
                                        {
						// if this is not used in a 'do' or list comprehension, then should return an error.
						auto* args = linkNodes((yyvsp[-1].node), bison_cxt->allocParseNode<PyrSlotNode>((yylsp[0]), PyrSlot{}, PyrParseNodeType::PushLitNode));
						args = linkNodes(args, bison_cxt->allocParseNode<PyrSlotNode>((yylsp[0]), PyrSlot{}, PyrParseNodeType::PushLitNode));
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_series));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3075 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 118: /* valrange2: DOTDOT exprseq  */
#line 725 "lang/LangSource/Bison/lang11d"
                                        {
						auto* args = linkNodes(
							bison_cxt->allocParseNode<PyrSlotNode>((yylsp[-1]), PyrSlot::make(0), PyrParseNodeType::PushLitNode),
							bison_cxt->allocParseNode<PyrSlotNode>((yylsp[-1]), PyrSlot{}, PyrParseNodeType::PushLitNode),
							(yyvsp[0].node)
						);
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_series));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3089 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 119: /* valrange2: exprseq DOTDOT exprseq  */
#line 735 "lang/LangSource/Bison/lang11d"
                                        {
						auto* args = linkNodes((yyvsp[-2].node), bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode), (yyvsp[0].node));
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_series));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3099 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 120: /* valrange2: exprseq COMMA exprseq DOTDOT exprseq  */
#line 741 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_series));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, linkNodes((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)), nullptr);
					}
#line 3108 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 121: /* valrange2: exprseq COMMA exprseq DOTDOT  */
#line 746 "lang/LangSource/Bison/lang11d"
                                        {
						// if this is not used in a 'do' or list comprehension, then should return an error.
						auto* args = linkNodes((yyvsp[-3].node), (yyvsp[-1].node), bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot{}, PyrParseNodeType::PushLitNode));
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_series));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3119 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 122: /* valrange3: DOTDOT exprseq  */
#line 754 "lang/LangSource/Bison/lang11d"
                                        {
						auto* args = linkNodes(
							bison_cxt->allocParseNode<PyrSlotNode>((yylsp[-1]), PyrSlot::make(0), PyrParseNodeType::PushLitNode),
							bison_cxt->allocParseNode<PyrSlotNode>((yylsp[-1]), PyrSlot{}, PyrParseNodeType::PushLitNode),
							(yyvsp[0].node)
						);
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("seriesIter")));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3133 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 123: /* valrange3: exprseq DOTDOT  */
#line 764 "lang/LangSource/Bison/lang11d"
                                        {
						auto* args = linkNodes(
							(yyvsp[-1].node),
							bison_cxt->allocParseNode<PyrSlotNode>((yylsp[-1]), PyrSlot{}, PyrParseNodeType::PushLitNode),
							bison_cxt->allocParseNode<PyrSlotNode>((yylsp[-1]), PyrSlot{}, PyrParseNodeType::PushLitNode)
						);
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("seriesIter")));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3147 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 124: /* valrange3: exprseq DOTDOT exprseq  */
#line 775 "lang/LangSource/Bison/lang11d"
                                        {
						auto* args = linkNodes(
							(yyvsp[-2].node),
							bison_cxt->allocParseNode<PyrSlotNode>((yylsp[-1]), PyrSlot{}, PyrParseNodeType::PushLitNode),
							(yyvsp[0].node)
						);
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("seriesIter")));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3161 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 125: /* valrange3: exprseq COMMA exprseq DOTDOT  */
#line 785 "lang/LangSource/Bison/lang11d"
                                        {
						auto* args = linkNodes(
							(yyvsp[-3].node),
							(yyvsp[-1].node),
							bison_cxt->allocParseNode<PyrSlotNode>((yylsp[-2]), PyrSlot{}, PyrParseNodeType::PushLitNode)
						);
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("seriesIter")));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3175 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 126: /* valrange3: exprseq COMMA exprseq DOTDOT exprseq  */
#line 795 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(getsym("seriesIter")));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, linkNodes((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)), nullptr);
					}
#line 3184 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 130: /* expr: CLASSNAME  */
#line 804 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::PushNameNode>(); }
#line 3190 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 131: /* expr: expr DOT OPENSQUARE arglist1 CLOSESQUARE  */
#line 806 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_at));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, linkNodes((yyvsp[-4].node), (yyvsp[-1].node)), nullptr);
					}
#line 3199 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 132: /* expr: BACKTICK expr  */
#line 811 "lang/LangSource/Bison/lang11d"
                                        {
						auto* args = linkNodes(
							bison_cxt->allocParseNode<PyrSlotNode>((yylsp[-1]), PyrSlot::make(s_ref), PyrParseNodeType::PushNameNode),
							(yyvsp[0].node)
						);
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_new));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3212 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 133: /* expr: expr binop2 adverb expr  */
#line 820 "lang/LangSource/Bison/lang11d"
                                        {
						(yyvsp[-3].node)->mNext = (yyvsp[0].node);
						(yyvsp[0].node)->mNext = (yyvsp[-1].node);
						(yyval.node) = bison_cxt->allocParseNode<PyrBinopCallNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[-3].node));
					}
#line 3222 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 134: /* expr: name EQUALSSIGN expr  */
#line 826 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrAssignNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[0].node)); }
#line 3228 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 135: /* expr: TILDE name EQUALSSIGN expr  */
#line 828 "lang/LangSource/Bison/lang11d"
                                        {
						auto* args = linkNodes((yyvsp[-2].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(), (yyvsp[0].node));
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_envirPut));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, args, nullptr);
					}
#line 3238 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 136: /* expr: expr DOT name EQUALSSIGN expr  */
#line 834 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrSetterNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[-4].node), (yyvsp[0].node)); }
#line 3244 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 137: /* expr: name OPENPAREN arglist1 optkeyarglist CLOSEPAREN EQUALSSIGN expr  */
#line 838 "lang/LangSource/Bison/lang11d"
                                        {
						if ((yyvsp[-3].node) != nullptr) {
							bison_cxt->postErrorInCurrentFile((yyvsp[-3].node)->location, "Setter method call with keyword arguments", "delete these keyword arguments.");
						}
						(yyval.node) = bison_cxt->allocParseNode<PyrSetterNode>((yyloc), (yyvsp[-6].slotNode), (yyvsp[-4].node), (yyvsp[0].node));
					}
#line 3255 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 138: /* expr: HASH mavars EQUALSSIGN expr  */
#line 845 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrMultiAssignNode>((yyloc), (yyvsp[-2].multiAssignListNode), (yyvsp[0].node)); }
#line 3261 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 139: /* expr: expr1 OPENSQUARE arglist1 CLOSESQUARE EQUALSSIGN expr  */
#line 847 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_put));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, linkNodes((yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[0].node)), nullptr);
					}
#line 3270 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 140: /* expr: expr DOT OPENSQUARE arglist1 CLOSESQUARE EQUALSSIGN expr  */
#line 852 "lang/LangSource/Bison/lang11d"
                                        {
						auto* selectornode = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(s_put));
						(yyval.node) = bison_cxt->allocParseNode<PyrCallNode>((yyloc), selectornode, linkNodes((yyvsp[-6].node), (yyvsp[-3].node), (yyvsp[0].node)), nullptr);
					}
#line 3279 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 141: /* adverb: %empty  */
#line 857 "lang/LangSource/Bison/lang11d"
                                  { (yyval.node) = nullptr; }
#line 3285 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 142: /* adverb: DOT name  */
#line 858 "lang/LangSource/Bison/lang11d"
                                           { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(); }
#line 3291 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 143: /* adverb: DOT integer  */
#line 859 "lang/LangSource/Bison/lang11d"
                                              { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(); }
#line 3297 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 144: /* adverb: DOT OPENPAREN exprseq CLOSEPAREN  */
#line 860 "lang/LangSource/Bison/lang11d"
                                                                   { (yyval.node) = (yyvsp[-1].node); }
#line 3303 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 146: /* exprn: exprn SEMICOLON expr  */
#line 864 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrDropNode>((yyloc), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3309 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 148: /* arrayelems: %empty  */
#line 868 "lang/LangSource/Bison/lang11d"
                                 { (yyval.node) = nullptr; }
#line 3315 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 149: /* arrayelems: arrayelems1 optcomma  */
#line 869 "lang/LangSource/Bison/lang11d"
                                                       { (yyval.node) = (yyvsp[-1].node); }
#line 3321 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 151: /* arrayelems1: exprseq COLON exprseq  */
#line 873 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = linkNodes((yyvsp[-2].node), (yyvsp[0].node)); }
#line 3327 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 152: /* arrayelems1: KEYBINOP exprseq  */
#line 875 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = linkNodes( (yyvsp[-1].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(), (yyvsp[0].node)); }
#line 3333 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 153: /* arrayelems1: arrayelems1 COMMA exprseq  */
#line 877 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = linkNodes((yyvsp[-2].node), (yyvsp[0].node)); }
#line 3339 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 154: /* arrayelems1: arrayelems1 COMMA KEYBINOP exprseq  */
#line 879 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = linkNodes((yyvsp[-3].node), (yyvsp[-1].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(), (yyvsp[0].node)); }
#line 3345 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 155: /* arrayelems1: arrayelems1 COMMA exprseq COLON exprseq  */
#line 881 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = linkNodes((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 3351 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 157: /* arglist1: arglist1 COMMA exprseq  */
#line 885 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = linkNodes((yyvsp[-2].node), (yyvsp[0].node)); }
#line 3357 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 158: /* arglistv1: MULTIPLY exprseq  */
#line 888 "lang/LangSource/Bison/lang11d"
                                        { (yylsp[0]) = (yyloc); (yyval.node) = (yyvsp[0].node); }
#line 3363 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 159: /* arglistv1: arglist1 COMMA MULTIPLY exprseq  */
#line 890 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = linkNodes((yyvsp[-3].node), (yyvsp[0].node)); }
#line 3369 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 161: /* keyarglist1: keyarglist1 COMMA keyarg  */
#line 894 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = linkNodes((yyvsp[-2].node), (yyvsp[0].node)); }
#line 3375 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 162: /* keyarg: KEYBINOP exprseq  */
#line 897 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrPushKeyArgNode>((yyloc), (yyvsp[-1].slotNode), (yyvsp[0].node)); }
#line 3381 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 163: /* optkeyarglist: optcomma  */
#line 899 "lang/LangSource/Bison/lang11d"
                           { (yyval.node) = nullptr; }
#line 3387 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 164: /* optkeyarglist: COMMA keyarglist1 optcomma  */
#line 900 "lang/LangSource/Bison/lang11d"
                                                             { (yyval.node) = (yyvsp[-1].node); }
#line 3393 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 165: /* mavars: nameList  */
#line 903 "lang/LangSource/Bison/lang11d"
                                        { (yyval.multiAssignListNode) = bison_cxt->allocParseNode<PyrMultiAssignVarListNode>((yyloc), (yyvsp[0].slotNode), nullptr); }
#line 3399 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 166: /* mavars: nameList ELLIPSIS name  */
#line 905 "lang/LangSource/Bison/lang11d"
                                        { (yyval.multiAssignListNode) = bison_cxt->allocParseNode<PyrMultiAssignVarListNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[0].slotNode)); }
#line 3405 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 168: /* nameList: nameList COMMA name  */
#line 909 "lang/LangSource/Bison/lang11d"
                                        { (yyval.slotNode) = linkNodes((yyvsp[-2].slotNode), (yyvsp[0].slotNode)); }
#line 3411 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 169: /* slotliteral: integer  */
#line 911 "lang/LangSource/Bison/lang11d"
                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3417 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 170: /* slotliteral: floatp  */
#line 912 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3423 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 171: /* slotliteral: ASCII  */
#line 913 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3429 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 172: /* slotliteral: STRING  */
#line 914 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3435 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 173: /* slotliteral: SYMBOL  */
#line 915 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3441 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 174: /* slotliteral: TRUEOBJ  */
#line 916 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3447 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 175: /* slotliteral: FALSEOBJ  */
#line 917 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3453 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 176: /* slotliteral: NILOBJ  */
#line 918 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3459 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 177: /* slotliteral: listlit  */
#line 919 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(static_cast<void*>((yyvsp[0].node))), PyrParseNodeType::LiteralNode); }
#line 3465 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 178: /* blockliteral: block  */
#line 921 "lang/LangSource/Bison/lang11d"
                        { (yyval.node) = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(static_cast<void*>((yyvsp[0].node))), PyrParseNodeType::PushLitNode); }
#line 3471 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 179: /* pushname: name  */
#line 923 "lang/LangSource/Bison/lang11d"
                               { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::PushNameNode>(); }
#line 3477 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 180: /* pushliteral: integer  */
#line 925 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(); }
#line 3483 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 181: /* pushliteral: floatp  */
#line 926 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(); }
#line 3489 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 182: /* pushliteral: ASCII  */
#line 927 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(); }
#line 3495 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 183: /* pushliteral: STRING  */
#line 928 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(); }
#line 3501 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 184: /* pushliteral: SYMBOL  */
#line 929 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(); }
#line 3507 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 185: /* pushliteral: TRUEOBJ  */
#line 930 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(); }
#line 3513 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 186: /* pushliteral: FALSEOBJ  */
#line 931 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(); }
#line 3519 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 187: /* pushliteral: NILOBJ  */
#line 932 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(); }
#line 3525 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 188: /* pushliteral: listlit  */
#line 933 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(static_cast<void*>((yyvsp[0].node))), PyrParseNodeType::PushLitNode); }
#line 3531 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 189: /* listliteral: integer  */
#line 935 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>();}
#line 3537 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 190: /* listliteral: floatp  */
#line 936 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3543 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 191: /* listliteral: ASCII  */
#line 937 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3549 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 192: /* listliteral: STRING  */
#line 938 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3555 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 193: /* listliteral: SYMBOL  */
#line 939 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3561 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 194: /* listliteral: name  */
#line 940 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3567 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 195: /* listliteral: TRUEOBJ  */
#line 941 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3573 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 196: /* listliteral: FALSEOBJ  */
#line 942 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3579 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 197: /* listliteral: NILOBJ  */
#line 943 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = (yyvsp[0].slotNode)->changeLiteralType<PyrParseNodeType::LiteralNode>(); }
#line 3585 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 198: /* listliteral: listlit2  */
#line 944 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = bison_cxt->allocParseNode<PyrSlotNode>((yyloc), PyrSlot::make(static_cast<void*>((yyvsp[0].node))), PyrParseNodeType::LiteralNode); }
#line 3591 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 199: /* block: OPENCURLY argdecls funcvardecls funcbody CLOSECURLY  */
#line 947 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), (yyvsp[-3].argListNode), (yyvsp[-2].varListNode), (yyvsp[-1].node), false); }
#line 3597 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 200: /* block: BEGINCLOSEDFUNC argdecls funcvardecls funcbody CLOSECURLY  */
#line 949 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrBlockNode>((yyloc), (yyvsp[-3].argListNode), (yyvsp[-2].varListNode), (yyvsp[-1].node), true); }
#line 3603 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 201: /* funcvardecls: %empty  */
#line 951 "lang/LangSource/Bison/lang11d"
                         { (yyval.varListNode) = nullptr; }
#line 3609 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 202: /* funcvardecls: funcvardecls funcvardecl  */
#line 953 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varListNode) = linkNodes((yyvsp[-1].varListNode), (yyvsp[0].varListNode)); }
#line 3615 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 204: /* funcvardecls1: funcvardecls1 funcvardecl  */
#line 957 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varListNode) = linkNodes((yyvsp[-1].varListNode), (yyvsp[0].varListNode)); }
#line 3621 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 205: /* funcvardecl: VAR vardeflist SEMICOLON  */
#line 960 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varListNode) = bison_cxt->allocParseNode<PyrVarListNode>((yyloc), (yyvsp[-1].varDefNode), varLocal); }
#line 3627 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 206: /* argdecls: %empty  */
#line 962 "lang/LangSource/Bison/lang11d"
                                 { (yyval.argListNode) = nullptr; }
#line 3633 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 207: /* argdecls: ARG vardeflist SEMICOLON  */
#line 964 "lang/LangSource/Bison/lang11d"
                                        { (yyval.argListNode) = bison_cxt->allocParseNode<PyrArgListNode>((yyloc), (yyvsp[-1].varDefNode), nullptr, nullptr); }
#line 3639 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 208: /* argdecls: ARG vardeflist0 ELLIPSIS name SEMICOLON  */
#line 966 "lang/LangSource/Bison/lang11d"
                                        { (yyval.argListNode) = bison_cxt->allocParseNode<PyrArgListNode>((yyloc), (yyvsp[-3].varDefNode), (yyvsp[-1].slotNode), nullptr); }
#line 3645 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 209: /* argdecls: ARG vardeflist0 ELLIPSIS name COMMA name SEMICOLON  */
#line 968 "lang/LangSource/Bison/lang11d"
                                        { (yyval.argListNode) = bison_cxt->allocParseNode<PyrArgListNode>((yyloc), (yyvsp[-5].varDefNode), (yyvsp[-3].slotNode), (yyvsp[-1].slotNode)); }
#line 3651 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 210: /* argdecls: PIPE slotdeflist PIPE  */
#line 970 "lang/LangSource/Bison/lang11d"
                                        { (yyval.argListNode) = bison_cxt->allocParseNode<PyrArgListNode>((yyloc), (yyvsp[-1].varDefNode), nullptr, nullptr); }
#line 3657 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 211: /* argdecls: PIPE slotdeflist0 ELLIPSIS name PIPE  */
#line 972 "lang/LangSource/Bison/lang11d"
                                        { (yyval.argListNode) = bison_cxt->allocParseNode<PyrArgListNode>((yyloc), (yyvsp[-3].varDefNode), (yyvsp[-1].slotNode), nullptr); }
#line 3663 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 212: /* argdecls: PIPE slotdeflist0 ELLIPSIS name COMMA name PIPE  */
#line 974 "lang/LangSource/Bison/lang11d"
                                    { (yyval.argListNode) = bison_cxt->allocParseNode<PyrArgListNode>((yyloc), (yyvsp[-5].varDefNode), (yyvsp[-3].slotNode), (yyvsp[-1].slotNode)); }
#line 3669 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 213: /* argdecls1: ARG vardeflist SEMICOLON  */
#line 977 "lang/LangSource/Bison/lang11d"
                                        { (yyval.argListNode) = bison_cxt->allocParseNode<PyrArgListNode>((yyloc), (yyvsp[-1].varDefNode), nullptr, nullptr); }
#line 3675 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 214: /* argdecls1: ARG vardeflist0 ELLIPSIS name SEMICOLON  */
#line 979 "lang/LangSource/Bison/lang11d"
                                        { (yyval.argListNode) = bison_cxt->allocParseNode<PyrArgListNode>((yyloc), (yyvsp[-3].varDefNode), (yyvsp[-1].slotNode), nullptr); }
#line 3681 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 215: /* argdecls1: ARG vardeflist0 ELLIPSIS name COMMA name SEMICOLON  */
#line 981 "lang/LangSource/Bison/lang11d"
                                        { (yyval.argListNode) = bison_cxt->allocParseNode<PyrArgListNode>((yyloc), (yyvsp[-5].varDefNode), (yyvsp[-3].slotNode), (yyvsp[-1].slotNode)); }
#line 3687 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 216: /* argdecls1: PIPE slotdeflist PIPE  */
#line 983 "lang/LangSource/Bison/lang11d"
                                        { (yyval.argListNode) = bison_cxt->allocParseNode<PyrArgListNode>((yyloc), (yyvsp[-1].varDefNode), nullptr, nullptr); }
#line 3693 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 217: /* argdecls1: PIPE slotdeflist0 ELLIPSIS name PIPE  */
#line 985 "lang/LangSource/Bison/lang11d"
                                        { (yyval.argListNode) = bison_cxt->allocParseNode<PyrArgListNode>((yyloc), (yyvsp[-3].varDefNode), (yyvsp[-1].slotNode), nullptr); }
#line 3699 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 218: /* argdecls1: PIPE slotdeflist0 ELLIPSIS name COMMA name PIPE  */
#line 987 "lang/LangSource/Bison/lang11d"
                                    { (yyval.argListNode) = bison_cxt->allocParseNode<PyrArgListNode>((yyloc), (yyvsp[-5].varDefNode), (yyvsp[-3].slotNode), (yyvsp[-1].slotNode)); }
#line 3705 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 220: /* constdeflist: constdeflist optcomma constdef  */
#line 991 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varDefNode) = linkNodes((yyvsp[-2].varDefNode), (yyvsp[0].varDefNode)); }
#line 3711 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 221: /* constdef: rspec name EQUALSSIGN slotliteral  */
#line 994 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varDefNode) = bison_cxt->allocParseNode<PyrVarDefNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[0].node), (yyvsp[-3].rwAccessor)); }
#line 3717 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 222: /* slotdeflist0: %empty  */
#line 996 "lang/LangSource/Bison/lang11d"
                         { (yyval.varDefNode) = nullptr; }
#line 3723 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 225: /* slotdeflist: slotdeflist optcomma slotdef  */
#line 1001 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varDefNode) = linkNodes((yyvsp[-2].varDefNode), (yyvsp[0].varDefNode)); }
#line 3729 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 226: /* slotdef: name  */
#line 1004 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varDefNode) = bison_cxt->allocParseNode<PyrVarDefNode>((yyloc), (yyvsp[0].slotNode), nullptr, ReadWriteAccessor::Private); }
#line 3735 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 227: /* slotdef: name optequal slotliteral  */
#line 1006 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varDefNode) = bison_cxt->allocParseNode<PyrVarDefNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[0].node), ReadWriteAccessor::Private); }
#line 3741 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 228: /* slotdef: name optequal OPENPAREN exprseq CLOSEPAREN  */
#line 1008 "lang/LangSource/Bison/lang11d"
                                        {
						PyrParseNode* node = (yyvsp[-1].node);
						node->mParens = 1;
						(yyval.varDefNode) = bison_cxt->allocParseNode<PyrVarDefNode>((yyloc), (yyvsp[-4].slotNode), node, ReadWriteAccessor::Private);
					}
#line 3751 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 229: /* vardeflist0: %empty  */
#line 1014 "lang/LangSource/Bison/lang11d"
                          { (yyval.varDefNode) = nullptr; }
#line 3757 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 232: /* vardeflist: vardeflist COMMA vardef  */
#line 1019 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varDefNode) = linkNodes((yyvsp[-2].varDefNode), (yyvsp[0].varDefNode)); }
#line 3763 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 233: /* vardef: name  */
#line 1022 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varDefNode) = bison_cxt->allocParseNode<PyrVarDefNode>((yyloc), (yyvsp[0].slotNode), nullptr, ReadWriteAccessor::Private); }
#line 3769 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 234: /* vardef: name EQUALSSIGN expr  */
#line 1024 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varDefNode) = bison_cxt->allocParseNode<PyrVarDefNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[0].node), ReadWriteAccessor::Private); }
#line 3775 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 235: /* vardef: name OPENPAREN exprseq CLOSEPAREN  */
#line 1026 "lang/LangSource/Bison/lang11d"
                                        {
						PyrParseNode* node = (yyvsp[-1].node);
						node->mParens = 1;
						(yyval.varDefNode) = bison_cxt->allocParseNode<PyrVarDefNode>((yyloc), (yyvsp[-3].slotNode), node, ReadWriteAccessor::Private);
					}
#line 3785 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 236: /* dictslotdef: exprseq COLON exprseq  */
#line 1033 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = linkNodes((yyvsp[-2].node), (yyvsp[0].node)); }
#line 3791 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 237: /* dictslotdef: KEYBINOP exprseq  */
#line 1035 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = linkNodes((yyvsp[-1].slotNode)->changeLiteralType<PyrParseNodeType::PushLitNode>(), (yyvsp[0].node)); }
#line 3797 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 239: /* dictslotlist1: dictslotlist1 COMMA dictslotdef  */
#line 1039 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = linkNodes((yyvsp[-2].node), (yyvsp[0].node)); }
#line 3803 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 240: /* dictslotlist: %empty  */
#line 1041 "lang/LangSource/Bison/lang11d"
                         { (yyval.node) = nullptr; }
#line 3809 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 243: /* rwslotdeflist: rwslotdeflist COMMA rwslotdef  */
#line 1046 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varDefNode) = linkNodes((yyvsp[-2].varDefNode), (yyvsp[0].varDefNode)); }
#line 3815 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 244: /* rwslotdef: rwspec name  */
#line 1049 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varDefNode) = bison_cxt->allocParseNode<PyrVarDefNode>((yyloc), (yyvsp[0].slotNode), nullptr, (yyvsp[-1].rwAccessor)); }
#line 3821 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 245: /* rwslotdef: rwspec name EQUALSSIGN slotliteral  */
#line 1051 "lang/LangSource/Bison/lang11d"
                                        { (yyval.varDefNode) = bison_cxt->allocParseNode<PyrVarDefNode>((yyloc), (yyvsp[-2].slotNode), (yyvsp[0].node), (yyvsp[-3].rwAccessor)); }
#line 3827 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 246: /* listlit: HASH OPENSQUARE literallistc CLOSESQUARE  */
#line 1054 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrLitListNode>((yyloc), nullptr, (yyvsp[-1].node)); }
#line 3833 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 247: /* listlit: HASH CLASSNAME OPENSQUARE literallistc CLOSESQUARE  */
#line 1056 "lang/LangSource/Bison/lang11d"
                                                { (yyval.node) = bison_cxt->allocParseNode<PyrLitListNode>((yyloc), (yyvsp[-3].slotNode), (yyvsp[-1].node)); }
#line 3839 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 248: /* listlit2: OPENSQUARE literallistc CLOSESQUARE  */
#line 1059 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrLitListNode>((yyloc), nullptr, (yyvsp[-1].node)); }
#line 3845 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 249: /* listlit2: CLASSNAME OPENSQUARE literallistc CLOSESQUARE  */
#line 1061 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = bison_cxt->allocParseNode<PyrLitListNode>((yyloc), (yyvsp[-3].slotNode), (yyvsp[-1].node)); }
#line 3851 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 250: /* literallistc: %empty  */
#line 1063 "lang/LangSource/Bison/lang11d"
                         { (yyval.node) = nullptr; }
#line 3857 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 253: /* literallist1: literallist1 COMMA listliteral  */
#line 1068 "lang/LangSource/Bison/lang11d"
                                        { (yyval.node) = linkNodes((yyvsp[-2].node), (yyvsp[0].node)); }
#line 3863 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 254: /* rwspec: %empty  */
#line 1070 "lang/LangSource/Bison/lang11d"
                                 { (yyval.rwAccessor) = ReadWriteAccessor::Private; }
#line 3869 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 255: /* rwspec: LESSTHAN  */
#line 1071 "lang/LangSource/Bison/lang11d"
                                           { (yyval.rwAccessor) = ReadWriteAccessor::Read; }
#line 3875 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 256: /* rwspec: READWRITEVAR  */
#line 1072 "lang/LangSource/Bison/lang11d"
                                               { (yyval.rwAccessor) = ReadWriteAccessor::ReadWrite; }
#line 3881 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 257: /* rwspec: GREATERTHAN  */
#line 1073 "lang/LangSource/Bison/lang11d"
                                              { (yyval.rwAccessor) = ReadWriteAccessor::Write; }
#line 3887 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 258: /* rspec: %empty  */
#line 1075 "lang/LangSource/Bison/lang11d"
                                 { (yyval.rwAccessor) = ReadWriteAccessor::Private; }
#line 3893 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 259: /* rspec: LESSTHAN  */
#line 1076 "lang/LangSource/Bison/lang11d"
                                           { (yyval.rwAccessor) = ReadWriteAccessor::Read; }
#line 3899 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 261: /* integer: MINUS INTEGER  */
#line 1080 "lang/LangSource/Bison/lang11d"
                                        {
						const auto v = (yyvsp[0].slotNode)->mSlot.getInt();
						(yyvsp[0].slotNode)->mSlot = PyrSlot::make(-v);
						(yyval.slotNode) = (yyvsp[0].slotNode);
					}
#line 3909 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 263: /* floatr: MINUS SC_FLOAT  */
#line 1088 "lang/LangSource/Bison/lang11d"
                                        {
						const double v = (yyvsp[0].slotNode)->mSlot.getDouble();
						(yyvsp[0].slotNode)->mSlot = PyrSlot::make(-v);
						(yyval.slotNode) = (yyvsp[0].slotNode);
					}
#line 3919 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 265: /* accidental: MINUS ACCIDENTAL  */
#line 1096 "lang/LangSource/Bison/lang11d"
                                        {
						const double in = (yyvsp[0].slotNode)->mSlot.getDouble();
						const double intval = floor(in + 0.5);
						const double fracval = in - intval;
						(yyvsp[0].slotNode)->mSlot = PyrSlot::make(-intval + fracval);
						(yyval.slotNode) = (yyvsp[0].slotNode);
					}
#line 3931 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 268: /* floatp: floatr PIE  */
#line 1107 "lang/LangSource/Bison/lang11d"
                                        {
						(yyvsp[-1].slotNode)->mSlot = PyrSlot::make((yyvsp[-1].slotNode)->mSlot.getDouble() * pi);
						(yyvsp[-1].slotNode)->location = (yyloc);
						(yyval.slotNode) = (yyvsp[-1].slotNode);
					}
#line 3941 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 269: /* floatp: integer PIE  */
#line 1113 "lang/LangSource/Bison/lang11d"
                                        {
						(yyvsp[-1].slotNode)->mSlot = PyrSlot::make((yyvsp[-1].slotNode)->mSlot.getInt() * pi);
						(yyvsp[-1].slotNode)->location = (yyloc);
						(yyval.slotNode) = (yyvsp[-1].slotNode);
					}
#line 3951 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 271: /* floatp: MINUS PIE  */
#line 1120 "lang/LangSource/Bison/lang11d"
                                        {
						(yyvsp[0].slotNode)->mSlot = PyrSlot::make(-pi);
						(yyvsp[0].slotNode)->location = (yyloc);
						(yyval.slotNode) = (yyvsp[0].slotNode);
					}
#line 3961 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;

  case 284: /* curryArg: CURRYARG  */
#line 1129 "lang/LangSource/Bison/lang11d"
                                   { (yyval.node) = bison_cxt->allocParseNode<PyrCurryArgNode>((yyloc)); }
#line 3967 "lang/LangSource/Bison/lang11d_tab.cpp"
    break;


#line 3971 "lang/LangSource/Bison/lang11d_tab.cpp"

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

