// Copyright Jordan Henderson 2026
#include "rewrite/rewriter.hpp"

#include <variant>

namespace sc::format {

namespace {

using lex::TokenType;

// `{ a; b; }` becomes `{ a; b }`, in functions and methods. Not after a `var` statement, where the body returns
// nothing.
class FinalSemicolonRewriter {
public:
    FinalSemicolonRewriter(const ast::ASTGraph& graph, const LayoutDraft& layout): m_graph(graph), m_layout(layout) {}

    std::vector<Edit> run() {
        m_graph.flat_walk([&](const ast::ASTGraph::Edges&, const ast::ASTGraph::Variant& node,
                              const src::SourceCodeRange&, std::size_t node_index) {
            if (holds_any<ast::BlockNode, ast::Method, ast::ClassMethod>(node))
                for (const auto child : children_of(m_graph, ast::Index { static_cast<std::uint32_t>(node_index) }))
                    if (std::holds_alternative<ast::BlockContentsList>(m_graph.payload(child)))
                        remove_final_semicolon(child);
        });
        return std::move(m_edits).clear_of_verbatim_lines(m_layout);
    }

private:
    void remove_final_semicolon(ast::Index body) {
        const auto items = children_of(m_graph, body);
        if (items.empty() || std::holds_alternative<ast::DeclareVariableList>(m_graph.payload(items.back())))
            return;
        const auto semicolon =
            m_layout.first_non_comment_from(m_layout.first_token_at_or_after(end_of(m_graph.location(body))));
        if (!m_layout.is(semicolon, TokenType::SemiColon)
            || !m_layout.is(m_layout.next_non_comment(semicolon), TokenType::CloseCurly))
            return;
        // With the whitespace before it, unless that holds a comment's newline: then with the spaces after it.
        const auto previous = util::retreat(semicolon);
        const auto next = util::advance(semicolon);
        const bool after_comment_line =
            m_layout.is_comment(previous) && m_layout.whitespace_before[semicolon].newlines > 0;
        const auto& semicolon_token = m_layout.tokens[semicolon];
        m_edits.start_group();
        if (!after_comment_line)
            m_edits.add(m_layout.tokens[previous].end, semicolon_token.end, "");
        else if (m_layout.whitespace_before[next].newlines == 0)
            m_edits.add(semicolon_token.begin, m_layout.tokens[next].begin, "");
        else
            m_edits.add(semicolon_token.begin, semicolon_token.end, "");
    }

    const ast::ASTGraph& m_graph;
    const LayoutDraft& m_layout;
    EditGroups m_edits;
};

// A `;` between two top-level items of an scd file where there is none, which the parser guesses from the lines, see
// yylex. The item's brackets belong to the list, so between two items are the first's closers, comments, and the
// next's openers; the `;` goes after the closers, before a comment.
class RegionSemicolonRewriter {
public:
    RegionSemicolonRewriter(const ast::ASTGraph& graph, const LayoutDraft& layout): m_graph(graph), m_layout(layout) {}

    std::vector<Edit> run() {
        const auto root = m_graph.root_any();
        if (!root || !std::holds_alternative<ast::RegionList>(m_graph.payload(*root)))
            return {};
        const auto items = children_of(m_graph, *root);
        for (auto item = items.begin(); item != items.end() && std::next(item) != items.end(); ++item)
            insert_semicolon_between(m_graph.location(*item), m_graph.location(*std::next(item)));
        return std::move(m_edits).clear_of_verbatim_lines(m_layout);
    }

private:
    void insert_semicolon_between(const src::SourceCodeRange& first_item, const src::SourceCodeRange& second_item) {
        const auto second_item_start = m_layout.first_token_at_or_after(begin_of(second_item));
        // Only a `;` after the closers: `(a = 1;)` has its `;` inside.
        const auto first_item_end = m_layout.first_token_at_or_after(end_of(first_item));
        for (auto token = second_item_start; *token > *first_item_end;) {
            token = util::retreat(token);
            if (m_layout.is(token, TokenType::CloseParen))
                break;
            if (m_layout.is(token, TokenType::SemiColon))
                return;
        }
        auto insert_after = second_item_start;
        do {
            if (*insert_after == 0)
                return;
            insert_after = util::retreat(insert_after);
        } while (m_layout.is_comment(insert_after) || m_layout.is(insert_after, TokenType::OpenParen));
        m_edits.start_group();
        m_edits.add(m_layout.tokens[insert_after].end, m_layout.tokens[insert_after].end, ";");
    }

    const ast::ASTGraph& m_graph;
    const LayoutDraft& m_layout;
    EditGroups m_edits;
};
}

std::vector<Edit> final_semicolon_edits(const ast::ASTGraph& graph, const LayoutDraft& layout, const Style& style) {
    if (style.final_semicolon == FinalSemicolon::Keep)
        return {};
    return FinalSemicolonRewriter { graph, layout }.run();
}

std::vector<Edit> region_semicolon_edits(const ast::ASTGraph& graph, const LayoutDraft& layout, const Style& style) {
    if (style.region_semicolons == RegionSemicolons::Keep)
        return {};
    return RegionSemicolonRewriter { graph, layout }.run();
}
}
