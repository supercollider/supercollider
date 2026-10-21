#pragma once
#include "indexes_typed.hpp"
#include "sc_parser/nodes.hpp"
#include "sc_util/typed_graph_iter.hpp"

namespace sc::ast {
struct ASTGraph;

[[nodiscard]] AnyExprIndex::Variant to_variant(const ASTGraph&, AnyExprIndex);
[[nodiscard]] BlockItemIndex::Variant to_variant(const ASTGraph&, BlockItemIndex);


// If you have encountered this, you need to an another overload above with the specific type.
template <typename T> AnyExprIndex::Variant to_variant(const ASTGraph&, T) = delete;


[[nodiscard]] const IntNode& payload(const ASTGraph&, IntLitIndex);
[[nodiscard]] const FloatNode& payload(const ASTGraph&, FloatLitIndex);


[[nodiscard]] Iter<OptionalIndex, AnyExprIndex> iter(const ASTGraph&, ExprSeqIndex);

[[nodiscard]] AnyExprIndex child(const ASTGraph&, NonLocalReturnExpr);

}