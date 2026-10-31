#pragma once

#include "sc_sema/details/symbols.hpp"
namespace sc::ir {

struct SymbolTable;

struct CommonClasses {
    SymSCClassName root_object;
    SymSCClassName default_inheritor;
    SymSCClassName class_;
    SymSCClassName string;
    SymSCClassName true_;
    SymSCClassName false_;
    SymSCClassName nil;
    SymSCClassName float_;
    SymSCClassName int_;
    SymSCClassName char_;
    SymSCClassName symbol;
    SymSCClassName array;
    SymSCClassName event;
    SymSCClassName ref;

    template <typename F> void for_each(F f) const {
        // Probably undefined behaviour, but should be okay?
        const auto* first = reinterpret_cast<const SymSCClassName*>(this);
        const auto* last = first + sizeof(CommonClasses) / sizeof(SymSCClassName);
        for (const auto* c = first; c != last; ++c)
            f(*c);
    }
};

// Adds the classes the compiler refers to by name to the symbol table.
[[nodiscard]] CommonClasses symbol_table_and_common_classes(SymbolTable& sym_table, SymSCClassName root_object);

}
