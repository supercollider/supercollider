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
 *  BELA I/O UGens created by nescivi, (c) 2016
 *  https://www.nescivi.eu
 */

// #include <SC_Lock.h>

#include <atomic>

#include "Bela.h"
// Xenomai-specific includes
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/intr.h>
#include <rtdk.h>

#include "SC_PlugIn.h"

static InterfaceTable *ft;


struct AnalogIn : public Unit
{
  int mAudioFramesPerAnalogFrame;
};

struct AnalogOut : public Unit
{
  int mAudioFramesPerAnalogFrame;
};

// static digital pin, static function (in)
struct DigitalIn : public Unit
{
  int mDigitalPin;
};

// static digital pin, static function (out)
struct DigitalOut : public Unit
{
  int mDigitalPin;
};

// flexible digital pin, flexible function (in or out)
struct DigitalIO : public Unit
{
  int mLastOut;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

void AnalogIn_next(AnalogIn *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  float *fin = IN(0); // analog in pin, can be modulated
  float *out = ZOUT(0);
  int analogPin = 0;
  float analogValue = 0;
  
  // context->audioFrames should be equal to inNumSamples
//   for(unsigned int n = 0; n < context->audioFrames; n++) {
  for(unsigned int n = 0; n < inNumSamples; n++) {
	analogPin = (int) fin[n];
// 	analogPin = sc_clip( analogPin, 0.0, context->analogInChannels );
	if ( (analogPin < 0) || (analogPin >= context->analogInChannels) ){
	    rt_printf( "analog pin must be between %i and %i, it is %i", 0, context->analogInChannels, analogPin );
	} else {
	  if(!(n % unit->mAudioFramesPerAnalogFrame)) {
	    analogValue = analogRead(context, n/unit->mAudioFramesPerAnalogFrame, analogPin);
	  }
	}
	*++out = analogValue;
  }
}

void AnalogIn_Ctor(AnalogIn *unit)
{
	BelaContext *context = unit->mWorld->mBelaContext;
  
	if(context->analogFrames == 0 || context->analogFrames > context->audioFrames) {
		rt_printf("Error: the UGen needs BELA analog enabled, with 4 or 8 channels\n");
		return;
	}

	unit->mAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;

	// initiate first sample
	AnalogIn_next( unit, 1);  
	// set calculation method
	SETCALC(AnalogIn_next);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void AnalogOut_next(AnalogOut *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  float *fin = IN(0); // analog in pin, can be modulated
  float *in = IN(1);
  
  int analogPin = 0;
  float newinput = 0;
  // context->audioFrames should be equal to inNumSamples
//   for(unsigned int n = 0; n < context->audioFrames; n++) {
  for(unsigned int n = 0; n < inNumSamples; n++) {
	// read input
	analogPin = (int) fin[n];
// 	analogPin = sc_clip( analogPin, 0.0, 7.0 );
	if ( (analogPin < 0) || (analogPin >= context->analogOutChannels) ){
	    rt_printf( "analog pin must be between %i and %i, it is %i", 0, context->analogOutChannels, analogPin );
	} else {
	  newinput = in[n]; // read next input sample
	  if(!(n % unit->mAudioFramesPerAnalogFrame)) {
	    analogWrite(context,  n/ unit->mAudioFramesPerAnalogFrame, analogPin, newinput);
	  }
	}
  }
}

void AnalogOut_Ctor(AnalogOut *unit)
{
	BelaContext *context = unit->mWorld->mBelaContext;
  
	if(context->analogFrames == 0 ) {
		rt_printf("Error: the UGen needs BELA analog enabled\n");
		return;
	}

	unit->mAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;

	// initiate first sample
	AnalogOut_next( unit, 1);  
	// set calculation method
	SETCALC(AnalogOut_next);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void DigitalIn_next(DigitalIn *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  int pinid = unit->mDigitalPin;
  int digitalValue;
  float *out = ZOUT(0);
  
  // context->audioFrames should be equal to inNumSamples
//   for(unsigned int n = 0; n < context->audioFrames; n++) {
  for(unsigned int n = 0; n < inNumSamples; n++) {
	digitalValue=digitalRead(context, n, pinid); //read the value of the button    
	*++out = (float) digitalValue;
  }
}

void DigitalIn_next_dummy(DigitalIn *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  
  for(unsigned int n = 0; n < inNumSamples; n++) {
	*++out = 0.0;
  }
}

void DigitalIn_Ctor(DigitalIn *unit)
{
	BelaContext *context = unit->mWorld->mBelaContext;
  
	float fDigitalIn = ZIN0(0); // digital in pin -- cannot change after construction
	unit->mDigitalPin = (int) fDigitalIn;
// 	unit->mDigitalPin = (int) sc_clip( fDigitalIn, 0., 15.0 );
	if ( (unit->mDigitalPin < 0) || (unit->mDigitalPin >= context->digitalChannels) ){
	    rt_printf( "digital pin must be between %i and %i, it is %i", 0, context->digitalChannels, unit->mDigitalPin );
	  // initiate first sample
	  DigitalIn_next_dummy( unit, 1);  
	  // set calculation method
	  SETCALC(DigitalIn_next_dummy);
	} else {
	  pinMode(context, 0, unit->mDigitalPin, INPUT);
	  // initiate first sample
	  DigitalIn_next( unit, 1);  
	  // set calculation method
	  SETCALC(DigitalIn_next);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void DigitalOut_next(DigitalOut *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  int pinid = unit->mDigitalPin;
  float *in = IN(1);
  
  float newinput = 0;
  int newinputInt = 0;

  // context->audioFrames should be equal to inNumSamples
//   for(unsigned int n = 0; n < context->audioFrames; n++) {
  for(unsigned int n = 0; n < inNumSamples; n++) {
	// read input
	newinput = in[n];
	if ( newinput > 0.5 ){ 
// 	  digitalWriteOnce(context, n, pinid, GPIO_HIGH );
	  digitalWrite(context, n, pinid, GPIO_HIGH );
	} else { 
// 	  digitalWriteOnce(context, n, pinid, GPIO_LOW );
	  digitalWrite(context, n, pinid, GPIO_LOW );
	}
  }
}

void DigitalOut_next_dummy(DigitalOut *unit, int inNumSamples)
{
}

void DigitalOut_Ctor(DigitalOut *unit)
{
	BelaContext *context = unit->mWorld->mBelaContext;

	float fDigital = ZIN0(0); // digital in pin -- cannot change after construction
	unit->mDigitalPin = (int) fDigital;
	if ( (unit->mDigitalPin < 0) || (unit->mDigitalPin >= context->digitalChannels) ){
	  rt_printf( "digital pin must be between %i and %i, it is %i", 0, context->digitalChannels, unit->mDigitalPin );
	  // initiate first sample
	  DigitalOut_next_dummy( unit, 1);  
	  // set calculation method	    
	  SETCALC(DigitalOut_next_dummy);
	} else {
	  pinMode(context, 0, unit->mDigitalPin, OUTPUT);
	  // initiate first sample
	  DigitalOut_next( unit, 1);  
	  // set calculation method
	  SETCALC(DigitalOut_next);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////


void DigitalIO_next(DigitalIO *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  float *pinid = IN(0);
  float *in = IN(1); // input value
  float *iomode = IN(2); // IO mode : < 0.5 = input, else output
  float *out = ZOUT(0); // output value = last output value
  
  int newpin;
  float newmode = 0; // input
  float newinput = 0;
  int newinputInt = 0;
  int newoutput = unit->mLastOut;

  // context->audioFrames should be equal to inNumSamples
//   for(unsigned int n = 0; n < context->digitalFrames; n++) {
  for(unsigned int n = 0; n < inNumSamples; n++) {
	// read input
	newpin = (int) pinid[n];
	if ( (newpin < 0) || (newpin >= context->digitalChannels) ){
	    rt_printf( "digital pin must be between %i and %i, it is %i", 0, context->digitalChannels, newpin );
	} else {
	  newinput = in[n];
	  newmode = iomode[n];
	  if ( newmode < 0.5 ){
  // 	    pinModeOnce( context, n, newpin, INPUT );
	    pinMode( context, n, newpin, INPUT );
	    newoutput = digitalRead(context, n, newpin);
	  } else {	  
  // 	    pinModeOnce( context, n, newpin, OUTPUT );
	    pinMode( context, n, newpin, OUTPUT );
	    if ( newinput > 0.5 ){ 
	      newinputInt = GPIO_HIGH; 
	    } else { 
	      newinputInt = GPIO_LOW;  
	    }
  // 	    digitalWriteOnce(context, n, newpin, newinputInt);
	    digitalWrite(context, n, newpin, newinputInt);
	  }
	}
	  // always write to the output of the UGen
	*++out = (float) newoutput;
  }
  unit->mLastOut = newoutput;
}

void DigitalIO_Ctor(DigitalIO *unit)
{
	BelaContext *context = unit->mWorld->mBelaContext;

	unit->mLastOut = 0;

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

// // the functions below are needed??
// 
// void render(BelaContext *belaContext, void *userData)
// {
// // 	SC_BelaDriver *driver = (SC_BelaDriver*)userData;
// // 	driver->BelaAudioCallback(belaContext);
// }
// // setup() is called once before the audio rendering starts.
// // Use it to perform any initialisation and allocation which is dependent
// // on the period size or sample rate.
// //
// // userData holds an opaque pointer to a data structure that was passed
// // in from the call to initAudio().
// //
// // Return true on success; returning false halts the program.
// bool setup(BelaContext* belaContext, void* userData)
// {
// 	if(userData == 0){
// 		printf("BelaPLUGINS: error, setup() got no user data\n");
// 		return false;
// 	}
// 
// 	return true;
// }
// 
// // cleanup() is called once at the end, after the audio has stopped.
// // Release any resources that were allocated in setup().
// void cleanup(BelaContext *belaContext, void *userData)
// {
// }


PluginLoad(BELA)
{
	ft = inTable;

	DefineSimpleUnit(AnalogIn);
	DefineSimpleUnit(AnalogOut);
	DefineSimpleUnit(DigitalIn);
	DefineSimpleUnit(DigitalOut);
	DefineSimpleUnit(DigitalIO);
}


// C_LINKAGE SC_API_EXPORT void unload(InterfaceTable *inTable)
// {
// 
// }
