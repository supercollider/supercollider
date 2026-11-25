// Copyright Jordan Henderson 2026
#include "parse.hpp"

#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/sc_parser.hpp"
#include "sc_source/normalise_source.hpp"
#include "sc_source/text_info.hpp"

#include <algorithm>
#include <stdexcept>
#include <tuple>

namespace sc::format {

namespace {

TextRange text_range(const src::SourceCodeRange& range) {
    return { TextOffset { range.begin.absolute }, TextOffset { range.end.absolute } };
}

// See Parsed::unparsed_ranges.
std::vector<TextRange> unparsed_ranges_of(const ast::ASTGraph& graph) {
    std::vector<TextRange> ranges;
    graph.flat_walk([&](const ast::ASTGraph::Edges&, const ast::ASTGraph::Variant& node,
                        const src::SourceCodeRange& range, std::size_t) {
        if (std::holds_alternative<ast::Error>(node))
            ranges.push_back(text_range(range));
    });
    const auto& source_tokens = *graph.source_tokens();
    for (const auto& range : source_tokens.unmatched_brackets)
        ranges.push_back(text_range(range));
    // An added closing bracket, from the bracket it closes, as what comes after it may belong inside.
    for (const auto& source_token : source_tokens.tokens.raw())
        if (source_token.inserted()) {
            const auto opener = source_token.matching();
            ranges.push_back({ opener ? TextOffset { source_tokens.tokens[*opener].range.begin.absolute }
                                      : TextOffset { source_token.range.begin.absolute },
                               TextOffset { source_token.range.end.absolute } });
        }
    for (const auto& diagnostic : graph.diagnostics())
        if (diagnostic.severity() == diag::Severity::Error)
            for (const auto& location : diagnostic.locations())
                ranges.push_back(text_range(location.range));
    std::sort(ranges.begin(), ranges.end(),
              [](const TextRange& left, const TextRange& right) { return *left.begin < *right.begin; });
    return ranges;
}
}

Parsed parse(std::string_view text, Kind kind) {
    Parsed parsed { nullptr, false, std::nullopt, {} };
    try {
        auto info = std::make_shared<const src::TextInfo>(
            src::NormalisedSource { std::string { text } }, src::FileCodeLocation {}, std::nullopt,
            kind == Kind::Class ? src::TextKind::ClassFile : src::TextKind::ScdFile);
        std::tie(parsed.graph, parsed.accepted) = ast::parse(info, ast::KeepTokens::Yes);
    } catch (const std::length_error& error) {
        parsed.first_error = error.what();
        return parsed;
    }
    if (parsed.graph)
        for (const auto& diagnostic : parsed.graph->diagnostics()) {
            if (diagnostic.severity() != diag::Severity::Error)
                continue;
            if (diagnostic.locations().empty()) {
                parsed.first_error = std::string { diagnostic.name() };
                break;
            }
            const auto& location = diagnostic.locations().back();
            parsed.first_error = "line " + std::to_string(location.range.begin.line_number + 1) + ": "
                + (location.msg.empty() ? std::string { diagnostic.name() } : location.msg);
            break;
        }
    if (!parsed.graph || !parsed.accepted) {
        if (!parsed.first_error)
            parsed.first_error = "the parser stopped";
        return parsed;
    }
    if (parsed.first_error)
        parsed.unparsed_ranges = unparsed_ranges_of(*parsed.graph);
    return parsed;
}

}
