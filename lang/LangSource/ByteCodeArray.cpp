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

#include "ByteCodeArray.h"

void CompilingBytecodes::consume(CompilingBytecodes&& other) {
    data.codes.insert(data.codes.end(), other.data.codes.begin(), other.data.codes.end());
    data.startAndEndLocations.insert(data.startAndEndLocations.end(), other.data.startAndEndLocations.begin(),
                                     other.data.startAndEndLocations.end());
    data.sizeOfCodes.insert(data.sizeOfCodes.end(), other.data.sizeOfCodes.begin(), other.data.sizeOfCodes.end());
}

[[nodiscard]] size_t CompilingBytecodes::length() const noexcept { return data.codes.size(); }

[[nodiscard]] CompilingBytecodes::Data CompilingBytecodes::finish() && noexcept {
    assert(data.startAndEndLocations.size() == data.sizeOfCodes.size() * 2); // twice as many.
    Data out { std::move(data) }; // this empties this->data.
    return out;
}

void CompilingBytecodes::backSetByte(size_t index, Byte newValue) noexcept {
    assert(index < data.codes.size());
    data.codes[index] = newValue;
}

void CompilingBytecodes::assertEmpty() const noexcept {
    assert(data.codes.empty());
    assert(data.startAndEndLocations.empty());
    assert(data.sizeOfCodes.empty());
}
