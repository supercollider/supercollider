
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



#define	YYFINAL		311
#define	YYFLAG		-32768
#define	YYNTBASE	49

#define YYTRANSLATE(x) ((unsigned)(x) <= 281 ? yytranslate[x] : 117)

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
   132,   136,   137,   140,   147,   154,   162,   170,   179,   184,
   186,   188,   190,   192,   195,   197,   199,   204,   210,   214,
   217,   221,   225,   229,   234,   238,   243,   249,   257,   262,
   264,   268,   271,   272,   275,   277,   281,   283,   287,   290,
   291,   294,   296,   300,   302,   306,   308,   310,   312,   314,
   316,   318,   320,   322,   324,   326,   328,   330,   332,   334,
   336,   338,   340,   342,   344,   346,   348,   350,   352,   354,
   356,   358,   360,   362,   364,   366,   368,   374,   381,   382,
   385,   387,   390,   394,   395,   399,   405,   409,   415,   416,
   418,   420,   424,   426,   430,   432,   436,   439,   444,   449,
   455,   459,   464,   465,   468,   470,   474,   475,   477,   479,
   481,   483,   486,   488,   491,   493,   495,   498,   501,   503,
   506,   508,   510,   512,   514,   516,   518,   520,   522,   524,
   526,   529,   531,   533,   535,   537,   539,   541,   543,   545,
   547,   549
};

static const short yyrhs[] = {    50,
     0,    51,     0,    23,    63,     0,     0,    50,    52,     0,
    53,     0,    51,    53,     0,   104,    55,    37,    56,    58,
    38,     0,   104,    39,    54,    40,    55,    37,    56,    58,
    38,     0,    31,   104,    37,    58,    38,     0,     0,   103,
     0,     0,    41,   104,     0,     0,    56,    57,     0,    13,
    92,    42,     0,    11,    92,    42,     0,     0,    58,    59,
     0,   103,    37,    88,    85,    65,    64,    38,     0,    30,
   103,    37,    88,    85,    65,    64,    38,     0,   115,    37,
    88,    85,    65,    64,    38,     0,    30,   115,    37,    88,
    85,    65,    64,    38,     0,     0,    42,     0,     0,    43,
     0,    67,     0,    72,    67,     0,    44,    86,    62,    45,
     0,    86,    62,     0,    62,     0,    66,     0,    72,    66,
     0,     0,   105,    60,     0,     0,    46,    70,    60,     0,
     0,    46,    70,    60,     0,     0,    68,    81,     0,   103,
    44,    74,    77,    45,    68,     0,    70,    34,    44,    75,
    45,    68,     0,    70,    34,   103,    44,    75,    45,    68,
     0,    70,    34,    44,    74,    77,    45,    68,     0,    70,
    34,   103,    44,    74,    77,    45,    68,     0,    70,    34,
   103,    68,     0,    82,     0,    81,     0,    69,     0,   103,
     0,    47,   103,     0,   114,     0,   104,     0,   104,    44,
    75,    45,     0,   104,    44,    74,    77,    45,     0,    44,
    72,    45,     0,    35,    70,     0,    70,   115,    70,     0,
    70,   116,    70,     0,    39,    73,    40,     0,   104,    39,
    73,    40,     0,   103,    24,    70,     0,    47,   103,    24,
    70,     0,    70,    34,   103,    24,    70,     0,   103,    44,
    74,    77,    45,    24,    70,     0,    48,    78,    24,    70,
     0,    70,     0,    71,    42,    70,     0,    71,    60,     0,
     0,    74,    61,     0,    72,     0,    74,    43,    72,     0,
    76,     0,    75,    43,    76,     0,   116,    72,     0,     0,
    43,    75,     0,    79,     0,    79,    20,   103,     0,   103,
     0,    79,    43,   103,     0,    99,     0,   102,     0,   110,
     0,   112,     0,   111,     0,   106,     0,   107,     0,   108,
     0,   109,     0,    94,     0,    84,     0,    99,     0,   102,
     0,   110,     0,   112,     0,   111,     0,   106,     0,   107,
     0,   108,     0,   109,     0,    94,     0,    99,     0,   102,
     0,   110,     0,   112,     0,   111,     0,   106,     0,   107,
     0,   108,     0,   109,     0,    95,     0,    37,    88,    85,
    62,    38,     0,    48,    37,    88,    85,    62,    38,     0,
     0,    85,    87,     0,    87,     0,    86,    87,     0,    11,
    90,    42,     0,     0,    12,    90,    42,     0,    12,    89,
    20,   103,    42,     0,    32,    90,    32,     0,    32,    89,
    20,   103,    32,     0,     0,    90,     0,    91,     0,    90,
    43,    91,     0,   103,     0,   103,    24,    80,     0,    93,
     0,    92,    43,    93,     0,    98,   103,     0,    98,   103,
    24,    80,     0,    48,    39,    96,    40,     0,    48,   104,
    39,    96,    40,     0,    39,    96,    40,     0,   104,    39,
    96,    40,     0,     0,    97,    61,     0,    83,     0,    97,
    43,    83,     0,     0,    28,     0,    33,     0,    29,     0,
     4,     0,    27,     4,     0,     5,     0,    27,     5,     0,
    21,     0,   100,     0,   100,   101,     0,    99,   101,     0,
   101,     0,    27,   101,     0,     3,     0,    10,     0,     9,
     0,    16,     0,    17,     0,    15,     0,    18,     0,     8,
     0,     6,     0,   113,     0,   112,   113,     0,     7,     0,
    19,     0,    25,     0,    33,     0,    28,     0,    29,     0,
    27,     0,    30,     0,    31,     0,    32,     0,    26,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    34,    36,    38,    42,    43,    47,    48,    52,    56,    63,
    69,    70,    73,    74,    78,    79,    83,    85,    89,    90,
    94,    97,   100,   103,   108,   109,   112,   113,   116,   117,
   121,   123,   125,   129,   130,   134,   135,   139,   141,   145,
   147,   151,   152,   158,   163,   172,   177,   190,   196,   202,
   203,   204,   205,   207,   217,   219,   221,   232,   245,   247,
   261,   266,   271,   273,   275,   279,   290,   295,   305,   312,
   313,   319,   322,   323,   326,   327,   331,   332,   336,   340,
   341,   344,   346,   350,   351,   356,   357,   358,   359,   360,
   361,   362,   363,   364,   365,   368,   371,   372,   373,   374,
   375,   376,   377,   378,   379,   380,   383,   384,   385,   386,
   387,   388,   389,   390,   391,   392,   395,   398,   403,   404,
   408,   409,   413,   417,   418,   422,   426,   430,   436,   437,
   440,   441,   445,   447,   451,   452,   456,   458,   464,   466,
   470,   472,   476,   477,   480,   481,   485,   486,   488,   490,
   494,   495,   504,   505,   514,   517,   518,   524,   532,   539,
   548,   551,   554,   557,   560,   563,   566,   569,   572,   575,
   576,   602,   605,   608,   609,   610,   611,   612,   613,   614,
   615,   618
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
"primitive","retval","funretval","blocklist","msgsend","expr","exprn","exprseq",
"arglistc","arglist1","keyarglist1","keyarg","optkeyarglist","mavars","mavarlist",
"slotliteral","blockliteral","pushliteral","listliteral","block","funcvardecls",
"funcvardecls1","funcvardecl","argdecls","slotdeflist0","slotdeflist","slotdef",
"rwslotdeflist","rwslotdef","listlit","listlit2","literallistc","literallist1",
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
    67,    68,    68,    69,    69,    69,    69,    69,    69,    70,
    70,    70,    70,    70,    70,    70,    70,    70,    70,    70,
    70,    70,    70,    70,    70,    70,    70,    70,    70,    71,
    71,    72,    73,    73,    74,    74,    75,    75,    76,    77,
    77,    78,    78,    79,    79,    80,    80,    80,    80,    80,
    80,    80,    80,    80,    80,    81,    82,    82,    82,    82,
    82,    82,    82,    82,    82,    82,    83,    83,    83,    83,
    83,    83,    83,    83,    83,    83,    84,    84,    85,    85,
    86,    86,    87,    88,    88,    88,    88,    88,    89,    89,
    90,    90,    91,    91,    92,    92,    93,    93,    94,    94,
    95,    95,    96,    96,    97,    97,    98,    98,    98,    98,
    99,    99,   100,   100,   101,   102,   102,   102,   102,   102,
   103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
   112,   113,   114,   115,   115,   115,   115,   115,   115,   115,
   115,   116
};

static const short yyr2[] = {     0,
     1,     1,     2,     0,     2,     1,     2,     6,     9,     5,
     0,     1,     0,     2,     0,     2,     3,     3,     0,     2,
     7,     8,     7,     8,     0,     1,     0,     1,     1,     2,
     4,     2,     1,     1,     2,     0,     2,     0,     3,     0,
     3,     0,     2,     6,     6,     7,     7,     8,     4,     1,
     1,     1,     1,     2,     1,     1,     4,     5,     3,     2,
     3,     3,     3,     4,     3,     4,     5,     7,     4,     1,
     3,     2,     0,     2,     1,     3,     1,     3,     2,     0,
     2,     1,     3,     1,     3,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     5,     6,     0,     2,
     1,     2,     3,     0,     3,     5,     3,     5,     0,     1,
     1,     3,     1,     3,     1,     3,     2,     4,     4,     5,
     3,     4,     0,     2,     1,     3,     0,     1,     1,     1,
     1,     2,     1,     2,     1,     1,     2,     2,     1,     2,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1
};

static const short yydefact[] = {     4,
    40,     0,     1,     2,     6,   161,   151,   153,   169,   172,
   168,   162,     0,   166,   164,   165,   167,   173,   155,     0,
     0,   124,    73,     0,     0,     0,     0,    33,     3,    29,
    52,    70,    25,    40,    51,    50,    96,    40,   121,   106,
    97,   156,   159,    98,    53,    56,   102,   103,   104,   105,
    99,   101,   100,   170,    55,     0,     5,    13,     7,     0,
   131,   133,   152,   154,   160,     0,    60,   129,   129,   119,
    75,     0,    27,     0,    40,    25,    54,   124,   143,     0,
    82,    84,     0,   174,   182,   178,   176,   177,   179,   180,
   181,   175,     0,     0,     0,    26,    72,    30,    32,   122,
   158,   157,     0,     0,    73,     0,   171,    19,    11,     0,
     0,   123,     0,     0,     0,   130,     0,   130,    40,    63,
    28,    74,    59,     0,    26,    41,     0,   119,   143,   145,
   116,     0,    27,   107,   108,     0,   112,   113,   114,   115,
   109,   111,   110,     0,     0,     0,   143,     0,    42,    61,
    62,    71,    65,    80,     0,    80,     0,    77,     0,     0,
     0,    12,    14,    15,   132,     0,   134,    95,    86,    87,
    91,    92,    93,    94,    88,    90,    89,     0,   125,     0,
   127,     0,   120,    76,    31,    66,    40,     0,   139,    28,
   144,   143,    69,    83,    85,     0,    80,     0,     0,     0,
    49,     0,     0,    64,     0,     0,    57,    79,   179,    10,
    20,     0,     0,    13,    19,     0,     0,   117,     0,   141,
   146,     0,   140,     0,    42,    67,    80,     0,     0,    43,
    81,    42,    58,    78,     0,     0,   124,   124,     0,   147,
   147,    16,     0,   126,   128,   118,   142,    42,    45,     0,
    42,     0,    44,   124,   124,   119,   119,    15,   148,   150,
   149,     0,   135,     0,     0,     8,    47,    42,    46,    68,
   119,   119,    36,    36,    19,    18,   147,   137,    17,    48,
    36,    36,   163,    38,    25,    38,     0,   136,     0,    38,
    38,     0,     0,    34,    38,    37,     0,     9,   138,     0,
     0,    25,    21,    35,    23,    22,    24,    39,     0,     0,
     0
};

static const short yydefgoto[] = {   309,
     3,     4,    57,     5,   161,   111,   215,   242,   160,   211,
    97,   122,    28,    29,   293,   284,   294,    30,   201,    31,
    32,    33,    71,    72,    73,   157,   158,   203,    80,    81,
   167,    35,    36,   130,    37,   119,    38,   183,    70,   115,
    60,    61,   262,   263,    40,   131,   132,   133,   264,    41,
    42,    43,    44,    45,    46,   285,    47,    48,    49,    50,
    51,    52,    53,    54,    55,    94,    95
};

static const short yypact[] = {    32,
   289,    14,    14,    72,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    30,-32768,-32768,-32768,-32768,-32768,-32768,    91,
   519,    19,   519,   381,   519,    30,   166,-32768,-32768,-32768,
-32768,   650,    69,   100,-32768,-32768,-32768,   335,-32768,-32768,
   136,   136,-32768,-32768,    46,    61,-32768,-32768,-32768,-32768,
-32768,-32768,   153,-32768,-32768,   147,-32768,     1,-32768,   131,
-32768,   164,-32768,-32768,-32768,   519,-32768,    30,    30,-32768,
-32768,   159,   176,   157,   335,   632,   199,    19,   564,   206,
    -8,-32768,   193,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    22,   519,   519,   519,-32768,-32768,-32768,-32768,
-32768,-32768,   519,   519,   519,   427,-32768,-32768,    30,    14,
   197,-32768,    30,   238,   218,   167,   219,   -16,   335,-32768,
   519,-32768,-32768,   195,-32768,-32768,   519,-32768,   564,-32768,
-32768,   201,   207,   136,-32768,   210,-32768,-32768,-32768,-32768,
-32768,-32768,   153,   519,    30,    30,   564,   427,   123,   217,
   217,   650,   650,   214,   220,   214,    59,-32768,   519,   580,
   221,-32768,-32768,-32768,-32768,    -5,-32768,-32768,   136,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   153,    30,-32768,    30,
-32768,   224,-32768,-32768,-32768,   650,   335,   223,-32768,   564,
-32768,   564,   650,-32768,-32768,   227,   214,   120,   519,   427,
    -7,   427,   225,-32768,   226,   242,-32768,-32768,   623,-32768,
-32768,   235,   239,   236,   181,   222,   246,-32768,   241,-32768,
-32768,   240,-32768,   237,-32768,   650,   214,   155,   244,-32768,
   245,   263,-32768,-32768,   254,   261,    19,    19,   264,   178,
   178,-32768,   592,-32768,-32768,-32768,-32768,-32768,    -7,   257,
-32768,   519,    -7,    19,    19,-32768,-32768,-32768,-32768,-32768,
-32768,   175,-32768,    30,   183,-32768,    -7,-32768,    -7,   650,
-32768,-32768,   204,   204,   181,-32768,   178,   285,-32768,    -7,
   204,   204,-32768,   473,   269,   473,   611,-32768,   238,   473,
   473,   519,   274,-32768,   267,-32768,   276,-32768,-32768,   277,
   279,   632,-32768,-32768,-32768,-32768,-32768,-32768,   318,   319,
-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,   316,-32768,   107,    64,-32768,  -186,-32768,
   -73,   190,    -6,-32768,  -136,   -91,    35,   291,  -106,-32768,
   -19,-32768,    -1,   229,   -57,  -120,   125,  -135,-32768,-32768,
    43,   -45,-32768,   154,-32768,  -119,   323,    12,   -59,   280,
   168,   247,   114,    71,  -107,-32768,  -121,-32768,-32768,   -75,
-32768,     2,   -69,   -12,   137,-32768,   -68,   -62,   -61,   -41,
   -31,   -20,   -15,   -33,-32768,  -102,   -13
};


#define	YYLAST		684


static const short yytable[] = {    34,
    62,    67,   126,   134,    12,    76,   168,   188,   187,   135,
   137,   145,    39,    77,    82,   181,   138,   139,   128,   107,
   205,    65,    74,    12,     6,   196,   113,   198,   243,    22,
    68,    99,     6,    79,   146,    39,    34,   140,   169,   109,
   229,   110,   101,   102,   170,   171,   154,   141,   156,   100,
    69,   172,   173,   134,     1,    62,    62,   213,   142,   135,
   137,   224,     2,   143,    74,   148,   138,   139,   124,   103,
   222,   134,   174,    34,   150,   151,   152,   135,   137,   228,
   149,   231,   175,   153,   138,   139,   100,   140,   287,   104,
   197,   250,   159,   176,    63,    64,   162,   141,   177,   105,
    62,   206,     2,   207,   106,   140,   236,   186,   142,   107,
    96,    19,   182,   143,   134,   141,   134,    34,   249,   184,
   135,   137,   135,   137,   193,   253,   142,   138,   139,   138,
   139,   143,   194,   195,   159,   101,   273,   274,    56,    58,
   213,   267,   227,   107,   269,    25,   199,   212,   140,   297,
   140,   281,   282,   300,   301,   230,    19,   208,   141,    10,
   141,   280,   206,    83,   225,   216,   200,   217,     6,   142,
   101,   142,   112,   113,   143,    12,   143,   256,   257,   226,
   219,   168,   286,   108,   213,    34,   159,   114,   159,   290,
   291,   240,   159,   241,   271,   272,   235,   206,   120,   251,
   184,   123,    78,   230,    79,   259,   260,   230,   179,   113,
   261,   296,   283,   169,    13,   136,   276,   277,   121,   170,
   171,   230,   127,   230,   279,   277,   172,   173,   308,   144,
   212,   147,   270,   164,   230,   116,   118,   178,   180,   185,
   189,     7,     8,     9,    10,    11,   163,   174,   192,   190,
    93,   278,    14,    15,    16,    17,   202,   175,    19,   204,
   214,   218,   220,   244,    20,   136,   223,    85,   176,   232,
   233,   237,   302,   177,   212,   238,   110,   245,   246,   247,
    78,   248,   295,   136,   295,   166,   252,   206,   295,   295,
   254,     6,     7,     8,     9,    10,    11,   255,    12,    13,
   258,   268,    83,    14,    15,    16,    17,    18,   289,    19,
   125,   303,   292,   305,   306,    20,   307,   310,   311,    59,
   239,   275,   191,    21,    98,    22,   136,    23,   136,   304,
   234,   299,    24,   155,    25,    26,    27,     6,     7,     8,
     9,    10,    11,   221,    12,    13,    75,   288,   117,    14,
    15,    16,    17,    18,   265,    19,     0,     0,     0,   165,
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
     0,    23,     0,     0,     0,     0,    66,     0,   292,    26,
    27,     6,     7,     8,     9,    10,    11,     0,    12,     0,
     0,     0,     0,    14,    15,    16,    17,    18,     0,    19,
     0,     0,     0,     0,     0,    20,     0,     0,     0,     0,
     0,     0,     0,    21,     0,    22,     0,    23,     0,     0,
     0,     0,    66,     0,     0,    26,    27,     7,     8,     9,
    10,    11,     0,    12,     0,     0,     0,     0,    14,    15,
    16,    17,     6,     0,    19,     0,     0,     0,     0,     0,
    20,     0,     0,     0,     6,     0,     0,     0,     0,     0,
     0,     0,   129,     0,    84,     0,    86,    87,    88,   209,
    90,    91,    92,     6,     0,     0,    84,   210,    86,    87,
    88,   209,    90,    91,    92,     6,     0,     0,     0,   266,
     0,     0,     0,     0,     0,    84,     0,    86,    87,    88,
   209,    90,    91,    92,     0,     0,     0,    84,   298,    86,
    87,    88,    89,    90,    91,    92,    84,    85,    86,    87,
    88,    89,    90,    91,    92,    93,     0,     0,     0,     0,
     0,     0,     0,   125,    84,    85,    86,    87,    88,    89,
    90,    91,    92,    93
};

static const short yycheck[] = {     1,
    13,    21,    76,    79,    10,    25,   114,   129,   128,    79,
    79,    20,     1,    26,    27,    32,    79,    79,    78,    53,
   156,    20,    24,    10,     3,   147,    43,   148,   215,    37,
    12,    38,     3,    39,    43,    24,    38,    79,   114,    39,
    48,    41,    41,    42,   114,   114,   104,    79,   106,    38,
    32,   114,   114,   129,    23,    68,    69,   160,    79,   129,
   129,   197,    31,    79,    66,    44,   129,   129,    75,    24,
   192,   147,   114,    75,    94,    95,    96,   147,   147,   200,
    93,   202,   114,   103,   147,   147,    75,   129,   275,    44,
   148,   227,   106,   114,     4,     5,   109,   129,   114,    39,
   113,    43,    31,    45,    44,   147,   209,   127,   129,   143,
    42,    21,   119,   129,   190,   147,   192,   119,   225,   121,
   190,   190,   192,   192,   144,   232,   147,   190,   190,   192,
   192,   147,   145,   146,   148,   134,   256,   257,     2,     3,
   243,   248,   200,   177,   251,    46,    24,   160,   190,   286,
   192,   271,   272,   290,   291,   201,    21,   159,   190,     7,
   192,   268,    43,    27,    45,   178,    44,   180,     3,   190,
   169,   192,    42,    43,   190,    10,   192,   237,   238,   199,
   187,   289,   274,    37,   287,   187,   200,    24,   202,   281,
   282,    11,   206,    13,   254,   255,   209,    43,    40,    45,
   202,    45,    37,   249,    39,    28,    29,   253,    42,    43,
    33,   285,     9,   289,    11,    79,    42,    43,    43,   289,
   289,   267,    24,   269,    42,    43,   289,   289,   302,    24,
   243,    39,   252,    37,   280,    68,    69,    20,    20,    45,
    40,     4,     5,     6,     7,     8,   110,   289,    39,    43,
    34,   264,    15,    16,    17,    18,    43,   289,    21,    40,
    40,    38,    40,    42,    27,   129,    40,    26,   289,    45,
    45,    37,   292,   289,   287,    37,    41,    32,    38,    40,
    37,    45,   284,   147,   286,    48,    24,    43,   290,   291,
    37,     3,     4,     5,     6,     7,     8,    37,    10,    11,
    37,    45,   166,    15,    16,    17,    18,    19,    24,    21,
    42,    38,    46,    38,    38,    27,    38,     0,     0,     4,
   214,   258,   133,    35,    34,    37,   190,    39,   192,   295,
   206,   289,    44,   105,    46,    47,    48,     3,     4,     5,
     6,     7,     8,   190,    10,    11,    24,   277,    69,    15,
    16,    17,    18,    19,   241,    21,    -1,    -1,    -1,   113,
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
    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    35,    -1,    37,    -1,    39,    -1,    -1,
    -1,    -1,    44,    -1,    -1,    47,    48,     4,     5,     6,
     7,     8,    -1,    10,    -1,    -1,    -1,    -1,    15,    16,
    17,    18,     3,    -1,    21,    -1,    -1,    -1,    -1,    -1,
    27,    -1,    -1,    -1,     3,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    39,    -1,    25,    -1,    27,    28,    29,    30,
    31,    32,    33,     3,    -1,    -1,    25,    38,    27,    28,
    29,    30,    31,    32,    33,     3,    -1,    -1,    -1,    38,
    -1,    -1,    -1,    -1,    -1,    25,    -1,    27,    28,    29,
    30,    31,    32,    33,    -1,    -1,    -1,    25,    38,    27,
    28,    29,    30,    31,    32,    33,    25,    26,    27,    28,
    29,    30,    31,    32,    33,    34,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    42,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34
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
case 42:
#line 151 "lang11d"
{ yyval = 0; ;
    break;}
case 43:
#line 153 "lang11d"
{ 
					yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); 
				;
    break;}
case 44:
#line 159 "lang11d"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-3], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 45:
#line 164 "lang11d"
{ 
				PyrSlotNode *selectornode;
				PyrSlot slot;
				
				SetSymbol(&slot, s_value);
				selectornode = newPyrSlotNode(&slot);
				yyval = (long)newPyrCallNode(selectornode, (PyrParseNode*)yyvsp[-5], (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 46:
#line 173 "lang11d"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-4], (PyrParseNode*)yyvsp[-6], 
					(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 47:
#line 178 "lang11d"
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
case 48:
#line 191 "lang11d"
{ 
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-7], (PyrParseNode*)yyvsp[-3]);
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-5], args, (PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 49:
#line 197 "lang11d"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[-3], 0, (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 53:
#line 206 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 54:
#line 208 "lang11d"
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
case 55:
#line 218 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 56:
#line 220 "lang11d"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 57:
#line 222 "lang11d"
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
case 58:
#line 233 "lang11d"
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
case 59:
#line 246 "lang11d"
{ yyval = yyvsp[-1]; ;
    break;}
case 60:
#line 248 "lang11d"
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
case 61:
#line 262 "lang11d"
{ 
				yyval = (long)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-1], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 62:
#line 267 "lang11d"
{ 
				yyval = (long)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-1], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 63:
#line 272 "lang11d"
{ yyval = (long)newPyrDynListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 64:
#line 274 "lang11d"
{ yyval = (long)newPyrDynListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 65:
#line 276 "lang11d"
{ 
				yyval = (long)newPyrAssignNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); 
			;
    break;}
case 66:
#line 280 "lang11d"
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
case 67:
#line 291 "lang11d"
{ 
				yyval = (long)newPyrSetterNode((PyrSlotNode*)yyvsp[-2], 
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 68:
#line 296 "lang11d"
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
case 69:
#line 306 "lang11d"
{ 
				yyval = (long)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[0], 0); 
			;
    break;}
case 71:
#line 314 "lang11d"
{ 
				yyval = (long)newPyrDropNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 73:
#line 322 "lang11d"
{ yyval = 0; ;
    break;}
case 76:
#line 328 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 78:
#line 333 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 79:
#line 337 "lang11d"
{ yyval = (long)newPyrPushKeyArgNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 80:
#line 340 "lang11d"
{ yyval = 0; ;
    break;}
case 81:
#line 341 "lang11d"
{ yyval = yyvsp[0]; ;
    break;}
case 82:
#line 345 "lang11d"
{ yyval = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 83:
#line 347 "lang11d"
{ yyval = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); ;
    break;}
case 85:
#line 352 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 86:
#line 356 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 87:
#line 357 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 88:
#line 358 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 89:
#line 359 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 90:
#line 360 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 91:
#line 361 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 92:
#line 362 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 93:
#line 363 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 94:
#line 364 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 95:
#line 365 "lang11d"
{ yyval = (long)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 96:
#line 368 "lang11d"
{ yyval = (long)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 97:
#line 371 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 98:
#line 372 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 99:
#line 373 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 100:
#line 374 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 101:
#line 375 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 102:
#line 376 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 103:
#line 377 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 104:
#line 378 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 105:
#line 379 "lang11d"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 106:
#line 380 "lang11d"
{ yyval = (long)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 107:
#line 383 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 108:
#line 384 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 109:
#line 385 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 110:
#line 386 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 111:
#line 387 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 112:
#line 388 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 113:
#line 389 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 114:
#line 390 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 115:
#line 391 "lang11d"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 116:
#line 392 "lang11d"
{ yyval = (long)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 117:
#line 396 "lang11d"
{ yyval = (long)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[-1], false); ;
    break;}
case 118:
#line 399 "lang11d"
{ yyval = (long)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[-1], true); ;
    break;}
case 119:
#line 403 "lang11d"
{ yyval = 0; ;
    break;}
case 120:
#line 405 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 122:
#line 410 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 123:
#line 414 "lang11d"
{ yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); ;
    break;}
case 124:
#line 417 "lang11d"
{ yyval = 0; ;
    break;}
case 125:
#line 419 "lang11d"
{
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); 
				;
    break;}
case 126:
#line 423 "lang11d"
{ 
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]); 
				;
    break;}
case 127:
#line 427 "lang11d"
{
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); 
				;
    break;}
case 128:
#line 431 "lang11d"
{ 
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]); 
				;
    break;}
case 129:
#line 436 "lang11d"
{ yyval = 0; ;
    break;}
case 132:
#line 442 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 133:
#line 446 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); ;
    break;}
case 134:
#line 448 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], 0); ;
    break;}
case 136:
#line 453 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 137:
#line 457 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); ;
    break;}
case 138:
#line 459 "lang11d"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], yyvsp[-3]); ;
    break;}
case 139:
#line 465 "lang11d"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 140:
#line 467 "lang11d"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 141:
#line 471 "lang11d"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 142:
#line 473 "lang11d"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 143:
#line 476 "lang11d"
{ yyval = 0; ;
    break;}
case 146:
#line 482 "lang11d"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 147:
#line 485 "lang11d"
{ yyval = rwPrivate; ;
    break;}
case 148:
#line 487 "lang11d"
{ yyval = rwReadOnly; ;
    break;}
case 149:
#line 489 "lang11d"
{ yyval = rwReadWrite; ;
    break;}
case 150:
#line 491 "lang11d"
{ yyval = rwWriteOnly; ;
    break;}
case 151:
#line 494 "lang11d"
{ yyval = zzval; ;
    break;}
case 152:
#line 496 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.ui = -node->slot.ui;
				yyval = zzval;
			;
    break;}
case 153:
#line 504 "lang11d"
{ yyval = zzval; ;
    break;}
case 154:
#line 506 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -node->slot.uf;
				yyval = zzval;
			;
    break;}
case 155:
#line 514 "lang11d"
{ yyval = zzval; ;
    break;}
case 157:
#line 519 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)yyvsp[-1];
				node->slot.uf *= pi;
			;
    break;}
case 158:
#line 525 "lang11d"
{
				PyrSlotNode *node; 
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = node->slot.ui;
				node->slot.uf = ival * pi;
			;
    break;}
case 159:
#line 533 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = pi;
				yyval = zzval;
			;
    break;}
case 160:
#line 540 "lang11d"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -pi;
				yyval = zzval;
			;
    break;}
case 161:
#line 548 "lang11d"
{ yyval = zzval; ;
    break;}
case 162:
#line 551 "lang11d"
{ yyval = zzval; ;
    break;}
case 163:
#line 554 "lang11d"
{ yyval = zzval; ;
    break;}
case 164:
#line 557 "lang11d"
{ yyval = zzval; ;
    break;}
case 165:
#line 560 "lang11d"
{ yyval = zzval; ;
    break;}
case 166:
#line 563 "lang11d"
{ yyval = zzval; ;
    break;}
case 167:
#line 566 "lang11d"
{ yyval = zzval; ;
    break;}
case 168:
#line 569 "lang11d"
{ yyval = zzval; ;
    break;}
case 169:
#line 572 "lang11d"
{ yyval = zzval; ;
    break;}
case 171:
#line 577 "lang11d"
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
case 172:
#line 602 "lang11d"
{ yyval = zzval; ;
    break;}
case 173:
#line 605 "lang11d"
{ yyval = zzval; ;
    break;}
case 174:
#line 608 "lang11d"
{ yyval = zzval; ;
    break;}
case 175:
#line 609 "lang11d"
{ yyval = zzval; ;
    break;}
case 176:
#line 610 "lang11d"
{ yyval = zzval; ;
    break;}
case 177:
#line 611 "lang11d"
{ yyval = zzval; ;
    break;}
case 178:
#line 612 "lang11d"
{ yyval = zzval; ;
    break;}
case 179:
#line 613 "lang11d"
{ yyval = zzval; ;
    break;}
case 180:
#line 614 "lang11d"
{ yyval = zzval; ;
    break;}
case 181:
#line 615 "lang11d"
{ yyval = zzval; ;
    break;}
case 182:
#line 618 "lang11d"
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
#line 621 "lang11d"
