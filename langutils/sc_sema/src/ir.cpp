#include "sc_sema/ir.hpp"
#include "sc_source/text_info.hpp"
#include "sc_util/typed_graph.hpp"

#include <cassert>

namespace sc::util::typed_graph {

template struct sc::util::typed_graph::GraphImplementation<sc::ir::OptNodeIndex, sc::ir::NodeIndex,
                                                           sc::ir::NodeCollectionHelper>;
}


namespace sc::ir {

const std::shared_ptr<const src::TextInfo>& IRGraph::text_info(SourceIndex i) const {
    assert(m_sources.contains(i));
    return m_sources[i];
}

src::SourceCodeRange IRGraph::range(SourceLocation l) { return l.range; }

std::string_view IRGraph::read(SourceLocation l) const { return text_info(l.source)->read(range(l)); }
}