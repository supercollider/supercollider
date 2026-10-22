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
}

namespace sc::ast {
struct ASTGraph;
}

namespace sc::ir {

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

    // Interpreted code, whose root is a region or a file as a function, compiled after the class library, as if it
    // were written inside `Interpreter:functionCompileContext`. Without that method, or if Interpreter is poisoned, it
    // isn't compiled, and ir is null.
    struct Interpreted {
        std::shared_ptr<IRGraph> ir;
        std::vector<sc::diag::Diagnostic> diagnostics;
    };
    [[nodiscard]] Interpreted compile_interpreted(std::shared_ptr<const ast::ASTGraph> ast_graph);

    // Poisoned classes aren't compiled, so have no graph, and are skipped. Some are still in the hierarchy, e.g., one
    // declared with an invalid slot type, `Foo[bad] {}`.
    template<typename F>
    void depth_first_do(F&& f) const {
        for(auto n : m_topological_ordering) {
            if (const auto fnd = m_classes.find(n); fnd != m_classes.end())
                std::invoke(f, *fnd->second);
        }
    }

private:
    Context& ctx;

    std::atomic<bool> m_compile_has_been_called { false };

    mutable std::mutex m_classes_lock;
    std::unordered_map<SymSCClassName, std::shared_ptr<IRGraph>, std::hash<Symbol>> m_classes { };

    // Topological ordering is only stable on a single threaded implementation, and only then, if the input files are in the same order.
    std::vector<SymSCClassName> m_topological_ordering{};
};


} // sc::ir
