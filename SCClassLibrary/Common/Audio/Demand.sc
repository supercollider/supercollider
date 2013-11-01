Demand : MultiOutUGen {
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

// old version with gap first
TDuty_old  {
	*ar { arg dur = 1.0, reset = 0.0, level = 1.0, doneAction = 0;
		^TDuty.ar(dur, reset, level, doneAction, 1)
	}
	*kr { arg dur = 1.0, reset = 0.0, level = 1.0, doneAction = 0;
		^TDuty.kr(dur, reset, level, doneAction, 1)
	}
}

DemandEnvGen : UGen {

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
	*new { arg bufnum = 0, phase = 0.0, loop = 1.0;
		^this.multiNew('demand', bufnum, phase, loop)
	}
}

Dbufwr : DUGen {
	*new { arg input = 0.0, bufnum = 0, phase = 0.0, loop = 1.0;
		^this.multiNew('demand', bufnum, phase, input, loop)
	}
}

ListDUGen : DUGen {
	*new { arg list, repeats = 1;
		^this.multiNewList(['demand', repeats] ++ list)
	}
}

Dseq : ListDUGen {}
Dser : ListDUGen {}
Dshuf : ListDUGen {}
Drand : ListDUGen {}
Dxrand : ListDUGen {}

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
	*new { arg lo = 0.0, hi = 1.0, length = inf;
		^this.multiNew('demand', length, lo, hi)
	}
}

Diwhite : Dwhite {}

Dbrown : DUGen {
	*new { arg lo = 0.0, hi = 1.0, step = 0.01, length = inf;
		^this.multiNew('demand', length, lo, hi, step)
	}
}

Dibrown : Dbrown {}

Dstutter : DUGen {
	*new { arg n, in;
		^this.multiNew('demand', n, in)
	}
}

Donce : DUGen {
	*new { arg in;
		^this.multiNew('demand', in)
	}
}

Dreset : DUGen {
	*new { arg in, reset = 0.0;
		^this.multiNew('demand', in, reset)
	}
}

Dpoll : DUGen {
	*new { arg in, label, run = 1, trigid = -1;
		^this.multiNew('demand', in, label, run, trigid)
	}

	*new1 { arg rate, in, label, run, trigid;
		label = label ?? { "DemandUGen(%)".format(in.class) };
		label = label.ascii;
		^super.new.rate_(rate).addToSynth.init(*[in, trigid, run, label.size] ++ label);
	}
}

// behave as identical in multiple uses

Dunique : UGen {
	var <>source, stutter, numUses;

	*new { arg source;
		^super.new.source_(source).init
	}

	init {
		numUses = 0;
		stutter = Dstutter(1, source);
	}

	asUGenInput {
		numUses = numUses + 1;
		stutter.inputs[0] = numUses;
		^stutter
	}

}
