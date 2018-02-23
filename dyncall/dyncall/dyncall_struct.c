/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_struct.c
 Description: C interface to compute struct size
 License:

   Copyright (c) 2010-2015 Olivier Chafik <olivier.chafik@centraliens.net>

   Permission to use, copy, modify, and distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/



#include "dyncall.h"
#include "dyncall_signature.h"
#include "dyncall_struct.h"
#include "dyncall_alloc.h"
#include <stdio.h>
#include <assert.h>


DCstruct* dcNewStruct(DCsize fieldCount, DCint alignment)
{
	DCstruct* s = (DCstruct*)dcAllocMem(sizeof(DCstruct));
	s->pCurrentStruct = s;
	s->pLastStruct = NULL;
	s->nextField = 0;
	s->fieldCount = fieldCount;
	s->alignment = alignment;
	s->size = 0;
	s->pFields = (DCfield*)dcAllocMem(fieldCount * sizeof(DCfield));
	return s;
}


void dcStructField(DCstruct* s, DCint type, DCint alignment, DCsize arrayLength)
{
	DCfield *f;
	if (type == DC_SIGCHAR_STRING) {
		assert(!"Use dcSubStruct instead !!!");
		return;
	}
	assert(s && s->pCurrentStruct);
	assert(s->pCurrentStruct->nextField <= (DCint)s->pCurrentStruct->fieldCount - 1);
	f = s->pCurrentStruct->pFields + (s->pCurrentStruct->nextField++);
	f->type = type;
	f->alignment = alignment;
	f->arrayLength = arrayLength;
	f->pSubStruct = NULL;
	switch (type) {
    	case DC_SIGCHAR_BOOL:       f->size = sizeof(DCbool);     break;
    	case DC_SIGCHAR_CHAR:       
    	case DC_SIGCHAR_UCHAR:      f->size = sizeof(DCchar);     break;
    	case DC_SIGCHAR_SHORT:      
    	case DC_SIGCHAR_USHORT:     f->size = sizeof(DCshort);    break;
    	case DC_SIGCHAR_INT:        
    	case DC_SIGCHAR_UINT:       f->size = sizeof(DCint);      break;
    	case DC_SIGCHAR_LONG:       
    	case DC_SIGCHAR_ULONG:      f->size = sizeof(DClong);     break;
    	case DC_SIGCHAR_LONGLONG:   
    	case DC_SIGCHAR_ULONGLONG:  f->size = sizeof(DClonglong); break;
    	case DC_SIGCHAR_FLOAT:      f->size = sizeof(DCfloat);    break;
    	case DC_SIGCHAR_DOUBLE:     f->size = sizeof(DCdouble);   break;
    	case DC_SIGCHAR_POINTER:
		case DC_SIGCHAR_STRING:     f->size = sizeof(DCpointer);  break;
		default:                    assert(0);
	}
}


void dcSubStruct(DCstruct* s, DCsize fieldCount, DCint alignment, DCsize arrayLength)
{
	DCfield *f = s->pCurrentStruct->pFields + (s->pCurrentStruct->nextField++);
	f->type = DC_SIGCHAR_STRUCT;
	f->arrayLength = arrayLength;
	f->alignment = alignment;
	f->pSubStruct = dcNewStruct(fieldCount, alignment);
	f->pSubStruct->pLastStruct = s->pCurrentStruct;
	s->pCurrentStruct = f->pSubStruct;
}  	


static void dcAlign(DCsize *size, DCsize alignment)
{
	DCsize mod = (*size) % alignment;
	if (mod) {
		DCsize rest = alignment - mod;
		(*size) += rest;
	}
}


static void dcComputeStructSize(DCstruct* s)
{
	DCsize i;
	assert(s);
	for (i = 0; i < s->fieldCount; i++) {
		DCfield *f = s->pFields + i;
		DCsize fieldAlignment;
		if (f->type == DC_SIGCHAR_STRUCT) {
			dcComputeStructSize(f->pSubStruct);
			f->size = f->pSubStruct->size;
			fieldAlignment = f->pSubStruct->alignment;
		} else {
			fieldAlignment = f->size;
		}
		if (!f->alignment)
			f->alignment = fieldAlignment;
		
		if (f->alignment > s->alignment)
			s->alignment = f->alignment;
		
		f->size *= f->arrayLength;
		
		/*printf("FIELD %d, size = %d, alignment = %d\n", (int)i, (int)f->size, (int)f->alignment);@@@*/
	}
	for (i = 0; i < s->fieldCount; i++) {
		DCfield *f = s->pFields + i;
		dcAlign(&s->size, f->alignment);
		s->size += f->size;
	}
	dcAlign(&s->size, s->alignment);
	
	/*printf("STRUCT size = %d, alignment = %d\n", (int)s->size, (int)s->alignment);@@@*/
}


void dcCloseStruct(DCstruct* s)
{
	assert(s);
	assert(s->pCurrentStruct);
	assert(s->pCurrentStruct->nextField == s->pCurrentStruct->fieldCount);
	if (!s->pCurrentStruct->pLastStruct) {
		dcComputeStructSize(s->pCurrentStruct);
	}
	s->pCurrentStruct = s->pCurrentStruct->pLastStruct;
}


void dcFreeStruct(DCstruct* s)
{
	DCsize i;
	assert(s);
	for (i = 0; i < s->fieldCount; i++) {
		DCfield *f = s->pFields + i;
		if (f->type == DC_SIGCHAR_STRUCT)
			dcFreeStruct(f->pSubStruct);
	}
	free(s->pFields);
	free(s);
}


DCsize dcStructSize(DCstruct* s)
{
	assert(!s->pCurrentStruct && "Struct was not closed");
	return s->size;
}

DCsize dcStructAlignment(DCstruct* s)
{
	assert(!s->pCurrentStruct && "Struct was not closed");
	return s->alignment;
}


void dcArgStructUnroll(DCCallVM* vm, DCstruct* s, DCpointer  value)
{
	DCsize i;
	/*printf("UNROLLING STRUCT !\n");@@@*/
	assert(s && value);
	for (i = 0; i < s->fieldCount; i++) {
		DCfield *f = s->pFields + i;
		DCpointer p = (char*)value + f->offset;
		switch(f->type) {
		  case DC_SIGCHAR_STRUCT:
		  	dcArgStruct(vm, f->pSubStruct, p);
		  	break;
		  case DC_SIGCHAR_BOOL: 
			dcArgBool      (vm, *(DCbool*)p); 
			break;
		  case DC_SIGCHAR_CHAR:
		  case DC_SIGCHAR_UCHAR:
			dcArgChar      (vm, *(DCchar*)p);
			break;
		  case DC_SIGCHAR_SHORT:
		  case DC_SIGCHAR_USHORT:
			dcArgShort     (vm, *(DCshort*)p);
			break;
		  case DC_SIGCHAR_INT:
		  case DC_SIGCHAR_UINT:
			dcArgInt       (vm, *(DCint*)p);
			break;
		  case DC_SIGCHAR_LONG:
		  case DC_SIGCHAR_ULONG:
			dcArgLong      (vm, *(DCulong*)p);
			break;
		  case DC_SIGCHAR_LONGLONG:
		  case DC_SIGCHAR_ULONGLONG:
			dcArgLongLong  (vm, *(DCulonglong*)p);
			break;
		  case DC_SIGCHAR_FLOAT:
			dcArgFloat     (vm, *(DCfloat*)p);
			break;
		  case DC_SIGCHAR_DOUBLE:
			dcArgDouble    (vm, *(DCdouble*)p);
			break;
		  case DC_SIGCHAR_POINTER:
		  case DC_SIGCHAR_STRING:
			dcArgPointer   (vm, *(DCpointer**)p);
			break;
	       default:
	       	assert(0);
		}	
	}
}


static DCint readInt(const char** ptr)
{
	return strtol(*ptr, (char**)ptr, 10);/*@@@ enough*/
}


DCstruct* dcDefineStruct(const char* signature)
{
	DCstruct* s;
	const char* ptr = signature;
	DCint fieldCount = readInt(&ptr);
	s = dcNewStruct(fieldCount, DEFAULT_ALIGNMENT);
	
	while (*ptr) {
		char type = *(ptr++);
		if (type == DC_SIGCHAR_STRUCT) {
			/*dcSubStruct(	@@@*/
		} else {
			dcStructField(s, type, DEFAULT_ALIGNMENT, readInt(&ptr));
		}
	}
	dcCloseStruct(s);
	return s;
}

