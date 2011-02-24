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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */


#import <Cocoa/Cocoa.h>
#import <objc/objc-runtime.h>
#import <objc/Object.h>
#import <QTKit/QTTime.h>
#import <QTKit/QTTimeRange.h>
#import <Foundation/NSMethodSignature.h>
#import "SCBase.h"
#import "PyrSymbol.h"
#import "PyrPrimitive.h"
#import "PyrObject.h"
#import "PyrKernel.h"
#import "VMGlobals.h"
#import "MyDocument.h"
#import "GC.h"
#import "SCVirtualMachine.h"

#if (MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_4)
#define class_getName(a)	((a)->name)
#endif

#define COCOABRIDGE_VERBOSITY						0
#define COCOABRIDGE_STORE_OBJECTS_IN_ARRAY			1 // Conditional for storing all created objects in an NSArray. For future test with special Pool.
#define COCOABRIDGE_AUTOCONVERT_NSSTRING			1 /* for backward compat. set to 0.
														Used only for converting NSString to PyrString on INVOCATION RETURN.
														PyrString are automatically converted to NSString when passed as arguments
                                                      */
#if COCOABRIDGE_VERBOSITY
	#define COCOABRIDGE_OBJ_RETAIN_LOG(nsobj) NSLog(@"OBJ_RETA NSObject(%p), type(%@)", nsobj, [nsobj class]);
	#define COCOABRIDGE_OBJ_RELEASE_LOG(nsobj) NSLog(@"OBJ_RELE NSObject(%p), type(%@)", nsobj, [nsobj class]);
	#define COCOABRIDGE_POOL_ADD_LOG(nsobj) NSLog(@"POOL_ADD NSObject(%p), type(%@), POOL_OBJ_NUMBER: %i", nsobj, [nsobj class], [nsobjects count]);
	#define COCOABRIDGE_POOL_REM_LOG(nsobj)	NSLog(@"POOL_REM NSObject(%p), type(%@), POOL_OBJ_NUMBER: %i", nsobj, [nsobj class], [nsobjects count]-1);
#else
	#define COCOABRIDGE_OBJ_RETAIN_LOG(nsobj) ;
	#define COCOABRIDGE_OBJ_RELEASE_LOG(nsobj) ;
	#define COCOABRIDGE_POOL_ADD_LOG(nsobj) ;
	#define COCOABRIDGE_POOL_REM_LOG(nsobj) ;
#endif

#if COCOABRIDGE_STORE_OBJECTS_IN_ARRAY
# define COCOABRIDGE_POOL_ADD(nsobj)	\
	do {	\
		[nsobjects addObject: nsobj];	\
		COCOABRIDGE_POOL_ADD_LOG(nsobj)	\
	} while(0);

# define COCOABRIDGE_POOL_REM(nsobj)	\
	do {	\
		COCOABRIDGE_POOL_REM_LOG(nsobj)	\
		[nsobjects removeObject: nsobj];	\
	} while(0);
#else
# define COCOABRIDGE_POOL_ADD(nsobj)
# define COCOABRIDGE_POOL_REM(nsobj)
#endif

#define COCOABRIDGE_OBJ_RETAIN(nsobj)	\
	do {	\
		/*[nsobj retain];*/	\
		/*COCOABRIDGE_OBJ_RETAIN_LOG(nsobj)*/	\
		COCOABRIDGE_POOL_ADD(nsobj)	\
	}while(0)

#define COCOABRIDGE_OBJ_RELEASE(nsobj)	\
	do {	\
		/*COCOABRIDGE_OBJ_RELEASE_LOG(nsobj)*/	\
		COCOABRIDGE_POOL_REM(nsobj)	\
		/*[nsobj release];*/	\
	}while(0)

#define TryInvocation(anInvocation, receiver)	\
	@try {	\
		[anInvocation invoke];	\
	}@catch(NSException* e){	\
			post("ERROR: CocoaBridge exception caught: %s %s\n", [[e name]UTF8String], [[e reason]UTF8String]);	\
			SetNil(receiver);	\
			return errFailed;	\
	}	\


PyrSymbol *s_nsObject;
//NSAutoreleasePool *nsAutoReleasePool;

#if COCOABRIDGE_STORE_OBJECTS_IN_ARRAY
//NSMutableSet *nsobjects = nil;
NSMutableArray *nsobjects = nil;
#endif


inline NSString* nsStringFromPyrSlot(PyrSlot *slot)
{
	if(IsSym(slot)) {
		NSString* retval;
		int len = strlen(slotRawSymbol(slot)->name);
		char* symstr = (char*)malloc(len+1);
		strcpy(symstr, slotRawSymbol(slot)->name);
#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_4
		retval = [NSString stringWithCString:symstr length:len]; // Deprecated 10.4
#else
		retval = [NSString stringWithCString:symstr encoding:NSASCIIStringEncoding];
#endif
		free(symstr);
		return retval;
	}
    else {
		PyrString* pyrString = slotRawString(slot);
#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_4
		return [NSString stringWithCString: pyrString->s length: pyrString->size]; // Deprecated 10.4
#else
		char* symstr = (char*)malloc(pyrString->size + 1);
		memcpy(symstr, pyrString->s, pyrString->size); symstr[pyrString->size] = 0;
		NSString *retval = [NSString stringWithCString:symstr encoding:NSASCIIStringEncoding];
		free(symstr);
		return retval;
#endif
	}
}


inline int ObjcTypeFillPyrSlot(PyrSlot* slot, const char* type, void* value, PyrObjectHdr** retPyrObject ) {
	VMGlobals *g = gMainVMGlobals;

	if(retPyrObject)
		*retPyrObject = NULL;

	if(*type == 'r') ++type;
	switch(*type) {
		case _C_ID:
			{
				id idval = *(id*)value;
				if([idval isKindOfClass:[NSColor class]]) {
					//NSLog(@"ObjcTypeFillPyrSlot [_NSCOLOR]");
					NSColor* nscol = [idval colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
					PyrObject *color = instantiateObject(g->gc, s_color->u.classobj, 0, false, true);
					PyrSlot *slots = color->slots;
					SetFloat(slots+0, [nscol redComponent]);
					SetFloat(slots+1, [nscol greenComponent]);
					SetFloat(slots+2, [nscol blueComponent]);
					SetFloat(slots+3, [nscol alphaComponent]);
					SetObject(slot, color);
					if(retPyrObject)
						*retPyrObject = color;
				}
#if COCOABRIDGE_AUTOCONVERT_NSSTRING
				else if([idval isKindOfClass:[NSString class]]) {
					//NSLog(@"ObjcTypeFillPyrSlot [_NSSTRING]");
					NSString* string = (NSString*)idval;
					PyrString* pyrPathString = newPyrString(g->gc,[string UTF8String],0,true);
					SetObject(slot, pyrPathString);
					if(retPyrObject)
						*retPyrObject = pyrPathString;
				}
#endif
				else {
					//NSLog(@"ObjcTypeFillPyrSlot [_C_ID]");
					if(idval) {
						COCOABRIDGE_OBJ_RETAIN(idval);
						SetPtr(slot, (id) idval);
					}else
						SetNil(slot);
				}
			}
			break;

		case _C_VOID:
		case _C_UNDEF:
			//NSLog(@"ObjcTypeFillPyrSlot [_C_VOID | _C_UNDEF]");
			SetNil(slot);
			break;

		case _C_CHR:
		case _C_UCHR:
			//NSLog(@"ObjcTypeFillPyrSlot [_C_CHR | _C_UCHAR]");
			//SetChar(slot, *(char*)value);
			SetInt(slot, *(char*)value);
			break;


		case _C_LNG:
		case _C_ULNG:
			{
			//NSLog(@"ObjcTypeFillPyrSlot [_C_LONG...]");
			long sval = *(long*)value;
			SetInt(slot, sval);
			}
			break;

		case _C_SHT:
		case _C_USHT:
			{
			//NSLog(@"ObjcTypeFillPyrSlot [_C_SHORT...]");
			short sval = *(short*)value;
			SetInt(slot, sval);
			}
			break;

		case _C_INT:
		case _C_UINT:
			{
			//NSLog(@"ObjcTypeFillPyrSlot [_C_INT...]");
			SetInt(slot, *(int*)value);
			}
			break;

		case _C_FLT:
			{
			float fval = *(float*)value;
			SetFloat(slot, fval);
			}
			break;

		case _C_DBL:
			{
			//NSLog(@"ObjcTypeFillPyrSlot [_C_FLT...]");
			double dval = *(double*)value;
			SetFloat(slot, dval);
			}
			break;

		case 'B':
			{
			//NSLog(@"ObjcTypeFillPyrSlot [BOOLEAN...]");
			bool b = *(bool*)value;
			if(b)
				SetTrue(slot);
			else
				SetFalse(slot);
			}
			break;

		case _C_BFLD:
			//NSLog(@"ObjcTypeFillPyrSlot [_C_BFLD BITFIELD...]");
			SetNil(slot);
			break;

		case _C_PTR:
			//NSLog(@"ObjcTypeFillPyrSlot [_C_CHR | _C_UCHAR]");
			SetPtr(slot, *(void**)value);
			break;

		case _C_SEL:
			{
			//NSLog(@"ObjcTypeFillPyrSlot [_C_SEL]");
			SEL sel = *(SEL*)value;
			PyrString* pyrPathString = newPyrString(g->gc, sel_getName(sel), 0, true);
			SetObject(slot, pyrPathString);
			if(retPyrObject)
				*retPyrObject = pyrPathString;
			}
			break;

		case _C_CHARPTR:
			{
			//NSLog(@"ObjcTypeFillPyrSlot [_C_CHARPTR | r]");
			char* str = *(char**)value;
			PyrString* pyrPathString = newPyrString(g->gc, str,0,true);
			SetObject(slot, pyrPathString);
			if(retPyrObject)
				*retPyrObject = pyrPathString;
			}
			break;

		case _C_STRUCT_B:
			{
				/*
				 support only for basic types.
				 TODO: plug some functions of the PyObjc to parse structure types.
				 */

				if(strncmp(type+1, "_NSSize", 7) == 0) {
					//NSLog(@"ObjcTypeFillPyrSlot [_NSSize]");
					// return an array
					PyrObject		*array = ::instantiateObject(g->gc, s_array->u.classobj, 2, true, true);
					if(!array) return errOutOfMemory;
					NSSize			s = *(NSSize*)value;
					SetFloat(array->slots+0, s.width);
					SetFloat(array->slots+1, s.height);
					SetObject(slot, array);
					if(retPyrObject)
						*retPyrObject = array;

				}else if(strncmp(type+1, "_NSRange", 8) == 0) {
					//NSLog(@"ObjcTypeFillPyrSlot [_NSRange]");
					// return an array
					PyrObject		*array = ::instantiateObject(g->gc, s_array->u.classobj, 2, true, true);
					if(!array) return errOutOfMemory;
					NSRange			r = *(NSRange*)value;
					SetInt(array->slots+0, r.location);
					SetInt(array->slots+1, r.length);
					SetObject(slot, array);
					if(retPyrObject)
						*retPyrObject = array;
				}else if(strncmp(type+1, "_NSRect", 7) == 0) {
					//NSLog(@"ObjcTypeFillPyrSlot [_NSRect]");
					// return a SCRect
					PyrSlot			*slots;
					PyrObject		*rect = instantiateObject(g->gc, s_rect->u.classobj, 0, false, true);
					NSRect			r = *(NSRect*)value;

					slots = rect->slots;
					SetFloat(slots+0, r.origin.x);
					SetFloat(slots+1, r.origin.y);
					SetFloat(slots+2, r.size.width);
					SetFloat(slots+3, r.size.height);
					SetObject(slot, rect);
					if(retPyrObject)
						*retPyrObject = rect;
				}else if(strncmp(type+1, "_NSPoint", 8) == 0) {
					//NSLog(@"ObjcTypeFillPyrSlot [_NSPoint]");
					// return a SCPoint
					PyrSlot			*slots;
					PyrObject		*point = instantiateObject(g->gc, s_point->u.classobj, 0, false, true);
					NSPoint			p = *(NSPoint*)value;

					slots = point->slots;
					SetFloat(slots+0, p.x);
					SetFloat(slots+1, p.y);
					SetObject(slot, point);
					if(retPyrObject)
						*retPyrObject = point;
				}else if(strncmp(type, "{?=qll}", 7) == 0) { // QTTime
					PyrObject		*pyrarray = instantiateObject(g->gc, s_array->u.classobj, 3, true, true);
					if(!pyrarray) return errOutOfMemory;
					QTTime			p = *(QTTime*)value;
					SetInt(pyrarray->slots+0, (int)p.timeValue);
					SetInt(pyrarray->slots+1, (int)p.timeScale);
					SetInt(pyrarray->slots+2, (int)p.flags);
					SetObject(slot, pyrarray);
					if(retPyrObject)
						*retPyrObject = pyrarray;
				}else{
					//NSLog(@"ObjcTypeFillPyrSlot [_C_STRUCT]");
					SetNil(slot);
					post("Warning: CocoaBridge objc structure type not supported.\n");
				}
				//cType[ptr-cType-1] = '=';
			}
			break;

		case _C_ARY_E:
		case _C_UNION_E:
		case _C_STRUCT_E:
			//NSLog(@"ObjcTypeFillPyrSlot [_C_STRUCT | _C_UNION | _C_ARY_E]");
			return errNone;

		// not handled
		case _C_CLASS:
		case _C_ARY_B:
		case _C_UNION_B:
		//case _C_STRUCT_B:
		default:
			SetNil(slot);
			post("ERROR: ObjcType conversion not handled: %c!\n", *type);
			return errWrongType;
	}

	return errNone;
}


//put this in a .h
@interface SCCocoaToLangAction : NSResponder
{
    struct PyrObject *mSCObject;
	NSMutableDictionary	 *mSelectorArray;
}


- (void)setSCObject: (struct PyrObject*)inObject;
- (struct PyrObject*)getSCObject;
- (void) doAction: (id) sender;
- (void) doFloatAction: (id) sender;
- (void) doIntAction: (id) sender;
- (void) doStateAction: (id) sender;
- (void) doFloatArrayAction: (id) sender;
- (void) doSCObjectAction: (id) sender;

- (void) keyUp: (NSEvent*) event;
- (void) keyDown: (NSEvent*) event;
- (void) mouseDown: (NSEvent*) event;
- (void)windowWillClose:(NSNotification *)aNotification;
- (void)notificationReceived:(NSNotification*)aNotification;

- (void)addSelector:(SEL)aSelector withObjcTypes:(const char*)cTypes;
- (void)removeSelector:(SEL)aSelector;
//- forward: (SEL)sel : (marg_list)args;
@end
//

extern bool compiledOK;

@implementation SCCocoaToLangAction

-(id)init {
	if([super init]) {
		mSelectorArray = [[NSMutableDictionary allocWithZone:[self zone]]initWithCapacity:16];
		return self;
	}
	return nil;
}
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
//
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
- (void) doArrayAction: (id) sender
{
    pthread_mutex_lock (&gLangMutex);
	PyrObject * pobj = [self getSCObject];
	if(compiledOK && pobj){
		PyrSymbol *method = getsym("doAction");
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, pobj);
		//get Size
		size_t size = (size_t) [sender arraySize];
		float *src = (float*) [sender floatArray];
		PyrDoubleArray* array = newPyrDoubleArray(g->gc, size, 0 , true);
		for(int i=0; i<size; i++){
			array->d[i] = src[i];
		}
		++g->sp;  SetObject(g->sp, array);
		runInterpreter(g, method, 2);
		g->canCallOS = false;
	}
    pthread_mutex_unlock (&gLangMutex);

}

/* experimental ! */
- (void) doFloatArrayAction: (id) sender
{
    pthread_mutex_lock (&gLangMutex);
	PyrObject * pobj = [self getSCObject];
	if(compiledOK && pobj){
		PyrSymbol *method = getsym("doAction");
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, pobj);
		//get Size
		size_t size = (size_t) [sender arraySize];
		float *src = (float*) [sender floatArray];
		PyrDoubleArray* array = newPyrDoubleArray(g->gc, size, 0 , true);
		for(int i=0; i<size; i++){
			array->d[i] = src[i];
		}
		++g->sp;  SetObject(g->sp, array);
		runInterpreter(g, method, 2);
		g->canCallOS = false;
	}
    pthread_mutex_unlock (&gLangMutex);
}
/* experimental ! */
- (void) doSCObjectAction: (id) sender
{
    pthread_mutex_lock (&gLangMutex);
	PyrObject * pobj = [self getSCObject];
	if(compiledOK && pobj){
		PyrSymbol *method = getsym("doAction");
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, pobj);
		//push values from sender
		PyrObject* reslut = (PyrObject*) [sender valueWith: self];

		++g->sp;  SetObject(g->sp, reslut);
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

- (void)dealloc {
	mSCObject = NULL;
	[mSelectorArray release];
	[[NSNotificationCenter defaultCenter]removeObserver:self]; // in case
	[super dealloc];
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


//// added thelych
#pragma mark __SCCOCOATOLANG_FORWARD___
- (void)notificationReceived:(NSNotification*)aNotification
{
	//post("notification received by delegate\n");
	pthread_mutex_lock (&gLangMutex);
	PyrObject * pobj = [self getSCObject];
	if(compiledOK && pobj){
		PyrSymbol *method = getsym("doNotificationAction");
		VMGlobals *g = gMainVMGlobals;

		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, pobj);

		PyrString* pyrString = newPyrString(g->gc,[[aNotification name] cString],0,true);
		++g->sp;  SetObject(g->sp, pyrString);
		++g->sp;  SetPtr(g->sp, (void*)aNotification);
		++g->sp;  SetPtr(g->sp, (void*)[aNotification object]);

		runInterpreter(g, method, 4);
		g->canCallOS = false;
	}
    pthread_mutex_unlock (&gLangMutex);
}

/*
- (void*)forward: (SEL)sel : (marg_list)args {
	NSLog(@"Forward invoked with Selector %s ", sel_getName(sel));
	[super forward:sel :args];
}
*/

-(NSMethodSignature*)methodSignatureForSelector:(SEL)aSel {
	return (NSMethodSignature*)[mSelectorArray objectForKey:[NSNumber numberWithLong:(long)aSel]];
}

-(BOOL)respondsToSelector:(SEL)sel {
	//post("trying %s ", sel_getName(sel));
	NSEnumerator *e = [mSelectorArray keyEnumerator];
	NSNumber* obj=nil;
	while( (obj = (NSNumber*)[e nextObject]) ) {
		SEL thesel = (SEL)[obj longValue];
		if(sel == thesel) {
			return YES;
		}
	}
	return [super respondsToSelector:sel];
}

-(void)forwardInvocation:(NSInvocation*)inv {

	SEL selector = [inv selector];
	const char* retType = [[inv methodSignature]methodReturnType];
	const char* csel = sel_getName(selector);

	pthread_mutex_lock (&gLangMutex);
	PyrObject * pobj = [self getSCObject];
	if(compiledOK && pobj){

		NSMethodSignature* nsSignature = [inv methodSignature];
		int maxArgs = [nsSignature numberOfArguments];

		PyrSymbol *method = getsym("doDelegateAction");
		VMGlobals *g = gMainVMGlobals;

		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, pobj);

		PyrString* pyrString = newPyrString(g->gc, csel, 0, true);
		++g->sp;  SetObject(g->sp, pyrString);
		void* value;

		//NSLog(@"NSInvocation Name: %s has %i number of arguments", csel, maxArgs);

		++g->sp;
		if(maxArgs - 2 > 0) {
			PyrObject*		array;
			PyrObjectHdr*	pyrObject;

			//array = newPyrArray(g->gc, 2, 0, true);
			array = ::instantiateObject(g->gc, s_array->u.classobj, maxArgs-2, true, true);
			SetObject(g->sp, array);

			PyrSlot* slots = array->slots;
			for(int i=2; i < maxArgs; ++i)
			{
				[inv getArgument:(void*)&value atIndex:i];
				ObjcTypeFillPyrSlot(slots++, [nsSignature getArgumentTypeAtIndex:i], (void*)&value, &pyrObject);
				if(pyrObject) {
					g->gc->GCWrite(array, pyrObject);
				}
			}
		}
		else
			SetNil(g->sp);

		runInterpreter(g, method, 3);

		g->canCallOS = false;
		if(*retType != _C_VOID && *retType != _C_UNDEF) {
			//NSLog(@"Setting NSInvocation Return Type");
			//[inv retainArguments];

			g = gMainVMGlobals;
			PyrSlot* slot = &(g->result);
			void * retbuffer=NULL;

			if(*retType == 'r') ++retType;
			switch(*retType) {
				case _C_ID:
				{
					//NSLog(@"Setting [_C_ID]");
					id retval = nil;
					if(
					   isKindOfSlot(slot, s_nsObject->u.classobj) ||
					   isKindOfSlot(slot, slotRawSymbol(&s_nsObject->u.classobj->superclass)->u.classobj)
					) {
						//NSLog(@"Slot is a NSObject");
						retval = (id)slotRawPtr(slotRawObject(slot)->slots);
					}
					else if(isKindOfSlot(slot, class_string) || IsSym(slot)) {
						//NSLog(@"Slot is a string");
						retval = (id)nsStringFromPyrSlot(slot);
					}
					else if(IsPtr(slot) ) {
						//NSLog(@"Slot is a Pointer");
						retval = (id)slotRawPtr(slot);
					}
					else if(IsFloat(slot)) {
						//NSLog(@"Slot is a Float");
						float f; NSNumber *n;
						slotFloatVal(slot, &f);
						n = [NSNumber numberWithFloat:f];
						retval = (id)n;
					}
					else if(IsInt(slot)) {
						//NSLog(@"Slot is an Int");
						int i; NSNumber *n;
						slotIntVal(slot, &i);
						n = [NSNumber numberWithInt:i];
						retval = (id)n;
					}
					else if(isKindOfSlot(slot, s_color->u.classobj)) {
						//NSLog(@"Slot is a Color");
						float r, g, b, a;
						PyrSlot *slots = slotRawObject(slot)->slots;
						slotFloatVal(slots+0, &r);
						slotFloatVal(slots+1, &g);
						slotFloatVal(slots+2, &b);
						slotFloatVal(slots+3, &a);
						NSColor *color = [NSColor colorWithCalibratedRed:r green:g blue:b alpha:a];
						retval = (id)color;
					}
					else
						post("ERROR: Cocoa Bridge wrong type. expected %c. Set to NIL\n", _C_ID);
					retbuffer = (void*)&retval;
				}
				break;

				case _C_UCHR:
				case _C_CHR:
				{
					//NSLog(@"Setting [_C_CHR]");
					char retval = 0;
					if( IsChar(slot) )
						retval = slotRawChar(slot);
					else
						post("ERROR: Cocoa Bridge wrong type. expected %c. Set to 0\n", _C_CHR);
					retbuffer = (void*)&retval;
				}
				break;

				case _C_SHT:
				case _C_USHT: // same size PPC, PPC64, IA32...ect...
				{
					//NSLog(@"Setting [_C_SHT]");
					int retval = 0; short shortval;
					if( IsTrue(slot) )
						retval = 1;
					else if( IsFalse(slot) )
						retval = 0;
					else if(slotIntVal(slot, &retval)  != errNone)
						post("ERROR: Cocoa Bridge wrong type. expected %c\n", _C_SHT);
					shortval = retval;
					retbuffer  = (void*)&shortval;
				}

				case _C_INT:
				case _C_UINT: // same size PPC, PPC64, IA32...ect...
				{
					//NSLog(@"Setting [_C_INT]");
					int retval = 0;
					if( IsTrue(slot) )
						retval = 1;
					else if( IsFalse(slot) )
						retval = 0;
					else if(slotIntVal(slot, &retval)  != errNone)
						post("ERROR: Cocoa Bridge wrong type. expected %c\n", _C_INT);
					retbuffer  = (void*)&retval;
				}
				break;

				case _C_LNG:
				case _C_ULNG: // same size PPC, PPC64, IA32...ect...
				{
					//NSLog(@"Setting [_C_LNG]");
					int retval = 0; long longval;
					if( IsTrue(slot) )
						retval = 1;
					else if( IsFalse(slot) )
						retval = 0;
					else if(slotIntVal(slot, &retval)  != errNone)
						post("ERROR: Cocoa Bridge wrong type. expected %c\n", _C_LNG);
					longval = (long)retval;
					retbuffer  = (void*)&longval;
				}
				break;

				case _C_FLT:
				{
					//NSLog(@"Setting [_C_FLT]");
					float retval = 0;
					if(slotFloatVal(slot, &retval) != errNone)
						post("ERROR: Cocoa Bridge wrong type. expected %c\n", _C_FLT);
					retbuffer = (void*)&retval;
				}
				break;

				case _C_DBL:
				{
					//NSLog(@"Setting [_C_DBL]");
					double retval = 0;
					if(slotDoubleVal(slot, &retval) != errNone)
						post("ERROR: Cocoa Bridge wrong type. expected %c\n", _C_DBL);
					retbuffer = (void*)&retval;
				}
				break;

				case _C_PTR:
				{
					//NSLog(@"Setting [_C_PTR]");
					void* ptr = 0;
					if(IsPtr( slot )) {
						ptr = slotRawPtr(slot);
						retbuffer = &ptr;
					}
					else
						post("ERROR: Cocoa Bridge wrong type. expected %c\n", _C_PTR);
				}
				break;

				case _C_CHARPTR:
				{
					//NSLog(@"Setting [_C_CHARPTR | STRING]");
					int size = 0;
					char *retval = "";

					if ( IsSym(slot) )
						size = strlen(slotRawSymbol(slot)->name);
					else if( isKindOfSlot(slot, class_string) ) {
						size = ((PyrString*)slotRawObject(slot))->size;
					}
					else {
						post("ERROR: Cocoa Bridge wrong type. expected %c. return ("")\n", _C_CHARPTR);
						retbuffer = (void*)&retval;
						break;
					}

					retval = (char*)malloc(sizeof(char) * (size+1));
					slotStrVal(slot, retval, size);
					retbuffer = (void*)&retval;
				}
				break;

				case _C_STRUCT_B:
				{
					/*
					 support only for basic types.
					 TODO: plug some functions of the PyObjc to parse structure types.
					 */

					char * ptr, * cType;
					cType = (char*)retType;
					ptr = strchr(retType, '=') + 1;
					cType[ptr-cType-1] = '\0';

					if( (strcmp(cType+1, "_NSSize") == 0) && (
							isKindOfSlot(slot, s_point->u.classobj) ||
							isKindOfSlot(slot, class_array))
					) {
						PyrSlot *slots = slotRawObject(slot)->slots;
						NSSize s;
						slotVal(slots+0, &s.width);
						slotVal(slots+1, &s.height);
						retbuffer = (void*)&s;
					}
					else if((strcmp(cType+1, "_NSRange") == 0) && (
							isKindOfSlot(slot, s_point->u.classobj) ||
							isKindOfSlot(slot, class_array))
					) {
						PyrSlot *slots = slotRawObject(slot)->slots;
						NSRange s;
						slotIntVal(slots+0, (int*)&s.location);
						slotIntVal(slots+1, (int*)&s.length);
						retbuffer = (void*)&s;
					}
					else if((strcmp(cType+1, "_NSRect") == 0) && (
							isKindOfSlot(slot, s_rect->u.classobj) ||
							isKindOfSlot(slot, class_array))
					){
						PyrSlot *slots = slotRawObject(slot)->slots;
						NSRect s;
						slotVal(slots+0, &(s.origin.x));
						slotVal(slots+1, &(s.origin.y));
						slotVal(slots+2, &(s.size.width));
						slotVal(slots+3, &(s.size.height));
						retbuffer = (void*)&s;
					}
					else if((strcmp(cType+1, "_NSPoint") == 0) && (
							isKindOfSlot(slot, s_point->u.classobj) ||
							isKindOfSlot(slot, class_array))
					) {
						PyrSlot *slots = slotRawObject(slot)->slots;
						NSPoint s;
						slotVal(slots+0, &s.x);
						slotVal(slots+1, &s.y);
						retbuffer = (void*)&s;
					}else{
						post("ERROR: Warning structure type not supported %c.\n", _C_STRUCT_B);
					}
					cType[ptr-cType-1] = '=';
				}
				break;

				//// Not handled
				case _C_BFLD:
				case _C_CLASS:
				case _C_SEL:
				case _C_ARY_B:
				case _C_UNION_B:
				case _C_UNION_E:
				case _C_ARY_E:
				case _C_STRUCT_E:
				default:
					NSLog(@"ERROR: UNHANDLED RETURN TYPE OF INVOCATION: %c", *retType);
				//post("return type not handled");
				break;
			}

			//NSLog(@"Returning NSInvocation");
			[inv setReturnValue:retbuffer];
		} // End Of RetValue
	} // End Of Compiled ok
    pthread_mutex_unlock (&gLangMutex);
}

-(void)dummy{
	NSLog(@"Dummy");
}

- (void)addSelector:(SEL)aSelector withObjcTypes:(const char*)cTypes
{
	NSNumber* n = [NSNumber numberWithLong:(long)aSelector];
	NSMethodSignature *signature;

	if(cTypes && strlen(cTypes) > 0)
		signature = [NSMethodSignature signatureWithObjCTypes:cTypes]; // defined in <Foundation/NSMethodSignature>
	else
		signature = [[self class] instanceMethodSignatureForSelector:@selector(dummy)];

	if(!signature) {
		post("ERROR: CocoaBridge Failed Creating valid signature for delegate !\n");
		return;
	}

	[mSelectorArray setObject:signature forKey:n];
}

- (void)removeSelector:(SEL)aSelector
{
	NSNumber* n = [NSNumber numberWithLong:(long)aSelector];
	[mSelectorArray removeObjectForKey:n];
}

@end

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

static int nsinvocationSetArguments(PyrSlot* args, NSInvocation *anInvocation)
{
	int len = slotRawObject(args)->size;
	for(int i =0; i<len; i++)
	{
		PyrSlot * slot = slotRawObject(args)->slots+i;
		char *cType = (char *)[[anInvocation methodSignature] getArgumentTypeAtIndex: i+2];
		if(*cType == 'r') ++cType; // increment and go

		if(IsFloat(slot))
		{
			float val;
			if(slotFloatVal(slot, &val) != noErr)
				return errFailed;

			if(*cType == _C_FLT) {
				[anInvocation setArgument: &val atIndex: i+2];
			}
			else if(*cType == _C_DBL) {
				double dblval = (double)val;
				[anInvocation setArgument: &dblval atIndex: i+2];
			}
			else if(*cType == _C_INT || *cType == _C_UINT){
				int ival = (int)val;
				[anInvocation setArgument: &ival atIndex: i+2];
			}
			else if(*cType == _C_LNG || *cType == _C_ULNG) {
				long lval = (long)val;
				[anInvocation setArgument: &lval atIndex: i+2];
			}
			else if(*cType == _C_SHT || *cType == _C_USHT) {
				short sval = (short)val;
				[anInvocation setArgument: &sval atIndex: i+2];
			}
			else if(*cType == _C_CHR || *cType == _C_UCHR) {
				char cval = (char)val;
				[anInvocation setArgument: &cval atIndex: i+2];
			}
			else if(*cType == _C_ID)
			{
				double dval;
				if(slotDoubleVal(slot, &dval) != noErr)
					return errFailed;
				NSNumber* number = [NSNumber numberWithDouble:dval];
				[anInvocation setArgument: &number atIndex: i+2];
			}
			else {
				post("wrong type (Float / Double) at arg: %i expected float\n", i);
				return errWrongType;
			}

		}
		else if(IsInt(slot))
		{
			int val;
			if(slotIntVal(slot, &val) != noErr)
				return errFailed;

			if(*cType == _C_INT || *cType == _C_UINT) {
				[anInvocation setArgument: &val atIndex: i+2];
			}
			else if(*cType == _C_LNG || *cType == _C_ULNG) {
				long lval = (long)val;
				[anInvocation setArgument: &lval atIndex: i+2];
			}
			else if(*cType == _C_SHT || *cType == _C_USHT) {
				short sval = (short)val;
				[anInvocation setArgument: &sval atIndex: i+2];
			}
			else if(*cType == _C_CHR || *cType == _C_UCHR) {
				char cval = (char)val;
				[anInvocation setArgument: &cval atIndex: i+2];
			}
			else if(*cType == _C_FLT) {
				float fval;
				fval = (float)val;
				[anInvocation setArgument: &fval atIndex: i+2];
			}else if(*cType == _C_DBL) {
				double fval;
				fval = (double)val;
				[anInvocation setArgument: &fval atIndex: i+2];
			}else if(*cType == _C_ID){
				NSNumber* number = [NSNumber numberWithInt:val];
				[anInvocation setArgument: &number atIndex: i+2];
			}else{
				post("wrong type (Integer) at arg: %i expected %c \n", i, *cType); return errWrongType;
			}
		}
		else if(
			isKindOfSlot(slot, s_nsObject->u.classobj) ||
			isKindOfSlot(slot, slotRawSymbol(&s_nsObject->u.classobj->superclass)->u.classobj)
		)
		{
			if(*cType != _C_ID) {
				post("wrong type (SCNSObject) at arg: %i expected %c \n", i, *cType);
				return errWrongType;
			}
			id val = (id) slotRawPtr(slotRawObject(slot)->slots);
			[anInvocation setArgument: &val atIndex: i+2];
		}
		else if(IsFalse(slot)) {
			bool val = false;
			if(*cType == 'B') {
				[anInvocation setArgument: &val atIndex: i+2];
			}
			else if(*cType == _C_INT || *cType == _C_UINT) {
				int ival = (int)val;
				[anInvocation setArgument: &ival atIndex: i+2];
			}
			else if(*cType == _C_SHT || *cType == _C_USHT) {
				short sval = (short)val;
				[anInvocation setArgument: &sval atIndex: i+2];
			}
			else if(*cType == _C_LNG || *cType == _C_ULNG) {
				short lval = (long)val;
				[anInvocation setArgument: &lval atIndex: i+2];
			}
			else if(*cType == _C_CHR || *cType == _C_UCHR) {
				char cval = (char)val;
				[anInvocation setArgument: &cval atIndex: i+2];
			}
			else if(*cType == _C_ID) {
				NSNumber* number = [NSNumber numberWithBool:NO];
				[anInvocation setArgument: &number atIndex: i+2];
			}
			else{
				post("wrong type (Boolean False) at arg: %i expected %c \n", i, *cType); return errWrongType;
			}
		}
		else if(IsTrue(slot)) {
			bool val = true;
			if(*cType == 'B') {
				[anInvocation setArgument: &val atIndex: i+2];
			}
			else if(*cType == _C_INT || *cType == _C_UINT) {
				int ival = (int)val;
				[anInvocation setArgument: &ival atIndex: i+2];
			}
			else if(*cType == _C_SHT || *cType == _C_USHT) {
				short sval = (short)val;
				[anInvocation setArgument: &sval atIndex: i+2];
			}
			else if(*cType == _C_LNG || *cType == _C_ULNG) {
				short lval = (long)val;
				[anInvocation setArgument: &lval atIndex: i+2];
			}
			else if(*cType == _C_CHR || *cType == _C_UCHR) {
				char cval = (char)val;
				[anInvocation setArgument: &cval atIndex: i+2];
			}
			else if(*cType == _C_ID) {
				NSNumber* number = [NSNumber numberWithBool:YES];
				[anInvocation setArgument: &number atIndex: i+2];
			}
			else{
				post("wrong type (Boolean True) at arg: %i expected %c \n", i, *cType); return errWrongType;
			}
		}
		else if(IsNil(slot))
		{
			if(*cType == _C_ID) {
				id idval = nil;
				[anInvocation setArgument: (void*)&idval atIndex: i+2];
			}
			else if(*cType == _C_PTR) {
				void *ptr = NULL;
				[anInvocation setArgument: &ptr atIndex: i+2];
			}else{
				post("wrong type (Nil) at arg: %i expected %c \n", i, *cType); return errWrongType;
			}
		}
		else if (isKindOfSlot(slot, s_rect->u.classobj))
		{
			if(*cType != _C_STRUCT_B) {
				post("wrong type (Rect) at arg: %i expected %c \n", i, *cType);
				return errWrongType;
			}

			NSRect bounds;
			int err = slotGetNSRect(slot, &bounds);
			if (err) return err;
			[anInvocation setArgument: &bounds atIndex: i+2];
		}
		else if (isKindOfSlot(slot, s_color->u.classobj))
		{
			if(*cType != _C_ID) {
				post("wrong type (Color) at arg: %i expected %c \n", i, *cType);
				return errWrongType;
			}

			PyrSlot *slots = slotRawObject(slot)->slots;
			int err;
			float r, g, b, a;
			NSColor * col;

			err = slotFloatVal(slots+0, &r);
			if (err) return err;
			err = slotFloatVal(slots+1, &g);
			if (err) return err;
			err = slotFloatVal(slots+2, &b);
			if (err) return err;
			err = slotFloatVal(slots+3, &a);
			if (err) return err;

			col = [NSColor colorWithCalibratedRed:r green:g blue:b alpha:a];
			[anInvocation setArgument: &col atIndex: i+2];
		}
		else if (isKindOfSlot(slot, s_string->u.classobj))
		{
			if(
			   *cType == _C_CHARPTR ||
			   *cType == _C_PTR
			) {
				char * ch = slotRawString(slot)->s;
				if(!ch) return errFailed;
				[anInvocation setArgument: (void*)  &ch atIndex: i+2];
			}
			else if(*cType == _C_ID) {
				NSString *s = nsStringFromPyrSlot(slot);
				if(!s) return errFailed;
				[anInvocation setArgument: (void*)&s atIndex: i+2];
			}
			else if(*cType == _C_SEL) { // blackrain
				NSString *s = nsStringFromPyrSlot(slot);
				if(!s) return errFailed;
				SEL sel = NSSelectorFromString(s);
				[anInvocation setArgument: (void*)&sel atIndex: i+2];
			}
			else {
				post("wrong type (String) at arg: %i expected %c \n", i, *cType);
				return errWrongType;
			}
		}
		else if (isKindOfSlot(slot, s_point->u.classobj))
		{
			if(*cType != _C_STRUCT_B) return errWrongType;
			NSPoint point;
			int err = slotGetPoint(slot, &point);
			if (err) return err;
			[anInvocation setArgument: &point atIndex: i+2];
		}
		else if (isKindOfSlot(slot, class_signal))
		{
			float *slotvalues = (float*)(slotRawObject(slot)->slots) ;
			[anInvocation setArgument: &slotvalues atIndex: i+2];

		}
		else if (isKindOfSlot(slot, s_int8array->u.classobj))
		{
			if( *cType == _C_PTR ) {
				void* ptr = (void*)(((PyrInt8Array*)slotRawObject(slot))->b);
				if(!ptr) return errFailed;
				[anInvocation setArgument: &ptr atIndex: i+2];
			}else{
				post("wrong type (Int8Array) at arg: %i expected %c \n", i, *cType); return errWrongType;
			}
		}
		else if (isKindOfSlot(slot, s_int16array->u.classobj))
		{
			if( *cType == _C_PTR ) {
				void* ptr = (void*)(((PyrInt16Array*)slotRawObject(slot))->i);
				if(!ptr) return errFailed;
				[anInvocation setArgument: &ptr atIndex: i+2];
			}else{
				post("wrong type (Int16Array) at arg: %i expected %c \n", i, *cType); return errWrongType;
			}
		}
		else if (isKindOfSlot(slot, s_int32array->u.classobj))
		{
			if( *cType == _C_PTR ) {
				void* ptr = (void*)(((PyrInt32Array*)slotRawObject(slot))->i);
				if(!ptr) return errFailed;
				[anInvocation setArgument: &ptr atIndex: i+2];
			}else{
				post("wrong type (Int32Array) at arg: %i expected %c \n", i, *cType); return errWrongType;
			}
		}
		else if (isKindOfSlot(slot, s_doublearray->u.classobj))
		{
			if( *cType == _C_PTR ) {
				void* ptr = (void*)(((PyrDoubleArray*)slotRawObject(slot))->d);
				if(!ptr) return errFailed;
				[anInvocation setArgument: &ptr atIndex: i+2];
			}else{
				post("wrong type (DoubleArray) at arg: %i expected %c \n", i, *cType); return errWrongType;
			}
		}
		else if (isKindOfSlot(slot, s_floatarray->u.classobj))
		{
			if( *cType == _C_PTR ) {
				void* ptr = (void*)(((PyrFloatArray*)slotRawObject(slot))->f);
				if(!ptr) return errFailed;
				[anInvocation setArgument: &ptr atIndex: i+2];
			}else{
				post("wrong type (FloatArray) at arg: %i expected %c \n", i, *cType); return errWrongType;
			}
		}
		else if (isKindOfSlot(slot, s_arrayed_collection->u.classobj))
		{
			if(strncmp(cType, "{_NSSize", 8) == 0) {
				NSSize size =  (NSSize){0.f, 0.f};
				float value;
				if(slotRawObject(slot)->size > 0) {
					if(slotFloatVal(slotRawObject(slot)->slots+0, &value)) return errFailed;
					size.width = value;
				}
				if(slotRawObject(slot)->size > 1) {
					if(slotFloatVal(slotRawObject(slot)->slots+1, &value)) return errFailed;
					size.height = value;
				}
				[anInvocation setArgument: (void*)&size atIndex: i+2];
			}
			else if(strncmp(cType, "{_NSRange", 9) == 0) {
				NSRange range =  (NSRange){0, 0};
				int value;
				if(slotRawObject(slot)->size > 0) {
					if(slotIntVal(slotRawObject(slot)->slots+0, &value)) return errFailed;
					range.location = (unsigned int)value;
				}
				if(slotRawObject(slot)->size > 1) {
					if(slotIntVal(slotRawObject(slot)->slots+1, &value)) return errFailed;
					range.length = (unsigned int)value;
				}
				[anInvocation setArgument: (void*)&range atIndex: i+2];
			}
			else if( strncmp(cType, "{?=qll}", 6) == 0 ) { // special QTTime structure

				QTTime time;
				PyrSlot* slots = slotRawObject(slot)->slots;

				if(slotRawObject(slot)->size < 2) {
					post("wrong number of elements in Array : 2 elements expected\n"); return errFailed;
				}

				time = QTMakeTime(
					(long long)(IsInt(slots+0) ? slotRawInt(&slots[0]) : (IsFloat(slots+0) ? slotRawFloat(slots) : 0)),
					(long)(IsInt(slots+1) ? slotRawInt(&slots[1]) : (IsFloat(slots+1) ? slotRawFloat(&slots[1]) : 0))
				);

				if( slotRawObject(slot)->size > 2 )
					time.flags = (long) (IsInt(slots+2) ? slotRawInt(&slots[2]) : (IsFloat(slots+2) ? slotRawFloat(&slots[2]) : 0));

				[anInvocation setArgument: (void*)&time atIndex: i+2];
			}
			else if( strncmp(cType, "{?={?=qll}{?=qll}}", 18) == 0 ) { // special QTTimeRange structure

				QTTimeRange timeRange; QTTime* time;
				if(slotRawObject(slot)->size < 2) {
					post("wrong number of elements in Array: expected 2 elements\n"); return errFailed;
				}

				PyrSlot* slots;
				for( int i=0; i < 2; ++i ) {
					if(slotRawObject(&slotRawObject(slot)->slots[i])->size < 2) {
						post("wrong number of elements in sub Array: expected 2 elements\n"); return errFailed;
					}

					slots = slotRawObject(&slotRawObject(slot)->slots[i])->slots;
					time = (i == 0) ? &timeRange.time : &timeRange.duration;
					*time = QTMakeTime(
									   (long long) (IsInt(slots+0) ? slotRawInt(&slots[0]) : (IsFloat(slots+0) ? slotRawFloat(slots) : 0)),
									   (long) (IsInt(slots+1) ? slotRawInt(&slots[1]) : (IsFloat(slots+1) ? slotRawFloat(&slots[1]) : 0))
									   );
					if( slotRawObject(slot)->size > 2 )
						time->flags = (long) (IsInt(slots+2) ? slotRawInt(&slots[2]) : (IsFloat(slots+2) ? slotRawFloat(&slots[2]) : 0));

				}

			}
			else {
				//for now only arrays of SCNSObject
				id * nsObjects = (id*) malloc(slotRawObject(args)->size * sizeof(id));
				for(int j=0;  j<slotRawObject(slot)->size; j++)
				{
					PyrSlot * nsslot = slotRawObject(slot)->slots+j;
					nsObjects[j] = (id) slotRawPtr(slotRawObject(nsslot)->slots);
				}
				if(!nsObjects) return errFailed;
				[anInvocation setArgument: &nsObjects atIndex: i+2];
			}
		}
		else {
			post("ERROR: bad argument type or Nil argument !\n");
			return errFailed;
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
	NSString * nsclassname = nsStringFromPyrSlot(classname);
	if([nsclassname length] < 1){
		post("ERROR: CocoaBridge NSClass name invalid\n");
		SetNil(receiver);
		return errFailed;
	}

    Class nsclass = [NSClassFromString( nsclassname ) class];
	if(!nsclass){
		post("ERROR: CocoaBridge Failed finding valid NSClass\n");
		SetNil(receiver);
		return errFailed;
	}

	NSString * nsinitname = nsStringFromPyrSlot(initname);
	if([nsinitname length] < 1) {
		post("ERROR: CocoaBridge Failed converting method to NSString\n");
		SetNil(receiver);
		return errFailed;
	}

	//post("Trying Class: %s selector: %s\n", [nsclassname UTF8String], [nsinitname UTF8String]);

	SEL sel = NSSelectorFromString(nsinitname);
	BOOL isInstanceMethod = [nsclass instancesRespondToSelector: sel];
	if(
		!isInstanceMethod &&
		((isInstanceMethod = ![nsclass respondsToSelector: sel]) == YES)
	) {
		// not an instance method and not a class method
		SetNil(receiver);
		post("Warning: NSClass: %s does not respond to: %s \n", [nsclassname cString], [nsinitname cString]);
		return errNone;
	}

	id newThing = NULL;
	NSMethodSignature *sig = isInstanceMethod ? [nsclass  instanceMethodSignatureForSelector: sel] : [nsclass methodSignatureForSelector: sel];
	if(!sig) {
		post("ERROR: CocoaBridge NSMethodSignature not found\n");
		return errFailed;
	}

	NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
	if(!anInvocation) {
		post("ERROR: CocoaBridge Failed creating NSInvocation\n");
		return errFailed;
	}

    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];

	int numberOfArgs = (int) [sig numberOfArguments] - 2; //cocoa args start at 2

	if (hasArg && isCollection && numberOfArgs>0)
	{
		int len = slotRawObject(args)->size;

		//should check for type [sig getArgumentTypeAtIndex: i];
		if(numberOfArgs != len)
		{
			post("Warning: NSClass: %s numberOfArguments does not match: %i, provided: %i \n", [nsclassname cString],numberOfArgs, len);
			SetNil(receiver);
			return errFailed;
		}
		int err = nsinvocationSetArguments(args, anInvocation);
		if(err){
			post("ERROR: CocoaBridge failed setting arguments for Invocation\n");
			SetNil(receiver);
			return err;
		}
	} else if (numberOfArgs>0)
	{
		post("Warning: CocoaBridge mismatching arguments \n");
		SetNil(receiver);
		return errFailed;
	}

	[anInvocation setTarget: isInstanceMethod ? [nsclass alloc] : (id)nsclass];
	[anInvocation setSelector: sel];

	if(IsTrue(defer))
		[scvm defer: anInvocation];
	else
	{
		PyrObjectHdr	*convertedObject=NULL;
		PyrSlot			retslot;
		const char		*cType=NULL;
		unsigned int	length;
		void			*retval=NULL;

		TryInvocation(anInvocation, receiver);

		cType = [[anInvocation methodSignature]methodReturnType];
		length = [[anInvocation methodSignature] methodReturnLength];
		//NSLog(@"AllocInit type: %c length: %i", cType, length);

		SetNil(&retslot);

		if(length > 0) {
			retval = (void*)malloc(length);
			[anInvocation getReturnValue: (void*)retval];
		}

		if(ObjcTypeFillPyrSlot(&retslot, cType, retval, &convertedObject) != noErr) {
			post("ERROR: CocoaBridge returned object type: %c is not-of-type _C_ID !\n", *cType);
			if(retval) free(retval);
			return errFailed;
		}

		// If no suitable Cocoa<->SCLang conversion can be done for the object value
		if(*cType == _C_ID && !convertedObject) {

			if(IsNil(&retslot)) { // the returned object is Nil - might lead to EXC_BAD_ACCESS after
				SetNil(slotRawObject(receiver)->slots+0); // set the dataptr slot to nil
				SetNil(receiver); // return nil
				if(retval) free(retval);
				return errNone; // should returning nil be considered as an error ?
			}

			slotCopy(&slotRawObject(receiver)->slots[0], &retslot);
			newThing = (id) slotRawPtr(slotRawObject(receiver)->slots);

			if([newThing isKindOfClass:[NSWindow class]])
				[newThing setReleasedWhenClosed:NO]; // prevent user crash for this specific UI

			NSString *classNameString = NSStringFromClass( [newThing class] );
			PyrString *pyrClassNameString = newPyrString(g->gc,[classNameString UTF8String],0,true);
			SetObject(slotRawObject(receiver)->slots + 1, pyrClassNameString);

			if(isInstanceMethod)
				[newThing release]; // retained and added already in ObjcTypeFillPyrSlot
		}
		else
			slotCopy(receiver, &retslot);

		if(retval)
			free(retval);
	}
    return errNone;
}

int prObjcSetActionForControl(struct VMGlobals *g, int numArgsPushed);
int prObjcSetActionForControl(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 2;
    PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;
	id objc = (id) slotRawPtr(slotRawObject(a)->slots);
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
	if(!action)
		return errFailed;

	COCOABRIDGE_OBJ_RETAIN(action);
	[action release]; // already retained

	[action setSCObject: slotRawObject(b)];

	[objc setAction: selAc];
	[objc setTarget: action];

	SetPtr(slotRawObject(b)->slots + 0, action);
    return errNone;
}


//// Notification Test thelych
int prObjcRegisterNotification(struct VMGlobals *g, int numArgsPushed);
int prObjcRegisterNotification(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *c = g->sp;
	PyrSlot *b = g->sp - 1;
	PyrSlot *a = g->sp - 2;
	id objc = (id) slotRawPtr(slotRawObject(a)->slots);
	id delegate = nil;
	if(!objc) return errFailed;

	if([[objc class] instancesRespondToSelector:@selector(delegate)])
		delegate = [objc delegate];

	if(!delegate) {
		//post("ERROR delegate does not exists\n"); // try to recover
		//return errFailed;
		if(
			IsNil(&(slotRawObject(a)->slots[3])) ||
			IsNil((slotRawObject(&slotRawObject(a)->slots[3]))->slots)
		) {
			printf("Error: Slot is nil\n");
			return errNone;
		}
		else
			delegate = (id) slotRawPtr(slotRawObject(&slotRawObject(a)->slots[3])->slots); // some objects do not respond to
		// setDelegate but do post notifications...
	}

	NSString *notificationName = nsStringFromPyrSlot(b);
	post([[NSString stringWithFormat:@"Registering Notification: %@\n", notificationName] UTF8String]);

	if (IsNil(c)) { // br
		objc = nil;
		post("Listening to Notifications from any Object.\n");
	} else
		objc = (id) slotRawPtr(slotRawObject(c)->slots);

	[[NSNotificationCenter defaultCenter] addObserver:delegate
		selector:@selector(notificationReceived:)
		name:notificationName object:objc // apply only to object
	];

	return errNone;
}

int prObjcSetDelegate(struct VMGlobals *g, int numArgsPushed);
int prObjcSetDelegate(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1;
    PyrSlot *b  = g->sp;
	id objc = (id) slotRawPtr(slotRawObject(a)->slots);
	if(!objc) {
#pragma mark __TEST_CUSTOM_DELEGATE_
			SCCocoaToLangAction *action = [[SCCocoaToLangAction alloc] init];
			post("Warning: creating a special delegate %p\n", action);
			if(!action)
				return errFailed;
			COCOABRIDGE_OBJ_RETAIN(action);
			[action release]; // already retained

			[action setSCObject: slotRawObject(b)];
			SetPtr(slotRawObject(b)->slots + 0, action);
			return errNone;
	}

	if(![[objc  class] instancesRespondToSelector:@selector(setDelegate:)])
	{
		// setting void action so it can register notifications
		SCCocoaToLangAction *action = [[SCCocoaToLangAction alloc] init];
		post("Warning: creating a void delegate\n");
		if(!action)
			return errFailed;

		COCOABRIDGE_OBJ_RETAIN(action);
		[action release]; // already retained
		[action setSCObject: slotRawObject(b)];
		SetPtr(slotRawObject(b)->slots + 0, action);
		return errNone;
	}

	// do not create a delegate if a previous delegate exists already
	// default delegate can be SCVirtualMachine
	if(![objc delegate]) {
		SCCocoaToLangAction *action = [[SCCocoaToLangAction alloc] init];
		if(!action)
			return errFailed;

		COCOABRIDGE_OBJ_RETAIN(action);
		[action release]; // already retained
		[action setSCObject: slotRawObject(b)];
		[objc setDelegate: action];
		SetPtr(slotRawObject(b)->slots + 0, action);
	}else{
		post("Warning: There is already a delegate for this NSObject (%s).\n", [[objc delegate]className]);
		SetNil(slotRawObject(b)->slots + 0);
	}
	post("delegate created successfully for object %p of class %s\n", objc, [[objc className]UTF8String]);
    return errNone;
}

int prObjcGetClass(struct VMGlobals *g, int numArgsPushed);
int prObjcGetClass(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *receiver = g->sp - 1;
    PyrSlot *classname = g->sp ;

	id nsclass = [objc_getClass(slotRawString(classname)->s) class];

	[nsclass retain];
	SetPtr(slotRawObject(receiver)->slots + 0, nsclass);
    return errNone;
}

int prObjcSendMessage(struct VMGlobals *g, int numArgsPushed);
int prObjcSendMessage(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *receiver = g->sp - 2;
	PyrSlot *msgname = g->sp - 1;
	PyrSlot *args  = g->sp;
//    PyrSlot *defer  = g->sp;


//	bool hasArg = NotNil(args);
//	bool isCollection = isKindOfSlot(args, class_array);

	id newThing = (id) slotRawPtr(slotRawObject(receiver)->slots);
	if (!newThing) return errFailed;

	NSString * nsinitname = nsStringFromPyrSlot(msgname);
	if([nsinitname length] < 1){  return errNone; }

	SEL sel = NSSelectorFromString(nsinitname);


//	int val;
//	slotIntVal(slotRawObject(args)->slots+0, &val);
	newThing = objc_msgSend(newThing, sel, args);
//	[newThing retain];
	if(newThing)
		SetPtr(slotRawObject(receiver)->slots + 0, newThing);
	else
		SetNil(slotRawObject(receiver)->slots + 0);

	return errNone;
}

int prObjcAllocSend(struct VMGlobals *g, int numArgsPushed);
int prObjcAllocSend(struct VMGlobals *g, int numArgsPushed)
{
//under construction ...
	PyrSlot *receiver = g->sp - 3;
    PyrSlot *classname = g->sp - 2;
	PyrSlot *initname = g->sp - 1;
//    PyrSlot *args  = g->sp;


//	bool hasArg = NotNil(args);
//	bool isCollection = isKindOfSlot(args, class_array);

	NSString * nsclassname = nsStringFromPyrSlot(classname);
	if([nsclassname length] < 1){ return errFailed; }
	NSString * nsinitname = nsStringFromPyrSlot(initname);
	if([nsinitname length] < 1){  return errNone; }

	SEL sel = NSSelectorFromString(nsinitname);

//	NSMethodSignature *sig = [[NSClassFromString( nsclassname ) class]  instanceMethodSignatureForSelector: sel];
	#if 1
	id nsclass = objc_getClass(slotRawString(classname)->s);

	int val = 0;
//	slotIntVal(slotRawObject(args)->slots+0, &val);
	id newThing = objc_msgSend(nsclass, sel, val);
	[newThing retain];
	#else
	/// //for alloc
	int val = NULL;
	slotIntVal(slotRawObject(args)->slots+0, &val);

    id newThing = [[NSClassFromString( nsclassname ) class] sel val];
	[newThing retain];

	#endif
	SetPtr(slotRawObject(receiver)->slots + 0, newThing);
//	SetInt(slotRawObject(receiver)->slots + 0, 123);

    return errNone;
}

int prObjcGetClassName(struct VMGlobals *g, int numArgsPushed);
int prObjcGetClassName(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1;
//	PyrSlot *b  = g->sp;
	id objc = (id) slotRawPtr(slotRawObject(a)->slots + 0);
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

    if (!g->canCallOS)
		return errCantCallOS; //moght be better to check this ?

	bool hasArg = NotNil(args) && isKindOfSlot(args, class_array);
	bool isCollection = isKindOfSlot(args, class_array);
	if(!isCollection) return errFailed;

    // must defer in case of large loads and gui
	id newThing = (id) slotRawPtr(slotRawObject(receiver)->slots);
    if (!newThing) {
		post("ERROR: no valid NSObject to defer Invocation to !\n");
		return errFailed;
	}

	NSString * nsclassname = [newThing className];
    Class nsclass = [NSClassFromString( nsclassname ) class];
	NSString * nsinitname = nsStringFromPyrSlot(initname);

	if([nsinitname length] < 1){
		post("ERROR: Method argument is invalid !\n");
		return errFailed;
	}

	SEL sel = NSSelectorFromString(nsinitname);
	if(![nsclass instancesRespondToSelector:sel])
	{
		post("Warning: %s does not respond to: %s \n", [nsclassname cString], [nsinitname cString]);
		return errNone;
	}

	NSMethodSignature *sig = [nsclass instanceMethodSignatureForSelector: sel];
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: newThing];
    [anInvocation setSelector: sel];

	int numberOfArgs = (int) [sig numberOfArguments] - 2; //cocoa args start at 2
	int len = slotRawObject(args)->size;

	if(len < numberOfArgs) return errFailed;
	if(len > numberOfArgs) len = numberOfArgs;

	if(hasArg)
	{
		int err = nsinvocationSetArguments(args, anInvocation);
		if(err) return err;
	}

	if(IsTrue(defer)){
		[anInvocation retainArguments];
		[scvm defer: anInvocation];
	}
	else
	{
		//?use void objc_msgSend_stret(void * stretAddr, id theReceiver, SEL theSelector, ...);
		PyrObjectHdr	*convertedObject=NULL;
		unsigned int	length = 0;
		char			*cType = NULL;
		void			*retval = NULL;

		TryInvocation(anInvocation, receiver);

		length = [[anInvocation methodSignature] methodReturnLength];
		cType = (char *)[[anInvocation methodSignature] methodReturnType];

		if(*cType != _C_VOID && *cType != _C_UNDEF && (length > 0)) {
			retval = (void*)malloc(length);
			[anInvocation getReturnValue: (void*)retval];

			if(ObjcTypeFillPyrSlot(receiver, cType, retval, &convertedObject) != noErr) {
				post("ERROR: CocoaBridge returned object type: %c is not-of-type _C_ID !\n", *cType);
				if(retval) free(retval);
				return errFailed;
			}
		}

		if(retval)
			free(retval);
	}
	return errNone;
}

int prObjcDealloc(struct VMGlobals *g, int numArgsPushed);
int prObjcDealloc(struct VMGlobals *g, int numArgsPushed)
{
    //PyrSlot *receiver = g->sp - 1;
	PyrSlot *receiver = g->sp;
    //PyrSlot *dataptr = g->sp;

    // have to put them in a specific pool so they can be released on command
    //if(! IsInt(dataptr) )
	//	return errWrongType;

	//id nsobj = (id)dataptr->ui;
	// check retain count and return in case
	id nsobj = (id)slotRawPtr(slotRawObject(receiver)->slots);
	if(nsobj) {
		COCOABRIDGE_OBJ_RELEASE(nsobj);
		SetPtr(slotRawObject(receiver)->slots, 0);
	}else
		post("SCNSObject object pointer is nil\n");

	return errNone;
}

int prObjcLoadBundle(struct VMGlobals *g, int numArgsPushed);
int prObjcLoadBundle(struct VMGlobals *g, int numArgsPushed)
{
    PyrSlot *a = g->sp - 1;
	PyrSlot *path = g->sp;
//lazy load of a bundle and its principla class as desciped by apple
 	NSString * bundlePath = nsStringFromPyrSlot(path);
	NSBundle *bundle = [NSBundle bundleWithPath:bundlePath];
	if(bundle)
		SetPtr(slotRawObject(a)->slots + 0, bundle);
	else
		SetNil(a);
	return errNone;
}

int prObjcBundleAllocPrincipalClass(struct VMGlobals *g, int numArgsPushed);
int prObjcBundleAllocPrincipalClass(struct VMGlobals *g, int numArgsPushed)
{

	PyrSlot *receiver = g->sp;

    // have to put them in a specific pool so they can be released on command
	id bundle = (id) slotRawPtr(slotRawObject(receiver)->slots);
    if (!bundle) return errFailed;
	Class principalClass = [bundle principalClass];
	NSObject * _bundleObject = NULL;

	if(principalClass)
	{
		_bundleObject = [[principalClass alloc] init];         // 5
	}
	if(_bundleObject)
		SetPtr(receiver, _bundleObject);
	else
		SetNil(receiver);
	return errNone;
}

int prObjcBundleAllocClassNamed(struct VMGlobals *g, int numArgsPushed);
int prObjcBundleAllocClassNamed(struct VMGlobals *g, int numArgsPushed)
{

    PyrSlot *receiver = g->sp - 4;
    PyrSlot *name = g->sp - 3;
	PyrSlot *initname = g->sp - 2;
    PyrSlot *args  = g->sp-1;
    PyrSlot *defer  = g->sp;

	bool hasArg = NotNil(args);
	bool isCollection = isKindOfSlot(args, class_array);

	//lazy load of a bundle and its principla class as desciped by apple
	NSString * nsclassname = nsStringFromPyrSlot(name);
	id bundle = (id) slotRawPtr(slotRawObject(receiver)->slots);
	if (!bundle) return errFailed;
	Class nsclass = NULL;
	NSObject * newThing = NULL;
	nsclass = [bundle classNamed:nsclassname];
	if(!nsclass){SetNil(receiver); post("class %s not found in bundle\n", [nsclassname cStringUsingEncoding: NSMacOSRomanStringEncoding]);return errFailed;}
	// sort out initMethod
	NSString * nsinitname = nsStringFromPyrSlot(initname);
	if([nsinitname length] < 1){ SetNil(receiver); return errFailed; }

	SEL sel = NSSelectorFromString(nsinitname);
	if(![nsclass instancesRespondToSelector:sel])
	{
		SetNil(receiver);
		post("Warning: NSClass: %s does not respond to: %s \n", [nsclassname cString], [nsinitname cString]);
		return errNone;
	}
	NSMethodSignature *sig = [nsclass  instanceMethodSignatureForSelector: sel];
	NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
	SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];

	int numberOfArgs = (int) [sig numberOfArguments] - 2; //cocoa args start at 2

	if (hasArg && isCollection && numberOfArgs>0)
	{
		int len = slotRawObject(args)->size;

		//should check for type [sig getArgumentTypeAtIndex: i];
		if(numberOfArgs != len)
		{
			post("Warning: NSClass: %s numberOfArguments does not match: %i, provided: %i \n", [nsclassname cString],numberOfArgs, len);
			SetNil(receiver);
			return errFailed;
		};
		int err = nsinvocationSetArguments(args, anInvocation);
		if(err){ SetNil(receiver); return err;}


	} else if (numberOfArgs>0)
	{
		post("Warning: mismatching arguments \n");
		SetNil(receiver);
		return errFailed;
	}
	newThing = [nsclass alloc];
	[anInvocation setTarget: newThing];
	[anInvocation setSelector: sel];
	if(IsTrue(defer))
	{
		[scvm defer: anInvocation];
	}
	else
	{
		id val;
		[anInvocation invoke];
		[anInvocation getReturnValue: (void*)&val];
		newThing = (id) val;
		[newThing retain];
		SetPtr(receiver, newThing);
		COCOABRIDGE_OBJ_RETAIN(newThing);
	}

    return errNone;

//	if(_bundleObject)
//		SetPtr(receiver, _bundleObject);
//	else
//		SetNil(receiver);
}


int prObjcNSStringToPyrString(struct VMGlobals *g, int numArgsPushed);
int prObjcNSStringToPyrString(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a  = g->sp; // the scns_object
	id nsstring = (id)slotRawPtr(slotRawObject(a)->slots);
	PyrString * pyrString;
	NSString* className = [nsstring className];
	if(
		[nsstring isKindOfClass:[NSString class]]
	)
		pyrString = newPyrString(g->gc, [(NSString*)nsstring UTF8String], 0, true);
	else
		pyrString = newPyrString(g->gc, [className UTF8String], 0, true);

	SetObject(a, pyrString);
	return errNone;
}

int prObjcIsSubclassOfNSClass(struct VMGlobals *g, int numArgsPushed);
int prObjcIsSubclassOfNSClass(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;


	NSString *nsstr = nsStringFromPyrSlot(b);
	if(!nsstr)
		return errFailed;

	Class objcclass = NSClassFromString(nsstr);
	if(objcclass != nil) {
		id obj = (id)slotRawPtr(slotRawObject(a)->slots);
		if([obj isKindOfClass:objcclass]) {
			SetTrue(a);
			return errNone;
		}
	}

	SetFalse(a);
	return errNone;
}

int prObjcDelegateAddSelector(struct VMGlobals *g, int numArgsPushed);
int prObjcDelegateAddSelector(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;
	//PyrObject* obj;
	char slotString[64];

	SCCocoaToLangAction* nsobj = (SCCocoaToLangAction*)slotRawPtr(slotRawObject(a)->slots);

	if(!isKindOfSlot(b, class_string) || !isKindOfSlot(c, class_string))
		return errWrongType;

	if(slotStrVal(b, slotString, 64) != noErr)
		return errWrongType;

	SEL selector = sel_getUid(slotString);
	if(!selector)
		return errFailed;

	if(slotStrVal(c, slotString, 64) != noErr)
		return errWrongType;

	[nsobj addSelector:selector withObjcTypes:slotString];

	return errNone;
}

int prObjcDelegateRemoveSelector(struct VMGlobals *g, int numArgsPushed);
int prObjcDelegateRemoveSelector(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;
	SCCocoaToLangAction* nsobj = (SCCocoaToLangAction*)slotRawPtr(slotRawObject(a)->slots);

	if(!isKindOfSlot(b, class_string))
		return errWrongType;

	char slotString[64];
	if(slotStrVal(b, slotString, 64) != noErr)
		return errWrongType;

	SEL selector = sel_getUid(slotString);
	if(!selector)
		return errFailed;

	[nsobj removeSelector:selector];
	return errNone;
}

int prObjcDumpPool(struct VMGlobals *g, int numArgsPushed);
int prObjcDumpPool(struct VMGlobals *g, int numArgsPushed)
{
#if COCOABRIDGE_STORE_OBJECTS_IN_ARRAY
	if(!nsobjects) {
		return errFailed;
	}

	post("\n--- POOL DUMP  %i ---\n", [nsobjects count]);
	NSEnumerator *e = [nsobjects objectEnumerator];
	id obj; int i=0;
	while((obj = [e nextObject])) {
		post("OBJECT (%p), CLASS (%s), IDX(%i)\n", obj, [NSStringFromClass([obj class]) UTF8String], i);
		++i;
	}
	post("--- END OF DUMP ---\n");
#endif
	return errNone;
}

int prObjcFreePool(struct VMGlobals *g, int numArgsPushed);
int prObjcFreePool(struct VMGlobals *g, int numArgsPushed)
{
#if COCOABRIDGE_STORE_OBJECTS_IN_ARRAY
	if(!nsobjects) {
		return errFailed;
	}

	[nsobjects removeAllObjects];
	post("POOL FREED\n");
#endif
	return errNone;
}

int prObjcNSDataToPyrArray(struct VMGlobals *g, int numArgsPushed);
int prObjcNSDataToPyrArray(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a  = g->sp - 2; // the scns_object
	PyrSlot *b  = g->sp - 1; // the array (or string)
	PyrSlot *c  = g->sp;	 // length to retrieve

	PyrObject*	pyrarray = slotRawObject(b);
	PyrClass*	pyrclass = pyrarray->classptr;

	NSData*		nsobject = (NSData*)slotRawPtr(slotRawObject(a)->slots);
	int			size=0, nssize = [nsobject length]; // in bytes

	if(!IsInt(c) && !IsFloat(c))
		return errWrongType;

	size = IsFloat(c) ? (int)slotRawFloat(c) : (int)slotRawInt(c);
	if(size <= 0 || nssize <= 0) {
		SetNil(a);
		return errNone;
	}

	if(pyrclass == class_int8array) {

		size = sc_min(nssize, size);
		memcpy(((PyrInt8Array*)pyrarray)->b, [nsobject bytes], size);

	} else if(pyrclass == class_int32array){

		size = sc_min(nssize, size/* >> 2*/);
		memcpy(((PyrInt32Array*)pyrarray)->i, [nsobject bytes], size);

	} else if(pyrclass == class_int16array) {

		size = sc_min(nssize, size/* >> 1*/);
		memcpy(((PyrInt32Array*)pyrarray)->i, [nsobject bytes], size);

	} else if(pyrclass == class_doublearray) {

		size = sc_min(nssize, size/* >> (sizeof(double)>>1)*/); // double size same for PPC32 - IA32 - IA64 according to Mac ABI
		memcpy(((PyrDoubleArray*)pyrarray)->d, [nsobject bytes], size);

	} else if(pyrclass == class_floatarray) {

		size = sc_min(nssize, size /*>> (sizeof(float)>>1)*/); // float size same for PPC32 - IA32 - IA64 according to Mac ABI
		memcpy(((PyrFloatArray*)pyrarray)->f, [nsobject bytes], size);

	} else if(pyrclass == class_string) {

		size = sc_min(nssize, size);
		memcpy(((PyrString*)pyrarray)->s, [nsobject bytes], size);

	} else
		return errWrongType;

	//pyrarray->size = size;
	SetObject(a, pyrarray);
	return errNone;
}

void initCocoaBridgePrimitives()
{
    int base, index;

    base = nextPrimitiveIndex();
    index = 0;
	s_nsObject = getsym("SCNSObject");

#if	COCOABRIDGE_STORE_OBJECTS_IN_ARRAY
	if(nsobjects)
	{
#if COCOABRIDGE_VERBOSITY
		NSLog(@"----- POOL_MNG -----");
		NSLog(@"Number of objects: %i", [nsobjects count]);
#endif
		id nsobj;
		NSEnumerator* e = [nsobjects objectEnumerator];
		while((nsobj = [e nextObject])) {
			if(strcmp(class_getName(nsobj->isa->isa), "FREED(id)") == 0) { // may happen so add verbosity
				NSLog(@"Already FREED NSObject in POOL -> will cause crash");
				continue;
			}

			COCOABRIDGE_POOL_REM_LOG(nsobj)
			if([nsobj isKindOfClass:[NSWindow class]]) {
				// NSWindows are special since they are directly deallocated (freed - not just released)
				// by the windowing system - so we must take care that it won't crash by releasing an already freed reference
				// or we should disable this default behaviour
				NSWindow* wobj = (NSWindow*)nsobj;
				if([wobj isVisible]) {
					[wobj setReleasedWhenClosed:NO]; // otherwise might lead to a crash
					//[wobj close]; // close any windows instance
				}
			}
		}
		[nsobjects release]; // will release all references
		nsobjects = nil;
	}
	// nsobjects = [[NSMutableSet setWithCapacity: 8]retain];
	nsobjects = [[NSMutableArray arrayWithCapacity: 8]retain];
#endif

    definePrimitive(base, index++, "_ObjC_AllocInit", prObjcAllocInit, 5, 0);
    definePrimitive(base, index++, "_ObjC_Dealloc", prObjcDealloc, 1, 0);
    definePrimitive(base, index++, "_ObjC_Invoke", prObjcDeferInvocation, 4, 0);
	definePrimitive(base, index++, "_ObjC_GetClassName", prObjcGetClassName, 2, 0);
    definePrimitive(base, index++, "_ObjC_SetActionForControl", prObjcSetActionForControl, 3, 0);
    definePrimitive(base, index++, "_ObjC_SetDelegate", prObjcSetDelegate, 2, 0);
    definePrimitive(base, index++, "_ObjC_AllocSend", prObjcAllocSend, 4, 0);
    definePrimitive(base, index++, "_ObjC_GetClass", prObjcGetClass, 2, 0);
    definePrimitive(base, index++, "_ObjC_SendMessage", prObjcSendMessage, 3, 0);
    definePrimitive(base, index++, "_ObjC_LoadBundle", prObjcLoadBundle, 2, 0);
    definePrimitive(base, index++, "_ObjcBundleAllocPrincipalClass", prObjcBundleAllocPrincipalClass, 1, 0);
    definePrimitive(base, index++, "_ObjcBundleAllocClassNamed", prObjcBundleAllocClassNamed, 5, 0);
	definePrimitive(base, index++, "_ObjC_RegisterNotification", prObjcRegisterNotification, 3, 0);	 // added cp
	definePrimitive(base, index++, "_ObjC_NSDataToSCArray", prObjcNSDataToPyrArray, 3, 0);
	definePrimitive(base, index++, "_ObjC_NSStringToPyrString", prObjcNSStringToPyrString, 1, 0);
	definePrimitive(base, index++, "_ObjC_IsSubclassOfNSClass", prObjcIsSubclassOfNSClass, 2, 0);
	definePrimitive(base, index++, "_ObjC_DelegateAddSelector", prObjcDelegateAddSelector, 3, 0);
	definePrimitive(base, index++, "_ObjC_DelegateRemoveSelector", prObjcDelegateRemoveSelector, 2, 0);
	definePrimitive(base, index++, "_ObjC_DumpPool", prObjcDumpPool, 1, 0);
	definePrimitive(base, index++, "_ObjC_FreePool", prObjcFreePool, 1, 0);
// loadNibFile
}
