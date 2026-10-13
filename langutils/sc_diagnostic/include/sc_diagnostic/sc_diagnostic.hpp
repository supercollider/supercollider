#pragma once

#include "sc_lexer/text_info.hpp"
#include "sc_lexer/text_location.hpp"

#include <ostream>
#include <memory>
#include <vector>

namespace sc::diag {
enum struct Severity { Error, ErrorButCanCompileClassLibrary, Warning };

enum struct DiagnosticKind {
    UnexpectedToken,
    MissingSemiColonBetweenRegions,
};


class Diagnostic {
public:
    struct Location {
        std::shared_ptr<const sc::lex::TextInfo> text_info;
        sc::lex::SourceCodeRange range;
        std::string msg;
    };

    [[nodiscard]] static Diagnostic unexpectedToken(std::shared_ptr<const sc::lex::TextInfo> text_info,
                                                    sc::lex::SourceCodeRange loc, std::string expected,
                                                    const char* received);
    [[nodiscard]] static Diagnostic regionMissingSemi(std::shared_ptr<const sc::lex::TextInfo> text_info,
                                                      sc::lex::SourceCodeRange loc);

    [[nodiscard]] static Diagnostic duplicate_class(std::shared_ptr<const sc::lex::TextInfo> first_text_info,
                                                    sc::lex::SourceCodeRange first_loc,
                                                    std::shared_ptr<const sc::lex::TextInfo> second_text_info,
                                                    sc::lex::SourceCodeRange second_loc);
    [[nodiscard]] static Diagnostic todo(std::shared_ptr<const sc::lex::TextInfo>, sc::lex::SourceCodeRange loc);

    Diagnostic() = delete;
    Diagnostic(Diagnostic&&) noexcept = default;
    Diagnostic(const Diagnostic&) = default;
    Diagnostic& operator=(Diagnostic&&) noexcept = default;
    Diagnostic& operator=(const Diagnostic&) = default;
    ~Diagnostic() = default;

    [[nodiscard]] const std::vector<Location>& locations() const;
    [[nodiscard]] Severity severity() const;
    [[nodiscard]] bool fatal() const;
    [[nodiscard]] const std::vector<Location>& extra_locations() const;
    [[nodiscard]] bool has_extra_locations() const;
    [[nodiscard]] const char* name() const;

    friend std::ostream& operator<<(std::ostream& s, const Diagnostic& d);

    std::string to_string() const;

private:
    Diagnostic(const char* name, Severity severity, std::vector<Location> locations = {}) noexcept;
    std::vector<Location> m_locations;
    const char* m_name;
    Severity m_severity;
};


inline std::ostream& operator<<(std::ostream& s, const Diagnostic& d) {
    s << d.to_string();
    s.flush();
    return s;
}

}
