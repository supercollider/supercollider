#include "ast2ir/body.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/symbol_table.hpp"
#include <algorithm>
#include <vector>

namespace sc::ir::ast2ir {

namespace {

// Used to deduce whether we can perform a read at compile time.
struct VariableUses {
    std::vector<SymSCIdentifier> assigned;
    std::vector<SymSCIdentifier> captured;

    [[nodiscard]] static bool contains(const std::vector<SymSCIdentifier>& names, SymSCIdentifier name) {
        return std::find(names.begin(), names.end(), name) != names.end();
    }
    [[nodiscard]] bool can_be_constant(SymSCIdentifier name) const {
        return !contains(assigned, name) && !contains(captured, name);
    }
};

class VariableScanner {
public:
    VariableScanner(const ast::ASTGraph& g, SymbolTable& symbols, VariableUses& uses):
        g(g),
        symbols(symbols),
        uses(uses) {}

    void scan(ast::OptionalIndex node) {
        if (const auto assignment = g.as_a<ast::AssignmentIndex>(node);
            assignment && g.payload(*assignment).target == ast::AssignmentNode::Target::Normal) {
            const auto [name, value] = g.children(*assignment);
            if (const auto sym = symbol(name); !hidden(sym))
                uses.assigned.push_back(sym);
            scan(value);
            return;
        }
        if (const auto pack = g.as_a<ast::VariablePackAssignIndex>(node)) {
            const auto [names, value] = g.children(*pack);
            record_names(names, uses.assigned);
            scan(value);
            return;
        }
        if (const auto block = g.as_a<ast::BlockIndex>(node)) {
            const auto [args, contents] = g.children(*block);
            scan_function(args, contents);
            return;
        }
        if (const auto block = g.as_a<ast::ImmediatelyEvaluatedBlockIndex>(node)) {
            const auto [args, contents] = g.children(*block);
            scan_function(args, contents);
            return;
        }
        if (g.as_a<ast::GeneratorIndex>(node) || g.as_a<ast::GeneratorFakeIndex>(node)) {
            // Generators are functions too, their declarations aren't tracked, which is cautious.
            hidden_names.emplace_back();
            scan_children(node);
            hidden_names.pop_back();
            return;
        }
        if (const auto declarations = g.as_a<ast::DeclareVariableListIndex>(node); declarations && in_function()) {
            // A function's variables hide the outer ones, but only after they are declared.
            for (auto it = g.children(*declarations).iter(); it; ++it) {
                const ast::DeclareAnyVariableIndex item = *it;
                if (const auto name = g.as_a<ast::NamedIdentifierIndex>(item)) {
                    hide(*name);
                } else if (const auto with_default = g.as_a<ast::DeclareVariableWithDefaultIndex>(item)) {
                    const auto [name, value] = g.children(*with_default);
                    scan(value);
                    hide(name);
                } else {
                    scan_children(item);
                }
            }
            return;
        }
        if (const auto name = g.as_a<ast::NamedIdentifierIndex>(node); name && in_function())
            if (const auto sym = symbol(*name); !hidden(sym))
                uses.captured.push_back(sym);
        scan_children(node);
    }

private:
    [[nodiscard]] bool in_function() const { return !hidden_names.empty(); }

    [[nodiscard]] SymSCIdentifier symbol(ast::NamedIdentifierIndex name) {
        return symbols.get_identifier(std::string { get_string_view(g, name) });
    }

    [[nodiscard]] bool hidden(SymSCIdentifier name) const {
        for (const auto& names : hidden_names)
            if (VariableUses::contains(names, name))
                return true;
        return false;
    }

    void hide(ast::NamedIdentifierIndex name) { hidden_names.back().push_back(symbol(name)); }

    void scan_children(ast::OptionalIndex node) {
        for (auto c = g.edges(node).first_child; c; c = g.edges(c).next_sibling)
            scan(c);
    }

    // The names in a pack, `#a, b`, that aren't hidden.
    void record_names(ast::OptionalIndex node, std::vector<SymSCIdentifier>& into) {
        if (const auto name = g.as_a<ast::NamedIdentifierIndex>(node)) {
            if (const auto sym = symbol(*name); !hidden(sym))
                into.push_back(sym);
            return;
        }
        for (auto c = g.edges(node).first_child; c; c = g.edges(c).next_sibling)
            record_names(c, into);
    }

    // An inner function, its arguments hide outer names.
    void scan_function(ast::DeclareArgumentListIndex args, ast::OptionalIndex contents) {
        hidden_names.emplace_back();
        for (auto it = g.children(args).iter(); it; ++it) {
            const auto arg = *it;
            if (const auto name = g.as_a<ast::NamedIdentifierIndex>(arg)) {
                hide(*name);
            } else if (const auto with_default = g.as_a<ast::DeclareArgumentWithDefaultIndex>(arg)) {
                const auto [name, value] = g.children(*with_default);
                scan(value);
                hide(name);
            } else if (const auto variadic = g.as_a<ast::DeclareArgumentVariadicIndex>(arg)) {
                const auto [name] = g.children(*variadic);
                hide(name);
            }
        }
        scan(contents);
        hidden_names.pop_back();
    }

    const ast::ASTGraph& g;
    SymbolTable& symbols;
    VariableUses& uses;
    // One list per inner function, the outermost body has none.
    std::vector<std::vector<SymSCIdentifier>> hidden_names;
};

struct BodyContext {
    Expr& expr;
    IRGraph& ir;
    Context& ctx;
    const std::shared_ptr<const ast::ASTGraph>& ast_graph;
    join<SCFunction_I, SCMethod_I> owner;
    SCBody_I body;
    VariableUses uses;
};

// `var a = 1`, or `var a` when value is missing.
// Returns whether the initial value is constant, so evaluating the declaration has no effects.
bool declare_variable(BodyContext& b, ast::NamedIdentifierIndex name_i, std::optional<ast::AnyExprIndex> value,
                      ast::OptionalIndex whole) {
    const auto& g = *b.ast_graph;
    // Compiled before the variable is declared, so it can't refer to itself.
    std::optional<Return<SCAnyExpr_I>> value_r;
    if (value)
        value_r = b.expr(*value);
    const maybe<SCAnyExpr_I> value_i = value_r
        ? maybe<SCAnyExpr_I> { value_r->ir_node }
        : maybe<SCAnyExpr_I> { b.ir.create(SCMissing {}, location_of(b.ir, g, whole)) };

    const auto name = b.ctx.symbol_table.get_identifier(std::string { get_string_view(g, name_i) });
    const auto declare =
        b.ir.create(SCVariableDeclare { { location_of(b.ir, g, name_i), name } }, location_of(b.ir, g, whole), value_i);
    b.ir.append(b.body, declare);

    // Arguments and variables of the same function share a scope, so a name can't be declared twice.
    auto& current = b.expr.scope.back();
    if (const auto fnd = current.find(name); fnd != current.end()) {
        const auto previous = b.ir.location(fnd->second.declare_index);
        b.expr.sink(sc::diag::Diagnostic::duplicate_variable(b.ir.text_info(previous.source), IRGraph::range(previous),
                                                             get_text_info(g), get_location(g, name_i)));
        return false;
    }
    // Constant if its value is (`var a` is nil) and it is never assigned or used by an inner function.
    // Mutable values, e.g., arrays, are never constant, as any method could change them, through this variable or any
    // other reference to the object.
    std::optional<ConstExprIndex> constant;
    std::optional<TypeInfo> type;
    if (b.uses.can_be_constant(name)) {
        if (!value_r) {
            constant = b.ctx.constant_table.register_constexpr({ Nil {} });
            type = TypeInfo { b.ctx.common_classes.nil, TypeConfidence::Absolute };
        } else if (value_r->const_index && !b.ctx.constant_table(*value_r->const_index).is_mutable()) {
            constant = value_r->const_index;
            type = value_r->type;
        }
    }
    b.expr.add_symbol(name,
                      ScopeEntry { nullptr, b.owner, declare, constant, type, ScopeEntry::Kind::Local,
                                   ScopeEntry::State::Initalised });
    return !value_r || value_r->const_index;
}

// Returns whether every initial value is constant.
bool declare_variables(BodyContext& b, ast::DeclareVariableListIndex list) {
    const auto& g = *b.ast_graph;
    bool all_constant = true;
    for (auto it = g.children(list).iter(); it; ++it) {
        const ast::DeclareAnyVariableIndex item = *it;
        if (const auto name = g.as_a<ast::NamedIdentifierIndex>(item)) {
            all_constant = declare_variable(b, *name, std::nullopt, *name) && all_constant;
        } else if (const auto with_default = g.as_a<ast::DeclareVariableWithDefaultIndex>(item)) {
            const auto [name_i, value] = g.children(*with_default);
            all_constant = declare_variable(b, name_i, value, *with_default) && all_constant;
        } else {
            // `var #a, b = c`, not implemented yet.
            b.expr.sink(sc::diag::Diagnostic::todo(get_text_info(g), get_location(g, item)));
            all_constant = false;
        }
    }
    return all_constant;
}
}

BodyResult compile_body(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph, Context& ctx,
                        join<SCFunction_I, SCMethod_I> owner, SCBody_I body, ast::BlockContentsListIndex contents) {
    VariableUses uses;
    VariableScanner { *ast_graph, ctx.symbol_table, uses }.scan(contents);
    BodyContext b { expr, ir, ctx, ast_graph, owner, body, std::move(uses) };

    // Only the last item's value matters, the others only need to be constant.
    bool all_constant = true;
    std::optional<ConstExprIndex> last;
    bool explicit_return = false;
    for (auto it = ast_graph->children(contents).iter(); it; ++it) {
        const ast::BlockItemIndex item = *it;
        explicit_return = false;
        if (const auto declarations = ast_graph->as_a<ast::DeclareVariableListIndex>(item)) {
            all_constant = declare_variables(b, *declarations) && all_constant;
            last = std::nullopt;
            continue;
        }
        const auto r = expr(ast::AnyExprIndex { *item });
        ir.append(body, r.ir_node);
        last = r.const_index;
        // A return isn't a value, but what it returns can be constant.
        if (const auto ret = ir.as_a<SCExplicitReturn_I>(r.ir_node)) {
            const auto [value] = ir.children(*ret);
            last = ir.node_as_constexpr(value);
            explicit_return = true;
        }
        all_constant = all_constant && last;
    }

    if (!all_constant)
        return {};
    if (last)
        return { last, explicit_return };
    // Empty, a body ending in a declaration isn't a value.
    if (ast_graph->children(contents).iter())
        return {};
    return { ctx.constant_table.register_constexpr({ Nil {} }), false };
}
}
