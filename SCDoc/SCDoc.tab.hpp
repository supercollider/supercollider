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
    METHOD = 270,                  /* METHOD  */
    ARGUMENT = 271,                /* ARGUMENT  */
    DESCRIPTION = 272,             /* DESCRIPTION  */
    CLASSMETHODS = 273,            /* CLASSMETHODS  */
    INSTANCEMETHODS = 274,         /* INSTANCEMETHODS  */
    EXAMPLES = 275,                /* EXAMPLES  */
    RETURNS = 276,                 /* RETURNS  */
    DISCUSSION = 277,              /* DISCUSSION  */
    LIST = 278,                    /* LIST  */
    TREE = 279,                    /* TREE  */
    NUMBEREDLIST = 280,            /* NUMBEREDLIST  */
    DEFINITIONLIST = 281,          /* DEFINITIONLIST  */
    TABLE = 282,                   /* TABLE  */
    FOOTNOTE = 283,                /* FOOTNOTE  */
    NOTE = 284,                    /* NOTE  */
    WARNING = 285,                 /* WARNING  */
    CODE = 286,                    /* CODE  */
    LINK = 287,                    /* LINK  */
    ANCHOR = 288,                  /* ANCHOR  */
    SOFT = 289,                    /* SOFT  */
    IMAGE = 290,                   /* IMAGE  */
    TELETYPE = 291,                /* TELETYPE  */
    MATH = 292,                    /* MATH  */
    STRONG = 293,                  /* STRONG  */
    EMPHASIS = 294,                /* EMPHASIS  */
    CODEBLOCK = 295,               /* "CODE block"  */
    TELETYPEBLOCK = 296,           /* "TELETYPE block"  */
    MATHBLOCK = 297,               /* "MATH block"  */
    TAGSYM = 298,                  /* "::"  */
    BARS = 299,                    /* "||"  */
    HASHES = 300,                  /* "##"  */
    TEXT = 301,                    /* "text"  */
    URL = 302,                     /* URL  */
    COMMA = 303,                   /* COMMA  */
    METHODNAME = 304,              /* "method name"  */
    METHODARGS = 305,              /* "arguments string"  */
    NEWLINE = 306,                 /* "newline"  */
    EMPTYLINES = 307,              /* "empty lines"  */
    BAD_METHODNAME = 308,          /* "bad method name"  */
    START_FULL = 309,              /* START_FULL  */
    START_PARTIAL = 310,           /* START_PARTIAL  */
    START_METADATA = 311           /* START_METADATA  */
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

#line 127 "SCDoc.tab.hpp"

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
