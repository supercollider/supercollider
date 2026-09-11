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
#include "text_location.hpp"
#include <cassert>
#include <cstdint>
#include <limits>
#include <vector>


typedef unsigned char Byte;
class CompilingBytecodes {
public:
    using Location = sc::lex::SourceCodeRange;
    CompilingBytecodes() = default;
    ~CompilingBytecodes() = default;

    // No copies.
    CompilingBytecodes(const CompilingBytecodes&&) = delete;
    CompilingBytecodes& operator=(const CompilingBytecodes&&) = delete;

    // Moves only. Used to store a temporary.
    CompilingBytecodes(CompilingBytecodes&&) = default;
    CompilingBytecodes& operator=(CompilingBytecodes&&) = default;

    struct Data {
        std::vector<Byte> codes;
        std::vector<int> startAndEndLocations; // location in source text. Twice as large as the codes vector.
        std::vector<std::uint8_t> sizeOfCodes; // codes can be variable width.
    };

    void consume(CompilingBytecodes&& other);

    [[nodiscard]] size_t length() const noexcept;

    template <typename... BYTES> void emit(sc::lex::SourceCodeRange loc, BYTES... bytes) {
        static_assert(sizeof...(BYTES) > 0);
        static_assert(sizeof...(BYTES) <= std::numeric_limits<std::uint8_t>::max());
        // SCLang only support int type.
        assert(loc.begin.absolute <= std::numeric_limits<int>::max());
        assert(loc.end.absolute <= std::numeric_limits<int>::max());
        data.startAndEndLocations.push_back(loc.begin.absolute);
        data.startAndEndLocations.push_back(loc.end.absolute);
        (data.codes.push_back(static_cast<Byte>(bytes)), ...);
        data.sizeOfCodes.push_back(static_cast<std::uint8_t>(sizeof...(bytes)));
    }

    // Returns data and sets the held data to empty as per the move constructor (not assignment).
    // Note the r value qualifier.
    [[nodiscard]] Data finish() && noexcept;

    void backSetByte(size_t index, Byte newValue) noexcept;

    void assertEmpty() const noexcept;

private:
    Data data;
};
