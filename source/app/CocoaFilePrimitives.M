/*
 *  FileDialogPrimitives.M
 *  SC3lang
 *
 *  Created by cruxxial on Mon Nov 25 2002.
 *  Copyright (c) 2002 __MyCompanyName__. All rights reserved.
 *
 */

#include <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>
#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "VMGlobals.h"
#include "SC_RGen.h"
#include "GC.h"



int prGetPathsDialog(struct VMGlobals *g, int numArgsPushed);
int prGetPathsDialog(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

        PyrSlot *receiver = g->sp - 1; // a cocoa
        PyrSlot *array = g->sp; // an array

        NSDocumentController *docctl = [NSDocumentController sharedDocumentController];
        if (!docctl) {
            NSLog(@"No NSDocumentController");
            return errNone;
        }
        
        NSArray *urls = [docctl URLsFromRunningOpenPanel];
        if(urls) {
        
            int i;
            int count = [urls count];
            
            // TODO: limit at max size of array
            for (i = 0; i < count; i++)
            {
                PyrString* pyrPathString = newPyrString(g->gc,[[[urls objectAtIndex: i ] path] cString],0,true);
                
                PyrSlot slot;
                SetObject(&slot, pyrPathString);
                
                array->uo->slots[i].ucopy = slot.ucopy;

                g->gc->GCWrite(array->uo,pyrPathString);
                // have to set size field each time in order that gc can find the created objects
                array->uo->size = i+1;
            }
        }
        return errNone;
}


/**
//works fine
NSString* pyrSlotToNSString(PyrSlot *slot)
{
    PyrString* pyrString = slot->uos;
    return [NSString stringWithCString: pyrString->s length: pyrString->size];
}


//not done
id slotAsNSStringKey(PyrSlot* slot)
{
    // PyrClass *classOfSlot = classOfSlot(slot); 
    if(classOfSlot == class_symbol) {
        printf("symbol key\n");
        return [NSString stringWithCString: slot->us->name ];
    }
    // string stringFromPyrString
    if (slot->uo->classptr == class_string) {
        return pyrSlotToNSString(slot);
    }
    return @"this type not yet implemented";
}
**/

/*
int prSaveAsPlist(struct VMGlobals *g, int numArgsPushed);
int prSaveAsPlist(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;//if its deferred, does this matter ?

        PyrSlot *receiver = g->sp - 2; // Cocoa.sc
        PyrSlot *object = g->sp - 1;
        PyrSlot *path = g->sp;
        
      //  dumpObjectSlot(object);
       // dumpObjectSlot(path);
       // dumpObjectSlot(receiver);

       PListConvertor *pconv =  [[PListConvertor alloc] init];
       [pconv convertSCSlot: object];
        return errNone;
}
*/


/*
this is the start of an NSTask interface that returns stdout to SC

- (void)readTaskData:(id)sender
{
    NSTask *pipeTask = [[NSTask alloc] init];
    NSPipe *newPipe = [NSPipe pipe];
    NSFileHandle *readHandle = [newPipe fileHandleForReading];
    NSData *inData = nil;

    // write handle is closed to this process
    [pipeTask setStandardOutput:newPipe]; 
    [pipeTask setLaunchPath:[NSHomeDirectory()
            stringByAppendingPathComponent:
                    @"PipeTask.app/Contents/MacOS/PipeTask"]];
    [pipeTask launch];
	
    while ((inData = [readHandle availableData]) && [inData length]) {
        [self processData:inData];
    }
    [pipeTask release];
}

*/

void initCocoaFilePrimitives()
{
	int base, index;
	
	base = nextPrimitiveIndex();
	index = 0;

	definePrimitive(base, index++, "_Cocoa_GetPathsDialog", prGetPathsDialog, 2, 0);	
	//definePrimitive(base, index++, "_Cocoa_SaveAsPlist", prSaveAsPlist, 3, 0);	

}

