// Collections. When all the elements are constant, the collection is registered as constant too.
// Needs the ast graph for the children and payloads.
#include "ast2ir/expr.hpp"
#include "ast2ir/literal_synthesis.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"
#include <cassert>
#include <optional>

namespace sc::ir::ast2ir {

namespace {

// Expr's graph and context are private, so its members pass them in.
struct CollectionContext {
    Expr& expr;
    Context& ctx;
    IRGraph& ir;
    const std::shared_ptr<const ast::ASTGraph>& ast_graph;
};

Return<SCAnyExpr_I> symbol(CollectionContext& c, ast::OptionalIndex i, std::string_view name) {
    const auto const_i =
        c.ctx.constant_table.register_constexpr({ c.ctx.symbol_table.get_symbol(std::string { name }) });
    const auto lit = c.ir.create(SCLiteral { const_i }, location_of(c.ir, *c.ast_graph, i));
    c.ir.register_node_as_consteval(lit, const_i);
    return { lit, const_i, TypeInfo { c.ctx.common_classes.symbol, TypeConfidence::Absolute } };
}

// Compiles the elements of an array literal into the collection.
// If all constant, and the collection is known, return a constexpr index
Return<SCAnyExpr_I> array(CollectionContext& c, ast::ArrayIndex i, bool is_immutable);

// Compiles the elements of an array literal into the collection.
// Returns the elements' values if they are all constant.
// `#[a, b]` is immutable, its names are symbols and all its elements must be constant. This applies to the arrays
// inside it too.
std::optional<ConstExprArray> append_elements(CollectionContext& c, SCCollection_I collection, ast::ArrayIndex i,
                                              bool is_immutable) {
    const auto& g = *c.ast_graph;
    ConstExprArray values;
    bool all_constant = true;

    for (auto it = g.children(i).iter(); it; ++it) {
        const auto element = *it;
        const auto r = [&]() -> Return<SCAnyExpr_I> {
            // `[a: 1]` is `[\a, 1]`.
            if (const auto keyword = g.as_a<ast::KeyBinOpIndex>(element))
                return symbol(c, *keyword, synthesize_literal(g.payload(*keyword), get_string_view(g, *keyword)));
            if (const auto nested = g.as_a<ast::ArrayIndex>(element))
                return array(c, *nested, is_immutable);
            if (is_immutable)
                if (const auto name = g.as_a<ast::NamedIdentifierIndex>(element))
                    return symbol(c, *name, get_string_view(g, *name));
            return c.expr(ast::AnyExprIndex { *element });
        }();

        if (is_immutable && !r.const_index)
            c.expr.sink(sc::diag::Diagnostic::not_constant(get_text_info(g), get_location(g, element)));

        c.ir.append(collection, r.ir_node);
        ++c.ir.payload(collection).size;
        if (all_constant && r.const_index)
            values.push_back(c.ctx.constant_table(*r.const_index));
        else
            all_constant = false;
    }

    if (!all_constant)
        return std::nullopt;
    return values;
}

Return<SCAnyExpr_I> array(CollectionContext& c, ast::ArrayIndex i, bool is_immutable) {
    is_immutable = is_immutable || c.ast_graph->payload(i).is_immutable;

    const TypeInfo type { c.ctx.common_classes.array, TypeConfidence::Absolute };
    const auto collection =
        c.ir.create(SCCollection { { std::nullopt, c.ctx.common_classes.array } }, location_of(c.ir, *c.ast_graph, i));

    auto values = append_elements(c, collection, i, is_immutable);
    if (!values)
        return { collection, type };

    const auto const_i = c.ctx.constant_table.register_constexpr({ std::move(*values) });
    c.ir.register_node_as_consteval(collection, const_i);
    return { collection, const_i, type };
}

// The value of `Class[...]` when its elements are all constant, only some classes can be constants.
std::optional<ConstExpression> collection_constant(const Context& ctx, SymSCClassName class_name,
                                                   ConstExprArray values) {
    const auto& classes = ctx.common_classes;
    // `Class[...]` adds each element, so folding assumes what add does.
    if (!ctx.known_methods.is_known(class_name, ctx.common_symbols.add))
        return std::nullopt;

    if (class_name == classes.array)
        return ConstExpression { std::move(values) };

    // `Event[a: 1, b: 2]` is `(a: 1, b: 2)`, the elements are keys and values.
    if (class_name == classes.event) {
        if (values.size() % 2 != 0)
            return std::nullopt;
        ConstExprEvent pairs;
        pairs.reserve(values.size() / 2);
        for (std::size_t k { 0 }; k < values.size(); k += 2)
            pairs.push_back({ std::move(values[k]), std::move(values[k + 1]) });
        return ConstExpression { std::move(pairs) };
    }

    return std::nullopt;
}
}

Return<SCAnyExpr_I> Expr::operator()(ast::ArrayIndex i) {
    CollectionContext c { *this, ctx, ir, ast_graph };
    return array(c, i, false);
}

// `Set[1, 2]` is a collection of that class.
Return<SCAnyExpr_I> Expr::operator()(ast::CollectionIndex i) {
    CollectionContext c { *this, ctx, ir, ast_graph };
    const auto [class_name, elements] = ast_graph->children(i);
    const auto class_sym = ctx.symbol_table.get_class_name(std::string { get_string_view(*ast_graph, class_name) });
    const TypeInfo type { class_sym, TypeConfidence::Absolute };

    // The grammar only allows `Class[...]`, `Class#[...]` can't be written.
    assert(!ast_graph->payload(elements).is_immutable);

    const auto collection = ir.create(SCCollection { { location(class_name), class_sym } }, location(i));
    auto values = append_elements(c, collection, elements, false);

    // The elements of an event are pairs, an odd number means the last key has no value.
    if (class_sym == ctx.common_classes.event && ir.payload(collection).size % 2 != 0)
        sink(sc::diag::Diagnostic::event_key_without_value(get_text_info(*ast_graph),
                                                           get_location(*ast_graph, ast_graph->last_child(elements))));

    if (!values)
        return { collection, type };

    auto constant = collection_constant(ctx, class_sym, std::move(*values));
    if (!constant)
        return { collection, type };

    const auto const_i = ctx.constant_table.register_constexpr(std::move(*constant));
    ir.register_node_as_consteval(collection, const_i);
    return { collection, const_i, type };
}

// `(a: 1, b: 2)`, the children alternate between keys and values.
Return<SCAnyExpr_I> Expr::operator()(ast::DictionaryIndex i) {
    CollectionContext c { *this, ctx, ir, ast_graph };
    const auto& g = *ast_graph;

    const TypeInfo type { ctx.common_classes.event, TypeConfidence::Absolute };
    const auto collection = ir.create(SCCollection { { std::nullopt, ctx.common_classes.event } }, location(i));
    ConstExprEvent values;
    bool all_constant = true;

    for (auto it = g.children(i).iter(); it; ++it) {
        const auto [key_t, value] = g.children(*it);
        const auto key = key_t;
        const auto key_r = [&]() -> Return<SCAnyExpr_I> {
            // `(a: 1)` is `(\a: 1)`.
            if (const auto keyword = g.as_a<ast::KeyBinOpIndex>(key))
                return symbol(c, *keyword, synthesize_literal(g.payload(*keyword), get_string_view(g, *keyword)));
            return (*this)(ast::AnyExprIndex { *key });
        }();
        const auto value_r = (*this)(ast::AnyExprIndex { value });

        ir.append(collection, key_r.ir_node);
        ir.append(collection, value_r.ir_node);
        ir.payload(collection).size += 2;
        if (all_constant && key_r.const_index && value_r.const_index)
            values.push_back({ ctx.constant_table(*key_r.const_index), ctx.constant_table(*value_r.const_index) });
        else
            all_constant = false;
    }

    if (!all_constant)
        return { collection, type };

    const auto const_i = ctx.constant_table.register_constexpr({ std::move(values) });
    ir.register_node_as_consteval(collection, const_i);
    return { collection, const_i, type };
}
}
