
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



#define	YYFINAL		301
#define	YYFLAG		-32768
#define	YYNTBASE	49

#define YYTRANSLATE(x) ((unsigned)(x) <= 281 ? yytranslate[x] : 114)

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
   132,   136,   138,   140,   143,   145,   147,   152,   158,   162,
   165,   169,   173,   179,   185,   192,   199,   207,   211,   215,
   220,   224,   229,   235,   243,   248,   250,   254,   257,   258,
   261,   263,   267,   269,   273,   276,   277,   280,   282,   286,
   288,   292,   294,   296,   298,   300,   302,   304,   306,   308,
   310,   312,   314,   316,   318,   320,   322,   324,   326,   328,
   330,   332,   334,   336,   338,   340,   342,   344,   346,   348,
   350,   352,   354,   360,   367,   368,   371,   373,   376,   380,
   381,   385,   391,   395,   401,   402,   404,   406,   410,   412,
   416,   418,   422,   425,   430,   435,   441,   445,   450,   451,
   454,   456,   460,   461,   463,   465,   467,   469,   472,   474,
   477,   479,   481,   484,   487,   489,   492,   494,   496,   498,
   500,   502,   504,   506,   508,   510,   512,   515,   517,   519,
   521,   523,   525,   527,   529,   531,   533,   535
};

static const short yyrhs[] = {    50,
     0,    51,     0,    23,    63,     0,     0,    50,    52,     0,
    53,     0,    51,    53,     0,   101,    55,    37,    56,    58,
    38,     0,   101,    39,    54,    40,    55,    37,    56,    58,
    38,     0,    31,   101,    37,    58,    38,     0,     0,   100,
     0,     0,    41,   101,     0,     0,    56,    57,     0,    13,
    89,    42,     0,    11,    89,    42,     0,     0,    58,    59,
     0,   100,    37,    85,    82,    65,    64,    38,     0,    30,
   100,    37,    85,    82,    65,    64,    38,     0,   112,    37,
    85,    82,    65,    64,    38,     0,    30,   112,    37,    85,
    82,    65,    64,    38,     0,     0,    42,     0,     0,    43,
     0,    67,     0,    70,    67,     0,    44,    83,    62,    45,
     0,    83,    62,     0,    62,     0,    66,     0,    70,    66,
     0,     0,   102,    60,     0,     0,    46,    68,    60,     0,
     0,    46,    68,    60,     0,    79,     0,   100,     0,    47,
   100,     0,   111,     0,   101,     0,   101,    44,    73,    45,
     0,   101,    44,    72,    75,    45,     0,    44,    70,    45,
     0,    35,    68,     0,    68,   112,    68,     0,    68,   113,
    68,     0,   100,    44,    72,    75,    45,     0,    68,    34,
    44,    73,    45,     0,    68,    34,   100,    44,    73,    45,
     0,    68,    34,    44,    72,    75,    45,     0,    68,    34,
   100,    44,    72,    75,    45,     0,    68,    34,   100,     0,
    39,    71,    40,     0,   101,    39,    71,    40,     0,   100,
    24,    68,     0,    47,   100,    24,    68,     0,    68,    34,
   100,    24,    68,     0,   100,    44,    72,    75,    45,    24,
    68,     0,    48,    76,    24,    68,     0,    68,     0,    69,
    42,    68,     0,    69,    60,     0,     0,    72,    61,     0,
    70,     0,    72,    43,    70,     0,    74,     0,    73,    43,
    74,     0,   113,    70,     0,     0,    43,    73,     0,    77,
     0,    77,    20,   100,     0,   100,     0,    77,    43,   100,
     0,    96,     0,    99,     0,   107,     0,   109,     0,   108,
     0,   103,     0,   104,     0,   105,     0,   106,     0,    91,
     0,    96,     0,    99,     0,   107,     0,   109,     0,   108,
     0,   103,     0,   104,     0,   105,     0,   106,     0,    81,
     0,    91,     0,    96,     0,    99,     0,   107,     0,   109,
     0,   108,     0,   103,     0,   104,     0,   105,     0,   106,
     0,    92,     0,    37,    85,    82,    62,    38,     0,    48,
    37,    85,    82,    62,    38,     0,     0,    82,    84,     0,
    84,     0,    83,    84,     0,    11,    87,    42,     0,     0,
    12,    87,    42,     0,    12,    86,    20,   100,    42,     0,
    32,    87,    32,     0,    32,    86,    20,   100,    32,     0,
     0,    87,     0,    88,     0,    87,    43,    88,     0,   100,
     0,   100,    24,    78,     0,    90,     0,    89,    43,    90,
     0,    95,   100,     0,    95,   100,    24,    78,     0,    48,
    39,    93,    40,     0,    48,   101,    39,    93,    40,     0,
    39,    93,    40,     0,   101,    39,    93,    40,     0,     0,
    94,    61,     0,    80,     0,    94,    43,    80,     0,     0,
    28,     0,    33,     0,    29,     0,     4,     0,    27,     4,
     0,     5,     0,    27,     5,     0,    21,     0,    97,     0,
    97,    98,     0,    96,    98,     0,    98,     0,    27,    98,
     0,     3,     0,    10,     0,     9,     0,    16,     0,    17,
     0,    15,     0,    18,     0,     8,     0,     6,     0,   110,
     0,   109,   110,     0,     7,     0,    19,     0,    25,     0,
    33,     0,    28,     0,    29,     0,    27,     0,    30,     0,
    31,     0,    32,     0,    26,     0
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
   405,   409,   413,   417,   423,   424,   427,   428,   432,   434,
   438,   439,   443,   445,   451,   453,   457,   459,   463,   464,
   467,   468,   472,   473,   475,   477,   481,   482,   491,   492,
   501,   504,   505,   511,   519,   526,   535,   538,   541,   544,
   547,   550,   553,   556,   559,   562,   563,   589,   592,   595,
   596,   597,   598,   599,   600,   601,   602,   605
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
"primitive","retval","funretval","expr","exprn","exprseq","arglistc","arglist1",
"keyarglist1","keyarg","optkeyarglist","mavars","mavarlist","slotliteral","pushliteral",
"listliteral","block","funcvardecls","funcvardecls1","funcvardecl","argdecls",
"slotdeflist0","slotdeflist","slotdef","rwslotdeflist","rwslotdef","listlit",
"listlit2","literallistc","literallist1","rwspec","integer","floatr","pie","floatp",
"name","classname","primname","trueobj","falseobj","nilobj","infobj","ascii",
"symbol","string","string1","pseudovar","binop","keybinop", NULL
};
#endif

static const short yyr1[] = {     0,
    49,    49,    49,    50,    50,    51,    51,    52,    52,    53,
    54,    54,    55,    55,    56,    56,    57,    57,    58,    58,
    59,    59,    59,    59,    60,    60,    61,    61,    62,    62,
    63,    63,    63,    64,    64,    65,    65,    66,    66,    67,
    67,    68,    68,    68,    68,    68,    68,    68,    68,    68,
    68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
    68,    68,    68,    68,    68,    69,    69,    70,    71,    71,
    72,    72,    73,    73,    74,    75,    75,    76,    76,    77,
    77,    78,    78,    78,    78,    78,    78,    78,    78,    78,
    78,    79,    79,    79,    79,    79,    79,    79,    79,    79,
    79,    79,    80,    80,    80,    80,    80,    80,    80,    80,
    80,    80,    81,    81,    82,    82,    83,    83,    84,    85,
    85,    85,    85,    85,    86,    86,    87,    87,    88,    88,
    89,    89,    90,    90,    91,    91,    92,    92,    93,    93,
    94,    94,    95,    95,    95,    95,    96,    96,    97,    97,
    98,    99,    99,    99,    99,    99,   100,   101,   102,   103,
   104,   105,   106,   107,   108,   109,   109,   110,   111,   112,
   112,   112,   112,   112,   112,   112,   112,   113
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
     3,     5,     3,     5,     0,     1,     1,     3,     1,     3,
     1,     3,     2,     4,     4,     5,     3,     4,     0,     2,
     1,     3,     0,     1,     1,     1,     1,     2,     1,     2,
     1,     1,     2,     2,     1,     2,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1
};

static const short yydefact[] = {     4,
    40,     0,     1,     2,     6,   157,   147,   149,   165,   168,
   164,   158,     0,   162,   160,   161,   163,   169,   151,     0,
     0,   120,    69,     0,     0,     0,     0,    33,     3,    29,
    66,    25,    40,    42,   101,    40,   117,   102,    92,   152,
   155,    93,    43,    46,    97,    98,    99,   100,    94,    96,
    95,   166,    45,     0,     5,    13,     7,     0,   127,   129,
   148,   150,   156,     0,    50,   125,   125,   115,    71,     0,
    27,     0,    40,    25,    44,   120,   139,     0,    78,    80,
     0,   170,   178,   174,   172,   173,   175,   176,   177,   171,
     0,     0,     0,    26,    68,    30,    32,   118,   154,   153,
     0,     0,    69,     0,   167,    19,    11,     0,     0,   119,
     0,     0,     0,   126,     0,   126,    40,    59,    28,    70,
    49,     0,    26,    41,     0,   115,   139,   141,   112,     0,
    27,   103,   104,     0,   108,   109,   110,   111,   105,   107,
   106,     0,     0,     0,   139,     0,    58,    51,    52,    67,
    61,    76,     0,    76,     0,    73,     0,     0,     0,    12,
    14,    15,   128,     0,   130,    91,    82,    83,    87,    88,
    89,    90,    84,    86,    85,     0,   121,     0,   123,     0,
   116,    72,    31,    62,    40,     0,   135,    28,   140,   139,
    65,    79,    81,     0,    76,     0,     0,     0,     0,     0,
    60,     0,     0,    47,    75,   175,    10,    20,     0,     0,
    13,    19,     0,     0,   113,     0,   137,   142,     0,   136,
     0,    54,    63,    76,     0,    77,    53,    48,    74,     0,
     0,   120,   120,     0,   143,   143,    16,     0,   122,   124,
   114,   138,    56,     0,    55,     0,   120,   120,   115,   115,
    15,   144,   146,   145,     0,   131,     0,     0,     8,    57,
    64,   115,   115,    36,    36,    19,    18,   143,   133,    17,
    36,    36,   159,    38,    25,    38,     0,   132,     0,    38,
    38,     0,     0,    34,    38,    37,     0,     9,   134,     0,
     0,    25,    21,    35,    23,    22,    24,    39,     0,     0,
     0
};

static const short yydefgoto[] = {   299,
     3,     4,    55,     5,   159,   109,   212,   237,   158,   208,
    95,   120,    28,    29,   283,   274,   284,    30,    31,    32,
    69,    70,    71,   155,   156,   200,    78,    79,   165,    34,
   128,    35,   117,    36,   181,    68,   113,    58,    59,   255,
   256,    38,   129,   130,   131,   257,    39,    40,    41,    42,
    43,    44,   275,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    92,    93
};

static const short yypact[] = {    -2,
   278,    52,    52,     7,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    16,-32768,-32768,-32768,-32768,-32768,-32768,    28,
   508,    10,   508,   370,   508,    16,   134,-32768,-32768,-32768,
   631,   -11,    23,-32768,-32768,   324,-32768,-32768,    62,    62,
-32768,-32768,     3,     1,-32768,-32768,-32768,-32768,-32768,-32768,
    80,-32768,-32768,    76,-32768,    70,-32768,   148,-32768,    73,
-32768,-32768,-32768,   508,-32768,    16,    16,-32768,-32768,    98,
   102,   110,   324,   613,   139,    10,   557,   143,    -9,-32768,
   119,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
     9,   508,   508,   508,-32768,-32768,-32768,-32768,-32768,-32768,
   508,   508,   508,   416,-32768,-32768,    16,    52,   137,-32768,
    16,   533,   162,   174,   167,    -6,   324,-32768,   508,-32768,
-32768,   152,-32768,-32768,   508,-32768,   557,-32768,-32768,   169,
   159,    62,-32768,   186,-32768,-32768,-32768,-32768,-32768,-32768,
    80,   508,    16,    16,   557,   416,     4,   179,   179,   631,
   631,   185,   191,   185,    81,-32768,   508,   239,   197,-32768,
-32768,-32768,-32768,     0,-32768,-32768,    62,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    80,    16,-32768,    16,-32768,   201,
-32768,-32768,-32768,   631,   324,   200,-32768,   557,-32768,   557,
   631,-32768,-32768,   203,   185,   105,   508,   416,   416,   196,
-32768,   199,   220,-32768,-32768,   604,-32768,-32768,   210,   212,
   209,   170,   213,   219,-32768,   215,-32768,-32768,   214,-32768,
   216,-32768,   631,   185,   160,   217,   232,-32768,-32768,   221,
   225,    10,    10,   237,    60,    60,-32768,   573,-32768,-32768,
-32768,-32768,-32768,   242,-32768,   508,    10,    10,-32768,-32768,
-32768,-32768,-32768,-32768,   181,-32768,    16,   193,-32768,-32768,
   631,-32768,-32768,   211,   211,   170,-32768,    60,   233,-32768,
   211,   211,-32768,   462,   248,   462,   585,-32768,   533,   462,
   462,   508,   253,-32768,   246,-32768,   260,-32768,-32768,   262,
   263,   613,-32768,-32768,-32768,-32768,-32768,-32768,   302,   304,
-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,   303,-32768,    95,    57,-32768,  -188,-32768,
   -71,   178,   -23,-32768,   -81,   -64,    25,   279,   -19,-32768,
    -1,   208,     8,   -13,   111,   -90,-32768,-32768,    37,-32768,
   130,-32768,   -70,   295,   116,   -72,   254,    36,   222,    84,
    55,  -107,-32768,  -120,-32768,-32768,   -69,-32768,    21,   -68,
   -12,    88,-32768,   -61,   -60,   -47,   -41,   -31,   -20,    24,
   -33,-32768,  -138,    31
};


#define	YYLAST		665


static const short yytable[] = {    33,
    60,    65,   124,   126,   166,    74,   186,   132,   133,    12,
   143,     6,    97,    75,    80,   135,   136,   105,     6,   210,
     1,    66,    72,   238,   194,   179,   101,   197,     2,   137,
    94,    61,    62,   144,    33,   138,   111,     2,    77,   103,
    63,    67,   167,   168,   104,   139,   102,   198,    19,   122,
   169,   170,   146,    60,    60,   185,   140,   132,   133,    99,
   100,    12,    72,   202,   171,   135,   136,   231,    25,   219,
   172,    33,   148,   149,   150,   132,   133,   277,   147,   137,
   173,   151,    19,   135,   136,   138,    10,   252,   253,    54,
    56,   174,   254,   180,   160,   139,   112,   137,    60,   210,
   141,   114,   116,   138,   221,   184,   140,   105,   107,   152,
   108,   154,   106,   139,    81,    33,    37,   182,   132,   133,
   132,   133,   191,   203,   140,   204,   135,   136,   135,   136,
   192,   193,   196,   244,   157,   175,     6,   118,   210,    37,
   137,   105,   137,    12,   119,   209,   138,   203,   138,   222,
   141,    98,    99,   195,   121,   205,   139,   145,   139,   249,
   250,   216,   125,   213,   134,   214,   142,   140,   141,   140,
    76,   166,    77,   162,   262,   263,   157,   223,   264,   265,
   235,   176,   236,    33,   225,   226,   178,    99,    98,   110,
   111,   271,   272,   230,   287,   161,   183,   182,   290,   291,
   276,   188,   203,   286,   245,   224,   280,   281,   187,   167,
   168,   141,    91,   141,   134,   177,   111,   169,   170,   273,
   298,    13,   267,   268,   190,   209,   261,   199,   157,   157,
   201,   171,   134,   157,   270,   268,   211,   172,   215,   217,
   227,     6,   220,   228,   269,    83,   232,   173,   233,   108,
   240,    81,   241,   242,   239,   246,   279,   247,   174,   203,
   243,   248,   292,    82,   209,    84,    85,    86,   206,    88,
    89,    90,   285,   251,   285,   134,   207,   134,   285,   285,
     6,     7,     8,     9,    10,    11,   260,    12,    13,   123,
   293,   282,    14,    15,    16,    17,    18,   295,    19,   296,
   297,   300,   175,   301,    20,   234,    57,   266,   189,   294,
   153,    96,    21,   229,    22,   289,    23,   218,    73,   258,
   115,    24,   278,    25,    26,    27,     6,     7,     8,     9,
    10,    11,   163,    12,    13,     0,     0,     0,    14,    15,
    16,    17,    18,     0,    19,     0,     0,     0,     0,     0,
    20,     0,     0,     0,     0,     0,     0,     0,    21,     0,
    22,     0,    23,     0,     0,     0,     0,    64,     0,    25,
    26,    27,     6,     7,     8,     9,    10,    11,     0,    12,
    13,     0,     0,     0,    14,    15,    16,    17,    18,     0,
    19,     0,     0,     0,     0,     0,    20,     0,     0,     0,
     0,     0,     0,     0,    21,     0,    22,     0,    23,     0,
     0,     0,     0,    64,     0,     0,    26,    27,     6,     7,
     8,     9,    10,    11,     0,    12,     0,     0,     0,     0,
    14,    15,    16,    17,    18,     0,    19,     0,     0,     0,
     0,    83,    20,     0,     0,     0,     0,     0,     0,     0,
    21,     0,    22,     0,    23,     0,     0,     0,     0,    64,
     0,     0,    26,    27,     6,     7,     8,     9,    10,    11,
     0,    12,     0,     0,     0,     0,    14,    15,    16,    17,
    18,     0,    19,     0,     0,     0,     0,     0,    20,     0,
     0,     0,     0,     0,     0,     0,    21,     0,    22,     0,
    23,     0,     0,     0,     0,    64,     0,   282,    26,    27,
     6,     7,     8,     9,    10,    11,     0,    12,     0,     0,
     0,     0,    14,    15,    16,    17,    18,     0,    19,     0,
     0,     0,     0,     0,    20,     0,     7,     8,     9,    10,
    11,     0,    21,     0,    22,     0,    23,    14,    15,    16,
    17,    64,     0,    19,    26,    27,     0,     0,     0,    20,
     7,     8,     9,    10,    11,     0,    12,     0,     0,     0,
     0,    14,    15,    16,    17,     6,     0,    19,     0,     0,
   164,     0,     0,    20,     0,     0,     0,     6,     0,     0,
     0,     0,     0,     0,     0,   127,     0,    82,     0,    84,
    85,    86,   206,    88,    89,    90,     6,     0,     0,    82,
   259,    84,    85,    86,   206,    88,    89,    90,     0,     0,
     0,     0,   288,     0,     0,     0,     0,     0,    82,     0,
    84,    85,    86,    87,    88,    89,    90,    82,    83,    84,
    85,    86,    87,    88,    89,    90,    91,     0,     0,     0,
     0,     0,     0,     0,   123,    82,    83,    84,    85,    86,
    87,    88,    89,    90,    91
};

static const short yycheck[] = {     1,
    13,    21,    74,    76,   112,    25,   127,    77,    77,    10,
    20,     3,    36,    26,    27,    77,    77,    51,     3,   158,
    23,    12,    24,   212,   145,    32,    24,    24,    31,    77,
    42,     4,     5,    43,    36,    77,    43,    31,    39,    39,
    20,    32,   112,   112,    44,    77,    44,    44,    21,    73,
   112,   112,    44,    66,    67,   126,    77,   127,   127,    39,
    40,    10,    64,   154,   112,   127,   127,   206,    46,   190,
   112,    73,    92,    93,    94,   145,   145,   266,    91,   127,
   112,   101,    21,   145,   145,   127,     7,    28,    29,     2,
     3,   112,    33,   117,   107,   127,    24,   145,   111,   238,
    77,    66,    67,   145,   195,   125,   127,   141,    39,   102,
    41,   104,    37,   145,    27,   117,     1,   119,   188,   188,
   190,   190,   142,    43,   145,    45,   188,   188,   190,   190,
   143,   144,   146,   224,   104,   112,     3,    40,   277,    24,
   188,   175,   190,    10,    43,   158,   188,    43,   190,    45,
   127,    36,   132,   146,    45,   157,   188,    39,   190,   232,
   233,   185,    24,   176,    77,   178,    24,   188,   145,   190,
    37,   279,    39,    37,   247,   248,   146,   197,   249,   250,
    11,    20,    13,   185,   198,   199,    20,   167,    73,    42,
    43,   262,   263,   206,   276,   108,    45,   199,   280,   281,
   265,    43,    43,   275,    45,   198,   271,   272,    40,   279,
   279,   188,    34,   190,   127,    42,    43,   279,   279,     9,
   292,    11,    42,    43,    39,   238,   246,    43,   198,   199,
    40,   279,   145,   203,    42,    43,    40,   279,    38,    40,
    45,     3,    40,    45,   257,    26,    37,   279,    37,    41,
    32,   164,    38,    40,    42,    24,    24,    37,   279,    43,
    45,    37,   282,    25,   277,    27,    28,    29,    30,    31,
    32,    33,   274,    37,   276,   188,    38,   190,   280,   281,
     3,     4,     5,     6,     7,     8,    45,    10,    11,    42,
    38,    46,    15,    16,    17,    18,    19,    38,    21,    38,
    38,     0,   279,     0,    27,   211,     4,   251,   131,   285,
   103,    33,    35,   203,    37,   279,    39,   188,    24,   236,
    67,    44,   268,    46,    47,    48,     3,     4,     5,     6,
     7,     8,   111,    10,    11,    -1,    -1,    -1,    15,    16,
    17,    18,    19,    -1,    21,    -1,    -1,    -1,    -1,    -1,
    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    -1,
    37,    -1,    39,    -1,    -1,    -1,    -1,    44,    -1,    46,
    47,    48,     3,     4,     5,     6,     7,     8,    -1,    10,
    11,    -1,    -1,    -1,    15,    16,    17,    18,    19,    -1,
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
    38,    27,    28,    29,    30,    31,    32,    33,    -1,    -1,
    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    25,    -1,
    27,    28,    29,    30,    31,    32,    33,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    42,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34
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
#line 414 "lang11d"
{
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); 
				;
    break;}
case 124:
#line 418 "lang11d"
{ 
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]); 
				;
    break;}
case 125:
#line 423 "lang11d"
{ yyval = 0; ;
    break;}
case 128:
#line 429 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 129:
#line 433 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); ;
    break;}
case 130:
#line 435 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], 0); ;
    break;}
case 132:
#line 440 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 133:
#line 444 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); ;
    break;}
case 134:
#line 446 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], yyvsp[-3]); ;
    break;}
case 135:
#line 452 "lang11d"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 136:
#line 454 "lang11d"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 137:
#line 458 "lang11d"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 138:
#line 460 "lang11d"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 139:
#line 463 "lang11d"
{ yyval = 0; ;
    break;}
case 142:
#line 469 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 143:
#line 472 "lang11d"
{ yyval = rwPrivate; ;
    break;}
case 144:
#line 474 "lang11d"
{ yyval = rwReadOnly; ;
    break;}
case 145:
#line 476 "lang11d"
{ yyval = rwReadWrite; ;
    break;}
case 146:
#line 478 "lang11d"
{ yyval = rwWriteOnly; ;
    break;}
case 147:
#line 481 "lang11d"
{ yyval = zzval; ;
    break;}
case 148:
#line 483 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.ui = -node->slot.ui;
				yyval = zzval;
			;
    break;}
case 149:
#line 491 "lang11d"
{ yyval = zzval; ;
    break;}
case 150:
#line 493 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -node->slot.uf;
				yyval = zzval;
			;
    break;}
case 151:
#line 501 "lang11d"
{ yyval = zzval; ;
    break;}
case 153:
#line 506 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)yyvsp[-1];
				node->slot.uf *= pi;
			;
    break;}
case 154:
#line 512 "lang11d"
{
				PyrSlotNode *node; 
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = node->slot.ui;
				node->slot.uf = ival * pi;
			;
    break;}
case 155:
#line 520 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = pi;
				yyval = zzval;
			;
    break;}
case 156:
#line 527 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -pi;
				yyval = zzval;
			;
    break;}
case 157:
#line 535 "lang11d"
{ yyval = zzval; ;
    break;}
case 158:
#line 538 "lang11d"
{ yyval = zzval; ;
    break;}
case 159:
#line 541 "lang11d"
{ yyval = zzval; ;
    break;}
case 160:
#line 544 "lang11d"
{ yyval = zzval; ;
    break;}
case 161:
#line 547 "lang11d"
{ yyval = zzval; ;
    break;}
case 162:
#line 550 "lang11d"
{ yyval = zzval; ;
    break;}
case 163:
#line 553 "lang11d"
{ yyval = zzval; ;
    break;}
case 164:
#line 556 "lang11d"
{ yyval = zzval; ;
    break;}
case 165:
#line 559 "lang11d"
{ yyval = zzval; ;
    break;}
case 167:
#line 564 "lang11d"
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
#line 596 "lang11d"
{ yyval = zzval; ;
    break;}
case 172:
#line 597 "lang11d"
{ yyval = zzval; ;
    break;}
case 173:
#line 598 "lang11d"
{ yyval = zzval; ;
    break;}
case 174:
#line 599 "lang11d"
{ yyval = zzval; ;
    break;}
case 175:
#line 600 "lang11d"
{ yyval = zzval; ;
    break;}
case 176:
#line 601 "lang11d"
{ yyval = zzval; ;
    break;}
case 177:
#line 602 "lang11d"
{ yyval = zzval; ;
    break;}
case 178:
#line 605 "lang11d"
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
#line 608 "lang11d"
