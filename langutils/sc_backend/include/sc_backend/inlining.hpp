#pragma once

#include "sc_bytecode/OpcodeOperands.h"
#include "sc_sema/const_expr_index.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbols.hpp"

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

namespace sc::ir {
struct ConstantTable;
struct SymbolTable;
}

namespace sc::backend {

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

enum struct Inlined : std::uint8_t;
struct SpecialSymbols;

struct InliningContext {
    const SpecialSymbols& symbols;
    const ir::ConstantTable& constants;
};

// The old compiler's rule: only functions without arguments or variables are inlined.
// From 3.16 a function with variables that aren't can be inlined to, they are hoisted.
[[nodiscard]] std::optional<Inlined> inlining(const InliningContext& ctx, const ir::IRGraph& g, ir::SCMessage_I m,
                                              ast::LanguageVersion version);

// `{ c }.while` inlines its receiver, `if (c)` evaluates it.
[[nodiscard]] bool inlines_receiver(Inlined kind);

////////////////////////////////////////////////////////////////////////////////
// Inlined sends
////////////////////////////////////////////////////////////////////////////////

enum struct Inlined : std::uint8_t {
    If, // `if (c) { a } { b }`
    IfNil, // `if (x.isNil) { a } { b }`, isNil isn't sent.
    IfNotNil, // `if (x.notNil) { a } { b }`
    And, // `a and: { b }`
    Or, // `a or: { b }`
    OrIfNil, // `a ?? { b }`
    AndIfNotNil, // `a !? { b }`
    QuestionMark, // `a ? b`, inlines nothing, it has its own bytecode.
    While, // `{ c }.while { b }`, `{ c }.while`
    WhileTrue, // `true.while { b }`
    Loop, // `{ b }.loop`
    Case, // `case { c } { a } { e }`, as nested ifs.
    Switch, // `switch (x, 1, { a }, { e })`
};

[[nodiscard]] bool is_inlineable(const ir::IRGraph& g, ir::SCAnyExpr_I i, ast::LanguageVersion version);

[[nodiscard]] std::vector<ir::SCAnyExpr_I> positional_arguments(const ir::IRGraph& g, ir::SCMessage_I m);

////////////////////////////////////////////////////////////////////////////////
// Nil tests
////////////////////////////////////////////////////////////////////////////////

// `x.isNil` or `x.notNil`.
struct NilTest {
    bool is_nil;
    ir::SCMessage_I message;

    [[nodiscard]] Inlined kind() const { return is_nil ? Inlined::IfNil : Inlined::IfNotNil; }
};

[[nodiscard]] std::optional<NilTest> nil_test(const InliningContext& ctx, const ir::IRGraph& g, ir::SCAnyExpr_I i);

// A `case` condition `{ x.isNil }` is tested as an `if` would.
[[nodiscard]] std::optional<NilTest> nil_test_body(const InliningContext& ctx, const ir::IRGraph& g,
                                                   ir::SCFunction_I f);

////////////////////////////////////////////////////////////////////////////////
// Switch keys
////////////////////////////////////////////////////////////////////////////////

// An integer, float, char, symbol or boolean literal, or a function returning one, though `{ 1 } == 1` is false when
// sent. Not nil, which marks an empty slot of the table.
[[nodiscard]] std::optional<ir::ConstExprIndex> switch_key(const InliningContext& ctx, const ir::IRGraph& g,
                                                           ir::SCAnyExpr_I i, ast::LanguageVersion version);

////////////////////////////////////////////////////////////////////////////////
// Special symbols
////////////////////////////////////////////////////////////////////////////////

struct SpecialSymbols {
    explicit SpecialSymbols(ir::SymbolTable& symbols);

    using SpecialSelector = std::variant<OpSpecialSelectors, OpUnaryMath, OpBinaryMath>;

    std::unordered_map<ir::Symbol, SpecialSelector, std::hash<ir::Symbol>> special_selectors;
    std::unordered_map<ir::Symbol, std::uint8_t, std::hash<ir::Symbol>> special_classes;
    ir::SymSCSelector series;

    [[nodiscard]] std::optional<SpecialSelector> find(ir::Symbol selector) const;
};

}
