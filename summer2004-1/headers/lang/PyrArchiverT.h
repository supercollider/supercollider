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

An object archiving system for SuperCollider.

*/


#ifndef _PyrArchiver_
#define _PyrArchiver_

#include "PyrObject.h"
#include "SC_AllocPool.h"

#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "VMGlobals.h"
#include "GC.h"
#include "ReadWriteMacros.h"

const int32 kArchHdrSize = 12;
const int32 kObjectArrayInitialCapacity = 32;

template <class S>
class PyrArchiver
{
public:
	PyrArchiver(VMGlobals *inG, bool inSameAddressSpace = false)
		: g(inG), mObjectArray(mInitialObjectArray), mNumObjects(0), 
			mObjectArrayCapacity( kObjectArrayInitialCapacity ),
			mSameAddressSpace(inSameAddressSpace), mReadArchiveVersion(0)
		{
		}
	
	~PyrArchiver()
		{
			if (mObjectArray != mInitialObjectArray) {
				g->allocPool->Free(mObjectArray);
			}
		}
		
	void setStream(S s) { mStream.SetStream(s); }
	
	int32 calcArchiveSize()
		{
			PyrSlot *slot;
			int32 size = kArchHdrSize;
			if (mNumObjects == 0) {
				size += sizeOfElem(&mTopSlot) + 1;
			} else {
				// object table size
				for (int i=0; i<mNumObjects; ++i) {
					PyrObject* obj = mObjectArray[i];
					size += obj->classptr->name.us->length + 1; // class name symbol
					size += sizeof(int32); // size
					if (obj->obj_format <= obj_slot) {
						size += obj->size; // tags
						slot = obj->slots;
						for (int j=0; j<obj->size; ++j, ++slot) {
							size += sizeOfElem(slot);
						}
					} else if (obj->obj_format == obj_symbol) {
						PyrSymbol **symbol = ((PyrSymbolArray*)obj)->symbols;
						for (int j=0; j<obj->size; ++j, ++symbol) {
							size += (**symbol).length + 1;
						}
					} else {
						size += obj->size * gFormatElemSize[obj->obj_format];
					}
				}
			}
			return size;
		}
	
	long prepareToWriteArchive(PyrSlot *objectSlot)
		{
			long err = errNone;
				
			try {
				mTopSlot.ucopy = objectSlot->ucopy;
				if (IsObj(objectSlot)) constructObjectArray(objectSlot->uo);				
			} catch (std::exception &ex) {
				error(ex.what());
				err = errFailed;
			}
			return err;
		}
		
	long writeArchive()
		{
			long err = errNone;
				
			try {				
				writeArchiveHeader();
				
				if (mNumObjects == 0) {
					writeSlot(&mTopSlot);
				} else {
					for (int i=0; i<mNumObjects; ++i) {
						writeObjectHeader(mObjectArray[i]);
					}
					for (int i=0; i<mNumObjects; ++i) {
						writeSlots(mObjectArray[i]);
					}
				}
			} catch (std::exception &ex) {
				error(ex.what());
				err = errFailed;
			}
			return err;
		}

	long readArchive(PyrSlot *objectSlot)
		{
			//postfl("->readArchive\n");
			long err = errNone;
			
			
			SetNil(objectSlot);
			
			try {
				readArchiveHeader();
				//postfl("readObjectHeaders %d\n", mNumObjects);
				if (mNumObjects == 0) {
					readSlot(objectSlot);
				} else {
					for (int i=0; i<mNumObjects; ++i) {
						mObjectArray[i] = readObjectHeader();
					}
					//postfl("readSlots\n");
					for (int i=0; i<mNumObjects; ++i) {
						readSlots(mObjectArray[i]);
					}
					//postfl("done reading\n");
					//postfl("SetObject\n");
					SetObject(objectSlot, mObjectArray[0]);
				}
			} catch (std::exception &ex) {
				error(ex.what());
				err = errFailed;
			} catch (...) {
				err = errFailed;
			}
			//postfl("<-readArchive\n");
			return err;
		}
	
private:
		
	void writeArchiveHeader()
		{
			mStream.writeInt32_be('!SCa');
			mStream.writeInt32_be(2); // file version
			mStream.writeInt32_be(mNumObjects);
		}
	
	void readArchiveHeader()
		{
			int32 magicNumber = mStream.readInt32_be();
			if (magicNumber != '!SCa') {
				throw std::runtime_error("not an SC archive.\n");
			}
			mReadArchiveVersion = mStream.readInt32_be(); // file version
			mNumObjects = mStream.readInt32_be();
			//post("readArchiveHeader %d %d\n", mReadArchiveVersion, mNumObjects);
			
			if (mNumObjects > kObjectArrayInitialCapacity) {
				mObjectArray = (PyrObject**)g->allocPool->Alloc(mNumObjects * sizeof(PyrObject*));
				mObjectArrayCapacity = mNumObjects;
			}
			
		}

	void recurse(PyrObject *obj, int n)
		{	
			PyrSlot *slot = obj->slots;
			for (int i=0; i<n; ++i, ++slot) {
				if (IsObj(slot)) constructObjectArray(slot->uo);
			}
		}

	void growObjectArray()
		{
			int32 newObjectArrayCapacity = mObjectArrayCapacity << 1;
			
			int32 newSize = newObjectArrayCapacity * sizeof(PyrObject*);
			PyrObject** newArray = (PyrObject**)g->allocPool->Alloc(newSize);
			memcpy(newArray, mObjectArray, mNumObjects * sizeof(PyrObject*));
			if (mObjectArray != mInitialObjectArray) {
				g->allocPool->Free(mObjectArray);
			}
			mObjectArrayCapacity = newObjectArrayCapacity;
			mObjectArray = newArray;
		}
		
	void putObject(PyrObject *obj)
		{
			obj->SetMark();
			obj->scratch1 = mNumObjects;
			
			// expand array if needed
			if (mNumObjects >= mObjectArrayCapacity) growObjectArray();
			
			// add to array
			mObjectArray[mNumObjects++] = obj;
		}

	void constructObjectArray(PyrObject *obj)
			{
				if (!obj->IsMarked()) {
					if (isKindOf(obj, class_class)) {
					} else if (isKindOf(obj, class_process)) {
					} else if (isKindOf(obj, s_interpreter->u.classobj)) {
					} else if (isKindOf(obj, class_method)) {
						throw std::runtime_error("cannot archive Methods.\n");
					} else if (isKindOf(obj, class_thread)) {
						throw std::runtime_error("cannot archive Threads.\n");
					} else if (isKindOf(obj, class_frame)) {
						throw std::runtime_error("cannot archive Frames.\n");
					} else if (isKindOf(obj, class_func)) {
						if (NotNil(&((PyrClosure*)obj)->block.uoblk->context)) {
							throw std::runtime_error("open Function can not be archived.\n");
						}
						putObject(obj);
						recurse(obj, obj->size);
					} else {
						if (mSameAddressSpace && obj->IsPermanent()) {
							// skip it
						} else {
							if (isKindOf(obj, class_rawarray)) {
								putObject(obj);
							} else if (isKindOf(obj, class_array)) {
								putObject(obj);
								recurse(obj, obj->size);
								
							} else {
								putObject(obj);
								recurse(obj, obj->size);
							}
						}
					}
				}
			}

	int32 sizeOfElem(PyrSlot *slot)
		{
			//postfl("writeSlot %08X\n", slot->utag);
			switch (slot->utag) {
				case tagObj : 
					if (isKindOf(slot->uo, class_class)) {
						return slot->uoc->name.us->length + 1;
					} else if (isKindOf(slot->uo, class_process)) {
						return 0;
					} else if (isKindOf(slot->uo, s_interpreter->u.classobj)) {
						return 0;
					} else {
						return sizeof(int32);
					}
					break;
				case tagHFrame : 
				case tagSFrame : 
					return 0;
				case tagInt : 
					return sizeof(int32);
				case tagSym : 
					return slot->us->length + 1;
				case tagChar : 
					return sizeof(int32);
				case tagNil : 
					return 0;
				case tagFalse : 
					return 0;
				case tagTrue : 
					return 0;
				case tagInf : 
					return 0;
				case tagPtr : 
					throw std::runtime_error("cannot archive RawPointers.");
					return 0;
				default : 
					return sizeof(double);
			}
		}

	PyrSymbol* readSymbolID()
		{
			char str[256];
			mStream.readSymbol(str);
			return getsym(str);
		}


	PyrObject* readObjectID()
		{
			int32 objID = mStream.readInt32_be();
			//postfl("readObjectID %d\n", objID);
			return mObjectArray[objID];
		}

	void writeObjectHeader(PyrObject *obj)
		{
			obj->ClearMark();
			
			//postfl("writeObjectHeader %s\n", obj->classptr->name.us->name);
			mStream.writeSymbol(obj->classptr->name.us->name);
			
			mStream.writeInt32_be(obj->size);
		}

	PyrObject* readObjectHeader()
		{
			PyrSymbol* classname = readSymbolID();
			//post("readObjectHeader %s\n", classname->name);
			PyrObject *obj;
			int32 size = mStream.readInt32_be();
			if (classname->u.classobj->classFlags.ui & classHasIndexableInstances) {
				obj = instantiateObject(g->gc, classname->u.classobj, size, false, false);
				obj->size = size;
			} else {
				obj = instantiateObject(g->gc, classname->u.classobj, 0, false, false);
			}
			return obj;
		}

	void writeSlots(PyrObject *obj)
		{
			//postfl("  writeSlots %s\n", obj->classptr->name.us->name);
			if (isKindOf(obj, class_rawarray)) {
				writeRawArray(obj);
			} else if (isKindOf(obj, class_func)) {
				PyrClosure* closure = (PyrClosure*)obj;
				writeSlot(&closure->block);
			} else {
				for (int i=0; i<obj->size; ++i) {
					writeSlot(obj->slots + i);
				}
			}
		}

	void readSlots(PyrObject *obj)
		{
			//postfl("readSlots\n");
			if (isKindOf(obj, class_rawarray)) {
				readRawArray(obj);
			} else if (isKindOf(obj, class_func)) {
				PyrClosure* closure = (PyrClosure*)obj;
				readSlot(&closure->block);
				closure->context.ucopy = g->process->interpreter.uoi->context.ucopy;
			} else {
				for (int i=0; i<obj->size; ++i) {
					readSlot(obj->slots + i);
				}
			}
		}

	void writeSlot(PyrSlot *slot)
		{
			PyrObject *obj;
			//postfl("    writeSlot %08X\n", slot->utag);
			switch (slot->utag) {
				case tagObj : 
					obj = slot->uo;
					if (isKindOf(obj, class_class)) {
						mStream.writeInt8('C');
						mStream.writeSymbol(slot->uoc->name.us->name);
					} else if (isKindOf(obj, class_process)) {
						mStream.writeInt8('P');
					} else if (isKindOf(obj, s_interpreter->u.classobj)) {
						mStream.writeInt8('R');
					} else {
						if (mSameAddressSpace && obj->IsPermanent()) {
							mStream.writeInt8('z');
							mStream.writeInt32_be((int32)obj);
						} else {
							mStream.writeInt8('o');
							mStream.writeInt32_be(obj->scratch1);
						}
					}
					break;
				case tagHFrame : 
				case tagSFrame : 
					mStream.writeInt8('N');
					break;
				case tagInt : 
					mStream.writeInt8('i');
					mStream.writeInt32_be(slot->ui);
					break;
				case tagSym : 
					mStream.writeInt8('s');
					mStream.writeSymbol(slot->us->name);
					break;
				case tagChar : 
					mStream.writeInt8('c');
					mStream.writeInt32_be(slot->ui);
					break;
				case tagNil : 
					mStream.writeInt8('N');
					break;
				case tagFalse : 
					mStream.writeInt8('F');
					break;
				case tagTrue : 
					mStream.writeInt8('T');
					break;
				case tagInf : 
					mStream.writeInt8('I');
					break;
				case tagPtr : 
					mStream.writeInt8('N');
					break;
				default : 
					mStream.writeInt8('f');
					mStream.writeDouble_be(slot->uf);
					break;
			}
		}

	void readSlot(PyrSlot *slot)
		{
			char tag = mStream.readInt8();
			switch (tag) {
				case 'o' :
					slot->utag = tagObj;
					slot->uo = readObjectID();
					break;
				case 'z' :
					slot->utag = tagObj;
					slot->ui = mStream.readInt32_be();
					break;
				case 'C' :
					slot->utag = tagObj;
					slot->uo = (PyrObject*)readSymbolID()->u.classobj;
					break;
				case 'P' :
					slot->utag = tagObj;
					slot->uo = (PyrObject*)g->process;
					break;
				case 'R' :
					slot->utag = tagObj;
					slot->uo = g->process->interpreter.uo;
					break;
				case 'i' :
					slot->utag = tagInt;
					slot->ui = mStream.readInt32_be();
					break;
				case 's' :
					slot->utag = tagSym;
					slot->us = readSymbolID();
					break;
				case 'c' :
					slot->utag = tagChar;
					slot->ui = mStream.readInt32_be();
					break;
				case 'f' :
					slot->uf = mStream.readDouble_be();
					break;
				case 'N' :
					slot->utag = tagNil;
					slot->ui = 0;
					break;
				case 'T' :
					slot->utag = tagTrue;
					slot->ui = 0;
					break;
				case 'F' :
					slot->utag = tagFalse;
					slot->ui = 0;
					break;
				case 'I' :
					slot->utag = tagInf;
					slot->ui = 0;
					break;
				default :
					slot->utag = tagNil;
					slot->ui = 0;
					break;
			}
		}

	void writeRawArray(PyrObject *obj)
		{
			int32 size = obj->size;
			//postfl("writeRawArray %d\n", size);
			switch (obj->obj_format) {
				case obj_char : 
				case obj_int8 : {
					char *data = (char*)obj->slots;
					mStream.writeData(data, size);
				} break;
				case obj_int16 : {
					int16 *data = (int16*)obj->slots;
					for (int i=0; i<size; ++i) {
						mStream.writeInt16_be(data[i]);
					}
				} break;
				case obj_int32 : 
				case obj_float : {
					int32 *data = (int32*)obj->slots;
					for (int i=0; i<size; ++i) {
						mStream.writeInt32_be(data[i]);
					}
				} break;
				case obj_double : {
					double *data = (double*)obj->slots;
					for (int i=0; i<size; ++i) {
						mStream.writeDouble_be(data[i]);
					}
				} break;
				case obj_symbol : {
					PyrSymbol **data = (PyrSymbol**)obj->slots;
					for (int i=0; i<size; ++i) {
						mStream.writeSymbol(data[i]->name);
					}
				} break;
			}		
		}

	void readRawArray(PyrObject *obj)
		{
			//postfl("readRawArray\n");
			int32 size = obj->size;
			switch (obj->obj_format) {
				case obj_char : 
				case obj_int8 : {
					int8 *data = (int8*)obj->slots;
					for (int i=0; i<size; ++i) {
						data[i] = mStream.readInt8();
					}
				} break;
				case obj_int16 : {
					int16 *data = (int16*)obj->slots;
					for (int i=0; i<size; ++i) {
						data[i] = mStream.readInt16_be();
					}
				} break;
				case obj_int32 : 
				case obj_float : {
					int32 *data = (int32*)obj->slots;
					for (int i=0; i<size; ++i) {
						data[i] = mStream.readInt32_be();
					}
				} break;
				case obj_double : {
					double *data = (double*)obj->slots;
					for (int i=0; i<size; ++i) {
						data[i] = mStream.readDouble_be();
					}
				} break;
				case obj_symbol : {
					PyrSymbol **data = (PyrSymbol**)obj->slots;
					for (int i=0; i<size; ++i) {
						data[i] = readSymbolID();
					}
				} break;
			}
					
		}
		
	VMGlobals *g;
		
	PyrObject **mObjectArray;
	int32 mNumObjects;
	int32 mObjectArrayCapacity;
	PyrSlot mTopSlot;
		
	bool mSameAddressSpace;
	SC_IOStream<S> mStream;
	int32 mReadArchiveVersion;

	PyrObject *mInitialObjectArray[kObjectArrayInitialCapacity];
};

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

An object archiving system for SuperCollider.

*/



#endif

