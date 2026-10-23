// Reading and writing variables, and the scope used to resolve them.
// Needs the ast graph for the children and payloads, and the symbol table for the names.
#include "ast2ir/expr.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/common_symbols.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"

namespace sc::ir::ast2ir {

void Expr::add_symbol(Symbol n, ScopeEntry e) { scope.back().insert({ n, e }); }

bool Expr::declared(Symbol name) {
    const auto k = scope.size() - 1;
    const auto fnd = std::find_if(initialising.begin(), initialising.end(),
                                  [&](const auto& v) { return v.later && v.scope == k && v.name == name; });
    if (fnd == initialising.end())
        return false;
    const bool escaped = fnd->escaped;
    initialising.erase(fnd);
    return escaped;
}

void Expr::close_frame() {
    const auto k = scope.size() - 1;
    initialising.erase(std::remove_if(initialising.begin(), initialising.end(),
                                      [&](const auto& v) { return v.later && v.scope == k; }),
                       initialising.end());
}

SourceLocation declaration_location(IRGraph& ir, const ScopeEntry& entry) {
    if (!entry.graph)
        return ir.location(entry.declare_index);
    return ir.make_location(entry.declaration_text, entry.declaration_range);
}

std::optional<ScopeEntry> Expr::resolve_symbol(Symbol i, bool get_uninitalised) const {
    for (auto it = scope.rbegin(); it != scope.rend(); ++it) {
        if (auto fnd = it->find(i); fnd != it->end()) {
            if (fnd->second.state == ScopeEntry::State::Initalised || get_uninitalised) {
                return { fnd->second };
            }
        }
    }
    return {};
}

Return<SCAnyExpr_I> Expr::operator()(ast::EnvIdentifierIndex i) {
    const auto& t = get_text_info(*ast_graph);
    const auto [child] = ast_graph->children(i);
    const auto loc = get_location(*ast_graph, child);
    const auto txt = get_string_view(*ast_graph, child);
    const auto name_sym = ctx.symbol_table.get_identifier(std::string { txt });
    const auto index = ir.create(SCRead { name_sym, SCRead::Kind::Environment }, location({ *i }));
    return { index };
}

namespace {
SCRead::Kind to_read_kind(ScopeEntry::Kind k) {
    switch (k) {
    case ScopeEntry::Kind::Local:
    case ScopeEntry::Kind::Argument:
        return SCRead::Kind::Local;
    case ScopeEntry::Kind::Instance:
        return SCRead::Kind::Instance;
    case ScopeEntry::Kind::Class:
        return SCRead::Kind::Class;
    }
    assert(false && "unhandled ScopeEntry::Kind");
    return SCRead::Kind::Local;
}

SCAssign::Kind to_assign_kind(ScopeEntry::Kind k) {
    switch (k) {
    case ScopeEntry::Kind::Local:
    case ScopeEntry::Kind::Argument:
        return SCAssign::Kind::Local;
    case ScopeEntry::Kind::Instance:
        return SCAssign::Kind::Instance;
    case ScopeEntry::Kind::Class:
        return SCAssign::Kind::Class;
    }
    assert(false && "unhandled ScopeEntry::Kind");
    return SCAssign::Kind::Local;
}
}

// Reads are constant when the variable is, see compile_body.
namespace {
std::optional<SCSpecial::Kind> special_kind(const CommonSymbols& s, SymSCIdentifier name) {
    if (name == s.this_)
        return SCSpecial::Kind::This;
    if (name == s.super_)
        return SCSpecial::Kind::Super;
    if (name == s.this_process)
        return SCSpecial::Kind::Process;
    if (name == s.this_thread)
        return SCSpecial::Kind::Thread;
    if (name == s.this_method)
        return SCSpecial::Kind::Method;
    if (name == s.this_function_def)
        return SCSpecial::Kind::FunctionDef;
    if (name == s.this_function)
        return SCSpecial::Kind::Function;
    return std::nullopt;
}
}

namespace {
// A use of a variable whose initial value is being compiled, or, in legacy code, isn't declared yet, see
// Expr::initialising.
struct InitialisingUse {
    Expr::Initialising* variable;
    // Whether the use is in a function in the initial value.
    bool in_function;
};

// Whether a use of the variable refers to it before it is declared. A function in its initial value runs after it is
// assigned, so can, e.g., `var f = { f }`. Legacy code's frame holds a function's arguments and variables, so they
// can be used anywhere in its leading declarations. Otherwise the use refers to the name it hides, e.g., `var x = x +
// 1` reads the outer `x`.
bool visible(const Expr::Initialising& variable, bool in_function, ast::LanguageVersion version) {
    return variable.later || in_function || (variable.in_frame && frame_scoped(version));
}

// Whether name refers to a variable being initialised, rather than one already declared, which it would hide. The one
// being initialised is the latest, so it is found before its own entry in legacy code's frame. One that isn't visible
// is skipped, so the search goes on to the names it hides. A legacy generator qualifier's is skipped even from a
// function, which 3.16 would refer to, so that use, at, warns.
std::optional<InitialisingUse> find_initialising(Expr& expr, Symbol name, const ast::ASTGraph& g, ast::Index at) {
    bool in_function = false;
    for (std::size_t k = expr.scope.size(); k-- > 0;) {
        for (auto variable = expr.initialising.rbegin(); variable != expr.initialising.rend(); ++variable) {
            if (variable->scope != k || variable->name != name || !visible(*variable, in_function, expr.version))
                continue;
            if (!variable->outside_value)
                return InitialisingUse { &*variable, in_function };
            const auto& d = variable->declaration;
            expr.sink(sc::diag::Diagnostic::qualifier_hidden_from_value(g.text_info(), IRGraph::range(d),
                                                                        get_text_info(g), get_location(g, at)));
        }
        if (expr.scope[k].count(name))
            return std::nullopt;
        if (expr.scope[k].starts_function)
            in_function = true;
    }
    return std::nullopt;
}

// The declaration a use of an initialising variable refers to. Legacy code using one before its declaration refers to
// the variable in its frame, which is nil until its initial value runs, unless that is a literal, but 3.16 doesn't,
// so it warns.
SourceLocation resolve_initialising(InitialisingUse use, bool assigns, DiagnosticSink& sink, IRGraph& ir,
                                    const ast::ASTGraph& g, ast::Index at) {
    auto& variable = *use.variable;
    if (variable.later || !use.in_function) {
        sink(sc::diag::Diagnostic::used_before_declaration(ir.text_info(variable.declaration.source),
                                                           IRGraph::range(variable.declaration), get_text_info(g),
                                                           get_location(g, at)));
        variable.escaped = variable.escaped || assigns || use.in_function;
    }
    return variable.declaration;
}
}

Return<SCAnyExpr_I> Expr::operator()(ast::NamedIdentifierIndex i) {
    const auto txt = get_string_view(*ast_graph, i);
    const auto name_sym = ctx.symbol_table.get_identifier(std::string { txt });
    // These can't be declared, so can't be hidden by a variable.
    if (const auto kind = special_kind(ctx.common_symbols, name_sym))
        return { ir.create(SCSpecial { *kind }, location({ *i })) };
    if (const auto use = find_initialising(*this, name_sym, *ast_graph, { *i }))
        return { ir.create(
            SCRead { name_sym, SCRead::Kind::Local, resolve_initialising(*use, false, sink, ir, *ast_graph, { *i }) },
            location({ *i })) };
    if (const auto entry = resolve_symbol(name_sym)) {
        const auto read = ir.create(SCRead { name_sym, to_read_kind(entry->kind), declaration_location(ir, *entry) },
                                    location({ *i }));
        // Only locals can be constant, a member's initial value isn't, as the member can be assigned.
        if (entry->kind == ScopeEntry::Kind::Local && entry->const_expr) {
            ir.register_node_as_consteval(read, *entry->const_expr);
            if (entry->type_info)
                return { read, *entry->const_expr, *entry->type_info };
        }
        return { read };
    }

    sink(sc::diag::Diagnostic::undefined_variable(get_text_info(*ast_graph), get_location(*ast_graph, i), txt));
    return { ir.create(SCExprList {}, location({ *i })) };
}

Return<SCAnyExpr_I> Expr::operator()(ast::AssignmentIndex i) {
    const auto [name, expr] = ast_graph->children(i);
    const auto expr_r = (*this)(ast::AnyExprIndex { expr });

    const auto txt = get_string_view(*ast_graph, name);
    const auto name_sym = ctx.symbol_table.get_identifier(std::string { txt });
    const Locatable<SymSCIdentifier> located_name { location({ *name }), name_sym };

    const auto make_return = [&](SCAssign::Kind kind,
                                 std::optional<SourceLocation> declaration) -> Return<SCAnyExpr_I> {
        // Assignment returns the assigned value, but is never a const expression as it has a side effect.
        const auto index = ir.create(SCAssign { located_name, kind, declaration }, location({ *i }), expr_r.ir_node);
        if (expr_r.type)
            return { index, *expr_r.type };
        return { index };
    };

    // Environment vars are not registered in the scope.
    if (ast_graph->payload(i).target == ast::AssignmentNode::Target::Environment) {
        return make_return(SCAssign::Kind::Environment, std::nullopt);
    }

    if (ctx.common_symbols.is_special(name_sym)) {
        sink(
            sc::diag::Diagnostic::assigned_special_variable(get_text_info(*ast_graph), get_location(*ast_graph, name)));
        return expr_r;
    }
    if (const auto use = find_initialising(*this, name_sym, *ast_graph, { *name }))
        return make_return(SCAssign::Kind::Local, resolve_initialising(*use, true, sink, ir, *ast_graph, { *name }));

    // TODO: error when assigning to a const once ScopeEntry records const-ness.
    if (const auto entry = resolve_symbol(name_sym))
        return make_return(to_assign_kind(entry->kind), declaration_location(ir, *entry));

    sink(sc::diag::Diagnostic::undefined_variable(get_text_info(*ast_graph), get_location(*ast_graph, name), txt));
    return expr_r;
}

// `#a, ~b ...c = value`, see SCPackAssign.
Return<SCAnyExpr_I> Expr::operator()(ast::VariablePackAssignIndex i) {
    const auto [names, value] = ast_graph->children(i);
    const auto value_r = (*this)(ast::AnyExprIndex { value });
    const auto [list, rest] = ast_graph->children(names);

    const auto targets = ir.create(SCPackTargets {}, location({ *names }));
    const auto add_target = [&](ast::Index target, std::uint32_t index, bool is_rest) {
        if (const auto env = ast_graph->as_a<ast::EnvIdentifierIndex>(target)) {
            const auto [env_name] = ast_graph->children(*env);
            const auto sym = ctx.symbol_table.get_identifier(std::string { get_string_view(*ast_graph, env_name) });
            ir.append(
                targets,
                ir.create(SCPackTarget { { location({ **env }), sym }, SCAssign::Kind::Environment, index, is_rest },
                          location({ **env })));
            return;
        }
        const auto name = ast_graph->as_a<ast::NamedIdentifierIndex>(target);
        assert(name);
        const auto txt = get_string_view(*ast_graph, *name);
        const auto sym = ctx.symbol_table.get_identifier(std::string { txt });
        if (ctx.common_symbols.is_special(sym)) {
            sink(sc::diag::Diagnostic::assigned_special_variable(get_text_info(*ast_graph),
                                                                 get_location(*ast_graph, *name)));
            return;
        }
        if (const auto use = find_initialising(*this, sym, *ast_graph, { **name })) {
            ir.append(targets,
                      ir.create(SCPackTarget { { location({ **name }), sym },
                                               SCAssign::Kind::Local,
                                               index,
                                               is_rest,
                                               resolve_initialising(*use, true, sink, ir, *ast_graph, { **name }) },
                                location({ **name })));
            return;
        }
        if (const auto entry = resolve_symbol(sym)) {
            ir.append(targets,
                      ir.create(SCPackTarget { { location({ **name }), sym },
                                               to_assign_kind(entry->kind),
                                               index,
                                               is_rest,
                                               declaration_location(ir, *entry) },
                                location({ **name })));
            return;
        }
        sink(sc::diag::Diagnostic::undefined_variable(get_text_info(*ast_graph), get_location(*ast_graph, *name), txt));
    };

    std::uint32_t index { 0 };
    for (auto it = ast_graph->children(list).iter(); it; ++it, ++index)
        add_target({ **it }, index, false);
    if (!ast_graph->as_a<ast::MissingIndex>(rest))
        add_target({ *rest }, index, true);

    const auto pack = ir.create(SCPackAssign {}, location({ *i }), value_r.ir_node, targets);
    if (value_r.type)
        return { pack, *value_r.type };
    return { pack };
}
}
