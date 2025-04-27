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

#ifndef YY_YY_LANG11D_TAB_H_INCLUDED
# define YY_YY_LANG11D_TAB_H_INCLUDED
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
    NAME = 258,                    /* NAME  */
    INTEGER = 259,                 /* INTEGER  */
    SC_FLOAT = 260,                /* SC_FLOAT  */
    ACCIDENTAL = 261,              /* ACCIDENTAL  */
    SYMBOL = 262,                  /* SYMBOL  */
    STRING = 263,                  /* STRING  */
    ASCII = 264,                   /* ASCII  */
    PRIMITIVENAME = 265,           /* PRIMITIVENAME  */
    CLASSNAME = 266,               /* CLASSNAME  */
    CURRYARG = 267,                /* CURRYARG  */
    VAR = 268,                     /* VAR  */
    ARG = 269,                     /* ARG  */
    CLASSVAR = 270,                /* CLASSVAR  */
    SC_CONST = 271,                /* SC_CONST  */
    NILOBJ = 272,                  /* NILOBJ  */
    TRUEOBJ = 273,                 /* TRUEOBJ  */
    FALSEOBJ = 274,                /* FALSEOBJ  */
    PSEUDOVAR = 275,               /* PSEUDOVAR  */
    ELLIPSIS = 276,                /* ELLIPSIS  */
    DOTDOT = 277,                  /* DOTDOT  */
    PIE = 278,                     /* PIE  */
    BEGINCLOSEDFUNC = 279,         /* BEGINCLOSEDFUNC  */
    BADTOKEN = 280,                /* BADTOKEN  */
    INTERPRET = 281,               /* INTERPRET  */
    BEGINGENERATOR = 282,          /* BEGINGENERATOR  */
    LEFTARROW = 283,               /* LEFTARROW  */
    WHILE = 284,                   /* WHILE  */
    BINOP = 285,                   /* BINOP  */
    KEYBINOP = 286,                /* KEYBINOP  */
    READWRITEVAR = 287,            /* READWRITEVAR  */
    UMINUS = 288                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_LANG11D_TAB_H_INCLUDED  */
