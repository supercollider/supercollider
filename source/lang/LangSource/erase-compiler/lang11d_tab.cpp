
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



#define	YYFINAL		327
#define	YYFLAG		-32768
#define	YYNTBASE	49

#define YYTRANSLATE(x) ((unsigned)(x) <= 281 ? yytranslate[x] : 120)

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
   177,   186,   191,   193,   195,   197,   199,   203,   206,   210,
   212,   217,   223,   228,   233,   235,   241,   243,   246,   250,
   254,   258,   263,   269,   277,   282,   289,   297,   299,   303,
   306,   307,   310,   312,   316,   318,   322,   325,   326,   329,
   331,   335,   337,   341,   343,   345,   347,   349,   351,   353,
   355,   357,   359,   361,   363,   365,   367,   369,   371,   373,
   375,   377,   379,   381,   383,   385,   387,   389,   391,   393,
   395,   397,   399,   401,   403,   405,   411,   418,   419,   422,
   424,   427,   431,   432,   436,   442,   446,   452,   453,   455,
   457,   461,   463,   467,   469,   473,   476,   481,   486,   492,
   496,   501,   502,   505,   507,   511,   512,   514,   516,   518,
   520,   523,   525,   528,   530,   532,   535,   538,   540,   543,
   545,   547,   549,   551,   553,   555,   557,   559,   561,   563,
   566,   568,   570,   572,   574,   576,   578,   580,   582,   584,
   586
};

static const short yyrhs[] = {    50,
     0,    51,     0,    23,    63,     0,     0,    50,    52,     0,
    53,     0,    51,    53,     0,   107,    55,    37,    56,    58,
    38,     0,   107,    39,    54,    40,    55,    37,    56,    58,
    38,     0,    31,   107,    37,    58,    38,     0,     0,   106,
     0,     0,    41,   107,     0,     0,    56,    57,     0,    13,
    95,    42,     0,    11,    95,    42,     0,     0,    58,    59,
     0,   106,    37,    91,    88,    65,    64,    38,     0,    30,
   106,    37,    91,    88,    65,    64,    38,     0,   118,    37,
    91,    88,    65,    64,    38,     0,    30,   118,    37,    91,
    88,    65,    64,    38,     0,     0,    42,     0,     0,    43,
     0,    67,     0,    74,    67,     0,    44,    89,    62,    45,
     0,    89,    62,     0,    62,     0,    66,     0,    74,    66,
     0,     0,   108,    60,     0,     0,    46,    72,    60,     0,
     0,    46,    72,    60,     0,    83,     0,    68,    83,     0,
     0,    68,     0,   106,    68,     0,   106,    44,    76,    79,
    45,    69,     0,    72,    34,    44,    77,    45,    69,     0,
    72,    34,   106,    44,    77,    45,    69,     0,    72,    34,
    44,    76,    79,    45,    69,     0,    72,    34,   106,    44,
    76,    79,    45,    69,     0,    72,    34,   106,    69,     0,
    85,     0,    83,     0,    84,     0,    70,     0,    44,    74,
    45,     0,    47,   106,     0,    39,    75,    40,     0,   117,
     0,   107,    44,    77,    45,     0,   107,    44,    76,    79,
    45,     0,   107,    39,    75,    40,     0,    71,    39,    76,
    40,     0,    71,     0,    72,    34,    39,    76,    40,     0,
   107,     0,    35,    72,     0,    72,   118,    72,     0,    72,
   119,    72,     0,   106,    24,    72,     0,    47,   106,    24,
    72,     0,    72,    34,   106,    24,    72,     0,   106,    44,
    76,    79,    45,    24,    72,     0,    48,    80,    24,    72,
     0,    71,    39,    76,    40,    24,    72,     0,    72,    34,
    39,    76,    40,    24,    72,     0,    72,     0,    73,    42,
    72,     0,    73,    60,     0,     0,    76,    61,     0,    74,
     0,    76,    43,    74,     0,    78,     0,    77,    43,    78,
     0,   119,    74,     0,     0,    43,    77,     0,    81,     0,
    81,    20,   106,     0,   106,     0,    81,    43,   106,     0,
   102,     0,   105,     0,   113,     0,   115,     0,   114,     0,
   109,     0,   110,     0,   111,     0,   112,     0,    97,     0,
    87,     0,   106,     0,   102,     0,   105,     0,   113,     0,
   115,     0,   114,     0,   109,     0,   110,     0,   111,     0,
   112,     0,    97,     0,   102,     0,   105,     0,   113,     0,
   115,     0,   114,     0,   109,     0,   110,     0,   111,     0,
   112,     0,    98,     0,    37,    91,    88,    62,    38,     0,
    48,    37,    91,    88,    62,    38,     0,     0,    88,    90,
     0,    90,     0,    89,    90,     0,    11,    93,    42,     0,
     0,    12,    93,    42,     0,    12,    92,    20,   106,    42,
     0,    32,    93,    32,     0,    32,    92,    20,   106,    32,
     0,     0,    93,     0,    94,     0,    93,    43,    94,     0,
   106,     0,   106,    24,    82,     0,    96,     0,    95,    43,
    96,     0,   101,   106,     0,   101,   106,    24,    82,     0,
    48,    39,    99,    40,     0,    48,   107,    39,    99,    40,
     0,    39,    99,    40,     0,   107,    39,    99,    40,     0,
     0,   100,    61,     0,    86,     0,   100,    43,    86,     0,
     0,    28,     0,    33,     0,    29,     0,     4,     0,    27,
     4,     0,     5,     0,    27,     5,     0,    21,     0,   103,
     0,   103,   104,     0,   102,   104,     0,   104,     0,    27,
   104,     0,     3,     0,    10,     0,     9,     0,    16,     0,
    17,     0,    15,     0,    18,     0,     8,     0,     6,     0,
   116,     0,   115,   116,     0,     7,     0,    19,     0,    25,
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
   198,   204,   210,   211,   212,   213,   214,   216,   226,   228,
   230,   241,   254,   256,   271,   272,   285,   287,   301,   306,
   311,   315,   326,   331,   341,   346,   360,   376,   377,   383,
   386,   387,   390,   391,   395,   396,   400,   404,   405,   408,
   410,   414,   415,   420,   421,   422,   423,   424,   425,   426,
   427,   428,   429,   432,   435,   438,   439,   440,   441,   442,
   443,   444,   445,   446,   447,   450,   451,   452,   453,   454,
   455,   456,   457,   458,   459,   462,   465,   470,   471,   475,
   476,   480,   484,   485,   489,   493,   497,   503,   504,   507,
   508,   512,   514,   518,   519,   523,   525,   531,   533,   537,
   539,   543,   544,   547,   548,   552,   553,   555,   557,   561,
   562,   571,   572,   581,   584,   585,   591,   599,   606,   615,
   618,   621,   624,   627,   630,   633,   636,   639,   642,   643,
   669,   672,   675,   676,   677,   678,   679,   680,   681,   682,
   685
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
"primitive","retval","funretval","blocklist1","blocklist","msgsend","expr1",
"expr","exprn","exprseq","arglistc","arglist1","keyarglist1","keyarg","optkeyarglist",
"mavars","mavarlist","slotliteral","blockliteral","pushname","pushliteral","listliteral",
"block","funcvardecls","funcvardecls1","funcvardecl","argdecls","slotdeflist0",
"slotdeflist","slotdef","rwslotdeflist","rwslotdef","listlit","listlit2","literallistc",
"literallist1","rwspec","integer","floatr","pie","floatp","name","classname",
"primname","trueobj","falseobj","nilobj","infobj","ascii","symbol","string",
"string1","pseudovar","binop","keybinop", NULL
};
#endif

static const short yyr1[] = {     0,
    49,    49,    49,    50,    50,    51,    51,    52,    52,    53,
    54,    54,    55,    55,    56,    56,    57,    57,    58,    58,
    59,    59,    59,    59,    60,    60,    61,    61,    62,    62,
    63,    63,    63,    64,    64,    65,    65,    66,    66,    67,
    67,    68,    68,    69,    69,    70,    70,    70,    70,    70,
    70,    70,    71,    71,    71,    71,    71,    71,    71,    71,
    71,    71,    71,    71,    72,    72,    72,    72,    72,    72,
    72,    72,    72,    72,    72,    72,    72,    73,    73,    74,
    75,    75,    76,    76,    77,    77,    78,    79,    79,    80,
    80,    81,    81,    82,    82,    82,    82,    82,    82,    82,
    82,    82,    82,    83,    84,    85,    85,    85,    85,    85,
    85,    85,    85,    85,    85,    86,    86,    86,    86,    86,
    86,    86,    86,    86,    86,    87,    87,    88,    88,    89,
    89,    90,    91,    91,    91,    91,    91,    92,    92,    93,
    93,    94,    94,    95,    95,    96,    96,    97,    97,    98,
    98,    99,    99,   100,   100,   101,   101,   101,   101,   102,
   102,   103,   103,   104,   105,   105,   105,   105,   105,   106,
   107,   108,   109,   110,   111,   112,   113,   114,   115,   115,
   116,   117,   118,   118,   118,   118,   118,   118,   118,   118,
   119
};

static const short yyr2[] = {     0,
     1,     1,     2,     0,     2,     1,     2,     6,     9,     5,
     0,     1,     0,     2,     0,     2,     3,     3,     0,     2,
     7,     8,     7,     8,     0,     1,     0,     1,     1,     2,
     4,     2,     1,     1,     2,     0,     2,     0,     3,     0,
     3,     1,     2,     0,     1,     2,     6,     6,     7,     7,
     8,     4,     1,     1,     1,     1,     3,     2,     3,     1,
     4,     5,     4,     4,     1,     5,     1,     2,     3,     3,
     3,     4,     5,     7,     4,     6,     7,     1,     3,     2,
     0,     2,     1,     3,     1,     3,     2,     0,     2,     1,
     3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     5,     6,     0,     2,     1,
     2,     3,     0,     3,     5,     3,     5,     0,     1,     1,
     3,     1,     3,     1,     3,     2,     4,     4,     5,     3,
     4,     0,     2,     1,     3,     0,     1,     1,     1,     1,
     2,     1,     2,     1,     1,     2,     2,     1,     2,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1
};

static const short yydefact[] = {     4,
    40,     0,     1,     2,     6,   170,   160,   162,   178,   181,
   177,   171,     0,   175,   173,   174,   176,   182,   164,     0,
     0,   133,    81,     0,     0,     0,     0,    33,     3,    29,
    56,    65,    78,    25,    40,    54,    55,    53,   104,    40,
   130,   115,   106,   165,   168,   107,   105,    67,   111,   112,
   113,   114,   108,   110,   109,   179,    60,     0,     5,    13,
     7,     0,   140,   142,   161,   163,   169,     0,    68,   138,
   138,   128,    83,     0,    27,     0,    40,    25,    58,   133,
   152,     0,    90,    92,     0,     0,   183,   191,   187,   185,
   186,   188,   189,   190,   184,     0,     0,     0,    26,    80,
    30,    32,   131,   167,   166,     0,     0,     0,    46,    42,
    81,     0,   180,    19,    11,     0,     0,   132,     0,     0,
     0,   139,     0,   139,    40,    59,    28,    82,    57,     0,
    26,    41,     0,   128,   152,   154,   125,     0,    27,   116,
   117,     0,   121,   122,   123,   124,   118,   120,   119,     0,
     0,     0,   152,     0,     0,     0,    44,    69,    70,    79,
    71,    88,    43,     0,    88,     0,    85,     0,     0,     0,
    12,    14,    15,   141,     0,   143,   103,    94,    95,    99,
   100,   101,   102,    96,    98,    97,     0,   134,     0,   136,
     0,   129,    84,    31,    72,    40,     0,   148,    28,   153,
   152,    75,    91,    93,     0,    64,     0,     0,    88,     0,
     0,     0,    45,    52,     0,     0,    63,     0,     0,    61,
    87,   188,    10,    20,     0,     0,    13,    19,     0,     0,
   126,     0,   150,   155,     0,   149,     0,    66,     0,    44,
    73,    88,     0,    89,    44,    62,    86,     0,     0,   133,
   133,     0,   156,   156,    16,     0,   135,   137,   127,   151,
    76,     0,    44,    48,     0,    44,     0,    47,   133,   133,
   128,   128,    15,   157,   159,   158,     0,   144,     0,     0,
     8,    77,    50,    44,    49,    74,   128,   128,    36,    36,
    19,    18,   156,   146,    17,    51,    36,    36,   172,    38,
    25,    38,     0,   145,     0,    38,    38,     0,     0,    34,
    38,    37,     0,     9,   147,     0,     0,    25,    21,    35,
    23,    22,    24,    39,     0,     0,     0
};

static const short yydefgoto[] = {   325,
     3,     4,    59,     5,   170,   117,   228,   255,   169,   224,
   100,   128,    28,    29,   309,   300,   310,    30,   213,   214,
    31,    32,    33,    34,    73,    74,    75,   166,   167,   216,
    82,    83,   176,    36,    37,    38,   136,    39,   125,    40,
   192,    72,   121,    62,    63,   277,   278,    42,   137,   138,
   139,   279,    43,    44,    45,    46,    47,    48,   301,    49,
    50,    51,    52,    53,    54,    55,    56,    57,    97,    98
};

static const short yypact[] = {   137,
   277,    12,    12,   -15,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    63,-32768,-32768,-32768,-32768,-32768,-32768,   169,
   508,     9,   508,   370,   508,    63,    16,-32768,-32768,-32768,
-32768,    52,   643,    62,   109,-32768,-32768,-32768,-32768,   324,
-32768,-32768,   144,   144,-32768,-32768,   -17,    -7,-32768,-32768,
-32768,-32768,-32768,-32768,   165,-32768,-32768,   148,-32768,     3,
-32768,    73,-32768,   181,-32768,-32768,-32768,   508,-32768,    63,
    63,-32768,-32768,   171,   178,   167,   324,   625,   207,     9,
   557,   214,   101,-32768,   211,   508,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   102,   508,   508,   508,-32768,
-32768,-32768,-32768,-32768,-32768,   508,   508,   223,    14,-32768,
   508,   416,-32768,-32768,    63,    12,   228,-32768,    63,   533,
   246,    91,   248,   119,   324,-32768,   508,-32768,-32768,   224,
-32768,-32768,   508,-32768,   557,-32768,-32768,   232,   231,   144,
-32768,   236,-32768,-32768,-32768,-32768,-32768,-32768,   165,   508,
    63,    63,   557,   183,   508,   416,   156,   242,   242,   643,
   643,   234,-32768,   238,   234,   187,-32768,   508,   573,   239,
-32768,-32768,-32768,-32768,     1,-32768,-32768,   144,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   165,    63,-32768,    63,-32768,
   252,-32768,-32768,-32768,   643,   324,   257,-32768,   557,-32768,
   557,   643,-32768,-32768,   260,   262,   508,   206,   234,   194,
   508,   416,    14,-32768,   416,   258,-32768,   263,   276,-32768,
-32768,   616,-32768,-32768,   270,   272,   269,   243,   271,   279,
-32768,   280,-32768,-32768,   275,-32768,   508,   293,   274,    14,
   643,   234,   212,   283,     6,-32768,-32768,   285,   299,     9,
     9,   300,   180,   180,-32768,   585,-32768,-32768,-32768,-32768,
   643,   508,    14,-32768,   301,    14,   508,-32768,     9,     9,
-32768,-32768,-32768,-32768,-32768,-32768,   136,-32768,    63,   221,
-32768,   643,-32768,    14,-32768,   643,-32768,-32768,   250,   250,
   243,-32768,   180,   296,-32768,-32768,   250,   250,-32768,   462,
   302,   462,   604,-32768,   533,   462,   462,   508,   309,-32768,
   292,-32768,   310,-32768,-32768,   311,   312,   625,-32768,-32768,
-32768,-32768,-32768,-32768,   352,   353,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,   350,-32768,   129,    82,-32768,  -195,-32768,
   -73,   218,    20,-32768,  -120,  -178,    47,   325,   315,   -69,
-32768,-32768,   -19,-32768,    -1,   253,   -62,  -104,   146,  -140,
-32768,-32768,    61,   -44,-32768,-32768,   168,-32768,   -36,   345,
    46,   -67,   308,   128,   264,   130,    89,  -116,-32768,   -10,
-32768,-32768,   -72,-32768,    -8,   -71,   -12,    72,-32768,   -64,
   -63,   -52,   -47,   -43,   -35,    28,   -27,-32768,  -161,     5
};


#define	YYLAST		677


static const short yytable[] = {    35,
    64,    69,   110,   177,   132,    78,   106,   226,   140,   141,
    12,    67,   134,    79,    84,     2,   143,   144,     6,    22,
    70,    12,    76,   154,   218,    12,   107,   113,   145,   267,
   108,   111,   256,   146,   104,   105,   112,   147,    35,    81,
    71,   115,    22,   116,   162,   148,    41,   178,   179,   165,
    22,   210,    80,   108,    81,   180,   181,    64,    64,   102,
   249,   108,   140,   141,   163,     6,    76,   182,   239,    41,
   143,   144,   183,    58,    60,    35,   184,   158,   159,   160,
   140,   141,   145,   157,   185,   103,   161,   146,   143,   144,
    86,   147,   208,   209,   226,   303,   130,   196,    85,   148,
   145,   265,   171,    99,     6,   146,    64,   243,   149,   147,
   244,   302,   110,   195,   118,   119,   168,   148,   306,   307,
   151,   113,   103,    35,   197,   193,   140,   141,   140,   141,
   202,   104,   188,   119,   143,   144,   143,   144,   203,   204,
   155,   226,   205,   152,   191,   156,   145,   186,   145,   242,
   190,   146,   142,   146,    25,   147,   225,   147,   113,     1,
   168,   119,   149,   148,    19,   148,   221,     2,   163,   104,
   264,    10,    65,    66,   229,   268,   230,   292,   293,   211,
   149,   313,   271,   272,   114,   316,   317,   172,   177,    19,
   235,   241,    22,   283,    35,   110,   285,   122,   124,   212,
   110,   287,   288,   108,   120,   193,   142,   274,   275,   248,
   126,   129,   276,   193,   296,   232,   168,   261,   110,   168,
   127,   110,   206,   168,   142,   207,   149,   312,   149,   219,
   133,   220,   178,   179,   289,   290,   219,   150,   240,   110,
   180,   181,   282,   225,   324,   238,    85,   286,   207,   153,
   297,   298,   182,   253,   219,   254,   266,   183,   299,    80,
    13,   184,   295,   293,   173,   187,   294,   189,   194,   185,
   142,   198,   142,   199,   201,    96,   215,   217,   227,     6,
     7,     8,     9,    10,    11,   237,    12,    13,   318,   231,
   225,    14,    15,    16,    17,    18,   233,    19,   311,   236,
   311,    88,   245,    20,   311,   311,   250,   246,   251,   116,
   258,    21,   257,    22,   260,    23,   262,   259,   263,   305,
    24,   269,    25,    26,    27,   219,     6,     7,     8,     9,
    10,    11,   186,    12,    13,   270,   273,   308,    14,    15,
    16,    17,    18,   131,    19,   284,   319,   321,   322,   323,
    20,   326,   327,    61,   291,   252,   200,   320,    21,   101,
    22,   109,    23,   164,   247,   315,   234,    68,    77,    25,
    26,    27,     6,     7,     8,     9,    10,    11,   123,    12,
    13,   304,   174,   280,    14,    15,    16,    17,    18,     0,
    19,     0,     0,     0,     0,     0,    20,     0,     0,     0,
     0,     0,     0,     0,    21,     0,    22,     0,    23,     0,
     0,     0,     0,    68,     0,     0,    26,    27,     6,     7,
     8,     9,    10,    11,     0,    12,     0,     0,     0,     0,
    14,    15,    16,    17,    18,     0,    19,     0,     0,     0,
     0,    88,    20,     0,     0,     0,     0,     0,     0,     0,
    21,     0,    22,     0,    23,     0,     0,     0,     0,    68,
     0,     0,    26,    27,     6,     7,     8,     9,    10,    11,
     0,    12,     0,     0,     0,     0,    14,    15,    16,    17,
    18,     0,    19,     0,     0,     0,     0,     0,    20,     0,
     0,     0,     0,     0,     0,     0,    21,     0,    22,     0,
    23,     0,     0,     0,     0,    68,     0,   308,    26,    27,
     6,     7,     8,     9,    10,    11,     0,    12,     0,     0,
     0,     0,    14,    15,    16,    17,    18,     0,    19,     0,
     0,     0,     0,     0,    20,     0,     7,     8,     9,    10,
    11,     0,    21,     0,    22,     0,    23,    14,    15,    16,
    17,    68,     0,    19,    26,    27,     0,     0,     0,    20,
     7,     8,     9,    10,    11,     0,    12,     0,     0,     0,
     0,    14,    15,    16,    17,     6,     0,    19,     0,     0,
   175,     0,     0,    20,     0,     0,     0,     6,     0,     0,
     0,     0,     0,     0,     0,   135,     0,    87,     0,    89,
    90,    91,   222,    93,    94,    95,     6,     0,     0,    87,
   223,    89,    90,    91,   222,    93,    94,    95,     6,     0,
     0,     0,   281,     0,     0,     0,     0,     0,    87,     0,
    89,    90,    91,   222,    93,    94,    95,     0,     0,     0,
    87,   314,    89,    90,    91,    92,    93,    94,    95,    87,
    88,    89,    90,    91,    92,    93,    94,    95,    96,     0,
     0,     0,     0,     0,     0,     0,   131,    87,    88,    89,
    90,    91,    92,    93,    94,    95,    96
};

static const short yycheck[] = {     1,
    13,    21,    47,   120,    78,    25,    24,   169,    81,    81,
    10,    20,    80,    26,    27,    31,    81,    81,     3,    37,
    12,    10,    24,    86,   165,    10,    44,    55,    81,    24,
    48,    39,   228,    81,    43,    44,    44,    81,    40,    39,
    32,    39,    37,    41,   107,    81,     1,   120,   120,   112,
    37,   156,    37,    48,    39,   120,   120,    70,    71,    40,
   222,    48,   135,   135,   109,     3,    68,   120,   209,    24,
   135,   135,   120,     2,     3,    77,   120,    97,    98,    99,
   153,   153,   135,    96,   120,    40,   106,   135,   153,   153,
    39,   135,   155,   156,   256,   291,    77,   134,    27,   135,
   153,   242,   115,    42,     3,   153,   119,   212,    81,   153,
   215,   290,   157,   133,    42,    43,   112,   153,   297,   298,
    20,   149,    77,   125,   135,   127,   199,   199,   201,   201,
   150,   140,    42,    43,   199,   199,   201,   201,   151,   152,
    39,   303,   153,    43,   125,    44,   199,   120,   201,   212,
    32,   199,    81,   201,    46,   199,   169,   201,   186,    23,
   156,    43,   135,   199,    21,   201,   168,    31,   213,   178,
   240,     7,     4,     5,   187,   245,   189,    42,    43,    24,
   153,   302,   250,   251,    37,   306,   307,   116,   305,    21,
   201,   211,    37,   263,   196,   240,   266,    70,    71,    44,
   245,   269,   270,    48,    24,   207,   135,    28,    29,   222,
    40,    45,    33,   215,   284,   196,   212,   237,   263,   215,
    43,   266,    40,   219,   153,    43,   199,   301,   201,    43,
    24,    45,   305,   305,   271,   272,    43,    24,    45,   284,
   305,   305,   262,   256,   318,    40,   175,   267,    43,    39,
   287,   288,   305,    11,    43,    13,    45,   305,     9,    37,
    11,   305,    42,    43,    37,    20,   279,    20,    45,   305,
   199,    40,   201,    43,    39,    34,    43,    40,    40,     3,
     4,     5,     6,     7,     8,    24,    10,    11,   308,    38,
   303,    15,    16,    17,    18,    19,    40,    21,   300,    40,
   302,    26,    45,    27,   306,   307,    37,    45,    37,    41,
    32,    35,    42,    37,    40,    39,    24,    38,    45,    24,
    44,    37,    46,    47,    48,    43,     3,     4,     5,     6,
     7,     8,   305,    10,    11,    37,    37,    46,    15,    16,
    17,    18,    19,    42,    21,    45,    38,    38,    38,    38,
    27,     0,     0,     4,   273,   227,   139,   311,    35,    35,
    37,    47,    39,   111,   219,   305,   199,    44,    24,    46,
    47,    48,     3,     4,     5,     6,     7,     8,    71,    10,
    11,   293,   119,   254,    15,    16,    17,    18,    19,    -1,
    21,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    35,    -1,    37,    -1,    39,    -1,
    -1,    -1,    -1,    44,    -1,    -1,    47,    48,     3,     4,
     5,     6,     7,     8,    -1,    10,    -1,    -1,    -1,    -1,
    15,    16,    17,    18,    19,    -1,    21,    -1,    -1,    -1,
    -1,    26,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    35,    -1,    37,    -1,    39,    -1,    -1,    -1,    -1,    44,
    -1,    -1,    47,    48,     3,     4,     5,     6,     7,     8,
    -1,    10,    -1,    -1,    -1,    -1,    15,    16,    17,    18,
    19,    -1,    21,    -1,    -1,    -1,    -1,    -1,    27,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    35,    -1,    37,    -1,
    39,    -1,    -1,    -1,    -1,    44,    -1,    46,    47,    48,
     3,     4,     5,     6,     7,     8,    -1,    10,    -1,    -1,
    -1,    -1,    15,    16,    17,    18,    19,    -1,    21,    -1,
    -1,    -1,    -1,    -1,    27,    -1,     4,     5,     6,     7,
     8,    -1,    35,    -1,    37,    -1,    39,    15,    16,    17,
    18,    44,    -1,    21,    47,    48,    -1,    -1,    -1,    27,
     4,     5,     6,     7,     8,    -1,    10,    -1,    -1,    -1,
    -1,    15,    16,    17,    18,     3,    -1,    21,    -1,    -1,
    48,    -1,    -1,    27,    -1,    -1,    -1,     3,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    39,    -1,    25,    -1,    27,
    28,    29,    30,    31,    32,    33,     3,    -1,    -1,    25,
    38,    27,    28,    29,    30,    31,    32,    33,     3,    -1,
    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    25,    -1,
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
					(PyrParseNode*)yyvsp[-6], 
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
case 57:
#line 215 "lang11d"
{ yyval = yyvsp[-1]; ;
    break;}
case 58:
#line 217 "lang11d"
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
case 59:
#line 227 "lang11d"
{ yyval = (long)newPyrDynListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 60:
#line 229 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 61:
#line 231 "lang11d"
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
case 62:
#line 242 "lang11d"
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
case 63:
#line 255 "lang11d"
{ yyval = (long)newPyrDynListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 64:
#line 257 "lang11d"
{ 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-3], 
					(PyrParseNode*)yyvsp[-1]);
				yyval = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;
    break;}
case 66:
#line 273 "lang11d"
{ 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_at);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)yyvsp[-4], 
					(PyrParseNode*)yyvsp[-1]);
				yyval = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;
    break;}
case 67:
#line 286 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 68:
#line 288 "lang11d"
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
case 69:
#line 302 "lang11d"
{ 
				yyval = (long)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-1], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 70:
#line 307 "lang11d"
{ 
				yyval = (long)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-1], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 71:
#line 312 "lang11d"
{ 
				yyval = (long)newPyrAssignNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); 
			;
    break;}
case 72:
#line 316 "lang11d"
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
case 73:
#line 327 "lang11d"
{ 
				yyval = (long)newPyrSetterNode((PyrSlotNode*)yyvsp[-2], 
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 74:
#line 332 "lang11d"
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
case 75:
#line 342 "lang11d"
{ 
				yyval = (long)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[0], 0); 
			;
    break;}
case 76:
#line 347 "lang11d"
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
				yyval = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;
    break;}
case 77:
#line 361 "lang11d"
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
				yyval = (long)newPyrCallNode(selectornode, args, 0, 0); 
			;
    break;}
case 79:
#line 378 "lang11d"
{ 
				yyval = (long)newPyrDropNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 81:
#line 386 "lang11d"
{ yyval = 0; ;
    break;}
case 84:
#line 392 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 86:
#line 397 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 87:
#line 401 "lang11d"
{ yyval = (long)newPyrPushKeyArgNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 88:
#line 404 "lang11d"
{ yyval = 0; ;
    break;}
case 89:
#line 405 "lang11d"
{ yyval = yyvsp[0]; ;
    break;}
case 90:
#line 409 "lang11d"
{ yyval = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 91:
#line 411 "lang11d"
{ yyval = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); ;
    break;}
case 93:
#line 416 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 94:
#line 420 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 95:
#line 421 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 96:
#line 422 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 97:
#line 423 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 98:
#line 424 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 99:
#line 425 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 100:
#line 426 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 101:
#line 427 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 102:
#line 428 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 103:
#line 429 "lang11d"
{ yyval = (long)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 104:
#line 432 "lang11d"
{ yyval = (long)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 105:
#line 435 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 106:
#line 438 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 107:
#line 439 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 108:
#line 440 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 109:
#line 441 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 110:
#line 442 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 111:
#line 443 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 112:
#line 444 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 113:
#line 445 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 114:
#line 446 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 115:
#line 447 "lang11d"
{ yyval = (long)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 116:
#line 450 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 117:
#line 451 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 118:
#line 452 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 119:
#line 453 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 120:
#line 454 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 121:
#line 455 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 122:
#line 456 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 123:
#line 457 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 124:
#line 458 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 125:
#line 459 "lang11d"
{ yyval = (long)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 126:
#line 463 "lang11d"
{ yyval = (long)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[-1], false); ;
    break;}
case 127:
#line 466 "lang11d"
{ yyval = (long)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[-1], true); ;
    break;}
case 128:
#line 470 "lang11d"
{ yyval = 0; ;
    break;}
case 129:
#line 472 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 131:
#line 477 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 132:
#line 481 "lang11d"
{ yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); ;
    break;}
case 133:
#line 484 "lang11d"
{ yyval = 0; ;
    break;}
case 134:
#line 486 "lang11d"
{
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); 
				;
    break;}
case 135:
#line 490 "lang11d"
{ 
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]); 
				;
    break;}
case 136:
#line 494 "lang11d"
{
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); 
				;
    break;}
case 137:
#line 498 "lang11d"
{ 
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]); 
				;
    break;}
case 138:
#line 503 "lang11d"
{ yyval = 0; ;
    break;}
case 141:
#line 509 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 142:
#line 513 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); ;
    break;}
case 143:
#line 515 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], 0); ;
    break;}
case 145:
#line 520 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 146:
#line 524 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); ;
    break;}
case 147:
#line 526 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], yyvsp[-3]); ;
    break;}
case 148:
#line 532 "lang11d"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 149:
#line 534 "lang11d"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 150:
#line 538 "lang11d"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 151:
#line 540 "lang11d"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 152:
#line 543 "lang11d"
{ yyval = 0; ;
    break;}
case 155:
#line 549 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 156:
#line 552 "lang11d"
{ yyval = rwPrivate; ;
    break;}
case 157:
#line 554 "lang11d"
{ yyval = rwReadOnly; ;
    break;}
case 158:
#line 556 "lang11d"
{ yyval = rwReadWrite; ;
    break;}
case 159:
#line 558 "lang11d"
{ yyval = rwWriteOnly; ;
    break;}
case 160:
#line 561 "lang11d"
{ yyval = zzval; ;
    break;}
case 161:
#line 563 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.ui = -node->slot.ui;
				yyval = zzval;
			;
    break;}
case 162:
#line 571 "lang11d"
{ yyval = zzval; ;
    break;}
case 163:
#line 573 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -node->slot.uf;
				yyval = zzval;
			;
    break;}
case 164:
#line 581 "lang11d"
{ yyval = zzval; ;
    break;}
case 166:
#line 586 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)yyvsp[-1];
				node->slot.uf *= pi;
			;
    break;}
case 167:
#line 592 "lang11d"
{
				PyrSlotNode *node; 
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = node->slot.ui;
				node->slot.uf = ival * pi;
			;
    break;}
case 168:
#line 600 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = pi;
				yyval = zzval;
			;
    break;}
case 169:
#line 607 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -pi;
				yyval = zzval;
			;
    break;}
case 170:
#line 615 "lang11d"
{ yyval = zzval; ;
    break;}
case 171:
#line 618 "lang11d"
{ yyval = zzval; ;
    break;}
case 172:
#line 621 "lang11d"
{ yyval = zzval; ;
    break;}
case 173:
#line 624 "lang11d"
{ yyval = zzval; ;
    break;}
case 174:
#line 627 "lang11d"
{ yyval = zzval; ;
    break;}
case 175:
#line 630 "lang11d"
{ yyval = zzval; ;
    break;}
case 176:
#line 633 "lang11d"
{ yyval = zzval; ;
    break;}
case 177:
#line 636 "lang11d"
{ yyval = zzval; ;
    break;}
case 178:
#line 639 "lang11d"
{ yyval = zzval; ;
    break;}
case 180:
#line 644 "lang11d"
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
case 181:
#line 669 "lang11d"
{ yyval = zzval; ;
    break;}
case 182:
#line 672 "lang11d"
{ yyval = zzval; ;
    break;}
case 183:
#line 675 "lang11d"
{ yyval = zzval; ;
    break;}
case 184:
#line 676 "lang11d"
{ yyval = zzval; ;
    break;}
case 185:
#line 677 "lang11d"
{ yyval = zzval; ;
    break;}
case 186:
#line 678 "lang11d"
{ yyval = zzval; ;
    break;}
case 187:
#line 679 "lang11d"
{ yyval = zzval; ;
    break;}
case 188:
#line 680 "lang11d"
{ yyval = zzval; ;
    break;}
case 189:
#line 681 "lang11d"
{ yyval = zzval; ;
    break;}
case 190:
#line 682 "lang11d"
{ yyval = zzval; ;
    break;}
case 191:
#line 685 "lang11d"
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
#line 688 "lang11d"
