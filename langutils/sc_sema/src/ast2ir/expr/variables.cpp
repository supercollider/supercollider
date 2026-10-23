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

void Expr::predeclare_legacy_frame(join<SCFunction_I, SCMethod_I> owner, ast::DeclareArgumentListIndex args,
                                   std::optional<ast::BlockContentsListIndex> contents) {
    if (!is_frame_scoped(version))
        return;
    const auto& g = *ast_graph;
    // The locations are those the declarations get, so a read before one refers to the same place.
    // Each is a new variable, a name declared twice is two, see local_for_declaration.
    const auto later = [&](ast::NamedIdentifierIndex name, ast::Index whole) {
        const auto sym = ctx.symbol_table.get_identifier(std::string { get_string_view(g, name) });
        const auto loc = location_of(ir, g, whole);
        Initialising variable { sym, scope.size() - 1, loc, create_local(sym, loc, owner) };
        variable.in_frame = true;
        variable.later = true;
        initialising.push_back(variable);
    };

    for (auto it = g.children(args).iter(); it; ++it) {
        const ast::Index arg { **it };
        if (const auto name = g.as_a<ast::NamedIdentifierIndex>(arg)) {
            later(*name, arg);
        } else if (const auto with_default = g.as_a<ast::DeclareArgumentWithDefaultIndex>(arg)) {
            const auto [name, value] = g.children(*with_default);
            later(name, arg);
        } else if (const auto variadic = g.as_a<ast::DeclareArgumentVariadicIndex>(arg)) {
            const auto [name] = g.children(*variadic);
            later(name, { *name });
        }
    }

    if (!contents)
        return;
    for (auto it = g.children(*contents).iter(); it; ++it) {
        const auto declarations = g.as_a<ast::DeclareVariableListIndex>(ast::Index { **it });
        if (!declarations)
            return;
        for (auto item = g.children(*declarations).iter(); item; ++item) {
            const ast::Index declaration { **item };
            if (const auto name = g.as_a<ast::NamedIdentifierIndex>(declaration)) {
                later(*name, declaration);
            } else if (const auto with_default = g.as_a<ast::DeclareVariableWithDefaultIndex>(declaration)) {
                const auto [name, value] = g.children(*with_default);
                later(name, declaration);
            } else if (const auto pack = g.as_a<ast::VariablePackAssignIndex>(declaration)) {
                const auto [names, value] = g.children(*pack);
                const auto [list, rest] = g.children(names);
                for (auto target = g.children(list).iter(); target; ++target)
                    if (const auto name = g.as_a<ast::NamedIdentifierIndex>(ast::Index { **target }))
                        later(*name, { **name });
                if (const auto name = g.as_a<ast::NamedIdentifierIndex>(ast::Index { *rest }))
                    later(*name, { **name });
            }
        }
    }
}

LocalIndex Expr::create_local(SymSCIdentifier name, SourceLocation declaration, SCLocalOwner_I owner) {
    return ir.add_local({ name, owner, declaration });
}

LocalIndex Expr::local_for_declaration(SymSCIdentifier name, SourceLocation declaration, SCLocalOwner_I owner) {
    const auto k = scope.size() - 1;
    const auto fnd = std::find_if(initialising.begin(), initialising.end(),
                                  [&](const auto& v) { return v.later && v.scope == k && v.name == name; });
    if (fnd != initialising.end())
        return fnd->local;
    return create_local(name, declaration, owner);
}

void Expr::close_legacy_frame() {
    const auto k = scope.size() - 1;
    initialising.erase(std::remove_if(initialising.begin(), initialising.end(),
                                      [&](const auto& v) { return v.later && v.scope == k; }),
                       initialising.end());
}

SourceLocation declaration_location(IRGraph& ir, const ScopeEntry& entry) {
    if (entry.local)
        return ir.local(*entry.local).declaration;
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

void Expr::record_local_use(Symbol name, LocalIndex local, bool assigns) {
    // Found the same way as resolve_symbol, it is captured if a function starts inside the scope declaring it.
    bool in_function = false;
    for (std::size_t k = scope.size(); k-- > 0;) {
        if (const auto fnd = scope[k].find(name);
            fnd != scope[k].end() && fnd->second.state == ScopeEntry::State::Initalised)
            break;
        in_function = in_function || scope[k].starts_function;
    }
    auto& variable = ir.local(local);
    variable.assigned_to = variable.assigned_to || assigns;
    variable.captured = variable.captured || in_function;
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
    return variable.later || in_function || (variable.in_frame && is_frame_scoped(version));
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

// The variable a use of an initialising variable refers to. Legacy code using one before its declaration refers to
// the variable in its frame, which is nil until its initial value runs, unless that is a literal, but 3.16 doesn't,
// so it warns.
LocalIndex resolve_initialising(InitialisingUse use, bool assigns, DiagnosticSink& sink, IRGraph& ir,
                                const ast::ASTGraph& g, ast::Index at) {
    auto& variable = *use.variable;
    if (variable.later || !use.in_function) {
        sink(sc::diag::Diagnostic::used_before_declaration(ir.text_info(variable.declaration.source),
                                                           IRGraph::range(variable.declaration), get_text_info(g),
                                                           get_location(g, at)));
        variable.escaped = variable.escaped || assigns || use.in_function;
    }
    auto& local = ir.local(variable.local);
    local.assigned_to = local.assigned_to || assigns;
    local.captured = local.captured || use.in_function;
    return variable.local;
}

// Reports assigning to a constant at the name at, returns whether it is one.
bool assigns_to_constant(Expr& expr, const ScopeEntry& entry, const ast::ASTGraph& g, ast::Index at) {
    if (!entry.is_const)
        return false;
    expr.sink(sc::diag::Diagnostic::assigned_constant(entry.declaration_text, entry.declaration_range, get_text_info(g),
                                                      get_location(g, at)));
    return true;
}

void report_undefined(Expr& expr, IRGraph& ir, Symbol name, const ast::ASTGraph& g, ast::Index at) {
    const auto declaration = [&]() -> std::optional<SourceLocation> {
        for (auto outside = expr.outside_closed.rbegin(); outside != expr.outside_closed.rend(); ++outside) {
            for (auto v = outside->initialising.rbegin(); v != outside->initialising.rend(); ++v)
                if (v->name == name)
                    return v->declaration;
            for (auto s = outside->scope.rbegin(); s != outside->scope.rend(); ++s)
                if (const auto fnd = s->find(name); fnd != s->end())
                    return declaration_location(ir, fnd->second);
        }
        return std::nullopt;
    }();
    if (declaration) {
        expr.sink(sc::diag::Diagnostic::outside_closed_function(
            ir.text_info(declaration->source), IRGraph::range(*declaration), get_text_info(g), get_location(g, at)));
        return;
    }
    expr.sink(sc::diag::Diagnostic::undefined_variable(get_text_info(g), get_location(g, at), get_string_view(g, at)));
}

// A member is found by the class declaring it, see MemberVariable, locals by their index.
OptionalMemberIndex member_of(IRGraph& ir, SymSCIdentifier name, const ScopeEntry& entry) {
    if (!entry.owning_class)
        return {};
    return ir.member_index({ name, *entry.owning_class, declaration_location(ir, entry) });
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
        if (entry->local)
            record_local_use(name_sym, LocalIndex { *entry->local }, false);
        const auto read =
            ir.create(SCRead { name_sym, to_read_kind(entry->kind), entry->local, member_of(ir, name_sym, *entry) },
                      location({ *i }));
        // Only locals can be constant, a member's initial value isn't, as the member can be assigned.
        if (entry->kind == ScopeEntry::Kind::Local && entry->const_expr) {
            ir.register_node_as_consteval(read, *entry->const_expr);
            if (entry->type_info)
                return { read, *entry->const_expr, *entry->type_info };
        }
        return { read };
    }

    report_undefined(*this, ir, name_sym, *ast_graph, { *i });
    return { ir.create(SCExprList {}, location({ *i })) };
}

Return<SCAnyExpr_I> Expr::operator()(ast::AssignmentIndex i) {
    const auto [name, expr] = ast_graph->children(i);
    const auto expr_r = (*this)(ast::AnyExprIndex { expr });

    const auto txt = get_string_view(*ast_graph, name);
    const auto name_sym = ctx.symbol_table.get_identifier(std::string { txt });
    const Locatable<SymSCIdentifier> located_name { location({ *name }), name_sym };

    const auto make_return = [&](SCAssign::Kind kind, OptionalLocalIndex local,
                                 OptionalMemberIndex member) -> Return<SCAnyExpr_I> {
        // Assignment returns the assigned value, but is never a const expression as it has a side effect.
        const auto index = ir.create(SCAssign { located_name, kind, local, member }, location({ *i }), expr_r.ir_node);
        if (expr_r.type)
            return { index, *expr_r.type };
        return { index };
    };

    // Environment vars are not registered in the scope.
    if (ast_graph->payload(i).target == ast::AssignmentNode::Target::Environment) {
        return make_return(SCAssign::Kind::Environment, {}, {});
    }

    if (ctx.common_symbols.is_special(name_sym)) {
        sink(
            sc::diag::Diagnostic::assigned_special_variable(get_text_info(*ast_graph), get_location(*ast_graph, name)));
        return expr_r;
    }
    if (const auto use = find_initialising(*this, name_sym, *ast_graph, { *name }))
        return make_return(SCAssign::Kind::Local, resolve_initialising(*use, true, sink, ir, *ast_graph, { *name }),
                           {});

    if (const auto entry = resolve_symbol(name_sym)) {
        if (assigns_to_constant(*this, *entry, *ast_graph, { *name }))
            return expr_r;
        if (entry->local)
            record_local_use(name_sym, LocalIndex { *entry->local }, true);
        return make_return(to_assign_kind(entry->kind), entry->local, member_of(ir, name_sym, *entry));
    }

    report_undefined(*this, ir, name_sym, *ast_graph, { *name });
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
            if (assigns_to_constant(*this, *entry, *ast_graph, { **name }))
                return;
            if (entry->local)
                record_local_use(sym, LocalIndex { *entry->local }, true);
            ir.append(targets,
                      ir.create(SCPackTarget { { location({ **name }), sym },
                                               to_assign_kind(entry->kind),
                                               index,
                                               is_rest,
                                               entry->local,
                                               member_of(ir, sym, *entry) },
                                location({ **name })));
            return;
        }
        report_undefined(*this, ir, sym, *ast_graph, { **name });
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
