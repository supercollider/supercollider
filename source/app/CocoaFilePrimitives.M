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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#import <Cocoa/Cocoa.h>

#import "PyrPrimitive.h"
#import "PyrObject.h"
#import "PyrKernel.h"
#import "VMGlobals.h"
#import "MyDocument.h"
#import "GC.h"

#import "SCDialog.h"

//#import "PyrNSconversions.M"

inline NSString* nsStringFromPyrSlot(PyrSlot *slot)
{
    PyrString* pyrString = slot->uos;
    return [NSString stringWithCString: pyrString->s length: pyrString->size];
}

//id nsCodableFromPyrSlot(PyrSlot* b);
id nsCodableFromPyrSlot(PyrSlot* b)
{
    if(IsFloat(b)) { // floats don't have tags, check for them first
        return [NSNumber numberWithFloat: b->uf];
    }
    if( ( classOfSlot(b) )->name.us == s_string ) {
        return nsStringFromPyrSlot(b);
    }
    
    // !! WARNING :  will break if array is not first instance variable in the SC class
    // identity dictionary doesn't work this way
    // maybe better to have a primitive for dictionaries that passes in keys, values
    if(  ( classOfSlot(b) )->name.us == s_dictionary ) {
        NSMutableDictionary *dict = [NSMutableDictionary dictionary];
        PyrSlot *array = &(b->uo->slots[0]); 
        int i = 0;
        PyrSlot *key;
        for(;i < array->uo->size; i += 2) {
            key = &( array->uo->slots[i] );
            if( NotNil( key )) {
                [dict 
                    setObject: nsCodableFromPyrSlot( &(array->uo->slots[i+1]) ) 
                    forKey: [(NSObject*)nsCodableFromPyrSlot(key) description] // symbol, string, number, or ?
                ];
            }
        }
        return dict;
    }
    
    switch (b->utag) {
            case tagInt :
                return [NSNumber numberWithInt: b->ui];
                break;
            case tagSym :
                return [NSString stringWithCString: b->us->name];
                break;
            case tagChar :
                return [NSString stringWithFormat: @"%c",b->uc];
                break;
            case tagNil :
                return [NSNull null];
                break;
            case tagFalse :
                return [NSNumber numberWithBool: NO];
                break;
            case tagTrue :
                return [NSNumber numberWithBool: YES];
                break;
            case tagPtr:
                return [NSNull null];
                break;
            case tagObj:
                    if (! isSubclassOf( b->uo->classptr, class_sequenceable_collection )) {
                        post("Cannot convert object for PropertyList:\n");
                        dumpObjectSlot(b);
                        return [NSNull null];
                    } else { // sequenceable collections
                        if(b->uo->size) {
                            NSMutableArray *array = [NSMutableArray arrayWithCapacity: b->uo->size];
                            int i;
                            for(i = 0; i < b->uo->size; i++) {
                                [array insertObject: nsCodableFromPyrSlot(&(b->uo->slots[i])) atIndex: i];
                            }
                            return array;
                        } else {
                            return [NSArray array];
                        }
                    }
                    break;
                
    /*   
            case tagInf :
            //dict must do values
                NSNumber with infinity
            NaN
                NSNumber with NaN
                
            default : // object asString
    */    
    }
    
    // can't happen ?
    NSLog( @"writeAsPlist failed to determine type of object");
    return [NSNull null];
}


/** PRIMITIVES **/

int prGetPathsDialog(struct VMGlobals *g, int numArgsPushed);
int prGetPathsDialog(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *receiver = g->sp - 1; // CocoaDialog class
    PyrSlot *array = g->sp; // an array
    
    SCDialog *dialog = [SCDialog receiver: receiver->uo resultArray: array->uo ];

    [dialog scvmDeferWithSelector:@selector(getPaths) ];

    return errNone;
}


int prSaveAsPlist(struct VMGlobals *g, int numArgsPushed);
int prSaveAsPlist(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    PyrSlot *object = g->sp - 1;
    PyrSlot *path = g->sp;
    
    if([NSKeyedArchiver archiveRootObject:nsCodableFromPyrSlot(object) toFile: nsStringFromPyrSlot(path) ]) {
        return errNone;
    } else {
        post("prSaveAsPlist failed !!\n");
        return errFailed;
    }
}

int prStandardizePath(struct VMGlobals *g, int numArgsPushed);
int prStandardizePath(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    PyrSlot *string = g->sp;
    PyrString *pyrString;
    NSString *path = nsStringFromPyrSlot(string);
    path = [path stringByStandardizingPath];
    pyrString = newPyrString(g->gc, [path cString], 0, true);
    SetObject(string, pyrString);
    return errNone;
}

//still buggy
int prGetPathsInDirectory(struct VMGlobals *g, int numArgsPushed);
int prGetPathsInDirectory(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *receiver = g->sp - 3;
    PyrSlot *directoryPath = g->sp - 2;
    PyrSlot *extension = g->sp - 1;
    PyrSlot *returnArray  = g->sp;
    
    if(IsNil(directoryPath)) return errWrongType;
    
   /*
    dumpPyrSlot(receiver);
    dumpPyrSlot(returnArray);
    dumpPyrSlot(extension);
    dumpPyrSlot(directoryPath);
   */
    
    PyrObject *returnObject = returnArray->uo;
    
    NSDirectoryEnumerator *direnum = [[NSFileManager defaultManager]
            enumeratorAtPath:nsStringFromPyrSlot(directoryPath)];
    NSString *pname;
    int i=0;
    int maxsize = MAXINDEXSIZE(returnObject);
    if(IsNil(extension)) {
        while (pname = [direnum nextObject]) {
            if(i >= maxsize) return errIndexOutOfRange;
            
            PyrString* pyrPathString = newPyrString(g->gc,[pname cString],0,true);
            
            PyrSlot slot;
            SetObject(&slot, pyrPathString);
            returnObject->slots[i].ucopy = slot.ucopy;
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
                //[direnum skipDescendents]; /* don't enumerate this directory */
            //} else {
            /*
                PyrString* pyrPathString = newPyrString(g->gc,[pname cString],0,true);
                
                PyrSlot slot;
                SetObject(&slot, pyrPathString);
                
                returnObject->slots[i].ucopy = slot.ucopy;
        
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

    PyrSlot *receiver = g->sp;
    
    // this function should be moved out of the MyDocument instance
    NSString *path = [[[NSDocumentController sharedDocumentController] currentDocument]
        pathOfHelpFileFor: nsStringFromPyrSlot(receiver) ];
    if(path) {
        PyrString* pyrPathString = newPyrString(g->gc,[path cString],0,true);
        SetObject(receiver,pyrPathString);
    } else {
        SetNil(receiver);
    }
    return errNone;
}



void initCocoaFilePrimitives()
{
    int base, index;
    
    base = nextPrimitiveIndex();
    index = 0;

    definePrimitive(base, index++, "_Cocoa_GetPathsDialog", prGetPathsDialog, 2, 0);	
    definePrimitive(base, index++, "_Cocoa_SaveAsPlist", prSaveAsPlist, 2, 0);	
    definePrimitive(base, index++, "_Cocoa_StandardizePath", prStandardizePath, 1, 0);	
    definePrimitive(base, index++, "_Cocoa_GetPathsInDirectory", prGetPathsInDirectory, 4, 0);	
    definePrimitive(base, index++, "_Cocoa_HelpFileForString_", prHelpFileForString, 1, 0);	

}







