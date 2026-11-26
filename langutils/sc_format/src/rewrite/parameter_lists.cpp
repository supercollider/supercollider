// Copyright Jordan Henderson 2026
#include "rewrite/rewriter.hpp"

#include <cstdint>
#include <iterator>
#include <string_view>
#include <variant>

namespace sc::format {

namespace {

using lex::TokenType;

// A default the pipe grammar takes without brackets.
bool needs_no_brackets_in_pipes(const ast::ASTGraph::Variant& node) {
    return holds_any<ast::IntNode, ast::FloatNode, ast::PiNode, ast::AccidentalNode, ast::SymbolSlash, ast::SymbolQuote,
                     ast::StringLineList, ast::BooleanNode, ast::NilNode, ast::ASCIINode, ast::BlockNode,
                     ast::ArrayNode, ast::DictionaryNode>(node);
}

class ParameterListRewriter {
public:
    ParameterListRewriter(const ast::ASTGraph& graph, const LayoutDraft& layout, const Style& style,
                          AllowedAstChanges& changes):
        m_graph(graph),
        m_layout(layout),
        m_style(style),
        m_changes(changes) {}

    std::vector<Edit> run() {
        m_graph.flat_walk([&](const ast::ASTGraph::Edges& edges, const ast::ASTGraph::Variant& node,
                              const src::SourceCodeRange& range, std::size_t node_index) {
            if (!std::holds_alternative<ast::DeclareArgumentList>(node))
                return;
            const ast::Index list { static_cast<std::uint32_t>(node_index) };
            std::vector<ast::Index> parameters;
            for (auto child = edges.first_child; child; child = m_graph.edges(ast::Index { *child }).next_sibling)
                parameters.push_back(ast::Index { *child });
            if (m_style.parameter_pipes != ParameterPipes::Keep)
                use_pipes(list, range, parameters);
            if (m_style.parameter_commas == Switch::On)
                add_missing_commas(parameters);
            if (m_style.parameter_default_equals == Switch::On)
                for (const auto parameter : parameters)
                    add_default_equals(parameter);
        });
        return std::move(m_edits).clear_of_verbatim_lines(m_layout);
    }

private:
    // `arg a, b;` becomes `|a, b|`, and `arg;` goes, except a file's, which makes the file a function. Not where a
    // comment is next to `arg` or the `;`.
    void use_pipes(ast::Index list, const src::SourceCodeRange& range, const std::vector<ast::Index>& parameters) {
        if (parameters.empty() && is_file_parameters(list))
            return;
        m_edits.start_group();
        auto keyword = m_layout.first_token_at_or_after(TextOffset { range.begin.absolute });
        if (!m_layout.is(keyword, TokenType::Arg)) {
            if (*keyword == 0 || !m_layout.is(util::retreat(keyword), TokenType::Arg))
                return;
            keyword = util::retreat(keyword);
        }
        const auto semicolon =
            parameters.empty() ? util::advance(keyword) : m_layout.first_token_at_or_after(end_of(range));
        if (!m_layout.is(semicolon, TokenType::SemiColon))
            return;
        if (m_style.parameter_pipes == ParameterPipes::OneLine)
            for (auto between = util::advance(keyword); *between <= *semicolon; between = util::advance(between))
                if (m_layout.whitespace_before[between].newlines > 0
                    || (m_layout.is_comment(between)
                        && m_layout.token_text(between).find('\n') != std::string_view::npos))
                    return;
        if (parameters.empty()) {
            // With the whitespace before it, or after it at the start of the text or after a comment, whose
            // newline stays.
            const auto after_semicolon = util::advance(semicolon);
            const auto removed_end = m_layout.tokens.contains(after_semicolon)
                ? m_layout.tokens[after_semicolon].begin
                : TextOffset::from(m_layout.normalised_text.size());
            if (*keyword == 0 || m_layout.is_comment(util::retreat(keyword)))
                m_edits.add(m_layout.tokens[keyword].begin, removed_end, "");
            else
                m_edits.add(m_layout.tokens[util::retreat(keyword)].end, m_layout.tokens[semicolon].end, "");
            return;
        }
        const auto first_parameter_token = util::advance(keyword);
        const auto last_parameter_token = util::retreat(semicolon);
        if (m_layout.is_comment(first_parameter_token) || m_layout.is_comment(last_parameter_token))
            return;
        // A default's brackets are its own in `arg a = (1);`, and the declaration's in `|a = (1)|`.
        m_changes.default_brackets = true;
        m_edits.add(m_layout.tokens[keyword].begin, m_layout.tokens[first_parameter_token].begin, "|");
        // `arg a;// x` would become `|a|// x`, where `|//` is an operator.
        const auto after_semicolon = util::advance(semicolon);
        const bool touches_semicolon = m_layout.tokens.contains(after_semicolon)
            && m_layout.tokens[after_semicolon].begin == m_layout.tokens[semicolon].end;
        const bool would_join = touches_semicolon && lexes_as_one("|", m_layout.token_text(after_semicolon));
        m_edits.add(m_layout.tokens[last_parameter_token].end, m_layout.tokens[semicolon].end, would_join ? "| " : "|");
        for (const auto parameter : parameters)
            bracket_default(parameter);
    }

    bool is_file_parameters(ast::Index list) const {
        const auto root = m_graph.root_any();
        if (!root || !std::holds_alternative<ast::FileAsFunction>(m_graph.payload(*root)))
            return false;
        const auto first_child = m_graph.edges(*root).first_child;
        return first_child && *first_child == *list;
    }

    // `a = 4.dbamp` becomes `a = (4.dbamp)`, as the pipe grammar takes only a literal.
    void bracket_default(ast::Index declaration) {
        const auto* with_default = std::get_if<ast::DeclareArgumentWithDefaultNode>(&m_graph.payload(declaration));
        if (!with_default || with_default->bracketed)
            return;
        const auto& edges = m_graph.edges(declaration);
        const ast::Index name_node { *edges.first_child };
        const ast::Index value { *m_graph.edges(name_node).next_sibling };
        if (needs_no_brackets_in_pipes(m_graph.payload(value)))
            return;
        const auto equals =
            m_layout.first_non_comment_from(m_layout.first_token_at_or_after(end_of(m_graph.location(name_node))));
        if (!m_layout.is(equals, TokenType::EqualsSign))
            return;
        const auto value_first = m_layout.next_non_comment(equals);
        const auto value_last = util::retreat(m_layout.first_token_at_or_after(end_of(m_graph.location(declaration))));
        m_edits.add(m_layout.tokens[value_first].begin, m_layout.tokens[value_first].begin, "(");
        m_edits.add(m_layout.tokens[value_last].end, m_layout.tokens[value_last].end, ")");
    }

    // `|a b|` becomes `|a, b|`. Never before `...rest`, where the grammar takes no comma.
    void add_missing_commas(const std::vector<ast::Index>& parameters) {
        for (auto parameter = parameters.begin();
             parameter != parameters.end() && std::next(parameter) != parameters.end(); ++parameter) {
            if (std::holds_alternative<ast::DeclareArgumentVariadicNode>(m_graph.payload(*std::next(parameter))))
                continue;
            const auto parameter_end = end_of(m_graph.location(*parameter));
            const auto next_parameter_start = m_layout.token_at_start_of(m_graph.location(*std::next(parameter)));
            if (!next_parameter_start)
                continue;
            bool has_comma { false };
            for (auto between = m_layout.first_token_at_or_after(parameter_end); *between < **next_parameter_start;
                 between = util::advance(between))
                has_comma = has_comma || m_layout.is(between, TokenType::Comma);
            if (!has_comma) {
                m_edits.start_group();
                m_edits.add(parameter_end, parameter_end, ",");
            }
        }
    }

    // `|a 1|` becomes `|a = 1|`, and `|a(1)|` becomes `|a = (1)|`.
    void add_default_equals(ast::Index declaration) {
        if (!std::holds_alternative<ast::DeclareArgumentWithDefaultNode>(m_graph.payload(declaration)))
            return;
        const auto& name_range = m_graph.location(ast::Index { *m_graph.edges(declaration).first_child });
        const auto after_name = m_layout.first_non_comment_from(m_layout.first_token_at_or_after(end_of(name_range)));
        if (m_layout.is(after_name, TokenType::EqualsSign) || !m_layout.tokens.contains(after_name))
            return;
        const auto name_end = end_of(name_range);
        m_edits.start_group();
        m_edits.add(name_end, name_end,
                    *m_layout.tokens[m_layout.first_token_at_or_after(name_end)].begin > *name_end ? " =" : " = ");
    }

    const ast::ASTGraph& m_graph;
    const LayoutDraft& m_layout;
    EditGroups m_edits;
    const Style& m_style;
    AllowedAstChanges& m_changes;
};
}

std::vector<Edit> parameter_edits(const ast::ASTGraph& graph, const LayoutDraft& layout, const Style& style,
                                  AllowedAstChanges& changes) {
    if (!any_not_keep(style.parameter_pipes, style.parameter_commas, style.parameter_default_equals))
        return {};
    return ParameterListRewriter { graph, layout, style, changes }.run();
}
}
