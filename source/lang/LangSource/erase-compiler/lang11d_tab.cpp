
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
#define	DOUBLEEQUAL	281
#define	UMINUS	282

#line 15 "lang11d"


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



#define	YYFINAL		340
#define	YYFLAG		-32768
#define	YYNTBASE	50

#define YYTRANSLATE(x) ((unsigned)(x) <= 282 ? yytranslate[x] : 124)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,    49,     2,     2,     2,     2,    45,
    46,    31,    32,    44,    28,    36,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    24,    43,    29,
    25,    30,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    41,     2,    42,    47,     2,    37,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    39,    33,    40,    48,     2,     2,     2,     2,
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
    17,    18,    19,    20,    21,    22,    23,    26,    27,    34,
    35,    38
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     7,     8,    11,    13,    16,    23,    33,
    39,    40,    42,    43,    46,    47,    50,    54,    58,    59,
    62,    70,    79,    87,    96,    97,    99,   100,   102,   104,
   107,   112,   115,   117,   119,   122,   123,   126,   127,   131,
   132,   136,   138,   141,   142,   144,   147,   154,   161,   169,
   177,   186,   191,   193,   195,   197,   199,   203,   206,   210,
   214,   216,   221,   227,   232,   237,   239,   245,   247,   250,
   254,   258,   262,   267,   273,   281,   286,   293,   301,   303,
   307,   310,   311,   314,   316,   320,   322,   326,   329,   330,
   333,   335,   339,   341,   345,   347,   349,   351,   353,   355,
   357,   359,   361,   363,   365,   367,   369,   371,   373,   375,
   377,   379,   381,   383,   385,   387,   389,   391,   393,   395,
   397,   399,   401,   403,   405,   407,   409,   415,   422,   423,
   426,   428,   431,   435,   436,   440,   446,   450,   456,   457,
   459,   461,   465,   467,   471,   475,   478,   480,   484,   485,
   487,   489,   493,   496,   501,   506,   512,   516,   521,   522,
   525,   527,   531,   532,   534,   536,   538,   540,   543,   545,
   548,   550,   552,   555,   558,   560,   563,   565,   567,   569,
   571,   573,   575,   577,   579,   581,   583,   586,   588,   590,
   592,   594,   596,   598,   600,   602,   604,   606,   608
};

static const short yyrhs[] = {    51,
     0,    52,     0,    23,    64,     0,     0,    51,    53,     0,
    54,     0,    52,    54,     0,   111,    56,    39,    57,    59,
    40,     0,   111,    41,    55,    42,    56,    39,    57,    59,
    40,     0,    32,   111,    39,    59,    40,     0,     0,   110,
     0,     0,    24,   111,     0,     0,    57,    58,     0,    13,
    99,    43,     0,    11,    99,    43,     0,     0,    59,    60,
     0,   110,    39,    92,    89,    66,    65,    40,     0,    31,
   110,    39,    92,    89,    66,    65,    40,     0,   122,    39,
    92,    89,    66,    65,    40,     0,    31,   122,    39,    92,
    89,    66,    65,    40,     0,     0,    43,     0,     0,    44,
     0,    68,     0,    75,    68,     0,    45,    90,    63,    46,
     0,    90,    63,     0,    63,     0,    67,     0,    75,    67,
     0,     0,   112,    61,     0,     0,    47,    73,    61,     0,
     0,    47,    73,    61,     0,    84,     0,    69,    84,     0,
     0,    69,     0,   110,    69,     0,   110,    45,    77,    80,
    46,    70,     0,    73,    36,    45,    78,    46,    70,     0,
    73,    36,   110,    45,    78,    46,    70,     0,    73,    36,
    45,    77,    80,    46,    70,     0,    73,    36,   110,    45,
    77,    80,    46,    70,     0,    73,    36,   110,    70,     0,
    86,     0,    84,     0,    85,     0,    71,     0,    45,    75,
    46,     0,    48,   110,     0,    41,    76,    42,     0,    45,
    98,    46,     0,   121,     0,   111,    45,    78,    46,     0,
   111,    45,    77,    80,    46,     0,   111,    41,    76,    42,
     0,    72,    41,    77,    42,     0,    72,     0,    73,    36,
    41,    77,    42,     0,   111,     0,    37,    73,     0,    73,
   122,    73,     0,    73,   123,    73,     0,   110,    25,    73,
     0,    48,   110,    25,    73,     0,    73,    36,   110,    25,
    73,     0,   110,    45,    77,    80,    46,    25,    73,     0,
    49,    81,    25,    73,     0,    72,    41,    77,    42,    25,
    73,     0,    73,    36,    41,    77,    42,    25,    73,     0,
    73,     0,    74,    43,    73,     0,    74,    61,     0,     0,
    77,    62,     0,    75,     0,    77,    44,    75,     0,    79,
     0,    78,    44,    79,     0,   123,    75,     0,     0,    44,
    78,     0,    82,     0,    82,    20,   110,     0,   110,     0,
    82,    44,   110,     0,   106,     0,   109,     0,   117,     0,
   119,     0,   118,     0,   113,     0,   114,     0,   115,     0,
   116,     0,   101,     0,    88,     0,   110,     0,   106,     0,
   109,     0,   117,     0,   119,     0,   118,     0,   113,     0,
   114,     0,   115,     0,   116,     0,   101,     0,   106,     0,
   109,     0,   117,     0,   119,     0,   118,     0,   113,     0,
   114,     0,   115,     0,   116,     0,   102,     0,    39,    92,
    89,    63,    40,     0,    49,    39,    92,    89,    63,    40,
     0,     0,    89,    91,     0,    91,     0,    90,    91,     0,
    11,    94,    43,     0,     0,    12,    94,    43,     0,    12,
    93,    20,   110,    43,     0,    33,    94,    33,     0,    33,
    93,    20,   110,    33,     0,     0,    94,     0,    95,     0,
    94,    44,    95,     0,   110,     0,   110,    25,    83,     0,
    73,    24,    73,     0,   123,    73,     0,    96,     0,    97,
    44,    96,     0,     0,    97,     0,   100,     0,    99,    44,
   100,     0,   105,   110,     0,   105,   110,    25,    83,     0,
    49,    41,   103,    42,     0,    49,   111,    41,   103,    42,
     0,    41,   103,    42,     0,   111,    41,   103,    42,     0,
     0,   104,    62,     0,    87,     0,   104,    44,    87,     0,
     0,    29,     0,    34,     0,    30,     0,     4,     0,    28,
     4,     0,     5,     0,    28,     5,     0,    21,     0,   107,
     0,   107,   108,     0,   106,   108,     0,   108,     0,    28,
   108,     0,     3,     0,    10,     0,     9,     0,    16,     0,
    17,     0,    15,     0,    18,     0,     8,     0,     6,     0,
   120,     0,   119,   120,     0,     7,     0,    19,     0,    26,
     0,    34,     0,    35,     0,    29,     0,    30,     0,    28,
     0,    31,     0,    32,     0,    33,     0,    27,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    35,    37,    39,    43,    44,    48,    49,    53,    57,    64,
    70,    71,    74,    75,    79,    80,    84,    86,    90,    91,
    95,    98,   101,   104,   109,   110,   113,   114,   117,   118,
   122,   124,   126,   130,   131,   135,   136,   140,   142,   146,
   148,   152,   153,   159,   160,   163,   167,   172,   181,   186,
   199,   205,   211,   212,   213,   214,   215,   217,   227,   229,
   231,   233,   244,   257,   259,   274,   275,   288,   290,   304,
   309,   314,   318,   329,   334,   344,   349,   363,   379,   380,
   386,   389,   390,   393,   394,   398,   399,   403,   407,   408,
   411,   413,   417,   418,   423,   424,   425,   426,   427,   428,
   429,   430,   431,   432,   435,   438,   441,   442,   443,   444,
   445,   446,   447,   448,   449,   450,   453,   454,   455,   456,
   457,   458,   459,   460,   461,   462,   465,   468,   473,   474,
   478,   479,   483,   487,   488,   492,   496,   500,   507,   508,
   511,   512,   516,   518,   522,   524,   531,   532,   536,   537,
   540,   541,   545,   547,   553,   555,   559,   561,   565,   566,
   569,   570,   574,   575,   577,   579,   583,   584,   593,   594,
   603,   606,   607,   613,   621,   628,   637,   640,   643,   646,
   649,   652,   655,   658,   661,   664,   665,   691,   694,   697,
   698,   699,   700,   701,   702,   703,   704,   705,   708
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","NAME","INTEGER",
"FLOAT","SYMBOL","STRING","ASCII","PRIMITIVENAME","CLASSNAME","VAR","ARG","CLASSVAR",
"CONST","NILOBJ","TRUEOBJ","FALSEOBJ","INFINITUMOBJ","PSEUDOVAR","ELLIPSIS",
"PIE","BADTOKEN","INTERPRET","':'","'='","BINOP","KEYBINOP","'-'","'<'","'>'",
"'*'","'+'","'|'","READWRITEVAR","DOUBLEEQUAL","'.'","'`'","UMINUS","'{'","'}'",
"'['","']'","';'","','","'('","')'","'^'","'~'","'#'","root","classes","classextensions",
"classdef","classextension","optname","superclass","classvardecls","classvardecl",
"methods","methoddef","optsemi","optcomma","funcbody","cmdlinecode","methbody",
"primitive","retval","funretval","blocklist1","blocklist","msgsend","expr1",
"expr","exprn","exprseq","arglistc","arglist1","keyarglist1","keyarg","optkeyarglist",
"mavars","mavarlist","slotliteral","blockliteral","pushname","pushliteral","listliteral",
"block","funcvardecls","funcvardecls1","funcvardecl","argdecls","slotdeflist0",
"slotdeflist","slotdef","dictslotdef","dictslotlist1","dictslotlist","rwslotdeflist",
"rwslotdef","listlit","listlit2","literallistc","literallist1","rwspec","integer",
"floatr","pie","floatp","name","classname","primname","trueobj","falseobj","nilobj",
"infobj","ascii","symbol","string","string1","pseudovar","binop","keybinop", NULL
};
#endif

static const short yyr1[] = {     0,
    50,    50,    50,    51,    51,    52,    52,    53,    53,    54,
    55,    55,    56,    56,    57,    57,    58,    58,    59,    59,
    60,    60,    60,    60,    61,    61,    62,    62,    63,    63,
    64,    64,    64,    65,    65,    66,    66,    67,    67,    68,
    68,    69,    69,    70,    70,    71,    71,    71,    71,    71,
    71,    71,    72,    72,    72,    72,    72,    72,    72,    72,
    72,    72,    72,    72,    72,    73,    73,    73,    73,    73,
    73,    73,    73,    73,    73,    73,    73,    73,    74,    74,
    75,    76,    76,    77,    77,    78,    78,    79,    80,    80,
    81,    81,    82,    82,    83,    83,    83,    83,    83,    83,
    83,    83,    83,    83,    84,    85,    86,    86,    86,    86,
    86,    86,    86,    86,    86,    86,    87,    87,    87,    87,
    87,    87,    87,    87,    87,    87,    88,    88,    89,    89,
    90,    90,    91,    92,    92,    92,    92,    92,    93,    93,
    94,    94,    95,    95,    96,    96,    97,    97,    98,    98,
    99,    99,   100,   100,   101,   101,   102,   102,   103,   103,
   104,   104,   105,   105,   105,   105,   106,   106,   107,   107,
   108,   109,   109,   109,   109,   109,   110,   111,   112,   113,
   114,   115,   116,   117,   118,   119,   119,   120,   121,   122,
   122,   122,   122,   122,   122,   122,   122,   122,   123
};

static const short yyr2[] = {     0,
     1,     1,     2,     0,     2,     1,     2,     6,     9,     5,
     0,     1,     0,     2,     0,     2,     3,     3,     0,     2,
     7,     8,     7,     8,     0,     1,     0,     1,     1,     2,
     4,     2,     1,     1,     2,     0,     2,     0,     3,     0,
     3,     1,     2,     0,     1,     2,     6,     6,     7,     7,
     8,     4,     1,     1,     1,     1,     3,     2,     3,     3,
     1,     4,     5,     4,     4,     1,     5,     1,     2,     3,
     3,     3,     4,     5,     7,     4,     6,     7,     1,     3,
     2,     0,     2,     1,     3,     1,     3,     2,     0,     2,
     1,     3,     1,     3,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     5,     6,     0,     2,
     1,     2,     3,     0,     3,     5,     3,     5,     0,     1,
     1,     3,     1,     3,     3,     2,     1,     3,     0,     1,
     1,     3,     2,     4,     4,     5,     3,     4,     0,     2,
     1,     3,     0,     1,     1,     1,     1,     2,     1,     2,
     1,     1,     2,     2,     1,     2,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1
};

static const short yydefact[] = {     4,
    40,     0,     1,     2,     6,   177,   167,   169,   185,   188,
   184,   178,     0,   182,   180,   181,   183,   189,   171,     0,
     0,   134,    82,   149,     0,     0,     0,    33,     3,    29,
    56,    66,    79,    25,    40,    54,    55,    53,   105,    40,
   131,   116,   107,   172,   175,   108,   106,    68,   112,   113,
   114,   115,   109,   111,   110,   186,    61,     0,     5,    13,
     7,     0,   141,   143,   168,   170,   176,   149,    69,   139,
   139,   129,    84,     0,    27,   199,    79,     0,    40,   147,
   150,     0,     0,    25,    58,   134,   159,     0,    91,    93,
     0,     0,   190,   195,   193,   194,   196,   197,   198,   191,
   192,     0,     0,     0,    26,    81,    30,    32,   132,   174,
   173,     0,     0,     0,    46,    42,    82,     0,   187,    19,
     0,    11,     0,   133,     0,     0,     0,   140,     0,   140,
    40,    59,    28,    83,     0,    57,     0,     0,    60,   146,
    26,    41,     0,   129,   159,   161,   126,     0,    27,   117,
   118,     0,   122,   123,   124,   125,   119,   121,   120,     0,
     0,     0,   159,     0,     0,     0,    44,    70,    71,    80,
    72,    89,    43,     0,    89,     0,    86,     0,     0,    14,
     0,    12,    15,   142,     0,   144,   104,    95,    96,   100,
   101,   102,   103,    97,    99,    98,     0,   135,     0,   137,
     0,   130,    85,   145,    31,     0,   148,    73,    40,     0,
   155,    28,   160,   159,    76,    92,    94,     0,    65,     0,
     0,    89,     0,     0,     0,    45,    52,     0,     0,    64,
     0,     0,    62,    88,   196,    10,    20,     0,     0,    13,
    19,     0,     0,   127,     0,   157,   162,     0,   156,     0,
    67,     0,    44,    74,    89,     0,    90,    44,    63,    87,
     0,     0,   134,   134,     0,   163,   163,    16,     0,   136,
   138,   128,   158,    77,     0,    44,    48,     0,    44,     0,
    47,   134,   134,   129,   129,    15,   164,   166,   165,     0,
   151,     0,     0,     8,    78,    50,    44,    49,    75,   129,
   129,    36,    36,    19,    18,   163,   153,    17,    51,    36,
    36,   179,    38,    25,    38,     0,   152,     0,    38,    38,
     0,     0,    34,    38,    37,     0,     9,   154,     0,     0,
    25,    21,    35,    23,    22,    24,    39,     0,     0,     0
};

static const short yydefgoto[] = {   338,
     3,     4,    59,     5,   181,   123,   241,   268,   179,   237,
   106,   134,    28,    29,   322,   313,   323,    30,   226,   227,
    31,    32,    33,    34,    73,    74,    75,   176,   177,   229,
    88,    89,   186,    36,    37,    38,   146,    39,   131,    40,
   202,    72,   127,    62,    63,    80,    81,    82,   290,   291,
    42,   147,   148,   149,   292,    43,    44,    45,    46,    47,
    48,   314,    49,    50,    51,    52,    53,    54,    55,    56,
    57,   103,   104
};

static const short yypact[] = {   112,
   317,    30,    30,    11,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    52,-32768,-32768,-32768,-32768,-32768,-32768,   199,
   552,    42,   552,   364,   552,    52,    24,-32768,-32768,-32768,
-32768,    21,   705,    25,    27,-32768,-32768,-32768,-32768,   411,
-32768,-32768,    68,    68,-32768,-32768,    73,    80,-32768,-32768,
-32768,-32768,-32768,-32768,   108,-32768,-32768,    95,-32768,   -16,
-32768,   167,-32768,   115,-32768,-32768,-32768,   458,-32768,    52,
    52,-32768,-32768,   120,   131,-32768,   694,   142,   411,-32768,
   147,   152,   552,   676,   176,    42,   617,   204,     8,-32768,
   177,   552,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   161,   552,   552,   552,-32768,-32768,-32768,-32768,-32768,
-32768,   552,   552,   194,    -4,-32768,   552,   458,-32768,-32768,
    30,    52,   201,-32768,    52,   598,   230,   193,   239,   -24,
   411,-32768,   552,-32768,   552,-32768,   228,   458,-32768,   705,
-32768,-32768,   552,-32768,   617,-32768,-32768,   221,   234,    68,
-32768,   238,-32768,-32768,-32768,-32768,-32768,-32768,   108,   552,
    52,    52,   617,    63,   552,   458,   168,   245,   245,   705,
   705,   241,-32768,   240,   241,   109,-32768,   552,   263,-32768,
   244,-32768,-32768,-32768,    20,-32768,-32768,    68,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   108,    52,-32768,    52,-32768,
   247,-32768,-32768,   705,-32768,   694,-32768,   705,   411,   248,
-32768,   617,-32768,   617,   705,-32768,-32768,   257,   275,   552,
   117,   241,   178,   552,   458,    -4,-32768,   458,   255,-32768,
   259,   279,-32768,-32768,   666,-32768,-32768,   269,   270,   287,
   217,   272,   280,-32768,   276,-32768,-32768,   284,-32768,   552,
   292,   283,    -4,   705,   241,   214,   286,    -3,-32768,-32768,
   301,   302,    42,    42,   303,   160,   160,-32768,   633,-32768,
-32768,-32768,-32768,   705,   552,    -4,-32768,   285,    -4,   552,
-32768,    42,    42,-32768,-32768,-32768,-32768,-32768,-32768,   211,
-32768,    52,   224,-32768,   705,-32768,    -4,-32768,   705,-32768,
-32768,   262,   262,   217,-32768,   160,   318,-32768,-32768,   262,
   262,-32768,   505,   304,   505,   646,-32768,   598,   505,   505,
   552,   306,-32768,   297,-32768,   308,-32768,-32768,   309,   310,
   676,-32768,-32768,-32768,-32768,-32768,-32768,   351,   352,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,   349,-32768,   119,    69,-32768,  -209,-32768,
   -80,   208,   -23,-32768,   -94,  -127,    36,   326,   316,   -14,
-32768,-32768,   -19,-32768,    -1,   256,    82,  -124,   144,  -149,
-32768,-32768,    59,   -44,-32768,-32768,   166,-32768,  -128,   360,
   102,   -67,   315,   206,   264,   249,-32768,-32768,   121,    84,
  -119,-32768,  -132,-32768,-32768,   -76,-32768,     4,   -75,   -12,
   125,-32768,   -69,   -66,   -54,   -49,   -46,   -43,   -34,   -26,
-32768,  -169,    13
};


#define	YYLAST		741


static const short yytable[] = {    35,
    64,    69,   116,   142,    77,    84,   187,   121,   200,   239,
   150,   151,   210,    85,    90,   209,   108,   153,   144,   125,
   154,   280,    78,    67,   122,   231,     6,   161,   119,    12,
   218,   269,   155,    12,    22,    22,    83,   156,    35,    12,
   157,   223,     2,   158,   114,   114,   110,   111,    77,   188,
   189,   162,   159,    70,     6,   137,   190,    64,    64,   191,
    87,    92,    86,   140,    87,   262,    78,   105,   150,   151,
   173,   192,   252,    25,    71,   153,   193,    35,   154,   194,
    83,   248,   195,   168,   169,   170,   150,   151,    19,   167,
   155,   196,   171,   153,   316,   156,   154,   112,   157,   239,
   256,   158,    41,   257,   219,   278,   220,   201,   155,   182,
   159,    22,    64,   156,    10,   204,   157,   113,   206,   158,
   117,   114,   116,   208,   118,    41,    58,    60,   159,    35,
   178,   203,   119,   120,     1,   150,   151,   150,   151,   126,
   215,   109,   153,     2,   153,   154,   239,   154,   216,   217,
    83,    91,   232,   110,   233,   302,   303,   155,   251,   155,
   220,   132,   156,     6,   156,   157,   238,   157,   158,   119,
   158,   310,   311,   164,   133,   315,   234,   159,   178,   159,
   109,   173,   319,   320,   242,   245,   243,   136,   287,   288,
   138,   110,   224,   289,   172,   284,   285,   139,   187,   175,
   143,   165,    65,    66,   254,   166,    22,    35,   116,   124,
   125,   152,   225,   116,   300,   301,   114,   163,   203,    19,
   326,   232,   261,   253,   329,   330,   203,   266,   160,   267,
   274,   116,    86,   325,   116,   198,   125,   178,   277,   183,
   178,   188,   189,   281,   178,   180,   221,   222,   190,   197,
   337,   191,   116,   305,   306,   295,   238,   232,   199,   279,
   299,   296,   211,   192,   298,     6,   308,   306,   193,   152,
   312,   194,    13,   205,   195,   128,   130,   212,   214,   307,
   102,   230,   309,   196,   228,   240,   244,   152,    93,   246,
    94,    95,    96,   235,    98,    99,   100,   101,   249,   250,
   258,   331,   236,   238,   259,    76,   255,   263,   264,    91,
   121,   324,   271,   324,   270,   272,   275,   324,   324,     6,
     7,     8,     9,    10,    11,   273,    12,    13,   276,   232,
   297,    14,    15,    16,    17,    18,   152,    19,   152,   282,
   283,   286,   318,   321,    20,   332,   141,   334,   335,   336,
   339,   340,    61,    21,   304,    22,   213,    23,   265,   333,
   107,    24,   115,    25,    26,    27,     6,     7,     8,     9,
    10,    11,   174,    12,    13,   260,   328,   247,    14,    15,
    16,    17,    18,    79,    19,   129,   207,   293,   184,   317,
    76,    20,     0,     0,     0,     0,     0,     0,     0,     0,
    21,     0,    22,     0,    23,     0,     0,     0,    68,     0,
     0,    26,    27,     6,     7,     8,     9,    10,    11,     0,
    12,    13,     0,     0,     0,    14,    15,    16,    17,    18,
     0,    19,     0,     0,     0,     0,     0,     0,    20,     0,
     0,     0,     0,     0,     0,     0,     0,    21,     0,    22,
     0,    23,     0,     0,     0,    68,     0,    25,    26,    27,
     6,     7,     8,     9,    10,    11,     0,    12,     0,     0,
     0,     0,    14,    15,    16,    17,    18,     0,    19,     0,
     0,     0,     0,     0,    76,    20,     0,     0,     0,     0,
     0,     0,     0,     0,    21,     0,    22,     0,    23,     0,
     0,     0,    68,     0,     0,    26,    27,     6,     7,     8,
     9,    10,    11,     0,    12,     0,     0,     0,     0,    14,
    15,    16,    17,    18,     0,    19,     0,     0,     0,     0,
     0,     0,    20,     0,     0,     0,     0,     0,     0,     0,
     0,    21,     0,    22,     0,    23,     0,     0,     0,    68,
     0,   321,    26,    27,     6,     7,     8,     9,    10,    11,
     0,    12,     0,     0,     0,     0,    14,    15,    16,    17,
    18,     0,    19,     0,     0,     0,     0,     0,     0,    20,
     0,     0,     0,     0,     0,     0,     0,     0,    21,     0,
    22,     0,    23,     0,     0,     0,    68,     0,     0,    26,
    27,     7,     8,     9,    10,    11,     0,     0,     0,     0,
     0,     0,    14,    15,    16,    17,     0,     0,    19,     0,
     7,     8,     9,    10,    11,    20,    12,     0,     0,     0,
     0,    14,    15,    16,    17,     6,     0,    19,     0,     0,
     0,     0,     0,     0,    20,     0,   185,     0,     6,     0,
     0,     0,     0,     0,     0,     0,     0,   145,    93,     0,
    94,    95,    96,   235,    98,    99,   100,   101,     6,     0,
     0,    93,   294,    94,    95,    96,   235,    98,    99,   100,
   101,     0,     0,     0,     0,   327,     0,     0,     0,     0,
     0,    93,     0,    94,    95,    96,    97,    98,    99,   100,
   101,    93,    76,    94,    95,    96,    97,    98,    99,   100,
   101,   102,     0,     0,     0,     0,     0,   135,   141,    93,
    76,    94,    95,    96,    97,    98,    99,   100,   101,   102,
    93,    76,    94,    95,    96,    97,    98,    99,   100,   101,
   102
};

static const short yycheck[] = {     1,
    13,    21,    47,    84,    24,    25,   126,    24,    33,   179,
    87,    87,   145,    26,    27,   144,    40,    87,    86,    44,
    87,    25,    24,    20,    41,   175,     3,    20,    55,    10,
   163,   241,    87,    10,    39,    39,    24,    87,    40,    10,
    87,   166,    32,    87,    49,    49,    43,    44,    68,   126,
   126,    44,    87,    12,     3,    79,   126,    70,    71,   126,
    41,    41,    39,    83,    41,   235,    68,    43,   145,   145,
   115,   126,   222,    47,    33,   145,   126,    79,   145,   126,
    68,   214,   126,   103,   104,   105,   163,   163,    21,   102,
   145,   126,   112,   163,   304,   145,   163,    25,   145,   269,
   225,   145,     1,   228,    42,   255,    44,   131,   163,   122,
   145,    39,   125,   163,     7,   135,   163,    45,   138,   163,
    41,    49,   167,   143,    45,    24,     2,     3,   163,   131,
   118,   133,   159,    39,    23,   212,   212,   214,   214,    25,
   160,    40,   212,    32,   214,   212,   316,   214,   161,   162,
   138,    27,    44,   150,    46,   284,   285,   212,    42,   214,
    44,    42,   212,     3,   214,   212,   179,   214,   212,   196,
   214,   300,   301,    92,    44,   303,   178,   212,   166,   214,
    79,   226,   310,   311,   197,   209,   199,    46,    29,    30,
    44,   188,    25,    34,   113,   263,   264,    46,   318,   118,
    25,    41,     4,     5,   224,    45,    39,   209,   253,    43,
    44,    87,    45,   258,   282,   283,    49,    41,   220,    21,
   315,    44,   235,    46,   319,   320,   228,    11,    25,    13,
   250,   276,    39,   314,   279,    43,    44,   225,   253,    39,
   228,   318,   318,   258,   232,   121,   165,   166,   318,    20,
   331,   318,   297,    43,    44,   275,   269,    44,    20,    46,
   280,   276,    42,   318,   279,     3,    43,    44,   318,   145,
     9,   318,    11,    46,   318,    70,    71,    44,    41,   292,
    36,    42,   297,   318,    44,    42,    40,   163,    26,    42,
    28,    29,    30,    31,    32,    33,    34,    35,    42,    25,
    46,   321,    40,   316,    46,    27,   225,    39,    39,   185,
    24,   313,    33,   315,    43,    40,    25,   319,   320,     3,
     4,     5,     6,     7,     8,    42,    10,    11,    46,    44,
    46,    15,    16,    17,    18,    19,   212,    21,   214,    39,
    39,    39,    25,    47,    28,    40,    43,    40,    40,    40,
     0,     0,     4,    37,   286,    39,   149,    41,   240,   324,
    35,    45,    47,    47,    48,    49,     3,     4,     5,     6,
     7,     8,   117,    10,    11,   232,   318,   212,    15,    16,
    17,    18,    19,    24,    21,    71,   138,   267,   125,   306,
    27,    28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    37,    -1,    39,    -1,    41,    -1,    -1,    -1,    45,    -1,
    -1,    48,    49,     3,     4,     5,     6,     7,     8,    -1,
    10,    11,    -1,    -1,    -1,    15,    16,    17,    18,    19,
    -1,    21,    -1,    -1,    -1,    -1,    -1,    -1,    28,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,    -1,    39,
    -1,    41,    -1,    -1,    -1,    45,    -1,    47,    48,    49,
     3,     4,     5,     6,     7,     8,    -1,    10,    -1,    -1,
    -1,    -1,    15,    16,    17,    18,    19,    -1,    21,    -1,
    -1,    -1,    -1,    -1,    27,    28,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    37,    -1,    39,    -1,    41,    -1,
    -1,    -1,    45,    -1,    -1,    48,    49,     3,     4,     5,
     6,     7,     8,    -1,    10,    -1,    -1,    -1,    -1,    15,
    16,    17,    18,    19,    -1,    21,    -1,    -1,    -1,    -1,
    -1,    -1,    28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    37,    -1,    39,    -1,    41,    -1,    -1,    -1,    45,
    -1,    47,    48,    49,     3,     4,     5,     6,     7,     8,
    -1,    10,    -1,    -1,    -1,    -1,    15,    16,    17,    18,
    19,    -1,    21,    -1,    -1,    -1,    -1,    -1,    -1,    28,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,    -1,
    39,    -1,    41,    -1,    -1,    -1,    45,    -1,    -1,    48,
    49,     4,     5,     6,     7,     8,    -1,    -1,    -1,    -1,
    -1,    -1,    15,    16,    17,    18,    -1,    -1,    21,    -1,
     4,     5,     6,     7,     8,    28,    10,    -1,    -1,    -1,
    -1,    15,    16,    17,    18,     3,    -1,    21,    -1,    -1,
    -1,    -1,    -1,    -1,    28,    -1,    49,    -1,     3,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    26,    -1,
    28,    29,    30,    31,    32,    33,    34,    35,     3,    -1,
    -1,    26,    40,    28,    29,    30,    31,    32,    33,    34,
    35,    -1,    -1,    -1,    -1,    40,    -1,    -1,    -1,    -1,
    -1,    26,    -1,    28,    29,    30,    31,    32,    33,    34,
    35,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    -1,    -1,    -1,    -1,    -1,    24,    43,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36
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
#line 36 "lang11d"
{ gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; ;
    break;}
case 2:
#line 38 "lang11d"
{ gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; ;
    break;}
case 3:
#line 40 "lang11d"
{ gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 2; ;
    break;}
case 4:
#line 43 "lang11d"
{ yyval = 0; ;
    break;}
case 5:
#line 45 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 7:
#line 50 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 8:
#line 54 "lang11d"
{ yyval = (long)newPyrClassNode((PyrSlotNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-4], 
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1], 0); 
				;
    break;}
case 9:
#line 58 "lang11d"
{ yyval = (long)newPyrClassNode((PyrSlotNode*)yyvsp[-8], (PyrSlotNode*)yyvsp[-4], 
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1], 
					(PyrSlotNode*)yyvsp[-6]); 
				;
    break;}
case 10:
#line 65 "lang11d"
{ 
					yyval = (long)newPyrClassExtNode((PyrSlotNode*)yyvsp[-3], (PyrMethodNode*)yyvsp[-1]); 
				;
    break;}
case 11:
#line 70 "lang11d"
{ yyval = 0; ;
    break;}
case 13:
#line 74 "lang11d"
{ yyval = 0; ;
    break;}
case 14:
#line 76 "lang11d"
{ yyval = yyvsp[0]; ;
    break;}
case 15:
#line 79 "lang11d"
{ yyval = 0; ;
    break;}
case 16:
#line 81 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 17:
#line 85 "lang11d"
{ yyval = (long)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varClass); ;
    break;}
case 18:
#line 87 "lang11d"
{ yyval = (long)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varInst); ;
    break;}
case 19:
#line 90 "lang11d"
{ yyval = 0; ;
    break;}
case 20:
#line 92 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 21:
#line 96 "lang11d"
{ yyval = (long)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2], 
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); ;
    break;}
case 22:
#line 99 "lang11d"
{ yyval = (long)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2], 
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); ;
    break;}
case 23:
#line 102 "lang11d"
{ yyval = (long)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2], 
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); ;
    break;}
case 24:
#line 105 "lang11d"
{ yyval = (long)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2], 
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); ;
    break;}
case 30:
#line 119 "lang11d"
{ yyval = (long)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 31:
#line 123 "lang11d"
{ yyval = (long)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); ;
    break;}
case 32:
#line 125 "lang11d"
{ yyval = (long)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); ;
    break;}
case 33:
#line 127 "lang11d"
{ yyval = (long)newPyrBlockNode(NULL, NULL, (PyrParseNode*)yyvsp[0], false); ;
    break;}
case 35:
#line 132 "lang11d"
{ yyval = (long)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 36:
#line 135 "lang11d"
{ yyval = 0; ;
    break;}
case 37:
#line 137 "lang11d"
{ yyval = yyvsp[-1]; ;
    break;}
case 38:
#line 141 "lang11d"
{ yyval = (long)newPyrReturnNode(NULL); ;
    break;}
case 39:
#line 143 "lang11d"
{ yyval = (long)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); ;
    break;}
case 40:
#line 147 "lang11d"
{ yyval = (long)newPyrBlockReturnNode(); ;
    break;}
case 41:
#line 149 "lang11d"
{ yyval = (long)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); ;
    break;}
case 43:
#line 154 "lang11d"
{ 
					yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); 
				;
    break;}
case 44:
#line 159 "lang11d"
{ yyval = 0; ;
    break;}
case 46:
#line 164 "lang11d"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], 0, 0); 
			;
    break;}
case 47:
#line 168 "lang11d"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-3], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 48:
#line 173 "lang11d"
{ 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				
				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				yyval = (long)newPyrCallNode(selectornode, (PyrParseNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 49:
#line 182 "lang11d"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-4], (PyrParseNode*)yyvsp[-6], 
					(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 50:
#line 187 "lang11d"
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
#line 200 "lang11d"
{ 
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-3]);
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-5], args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 52:
#line 206 "lang11d"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[-3], 0, (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 57:
#line 216 "lang11d"
{ yyval = yyvsp[-1]; ;
    break;}
case 58:
#line 218 "lang11d"
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
#line 228 "lang11d"
{ yyval = (long)newPyrDynListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 60:
#line 230 "lang11d"
{ yyval = (long)newPyrDynDictNode((PyrParseNode*)yyvsp[-1]); ;
    break;}
case 61:
#line 232 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 62:
#line 234 "lang11d"
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
case 63:
#line 245 "lang11d"
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
case 64:
#line 258 "lang11d"
{ yyval = (long)newPyrDynListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 65:
#line 260 "lang11d"
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
case 67:
#line 276 "lang11d"
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
case 68:
#line 289 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 69:
#line 291 "lang11d"
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
case 70:
#line 305 "lang11d"
{ 
				yyval = (long)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-1], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 71:
#line 310 "lang11d"
{ 
				yyval = (long)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-1], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 72:
#line 315 "lang11d"
{ 
				yyval = (long)newPyrAssignNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); 
			;
    break;}
case 73:
#line 319 "lang11d"
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
case 74:
#line 330 "lang11d"
{ 
				yyval = (long)newPyrSetterNode((PyrSlotNode*)yyvsp[-2], 
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 75:
#line 335 "lang11d"
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
case 76:
#line 345 "lang11d"
{ 
				yyval = (long)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[0], 0); 
			;
    break;}
case 77:
#line 350 "lang11d"
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
case 78:
#line 364 "lang11d"
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
case 80:
#line 381 "lang11d"
{ 
				yyval = (long)newPyrDropNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 82:
#line 389 "lang11d"
{ yyval = 0; ;
    break;}
case 85:
#line 395 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 87:
#line 400 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 88:
#line 404 "lang11d"
{ yyval = (long)newPyrPushKeyArgNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 89:
#line 407 "lang11d"
{ yyval = 0; ;
    break;}
case 90:
#line 408 "lang11d"
{ yyval = yyvsp[0]; ;
    break;}
case 91:
#line 412 "lang11d"
{ yyval = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 92:
#line 414 "lang11d"
{ yyval = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); ;
    break;}
case 94:
#line 419 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 95:
#line 423 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 96:
#line 424 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 97:
#line 425 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 98:
#line 426 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 99:
#line 427 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 100:
#line 428 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 101:
#line 429 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 102:
#line 430 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 103:
#line 431 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 104:
#line 432 "lang11d"
{ yyval = (long)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 105:
#line 435 "lang11d"
{ yyval = (long)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 106:
#line 438 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 107:
#line 441 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 108:
#line 442 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 109:
#line 443 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 110:
#line 444 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 111:
#line 445 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 112:
#line 446 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 113:
#line 447 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 114:
#line 448 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 115:
#line 449 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 116:
#line 450 "lang11d"
{ yyval = (long)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 117:
#line 453 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 118:
#line 454 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 119:
#line 455 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 120:
#line 456 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 121:
#line 457 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 122:
#line 458 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 123:
#line 459 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 124:
#line 460 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 125:
#line 461 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 126:
#line 462 "lang11d"
{ yyval = (long)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 127:
#line 466 "lang11d"
{ yyval = (long)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[-1], false); ;
    break;}
case 128:
#line 469 "lang11d"
{ yyval = (long)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[-1], true); ;
    break;}
case 129:
#line 473 "lang11d"
{ yyval = 0; ;
    break;}
case 130:
#line 475 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 132:
#line 480 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 133:
#line 484 "lang11d"
{ yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); ;
    break;}
case 134:
#line 487 "lang11d"
{ yyval = 0; ;
    break;}
case 135:
#line 489 "lang11d"
{
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); 
				;
    break;}
case 136:
#line 493 "lang11d"
{ 
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]); 
				;
    break;}
case 137:
#line 497 "lang11d"
{
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); 
				;
    break;}
case 138:
#line 501 "lang11d"
{ 
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]); 
				;
    break;}
case 139:
#line 507 "lang11d"
{ yyval = 0; ;
    break;}
case 142:
#line 513 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 143:
#line 517 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); ;
    break;}
case 144:
#line 519 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], 0); ;
    break;}
case 145:
#line 523 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 146:
#line 525 "lang11d"
{
					PyrParseNode* key = newPyrPushLitNode((PyrSlotNode*)yyvsp[-1], NULL);
					yyval = (long)linkNextNode(key, (PyrParseNode*)yyvsp[0]); 
				;
    break;}
case 148:
#line 533 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 149:
#line 536 "lang11d"
{ yyval = 0; ;
    break;}
case 152:
#line 542 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 153:
#line 546 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); ;
    break;}
case 154:
#line 548 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], yyvsp[-3]); ;
    break;}
case 155:
#line 554 "lang11d"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 156:
#line 556 "lang11d"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 157:
#line 560 "lang11d"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 158:
#line 562 "lang11d"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 159:
#line 565 "lang11d"
{ yyval = 0; ;
    break;}
case 162:
#line 571 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 163:
#line 574 "lang11d"
{ yyval = rwPrivate; ;
    break;}
case 164:
#line 576 "lang11d"
{ yyval = rwReadOnly; ;
    break;}
case 165:
#line 578 "lang11d"
{ yyval = rwReadWrite; ;
    break;}
case 166:
#line 580 "lang11d"
{ yyval = rwWriteOnly; ;
    break;}
case 167:
#line 583 "lang11d"
{ yyval = zzval; ;
    break;}
case 168:
#line 585 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.ui = -node->slot.ui;
				yyval = zzval;
			;
    break;}
case 169:
#line 593 "lang11d"
{ yyval = zzval; ;
    break;}
case 170:
#line 595 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -node->slot.uf;
				yyval = zzval;
			;
    break;}
case 171:
#line 603 "lang11d"
{ yyval = zzval; ;
    break;}
case 173:
#line 608 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)yyvsp[-1];
				node->slot.uf *= pi;
			;
    break;}
case 174:
#line 614 "lang11d"
{
				PyrSlotNode *node; 
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = node->slot.ui;
				node->slot.uf = ival * pi;
			;
    break;}
case 175:
#line 622 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = pi;
				yyval = zzval;
			;
    break;}
case 176:
#line 629 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -pi;
				yyval = zzval;
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
#line 646 "lang11d"
{ yyval = zzval; ;
    break;}
case 181:
#line 649 "lang11d"
{ yyval = zzval; ;
    break;}
case 182:
#line 652 "lang11d"
{ yyval = zzval; ;
    break;}
case 183:
#line 655 "lang11d"
{ yyval = zzval; ;
    break;}
case 184:
#line 658 "lang11d"
{ yyval = zzval; ;
    break;}
case 185:
#line 661 "lang11d"
{ yyval = zzval; ;
    break;}
case 187:
#line 666 "lang11d"
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
case 188:
#line 691 "lang11d"
{ yyval = zzval; ;
    break;}
case 189:
#line 694 "lang11d"
{ yyval = zzval; ;
    break;}
case 190:
#line 697 "lang11d"
{ yyval = zzval; ;
    break;}
case 191:
#line 698 "lang11d"
{ yyval = zzval; ;
    break;}
case 192:
#line 699 "lang11d"
{ yyval = zzval; ;
    break;}
case 193:
#line 700 "lang11d"
{ yyval = zzval; ;
    break;}
case 194:
#line 701 "lang11d"
{ yyval = zzval; ;
    break;}
case 195:
#line 702 "lang11d"
{ yyval = zzval; ;
    break;}
case 196:
#line 703 "lang11d"
{ yyval = zzval; ;
    break;}
case 197:
#line 704 "lang11d"
{ yyval = zzval; ;
    break;}
case 198:
#line 705 "lang11d"
{ yyval = zzval; ;
    break;}
case 199:
#line 708 "lang11d"
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
#line 711 "lang11d"
