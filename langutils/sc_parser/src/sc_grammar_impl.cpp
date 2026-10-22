#include "sc_grammar_impl.hpp"

#include "parser_context.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_lexer/codepoint.hpp"
#include "sc_lexer/codepoint_stream.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_lexer/tokens.hpp"

// Only records the error, so error recovery can decide how to report it, e.g., as a missing semicolon, see
// error_recovery::report.
void sc::ast::parser::parser::report_syntax_error(const context& symbol_cxt) const {
    // One that the recovery didn't report, e.g., when there wasn't a rule to recover with.
    error_recovery::report(cxt);

    std::vector<symbol_kind_type> expected(12);
    expected.resize(static_cast<size_t>(symbol_cxt.expected_tokens(expected.data(), 12)));

    std::vector<int> expected_int(expected.size());
    std::transform(expected.begin(), expected.end(), expected_int.begin(),
                   [](symbol_kind_type r) -> int { return static_cast<int>(r); });

    cxt.unexpected_token_error = UnexpectedToken { symbol_cxt.location(), std::move(expected_int),
                                                   static_cast<int>(symbol_cxt.lookahead().kind_) };
}

void sc::ast::parser::parser::error(const sc::lex::SourceCodeRange&, const std::string&) {
    // TODO: temporary code.
    ////////////////	cxt.error_handler->operator()(cxt.text_info, loc, message);
}


namespace sc::ast::parser::error_recovery {

void region_separator(ParserContext& cxt, sc ::lex::SourceCodeRange last_valid) {
    auto _ = cxt.consume_error();
    cxt.graph->add_diagnostic(diag::Diagnostic::region_missing_semicolon(cxt.text_info, last_valid));
}

void report(ParserContext& cxt) {
    using Symbol = sc::ast::parser::parser::symbol_kind_type;
    // Errors bison doesn't report, e.g., at the same token as the last one, have nothing to report.
    const auto error = cxt.consume_error();
    if (!error)
        return;
    const auto& unexpected = *error;
    const auto got = static_cast<Symbol>(unexpected.got);

    const char* got_name = sc::ast::parser::parser::symbol_name(got);

    std::string expected;
    const auto sz = unexpected.expected.size();
    for (size_t i { 0 }; i < sz; ++i) {
        expected += sc::ast::parser::parser::symbol_name(static_cast<Symbol>(unexpected.expected[i]));
        if (i + 2 == sz)
            expected += ", or ";
        else if (i + 2 < sz)
            expected += ", ";
    }

    cxt.graph->add_diagnostic(diag::Diagnostic::unexpectedToken(cxt.text_info, unexpected.location, expected, got_name));
}

void expr(ParserContext& cxt) { report(cxt); }


sc::lex::SourceCodeRange bracket(ParserContext& cxt, sc::lex::SourceCodeRange open_bracket) {
    report(cxt);
    // The lookahead, which caused the error, might be the match itself.
    if (cxt.previous && cxt.previous->extra == open_bracket)
        return cxt.previous->range;

    while (true) {
        cxt.previous = sc::lex::lexer(cxt.cps, cxt.action);
        if (cxt.previous->extra == open_bracket || cxt.previous->token == sc::lex::TokenType::EndOfFile)
            return cxt.previous->range;
    }
}

void method(ParserContext& cxt, sc::lex::SourceCodeRange open_bracket) {
    report(cxt);
    if (cxt.previous) {
        if (cxt.previous->token == sc::lex::TokenType::CloseCurly) {
            return; // do nothing. Can't guess when brackets mismatch.
        }
    }

    // copy
    sc::lex::CodePointStream new_cps = cxt.cps;

    while (true) {
        sc::lex::CodePointStream peek_cps = new_cps;
        const auto [lex_token, location, extra_location] = sc::lex::lexer(peek_cps, cxt.action);
        if (lex_token == sc::lex::TokenType::CloseCurly) {
            if (extra_location) {
                if (*extra_location == open_bracket) {
                    cxt.previous = { lex_token, location, extra_location };
                    cxt.cps = peek_cps;
                    return;
                }
            }
        } else if (lex_token == sc::lex::TokenType::EndOfFile) {
            cxt.cps = new_cps;
            return;
        }
        new_cps = peek_cps;
    }
}
}
