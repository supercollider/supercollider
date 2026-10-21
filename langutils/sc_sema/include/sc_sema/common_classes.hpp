#pragma once

#include "sc_sema/symbols.hpp"
namespace sc::ir {

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
        const auto* ptr = reinterpret_cast<const SymSCClassName*>(this);
        for (size_t i { 0 }; i < (sizeof(CommonClasses) / sizeof(SymSCClassName)); ++i)
            f(ptr[i]);
    }
};

}