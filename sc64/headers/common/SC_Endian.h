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

/* NOTE: This file should declare/define the following functions/macros:
	
	htonl
	htons
	ntohl
	ntohs

   either explicitly or implicitly by including system headers.
*/

#ifndef SC_ENDIAN_H_INCLUDED
#define SC_ENDIAN_H_INCLUDED

#ifdef SC_DARWIN

# include <machine/endian.h>

#elif defined(SC_WIN32)

# define LITTLE_ENDIAN 1234
# define BIG_ENDIAN 4321
# define BYTE_ORDER LITTLE_ENDIAN
# include <winsock2.h>

#else

# include <endian.h>
# include <netinet/in.h>

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
