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
    const auto lit = c.ir.create(SCLiteral { const_i }, { c.ast_graph, { *i } });
    c.ir.register_node_as_consteval(lit, const_i);
    return { lit, const_i, TypeInfo { c.ctx.common_classes.symbol, TypeConfidence::Absolute } };
}

// `#[a, b]` is immutable, its names are symbols and all its elements must be constant. This applies to the arrays
// inside it too.
Return<SCAnyExpr_I> array(CollectionContext& c, ast::ArrayIndex i, bool is_immutable) {
    const auto& g = *c.ast_graph;
    is_immutable = is_immutable || g.payload(i).is_immutable;

    const TypeInfo type { c.ctx.common_classes.array, TypeConfidence::Absolute };
    const auto collection =
        c.ir.create(SCCollection { { std::nullopt, c.ctx.common_classes.array } }, { c.ast_graph, { *i } });
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
        return { collection, type };

    const auto const_i = c.ctx.constant_table.register_constexpr({ std::move(values) });
    c.ir.register_node_as_consteval(collection, const_i);
    return { collection, const_i, type };
}

}

Return<SCAnyExpr_I> Expr::operator()(ast::ArrayIndex i) {
    CollectionContext c { *this, ctx, ir, ast_graph };
    return array(c, i, false);
}

// `(a: 1, b: 2)`, the children alternate between keys and values.
Return<SCAnyExpr_I> Expr::operator()(ast::DictionaryIndex i) {
    CollectionContext c { *this, ctx, ir, ast_graph };
    const auto& g = *ast_graph;

    const TypeInfo type { ctx.common_classes.event, TypeConfidence::Absolute };
    const auto collection =
        ir.create(SCCollection { { std::nullopt, ctx.common_classes.event } }, { ast_graph, { *i } });
    ConstExprEvent values;
    bool all_constant = true;

    for (auto it = g.children(i).iter(); it; ++it) {
        const auto [key, value] = g.children(*it);
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
