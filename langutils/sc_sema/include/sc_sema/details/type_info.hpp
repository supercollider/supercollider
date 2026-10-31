#pragma once

#include "sc_sema/details/symbols.hpp"

namespace sc::ir {

enum struct TypeConfidence {
    // When a type is known exactly, usually the result of a literal expression
    Absolute,
    // When a type is deduced from the messages the object is sent.
    Probable,
};

struct TypeInfo {
    OptionalSymSCClassName class_name; // The class, empty if it is not known.
    // How sure we are of the class. If class name is empty this is not useful.
    // It still would have meaning, but would be how certain we are about not knowing the type, which is useless.
    TypeConfidence confidence;
};

} // sc::ir
