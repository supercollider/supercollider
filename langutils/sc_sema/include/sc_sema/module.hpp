#pragma once
#include "sc_util/strong_index.hpp"
#include <cstdint>
#include <filesystem>

namespace sc::ir {
using ModuleIndex = util::StrongIndex<std::uint32_t, struct ModulePath__>;
struct ModuleTable {
    [[nodiscard]] ModuleIndex operator()(const std::filesystem::path& p) {
        if (auto fnd = m_path_to_it.find(p); fnd != m_path_to_it.end())
            return fnd->second;
        const auto index = ModuleIndex { counter };
        m_path_to_it.insert({ p, index });
        counter += 1;
        return index;
    }

private:
    std::uint32_t counter { 0 };
    std::unordered_map<std::filesystem::path, ModuleIndex> m_path_to_it;
};


}