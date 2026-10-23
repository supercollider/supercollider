// Copyright Jordan Henderson 2026
#include "parser_context.hpp"

#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_lexer/lexer.hpp"

#include <charconv>
#include <stdexcept>
#include <string>

namespace sc::ast::parser {

namespace {
using sc::lex::SourceCodeRange;
using sc::lex::TokenType;

// Drops the whitespace and comments, which the parser never sees, other than version comments, see tokenise.
struct Action {
    struct Output {
        TokenType token;
        SourceCodeRange range;
    };

    template <TokenType T> [[nodiscard]] std::optional<Output> process(SourceCodeRange loc) {
        if constexpr (T != TokenType::VersionComment && (sc::lex::is_whitespace(T) || sc::lex::is_comment(T)))
            return std::nullopt;
        else
            return Output { T, loc };
    }
};

// The version in a version comment, `//# VERSION 3.16`, reporting it if it isn't `major.minor` or is unknown.
std::optional<LanguageVersion> parse_version(const std::shared_ptr<const sc::lex::TextInfo>& text_info, ASTGraph& graph,
                                             SourceCodeRange r) {
    auto text = text_info->read(r);
    text.remove_prefix(std::string_view { "//# VERSION" }.size());
    const auto skip_blank = [&] {
        while (!text.empty() && (text.front() == ' ' || text.front() == '\t'))
            text.remove_prefix(1);
    };
    const auto number = [&]() -> std::optional<unsigned> {
        unsigned n { 0 };
        const auto [end, ec] = std::from_chars(text.data(), text.data() + text.size(), n);
        if (ec != std::errc {})
            return std::nullopt;
        text.remove_prefix(static_cast<size_t>(end - text.data()));
        return n;
    };

    skip_blank();
    const auto major = number();
    const bool dot = !text.empty() && text.front() == '.';
    if (dot)
        text.remove_prefix(1);
    const auto minor = dot ? number() : std::nullopt;
    skip_blank();
    if (!major || !minor || !text.empty()) {
        graph.add_diagnostic(diag::Diagnostic::malformed_version(text_info, r));
        return std::nullopt;
    }
    const auto version = version_for_release(*major, *minor);
    if (!version)
        graph.add_diagnostic(
            diag::Diagnostic::unknown_version(text_info, r, to_string(language_releases.back().version)));
    return version;
}

bool closes(TokenType open, TokenType close) { return sc::lex::get_closing_bracket(open) == close; }

// Lexes all of the input, reporting the lexer's errors, as the brackets may be paired more than once, see tokenise.
struct Lexed {
    std::shared_ptr<const sc::lex::TextInfo> text_info;
    ASTGraph& graph;
    sc::lex::CodePointStream cps;
    Action action {};

    std::optional<SourceCodeRange> first_invalid_utf8 {};
    size_t invalid_utf8 { 0 };

    // Reported here, as the lexer knows what is wrong, the parser's error at it is dropped, see
    // error_recovery::report. Invalid UTF-8 is reported once, see report_invalid_utf8.
    Action::Output lex() {
        const auto out = sc::lex::lexer(cps, action);
        if (out.token == TokenType::ErInvalidUTF8) {
            if (invalid_utf8++ == 0)
                first_invalid_utf8 = out.range;
        } else if (sc::lex::is_error(out.token)) {
            graph.add_diagnostic(diag::Diagnostic::lexer_error(text_info, out.token, out.range));
        }
        return out;
    }

    void report_invalid_utf8() const {
        if (first_invalid_utf8)
            graph.add_diagnostic(diag::Diagnostic::invalid_utf8(text_info, *first_invalid_utf8, invalid_utf8 - 1));
    }
};

// The next closing bracket after i at its depth, skipping the brackets nested inside, or nothing at the end.
std::optional<TokenType> next_close(const std::vector<Action::Output>& input, size_t i) {
    size_t depth { 0 };
    for (++i; i < input.size(); ++i) {
        const auto t = input[i].token;
        if (sc::lex::is_open_bracket(t))
            ++depth;
        else if (sc::lex::is_close_bracket(t) && depth-- == 0)
            return t;
    }
    return std::nullopt;
}

// Pairs brackets.
struct BracketPairer {
    std::shared_ptr<const sc::lex::TextInfo> text_info;
    const std::vector<Action::Output>& input;
    bool use_indentation; // a different algorythm.

    std::vector<Token> tokens {};
    std::vector<diag::Diagnostic> diagnostics {};

    // Indexes into input vector are uint32_t, we do not support more tokens that this.
    std::vector<std::uint32_t> open {};
    std::vector<std::optional<std::uint32_t>> open_indentation {};

    TokenType innermost() const { return tokens[open.back()].token; }

    void close_bracket(TokenType t, SourceCodeRange r, bool inserted) {
        const auto o = open.back();
        open.pop_back();
        open_indentation.pop_back();
        tokens[o].set_matching_index(static_cast<std::uint32_t>(tokens.size()));
        auto& closing = tokens.emplace_back(t, r);
        closing.set_matching_index(o);
        if (inserted)
            closing.set_inserted();
    }

    void close_bracket(size_t i) {
        const auto t = input[i].token;
        const auto r = input[i].range;
        if (!open.empty() && closes(innermost(), t)) {
            close_bracket(t, r, false);
            return;
        }
        const auto next = next_close(input, i);
        if (open.empty() || (next && closes(innermost(), *next))) {
            diagnostics.push_back(diag::Diagnostic::unmatched_bracket(text_info, r));
            return;
        }
        const auto outer = std::find_if(open.rbegin(), open.rend(), [&](auto o) { return closes(tokens[o].token, t); });
        if (outer == open.rend()) {
            diagnostics.push_back(diag::Diagnostic::mismatched_bracket(text_info, tokens[open.back()].range, r));
            close_bracket(sc::lex::get_closing_bracket(innermost()), r, false);
            return;
        }
        produce_all_unclosed(static_cast<size_t>(open.rend() - outer), r);
        close_bracket(t, r, false);
    }

    void produce_closing_bracket() {
        const auto end = tokens.back().range.end;
        close_bracket(sc::lex::get_closing_bracket(innermost()), { end, end }, true);
    }

    void produce_all_unclosed(size_t kept, std::optional<SourceCodeRange> outer_close) {
        for (size_t i { kept }; i < open.size(); ++i)
            diagnostics.push_back(diag::Diagnostic::unclosed_bracket(text_info, tokens[open[i]].range, outer_close));
        while (open.size() > kept)
            produce_closing_bracket();
    }

    void produce_close_by_indentation(TokenType t, std::uint32_t indentation) {
        while (!open_indentation.empty() && open_indentation.back()) {
            const auto open_at = *open_indentation.back();
            if (open_at < indentation || (open_at == indentation && closes(innermost(), t)))
                return;
            const auto end = tokens.back().range.end;
            diagnostics.push_back(
                diag::Diagnostic::unclosed_bracket_by_indentation(text_info, tokens[open.back()].range, { end, end }));
            produce_closing_bracket();
        }
    }
};

struct PairedBrackets {
    std::vector<Token> tokens;
    std::vector<diag::Diagnostic> diagnostics;
    // Brackets were still open at the end of the file.
    bool unclosed_at_end;
};

PairedBrackets pair_brackets(std::shared_ptr<const sc::lex::TextInfo> text_info,
                             const std::vector<Action::Output>& input, bool use_indentation) {
    BracketPairer p { std::move(text_info), input, use_indentation };
    bool unclosed_at_end { false };
    std::uint32_t line_indentation { 0 };
    for (size_t i { 0 }; i < input.size(); ++i) {
        const auto [t, r] = input[i];
        const bool starts_line = i == 0 || input[i - 1].range.end.line_number < r.begin.line_number;
        if (starts_line) {
            line_indentation = r.begin.column;
            if (use_indentation && t != TokenType::EndOfFile)
                p.produce_close_by_indentation(t, line_indentation);
        }

        if (sc::lex::is_open_bracket(t)) {
            const bool ends_line = i + 1 < input.size() && input[i + 1].range.begin.line_number > r.end.line_number;
            const bool curly = t == TokenType::OpenCurly || t == TokenType::BeginClosedFunction;
            p.open.push_back(static_cast<std::uint32_t>(p.tokens.size()));
            p.open_indentation.push_back(curly && ends_line ? std::optional { line_indentation } : std::nullopt);
            p.tokens.emplace_back(t, r);
        } else if (sc::lex::is_close_bracket(t)) {
            p.close_bracket(i);
        } else if (t == TokenType::EndOfFile) {
            unclosed_at_end = !p.open.empty();
            p.produce_all_unclosed(0, std::nullopt);
            p.tokens.emplace_back(t, r);
        } else {
            p.tokens.emplace_back(t, r);
        }
    }
    return { std::move(p.tokens), std::move(p.diagnostics), unclosed_at_end };
}
}

void tokenise(ParserContext& cxt, sc::lex::CodePointStream cps) {
    Lexed lexed { cxt.text_info, *cxt.graph, std::move(cps) };
    std::vector<Action::Output> input;
    while (true) {
        const auto out = lexed.lex();
        if (out.token == TokenType::VersionComment) {
            const auto after = input.empty() ? std::nullopt : std::optional { input.back().range };
            cxt.directives.push_back({ out.range, parse_version(cxt.text_info, *cxt.graph, out.range), after });
            continue;
        }
        input.push_back(out);
        if (out.token == TokenType::EndOfFile)
            break;
    }
    lexed.report_invalid_utf8();
    if (input.size() > Token::max_tokens)
        throw std::length_error { "source has more than the " + std::to_string(Token::max_tokens)
                                  + " tokens supported" };

    auto pairing = pair_brackets(cxt.text_info, input, false);

    // Failed. Attempt to guess via indentation.
    if (pairing.unclosed_at_end) {
        auto by_indentation = pair_brackets(cxt.text_info, input, true);
        if (!by_indentation.unclosed_at_end && by_indentation.diagnostics.size() <= pairing.diagnostics.size())
            pairing = std::move(by_indentation);
    }
    for (auto& d : pairing.diagnostics)
        cxt.graph->add_diagnostic(std::move(d));

    cxt.tokens = std::move(pairing.tokens);
}
}
