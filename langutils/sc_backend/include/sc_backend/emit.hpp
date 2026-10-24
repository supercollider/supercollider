#pragma once

#include "sc_backend/frames.hpp"
#include "sc_backend/lir.hpp"
#include "sc_bytecode/ByteCodeArray.h"
#include "sc_bytecode/OpcodeOperands.h"
#include "sc_diagnostic/sc_diagnostic.hpp"

#include <cstdint>
#include <ostream>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

namespace sc::ir {
struct ConstantTable;
struct SymbolTable;
}

namespace sc::backend {

struct SpecialSymbols {
    explicit SpecialSymbols(ir::SymbolTable& symbols);

    using SpecialSelector = std::variant<OpSpecialSelectors, OpUnaryMath, OpBinaryMath>;

    std::unordered_map<ir::Symbol, SpecialSelector, std::hash<ir::Symbol>> special_selectors;
    std::unordered_map<ir::Symbol, std::uint8_t, std::hash<ir::Symbol>> special_classes;
    ir::SymSCSelector series;
};


// A class's `const`, which is the class's own object, see lir::PushClassConstant. owner is the meta class.
struct ClassConstant {
    ir::SymSCClassName owner;
    ir::SymSCIdentifier name;
    ir::ConstExprIndex value;
};

using SelectorContent = std::variant<ir::SymSCSelector, ir::SymSCClassName, lir::FunctionIndex>;
// Into a function's selectors table, which the bytecode indexes with a byte.
using SelectorIndex = util::StrongIndex<std::uint8_t, struct SelectorIndex__>;
using ConstantContent = std::variant<ir::ConstExprIndex, ir::SymSCSymbol, ClassConstant>;

struct EmittedFunction {
    CompilingBytecodes::Data code {};
    util::IndexedVector<SelectorIndex, SelectorContent> selectors {};
    std::vector<ConstantContent> constants {};
};

// This is the output of the emit function, the input is LoweredGraph.
struct EmittedGraph {
    util::IndexedVector<lir::FunctionIndex, EmittedFunction> functions {};
    std::vector<lir::FunctionIndex> roots {};
    std::vector<sc::diag::Diagnostic> diagnostics {};
};

struct EmitContext {
    const ir::IRGraph& graph;
    const Frames& frames;
    const ir::ConstantTable& constants;
    const SpecialSymbols& special_symbols;
};

// Encodes the lowered graph as bytecode, choosing the bytecodes the old compiler (PyrParseNode.cpp) did, and numbering
// the tables in the order it did, so legacy code compiles to the same bytes.
[[nodiscard]] EmittedGraph emit(const EmitContext& ctx, const lir::LoweredGraph& lowered);

// The tables, then each instruction's offset, bytes and name, for dumps. indent starts each line.
void write_emitted(std::ostream& out, const EmittedFunction& function, std::string_view indent,
                   const ir::SymbolTable& symbols, const ir::ConstantTable& constants);

}
