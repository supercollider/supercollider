#pragma once

#include <vector>

template <typename OptIndex> struct EdgesTemplate { OptIndex first_child, parent, next_sibling, prev_sibling; };

template <typename OptNodeIndex, typename T> //
struct Iter {
    using Edges = EdgesTemplate<OptNodeIndex>;
    constexpr Iter(OptNodeIndex current, const std::vector<Edges>& edge): m_current(current), m_edges(edge) {}

    Iter& operator+=(int offset);
    [[nodiscard]] Iter operator++(int);
    Iter& operator++() { return this->operator+=(1); }
    [[nodiscard]] constexpr explicit operator bool() const { return static_cast<bool>(m_current); }

    [[nodiscard]] constexpr T operator*() const;

private:
    OptNodeIndex m_current;
    const std::vector<Edges>& m_edges;
};

template <typename OptNodeIndex, typename T> //
struct Container {
    using Edges = EdgesTemplate<OptNodeIndex>;
    constexpr Container(const std::vector<Edges>& edges, OptNodeIndex first): m_edges(edges), m_first(first) {}
    [[nodiscard]] Iter<OptNodeIndex, T> iter() const { return { m_first, m_edges }; }

private:
    const std::vector<Edges>& m_edges;
    OptNodeIndex m_first;
};

template <typename OptNodeIndex, typename T> Iter<OptNodeIndex, T>& Iter<OptNodeIndex, T>::operator+=(int offset) {
    if (offset >= 0) {
        for (int i { 0 }; i < offset; ++i) {
            if (!m_current)
                break;
            m_current = m_edges[*m_current].next_sibling;
        }
    } else {
        for (int i { 0 }; i > offset; --i) {
            if (!m_current)
                break;
            m_current = m_edges[*m_current].prev_sibling;
        }
    }
    return *this;
}


template <typename OptNodeIndex, typename T>
[[nodiscard]] Iter<OptNodeIndex, T> Iter<OptNodeIndex, T>::operator++(int) {
    Iter copy = *this;
    if (m_current) {
        m_current = m_edges[*m_current].next_sibling;
    }
    return copy;
}

template <typename OptNodeIndex, typename T> [[nodiscard]] constexpr T Iter<OptNodeIndex, T>::operator*() const {
    // assert(m_current);
    return T { *m_current };
}