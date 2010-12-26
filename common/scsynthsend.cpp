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

#include <sys/types.h>
#ifdef _WIN32
# include <winsock2.h>
# define bzero( ptr, count ) memset( ptr, 0, count )
#else
# include <netinet/in.h>
# include <sys/socket.h>
#endif
#include "scsynthsend.h"
#include "SC_Endian.h"

void makeSockAddr(struct sockaddr_in &toaddr, int32 addr, int32 port)
{
    toaddr.sin_family = AF_INET;     // host byte order
    toaddr.sin_port = htons(port); // short, network byte order
    toaddr.sin_addr.s_addr = htonl(addr);
    bzero(&(toaddr.sin_zero), 8);    // zero the rest of the struct
}

int sendallto(int socket, const void *msg, size_t len, struct sockaddr *toaddr, int addrlen);
int sendall(int socket, const void *msg, size_t len);



