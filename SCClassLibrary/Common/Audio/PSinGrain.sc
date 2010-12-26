/*
	PSinGrain - fixed frequency sine oscillator
	arguments :
		freq - frequency in cycles per second. Must be a scalar.
		dur - grain duration
		amp - amplitude of grain

	This unit generator uses a very fast algorithm for generating a sine
	wave at a fixed frequency.
*/

PSinGrain : UGen {
	*ar { arg freq = 440.0, dur = 0.2, amp = 1.0;
		^this.multiNew('audio', freq, dur, amp)
	}
}
