/*
 *  FileDialogPrimitives.M
 *  SC3lang
 *
 *  Created by cruxxial on Mon Nov 25 2002.


File utilties that depend on the Cocoa framework.


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

#import <Cocoa/Cocoa.h>

#import "PyrPrimitive.h"
#import "PyrObject.h"
#import "PyrKernel.h"
#import "VMGlobals.h"
#import "MyDocument.h"
#import "GC.h"

#import "SCDialog.h"

inline NSString* nsStringFromPyrSlot(PyrSlot *slot)
{
    PyrString* pyrString = slotRawString(slot);
    return [NSString stringWithCString: pyrString->s length: pyrString->size];
}


/** PRIMITIVES **/
int prGetPathsDialog(struct VMGlobals *g, int numArgsPushed);
int prGetPathsDialog(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *receiver = g->sp - 1; // CocoaDialog class
    PyrSlot *allowsMultiple = g->sp; // a bool

    SCDialog *dialog = [SCDialog receiver: slotRawObject(receiver) result: nil ];

	BOOL allows = IsTrue(allowsMultiple);
	SEL selector = @selector(getPaths:);
	NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:
								[dialog methodSignatureForSelector: selector]];
    [invocation setTarget:dialog];
    [invocation setSelector: selector];
	[invocation setArgument:&allows atIndex:2];
    [invocation retainArguments];

    [[SCVirtualMachine sharedInstance] defer: invocation];

    return errNone;
}

int prSavePanel(struct VMGlobals *g, int numArgsPushed);
int prSavePanel(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *receiver = g->sp - 1; // CocoaDialog class
    PyrSlot *string = g->sp; // a string

    SCDialog *dialog = [SCDialog receiver: slotRawObject(receiver) result: slotRawObject(string) ];

    [dialog scvmDeferWithSelector:@selector(savePanel) ];

    return errNone;
}

// NOTE: unused
int prStandardizePath(struct VMGlobals *g, int numArgsPushed);
int prStandardizePath(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *string = g->sp;
    PyrString *pyrString;
	NSString *path = nsStringFromPyrSlot(string);
	if([path length] != 0) {
		path = [path stringByStandardizingPath];
		pyrString = newPyrString(g->gc, [path fileSystemRepresentation], 0, true);
	} else {
		pyrString = newPyrString(g->gc, [path cString],0,true);
	}
	SetObject(string, pyrString);
    return errNone;
}

int prGetPathsInDirectory(struct VMGlobals *g, int numArgsPushed);
int prGetPathsInDirectory(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *receiver = g->sp - 3;
    PyrSlot *directoryPath = g->sp - 2;
    PyrSlot *extension = g->sp - 1;
    PyrSlot *returnArray  = g->sp;

    if(IsNil(directoryPath)) return errWrongType;

    PyrObject *returnObject = slotRawObject(returnArray);

    NSDirectoryEnumerator *direnum = [[NSFileManager defaultManager]
            enumeratorAtPath:nsStringFromPyrSlot(directoryPath)];
    NSString *pname;
    int i=0;
    int maxsize = MAXINDEXSIZE(returnObject);
    if(IsNil(extension)) {
        while (pname = [direnum nextObject]) {
            if(i >= maxsize) return errIndexOutOfRange;

            PyrString* pyrPathString = newPyrString(g->gc,[pname fileSystemRepresentation],0,true);

            PyrSlot slot;
            SetObject(&slot, pyrPathString);
            slotCopy(&returnObject->slots[i], &slot);
            g->gc->GCWrite(returnObject,pyrPathString);
            returnObject->size = i+1;
            i++;
        }
    } else {
        /*
		NSString *nsextension;
        nsextension = nsStringFromPyrSlot(extension);
        while (pname = [direnum nextObject]) {
            if (![[pname pathExtension] isEqualToString:nsextension]) {
                //[direnum skipDescendents]; don't enumerate this directory
            //} else {
                PyrString* pyrPathString = newPyrString(g->gc,[pname fileSystemRepresentation],0,true);

                PyrSlot slot;
                SetObject(&slot, pyrPathString);

                slotCopy(&returnObject->slots[i], &slot);

                g->gc->GCWrite(returnObject,pyrPathString);
                returnObject->size = i+1;
                i++;
            }
        }
        */
    }
    SetObject(receiver,returnObject);
    return errNone;
}

    /*
        NSDictionary *fattrs = [manager fileAttributesAtPath:fullpath traverseLink:YES];
        //NSString *fileName = [pname lastPathComponent];

        if ([fattrs objectForKey:NSFileType] != NSFileTypeDirectory)
    */



int prHelpFileForString(struct VMGlobals *g, int numArgsPushed);
int prHelpFileForString(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp;
    PyrSlot *receiver = g->sp - 1;

    // this function should be moved out of the MyDocument instance
    NSString *path = pathOfHelpFileFor( nsStringFromPyrSlot(a));

    if(path) {
        PyrString* pyrPathString = newPyrString(g->gc,[path fileSystemRepresentation],0,true);
        SetObject(receiver,pyrPathString);
    } else {
        SetNil(receiver);
    }
    return errNone;
}

/*
these are a still experimental cocoa <-> sc bridge
an sc class: NSObjectHolder holds cocoa objects and lets you send and receive
messages.

	prAllocInit { arg classname, initname,args;
		_ObjC_allocinit;
		^this.primitiveFailed;
	}
	prDealloc {
		_ObjC_dealloc;
	}
*/
/*
int prAllocInit(struct VMGlobals *g, int numArgsPushed);
int prAllocInit(struct VMGlobals *g, int numArgsPushed)
{
    PyrSlot *receiver = g->sp - 3;
    PyrSlot *classname = g->sp - 2;
   // PyrSlot *initname = g->sp - 1;
    //PyrSlot *args  = g->sp;

    // have to put them in a specific pool so they can be released on command

    // must defer in case of large loads and gui
    id newThing = [[NSClassFromString( nsStringFromPyrSlot(classname) ) class] alloc];
    [newThing init];
    if(newThing) {
        [newThing retain];
    } else {
        return errFailed;
    }
    //receiver set dataptr
    SetInt( receiver, (int)newThing );
    return errNone;
}

int prObjDo(struct VMGlobals *g, int numArgsPushed);
int prObjDo(struct VMGlobals *g, int numArgsPushed)
{
    PyrSlot *receiver = g->sp - 3;
    PyrSlot *dataptr = g->sp - 2;
    PyrSlot *methodname = g->sp - 1;
    PyrSlot *args = g->sp;

    dumpObjectSlot(receiver);
    dumpObjectSlot(dataptr);
    dumpObjectSlot(methodname);
    dumpObjectSlot(args);

    // have to put them in a specific pool so they can be released on command
    if(! IsInt(dataptr) ) return errWrongType;

    // check retain count and return in case
    SEL selector = NSSelectorFromString(nsStringFromPyrSlot(methodname));
    //err if not found

    NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:
                                        [(id)slotRawInt(dataptr) methodSignatureForSelector: selector]];
    [invocation setTarget:(id)slotRawInt(dataptr)];
    [invocation setSelector: selector];
    // check number of args is right

    if (!isSubclassOf( slotRawObject(args)->classptr, class_sequenceable_collection ))
        return errWrongType;

    int i;
    for(i = 0; i < slotRawObject(args)->size; i++) {
        [invocation setArgument: nsCodableFromPyrSlot(&(slotRawObject(args)->slots[i])) atIndex: i + 2];
        //setInvocationArgFromPyrSlot(invocation,i + 2,&(slotRawObject(args)->slots[i]));
    }
    [invocation retainArguments];
    NSLog(@"invocation: %@",invocation);
    [[SCVirtualMachine sharedInstance] defer: invocation];

    return errNone;
}

int prDealloc(struct VMGlobals *g, int numArgsPushed);
int prDealloc(struct VMGlobals *g, int numArgsPushed)
{
    //PyrSlot *receiver = g->sp - 1;
    PyrSlot *dataptr = g->sp;

    // have to put them in a specific pool so they can be released on command
    if(! IsInt(dataptr) ) return errWrongType;

    // check retain count and return in case
    [(id)slotRawInt(dataptr) release];
    return errNone;
}
*/

void initCocoaFilePrimitives()
{
    int base, index;

    base = nextPrimitiveIndex();
    index = 0;

    definePrimitive(base, index++, "_Cocoa_GetPathsDialog", prGetPathsDialog, 2, 0);
    definePrimitive(base, index++, "_Cocoa_SavePanel", prSavePanel, 2, 0);
    //definePrimitive(base, index++, "_Cocoa_SaveAsPlist", prSaveAsPlist, 2, 0);
//     definePrimitive(base, index++, "_Cocoa_StandardizePath", prStandardizePath, 1, 0);
    definePrimitive(base, index++, "_Cocoa_GetPathsInDirectory", prGetPathsInDirectory, 4, 0);
    definePrimitive(base, index++, "_Cocoa_HelpFileForString_", prHelpFileForString, 2, 0);

  /*  definePrimitive(base, index++, "_ObjC_AllocInit", prAllocInit, 4, 0);
    definePrimitive(base, index++, "_ObjC_Dealloc", prDealloc, 2, 0);
    definePrimitive(base, index++, "_ObjC_objectDo", prObjDo, 4, 0);	*/

}







