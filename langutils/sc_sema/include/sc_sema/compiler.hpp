#pragma once
#include "sc_diagnostic/sc_diagnostic.hpp"

#include "sc_sema/common_classes.hpp"
#include "sc_sema/intrinsic_constraint.hpp"

#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <unordered_map>

namespace sc::ir {

struct Context;
struct SymbolTable;
struct IRGraph;

CommonClasses symbol_table_and_common_classes(SymbolTable& sym_table, SymSCClassName root_object);


struct IntrinsicConstraint;
struct ClassDeclaration;
[[nodiscard]] std::vector<sc::diag::Diagnostic> check_intrinsic_constraint(const IntrinsicConstraint& c, Context& cxt,
                                                                           ClassDeclaration& decl);

struct IRCompiler {
    IRCompiler(Context& mctx): ctx(mctx) {}

    // It is required that all common classes (in the context) are also listed here as constaints.
    [[nodiscard]] std::vector<sc::diag::Diagnostic>
    compile_class_library(const IntrinsicConstraints& intrinsic_constraints, //
                          size_t num_threads = std::thread::hardware_concurrency() //
    );

private:
    Context& ctx;

    std::atomic<bool> m_compile_has_been_called { false };

    mutable std::mutex m_classes_lock;
    std::unordered_map<SymSCClassName, std::shared_ptr<IRGraph>, std::hash<Symbol>> m_classes { };
};



} // sc::ir
