ControlName {
	var <>name, <>index, <>rate, <>defaultValue, <>argNum, <>lag;

	*new { |name, index, rate, defaultValue, argNum, lag|
		^super.newCopyArgs(name.asSymbol, index, rate, defaultValue, argNum, lag ? 0.0)
	}

	numChannels {
		^defaultValue.asArray.size
	}

	printOn { |stream|
		stream << "ControlName  P " << index.asString;
		if(name.notNil) { stream << " " << name };
		if(rate.notNil) { stream << " " << rate };
		if(defaultValue.notNil) { stream << " " << defaultValue }
		//stream << "\n"
	}

	spec {
		^UGen.buildSynthDef.specs[name]
	}

	spec_{ |spec|
		if(spec.notNil) {
			spec = spec.asSpec;

			defaultValue ?? {
				defaultValue = spec.default
			};

			UGen.buildSynthDef.specs[name] !? _.setFrom(spec) ?? {
				UGen.buildSynthDef.specs[name] = spec
			}
		}
	}

}

Control : MultiOutUGen {
	var <values;

	*names { |names|
		var synthDef, index;
		synthDef = UGen.buildSynthDef;
		index = synthDef.controlIndex;
		names = names.asArray;
		names.do { |name, i|
			synthDef.addControlName(
				ControlName(name, index + i, 'control',
					nil, synthDef.allControlNames.size)
			)
		}
	}
	*kr { |values|
		^this.multiNewList(['control'] ++ values.asArray)
	}
	*ir { |values|
		^this.multiNewList(['scalar'] ++ values.asArray)
	}
	init { |... argValues|
		var ctlNames, lastControl;
		values = argValues;
		if(synthDef.notNil) {
			specialIndex = synthDef.controls.size;
			synthDef.controls = synthDef.controls.addAll(values);
			ctlNames = synthDef.controlNames;

			if(ctlNames.size > 0) {
				// the current control is always the last added, so:
				lastControl = synthDef.controlNames.last;
				if(lastControl.defaultValue.isNil) {
					// only write if not there yet:
					lastControl.defaultValue_(values.unbubble)
				}
			};

			synthDef.controlIndex = synthDef.controlIndex + values.size

		};
		^this.initOutputs(values.size, rate)
	}
	*isControlUGen { ^true }

}

AudioControl : MultiOutUGen {
	var <values;

	*names { |names|
		var synthDef, index;
		synthDef = UGen.buildSynthDef;
		index = synthDef.controlIndex;
		names = names.asArray;
		names.do { |name, i|
			synthDef.addControlName(
				ControlName(name, index + i, 'audio',
					nil, synthDef.allControlNames.size)
			)
		}
	}
	*ar { |values|
		^this.multiNewList(['audio'] ++ values.asArray)
	}
	init { |... argValues|
		values = argValues;
		if(synthDef.notNil) {
			specialIndex = synthDef.controls.size;
			synthDef.controls = synthDef.controls.addAll(values);
			synthDef.controlIndex = synthDef.controlIndex + values.size
		};
		^this.initOutputs(values.size, rate)
	}
	*isAudioControlUGen { ^true }
	*isControlUGen { ^true }
}

TrigControl : Control {}

LagControl : Control {
	*kr { |values, lags|
		var outputs;

		values = values.asArray;

		if(lags.isNumber) {
			lags = lags ! values.size
		} {
			lags = lags.asArray
		};

		if(values.size != lags.size) {
			"LagControl values.size != lags.size".error;
			^nil
		};
		values = values.clump(16);
		lags = lags.clump(16);
		outputs = [];
		values.size.do { |i|
			outputs = outputs ++ this.multiNewList(['control'] ++ values.at(i) ++ lags.at(i))
		};
		^outputs
	}
	*ar { |values, lags|
		^AudioControl.ar(values).lag(lags)
	}
	*ir {
		^this.shouldNotImplement(thisMethod)
	}
	init { |... stuff|
		var lags, size, size2;
		size = stuff.size;
		size2 = size >> 1;
		values = stuff[.. size2 - 1];
		inputs = stuff[size2 .. size - 1];
		if(synthDef.notNil) {
			specialIndex = synthDef.controls.size;
			synthDef.controls = synthDef.controls.addAll(values);
			synthDef.controlIndex = synthDef.controlIndex + values.size
		};
		^this.initOutputs(values.size, rate)
	}
}

AbstractIn : MultiOutUGen {
	*isInputUGen { ^true }
}

In : AbstractIn {
	*ar { |bus = 0, numChannels = 1|
		^this.multiNew('audio', numChannels, bus)
	}
	*kr { |bus = 0, numChannels = 1|
		^this.multiNew('control', numChannels, bus)
	}
	init { |numChannels ... argBus|
		inputs = argBus.asArray;
		^this.initOutputs(numChannels, rate)
	}
}

LocalIn : AbstractIn {
	*ar { |numChannels = 1, default = 0.0|
		^this.multiNew('audio', numChannels, *default)
	}
	*kr { |numChannels = 1, default = 0.0|
		^this.multiNew('control', numChannels, *default)
	}
	init { |numChannels ... default|
		inputs = default.wrapExtend(numChannels);
		^this.initOutputs(numChannels, rate)
	}
}


LagIn : AbstractIn {
	*kr { |bus = 0, numChannels = 1, lag = 0.1|
		^this.multiNew('control', numChannels, bus, lag)
	}
	init { |numChannels ... argInputs|
		inputs = argInputs.asArray;
		^this.initOutputs(numChannels, rate)
	}
}

InFeedback : AbstractIn {
	*ar { |bus = 0, numChannels = 1|
		^this.multiNew('audio', numChannels, bus)
	}
	init { |numChannels ... argBus|
		inputs = argBus.asArray;
		^this.initOutputs(numChannels, rate)
	}
}

InTrig : AbstractIn {
	*kr { |bus = 0, numChannels = 1|
		^this.multiNew('control', numChannels, bus)
	}
	init { |numChannels ... argBus|
		inputs = argBus.asArray;
		^this.initOutputs(numChannels, rate)
	}
}

AbstractOut : UGen {
	numOutputs { ^0 }
	writeOutputSpecs {}
	checkInputs {
		if(rate == 'audio') {
			for(this.class.numFixedArgs, inputs.size - 1) { |i|
				if(inputs.at(i).rate != 'audio') {
					^(" input at index " + i +
							"(" + inputs.at(i) + ") is not audio rate")
				}
			}
		} {
			if(inputs.size <= this.class.numFixedArgs) {
				^"missing input at index 1"
			}
		};
		^this.checkValidInputs
	}

	*isOutputUGen { ^true }
	*numFixedArgs { ^this.subclassResponsibility(thisMethod) }

	numAudioChannels {
		^inputs.size - this.class.numFixedArgs
	}

	writesToBus { ^this.subclassResponsibility(thisMethod) }
}

Out : AbstractOut {
	*ar { |bus, channelsArray|
		channelsArray = this.replaceZeroesWithSilence(channelsArray.asUGenInput(this).asArray);
		this.multiNewList(['audio', bus] ++ channelsArray)
		^0.0		// Out has no output
	}
	*kr { |bus, channelsArray|
		this.multiNewList(['control', bus] ++ channelsArray.asArray)
		^0.0		// Out has no output
	}
	*numFixedArgs { ^1 }
	writesToBus { ^true }
}

ReplaceOut : Out {}
OffsetOut : Out {
	*kr { ^this.shouldNotImplement(thisMethod) }
}

LocalOut : AbstractOut {
	*ar { |channelsArray|
		channelsArray = this.replaceZeroesWithSilence(channelsArray.asUGenInput(this).asArray);
		this.multiNewList(['audio'] ++ channelsArray)
		^0.0		// LocalOut has no output
	}
	*kr { |channelsArray|
		this.multiNewList(['control'] ++ channelsArray.asArray)
		^0.0		// LocalOut has no output
	}
	*numFixedArgs { ^0 }
	writesToBus { ^false }
}


XOut : AbstractOut {
	*ar { |bus, xfade, channelsArray|
		channelsArray = this.replaceZeroesWithSilence(channelsArray.asUGenInput(this).asArray);
		this.multiNewList(['audio', bus, xfade] ++ channelsArray)
		^0.0		// Out has no output
	}
	*kr { |bus, xfade, channelsArray|
		this.multiNewList(['control', bus, xfade] ++ channelsArray.asArray)
		^0.0		// Out has no output
	}
	*numFixedArgs { ^2 }
	writesToBus { ^true }
}
