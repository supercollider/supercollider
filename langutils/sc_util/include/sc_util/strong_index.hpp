#pragma once

#include <cstddef>
#include <functional>
#include <limits>
#include <type_traits>

namespace sc::util {

template <typename UNDERLYING, typename TAG>//
 struct StrongIndex {
    using Tag = TAG;
    using Underlying = UNDERLYING;
    static_assert(std::is_integral_v<UNDERLYING>);

    template<typename T>
    [[nodiscard]] static constexpr StrongIndex from(T t) {
        return StrongIndex{static_cast<Underlying>(t)};
    }

    constexpr StrongIndex(Underlying u): m_value(u) {}
    constexpr StrongIndex(StrongIndex&&) noexcept = default;
    constexpr StrongIndex(const StrongIndex&) noexcept = default;
    constexpr StrongIndex& operator=(StrongIndex&&) noexcept = default;
    constexpr StrongIndex& operator=(const StrongIndex&) noexcept = default;

    [[nodiscard]] constexpr Underlying operator*() const { return m_value; }
    [[nodiscard]] constexpr Underlying value() const { return m_value; }

    [[nodiscard]] constexpr bool operator==(StrongIndex<UNDERLYING, TAG> other) const {
        return m_value == other.m_value;
    }

    [[nodiscard]] constexpr bool operator!=(StrongIndex<UNDERLYING, TAG> other) const { return !(*this == other); }

private:
    Underlying m_value {};
};

template<typename T>
[[nodiscard]] static constexpr T default_invalid() {
    static_assert(std::is_integral_v<T>);
    if constexpr(std::is_unsigned_v<T>){
        return std::numeric_limits<T>::max();
    } else {
        return -1;
    }
}

template <typename UNDERLYING, typename TAG, UNDERLYING INVALID_VALUE = default_invalid<UNDERLYING>()>//
 struct StrongOptionalIndex {
    using Tag = TAG;
    using Underlying = UNDERLYING;
    static_assert(std::is_integral_v<Underlying>);

    constexpr StrongOptionalIndex(): m_value(INVALID_VALUE) {}
    constexpr StrongOptionalIndex(Underlying u): m_value(u) {}
    constexpr StrongOptionalIndex(StrongIndex<UNDERLYING, TAG> u): m_value(*u) {}

    constexpr StrongOptionalIndex(const StrongOptionalIndex&) noexcept = default;
    constexpr StrongOptionalIndex(StrongOptionalIndex&&) noexcept = default;
    constexpr StrongOptionalIndex& operator=(const StrongOptionalIndex&) noexcept = default;
    constexpr StrongOptionalIndex& operator=(StrongOptionalIndex&&) noexcept = default;

    [[nodiscard]] constexpr Underlying operator*() const { return m_value; }
    [[nodiscard]] constexpr Underlying value() const { return m_value; }
    [[nodiscard]] constexpr operator bool() const { return m_value != INVALID_VALUE; }

    [[nodiscard]] constexpr bool operator==(StrongOptionalIndex<UNDERLYING, TAG, INVALID_VALUE> other) const {
        return m_value == other.m_value;
    }
    [[nodiscard]] constexpr bool operator!=(StrongOptionalIndex<UNDERLYING, TAG, INVALID_VALUE> other) const {
        return !(*this == other);
    }

private:
    Underlying m_value { INVALID_VALUE };
};

template <typename T, typename Y> [[nodiscard]] static constexpr bool is_a_pair() {
    if (std::is_same_v<T, Y>)
        return false;
    if (!std::is_same_v<typename T::Tag, typename Y::Tag>)
        return false;
    if (!std::is_same_v<typename T::Underlying, typename Y::Underlying>)
        return false;
    return std::is_convertible_v<T, bool> || std::is_convertible_v<Y, bool>;
}

}

namespace std {



template <typename UNDERLYING, typename TAG>//
struct hash<sc::util::StrongIndex<UNDERLYING, TAG>> {
    using T = sc::util::StrongIndex<UNDERLYING, TAG>;
    std::size_t operator()(const T& t) const { return std::hash<UNDERLYING> {}(*t); }
};

template <typename UNDERLYING, typename TAG, UNDERLYING INVALID>//
struct hash<sc::util::StrongOptionalIndex<UNDERLYING, TAG, INVALID>> {
    using T = sc::util::StrongOptionalIndex<UNDERLYING, TAG, INVALID>;
    std::size_t operator()(const T& t) const { return std::hash<UNDERLYING> {}(*t); }
};


}
