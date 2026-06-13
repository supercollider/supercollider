#pragma once
#include "text_location.hpp"

using SourceCodeRange = sc::lex::SourceCodeRange;

// This macro defines the default rule for how to combine location types. It is used in the parser.
#define YYLLOC_DEFAULT(Current, Rhs, N)                                                                                \
    do                                                                                                                 \
        if ((N) == 0) {                                                                                                \
            (Current) = YYRHSLOC(Rhs, 0);                                                                              \
        } else {                                                                                                       \
            (Current) = sc::lex::SourceCodeRange::range(YYRHSLOC(Rhs, 1), YYRHSLOC(Rhs, N));                           \
        }                                                                                                              \
    while (0)
