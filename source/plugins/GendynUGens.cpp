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

//Gendy1 UGen implemented by Nick Collins (sicklincoln.org)

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

        extern "C" {  
  
            void Gendy1_next_k(Gendy1 *unit, int inNumSamples);
            void Gendy1_Ctor(Gendy1* unit);
            void Gendy1_Dtor(Gendy1* unit);
            void Gendy1_distribution( Gendy1 *unit, int which);
        }
    
        void Gendy1_Ctor( Gendy1* unit ) {
                         
			SETCALC(Gendy1_next_k);
	 
			unit->mFreqMul = unit->mRate->mSampleDur;
			unit->mPhase = 1.f;	//should immediately decide on new target 
			unit->mAmp = 0.0; 
			unit->mNextAmp = 0.0;
			unit->mSpeed = 100; 
			
			unit->mMemorySize= (int) ZIN0(8);	//default is 12
			//printf("memsize %d %f", unit->mMemorySize, ZIN0(8));
			if(unit->mMemorySize<1) unit->mMemorySize=1;
			unit->mIndex=0;
			unit->mMemoryAmp= (float*)RTAlloc(unit->mWorld, unit->mMemorySize * sizeof(float));
			unit->mMemoryDur= (float*)RTAlloc(unit->mWorld, unit->mMemorySize * sizeof(float));
			
			
			RGen& rgen = *unit->mParent->mRGen;
			
			//initialise to zeroes and separations
			int i=0;
			for(i=0; i<unit->mMemorySize;++i) {
				unit->mMemoryAmp[i]=2*rgen.frand() - 1.0;
				unit->mMemoryDur[i]=rgen.frand();
			}
			
        }
    
        void Gendy1_Dtor(Gendy1 *unit)
        {
			RTFree(unit->mWorld, unit->mMemoryAmp);
			RTFree(unit->mWorld, unit->mMemoryDur);
        }
    
    
        //called once per period so OK to work out constants in here
        float Gendy1_distribution( Gendy1 *unit, int which, float a) {
        
			RGen& rgen = *unit->mParent->mRGen;
			float f= rgen.frand(); //linear distribution 0.0 to 1.0
			float temp, c;
			
			if(a>1.0) a=1.0;       //a must be in range 0 to 1
			if(a<0.0001) a=0.0001; 	//for safety with some distributions, don't want divide by zero errors
			
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
					c= atan(10*a);		//PERHAPS CHANGE TO a=1/a;
					//incorrect- missed out divisor of pi in norm temp= a*tan(c*(2*pi*f - 1));	
					temp= (1/a)*tan(c*(2*f - 1));	//Cauchy distribution, C is precalculated
					
					//printf("cauchy f %f c %f temp %f out %f \n",f,  c, temp, temp/10);
					
					return temp*0.1; //(temp+100)/200;
					
				case 2: //LOGIST (ic)
					//X has -(log((1-z)/z)+b)/a which is not very usable as is
					
					c=0.5+(0.499*a); //calculate normalisation constant
					c= log((1-c)/c); 
				   
					//remap into range of valid inputs to avoid infinities in the log
					
				   //f= ((f-0.5)*0.499*a)+0.5;
					f= ((f-0.5)*0.998*a)+0.5; //[0,1]->[0.001,0.999]; squashed around midpoint 0.5 by a
					//Xenakis calls this the LOGIST map, it's from the range [0,1] to [inf,0] where 0.5->1
					//than take natural log. to avoid infinities in practise I take [0,1] -> [0.001,0.999]->[6.9,-6.9]
					//an interesting property is that 0.5-e is the reciprocal of 0.5+e under (1-f)/f 
					//and hence the logs are the negative of each other
					temp= log((1-f)/f)/c;	//n range [-1,1]
					//X also had two constants in his- I don't bother
					
					//printf("logist f %f temp %f\n", f, temp);
					
					return temp; //a*0.5*(temp+1.0);	//to [0,1]
					
				case 3: //HYPERBCOS
					//X original a*log(tan(z*pi/2)) which is [0,1]->[0,pi/2]->[0,inf]->[-inf,inf]
					//unmanageable in this pure form
					c=tan(1.5692255*a);    //tan(0.999*a*pi*0.5);    	//[0, 636.6] maximum range
					temp= tan(1.5692255*a*f)/c;	//[0,1]->[0,1] 
					temp= log(temp*0.999+0.001)*(-0.1447648);  // multiplier same as /(-6.9077553); //[0,1]->[0,1]
				   
					 //printf("hyperbcos f %f c %f temp %f\n", f, c, temp);
					
					return 2*temp-1.0;
					
				case 4: //ARCSINE
					//X original a/2*(1-sin((0.5-z)*pi)) aha almost a better behaved one though [0,1]->[2,0]->[a,0] 
					c= sin(1.5707963*a); //sin(pi*0.5*a);	//a as scaling factor of domain of sine input to use
					temp= sin(pi*(f-0.5)*a)/c; //[-1,1] which is what I need
					
					//printf("arcsine f %f c %f temp %f\n", f, c, temp);
					
					return temp;
					
					case 5: //EXPON
					//X original -(log(1-z))/a [0,1]-> [1,0]-> [0,-inf]->[0,inf]
					c= log(1.0-(0.999*a));
					temp= log(1.0-(f*0.999*a))/c;
					
					//printf("expon f %f c %f temp %f\n", f, c, temp);
					
					return 2*temp-1.0;
					
				case 6: //SINUS
					//X original a*sin(smp * 2*pi/44100 * b) ie depends on a second oscillator's value- 
					//hmmm, plug this in as a I guess, will automatically accept control rate inputs then!
					return 2*a-1.0;
					
				default:
					break;
				}
			
			return 2*f-1.0;
        }
        
    
        void Gendy1_next_k( Gendy1 *unit, int inNumSamples ) {
        
			float *out = ZOUT(0);
			
			//distribution choices for amp and dur and constants of distribution
			int whichamp= ZIN0(0);
			int whichdur= ZIN0(1);
			float aamp = ZIN0(2);
			float adur = ZIN0(3);
			float minfreq = ZIN0(4);
			float maxfreq = ZIN0(5);
			float scaleamp = ZIN0(6);
			float scaledur = ZIN0(7); 
			
			float rate= unit->mDur;
			
			//phase gives proportion for linear interpolation automatically
			double phase = unit->mPhase;
		
			float amp= unit->mAmp;
			float nextamp= unit->mNextAmp;

			float speed= unit->mSpeed;
		  
			//RGen& rgen = *unit->mParent->mRGen;

			LOOP(inNumSamples, 
			float z;
					
			if (phase >= 1.f) {
					phase -= 1.f;
	
					int index= unit->mIndex;
					int num= (int)(ZIN0(9));//(unit->mMemorySize);(((int)ZIN0(9))%(unit->mMemorySize))+1;
					if((num>(unit->mMemorySize)) || (num<1)) num=unit->mMemorySize;
					
					//new code for indexing
					index=(index+1)%num;
					amp=nextamp;
				   
					unit->mIndex=index;
					
				   //Gendy dist gives value [-1,1], then use scaleamp
				   //first term was amp before, now must check new memory slot
					nextamp= (unit->mMemoryAmp[index])+(scaleamp*Gendy1_distribution(unit, whichamp, aamp));
				
					//mirroring for bounds- safe version 
					if(nextamp>1.0 || nextamp<-1.0) {
					
					//printf("mirroring nextamp %f ", nextamp);
	
					//to force mirroring to be sensible
					if(nextamp<0.0) nextamp=nextamp+4.0;
					
					nextamp=fmod(nextamp,4.0); 
					//printf("fmod  %f ", nextamp);
	
					if(nextamp>1.0 && nextamp<3.0)
					nextamp= 2.0-nextamp;
					
					else if(nextamp>1.0)
					nextamp=nextamp-4.0;
					
					//printf("mirrorednextamp %f \n", nextamp);
					};
					
					unit->mMemoryAmp[index]= nextamp;
					
					//Gendy dist gives value [-1,1]
					rate= (unit->mMemoryDur[index])+(scaledur*Gendy1_distribution(unit, whichdur, adur));
				 
					if(rate>1.0 || rate<0.0)
					{
					if(rate<0.0) rate=rate+2.0;
					rate= fmod(rate,2.0);
					rate= 2.0-rate;
					}
				
					unit->mMemoryDur[index]= rate;
					
					//printf("nextamp %f rate %f \n", nextamp, rate);
	
					//define range of speeds (say between 20 and 1000 Hz)
					//can have bounds as fourth and fifth inputs
					speed=  (minfreq+((maxfreq-minfreq)*rate))*(unit->mFreqMul);
					
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

    
        extern "C" void load(InterfaceTable *inTable) {
                
						ft = inTable;

						DefineDtorUnit(Gendy1);
                    }
        
        
    