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


@implementation SCDialog

+(id)receiver:(PyrObject*)argReceiver resultArray:(PyrObject*)argResultArray
{
    return [[super alloc] initWithReceiver: argReceiver resultArray: argResultArray];
}

-(id)initWithReceiver:(PyrObject*)argReceiver resultArray:(PyrObject*)argResultArray
{	
    if(self == [super init]) {
        receiver = argReceiver;
        resultArray = argResultArray;
	}
    return self;
}

-(void)scvmDeferWithSelector:(SEL)selector
{
    NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:
                                        [self methodSignatureForSelector: selector]];
    [invocation setTarget:self];
    [invocation setSelector: selector];
    [invocation retainArguments];
    
    [[SCVirtualMachine sharedInstance] defer: invocation];
}


-(void)getPaths
{
    NSDocumentController *docctl = [NSDocumentController sharedDocumentController];
    
    NSArray *urls = [docctl URLsFromRunningOpenPanel];
    if(urls) {
        temp = [urls retain];
        [self returnPaths];
    } else {
        [self cancel];
    }
}
        
-(void)returnPaths
{
    int i;
    NSArray *urls = (NSArray*) temp;
    [temp release];
    int count = [urls count];
    
    VMGlobals *g = gMainVMGlobals;
    pthread_mutex_lock (&gLangMutex);
    for (i = 0; i < count; i++)
    {
        PyrString* pyrPathString = newPyrString(g->gc,[[[urls objectAtIndex: i ] path] cString],0,true);
        
        PyrSlot slot;
        SetObject(&slot, pyrPathString);
        
        resultArray->slots[i].ucopy = slot.ucopy;

        g->gc->GCWrite(resultArray,pyrPathString);
        // have to set size field each time in order that gc can find the created objects
        resultArray->size = i+1;
    }
    pthread_mutex_unlock (&gLangMutex);

    [self ok];
}





-(void)ok
{
    pthread_mutex_lock (&gLangMutex);
		PyrSymbol *method = getsym("ok");
        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
        ++g->sp;  SetObject(g->sp, receiver ); 
        runInterpreter(g, method, 1);
        g->canCallOS = false;
    pthread_mutex_unlock (&gLangMutex);
}

-(void)cancel
{
    pthread_mutex_lock (&gLangMutex);
		PyrSymbol *method = getsym("cancel");
        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
        ++g->sp;  SetObject(g->sp, receiver ); 
        runInterpreter(g, method, 1);
        g->canCallOS = false;
    pthread_mutex_unlock (&gLangMutex);
}



    
@end
