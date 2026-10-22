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
}

// Each item is evaluated in turn, and the last's value is the result. Items share the function's scope, so `a = 1`
// assigns the Interpreter's member.
Return<SCInterpreted_I> Interpreted::operator()(ast::RegionListIndex i) {
    const auto here = location({ *i });
    const auto positional = ir.create(SCArgumentDeclareList {}, here);
    const auto variadic = ir.create(SCArgumentDeclareList {}, here);
    const auto body = ir.create(SCBody {}, here);
    const auto function = ir.create(SCFunction {}, here, positional, variadic, body);

    expr.scope.emplace_back();
    expr.scope.back().starts_function = true;
    // An empty region is nil.
    std::optional<ConstExprIndex> result = ctx.constant_table.register_constexpr({ Nil {} });
    for (auto it = ast_graph->children(i).iter(); it; ++it) {
        const ast::RegionItemIndex item = *it;
        // The parser has reported it.
        if (ast_graph->as_a<ast::ErrorIndex>(item)) {
            result = std::nullopt;
            continue;
        }
        if (const auto block = ast_graph->as_a<ast::ImmediatelyEvaluatedBlockIndex>(item)) {
            sink(sc::diag::Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, *block)));
            result = std::nullopt;
            continue;
        }
        const auto r = expr(ast::AnyExprIndex { *item });
        ir.append(body, r.ir_node);
        result = r.const_index;
    }
    expr.scope.pop_back();
    return finish(function, result, false);
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

    const bool has_arguments = ir.children(positional).iter() || ir.children(variadic).iter();
    return finish(function, result.explicit_return ? std::nullopt : result.constant, has_arguments);
}

Return<SCInterpreted_I> Interpreted::finish(SCFunction_I function, std::optional<ConstExprIndex> result,
                                            bool has_arguments) {
    const auto interpreted = ir.create(SCInterpreted {}, ir.location(NodeIndex { *function }), function);
    // Like a block, only known without arguments.
    if (result && !has_arguments && !sink.fatal)
        ir.payload(function).constant_result = *result;
    ir.payload(interpreted).poisoned = sink.fatal;
    ctx.diagnostics(std::move(sink.diagnostics));
    sink = {};
    return { interpreted };
}

}
