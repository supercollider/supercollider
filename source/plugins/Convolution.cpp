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

//Convolution by sick lincoln for sc3 
//see ch18 http://www.dspguide.com/ch18.htm Steven W Smith

#include "SC_PlugIn.h"
#include <sndfile.h>
#include "SCComplex.h"

// not ready for altivec yet..
#undef __VEC__
#define __VEC__ 0

#if __VEC__
	#include <vecLib/vecLib.h>
	FFTSetup fftsetup[32];
#else
extern "C" {
	#include "fftlib.h"
	static float *cosTable[32];
}
#endif

#include <string.h>

static InterfaceTable *ft;

static float *fftWindow[32];

struct Convolution : Unit
{
	int m_pos, m_insize, m_fftsize,m_mask;
	int m_log2n;
        
        float *m_inbuf1,*m_inbuf2, *m_fftbuf1, *m_fftbuf2, *m_outbuf,*m_overlapbuf;
       
};


struct SCComplexBuf 
{
	float dc, nyq;
	SCComplex bin[1];
};

struct SCPolarBuf 
{
	float dc, nyq;
	SCPolar bin[1];
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

        void Convolution_next(Convolution *unit, int wrongNumSamples);
        void Convolution_Ctor(Convolution *unit);
        void Convolution_Dtor(Convolution *unit);
   
}

//////////////////////////////////////////////////////////////////////////////////////////////////


SCPolarBuf* ToPolarApx(SndBuf *buf);
SCPolarBuf* ToPolarApx(SndBuf *buf)
{
	if (buf->coord == coord_Complex) {
		SCComplexBuf* p = (SCComplexBuf*)buf->data;
		int numbins = buf->samples - 2 >> 1;
		for (int i=0; i<numbins; ++i) {
			p->bin[i].ToPolarApxInPlace();
		}
		buf->coord = coord_Polar;
	}
	return (SCPolarBuf*)buf->data;
}

SCComplexBuf* ToComplexApx(SndBuf *buf);
SCComplexBuf* ToComplexApx(SndBuf *buf)
{
	if (buf->coord == coord_Polar) {
		SCPolarBuf* p = (SCPolarBuf*)buf->data;
		int numbins = buf->samples - 2 >> 1;
		for (int i=0; i<numbins; ++i) {
			p->bin[i].ToComplexApxInPlace();
		}
		buf->coord = coord_Complex;
	}
	return (SCComplexBuf*)buf->data;
}


void DoWindowing(int log2n, float * fftbuf, int bufsize);
void DoWindowing(int log2n, float * fftbuf, int bufsize)
{
	float *win = fftWindow[log2n];
	if (!win) return;
	float *in = fftbuf - 1;
	win--;
        
	for (int i=0; i<bufsize; ++i) {
		*++in *= *++win;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

//PROPER CONVOLVER
//two possibilities- fixed kernel (in which case can derive the kernel spectrum in the constructor)
//and changing kernel (same size as target)

void Convolution_Ctor(Convolution *unit)
{
	        
        //require size N+M-1 to be a power of two
        //transform samp= 
        
        unit->m_insize=(int)ZIN0(2);	//could be input parameter
        
        printf("hello %i /n", unit->m_insize);
        unit->m_fftsize=2*(unit->m_insize);
        //just use memory for the input buffers and fft buffers
        int insize = unit->m_insize * sizeof(float);
        int fftsize = unit->m_fftsize * sizeof(float);
        
	unit->m_inbuf1 = (float*)RTAlloc(unit->mWorld, insize);
        unit->m_inbuf2 = (float*)RTAlloc(unit->mWorld, insize);
        
        unit->m_fftbuf1 = (float*)RTAlloc(unit->mWorld, fftsize);
        unit->m_fftbuf2 = (float*)RTAlloc(unit->mWorld, fftsize);
       
        unit->m_outbuf = (float*)RTAlloc(unit->mWorld, fftsize);
        unit->m_overlapbuf = (float*)RTAlloc(unit->mWorld, insize);
       
        memset(unit->m_outbuf, 0, fftsize);
        memset(unit->m_overlapbuf, 0, insize);
        
	unit->m_log2n = LOG2CEIL(unit->m_fftsize);
        
        //test for full input buffer
	unit->m_mask = unit->m_insize;
	unit->m_pos = 0;
	
	SETCALC(Convolution_next);
        
}
        
        
void Convolution_Dtor(Convolution *unit)
{
	RTFree(unit->mWorld, unit->m_inbuf1);
        RTFree(unit->mWorld, unit->m_inbuf2);
        RTFree(unit->mWorld, unit->m_fftbuf1);
        RTFree(unit->mWorld, unit->m_fftbuf2);
        RTFree(unit->mWorld, unit->m_outbuf);
        RTFree(unit->mWorld, unit->m_overlapbuf);
}


void Convolution_next(Convolution *unit, int wrongNumSamples)
{
	
	float *in1 = IN(0);
	float *in2 = IN(1);
        
	float *out1 = unit->m_inbuf1 + unit->m_pos;
	float *out2 = unit->m_inbuf2 + unit->m_pos;
	
	int numSamples = unit->mWorld->mFullRate.mBufLength;
	
	// copy input
        Copy(numSamples, out1, in1);
	Copy(numSamples, out2, in2);
	
	unit->m_pos += numSamples;
	
	if (unit->m_pos & unit->m_insize) {
        
        //have collected enough samples to transform next frame
        unit->m_pos = 0; //reset collection counter
		
        // copy to fftbuf

        uint32 insize=unit->m_insize * sizeof(float);
        memcpy(unit->m_fftbuf1, unit->m_inbuf1, insize);
        memcpy(unit->m_fftbuf2, unit->m_inbuf2, insize);
	
        //zero pad second part of buffer to allow for convolution
        memset(unit->m_fftbuf1+unit->m_insize, 0, insize);
        memset(unit->m_fftbuf2+unit->m_insize, 0, insize);
                   
        int log2n = unit->m_log2n;                     	
        
        // do windowing
        DoWindowing(log2n, unit->m_fftbuf1, unit->m_fftsize);
        DoWindowing(log2n, unit->m_fftbuf2, unit->m_fftsize);
		
		// do fft
/*		#if __VEC__
		ctoz(unit->m_fftbuf1, 2, outbuf1, 1, 1L<<log2n); ctoz(unit->m_fftbuf2, 2, outbuf2, 1, 1L<<log2n);
		#else      */

//in place transform for now
		rffts(unit->m_fftbuf1, log2n, 1, cosTable[log2n]);
                rffts(unit->m_fftbuf2, log2n, 1, cosTable[log2n]);
//#endif


//complex multiply time
    
int numbins = unit->m_fftsize >> 1; //unit->m_fftsize - 2 >> 1;
  
        float * p1= unit->m_fftbuf1;
        float * p2= unit->m_fftbuf2;
        
        p1[0] *= p2[0];
        p1[1] *= p2[1];
    
        //complex multiply
        for (int i=1; i<numbins; ++i) {
            float real,imag;
            int realind,imagind;
            realind= 2*i; imagind= realind+1;
            real= p1[realind]*p2[realind]- p1[imagind]*p2[imagind];
            imag= p1[realind]*p2[imagind]+ p1[imagind]*p2[realind];
                p1[realind] = real; //p2->bin[i];
                p1[imagind]= imag;
	}
        
        //copy second part from before to overlap                 
        memcpy(unit->m_overlapbuf, unit->m_outbuf+unit->m_insize, unit->m_insize * sizeof(float));	
     
        //inverse fft into outbuf        
        memcpy(unit->m_outbuf, unit->m_fftbuf1, unit->m_fftsize * sizeof(float));

         //in place
        riffts(unit->m_outbuf, log2n, 1, cosTable[log2n]);	
        
        DoWindowing(log2n, unit->m_outbuf, unit->m_fftsize);
}

//write out samples copied from outbuf, with overlap added in 
 
 float *output = ZOUT(0);
 float *out= unit->m_outbuf+unit->m_pos; 
 float *overlap= unit->m_overlapbuf+unit->m_pos; 
    
 for (int i=0; i<numSamples; ++i) {
		*++output = *++out + *++overlap;
	}
       
}



float* create_cosTable(int log2n);
float* create_cosTable(int log2n)
{
	int size = 1 << log2n;
	int size2 = size / 4 + 1;
	float *win = (float*)malloc(size2 * sizeof(float));
	double winc = twopi / size;
	for (int i=0; i<size2; ++i) {
		double w = i * winc;
		win[i] = cos(w);
	}
	return win;
}

float* create_fftwindow(int log2n);
float* create_fftwindow(int log2n)
{
	int size = 1 << log2n;
	float *win = (float*)malloc(size * sizeof(float));
	//double winc = twopi / size;
	double winc = pi / size;
	for (int i=0; i<size; ++i) {
		double w = i * winc;
		//win[i] = 0.5 - 0.5 * cos(w);
		win[i] = sin(w);
	}
	return win;
}

void init_ffts();
void init_ffts()
{
#if __VEC__
	
	for (int i=0; i<32; ++i) {
		fftsetup[i] = 0;
	}
	for (int i=0; i<15; ++i) {
		fftsetup[i] = create_fftsetup(i, kFFTRadix2);
	}
#else
	for (int i=0; i<32; ++i) {
		cosTable[i] = 0;
		fftWindow[i] = 0;
	}
        //used to be up to 13
	for (int i=3; i<15; ++i) {
		cosTable[i] = create_cosTable(i);
		fftWindow[i] = create_fftwindow(i);
	}
#endif
}


void load(InterfaceTable *inTable)
{
	ft = inTable;

	//init_SCComplex(inTable);
	init_ffts();
        
        DefineDtorUnit(Convolution);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
////DEBUG CODE

/*

       int firsttime;	//debug variable
       
       
        unit->firsttime=0;
       
 int j;
        
        
        ++unit->firsttime;     
       
        if(unit->firsttime==10) 
        {
        
        
        for(j=0; j<unit->m_fftsize; ++j)
        printf("%i val %f   %f \n",j, unit->m_fftbuf1[j], unit->m_fftbuf2[j]);
        
        } 


           
         
        if(unit->firsttime==10) 
        {
        
        
        for(j=0; j<unit->m_fftsize; ++j)
        printf("%i val %f   %f \n",j, unit->m_fftbuf1[j], unit->m_outbuf[j]);
        
        }   
*/
      
   ////////DISCARDED
   
          
                 		
		//unit->m_fftsndbuf->coord = coord_Complex;


/*
	SCComplexBuf *p1 = (SCComplexBuf*)(unit->m_fftbuf1);
	SCComplexBuf *p2 = (SCComplexBuf*)(unit->m_fftbuf2);
	
        
        p1->dc *= 0.0; //p2->dc;
	p1->nyq *= 0.0; //p2->nyq;
        
	for (int i=0; i<numbins; ++i) {
                p1->bin[i] *= 0.0; //p2->bin[i];
	}
  */         
                  

