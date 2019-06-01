/*
 *  Rendezvous.cpp
 *  SC3synth
 *
 *  Created by C. Ramakrishnan on Wed Dec 18 2002.
 *  Illposed Software
 *
 *  Avahi implementation (c) 2006 stefan kersten
 *  Howl implementation (c) 2005 2006 stefan kersten
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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "Rendezvous.h"
#include "SC_Export.h"

SCSYNTH_DLLEXPORT_C int scprintf(const char* fmt, ...);

#ifndef __APPLE__
static const char* kSCRendezvousServiceName = "SuperCollider";

static const char* SCRendezvousProtocolString(SCRendezvousProtocol proto) {
    switch (proto) {
    case kSCRendezvous_UDP:
        return "_osc._udp.";
    case kSCRendezvous_TCP:
        return "_osc._tcp.";
    default:
        return 0;
    };
}
#endif

#if defined(__APPLE__) && !defined(SC_IPHONE)
#    include <CoreServices/CoreServices.h>

void PublishPortToRendezvous(SCRendezvousProtocol protocol, short portNum) {
    scprintf("PublishPortToRendezvous %d %hu\n", protocol, portNum);
    CFStringRef serviceType = 0;
    switch (protocol) {
    case kSCRendezvous_UDP:
        serviceType = CFSTR("_osc._udp.");
        break;
    case kSCRendezvous_TCP:
        serviceType = CFSTR("_osc._tcp.");
        break;
    }

    CFNetServiceRef netServiceRef = CFNetServiceCreate(NULL, // use default allocator
                                                       CFSTR(""), // use default domain
                                                       serviceType, CFSTR("SuperCollider"), portNum);
    // DEBUG
    if (!netServiceRef) {
        scprintf("Couldn't create a Rendezvous net service.\n");
        return;
    }

    CFNetServiceRegisterWithOptions(netServiceRef, 0, NULL); // don't care about the error
    CFRelease(netServiceRef);
}

#elif HAVE_AVAHI
#    include <avahi-client/client.h>
#    include <avahi-client/publish.h>
#    include <avahi-common/alternative.h>
#    include <avahi-common/error.h>
#    include <avahi-common/malloc.h>
#    include <avahi-common/thread-watch.h>
#    include <avahi-common/timeval.h>
#    include <stdlib.h>
#    include <stdio.h>
#    include <SC_Lock.h>

struct AvahiEntry {
    AvahiEntry* mNext;
    SCRendezvousProtocol mProto;
    short mPort;
    bool mRegistered;
};

struct AvahiSession {
    AvahiSession();
    ~AvahiSession();

    void PublishPort(SCRendezvousProtocol proto, short port);
    void CreateServices(AvahiClient* client);
    void CreateServices() { CreateServices(mClient); }
    void ResetServices();
    void RenameService();

    static void client_cb(AvahiClient* client, AvahiClientState state, void* data);
    static void group_cb(AVAHI_GCC_UNUSED AvahiEntryGroup*, AvahiEntryGroupState state, void* data);
    static void modify_cb(AVAHI_GCC_UNUSED AvahiTimeout*, void* data);

    AvahiThreadedPoll* mPoll;
    AvahiClient* mClient;
    AvahiEntryGroup* mGroup;
    AvahiEntry* mEntries;
    SC_Lock mMutex;
    char* mServiceName;
};

struct AvahiSessionInstance {
    AvahiSessionInstance(): mSession(0) {}
    ~AvahiSessionInstance() {
        if (mSession) {
            delete mSession;
        }
    }
    AvahiSession* GetSession() {
        if (!mSession) {
            mSession = new AvahiSession();
        }
        return mSession;
    }

private:
    AvahiSession* mSession;
};

static AvahiSessionInstance gAvahiSession;

AvahiSession::AvahiSession(): mPoll(0), mClient(0), mGroup(0), mEntries(0), mServiceName(0) {
    int err;

    mServiceName = avahi_strdup(kSCRendezvousServiceName);

    mPoll = avahi_threaded_poll_new();
    if (!mPoll) {
        scprintf("Zeroconf: failed to create poll API\n");
        return;
    }

    mClient = avahi_client_new(avahi_threaded_poll_get(mPoll), (AvahiClientFlags)0, client_cb, this, &err);
    if (!mClient) {
        scprintf("Zeroconf: failed to create client: %s\n", avahi_strerror(err));
        avahi_threaded_poll_free(mPoll);
        mPoll = 0;
        return;
    }

    avahi_threaded_poll_start(mPoll);
}

AvahiSession::~AvahiSession() {
    if (mClient) {
        avahi_threaded_poll_stop(mPoll);
        avahi_client_free(mClient);
        avahi_threaded_poll_free(mPoll);
        AvahiEntry* entry = mEntries;
        while (entry) {
            AvahiEntry* next = entry->mNext;
            delete entry;
            entry = next;
        }
    }
    free(mServiceName);
}

void AvahiSession::client_cb(AvahiClient* client, AvahiClientState state, void* data) {
    AvahiSession* self = (AvahiSession*)data;

    switch (state) {
    case AVAHI_CLIENT_S_RUNNING:
        self->CreateServices(client);
        break;
    case AVAHI_CLIENT_S_COLLISION:
        self->ResetServices();
        break;
    case AVAHI_CLIENT_FAILURE:
        scprintf("Zeroconf: client failure: %s\n", avahi_strerror(avahi_client_errno(self->mClient)));
        break;
    case AVAHI_CLIENT_CONNECTING:
    case AVAHI_CLIENT_S_REGISTERING:
        break;
    }
}

void AvahiSession::group_cb(AVAHI_GCC_UNUSED AvahiEntryGroup*, AvahiEntryGroupState state, void* data) {
    AvahiSession* self = (AvahiSession*)data;

    switch (state) {
    case AVAHI_ENTRY_GROUP_ESTABLISHED:
        scprintf("Zeroconf: registered service '%s'\n", self->mServiceName);
        break;
    case AVAHI_ENTRY_GROUP_COLLISION: {
        self->RenameService();
        self->CreateServices();
        break;
    }
    case AVAHI_ENTRY_GROUP_FAILURE:
    case AVAHI_ENTRY_GROUP_UNCOMMITED:
    case AVAHI_ENTRY_GROUP_REGISTERING:
        break;
    }
}

void AvahiSession::modify_cb(AVAHI_GCC_UNUSED AvahiTimeout* e, void* data) {
    AvahiSession* self = (AvahiSession*)data;
    if (avahi_client_get_state(self->mClient) == AVAHI_CLIENT_S_RUNNING)
        self->CreateServices();
}

void AvahiSession::PublishPort(SCRendezvousProtocol proto, short port) {
    if (!mClient)
        return;

    AvahiEntry* entry = new AvahiEntry;
    entry->mProto = proto;
    entry->mPort = port;
    entry->mRegistered = false;

    mMutex.lock();
    entry->mNext = mEntries;
    mEntries = entry;
    mMutex.unlock();

    avahi_threaded_poll_lock(mPoll);
    struct timeval tv;
    avahi_threaded_poll_get(mPoll)->timeout_new(avahi_threaded_poll_get(mPoll), avahi_elapse_time(&tv, 0, 0), modify_cb,
                                                this);
    avahi_threaded_poll_unlock(mPoll);
}

void AvahiSession::CreateServices(AvahiClient* client) {
    int err;

    if (mGroup) {
        avahi_entry_group_reset(mGroup);
    } else {
        mGroup = avahi_entry_group_new(client, group_cb, this);
        if (!mGroup) {
            scprintf("Zeroconf: failed to create entry group: %s\n", avahi_strerror(avahi_client_errno(client)));
            return;
        }
    }

    mMutex.lock();
    AvahiEntry* entry = mEntries;
    while (entry) {
        const char* type = SCRendezvousProtocolString(entry->mProto);
        err = avahi_entry_group_add_service(mGroup, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, (AvahiPublishFlags)0,
                                            mServiceName, type, NULL, NULL, entry->mPort, NULL);
        if (err == AVAHI_ERR_COLLISION) {
            // BUG: shouldn't this actually be triggered in the entry
            //      group callback?
            RenameService();
            err = avahi_entry_group_add_service(mGroup, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, (AvahiPublishFlags)0,
                                                mServiceName, type, NULL, NULL, entry->mPort, NULL);
        }
        if (err < 0) {
            scprintf("Zeroconf: failed to register service '%s': %s\n", mServiceName, avahi_strerror(err));
        }
        entry = entry->mNext;
    }
    mMutex.unlock();

    if (!avahi_entry_group_is_empty(mGroup)) {
        err = avahi_entry_group_commit(mGroup);
        if (err < 0) {
            scprintf("Zeroconf: failed to commit entry group: %s\n", avahi_strerror(err));
            return;
        }
    }
}

void AvahiSession::ResetServices() {
    if (mGroup)
        avahi_entry_group_reset(mGroup);
}

void AvahiSession::RenameService() {
    char* name = avahi_alternative_service_name(mServiceName);
    avahi_free(mServiceName);
    mServiceName = name;
}

void PublishPortToRendezvous(SCRendezvousProtocol proto, short port) {
    gAvahiSession.GetSession()->PublishPort(proto, port);
}

#elif HAVE_HOWL
#    include <howl.h>
#    include <sys/poll.h>
#    include <SC_Lock.h>

struct HowlSession {
    HowlSession();
    ~HowlSession();

    void PublishPort(SCRendezvousProtocol protocol, short portNum);
    void PublishPort(sw_discovery session, SCRendezvousProtocol protocol, short portNum);

    sw_discovery mSession;
    SC_Lock mMutex;
};

struct HowlSessionInstance {
    HowlSessionInstance(): mSession(0) {}
    ~HowlSessionInstance() {
        if (mSession) {
            delete mSession;
        }
    }
    HowlSession* GetSession() {
        if (!mSession) {
            mSession = new HowlSession();
        }
        return mSession;
    }

private:
    HowlSession* mSession;
};

static HowlSessionInstance gHowlSession;

HowlSession::HowlSession(): mSession(0) {}

HowlSession::~HowlSession() {
    if (mSession)
        sw_discovery_fina(mSession);
}

void HowlSession::PublishPort(SCRendezvousProtocol protocol, short portNum) {
    mMutex.lock();
    if (!mSession)
        sw_discovery_init(&mSession);
    if (mSession)
        PublishPort(mSession, protocol, portNum);
    mMutex.unlock();
}

sw_result howl_publish_reply_func(sw_discovery, sw_discovery_oid, sw_discovery_publish_status, sw_opaque) {
    return SW_OKAY;
}

void HowlSession::PublishPort(sw_discovery session, SCRendezvousProtocol protocol, short portNum) {
    const char* serviceType = SCRendezvousProtocolString(protocol);
    sw_discovery_oid oid;

    sw_result res = sw_discovery_publish(session,
                                         0, // interface
                                         kSCRendezvousServiceName, // name
                                         serviceType, // type
                                         0, // domain (.local)
                                         0, // host
                                         portNum, // port
                                         0, 0, // text records
                                         howl_publish_reply_func, 0, // reply func
                                         &oid // request id
    );

    scprintf("Zeroconf: publishing service %s on port %hu %s\n", serviceType, portNum,
             res == SW_OKAY ? "succeeded" : "failed");
}

void PublishPortToRendezvous(SCRendezvousProtocol protocol, short portNum) {
    gHowlSession.GetSession()->PublishPort(protocol, portNum);
}

#else // !__APPLE__ && !HAVE_AVAHI && !HAVE_HOWL

void PublishPortToRendezvous(SCRendezvousProtocol protocol, short portNum) {}

#endif // __APPLE__ || HAVE_AVAHI || HAVE_HOWL
