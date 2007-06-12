
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



#define	YYFINAL		293
#define	YYFLAG		-32768
#define	YYNTBASE	48

#define YYTRANSLATE(x) ((unsigned)(x) <= 281 ? yytranslate[x] : 113)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,    47,     2,     2,     2,     2,    43,
    44,    30,    31,    42,    27,    33,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    40,    41,    28,
    24,    29,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    38,     2,    39,    45,     2,    34,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    36,     2,    37,    46,     2,     2,     2,     2,
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
    17,    18,    19,    20,    21,    22,    23,    25,    26,    32,
    35
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     7,     8,    11,    13,    16,    23,    33,
    39,    40,    42,    43,    46,    47,    50,    54,    58,    59,
    62,    70,    79,    87,    96,    97,    99,   100,   102,   104,
   107,   112,   115,   117,   119,   122,   123,   126,   127,   131,
   132,   136,   138,   140,   143,   145,   147,   152,   158,   162,
   165,   169,   173,   179,   185,   192,   199,   207,   211,   215,
   220,   224,   229,   235,   243,   248,   250,   254,   257,   258,
   261,   263,   267,   269,   273,   276,   277,   280,   282,   286,
   288,   292,   294,   296,   298,   300,   302,   304,   306,   308,
   310,   312,   314,   316,   318,   320,   322,   324,   326,   328,
   330,   332,   334,   336,   338,   340,   342,   344,   346,   348,
   350,   352,   354,   360,   367,   368,   371,   373,   376,   380,
   381,   385,   391,   392,   394,   396,   400,   402,   406,   408,
   412,   415,   420,   425,   431,   435,   440,   441,   444,   446,
   450,   451,   453,   455,   457,   459,   462,   464,   467,   469,
   471,   474,   477,   479,   482,   484,   486,   488,   490,   492,
   494,   496,   498,   500,   502,   505,   507,   509,   511,   513,
   515,   517,   519,   521,   523
};

static const short yyrhs[] = {    49,
     0,    50,     0,    23,    62,     0,     0,    49,    51,     0,
    52,     0,    50,    52,     0,   100,    54,    36,    55,    57,
    37,     0,   100,    38,    53,    39,    54,    36,    55,    57,
    37,     0,    31,   100,    36,    57,    37,     0,     0,    99,
     0,     0,    40,   100,     0,     0,    55,    56,     0,    13,
    88,    41,     0,    11,    88,    41,     0,     0,    57,    58,
     0,    99,    36,    84,    81,    64,    63,    37,     0,    30,
    99,    36,    84,    81,    64,    63,    37,     0,   111,    36,
    84,    81,    64,    63,    37,     0,    30,   111,    36,    84,
    81,    64,    63,    37,     0,     0,    41,     0,     0,    42,
     0,    66,     0,    69,    66,     0,    43,    82,    61,    44,
     0,    82,    61,     0,    61,     0,    65,     0,    69,    65,
     0,     0,   101,    59,     0,     0,    45,    67,    59,     0,
     0,    45,    67,    59,     0,    78,     0,    99,     0,    46,
    99,     0,   110,     0,   100,     0,   100,    43,    72,    44,
     0,   100,    43,    71,    74,    44,     0,    43,    69,    44,
     0,    34,    67,     0,    67,   111,    67,     0,    67,   112,
    67,     0,    99,    43,    71,    74,    44,     0,    67,    33,
    43,    72,    44,     0,    67,    33,    99,    43,    72,    44,
     0,    67,    33,    43,    71,    74,    44,     0,    67,    33,
    99,    43,    71,    74,    44,     0,    67,    33,    99,     0,
    38,    70,    39,     0,   100,    38,    70,    39,     0,    99,
    24,    67,     0,    46,    99,    24,    67,     0,    67,    33,
    99,    24,    67,     0,    99,    43,    71,    74,    44,    24,
    67,     0,    47,    75,    24,    67,     0,    67,     0,    68,
    41,    67,     0,    68,    59,     0,     0,    71,    60,     0,
    69,     0,    71,    42,    69,     0,    73,     0,    72,    42,
    73,     0,   112,    69,     0,     0,    42,    72,     0,    76,
     0,    76,    20,    99,     0,    99,     0,    76,    42,    99,
     0,    95,     0,    98,     0,   106,     0,   108,     0,   107,
     0,   102,     0,   103,     0,   104,     0,   105,     0,    90,
     0,    95,     0,    98,     0,   106,     0,   108,     0,   107,
     0,   102,     0,   103,     0,   104,     0,   105,     0,    80,
     0,    90,     0,    95,     0,    98,     0,   106,     0,   108,
     0,   107,     0,   102,     0,   103,     0,   104,     0,   105,
     0,    91,     0,    36,    84,    81,    61,    37,     0,    47,
    36,    84,    81,    61,    37,     0,     0,    81,    83,     0,
    83,     0,    82,    83,     0,    11,    86,    41,     0,     0,
    12,    86,    41,     0,    12,    85,    20,    99,    41,     0,
     0,    86,     0,    87,     0,    86,    42,    87,     0,    99,
     0,    99,    24,    77,     0,    89,     0,    88,    42,    89,
     0,    94,    99,     0,    94,    99,    24,    77,     0,    47,
    38,    92,    39,     0,    47,   100,    38,    92,    39,     0,
    38,    92,    39,     0,   100,    38,    92,    39,     0,     0,
    93,    60,     0,    79,     0,    93,    42,    79,     0,     0,
    28,     0,    32,     0,    29,     0,     4,     0,    27,     4,
     0,     5,     0,    27,     5,     0,    21,     0,    96,     0,
    96,    97,     0,    95,    97,     0,    97,     0,    27,    97,
     0,     3,     0,    10,     0,     9,     0,    16,     0,    17,
     0,    15,     0,    18,     0,     8,     0,     6,     0,   109,
     0,   108,   109,     0,     7,     0,    19,     0,    25,     0,
    32,     0,    28,     0,    29,     0,    27,     0,    30,     0,
    31,     0,    26,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    34,    36,    38,    42,    43,    47,    48,    52,    56,    63,
    69,    70,    73,    74,    78,    79,    83,    85,    89,    90,
    94,    97,   100,   103,   108,   109,   112,   113,   116,   117,
   121,   123,   125,   129,   130,   134,   135,   139,   141,   145,
   147,   151,   152,   154,   164,   166,   168,   179,   192,   194,
   208,   213,   218,   223,   232,   237,   250,   256,   260,   262,
   264,   268,   279,   284,   294,   301,   302,   308,   311,   312,
   315,   316,   320,   321,   325,   329,   330,   333,   335,   339,
   340,   345,   346,   347,   348,   349,   350,   351,   352,   353,
   354,   357,   358,   359,   360,   361,   362,   363,   364,   365,
   366,   367,   370,   371,   372,   373,   374,   375,   376,   377,
   378,   379,   382,   385,   390,   391,   395,   396,   400,   404,
   405,   409,   415,   416,   419,   420,   424,   426,   430,   431,
   435,   437,   443,   445,   449,   451,   455,   456,   459,   460,
   464,   465,   467,   469,   473,   474,   483,   484,   493,   496,
   497,   503,   511,   518,   527,   530,   533,   536,   539,   542,
   545,   548,   551,   554,   555,   581,   584,   587,   588,   589,
   590,   591,   592,   593,   596
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","NAME","INTEGER",
"FLOAT","SYMBOL","STRING","ASCII","PRIMITIVENAME","CLASSNAME","VAR","ARG","CLASSVAR",
"CONST","NILOBJ","TRUEOBJ","FALSEOBJ","INFINITUMOBJ","PSEUDOVAR","ELLIPSIS",
"PIE","BADTOKEN","INTERPRET","'='","BINOP","KEYBINOP","'-'","'<'","'>'","'*'",
"'+'","READWRITEVAR","'.'","'`'","UMINUS","'{'","'}'","'['","']'","':'","';'",
"','","'('","')'","'^'","'~'","'#'","root","classes","classextensions","classdef",
"classextension","optname","superclass","classvardecls","classvardecl","methods",
"methoddef","optsemi","optcomma","funcbody","cmdlinecode","methbody","primitive",
"retval","funretval","expr","exprn","exprseq","arglistc","arglist1","keyarglist1",
"keyarg","optkeyarglist","mavars","mavarlist","slotliteral","pushliteral","listliteral",
"block","funcvardecls","funcvardecls1","funcvardecl","argdecls","slotdeflist0",
"slotdeflist","slotdef","rwslotdeflist","rwslotdef","listlit","listlit2","literallistc",
"literallist1","rwspec","integer","floatr","pie","floatp","name","classname",
"primname","trueobj","falseobj","nilobj","infobj","ascii","symbol","string",
"string1","pseudovar","binop","keybinop", NULL
};
#endif

static const short yyr1[] = {     0,
    48,    48,    48,    49,    49,    50,    50,    51,    51,    52,
    53,    53,    54,    54,    55,    55,    56,    56,    57,    57,
    58,    58,    58,    58,    59,    59,    60,    60,    61,    61,
    62,    62,    62,    63,    63,    64,    64,    65,    65,    66,
    66,    67,    67,    67,    67,    67,    67,    67,    67,    67,
    67,    67,    67,    67,    67,    67,    67,    67,    67,    67,
    67,    67,    67,    67,    67,    68,    68,    69,    70,    70,
    71,    71,    72,    72,    73,    74,    74,    75,    75,    76,
    76,    77,    77,    77,    77,    77,    77,    77,    77,    77,
    77,    78,    78,    78,    78,    78,    78,    78,    78,    78,
    78,    78,    79,    79,    79,    79,    79,    79,    79,    79,
    79,    79,    80,    80,    81,    81,    82,    82,    83,    84,
    84,    84,    85,    85,    86,    86,    87,    87,    88,    88,
    89,    89,    90,    90,    91,    91,    92,    92,    93,    93,
    94,    94,    94,    94,    95,    95,    96,    96,    97,    98,
    98,    98,    98,    98,    99,   100,   101,   102,   103,   104,
   105,   106,   107,   108,   108,   109,   110,   111,   111,   111,
   111,   111,   111,   111,   112
};

static const short yyr2[] = {     0,
     1,     1,     2,     0,     2,     1,     2,     6,     9,     5,
     0,     1,     0,     2,     0,     2,     3,     3,     0,     2,
     7,     8,     7,     8,     0,     1,     0,     1,     1,     2,
     4,     2,     1,     1,     2,     0,     2,     0,     3,     0,
     3,     1,     1,     2,     1,     1,     4,     5,     3,     2,
     3,     3,     5,     5,     6,     6,     7,     3,     3,     4,
     3,     4,     5,     7,     4,     1,     3,     2,     0,     2,
     1,     3,     1,     3,     2,     0,     2,     1,     3,     1,
     3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     5,     6,     0,     2,     1,     2,     3,     0,
     3,     5,     0,     1,     1,     3,     1,     3,     1,     3,
     2,     4,     4,     5,     3,     4,     0,     2,     1,     3,
     0,     1,     1,     1,     1,     2,     1,     2,     1,     1,
     2,     2,     1,     2,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1
};

static const short yydefact[] = {     4,
    40,     0,     1,     2,     6,   155,   145,   147,   163,   166,
   162,   156,     0,   160,   158,   159,   161,   167,   149,     0,
     0,   120,    69,     0,     0,     0,     0,    33,     3,    29,
    66,    25,    40,    42,   101,    40,   117,   102,    92,   150,
   153,    93,    43,    46,    97,    98,    99,   100,    94,    96,
    95,   164,    45,     0,     5,    13,     7,     0,   125,   127,
   146,   148,   154,     0,    50,   123,   115,    71,     0,    27,
     0,    40,    25,    44,   120,   137,     0,    78,    80,     0,
   168,   175,   172,   170,   171,   173,   174,   169,     0,     0,
     0,    26,    68,    30,    32,   118,   152,   151,     0,     0,
    69,     0,   165,    19,    11,     0,     0,   119,     0,     0,
     0,   124,    40,    59,    28,    70,    49,     0,    26,    41,
     0,   115,   137,   139,   112,     0,    27,   103,   104,     0,
   108,   109,   110,   111,   105,   107,   106,     0,     0,     0,
   137,     0,    58,    51,    52,    67,    61,    76,     0,    76,
     0,    73,     0,     0,     0,    12,    14,    15,   126,     0,
   128,    91,    82,    83,    87,    88,    89,    90,    84,    86,
    85,     0,   121,     0,   116,    72,    31,    62,    40,     0,
   133,    28,   138,   137,    65,    79,    81,     0,    76,     0,
     0,     0,     0,     0,    60,     0,     0,    47,    75,   173,
    10,    20,     0,     0,    13,    19,     0,   113,     0,   135,
   140,     0,   134,     0,    54,    63,    76,     0,    77,    53,
    48,    74,     0,     0,   120,   120,     0,   141,   141,    16,
     0,   122,   114,   136,    56,     0,    55,     0,   120,   120,
   115,   115,    15,   142,   144,   143,     0,   129,     0,     0,
     8,    57,    64,   115,   115,    36,    36,    19,    18,   141,
   131,    17,    36,    36,   157,    38,    25,    38,     0,   130,
     0,    38,    38,     0,     0,    34,    38,    37,     0,     9,
   132,     0,     0,    25,    21,    35,    23,    22,    24,    39,
     0,     0,     0
};

static const short yydefgoto[] = {   291,
     3,     4,    55,     5,   155,   107,   206,   230,   154,   202,
    93,   116,    28,    29,   275,   266,   276,    30,    31,    32,
    68,    69,    70,   151,   152,   194,    77,    78,   161,    34,
   124,    35,   113,    36,   175,    67,   111,    58,    59,   247,
   248,    38,   125,   126,   127,   249,    39,    40,    41,    42,
    43,    44,   267,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    90,    91
};

static const short yypact[] = {    39,
   272,    16,    16,    48,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    33,-32768,-32768,-32768,-32768,-32768,-32768,    35,
   497,    38,   497,   362,   497,    33,    96,-32768,-32768,-32768,
   616,    74,    41,-32768,-32768,   317,-32768,-32768,    71,    71,
-32768,-32768,     3,    51,-32768,-32768,-32768,-32768,-32768,-32768,
   123,-32768,-32768,   115,-32768,    -8,-32768,    63,-32768,   147,
-32768,-32768,-32768,   497,-32768,    33,-32768,-32768,   152,   145,
   151,   317,   599,   173,    38,   545,   175,     5,-32768,   162,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,     9,   497,
   497,   497,-32768,-32768,-32768,-32768,-32768,-32768,   497,   497,
   497,   407,-32768,-32768,    33,    16,   165,-32768,    33,   521,
   182,   117,   317,-32768,   497,-32768,-32768,   159,-32768,-32768,
   497,-32768,   545,-32768,-32768,   172,   180,    71,-32768,   166,
-32768,-32768,-32768,-32768,-32768,-32768,   123,   497,    33,    33,
   545,   407,    10,   181,   181,   616,   616,   183,   176,   183,
    67,-32768,   497,   218,   184,-32768,-32768,-32768,-32768,    -5,
-32768,-32768,    71,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   123,    33,-32768,   187,-32768,-32768,-32768,   616,   317,   189,
-32768,   545,-32768,   545,   616,-32768,-32768,   191,   183,    80,
   497,   407,   407,   188,-32768,   190,   205,-32768,-32768,   591,
-32768,-32768,   197,   202,   204,    56,   201,-32768,   214,-32768,
-32768,   213,-32768,   209,-32768,   616,   183,   120,   212,   234,
-32768,-32768,   223,   224,    38,    38,   226,   118,   118,-32768,
   561,-32768,-32768,-32768,-32768,   219,-32768,   497,    38,    38,
-32768,-32768,-32768,-32768,-32768,-32768,   140,-32768,    33,   148,
-32768,-32768,   616,-32768,-32768,   168,   168,    56,-32768,   118,
   240,-32768,   168,   168,-32768,   452,   225,   452,   572,-32768,
   521,   452,   452,   497,   231,-32768,   228,-32768,   232,-32768,
-32768,   233,   237,   599,-32768,-32768,-32768,-32768,-32768,-32768,
   281,   284,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,   282,-32768,    87,    42,-32768,  -190,-32768,
   -71,   167,   -23,-32768,  -185,  -235,    18,   265,   -18,-32768,
    -1,   199,   -94,     1,   104,  -109,-32768,-32768,    31,-32768,
   121,-32768,  -101,   280,    19,   -56,-32768,   241,   200,    82,
    52,  -106,-32768,   -99,-32768,-32768,   -66,-32768,    -2,   -65,
   -12,    57,-32768,   -59,   -45,   -10,    25,    26,    34,    45,
   -42,-32768,   -93,    43
};


#define	YYLAST		649


static const short yytable[] = {    33,
    60,   120,    65,   162,    12,   148,    73,   150,   103,   128,
   129,     6,    95,    74,    79,   231,   131,    63,   122,    37,
   179,   268,    71,   180,   139,    12,    99,   272,   273,   105,
   132,   106,    76,   191,    33,     6,    97,    98,    61,    62,
   196,   188,    37,   163,   164,   100,   140,   189,   118,    66,
   165,   142,   192,    60,    96,    19,   128,   129,    54,    56,
   204,     1,    71,   131,   166,   133,   228,   269,   229,     2,
    33,   144,   145,   146,   128,   129,   143,   132,     2,   214,
   147,   131,   279,    80,   212,    25,   282,   283,   101,   174,
    96,    19,   156,   102,   103,   132,    60,   217,     6,   167,
   134,   135,   178,   108,   109,    12,   224,   236,   197,   136,
   198,    33,   133,   176,    92,   128,   129,   128,   129,   185,
   137,   197,   131,   215,   131,    97,   186,   187,   103,    10,
   133,    75,   130,    76,   168,   169,   132,   204,   132,   256,
   257,   203,   190,   170,   153,   244,   245,   134,   135,   246,
   104,   199,   263,   264,   171,   209,   136,   173,   109,   207,
    97,   197,   157,   237,   162,   134,   135,   137,   241,   242,
   110,   133,   216,   133,   136,   204,   265,    33,    13,   130,
   259,   260,   254,   255,   153,   137,   115,   223,   262,   260,
   114,   176,   218,   219,   117,   278,   121,   130,   138,   141,
   158,   172,   177,   184,   163,   164,   134,   135,   134,   135,
   181,   165,   290,    89,   195,   136,    80,   136,   203,   253,
     6,   182,   205,   208,   193,   166,   137,   210,   137,   213,
    82,   220,   225,   221,   153,   153,   261,   226,   130,   153,
   130,   232,    81,   106,    83,    84,    85,   200,    87,    88,
   233,   234,   235,   197,   201,   284,   203,   238,   239,   240,
   167,   243,   252,   271,   277,   119,   277,   285,   287,   288,
   277,   277,   274,   289,     6,     7,     8,     9,    10,    11,
   292,    12,    13,   293,   258,    57,    14,    15,    16,    17,
    18,   227,    19,   183,   286,   168,   169,    94,    20,   149,
   222,   281,   211,    72,   170,    21,   112,    22,   159,    23,
   250,   270,     0,     0,    24,   171,    25,    26,    27,     6,
     7,     8,     9,    10,    11,     0,    12,    13,     0,     0,
     0,    14,    15,    16,    17,    18,     0,    19,     0,     0,
     0,     0,     0,    20,     0,     0,     0,     0,     0,     0,
    21,     0,    22,     0,    23,     0,     0,     0,     0,    64,
     0,    25,    26,    27,     6,     7,     8,     9,    10,    11,
     0,    12,    13,     0,     0,     0,    14,    15,    16,    17,
    18,     0,    19,     0,     0,     0,     0,     0,    20,     0,
     0,     0,     0,     0,     0,    21,     0,    22,     0,    23,
     0,     0,     0,     0,    64,     0,     0,    26,    27,     6,
     7,     8,     9,    10,    11,     0,    12,     0,     0,     0,
     0,    14,    15,    16,    17,    18,     0,    19,     0,     0,
     0,     0,    82,    20,     0,     0,     0,     0,     0,     0,
    21,     0,    22,     0,    23,     0,     0,     0,     0,    64,
     0,     0,    26,    27,     6,     7,     8,     9,    10,    11,
     0,    12,     0,     0,     0,     0,    14,    15,    16,    17,
    18,     0,    19,     0,     0,     0,     0,     0,    20,     0,
     0,     0,     0,     0,     0,    21,     0,    22,     0,    23,
     0,     0,     0,     0,    64,     0,   274,    26,    27,     6,
     7,     8,     9,    10,    11,     0,    12,     0,     0,     0,
     0,    14,    15,    16,    17,    18,     0,    19,     0,     0,
     0,     0,     0,    20,     7,     8,     9,    10,    11,     0,
    21,     0,    22,     0,    23,    14,    15,    16,    17,    64,
     0,    19,    26,    27,     0,     0,     0,    20,     7,     8,
     9,    10,    11,     0,    12,     0,     0,     0,     0,    14,
    15,    16,    17,     6,     0,    19,     0,   160,     0,     0,
     0,    20,     0,     0,     6,     0,     0,     0,     0,     0,
     0,     0,   123,     0,     0,    81,     0,    83,    84,    85,
   200,    87,    88,     6,     0,     0,    81,   251,    83,    84,
    85,   200,    87,    88,     0,     0,     0,     0,   280,     0,
     0,     0,     0,     0,     0,    81,     0,    83,    84,    85,
    86,    87,    88,    81,    82,    83,    84,    85,    86,    87,
    88,    89,     0,     0,     0,     0,     0,     0,     0,   119,
    81,    82,    83,    84,    85,    86,    87,    88,    89
};

static const short yycheck[] = {     1,
    13,    73,    21,   110,    10,   100,    25,   102,    51,    76,
    76,     3,    36,    26,    27,   206,    76,    20,    75,     1,
   122,   257,    24,   123,    20,    10,    24,   263,   264,    38,
    76,    40,    38,    24,    36,     3,    39,    40,     4,     5,
   150,   141,    24,   110,   110,    43,    42,   142,    72,    12,
   110,    43,    43,    66,    36,    21,   123,   123,     2,     3,
   154,    23,    64,   123,   110,    76,    11,   258,    13,    31,
    72,    90,    91,    92,   141,   141,    89,   123,    31,   189,
    99,   141,   268,    27,   184,    45,   272,   273,    38,   113,
    72,    21,   105,    43,   137,   141,   109,   192,     3,   110,
    76,    76,   121,    41,    42,    10,   200,   217,    42,    76,
    44,   113,   123,   115,    41,   182,   182,   184,   184,   138,
    76,    42,   182,    44,   184,   128,   139,   140,   171,     7,
   141,    36,    76,    38,   110,   110,   182,   231,   184,   241,
   242,   154,   142,   110,   102,    28,    29,   123,   123,    32,
    36,   153,   254,   255,   110,   179,   123,    41,    42,   172,
   163,    42,   106,    44,   271,   141,   141,   123,   225,   226,
    24,   182,   191,   184,   141,   269,     9,   179,    11,   123,
    41,    42,   239,   240,   142,   141,    42,   200,    41,    42,
    39,   193,   192,   193,    44,   267,    24,   141,    24,    38,
    36,    20,    44,    38,   271,   271,   182,   182,   184,   184,
    39,   271,   284,    33,    39,   182,   160,   184,   231,   238,
     3,    42,    39,    37,    42,   271,   182,    39,   184,    39,
    26,    44,    36,    44,   192,   193,   249,    36,   182,   197,
   184,    41,    25,    40,    27,    28,    29,    30,    31,    32,
    37,    39,    44,    42,    37,   274,   269,    24,    36,    36,
   271,    36,    44,    24,   266,    41,   268,    37,    37,    37,
   272,   273,    45,    37,     3,     4,     5,     6,     7,     8,
     0,    10,    11,     0,   243,     4,    15,    16,    17,    18,
    19,   205,    21,   127,   277,   271,   271,    33,    27,   101,
   197,   271,   182,    24,   271,    34,    66,    36,   109,    38,
   229,   260,    -1,    -1,    43,   271,    45,    46,    47,     3,
     4,     5,     6,     7,     8,    -1,    10,    11,    -1,    -1,
    -1,    15,    16,    17,    18,    19,    -1,    21,    -1,    -1,
    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,
    34,    -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    43,
    -1,    45,    46,    47,     3,     4,     5,     6,     7,     8,
    -1,    10,    11,    -1,    -1,    -1,    15,    16,    17,    18,
    19,    -1,    21,    -1,    -1,    -1,    -1,    -1,    27,    -1,
    -1,    -1,    -1,    -1,    -1,    34,    -1,    36,    -1,    38,
    -1,    -1,    -1,    -1,    43,    -1,    -1,    46,    47,     3,
     4,     5,     6,     7,     8,    -1,    10,    -1,    -1,    -1,
    -1,    15,    16,    17,    18,    19,    -1,    21,    -1,    -1,
    -1,    -1,    26,    27,    -1,    -1,    -1,    -1,    -1,    -1,
    34,    -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    43,
    -1,    -1,    46,    47,     3,     4,     5,     6,     7,     8,
    -1,    10,    -1,    -1,    -1,    -1,    15,    16,    17,    18,
    19,    -1,    21,    -1,    -1,    -1,    -1,    -1,    27,    -1,
    -1,    -1,    -1,    -1,    -1,    34,    -1,    36,    -1,    38,
    -1,    -1,    -1,    -1,    43,    -1,    45,    46,    47,     3,
     4,     5,     6,     7,     8,    -1,    10,    -1,    -1,    -1,
    -1,    15,    16,    17,    18,    19,    -1,    21,    -1,    -1,
    -1,    -1,    -1,    27,     4,     5,     6,     7,     8,    -1,
    34,    -1,    36,    -1,    38,    15,    16,    17,    18,    43,
    -1,    21,    46,    47,    -1,    -1,    -1,    27,     4,     5,
     6,     7,     8,    -1,    10,    -1,    -1,    -1,    -1,    15,
    16,    17,    18,     3,    -1,    21,    -1,    47,    -1,    -1,
    -1,    27,    -1,    -1,     3,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    38,    -1,    -1,    25,    -1,    27,    28,    29,
    30,    31,    32,     3,    -1,    -1,    25,    37,    27,    28,
    29,    30,    31,    32,    -1,    -1,    -1,    -1,    37,    -1,
    -1,    -1,    -1,    -1,    -1,    25,    -1,    27,    28,    29,
    30,    31,    32,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,
    25,    26,    27,    28,    29,    30,    31,    32,    33
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
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 44:
#line 155 "lang11d"
{ 
				PyrParseNode* argnode;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL);
				SetSymbol(&slot, s_envirGet);
				selectornode = newPyrSlotNode(&slot);
				yyval = (long)newPyrCallNode(selectornode, argnode, 0); 
			;
    break;}
case 45:
#line 165 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 46:
#line 167 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 47:
#line 169 "lang11d"
{ 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = (PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)yyvsp[-3]);
				yyval = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)yyvsp[-1]); 
			;
    break;}
case 48:
#line 180 "lang11d"
{ 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_new);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)yyvsp[-4]), 
					(PyrParseNode*)yyvsp[-2]);
				yyval = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)yyvsp[-1]); 
			;
    break;}
case 49:
#line 193 "lang11d"
{ yyval = yyvsp[-1]; ;
    break;}
case 50:
#line 195 "lang11d"
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
				yyval = (long)newPyrCallNode(slotnode, args, 0); 
			;
    break;}
case 51:
#line 209 "lang11d"
{ 
				yyval = (long)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-1], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 52:
#line 214 "lang11d"
{ 
				yyval = (long)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-1], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 53:
#line 219 "lang11d"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-4], (PyrParseNode*)yyvsp[-2], 
						(PyrParseNode*)yyvsp[-1]); 
			;
    break;}
case 54:
#line 224 "lang11d"
{ 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				
				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				yyval = (long)newPyrCallNode(selectornode, (PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 55:
#line 233 "lang11d"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-5], 
					(PyrParseNode*)yyvsp[-1]); 
			;
    break;}
case 56:
#line 238 "lang11d"
{ 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				PyrParseNode* args;
				
				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				args = linkNextNode(
					(PyrParseNode*)newPyrPushNameNode((PyrSlotNode*)yyvsp[-5]), 
					(PyrParseNode*)yyvsp[-2]);
				yyval = (long)newPyrCallNode(selectornode, args, (PyrParseNode*)yyvsp[-1]); 
			;
    break;}
case 57:
#line 251 "lang11d"
{ 
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-2]);
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-4], args, (PyrParseNode*)yyvsp[-1]); 
			;
    break;}
case 58:
#line 257 "lang11d"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[0], (PyrParseNode*)yyvsp[-2], 0); 
			;
    break;}
case 59:
#line 261 "lang11d"
{ yyval = (long)newPyrDynListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 60:
#line 263 "lang11d"
{ yyval = (long)newPyrDynListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 61:
#line 265 "lang11d"
{ 
				yyval = (long)newPyrAssignNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); 
			;
    break;}
case 62:
#line 269 "lang11d"
{ 
				PyrParseNode *argnode, *args;
				PyrSlotNode* selectornode;
				PyrSlot slot;
				argnode = (PyrParseNode*)newPyrPushLitNode((PyrSlotNode*)yyvsp[-2], NULL);
				args = linkNextNode(argnode, (PyrParseNode*)yyvsp[0]);
				SetSymbol(&slot, s_envirPut);
				selectornode = newPyrSlotNode(&slot);
				yyval = (long)newPyrCallNode(selectornode, args, 0); 
			;
    break;}
case 63:
#line 280 "lang11d"
{ 
				yyval = (long)newPyrSetterNode((PyrSlotNode*)yyvsp[-2], 
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 64:
#line 285 "lang11d"
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
case 65:
#line 295 "lang11d"
{ 
				yyval = (long)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[0], 0); 
			;
    break;}
case 67:
#line 303 "lang11d"
{ 
				yyval = (long)newPyrDropNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 69:
#line 311 "lang11d"
{ yyval = 0; ;
    break;}
case 72:
#line 317 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 74:
#line 322 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 75:
#line 326 "lang11d"
{ yyval = (long)newPyrPushKeyArgNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 76:
#line 329 "lang11d"
{ yyval = 0; ;
    break;}
case 77:
#line 330 "lang11d"
{ yyval = yyvsp[0]; ;
    break;}
case 78:
#line 334 "lang11d"
{ yyval = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 79:
#line 336 "lang11d"
{ yyval = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); ;
    break;}
case 81:
#line 341 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 82:
#line 345 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 83:
#line 346 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 84:
#line 347 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 85:
#line 348 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 86:
#line 349 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 87:
#line 350 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 88:
#line 351 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 89:
#line 352 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 90:
#line 353 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 91:
#line 354 "lang11d"
{ yyval = (long)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 92:
#line 357 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 93:
#line 358 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 94:
#line 359 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 95:
#line 360 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 96:
#line 361 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 97:
#line 362 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 98:
#line 363 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 99:
#line 364 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 100:
#line 365 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 101:
#line 366 "lang11d"
{ yyval = (long)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 102:
#line 367 "lang11d"
{ yyval = (long)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 103:
#line 370 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 104:
#line 371 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 105:
#line 372 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 106:
#line 373 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 107:
#line 374 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 108:
#line 375 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 109:
#line 376 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 110:
#line 377 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 111:
#line 378 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 112:
#line 379 "lang11d"
{ yyval = (long)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 113:
#line 383 "lang11d"
{ yyval = (long)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[-1], false); ;
    break;}
case 114:
#line 386 "lang11d"
{ yyval = (long)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[-1], true); ;
    break;}
case 115:
#line 390 "lang11d"
{ yyval = 0; ;
    break;}
case 116:
#line 392 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 118:
#line 397 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 119:
#line 401 "lang11d"
{ yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); ;
    break;}
case 120:
#line 404 "lang11d"
{ yyval = 0; ;
    break;}
case 121:
#line 406 "lang11d"
{
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); 
				;
    break;}
case 122:
#line 410 "lang11d"
{ 
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]); 
				;
    break;}
case 123:
#line 415 "lang11d"
{ yyval = 0; ;
    break;}
case 126:
#line 421 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 127:
#line 425 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); ;
    break;}
case 128:
#line 427 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], 0); ;
    break;}
case 130:
#line 432 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 131:
#line 436 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); ;
    break;}
case 132:
#line 438 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], yyvsp[-3]); ;
    break;}
case 133:
#line 444 "lang11d"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 134:
#line 446 "lang11d"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 135:
#line 450 "lang11d"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 136:
#line 452 "lang11d"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 137:
#line 455 "lang11d"
{ yyval = 0; ;
    break;}
case 140:
#line 461 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 141:
#line 464 "lang11d"
{ yyval = rwPrivate; ;
    break;}
case 142:
#line 466 "lang11d"
{ yyval = rwReadOnly; ;
    break;}
case 143:
#line 468 "lang11d"
{ yyval = rwReadWrite; ;
    break;}
case 144:
#line 470 "lang11d"
{ yyval = rwWriteOnly; ;
    break;}
case 145:
#line 473 "lang11d"
{ yyval = zzval; ;
    break;}
case 146:
#line 475 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.ui = -node->slot.ui;
				yyval = zzval;
			;
    break;}
case 147:
#line 483 "lang11d"
{ yyval = zzval; ;
    break;}
case 148:
#line 485 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -node->slot.uf;
				yyval = zzval;
			;
    break;}
case 149:
#line 493 "lang11d"
{ yyval = zzval; ;
    break;}
case 151:
#line 498 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)yyvsp[-1];
				node->slot.uf *= pi;
			;
    break;}
case 152:
#line 504 "lang11d"
{
				PyrSlotNode *node; 
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = node->slot.ui;
				node->slot.uf = ival * pi;
			;
    break;}
case 153:
#line 512 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = pi;
				yyval = zzval;
			;
    break;}
case 154:
#line 519 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -pi;
				yyval = zzval;
			;
    break;}
case 155:
#line 527 "lang11d"
{ yyval = zzval; ;
    break;}
case 156:
#line 530 "lang11d"
{ yyval = zzval; ;
    break;}
case 157:
#line 533 "lang11d"
{ yyval = zzval; ;
    break;}
case 158:
#line 536 "lang11d"
{ yyval = zzval; ;
    break;}
case 159:
#line 539 "lang11d"
{ yyval = zzval; ;
    break;}
case 160:
#line 542 "lang11d"
{ yyval = zzval; ;
    break;}
case 161:
#line 545 "lang11d"
{ yyval = zzval; ;
    break;}
case 162:
#line 548 "lang11d"
{ yyval = zzval; ;
    break;}
case 163:
#line 551 "lang11d"
{ yyval = zzval; ;
    break;}
case 165:
#line 556 "lang11d"
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
case 166:
#line 581 "lang11d"
{ yyval = zzval; ;
    break;}
case 167:
#line 584 "lang11d"
{ yyval = zzval; ;
    break;}
case 168:
#line 587 "lang11d"
{ yyval = zzval; ;
    break;}
case 169:
#line 588 "lang11d"
{ yyval = zzval; ;
    break;}
case 170:
#line 589 "lang11d"
{ yyval = zzval; ;
    break;}
case 171:
#line 590 "lang11d"
{ yyval = zzval; ;
    break;}
case 172:
#line 591 "lang11d"
{ yyval = zzval; ;
    break;}
case 173:
#line 592 "lang11d"
{ yyval = zzval; ;
    break;}
case 174:
#line 593 "lang11d"
{ yyval = zzval; ;
    break;}
case 175:
#line 596 "lang11d"
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
#line 599 "lang11d"
