#pragma once
#include "sc_sema/details/indexes.hpp"

#include <cstdint>
#include <filesystem>

namespace sc::ir {

/// @brief Gives each path given to the compiler a ModuleIndex. A module is just a path.
/// This is currently only loosely implemented.
/// Today, each entry in the lanaguage config will be considered a module.
/// This is only used to help give rules to colliding class extentions.
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