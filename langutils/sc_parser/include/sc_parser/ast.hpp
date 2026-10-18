// Copyright Jordan Henderson 2026
#pragma once

#include "indexes_typed.hpp"
#include "sc_lexer/text_info.hpp"
#include "sc_lexer/text_location.hpp"
#include "nodes.hpp"

#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_util/typed_graph.hpp"

#include <functional>
#include <memory>
#include <type_traits>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>


namespace sc::ast {
using NodeCollectionHelper = sc::util::typed_graph::TypedGraphHelper< //
    NodeFlag, //
    TypedIndex, //
    Missing, //
    ASCIINode, //
    IntNode, //
    FloatNode, //
    PiNode, //
    AccidentalNode, //
    StringLineNode, //
    StringLineList, //


    KeyBinOp, //
    SymbolSlash, //
    SymbolQuote, //

    BooleanNode, //
    NilNode, //
    CurryNode, //
    BlockList, //
    ArrayNode, //
    NamedIdentifier, //
    PrimitiveIdentifier, //
    ClassNameIdentifier, //

    NameList, //
    NamePack, //

    EnvIdentifierNode, //
    SelectorNode, //
    SelectorWAdverb, //
    VariadicArgNode, //
    ArgumentList, //
    BlockNode, //
    KwArgNode, //
    ExprSeq, //

    Range, //
    RangeGenerator, //
    RangeAssign, //

    ImmediatelyEvaluatedBlock, //

    GeneratorNode, //
    GeneratorFakeNode, //
    GeneratorQualList, //
    GeneratorQualAssign, //
    GeneratorQualDeclare, //
    GeneratorQualIf, //
    GeneratorQualSideEffect, //
    GeneratorQualGuard, //

    PrefixMessage, //
    PostfixMessage, //
    BinaryMessage, //
    ValueMessage, //
    NewMessage, //
    AtMessage, //

    AdverbExprNode, //
    ReferenceNode, //
    AssignmentNode, //
    AssignmentAtNode, //
    SetterNode, //
    DictionaryNode, //

    DictionaryEntryNode, //
    CollectionNode, //
    DeclareArgumentWithDefaultNode, //
    DeclareArgumentVariadicNode, //

    DeclareArgumentList, //

    DeclareVariableWithDefaultNode, //
    VariablePackAssign,

    DeclareVariableList, //

    BlockContentsList, //
    NonLocalReturnExpr, //

    Method, //
    ClassMethod, //
    Class, //
    ClassExtension, //
    RWAccessor,
    BigMemberList, //
    DeclareMemberList, //
    DeclareClassMemberList, //

    DeclareConstList, //
    DeclareMember, //
    MethodList, //
    ClassOrExtensionList, //
    FileAsFunction, //
    RegionList, //
    Error //
    >;
}


namespace sc::util::typed_graph {
extern template struct sc::util::typed_graph::GraphImplementation<sc::ast::OptionalIndex, sc::ast::Index,
                                                                  sc::ast::NodeCollectionHelper>;
}

namespace sc::ast {

namespace priv {
template <typename... Os> struct overload : Os... {
    using Os::operator()...;
};

template <typename... Os> overload(Os...) -> overload<Os...>;
}

////////////////////////////////////////////////////////////////////////////////

class ASTGraph : private sc::util::typed_graph::GraphImplementation<OptionalIndex, Index, NodeCollectionHelper> {
    using Base = sc::util::typed_graph::GraphImplementation<OptionalIndex, Index, NodeCollectionHelper>;
    using Helper = Base::Helper;

public:
    ASTGraph(std::shared_ptr<const lex::TextInfo> text_info): m_text_info(text_info) { }
    ASTGraph(const ASTGraph&) = default;
    ASTGraph(ASTGraph&&) noexcept = default;
    ASTGraph& operator=(const ASTGraph&) = delete;
    ASTGraph& operator=(ASTGraph&&) = delete;

    using Edges = Base::Edges;
    using Variant = Base::Variant;

    template <typename I> using NodeTypeFromIndex = Base::NodeTypeFromIndex<I>;

    ////////////////////////////////////////////////////////////////////////////////
    // Create
    ////////////////////////////////////////////////////////////////////////////////
    template <typename Node, typename... ChildrenIndexes>
    [[nodiscard]] typename Node::Index create(Node&& node, lex::SourceCodeRange location,
                                              ChildrenIndexes&&... child_indexes);

    ////////////////////////////////////////////////////////////////////////////////
    // Creating Edges
    ////////////////////////////////////////////////////////////////////////////////

    using Base::append;
    using Base::merge;
    using Base::prepend;

    template <typename I, typename... Children> //
    auto append(I p, lex::SourceCodeRange range, Children... children);

    ////////////////////////////////////////////////////////////////////////////////
    // node type operations
    ////////////////////////////////////////////////////////////////////////////////

    using Base::as_a;
    using Base::cast;
    using Base::is_a;

    template <typename TypedIndex, typename = std::enable_if<sc::util::typed_index::is_type_set_index<TypedIndex>()>>
    [[nodiscard]] static constexpr const char* node_name(TypedIndex);
    [[nodiscard]] static constexpr const char* node_name(const Variant& variant);

    /// Turns as missing<NODEINDEX> into a std::optional<NODEINDEX>
    template <typename TypedIndex, typename = std::enable_if<sc::util::typed_index::is_type_set_index<TypedIndex>()>>
    [[nodiscard]] constexpr auto present(TypedIndex i) const
        -> std::optional<typename decltype(TypedIndex::template remove_from_set<NodeFlag::Missing>())::type>;


    ////////////////////////////////////////////////////////////////////////////////
    // accessors
    ////////////////////////////////////////////////////////////////////////////////

    using Base::edges;
    using Base::last_child;
    using Base::orphans;
    using Base::payload;

    const sc::lex::SourceCodeRange& location(UntypedNodeIndex i) const { return m_node_locations[*i]; }
    sc::lex::SourceCodeRange& location(UntypedNodeIndex i) { return m_node_locations[*i]; }

    [[nodiscard]] std::shared_ptr<const lex::TextInfo> text_info() const { return m_text_info; }

    [[nodiscard]] std::string_view string_view(UntypedNodeIndex i) const { return m_text_info->read(location(i)); }
    [[nodiscard]] std::string string(UntypedNodeIndex i) const {
        return std::string { m_text_info->read(location(i)) };
    }

    void register_bracketed_expr(UntypedNodeIndex i) { m_bracketed_exprs.insert(i); }

    [[nodiscard]] bool is_bracketed(UntypedNodeIndex i) const {
        return m_bracketed_exprs.find(i) != m_bracketed_exprs.end();
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Children. This is the type safe way to walk the graph.
    ////////////////////////////////////////////////////////////////////////////////

    using Base::children;
    using Base::index_to_variant;
    using Base::list_unpack;
    using Base::num_children;
    using Base::visit;

    ////////////////////////////////////////////////////////////////////////////////
    // Roots
    ////////////////////////////////////////////////////////////////////////////////

    template <typename I> I assign_root(I i) {
        assert(i);
        m_roots = i;
        return i;
    }

    [[nodiscard]] std::optional<Index> root_any() const;

    [[nodiscard]] std::optional<std::variant<RegionListIndex, ClassOrExtensionListIndex, FileAsFunctionIndex>>
    root() const;

    ////////////////////////////////////////////////////////////////////////////////
    // Diagnostics
    ////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] explicit operator bool() const;
    void add_diagnostic(sc::diag::Diagnostic d);
    [[nodiscard]] const std::vector<sc::diag::Diagnostic>& diagnostics() const&;
    [[nodiscard]] std::vector<sc::diag::Diagnostic> diagnostics() &&;

    ////////////////////////////////////////////////////////////////////////////////
    // Traversal
    ////////////////////////////////////////////////////////////////////////////////
    template <typename F> [[nodiscard]] static constexpr bool signature_check() {
        return std::is_invocable_v<F, const Edges&, const Variant&, const lex::SourceCodeRange&, Index>;
    }


    template <typename F, typename = std::enable_if<signature_check<F>()>> //
    void flat_walk(F f) const;

    template <typename F, typename = std::enable_if<signature_check<F>()>> //
    void traverse_only_children(F& f, Index i);

    struct Default {
        void operator()(const Edges&, const Variant&, const lex::SourceCodeRange&, Index, size_t /*depth*/) { }
    };


    /**
    @brief Expects functions of the signature:
        (const Edges& e, const Variant& v, const lex::SourceCodeRange&, Index i, size_t depth)
    */
    template <typename EnterNode, typename BeforeChildren = Default, typename AfterChildren = Default,
              typename ExitNode = Default,
              typename = std::enable_if<signature_check<EnterNode>()>, //
              typename = std::enable_if<signature_check<BeforeChildren>()>, //
              typename = std::enable_if<signature_check<AfterChildren>()>, //
              typename = std::enable_if<signature_check<ExitNode>()>>

    void depth_first_traverse(Index i, EnterNode enter_node, BeforeChildren before_children = {},
                              AfterChildren after_children = {}, ExitNode exit_node = {}) const;

    void debug_df_print(std::ostream&) const;
    void debug_flat_print(std::ostream&) const;
    using Base::debug_find_loop;


private:
    std::shared_ptr<const lex::TextInfo> m_text_info;
    std::vector<lex::SourceCodeRange> m_node_locations {};

    std::vector<sc::diag::Diagnostic> m_diagnostics { };

    bool m_has_fatal_diagnostic { false };

    std::variant<std::monostate, RegionListIndex, ClassOrExtensionListIndex, FileAsFunctionIndex> m_roots {};

    std::unordered_set<UntypedNodeIndex> m_bracketed_exprs {};
};

////////////////////////////////////////////////////////////////////////////////

struct ASTGraphAndValidIndex {
    ASTGraph graph;
    ClassOrExtensionListIndex index;
};

////////////////////////////////////////////////////////////////////////////////


template <typename TypedIndex, typename>
[[nodiscard]] constexpr auto ASTGraph::present(TypedIndex i) const
    -> std::optional<typename decltype(TypedIndex::template remove_from_set<NodeFlag::Missing>())::type> //
{
    return is_a<MissingIndex>(i) ? std::nullopt : std::optional { *i };
}

template <typename F, typename> inline void ASTGraph::flat_walk(F f) const {
    Base::flat_walk([&](const Edges& e, const Variant& v, size_t i) { std::invoke(f, e, v, m_node_locations[i], i); });
}

template <typename TypedIndex, typename> //
[[nodiscard]] constexpr const char* ASTGraph::node_name(TypedIndex) {
    using NodeType = NodeTypeFromIndex<TypedIndex>;
    return NodeType::name;
}

template <typename I, typename... Children>
inline auto ASTGraph::append(I p, lex::SourceCodeRange range, Children... children) {
    (Base::append(p, children), ...);
    // At this point we know this is valid because the base does the checks
    m_node_locations[*p] = std::move(range);
    return p;
};

template <typename EnterNode, typename BeforeChildren, typename AfterChildren, typename ExitNode, typename, typename,
          typename, typename>
inline void ASTGraph::depth_first_traverse(Index i, EnterNode enter_node, BeforeChildren before_children,
                                           AfterChildren after_children, ExitNode exit_node) const {
    const auto wrap = [&](auto& f) {
        return [&](const Edges& e, const Variant& v, Index index, size_t d) {
            f(e, v, m_node_locations[*index], index, d);
        };
    };
    Base::depth_first_traverse(i, wrap(enter_node), wrap(before_children), wrap(after_children), wrap(exit_node));
}


template <typename Node, typename... ChildrenIndexes>
typename Node::Index ASTGraph::create(Node&& node, lex::SourceCodeRange location, ChildrenIndexes&&... child_indexes) {
    const auto index = Base::create(std::forward<Node>(node), std::forward<ChildrenIndexes>(child_indexes)...);

    if (*index < m_node_locations.size()) {
        m_node_locations[*index] = std::move(location);
    } else {
        assert(*index == m_node_locations.size());
        m_node_locations.push_back(std::move(location));
    }

    return index;
}

template <typename F, typename> inline void ASTGraph::traverse_only_children(F& f, Index i) {
    Base::traverse_only_children(
        [&](const Edges& e, const Variant& v, Index index) { f(e, v, m_node_locations[*index], index); }, i);
}

[[nodiscard]] constexpr const char* ASTGraph::node_name(const Variant& variant) {
    return std::visit(
        [](const auto& v) -> const char* {
            using V = std::remove_reference_t<std::remove_cv_t<decltype(v)>>;
            return V::name;
        },
        variant);
};

}
