//4 outs
BeatTrack : MultiOutUGen {

	*kr { arg chain, lock=0;

		if(chain.isKindOf(FFT).not){
			// Automatically drop in an FFT, possible now that we have LocalBuf
			chain = FFT(LocalBuf(if(SampleRate.ir>48000, 2048, 1024)), chain);
		};

		^this.multiNew('control',chain, lock);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(4, rate);
	}
}

//loudness output in sones
Loudness : UGen {

	*kr { arg chain, smask=0.25, tmask=1;

		^this.multiNew('control',chain, smask, tmask);
	}
}


Onsets : UGen {
	*kr { |chain, threshold=0.5, odftype=\rcomplex, relaxtime=1,
				floor=0.1, mingap=10, medianspan=11, whtype=1, rawodf=0|
		if(odftype.class == Symbol){
			odftype = #[\power, \magsum, \complex, \rcomplex, \phase, \wphase,\mkl]
				.indexOf(odftype)
		};
		// mingap of 10 frames, @ 44100 & 512 & 50%, is about 0.058 seconds
		^this.multiNew('control', chain, threshold, odftype, relaxtime,
				floor, mingap, medianspan, whtype, rawodf)
	}
}


//transient input not currently used but reserved for future use in downweighting frames which have high transient content
KeyTrack : UGen {

	*kr { arg chain,keydecay=2.0,chromaleak= 0.5; //transient=0.0;

		^this.multiNew('control',chain,keydecay,chromaleak); //transient;
	}
}


//a bufnum could be added as third argument for passing arbitrary band spacing data
MFCC : MultiOutUGen {
	*kr { arg chain, numcoeff=13;
		^this.multiNew('control', chain, numcoeff);
	}

	init { arg ... theInputs;
		inputs = theInputs;

		^this.initOutputs(theInputs[1], rate);
	}
}



//6 outs
BeatTrack2 : MultiOutUGen {

	*kr { arg busindex, numfeatures, windowsize=2.0, phaseaccuracy=0.02, lock=0, weightingscheme;

		^this.multiNew('control',busindex, numfeatures,windowsize, phaseaccuracy, lock, weightingscheme ? (-2.1));
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(6, rate);
	}
}



SpecFlatness : UGen
{
	*kr { | buffer |
		^this.multiNew('control', buffer)
	}
}
SpecPcile : UGen
{
	*kr { | buffer, fraction = 0.5, interpolate = 0 |
		^this.multiNew('control', buffer, fraction, interpolate)
	}
}
SpecCentroid : UGen
{
	*kr { | buffer |
		^this.multiNew('control', buffer)
	}
}
