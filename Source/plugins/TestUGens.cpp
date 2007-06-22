/*
 *  TestUGens.cpp
 *  Plugins
 *
 *  Created by Scott Wilson on 22/06/2007.
 * 
 *
 */

#include "SC_PlugIn.h"
#include "SC_PlugIn.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

// Visual C++ doesn't have fpclassify (C99), so define it here if needed
#ifdef SC_WIN32
#include <float.h>
enum { FP_NORMAL, FP_NAN, FP_INFINITE, FP_SUBNORMAL };
int fpclassify(float x);
int fpclassify(float x) {
	int result;
	int kind = _fpclass((double)x);
	switch (kind) 
	{ 
		case _FPCLASS_NINF: 
			result = FP_INFINITE;
			break;
		case _FPCLASS_PINF: 
			result = FP_INFINITE;
			break; 
		case _FPCLASS_SNAN: 
			result = FP_NAN;
			break;
		case _FPCLASS_QNAN: 
			result = FP_NAN;
			break;  
		case _FPCLASS_ND:
			result = FP_SUBNORMAL;
			break;
		case _FPCLASS_PD:
			result = FP_SUBNORMAL;
			break;
		default: 
			result = FP_NORMAL;
	};
	return result;
	
}

#endif // SC_WIN32

//////////////////////////////////////////////////////////////////////////////////////////////////

static InterfaceTable *ft;


struct CheckBadValues : public Unit {};

// declare unit generator functions 
extern "C"
{
	void load(InterfaceTable *inTable);
	
	void CheckBadValues_Ctor(CheckBadValues* unit);
	void CheckBadValues_next(CheckBadValues* unit, int inNumSamples);
	
};

//////////////////////////////////////////////////////////////////////////////////////////////////

void CheckBadValues_Ctor(CheckBadValues* unit)
{	
	SETCALC(CheckBadValues_next);
}


void CheckBadValues_next(CheckBadValues* unit, int inNumSamples)
{
	
	
	float *in = ZIN(0);
	float *out = ZOUT(0);
	float id = ZIN0(1);
	float post = ZIN0(2);
	
	float samp, output;
	int classification;
	
	LOOP(inNumSamples,
		 samp = ZXP(in);
		 classification = fpclassify(samp);
		 switch (classification) 
		 { 
			 case FP_INFINITE: 
				 if(post) printf("Infinite number found in Synth %d, ID: %d\n", unit->mParent->mNode.mID, (int)id); 
				 output = 2;
				 break; 
			 case FP_NAN: 
				 if(post) printf("NaN found in Synth %d, ID: %d\n", unit->mParent->mNode.mID, (int)id); 
				 output = 1;
				 break; 
			 case FP_SUBNORMAL:
				 if(post) printf("Denormal found in Synth %d, ID: %d\n", unit->mParent->mNode.mID, (int)id); 
				 output = 3;
				 break;
			 default: 
				 output = 0;
		 };
		 
		 ZXP(out) = output;
		 );
}


////////////////////////////////////////////////////////////////////

// the load function is called by the host when the plug-in is loaded
void load(InterfaceTable *inTable)
{
	ft = inTable;
	
	DefineSimpleUnit(CheckBadValues);
}


