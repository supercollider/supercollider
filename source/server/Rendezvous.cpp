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

extern "C" {
	int scprintf(const char *fmt, ...);
}

#if SC_DARWIN
#include <CoreServices/CoreServices.h>

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
#endif // SC_DARWIN

#if SC_LINUX
# if HAVE_CONFIG_H
#  include "config.h"
# endif

# if HAVE_HOWL
#  include <howl/howl.h>
#  include <sys/poll.h>
#  include <pthread.h>

struct HowlSession
{
	HowlSession();
	~HowlSession();

	void PublishPort(SCRendezvousProtocol protocol, short portNum);
	void PublishPort(sw_discovery session, SCRendezvousProtocol protocol, short portNum);

	sw_discovery		mSession;
	pthread_mutex_t		mMutex;
};

static HowlSession gHowlSession;

HowlSession::HowlSession()
	: mSession(0)
{
	pthread_mutex_init(&mMutex, 0);
}

HowlSession::~HowlSession()
{
	if (mSession) sw_discovery_fina(mSession);
	pthread_mutex_destroy(&mMutex);
}

void HowlSession::PublishPort(SCRendezvousProtocol protocol, short portNum)
{
	pthread_mutex_lock(&mMutex);
	if (!mSession) sw_discovery_init(&mSession);
	if (mSession) PublishPort(mSession, protocol, portNum);
	pthread_mutex_unlock(&mMutex);
}

sw_result
howl_publish_reply_func(
	sw_discovery,
	sw_discovery_oid,
	sw_discovery_publish_status,
	sw_opaque
	)
{
	return SW_OKAY;
}

void HowlSession::PublishPort(sw_discovery session, SCRendezvousProtocol protocol, short portNum)
{
	char* serviceType = 0;
	sw_discovery_oid oid;

	switch (protocol) {
		case kSCRendezvous_UDP:
			serviceType = "_osc._udp.";
			break;
		case kSCRendezvous_TCP:
			serviceType = "_osc._tcp.";
			break;
	};

	sw_result res =
		sw_discovery_publish(
			session,
			0,							// interface
			"SuperCollider",			// name
			serviceType,				// type
			0,							// domain (.local)
			0,							// host
			portNum,					// port
			0, 0,						// text records
			howl_publish_reply_func, 0,	// reply func
			&oid						// request id
			);

	scprintf(
		"Zeroconf: publishing service %s on port %hu %s\n",
		serviceType, portNum,
		res == SW_OKAY ? "succeeded" : "failed"
		);
}

void PublishPortToRendezvous(SCRendezvousProtocol protocol, short portNum)
{
	gHowlSession.PublishPort(protocol, portNum);
}
# else // !HAVE_HOWL
void PublishPortToRendezvous(SCRendezvousProtocol protocol, short portNum)
{
}
# endif // HAVE_HOWL
#endif // SC_LINUX
