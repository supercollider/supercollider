#pragma once
#include "sc_sema/symbols.hpp"
#include <vector>
#include <optional>

namespace sc::ir {

struct IntrinsicConstraint {
    SymSCClassName target;
    std::optional<OptionalSymSCClassName> super_class;
    // These are ordered.
    std::optional<std::vector<SymSCIdentifier>> member_names;
    std::optional<SymSCIdentifier> slot_def;
};

using IntrinsicConstraints = std::unordered_map<SymSCClassName, IntrinsicConstraint, std::hash<Symbol>>;



}