// Copyright Jordan Henderson 2026
#include "rewrite/rewriter.hpp"

#include <algorithm>
#include <optional>
#include <variant>
#include <vector>

namespace sc::format {

namespace {

using lex::TokenType;

bool contains(const Edit& outer, const Edit& inner) { return *outer.begin <= *inner.begin && *inner.end <= *outer.end; }

// `x.if { }` becomes `if(x) { }`, `(a > b).if({ }, { })` becomes `if(a > b, { }, { })`, and `1.for(9) { }` becomes
// `for(1, 9) { }`, for control flow, not iterators. Not with an adverb, `super` (a super send), or a comment in the
// part that changes. Where one such message is the receiver of another, the inner one is rewritten and the outer one
// in the next round.
class ControlPrefixRewriter {
public:
    ControlPrefixRewriter(const ast::ASTGraph& graph, const LayoutDraft& layout, AllowedAstChanges& changes):
        m_graph(graph),
        m_layout(layout),
        m_changes(changes) {}

    std::vector<Edit> run() {
        std::vector<Edit> candidates;
        m_graph.flat_walk([&](const ast::ASTGraph::Edges&, const ast::ASTGraph::Variant& node,
                              const src::SourceCodeRange&, std::size_t node_index) {
            if (std::holds_alternative<ast::PostfixMessage>(node))
                if (auto edit = rewrite_message(ast::Index { static_cast<std::uint32_t>(node_index) }))
                    candidates.push_back(std::move(*edit));
        });
        // Only edits that hold no other, as the receiver's text is copied.
        for (const auto& candidate : candidates) {
            const bool contains_another = std::any_of(candidates.begin(), candidates.end(), [&](const Edit& other) {
                return &other != &candidate && contains(candidate, other);
            });
            if (!contains_another) {
                m_changes.control_prefix = true;
                m_edits.start_group();
                m_edits.add(candidate.begin, candidate.end, candidate.text);
            }
        }
        return std::move(m_edits).clear_of_verbatim_lines(m_layout);
    }

private:
    // Children in the node's order: receiver, selector, arguments, functions.
    std::optional<Edit> rewrite_message(ast::Index message) {
        const auto parts = children_of(m_graph, message);
        if (parts.size() != 4 || !std::holds_alternative<ast::NamedIdentifier>(m_graph.payload(parts[1])))
            return std::nullopt;
        const auto selector = m_graph.string_view(parts[1]);
        if (!is_control_flow(selector) || m_graph.string_view(parts[0]) == "super")
            return std::nullopt;
        const auto message_first = m_layout.first_token_at_or_after(begin_of(m_graph.location(message)));
        const auto selector_token = m_layout.token_at_start_of(m_graph.location(parts[1]));
        if (!selector_token || **selector_token < 2 || !m_layout.is(util::retreat(*selector_token), TokenType::Dot))
            return std::nullopt;
        // The receiver's text from the message's first token, as a negative number's node leaves out its `-`, without
        // the brackets around it.
        auto receiver_first = message_first;
        auto receiver_last = util::retreat(util::retreat(*selector_token));
        if (m_graph.is_bracketed(parts[0])) {
            if (!m_layout.is(receiver_first, TokenType::OpenParen)
                || !m_layout.is(receiver_last, TokenType::CloseParen))
                return std::nullopt;
            receiver_first = util::advance(receiver_first);
            receiver_last = util::retreat(receiver_last);
        }
        const auto receiver_text =
            m_layout.text_between(m_layout.tokens[receiver_first].begin, m_layout.tokens[receiver_last].end);
        auto replaced_last = *selector_token;
        // Past a comment, so `x.if /* c */ (` keeps its comment in the part that changes.
        const auto after_selector = m_layout.next_non_comment(replaced_last);
        const bool has_paren = m_layout.is(after_selector, TokenType::OpenParen);
        if (has_paren)
            replaced_last = after_selector;
        for (auto replaced_token = message_first; *replaced_token <= *replaced_last;
             replaced_token = util::advance(replaced_token))
            if (m_layout.is_comment(replaced_token))
                return std::nullopt;
        std::string replacement { selector };
        replacement += "(";
        replacement += receiver_text;
        if (!has_paren)
            replacement += ")";
        else if (m_graph.edges(parts[2]).first_child) {
            // `x.if( {` and `x.if(` before a newline have their whitespace after the comma already.
            const bool spaced =
                m_layout.tokens[util::advance(replaced_last)].begin != m_layout.tokens[replaced_last].end;
            replacement += spaced ? "," : ", ";
        }
        return Edit { m_layout.tokens[message_first].begin, m_layout.tokens[replaced_last].end,
                      std::move(replacement) };
    }

    const ast::ASTGraph& m_graph;
    const LayoutDraft& m_layout;
    EditGroups m_edits;
    AllowedAstChanges& m_changes;
};
}

std::vector<Edit> control_prefix_edits(const ast::ASTGraph& graph, const LayoutDraft& layout, const Style& style,
                                       AllowedAstChanges& changes) {
    if (style.control_prefix == Switch::Keep)
        return {};
    return ControlPrefixRewriter { graph, layout, changes }.run();
}
}
