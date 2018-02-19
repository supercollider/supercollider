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

An object archiving system for SuperCollider.

*/

#include "PyrArchiverT.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "VMGlobals.h"
#include "GC.h"
#include "ReadWriteMacros.h"

#include "SC_PrimRegistry.hpp"

LIBSCLANG_PRIMITIVE_GROUP( Archiver );

SCLANG_DEFINE_PRIMITIVE( AsArchive, 1 )
{
	PyrSlot *a = g->sp;

	PyrArchiver<char*> arch(g);

	int err = arch.prepareToWriteArchive(a);
	if (err) return err;

	int32 size = arch.calcArchiveSize();

	PyrInt8Array *obj = newPyrInt8Array(g->gc, size, 0, true);
	obj->size = size;
	arch.setStream((char*)obj->b);
	err = arch.writeArchive();

	if (err == errNone) SetObject(a, obj);
	else SetNil(a);

	return err;
}

SCLANG_DEFINE_PRIMITIVE( Unarchive, 1 )
{
	PyrSlot *a = g->sp;

	if (!isKindOfSlot(a, class_int8array)) return errWrongType;

	PyrArchiver<char*> arch(g);

	arch.setStream((char*)slotRawObject(a)->slots);
	int err = arch.readArchive(a);
	return err;
}

SCLANG_DEFINE_PRIMITIVE( WriteArchive, 2 )
{
	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

	if (!isKindOfSlot(b, class_string)) return errWrongType;

	char pathname[PATH_MAX];
	memcpy(pathname, slotRawString(b)->s, slotRawObject(b)->size);
	pathname[slotRawString(b)->size] = 0;

	PyrArchiver<FILE*> arch(g);
	FILE *file = fopen(pathname, "wb");
	int err = errNone;
	if (file) {
		err = arch.prepareToWriteArchive(a);
		if (!err) {
			arch.setStream(file);
			err = arch.writeArchive();
		}
		fclose(file);
	} else {
		error("file open failed\n");
		err = errFailed;
	}
	return err;
}

SCLANG_DEFINE_PRIMITIVE( ReadArchive, 2 )
{
	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

	if (!isKindOfSlot(b, class_string)) return errWrongType;

	char pathname[PATH_MAX];
	memcpy(pathname, slotRawString(b)->s, slotRawObject(b)->size);
	pathname[slotRawString(b)->size] = 0;

	PyrArchiver<FILE*> arch(g);
	FILE *file = fopen(pathname, "rb");

	int err;
	if (file) {
		arch.setStream(file);
		err = arch.readArchive(a);
		fclose(file);
	} else {
		error("file open failed\n");
		err = errFailed;
	}
	return err;
}
