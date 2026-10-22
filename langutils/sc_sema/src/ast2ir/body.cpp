#include "ast2ir/body.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_util/overload.hpp"
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

    void scan(ast::Index node) {
        if (const auto assignment = g.as_a<ast::AssignmentIndex>(node);
            assignment && g.payload(*assignment).target == ast::AssignmentNode::Target::Normal) {
            const auto [name, value] = g.children(*assignment);
            if (const auto sym = symbol(name); !hidden(sym))
                uses.assigned.push_back(sym);
            scan({ *value });
            return;
        }
        if (const auto pack = g.as_a<ast::VariablePackAssignIndex>(node)) {
            const auto [names, value] = g.children(*pack);
            record_names({ *names }, uses.assigned);
            scan({ *value });
            return;
        }
        if (const auto block = g.as_a<ast::BlockIndex>(node)) {
            const auto [args, contents] = g.children(*block);
            scan_function(args, { *contents });
            return;
        }
        if (const auto block = g.as_a<ast::ImmediatelyEvaluatedBlockIndex>(node)) {
            const auto [args, contents] = g.children(*block);
            scan_function(args, { *contents });
            return;
        }
        if (g.as_a<ast::GeneratorIndex>(node) || g.as_a<ast::GeneratorFakeIndex>(node)) {
            // Generators are functions too, their declarations aren't tracked, which is cautious.
            hidden_names.emplace_back();
            scan_children(node);
            hidden_names.pop_back();
            return;
        }
        if (const auto declarations = g.as_a<ast::DeclareVariableListIndex>(node)) {
            // Declaring isn't assigning. A function's variables hide the outer ones, but only after they are declared.
            for (auto it = g.children(*declarations).iter(); it; ++it) {
                const ast::DeclareAnyVariableIndex item = *it;
                if (const auto name = g.as_a<ast::NamedIdentifierIndex>(item)) {
                    hide(*name);
                } else if (const auto with_default = g.as_a<ast::DeclareVariableWithDefaultIndex>(item)) {
                    const auto [name, value] = g.children(*with_default);
                    scan({ *value });
                    hide(name);
                } else if (const auto pack = g.as_a<ast::VariablePackAssignIndex>(item)) {
                    const auto [names, value] = g.children(*pack);
                    scan({ *value });
                    hide_names({ *names });
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

    // Only a function's variables hide names, the outermost body's are what is being scanned for.
    void hide(ast::NamedIdentifierIndex name) {
        if (in_function())
            hidden_names.back().push_back(symbol(name));
    }

    // The names in a pack, `#a, b`. Environment variables, `#~a`, aren't variables, so are skipped.
    void hide_names(ast::Index node) {
        if (g.as_a<ast::EnvIdentifierIndex>(node))
            return;
        if (const auto name = g.as_a<ast::NamedIdentifierIndex>(node)) {
            hide(*name);
            return;
        }
        for (auto c = g.edges(node).first_child; c; c = g.edges(c).next_sibling)
            hide_names({ *c });
    }

    void scan_children(ast::Index node) {
        for (auto c = g.edges(node).first_child; c; c = g.edges(c).next_sibling)
            scan({ *c });
    }

    // The names in a pack, `#a, b`, that aren't hidden. Environment variables, `#~a`, aren't variables, so are skipped.
    void record_names(ast::Index node, std::vector<SymSCIdentifier>& into) {
        if (g.as_a<ast::EnvIdentifierIndex>(node))
            return;
        if (const auto name = g.as_a<ast::NamedIdentifierIndex>(node)) {
            if (const auto sym = symbol(*name); !hidden(sym))
                into.push_back(sym);
            return;
        }
        for (auto c = g.edges(node).first_child; c; c = g.edges(c).next_sibling)
            record_names({ *c }, into);
    }

    // An inner function, its arguments hide outer names.
    void scan_function(ast::DeclareArgumentListIndex args, ast::Index contents) {
        hidden_names.emplace_back();
        for (auto it = g.children(args).iter(); it; ++it) {
            const auto arg = *it;
            if (const auto name = g.as_a<ast::NamedIdentifierIndex>(arg)) {
                hide(*name);
            } else if (const auto with_default = g.as_a<ast::DeclareArgumentWithDefaultIndex>(arg)) {
                const auto [name, value] = g.children(*with_default);
                scan({ *value });
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

// The class of a constant, e.g., an element of a constant array.
TypeInfo type_of_constant(Context& ctx, const ConstExpression& c) {
    const auto& classes = ctx.common_classes;
    const OptionalSymSCClassName name = c.visit(util::overload {
        [&](double) -> OptionalSymSCClassName { return classes.float_; },
        [&](std::int32_t) -> OptionalSymSCClassName { return classes.int_; },
        [&](char) -> OptionalSymSCClassName { return classes.char_; },
        [&](bool b) -> OptionalSymSCClassName { return b ? classes.true_ : classes.false_; },
        [&](Nil) -> OptionalSymSCClassName { return classes.nil; },
        [&](SymSCSymbol) -> OptionalSymSCClassName { return classes.symbol; },
        [&](SymSCString) -> OptionalSymSCClassName { return classes.string; },
        [&](SymSCClassName n) -> OptionalSymSCClassName { return ctx.symbol_table.get_meta_class_name(n); },
        [&](const ConstExprArray&) -> OptionalSymSCClassName { return classes.array; },
        [&](const ConstExprEvent&) -> OptionalSymSCClassName { return classes.event; },
    });
    return TypeInfo { name, TypeConfidence::Absolute };
}

struct BodyContext {
    Expr& expr;
    IRGraph& ir;
    Context& ctx;
    const std::shared_ptr<const ast::ASTGraph>& ast_graph;
    join<SCFunction_I, SCMethod_I> owner;
    SCBody_I body;
    VariableUses uses;
};

// Adds the variable declared by declare to the scope. constant is its initial value, if it is known.
// Returns false if the name was already declared.
bool add_variable(BodyContext& b, ast::NamedIdentifierIndex name_i, SymSCIdentifier name,
                  join<SCVariableDeclare_I, SCPackTarget_I> declare, std::optional<ConstExprIndex> constant,
                  std::optional<TypeInfo> type) {
    const auto& g = *b.ast_graph;

    // Arguments and variables of the same function share a scope, so a name can't be declared twice.
    auto& current = b.expr.scope.back();
    if (const auto fnd = current.find(name); fnd != current.end()) {
        const auto previous = b.ir.location(fnd->second.declare_index);
        b.expr.sink(sc::diag::Diagnostic::duplicate_variable(b.ir.text_info(previous.source), IRGraph::range(previous),
                                                             get_text_info(g), get_location(g, name_i)));
        return false;
    }
    // Constant if its value is and it is never assigned or used by an inner function.
    // Mutable values, e.g., arrays, are never constant, as any method could change them, through this variable or any
    // other reference to the object.
    if (!b.uses.can_be_constant(name) || (constant && b.ctx.constant_table(*constant).is_mutable())) {
        constant = std::nullopt;
        type = std::nullopt;
    }
    b.expr.add_symbol(name,
                      ScopeEntry { nullptr, b.owner, declare, constant, type, ScopeEntry::Kind::Local,
                                   ScopeEntry::State::Initalised });
    return true;
}

// `var a = 1`, or `var a` when value is missing, which is nil.
// Returns whether the initial value is constant, so evaluating the declaration has no effects.
bool declare_variable(BodyContext& b, ast::NamedIdentifierIndex name_i, std::optional<ast::AnyExprIndex> value,
                      ast::Index whole) {
    const auto& g = *b.ast_graph;
    const auto name = b.ctx.symbol_table.get_identifier(std::string { get_string_view(g, name_i) });
    const auto declare_node = [&](maybe<SCAnyExpr_I> value_i) {
        const auto declare = b.ir.create(SCVariableDeclare { { location_of(b.ir, g, { *name_i }), name } },
                                         location_of(b.ir, g, whole), value_i);
        b.ir.append(b.body, declare);
        return declare;
    };
    if (!value) {
        const auto declare = declare_node(b.ir.create(SCMissing {}, location_of(b.ir, g, whole)));
        (void)add_variable(b, name_i, name, declare, b.ctx.constant_table.register_constexpr({ Nil {} }),
                           TypeInfo { b.ctx.common_classes.nil, TypeConfidence::Absolute });
        return true;
    }
    // Compiled before the variable is declared, so it can't refer to itself.
    const auto value_r = b.expr(*value);
    (void)add_variable(b, name_i, name, declare_node(value_r.ir_node), value_r.const_index, value_r.type);
    return value_r.const_index.has_value();
}

// `var #a, b ...c = value`, see SCPackDeclare.
// When value is a constant array, and `at` is known, the variables have its elements as their initial values.
// Returns whether evaluating it has no effects.
bool declare_pack(BodyContext& b, ast::VariablePackAssignIndex pack) {
    const auto& g = *b.ast_graph;
    const auto [names, value] = g.children(pack);
    // Compiled before the variables are declared, so it can't refer to them.
    const auto value_r = b.expr(value);
    const auto [list, rest] = g.children(names);

    const ConstExprArray* elements = nullptr;
    if (value_r.const_index && b.ctx.known_methods.is_known(b.ctx.common_classes.array, b.ctx.common_symbols.at))
        elements = b.ctx.constant_table(*value_r.const_index).get_if<ConstExprArray>();

    const auto targets = b.ir.create(SCPackTargets {}, location_of(b.ir, g, { *names }));
    const auto add_target = [&](ast::Index target, std::uint32_t index, bool is_rest) {
        if (const auto env = g.as_a<ast::EnvIdentifierIndex>(target)) {
            b.expr.sink(sc::diag::Diagnostic::declared_environment_variable(get_text_info(g), get_location(g, *env)));
            return;
        }
        const auto name = g.as_a<ast::NamedIdentifierIndex>(target);
        assert(name);
        // `at` past the end is nil, the rest is a new array, so is never constant.
        std::optional<ConstExprIndex> constant;
        std::optional<TypeInfo> type;
        if (elements && !is_rest) {
            ConstExpression element = index < elements->size() ? (*elements)[index] : ConstExpression { Nil {} };
            type = type_of_constant(b.ctx, element);
            constant = b.ctx.constant_table.register_constexpr(std::move(element));
        }
        const auto sym = b.ctx.symbol_table.get_identifier(std::string { get_string_view(g, *name) });
        const auto loc = location_of(b.ir, g, { **name });
        const auto declare = b.ir.create(SCPackTarget { { loc, sym }, SCAssign::Kind::Local, index, is_rest }, loc);
        b.ir.append(targets, declare);
        (void)add_variable(b, *name, sym, declare, constant, type);
    };

    std::uint32_t index { 0 };
    for (auto it = g.children(list).iter(); it; ++it, ++index)
        add_target({ **it }, index, false);
    const bool has_rest = !g.as_a<ast::MissingIndex>(rest);
    if (has_rest)
        add_target({ *rest }, index, true);

    b.ir.append(b.body, b.ir.create(SCPackDeclare {}, location_of(b.ir, g, { *pack }), value_r.ir_node, targets));
    // copyToEnd isn't known, so only a pack without a rest has no effects.
    return elements && !has_rest;
}

// Returns whether every initial value is constant.
bool declare_variables(BodyContext& b, ast::DeclareVariableListIndex list) {
    const auto& g = *b.ast_graph;
    bool all_constant = true;
    for (auto it = g.children(list).iter(); it; ++it) {
        const ast::DeclareAnyVariableIndex item = *it;
        if (const auto name = g.as_a<ast::NamedIdentifierIndex>(item)) {
            all_constant = declare_variable(b, *name, std::nullopt, { **name }) && all_constant;
        } else if (const auto with_default = g.as_a<ast::DeclareVariableWithDefaultIndex>(item)) {
            const auto [name_i, value] = g.children(*with_default);
            all_constant = declare_variable(b, name_i, value, { **with_default }) && all_constant;
        } else if (const auto pack = g.as_a<ast::VariablePackAssignIndex>(item)) {
            all_constant = declare_pack(b, *pack) && all_constant;
        }
    }
    return all_constant;
}
}

BodyResult compile_body(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph, Context& ctx,
                        join<SCFunction_I, SCMethod_I> owner, SCBody_I body, ast::BlockContentsListIndex contents) {
    VariableUses uses;
    VariableScanner { *ast_graph, ctx.symbol_table, uses }.scan({ *contents });
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
