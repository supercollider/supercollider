/*
 *  TestUGens.cpp
 *  Plugins
 *
 *  Created by Scott Wilson on 22/06/2007.
 *  Modified by James Harkins on 28/07/2007.
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


struct CheckBadValues : public Unit {
	long	sameCount;
	int		prevclass;
};

// declare unit generator functions 
extern "C"
{
	void load(InterfaceTable *inTable);
	
	void CheckBadValues_Ctor(CheckBadValues* unit);
	void CheckBadValues_next(CheckBadValues* unit, int inNumSamples);
	const char *CheckBadValues_fpclassString(int fpclass);
	inline int CheckBadValues_fold_fpclasses(int fpclass);
};

//////////////////////////////////////////////////////////////////////////////////////////////////

void CheckBadValues_Ctor(CheckBadValues* unit)
{
	unit->prevclass = FP_NORMAL;
	unit->sameCount = 0;
	SETCALC(CheckBadValues_next);
}


void CheckBadValues_next(CheckBadValues* unit, int inNumSamples)
{
	
	
	float *in = ZIN(0);
	float *out = ZOUT(0);
	float id = ZIN0(1);
	int post = (int) ZIN0(2);
	
	float samp;
	int classification;
	
	switch(post) {
		case 1:		// post a line on every bad value
			LOOP(inNumSamples,
				 samp = ZXP(in);
				 classification = std::fpclassify(samp);
				 switch (classification) 
				 { 
					 case FP_INFINITE: 
						 printf("Infinite number found in Synth %d, ID: %d\n", unit->mParent->mNode.mID, (int)id); 
						 ZXP(out) = 2;
						 break; 
					 case FP_NAN: 
						 printf("NaN found in Synth %d, ID: %d\n", unit->mParent->mNode.mID, (int)id); 
						 ZXP(out) = 1;
						 break; 
					 case FP_SUBNORMAL:
						 printf("Denormal found in Synth %d, ID: %d\n", unit->mParent->mNode.mID, (int)id); 
						 ZXP(out) = 3;
						 break;
					 default: 
						 ZXP(out) = 0;
				 };
			 );
			 break;
		case 2:
			LOOP(inNumSamples,
				samp = ZXP(in);
				classification = CheckBadValues_fold_fpclasses(std::fpclassify(samp));
				if(classification != unit->prevclass) {
					if(unit->sameCount == 0) {
						printf("CheckBadValues: %s found in Synth %d, ID %d\n",
							CheckBadValues_fpclassString(classification), unit->mParent->mNode.mID, (int)id);
					} else {
						printf("CheckBadValues: %s found in Synth %d, ID %d (previous %d values were %s)\n",
							CheckBadValues_fpclassString(classification), unit->mParent->mNode.mID, (int)id,
							(int)unit->sameCount, CheckBadValues_fpclassString(unit->prevclass)
						);
					};
					unit->sameCount = 0;
				};
				switch (classification) 
				{ 
					case FP_INFINITE: 
						ZXP(out) = 2;
						break; 
					case FP_NAN: 
						ZXP(out) = 1;
						break; 
					case FP_SUBNORMAL:
						ZXP(out) = 3;
						break;
					default: 
						ZXP(out) = 0;
				};
				unit->sameCount++;
				unit->prevclass = classification;
			);
			break;
		default:		// no post
			LOOP(inNumSamples,
				 samp = ZXP(in);
				 classification = std::fpclassify(samp);
				 switch (classification) 
				 { 
					 case FP_INFINITE: 
						 ZXP(out) = 2;
						 break; 
					 case FP_NAN: 
						 ZXP(out) = 1;
						 break; 
					 case FP_SUBNORMAL:
						 ZXP(out) = 3;
						 break;
					 default: 
						 ZXP(out) = 0;
				 };
			 );
			 break;
	}
}

const char *CheckBadValues_fpclassString(int fpclass)
{
	switch(fpclass) {
		case FP_NORMAL:       return "normal";
		case FP_NAN:          return "NaN";
		case FP_INFINITE:     return "infinity";
		case FP_ZERO:         return "zero";
		case FP_SUBNORMAL:    return "denormal";
		default:              return "unknown";
	}
}

inline int CheckBadValues_fold_fpclasses(int fpclass)
{
	switch(fpclass) {
		case FP_ZERO:   return FP_NORMAL; // a bit hacky. we mean "zero is fine too".
		default:        return fpclass;
	}
}


////////////////////////////////////////////////////////////////////

// the load function is called by the host when the plug-in is loaded
void load(InterfaceTable *inTable)
{
	ft = inTable;
	
	DefineSimpleUnit(CheckBadValues);
}


