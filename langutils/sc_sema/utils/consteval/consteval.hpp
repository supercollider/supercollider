// Copyright Jordan Henderson 2026
#pragma once

#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/details/class_library_sources.hpp"
#include "sc_sema/module.hpp"
#include "sc_sema/symbol_table.hpp"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace sc::const_eval {

/// @brief Interpreted code evaluated as a constant, e.g., a configuration file written as an event.
///
/// The code is compiled after a small class library, the classes the compiler needs, as in the regression tests'
/// prelude, and as 3.16 unless its version comment says otherwise. It owns the tables the constant's parts are in.
struct Evaluated {
    /// The constant, or nothing if the code isn't one; the diagnostics may say why.
    [[nodiscard]] const ir::ConstExpression* value() const;
    /// An element of an array or event.
    [[nodiscard]] const ir::ConstExpression& operator()(ir::ConstExprIndex i) const;
    [[nodiscard]] const ir::SymbolTable& symbols() const { return m_symbols; }
    [[nodiscard]] const ir::ConstantTable& constants() const;
    [[nodiscard]] const std::vector<diag::Diagnostic>& diagnostics() const { return m_diagnostics; }

private:
    friend std::unique_ptr<const Evaluated> evaluate(std::string, std::optional<std::filesystem::path>);
    Evaluated() = default;

    // The context refers to the tables and sources, so it is last, and the struct doesn't move.
    ir::SymbolTable m_symbols {};
    ir::ModuleTable m_modules {};
    ir::ClassLibrarySources m_sources {};
    std::vector<diag::Diagnostic> m_diagnostics {};
    ir::OptionalConstExprIndex m_value {};
    ir::ContextPtr m_context {};
};

/// @brief Evaluates interpreted code as a constant, see Evaluated.
/// @param path Where the code is from, for the diagnostics.
[[nodiscard]] std::unique_ptr<const Evaluated> evaluate(std::string source,
                                                        std::optional<std::filesystem::path> path = std::nullopt);

}
