//third party FFT UGens

//sick lincoln remembers complex analysis courses
PV_ConformalMap : UGen 
{

	*new { arg buffer, areal = 0.0, aimag = 0.0;
		^this.multiNew('control', buffer, areal, aimag)
	}
}


//sick lincoln learns what real convolution involves
Convolution : UGen
{
	*ar { arg in, kernel, framesize=512,mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, kernel, framesize).madd(mul, add);
	}
}


//jensen andersen inspired FFT feature detector
PV_JensenAndersen : UGen
{
	*ar { arg buffer, propsc=0.25, prophfe=0.25, prophfc=0.25, propsf=0.25, threshold=1.0, waittime=0.04;
		^this.multiNew('audio', buffer, propsc, prophfe, prophfc, propsf,  threshold, waittime);
	}
}


PV_HainsworthFoote : UGen
{
	*ar { arg buffer, propmkl=0.0, propfoote=0.0, threshold=1.0, waittime=0.04;
		^this.multiNew('audio', buffer, propmkl, propfoote, threshold, waittime);
	}
}

//not FFT but useful for time domain onset detection
RunningSum : UGen
{
	*ar { arg in, numsamp=40;
		^this.multiNew('audio', in, numsamp);
	}
	
	*rms { arg in, numsamp=40;
		^(RunningSum.ar(in.squared,numsamp)*(numsamp.reciprocal)).sqrt;
	}
}


