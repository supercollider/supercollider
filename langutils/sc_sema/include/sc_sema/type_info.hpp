#pragma once

#include "sc_sema/symbols.hpp"

namespace sc::ir {

enum struct TypeConfidence {
    /// When a type is known exactly, usually the result of a literal expression
    Absolute,
    /// When a type is deduced from the messages the object is sent.
    Probable,
};

struct TypeInfo {
    SymSCClassName class_name;
    TypeConfidence confidence; 
};

} // sc::ir
