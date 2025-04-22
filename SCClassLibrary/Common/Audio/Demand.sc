Demand : MultiOutUGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^false }

	*ar { arg trig, reset, demandUGens;
		^this.multiNewList(['audio', trig, reset] ++ demandUGens.asArray)
	}
	*kr { arg trig, reset, demandUGens;
		^this.multiNewList(['control', trig, reset] ++ demandUGens.asArray)
	}
	init { arg ... argInputs;
		inputs = argInputs;
		^this.initOutputs(inputs.size - 2, rate)
	}
	checkInputs { ^this.checkSameRateAsFirstInput }
}

Duty : UGen {
	resourceManagers { ^if(this.hasObservableEffect) { [UGenDoneResourceManager] } { [] } }
	hasObservableEffect { ^this.implHasObservableEffectViaDoneAction(2) }
	canBeReplacedByIdenticalCall { ^false }

	*ar { arg dur = 1.0, reset = 0.0, level = 1.0, doneAction = 0;
		^this.multiNew('audio', dur, reset, doneAction, level)
	}
	*kr { arg dur = 1.0, reset = 0.0, level = 1.0, doneAction = 0;
		^this.multiNew('control', dur, reset, doneAction, level)
	}
	checkInputs {
		^if(inputs.at(0).rate === \demand) {
			if (inputs.at(1).rate !== \demand and: { inputs.at(1).rate !== \scalar } and:
				{ inputs.at(1).rate !== rate }) {
				("reset input is not" + rate + "rate: " + inputs.at(1) + inputs.at(1).rate);
			}
		} {
			this.checkValidInputs
		}
	}
}

TDuty : Duty {
	*ar { arg dur = 1.0, reset = 0.0, level = 1.0, doneAction = 0, gapFirst = 0;
		^this.multiNew('audio', dur, reset, doneAction, level, gapFirst)
	}
	*kr { arg dur = 1.0, reset = 0.0, level = 1.0, doneAction = 0, gapFirst = 0;
		^this.multiNew('control', dur, reset, doneAction, level, gapFirst)
	}
}

DemandEnvGen : UGen {
	resourceManagers { ^if(this.hasObservableEffect) { [UGenDoneResourceManager] } { [] } }
	hasObservableEffect { ^this.implHasObservableEffectViaDoneAction(9) }
	canBeReplacedByIdenticalCall { ^true }

	*kr { arg level, dur, shape = 1, curve = 0, gate = 1.0, reset = 1.0,
		levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction=0;
		^this.multiNew('control', level, dur, shape, curve, gate, reset,
			levelScale, levelBias, timeScale, doneAction)
	}
	*ar { arg level, dur, shape = 1, curve = 0, gate = 1.0, reset = 1.0,
		levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction=0;
		if(gate.rate === 'audio' or: { reset.rate === 'audio' }) {
			if(gate.rate !== 'audio') { gate = K2A.ar(gate) };
			if(reset.rate !== 'audio') { reset = K2A.ar(reset) };
		};
		^this.multiNew('audio', level, dur, shape, curve, gate, reset,
			levelScale, levelBias, timeScale, doneAction)
	}
}

DUGen : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	// some n-ary op special cases
	linlin { arg inMin, inMax, outMin, outMax, clip=\minmax;
		^((this.prune(inMin, inMax, clip)-inMin)/(inMax-inMin) * (outMax-outMin) + outMin);
	}

	linexp { arg inMin, inMax, outMin, outMax, clip=\minmax;
		^(pow(outMax/outMin, (this-inMin)/(inMax-inMin)) * outMin)
		.prune(outMin, outMax, clip);
	}

	explin { arg inMin, inMax, outMin, outMax, clip=\minmax;
		^(log(this.prune(inMin, inMax, clip)/inMin))
		/ (log(inMax/inMin)) * (outMax-outMin) + outMin
	}

	expexp { arg inMin, inMax, outMin, outMax, clip=\minmax;
		^pow(outMax/outMin, log(this.prune(inMin, inMax, clip/inMin) / log(inMax/inMin)) * outMin)
	}
}

Dseries : DUGen {
	*new { arg start = 1, step = 1, length = inf;
		^this.multiNew('demand', length, start, step)
	}
}

Dgeom : DUGen {
	*new { arg start = 1, grow = 2, length = inf;
		^this.multiNew('demand', length, start, grow)
	}
}

Dbufrd : DUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg bufnum = 0, phase = 0.0, loop = 1.0;
		^this.multiNew('demand', bufnum, phase, loop)
	}
}

Dbufwr : DUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg input = 0.0, bufnum = 0, phase = 0.0, loop = 1.0;
		^this.multiNew('demand', bufnum, phase, input, loop)
	}
}

ListDUGen : DUGen {
	*new { arg list, repeats = 1;
		^this.multiNewList(['demand', repeats] ++ list)
	}
}

Dseq : ListDUGen { }
Dser : ListDUGen { }
Dshuf : ListDUGen { }
Drand : ListDUGen { }
Dxrand : ListDUGen { }

Dwrand : DUGen {
	*new { arg list, weights, repeats = 1;
		var size = list.size;
		weights = weights.extend(size, 0.0);
		^this.multiNewList(['demand', repeats, size] ++ weights ++ list)
	}
}

Dswitch1 : DUGen {
	*new { arg list, index;
		^this.multiNewList(['demand', index] ++ list)
	}
}

Dswitch : Dswitch1 {}

Dwhite : DUGen {
	resourceManagers { ^[UGenRandomResourceManager] }
	randomAccessType { ^\gen }
	canBeReplacedByIdenticalCall { ^false } // Randomness is not identical

	*new { arg lo = 0.0, hi = 1.0, length = inf;
		^this.multiNew('demand', length, lo, hi)
	}
}

Diwhite : Dwhite {}

Dbrown : DUGen {
	resourceManagers { ^[UGenRandomResourceManager] }
	randomAccessType { ^\gen }
	canBeReplacedByIdenticalCall { ^false }

	*new { arg lo = 0.0, hi = 1.0, step = 0.01, length = inf;
		^this.multiNew('demand', length, lo, hi, step)
	}
}

Dibrown : Dbrown {}

Dstutter : DUGen {
	classvar suggestNew;

	*initClass {
		suggestNew = { |n, in|
			"The use of Dstutter is not recommended. Please use Ddup instead.".warn;
			suggestNew = {|n,in| Ddup(n, in)};
			suggestNew.(n, in);
		};
	}

	*new { |n, in|
		^suggestNew.(n, in);
	}
}

Ddup : DUGen {
	*new { |n, in|
		^this.multiNew('demand', n, in)
	}
}

Dconst : DUGen {
	*new { arg sum, in, tolerance = 0.001;
		^this.multiNew('demand', sum, in, tolerance);
	}
}

Dreset : DUGen {
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^false }

	*new { arg in, reset = 0.0;
		^this.multiNew('demand', in, reset)
	}
}

Dpoll : DUGen {
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^false }

	*new { arg in, label, run = 1, trigid = -1;
		^this.multiNew('demand', in, label, run, trigid)
	}

	*new1 { arg rate, in, label, run, trigid;
		label = label ?? { "DemandUGen(%)".format(in.class) };
		label = label.ascii;
		^super.new.rate_(rate).addToSynth.init(*[in, trigid, run, label.size] ++ label);
	}
}

// TODO: what does this comment refer to?
// behave as identical in multiple uses

Dunique : UGen {
	// TODO: This class needs revisiting and should call createWeakConnectionTo as needed.

	var <protected, buffer, writer, iwr;

	resourceManagers { ^[] }
	hasObservableEffect { ^false } // TODO: maybe?

	*new { arg source, maxBufferSize = 1024, protected = true;
		^super.new.init(source, maxBufferSize, protected)
	}

	init { arg source, maxBufferSize, argProtected;
		protected = argProtected.asBoolean;
		buffer = LocalBuf(maxBufferSize).clear;
		if(protected) {
			iwr = LocalBuf(1).clear;
			// here we also limit to the largest integer a 32bit float can correctly address
			writer = Dbufwr(source, buffer, Dbufwr(Dseries(0, 1, 16777216), iwr), 1);
		} {
			// here we can simply loop
			writer = Dbufwr(source, buffer, Dseq([Dseries(0, 1, maxBufferSize)], inf), 0);
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
			brd = Dswitch1([brd, Dseries(length:0)], overrun);
		} {
			index = Dseq([Dseries(0, 1, buffer.numFrames)], inf)
		};

		^Dbufrd(brd, index, 1);
	}

}
