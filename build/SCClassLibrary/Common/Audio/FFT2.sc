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


