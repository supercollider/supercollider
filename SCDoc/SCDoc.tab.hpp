/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.
   
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

#ifndef YY_SCDOC_SCDOC_TAB_HPP_INCLUDED
# define YY_SCDOC_SCDOC_TAB_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int scdocdebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     END = 0,
     CLASS = 258,
     TITLE = 259,
     SUMMARY = 260,
     RELATED = 261,
     CATEGORIES = 262,
     REDIRECT = 263,
     CLASSTREE = 264,
     COPYMETHOD = 265,
     KEYWORD = 266,
     PRIVATE = 267,
     SECTION = 268,
     SUBSECTION = 269,
     METHOD = 270,
     ARGUMENT = 271,
     DESCRIPTION = 272,
     CLASSMETHODS = 273,
     INSTANCEMETHODS = 274,
     EXAMPLES = 275,
     RETURNS = 276,
     DISCUSSION = 277,
     LIST = 278,
     TREE = 279,
     NUMBEREDLIST = 280,
     DEFINITIONLIST = 281,
     TABLE = 282,
     FOOTNOTE = 283,
     NOTE = 284,
     WARNING = 285,
     CODE = 286,
     LINK = 287,
     ANCHOR = 288,
     SOFT = 289,
     IMAGE = 290,
     TELETYPE = 291,
     STRONG = 292,
     EMPHASIS = 293,
     CODEBLOCK = 294,
     TELETYPEBLOCK = 295,
     TAGSYM = 296,
     BARS = 297,
     HASHES = 298,
     TEXT = 299,
     URL = 300,
     COMMA = 301,
     METHODNAME = 302,
     METHODARGS = 303,
     NEWLINE = 304,
     EMPTYLINES = 305,
     BAD_METHODNAME = 306,
     START_FULL = 307,
     START_PARTIAL = 308,
     START_METADATA = 309
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2053 of yacc.c  */
#line 55 "SCDoc.y"

    int i;
    const char *id;
    char *str;
    DocNode *doc_node;


/* Line 2053 of yacc.c  */
#line 120 "SCDoc.tab.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE scdoclval;
extern YYLTYPE scdoclloc;
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int scdocparse (void *YYPARSE_PARAM);
#else
int scdocparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int scdocparse (void);
#else
int scdocparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_SCDOC_SCDOC_TAB_HPP_INCLUDED  */
