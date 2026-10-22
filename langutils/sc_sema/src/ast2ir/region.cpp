#include "region.hpp"
#include "ast2ir/arguments.hpp"
#include "ast2ir/body.hpp"
#include "ast2ir/method.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"

namespace sc::ir::ast2ir {

Interpreted::Interpreted(IRGraph& ir, std::shared_ptr<const ast::ASTGraph> ast, Context& cxt,
                         const Scope* meta_class_members, const Scope& class_members):
    Base(ir, ast, cxt),
    expr(ir, std::move(ast), cxt, sink) {
    push_member_scopes(expr, meta_class_members, class_members);
    expr.returns_allowed = false;
}

// Each item is a region, compiled into its own function, evaluated in turn, and the last's value is the result. Each
// has its own scope, an expression can't declare a variable, so `a = 1` assigns the Interpreter's member, but
// `(var a = 1; a)` declares its own.
Return<SCInterpreted_I> Interpreted::operator()(ast::RegionListIndex i) {
    const auto list = ir.create(SCRegionList {}, location({ *i }));
    // An empty region is nil.
    OptionalConstExprIndex result = ctx.constant_table.register_constexpr({ Nil {} });
    for (auto it = ast_graph->children(i).iter(); it; ++it) {
        const ast::RegionItemIndex item = *it;
        // The parser has reported it.
        if (ast_graph->as_a<ast::ErrorIndex>(item)) {
            result = {};
            continue;
        }
        // Whether this region is fatal, rather than an earlier one.
        const bool earlier_fatal = std::exchange(sink.fatal, false);
        const auto function = [&]() -> SCFunction_I {
            if (const auto block = ast_graph->as_a<ast::ImmediatelyEvaluatedBlockIndex>(item))
                return immediately_evaluated(*block);
            return expression(ast::AnyExprIndex { *item });
        }();
        if (sink.fatal)
            ir.payload(function).constant_result = {};
        sink.fatal = sink.fatal || earlier_fatal;
        ir.append(list, ir.create(SCRegion {}, ir.location(NodeIndex { *function }), function));
        result = ir.payload(function).constant_result;
    }
    if (!sink.fatal)
        ir.payload(list).constant_result = result;
    return finish(list);
}

// `|x| x + 1`, or `var a = 1; a`, the whole file is the function's body.
Return<SCInterpreted_I> Interpreted::operator()(ast::FileAsFunctionIndex i) {
    const auto [arg_list, contents] = ast_graph->children(i);
    const auto here = location({ *i });
    const auto positional = ir.create(SCArgumentDeclareList {}, location({ *arg_list }));
    const auto variadic = ir.create(SCArgumentDeclareList {}, location({ *arg_list }));
    const auto body = ir.create(SCBody {}, here);
    const auto function = ir.create(SCFunction {}, here, positional, variadic, body);

    expr.scope.emplace_back();
    expr.scope.back().starts_function = true;
    declare_arguments(expr, ir, ast_graph, ctx, positional, variadic, arg_list, function);
    const auto result = compile_body(expr, ir, ast_graph, ctx, function, body, contents);
    expr.scope.pop_back();

    // Like a block, only known without arguments.
    const bool has_arguments = ir.children(positional).iter() || ir.children(variadic).iter();
    if (!result.explicit_return && !has_arguments && !sink.fatal && result.constant)
        ir.payload(function).constant_result = *result.constant;
    return finish(function);
}

// `a = 1`, a function whose body is the expression, in its own scope.
SCFunction_I Interpreted::expression(ast::AnyExprIndex i) {
    const auto here = location({ *i });
    const auto positional = ir.create(SCArgumentDeclareList {}, here);
    const auto variadic = ir.create(SCArgumentDeclareList {}, here);
    const auto body = ir.create(SCBody {}, here);
    const auto function = ir.create(SCFunction {}, here, positional, variadic, body);

    expr.scope.emplace_back();
    expr.scope.back().starts_function = true;
    const auto r = expr(i);
    expr.scope.pop_back();
    ir.append(body, r.ir_node);
    if (r.const_index)
        ir.payload(function).constant_result = *r.const_index;
    return function;
}

// `(var a = 1; a + 1)` is a function that is called straight away, so its arguments are their defaults. Its value is
// known if the function's result is.
SCFunction_I Interpreted::immediately_evaluated(ast::ImmediatelyEvaluatedBlockIndex i) {
    const auto [arg_list, contents] = ast_graph->children(i);
    const auto function = compile_function(expr, ir, ast_graph, ctx, { *i }, arg_list, contents);
    return *ir.as_a<SCFunction_I>(NodeIndex { *function.ir_node });
}

Return<SCInterpreted_I> Interpreted::finish(join<SCFunction_I, SCRegionList_I> code) {
    const auto interpreted = ir.create(SCInterpreted {}, ir.location(NodeIndex { *code }), code);
    ir.payload(interpreted).poisoned = sink.fatal;
    ctx.diagnostics(std::move(sink.diagnostics));
    sink = {};
    return { interpreted };
}
}
