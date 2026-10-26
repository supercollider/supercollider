#pragma once

#include <cassert>
#include <type_traits>

namespace sc::util {

// An integer converted to a narrower or differently signed type, asserting the value is unchanged, as std::in_range
// from C++20 checks.
template <typename To, typename From> [[nodiscard]] constexpr To narrow(From value) noexcept {
    static_assert(std::is_integral_v<To> && std::is_integral_v<From>);
    const auto result = static_cast<To>(value);
    assert(static_cast<From>(result) == value && (result < To {}) == (value < From {}));
    return result;
}

}
