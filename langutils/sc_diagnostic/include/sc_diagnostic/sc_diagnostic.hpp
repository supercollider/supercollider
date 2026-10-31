#pragma once

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

    [[nodiscard]] static Diagnostic unexpectedToken(TextInfoPtr text_info, sc::src::SourceCodeRange loc,
                                                    std::string_view expected, std::string_view found);

    [[nodiscard]] static Diagnostic missing_operator(TextInfoPtr text_info, sc::src::SourceCodeRange before,
                                                     sc::src::SourceCodeRange found, std::string_view found_text,
                                                     std::optional<std::string_view> separator);

    [[nodiscard]] static Diagnostic missing_expression_after(TextInfoPtr text_info, sc::src::SourceCodeRange op,
                                                             sc::src::SourceCodeRange found,
                                                             std::string_view found_text);

    [[nodiscard]] static Diagnostic lexer_error(TextInfoPtr text_info, sc::lex::TokenType,
                                                sc::src::SourceCodeRange loc);

    [[nodiscard]] static Diagnostic invalid_utf8(TextInfoPtr text_info, sc::src::SourceCodeRange first,
                                                 std::size_t others);

    [[nodiscard]] static Diagnostic unmatched_bracket(TextInfoPtr text_info, sc::src::SourceCodeRange close);

    [[nodiscard]] static Diagnostic mismatched_bracket(TextInfoPtr text_info, sc::src::SourceCodeRange open,
                                                       sc::src::SourceCodeRange close);

    [[nodiscard]] static Diagnostic unclosed_bracket(TextInfoPtr text_info, sc::src::SourceCodeRange open,
                                                     std::optional<sc::src::SourceCodeRange> outer_close);

    [[nodiscard]] static Diagnostic
    unclosed_bracket_by_indentation(TextInfoPtr text_info, sc::src::SourceCodeRange open, sc::src::SourceCodeRange end);

    [[nodiscard]] static Diagnostic region_missing_semicolon(TextInfoPtr text_info, sc::src::SourceCodeRange loc);

    [[nodiscard]] static Diagnostic duplicate_class(TextInfoPtr first_text_info, sc::src::SourceCodeRange first_loc,
                                                    TextInfoPtr second_text_info, sc::src::SourceCodeRange second_loc);

    [[nodiscard]] static Diagnostic todo(TextInfoPtr, sc::src::SourceCodeRange loc);

    [[nodiscard]] static Diagnostic missing_intrinsic_class(std::string);

    [[nodiscard]] static Diagnostic missing_common_class(std::string);

    [[nodiscard]] static Diagnostic intrinsic_class_wrong_member_name(TextInfoPtr, sc::src::SourceCodeRange loc,
                                                                      std::string_view expected);

    [[nodiscard]] static Diagnostic intrinsic_class_wrong_members(TextInfoPtr, sc::src::SourceCodeRange loc,
                                                                  std::string_view expected);

    [[nodiscard]] static Diagnostic primitive_argument_count(TextInfoPtr t, sc::src::SourceCodeRange arguments,
                                                             sc::src::SourceCodeRange primitive, std::size_t expected,
                                                             std::size_t got);

    [[nodiscard]] static Diagnostic primitive_needs_variadic_arguments(TextInfoPtr t,
                                                                       sc::src::SourceCodeRange arguments,
                                                                       sc::src::SourceCodeRange primitive);

    [[nodiscard]] static Diagnostic primitive_needs_keyword_arguments(TextInfoPtr t, sc::src::SourceCodeRange arguments,
                                                                      sc::src::SourceCodeRange primitive);

    [[nodiscard]] static Diagnostic duplicate_extention(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                        TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

    [[nodiscard]] static Diagnostic extension_conflict(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                       TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

    [[nodiscard]] static Diagnostic extension_overwritten(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                          TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

    [[nodiscard]] static Diagnostic extension_of_undefined_class(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic
    super_class_not_defined(std::string name, std::vector<std::pair<TextInfoPtr, sc::src::SourceCodeRange>>);

    [[nodiscard]] static Diagnostic inheritance_cycle(std::vector<std::pair<TextInfoPtr, sc::src::SourceCodeRange>>);

    [[nodiscard]] static Diagnostic self_inheritance(TextInfoPtr, sc::src::SourceCodeRange);

    [[nodiscard]] static Diagnostic duplicate_method_name(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                          TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

    [[nodiscard]] static Diagnostic duplicate_member(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                     TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

    [[nodiscard]] static Diagnostic hidden_class_member(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                        TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

    [[nodiscard]] static Diagnostic duplicate_member_in_base(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                             TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

    [[nodiscard]] static Diagnostic default_inheritor_must_have_super(TextInfoPtr first_t,
                                                                      sc::src::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic root_object_must_not_inherit(TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic unexpected_slot_def(TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic incorrect_super_class(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                          std::string_view expected);

    [[nodiscard]] static Diagnostic incorrect_super_class(TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic incorrect_slot_def(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                       std::string_view expected);

    [[nodiscard]] static Diagnostic incorrect_slot_def(TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic non_terminating_return(TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic message_without_receiver(TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic not_constant(TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic nested_too_deeply(TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t limit);

    [[nodiscard]] static Diagnostic too_many_variables(TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t count,
                                                       std::size_t limit);

    [[nodiscard]] static Diagnostic instance_variable_out_of_reach(TextInfoPtr t, sc::src::SourceCodeRange l,
                                                                   std::size_t index, std::size_t limit);

    [[nodiscard]] static Diagnostic class_variable_out_of_reach(TextInfoPtr t, sc::src::SourceCodeRange l,
                                                                std::size_t index, std::size_t limit);

    [[nodiscard]] static Diagnostic selector_table_full(TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t limit);

    [[nodiscard]] static Diagnostic jump_too_long(TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t length,
                                                  std::size_t limit);

    [[nodiscard]] static Diagnostic too_many_arguments(TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t count,
                                                       std::size_t limit);

    [[nodiscard]] static Diagnostic hard_coded_loop_signature(TextInfoPtr t, sc::src::SourceCodeRange l,
                                                              std::size_t arguments, std::size_t variables);

    [[nodiscard]] static Diagnostic hard_coded_loop_extension_ignored(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic hard_coded_loop_computed_value(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic literal_out_of_range(TextInfoPtr t, sc::src::SourceCodeRange l, bool is_float);

    [[nodiscard]] static Diagnostic unbounded_range(TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic unbounded_range_do(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic range_too_many_arguments(TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic range_argument_not_value(TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic event_key_without_value(TextInfoPtr first_t, sc::src::SourceCodeRange first_l);

    [[nodiscard]] static Diagnostic variadic_argument_not_last(TextInfoPtr variadic_t,
                                                               sc::src::SourceCodeRange variadic_l,
                                                               TextInfoPtr positional_t,
                                                               sc::src::SourceCodeRange positional_l);

    [[nodiscard]] static Diagnostic duplicate_variable(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                       TextInfoPtr second_t, sc::src::SourceCodeRange second_l);

    [[nodiscard]] static Diagnostic undefined_variable(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                       std::string_view name);

    [[nodiscard]] static Diagnostic assigned_special_variable(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic not_compiled(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic unsupported_environment_pack(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic assigned_constant(TextInfoPtr const_t, sc::src::SourceCodeRange const_l,
                                                      TextInfoPtr assign_t, sc::src::SourceCodeRange assign_l);

    [[nodiscard]] static Diagnostic constant_setter(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic declared_special_variable(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic curry_outside_call(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic declared_environment_variable(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic missing_interpreter_context(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic missing_interpreter_class(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic declaration_after_expression(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic return_in_closed_function(TextInfoPtr t, sc::src::SourceCodeRange l, bool legacy);

    [[nodiscard]] static Diagnostic outside_closed_function(TextInfoPtr declaration_t,
                                                            sc::src::SourceCodeRange declaration, TextInfoPtr use_t,
                                                            sc::src::SourceCodeRange use);

    [[nodiscard]] static Diagnostic return_in_interpreted_code(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic qualifier_hidden_from_value(TextInfoPtr declaration_t,
                                                                sc::src::SourceCodeRange declaration, TextInfoPtr use_t,
                                                                sc::src::SourceCodeRange use);

    [[nodiscard]] static Diagnostic malformed_version(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic unknown_version(TextInfoPtr t, sc::src::SourceCodeRange l, std::string_view latest);

    [[nodiscard]] static Diagnostic duplicate_version(TextInfoPtr t, sc::src::SourceCodeRange first,
                                                      sc::src::SourceCodeRange second);

    [[nodiscard]] static Diagnostic misplaced_version(TextInfoPtr t, sc::src::SourceCodeRange l);

    [[nodiscard]] static Diagnostic version_in_interpreted_code(TextInfoPtr t, sc::src::SourceCodeRange l);

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
    [[nodiscard]] std::string to_string(const std::filesystem::path& relative_to) const;

private:
    Diagnostic(const char* name, Severity severity, std::vector<Location> locations = {},
               std::string extra = {}) noexcept;
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
