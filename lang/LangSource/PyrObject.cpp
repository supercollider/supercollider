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

#include <atomic>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "GC.h"
#include "PyrMessage.h"
#include "PyrInterpreter.h"
#include "PyrSymbolTable.h"
#include "PyrObjectProto.h"
#include "PyrKernelProto.h"
#include "PyrLexer.h"
#include "InitAlloc.h"
#include "Hash.h"
#include "SC_Constants.h"
#include "SC_Alloca.h"
#include "SC_Lock.h"

#include <set>
#include <limits>

#include <memory>
#include <boost/range/irange.hpp>

#define BOOST_THREAD_VERSION 4
#define BOOST_THREAD_PROVIDES_EXECUTORS

#include <boost/thread/future.hpp>
#include <boost/thread/executor.hpp>
#include <boost/thread/executors/basic_thread_pool.hpp>

#ifdef _MSC_VER
#    include <future>
#endif

#if 0 // not yet
#    include <parallel/algorithm>
#endif


PyrClass* gClassList = NULL;
int gNumSelectors = 0;
int gNumClasses = 0;
int gNumClassVars = 0;
int gFormatElemSize[NUMOBJFORMATS];
int gFormatElemCapc[NUMOBJFORMATS];
int gFormatElemTag[NUMOBJFORMATS];
PyrMethod* gNullMethod; // used to fill row table
PyrClass* gTagClassTable[16];

PyrClass* class_object;
PyrClass* class_dict;
PyrClass* class_array;
PyrClass *class_list, *class_method, *class_fundef, *class_frame, *class_class;
PyrClass *class_symbol, *class_nil;
PyrClass *class_boolean, *class_true, *class_false;
PyrClass *class_int, *class_char, *class_float, *class_complex;
PyrClass* class_rawptr;
PyrClass* class_string;
PyrClass *class_magnitude, *class_number, *class_collection, *class_ordered_collection;
PyrClass* class_arrayed_collection;
PyrClass* class_sequenceable_collection;
PyrClass* class_simple_number;
PyrClass* class_rawarray;
PyrClass* class_signal;
PyrClass* class_wavetable;
PyrClass* class_floatarray;
PyrClass* class_int8array;
PyrClass* class_int16array;
PyrClass* class_int32array;
PyrClass* class_symbolarray;
PyrClass* class_doublearray;
PyrClass *class_func, *class_absfunc;
PyrClass* class_stream;
PyrClass* class_process;
PyrClass* class_interpreter;
PyrClass* class_thread;
PyrClass* class_routine;
PyrClass* class_finalizer;
PyrClass* class_server_shm_interface;

PyrSymbol* s_none;
PyrSymbol* s_object;
PyrSymbol* s_bag;
PyrSymbol* s_set;
PyrSymbol* s_identityset;
PyrSymbol* s_dictionary;
PyrSymbol* s_identitydictionary;
PyrSymbol* s_linkedlist;
PyrSymbol* s_sortedlist;
PyrSymbol* s_array;
PyrSymbol *s_list, *s_method, *s_fundef, *s_frame, *s_class;
PyrSymbol *s_symbol, *s_nil;
PyrSymbol *s_boolean, *s_true, *s_false;
PyrSymbol *s_int, *s_char, *s_color, *s_float, *s_complex;
PyrSymbol* s_rawptr;
PyrSymbol* s_string;
PyrSymbol *s_magnitude, *s_number, *s_collection, *s_ordered_collection;
PyrSymbol* s_arrayed_collection;
PyrSymbol* s_sequenceable_collection;
PyrSymbol* s_simple_number;
PyrSymbol* s_signal;
PyrSymbol* s_wavetable;

PyrSymbol* s_rawarray;
PyrSymbol* s_int8array;
PyrSymbol* s_int16array;
PyrSymbol* s_int32array;
PyrSymbol* s_symbolarray;
PyrSymbol* s_doublearray;
PyrSymbol* s_floatarray;
PyrSymbol* s_rect;
PyrSymbol *s_func, *s_absfunc;
PyrSymbol* s_stream;
PyrSymbol* s_process;
PyrSymbol* s_main;
PyrSymbol* s_thread;
PyrSymbol* s_routine;
PyrSymbol* s_task;
PyrSymbol* s_prstart;
PyrSymbol* s_interpreter;
PyrSymbol* s_finalizer;
PyrSymbol* s_awake;
PyrSymbol* s_systemclock;
PyrSymbol* s_server_shm_interface;
PyrSymbol *s_interpretCmdLine, *s_interpretPrintCmdLine;

PyrSymbol* s_doesNotUnderstand;
PyrSymbol *s_curProcess, *s_curMethod, *s_curBlock, *s_curClosure, *s_curThread;
PyrSymbol *s_run, *s_stop, *s_tick;
PyrSymbol* s_startup;
PyrSymbol* s_next;
PyrSymbol* s_env;
PyrSymbol *s_new, *s_ref, *s_value, *s_at, *s_put;
PyrSymbol *s_performList, *s_superPerformList;
PyrSymbol *s_series, *s_copyseries, *s_putseries;
PyrSymbol *s_envirGet, *s_envirPut;
PyrSymbol *s_synth, *s_environment, *s_event;
PyrSymbol* s_shutdown;
PyrSymbol *s_super, *s_this;

PyrSlot o_nil, o_true, o_false;
PyrSlot o_fhalf, o_fnegone, o_fzero, o_fone, o_ftwo, o_inf;
PyrSlot o_negone, o_zero, o_one, o_two;
PyrSlot o_none;
PyrSlot o_emptyarray, o_onenilarray, o_argnamethis;


void initSymbols() {
    // basic keywords
    s_new = getsym("new");
    s_none = getsym("__none");
    /* Dummy symbol for null superclass or primitive. Prefixed with
     * `__` to avoid collisions with possible method and primitive names.
     */
    s_this = getsym("this");
    s_super = getsym("super");
    s_curProcess = getsym("thisProcess");
    s_curThread = getsym("thisThread");
    s_curMethod = getsym("thisMethod");
    s_curBlock = getsym("thisFunctionDef");
    s_curClosure = getsym("thisFunction");

    // classes
    s_object = getsym("Object");
    s_ref = getsym("Ref");
    s_dictionary = getsym("Dictionary");
    s_bag = getsym("Bag");
    s_set = getsym("Set");
    s_identityset = getsym("IdentitySet");
    s_identitydictionary = getsym("IdentityDictionary");
    s_linkedlist = getsym("LinkedList");
    s_sortedlist = getsym("SortedList");
    s_array = getsym("Array");
    s_list = getsym("List");
    s_method = getsym("Method");
    s_fundef = getsym("FunctionDef");
    s_frame = getsym("Frame");
    s_class = getsym("Class");
    s_symbol = getsym("Symbol");
    s_nil = getsym("Nil");
    s_true = getsym("True");
    s_false = getsym("False");
    s_int = getsym("Integer");
    s_float = getsym("Float");
    s_char = getsym("Char");
    s_color = getsym("Color");
    s_rawptr = getsym("RawPointer");
    s_string = getsym("String");
    s_magnitude = getsym("Magnitude");
    s_number = getsym("Number");
    s_simple_number = getsym("SimpleNumber");
    s_collection = getsym("Collection");
    s_arrayed_collection = getsym("ArrayedCollection");
    s_sequenceable_collection = getsym("SequenceableCollection");
    s_boolean = getsym("Boolean");
    s_signal = getsym("Signal");
    s_wavetable = getsym("Wavetable");
    s_rawarray = getsym("RawArray");
    s_int8array = getsym("Int8Array");
    s_int16array = getsym("Int16Array");
    s_int32array = getsym("Int32Array");
    s_symbolarray = getsym("SymbolArray");
    s_floatarray = getsym("FloatArray");
    s_doublearray = getsym("DoubleArray");
    s_complex = getsym("Complex");
    s_rect = getsym("Rect");
    s_absfunc = getsym("AbstractFunction");
    s_func = getsym("Function");
    s_stream = getsym("Stream");
    s_process = getsym("Process");
    s_main = getsym("Main");
    s_thread = getsym("Thread");
    s_routine = getsym("Routine");
    s_task = getsym("Task");
    s_interpreter = getsym("Interpreter");
    s_finalizer = getsym("Finalizer");
    s_systemclock = getsym("SystemClock");
    s_server_shm_interface = getsym("ServerShmInterface");
    s_env = getsym("Env");
    s_synth = getsym("Synth");
    s_environment = getsym("Environment");
    s_event = getsym("Event");

    // interpreter
    s_interpretCmdLine = getsym("interpretCmdLine");
    s_interpretPrintCmdLine = getsym("interpretPrintCmdLine");

    s_doesNotUnderstand = getsym("doesNotUnderstand");

    s_startup = getsym("startup");
    s_awake = getsym("awake");
    s_shutdown = getsym("shutdown");

    // methods
    s_run = getsym("run");
    s_stop = getsym("stop");
    s_tick = getsym("tick");
    s_next = getsym("next");
    s_value = getsym("value");
    s_performList = getsym("performList");
    s_superPerformList = getsym("superPerformList");
    s_at = getsym("at");
    s_put = getsym("put");
    s_prstart = getsym("prStart");
    s_series = getsym("prSimpleNumberSeries");
    s_copyseries = getsym("copySeries");
    s_putseries = getsym("putSeries");

    s_envirGet = getsym("envirGet");
    s_envirPut = getsym("envirPut");

    // set special value slots
    SetSymbol(&o_none, s_none);

    SetNil(&o_nil);
    SetFalse(&o_false);
    SetTrue(&o_true);

    SetInt(&o_negone, -1);
    SetInt(&o_zero, 0);
    SetInt(&o_one, 1);
    SetInt(&o_two, 2);

    SetFloat(&o_fhalf, .5);
    SetFloat(&o_fnegone, -1.);
    SetFloat(&o_fzero, 0.);
    SetFloat(&o_fone, 1.);
    SetFloat(&o_ftwo, 2.);
    SetFloat(&o_inf, std::numeric_limits<double>::infinity());

    slotCopy(&gSpecialValues[svNil], &o_nil);
    slotCopy(&gSpecialValues[svFalse], &o_false);
    slotCopy(&gSpecialValues[svTrue], &o_true);

    slotCopy(&gSpecialValues[svNegOne], &o_negone);
    slotCopy(&gSpecialValues[svZero], &o_zero);
    slotCopy(&gSpecialValues[svOne], &o_one);
    slotCopy(&gSpecialValues[svTwo], &o_two);

    slotCopy(&gSpecialValues[svFHalf], &o_fhalf);
    slotCopy(&gSpecialValues[svFNegOne], &o_fnegone);
    slotCopy(&gSpecialValues[svFZero], &o_fzero);
    slotCopy(&gSpecialValues[svFOne], &o_fone);
    slotCopy(&gSpecialValues[svFTwo], &o_ftwo);
    slotCopy(&gSpecialValues[svInf], &o_inf);


    gFormatElemSize[obj_notindexed] = sizeof(PyrSlot);
    gFormatElemSize[obj_slot] = sizeof(PyrSlot);
    gFormatElemSize[obj_double] = sizeof(double);
    gFormatElemSize[obj_float] = sizeof(float);
    gFormatElemSize[obj_int32] = sizeof(int32);
    gFormatElemSize[obj_int16] = sizeof(int16);
    gFormatElemSize[obj_int8] = sizeof(int8);
    gFormatElemSize[obj_char] = sizeof(char);
    gFormatElemSize[obj_symbol] = sizeof(PyrSymbol*);

    gFormatElemCapc[obj_notindexed] = sizeof(PyrSlot) / sizeof(PyrSlot);
    gFormatElemCapc[obj_slot] = sizeof(PyrSlot) / sizeof(PyrSlot);
    gFormatElemCapc[obj_double] = sizeof(PyrSlot) / sizeof(double);
    gFormatElemCapc[obj_float] = sizeof(PyrSlot) / sizeof(float);
    gFormatElemCapc[obj_int32] = sizeof(PyrSlot) / sizeof(int32);
    gFormatElemCapc[obj_int16] = sizeof(PyrSlot) / sizeof(int16);
    gFormatElemCapc[obj_int8] = sizeof(PyrSlot) / sizeof(int8);
    gFormatElemCapc[obj_char] = sizeof(PyrSlot) / sizeof(char);
    gFormatElemCapc[obj_symbol] = sizeof(PyrSlot) / sizeof(PyrSymbol*);

    gFormatElemTag[obj_notindexed] = -1;
    gFormatElemTag[obj_slot] = -1;
    gFormatElemTag[obj_double] = 0;
    gFormatElemTag[obj_float] = 0;
    gFormatElemTag[obj_int32] = tagInt;
    gFormatElemTag[obj_int16] = tagInt;
    gFormatElemTag[obj_int8] = tagInt;
    gFormatElemTag[obj_char] = tagChar;
    gFormatElemTag[obj_symbol] = tagSym;
}

const char* slotSymString(PyrSlot* slot) {
    switch (GetTag(slot)) {
    case tagObj:
        return slotRawSymbol(&slotRawObject(slot)->classptr->name)->name;
    case tagInt:
        return "Integer";
    case tagChar:
        return "Char";
    case tagSym:
        return slotRawSymbol(slot)->name;
    case tagNil:
        return "Nil";
    case tagFalse:
        return "False";
    case tagTrue:
        return "True";
    default:
        return "<float>";
    }
}

PyrClass* newClassObj(PyrClass* classObjSuperClass, PyrSymbol* className, PyrSymbol* superClassName, int numInstVars,
                      int numClassVars, int numConsts, int numInstMethods, int instFormat, int instFlags) {
    PyrClass *classobj, *superclassobj;
    PyrObject* array;
    PyrSymbolArray* symarray;
    int classFlags;

    /*{
        int c;
        c = className->name[0];
        if (!(c >= 'A' && c <= 'Z')) Debugger();
    }*/
    // pyrmalloc:
    // lifetime: kill upon recompiling library
    classobj = (PyrClass*)pyr_pool_runtime->Alloc(sizeof(PyrClass));
    MEMFAIL(classobj);
    classobj->size = (sizeof(PyrClass) - sizeof(PyrObjectHdr)) / sizeof(PyrSlot);
    classobj->prev = classobj->next = NULL;
    classobj->obj_flags = obj_immutable;
    classobj->obj_format = obj_notindexed;
    classobj->gc_color = obj_permanent;
    classobj->obj_sizeclass = LOG2CEIL(classobj->size);
    classobj->classptr = classObjSuperClass;


    // append to the list of classes
    if (gClassList)
        SetObject(&classobj->nextclass, gClassList);
    else
        SetNil(&classobj->nextclass);

    gClassList = classobj;

    className->u.classobj = classobj;
    // postfl("> '%s' %d %d\n", className->name, className, classobj);

    SetSymbol(&classobj->name, className);
    if (superClassName) {
        SetSymbol(&classobj->superclass, superClassName);
        superclassobj = superClassName->u.classobj;
    } else {
        SetSymbol(&classobj->superclass, s_none);
        superclassobj = NULL;
    }

    SetInt(&classobj->subclasses, 0); // to be filled with subclasses later
    // in the meantime it is used as a tally of subclasses so that its allocation
    // size can be known

    if (numInstMethods) {
        array = newPyrArray(NULL, numInstMethods, obj_permanent | obj_immutable, false);
        SetObject(&classobj->methods, array);
    } else {
        SetNil(&classobj->methods);
    }

    if (numInstVars) {
        symarray = newPyrSymbolArray(NULL, numInstVars, obj_permanent | obj_immutable, false);
        SetObject(&classobj->instVarNames, symarray);

        array = newPyrArray(NULL, numInstVars, obj_permanent | obj_immutable, false);
        SetObject(&classobj->iprototype, array);
        nilSlots(array->slots, numInstVars);
    } else {
        SetNil(&classobj->instVarNames);
        SetNil(&classobj->iprototype);
    }

    if (numClassVars) {
        symarray = newPyrSymbolArray(NULL, numClassVars, obj_permanent | obj_immutable, false);
        SetObject(&classobj->classVarNames, symarray);

        array = newPyrArray(NULL, numClassVars, obj_permanent | obj_immutable, false);
        SetObject(&classobj->cprototype, array);
        nilSlots(array->slots, numClassVars);
    } else {
        SetNil(&classobj->classVarNames);
        SetNil(&classobj->cprototype);
    }

    if (numConsts) {
        symarray = newPyrSymbolArray(NULL, numConsts, obj_permanent | obj_immutable, false);
        SetObject(&classobj->constNames, symarray);

        array = newPyrArray(NULL, numConsts, obj_permanent | obj_immutable, false);
        SetObject(&classobj->constValues, array);
        nilSlots(array->slots, numConsts);
    } else {
        SetNil(&classobj->constNames);
        SetNil(&classobj->constValues);
    }

    classFlags = 0;
    if (instFormat != obj_notindexed) {
        classFlags |= classHasIndexableInstances;
    }

    SetInt(&classobj->instanceFormat, instFormat);
    SetInt(&classobj->instanceFlags, instFlags);
    SetInt(&classobj->classIndex, -1);
    SetInt(&classobj->classFlags, classFlags);
    SetInt(&classobj->maxSubclassIndex, 0);
    SetNil(&classobj->filenameSym);
    SetInt(&classobj->charPos, 0);
    SetInt(&classobj->classVarIndex, gNumClassVars);
    // if (numClassVars) post("%16s %4d %4d\n", className->name, gNumClassVars, numClassVars);
    gNumClassVars += numClassVars;
    return classobj;
}

void reallocClassObj(PyrClass* classobj, int numInstVars, int numClassVars, int numConsts, int numMethods,
                     int instFormat, int instFlags) {
    PyrObject* array;
    PyrSymbolArray* symarray;
    PyrClass* superclassobj;

    freePyrSlot(&classobj->methods);
    freePyrSlot(&classobj->instVarNames);
    freePyrSlot(&classobj->classVarNames);
    freePyrSlot(&classobj->iprototype);
    freePyrSlot(&classobj->cprototype);

    freePyrSlot(&classobj->constNames);
    freePyrSlot(&classobj->constValues);

    if (numMethods) {
        array = newPyrArray(NULL, numMethods, obj_permanent | obj_immutable, false);
        SetObject(&classobj->methods, array);
    } else {
        SetNil(&classobj->methods);
    }

    if (numInstVars) {
        // post("reallocClassObj %s numInstVars %d\n", slotRawSymbol(&classobj->name)->name, numInstVars);
        symarray = newPyrSymbolArray(NULL, numInstVars, obj_permanent | obj_immutable, false);
        SetObject(&classobj->instVarNames, symarray);

        array = newPyrArray(NULL, numInstVars, obj_permanent | obj_immutable, false);
        SetObject(&classobj->iprototype, array);
        nilSlots(array->slots, numInstVars);
    } else {
        SetNil(&classobj->instVarNames);
        SetNil(&classobj->iprototype);
    }

    if (numClassVars) {
        // post("reallocClassObj %s numClassVars %d\n", slotRawSymbol(&classobj->name)->name, numClassVars);
        symarray = newPyrSymbolArray(NULL, numClassVars, obj_permanent | obj_immutable, false);
        SetObject(&classobj->classVarNames, symarray);
        nilSlots(array->slots, numClassVars);

        array = newPyrArray(NULL, numClassVars, obj_permanent | obj_immutable, false);
        SetObject(&classobj->cprototype, array);
        nilSlots(array->slots, numClassVars);
    } else {
        SetNil(&classobj->classVarNames);
        SetNil(&classobj->cprototype);
    }

    if (numConsts) {
        // post("reallocClassObj %s numConsts %d\n", slotRawSymbol(&classobj->name)->name, numConsts);
        symarray = newPyrSymbolArray(NULL, numConsts, obj_permanent | obj_immutable, false);
        SetObject(&classobj->constNames, symarray);

        array = newPyrArray(NULL, numConsts, obj_permanent | obj_immutable, false);
        SetObject(&classobj->constValues, array);
        nilSlots(array->slots, numConsts);
    } else {
        SetNil(&classobj->constNames);
        SetNil(&classobj->constValues);
    }

    superclassobj = slotRawSymbol(&classobj->superclass)->u.classobj;

    SetInt(&classobj->instanceFormat, instFormat);
    SetInt(&classobj->instanceFlags, instFlags);
    SetInt(&classobj->classIndex, -1);
}

void fixClassArrays(PyrClass* classobj);
void fixClassArrays(PyrClass* classobj) {
    if (IsObj(&classobj->methods))
        slotRawObject(&classobj->methods)->classptr = class_array;
    if (IsObj(&classobj->instVarNames))
        slotRawObject(&classobj->instVarNames)->classptr = class_symbolarray;
    if (IsObj(&classobj->classVarNames))
        slotRawObject(&classobj->classVarNames)->classptr = class_symbolarray;
    if (IsObj(&classobj->iprototype))
        slotRawObject(&classobj->iprototype)->classptr = class_array;
    if (IsObj(&classobj->cprototype))
        slotRawObject(&classobj->cprototype)->classptr = class_array;
}

int numInstVars(PyrClass* classobj) {
    int res;
    if (IsNil(&classobj->instVarNames))
        res = 0;
    else
        res = slotRawObject(&classobj->instVarNames)->size;
    return res;
}

int numClassVars(PyrClass* classobj) {
    int res;
    if (IsNil(&classobj->classVarNames))
        res = 0;
    else
        res = slotRawObject(&classobj->classVarNames)->size;
    return res;
}


void objAddIndexedSlotGrow(PyrSlot* arraySlot, PyrSlot* addSlot);
void objAddIndexedSlotGrow(PyrSlot* arraySlot, PyrSlot* addSlot) {
    PyrObject* obj;
    if (IsNil(arraySlot)) {
        PyrObject* newobj = (PyrObject*)newPyrArray(NULL, 1, obj_permanent | obj_immutable, false);
        SetObject(arraySlot, newobj);
        obj = newobj;
    } else {
        obj = slotRawObject(arraySlot);
        if (obj->size >= ARRAYMAXINDEXSIZE(obj)) {
            // post("objAddIndexedSlotGrow\n");
            PyrObject* newobj = (PyrObject*)newPyrArray(NULL, obj->size * 2, obj_permanent | obj_immutable, false);
            memcpy(newobj->slots, obj->slots, obj->size * sizeof(PyrSlot));
            newobj->size = obj->size;
            SetObject(arraySlot, newobj);
            pyr_pool_runtime->Free((void*)obj);
            obj = newobj;
        }
    }
    slotCopy(&obj->slots[obj->size++], addSlot);
}

void addMethod(PyrClass* classobj, PyrMethod* method) {
    PyrSlot slot;
    SetObject(&slot, method);
    objAddIndexedSlotGrow(&classobj->methods, &slot);
}


PyrMethod* classFindDirectMethod(PyrClass* classobj, PyrSymbol* name) {
    PyrMethod* method;
    PyrSlot* methods;
    int i, numMethods;
    if (IsNil(&classobj->methods))
        return NULL;
    methods = slotRawObject(&classobj->methods)->slots;
    numMethods = slotRawObject(&classobj->methods)->size;
    for (i = 0; i < numMethods; ++i) {
        method = slotRawMethod(&methods[i]);
        if (slotRawSymbol(&method->name) == name)
            break;
    }
    if (i >= numMethods)
        method = NULL;
    return method;
}

int numSuperInstVars(PyrClass* superclassobj) {
    int superinstvars = 0;
    if (superclassobj) {
        if (IsObj(&superclassobj->iprototype)) {
            superinstvars = slotRawObject(&superclassobj->iprototype)->size;
        }
    }
    return superinstvars;
}

bool classFindInstVar(PyrClass* classobj, PyrSymbol* name, int* index) {
    PyrSymbolArray* ivnames;
    PyrSymbol* ivname;
    int i;
    if (NotNil(&classobj->instVarNames)) {
        ivnames = slotRawSymbolArray(&classobj->instVarNames);
        if (ivnames) {
            for (i = 0; i < ivnames->size; ++i) {
                ivname = ivnames->symbols[i];
                if (ivname == name) {
                    // numsupervars = numSuperInstVars(slotRawSymbol(&classobj->superclass)->u.classobj);
                    //*index = numsupervars + i;
                    *index = i;
                    return true;
                }
            }
        }
    }
    return false;
}

int instVarOffset(const char* classname, const char* instvarname) {
    PyrSymbol *instvarsymbol, *classsymbol;
    PyrClass* classobj;
    int index;
    char c;

    classsymbol = getsym(classname);
    instvarsymbol = getsym(instvarname);

    c = classname[0];
    if (!(c >= 'A' && c <= 'Z'))
        return -1;

    classobj = classsymbol->u.classobj;
    if (!classobj)
        return -1;
    if (!classFindInstVar(classobj, instvarsymbol, &index))
        return -1;
    return index;
}

int classVarOffset(const char* classname, const char* classvarname, PyrClass** classobj) {
    PyrSymbol *classvarsymbol, *classsymbol;
    int index;
    char c;

    classsymbol = getsym(classname);
    classvarsymbol = getsym(classvarname);

    c = classname[0];
    if (!(c >= 'A' && c <= 'Z'))
        return -1;

    *classobj = classsymbol->u.classobj;
    if (!*classobj)
        return -1;
    if (!classFindClassVar(classobj, classvarsymbol, &index))
        return -1;
    return index;
}

bool classFindClassVar(PyrClass** classobj, PyrSymbol* name, int* index) {
    PyrSymbolArray* cvnames;
    PyrSymbol* cvname;
    int i, j;
    char* classname;
    PyrClass* localclassobj = *classobj;
    // if this is a Metaclass then we need to search its normal Class for
    // the class vars
    classname = slotRawSymbol(&localclassobj->name)->name;
    if (strncmp(classname, "Meta_", 5) == 0) {
        localclassobj = getsym(classname + 5)->u.classobj;
    }
    for (j = 0; localclassobj; ++j) {
        if (NotNil(&localclassobj->classVarNames)) {
            cvnames = slotRawSymbolArray(&localclassobj->classVarNames);
            if (cvnames) {
                for (i = 0; i < cvnames->size; ++i) {
                    cvname = cvnames->symbols[i];
                    if (cvname == name) {
                        *classobj = localclassobj;
                        *index = i;
                        return true;
                    }
                }
            }
        }
        if (IsSym(&localclassobj->superclass)) {
            localclassobj = slotRawSymbol(&localclassobj->superclass)->u.classobj;
        } else {
            localclassobj = NULL;
        }
    }
    return false;
}

bool classFindConst(PyrClass** classobj, PyrSymbol* name, int* index) {
    PyrSymbolArray* knames;
    PyrSymbol* kname;
    int i, j;
    char* classname;
    PyrClass* localclassobj = *classobj;
    // if this is a Metaclass then we need to search its normal Class for
    // the class vars
    classname = slotRawSymbol(&localclassobj->name)->name;
    if (strncmp(classname, "Meta_", 5) == 0) {
        localclassobj = getsym(classname + 5)->u.classobj;
    }
    for (j = 0; localclassobj; ++j) {
        if (NotNil(&localclassobj->constNames)) {
            knames = slotRawSymbolArray(&localclassobj->constNames);
            if (knames) {
                for (i = 0; i < knames->size; ++i) {
                    kname = knames->symbols[i];
                    if (kname == name) {
                        *classobj = localclassobj;
                        *index = i;
                        return true;
                    }
                }
            }
        }
        if (IsSym(&localclassobj->superclass)) {
            localclassobj = slotRawSymbol(&localclassobj->superclass)->u.classobj;
        } else {
            localclassobj = NULL;
        }
    }
    return false;
}

struct compareByName {
    bool operator()(PyrClass* lhs, PyrClass* rhs) const {
        return strcmp(slotRawSymbol(&lhs->name)->name, slotRawSymbol(&rhs->name)->name) < 0;
    }
};


template <class T> class pyr_pool_compile_allocator {
public:
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    template <class U> struct rebind { typedef pyr_pool_compile_allocator<U> other; };

    pyr_pool_compile_allocator(void) {}

    template <class U> pyr_pool_compile_allocator(pyr_pool_compile_allocator<U> const&) {}

    pointer address(reference x) const { return &x; }

    const_pointer address(const_reference x) const { return &x; }

    pointer allocate(size_type n, const void* hint = 0) { return (pointer)pyr_pool_compile->Alloc(n * sizeof(T)); }

    void deallocate(pointer p, size_type n) { pyr_pool_compile->Free(p); }

    void construct(pointer p, const T& val) { ::new (p) T(val); }

    void destroy(pointer p) { p->~T(); }
};


/* sort list of classes:
 * we fill a binary search tree
 *
 */
static PyrClass* sortClasses(PyrClass* aClassList) {
    typedef std::set<PyrClass*, compareByName, pyr_pool_compile_allocator<PyrClass*>> classSetType;
    classSetType classSet;

    PyrClass* insertHead = aClassList;
    do {
        assert(classSet.find(insertHead) == classSet.end());
        classSet.insert(insertHead);
        insertHead = slotRawClass(&insertHead->nextclass);
    } while (insertHead);

    classSetType::iterator it = classSet.begin();
    PyrClass* sortedClasses = *it;
    ++it;

    PyrClass* lastClass = sortedClasses;
    for (; it != classSet.end(); ++it) {
        PyrClass* current = *it;
        SetObject(&lastClass->nextclass, (PyrObject*)current);
        lastClass = current;
    }
    SetNil(&lastClass->nextclass);
    return sortedClasses;
}


void buildClassTree() {
    // after all classes are compiled this function builds the tree and
    // indexes the classes

    // count subclasses
    // postfl("count subclasses\n");
    PyrClass* classobj = gClassList;
    while (classobj) {
        PyrClass* superclassobj = slotRawSymbol(&classobj->superclass)->u.classobj;
        if (superclassobj) {
            // postfl("     superclassobj %s\n", slotRawSymbol(&superclassobj->name)->name);
            SetRaw(&superclassobj->subclasses, slotRawInt(&superclassobj->subclasses) + 1);
        }
        classobj = slotRawClass(&classobj->nextclass);
    }
    // allocate subclass arrays
    // postfl("allocate subclass arrays\n");
    classobj = gClassList;
    while (classobj) {
        int numSubclasses;
        numSubclasses = slotRawInt(&classobj->subclasses);
        // postfl("  %s %d\n", slotRawSymbol(&classobj->name)->name, numSubclasses);
        if (numSubclasses) {
            SetObject(&classobj->subclasses,
                      (PyrObject*)newPyrArray(NULL, numSubclasses, obj_permanent | obj_immutable, false));
            slotRawObject(&classobj->subclasses)->size = 0;
        } else {
            SetNil(&classobj->subclasses);
        }
        classobj = slotRawClass(&classobj->nextclass);
    }
    // fill in subclass arrays
    // postfl("fill in subclass arrays\n");
    classobj = gClassList;
    while (classobj) {
        // postfl("  %s\n", slotRawSymbol(&classobj->name)->name);
        PyrClass* superclassobj = slotRawSymbol(&classobj->superclass)->u.classobj;
        if (superclassobj) {
            objAddIndexedObject(slotRawObject(&superclassobj->subclasses), (PyrObject*)classobj);
            // postfl("     superclassobj %s %d\n", slotRawSymbol(&superclassobj->name)->name,
            //	slotRawObject(&superclassobj->subclasses)->size);
        }
        classobj = slotRawClass(&classobj->nextclass);
    }

    // alpha sort the classes via insertion sort
    gClassList = sortClasses(gClassList);
}

void indexClassTree(PyrClass* classobj, int numSuperMethods) {
    int i, numMethods;

    if (!classobj)
        return;

    SetInt(&classobj->classIndex, gNumClasses);
    gNumClasses++;

    if (IsObj(&classobj->methods)) {
        PyrObject* methods = slotRawObject(&classobj->methods);
        numMethods = methods->size;
    } else
        numMethods = 0;

    numMethods = numSuperMethods + numMethods;
    if (IsObj(&classobj->subclasses)) {
        PyrObject* subclasses = slotRawObject(&classobj->subclasses);
        for (i = 0; i < subclasses->size; ++i)
            indexClassTree(slotRawClass(&subclasses->slots[i]), numMethods);
    }
    SetInt(&classobj->maxSubclassIndex, gNumClasses - 1);
}

void findDiscrepancy();
void findDiscrepancy() {
    PyrClass *classobjA, *classobjB;

    classobjA = gClassList;
    while (classobjA) {
        classobjB = slotRawClass(&classobjA->nextclass);
        ;
        while (classobjB) {
            if (slotRawSymbol(&classobjA->name) == slotRawSymbol(&classobjB->name)) {
                post("duplicate %s\n", slotRawSymbol(&classobjA->name)->name);
            }
            classobjB = slotRawClass(&classobjB->nextclass);
        }
        classobjA = slotRawClass(&classobjA->nextclass);
    }
}


static void indent(int n) {
    for (int i = 0; i < n; ++i) {
        post("  ");
    }
}

void postClassTree(PyrClass* classobj, int level) {
    PyrObject* subclasses;
    int i;

    // post("%4d  ", slotRawInt(&classobj->classIndex));
    indent(level);
    post("%s\n", slotRawSymbol(&classobj->name)->name);

    if (classobj == class_class) {
        indent(level + 1);
        post("      [.. all metaclasses ..]\n");
    } else {
        if (IsNil(&classobj->subclasses))
            return; // FIXME: can we initialize subclasses with a NULL pointer?
        subclasses = slotRawObject(&classobj->subclasses);
        if (subclasses) {
            // determine if can put on one line
            bool oneline = subclasses->size <= 5;
            for (i = 0; oneline && i < subclasses->size; ++i) {
                PyrClass* subclassobj = slotRawClass(&subclasses->slots[i]);
                if (IsObj(&subclassobj->subclasses))
                    // FIXME: shall we do a null-pointer check?
                    oneline = false;
            }
            if (oneline) {
                indent(level + 1);
                post("[");
                for (i = 0; i < subclasses->size; ++i) {
                    PyrClass* subclassobj = slotRawClass(&subclasses->slots[i]);
                    post(" %s", slotRawSymbol(&subclassobj->name)->name);
                }
                post(" ]\n");
            } else {
                indent(level);
                post("[\n");
                for (i = 0; i < subclasses->size; ++i) {
                    postClassTree(slotRawClass(&subclasses->slots[i]), level + 1);
                }
                indent(level);
                post("]\n");
            }
        }
    }
}


void setSelectorFlags() {
    int i;

    PyrClass* classobj = gClassList;
    while (classobj) {
        if (IsObj(&classobj->methods)) {
            PyrObject* methods = slotRawObject(&classobj->methods);
            for (i = 0; i < methods->size; ++i) {
                PyrMethod* method = slotRawMethod(&methods->slots[i]);
                slotRawSymbol(&method->name)->flags |= sym_Selector;
                // if (method->methType == methRedirect) {
                //	post("rd: %3d %s:%s\n", k++, slotRawSymbol(&classobj->name)->name,
                //		slotRawSymbol(&method->name)->name);
                //}
            }
        }
        classobj = slotRawClass(&classobj->nextclass);
    }
    // count selectors
    gNumSelectors = 0;
    SymbolTable* symbolTable = gMainVMGlobals->symbolTable;
    for (int i = 0; i < symbolTable->TableSize(); ++i) {
        PyrSymbol* sym = symbolTable->Get(i);
        if (sym && (sym->flags & sym_Selector)) {
            sym->u.index = gNumSelectors++;
        }
    }
    // post("gNumSelectors %d\n", gNumSelectors);
}

// the chunky stuff can be commented back in for implementing a better
// compression scheme. The Q&D method works fine for my small class tree for now.

typedef struct {
    PyrSymbol* selector;
    int minClassIndex;
    int maxClassIndex;
    int rowWidth;
    // int largestChunk;
    // int chunkOffset;
    int selectorIndex;
    int population;
    int rowOffset;
} ColumnDescriptor;

int compareColDescs(const void* va, const void* vb);
int compareColDescs(const void* va, const void* vb) {
    ColumnDescriptor* a = (ColumnDescriptor*)va;
    ColumnDescriptor* b = (ColumnDescriptor*)vb;
    int diff;
    // diff = b->largestChunk - a->largestChunk;
    // if (diff != 0) return diff;
    diff = b->rowWidth - a->rowWidth;
    if (diff != 0)
        return diff;
    // diff = b->chunkOffset - a->chunkOffset;
    diff = b->minClassIndex - a->minClassIndex;
    return diff;
}

#define CHECK_METHOD_LOOKUP_TABLE_BUILD_TIME 0
#if CHECK_METHOD_LOOKUP_TABLE_BUILD_TIME
double elapsedTime();
#endif

#ifdef _MSC_VER
static size_t fillClassRows(const PyrClass* classobj, PyrMethod** bigTable);
#else
static size_t fillClassRows(const PyrClass* classobj, PyrMethod** bigTable, boost::basic_thread_pool& pool);
#endif


static void binsortClassRows(PyrMethod const** bigTable, const ColumnDescriptor* sels, size_t numSelectors,
                             size_t begin, size_t end) {
    // bin sort the class rows to the new ordering
    // post("reorder rows\n");
    const int allocaThreshold = 16384;
    PyrMethod** temprow = (numSelectors < allocaThreshold) ? (PyrMethod**)alloca(numSelectors * sizeof(PyrMethod*))
                                                           : (PyrMethod**)malloc(numSelectors * sizeof(PyrMethod*));

    for (int j = begin; j < end; ++j) {
        PyrMethod const** row = bigTable + j * numSelectors;
        memcpy(temprow, row, numSelectors * sizeof(PyrMethod*));

#pragma GCC ivdep
        for (int i = 0; i < numSelectors; ++i)
            row[i] = temprow[sels[i].selectorIndex];
    }

    if (numSelectors >= allocaThreshold)
        free(temprow);
}

static ColumnDescriptor* prepareColumnTable(ColumnDescriptor* sels, int numSelectors) {
    // fill selector table
    // post("fill selector table\n");
    SymbolTable* symbolTable = gMainVMGlobals->symbolTable;

    int selectorTableIndex = 0;
    for (int i : boost::irange(0, symbolTable->TableSize())) {
        PyrSymbol* sym = symbolTable->Get(i);
        if (sym && (sym->flags & sym_Selector))
            sels[selectorTableIndex++].selector = sym;
    }
    assert(selectorTableIndex == numSelectors);

    for (int i = 0; i < numSelectors; ++i) {
        // postfl("%3d %s\n", i, sels[i].selector->name);
        sels[i].minClassIndex = INT_MAX;
        sels[i].maxClassIndex = 0;
        // sels[i].largestChunk = 0;
        // sels[i].chunkOffset = 0;
        sels[i].selectorIndex = i;
        sels[i].population = 0;
    }
    return sels;
}


static void calcRowStats(PyrMethod const* const* bigTable, ColumnDescriptor* sels, int numClasses, int numSelectors,
                         int begin, int end) {
    // chunkSize = 0;
    // chunkOffset = 0;
    for (int classIndex = 0; classIndex < numClasses; ++classIndex) {
        for (int selectorIndex = begin; selectorIndex < end; ++selectorIndex) {
            PyrMethod const* method = bigTable[classIndex * numSelectors + selectorIndex];
            if (method) {
                // classobj = method->ownerclass.uoc;
                if (classIndex > sels[selectorIndex].maxClassIndex)
                    sels[selectorIndex].maxClassIndex = classIndex;

                if (classIndex < sels[selectorIndex].minClassIndex)
                    sels[selectorIndex].minClassIndex = classIndex;

                sels[selectorIndex].population++;
                // if (chunkSize == 0) chunkOffset = j;
                // chunkSize++;
                // postfl("    %8s %3d %3d %3d %3d\n", slotRawSymbol(&classobj->name)->name, i, j,
                //	chunkSize, slotRawInt(&classobj->classIndex));
                //} else {
                // if (chunkSize > sels[i].largestChunk) {
                //	sels[i].largestChunk = chunkSize;
                //	sels[i].chunkOffset = chunkOffset;
                //}
                // chunkSize = 0;
            }
        }
    }

    for (int i = begin; i < end; ++i)
        sels[i].rowWidth = sels[i].maxClassIndex - sels[i].minClassIndex + 1;
}

void buildBigMethodMatrix() {
    PyrMethod **bigTable, **row;
    PyrClass** classes;
    int j, k;
    int popSum, widthSum;
    int rowOffset, freeIndex;
    int rowTableSize;
    int bigTableSize;
    const size_t numSelectors = gNumSelectors;
    const size_t numClasses = gNumClasses;
    // post("allocate arrays\n");

#if CHECK_METHOD_LOOKUP_TABLE_BUILD_TIME
    double t0 = elapsedTime();
#endif

    const int hw_concurrency = SC_Thread::hardware_concurrency();
    const int cpuCount = hw_concurrency > 0 ? hw_concurrency : 1;
    const int helperThreadCount = cpuCount > 1 ? cpuCount - 1 : 1;
    boost::basic_thread_pool pool(helperThreadCount);

    // pyrmalloc:
    // lifetime: kill after compile
    bigTableSize = numSelectors * numClasses;
    // post("bigTableSize %d %d %d\n", bigTableSize, numSelectors, numClasses);
    ColumnDescriptor* sels = (ColumnDescriptor*)pyr_pool_compile->Alloc(numSelectors * sizeof(ColumnDescriptor));
    MEMFAIL(sels);
#ifdef _MSC_VER
    auto filledSelectorsFuture = std::async(std::launch::deferred, std::bind(&prepareColumnTable, sels, numSelectors));
#else
    auto filledSelectorsFuture = boost::async(pool, std::bind(&prepareColumnTable, sels, numSelectors));
#endif

    classes = (PyrClass**)pyr_pool_compile->Alloc(numClasses * sizeof(PyrClass*));
    MEMFAIL(classes);

    auto fillClassIndices = [](PyrClass** classes) {
        PyrClass* classobj = gClassList;
        while (classobj) {
            classes[slotRawInt(&classobj->classIndex)] = classobj;
            classobj = slotRawClass(&classobj->nextclass);
        }
        return classes;
    };
#ifdef _MSC_VER
    auto filledClassIndices = std::async(std::launch::deferred, fillClassIndices, classes);
#else
    auto filledClassIndices = boost::async(pool, fillClassIndices, classes);
#endif

    bigTable = (PyrMethod**)pyr_pool_compile->Alloc(bigTableSize * sizeof(PyrMethod*));
    MEMFAIL(bigTable);

#ifndef _MSC_VER
    pool.try_executing_one();
#endif
    filledClassIndices.wait();
#ifdef _MSC_VER
    size_t numentries = fillClassRows(class_object, bigTable);
#else
    size_t numentries = fillClassRows(class_object, bigTable, pool);
#endif
    post("\tnumentries = %lu / %d = %.2g\n", numentries, bigTableSize, (double)numentries / (double)bigTableSize);


    ColumnDescriptor* filledSelectors = filledSelectorsFuture.get();
#ifdef _MSC_VER
    std::vector<std::future<void>> columnDescriptorsWithStats;
#else
    std::vector<boost::future<void>> columnDescriptorsWithStats;
#endif
    size_t selectorsPerJob = numSelectors / cpuCount / 2;
    for (size_t beginSelectorIndex : boost::irange(selectorsPerJob, numSelectors, selectorsPerJob)) {
        size_t endSelectorIndex = std::min(beginSelectorIndex + selectorsPerJob, numSelectors);
#ifdef _MSC_VER
        auto future = std::async(std::launch::deferred, calcRowStats, bigTable, filledSelectors, numClasses,
                                 numSelectors, beginSelectorIndex, endSelectorIndex);
#else
        auto future = boost::async(pool, calcRowStats, bigTable, filledSelectors, numClasses, numSelectors,
                                   beginSelectorIndex, endSelectorIndex);
#endif
        columnDescriptorsWithStats.push_back(std::move(future));
    }

    calcRowStats(bigTable, filledSelectors, numClasses, numSelectors, 0, std::min(selectorsPerJob, numSelectors));

    for (auto& future : columnDescriptorsWithStats) {
#ifdef _MSC_VER
        future.wait();
#else
        while (!future.is_ready())
            pool.schedule_one_or_yield();
#endif
    }

    // post("qsort\n");
    // sort rows by largest chunk, then by width, then by chunk offset

#if 0 // not yet
	__gnu_parallel::sort(sels, sels + numSelectors, [](ColumnDescriptor const & rhs, ColumnDescriptor const & lhs) {
		return compareColDescs(&rhs, &lhs) < 0;
	});
#else
    std::sort(sels, sels + numSelectors,
              [](ColumnDescriptor const& rhs, ColumnDescriptor const& lhs) { return compareColDescs(&rhs, &lhs) < 0; });
#endif

    // bin sort the class rows to the new ordering
    // post("reorder rows\n");
#ifdef _MSC_VER
    std::vector<std::future<void>> binsortedClassRowFuture;
#else
    std::vector<boost::future<void>> binsortedClassRowFuture;
#endif
    size_t classesPerJob = numClasses / cpuCount / 2;
    for (size_t beginClassIndex : boost::irange(classesPerJob, numClasses, classesPerJob)) {
        size_t endClassIndex = std::min(beginClassIndex + classesPerJob, numClasses);
#ifdef _MSC_VER
        auto future = std::async(std::launch::deferred, binsortClassRows, (PyrMethod const**)bigTable, sels,
                                 numSelectors, beginClassIndex, endClassIndex);
#else
        auto future = boost::async(pool, binsortClassRows, (PyrMethod const**)bigTable, sels, numSelectors,
                                   beginClassIndex, endClassIndex);
#endif
        binsortedClassRowFuture.push_back(std::move(future));
    }

    binsortClassRows((PyrMethod const**)bigTable, sels, numSelectors, 0, std::min(classesPerJob, numClasses));

    for (auto& future : binsortedClassRowFuture) {
#ifdef _MSC_VER
        future.wait();
#else
        while (!future.is_ready())
            pool.schedule_one_or_yield();
#endif
    }

    // post("calc row offsets %d\n", numSelectors);
    widthSum = 0;
    popSum = 0;
    freeIndex = 0;
    rowOffset = -1;
    for (int i = 0; i < numSelectors; ++i) {
        widthSum += sels[i].rowWidth;
        popSum += sels[i].population;
        rowOffset = sc_max(rowOffset + 1, freeIndex - sels[i].minClassIndex);
        freeIndex = rowOffset + sels[i].maxClassIndex + 1;
        sels[i].rowOffset = rowOffset;
        sels[i].selector->u.index = rowOffset;
        // post("%3d  %24s %3d %5d %5d\n", i, sels[i].selector->name,
        //	sels[i].rowWidth, rowOffset, freeIndex);
    }
    // post("alloc row table %d\n", freeIndex);
    rowTableSize = (freeIndex + numClasses) * sizeof(PyrMethod*);
    gRowTable = (PyrMethod**)pyr_pool_runtime->Alloc(rowTableSize);
    MEMFAIL(gRowTable);

    // having the method ptr always be valid saves a branch in SendMessage()
    for (int i = 0; i < freeIndex + numClasses; ++i)
        gRowTable[i] = gNullMethod;

    // post("fill compressed table\n");
    //{ FILE* fp;
    // newPyrMethod
    // fp = fopen("meth table", "w");
    for (int i = 0; i < numSelectors; ++i) {
        int offset, maxwidth;
        offset = sels[i].rowOffset + sels[i].minClassIndex;
        maxwidth = offset + sels[i].rowWidth;
        row = bigTable + sels[i].minClassIndex * numSelectors + i;
        PyrMethod** table = gRowTable;
        for (j = offset, k = 0; j < maxwidth; ++j, k += numSelectors) {
            if (row[k])
                table[j] = row[k];
        }
    }
    // fclose(fp);
    //}

    for (int i = 0; i < freeIndex + numClasses; ++i)
        assert(gRowTable[i]);


        // post("freeIndex %d\n", freeIndex);
        // post("widthSum %d\n", widthSum);
        // post("popSum %d\n", popSum);

#if CHECK_METHOD_LOOKUP_TABLE_BUILD_TIME
    post("building table took %.3g seconds\n", elapsedTime() - t0);
    {
        int numFilled = 0;
        for (int i = 0; i < rowTableSize / sizeof(PyrMethod*); ++i) {
            if (gRowTable[i] != gNullMethod)
                numFilled++;
        }
        post("Filled %d of %d = %f percent\n", numFilled, rowTableSize / sizeof(PyrMethod*),
             100. * (double)numFilled / (rowTableSize / sizeof(PyrMethod*)));
    }
#endif
    post("\t%d method selectors, %d classes\n", numSelectors, numClasses);
    post("\tmethod table size %d bytes, ", rowTableSize);
    post("big table size %d\n", numSelectors * numClasses * sizeof(PyrMethod*));
    // postfl("%p %p %p\n", classes, bigTable, sels);
    /*
        // not necessary since the entire pool will be freed..
        pyr_pool_compile->Free(classes);
        pyr_pool_compile->Free(bigTable);
        pyr_pool_compile->Free(sels);
    */
}

#ifdef _MSC_VER
static size_t fillClassRow(const PyrClass* classobj, PyrMethod** bigTable)
#else
static size_t fillClassRow(const PyrClass* classobj, PyrMethod** bigTable, boost::basic_thread_pool& pool)
#endif
{
    size_t count = 0;

    PyrMethod** myrow = bigTable + slotRawInt(&classobj->classIndex) * gNumSelectors;
    PyrClass* superclassobj = slotRawSymbol(&classobj->superclass)->u.classobj;
    if (superclassobj) {
        PyrMethod** superrow = bigTable + slotRawInt(&superclassobj->classIndex) * gNumSelectors;

#pragma GCC ivdep
        for (int i = 0; i != gNumSelectors; ++i) {
            myrow[i] = superrow[i];
            if (superrow[i])
                ++count;
        }

    } else {
        memset(myrow, 0, gNumSelectors * sizeof(PyrMethod*));
    }

    if (IsObj(&classobj->methods)) {
        PyrObject* methods = const_cast<PyrObject*>(slotRawObject(&classobj->methods));
        // postfl("        %d\n", methods->size);
        for (int i = 0; i < methods->size; ++i) {
            PyrMethod* method = slotRawMethod(&methods->slots[i]);
            int selectorIndex = slotRawSymbol(&method->name)->u.index;

            if (myrow[selectorIndex] == 0)
                ++count;

            myrow[selectorIndex] = method;
        }
    }

    size_t result = count;

    if (IsObj(&classobj->subclasses)) {
        const PyrObject* subclasses = slotRawObject(&classobj->subclasses);
        int numSubclasses = subclasses->size;

        if (numSubclasses) {
#ifdef _MSC_VER
            if (numSubclasses <= 2) {
                for (int subClassIndex : boost::irange(0, numSubclasses))
                    result += fillClassRow(slotRawClass(&subclasses->slots[subClassIndex]), bigTable);
            } else {
                typedef std::vector<std::future<size_t>> VectorOfFutures;

                VectorOfFutures subclassResults;
                for (int subClassIndex : boost::irange(1, numSubclasses)) {
                    auto subclassResult = std::async(std::launch::deferred, fillClassRow,
                                                     slotRawClass(&subclasses->slots[subClassIndex]), bigTable);
                    subclassResults.emplace_back(std::move(subclassResult));
                }

                result += fillClassRow(slotRawClass(&subclasses->slots[0]), bigTable);

                for (auto& subclassResult : subclassResults) {
                    result += subclassResult.get();
                }
            }
#else
            if (numSubclasses <= 2) {
                for (int subClassIndex : boost::irange(0, numSubclasses))
                    result += fillClassRow(slotRawClass(&subclasses->slots[subClassIndex]), bigTable, pool);
            } else {
                typedef std::vector<boost::future<size_t>> VectorOfFutures;

                VectorOfFutures subclassResults;
                for (int subClassIndex : boost::irange(1, numSubclasses)) {
                    auto subclassResult =
                        boost::async(pool, fillClassRow, slotRawClass(&subclasses->slots[subClassIndex]), bigTable,
                                     boost::ref(pool));
                    subclassResults.emplace_back(std::move(subclassResult));
                }

                result += fillClassRow(slotRawClass(&subclasses->slots[0]), bigTable, pool);

                for (auto& subclassResult : subclassResults) {
                    while (!subclassResult.is_ready())
                        pool.schedule_one_or_yield();

                    result += subclassResult.get();
                }
            }
#endif
        }
    }

    return result;
}

#ifdef _MSC_VER
static size_t fillClassRows(const PyrClass* classobj, PyrMethod** bigTable) { return fillClassRow(classobj, bigTable); }
#else
static size_t fillClassRows(const PyrClass* classobj, PyrMethod** bigTable, boost::basic_thread_pool& pool) {
    return fillClassRow(classobj, bigTable, pool);
}
#endif

bool funcFindArg(PyrBlock* func, PyrSymbol* name, int* index) {
    int i;
    for (i = 0; i < slotRawSymbolArray(&func->argNames)->size; ++i) {
        if (slotRawSymbolArray(&func->argNames)->symbols[i] == name) {
            *index = i;
            return true;
        }
    }
    return false;
}

bool funcFindVar(PyrBlock* func, PyrSymbol* name, int* index) {
    int i;
    for (i = 0; i < slotRawSymbolArray(&func->varNames)->size; ++i) {
        if (slotRawSymbolArray(&func->varNames)->symbols[i] == name) {
            *index = i;
            return true;
        }
    }
    return false;
}

PyrClass* makeIntrinsicClass(PyrSymbol* className, PyrSymbol* superClassName, int numInstVars, int numClassVars) {
    PyrClass* superClass = NULL;
    PyrClass* metaSuperClass = NULL;
    PyrSymbol* metaClassName = NULL;
    PyrSymbol* metaSuperClassName = NULL;
    PyrClass* classobj = NULL;
    PyrClass* metaclassobj = NULL;
    int superInstVars;

    // postfl("makeIntrinsicClass '%s'\n", className->name);
    if (superClassName) {
        superClass = superClassName->u.classobj;
        if (!superClass) {
            error("Can't find superclass '%s' of '%s'\n", superClassName->name, className->name);
            return NULL;
        }
        metaSuperClassName = getmetasym(superClassName->name);
        metaSuperClass = metaSuperClassName->u.classobj;
        superInstVars = numSuperInstVars(superClass);
    } else {
        // else it must be Object and so has no superclass
        metaSuperClassName = NULL;
        superInstVars = 0;
    }

    metaClassName = getmetasym(className->name);
    metaClassName->flags |= sym_MetaClass;
    metaclassobj =
        newClassObj(class_class, metaClassName, metaSuperClassName, classClassNumInstVars, 0, 0, 0, obj_notindexed, 0);
    SetInt(&metaclassobj->classFlags, slotRawInt(&metaclassobj->classFlags) | classIsIntrinsic);

    if (metaSuperClassName && classClassNumInstVars) {
        memcpy(slotRawObject(&metaclassobj->iprototype)->slots, slotRawObject(&metaSuperClass->iprototype)->slots,
               sizeof(PyrSlot) * classClassNumInstVars);
        memcpy(slotRawSymbolArray(&metaclassobj->instVarNames)->symbols,
               slotRawSymbolArray(&metaSuperClass->instVarNames)->symbols, sizeof(PyrSymbol*) * classClassNumInstVars);
        slotRawObject(&metaclassobj->iprototype)->size = classClassNumInstVars;
        slotRawObject(&metaclassobj->instVarNames)->size = classClassNumInstVars;
        // dumpObject((PyrObject*)metaclassobj);
    }

    classobj = newClassObj(metaclassobj, className, superClassName, numInstVars + superInstVars, numClassVars, 0, 0,
                           obj_notindexed, 0);
    SetInt(&classobj->classFlags, slotRawInt(&classobj->classFlags) | classIsIntrinsic);

    // postfl("%s:%s  : %d\n", className->name, superClassName->name, superInstVars);
    if (superClass && superInstVars) {
        memcpy(slotRawObject(&classobj->iprototype)->slots, slotRawObject(&superClass->iprototype)->slots,
               sizeof(PyrSlot) * superInstVars);
        memcpy(slotRawSymbolArray(&classobj->instVarNames)->symbols,
               slotRawSymbolArray(&superClass->instVarNames)->symbols, sizeof(PyrSymbol*) * superInstVars);
        slotRawObject(&classobj->iprototype)->size = superInstVars;
        slotRawObject(&classobj->instVarNames)->size = superInstVars;
    }

    return classobj;
}

void addIntrinsicVar(PyrClass* classobj, const char* varName, PyrSlot* slot) {
    // postfl("%s  %s  %d\n", slotRawSymbol(&classobj->name)->name, varName,
    //	slotRawObject(&classobj->instVarNames)->size);
    objAddIndexedSymbol(slotRawSymbolArray(&classobj->instVarNames), getsym(varName));
    objAddIndexedSlot(slotRawObject(&classobj->iprototype), slot);
}

void addIntrinsicClassVar(PyrClass* classobj, const char* varName, PyrSlot* slot);
void addIntrinsicClassVar(PyrClass* classobj, const char* varName, PyrSlot* slot) {
    // postfl("%s  %s  %d\n", slotRawSymbol(&classobj->name)->name, varName,
    //	slotRawObject(&classobj->instVarNames)->size);
    objAddIndexedSymbol(slotRawSymbolArray(&classobj->classVarNames), getsym(varName));
    objAddIndexedSlot(slotRawObject(&classobj->cprototype), slot);
}

void initClasses() {
    PyrClass* class_object_meta;
    PyrMethodRaw* methraw;

    // BOOTSTRAP THE OBJECT HIERARCHY

    gNumClassVars = 0;
    gClassList = NULL;
    gNullMethod = newPyrMethod();
    SetSymbol(&gNullMethod->name, (PyrSymbol*)NULL);
    methraw = METHRAW(gNullMethod);
    methraw->methType = methNormal;

    // build intrinsic classes
    class_class = NULL;
    class_object = makeIntrinsicClass(s_object, 0, 0, 4);
    class_class = makeIntrinsicClass(s_class, s_object, classClassNumInstVars, 1);

    // now fix class_class ptrs that were just previously installed erroneously
    class_object->classptr->classptr = class_class;
    class_class->classptr->classptr = class_class;
    class_object_meta = class_object->classptr;
    class_object_meta->superclass = class_class->name;

    addIntrinsicClassVar(class_object, "dependantsDictionary", &o_nil);
    addIntrinsicClassVar(class_object, "currentEnvironment", &o_nil);
    addIntrinsicClassVar(class_object, "topEnvironment", &o_nil);
    addIntrinsicClassVar(class_object, "uniqueMethods", &o_nil);

    // declare varNames for Class

    addIntrinsicVar(class_class, "name", &o_nil);
    addIntrinsicVar(class_class, "nextclass", &o_nil);
    addIntrinsicVar(class_class, "superclass", &o_nil);
    addIntrinsicVar(class_class, "subclasses", &o_nil);
    addIntrinsicVar(class_class, "methods", &o_nil);

    addIntrinsicVar(class_class, "instVarNames", &o_nil);
    addIntrinsicVar(class_class, "classVarNames", &o_nil);
    addIntrinsicVar(class_class, "iprototype", &o_nil);
    addIntrinsicVar(class_class, "cprototype", &o_nil);

    addIntrinsicVar(class_class, "constNames", &o_nil);
    addIntrinsicVar(class_class, "constValues", &o_nil);

    addIntrinsicVar(class_class, "instanceFormat", &o_nil);
    addIntrinsicVar(class_class, "instanceFlags", &o_zero);
    addIntrinsicVar(class_class, "classIndex", &o_zero);
    addIntrinsicVar(class_class, "classFlags", &o_zero);
    addIntrinsicVar(class_class, "maxSubclassIndex", &o_zero);
    addIntrinsicVar(class_class, "filenameSymbol", &o_nil);
    addIntrinsicVar(class_class, "charPos", &o_zero);
    addIntrinsicVar(class_class, "classVarIndex", &o_zero);

    addIntrinsicClassVar(class_class, "classesInited", &o_nil);

    // class_object_meta's inst var names need to be copied from class_class
    // because class_class didn't exist when it was created
    memcpy(slotRawObject(&class_object_meta->iprototype)->slots, slotRawObject(&class_class->iprototype)->slots,
           sizeof(PyrSlot) * classClassNumInstVars);
    memcpy(slotRawSymbolArray(&class_object_meta->instVarNames)->symbols,
           slotRawSymbolArray(&class_class->instVarNames)->symbols, sizeof(PyrSymbol*) * classClassNumInstVars);

    memcpy(slotRawObject(&class_class->classptr->iprototype)->slots, slotRawObject(&class_class->iprototype)->slots,
           sizeof(PyrSlot) * classClassNumInstVars);
    memcpy(slotRawSymbolArray(&class_class->classptr->instVarNames)->symbols,
           slotRawSymbolArray(&class_class->instVarNames)->symbols, sizeof(PyrSymbol*) * classClassNumInstVars);


    // OK the next thing I need is arrays..
    class_collection = makeIntrinsicClass(s_collection, s_object, 0, 0);
    class_sequenceable_collection = makeIntrinsicClass(s_sequenceable_collection, s_collection, 0, 0);

    class_arrayed_collection = makeIntrinsicClass(s_arrayed_collection, s_sequenceable_collection, 0, 0);
    class_array = makeIntrinsicClass(s_array, s_arrayed_collection, 0, 0);
    SetInt(&class_array->instanceFormat, obj_slot);
    SetInt(&class_array->classFlags, slotRawInt(&class_array->classFlags) | classHasIndexableInstances);

    // now fix array classptrs in already created classes
    fixClassArrays(class_class);
    fixClassArrays(class_class->classptr);
    fixClassArrays(class_object_meta);
    fixClassArrays(class_collection);
    fixClassArrays(class_sequenceable_collection);
    fixClassArrays(class_arrayed_collection);
    fixClassArrays(class_array);

    class_fundef = makeIntrinsicClass(s_fundef, s_object, 10, 0);
    // declare varNames for Block

    addIntrinsicVar(class_fundef, "raw1", &o_nil);
    addIntrinsicVar(class_fundef, "raw2", &o_nil);
    addIntrinsicVar(class_fundef, "code", &o_nil);
    addIntrinsicVar(class_fundef, "selectors", &o_nil);
    addIntrinsicVar(class_fundef, "constants", &o_nil);

    addIntrinsicVar(class_fundef, "prototypeFrame", &o_nil);
    addIntrinsicVar(class_fundef, "context", &o_nil);
    addIntrinsicVar(class_fundef, "argNames", &o_nil);
    addIntrinsicVar(class_fundef, "varNames", &o_nil);
    addIntrinsicVar(class_fundef, "sourceCode", &o_nil);

    class_method = makeIntrinsicClass(s_method, s_fundef, 5, 0);
    addIntrinsicVar(class_method, "ownerClass", &o_nil);
    addIntrinsicVar(class_method, "name", &o_nil);
    addIntrinsicVar(class_method, "primitiveName", &o_nil);
    addIntrinsicVar(class_method, "filenameSymbol", &o_nil);
    addIntrinsicVar(class_method, "charPos", &o_zero);
    // addIntrinsicVar(class_method, "byteMeter", &o_zero);
    // addIntrinsicVar(class_method, "callMeter", &o_zero);

    class_frame = makeIntrinsicClass(s_frame, s_object, 0, 0);
    SetInt(&class_frame->classFlags, slotRawInt(&class_frame->classFlags) | classHasIndexableInstances);
    // addIntrinsicVar(class_frame, "method", &o_nil);
    // addIntrinsicVar(class_frame, "caller", &o_nil);
    // addIntrinsicVar(class_frame, "context", &o_nil);
    // addIntrinsicVar(class_frame, "homeContext", &o_nil);
    // addIntrinsicVar(class_frame, "ip", &o_nil);

    class_process = makeIntrinsicClass(s_process, s_object, 6, 0);
    addIntrinsicVar(class_process, "classVars", &o_nil);
    addIntrinsicVar(class_process, "interpreter", &o_nil);
    addIntrinsicVar(class_process, "curThread", &o_nil);
    addIntrinsicVar(class_process, "mainThread", &o_nil);
    addIntrinsicVar(class_process, "schedulerQueue", &o_nil);
    addIntrinsicVar(class_process, "nowExecutingPath", &o_nil);

    class_interpreter = makeIntrinsicClass(s_interpreter, s_object, 29, 0);
    addIntrinsicVar(class_interpreter, "cmdLine", &o_nil);
    addIntrinsicVar(class_interpreter, "context", &o_nil);
    for (int i = 0; i < 26; ++i) {
        char name[2];
        name[0] = 'a' + i;
        name[1] = 0;
        addIntrinsicVar(class_interpreter, name, &o_nil);
    }
    addIntrinsicVar(class_interpreter, "codeDump", &o_nil);
    addIntrinsicVar(class_interpreter, "preProcessor", &o_nil);

    class_absfunc = makeIntrinsicClass(s_absfunc, s_object, 0, 0);
    class_stream = makeIntrinsicClass(s_stream, s_absfunc, 0, 0);

    class_thread = makeIntrinsicClass(s_thread, s_stream, 27, 0);
    addIntrinsicVar(class_thread, "state", &o_nil);
    addIntrinsicVar(class_thread, "func", &o_nil);
    addIntrinsicVar(class_thread, "stack", &o_nil);
    addIntrinsicVar(class_thread, "method", &o_nil);
    addIntrinsicVar(class_thread, "block", &o_nil);
    addIntrinsicVar(class_thread, "frame", &o_nil);
    addIntrinsicVar(class_thread, "ip", &o_zero);
    addIntrinsicVar(class_thread, "sp", &o_zero);
    addIntrinsicVar(class_thread, "numpop", &o_zero);
    addIntrinsicVar(class_thread, "receiver", &o_nil);
    addIntrinsicVar(class_thread, "numArgsPushed", &o_zero);
    addIntrinsicVar(class_thread, "parent", &o_nil);
    addIntrinsicVar(class_thread, "terminalValue", &o_nil);

    addIntrinsicVar(class_thread, "primitiveError", &o_zero);
    addIntrinsicVar(class_thread, "primitiveIndex", &o_zero);
    addIntrinsicVar(class_thread, "randData", &o_zero);
    addIntrinsicVar(class_thread, "beats", &o_fzero);
    addIntrinsicVar(class_thread, "seconds", &o_fzero);
    addIntrinsicVar(class_thread, "clock", &o_nil);
    addIntrinsicVar(class_thread, "nextBeat", &o_nil);
    addIntrinsicVar(class_thread, "endBeat", &o_nil);
    addIntrinsicVar(class_thread, "endValue", &o_nil);

    addIntrinsicVar(class_thread, "environment", &o_nil);
    addIntrinsicVar(class_thread, "exceptionHandler", &o_nil);
    addIntrinsicVar(class_thread, "threadPlayer", &o_nil);

    addIntrinsicVar(class_thread, "executingPath", &o_nil);
    addIntrinsicVar(class_thread, "oldExecutingPath", &o_nil);

    class_finalizer = makeIntrinsicClass(s_finalizer, s_object, 2, 0);
    addIntrinsicVar(class_finalizer, "cFunction", &o_nil);
    addIntrinsicVar(class_finalizer, "object", &o_nil);

    class_routine = makeIntrinsicClass(s_routine, s_thread, 0, 0);

    class_symbol = makeIntrinsicClass(s_symbol, s_object, 0, 0);
    class_nil = makeIntrinsicClass(s_nil, s_object, 0, 0);

    class_boolean = makeIntrinsicClass(s_boolean, s_object, 0, 0);
    class_true = makeIntrinsicClass(s_true, s_boolean, 0, 0);
    class_false = makeIntrinsicClass(s_false, s_boolean, 0, 0);

    class_magnitude = makeIntrinsicClass(s_magnitude, s_object, 0, 0);
    class_char = makeIntrinsicClass(s_char, s_magnitude, 0, 0);
    class_number = makeIntrinsicClass(s_number, s_magnitude, 0, 0);
    class_simple_number = makeIntrinsicClass(s_simple_number, s_number, 0, 0);
    class_int = makeIntrinsicClass(s_int, s_simple_number, 0, 0);
    class_float = makeIntrinsicClass(s_float, s_simple_number, 0, 0);

    class_rawptr = makeIntrinsicClass(s_rawptr, s_object, 0, 0);

    /*
        class_complex = makeIntrinsicClass(s_complex, s_number, 2, 0);
            addIntrinsicVar(class_complex, "real", &o_nil);
            addIntrinsicVar(class_complex, "imag", &o_nil);
    */

    class_rawarray = makeIntrinsicClass(s_rawarray, s_arrayed_collection, 0, 0);
    // SetInt(&class_rawarray->instanceFormat, obj_int8);
    // slotRawInt(&class_rawarray->classFlags) |= classHasIndexableInstances;
    class_int8array = makeIntrinsicClass(s_int8array, s_rawarray, 0, 0);
    SetInt(&class_int8array->instanceFormat, obj_int8);
    SetInt(&class_int8array->classFlags, slotRawInt(&class_int8array->classFlags) | classHasIndexableInstances);
    class_int16array = makeIntrinsicClass(s_int16array, s_rawarray, 0, 0);
    SetInt(&class_int16array->instanceFormat, obj_int16);
    SetInt(&class_int16array->classFlags, slotRawInt(&class_int16array->classFlags) | classHasIndexableInstances);
    class_int32array = makeIntrinsicClass(s_int32array, s_rawarray, 0, 0);
    SetInt(&class_int32array->instanceFormat, obj_int32);
    SetInt(&class_int32array->classFlags, slotRawInt(&class_int32array->classFlags) | classHasIndexableInstances);
    class_symbolarray = makeIntrinsicClass(s_symbolarray, s_rawarray, 0, 0);
    SetInt(&class_symbolarray->instanceFormat, obj_symbol);
    SetInt(&class_symbolarray->classFlags, slotRawInt(&class_symbolarray->classFlags) | classHasIndexableInstances);
    class_string = makeIntrinsicClass(s_string, s_rawarray, 0, 1);
    addIntrinsicClassVar(class_string, "unixCmdActions", &o_nil);
    SetInt(&class_string->instanceFormat, obj_char);
    SetInt(&class_string->classFlags, slotRawInt(&class_string->classFlags) | classHasIndexableInstances);
    class_floatarray = makeIntrinsicClass(s_floatarray, s_rawarray, 0, 0);
    SetInt(&class_floatarray->instanceFormat, obj_float);
    SetInt(&class_floatarray->classFlags, slotRawInt(&class_floatarray->classFlags) | classHasIndexableInstances);
    class_signal = makeIntrinsicClass(s_signal, s_floatarray, 0, 0);
    SetInt(&class_signal->instanceFormat, obj_float);
    SetInt(&class_signal->classFlags, slotRawInt(&class_signal->classFlags) | classHasIndexableInstances);
    class_wavetable = makeIntrinsicClass(s_wavetable, s_floatarray, 0, 0);
    SetInt(&class_wavetable->instanceFormat, obj_float);
    SetInt(&class_wavetable->classFlags, slotRawInt(&class_wavetable->classFlags) | classHasIndexableInstances);

    // addIntrinsicVar(class_signal, "rate", &o_nil);
    class_doublearray = makeIntrinsicClass(s_doublearray, s_rawarray, 0, 0);
    SetInt(&class_doublearray->instanceFormat, obj_double);
    SetInt(&class_doublearray->classFlags, slotRawInt(&class_doublearray->classFlags) | classHasIndexableInstances);

    class_list = makeIntrinsicClass(s_list, s_sequenceable_collection, 1, 0);
    addIntrinsicVar(class_list, "array", &o_nil);
    // addIntrinsicVar(class_list, "size", &o_zero);

    class_func = makeIntrinsicClass(s_func, s_absfunc, 2, 0);
    addIntrinsicVar(class_func, "def", &o_nil);
    addIntrinsicVar(class_func, "context", &o_nil);

    class_server_shm_interface = makeIntrinsicClass(s_server_shm_interface, s_object, 2, 0);
    addIntrinsicVar(class_server_shm_interface, "ptr", &o_nil);
    addIntrinsicVar(class_server_shm_interface, "finalizer", &o_nil);

    gTagClassTable[0] = NULL;
    gTagClassTable[1] = NULL;
    gTagClassTable[2] = class_int;
    gTagClassTable[3] = class_symbol;
    gTagClassTable[4] = class_char;
    gTagClassTable[5] = class_nil;
    gTagClassTable[6] = class_false;
    gTagClassTable[7] = class_true;
    gTagClassTable[8] = class_rawptr;
    gTagClassTable[9] = class_float;
    gTagClassTable[10] = class_float;
    gTagClassTable[11] = class_float;
    gTagClassTable[12] = class_float;

    SetObject(&o_emptyarray, newPyrArray(NULL, 0, obj_permanent | obj_immutable, false));

    SetObject(&o_onenilarray, newPyrArray(NULL, 1, obj_permanent | obj_immutable, false));
    slotRawObject(&o_onenilarray)->size = 1;
    SetNil(slotRawObject(&o_onenilarray)->slots);

    SetObject(&o_argnamethis, newPyrSymbolArray(NULL, 1, obj_permanent | obj_immutable, false));
    slotRawSymbolArray(&o_argnamethis)->size = 1;
    slotRawSymbolArray(&o_argnamethis)->symbols[0] = s_this;

    /*
    post("array %p '%s'\n", class_array, class_array->name.us->name);
    post("o_emptyarray %p '%s'\n", slotRawObject(&o_emptyarray)->classptr,
    slotRawObject(&o_emptyarray)->classptr->name.us->name); post("o_argnamethis %p '%s'\n",
    slotRawObject(&o_argnamethis)->classptr, slotRawObject(&o_argnamethis)->classptr->name.us->name);
    post("o_onenilarray %p '%s'\n", slotRawObject(&o_onenilarray)->classptr,
    slotRawObject(&o_onenilarray)->classptr->name.us->name); dumpObjectSlot(&o_emptyarray);
    dumpObjectSlot(&o_argnamethis);
    dumpObjectSlot(&o_onenilarray);
    */
}

PyrObject* instantiateObject(class PyrGC* gc, PyrClass* classobj, int size, bool fill, bool runGC) {
    PyrObject *newobj, *proto;
    int numbytes, format, flags;

    format = slotRawInt(&classobj->instanceFormat);
    flags = slotRawInt(&classobj->instanceFlags);

    if (slotRawInt(&classobj->classFlags) & classHasIndexableInstances) {
        // create an indexable object
        numbytes = size * gFormatElemSize[format];
        newobj = gc->New(numbytes, flags, format, runGC);
        if (fill) {
            newobj->size = size;
            if (format == obj_slot) {
                nilSlots(newobj->slots, size);
            } else {
                memset(newobj->slots, format == obj_char ? ' ' : 0, size * gFormatElemSize[format]);
            }
        } else {
            newobj->size = 0;
        }
    } else {
        if (IsObj(&classobj->iprototype)) {
            proto = slotRawObject(&classobj->iprototype);
            size = proto->size;
            numbytes = size * sizeof(PyrSlot);
            newobj = gc->New(numbytes, flags, format, runGC);
            newobj->size = size;
            if (size) {
                memcpy(newobj->slots, proto->slots, numbytes);
            }
        } else {
            numbytes = 0;
            newobj = gc->New(numbytes, flags, format, runGC);
            newobj->size = 0;
        }
    }
    newobj->classptr = classobj;
    return newobj;
}

PyrObject* instantiateObjectLight(class PyrGC* gc, PyrClass* classobj, int size, bool runGC);
PyrObject* instantiateObjectLight(class PyrGC* gc, PyrClass* classobj, int size, bool runGC) {
    PyrObject *newobj, *proto;
    int numbytes, format, flags;

    format = slotRawInt(&classobj->instanceFormat);
    flags = slotRawInt(&classobj->instanceFlags);

    if (slotRawInt(&classobj->classFlags) & classHasIndexableInstances) {
        numbytes = size * gFormatElemSize[format];
    } else {
        if (IsObj(&classobj->iprototype)) {
            proto = slotRawObject(&classobj->iprototype);
            size = proto->size;
            numbytes = size * sizeof(PyrSlot);
        } else {
            size = 0;
            numbytes = 0;
        }
    }
    newobj = gc->New(numbytes, flags, format, runGC);
    newobj->size = size;
    newobj->classptr = classobj;

    return newobj;
}

PyrObject* copyObject(class PyrGC* gc, PyrObject* inobj, bool runGC) {
    PyrObject* newobj;

    // copies are neither immutable not permanent
    int flags = ~(obj_immutable)&inobj->obj_flags;
    flags = ~(obj_permanent)&flags;

    int elemsize = gFormatElemSize[inobj->obj_format];
    int numbytes = inobj->size * elemsize;

    newobj = gc->New(numbytes, flags, inobj->obj_format, runGC);

    newobj->size = inobj->size;
    newobj->classptr = inobj->classptr;

    memcpy(newobj->slots, inobj->slots, inobj->size * elemsize);
    return newobj;
}

PyrObject* copyObjectRange(class PyrGC* gc, PyrObject* inobj, int start, int end, bool runGC) {
    PyrObject* newobj;

    if (start < 0)
        start = 0;
    if (end >= inobj->size)
        end = inobj->size - 1;
    int length = end - start + 1;
    if (length < 0)
        length = 0;

    int elemsize = gFormatElemSize[inobj->obj_format];
    int numbytes = length * elemsize;

    // copies are neither immutable not permanent
    int flags = ~(obj_immutable)&inobj->obj_flags;
    flags = ~(obj_permanent)&flags;

    newobj = gc->New(numbytes, flags, inobj->obj_format, runGC);
    newobj->size = length;
    newobj->classptr = inobj->classptr;

    if (length > 0) {
        memcpy(newobj->slots, (char*)(inobj->slots) + start * elemsize, length * elemsize);
    }
    return newobj;
}

void dumpObject(PyrObject* obj) {
    char str[256];
    PyrClass* classobj;
    int i;

    if (obj == NULL) {
        postfl("NULL object pointer\n");
        return;
    }
    classobj = obj->classptr;
    if (isKindOf(obj, class_class)) {
        post("class %s (%p) {\n", slotRawSymbol(&((PyrClass*)obj)->name)->name, obj);
    } else {
        // post("Instance of %s (%p) {\n", slotRawSymbol(&classobj->name)->name, obj);
        post("Instance of %s {    (%p, gc=%02X, fmt=%02X, flg=%02X, set=%02X)\n", slotRawSymbol(&classobj->name)->name,
             obj, obj->gc_color, obj->obj_format, obj->obj_flags, obj->obj_sizeclass);
    }
    // flushPostBuf();

    if (obj->obj_format == obj_notindexed) {
        post("  instance variables [%d]\n", obj->size);
        for (i = 0; i < obj->size; ++i) {
            slotString(obj->slots + i, str);
            post("    %s : %s\n", slotRawSymbolArray(&classobj->instVarNames)->symbols[i]->name, str);
        }
    } else {
        int maxsize;
        post("  indexed slots [%d]\n", obj->size);
        maxsize = sc_min(32, obj->size);
        switch (obj->obj_format) {
        case obj_slot:
            for (i = 0; i < maxsize; ++i) {
                slotString(obj->slots + i, str);
                post("    %3d : %s\n", i, str);
            }
            break;
        case obj_double:
            for (i = 0; i < maxsize; ++i) {
                post("    %3d : %f\n", i, slotRawFloat(&obj->slots[i]));
            }
            break;
        case obj_float:
            for (i = 0; i < maxsize; ++i) {
                float val;
                val = ((float*)(obj->slots))[i];
                post("    %3d : %g\n", i, val);
            }
            break;
        case obj_int32:
            for (i = 0; i < maxsize; ++i) {
                int32 val;
                val = ((int32*)(obj->slots))[i];
                post("    %3d : %d\n", i, val);
            }
            break;
        case obj_int16:
            for (i = 0; i < maxsize; ++i) {
                int16 val;
                val = ((int16*)(obj->slots))[i];
                post("    %3d : %d\n", i, val);
            }
            break;
        case obj_int8:
            for (i = 0; i < maxsize; ++i) {
                int8 val;
                val = ((int8*)(obj->slots))[i];
                post("    %3d : %4d %4u 0x%02X\n", i, val, val & 255, val & 255);
            }
            break;
        case obj_char:
            for (i = 0; i < maxsize; ++i) {
                char val;
                val = ((char*)(obj->slots))[i];
                post("    %3d : %c\n", i, val);
            }
            break;
        case obj_symbol:
            for (i = 0; i < maxsize; ++i) {
                PyrSymbol* sym;
                sym = ((PyrSymbol**)(obj->slots))[i];
                post("    %3d : '%s'\n", i, sym->name);
            }
            break;
        default:
            post("unknown obj_format %X\n", obj->obj_format);
        }
        if (obj->size > maxsize) {
            post("    ...\n");
        }
    }
    post("}\n");
}

void dumpBadObject(PyrObject* obj) {
    char str[128];
    PyrClass* classobj;
    int i;

    if (obj == NULL) {
        postfl("NULL object pointer\n");
        return;
    }
    classobj = obj->classptr;
    if (isKindOf(obj, class_class)) {
        postfl("class %s (%p) {\n", slotRawSymbol(&((PyrClass*)obj)->name)->name, obj);
    } else {
        // postfl("Instance of %s (%p) {\n", slotRawSymbol(&classobj->name)->name, obj);
        postfl("Instance of %s {    (%p, gc=%02X, fmt=%02X, flg=%02X, set=%02X)\n",
               slotRawSymbol(&classobj->name)->name, obj, obj->gc_color, obj->obj_format, obj->obj_flags,
               obj->obj_sizeclass);
    }
    if (obj->obj_format == obj_notindexed) {
        postfl("  instance variables [%d]\n", obj->size);
        for (i = 0; i < obj->size; ++i) {
            slotString(obj->slots + i, str);
            postfl("    %s : %s\n", slotRawSymbolArray(&classobj->instVarNames)->symbols[i]->name, str);
        }
    } else {
        int maxsize;
        postfl("  indexed slots [%d]\n", obj->size);
        maxsize = obj->size;
        maxsize = sc_min(32, maxsize);
        switch (obj->obj_format) {
        case obj_slot:
            for (i = 0; i < maxsize; ++i) {
                slotString(obj->slots + i, str);
                postfl("    %3d : %s\n", i, str);
            }
            break;
        case obj_double:
            for (i = 0; i < maxsize; ++i) {
                postfl("    %3d : %f\n", i, slotRawFloat(&obj->slots[i]));
            }
            break;
        case obj_float:
            for (i = 0; i < maxsize; ++i) {
                float val;
                val = ((float*)(obj->slots))[i];
                postfl("    %3d : %g\n", i, val);
            }
            break;
        case obj_int32:
            for (i = 0; i < maxsize; ++i) {
                int32 val;
                val = ((int32*)(obj->slots))[i];
                postfl("    %3d : %d\n", i, val);
            }
            break;
        case obj_int16:
            for (i = 0; i < maxsize; ++i) {
                int16 val;
                val = ((int16*)(obj->slots))[i];
                postfl("    %3d : %d\n", i, val);
            }
            break;
        case obj_int8:
            for (i = 0; i < maxsize; ++i) {
                int8 val;
                val = ((int8*)(obj->slots))[i];
                postfl("    %3d : %4d %4u 0x%02X\n", i, val, val & 255, val & 255);
            }
            break;
        case obj_char:
            for (i = 0; i < maxsize; ++i) {
                char val;
                val = ((char*)(obj->slots))[i];
                postfl("    %3d : %c\n", i, val);
            }
            break;
        case obj_symbol:
            for (i = 0; i < maxsize; ++i) {
                PyrSymbol* sym;
                sym = ((PyrSymbol**)(obj->slots))[i];
                post("    %3d : '%s'\n", i, sym->name);
            }
            break;
        default:
            postfl("unknown obj_format %X\n", obj->obj_format);
        }
        if (obj->size > maxsize) {
            postfl("    ...\n");
        }
    }
    postfl("}\n");
}

void dumpObjectSlot(PyrSlot* slot) {
    if (IsObj(slot)) {
        dumpObject(slotRawObject(slot));
    } else {
        dumpPyrSlot(slot);
    }
}

void dumpSlotOneWord(const char* tagstr, PyrSlot* slot) {
    char str[256];
    slotOneWord(slot, str);
    post("%s %s\n", tagstr, str);
}

void CallStackSanity(VMGlobals* g, const char* tagstr);
void CallStackSanity(VMGlobals* g, const char* tagstr) {
    PyrFrame* frame;
    frame = g->frame;
    while (frame) {
        if (FrameSanity(frame, tagstr)) {
            DumpBackTrace(g);
            // Debugger();
            break;
        }
        frame = slotRawFrame(&frame->caller);
    }
}

bool FrameSanity(PyrFrame* frame, const char* tagstr);
bool FrameSanity(PyrFrame* frame, const char* tagstr) {
    bool failed = false;
    if (frame == NULL)
        return false;
    if (NotObj(&frame->method)) {
        postfl("Frame %p method tag wrong %p\n", frame, GetTag(&frame->method));
        failed = true;
        //} else if (!isKindOf((PyrObject*)slotRawObject(&frame->method)->classptr, class_fundef)) {
    } else if (slotRawObject(&frame->method)->classptr != class_method
               && slotRawObject(&frame->method)->classptr != class_fundef) {
        postfl("Frame %p method class wrong %p\n", frame, slotRawObject(&frame->method)->classptr);
        failed = true;
        // if (slotRawObject(&frame->method)->classptr->classptr == class_class) {
        postfl("class: '%s'\n", slotRawSymbol(&slotRawObject(&frame->method)->classptr->name)->name);
        ///} else {
        //	postfl("not even a class\n");
        //}
    } else if (NotObj(&slotRawBlock(&frame->method)->code)) {
        postfl("Method %p code tag wrong %p\n", slotRawBlock(&frame->method),
               GetTag(&slotRawBlock(&frame->method)->code));
        failed = true;
    } else if (slotRawObject(&slotRawBlock(&frame->method)->code)->classptr != class_int8array) {
        postfl("Code %p class wrong %p\n", slotRawObject(&slotRawBlock(&frame->method)->code),
               slotRawObject(&slotRawBlock(&frame->method)->code)->classptr);
        postfl("class: '%s'\n",
               slotRawSymbol(&slotRawObject(&slotRawBlock(&frame->method)->code)->classptr->name)->name);
        failed = true;
    }
    /*
    if (frame->caller.utag != tagHFrame && frame->caller.utag != tagNil) {
        postfl("Frame %p caller tag wrong %p\n", frame, frame->caller.utag);
        failed = true;
    }
    if (frame->context.utag != tagHFrame && frame->context.utag != tagNil) {
        postfl("Frame %p context tag wrong %p\n", frame, frame->context.utag);
        failed = true;
    }
    if (frame->homeContext.utag != tagHFrame && frame->homeContext.utag != tagNil) {
        postfl("Frame %p homeContext tag wrong %p\n", frame, frame->homeContext.utag);
        failed = true;
    }
    */
    if (!IsPtr(&frame->ip)) {
        postfl("Frame %p ip tag wrong %p\n", frame, GetTag(&frame->ip));
        failed = true;
    }
    return failed;
}

void DumpFrame(PyrFrame* frame) {
    char str[256];
    int i, numargs;
    PyrMethod* meth;
    PyrMethodRaw* methraw;

    if (FrameSanity(frame, "DumpFrame")) {
        post("FRAME CORRUPTED\n");
        return;
    }
    slotOneWord(&frame->method, str);
    // slotString(&frame->method, str);

    meth = slotRawMethod(&frame->method);
    methraw = METHRAW(meth);
    if (methraw->numtemps) {
        post("\t%s\n", str);
        numargs = methraw->numargs + methraw->varargs;
        for (i = 0; i < methraw->numtemps; ++i) {
            slotOneWord(frame->vars + i, str);
            // slotString(frame->vars + i, str);
            if (i < numargs) {
                post("\t\targ %s = %s\n", slotRawSymbolArray(&meth->argNames)->symbols[i]->name, str);
            } else {
                post("\t\tvar %s = %s\n", slotRawSymbolArray(&meth->varNames)->symbols[i - numargs]->name, str);
            }
        }
    } else {
        post("\t%s  (no arguments or variables)\n", str);
    }
}

void dumpByteCodes(PyrBlock* theBlock);

void DumpDetailedFrame(PyrFrame* frame);
void DumpDetailedFrame(PyrFrame* frame) {
    char mstr[256];
    char str[256];
    int i, numargs;
    PyrMethod* meth;
    PyrMethodRaw* methraw;

    if (FrameSanity(frame, "DumpDetailedFrame")) {
        post("FRAME CORRUPTED\n");
        return;
    }
    slotOneWord(&frame->method, mstr);
    // slotString(&frame->method, str);

    meth = slotRawMethod(&frame->method);
    methraw = METHRAW(meth);

    if (methraw->numtemps) {
        post("\t%s\n", mstr);
        numargs = methraw->numargs + methraw->varargs;
        for (i = 0; i < methraw->numtemps; ++i) {
            slotOneWord(frame->vars + i, str);
            // slotString(frame->vars + i, str);
            if (i < numargs) {
                post("\t\targ %s = %s\n", slotRawSymbolArray(&meth->argNames)->symbols[i]->name, str);
            } else {
                post("\t\tvar %s = %s\n", slotRawSymbolArray(&meth->varNames)->symbols[i - numargs]->name, str);
            }
        }
    } else {
        post("\t%s  (no arguments or variables)\n", mstr);
    }

    post("\t....%s details:\n", mstr);
    post("\t\tneedsHeapContext  = %d\n", methraw->needsHeapContext);
    post("\t\tnumtemps  = %d\n", methraw->numtemps);
    post("\t\tpopSize  = %d\n", methraw->popSize);

    slotString(&frame->method, str);
    post("\t\tmethod  = %s\n", str);
    slotString(&frame->caller, str);
    post("\t\tcaller  = %s\n", str);
    slotString(&frame->context, str);
    post("\t\tcontext = %s\n", str);
    slotString(&frame->homeContext, str);
    post("\t\thomeCtx = %s\n", str);
    slotString(&frame->ip, str);
    post("\t\tip      = %s\n", str);

    if (IsPtr(&frame->ip)) {
        post("ipoffset = %d\n", (char*)slotRawPtr(&frame->ip) - (char*)slotRawInt8Array(&meth->code)->b);
        dumpByteCodes(meth);
    }
}


bool respondsTo(PyrSlot* slot, PyrSymbol* selector) {
    PyrClass* classobj;
    PyrMethod* meth;
    int index;

    classobj = classOfSlot(slot);

    index = slotRawInt(&classobj->classIndex) + selector->u.index;
    meth = gRowTable[index];
    return slotRawSymbol(&meth->name) == selector;
}

PyrMethod* methodLookup(PyrSlot* slot, PyrSymbol* selector);
PyrMethod* methodLookup(PyrSlot* slot, PyrSymbol* selector) {
    PyrClass* classobj;
    PyrMethod* meth;
    int index;

    classobj = classOfSlot(slot);

    index = slotRawInt(&classobj->classIndex) + selector->u.index;
    meth = gRowTable[index];
    return meth;
}


bool isSubclassOf(PyrClass* classobj, PyrClass* testclass) {
    while (classobj) {
        if (classobj == testclass) {
            return true;
        }
        classobj = slotRawSymbol(&classobj->superclass)->u.classobj;
    }
    return false;
}

/*bool isKindOf(PyrObjectHdr *obj, PyrClass *testclass)
{
    int objClassIndex = slotRawInt(&obj->classptr->classIndex);
    return objClassIndex >= slotRawInt(&testclass->classIndex) && objClassIndex <=
slotRawInt(&testclass->maxSubclassIndex);
}*/

bool objAddIndexedSlot(PyrObject* obj, PyrSlot* slot) {
    if (obj->size < ARRAYMAXINDEXSIZE(obj)) {
        slotCopy(&obj->slots[obj->size++], slot);
        return true;
    } else {
        return false;
    }
}

bool objAddIndexedSymbol(PyrSymbolArray* obj, PyrSymbol* symbol) {
    if (obj->size < MAXINDEXSIZE((PyrObject*)obj)) {
        obj->symbols[obj->size++] = symbol;
        return true;
    } else {
        return false;
    }
}

bool objAddIndexedObject(PyrObject* obj, PyrObject* obj2) {
    if (obj->size < ARRAYMAXINDEXSIZE(obj)) {
        SetObject(obj->slots + obj->size, obj2);
        obj->size++;
        return true;
    } else {
        return false;
    }
}

void fillSlots(PyrSlot* slot, int size, PyrSlot* fillslot) {
    for (int i = 0; i != size; ++i)
        slotCopy(&slot[i], fillslot);
}

void nilSlots(PyrSlot* slot, int size) { fillSlots(slot, size, &o_nil); }

void zeroSlots(PyrSlot* slot, int size) {
    PyrSlot zero;
    SetTagRaw(&zero, 0);
    SetRaw(&zero, 0.0);
    fillSlots(slot, size, &zero);
}

PyrObject* newPyrObject(class PyrGC* gc, size_t inNumBytes, int inFlags, int inFormat, bool inRunGC) {
    return gc->New(inNumBytes, inFlags, inFormat, inRunGC);
}

PyrObject* newPyrArray(class PyrGC* gc, int size, int flags, bool runGC) {
    PyrObject* array;

    int numbytes = size * sizeof(PyrSlot);
    if (!gc)
        array = PyrGC::NewPermanent(numbytes, flags, obj_slot);
    else
        array = gc->New(numbytes, flags, obj_slot, runGC);
    array->classptr = class_array;
    return array;
}

PyrSymbolArray* newPyrSymbolArray(class PyrGC* gc, int size, int flags, bool runGC) {
    PyrSymbolArray* array;

    int numbytes = size * sizeof(PyrSymbol*);
    if (!gc)
        array = (PyrSymbolArray*)PyrGC::NewPermanent(numbytes, flags, obj_symbol);
    else
        array = (PyrSymbolArray*)gc->New(numbytes, flags, obj_symbol, runGC);
    array->classptr = class_symbolarray;
    return array;
}

PyrInt8Array* newPyrInt8Array(class PyrGC* gc, int size, int flags, bool runGC) {
    PyrInt8Array* array;

    if (!gc)
        array = (PyrInt8Array*)PyrGC::NewPermanent(size, flags, obj_int8);
    else
        array = (PyrInt8Array*)gc->New(size, flags, obj_int8, runGC);
    array->classptr = class_int8array;
    return array;
}

PyrInt32Array* newPyrInt32Array(class PyrGC* gc, int size, int flags, bool runGC) {
    PyrInt32Array* array;
    int numbytes = size * sizeof(int32);
    if (!gc)
        array = (PyrInt32Array*)PyrGC::NewPermanent(numbytes, flags, obj_int32);
    else
        array = (PyrInt32Array*)gc->New(numbytes, flags, obj_int32, runGC);
    array->classptr = class_int32array;
    return array;
}

PyrDoubleArray* newPyrDoubleArray(class PyrGC* gc, int size, int flags, bool runGC) {
    PyrDoubleArray* array;

    int numbytes = size * sizeof(double);
    if (!gc)
        array = (PyrDoubleArray*)PyrGC::NewPermanent(numbytes, flags, obj_double);
    else
        array = (PyrDoubleArray*)gc->New(size, flags, obj_double, runGC);
    array->classptr = class_doublearray;
    return array;
}

PyrString* newPyrString(class PyrGC* gc, const char* s, int flags, bool runGC) {
    PyrString* string;
    int length = strlen(s);

    if (!gc)
        string = (PyrString*)PyrGC::NewPermanent(length, flags, obj_char);
    else
        string = (PyrString*)gc->New(length, flags, obj_char, runGC);
    string->classptr = class_string;
    string->size = length;
    memcpy(string->s, s, length);
    return string;
}

PyrString* newPyrStringN(class PyrGC* gc, int length, int flags, bool runGC) {
    PyrString* string;

    if (!gc)
        string = (PyrString*)PyrGC::NewPermanent(length, flags, obj_char);
    else
        string = (PyrString*)gc->New(length, flags, obj_char, runGC);
    string->classptr = class_string;
    string->size = length; // filled with garbage!
    return string;
}

PyrBlock* newPyrBlock(int flags) {
    PyrBlock* block;
    PyrMethodRaw* methraw;


    int32 numbytes = sizeof(PyrBlock) - sizeof(PyrObjectHdr);
    int32 numSlots = numbytes / sizeof(PyrSlot);

    if (!compilingCmdLine)
        block = (PyrBlock*)PyrGC::NewPermanent(numbytes, flags, obj_notindexed);
    else
        block = (PyrBlock*)gMainVMGlobals->gc->New(numbytes, flags, obj_notindexed, false);
    block->classptr = class_fundef;
    block->size = numSlots;

    // clear out raw area
    methraw = METHRAW(block);
    methraw->specialIndex = 0;
    methraw->methType = methBlock;
    methraw->needsHeapContext = 0;
    methraw->frameSize = 0;
    methraw->varargs = 0;
    methraw->numargs = 0;
    methraw->numvars = 0;
    methraw->numtemps = 0;
    methraw->popSize = 0;

    nilSlots(&block->rawData1, numSlots);
    return block;
}

SCLANG_DLLEXPORT_C struct VMGlobals* scGlobals() { return gMainVMGlobals; }

PyrMethod* initPyrMethod(PyrMethod* method) {
    int32 numbytes = sizeof(PyrMethod) - sizeof(PyrObjectHdr);
    int32 numSlots = numbytes / sizeof(PyrSlot);

    method->classptr = class_method;
    method->size = 0;
    method->size = numSlots;
    SetFloat(&method->rawData1, 0.0);
    SetFloat(&method->rawData2, 0.0);
    nilSlots(&method->code, numSlots - 2);
    // slotCopy(&method->byteMeter, &o_zero);
    // slotCopy(&method->callMeter, &o_zero);
    // post("<- newPyrMethod %p %p\n", method, methraw);
    return method;
}

PyrMethod* newPyrMethod() {
    int32 numbytes = sizeof(PyrMethod) - sizeof(PyrObjectHdr);
    PyrMethod* method = (PyrMethod*)PyrGC::NewPermanent(numbytes, obj_permanent | obj_immutable, obj_notindexed);
    return initPyrMethod(method);
}

void freePyrSlot(PyrSlot* slot) {
    if (IsObj(slot)) {
        PyrObject* obj = slotRawObject(slot);

        if (obj && obj->IsPermanent()) {
            // don't deallocate these
            if (obj != slotRawObject(&o_emptyarray) && obj != slotRawObject(&o_onenilarray)
                && obj != slotRawObject(&o_argnamethis))
                pyr_pool_runtime->Free((void*)obj);

            SetNil(slot);
        }
    }
}

void freePyrObject(PyrObject* obj) {
    if (obj->IsPermanent()) {
        pyr_pool_runtime->Free((void*)obj);
    }
}

int getIndexedInt(PyrObject* obj, int index, int* value) {
    PyrSlot* slot;
    int err = errNone;
    if (index < 0 || index >= obj->size)
        return errIndexOutOfRange;
    switch (obj->obj_format) {
    case obj_slot:
        slot = obj->slots + index;
        if (IsFloat(slot)) {
            *value = (int)slotRawFloat(slot);
        } else if (IsInt(slot)) {
            *value = slotRawInt(slot);
        } else {
            err = errWrongType;
        }
        break;
    case obj_double:
        *value = (int)((double*)(obj->slots))[index];
        break;
    case obj_float:
        *value = (int)((float*)(obj->slots))[index];
        break;
    case obj_int32:
        *value = ((int32*)(obj->slots))[index];
        break;
    case obj_int16:
        *value = ((int16*)(obj->slots))[index];
        break;
    case obj_int8:
        *value = ((int8*)(obj->slots))[index];
        break;
    default:
        err = errWrongType;
    }
    return err;
}

int getIndexedFloat(PyrObject* obj, int index, float* value) {
    PyrSlot* slot;
    int err = errNone;
    if (index < 0 || index >= obj->size)
        return errIndexOutOfRange;
    switch (obj->obj_format) {
    case obj_slot:
        slot = obj->slots + index;
        if (IsFloat(slot)) {
            *value = slotRawFloat(slot);
        } else if (IsInt(slot)) {
            *value = slotRawInt(slot);
        } else {
            err = errWrongType;
        }
        break;
    case obj_double:
        *value = ((double*)(obj->slots))[index];
        break;
    case obj_float:
        *value = ((float*)(obj->slots))[index];
        break;
    case obj_int32:
        *value = ((int32*)(obj->slots))[index];
        break;
    case obj_int16:
        *value = ((int16*)(obj->slots))[index];
        break;
    case obj_int8:
        *value = ((int8*)(obj->slots))[index];
        break;
    default:
        err = errWrongType;
    }
    return err;
}

int getIndexedDouble(PyrObject* obj, int index, double* value) {
    PyrSlot* slot;
    int err = errNone;
    if (index < 0 || index >= obj->size)
        return errIndexOutOfRange;
    switch (obj->obj_format) {
    case obj_slot:
        slot = obj->slots + index;
        if (IsFloat(slot)) {
            *value = slotRawFloat(slot);
        } else if (IsInt(slot)) {
            *value = slotRawInt(slot);
        } else {
            err = errWrongType;
        }
        break;
    case obj_double:
        *value = ((double*)(obj->slots))[index];
        break;
    case obj_float:
        *value = ((float*)(obj->slots))[index];
        break;
    case obj_int32:
        *value = ((int32*)(obj->slots))[index];
        break;
    case obj_int16:
        *value = ((int16*)(obj->slots))[index];
        break;
    case obj_int8:
        *value = ((int8*)(obj->slots))[index];
        break;
    default:
        err = errWrongType;
    }
    return err;
}


void getIndexedSlot(PyrObject* obj, PyrSlot* a, int index) {
    //	postfl("getIndexedSlot %s %X %d\n", slotRawSymbol(&obj->classptr->name)->name,
    //		obj, index);
    switch (obj->obj_format) {
    case obj_slot:
        slotCopy(a, &obj->slots[index]);
        break;
    case obj_double:
        SetFloat(a, ((double*)(obj->slots))[index]);
        break;
    case obj_float:
        SetFloat(a, ((float*)(obj->slots))[index]);
        break;
    case obj_int32:
        SetInt(a, ((int32*)(obj->slots))[index]);
        break;
    case obj_int16:
        SetInt(a, ((int16*)(obj->slots))[index]);
        break;
    case obj_int8:
        SetInt(a, ((int8*)(obj->slots))[index]);
        break;
    case obj_symbol:
        SetSymbol(a, (PyrSymbol*)((int**)(obj->slots))[index]);
        break;
    case obj_char:
        SetChar(a, ((unsigned char*)(obj->slots))[index]);
        break;
    }
}

int putIndexedSlot(VMGlobals* g, PyrObject* obj, PyrSlot* c, int index) {
    PyrSlot* slot;
    switch (obj->obj_format) {
    case obj_slot:
        if (obj->IsImmutable())
            return errImmutableObject;
        slot = obj->slots + index;
        slotCopy(slot, c);
        g->gc->GCWrite(obj, slot);
        break;
    case obj_double:
        if (NotFloat(c)) {
            if (NotInt(c))
                return errWrongType;
            else {
                ((double*)(obj->slots))[index] = slotRawInt(c);
            }
        } else
            ((double*)(obj->slots))[index] = slotRawFloat(c);
        break;
    case obj_float:
        if (NotFloat(c)) {
            if (NotInt(c))
                return errWrongType;
            else {
                ((float*)(obj->slots))[index] = slotRawInt(c);
            }
        } else
            ((float*)(obj->slots))[index] = slotRawFloat(c);
        break;
    case obj_int32:
        if (NotInt(c))
            return errWrongType;
        ((int32*)(obj->slots))[index] = slotRawInt(c);
        break;
    case obj_int16:
        if (NotInt(c))
            return errWrongType;
        ((int16*)(obj->slots))[index] = slotRawInt(c);
        break;
    case obj_int8:
        if (NotInt(c))
            return errWrongType;
        ((int8*)(obj->slots))[index] = slotRawInt(c);
        break;
    case obj_symbol:
        if (NotSym(c))
            return errWrongType;
        ((PyrSymbol**)(obj->slots))[index] = slotRawSymbol(c);
        break;
    case obj_char:
        if (NotChar(c))
            return errWrongType;
        ((unsigned char*)(obj->slots))[index] = slotRawChar(c);
        break;
    }
    return errNone;
}

int putIndexedFloat(PyrObject* obj, double val, int index) {
    PyrSlot* slot;
    switch (obj->obj_format) {
    case obj_slot:
        if (obj->IsImmutable())
            return errImmutableObject;
        slot = obj->slots + index;
        SetFloat(slot, val);
        break;
    case obj_double:
        ((double*)(obj->slots))[index] = val;
        break;
    case obj_float:
        ((float*)(obj->slots))[index] = (float)val;
        break;
    case obj_int32:
        ((int32*)(obj->slots))[index] = (int32)val;
        break;
    case obj_int16:
        ((int16*)(obj->slots))[index] = (int16)val;
        break;
    case obj_int8:
        ((int8*)(obj->slots))[index] = (int8)val;
        break;
    }
    return errNone;
}

static int hashPtr(void* ptr) {
    int32 hashed_part = int32((size_t)ptr & 0xffffffff);
    return Hash(hashed_part);
}

int calcHash(PyrSlot* a);
int calcHash(PyrSlot* a) {
    int hash;
    switch (GetTag(a)) {
    case tagObj:
        hash = hashPtr(slotRawObject(a));
        break;
    case tagInt:
        hash = Hash(slotRawInt(a));
        break;
    case tagChar:
        hash = Hash(slotRawChar(a) & 255);
        break;
    case tagSym:
        hash = slotRawSymbol(a)->hash;
        break;
    case tagNil:
        hash = 0xA5A5A5A5;
        break;
    case tagFalse:
        hash = 0x55AA55AA;
        break;
    case tagTrue:
        hash = 0x69696969;
        break;
    case tagPtr:
        hash = hashPtr(slotRawPtr(a));
        break;
    default:
        // hash for a double
        union {
            int32 i[2];
            double d;
        } u;
        u.d = slotRawFloat(a);
        hash = Hash(u.i[0] + Hash(u.i[1]));
    }
    return hash;
}

void InstallFinalizer(VMGlobals* g, PyrObject* inObj, int slotIndex, ObjFuncPtr inFunc) {
    PyrObject* finalizer = g->gc->NewFinalizer(inFunc, inObj, false);
    SetObject(inObj->slots + slotIndex, finalizer);
    g->gc->GCWriteNew(inObj, finalizer); // we know finalizer is white so we can use GCWriteNew
}
