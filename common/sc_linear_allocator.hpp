/*
 * linear memory allocator (aka bump allocator)
 *
 * Copyright (C) 2005-2012  Tim Blechmann
 * Copyright (C) 2026       Christof Ressi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#pragma once

#include "sc_align.hpp"

#include <cassert>
#include <cstddef>

namespace sc {

//! @brief a linear memory allocator (also known as bump allocator)
struct linear_allocator {
    linear_allocator(char* chunk, char* end): chunk(chunk), end(end) {}

    //! @brief allocate memory for a number of objects with the given type.
    //! @return a suitably aligned pointer on success, nullptr on failure.
    template <typename T> [[nodiscard]] T* alloc(size_t count = 1) noexcept {
        // make sure that the memory is suitable aligned for the given type
        return alloc_aligned<T>(count, alignof(T));
    }

    //! @brief allocate memory for a number of objects with the given type and alignment
    //! @return an aligned pointer on success, nullptr on failure.
    template <typename T> [[nodiscard]] T* alloc_aligned(size_t count, size_t alignment) noexcept {
        chunk = sc::align_up(chunk, alignment);
        T* ret = reinterpret_cast<T*>(chunk);
        auto next = chunk + count * sizeof(T);
        if (next > end)
            return nullptr;
        chunk = next;
        return ret;
    }

    //! @brief allocate memory for a single object with the given type and alignment
    //! @return an aligned pointer on success, nullptr on failure.
    template <typename T> [[nodiscard]] T* alloc_aligned(size_t alignment) noexcept {
        return alloc_aligned<T>(1, alignment);
    }

    //! @brief query the available memory
    [[nodiscard]] size_t available_memory() const noexcept {
        assert(check_bounds());
        return end - chunk;
    }

    //! @brief check whether the allocator is still within its bounds,
    //! i.e. it has not allocated past the end of the buffer.
    [[nodiscard]] bool check_bounds() const noexcept { return chunk <= end; }

private:
    char* chunk;
    char* end;
};

} // namespace sc
