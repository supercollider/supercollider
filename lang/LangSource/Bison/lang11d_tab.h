/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_LANG_LANGSOURCE_BISON_LANG11D_TAB_H_INCLUDED
# define YY_YY_LANG_LANGSOURCE_BISON_LANG11D_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    OPENCURLY = 258,               /* OPENCURLY  */
    CLOSECURLY = 259,              /* CLOSECURLY  */
    OPENSQUARE = 260,              /* OPENSQUARE  */
    CLOSESQUARE = 261,             /* CLOSESQUARE  */
    OPENPAREN = 262,               /* OPENPAREN  */
    CLOSEPAREN = 263,              /* CLOSEPAREN  */
    SEMICOLON = 264,               /* SEMICOLON  */
    NONLOCALRETURN = 265,          /* NONLOCALRETURN  */
    COMMA = 266,                   /* COMMA  */
    HASH = 267,                    /* HASH  */
    TILDE = 268,                   /* TILDE  */
    NAME = 269,                    /* NAME  */
    INTEGER = 270,                 /* INTEGER  */
    SC_FLOAT = 271,                /* SC_FLOAT  */
    ACCIDENTAL = 272,              /* ACCIDENTAL  */
    SYMBOL = 273,                  /* SYMBOL  */
    STRING = 274,                  /* STRING  */
    ASCII = 275,                   /* ASCII  */
    PRIMITIVENAME = 276,           /* PRIMITIVENAME  */
    CLASSNAME = 277,               /* CLASSNAME  */
    CURRYARG = 278,                /* CURRYARG  */
    VAR = 279,                     /* VAR  */
    ARG = 280,                     /* ARG  */
    CLASSVAR = 281,                /* CLASSVAR  */
    SC_CONST = 282,                /* SC_CONST  */
    NILOBJ = 283,                  /* NILOBJ  */
    TRUEOBJ = 284,                 /* TRUEOBJ  */
    FALSEOBJ = 285,                /* FALSEOBJ  */
    PSEUDOVAR = 286,               /* PSEUDOVAR  */
    ELLIPSIS = 287,                /* ELLIPSIS  */
    DOTDOT = 288,                  /* DOTDOT  */
    PIE = 289,                     /* PIE  */
    BEGINCLOSEDFUNC = 290,         /* BEGINCLOSEDFUNC  */
    BADTOKEN = 291,                /* BADTOKEN  */
    INTERPRET = 292,               /* INTERPRET  */
    LEFTARROW = 293,               /* LEFTARROW  */
    WHILE = 294,                   /* WHILE  */
    COLON = 295,                   /* COLON  */
    EQUALSSIGN = 296,              /* EQUALSSIGN  */
    BINOP = 297,                   /* BINOP  */
    KEYBINOP = 298,                /* KEYBINOP  */
    MINUS = 299,                   /* MINUS  */
    LESSTHAN = 300,                /* LESSTHAN  */
    GREATERTHAN = 301,             /* GREATERTHAN  */
    MULTIPLY = 302,                /* MULTIPLY  */
    ADD = 303,                     /* ADD  */
    PIPE = 304,                    /* PIPE  */
    READWRITEVAR = 305,            /* READWRITEVAR  */
    DOT = 306,                     /* DOT  */
    BACKTICK = 307,                /* BACKTICK  */
    UMINUS = 308                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
typedef sc::lex::SourceCodeRange YYLTYPE;


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (void);


#endif /* !YY_YY_LANG_LANGSOURCE_BISON_LANG11D_TAB_H_INCLUDED  */
