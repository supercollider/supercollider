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

#include <stdlib.h>
#include <string.h>
#include <DebugTable.h>
#include <SC_Assert.h>

DebugTableEntry DebugTable::mCurrentPosition;

void DebugTable::updatePosition(DebugTableEntry::ValueT line, DebugTableEntry::ValueT character) {
    mCurrentPosition = DebugTableEntry(line, character);
}

DebugTableEntry DebugTable::getPosition() { return mCurrentPosition; }

void DebugTable::advance() { mTable.push_back(mCurrentPosition); }

void DebugTable::addEntry(const DebugTableEntry& entry) { mTable.push_back(entry); }

void DebugTable::setEntry(size_t index, const DebugTableEntry& entry) {
    if (SC_COND_ASSERT(index < mTable.size())) {
        mTable[index] = entry;
    }
}

void DebugTable::dumpEntries(const char* name) const {
    printf("Compiling: %s\n", name);
    for (const DebugTableEntry& entry : mTable) {
        printf("    %d:%d \n", entry.mLine, entry.mCharacter);
    }
}

std::size_t DebugTable::size() const { return mTable.size(); }

const std::deque<DebugTableEntry>& DebugTable::entries() const { return mTable; }

void DebugTable::getEntries(PyrInt32Array& array, int lineOffset, int charOffset) const {
    SC_ASSERT(array.size == mTable.size() * kEntrySize);
    int tableI = 0;
    for (const auto& entry : mTable) {
        array.i[tableI++] = entry.getLine() + lineOffset;
        array.i[tableI++] = entry.getCharacter() + charOffset;
    }
}
