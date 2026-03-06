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
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <tuple>


typedef uint8_t Byte;
static_assert(sizeof(Byte) == 1, "Byte must be a byte.");
struct LocationInSourceCode : std::tuple<uint32_t, uint32_t> {
    // Start and end index in source code.
    using std::tuple<uint32_t, uint32_t>::tuple;

    [[nodiscard]] constexpr uint32_t& start() { return std::get<0>(*this); }
    [[nodiscard]] constexpr uint32_t& end() { return std::get<1>(*this); }
    [[nodiscard]] constexpr uint32_t start() const { return std::get<0>(*this); }
    [[nodiscard]] constexpr uint32_t end() const { return std::get<1>(*this); }
};

class CompilingBytecodes {
public:
    CompilingBytecodes() = default;
    ~CompilingBytecodes() = default;

    // No copies.
    CompilingBytecodes(const CompilingBytecodes&&) = delete;
    CompilingBytecodes& operator=(const CompilingBytecodes&&) = delete;

    // Moves only. Used to store a temporary.
    CompilingBytecodes(CompilingBytecodes&&) = default;
    CompilingBytecodes& operator=(CompilingBytecodes&&) = default;

    void consume(CompilingBytecodes&& other) {
        data.assertValid();
        other.data.assertValid();
        data.codes.insert(data.codes.end(), other.data.codes.begin(), other.data.codes.end());
        data.startAndEndLocations.insert(data.startAndEndLocations.end(), other.data.startAndEndLocations.begin(),
                                         other.data.startAndEndLocations.end());
        data.sizeOfCodes.insert(data.sizeOfCodes.end(), other.data.sizeOfCodes.begin(), other.data.sizeOfCodes.end());
        data.assertValid();
    }

    template <class Opcode> [[nodiscard]] bool previousInstructionWas(const Opcode& opcode) const {
        const auto size = opcode.byteSize;
        if (data.sizeOfCodes.empty())
            return false;
        if (data.sizeOfCodes.back() != size)
            return false;
        return data.codes[data.codes.size() - data.sizeOfCodes.back()] == opcode.code;
    }

    void erasePreviousInstruction() {
        assert(!data.sizeOfCodes.empty());
        const auto size = data.sizeOfCodes.back();
        data.startAndEndLocations.erase(data.startAndEndLocations.end() - 2, data.startAndEndLocations.end());
        data.codes.erase(data.codes.end() - size, data.codes.end());
        data.sizeOfCodes.pop_back();
        data.assertValid();
    }

    template <typename... BYTES> void emit(LocationInSourceCode loc, BYTES... bytes) {
        static_assert(sizeof...(BYTES) > 0);
        data.startAndEndLocations.push_back(std::get<0>(loc));
        data.startAndEndLocations.push_back(std::get<1>(loc));
        (data.codes.push_back(static_cast<Byte>(bytes)), ...);
        data.sizeOfCodes.push_back(static_cast<uint8>(sizeof...(bytes)));
        data.assertValid();
    }

    [[nodiscard]] size_t length() const noexcept { return data.codes.size(); }

    struct Data {
        std::vector<Byte> codes;
        std::vector<uint32_t> startAndEndLocations; // location is source text. Twice as large as the codes vector.
        std::vector<uint8_t> sizeOfCodes; // codes can be variable width.

        void assertValid() const {
            // The sum of the sizeOfCodes should be equal to the size of codes.
            assert(codes.size() >= sizeOfCodes.size());
            assert(sizeOfCodes.size() * 2 == startAndEndLocations.size());
        }
    };

    // Returns data and sets the held data to empty as per the move constructor (not assignment).
    // Note the r value qualifier.
    [[nodiscard]] Data finishGetData() && noexcept {
        // Validate data is good here.
        assert(!data.codes.empty());
        assert(!data.startAndEndLocations.empty());
        assert(!data.sizeOfCodes.empty());
        assert(data.startAndEndLocations.size() == data.sizeOfCodes.size() * 2); // twice as many.
        Data out { std::move(data) }; // this empties this->data.
        // But I'm writing this as asserts because move semantics aren't always obvious.
        assert(data.codes.empty());
        assert(data.startAndEndLocations.empty());
        assert(data.sizeOfCodes.empty());

        return out;
    }

    void backsetByte(size_t index, Byte newValue) {
        assert(index < data.codes.size());
        data.codes[index] = newValue;
    }

    void assertEmpty() const {
        assert(data.codes.empty());
        assert(data.startAndEndLocations.empty());
        assert(data.sizeOfCodes.empty());
    }

private:
    Data data;
};

extern CompilingBytecodes gCompilingBytecodes;
