#include "sc_sema/tables.hpp"
#include "sc_sema/symbols.hpp"
#include <algorithm>

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


std::string_view SymbolTable::operator()(Symbol i) const noexcept {
    std::scoped_lock lock { m_lock };
    return m_id_to_string[*i];
}

[[nodiscard]] const ClassDeclaration::Member* ClassDeclaration::find_member(SymSCIdentifier i) const {
    const auto fnd = std::find_if(members.begin(), members.end(), [=](const Member& m) { return m.name.value == i; });
    if (fnd == members.end())
        return nullptr;
    return &*fnd;
}

[[nodiscard]] ClassDeclaration::Member* ClassDeclaration::find_member(SymSCIdentifier i) {
    const auto fnd = std::find_if(members.begin(), members.end(), [=](const Member& m) { return m.name.value == i; });
    if (fnd == members.end())
        return nullptr;
    return &*fnd;
}

using Method = ClassDeclaration::Method;
using Member = ClassDeclaration::Member;

[[nodiscard]] const std::variant<Method, std::pair<Method, Method>>*
ClassDeclaration::find_method(SymSCSelector i) const {
    if (auto fnd = methods.find(i); fnd != methods.end())
        return &fnd->second;
    else
        return nullptr;
}
[[nodiscard]] std::variant<Method, std::pair<Method, Method>>* ClassDeclaration::find_method(SymSCSelector i) {
    if (auto fnd = methods.find(i); fnd != methods.end())
        return &fnd->second;
    else
        return nullptr;
}

void ClassDeclarations::register_class_declaration(ClassDeclaration decl) {
    const auto name = decl.name;
    std::scoped_lock lock { m_lock };
    m_class_declarations.insert({ name.value, std::move(decl) });
}

[[nodiscard]] bool ClassDeclarations::exists(SymSCClassName name) const {
    std::scoped_lock lock { m_lock };
    return m_class_declarations.find(name) != m_class_declarations.end();
}

[[nodiscard]] size_t ClassDeclarations::size() const { return m_class_declarations.size(); }

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
