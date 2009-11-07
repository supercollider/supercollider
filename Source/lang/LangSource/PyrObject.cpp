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


PyrClass *gClassList = NULL;
int gNumSelectors = 0;
int gNumClasses = 0;
int gNumClassVars = 0;
int gFormatElemSize[NUMOBJFORMATS];
int gFormatElemCapc[NUMOBJFORMATS];
int gFormatElemTag[NUMOBJFORMATS];
PyrMethod *gNullMethod; // used to fill row table
PyrClass* gTagClassTable[16];

PyrClass *class_object;
PyrClass *class_dict;
PyrClass *class_array;
PyrClass *class_list, *class_method, *class_fundef, *class_frame, *class_class;
PyrClass *class_symbol, *class_nil;
PyrClass *class_boolean, *class_true, *class_false;
PyrClass *class_int, *class_char, *class_float, *class_complex;
PyrClass *class_rawptr;
PyrClass *class_string;
PyrClass *class_magnitude, *class_number, *class_collection, *class_ordered_collection;
PyrClass *class_arrayed_collection;
PyrClass *class_sequenceable_collection;
PyrClass *class_simple_number;
PyrClass *class_rawarray;
PyrClass *class_signal;
PyrClass *class_wavetable;
PyrClass *class_floatarray;
PyrClass *class_int8array;
PyrClass *class_int16array;
PyrClass *class_int32array;
PyrClass *class_symbolarray;
PyrClass *class_doublearray;
PyrClass *class_func, *class_absfunc;
PyrClass *class_stream;
PyrClass *class_process;
PyrClass *class_interpreter;
PyrClass *class_thread;
PyrClass *class_routine;
PyrClass *class_finalizer;

PyrSymbol *s_none;
PyrSymbol *s_object;
PyrSymbol *s_bag;
PyrSymbol *s_set;
PyrSymbol *s_identityset;
PyrSymbol *s_dictionary;
PyrSymbol *s_identitydictionary;
PyrSymbol *s_linkedlist;
PyrSymbol *s_sortedlist;
PyrSymbol *s_array;
PyrSymbol *s_list, *s_method, *s_fundef, *s_frame, *s_class;
PyrSymbol *s_symbol, *s_nil;
PyrSymbol *s_boolean, *s_true, *s_false;
PyrSymbol *s_int, *s_char, *s_color, *s_float, *s_complex;
PyrSymbol *s_rawptr, *s_objptr;
PyrSymbol *s_string;
PyrSymbol *s_magnitude, *s_number, *s_collection, *s_ordered_collection;
PyrSymbol *s_arrayed_collection;
PyrSymbol *s_sequenceable_collection;
PyrSymbol *s_simple_number;
PyrSymbol *s_signal;
PyrSymbol *s_wavetable;

PyrSymbol *s_rawarray;
PyrSymbol *s_int8array;
PyrSymbol *s_int16array;
PyrSymbol *s_int32array;
PyrSymbol *s_symbolarray;
PyrSymbol *s_doublearray;
PyrSymbol *s_floatarray;
PyrSymbol *s_point;
PyrSymbol *s_rect;
PyrSymbol *s_func, *s_absfunc;
PyrSymbol *s_stream;
PyrSymbol *s_process;
PyrSymbol *s_main;
PyrSymbol *s_thread;
PyrSymbol *s_routine;
PyrSymbol *s_task;
PyrSymbol *s_prstart;
PyrSymbol *s_interpreter;
PyrSymbol *s_finalizer;
PyrSymbol *s_awake;
PyrSymbol *s_appclock;
PyrSymbol *s_systemclock;

PyrSymbol *s_nocomprendo;
PyrSymbol *s_curProcess, *s_curMethod, *s_curBlock, *s_curClosure, *s_curThread;
//PyrSymbol *s_sampleRate;
//PyrSymbol *s_audioClock, *s_logicalClock;
PyrSymbol *s_run;
PyrSymbol *s_startup;
PyrSymbol *s_docmdline;
PyrSymbol *s_audio;
PyrSymbol *s_control;
PyrSymbol *s_scalar;
PyrSymbol *s_next;
PyrSymbol *s_env;
PyrSymbol *s_ugen, *s_outputproxy;
PyrSymbol *s_new, *s_ref, *s_value, *s_at, *s_put;
PyrSymbol *s_performList, *s_superPerformList;
PyrSymbol *s_series, *s_copyseries, *s_putseries;
PyrSymbol *s_envirGet, *s_envirPut;
PyrSymbol *s_synth, *s_spawn, *s_environment, *s_event;
PyrSymbol *s_hardwaresetup, *s_shutdown;
PyrSymbol *s_linear, *s_exponential, *s_gate;
PyrSymbol *s_super, *s_this;
PyrSlot o_nil, o_true, o_false, o_end;
PyrSlot o_pi, o_twopi;
PyrSlot o_fhalf, o_fnegone, o_fzero, o_fone, o_ftwo, o_inf;
PyrSlot o_negtwo, o_negone, o_zero, o_one, o_two;
PyrSlot o_nullframe, o_none;
PyrSlot o_emptyarray, o_onenilarray, o_argnamethis;


void initSymbols()
{
	s_new = getsym("new");
	s_ref = getsym("Ref");
	s_none = getsym("none");
	s_object = getsym("Object");
	s_this = getsym("this");
	s_super = getsym("super");

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
	s_objptr = getsym("ObjectPointer");
	s_string = getsym("String");
	s_magnitude = getsym("Magnitude");
	s_number = getsym("Number");
	s_simple_number = getsym("SimpleNumber");
	s_collection = getsym("Collection");
	//s_ordered_collection = getsym("OrderedCollection");
	s_arrayed_collection = getsym("ArrayedCollection");
	s_sequenceable_collection = getsym("SequenceableCollection");
	s_boolean = getsym("Boolean");
	s_signal = getsym("Signal");
	s_wavetable = getsym("Wavetable");
	//s_signalnode = getsym("SignalNode");
	s_rawarray = getsym("RawArray");
	s_int8array = getsym("Int8Array");
	s_int16array = getsym("Int16Array");
	s_int32array = getsym("Int32Array");
	s_symbolarray = getsym("SymbolArray");
	s_floatarray = getsym("FloatArray");
	s_doublearray = getsym("DoubleArray");
	s_complex = getsym("Complex");
	s_point = getsym("Point");
	s_rect = getsym("Rect");
	s_absfunc = getsym("AbstractFunction");
	s_func = getsym("Function");
	s_stream = getsym("Stream");
	s_process = getsym("Process");
	s_main = getsym("Main");
	s_thread = getsym("Thread");
	s_routine = getsym("Routine");
	s_task = getsym("Task");
	s_prstart = getsym("prStart");
	s_interpreter = getsym("Interpreter");
	s_finalizer = getsym("Finalizer");
	s_awake = getsym("awake");
	s_appclock = getsym("AppClock");
	s_systemclock = getsym("SystemClock");

	s_linear = getsym("linear");
	s_exponential = getsym("exponential");
	s_gate = getsym("gate");

	//s_dsp = getsym("DSP");
	//s_dspsettings = getsym("DSPSettings");
	s_synth = getsym("Synth");
	s_spawn = getsym("BasicSpawn");
	s_environment = getsym("Environment");
	s_event = getsym("Event");
	s_hardwaresetup = getsym("hardwareSetup");
	s_shutdown = getsym("shutdown");

	s_nocomprendo = getsym("doesNotUnderstand");

	s_curProcess = getsym("thisProcess");
	s_curThread = getsym("thisThread");
	s_curMethod = getsym("thisMethod");
	s_curBlock = getsym("thisFunctionDef");
	s_curClosure = getsym("thisFunction");
	//s_sampleRate = getsym("gSR");
	//s_logicalClock = getsym("gTime");
	//s_audioClock = getsym("gAudioTime");
	s_audio = getsym("audio");
	s_control = getsym("control");
	s_scalar = getsym("scalar");
	s_run = getsym("run");
	s_startup = getsym("startup");
	s_docmdline = getsym("doCmdLine");
	s_next = getsym("next");
	s_value = getsym("value");
	s_performList = getsym("performList");
	s_superPerformList = getsym("superPerformList");
	s_at = getsym("at");
	s_put = getsym("put");

	s_series = getsym("prSimpleNumberSeries");
	s_copyseries = getsym("copySeries");
	s_putseries = getsym("putSeries");

	s_ugen = getsym("UGen");
	s_outputproxy = getsym("OutputProxy");
	s_env = getsym("Env");

	s_envirGet = getsym("envirGet");
	s_envirPut = getsym("envirPut");

	o_nil.utag = tagNil;		o_nil.ui = 0;
	o_false.utag = tagFalse;	o_false.ui = 0;
	o_true.utag = tagTrue;		o_true.ui = 0;

	SetFloat(&o_pi, pi);
	SetFloat(&o_twopi, twopi);
	SetFloat(&o_fhalf, .5);
	SetFloat(&o_fnegone, -1.);
	SetFloat(&o_fzero, 0.);
	SetFloat(&o_fone, 1.);
	SetFloat(&o_ftwo, 2.);
	SetInt(&o_negtwo, -2);
	SetInt(&o_negone, -1);
	SetInt(&o_zero, 0);
	SetInt(&o_one, 1);
	SetInt(&o_two, 2);
	SetSymbol(&o_none, s_none);
#ifdef SC_WIN32
  {
    double a = 0.0;
    double b = 1.0/a;
    SetFloat(&o_inf, b);
  }
#else
 	SetFloat(&o_inf, INFINITY);
#endif

	gSpecialValues[svNil] = o_nil.uf;
	gSpecialValues[svFalse] = o_false.uf;
	gSpecialValues[svTrue] = o_true.uf;
	gSpecialValues[svNegOne] = o_negone.uf;
	gSpecialValues[svZero] = o_zero.uf;
	gSpecialValues[svOne] = o_one.uf;
	gSpecialValues[svTwo] = o_two.uf;
	gSpecialValues[svFHalf] = o_fhalf.uf;
	gSpecialValues[svFNegOne] = o_fnegone.uf;
	gSpecialValues[svFZero] = o_fzero.uf;
	gSpecialValues[svFOne] = o_fone.uf;
	gSpecialValues[svFTwo] = o_ftwo.uf;
	gSpecialValues[svInf] = o_inf.uf;

	gFormatElemSize[obj_notindexed] = sizeof(PyrSlot);
	gFormatElemSize[obj_slot  ] = sizeof(PyrSlot);
	gFormatElemSize[obj_double] = sizeof(double);
	gFormatElemSize[obj_float ] = sizeof(float);
	gFormatElemSize[obj_int32  ] = sizeof(int32);
	gFormatElemSize[obj_int16 ] = sizeof(int16);
	gFormatElemSize[obj_int8  ] = sizeof(int8);
	gFormatElemSize[obj_char  ] = sizeof(char);
	gFormatElemSize[obj_symbol  ] = sizeof(PyrSymbol*);

	gFormatElemCapc[obj_notindexed] = sizeof(PyrSlot) / sizeof(PyrSlot);
	gFormatElemCapc[obj_slot  ] = sizeof(PyrSlot) / sizeof(PyrSlot);
	gFormatElemCapc[obj_double] = sizeof(PyrSlot) / sizeof(double);
	gFormatElemCapc[obj_float ] = sizeof(PyrSlot) / sizeof(float);
	gFormatElemCapc[obj_int32  ] = sizeof(PyrSlot) / sizeof(int32);
	gFormatElemCapc[obj_int16 ] = sizeof(PyrSlot) / sizeof(int16);
	gFormatElemCapc[obj_int8  ] = sizeof(PyrSlot) / sizeof(int8);
	gFormatElemCapc[obj_char  ] = sizeof(PyrSlot) / sizeof(char);
	gFormatElemCapc[obj_symbol] = sizeof(PyrSlot) / sizeof(PyrSymbol*);

	gFormatElemTag[obj_notindexed] = -1;
	gFormatElemTag[obj_slot  ] = -1;
	gFormatElemTag[obj_double] = 0;
	gFormatElemTag[obj_float ] = 0;
	gFormatElemTag[obj_int32  ] = tagInt;
	gFormatElemTag[obj_int16 ] = tagInt;
	gFormatElemTag[obj_int8  ] = tagInt;
	gFormatElemTag[obj_char  ] = tagChar;
	gFormatElemTag[obj_symbol] = tagSym;
}

const char *slotSymString(PyrSlot* slot)
{
	switch (slot->utag) {
		case tagObj : return slot->uo->classptr->name.us->name;
		case tagInt : return "Integer";
		case tagChar : return "Char";
		case tagSym : return slot->us->name;
		case tagNil : return "Nil";
		case tagFalse : return "False";
		case tagTrue : return "True";
		default : return "<float>";
	}
}

PyrClass* newClassObj(PyrClass *classObjSuperClass,
	PyrSymbol* className, PyrSymbol* superClassName,
	int numInstVars, int numClassVars, int numConsts, int numInstMethods,
	int instFormat, int instFlags)
{
	PyrClass* classobj, *superclassobj;
	PyrObject* array;
	PyrSymbolArray *symarray;
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
	classobj->size = (sizeof(PyrClass) - sizeof(PyrObjectHdr))/sizeof(PyrSlot);
	classobj->prev = classobj->next = NULL;
	classobj->obj_flags = obj_immutable ;
	classobj->obj_format = obj_notindexed ;
	classobj->gc_color = obj_permanent ;
	classobj->obj_sizeclass = LOG2CEIL(classobj->size);
	classobj->classptr = classObjSuperClass;


	// append to the list of classes
	if (gClassList) SetObject(&classobj->nextclass, gClassList);
	else SetNil(&classobj->nextclass);

	gClassList = classobj;

	className->u.classobj = classobj;
	//postfl("> '%s' %d %d\n", className->name, className, classobj);

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
	//if (numClassVars) post("%16s %4d %4d\n", className->name, gNumClassVars, numClassVars);
	gNumClassVars += numClassVars;
	return classobj;
}

void reallocClassObj(PyrClass* classobj,
	int numInstVars, int numClassVars, int numConsts, int numMethods,
	int instFormat, int instFlags)
{
	PyrObject* array;
	PyrSymbolArray *symarray;
	PyrClass *superclassobj;

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
		//post("reallocClassObj %s numInstVars %d\n", classobj->name.us->name, numInstVars);
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
		//post("reallocClassObj %s numClassVars %d\n", classobj->name.us->name, numClassVars);
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
		//post("reallocClassObj %s numConsts %d\n", classobj->name.us->name, numConsts);
		symarray = newPyrSymbolArray(NULL, numConsts, obj_permanent | obj_immutable, false);
		SetObject(&classobj->constNames, symarray);

		array = newPyrArray(NULL, numConsts, obj_permanent | obj_immutable, false);
		SetObject(&classobj->constValues, array);
		nilSlots(array->slots, numConsts);
	} else {
		SetNil(&classobj->constNames);
		SetNil(&classobj->constValues);
	}

	superclassobj = classobj->superclass.us->u.classobj;

	SetInt(&classobj->instanceFormat, instFormat);
	SetInt(&classobj->instanceFlags, instFlags);
	SetInt(&classobj->classIndex, -1);
}

void fixClassArrays(PyrClass *classobj);
void fixClassArrays(PyrClass *classobj)
{
	if (classobj->methods.utag == tagObj) classobj->methods.uo->classptr = class_array;
	if (classobj->instVarNames.utag == tagObj) classobj->instVarNames.uo->classptr = class_symbolarray;
	if (classobj->classVarNames.utag == tagObj) classobj->classVarNames.uo->classptr = class_symbolarray;
	if (classobj->iprototype.utag == tagObj) classobj->iprototype.uo->classptr = class_array;
	if (classobj->cprototype.utag == tagObj) classobj->cprototype.uo->classptr = class_array;
}

int numInstVars(PyrClass* classobj)
{
	int res;
	if (IsNil(&classobj->instVarNames)) res = 0;
	else res = classobj->instVarNames.uo->size;
	return res;
}

int numClassVars(PyrClass* classobj)
{
	int res;
	if (IsNil(&classobj->classVarNames)) res = 0;
	else res = classobj->classVarNames.uo->size;
	return res;
}


void objAddIndexedSlotGrow(PyrSlot *arraySlot, PyrSlot *addSlot);
void objAddIndexedSlotGrow(PyrSlot *arraySlot, PyrSlot *addSlot)
{
	PyrObject *obj;
	if (IsNil(arraySlot)) {
		PyrObject *newobj = (PyrObject*)newPyrArray(NULL, 1, obj_permanent | obj_immutable, false);
		SetObject(arraySlot, newobj);
		obj = newobj;
	} else {
		obj = arraySlot->uo;
		if (obj->size >= ARRAYMAXINDEXSIZE(obj)) {
			//post("objAddIndexedSlotGrow\n");
			PyrObject *newobj = (PyrObject*)newPyrArray(NULL, obj->size * 2, obj_permanent | obj_immutable, false);
			memcpy(newobj->slots, obj->slots, obj->size * sizeof(PyrSlot));
			newobj->size = obj->size;
			SetObject(arraySlot, newobj);
			pyr_pool_runtime->Free((void*)obj);
			obj = newobj;
		}
	}
	slotCopy(&obj->slots[obj->size++], addSlot);
}

void addMethod(PyrClass *classobj, PyrMethod *method)
{
	PyrSlot slot;
	SetObject(&slot, method);
	objAddIndexedSlotGrow(&classobj->methods, &slot);
}



PyrMethod* classFindDirectMethod(PyrClass* classobj, PyrSymbol *name)
{
	PyrMethod *method;
	PyrSlot *methods;
	int i, numMethods;
	if (IsNil(&classobj->methods)) return NULL;
	methods = classobj->methods.uo->slots;
	numMethods = classobj->methods.uo->size;
	for (i=0; i<numMethods; ++i) {
		method = methods[i].uom;
		if (method->name.us == name) break;
	}
	if (i>=numMethods) method = NULL;
	return method;
}

int numSuperInstVars(PyrClass *superclassobj)
{
	int superinstvars = 0;
	if (superclassobj) {
		if (superclassobj->iprototype.utag == tagObj) {
			superinstvars = superclassobj->iprototype.uo->size;
		}
	}
	return superinstvars;
}

bool classFindInstVar(PyrClass* classobj, PyrSymbol *name, int *index)
{
	PyrSymbolArray *ivnames;
	PyrSymbol *ivname;
	int i;
	if (NotNil(&classobj->instVarNames)) {
		ivnames = classobj->instVarNames.uosym;
		if (ivnames) {
			for (i=0; i<ivnames->size; ++i) {
				ivname = ivnames->symbols[i];
				if (ivname == name) {
					//numsupervars = numSuperInstVars(classobj->superclass.us->u.classobj);
					//*index = numsupervars + i;
					*index = i;
					return true;
				}
			}
		}
	}
	return false;
}

int instVarOffset(const char *classname, const char *instvarname)
{
	PyrSymbol *instvarsymbol, *classsymbol;
	PyrClass* classobj;
	int index;
	char c;

	classsymbol = getsym(classname);
	instvarsymbol = getsym(instvarname);

	c = classname[0];
	if (!(c >= 'A' && c <= 'Z')) return -1;

	classobj = classsymbol->u.classobj;
	if (!classobj) return -1;
	if (!classFindInstVar(classobj, instvarsymbol, &index)) return -1;
	return index;
}

int classVarOffset(const char *classname, const char *classvarname, PyrClass** classobj)
{
	PyrSymbol *classvarsymbol, *classsymbol;
	int index;
	char c;

	classsymbol = getsym(classname);
	classvarsymbol = getsym(classvarname);

	c = classname[0];
	if (!(c >= 'A' && c <= 'Z')) return -1;

	*classobj = classsymbol->u.classobj;
	if (!*classobj) return -1;
	if (!classFindClassVar(classobj, classvarsymbol, &index)) return -1;
	return index;
}

bool classFindClassVar(PyrClass** classobj, PyrSymbol *name, int *index)
{
	PyrSymbolArray *cvnames;
	PyrSymbol *cvname;
	int i, j;
	char *classname;
	PyrClass* localclassobj = *classobj;
	// if this is a Metaclass then we need to search its normal Class for
	// the class vars
	classname = localclassobj->name.us->name;
	if (strncmp(classname, "Meta_", 5) == 0) {
		localclassobj = getsym(classname+5)->u.classobj;
	}
	for (j=0; localclassobj; ++j) {
		if (NotNil(&localclassobj->classVarNames)) {
			cvnames = localclassobj->classVarNames.uosym;
			if (cvnames) {
				for (i=0; i<cvnames->size; ++i) {
					cvname = cvnames->symbols[i];
					if (cvname == name) {
						*classobj = localclassobj;
						*index = i;
						return true;
					}
				}
			}
		}
		if (localclassobj->superclass.utag == tagSym) {
			localclassobj = localclassobj->superclass.us->u.classobj;
		} else {
			localclassobj = NULL;
		}
	}
	return false;
}

bool classFindConst(PyrClass** classobj, PyrSymbol *name, int *index)
{
	PyrSymbolArray *knames;
	PyrSymbol *kname;
	int i, j;
	char *classname;
	PyrClass* localclassobj = *classobj;
	// if this is a Metaclass then we need to search its normal Class for
	// the class vars
	classname = localclassobj->name.us->name;
	if (strncmp(classname, "Meta_", 5) == 0) {
		localclassobj = getsym(classname+5)->u.classobj;
	}
	for (j=0; localclassobj; ++j) {
		if (NotNil(&localclassobj->constNames)) {
			knames = localclassobj->constNames.uosym;
			if (knames) {
				for (i=0; i<knames->size; ++i) {
					kname = knames->symbols[i];
					if (kname == name) {
						*classobj = localclassobj;
						*index = i;
						return true;
					}
				}
			}
		}
		if (localclassobj->superclass.utag == tagSym) {
			localclassobj = localclassobj->superclass.us->u.classobj;
		} else {
			localclassobj = NULL;
		}
	}
	return false;
}


void buildClassTree()
{
	// after all classes are compiled this function builds the tree and
	// indexes the classes
	PyrClass *classobj, *superclassobj;

	// count subclasses
	//postfl("count subclasses\n");
	classobj = gClassList;
	while (classobj) {
		superclassobj = classobj->superclass.us->u.classobj;
		if (superclassobj) {
			//postfl("     superclassobj %s\n", superclassobj->name.us->name);
			superclassobj->subclasses.ui ++;
		}
		classobj = classobj->nextclass.uoc;
	}
	// allocate subclass arrays
	//postfl("allocate subclass arrays\n");
	classobj = gClassList;
	while (classobj) {
		int numSubclasses;
		numSubclasses = classobj->subclasses.ui;
		//postfl("  %s %d\n", classobj->name.us->name, numSubclasses);
		if (numSubclasses) {
			classobj->subclasses.uo = (PyrObject*)newPyrArray(NULL, numSubclasses, obj_permanent | obj_immutable, false);
			classobj->subclasses.utag = tagObj;
			classobj->subclasses.uo->size = 0;
		} else {
			SetNil(&classobj->subclasses);
		}
		classobj = classobj->nextclass.uoc;
	}
	// fill in subclass arrays
	//postfl("fill in subclass arrays\n");
	classobj = gClassList;
	while (classobj) {
		//postfl("  %s\n", classobj->name.us->name);
		superclassobj = classobj->superclass.us->u.classobj;
		if (superclassobj) {
			objAddIndexedObject(superclassobj->subclasses.uo, (PyrObject*)classobj);
			//postfl("     superclassobj %s %d\n", superclassobj->name.us->name,
			//	superclassobj->subclasses.uo->size);
		}
		classobj = classobj->nextclass.uoc;
	}

	// alpha sort the classes via insertion sort
	{
		PyrClass *alpha, *prev, *cur, *subj;
		alpha = gClassList;
		gClassList = gClassList->nextclass.uoc;
		SetNil(&alpha->nextclass);
		while (gClassList) {
			subj = gClassList;
			gClassList = gClassList->nextclass.uoc;
			prev = NULL;
			cur = alpha;
			while (cur && strcmp(subj->name.us->name, cur->name.us->name)>0) {
				prev = cur;
				cur = cur->nextclass.uoc;
			}
			SetObjectOrNil(&subj->nextclass, (PyrObject*)cur);
			if (prev) SetObjectOrNil(&prev->nextclass, (PyrObject*)subj);
			else alpha = subj;
		}
		gClassList = alpha;
	}
}

void indexClassTree(PyrClass *classobj, int numSuperMethods)
{
	PyrObject *subclasses;
	PyrObject *methods;
	int i, numMethods;

	if (!classobj) return;
	subclasses = classobj->subclasses.uo;

	SetInt(&classobj->classIndex, 	gNumClasses);
	gNumClasses ++;
	methods = classobj->methods.uo;
	numMethods = methods ? methods->size : 0;
	numMethods = numSuperMethods + numMethods;
	if (subclasses) {
		for (i=0; i<subclasses->size; ++i) {
			indexClassTree(subclasses->slots[i].uoc, numMethods);
		}
	}
	SetInt(&classobj->maxSubclassIndex,  gNumClasses - 1);
}

void findDiscrepancy();
void findDiscrepancy()
{
	PyrClass *classobjA, *classobjB;

	classobjA = gClassList;
	while (classobjA) {
		classobjB = classobjA->nextclass.uoc;;
		while (classobjB) {
			if (classobjA->name.us == classobjB->name.us) {
				post("duplicate %s\n", classobjA->name.us->name);
			}
			classobjB = classobjB->nextclass.uoc;
		}
		classobjA = classobjA->nextclass.uoc;
	}
}


static void indent(int n)
{
	for (int i=0; i<n; ++i) {
		post("  ");
	}
}

void postClassTree(PyrClass *classobj, int level)
{
	PyrObject *subclasses;
	int i;

	//post("%4d  ", classobj->classIndex.ui);
	indent(level);
	post("%s\n", classobj->name.us->name);

	if (classobj == class_class) {
		indent(level+1);
		post("      [.. all metaclasses ..]\n");
	} else {
		subclasses = classobj->subclasses.uo;
		if (subclasses) {
			// determine if can put on one line
			bool oneline = subclasses->size <= 5;
			for (i=0; oneline && i<subclasses->size; ++i) {
				PyrClass *subclassobj = subclasses->slots[i].uoc;
				if (subclassobj->subclasses.uo) oneline = false;
			}
			if (oneline) {
				indent(level+1);
				post("[");
				for (i=0; i<subclasses->size; ++i) {
					PyrClass *subclassobj = subclasses->slots[i].uoc;
					post(" %s", subclassobj->name.us->name);
				}
				post(" ]\n");
			} else {
				indent(level);
				post("[\n");
				for (i=0; i<subclasses->size; ++i) {
					postClassTree(subclasses->slots[i].uoc, level+1);
				}
				indent(level);
				post("]\n");
			}
		}
	}
}


void setSelectorFlags()
{
	PyrClass *classobj;
	PyrObject *methods;
	PyrMethod *method;
	int i;

	classobj = gClassList;
	while (classobj) {
		methods = classobj->methods.uo;
		if (methods) {
			for (i=0; i<methods->size; ++i) {
				method = methods->slots[i].uom;
				method->name.us->flags |= sym_Selector;
				//if (method->methType == methRedirect) {
				//	post("rd: %3d %s:%s\n", k++, classobj->name.us->name,
				//		method->name.us->name);
				//}
			}
		}
		classobj = classobj->nextclass.uoc;
	}
	// count selectors
	gNumSelectors = 0;
	SymbolTable* symbolTable = gMainVMGlobals->symbolTable;
	for (int i=0; i<symbolTable->TableSize(); ++i) {
		PyrSymbol *sym = symbolTable->Get(i);
		if (sym && (sym->flags & sym_Selector)) {
			sym->u.index = gNumSelectors++;
		}
	}
	//post("gNumSelectors %d\n", gNumSelectors);
}

// the chunky stuff can be commented back in for implementing a better
// compression scheme. The Q&D method works fine for my small class tree for now.

typedef struct {
	PyrSymbol *selector;
	int minClassIndex;
	int maxClassIndex;
	int rowWidth;
	//int largestChunk;
	//int chunkOffset;
	int selectorIndex;
	int population;
	int rowOffset;
} ColumnDescriptor;

int compareColDescs(const void *va, const void *vb);
int compareColDescs(const void *va, const void *vb)
{
        ColumnDescriptor *a = (ColumnDescriptor*)va;
        ColumnDescriptor *b = (ColumnDescriptor*)vb;
	int diff;
	//diff = b->largestChunk - a->largestChunk;
	//if (diff != 0) return diff;
	diff = b->rowWidth - a->rowWidth;
	if (diff != 0) return diff;
	//diff = b->chunkOffset - a->chunkOffset;
	diff = b->minClassIndex - a->minClassIndex;
	return diff;
}

#define CHECK_METHOD_LOOKUP_TABLE_BUILD_TIME 0
#if CHECK_METHOD_LOOKUP_TABLE_BUILD_TIME
double elapsedTime();
#endif

void buildBigMethodMatrix()
{
	PyrMethod **bigTable, **temprow, **row;
	PyrClass *classobj, **classes;
	int i, j, k;
	int popSum, widthSum;
	int rowOffset, freeIndex;
	int rowTableSize;
	int bigTableSize;
	int numSelectors = gNumSelectors;
	int numClasses = gNumClasses;
	//post("allocate arrays\n");

#if CHECK_METHOD_LOOKUP_TABLE_BUILD_TIME
	double t0 = elapsedTime();
#endif

	// pyrmalloc:
	// lifetime: kill after compile
	bigTableSize = numSelectors * numClasses;
	//post("bigTableSize %d %d %d\n", bigTableSize, numSelectors, numClasses);
	ColumnDescriptor *sels = (ColumnDescriptor*)pyr_pool_compile->Alloc(numSelectors * sizeof(ColumnDescriptor));
	MEMFAIL(sels);
	bigTable = (PyrMethod**)pyr_pool_compile->Alloc(bigTableSize * sizeof(PyrMethod*));
	MEMFAIL(bigTable);
	classes = (PyrClass**)pyr_pool_compile->Alloc(numClasses * sizeof(PyrClass*));
	MEMFAIL(classes);

	classobj = gClassList;
	while (classobj) {
		classes[classobj->classIndex.ui] = classobj;
		classobj = classobj->nextclass.uoc;
	}

	fillClassRow(class_object, bigTable);

	{
		int numentries = 0;
		for (int z = 0; z<bigTableSize; ++z) {
			if (bigTable[z] != NULL) numentries ++ ;
		}
		post("numentries = %d / %d = %.2g\n", numentries, bigTableSize, (double)numentries/(double)bigTableSize);
	}

	// fill selector table
	//post("fill selector table\n");
	SymbolTable* symbolTable = gMainVMGlobals->symbolTable;
	for (int i=0, j=0; i<symbolTable->TableSize(); ++i) {
		PyrSymbol *sym = symbolTable->Get(i);
		if (sym && (sym->flags & sym_Selector)) {
			sels[j++].selector = sym;
		}
	}

	// calc row stats
	//post("calc row stats\n");
	for (i=0; i<numSelectors; ++i) {
		//postfl("%3d %s\n", i, sels[i].selector->name);
		sels[i].minClassIndex = INT_MAX;
		sels[i].maxClassIndex = 0;
		//sels[i].largestChunk = 0;
		//sels[i].chunkOffset = 0;
		sels[i].selectorIndex = i;
		sels[i].population = 0;
	}
		//chunkSize = 0;
		//chunkOffset = 0;
	PyrMethod** methodPtr = bigTable;
	for (j=0; j<numClasses; ++j) {
		for (i=0; i<numSelectors; ++i) {
			PyrMethod* method = *methodPtr++;
			if (method) {
				//classobj = method->ownerclass.uoc;
				if (j > sels[i].maxClassIndex) {
					sels[i].maxClassIndex = j;
				}
				if (j < sels[i].minClassIndex) {
					sels[i].minClassIndex = j;
				}
				sels[i].population ++;
				//if (chunkSize == 0) chunkOffset = j;
				//chunkSize++;
				//postfl("    %8s %3d %3d %3d %3d\n", classobj->name.us->name, i, j,
				//	chunkSize, classobj->classIndex.ui);
			//} else {
				//if (chunkSize > sels[i].largestChunk) {
				//	sels[i].largestChunk = chunkSize;
				//	sels[i].chunkOffset = chunkOffset;
				//}
				//chunkSize = 0;
			}
		}
	}
	for (i=0; i<numSelectors; ++i) {
		//if (chunkSize > sels[i].largestChunk) {
		//	sels[i].largestChunk = chunkSize;
		//	sels[i].chunkOffset = chunkOffset;
		//}
		sels[i].rowWidth = sels[i].maxClassIndex - sels[i].minClassIndex + 1;
	}

	//post("qsort\n");
	// sort rows by largest chunk, then by width, then by chunk offset
	//qsort(sels, numSelectors, sizeof(ColumnDescriptor), (std::_compare_function)compareColDescs);
	qsort(sels, numSelectors, sizeof(ColumnDescriptor), compareColDescs);
	// bin sort the class rows to the new ordering
	//post("reorder rows\n");
	// pyrmalloc:
	// lifetime: kill after compile
	temprow = (PyrMethod**)pyr_pool_compile->Alloc(numSelectors * sizeof(PyrMethod*));
	MEMFAIL(temprow);
	for (j=0; j<numClasses; ++j) {
		row = bigTable + j * numSelectors;
		memcpy(temprow, row, numSelectors * sizeof(PyrMethod*));
		for (i=0; i<numSelectors; ++i) {
			row[i] = temprow[sels[i].selectorIndex];
		}
	}
	pyr_pool_compile->Free(temprow);

	//post("calc row offsets %d\n", numSelectors);
	widthSum = 0;
	popSum = 0;
	freeIndex = 0;
	rowOffset = -1;
	for (i=0; i<numSelectors; ++i) {
		widthSum += sels[i].rowWidth;
		popSum += sels[i].population;
		rowOffset = sc_max(rowOffset+1, freeIndex - sels[i].minClassIndex);
		freeIndex = rowOffset + sels[i].maxClassIndex + 1;
		sels[i].rowOffset = rowOffset;
		sels[i].selector->u.index = rowOffset;
		//post("%3d  %24s %3d %5d %5d\n", i, sels[i].selector->name,
		//	sels[i].rowWidth, rowOffset, freeIndex);
	}
	//post("alloc row table %d\n", freeIndex);
	rowTableSize = (freeIndex + numClasses)  * sizeof(PyrMethod*);
	gRowTable = (PyrMethod**)pyr_pool_runtime->Alloc(rowTableSize);
	MEMFAIL(gRowTable);
	memset(gRowTable, 0, (freeIndex + numClasses) * sizeof(PyrMethod*));

	//post("fill compressed table\n");
	//{ FILE* fp;
	// newPyrMethod
	//fp = fopen("meth table", "w");
	for (i=0; i<numSelectors; ++i) {
		int offset, maxwidth;
		offset = sels[i].rowOffset + sels[i].minClassIndex;
		maxwidth = offset + sels[i].rowWidth;
		row = bigTable + sels[i].minClassIndex * numSelectors + i;
		PyrMethod **table = gRowTable;
		for (j=offset,k=0; j<maxwidth; ++j, k+=numSelectors) {
			table[j] = row[k];
		}
	}
	//fclose(fp);
	//}

	// having the method ptr always be valid saves a branch in SendMessage()
	for (i=0; i<freeIndex + numClasses; ++i) {
		if (gRowTable[i] == NULL) gRowTable[i] = gNullMethod;
	}

	//post("freeIndex %d\n", freeIndex);
	//post("widthSum %d\n", widthSum);
	//post("popSum %d\n", popSum);

#if CHECK_METHOD_LOOKUP_TABLE_BUILD_TIME
	post("building table took %.3g seconds\n", elapsedTime() - t0);
	{
		int numFilled = 0;
		for (i=0; i<rowTableSize/sizeof(PyrMethod*); ++i) {
			if (gRowTable[i] != gNullMethod) numFilled++;
		}
		post("Filled %d of %d = %f percent\n", numFilled, rowTableSize/sizeof(PyrMethod*),
			100. * (double)numFilled/(rowTableSize/sizeof(PyrMethod*)));
	}
#endif
	post("\t%d method selectors, %d classes\n", numSelectors, numClasses);
	post("\tmethod table size %d bytes, ", rowTableSize);
	post("big table size %d\n", numSelectors * numClasses * sizeof(PyrMethod*));
	//postfl("%08X %08X %08X\n", classes, bigTable, sels);
/*
	// not necessary since the entire pool will be freed..
	pyr_pool_compile->Free(classes);
	pyr_pool_compile->Free(bigTable);
	pyr_pool_compile->Free(sels);
*/
}

void fillClassRow(PyrClass *classobj, PyrMethod** bigTable)
{
	PyrObject *subclasses, *methods;
	PyrClass* superclassobj;
	PyrMethod **superrow, **myrow, *method;
	int i, selectorIndex;
	int bigTableSize;

	bigTableSize = gNumSelectors * gNumClasses;

	myrow = bigTable + classobj->classIndex.ui * gNumSelectors;
	superclassobj = classobj->superclass.us->u.classobj;
	if (superclassobj) {
		superrow = bigTable + superclassobj->classIndex.ui * gNumSelectors;
		memcpy(myrow, superrow, gNumSelectors * sizeof(PyrMethod*));
	} else {
		memset(myrow, 0, gNumSelectors * sizeof(PyrMethod*));
	}

	methods = classobj->methods.uo;
	if (methods) {
		//postfl("        %d\n", methods->size);
		for (i=0; i<methods->size; ++i) {
			method = methods->slots[i].uom;
			selectorIndex = method->name.us->u.index;
			myrow[selectorIndex] = method;
		}
	}

	subclasses = classobj->subclasses.uo;
	if (subclasses) {
		for (i=0; i<subclasses->size; ++i) {
			fillClassRow(subclasses->slots[i].uoc, bigTable);
		}
	}
}


bool funcFindArg(PyrBlock* func, PyrSymbol *name, int *index)
{
	int i;
	for (i=0; i<func->argNames.uosym->size; ++i) {
		if (func->argNames.uosym->symbols[i] == name) {
			*index = i;
			return true;
		}
	}
	return false;
}

bool funcFindVar(PyrBlock* func, PyrSymbol *name, int *index)
{
	int i;
	for (i=0; i<func->varNames.uosym->size; ++i) {
		if (func->varNames.uosym->symbols[i] == name) {
			*index = i;
			return true;
		}
	}
	return false;
}

PyrClass* makeIntrinsicClass(PyrSymbol *className, PyrSymbol *superClassName,
	int numInstVars, int numClassVars)
{
	PyrClass *superClass = NULL;
  PyrClass *metaSuperClass = NULL;
	PyrSymbol *metaClassName = NULL;
	PyrSymbol *metaSuperClassName = NULL;
	PyrClass *classobj = NULL;
  PyrClass *metaclassobj = NULL;
	int superInstVars;

	//postfl("makeIntrinsicClass '%s'\n", className->name);
	if (superClassName) {
		superClass = superClassName->u.classobj;
		if (!superClass) {
			error("Can't find superclass '%s' of '%s'\n", superClassName->name,
				className->name);
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
	metaclassobj = newClassObj( class_class,
		metaClassName, metaSuperClassName,
		classClassNumInstVars, 0, 0, 0, obj_notindexed, 0);
	metaclassobj->classFlags.ui |= classIsIntrinsic;

	if (metaSuperClassName && classClassNumInstVars) {
		memcpy(metaclassobj->iprototype.uo->slots, metaSuperClass->iprototype.uo->slots,
			sizeof(PyrSlot) * classClassNumInstVars);
		memcpy(metaclassobj->instVarNames.uosym->symbols, metaSuperClass->instVarNames.uosym->symbols,
			sizeof(PyrSlot) * classClassNumInstVars);
		metaclassobj->iprototype.uo->size = classClassNumInstVars;
		metaclassobj->instVarNames.uo->size = classClassNumInstVars;
		//dumpObject((PyrObject*)metaclassobj);
	}

	classobj = newClassObj(metaclassobj,
		className, superClassName,
		numInstVars + superInstVars, numClassVars, 0, 0, obj_notindexed, 0);
	classobj->classFlags.ui |= classIsIntrinsic;

	//postfl("%s:%s  : %d\n", className->name, superClassName->name, superInstVars);
	if (superClass && superInstVars) {
		memcpy(classobj->iprototype.uo->slots, superClass->iprototype.uo->slots,
			sizeof(PyrSlot) * superInstVars);
		memcpy(classobj->instVarNames.uosym->symbols, superClass->instVarNames.uosym->symbols,
			sizeof(PyrSlot) * superInstVars);
		classobj->iprototype.uo->size = superInstVars;
		classobj->instVarNames.uo->size = superInstVars;
	}

	return classobj;
}

void addIntrinsicVar(PyrClass *classobj, const char *varName, PyrSlot *slot)
{
	//postfl("%s  %s  %d\n", classobj->name.us->name, varName,
	//	classobj->instVarNames.uo->size);
	objAddIndexedSymbol(classobj->instVarNames.uosym, getsym(varName));
	objAddIndexedSlot(classobj->iprototype.uo, slot);
}

void addIntrinsicClassVar(PyrClass *classobj, const char *varName, PyrSlot *slot);
void addIntrinsicClassVar(PyrClass *classobj, const char *varName, PyrSlot *slot)
{
	//postfl("%s  %s  %d\n", classobj->name.us->name, varName,
	//	classobj->instVarNames.uo->size);
	objAddIndexedSymbol(classobj->classVarNames.uosym, getsym(varName));
	objAddIndexedSlot(classobj->cprototype.uo, slot);
}

void initClasses()
{
	PyrClass *class_object_meta;
	PyrMethodRaw *methraw;

	// BOOTSTRAP THE OBJECT HIERARCHY

	gNumClassVars = 0;
	gClassList = NULL;
	gNullMethod = newPyrMethod();
	gNullMethod->name.us = 0;
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
	memcpy(class_object_meta->iprototype.uo->slots, class_class->iprototype.uo->slots,
		sizeof(PyrSlot) * classClassNumInstVars);
	memcpy(class_object_meta->instVarNames.uosym->symbols, class_class->instVarNames.uosym->symbols,
		sizeof(PyrSymbol*) * classClassNumInstVars);

	memcpy(class_class->classptr->iprototype.uo->slots, class_class->iprototype.uo->slots,
		sizeof(PyrSlot) * classClassNumInstVars);
	memcpy(class_class->classptr->instVarNames.uosym->symbols, class_class->instVarNames.uosym->symbols,
		sizeof(PyrSymbol*) * classClassNumInstVars);


	// OK the next thing I need is arrays..
	class_collection = makeIntrinsicClass(s_collection, s_object, 0, 0);
	class_sequenceable_collection = makeIntrinsicClass(s_sequenceable_collection, s_collection, 0, 0);

	class_arrayed_collection = makeIntrinsicClass(s_arrayed_collection, s_sequenceable_collection, 0, 0);
	class_array = makeIntrinsicClass(s_array, s_arrayed_collection, 0, 0);
		SetInt(&class_array->instanceFormat, obj_slot);
		class_array->classFlags.ui |= classHasIndexableInstances;

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
		//addIntrinsicVar(class_method, "byteMeter", &o_zero);
		//addIntrinsicVar(class_method, "callMeter", &o_zero);

	class_frame = makeIntrinsicClass(s_frame, s_object, 0, 0);
		class_frame->classFlags.ui |= classHasIndexableInstances;
		//addIntrinsicVar(class_frame, "method", &o_nil);
		//addIntrinsicVar(class_frame, "caller", &o_nil);
		//addIntrinsicVar(class_frame, "context", &o_nil);
		//addIntrinsicVar(class_frame, "homeContext", &o_nil);
		//addIntrinsicVar(class_frame, "ip", &o_nil);

	class_process = makeIntrinsicClass(s_process, s_object, 5, 0);
		addIntrinsicVar(class_process, "classVars", &o_nil);
		addIntrinsicVar(class_process, "interpreter", &o_nil);
		addIntrinsicVar(class_process, "curThread", &o_nil);
		addIntrinsicVar(class_process, "mainThread", &o_nil);
		addIntrinsicVar(class_process, "schedulerQueue", &o_nil);

	class_interpreter = makeIntrinsicClass(s_interpreter, s_object, 29, 0);
		addIntrinsicVar(class_interpreter, "cmdLine", &o_nil);
		addIntrinsicVar(class_interpreter, "context", &o_nil);
		for (int i=0; i<26; ++i) {
			char name[2];
			name[0] = 'a' + i;
			name[1] = 0;
			addIntrinsicVar(class_interpreter, name, &o_nil);
		}
		addIntrinsicVar(class_interpreter, "codeDump", &o_nil);
		addIntrinsicVar(class_interpreter, "preProcessor", &o_nil);

	class_absfunc = makeIntrinsicClass(s_absfunc, s_object, 0, 0);
	class_stream = makeIntrinsicClass(s_stream, s_absfunc, 0, 0);

	class_thread = makeIntrinsicClass(s_thread, s_stream, 24, 0);
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
		//SetInt(&class_rawarray->instanceFormat, obj_int8);
		//class_rawarray->classFlags.ui |= classHasIndexableInstances;
	class_int8array = makeIntrinsicClass(s_int8array, s_rawarray, 0, 0);
		SetInt(&class_int8array->instanceFormat, obj_int8);
		class_int8array->classFlags.ui |= classHasIndexableInstances;
	class_int16array = makeIntrinsicClass(s_int16array, s_rawarray, 0, 0);
		SetInt(&class_int16array->instanceFormat, obj_int16);
		class_int16array->classFlags.ui |= classHasIndexableInstances;
	class_int32array = makeIntrinsicClass(s_int32array, s_rawarray, 0, 0);
		SetInt(&class_int32array->instanceFormat, obj_int32);
		class_int32array->classFlags.ui |= classHasIndexableInstances;
	class_symbolarray = makeIntrinsicClass(s_symbolarray, s_rawarray, 0, 0);
		SetInt(&class_symbolarray->instanceFormat, obj_symbol);
		class_symbolarray->classFlags.ui |= classHasIndexableInstances;
	class_string = makeIntrinsicClass(s_string, s_rawarray, 0, 1);
		addIntrinsicClassVar(class_string, "unixCmdActions", &o_nil);
		SetInt(&class_string->instanceFormat, obj_char);
		class_string->classFlags.ui |= classHasIndexableInstances;
	class_floatarray = makeIntrinsicClass(s_floatarray, s_rawarray, 0, 0);
		SetInt(&class_floatarray->instanceFormat, obj_float);
		class_floatarray->classFlags.ui |= classHasIndexableInstances;
	class_signal = makeIntrinsicClass(s_signal, s_floatarray, 0, 0);
		SetInt(&class_signal->instanceFormat, obj_float);
		class_signal->classFlags.ui |= classHasIndexableInstances;
	class_wavetable = makeIntrinsicClass(s_wavetable, s_floatarray, 0, 0);
		SetInt(&class_wavetable->instanceFormat, obj_float);
		class_wavetable->classFlags.ui |= classHasIndexableInstances;

		//addIntrinsicVar(class_signal, "rate", &o_nil);
	class_doublearray = makeIntrinsicClass(s_doublearray, s_rawarray, 0, 0);
		SetInt(&class_doublearray->instanceFormat, obj_double);
		class_doublearray->classFlags.ui |= classHasIndexableInstances;

	class_list = makeIntrinsicClass(s_list, s_sequenceable_collection, 1, 0);
		addIntrinsicVar(class_list, "array", &o_nil);
		//addIntrinsicVar(class_list, "size", &o_zero);

	class_func = makeIntrinsicClass(s_func, s_absfunc, 2, 0);
		addIntrinsicVar(class_func, "def", &o_nil);
		addIntrinsicVar(class_func, "context", &o_nil);

	gTagClassTable[ 0] = NULL;
	gTagClassTable[ 1] = NULL;
	gTagClassTable[ 2] = class_int;
	gTagClassTable[ 3] = class_symbol;
	gTagClassTable[ 4] = class_char;
	gTagClassTable[ 5] = class_nil;
	gTagClassTable[ 6] = class_false;
	gTagClassTable[ 7] = class_true;
	gTagClassTable[ 8] = class_rawptr;
	gTagClassTable[ 9] = class_float;
	gTagClassTable[10] = class_float;
	gTagClassTable[11] = class_float;
	gTagClassTable[12] = class_float;

	o_emptyarray.utag = tagObj;
	o_emptyarray.uo = newPyrArray(NULL, 0, obj_permanent | obj_immutable, false);

	o_onenilarray.utag = tagObj;
	o_onenilarray.uo = newPyrArray(NULL, 1, obj_permanent | obj_immutable, false);
	o_onenilarray.uo->size = 1;
	SetNil(o_onenilarray.uo->slots);

	o_argnamethis.utag = tagObj;
	o_argnamethis.uosym = newPyrSymbolArray(NULL, 1, obj_permanent | obj_immutable, false);
	o_argnamethis.uosym->size = 1;
	o_argnamethis.uosym->symbols[0] = s_this;

	/*
	post("array %08X '%s'\n", class_array, class_array->name.us->name);
	post("o_emptyarray %08X '%s'\n", o_emptyarray.uo->classptr, o_emptyarray.uo->classptr->name.us->name);
	post("o_argnamethis %08X '%s'\n", o_argnamethis.uo->classptr, o_argnamethis.uo->classptr->name.us->name);
	post("o_onenilarray %08X '%s'\n", o_onenilarray.uo->classptr, o_onenilarray.uo->classptr->name.us->name);
	dumpObjectSlot(&o_emptyarray);
	dumpObjectSlot(&o_argnamethis);
	dumpObjectSlot(&o_onenilarray);
	*/
}

PyrObject* instantiateObject(class PyrGC *gc, PyrClass* classobj, int size,
	bool fill, bool collect)
{
	PyrObject *newobj, *proto;
	int numbytes, format, flags;

	format = classobj->instanceFormat.ui;
	flags = classobj->instanceFlags.ui;

	if (classobj->classFlags.ui & classHasIndexableInstances) {
		// create an indexable object
		numbytes = size * gFormatElemSize[format];
		newobj = gc->New(numbytes, flags, format, collect);
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
		if (classobj->iprototype.utag == tagObj) {
			proto = classobj->iprototype.uo;
			size = proto->size;
			numbytes = size * sizeof(PyrSlot);
			newobj = gc->New(numbytes, flags, format, collect);
			newobj->size = size;
			if (size) {
				memcpy(newobj->slots, proto->slots, numbytes);
			}
		} else {
			numbytes = 0;
			newobj = gc->New(numbytes, flags, format, collect);
			newobj->size = 0;
		}
	}
	newobj->classptr = classobj;
	return newobj;
}

PyrObject* instantiateObjectLight(class PyrGC *gc, PyrClass* classobj, int size, bool collect);
PyrObject* instantiateObjectLight(class PyrGC *gc, PyrClass* classobj, int size, bool collect)
{
	PyrObject *newobj, *proto;
	int numbytes, format, flags;

	format = classobj->instanceFormat.ui;
	flags = classobj->instanceFlags.ui;

	if (classobj->classFlags.ui & classHasIndexableInstances) {
		numbytes = size * gFormatElemSize[format];
	} else {
		if (classobj->iprototype.utag == tagObj) {
			proto = classobj->iprototype.uo;
			size = proto->size;
			numbytes = size * sizeof(PyrSlot);
		} else {
			size = 0;
			numbytes = 0;
		}
	}
	newobj = gc->New(numbytes, flags, format, collect);
	newobj->size = size;
	newobj->classptr = classobj;

	return newobj;
}

PyrObject* copyObject(class PyrGC *gc, PyrObject *inobj, bool collect)
{
	PyrObject *newobj;

	int flags = ~(obj_immutable) & inobj->obj_flags;

	int elemsize = gFormatElemSize[inobj->obj_format];
	int numbytes = inobj->size * elemsize;

	newobj = gc->New(numbytes, flags, inobj->obj_format, collect);

	newobj->size = inobj->size;
	newobj->classptr = inobj->classptr;

	memcpy(newobj->slots, inobj->slots, inobj->size * elemsize);
	return newobj;
}

PyrObject* copyObjectRange(class PyrGC *gc, PyrObject *inobj, int start, int end, bool collect)
{
	PyrObject *newobj;

 	if (start < 0) start = 0;
 	if (end >= inobj->size) end = inobj->size - 1;
 	int length = end - start + 1;
 	if (length < 0) length = 0;

	int elemsize = gFormatElemSize[inobj->obj_format];
	int numbytes = length * elemsize;

	int flags = ~(obj_immutable) & inobj->obj_flags;

	newobj = gc->New(numbytes, flags, inobj->obj_format, collect);
	newobj->size = length;
	newobj->classptr = inobj->classptr;

	if (length > 0) {
		memcpy(newobj->slots, (char*)(inobj->slots) + start * elemsize, length * elemsize);
	}
	return newobj;
}

extern "C" {
	extern char *g_fmt(char *, double);
}

void dumpObject(PyrObject *obj)
{
	char str[256];
	PyrClass *classobj;
	int i;

	if (obj == NULL) {
		postfl("NULL object pointer\n");
		return;
	}
	classobj = obj->classptr;
	if (isKindOf(obj, class_class)) {
		post("class %s (%08X) {\n", ((PyrClass*)obj)->name.us->name, obj);
	} else {
		//post("Instance of %s (%08X) {\n", classobj->name.us->name, obj);
		post("Instance of %s {    (%08X, gc=%02X, fmt=%02X, flg=%02X, set=%02X)\n",
			classobj->name.us->name, obj, obj->gc_color, obj->obj_format, obj->obj_flags,
			obj->obj_sizeclass);
	}
        //flushPostBuf();

	if (obj->obj_format == obj_notindexed) {
		post("  instance variables [%d]\n", obj->size);
		for (i=0; i<obj->size; ++i) {
			slotString(obj->slots + i, str);
			post("    %s : %s\n", classobj->instVarNames.uosym->symbols[i]->name, str);
		}
	} else {
		int maxsize;
		post("  indexed slots [%d]\n", obj->size);
		maxsize = sc_min(32, obj->size);
		switch (obj->obj_format) {
			case obj_slot :
				for (i=0; i<maxsize; ++i) {
					slotString(obj->slots + i, str);
					post("    %3d : %s\n", i, str);
				}
				break;
			case obj_double :
				for (i=0; i<maxsize; ++i) {
					char fstr[32];
					g_fmt(fstr, obj->slots[i].uf);
					post("    %3d : %s\n", i, fstr);
				}
				break;
			case obj_float :
				for (i=0; i<maxsize; ++i) {
					float val;
					val = ((float*)(obj->slots))[i];
					post("    %3d : %g\n", i, val);
				}
				break;
			case obj_int32 :
				for (i=0; i<maxsize; ++i) {
					int32 val;
					val = ((int32*)(obj->slots))[i];
					post("    %3d : %d\n", i, val);
				}
				break;
			case obj_int16 :
				for (i=0; i<maxsize; ++i) {
					int16 val;
					val = ((int16*)(obj->slots))[i];
					post("    %3d : %d\n", i, val);
				}
				break;
			case obj_int8 :
				for (i=0; i<maxsize; ++i) {
					int8 val;
					val = ((int8*)(obj->slots))[i];
					post("    %3d : %4d %4u 0x%02X\n", i, val, val&255, val&255);
				}
				break;
			case obj_char :
				for (i=0; i<maxsize; ++i) {
					char val;
					val = ((char*)(obj->slots))[i];
					post("    %3d : %c\n", i, val);
				}
				break;
			case obj_symbol :
				for (i=0; i<maxsize; ++i) {
					PyrSymbol* sym;
					sym = ((PyrSymbol**)(obj->slots))[i];
					post("    %3d : '%s'\n", i, sym->name);
				}
				break;
			default :
				post("unknown obj_format %X\n", obj->obj_format);
		}
		if (obj->size > maxsize) {
			post("    ...\n");
		}
	}
	post("}\n");
}

void dumpBadObject(PyrObject *obj)
{
	char str[128];
	PyrClass *classobj;
	int i;

	if (obj == NULL) {
		postfl("NULL object pointer\n");
		return;
	}
	classobj = obj->classptr;
	if (isKindOf(obj, class_class)) {
		postfl("class %s (%08X) {\n", ((PyrClass*)obj)->name.us->name, obj);
	} else {
		//postfl("Instance of %s (%08X) {\n", classobj->name.us->name, obj);
		postfl("Instance of %s {    (%08X, gc=%02X, fmt=%02X, flg=%02X, set=%02X)\n",
			classobj->name.us->name, obj, obj->gc_color, obj->obj_format, obj->obj_flags,
			obj->obj_sizeclass);
	}
	if (obj->obj_format == obj_notindexed) {
		postfl("  instance variables [%d]\n", obj->size);
		for (i=0; i<obj->size; ++i) {
			slotString(obj->slots + i, str);
			postfl("    %s : %s\n", classobj->instVarNames.uosym->symbols[i]->name, str);
		}
	} else {
		int maxsize;
		postfl("  indexed slots [%d]\n", obj->size);
		maxsize = obj->size;
		maxsize = sc_min(32, maxsize);
		switch (obj->obj_format) {
			case obj_slot :
				for (i=0; i<maxsize; ++i) {
					slotString(obj->slots + i, str);
					postfl("    %3d : %s\n", i, str);
				}
				break;
			case obj_double :
				for (i=0; i<maxsize; ++i) {
					char fstr[32];
					g_fmt(fstr, obj->slots[i].uf);
					postfl("    %3d : %s\n", i, fstr);
				}
				break;
			case obj_float :
				for (i=0; i<maxsize; ++i) {
					float val;
					val = ((float*)(obj->slots))[i];
					postfl("    %3d : %g\n", i, val);
				}
				break;
			case obj_int32 :
				for (i=0; i<maxsize; ++i) {
					int32 val;
					val = ((int32*)(obj->slots))[i];
					postfl("    %3d : %d\n", i, val);
				}
				break;
			case obj_int16 :
				for (i=0; i<maxsize; ++i) {
					int16 val;
					val = ((int16*)(obj->slots))[i];
					postfl("    %3d : %d\n", i, val);
				}
				break;
			case obj_int8 :
				for (i=0; i<maxsize; ++i) {
					int8 val;
					val = ((int8*)(obj->slots))[i];
					postfl("    %3d : %4d %4u 0x%02X\n", i, val, val&255, val&255);
				}
				break;
			case obj_char :
				for (i=0; i<maxsize; ++i) {
					char val;
					val = ((char*)(obj->slots))[i];
					postfl("    %3d : %c\n", i, val);
				}
				break;
			case obj_symbol :
				for (i=0; i<maxsize; ++i) {
					PyrSymbol* sym;
					sym = ((PyrSymbol**)(obj->slots))[i];
					post("    %3d : '%s'\n", i, sym->name);
				}
				break;
			default :
				postfl("unknown obj_format %X\n", obj->obj_format);
		}
		if (obj->size > maxsize) {
			postfl("    ...\n");
		}
	}
	postfl("}\n");
}

void dumpObjectSlot(PyrSlot *slot)
{
	if (slot->utag == tagObj) {
		dumpObject(slot->uo);
	} else {
		dumpPyrSlot(slot);
	}
}

void dumpSlotOneWord(const char *tagstr, PyrSlot *slot)
{
	char str[256];
	slotOneWord(slot, str);
	post("%s %s\n", tagstr, str);
}

void CallStackSanity(VMGlobals *g, char *tagstr);
void CallStackSanity(VMGlobals *g, char *tagstr)
{
	PyrFrame *frame;
	frame = g->frame;
	while (frame) {
		if (FrameSanity(frame, tagstr)) {
			DumpBackTrace(g);
			//Debugger();
			break;
		}
		frame = frame->caller.uof;
	}
}

bool FrameSanity(PyrFrame *frame, const char *tagstr);
bool FrameSanity(PyrFrame *frame, const char *tagstr)
{
	bool failed = false;
	if (frame==NULL) return false;
	if (frame->method.utag != tagObj) {
		postfl("Frame %X method tag wrong %X\n", frame, frame->method.utag);
		failed = true;
	//} else if (!isKindOf((PyrObject*)frame->method.uo->classptr, class_fundef)) {
	} else if (frame->method.uo->classptr != class_method && frame->method.uo->classptr != class_fundef) {
		postfl("Frame %X method class wrong %X\n", frame, frame->method.uo->classptr);
		failed = true;
		//if (frame->method.uo->classptr->classptr == class_class) {
			postfl("class: '%s'\n", frame->method.uo->classptr->name.us->name);
		///} else {
		//	postfl("not even a class\n");
		//}
	} else if (frame->method.uoblk->code.utag != tagObj) {
		postfl("Method %X code tag wrong %X\n", frame->method.uoblk, frame->method.uoblk->code.utag);
		failed = true;
	} else if (frame->method.uoblk->code.uo->classptr != class_int8array) {
		postfl("Code %X class wrong %X\n", frame->method.uoblk->code.uo, frame->method.uoblk->code.uo->classptr);
			postfl("class: '%s'\n", frame->method.uoblk->code.uo->classptr->name.us->name);
		failed = true;
	}
	/*
	if (frame->caller.utag != tagHFrame && frame->caller.utag != tagNil) {
		postfl("Frame %X caller tag wrong %X\n", frame, frame->caller.utag);
		failed = true;
	}
	if (frame->context.utag != tagHFrame && frame->context.utag != tagNil) {
		postfl("Frame %X context tag wrong %X\n", frame, frame->context.utag);
		failed = true;
	}
	if (frame->homeContext.utag != tagHFrame && frame->homeContext.utag != tagNil) {
		postfl("Frame %X homeContext tag wrong %X\n", frame, frame->homeContext.utag);
		failed = true;
	}
	*/
	if (frame->ip.utag != tagPtr) {
		postfl("Frame %X ip tag wrong %X\n", frame, frame->ip.utag);
		failed = true;
	}
	return failed;
}

void DumpFrame(PyrFrame *frame)
{
	char str[256];
	int i, numargs;
	PyrMethod *meth;
	PyrMethodRaw *methraw;

	if (FrameSanity(frame, "DumpFrame")) {
		post("FRAME CORRUPTED\n");
		return;
	}
	slotOneWord(&frame->method, str);
	//slotString(&frame->method, str);

	meth = frame->method.uom;
	methraw = METHRAW(meth);
	if (methraw->numtemps) {
		post("\t%s   %08X\n", str, frame);
//#ifndef SC_LINUX
// sk: crashes on linux when accessing meth->argNames.uosym->symbols[i]
		numargs = methraw->numargs + methraw->varargs;
		for (i=0; i<methraw->numtemps; ++i) {
			slotOneWord(frame->vars + i, str);
			//slotString(frame->vars + i, str);
			if (i < numargs) {
				post("\t\targ %s = %s\n", meth->argNames.uosym->symbols[i]->name, str);
			} else {
				post("\t\tvar %s = %s\n", meth->varNames.uosym->symbols[i - numargs]->name, str);
			}
		}
//#endif // !SC_LINUX
	} else {
		post("\t%s  (no arguments or variables)\n", str);
	}
}

void dumpByteCodes(PyrBlock *theBlock);

void DumpDetailedFrame(PyrFrame *frame);
void DumpDetailedFrame(PyrFrame *frame)
{
	char mstr[256];
	char str[256];
	int i, numargs;
	PyrMethod *meth;
	PyrMethodRaw *methraw;

	if (FrameSanity(frame, "DumpDetailedFrame")) {
		post("FRAME CORRUPTED\n");
		return;
	}
	slotOneWord(&frame->method, mstr);
	//slotString(&frame->method, str);

	meth = frame->method.uom;
	methraw = METHRAW(meth);

	if (methraw->numtemps) {
		post("\t%s\n", mstr);
		numargs = methraw->numargs + methraw->varargs;
		for (i=0; i<methraw->numtemps; ++i) {
			slotOneWord(frame->vars + i, str);
			//slotString(frame->vars + i, str);
			if (i < numargs) {
				post("\t\targ %s = %s\n", meth->argNames.uosym->symbols[i]->name, str);
			} else {
				post("\t\tvar %s = %s\n", meth->varNames.uosym->symbols[i - numargs]->name, str);
			}
		}
	} else {
		post("\t%s  (no arguments or variables)\n", mstr);
	}

	post("\t....%s details:\n", mstr);
	post("\t\tneedsHeapContext  = %d\n", methraw->needsHeapContext);
	post("\t\tnumtemps  = %d\n", methraw->numtemps);
	post("\t\tpopSize  = %d\n", methraw->popSize);

	slotString(&frame->method, str);		post("\t\tmethod  = %s\n", str);
	slotString(&frame->caller, str);		post("\t\tcaller  = %s\n", str);
	slotString(&frame->context, str);		post("\t\tcontext = %s\n", str);
	slotString(&frame->homeContext, str);	post("\t\thomeCtx = %s\n", str);
	slotString(&frame->ip, str);			post("\t\tip      = %s\n", str);

	if (IsPtr(&frame->ip)) {
		post("ipoffset = %d\n", (char*)frame->ip.ui - (char*)meth->code.uob->b);
		dumpByteCodes(meth);
	}

}


bool respondsTo(PyrSlot *slot, PyrSymbol *selector)
{
	PyrClass *classobj;
	PyrMethod *meth;
	int index;

	classobj = classOfSlot(slot);

	index = classobj->classIndex.ui + selector->u.index;
	meth = gRowTable[index];
	return meth->name.us == selector;
}

PyrMethod* methodLookup(PyrSlot *slot, PyrSymbol *selector);
PyrMethod* methodLookup(PyrSlot *slot, PyrSymbol *selector)
{
	PyrClass *classobj;
	PyrMethod *meth;
	int index;

	classobj = classOfSlot(slot);

	index = classobj->classIndex.ui + selector->u.index;
	meth = gRowTable[index];
	return meth;
}


bool isSubclassOf(PyrClass *classobj, PyrClass *testclass)
{
	while (classobj) {
		if (classobj == testclass) {
			return true;
		}
		classobj = classobj->superclass.us->u.classobj;
	}
	return false;
}

/*bool isKindOf(PyrObjectHdr *obj, PyrClass *testclass)
{
	int objClassIndex = obj->classptr->classIndex.ui;
	return objClassIndex >= testclass->classIndex.ui && objClassIndex <= testclass->maxSubclassIndex.ui;
}*/

bool objAddIndexedSlot(PyrObject *obj, PyrSlot *slot)
{
	if (obj->size < ARRAYMAXINDEXSIZE(obj)) {
		slotCopy(&obj->slots[obj->size++], slot);
		return true;
	} else {
		return false;
	}
}

bool objAddIndexedSymbol(PyrSymbolArray *obj, PyrSymbol *symbol)
{
	if (obj->size < MAXINDEXSIZE((PyrObject*)obj)) {
		obj->symbols[obj->size++] = symbol;
		return true;
	} else {
		return false;
	}
}

bool objAddIndexedObject(PyrObject *obj, PyrObject *obj2)
{
	if (obj->size < ARRAYMAXINDEXSIZE(obj)) {
		SetObject(obj->slots + obj->size, obj2);
		obj->size++;
		return true;
	} else {
		return false;
	}
}


void fillSlots(PyrSlot* slot, int size, PyrSlot* fillslot)
{
	double fillval = fillslot->uf;

	double *p = &slot->uf - 1;
	double *pend = p + size;
	while (p < pend) *++p = fillval;
}

void nilSlots(PyrSlot* slot, int size)
{
	double xnil = o_nil.uf;

	double *p = &slot->uf - 1;
	double *pend = p + size;
	while (p < pend) *++p = xnil;
}

void zeroSlots(PyrSlot* slot, int size)
{
	double *p = &slot->uf - 1;
	double *pend = p + size;
	while (p < pend) *++p = 0.;
}

PyrObject* newPyrObject(class PyrGC *gc, size_t inNumBytes, int inFlags, int inFormat, bool inCollect)
{
	return gc->New(inNumBytes, inFlags, inFormat, inCollect);
}

PyrObject* newPyrArray(class PyrGC *gc, int size, int flags, bool collect)
{
	PyrObject* array;

	int numbytes = size * sizeof(PyrSlot);
	if (!gc) array = (PyrObject*)PyrGC::NewPermanent(numbytes, flags, obj_slot);
	else array = (PyrObject*)gc->New(numbytes, flags, obj_slot, collect);
	array->classptr = class_array;
	return array;
}

PyrSymbolArray* newPyrSymbolArray(class PyrGC *gc, int size, int flags, bool collect)
{
	PyrSymbolArray* array;

	int numbytes = size * sizeof(PyrSymbol*);
	if (!gc) array = (PyrSymbolArray*)PyrGC::NewPermanent(numbytes, flags, obj_symbol);
	else array = (PyrSymbolArray*)gc->New(numbytes, flags, obj_symbol, collect);
	array->classptr = class_symbolarray;
	return array;
}

PyrInt8Array* newPyrInt8Array(class PyrGC *gc, int size, int flags, bool collect)
{
	PyrInt8Array* array;

	if (!gc) array = (PyrInt8Array*)PyrGC::NewPermanent(size, flags, obj_int8);
	else array = (PyrInt8Array*)gc->New(size, flags, obj_int8, collect);
	array->classptr = class_int8array;
	return array;
}

PyrInt32Array* newPyrInt32Array(class PyrGC *gc, int size, int flags, bool collect)
{
	PyrInt32Array* array;
	int numbytes = size * sizeof(int32);
	if (!gc) array = (PyrInt32Array*)PyrGC::NewPermanent(numbytes, flags, obj_int32);
	else array = (PyrInt32Array*)gc->New(numbytes, flags, obj_int32, collect);
	array->classptr = class_int32array;
	return array;
}

PyrDoubleArray* newPyrDoubleArray(class PyrGC *gc, int size, int flags, bool collect)
{
	PyrDoubleArray* array;

	int numbytes = size * sizeof(double);
	if (!gc) array = (PyrDoubleArray*)PyrGC::NewPermanent(numbytes, flags, obj_double);
	else array = (PyrDoubleArray*)gc->New(size, flags, obj_double, collect);
	array->classptr = class_doublearray;
	return array;
}

PyrString* newPyrString(class PyrGC *gc, const char *s, int flags, bool collect)
{
	PyrString* string;
	int length = strlen(s);

	if (!gc) string = (PyrString*)PyrGC::NewPermanent(length, flags, obj_char);
	else string = (PyrString*)gc->New(length, flags, obj_char, collect);
	string->classptr = class_string;
	string->size = length;
	memcpy(string->s, s, length);
	return string;
}

PyrString* newPyrStringN(class PyrGC *gc, int length, int flags, bool collect)
{
	PyrString* string;

	if (!gc) string = (PyrString*)PyrGC::NewPermanent(length, flags, obj_char);
	else string = (PyrString*)gc->New(length, flags, obj_char, collect);
	string->classptr = class_string;
	string->size = length; // filled with garbage!
	return string;
}

PyrBlock* newPyrBlock(int flags)
{
	PyrBlock* block;
	PyrMethodRaw *methraw;


	int32 numbytes = sizeof(PyrBlock) - sizeof(PyrObjectHdr);
	int32 numSlots = numbytes / sizeof(PyrSlot);

	if (!compilingCmdLine) block = (PyrBlock*)PyrGC::NewPermanent(numbytes, flags, obj_notindexed);
	else block = (PyrBlock*)gMainVMGlobals->gc->New(numbytes, flags, obj_notindexed, false);
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

	nilSlots(&block->rawData1,  numSlots);
	return block;
}

struct VMGlobals* scGlobals();
struct VMGlobals* scGlobals()
{
	return gMainVMGlobals;
}

PyrMethod* initPyrMethod(PyrMethod* method)
{

	int32 numbytes = sizeof(PyrMethod) - sizeof(PyrObjectHdr);
	int32 numSlots = numbytes / sizeof(PyrSlot);

	method->classptr = class_method;
	method->size = 0;
	method->size = numSlots;
	method->rawData1.uf = 0.0;
	method->rawData2.uf = 0.0;
	nilSlots(&method->code,  numSlots-2);
	//slotCopy(&method->byteMeter, &o_zero);
	//slotCopy(&method->callMeter, &o_zero);
	//post("<- newPyrMethod %08X %08X\n", method, methraw);
	return method;
}

PyrMethod* newPyrMethod()
{
	int32 numbytes = sizeof(PyrMethod) - sizeof(PyrObjectHdr);
	PyrMethod* method = (PyrMethod*)PyrGC::NewPermanent(numbytes, obj_permanent | obj_immutable, obj_notindexed);
	return initPyrMethod(method);
}

void freePyrSlot(PyrSlot *slot)
{
	if (NotNil(slot)) {
		PyrObject *obj;
		obj = slot->uo;
		if (obj && obj->IsPermanent()) {
			// don't deallocate these
			if (obj != o_emptyarray.uo && obj != o_onenilarray.uo && obj != o_argnamethis.uo) {
				pyr_pool_runtime->Free((void*)obj);
			}
			SetNil(slot);
		}
	}
}

void freePyrObject(PyrObject *obj)
{
	if (obj->IsPermanent()) {
		pyr_pool_runtime->Free((void*)obj);
	}
}

int getIndexedInt(PyrObject *obj, int index, int *value)
{
	PyrSlot *slot;
	int err = errNone;
	if (index < 0 || index >= obj->size) return errIndexOutOfRange;
	switch (obj->obj_format) {
		case obj_slot :
			slot = obj->slots + index;
			if (IsFloat(slot)) {
				*value = (int)slot->uf;
			} else if (slot->utag == tagInt) {
				*value = slot->ui;
			} else {
				err = errWrongType;
			}
			break;
		case obj_double :
			*value = (int)obj->slots[index].uf;
			break;
		case obj_float :
			*value = (int)((float*)(obj->slots))[index];
			break;
		case obj_int32 :
			*value = ((int32*)(obj->slots))[index];
			break;
		case obj_int16 :
			*value = ((int16*)(obj->slots))[index];
			break;
		case obj_int8 :
			*value = ((int8*)(obj->slots))[index];
			break;
		default :
			err = errWrongType;
	}
	return err;
}

int getIndexedFloat(PyrObject *obj, int index, float *value)
{
	PyrSlot *slot;
	int err = errNone;
	if (index < 0 || index >= obj->size) return errIndexOutOfRange;
	switch (obj->obj_format) {
		case obj_slot :
			slot = obj->slots + index;
			if (IsFloat(slot)) {
				*value = slot->uf;
			} else if (slot->utag == tagInt) {
				*value = slot->ui;
			} else {
				err = errWrongType;
			}
			break;
		case obj_double :
			*value = obj->slots[index].uf;
			break;
		case obj_float :
			*value = ((float*)(obj->slots))[index];
			break;
		case obj_int32 :
			*value = ((int32*)(obj->slots))[index];
			break;
		case obj_int16 :
			*value = ((int16*)(obj->slots))[index];
			break;
		case obj_int8 :
			*value = ((int8*)(obj->slots))[index];
			break;
		default :
			err = errWrongType;
	}
	return err;
}

int getIndexedDouble(PyrObject *obj, int index, double *value)
{
	PyrSlot *slot;
	int err = errNone;
	if (index < 0 || index >= obj->size) return errIndexOutOfRange;
	switch (obj->obj_format) {
		case obj_slot :
			slot = obj->slots + index;
			if (IsFloat(slot)) {
				*value = slot->uf;
			} else if (slot->utag == tagInt) {
				*value = slot->ui;
			} else {
				err = errWrongType;
			}
			break;
		case obj_double :
			*value = obj->slots[index].uf;
			break;
		case obj_float :
			*value = ((float*)(obj->slots))[index];
			break;
		case obj_int32 :
			*value = ((int32*)(obj->slots))[index];
			break;
		case obj_int16 :
			*value = ((int16*)(obj->slots))[index];
			break;
		case obj_int8 :
			*value = ((int8*)(obj->slots))[index];
			break;
		default :
			err = errWrongType;
	}
	return err;
}


void getIndexedSlot(PyrObject *obj, PyrSlot *a, int index)
{
//	postfl("getIndexedSlot %s %X %d\n", obj->classptr->name.us->name,
//		obj, index);
	switch (obj->obj_format) {
		case obj_slot :
		case obj_double :
			slotCopy(a, &obj->slots[index]);
			break;
		case obj_float :
			a->uf = ((float*)(obj->slots))[index];
			break;
		case obj_int32 :
			a->ui = ((int32*)(obj->slots))[index];
			a->utag = tagInt;
			break;
		case obj_int16 :
			a->ui = ((int16*)(obj->slots))[index];
			a->utag = tagInt;
			break;
		case obj_int8 :
			a->ui = ((int8*)(obj->slots))[index];
			a->utag = tagInt;
			break;
		case obj_symbol :
			a->ui = ((int*)(obj->slots))[index];
			a->utag = tagSym;
			break;
		case obj_char :
			a->ui = ((unsigned char*)(obj->slots))[index];
			a->utag = tagChar;
			break;
	}
}

int putIndexedSlot(VMGlobals *g, PyrObject *obj, PyrSlot *c, int index)
{
	PyrSlot *slot;
	switch (obj->obj_format) {
		case obj_slot :
			if (obj->obj_flags & obj_immutable) return errImmutableObject;
			slot = obj->slots + index;
			slotCopy(slot, c);
			g->gc->GCWrite(obj, slot);
			break;
		case obj_double :
			if (NotFloat(c)) {
				if (c->utag != tagInt) return errWrongType;
				else {
					obj->slots[index].uf = c->ui;
				}
			} else {
				obj->slots[index].uf = c->uf;
			}
			break;
		case obj_float :
			if (NotFloat(c)) {
				if (c->utag != tagInt) return errWrongType;
				else {
					((float*)(obj->slots))[index] = c->ui;
				}
			} else {
				((float*)(obj->slots))[index] = c->uf;
			}
			break;
		case obj_int32 :
			if (c->utag != tagInt) return errWrongType;
			((int32*)(obj->slots))[index] = c->ui;
			break;
		case obj_int16 :
			if (c->utag != tagInt) return errWrongType;
			((int16*)(obj->slots))[index] = c->ui;
			break;
		case obj_int8 :
			if (c->utag != tagInt) return errWrongType;
			((int8*)(obj->slots))[index] = c->ui;
			break;
		case obj_symbol :
			if (c->utag != tagSym) return errWrongType;
			((int*)(obj->slots))[index] = c->ui;
			break;
		case obj_char :
			if (c->utag != tagChar) return errWrongType;
			((unsigned char*)(obj->slots))[index] = c->ui;
			break;
	}
	return errNone;
}

int putIndexedFloat(PyrObject *obj, double val, int index)
{
	PyrSlot *slot;
	switch (obj->obj_format) {
		case obj_slot :
			if (obj->obj_flags & obj_immutable) return errImmutableObject;
			slot = obj->slots + index;
			slot->uf = val;
			break;
		case obj_double :
			obj->slots[index].uf = val;
			break;
		case obj_float :
			((float*)(obj->slots))[index] = (float)val;
			break;
		case obj_int32 :
			((int32*)(obj->slots))[index] = (int32)val;
			break;
		case obj_int16 :
			((int16*)(obj->slots))[index] = (int16)val;
			break;
		case obj_int8 :
			((int8*)(obj->slots))[index] = (int8)val;
			break;
	}
	return errNone;
}

int calcHash(PyrSlot *a);
int calcHash(PyrSlot *a)
{
	int hash;
	switch (a->utag) {
		case tagObj : hash = Hash(a->ui); break;
		case tagInt : hash = Hash(a->ui); break;
		case tagChar : hash = Hash(a->ui & 255); break;
		case tagSym : hash = a->us->hash; break;
		case tagNil : hash = 0xA5A5A5A5; break;
		case tagFalse : hash = 0x55AA55AA; break;
		case tagTrue : hash = 0x69696969; break;
		case tagPtr : hash = Hash(a->ui); break;
		default : hash = Hash(a->utag + Hash(a->ui)); break; // hash for a double
	}
	return hash;
}

void InstallFinalizer(VMGlobals* g, PyrObject *inObj, int slotIndex, ObjFuncPtr inFunc)
{
	PyrObject *finalizer = g->gc->NewFinalizer(inFunc, inObj, false);
	SetObject(inObj->slots + slotIndex, finalizer);
	g->gc->GCWrite(inObj, finalizer);
}
