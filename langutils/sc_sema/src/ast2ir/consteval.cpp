
#include "consteval.hpp"
#include "ast2ir/literal_synthesis.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_parser/nodes.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_sema/type_info.hpp"
#include <algorithm>
#include <cmath>
#include <string>

namespace sc::ir::ast2ir {
    using Diagnostic = diag::Diagnostic;

    ConstEval::Result ConstEval::eval(ast::AnyExprIndex i) { return std::visit(*this, ast_graph->index_to_variant(i)); }

    ConstEval::Result ConstEval::operator()(ast::ExprSeqIndex i) {
        return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
    }
ConstEval::Result ConstEval::operator()(ast::EnvIdentifierIndex i) {
    return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::NonLocalReturnExprIndex i) {
    return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::ReferenceIndex i) {
    return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::AssignmentIndex i) {
    return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::AssignmentAtIndex i) {
    return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::SetterIndex i) {
    return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::VariablePackAssignIndex i) {
    return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::GeneratorAnyIndex i) {
    return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::ImmediatelyEvaluatedBlockIndex i) {
    return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::RangeGeneratorIndex i) {
    return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::RangeAssignIndex i) {
    return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}

ConstEval::Result ConstEval::operator()(ast::ClassNameIdentifierIndex i) {
    const auto name = std::string { get_string_view(*ast_graph, i) };
    const auto class_index = cxt.symbol_table.get_class_name(name);
    const auto meta_class_index = cxt.symbol_table.get_meta_class_name(name);
    const auto const_expr_i = cxt.constant_table.register_constexpr({ class_index });
    return Success { const_expr_i, TypeInfo { meta_class_index, TypeConfidence::Absolute } };
}

ConstEval::Result ConstEval::operator()(ast::NamedIdentifierIndex i) {
    if (encountered_immutable_array) {
        const auto sym_i = cxt.symbol_table.get_symbol(std::string { get_string_view(*ast_graph, i) });
        const auto const_i = cxt.constant_table.register_constexpr({ sym_i });
        return Success { const_i, TypeInfo { cxt.common_classes.symbol, TypeConfidence::Absolute } };
    }

    if (!class_decl)
        return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };

    const auto name = ast_graph->string(i);
    const auto name_sym = cxt.symbol_table.get_identifier(name);
    if (auto member = class_decl->find_member(name_sym)) {
        // Other members can change, so only constants can be used.
        if (member->is_const && member->expr_index && member->type_info
            && member->type_info->confidence == TypeConfidence::Absolute)
            return Success { member->expr_index->first, *member->type_info };
    }
    return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}

ConstEval::Result ConstEval::operator()(ast::AnyMessageIndex i) {
    return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}

namespace {
// Evaluates e to an integer, or sets failure to the reason it isn't one.
std::optional<std::int32_t> evaluate_integer(ConstEval& eval, ast::AnyExprIndex e, ast::Index whole,
                                             std::optional<Diagnostic>& failure) {
    auto r = eval.eval(e);
    if (const auto s = r.success()) {
        if (const auto v = eval.cxt.constant_table(s->first).get_if<std::int32_t>())
            return *v;
        failure = Diagnostic::not_constant(get_text_info(*eval.ast_graph), get_location(*eval.ast_graph, whole));
        return std::nullopt;
    }
    failure = *std::move(r).diagnostic();
    return std::nullopt;
}
}

ConstEval::Result ConstEval::operator()(ast::AtMessageIndex i) {
    const auto [receiver, arg] = ast_graph->children(i);
    const auto not_constant = [&]() -> Result {
        return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
    };
    const auto range = ast_graph->as_a<ast::RangeIndex>(arg);
    if (!range)
        return not_constant();

    auto receiver_r = eval(ast::AnyExprIndex { receiver });
    const auto receiver_s = receiver_r.success();
    if (!receiver_s)
        return receiver_r;

    auto parts = range_parts(*ast_graph, *range);
    if (parts.error)
        return { *std::move(parts.error) };

    std::optional<Diagnostic> failure;
    std::optional<std::int32_t> first;
    std::optional<std::int32_t> second;
    std::optional<std::int32_t> last;
    if (parts.first && !(first = evaluate_integer(*this, *parts.first, { *i }, failure)))
        return { *std::move(failure) };
    if (parts.second && !(second = evaluate_integer(*this, *parts.second, { *i }, failure)))
        return { *std::move(failure) };
    if (parts.last && !(last = evaluate_integer(*this, *parts.last, { *i }, failure)))
        return { *std::move(failure) };

    const auto& value = cxt.constant_table(receiver_s->first);
    const auto receiver_class = value.get_if<SymSCString>() ? cxt.common_classes.string : cxt.common_classes.array;
    // Folding assumes what copySeries does.
    if (!cxt.known_methods.is_known(receiver_class, cxt.common_symbols.copy_series))
        return not_constant();

    auto folded = fold_copy_series(value, first, second, last, cxt.symbol_table);
    if (!folded)
        return not_constant();
    const auto const_i = cxt.constant_table.register_constexpr(std::move(*folded));
    return Success { const_i, TypeInfo { receiver_class, TypeConfidence::Absolute } };
}

// This is a slice.
std::optional<ConstExpression> ConstEval::fold_copy_series(const ConstExpression& receiver,
                                                           std::optional<std::int32_t> first,
                                                           std::optional<std::int32_t> second,
                                                           std::optional<std::int32_t> last, SymbolTable& symbols) {
    const auto* array = receiver.get_if<ConstExprArray>();
    const auto* string_i = receiver.get_if<SymSCString>();
    if (!array && !string_i)
        return std::nullopt;
    const std::string_view string = string_i ? symbols(*string_i) : std::string_view {};
    const std::int64_t size =
        array ? static_cast<std::int64_t>(array->size()) : static_cast<std::int64_t>(string.size());

    const std::int64_t f = first.value_or(0);
    const std::int64_t l = last ? std::int64_t { *last } : size - 1;
    const std::int64_t step = second ? std::int64_t { *second } - f : (l >= f ? 1 : -1);
    if (step == 0 || (l > f && step < 0) || (l < f && step > 0))
        return std::nullopt;
    // With a step of one, clamping first into the receiver and skipping indices outside of it are the same. Otherwise
    // they aren't, and it isn't clear which is meant.
    const bool first_in_range = f >= 0 && f < size;
    if ((step != 1 && step != -1) && !first_in_range)
        return std::nullopt;

    // Clipped to the receiver.
    const std::int64_t start = step > 0 ? std::max<std::int64_t>(f, 0) : std::min<std::int64_t>(f, size - 1);
    const std::int64_t end = step > 0 ? std::min<std::int64_t>(l, size - 1) : std::max<std::int64_t>(l, 0);

    ConstExprArray elements;
    std::string chars;
    for (std::int64_t k = start; step > 0 ? k <= end : k >= end; k += step) {
        if (array)
            elements.push_back((*array)[static_cast<std::size_t>(k)]);
        else
            chars.push_back(string[static_cast<std::size_t>(k)]);
    }

    if (array)
        return ConstExpression { std::move(elements) };
    return ConstExpression { symbols.get_string(std::move(chars)) };
}

namespace {
ConstEval::RangeParts range_parts_of(const ast::ASTGraph& ast_graph, ast::ArgumentListIndex args,
                                     ast::maybe<ast::AnyExprIndex> maybe_last) {
    ConstEval::RangeParts parts;
    if (!ast_graph.as_a<ast::MissingIndex>(maybe_last))
        parts.last = ast::AnyExprIndex { *maybe_last };

    std::size_t count = 0;
    std::optional<sc::lex::SourceCodeRange> extra;
    for (auto it = ast_graph.children(args).iter(); it; ++it) {
        const ast::ArgumentEntryIndex entry = *it;
        // e.g., `x[a: 1..3]` or `x[*a..3]`, they are skipped so the rest can still be compiled.
        if (ast_graph.as_a<ast::KwArgIndex>(entry) || ast_graph.as_a<ast::VariadicArgIndex>(entry)
            || ast_graph.as_a<ast::BlockListIndex>(entry)) {
            if (!parts.error)
                parts.error =
                    Diagnostic::range_argument_not_value(get_text_info(ast_graph), get_location(ast_graph, entry));
            continue;
        }
        if (count == 0)
            parts.first = ast::AnyExprIndex { *entry };
        else if (count == 1)
            parts.second = ast::AnyExprIndex { *entry };
        else {
            // Underlines from the first extra argument to the last.
            const auto loc = get_location(ast_graph, entry);
            extra = extra ? sc::lex::range(*extra, loc) : loc;
        }
        ++count;
    }
    if (extra && !parts.error)
        parts.error = Diagnostic::range_too_many_arguments(get_text_info(ast_graph), *extra);
    return parts;
}
}

ConstEval::RangeParts ConstEval::range_parts(const ast::ASTGraph& ast_graph, ast::RangeIndex i) {
    const auto [args, maybe_last] = ast_graph.children(i);
    return range_parts_of(ast_graph, args, maybe_last);
}

ConstEval::RangeParts ConstEval::range_parts(const ast::ASTGraph& ast_graph, ast::RangeGeneratorIndex i) {
    const auto [args, maybe_last] = ast_graph.children(i);
    return range_parts_of(ast_graph, args, maybe_last);
}

std::optional<ConstExprArray> ConstEval::fold_series(std::int32_t first, std::optional<std::int32_t> second,
                                                     std::int32_t last) {
    const std::int64_t span = std::int64_t { last } - first;
    const std::int64_t step = second ? std::int64_t { *second } - first : (span >= 0 ? 1 : -1);
    if (step == 0 || (span > 0 && step < 0) || (span < 0 && step > 0))
        return std::nullopt;

    const std::int64_t count = span / step + 1;
    if (count > static_cast<std::int64_t>(max_constant_range_size))
        return std::nullopt;

    ConstExprArray values;
    values.reserve(static_cast<std::size_t>(count));
    for (std::int64_t k { 0 }; k < count; ++k)
        values.push_back(ConstExpression { static_cast<std::int32_t>(first + k * step) });
    return values;
}

// `(first..last)`, `(first, second..last)`, `(..last)` is `(0..last)`.
ConstEval::Result ConstEval::operator()(ast::RangeIndex i) {
    auto parts = range_parts(*ast_graph, i);
    if (parts.error)
        return { *std::move(parts.error) };
    if (!parts.last)
        return { Diagnostic::unbounded_range(get_text_info(*ast_graph), get_location(*ast_graph, i)) };

    const auto not_constant = [&]() -> Result {
        return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
    };

    // Only integer ranges are folded, see fold_series.
    // Writes to failure from the nested lambda.
    std::optional<Diagnostic> failure;
    // If integer failed, read the failure.
    const auto integer = [&](ast::AnyExprIndex e) -> std::optional<std::int32_t> {
        auto r = eval(e);
        if (const auto s = r.success()) {
            if (const auto v = cxt.constant_table(s->first).get_if<std::int32_t>())
                return *v;
            failure = *not_constant().diagnostic();
            return std::nullopt;
        }
        failure = *std::move(r).diagnostic();
        return std::nullopt;
    };

    std::optional<std::int32_t> first = 0;
    std::optional<std::int32_t> second;
    if (parts.first && !(first = integer(*parts.first)))
        return { *std::move(failure) };
    if (parts.second && !(second = integer(*parts.second)))
        return { *std::move(failure) };
    const auto last = integer(*parts.last);
    if (!last)
        return { *std::move(failure) };

    // Folding assumes what series does.
    if (!cxt.known_methods.is_known(cxt.common_classes.int_, cxt.common_symbols.series))
        return not_constant();
    auto values = fold_series(*first, second, *last);
    if (!values)
        return not_constant();

    const auto const_i = cxt.constant_table.register_constexpr({ std::move(*values) });
    return Success { const_i, TypeInfo { cxt.common_classes.array, TypeConfidence::Absolute } };
}


ConstEval::Result ConstEval::operator()(ast::KeyBinOpIndex i) {
    const auto sym_i = cxt.symbol_table.get_symbol(
        std::string { synthesize_literal(ast_graph->payload(i), get_string_view(*ast_graph, i)) });
    const auto const_i = cxt.constant_table.register_constexpr({ sym_i });
    return Success { const_i, TypeInfo { cxt.common_classes.symbol, TypeConfidence::Absolute } };
}

ConstEval::Result ConstEval::operator()(ast::CurryIndex i) {
    return { Diagnostic::not_constant(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}

ConstEval::Result ConstEval::operator()(ast::ArrayIndex i) {
    const auto p = ast_graph->payload(i);
    encountered_immutable_array |= p.is_immutable;
    std::vector<ConstExpression> out;
    for (auto it = ast_graph->children(i).iter(); it; ++it) {
        Result r = std::visit(*this, ast_graph->index_to_variant(*it));
        if (auto s = r.success()) {
            const auto const_i = s->first;
            out.push_back(cxt.constant_table(const_i));
        } else {
            return *std::move(r).diagnostic();
        }
    }

    const auto const_i = cxt.constant_table.register_constexpr({ std::move(out) });
    return Success { const_i, TypeInfo { cxt.common_classes.array, TypeConfidence::Absolute } };
}

ConstEval::Result ConstEval::operator()(ast::CollectionIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}

ConstEval::Result ConstEval::operator()(ast::DictionaryIndex i) {
    std::vector<std::pair<ConstExpression, ConstExpression>> out;
    for (auto c = ast_graph->children(i).iter(); c; ++c) {
        const auto [key, value] = ast_graph->children(*c);

        Result key_r = std::visit(*this, ast_graph->index_to_variant(key));
        if (key_r.success()) {
            Result value_r = std::visit(*this, ast_graph->index_to_variant(value));

            if (value_r.success()) {
                out.push_back(
                    { cxt.constant_table(key_r.success()->first), cxt.constant_table(value_r.success()->first) });
            } else {
                return *std::move(value_r).diagnostic();
            }
        } else {
            return *std::move(key_r).diagnostic();
        }
    }


    const auto const_i = cxt.constant_table.register_constexpr({ std::move(out) });
    return Success { const_i, TypeInfo { cxt.common_classes.event, TypeConfidence::Absolute } };
}

ConstEval::Result ConstEval::operator()(ast::ASCIIIndex i) {
    const char r = synthesize_literal(ast_graph->payload(i), get_string_view(*ast_graph, i));
    const auto const_i = cxt.constant_table.register_constexpr({ r });
    return Success { const_i, TypeInfo { cxt.common_classes.char_, TypeConfidence::Absolute } };
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}

ConstEval::Result ConstEval::operator()(ast::IntLitIndex i) {
    const std::int32_t r = synthesize_literal(ast_graph->payload(i), get_string_view(*ast_graph, i));
    const auto const_i = cxt.constant_table.register_constexpr({ r });
    return Success { const_i, TypeInfo { cxt.common_classes.int_, TypeConfidence::Absolute } };
}

ConstEval::Result ConstEval::operator()(ast::FloatLitIndex i) {
    const double r = synthesize_literal(ast_graph->payload(i), get_string_view(*ast_graph, i));
    const auto const_i = cxt.constant_table.register_constexpr({ r });
    return Success { const_i, TypeInfo { cxt.common_classes.float_, TypeConfidence::Absolute } };
}

ConstEval::Result ConstEval::operator()(ast::PiLitIndex pi_i) {
    const auto [maybe_num] = ast_graph->children(pi_i);

    double d = std::visit(
        //
        util::overload {
            [&](auto i) -> double {
                return M_PI * synthesize_literal(ast_graph->payload(i), get_string_view(*ast_graph, i));
            },
            [&](ast::MissingIndex) -> double { return M_PI; } //
        },
        ast_graph->index_to_variant(maybe_num));
    // `-pi`, when there is a multiplier its own sign is used, e.g., `-2pi`.
    if (ast_graph->payload(pi_i).sign == ast::PiNode::Sign::Negative)
        d = -d;

    const auto const_i = cxt.constant_table.register_constexpr({ d });
    return Success { const_i, TypeInfo { cxt.common_classes.float_, TypeConfidence::Absolute } };
}

ConstEval::Result ConstEval::operator()(ast::AccidentalIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}

ConstEval::Result ConstEval::operator()(ast::StringLitIndex i) {
    const auto string_i = cxt.symbol_table.get_string(synthesize_literal(i, *ast_graph));
    const auto const_i = cxt.constant_table.register_constexpr({ string_i });

    return Success { const_i, TypeInfo { cxt.common_classes.string, TypeConfidence::Absolute } };
}

ConstEval::Result ConstEval::operator()(ast::BooleanLitIndex i) {
    const bool r = synthesize_literal(ast_graph->payload(i), get_string_view(*ast_graph, i));
    const auto const_i = cxt.constant_table.register_constexpr({ r });

    return Success { const_i,
                     TypeInfo { r ? cxt.common_classes.true_ : cxt.common_classes.false_, TypeConfidence::Absolute } };
}

ConstEval::Result ConstEval::operator()(ast::NilLitIndex) {
    const auto const_i = cxt.constant_table.register_constexpr({ Nil {} });
    return Success { const_i, TypeInfo { cxt.common_classes.nil, TypeConfidence::Absolute } };
}

ConstEval::Result ConstEval::operator()(ast::SymbolQuoteIndex i) {
    const auto sym_i =
        cxt.symbol_table.get_symbol(synthesize_literal(ast_graph->payload(i), get_string_view(*ast_graph, i)));
    const auto const_i = cxt.constant_table.register_constexpr({ sym_i });
    return Success { const_i, TypeInfo { cxt.common_classes.symbol, TypeConfidence::Absolute } };
}

ConstEval::Result ConstEval::operator()(ast::SymbolSlashIndex i) {
    const auto sym_i = cxt.symbol_table.get_symbol(
        std::string { synthesize_literal(ast_graph->payload(i), get_string_view(*ast_graph, i)) });
    const auto const_i = cxt.constant_table.register_constexpr({ sym_i });
    return Success { const_i, TypeInfo { cxt.common_classes.symbol, TypeConfidence::Absolute } };
}

ConstEval::Result ConstEval::operator()(ast::BlockIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}


[[nodiscard]] std::optional<ConstEval::Success> ConstEval::Result::success() const {
    if (auto s = std::get_if<Success>(&v)) {
        return { *s };
    } else
        return {};
}
[[nodiscard]] std::optional<Diagnostic> ConstEval::Result::diagnostic() const& {
    if (auto s = std::get_if<Diagnostic>(&v)) {
        return { *s };
    } else
        return {};
}
[[nodiscard]] std::optional<Diagnostic> ConstEval::Result::diagnostic() && {
    if (auto s = std::get_if<Diagnostic>(&v)) {
        return { std::move(*s) };
    } else
        return {};
}
}
