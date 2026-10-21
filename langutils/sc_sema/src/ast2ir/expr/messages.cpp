// Messages. Every form is lowered to a single SCMessage, receiver.selector(positional, keyword: value, *variadic).
// Trailing blocks are appended to the positional arguments.
// Needs the ast graph for the children and payloads, and the symbol table for the selectors.
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
struct MessageContext {
    Expr& expr;
    Context& ctx;
    IRGraph& ir;
    const std::shared_ptr<const ast::ASTGraph>& ast_graph;
};

// The argument lists are created up front, so arguments are appended to them as they are compiled.
struct MessageArguments {
    SCPosArgList_I positional;
    SCKwArgList_I keyword;
    SCVariadicArgList_I variadic;
    // Functional notation, e.g., `foo(a, b)`, where the first positional argument is the receiver.
    bool first_positional_is_receiver;
    std::optional<SCAnyExpr_I> receiver {};
};

MessageArguments make_arguments(MessageContext& msg_ctx, ast::OptionalIndex message_i,
                                bool first_positional_is_receiver = false) {
    const ASTLocation location { msg_ctx.ast_graph, { *message_i } };
    return { msg_ctx.ir.create(SCPositionalArgs {}, location), msg_ctx.ir.create(SCKwArgs {}, location),
             msg_ctx.ir.create(SCVariadicArgs {}, location), first_positional_is_receiver };
}

void append_positional(MessageContext& msg_ctx, MessageArguments& args, SCAnyExpr_I arg) {
    if (args.first_positional_is_receiver && !args.receiver) {
        args.receiver = arg;
        return;
    }
    msg_ctx.ir.append(args.positional, arg);
    ++msg_ctx.ir.payload(args.positional).size;
}

struct Selector {
    OptionallyLocatable<SymSCSelector> name;
    std::optional<SCAnyExpr_I> adverb;
};

void append_blocks(MessageContext& msg_ctx, MessageArguments& args, ast::BlockListIndex i) {
    for (auto it = msg_ctx.ast_graph->children(i).iter(); it; ++it)
        append_positional(msg_ctx, args, msg_ctx.expr(ast::AnyExprIndex { *it }).ir_node);
}

void append_arguments(MessageContext& msg_ctx, MessageArguments& args, ast::ArgumentListIndex i) {
    const auto& g = *msg_ctx.ast_graph;
    for (auto it = g.children(i).iter(); it; ++it) {
        const ast::ArgumentEntryIndex entry = *it;
        if (const auto kw = g.as_a<ast::KwArgIndex>(entry)) {
            const auto [keyword, value] = g.children(*kw);
            const auto name = msg_ctx.ctx.symbol_table.get_identifier(
                std::string { synthesize_literal(g.payload(keyword), get_string_view(g, keyword)) });
            const auto value_r = msg_ctx.expr(ast::AnyExprIndex { value });
            const Locatable<SymSCIdentifier> located_name { { msg_ctx.ast_graph, { *keyword } }, name };
            msg_ctx.ir.append(
                args.keyword,
                msg_ctx.ir.create(SCKwArg { located_name }, { msg_ctx.ast_graph, { *entry } }, value_r.ir_node));
        } else if (const auto variadic = g.as_a<ast::VariadicArgIndex>(entry)) {
            const auto [expr] = g.children(*variadic);
            msg_ctx.ir.append(args.variadic, msg_ctx.expr(ast::AnyExprIndex { expr }).ir_node);
        } else if (const auto blocks = g.as_a<ast::BlockListIndex>(entry)) {
            append_blocks(msg_ctx, args, *blocks);
        } else {
            append_positional(msg_ctx, args, msg_ctx.expr(ast::AnyExprIndex { *entry }).ir_node);
        }
    }
}

SCAnyExpr_I adverb(MessageContext& msg_ctx, ast::AdverbIndex i) {
    const auto& g = *msg_ctx.ast_graph;
    if (const auto e = g.as_a<ast::AdverbExprIndex>(i)) {
        const auto [expr] = g.children(*e);
        return msg_ctx.expr(ast::AnyExprIndex { expr }).ir_node;
    }
    if (const auto n = g.as_a<ast::IntLitIndex>(i))
        return msg_ctx.expr(*n).ir_node;

    // A name, e.g., `a +.x b`, is a symbol.
    const auto sym = msg_ctx.ctx.symbol_table.get_symbol(std::string { get_string_view(g, i) });
    const auto const_i = msg_ctx.ctx.constant_table.register_constexpr({ sym });
    const auto lit = msg_ctx.ir.create(SCLiteral { const_i }, { msg_ctx.ast_graph, { *i } });
    msg_ctx.ir.register_node_as_consteval(lit, const_i);
    return lit;
}

Selector selector(MessageContext& msg_ctx, ast::SelectorMaybeAdverbIndex i) {
    const auto& g = *msg_ctx.ast_graph;
    std::optional<SCAnyExpr_I> adverb_i;
    const ast::SelectorIndex name_i = [&]() -> ast::SelectorIndex {
        const auto with_adverb = g.as_a<ast::SelectorWAdverbIndex>(i);
        if (!with_adverb)
            return ast::SelectorIndex { *i };
        const auto [name, adv] = g.children(*with_adverb);
        adverb_i = adverb(msg_ctx, adv);
        return name;
    }();

    auto name = std::string { get_string_view(g, name_i) };
    // Infix keywords, e.g., `a max: b`, include the colon.
    using SelectorLiteralIndex = ast::TypedIndex<ast::NodeFlag::SelectorLiteral>;
    if (const auto literal_i = g.as_a<SelectorLiteralIndex>(name_i);
        literal_i && g.payload(*literal_i).is_infix_keyword) {
        assert(name.back() == ':');
        name.pop_back();
    }

    return { { ASTLocation { msg_ctx.ast_graph, { *name_i } }, msg_ctx.ctx.symbol_table.get_selector(std::move(name)) },
             adverb_i };
}

// For messages whose selector isn't written, e.g., `a.()` is `a.value()`.
Selector implicit_selector(MessageContext& msg_ctx, const char* name) {
    return { { std::nullopt, msg_ctx.ctx.symbol_table.get_selector(name) }, std::nullopt };
}

Return<SCAnyExpr_I> create_message(MessageContext& msg_ctx, ast::OptionalIndex message_i, SCAnyExpr_I receiver,
                                   const Selector& selector, const MessageArguments& args) {
    const ASTLocation location { msg_ctx.ast_graph, { *message_i } };
    const maybe<SCAnyExpr_I> adverb_i = selector.adverb
        ? maybe<SCAnyExpr_I> { *selector.adverb }
        : maybe<SCAnyExpr_I> { msg_ctx.ir.create(SCMissing {}, location) };

    return { msg_ctx.ir.create(SCMessage { selector.name }, location, receiver, args.positional, args.keyword,
                               args.variadic, adverb_i) };
}

}

Return<SCAnyExpr_I> Expr::operator()(ast::PostfixMessageIndex i) {
    MessageContext msg_ctx { *this, ctx, ir, ast_graph };
    const auto [receiver, selector_i, arg_list, blocks] = ast_graph->children(i);
    const auto receiver_r = msg_ctx.expr(ast::AnyExprIndex { receiver });
    const auto sel = selector(msg_ctx, selector_i);
    auto args = make_arguments(msg_ctx, i);
    append_arguments(msg_ctx, args, arg_list);
    append_blocks(msg_ctx, args, blocks);
    return create_message(msg_ctx, i, receiver_r.ir_node, sel, args);
}

Return<SCAnyExpr_I> Expr::operator()(ast::PrefixMessageIndex i) {
    MessageContext msg_ctx { *this, ctx, ir, ast_graph };
    const auto [selector_i, arg_list, blocks] = ast_graph->children(i);
    const auto sel = selector(msg_ctx, selector_i);
    // Functional notation, `foo(a, b)` is `a.foo(b)`.
    auto args = make_arguments(msg_ctx, i, true);
    append_arguments(msg_ctx, args, arg_list);
    append_blocks(msg_ctx, args, blocks);

    if (!args.receiver) {
        ctx.diagnostics(
            sc::diag::Diagnostic::message_without_receiver(get_text_info(*ast_graph), get_location(*ast_graph, i)));
        return { ir.create(SCExprList {}, { ast_graph, { *i } }) };
    }
    return create_message(msg_ctx, i, *args.receiver, sel, args);
}

Return<SCAnyExpr_I> Expr::operator()(ast::BinaryMessageIndex i) {
    MessageContext msg_ctx { *this, ctx, ir, ast_graph };
    const auto [selector_i, lhs, rhs] = ast_graph->children(i);
    const auto lhs_r = msg_ctx.expr(ast::AnyExprIndex { lhs });
    const auto rhs_r = msg_ctx.expr(ast::AnyExprIndex { rhs });
    auto args = make_arguments(msg_ctx, i);
    append_positional(msg_ctx, args, rhs_r.ir_node);
    return create_message(msg_ctx, i, lhs_r.ir_node, selector(msg_ctx, selector_i), args);
}

Return<SCAnyExpr_I> Expr::operator()(ast::ValueMessageIndex i) {
    MessageContext msg_ctx { *this, ctx, ir, ast_graph };
    const auto [receiver, arg_list, blocks] = ast_graph->children(i);
    const auto receiver_r = (*this)(ast::AnyExprIndex { receiver });
    auto args = make_arguments(msg_ctx, i);
    append_arguments(msg_ctx, args, arg_list);
    append_blocks(msg_ctx, args, blocks);
    return create_message(msg_ctx, i, receiver_r.ir_node, implicit_selector(msg_ctx, "value"), args);
}

Return<SCAnyExpr_I> Expr::operator()(ast::NewMessageIndex i) {
    MessageContext msg_ctx { *this, ctx, ir, ast_graph };
    const auto [class_name, arg_list, blocks] = ast_graph->children(i);
    const auto receiver_r = (*this)(class_name);
    auto args = make_arguments(msg_ctx, i);
    append_arguments(msg_ctx, args, arg_list);
    append_blocks(msg_ctx, args, blocks);
    return create_message(msg_ctx, i, receiver_r.ir_node, implicit_selector(msg_ctx, "new"), args);
}

Return<SCAnyExpr_I> Expr::operator()(ast::AtMessageIndex i) {
    MessageContext msg_ctx { *this, ctx, ir, ast_graph };
    const auto [receiver, arg] = ast_graph->children(i);
    const auto receiver_r = (*this)(ast::AnyExprIndex { receiver });

    // e.g., `a[1..3]`, this is a different message, and ranges aren't implemented yet.
    if (const auto range = ast_graph->as_a<ast::RangeIndex>(arg)) {
        ctx.diagnostics(sc::diag::Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, *range)));
        return { ir.create(SCExprList {}, { ast_graph, { *i } }) };
    }

    auto args = make_arguments(msg_ctx, i);
    append_arguments(msg_ctx, args, ast::ArgumentListIndex { *arg });
    return create_message(msg_ctx, i, receiver_r.ir_node, implicit_selector(msg_ctx, "at"), args);
}

}
