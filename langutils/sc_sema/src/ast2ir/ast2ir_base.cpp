#include "ast2ir/ast2ir_base.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_sema/ir.hpp"

namespace sc::ir::ast2ir {

SourceLocation location_of(IRGraph& ir, const ast::ASTGraph& ast_graph, ast::OptionalIndex i) {
    return ir.make_location(get_text_info(ast_graph), get_location(ast_graph, i));
}

}
