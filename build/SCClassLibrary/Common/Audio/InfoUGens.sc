
SampleRate : UGen {
	*ir { 
		^this.multiNew('control')
	}
}

SampleDur : SampleRate {}
RadiansPerSample : SampleRate {}
ControlRate : SampleRate {}

BufSampleRate : UGen {
	*kr { arg bufnum;
		^this.multiNew('control', bufnum)
	}
}

BufRateScale : BufSampleRate {}
BufFrames : BufSampleRate {}
BufSamples : UGen {}
BufChannels : UGen {}
