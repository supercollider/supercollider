#pragma once

#include "sc_sema/const_expr_index.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_util/strong_index.hpp"

#include <cstdint>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <variant>

namespace sc::ir {
struct IRCompiler;
struct SymbolTable;
}

namespace sc::backend {

// Where a member of a class lives at runtime.
// An instance variable is a slot of the object, the same in every subclass, as a class's layout starts with its
// superclass's. A class variable is in the one array of every class's class variables, at its class's first index (the
// old compiler's classVarIndex) plus its position among its class's own. A constant has no storage, its value is
// pushed.
using InstanceSlot = util::StrongIndex<std::uint32_t, struct InstanceSlot__>;

// An index into the array of every class's class variables.
using ClassVariableIndex = util::StrongIndex<std::uint32_t, struct ClassVariableIndex__>;

// Const members don't need to exist at this stage of the compiler, we can just use their constant expression in their
// place.
using MemberLocation = std::variant<InstanceSlot, ClassVariableIndex, ir::OptionalConstExprIndex>;

// Every member of every compiled class, by owning class and name, as MemberVariable finds them. A class variable's or
// constant's owner is the meta class.
struct MemberTable {
    MemberTable(const ir::IRCompiler& compiler, const ir::SymbolTable& symbols);

    [[nodiscard]] std::optional<MemberLocation> find(ir::SymSCClassName owner, ir::SymSCIdentifier name) const;

    // The index of a meta class's first class variable. Classes are numbered in name order, so it doesn't depend on how
    // they are hashed or compiled. The loader must give each class the same.
    [[nodiscard]] ClassVariableIndex class_variable_base(ir::SymSCClassName meta) const;

    // Total number of class variables in the library.
    [[nodiscard]] std::uint32_t num_class_variables() const { return m_num_class_variables; }

private:
    using MemberLocationKey = std::tuple<ir::SymSCClassName, ir::SymSCIdentifier>;

    struct MemberLocationKeyHash {
        [[nodiscard]] std::size_t operator()(const MemberLocationKey& k) const {
            return std::hash<std::uint64_t> {}((static_cast<std::uint64_t>(*std::get<0>(k)) << 32) | *std::get<1>(k));
        }
    };
    [[nodiscard]] static MemberLocationKey key(ir::SymSCClassName owner, ir::SymSCIdentifier name) {
        return { owner, name };
    }

    std::unordered_map<MemberLocationKey, MemberLocation, MemberLocationKeyHash> m_members {};
    std::unordered_map<ir::SymSCClassName, ClassVariableIndex, std::hash<ir::Symbol>> m_class_variable_bases {};

    std::uint32_t m_num_class_variables {};
};

}
