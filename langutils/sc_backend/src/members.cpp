#include "sc_backend/members.hpp"
#include "sc_sema/compiler.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"

#include <algorithm>
#include <cassert>
#include <vector>

namespace sc::backend {

MemberTable::MemberTable(const ir::IRCompiler& compiler, const ir::SymbolTable& symbols) {
    std::vector<const ir::IRGraph*> meta_classes;
    compiler.depth_first_do([&](const ir::IRGraph& g) {
        const auto class_i = std::get<ir::SCClass_I>(*g.root());
        const auto [members, _] = g.children(class_i);
        std::uint32_t slot { 0 };
        // The instance variables, those of non meta classes, are in the same slot in every class that has them.
        for (auto it = g.children(members).iter(); it; ++it, ++slot) {
            const auto& m = g.payload(*it);
            if (symbols.is_meta(m.owning_class))
                continue;
            const auto [fnd, inserted] = m_members.insert({ key(m.owning_class, m.name), InstanceSlotIndex { slot } });
            assert(inserted || *std::get<InstanceSlotIndex>(fnd->second) == slot);
            (void)fnd;
            (void)inserted;
        }
        if (symbols.is_meta(g.payload(class_i).name))
            meta_classes.push_back(&g);
    });

    const auto name = [&](const ir::IRGraph* g) {
        return symbols(g->payload(std::get<ir::SCClass_I>(*g->root())).name);
    };
    std::sort(meta_classes.begin(), meta_classes.end(), [&](auto* l, auto* r) { return name(l) < name(r); });

    // A meta class's own class variables and constants, those it owns rather than inherits, in the order written.
    for (const auto* g : meta_classes) {
        const auto class_i = std::get<ir::SCClass_I>(*g->root());
        const auto meta = g->payload(class_i).name;
        const auto [members, _] = g->children(class_i);
        m_class_variable_bases.insert({ meta, ClassVariableIndex { m_num_class_variables } });
        for (auto it = g->children(members).iter(); it; ++it) {
            const auto& m = g->payload(*it);
            if (m.owning_class != meta)
                continue;
            if (m.is_const)
                m_members.insert({ key(meta, m.name), m.expr });
            else
                m_members.insert({ key(meta, m.name), ClassVariableIndex { m_num_class_variables++ } });
        }
    }
}

std::optional<MemberLocation> MemberTable::find(ir::SymSCClassName owner, ir::SymSCIdentifier name) const {
    if (const auto fnd = m_members.find(key(owner, name)); fnd != m_members.end())
        return fnd->second;
    return std::nullopt;
}

ClassVariableIndex MemberTable::class_variable_base(ir::SymSCClassName meta) const {
    return m_class_variable_bases.at(meta);
}

}
