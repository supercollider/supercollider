#pragma once

#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/common_classes.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/decls.hpp"
#include "sc_sema/module.hpp"
#include "sc_sema/symbols.hpp"

#include <mutex>

namespace sc::ir {
struct CurrentDiagnostics {
    void operator()(sc::diag::Diagnostic diag) {
        std::scoped_lock lock { m_lock };
        m_diagnostics.push_back(std::move(diag));
    }

    void operator()(std::vector<sc::diag::Diagnostic> diags) {
        std::scoped_lock lock { m_lock };
        for (auto& diag : diags)
            m_diagnostics.push_back(std::move(diag));
    }

    [[nodiscard]] bool empty() const {
        std::scoped_lock lock { m_lock };
        return m_diagnostics.empty();
    }

    [[nodiscard]] explicit operator bool() const { return empty(); }

    [[nodiscard]] std::vector<sc::diag::Diagnostic> consume() {
        std::scoped_lock lock { m_lock };
        return std::move(m_diagnostics);
    }

private:
    mutable std::mutex m_lock { };
    std::vector<sc::diag::Diagnostic> m_diagnostics { };
};


struct InheritanceEdges {
    void insert(SymSCClassName parent, SymSCClassName child);
    void insert(SymSCClassName none) { m_sub_classes[none]; };



    /// Requires root has no parents.
    /// Returns inheritance cycles.
    /// If empty, no cycles.
    std::vector<std::vector<SymSCClassName>> cycle_check(SymSCClassName root, std::vector<SymSCClassName>& topo_ordering) const;

    // Not thread safe.
    const std::vector<SymSCClassName>& get_unsafe(SymSCClassName n) const;


    template <typename F> void depth_first_do(SymSCClassName root, F&& f) {
        std::invoke(f, root);
        for (const auto sub : m_sub_classes.at(root)) {
            depth_first_do(sub, f);
        }
    }

private:
    mutable std::mutex m_lock { };
    std::unordered_map<SymSCClassName, std::vector<SymSCClassName>, std::hash<Symbol>> m_sub_classes { };
    std::unordered_map<SymSCClassName, SymSCClassName, std::hash<Symbol>> m_super_class { };
};

struct ClassNames {
    using V = std::pair<std::shared_ptr<const ast::ASTGraph>, ast::ClassNameIdentifierIndex>;
    [[nodiscard]] std::optional<V> encountered_class_before(SymSCClassName n, std::shared_ptr<const ast::ASTGraph> g,
                                                            ast::ClassNameIdentifierIndex i);

private:
    mutable std::mutex m_lock {};
    std::unordered_map<SymSCClassName, V, std::hash<Symbol>> m_class_locations {};
};

struct SymbolTable;

struct SourceEntry {
    std::shared_ptr<const ast::ASTGraph> graph;
    ast::ClassOrExtensionListIndex index;
    ModuleIndex module;
};


struct Context {
    Context(SymbolTable& sym_table, CommonClasses com, ModuleTable& mod_t, std::vector<std::optional<SourceEntry>>& class_library):
        symbol_table(sym_table),
        common_classes(com),
        modules(mod_t),
        class_library_ast(class_library) {}

    SymbolTable& symbol_table;
    CommonClasses common_classes;
    ModuleTable& modules;
    ClassDeclarations class_declarations { };
    std::vector<std::optional<SourceEntry>>& class_library_ast;
    ConstantTable constant_table { };
    CurrentDiagnostics diagnostics { };
    InheritanceEdges inheritance_edges { };
    ClassNames class_names {};
};

}