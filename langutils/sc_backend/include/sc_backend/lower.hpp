#pragma once

#include "sc_backend/details/frames.hpp"
#include "sc_backend/details/inlining.hpp"
#include "sc_backend/details/lir.hpp"
#include "sc_backend/details/members.hpp"

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

/// @brief Everything lower() needs, all referred to rather than owned, so they must outlive the call.
struct LoweringContext {
    const ir::IRGraph& graph;
    const Frames& frames;
    const MemberTable& members;
    const ir::ConstantTable& constants;
    ir::SymbolTable& symbols;
    const LoweringSymbols& selectors;
    const InliningContext& inlining;
    ir::SymSCClassName array;
    ir::SymSCClassName event;
    ir::OptionalSymSCClassName poisoned_method_error;
};

/// @brief Lowers a class or interpreted code to instructions in the order the VM runs them.
///
/// A class's poisoned methods are lowered as throwing ctx.poisoned_method_error, as are those that fail to lower, after
/// their failed functions.
/// @param root The class or interpreted code. It cannot be a poisoned method nor one that is too large for the VM.
/// @return Every function, with the root's first and its nested functions following.
[[nodiscard]] lir::LoweredGraph lower(const LoweringContext& ctx, ir::join<ir::SCClass_I, ir::SCInterpreted_I> root);

void write_instruction(std::ostream& out, const lir::Function& function, const lir::Instruction& instruction,
                       const ir::IRGraph& graph, const ir::SymbolTable& symbols, const ir::ConstantTable& constants);

////////////////////////////////////////////////////////////////////////////////
// Lowering symbols
////////////////////////////////////////////////////////////////////////////////

struct LoweringSymbols {
    explicit LoweringSymbols(ir::SymbolTable& symbols);

    ir::SymSCSelector new_;
    ir::SymSCSelector throw_;
    ir::SymSCSelector add;
    ir::SymSCSelector put;
    ir::SymSCSelector at;
    ir::SymSCSelector copy;
    ir::SymSCSelector copy_to_end;
    /// `prSimpleNumberSeries`, the old grammar's s_series, not the special selector `series`.
    ir::SymSCSelector series;
    ir::SymSCSelector envir_get;
    ir::SymSCSelector envir_put;
    ir::SymSCSelector perform_list; ///< `performList`, for variadic arguments.
    ir::SymSCSelector super_perform_list; ///< `performList` sent to `super`.
    /// `case { c }` is sent as `c.if`.
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
