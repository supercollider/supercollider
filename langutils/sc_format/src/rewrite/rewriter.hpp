// Copyright Jordan Henderson 2026
#pragma once

#include "layout/layout_draft.hpp"

#include "sc_parser/ast.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace sc::format {

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

struct Edit;

// What a rewrite may change in the AST, besides the source positions.
struct AllowedAstChanges {
    bool default_brackets; // whether a parameter's default is in brackets
    bool trailing_closures; // whether a message's functions are in its arguments or after them
    bool control_prefix; // whether a control message's receiver is before it or its first argument
    bool switch_tests; // whether a `switch` test is a literal or a function returning it, `\a` or `{ \a }`
};

// The edits the parameter list options make, and what they may change.
[[nodiscard]] std::vector<Edit> parameter_edits(const ast::ASTGraph& graph, const LayoutDraft& layout,
                                                const Style& style, AllowedAstChanges& changes);

// The edits the final semicolon option makes. They change nothing in the AST.
[[nodiscard]] std::vector<Edit> final_semicolon_edits(const ast::ASTGraph& graph, const LayoutDraft& layout,
                                                      const Style& style);

// The edits the region semicolons option makes. They change nothing in the AST.
[[nodiscard]] std::vector<Edit> region_semicolon_edits(const ast::ASTGraph& graph, const LayoutDraft& layout,
                                                       const Style& style);

// The edits the trailing closures option makes, and what they may change.
[[nodiscard]] std::vector<Edit> trailing_closure_edits(const ast::ASTGraph& graph, const LayoutDraft& layout,
                                                       const Style& style, AllowedAstChanges& changes);

// The edits the control prefix option makes, and what they may change.
[[nodiscard]] std::vector<Edit> control_prefix_edits(const ast::ASTGraph& graph, const LayoutDraft& layout,
                                                     const Style& style, AllowedAstChanges& changes);

// Apart from source positions and the changes allowed.
[[nodiscard]] bool same_ast(const ast::ASTGraph& original, const ast::ASTGraph& rewritten, AllowedAstChanges allowed);

// A `switch` test that can go in a function, `\a` to `{ \a }`: a key of the table an inlined `switch` jumps through,
// which the compilers read from either. Not a string or nil, which no table holds.
[[nodiscard]] bool is_switch_test(const ast::ASTGraph::Variant& node);

////////////////////////////////////////////////////////////////////////////////
// Edits
////////////////////////////////////////////////////////////////////////////////

// Replaces the text from begin to end. An insertion has begin == end.
struct Edit {
    TextOffset begin;
    TextOffset end;
    std::string text;
};

inline TextOffset begin_of(const src::SourceCodeRange& range) { return TextOffset { range.begin.absolute }; }
inline TextOffset end_of(const src::SourceCodeRange& range) { return TextOffset { range.end.absolute }; }

inline std::vector<ast::Index> children_of(const ast::ASTGraph& graph, ast::Index parent) {
    std::vector<ast::Index> child_nodes;
    for (auto child = graph.edges(parent).first_child; child; child = graph.edges(ast::Index { *child }).next_sibling)
        child_nodes.push_back(ast::Index { *child });
    return child_nodes;
}

// Edits in groups. A group is dropped whole if one of its edits touches a token on a verbatim line.
class EditGroups {
public:
    void start_group() { m_group = util::advance(m_group); }

    void add(TextOffset begin, TextOffset end, std::string text) {
        m_edits.push_back({ begin, end, std::move(text) });
        m_group_of_edit.push_back(m_group);
    }

    [[nodiscard]] std::vector<Edit> clear_of_verbatim_lines(const LayoutDraft& layout) && {
        if (!layout.any_token_on_verbatim_line)
            return std::move(m_edits);
        util::IndexedVector<EditGroupIndex, std::uint8_t> dropped; // not bool, as vector<bool> has no references
        dropped.resize(*m_group + 1);
        auto group_of_edit = m_group_of_edit.begin();
        for (const auto& edit : m_edits)
            if (const auto group = *group_of_edit++; touches_verbatim_token(layout, edit))
                dropped[group] = 1;
        std::vector<Edit> kept;
        group_of_edit = m_group_of_edit.begin();
        for (auto& edit : m_edits)
            if (!dropped[*group_of_edit++])
                kept.push_back(std::move(edit));
        return kept;
    }

private:
    struct EditGroupTag;
    using EditGroupIndex = util::StrongIndex<std::uint32_t, EditGroupTag>;

    // A token the edit replaces, or one next to it.
    static bool touches_verbatim_token(const LayoutDraft& layout, const Edit& edit) {
        auto nearby = layout.first_token_at_or_after(edit.begin);
        if (*nearby > 0)
            nearby = util::retreat(nearby);
        while (layout.tokens.contains(nearby) && *layout.tokens[nearby].begin <= *edit.end) {
            const auto& token = layout.tokens[nearby];
            if (*token.end >= *edit.begin && token.on_verbatim_line)
                return true;
            nearby = util::advance(nearby);
        }
        return false;
    }

    std::vector<Edit> m_edits;
    std::vector<EditGroupIndex> m_group_of_edit;
    EditGroupIndex m_group { 0 };
};
}
