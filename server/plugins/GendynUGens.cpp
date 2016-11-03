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

//Gendyn UGens implemented by Nick Collins

#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct Gendy1 : public Unit   // Iannis Xenakis/Marie-Helene Serra GENDYN simulation
{
	double mPhase;
	float mFreqMul, mAmp, mNextAmp, mSpeed, mDur;
	int mMemorySize, mIndex;
	float* mMemoryAmp; 	//could hard code as 12
	float* mMemoryDur;
};

//following Hoffmann paper from CMJ- primary and secondary random walks
struct Gendy2 : public Unit
{
	double mPhase;
	float mFreqMul, mAmp, mNextAmp, mSpeed, mDur;
	int mMemorySize, mIndex;
	float* mMemoryAmp;
	float* mMemoryAmpStep;
	float* mMemoryDur;
	float* mMemoryDurStep;
};


//Random walks as Gendy1 but works out all breakpoints per cycle and normalises time intervals to desired frequency
struct Gendy3 : public Unit
{
	double mPhase, mNextPhase, mLastPhase;
	float mSpeed, mFreqMul;
	float mAmp, mNextAmp, mInterpMult;
	int mMemorySize, mIndex;
	float* mMemoryAmp;
	float* mMemoryDur;
	double* mPhaseList;
	float* mAmpList;
};

extern "C" {

	void Gendy1_next_k(Gendy1 *unit, int inNumSamples);
	void Gendy1_Ctor(Gendy1* unit);
	void Gendy1_Dtor(Gendy1* unit);

	void Gendy2_next_k(Gendy2 *unit, int inNumSamples);
	void Gendy2_Ctor(Gendy2* unit);
	void Gendy2_Dtor(Gendy2* unit);

	void Gendy3_next_k(Gendy3 *unit, int inNumSamples);
	void Gendy3_Ctor(Gendy3* unit);
	void Gendy3_Dtor(Gendy3* unit);
}


void Gendy1_Ctor( Gendy1* unit )
{
	SETCALC(Gendy1_next_k);

	unit->mFreqMul = unit->mRate->mSampleDur;
	unit->mPhase = 1.0;	//should immediately decide on new target
	unit->mAmp = 0.0f;
	unit->mNextAmp = 0.0f;
	unit->mSpeed = 100.f;

	unit->mMemorySize = (int) ZIN0(8);	//default is 12
	//printf("memsize %d %f", unit->mMemorySize, ZIN0(8));
	if(unit->mMemorySize<1)
		unit->mMemorySize = 1;
	unit->mIndex = 0;
	unit->mMemoryAmp = (float*)RTAlloc(unit->mWorld, unit->mMemorySize * sizeof(float));
	unit->mMemoryDur = (float*)RTAlloc(unit->mWorld, unit->mMemorySize * sizeof(float));

	RGen& rgen = *unit->mParent->mRGen;

	//initialise to zeroes and separations
	for(int i=0; i<unit->mMemorySize;++i) {
		unit->mMemoryAmp[i] = 2*rgen.frand() - 1.0f;
		unit->mMemoryDur[i] = rgen.frand();
	}

	// compute one sample of output to avoid sending garbage memory downstream to other Ctor functions
	// first sample of the _next output will be the current amplitude (which is 0)
	OUT0(0) = 0.0f;
}

void Gendy1_Dtor(Gendy1 *unit)
{
	RTFree(unit->mWorld, unit->mMemoryAmp);
	RTFree(unit->mWorld, unit->mMemoryDur);
}


//called once per period so OK to work out constants in here
static float Gendyn_distribution( int which, float a, float f)
{
	float temp, c;

	if(a>1.f) a = 1.f;       //a must be in range 0 to 1
	if(a<0.0001f) a = 0.0001f; 	//for safety with some distributions, don't want divide by zero errors

	switch (which)
	{
	case 0: //LINEAR
			//linear
		break;
	case 1: //CAUCHY
		//X has a*tan((z-0.5)*pi)
		//I went back to first principles of the Cauchy distribution and re-integrated with a
		//normalisation constant

		//choice of 10 here is such that f=0.95 gives about 0.35 for temp, could go with 2 to make it finer
		c = atan(10*a);		//PERHAPS CHANGE TO a=1/a;
		//incorrect- missed out divisor of pi in norm temp= a*tan(c*(2*pi*f - 1));
		temp = (1.f/a)*tan(c*(2.f*f - 1.f));	//Cauchy distribution, C is precalculated

		//printf("cauchy f %f c %f temp %f out %f \n",f,  c, temp, temp/10);

		return temp*0.1f; //(temp+100)/200;

	case 2: //LOGIST (ic)
		//X has -(log((1-z)/z)+b)/a which is not very usable as is

		c = 0.5f+(0.499f*a); //calculate normalisation constant
		c = log((1.f-c)/c);

		//remap into range of valid inputs to avoid infinities in the log

		//f= ((f-0.5)*0.499*a)+0.5;
		f = ((f-0.5f)*0.998f*a)+0.5f; //[0,1]->[0.001,0.999]; squashed around midpoint 0.5 by a
		//Xenakis calls this the LOGIST map, it's from the range [0,1] to [inf,0] where 0.5->1
		//than take natural log. to avoid infinities in practise I take [0,1] -> [0.001,0.999]->[6.9,-6.9]
		//an interesting property is that 0.5-e is the reciprocal of 0.5+e under (1-f)/f
		//and hence the logs are the negative of each other
		temp = log((1.f-f)/f)/c;	//n range [-1,1]
		//X also had two constants in his- I don't bother

		//printf("logist f %f temp %f\n", f, temp);

		return temp; //a*0.5*(temp+1.0);	//to [0,1]

	case 3: //HYPERBCOS
		//X original a*log(tan(z*pi/2)) which is [0,1]->[0,pi/2]->[0,inf]->[-inf,inf]
		//unmanageable in this pure form
		c = tan(1.5692255f*a);    //tan(0.999*a*pi*0.5);    	//[0, 636.6] maximum range
		temp = tan(1.5692255f*a*f)/c;	//[0,1]->[0,1]
		temp = log(temp*0.999f+0.001f)*(-0.1447648f);  // multiplier same as /(-6.9077553); //[0,1]->[0,1]

		//printf("hyperbcos f %f c %f temp %f\n", f, c, temp);

		return 2.f*temp-1.0f;

	case 4: //ARCSINE
		//X original a/2*(1-sin((0.5-z)*pi)) aha almost a better behaved one though [0,1]->[2,0]->[a,0]
		c = sin(1.5707963f*a); //sin(pi*0.5*a);	//a as scaling factor of domain of sine input to use
		temp = sin(pi_f*(f-0.5f)*a)/c; //[-1,1] which is what I need

		//printf("arcsine f %f c %f temp %f\n", f, c, temp);

		return temp;

		case 5: //EXPON
		//X original -(log(1-z))/a [0,1]-> [1,0]-> [0,-inf]->[0,inf]
		c = log(1.f-(0.999f*a));
		temp = log(1.f-(f*0.999f*a))/c;

		//printf("expon f %f c %f temp %f\n", f, c, temp);

		return 2.f*temp-1.f;

	case 6: //SINUS
		//X original a*sin(smp * 2*pi/44100 * b) ie depends on a second oscillator's value-
		//hmmm, plug this in as a I guess, will automatically accept control rate inputs then!
		return 2.f*a-1.f;

	default:
		break;
	}

	return 2.f*f-1.f;
}


void Gendy1_next_k( Gendy1 *unit, int inNumSamples )
{
	float *out = ZOUT(0);

	//distribution choices for amp and dur and constants of distribution
	int whichamp = (int)ZIN0(0);
	int whichdur = (int)ZIN0(1);
	float aamp = ZIN0(2);
	float adur = ZIN0(3);
	float minfreq = ZIN0(4);
	float maxfreq = ZIN0(5);
	float scaleamp = ZIN0(6);
	float scaledur = ZIN0(7);

	float rate = unit->mDur;

	//phase gives proportion for linear interpolation automatically
	double phase = unit->mPhase;

	float amp = unit->mAmp;
	float nextamp = unit->mNextAmp;

	float speed = unit->mSpeed;

	RGen& rgen = *unit->mParent->mRGen;
	//linear distribution 0.0 to 1.0 using rgen.frand()

	LOOP1(inNumSamples,
		float z;

		if (phase >= 1.0) {
			phase -= 1.0;

			int index = unit->mIndex;
			int num = (int)(ZIN0(9));//(unit->mMemorySize);(((int)ZIN0(9))%(unit->mMemorySize))+1;
			if((num>(unit->mMemorySize)) || (num<1)) num=unit->mMemorySize;

			//new code for indexing
			index = (index+1)%num;
			amp = nextamp;

			unit->mIndex = index;

			//Gendy dist gives value [-1,1], then use scaleamp
			//first term was amp before, now must check new memory slot
			nextamp = (unit->mMemoryAmp[index])+(scaleamp*Gendyn_distribution(whichamp, aamp,rgen.frand()));

			//mirroring for bounds- safe version
			if(nextamp>1.0f || nextamp<-1.0f) {

			//printf("mirroring nextamp %f ", nextamp);

			//to force mirroring to be sensible
			if(nextamp<0.0f) nextamp = nextamp+4.f;

			nextamp = fmod(nextamp,4.f);
			//printf("fmod  %f ", nextamp);

			if(nextamp>1.0f && nextamp<3.f)
				nextamp = 2.f-nextamp;

			else if(nextamp>1.0f)
				nextamp = nextamp-4.f;

			//printf("mirrorednextamp %f \n", nextamp);
			};

			unit->mMemoryAmp[index] = nextamp;

			//Gendy dist gives value [-1,1]
			rate= (unit->mMemoryDur[index]) + (scaledur*Gendyn_distribution(whichdur, adur, rgen.frand()));

			if(rate>1.0f || rate<0.0f)
			{
				if(rate<0.0) rate = rate+2.f;
				rate = fmod(rate,2.0f);
				rate = 2.f-rate;
			}

			unit->mMemoryDur[index] = rate;

			//printf("nextamp %f rate %f \n", nextamp, rate);

			//define range of speeds (say between 20 and 1000 Hz)
			//can have bounds as fourth and fifth inputs
			speed =  (minfreq+((maxfreq-minfreq)*rate))*(unit->mFreqMul);

			//if there are 12 control points in memory, that is 12 per cycle
			//the speed is multiplied by 12
			//(I don't store this because updating rates must remain in range [0,1]
			speed *= num;
		}

		//linear interpolation could be changed
		z = ((1.0-phase)*amp) + (phase*nextamp);

		phase +=  speed;
		ZXP(out) = z;
	);

	unit->mPhase = phase;
	unit->mAmp =  amp;
	unit->mNextAmp = nextamp;
	unit->mSpeed = speed;
	unit->mDur = rate;
}




void Gendy2_Ctor( Gendy2* unit )
{
	SETCALC(Gendy2_next_k);

	unit->mFreqMul = unit->mRate->mSampleDur;
	unit->mPhase = 1.0;	//should immediately decide on new target
	unit->mAmp = 0.0f;
	unit->mNextAmp = 0.0f;
	unit->mSpeed = 100.f;

	unit->mMemorySize = (int) ZIN0(8);	//default is 12
	//printf("memsize %d %f", unit->mMemorySize, ZIN0(8));
	if(unit->mMemorySize<1) unit->mMemorySize = 1;
	unit->mIndex = 0;
	unit->mMemoryAmp = (float*)RTAlloc(unit->mWorld, unit->mMemorySize * sizeof(float));
	unit->mMemoryDur = (float*)RTAlloc(unit->mWorld, unit->mMemorySize * sizeof(float));
	unit->mMemoryAmpStep = (float*)RTAlloc(unit->mWorld, unit->mMemorySize * sizeof(float));
	unit->mMemoryDurStep = (float*)RTAlloc(unit->mWorld, unit->mMemorySize * sizeof(float));

	RGen& rgen = *unit->mParent->mRGen;

	//initialise to zeroes and separations
	for(int i=0; i<unit->mMemorySize;++i) {
		unit->mMemoryAmp[i] = 2*rgen.frand() - 1.0f;
		unit->mMemoryDur[i] = rgen.frand();
		unit->mMemoryAmpStep[i] = 2*rgen.frand() - 1.0f;
		unit->mMemoryDurStep[i] = 2*rgen.frand() - 1.0f;
	}

	// compute one sample of output to avoid sending garbage memory downstream to other Ctor functions
	// first sample of the _next output will be the current amplitude (which is 0)
	OUT0(0) = 0.0f;
}

void Gendy2_Dtor(Gendy2 *unit)
{
	RTFree(unit->mWorld, unit->mMemoryAmp);
	RTFree(unit->mWorld, unit->mMemoryDur);
	RTFree(unit->mWorld, unit->mMemoryAmpStep);
	RTFree(unit->mWorld, unit->mMemoryDurStep);
}

static float Gendyn_mirroring (float lower, float upper, float in)
{
	//mirroring for bounds- safe version
	if(in>upper || in<lower) {
		float range= (upper-lower);

		if(in<lower)
			in = (2.0f*upper-lower)-in;

		in = fmod(in-upper,2.0f*range);

		if(in<range)
			in = upper-in;
		else
			in = in- (range);
	}

	return in;
}

void Gendy2_next_k( Gendy2 *unit, int inNumSamples )
{
	float *out = ZOUT(0);

	//distribution choices for amp and dur and constants of distribution
	int whichamp = (int)ZIN0(0);
	int whichdur = (int)ZIN0(1);
	float aamp = ZIN0(2);
	float adur = ZIN0(3);
	float minfreq = ZIN0(4);
	float maxfreq = ZIN0(5);
	float scaleamp = ZIN0(6);
	float scaledur = ZIN0(7);

	float rate = unit->mDur;

	//phase gives proportion for linear interpolation automatically
	double phase = unit->mPhase;

	float amp = unit->mAmp;
	float nextamp = unit->mNextAmp;

	float speed = unit->mSpeed;

	RGen& rgen = *unit->mParent->mRGen;

	LOOP1(inNumSamples,
		float z;

		if (phase >= 1.0) {
			phase -= 1.0;

			int index = unit->mIndex;
			int num = (int)(ZIN0(9));//(unit->mMemorySize);(((int)ZIN0(9))%(unit->mMemorySize))+1;
			if((num>(unit->mMemorySize)) || (num<1)) num=unit->mMemorySize;

			//new code for indexing
			index = (index+1)%num;

			//using last amp value as seed
			//random values made using a lehmer number generator xenakis style
			float a = ZIN0(10);
			float c = ZIN0(11);

			float lehmerxen = fmod(((amp)*a)+c,1.0f);

			//printf("lehmer %f \n", lehmerxen);

			amp = nextamp;

			unit->mIndex = index;

			//Gendy dist gives value [-1,1], then use scaleamp
			//first term was amp before, now must check new memory slot

			float ampstep = (unit->mMemoryAmpStep[index])+ Gendyn_distribution(whichamp, aamp, fabs(lehmerxen));
			ampstep = Gendyn_mirroring(-1.0f,1.0f,ampstep);

			unit->mMemoryAmpStep[index] = ampstep;

			nextamp = (unit->mMemoryAmp[index])+(scaleamp*ampstep);

			nextamp = Gendyn_mirroring(-1.0f,1.0f,nextamp);

			unit->mMemoryAmp[index] = nextamp;

			float durstep = (unit->mMemoryDurStep[index])+ Gendyn_distribution(whichdur, adur, rgen.frand());
			durstep = Gendyn_mirroring(-1.0f,1.0f,durstep);

			unit->mMemoryDurStep[index] = durstep;

			rate = (unit->mMemoryDur[index])+(scaledur*durstep);

			rate = Gendyn_mirroring(0.0f,1.0f,rate);

			unit->mMemoryDur[index] = rate;

			//printf("nextamp %f rate %f \n", nextamp, rate);

			//define range of speeds (say between 20 and 1000 Hz)
			//can have bounds as fourth and fifth inputs
			speed = (minfreq+((maxfreq-minfreq)*rate))*(unit->mFreqMul);

			//if there are 12 control points in memory, that is 12 per cycle
			//the speed is multiplied by 12
			//(I don't store this because updating rates must remain in range [0,1]
			speed *= num;
		}

		//linear interpolation could be changed
		z = ((1.0-phase)*amp) + (phase*nextamp);

		phase +=  speed;
		ZXP(out) = z;
	);

	unit->mPhase = phase;
	unit->mAmp =  amp;
	unit->mNextAmp = nextamp;
	unit->mSpeed = speed;
	unit->mDur = rate;
}

void Gendy3_Ctor( Gendy3* unit )
{
	SETCALC(Gendy3_next_k);

	unit->mFreqMul = unit->mRate->mSampleDur;
	unit->mPhase = 1.0;	//should immediately decide on new target
	unit->mAmp = 0.0f;
	unit->mNextAmp = 0.0f;
	unit->mNextPhase = 0.0;
	unit->mLastPhase = 0.0;
	unit->mInterpMult = 1.0f;
	unit->mSpeed = 100.f;

	unit->mMemorySize = (int) ZIN0(7);
	if(unit->mMemorySize<1) unit->mMemorySize = 1;
	unit->mIndex = 0;
	unit->mMemoryAmp = (float*)RTAlloc(unit->mWorld, unit->mMemorySize * sizeof(float));
	unit->mMemoryDur = (float*)RTAlloc(unit->mWorld, unit->mMemorySize * sizeof(float));

	//one more in amp list for guard (wrap) element
	unit->mAmpList = (float*)RTAlloc(unit->mWorld, (unit->mMemorySize+1) * sizeof(float));
	unit->mPhaseList = (double*)RTAlloc(unit->mWorld, (unit->mMemorySize+1) * sizeof(double));

	RGen& rgen = *unit->mParent->mRGen;

	//initialise to zeroes and separations
	for(int i = 0; i<unit->mMemorySize;++i) {
		unit->mMemoryAmp[i] = 2*rgen.frand() - 1.0f;
		unit->mMemoryDur[i] = rgen.frand();
		unit->mAmpList[i] = 2*rgen.frand() - 1.0f;
		unit->mPhaseList[i] = 1.0; //will be intialised immediately
	}

	unit->mMemoryAmp[0] = 0.0f;	//always zeroed first BP

	// compute one sample of output to avoid sending garbage memory downstream to other Ctor functions
	// first sample of the _next output will be the current amplitude (which is 0)
	OUT0(0) = 0.0f;
}

void Gendy3_Dtor(Gendy3 *unit)
{
	RTFree(unit->mWorld, unit->mMemoryAmp);
	RTFree(unit->mWorld, unit->mMemoryDur);
	RTFree(unit->mWorld, unit->mAmpList);
	RTFree(unit->mWorld, unit->mPhaseList);
}

void Gendy3_next_k( Gendy3 *unit, int inNumSamples )
{
	float *out = ZOUT(0);

	//distribution choices for amp and dur and constants of distribution
	int whichamp = (int)ZIN0(0);
	int whichdur = (int)ZIN0(1);
	float aamp = ZIN0(2);
	float adur = ZIN0(3);
	float freq = ZIN0(4);
	float scaleamp = ZIN0(5);
	float scaledur = ZIN0(6);

	double phase = unit->mPhase;
	float amp = unit->mAmp;
	float nextamp= unit->mNextAmp;
	float speed= unit->mSpeed;
	int index = unit->mIndex;
	int interpmult = (int)unit->mInterpMult;
	double lastphase = unit->mLastPhase;
	double nextphase = unit->mNextPhase;

	RGen& rgen = *unit->mParent->mRGen;

	float * amplist = unit->mAmpList;
	double * phaselist = unit->mPhaseList;

	LOOP1(inNumSamples,
		float z;

		if (phase >= 1.) { //calculate all targets for new period
			phase -= 1.;

			int num = (int)(ZIN0(8));
			if((num>(unit->mMemorySize)) || (num<1)) num  = unit->mMemorySize;

			float dursum = 0.0f;

			float * memoryamp = unit->mMemoryAmp;
			float * memorydur = unit->mMemoryDur;

			for(int j = 0; j<num; ++j) {

				if(j>0) {   //first BP always stays at 0
					float amp = (memoryamp[j])+ (scaleamp*Gendyn_distribution(whichamp, aamp, rgen.frand()));
					amp = Gendyn_mirroring(-1.0f,1.0f,amp);
					memoryamp[j] = amp;
				}

				float dur = (memorydur[j])+ (scaledur*Gendyn_distribution(whichdur, adur, rgen.frand()));
				dur = Gendyn_mirroring(0.01f,1.0f,dur);	//will get normalised in a moment, don't allow zeroes
				memorydur[j] = dur;
				dursum += dur;
			}

			//normalising constant
			dursum = 1.f/dursum;

			int active = 0;

			//phase duration of a sample
			float minphase = unit->mFreqMul;

			speed = freq*minphase;

			//normalise and discard any too short (even first)
			for(int j = 0; j<num; ++j) {

				float dur = memorydur[j];
				dur *= dursum;

				if(dur>=minphase) {
					amplist[active] =  memoryamp[j];
					phaselist[active]  =  dur;
					++active;
				}
			}

			//add a zero on the end at active
			amplist[active] = 0.0f; //guard element
			phaselist[active] = 2.0; //safety element

		//lastphase=0.0;
//					nextphase= phaselist[0];
//					amp=amplist[0];
//					nextamp=amplist[1];
//					index=0;
//					unit->mIndex=index;
//
			//setup to trigger next block
			nextphase = 0.0;
			nextamp = amplist[0];
			index = -1;
		}

		if (phase >= nextphase) { //are we into a new region?
			//new code for indexing
			++index; //=index+1; //%num;

			amp  = nextamp;

			unit->mIndex = index;

			lastphase = nextphase;
			nextphase = lastphase+phaselist[index];
			nextamp = amplist[index+1];

			interpmult = (int)(1.0/(nextphase-lastphase));
		}

		float interp = (phase-lastphase)*interpmult;

		//linear interpolation could be changed
		z = ((1.0f-interp)*amp) + (interp*nextamp);

		phase +=  speed;
		ZXP(out) = z;
	);

	unit->mPhase = phase;
	unit->mSpeed = speed;
	unit->mInterpMult = interpmult;
	unit->mAmp = amp;
	unit->mNextAmp = nextamp;
	unit->mLastPhase  = lastphase;
	unit->mNextPhase  = nextphase;
}

PluginLoad(Gendyn)
{
	ft = inTable;

	DefineDtorUnit(Gendy1);
	DefineDtorUnit(Gendy2);
	DefineDtorUnit(Gendy3);
}
