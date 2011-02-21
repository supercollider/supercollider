/*
	SuperCollider real time audio synthesis system
	Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com

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
/*

Primitives for File i/o.

*/


#include "GC.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "PyrSymbol.h"
#include "PyrFilePrim.h"
#include "PyrFileUtils.h"
#include "ReadWriteMacros.h"
#include "SCBase.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <cerrno>

#ifdef NOCLASSIC
#include <TextUtils.h>
#include <Navigation.h>
#endif

#ifndef SC_WIN32
# include <unistd.h>
#else
# include <direct.h>
# include <malloc.h>
# define strcasecmp stricmp
#endif

#ifdef SC_WIN32
#include "SC_Win32Utils.h"
#include "SC_DirUtils.h"
#endif

#include <fcntl.h>
#include <math.h>

#define DELIMITOR ':'

bool filelen(FILE *file, size_t *length);

int prFileDelete(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	char filename[PATH_MAX];

	a = g->sp - 1;
	b = g->sp;
	if (NotObj(b) || !isKindOf(slotRawObject(b), class_string))
		return errWrongType;
	if (slotRawObject(b)->size > PATH_MAX - 1) return errFailed;

	memcpy(filename, slotRawString(b)->s, slotRawObject(b)->size);
	filename[slotRawString(b)->size] = 0;

	int err = unlink(filename);
	SetBool(a, err == 0);

	return errNone;
}


int prFileOpen(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	char filename[PATH_MAX];
	char mode[12];
	PyrFile *pfile;
	FILE *file;

	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;
	if (NotObj(c) || !isKindOf(slotRawObject(c), class_string)
		|| NotObj(b) || !isKindOf(slotRawObject(b), class_string))
		return errWrongType;
	if (slotRawObject(b)->size > PATH_MAX - 1) return errFailed;
	if (slotRawObject(c)->size > 11) return errFailed;
	pfile = (PyrFile*)slotRawObject(a);

	memcpy(filename, slotRawString(b)->s, slotRawObject(b)->size);
	filename[slotRawString(b)->size] = 0;

	memcpy(mode, slotRawString(c)->s, slotRawObject(c)->size);
	mode[slotRawString(c)->size] = 0;

#ifdef SC_WIN32
	win32_ReplaceCharInString(filename,PATH_MAX,'/','\\');
	if(strcmp(mode,"w") == 0)
	strcpy(mode,"wb");
	if(strcmp(mode,"r") == 0)
	strcpy(mode,"rb");
#endif
	//SC_WIN32
	file = fopen(filename, mode);
	if (file) {
		SetPtr(&pfile->fileptr, file);
		SetTrue(a);
	} else {
#ifdef SC_WIN32
		// check if directory exisits
		// create a temporary file (somewhere) for a handle
		// the file is deleted automatically when closed
		if (sc_DirectoryExists(filename)) {
			int err;
			err = tmpfile_s(&file);
			if (!err) {
				SetPtr(&pfile->fileptr, file);
				SetTrue(a);
				return errNone;
			}
		}
#endif
		SetNil(a);
		SetFalse(a);
	}
	return errNone;
}

int prFileClose(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;

	a = g->sp;
	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errNone;
	SetPtr(&pfile->fileptr, NULL);
	if (fclose(file)) return errFailed;
	return errNone;
}

int prFileFlush(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;

	a = g->sp;
	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file != NULL) fflush(file);
	return errNone;
}

int prFilePos(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;
	fpos_t pos;
	int length;

	a = g->sp;
	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;
	if (fgetpos(file, &pos)) return errFailed;

#ifdef SC_LINUX
	// sk: hack alert!
	length = pos.__pos;
#else
	length = pos;
#endif

	SetInt(a, length);
	return errNone;
}

int prFileLength(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;
	fpos_t pos;
	size_t length;

	a = g->sp;
	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;
	// preserve file position
	if (fgetpos(file, &pos)) return errFailed;
	if (filelen(file, &length)) return errFailed;
	if (fsetpos(file, &pos)) return errFailed;

	SetInt(a, length);
	return errNone;
}

int prFileSeek(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	PyrFile *pfile;
	FILE *file;
	size_t offset;
	int origin;
	static int originTable[3] = { SEEK_SET, SEEK_CUR, SEEK_END };

	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;
	if (NotInt(b)) return errWrongType;
	if (NotInt(c)) return errWrongType;
	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;
	offset = slotRawInt(b);
	origin = slotRawInt(c);
	if (origin < 0 || origin > 2) return errIndexOutOfRange;
	origin = originTable[origin]; // translate in case ANSI constants ever change..
	if (fseek(file, offset, origin)) return errFailed;
	return errNone;
}


int prFileWrite(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *ptr;
	PyrFile *pfile;
	FILE *file;
	PyrObject *obj;
	char chr;

	a = g->sp - 1;
	b = g->sp;
	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;
	switch (GetTag(b)) {
		case tagInt :
		{
			SC_IOStream<FILE*> scio(file);
			scio.writeInt32_be(slotRawInt(b));
			break;
		}
		case tagSym :
			fwrite(slotRawSymbol(b)->name, sizeof(char), slotRawSymbol(b)->length, file);
			break;
		case tagChar :
			chr = slotRawChar(b);
			fwrite(&chr, sizeof(char), 1, file);
			break;
		case tagNil :
		case tagFalse :
		case tagTrue :
		case tagPtr :
			return errWrongType;
		case tagObj :
		{
			// writes the indexable part of any non obj_slot format object
			obj = slotRawObject(b);
			if (!isKindOf(obj, class_rawarray)
				|| isKindOf(obj, class_symbolarray)) return errWrongType;
			if (obj->size) {
				ptr = obj->slots;
				int elemSize = gFormatElemSize[obj->obj_format];
				int numElems = obj->size;
	#if BYTE_ORDER != BIG_ENDIAN
				switch (elemSize) {
					case 1:
						fwrite(ptr, elemSize, numElems, file);
						break;
					case 2:
					{
						char *ptr = slotRawString(b)->s;
						char *ptrend = ptr + numElems*2;
						for (; ptr < ptrend; ptr+=2) {
							fputc(ptr[1], file);
							fputc(ptr[0], file);
						}
						break;
					}
					case 4:
					{
						char *ptr = slotRawString(b)->s;
						char *ptrend = ptr + numElems*4;
						for (; ptr < ptrend; ptr+=4) {
							fputc(ptr[3], file);
							fputc(ptr[2], file);
							fputc(ptr[1], file);
							fputc(ptr[0], file);
						}
						break;
					}
					case 8:
					{
						char *ptr = slotRawString(b)->s;
						char *ptrend = ptr + numElems*8;
						for (; ptr < ptrend; ptr+=8) {
							fputc(ptr[7], file);
							fputc(ptr[6], file);
							fputc(ptr[5], file);
							fputc(ptr[4], file);
							fputc(ptr[3], file);
							fputc(ptr[2], file);
							fputc(ptr[1], file);
							fputc(ptr[0], file);
						}
						break;
					}
				}
	#else
				fwrite(ptr, elemSize, numElems, file);
	#endif
			}
			break;
		}
		default : // double
		{
			SC_IOStream<FILE*> scio(file);
			scio.writeDouble_be(slotRawFloat(b));
			break;
		}
	}
	return errNone;
}


int prFileWriteLE(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *ptr;
	PyrFile *pfile;
	FILE *file;
	PyrObject *obj;
	char chr;

	a = g->sp - 1;
	b = g->sp;
	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;
	switch (GetTag(b)) {
		case tagInt :
		{
			SC_IOStream<FILE*> scio(file);
			scio.writeInt32_le(slotRawInt(b));
			break;
		}
		case tagSym :
			fwrite(slotRawSymbol(b)->name, sizeof(char), slotRawSymbol(b)->length, file);
			break;
		case tagChar :
			chr = slotRawInt(b);
			fwrite(&chr, sizeof(char), 1, file);
			break;
		case tagNil :
		case tagFalse :
		case tagTrue :
		case tagPtr :
			return errWrongType;
		case tagObj :
		{
			// writes the indexable part of any non obj_slot format object
			obj = slotRawObject(b);
			if (!isKindOf(obj, class_rawarray)
				|| isKindOf(obj, class_symbolarray)) return errWrongType;
			if (obj->size) {
				ptr = obj->slots;
				int elemSize = gFormatElemSize[obj->obj_format];
				int numElems = obj->size;
#if BYTE_ORDER == BIG_ENDIAN
				switch (elemSize) {
					case 1:
						fwrite(ptr, elemSize, numElems, file);
						break;
					case 2:
					{
						char *ptr = slotRawString(b)->s;
						char *ptrend = ptr + numElems*2;
						for (; ptr < ptrend; ptr+=2) {
							fputc(ptr[1], file);
							fputc(ptr[0], file);
						}
						break;
					}
					case 4:
					{
						char *ptr = slotRawString(b)->s;
						char *ptrend = ptr + numElems*4;
						for (; ptr < ptrend; ptr+=4) {
							fputc(ptr[3], file);
							fputc(ptr[2], file);
							fputc(ptr[1], file);
							fputc(ptr[0], file);
						}
						break;
					}
					case 8:
					{
						char *ptr = slotRawString(b)->s;
						char *ptrend = ptr + numElems*8;
						for (; ptr < ptrend; ptr+=8) {
							fputc(ptr[7], file);
							fputc(ptr[6], file);
							fputc(ptr[5], file);
							fputc(ptr[4], file);
							fputc(ptr[3], file);
							fputc(ptr[2], file);
							fputc(ptr[1], file);
							fputc(ptr[0], file);
						}
						break;
					}
				}
#else
				fwrite(ptr, elemSize, numElems, file);
#endif
			}
			break;
		}
		default : // double
		{
			SC_IOStream<FILE*> scio(file);
			scio.writeDouble_le(slotRawFloat(b));
			break;
		}
	}
	return errNone;
}

int prFileReadLine(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b; // receiver(a File), string
	PyrFile *pfile;
	FILE *file;

	a = g->sp - 1;
	b = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	char* result = fgets(slotRawString(b)->s, MAXINDEXSIZE(slotRawObject(b)) - 1, file);
	if (!result) {
		SetNil(a);
	} else {
		slotRawString(b)->size = strlen(slotRawString(b)->s);
		if (slotRawString(b)->s[slotRawString(b)->size-1] == '\n') slotRawString(b)->size--;
		slotCopy(a,b);
	}
	return errNone;
}

int prFilePutInt32(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;

	a = g->sp - 1;
	b = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

        int val;
        int err = slotIntVal(b, &val);
        if (err) return err;

	SC_IOStream<FILE*> scio(file);
	scio.writeInt32_be(val);

	return errNone;
}

int prFilePutInt16(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;

	a = g->sp - 1;
	b = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	int val;
	int err = slotIntVal(b, &val);
	if (err) return err;


	SC_IOStream<FILE*> scio(file);
	scio.writeInt16_be(val);

	return errNone;
}


int prFilePutInt32LE(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;

	a = g->sp - 1;
	b = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

        int val;
        int err = slotIntVal(b, &val);
        if (err) return err;

	SC_IOStream<FILE*> scio(file);
	scio.writeInt32_le(val);

	return errNone;
}

int prFilePutInt16LE(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;

	a = g->sp - 1;
	b = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	int val;
	int err = slotIntVal(b, &val);
	if (err) return err;


	SC_IOStream<FILE*> scio(file);
	scio.writeInt16_le(val);

	return errNone;
}

int prFilePutInt8(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;

	a = g->sp - 1;
	b = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	int val;
	int err = slotIntVal(b, &val);
	if (err) return err;


	SC_IOStream<FILE*> scio(file);
	scio.writeInt8(val);

	return errNone;
}

int prFilePutChar(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;
	char z;

	a = g->sp - 1;
	b = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;
	if (NotChar(b)) return errWrongType;

	z = slotRawInt(b);

	SC_IOStream<FILE*> scio(file);
	scio.writeInt8(z);

	return errNone;
}

int prFilePutFloat(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;

	a = g->sp - 1;
	b = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) {
		dumpObjectSlot(a);
		return errFailed;
	}

	float val;
	int err = slotFloatVal(b, &val);
	if (err) return err;

	SC_IOStream<FILE*> scio(file);
	scio.writeFloat_be(val);

	return errNone;
}

int prFilePutDouble(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;

	a = g->sp - 1;
	b = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;


	double val;
	int err = slotDoubleVal(b, &val);
	if (err) return err;

	SC_IOStream<FILE*> scio(file);
	scio.writeDouble_be(val);

	return errNone;
}


int prFilePutFloatLE(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;

	a = g->sp - 1;
	b = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) {
		dumpObjectSlot(a);
		return errFailed;
	}

	float val;
	int err = slotFloatVal(b, &val);
	if (err) return err;

	SC_IOStream<FILE*> scio(file);
	scio.writeFloat_le(val);

	return errNone;
}

int prFilePutDoubleLE(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;

	a = g->sp - 1;
	b = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;


	double val;
	int err = slotDoubleVal(b, &val);
	if (err) return err;

	SC_IOStream<FILE*> scio(file);
	scio.writeDouble_le(val);

	return errNone;
}

int prFilePutString(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;
	PyrString *string;

	a = g->sp - 1;
	b = g->sp;
	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;
	if (NotObj(b) || slotRawObject(b)->classptr != class_string) return errWrongType;
	string = slotRawString(b);
	if (string->size) {
		fwrite(string->s, 1, string->size, file);
	}
	return errNone;
}


int prFileGetDouble(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;

	a = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	if (feof(file)) SetNil(a);
	else {
		SC_IOStream<FILE*> scio(file);
		SetFloat(a, scio.readDouble_be());
	}
	return errNone;
}

int prFileGetFloat(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;

	a = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	if (feof(file)) SetNil(a);
	else {
		SC_IOStream<FILE*> scio(file);
		SetFloat(a, scio.readFloat_be());
	}
	return errNone;
}


int prFileGetDoubleLE(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;

	a = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	if (feof(file)) SetNil(a);
	else {
		SC_IOStream<FILE*> scio(file);
		SetFloat(a, scio.readDouble_le());
	}
	return errNone;
}

int prFileGetFloatLE(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;

	a = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	if (feof(file)) SetNil(a);
	else {
		SC_IOStream<FILE*> scio(file);
		SetFloat(a, scio.readFloat_le());
	}
	return errNone;
}

int prFileGetChar(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;
	char z;

	a = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	int count = fread(&z, sizeof(char), 1, file);
	if (count==0) SetNil(a);
	else SetChar(a, z);
	return errNone;
}

int prFileGetInt8(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;
	char z;

	a = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	int count = fread(&z, sizeof(char), 1, file);
	if (count==0) SetNil(a);
	else SetInt(a, z);
	return errNone;
}

int prFileGetInt16(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;

	a = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	if (feof(file)) SetNil(a);
	else {
		SC_IOStream<FILE*> scio(file);
		SetInt(a, scio.readInt16_be());
	}
	return errNone;
}

int prFileGetInt32(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;

	a = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL)
    return errFailed;

	if (feof(file)) SetNil(a);
	else {
		SC_IOStream<FILE*> scio(file);
		SetInt(a, scio.readInt32_be());
	}
	return errNone;
}


int prFileGetInt16LE(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;

	a = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	if (feof(file)) SetNil(a);
	else {
		SC_IOStream<FILE*> scio(file);
		SetInt(a, scio.readInt16_le());
	}
	return errNone;
}

int prFileGetInt32LE(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;

	a = g->sp;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	if (feof(file)) SetNil(a);
	else {
		SC_IOStream<FILE*> scio(file);
		SetInt(a, scio.readInt32_le());
	}
	return errNone;
}

int prFileReadRaw(struct VMGlobals *g, int numArgsPushed)
{
	PyrFile *pfile;
	FILE *file;

	PyrSlot* a = g->sp - 1;
	PyrSlot* b = g->sp;

	if (!isKindOfSlot(b, class_rawarray)
		|| isKindOfSlot(b, class_symbolarray)) return errWrongType;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	int elemSize = gFormatElemSize[slotRawObject(b)->obj_format];
	int numElems = slotRawObject(b)->size;
	numElems = fread(slotRawString(b)->s, elemSize, numElems, file);
	slotRawObject(b)->size = numElems;

#if BYTE_ORDER != BIG_ENDIAN
	switch (elemSize) {
		case 1:
			break;
		case 2:
		{
			char *ptr = slotRawString(b)->s;
			char *ptrend = ptr + numElems*2;
			for (; ptr < ptrend; ptr+=2) {
				char temp = ptr[0];
				ptr[0] = ptr[1];
				ptr[1] = temp;
			}
			break;
		}
		case 4:
		{
			char *ptr = slotRawString(b)->s;
			char *ptrend = ptr + numElems*4;
			for (; ptr < ptrend; ptr+=4) {
				char temp = ptr[0];
				ptr[0] = ptr[3];
				ptr[3] = temp;

				temp = ptr[1];
				ptr[1] = ptr[2];
				ptr[2] = temp;
			}
			break;
		}
		case 8:
		{
			char *ptr = slotRawString(b)->s;
			char *ptrend = ptr + numElems*8;
			for (; ptr < ptrend; ptr+=8) {
				char temp = ptr[0];
				ptr[0] = ptr[7];
				ptr[7] = temp;

				temp = ptr[1];
				ptr[1] = ptr[6];
				ptr[6] = temp;

				temp = ptr[2];
				ptr[2] = ptr[5];
				ptr[5] = temp;

				temp = ptr[3];
				ptr[3] = ptr[4];
				ptr[4] = temp;
			}
			break;
		}
	}
#endif

	if (slotRawObject(b)->size==0) SetNil(a);
	else slotCopy(a,b);
	return errNone;
}

int prFileReadRawLE(struct VMGlobals *g, int numArgsPushed)
{
	PyrFile *pfile;
	FILE *file;

	PyrSlot* a = g->sp - 1;
	PyrSlot* b = g->sp;

	if (!isKindOfSlot(b, class_rawarray)
		|| isKindOfSlot(b, class_symbolarray)) return errWrongType;

	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errFailed;

	int elemSize = gFormatElemSize[slotRawObject(b)->obj_format];
	int numElems = slotRawObject(b)->size;
	numElems = fread(slotRawString(b)->s, elemSize, numElems, file);
	slotRawObject(b)->size = numElems;

#if BYTE_ORDER == BIG_ENDIAN
	switch (elemSize) {
		case 1:
			break;
		case 2:
		{
			char *ptr = slotRawString(b)->s;
			char *ptrend = ptr + numElems*2;
			for (; ptr < ptrend; ptr+=2) {
				char temp = ptr[0];
				ptr[0] = ptr[1];
				ptr[1] = temp;
			}
			break;
		}
		case 4:
		{
			char *ptr = slotRawString(b)->s;
			char *ptrend = ptr + numElems*4;
			for (; ptr < ptrend; ptr+=4) {
				char temp = ptr[0];
				ptr[0] = ptr[3];
				ptr[3] = temp;

				temp = ptr[1];
				ptr[1] = ptr[2];
				ptr[2] = temp;
			}
			break;
		}
		case 8:
		{
			char *ptr = slotRawString(b)->s;
			char *ptrend = ptr + numElems*8;
			for (; ptr < ptrend; ptr+=8) {
				char temp = ptr[0];
				ptr[0] = ptr[7];
				ptr[7] = temp;

				temp = ptr[1];
				ptr[1] = ptr[6];
				ptr[6] = temp;

				temp = ptr[2];
				ptr[2] = ptr[5];
				ptr[5] = temp;

				temp = ptr[3];
				ptr[3] = ptr[4];
				ptr[4] = temp;
			}
			break;
		}
	}
#endif

	if (slotRawObject(b)->size==0) SetNil(a);
	else slotCopy(a,b);
	return errNone;
}

int prFileGetcwd(struct VMGlobals *g, int numArgsPushed)
{
	//PyrSlot* a = g->sp - 1; // File
	PyrSlot* string = g->sp;

	if (!isKindOfSlot(string, class_string))  return errWrongType;

	char * cwd = getcwd(slotRawString(string)->s,255);
	if (cwd == NULL) {
		error(strerror(errno));
		return errFailed;
	}
	slotRawString(string)->size = strlen(slotRawString(string)->s);

	return errNone;
}

////////

#ifndef SC_WIN32
int prPipeOpen(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	char mode[12];
	PyrFile *pfile;
	FILE *file;

	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;

	if (NotObj(c) || !isKindOf(slotRawObject(c), class_string)
		|| NotObj(b) || !isKindOf(slotRawObject(b), class_string))
		return errWrongType;
	if (slotRawObject(c)->size > 11) return errFailed;
	pfile = (PyrFile*)slotRawObject(a);

        char *commandLine = (char*)malloc(slotRawObject(b)->size + 1);
	memcpy(commandLine, slotRawString(b)->s, slotRawObject(b)->size);
	commandLine[slotRawString(b)->size] = 0;

	memcpy(mode, slotRawString(c)->s, slotRawObject(c)->size);
	mode[slotRawString(c)->size] = 0;

	file = popen(commandLine, mode);
	free(commandLine);
	if (file) {
		SetPtr(&pfile->fileptr, file);
		SetTrue(a);
	} else {
		SetNil(a);
		SetFalse(a);
	}
	return errNone;
}

int prPipeClose(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;

	a = g->sp;
	pfile = (PyrFile*)slotRawObject(a);
	file = (FILE*)slotRawPtr(&pfile->fileptr);
	if (file == NULL) return errNone;
	SetPtr(&pfile->fileptr, NULL);
	int perr = pclose(file);
	SetInt(a, perr);
	if (perr == -1)
		return errFailed;
	return errNone;
}
#endif

////////

#ifdef SC_WIN32
	#include <sndfile-win.h>
#else
	#include <sndfile.h>
#endif

int sampleFormatToString(struct SF_INFO *info, const char **string);
int sampleFormatToString(struct SF_INFO *info, const char **string)
{
	unsigned int format =  info->format & SF_FORMAT_SUBMASK;
	switch (format)
	{
		case SF_FORMAT_DPCM_8:
		case SF_FORMAT_PCM_S8:
			*string = "int16";
			break;
		case SF_FORMAT_DPCM_16:
		case SF_FORMAT_PCM_16:
		case SF_FORMAT_DWVW_16:
			*string = "int16";
			break;
		case SF_FORMAT_PCM_24:
		case SF_FORMAT_DWVW_24:
			*string = "int24";
			break;
		case SF_FORMAT_PCM_32:
			*string = "int32";
			break;
		case SF_FORMAT_FLOAT:
			*string = "float";
			break;
		case SF_FORMAT_DOUBLE:
			*string = "double";
			break;
		case SF_FORMAT_ULAW:
			*string = "ulaw";
			break;
		case SF_FORMAT_ALAW:
			*string = "alaw";
			break;
		default:
			*string = "float";
			break;
	}
	return errNone;
}


int headerFormatToString(struct SF_INFO *info, const char **string);
int headerFormatToString(struct SF_INFO *info, const char **string){
	switch (info->format & SF_FORMAT_TYPEMASK)
	{
		case SF_FORMAT_WAV :
				*string = "WAV";
				break;
		case SF_FORMAT_AIFF :
				*string = "AIFF";
				break ;
		case SF_FORMAT_AU :
				*string = "SUN";
				break ;
		case SF_FORMAT_IRCAM :
				*string = "IRCAM";
				break ;
		case SF_FORMAT_RAW :
				*string = "raw";
				break ;
		case SF_FORMAT_W64 :
				*string = "WAV";
				break ;
		case SF_FORMAT_FLAC :
				*string = "FLAC";
				break ;
// TODO allow other platforms to know vorbis once libsndfile 1.0.18 is established
#if SC_DARWIN || SC_WIN32 || LIBSNDFILE_1018
		case SF_FORMAT_VORBIS :
				*string = "vorbis";
				break ;
#endif
/*
		case SF_FORMAT_PAF :

				break ;

		case SF_FORMAT_SVX :

				break ;

		case SF_FORMAT_NIST :

				break ;

		case SF_FORMAT_VOC :

				break ;

		case SF_FORMAT_MAT4 :

				break ;

		case SF_FORMAT_MAT5 :

				break ;

		case SF_FORMAT_PVF :

				break ;

		case SF_FORMAT_XI :

				break ;

		case SF_FORMAT_HTK :

				break ;

		case SF_FORMAT_SDS :

				break ;
*/
		default :
			*string = " ";
		break ;
		}
return errNone;
}

int sndfileFormatInfoToStrings(struct SF_INFO *info, const char **stringHead,  const char **stringSample);
int sndfileFormatInfoToStrings(struct SF_INFO *info, const char **stringHead,  const char **stringSample)
{
	int error = 0;
	error = headerFormatToString(info, stringHead);
	error = sampleFormatToString(info, stringSample);
	return error;
}

int prSFOpenRead(struct VMGlobals *g, int numArgsPushed);
int prSFOpenRead(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	char filename[PATH_MAX];
	SNDFILE *file;
	SF_INFO info;
	const char *headerstr;
	const char *sampleformatstr;

	a = g->sp - 1;
	b = g->sp;

	if (!isKindOfSlot(b, class_string)) return errWrongType;
	if (slotRawObject(b)->size > PATH_MAX - 1) return errFailed;

	memcpy(filename, slotRawString(b)->s, slotRawObject(b)->size);
	filename[slotRawString(b)->size] = 0;

	info.format = 0;
	file = sf_open(filename, SFM_READ, &info);


	if (file) {
		SetPtr(slotRawObject(a)->slots + 0, file);
		sndfileFormatInfoToStrings(&info, &headerstr, &sampleformatstr);
		//headerFormatToString(&info, &headerstr);
		PyrString *hpstr = newPyrString(g->gc, headerstr, 0, true);
		SetObject(slotRawObject(a)->slots+1, hpstr);
		PyrString *smpstr = newPyrString(g->gc, sampleformatstr, 0, true);
		SetObject(slotRawObject(a)->slots+2, smpstr);
		SetInt(slotRawObject(a)->slots + 3, info.frames);
		SetInt(slotRawObject(a)->slots + 4, info.channels);
		SetInt(slotRawObject(a)->slots + 5, info.samplerate);

		SetTrue(a);
	} else {
		SetNil(a);
		SetFalse(a);
	}
	return errNone;
}
/// copied from SC_World.cpp:

int sampleFormatFromString(const char* name);
int sampleFormatFromString(const char* name)
{
	if (!name) return SF_FORMAT_PCM_16;

	size_t len = strlen(name);
	if (len < 1) return 0;

	if (name[0] == 'u') {
		if (len < 5) return 0;
		if (name[4] == '8') return SF_FORMAT_PCM_U8; // uint8
		return 0;
	} else if (name[0] == 'i') {
		if (len < 4) return 0;
		if (name[3] == '8') return SF_FORMAT_PCM_S8;      // int8
		else if (name[3] == '1') return SF_FORMAT_PCM_16; // int16
		else if (name[3] == '2') return SF_FORMAT_PCM_24; // int24
		else if (name[3] == '3') return SF_FORMAT_PCM_32; // int32
	} else if (name[0] == 'f') {
		return SF_FORMAT_FLOAT; // float
	} else if (name[0] == 'd') {
		return SF_FORMAT_DOUBLE; // double
	} else if (name[0] == 'm' || name[0] == 'u') {
		return SF_FORMAT_ULAW; // mulaw ulaw
	} else if (name[0] == 'a') {
		return SF_FORMAT_ALAW; // alaw
	}
	return 0;
}

int headerFormatFromString(const char *name);
int headerFormatFromString(const char *name)
{
	if (!name) return SF_FORMAT_AIFF;
	if (strcasecmp(name, "AIFF")==0) return SF_FORMAT_AIFF;
	if (strcasecmp(name, "AIFC")==0) return SF_FORMAT_AIFF;
	if (strcasecmp(name, "RIFF")==0) return SF_FORMAT_WAV;
	if (strcasecmp(name, "WAVEX")==0) return SF_FORMAT_WAVEX;
	if (strcasecmp(name, "WAVE")==0) return SF_FORMAT_WAV;
	if (strcasecmp(name, "WAV" )==0) return SF_FORMAT_WAV;
	if (strcasecmp(name, "Sun" )==0) return SF_FORMAT_AU;
	if (strcasecmp(name, "IRCAM")==0) return SF_FORMAT_IRCAM;
	if (strcasecmp(name, "NeXT")==0) return SF_FORMAT_AU;
	if (strcasecmp(name, "raw")==0) return SF_FORMAT_RAW;
	if (strcasecmp(name, "MAT4")==0) return SF_FORMAT_MAT4;
	if (strcasecmp(name, "MAT5")==0) return SF_FORMAT_MAT5;
	if (strcasecmp(name, "PAF")==0) return SF_FORMAT_PAF;
	if (strcasecmp(name, "SVX")==0) return SF_FORMAT_SVX;
	if (strcasecmp(name, "NIST")==0) return SF_FORMAT_NIST;
	if (strcasecmp(name, "VOC")==0) return SF_FORMAT_VOC;
	if (strcasecmp(name, "W64")==0) return SF_FORMAT_W64;
	if (strcasecmp(name, "PVF")==0) return SF_FORMAT_PVF;
	if (strcasecmp(name, "XI")==0) return SF_FORMAT_XI;
	if (strcasecmp(name, "HTK")==0) return SF_FORMAT_HTK;
	if (strcasecmp(name, "SDS")==0) return SF_FORMAT_SDS;
	if (strcasecmp(name, "AVR")==0) return SF_FORMAT_AVR;
	if (strcasecmp(name, "SD2")==0) return SF_FORMAT_SD2;
	if (strcasecmp(name, "FLAC")==0) return SF_FORMAT_FLAC;
	if (strcasecmp(name, "CAF")==0) return SF_FORMAT_CAF;
// TODO allow other platforms to know vorbis once libsndfile 1.0.18 is established
#if SC_DARWIN || SC_WIN32
	if (strcasecmp(name, "VORBIS")==0) return SF_FORMAT_VORBIS;
#endif
	return 0;
}


int sndfileFormatInfoFromStrings(struct SF_INFO *info, const char *headerFormatString, const char *sampleFormatString)
{
	int headerFormat = headerFormatFromString(headerFormatString);
	if (!headerFormat) return errWrongType;

	int sampleFormat = sampleFormatFromString(sampleFormatString);
	if (!sampleFormat) return errWrongType;

	info->format = (unsigned int)(headerFormat | sampleFormat);
	return errNone;
}
// end copy

int prSFOpenWrite(struct VMGlobals *g, int numArgsPushed);
int prSFOpenWrite(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	char filename[PATH_MAX];
	SNDFILE *file;
	SF_INFO info;
	PyrSlot *headerSlot;
	PyrSlot *formatSlot;
	int error;


	a = g->sp - 1;
	b = g->sp;

	headerSlot = (slotRawObject(a)->slots + 1);
	formatSlot = (slotRawObject(a)->slots + 2);


	if (!isKindOfSlot(headerSlot, class_string)) return errWrongType;
	if (!isKindOfSlot(formatSlot, class_string)) return errWrongType;

	if (!isKindOfSlot(b, class_string)) return errWrongType;
	if (slotRawObject(b)->size > PATH_MAX - 1) return errFailed;

	memcpy(filename, slotRawString(b)->s, slotRawObject(b)->size);
	filename[slotRawString(b)->size] = 0;

#ifdef SC_WIN32
	char* headerFormat = (char *)malloc(slotRawObject(headerSlot)->size);
#else
	char headerFormat[slotRawString(headerSlot)->size];
#endif
	memcpy(headerFormat, slotRawString(headerSlot)->s, slotRawObject(headerSlot)->size);
	headerFormat[slotRawString(headerSlot)->size] = 0;

#ifdef SC_WIN32
	char* sampleFormat = (char *)malloc(slotRawString(formatSlot)->size);
#else
	char sampleFormat[slotRawString(formatSlot)->size];
#endif
	memcpy(sampleFormat, slotRawString(formatSlot)->s, slotRawObject(formatSlot)->size);
	sampleFormat[slotRawString(formatSlot)->size] = 0;

	error = sndfileFormatInfoFromStrings(&info, headerFormat, sampleFormat);
	if(error) {
#ifdef SC_WIN32
		free(sampleFormat);
		free(headerFormat);
#endif
		return errFailed;
	}

	if(error) 	return errFailed;
	//slotIntVal(slotRawObject(a)->slots + 3, &info.frames);
	slotIntVal(slotRawObject(a)->slots + 4, &info.channels);
	slotIntVal(slotRawObject(a)->slots + 5, &info.samplerate);

	file = sf_open(filename, SFM_WRITE, &info);
	if (file) {
		SetPtr(slotRawObject(a)->slots+0, file);
		SetTrue(a);
	} else {
		SetNil(a);
		SetFalse(a);
	}

	return errNone;
}

int prSFClose(struct VMGlobals *g, int numArgsPushed);
int prSFClose(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;

	a = g->sp;

	SNDFILE *file = (SNDFILE*)slotRawPtr(&slotRawObject(a)->slots[0]);
	if (file) {
		sf_close(file);
		SetNil(slotRawObject(a)->slots + 0);
	}

	return errNone;
}

int prSFRead(struct VMGlobals *g, int numArgsPushed);
int prSFRead(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;

	a = g->sp - 1;
	b = g->sp;

	SNDFILE *file = (SNDFILE*)slotRawPtr(&slotRawObject(a)->slots[0]);

	if (!isKindOfSlot(b, class_rawarray)) return errWrongType;

	switch (slotRawObject(b)->obj_format) {
		case obj_int16 :
			slotRawObject(b)->size = sf_read_short(file, (short*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
			break;
		case obj_int32 :
			slotRawObject(b)->size = sf_read_int(file, (int*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
			break;
		case obj_float :
			slotRawObject(b)->size = sf_read_float(file, (float*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
			break;
		case obj_double :
			slotRawObject(b)->size = sf_read_double(file, (double*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
			break;
		default:
			error("sample format not supported.\n");
			return errFailed;
	}

	return errNone;
}

int prSFWrite(struct VMGlobals *g, int numArgsPushed);
int prSFWrite(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;

	a = g->sp - 1;
	b = g->sp;

	SNDFILE *file = (SNDFILE*)slotRawPtr(&slotRawObject(a)->slots[0]);

	if (!isKindOfSlot(b, class_rawarray)) return errWrongType;

	switch (slotRawObject(b)->obj_format) {
		case obj_int16 :
			sf_write_short(file, (short*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
			break;
		case obj_int32 :
			sf_write_int(file, (int*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
			break;
		case obj_float :
			sf_write_float(file, (float*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
			break;
		case obj_double :
			sf_write_double(file, (double*)slotRawInt8Array(b)->b, slotRawObject(b)->size);
			break;
		default:
			error("sample format not supported.\n");
			return errFailed;
	}

	return errNone;
}

int prSFSeek(struct VMGlobals *g, int numArgsPushed);
int prSFSeek(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;

	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;

	SNDFILE *file = (SNDFILE*)slotRawPtr(&slotRawObject(a)->slots[0]);

	int origin, offset;
	int err = slotIntVal(b, &offset);
	if (err) return err;

	err = slotIntVal(c, &origin);
	if (err) return err;

	sf_seek(file, offset, origin);

	return errNone;
}

int prSFHeaderInfoString(struct VMGlobals *g, int numArgsPushed);
int prSFHeaderInfoString(struct VMGlobals *g, int numArgsPushed)
{

	PyrSlot *a;
	a = g->sp;
	SNDFILE *file = (SNDFILE*)slotRawPtr(&slotRawObject(a)->slots[0]);
	if(file){
		static	char	strbuffer [(1 << 16)] ;
		sf_command (file, SFC_GET_LOG_INFO, strbuffer, (1 << 16)) ;
		PyrString *pstring = newPyrString(g->gc, strbuffer, 0, true);
		// post(strbuffer);
		SetObject(a, pstring);
		return errNone;
	}
	return errFailed;
}


//////////
#ifdef NOCLASSIC

int dir_Lookup(char *pathString, int pathStringLength, int index,
  /* outputs: */
  char *name, int *nameLength, int *creationDate, int *modificationDate,
  int *isDirectory, int *isVisible, int *sizeIfFile);

int prDirectory_At(struct VMGlobals *g, int numArgsPushed);
int prDirectory_At(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	PyrSlot *dirPathSlot = slotRawObject(a)->slots + 0;
	int err, index;
	err = slotIntVal(c, &index);
	if (err) {
		SetNil(a);
		return err;
	}

	char name[256], fullPathName[256];
	int nameLength, creationDate, modificationDate, isDirectory, isVisible, sizeIfFile;
	int dirPathLength = slotRawObject(dirPathSlot)->size;

	err = dir_Lookup(slotRawObject(dirPathSlot)s->s, dirPathLength, index+1,
		name, &nameLength, &creationDate, &modificationDate, &isDirectory, &isVisible, &sizeIfFile);
	if (err == 1) {
		SetNil(a);
		return errNone;
	}
	if (err) {
		error("Invalid path\n");
		SetNil(a);
		return errFailed;
	}

	if (dirPathLength + nameLength + 1 > 255) {
		error("Full path name too long.\n");
		SetNil(a);
		return errFailed;
	}

	PyrSlot *entryName = slotRawObject(b)->slots + 0;
	PyrSlot *entryPath = slotRawObject(b)->slots + 1;
	PyrSlot *entryIsDir = slotRawObject(b)->slots + 2;
	PyrSlot *entryIsVisible = slotRawObject(b)->slots + 3;

	PyrString *nameString = newPyrString(g->gc, name, 0, true);
	SetObject(entryName, nameString);
	g->gc->GCWrite(slotRawObject(b), (PyrObject*)nameString);

	memcpy(fullPathName, slotRawObject(dirPathSlot)s->s, dirPathLength);
	fullPathName[dirPathLength] = DELIMITOR;
	strcpy(fullPathName + dirPathLength + 1, name);

	PyrString *pathString = newPyrString(g->gc, fullPathName, 0, true);
	SetObject(entryPath, pathString);
	g->gc->GCWrite(slotRawObject(b), (PyrObject*)pathString);

	if (isDirectory) { SetTrue(entryIsDir); } else { SetFalse(entryIsDir); }
	if (isVisible) { SetTrue(entryIsVisible); } else { SetFalse(entryIsVisible); }

	slotCopy(a,b);

	return errNone;
}

Boolean		GetFullPathname(const FSSpec* aSpec, Str255 pathname);
void pstrncpy(unsigned char *s1, unsigned char *s2, int n);

int prFile_GetFile(struct VMGlobals *g, int numArgsPushed);
int prFile_GetFile(struct VMGlobals *g, int numArgsPushed)
{

	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

	NavDialogOptions options;

	int err = NavGetDefaultDialogOptions(&options);
	if (err) return errFailed;

	options.dialogOptionFlags |= kNavNoTypePopup;
	options.dialogOptionFlags |= kNavDontAutoTranslate;
	options.dialogOptionFlags |= kNavDontAddTranslateItems;
	options.dialogOptionFlags |= kNavSelectDefaultLocation;
	options.dialogOptionFlags &= ~kNavAllowPreviews;
	options.dialogOptionFlags &= ~kNavAllowMultipleFiles;

	if (isKindOfSlot(b, class_string)) {
		pstringFromPyrString((PyrString*)slotRawObject(b), options.message, 256);
	}

	NavReplyRecord reply;
	err = NavGetFile(0, &reply, &options, 0, 0, 0, 0, 0);

	if (err == noErr && reply.validRecord) {
		AEKeyword keyword;
		DescType actualType;
		Size actualSize;
		FSSpec fsspec;

		err = AEGetNthPtr(&reply.selection, 1, typeFSS, &keyword, &actualType,
							&fsspec, sizeof(FSSpec), &actualSize);

		if (err == noErr) {
			Str255 pathname;
			GetFullPathname(&fsspec, pathname);
			p2cstr(pathname);
			PyrString *string = newPyrString(g->gc, (char*)pathname, 0, true);
			SetObject(a, string);
		} else {
			SetNil(a);
		}
		err = NavDisposeReply(&reply);
	} else {
		SetNil(a);
	}
	return errNone;
}



int prFile_PutFile(struct VMGlobals *g, int numArgsPushed);
int prFile_PutFile(struct VMGlobals *g, int numArgsPushed)
{

	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	NavDialogOptions options;

	int err = NavGetDefaultDialogOptions(&options);
	if (err) return errFailed;

	options.dialogOptionFlags |= kNavNoTypePopup;
	options.dialogOptionFlags |= kNavDontAutoTranslate;
	options.dialogOptionFlags |= kNavDontAddTranslateItems;
	options.dialogOptionFlags |= kNavSelectDefaultLocation;
	options.dialogOptionFlags &= ~kNavAllowPreviews;
	options.dialogOptionFlags &= ~kNavAllowMultipleFiles;

	if (isKindOfSlot(b, class_string)) {
		pstringFromPyrString((PyrString*)slotRawObject(b), options.message, 256);
	}

	if (isKindOfSlot(c, class_string)) {
		pstringFromPyrString((PyrString*)slotRawObject(c), options.savedFileName, 256);
	} else {
		//pstrncpy(options.savedFileName, "\pUntitled", 255);
	}

	NavReplyRecord reply;
	err = NavPutFile(0, &reply, &options, 0, 'TEXT', 'SCjm', 0);

	if (err == noErr && reply.validRecord) {
		AEKeyword keyword;
		DescType actualType;
		Size actualSize;
		FSSpec fsspec;

		err = AEGetNthPtr(&reply.selection, 1, typeFSS, &keyword, &actualType,
							&fsspec, sizeof(FSSpec), &actualSize);

		if (err == noErr) {
			Str255 pathname;
			GetFullPathname(&fsspec, pathname);
			p2cstr(pathname);
			PyrString *string = newPyrString(g->gc, (char*)pathname, 0, true);
			SetObject(a, string);

			err = NavCompleteSave(&reply, kNavTranslateInPlace);
		} else {
			SetNil(a);
		}
		err = NavDisposeReply(&reply);
	} else {
		SetNil(a);
	}
	return errNone;
}

#endif

/////////////

void initFilePrimitives()
{
	int base, index;

	base = nextPrimitiveIndex();
	index = 0;

	definePrimitive(base, index++, "_SFOpenRead", prSFOpenRead, 2, 0);
	definePrimitive(base, index++, "_SFOpenWrite", prSFOpenWrite, 2, 0);
	definePrimitive(base, index++, "_SFClose", prSFClose, 1, 0);
	definePrimitive(base, index++, "_SFWrite", prSFWrite, 2, 0);
	definePrimitive(base, index++, "_SFRead", prSFRead, 2, 0);
	definePrimitive(base, index++, "_SFSeek", prSFSeek, 3, 0);
	definePrimitive(base, index++, "_SFHeaderInfoString", prSFHeaderInfoString, 1, 0);

#ifndef SC_WIN32
	definePrimitive(base, index++, "_PipeOpen", prPipeOpen, 3, 0);
	definePrimitive(base, index++, "_PipeClose", prPipeClose, 1, 0);
#endif

	definePrimitive(base, index++, "_FileDelete", prFileDelete, 2, 0);
	definePrimitive(base, index++, "_FileOpen", prFileOpen, 3, 0);
	definePrimitive(base, index++, "_FileClose", prFileClose, 1, 0);
	definePrimitive(base, index++, "_FileFlush", prFileFlush, 1, 0);
	definePrimitive(base, index++, "_FileSeek", prFileSeek, 3, 0);
	definePrimitive(base, index++, "_FilePos", prFilePos, 1, 0);
	definePrimitive(base, index++, "_FileLength", prFileLength, 1, 0);
	definePrimitive(base, index++, "_FileWrite", prFileWrite, 2, 0);
	definePrimitive(base, index++, "_FileWriteLE", prFileWriteLE, 2, 0);
	definePrimitive(base, index++, "_FileReadLine", prFileReadLine, 2, 0);
	definePrimitive(base, index++, "_File_getcwd", prFileGetcwd, 2, 0);

	definePrimitive(base, index++, "_FilePutChar", prFilePutChar, 2, 0);
	definePrimitive(base, index++, "_FilePutInt8", prFilePutInt8, 2, 0);
	definePrimitive(base, index++, "_FilePutInt16", prFilePutInt16, 2, 0);
	definePrimitive(base, index++, "_FilePutInt32", prFilePutInt32, 2, 0);
	definePrimitive(base, index++, "_FilePutFloat", prFilePutFloat, 2, 0);
	definePrimitive(base, index++, "_FilePutDouble", prFilePutDouble, 2, 0);
	definePrimitive(base, index++, "_FilePutInt16LE", prFilePutInt16LE, 2, 0);
	definePrimitive(base, index++, "_FilePutInt32LE", prFilePutInt32LE, 2, 0);
	definePrimitive(base, index++, "_FilePutFloatLE", prFilePutFloatLE, 2, 0);
	definePrimitive(base, index++, "_FilePutDoubleLE", prFilePutDoubleLE, 2, 0);

	definePrimitive(base, index++, "_FileGetChar", prFileGetChar, 1, 0);
	definePrimitive(base, index++, "_FileGetInt8", prFileGetInt8, 1, 0);
	definePrimitive(base, index++, "_FileGetInt16", prFileGetInt16, 1, 0);
	definePrimitive(base, index++, "_FileGetInt32", prFileGetInt32, 1, 0);
	definePrimitive(base, index++, "_FileGetFloat", prFileGetFloat, 1, 0);
	definePrimitive(base, index++, "_FileGetDouble", prFileGetDouble, 1, 0);
	definePrimitive(base, index++, "_FileGetInt16LE", prFileGetInt16LE, 1, 0);
	definePrimitive(base, index++, "_FileGetInt32LE", prFileGetInt32LE, 1, 0);
	definePrimitive(base, index++, "_FileGetFloatLE", prFileGetFloatLE, 1, 0);
	definePrimitive(base, index++, "_FileGetDoubleLE", prFileGetDoubleLE, 1, 0);

	definePrimitive(base, index++, "_FilePutString", prFilePutString, 2, 0);

	definePrimitive(base, index++, "_FileReadRaw", prFileReadRaw, 2, 0);
	definePrimitive(base, index++, "_FileReadRawLE", prFileReadRawLE, 2, 0);

#ifdef NOCLASSIC
	definePrimitive(base, index++, "_Directory_At", prDirectory_At, 3, 0);
	definePrimitive(base, index++, "_File_GetFile", prFile_GetFile, 2, 0);
	definePrimitive(base, index++, "_File_PutFile", prFile_PutFile, 3, 0);
#endif
}






