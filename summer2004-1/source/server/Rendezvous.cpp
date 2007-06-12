/*
 *  Rendezvous.cpp
 *  SC3synth
 *
 *  Created by C. Ramakrishnan on Wed Dec 18 2002.
 *  Illposed Software
 *
 */
 
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

#include "Rendezvous.h"
#include <CoreServices/CoreServices.h>

extern "C" {
	int scprintf(const char *fmt, ...);
}

void PublishPortToRendezvous(SCRendezvousProtocol protocol, short portNum)
{
	scprintf("PublishPortToRendezvous %d %hu\n", protocol, portNum);
	CFStringRef serviceType = 0;
	switch (protocol)
	{
		case kSCRendezvous_UDP:
			serviceType = CFSTR("_osc._udp.");
			break;
		case kSCRendezvous_TCP:
			serviceType = CFSTR("_osc._tcp.");
			break;
	}
	
	CFNetServiceRef netServiceRef =  
		CFNetServiceCreate(NULL, // use default allocator
			CFSTR(""),           // use default domain
			serviceType,
			CFSTR("SuperCollider"),
			portNum);
	// DEBUG
	if (!netServiceRef)
	{
		scprintf("Couldn't create a Rendezvous net service.\n");
		return;
	}
	
	CFNetServiceRegister(netServiceRef, NULL); // don't care about the error
}