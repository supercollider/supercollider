#pragma once
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ir.hpp"
#include "sc_parser/ast.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_sema/tables.hpp"

#include <mutex>
#include <thread>
#include <optional>
#include <atomic>

namespace sc::ir {
using Diagnostic = sc::diag::Diagnostic;

struct CurrentDiagnostics {
    void operator()(Diagnostic diag) {
        std::scoped_lock lock { m_lock };
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
    SymSCClassName root_object, object, class_, string, true_, false_, nil, float_, int_, char_, symbol, array;
};


using GraphAndIndex = std::pair<ast::ASTGraph, ast::ClassOrExtensionListIndex>;
// This is the runtime context.
// It is all the class library information, and the symbol and constant tables.
// When a irnode compiler pass needs to know something about the rest of the world, it goes through this.
struct Context {
    Context(SymbolTable sym_table, CommonClasses com, std::vector<GraphAndIndex> lib):
        symbol_table(std::move(sym_table)),
        common_classes(com),
        m_ast_of_class_library(std::move(lib)) {}


    SymbolTable symbol_table;
    ClassDeclarations class_declarations { };
    ConstantTable constant_table { };
    CurrentDiagnostics diagnostics { };
    CommonClasses common_classes;
    [[nodiscard]] const std::vector<GraphAndIndex>& ast_of_class_library() const { return m_ast_of_class_library; }

private:
    std::vector<GraphAndIndex> m_ast_of_class_library;
};

struct IntrinsicConstraints {
    SymSCClassName target;
    std::optional<SymSCClassName> super_class;
    std::optional<std::vector<SymSCIdentifier>> member_names;
    std::optional<SymSCIdentifier> slot_def;
};

struct IRCompiler {
    [[nodiscard]] std::vector<Diagnostic> compile_class_library( //
        std::unordered_map<SymSCClassName, IntrinsicConstraints, std::hash<Symbol>> intrinsic_constraints, //
        std::vector<GraphAndIndex> class_lib, //
        size_t num_threads = std::thread::hardware_concurrency() //
    );

    [[nodiscard]] const std::optional<Context>& context() const { return m_context; }

private:
    // Only valid once compile_class_library has been called.
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

    void insert_class_ir(SymSCClassName name, IRGraph&& graph, SCClass_I class_index);

    void poison_class(SymSCClassName name);

    mutable std::mutex m_ir_graphs_lock { };
    std::unordered_map<SymSCClassName, ClassData, std::hash<Symbol>> m_classes { };
};


} // sc::ir
