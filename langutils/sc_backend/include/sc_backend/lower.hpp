#pragma once

#include "sc_backend/frames.hpp"
#include "sc_backend/lir.hpp"
#include "sc_backend/members.hpp"

#include <ostream>

namespace sc::ir {
struct ConstantTable;
struct SymbolTable;
}

namespace sc::backend {

// The selectors lowering sends that the source doesn't write, e.g., `copy` for a new mutable constant, or `envirGet`
// for `~x`.
struct LoweringSymbols {
    explicit LoweringSymbols(ir::SymbolTable& symbols);

    ir::SymSCSelector new_;
    ir::SymSCSelector add;
    ir::SymSCSelector put;
    ir::SymSCSelector at;
    ir::SymSCSelector copy;
    ir::SymSCSelector copy_to_end;
    // `prSimpleNumberSeries`, which the old grammar named s_series, not the special selector `series`.
    ir::SymSCSelector series;
    ir::SymSCSelector envir_get;
    ir::SymSCSelector envir_put;
    ir::SymSCSelector perform_list;
    ir::SymSCSelector super_perform_list;
};

struct LoweringContext {
    const ir::IRGraph& graph;
    const Frames& frames;
    const MemberTable& members;
    const ir::ConstantTable& constants;
    // Names become symbols, e.g., a keyword argument's.
    ir::SymbolTable& symbols;
    const LoweringSymbols& selectors;
    // `[1, 2]` is an Array and `(a: 1)` an Event, see CommonClasses.
    ir::SymSCClassName array;
    ir::SymSCClassName event;
};

// Lowers a class's methods, or interpreted code. A poisoned method, or one with a frame too large for the VM, isn't
// lowered, so isn't a root. The graph's frames must be those layout_frames gave for the same root.
[[nodiscard]] lir::LoweredGraph lower(const LoweringContext& ctx, ir::join<ir::SCClass_I, ir::SCInterpreted_I> root);

// One instruction as text, e.g., `Send + (positional 1)`, for dumps.
void write_instruction(std::ostream& out, const lir::Instruction& instruction, const ir::IRGraph& graph,
                       const ir::SymbolTable& symbols, const ir::ConstantTable& constants);

}
