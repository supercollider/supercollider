#pragma once

#include "sc_backend/frames.hpp"
#include "sc_backend/inlining.hpp"
#include "sc_backend/lir.hpp"
#include "sc_bytecode/ByteCodeArray.h"
#include "sc_diagnostic/sc_diagnostic.hpp"

#include <cstdint>
#include <ostream>
#include <string_view>
#include <variant>
#include <vector>

namespace sc::ir {
struct ConstantTable;
struct SymbolTable;
}

namespace sc::backend {

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

struct EmittedGraph;
struct EmittedFunction;

struct EmitContext {
    const ir::IRGraph& graph;
    const Frames& frames;
    const ir::ConstantTable& constants;
    const SpecialSymbols& special_symbols;
};

[[nodiscard]] EmittedGraph emit(const EmitContext& ctx, const lir::LoweredGraph& lowered);

void write_emitted(std::ostream& out, const EmittedFunction& function, std::string_view indent,
                   const ir::SymbolTable& symbols, const ir::ConstantTable& constants);


////////////////////////////////////////////////////////////////////////////////
// Emitted function
////////////////////////////////////////////////////////////////////////////////

using SelectorIndex = util::StrongIndex<std::uint8_t, struct SelectorIndex__>;
using ConstantIndex = util::StrongIndex<std::uint32_t, struct ConstantIndex__>;

struct ClassConstant {
    ir::SymSCClassName owner;
    ir::SymSCIdentifier name;
    ir::ConstExprIndex value;
};
static_assert(sizeof(ClassConstant) == 12);

using SelectorContent = std::variant<ir::SymSCSelector, ir::SymSCClassName, lir::FunctionIndex>;
static_assert(sizeof(SelectorContent) == 8);

// A byte of a function's code, as an offset from a point the instruction using it defines.
using CodeOffset = util::StrongIndex<std::uint32_t, struct CodeOffset__>;

// Inlined switch table.
struct SwitchTable {
    std::uint32_t capacity { 0 };
    std::vector<std::pair<ir::ConstExprIndex, CodeOffset>> offsets {};
    CodeOffset default_offset { 0 };
};

using SwitchTableIndex = util::StrongIndex<std::uint32_t, struct SwitchTableIndex__>;

using ConstantContent = std::variant<ir::ConstExprIndex, ir::SymSCSymbol, ClassConstant, SwitchTableIndex>;
static_assert(sizeof(ConstantContent) == 16);


struct EmittedFunction {
    CompilingBytecodes::Data code {};
    util::IndexedVector<SelectorIndex, SelectorContent> selectors {};
    util::IndexedVector<ConstantIndex, ConstantContent> constants {};
    util::IndexedVector<SwitchTableIndex, SwitchTable> switch_tables {};
};

////////////////////////////////////////////////////////////////////////////////
// Emitted graph
////////////////////////////////////////////////////////////////////////////////

// This is the output of the emit function, the input is LoweredGraph.
struct EmittedGraph {
    util::IndexedVector<lir::FunctionIndex, EmittedFunction> functions {};
    std::vector<lir::FunctionIndex> roots {};
    std::vector<sc::diag::Diagnostic> diagnostics {};
};
}
