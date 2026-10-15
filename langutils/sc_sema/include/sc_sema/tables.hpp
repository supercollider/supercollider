#pragma once
#include "sc_parser/sc_grammar_shared.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_sema/type_info.hpp"
#include <mutex>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

namespace sc::ir {

////////////////////////////////////////////////////////////////////////////////
// symbol table
////////////////////////////////////////////////////////////////////////////////

struct SymbolTable {
    // TODO: use boost unordered flat map (when we can figure out how the includes work in sc...)

    [[nodiscard]] Symbol get(std::string s);

    [[nodiscard]] SymSCSelector get_selector(std::string s);
    [[nodiscard]] SymSCClassName get_class_name(std::string s);
    [[nodiscard]] SymSCClassName get_meta_class_name(std::string s);
    [[nodiscard]] SymSCClassName get_meta_class_name(SymSCClassName);

    [[nodiscard]] SymSCSymbol get_symbol(std::string s);
    [[nodiscard]] SymSCIdentifier get_identifier(std::string s);
    [[nodiscard]] SymSCString get_string(std::string s);

    [[nodiscard]] bool is_meta(SymSCClassName n) const;

    SymbolTable() = default;


    [[nodiscard]] std::string_view operator()(Symbol i) const noexcept;

private:
    mutable std::mutex m_lock { };
    std::vector<std::string> m_id_to_string {};
    std::unordered_map<std::string, Symbol> m_string_to_id { };
    std::unordered_set<Symbol> m_meta_classes {};
};

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////


struct ClassDeclaration {
    Locatable<SymSCClassName> name;
    std::optional<Locatable<SymSCClassName>> super {};
    std::optional<Locatable<SymSCIdentifier>> slot_def {};
    // Meta classes don't have meta classes.
    std::optional<SymSCClassName> meta_class {};

    struct Member {
        Locatable<SymSCIdentifier> name;
        Locatable<ast::ReadWriteAccessor> accessor;
        std::optional<Locatable<ConstExprIndex>> expr_index;
        std::optional<TypeInfo> type_info;
        bool poisoned { false };
    };

    std::vector<Member> members {};

    struct Method {
        Locatable<SymSCSelector> name;
        bool compiler_known_implementation { false };
        bool poisoned { false };
    };
    // Either the Method, or the method and its extention.
    // It is an error to have multiple extention methods, we just taken a random one and produce a warning.
    std::unordered_map<SymSCSelector, std::variant<Method, std::pair<Method, Method>>, std::hash<Symbol>> methods {};

    bool poisoned { false };

    [[nodiscard]] const Member* find_member(SymSCIdentifier i) const;
    [[nodiscard]] Member* find_member(SymSCIdentifier i);

    [[nodiscard]] const std::variant<Method, std::pair<Method, Method>>* find_method(SymSCSelector i) const;
    [[nodiscard]] std::variant<Method, std::pair<Method, Method>>* find_method(SymSCSelector i);
};


struct ClassDeclarations {
    void register_class_declaration(ClassDeclaration decl);

    [[nodiscard]] bool exists(SymSCClassName name) const;

    [[nodiscard]] size_t size() const;

    /// This will lock.
    /// Requires name be registered.
    template <typename F> auto with_decl(SymSCClassName name, F&& f) {
        std::scoped_lock lock { m_lock };
        using R = std::invoke_result_t<F, ClassDeclaration&>;
        if constexpr (std::is_same_v<R, void>) {
            if (const auto fnd = m_class_declarations.find(name); fnd != m_class_declarations.end())
                std::invoke(std::forward<F>(f), m_class_declarations.find(name)->second);
            else
                return;
        } else if constexpr (std::is_pointer_v<R>) {
            if (const auto fnd = m_class_declarations.find(name); fnd != m_class_declarations.end())
                std::invoke(std::forward<F>(f), m_class_declarations.find(name)->second);
            else
                return nullptr;
        } else {
            if (const auto fnd = m_class_declarations.find(name); fnd != m_class_declarations.end())
                return std::optional<R> { std::invoke(std::forward<F>(f), m_class_declarations.find(name)->second) };
            else
                return std::optional<R> { std::nullopt };
        }
    }

    /// This will lock.
    /// Requires name be registered.
    template <typename F> auto with_decl(SymSCClassName name, F&& f) const {
        std::scoped_lock lock { m_lock };
        using R = std::invoke_result_t<F, const ClassDeclaration&>;
        if constexpr (std::is_same_v<R, void>) {
            if (const auto fnd = m_class_declarations.find(name); fnd != m_class_declarations.end()) {
                std::invoke(std::forward<F>(f), m_class_declarations.find(name)->second);
            }
            return;
        } else {
            if (const auto fnd = m_class_declarations.find(name); fnd != m_class_declarations.end()) {
                return std::optional<R> { std::invoke(std::forward<F>(f), m_class_declarations.find(name)->second) };
            } else {
                return std::optional<R> { std::nullopt };
            }
        }
    }

    template <typename F> auto for_all(F f) const {
        std::scoped_lock lock { m_lock };
        for (const auto& [name, decl] : m_class_declarations) {
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


private:
    mutable std::mutex m_lock {};
    std::unordered_map<SymSCClassName, ClassDeclaration, std::hash<Symbol>> m_class_declarations { };
};

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

struct ConstantTable {
    [[nodiscard]] ConstExprIndex register_constexpr(ConstExpression&& expr);

    // this is marked noexcept because you should never construct an index yourself.
    [[nodiscard]] const ConstExpression& operator()(ConstExprIndex index) const noexcept;


private:
    mutable std::mutex m_lock {};
    ConstExprIndex::Underlying m_rolling { 0 };
    // This means we duplicate the constexpr.
    // This could probably be optimized.
    std::unordered_map<ConstExprIndex, ConstExpression> index_to_expr { };
    std::unordered_map<ConstExpression, ConstExprIndex> expr_to_index { };
};

}
