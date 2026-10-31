#include "method.hpp"
#include "ast2ir/arguments.hpp"
#include "ast2ir/body.hpp"
#include "ast2ir/expr.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"

namespace sc::ir::ast2ir {

Scope member_scope(std::shared_ptr<const IRGraph> class_ir, SCMemberList_I members) {
    Scope scope;
    for (auto it = class_ir->children(members).iter(); it; ++it) {
        const SCMember_I member_i = *it;
        const SCMember& member = class_ir->payload(member_i);
        std::optional<ConstExprIndex> const_expr;
        std::optional<TypeInfo> type_info;
        if (member.expr) {
            const_expr = *member.expr;
            type_info = member.expr_type_info;
        }
        const auto loc = class_ir->location(member_i);
        ScopeEntry entry { class_ir,
                           members,
                           member_i,
                           const_expr,
                           type_info,
                           ScopeEntry::Kind::Instance,
                           ScopeEntry::State::Initalised,
                           class_ir->text_info(loc.source),
                           IRGraph::range(loc) };
        entry.owning_class = member.owning_class;
        entry.is_const = member.is_const;
        scope.insert({ member.name, entry });
    }
    return scope;
}

Scope member_scope(std::shared_ptr<const IRGraph> class_ir) {
    const auto root = class_ir->root();
    assert(root);
    const auto [members, _] = class_ir->children(std::get<SCClass_I>(*root));
    return member_scope(std::move(class_ir), members);
}

void push_member_scopes(Expr& expr, const Scope* meta_class_members, const Scope& class_members) {
    if (meta_class_members) {
        // The meta class's instance variables are this class's class variables.
        Scope class_vars = *meta_class_members;
        for (auto& [_, entry] : class_vars)
            entry.kind = ScopeEntry::Kind::Class;
        (void)expr.scope.push_back(std::move(class_vars));
    }
    (void)expr.scope.push_back(class_members);
}

Method::Method(IRGraph& ir, std::shared_ptr<const ast::ASTGraph> ast, Context& cxt, ast::LanguageVersion version,
               const Scope* meta_class_members, const Scope& class_members, const InterpreterScopes* interpreter):
    Base(ir, ast, cxt, version),
    expr(ir, std::move(ast), cxt, version, sink) {
    push_member_scopes(expr, meta_class_members, class_members);
    expr.interpreter = interpreter;
}

// The errors that poisoned a method, written in full, as the exception it throws shows them. Those the parser reported
// inside the method aren't in the sink.
static ConstExprIndex poison_message(Context& ctx, const ast::ASTGraph& ast, sc::src::SourceCodeRange range,
                                     const std::vector<sc::diag::Diagnostic>& sink_diagnostics) {
    std::string text;
    const auto add = [&](const sc::diag::Diagnostic& d) {
        if (!d.fatal())
            return;
        // Starts and ends with a blank line.
        auto written = d.to_string();
        const auto first = written.find_first_not_of('\n');
        const auto last = written.find_last_not_of('\n');
        if (first != std::string::npos)
            text += written.substr(first, last - first + 1) + '\n';
    };
    for (const auto& d : ast.diagnostics())
        if (d.within(ast.text_info(), range))
            add(d);
    for (const auto& d : sink_diagnostics)
        add(d);
    return ctx.constant_table.register_constexpr({ ctx.symbol_table.get_string(text) });
}

Return<SCMethod_I> Method::operator()(ast::AnyValidMethodIndex i, SymSCSelector selector, ast::Index name_index) {
    const auto [name, arg_list, maybe_prim, body] =
        std::visit([&](auto i) { return ast_graph->children(i); }, ast_graph->index_to_variant(i));

    const auto argument_list = ir.create(SCArgumentDeclareList {}, location({ *arg_list }));
    const auto variadic_args = ir.create(SCArgumentDeclareList {}, location({ *arg_list }));

    using PrimT = std::optional<Locatable<SymSCPrimitive>>;
    const PrimT primitive = std::visit(util::overload {
                                           [](ast::MissingIndex) -> PrimT { return {}; },
                                           [&](ast::PrimitiveIdentifierIndex i) -> PrimT {
                                               return { {
                                                   location({ *i }), //
                                                   ctx.symbol_table.get_primitive(ast_graph->string(i)) //
                                               } };
                                           },
                                       },
                                       ast_graph->index_to_variant(maybe_prim));

    const auto body_i = ir.create(SCBody {}, location({ *body }));
    // The method is created first, so its arguments can refer to it as their scope.
    const auto method = ir.create(SCMethod { { location(name_index), selector }, primitive, version }, location({ *i }),
                                  argument_list, variadic_args, body_i);

    // Not compiled, as it would overflow the stack, so it is an empty, poisoned method.
    if (const auto deep = nested_too_deeply(*ast_graph, { *i })) {
        auto too_deep = sc::diag::Diagnostic::nested_too_deeply(get_text_info(*ast_graph),
                                                                get_location(*ast_graph, *deep), max_nesting_depth);
        ir.payload(method).poisoned = true;
        ir.payload(method).poison_message = poison_message(ctx, *ast_graph, {}, { too_deep });
        ctx.diagnostics(std::move(too_deep));
        return { method };
    }

    // Methods have their own scope, holding the arguments and variables, inside the member scopes.
    (void)expr.scope.push_back({});
    expr.predeclare_legacy_frame(method, arg_list, body);
    declare_arguments(expr, ir, ast_graph, ctx, argument_list, variadic_args, arg_list, method);
    if (primitive)
        check_primitive(primitive->value, argument_list, variadic_args, { *arg_list }, { *maybe_prim }, name_index);
    const auto result = compile_body(expr, ir, ast_graph, ctx, method, body_i, body);
    expr.close_legacy_frame();
    expr.scope.pop_back();

    // A fatal error anywhere in the method poisons all of it.
    sink.fatal = sink.fatal || parse_error_inside(*ast_graph, ast_graph->location({ *i }));
    ir.payload(method).poisoned = sink.fatal;
    if (sink.fatal)
        ir.payload(method).poison_message =
            poison_message(ctx, *ast_graph, ast_graph->location({ *i }), sink.diagnostics);

    // Without `^` a method returns `this`, which isn't constant.
    // A primitive's body only runs when the primitive fails, so is never constant.
    const bool has_arguments = ir.children(argument_list).iter() || ir.children(variadic_args).iter();
    if (result.constant && result.explicit_return && !primitive && !has_arguments && !sink.fatal)
        ir.payload(method).constant_result = *result.constant;
    ctx.diagnostics(std::move(sink.diagnostics));
    sink = {};

    return { method };
}

// Only the VM knows its primitives' arguments, which the old compiler checked against its table too.
void Method::check_primitive(SymSCPrimitive primitive, SCArgumentDeclareList_I arguments,
                             SCArgumentDeclareList_I variadic, ast::Index arguments_i, ast::Index primitive_i,
                             ast::Index name_index) {
    const auto fnd = ctx.primitive_signatures.find(primitive);
    if (fnd == ctx.primitive_signatures.end())
        return;
    const auto& signature = fnd->second;
    const auto text = get_text_info(*ast_graph);
    const auto primitive_l = get_location(*ast_graph, primitive_i);
    // Without any arguments, the method's name.
    auto arguments_l = get_location(*ast_graph, arguments_i);
    if (arguments_l.size() == 0)
        arguments_l = get_location(*ast_graph, name_index);
    const auto count = [&](SCArgumentDeclareList_I list) {
        std::size_t n = 0;
        for (auto it = ir.children(list).iter(); it; ++it)
            ++n;
        return n;
    };
    const auto num_arguments = 1 + count(arguments);
    const auto num_variadic = count(variadic);
    if (num_arguments != signature.num_arguments)
        sink(sc::diag::Diagnostic::primitive_argument_count(text, arguments_l, primitive_l, signature.num_arguments,
                                                            num_arguments));
    if (signature.variadic_arguments && num_variadic < 1)
        sink(sc::diag::Diagnostic::primitive_needs_variadic_arguments(text, arguments_l, primitive_l));
    if (signature.keyword_arguments && num_variadic < 2)
        sink(sc::diag::Diagnostic::primitive_needs_keyword_arguments(text, arguments_l, primitive_l));
}
}