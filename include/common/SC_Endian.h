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

/* NOTE: This file should declare/define the following functions/macros:

	sc_htonl
	sc_htons
	sc_ntohl
	sc_ntohs

*/

#ifndef SC_ENDIAN_H_INCLUDED
#define SC_ENDIAN_H_INCLUDED

#if defined(__APPLE__)

# include <machine/endian.h>

#elif defined(__FreeBSD__) || defined(__OpenBSD__)

# include <sys/types.h>
# include <netinet/in.h>

#elif defined(_WIN32)

# define LITTLE_ENDIAN 1234
# define BIG_ENDIAN 4321
# define BYTE_ORDER LITTLE_ENDIAN

#define SC_NO_ENDIAN_FUNCTIONS

#elif defined(__linux__)

# include <endian.h>
# include <netinet/in.h>

#else

# error cannot find endianess on this platform

#endif


#ifndef SC_NO_ENDIAN_FUNCTIONS

static inline int sc_htonl(int arg)
{
	return htonl(arg);
}

static inline short sc_htons(short arg)
{
	return htons(arg);
}

static inline int sc_ntohl(int arg)
{
	return ntohl(arg);
}

static inline short sc_ntohs(short arg)
{
	return ntohs(arg);
}

#else

static inline int sc_htonl(int x)
{
#if BYTE_ORDER == LITTLE_ENDIAN
	char *s = (char *)&x;
	return (int)(s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3]);
#else
	return x;
#endif
}

static inline short sc_htons(short x)
{
#if BYTE_ORDER == LITTLE_ENDIAN
	char *s = (char *) &x;
	return (short)(s[0] << 8 | s[1]);
#else
	return x;
#endif
}

static inline int sc_ntohl(int x)
{
	return sc_htonl(x);
}

static inline short sc_ntohs(short x)
{
	return sc_htons(x);
}

#endif


#ifndef BYTE_ORDER
# error BYTE_ORDER undefined, check __FILE__
#endif // BYTE_ORDER

#ifndef BIG_ENDIAN
# error BIG_ENDIAN undefined, check __FILE__
#endif // BIG_ENDIAN

#ifndef LITTLE_ENDIAN
# error LITTLE_ENDIAN undefined, check __FILE__
#endif // LITTLE_ENDIAN

#endif // SC_ENDIAN_H_INCLUDED
