#pragma once

#include "sc_util/strong_index.hpp"
#include <cstdint>

namespace sc::ir{

using ConstExprIndex = sc::util::StrongIndex<std::uint32_t, struct ConstantExpressionIndex__>;
using OptionalConstExprIndex = sc::util::StrongOptionalIndex<std::uint32_t, struct ConstantExpressionIndex__,
                                                             std::numeric_limits<std::uint32_t>::max()>;
}