#pragma once

#include "sc_diagnostic/style.hpp"
#include "sc_source/text_info.hpp"
#include "sc_source/text_location.hpp"

#include <cstddef>
#include <filesystem>
#include <ostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace sc::lex {
enum struct TokenType : unsigned int;
}

namespace sc::diag {
enum struct Severity { Error, Warning };

enum struct DiagnosticKind {
    UnexpectedToken,
    MissingSemiColonBetweenRegions,
};


// Right now this file needs cleaning.
// AI has litered it with obvious comments and the messages are unreadable by a human.
// Do this later because as long as the diagnostic is emitted in the correct place, highlight the code correctly,
//   it is trivial to change.


class Diagnostic {
public:
    using TextInfoPtr = std::shared_ptr<const sc::src::TextInfo>;

    /// When adding multiple locations to the diagnostic, the final location added is considered the true location.
    struct Location {
        TextInfoPtr text_info;
        sc::src::SourceCodeRange range;
        std::string msg;
    };

    Diagnostic() = delete;
    Diagnostic(Diagnostic&&) noexcept = default;
    Diagnostic(const Diagnostic&) = default;
    Diagnostic& operator=(Diagnostic&&) noexcept = default;
    Diagnostic& operator=(const Diagnostic&) = default;
    ~Diagnostic() = default;

    [[nodiscard]] const std::vector<Location>& locations() const;
    [[nodiscard]] Severity severity() const;
    [[nodiscard]] bool fatal() const;
    [[nodiscard]] bool within(const TextInfoPtr& text, sc::src::SourceCodeRange range) const;
    [[nodiscard]] const std::vector<Location>& extra_locations() const;
    [[nodiscard]] bool has_extra_locations() const;
    [[nodiscard]] const char* name() const;

    friend std::ostream& operator<<(std::ostream& s, const Diagnostic& d);

    [[nodiscard]] std::string to_string() const;
    // Paths are written relative to relative_to, rather than as absolute file uris, e.g., so test output doesn't
    // depend on where the source is.
    [[nodiscard]] std::string to_string(const std::filesystem::path& relative_to,
                                        const Style& style = plain_style()) const;
    // Decorated with the style, e.g., AnsiStyle for a terminal or HtmlStyle for a page.
    [[nodiscard]] std::string to_string(const Style& style) const;

    Diagnostic(const char* name, Severity severity, std::vector<Location> locations = {},
               std::string extra = {}) noexcept;

private:
    std::vector<Location> m_locations;
    std::string extra_info;
    const char* m_name;
    Severity m_severity;
};


[[nodiscard]] Diagnostic unexpectedToken(Diagnostic::TextInfoPtr text_info, sc::src::SourceCodeRange loc,
                                         std::string_view expected, std::string_view found);

[[nodiscard]] Diagnostic missing_operator(Diagnostic::TextInfoPtr text_info, sc::src::SourceCodeRange before,
                                          sc::src::SourceCodeRange found, std::string_view found_text,
                                          std::optional<std::string_view> separator);

[[nodiscard]] Diagnostic missing_expression_after(Diagnostic::TextInfoPtr text_info, sc::src::SourceCodeRange op,
                                                  sc::src::SourceCodeRange found, std::string_view found_text);

[[nodiscard]] Diagnostic lexer_error(Diagnostic::TextInfoPtr text_info, sc::lex::TokenType,
                                     sc::src::SourceCodeRange loc);

[[nodiscard]] Diagnostic invalid_utf8(Diagnostic::TextInfoPtr text_info, sc::src::SourceCodeRange first,
                                      std::size_t others);

[[nodiscard]] Diagnostic unmatched_bracket(Diagnostic::TextInfoPtr text_info, sc::src::SourceCodeRange close);

[[nodiscard]] Diagnostic mismatched_bracket(Diagnostic::TextInfoPtr text_info, sc::src::SourceCodeRange open,
                                            sc::src::SourceCodeRange close);

[[nodiscard]] Diagnostic unclosed_bracket(Diagnostic::TextInfoPtr text_info, sc::src::SourceCodeRange open,
                                          std::optional<sc::src::SourceCodeRange> outer_close);

[[nodiscard]] Diagnostic unclosed_bracket_by_indentation(Diagnostic::TextInfoPtr text_info,
                                                         sc::src::SourceCodeRange open, sc::src::SourceCodeRange end);

[[nodiscard]] Diagnostic region_missing_semicolon(Diagnostic::TextInfoPtr text_info, sc::src::SourceCodeRange loc);

[[nodiscard]] Diagnostic duplicate_class(Diagnostic::TextInfoPtr first_text_info, sc::src::SourceCodeRange first_loc,
                                         Diagnostic::TextInfoPtr second_text_info, sc::src::SourceCodeRange second_loc);

[[nodiscard]] Diagnostic todo(Diagnostic::TextInfoPtr, sc::src::SourceCodeRange loc);

[[nodiscard]] Diagnostic missing_intrinsic_class(std::string);

[[nodiscard]] Diagnostic missing_common_class(std::string);

[[nodiscard]] Diagnostic intrinsic_class_wrong_member_name(Diagnostic::TextInfoPtr, sc::src::SourceCodeRange loc,
                                                           std::string_view expected);

[[nodiscard]] Diagnostic intrinsic_class_wrong_members(Diagnostic::TextInfoPtr, sc::src::SourceCodeRange loc,
                                                       std::string_view expected);

[[nodiscard]] Diagnostic primitive_argument_count(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange arguments,
                                                  sc::src::SourceCodeRange primitive, std::size_t expected,
                                                  std::size_t got);

[[nodiscard]] Diagnostic primitive_needs_variadic_arguments(Diagnostic::TextInfoPtr t,
                                                            sc::src::SourceCodeRange arguments,
                                                            sc::src::SourceCodeRange primitive);

[[nodiscard]] Diagnostic primitive_needs_keyword_arguments(Diagnostic::TextInfoPtr t,
                                                           sc::src::SourceCodeRange arguments,
                                                           sc::src::SourceCodeRange primitive);

[[nodiscard]] Diagnostic duplicate_extention(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                             Diagnostic::TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

[[nodiscard]] Diagnostic extension_conflict(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                            Diagnostic::TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

[[nodiscard]] Diagnostic extension_overwritten(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                               Diagnostic::TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

[[nodiscard]] Diagnostic extension_of_undefined_class(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic
super_class_not_defined(std::string name, std::vector<std::pair<Diagnostic::TextInfoPtr, sc::src::SourceCodeRange>>);

[[nodiscard]] Diagnostic inheritance_cycle(std::vector<std::pair<Diagnostic::TextInfoPtr, sc::src::SourceCodeRange>>);

[[nodiscard]] Diagnostic self_inheritance(Diagnostic::TextInfoPtr, sc::src::SourceCodeRange);

[[nodiscard]] Diagnostic duplicate_method_name(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                               Diagnostic::TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

[[nodiscard]] Diagnostic duplicate_member(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                          Diagnostic::TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

[[nodiscard]] Diagnostic hidden_class_member(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                             Diagnostic::TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

[[nodiscard]] Diagnostic duplicate_member_in_base(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                  Diagnostic::TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

[[nodiscard]] Diagnostic default_inheritor_must_have_super(Diagnostic::TextInfoPtr first_t,
                                                           sc::src::SourceCodeRange first_l);

[[nodiscard]] Diagnostic root_object_must_not_inherit(Diagnostic::TextInfoPtr first_t,
                                                      sc::src::SourceCodeRange first_l);

[[nodiscard]] Diagnostic unexpected_slot_def(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

[[nodiscard]] Diagnostic incorrect_super_class(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                               std::string_view expected);

[[nodiscard]] Diagnostic incorrect_super_class(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

[[nodiscard]] Diagnostic incorrect_slot_def(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                            std::string_view expected);

[[nodiscard]] Diagnostic incorrect_slot_def(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

[[nodiscard]] Diagnostic non_terminating_return(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

[[nodiscard]] Diagnostic message_without_receiver(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

[[nodiscard]] Diagnostic not_constant(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

[[nodiscard]] Diagnostic nested_too_deeply(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t limit);

[[nodiscard]] Diagnostic too_many_variables(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t count,
                                            std::size_t limit);

[[nodiscard]] Diagnostic instance_variable_out_of_reach(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l,
                                                        std::size_t index, std::size_t limit);

[[nodiscard]] Diagnostic class_variable_out_of_reach(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l,
                                                     std::size_t index, std::size_t limit);

[[nodiscard]] Diagnostic local_variable_out_of_reach(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l,
                                                     std::size_t depth, std::size_t limit);

[[nodiscard]] Diagnostic selector_table_full(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t limit);

[[nodiscard]] Diagnostic jump_too_long(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t length,
                                       std::size_t limit);

[[nodiscard]] Diagnostic too_many_arguments(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t count,
                                            std::size_t limit);

[[nodiscard]] Diagnostic hard_coded_loop_signature(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l,
                                                   std::size_t arguments, std::size_t variables);

[[nodiscard]] Diagnostic hard_coded_loop_extension_ignored(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic hard_coded_loop_computed_value(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic literal_out_of_range(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l, bool is_float);

[[nodiscard]] Diagnostic unbounded_range(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

[[nodiscard]] Diagnostic unbounded_range_do(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic range_too_many_arguments(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

[[nodiscard]] Diagnostic range_argument_not_value(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

[[nodiscard]] Diagnostic event_key_without_value(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

[[nodiscard]] Diagnostic variadic_argument_not_last(Diagnostic::TextInfoPtr variadic_t,
                                                    sc::src::SourceCodeRange variadic_l,
                                                    Diagnostic::TextInfoPtr positional_t,
                                                    sc::src::SourceCodeRange positional_l);

[[nodiscard]] Diagnostic duplicate_variable(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                            Diagnostic::TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

[[nodiscard]] Diagnostic undefined_variable(Diagnostic::TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                            std::string_view name);

[[nodiscard]] Diagnostic assigned_special_variable(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic not_compiled(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic unsupported_environment_pack(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic assigned_constant(Diagnostic::TextInfoPtr const_t, sc::src::SourceCodeRange const_l,
                                           Diagnostic::TextInfoPtr assign_t, sc::src::SourceCodeRange assign_l);

[[nodiscard]] Diagnostic constant_setter(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic declared_special_variable(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic curry_outside_call(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic declared_environment_variable(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic missing_interpreter_context(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic missing_interpreter_class(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic declaration_after_expression(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic return_in_closed_function(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l, bool legacy);

[[nodiscard]] Diagnostic outside_closed_function(Diagnostic::TextInfoPtr declaration_t,
                                                 sc::src::SourceCodeRange declaration, Diagnostic::TextInfoPtr use_t,
                                                 sc::src::SourceCodeRange use);

[[nodiscard]] Diagnostic return_in_interpreted_code(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic qualifier_hidden_from_value(Diagnostic::TextInfoPtr declaration_t,
                                                     sc::src::SourceCodeRange declaration,
                                                     Diagnostic::TextInfoPtr use_t, sc::src::SourceCodeRange use);

[[nodiscard]] Diagnostic malformed_version(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic unknown_version(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l,
                                         std::string_view latest);

[[nodiscard]] Diagnostic duplicate_version(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange first,
                                           sc::src::SourceCodeRange second);

[[nodiscard]] Diagnostic misplaced_version(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic latest_version_comment(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l);

[[nodiscard]] Diagnostic conflicting_version(Diagnostic::TextInfoPtr t, sc::src::SourceCodeRange l,
                                             std::string_view written, std::string_view given);

inline std::ostream& operator<<(std::ostream& s, const Diagnostic& d) {
    s << d.to_string();
    s.flush();
    return s;
}

}
