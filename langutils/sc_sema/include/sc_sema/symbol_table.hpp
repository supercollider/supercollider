#pragma once

#include "sc_sema/symbols.hpp"
#include <deque>
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


    // The view stays valid while the table exists, even as other threads add symbols.
    [[nodiscard]] std::string_view operator()(Symbol i) const noexcept;

private:
    mutable std::mutex m_lock { };
    // A deque, as it doesn't move its strings when it grows, which would invalidate the views of short strings, whose
    // characters are stored inside the string.
    // This would work as a vector IF string didn't do small string optimisation (which was my original assumption)!
    std::deque<std::string> m_id_to_string {};
    std::unordered_map<std::string, Symbol> m_string_to_id { };
    std::unordered_set<Symbol> m_meta_classes {};
};
}