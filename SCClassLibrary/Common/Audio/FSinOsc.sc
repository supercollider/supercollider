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
	*ar { arg specificationsArrayRef, freqscale = 1.0, freqoffset = 0.0;
		specificationsArrayRef = specificationsArrayRef.multichannelExpandRef(2);
			^this.multiNewList(['audio', freqscale,
						freqoffset, specificationsArrayRef] )
	}
	*new1 { arg rate, freqscale, freqoffset, arrayRef;
		var specs, freqs, amps, phases;
		# freqs, amps, phases = arrayRef.dereference;
		specs = [freqs,
				amps ?? {Array.fill(freqs.size,1.0)},
				phases ?? {Array.fill(freqs.size,0.0)}
				].flop.flat;

		^super.new.rate_(rate).addToSynth.init([freqscale,freqoffset] ++ specs);
	}
 	init { arg theInputs;
 		// store the inputs as an array
 		inputs = theInputs;
 	}
	argNamesInputsOffset { ^2 }
}

Klank : UGen {
	*ar { arg specificationsArrayRef, input, freqscale = 1.0, freqoffset = 0.0, decayscale = 1.0;
			specificationsArrayRef = specificationsArrayRef.multichannelExpandRef(2);
			^this.multiNewList(['audio',  input, freqscale,
						freqoffset, decayscale, specificationsArrayRef] )
	}
	*new1 { arg rate, input, freqscale, freqoffset, decayscale, arrayRef;
		var specs, freqs, amps, times;
		# freqs, amps, times = arrayRef.dereference;
		specs = [freqs,
				amps ?? {Array.fill(freqs.size,1.0)},
				times ?? {Array.fill(freqs.size,1.0)}
				].flop.flat;

		^super.new.rate_(rate).addToSynth.init([input,freqscale,freqoffset,decayscale] ++ specs);
	}
 	init { arg theInputs;
 		// store the inputs as an array
 		inputs = theInputs;
 	}
	argNamesInputsOffset { ^2 }
}

DynKlank : UGen {

	*ar { arg specificationsArrayRef, input, freqscale = 1.0, freqoffset = 0.0, decayscale = 1.0;
		^this.multiNew(\audio, specificationsArrayRef, input, freqscale, freqoffset, decayscale)
	}

	*kr { arg specificationsArrayRef, input, freqscale = 1.0, freqoffset = 0.0, decayscale = 1.0;
		^this.multiNew(\control, specificationsArrayRef, input, freqscale, freqoffset, decayscale)
	}

	*new1 { arg rate, specificationsArrayRef, input, freqscale = 1.0, freqoffset = 0.0, decayscale = 1.0;
		var spec = specificationsArrayRef.value;
		var selector = this.methodSelectorForRate(rate);
		^Ringz.perform(selector,
				input,
				spec[0] ? #[440.0] * freqscale + freqoffset,
				spec[2] ? #[1.0] * decayscale,
				spec[1] ? #[1.0]
		).sum
	}
}

DynKlang : UGen {

	*ar { arg specificationsArrayRef, freqscale = 1.0, freqoffset = 0.0;
		^this.multiNew(\audio, specificationsArrayRef, freqscale, freqoffset);
	}

	*kr { arg specificationsArrayRef, freqscale = 1.0, freqoffset = 0.0;
		^this.multiNew(\control, specificationsArrayRef, freqscale, freqoffset);
	}

	*new1 { arg rate, specificationsArrayRef, freqscale = 1.0, freqoffset = 0.0;
		var spec = specificationsArrayRef.value;
		var selector = this.methodSelectorForRate(rate);
		^SinOsc.perform(selector,
				spec[0] ? #[440.0] * freqscale + freqoffset,
				spec[2] ? #[0.0],
				spec[1] ? #[1.0]
		).sum
	}
}


Blip : UGen {
	*ar { arg freq=440.0, numharm = 200.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, numharm).madd(mul, add)
	}
	*kr { arg freq=440.0, numharm = 200.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq, numharm).madd(mul, add)
	}
}

Saw : UGen {
	*ar { arg freq=440.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq).madd(mul, add)
	}
	*kr { arg freq=440.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq).madd(mul, add)
	}
}

Pulse : UGen {
	*ar { arg freq=440.0, width = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, width).madd(mul, add)
	}
	*kr { arg freq=440.0, width = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq, width).madd(mul, add)
	}
}
