

IODesc {
	var <>rate, <>numberOfChannels, <>startingChannel;
	
	*new { arg rate, numberOfChannels, startingChannel="?";
		^super.newCopyArgs(rate, numberOfChannels, startingChannel)
	}
	
	printOn { arg stream;
		stream << rate.asString << " " << startingChannel << " " << numberOfChannels << "\n"
	}	
}


SynthDesc {
	var <>name, <>controlNames;
	var <>controls, <>inputs, <>outputs; 
	
	var <>constants, <>def;
	
	printOn { arg stream;
		stream << name << " :\n";
		controls.do {|control| control.printOn(stream) };
		inputs.do {|input| stream << "   I "; input.printOn(stream) };
		outputs.do {|output| stream << "   O "; output.printOn(stream) };
	}
	
	*read { arg path;
		^path.pathMatch.collect { |filename|
			var file, result;
			//filename.postln;
			file = File(filename, "r");
			result = this.readFile(file);
			file.close;
			result
		}
	}
	*readFile { arg stream, keepDefs=false;
		var numDefs, array;
		stream.getInt32; // 'SCgf'
		stream.getInt32; // version
		numDefs = stream.getInt16;
		numDefs.do {
			array = array.add( SynthDesc.new.readSynthDef(stream, keepDefs));
		}
		^array
	}
	readSynthDef { arg stream, keepDef=false;
		var numControls, numConstants, numControlNames, numUGens;
		
		inputs = [];
		outputs = [];
		
		name = stream.getPascalString;
		
		def = SynthDef.prNew(name);
		UGen.buildSynthDef = def;
		
		numConstants = stream.getInt16;
		constants = FloatArray.newClear(numConstants);
		stream.read(constants);
				
		numControls = stream.getInt16;
		def.controls = FloatArray.newClear(numControls);
		stream.read(def.controls);
		
		controls = Array.fill(numControls) 
			{ |i|
				ControlName("?", i, '?', def.controls[i]);
			};
		
		numControlNames = stream.getInt16;
		numControlNames.do 
			{
				var controlName, controlIndex;
				controlName = stream.getPascalString;
				controlIndex = stream.getInt16;
				controls[controlIndex].name = controlName;
				controlNames = controlNames.add(controlName);
			};
			
		numUGens = stream.getInt16;
		numUGens.do {
			this.readUGenSpec(stream);
		};
		
		def.controlNames = controls.select {|x| x.name.notNil };
		def.constants = Dictionary.new;
		constants.do {|k,i| def.constants.put(k,i) };
		if (keepDef.not) {
			// throw away unneeded stuff
			def = nil;
			constants = nil;
		}
	}
	
	readUGenSpec { arg stream;
		var ugenClass, rateIndex, rate, numInputs, numOutputs, specialIndex;
		var inputSpecs, outputSpecs;
		var bus;
		var ugenInputs, ugen;
		var control;
		
		ugenClass = stream.getPascalString.asSymbol.asClass;
		rateIndex = stream.getInt8;
		rate = [\scalar,\control,\audio][rateIndex];
		numInputs = stream.getInt16;
		numOutputs = stream.getInt16;
		specialIndex = stream.getInt16;
		inputSpecs = Int16Array.newClear(numInputs * 2);
		outputSpecs = Int8Array.newClear(numOutputs);
		stream.read(inputSpecs);
		stream.read(outputSpecs);
				
		ugenInputs = [];
		forBy (0,inputSpecs.size-1,2) {|i|
			var ugenIndex, outputIndex, input, ugen;
			ugenIndex = inputSpecs[i];
			outputIndex = inputSpecs[i+1];
			input = if (ugenIndex < 0) 
				{ 
					constants[outputIndex] 
				}{ 
					ugen = def.children[ugenIndex];
					if (ugen.isKindOf(MultiOutUGen)) {
						ugen.channels[outputIndex]
					}{
						ugen
					}
				};
			ugenInputs = ugenInputs.add(input);
		};
		ugen = ugenClass.newFromDesc(rate, numOutputs, ugenInputs, specialIndex).source;
		ugen.addToSynth(ugen);
		
		if (ugenClass.isControlUGen) {
			numOutputs.do { |i|
				controls[i+specialIndex].rate = rate;
			}
		} {
			if (ugenClass.isInputUGen) {
				bus = ugen.inputs[0];
				if (bus.class.isControlUGen) {
					control = controls.detect {|item| item.index == bus.specialIndex };
					if (control.notNil) { bus = control.name }
				};
				inputs = inputs.add( IODesc(rate, numOutputs, bus))
			} {
			if (ugenClass.isOutputUGen) {
				bus = ugen.inputs[0].source;
				if (bus.class.isControlUGen) {
					control = controls.detect {|item| item.index == bus.specialIndex };
					if (control.notNil) { bus = control.name }
				};
				outputs = outputs.add( IODesc(rate, numInputs - ugenClass.numFixedArgs, bus))
			}}
		};
	}
}

