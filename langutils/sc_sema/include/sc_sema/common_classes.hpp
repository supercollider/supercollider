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

    template <typename F> void for_each(F f) const {
        // We could cast this into a SymSCClassName*... ?
        f(root_object);
        f(default_inheritor);
        f(class_);
        f(string);
        f(true_);
        f(false_);
        f(nil);
        f(float_);
        f(int_);
        f(char_);
        f(symbol);
    }
};

}