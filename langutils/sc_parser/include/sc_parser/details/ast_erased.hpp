// Copyright Jordan Henderson 2026
#pragma once

// Free functions that mirror ASTGraph members, so headers can use the graph without including it.
//
// Including the whole ast graph is slow (particularly for clangd), this provides some type erasure.

#include "indexes_typed.hpp"
#include "sc_source/text_location.hpp"
#include "sc_util/details/typed_graph_iter.hpp"
#include <memory>
#include <string>
#include <string_view>

namespace sc::src {
struct TextInfo;
}
namespace sc::ast {
struct ASTGraph;
struct IntNode;
struct FloatNode;
struct NonLocalReturnExpr;

[[nodiscard]] const std::shared_ptr<const sc::src::TextInfo>& get_text_info(const ASTGraph&);

[[nodiscard]] sc::src::SourceCodeRange get_location(const ASTGraph&, OptionalIndex);
[[nodiscard]] std::string_view get_string_view(const ASTGraph&, OptionalIndex);
[[nodiscard]] std::string get_string(const ASTGraph&, OptionalIndex);

[[nodiscard]] OptionalIndex next_sibling(const ASTGraph&, OptionalIndex i);

[[nodiscard]] AnyExprIndex::Variant to_variant(const ASTGraph&, AnyExprIndex);
[[nodiscard]] BlockItemIndex::Variant to_variant(const ASTGraph&, BlockItemIndex);

// If you have encountered this, add an overload above with the specific type.
template <typename T> AnyExprIndex::Variant to_variant(const ASTGraph&, T) = delete;

[[nodiscard]] const IntNode& payload(const ASTGraph&, IntLitIndex);
[[nodiscard]] const FloatNode& payload(const ASTGraph&, FloatLitIndex);

[[nodiscard]] util::typed_graph::Iter<OptionalIndex, AnyExprIndex> iter(const ASTGraph&, ExprSeqIndex);

[[nodiscard]] AnyExprIndex child(const ASTGraph&, NonLocalReturnExpr);
}
