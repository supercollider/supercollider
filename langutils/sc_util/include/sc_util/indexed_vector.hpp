#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

namespace sc::util {

// A vector that can only be indexed by Index, e.g., a StrongIndex or a TypedIndex, so each access is type checked.
// A typed index converts to a join that includes it, so a vector by a join is indexed by any of its types.
template <typename Index, typename T> class IndexedVector {
public:
    [[nodiscard]] T& operator[](Index i) { return m_values[*i]; }
    [[nodiscard]] const T& operator[](Index i) const { return m_values[*i]; }
    [[nodiscard]] bool contains(Index i) const { return *i < m_values.size(); }

    // Returns the new value's index.
    Index push_back(T value) {
        m_values.push_back(std::move(value));
        return Index { static_cast<decltype(*std::declval<Index>())>(m_values.size() - 1) };
    }

    // Grows up to i, so we can assign a value for i.
    void grow_to_hold(Index i) {
        static_assert(std::is_default_constructible_v<T>, "grow_to_hold fills with default values");
        if (!contains(i))
            m_values.resize(*i + 1);
    }

    void resize(std::size_t size) {
        static_assert(std::is_default_constructible_v<T>, "resize fills with default values");
        m_values.resize(size);
    }

    [[nodiscard]] std::size_t size() const { return m_values.size(); }

    // Only type check insertion, I don't want a full iterator interface here.
    [[nodiscard]] std::vector<T>& raw() { return m_values; }
    [[nodiscard]] const std::vector<T>& raw() const { return m_values; }

private:
    std::vector<T> m_values {};
};

}
