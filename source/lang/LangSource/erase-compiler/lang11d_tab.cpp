
/*  A Bison parser, made from lang11d
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	NAME	257
#define	INTEGER	258
#define	FLOAT	259
#define	SYMBOL	260
#define	STRING	261
#define	ASCII	262
#define	PRIMITIVENAME	263
#define	CLASSNAME	264
#define	VAR	265
#define	ARG	266
#define	CLASSVAR	267
#define	CONST	268
#define	NILOBJ	269
#define	TRUEOBJ	270
#define	FALSEOBJ	271
#define	INFINITUMOBJ	272
#define	PSEUDOVAR	273
#define	ELLIPSIS	274
#define	PIE	275
#define	BADTOKEN	276
#define	INTERPRET	277
#define	BINOP	278
#define	KEYBINOP	279
#define	READWRITEVAR	280
#define	UMINUS	281

#line 14 "lang11d"


#include <stdlib.h>
#include <string.h>
#include "PyrLexer.h"
#include "PyrParseNode.h"
#include "SC_Constants.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "InitAlloc.h"
#include "PredefinedSymbols.h"

void *alloca(unsigned long size); 
void bcopy(void *src, void *dst, size_t size) ;
int yyparse();

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		314
#define	YYFLAG		-32768
#define	YYNTBASE	49

#define YYTRANSLATE(x) ((unsigned)(x) <= 281 ? yytranslate[x] : 118)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,    48,     2,     2,     2,     2,    44,
    45,    30,    31,    43,    27,    34,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    41,    42,    28,
    24,    29,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    39,     2,    40,    46,     2,    35,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    37,    32,    38,    47,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    25,    26,    33,
    36
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     7,     8,    11,    13,    16,    23,    33,
    39,    40,    42,    43,    46,    47,    50,    54,    58,    59,
    62,    70,    79,    87,    96,    97,    99,   100,   102,   104,
   107,   112,   115,   117,   119,   122,   123,   126,   127,   131,
   132,   136,   138,   141,   142,   144,   147,   154,   161,   169,
   177,   186,   191,   193,   195,   197,   199,   202,   204,   206,
   211,   217,   221,   224,   228,   232,   236,   241,   245,   250,
   256,   264,   269,   271,   275,   278,   279,   282,   284,   288,
   290,   294,   297,   298,   301,   303,   307,   309,   313,   315,
   317,   319,   321,   323,   325,   327,   329,   331,   333,   335,
   337,   339,   341,   343,   345,   347,   349,   351,   353,   355,
   357,   359,   361,   363,   365,   367,   369,   371,   373,   375,
   381,   388,   389,   392,   394,   397,   401,   402,   406,   412,
   416,   422,   423,   425,   427,   431,   433,   437,   439,   443,
   446,   451,   456,   462,   466,   471,   472,   475,   477,   481,
   482,   484,   486,   488,   490,   493,   495,   498,   500,   502,
   505,   508,   510,   513,   515,   517,   519,   521,   523,   525,
   527,   529,   531,   533,   536,   538,   540,   542,   544,   546,
   548,   550,   552,   554,   556
};

static const short yyrhs[] = {    50,
     0,    51,     0,    23,    63,     0,     0,    50,    52,     0,
    53,     0,    51,    53,     0,   105,    55,    37,    56,    58,
    38,     0,   105,    39,    54,    40,    55,    37,    56,    58,
    38,     0,    31,   105,    37,    58,    38,     0,     0,   104,
     0,     0,    41,   105,     0,     0,    56,    57,     0,    13,
    93,    42,     0,    11,    93,    42,     0,     0,    58,    59,
     0,   104,    37,    89,    86,    65,    64,    38,     0,    30,
   104,    37,    89,    86,    65,    64,    38,     0,   116,    37,
    89,    86,    65,    64,    38,     0,    30,   116,    37,    89,
    86,    65,    64,    38,     0,     0,    42,     0,     0,    43,
     0,    67,     0,    73,    67,     0,    44,    87,    62,    45,
     0,    87,    62,     0,    62,     0,    66,     0,    73,    66,
     0,     0,   106,    60,     0,     0,    46,    71,    60,     0,
     0,    46,    71,    60,     0,    82,     0,    68,    82,     0,
     0,    68,     0,   104,    68,     0,   104,    44,    75,    78,
    45,    69,     0,    71,    34,    44,    76,    45,    69,     0,
    71,    34,   104,    44,    76,    45,    69,     0,    71,    34,
    44,    75,    78,    45,    69,     0,    71,    34,   104,    44,
    75,    78,    45,    69,     0,    71,    34,   104,    69,     0,
    83,     0,    82,     0,    70,     0,   104,     0,    47,   104,
     0,   115,     0,   105,     0,   105,    44,    76,    45,     0,
   105,    44,    75,    78,    45,     0,    44,    73,    45,     0,
    35,    71,     0,    71,   116,    71,     0,    71,   117,    71,
     0,    39,    74,    40,     0,   105,    39,    74,    40,     0,
   104,    24,    71,     0,    47,   104,    24,    71,     0,    71,
    34,   104,    24,    71,     0,   104,    44,    75,    78,    45,
    24,    71,     0,    48,    79,    24,    71,     0,    71,     0,
    72,    42,    71,     0,    72,    60,     0,     0,    75,    61,
     0,    73,     0,    75,    43,    73,     0,    77,     0,    76,
    43,    77,     0,   117,    73,     0,     0,    43,    76,     0,
    80,     0,    80,    20,   104,     0,   104,     0,    80,    43,
   104,     0,   100,     0,   103,     0,   111,     0,   113,     0,
   112,     0,   107,     0,   108,     0,   109,     0,   110,     0,
    95,     0,    85,     0,   100,     0,   103,     0,   111,     0,
   113,     0,   112,     0,   107,     0,   108,     0,   109,     0,
   110,     0,    95,     0,   100,     0,   103,     0,   111,     0,
   113,     0,   112,     0,   107,     0,   108,     0,   109,     0,
   110,     0,    96,     0,    37,    89,    86,    62,    38,     0,
    48,    37,    89,    86,    62,    38,     0,     0,    86,    88,
     0,    88,     0,    87,    88,     0,    11,    91,    42,     0,
     0,    12,    91,    42,     0,    12,    90,    20,   104,    42,
     0,    32,    91,    32,     0,    32,    90,    20,   104,    32,
     0,     0,    91,     0,    92,     0,    91,    43,    92,     0,
   104,     0,   104,    24,    81,     0,    94,     0,    93,    43,
    94,     0,    99,   104,     0,    99,   104,    24,    81,     0,
    48,    39,    97,    40,     0,    48,   105,    39,    97,    40,
     0,    39,    97,    40,     0,   105,    39,    97,    40,     0,
     0,    98,    61,     0,    84,     0,    98,    43,    84,     0,
     0,    28,     0,    33,     0,    29,     0,     4,     0,    27,
     4,     0,     5,     0,    27,     5,     0,    21,     0,   101,
     0,   101,   102,     0,   100,   102,     0,   102,     0,    27,
   102,     0,     3,     0,    10,     0,     9,     0,    16,     0,
    17,     0,    15,     0,    18,     0,     8,     0,     6,     0,
   114,     0,   113,   114,     0,     7,     0,    19,     0,    25,
     0,    33,     0,    28,     0,    29,     0,    27,     0,    30,
     0,    31,     0,    32,     0,    26,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    34,    36,    38,    42,    43,    47,    48,    52,    56,    63,
    69,    70,    73,    74,    78,    79,    83,    85,    89,    90,
    94,    97,   100,   103,   108,   109,   112,   113,   116,   117,
   121,   123,   125,   129,   130,   134,   135,   139,   141,   145,
   147,   151,   152,   158,   159,   162,   166,   171,   180,   185,
   198,   204,   210,   211,   212,   213,   215,   225,   227,   229,
   240,   253,   255,   269,   274,   279,   281,   283,   287,   298,
   303,   313,   320,   321,   327,   330,   331,   334,   335,   339,
   340,   344,   348,   349,   352,   354,   358,   359,   364,   365,
   366,   367,   368,   369,   370,   371,   372,   373,   376,   379,
   380,   381,   382,   383,   384,   385,   386,   387,   388,   391,
   392,   393,   394,   395,   396,   397,   398,   399,   400,   403,
   406,   411,   412,   416,   417,   421,   425,   426,   430,   434,
   438,   444,   445,   448,   449,   453,   455,   459,   460,   464,
   466,   472,   474,   478,   480,   484,   485,   488,   489,   493,
   494,   496,   498,   502,   503,   512,   513,   522,   525,   526,
   532,   540,   547,   556,   559,   562,   565,   568,   571,   574,
   577,   580,   583,   584,   610,   613,   616,   617,   618,   619,
   620,   621,   622,   623,   626
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","NAME","INTEGER",
"FLOAT","SYMBOL","STRING","ASCII","PRIMITIVENAME","CLASSNAME","VAR","ARG","CLASSVAR",
"CONST","NILOBJ","TRUEOBJ","FALSEOBJ","INFINITUMOBJ","PSEUDOVAR","ELLIPSIS",
"PIE","BADTOKEN","INTERPRET","'='","BINOP","KEYBINOP","'-'","'<'","'>'","'*'",
"'+'","'|'","READWRITEVAR","'.'","'`'","UMINUS","'{'","'}'","'['","']'","':'",
"';'","','","'('","')'","'^'","'~'","'#'","root","classes","classextensions",
"classdef","classextension","optname","superclass","classvardecls","classvardecl",
"methods","methoddef","optsemi","optcomma","funcbody","cmdlinecode","methbody",
"primitive","retval","funretval","blocklist1","blocklist","msgsend","expr","exprn",
"exprseq","arglistc","arglist1","keyarglist1","keyarg","optkeyarglist","mavars",
"mavarlist","slotliteral","blockliteral","pushliteral","listliteral","block",
"funcvardecls","funcvardecls1","funcvardecl","argdecls","slotdeflist0","slotdeflist",
"slotdef","rwslotdeflist","rwslotdef","listlit","listlit2","literallistc","literallist1",
"rwspec","integer","floatr","pie","floatp","name","classname","primname","trueobj",
"falseobj","nilobj","infobj","ascii","symbol","string","string1","pseudovar",
"binop","keybinop", NULL
};
#endif

static const short yyr1[] = {     0,
    49,    49,    49,    50,    50,    51,    51,    52,    52,    53,
    54,    54,    55,    55,    56,    56,    57,    57,    58,    58,
    59,    59,    59,    59,    60,    60,    61,    61,    62,    62,
    63,    63,    63,    64,    64,    65,    65,    66,    66,    67,
    67,    68,    68,    69,    69,    70,    70,    70,    70,    70,
    70,    70,    71,    71,    71,    71,    71,    71,    71,    71,
    71,    71,    71,    71,    71,    71,    71,    71,    71,    71,
    71,    71,    72,    72,    73,    74,    74,    75,    75,    76,
    76,    77,    78,    78,    79,    79,    80,    80,    81,    81,
    81,    81,    81,    81,    81,    81,    81,    81,    82,    83,
    83,    83,    83,    83,    83,    83,    83,    83,    83,    84,
    84,    84,    84,    84,    84,    84,    84,    84,    84,    85,
    85,    86,    86,    87,    87,    88,    89,    89,    89,    89,
    89,    90,    90,    91,    91,    92,    92,    93,    93,    94,
    94,    95,    95,    96,    96,    97,    97,    98,    98,    99,
    99,    99,    99,   100,   100,   101,   101,   102,   103,   103,
   103,   103,   103,   104,   105,   106,   107,   108,   109,   110,
   111,   112,   113,   113,   114,   115,   116,   116,   116,   116,
   116,   116,   116,   116,   117
};

static const short yyr2[] = {     0,
     1,     1,     2,     0,     2,     1,     2,     6,     9,     5,
     0,     1,     0,     2,     0,     2,     3,     3,     0,     2,
     7,     8,     7,     8,     0,     1,     0,     1,     1,     2,
     4,     2,     1,     1,     2,     0,     2,     0,     3,     0,
     3,     1,     2,     0,     1,     2,     6,     6,     7,     7,
     8,     4,     1,     1,     1,     1,     2,     1,     1,     4,
     5,     3,     2,     3,     3,     3,     4,     3,     4,     5,
     7,     4,     1,     3,     2,     0,     2,     1,     3,     1,
     3,     2,     0,     2,     1,     3,     1,     3,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     5,
     6,     0,     2,     1,     2,     3,     0,     3,     5,     3,
     5,     0,     1,     1,     3,     1,     3,     1,     3,     2,
     4,     4,     5,     3,     4,     0,     2,     1,     3,     0,
     1,     1,     1,     1,     2,     1,     2,     1,     1,     2,
     2,     1,     2,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1
};

static const short yydefact[] = {     4,
    40,     0,     1,     2,     6,   164,   154,   156,   172,   175,
   171,   165,     0,   169,   167,   168,   170,   176,   158,     0,
     0,   127,    76,     0,     0,     0,     0,    33,     3,    29,
    55,    73,    25,    40,    54,    53,    99,    40,   124,   109,
   100,   159,   162,   101,    56,    59,   105,   106,   107,   108,
   102,   104,   103,   173,    58,     0,     5,    13,     7,     0,
   134,   136,   155,   157,   163,     0,    63,   132,   132,   122,
    78,     0,    27,     0,    40,    25,    57,   127,   146,     0,
    85,    87,     0,   177,   185,   181,   179,   180,   182,   183,
   184,   178,     0,     0,     0,    26,    75,    30,    32,   125,
   161,   160,     0,     0,     0,    46,    42,    76,     0,   174,
    19,    11,     0,     0,   126,     0,     0,     0,   133,     0,
   133,    40,    66,    28,    77,    62,     0,    26,    41,     0,
   122,   146,   148,   119,     0,    27,   110,   111,     0,   115,
   116,   117,   118,   112,   114,   113,     0,     0,     0,   146,
     0,    44,    64,    65,    74,    68,    83,    43,     0,    83,
     0,    80,     0,     0,     0,    12,    14,    15,   135,     0,
   137,    98,    89,    90,    94,    95,    96,    97,    91,    93,
    92,     0,   128,     0,   130,     0,   123,    79,    31,    69,
    40,     0,   142,    28,   147,   146,    72,    86,    88,     0,
    83,     0,     0,     0,    45,    52,     0,     0,    67,     0,
     0,    60,    82,   182,    10,    20,     0,     0,    13,    19,
     0,     0,   120,     0,   144,   149,     0,   143,     0,    44,
    70,    83,     0,    84,    44,    61,    81,     0,     0,   127,
   127,     0,   150,   150,    16,     0,   129,   131,   121,   145,
    44,    48,     0,    44,     0,    47,   127,   127,   122,   122,
    15,   151,   153,   152,     0,   138,     0,     0,     8,    50,
    44,    49,    71,   122,   122,    36,    36,    19,    18,   150,
   140,    17,    51,    36,    36,   166,    38,    25,    38,     0,
   139,     0,    38,    38,     0,     0,    34,    38,    37,     0,
     9,   141,     0,     0,    25,    21,    35,    23,    22,    24,
    39,     0,     0,     0
};

static const short yydefgoto[] = {   312,
     3,     4,    57,     5,   165,   114,   220,   245,   164,   216,
    97,   125,    28,    29,   296,   287,   297,    30,   205,   206,
    31,    32,    33,    71,    72,    73,   161,   162,   208,    80,
    81,   171,    35,    36,   133,    37,   122,    38,   187,    70,
   118,    60,    61,   265,   266,    40,   134,   135,   136,   267,
    41,    42,    43,    44,    45,    46,   288,    47,    48,    49,
    50,    51,    52,    53,    54,    55,    94,    95
};

static const short yypact[] = {    85,
   264,     3,     3,    20,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    24,-32768,-32768,-32768,-32768,-32768,-32768,   190,
   494,    10,   494,   356,   494,    24,    88,-32768,-32768,-32768,
-32768,   629,    -3,    14,-32768,-32768,-32768,   310,-32768,-32768,
    82,    82,-32768,-32768,    -4,    28,-32768,-32768,-32768,-32768,
-32768,-32768,   102,-32768,-32768,    75,-32768,    74,-32768,   184,
-32768,   126,-32768,-32768,-32768,   494,-32768,    24,    24,-32768,
-32768,   115,   117,   119,   310,   611,   154,    10,   543,   172,
    18,-32768,   132,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,     9,   494,   494,   494,-32768,-32768,-32768,-32768,
-32768,-32768,   494,   494,   163,    -7,-32768,   494,   402,-32768,
-32768,    24,     3,   168,-32768,    24,   519,   189,   200,   194,
    50,   310,-32768,   494,-32768,-32768,   183,-32768,-32768,   494,
-32768,   543,-32768,-32768,   191,   197,    82,-32768,   215,-32768,
-32768,-32768,-32768,-32768,-32768,   102,   494,    24,    24,   543,
   402,   145,   212,   212,   629,   629,   216,-32768,   218,   216,
   104,-32768,   494,   559,   220,-32768,-32768,-32768,-32768,    -5,
-32768,-32768,    82,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   102,    24,-32768,    24,-32768,   224,-32768,-32768,-32768,   629,
   310,   223,-32768,   543,-32768,   543,   629,-32768,-32768,   225,
   216,   142,   494,   402,    -7,-32768,   402,   219,-32768,   221,
   247,-32768,-32768,   602,-32768,-32768,   240,   250,   243,    79,
   248,   257,-32768,   256,-32768,-32768,   255,-32768,   251,    -7,
   629,   216,   174,   254,    70,-32768,-32768,   261,   263,    10,
    10,   265,   147,   147,-32768,   571,-32768,-32768,-32768,-32768,
    -7,-32768,   259,    -7,   494,-32768,    10,    10,-32768,-32768,
-32768,-32768,-32768,-32768,   207,-32768,    24,   210,-32768,-32768,
    -7,-32768,   629,-32768,-32768,   209,   209,    79,-32768,   147,
   281,-32768,-32768,   209,   209,-32768,   448,   267,   448,   590,
-32768,   519,   448,   448,   494,   268,-32768,   276,-32768,   269,
-32768,-32768,   285,   286,   611,-32768,-32768,-32768,-32768,-32768,
-32768,   330,   332,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,   331,-32768,   120,    73,-32768,  -212,-32768,
   -73,   202,   -17,-32768,   -90,  -253,    38,   306,   296,   162,
-32768,   -19,-32768,    -1,   234,    47,  -105,   133,  -149,-32768,
-32768,    54,   -38,-32768,   156,-32768,  -121,   324,    35,   -33,
   282,   188,   236,   109,    89,  -113,-32768,   -61,-32768,-32768,
   -70,-32768,     8,   -63,   -12,    16,-32768,   -62,   -54,   -53,
   -31,    27,    41,    51,   -24,-32768,  -129,    37
};


#define	YYLAST		663


static const short yytable[] = {    34,
    62,    67,   129,   172,    12,    76,   107,   246,   137,   191,
   210,     6,    12,    77,    82,   138,   140,    56,    58,   103,
    99,    68,    74,   289,   141,   142,     6,    65,   110,    22,
   293,   294,    22,    79,   218,    39,    34,   148,    96,   104,
   105,    69,    83,   105,   131,   202,   173,   143,   101,   102,
     2,   229,   151,   174,   175,    62,    62,   127,    39,    25,
   149,   137,   176,   177,    74,   290,   108,   158,   138,   140,
   192,   109,   100,    34,   153,   154,   155,   141,   142,   137,
   152,   185,   253,   156,   239,   178,   138,   140,   200,   243,
     6,   244,   116,   255,   139,   141,   142,    12,   233,   166,
   143,   234,    19,    62,   186,   144,    22,     1,    10,   100,
   190,   111,   112,   107,   113,     2,   218,   105,   143,   145,
    34,   110,   188,   137,    78,   137,    79,   197,   167,   146,
   138,   140,   138,   140,   227,   198,   199,   276,   277,   141,
   142,   141,   142,   179,   101,   163,   211,   139,   212,   117,
   157,   217,   284,   285,   123,   160,   110,   180,   144,   124,
   218,   213,   143,   126,   143,   139,   158,   181,   203,   221,
   150,   222,   145,   224,   262,   263,   144,   130,   172,   264,
   101,    22,   146,   231,   211,    83,   230,   163,   204,    34,
   145,   107,   105,    63,    64,   147,   107,   201,   300,    78,
   146,   238,   303,   304,   168,   188,   259,   260,   182,   139,
    19,   139,   107,   184,   299,   107,   211,   286,   254,    13,
   144,   173,   144,   274,   275,   115,   116,   189,   174,   175,
   193,   311,   107,   217,   145,   273,   145,   176,   177,   194,
   163,   183,   116,   163,   146,    93,   146,   163,   279,   280,
   232,   282,   280,   196,   281,   119,   121,   209,   207,   219,
   178,   223,   225,   235,   228,   236,     6,     7,     8,     9,
    10,    11,    85,    12,    13,   305,   240,   217,    14,    15,
    16,    17,    18,   113,    19,   298,   241,   298,   248,   247,
    20,   298,   298,   249,   250,   251,   211,   257,    21,   258,
    22,   261,    23,   271,   292,   306,   308,    24,   128,    25,
    26,    27,     6,     7,     8,     9,    10,    11,   179,    12,
    13,   295,   309,   310,    14,    15,    16,    17,    18,   313,
    19,   314,   180,   278,    59,   307,    20,   195,   242,    98,
   106,   159,   181,   237,    21,   302,    22,    75,    23,   226,
   120,   169,   268,    66,     0,    25,    26,    27,     6,     7,
     8,     9,    10,    11,     0,    12,    13,     0,   291,     0,
    14,    15,    16,    17,    18,     0,    19,     0,     0,     0,
     0,     0,    20,     0,     0,     0,     0,     0,     0,     0,
    21,   252,    22,     0,    23,     0,   256,     0,     0,    66,
     0,     0,    26,    27,     6,     7,     8,     9,    10,    11,
     0,    12,   270,     0,     0,   272,    14,    15,    16,    17,
    18,     0,    19,     0,     0,     0,     0,    85,    20,     0,
     0,     0,   283,     0,     0,     0,    21,     0,    22,     0,
    23,     0,     0,     0,     0,    66,     0,     0,    26,    27,
     6,     7,     8,     9,    10,    11,     0,    12,     0,     0,
     0,     0,    14,    15,    16,    17,    18,     0,    19,     0,
     0,     0,     0,     0,    20,     0,     0,     0,     0,     0,
     0,     0,    21,     0,    22,     0,    23,     0,     0,     0,
     0,    66,     0,   295,    26,    27,     6,     7,     8,     9,
    10,    11,     0,    12,     0,     0,     0,     0,    14,    15,
    16,    17,    18,     0,    19,     0,     0,     0,     0,     0,
    20,     0,     7,     8,     9,    10,    11,     0,    21,     0,
    22,     0,    23,    14,    15,    16,    17,    66,     0,    19,
    26,    27,     0,     0,     0,    20,     7,     8,     9,    10,
    11,     0,    12,     0,     0,     0,     0,    14,    15,    16,
    17,     6,     0,    19,     0,     0,   170,     0,     0,    20,
     0,     0,     0,     6,     0,     0,     0,     0,     0,     0,
     0,   132,     0,    84,     0,    86,    87,    88,   214,    90,
    91,    92,     6,     0,     0,    84,   215,    86,    87,    88,
   214,    90,    91,    92,     6,     0,     0,     0,   269,     0,
     0,     0,     0,     0,    84,     0,    86,    87,    88,   214,
    90,    91,    92,     0,     0,     0,    84,   301,    86,    87,
    88,    89,    90,    91,    92,    84,    85,    86,    87,    88,
    89,    90,    91,    92,    93,     0,     0,     0,     0,     0,
     0,     0,   128,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93
};

static const short yycheck[] = {     1,
    13,    21,    76,   117,    10,    25,    45,   220,    79,   131,
   160,     3,    10,    26,    27,    79,    79,     2,     3,    24,
    38,    12,    24,   277,    79,    79,     3,    20,    53,    37,
   284,   285,    37,    39,   164,     1,    38,    20,    42,    44,
    48,    32,    27,    48,    78,   151,   117,    79,    41,    42,
    31,   201,    44,   117,   117,    68,    69,    75,    24,    46,
    43,   132,   117,   117,    66,   278,    39,   106,   132,   132,
   132,    44,    38,    75,    94,    95,    96,   132,   132,   150,
    93,    32,   232,   103,   214,   117,   150,   150,   150,    11,
     3,    13,    43,    24,    79,   150,   150,    10,   204,   112,
   132,   207,    21,   116,   122,    79,    37,    23,     7,    75,
   130,    37,    39,   152,    41,    31,   246,    48,   150,    79,
   122,   146,   124,   194,    37,   196,    39,   147,   113,    79,
   194,   194,   196,   196,   196,   148,   149,   259,   260,   194,
   194,   196,   196,   117,   137,   109,    43,   132,    45,    24,
   104,   164,   274,   275,    40,   109,   181,   117,   132,    43,
   290,   163,   194,    45,   196,   150,   205,   117,    24,   182,
    39,   184,   132,   191,    28,    29,   150,    24,   292,    33,
   173,    37,   132,   203,    43,   170,    45,   151,    44,   191,
   150,   230,    48,     4,     5,    24,   235,   151,   289,    37,
   150,   214,   293,   294,    37,   207,   240,   241,    20,   194,
    21,   196,   251,    20,   288,   254,    43,     9,    45,    11,
   194,   292,   196,   257,   258,    42,    43,    45,   292,   292,
    40,   305,   271,   246,   194,   255,   196,   292,   292,    43,
   204,    42,    43,   207,   194,    34,   196,   211,    42,    43,
   204,    42,    43,    39,   267,    68,    69,    40,    43,    40,
   292,    38,    40,    45,    40,    45,     3,     4,     5,     6,
     7,     8,    26,    10,    11,   295,    37,   290,    15,    16,
    17,    18,    19,    41,    21,   287,    37,   289,    32,    42,
    27,   293,   294,    38,    40,    45,    43,    37,    35,    37,
    37,    37,    39,    45,    24,    38,    38,    44,    42,    46,
    47,    48,     3,     4,     5,     6,     7,     8,   292,    10,
    11,    46,    38,    38,    15,    16,    17,    18,    19,     0,
    21,     0,   292,   261,     4,   298,    27,   136,   219,    34,
    45,   108,   292,   211,    35,   292,    37,    24,    39,   194,
    69,   116,   244,    44,    -1,    46,    47,    48,     3,     4,
     5,     6,     7,     8,    -1,    10,    11,    -1,   280,    -1,
    15,    16,    17,    18,    19,    -1,    21,    -1,    -1,    -1,
    -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    35,   230,    37,    -1,    39,    -1,   235,    -1,    -1,    44,
    -1,    -1,    47,    48,     3,     4,     5,     6,     7,     8,
    -1,    10,   251,    -1,    -1,   254,    15,    16,    17,    18,
    19,    -1,    21,    -1,    -1,    -1,    -1,    26,    27,    -1,
    -1,    -1,   271,    -1,    -1,    -1,    35,    -1,    37,    -1,
    39,    -1,    -1,    -1,    -1,    44,    -1,    -1,    47,    48,
     3,     4,     5,     6,     7,     8,    -1,    10,    -1,    -1,
    -1,    -1,    15,    16,    17,    18,    19,    -1,    21,    -1,
    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    35,    -1,    37,    -1,    39,    -1,    -1,    -1,
    -1,    44,    -1,    46,    47,    48,     3,     4,     5,     6,
     7,     8,    -1,    10,    -1,    -1,    -1,    -1,    15,    16,
    17,    18,    19,    -1,    21,    -1,    -1,    -1,    -1,    -1,
    27,    -1,     4,     5,     6,     7,     8,    -1,    35,    -1,
    37,    -1,    39,    15,    16,    17,    18,    44,    -1,    21,
    47,    48,    -1,    -1,    -1,    27,     4,     5,     6,     7,
     8,    -1,    10,    -1,    -1,    -1,    -1,    15,    16,    17,
    18,     3,    -1,    21,    -1,    -1,    48,    -1,    -1,    27,
    -1,    -1,    -1,     3,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    39,    -1,    25,    -1,    27,    28,    29,    30,    31,
    32,    33,     3,    -1,    -1,    25,    38,    27,    28,    29,
    30,    31,    32,    33,     3,    -1,    -1,    -1,    38,    -1,
    -1,    -1,    -1,    -1,    25,    -1,    27,    28,    29,    30,
    31,    32,    33,    -1,    -1,    -1,    25,    38,    27,    28,
    29,    30,    31,    32,    33,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    42,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    34
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 35 "lang11d"
{ gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; ;
    break;}
case 2:
#line 37 "lang11d"
{ gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; ;
    break;}
case 3:
#line 39 "lang11d"
{ gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 2; ;
    break;}
case 4:
#line 42 "lang11d"
{ yyval = 0; ;
    break;}
case 5:
#line 44 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 7:
#line 49 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 8:
#line 53 "lang11d"
{ yyval = (long)newPyrClassNode((PyrSlotNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-4], 
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1], 0); 
				;
    break;}
case 9:
#line 57 "lang11d"
{ yyval = (long)newPyrClassNode((PyrSlotNode*)yyvsp[-8], (PyrSlotNode*)yyvsp[-4], 
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1], 
					(PyrSlotNode*)yyvsp[-6]); 
				;
    break;}
case 10:
#line 64 "lang11d"
{ 
					yyval = (long)newPyrClassExtNode((PyrSlotNode*)yyvsp[-3], (PyrMethodNode*)yyvsp[-1]); 
				;
    break;}
case 11:
#line 69 "lang11d"
{ yyval = 0; ;
    break;}
case 13:
#line 73 "lang11d"
{ yyval = 0; ;
    break;}
case 14:
#line 75 "lang11d"
{ yyval = yyvsp[0]; ;
    break;}
case 15:
#line 78 "lang11d"
{ yyval = 0; ;
    break;}
case 16:
#line 80 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 17:
#line 84 "lang11d"
{ yyval = (long)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varClass); ;
    break;}
case 18:
#line 86 "lang11d"
{ yyval = (long)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varInst); ;
    break;}
case 19:
#line 89 "lang11d"
{ yyval = 0; ;
    break;}
case 20:
#line 91 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 21:
#line 95 "lang11d"
{ yyval = (long)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2], 
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); ;
    break;}
case 22:
#line 98 "lang11d"
{ yyval = (long)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2], 
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); ;
    break;}
case 23:
#line 101 "lang11d"
{ yyval = (long)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2], 
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); ;
    break;}
case 24:
#line 104 "lang11d"
{ yyval = (long)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2], 
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); ;
    break;}
case 30:
#line 118 "lang11d"
{ yyval = (long)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 31:
#line 122 "lang11d"
{ yyval = (long)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); ;
    break;}
case 32:
#line 124 "lang11d"
{ yyval = (long)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); ;
    break;}
case 33:
#line 126 "lang11d"
{ yyval = (long)newPyrBlockNode(NULL, NULL, (PyrParseNode*)yyvsp[0], false); ;
    break;}
case 35:
#line 131 "lang11d"
{ yyval = (long)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 36:
#line 134 "lang11d"
{ yyval = 0; ;
    break;}
case 37:
#line 136 "lang11d"
{ yyval = yyvsp[-1]; ;
    break;}
case 38:
#line 140 "lang11d"
{ yyval = (long)newPyrReturnNode(NULL); ;
    break;}
case 39:
#line 142 "lang11d"
{ yyval = (long)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); ;
    break;}
case 40:
#line 146 "lang11d"
{ yyval = (long)newPyrBlockReturnNode(); ;
    break;}
case 41:
#line 148 "lang11d"
{ yyval = (long)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); ;
    break;}
case 43:
#line 153 "lang11d"
{ 
					yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); 
				;
    break;}
case 44:
#line 158 "lang11d"
{ yyval = 0; ;
    break;}
case 46:
#line 163 "lang11d"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], 0, 0); 
			;
    break;}
case 47:
#line 167 "lang11d"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-3], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 48:
#line 172 "lang11d"
{ 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				
				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				yyval = (long)newPyrCallNode(selectornode, (PyrParseNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 49:
#line 181 "lang11d"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-4], (PyrParseNode*)yyvsp[-6], 
					(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 50:
#line 186 "lang11d"
{ 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)yyvsp[-6]), 
					(PyrParseNode*)yyvsp[-3]);
				yyval = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 51:
#line 199 "lang11d"
{ 
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-3]);
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-5], args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 52:
#line 205 "lang11d"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[-3], 0, (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 56:
#line 214 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 57:
#line 216 "lang11d"
{ 
				PyrParseNode* argnode;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL);
				SetSymbol(&slot, s_envirGet);
				selectornode = newPyrSlotNode(&slot);
				yyval = (long)newPyrCallNode(selectornode, argnode, 0, 0); 
			;
    break;}
case 58:
#line 226 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 59:
#line 228 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 60:
#line 230 "lang11d"
{ 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)yyvsp[-3]);
				yyval = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)yyvsp[-1], 0); 
			;
    break;}
case 61:
#line 241 "lang11d"
{ 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)yyvsp[-4]), 
					(PyrParseNode*)yyvsp[-2]);
				yyval = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)yyvsp[-1], 0); 
			;
    break;}
case 62:
#line 254 "lang11d"
{ yyval = yyvsp[-1]; ;
    break;}
case 63:
#line 256 "lang11d"
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
				yyval = (long)newPyrCallNode(slotnode, args, 0, 0); 
			;
    break;}
case 64:
#line 270 "lang11d"
{ 
				yyval = (long)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-1], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 65:
#line 275 "lang11d"
{ 
				yyval = (long)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-1], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 66:
#line 280 "lang11d"
{ yyval = (long)newPyrDynListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 67:
#line 282 "lang11d"
{ yyval = (long)newPyrDynListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 68:
#line 284 "lang11d"
{ 
				yyval = (long)newPyrAssignNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); 
			;
    break;}
case 69:
#line 288 "lang11d"
{ 
				PyrParseNode *argnode, *args;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)yyvsp[-2], NULL);
				args = linkNextNode(argnode, (PyrParseNode*)yyvsp[0]);
				SetSymbol(&slot, s_envirPut);
				selectornode = newPyrSlotNode(&slot);
				yyval = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;
    break;}
case 70:
#line 299 "lang11d"
{ 
				yyval = (long)newPyrSetterNode((PyrSlotNode*)yyvsp[-2], 
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 71:
#line 304 "lang11d"
{ 
				if (yyvsp[-3] != 0) {
					error("Setter method called with keyword arguments.\n");
					nodePostErrorLine((PyrParseNode*)yyvsp[-3]);
					compileErrors++;
				}
				yyval = (long)newPyrSetterNode((PyrSlotNode*)yyvsp[-6], 
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 72:
#line 314 "lang11d"
{ 
				yyval = (long)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[0], 0); 
			;
    break;}
case 74:
#line 322 "lang11d"
{ 
				yyval = (long)newPyrDropNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 76:
#line 330 "lang11d"
{ yyval = 0; ;
    break;}
case 79:
#line 336 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 81:
#line 341 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 82:
#line 345 "lang11d"
{ yyval = (long)newPyrPushKeyArgNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 83:
#line 348 "lang11d"
{ yyval = 0; ;
    break;}
case 84:
#line 349 "lang11d"
{ yyval = yyvsp[0]; ;
    break;}
case 85:
#line 353 "lang11d"
{ yyval = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 86:
#line 355 "lang11d"
{ yyval = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); ;
    break;}
case 88:
#line 360 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 89:
#line 364 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 90:
#line 365 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 91:
#line 366 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 92:
#line 367 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 93:
#line 368 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 94:
#line 369 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 95:
#line 370 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 96:
#line 371 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 97:
#line 372 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 98:
#line 373 "lang11d"
{ yyval = (long)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 99:
#line 376 "lang11d"
{ yyval = (long)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 100:
#line 379 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 101:
#line 380 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 102:
#line 381 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 103:
#line 382 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 104:
#line 383 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 105:
#line 384 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 106:
#line 385 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 107:
#line 386 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 108:
#line 387 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 109:
#line 388 "lang11d"
{ yyval = (long)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 110:
#line 391 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 111:
#line 392 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 112:
#line 393 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 113:
#line 394 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 114:
#line 395 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 115:
#line 396 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 116:
#line 397 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 117:
#line 398 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 118:
#line 399 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 119:
#line 400 "lang11d"
{ yyval = (long)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 120:
#line 404 "lang11d"
{ yyval = (long)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[-1], false); ;
    break;}
case 121:
#line 407 "lang11d"
{ yyval = (long)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[-1], true); ;
    break;}
case 122:
#line 411 "lang11d"
{ yyval = 0; ;
    break;}
case 123:
#line 413 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 125:
#line 418 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 126:
#line 422 "lang11d"
{ yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); ;
    break;}
case 127:
#line 425 "lang11d"
{ yyval = 0; ;
    break;}
case 128:
#line 427 "lang11d"
{
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); 
				;
    break;}
case 129:
#line 431 "lang11d"
{ 
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]); 
				;
    break;}
case 130:
#line 435 "lang11d"
{
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); 
				;
    break;}
case 131:
#line 439 "lang11d"
{ 
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]); 
				;
    break;}
case 132:
#line 444 "lang11d"
{ yyval = 0; ;
    break;}
case 135:
#line 450 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 136:
#line 454 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); ;
    break;}
case 137:
#line 456 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], 0); ;
    break;}
case 139:
#line 461 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 140:
#line 465 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); ;
    break;}
case 141:
#line 467 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], yyvsp[-3]); ;
    break;}
case 142:
#line 473 "lang11d"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 143:
#line 475 "lang11d"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 144:
#line 479 "lang11d"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 145:
#line 481 "lang11d"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 146:
#line 484 "lang11d"
{ yyval = 0; ;
    break;}
case 149:
#line 490 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 150:
#line 493 "lang11d"
{ yyval = rwPrivate; ;
    break;}
case 151:
#line 495 "lang11d"
{ yyval = rwReadOnly; ;
    break;}
case 152:
#line 497 "lang11d"
{ yyval = rwReadWrite; ;
    break;}
case 153:
#line 499 "lang11d"
{ yyval = rwWriteOnly; ;
    break;}
case 154:
#line 502 "lang11d"
{ yyval = zzval; ;
    break;}
case 155:
#line 504 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.ui = -node->slot.ui;
				yyval = zzval;
			;
    break;}
case 156:
#line 512 "lang11d"
{ yyval = zzval; ;
    break;}
case 157:
#line 514 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -node->slot.uf;
				yyval = zzval;
			;
    break;}
case 158:
#line 522 "lang11d"
{ yyval = zzval; ;
    break;}
case 160:
#line 527 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)yyvsp[-1];
				node->slot.uf *= pi;
			;
    break;}
case 161:
#line 533 "lang11d"
{
				PyrSlotNode *node; 
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = node->slot.ui;
				node->slot.uf = ival * pi;
			;
    break;}
case 162:
#line 541 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = pi;
				yyval = zzval;
			;
    break;}
case 163:
#line 548 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -pi;
				yyval = zzval;
			;
    break;}
case 164:
#line 556 "lang11d"
{ yyval = zzval; ;
    break;}
case 165:
#line 559 "lang11d"
{ yyval = zzval; ;
    break;}
case 166:
#line 562 "lang11d"
{ yyval = zzval; ;
    break;}
case 167:
#line 565 "lang11d"
{ yyval = zzval; ;
    break;}
case 168:
#line 568 "lang11d"
{ yyval = zzval; ;
    break;}
case 169:
#line 571 "lang11d"
{ yyval = zzval; ;
    break;}
case 170:
#line 574 "lang11d"
{ yyval = zzval; ;
    break;}
case 171:
#line 577 "lang11d"
{ yyval = zzval; ;
    break;}
case 172:
#line 580 "lang11d"
{ yyval = zzval; ;
    break;}
case 174:
#line 585 "lang11d"
{
					PyrSlotNode	*node1, *node2, *node3;
					PyrSlot slot;
					PyrString *string;
					char *str;
					long len1, len2, len3;
					
					node1 = (PyrSlotNode*)yyvsp[-1];
					node2 = (PyrSlotNode*)yyvsp[0];
					len1 = node1->slot.uo->size;
					len2 = node2->slot.uo->size;
					len3 = len1 + len2;
					string = newPyrStringN(NULL, len3, obj_permanent | obj_immutable, false);
					memcpy(string->s,        node1->slot.uos->s, len1);
					memcpy(string->s + len1, node2->slot.uos->s, len2);
					
					pyr_pool_runtime->Free(node1->slot.uo);
					pyr_pool_runtime->Free(node2->slot.uo);
					
					SetObject(&slot, string);
					node3 = newPyrSlotNode(&slot);
					yyval = (long)node3;
				;
    break;}
case 175:
#line 610 "lang11d"
{ yyval = zzval; ;
    break;}
case 176:
#line 613 "lang11d"
{ yyval = zzval; ;
    break;}
case 177:
#line 616 "lang11d"
{ yyval = zzval; ;
    break;}
case 178:
#line 617 "lang11d"
{ yyval = zzval; ;
    break;}
case 179:
#line 618 "lang11d"
{ yyval = zzval; ;
    break;}
case 180:
#line 619 "lang11d"
{ yyval = zzval; ;
    break;}
case 181:
#line 620 "lang11d"
{ yyval = zzval; ;
    break;}
case 182:
#line 621 "lang11d"
{ yyval = zzval; ;
    break;}
case 183:
#line 622 "lang11d"
{ yyval = zzval; ;
    break;}
case 184:
#line 623 "lang11d"
{ yyval = zzval; ;
    break;}
case 185:
#line 626 "lang11d"
{ yyval = zzval; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 629 "lang11d"
