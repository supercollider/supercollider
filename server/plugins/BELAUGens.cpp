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

// static digital pin, static function (out) - uses DigitalWrite and a check whether value changed 
struct DigitalOut : public Unit
{
  int mDigitalPin;
  int mLastOut;
};

// static digital pin, static function (out) - uses DigitalWriteOnce
struct DigitalOutA : public Unit
{
  int mDigitalPin;
  int mLastOut;
};

// flexible digital pin, flexible function (in or out)
struct DigitalIO : public Unit
{
  int mLastDigitalIn;
  int mLastDigitalOut;
};

/*
struct BelaScope : public Unit
{
};

struct BelaScopeChannel : public Unit
{
    int mScopeChannel;
};
*/


//////////////////////////////////////////////////////////////////////////////////////////////////

void AnalogIn_next_aa(AnalogIn *unit, int inNumSamples)
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

void AnalogIn_next_ak(AnalogIn *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  int analogPin = (float) IN0(0);
  float *out = ZOUT(0);
  float analogValue = 0;

    if ( (analogPin < 0) || (analogPin >= context->analogInChannels) ){
        rt_printf( "analog pin must be between %i and %i, it is %i", 0, context->analogInChannels, analogPin );
        for(unsigned int n = 0; n < inNumSamples; n++) {
            *++out = 0;
        }
    } else {
        for(unsigned int n = 0; n < inNumSamples; n++) {
            if(!(n % unit->mAudioFramesPerAnalogFrame)) {
                analogValue = analogRead(context, n/unit->mAudioFramesPerAnalogFrame, analogPin);
            }
            *++out = analogValue;
        }
    }
}


void AnalogIn_next_kk(AnalogIn *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  int analogPin = (float) IN0(0);

  if ( (analogPin < 0) || (analogPin >= context->analogInChannels) ){
    rt_printf( "analog pin must be between %i and %i, it is %i", 0, context->analogInChannels, analogPin );
    ZOUT0(0) = 0.0;  
  } else {
    ZOUT0(0) = analogRead(context, 0, analogPin);
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
	AnalogIn_next_kk( unit, 1);  
	// set calculation method
        if (unit->mCalcRate == calc_FullRate) {
            if (INRATE(0) == calc_FullRate) {
                SETCALC(AnalogIn_next_aa);
            } else {
                SETCALC(AnalogIn_next_ak);
            }
        } else {
            if (INRATE(0) == calc_FullRate) {
                rt_printf("AnalogIn warning: output rate is control rate, so cannot change analog pin at audio rate\n");
            }
            SETCALC(AnalogIn_next_kk);
        }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void AnalogOut_next_aaa(AnalogOut *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  float *fin = IN(0); // analog in pin, can be modulated
  float *in = IN(1);
  
  int analogPin = 0;
  float newinput = 0;
  for(unsigned int n = 0; n < inNumSamples; n++) {
	// read input
	analogPin = (int) fin[n];
	if ( (analogPin < 0) || (analogPin >= context->analogOutChannels) ){
	    rt_printf( "analog pin must be between %i and %i, it is %i", 0, context->analogOutChannels, analogPin );
	} else {
	  newinput = in[n]; // read next input sample
	  if(!(n % unit->mAudioFramesPerAnalogFrame)) {
	    analogWriteOnce(context,  n/ unit->mAudioFramesPerAnalogFrame, analogPin, newinput);
	  }
	}
  }
}

void AnalogOut_next_aka(AnalogOut *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  int analogPin = (int) IN0(0); // analog in pin, can be modulated
  float *in = IN(1);
  
  float newinput = 0;
  if ( (analogPin < 0) || (analogPin >= context->analogOutChannels) ){
    rt_printf( "analog pin must be between %i and %i, it is %i", 0, context->analogOutChannels, analogPin );
  } else {
    for(unsigned int n = 0; n < inNumSamples; n++) {
        newinput = in[n]; // read next input sample
	if(!(n % unit->mAudioFramesPerAnalogFrame)) {
	    analogWriteOnce(context,  n/ unit->mAudioFramesPerAnalogFrame, analogPin, newinput);
	}
    }
  }
}

void AnalogOut_next_aak(AnalogOut *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  float *fin = IN(0); // analog in pin, can be modulated
  float in = IN0(1);
  
  int analogPin = 0;
  for(unsigned int n = 0; n < inNumSamples; n++) {
	// read input
	analogPin = (int) fin[n];
	if ( (analogPin < 0) || (analogPin >= context->analogOutChannels) ){
	    rt_printf( "analog pin must be between %i and %i, it is %i", 0, context->analogOutChannels, analogPin );
	} else {
// 	  newinput = in[n]; // read next input sample
	  if(!(n % unit->mAudioFramesPerAnalogFrame)) {
	    analogWriteOnce(context,  n/ unit->mAudioFramesPerAnalogFrame, analogPin, in);
	  }
	}
  }
}


void AnalogOut_next_kk(AnalogOut *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  int analogPin = (int) IN0(0); // analog in pin, can be modulated
  float in = IN0(1);
  
  if ( (analogPin < 0) || (analogPin >= context->analogOutChannels) ){
    rt_printf( "analog pin must be between %i and %i, it is %i", 0, context->analogOutChannels, analogPin );
  } else {
    analogWrite(context, 0, analogPin, in);
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
	AnalogOut_next_kk( unit, 1);  

        if (unit->mCalcRate == calc_FullRate) { // ugen running at audio rate;
            if (INRATE(0) == calc_FullRate) { // pin changed at audio rate                
                if (INRATE(1) == calc_FullRate) { // output changed at audio rate
                    SETCALC(AnalogOut_next_aaa);
                } else {
                    SETCALC(AnalogOut_next_aak);
                }
            } else { // pin changed at control rate
                if (INRATE(1) == calc_FullRate) { // output changed at audio rate
                    SETCALC(AnalogOut_next_aka);
                } else { // 
                    SETCALC(AnalogOut_next_kk);
                }
            }
        } else { // ugen at control rate
            if ( (INRATE(0) == calc_FullRate) || (INRATE(1) == calc_FullRate) ) {
                rt_printf("AnalogOut warning: output rate is control rate, so cannot change inputs at audio rate\n");
            }
            SETCALC(AnalogOut_next_kk);
        }

}

//////////////////////////////////////////////////////////////////////////////////////////////////

void DigitalIn_next_a(DigitalIn *unit, int inNumSamples)
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

void DigitalIn_next_k(DigitalIn *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  int pinid = unit->mDigitalPin;
  int digitalValue = digitalRead(context, 0, pinid); //read the value of the button    
  ZOUT0(0) = (float) digitalValue;  
}

void DigitalIn_next_dummy_a(DigitalIn *unit, int inNumSamples)
{
  float *out = ZOUT(0);
  
  for(unsigned int n = 0; n < inNumSamples; n++) {
	*++out = 0.0;
  }
}

void DigitalIn_next_dummy_k(DigitalIn *unit, int inNumSamples)
{
    ZOUT0(0) = 0.0;  
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
          if (unit->mCalcRate == calc_FullRate) { // ugen running at audio rate;
            DigitalIn_next_dummy_a( unit, 1);  
          } else {
            DigitalIn_next_dummy_k( unit, 1);  
          }
	} else {
	  pinMode(context, 0, unit->mDigitalPin, INPUT);
	  // initiate first sample
	  DigitalIn_next_k( unit, 1);  
	  // set calculation method
          if (unit->mCalcRate == calc_FullRate) { // ugen running at audio rate;
                SETCALC(DigitalIn_next_a);
          } else {
                SETCALC(DigitalIn_next_k);
          }
	}
	

}

//////////////////////////////////////////////////////////////////////////////////////////////////

void DigitalOut_next_a_once(DigitalOut *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  int pinid = unit->mDigitalPin;
  float *in = IN(1);
  
  float newinput = 0;
  int lastOut = unit->mLastOut;

  for(unsigned int n = 0; n < inNumSamples; n++) {
	// read input
	newinput = in[n];
	if ( newinput > 0.5 ){ 
	  digitalWriteOnce(context, n, pinid, GPIO_HIGH );
	} else if ( lastOut == 1 ) {
	  digitalWrite(context, n, pinid, GPIO_LOW );
	}
  }
  unit->mLastOut = lastOut;
}

void DigitalOut_next_a(DigitalOut *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  int pinid = unit->mDigitalPin;
  float *in = IN(1);
  
  float newinput = 0;
  int lastOut = unit->mLastOut;

  for(unsigned int n = 0; n < inNumSamples; n++) {
	// read input
	newinput = in[n];
	if ( newinput > 0.5 ){ 
            if (lastOut == 0) {
                lastOut = 1;
                digitalWrite(context, n, pinid, GPIO_HIGH );
            }
	} else if ( lastOut == 1 ) {
          lastOut = 0;
	  digitalWrite(context, n, pinid, GPIO_LOW );
	}
  }
  unit->mLastOut = lastOut;
}

void DigitalOut_next_k(DigitalOut *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  int pinid = unit->mDigitalPin;
  float in = IN0(1);
  
  int lastOut = unit->mLastOut;
  if ( in > 0.5 ){ 
    if (lastOut == 0) {
        lastOut = 1;
        digitalWrite(context, 0, pinid, GPIO_HIGH );
    } else if ( lastOut == 1 ) {
        lastOut = 0;
        digitalWrite(context, 0, pinid, GPIO_LOW );
    }
  }
  unit->mLastOut = lastOut;
}

void DigitalOut_next_dummy(DigitalOut *unit, int inNumSamples)
{
}

void DigitalOut_Ctor(DigitalOut *unit)
{
	BelaContext *context = unit->mWorld->mBelaContext;

	float fDigital = ZIN0(0); // digital in pin -- cannot change after construction
        int writeMode = (int) ZIN0(2); // method of writing; 1 = writeOnce; 0 = write on change
	unit->mDigitalPin = (int) fDigital;
	unit->mLastOut = 0;

        if ( (unit->mDigitalPin < 0) || (unit->mDigitalPin >= context->digitalChannels) ){
	  rt_printf( "digital pin must be between %i and %i, it is %i", 0, context->digitalChannels, unit->mDigitalPin );
	  // initiate first sample
	  DigitalOut_next_dummy( unit, 1);  
	  // set calculation method	    
	  SETCALC(DigitalOut_next_dummy);
	} else {
	  pinMode(context, 0, unit->mDigitalPin, OUTPUT);
	  // initiate first sample
	  DigitalOut_next_k( unit, 1);

          if (unit->mCalcRate == calc_FullRate) { // ugen running at audio rate;
            if (INRATE(1) == calc_FullRate) { // output changed at audio rate
                if ( writeMode ){
                    SETCALC(DigitalOut_next_a_once);
                } else {
                    SETCALC(DigitalOut_next_a);
                }
            } else { // not much reason to actually do audiorate output
                SETCALC(DigitalOut_next_k);
            }
        } else { // ugen at control rate
            if ( INRATE(1) == calc_FullRate ) {
                rt_printf("DigitalOut warning: UGen rate is control rate, so cannot change inputs at audio rate\n");
            }
            SETCALC(DigitalOut_next_k);
            }
        }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void DigitalIO_next_aaaa_once(DigitalIO *unit, int inNumSamples)
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
  
  int newDigInInt = unit->mLastDigitalIn;
  float newDigIn = (float) newDigInInt;
  int newDigOut = unit->mLastDigitalOut;

  for(unsigned int n = 0; n < inNumSamples; n++) {
	// read input
	newpin = (int) pinid[n];
	if ( (newpin < 0) || (newpin >= context->digitalChannels) ){
	    rt_printf( "digital pin must be between %i and %i, it is %i", 0, context->digitalChannels, newpin );
	} else {
          newDigOut = (int) in[n];
	  newmode = iomode[n];
	  if ( newmode < 0.5 ){
            pinModeOnce( context, n, newpin, INPUT );
	    newDigInInt = digitalRead(context, n, newpin);
	  } else {	  
	    pinModeOnce( context, n, newpin, OUTPUT );
	    digitalWriteOnce(context, n, newpin, newDigOut);
	  }
	}
        // always write to the output of the UGen
	*++out = (float) newDigInInt;
  }
  unit->mLastDigitalIn = newDigInInt;
  unit->mLastDigitalOut = newDigOut;
}


void DigitalIO_next_aaak_once(DigitalIO *unit, int inNumSamples)
{
    // pinMode at control rate
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  float *pinid = IN(0);
  float *in = IN(1); // input value
  float iomode = IN0(2); // IO mode : < 0.5 = input, else output
  float *out = ZOUT(0); // output value = last output value
  
  int newDigInInt = unit->mLastDigitalIn;
  float newDigIn = (float) newDigInInt;
  
  int newDigOut = unit->mLastDigitalOut;
//   float newinput;

  int newpin;
  if ( iomode < 0.5 ){
    for(unsigned int n = 0; n < inNumSamples; n++) {
	newpin = (int) pinid[n];
	if ( (newpin < 0) || (newpin >= context->digitalChannels) ){
	    rt_printf( "digital pin must be between %i and %i, it is %i", 0, context->digitalChannels, newpin );
	} else {
          pinModeOnce( context, n, newpin, INPUT );
          newDigInInt = digitalRead(context, n, newpin);
        }
        // always write to the output of the UGen
	*++out = (float) newDigInInt;
        }
  } else {
    for(unsigned int n = 0; n < inNumSamples; n++) {
	newpin = (int) pinid[n];
	if ( (newpin < 0) || (newpin >= context->digitalChannels) ){
	    rt_printf( "digital pin must be between %i and %i, it is %i", 0, context->digitalChannels, newpin );
	} else {
          pinModeOnce( context, n, newpin, OUTPUT );
	  newDigOut = (int) in[n];
	  digitalWriteOnce(context, n, newpin, newDigOut);
        }
        *++out = (float) newDigInInt;
    }
  }  
  unit->mLastDigitalIn = newDigInInt;
  unit->mLastDigitalOut = newDigOut;
}

// output changing at control rate, rest audio
void DigitalIO_next_aaka_once(DigitalIO *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  float *pinid = IN(0);
  float in = IN0(1); // input value
  float *iomode = IN(2); // IO mode : < 0.5 = input, else output
  float *out = ZOUT(0); // output value = last output value
  
  int newpin;
  float newmode = 0; // input
  
  int newDigInInt = unit->mLastDigitalIn;
  float newDigIn = (float) newDigInInt;
//   int newDigOut = unit->mLastDigitalOut;
  int newDigOut = (int) in;

  for(unsigned int n = 0; n < inNumSamples; n++) {
	// read input
	newpin = (int) pinid[n];
	if ( (newpin < 0) || (newpin >= context->digitalChannels) ){
	    rt_printf( "digital pin must be between %i and %i, it is %i", 0, context->digitalChannels, newpin );
	} else {
	  newmode = iomode[n];
	  if ( newmode < 0.5 ){
            pinModeOnce( context, n, newpin, INPUT );
	    newDigInInt = digitalRead(context, n, newpin);
	  } else {
	    pinModeOnce( context, n, newpin, OUTPUT );
	    digitalWriteOnce(context, n, newpin, newDigOut);
	  }
	}
        // always write to the output of the UGen
	*++out = (float) newDigInInt;
  }
  unit->mLastDigitalIn = newDigInInt;
  unit->mLastDigitalOut = newDigOut;
}


// output changing at control rate, and pin mode at control rate
void DigitalIO_next_aakk_once(DigitalIO *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  float *pinid = IN(0);
  float in = IN0(1); // input value
  float iomode = IN0(2); // IO mode : < 0.5 = input, else output
  float *out = ZOUT(0); // output value = last output value
  
  int newpin;
  float newmode = 0; // input
  
  int newDigInInt = unit->mLastDigitalIn;
  float newDigIn = (float) newDigInInt;
//   int newDigOut = unit->mLastDigitalOut;
  int newDigOut = (int) in;

  int newpin;
  if ( iomode < 0.5 ){
    for(unsigned int n = 0; n < inNumSamples; n++) {
	newpin = (int) pinid[n];
	if ( (newpin < 0) || (newpin >= context->digitalChannels) ){
	    rt_printf( "digital pin must be between %i and %i, it is %i", 0, context->digitalChannels, newpin );
	} else {
          pinModeOnce( context, n, newpin, INPUT );
          newDigInInt = digitalRead(context, n, newpin);
        }
        // always write to the output of the UGen
	*++out = (float) newDigInInt;
        }
  } else {
    for(unsigned int n = 0; n < inNumSamples; n++) {
	newpin = (int) pinid[n];
	if ( (newpin < 0) || (newpin >= context->digitalChannels) ){
	    rt_printf( "digital pin must be between %i and %i, it is %i", 0, context->digitalChannels, newpin );
	} else {
          pinModeOnce( context, n, newpin, OUTPUT );
	  digitalWriteOnce(context, n, newpin, newDigOut);
        }
        *++out = (float) newDigInInt;
    }
  }  
  unit->mLastDigitalIn = newDigInInt;
  unit->mLastDigitalOut = newDigOut;
    
}


// pin changing at control rate, output control rate, rest audio rate
void DigitalIO_next_akaa_once(DigitalIO *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  float pinid = IN0(0);
  float in = IN0(1); // input value
  float *iomode = IN(2); // IO mode : < 0.5 = input, else output
  float *out = ZOUT(0); // output value = last output value
  
  int newpin = (int) pinid;
  float newmode = 0; // input
//   float newinput;
  
  int newDigInInt = unit->mLastDigitalIn;
  float newDigIn = (float) newDigInInt;
  
  int newDigOut = (int) in;

  if ( (newpin < 0) || (newpin >= context->digitalChannels) ){
	    rt_printf( "digital pin must be between %i and %i, it is %i", 0, context->digitalChannels, newpin );
  } else {
    for(unsigned int n = 0; n < inNumSamples; n++) {
// 	  newinput = in[n];
	  newmode = iomode[n];
	  if ( newmode < 0.5 ){
            pinModeOnce( context, n, newpin, INPUT );
	    newDigInInt = digitalRead(context, n, newpin);
	  } else {	  
	    pinModeOnce( context, n, newpin, OUTPUT );
	    digitalWriteOnce(context, n, newpin, newDigOut);
	  }
        // always write to the output of the UGen
	*++out = (float) newDigInInt;
    }
  }
  unit->mLastDigitalIn = newDigInInt;
  unit->mLastDigitalOut = newDigOut;
}

// pin changing at control rate, output at control rate, mode at audio rate
void DigitalIO_next_akka_once(DigitalIO *unit, int inNumSamples)
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
  
  int newDigInInt = unit->mLastDigitalIn;
  float newDigIn = (float) newDigInInt;
  int newDigOut = unit->mLastDigitalOut;

  for(unsigned int n = 0; n < inNumSamples; n++) {
	// read input
	newpin = (int) pinid[n];
	if ( (newpin < 0) || (newpin >= context->digitalChannels) ){
	    rt_printf( "digital pin must be between %i and %i, it is %i", 0, context->digitalChannels, newpin );
	} else {
	  newinput = in[n];
	  newmode = iomode[n];
	  if ( newmode < 0.5 ){
            pinModeOnce( context, n, newpin, INPUT );
	    newDigInInt = digitalRead(context, n, newpin);
	  } else {	  
	    pinModeOnce( context, n, newpin, OUTPUT );
	    if ( newinput > 0.5 ){ 
	      newDigOut = GPIO_HIGH; 
	    } else { 
	      newDigOut = GPIO_LOW;  
	    }
	    digitalWriteOnce(context, n, newpin, newDigOut);
	  }
	}
        // always write to the output of the UGen
	*++out = (float) newDigInInt;
  }
  unit->mLastDigitalIn = newDigInInt;
  unit->mLastDigitalOut = newDigOut;
}


// all inputs at control rate, output at audio rate
void DigitalIO_next_ak(DigitalIO *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  float pinid = IN0(0);
  float in = IN0(1); // input value
  float iomode = IN0(2); // IO mode : < 0.5 = input, else output
  float *out = ZOUT(0); // output value = last output value
  
  int newpin = (int) pinid;
  
  int newDigInInt = unit->mLastDigitalIn;
  float newDigIn = (float) newDigInInt;
  int newDigOut = (int) in;

  if ( (pinid < 0) || (pinid >= context->digitalChannels) ){
    rt_printf( "digital pin must be between %i and %i, it is %i", 0, context->digitalChannels, newpin );
  } else {
    if ( iomode < 0.5 ){
        pinMode( context, n, newpin, INPUT );
        for(unsigned int n = 0; n < inNumSamples; n++) {
            // read input
            newDigInInt = digitalRead(context, n, newpin);
            // always write to the output of the UGen
            *++out = (float) newDigInInt;
        }
    } else {  
        pinMode( context, n, newpin, OUTPUT );
        for(unsigned int n = 0; n < inNumSamples; n++) {
            if ( in > 0.5 ){ 
                newDigOut = GPIO_HIGH; 
            } else { 
                newDigOut = GPIO_LOW;  
            }
            digitalWriteOnce(context, n, newpin, newDigOut);
            // always write to the output of the UGen
            *++out = (float) newDigInInt;
        }
    }
  }
  unit->mLastDigitalIn = newDigInInt;
  unit->mLastDigitalOut = newDigOut;
}

// all at control rate, output at control rate
void DigitalIO_next_kk(DigitalIO *unit, int inNumSamples)
{
  World *world = unit->mWorld;
  int bufLength = world->mBufLength;
  BelaContext *context = world->mBelaContext;

  int pinid = (int) IN0(0);
  float in = IN0(1); // input value
  float iomode = IN0(2); // IO mode : < 0.5 = input, else output
//   float *out = ZOUT(0); // output value = last output value
  
  int newDigInInt = unit->mLastDigitalIn;
  int newDigOut = unit->mLastDigitalOut;

  if ( (pinid < 0) || (pinid >= context->digitalChannels) ){
    rt_printf( "digital pin must be between %i and %i, it is %i", 0, context->digitalChannels, newpin );
  } else {
    if ( iomode < 0.5 ){
        pinMode( context, n, newpin, INPUT );
	newDigInInt = digitalRead(context, n, newpin);
    } else {  
        pinMode( context, n, newpin, OUTPUT );
        if ( in > 0.5 ){ 
            newDigOut = GPIO_HIGH; 
        } else { 
	    newDigOut = GPIO_LOW;  
        }
	digitalWrite(context, n, newpin, newDigOut);
    }
  }
  ZOUT0(0) = (float) newDigInInt;

  unit->mLastDigitalIn = newDigInInt;
  unit->mLastDigitalOut = newDigOut;
  
}

/*
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
  int newoutput = unit->mLastIn;

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
  unit->mLastDigitalIn = newoutput;
  unit->mLastDigitalOut = newinput;
}
*/

void DigitalIO_Ctor(DigitalIO *unit)
{
	BelaContext *context = unit->mWorld->mBelaContext;

        unit->mLastDigitalIn = 0;
        unit->mLastDigitalOut = 0;

//         int writeMode = (int) ZIN0(3); // method of writing; 1 = writeOnce; 0 = write on change
        
	// initiate first sample
	DigitalIO_next_kk( unit, 1);  
	// set calculation method
// 	SETCALC(DigitalIO_next);
        if (unit->mCalcRate == calc_FullRate) { // ugen running at audio rate;
            if (INRATE(0) == calc_FullRate) { // pin changed at audio rate
                if (INRATE(1) == calc_FullRate) { // output changed at audio rate
                    if (INRATE(2) == calc_FullRate) { // pinmode changed at audio rate
//                         if ( writeMode ){
                            SETCALC(DigitalIO_next_aaaa_once);
//                         } else {
//                             SETCALC(DigitalIO_next_aaaa);
//                         }
                    } else {
//                         if ( writeMode ){
                            SETCALC(DigitalIO_next_aaak_once);
//                         } else {
//                             SETCALC(DigitalIO_next_aaak);
//                         }
                    }
                } else { // output changed at control rate
                    if (INRATE(2) == calc_FullRate) { // pinmode changed at audio rate
//                         if ( writeMode ){
                            SETCALC(DigitalIO_next_aaka_once);
//                         } else {
//                             SETCALC(DigitalIO_next_aaka);
//                         }
                    } else {
//                         if ( writeMode ){
                            SETCALC(DigitalIO_next_aakk_once);
//                         } else {
//                             SETCALC(DigitalIO_next_aakk);
//                         }
                    }
                }
            } else { // pin changed at control rate
                if (INRATE(1) == calc_FullRate) { // output changed at audio rate
                    if (INRATE(2) == calc_FullRate) { // pinmode changed at audio rate
//                         if ( writeMode ){
                            SETCALC(DigitalIO_next_akaa_once);
//                         } else {
//                             SETCALC(DigitalIO_next_akaa);
//                         }
                    } else {
//                         if ( writeMode ){
                            SETCALC(DigitalIO_next_akak_once);
//                         } else {
//                             SETCALC(DigitalIO_next_akak);
//                         }
                    }
                } else {  // output changed at control rate
                    if (INRATE(2) == calc_FullRate) { // pinmode changed at audio rate
//                         if ( writeMode ){
                            SETCALC(DigitalIO_next_akka_once);
//                         } else {
//                             SETCALC(DigitalIO_next_akka);
//                         }
                    } else { // pinmode at control rate
                        SETCALC(DigitalIO_next_ak);
                    }
                }
            }
        } else { // ugen at control rate
            if ( (INRATE(1) == calc_FullRate) || (INRATE(2) == calc_FullRate) || (INRATE(3) == calc_FullRate) ) {
                rt_printf("DigitalIO warning: UGen rate is control rate, so cannot change inputs at audio rate\n");
            }
            SETCALC(DigitalIO_next_kk);
        }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

/*
int noScopeChannels = 0;
Scope * belaScope;


void BelaScopeChannel_next( BelaScope *unit )
{
    int scopeChannel = unit->mScopeChannel;
    float *in = IN(1);

    for(unsigned int n = 0; n < inNumSamples; n++) {
        belaScope->logChannel( scopeChannel, in[n] );
    }
}
    

void BelaScopeChannel_Ctor(BelaScope *unit)
{
    BelaContext *context = unit->mWorld->mBelaContext;
    
//     belaScope = Scope();
    // which channel is an input variable
//     belaScope->setup(3, context->audioSampleRate);
    float fChan = ZIN0(0); // number of channels
    mScopeChannel = (int ) fChan;
    // check whether channel is within number of channels of scope
    if ( mScopeChannel > noScopeChannels ){
            // error
    }
    // initiate first sample
    
    BelaScopeChannel_next( unit, 1);  
    // set calculation method
    SETCALC(BelaScopeChannel_next);
}



void BelaScope_next(BelaScope *unit)
{
}

void BelaScope_Ctor(BelaScope *unit)
{
    BelaContext *context = unit->mWorld->mBelaContext;
    
    float fChan = ZIN0(0); // number of channels
    noScopeChannels = (int) fChan;

    belaScope = Scope();
    // number of channels is a variable
    belaScope->setup(noScopeChannels, context->audioSampleRate);

    // initiate first sample
    BelaScope_next( unit, 1);  
    // set calculation method
    SETCALC(BelaScope_next);
}

void BelaScope_Dtor(BelaScope *unit)
{
    belaScope->stop();
    delete belaScope;
    noScopeChannels = 0;
}
*/

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
