#pragma once

#include "sc_backend/frames.hpp"
#include "sc_backend/inlining.hpp"
#include "sc_backend/lir.hpp"
#include "sc_backend/members.hpp"

#include <ostream>

namespace sc::ir {
struct ConstantTable;
struct SymbolTable;
}

namespace sc::backend {

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

struct LoweringSymbols;

struct LoweringContext {
    const ir::IRGraph& graph;
    // From layout_frames, given the same root and inlining.
    const Frames& frames;
    const MemberTable& members;
    const ir::ConstantTable& constants;
    ir::SymbolTable& symbols;
    const LoweringSymbols& selectors;
    const InliningContext& inlining;
    ir::SymSCClassName array;
    ir::SymSCClassName event;
    // A poisoned method is lowered to throw this, with `thisMethod` and its errors. Without it, such a method is
    // skipped.
    ir::OptionalSymSCClassName poisoned_method_error;
};

// Root cannot be a poisoned method nor one that is too large for the VM. A class's poisoned methods are lowered as
// throwing ctx.poisoned_method_error.
[[nodiscard]] lir::LoweredGraph lower(const LoweringContext& ctx, ir::join<ir::SCClass_I, ir::SCInterpreted_I> root);

void write_instruction(std::ostream& out, const lir::Function& function, const lir::Instruction& instruction,
                       const ir::IRGraph& graph, const ir::SymbolTable& symbols, const ir::ConstantTable& constants);

////////////////////////////////////////////////////////////////////////////////
// Lowering symbols
////////////////////////////////////////////////////////////////////////////////

// Selectors lowering sends that the source doesn't write.
struct LoweringSymbols {
    explicit LoweringSymbols(ir::SymbolTable& symbols);

    ir::SymSCSelector new_;
    ir::SymSCSelector throw_;
    ir::SymSCSelector add;
    ir::SymSCSelector put;
    ir::SymSCSelector at;
    ir::SymSCSelector copy;
    ir::SymSCSelector copy_to_end;
    // `prSimpleNumberSeries`, the old grammar's s_series, not the special selector `series`.
    ir::SymSCSelector series;
    ir::SymSCSelector envir_get;
    ir::SymSCSelector envir_put;
    ir::SymSCSelector perform_list;
    ir::SymSCSelector super_perform_list;
    // `case { c }` is sent as `c.if`.
    ir::SymSCSelector if_;
    // Generators, as the old grammar rewrote them.
    ir::SymSCSelector r;
    ir::SymSCSelector do_;
    ir::SymSCSelector for_series;
    ir::SymSCSelector value;
    ir::SymSCSelector yield;
    ir::SymSCSelector always_yield;
    ir::SymSCSelector series_iter;
};
}
