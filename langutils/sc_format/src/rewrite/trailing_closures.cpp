// Copyright Jordan Henderson 2026
#include "rewrite/rewriter.hpp"

#include <algorithm>
#include <iterator>
#include <optional>
#include <string_view>
#include <tuple>
#include <variant>

namespace sc::format {

namespace {

using lex::TokenType;

// The messages `control` covers, see is_control_flow_or_iterator(). `if`, `switch`, and `case` are only rewritten
// when every argument after the receiver is a function: the compiler inlines them only then, and `switch` and `case`
// would have their pairs split.
bool needs_every_function(std::string_view selector) { return is_any_of(selector, "if", "switch", "case"); }

bool is_negative(const ast::ASTGraph::Variant& node) {
    if (const auto* int_node = std::get_if<ast::IntNode>(&node))
        return int_node->sign == ast::IntNode::Sign::Negative;
    if (const auto* float_node = std::get_if<ast::FloatNode>(&node))
        return float_node->sign == ast::FloatNode::Sign::Negative;
    if (const auto* accidental = std::get_if<ast::AccidentalNode>(&node))
        return accidental->sign == ast::AccidentalNode::Sign::Negative;
    return false;
}

// `f(a, { 1 }, { 2 })` becomes `f(a) { 1 } { 2 }`, and `x.do({ })` becomes `x.do { }`. The functions that move are
// the longest run at the end of the arguments, and go before any already after the `)`. Not with a keyword or `*`
// argument, or a comment between the parts that move.
class TrailingClosureRewriter {
public:
    TrailingClosureRewriter(const ast::ASTGraph& graph, const LayoutDraft& layout, const Style& style,
                            AllowedAstChanges& changes):
        m_graph(graph),
        m_layout(layout),
        m_style(style),
        m_changes(changes) {}

    std::vector<Edit> run() {
        m_graph.flat_walk([&](const ast::ASTGraph::Edges&, const ast::ASTGraph::Variant& node,
                              const src::SourceCodeRange&, std::size_t node_index) {
            const bool prefix = std::holds_alternative<ast::PrefixMessage>(node);
            if (prefix || std::holds_alternative<ast::PostfixMessage>(node)
                || std::holds_alternative<ast::NewMessage>(node))
                rewrite_message(ast::Index { static_cast<std::uint32_t>(node_index) }, prefix);
        });
        return std::move(m_edits).clear_of_verbatim_lines(m_layout);
    }

private:
    using ArgumentIterator = std::vector<ast::Index>::const_iterator;

    void rewrite_message(ast::Index message, bool prefix) {
        const auto parts = find_message_parts(message, prefix);
        if (!parts)
            return;
        const auto& selector = parts->selector;
        const bool is_control = selector && is_control_flow_or_iterator(*selector);
        if (m_style.trailing_closures == TrailingClosures::Control && !is_control)
            return;
        const auto argument_nodes = children_of(m_graph, parts->arguments);
        const auto first_moved = first_trailing_function(argument_nodes);
        if (first_moved == argument_nodes.end() || any_keyword_or_variadic(argument_nodes.begin(), first_moved))
            return;
        // A prefix message's first argument is its receiver, except for `case`, whose arguments are all pairs.
        // `argument_nodes` holds at least one function, so it isn't empty.
        const auto after_receiver = std::next(argument_nodes.begin(), prefix && *selector != "case" ? 1 : 0);
        const bool non_functions_after_receiver = std::distance(after_receiver, first_moved) > 0;
        if (is_control && non_functions_after_receiver) {
            if (*selector == "switch") {
                if (const auto tests = switch_tests_to_brace(argument_nodes, after_receiver))
                    move_functions_after_parens(argument_nodes, after_receiver, *tests);
                return;
            }
            if (needs_every_function(*selector))
                return;
        }
        move_functions_after_parens(argument_nodes, first_moved, {});
    }

    struct MessageParts {
        ast::Index arguments;
        std::optional<std::string_view> selector; // none for `Class(...)`
    };

    // None for a message with an adverb, `a +.x b`, or one without both an argument list and a function list.
    std::optional<MessageParts> find_message_parts(ast::Index message, bool prefix) const {
        // Children in the node's order: a prefix message's selector first, a postfix message's after its receiver.
        const auto message_parts = children_of(m_graph, message);
        std::optional<ast::Index> arguments;
        bool has_blocks { false };
        for (const auto part : message_parts) {
            const auto& node = m_graph.payload(part);
            if (std::holds_alternative<ast::ArgumentList>(node))
                arguments = part;
            else if (std::holds_alternative<ast::BlockList>(node))
                has_blocks = true;
            else if (std::holds_alternative<ast::SelectorWAdverb>(node))
                return std::nullopt;
        }
        if (!arguments || !has_blocks)
            return std::nullopt;
        const bool postfix = std::holds_alternative<ast::PostfixMessage>(m_graph.payload(message));
        std::optional<std::string_view> selector;
        if ((prefix || postfix) && message_parts.size() > (postfix ? 1u : 0u))
            selector = m_graph.string_view(message_parts[postfix ? 1 : 0]);
        return MessageParts { *arguments, selector };
    }

    // The start of the longest run of functions ending the arguments, the end if there is none.
    ArgumentIterator first_trailing_function(const std::vector<ast::Index>& argument_nodes) const {
        auto first = argument_nodes.end();
        while (first != argument_nodes.begin() && is_function(*std::prev(first)))
            --first;
        return first;
    }

    bool any_keyword_or_variadic(ArgumentIterator begin, ArgumentIterator end) const {
        return std::any_of(begin, end, [&](ast::Index argument) {
            const auto& node = m_graph.payload(argument);
            return std::holds_alternative<ast::KwArgNode>(node) || std::holds_alternative<ast::VariadicArgNode>(node);
        });
    }

    // `switch(x, \a, { 1 })` becomes `switch(x) { \a } { 1 }`, only where both compilers inline the `switch` either
    // way: else `{ \a }` would be a function to send `value` to, not a key. Returns the tests to put in braces, or none
    // if a pair can't move.
    std::optional<std::vector<ast::Index>> switch_tests_to_brace(const std::vector<ast::Index>& argument_nodes,
                                                                 ArgumentIterator first_pair) const {
        std::vector<ast::Index> tests;
        for (auto argument = first_pair; argument != argument_nodes.end(); ++argument) {
            const bool is_test =
                std::distance(first_pair, argument) % 2 == 0 && std::distance(argument, argument_nodes.end()) > 1;
            if (is_test && !is_function(*argument)) {
                if (!is_switch_test(m_graph.payload(*argument)) || m_graph.is_bracketed(*argument))
                    return std::nullopt;
                tests.push_back(*argument);
            } else if (!is_function(*argument) || !inlineable(*argument)) {
                return std::nullopt;
            }
        }
        return tests;
    }

    // A function both compilers inline: not `#{ }`, no parameters, and no `var`s, which only 3.16 inlines.
    bool inlineable(ast::Index function) const {
        const auto* block = std::get_if<ast::BlockNode>(&m_graph.payload(function));
        if (!block || block->closed)
            return false;
        for (const auto part : children_of(m_graph, function)) {
            const auto& part_node = m_graph.payload(part);
            if (std::holds_alternative<ast::DeclareArgumentList>(part_node) && m_graph.edges(part).first_child)
                return false;
            if (std::holds_alternative<ast::BlockContentsList>(part_node))
                for (const auto item : children_of(m_graph, part))
                    if (std::holds_alternative<ast::DeclareVariableList>(m_graph.payload(item)))
                        return false;
        }
        return true;
    }

    // Where an argument's text starts: a negative number's node leaves out its `-`.
    TextOffset text_begin(ast::Index argument) const {
        const auto node_begin = begin_of(m_graph.location(argument));
        if (!is_negative(m_graph.payload(argument)))
            return node_begin;
        const auto node_first_token = m_layout.first_token_at_or_after(node_begin);
        return *node_first_token > 0 ? m_layout.tokens[util::retreat(node_first_token)].begin : node_begin;
    }

    bool is_function(ast::Index node) const {
        return std::holds_alternative<ast::BlockNode>(m_graph.payload(node)) && !m_graph.is_bracketed(node);
    }

    // Moves the arguments from `first_moved` on after the `)`, if only the expected tokens are between them, putting
    // the tests in braces.
    void move_functions_after_parens(const std::vector<ast::Index>& argument_nodes, ArgumentIterator first_moved,
                                     const std::vector<ast::Index>& tests) {
        const auto opener_or_comma = util::retreat(m_layout.first_token_at_or_after(text_begin(*first_moved)));
        std::vector<std::tuple<TextOffset, TextOffset, std::string>> edits;
        // A line break after the `(` or comma stays, as between the functions.
        const bool line_broken = m_layout.whitespace_before[util::advance(opener_or_comma)].newlines > 0;
        if (first_moved == argument_nodes.begin()) {
            // `x.do({` becomes `x.do {`.
            if (!m_layout.is(opener_or_comma, TokenType::OpenParen) || *opener_or_comma == 0)
                return;
            if (m_layout.is_comment(util::retreat(opener_or_comma)))
                return;
            const auto previous_end = m_layout.tokens[util::retreat(opener_or_comma)].end;
            if (line_broken)
                edits.emplace_back(previous_end, m_layout.tokens[opener_or_comma].end, "");
            else
                edits.emplace_back(previous_end, text_begin(*first_moved), " ");
        } else {
            // `f(a, {` becomes `f(a) {`.
            const auto kept_end = end_of(m_graph.location(*std::prev(first_moved)));
            if (!m_layout.is(opener_or_comma, TokenType::Comma)
                || m_layout.first_token_at_or_after(kept_end) != opener_or_comma)
                return;
            if (line_broken)
                edits.emplace_back(kept_end, m_layout.tokens[opener_or_comma].end, ")");
            else
                edits.emplace_back(kept_end, text_begin(*first_moved), ") ");
        }
        // `}, {` becomes `} {`, keeping a line break after the comma.
        for (auto argument = first_moved; std::next(argument) != argument_nodes.end(); ++argument) {
            const auto argument_end = end_of(m_graph.location(*argument));
            const auto comma = m_layout.first_token_at_or_after(argument_end);
            const auto next_begin = text_begin(*std::next(argument));
            if (!m_layout.is(comma, TokenType::Comma) || m_layout.tokens[util::advance(comma)].begin != next_begin)
                return;
            if (m_layout.whitespace_before[util::advance(comma)].newlines > 0)
                edits.emplace_back(argument_end, m_layout.tokens[comma].end, "");
            else
                edits.emplace_back(argument_end, next_begin, " ");
        }
        // `})` becomes `}`, with a trailing comma and the whitespace before the `)`.
        const auto last_end = end_of(m_graph.location(argument_nodes.back()));
        auto close_paren = m_layout.first_token_at_or_after(last_end);
        if (m_layout.is(close_paren, TokenType::Comma))
            close_paren = util::advance(close_paren);
        if (!m_layout.is(close_paren, TokenType::CloseParen) || m_layout.is_comment(util::retreat(close_paren)))
            return;
        edits.emplace_back(last_end, m_layout.tokens[close_paren].end, "");

        m_changes.trailing_closures = true;
        m_changes.switch_tests = m_changes.switch_tests || !tests.empty();
        m_edits.start_group();
        for (auto& [edit_begin, edit_end, edit_text] : edits)
            m_edits.add(edit_begin, edit_end, std::move(edit_text));
        for (const auto test : tests) {
            m_edits.add(text_begin(test), text_begin(test), "{ ");
            m_edits.add(end_of(m_graph.location(test)), end_of(m_graph.location(test)), " }");
        }
    }

    const ast::ASTGraph& m_graph;
    const LayoutDraft& m_layout;
    EditGroups m_edits;
    const Style& m_style;
    AllowedAstChanges& m_changes;
};
}

bool is_switch_test(const ast::ASTGraph::Variant& node) {
    return holds_any<ast::IntNode, ast::FloatNode, ast::PiNode, ast::AccidentalNode, ast::SymbolSlash, ast::SymbolQuote,
                     ast::BooleanNode, ast::ASCIINode>(node);
}

std::vector<Edit> trailing_closure_edits(const ast::ASTGraph& graph, const LayoutDraft& layout, const Style& style,
                                         AllowedAstChanges& changes) {
    if (style.trailing_closures == TrailingClosures::Keep)
        return {};
    return TrailingClosureRewriter { graph, layout, style, changes }.run();
}
}
