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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef _sc_msg_iter_
#define _sc_msg_iter_

#include "SC_Endian.h"
#include "SC_Types.h"
#include <string.h>

// return the ptr to the byte after the OSC string.
inline char* OSCstrskip(char *str)
{
//	while (str[3]) { str += 4; }
//	return str + 4;
	do { str += 4; } while (str[-1]);
	return str;
}

// returns the number of bytes (including padding) for an OSC string.
inline int OSCstrlen(char *strin)
{
	return OSCstrskip(strin) - strin;
}

// returns a float, converting an int if necessary
inline float32 OSCfloat(char* inData)
{
	elem32* elem = (elem32*)inData;
	elem->u = ntohl(elem->u);
	return elem->f;
}

inline int32 OSCint(char* inData)
{
	return (int32)ntohl(*(uint32*)inData);
}

inline int64 OSCtime(char* inData)
{
	return ((int64)ntohl(*(uint32*)inData) << 32) + (ntohl(*(uint32*)(inData + 4)));
}

inline float64 OSCdouble(char* inData)
{
	elem64 slot;
	slot.i = ((int64)ntohl(*(uint32*)inData) << 32) + (ntohl(*(uint32*)(inData + 4)));
	return slot.f;
}

struct sc_msg_iter 
{
	char *data, *rdpos, *endpos, *tags;
	int size, count;
	
	sc_msg_iter();
	sc_msg_iter(int inSize, char* inData);
	void init(int inSize, char* inData);
	int32 geti(int32 defaultValue = 0);
	float32 getf(float32 defaultValue = 0.f);
	float64 getd(float64 defaultValue = 0.f);
	char *gets(char* defaultValue = 0);
	int32 *gets4(char* defaultValue = 0);
	size_t getbsize();
	void getb(char* outData, size_t inSize);
	void skipb();
	int remain() { return endpos - rdpos; }
        
    char nextTag(char defaultTag = 'f') { return tags ? tags[count] : defaultTag; }
};

inline sc_msg_iter::sc_msg_iter()
{
}

inline sc_msg_iter::sc_msg_iter(int inSize, char* inData)
{
	init(inSize, inData);
}

inline void sc_msg_iter::init(int inSize, char* inData)
{
	data = inData;
	size = inSize;
	endpos = data + size;
	count = 0;
	if (data[0] == ',') {
		tags = data+1;
		rdpos = OSCstrskip(data);
	} else {
		tags = 0;
		rdpos = data;
	}
}

inline int32 sc_msg_iter::geti(int32 defaultValue)
{
	int value;
	if (remain() <= 0) return defaultValue;
	if (tags) {
		if (tags[count] == 'i') {
			value = OSCint(rdpos);
			rdpos += sizeof(int32);
		} else if (tags[count] == 'f') {
			value = (int32)OSCfloat(rdpos);
			rdpos += sizeof(float32);
/*		} else if (tags[count] == 's') {
			value = atoi(rdpos);
			rdpos = OSCstrskip(rdpos);
*/
		} else {
			value = defaultValue;
		}
	} else {
		value = (int)OSCint(rdpos);
		rdpos += sizeof(int32);
	}
	count ++;
	return value;
}

inline float32 sc_msg_iter::getf(float32 defaultValue)
{
	float32 value;
	if (remain() <= 0) return defaultValue;
	if (tags) {
		if (tags[count] == 'f') {
			value = OSCfloat(rdpos);
			rdpos += sizeof(float32);
		} else if (tags[count] == 'd') {
			value = (float64)OSCdouble(rdpos);
			rdpos += sizeof(float64);
		} else if (tags[count] == 'i') {
			value = (float32)OSCint(rdpos);
			rdpos += sizeof(int32);
/*		} else if (tags[count] == 's') {
			value = atof(rdpos);
			rdpos = OSCstrskip(rdpos);
*/
		} else {
			value = defaultValue;
		}
	} else {
		value = OSCfloat(rdpos);
		rdpos += sizeof(float32);
	}
	count ++;
	return value;
}

inline float64 sc_msg_iter::getd(float64 defaultValue)
{
	float64 value;
	if (remain() <= 0) return defaultValue;
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
/*		} else if (tags[count] == 's') {
			value = atof(rdpos);
			rdpos = OSCstrskip(rdpos);
*/
		} else {
			value = defaultValue;
		}
	} else {
		value = OSCdouble(rdpos);
		rdpos += sizeof(float64);
	}
	count ++;
	return value;
}


inline char* sc_msg_iter::gets(char* defaultValue)
{
	char* value;
	if (remain() <= 0) return 0;
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
	count ++;
	return value;
}

inline int32* sc_msg_iter::gets4(char* defaultValue)
{
	int32* value;
	if (remain() <= 0) return 0;
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
	count ++;
	return value;
}

inline size_t sc_msg_iter::getbsize()
{
	if (remain() <= 0) return 0;
	if (tags && tags[count] != 'b') return 0;
	return (size_t)OSCint(rdpos);
}

inline void sc_msg_iter::getb(char* outArray, size_t size)
{
	size_t len = OSCint(rdpos);
	if (size < len) return;
	rdpos += sizeof(int32);
	size_t len4 = (len + 3) & -4;
	memcpy(outArray, rdpos, size);
	rdpos += len4;
	count ++;
}

inline void sc_msg_iter::skipb()
{
	size_t len = OSCint(rdpos);
	rdpos += sizeof(int32);
	size_t len4 = (len + 3) & -4;
	rdpos += len4;
	count ++;
}

#endif
