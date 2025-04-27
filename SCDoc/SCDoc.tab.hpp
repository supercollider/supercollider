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

#ifndef YY_SCDOC_SCDOC_TAB_HPP_INCLUDED
# define YY_SCDOC_SCDOC_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int scdocdebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    END = 0,                       /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    CLASS = 258,                   /* CLASS  */
    TITLE = 259,                   /* TITLE  */
    SUMMARY = 260,                 /* SUMMARY  */
    RELATED = 261,                 /* RELATED  */
    CATEGORIES = 262,              /* CATEGORIES  */
    REDIRECT = 263,                /* REDIRECT  */
    CLASSTREE = 264,               /* CLASSTREE  */
    COPYMETHOD = 265,              /* COPYMETHOD  */
    KEYWORD = 266,                 /* KEYWORD  */
    PRIVATE = 267,                 /* PRIVATE  */
    SECTION = 268,                 /* SECTION  */
    SUBSECTION = 269,              /* SUBSECTION  */
    SUBSUBSECTION = 270,           /* SUBSUBSECTION  */
    METHOD = 271,                  /* METHOD  */
    ARGUMENT = 272,                /* ARGUMENT  */
    DESCRIPTION = 273,             /* DESCRIPTION  */
    CLASSMETHODS = 274,            /* CLASSMETHODS  */
    INSTANCEMETHODS = 275,         /* INSTANCEMETHODS  */
    EXAMPLES = 276,                /* EXAMPLES  */
    RETURNS = 277,                 /* RETURNS  */
    DISCUSSION = 278,              /* DISCUSSION  */
    LIST = 279,                    /* LIST  */
    TREE = 280,                    /* TREE  */
    NUMBEREDLIST = 281,            /* NUMBEREDLIST  */
    DEFINITIONLIST = 282,          /* DEFINITIONLIST  */
    TABLE = 283,                   /* TABLE  */
    FOOTNOTE = 284,                /* FOOTNOTE  */
    NOTE = 285,                    /* NOTE  */
    WARNING = 286,                 /* WARNING  */
    CODE = 287,                    /* CODE  */
    LINK = 288,                    /* LINK  */
    ANCHOR = 289,                  /* ANCHOR  */
    SOFT = 290,                    /* SOFT  */
    IMAGE = 291,                   /* IMAGE  */
    TELETYPE = 292,                /* TELETYPE  */
    MATH = 293,                    /* MATH  */
    STRONG = 294,                  /* STRONG  */
    EMPHASIS = 295,                /* EMPHASIS  */
    CODEBLOCK = 296,               /* "CODE block"  */
    TELETYPEBLOCK = 297,           /* "TELETYPE block"  */
    MATHBLOCK = 298,               /* "MATH block"  */
    TAGSYM = 299,                  /* "::"  */
    BARS = 300,                    /* "||"  */
    HASHES = 301,                  /* "##"  */
    TEXT = 302,                    /* "text"  */
    URL = 303,                     /* URL  */
    COMMA = 304,                   /* COMMA  */
    METHODNAME = 305,              /* "method name"  */
    METHODARGS = 306,              /* "arguments string"  */
    NEWLINE = 307,                 /* "newline"  */
    EMPTYLINES = 308,              /* "empty lines"  */
    BAD_METHODNAME = 309,          /* "bad method name"  */
    START_FULL = 310,              /* START_FULL  */
    START_PARTIAL = 311,           /* START_PARTIAL  */
    START_METADATA = 312           /* START_METADATA  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 55 "SCDoc.y"

    intptr_t i;
    const char *id;
    char *str;
    DocNode *doc_node;

#line 128 "SCDoc.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE scdoclval;
extern YYLTYPE scdoclloc;

int scdocparse (void);


#endif /* !YY_SCDOC_SCDOC_TAB_HPP_INCLUDED  */
