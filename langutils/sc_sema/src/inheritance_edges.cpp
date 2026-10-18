#include "sc_sema/context.hpp"

namespace sc::ir {

void InheritanceEdges::insert(SymSCClassName parent, SymSCClassName child) {
    std::scoped_lock l { m_lock };
    m_sub_classes[parent].push_back(child);
    // ensure child exists
    const auto& _ = m_sub_classes[child];

    // This means we
    assert(m_super_class.count(child) == 0);
    m_super_class.insert({ child, parent });
}

std::vector<std::vector<SymSCClassName>> InheritanceEdges::cycle_check(SymSCClassName root) const {
    std::scoped_lock l { m_lock };
    // Because root is empty, and we have single inheritance, it is impossible to create cycles that are attached to
    // root.
    std::unordered_set<SymSCClassName, std::hash<Symbol>> not_visited, visited;

    const auto dfs = [&](auto&& self, SymSCClassName n) -> void {
        assert(visited.count(n) == 0);
        not_visited.erase(n);
        visited.insert(n);

        for (const SymSCClassName child : m_sub_classes.at(n))
            self(self, child);
    };

    for (const auto& [k, v] : m_sub_classes) {
        not_visited.insert(k);
    }

    dfs(dfs, root);

    std::vector<std::vector<SymSCClassName>> loops;

    const auto loop_maker = [&](auto&& self, SymSCClassName n, std::vector<SymSCClassName>& current_cycle) -> void {
        // This is the end of a loop.
        assert(not_visited.count(n));
        current_cycle.push_back(n);
        not_visited.erase(n);

        for (const SymSCClassName child : m_sub_classes.at(n)) {
            if (not_visited.count(child) == 0)
                continue;
            self(self, child, current_cycle);
        }
    };

    // Effectively pulls a random class from the not_visited set and returns the cycle it is connected to.
    // Repeats until there are no more.
    while (!not_visited.empty()) {
        std::vector<SymSCClassName> cycles;
        SymSCClassName n = *not_visited.begin();
        std::unordered_set<SymSCClassName, std::hash<Symbol>> parent_visited;
        parent_visited.insert(n);

        while (true) {
            auto fnd = m_super_class.find(n);
            if (fnd == m_super_class.end())
                break;
            SymSCClassName s = fnd->second;
            if (parent_visited.count(s)) {
                break;
            }
            parent_visited.insert(s);
            n = s;
        }

        loop_maker(loop_maker, n, cycles);
        loops.push_back(std::move(cycles));
    }

    return loops;
}

const std::vector<SymSCClassName>& InheritanceEdges::get_unsafe(SymSCClassName n) const {
    assert(m_sub_classes.count(n));
    return m_sub_classes.at(n);
}

[[nodiscard]] std::optional<ClassNames::V> ClassNames::encountered_class_before(SymSCClassName n,
                                                                                std::shared_ptr<const ast::ASTGraph> g,
                                                                                ast::ClassNameIdentifierIndex i) {
    std::scoped_lock lk { m_lock };
    if (auto fnd = m_class_locations.find(n); fnd != m_class_locations.end())
        return { fnd->second };
    m_class_locations.insert({ n, std::pair { g, i } });
    return {};
}
}