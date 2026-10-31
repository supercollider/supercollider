#pragma once
#include "sc_util/language_version.hpp"
#include "sc_util/indexed_vector.hpp"
#include "sc_util/strong_index.hpp"

#include "sc_sema/details/intrinsic_constraint.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <thread>
#include <atomic>
#include <unordered_map>

namespace sc::diag {
class Diagnostic;
}

namespace sc::ir {

struct Context;
struct SymbolTable;
struct IRGraph;
}

namespace sc::ast {
struct ASTGraph;
}

namespace sc::ir {

struct IntrinsicConstraint;
struct ClassDeclaration;

/// @brief Checks a class declaration against an intrinsic constraint, the shape the VM requires of it.
[[nodiscard]] std::vector<sc::diag::Diagnostic> check_intrinsic_constraint(const IntrinsicConstraint& c, Context& cxt,
                                                                           ClassDeclaration& decl);

/// @brief Compiles parsed classes and interpreted code into IR, one IRGraph per class.
/// The context is referred to, not owned, so it must outlive the compiler.
struct IRCompiler {
    IRCompiler(Context& mctx);
    ~IRCompiler();

    /// @brief Compiles every class declared in the context, in parallel where it can.
    /// Can only be called once.
    [[nodiscard]] std::vector<sc::diag::Diagnostic>
    compile_class_library(const IntrinsicConstraints& intrinsic_constraints, //
                          size_t num_threads = std::thread::hardware_concurrency() //
    );

    struct Interpreted {
        std::shared_ptr<IRGraph> ir; // null if failed
        std::vector<sc::diag::Diagnostic> diagnostics;
    };

    /// @brief Compiles interpreted code, after the class library.
    ///
    /// The root must be a region or a file as a function. It is compiled as if it were written inside
    /// `Interpreter:functionCompileContext`. Without that method, or if Interpreter is poisoned, it isn't compiled,
    /// and `ir` is null. Several can be compiled at once, on different threads, as each keeps its own diagnostics and
    /// only reads the class library.
    ///
    [[nodiscard]] Interpreted compile_interpreted(std::shared_ptr<const ast::ASTGraph> ast_graph,
                                                  std::optional<ast::LanguageVersion> given = std::nullopt,
                                                  ast::LanguageVersion fallback = ast::LanguageVersion::Legacy);

    /// @brief The graph of a compiled class.
    /// @return The graph, or null when the class is poisoned or isn't declared.
    [[nodiscard]] std::shared_ptr<const IRGraph> class_graph(SymSCClassName name) const;

    /// @brief Calls a function with the graph of every compiled class.
    /// Poisoned classes aren't compiled, so have no graph, and are skipped. Some are still in the hierarchy, e.g., one
    /// declared with an invalid slot type, `Foo[bad] {}`.
    /// @param f Called with each compiled `const IRGraph&`, a superclass before its subclasses.
    template <typename F> void depth_first_do(F&& f) const {
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
