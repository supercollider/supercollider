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

[[nodiscard]] constexpr bool allows_closed_function_return(ast::LanguageVersion v) {
    return v < ast::LanguageVersion::V3_16;
}

}
