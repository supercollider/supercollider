/*
 *  FileDialogPrimitives.M
 *  SC3lang
 *
 *  Created by cruxxial on Mon Nov 25 2002.

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

#include <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>
#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "VMGlobals.h"
#include "GC.h"

#import "SCDialog.h"


inline NSString* pyrSlotToNSString(PyrSlot *slot)
{
    PyrString* pyrString = slot->uos;
    return [NSString stringWithCString: pyrString->s length: pyrString->size];
}

id convertSCSlot(PyrSlot* b);
id convertSCSlot(PyrSlot* b)
{
    if(IsFloat(b)) { // floats don't have tags, check for them first
        return [NSNumber numberWithFloat: b->uf];
    }
    if( ( classOfSlot(b) )->name.us == s_string ) {
        return pyrSlotToNSString(b);
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
                    setObject: convertSCSlot( &(array->uo->slots[i+1]) ) 
                    forKey: [(NSObject*)convertSCSlot(key) description] // symbol, string, number, or ?
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
                                [array insertObject: convertSCSlot(&(b->uo->slots[i])) atIndex: i];
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
    
    if([NSKeyedArchiver archiveRootObject:convertSCSlot(object) toFile: pyrSlotToNSString(path) ]) {
        return errNone;
    } else {
        post("prSaveAsPlist failed !!\n");
        return errFailed;
    }
}


void initCocoaFilePrimitives()
{
    int base, index;
    
    base = nextPrimitiveIndex();
    index = 0;

    definePrimitive(base, index++, "_Cocoa_GetPathsDialog", prGetPathsDialog, 2, 0);	
    definePrimitive(base, index++, "_Cocoa_SaveAsPlist", prSaveAsPlist, 2, 0);	
}







