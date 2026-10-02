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

#include "SC_Endian.h"
#include "SC_Types.h"
#include <string.h>

// return the ptr to the byte after the OSC string.
SC_INLINE const char* OSCstrskip(const char* str) {
    //	while (str[3]) { str += 4; }
    //	return str + 4;
    do {
        str += 4;
    } while (str[-1]);
    return str;
}

// returns the number of bytes (including padding) for an OSC string.
SC_INLINE size_t OSCstrlen(const char* strin) { return (size_t)(OSCstrskip(strin) - strin); }

// returns a float, converting an int if necessary
SC_INLINE float32 OSCfloat(const char* inData) {
    elem32 elem;
    elem.u = sc_ntohl(*(uint32*)inData);
    return elem.f;
}

SC_INLINE int32 OSCint(const char* inData) { return (int32)sc_ntohl(*(uint32*)inData); }

SC_INLINE int64 OSCtime(const char* inData) {
    return ((int64)sc_ntohl(*(uint32*)inData) << 32) + (sc_ntohl(*(uint32*)(inData + 4)));
}

SC_INLINE float64 OSCdouble(const char* inData) {
    elem64 slot;
    slot.i = ((int64)sc_ntohl(*(uint32*)inData) << 32) + (sc_ntohl(*(uint32*)(inData + 4)));
    return slot.f;
}

struct sc_msg_blob {
    const void* data;
    size_t size;
};

/** @class Read the remaining OSC arguments from an OSC message.
 *
 *  This is used by bufgen, plugin command or unit command functions.
 *
 *  @attention Before SC 3.15, geti(), getf(), getd() and gets() would lead to
 *  unexpected behavior if the OSC argument was anything other than 'i', 'f', 'd' or 's'.
 *  Similarly, gett() and getb() did not correctly handle wrong argument types.
 *  Since SC 3.15, these functions correctly handle all argument types.
 */
struct sc_msg_iter {
    const char* data;
    const char* rdpos;
    const char* endpos;
    const char* tags;
    int32 size;
    int32 count;

#ifdef __cplusplus
    sc_msg_iter();
    sc_msg_iter(int inSize, const char* inData);

    /** @brief (re)initialize the sc_msg_iter */
    void init(int inSize, const char* inData);

    /** @brief Returns a timetag argument ('t')
     *
     *  If the OSC argument has the wrong type or there are no more arguments,
     *  the function returns `defaultValue`.
     *
     *  @note This will always consume an OSC argument (if any).
     */
    int64 gett(int64 defaultValue = 1);

    /** @brief Returns a numeric argument ('i' or 'f') as an int32.
     *
     *  If the OSC argument has the wrong type or there are no more arguments,
     *  the function returns `defaultValue`.
     *
     *  @note This will always consume an OSC argument (if any).
     */
    int32 geti(int32 defaultValue = 0);

    /** @brief Returns a numeric argument ('i', 'f' or 'd') as a float32.
     *
     *  If the OSC argument has the wrong type or there are no more arguments,
     *  the function returns `defaultValue`.
     *
     *  @note This will always consume an OSC argument (if any).
     */
    float32 getf(float32 defaultValue = 0.f);

    /** @brief Returns a numeric argument ('i', 'f' or 'd') as a float32.
     *
     *  If the argument has the wrong type or there are no more arguments,
     *  the function returns `defaultValue`.
     *
     *  @note This will always consume an OSC argument (if any).
     */
    float64 getd(float64 defaultValue = 0.0);

    /** @brief Returns a string argument ('s').
     *
     *  If the argument has the wrong type or there are no more arguments,
     *  the function returns `defaultValue`.
     *
     *  @note This will always consume an OSC argument (if any).
     *
     *  @attention Before SC 3.15 gets() returned `nullptr` if the end of
     *  arguments has been reached. Since 3.15 it returns `defaultValue`.
     */
    const char* gets(const char* defaultValue = nullptr);

    /** @brief Returns a string argument ('s') as a int32*.
     *
     *  If the argument has the wrong type or there are no more arguments,
     *  the function returns `defaultValue`.
     *
     *  @note This will always consume an OSC argument (if any).
     *
     *  @attention Before SC 3.15 gets() returned `nullptr` if the end of
     *  arguments has been reached. Since 3.15 it returns `defaultValue`.
     */
    int32* gets4(char* defaultValue = nullptr);

    /** @brief Returns the size of a blob ('b') or MIDI ('m') argument.
     *
     *  If the argument is not 'b' or 'm' (or there are no more arguments),
     *  the function returns 0.
     *
     *  This is typically used in conjunction with getb() below.
     *
     *  @note This does NOT consume the OSC argument. This is only done
     *  by getb() resp. skipb().
     */
    size_t getbsize() const;

    /** @brief Copies the blob or MIDI data into the given buffer and returns
     *  the number of bytes it has written.
     *
     *  Typically, you would first call getbsize() to get the blob size so
     *  you can allocate a suitably large buffer.
     *
     *  If the OSC argument is not 'b' or 'm' or there are no more arguments,
     *  the function returns 0.
     *
     *  @note This will always consume an OSC argument (if any).
     */
    size_t getb(char* outData, size_t inSize);

    /** @brief Returns a blob ('b') or MIDI ('m') argument as a sc_msg_blob.
     *
     *  If the OSC argument is not 'b' or 'm' or there are no more arguments,
     *  an empty struct is returned.
     *
     *  sc_msg_blob is a read-only view of the data. No copy is made!
     *
     *  @note This will always consume an OSC argument (if any).
     */
    sc_msg_blob getb();

    /** @brief Skips a blob ('b) or MIDI ('m') argument.
     *
     *  This function does the same as skiparg(). It is only kept for
     *  backwards compatibility.
     *
     *  @note This will always consume an OSC argument (if any).
     */
    void skipb();

    /** @brief Skips an OSC argument (if any).
     *
     *  This handles all OSC type tags described in the OSC 1.0 specification,
     *  including the "non-standard" tags.
     */
    void skiparg();

    /** @brief returns the number of remaining bytes.
     *
     *  @note If you want to check for remaining arguments, use hasArgs() instead!
     *  Certain argument types (T, F, N, I) do not have data, so remain() might
     *  return 0 even though there are still arguments left.
     */
    size_t remain() const { return (size_t)(endpos - rdpos); }

    /** @brief returns whether there are remaining OSC arguments. */
    bool hasArgs() const { return tags ? tags[count] != '\0' : false; }

    /** @brief Returns the next tag, or '\0' if the end of tags is reached.
     *  If the OSC message has not tags, `defaultTag` is returned.
     */
    char nextTag(char defaultTag = 'f') const { return tags ? tags[count] : defaultTag; }
#endif
};

#ifdef __cplusplus

inline sc_msg_iter::sc_msg_iter() {}

inline sc_msg_iter::sc_msg_iter(int inSize, const char* inData) { init(inSize, inData); }

inline void sc_msg_iter::init(int inSize, const char* inData) {
    data = inData;
    size = inSize;
    endpos = data + size;
    count = 0;
    if (data[0] == ',') {
        tags = data + 1;
        rdpos = OSCstrskip(data);
    } else {
        tags = nullptr;
        rdpos = data;
    }
}

inline int64 sc_msg_iter::gett(int64 defaultValue) {
    if (remain() <= 0)
        return defaultValue;
    int64 value;
    if (tags) {
        if (tags[count] == 't') {
            value = OSCtime(rdpos);
            rdpos += sizeof(int64);
        } else {
            skiparg();
            return defaultValue;
        }
    } else {
        value = OSCtime(rdpos);
        rdpos += sizeof(int64);
    }
    count++;
    return value;
}

inline int32 sc_msg_iter::geti(int32 defaultValue) {
    if (remain() <= 0)
        return defaultValue;
    int32 value;
    if (tags) {
        if (tags[count] == 'i') {
            value = OSCint(rdpos);
            rdpos += sizeof(int32);
        } else if (tags[count] == 'f') {
            value = (int32)OSCfloat(rdpos);
            rdpos += sizeof(float32);
        } else {
            skiparg();
            return defaultValue;
        }
    } else {
        value = (int32)OSCint(rdpos);
        rdpos += sizeof(int32);
    }
    count++;
    return value;
}

inline float32 sc_msg_iter::getf(float32 defaultValue) {
    if (remain() <= 0)
        return defaultValue;
    float32 value;
    if (tags) {
        if (tags[count] == 'f') {
            value = OSCfloat(rdpos);
            rdpos += sizeof(float32);
        } else if (tags[count] == 'd') {
            value = static_cast<float32>(OSCdouble(rdpos));
            rdpos += sizeof(float64);
        } else if (tags[count] == 'i') {
            value = static_cast<float32>(OSCint(rdpos));
            rdpos += sizeof(int32);
        } else {
            skiparg();
            return defaultValue;
        }
    } else {
        value = OSCfloat(rdpos);
        rdpos += sizeof(float32);
    }
    count++;
    return value;
}

inline float64 sc_msg_iter::getd(float64 defaultValue) {
    if (remain() <= 0)
        return defaultValue;
    float64 value;
    if (tags) {
        if (tags[count] == 'f') {
            value = (float64)OSCfloat(rdpos);
            rdpos += sizeof(float32);
        } else if (tags[count] == 'd') {
            value = OSCdouble(rdpos);
            rdpos += sizeof(float64);
        } else if (tags[count] == 'i') {
            value = (float64)OSCint(rdpos);
            rdpos += sizeof(int32);
        } else {
            skiparg();
            return defaultValue;
        }
    } else {
        value = OSCdouble(rdpos);
        rdpos += sizeof(float64);
    }
    count++;
    return value;
}

inline const char* sc_msg_iter::gets(const char* defaultValue) {
    if (remain() <= 0)
        return defaultValue;
    const char* value;
    if (tags) {
        if (tags[count] == 's' || tags[count] == 'S') {
            value = rdpos;
            rdpos = OSCstrskip(rdpos);
        } else {
            skiparg();
            return defaultValue;
        }
    } else {
        value = rdpos;
        rdpos = OSCstrskip(rdpos);
    }
    count++;
    return value;
}

inline int32* sc_msg_iter::gets4(char* defaultValue) { return (int32*)gets(defaultValue); }

inline size_t sc_msg_iter::getbsize() const {
    if (remain() <= 0)
        return 0;
    size_t len = 0;
    if (tags) {
        if (tags[count] == 'b')
            len = (size_t)OSCint(rdpos);
        else if (tags[count] == 'm')
            len = 4;
    } else {
        // assume 'b'
        len = (size_t)OSCint(rdpos);
    }
    return len;
}

inline size_t sc_msg_iter::getb(char* outArray, size_t arraySize) {
    if (remain() <= 0)
        return 0;
    size_t len = 0;
    if (tags) {
        if (tags[count] == 'b') {
            len = (size_t)OSCint(rdpos);
            rdpos += sizeof(int32);
        } else if (tags[count] == 'm') {
            len = 4;
        } else {
            skiparg();
            return 0;
        }
    } else {
        // assume 'b'
        len = (size_t)OSCint(rdpos);
    }
    if (len > arraySize)
        return 0;
    memcpy(outArray, rdpos, len);
    size_t len4 = (len + 3) & (size_t)-4;
    rdpos += len4;
    count++;
    return len;
}

inline sc_msg_blob sc_msg_iter::getb() {
    if (remain() <= 0)
        return sc_msg_blob {};
    const void* data = nullptr;
    size_t len = 0;
    if (remain() > 0 && tags) {
        if (tags[count] == 'b') {
            len = (size_t)OSCint(rdpos);
            data = rdpos + 4;
            rdpos += sizeof(int32);
        } else if (tags[count] == 'm') {
            len = 4;
            data = rdpos;
        } else {
            skiparg();
            return sc_msg_blob {};
        }
    }
    size_t len4 = (len + 3) & (size_t)-4;
    rdpos += len4;
    count++;
    return sc_msg_blob { data, len };
}

inline void sc_msg_iter::skipb() { skiparg(); }

inline void sc_msg_iter::skiparg() {
    if (tags == nullptr)
        return; // cannot skip arguments without tags...
    auto t = tags[count];
    if (t == '\0')
        return; // no more arguments
    size_t len;
    switch (t) {
    case 'i':
    case 'f':
    case 'c':
    case 'm':
    case 'r':
        len = 4;
        break;
    case 'h':
    case 'd':
    case 't':
        len = 8;
        break;
    case 's': // fallthrough
    case 'S':
        len = OSCstrlen(rdpos);
        break;
    case 'b':
        len = (size_t)OSCint(rdpos);
        rdpos += sizeof(int32);
        break;
    default:
        // all other types have no data
        len = 0;
        break;
    }
    size_t len4 = (len + 3) & (size_t)-4;
    rdpos += len4;
    count++;
}

#endif // __cplusplus
