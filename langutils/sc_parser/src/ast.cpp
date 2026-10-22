// Copyright Jordan Henderson 2026
#include "sc_parser/ast.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_util/typed_graph.hpp"
#include "sc_parser/ast_untyped.hpp"
#include <ostream>
namespace sc::util::typed_graph {
template struct sc::util::typed_graph::GraphImplementation<sc::ast::OptionalIndex, sc::ast::Index,
                                                           sc::ast::NodeCollectionHelper>;
}

namespace sc::ast {

[[nodiscard]] const IntNode& payload(const ASTGraph& g, IntLitIndex i) { return g.payload(i); }

[[nodiscard]] const FloatNode& payload(const ASTGraph& g, FloatLitIndex i) { return g.payload(i); }

AnyExprIndex::Variant to_variant(const ASTGraph& g, AnyExprIndex i) { return g.index_to_variant(i); }
BlockItemIndex::Variant to_variant(const ASTGraph& g, BlockItemIndex i) { return g.index_to_variant(i); }

std::shared_ptr<const lex::TextInfo> get_text_info(const ASTGraph& graph) { return graph.text_info(); }

sc::lex::SourceCodeRange get_location(const ASTGraph& graph, OptionalIndex i) { return graph.location(i); }

std::string_view get_string_view(const ASTGraph& graph, OptionalIndex i) { return graph.string_view(i); }
std::string get_string(const ASTGraph& graph, OptionalIndex i) { return std::string { graph.string_view(i) }; }


[[nodiscard]] OptionalIndex next_sibling(const ASTGraph& g, OptionalIndex i) { return g.edges(i).next_sibling; }


[[nodiscard]] Iter<OptionalIndex, AnyExprIndex> iter(const ASTGraph& g, ExprSeqIndex i) { return g.children(i).iter(); }


void ASTGraph::add_diagnostic(sc::diag::Diagnostic d) {
    m_has_fatal_diagnostic |= d.fatal();
    m_diagnostics.push_back(std::move(d));
}

[[nodiscard]] const std::vector<sc::diag::Diagnostic>& ASTGraph::diagnostics() const& { return m_diagnostics; }

[[nodiscard]] std::vector<sc::diag::Diagnostic> ASTGraph::diagnostics() && { return std::move(m_diagnostics); }


RegionListIndex ASTGraph::assign_root(RegionListIndex i) {
    assert(i);
    m_roots = i;
    return i;
}
ClassOrExtensionListIndex ASTGraph::assign_root(ClassOrExtensionListIndex i) {
    assert(i);
    m_roots = i;
    return i;
}
FileAsFunctionIndex ASTGraph::assign_root(FileAsFunctionIndex i) {
    assert(i);
    m_roots = i;
    return i;
}


[[nodiscard]] std::optional<Index> ASTGraph::root_any() const {
    return std::visit(priv::overload {
                          [](std::monostate) -> std::optional<Index> { return std::nullopt; },
                          [](auto a) -> std::optional<Index> { return Index { *a }; },
                      },
                      m_roots);
}

template <typename... Ts> struct overload : Ts... {
    using Ts::operator()...;
};
template <typename... Ts> overload(Ts... ts) -> overload<Ts...>;

[[nodiscard]] std::optional<std::variant<RegionListIndex, ClassOrExtensionListIndex, FileAsFunctionIndex>>
ASTGraph::root() const {
    using R = std::optional<std::variant<RegionListIndex, ClassOrExtensionListIndex, FileAsFunctionIndex>>;
    return std::visit(overload {
                          [](std::monostate) -> R { return std::nullopt; },
                          [](auto i) -> R { return { i }; },
                      },
                      m_roots);
};


[[nodiscard]] ASTGraph::operator bool() const { return !m_has_fatal_diagnostic; }


void ASTGraph::debug_df_print(std::ostream& stream) const {
    const auto r = root();
    if (!r) {
        stream << "EMPTY\n";
        return;
    }
    const Index i = std::visit(overload { [](auto i) { return Index { *i }; } }, *r);

    depth_first_traverse(
        i, [&](const Edges& e, const Variant& payload, const sc::lex::SourceCodeRange& loc, Index i, size_t depth) {
            for (size_t di { 0 }; di < depth; ++di)
                stream << "|  ";

            stream << node_name(payload) << ": ";

            const auto view = text_info()->read(loc);
            for (size_t i { 0 }; i < view.size(); ++i) {
                if (view[i] == '\n') {
                    stream << "'\\n'";
                } else {
                    stream.write(view.begin() + i, 1);
                }
            }
            stream << "\n";
        });
}

void ASTGraph::debug_flat_print(std::ostream& stream) const {
    flat_walk([&](const Edges& e, const Variant& payload, const lex::SourceCodeRange&, Index i) {
        stream << *i << " " << node_name(payload) << ": "
               << "parent: " << *e.parent << " first_child: " << *e.first_child << " next_sib: " << *e.next_sibling
               << " prev_sib: " << *e.prev_sibling

               << '\n';
    });
}

const sc::lex::SourceCodeRange& ASTGraph::location(UntypedNodeIndex i) const { return m_node_locations[*i]; }
sc::lex::SourceCodeRange& ASTGraph::location(UntypedNodeIndex i) { return m_node_locations[*i]; }
std::shared_ptr<const lex::TextInfo> ASTGraph::text_info() const { return m_text_info; }
std::string_view ASTGraph::string_view(UntypedNodeIndex i) const { return m_text_info->read(location(i)); }
std::string ASTGraph::string(UntypedNodeIndex i) const { return std::string { m_text_info->read(location(i)) }; }
void ASTGraph::register_bracketed_expr(UntypedNodeIndex i) { m_bracketed_exprs.insert(i); }
bool ASTGraph::is_bracketed(UntypedNodeIndex i) const { return m_bracketed_exprs.find(i) != m_bracketed_exprs.end(); };


} // sc::parser::graph
