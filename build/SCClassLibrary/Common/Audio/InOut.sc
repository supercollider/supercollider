
ControlName
{
	var <name, <index;
	*new { arg name, index;
		^super.newCopyArgs(name, index)
	}
		
}

Control : MultiOutUGen {
	var <values, <specialIndex;
	*names { arg names;
		var synthDef, index;
		synthDef = UGen.buildSynthDef;
		index = synthDef.controls.size;
		names.asArray.do({ arg name, i; 
			synthDef.controlNames = synthDef.controlNames.add(
				ControlName(name.asString, index + i)
			);
		});
	}
	*kr { arg values;
		^this.multiNewList(['control'] ++ values.asArray)
	}
	*ir { arg values;
		^this.multiNewList(['scalar'] ++ values.asArray)
	}
	init { arg ... argValues;
		values = argValues;
		if (synthDef.notNil, { 
			specialIndex = synthDef.controls.size;
			synthDef.controls = synthDef.controls.addAll(values);
		});
		^this.initOutputs(values.size, rate)
	}
}

ControlTrig : UGen {
	*kr {
		^this.multiNew('control')
	}
}

In : MultiOutUGen {	
	*ar { arg bus = 0, numChannels = 1;
		^this.multiNew('audio', numChannels, bus)
	}
	*kr { arg bus = 0, numChannels = 1;
		^this.multiNew('control', numChannels, bus)
	}
	init { arg numChannels ... argBus;
		inputs = argBus.asArray;
		^this.initOutputs(numChannels, rate)
	}
}

InFeedback : MultiOutUGen {	
	*ar { arg bus = 0, numChannels = 1;
		^this.multiNew('audio', numChannels, bus)
	}
	init { arg numChannels ... argBus;
		inputs = argBus.asArray;
		^this.initOutputs(numChannels, rate)
	}
}

InTrig : MultiOutUGen {	
	*kr { arg bus = 0, numChannels = 1;
		^this.multiNew('control', numChannels, bus)
	}
	init { arg numChannels ... argBus;
		inputs = argBus.asArray;
		^this.initOutputs(numChannels, rate)
	}
}

AbstractOut : UGen {
	numOutputs { ^0 }
	writeOutputSpecs {}
 	checkInputs {
 		if (rate == 'audio', {
 			for(1, inputs.size - 1, { arg i;
 				if (inputs.at(i).rate != 'audio', { 
 					^(" input at index " + i + 
 						"(" + inputs.at(i) + ") is not audio rate");
 				});
 			});
 		});
 		^nil
 	}
}

Out : AbstractOut {
	*ar { arg bus, channelsArray;
		this.multiNewList(['audio', bus] ++ channelsArray.asArray)
		^0.0		// Out has no output
	}
	*kr { arg bus, channelsArray;
		this.multiNewList(['control', bus] ++ channelsArray.asArray)
		^0.0		// Out has no output
	}
}


ReplaceOut : Out {}
OffsetOut : Out {}

XOut : AbstractOut {
	*ar { arg bus, xfade, channelsArray;
		this.multiNewList(['audio', bus, xfade] ++ channelsArray.asArray)
		^0.0		// Out has no output
	}
	*kr { arg bus, xfade, channelsArray;
		this.multiNewList(['control', bus, xfade] ++ channelsArray.asArray)
		^0.0		// Out has no output
	}
	checkInputs {
 		if (rate == 'audio', {
 			for(2, inputs.size - 1, { arg i;
 				if (inputs.at(i).rate != 'audio', { 
 					^(" input at index " + i + 
 						"(" + inputs.at(i) + ") is not audio rate");
 				});
 			});
 		});
 		^nil
 	}
}

