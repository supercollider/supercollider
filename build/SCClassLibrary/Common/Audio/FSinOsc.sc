/*
	FSinOsc - fixed frequency sine oscillator
	arguments :
		freq - frequency in cycles per second. Must be a scalar.
		mul - multiply by signal or scalar
		add - add to signal or scalar
		
	This unit generator uses a very fast algorithm for generating a sine
	wave at a fixed frequency.
*/

FSinOsc : UGen {	
	*ar { arg freq=440.0, iphase = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, iphase).madd(mul, add)
	}
	*kr { arg freq=440.0, iphase = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq, iphase).madd(mul, add)
	}
}


Klang : UGen {
	var freqs, amps, phases;
	
	*ar { arg specificationsArrayRef, freqscale = 1.0, freqoffset = 0.0;
		var specs, a, b, c;
		# a, b, c = specificationsArrayRef.value;
		specs = [a, b ?? {Array.fill(a.size,1.0)}, c ?? {Array.fill(a.size,0.0)}].flop.flat;
		^this.multiNewList(['audio', freqscale, freqoffset] ++ specs )
	}
}

Klank : UGen {
	var freqs, amps, times;
	
	*ar { arg specificationsArrayRef, input, freqscale = 1.0, freqoffset = 0.0, decayscale = 1.0;
		var specs, a, b, c;
		# a, b, c = specificationsArrayRef.value;
		specs = [a, b ?? {Array.fill(a.size,1.0)}, c ?? {Array.fill(a.size,1.0)}].flop.flat;
		^this.multiNewList(['audio', input, freqscale, freqoffset, decayscale] ++ specs )
	}
}


Blip : UGen {	
	*ar { arg freq=440.0, numharm = 200.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, numharm).madd(mul, add)
	}
}

Saw : UGen {	
	*ar { arg freq=440.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq).madd(mul, add)
	}
}

Pulse : UGen {	
	*ar { arg freq=440.0, width = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, width).madd(mul, add)
	}
}
