#pragma once

#include <ostream>
#include <string_view>

namespace sc::diag {

/// What a piece of printed text is. The printer sets this, a `Style` decides how it looks.
enum struct Role {
    Diagnostic,
    Rule, ///< The lines above and below a diagnostic.
    Error,
    Warning,
    Title,
    Path,
    LineNumber,
    Gutter, ///< The box drawing around the source, and the lines that join multi line ranges.
    Source,
    Secondary,
    Extra,
};

/// How roles are written to a stream.
/// Every piece of text from outside (source, paths, messages) is passed to `text`, so a style can escape it.
class Style {
public:
    virtual ~Style() = default;
    virtual void begin(std::ostream& ss, Role role) const = 0;
    virtual void end(std::ostream& ss, Role role) const = 0;
    virtual void text(std::ostream& ss, std::string_view s) const { ss << s; }
    /// Text written between these is a link to `uri`, which is already percent-encoded. Most styles ignore links.
    virtual void begin_link(std::ostream&, std::string_view /*uri*/) const {}
    virtual void end_link(std::ostream&) const {}
};

class PlainStyle final : public Style {
public:
    void begin(std::ostream&, Role) const override {}
    void end(std::ostream&, Role) const override {}
};

/// Terminal escape codes (SGR, the 16 colour palette).
class AnsiStyle final : public Style {
public:
    void begin(std::ostream& ss, Role role) const override;
    void end(std::ostream& ss, Role role) const override;
    void begin_link(std::ostream& ss, std::string_view uri) const override {}
    void end_link(std::ostream& ss) const override {}
};

/// HTML. Each role is a `<span class="sc-...">`, and the whole diagnostic is a `<pre class="sc-diagnostic">`.
/// Colours are left to the page's CSS. Text is escaped.
/**
CSS classes:
        sc-diagnostic
        sc-rule
        sc-error
        sc-warning
        sc-title
        sc-path
        sc-line-number
        sc-gutter
        sc-source
        sc-secondary
        sc-extra
*/
class HtmlStyle final : public Style {
public:
    void begin(std::ostream& ss, Role role) const override;
    void end(std::ostream& ss, Role role) const override;
    void text(std::ostream& ss, std::string_view s) const override;
    void begin_link(std::ostream& ss, std::string_view uri) const override;
    void end_link(std::ostream& ss) const override;
};

// When used as a default argument.
const Style& plain_style();

} // namespace sc::diag
