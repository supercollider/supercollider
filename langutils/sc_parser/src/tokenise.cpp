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

struct Lexed {
    std::shared_ptr<const sc::lex::TextInfo> text_info;
    ASTGraph& graph;
    sc::lex::CodePointStream cps;
    Action action {};

    // Those lexed but not taken yet start at head. It is emptied when they are all taken, so keeps its capacity.
    std::vector<Action::Output> ahead {};
    size_t head { 0 };

    std::optional<SourceCodeRange> first_invalid_utf8 {};
    size_t invalid_utf8 { 0 };

    // The token n after the next one.
    const Action::Output& peek(size_t n) {
        while (ahead.size() - head <= n)
            ahead.push_back(lex());
        return ahead[head + n];
    }

    Action::Output next() {
        const auto out = peek(0);
        if (++head == ahead.size()) {
            ahead.clear();
            head = 0;
        }
        return out;
    }

    // Invalid UTF-8 is reported once
    void report_invalid_utf8() const {
        if (first_invalid_utf8)
            graph.add_diagnostic(diag::Diagnostic::invalid_utf8(text_info, *first_invalid_utf8, invalid_utf8 - 1));
    }

private:
    // Reported here, as the lexer knows what is wrong, the parser's error at it is dropped, see
    // error_recovery::report.
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
};

// The next closing bracket at this depth, skipping the brackets nested inside, or nothing at the end.
std::optional<TokenType> next_close(Lexed& lexed) {
    size_t depth { 0 };
    for (size_t i { 0 };; ++i) {
        const auto t = lexed.peek(i).token;
        if (t == TokenType::EndOfFile)
            return std::nullopt;
        if (sc::lex::is_open_bracket(t))
            ++depth;
        else if (sc::lex::is_close_bracket(t) && depth-- == 0)
            return t;
    }
}

struct BracketPairing {
    std::shared_ptr<const sc::lex::TextInfo> text_info;
    ASTGraph& graph;
    std::vector<Token> tokens {};
    // The brackets not closed yet, innermost last.
    std::vector<std::uint32_t> open {};

    TokenType innermost() const { return tokens[open.back()].token; }

    void close(TokenType t, SourceCodeRange r, bool inserted) {
        const auto o = open.back();
        open.pop_back();
        tokens[o].set_match(static_cast<std::uint32_t>(tokens.size()));
        auto& closing = tokens.emplace_back(t, r);
        closing.set_match(o);
        if (inserted)
            closing.set_inserted();
    }

    // Reports the open brackets after the first kept as unclosed, closing each after the last token, so the parser
    // sees them closed. They are reported in source order, before outer_close, or the end.
    void close_unclosed(size_t kept, std::optional<SourceCodeRange> outer_close) {
        for (size_t i { kept }; i < open.size(); ++i)
            graph.add_diagnostic(diag::Diagnostic::unclosed_bracket(text_info, tokens[open[i]].range, outer_close));
        while (open.size() > kept) {
            const auto end = tokens.back().range.end;
            close(sc::lex::get_closing_bracket(innermost()), { end, end }, true);
        }
    }

    // Brackets that don't match are guessed at: when the next closing bracket closes the innermost one, this is an
    // extra one, so it is dropped, e.g., `(1])`; when it closes a bracket around the innermost, the ones inside are
    // unclosed, e.g., `{ (1 }`; otherwise it is the wrong kind, so it closes the innermost, e.g., `{ (1] }`.
    void close_bracket(Lexed& lexed, TokenType t, SourceCodeRange r) {
        if (!open.empty() && closes(innermost(), t)) {
            close(t, r, false);
            return;
        }
        const auto next = next_close(lexed);
        if (open.empty() || (next && closes(innermost(), *next))) {
            graph.add_diagnostic(diag::Diagnostic::unmatched_bracket(text_info, r));
            return;
        }
        const auto outer = std::find_if(open.rbegin(), open.rend(), [&](auto o) { return closes(tokens[o].token, t); });
        if (outer == open.rend()) {
            graph.add_diagnostic(diag::Diagnostic::mismatched_bracket(text_info, tokens[open.back()].range, r));
            close(sc::lex::get_closing_bracket(innermost()), r, false);
            return;
        }
        close_unclosed(static_cast<size_t>(open.rend() - outer), r);
        close(t, r, false);
    }
};
}

void tokenise(ParserContext& cxt, sc::lex::CodePointStream cps) {
    Lexed lexed { cxt.text_info, *cxt.graph, std::move(cps) };
    BracketPairing pairing { cxt.text_info, *cxt.graph };
    while (true) {
        const auto [t, r] = lexed.next();
        if (sc::lex::is_open_bracket(t)) {
            pairing.open.push_back(static_cast<std::uint32_t>(pairing.tokens.size()));
            pairing.tokens.emplace_back(t, r);
        } else if (sc::lex::is_close_bracket(t)) {
            pairing.close_bracket(lexed, t, r);
        } else if (t == TokenType::VersionComment) {
            const auto after = pairing.tokens.empty() ? std::nullopt : std::optional { pairing.tokens.back().range };
            cxt.directives.push_back({ r, parse_version(cxt.text_info, *cxt.graph, r), after });
        } else if (t == TokenType::EndOfFile) {
            pairing.close_unclosed(0, std::nullopt);
            pairing.tokens.emplace_back(t, r);
            break;
        } else {
            pairing.tokens.emplace_back(t, r);
        }
    }
    lexed.report_invalid_utf8();
    if (pairing.tokens.size() > Token::max_tokens)
        throw std::length_error { "source has more than the " + std::to_string(Token::max_tokens)
                                  + " tokens supported" };
    cxt.tokens = std::move(pairing.tokens);
}
}
