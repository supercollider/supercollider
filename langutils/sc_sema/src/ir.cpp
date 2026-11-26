#include "sc_sema/ir.hpp"
#include "sc_source/text_info.hpp"
#include "sc_util/overload.hpp"
#include "sc_util/typed_graph.hpp"

#include <cassert>

namespace sc::util::typed_graph {

template struct sc::util::typed_graph::GraphImplementation<sc::ir::OptNodeIndex, sc::ir::NodeIndex,
                                                           sc::ir::NodeCollectionHelper>;
}


namespace sc::ir {

[[nodiscard]] const char* IRGraph::node_name(const Variant& variant) {
    return std::visit(
        [](const auto& v) -> const char* {
            using V = std::remove_reference_t<std::remove_cv_t<decltype(v)>>;
            return V::type_name;
        },
        variant);
};
void IRGraph::add_root(SCClass_I c) {
    assert(std::get_if<std::monostate>(&m_roots) != nullptr);
    m_roots = c;
}
void IRGraph::add_root(SCAnyExpr_I c) {
    assert(std::get_if<std::monostate>(&m_roots) != nullptr);
    m_roots = c;
}
void IRGraph::add_root(SCInterpreted_I c) {
    assert(std::get_if<std::monostate>(&m_roots) != nullptr);
    m_roots = c;
}
std::optional<std::variant<SCClass_I, SCAnyExpr_I, SCInterpreted_I>> IRGraph::root() const {
    using R = std::optional<std::variant<SCClass_I, SCAnyExpr_I, SCInterpreted_I>>;
    return std::visit(util::overload {
                          [&](SCClass_I i) -> R { return { i }; },
                          [&](SCAnyExpr_I i) -> R { return { i }; },
                          [&](SCInterpreted_I i) -> R { return { i }; },
                          [&](std::monostate) -> R { return {}; },
                      },
                      m_roots);
}
[[nodiscard]] SourceIndex IRGraph::register_source(const std::shared_ptr<const src::TextInfo>& text) {
    // There are only a few sources per graph, e.g., the class, its extensions, and its base classes.
    for (const auto i : m_sources.indices())
        if (m_sources[i] == text)
            return i;
    return m_sources.push_back(text);
}
[[nodiscard]] SourceLocation IRGraph::make_location(const std::shared_ptr<const src::TextInfo>& text,
                                                    src::SourceCodeRange range) {
    return { register_source(text), range };
}

const std::shared_ptr<const src::TextInfo>& IRGraph::text_info(SourceIndex i) const {
    assert(m_sources.contains(i));
    return m_sources[i];
}
[[nodiscard]] const util::IndexedVector<SourceIndex, std::shared_ptr<const src::TextInfo>>& IRGraph::sources() const {
    return m_sources;
}


src::SourceCodeRange IRGraph::range(SourceLocation l) { return l.range; }

std::string_view IRGraph::read(SourceLocation l) const { return text_info(l.source)->read(range(l)); }
[[nodiscard]] MemberIndex IRGraph::member_index(MemberVariable v) {
    for (const auto i : m_members.indices())
        if (m_members[i].owner == v.owner && m_members[i].name == v.name)
            return i;
    return m_members.push_back(v);
}
void IRGraph::register_node_as_consteval(NodeIndex node, ConstExprIndex expr) {
    nodes_to_constants.insert({ node, expr });
}
[[nodiscard]] std::optional<ConstExprIndex> IRGraph::node_as_constexpr(NodeIndex node) const {
    if (const auto fnd = nodes_to_constants.find(node); fnd != nodes_to_constants.end())
        return { fnd->second };
    else
        return {};
}
}