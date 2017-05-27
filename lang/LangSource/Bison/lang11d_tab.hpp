/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_LANG11D_TAB_HPP_INCLUDED
# define YY_YY_LANG11D_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    NAME = 258,
    INTEGER = 259,
    SC_FLOAT = 260,
    ACCIDENTAL = 261,
    SYMBOL = 262,
    STRING = 263,
    ASCII = 264,
    PRIMITIVENAME = 265,
    CLASSNAME = 266,
    CURRYARG = 267,
    VAR = 268,
    ARG = 269,
    CLASSVAR = 270,
    SC_CONST = 271,
    NILOBJ = 272,
    TRUEOBJ = 273,
    FALSEOBJ = 274,
    PSEUDOVAR = 275,
    ELLIPSIS = 276,
    DOTDOT = 277,
    PIE = 278,
    BEGINCLOSEDFUNC = 279,
    BADTOKEN = 280,
    INTERPRET = 281,
    BEGINGENERATOR = 282,
    LEFTARROW = 283,
    WHILE = 284,
    BINOP = 285,
    KEYBINOP = 286,
    READWRITEVAR = 287,
    UMINUS = 288
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_LANG11D_TAB_HPP_INCLUDED  */
