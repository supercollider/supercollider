#pragma once

#include "sc_sema/symbols.hpp"
#include <mutex>
#include <string>
#include <unordered_set>
namespace sc::ir {

struct SymbolTable {
    // TODO: use boost unordered flat map (when we can figure out how the includes work in sc...)

    [[nodiscard]] Symbol get(std::string s);

    [[nodiscard]] SymSCSelector get_selector(std::string s);
    [[nodiscard]] SymSCClassName get_class_name(std::string s);
    [[nodiscard]] SymSCClassName get_meta_class_name(std::string s);
    [[nodiscard]] SymSCClassName get_meta_class_name(SymSCClassName);


    [[nodiscard]] SymSCPrimitive get_primitive(std::string);

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
}