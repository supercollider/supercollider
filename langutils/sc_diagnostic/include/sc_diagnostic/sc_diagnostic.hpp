#pragma once

#include "sc_lexer/text_info.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_lexer/tokens.hpp"

#include <filesystem>
#include <ostream>
#include <memory>
#include <optional>
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

    /// When adding multiple locations to the diagnostic, the final location added is considered the true location.
    struct Location {
        TextInfoPtr text_info;
        sc::lex::SourceCodeRange range;
        std::string msg;
    };

    [[nodiscard]] static Diagnostic unexpectedToken(TextInfoPtr text_info, sc::lex::SourceCodeRange loc,
                                                    std::string_view expected, std::string_view found);
    // The lexer's error tokens, e.g., an unclosed string, at the token's range.
    [[nodiscard]] static Diagnostic lexer_error(TextInfoPtr text_info, sc::lex::TokenType,
                                                sc::lex::SourceCodeRange loc);
    // The first character that isn't valid UTF-8, with how many others in the file aren't either.
    [[nodiscard]] static Diagnostic invalid_utf8(TextInfoPtr text_info, sc::lex::SourceCodeRange first,
                                                 std::size_t others);
    // A closing bracket without an opening one.
    [[nodiscard]] static Diagnostic unmatched_bracket(TextInfoPtr text_info, sc::lex::SourceCodeRange close);
    // A closing bracket of the wrong kind, e.g., `(1]`.
    [[nodiscard]] static Diagnostic mismatched_bracket(TextInfoPtr text_info, sc::lex::SourceCodeRange open,
                                                       sc::lex::SourceCodeRange close);
    // An opening bracket without a closing one, before the closing bracket of the one around it, or the end.
    [[nodiscard]] static Diagnostic unclosed_bracket(TextInfoPtr text_info, sc::lex::SourceCodeRange open,
                                                     std::optional<sc::lex::SourceCodeRange> outer_close);
    // A warning, as the inserted semicolon only separates interpreted regions, so the code is still as written.
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

    [[nodiscard]] static Diagnostic unbounded_range(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic range_too_many_arguments(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic range_argument_not_value(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l);

    // A warning, the last element of `Event[...]` has no value.
    [[nodiscard]] static Diagnostic event_key_without_value(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic variadic_argument_not_last(TextInfoPtr variadic_t,
                                                               sc::lex::SourceCodeRange variadic_l,
                                                               TextInfoPtr positional_t,
                                                               sc::lex::SourceCodeRange positional_l);

    [[nodiscard]] static Diagnostic perform_selector_not_symbol(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l);
    // When the selector is a string, the suggestion is the symbol with the same text.
    [[nodiscard]] static Diagnostic perform_selector_not_symbol(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                                std::string_view string);

    [[nodiscard]] static Diagnostic duplicate_variable(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                       TextInfoPtr second_t, sc::lex::SourceCodeRange second_l);

    [[nodiscard]] static Diagnostic undefined_variable(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                       std::string_view name);

    // `var f = f`, a variable can only refer to itself from a function in its initial value, e.g., `var f = { f }`.
    [[nodiscard]] static Diagnostic used_in_own_initial_value(TextInfoPtr t, sc::lex::SourceCodeRange l);

    // `this = 1` or `#this = a`, see declared_special_variable.
    [[nodiscard]] static Diagnostic assigned_special_variable(TextInfoPtr t, sc::lex::SourceCodeRange l);
    // `var this`, `|thisProcess|`, ..., `this`, `super` and the pseudo variables, e.g., `thisProcess`, are set by the
    // language.
    [[nodiscard]] static Diagnostic declared_special_variable(TextInfoPtr t, sc::lex::SourceCodeRange l);

    // `_` that isn't the receiver, an argument, or an element of a call, so isn't partially applied.
    [[nodiscard]] static Diagnostic curry_outside_call(TextInfoPtr t, sc::lex::SourceCodeRange l);

    // `var #a, ~b = c`, an environment variable can't be declared.
    [[nodiscard]] static Diagnostic declared_environment_variable(TextInfoPtr t, sc::lex::SourceCodeRange l);

    // Interpreted code, at l, is compiled as if it were written inside `Interpreter:functionCompileContext`, but the
    // class library doesn't define it, or it is poisoned.
    [[nodiscard]] static Diagnostic missing_interpreter_context(TextInfoPtr t, sc::lex::SourceCodeRange l);

    // `^1` in interpreted code, including inside its functions, there is no method to return from.
    [[nodiscard]] static Diagnostic return_in_interpreted_code(TextInfoPtr t, sc::lex::SourceCodeRange l);

    // A warning, legacy code using a function's argument or variable before its declaration, e.g., `var a = b, b = 1`,
    // as the old compiler allowed. It reads the variable, which 3.16 doesn't.
    [[nodiscard]] static Diagnostic used_before_declaration(TextInfoPtr declaration_t,
                                                            sc::lex::SourceCodeRange declaration, TextInfoPtr use_t,
                                                            sc::lex::SourceCodeRange use);

    // `//# VERSION` not followed by `major.minor`, e.g., `//# VERSION 3`.
    [[nodiscard]] static Diagnostic malformed_version(TextInfoPtr t, sc::lex::SourceCodeRange l);

    // A version after the latest release, whose rules the compiler doesn't know.
    [[nodiscard]] static Diagnostic unknown_version(TextInfoPtr t, sc::lex::SourceCodeRange l, std::string_view latest);

    // A warning, two versions for the same class or file, the second is used.
    [[nodiscard]] static Diagnostic duplicate_version(TextInfoPtr t, sc::lex::SourceCodeRange first,
                                                      sc::lex::SourceCodeRange second);

    // A warning, a version that isn't the first thing in the file or in a class's body, so applies to nothing.
    [[nodiscard]] static Diagnostic misplaced_version(TextInfoPtr t, sc::lex::SourceCodeRange l);

    // A warning, interpreted code is given its version when it is compiled.
    [[nodiscard]] static Diagnostic version_in_interpreted_code(TextInfoPtr t, sc::lex::SourceCodeRange l);


    Diagnostic() = delete;
    Diagnostic(Diagnostic&&) noexcept = default;
    Diagnostic(const Diagnostic&) = default;
    Diagnostic& operator=(Diagnostic&&) noexcept = default;
    Diagnostic& operator=(const Diagnostic&) = default;
    ~Diagnostic() = default;

    [[nodiscard]] const std::vector<Location>& locations() const;
    [[nodiscard]] Severity severity() const;
    [[nodiscard]] bool fatal() const;
    [[nodiscard]] bool within(const TextInfoPtr& text, sc::lex::SourceCodeRange range) const;
    [[nodiscard]] const std::vector<Location>& extra_locations() const;
    [[nodiscard]] bool has_extra_locations() const;
    [[nodiscard]] const char* name() const;

    friend std::ostream& operator<<(std::ostream& s, const Diagnostic& d);

    [[nodiscard]] std::string to_string() const;
    // Paths are written relative to relative_to, rather than as absolute file uris, e.g., so test output doesn't
    // depend on where the source is.
    [[nodiscard]] std::string to_string(const std::filesystem::path& relative_to) const;

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
