#pragma once

#include "sc_lexer/text_info.hpp"
#include "sc_lexer/text_location.hpp"

#include <ostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace sc::diag {
enum struct Severity { Error, Warning };

enum struct DiagnosticKind {
    UnexpectedToken,
    MissingSemiColonBetweenRegions,
};


class Diagnostic {
public:
    using TextInfoPtr = std::shared_ptr<const sc::lex::TextInfo>;
    struct Location {
        TextInfoPtr text_info;
        sc::lex::SourceCodeRange range;
        std::string msg;
    };

    [[nodiscard]] static Diagnostic unexpectedToken(TextInfoPtr text_info, sc::lex::SourceCodeRange loc,
                                                    std::string expected, const char* received);
    [[nodiscard]] static Diagnostic region_missing_semicolon(TextInfoPtr text_info, sc::lex::SourceCodeRange loc);

    [[nodiscard]] static Diagnostic duplicate_class(TextInfoPtr first_text_info, sc::lex::SourceCodeRange first_loc,
                                                    TextInfoPtr second_text_info, sc::lex::SourceCodeRange second_loc);

    [[nodiscard]] static Diagnostic todo(TextInfoPtr, sc::lex::SourceCodeRange loc);

    [[nodiscard]] static Diagnostic missing_intrinsic_class(std::string);
    [[nodiscard]] static Diagnostic missing_common_class(std::string);

    [[nodiscard]] static Diagnostic intrinsic_class_wrong_member_name(TextInfoPtr, sc::lex::SourceCodeRange loc,
                                                                      std::string_view expected);

    [[nodiscard]] static Diagnostic duplicate_extention(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                        TextInfoPtr second_t, sc::lex::SourceCodeRange second_l);

    [[nodiscard]] static Diagnostic super_class_not_defined( //
        std::string name, std::vector<std::pair<TextInfoPtr, sc::lex::SourceCodeRange>>);
    [[nodiscard]] static Diagnostic inheritance_cycle(std::vector<std::pair<TextInfoPtr, sc::lex::SourceCodeRange>>);
    [[nodiscard]] static Diagnostic self_inheritance(TextInfoPtr, sc::lex::SourceCodeRange);

    [[nodiscard]] static Diagnostic duplicate_method_name(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                        TextInfoPtr second_t, sc::lex::SourceCodeRange second_l);

    [[nodiscard]] static Diagnostic duplicate_member(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                        TextInfoPtr second_t, sc::lex::SourceCodeRange second_l);

    [[nodiscard]] static Diagnostic duplicate_member_in_base(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                        TextInfoPtr second_t, sc::lex::SourceCodeRange second_l);

    [[nodiscard]] static Diagnostic default_inheritor_must_have_super(TextInfoPtr first_t,
                                                                      sc::lex::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic root_object_must_not_inherit(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l);
    [[nodiscard]] static Diagnostic unexpected_slot_def(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l);

    // intrinsic
    [[nodiscard]] static Diagnostic incorrect_super_class(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                          std::string_view expected);
    [[nodiscard]] static Diagnostic incorrect_super_class(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l);
    [[nodiscard]] static Diagnostic incorrect_slot_def(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                       std::string_view expected);
    [[nodiscard]] static Diagnostic incorrect_slot_def(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic non_terminating_return(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic message_without_receiver(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic not_constant(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic variadic_argument_not_last(TextInfoPtr variadic_t,
                                                               sc::lex::SourceCodeRange variadic_l,
                                                               TextInfoPtr positional_t,
                                                               sc::lex::SourceCodeRange positional_l);

    [[nodiscard]] static Diagnostic perform_selector_not_symbol(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l);
    // When the selector is a string, the suggestion is the symbol with the same text.
    [[nodiscard]] static Diagnostic perform_selector_not_symbol(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                                std::string_view string);

    [[nodiscard]] static Diagnostic undefined_variable(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                       std::string_view name);


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

    [[nodiscard]] std::string to_string() const;

private:
    Diagnostic(const char* name, Severity severity, std::vector<Location> locations = { },
               std::string extra = { }) noexcept;
    std::vector<Location> m_locations;
    std::string extra_info;
    const char* m_name;
    Severity m_severity;
};


inline std::ostream& operator<<(std::ostream& s, const Diagnostic& d) {
    s << d.to_string();
    s.flush();
    return s;
}

}
