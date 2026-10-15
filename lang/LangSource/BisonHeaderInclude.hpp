#pragma once
#include "CompilerContext.hpp"
#include "text_location.hpp"

namespace {
using SourceCodeRange = sc::lex::SourceCodeRange;
}

enum struct ReadWriteAccessor {
    Private,
    Read,
    Write,
    ReadWrite,
};

[[nodiscard]] inline bool has_read(ReadWriteAccessor r) {
    return r == ReadWriteAccessor::Read || r == ReadWriteAccessor::ReadWrite;
}

[[nodiscard]] inline bool has_write(ReadWriteAccessor r) {
    return r == ReadWriteAccessor::Write || r == ReadWriteAccessor::ReadWrite;
}

struct EmptyYYVal {};

extern struct CompilerContext* bison_cxt;


// This macro defines the default rule for how to combine location types. It is used in the parser.
#define YYLLOC_DEFAULT(Current, Rhs, N)                                                                                \
    do                                                                                                                 \
        if ((N) == 0) {                                                                                                \
            (Current) = YYRHSLOC(Rhs, 0);                                                                              \
        } else {                                                                                                       \
            (Current) = sc::lex::SourceCodeRange::range(YYRHSLOC(Rhs, 1), YYRHSLOC(Rhs, N));                           \
        }                                                                                                              \
    while (0)
