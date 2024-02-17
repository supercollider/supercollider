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

#include <cstring>
#include <tuple>
#include <limits>
#include "SC_Types.h"
#include "SC_Endian.h"

// TODO: could this be made unsigned, integer overflow is only implementation defined on signed types?
using hash_t = int32;


// bit cast from c++20, copied from cpp-ref, used to remove the c cast, which is implementation defined.
template <class To, class From>
constexpr inline
std::enable_if_t<sizeof(To) == sizeof(From) && std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>,
    To>
bit_cast(const From& src) noexcept {
    static_assert(std::is_trivially_constructible_v<To>,
                  "This implementation additionally requires "
                  "destination type to be trivially constructible");

    To dst;
    memcpy(&dst, &src, sizeof(To));
    return dst;
}


// These hash functions are among the best there are in terms of both speed and quality.
// A good hash function makes a lot of difference.
// I have not used Bob Jenkins own hash function because the keys I use are relatively short.


// hash function for a string
constexpr inline hash_t Hash(const char* key) {
// the one-at-a-time hash.
// a very good hash function. ref: a web page by Bob Jenkins.
// http://www.burtleburtle.net/bob/hash/doobs.html
    hash_t hash = 0;
    while (*key) {
        hash += *key++;
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

// hash function for a string that also returns the length
constexpr inline hash_t Hash(const char* key, size_t* outLength) {
// the one-at-a-time hash.
// a very good hash function. ref: a web page by Bob Jenkins.
    const char* startKey = key;
    hash_t hash = 0;
    while (*key) {
        hash += *key++;
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
// pedantic but possible
    static_assert(std::numeric_limits<size_t>::max() >= std::numeric_limits<decltype(key - startKey)>::max());
    *outLength = static_cast<size_t>(key - startKey);
    return hash;
}

constexpr inline std::tuple<hash_t, size_t> HashWithSize(const char* in) {
    size_t size = 0;
    hash_t h = Hash(in, &size);
    return { h, size };
}

// hash function for an array of char
constexpr inline hash_t Hash(const char* key, uint32 inLength) {
// the one-at-a-time hash.
// a very good hash function. ref: a web page by Bob Jenkins.
    hash_t hash = 0;
    for (decltype(inLength) i = 0; i < inLength; ++i) {
        hash += *key++;
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

// hash function for integers
constexpr inline hash_t Hash(int32 inKey) {
// Thomas Wang's integer hash.
// http://www.concentric.net/~Ttwang/tech/inthash.htm
// a faster hash for integers. also very good.
    auto hash = bit_cast<uint32>(inKey);
    hash += ~(hash << 15);
    hash ^= hash >> 10;
    hash += hash << 3;
    hash ^= hash >> 6;
    hash += ~(hash << 11);
    hash ^= hash >> 16;
    return (hash_t)hash;
}

// this function isn't used in the vm (where hash_t is int32), but might be used elsewhere
constexpr inline int64 Hash64(int64 inKey) {
// Thomas Wang's 64 bit integer hash.
    auto hash = bit_cast<uint64>(inKey);
    hash += ~(hash << 32);
    hash ^= (hash >> 22);
    hash += ~(hash << 13);
    hash ^= (hash >> 8);
    hash += (hash << 3);
    hash ^= (hash >> 15);
    hash += ~(hash << 27);
    hash ^= (hash >> 31);
    return (int64)hash;
}

constexpr inline hash_t Hash(const int32* inKey, uint32 inLength) {
// one-at-a-time hashing of a string of int32's.
// uses Thomas Wang's integer hash for the combining step.
    hash_t hash = 0;
    for (decltype(inLength) i = 0; i < inLength; ++i)
        hash = Hash(hash + *inKey++);
    return hash;
}

#if BYTE_ORDER == LITTLE_ENDIAN
const int32 kLASTCHAR = (int32)0xFF000000;
#else
const int32 kLASTCHAR = (int32)0x000000FF;
#endif

constexpr inline hash_t Hash(const int32* inKey) {
// hashing of a string of int32's.
// uses Thomas Wang's integer hash for the combining step.
    hash_t hash = 0;
    for (;;) {
        const int32 c = *inKey++;
        hash = Hash(hash + c);
        if (!(c & kLASTCHAR))
            break;
    }
    return hash;
}
