//
//  SCDialogs.m
//  SC3lang
//
//  Created by cruxxial on Tue Dec 17 2002.
//  Copyright (c) 2002 crucial-systems. All rights reserved.
//

#import "SCDialog.h"

#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "VMGlobals.h"
#include "SC_RGen.h"
#include "GC.h"
#include "PyrSched.h"



@implementation SCDialog

+(id)receiver:(PyrObject*)receiver returnSlot:(PyrObject*)returnSlot
{
    return [[super alloc] initWithReceiver: receiver returnSlot: returnSlot];
}
-(id)initWithReceiver:(PyrObject*)argreceiver returnSlot:(PyrObject*)argreturnSlot
{	
    if(self == [super init]) {
        receiver = argreceiver;
        returnSlot = argreturnSlot;
    }
    return self;
}



// call method on self for Cocoa usage
-(void)detachThreadWithSelector:(SEL)selector
{
    // detach
    [NSThread detachNewThreadSelector:selector toTarget:self withObject:NULL];
}
// call method on self when SCVM is ready for sclang usage
-(void)scvmDeferWithSelector:(SEL)selector
{
    NSInvocation *invocation =  [NSInvocation invocationWithMethodSignature:
                                                    [self methodSignatureForSelector: selector]];
    [invocation setTarget:self];
    [invocation setSelector: selector];
    [invocation retainArguments];
    
    [[SCVirtualMachine sharedInstance] defer: invocation];
}


-(void)getPaths
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

 //   [NSThread setThreadPriority: [NSThread threadPriority] * 0.9 ]; 
//    NSLog(@"in detached thread , my priority: %f",[NSThread threadPriority]);
    NSDocumentController *docctl = [NSDocumentController sharedDocumentController];
    
    NSArray *urls = [docctl URLsFromRunningOpenPanel];
    // on return go through this
    if(urls) {
        temp = [urls retain];
        [self scvmDeferWithSelector: @selector(returnPaths) ];
    } else {
        [self scvmDeferWithSelector: @selector(cancel) ];
    }
    [pool release];
}
        
-(void)returnPaths
{
    int i;
    NSArray *urls = (NSArray*) temp;
    int count = [urls count];
    
    VMGlobals *g = gMainVMGlobals;
    pthread_mutex_lock (&gLangMutex);
    // TODO: limit at max size of array
    for (i = 0; i < count; i++)
    {
        PyrString* pyrPathString = newPyrString(g->gc,[[[urls objectAtIndex: i ] path] cString],0,true);
        
        PyrSlot slot;
        SetObject(&slot, pyrPathString);
        
        returnSlot->slots[i].ucopy = slot.ucopy;

        g->gc->GCWrite(returnSlot,pyrPathString);
        // have to set size field each time in order that gc can find the created objects
        returnSlot->size = i+1;
    }
    pthread_mutex_unlock (&gLangMutex);

    [self ok];
}




-(void)ok
{
    PyrSymbol *method = getsym("ok");
    pthread_mutex_lock (&gLangMutex);
        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
        ++g->sp;  SetObject(g->sp, receiver ); 
        runInterpreter(g, method, 1);
        g->canCallOS = false;
    pthread_mutex_unlock (&gLangMutex);
}
-(void)cancel
{
    PyrSymbol *method = getsym("cancel");
    pthread_mutex_lock (&gLangMutex);
        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
        ++g->sp;  SetObject(g->sp, receiver ); 
        runInterpreter(g, method, 1);
        g->canCallOS = false;
    pthread_mutex_unlock (&gLangMutex);
}



    
@end
