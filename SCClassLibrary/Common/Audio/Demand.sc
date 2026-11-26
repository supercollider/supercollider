Demand : MultiOutUGen {
	*ar { |trig, reset, demandUGens|
		^this.multiNewList(['audio', trig, reset] ++ demandUGens.asArray)
	}
	*kr { |trig, reset, demandUGens|
		^this.multiNewList(['control', trig, reset] ++ demandUGens.asArray)
	}
	init { |... argInputs|
		inputs = argInputs;
		^this.initOutputs(inputs.size - 2, rate)
	}
	checkInputs { ^this.checkSameRateAsFirstInput }
}

Duty : UGen {

	*ar { |dur = 1.0, reset = 0.0, level = 1.0, doneAction = 0|
		^this.multiNew('audio', dur, reset, doneAction, level)
	}
	*kr { |dur = 1.0, reset = 0.0, level = 1.0, doneAction = 0|
		^this.multiNew('control', dur, reset, doneAction, level)
	}
	checkInputs {
		^if(inputs.at(0).rate === \demand) {
			if(inputs.at(1).rate !== \demand and: { inputs.at(1).rate !== \scalar } and:
				{ inputs.at(1).rate !== rate }) {
					("reset input is not" + rate + "rate: " + inputs.at(1) + inputs.at(1).rate)
				}
		} {
			this.checkValidInputs
		}
	}
}

TDuty : Duty {
	*ar { |dur = 1.0, reset = 0.0, level = 1.0, doneAction = 0, gapFirst = 0|
		^this.multiNew('audio', dur, reset, doneAction, level, gapFirst)
	}
	*kr { |dur = 1.0, reset = 0.0, level = 1.0, doneAction = 0, gapFirst = 0|
		^this.multiNew('control', dur, reset, doneAction, level, gapFirst)
	}
}

DemandEnvGen : UGen {

	*kr { |level, dur, shape = 1, curve = 0, gate = 1.0, reset = 1.0,
			levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction = 0|
		^this.multiNew('control', level, dur, shape, curve, gate, reset,
			levelScale, levelBias, timeScale, doneAction)
	}
	*ar { |level, dur, shape = 1, curve = 0, gate = 1.0, reset = 1.0,
			levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction = 0|
		if(gate.rate === 'audio' or: { reset.rate === 'audio' }) {
			if(gate.rate !== 'audio') { gate = K2A.ar(gate) };
			if(reset.rate !== 'audio') { reset = K2A.ar(reset) }
		};
		^this.multiNew('audio', level, dur, shape, curve, gate, reset,
			levelScale, levelBias, timeScale, doneAction)
	}
}

DUGen : UGen {
	// some n-ary op special cases
	linlin { |inMin, inMax, outMin, outMax, clip = \minmax|
		^((this.prune(inMin, inMax, clip) - inMin) / (inMax - inMin) * (outMax - outMin) + outMin)
	}

	linexp { |inMin, inMax, outMin, outMax, clip = \minmax|
		^(pow(outMax / outMin, (this - inMin) / (inMax - inMin)) * outMin)
		.prune(outMin, outMax, clip)
	}

	explin { |inMin, inMax, outMin, outMax, clip = \minmax|
		^(log(this.prune(inMin, inMax, clip) / inMin))
			/ (log(inMax / inMin)) * (outMax - outMin) + outMin
	}

	expexp { |inMin, inMax, outMin, outMax, clip = \minmax|
		^pow(outMax / outMin, log(this.prune(inMin, inMax, clip / inMin) / log(inMax / inMin)) * outMin)
	}
}

Dseries : DUGen {
	*new { |start = 1, step = 1, length = inf|
		^this.multiNew('demand', length, start, step)
	}
}

Dgeom : DUGen {
	*new { |start = 1, grow = 2, length = inf|
		^this.multiNew('demand', length, start, grow)
	}
}

Dbufrd : DUGen {
	*new { |bufnum = 0, phase = 0.0, loop = 1.0|
		^this.multiNew('demand', bufnum, phase, loop)
	}
}

Dbufwr : DUGen {
	*new { |input = 0.0, bufnum = 0, phase = 0.0, loop = 1.0|
		^this.multiNew('demand', bufnum, phase, input, loop)
	}
}

ListDUGen : DUGen {
	*new { |list, repeats = 1|
		^this.multiNewList(['demand', repeats] ++ list)
	}
}

Dseq : ListDUGen {}
Dser : ListDUGen {}
Dshuf : ListDUGen {}
Drand : ListDUGen {}
Dxrand : ListDUGen {}

Dwrand : DUGen {
	*new { |list, weights, repeats = 1|
		var size = list.size;
		weights = weights.extend(size, 0.0);
		^this.multiNewList(['demand', repeats, size] ++ weights ++ list)
	}
}

Dswitch1 : DUGen {
	*new { |list, index|
		^this.multiNewList(['demand', index] ++ list)
	}
}

Dswitch : Dswitch1 {}

Dwhite : DUGen {
	*new { |lo = 0.0, hi = 1.0, length = inf|
		^this.multiNew('demand', length, lo, hi)
	}
}

Diwhite : Dwhite {}

Dbrown : DUGen {
	*new { |lo = 0.0, hi = 1.0, step = 0.01, length = inf|
		^this.multiNew('demand', length, lo, hi, step)
	}
}

Dibrown : Dbrown {}

Dstutter : DUGen {
	classvar suggestNew;

	*initClass {
		suggestNew = { |n, in|
			"The use of Dstutter is not recommended. Please use Ddup instead.".warn;
			suggestNew = { |n, in| Ddup(n, in) };
			suggestNew.(n, in)
		}
	}

	*new { |n, in|
		^suggestNew.(n, in)
	}
}

Ddup : DUGen {
	*new { |n, in|
		^this.multiNew('demand', n, in)
	}
}

Dconst : DUGen {
	*new { |sum, in, tolerance = 0.001|
		^this.multiNew('demand', sum, in, tolerance)
	}
}

Dreset : DUGen {
	*new { |in, reset = 0.0|
		^this.multiNew('demand', in, reset)
	}
}

Dpoll : DUGen {
	*new { |in, label, run = 1, trigid = -1|
		^this.multiNew('demand', in, label, run, trigid)
	}

	*new1 { |rate, in, label, run, trigid|
		label = label ?? { "DemandUGen(%)".format(in.class) };
		label = label.ascii;
		^super.new.rate_(rate).addToSynth.init(*[in, trigid, run, label.size] ++ label)
	}
}

// behave as identical in multiple uses

Dunique : UGen {
	var <protected, buffer, writer, iwr;

	*new { |source, maxBufferSize = 1024, protected = true|
		^super.new.init(source, maxBufferSize, protected)
	}

	init { |source, maxBufferSize, argProtected|
		protected = argProtected.asBoolean;
		buffer = LocalBuf(maxBufferSize).clear;
		if(protected) {
			iwr = LocalBuf(1).clear;
			// here we also limit to the largest integer a 32bit float can correctly address
			writer = Dbufwr(source, buffer, Dbufwr(Dseries(0, 1, 16777216), iwr), 1)
		} {
			// here we can simply loop
			writer = Dbufwr(source, buffer, Dseq([Dseries(0, 1, maxBufferSize)], inf), 0)
		}
	}

	asUGenInput {
		var index, ird, overrun;
		var brd = buffer <! writer; // we call the writer from each reader

		if(protected) {
			ird = LocalBuf(1).clear;
			index = Dbufwr(Dseries(0, 1, inf), ird);
			overrun = Dbufrd(iwr) - Dbufrd(ird) > buffer.numFrames;
			// catch buffer overrun by switching to a zero length series
			brd = Dswitch1([brd, Dseries(length: 0)], overrun)
		} {
			index = Dseq([Dseries(0, 1, buffer.numFrames)], inf)
		};

		^Dbufrd(brd, index, 1)
	}

}
