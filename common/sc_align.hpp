/*
 * alignment helper functions
 *
 * Copyright (c) 2026 Christof Ressi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#pragma once

#include <cassert>
#include <cstddef>

namespace sc {

namespace detail {

[[nodiscard]] inline constexpr bool is_alignment(size_t alignment) noexcept {
    return alignment > 0 && (alignment & (alignment - 1)) == 0;
}

} // detail

[[nodiscard]] inline bool is_aligned(size_t value, size_t alignment) noexcept {
    assert(detail::is_alignment(alignment));
    return (value & (alignment - 1)) == 0;
}

[[nodiscard]] inline bool is_aligned(void* ptr, size_t alignment) noexcept {
    return is_aligned(reinterpret_cast<std::size_t>(ptr), alignment);
}

template <typename T> [[nodiscard]] inline bool is_aligned(T* ptr) noexcept {
    return is_aligned(reinterpret_cast<std::size_t>(ptr), alignof(T));
}

[[nodiscard]] inline size_t align_up(size_t value, size_t alignment) noexcept {
    assert(detail::is_alignment(alignment));
    const auto mask = alignment - 1;
    return (value + mask) & ~mask;
}

[[nodiscard]] inline void* align_up(void* ptr, size_t alignment) noexcept {
    return reinterpret_cast<void*>(align_up(reinterpret_cast<size_t>(ptr), alignment));
}

template <typename T> [[nodiscard]] inline T* align_up(T* ptr, size_t alignment = alignof(T)) noexcept {
    return reinterpret_cast<T*>(align_up(reinterpret_cast<size_t>(ptr), alignment));
}

} // sc
