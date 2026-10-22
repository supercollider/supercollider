// Generators, `{: value, qualifiers }` and `{; value, qualifiers }`, see SCGenerator.
// Needs the ast graph for the children, and the symbol table for the declared names.
#include "ast2ir/expr.hpp"
#include "ast2ir/expr/ranges.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"
#include <cstddef>

namespace sc::ir::ast2ir {

namespace {

// Scopes opened in expr, which are all closed when this is destroyed.
class OpenScopes {
public:
    explicit OpenScopes(Expr& e): expr(e) {}
    OpenScopes(const OpenScopes&) = delete;
    OpenScopes& operator=(const OpenScopes&) = delete;
    ~OpenScopes() {
        for (std::size_t i { 0 }; i < count; ++i)
            expr.scope.pop_back();
    }

    void open() {
        expr.scope.emplace_back();
        ++count;
    }

private:
    Expr& expr;
    std::size_t count { 0 };
};

struct GeneratorContext {
    Expr& expr;
    IRGraph& ir;
    Context& ctx;
    const std::shared_ptr<const ast::ASTGraph>& ast_graph;
    // Owns the variables the qualifiers declare.
    SCGenQualList_I qualifiers;
    // The scopes opened by the qualifiers, they are open until the context is destroyed.
    OpenScopes scopes;
};

Locatable<SymSCIdentifier> name_of(GeneratorContext& c, ast::NamedIdentifierIndex name) {
    return { location_of(c.ir, *c.ast_graph, { *name }),
             c.ctx.symbol_table.get_identifier(std::string { get_string_view(*c.ast_graph, name) }) };
}

// Each declaring qualifier gets its own scope, as each was a block in the old compiler, so a later qualifier can hide
// an earlier one's names. The variables change each iteration, so are never constant.
void open_scope(GeneratorContext& c) { c.scopes.open(); }

void declare(GeneratorContext& c, Locatable<SymSCIdentifier> name,
             join<SCGenQualIterate_I, SCGenQualDeclare_I> declare) {
    if (c.ctx.common_symbols.is_special(*name)) {
        c.expr.sink(sc::diag::Diagnostic::declared_special_variable(c.ir.text_info(name.location.source),
                                                                    IRGraph::range(name.location)));
        return;
    }
    c.expr.add_symbol(
        *name,
        ScopeEntry { nullptr, c.qualifiers, declare, {}, {}, ScopeEntry::Kind::Local, ScopeEntry::State::Initalised });
}

// The collection being iterated. It is only iterated, so a range can be unbounded, e.g., `x <- (1..)`.
SCAnyExpr_I compile_source(GeneratorContext& c, ast::AnyExprIndex source) {
    if (const auto range = c.ast_graph->as_a<ast::RangeIndex>(source))
        return compile_range(c.expr, c.ir, c.ast_graph, c.ctx, *range, true).ir_node;
    return c.expr(source).ir_node;
}

// `x <- source` or `x i <- source`. The source can't use the names it declares.
void iterate(GeneratorContext& c, ast::GeneratorQualAssignIndex q) {
    const auto& g = *c.ast_graph;
    const auto [element_i, maybe_index_i, source] = g.children(q);
    const auto source_i = compile_source(c, source);

    const auto element = name_of(c, element_i);
    std::optional<Locatable<SymSCIdentifier>> index;
    const auto index_i = g.as_a<ast::NamedIdentifierIndex>(maybe_index_i);
    if (index_i)
        index = name_of(c, *index_i);

    const auto node = c.ir.create(SCGenQualIterate { element, index }, location_of(c.ir, g, { *q }), source_i);
    c.ir.append(c.qualifiers, node);

    open_scope(c);
    declare(c, element, node);
    if (!index)
        return;
    // `x x <- source`, the old compiler made these the arguments of a block, which can't share a name.
    if (**index == *element) {
        c.expr.sink(sc::diag::Diagnostic::duplicate_variable(get_text_info(g), get_location(g, element_i),
                                                             get_text_info(g), get_location(g, *index_i)));
        return;
    }
    declare(c, *index, node);
}

// `var x = value`. The value can't use the name it declares.
void declare_variable(GeneratorContext& c, ast::GeneratorQualDeclareIndex q) {
    const auto [name_i, value] = c.ast_graph->children(q);
    const auto value_r = c.expr(value);
    const auto name = name_of(c, name_i);
    const auto node = c.ir.create(SCGenQualDeclare { name }, location_of(c.ir, *c.ast_graph, { *q }), value_r.ir_node);
    c.ir.append(c.qualifiers, node);
    open_scope(c);
    declare(c, name, node);
}

void qualifier(GeneratorContext& c, ast::GeneratorQualAnyIndex q) {
    const auto& g = *c.ast_graph;
    const auto here = location_of(c.ir, g, { *q });
    if (const auto assign = g.as_a<ast::GeneratorQualAssignIndex>(q)) {
        iterate(c, *assign);
    } else if (const auto declaration = g.as_a<ast::GeneratorQualDeclareIndex>(q)) {
        declare_variable(c, *declaration);
    } else if (const auto condition = g.as_a<ast::GeneratorQualIfIndex>(q)) {
        const auto [expr] = g.children(*condition);
        c.ir.append(c.qualifiers, c.ir.create(SCGenQualIf {}, here, c.expr(expr).ir_node));
    } else if (const auto effect = g.as_a<ast::GeneratorQualSideEffectIndex>(q)) {
        const auto [expr] = g.children(*effect);
        c.ir.append(c.qualifiers, c.ir.create(SCGenQualSideEffect {}, here, c.expr(expr).ir_node));
    } else if (const auto guard = g.as_a<ast::GeneratorQualGuardIndex>(q)) {
        const auto [expr] = g.children(*guard);
        c.ir.append(c.qualifiers, c.ir.create(SCGenQualWhile {}, here, c.expr(expr).ir_node));
    } else {
        assert(false && "unhandled generator qualifier");
    }
}

// A generator is never constant, a Routine is made each time, and an Immediate one runs code.
Return<SCAnyExpr_I> generator(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph,
                              Context& ctx, ast::Index whole, ast::AnyExprIndex value, ast::GeneratorQualListIndex list,
                              SCGenerator::Kind kind) {
    const auto qualifiers = ir.create(SCGenQualList {}, location_of(ir, *ast_graph, { *list }));
    // The value is compiled while the qualifiers' scopes are open, c closes them.
    const auto value_r = [&] {
        GeneratorContext c { expr, ir, ctx, ast_graph, qualifiers, OpenScopes { expr } };
        for (auto it = ast_graph->children(list).iter(); it; ++it)
            qualifier(c, *it);
        return expr(value);
    }();

    return { ir.create(SCGenerator { kind }, location_of(ir, *ast_graph, whole), qualifiers, value_r.ir_node) };
}

}

Return<SCAnyExpr_I> Expr::operator()(ast::GeneratorIndex i) {
    const auto [value, list] = ast_graph->children(i);
    return generator(*this, ir, ast_graph, ctx, { *i }, value, list, SCGenerator::Kind::Routine);
}

Return<SCAnyExpr_I> Expr::operator()(ast::GeneratorFakeIndex i) {
    const auto [value, list] = ast_graph->children(i);
    return generator(*this, ir, ast_graph, ctx, { *i }, value, list, SCGenerator::Kind::Immediate);
}

}
