
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



#define	YYFINAL		320
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
   177,   186,   191,   193,   195,   197,   199,   204,   208,   211,
   215,   220,   222,   224,   229,   235,   238,   242,   246,   250,
   255,   261,   269,   274,   281,   283,   287,   290,   291,   294,
   296,   300,   302,   306,   309,   310,   313,   315,   319,   321,
   325,   327,   329,   331,   333,   335,   337,   339,   341,   343,
   345,   347,   349,   351,   353,   355,   357,   359,   361,   363,
   365,   367,   369,   371,   373,   375,   377,   379,   381,   383,
   385,   387,   393,   400,   401,   404,   406,   409,   413,   414,
   418,   424,   428,   434,   435,   437,   439,   443,   445,   449,
   451,   455,   458,   463,   468,   474,   478,   483,   484,   487,
   489,   493,   494,   496,   498,   500,   502,   505,   507,   510,
   512,   514,   517,   520,   522,   525,   527,   529,   531,   533,
   535,   537,   539,   541,   543,   545,   548,   550,   552,   554,
   556,   558,   560,   562,   564,   566,   568
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
    70,     0,    83,     0,    82,     0,   104,     0,   104,    39,
    75,    40,     0,    44,    73,    45,     0,    47,   104,     0,
    39,    74,    40,     0,   105,    39,    74,    40,     0,   115,
     0,   105,     0,   105,    44,    76,    45,     0,   105,    44,
    75,    78,    45,     0,    35,    71,     0,    71,   116,    71,
     0,    71,   117,    71,     0,   104,    24,    71,     0,    47,
   104,    24,    71,     0,    71,    34,   104,    24,    71,     0,
   104,    44,    75,    78,    45,    24,    71,     0,    48,    79,
    24,    71,     0,   104,    39,    75,    40,    24,    71,     0,
    71,     0,    72,    42,    71,     0,    72,    60,     0,     0,
    75,    61,     0,    73,     0,    75,    43,    73,     0,    77,
     0,    76,    43,    77,     0,   117,    73,     0,     0,    43,
    76,     0,    80,     0,    80,    20,   104,     0,   104,     0,
    80,    43,   104,     0,   100,     0,   103,     0,   111,     0,
   113,     0,   112,     0,   107,     0,   108,     0,   109,     0,
   110,     0,    95,     0,    85,     0,   100,     0,   103,     0,
   111,     0,   113,     0,   112,     0,   107,     0,   108,     0,
   109,     0,   110,     0,    95,     0,   100,     0,   103,     0,
   111,     0,   113,     0,   112,     0,   107,     0,   108,     0,
   109,     0,   110,     0,    96,     0,    37,    89,    86,    62,
    38,     0,    48,    37,    89,    86,    62,    38,     0,     0,
    86,    88,     0,    88,     0,    87,    88,     0,    11,    91,
    42,     0,     0,    12,    91,    42,     0,    12,    90,    20,
   104,    42,     0,    32,    91,    32,     0,    32,    90,    20,
   104,    32,     0,     0,    91,     0,    92,     0,    91,    43,
    92,     0,   104,     0,   104,    24,    81,     0,    94,     0,
    93,    43,    94,     0,    99,   104,     0,    99,   104,    24,
    81,     0,    48,    39,    97,    40,     0,    48,   105,    39,
    97,    40,     0,    39,    97,    40,     0,   105,    39,    97,
    40,     0,     0,    98,    61,     0,    84,     0,    98,    43,
    84,     0,     0,    28,     0,    33,     0,    29,     0,     4,
     0,    27,     4,     0,     5,     0,    27,     5,     0,    21,
     0,   101,     0,   101,   102,     0,   100,   102,     0,   102,
     0,    27,   102,     0,     3,     0,    10,     0,     9,     0,
    16,     0,    17,     0,    15,     0,    18,     0,     8,     0,
     6,     0,   114,     0,   113,   114,     0,     7,     0,    19,
     0,    25,     0,    33,     0,    28,     0,    29,     0,    27,
     0,    30,     0,    31,     0,    32,     0,    26,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    34,    36,    38,    42,    43,    47,    48,    52,    56,    63,
    69,    70,    73,    74,    78,    79,    83,    85,    89,    90,
    94,    97,   100,   103,   108,   109,   112,   113,   116,   117,
   121,   123,   125,   129,   130,   134,   135,   139,   141,   145,
   147,   151,   152,   158,   159,   162,   166,   171,   180,   185,
   198,   204,   210,   211,   212,   213,   215,   228,   230,   240,
   242,   244,   246,   248,   259,   272,   286,   291,   296,   300,
   311,   316,   326,   331,   347,   348,   354,   357,   358,   361,
   362,   366,   367,   371,   375,   376,   379,   381,   385,   386,
   391,   392,   393,   394,   395,   396,   397,   398,   399,   400,
   403,   406,   407,   408,   409,   410,   411,   412,   413,   414,
   415,   418,   419,   420,   421,   422,   423,   424,   425,   426,
   427,   430,   433,   438,   439,   443,   444,   448,   452,   453,
   457,   461,   465,   471,   472,   475,   476,   480,   482,   486,
   487,   491,   493,   499,   501,   505,   507,   511,   512,   515,
   516,   520,   521,   523,   525,   529,   530,   539,   540,   549,
   552,   553,   559,   567,   574,   583,   586,   589,   592,   595,
   598,   601,   604,   607,   610,   611,   637,   640,   643,   644,
   645,   646,   647,   648,   649,   650,   653
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
    71,    71,    71,    71,    72,    72,    73,    74,    74,    75,
    75,    76,    76,    77,    78,    78,    79,    79,    80,    80,
    81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
    82,    83,    83,    83,    83,    83,    83,    83,    83,    83,
    83,    84,    84,    84,    84,    84,    84,    84,    84,    84,
    84,    85,    85,    86,    86,    87,    87,    88,    89,    89,
    89,    89,    89,    90,    90,    91,    91,    92,    92,    93,
    93,    94,    94,    95,    95,    96,    96,    97,    97,    98,
    98,    99,    99,    99,    99,   100,   100,   101,   101,   102,
   103,   103,   103,   103,   103,   104,   105,   106,   107,   108,
   109,   110,   111,   112,   113,   113,   114,   115,   116,   116,
   116,   116,   116,   116,   116,   116,   117
};

static const short yyr2[] = {     0,
     1,     1,     2,     0,     2,     1,     2,     6,     9,     5,
     0,     1,     0,     2,     0,     2,     3,     3,     0,     2,
     7,     8,     7,     8,     0,     1,     0,     1,     1,     2,
     4,     2,     1,     1,     2,     0,     2,     0,     3,     0,
     3,     1,     2,     0,     1,     2,     6,     6,     7,     7,
     8,     4,     1,     1,     1,     1,     4,     3,     2,     3,
     4,     1,     1,     4,     5,     2,     3,     3,     3,     4,
     5,     7,     4,     6,     1,     3,     2,     0,     2,     1,
     3,     1,     3,     2,     0,     2,     1,     3,     1,     3,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     5,     6,     0,     2,     1,     2,     3,     0,     3,
     5,     3,     5,     0,     1,     1,     3,     1,     3,     1,
     3,     2,     4,     4,     5,     3,     4,     0,     2,     1,
     3,     0,     1,     1,     1,     1,     2,     1,     2,     1,
     1,     2,     2,     1,     2,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     2,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1
};

static const short yydefact[] = {     4,
    40,     0,     1,     2,     6,   166,   156,   158,   174,   177,
   173,   167,     0,   171,   169,   170,   172,   178,   160,     0,
     0,   129,    78,     0,     0,     0,     0,    33,     3,    29,
    53,    75,    25,    40,    55,    54,   101,    40,   126,   111,
   102,   161,   164,   103,    56,    63,   107,   108,   109,   110,
   104,   106,   105,   175,    62,     0,     5,    13,     7,     0,
   136,   138,   157,   159,   165,     0,    66,   134,   134,   124,
    80,     0,    27,     0,    40,    25,    59,   129,   148,     0,
    87,    89,     0,   179,   187,   183,   181,   182,   184,   185,
   186,   180,     0,     0,     0,    26,    77,    30,    32,   127,
   163,   162,     0,     0,     0,     0,    46,    42,    78,     0,
   176,    19,    11,     0,     0,   128,     0,     0,     0,   135,
     0,   135,    40,    60,    28,    79,    58,     0,    26,    41,
     0,   124,   148,   150,   121,     0,    27,   112,   113,     0,
   117,   118,   119,   120,   114,   116,   115,     0,     0,     0,
   148,     0,    44,    67,    68,    76,    69,     0,    85,    43,
     0,    85,     0,    82,     0,     0,     0,    12,    14,    15,
   137,     0,   139,   100,    91,    92,    96,    97,    98,    99,
    93,    95,    94,     0,   130,     0,   132,     0,   125,    81,
    31,    70,    40,     0,   144,    28,   149,   148,    73,    88,
    90,     0,    85,     0,     0,     0,    45,    52,    57,     0,
     0,     0,    61,     0,     0,    64,    84,   184,    10,    20,
     0,     0,    13,    19,     0,     0,   122,     0,   146,   151,
     0,   145,     0,    44,    71,    85,     0,     0,    86,    44,
    65,    83,     0,     0,   129,   129,     0,   152,   152,    16,
     0,   131,   133,   123,   147,    44,    48,     0,    44,    74,
     0,    47,   129,   129,   124,   124,    15,   153,   155,   154,
     0,   140,     0,     0,     8,    50,    44,    49,    72,   124,
   124,    36,    36,    19,    18,   152,   142,    17,    51,    36,
    36,   168,    38,    25,    38,     0,   141,     0,    38,    38,
     0,     0,    34,    38,    37,     0,     9,   143,     0,     0,
    25,    21,    35,    23,    22,    24,    39,     0,     0,     0
};

static const short yydefgoto[] = {   318,
     3,     4,    57,     5,   167,   115,   224,   250,   166,   220,
    97,   126,    28,    29,   302,   293,   303,    30,   207,   208,
    31,    32,    33,    71,    72,    73,   163,   164,   212,    80,
    81,   173,    35,    36,   134,    37,   123,    38,   189,    70,
   119,    60,    61,   271,   272,    40,   135,   136,   137,   273,
    41,    42,    43,    44,    45,    46,   294,    47,    48,    49,
    50,    51,    52,    53,    54,    55,    94,    95
};

static const short yypact[] = {    68,
   319,    -3,    -3,    23,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    69,-32768,-32768,-32768,-32768,-32768,-32768,   176,
   549,    21,   549,   411,   549,    69,   116,-32768,-32768,-32768,
-32768,   653,    24,    88,-32768,-32768,-32768,   365,-32768,-32768,
   119,   119,-32768,-32768,    -5,    50,-32768,-32768,-32768,-32768,
-32768,-32768,   138,-32768,-32768,   114,-32768,   -10,-32768,   209,
-32768,   145,-32768,-32768,-32768,   549,-32768,    69,    69,-32768,
-32768,   122,   136,   137,   365,   635,   165,    21,   598,   170,
    30,-32768,   161,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    25,   549,   549,   549,-32768,-32768,-32768,-32768,
-32768,-32768,   549,   549,   549,   192,    63,-32768,   549,   457,
-32768,-32768,    69,    -3,   194,-32768,    69,   574,   220,   214,
   225,    96,   365,-32768,   549,-32768,-32768,   202,-32768,-32768,
   549,-32768,   598,-32768,-32768,   226,   191,   119,-32768,   210,
-32768,-32768,-32768,-32768,-32768,-32768,   138,   549,    69,    69,
   598,   457,   189,   228,   228,   653,   653,    78,   211,-32768,
   229,   211,   123,-32768,   549,   243,   237,-32768,-32768,-32768,
-32768,     1,-32768,-32768,   119,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   138,    69,-32768,    69,-32768,   242,-32768,-32768,
-32768,   653,   365,   245,-32768,   598,-32768,   598,   653,-32768,
-32768,   246,   211,   162,   549,   457,    63,-32768,   259,   549,
   457,   244,-32768,   248,   261,-32768,-32768,   626,-32768,-32768,
   251,   253,   250,   201,   254,   263,-32768,   271,-32768,-32768,
   270,-32768,   256,    63,   653,   211,   179,   549,   268,    -7,
-32768,-32768,   277,   278,    21,    21,   279,    75,    75,-32768,
   275,-32768,-32768,-32768,-32768,    63,-32768,   267,    63,   653,
   549,-32768,    21,    21,-32768,-32768,-32768,-32768,-32768,-32768,
   216,-32768,    69,   222,-32768,-32768,    63,-32768,   653,-32768,
-32768,   219,   219,   201,-32768,    75,   293,-32768,-32768,   219,
   219,-32768,   503,   286,   503,   614,-32768,   574,   503,   503,
   549,   281,-32768,   274,-32768,   283,-32768,-32768,   294,   295,
   635,-32768,-32768,-32768,-32768,-32768,-32768,   331,   339,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,   337,-32768,   120,    80,-32768,  -202,-32768,
   -73,   208,   -17,-32768,  -112,  -245,    44,   315,   306,   -41,
-32768,   -19,-32768,    -1,   252,     5,  -116,   142,  -144,-32768,
-32768,    54,   -36,-32768,   157,-32768,   -64,   335,    11,   -68,
   291,    90,   247,   113,    91,  -114,-32768,   -91,-32768,-32768,
   -71,-32768,   -15,   -66,   -12,   146,-32768,   -63,   -55,   -54,
   -35,   -31,    52,    57,   -33,-32768,  -105,   -59
};


#define	YYLAST		687


static const short yytable[] = {    34,
    62,    67,   130,   174,    65,    76,    12,   138,   108,   132,
    12,    39,   139,    77,    82,   141,   261,   214,   103,   111,
    99,   251,    74,   142,   143,   101,   102,     6,   113,    22,
   114,    22,    68,   104,    39,   204,    34,   295,   105,    79,
   106,   194,   106,   144,   299,   300,   175,   145,   100,   149,
   165,   176,    69,     2,   177,    62,    62,   128,   233,   202,
   222,   138,   178,   179,    74,    96,   139,   193,   152,   141,
   160,     6,   150,    34,   154,   155,   156,   142,   143,   138,
   153,   296,   180,   157,   139,   100,   181,   141,   109,   237,
     1,   258,   165,   110,   239,   142,   143,   144,     2,    22,
   168,   145,   268,   269,    62,   188,   231,   270,   158,   159,
   106,   192,   244,   111,   162,   144,   108,   209,     6,   145,
   210,    34,   101,   190,   138,    12,   138,   187,   199,   139,
   146,   139,   141,    25,   141,   147,   200,   201,   117,    19,
   142,   143,   142,   143,    10,   222,   165,    56,    58,   111,
   112,   165,    78,   221,    79,   165,   203,   120,   122,   101,
   144,   124,   144,   217,   145,   215,   145,   216,   118,   182,
   160,   225,    83,   226,   183,   228,   265,   266,   125,    63,
    64,   127,   306,   174,   146,   235,   309,   310,   131,   147,
   222,    34,   257,   148,   280,   281,    19,   108,   262,   151,
   282,   283,   146,   108,   215,   243,   234,   147,   190,   190,
   236,   248,   205,   249,   276,   290,   291,   278,   260,   108,
   305,   215,   108,   259,   140,    22,   175,   292,    78,    13,
   170,   176,   206,   196,   177,   289,   106,   317,   221,   184,
   108,   279,   178,   179,   186,     6,   191,   146,   198,   146,
   116,   117,   147,   211,   147,   185,   117,   285,   286,   169,
   287,    93,   180,   288,   286,   195,   181,    84,   213,    86,
    87,    88,   218,    90,    91,    92,   223,     6,   140,   227,
   219,   311,   238,   221,   229,   232,    85,   245,   240,   246,
   114,   304,   241,   304,   253,   252,   140,   304,   304,    84,
   256,    86,    87,    88,   218,    90,    91,    92,   254,   255,
   215,   277,   275,   263,   264,   267,   298,    83,   312,   301,
   314,     6,     7,     8,     9,    10,    11,   129,    12,    13,
   319,   315,   316,    14,    15,    16,    17,    18,   320,    19,
    59,   140,   247,   140,   197,    20,   284,   313,    98,   182,
   107,   308,   230,    21,   183,    22,   242,    23,    75,   121,
   161,   274,    24,   171,    25,    26,    27,     6,     7,     8,
     9,    10,    11,     0,    12,    13,   297,     0,     0,    14,
    15,    16,    17,    18,     0,    19,     0,     0,     0,     0,
     0,    20,     0,     0,     0,     0,     0,     0,     0,    21,
     0,    22,     0,    23,     0,     0,     0,     0,    66,     0,
    25,    26,    27,     6,     7,     8,     9,    10,    11,     0,
    12,    13,     0,     0,     0,    14,    15,    16,    17,    18,
     0,    19,     0,     0,     0,     0,     0,    20,     0,     0,
     0,     0,     0,     0,     0,    21,     0,    22,     0,    23,
     0,     0,     0,     0,    66,     0,     0,    26,    27,     6,
     7,     8,     9,    10,    11,     0,    12,     0,     0,     0,
     0,    14,    15,    16,    17,    18,     0,    19,     0,     0,
     0,     0,    85,    20,     0,     0,     0,     0,     0,     0,
     0,    21,     0,    22,     0,    23,     0,     0,     0,     0,
    66,     0,     0,    26,    27,     6,     7,     8,     9,    10,
    11,     0,    12,     0,     0,     0,     0,    14,    15,    16,
    17,    18,     0,    19,     0,     0,     0,     0,     0,    20,
     0,     0,     0,     0,     0,     0,     0,    21,     0,    22,
     0,    23,     0,     0,     0,     0,    66,     0,   301,    26,
    27,     6,     7,     8,     9,    10,    11,     0,    12,     0,
     0,     0,     0,    14,    15,    16,    17,    18,     0,    19,
     0,     0,     0,     0,     0,    20,     0,     7,     8,     9,
    10,    11,     0,    21,     0,    22,     0,    23,    14,    15,
    16,    17,    66,     0,    19,    26,    27,     0,     0,     0,
    20,     7,     8,     9,    10,    11,     0,    12,     0,     0,
     0,     0,    14,    15,    16,    17,     6,     0,    19,     0,
     0,   172,     0,     0,    20,     0,     0,     0,     6,     0,
     0,     0,     0,     0,     0,     0,   133,     0,    84,     0,
    86,    87,    88,   218,    90,    91,    92,     0,     0,     0,
    84,   307,    86,    87,    88,    89,    90,    91,    92,    84,
    85,    86,    87,    88,    89,    90,    91,    92,    93,     0,
     0,     0,     0,     0,     0,     0,   129,    84,    85,    86,
    87,    88,    89,    90,    91,    92,    93
};

static const short yycheck[] = {     1,
    13,    21,    76,   118,    20,    25,    10,    79,    45,    78,
    10,     1,    79,    26,    27,    79,    24,   162,    24,    53,
    38,   224,    24,    79,    79,    41,    42,     3,    39,    37,
    41,    37,    12,    39,    24,   152,    38,   283,    44,    39,
    48,   133,    48,    79,   290,   291,   118,    79,    38,    20,
   110,   118,    32,    31,   118,    68,    69,    75,   203,   151,
   166,   133,   118,   118,    66,    42,   133,   132,    44,   133,
   107,     3,    43,    75,    94,    95,    96,   133,   133,   151,
    93,   284,   118,   103,   151,    75,   118,   151,    39,   206,
    23,   236,   152,    44,   211,   151,   151,   133,    31,    37,
   113,   133,    28,    29,   117,   123,   198,    33,   104,   105,
    48,   131,   218,   147,   110,   151,   153,    40,     3,   151,
    43,   123,   138,   125,   196,    10,   198,    32,   148,   196,
    79,   198,   196,    46,   198,    79,   149,   150,    43,    21,
   196,   196,   198,   198,     7,   251,   206,     2,     3,   183,
    37,   211,    37,   166,    39,   215,   152,    68,    69,   175,
   196,    40,   198,   165,   196,    43,   198,    45,    24,   118,
   207,   184,    27,   186,   118,   193,   245,   246,    43,     4,
     5,    45,   295,   298,   133,   205,   299,   300,    24,   133,
   296,   193,   234,    24,   263,   264,    21,   234,   240,    39,
   265,   266,   151,   240,    43,   218,    45,   151,   210,   211,
   206,    11,    24,    13,   256,   280,   281,   259,   238,   256,
   294,    43,   259,    45,    79,    37,   298,     9,    37,    11,
    37,   298,    44,    43,   298,   277,    48,   311,   251,    20,
   277,   261,   298,   298,    20,     3,    45,   196,    39,   198,
    42,    43,   196,    43,   198,    42,    43,    42,    43,   114,
   273,    34,   298,    42,    43,    40,   298,    25,    40,    27,
    28,    29,    30,    31,    32,    33,    40,     3,   133,    38,
    38,   301,    24,   296,    40,    40,    26,    37,    45,    37,
    41,   293,    45,   295,    32,    42,   151,   299,   300,    25,
    45,    27,    28,    29,    30,    31,    32,    33,    38,    40,
    43,    45,    38,    37,    37,    37,    24,   172,    38,    46,
    38,     3,     4,     5,     6,     7,     8,    42,    10,    11,
     0,    38,    38,    15,    16,    17,    18,    19,     0,    21,
     4,   196,   223,   198,   137,    27,   267,   304,    34,   298,
    45,   298,   196,    35,   298,    37,   215,    39,    24,    69,
   109,   249,    44,   117,    46,    47,    48,     3,     4,     5,
     6,     7,     8,    -1,    10,    11,   286,    -1,    -1,    15,
    16,    17,    18,    19,    -1,    21,    -1,    -1,    -1,    -1,
    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,
    -1,    37,    -1,    39,    -1,    -1,    -1,    -1,    44,    -1,
    46,    47,    48,     3,     4,     5,     6,     7,     8,    -1,
    10,    11,    -1,    -1,    -1,    15,    16,    17,    18,    19,
    -1,    21,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    35,    -1,    37,    -1,    39,
    -1,    -1,    -1,    -1,    44,    -1,    -1,    47,    48,     3,
     4,     5,     6,     7,     8,    -1,    10,    -1,    -1,    -1,
    -1,    15,    16,    17,    18,    19,    -1,    21,    -1,    -1,
    -1,    -1,    26,    27,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    35,    -1,    37,    -1,    39,    -1,    -1,    -1,    -1,
    44,    -1,    -1,    47,    48,     3,     4,     5,     6,     7,
     8,    -1,    10,    -1,    -1,    -1,    -1,    15,    16,    17,
    18,    19,    -1,    21,    -1,    -1,    -1,    -1,    -1,    27,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    -1,    37,
    -1,    39,    -1,    -1,    -1,    -1,    44,    -1,    46,    47,
    48,     3,     4,     5,     6,     7,     8,    -1,    10,    -1,
    -1,    -1,    -1,    15,    16,    17,    18,    19,    -1,    21,
    -1,    -1,    -1,    -1,    -1,    27,    -1,     4,     5,     6,
     7,     8,    -1,    35,    -1,    37,    -1,    39,    15,    16,
    17,    18,    44,    -1,    21,    47,    48,    -1,    -1,    -1,
    27,     4,     5,     6,     7,     8,    -1,    10,    -1,    -1,
    -1,    -1,    15,    16,    17,    18,     3,    -1,    21,    -1,
    -1,    48,    -1,    -1,    27,    -1,    -1,    -1,     3,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    39,    -1,    25,    -1,
    27,    28,    29,    30,    31,    32,    33,    -1,    -1,    -1,
    25,    38,    27,    28,    29,    30,    31,    32,    33,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    42,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34
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
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)yyvsp[-3]), 
					(PyrParseNode*)yyvsp[-1]);
				yyval = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;
    break;}
case 58:
#line 229 "lang11d"
{ yyval = yyvsp[-1]; ;
    break;}
case 59:
#line 231 "lang11d"
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
case 60:
#line 241 "lang11d"
{ yyval = (long)newPyrDynListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 61:
#line 243 "lang11d"
{ yyval = (long)newPyrDynListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 62:
#line 245 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 63:
#line 247 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 64:
#line 249 "lang11d"
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
case 65:
#line 260 "lang11d"
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
case 66:
#line 273 "lang11d"
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
case 67:
#line 287 "lang11d"
{ 
				yyval = (long)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-1], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 68:
#line 292 "lang11d"
{ 
				yyval = (long)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-1], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 69:
#line 297 "lang11d"
{ 
				yyval = (long)newPyrAssignNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); 
			;
    break;}
case 70:
#line 301 "lang11d"
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
case 71:
#line 312 "lang11d"
{ 
				yyval = (long)newPyrSetterNode((PyrSlotNode*)yyvsp[-2], 
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 72:
#line 317 "lang11d"
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
case 73:
#line 327 "lang11d"
{ 
				yyval = (long)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[0], 0); 
			;
    break;}
case 74:
#line 332 "lang11d"
{ 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_put);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)yyvsp[-5]), 
					(PyrParseNode*)yyvsp[-3]);
				args = linkNextNode( args, (PyrParseNode*)yyvsp[0]);
				yyval = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;
    break;}
case 76:
#line 349 "lang11d"
{ 
				yyval = (long)newPyrDropNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 78:
#line 357 "lang11d"
{ yyval = 0; ;
    break;}
case 81:
#line 363 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 83:
#line 368 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 84:
#line 372 "lang11d"
{ yyval = (long)newPyrPushKeyArgNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 85:
#line 375 "lang11d"
{ yyval = 0; ;
    break;}
case 86:
#line 376 "lang11d"
{ yyval = yyvsp[0]; ;
    break;}
case 87:
#line 380 "lang11d"
{ yyval = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 88:
#line 382 "lang11d"
{ yyval = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); ;
    break;}
case 90:
#line 387 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 91:
#line 391 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 92:
#line 392 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 93:
#line 393 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 94:
#line 394 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 95:
#line 395 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 96:
#line 396 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 97:
#line 397 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 98:
#line 398 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 99:
#line 399 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 100:
#line 400 "lang11d"
{ yyval = (long)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 101:
#line 403 "lang11d"
{ yyval = (long)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 102:
#line 406 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 103:
#line 407 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 104:
#line 408 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 105:
#line 409 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 106:
#line 410 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 107:
#line 411 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 108:
#line 412 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 109:
#line 413 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 110:
#line 414 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 111:
#line 415 "lang11d"
{ yyval = (long)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 112:
#line 418 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 113:
#line 419 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 114:
#line 420 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 115:
#line 421 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 116:
#line 422 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 117:
#line 423 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 118:
#line 424 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 119:
#line 425 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 120:
#line 426 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 121:
#line 427 "lang11d"
{ yyval = (long)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 122:
#line 431 "lang11d"
{ yyval = (long)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[-1], false); ;
    break;}
case 123:
#line 434 "lang11d"
{ yyval = (long)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[-1], true); ;
    break;}
case 124:
#line 438 "lang11d"
{ yyval = 0; ;
    break;}
case 125:
#line 440 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 127:
#line 445 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 128:
#line 449 "lang11d"
{ yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); ;
    break;}
case 129:
#line 452 "lang11d"
{ yyval = 0; ;
    break;}
case 130:
#line 454 "lang11d"
{
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); 
				;
    break;}
case 131:
#line 458 "lang11d"
{ 
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]); 
				;
    break;}
case 132:
#line 462 "lang11d"
{
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); 
				;
    break;}
case 133:
#line 466 "lang11d"
{ 
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]); 
				;
    break;}
case 134:
#line 471 "lang11d"
{ yyval = 0; ;
    break;}
case 137:
#line 477 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 138:
#line 481 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); ;
    break;}
case 139:
#line 483 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], 0); ;
    break;}
case 141:
#line 488 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 142:
#line 492 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); ;
    break;}
case 143:
#line 494 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], yyvsp[-3]); ;
    break;}
case 144:
#line 500 "lang11d"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 145:
#line 502 "lang11d"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 146:
#line 506 "lang11d"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 147:
#line 508 "lang11d"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 148:
#line 511 "lang11d"
{ yyval = 0; ;
    break;}
case 151:
#line 517 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 152:
#line 520 "lang11d"
{ yyval = rwPrivate; ;
    break;}
case 153:
#line 522 "lang11d"
{ yyval = rwReadOnly; ;
    break;}
case 154:
#line 524 "lang11d"
{ yyval = rwReadWrite; ;
    break;}
case 155:
#line 526 "lang11d"
{ yyval = rwWriteOnly; ;
    break;}
case 156:
#line 529 "lang11d"
{ yyval = zzval; ;
    break;}
case 157:
#line 531 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.ui = -node->slot.ui;
				yyval = zzval;
			;
    break;}
case 158:
#line 539 "lang11d"
{ yyval = zzval; ;
    break;}
case 159:
#line 541 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -node->slot.uf;
				yyval = zzval;
			;
    break;}
case 160:
#line 549 "lang11d"
{ yyval = zzval; ;
    break;}
case 162:
#line 554 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)yyvsp[-1];
				node->slot.uf *= pi;
			;
    break;}
case 163:
#line 560 "lang11d"
{
				PyrSlotNode *node; 
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = node->slot.ui;
				node->slot.uf = ival * pi;
			;
    break;}
case 164:
#line 568 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = pi;
				yyval = zzval;
			;
    break;}
case 165:
#line 575 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -pi;
				yyval = zzval;
			;
    break;}
case 166:
#line 583 "lang11d"
{ yyval = zzval; ;
    break;}
case 167:
#line 586 "lang11d"
{ yyval = zzval; ;
    break;}
case 168:
#line 589 "lang11d"
{ yyval = zzval; ;
    break;}
case 169:
#line 592 "lang11d"
{ yyval = zzval; ;
    break;}
case 170:
#line 595 "lang11d"
{ yyval = zzval; ;
    break;}
case 171:
#line 598 "lang11d"
{ yyval = zzval; ;
    break;}
case 172:
#line 601 "lang11d"
{ yyval = zzval; ;
    break;}
case 173:
#line 604 "lang11d"
{ yyval = zzval; ;
    break;}
case 174:
#line 607 "lang11d"
{ yyval = zzval; ;
    break;}
case 176:
#line 612 "lang11d"
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
case 177:
#line 637 "lang11d"
{ yyval = zzval; ;
    break;}
case 178:
#line 640 "lang11d"
{ yyval = zzval; ;
    break;}
case 179:
#line 643 "lang11d"
{ yyval = zzval; ;
    break;}
case 180:
#line 644 "lang11d"
{ yyval = zzval; ;
    break;}
case 181:
#line 645 "lang11d"
{ yyval = zzval; ;
    break;}
case 182:
#line 646 "lang11d"
{ yyval = zzval; ;
    break;}
case 183:
#line 647 "lang11d"
{ yyval = zzval; ;
    break;}
case 184:
#line 648 "lang11d"
{ yyval = zzval; ;
    break;}
case 185:
#line 649 "lang11d"
{ yyval = zzval; ;
    break;}
case 186:
#line 650 "lang11d"
{ yyval = zzval; ;
    break;}
case 187:
#line 653 "lang11d"
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
#line 656 "lang11d"
