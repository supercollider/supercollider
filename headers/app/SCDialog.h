//
//  SCDialogs.h
//  SC3lang
//
//  Created by cruxxial on Tue Dec 17 2002.
//  Copyright (c) 2002 crucial-systems. All rights reserved.
//

#include <Cocoa/Cocoa.h>

#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "VMGlobals.h"
#import "SCVirtualMachine.h"



@interface SCDialog : NSObject {
    PyrObject *receiver;
    PyrObject *returnSlot;
    id temp;
}

+(id)receiver:(PyrObject*)receiver returnSlot:(PyrObject*)returnSlot;
-(id)initWithReceiver:(PyrObject*)argreceiver returnSlot:(PyrObject*)argreturnSlot;

// call method on self for Cocoa usage
-(void)detachThreadWithSelector:(SEL)selector;
// call method on self when SCVM is ready for sclang usage
-(void)scvmDeferWithSelector:(SEL)selector;


-(void)ok;
-(void)cancel;

-(void)getPaths;

@end
