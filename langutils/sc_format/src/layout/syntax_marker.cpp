// Copyright Jordan Henderson 2026
#include "layout/passes.hpp"

#include "syntax.hpp"
#include "text.hpp"

#include "sc_parser/ast.hpp"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string_view>
#include <variant>
#include <vector>

namespace sc::format {

namespace {

using lex::TokenType;

// How wide the whitespace at the start of text is, with tab stops every 4 columns, to compare indentation written
// with tabs and spaces.
std::uint32_t indent_width(std::string_view text) {
    std::uint32_t width { 0 };
    for (auto remaining = text; !remaining.empty();) {
        const auto [code_point, size] = first_codepoint(remaining);
        if (size == 0 || !is_space_or_tab(code_point))
            break;
        width = src::is_tab(code_point) ? (width / 4 + 1) * 4 : width + 1;
        remaining.remove_prefix(size);
    }
    return width;
}

// Lists whose children are items, so a line starting a child continues nothing. Not declarations, as in
// `var a,\n b;`, where the line continues the `var`.
bool is_list_of_items(const ast::ASTGraph::Variant& node) {
    return holds_any<ast::ArgumentList, ast::ArrayNode, ast::BigMemberList, ast::BlockContentsList,
                     ast::ClassOrExtensionList, ast::DictionaryNode, ast::ExprSeq, ast::GeneratorQualList,
                     ast::MethodList, ast::RegionList>(node);
}

bool has_receiver(const ast::ASTGraph::Variant& node) {
    return holds_any<ast::PostfixMessage, ast::ValueMessage, ast::SetterNode>(node);
}

class SyntaxMarker {
public:
    SyntaxMarker(const LayoutDraft& layout, SyntaxMarks& marks, const ast::ASTGraph& graph):
        m_layout(layout),
        m_marks(marks),
        m_graph(graph) {}

    void run(bool method_chains_and_strings, bool list_items_and_regions, bool roles, bool control_functions) {
        m_graph.flat_walk([&](const ast::ASTGraph::Edges& edges, const ast::ASTGraph::Variant& node,
                              const src::SourceCodeRange& range, std::size_t) {
            if (roles)
                mark_role(edges, node, range);
            if (std::holds_alternative<ast::PrefixMessage>(node) || std::holds_alternative<ast::PostfixMessage>(node)) {
                if (const auto message =
                        message_with_functions(edges, std::holds_alternative<ast::PostfixMessage>(node))) {
                    if (control_functions)
                        mark_control_pairs(*message, range);
                    if (roles || control_functions)
                        mark_control_functions(*message, range, roles, control_functions);
                }
            }
            if (list_items_and_regions && is_list_of_items(node)) {
                for (auto child = edges.first_child; child; child = m_graph.edges(ast::Index { *child }).next_sibling)
                    if (const auto first_token = first_token_of_item(ast::Index { *child }))
                        m_marks.tokens[*first_token].starts_list_item = true;
            } else if (list_items_and_regions && std::holds_alternative<ast::PrimitiveIdentifier>(node)) {
                if (const auto first_token = m_layout.token_at_start_of(range))
                    m_marks.tokens[*first_token].starts_list_item = true;
            }
            if (!method_chains_and_strings)
                return;
            if (has_receiver(node) && edges.first_child) {
                mark_method_chain_line(ast::Index { *edges.first_child });
            } else if (std::holds_alternative<ast::StringLineList>(node) && edges.first_child) {
                mark_string_continuation_lines(ast::Index { *edges.first_child });
            }
        });
        if (list_items_and_regions)
            mark_regions();
        std::sort(m_marks.relative_indent_lines.begin(), m_marks.relative_indent_lines.end(),
                  [](const RelativeIndentLine& left, const RelativeIndentLine& right) {
                      return *left.first_token < *right.first_token;
                  });
    }

private:
    void mark_role(const ast::ASTGraph::Edges& edges, const ast::ASTGraph::Variant& node,
                   const src::SourceCodeRange& range) {
        if (std::holds_alternative<ast::BinaryMessage>(node) && edges.first_child)
            mark_binary_operator(m_graph.location(ast::Index { *edges.first_child }));
        else if (std::holds_alternative<ast::DeclareArgumentWithDefaultNode>(node))
            mark_equals_before_value(edges, TokenRole::ParameterDefaultEquals);
        else if (is_assignment(node))
            mark_equals_before_value(edges, TokenRole::AssignmentEquals);
        else if (std::holds_alternative<ast::DeclareArgumentList>(node))
            mark_parameter_list_pipes(range);
        else if (holds_any<ast::KwArgNode, ast::DictionaryEntryNode>(node) && edges.first_child)
            mark_key_colon(m_graph.location(ast::Index { *edges.first_child }));
        else if (std::holds_alternative<ast::ArrayNode>(node))
            mark_array_open_bracket(range);
        else if (std::holds_alternative<ast::DictionaryNode>(node))
            mark_dictionary_open_paren(range);
        else if (std::holds_alternative<ast::BlockList>(node))
            mark_trailing_closure_braces(edges);
        else if (std::holds_alternative<ast::ArgumentList>(node) && edges.first_child && parent_is_message(edges))
            mark_argument_list_open_paren(ast::Index { *edges.first_child });
    }

    static bool is_message(const ast::ASTGraph::Variant& node) {
        return holds_any<ast::PrefixMessage, ast::PostfixMessage, ast::ValueMessage, ast::NewMessage>(node);
    }

    bool parent_is_message(const ast::ASTGraph::Edges& edges) const {
        return edges.parent && is_message(m_graph.payload(ast::Index { *edges.parent }));
    }

    void mark_dictionary_open_paren(const src::SourceCodeRange& range) {
        const auto open_paren = m_layout.token_at_start_of(range);
        if (open_paren && m_layout.is(*open_paren, TokenType::OpenParen))
            m_marks.tokens[*open_paren].role = TokenRole::DictionaryOpenParen;
    }

    // A brace that already has a role keeps it.
    void mark_trailing_closure_braces(const ast::ASTGraph::Edges& block_list) {
        for (auto child = block_list.first_child; child; child = m_graph.edges(ast::Index { *child }).next_sibling) {
            const auto open_brace = m_layout.token_at_start_of(m_graph.location(ast::Index { *child }));
            if (!open_brace || !m_layout.is_open_brace(*open_brace))
                continue;
            if (m_marks.tokens[*open_brace].role == TokenRole::None)
                m_marks.tokens[*open_brace].role = TokenRole::TrailingClosureOpenBrace;
        }
    }

    // A named message with functions after its arguments, `if(x) { }`, `x.do { }`.
    struct MessageWithFunctions {
        std::string_view name;
        bool postfix;
        ast::Index arguments;
        ast::Index functions;
    };

    // A prefix message's children are its selector, arguments, and functions. A postfix message's receiver is first.
    std::optional<MessageWithFunctions> message_with_functions(const ast::ASTGraph::Edges& edges, bool postfix) const {
        std::vector<ast::Index> parts;
        for (auto child = edges.first_child; child; child = m_graph.edges(ast::Index { *child }).next_sibling)
            parts.push_back(ast::Index { *child });
        if (parts.size() != (postfix ? 4u : 3u))
            return std::nullopt;
        const auto selector = parts[postfix ? 1 : 0];
        if (!std::holds_alternative<ast::NamedIdentifier>(m_graph.payload(selector)))
            return std::nullopt;
        return MessageWithFunctions { m_graph.string_view(selector), postfix, parts[postfix ? 2 : 1], parts.back() };
    }

    // The functions after a control message's `)`, `if(x) { 1 } { 2 }`. K&R braces join them to the line before if
    // one spans lines, or each is a literal: longer ones on one line would make a long line. Those stay on their own
    // lines, one level more than the message's line. With a multi-line one, the lines are as written (Allman).
    void mark_control_functions(const MessageWithFunctions& message, const src::SourceCodeRange& message_range,
                                bool mark_roles, bool add_relative_indent_lines) {
        const auto name = message.name;
        if (!is_control_flow_or_iterator(name) || name == "switch" || name == "case")
            return;
        std::vector<TokenIndex> open_braces;
        bool all_literal { true };
        bool any_spans { false };
        for (auto child = m_graph.edges(message.functions).first_child; child;
             child = m_graph.edges(ast::Index { *child }).next_sibling) {
            const ast::Index function { *child };
            const auto& location = m_graph.location(function);
            const auto open_brace = m_layout.token_at_start_of(location);
            if (!open_brace || !m_layout.is_open_brace(*open_brace))
                return;
            const auto close_brace =
                util::retreat(m_layout.first_token_at_or_after(TextOffset { location.end.absolute }));
            const bool function_spans = m_layout.line_break_between(*open_brace, close_brace);
            all_literal = all_literal && is_literal_function(function);
            any_spans = any_spans || function_spans;
            open_braces.push_back(*open_brace);
        }
        const auto message_start = m_layout.first_token_at_or_after(TextOffset { message_range.begin.absolute });
        for (const auto open_brace : open_braces) {
            if (mark_roles && (any_spans || all_literal))
                m_marks.tokens[open_brace].role = TokenRole::ControlFunctionOpenBrace;
            if (add_relative_indent_lines && !any_spans)
                m_marks.relative_indent_lines.push_back(
                    { open_brace, message_start, RelativeIndentLine::Kind::ControlFunction, false });
        }
    }

    // `{ 1 }`, `{ \a }`, `{ "a" }`: a function without parameters whose body is one literal.
    bool is_literal_function(ast::Index function) const {
        std::optional<ast::Index> body;
        for (auto child = m_graph.edges(function).first_child; child;
             child = m_graph.edges(ast::Index { *child }).next_sibling) {
            const ast::Index part { *child };
            if (std::holds_alternative<ast::DeclareArgumentList>(m_graph.payload(part))) {
                if (m_graph.edges(part).first_child)
                    return false;
            } else {
                body = part;
            }
        }
        auto item = body;
        while (item && !is_literal(m_graph.payload(*item))) {
            const auto& edges = m_graph.edges(*item);
            const auto& node = m_graph.payload(*item);
            const bool wraps_expressions = holds_any<ast::BlockContentsList, ast::ExprSeq>(node);
            const bool has_one_child =
                edges.first_child && !m_graph.edges(ast::Index { *edges.first_child }).next_sibling;
            if (!wraps_expressions || !has_one_child)
                return false;
            item = ast::Index { *edges.first_child };
        }
        return item.has_value();
    }

    static bool is_literal(const ast::ASTGraph::Variant& node) {
        return holds_any<ast::IntNode, ast::FloatNode, ast::PiNode, ast::AccidentalNode, ast::SymbolSlash,
                         ast::SymbolQuote, ast::StringLineList, ast::BooleanNode, ast::NilNode, ast::ASCIINode>(node);
    }

    // An array's range includes its brackets, and a `#` before them.
    void mark_array_open_bracket(const src::SourceCodeRange& range) {
        auto open_square = m_layout.token_at_start_of(range);
        if (open_square && m_layout.is(*open_square, TokenType::Hash))
            open_square = util::advance(*open_square);
        if (open_square && m_layout.is(*open_square, TokenType::OpenSquare))
            m_marks.tokens[*open_square].role = TokenRole::ArrayOpenBracket;
    }

    // The `(` before a message's first argument, not the `[` of `a[0]`.
    void mark_argument_list_open_paren(ast::Index first_argument) {
        const auto first_token = first_token_of_item(first_argument);
        const auto open_paren = first_token ? m_layout.previous_non_comment(*first_token) : std::nullopt;
        if (open_paren && m_layout.is(*open_paren, TokenType::OpenParen))
            m_marks.tokens[*open_paren].role = TokenRole::ArgumentListOpenParen;
    }

    // `freq: 440`, `(a: 1)`, `(\a: 1)`: the keyword, or the `:` after the key.
    void mark_key_colon(const src::SourceCodeRange& key) {
        if (key.begin.absolute == key.end.absolute)
            return;
        const auto token_after_key = m_layout.first_token_at_or_after(TextOffset { key.end.absolute });
        const auto last_key_token = util::retreat(token_after_key);
        if (m_layout.is(last_key_token, TokenType::KeywordBinaryOperator))
            m_marks.tokens[last_key_token].role = TokenRole::KeyColon;
        else if (m_layout.is(token_after_key, TokenType::Colon))
            m_marks.tokens[token_after_key].role = TokenRole::KeyColon;
    }

    static bool is_assignment(const ast::ASTGraph::Variant& node) {
        return holds_any<ast::AssignmentNode, ast::AssignmentAtNode, ast::SetterNode,
                         ast::DeclareVariableWithDefaultNode, ast::VariablePackAssign, ast::GeneratorQualDeclare,
                         ast::RangeAssign>(node);
    }

    // The selector's tokens, one or, with an adverb, several.
    void mark_binary_operator(const src::SourceCodeRange& selector) {
        const auto first_token = m_layout.token_at_start_of(selector);
        if (!first_token)
            return;
        const auto last_token = util::retreat(m_layout.first_token_at_or_after(TextOffset { selector.end.absolute }));
        if (*first_token != last_token) {
            m_marks.tokens[*first_token].role = TokenRole::BinaryOperatorFirstPart;
            m_marks.tokens[last_token].role = TokenRole::BinaryOperatorLastPart;
        } else {
            m_marks.tokens[*first_token].role = m_layout.is(*first_token, TokenType::KeywordBinaryOperator)
                ? TokenRole::KeywordOperator
                : TokenRole::BinaryOperator;
        }
    }

    // The `=` before the value, the last child: `a = 1`, `a[0] = 1`, `|a = 1|`, but not `|a(1)|`.
    void mark_equals_before_value(const ast::ASTGraph::Edges& edges, TokenRole role) {
        std::optional<ast::Index> target, value;
        for (auto child = edges.first_child; child; child = m_graph.edges(ast::Index { *child }).next_sibling) {
            target = value;
            value = ast::Index { *child };
        }
        if (!target)
            return;
        auto token = m_layout.first_token_at_or_after(TextOffset { m_graph.location(*target).end.absolute });
        while (m_layout.is_comment(token) || m_layout.is(token, TokenType::CloseSquare))
            token = util::advance(token);
        if (m_layout.is(token, TokenType::EqualsSign))
            m_marks.tokens[token].role = role;
    }

    // The pipes around a parameter list, `|a, b|`, or `| |`, where the list's range is empty. Past comments,
    // `|a /* c */|`.
    void mark_parameter_list_pipes(const src::SourceCodeRange& range) {
        auto open_pipe = m_layout.first_token_at_or_after(TextOffset { range.begin.absolute });
        auto close_pipe =
            m_layout.first_non_comment_from(m_layout.first_token_at_or_after(TextOffset { range.end.absolute }));
        if (range.begin.absolute == range.end.absolute) {
            if (!m_layout.is(close_pipe, TokenType::Pipe))
                return;
            if (const auto before = m_layout.previous_non_comment(close_pipe);
                before && m_layout.is(*before, TokenType::Pipe))
                open_pipe = *before;
            else
                close_pipe = m_layout.next_non_comment(open_pipe);
        } else {
            const auto before = m_layout.previous_non_comment(open_pipe);
            if (!before)
                return;
            open_pipe = *before;
        }
        if (m_layout.is(open_pipe, TokenType::Pipe) && m_layout.is(close_pipe, TokenType::Pipe)) {
            m_marks.tokens[open_pipe].role = TokenRole::ParameterListOpenPipe;
            m_marks.tokens[close_pipe].role = TokenRole::ParameterListClosePipe;
        }
    }

    // The first token of an item. The brackets around one and the keyword before declarations are its parent's, so
    // it is a `(` if it is bracketed, and the keyword before declarations, as in `var\n a, b;`.
    std::optional<TokenIndex> first_token_of_item(ast::Index item) const {
        const auto& range = m_graph.location(item);
        auto first_token = m_layout.token_at_start_of(range);
        if (first_token) {
            if (const auto keyword = m_layout.previous_non_comment(*first_token);
                keyword && is_declaration(m_layout.tokens[*keyword].type))
                return keyword;
        }
        if (!first_token || !m_graph.is_bracketed(item))
            return first_token;
        auto token_after_item = m_layout.first_token_at_or_after(TextOffset { range.end.absolute });
        const auto in_parens = [&] {
            if (**first_token == 0)
                return false;
            return m_layout.is(util::retreat(*first_token), TokenType::OpenParen)
                && m_layout.is(token_after_item, TokenType::CloseParen);
        };
        while (in_parens()) {
            first_token = util::retreat(*first_token);
            token_after_item = util::advance(token_after_item);
        }
        return first_token;
    }

    std::uint32_t written_indent_of_line_holding(TextOffset offset) const {
        auto line = m_layout.text_between(TextOffset { 0 }, offset);
        if (const auto newline = line.rfind('\n'); newline != std::string_view::npos)
            line.remove_prefix(newline + 1);
        return indent_width(line);
    }

    std::uint32_t written_indent_before(TokenIndex token) const {
        WhitespaceLines whitespace_lines;
        split_whitespace_lines(m_layout.written_whitespace_before(token), whitespace_lines);
        return indent_width(whitespace_lines.back());
    }

    // A `.` after a receiver that starts a line, e.g., `x.a\n.b`, and bracketed, `(x + y)\n.c`.
    void mark_method_chain_line(ast::Index receiver) {
        const auto& range = m_graph.location(receiver);
        const auto receiver_start = m_layout.token_at_start_of(range);
        if (!receiver_start)
            return;
        auto dot = m_layout.first_token_at_or_after(TextOffset { range.end.absolute });
        while (m_layout.is_comment(dot) || m_layout.is(dot, TokenType::CloseParen))
            dot = util::advance(dot);
        const auto dot_line_start =
            m_layout.is(dot, TokenType::Dot) ? m_layout.line_start_if_only_comments_before(dot) : std::nullopt;
        if (!dot_line_start)
            return;
        m_marks.relative_indent_lines.push_back(
            { dot, *receiver_start, RelativeIndentLine::Kind::MethodChain,
              written_indent_before(*dot_line_start)
                  > written_indent_of_line_holding(m_layout.tokens[*receiver_start].begin) });
    }

    // `switch(x) { 1 } { a } { b }`, `x.switch { 1 } { a }`, `case { c } { a }`: every case after the `)`.
    void mark_control_pairs(const MessageWithFunctions& message, const src::SourceCodeRange& message_range) {
        const auto name = message.name;
        const bool postfix = message.postfix;
        // The receiver is the first argument of `switch(x)`, and `case` has none.
        std::uint32_t argument_count { 0 };
        for (auto child = m_graph.edges(message.arguments).first_child; child;
             child = m_graph.edges(ast::Index { *child }).next_sibling)
            ++argument_count;
        const bool is_switch = name == "switch" && argument_count == (postfix ? 0u : 1u);
        const bool is_case = name == "case" && !postfix && argument_count == 0;
        if (!is_switch && !is_case)
            return;
        std::vector<TokenIndex> openers;
        for (auto child = m_graph.edges(message.functions).first_child; child;
             child = m_graph.edges(ast::Index { *child }).next_sibling) {
            const ast::Index function { *child };
            if (!std::holds_alternative<ast::BlockNode>(m_graph.payload(function)))
                return;
            const auto open_brace = m_layout.token_at_start_of(m_graph.location(function));
            if (!open_brace || !m_layout.is_open_brace(*open_brace))
                return;
            openers.push_back(*open_brace);
        }
        if (openers.size() < 2)
            return;
        const auto message_start = m_layout.first_token_at_or_after(TextOffset { message_range.begin.absolute });
        m_marks.control_pairs.push_back({ message_start, openers.front(), static_cast<std::uint32_t>(openers.size()) });
        // A function starting a line is placed from the message's line: each test and the default, and an action
        // after a comment.
        for (const auto open_brace : openers)
            m_marks.relative_indent_lines.push_back(
                { open_brace, message_start, RelativeIndentLine::Kind::ControlFunction, false });
    }

    // The lines of a string after its first, `"a"\n"b"`.
    void mark_string_continuation_lines(ast::Index first_line) {
        const auto string_start = m_layout.token_at_start_of(m_graph.location(first_line));
        if (!string_start)
            return;
        for (auto child = m_graph.edges(first_line).next_sibling; child;
             child = m_graph.edges(ast::Index { *child }).next_sibling)
            if (const auto line_start = m_layout.token_at_start_of(m_graph.location(ast::Index { *child }));
                line_start && m_layout.line_start_if_only_comments_before(*line_start))
                m_marks.relative_indent_lines.push_back(
                    { *line_start, *string_start, RelativeIndentLine::Kind::StringContinuation, false });
    }

    // The top-level items of an scd file, written in `( )`.
    void mark_regions() {
        const auto root = m_graph.root_any();
        if (!root)
            return;
        // The root's items, or a file written as a function's body's.
        auto region_list = *root;
        if (std::holds_alternative<ast::FileAsFunction>(m_graph.payload(*root))) {
            const auto [arguments, body] = m_graph.children(*m_graph.as_a<ast::FileAsFunctionIndex>(*root));
            region_list = ast::Index { *body };
        } else if (!std::holds_alternative<ast::RegionList>(m_graph.payload(*root))) {
            return;
        }
        for (auto child = m_graph.edges(region_list).first_child; child;
             child = m_graph.edges(ast::Index { *child }).next_sibling)
            if (const auto open_paren = region_open_paren(ast::Index { *child }))
                mark_region(*open_paren);
    }

    // The `(` of a region: an ImmediatelyEvaluatedBlock's, or the brackets around the whole item.
    std::optional<TokenIndex> region_open_paren(ast::Index item) const {
        const auto& range = m_graph.location(item);
        const auto first_token = m_layout.token_at_start_of(range);
        if (!first_token)
            return std::nullopt;
        if (std::holds_alternative<ast::ImmediatelyEvaluatedBlock>(m_graph.payload(item)))
            return m_layout.is(*first_token, TokenType::OpenParen) ? first_token : std::nullopt;
        const auto open_paren = m_layout.previous_non_comment(*first_token);
        auto close_paren = m_layout.first_token_at_or_after(TextOffset { range.end.absolute });
        while (m_layout.is_comment(close_paren) || m_layout.is(close_paren, TokenType::SemiColon))
            close_paren = util::advance(close_paren);
        if (open_paren && m_layout.is(*open_paren, TokenType::OpenParen)
            && m_layout.is(close_paren, TokenType::CloseParen))
            return open_paren;
        return std::nullopt;
    }

    // Also sets region_indented_in_source.
    void mark_region(TokenIndex open_paren) {
        auto& open_paren_marks = m_marks.tokens[open_paren];
        open_paren_marks.opens_region = true;
        std::uint32_t bracket_depth { 0 };
        for (auto token = open_paren; m_layout.tokens.contains(token); token = util::advance(token)) {
            const auto type = m_layout.tokens[token].type;
            if (lex::is_close_bracket(type) && --bracket_depth == 0)
                return;
            // The first line directly inside it, as a deeper line or a comment is placed by something else.
            const bool starts_line = token != open_paren && m_layout.whitespace_before[token].newlines > 0;
            const bool is_code = !lex::is_close_bracket(type) && !lex::is_comment(type);
            if (starts_line && bracket_depth == 1 && is_code) {
                open_paren_marks.region_indented_in_source =
                    written_indent_before(token) > written_indent_of_line_holding(m_layout.tokens[open_paren].begin);
                return;
            }
            if (lex::is_open_bracket(type))
                ++bracket_depth;
        }
    }

    const LayoutDraft& m_layout;
    SyntaxMarks& m_marks;
    const ast::ASTGraph& m_graph;
};

}

// Each mark is only made when an option reads it.
SyntaxMarks mark_syntax(const LayoutDraft& layout, const ast::ASTGraph& graph, const Style& style) {
    // A declaration's keyword starts a list item, as the list after it doesn't include it.
    SyntaxMarks marks;
    marks.tokens.raw().reserve(layout.tokens.size());
    for (const auto& token : layout.tokens.raw())
        (void)marks.tokens.push_back({ TokenRole::None, is_declaration(token.type), false, false });
    const bool indentation = style.indent.kind != Indent::Kind::Keep;
    const bool list_options = any_not_keep(style.multiline_arrays, style.multiline_arguments);
    const bool kr_options = any_not_keep(style.kr_braces, style.kr_parens, style.kr_brackets);
    const bool list_items_and_regions =
        indentation || list_options || kr_options || any_not_keep(style.region_brackets);
    const bool control_functions = indentation || any_not_keep(style.multiline_control_pairs);
    const bool spacing_reads_roles = any_not_keep(
        style.space_around_binary_operators, style.space_around_keyword_operators, style.space_around_assignment,
        style.space_around_parameter_equals, style.space_before_parameters, style.space_inside_pipes,
        style.space_after_parameters, style.space_after_key, style.space_before_closure);
    const bool line_breaks_read_roles =
        list_options || kr_options || any_not_keep(style.parameters_on_brace_line, style.trailing_comma_lists);
    const bool roles = spacing_reads_roles || line_breaks_read_roles;
    if (list_items_and_regions || roles || control_functions)
        SyntaxMarker { layout, marks, graph }.run(indentation, list_items_and_regions, roles, control_functions);
    return marks;
}
}
