
InfoUGenBase : UGen {
	*ir {
		^this.multiNew('scalar')
	}
}

BufInfoUGenBase : UGen {
	*kr { arg bufnum;
		^this.multiNew('control', bufnum)
	}
	
	// the .ir method is not the safest choice. Since a buffer can be reallocated at any time,
	// using .ir will not track the changes.
	*ir { arg bufnum;
		^this.multiNew('scalar',bufnum)
	}
}

SampleRate : InfoUGenBase {}
SampleDur : InfoUGenBase {}
RadiansPerSample : InfoUGenBase {}
ControlRate : InfoUGenBase {}
SubsampleOffset : InfoUGenBase {}

NumOutputBuses : InfoUGenBase {}
NumInputBuses : InfoUGenBase {}
NumAudioBuses : InfoUGenBase {}
NumControlBuses : InfoUGenBase {}
NumBuffers : InfoUGenBase {}
NumRunningSynths : InfoUGenBase {}

BufSampleRate : BufInfoUGenBase {}
BufRateScale : BufInfoUGenBase {}
BufFrames : BufInfoUGenBase {}
BufSamples : BufInfoUGenBase {}
BufDur : BufInfoUGenBase {}
BufChannels : BufInfoUGenBase {}

////////////////////////////////////////////

