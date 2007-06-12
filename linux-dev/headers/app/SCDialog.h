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
#include "GC.h"
#include "PyrSched.h"
#import "SCVirtualMachine.h"

@interface SCDialog : NSObject {
    PyrObject *receiver;
    PyrObject *resultArray;
    id temp;
}

+(id)receiver:(PyrObject*)argReceiver resultArray:(PyrObject*)argResultArray;
-(id)initWithReceiver:(PyrObject*)argReceiver resultArray:(PyrObject*)argResultArray;

// call method on self when SCVM is ready for sclang usage
-(void)scvmDeferWithSelector:(SEL)selector;

-(void)ok;
-(void)cancel;

-(void)returnPaths;
-(void)getPaths;

@end
