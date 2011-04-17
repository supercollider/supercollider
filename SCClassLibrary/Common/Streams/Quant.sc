// This class is used to encapsulate quantization issues associated with EventStreamPlayer and TempoClock
// quant and phase determine the starting time of something scheduled by a TempoClock
// timingOffset is an additional timing factor that allows an EventStream to compute "ahead of time" enough to allow
// negative lags for strumming a chord, etc
Quant {
	classvar	default;
	var <>quant, <>phase, <>timingOffset;

	*default { ^default ?? { Quant.new } }
	*default_ { |quant| default = quant.asQuant }

	*new { |quant = 0, phase, timingOffset| ^super.newCopyArgs(quant, phase, timingOffset) }

	nextTimeOnGrid { | clock |
		^clock.nextTimeOnGrid(quant, (phase ? 0) - (timingOffset ? 0));
	}

	asQuant { ^this.copy }

	printOn { |stream|
		stream << "Quant(" << quant;
		if(phase.notNil) { stream << ", " << phase };
		if(timingOffset.notNil) {
			stream << ", ";
			if(phase.isNil) {
				stream << "nil, ";
			};
			stream << timingOffset
		};
		stream << ")"
	}

	storeArgs { ^[quant, phase, timingOffset] }
}
