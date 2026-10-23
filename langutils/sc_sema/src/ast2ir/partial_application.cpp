#include "ast2ir/partial_application.hpp"
#include "ast2ir/expr.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"
#include <algorithm>
#include <string>
#include <utility>

namespace sc::ir::ast2ir {

std::vector<ast::CurryIndex> curry_arguments(const ast::ASTGraph& g, ast::Index node) {
    std::vector<ast::CurryIndex> curries;
    const auto add = [&](ast::Index i) {
        if (const auto curry = g.as_a<ast::CurryIndex>(i))
            curries.push_back(*curry);
    };
    const auto add_arguments = [&](ast::ArgumentListIndex list) {
        for (auto it = g.children(list).iter(); it; ++it) {
            const ast::ArgumentEntryIndex entry = *it;
            if (const auto keyword = g.as_a<ast::KwArgIndex>(entry)) {
                const auto [_, value] = g.children(*keyword);
                add({ *value });
            } else if (const auto variadic = g.as_a<ast::VariadicArgIndex>(entry)) {
                const auto [value] = g.children(*variadic);
                add({ *value });
            } else {
                add({ *entry });
            }
        }
    };
    const auto add_range = [&](ast::RangeIndex range) {
        const auto [args, last] = g.children(range);
        add_arguments(args);
        add({ *last });
    };
    const auto add_array = [&](ast::ArrayIndex array) {
        for (auto it = g.children(array).iter(); it; ++it)
            add({ **it });
    };

    if (const auto m = g.as_a<ast::PrefixMessageIndex>(node)) {
        const auto [_, args, blocks] = g.children(*m);
        add_arguments(args);
    } else if (const auto m = g.as_a<ast::PostfixMessageIndex>(node)) {
        const auto [receiver, _, args, blocks] = g.children(*m);
        add({ *receiver });
        add_arguments(args);
    } else if (const auto m = g.as_a<ast::BinaryMessageIndex>(node)) {
        const auto [_, lhs, rhs] = g.children(*m);
        add({ *lhs });
        add({ *rhs });
    } else if (const auto m = g.as_a<ast::ValueMessageIndex>(node)) {
        const auto [receiver, args, blocks] = g.children(*m);
        add({ *receiver });
        add_arguments(args);
    } else if (const auto m = g.as_a<ast::NewMessageIndex>(node)) {
        const auto [_, args, blocks] = g.children(*m);
        add_arguments(args);
    } else if (const auto m = g.as_a<ast::AtMessageIndex>(node)) {
        // `a[_]`, or a slice, `a[_.._]`, whose range is part of the call.
        const auto [receiver, arg] = g.children(*m);
        add({ *receiver });
        if (const auto range = g.as_a<ast::RangeIndex>(arg))
            add_range(*range);
        else if (const auto args = g.as_a<ast::ArgumentListIndex>(arg))
            add_arguments(*args);
    } else if (const auto a = g.as_a<ast::AssignmentAtIndex>(node)) {
        const auto [thing, args, value] = g.children(*a);
        add({ *thing });
        add_arguments(args);
        add({ *value });
    } else if (const auto s = g.as_a<ast::SetterIndex>(node)) {
        const auto [thing, _, value] = g.children(*s);
        if (const auto args = g.as_a<ast::ArgumentListIndex>(thing))
            add_arguments(*args);
        else
            add({ *thing });
        add({ *value });
    } else if (const auto r = g.as_a<ast::RangeAssignIndex>(node)) {
        const auto [receiver, range, value] = g.children(*r);
        add({ *receiver });
        add_range(range);
        add({ *value });
    } else if (const auto r = g.as_a<ast::ReferenceIndex>(node)) {
        // `` `_ `` is `Ref.new(_)`.
        const auto [value] = g.children(*r);
        add({ *value });
    } else if (const auto r = g.as_a<ast::RangeIndex>(node)) {
        add_range(*r);
    } else if (const auto a = g.as_a<ast::ArrayIndex>(node)) {
        add_array(*a);
    } else if (const auto c = g.as_a<ast::CollectionIndex>(node)) {
        const auto [_, array] = g.children(*c);
        add_array(array);
    } else if (const auto d = g.as_a<ast::DictionaryIndex>(node)) {
        for (auto it = g.children(*d).iter(); it; ++it) {
            const auto [key, value] = g.children(*it);
            add({ *key });
            add({ *value });
        }
    }

    // In the order they are written, which is the order of the function's arguments.
    std::sort(curries.begin(), curries.end(), [&](ast::CurryIndex l, ast::CurryIndex r) {
        return get_location(g, l).begin.absolute < get_location(g, r).begin.absolute;
    });
    return curries;
}

SymSCIdentifier curry_argument_name(Context& ctx, std::size_t n, std::size_t count) {
    return ctx.symbol_table.get_identifier(count == 1 ? "_" : "_" + std::to_string(n + 1));
}

Return<SCAnyExpr_I> partially_apply(Expr& expr, IRGraph& ir, const std::shared_ptr<const ast::ASTGraph>& ast_graph,
                                    Context& ctx, ast::AnyExprIndex node, const std::vector<ast::CurryIndex>& curries) {
    const auto here = location_of(ir, *ast_graph, { *node });
    // Created first, so its arguments can refer to it as their scope, like a block.
    const auto positional = ir.create(SCArgumentDeclareList {}, here);
    const auto variadic = ir.create(SCArgumentDeclareList {}, here);
    const auto body = ir.create(SCBody {}, here);
    const auto function = ir.create(SCFunction {}, here, positional, variadic, body);

    expr.scope.emplace_back();
    expr.scope.back().starts_function = true;
    for (std::size_t n { 0 }; n < curries.size(); ++n) {
        const auto name = curry_argument_name(ctx, n, curries.size());
        const auto curry_location = location_of(ir, *ast_graph, { *curries[n] });
        const auto missing_default = ir.create(SCMissing {}, curry_location);
        const auto local = expr.create_local(name, curry_location, function);
        const auto arg = ir.create(SCArgumentDeclare { name, local }, curry_location, missing_default);
        ir.append(positional, arg);
        ScopeEntry entry { nullptr, function, arg, {}, {}, ScopeEntry::Kind::Argument, ScopeEntry::State::Initalised };
        entry.local = local;
        expr.add_symbol(name, entry);
    }

    // Now that its `_` are being applied, node compiles as normal. A `_` is always compiled while its own call is the
    // innermost being applied, as a nested call is a separate argument, so only this call's are needed.
    auto outer = std::exchange(expr.applied_curries, curries);
    ir.append(body, expr(node).ir_node);
    expr.applied_curries = std::move(outer);
    expr.scope.pop_back();
    // It has arguments, so what calling it returns isn't known.
    return { function };
}

// A `_` being partially applied reads its argument, any other isn't in a call.
Return<SCAnyExpr_I> Expr::operator()(ast::CurryIndex i) {
    const auto is_i = [&](ast::CurryIndex c) { return *c == *i; };
    if (const auto fnd = std::find_if(applied_curries.begin(), applied_curries.end(), is_i);
        fnd != applied_curries.end()) {
        const auto name = curry_argument_name(ctx, fnd - applied_curries.begin(), applied_curries.size());
        const auto entry = resolve_symbol(name);
        assert(entry);
        return { ir.create(SCRead { name, SCRead::Kind::Local, entry->local }, location({ *i })) };
    }
    sink(sc::diag::Diagnostic::curry_outside_call(get_text_info(*ast_graph), get_location(*ast_graph, i)));
    return { ir.create(SCExprList {}, location({ *i })) };
}

}
