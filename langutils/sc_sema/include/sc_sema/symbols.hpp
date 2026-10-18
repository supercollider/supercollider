#pragma once
#include "sc_util/type_set_index.hpp"
#include <cstdint>

namespace sc::ir {

enum struct SymbolTypes {
    SCSymbol,
    SCString,
    SCClassName,
    SCNamedIdentifier,
    SCSelector,
    SCPrimitive,
};

using SymbolDef = util::typed_index::QuicklyDefineTypesFromSpec<
    util::typed_index::Spec<std::uint32_t, SymbolTypes, struct Symbol____>>;

using Symbol = SymbolDef::Index;

using SymSCSymbol = SymbolDef::TypedIndex<SymbolTypes::SCSymbol>;
using SymSCString = SymbolDef::TypedIndex<SymbolTypes::SCString>;
using SymSCClassName = SymbolDef::TypedIndex<SymbolTypes::SCClassName>;
using OptionalSymSCClassName = SymbolDef::OptionalTypedIndex<SymbolTypes::SCClassName>;

using SymSCIdentifier = SymbolDef::TypedIndex<SymbolTypes::SCNamedIdentifier>;
using OptionalSymSCIdentifier = SymbolDef::OptionalTypedIndex<SymbolTypes::SCNamedIdentifier>;
using SymSCSelector = SymbolDef::TypedIndex<SymbolTypes::SCSelector>;
using SymSCPrimitive = SymbolDef::TypedIndex<SymbolTypes::SCPrimitive>;

}
