//
//  SCDialogs.h
//  SC3lang
//
//  Created by cruxxial on Tue Dec 17 2002.
//  Copyright (c) 2002 crucial-systems. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "VMGlobals.h"
#include "GC.h"
#include "PyrSched.h"
#import "SCVirtualMachine.h"

@interface SCDialog : NSObject {
    PyrObject *receiver;
    PyrObject *result;
	NSOpenPanel *openPanel;
}

+(id)receiver:(PyrObject*)argReceiver result:(PyrObject*)argResult;
-(id)initWithReceiver:(PyrObject*)argReceiver result:(PyrObject*)argResult;

-(void)ok;
-(void)cancel;
-(void)error;

-(void)returnPaths:(NSArray*)urls;
-(void)getPaths;
-(void)returnPath:(NSString*)path;


// call method on self when SCVM is ready for sclang usage
-(void)scvmDeferWithSelector:(SEL)selector;


@end
