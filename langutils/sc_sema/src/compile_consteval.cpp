#include "ast2ir/consteval.hpp"
#include "sc_parser/ast.hpp"
#include "sc_sema/compiler.hpp"
#include "sc_sema/compiler_consteval.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/symbol_table.hpp"

namespace sc::ir {

std::variant<ConstExpression, sc::diag::Diagnostic>
compile_consteval(std::shared_ptr<const ast::ASTGraph> ast_graph, ast::AnyExprIndex expr_i, SymbolTable& sym_table) {
    auto common_c = symbol_table_and_common_classes(sym_table, sym_table.get_class_name("AbstractObject"));
    std::vector<std::optional<SourceEntry>> class_lib {};

    ModuleTable mod_table {};

    Context context { sym_table, std::move(common_c), mod_table, class_lib };

    using R = std::variant<ConstExpression, sc::diag::Diagnostic>;
    auto r = ast2ir::ConstEval { context, ast_graph, ASTGraphIndex { 0 } }.eval(expr_i);

    return std::move(r).visit([&](ast2ir::ConstEval::Success s) -> R { return { context.constant_table(s.first) }; },
                              [&](sc::diag::Diagnostic d) -> R { return { std::move(d) }; });
}

}