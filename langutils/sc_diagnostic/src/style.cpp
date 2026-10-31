#include "sc_diagnostic/style.hpp"

namespace sc::diag {

namespace {

// Select Graphic Rendition parameters, empty when the role is left undecorated.
const char* sgr(Role role) {
    switch (role) {
    case Role::Error:
        return "1;31";
    case Role::Warning:
        return "1;33";
    case Role::Secondary:
    case Role::LineNumber:
    case Role::Gutter:
        return "1;34";
    case Role::Title:
        return "1";
    case Role::Path:
        return "36";
    case Role::Rule:
        return "2";
    case Role::Diagnostic:
    case Role::Source:
    case Role::Extra:
        return "";
    }
    return "";
}

const char* css_class(Role role) {
    switch (role) {
    case Role::Diagnostic:
        return "sc-diagnostic";
    case Role::Rule:
        return "sc-rule";
    case Role::Error:
        return "sc-error";
    case Role::Warning:
        return "sc-warning";
    case Role::Title:
        return "sc-title";
    case Role::Path:
        return "sc-path";
    case Role::LineNumber:
        return "sc-line-number";
    case Role::Gutter:
        return "sc-gutter";
    case Role::Source:
        return "sc-source";
    case Role::Secondary:
        return "sc-secondary";
    case Role::Extra:
        return "sc-extra";
    }
    return "";
}

const char* tag(Role role) { return role == Role::Diagnostic ? "pre" : "span"; }

} // namespace

void AnsiStyle::begin(std::ostream& ss, Role role) const {
    if (const auto* code = sgr(role); *code)
        ss << "\x1b[" << code << 'm';
}

void AnsiStyle::end(std::ostream& ss, Role role) const {
    if (*sgr(role))
        ss << "\x1b[0m";
}


void HtmlStyle::begin_link(std::ostream& ss, std::string_view uri) const {
    ss << "<a href=\"";
    text(ss, uri);
    ss << "\">";
}

void HtmlStyle::end_link(std::ostream& ss) const { ss << "</a>"; }

void HtmlStyle::begin(std::ostream& ss, Role role) const {
    ss << '<' << tag(role) << " class=\"" << css_class(role) << "\">";
}

void HtmlStyle::end(std::ostream& ss, Role role) const { ss << "</" << tag(role) << '>'; }

void HtmlStyle::text(std::ostream& ss, std::string_view s) const {
    std::size_t from { 0 };
    for (std::size_t i = 0; i < s.size(); ++i) {
        const char* replacement { nullptr };
        switch (s[i]) {
        case '&':
            replacement = "&amp;";
            break;
        case '<':
            replacement = "&lt;";
            break;
        case '>':
            replacement = "&gt;";
            break;
        case '"':
            replacement = "&quot;";
            break;
        case '\'':
            replacement = "&#39;";
            break;
        default:
            continue;
        }
        ss << s.substr(from, i - from) << replacement;
        from = i + 1;
    }
    ss << s.substr(from);
}

const Style& plain_style() {
    static const PlainStyle style;
    return style;
}

} // namespace sc::diag
