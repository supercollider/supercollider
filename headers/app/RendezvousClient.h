//
//  RendezvousClient.h
//  SC3lang
//
//  Created by C. Ramakrishnan on Mon Feb 24 2003.
//  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface OSCService : NSObject {
// a glorified struct (declared as an Obj-C class so I can put it in NSArrays)
// stored in the oscServices ivar of RendezvousClient
@public
	NSNetService*   netService;
	BOOL            isResolved;
	NSString* 	    hostName;
	const struct    sockaddr_in* sockaddr;
	unsigned 	    hostAddress;
	unsigned short  port;
	int				refCount;
}

@end

@interface RendezvousClient : NSObject {
	NSNetServiceBrowser* browser;
	NSMutableArray* oscServices;
}

+ (RendezvousClient*)sharedClient;

// interface
- (void)findOSCServices;
- (OSCService*)oscServiceAtIndex:(unsigned)index;
- (unsigned)numberOfOSCServices;

@end

