#pragma once

#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/source_location.hpp"
#include "sc_sema/symbol_table.hpp"

#include <filesystem>
#include <optional>
#include <ostream>

// How a node of the ir is written, shared by ir_dump and sc_explorer, so they show the same thing.
namespace sc::ir::describe {

struct Describe {
    const IRGraph& graph;
    const Context& ctx;
    // Interpreted code is shown by its file, written relative to this, if given.
    std::optional<std::filesystem::path> relative_to {};
};

// `(line:column)`, counting from one, as in the diagnostics.
void write_location(std::ostream& out, const SourceLocation& l);

// Left out, as it adds nothing, e.g., an empty argument list, or a member inherited from a class other than the one
// being dumped.
[[nodiscard]] bool hidden(const Describe& d, const IRGraph::Edges& edges, const IRGraph::Variant& payload,
                          std::optional<SymSCClassName> dumped_class);

// A meta class with nothing of its own, no class variables or class methods, only repeats its class.
[[nodiscard]] bool repeats_class(const Describe& d, SCClass_I c);

// What follows the node's name, e.g., `: local a = 1, declared (1:5): a`. Its constant value is ` = value`, and, for
// methods and functions, what calling them returns, ` -> value`. Only a method shows its source, the rest is found by
// location.
void write_details(const Describe& d, std::ostream& out, const IRGraph::Variant& payload, const SourceLocation& loc,
                   NodeIndex i);

}
