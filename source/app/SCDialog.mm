//
//  SCDialogs.m
//  SC3lang
//
//  Created by cruxxial on Tue Dec 17 2002.
/*
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

#import "SCDialog.h"

#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "VMGlobals.h"
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



// detach method on self for Cocoa usage
-(void)detachThreadWithSelector:(SEL)selector
{
    [NSThread detachNewThreadSelector:selector toTarget:self withObject:NULL];
}
// SCVM defer method on self for sclang usage
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
    NSDocumentController *docctl = [NSDocumentController sharedDocumentController];
    
    NSArray *urls = [docctl URLsFromRunningOpenPanel];
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
