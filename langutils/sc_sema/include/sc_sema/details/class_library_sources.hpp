#pragma once

#include "sc_parser/details/indexes_typed.hpp"
#include "sc_sema/details/indexes.hpp"
#include "sc_util/indexed_vector.hpp"

#include <memory>
#include <optional>

namespace sc::ast {
struct ASTGraph;
}

namespace sc::ir {

struct SourceEntry {
    std::shared_ptr<const ast::ASTGraph> graph;
    ast::ClassOrExtensionListIndex index;
    ModuleIndex module;
};

// Each file of the class library, empty when it didn't parse.
using ClassLibrarySources = util::IndexedVector<SourceEntryIndex, std::optional<SourceEntry>>;

}
