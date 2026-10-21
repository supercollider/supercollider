
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
#include <cmath>

namespace sc::ir::ast2ir {
    using Diagnostic = diag::Diagnostic;

ConstEval::Result ConstEval::operator()(ast::ExprSeqIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::EnvIdentifierIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::NonLocalReturnExprIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::ReferenceIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::AssignmentIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::AssignmentAtIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::SetterIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::VariablePackAssignIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::GeneratorAnyIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::ImmediatelyEvaluatedBlockIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::RangeGeneratorIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}
ConstEval::Result ConstEval::operator()(ast::RangeAssignIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
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
        return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };

    const auto name = ast_graph->string(i);
    const auto name_sym = cxt.symbol_table.get_identifier(name);
    if (auto member = class_decl->find_member(name_sym)) {
        if (member->expr_index && member->type_info && member->type_info->confidence == TypeConfidence::Absolute)
            return Success { member->expr_index->first, *member->type_info };
    }
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}

ConstEval::Result ConstEval::operator()(ast::AnyMessageIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
}

ConstEval::Result ConstEval::operator()(ast::RangeIndex i) {
    // const auto [args_i_t, last_i_t] = ast_graph->children(i);
    // const ast::ArgumentListIndex args_i { args_i_t };
    // const ast::AnyExprIndex last_i { last_i_t };

    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };

    // const auto r = ast_graph->list_unpack<2>(args_i);
    // if (!r)
    //     return { Diagnostic::todo(text_info(*ast_graph), get_location(*ast_graph, i)) };

    // const ast::ArgumentEntryIndex first = (*r)[0];
    // const ast::ArgumentEntryIndex second = (*r)[1];

    // const auto visit_args = util::overload {
    //     //
    //     [&](ast::KwArgIndex i) -> ConstEval::Result {
    //     },
    //     [&](ast::VariadicArgIndex i) -> ConstEval::Result {
    //         return { Diagnostic::todo(text_info(*ast_graph), location(*ast_graph, i)) };
    //     },
    //     [&](ast::BlockListIndex i) -> ConstEval::Result {
    //         return { Diagnostic::todo(text_info(*ast_graph), location(*ast_graph, i)) };
    //     },
    //     [&](ast::AnyExprIndex i) -> ConstEval::Result { return (*this)(i); } //
    // };

    // const auto first_r = ast_graph->visit(visit_args, first);
    // auto first_success = first_r.success();
    // if (!first_success) {
    //     return *std::move(first_r).diagnostic();
    // }
    // const auto first_i = context.constant_table(first_success->first).get_if<int>();
    // if (!first_i)
    //     return { Diagnostic::todo(text_info(*ast_graph), ast_graph->location(first)) };


    // const auto second_r = ast_graph->visit(visit_args, second);
    // auto second_success = second_r.success();
    // if (!second_success) {
    //     return *std::move(second_r).diagnostic();
    // }
    // const auto second_i = context.constant_table(second_success->first).get_if<int>();
    // if (!second_i)
    //     return { Diagnostic::todo(text_info(*ast_graph), ast_graph->location(second)) };

    // if (*second_i >= *first_i) {
    //     return { Diagnostic::todo(text_info(*ast_graph), ast_graph->location(second)) };
    // }

    // return (*this)(last_i).visit(
    //     [&](Success s) ->ConstEval::Result {
    //         const auto last_i = context.constant_table(s.first).get_if<int>();
    //         if (!last_i) {
    //             return { Diagnostic::todo(text_info(*ast_graph), ast_graph->location(second)) };
    //         }

    //         if ()


    //     },
    //     [&](Diagnostic&& d) -> ConstEval::Result {
    //         return {std::move(d)};
    //     });
}


ConstEval::Result ConstEval::operator()(ast::KeyBinOpIndex i) {
    const auto sym_i = cxt.symbol_table.get_symbol(
        std::string { synthesize_literal(ast_graph->payload(i), get_string_view(*ast_graph, i)) });
    const auto const_i = cxt.constant_table.register_constexpr({ sym_i });
    return Success { const_i, TypeInfo { cxt.common_classes.symbol, TypeConfidence::Absolute } };
}

ConstEval::Result ConstEval::operator()(ast::CurryIndex i) {
    return { Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, i)) };
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
