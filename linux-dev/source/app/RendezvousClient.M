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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#import "RendezvousClient.h"
#import "SCBase.h"

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


void initRendezvousPrimitives();

static RendezvousClient* sharedRendezvousClient = nil;

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
	
	browser = [[NSNetServiceBrowser alloc] init];
	[browser setDelegate: self];
	// 10 elements is a reasonable starting point
	oscServices = [[NSMutableArray alloc] initWithCapacity: 10];
	
	return self;
}

- (void)findOSCServices
{
	[browser searchForServicesOfType: @"_osc._udp." inDomain: @""];
}

- (unsigned)numberOfOSCServices
{
	return [oscServices count];
}

- (OSCService*)oscServiceAtIndex:(unsigned)index
{
	if (!(index < [oscServices count]))
		return nil;
	return [oscServices objectAtIndex: index];
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
	
	[aNetService setDelegate: self];
	[aNetService resolve];
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
	// get the addresses
	NSArray* addresses = [sender addresses];
	if (nil == addresses)
		return; // I don't think this should happen

	// just pick one of them
	NSData* address = [addresses lastObject];	
	if (nil == address)
		return; // I don't think this should happen
		
	OSCService* service = [[OSCService alloc] init];
	
	const struct sockaddr_in* sockaddr = (const struct sockaddr_in*) [address bytes];
	service->netService = sender;
	service->sockaddr = sockaddr;
	service->hostAddress = sockaddr->sin_addr.s_addr;
	service->port = sockaddr->sin_port;
	
	struct hostent*  hostent = gethostbyaddr((char*) &(sockaddr->sin_addr), 4, AF_INET);
	
	if (hostent)
		service->hostName = [[NSString alloc] initWithCString: hostent->h_name];
	else {
		// Couldn't find a host name for the address.
		// Convert the address to a string for the hostname
		char* addrOctets = inet_ntoa(sockaddr->sin_addr);
		service->hostName = [[NSString alloc] initWithCString: addrOctets];
	}

	// add this to the list of known services
	[oscServices addObject: service];
}

- (void)netService:(NSNetService *)sender didNotResolve:(NSDictionary *)errorDict
{
	post("Could not resolve the address for a discovered OSC Service\n");
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveDomain:(NSString *)domainString moreComing:(BOOL)moreComing
{
	// Ignore this -- I don't it happens, as I never triger a removal of a domain
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
			[oscServices removeObjectAtIndex: i];
			[service->hostName release];
			[service release];
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
	PyrObject *serverObject = serverSlot->uo;
	SetObject(&serverObject->slots[0], serverNameString);
	
	// hostName
	NSString* hostName = service->hostName;
	PyrString *hostNameString = newPyrString(g->gc, [hostName cString] , 0, true);	
	SetObject(&serverObject->slots[1], hostNameString);
	
	// port
	int port = (int) service->port;
	SetInt(&serverObject->slots[2], port);
	
/*
	// if OSCService is changed to hold onto a netAddr, use this to set the netAddr fields
	PyrSlot *netAddrSlot = &serverObject->slots[1];
	PyrObject *netAddrObject = netAddrSlot->uo;
	
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
