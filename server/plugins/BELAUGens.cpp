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

// ********** this version for windows and linux. for mac see UIUGens.mm

/*
 *  BELA I/O UGens created by nescivi, (c) 2016
 *  https://www.nescivi.eu
 */

// #include <SC_Lock.h>

#include <atomic>

#include "BeagleRT.h"
// Xenomai-specific includes
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/intr.h>
#include <rtdk.h>

#include "SC_PlugIn.h"

static InterfaceTable *ft;


struct AnalogInputUGen : public Unit
{
  int mAudioFramesPerAnalogFrame;
};

struct AnalogOutputUGen : public Unit
{
  int mAudioFramesPerAnalogFrame;
};

// static digital pin, static function (in)
struct DigitalInputUGen : public Unit
{
  int mDigitalPin;
};

// static digital pin, static function (out)
struct DigitalOutputUGen : public Unit
{
  int mDigitalPin;
};

// flexible digital pin, flexible function (in or out)
struct DigitalIOUGen : public Unit
{
  int mLastOutput;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

void AnalogInput_next(AnalogInputUGen *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BeagleRTContext *context = world>mBelaContext;

  float *fin = IN0(0); // analog in pin, can be modulated
  float analogPin;
  float analogValue = 0;
  float *out = ZOUT(0);
  
  // context->audioFrames should be equal to inNumSamples
//   for(unsigned int n = 0; n < context->audioFrames; n++) {
  for(unsigned int n = 0; n < inNumSamples; n++) {
	analogPin = (++*fin);
	if(!(n % unit->mAudioFramesPerAnalogFrame)) {
	  analogValue = analogReadFrame(context, n/unit->mAudioFramesPerAnalogFrame, (int) analogPin);
	}
	*++out = analogValue;
  }
}

void AnalogInput_Ctor(AnalogInputUGen *unit)
{
	BeagleRTContext *context = unit->mWorld->mBelaContext;
  
	if(context->analogFrames == 0 || context->analogFrames > context->audioFrames) {
		rt_printf("Error: the UGen needs BELA analog enabled, with 4 or 8 channels\n");
		return false;
	}

	unit->mAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;

	// initiate first sample
	AnalogInput_next( unit, 1);  
	// set calculation method
	SETCALC(AnalogInput_next);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void AnalogOutput_next(AnalogOutputUGen *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BeagleRTContext *context = world>mBelaContext;

  float *fin = IN0(0); // analog in pin, can be modulated
  float analogPin;
  float *in = IN(1);
  
  float newinput = 0;
//   float analogValue = 0;
  // context->audioFrames should be equal to inNumSamples
//   for(unsigned int n = 0; n < context->audioFrames; n++) {
  for(unsigned int n = 0; n < inNumSamples; n++) {
	// read input
	analogPin = (++*fin);
	newinput = ++*in; // read next input sample
	if(!(n % gAudioFramesPerAnalogFrame)) {
	  analogWriteFrameOnce(context,  n/ unit->mAudioFramesPerAnalogFrame, (int) analogPin, newinput);
	}
  }
}

void AnalogOutput_Ctor(AnalogOutputUGen *unit)
{
	BeagleRTContext *context = unit->mWorld->mBelaContext;
  
	if(context->analogFrames == 0 ) {
		rt_printf("Error: the UGen needs BELA analog enabled\n");
		return false;
	}

	unit->mAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;

	// initiate first sample
	AnalogOutput_next( unit, 1);  
	// set calculation method
	SETCALC(AnalogOutput_next);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void DigitalInput_next(DigitalInputUGen *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BeagleRTContext *context = world>mBelaContext;

  int pinid = unit->mDigitalPin;
  int digitalValue;
  float *out = ZOUT(0);
  
  // context->audioFrames should be equal to inNumSamples
//   for(unsigned int n = 0; n < context->audioFrames; n++) {
  for(unsigned int n = 0; n < inNumSamples; n++) {
// 	if(!(n % unit->mAudioFramesPerAnalogFrame)) {
// 	  analogValue = analogReadFrame(context, n/gAudioFramesPerAnalogFrame, iAnalogIn);
// 	}
	digitalValue=digitalReadFrame(context, n, pinid); //read the value of the button    
	*++out = (float) digitalValue;
  }
}

void DigitalInput_Ctor(DigitalInputUGen *unit)
{
	BeagleRTContext *context = unit->mWorld->mBelaContext;
  
	float fDigitalIn = ZIN0(0); // digital in pin -- cannot change after construction
	unit->mDigitalPin = (int) fDigitalIn;
	pinModeFrame(context, 0, unit->mDigitalPin, INPUT);
	
	// initiate first sample
	DigitalInput_next( unit, 1);  
	// set calculation method
	SETCALC(DigitalInput_next);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void DigitalOutput_next(DigitalOutputUGen *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BeagleRTContext *context = world>mBelaContext;

  int pinid = unit->mDigitalPin;
  float *in = IN(1);
  
  float newinput = 0;

  // context->audioFrames should be equal to inNumSamples
//   for(unsigned int n = 0; n < context->audioFrames; n++) {
  for(unsigned int n = 0; n < inNumSamples; n++) {
	// read input
	newinput = ++*in; // read next input sample
	if ( newinput > 0 ){ newinput = 1; }{ newinput = 0; }
	digitalWriteFrameOnce(context, n, pinid, (int) newinput);
  }
}

void DigitalOutput_Ctor(DigitalOutputUGen *unit)
{
	BeagleRTContext *context = unit->mWorld->mBelaContext;

	float fDigital = ZIN0(0); // digital in pin -- cannot change after construction
	unit->mDigitalPin = (int) fDigital;
	pinModeFrame(context, 0, unit->mDigitalPin, OUTPUT);

	// initiate first sample
	DigitalOutput_next( unit, 1);  
	// set calculation method
	SETCALC(DigitalOutput_next);
}

//////////////////////////////////////////////////////////////////////////////////////////////////


void DigitalIO_next(DigitalIOUGen *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BeagleRTContext *context = world>mBelaContext;

  float *pinid = IN(0);
  float *in = IN(1); // input value
  float *iomode = IN(2); // IO mode : < 0.5 = input, else output
  float *out = ZOUT(0); // output value = last output value
  
  int newpin;
  float newmode = 0; // input
  float newinput = 0;
  int newoutput = unit->mLastOutput;

  // context->audioFrames should be equal to inNumSamples
//   for(unsigned int n = 0; n < context->digitalFrames; n++) {
  for(unsigned int n = 0; n < inNumSamples; n++) {
	// read input
	newpin = (int) ++*pinid; // get pin id
	newinput = ++*in; // read next input sample
	newmode = ++*iomode; // get mode for this pin
	if ( newmode < 0.5 ){
	  pinModeFrameOnce( context, n, pinid, INPUT );
	  newoutput=digitalReadFrame(context, n, pinid);
	} else {	  
	  pinModeFrameOnce( context, n, pinid, OUTPUT );
	  digitalWriteFrameOnce(context, n, pinid, (int) newinput);
	}
	// always write to the output of the UGen
	*++out = (float) newoutput;
  }
  unit->mLastOutput = newoutput;
}

void DigitalIO_Ctor(DigitalIOUGen *unit)
{
	BeagleRTContext *context = unit->mWorld->mBelaContext;

	unit->mLastOutput = 0;

	// initiate first sample
	DigitalIO_next( unit, 1);  
	// set calculation method
	SETCALC(DigitalIO_next);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

// extern "C"
// {
// 
//   
// }

PluginLoad(BELA)
{
	ft = inTable;

	DefineSimpleUnit(AnalogInput);
	DefineSimpleUnit(AnalogOutput);
	DefineSimpleUnit(DigitalInput);
	DefineSimpleUnit(DigitalOutput);
	DefineSimpleUnit(DigitalIO);
// 	DefineUnit("AnalogInput", sizeof(AnalogInputUGen), (UnitCtorFunc)&AnalogInput_Ctor, 0, 0);
}


// C_LINKAGE SC_API_EXPORT void unload(InterfaceTable *inTable)
// {
// 
// }
