#include "sc_sema/ir.hpp"
#include "sc_lexer/text_info.hpp"
#include "sc_util/typed_graph.hpp"

#include <cassert>

namespace sc::util::typed_graph {

template struct sc::util::typed_graph::GraphImplementation<sc::ir::OptNodeIndex, sc::ir::NodeIndex,
                                                           sc::ir::NodeCollectionHelper>;
}


namespace sc::ir {

SourceIndex IRGraph::register_source(std::shared_ptr<const lex::TextInfo> text) {
    // There are only a few sources per graph, e.g., the class, its extensions, and its base classes.
    for (const auto i : m_sources.indices())
        if (m_sources[i] == text)
            return i;
    return m_sources.push_back(std::move(text));
}

SourceLocation IRGraph::make_location(std::shared_ptr<const lex::TextInfo> text, lex::SourceCodeRange range) {
    return { register_source(std::move(text)), range };
}

const std::shared_ptr<const lex::TextInfo>& IRGraph::text_info(SourceIndex i) const {
    assert(m_sources.contains(i));
    return m_sources[i];
}

lex::SourceCodeRange IRGraph::range(SourceLocation l) { return l.range; }

std::string_view IRGraph::read(SourceLocation l) const { return text_info(l.source)->read(range(l)); }
}