
/*  A Bison parser, made from lang11cx
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
#define	RGB	269
#define	NILOBJ	270
#define	TRUEOBJ	271
#define	FALSEOBJ	272
#define	INFINITUMOBJ	273
#define	PSEUDOVAR	274
#define	ELLIPSIS	275
#define	PIE	276
#define	BADTOKEN	277
#define	INTERPRET	278
#define	BINOP	279
#define	KEYBINOP	280
#define	READWRITEVAR	281
#define	UMINUS	282

#line 14 "lang11cx"


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



#define	YYFINAL		302
#define	YYFLAG		-32768
#define	YYNTBASE	49

#define YYTRANSLATE(x) ((unsigned)(x) <= 282 ? yytranslate[x] : 115)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,    48,     2,     2,     2,     2,    44,
    45,    31,    32,    43,    28,    34,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    41,    42,    29,
    25,    30,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    39,     2,    40,    46,     2,    35,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    37,     2,    38,    47,     2,     2,     2,     2,
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
    17,    18,    19,    20,    21,    22,    23,    24,    26,    27,
    33,    36
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     7,     8,    11,    13,    16,    23,    33,
    39,    40,    42,    43,    46,    47,    50,    54,    58,    59,
    62,    70,    79,    87,    96,    97,    99,   100,   102,   104,
   107,   112,   115,   117,   119,   122,   123,   126,   127,   131,
   132,   136,   138,   140,   143,   145,   147,   152,   158,   162,
   165,   169,   173,   179,   186,   194,   198,   202,   207,   211,
   216,   222,   230,   235,   237,   241,   244,   245,   248,   250,
   254,   256,   260,   263,   264,   267,   269,   273,   275,   279,
   281,   283,   285,   287,   289,   291,   293,   295,   297,   299,
   301,   303,   305,   307,   309,   311,   313,   315,   317,   319,
   321,   323,   325,   327,   329,   331,   333,   335,   337,   339,
   341,   343,   345,   347,   356,   367,   373,   380,   381,   384,
   386,   389,   393,   394,   398,   404,   405,   407,   409,   413,
   415,   419,   421,   425,   428,   433,   438,   444,   448,   453,
   454,   457,   459,   463,   464,   466,   468,   470,   472,   475,
   477,   480,   482,   484,   487,   490,   492,   495,   497,   499,
   501,   503,   505,   507,   509,   511,   513,   515,   518,   520,
   522,   524,   526,   528,   530,   532,   534,   536
};

static const short yyrhs[] = {    50,
     0,    51,     0,    24,    63,     0,     0,    50,    52,     0,
    53,     0,    51,    53,     0,   102,    55,    37,    56,    58,
    38,     0,   102,    39,    54,    40,    55,    37,    56,    58,
    38,     0,    32,   102,    37,    58,    38,     0,     0,   101,
     0,     0,    41,   102,     0,     0,    56,    57,     0,    13,
    90,    42,     0,    11,    90,    42,     0,     0,    58,    59,
     0,   101,    37,    86,    83,    65,    64,    38,     0,    31,
   101,    37,    86,    83,    65,    64,    38,     0,   113,    37,
    86,    83,    65,    64,    38,     0,    31,   113,    37,    86,
    83,    65,    64,    38,     0,     0,    42,     0,     0,    43,
     0,    67,     0,    70,    67,     0,    44,    84,    62,    45,
     0,    84,    62,     0,    62,     0,    66,     0,    70,    66,
     0,     0,   103,    60,     0,     0,    46,    68,    60,     0,
     0,    46,    68,    60,     0,    79,     0,   101,     0,    47,
   101,     0,   112,     0,   102,     0,   102,    44,    73,    45,
     0,   102,    44,    72,    75,    45,     0,    44,    70,    45,
     0,    35,    68,     0,    68,   113,    68,     0,    68,   114,
    68,     0,   101,    44,    72,    75,    45,     0,    68,    34,
   101,    44,    73,    45,     0,    68,    34,   101,    44,    72,
    75,    45,     0,    68,    34,   101,     0,    39,    71,    40,
     0,   102,    39,    71,    40,     0,   101,    25,    68,     0,
    47,   101,    25,    68,     0,    68,    34,   101,    25,    68,
     0,   101,    44,    72,    75,    45,    25,    68,     0,    48,
    76,    25,    68,     0,    68,     0,    69,    42,    68,     0,
    69,    60,     0,     0,    72,    61,     0,    70,     0,    72,
    43,    70,     0,    74,     0,    73,    43,    74,     0,   114,
    70,     0,     0,    43,    73,     0,    77,     0,    77,    21,
   101,     0,   101,     0,    77,    43,   101,     0,    97,     0,
   100,     0,   108,     0,   110,     0,   109,     0,    81,     0,
   104,     0,   105,     0,   106,     0,   107,     0,    92,     0,
    97,     0,   100,     0,   108,     0,   110,     0,   109,     0,
    81,     0,   104,     0,   105,     0,   106,     0,   107,     0,
    82,     0,    92,     0,    97,     0,   100,     0,   108,     0,
   110,     0,   109,     0,    81,     0,   104,     0,   105,     0,
   106,     0,   107,     0,    93,     0,    15,    44,    97,    43,
    97,    43,    97,    45,     0,    15,    44,    97,    43,    97,
    43,    97,    43,    97,    45,     0,    37,    86,    83,    62,
    38,     0,    48,    37,    86,    83,    62,    38,     0,     0,
    83,    85,     0,    85,     0,    84,    85,     0,    11,    88,
    42,     0,     0,    12,    88,    42,     0,    12,    87,    21,
   101,    42,     0,     0,    88,     0,    89,     0,    88,    43,
    89,     0,   101,     0,   101,    25,    78,     0,    91,     0,
    90,    43,    91,     0,    96,   101,     0,    96,   101,    25,
    78,     0,    48,    39,    94,    40,     0,    48,   102,    39,
    94,    40,     0,    39,    94,    40,     0,   102,    39,    94,
    40,     0,     0,    95,    61,     0,    80,     0,    95,    43,
    80,     0,     0,    29,     0,    33,     0,    30,     0,     4,
     0,    28,     4,     0,     5,     0,    28,     5,     0,    22,
     0,    98,     0,    98,    99,     0,    97,    99,     0,    99,
     0,    28,    99,     0,     3,     0,    10,     0,     9,     0,
    17,     0,    18,     0,    16,     0,    19,     0,     8,     0,
     6,     0,   111,     0,   110,   111,     0,     7,     0,    20,
     0,    26,     0,    33,     0,    29,     0,    30,     0,    28,
     0,    31,     0,    32,     0,    27,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    31,    33,    35,    39,    40,    44,    45,    49,    53,    60,
    66,    67,    70,    71,    75,    76,    80,    82,    86,    87,
    91,    94,    97,   100,   105,   106,   109,   110,   113,   114,
   118,   120,   122,   126,   127,   131,   132,   136,   138,   142,
   144,   148,   149,   151,   161,   163,   165,   176,   189,   191,
   205,   210,   215,   220,   225,   231,   235,   237,   239,   243,
   254,   259,   269,   276,   277,   283,   286,   287,   290,   291,
   295,   296,   300,   304,   305,   308,   310,   314,   315,   320,
   321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
   333,   334,   335,   336,   337,   338,   339,   340,   341,   342,
   343,   344,   347,   348,   349,   350,   351,   352,   353,   354,
   355,   356,   357,   360,   362,   366,   369,   374,   375,   379,
   380,   384,   388,   389,   393,   399,   400,   403,   404,   408,
   410,   414,   415,   419,   421,   427,   429,   433,   435,   439,
   440,   443,   444,   448,   449,   451,   453,   457,   458,   467,
   468,   477,   480,   481,   487,   495,   502,   511,   514,   517,
   520,   523,   526,   529,   532,   535,   538,   539,   565,   568,
   571,   572,   573,   574,   575,   576,   577,   580
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","NAME","INTEGER",
"FLOAT","SYMBOL","STRING","ASCII","PRIMITIVENAME","CLASSNAME","VAR","ARG","CLASSVAR",
"CONST","RGB","NILOBJ","TRUEOBJ","FALSEOBJ","INFINITUMOBJ","PSEUDOVAR","ELLIPSIS",
"PIE","BADTOKEN","INTERPRET","'='","BINOP","KEYBINOP","'-'","'<'","'>'","'*'",
"'+'","READWRITEVAR","'.'","'`'","UMINUS","'{'","'}'","'['","']'","':'","';'",
"','","'('","')'","'^'","'~'","'#'","root","classes","classextensions","classdef",
"classextension","optname","superclass","classvardecls","classvardecl","methods",
"methoddef","optsemi","optcomma","funcbody","cmdlinecode","methbody","primitive",
"retval","funretval","expr","exprn","exprseq","arglistc","arglist1","keyarglist1",
"keyarg","optkeyarglist","mavars","mavarlist","slotliteral","pushliteral","listliteral",
"rgbcolor","block","funcvardecls","funcvardecls1","funcvardecl","argdecls","slotdeflist0",
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
    67,    68,    68,    68,    68,    68,    68,    68,    68,    68,
    68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
    68,    68,    68,    69,    69,    70,    71,    71,    72,    72,
    73,    73,    74,    75,    75,    76,    76,    77,    77,    78,
    78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
    79,    79,    79,    79,    79,    79,    79,    79,    79,    79,
    79,    79,    80,    80,    80,    80,    80,    80,    80,    80,
    80,    80,    80,    81,    81,    82,    82,    83,    83,    84,
    84,    85,    86,    86,    86,    87,    87,    88,    88,    89,
    89,    90,    90,    91,    91,    92,    92,    93,    93,    94,
    94,    95,    95,    96,    96,    96,    96,    97,    97,    98,
    98,    99,   100,   100,   100,   100,   100,   101,   102,   103,
   104,   105,   106,   107,   108,   109,   110,   110,   111,   112,
   113,   113,   113,   113,   113,   113,   113,   114
};

static const short yyr2[] = {     0,
     1,     1,     2,     0,     2,     1,     2,     6,     9,     5,
     0,     1,     0,     2,     0,     2,     3,     3,     0,     2,
     7,     8,     7,     8,     0,     1,     0,     1,     1,     2,
     4,     2,     1,     1,     2,     0,     2,     0,     3,     0,
     3,     1,     1,     2,     1,     1,     4,     5,     3,     2,
     3,     3,     5,     6,     7,     3,     3,     4,     3,     4,
     5,     7,     4,     1,     3,     2,     0,     2,     1,     3,
     1,     3,     2,     0,     2,     1,     3,     1,     3,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     8,    10,     5,     6,     0,     2,     1,
     2,     3,     0,     3,     5,     0,     1,     1,     3,     1,
     3,     1,     3,     2,     4,     4,     5,     3,     4,     0,
     2,     1,     3,     0,     1,     1,     1,     1,     2,     1,
     2,     1,     1,     2,     2,     1,     2,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1
};

static const short yydefact[] = {     4,
    40,     0,     1,     2,     6,   158,   148,   150,   166,   169,
   165,   159,     0,     0,   163,   161,   162,   164,   170,   152,
     0,     0,   123,    67,     0,     0,     0,     0,    33,     3,
    29,    64,    25,    40,    42,    96,   101,    40,   120,   102,
    91,   153,   156,    92,    43,    46,    97,    98,    99,   100,
    93,    95,    94,   167,    45,     0,     5,    13,     7,     0,
   128,   130,     0,   149,   151,   157,     0,    50,   126,   118,
    69,     0,    27,     0,    40,    25,    44,   123,   140,     0,
    76,    78,     0,   171,   178,   175,   173,   174,   176,   177,
   172,     0,     0,     0,    26,    66,    30,    32,   121,   155,
   154,     0,     0,    67,     0,   168,    19,    11,     0,     0,
   122,     0,     0,     0,     0,     0,   127,    40,    57,    28,
    68,    49,     0,    26,    41,     0,   118,   140,   142,   108,
   113,     0,    27,   103,   104,     0,   109,   110,   111,   112,
   105,   107,   106,     0,     0,     0,   140,    56,    51,    52,
    65,    59,    74,     0,    74,     0,    71,     0,     0,     0,
    12,    14,    15,   129,     0,   131,    85,    90,    80,    81,
    86,    87,    88,    89,    82,    84,    83,     0,     0,   124,
     0,   119,    70,    31,    60,    40,     0,   136,    28,   141,
   140,    63,    77,    79,     0,     0,     0,     0,     0,    58,
     0,     0,    47,    73,   176,    10,    20,     0,     0,    13,
    19,     0,     0,   116,     0,   138,   143,     0,   137,    61,
    74,     0,    75,    53,    48,    72,     0,     0,   123,   123,
     0,   144,   144,    16,     0,     0,   125,   117,   139,     0,
    54,     0,   123,   123,   118,   118,    15,   145,   147,   146,
     0,   132,     0,     0,     8,     0,    55,    62,   118,   118,
    36,    36,    19,    18,   144,   134,    17,     0,   114,    36,
    36,   160,    38,    25,    38,     0,   133,     0,     0,    38,
    38,     0,     0,    34,    38,    37,     0,     9,   135,   115,
     0,     0,    25,    21,    35,    23,    22,    24,    39,     0,
     0,     0
};

static const short yydefgoto[] = {   300,
     3,     4,    57,     5,   160,   110,   211,   234,   159,   207,
    96,   121,    29,    30,   283,   273,   284,    31,    32,    33,
    34,    72,    73,   156,   157,   199,    80,    81,   166,    35,
   129,    36,    37,   118,    38,   182,    70,   116,    60,    61,
   251,   252,    40,   131,   132,   133,   253,    41,    42,    43,
    44,    45,    46,   274,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    93,    94
};

static const short yypact[] = {    -1,
   263,     6,     6,    -5,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    27,    -4,-32768,-32768,-32768,-32768,-32768,-32768,
   115,   493,    35,   493,   355,   493,    27,   157,-32768,-32768,
-32768,   626,    12,    20,-32768,-32768,-32768,   309,-32768,-32768,
    81,    81,-32768,-32768,    -8,   -19,-32768,-32768,-32768,-32768,
-32768,-32768,    50,-32768,-32768,    72,-32768,    23,-32768,   134,
-32768,    93,     0,-32768,-32768,-32768,   493,-32768,    27,-32768,
-32768,    95,    97,   102,   309,   609,   131,    35,   557,   138,
    -2,-32768,   151,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    27,   493,   493,   493,-32768,-32768,-32768,-32768,-32768,
-32768,   493,   493,   493,   401,-32768,-32768,    27,     6,   158,
-32768,    27,   538,   197,   154,   185,   166,   309,-32768,   493,
-32768,-32768,   173,-32768,-32768,   493,-32768,   557,-32768,-32768,
-32768,   183,   194,    81,-32768,   199,-32768,-32768,-32768,-32768,
-32768,-32768,    50,   493,    27,    27,   557,     9,   205,   205,
   626,   626,   200,   204,   200,    83,-32768,   493,   574,   206,
-32768,-32768,-32768,-32768,    11,-32768,-32768,-32768,    81,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,    50,     0,    27,-32768,
   207,-32768,-32768,-32768,   626,   309,   215,-32768,   557,-32768,
   557,   626,-32768,-32768,   216,   493,   401,   401,   217,-32768,
   227,   233,-32768,-32768,   221,-32768,-32768,   238,   239,   236,
   118,   241,   244,-32768,   249,-32768,-32768,   248,-32768,   626,
   200,   105,   246,   265,-32768,-32768,   255,   256,    35,    35,
   257,   187,   187,-32768,   585,     0,-32768,-32768,-32768,   250,
-32768,   493,    35,    35,-32768,-32768,-32768,-32768,-32768,-32768,
   184,-32768,    27,   193,-32768,   141,-32768,   626,-32768,-32768,
   182,   182,   118,-32768,   187,   271,-32768,     0,-32768,   182,
   182,-32768,   447,   259,   447,   596,-32768,   538,   252,   447,
   447,   493,   266,-32768,   260,-32768,   267,-32768,-32768,-32768,
   270,   280,   609,-32768,-32768,-32768,-32768,-32768,-32768,   303,
   321,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,   318,-32768,   113,    85,-32768,  -173,-32768,
   -74,   201,    -6,-32768,   -77,   -57,    45,   299,   -21,-32768,
   -16,   231,   -81,    32,   136,  -137,-32768,-32768,    58,-32768,
   150,   -76,-32768,   -72,   315,    10,   -65,-32768,   272,   230,
   110,    80,  -106,-32768,   -89,-32768,-32768,   -53,-32768,    65,
   -67,   -13,    42,-32768,   -50,   -46,   -37,   -36,   -30,   -20,
    21,   -47,-32768,   -91,   -40
};


#define	YYLAST		660


static const short yytable[] = {    62,
    68,   125,   130,     7,    76,   106,   168,    71,    74,   115,
    39,   135,   127,    77,    82,    12,   102,   201,   145,   104,
    12,   153,     1,   155,   105,   134,     2,   114,   137,     6,
     2,    98,   138,   196,    39,   103,   167,   235,   187,    63,
   146,   139,   140,    56,    58,   170,    69,    99,   141,    79,
    74,   130,   197,    95,   186,    62,    10,   195,   142,   169,
   135,   108,   171,   109,   158,    26,   172,   209,   123,    83,
   130,   149,   150,   151,   134,   173,   174,   137,   148,   135,
   152,   138,   175,   240,    99,    66,    71,    71,    71,   276,
   139,   140,   176,   134,   161,   106,   137,   141,    62,   143,
   138,   218,    20,   183,   185,   100,   101,   142,   107,   139,
   140,   181,   130,   228,   130,   221,   141,   113,    64,    65,
   136,   135,   192,   135,   212,   202,   142,   203,   232,   106,
   233,   193,   194,   177,   119,   134,    20,   134,   137,   120,
   137,   204,   138,   209,   138,   208,   122,   202,   143,   241,
   162,   139,   140,   139,   140,   126,   158,   158,   141,     6,
   141,   158,   144,   245,   246,   213,    12,   143,   142,   136,
   142,   168,   261,   262,   220,   111,   112,   259,   260,   215,
    71,   183,   256,   268,   209,   269,   270,   271,   136,   147,
   272,   227,    13,    78,   163,    79,   178,   287,   100,   286,
    64,   167,   291,   292,   275,   179,    83,   180,   112,   143,
   170,   143,   280,   281,   279,   248,   249,   184,   299,   250,
   258,   208,   188,     6,   169,   264,   265,   171,   222,   223,
   136,   172,   136,   100,   267,   265,   189,   191,    92,   266,
   173,   174,   198,   200,   214,   210,    84,   175,    86,    87,
    88,    89,    90,    91,   216,   219,   285,   176,   285,    85,
   293,   224,   208,   285,   285,     6,     7,     8,     9,    10,
    11,   225,    12,    13,   229,   230,   109,    14,    15,    16,
    17,    18,    19,   236,    20,   237,   238,   239,   202,   242,
    21,   243,   244,   247,   257,   278,   290,    22,   177,    23,
   124,    24,   301,   294,   296,   282,    25,   297,    26,    27,
    28,     6,     7,     8,     9,    10,    11,   298,    12,    13,
   302,    59,   231,    14,    15,    16,    17,    18,    19,   295,
    20,   263,    97,   190,   154,   289,    21,   226,   217,    75,
   117,   164,   254,    22,   277,    23,     0,    24,     0,     0,
     0,     0,    67,     0,    26,    27,    28,     6,     7,     8,
     9,    10,    11,     0,    12,    13,     0,     0,     0,    14,
    15,    16,    17,    18,    19,     0,    20,     0,     0,     0,
     0,     0,    21,     0,     0,     0,     0,     0,     0,    22,
     0,    23,     0,    24,     0,     0,     0,     0,    67,     0,
     0,    27,    28,     6,     7,     8,     9,    10,    11,     0,
    12,     0,     0,     0,     0,    14,    15,    16,    17,    18,
    19,     0,    20,     0,     0,     0,     0,    85,    21,     0,
     0,     0,     0,     0,     0,    22,     0,    23,     0,    24,
     0,     0,     0,     0,    67,     0,     0,    27,    28,     6,
     7,     8,     9,    10,    11,     0,    12,     0,     0,     0,
     0,    14,    15,    16,    17,    18,    19,     0,    20,     0,
     0,     0,     0,     0,    21,     0,     0,     0,     0,     0,
     0,    22,     0,    23,     0,    24,     0,     0,     0,     0,
    67,     0,   282,    27,    28,     6,     7,     8,     9,    10,
    11,     0,    12,     0,     0,     0,     0,    14,    15,    16,
    17,    18,    19,     0,    20,     0,     0,     0,     0,     0,
    21,     0,     0,     0,     0,     0,     0,    22,     0,    23,
     0,    24,     0,     0,     0,     0,    67,     0,     0,    27,
    28,     7,     8,     9,    10,    11,     0,     0,     0,     0,
     0,     0,    14,    15,    16,    17,    18,     0,     0,    20,
     7,     8,     9,    10,    11,    21,    12,     0,     0,     0,
     0,    14,    15,    16,    17,    18,     6,     0,    20,     0,
     0,     0,     0,     0,    21,   165,     0,     6,     0,     0,
     0,     0,     0,     0,     0,   128,     0,     0,     6,    84,
     0,    86,    87,    88,   205,    90,    91,     0,     0,     0,
    84,   206,    86,    87,    88,   205,    90,    91,     0,     0,
     0,    84,   255,    86,    87,    88,   205,    90,    91,     0,
     0,     0,     0,   288,    84,    85,    86,    87,    88,    89,
    90,    91,    92,     0,     0,     0,     0,     0,     0,     0,
   124,    84,    85,    86,    87,    88,    89,    90,    91,    92
};

static const short yycheck[] = {    13,
    22,    76,    79,     4,    26,    53,   113,    24,    25,    63,
     1,    79,    78,    27,    28,    10,    25,   155,    21,    39,
    10,   103,    24,   105,    44,    79,    32,    28,    79,     3,
    32,    38,    79,    25,    25,    44,   113,   211,   128,    44,
    43,    79,    79,     2,     3,   113,    12,    38,    79,    39,
    67,   128,    44,    42,   127,    69,     7,   147,    79,   113,
   128,    39,   113,    41,   105,    46,   113,   159,    75,    28,
   147,    93,    94,    95,   128,   113,   113,   128,    92,   147,
   102,   128,   113,   221,    75,    21,   103,   104,   105,   263,
   128,   128,   113,   147,   108,   143,   147,   128,   112,    79,
   147,   191,    22,   120,   126,    41,    42,   128,    37,   147,
   147,   118,   189,   205,   191,   197,   147,    25,     4,     5,
    79,   189,   144,   191,   178,    43,   147,    45,    11,   177,
    13,   145,   146,   113,    40,   189,    22,   191,   189,    43,
   191,   158,   189,   235,   191,   159,    45,    43,   128,    45,
   109,   189,   189,   191,   191,    25,   197,   198,   189,     3,
   191,   202,    25,   229,   230,   179,    10,   147,   189,   128,
   191,   278,   245,   246,   196,    42,    43,   243,   244,   186,
   197,   198,   236,    43,   276,    45,   259,   260,   147,    39,
     9,   205,    11,    37,    37,    39,    43,   275,   134,   274,
     4,   278,   280,   281,   262,    21,   165,    42,    43,   189,
   278,   191,   270,   271,   268,    29,    30,    45,   293,    33,
   242,   235,    40,     3,   278,    42,    43,   278,   197,   198,
   189,   278,   191,   169,    42,    43,    43,    39,    34,   253,
   278,   278,    43,    40,    38,    40,    26,   278,    28,    29,
    30,    31,    32,    33,    40,    40,   273,   278,   275,    27,
   282,    45,   276,   280,   281,     3,     4,     5,     6,     7,
     8,    45,    10,    11,    37,    37,    41,    15,    16,    17,
    18,    19,    20,    43,    22,    42,    38,    40,    43,    25,
    28,    37,    37,    37,    45,    25,    45,    35,   278,    37,
    42,    39,     0,    38,    38,    46,    44,    38,    46,    47,
    48,     3,     4,     5,     6,     7,     8,    38,    10,    11,
     0,     4,   210,    15,    16,    17,    18,    19,    20,   285,
    22,   247,    34,   133,   104,   278,    28,   202,   189,    25,
    69,   112,   233,    35,   265,    37,    -1,    39,    -1,    -1,
    -1,    -1,    44,    -1,    46,    47,    48,     3,     4,     5,
     6,     7,     8,    -1,    10,    11,    -1,    -1,    -1,    15,
    16,    17,    18,    19,    20,    -1,    22,    -1,    -1,    -1,
    -1,    -1,    28,    -1,    -1,    -1,    -1,    -1,    -1,    35,
    -1,    37,    -1,    39,    -1,    -1,    -1,    -1,    44,    -1,
    -1,    47,    48,     3,     4,     5,     6,     7,     8,    -1,
    10,    -1,    -1,    -1,    -1,    15,    16,    17,    18,    19,
    20,    -1,    22,    -1,    -1,    -1,    -1,    27,    28,    -1,
    -1,    -1,    -1,    -1,    -1,    35,    -1,    37,    -1,    39,
    -1,    -1,    -1,    -1,    44,    -1,    -1,    47,    48,     3,
     4,     5,     6,     7,     8,    -1,    10,    -1,    -1,    -1,
    -1,    15,    16,    17,    18,    19,    20,    -1,    22,    -1,
    -1,    -1,    -1,    -1,    28,    -1,    -1,    -1,    -1,    -1,
    -1,    35,    -1,    37,    -1,    39,    -1,    -1,    -1,    -1,
    44,    -1,    46,    47,    48,     3,     4,     5,     6,     7,
     8,    -1,    10,    -1,    -1,    -1,    -1,    15,    16,    17,
    18,    19,    20,    -1,    22,    -1,    -1,    -1,    -1,    -1,
    28,    -1,    -1,    -1,    -1,    -1,    -1,    35,    -1,    37,
    -1,    39,    -1,    -1,    -1,    -1,    44,    -1,    -1,    47,
    48,     4,     5,     6,     7,     8,    -1,    -1,    -1,    -1,
    -1,    -1,    15,    16,    17,    18,    19,    -1,    -1,    22,
     4,     5,     6,     7,     8,    28,    10,    -1,    -1,    -1,
    -1,    15,    16,    17,    18,    19,     3,    -1,    22,    -1,
    -1,    -1,    -1,    -1,    28,    48,    -1,     3,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    39,    -1,    -1,     3,    26,
    -1,    28,    29,    30,    31,    32,    33,    -1,    -1,    -1,
    26,    38,    28,    29,    30,    31,    32,    33,    -1,    -1,
    -1,    26,    38,    28,    29,    30,    31,    32,    33,    -1,
    -1,    -1,    -1,    38,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    42,    26,    27,    28,    29,    30,    31,    32,    33,    34
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
#line 32 "lang11cx"
{ gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; ;
    break;}
case 2:
#line 34 "lang11cx"
{ gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 1; ;
    break;}
case 3:
#line 36 "lang11cx"
{ gRootParseNode = (PyrParseNode*)yyvsp[0]; gParserResult = 2; ;
    break;}
case 4:
#line 39 "lang11cx"
{ yyval = 0; ;
    break;}
case 5:
#line 41 "lang11cx"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 7:
#line 46 "lang11cx"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 8:
#line 50 "lang11cx"
{ yyval = (long)newPyrClassNode((PyrSlotNode*)yyvsp[-5], (PyrSlotNode*)yyvsp[-4], 
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1], 0); 
				;
    break;}
case 9:
#line 54 "lang11cx"
{ yyval = (long)newPyrClassNode((PyrSlotNode*)yyvsp[-8], (PyrSlotNode*)yyvsp[-4], 
					(PyrVarListNode*)yyvsp[-2], (PyrMethodNode*)yyvsp[-1], 
					(PyrSlotNode*)yyvsp[-6]); 
				;
    break;}
case 10:
#line 61 "lang11cx"
{ 
					yyval = (long)newPyrClassExtNode((PyrSlotNode*)yyvsp[-3], (PyrMethodNode*)yyvsp[-1]); 
				;
    break;}
case 11:
#line 66 "lang11cx"
{ yyval = 0; ;
    break;}
case 13:
#line 70 "lang11cx"
{ yyval = 0; ;
    break;}
case 14:
#line 72 "lang11cx"
{ yyval = yyvsp[0]; ;
    break;}
case 15:
#line 75 "lang11cx"
{ yyval = 0; ;
    break;}
case 16:
#line 77 "lang11cx"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 17:
#line 81 "lang11cx"
{ yyval = (long)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varClass); ;
    break;}
case 18:
#line 83 "lang11cx"
{ yyval = (long)newPyrVarListNode((PyrVarDefNode*)yyvsp[-1], varInst); ;
    break;}
case 19:
#line 86 "lang11cx"
{ yyval = 0; ;
    break;}
case 20:
#line 88 "lang11cx"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 21:
#line 92 "lang11cx"
{ yyval = (long)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2], 
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); ;
    break;}
case 22:
#line 95 "lang11cx"
{ yyval = (long)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2], 
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); ;
    break;}
case 23:
#line 98 "lang11cx"
{ yyval = (long)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2], 
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 0); ;
    break;}
case 24:
#line 101 "lang11cx"
{ yyval = (long)newPyrMethodNode((PyrSlotNode*)yyvsp[-6], (PyrSlotNode*)yyvsp[-2], 
					(PyrArgListNode*)yyvsp[-4], (PyrVarListNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1], 1); ;
    break;}
case 30:
#line 115 "lang11cx"
{ yyval = (long)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 31:
#line 119 "lang11cx"
{ yyval = (long)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-2], (PyrParseNode*)yyvsp[-1], false); ;
    break;}
case 32:
#line 121 "lang11cx"
{ yyval = (long)newPyrBlockNode(NULL, (PyrVarListNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0], false); ;
    break;}
case 33:
#line 123 "lang11cx"
{ yyval = (long)newPyrBlockNode(NULL, NULL, (PyrParseNode*)yyvsp[0], false); ;
    break;}
case 35:
#line 128 "lang11cx"
{ yyval = (long)newPyrDropNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 36:
#line 131 "lang11cx"
{ yyval = 0; ;
    break;}
case 37:
#line 133 "lang11cx"
{ yyval = yyvsp[-1]; ;
    break;}
case 38:
#line 137 "lang11cx"
{ yyval = (long)newPyrReturnNode(NULL); ;
    break;}
case 39:
#line 139 "lang11cx"
{ yyval = (long)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); ;
    break;}
case 40:
#line 143 "lang11cx"
{ yyval = (long)newPyrBlockReturnNode(); ;
    break;}
case 41:
#line 145 "lang11cx"
{ yyval = (long)newPyrReturnNode((PyrParseNode*)yyvsp[-1]); ;
    break;}
case 43:
#line 150 "lang11cx"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 44:
#line 152 "lang11cx"
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
#line 162 "lang11cx"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 46:
#line 164 "lang11cx"
{ yyval = (long)newPyrPushNameNode((PyrSlotNode*)yyvsp[0]); ;
    break;}
case 47:
#line 166 "lang11cx"
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
#line 177 "lang11cx"
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
#line 190 "lang11cx"
{ yyval = yyvsp[-1]; ;
    break;}
case 50:
#line 192 "lang11cx"
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
#line 206 "lang11cx"
{ 
				yyval = (long)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-1], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 52:
#line 211 "lang11cx"
{ 
				yyval = (long)newPyrBinopCallNode((PyrSlotNode*)yyvsp[-1], 
						(PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 53:
#line 216 "lang11cx"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-4], (PyrParseNode*)yyvsp[-2], 
						(PyrParseNode*)yyvsp[-1]); 
			;
    break;}
case 54:
#line 221 "lang11cx"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-5], 
					(PyrParseNode*)yyvsp[-1]); 
			;
    break;}
case 55:
#line 226 "lang11cx"
{ 
				PyrParseNode* args;
				args = linkNextNode((PyrParseNode*)yyvsp[-6], (PyrParseNode*)yyvsp[-2]);
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[-4], args, (PyrParseNode*)yyvsp[-1]); 
			;
    break;}
case 56:
#line 232 "lang11cx"
{ 
				yyval = (long)newPyrCallNode((PyrSlotNode*)yyvsp[0], (PyrParseNode*)yyvsp[-2], 0); 
			;
    break;}
case 57:
#line 236 "lang11cx"
{ yyval = (long)newPyrDynListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 58:
#line 238 "lang11cx"
{ yyval = (long)newPyrDynListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 59:
#line 240 "lang11cx"
{ 
				yyval = (long)newPyrAssignNode((PyrSlotNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0], 0); 
			;
    break;}
case 60:
#line 244 "lang11cx"
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
case 61:
#line 255 "lang11cx"
{ 
				yyval = (long)newPyrSetterNode((PyrSlotNode*)yyvsp[-2], 
						(PyrParseNode*)yyvsp[-4], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 62:
#line 260 "lang11cx"
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
case 63:
#line 270 "lang11cx"
{ 
				yyval = (long)newPyrMultiAssignNode((PyrMultiAssignVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[0], 0); 
			;
    break;}
case 65:
#line 278 "lang11cx"
{ 
				yyval = (long)newPyrDropNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); 
			;
    break;}
case 67:
#line 286 "lang11cx"
{ yyval = 0; ;
    break;}
case 70:
#line 292 "lang11cx"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 72:
#line 297 "lang11cx"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 73:
#line 301 "lang11cx"
{ yyval = (long)newPyrPushKeyArgNode((PyrSlotNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 74:
#line 304 "lang11cx"
{ yyval = 0; ;
    break;}
case 75:
#line 305 "lang11cx"
{ yyval = yyvsp[0]; ;
    break;}
case 76:
#line 309 "lang11cx"
{ yyval = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 77:
#line 311 "lang11cx"
{ yyval = (long)newPyrMultiAssignVarListNode((PyrSlotNode*)yyvsp[-2], (PyrSlotNode*)yyvsp[0]); ;
    break;}
case 79:
#line 316 "lang11cx"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 80:
#line 320 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 81:
#line 321 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 82:
#line 322 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 83:
#line 323 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 84:
#line 324 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 85:
#line 325 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 86:
#line 326 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 87:
#line 327 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 88:
#line 328 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 89:
#line 329 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 90:
#line 330 "lang11cx"
{ yyval = (long)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 91:
#line 333 "lang11cx"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 92:
#line 334 "lang11cx"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 93:
#line 335 "lang11cx"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 94:
#line 336 "lang11cx"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 95:
#line 337 "lang11cx"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 96:
#line 338 "lang11cx"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 97:
#line 339 "lang11cx"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 98:
#line 340 "lang11cx"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 99:
#line 341 "lang11cx"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 100:
#line 342 "lang11cx"
{ yyval = (long)newPyrPushLitNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 101:
#line 343 "lang11cx"
{ yyval = (long)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 102:
#line 344 "lang11cx"
{ yyval = (long)newPyrPushLitNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 103:
#line 347 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 104:
#line 348 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 105:
#line 349 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 106:
#line 350 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 107:
#line 351 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 108:
#line 352 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 109:
#line 353 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 110:
#line 354 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 111:
#line 355 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 112:
#line 356 "lang11cx"
{ yyval = (long)newPyrLiteralNode((PyrSlotNode*)yyvsp[0], NULL); ;
    break;}
case 113:
#line 357 "lang11cx"
{ yyval = (long)newPyrLiteralNode(NULL, (PyrParseNode*)yyvsp[0]); ;
    break;}
case 114:
#line 361 "lang11cx"
{ yyval = (long)newRGBColor(yyvsp[-5], yyvsp[-3], yyvsp[-1], 0); ;
    break;}
case 115:
#line 363 "lang11cx"
{ yyval = (long)newRGBColor(yyvsp[-7], yyvsp[-5], yyvsp[-3], yyvsp[-1]); ;
    break;}
case 116:
#line 367 "lang11cx"
{ yyval = (long)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[-1], false); ;
    break;}
case 117:
#line 370 "lang11cx"
{ yyval = (long)newPyrBlockNode((PyrArgListNode*)yyvsp[-3], (PyrVarListNode*)yyvsp[-2], 
					(PyrParseNode*)yyvsp[-1], true); ;
    break;}
case 118:
#line 374 "lang11cx"
{ yyval = 0; ;
    break;}
case 119:
#line 376 "lang11cx"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 121:
#line 381 "lang11cx"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-1], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 122:
#line 385 "lang11cx"
{ yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); ;
    break;}
case 123:
#line 388 "lang11cx"
{ yyval = 0; ;
    break;}
case 124:
#line 390 "lang11cx"
{
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-1], NULL); 
				;
    break;}
case 125:
#line 394 "lang11cx"
{ 
					yyval = (long)newPyrArgListNode((PyrVarDefNode*)yyvsp[-3], (PyrSlotNode*)yyvsp[-1]); 
				;
    break;}
case 126:
#line 399 "lang11cx"
{ yyval = 0; ;
    break;}
case 129:
#line 405 "lang11cx"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 130:
#line 409 "lang11cx"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, 0); ;
    break;}
case 131:
#line 411 "lang11cx"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], 0); ;
    break;}
case 133:
#line 416 "lang11cx"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 134:
#line 420 "lang11cx"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[0], NULL, yyvsp[-1]); ;
    break;}
case 135:
#line 422 "lang11cx"
{ yyval = (long)newPyrVarDefNode((PyrSlotNode*)yyvsp[-2], (PyrLiteralNode*)yyvsp[0], yyvsp[-3]); ;
    break;}
case 136:
#line 428 "lang11cx"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 137:
#line 430 "lang11cx"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 138:
#line 434 "lang11cx"
{ yyval = (long)newPyrLitListNode(0, (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 139:
#line 436 "lang11cx"
{ yyval = (long)newPyrLitListNode((PyrParseNode*)yyvsp[-3], (PyrParseNode*)yyvsp[-1]); ;
    break;}
case 140:
#line 439 "lang11cx"
{ yyval = 0; ;
    break;}
case 143:
#line 445 "lang11cx"
{ yyval = (long)linkNextNode((PyrParseNode*)yyvsp[-2], (PyrParseNode*)yyvsp[0]); ;
    break;}
case 144:
#line 448 "lang11cx"
{ yyval = rwPrivate; ;
    break;}
case 145:
#line 450 "lang11cx"
{ yyval = rwReadOnly; ;
    break;}
case 146:
#line 452 "lang11cx"
{ yyval = rwReadWrite; ;
    break;}
case 147:
#line 454 "lang11cx"
{ yyval = rwWriteOnly; ;
    break;}
case 148:
#line 457 "lang11cx"
{ yyval = zzval; ;
    break;}
case 149:
#line 459 "lang11cx"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.ui = -node->slot.ui;
				yyval = zzval;
			;
    break;}
case 150:
#line 467 "lang11cx"
{ yyval = zzval; ;
    break;}
case 151:
#line 469 "lang11cx"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -node->slot.uf;
				yyval = zzval;
			;
    break;}
case 152:
#line 477 "lang11cx"
{ yyval = zzval; ;
    break;}
case 154:
#line 482 "lang11cx"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)yyvsp[-1];
				node->slot.uf *= pi;
			;
    break;}
case 155:
#line 488 "lang11cx"
{
				PyrSlotNode *node; 
				double ival;
				node = (PyrSlotNode*)yyvsp[-1];
				ival = node->slot.ui;
				node->slot.uf = ival * pi;
			;
    break;}
case 156:
#line 496 "lang11cx"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = pi;
				yyval = zzval;
			;
    break;}
case 157:
#line 503 "lang11cx"
{
				PyrSlotNode *node; 
				node = (PyrSlotNode*)zzval; 
				node->slot.uf = -pi;
				yyval = zzval;
			;
    break;}
case 158:
#line 511 "lang11cx"
{ yyval = zzval; ;
    break;}
case 159:
#line 514 "lang11cx"
{ yyval = zzval; ;
    break;}
case 160:
#line 517 "lang11cx"
{ yyval = zzval; ;
    break;}
case 161:
#line 520 "lang11cx"
{ yyval = zzval; ;
    break;}
case 162:
#line 523 "lang11cx"
{ yyval = zzval; ;
    break;}
case 163:
#line 526 "lang11cx"
{ yyval = zzval; ;
    break;}
case 164:
#line 529 "lang11cx"
{ yyval = zzval; ;
    break;}
case 165:
#line 532 "lang11cx"
{ yyval = zzval; ;
    break;}
case 166:
#line 535 "lang11cx"
{ yyval = zzval; ;
    break;}
case 168:
#line 540 "lang11cx"
{
					PyrSlotNode	*node1, *node2, *node3;
					PyrSlot slot;
					PyrString *string;
					char *str;
					long len1, len2, len3;
					
					node1 = (PyrSlotNode*)yyvsp[-1];
					node2 = (PyrSlotNode*)yyvsp[0];
					len1 = node1->slot.uo->indexedSize;
					len2 = node2->slot.uo->indexedSize;
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
case 169:
#line 565 "lang11cx"
{ yyval = zzval; ;
    break;}
case 170:
#line 568 "lang11cx"
{ yyval = zzval; ;
    break;}
case 171:
#line 571 "lang11cx"
{ yyval = zzval; ;
    break;}
case 172:
#line 572 "lang11cx"
{ yyval = zzval; ;
    break;}
case 173:
#line 573 "lang11cx"
{ yyval = zzval; ;
    break;}
case 174:
#line 574 "lang11cx"
{ yyval = zzval; ;
    break;}
case 175:
#line 575 "lang11cx"
{ yyval = zzval; ;
    break;}
case 176:
#line 576 "lang11cx"
{ yyval = zzval; ;
    break;}
case 177:
#line 577 "lang11cx"
{ yyval = zzval; ;
    break;}
case 178:
#line 580 "lang11cx"
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
#line 583 "lang11cx"
