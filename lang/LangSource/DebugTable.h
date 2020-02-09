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

#ifndef LANG_DEBUGTABLE_H
#define LANG_DEBUGTABLE_H

#include <memory>
#include <stack>

#include "PyrObject.h"

class DebugTableEntry {
public:
    typedef std::int32_t ValueT;
#define DEBUGTABLE_NOVALUE -1

    friend class DebugTable;

    DebugTableEntry(): mLine(DEBUGTABLE_NOVALUE), mCharacter(DEBUGTABLE_NOVALUE) {}

    DebugTableEntry(std::uint32_t line, std::uint32_t character): mLine(line), mCharacter(character) {}

    ValueT getLine() const { return mLine; }

    ValueT getCharacter() const { return mCharacter; }

private:
    ValueT mLine;
    ValueT mCharacter;
};

class DebugTable {
public:
    static const std::size_t kEntrySize = 2;

    static void updatePosition(DebugTableEntry::ValueT line, DebugTableEntry::ValueT character);

    static DebugTableEntry getPosition();

    void advance();

    void addEntry(const DebugTableEntry& entry);

    void setEntry(size_t index, const DebugTableEntry& entry);

    void dumpEntries(const char*) const;

    const std::deque<DebugTableEntry>& entries() const;

    std::size_t size() const;

    void getEntries(PyrInt32Array& array, int lineOffset = 0, int charOffset = 0) const;

private:
    static DebugTableEntry mCurrentPosition;

    std::deque<DebugTableEntry> mTable;
};

#endif
