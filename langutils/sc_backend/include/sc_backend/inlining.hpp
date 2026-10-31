#pragma once

#include "sc_bytecode/OpcodeOperands.h"
#include "sc_sema/const_expr_index.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_util/indexed_vector.hpp"
#include "sc_util/strong_index.hpp"

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

using ArgumentIndex = util::StrongIndex<std::uint32_t, struct ArgumentIndex__>;
using Arguments = util::IndexedVector<ArgumentIndex, ir::SCAnyExpr_I>;

[[nodiscard]] Arguments positional_arguments(const ir::IRGraph& g, ir::SCMessage_I m);

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

// Methods whose code is replaced by extended opcodes that loop using the method's frame, e.g., `Integer:do`.
enum struct HardCodedLoop : std::uint8_t {
    IntegerDo,
    IntegerReverseDo,
    IntegerFor,
    IntegerForBy,
    ArrayedCollectionDo,
    ArrayedCollectionReverseDo,
    DictionaryKeysValuesArrayDo,
    NumberForSeries,
    FloatDo,
    FloatReverseDo,
};

struct HardCodedLoopMethod {
    ir::SymSCClassName owner;
    ir::SymSCSelector selector;
    HardCodedLoop loop;
};
static_assert(sizeof(HardCodedLoopMethod) == 12);

struct SpecialSymbols {
    explicit SpecialSymbols(ir::SymbolTable& symbols);

    using SpecialSelector = std::variant<OpSpecialSelectors, OpUnaryMath, OpBinaryMath>;

    std::unordered_map<ir::Symbol, SpecialSelector, std::hash<ir::Symbol>> special_selectors;
    std::unordered_map<ir::Symbol, OpSpecialClassEnum, std::hash<ir::Symbol>> special_classes;
    ir::SymSCSelector series;
    // Methods that never get a shortcut.
    ir::SymSCSelector does_not_understand;
    ir::SymSCSelector function_compile_context;
    std::vector<HardCodedLoopMethod> hard_coded_loops;

    [[nodiscard]] std::optional<SpecialSelector> find(ir::Symbol selector) const;
    // Of an instance method, also in an extension.
    [[nodiscard]] std::optional<HardCodedLoop> find_hard_coded_loop(ir::SymSCClassName owner,
                                                                    ir::SymSCSelector selector) const;
};

}
