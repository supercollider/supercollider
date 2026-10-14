#pragma once
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/ir.hpp"
#include "sc_parser/ast.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_sema/tables.hpp"

#include <mutex>
#include <thread>
#include <optional>
#include <atomic>
#include <unordered_map>
#include <unordered_set>

namespace sc::ir {
using Diagnostic = sc::diag::Diagnostic;

struct CurrentDiagnostics {
    void operator()(Diagnostic diag) {
        std::scoped_lock lock { m_lock };
        m_diagnostics.push_back(std::move(diag));
    }

    void operator()(std::vector<Diagnostic> diags) {
        std::scoped_lock lock { m_lock };
        for (auto& diag : diags)
            m_diagnostics.push_back(std::move(diag));
    }

    [[nodiscard]] bool empty() const {
        std::scoped_lock lock { m_lock };
        return m_diagnostics.empty();
    }

    [[nodiscard]] explicit operator bool() const { return empty(); }

    [[nodiscard]] std::vector<Diagnostic> consume() {
        std::scoped_lock lock { m_lock };
        return std::move(m_diagnostics);
    }

private:
    mutable std::mutex m_lock { };
    std::vector<Diagnostic> m_diagnostics { };
};

struct CommonClasses {
    SymSCClassName root_object, default_inheritor, class_, string, true_, false_, nil, float_, int_, char_, symbol,
        array, event;
};


struct InheritanceEdges {
    void insert(SymSCClassName parent, SymSCClassName child);

    /// Requires root has no parents.
    /// Returns inheritance cycles.
    /// If empty, no cycles.
    std::vector<std::vector<SymSCClassName>> cycle_check(SymSCClassName root) const;

private:
    mutable std::mutex m_lock {};
    std::unordered_map<SymSCClassName, std::vector<SymSCClassName>, std::hash<Symbol>> m_sub_classes {};
};

using GraphAndIndex = std::pair<ast::ASTGraph, ast::ClassOrExtensionListIndex>;
// This is the runtime context.
// It is all the class library information, and the symbol and constant tables.
// When a irnode compiler pass needs to know something about the rest of the world, it goes through this.
struct Context {
    Context(SymbolTable& sym_table, CommonClasses com, std::vector<GraphAndIndex> lib):
        symbol_table(sym_table),
        common_classes(com),
        m_ast_of_class_library(std::move(lib)) {}


    SymbolTable& symbol_table;
    ClassDeclarations class_declarations { };
    ConstantTable constant_table { };
    CurrentDiagnostics diagnostics { };
    CommonClasses common_classes;
    InheritanceEdges inheritance_edges {};
    [[nodiscard]] const std::vector<GraphAndIndex>& ast_of_class_library() const { return m_ast_of_class_library; }

private:
    std::vector<GraphAndIndex> m_ast_of_class_library;
};

struct IntrinsicConstraint {
    SymSCClassName target;
    std::optional<SymSCClassName> super_class;
    // These are ordered.
    std::vector<SymSCIdentifier> member_names;
    std::optional<SymSCIdentifier> slot_def;

    // Checks the constraint.
    // As this mutates the class declaration, it is only thread safe if no other class mutates the same decl,
    //  this is relatively easy to enforce as the contraint is const.
    [[nodiscard]] std::vector<Diagnostic> operator()(Context& cxt, ClassDeclaration& decl,
                                                     const std::vector<GraphAndIndex>& ast_graphs) const;
};

using IntrinsicConstraints = std::unordered_map<SymSCClassName, IntrinsicConstraint, std::hash<Symbol>>;

struct IRCompiler {
    IRCompiler(SymbolTable& s): symbol_table(s) {}

    [[nodiscard]] std::vector<Diagnostic>
    compile_class_library(const IntrinsicConstraints& intrinsic_constraints, //
                          SymSCClassName root_object,
                          std::vector<GraphAndIndex> class_lib, //
                          size_t num_threads = std::thread::hardware_concurrency() //
    );

    [[nodiscard]] const std::optional<Context>& context() const { return m_context; }

private:
    SymbolTable& symbol_table;

    /// Only valid once compile_class_library has been called.
    std::optional<Context> m_context;

    std::atomic<bool> m_compile_has_been_called { false };

    struct MethodData {
        SymSCClassName parent_class;
        SymSCSelector name;
        SCMethod_I index;
        bool poisoned { false };
    };

    struct ClassData {
        SymSCClassName name;
        std::optional<SymSCClassName> super;
        std::vector<SymSCClassName> sub_classes;
        IRGraph ir_graph;
        SCClass_I class_index;
        std::unordered_map<SymSCSelector, MethodData, std::hash<Symbol>> method_lookup {};
        bool poisoned { false };
    };

    // Called during declaration.
    void insert_class_ir(SymSCClassName name, IRGraph&& graph, SCClass_I class_index);

    // Should only be called once the IRGraphs have been made.
    // That is after the declaration stage.
    void poison_class(SymSCClassName name);

    mutable std::mutex m_ir_graphs_lock { };
    std::unordered_map<SymSCClassName, ClassData, std::hash<Symbol>> m_classes { };
};


// Makes some basic assumptions about the class library.
// Should not be used when compiling true class library code where we want to allow users to provide extention methods
//   that override common methods.
std::variant<ConstExpression, Diagnostic> compile_consteval(ast::ASTGraph, ast::AnyExprIndex, SymbolTable&);


} // sc::ir
