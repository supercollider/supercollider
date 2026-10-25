#pragma once

#include "indexes_untyped.hpp"
#include "sc_source/text_location.hpp"
#include <memory>
#include <string_view>

namespace sc::src {
struct TextInfo;
}
namespace sc::ast {
struct ASTGraph;

// including the whole ast graph is slow (particularly for clangd), this provides some type erasure.

[[nodiscard]] std::shared_ptr<const sc::src::TextInfo> get_text_info(const ASTGraph&);

[[nodiscard]] sc::src::SourceCodeRange get_location(const ASTGraph&, OptionalIndex);
[[nodiscard]] std::string_view get_string_view(const ASTGraph&, OptionalIndex);
[[nodiscard]] std::string get_string(const ASTGraph&, OptionalIndex);

[[nodiscard]] OptionalIndex next_sibling(const ASTGraph&, OptionalIndex i);


}