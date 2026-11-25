// Copyright Jordan Henderson 2026
#include "sc_format/format.hpp"

#include "layout/layout_draft.hpp"
#include "parse.hpp"
#include "rewrite/rewrite.hpp"
#include "syntax.hpp"
#include "verbatim_lines.hpp"

#include "sc_lexer/details/tokens.hpp"
#include "sc_parser/ast.hpp"
#include "sc_source/normalise_source.hpp"

#include <algorithm>
#include <tuple>

namespace sc::format {

Kind kind_of(const std::filesystem::path& path) { return path.extension() == ".sc" ? Kind::Class : Kind::Scd; }

Style without_rewrites(Style style) {
    style.parameter_pipes = ParameterPipes::Keep;
    style.parameter_commas = Switch::Keep;
    style.parameter_default_equals = Switch::Keep;
    style.final_semicolon = FinalSemicolon::Keep;
    style.trailing_closures = TrailingClosures::Keep;
    style.control_prefix = Switch::Keep;
    style.region_semicolons = RegionSemicolons::Keep;
    return style;
}

bool rewrites(const Style& style) { return style != without_rewrites(style); }

bool same_tokens(std::string_view original, std::string_view formatted) {
    const src::NormalisedSource normalised_original { std::string { original } };
    const src::NormalisedSource normalised_formatted { std::string { formatted } };
    return non_whitespace_tokens(normalised_original) == non_whitespace_tokens(normalised_formatted);
}

bool same_comments(std::string_view original, std::string_view rewritten) {
    const auto comments_of = [](const src::NormalisedSource& source) {
        auto tokens = non_whitespace_tokens(source);
        tokens.erase(
            std::remove_if(tokens.begin(), tokens.end(),
                           [](const auto& token) { return !lex::is_comment(std::get<lex::TokenType>(token)); }),
            tokens.end());
        return tokens;
    };
    const src::NormalisedSource normalised_original { std::string { original } };
    const src::NormalisedSource normalised_rewritten { std::string { rewritten } };
    return comments_of(normalised_original) == comments_of(normalised_rewritten);
}

Formatted format(std::string_view input, Kind kind, const Style& style, std::optional<LineRange> lines) {
    auto parsed = parse(input, kind);
    if (!parsed.graph || !parsed.accepted)
        return ParseError { parsed.first_error.value_or("the parser stopped") };
    std::optional<ParseError> parse_error;
    if (parsed.first_error)
        parse_error = ParseError { *parsed.first_error };

    // Offsets are into the normalised text, which the parser reads.
    std::string text = parsed.graph->text_info()->source.as_string();
    auto verbatim = collect_verbatim_lines(*parsed.graph, text, std::move(parsed.unparsed_ranges), lines);
    CodeToFormat code { std::move(text), std::move(parsed.graph), std::move(verbatim.ranges) };

    // The checks inside throw InternalError, so a bug in one stops the whole format.
    try {
        apply_rewrites(code, kind, style, parse_error.has_value());
        auto formatted = lay_out_and_print(*code.graph, style, code.verbatim_lines);
        if (!same_tokens(code.text, formatted))
            throw InternalError { "formatting would change the code's tokens, please report this" };
        return Text { std::move(formatted), std::move(parse_error), std::move(verbatim.unparsed_lines) };
    } catch (InternalError& error) { return std::move(error); }
}
}
