// Copyright Jordan Henderson 2026
#include "rewrite/rewrite.hpp"

#include "parse.hpp"
#include "rewrite/rewriter.hpp"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace sc::format {

namespace {

// Moves ranges the edits don't touch to where they are in the edited text.
void move_ranges_past_edits(std::vector<TextRange>& ranges, std::vector<Edit> edits) {
    std::sort(edits.begin(), edits.end(),
              [](const Edit& left, const Edit& right) { return *left.begin < *right.begin; });
    const auto moved = [&](TextOffset offset, bool is_range_end) {
        std::int64_t shift { 0 };
        for (const auto& edit : edits) {
            // An insertion at a range's end goes after it.
            if (*edit.end > *offset || (is_range_end && *edit.begin == *offset))
                break;
            shift += static_cast<std::int64_t>(edit.text.size()) - (*edit.end - *edit.begin);
        }
        return TextOffset::from(static_cast<std::int64_t>(*offset) + shift);
    };
    for (auto& range : ranges)
        range = { moved(range.begin, false), moved(range.end, true) };
}

// Throws if two overlap or two insert at the same offset, as their order would be unknown.
std::string apply_edits(std::string_view text, std::vector<Edit> edits) {
    std::sort(edits.begin(), edits.end(), [](const Edit& left, const Edit& right) {
        return std::make_tuple(*left.begin, *left.end) < std::make_tuple(*right.begin, *right.end);
    });
    std::string edited;
    edited.reserve(text.size() + edits.size() * 2);
    TextOffset copied_up_to { 0 };
    std::optional<TextOffset> last_insertion;
    for (const auto& edit : edits) {
        const bool is_insertion = edit.begin == edit.end;
        const bool overlaps_previous = *edit.begin < *copied_up_to;
        const bool invalid_range = *edit.end < *edit.begin || *edit.end > text.size();
        const bool inserts_at_same_offset = is_insertion && last_insertion == edit.begin;
        if (overlaps_previous || invalid_range || inserts_at_same_offset)
            throw InternalError { "the rewrites overlap, please report this" };
        edited += text.substr(*copied_up_to, *edit.begin - *copied_up_to);
        edited += edit.text;
        copied_up_to = edit.end;
        last_insertion = is_insertion ? std::optional { edit.begin } : std::nullopt;
    }
    edited += text.substr(*copied_up_to);
    return edited;
}

// Returns whether the round made any edits.
template <typename MakeEdits>
bool rewrite_round(CodeToFormat& code, Kind kind, bool has_parse_error, const MakeEdits& make_edits) {
    const auto layout = make_layout_draft(*code.graph, code.verbatim_lines);
    AllowedAstChanges changes { false, false, false, false };
    auto edits = make_edits(layout, changes);
    if (edits.empty())
        return false;
    auto new_text = apply_edits(code.text, edits);
    auto reparsed = parse(new_text, kind);
    if (!reparsed.graph || !reparsed.accepted || reparsed.first_error.has_value() != has_parse_error)
        throw InternalError { "the rewrites wouldn't parse, " + reparsed.first_error.value_or("")
                              + ", please report this" };
    if (!same_ast(*code.graph, *reparsed.graph, changes))
        throw InternalError { "the rewrites would change the code, please report this" };
    // The AST holds no comments, so same_ast() can't see one lost or moved past another.
    if (!same_comments(code.text, new_text))
        throw InternalError { "the rewrites would lose or reorder a comment, please report this" };
    move_ranges_past_edits(code.verbatim_lines, std::move(edits));
    code.graph = std::move(reparsed.graph);
    code.text = std::move(new_text);
    return true;
}
}

void apply_rewrites(CodeToFormat& code, Kind kind, const Style& style, bool has_parse_error) {
    // Moving a receiver into the call goes first, so trailing closures see the call it makes. A control message whose
    // receiver is another is rewritten a round after it.
    while (rewrite_round(code, kind, has_parse_error, [&](const LayoutDraft& layout, AllowedAstChanges& changes) {
        return control_prefix_edits(*code.graph, layout, style, changes);
    })) {}
    rewrite_round(code, kind, has_parse_error, [&](const LayoutDraft& layout, AllowedAstChanges& changes) {
        auto edits = parameter_edits(*code.graph, layout, style, changes);
        for (auto& edit : final_semicolon_edits(*code.graph, layout, style))
            edits.push_back(std::move(edit));
        for (auto& edit : region_semicolon_edits(*code.graph, layout, style))
            edits.push_back(std::move(edit));
        for (auto& edit : trailing_closure_edits(*code.graph, layout, style, changes))
            edits.push_back(std::move(edit));
        return edits;
    });
}
}
