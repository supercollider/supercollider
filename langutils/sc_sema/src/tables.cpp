#include "sc_sema/const_expr.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_sema/decls.hpp"
#include <string>

namespace sc::ir {

const static std::string meta_prefix = "Meta_";

[[nodiscard]] Symbol SymbolTable::get(std::string s) {
    std::scoped_lock lock { m_lock };
    if (const auto fnd = m_string_to_id.find(s); fnd != m_string_to_id.end()) {
        return { *fnd->second };
    }
    const auto new_id = Symbol::from(m_id_to_string.size());
    m_string_to_id.insert({ s, new_id });
    m_id_to_string.push_back(std::move(s));
    return new_id;
}

[[nodiscard]] SymSCSelector SymbolTable::get_selector(std::string s) { return SymSCSelector::from(*get(std::move(s))); }
[[nodiscard]] SymSCClassName SymbolTable::get_class_name(std::string s) {
    return SymSCClassName::from(*get(std::move(s)));
}
[[nodiscard]] SymSCSymbol SymbolTable::get_symbol(std::string s) { return SymSCSymbol::from(*get(std::move(s))); }
[[nodiscard]] SymSCIdentifier SymbolTable::get_identifier(std::string s) {
    return SymSCIdentifier::from(*get(std::move(s)));
}
[[nodiscard]] SymSCString SymbolTable::get_string(std::string s) { return SymSCString::from(*get(std::move(s))); }
[[nodiscard]] bool SymbolTable::is_meta(SymSCClassName n) const { return m_meta_classes.count(n); }

[[nodiscard]] SymSCClassName SymbolTable::get_meta_class_name(std::string s) {
    auto m = get_class_name(meta_prefix + s);
    m_meta_classes.insert(m);
    return m;
}
[[nodiscard]] SymSCClassName SymbolTable::get_meta_class_name(SymSCClassName n) {
    auto m = get_class_name(meta_prefix + std::string { (*this)(n) });
    m_meta_classes.insert(m);
    return m;
}


[[nodiscard]] SymSCPrimitive SymbolTable::get_primitive(std::string s) {
    return SymSCPrimitive::from(*get(std::move(s)));
}

std::string_view SymbolTable::operator()(Symbol i) const noexcept {
    std::scoped_lock lock { m_lock };
    return m_id_to_string[*i];
}


[[nodiscard]] const ClassDeclaration::Member* ClassDeclaration::find_member(Symbol i) const {
    const auto fnd = m_members_map.find(i);
    if (fnd == m_members_map.end())
        return nullptr;
    return &fnd->second;
}

[[nodiscard]] ClassDeclaration::Member* ClassDeclaration::find_member(Symbol i) {
    const auto fnd = m_members_map.find(i);
    if (fnd == m_members_map.end())
        return nullptr;
    return &fnd->second;
}

using Method = ClassDeclaration::Method;
using Member = ClassDeclaration::Member;

[[nodiscard]] const Method* ClassDeclaration::find_method(Symbol i) const {
    if (auto fnd = m_methods.find(i); fnd != m_methods.end())
        return &fnd->second;
    else
        return nullptr;
}

[[nodiscard]] Method* ClassDeclaration::find_method(Symbol i) {
    if (auto fnd = m_methods.find(i); fnd != m_methods.end())
        return &fnd->second;
    else
        return nullptr;
}


/// Extention method overriding rules:
///     You can only override an extention method if it was written in the decl module.
///     If an extention method exists not from the decl module, it is ambiguos, and we generate a diagnostic.
ClassDeclaration::MethodInsertResult ClassDeclaration::insert(Method m) {
    ModuleIndex from = m.module_of_def;
    const auto name = m.name;
    if (auto existing = m_methods.find(name); existing != m_methods.end()) {
        if (existing->second.is_extention && existing->second.module_of_def == module) {
            m_overriden_methods.insert({ name, std::move(existing->second) });
            existing->second = std::move(m);
            return MethodInsertResult::Ok;
        } else if (existing->second.is_extention) {
            return MethodInsertResult::DuplicatedByExtentionMethod;
        }
        m_overriden_methods.insert({ name, std::move(existing->second) });
        existing->second = std::move(m);
        return MethodInsertResult::Ok;
    } else {
        m_methods.insert({name, std::move(m)});
        return MethodInsertResult::Ok;
    }
}

ClassDeclaration::DuplicateLocation ClassDeclaration::insert_no_ext(Method m) {
    const auto name = m.name;

    if (m_names_in_scope.count(name) == 1) {
        if (m_members_map.count(name) == 1) { return  DuplicateLocation::Member; }
        else return DuplicateLocation::Method;
    }

    m_methods.insert({name, std::move(m)});
    return DuplicateLocation::None;
}

bool ClassDeclaration::insert(Member m) {
    const auto name = m.name;
    if (auto fnd = m_members_map.find(name); fnd != m_members_map.end()) {
        return false;
    }
    m_members_map.insert({name, std::move(m)});
    m_members_order.push_back(name);
    return true;
}


bool ClassDeclarations::register_class_declaration(ClassDeclaration& decl) {
    SymSCClassName name = decl.name();
    std::scoped_lock lock { m_lock };
    if (m_class_declarations.find(name) != m_class_declarations.end()) {
        return false;
    }
    m_class_declarations.insert({ name, std::move(decl) });
    return true;
}

[[nodiscard]] bool ClassDeclarations::exists(SymSCClassName name) const {
    std::scoped_lock lock { m_lock };
    return m_class_declarations.find(name) != m_class_declarations.end();
}

[[nodiscard]] size_t ClassDeclarations::size() const { return m_class_declarations.size(); }

[[nodiscard]] size_t  ClassDeclarations::num_not_poisoned() const {
    size_t count {0};
    for(auto& [k, v] : m_class_declarations) 
        count += v.poisoned() ? 0 : 1;
    return count;
}

[[nodiscard]] ClassDeclaration* ClassDeclarations::get_unsafe(SymSCClassName n) {
    if (auto fnd = m_class_declarations.find(n); fnd != m_class_declarations.end()) {
        return &fnd->second;
    } else {
        return nullptr;
    }
}

[[nodiscard]] const ClassDeclaration* ClassDeclarations::get_unsafe(SymSCClassName n) const {
    if (auto fnd = m_class_declarations.find(n); fnd != m_class_declarations.end()) {
        return &fnd->second;
    } else {
        return nullptr;
    }
}
void ClassDeclarations::poison_all() {
    std::scoped_lock lk { m_lock };
    for (auto& [k, v] : m_class_declarations)
        v.poison();
}


[[nodiscard]] ConstExprIndex ConstantTable::register_constexpr(ConstExpression&& expr) {
    std::scoped_lock lock { m_lock };
    if (const auto fnd = expr_to_index.find(expr); fnd != expr_to_index.end()) {
        return fnd->second;
    }
    const auto index = m_rolling;
    m_rolling += 1;
    expr_to_index.insert({ expr, index });
    index_to_expr.insert({ index, expr });
    return index;
}

[[nodiscard]] const ConstExpression& ConstantTable::operator()(ConstExprIndex index) const noexcept {
    std::scoped_lock lock { m_lock };
    return index_to_expr.at(index);
}

} // sc::ir
