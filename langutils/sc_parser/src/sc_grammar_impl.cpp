#include "sc_grammar_impl.hpp"

#include "parser_context.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_lexer/tokens.hpp"

// Only records the error, so error recovery can decide how to report it, e.g., as a missing semicolon, see
// error_recovery::report.
void sc::ast::parser::parser::report_syntax_error(const context& symbol_cxt) const {
    // One that the recovery didn't report, e.g., when there wasn't a rule to recover with.
    error_recovery::report(cxt);

    // Large enough for every token, as bison returns none when there are more than it is given room for.
    std::vector<symbol_kind_type> expected(YYNTOKENS);
    expected.resize(static_cast<size_t>(symbol_cxt.expected_tokens(expected.data(), YYNTOKENS)));

    std::vector<int> expected_int(expected.size());
    std::transform(expected.begin(), expected.end(), expected_int.begin(),
                   [](symbol_kind_type r) -> int { return static_cast<int>(r); });

    const bool at_inserted = cxt.lookahead && cxt.tokens[*cxt.lookahead].inserted();
    std::optional<UnexpectedToken::Before> before;
    if (cxt.lookahead && *cxt.lookahead > 0) {
        const auto& token = cxt.tokens[*cxt.lookahead - 1];
        if (!token.inserted() && !sc::lex::is_error(token.token))
            before = UnexpectedToken::Before {
                static_cast<int>(symbol_type { to_parser_token(token.token), sc::ast::LexerToken {}, {} }.kind()),
                *cxt.lookahead - 1
            };
    }
    cxt.unexpected_token_error =
        UnexpectedToken { symbol_cxt.location(), std::move(expected_int),
                          static_cast<int>(symbol_cxt.lookahead().kind_), at_inserted, before };
}

void sc::ast::parser::parser::error(const sc::lex::SourceCodeRange&, const std::string&) {
    // TODO: temporary code.
    ////////////////	cxt.error_handler->operator()(cxt.text_info, loc, message);
}

namespace sc::ast::parser {

namespace {
// Uses the directives in [begin, end), all in the same place, so the last is used and the others are duplicates.
LanguageVersion use_versions(ParserContext& cxt, std::vector<VersionDirective>::iterator begin,
                             std::vector<VersionDirective>::iterator end) {
    assert(begin != end);
    for (auto it = begin; it != end; ++it) {
        it->used = true;
        if (it != begin)
            cxt.graph->add_diagnostic(diag::Diagnostic::duplicate_version(cxt.text_info, (it - 1)->range, it->range));
    }
    return (end - 1)->version.value_or(LanguageVersion::Legacy);
}
}

void ParserContext::claim_file_version() {
    if (mode != Mode::ClassLibrary) {
        for (auto& d : directives) {
            d.used = true;
            graph->add_diagnostic(diag::Diagnostic::version_in_interpreted_code(text_info, d.range));
        }
        return;
    }
    const auto end = std::find_if(directives.begin(), directives.end(), [](const auto& d) { return d.after; });
    if (end != directives.begin())
        file_version = use_versions(*this, directives.begin(), end);
}

LanguageVersion ParserContext::take_version(sc::lex::SourceCodeRange open_curly) {
    const auto begin = std::lower_bound(directives.begin(), directives.end(), open_curly.end,
                                        [](const auto& d, const auto& l) { return d.range.begin < l; });
    const auto end = std::find_if(begin, directives.end(), [&](const auto& d) { return d.after != open_curly; });
    if (begin == end)
        return file_version.value_or(LanguageVersion::Legacy);
    return use_versions(*this, begin, end);
}

void ParserContext::report_misplaced_versions() {
    for (const auto& d : directives)
        if (!d.used)
            graph->add_diagnostic(diag::Diagnostic::misplaced_version(text_info, d.range));
}

}


namespace sc::ast::parser::error_recovery {

void region_separator(ParserContext& cxt, sc ::lex::SourceCodeRange last_valid) {
    cxt.graph->add_diagnostic(diag::Diagnostic::region_missing_semicolon(cxt.text_info, last_valid));
}

namespace {
using Symbol = sc::ast::parser::parser::symbol_kind_type;
using S = sc::ast::parser::parser::symbol_kind;

// The tokens that start an expression, collapsed into one name when the core ones are all expected.
constexpr std::array expression_start {
    S::S_NAME,
    S::S_CLASSNAME,
    S::S_INTEGER,
    S::S_INTEGER_RADIX,
    S::S_HEXADECIMAL,
    S::S_FLOAT,
    S::S_FLOAT_RADIX,
    S::S_FLOAT_EXPONENT,
    S::S_FLOAT_INF,
    S::S_ACCIDENTAL_STEPS,
    S::S_ACCIDENTAL_CENTS,
    S::S_SYMBOL_QUOTE,
    S::S_SYMBOL_SLASH,
    S::S_STRINGLINE,
    S::S_ASCII,
    S::S_CURRYARG,
    S::S_NIL,
    S::S_TRUE,
    S::S_FALSE,
    S::S_PI,
    S::S_OPENPAREN,
    S::S_OPENSQUARE,
    S::S_OPENCURLY,
    S::S_BEGINCLOSEDFUNC,
    S::S_HASH,
    S::S_TILDE,
    S::S_MINUS,
    S::S_BACKTICK,
    S::S_WHILE,
};
constexpr std::array expression_core { S::S_NAME, S::S_INTEGER, S::S_OPENPAREN };

constexpr std::array binary_operator {
    S::S_BINOP,    S::S_KEYBINOP, S::S_MINUS, S::S_LESSTHAN,     S::S_GREATERTHAN,
    S::S_MULTIPLY, S::S_ADD,      S::S_PIPE,  S::S_READWRITEVAR,
};
constexpr std::array binary_operator_core { S::S_BINOP };

template <typename Container> bool contains(const Container& c, Symbol s) {
    return std::find(c.begin(), c.end(), s) != c.end();
}

// What separates the items in the innermost bracket around the token at i: `,` in an array or a call's arguments, `;`
// elsewhere, e.g., in a function or brackets around expressions.
std::string_view separator_inside(const ParserContext& cxt, std::uint32_t i) {
    using T = sc::lex::TokenType;
    for (auto j = static_cast<std::int64_t>(i); j >= 0; --j) {
        const auto& token = cxt.tokens[static_cast<std::size_t>(j)];
        // Skips a bracketed part, continuing before its opening bracket.
        if (sc::lex::is_close_bracket(token.token)) {
            if (const auto m = token.match())
                j = *m;
            continue;
        }
        if (!sc::lex::is_open_bracket(token.token))
            continue;
        if (token.token == T::OpenSquare)
            return ",";
        if (token.token == T::OpenParen && j > 0) {
            const auto before = cxt.tokens[static_cast<std::size_t>(j - 1)].token;
            if (before == T::Name || before == T::ClassName || before == T::Dot)
                return ",";
        }
        return ";";
    }
    return ";";
}

// How the token is named in a message, or nothing for the parser's own tokens, which the user never writes.
std::optional<std::string_view> token_name(Symbol s) {
    switch (s) {
    case S::S_YYEOF:
        return "the end of the file";
    case S::S_OPENCURLY:
        return "`{`";
    case S::S_CLOSECURLY:
        return "`}`";
    case S::S_OPENSQUARE:
        return "`[`";
    case S::S_CLOSESQUARE:
        return "`]`";
    case S::S_OPENPAREN:
        return "`(`";
    case S::S_CLOSEPAREN:
        return "`)`";
    case S::S_SEMICOLON:
        return "`;`";
    case S::S_NONLOCALRETURN:
        return "`^`";
    case S::S_COMMA:
        return "`,`";
    case S::S_HASH:
        return "`#`";
    case S::S_TILDE:
        return "`~`";
    case S::S_NAME:
        return "a name";
    case S::S_INTEGER:
    case S::S_INTEGER_RADIX:
    case S::S_HEXADECIMAL:
        return "an integer";
    case S::S_FLOAT:
    case S::S_FLOAT_RADIX:
    case S::S_FLOAT_EXPONENT:
        return "a float";
    case S::S_FLOAT_INF:
        return "`inf`";
    case S::S_ACCIDENTAL_STEPS:
    case S::S_ACCIDENTAL_CENTS:
        return "an accidental";
    case S::S_SYMBOL_QUOTE:
    case S::S_SYMBOL_SLASH:
        return "a symbol";
    case S::S_STRINGLINE:
        return "a string";
    case S::S_ASCII:
        return "a character";
    case S::S_PRIMITIVENAME:
        return "a primitive";
    case S::S_CLASSNAME:
        return "a class name";
    case S::S_CURRYARG:
        return "`_`";
    case S::S_VAR:
        return "`var`";
    case S::S_ARG:
        return "`arg`";
    case S::S_CLASSVAR:
        return "`classvar`";
    case S::S_CONST:
        return "`const`";
    case S::S_NIL:
        return "`nil`";
    case S::S_TRUE:
        return "`true`";
    case S::S_FALSE:
        return "`false`";
    case S::S_PI:
        return "`pi`";
    case S::S_ELLIPSIS:
        return "`...`";
    case S::S_DOTDOT:
        return "`..`";
    case S::S_BEGINCLOSEDFUNC:
        return "`#{`";
    case S::S_LEFTARROW:
        return "`<-`";
    case S::S_WHILE:
        return "`while`";
    case S::S_COLON:
        return "`:`";
    case S::S_EQUALSSIGN:
        return "`=`";
    case S::S_BINOP:
        return "a binary operator";
    case S::S_KEYBINOP:
        return "a keyword binary operator";
    case S::S_MINUS:
        return "`-`";
    case S::S_LESSTHAN:
        return "`<`";
    case S::S_GREATERTHAN:
        return "`>`";
    case S::S_MULTIPLY:
        return "`*`";
    case S::S_ADD:
        return "`+`";
    case S::S_PIPE:
        return "`|`";
    case S::S_READWRITEVAR:
        return "`<>`";
    case S::S_DOT:
        return "`.`";
    case S::S_BACKTICK:
        return "a backtick";
    default:
        return std::nullopt;
    }
}

// Replaces the group's tokens with its name, when the core ones are all expected, so the list reads as a category.
template <size_t N, size_t M>
void collapse(std::vector<Symbol>& expected, std::vector<std::string_view>& names, std::string_view name,
              const std::array<Symbol, N>& group, const std::array<Symbol, M>& core) {
    const auto has = [&](Symbol s) { return std::find(expected.begin(), expected.end(), s) != expected.end(); };
    if (!std::all_of(core.begin(), core.end(), has))
        return;
    names.push_back(name);
    expected.erase(std::remove_if(expected.begin(), expected.end(),
                                  [&](Symbol s) { return std::find(group.begin(), group.end(), s) != group.end(); }),
                   expected.end());
}

// E.g., "an expression, `)`, or `,`", with tokens of the same name, e.g., the kinds of integer, named once.
std::string expected_list(std::vector<Symbol> expected) {
    std::vector<std::string_view> names;
    collapse(expected, names, "an expression", expression_start, expression_core);
    collapse(expected, names, "a binary operator", binary_operator, binary_operator_core);
    // A number may be followed by pi, e.g., 2 pi, but suggesting it after every number is only noise.
    expected.erase(std::remove(expected.begin(), expected.end(), S::S_PI), expected.end());
    for (const auto s : expected) {
        const auto name = token_name(s);
        if (name && std::find(names.begin(), names.end(), *name) == names.end())
            names.push_back(*name);
    }
    if (names.empty())
        return "something else";

    std::string list;
    for (size_t i { 0 }; i < names.size(); ++i) {
        list += names[i];
        if (i + 2 == names.size())
            list += names.size() == 2 ? " or " : ", or ";
        else if (i + 2 < names.size())
            list += ", ";
    }
    return list;
}

// The token as written, on one line and shortened, e.g., a multiline string.
std::string found_text(const ParserContext& cxt, Symbol got, sc::lex::SourceCodeRange location) {
    if (got == S::S_YYEOF)
        return "the end of the file";
    constexpr size_t max_size = 20;
    const auto text = cxt.text_info->read(location);
    const auto line = text.substr(0, std::min(text.find('\n'), max_size));
    return "`" + std::string { line } + (line.size() < text.size() ? "...`" : "`");
}
}

void report(ParserContext& cxt) {
    // Errors bison doesn't report, e.g., at the same token as the last one, have nothing to report.
    const auto error = cxt.consume_error();
    if (!error)
        return;
    const auto& unexpected = *error;
    const auto got = static_cast<Symbol>(unexpected.got);
    // The lexer has reported it, or the bracket was unclosed, see tokenise.
    if (got == S::S_LEXER_ERROR || unexpected.at_inserted)
        return;

    std::vector<Symbol> expected(unexpected.expected.size());
    std::transform(unexpected.expected.begin(), unexpected.expected.end(), expected.begin(),
                   [](int i) { return static_cast<Symbol>(i); });

    if (unexpected.before) {
        const auto before = static_cast<Symbol>(unexpected.before->kind);
        const auto before_location = cxt.tokens[unexpected.before->index].range;
        const auto expects = [&](Symbol s) { return contains(expected, s); };
        const bool expects_expression = std::all_of(expression_core.begin(), expression_core.end(), expects);
        const bool got_expression = contains(expression_start, got) && !contains(binary_operator, got);

        // E.g., `1 2`.
        if (got_expression && !expects_expression && expects(S::S_BINOP)) {
            const auto usual = separator_inside(cxt, unexpected.before->index);
            const auto separator =
                expects(usual == "," ? S::S_COMMA : S::S_SEMICOLON) ? std::optional { usual } : std::nullopt;
            cxt.graph->add_diagnostic(
                diag::Diagnostic::missing_operator(cxt.text_info, before_location, unexpected.location,
                                                   found_text(cxt, got, unexpected.location), separator));
            return;
        }
        // E.g., `1 + }`. Where a `var` could go, e.g., after an argument list's closing `|`, it isn't an operator.
        if (!got_expression && expects_expression && !expects(S::S_VAR) && contains(binary_operator, before)) {
            cxt.graph->add_diagnostic(diag::Diagnostic::missing_expression_after(
                cxt.text_info, before_location, unexpected.location, found_text(cxt, got, unexpected.location)));
            return;
        }
    }

    cxt.graph->add_diagnostic(diag::Diagnostic::unexpectedToken(cxt.text_info, unexpected.location,
                                                                expected_list(std::move(expected)),
                                                                found_text(cxt, got, unexpected.location)));
}

void expr(ParserContext& cxt) { report(cxt); }


namespace {
// The bracket matching the opening one at open_bracket, which is at or before the lookahead.
std::uint32_t matching(const ParserContext& cxt, sc::lex::SourceCodeRange open_bracket) {
    auto i = cxt.lookahead.value_or(cxt.next);
    while (i > 0 && !(cxt.tokens[i].range == open_bracket && sc::lex::is_open_bracket(cxt.tokens[i].token)))
        --i;
    assert(cxt.tokens[i].match());
    return *cxt.tokens[i].match();
}

// Skips past the bracket matching the opening one, which the lookahead might be.
sc::lex::SourceCodeRange skip_to_match(ParserContext& cxt, sc::lex::SourceCodeRange open_bracket) {
    const auto close = matching(cxt, open_bracket);
    cxt.next = close + 1;
    cxt.separator_inserted = false;
    return cxt.tokens[close].range;
}
}

sc::lex::SourceCodeRange bracket(ParserContext& cxt, sc::lex::SourceCodeRange open_bracket) {
    report(cxt);
    return skip_to_match(cxt, open_bracket);
}

sc::lex::SourceCodeRange method(ParserContext& cxt, sc::lex::SourceCodeRange open_bracket) {
    report(cxt);
    return skip_to_match(cxt, open_bracket);
}
}
