/*
 *  CocoaBridgePrimitives.M
 *  SC3lang
 *
 *  Created by Jan Trutzschler 08/2005.


	sc3 bridge to obj-c/cocoa


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
#import "SCVirtualMachine.h"
#include "/usr/include/objc/objc-runtime.h"
//put this in a .h
@interface SCCocoaToLangAction : NSResponder
{
    struct PyrObject *mSCObject;
}


- (void)setSCObject: (struct PyrObject*)inObject;
- (struct PyrObject*)getSCObject;
- (void) doAction: (id) sender;
- (void) doFloatAction: (id) sender;
- (void) doIntAction: (id) sender;
- (void) doStateAction: (id) sender;
- (void) keyUp: (NSEvent*) event;
- (void) keyDown: (NSEvent*) event;
- (void) mouseDown: (NSEvent*) event;
- (void) release;
- (void)windowWillClose:(NSNotification *)aNotification;
@end
//

extern bool compiledOK;

@implementation SCCocoaToLangAction
- (void)setSCObject: (struct PyrObject*)inObject
{
	mSCObject = inObject;
}
- (struct PyrObject*)getSCObject
{
	return mSCObject;
}
- (void) doFloatAction: (id) sender
{
//	post("doAction \n");
	//need to sort out the event here ...
	PyrObject * pobj = [self getSCObject];
	if(compiledOK && pobj){
		PyrSymbol *method = getsym("doAction");
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, pobj); 
		++g->sp;  SetFloat(g->sp, [sender floatValue]); 
		runInterpreter(g, method, 2);
		g->canCallOS = false;
	}
}
- (void) doAction: (id) sender
{
//	post("doAction \n");
    pthread_mutex_lock (&gLangMutex);	
	PyrObject * pobj = [self getSCObject];
	if(compiledOK && pobj){
		PyrSymbol *method = getsym("doAction");
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, pobj); 
		++g->sp;  SetInt(g->sp, 1); 
		runInterpreter(g, method, 2);
		g->canCallOS = false;
	}
    pthread_mutex_unlock (&gLangMutex);		
}
- (void) doStateAction: (id) sender
{
//	post("doAction \n");
    pthread_mutex_lock (&gLangMutex);	
	PyrObject * pobj = [self getSCObject];
	if(compiledOK && pobj){
		PyrSymbol *method = getsym("doAction");
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, pobj); 
		++g->sp;  SetInt(g->sp, [sender state]); 
		runInterpreter(g, method, 2);
		g->canCallOS = false;
	}
    pthread_mutex_unlock (&gLangMutex);	

}
- (void) doIntAction: (id) sender
{
    pthread_mutex_lock (&gLangMutex);	
	PyrObject * pobj = [self getSCObject];
	if(compiledOK && pobj){
		PyrSymbol *method = getsym("doAction");
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, pobj); 
		++g->sp;  SetInt(g->sp, [sender intValue]); 
		runInterpreter(g, method, 2);
		g->canCallOS = false;
	}
    pthread_mutex_unlock (&gLangMutex);	
	
}
- (void) keyUp: (NSEvent*) event
{
}
- (void) keyDown: (NSEvent*) event
{
	post("keyDown in cocoaToLang \n");
}
- (void) mouseDown: (NSEvent*) event
{
	post("doMouse \n");

}
- (void) release
{
	printf("releasing cocoaAction\n");
	delete mSCObject;
	mSCObject = NULL;
	[super release];
}
- (void)windowWillClose:(NSNotification *)aNotification
{
   pthread_mutex_lock (&gLangMutex);	
	PyrObject * pobj = [self getSCObject];
	if(compiledOK && pobj){
		PyrSymbol *method = getsym("doAction");
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, pobj); 
		runInterpreter(g, method, 1);
		g->canCallOS = false;
	}
    pthread_mutex_unlock (&gLangMutex);	
}
@end

inline NSString* nsStringFromPyrSlot(PyrSlot *slot)
{
    PyrString* pyrString = slot->uos;
    return [NSString stringWithCString: pyrString->s length: pyrString->size];
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

int slotGetNSRect(PyrSlot* a, NSRect *r);
int slotGetPoint(PyrSlot* a, NSPoint *r);

PyrSymbol *s_nsObject;
//NSAutoreleasePool *nsAutoReleasePool;
NSMutableArray *nsobjects;


inline int nsinvocationSetArguments(PyrSlot* args, NSInvocation **anInvocation);
inline int nsinvocationSetArguments(PyrSlot* args, NSInvocation **anInvocation)
{
	int len = args->uo->size;
//	NSInvocation * anInvocation = invocation;
	for(int i =0; i<len; i++)
	{
		PyrSlot * slot = args->uo->slots+i;
		char *cType = (char *)[[*anInvocation methodSignature] getArgumentTypeAtIndex: i+2];		

		if(IsFloat(slot))
		{
			if(!(*cType == _C_FLT || *cType == _C_DBL)){post("wrong type at arg: %i excepted float\n", i); return errWrongType;}
			float val;
			slotFloatVal(slot, &val);				
			[*anInvocation setArgument: &val atIndex: i+2];       
		} else if(IsInt(slot))
		{
			if(!(*cType == _C_INT || *cType == _C_UINT || *cType == _C_LNG || *cType == _C_ULNG ||  *cType == _C_CHR))
			{post("wrong type at arg: %i excepted %c \n", i, *cType); return errWrongType;}								
			int val;
			slotIntVal(slot, &val);
			[*anInvocation setArgument: &val atIndex: i+2];       
		} else if(isKindOfSlot(slot, s_nsObject->u.classobj))
		{
			if(*cType != _C_ID) return errWrongType;
			id val = (id) slot->uo->slots[0].ui;
			[*anInvocation setArgument: &val atIndex: i+2];       
		} 
		//else if(IsNil)
		//{
		
		//} 
		else if (isKindOfSlot(slot, s_rect->u.classobj))
		{
			if(*cType != _C_STRUCT_B) return errWrongType;
			NSRect bounds;
			int err = slotGetNSRect(slot, &bounds);		
			if (err) return err;
			[*anInvocation setArgument: &bounds atIndex: i+2];       
		} else if (isKindOfSlot(slot, s_string->u.classobj))
		{
			if(!(*cType == _C_CHARPTR || *cType == 'r')) return errWrongType;
			char * ch = slot->uos->s;
			if(!ch) return errFailed;
			[*anInvocation setArgument: (void*)  &ch atIndex: i+2]; 
		} else if (isKindOfSlot(slot, s_point->u.classobj))
		{
			if(*cType != _C_STRUCT_B) return errWrongType;			
			NSPoint point;
			int err = slotGetPoint(slot, &point);		
			if (err) return err;
			[*anInvocation setArgument: &point atIndex: i+2];   				
		} else if (isKindOfSlot(slot, s_arrayed_collection->u.classobj))
		{
			//for now only arrays of SCNSObject
			id * nsObjects = (id*) malloc(args->uo->size * sizeof(id));
			for(int j=0;  j<slot->uo->size; j++)
			{
				PyrSlot * nsslot = slot->uo->slots+j;
				nsObjects[j] =  (id) nsslot->uo->slots[0].ui;
			}
			if(!nsObjects) return errFailed;
			[*anInvocation setArgument: &nsObjects atIndex: i+2];  
		}
	}
	return errNone;
}

int prObjcAllocInit(struct VMGlobals *g, int numArgsPushed);
int prObjcAllocInit(struct VMGlobals *g, int numArgsPushed)
{
    PyrSlot *receiver = g->sp - 4;
    PyrSlot *classname = g->sp - 3;
	PyrSlot *initname = g->sp - 2;
    PyrSlot *args  = g->sp-1;
    PyrSlot *defer  = g->sp;
	
	
	bool hasArg = NotNil(args);
	bool isCollection = isKindOfSlot(args, class_array);

    // have to put them in a specific pool so they can be released on command

    // must defer in case of large loads and gui
	NSString * nsclassname = nsStringFromPyrSlot(classname);
	if([nsclassname length] < 1){ return errFailed; }
	
    id newThing = [[NSClassFromString( nsclassname ) class] alloc];
	[newThing retain];
	NSString * nsinitname = nsStringFromPyrSlot(initname);
	if([nsinitname length] < 1){ [newThing release]; return errFailed; }

	SEL sel = NSSelectorFromString(nsinitname);
	if(![[NSClassFromString( nsclassname ) class] instancesRespondToSelector:sel])
	{
		//if there is a wrong init-method better set object to nil ...		
		[newThing release];
		SetNil(receiver);	
		post("Warning: NSClass: %s does not respond to: %s \n", [nsclassname cString], [nsinitname cString]);
		return errNone;
	}
	NSMethodSignature *sig = [[NSClassFromString( nsclassname ) class]  instanceMethodSignatureForSelector: sel];
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: newThing];
    [anInvocation setSelector: sel];
	int numberOfArgs = (int) [sig numberOfArguments] - 2; //cocoa args start at 2
	
	if (hasArg && isCollection && numberOfArgs>0)
	{
		int len = args->uo->size;
		
		//should check for type [sig getArgumentTypeAtIndex: i];
		if(numberOfArgs != len) 
		{
			post("Warning: NSClass: %s numberOfArguments does not match: %i, provided: %i \n", [nsclassname cString],numberOfArgs, len);		

			[newThing release]; 
			SetNil(receiver);			
			return errFailed;
		};
		int err = nsinvocationSetArguments(args, &anInvocation);
		if(err){ [newThing release]; SetNil(receiver); return err;}
	} else if (numberOfArgs>0)
	{
		post("Warning: mismatching arguments \n");
		[newThing release];
		SetNil(receiver);
		return errFailed;
	}
	
	if(IsTrue(defer))
		[scvm defer: anInvocation];
	else 
	{
		id val;
		[anInvocation invoke];
		[anInvocation getReturnValue: (void*)&val];				
		newThing = (id) val;
		[newThing retain];
	}
//	[nsAutoReleasePool addObject:newThing];	
//	[newThing autorelease];	
	[nsobjects addObject: newThing];
	//receiver set dataptr
	SetPtr(receiver->uo->slots + 0, newThing);
    return errNone;
}

int prObjcSetActionForControl(struct VMGlobals *g, int numArgsPushed);
int prObjcSetActionForControl(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 2;
    PyrSlot *b  = g->sp - 1;
	PyrSlot *c  = g->sp;
	id objc = (id) a->uo->slots[0].ui;
	if(!objc) return errFailed;
	NSString * actionname = nsStringFromPyrSlot(c);
	if([actionname length] < 1){  return errFailed; }
	SEL selAc = NSSelectorFromString(actionname);
		
	if(![[objc  class] instancesRespondToSelector:@selector(setAction:)])
	{ 
		post("Warning: %s does not respond to setAction:.\n", [[objc className] UTF8String]); 
		SetNil(b); 
		return errNone;
	}
	SCCocoaToLangAction *action = [[SCCocoaToLangAction alloc] init];
	[action setSCObject: b->uo];

	[objc setAction: selAc];
	[objc setTarget: action];
	[action retain];
//	[nsAutoReleasePool addObject: action];
	[nsobjects addObject: action];

	SetPtr(b->uo->slots + 0, action);	
    return errNone;
}

int prObjcSetDelegate(struct VMGlobals *g, int numArgsPushed);
int prObjcSetDelegate(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1;
    PyrSlot *b  = g->sp;
	id objc = (id) a->uo->slots[0].ui;
	if(!objc) return errFailed;
	if(![[objc  class] instancesRespondToSelector:@selector(setDelegate:)])
	{ 
		post("Warning: %s does not respond to setDelegate:.\n", [[objc className] UTF8String]); 
		SetNil(b); 
		return errNone;
	}	
	SCCocoaToLangAction *action = [[SCCocoaToLangAction alloc] init];
	[action setSCObject: b->uo];
	[objc setDelegate: action];
	[action retain];
//	[nsAutoReleasePool addObject: action];
	[nsobjects addObject: action];

	SetPtr(b->uo->slots + 0, action);	
    return errNone;
}

int prObjcAllocSend(struct VMGlobals *g, int numArgsPushed);
int prObjcAllocSend(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *receiver = g->sp - 4;
    PyrSlot *classname = g->sp - 3;
	PyrSlot *initname = g->sp - 2;
    PyrSlot *args  = g->sp-1;
    PyrSlot *defer  = g->sp;
	
	
	bool hasArg = NotNil(args);
	bool isCollection = isKindOfSlot(args, class_array);

	NSString * nsclassname = nsStringFromPyrSlot(classname);
	if([nsclassname length] < 1){ return errFailed; }
	NSString * nsinitname = nsStringFromPyrSlot(initname);
	if([nsinitname length] < 1){  return errNone; }

	SEL sel = NSSelectorFromString(nsinitname);
	
//	NSMethodSignature *sig = [[NSClassFromString( nsclassname ) class]  instanceMethodSignatureForSelector: sel];

	id nsclass = objc_getClass(classname->uos->s);
	
	int val;
	slotIntVal(args->uo->slots+0, &val);
	id newThing = objc_msgSend(nsclass, sel, val);

	SetPtr(receiver->uo->slots + 0, newThing);
    return errNone;
}

int prObjcGetClassName(struct VMGlobals *g, int numArgsPushed);
int prObjcGetClassName(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1;
	PyrSlot *b  = g->sp;
	id objc = (id) a->uo->slots[0].ui;
	if(!objc) return errFailed;	
	PyrString *pyrPathString = newPyrString(g->gc, [[objc className] UTF8String], 0, true);
	SetObject(a, pyrPathString);
	return errNone;
}

int prObjcDeferInvocation(struct VMGlobals *g, int numArgsPushed);
int prObjcDeferInvocation(struct VMGlobals *g, int numArgsPushed)
{
    PyrSlot *receiver  = g->sp - 3;
    PyrSlot *initname = g->sp - 2;
	PyrSlot *args = g->sp - 1;
    PyrSlot *defer  = g->sp;

    if (!g->canCallOS) return errCantCallOS; //moght be better to check this ?
	
	bool hasArg = NotNil(args) && isKindOfSlot(args, class_array);
	bool isCollection = isKindOfSlot(args, class_array);
	if(!isCollection) return errFailed;
    // have to put them in a specific pool so they can be released on command
//	NSString * nsclassname = nsStringFromPyrSlot(classname);
	
//	if([nsclassname length] < 1){ return errFailed; }
	
    // must defer in case of large loads and gui
	id newThing = (id) receiver->uo->slots[0].ui;
    if (!newThing) return errFailed;
	NSString * nsclassname = [newThing className];
	NSString * nsinitname = nsStringFromPyrSlot(initname);
	if([nsinitname length] < 1){ return errFailed; }
	SEL sel = NSSelectorFromString(nsinitname);
	if(![[NSClassFromString( nsclassname ) class] instancesRespondToSelector:sel]) 
	{
		post("Warning: %s does not respond to: %s \n", [nsclassname cString], [nsinitname cString]);
		return errNone;
	} 
	NSMethodSignature *sig = [[NSClassFromString( nsclassname ) class]  instanceMethodSignatureForSelector: sel];
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: newThing];
    [anInvocation setSelector: sel];

	int numberOfArgs = (int) [sig numberOfArguments] - 2; //cocoa args start at 2
	int len = args->uo->size;
	
	if(len < numberOfArgs) return errFailed;
	if(len > numberOfArgs) len = numberOfArgs;

	if(hasArg) 
	{
		int err = nsinvocationSetArguments(args, &anInvocation);
		if(err) return err;
	}

	if(IsTrue(defer)){
		[anInvocation retainArguments];       
		[scvm defer: anInvocation];
//		[anInvocation retain];
//		SetPtr(receiver, anInvocation); //test for number ..
	}
	else
	{
		unsigned int length = [[anInvocation methodSignature] methodReturnLength];
		char *cType = (char *)[[anInvocation methodSignature] methodReturnType];		
		//?use void objc_msgSend_stret(void * stretAddr, id theReceiver, SEL theSelector, ...);

		[anInvocation invoke];
		float fval;
		int ival;
		double dval;
		char cval;
		long lval;
		id idval;
		char * str;
		PyrString* pyrPathString;	
//		PyrObject* array;
		switch(*cType){
			case _C_FLT:
				[anInvocation getReturnValue: (void*)&fval];				
				SetFloat(receiver, (float) fval);
				break;
			case _C_DBL:
				[anInvocation getReturnValue: (void*)&dval];				
				SetFloat(receiver, (float) dval);
				break;
			case  _C_INT:
				[anInvocation getReturnValue: (void*)&ival];				
				SetInt(receiver, (int) ival);
				break;
			case _C_UINT:
				[anInvocation getReturnValue: (void*)&ival];				
				SetInt(receiver, (int) ival);
				break;
			case _C_LNG:
				[anInvocation getReturnValue: (void*)&lval];				
				SetInt(receiver, (int) lval);
				break;					
			case _C_ULNG:
				[anInvocation getReturnValue: (void*)&lval];				
				SetInt(receiver, (int) lval);
				break;
			case _C_CHARPTR:
				str = (char*) malloc(length * sizeof(char));
				[anInvocation getReturnValue: (void*)&str];
				pyrPathString = newPyrString(g->gc,str,0,true);
				SetObject(receiver,pyrPathString);
				break;
			case 'r':
				str = (char*) malloc(length);
				[anInvocation getReturnValue: (void*)&str];
				pyrPathString = newPyrString(g->gc,str,0,true);
				SetObject(receiver,pyrPathString);
				break;
			case _C_ID:
				[anInvocation getReturnValue: (void*)&idval];	
				[idval retain];			
				[nsobjects addObject: idval];
				SetPtr(receiver, (id) idval); 	
//				array = newPyrArray(g->gc, 2, 0, true);
//				SetObject(receiver, array);
//	
//				pyrPathString = newPyrString(g->gc, [[idval className] UTF8String], 0, true);
//				SetObject(array->slots + array->size, pyrPathString);
//				array->size++;
//				g->gc->GCWrite(array, pyrPathString);
//				SetPtr(array->slots + array->size, idval);
//				array->size++;
				break;	
					
			case _C_VOID: break;
			case _C_CHR:
				[anInvocation getReturnValue: (void*)&cval];				
				SetChar(receiver, (char) cval);
				break;
			case _C_UCHR:
				[anInvocation getReturnValue: (void*)&cval];				
				SetChar(receiver, (char) cval);
				break;
			default: post("Warning return type %s not recognized.\n", cType); break;
		}		
	}
    return errNone;
}



int prObjcDealloc(struct VMGlobals *g, int numArgsPushed);
int prObjcDealloc(struct VMGlobals *g, int numArgsPushed)
{
    //PyrSlot *receiver = g->sp - 1;
    PyrSlot *dataptr = g->sp;
    
    // have to put them in a specific pool so they can be released on command
    if(! IsInt(dataptr) ) return errWrongType;
    
    // check retain count and return in case
    [(id)dataptr->ui release];
    return errNone;
}


void initCocoaBridgePrimitives()
{
    int base, index;
    
    base = nextPrimitiveIndex();
    index = 0;
	s_nsObject = getsym("SCNSObject");
//	if(nsAutoReleasePool) [nsAutoReleasePool release]; 
//	nsAutoReleasePool = [[NSAutoreleasePool alloc] init];
	if(nsobjects)
	{
		NSMutableArray *copy = [nsobjects copy];
        [nsobjects removeAllObjects];
        [copy makeObjectsPerformSelector: @selector(release)];
        [copy release];
		[nsobjects release];
		nsobjects = NULL;
	}
	nsobjects = [NSMutableArray arrayWithCapacity: 8];
	[nsobjects retain];
	
    definePrimitive(base, index++, "_ObjC_AllocInit", prObjcAllocInit, 5, 0);	
    definePrimitive(base, index++, "_ObjC_Dealloc", prObjcDealloc, 2, 0);	
    definePrimitive(base, index++, "_ObjC_Invoke", prObjcDeferInvocation, 4, 0);
	definePrimitive(base, index++, "_ObjC_GetClassName", prObjcGetClassName, 2, 0);	
    definePrimitive(base, index++, "_ObjC_SetActionForControl", prObjcSetActionForControl, 3, 0);	
    definePrimitive(base, index++, "_ObjC_SetDelegate", prObjcSetDelegate, 2, 0);	
    definePrimitive(base, index++, "_ObjC_AllocSend", prObjcAllocSend, 5, 0);	

// loadNibFile
}
