#pragma once

#include "sc_sema/symbols.hpp"
namespace sc::ir {

struct SymbolTable;

// Symbols the compiler refers to by name. They are made once, before compiling, so they aren't looked up in the symbol
// table, which locks, while compiling.
struct CommonSymbols {
    explicit CommonSymbols(SymbolTable& table);

    // Selectors of messages that aren't written, e.g., `a.()` is `a.value()`.
    SymSCSelector value;
    SymSCSelector new_;
    SymSCSelector at;
    // `a.perform(\foo)` is `a.foo`.
    SymSCSelector perform;

    // The slot types a class can be declared with, e.g., `Foo[float] {}`.
    SymSCIdentifier slot;
    SymSCIdentifier float_;
    SymSCIdentifier int_;
};

}
