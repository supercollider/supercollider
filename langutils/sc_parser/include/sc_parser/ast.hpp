// Copyright Jordan Henderson 2026
#pragma once

#include "sc_source/text_info.hpp"
#include "sc_source/text_location.hpp"

#include "details/nodes.hpp"

#include "sc_util/details/typed_graph_helper.hpp"
#include "sc_util/typed_graph.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>


namespace sc::diag {
class Diagnostic;
}

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
template <typename... Os> struct overload : Os... { using Os::operator()...; };

template <typename... Os> overload(Os...) -> overload<Os...>;
}

////////////////////////////////////////////////////////////////////////////////

/// @brief The syntax tree produced by the parser.
///
/// A typed graph of nodes, each with the source range it was parsed from, plus the diagnostics found while parsing.
/// It keeps the text alive. Walk it type safely with children(), visit(), and the typed indexes.
struct ASTGraph : private sc::util::typed_graph::GraphImplementation<OptionalIndex, Index, NodeCollectionHelper> {
    using Base = sc::util::typed_graph::GraphImplementation<OptionalIndex, Index, NodeCollectionHelper>;
    using Helper = Base::Helper;

public:
    // Defined out of line, so sc_diagnostic is only needed where the diagnostics are used.
    ASTGraph(std::shared_ptr<const src::TextInfo> text_info);
    ~ASTGraph();
    ASTGraph(const ASTGraph&) = delete;
    ASTGraph(ASTGraph&&) noexcept;
    ASTGraph& operator=(const ASTGraph&) = delete;
    ASTGraph& operator=(ASTGraph&&) = delete;

    using Edges = Base::Edges;
    using Variant = Base::Variant;

    template <typename I> using NodeTypeFromIndex = Base::NodeTypeFromIndex<I>;

    ////////////////////////////////////////////////////////////////////////////////
    // Create
    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Adds a node to the graph.
    /// @param child_indexes The node's children, which must match what the node type accepts.
    template <typename Node, typename... ChildrenIndexes>
    [[nodiscard]] typename Node::Index create(Node&& node, src::SourceCodeRange location,
                                              ChildrenIndexes&&... child_indexes);

    ////////////////////////////////////////////////////////////////////////////////
    // Creating Edges
    ////////////////////////////////////////////////////////////////////////////////

    using Base::append;
    using Base::merge;
    using Base::prepend;

    /// @brief Appends children to a list node, and updates the list's location.
    template <typename I, typename... Children> //
    auto append(I p, src::SourceCodeRange range, Children... children);

    ////////////////////////////////////////////////////////////////////////////////
    // node type operations
    ////////////////////////////////////////////////////////////////////////////////

    using Base::as_a;
    using Base::assume_a;
    using Base::cast;
    using Base::is_a;

    template <typename TypedIndex> [[nodiscard]] static constexpr const char* node_name(TypedIndex);
    [[nodiscard]] static constexpr const char* node_name(const Variant& variant);

    /// @brief Turns an index that may be Missing, `maybe<I>`, into a `std::optional<I>`.
    template <typename TypedIndex>
    [[nodiscard]] constexpr auto present(TypedIndex i) const
        -> std::optional<typename decltype(TypedIndex::template remove_from_set<NodeFlag::Missing>())::type>;


    ////////////////////////////////////////////////////////////////////////////////
    // accessors
    ////////////////////////////////////////////////////////////////////////////////

    using Base::edges;
    using Base::last_child;
    using Base::orphans;
    using Base::payload;

    const sc::src::SourceCodeRange& location(UntypedNodeIndex i) const { return m_node_locations[*i]; }
    sc::src::SourceCodeRange& location(UntypedNodeIndex i) { return m_node_locations[*i]; }

    [[nodiscard]] const std::shared_ptr<const src::TextInfo>& text_info() const { return m_text_info; }

    /// @brief The source text of a node. Valid while the graph is alive.
    [[nodiscard]] std::string_view string_view(UntypedNodeIndex i) const;
    [[nodiscard]] std::string string(UntypedNodeIndex i) const;

    /// @brief Records that an expression was written in brackets, e.g., `(a + b)`, which can change its meaning in
    /// arguements. This is left over from the old implementation and must be supported.
    void register_bracketed_expr(UntypedNodeIndex i);
    [[nodiscard]] bool is_bracketed(UntypedNodeIndex i) const;

    /// @brief Whether there is a `_` anywhere in the graph.
    /// This is important in interpreted code where checking this later is slow.
    /// Class library code compilation performance matters less.
    [[nodiscard]] bool has_curry() const { return m_has_curry; }

    /// @brief A version comment on the first line of a file, which sets the version of all of it.
    struct FileVersion {
        std::optional<LanguageVersion> version; ///< Nothing if it is malformed or unknown, which has been reported.
        src::SourceCodeRange range;
    };
    /// @brief The file's version comment, if its first line is one.
    [[nodiscard]] const std::optional<FileVersion>& file_version() const { return m_file_version; }
    void set_file_version(FileVersion v) { m_file_version = v; }

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


    /// @name Roots
    /// A graph has one root: interpreted code, a file of classes, or a file written as a function.
    /// @{

    RegionListIndex assign_root(RegionListIndex i);
    ClassOrExtensionListIndex assign_root(ClassOrExtensionListIndex i);
    FileAsFunctionIndex assign_root(FileAsFunctionIndex i);

    [[nodiscard]] std::optional<Index> root_any() const;

    [[nodiscard]] std::optional<std::variant<RegionListIndex, ClassOrExtensionListIndex, FileAsFunctionIndex>>
    root() const;
    /// @}

    ////////////////////////////////////////////////////////////////////////////////
    // Diagnostics
    ////////////////////////////////////////////////////////////////////////////////

    /// @name Diagnostics
    /// @{

    /// @brief Whether no fatal diagnostic has been added.
    [[nodiscard]] explicit operator bool() const;

    void add_diagnostic(sc::diag::Diagnostic d);
    [[nodiscard]] const std::vector<sc::diag::Diagnostic>& diagnostics() const&;
    [[nodiscard]] std::vector<sc::diag::Diagnostic> diagnostics() &&;
    /// @}

    ////////////////////////////////////////////////////////////////////////////////
    // Traversal
    ////////////////////////////////////////////////////////////////////////////////
    /// @name Traversal
    /// @{


    /// @brief Calls @p f for every node, in the order they are stored rather than the order of the tree.
    template <typename F> //
    void flat_walk(F f) const;

    template <typename F> //
    void traverse_only_children(F& f, Index i);

    struct Default {
        void operator()(const Edges&, const Variant&, const src::SourceCodeRange&, Index, size_t /*depth*/) {}
    };


    /// @brief Walks the tree from a node, depth first.
    ///
    /// All the callbacks have the signature
    /// `(const Edges& e, const Variant& v, const src::SourceCodeRange&, Index i, size_t depth)`.
    ///
    /// Siblings of the start node are visited too, as a list is walked by looping rather than recursing.
    template <typename EnterNode, typename BeforeChildren = Default, typename AfterChildren = Default,
              typename ExitNode = Default>

    void depth_first_traverse(Index i, EnterNode enter_node, BeforeChildren before_children = {},
                              AfterChildren after_children = {}, ExitNode exit_node = {}) const;

    void debug_df_print(std::ostream&) const;
    void debug_flat_print(std::ostream&) const;
    using Base::debug_find_loop;


    /// @}
private:
    std::shared_ptr<const src::TextInfo> m_text_info;
    std::vector<src::SourceCodeRange> m_node_locations {};

    // No initialiser, as that would need Diagnostic to be complete here.
    std::vector<sc::diag::Diagnostic> m_diagnostics;

    bool m_has_fatal_diagnostic { false };

    bool m_has_curry { false };

    std::optional<FileVersion> m_file_version {};

    std::variant<std::monostate, RegionListIndex, ClassOrExtensionListIndex, FileAsFunctionIndex> m_roots {};

    std::unordered_set<UntypedNodeIndex> m_bracketed_exprs {};
};

////////////////////////////////////////////////////////////////////////////////

/// @brief A graph together with the index of its root.
struct ASTGraphAndValidIndex {
    ASTGraph graph;
    ClassOrExtensionListIndex index;
};

////////////////////////////////////////////////////////////////////////////////


template <typename TypedIndex>
[[nodiscard]] constexpr auto ASTGraph::present(TypedIndex i) const
    -> std::optional<typename decltype(TypedIndex::template remove_from_set<NodeFlag::Missing>())::type> //
{
    return is_a<MissingIndex>(i) ? std::nullopt : std::optional { *i };
}

template <typename F> inline void ASTGraph::flat_walk(F f) const {
    Base::flat_walk([&](const Edges& e, const Variant& v, size_t i) { std::invoke(f, e, v, m_node_locations[i], i); });
}

template <typename TypedIndex> //
[[nodiscard]] constexpr const char* ASTGraph::node_name(TypedIndex) {
    using NodeType = NodeTypeFromIndex<TypedIndex>;
    return NodeType::name;
}

constexpr const char* ASTGraph::node_name(const Variant& variant) {
    return std::visit(
        [](const auto& v) -> const char* {
            using V = std::remove_reference_t<std::remove_cv_t<decltype(v)>>;
            return V::name;
        },
        variant);
}

template <typename I, typename... Children>
inline auto ASTGraph::append(I p, src::SourceCodeRange range, Children... children) {
    (Base::append(p, children), ...);
    // At this point we know this is valid because the base does the checks
    m_node_locations[*p] = std::move(range);
    return p;
};

template <typename EnterNode, typename BeforeChildren, typename AfterChildren, typename ExitNode>
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
typename Node::Index ASTGraph::create(Node&& node, src::SourceCodeRange location, ChildrenIndexes&&... child_indexes) {
    const auto index = Base::create(std::forward<Node>(node), std::forward<ChildrenIndexes>(child_indexes)...);
    if constexpr (std::is_same_v<std::decay_t<Node>, CurryNode>)
        m_has_curry = true;

    if (*index < m_node_locations.size()) {
        m_node_locations[*index] = std::move(location);
    } else {
        assert(*index == m_node_locations.size());
        m_node_locations.push_back(std::move(location));
    }

    return index;
}

template <typename F> inline void ASTGraph::traverse_only_children(F& f, Index i) {
    Base::traverse_only_children(
        [&](const Edges& e, const Variant& v, Index index) { f(e, v, m_node_locations[*index], index); }, i);
}
}
