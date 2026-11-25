// Copyright Jordan Henderson 2026
#include "rewrite/rewriter.hpp"

#include <cstddef>
#include <optional>
#include <type_traits>
#include <variant>

namespace sc::format {

namespace {

using lex::TokenType;

// The fields a node holds besides its children and position.
template <typename T> bool same_fields(const T&, const T&) { return true; }
bool same_fields(const ast::IntNode& original, const ast::IntNode& rewritten) {
    return original.kind == rewritten.kind && original.sign == rewritten.sign;
}
bool same_fields(const ast::FloatNode& original, const ast::FloatNode& rewritten) {
    return original.kind == rewritten.kind && original.sign == rewritten.sign;
}
bool same_fields(const ast::PiNode& original, const ast::PiNode& rewritten) { return original.sign == rewritten.sign; }
bool same_fields(const ast::AccidentalNode& original, const ast::AccidentalNode& rewritten) {
    return original.kind == rewritten.kind && original.sign == rewritten.sign;
}
bool same_fields(const ast::BooleanNode& original, const ast::BooleanNode& rewritten) {
    return original.value == rewritten.value;
}
bool same_fields(const ast::ArrayNode& original, const ast::ArrayNode& rewritten) {
    return original.is_immutable == rewritten.is_immutable;
}
bool same_fields(const ast::SelectorNode& original, const ast::SelectorNode& rewritten) {
    return original.is_infix_keyword == rewritten.is_infix_keyword;
}
bool same_fields(const ast::AssignmentNode& original, const ast::AssignmentNode& rewritten) {
    return original.target == rewritten.target;
}
bool same_fields(const ast::BlockNode& original, const ast::BlockNode& rewritten) {
    return original.closed == rewritten.closed;
}
bool same_fields(const ast::DeclareArgumentWithDefaultNode& original,
                 const ast::DeclareArgumentWithDefaultNode& rewritten) {
    return original.bracketed == rewritten.bracketed;
}
bool same_fields(const ast::RWAccessor& original, const ast::RWAccessor& rewritten) {
    return original.accessor == rewritten.accessor;
}
bool same_fields(const ast::Class& original, const ast::Class& rewritten) {
    return original.version == rewritten.version;
}
bool same_fields(const ast::ClassExtension& original, const ast::ClassExtension& rewritten) {
    return original.version == rewritten.version;
}

bool is_message(const ast::ASTGraph::Variant& node) {
    return std::holds_alternative<ast::PrefixMessage>(node) || std::holds_alternative<ast::PostfixMessage>(node);
}

// Compares the trees node by node, allowing the changes the rewrites may make.
class AstComparer {
public:
    AstComparer(const ast::ASTGraph& original, const ast::ASTGraph& rewritten, AllowedAstChanges allowed):
        m_original(original),
        m_rewritten(rewritten),
        m_allowed(allowed) {}

    bool run() const {
        const auto original_root = m_original.root_any();
        const auto rewritten_root = m_rewritten.root_any();
        if (!original_root || !rewritten_root)
            return !original_root && !rewritten_root;
        // Depth first with a stack, as recursion overflows on deeply nested files.
        std::vector<NodePair> pending_pairs { { *original_root, *rewritten_root, false } };
        while (!pending_pairs.empty()) {
            const auto pair = pending_pairs.back();
            pending_pairs.pop_back();
            if (!same_node(pair, pending_pairs))
                return false;
        }
        return true;
    }

private:
    struct NodePair {
        ast::Index original;
        ast::Index rewritten;
        bool brackets_may_differ; // a parameter's default with pipes, a control message's receiver
    };

    // Compares the nodes, not their children, and adds the pairs of children to compare.
    bool same_node(NodePair pair, std::vector<NodePair>& pending_pairs) const {
        const auto original_node = pair.original;
        auto rewritten_node = pair.rewritten;
        if (m_allowed.switch_tests && is_switch_test(m_original.payload(original_node)))
            if (const auto inside = braced(rewritten_node))
                rewritten_node = *inside;
        const auto& original_payload = m_original.payload(original_node);
        const auto& rewritten_payload = m_rewritten.payload(rewritten_node);
        if (kind(original_payload) != kind(rewritten_payload) || !same_node_fields(original_payload, rewritten_payload))
            return false;
        if (!pair.brackets_may_differ
            && m_original.is_bracketed(original_node) != m_rewritten.is_bracketed(rewritten_node))
            return false;
        const auto original_children = children_of(m_original, original_node);
        const auto rewritten_children = children_of(m_rewritten, rewritten_node);
        if (original_children.size() != rewritten_children.size())
            return false;
        if (original_children.empty()
            && m_original.string_view(original_node) != m_rewritten.string_view(rewritten_node))
            return false;
        // A declaration's default is its second child, as is a message's receiver after the selector.
        const bool default_brackets_may_differ =
            m_allowed.default_brackets && std::holds_alternative<ast::DeclareArgumentWithDefaultNode>(original_payload);
        const bool receiver_brackets_may_differ = m_allowed.control_prefix && is_message(original_payload);
        const bool second_child_brackets_may_differ = default_brackets_may_differ || receiver_brackets_may_differ;
        for (std::size_t child = 0; child < original_children.size(); ++child)
            pending_pairs.push_back({ original_children[child], rewritten_children[child],
                                      second_child_brackets_may_differ && child == 1 });
        return true;
    }

    // With the control prefix, a postfix message compares as a prefix one.
    std::size_t kind(const ast::ASTGraph::Variant& node) const {
        return m_allowed.control_prefix && std::holds_alternative<ast::PostfixMessage>(node)
            ? ast::ASTGraph::Variant { ast::PrefixMessage {} }.index()
            : node.index();
    }

    // True for nodes of different types, which kind() matched.
    bool same_node_fields(const ast::ASTGraph::Variant& original_payload,
                          const ast::ASTGraph::Variant& rewritten_payload) const {
        if (original_payload.index() != rewritten_payload.index())
            return true;
        return std::visit(
            [&](const auto& original_alternative) {
                using T = std::decay_t<decltype(original_alternative)>;
                if constexpr (std::is_same_v<T, ast::DeclareArgumentWithDefaultNode>)
                    if (m_allowed.default_brackets)
                        return true;
                return same_fields(original_alternative, std::get<T>(rewritten_payload));
            },
            original_payload);
    }

    // A node's children. With trailing closures, a message's arguments and functions are one list. With the control
    // prefix, `x.if(a)` is `if(x, a)`: the selector, then the receiver and arguments as one list.
    std::vector<ast::Index> children_of(const ast::ASTGraph& graph, ast::Index parent) const {
        const auto& parent_node = graph.payload(parent);
        const bool flatten_blocks = m_allowed.trailing_closures
            && holds_any<ast::PrefixMessage, ast::PostfixMessage, ast::NewMessage>(parent_node);
        const bool flatten_arguments = flatten_blocks || (m_allowed.control_prefix && is_message(parent_node));
        const auto is_flattened = [&](const ast::ASTGraph::Variant& child_node) {
            if (std::holds_alternative<ast::ArgumentList>(child_node))
                return flatten_arguments;
            return flatten_blocks && std::holds_alternative<ast::BlockList>(child_node);
        };
        std::vector<ast::Index> children;
        for (auto child_link = graph.edges(parent).first_child; child_link;
             child_link = graph.edges(ast::Index { *child_link }).next_sibling) {
            const ast::Index child { *child_link };
            if (is_flattened(graph.payload(child)))
                for (auto grandchild_link = graph.edges(child).first_child; grandchild_link;
                     grandchild_link = graph.edges(ast::Index { *grandchild_link }).next_sibling)
                    children.push_back(ast::Index { *grandchild_link });
            else
                children.push_back(child);
        }
        if (m_allowed.control_prefix && std::holds_alternative<ast::PostfixMessage>(parent_node) && children.size() > 1)
            std::swap(children[0], children[1]);
        return children;
    }

    // `{ \a }`'s expression, where a `switch` test may have gained braces.
    std::optional<ast::Index> braced(ast::Index block) const {
        if (!std::holds_alternative<ast::BlockNode>(m_rewritten.payload(block)))
            return std::nullopt;
        const auto block_parts = children_of(m_rewritten, block);
        if (block_parts.size() != 2)
            return std::nullopt;
        const auto parameters = block_parts[0];
        const auto body = block_parts[1];
        if (m_rewritten.edges(parameters).first_child
            || !std::holds_alternative<ast::BlockContentsList>(m_rewritten.payload(body)))
            return std::nullopt;
        auto inside = children_of(m_rewritten, body);
        while (inside.size() == 1 && std::holds_alternative<ast::ExprSeq>(m_rewritten.payload(inside[0])))
            inside = children_of(m_rewritten, inside[0]);
        if (inside.size() != 1)
            return std::nullopt;
        return inside[0];
    }

    const ast::ASTGraph& m_original;
    const ast::ASTGraph& m_rewritten;
    AllowedAstChanges m_allowed;
};
}

bool same_ast(const ast::ASTGraph& original, const ast::ASTGraph& rewritten, AllowedAstChanges allowed) {
    return AstComparer { original, rewritten, allowed }.run();
}
}
