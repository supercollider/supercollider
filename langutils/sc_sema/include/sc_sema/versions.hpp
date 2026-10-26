#pragma once
#include "sc_parser/sc_grammar_shared.hpp"

// The rules that differ between versions of the language, by what they decide, so code doesn't compare versions.
namespace sc::ir {

// Old legacy behaviour.
[[nodiscard]] constexpr bool can_fold_constants(ast::LanguageVersion v) { return v >= ast::LanguageVersion::V3_16; }

[[nodiscard]] constexpr bool is_frame_scoped(ast::LanguageVersion v) { return v < ast::LanguageVersion::V3_16; }

[[nodiscard]] constexpr bool requires_predeclared_variables(ast::LanguageVersion v) {
    return v < ast::LanguageVersion::V3_16;
}

[[nodiscard]] constexpr bool hoists_inlined_variables(ast::LanguageVersion v) {
    return v >= ast::LanguageVersion::V3_16;
}

[[nodiscard]] constexpr bool rewrites_perform(ast::LanguageVersion v) { return v >= ast::LanguageVersion::V3_16; }

// Old code never emitted this, there is a bug in the VM so this was probably removed to avoid the bug.
[[nodiscard]] constexpr bool has_assign_shortcut(ast::LanguageVersion v) { return v >= ast::LanguageVersion::V3_16; }

// Old code only made these for accessors, never for a method returning or assigning a class variable.
[[nodiscard]] constexpr bool has_class_variable_shortcuts(ast::LanguageVersion v) {
    return v >= ast::LanguageVersion::V3_16;
}

// Old code replaced an extension's `Integer:do` and the other hard coded loop methods too, ignoring its body.
[[nodiscard]] constexpr bool hard_codes_extension_loops(ast::LanguageVersion v) {
    return v < ast::LanguageVersion::V3_16;
}

[[nodiscard]] constexpr bool warns_hidden_class_members(ast::LanguageVersion v) {
    return v >= ast::LanguageVersion::V3_16;
}

[[nodiscard]] constexpr bool allows_closed_function_return(ast::LanguageVersion v) {
    return v < ast::LanguageVersion::V3_16;
}

}
