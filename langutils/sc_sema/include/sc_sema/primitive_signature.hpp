#pragma once
#include "sc_sema/symbols.hpp"
#include <cstdint>
#include <unordered_map>

namespace sc::ir {

// A primitive's arguments, from the VM's table, which a method calling it must match.
struct PrimitiveSignature {
    // Counting the receiver, as the VM does.
    std::uint8_t num_arguments;
    bool variadic_arguments;
    bool keyword_arguments;
};

using PrimitiveSignatures = std::unordered_map<SymSCPrimitive, PrimitiveSignature, std::hash<Symbol>>;

}
