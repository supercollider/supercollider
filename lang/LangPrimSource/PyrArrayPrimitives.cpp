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

Primitives for Arrays.

*/

#include "GC.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "SC_InlineBinaryOp.h"
#include "SC_Constants.h"
#include <string.h>

int basicSize(VMGlobals* g, int numArgsPushed);
int basicMaxSize(VMGlobals* g, int numArgsPushed);

int basicSwap(struct VMGlobals* g, int numArgsPushed);
int basicAt(VMGlobals* g, int numArgsPushed);
int basicRemoveAt(VMGlobals* g, int numArgsPushed);
int basicClipAt(VMGlobals* g, int numArgsPushed);
int basicWrapAt(VMGlobals* g, int numArgsPushed);
int basicFoldAt(VMGlobals* g, int numArgsPushed);
int basicPut(VMGlobals* g, int numArgsPushed);
int basicClipPut(VMGlobals* g, int numArgsPushed);
int basicWrapPut(VMGlobals* g, int numArgsPushed);
int basicFoldPut(VMGlobals* g, int numArgsPushed);

int prArrayAdd(VMGlobals* g, int numArgsPushed);
int prArrayFill(VMGlobals* g, int numArgsPushed);
int prArrayPop(VMGlobals* g, int numArgsPushed);
int prArrayGrow(VMGlobals* g, int numArgsPushed);
int prArrayCat(VMGlobals* g, int numArgsPushed);

int prArrayReverse(VMGlobals* g, int numArgsPushed);
int prArrayScramble(VMGlobals* g, int numArgsPushed);
int prArrayRotate(VMGlobals* g, int numArgsPushed);
int prArrayStutter(VMGlobals* g, int numArgsPushed);
int prArrayMirror(VMGlobals* g, int numArgsPushed);
int prArrayMirror1(VMGlobals* g, int numArgsPushed);
int prArrayMirror2(VMGlobals* g, int numArgsPushed);
int prArrayExtendWrap(VMGlobals* g, int numArgsPushed);
int prArrayExtendFold(VMGlobals* g, int numArgsPushed);
int prArrayPermute(VMGlobals* g, int numArgsPushed);
int prArrayPyramid(VMGlobals* g, int numArgsPushed);
int prArraySlide(VMGlobals* g, int numArgsPushed);
int prArrayLace(VMGlobals* g, int numArgsPushed);
int prArrayContainsSeqColl(VMGlobals* g, int numArgsPushed);
int prArrayWIndex(VMGlobals* g, int numArgsPushed);
int prArrayNormalizeSum(VMGlobals* g, int numArgsPushed);
int prArrayIndexOfGreaterThan(VMGlobals* g, int numArgsPushed);


int basicSize(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrObject* obj;

    a = g->sp;
    if (NotObj(a)) {
        SetInt(a, 0);
        return errNone;
    }
    obj = slotRawObject(a);
    SetInt(a, obj->size);
    return errNone;
}

int basicMaxSize(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrObject* obj;
    int maxsize;

    a = g->sp;
    if (NotObj(a)) {
        SetInt(a, 0);
        return errNone;
    }
    obj = slotRawObject(a);
    maxsize = MAXINDEXSIZE(obj);
    SetInt(a, maxsize);
    return errNone;
}

int basicSwap(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c, tempi, tempj;
    int i, j;
    PyrObject* obj;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    if (NotObj(a))
        return errWrongType;
    if (NotInt(b))
        return errIndexNotAnInteger;
    if (NotInt(c))
        return errIndexNotAnInteger;
    obj = slotRawObject(a);
    if (obj->IsImmutable())
        return errImmutableObject;
    if (!(slotRawInt(&obj->classptr->classFlags) & classHasIndexableInstances))
        return errNotAnIndexableObject;
    i = slotRawInt(b);
    j = slotRawInt(c);
    if (i < 0 || i >= obj->size)
        return errIndexOutOfRange;
    if (j < 0 || j >= obj->size)
        return errIndexOutOfRange;
    getIndexedSlot(obj, &tempi, i);
    getIndexedSlot(obj, &tempj, j);
    putIndexedSlot(g, obj, &tempi, j);
    putIndexedSlot(g, obj, &tempj, i);
    // in case it is partial scan obj
    g->gc->GCWrite(obj, &tempi);
    g->gc->GCWrite(obj, &tempj);

    return errNone;
}

int getIndexedInt(PyrObject* obj, int index, int* value);
void DumpBackTrace(VMGlobals* g);

int basicAt(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    int index;
    PyrObject* obj;

    a = g->sp - 1;
    b = g->sp;

    if (NotObj(a))
        return errWrongType;
    obj = slotRawObject(a);
    if (!(slotRawInt(&obj->classptr->classFlags) & classHasIndexableInstances))
        return errNotAnIndexableObject;

    int err = slotIntVal(b, &index);
    if (!err) {
        if (index < 0 || index >= obj->size) {
            slotCopy(a, &o_nil);
        } else {
            getIndexedSlot(obj, a, index);
        }
    } else if (isKindOfSlot(b, class_arrayed_collection)) {
        PyrObject* indexArray = slotRawObject(b);
        int size = indexArray->size;
        PyrObject* outArray = newPyrArray(g->gc, size, 0, true);
        PyrSlot* outArraySlots = outArray->slots;
        for (int i = 0; i < size; ++i) {
            int err = getIndexedInt(indexArray, i, &index);
            if (err)
                return err;
            if (index < 0 || index >= obj->size) {
                slotCopy(&outArraySlots[i], &o_nil);
            } else {
                getIndexedSlot(obj, outArraySlots + i, index);
            }
        }
        outArray->size = size;
        SetObject(a, outArray);
    } else {
        return errIndexNotAnInteger;
    }
    return errNone;
}

int basicRemoveAt(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    int index, length, elemsize;
    PyrObject* obj;
    void* ptr;

    a = g->sp - 1;
    b = g->sp;

    if (NotObj(a))
        return errWrongType;
    int err = slotIntVal(b, &index);
    if (err)
        return errWrongType;

    obj = slotRawObject(a);
    if (obj->IsImmutable())
        return errImmutableObject;
    if (!(slotRawInt(&obj->classptr->classFlags) & classHasIndexableInstances))
        return errNotAnIndexableObject;

    if (index < 0 || index >= obj->size)
        return errIndexOutOfRange;
    switch (obj->obj_format) {
    default:
    case obj_slot:
        ptr = obj->slots + index;
        slotCopy(a, (PyrSlot*)ptr);
        break;
    case obj_double:
        ptr = obj->slots + index;
        SetFloat(a, *(double*)ptr);
        break;
    case obj_float:
        ptr = ((float*)(obj->slots)) + index;
        SetFloat(a, *(float*)ptr);
        break;
    case obj_int32:
        ptr = ((int32*)(obj->slots)) + index;
        SetInt(a, *(int32*)ptr);
        break;
    case obj_int16:
        ptr = ((int16*)(obj->slots)) + index;
        SetInt(a, *(int16*)ptr);
        break;
    case obj_int8:
        ptr = ((int8*)(obj->slots)) + index;
        SetInt(a, *(int8*)ptr);
        break;
    case obj_symbol:
        ptr = ((int*)(obj->slots)) + index;
        SetSymbol(a, *(PyrSymbol**)ptr);
        break;
    case obj_char:
        ptr = ((unsigned char*)(obj->slots)) + index;
        SetChar(a, *(unsigned char*)ptr);
        break;
    }
    length = obj->size - index - 1;
    if (length > 0) {
        elemsize = gFormatElemSize[obj->obj_format];
        memmove(ptr, (char*)ptr + elemsize, length * elemsize);
        if (obj->obj_format <= obj_slot) {
            // might be partial scan object
            g->gc->GCWrite(obj, obj->slots + index);
        }
    }
    obj->size--;
    return errNone;
}


int basicTakeAt(struct VMGlobals* g, int numArgsPushed);
int basicTakeAt(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    int index, lastIndex;
    PyrObject* obj;

    a = g->sp - 1;
    b = g->sp;

    if (NotObj(a))
        return errWrongType;
    int err = slotIntVal(b, &index);
    if (err)
        return errWrongType;

    obj = slotRawObject(a);
    if (obj->IsImmutable())
        return errImmutableObject;
    if (!(slotRawInt(&obj->classptr->classFlags) & classHasIndexableInstances))
        return errNotAnIndexableObject;

    lastIndex = obj->size - 1;
    if (index < 0 || index >= obj->size)
        return errIndexOutOfRange;
    switch (obj->obj_format) {
    case obj_slot: {
        PyrSlot* ptr = obj->slots + index;
        PyrSlot* lastptr = obj->slots + lastIndex;
        slotCopy(a, ptr);
        *ptr = *lastptr;
        // might be partial scan obj
        g->gc->GCWrite(obj, ptr);
    } break;
    case obj_double: {
        PyrSlot* ptr = obj->slots + index;
        PyrSlot* lastptr = obj->slots + lastIndex;
        SetFloat(a, *(double*)ptr);
        *ptr = *lastptr;
        // might be partial scan obj
        g->gc->GCWrite(obj, ptr);
    } break;
    case obj_float: {
        float* ptr = ((float*)(obj->slots)) + index;
        float* lastptr = ((float*)(obj->slots)) + lastIndex;
        SetFloat(a, *(float*)ptr);
        *ptr = *lastptr;
    } break;
    case obj_int32: {
        int32* ptr = ((int32*)(obj->slots)) + index;
        int32* lastptr = ((int32*)(obj->slots)) + lastIndex;
        SetInt(a, *(int32*)ptr);
        *ptr = *lastptr;
    } break;
    case obj_int16: {
        int16* ptr = ((int16*)(obj->slots)) + index;
        int16* lastptr = ((int16*)(obj->slots)) + lastIndex;
        SetInt(a, *(int16*)ptr);
        *ptr = *lastptr;
    } break;
    case obj_int8: {
        int8* ptr = ((int8*)(obj->slots)) + index;
        int8* lastptr = ((int8*)(obj->slots)) + lastIndex;
        SetInt(a, *(int8*)ptr);
        *ptr = *lastptr;
    } break;
    case obj_symbol: {
        int32* ptr = ((int32*)(obj->slots)) + index;
        int32* lastptr = ((int32*)(obj->slots)) + lastIndex;
        SetSymbol(a, *(PyrSymbol**)ptr);
        *ptr = *lastptr;
    } break;
    case obj_char: {
        unsigned char* ptr = ((unsigned char*)(obj->slots)) + index;
        unsigned char* lastptr = ((unsigned char*)(obj->slots)) + lastIndex;
        SetChar(a, *(unsigned char*)ptr);
        *ptr = *lastptr;
    } break;
    }
    obj->size--;
    return errNone;
}

int basicWrapAt(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    int index;
    PyrObject* obj;
    a = g->sp - 1;
    b = g->sp;

    if (NotObj(a))
        return errWrongType;
    obj = slotRawObject(a);
    if (!(slotRawInt(&obj->classptr->classFlags) & classHasIndexableInstances))
        return errNotAnIndexableObject;

    if (obj->size == 0) {
        SetNil(a);
        return errNone;
    }

    int err = slotIntVal(b, &index);

    if (!err) {
        index = sc_mod((int)index, (int)obj->size);
        getIndexedSlot(obj, a, index);
    } else if (isKindOfSlot(b, class_arrayed_collection)) {
        PyrObject* indexArray = slotRawObject(b);
        int size = indexArray->size;
        PyrObject* outArray = newPyrArray(g->gc, size, 0, true);
        PyrSlot* outArraySlots = outArray->slots;
        for (int i = 0; i < size; ++i) {
            int err = getIndexedInt(indexArray, i, &index);
            if (err)
                return err;
            index = sc_mod((int)index, (int)obj->size);
            getIndexedSlot(obj, outArraySlots + i, index);
        }
        outArray->size = size;
        SetObject(a, outArray);
    } else
        return errIndexNotAnInteger;

    return errNone;
}

int basicFoldAt(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    int index;
    PyrObject* obj;
    a = g->sp - 1;
    b = g->sp;

    if (NotObj(a))
        return errWrongType;
    obj = slotRawObject(a);
    if (!(slotRawInt(&obj->classptr->classFlags) & classHasIndexableInstances))
        return errNotAnIndexableObject;

    if (obj->size == 0) {
        SetNil(a);
        return errNone;
    }

    int err = slotIntVal(b, &index);

    if (!err) {
        index = sc_fold(index, 0, obj->size - 1);
        getIndexedSlot(obj, a, index);
    } else if (isKindOfSlot(b, class_arrayed_collection)) {
        PyrObject* indexArray = slotRawObject(b);
        int size = indexArray->size;
        PyrObject* outArray = newPyrArray(g->gc, size, 0, true);
        PyrSlot* outArraySlots = outArray->slots;
        for (int i = 0; i < size; ++i) {
            int err = getIndexedInt(indexArray, i, &index);
            if (err)
                return err;
            index = sc_fold(index, 0, obj->size - 1);
            getIndexedSlot(obj, outArraySlots + i, index);
        }
        outArray->size = size;
        SetObject(a, outArray);
    } else
        return errIndexNotAnInteger;

    return errNone;
}

int basicClipAt(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    int index;
    PyrObject* obj;
    a = g->sp - 1;
    b = g->sp;

    if (NotObj(a))
        return errWrongType;
    obj = slotRawObject(a);
    if (!(slotRawInt(&obj->classptr->classFlags) & classHasIndexableInstances))
        return errNotAnIndexableObject;

    if (obj->size == 0) {
        SetNil(a);
        return errNone;
    }

    int err = slotIntVal(b, &index);

    if (!err) {
        index = sc_clip(index, 0, obj->size - 1);
        getIndexedSlot(obj, a, index);
    } else if (isKindOfSlot(b, class_arrayed_collection)) {
        PyrObject* indexArray = slotRawObject(b);
        int size = indexArray->size;
        PyrObject* outArray = newPyrArray(g->gc, size, 0, true);
        PyrSlot* outArraySlots = outArray->slots;
        for (int i = 0; i < size; ++i) {
            int err = getIndexedInt(indexArray, i, &index);
            if (err)
                return err;
            index = sc_clip(index, 0, obj->size - 1);
            getIndexedSlot(obj, outArraySlots + i, index);
        }
        outArray->size = size;
        SetObject(a, outArray);
    } else
        return errIndexNotAnInteger;

    return errNone;
}


int basicPut(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    int index;
    PyrObject* obj;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    obj = slotRawObject(a);
    if (obj->IsImmutable())
        return errImmutableObject;
    if (!(slotRawInt(&obj->classptr->classFlags) & classHasIndexableInstances))
        return errNotAnIndexableObject;

    if (NotObj(a))
        return errWrongType;
    int err = slotIntVal(b, &index);

    if (!err) {
        if (index < 0 || index >= obj->size)
            return errIndexOutOfRange;
        return putIndexedSlot(g, obj, c, index);
    } else if (isKindOfSlot(b, class_arrayed_collection)) {
        PyrObject* indexArray = slotRawObject(b);
        int size = slotRawObject(b)->size;

        for (int i = 0; i < size; ++i) {
            int index;
            int err = getIndexedInt(indexArray, i, &index);
            if (err)
                return err;
            if (index < 0 || index >= obj->size)
                return errIndexOutOfRange;
            err = putIndexedSlot(g, obj, c, index);
            if (err)
                return err;
        }
        return errNone;
    } else
        return errIndexNotAnInteger;
}

int basicClipPut(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    int index;
    PyrObject* obj;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    obj = slotRawObject(a);
    if (obj->IsImmutable())
        return errImmutableObject;
    if (!(slotRawInt(&obj->classptr->classFlags) & classHasIndexableInstances))
        return errNotAnIndexableObject;

    if (NotObj(a))
        return errWrongType;
    int err = slotIntVal(b, &index);

    if (!err) {
        index = sc_clip(index, 0, obj->size - 1);
        return putIndexedSlot(g, obj, c, index);
    } else if (isKindOfSlot(b, class_arrayed_collection)) {
        PyrObject* indexArray = slotRawObject(b);
        int size = slotRawObject(b)->size;

        for (int i = 0; i < size; ++i) {
            int index;
            int err = getIndexedInt(indexArray, i, &index);
            if (err)
                return err;
            index = sc_clip(index, 0, obj->size - 1);
            err = putIndexedSlot(g, obj, c, index);
            if (err)
                return err;
        }
        return errNone;
    } else
        return errIndexNotAnInteger;
}

int basicWrapPut(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    int index;
    PyrObject* obj;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    obj = slotRawObject(a);
    if (obj->IsImmutable())
        return errImmutableObject;
    if (!(slotRawInt(&obj->classptr->classFlags) & classHasIndexableInstances))
        return errNotAnIndexableObject;

    if (NotObj(a))
        return errWrongType;
    int err = slotIntVal(b, &index);

    if (!err) {
        index = sc_mod((int)index, (int)obj->size);
        return putIndexedSlot(g, obj, c, index);
    } else if (isKindOfSlot(b, class_arrayed_collection)) {
        PyrObject* indexArray = slotRawObject(b);
        int size = slotRawObject(b)->size;

        for (int i = 0; i < size; ++i) {
            int index;
            int err = getIndexedInt(indexArray, i, &index);
            if (err)
                return err;
            index = sc_mod((int)index, (int)obj->size);
            err = putIndexedSlot(g, obj, c, index);
            if (err)
                return err;
        }
        return errNone;
    } else
        return errIndexNotAnInteger;
}

int basicFoldPut(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    int index;
    PyrObject* obj;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    obj = slotRawObject(a);
    if (obj->IsImmutable())
        return errImmutableObject;
    if (!(slotRawInt(&obj->classptr->classFlags) & classHasIndexableInstances))
        return errNotAnIndexableObject;

    if (NotObj(a))
        return errWrongType;
    int err = slotIntVal(b, &index);

    if (!err) {
        index = sc_fold(index, 0, obj->size - 1);
        return putIndexedSlot(g, obj, c, index);
    } else if (isKindOfSlot(b, class_arrayed_collection)) {
        PyrObject* indexArray = slotRawObject(b);
        int size = slotRawObject(b)->size;

        for (int i = 0; i < size; ++i) {
            int index;
            int err = getIndexedInt(indexArray, i, &index);
            if (err)
                return err;
            index = sc_fold(index, 0, obj->size - 1);
            err = putIndexedSlot(g, obj, c, index);
            if (err)
                return err;
        }
        return errNone;
    } else
        return errIndexNotAnInteger;
}

int prArrayPutEach(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    PyrObject* obj;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    obj = slotRawObject(a);
    if (obj->IsImmutable())
        return errImmutableObject;
    if (!(slotRawInt(&obj->classptr->classFlags) & classHasIndexableInstances))
        return errNotAnIndexableObject;

    if (!isKindOfSlot(b, class_arrayed_collection))
        return errWrongType;
    if (!isKindOfSlot(c, class_arrayed_collection))
        return errWrongType;

    PyrSlot* indices = slotRawObject(b)->slots;
    PyrSlot* values = slotRawObject(c)->slots;
    int size = slotRawObject(b)->size;
    int valsize = slotRawObject(c)->size;

    for (int i = 0; i < size; ++i) {
        int index;
        int err = slotIntVal(indices + i, &index);
        if (err)
            return err;
        if (index < 0 || index >= obj->size)
            return errIndexOutOfRange;
        int valindex = sc_mod(i, valsize);
        err = putIndexedSlot(g, obj, values + valindex, index);
        if (err)
            return err;
    }

    return errNone;
}


int prArrayAssocAt(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrObject* obj;
    bool found = false;

    a = g->sp - 1;
    b = g->sp;

    obj = slotRawObject(a);

    int size = obj->size;
    if (obj->obj_format == obj_slot) {
        PyrSlot* slots = obj->slots;
        for (int i = 0; i < size; i += 2) {
            if (SlotEq(slots + i, b)) {
                if (i + 1 >= size)
                    return errFailed;
                slotCopy(a, &slots[i + 1]);
                found = true;
                break;
            }
        }
    } else {
        PyrSlot slot;
        for (int i = 0; i < size; i += 2) {
            getIndexedSlot(obj, &slot, i);
            if (SlotEq(&slot, b)) {
                if (i + 1 >= size)
                    return errFailed;
                getIndexedSlot(obj, &slot, i + 1);
                slotCopy(a, &slot);
                found = true;
                break;
            }
        }
    }
    if (!found)
        SetNil(a);

    return errNone;
}


int prArrayAssocPut(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    PyrObject* obj;
    bool found = false;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    obj = slotRawObject(a);
    if (obj->IsImmutable())
        return errImmutableObject;

    int size = obj->size;
    if (obj->obj_format == obj_slot) {
        PyrSlot* slots = obj->slots;
        for (int i = 0; i < size; i += 2) {
            if (SlotEq(slots + i, b)) {
                if (i + 1 >= size)
                    return errFailed;
                slotCopy(&slots[i + 1], c);
                g->gc->GCWrite(obj, c);
                found = true;
                break;
            }
        }
    } else {
        PyrSlot slot;
        for (int i = 0; i < size; i += 2) {
            getIndexedSlot(obj, &slot, i);
            if (SlotEq(&slot, b)) {
                if (i + 1 >= size)
                    return errFailed;
                putIndexedSlot(g, obj, &slot, i + 1);
                g->gc->GCWrite(obj, c);
                found = true;
                break;
            }
        }
    }
    if (!found)
        SetNil(a);

    return errNone;
}

int prArrayIndexOf(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrObject* obj;
    bool found = false;

    a = g->sp - 1;
    b = g->sp;

    obj = slotRawObject(a);

    int size = obj->size;
    if (obj->obj_format == obj_slot) {
        PyrSlot* slots = obj->slots;
        for (int i = 0; i < size; ++i) {
            if (SlotEq(slots + i, b)) {
                SetInt(a, i);
                found = true;
                break;
            }
        }
    } else {
        PyrSlot slot;
        for (int i = 0; i < size; ++i) {
            getIndexedSlot(obj, &slot, i);
            if (SlotEq(&slot, b)) {
                SetInt(a, i);
                found = true;
                break;
            }
        }
    }
    if (!found)
        SetNil(a);

    return errNone;
}


int prArrayPutSeries(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c, *d, *e;

    a = g->sp - 4;
    b = g->sp - 3;
    c = g->sp - 2;
    d = g->sp - 1;
    e = g->sp;

    PyrObject* inobj = slotRawObject(a);
    if (inobj->IsImmutable())
        return errImmutableObject;

    int size = inobj->size;

    if (NotInt(b) && NotNil(b))
        return errWrongType;
    if (NotInt(c) && NotNil(c))
        return errWrongType;
    if (NotInt(d) && NotNil(d))
        return errWrongType;

    int first = IsInt(b) ? slotRawInt(b) : 0;
    int last = IsInt(d) ? slotRawInt(d) : size - 1;
    int second = IsInt(c) ? slotRawInt(c) : (first < last ? first + 1 : first - 1);

    int step = second - first;

    first = sc_clip(first, 0, size - 1);
    last = sc_clip(last, 0, size - 1);

    int err = errNone;

    if (step == 0)
        return errFailed;
    if (step == 1) {
        for (int i = first; i <= last; ++i) {
            err = putIndexedSlot(g, inobj, e, i);
            if (err)
                return err;
        }
    } else if (step == -1) {
        for (int i = last; i >= first; --i) {
            err = putIndexedSlot(g, inobj, e, i);
            if (err)
                return err;
        }
    } else if (step > 0) {
        int length = (last - first) / step + 1;

        for (int i = first, j = 0; j < length; i += step, ++j) {
            err = putIndexedSlot(g, inobj, e, i);
            if (err)
                return err;
        }
    } else if (step < 0) {
        int length = (first - last) / -step + 1;

        for (int i = first, j = 0; j < length; i += step, ++j) {
            err = putIndexedSlot(g, inobj, e, i);
            if (err)
                return err;
        }
    }
    return errNone;
}


int prArrayAdd(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *slots;
    int maxelems, elemsize, format, tag, numbytes;
    int err, ival;
    double fval;

    a = g->sp - 1;
    b = g->sp;

    PyrObject* array = slotRawObject(a);
    if (array->IsImmutable())
        return errImmutableObject;

    format = slotRawObject(a)->obj_format;
    tag = gFormatElemTag[format];
    /*if (tag > 0) {
        if (GetTag(b) != tag) return errWrongType;
    } else if (tag == 0) {
        if (NotFloat(b)) return errWrongType;
    } // else format is obj_slot, any tag is acceptable*/
    elemsize = gFormatElemSize[format];
    maxelems = MAXINDEXSIZE(array);
    if (array->size >= maxelems || array->IsImmutable()) {
        numbytes = sizeof(PyrSlot) << (array->obj_sizeclass + 1);
        array = g->gc->New(numbytes, 0, format, true);
        array->classptr = slotRawObject(a)->classptr;
        array->size = slotRawObject(a)->size;
        memcpy(array->slots, slotRawObject(a)->slots, slotRawObject(a)->size * elemsize);
        SetRaw(a, array);
    }
    slots = array->slots;
    switch (format) {
    case obj_slot:
        slotCopy(&slots[array->size++], b);
        g->gc->GCWrite(array, b);
        break;
    case obj_int32:
        err = slotIntVal(b, &ival);
        if (err)
            return err;
        ((int32*)slots)[array->size++] = ival;
        break;
    case obj_int16:
        err = slotIntVal(b, &ival);
        if (err)
            return err;
        ((int16*)slots)[array->size++] = ival;
        break;
    case obj_int8:
        err = slotIntVal(b, &ival);
        if (err)
            return err;
        ((int8*)slots)[array->size++] = ival;
        break;
    case obj_char:
        if (NotChar(b))
            return errWrongType;
        ((char*)slots)[array->size++] = slotRawChar(b);
        break;
    case obj_symbol:
        if (NotSym(b))
            return errWrongType;
        ((PyrSymbol**)slots)[array->size++] = slotRawSymbol(b);
        break;
    case obj_float:
        err = slotDoubleVal(b, &fval);
        if (err)
            return err;
        ((float*)slots)[array->size++] = fval;
        break;
    case obj_double:
        err = slotDoubleVal(b, &fval);
        if (err)
            return err;
        ((double*)slots)[array->size++] = fval;
        break;
    }
    return errNone;
}

int prArrayInsert(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c, *slots1, *slots2;
    PyrObject *array, *oldarray;
    int err, ival;
    double fval;

    a = g->sp - 2; // array
    b = g->sp - 1; // index
    c = g->sp; // value
    if (NotInt(b))
        return errWrongType;

    array = slotRawObject(a);
    const int format = slotRawObject(a)->obj_format;

    const int size = array->size;
    int index = slotRawInt(b);
    index = sc_clip(index, 0, size);
    const int remain = size - index;

    const int elemsize = gFormatElemSize[format];
    const int maxelems = MAXINDEXSIZE(array);
    if (size + 1 > maxelems || array->IsImmutable()) {
        oldarray = array;

        const int numbytes = sizeof(PyrSlot) << (array->obj_sizeclass + 1);
        array = g->gc->New(numbytes, 0, format, true);

        array->classptr = oldarray->classptr;

        array->size = size + 1;
        SetRaw(a, array);
        slots1 = array->slots;
        slots2 = oldarray->slots;
        if (index)
            memcpy(slots1, slots2, index * elemsize);

        switch (format) {
        case obj_slot:

            slotCopy(&slots1[index], c);
            if (remain)
                memcpy(slots1 + index + 1, slots2 + index, remain * elemsize);
            if (!g->gc->ObjIsGrey(array))
                g->gc->ToGrey(array);
            break;
        case obj_int32:
            err = slotIntVal(c, &ival);
            if (err)
                return err;
            ((int32*)slots1)[index] = ival;
            if (remain) {
                memcpy((int*)slots1 + index + 1, (int*)slots2 + index, remain * elemsize);
            }
            break;
        case obj_int16:
            err = slotIntVal(c, &ival);
            if (err)
                return err;
            ((int16*)slots1)[index] = ival;
            if (remain) {
                memcpy((short*)slots1 + index + 1, (short*)slots2 + index, remain * elemsize);
            }
            break;
        case obj_int8:
            err = slotIntVal(c, &ival);
            if (err)
                return err;
            ((int8*)slots1)[index] = ival;
            if (remain) {
                memcpy((char*)slots1 + index + 1, (char*)slots2 + index, remain * elemsize);
            }
            break;
        case obj_char:
            if (NotChar(c))
                return errWrongType;
            ((char*)slots1)[index] = slotRawInt(c);
            if (remain) {
                memcpy((char*)slots1 + index + 1, (char*)slots2 + index, remain * elemsize);
            }
            break;
        case obj_symbol:
            if (NotSym(c))
                return errWrongType;
            ((PyrSymbol**)slots1)[index] = slotRawSymbol(c);
            if (remain) {
                memcpy((int*)slots1 + index + 1, (int*)slots2 + index, remain * elemsize);
            }
            break;
        case obj_float:
            err = slotDoubleVal(c, &fval);
            if (err)
                return err;
            ((float*)slots1)[index] = fval;
            if (remain) {
                memcpy((float*)slots1 + index + 1, (float*)slots2 + index, remain * elemsize);
            }
            break;
        case obj_double:
            err = slotDoubleVal(c, &fval);
            if (err)
                return err;
            ((double*)slots1)[index] = fval;
            if (remain) {
                memcpy((double*)slots1 + index + 1, (double*)slots2 + index, remain * elemsize);
            }
            break;
        }
    } else {
        array->size = size + 1;
        slots1 = array->slots;
        switch (format) {
        case obj_slot:
            if (remain)
                memmove(slots1 + index + 1, slots1 + index, remain * elemsize);
            slotCopy(&slots1[index], c);
            if (!g->gc->ObjIsGrey(array))
                g->gc->ToGrey(array);
            break;
        case obj_int32:
            if (remain) {
                memmove((int*)slots1 + index + 1, (int*)slots1 + index, remain * elemsize);
            }
            err = slotIntVal(c, &ival);
            if (err)
                return err;
            ((int32*)slots1)[index] = ival;
            break;
        case obj_int16:
            if (remain) {
                memmove((short*)slots1 + index + 1, (short*)slots1 + index, remain * elemsize);
            }
            err = slotIntVal(c, &ival);
            if (err)
                return err;
            ((int16*)slots1)[index] = ival;
            break;
        case obj_int8:
            if (remain) {
                memmove((char*)slots1 + index + 1, (char*)slots1 + index, remain * elemsize);
            }
            err = slotIntVal(c, &ival);
            if (err)
                return err;
            ((int8*)slots1)[index] = ival;
            break;
        case obj_char:
            if (remain) {
                memmove((char*)slots1 + index + 1, (char*)slots1 + index, remain * elemsize);
            }
            if (NotChar(c))
                return errWrongType;
            ((char*)slots1)[index] = slotRawInt(c);
            break;
        case obj_symbol:
            if (remain) {
                memmove((int*)slots1 + index + 1, (int*)slots1 + index, remain * elemsize);
            }
            if (NotSym(c))
                return errWrongType;
            ((PyrSymbol**)slots1)[index] = slotRawSymbol(c);
            break;
        case obj_float:
            if (remain) {
                memmove((float*)slots1 + index + 1, (float*)slots1 + index, remain * elemsize);
            }
            err = slotDoubleVal(c, &fval);
            if (err)
                return err;
            ((float*)slots1)[index] = fval;
            break;
        case obj_double:
            if (remain) {
                memmove((double*)slots1 + index + 1, (double*)slots1 + index, remain * elemsize);
            }
            err = slotDoubleVal(c, &fval);
            if (err)
                return err;
            ((double*)slots1)[index] = fval;
            break;
        }
    }
    return errNone;
}


int prArrayFill(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *slots;
    PyrObject* array;
    PyrSymbol* sym;
    int i;
    int format;
    int err, ival;
    double fval;


    a = g->sp - 1;
    b = g->sp;

    array = slotRawObject(a);
    format = slotRawObject(a)->obj_format;
    /*int tag = gFormatElemTag[format];
    if (tag > 0) {
        if (GetTag(b) != tag) return errWrongType;
    } else if (tag == 0) {
        if (NotFloat(b)) return errWrongType;
    } // else format is obj_slot, any tag is acceptable*/
    slots = array->slots;
    switch (format) {
    case obj_slot:
        if (array->IsImmutable())
            return errImmutableObject;
        for (i = 0; i < array->size; ++i) {
            slotCopy(&slots[i], b);
        }
        g->gc->GCWrite(array, b);
        break;
    case obj_int32:
        err = slotIntVal(b, &ival);
        if (err)
            return err;
        for (i = 0; i < array->size; ++i) {
            ((int32*)slots)[i] = ival;
        }
        break;
    case obj_int16:
        err = slotIntVal(b, &ival);
        if (err)
            return err;
        for (i = 0; i < array->size; ++i) {
            ((int16*)slots)[i] = ival;
        }
        break;
    case obj_int8:
        err = slotIntVal(b, &ival);
        if (err)
            return err;
        for (i = 0; i < array->size; ++i) {
            ((int8*)slots)[i] = ival;
        }
        break;
    case obj_char:
        if (NotChar(b))
            return errWrongType;
        ival = slotRawInt(b);
        for (i = 0; i < array->size; ++i) {
            ((char*)slots)[i] = ival;
        }
        break;
    case obj_symbol:
        if (NotSym(b))
            return errWrongType;
        sym = slotRawSymbol(b);
        for (i = 0; i < array->size; ++i) {
            ((PyrSymbol**)slots)[i] = sym;
        }
        break;
    case obj_float:
        err = slotDoubleVal(b, &fval);
        if (err)
            return err;
        for (i = 0; i < array->size; ++i) {
            ((float*)slots)[i] = fval;
        }
        break;
    case obj_double:
        err = slotDoubleVal(b, &fval);
        if (err)
            return err;
        for (i = 0; i < array->size; ++i) {
            ((double*)slots)[i] = fval;
        }
        break;
    }
    return errNone;
}

int prArrayPop(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *slots;
    PyrObject* array;
    int z;
    int format;
    PyrSymbol* sym;

    a = g->sp;

    array = slotRawObject(a);
    if (array->IsImmutable())
        return errImmutableObject;
    if (array->size > 0) {
        format = array->obj_format;
        slots = array->slots;
        switch (format) {
        case obj_slot:
            slotCopy(a, &slots[--array->size]);
            break;
        case obj_int32:
            z = ((int32*)slots)[--array->size];
            SetInt(a, z);
            break;
        case obj_int16:
            z = ((int16*)slots)[--array->size];
            SetInt(a, z);
            break;
        case obj_int8:
            z = ((int8*)slots)[--array->size];
            SetInt(a, z);
            break;
        case obj_char:
            z = ((char*)slots)[--array->size];
            SetChar(a, z);
            break;
        case obj_symbol:
            sym = ((PyrSymbol**)slots)[--array->size];
            SetSymbol(a, sym);
            break;
        case obj_float:
            SetFloat(a, ((float*)slots)[--array->size]);
            break;
        case obj_double:
            SetFloat(a, slotRawFloat(&slots[--array->size]));
            break;
        }
    } else {
        slotCopy(a, &o_nil);
    }
    return errNone;
}


int prArrayExtend(struct VMGlobals* g, int numArgsPushed) {
    int numbytes, elemsize, format;
    int err;

    PyrSlot* a = g->sp - 2; // array
    PyrSlot* b = g->sp - 1; // size
    PyrSlot* c = g->sp; // filler item


    if (NotInt(b))
        return errWrongType;
    PyrObject* aobj = slotRawObject(a);
    if (slotRawInt(b) <= aobj->size) {
        aobj->size = slotRawInt(b);
        return errNone;
    }

    format = aobj->obj_format;
    if (slotRawInt(b) > MAXINDEXSIZE(aobj) || aobj->IsImmutable()) {
        elemsize = gFormatElemSize[format];
        numbytes = slotRawInt(b) * elemsize;

        PyrObject* obj = g->gc->New(numbytes, 0, format, true);
        obj->classptr = aobj->classptr;
        obj->size = aobj->size;
        memcpy(obj->slots, aobj->slots, aobj->size * elemsize);
        aobj = obj;
        SetRaw(a, aobj);
    }


    int fillSize = slotRawInt(b) - aobj->size;
    int32 ival;
    float fval;
    double dval;

    PyrSlot* slots = aobj->slots;
    switch (format) {
    case obj_slot:
        fillSlots(slots + aobj->size, fillSize, c);
        g->gc->GCWrite(aobj, c);
        break;
    case obj_int32: {
        int32* ptr = (int32*)slots + aobj->size;
        err = slotIntVal(c, &ival);
        if (err)
            return err;
        for (int i = 0; i < fillSize; ++i)
            ptr[i] = ival;
    } break;
    case obj_int16: {
        int16* ptr = (int16*)slots + aobj->size;
        err = slotIntVal(c, &ival);
        if (err)
            return err;
        for (int i = 0; i < fillSize; ++i)
            ptr[i] = ival;
    } break;
    case obj_int8: {
        int8* ptr = (int8*)slots + aobj->size;
        err = slotIntVal(c, &ival);
        if (err)
            return err;
        for (int i = 0; i < fillSize; ++i)
            ptr[i] = ival;
    } break;
    case obj_char: {
        char* ptr = (char*)slots + aobj->size;
        if (NotChar(c))
            return errWrongType;
        ival = slotRawChar(c);
        for (int i = 0; i < fillSize; ++i)
            ptr[i] = ival;
    } break;
    case obj_symbol: {
        PyrSymbol** ptr = (PyrSymbol**)slots + aobj->size;
        if (NotSym(c))
            return errWrongType;
        PyrSymbol* sym = slotRawSymbol(c);
        for (int i = 0; i < fillSize; ++i)
            ptr[i] = sym;
    } break;
    case obj_float: {
        float* ptr = (float*)slots + aobj->size;
        err = slotFloatVal(c, &fval);
        for (int i = 0; i < fillSize; ++i)
            ptr[i] = fval;
    } break;
    case obj_double: {
        double* ptr = (double*)slots + aobj->size;
        err = slotDoubleVal(c, &dval);
        for (int i = 0; i < fillSize; ++i)
            ptr[i] = dval;
    } break;
    }

    aobj->size = slotRawInt(b);
    return errNone;
}

int prArrayGrow(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrObject *obj, *aobj;
    int numbytes, elemsize, format;

    a = g->sp - 1;
    b = g->sp;

    if (NotInt(b))
        return errWrongType;
    if (slotRawInt(b) <= 0)
        return errNone;
    aobj = slotRawObject(a);

    if (aobj->size + slotRawInt(b) <= MAXINDEXSIZE(aobj))
        return errNone;

    format = aobj->obj_format;
    elemsize = gFormatElemSize[format];
    numbytes = ((aobj->size + slotRawInt(b)) * elemsize);

    obj = g->gc->New(numbytes, 0, format, true);
    obj->classptr = aobj->classptr;
    obj->size = aobj->size;
    memcpy(obj->slots, aobj->slots, aobj->size * elemsize);
    SetRaw(a, obj);

    return errNone;
}

int prArrayGrowClear(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrObject *obj, *aobj;
    int numbytes, elemsize, format;

    a = g->sp - 1;
    b = g->sp;

    if (NotInt(b))
        return errWrongType;
    if (slotRawInt(b) <= 0)
        return errNone;
    aobj = slotRawObject(a);

    if (aobj->size + slotRawInt(b) <= MAXINDEXSIZE(aobj) && aobj->IsMutable()) {
        obj = aobj;
    } else {
        format = aobj->obj_format;
        elemsize = gFormatElemSize[format];
        numbytes = ((aobj->size + slotRawInt(b)) * elemsize);

        obj = g->gc->New(numbytes, 0, format, true);
        obj->classptr = aobj->classptr;
        memcpy(obj->slots, aobj->slots, aobj->size * elemsize);
    }

    if (obj->obj_format == obj_slot) {
        nilSlots(obj->slots + aobj->size, slotRawInt(b));
    } else {
        memset((char*)(obj->slots) + aobj->size * gFormatElemSize[format], 0, slotRawInt(b) * gFormatElemSize[format]);
    }
    obj->size = aobj->size + slotRawInt(b);
    SetRaw(a, obj);

    return errNone;
}

int prArrayCat(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrObject *obj, *aobj, *bobj;
    int elemsize, size;
    int numbytes, format;

    a = g->sp - 1;
    b = g->sp;

    if (NotObj(b) || slotRawObject(a)->classptr != slotRawObject(b)->classptr)
        return errWrongType;
    aobj = slotRawObject(a);
    bobj = slotRawObject(b);
    size = aobj->size + bobj->size;
    format = aobj->obj_format;
    assert(aobj->obj_format == bobj->obj_format);
    elemsize = gFormatElemSize[format];
    numbytes = (size * elemsize);

    obj = g->gc->New(numbytes, 0, format, true);
    obj->classptr = aobj->classptr;
    obj->size = size;
    memcpy(obj->slots, aobj->slots, aobj->size * elemsize);
    memcpy((char*)obj->slots + aobj->size * elemsize, bobj->slots, bobj->size * elemsize);
    SetObject(a, obj);

    return errNone;
}


int prArrayAddAll(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrObject *obj, *aobj;
    int elemsize, newindexedsize, newsizebytes, asize, bsize;
    int format;

    a = g->sp - 1;
    b = g->sp;

    if (NotObj(b) || slotRawObject(a)->classptr != slotRawObject(b)->classptr)
        return errWrongType;
    aobj = slotRawObject(a);

    format = aobj->obj_format;
    elemsize = gFormatElemSize[format];
    asize = aobj->size;
    bsize = slotRawObject(b)->size;
    newindexedsize = asize + bsize;
    newsizebytes = newindexedsize * elemsize;

    if (newindexedsize > MAXINDEXSIZE(aobj) || aobj->IsImmutable()) {
        obj = g->gc->New(newsizebytes, 0, format, true);
        obj->classptr = aobj->classptr;
        memcpy(obj->slots, aobj->slots, asize * elemsize);
        SetObject(a, obj);
    } else {
        obj = aobj;
        if (format == obj_slot && !g->gc->ObjIsGrey(obj))
            g->gc->ToGrey(obj);
    }
    obj->size = newindexedsize;
    memcpy((char*)obj->slots + asize * elemsize, slotRawObject(b)->slots, bsize * elemsize);
    return errNone;
}


int prArrayOverwrite(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    PyrObject *obj, *aobj;
    int err, elemsize, newindexedsize, newsizebytes, pos, asize, bsize;
    int format;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp; // pos

    if (NotObj(b) || slotRawObject(a)->classptr != slotRawObject(b)->classptr)
        return errWrongType;
    err = slotIntVal(c, &pos);
    if (err)
        return errWrongType;
    if (pos < 0 || pos > slotRawObject(a)->size)
        return errIndexOutOfRange;

    aobj = slotRawObject(a);
    format = aobj->obj_format;
    elemsize = gFormatElemSize[format];
    asize = aobj->size;
    bsize = slotRawObject(b)->size;
    newindexedsize = pos + bsize;
    newindexedsize = sc_max(asize, newindexedsize);
    newsizebytes = newindexedsize * elemsize;

    if (newindexedsize > MAXINDEXSIZE(aobj) || aobj->IsImmutable()) {
        obj = g->gc->New(newsizebytes, 0, format, true);
        obj->classptr = aobj->classptr;
        memcpy(obj->slots, aobj->slots, asize * elemsize);
        SetObject(a, obj);
    } else {
        obj = aobj;
        if (format == obj_slot && !g->gc->ObjIsGrey(obj))
            g->gc->ToGrey(obj);
    }
    obj->size = newindexedsize;
    memcpy((char*)(obj->slots) + pos * elemsize, slotRawObject(b)->slots, bsize * elemsize);

    return errNone;
}

int prArrayReverse(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *slots1, *slots2;
    PyrObject *obj1, *obj2;
    int i, j, size;

    a = g->sp;
    obj1 = slotRawObject(a);
    size = obj1->size;
    obj2 = instantiateObject(g->gc, obj1->classptr, size, false, true);
    slots1 = obj1->slots;
    slots2 = obj2->slots;
    for (i = 0, j = size - 1; i < size; ++i, --j) {
        slotCopy(&slots2[j], &slots1[i]);
    }
    obj2->size = size;
    SetRaw(a, obj2);
    return errNone;
}

int prArrayScramble(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *slots1, *slots2, temp;
    PyrObject *obj1, *obj2;
    int i, j, k, m, size;

    a = g->sp;
    obj1 = slotRawObject(a);
    size = obj1->size;
    obj2 = instantiateObject(g->gc, obj1->classptr, size, false, true);
    slots1 = obj1->slots;
    slots2 = obj2->slots;
    memcpy(slots2, slots1, size * sizeof(PyrSlot));
    if (size > 1) {
        k = size;
        for (i = 0, m = k; i < k - 1; ++i, --m) {
            j = i + g->rgen->irand(m);
            slotCopy(&temp, &slots2[i]);
            slotCopy(&slots2[i], &slots2[j]);
            slotCopy(&slots2[j], &temp);
        }
    }
    obj2->size = size;
    SetRaw(a, obj2);
    return errNone;
}

int prArrayRotate(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *slots;
    PyrObject *obj1, *obj2;
    int i, j, n, size;

    a = g->sp - 1;
    b = g->sp;
    if (NotInt(b))
        return errWrongType;

    obj1 = slotRawObject(a);
    size = obj1->size;
    n = sc_mod((int)slotRawInt(b), (int)size);
    slots = obj1->slots;
    obj2 = instantiateObject(g->gc, obj1->classptr, size, false, true);
    for (i = 0, j = n; i < size; ++i) {
        slotCopy(&obj2->slots[j], &slots[i]);
        if (++j >= size)
            j = 0;
    }
    obj2->size = size;
    SetRaw(a, obj2);
    return errNone;
}

int prArrayStutter(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *slots1, *slots2;
    PyrObject *obj1, *obj2;
    int i, j, k, m, n, size;

    a = g->sp - 1;
    b = g->sp;
    if (NotInt(b))
        return errWrongType;

    obj1 = slotRawObject(a);
    n = slotRawInt(b);
    m = obj1->size;
    size = m * n;
    obj2 = instantiateObject(g->gc, obj1->classptr, size, false, true);
    slots1 = obj1->slots;
    slots2 = obj2->slots;
    for (i = 0, j = 0; i < m; ++i) {
        for (k = 0; k < n; ++k, ++j) {
            slotCopy(&slots2[j], &slots1[i]);
        }
    }
    obj2->size = size;
    SetRaw(a, obj2);
    return errNone;
}

int prArrayMirror(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *slots;
    PyrObject *obj1, *obj2;
    int i, j, k, size;

    a = g->sp;

    obj1 = slotRawObject(a);
    slots = obj1->slots;
    size = obj1->size * 2 - 1;
    obj2 = instantiateObject(g->gc, obj1->classptr, size, false, true);
    obj2->size = size;
    // copy first part of list
    memcpy(obj2->slots, slots, obj1->size * sizeof(PyrSlot));
    // copy second part
    k = size / 2;
    for (i = 0, j = size - 1; i < k; ++i, --j) {
        slotCopy(&obj2->slots[j], &slots[i]);
    }
    SetRaw(a, obj2);
    return errNone;
}

int prArrayMirror1(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *slots;
    PyrObject *obj1, *obj2;
    int i, j, k, size;

    a = g->sp;

    obj1 = slotRawObject(a);
    slots = obj1->slots;
    size = obj1->size * 2 - 2;
    obj2 = instantiateObject(g->gc, obj1->classptr, size, false, true);
    obj2->size = size;
    // copy first part of list
    memcpy(obj2->slots, slots, obj1->size * sizeof(PyrSlot));
    // copy second part
    k = size / 2;
    for (i = 1, j = size - 1; i < k; ++i, --j) {
        slotCopy(&obj2->slots[j], &slots[i]);
    }
    SetRaw(a, obj2);
    return errNone;
}

int prArrayMirror2(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *slots;
    PyrObject *obj1, *obj2;
    int i, j, k, size;

    a = g->sp;

    obj1 = slotRawObject(a);
    slots = obj1->slots;
    size = obj1->size * 2;
    obj2 = instantiateObject(g->gc, obj1->classptr, size, false, true);
    obj2->size = size;
    // copy first part of list
    memcpy(obj2->slots, slots, obj1->size * sizeof(PyrSlot));
    // copy second part
    k = size / 2;
    for (i = 0, j = size - 1; i < k; ++i, --j) {
        slotCopy(&obj2->slots[j], &slots[i]);
    }
    SetRaw(a, obj2);
    return errNone;
}


int prArrayExtendWrap(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *slots;
    PyrObject *obj1, *obj2;
    int i, j, m, size;

    a = g->sp - 1;
    b = g->sp;
    if (NotInt(b))
        return errWrongType;

    size = slotRawInt(b);
    if (size < 0)
        return errFailed;

    obj1 = slotRawObject(a);

    if (obj1->size > 0) {
        obj2 = instantiateObject(g->gc, obj1->classptr, size, false, true);
        obj2->size = size;
        slots = obj2->slots;
        // copy first part of list
        memcpy(slots, obj1->slots, sc_min(size, obj1->size) * sizeof(PyrSlot));
        if (size > obj1->size) {
            // copy second part
            m = obj1->size;
            for (i = 0, j = m; j < size; ++i, ++j) {
                slotCopy(&slots[j], &slots[i]);
            }
        }
    } else {
        obj2 = instantiateObject(g->gc, obj1->classptr, size, true, true);
    }
    SetRaw(a, obj2);
    return errNone;
}

int prArrayExtendFold(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *slots;
    PyrObject *obj1, *obj2;
    int i, j, m, size;

    a = g->sp - 1;
    b = g->sp;
    if (NotInt(b))
        return errWrongType;

    size = slotRawInt(b);
    if (size < 0)
        return errFailed;

    obj1 = slotRawObject(a);
    if (obj1->size > 0) {
        obj2 = instantiateObject(g->gc, obj1->classptr, size, false, true);
        obj2->size = size;
        slots = obj2->slots;
        // copy first part of list
        memcpy(slots, obj1->slots, sc_min(size, obj1->size) * sizeof(PyrSlot));
        if (size > obj1->size) {
            // copy second part
            m = obj1->size;
            for (i = 0, j = m; j < size; ++i, ++j) {
                slotCopy(&slots[j], &slots[sc_fold(j, 0, m - 1)]);
            }
        }
    } else {
        obj2 = instantiateObject(g->gc, obj1->classptr, size, true, true);
    }
    SetRaw(a, obj2);
    return errNone;
}

int prArrayExtendLast(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *slots, last;
    PyrObject *obj1, *obj2;
    int i, j, m, size;

    a = g->sp - 1;
    b = g->sp;
    if (NotInt(b))
        return errWrongType;

    size = slotRawInt(b);
    if (size < 0)
        return errFailed;

    obj1 = slotRawObject(a);
    if (obj1->size > 0) {
        obj2 = instantiateObject(g->gc, obj1->classptr, size, false, true);
        obj2->size = size;
        slots = obj2->slots;
        // copy first part of list
        memcpy(slots, obj1->slots, sc_min(size, obj1->size) * sizeof(PyrSlot));
        if (size > obj1->size) {
            // copy second part
            m = obj1->size;
            slotCopy(&last, &slots[m - 1]);
            for (i = 0, j = m; j < size; ++i, ++j)
                slotCopy(&slots[j], &last);
        }
    } else {
        obj2 = instantiateObject(g->gc, obj1->classptr, size, true, true);
    }
    SetRaw(a, obj2);
    return errNone;
}

int prArrayPermute(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *slots1, *slots2, temp;
    PyrObject *obj1, *obj2;
    int i, j, m, z, size;

    a = g->sp - 1;
    b = g->sp;
    if (NotInt(b))
        return errWrongType;

    obj1 = slotRawObject(a);
    size = obj1->size;
    obj2 = instantiateObject(g->gc, obj1->classptr, size, false, true);
    obj2->size = size;
    slots1 = obj1->slots;
    slots2 = obj2->slots;
    memcpy(slots2, slots1, size * sizeof(PyrSlot));
    z = slotRawInt(b);
    for (i = 0, m = size; i < size - 1; ++i, --m) {
        j = i + sc_mod((int)z, (int)(size - i));
        z = sc_div(z, size - i);
        slotCopy(&temp, &slots2[i]);
        slotCopy(&slots2[i], &slots2[j]);
        slotCopy(&slots2[j], &temp);
    }
    SetRaw(a, obj2);
    return errNone;
}


int prArrayAllTuples(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *slots1, *slots2, *slots3, *slotToCopy;
    PyrObject *obj1, *obj2, *obj3;

    a = g->sp - 1;
    b = g->sp;
    if (NotInt(b))
        return errWrongType;
    int maxSize = slotRawInt(b);

    obj1 = slotRawObject(a);
    slots1 = obj1->slots;
    int newSize = 1;
    int tupSize = obj1->size;
    for (int i = 0; i < tupSize; ++i) {
        if (isKindOfSlot(slots1 + i, class_array)) {
            newSize *= slotRawObject(&slots1[i])->size;
        }
    }
    if (newSize > maxSize)
        newSize = maxSize;

    obj2 = instantiateObject(g->gc, obj1->classptr, newSize, false, true);
    slots2 = obj2->slots;
    SetObject(b, obj2); // store reference on stack, so both source and destination objects can be reached by the gc

    for (int i = 0; i < newSize; ++i) {
        int k = i;
        obj3 = instantiateObject(g->gc, obj1->classptr, tupSize, false, true);
        slots3 = obj3->slots;
        for (int j = tupSize - 1; j >= 0; --j) {
            if (isKindOfSlot(slots1 + j, class_array)) {
                PyrObject* obj4 = slotRawObject(&slots1[j]);
                slotToCopy = &obj4->slots[k % obj4->size];
                slotCopy(&slots3[j], slotToCopy);
                g->gc->GCWrite(obj3, slotToCopy);
                k /= obj4->size;
            } else {
                slotToCopy = &slots1[j];
                slotCopy(&slots3[j], slotToCopy);
                g->gc->GCWrite(obj3, slotToCopy);
            }
        }
        obj3->size = tupSize;
        SetObject(obj2->slots + i, obj3);
        g->gc->GCWriteNew(obj2, obj3);
        obj2->size++;
    }
    SetRaw(a, obj2);
    return errNone;
}

int prArrayPyramid(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *slots;
    PyrObject *obj1, *obj2;
    int i, j, k, n, m, numslots, x;

    a = g->sp - 1;
    b = g->sp;
    if (NotInt(b))
        return errWrongType;

    obj1 = slotRawObject(a);
    slots = obj1->slots;
    m = sc_clip(slotRawInt(b), 1, 10);
    x = numslots = obj1->size;
    switch (m) {
    case 1:
        n = (x * x + x) / 2;
        obj2 = instantiateObject(g->gc, obj1->classptr, n, false, true);
        for (i = 0, k = 0; i < numslots; ++i) {
            for (j = 0; j <= i; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        obj2->size = k;
        break;
    case 2:
        n = (x * x + x) / 2;
        obj2 = instantiateObject(g->gc, obj1->classptr, n, false, true);
        for (i = 0, k = 0; i < numslots; ++i) {
            for (j = numslots - 1 - i; j <= numslots - 1; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        obj2->size = k;
        break;
    case 3:
        n = (x * x + x) / 2;
        obj2 = instantiateObject(g->gc, obj1->classptr, n, false, true);
        for (i = 0, k = 0; i < numslots; ++i) {
            for (j = 0; j <= numslots - 1 - i; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        obj2->size = k;
        break;
    case 4:
        n = (x * x + x) / 2;
        obj2 = instantiateObject(g->gc, obj1->classptr, n, false, true);
        for (i = 0, k = 0; i < numslots; ++i) {
            for (j = i; j <= numslots - 1; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        obj2->size = k;
        break;
    case 5:
        n = x * x;
        obj2 = instantiateObject(g->gc, obj1->classptr, n, false, true);
        for (i = 0, k = 0; i < numslots; ++i) {
            for (j = 0; j <= i; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        for (i = 0; i < numslots - 1; ++i) {
            for (j = 0; j <= numslots - 2 - i; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        obj2->size = k;
        break;
    case 6:
        n = x * x;
        obj2 = instantiateObject(g->gc, obj1->classptr, n, false, true);
        for (i = 0, k = 0; i < numslots; ++i) {
            for (j = numslots - 1 - i; j <= numslots - 1; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        for (i = 0; i < numslots - 1; ++i) {
            for (j = i + 1; j <= numslots - 1; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        obj2->size = k;
        break;
    case 7:
        n = x * x + x - 1;
        obj2 = instantiateObject(g->gc, obj1->classptr, n, false, true);
        for (i = 0, k = 0; i < numslots; ++i) {
            for (j = 0; j <= numslots - 1 - i; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        for (i = 1; i < numslots; ++i) {
            for (j = 0; j <= i; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        obj2->size = k;
        break;
    case 8:
        n = x * x + x - 1;
        obj2 = instantiateObject(g->gc, obj1->classptr, n, false, true);
        for (i = 0, k = 0; i < numslots; ++i) {
            for (j = i; j <= numslots - 1; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        for (i = 1; i < numslots; ++i) {
            for (j = numslots - 1 - i; j <= numslots - 1; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        obj2->size = k;
        break;
    case 9:
        n = x * x;
        obj2 = instantiateObject(g->gc, obj1->classptr, n, false, true);
        for (i = 0, k = 0; i < numslots; ++i) {
            for (j = 0; j <= i; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        for (i = 0; i < numslots - 1; ++i) {
            for (j = i + 1; j <= numslots - 1; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        obj2->size = k;
        break;
    case 10:
        n = x * x;
        obj2 = instantiateObject(g->gc, obj1->classptr, n, false, true);
        for (i = 0, k = 0; i < numslots; ++i) {
            for (j = numslots - 1 - i; j <= numslots - 1; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        for (i = 0; i < numslots - 1; ++i) {
            for (j = 0; j <= numslots - 2 - i; ++j, ++k) {
                slotCopy(&obj2->slots[k], &slots[j]);
            }
        }
        obj2->size = k;
        break;
    }
    SetRaw(a, obj2);
    return errNone;
}

int prArraySlide(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c, *slots;
    PyrObject *obj1, *obj2;
    int h, i, j, k, numslots, numwin;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;
    if (NotInt(b))
        return errWrongType;
    if (NotInt(c))
        return errWrongType;

    obj1 = slotRawObject(a);
    slots = obj1->slots;
    int m = slotRawInt(b);
    int n = slotRawInt(c);
    if (n <= 0)
        return errFailed;

    numwin = (obj1->size + n - m) / n;
    numslots = numwin * m;
    obj2 = instantiateObject(g->gc, obj1->classptr, numslots, false, true);
    for (i = h = k = 0; i < numwin; ++i, h += n) {
        for (j = h; j < m + h; ++j)
            slotCopy(&obj2->slots[k++], &slots[j]);
    }
    obj2->size = k;
    SetRaw(a, obj2);
    return errNone;
}

int prArrayLace(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *slots, *slot;
    PyrObject *obj1, *obj2, *obj3;
    int i, j, k, n, m, numLists, len;

    a = g->sp - 1;
    b = g->sp;
    obj1 = slotRawObject(a);
    slots = obj1->slots;
    numLists = obj1->size;

    if (IsNil(b)) {
        // If length argument is unspecified, compute length as the multiple of
        // the length of the shortest sub-array and the number of sub-arrays.
        for (j = 0; j < numLists; ++j) {
            slot = slots + j;
            if (isKindOfSlot(slot, class_list)) {
                obj2 = slotRawObject(slot);
                slot = &obj2->slots[0];
            }
            if (isKindOfSlot(slot, class_array)) {
                len = slotRawObject(slot)->size;
                if (j == 0 || n > len) {
                    n = len;
                }
            } else {
                // If not an array we assume a max per-element length of one.
                n = 1;
                break;
            }
        }
        n = n * numLists;

    } else if (IsInt(b)) {
        n = slotRawInt(b);
    } else {
        return errWrongType;
    }


    n = sc_max(1, n);
    if (obj1->size > 0) {
        obj2 = instantiateObject(g->gc, obj1->classptr, n, false, true);
        for (i = j = k = 0; i < n; ++i) {
            if (IsObj(&slots[k])) {
                obj3 = slotRawObject(&slots[k]);
                if (isKindOf(obj3, class_list)) {
                    obj3 = slotRawObject(&obj3->slots[0]); // get the list's array
                }
                if (obj3 && isKindOf(obj3, class_array)) {
                    if (obj3->size > 0) {
                        m = j % obj3->size;
                        slotCopy(&obj2->slots[i], &obj3->slots[m]);
                    } else {
                        SetNil(&obj2->slots[i]);
                    }
                } else {
                    slotCopy(&obj2->slots[i], &slots[k]);
                }
            } else {
                slotCopy(&obj2->slots[i], &slots[k]);
            }
            k = (k + 1) % obj1->size;
            if (k == 0)
                j++;
        }
        obj2->size = n;
    } else {
        if (isKindOf(obj1, class_array)) {
            obj2 = newPyrArray(g->gc, 0, 0, true);
        } else {
            obj2 = instantiateObject(g->gc, obj1->classptr, n, true, true);
            obj2->size = n;
        }
    }
    SetRaw(a, obj2);
    return errNone;
}

int prArrayContainsSeqColl(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *slot, *endptr;
    PyrObject* obj;
    int size;

    a = g->sp;
    obj = slotRawObject(a);
    size = obj->size;
    slot = obj->slots - 1;
    endptr = slot + size;
    while (slot < endptr) {
        ++slot;
        if (IsObj(slot)) {
            if (isKindOf(slotRawObject(slot), class_sequenceable_collection)) {
                SetTrue(a);
                return errNone;
            }
        }
    }
    SetFalse(a);
    return errNone;
}

int prArrayNormalizeSum(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *slots2;
    PyrObject *obj1, *obj2;
    int i, size, err;
    double w, sum, rsum;

    a = g->sp;
    obj1 = slotRawObject(a);
    size = obj1->size;
    obj2 = instantiateObject(g->gc, obj1->classptr, size, false, true);
    slots2 = obj2->slots;
    sum = 0.0;
    for (i = 0; i < size; ++i) {
        err = getIndexedDouble(obj1, i, &w);
        if (err)
            return err;
        sum += w;
        SetFloat(&slots2[i], w);
    }
    rsum = 1. / sum;
    for (i = 0; i < size; ++i) {
        double d = slotRawFloat(&slots2[i]);
        SetRaw(&slots2[i], d * rsum);
    }
    obj2->size = size;
    SetRaw(a, obj2);
    return errNone;
}


int prArrayWIndex(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *slots;
    PyrObject* obj;
    int i, j, size, err;
    double r, w, sum;

    a = g->sp;

    sum = 0.0;
    r = g->rgen->frand();
    obj = slotRawObject(a);
    size = obj->size;
    j = size - 1;
    slots = obj->slots;
    for (i = 0; i < size; ++i) {
        err = getIndexedDouble(obj, i, &w);
        if (err)
            return err;
        sum += w;
        if (sum >= r) {
            j = i;
            break;
        }
    }
    SetInt(a, j);
    return errNone;
}


enum {
    shape_Step,
    shape_Linear,
    shape_Exponential,
    shape_Sine,
    shape_Welch,
    shape_Curve,
    shape_Squared,
    shape_Cubed,
    shape_Hold
};

enum { kEnv_initLevel, kEnv_numStages, kEnv_releaseNode, kEnv_loopNode };

int prArrayEnvAt(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    PyrObject* env = slotRawObject(a);
    PyrSlot* slots = env->slots;

    // Env:asArray always gives at least 8 array elements
    if (env->size < 8)
        return errFailed;

    double time;
    int err = slotDoubleVal(b, &time);
    if (err)
        return err;

    double begLevel;
    err = slotDoubleVal(slots + kEnv_initLevel, &begLevel);
    if (err)
        return err;

    int numStages;
    err = slotIntVal(slots + kEnv_numStages, &numStages);
    if (err)
        return err;

    double begTime = 0.;
    double endTime = 0.;

    for (int i = 0; i < numStages; ++i) {
        double dur, endLevel;

        slots += 4;

        err = slotDoubleVal(slots + 0, &endLevel);
        if (err)
            return err;
        err = slotDoubleVal(slots + 1, &dur);
        if (err)
            return err;

        endTime += dur;

        // post("%d   %g   %g %g   %g   %g\n", i, time, begTime, endTime, dur, endLevel);

        if (time < endTime) {
            int shape;
            double curve;

            err = slotIntVal(slots + 2, &shape);
            if (err)
                return err;

            double level;
            double pos = (time - begTime) / dur;

            // post("    shape %d   pos %g\n", shape, pos);
            switch (shape) {
            case shape_Step:
                level = endLevel;
                break;
            case shape_Hold:
                level = begLevel;
                break;
            case shape_Linear:
            default:
                level = pos * (endLevel - begLevel) + begLevel;
                break;
            case shape_Exponential:
                level = begLevel * pow(endLevel / begLevel, pos);
                break;
            case shape_Sine:
                level = begLevel + (endLevel - begLevel) * (-cos(pi * pos) * 0.5 + 0.5);
                break;
            case shape_Welch: {
                if (begLevel < endLevel)
                    level = begLevel + (endLevel - begLevel) * sin(pi2 * pos);
                else
                    level = endLevel - (endLevel - begLevel) * sin(pi2 - pi2 * pos);
                break;
            }
            case shape_Curve:
                err = slotDoubleVal(slots + 3, &curve);
                if (err)
                    return err;

                if (fabs(curve) < 0.0001) {
                    level = pos * (endLevel - begLevel) + begLevel;
                } else {
                    double denom = 1. - exp(curve);
                    double numer = 1. - exp(pos * curve);
                    level = begLevel + (endLevel - begLevel) * (numer / denom);
                }
                break;
            case shape_Squared: {
                double sqrtBegLevel = sqrt(begLevel);
                double sqrtEndLevel = sqrt(endLevel);
                double sqrtLevel = pos * (sqrtEndLevel - sqrtBegLevel) + sqrtBegLevel;
                level = sqrtLevel * sqrtLevel;
                break;
            }
            case shape_Cubed: {
                double cbrtBegLevel = pow(begLevel, 0.3333333);
                double cbrtEndLevel = pow(endLevel, 0.3333333);
                double cbrtLevel = pos * (cbrtEndLevel - cbrtBegLevel) + cbrtBegLevel;
                level = cbrtLevel * cbrtLevel * cbrtLevel;
                break;
            }
            }
            SetFloat(a, level);
            return errNone;
        }

        begTime = endTime;
        begLevel = endLevel;
    }

    SetFloat(a, begLevel);

    return errNone;
}


int prArrayIndexOfGreaterThan(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *slots;
    PyrObject* obj;
    int i, size, err;
    double s, w;

    a = g->sp - 1;
    b = g->sp;

    obj = slotRawObject(a);

    size = obj->size;
    slots = obj->slots;

    err = slotDoubleVal(b, &s);
    if (err)
        return err;

    for (i = 0; i < size; ++i) {
        err = getIndexedDouble(obj, i, &w);
        if (err)
            return err;

        if (w > s) {
            SetInt(a, i);
            return errNone;
        }
    }

    SetNil(a);
    return errNone;
}


int prArrayUnlace(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c, *slots, *slots2, *slots3;
    PyrObject *obj1, *obj2, *obj3;
    int i, j, k, clump, numLists, size, size3, err;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    obj1 = slotRawObject(a);
    slots = obj1->slots;
    size = obj1->size;

    err = slotIntVal(b, &numLists);
    if (err)
        return err;

    err = slotIntVal(c, &clump);
    if (err)
        return err;

    obj2 = instantiateObject(g->gc, obj1->classptr, numLists, false, true);
    slots2 = obj2->slots;

    SetObject(b, obj2); // store reference on stack, so both source and destination objects can be reached by the gc

    size3 = size / numLists;
    size3 = size3 - (size3 % clump);
    if (size3 < 1)
        return errFailed;

    for (i = 0; i < numLists; ++i) {
        obj3 = instantiateObject(g->gc, obj1->classptr, size3, false, true);
        obj3->size = size3;
        slots3 = obj3->slots;
        for (j = 0; j < size3; j += clump) {
            for (k = 0; k < clump; ++k) {
                slotCopy(&slots3[j + k], &slots[i * clump + k + j * numLists]);
            }
        }
        SetObject(slots2 + i, obj3);
        g->gc->GCWriteNew(obj2, obj3); // we know obj3 is white so we can use GCWriteNew
        obj2->size++;
    }

    SetRaw(a, obj2);

    return errNone;
}

void initArrayPrimitives() {
    int base, index;

    base = nextPrimitiveIndex();
    index = 0;

    definePrimitive(base, index++, "_BasicSize", basicSize, 1, 0);
    definePrimitive(base, index++, "_BasicMaxSize", basicMaxSize, 1, 0);

    definePrimitive(base, index++, "_BasicSwap", basicSwap, 3, 0);
    definePrimitive(base, index++, "_BasicAt", basicAt, 2, 0);
    definePrimitive(base, index++, "_BasicRemoveAt", basicRemoveAt, 2, 0);
    definePrimitive(base, index++, "_BasicTakeAt", basicTakeAt, 2, 0);
    definePrimitive(base, index++, "_BasicClipAt", basicClipAt, 2, 0);
    definePrimitive(base, index++, "_BasicWrapAt", basicWrapAt, 2, 0);
    definePrimitive(base, index++, "_BasicFoldAt", basicFoldAt, 2, 0);
    definePrimitive(base, index++, "_BasicPut", basicPut, 3, 0);
    definePrimitive(base, index++, "_BasicClipPut", basicClipPut, 3, 0);
    definePrimitive(base, index++, "_BasicWrapPut", basicWrapPut, 3, 0);
    definePrimitive(base, index++, "_BasicFoldPut", basicFoldPut, 3, 0);

    definePrimitive(base, index++, "_ArrayExtend", prArrayExtend, 3, 0);
    definePrimitive(base, index++, "_ArrayGrow", prArrayGrow, 2, 0);
    definePrimitive(base, index++, "_ArrayGrowClear", prArrayGrowClear, 2, 0);
    definePrimitive(base, index++, "_ArrayAdd", prArrayAdd, 2, 0);
    definePrimitive(base, index++, "_ArrayInsert", prArrayInsert, 3, 0);
    definePrimitive(base, index++, "_ArrayFill", prArrayFill, 2, 0);
    definePrimitive(base, index++, "_ArrayPop", prArrayPop, 1, 0);
    definePrimitive(base, index++, "_ArrayCat", prArrayCat, 2, 0);
    definePrimitive(base, index++, "_ArrayPutEach", prArrayPutEach, 3, 0);
    definePrimitive(base, index++, "_ArrayAddAll", prArrayAddAll, 2, 0);
    definePrimitive(base, index++, "_ArrayPutSeries", prArrayPutSeries, 5, 0);
    definePrimitive(base, index++, "_ArrayOverwrite", prArrayOverwrite, 3, 0);
    definePrimitive(base, index++, "_ArrayIndexOf", prArrayIndexOf, 2, 0);

    definePrimitive(base, index++, "_ArrayNormalizeSum", prArrayNormalizeSum, 1, 0);
    definePrimitive(base, index++, "_ArrayWIndex", prArrayWIndex, 1, 0);
    definePrimitive(base, index++, "_ArrayReverse", prArrayReverse, 1, 0);
    definePrimitive(base, index++, "_ArrayScramble", prArrayScramble, 1, 0);
    definePrimitive(base, index++, "_ArrayMirror", prArrayMirror, 1, 0);
    definePrimitive(base, index++, "_ArrayMirror1", prArrayMirror1, 1, 0);
    definePrimitive(base, index++, "_ArrayMirror2", prArrayMirror2, 1, 0);
    definePrimitive(base, index++, "_ArrayRotate", prArrayRotate, 2, 0);
    definePrimitive(base, index++, "_ArrayPermute", prArrayPermute, 2, 0);
    definePrimitive(base, index++, "_ArrayAllTuples", prArrayAllTuples, 2, 0);
    definePrimitive(base, index++, "_ArrayPyramid", prArrayPyramid, 2, 0);
    definePrimitive(base, index++, "_ArrayRotate", prArrayRotate, 2, 0);
    definePrimitive(base, index++, "_ArrayExtendWrap", prArrayExtendWrap, 2, 0);
    definePrimitive(base, index++, "_ArrayExtendFold", prArrayExtendFold, 2, 0);
    definePrimitive(base, index++, "_ArrayExtendLast", prArrayExtendLast, 2, 0);
    definePrimitive(base, index++, "_ArrayLace", prArrayLace, 2, 0);
    definePrimitive(base, index++, "_ArrayStutter", prArrayStutter, 2, 0);
    definePrimitive(base, index++, "_ArraySlide", prArraySlide, 3, 0);
    definePrimitive(base, index++, "_ArrayContainsSeqColl", prArrayContainsSeqColl, 1, 0);

    definePrimitive(base, index++, "_ArrayEnvAt", prArrayEnvAt, 2, 0);
    definePrimitive(base, index++, "_ArrayIndexOfGreaterThan", prArrayIndexOfGreaterThan, 2, 0);
    definePrimitive(base, index++, "_ArrayUnlace", prArrayUnlace, 3, 0);
}


#if _SC_PLUGINS_

#    include "SCPlugin.h"

#    pragma export on
extern "C" {
SCPlugIn* loadPlugIn(void);
}
#    pragma export off


// define plug in object
class APlugIn : public SCPlugIn {
public:
    APlugIn();
    virtual ~APlugIn();

    virtual void AboutToCompile();
};

APlugIn::APlugIn() {
    // constructor for plug in
}

APlugIn::~APlugIn() {
    // destructor for plug in
}

void APlugIn::AboutToCompile() {
    // this is called each time the class library is compiled.
    initArrayPrimitives();
}

// This function is called when the plug in is loaded into SC.
// It returns an instance of APlugIn.
SCPlugIn* loadPlugIn() { return new APlugIn(); }

#endif
