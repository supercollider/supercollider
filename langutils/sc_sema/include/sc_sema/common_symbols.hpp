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
    // `a[i] = v` is `a.put(i, v)`.
    SymSCSelector put;
    // `a[1..3]` is `a.copySeries(1, nil, 3)`, and `a[1..3] = v` is `a.putSeries(1, nil, 3, v)`.
    SymSCSelector copy_series;
    SymSCSelector put_series;
    // `(1..5)` is `1.series(nil, 5)`.
    SymSCSelector series;
    // `Array[1, 2]` adds each element.
    SymSCSelector add;
    // `a.perform(\foo)` is `a.foo`.
    SymSCSelector perform;

    // Classes whose methods the compiler assumes the implementation of, see KnownMethods.
    SymSCClassName simple_number;
    SymSCClassName arrayed_collection;
    SymSCClassName dictionary;
    // Where perform is defined. This is the class named Object, not the default inheritor, which can change.
    SymSCClassName object;

    // `this`, `super`, and the pseudo variables, see SCSpecial. They can't be declared or assigned.
    SymSCIdentifier this_;
    SymSCIdentifier super_;
    SymSCIdentifier this_process;
    SymSCIdentifier this_thread;
    SymSCIdentifier this_method;
    SymSCIdentifier this_function_def;
    SymSCIdentifier this_function;
    [[nodiscard]] bool is_special(SymSCIdentifier name) const {
        return name == this_ || name == super_ || name == this_process || name == this_thread || name == this_method
            || name == this_function_def || name == this_function;
    }

    // The slot types a class can be declared with, e.g., `Foo[float] {}`.
    SymSCIdentifier slot;
    SymSCIdentifier float_;
    SymSCIdentifier int_;
};

}
