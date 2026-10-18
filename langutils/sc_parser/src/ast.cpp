// Copyright Jordan Henderson 2026
#include "sc_parser/ast.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_util/typed_graph.hpp"
#include <ostream>
namespace sc::util::typed_graph {
template struct sc::util::typed_graph::GraphImplementation<sc::ast::OptionalIndex, sc::ast::Index,
                                                           sc::ast::NodeCollectionHelper>;
}

namespace sc::ast {

void ASTGraph::add_diagnostic(sc::diag::Diagnostic d) {
    m_has_fatal_diagnostic |= d.fatal();
    m_diagnostics.push_back(std::move(d));
}

[[nodiscard]] const std::vector<sc::diag::Diagnostic>& ASTGraph::diagnostics() const& { return m_diagnostics; }

[[nodiscard]] std::vector<sc::diag::Diagnostic> ASTGraph::diagnostics() && { return std::move(m_diagnostics); }


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


} // sc::parser::graph
