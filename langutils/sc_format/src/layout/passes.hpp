// Copyright Jordan Henderson 2026
#pragma once

#include "layout/layout_draft.hpp"

#include <string>
#include <vector>

namespace sc::ast {
struct ASTGraph;
}

namespace sc::format {

// Building a LayoutDraft.
void mark_tokens_on_verbatim_lines(LayoutDraft& layout, const std::vector<TextRange>& verbatim_lines);

// The passes of lay_out_and_print(), in order.
[[nodiscard]] SyntaxMarks mark_syntax(const LayoutDraft& layout, const ast::ASTGraph& graph, const Style& style);
void limit_blank_lines_and_add_final_newline(LayoutDraft& layout, const Style& style);
[[nodiscard]] TablesToAlign break_and_join_lines(LayoutDraft& layout, const SyntaxMarks& marks, const Style& style);
void set_spaces_between_tokens(LayoutDraft& layout, const SyntaxMarks& marks, const Style& style);
void align_table_columns(LayoutDraft& layout, const TablesToAlign& tables, const Style& style);
void restore_verbatim_whitespace(LayoutDraft& layout);
[[nodiscard]] std::string print(const LayoutDraft& layout, const SyntaxMarks& marks, const Style& style);
}
