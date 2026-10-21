#pragma once

#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/nodes.hpp"
#include "sc_parser/sc_grammar_shared.hpp"
#include <string_view>

namespace sc::ast {
struct ASTGraph;
}

namespace sc::ir {


[[nodiscard]] int synthesize_literal(const ast::IntNode& node, std::string_view str);


[[nodiscard]] char synthesize_literal(const ast::ASCIINode&, std::string_view str);

[[nodiscard]] double synthesize_literal(const ast::FloatNode& node, std::string_view str);


[[nodiscard]] std::string synthesize_literal(const ast::SymbolQuote&, std::string_view str);

[[nodiscard]] std::string_view synthesize_literal(const ast::SymbolSlash&, std::string_view str);

[[nodiscard]] std::string_view synthesize_literal(const ast::KeyBinOp&, std::string_view str);

[[nodiscard]] std::string synthesize_literal(ast::StringLitIndex i, const ast::ASTGraph& ast_graph);

[[nodiscard]] bool synthesize_literal(const ast::BooleanNode&, std::string_view str);


[[nodiscard]] ast::ReadWriteAccessor synthesize_literal(const ast::MaybeRWAccessorIndex i, const ast::ASTGraph& g);
}
