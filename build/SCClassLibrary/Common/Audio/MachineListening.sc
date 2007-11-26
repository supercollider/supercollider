
//4 outs 
BeatTrack : MultiOutUGen {

	*kr { arg fft, lock=0;
	
		^this.multiNew('control',fft, lock);
	}
	
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(4, rate);
	}
}

//loudness output in sones
Loudness : UGen {

	*kr { arg fft, smask=0.25, tmask=1; 

		^this.multiNew('control',fft, smask, tmask);
	}
}


Onsets : UGen {
	*kr { |chain, threshold=0.5, odftype=\rcomplex, relaxtime=1, 
				floor=0.1, mingap=10, medianspan=11, whtype=1|
		if(odftype.class == Symbol){
			odftype = #[\power, \magsum, \complex, \rcomplex, \phase, \wphase,\mkl]
				.indexOf(odftype)
		};
		// mingap of 10 frames, @ 44100 & 512 & 50%, is about 0.058 seconds
		^this.multiNew('control', chain, threshold, odftype, relaxtime, 
				floor, mingap, medianspan, whtype)
	}
}
