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

#ifndef _scpacket_
#define _scpacket_

#include "SC_Endian.h"
#include "SC_Types.h"
#include <stdexcept>
#include <cstring>

struct netaddr {
	int socket;
	int addr;
	int port;
};
typedef struct netaddr netaddr;


template <int MaxPacketSize = 8192>
struct scpacket {
	static const int kBufSize = MaxPacketSize / sizeof(int32); // round down
	int32 *wrpos, *endpos, *msgsizepos;
	char *tagwrpos;
	int inbundle;
	int32 buf[kBufSize];

	void throw_overflow_exception()
	{
		throw std::runtime_error("buffer overflow");
	}

	scpacket() { reset(); }
	void reset()
	{
		wrpos = buf;
		endpos = buf + kBufSize;
		inbundle = 0;
	}
	void addi(int i)
	{
		if (wrpos >= endpos) throw_overflow_exception();
		*wrpos++ = htonl(i);
	}
	void addii(int64 ii)
	{
		int i;
		i = (int)(ii >> 32);
		addi(i);
		i = (int)ii;
		addi(i);
	}
	void addf(float f)
	{
		if (wrpos >= endpos) throw_overflow_exception();
		elem32 slot;
		slot.f = f;
		*wrpos++ = htonl(slot.i);
	}
	void addd(double f)
	{
		if (wrpos >= endpos) throw_overflow_exception();
		elem64 slot;
		slot.f = f;
		*wrpos++ = htonl(slot.i >> 32);
		*wrpos++ = htonl(slot.i & 0x00000000FFFFFFFF);
	}
	void adds(const char *src)
	{
		size_t len = strlen(src);
		size_t len4 = (len + 4) >> 2;
		if (wrpos + len4 > endpos) throw_overflow_exception();
		wrpos[len4 - 1] = 0;
		memcpy(wrpos, src, len);
		wrpos += len4;
	}
	void adds_slpre(const char *src) // prepends a slash
	{
		size_t len = strlen(src);
		size_t len4 = (len + 5) >> 2;
		if (wrpos + len4 > endpos) throw_overflow_exception();
		wrpos[len4 - 1] = 0;
		char* wrpos_c = (char*)wrpos;
		*wrpos_c = '/';
		memcpy(wrpos_c+1, src, len);
		wrpos += len4;
	}
	void adds(const char *src, size_t len)
	{
		size_t len4 = (len + 4) >> 2;
		if (wrpos + len4 > endpos) throw_overflow_exception();
		wrpos[len4 - 1] = 0;
		memcpy(wrpos, src, len);
		wrpos += len4;
	}
	void addb(uint8 *src, size_t len)
	{
		size_t len4 = (len + 3) >> 2;
		if (wrpos + (len4 + 1) > endpos) throw_overflow_exception();
		wrpos[len4 - 1] = 0;
		int32 swaplen = len;
		*wrpos++ = htonl(swaplen);
		memcpy(wrpos, src, len);
		wrpos += len4;
	}
	void addtag(char c) { *tagwrpos++ = c; }
	void skip(int n)
	{
		if (wrpos + n > endpos) throw_overflow_exception();
		wrpos += n;
	}
	void maketags(int n)
	{
		int size4 = (n + 4) >> 2;
		tagwrpos = (char*)wrpos;
		skip(size4);
		wrpos[-1] = 0;
	}
	int size() { return (char*)wrpos - (char*)buf; }
	char* data() { return (char*)buf; }

	void OpenBundle(int64 time)
	{
		inbundle++;
		adds("#bundle");
		addii(time);
	}
	void CloseBundle()
	{
		if (inbundle) inbundle--;
	}

	void BeginMsg()
	{
		if (inbundle) {
			msgsizepos = wrpos;
			addi(0);
		}
	}
	void EndMsg()
	{
		if (inbundle) {
			*msgsizepos = htonl(((wrpos - msgsizepos) - 1) * sizeof(int32));
		}
	}
};

typedef scpacket<> small_scpacket;
typedef scpacket<65516> big_scpacket;


#endif

