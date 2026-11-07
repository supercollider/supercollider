/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#pragma once

#include "SC_Types.h"

#ifdef __cplusplus
#    include <cstring>
#    include <type_traits>
#endif

#if defined(__APPLE__)

#    include <machine/endian.h>

#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)

#    include <sys/types.h>

#elif defined(_WIN32)

#    define LITTLE_ENDIAN 1234
#    define BIG_ENDIAN 4321
#    define BYTE_ORDER LITTLE_ENDIAN

#elif defined(__linux__) || defined(__EMSCRIPTEN__)

#    include <endian.h>

#else

#    error cannot find endianess on this platform

#endif

#ifndef BYTE_ORDER
#    error BYTE_ORDER undefined, check __FILE__
#endif // BYTE_ORDER

#ifndef BIG_ENDIAN
#    error BIG_ENDIAN undefined, check __FILE__
#endif // BIG_ENDIAN

#ifndef LITTLE_ENDIAN
#    error LITTLE_ENDIAN undefined, check __FILE__
#endif // LITTLE_ENDIAN

#ifdef __cplusplus

namespace sc {

/** @brief C++11 compatible version of C++20's std::endian.
 *
 *  @example
 *      if constexpr(sc::endian::native == sc::endian::big) {
 *          // big endian code
 *      } else {
 *          // little endian code
 *      }
 *
 *  @note Always prefer sc::endian over BYTE_ORDER comparisons because
 *  the preprocessor silently evaluates undefined macros as false!
 */
enum class endian { little = LITTLE_ENDIAN, big = BIG_ENDIAN, native = BYTE_ORDER };

// Rule out mixed endianess. This way users only need to check for either
// endian::big or endian::little, but not for both. In practice, there are
// no mixed endianess platforms that would run SuperCollider.
static_assert(endian::native == endian::big || endian::native == endian::little,
              "mixed endianess platforms not supported");

/** @brief a C++11 compatible version of C++23's std::byteswap. */
template <typename T> T byteswap(T x) {
    static_assert(std::is_integral<T>::value, "byteswap() requires an integral type");
    constexpr auto size = sizeof(T);
    char temp[size];
    auto bytes = reinterpret_cast<const char*>(&x);
    for (size_t i = 0; i < size; ++i) {
        temp[i] = bytes[size - i - 1];
    }
    T result;
    std::memcpy(&result, temp, size);
    return result;
}

} // namespace sc

#endif // __cplusplus

/////////////////////////////////////////////////////////////////////////////

/** @brief takes a 16-bit unsigned integer in network byte order (big endian)
 *  and returns the value in host byte order.
 */
SC_INLINE uint16_t sc_htons(uint16_t x) {
    // Note: this is a C function, so we cannot use sc::endian or sc::byteswap.
#if BYTE_ORDER == LITTLE_ENDIAN
    unsigned char* c = (unsigned char*)&x;
    return (uint16_t)c[0] << 8 | (uint16_t)c[1];
#else
    return x;
#endif
}

/** @brief takes a 32-bit unsigned integer in network byte order (big endian)
 *  and returns the value in host byte order.
 */
SC_INLINE uint32_t sc_htonl(uint32_t x) {
    // Note: this is a C function, so we cannot use sc::endian or sc::byteswap.
#if BYTE_ORDER == LITTLE_ENDIAN
    unsigned char* c = (unsigned char*)&x;
    return (uint32_t)c[0] << 24 | (uint32_t)c[1] << 16 | (uint32_t)c[2] << 8 | c[3];
#else
    return x;
#endif
}

/** @brief takes a 64-bit unsigned integer in network byte order (big endian)
 *  and returns the value in host byte order.
 */
SC_INLINE uint64_t sc_htonll(uint64_t x) {
    // Note: this is a C function, so we cannot use sc::endian or sc::byteswap.
#if BYTE_ORDER == LITTLE_ENDIAN
    unsigned char* c = (unsigned char*)&x;
    return (uint64_t)c[0] << 56 | (uint64_t)c[1] << 48 | (uint64_t)c[2] << 40 | (uint64_t)c[3] << 32
        | (uint64_t)c[4] << 24 | (uint64_t)c[5] << 16 | (uint64_t)c[6] << 8 | c[7];
#else
    return x;
#endif
}

/** @brief takes a 16-bit unsigned integer in host byte order and returns
 *  the value in network byte order (big endian).
 */
SC_INLINE uint32_t sc_ntohl(uint32_t x) { return sc_htonl(x); }

/** @brief takes a 32-bit unsigned integer in host byte order and returns
 *  the value in network byte order (big endian).
 */
SC_INLINE uint16_t sc_ntohs(uint16_t x) { return sc_htons(x); }

/** @brief takes a 64-bit unsigned integer in host byte order and returns
 *  the value in network byte order (big endian).
 */
SC_INLINE uint64_t sc_ntohll(uint64_t x) { return sc_htonll(x); }
