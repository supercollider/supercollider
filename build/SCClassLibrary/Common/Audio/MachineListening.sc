
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


