#pragma once

#include "sc_sema/compiler.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/module.hpp"
#include "sc_sema/symbol_table.hpp"

#include <memory>

namespace sc::compiler {

struct Backend;


// What the stages share, behind ClassLibrary::internals. The members refer to each other, so it doesn't move.
// Most users don't need access to this, but it is used for introspection, which the tests need.
struct ClassLibraryInternals {
    explicit ClassLibraryInternals(ir::ClassLibrarySources class_sources);
    ClassLibraryInternals(const ClassLibraryInternals&) = delete;
    ClassLibraryInternals& operator=(const ClassLibraryInternals&) = delete;
    ~ClassLibraryInternals();

    ir::SymbolTable symbols {};
    ir::ModuleTable modules {};
    // By the index of the class file each was parsed from.
    ir::ClassLibrarySources sources;
    ir::SymSCClassName abstract_object;
    ir::ContextPtr ctx;
    ir::IRCompiler compiler;
    // Set by compile_class_library, see details/backend.hpp.
    std::unique_ptr<Backend> backend;
};
}
