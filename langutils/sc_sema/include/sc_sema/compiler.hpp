#pragma once
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_parser/sc_grammar_shared.hpp"

#include "sc_sema/common_classes.hpp"
#include "sc_sema/intrinsic_constraint.hpp"
#include "sc_util/indexed_vector.hpp"
#include "sc_util/strong_index.hpp"

#include <cstdint>
#include <memory>
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
    IRCompiler(Context& mctx);
    ~IRCompiler();

    [[nodiscard]] std::vector<sc::diag::Diagnostic>
    compile_class_library(const IntrinsicConstraints& intrinsic_constraints, //
                          size_t num_threads = std::thread::hardware_concurrency() //
    );

    // Interpreted code, whose root is a region or a file as a function, compiled after the class library, as if it
    // were written inside `Interpreter:functionCompileContext`. Without that method, or if Interpreter is poisoned, it
    // isn't compiled, and ir is null. Its version is given here, as it isn't in a class, see ast::LanguageVersion.
    // Several can be compiled at once, on different threads, as each keeps its own diagnostics and only reads the
    // class library.
    struct Interpreted {
        std::shared_ptr<IRGraph> ir;
        std::vector<sc::diag::Diagnostic> diagnostics;
    };
    [[nodiscard]] Interpreted compile_interpreted(std::shared_ptr<const ast::ASTGraph> ast_graph,
                                                  ast::LanguageVersion version = ast::LanguageVersion::Legacy);

    // The graph of a compiled class, null when it is poisoned or isn't declared.
    [[nodiscard]] std::shared_ptr<const IRGraph> class_graph(SymSCClassName name) const;

    // Poisoned classes aren't compiled, so have no graph, and are skipped. Some are still in the hierarchy, e.g., one
    // declared with an invalid slot type, `Foo[bad] {}`.
    template<typename F>
    void depth_first_do(F&& f) const {
        for (const auto& g : m_classes.raw())
            if (g)
                std::invoke(f, *g);
    }

private:
    using TopologicalPosition = util::StrongIndex<std::uint32_t, struct TopologicalPosition__>;

    Context& ctx;

    std::atomic<bool> m_compile_has_been_called { false };

    util::IndexedVector<TopologicalPosition, SymSCClassName> m_topological_ordering {};
    // This will be a nullptr if the class is poisoned.
    util::IndexedVector<TopologicalPosition, std::shared_ptr<IRGraph>> m_classes {};
    std::unordered_map<SymSCClassName, TopologicalPosition, std::hash<Symbol>> m_class_positions {};

    // The Interpreter's members, built once after the class library, as every piece of interpreted code reads them.
    // Null if it, or functionCompileContext, didn't compile.
    struct InterpreterMembers;
    std::unique_ptr<const InterpreterMembers> m_interpreter_members;
};


} // sc::ir
