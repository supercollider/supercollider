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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/*

Primitives for File i/o.

*/

#include "GC.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "PyrFilePrim.h"
#include "PyrFileUtils.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifdef NOCLASSIC
#include <TextUtils.h>
#include <Navigation.h>
#endif

#include <unistd.h>
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
	if (b->utag != tagObj || !isKindOf(b->uo, class_string))
		return errWrongType; 
	if (b->uo->size > PATH_MAX - 1) return errFailed;
	
	memcpy(filename, b->uos->s, b->uo->size);
	filename[b->uos->size] = 0;
	
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
	if (c->utag != tagObj || !isKindOf(c->uo, class_string)
		|| b->utag != tagObj || !isKindOf(b->uo, class_string))
		return errWrongType; 
	if (b->uo->size > PATH_MAX - 1) return errFailed;
	if (c->uo->size > 11) return errFailed;
	pfile = (PyrFile*)a->uo;
	
	memcpy(filename, b->uos->s, b->uo->size);
	filename[b->uos->size] = 0;
	
	memcpy(mode, c->uos->s, c->uo->size);
	mode[c->uos->size] = 0;
	
	file = fopen(filename, mode);
	if (file) {
		SetPtr(&pfile->fileptr, file);
		SetTrue(a);
	} else {
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
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errNone;
	SetPtr(&pfile->fileptr, NULL);
	if (fclose(file)) return errFailed;
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
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
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
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
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
	if (b->utag != tagInt) return errWrongType;
	if (c->utag != tagInt) return errWrongType;
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;
	offset = b->ui;
	origin = c->ui;
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
	int elemsize;
	
	a = g->sp - 1;
	b = g->sp;
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;
	switch (b->utag) {
		case tagInt :
		case tagSym :
			fwrite(b->us->name, sizeof(char), b->us->length, file);
			break;
		case tagChar :
			chr = b->ui;
			fwrite(&chr, sizeof(char), 1, file);
			break;
		case tagHFrame :
		case tagSFrame :
		case tagNil :
		case tagFalse :
		case tagTrue :
		case tagInf :
		case tagPtr :
			return errWrongType;
		case tagObj : 	
			// writes the indexable part of any non obj_slot format object 
			obj = b->uo;
			if (obj->size) {
				if (obj->obj_format == obj_slot 
                                    || obj->obj_format == obj_symbol) return errWrongType;
				ptr = obj->slots;
				elemsize = gFormatElemSize[obj->obj_format];
				fwrite(ptr, elemsize, obj->size, file);
			}
			break;
		default : // double
			fwrite(&b->uf, sizeof(double), 1, file);
			break;
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
	
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;
	
	fgets(b->uos->s, MAXINDEXSIZE(b->uo) - 1, file);
	b->uos->size = strlen(b->uos->s);
	return errNone;
}

int prFilePutInt32(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;
	
	a = g->sp - 1;
	b = g->sp;
	
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;
        
        int val;
        int err = slotIntVal(b, &val);
        if (err) return err;
	
	fwrite(&val, sizeof(int32), 1, file);
	return errNone;
}

int prFilePutInt16(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;
	short z;
	
	a = g->sp - 1;
	b = g->sp;
	
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;

        int val;
        int err = slotIntVal(b, &val);
        if (err) return err;
	
	z = val;
	fwrite(&z, sizeof(short), 1, file);
	return errNone;
}

int prFilePutInt8(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;
	int8 z;
	
	a = g->sp - 1;
	b = g->sp;
	
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;

        int val;
        int err = slotIntVal(b, &val);
        if (err) return err;
	
	z = val;
	fwrite(&z, sizeof(int8), 1, file);
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
	
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;
	if (b->utag != tagChar) return errWrongType;
	
	z = b->ui;
	fwrite(&z, sizeof(char), 1, file);
	return errNone;
}

int prFilePutFloat(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;
	
	a = g->sp - 1;
	b = g->sp;
	
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) {
            dumpObjectSlot(a);
            return errFailed;
        }

        float val;
        int err = slotFloatVal(b, &val);
        if (err) return err;
	
	fwrite(&val, sizeof(float), 1, file);
	return errNone;
}

int prFilePutDouble(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrFile *pfile;
	FILE *file;
	
	a = g->sp - 1;
	b = g->sp;
	
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;


        double val;
        int err = slotDoubleVal(b, &val);
        if (err) return err;
	
	fwrite(&val, sizeof(double), 1, file);
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
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;
	if (b->utag != tagObj || b->uo->classptr != class_string) return errWrongType;
	string = b->uos;
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
	
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;
	
	fread(&a->uf, sizeof(double), 1, file);
	return errNone;
}

int prFileGetFloat(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;
	float z;
	
	a = g->sp;
	
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;
	
	fread(&z, sizeof(float), 1, file);
	SetFloat(a, z);
	return errNone;
}

int prFileGetChar(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;
	char z;
	
	a = g->sp;
	
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;
	
	fread(&z, sizeof(char), 1, file);
	SetChar(a, z);
	return errNone;
}

int prFileGetInt8(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;
	char z;
	
	a = g->sp;
	
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;
	
	fread(&z, sizeof(char), 1, file);
	SetInt(a, z);
	return errNone;
}

int prFileGetInt16(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;
	short z;
	
	a = g->sp;
	
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;
	
	fread(&z, sizeof(short), 1, file);
	SetInt(a, z);
	return errNone;
}

int prFileGetInt32(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrFile *pfile;
	FILE *file;
	
	a = g->sp;
	
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;
	
	fread(&a->ui, sizeof(int), 1, file);
	a->utag = tagInt;
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
        
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errFailed;
		
	b->uo->size = fread(b->uos->s, gFormatElemSize[b->uo->obj_format], b->uo->size, file);
	a->ucopy = b->ucopy;
	return errNone;
}

////////


int prPipeOpen(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	char filename[PATH_MAX];
	char mode[12];
	PyrFile *pfile;
	FILE *file;
	
	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;

	if (c->utag != tagObj || !isKindOf(c->uo, class_string)
		|| b->utag != tagObj || !isKindOf(b->uo, class_string))
		return errWrongType; 
	if (c->uo->size > 11) return errFailed;
	pfile = (PyrFile*)a->uo;
	
        char *commandLine = (char*)malloc(b->uo->size + 1);
	memcpy(commandLine, b->uos->s, b->uo->size);
	commandLine[b->uos->size] = 0;
	
	memcpy(mode, c->uos->s, c->uo->size);
	mode[c->uos->size] = 0;
	
	file = popen(filename, mode);
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
	pfile = (PyrFile*)a->uo;
	file = (FILE*)pfile->fileptr.ui;
	if (file == NULL) return errNone;
	SetPtr(&pfile->fileptr, NULL);
	if (pclose(file)) return errFailed;
	return errNone;
}


////////

#include <sndfile.h>

int prSFOpenRead(struct VMGlobals *g, int numArgsPushed);
int prSFOpenRead(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	char filename[PATH_MAX];
	SNDFILE *file;
	SF_INFO info;
        
	a = g->sp - 1;
	b = g->sp;

	if (!isKindOfSlot(b, class_string)) return errWrongType; 
	if (b->uo->size > PATH_MAX - 1) return errFailed;
	
	memcpy(filename, b->uos->s, b->uo->size);
	filename[b->uos->size] = 0;
	
	file = sf_open(filename, SFM_READ, &info);
        
        
	if (file) {
		SetPtr(a->uo->slots + 0, file);
                SetInt(a->uo->slots + 3, info.frames);
                SetInt(a->uo->slots + 4, info.channels);
                SetInt(a->uo->slots + 5, info.samplerate);
		SetTrue(a);
	} else {
		SetNil(a);
		SetFalse(a);
	}
	return errNone;
}

int prSFOpenWrite(struct VMGlobals *g, int numArgsPushed);
int prSFOpenWrite(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	char filename[PATH_MAX];
	SNDFILE *file;
	SF_INFO info;
        
	a = g->sp - 1;
	b = g->sp;

	if (!isKindOfSlot(b, class_string)) return errWrongType; 
	if (b->uo->size > PATH_MAX - 1) return errFailed;
	
	memcpy(filename, b->uos->s, b->uo->size);
	filename[b->uos->size] = 0;
	
	file = sf_open(filename, SFM_READ, &info);
	if (file) {
		SetPtr(a->uo->slots+0, file);
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

        SNDFILE *file = (SNDFILE*)a->uo->slots[0].ui;
        if (file) {
            sf_close(file);
            SetNil(a->uo->slots + 0);
        }
        
	return errNone;
}

int prSFRead(struct VMGlobals *g, int numArgsPushed);
int prSFRead(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
 
        a = g->sp - 1;
	b = g->sp;
       
        SNDFILE *file = (SNDFILE*)a->uo->slots[0].ui;
        
        if (!isKindOfSlot(b, class_rawarray)) return errWrongType;
        
        switch (b->uo->obj_format) {
            case obj_int16 :
                b->uo->size = sf_read_short(file, (short*)b->uob->b, b->uo->size); 
                break;
            case obj_int32 :
                b->uo->size = sf_read_int(file, (int*)b->uob->b, b->uo->size); 
                break;
            case obj_float :
                b->uo->size = sf_read_float(file, (float*)b->uob->b, b->uo->size); 
                break;
            case obj_double :
                b->uo->size = sf_read_double(file, (double*)b->uob->b, b->uo->size); 
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
       
        SNDFILE *file = (SNDFILE*)a->uo->slots[0].ui;
        
        if (!isKindOfSlot(b, class_rawarray)) return errWrongType;
        
        switch (b->uo->obj_format) {
            case obj_int16 :
                sf_write_short(file, (short*)b->uob->b, b->uo->size); 
                break;
            case obj_int32 :
                sf_write_int(file, (int*)b->uob->b, b->uo->size); 
                break;
            case obj_float :
                sf_write_float(file, (float*)b->uob->b, b->uo->size); 
                break;
            case obj_double :
                sf_write_double(file, (double*)b->uob->b, b->uo->size); 
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
       
        SNDFILE *file = (SNDFILE*)a->uo->slots[0].ui;

        int origin, offset;
        int err = slotIntVal(b, &offset);
        if (err) return err;
        
        err = slotIntVal(c, &origin);
        if (err) return err;
        
        sf_seek(file, offset, origin); 

	return errNone;
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
	
	PyrSlot *dirPathSlot = a->uo->slots + 0;
	int err, index;
	err = slotIntVal(c, &index);
	if (err) {
		SetNil(a);
		return err;
	}
	
	char name[256], fullPathName[256];
	int nameLength, creationDate, modificationDate, isDirectory, isVisible, sizeIfFile;
	int dirPathLength = dirPathSlot->uo->size;
	
	err = dir_Lookup(dirPathSlot->uos->s, dirPathLength, index+1,
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
	
	PyrSlot *entryName = b->uo->slots + 0;
	PyrSlot *entryPath = b->uo->slots + 1;
	PyrSlot *entryIsDir = b->uo->slots + 2;
	PyrSlot *entryIsVisible = b->uo->slots + 3;
	
	PyrString *nameString = newPyrString(g->gc, name, 0, true);
	SetObject(entryName, nameString);
	g->gc->GCWrite(b->uo, (PyrObject*)nameString);
	
	memcpy(fullPathName, dirPathSlot->uos->s, dirPathLength);
	fullPathName[dirPathLength] = DELIMITOR;
	strcpy(fullPathName + dirPathLength + 1, name);
	
	PyrString *pathString = newPyrString(g->gc, fullPathName, 0, true);
	SetObject(entryPath, pathString);
	g->gc->GCWrite(b->uo, (PyrObject*)pathString);
	
	if (isDirectory) { SetTrue(entryIsDir); } else { SetFalse(entryIsDir); }
	if (isVisible) { SetTrue(entryIsVisible); } else { SetFalse(entryIsVisible); }
	
	a->ucopy = b->ucopy;
	
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
		pstringFromPyrString((PyrString*)b->uo, options.message, 256);
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
		pstringFromPyrString((PyrString*)b->uo, options.message, 256);
	}
	
	if (isKindOfSlot(c, class_string)) {
		pstringFromPyrString((PyrString*)c->uo, options.savedFileName, 256);
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
	definePrimitive(base, index++, "_SFOpenWrite", prSFOpenWrite, 4, 0);	
	definePrimitive(base, index++, "_SFClose", prSFClose, 1, 0);	
	definePrimitive(base, index++, "_SFWrite", prSFWrite, 2, 0);	
	definePrimitive(base, index++, "_SFRead", prSFRead, 2, 0);	
	definePrimitive(base, index++, "_SFSeek", prSFSeek, 3, 0);	
        
	definePrimitive(base, index++, "_PipeOpen", prPipeOpen, 3, 0);	
	definePrimitive(base, index++, "_PipeClose", prPipeClose, 1, 0);	
	
	definePrimitive(base, index++, "_FileDelete", prFileDelete, 2, 0);	
	definePrimitive(base, index++, "_FileOpen", prFileOpen, 3, 0);	
	definePrimitive(base, index++, "_FileClose", prFileClose, 1, 0);	
	definePrimitive(base, index++, "_FileSeek", prFileSeek, 3, 0);	
	definePrimitive(base, index++, "_FilePos", prFilePos, 1, 0);	
	definePrimitive(base, index++, "_FileLength", prFileLength, 1, 0);	
	definePrimitive(base, index++, "_FileWrite", prFileWrite, 2, 0);	
	definePrimitive(base, index++, "_FileReadLine", prFileReadLine, 2, 0);	

	definePrimitive(base, index++, "_FilePutChar", prFilePutChar, 2, 0);	
	definePrimitive(base, index++, "_FilePutInt8", prFilePutInt8, 2, 0);	
	definePrimitive(base, index++, "_FilePutInt16", prFilePutInt16, 2, 0);	
	definePrimitive(base, index++, "_FilePutInt32", prFilePutInt32, 2, 0);	
	definePrimitive(base, index++, "_FilePutFloat", prFilePutFloat, 2, 0);	
	definePrimitive(base, index++, "_FilePutDouble", prFilePutDouble, 2, 0);	

	definePrimitive(base, index++, "_FileGetChar", prFileGetChar, 1, 0);	
	definePrimitive(base, index++, "_FileGetInt8", prFileGetInt8, 1, 0);	
	definePrimitive(base, index++, "_FileGetInt16", prFileGetInt16, 1, 0);	
	definePrimitive(base, index++, "_FileGetInt32", prFileGetInt32, 1, 0);	
	definePrimitive(base, index++, "_FileGetFloat", prFileGetFloat, 1, 0);	
	definePrimitive(base, index++, "_FileGetDouble", prFileGetDouble, 1, 0);	

	definePrimitive(base, index++, "_FilePutString", prFilePutString, 2, 0);	
	
	definePrimitive(base, index++, "_FileReadRaw", prFileReadRaw, 2, 0);	

#ifdef NOCLASSIC
	definePrimitive(base, index++, "_Directory_At", prDirectory_At, 3, 0);	
	definePrimitive(base, index++, "_File_GetFile", prFile_GetFile, 2, 0);
	definePrimitive(base, index++, "_File_PutFile", prFile_PutFile, 3, 0);
#endif
}






