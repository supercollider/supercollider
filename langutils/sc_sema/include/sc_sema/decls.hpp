#pragma once

#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/sc_grammar_shared.hpp"

#include "sc_sema/ast_location.hpp"
#include "sc_sema/const_expr_index.hpp"
#include "sc_sema/module.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_sema/type_info.hpp"

#include <memory>
#include <mutex>
#include <unordered_map>

namespace sc::ir {

struct ClassDeclaration {
public:
    struct Member {
        ast::Index total_index;
        // All ast indexes here are those in the main decl.
        SymSCIdentifier name;
        ast::Index name_index;
        ast::ReadWriteAccessor accessor;
        ast::OptionalIndex accessor_index;
        std::optional<std::pair<ConstExprIndex, ast::AnyExprIndex>> expr_index;
        std::optional<TypeInfo> type_info;
    };

    struct Method {
        // All ast indexes here refer to this graph.
        std::shared_ptr<const ast::ASTGraph> ast_graph;
        ModuleIndex module_of_def;
        ast::AnyValidMethodIndex ast_node_index;
        SymSCSelector name;
        ast::Index name_index;
        bool is_extention { false };
        bool compiler_known_implementation { false };
        bool poisoned { false };
    };

    ClassDeclaration(std::shared_ptr<const ast::ASTGraph> ast_graph, ModuleIndex mod, ast::Index index,
                     SymSCClassName name, ast::Index name_index):
        m_ast_graph_of_decl(ast_graph),
        module(mod),
        m_whole_index(index),
        m_name(name),
        m_name_index(name_index) { }

    [[nodiscard]] ast::Index index() const { return m_whole_index; }
    [[nodiscard]] SymSCClassName name() const { return m_name; }
    [[nodiscard]] ast::Index name_index() const { return m_name_index; }
    [[nodiscard]] std::shared_ptr<const ast::ASTGraph> ast_graph() const { return m_ast_graph_of_decl; }

    void super(SymSCClassName n, ast::Index i) { m_super.emplace(n, i); };
    [[nodiscard]] std::optional<std::pair<SymSCClassName, ast::Index>> super() const { return m_super; }
    void slot_def(SymSCIdentifier sym, ast::Index i) { m_slot_def = { sym, i }; };
    [[nodiscard]] std::optional<std::pair<SymSCIdentifier, ast::Index>> slot_def() const { return m_slot_def; }
    void meta_class(SymSCClassName m) { m_meta_class = m; }
    [[nodiscard]] std::optional<SymSCClassName> meta_class() const { return m_meta_class; }

    [[nodiscard]] const std::vector<Symbol>& member_order() const { return m_members_order; }

    [[nodiscard]] const Member* find_member(Symbol i) const;
    [[nodiscard]] Member* find_member(Symbol i);

    [[nodiscard]] const Method* find_method(Symbol i) const;
    [[nodiscard]] Method* find_method(Symbol i);

    enum struct MethodInsertResult {
        Ok,
        DuplicatedByMethod,
        DuplicatedByExtentionMethod,
        DuplicatedByAccessor,
    };
    [[nodiscard]] MethodInsertResult insert(Method m);

    // Called from within the class declaration, here we assume we are in the original class declaration.
    enum struct DuplicateLocation {
        None,
        Method,
        Member,
    };
    [[nodiscard]] DuplicateLocation insert_no_ext(Method m);

    [[nodiscard]] bool insert(Member);

    [[nodiscard]] auto unsafe_methods_begin() const { return m_methods.begin(); }
    [[nodiscard]] auto unsafe_methods_end() const { return m_methods.end(); }

    // The only reason a class should be poisoned is if there is a problem with its class heirachy (e.g., it is in a
    // cycle) These are thread safe because it is an atomic.
    void poison() { m_poisoned = true; }
    [[nodiscard]] bool poisoned() const { return m_poisoned; }


private:
    std::shared_ptr<const ast::ASTGraph> m_ast_graph_of_decl;
    ModuleIndex module;
    ast::Index m_whole_index;

    SymSCClassName m_name;
    ast::Index m_name_index;

    std::optional<std::pair<SymSCClassName, ast::Index>> m_super { };
    std::optional<std::pair<SymSCIdentifier, ast::Index>> m_slot_def { };
    // Meta classes don't have meta classes.
    std::optional<SymSCClassName> m_meta_class { };

    std::unordered_map<Symbol, Member, std::hash<Symbol>> m_members_map;
    std::vector<Symbol> m_members_order { };

    // Either the Method, or the method and its extention.
    // It is an error to have multiple extention methods, we just taken a random one and produce a warning.
    // Note, each method has its own ast_graph associated with it.
    std::unordered_map<Symbol, Method, std::hash<Symbol>> m_methods { };
    std::unordered_map<Symbol, Method, std::hash<Symbol>> m_overriden_methods { };

    enum struct NameLoc { Method, MethodAccessor, Member };
    std::unordered_map<Symbol, std::pair<Symbol, NameLoc>> m_names_in_scope;

    bool m_poisoned { false };

    // We don't have poisoned decls.
    // The only place where this could occur is when intrinsics are broken.
    // But in that case, we cannot continue compiling, so we just bail out there.
};


struct ClassDeclarations {
    // Locks container
    void register_class_declaration(ClassDeclaration decl);
    [[nodiscard]] bool exists(SymSCClassName name) const;


    // Locks individual class decl, does not lock the container.

    /// Requires name be registered.
    template <typename F> auto with_decl(SymSCClassName name, F&& f) {
        using R = std::invoke_result_t<F, ClassDeclaration&>;
        if constexpr (std::is_same_v<R, void>) {
            if (const auto fnd = m_class_declarations.find(name); fnd != m_class_declarations.end()) {
                std::scoped_lock lk { m_class_locks.at(name) };
                std::invoke(std::forward<F>(f), m_class_declarations.find(name)->second);
            } else
                return;
        } else if constexpr (std::is_pointer_v<R>) {
            if (const auto fnd = m_class_declarations.find(name); fnd != m_class_declarations.end()) {
                std::scoped_lock lk { m_class_locks.at(name) };
                std::invoke(std::forward<F>(f), m_class_declarations.find(name)->second);
            } else
                return nullptr;
        } else {
            if (const auto fnd = m_class_declarations.find(name); fnd != m_class_declarations.end()) {
                std::scoped_lock lk { m_class_locks.at(name) };
                return std::optional<R> { std::invoke(std::forward<F>(f), m_class_declarations.find(name)->second) };
            } else
                return std::optional<R> { std::nullopt };
        }
    }

    /// This will lock.
    /// Requires name be registered.
    template <typename F> auto with_decl(SymSCClassName name, F&& f) const {
        using R = std::invoke_result_t<F, const ClassDeclaration&>;
        if constexpr (std::is_same_v<R, void>) {
            if (const auto fnd = m_class_declarations.find(name); fnd != m_class_declarations.end()) {
                std::scoped_lock lk { m_class_locks.at(name) };
                std::invoke(std::forward<F>(f), m_class_declarations.find(name)->second);
            }
            return;
        } else {
            if (const auto fnd = m_class_declarations.find(name); fnd != m_class_declarations.end()) {
                std::scoped_lock lk { m_class_locks.at(name) };
                return std::optional<R> { std::invoke(std::forward<F>(f), m_class_declarations.find(name)->second) };
            } else {
                return std::optional<R> { std::nullopt };
            }
        }
    }

    template <typename F> auto for_all(F f) const {
        for (const auto& [name, decl] : m_class_declarations) {
            std::scoped_lock lk { m_class_locks.at(name) };
            std::invoke(f, name, decl);
        }
    }

    // This is not thread safe, and if you insert, it will invalidate the pointer.
    // Should only be used while declaring the classes and you are sure there are no concurrent accesses to the same
    // decl.
    [[nodiscard]] ClassDeclaration* get_unsafe(SymSCClassName n);

    // If you only access this as a const in all threads, this is safe.
    // After the classes have been declared, this is okay to use.
    [[nodiscard]] const ClassDeclaration* get_unsafe(SymSCClassName n) const;


    // iterator, assumes we are no longer inserting or removing, therefore no mutex is needed.
    [[nodiscard]] auto unsafe_begin() const -> decltype(auto) { return m_class_declarations.begin(); }
    [[nodiscard]] auto unsafe_end() const -> decltype(auto) { return m_class_declarations.end(); }

    [[nodiscard]] auto unsafe_begin() -> decltype(auto) { return m_class_declarations.begin(); }
    [[nodiscard]] auto unsafe_end() -> decltype(auto) { return m_class_declarations.end(); }

    // Not thread safe.
    [[nodiscard]] size_t size() const;
    [[nodiscard]] size_t num_not_poisoned() const;
    void poison_all();

private:
    mutable std::mutex m_insert_lock{};
    std::unordered_map<SymSCClassName, ClassDeclaration, std::hash<Symbol>> m_class_declarations { };
    std::unordered_map<SymSCClassName, std::mutex, std::hash<Symbol>> m_class_locks { };
};


}