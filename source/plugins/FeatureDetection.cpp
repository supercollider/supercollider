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

//Feature (Onset) Detection implemented by sick lincoln for sc3 
//Jensen,K. & Andersen, T. H. (2003). Real-time Beat Estimation Using Feature Extraction. 
//In Proceedings of the Computer Music Modeling and RetrievalSymposium, Lecture Notes in Computer Science. Springer Verlag.
//Hainsworth, S. (2003) Techniques for the Automated Analysis of Musical Audio. PhD, university of cambridge engineering dept.

//possible to make a Goto style Detector for a given band and with history of two samples-
//should do separately as PV_GotoBandTrack 

#include "FFT_UGens.h"

struct PV_OnsetDetectionBase : public Unit
{
float * m_prevframe;
int m_numbins;
int m_waiting, m_waitSamp, m_waitLen;
};

//FFT onset detector combining 4 advised features from Jensen/Andersen 
struct PV_JensenAndersen : public PV_OnsetDetectionBase
{
float m_hfc,m_hfe,m_sc,m_sf;
int m_fourkindex;
};


//FFT onset detector combining 2 advised features from Hainsworth PhD
struct PV_HainsworthFoote : public PV_OnsetDetectionBase
{
float m_prevNorm;
int m_5kindex,m_30Hzindex;

};

//for time domain onset detection/RMS
struct RunningSum : public Unit {
	int msamp, mcount;     
	float msum;
	float mmeanmult;
	float* msquares;
};



extern "C"
{
  void PV_OnsetDetectionBase_Ctor(PV_OnsetDetectionBase *unit);
  void PV_OnsetDetectionBase_Dtor(PV_OnsetDetectionBase *unit);

  void PV_JensenAndersen_Ctor(PV_JensenAndersen *unit);
  void PV_JensenAndersen_Dtor(PV_JensenAndersen *unit);
  void PV_JensenAndersen_next(PV_JensenAndersen *unit, int inNumSamples);
  
  void PV_HainsworthFoote_Ctor(PV_HainsworthFoote *unit);
  void PV_HainsworthFoote_Dtor(PV_HainsworthFoote *unit);
  void PV_HainsworthFoote_next(PV_HainsworthFoote *unit, int inNumSamples);
  
  void RunningSum_next_k(RunningSum *unit, int inNumSamples);
  void RunningSum_Ctor(RunningSum* unit);
  void RunningSum_Dtor(RunningSum* unit);
  
}		      



void PV_OnsetDetectionBase_Ctor(PV_OnsetDetectionBase *unit)
{
        World *world = unit->mWorld;
    
        uint32 bufnum = (uint32)ZIN0(0);
		if (bufnum >= world->mNumSndBufs) bufnum = 0;
		SndBuf *buf = world->mSndBufs + bufnum; 
 
		unit->m_numbins = buf->samples - 2 >> 1;
        int insize = unit->m_numbins * sizeof(float);
    
        unit->m_prevframe = (float*)RTAlloc(unit->mWorld, insize);
   
        memset(unit->m_prevframe, 0, insize);
    
	    unit->m_waiting=0;
        unit->m_waitSamp=0;
        unit->m_waitLen=0;  
}

void PV_OnsetDetectionBase_Dtor(PV_OnsetDetectionBase *unit)
{
		RTFree(unit->mWorld, unit->m_prevframe);
}



void PV_JensenAndersen_Ctor(PV_JensenAndersen *unit)
{
		PV_OnsetDetectionBase_Ctor(unit);

        unit->m_hfc= 0.0;
        unit->m_hfe= 0.0;
        unit->m_sf= 0.0;
        unit->m_sc= 0.0;
 
		unit->m_fourkindex= (4000.0/(unit->mWorld->mSampleRate))*(unit->m_numbins);
 
		SETCALC(PV_JensenAndersen_next);
}

void PV_JensenAndersen_Dtor(PV_JensenAndersen *unit)
{
		PV_OnsetDetectionBase_Dtor(unit);
}


void PV_JensenAndersen_next(PV_JensenAndersen *unit, int inNumSamples)
{
		float outval=0.0; 
        float fbufnum = ZIN0(0); 

        if(unit->m_waiting==1) {
			unit->m_waitSamp+=inNumSamples;
			if(unit->m_waitSamp>=unit->m_waitLen) {unit->m_waiting=0;}  
		}
        
        if (!(fbufnum < 0.f)) 
        //if buffer ready to process
        { 
	 
			uint32 ibufnum = (uint32)fbufnum; 
			World *world = unit->mWorld; 
			if (ibufnum >= world->mNumSndBufs) ibufnum = 0; 
			SndBuf *buf = world->mSndBufs + ibufnum; 
			int numbins = buf->samples - 2 >> 1;

			SCPolarBuf *p = ToPolarApx(buf);
					
			//four spectral features useful for onset detection according to Jensen/Andersen      
				
			float magsum=0.0, magsumk=0.0, magsumkk=0.0, sfsum=0.0, hfesum=0.0;
			
			int i;
			float * q= unit->m_prevframe;
		
			int k4= unit->m_fourkindex; 
			
			for (i=0; i<numbins; ++i) {
			float mag= ((p->bin[i]).mag);
			int k= i+1;
			float qmag= q[i];
			magsum+=mag;
			magsumk+=k*mag;
			magsumkk+=k*k*mag;
			sfsum+= fabs(mag- (qmag));
			if(i>k4) hfesum+=mag;
		}
			
			//normalise
			float sc= (magsumk/magsum)/numbins;
			float hfe= hfesum/numbins;
			float hfc= magsumkk/(numbins*numbins*numbins);
			float sf= sfsum/numbins;
		
			//printf("sc %f hfe %f hfc %f sf %f \n",sc, hfe, hfc, sf);
		  
			//does this trigger? 
			//may need to take derivatives across previous frames by storing old values
			
			float sum = (ZIN0(1)*sc)+(ZIN0(2)*hfe)+(ZIN0(3)*hfc)+(ZIN0(4)*sf);
			
			//printf("sum %f thresh %f \n",sum, ZIN0(7));
		  
			//if over threshold, may also impose a 50mS wait here
			if(sum>ZIN0(5) && (unit->m_waiting==0)) {//printf("bang! \n"); 
			outval=1.0;
			unit->m_waiting=1;
			unit->m_waitSamp=inNumSamples;
			unit->m_waitLen=(ZIN0(6)*(world->mSampleRate));  
        } 
       
        //take copy of this frame's magnitudes as prevframe
    
        for (i=0; i<numbins; ++i)
        q[i]= p->bin[i].mag;
        }
 
      float *out = ZOUT(0);
                 
      for (int j=0; j<inNumSamples; ++j) {
		*++out = outval;
	}
       
        
}



void PV_HainsworthFoote_Ctor(PV_HainsworthFoote *unit)
{
		PV_OnsetDetectionBase_Ctor(unit);

		World *world = unit->mWorld;
    
        unit->m_5kindex= (5000.0/(world->mSampleRate))*(unit->m_numbins);
        unit->m_30Hzindex= (30.0/(world->mSampleRate))*(unit->m_numbins);
       
	    unit->m_prevNorm= 1.0;
 
		SETCALC(PV_HainsworthFoote_next);
}

void PV_HainsworthFoote_Dtor(PV_HainsworthFoote *unit)
{
		PV_OnsetDetectionBase_Dtor(unit);
}



static const float lmult= 1.442695040889; //loge(2) reciprocal
			
void PV_HainsworthFoote_next(PV_HainsworthFoote *unit, int inNumSamples)
{

		float outval=0.0; 
        float fbufnum = ZIN0(0); 
	
        if(unit->m_waiting==1)
        {
        unit->m_waitSamp+=inNumSamples;
        if(unit->m_waitSamp>=unit->m_waitLen) {unit->m_waiting=0;}  
        }
        
        if (!(fbufnum < 0.f)) 
        //if buffer ready to process
        { 
	 
		uint32 ibufnum = (uint32)fbufnum; 
		World *world = unit->mWorld; 
		if (ibufnum >= world->mNumSndBufs) ibufnum = 0; 
		SndBuf *buf = world->mSndBufs + ibufnum; 
		int numbins = buf->samples - 2 >> 1;

		SCPolarBuf *p = ToPolarApx(buf);
				
		float dnk,prevmag, mkl=0.0, footesum=0.0, norm=0.0;
    
        int i;
        float * q= unit->m_prevframe;
    
        int k5= unit->m_5kindex; 
        int h30= unit->m_30Hzindex; 
        
        for (i=0; i<numbins; ++i) {
        float mag= ((p->bin[i]).mag);
        float qmag= q[i];
		
		if(i>=h30 && i<k5) {
		prevmag= qmag;
		//avoid divide by zero
		if(prevmag<0.0001) prevmag=0.0001;

		//no log2 in maths library, so use log2(x)= log(x)/log(2) where log is to base e
		//could just use log and ignore scale factor but hey let's stay accurate to the source for now
		dnk= log(mag/prevmag)*lmult;
		
		if(dnk>0.0) mkl+=dnk;
		}
		
		norm+=mag*mag;
		footesum+=mag*qmag;
	}
        
        
        mkl= mkl/(k5-h30);
        //Foote measure- footediv will be zero initially
        float footediv= ((sqrt(norm))*(sqrt(unit->m_prevNorm)));
        if(footediv<0.0001) footediv=0.0001;
        float foote= 1.0- (footesum/footediv); //1.0 - similarity
        //printf("mkl %f foote %f \n",mkl, foote);
      
		unit->m_prevNorm= norm;
        float sum = (ZIN0(1)*mkl)+(ZIN0(2)*foote);
        
        //printf("sum %f thresh %f \n",sum, ZIN0(7));
      
        //if over threshold, may also impose a 50mS wait here
        if(sum>ZIN0(3) && (unit->m_waiting==0)) {
		
		outval=1.0;
        unit->m_waiting=1;
        unit->m_waitSamp=inNumSamples;
        unit->m_waitLen=(ZIN0(4)*(unit->mWorld->mSampleRate));  
        } 
       
        //take copy of this frame's magnitudes as prevframe
    
        for (i=0; i<numbins; ++i)
        q[i]= p->bin[i].mag;
        
        }
 
      float *out = ZOUT(0);
        
      for (int j=0; j<inNumSamples; ++j) {
		*++out = outval;
	}
       
        
}


void RunningSum_Ctor( RunningSum* unit ) {
					 
	SETCALC(RunningSum_next_k);

	unit->msamp= (int) ZIN0(1);

	unit->mmeanmult= 1.0f/(unit->msamp);
	unit->msum=0.0f;
	unit->mcount=0; //unit->msamp-1;
	unit->msquares= (float*)RTAlloc(unit->mWorld, unit->msamp * sizeof(float));

	//initialise to zeroes
	for(int i=0; i<unit->msamp; ++i)
	unit->msquares[i]=0.f;

}

void RunningSum_Dtor(RunningSum *unit) {
	RTFree(unit->mWorld, unit->msquares);
}

//RMS is easy because convolution kernel can be updated just by deleting oldest sample and adding newest-
//half hanning window convolution etc requires updating values for all samples in memory on each iteration 
void RunningSum_next_k( RunningSum *unit, int inNumSamples ) {

	float *in = ZIN(0);
	float *out = ZOUT(0);

	int count= unit->mcount;
	int samp= unit->msamp;

	float * data= unit->msquares;
	float sum= unit->msum;
	//float meanmult= unit->mmeanmult;

	LOOP(inNumSamples, 

	//update sum
	sum -=data[count];  
	//prone to floating point error accumulation over time? not if assume error is distributed equally either side of 0
	float next= ZXP(in);
	data[count]= next;
	sum += next;
	 
	count=(count+1)%samp;

	ZXP(out) = sum; //*meanmult; //could ditch the mean if don't need normalised
	);

	unit->mcount =count;
	unit->msum =  sum;      

}





void initFeatureDetectors(InterfaceTable *it)
{
	DefineDtorUnit(PV_JensenAndersen);
	
	DefineDtorUnit(PV_HainsworthFoote);
	
	DefineDtorUnit(RunningSum);
}