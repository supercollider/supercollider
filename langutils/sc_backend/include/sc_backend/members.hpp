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

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

using InstanceSlotIndex = util::StrongIndex<std::uint32_t, struct InstanceSlot__>;
// Numbered by meta class name, which the loader must follow.
using ClassVariableIndex = util::StrongIndex<std::uint32_t, struct ClassVariableIndex__>;

// A constant has no storage, it is pushed.
using MemberLocation = std::variant<InstanceSlotIndex, ClassVariableIndex, ir::OptionalConstExprIndex>;
static_assert(sizeof(MemberLocation) == 8);

struct MemberTable {
    MemberTable(const ir::IRCompiler& compiler, const ir::SymbolTable& symbols);

    [[nodiscard]] std::optional<MemberLocation> find(ir::SymSCClassName owner, ir::SymSCIdentifier name) const;
    [[nodiscard]] ClassVariableIndex class_variable_base(ir::SymSCClassName meta) const;
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
