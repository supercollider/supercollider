#pragma once
#include "sc_util/language_version.hpp"

// The rules that differ between versions of the language, by what they decide, so code doesn't compare
// versions.

namespace sc::ir {

// Whether constant expressions are folded.
[[nodiscard]] constexpr bool can_fold_constants(ast::LanguageVersion v) { return v >= ast::LanguageVersion::V3_16; }

// Whether `-0.0` is a different constant to `0.0`.
// Old code compared `-0.0` with its special numbers as a number, so pushed it as `0.0`.
[[nodiscard]] constexpr bool distinguishes_negative_zero(ast::LanguageVersion v) {
    return v >= ast::LanguageVersion::V3_16;
}

// Whether a function's arguments and variables are all visible throughout its leading declarations.
// Legacy code's frame holds them, so they can be used before they are declared.
[[nodiscard]] constexpr bool is_frame_scoped(ast::LanguageVersion v) { return v < ast::LanguageVersion::V3_16; }

// Whether variables must be declared before the first expression of a function.
[[nodiscard]] constexpr bool requires_predeclared_variables(ast::LanguageVersion v) {
    return v < ast::LanguageVersion::V3_16;
}

// Whether variables declared in an inlined function are hoisted into the function it is inlined into.
// If not, a function that declares variables is not inlined.
[[nodiscard]] constexpr bool hoists_inlined_variables(ast::LanguageVersion v) {
    return v >= ast::LanguageVersion::V3_16;
}


// Whether the assign shortcut instruction is emitted.
// Old code never emitted this, there is a bug in the VM so this was probably removed to avoid the bug.
[[nodiscard]] constexpr bool has_assign_shortcut(ast::LanguageVersion v) { return v >= ast::LanguageVersion::V3_16; }

// Whether shortcut methods are made for returning or assigning a class variable.
// Old code only made these for accessors, never for a method returning or assigning a class variable.
[[nodiscard]] constexpr bool has_class_variable_shortcuts(ast::LanguageVersion v) {
    return v >= ast::LanguageVersion::V3_16;
}

// Whether an extension of a hard coded loop method, e.g., `Integer:do`, is replaced by the loop.
// Old code replaced an extension's `Integer:do` and the other hard coded loop methods too, ignoring its body.
[[nodiscard]] constexpr bool hard_codes_extension_loops(ast::LanguageVersion v) {
    return v < ast::LanguageVersion::V3_16;
}

// Whether a warning is given for a class variable or constant that hides the superclass's.
[[nodiscard]] constexpr bool warns_hidden_class_members(ast::LanguageVersion v) {
    return v >= ast::LanguageVersion::V3_16;
}

// Whether `(1..).do(f)` is allowed.
// Old code sent `(1..).do(f)` as one forSeries, so the range was never made.
[[nodiscard]] constexpr bool allows_unbounded_range_do(ast::LanguageVersion v) {
    return v < ast::LanguageVersion::V3_16;
}

// Whether a closed function may contain a non local return.
[[nodiscard]] constexpr bool allows_closed_function_return(ast::LanguageVersion v) {
    return v < ast::LanguageVersion::V3_16;
}

}
