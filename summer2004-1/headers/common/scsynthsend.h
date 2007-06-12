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

#ifndef _scpacket_
#define _scpacket_

#include "SC_Endian.h"
#include "SC_Types.h"
#include <stdexcept>

struct netaddr {
	int socket;
	int addr;
	int port;
};
typedef struct netaddr netaddr;

#define kSCMaxPacketSize 8192

struct scpacket {
	int32 *wrpos, *endpos, *msgsizepos;
	char *tagwrpos;
	int inbundle;
	int32 buf[kSCMaxPacketSize];

	scpacket();
	void reset();
	void addi(int i);
	void addii(int64 ii);
	void addf(float f);
	void addd(double f);
	void adds(char *cstr);
	void adds(char *src, size_t len);
	void addb(uint8 *src, size_t len);
	void addtag(char c);
	void skip(int n);
	void maketags(int n);
	int size() { return (char*)wrpos - (char*)buf; }
	char* data() { return (char*)buf; }
	
	void sendudp(int socket, int addr, int port);

	void OpenBundle(int64 time);
	void CloseBundle();
	
	void BeginMsg();
	void EndMsg();
};

inline scpacket::scpacket() { reset(); }

// ways to fail
#define BUFFEROVERFLOW return
//#define BUFFEROVERFLOW  throw std::runtime_error("buffer overflow")

inline void scpacket::reset()
{
	wrpos = buf;
	endpos = buf + kSCMaxPacketSize;
	inbundle = 0;
}

inline void scpacket::skip(int n)
{
	if (wrpos + n > endpos) BUFFEROVERFLOW;
	wrpos += n;
}

inline void scpacket::addtag(char c)
{
	*tagwrpos++ = c;
}

inline void scpacket::maketags(int n)
{
	int size4 = (n + 4) >> 2;
	tagwrpos = (char*)wrpos;
	skip(size4);
	wrpos[-1] = 0;
}


inline void scpacket::addi(int i)
{
	if (wrpos >= endpos) BUFFEROVERFLOW;
	*wrpos++ = htonl(i);
}

inline void scpacket::addii(int64 ii)
{
	int i;
	i = (int)(ii >> 32);
	addi(i);
	i = (int)ii;
	addi(i);
}

inline void scpacket::addf(float f)
{
	if (wrpos >= endpos) BUFFEROVERFLOW;
	elem32 slot;
	slot.f = f;
	*wrpos++ = htonl(slot.i);
}

inline void scpacket::addd(double f)
{
	if (wrpos >= endpos) BUFFEROVERFLOW;
	elem64 slot;
	slot.f = f;
	*wrpos++ = htonl(slot.i >> 32);
	*wrpos++ = htonl(slot.i & 0x00000000FFFFFFFF);
}

inline void scpacket::adds(char *src)
{
	size_t len = strlen(src);
	size_t len4 = (len + 4) >> 2;
	if (wrpos + len4 > endpos) BUFFEROVERFLOW;
	wrpos[len4 - 1] = 0;
	memcpy(wrpos, src, (size_t)len);
	wrpos += len4;
}

inline void scpacket::adds(char *src, size_t len)
{
	size_t len4 = (len + 4) >> 2;
	if (wrpos + len4 > endpos) BUFFEROVERFLOW;
	wrpos[len4 - 1] = 0;
	memcpy(wrpos, src, (size_t)len);
	wrpos += len4;
}

// support binary objects
inline void scpacket::addb(uint8 *src, size_t len)
{
	size_t len4 = (len + 3) >> 2;
	if (wrpos + (len4 + 1) > endpos) BUFFEROVERFLOW;
	wrpos[len4 - 1] = 0;
	int32 swaplen = len;
	*wrpos++ = htonl(swaplen);	
	memcpy(wrpos, src, (size_t)len);
	wrpos += len4;
}

inline void scpacket::OpenBundle(int64 time)
{
	inbundle++;
	adds("#bundle");
	addii(time);
}

inline void scpacket::CloseBundle()
{
    if (inbundle) inbundle--;
}

inline void scpacket::BeginMsg()
{
    if (inbundle) {
        msgsizepos = wrpos;
        addi(0);
    }
}

inline void scpacket::EndMsg()
{
	if (inbundle) {
		*msgsizepos = htonl(((wrpos - msgsizepos) - 1) * sizeof(int32));
	}
}

#endif

