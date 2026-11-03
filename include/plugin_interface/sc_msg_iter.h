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

/** @brief returns a ptr to the byte after the OSC string. */
SC_INLINE const char* OSCstrskip(const char* str) {
    // OSC strings are always zero-padded to a multiple of 4 bytes.
    // If the last byte is not '\0', we know that another 4 bytes must follow.
    while (str[3]) {
        str += 4;
    }
    return str + 4;
}

/** @brief returns the byte size (including padding) of an OSC string. */
SC_INLINE size_t OSCstrlen(const char* strin) { return (size_t)(OSCstrskip(strin) - strin); }

/** @brief reads and returns a 32-bit integer */
SC_INLINE int32 OSCint(const char* inData) {
    int32 i;
    memcpy(&i, inData, 4);
    return sc_ntohl(i);
}

/** @brief reads and returns a 64-bit integer */
SC_INLINE int64 OSCint64(const char* inData) {
    int64 i;
    memcpy(&i, inData, 8);
    return sc_ntohll(i);
}

/** @brief reads and returns a 32-bit float */
SC_INLINE float32 OSCfloat(const char* inData) {
    // get data as 32-bit int with correct byte order
    int32 i = OSCint(inData);
    // convert bit pattern to float32
    float32 f;
#ifdef __cplusplus
    static_assert(sizeof(i) == sizeof(f), "size mismatch");
#endif
    memcpy(&f, &i, sizeof(f));
    return f;
}

/** @brief reads and returns an OSC timetag */
SC_INLINE int64 OSCtime(const char* inData) { return OSCint64(inData); }

/** @brief reads and returns a 64-bit float */
SC_INLINE float64 OSCdouble(const char* inData) {
    // get data as 64-bit int with correct byte order
    int64 i = OSCint64(inData);
    // convert bit pattern to float64
    float64 d;
#ifdef __cplusplus
    static_assert(sizeof(i) == sizeof(d), "size mismatch");
#endif
    memcpy(&d, &i, sizeof(d));
    return d;
}

/** @brief A struct that only contains the argument part of an OSC message,
 *  from the typetag string to the end of the message.
 *  It is used for passing OSC message arguments to plugin command, unit
 *  command and bufgen functions.
 */
struct sc_msg_iter {
    /** @brief the OSC message data (starting at the typetag string) */
    const char* data;
    /** @brief the read position for the next argument data */
    const char* rdpos;
    /** @brief the end of the OSC message data */
    const char* endpos;
    /** @brief the start of the actual typetags, without the leading ','.
     *  `tags[count]` yields the typetag for the next argument or '\0'
     *  if all arguments have been read.
     */
    const char* tags;
    /** @brief the total size of the OSC message. Same as endpos - data. */
    int32 size;
    /** @brief the number of arguments read so far */
    int32 count;

#ifdef __cplusplus
    /** @brief default constructor */
    sc_msg_iter();
    /** @brief constructor that takes OSC message data */
    sc_msg_iter(int inSize, const char* inData);
    /** @brief (re)init with different data */
    void init(int inSize, const char* inData);
    /** @brief read and return a timetag ('t') argument.
     *  If the argument has the wrong type or there are no remaining
     *  arguments, the given default value is returned.
     */
    int64 gett(int64 defaultValue = 1);
    /** @brief read and return a 32-bit integer ('i') argument.
     *  If the argument has the wrong type or there are no remaining
     *  arguments, the given default value is returned.
     */
    int32 geti(int32 defaultValue = 0);
    /** @brief read and return a 32-bit float ('f') argument.
     *  If the argument has the wrong type or there are no remaining
     *  arguments, the given default value is returned.
     */
    float32 getf(float32 defaultValue = 0.f);
    /** @brief read and return a 64-bit float ('d') argument.
     *  If the argument has the wrong type or there are no remaining
     *  arguments, the given default value is returned.
     */
    float64 getd(float64 defaultValue = 0.f);
    /** @brief read and return an OSC string ('s') argument.
     *  If the argument has the wrong type or there are no remaining
     *  arguments, the given default value is returned.
     */
    const char* gets(const char* defaultValue = 0);
    /** @brief read and return an OSC string ('s') as a int32*.
     *  If the argument has the wrong type or there are no remaining
     *  arguments, the given default value is returned.
     */
    int32* gets4(char* defaultValue = 0);
    /** @brief returns the size of the upcoming OSC blob ('b') argument.
     *  MIDI arguments ('m') are also supported; in that case the size
     *  is always 4.
     *  If the argument has the wrong type or there are no remaining
     *  arguments, a size of 0 is reported.
     */
    size_t getbsize();
    /** @brief read the blob argument data into the provided buffer.
     *  If the argument is not a blob ('b') or MIDI message ('m'),
     *  the behavior is undefined.
     */
    void getb(char* outData, size_t inSize);
    /** @brief skip the current blob ('b') or MIDI ('m') argument.
     *  If the argument is not a blob ('b') or MIDI message ('m'),
     *  the behavior is undefined.
     */
    void skipb();
    /** @brief returns the remaining bytes of the OSC argument data.
     *  @note A return value of 0 does not necessarily mean that there
     *  are no more pending arguments! Some OSC typetags ('F', 'T',
     *  'N' and 'I') do not have any associated data.
     */
    size_t remain() { return (size_t)(endpos - rdpos); }
    /** @brief returns the typetag for the next argument.
     *  If there are no more arguments, '\0' is returned.
     */
    char nextTag(char defaultTag = 'f') { return tags ? tags[count] : defaultTag; }
#endif
};

#ifdef __cplusplus

inline sc_msg_iter::sc_msg_iter(): data(nullptr), rdpos(nullptr), endpos(nullptr), tags(nullptr), size(0), count(0) {}

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
        tags = 0;
        rdpos = data;
    }
}

inline int64 sc_msg_iter::gett(int64 defaultValue) {
    int64 value;
    if (remain() <= 0)
        return defaultValue;
    if (tags) {
        if (tags[count] == 't') {
            value = OSCtime(rdpos);
            rdpos += sizeof(int64);
        } else {
            /* this is dangerous, as rdpos is not
               advanced accordingly while count++ takes
                 place */
            value = defaultValue;
        }
    } else {
        value = OSCtime(rdpos);
        rdpos += sizeof(int64);
    }
    count++;
    return value;
}

inline int32 sc_msg_iter::geti(int32 defaultValue) {
    int value;
    if (remain() <= 0)
        return defaultValue;
    if (tags) {
        if (tags[count] == 'i') {
            value = OSCint(rdpos);
            rdpos += sizeof(int32);
        } else if (tags[count] == 'f') {
            value = (int32)OSCfloat(rdpos);
            rdpos += sizeof(float32);
        } else if (tags[count] == 's') {
            /*	value = atoi(rdpos); */
            value = defaultValue;
            rdpos = OSCstrskip(rdpos);
        } else if (tags[count] == 'b') {
            value = defaultValue;
            skipb();
        } else {
            /* this is dangerous, as rdpos is not
               advanced accordingly while count++ takes
               place */
            value = defaultValue;
        }
    } else {
        value = (int)OSCint(rdpos);
        rdpos += sizeof(int32);
    }
    count++;
    return value;
}

inline float32 sc_msg_iter::getf(float32 defaultValue) {
    float32 value;
    if (remain() <= 0)
        return defaultValue;
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
        } else if (tags[count] == 's') {
            /*    value = atof(rdpos); */
            value = defaultValue;
            rdpos = OSCstrskip(rdpos);
        } else if (tags[count] == 'b') {
            value = defaultValue;
            skipb();
        } else {
            /* this is dangerous, as rdpos is not
               advanced accordingly while count++ takes
               place */
            value = defaultValue;
        }
    } else {
        value = OSCfloat(rdpos);
        rdpos += sizeof(float32);
    }
    count++;
    return value;
}

inline float64 sc_msg_iter::getd(float64 defaultValue) {
    float64 value;
    if (remain() <= 0)
        return defaultValue;
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
        } else if (tags[count] == 's') {
            /*    value = atof(rdpos); */
            value = defaultValue;
            rdpos = OSCstrskip(rdpos);
        } else if (tags[count] == 'b') {
            value = defaultValue;
            skipb();
        } else {
            /* this is dangerous, as rdpos is not
               advanced accordingly while count++ takes
               place */
            value = defaultValue;
        }
    } else {
        value = OSCdouble(rdpos);
        rdpos += sizeof(float64);
    }
    count++;
    return value;
}


inline const char* sc_msg_iter::gets(const char* defaultValue) {
    const char* value;
    if (remain() <= 0)
        return 0;
    if (tags) {
        if (tags[count] == 's') {
            value = rdpos;
            rdpos = OSCstrskip(rdpos);
        } else {
            value = defaultValue;
        }
    } else {
        value = rdpos;
        rdpos = OSCstrskip(rdpos);
    }
    count++;
    return value;
}

inline int32* sc_msg_iter::gets4(char* defaultValue) {
    int32* value;
    if (remain() <= 0)
        return 0;
    if (tags) {
        if (tags[count] == 's') {
            value = (int32*)rdpos;
            rdpos = OSCstrskip(rdpos);
        } else {
            value = (int32*)defaultValue;
        }
    } else {
        value = (int32*)rdpos;
        rdpos = OSCstrskip(rdpos);
    }
    count++;
    return value;
}

inline size_t sc_msg_iter::getbsize() {
    size_t len = 0;
    if (remain() <= 0)
        return 0;
    if (tags) {
        if (tags[count] == 'b')
            len = (size_t)OSCint(rdpos);
        else if (tags[count] == 'm')
            len = 4;
    }
    return len;
}

inline void sc_msg_iter::getb(char* outArray, size_t arraySize) {
    size_t len = 0;
    if (tags[count] == 'b') {
        len = (size_t)OSCint(rdpos);
        if (arraySize < len)
            return;
        rdpos += sizeof(int32);
    } else if (tags[count] == 'm') {
        len = 4;
        if (arraySize < len)
            return;
    }
    size_t len4 = (len + 3) & (size_t)-4;
    memcpy(outArray, rdpos, arraySize);
    rdpos += len4;
    count++;
}

inline void sc_msg_iter::skipb() {
    size_t len = 0;
    if (tags[count] == 'b') {
        len = (size_t)OSCint(rdpos);
        rdpos += sizeof(int32);
    } else if (tags[count] == 'm')
        len = 4;
    size_t len4 = (len + 3) & (size_t)-4;
    rdpos += len4;
    count++;
}

#endif // __cplusplus
