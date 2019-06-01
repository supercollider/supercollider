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

#include "PyrKernel.h"
#include "PyrKernelProto.h"
#include "PyrPrimitive.h"
#include "PyrPrimitiveProto.h"
#include "PyrSymbol.h"
#include "PyrListPrim.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "PyrSignal.h"
#include "PyrMessage.h"
#include "PyrSched.h"
#include "SC_RGen.h"
#include "SCBase.h"
#include <stdlib.h>
#include <string.h>

int objectPerform(VMGlobals* g, int numArgsPushed);

int ivxIdentDict_array, ivxIdentDict_size, ivxIdentDict_parent, ivxIdentDict_proto, ivxIdentDict_know;

int class_array_index, class_array_maxsubclassindex;
int class_identdict_index, class_identdict_maxsubclassindex;

PyrClass* class_identdict;
PyrSymbol *s_proto, *s_parent;
PyrSymbol *s_delta, *s_dur, *s_stretch;

// used in prEvent_IsRest
PyrSymbol *s_type, *s_rest, *s_empty, *s_r, *s_isRest;
PyrClass *class_rest, *class_metarest;

#define HASHSYMBOL(sym) (sym >> 5)

#define ISKINDOF(obj, lo, hi)                                                                                          \
    (objClassIndex = slotRawInt(&obj->classptr->classIndex), objClassIndex >= lo && objClassIndex <= hi)


int prArrayMultiChanExpand(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *slot, *slots1, *slots2, *slots3, *slots4;
    PyrObject *obj1, *obj2, *obj3, *obj4;
    int i, j, size, len, maxlen;

    a = g->sp;
    obj1 = slotRawObject(a);
    size = obj1->size;
    slots1 = obj1->slots;
    maxlen = 1;
    for (j = 0; j < size; ++j) {
        slot = slots1 + j;
        if (IsObj(slot)) {
            if (slotRawObject(slot)->classptr == class_array) {
                len = slotRawObject(slot)->size;
                maxlen = len > maxlen ? len : maxlen;
            } else if (isKindOf(slotRawObject(slot), class_sequenceable_collection)
                       && (slotRawObject(slot)->classptr != class_string)) {
                return errFailed; // this primitive only handles Arrays.
            }
        }
    }

    obj2 = newPyrArray(g->gc, maxlen, 0, true);
    SetObject(a, obj2);
    slots2 = obj2->slots;
    for (i = 0; i < maxlen; ++i) {
        obj3 = newPyrArray(g->gc, size, 0, true);
        obj3->size = size;
        SetObject(slots2 + i, obj3);
        g->gc->GCWriteNew(obj2, obj3); // we know obj3 is white so we can use GCWriteNew
        obj2->size++;
        slots1 = obj1->slots;
        slots3 = obj3->slots;
        for (j = 0; j < size; ++j) {
            slot = slots1 + j;
            if (IsObj(slot)) {
                if (slotRawObject(slot)->classptr == class_array && slotRawObject(slot)->size > 0) {
                    PyrSlot* slotToCopy;
                    obj4 = slotRawObject(slot);
                    slots4 = obj4->slots;
                    slotToCopy = &slots4[i % obj4->size];
                    slotCopy(&slots3[j], slotToCopy);
                    g->gc->GCWrite(obj3, slotToCopy);
                } else {
                    slotCopy(&slots3[j], slot);
                    g->gc->GCWrite(obj3, slot);
                }
            } else {
                slotCopy(&slots3[j], slot); // we don't need GCWrite here, as slot is not an object
            }
        }
    }

    return errNone;
}


int arrayAtIdentityHash(PyrObject* array, PyrSlot* key) {
    PyrSlot *slots, *test;
    unsigned int i, start, end, hash, maxHash;

    hash = calcHash(key);
    maxHash = array->size;
    start = hash % maxHash;
    end = array->size;
    slots = array->slots;
    for (i = start; i < end; i++) {
        test = slots + i;
        if (IsNil(test) || SlotEq(test, key))
            return i;
    }
    end = start - 1;
    for (i = 0; i <= end; i++) {
        test = slots + i;
        if (IsNil(test) || SlotEq(test, key))
            return i;
    }
    return -1;
}


int prArray_AtIdentityHash(struct VMGlobals* g, int numArgsPushed);
int prArray_AtIdentityHash(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrObject* array;
    int index;

    a = g->sp - 1; // array
    b = g->sp; // key

    array = slotRawObject(a);
    if (array->size == 0)
        return errFailed;
    index = arrayAtIdentityHash(array, b);
    SetInt(a, index);
    return errNone;
}


int arrayAtIdentityHashInPairs(PyrObject* array, PyrSlot* key) {
    PyrSlot *slots, *test;
    unsigned int i, start, end, hash, maxHash;

    hash = calcHash(key);
    maxHash = array->size >> 1;
    start = (hash % maxHash) << 1;
    end = array->size;
    slots = array->slots;
    for (i = start; i < end; i += 2) {
        test = slots + i;
        if (IsNil(test) || SlotEq(test, key))
            return i;
    }
    end = start - 2;
    for (i = 0; i <= end; i += 2) {
        test = slots + i;
        if (IsNil(test) || SlotEq(test, key))
            return i;
    }
    return -2;
}


int arrayAtIdentityHashInPairsWithHash(PyrObject* array, PyrSlot* key, int hash) {
    PyrSlot *slots, *test;
    unsigned int i, start, end, maxHash;

    maxHash = array->size >> 1;
    start = (hash % maxHash) << 1;
    end = array->size;
    slots = array->slots;
    for (i = start; i < end; i += 2) {
        test = slots + i;
        if (IsNil(test) || SlotEq(test, key))
            return i;
    }
    end = start - 2;
    for (i = 0; i <= end; i += 2) {
        test = slots + i;
        if (IsNil(test) || SlotEq(test, key))
            return i;
    }
    return -2;
}


int identDictPut(struct VMGlobals* g, PyrObject* dict, PyrSlot* key, PyrSlot* value);
int identDictPut(struct VMGlobals* g, PyrObject* dict, PyrSlot* key, PyrSlot* value) {
    PyrSlot *slot, *newslot;
    int i, index, size;
    PyrObject* array;

    bool knows = IsTrue(dict->slots + ivxIdentDict_know);
    if (knows && IsSym(key)) {
        if (slotRawSymbol(key) == s_parent) {
            slotCopy(&dict->slots[ivxIdentDict_parent], value);
            g->gc->GCWrite(dict, value);
            return errNone;
        }
        if (slotRawSymbol(key) == s_proto) {
            slotCopy(&dict->slots[ivxIdentDict_proto], value);
            g->gc->GCWrite(dict, value);
            return errNone;
        }
    }
    array = slotRawObject(&dict->slots[ivxIdentDict_array]);
    if (array->IsImmutable())
        return errImmutableObject;
    if (!isKindOf((PyrObject*)array, class_array))
        return errFailed;

    index = arrayAtIdentityHashInPairs(array, key);
    slot = array->slots + index;
    slotCopy(&slot[1], value);
    g->gc->GCWrite(array, value);
    if (IsNil(slot)) {
        slotCopy(slot, key);
        g->gc->GCWrite(array, key);
        size = slotRawInt(&dict->slots[ivxIdentDict_size]) + 1;
        SetRaw(&dict->slots[ivxIdentDict_size], size);
        if (array->size < size * 3) {
            PyrObject* newarray;
            newarray = newPyrArray(g->gc, size * 3, 0, false);
            newarray->size = ARRAYMAXINDEXSIZE(newarray);
            nilSlots(newarray->slots, newarray->size);
            slot = array->slots;
            for (i = 0; i < array->size; i += 2, slot += 2) {
                if (NotNil(slot)) {
                    index = arrayAtIdentityHashInPairs(newarray, slot);
                    newslot = newarray->slots + index;
                    slotCopy(&newslot[0], &slot[0]);
                    slotCopy(&newslot[1], &slot[1]);
                }
            }
            SetRaw(&dict->slots[ivxIdentDict_array], newarray);
            g->gc->GCWriteNew(dict, newarray); // we know newarray is white so we can use GCWriteNew
        }
    }
    return errNone;
}

int prIdentDict_Put(struct VMGlobals* g, int numArgsPushed);
int prIdentDict_Put(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;

    a = g->sp - 2; // dict
    b = g->sp - 1; // key
    c = g->sp; // value
    if (IsNil(b))
        return errWrongType;
    if (IsNil(c))
        return errFailed; // will call removeAt
    return identDictPut(g, slotRawObject(a), b, c);
}

int prIdentDict_PutGet(struct VMGlobals* g, int numArgsPushed);
int prIdentDict_PutGet(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c, *d, *slot, *newslot;
    int i, index, size;
    PyrObject* dict;
    PyrObject* array;

    a = g->sp - 2; // dict
    b = g->sp - 1; // key
    c = g->sp; // value
    d = ++g->sp; // push the stack to save the receiver

    slotCopy(d, a);
    dict = slotRawObject(d);
    array = slotRawObject(&dict->slots[ivxIdentDict_array]);
    if (!isKindOf((PyrObject*)array, class_array)) {
        SetNil(a);
        --g->sp;
        return errFailed;
    }

    index = arrayAtIdentityHashInPairs(array, b);
    slot = array->slots + index;
    slotCopy(a, &slot[1]);
    slotCopy(&slot[1], c);
    g->gc->GCWrite(array, c);
    if (IsNil(slot)) {
        slotCopy(slot, b);
        g->gc->GCWrite(array, b);
        size = slotRawInt(&dict->slots[ivxIdentDict_size]) + 1;
        SetRaw(&dict->slots[ivxIdentDict_size], size);
        if (array->size < size * 3) {
            PyrObject* newarray;
            newarray = newPyrArray(g->gc, size * 3, 0, true);
            newarray->size = ARRAYMAXINDEXSIZE(newarray);
            nilSlots(newarray->slots, newarray->size);
            slot = array->slots;
            for (i = 0; i < array->size; i += 2, slot += 2) {
                if (NotNil(slot)) {
                    index = arrayAtIdentityHashInPairs(newarray, slot);
                    newslot = newarray->slots + index;
                    slotCopy(&newslot[0], &slot[0]);
                    slotCopy(&newslot[1], &slot[1]);
                }
            }
            SetRaw(&dict->slots[ivxIdentDict_array], newarray);
            g->gc->GCWriteNew(dict, newarray); // we know newarray is white so we can use GCWriteNew
        }
    }
    --g->sp;
    return errNone;
}


int prArray_AtIdentityHashInPairs(struct VMGlobals* g, int numArgsPushed);
int prArray_AtIdentityHashInPairs(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    unsigned int i;

    a = g->sp - 1; // array
    b = g->sp; // key

    if (slotRawObject(a)->size < 2)
        return errFailed;
    i = arrayAtIdentityHashInPairs(slotRawObject(a), b);
    SetInt(a, i);
    return errNone;
}


bool identDict_lookupNonNil(PyrObject* dict, PyrSlot* key, int hash, PyrSlot* result);
bool identDict_lookupNonNil(PyrObject* dict, PyrSlot* key, int hash, PyrSlot* result) {
again:
    PyrSlot* dictslots = dict->slots;
    PyrSlot* arraySlot = dictslots + ivxIdentDict_array;

    if (isKindOfSlot(arraySlot, class_array)) {
        PyrObject* array = slotRawObject(arraySlot);

        int index = arrayAtIdentityHashInPairsWithHash(array, key, hash);
        if (SlotEq(key, array->slots + index)) {
            slotCopy(result, &array->slots[index + 1]);
            return true;
        }
    }

    PyrClass* identDictClass = s_identitydictionary->u.classobj;
    PyrSlot* parentSlot = dictslots + ivxIdentDict_parent;
    PyrSlot* protoSlot = dictslots + ivxIdentDict_proto;
    if (isKindOfSlot(parentSlot, identDictClass)) {
        if (isKindOfSlot(protoSlot, identDictClass)) {
            // recursive call.
            if (identDict_lookupNonNil(slotRawObject(protoSlot), key, hash, result))
                return true;
        }

        dict = slotRawObject(parentSlot);
        goto again; // tail call
    } else {
        if (isKindOfSlot(protoSlot, identDictClass)) {
            dict = slotRawObject(protoSlot);
            goto again; // tail call
        }
    }
    return false;
}

bool identDict_lookup(PyrObject* dict, PyrSlot* key, int hash, PyrSlot* result);
bool identDict_lookup(PyrObject* dict, PyrSlot* key, int hash, PyrSlot* result) {
again:
    PyrSlot* dictslots = dict->slots;
    PyrSlot* arraySlot = dictslots + ivxIdentDict_array;

    if (isKindOfSlot(arraySlot, class_array)) {
        PyrObject* array = slotRawObject(arraySlot);

        int index = arrayAtIdentityHashInPairsWithHash(array, key, hash);
        if (SlotEq(key, array->slots + index)) {
            slotCopy(result, &array->slots[index + 1]);
            return true;
        }
    }

    PyrClass* identDictClass = s_identitydictionary->u.classobj;
    PyrSlot* parentSlot = dictslots + ivxIdentDict_parent;
    PyrSlot* protoSlot = dictslots + ivxIdentDict_proto;
    if (isKindOfSlot(parentSlot, identDictClass)) {
        if (isKindOfSlot(protoSlot, identDictClass)) {
            // recursive call.
            if (identDict_lookup(slotRawObject(protoSlot), key, hash, result))
                return true;
        }

        dict = slotRawObject(parentSlot);
        goto again; // tail call
    } else {
        if (isKindOfSlot(protoSlot, identDictClass)) {
            dict = slotRawObject(protoSlot);
            goto again; // tail call
        }
    }
    SetNil(result);
    return false;
}

int prIdentDict_At(struct VMGlobals* g, int numArgsPushed);
int prIdentDict_At(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1; // dict
    PyrSlot* key = g->sp; // key
    PyrObject* dict = slotRawObject(a);

    bool knows = IsTrue(dict->slots + ivxIdentDict_know);
    if (knows && IsSym(key)) {
        if (slotRawSymbol(key) == s_parent) {
            slotCopy(a, &dict->slots[ivxIdentDict_parent]);
            return errNone;
        }
        if (slotRawSymbol(key) == s_proto) {
            slotCopy(a, &dict->slots[ivxIdentDict_proto]);
            return errNone;
        }
    }

    identDict_lookup(dict, key, calcHash(key), a);
    return errNone;
}

int prSymbol_envirGet(struct VMGlobals* g, int numArgsPushed);
int prSymbol_envirGet(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, result;
    int objClassIndex;

    a = g->sp; // key

    PyrSlot* currentEnvironmentSlot = &g->classvars->slots[1];
    PyrObject* dict = slotRawObject(currentEnvironmentSlot);

    if (!IsObj(currentEnvironmentSlot))
        return errFailed;

    if (!ISKINDOF(dict, class_identdict_index, class_identdict_maxsubclassindex))
        return errFailed;

    identDict_lookup(dict, a, calcHash(a), &result);
    slotCopy(a, &result);

    return errNone;
}


int prSymbol_envirPut(struct VMGlobals* g, int numArgsPushed);
int prSymbol_envirPut(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    int objClassIndex;

    a = g->sp - 1; // key
    b = g->sp; // value

    PyrSlot* currentEnvironmentSlot = &g->classvars->slots[1];
    PyrObject* dict = slotRawObject(currentEnvironmentSlot);

    if (!IsObj(currentEnvironmentSlot))
        return errFailed;

    if (!ISKINDOF(dict, class_identdict_index, class_identdict_maxsubclassindex))
        return errFailed;

    int err = identDictPut(g, dict, a, b);
    if (err)
        return err;

    slotCopy(a, b);

    return errNone;
}


int prEvent_Delta(struct VMGlobals* g, int numArgsPushed);
int prEvent_Delta(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, key, dur, stretch, delta;
    double fdur, fstretch;
    int err;
    PyrClass* restClass = getsym("Rest")->u.classobj;
    PyrSlot* slot;

    a = g->sp; // dict

    SetSymbol(&key, s_delta);
    identDict_lookup(slotRawObject(a), &key, calcHash(&key), &delta);

    if (NotNil(&delta)) {
        if (isKindOfSlot(&delta, restClass)) {
            slot = slotRawObject(&delta)->slots;
            err = slotDoubleVal(slot, &fdur);
        } else {
            err = slotDoubleVal(&delta, &fdur);
        }
        if (err) {
            return err;
        } else {
            SetFloat(a, fdur);
            return errNone;
        }
    } else {
        SetSymbol(&key, s_dur);
        identDict_lookup(slotRawObject(a), &key, calcHash(&key), &dur);
        err = slotDoubleVal(&dur, &fdur);
        if (err) {
            if (IsNil(&dur)) {
                SetNil(g->sp);
                return errNone;
            } else if (isKindOfSlot(&dur, restClass)) {
                slot = slotRawObject(&dur)->slots;
                err = slotDoubleVal(slot, &fdur);
                if (err)
                    return err;
            } else {
                return errWrongType;
            }
        }
        SetSymbol(&key, s_stretch);
        identDict_lookup(slotRawObject(a), &key, calcHash(&key), &stretch);

        err = slotDoubleVal(&stretch, &fstretch);
        if (err) {
            if (NotNil(&stretch)) {
                if (isKindOfSlot(&stretch, restClass)) {
                    slot = slotRawObject(&stretch)->slots;
                    err = slotDoubleVal(slot, &fstretch);
                    if (err)
                        return err;
                } else {
                    return errWrongType;
                }
            } else {
                SetFloat(a, fdur);
                return errNone;
            }
        }

        SetFloat(a, fdur * fstretch);
    }

    return errNone;
}

/// Returns whether the slot is considered a rest for \c Event.isRest.
static bool slotIsRestlike(PyrSlot* slot) {
    PyrSymbol* slotSym;
    if (isKindOfSlot(slot, class_rest) || isKindOfSlot(slot, class_metarest)) {
        return true;
    } else if (!slotSymbolVal(slot, &slotSym)) {
        return slotSym == s_empty || slotSym == s_r || slotSym == s_rest;
    }
    // why no 'else'?
    // slotSymbolVal nonzero return = not a symbol;
    // non-symbols don't indicate rests, so, ignore them.

    return false;
}

/// Returns whether the dictionary has an entry with a 'restlike' value (see \c slotIsRestlike).
static bool dictHasRestlikeValue(PyrObject* array) {
    auto finalSlot = array->slots + array->size;

    // odd-numered slots are values
    for (auto slot = array->slots + 1; slot < finalSlot; slot += 2)
        if (slotIsRestlike(slot))
            return true;

    return false;
}

int prEvent_IsRest(struct VMGlobals* g, int numArgsPushed);
int prEvent_IsRest(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* dictslots = slotRawObject(g->sp)->slots;
    PyrSlot* arraySlot = dictslots + ivxIdentDict_array;
    static int isRestCount = 0;

    if (!isKindOfSlot(arraySlot, class_array)) {
        return errWrongType;
    }

    PyrSlot key, typeSlot;
    PyrSymbol* typeSym;
    // easy tests first: 'this[\type] == \rest'
    SetSymbol(&key, s_type);
    identDict_lookup(slotRawObject(g->sp), &key, calcHash(&key), &typeSlot);
    if (!slotSymbolVal(&typeSlot, &typeSym) && typeSym == s_rest) {
        SetBool(g->sp, 1);
        return errNone;
    }

    // and, 'this[\isRest] == true'
    SetSymbol(&key, s_isRest);
    identDict_lookup(slotRawObject(g->sp), &key, calcHash(&key), &typeSlot);
    if (IsTrue(&typeSlot)) {
        if (isRestCount == 0)
            post("\nWARNING: Setting isRest to true in an event is deprecated. See the Rest helpfile for supported "
                 "ways to specify rests.\n\n");
        isRestCount = (isRestCount + 1) % 100;
        SetBool(g->sp, 1);
        return errNone;
    }

    // failing those, scan slot values for something rest-like
    PyrObject* array = slotRawObject(arraySlot);
    SetBool(g->sp, dictHasRestlikeValue(array) ? 1 : 0);
    return errNone;
}

void PriorityQueueAdd(struct VMGlobals* g, PyrObject* queueobj, PyrSlot* item, double time);
void PriorityQueueAdd(struct VMGlobals* g, PyrObject* queueobj, PyrSlot* item, double time) {
    PyrObject *schedq, *newschedq;
    int size, maxsize;

    PyrSlot* schedqSlot = queueobj->slots;
    if (!IsObj(schedqSlot)) {
        size = 32;
        schedq = newPyrArray(g->gc, size, 0, true);
        schedq->size = 1;
        SetInt(schedq->slots + 0, 0); // stability count
        SetObject(schedqSlot, schedq);
        g->gc->GCWriteNew(queueobj, schedq); // we know schedq is white so we can use GCWriteNew
    } else {
        schedq = slotRawObject(schedqSlot);
        maxsize = ARRAYMAXINDEXSIZE(schedq);
        size = schedq->size;
        if (size + 3 > maxsize) {
            newschedq = newPyrArray(g->gc, maxsize * 2, 0, true);
            newschedq->size = size;

            slotCopy(newschedq->slots, schedq->slots, size);
            assert(IsInt(newschedq->slots));

            SetObject(schedqSlot, newschedq);
            g->gc->GCWriteNew(queueobj, newschedq); // we know newschedq is white so we can use GCWriteNew

            schedq = newschedq;
        }
    }

    addheap(g, schedq, time, item);
}

int prPriorityQueueAdd(struct VMGlobals* g, int numArgsPushed);
int prPriorityQueueAdd(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 2; // priority queue
    PyrSlot* b = g->sp - 1; // time
    PyrSlot* c = g->sp; // item

    double time;
    int err = slotDoubleVal(b, &time);
    if (err)
        return errNone; // nil is OK, nothing gets added

    PriorityQueueAdd(g, slotRawObject(a), c, time);
    return errNone;
}


void PriorityQueuePop(VMGlobals* g, PyrObject* queueobj, PyrSlot* result);
void PriorityQueuePop(VMGlobals* g, PyrObject* queueobj, PyrSlot* result) {
    PyrSlot* schedqSlot = queueobj->slots;

    if (IsObj(schedqSlot)) {
        PyrObject* schedq = slotRawObject(schedqSlot);
        double time;
        if (!getheap(g, schedq, &time, result)) {
            SetNil(result);
        }
    } else {
        SetNil(result);
    }
}

void PriorityQueueTop(PyrObject* queueobj, PyrSlot* result);
void PriorityQueueTop(PyrObject* queueobj, PyrSlot* result) {
    PyrSlot* schedqSlot = queueobj->slots;

    if (IsObj(schedqSlot)) {
        PyrObject* schedq = slotRawObject(schedqSlot);
        if (schedq->size > 1) {
            slotCopy(result, &schedq->slots[1]);
        } else {
            SetNil(result);
        }
    } else {
        SetNil(result);
    }
}

void PriorityQueueClear(PyrObject* queueobj);
void PriorityQueueClear(PyrObject* queueobj) {
    PyrSlot* schedqSlot = queueobj->slots;

    if (IsObj(schedqSlot)) {
        PyrObject* schedq = slotRawObject(schedqSlot);
        SetInt(schedq->slots, 0); // stability count
        schedq->size = 1;
    }
}

bool PriorityQueueEmpty(PyrObject* queueobj);
bool PriorityQueueEmpty(PyrObject* queueobj) {
    PyrSlot* schedqSlot = queueobj->slots;

    if (IsObj(schedqSlot)) {
        PyrObject* schedq = slotRawObject(schedqSlot);
        if (schedq->size > 1) {
            return false;
        }
    }
    return true;
}

int prPriorityQueuePop(struct VMGlobals* g, int numArgsPushed);
int prPriorityQueuePop(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp; // priority queue

    PriorityQueuePop(g, slotRawObject(a), a);
    return errNone;
}

int prPriorityQueueTop(struct VMGlobals* g, int numArgsPushed);
int prPriorityQueueTop(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp; // priority queue

    PriorityQueueTop(slotRawObject(a), a);
    return errNone;
}

int prPriorityQueueClear(struct VMGlobals* g, int numArgsPushed);
int prPriorityQueueClear(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp; // priority queue

    PriorityQueueClear(slotRawObject(a));
    return errNone;
}

int prPriorityQueueEmpty(struct VMGlobals* g, int numArgsPushed);
int prPriorityQueueEmpty(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;

    a = g->sp; // priority queue

    if (PriorityQueueEmpty(slotRawObject(a))) {
        SetTrue(a);
    } else {
        SetFalse(a);
    }
    return errNone;
}

void PriorityQueuePostpone(PyrObject* queueobj, double time);
void PriorityQueuePostpone(PyrObject* queueobj, double time) {
    PyrSlot* schedqSlot = queueobj->slots;

    if (IsObj(schedqSlot)) {
        PyrObject* schedq = slotRawObject(schedqSlot);
        PyrSlot* slots = schedq->slots;
        for (int i = 1; i < schedq->size; i += 3) {
            SetRaw(&slots[i], slotRawFloat(&slots[i]) + time);
        }
    }
}

int prPriorityQueuePostpone(struct VMGlobals* g, int numArgsPushed);
int prPriorityQueuePostpone(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1; // priority queue
    PyrSlot* b = g->sp; // time

    double time;
    int err = slotDoubleVal(b, &time);
    if (err)
        return err;

    PyrObject* queueobj = slotRawObject(a);
    PriorityQueuePostpone(queueobj, time);
    return errNone;
}


void initListPrimitives();
void initListPrimitives() {
    int base, index;

    base = nextPrimitiveIndex();
    index = 0;
    definePrimitive(base, index++, "_Array_AtIdentityHash", prArray_AtIdentityHash, 2, 0);
    definePrimitive(base, index++, "_Array_AtIdentityHashInPairs", prArray_AtIdentityHashInPairs, 2, 0);
    definePrimitive(base, index++, "_IdentDict_Put", prIdentDict_Put, 3, 0);
    definePrimitive(base, index++, "_IdentDict_PutGet", prIdentDict_PutGet, 3, 0);
    definePrimitive(base, index++, "_IdentDict_At", prIdentDict_At, 2, 0);
    definePrimitive(base, index++, "_Symbol_envirGet", prSymbol_envirGet, 1, 0);
    definePrimitive(base, index++, "_Symbol_envirPut", prSymbol_envirPut, 2, 0);
    definePrimitive(base, index++, "_ArrayMultiChannelExpand", prArrayMultiChanExpand, 1, 0);

    definePrimitive(base, index++, "_PriorityQueueAdd", prPriorityQueueAdd, 3, 0);
    definePrimitive(base, index++, "_PriorityQueuePop", prPriorityQueuePop, 1, 0);
    definePrimitive(base, index++, "_PriorityQueueTop", prPriorityQueueTop, 1, 0);
    definePrimitive(base, index++, "_PriorityQueueClear", prPriorityQueueClear, 1, 0);
    definePrimitive(base, index++, "_PriorityQueueEmpty", prPriorityQueueEmpty, 1, 0);
    definePrimitive(base, index++, "_PriorityQueuePostpone", prPriorityQueuePostpone, 2, 0);

    definePrimitive(base, index++, "_Event_Delta", prEvent_Delta, 1, 0);
    definePrimitive(base, index++, "_Event_IsRest", prEvent_IsRest, 1, 0);
}

void initPatterns();
void initPatterns() {
    PyrSymbol* sym;

    ivxIdentDict_array = instVarOffset("IdentityDictionary", "array");
    ivxIdentDict_size = instVarOffset("IdentityDictionary", "size");
    ivxIdentDict_parent = instVarOffset("IdentityDictionary", "parent");
    ivxIdentDict_proto = instVarOffset("IdentityDictionary", "proto");
    ivxIdentDict_know = instVarOffset("IdentityDictionary", "know");

    sym = getsym("IdentityDictionary");
    class_identdict = sym ? sym->u.classobj : NULL;
    class_identdict_index = slotRawInt(&class_identdict->classIndex);
    class_identdict_maxsubclassindex = slotRawInt(&class_identdict->maxSubclassIndex);

    class_array_index = slotRawInt(&class_array->classIndex);
    class_array_maxsubclassindex = slotRawInt(&class_array->maxSubclassIndex);

    s_parent = getsym("parent");
    s_proto = getsym("proto");
    s_delta = getsym("delta");
    s_dur = getsym("dur");
    s_stretch = getsym("stretch");

    // used in prEvent_IsRest
    s_type = getsym("type");
    s_rest = getsym("rest");
    s_empty = getsym("");
    s_r = getsym("r");
    s_isRest = getsym("isRest");

    class_rest = getsym("Rest")->u.classobj;
    class_metarest = getsym("Meta_Rest")->u.classobj;
}
