//
//  RendezvousClient.m
//  SC3lang
//
//  Created by C. Ramakrishnan on Mon Feb 24 2003.
//  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
//

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

#import "RendezvousClient.h"
#import "SCBase.h"

// for the locking mechanism
#import <Foundation/NSLock.h>

// SC headers for primitives
#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "VMGlobals.h"
#import "GC.h"

// Networking headers
// struct sockaddr_in
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// for gethostbyaddr
#include <netdb.h>

// DEBUG
#include <pthread.h>

@interface RendezvousClient (RendezvousClientNetServiceBrowserCallbacks)
// methods for callbacks from the NetServiceBrowser API

- (void)netServiceBrowserWillSearch:(NSNetServiceBrowser *)aNetServiceBrowser;
- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didFindService:(NSNetService *)aNetService moreComing:(BOOL)moreComing;
- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didNotSearch:(NSDictionary *)errorDict;
- (void)netBrowserDidStopSearch:(NSNetServiceBrowser *)aNetServiceBrowser;

- (void)netServiceWillResolve:(NSNetService *)sender;
- (void)netServiceDidResolveAddress:(NSNetService *)sender;
- (void)netService:(NSNetService *)sender didNotResolve:(NSDictionary *)errorDict;

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveDomain:(NSString *)domainString moreComing:(BOOL)moreComing;

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveService:(NSNetService *)aNetService moreComing:(BOOL)moreComing;

@end


@interface RendezvousClient (RendezvousClientPrivate)
// private methods for internal use
- (void)resolveAllServices;
- (void)resolveNextService;

@end


void initRendezvousPrimitives();

static NSString* kOSCServiceTypeStrings[kNumOSCServiceTypes] = {
	@"_osc._udp.",
	@"_osc._tcp."
};

static RendezvousClient* sharedRendezvousClient = nil;  // shared instance of the rendezvous client
static OSCService* serviceBeingResolved = nil;			// placeholder for what I'm currently resolving
static unsigned indexOfServiceBeingResolved = 0;		// which service am I currently resolving?


@implementation RendezvousClient

+ (RendezvousClient*)sharedClient
{
	if (nil == sharedRendezvousClient)
	{
		sharedRendezvousClient = [[RendezvousClient alloc] init];
	}
	
	return sharedRendezvousClient;
}

- (id)init
{
	[super init];
	
	for (int serviceType = 0; serviceType < kNumOSCServiceTypes; serviceType++) {
		browsers[serviceType] = [[NSNetServiceBrowser alloc] init];
		[browsers[serviceType] setDelegate: self];
	}
	
	// 10 elements is a reasonable starting point
	oscServices = [[NSMutableArray alloc] initWithCapacity: 10];
	
	return self;
}

- (void)findOSCServices
{
	for (int serviceType = 0; serviceType < kNumOSCServiceTypes; serviceType++) {
		[browsers[serviceType] searchForServicesOfType: kOSCServiceTypeStrings[serviceType] inDomain: @""];
	}
}

- (unsigned)numberOfOSCServices
{
	return [oscServices count];
}

- (OSCService*)oscServiceAtIndex:(unsigned)index
{
	if ((index >= [oscServices count]))
		return nil;
	return [oscServices objectAtIndex: index];
}

- (void)resolveAllServices
{
	// See the comment on re-entrancy in resolveNextService
	
	// begin with the first service, and let them each hand off to the next
	// (see the implementation of netServiceDidResolveAddress:)
	if ([oscServices count] < 1)
		return;
	
	indexOfServiceBeingResolved = 0;
	[self resolveNextService];
}

- (void)resolveNextService
{
	// NOTE ON RE-ENTRANCY OF THIS CODE
	// Theoretically, there should be locks between resolveAllServices, resolveNextService, and
	// netServiceDidResolveAddress, but in practice it is not necessary because all these run on the
	// same thread, so it is not possible, e.g., for resolveNextService to have simultaneous, interleaved
	// calls. Thus, there is no need for locking.
	
	// find something to resolve
	for (; indexOfServiceBeingResolved < [oscServices count]; indexOfServiceBeingResolved++) {
		serviceBeingResolved = [oscServices objectAtIndex: indexOfServiceBeingResolved];
		if (!serviceBeingResolved->isResolved)
			break;
	}

	// if we didn't find anything, we are done
	if (!(indexOfServiceBeingResolved < [oscServices count])) {
		// we're done
		serviceBeingResolved = nil;
		indexOfServiceBeingResolved = 0;
		return;
	}

	// resolve it!
	[serviceBeingResolved->netService setDelegate: self];
	[serviceBeingResolved->netService resolveWithTimeout:5]; // Deprecated; deprecator auto uses use resolveWithTimeout 5, should probably shorten
}

// Rendezvous implementation methods
- (void)netServiceBrowserWillSearch:(NSNetServiceBrowser *)aNetServiceBrowser
{
	// do nothing
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didFindService:(NSNetService *)aNetService moreComing:(BOOL)moreComing
{				
	if (nil == aNetService)
	{
		// I don't think this should happen...
		return;
	}
		
	// see if it is a duplicate
	unsigned arrayCount = [oscServices count];
	unsigned i;
	OSCService* potentialDuplicate;
	for(i = 0; i < arrayCount; i++)
	{
		potentialDuplicate = [oscServices objectAtIndex: i];
		if ([aNetService isEqual: potentialDuplicate->netService])
		{
			(potentialDuplicate->refCount)++;
			return;
		}
	}
	
	// allocate an OSCService object for this NSService
	// (autorelease it because we are going to put it into an array)
	OSCService* service = [[[OSCService alloc] init] autorelease];
	service->netService = aNetService;
	[service->netService retain];
	service->refCount++;
	service->isResolved = NO;
	
	// add this to the list of known services
	[oscServices addObject: service];
	
	// if there's no more coming, then it's time to resolve all the services
	if (!moreComing) {
		[self resolveAllServices];
	}
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didNotSearch:(NSDictionary *)errorDict
{
	post("Can't search for OSC Services\n");
}

- (void)netBrowserDidStopSearch:(NSNetServiceBrowser *)aNetServiceBrowser
{
	// do nothing
}

- (void)netServiceWillResolve:(NSNetService *)sender
{
	// do nothing
}

- (void)netServiceDidResolveAddress:(NSNetService *)sender
{
	if (!serviceBeingResolved) {
		// this is odd... I got a resolution I didn't ask for
		post("Resolved OSC Service %s without requesting it.\n", [[sender name] cString]);
		[sender stop];
		return;
	}

	// get the addresses
	NSArray* addresses = [sender addresses];
	if (nil == addresses)
		return; // I don't think this should happen

	// just pick one of them
	NSData* address = [addresses lastObject];	
	if (nil == address)
		return; // I don't think this should happen
	
	const struct sockaddr_in* sockaddr = (const struct sockaddr_in*) [address bytes];
	serviceBeingResolved->netService = sender;
	serviceBeingResolved->sockaddr = sockaddr;
	serviceBeingResolved->hostAddress = sockaddr->sin_addr.s_addr;
	serviceBeingResolved->port = sockaddr->sin_port;
	
	struct hostent*  hostent = gethostbyaddr((char*) &(sockaddr->sin_addr), 4, AF_INET);
	
	if (hostent)
		serviceBeingResolved->hostName = [[NSString alloc] initWithCString: hostent->h_name];
	else {
		// Couldn't find a host name for the address.
		// Convert the address to a string for the hostname
		char* addrOctets = inet_ntoa(sockaddr->sin_addr);
		serviceBeingResolved->hostName = [[NSString alloc] initWithCString: addrOctets];
	}
	serviceBeingResolved->isResolved = YES;
	[sender stop];
	
	[self resolveNextService];
}

- (void)netService:(NSNetService *)sender didNotResolve:(NSDictionary *)errorDict
{
	post("Could not resolve the address for a discovered OSC Service\n");
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveDomain:(NSString *)domainString moreComing:(BOOL)moreComing
{
	// Ignore this -- I don't think it happens, as I never triger a removal of a domain
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveService:(NSNetService *)aNetService moreComing:(BOOL)moreComing
{
	OSCService* service;

	// remove this from the list of services
	unsigned arrayCount = [oscServices count];
	unsigned i;
	for(i = 0; i < arrayCount; i++)
	{
		service = [oscServices objectAtIndex: i];
		if ([aNetService isEqual: service->netService])
		{
			service->refCount--;
			if (service->refCount < 1) {
				[service->hostName release];
				// removing it from the array will release the service object
				// too
				[oscServices removeObjectAtIndex: i];
			}
			return;
		}
	}
}

@end

@implementation OSCService

@end

// SuperCollider glue
int prNumOSCServices(struct VMGlobals *g, int numArgsPushed);
int prNumOSCServices(struct VMGlobals *g, int numArgsPushed)
{	
	unsigned numServices = [[RendezvousClient sharedClient] numberOfOSCServices];
	// set the result of the call to the numServices
	SetInt(g->sp, numServices);
	return errNone;
}

int prInitOSCService(struct VMGlobals *g, int numArgsPushed);
int prInitOSCService(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *serverSlot = g->sp -1;
	PyrSlot *indexSlot  = g->sp;
	
	int index;
	slotIntVal(indexSlot, &index);
	
	OSCService* service = [[RendezvousClient sharedClient] oscServiceAtIndex: index];
	if (nil == service)
		return errNone;
		
	// set the server name		
	NSString* name = [service->netService name];
	PyrString *serverNameString = newPyrString(g->gc, [name cString] , 0, true);
	PyrObject *serverObject = slotRawObject(serverSlot);
	SetObject(&serverObject->slots[0], serverNameString);
	
	if (!service->isResolved) {
		PyrString *hostNameString = newPyrString(g->gc, "" , 0, true);	
		SetObject(&serverObject->slots[1], hostNameString);
		
		// port
		int port = 0;
		SetInt(&serverObject->slots[2], port);
		return errNone;
	}
	
	// hostName
	NSString* hostName = service->hostName;
	PyrString *hostNameString = newPyrString(g->gc, [hostName cString] , 0, true);	
	SetObject(&serverObject->slots[1], hostNameString);
	
	// port
	int port = (int) service->port;
	SetInt(&serverObject->slots[2], port);
	
	// protocol
	NSString* type = [service->netService type];
	PyrSlot* protoSlot = &serverObject->slots[3];
	if ([type isEqualToString: kOSCServiceTypeStrings[kOSCServiceUDP]]) {
		SetSymbol(protoSlot, getsym("udp"));
	} else if ([type isEqualToString: kOSCServiceTypeStrings[kOSCServiceTCP]]) {
		SetSymbol(protoSlot, getsym("tcp"));
	} else {
		SetNil(protoSlot);
	}

/*
	// if OSCService is changed to hold onto a netAddr, use this to set the netAddr fields
	PyrSlot *netAddrSlot = &serverObject->slots[1];
	PyrObject *netAddrObject = slotRawObject(netAddrSlot);
	
	// addr
	int addr = (int) service->hostAddress;
	SetInt(&netAddrObject->slots[0], addr);
	
	// port
	int port = (int) service->port;
	SetInt(&netAddrObject->slots[1], port);
	
	// hostName
	NSString* hostName = service->hostName;
	PyrString *hostNameString = newPyrString(g->gc, [hostName cString] , 0, true);	
	SetObject(&serverObject->slots[2], hostNameString);
*/
		
	return errNone;
}

void initRendezvousPrimitives()
{
	int base, index;
	base = nextPrimitiveIndex();
	index = 0;
	definePrimitive(base, index++, "_NumOSCServices", prNumOSCServices, 1, 0);	
	definePrimitive(base, index++, "_InitOSCService", prInitOSCService, 3, 0);
}
