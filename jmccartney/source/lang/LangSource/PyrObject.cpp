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
PyrClass *class_inf;
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
PyrClass *class_thread;
PyrClass *class_routine;
PyrClass *class_task;
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
PyrSymbol *s_symbol, *s_nil, *s_inf;
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
PyrSymbol *s_new, *s_ref, *s_value;
PyrSymbol *s_envirGet, *s_envirPut;
PyrSymbol *s_synth, *s_spawn, *s_environment;
PyrSymbol *s_hardwaresetup, *s_shutdown;
PyrSymbol *s_linear, *s_exponential, *s_gate;
PyrSymbol *s_super, *s_this;
PyrSlot o_nil, o_true, o_false, o_end, o_inf;
PyrSlot o_pi, o_twopi;
PyrSlot o_fhalf, o_fnegone, o_fzero, o_fone, o_ftwo;
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
	s_inf = getsym("Infinitum");
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
	
	s_linear = getsym("linear");
	s_exponential = getsym("exponential");
	s_gate = getsym("gate");
	
	//s_dsp = getsym("DSP");
	//s_dspsettings = getsym("DSPSettings");
	s_synth = getsym("Synth");
	s_spawn = getsym("BasicSpawn");
	s_environment = getsym("Environment");
	s_hardwaresetup = getsym("hardwareSetup");
	s_shutdown = getsym("shutDown");
	
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
	s_startup = getsym("startUp");
	s_docmdline = getsym("doCmdLine");
	s_next = getsym("next");
	s_value = getsym("value");
	s_ugen = getsym("UGen");
	s_outputproxy = getsym("OutputProxy");
	s_env = getsym("Env");
	
	s_envirGet = getsym("envirGet");
	s_envirPut = getsym("envirPut");
	
	o_nil.utag = tagNil;		o_nil.ui = 0;
	o_false.utag = tagFalse;	o_false.ui = 0;
	o_true.utag = tagTrue;		o_true.ui = 0;
	o_inf.utag = tagInf;		o_inf.ui = 0;
	
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
	gSpecialValues[svEnd] = o_end.uf;
	
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

char *slotSymString(PyrSlot* slot)
{
	switch (slot->utag) {
		case tagObj : return slot->uo->classptr->name.us->name;
		case tagHFrame : return "Frame";
		case tagSFrame : return "Frame";
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
	int numInstVars, int numClassVars, int numInstMethods, 
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
	
	return classobj;
}

void reallocClassObj(PyrClass* classobj, 
	int numInstVars, int numClassVars, int numMethods,
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
		//post("reallocClassObj %s numClassVars %d\n", classobj->name.us->name, numInstVars);
		symarray = newPyrSymbolArray(NULL, numClassVars, obj_permanent | obj_immutable, false);
		//array->size = numClassVars;
		SetObject(&classobj->classVarNames, symarray);
		nilSlots(array->slots, numClassVars);
		
		array = newPyrArray(NULL, numClassVars, obj_permanent | obj_immutable, false);
		//array->size = numClassVars;
		SetObject(&classobj->cprototype, array);
		nilSlots(array->slots, numClassVars);
	} else {
		SetNil(&classobj->classVarNames);
		SetNil(&classobj->cprototype);
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
	PyrObject *obj = arraySlot->uo;
	if (obj->size >= ARRAYMAXINDEXSIZE(obj)) {
		post("objAddIndexedSlotGrow\n");
		PyrObject *newobj = (PyrObject*)newPyrArray(NULL, obj->size * 2, obj_permanent | obj_immutable, false);
		memcpy(newobj->slots, obj->slots, obj->size * sizeof(PyrSlot));
		newobj->size = obj->size;
		SetObject(arraySlot, newobj);
		pyr_pool_runtime->Free((void*)obj);
		obj = newobj;
	}
	obj->slots[obj->size++].ucopy = addSlot->ucopy;
}

void addMethod(PyrClass *classobj, PyrMethod *method)
{
	PyrSlot slot;
	SetObject(&slot, method);
	objAddIndexedSlotGrow(&classobj->methods, &slot);
}



PyrMethod* classFindDirectInstMethod(PyrClass* classobj, PyrSymbol *name)
{
	PyrMethod *method;
	PyrSlot *methods;
	int i, numMethods;
	methods = classobj->methods.uo->slots;
	numMethods = classobj->methods.uo->size;
	for (i=0; i<numMethods; ++i) {
		method = methods[i].uom;
		if (method->name.us == name) break;
	}
	if (i>=numMethods) method = NULL;
	return method;
}

PyrMethod* classFindDirectClassMethod(PyrClass* classobj, PyrSymbol *name)
{
	PyrMethod *method = NULL;
	PyrSlot *methods;
	PyrClass *metaclassobj;
	int i, numMethods;
	
	metaclassobj = classobj->classptr;
	if (metaclassobj) {
		methods = classobj->methods.uo->slots;
		numMethods = classobj->methods.uo->size;
		for (i=0; i<numMethods; ++i) {
			method = methods[i].uom;
			if (method->name.us == name) break;
		}
		if (i>=numMethods) method = NULL;
	}
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

int instVarOffset(char *classname, char *instvarname)
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

int classVarOffset(char *classname, char *classvarname, PyrClass** classobj)
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
	
	// if this is a Metaclass then we need to search its normal Class for
	// the class vars
	classname = (*classobj)->name.us->name;
	if (strncmp(classname, "Meta_", 5) == 0) {
		*classobj = getsym(classname+5)->u.classobj;
	}
	for (j=0; *classobj; ++j) {
		if (NotNil(&(*classobj)->classVarNames)) {
			cvnames = (*classobj)->classVarNames.uosym;
			if (cvnames) {
				for (i=0; i<cvnames->size; ++i) {
					cvname = cvnames->symbols[i];
					if (cvname == name) {
						*index = i;
						return true;
					}
				}
			}
		}
		if ((*classobj)->superclass.utag == tagSym) {
			*classobj = (*classobj)->superclass.us->u.classobj;
		} else {
			*classobj = NULL;
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

void postClassTree(PyrClass *classobj, int level)
{
	PyrObject *subclasses;
	int i;
	
	//post("%4d  ", classobj->classIndex.ui);
	for (i=0; i<level; ++i) {
		post("  ");
	}
	post("%s\n", classobj->name.us->name);
	
	if (classobj == class_class) {
		for (i=0; i<level+1; ++i) {
			post("  ");
		}
		post("      .. all metaclasses ..\n");
	} else {
		subclasses = classobj->subclasses.uo;
		if (subclasses) {
			for (i=0; i<subclasses->size; ++i) {
				postClassTree(subclasses->slots[i].uoc, level+1);
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
				//	post("rd: %3d %s-%s\n", k++, classobj->name.us->name,
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

void buildBigMethodMatrix()
{
	PyrMethod **bigTable, *method, **temprow, **row;
	PyrClass *classobj, **classes;
	int i, j, k;
	int popSum, widthSum;
	int rowOffset, freeIndex;
	int rowTableSize;
	int bigTableSize;
	
	//post("allocate arrays\n");
	
	// pyrmalloc: 
	// lifetime: kill after compile
	bigTableSize = gNumSelectors * gNumClasses;
	//post("bigTableSize %d %d %d\n", bigTableSize, gNumSelectors, gNumClasses);
	ColumnDescriptor *sels = (ColumnDescriptor*)pyr_pool_compile->Alloc(gNumSelectors * sizeof(ColumnDescriptor));
	MEMFAIL(sels);
	bigTable = (PyrMethod**)pyr_pool_compile->Alloc(gNumSelectors * gNumClasses * sizeof(PyrMethod*));
	MEMFAIL(bigTable);
	classes = (PyrClass**)pyr_pool_compile->Alloc(gNumClasses * sizeof(PyrClass*));
	MEMFAIL(classes);

	classobj = gClassList;
	while (classobj) {
		classes[classobj->classIndex.ui] = classobj;
		classobj = classobj->nextclass.uoc;
	}
	
	fillClassRow(class_object, bigTable);
	
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
	for (i=0; i<gNumSelectors; ++i) {
		//postfl("%3d %s\n", i, sels[i].selector->name);
		sels[i].minClassIndex = LONG_MAX;
		sels[i].maxClassIndex = 0;
		//sels[i].largestChunk = 0;
		//sels[i].chunkOffset = 0;
		sels[i].selectorIndex = i;
		sels[i].population = 0;
		//chunkSize = 0;
		//chunkOffset = 0;
		for (j=0; j<gNumClasses; ++j) {
			method = bigTable[j * gNumSelectors + i];
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
		//if (chunkSize > sels[i].largestChunk) {
		//	sels[i].largestChunk = chunkSize;
		//	sels[i].chunkOffset = chunkOffset;
		//}
		sels[i].rowWidth = sels[i].maxClassIndex - sels[i].minClassIndex + 1;
	}
	
	//post("qsort\n");
	// sort rows by largest chunk, then by width, then by chunk offset
	//qsort(sels, gNumSelectors, sizeof(ColumnDescriptor), (std::_compare_function)compareColDescs);
	qsort(sels, gNumSelectors, sizeof(ColumnDescriptor), compareColDescs);
	// bin sort the class rows to the new ordering
	//post("reorder rows\n");
	// pyrmalloc: 
	// lifetime: kill after compile
	temprow = (PyrMethod**)pyr_pool_compile->Alloc(gNumSelectors * sizeof(PyrMethod*));
	MEMFAIL(temprow);
	for (j=0; j<gNumClasses; ++j) {
		row = bigTable + j * gNumSelectors;
		memcpy(temprow, row, gNumSelectors * sizeof(PyrMethod*));
		for (i=0; i<gNumSelectors; ++i) {
			row[i] = temprow[sels[i].selectorIndex];
		}
	}
	pyr_pool_compile->Free(temprow);

	//post("calc row offsets %d\n", gNumSelectors);
	widthSum = 0;
	popSum = 0;
	freeIndex = 0;
	rowOffset = -1;
	for (i=0; i<gNumSelectors; ++i) {
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
	rowTableSize = (freeIndex + gNumClasses)  * sizeof(PyrMethod*);
	gRowTable = (PyrMethod**)pyr_pool_runtime->Alloc(rowTableSize);
	MEMFAIL(gRowTable);
	memset(gRowTable, 0, (freeIndex + gNumClasses) * sizeof(PyrMethod*));
	
	//post("fill compressed table\n");
	//{ FILE* fp;
	// newPyrMethod
	//fp = fopen("meth table", "w");
	for (i=0; i<gNumSelectors; ++i) {
		int offset, maxwidth;
		offset = sels[i].rowOffset + sels[i].minClassIndex;
		maxwidth = offset + sels[i].rowWidth;
		row = bigTable + sels[i].minClassIndex * gNumSelectors + i;
		for (j=offset,k=0; j<maxwidth; ++j,k+=gNumSelectors) {
			method = row[k];
			gRowTable[j] = method;
		}
	}
#if 0
	// redirection patching won't work because selector will not match one being looked up
	// so you'll always get doesNotUnderstand
	// patch redirections
	for (i=0; i<gNumSelectors; ++i) {
		int offset, maxwidth, minClassIndex;
		offset = sels[i].rowOffset;
		minClassIndex = sels[i].minClassIndex;
		maxwidth = offset + minClassIndex + sels[i].rowWidth;
		row = bigTable + sels[i].minClassIndex * gNumSelectors + i;
		for (j=offset + minClassIndex,k=0; j<maxwidth; ++j,k+=gNumSelectors) {
			method = row[k];
			if (method->methType == methRedirect) {
				int redirect, classIndex;
				classIndex = j - offset;
				redirect = method->literals.us->u.index + classes[classIndex]->classIndex.ui;
				gRowTable[j] = gRowTable[redirect];
			}
		}
	}
#endif
	//fclose(fp);
	//}
	
	// having the method ptr always be valid saves a branch in SendMessage()
	for (i=0; i<freeIndex + gNumClasses; ++i) {
		if (gRowTable[i] == NULL) gRowTable[i] = gNullMethod;
	}
	
	//post("freeIndex %d\n", freeIndex);
	//post("widthSum %d\n", widthSum);
	//post("popSum %d\n", popSum);
#if 0
	{
		int numFilled = 0;
		for (i=0; i<rowTableSize/sizeof(PyrMethod*); ++i) {
			if (gRowTable[i] != gNullMethod) numFilled++;
		}
		post("Filled %d of %d = %f percent\n", numFilled, rowTableSize/sizeof(PyrMethod*),
			100. * (double)numFilled/(rowTableSize/sizeof(PyrMethod*)));
	}
#endif
	post("   Method Table Size %d bytes\n", rowTableSize);
	post("   Number of Method Selectors %d\n", gNumSelectors);
	post("   Number of Classes %d\n", gNumClasses);
	//post("big table size %d\n", gNumSelectors * gNumClasses * sizeof(PyrMethod*));
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
	PyrClass *superClass;
	PyrClass *metaSuperClass;
	PyrSymbol *metaClassName;
	PyrSymbol *metaSuperClassName;
	PyrClass *classobj, *metaclassobj;
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
		classClassNumInstVars, 0, 0, obj_notindexed, 0);
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
		numInstVars + superInstVars, numClassVars, 0, obj_notindexed, 0);
	classobj->classFlags.ui |= classIsIntrinsic;

	//postfl("%s-%s  : %d\n", className->name, superClassName->name, superInstVars);
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

void addIntrinsicVar(PyrClass *classobj, char *varName, PyrSlot *slot)
{
	//postfl("%s  %s  %d\n", classobj->name.us->name, varName, 
	//	classobj->instVarNames.uo->size);
	objAddIndexedSymbol(classobj->instVarNames.uosym, getsym(varName));
	objAddIndexedSlot(classobj->iprototype.uo, slot);
}

void addIntrinsicClassVar(PyrClass *classobj, char *varName, PyrSlot *slot);
void addIntrinsicClassVar(PyrClass *classobj, char *varName, PyrSlot *slot)
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
	
	gClassList = NULL;
	gNullMethod = newPyrMethod();
	gNullMethod->name.us = 0;
	methraw = METHRAW(gNullMethod);
	methraw->methType = methNormal;
	
	// build intrinsic classes
	class_class = NULL;
	class_object = makeIntrinsicClass(s_object, 0, 0, 4);
	class_class = makeIntrinsicClass(s_class, s_object, classClassNumInstVars, 0);

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
		
		addIntrinsicVar(class_class, "instanceFormat", &o_nil);
		addIntrinsicVar(class_class, "instanceFlags", &o_zero);
		addIntrinsicVar(class_class, "classIndex", &o_zero);
		addIntrinsicVar(class_class, "classFlags", &o_zero);
		addIntrinsicVar(class_class, "maxSubclassIndex", &o_zero);
		addIntrinsicVar(class_class, "filenameSymbol", &o_nil);
		addIntrinsicVar(class_class, "charPos", &o_zero);
		
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
	
	class_fundef = makeIntrinsicClass(s_fundef, s_object, 8, 0);
		// declare varNames for Block
		
		addIntrinsicVar(class_fundef, "raw1", &o_nil);
		addIntrinsicVar(class_fundef, "raw2", &o_nil);
		addIntrinsicVar(class_fundef, "code", &o_nil);
		addIntrinsicVar(class_fundef, "literals", &o_nil);
		
		addIntrinsicVar(class_fundef, "prototypeFrame", &o_nil);
		addIntrinsicVar(class_fundef, "context", &o_nil);
		addIntrinsicVar(class_fundef, "argNames", &o_nil);
		addIntrinsicVar(class_fundef, "varNames", &o_nil);
	
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

	class_process = makeIntrinsicClass(s_process, s_object, 6, 0);
		addIntrinsicVar(class_process, "classVars", &o_nil);
		addIntrinsicVar(class_process, "interpreter", &o_nil);
		addIntrinsicVar(class_process, "curThread", &o_nil);
		addIntrinsicVar(class_process, "mainThread", &o_nil);
		addIntrinsicVar(class_process, "processID", &o_zero);
		addIntrinsicVar(class_process, "schedulerQueue", &o_nil);

	class_absfunc = makeIntrinsicClass(s_absfunc, s_object, 0, 0); 
	class_stream = makeIntrinsicClass(s_stream, s_absfunc, 0, 0); 

	class_thread = makeIntrinsicClass(s_thread, s_stream, 20, 0);
		addIntrinsicVar(class_thread, "state", &o_nil);
		addIntrinsicVar(class_thread, "func", &o_nil);
		addIntrinsicVar(class_thread, "stack", &o_nil);
		addIntrinsicVar(class_thread, "stackSize", &o_zero);
		addIntrinsicVar(class_thread, "method", &o_nil);
		addIntrinsicVar(class_thread, "block", &o_nil);
		addIntrinsicVar(class_thread, "frame", &o_nil);
		addIntrinsicVar(class_thread, "ip", &o_zero);
		addIntrinsicVar(class_thread, "sp", &o_zero);
		addIntrinsicVar(class_thread, "top", &o_zero);
		addIntrinsicVar(class_thread, "numpop", &o_zero);
		addIntrinsicVar(class_thread, "returnLevels", &o_zero);
		addIntrinsicVar(class_thread, "receiver", &o_nil);
		addIntrinsicVar(class_thread, "numArgsPushed", &o_zero);
		addIntrinsicVar(class_thread, "parent", &o_nil);
		addIntrinsicVar(class_thread, "terminalValue", &o_nil);
		
		addIntrinsicVar(class_thread, "primitiveError", &o_zero);
		addIntrinsicVar(class_thread, "primitiveIndex", &o_zero);
		addIntrinsicVar(class_thread, "randData", &o_zero);
		addIntrinsicVar(class_thread, "time", &o_fzero);

	class_finalizer = makeIntrinsicClass(s_finalizer, s_object, 2, 0);
		addIntrinsicVar(class_finalizer, "cFunction", &o_nil);
		addIntrinsicVar(class_finalizer, "object", &o_nil);

	class_routine = makeIntrinsicClass(s_routine, s_thread, 0, 0);
	class_task = makeIntrinsicClass(s_task, s_routine, 0, 0);

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
	class_inf = makeIntrinsicClass(s_inf, s_magnitude, 0, 0);

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
	class_string = makeIntrinsicClass(s_string, s_rawarray, 0, 0);
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
#if 0
	class_signalnode = makeIntrinsicClass(s_signalnode, s_signal, 1, 0);
		SetInt(&class_signalnode->instanceFormat, obj_float)
		class_signalnode->classFlags.ui |= classHasIndexableInstances;
		addIntrinsicVar(class_signalnode, "nextNode", &o_nil);
	class_multichannode = makeIntrinsicClass(getsym("MultiChannelNode"), s_array, 1, 0);
		SetInt(&class_multichannode->instanceFormat, obj_slot)
		class_multichannode->classFlags.ui |= classHasIndexableInstances;
		addIntrinsicVar(class_multichannode, "nextNode", &o_nil);
#endif
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
	gTagClassTable[ 2] = class_frame;
	gTagClassTable[ 3] = class_frame;
	gTagClassTable[ 4] = class_int;
	gTagClassTable[ 5] = class_symbol;
	gTagClassTable[ 6] = class_char;
	gTagClassTable[ 7] = class_nil;
	gTagClassTable[ 8] = class_false;
	gTagClassTable[ 9] = class_true;
	gTagClassTable[10] = class_inf;
	gTagClassTable[11] = class_rawptr;
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

PyrObject* instantiateObject(class GC *gc, PyrClass* classobj, int size, 
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

PyrObject* instantiateObjectLight(class GC *gc, PyrClass* classobj, int size, bool collect);
PyrObject* instantiateObjectLight(class GC *gc, PyrClass* classobj, int size, bool collect)
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

PyrObject* copyObject(class GC *gc, PyrObject *inobj, bool collect)
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

PyrObject* copyObjectRange(class GC *gc, PyrObject *inobj, int start, int end, bool collect)
{
	PyrObject *newobj;
 
 	if (start < 0) start = 0;
 	if (end > inobj->size) end = inobj->size;
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

void dumpObject(PyrObject *obj)
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
					double val;
					val = obj->slots[i].uf;
					post("    %3d : %.14g\n", i, val);
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
	//flushPostBuf();
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
					double val;
					val = obj->slots[i].uf;
					postfl("    %3d : %.14g\n", i, val);
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
	//flushPostBuf();
}

void dumpObjectSlot(PyrSlot *slot)
{
	if (slot->utag == tagObj) {
		dumpObject(slot->uo);
	//} else if (slot->utag == tagHFrame) {
	//	dumpPyrFrame(slot->uof);
	} else {
		dumpPyrSlot(slot);
	}
}

void CallStackSanity(VMGlobals *g);
void CallStackSanity(VMGlobals *g)
{
	PyrFrame *frame;
	frame = g->frame;
	while (frame) {
		if (FrameSanity(frame)) {
			DumpBackTrace(g);
			//Debugger();
			break;
		}
		frame = frame->caller.uof;
	}
}

bool FrameSanity(PyrFrame *frame);
bool FrameSanity(PyrFrame *frame)
{	
	bool failed = false;
	if (frame==NULL) return false;
	if (frame->method.utag != tagObj) {
		//flushPostBuf();
		postfl("Frame %X method tag wrong %X\n", frame, frame->method.utag);
		failed = true;
	//} else if (!isKindOf((PyrObject*)frame->method.uo->classptr, class_fundef)) {
	} else if (frame->method.uo->classptr != class_method && frame->method.uo->classptr != class_fundef) {
		//flushPostBuf();
		postfl("Frame %X method class wrong %X\n", frame, frame->method.uo->classptr);
		failed = true;
		//if (frame->method.uo->classptr->classptr == class_class) {
			postfl("class: '%s'\n", frame->method.uo->classptr->name.us->name);
		///} else {
		//	postfl("not even a class\n");
		//}
	} else if (frame->method.uoblk->code.utag != tagObj) {
		//flushPostBuf();
		postfl("Method %X code tag wrong %X\n", frame->method.uoblk, frame->method.uoblk->code.utag);
		failed = true;
	} else if (frame->method.uoblk->code.uo->classptr != class_int8array) {
		postfl("Code %X class wrong %X\n", frame->method.uoblk->code.uo, frame->method.uoblk->code.uo->classptr);
			postfl("class: '%s'\n", frame->method.uoblk->code.uo->classptr->name.us->name);
		failed = true;
	}
	if (frame->caller.utag != tagHFrame && frame->caller.utag != tagSFrame && frame->caller.utag != tagNil) {
		//flushPostBuf();
		postfl("Frame %X caller tag wrong %X\n", frame, frame->caller.utag);
		failed = true;
	}
	if (frame->context.utag != tagHFrame && frame->context.utag != tagSFrame && frame->context.utag != tagNil) {
		//flushPostBuf();
		postfl("Frame %X context tag wrong %X\n", frame, frame->context.utag);
		failed = true;
	}
	if (frame->homeContext.utag != tagHFrame && frame->homeContext.utag != tagSFrame && frame->homeContext.utag != tagNil) {
		//flushPostBuf();
		postfl("Frame %X homeContext tag wrong %X\n", frame, frame->homeContext.utag);
		failed = true;
	}
	if (frame->ip.utag != tagInt) {
		//flushPostBuf();
		postfl("Frame %X ip tag wrong %X\n", frame, frame->ip.utag);
		failed = true;
	}
	return failed;
}

#if 1
void DumpFrame(PyrFrame *frame)
{
	char str[256];
	int i, k, numargs;
	PyrMethod *meth;
	PyrMethodRaw *methraw;
	
	if (FrameSanity(frame)) {
		post("FRAME CORRUPTED\n");
		return;
	}
	slotOneWord(&frame->method, str);
	//slotString(&frame->method, str);

	meth = frame->method.uom;
	methraw = METHRAW(meth);
	if (methraw->numtemps) {
		post("\t%s\n", str);
		k = methraw->numtemps - 1;
		numargs = methraw->numargs + methraw->varargs;
		for (i=0; i<=k; ++i) {
			slotOneWord(frame->vars + i - k, str);
			//slotString(frame->vars + i - k, str);
			if (i < numargs) {
				post("\t\targ %s = %s\n", meth->argNames.uosym->symbols[i]->name, str);
			} else {
				post("\t\tvar %s = %s\n", meth->varNames.uosym->symbols[i - numargs]->name, str);
			}
		}
	} else {
		post("\t%s  (no arguments or variables)\n", str);
	}
}

void DumpDetailedFrame(PyrFrame *frame);
void DumpDetailedFrame(PyrFrame *frame)
{
	char mstr[256];
	char str[256];
	int i, k, numargs;
	PyrMethod *meth;
	PyrMethodRaw *methraw;
	
	if (FrameSanity(frame)) {
		post("FRAME CORRUPTED\n");
		return;
	}
	slotOneWord(&frame->method, mstr);
	//slotString(&frame->method, str);

	meth = frame->method.uom;
	methraw = METHRAW(meth);
	
	if (methraw->numtemps) {
		post("\t%s\n", mstr);
		k = methraw->numtemps - 1;
		numargs = methraw->numargs + methraw->varargs;
		for (i=0; i<=k; ++i) {
			slotOneWord(frame->vars + i - k, str);
			//slotString(frame->vars + i - k, str);
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
	
	slotString(&frame->myself, str);		post("\t\tmyself  = %s\n", str);
	slotString(&frame->method, str);		post("\t\tmethod  = %s\n", str);
	slotString(&frame->caller, str);		post("\t\tcaller  = %s\n", str);
	slotString(&frame->context, str);		post("\t\tcontext = %s\n", str);
	slotString(&frame->homeContext, str);	post("\t\thomeCtx = %s\n", str);
	slotString(&frame->ip, str);			post("\t\tip      = %s\n", str);
}

#if 0
void DumpFrame(PyrFrame *frame)
{
	char str[256];
	int i, k, numargs;
	PyrMethod *meth;
	PyrMethodRaw *methraw;
	PyrSlot *vars;
	
	if (FrameSanity(frame)) {
		post("FRAME CORRUPTED\n");
		return;
	}
	slotOneWord(&frame->method, str);
	//slotString(&frame->method, str);
	post("\t%s ( ", str);

	meth = frame->method.uom;
	methraw = METHRAW(meth);
	if (methraw->numtemps) {
		k = methraw->numtemps - 1;
		numargs = methraw->numargs + methraw->varargs;
		for (i=0; i<=k; ++i) {
			slotOneWord(frame->vars + i - k, str);
			//slotString(frame->vars + i - k, str);
			if (i < numargs) {
				post("%s = %s", meth->argNames.uosym->symbols[i]->name, str);
				if (i < numargs-1) post(", ");
				else post(" );\n");
			} else if (i == numargs) {
				if (numargs < methraw->numtemps) post("\t\tvar  ");
				goto below;
			} else {
				below:
				post("%s = %s", meth->varNames.uosym->symbols[i - numargs]->name, str);
				if (i < k) post(", ");
				else post(" ;\n");
			}
		}
	} else post(" );\n");
}
#endif
#else
void DumpFrame(PyrFrame *frame)
{
	char str[256];
	int i, k;
	PyrMethod *meth;
	PyrMethodRaw *methraw;
	PyrSlot *vars;
	
	if (FrameSanity(frame)) {
		post("FRAME CORRUPTED\n");
		return;
	}
	slotString(&frame->method, str);
	post("FRAME:   %s\n", str);

	//post("FRAME: %X\n", frame);
	//post("   method : %s\n", str);

#if 0
	// normal users don't need to know this stuff..
	slotString(&frame->caller, str);
	post("   caller : %s\n", str);

	slotString(&frame->context, str);
	post("   context : %s\n", str);

	slotString(&frame->homeContext, str);
	post("   homeContext : %s\n", str);

	slotString(&frame->ip, str);
	post("   ip : %s\n", str);
#endif	
/*
	meth = frame->method.uom;
	numtemps = meth->numtemps;
	vars = frame->vars - numtemps + 1;
	for (i=0; i<numtemps; ++i) {
		slotString(vars + i, str);
		post("   var %d : %s\n", i, str);
	}
*/
	meth = frame->method.uom;
	methraw = METHRAW(meth);
	k = methraw->numtemps - 1;
	if (k > 64) k = 64;
	for (i=0; i<=k; ++i) {
		slotString(frame->vars + i - k, str);
		if (i< methraw->numargs + methraw->varargs) {
			post("   %2d arg %s : %s\n", i, meth->argNames.uosym->symbols[i]->name, str);
		} else {
			post("   %2d var %s : %s\n", i, 
				meth->varNames.uosym->symbols[i - methraw->numargs - methraw->varargs]->name, str);
		}
	}
}
#endif	


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
		obj->slots[obj->size++].ucopy = slot->ucopy;
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

PyrObject* newPyrObject(class GC *gc, size_t inNumBytes, int inFlags, int inFormat, bool inCollect) 
{
	return gc->New(inNumBytes, inFlags, inFormat, inCollect);
}

PyrObject* newPyrArray(class GC *gc, int size, int flags, bool collect) 
{
	PyrObject* array;

	if (!gc) array = (PyrObject*)GC::NewPermanent(size * sizeof(PyrSlot), flags, obj_slot);
	else array = (PyrObject*)gc->New(size * sizeof(PyrSlot), flags, obj_slot, collect);
	array->classptr = class_array;
	return array;
}

PyrSymbolArray* newPyrSymbolArray(class GC *gc, int size, int flags, bool collect) 
{
	PyrSymbolArray* array;

	if (!gc) array = (PyrSymbolArray*)GC::NewPermanent(size * sizeof(PyrSlot), flags, obj_symbol);
	else array = (PyrSymbolArray*)gc->New(size * sizeof(PyrSymbol*), flags, obj_symbol, collect);
	array->classptr = class_symbolarray;
	return array;
}

PyrInt8Array* newPyrInt8Array(class GC *gc, int size, int flags, bool collect) 
{
	PyrInt8Array* array;

	if (!gc) array = (PyrInt8Array*)GC::NewPermanent(size, flags, obj_int8);
	else array = (PyrInt8Array*)gc->New(size, flags, obj_int8, collect);
	array->classptr = class_int8array;
	return array;
}

PyrInt32Array* newPyrInt32Array(class GC *gc, int size, int flags, bool collect) 
{
	PyrInt32Array* array;

	if (!gc) array = (PyrInt32Array*)GC::NewPermanent(size, flags, obj_int32);
	else array = (PyrInt32Array*)gc->New(size, flags, obj_int32, collect);
	array->classptr = class_int32array;
	return array;
}

PyrString* newPyrString(class GC *gc, const char *s, int flags, bool collect)
{
	PyrString* string;
	int length = strlen(s);
	
	if (!gc) string = (PyrString*)GC::NewPermanent(length, flags, obj_char);
	else string = (PyrString*)gc->New(length, flags, obj_char, collect);
	string->classptr = class_string;
	string->size = length;
	memcpy(string->s, s, length);	
	return string;
}

PyrString* newPyrStringN(class GC *gc, int length, int flags, bool collect)
{
	PyrString* string;
	
	if (!gc) string = (PyrString*)GC::NewPermanent(length, flags, obj_char);
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
	
	if (!compilingCmdLine) block = (PyrBlock*)GC::NewPermanent(numbytes, flags, obj_notindexed);
	else block = (PyrBlock*)gMainVMGlobals->gc->New(numbytes, flags, obj_notindexed, false);
	block->classptr = class_fundef;
	block->size = numSlots;
	
	// clear out raw area
	methraw = METHRAW(block);
	methraw->specialIndex = 0;
	methraw->methType = methBlock;
#if USESTACKFRAMES
	methraw->needsHeapContext = 0;
#else
	methraw->needsHeapContext = 1;
#endif
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

PyrMethod* newPyrMethod() 
{
	PyrMethod* method;
	PyrMethodRaw* methraw;
	
	int32 numbytes = sizeof(PyrMethod) - sizeof(PyrObjectHdr);
	int32 numSlots = numbytes / sizeof(PyrSlot);

	method = (PyrMethod*)GC::NewPermanent(numbytes, obj_permanent | obj_immutable, obj_notindexed);
	method->classptr = class_method;
	method->size = 0;
	method->size = numSlots;
	
	// clear out raw area
	methraw = METHRAW(method);
	//post("newPyrMethod %08X %08X %08X %d\n", method, methraw, (char*)method + sizeof(PyrMethod), (PyrSlot*)methraw - (PyrSlot*)method);
	
	methraw->specialIndex = 0;
	methraw->methType = 0;
#if USESTACKFRAMES
	methraw->needsHeapContext = 0;
#else
	methraw->needsHeapContext = 1;
#endif
	methraw->frameSize = 0;
	methraw->varargs = 0;
	methraw->numargs = 0;
	methraw->numvars = 0;
	methraw->numtemps = 0;

	nilSlots(&method->rawData1,  numSlots);
	//method->byteMeter.ucopy = o_zero.ucopy;
	//method->callMeter.ucopy = o_zero.ucopy;
	//post("<- newPyrMethod %08X %08X\n", method, methraw);
	return method;
}

void freePyrSlot(PyrSlot *slot) 
{
	if (NotNil(slot)) {
		PyrObject *obj;
		obj = slot->uo;
		if (obj && obj->IsPermanent()) {
			pyr_pool_runtime->Free((void*)obj);
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


bool IsSharableObject(PyrObject *obj)
{
	if (isKindOf(obj, class_rawarray)) {
		return obj->obj_format != obj_double;
	} else if (obj->classptr == class_fundef) {
		return IsNil(&((PyrBlock*)obj)->context);
	/*} else if (obj->classptr == class_func) {
		PyrSlot *fundef = &((PyrClosure*)obj)->block;
		return IsObj(fundef) && IsNil(&fundef->uoblk->context);*/
	} else {
		return obj->obj_flags & obj_immutable;
	}
}

bool IsSharableSlot(PyrSlot *slot)
{
	if (IsObj(slot)) return IsSharableObject(slot->uo); 
	return !IsFrame(slot);
}

bool IsPermanentSlot(PyrSlot *slot)
{
	if (IsObj(slot)) return slot->uo->gc_color == obj_permanent; 
	return !IsFrame(slot);
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
			a->ucopy = obj->slots[index].ucopy;
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
			slot->ucopy = c->ucopy;
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
	int hash, z;
	switch (a->utag) {
		case tagObj : hash = Hash(a->ui); break;
		case tagHFrame : hash = Hash(a->ui); break;
		case tagSFrame : hash = Hash(a->ui); break;
		case tagInt : hash = Hash(a->ui); break;
		case tagChar : z = Hash(a->ui & 255); break;
		case tagSym : hash = a->us->hash; break;
		case tagNil : hash = 0xA5A5A5A5; break;
		case tagFalse : hash = 0x55AA55AA; break;
		case tagTrue : hash = 0x69696969; break;
		case tagInf : hash = 0x78912347; break;
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
