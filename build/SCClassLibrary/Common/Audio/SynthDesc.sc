

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
	var <>name, <>parameters, <>inputs, <>outputs; 
	var <>constants, <>def;
	
	printOn { arg stream;
		stream << name << " :\n";
		parameters.do {|param| param.printOn(stream) };
		inputs.do {|input| stream << "I "; input.printOn(stream) };
		outputs.do {|output| stream << "O "; output.printOn(stream) };
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
	*readFile { arg stream;
		var numDefs, array;
		stream.getInt32; // 'SCgf'
		stream.getInt32; // version
		numDefs = stream.getInt16;
		numDefs.do {
			array = array.add( SynthDesc.new.readSynthDef(stream));
		}
		^array
	}
	readSynthDef { arg stream;
		var numParameters, numConstants, numParamNames, numUGens;
		
		inputs = [];
		outputs = [];
		
		name = stream.getPascalString;
		
		def = SynthDef.prNew(name);
		UGen.buildSynthDef = def;
		
		numConstants = stream.getInt16;
		constants = FloatArray.newClear(numConstants);
		stream.read(constants);
				
		numParameters = stream.getInt16;
		def.controls = FloatArray.newClear(numParameters);
		stream.read(def.controls);
		
		parameters = Array.fill(numParameters) 
			{ |i|
				ControlName("?", i, '?', def.controls[i]);
			};
		
		numParamNames = stream.getInt16;
		numParamNames.do 
			{
				var paramName, paramIndex;
				paramName = stream.getPascalString;
				paramIndex = stream.getInt16;
				parameters[paramIndex].name = paramName;
			};
			
		numUGens = stream.getInt16;
		numUGens.do {
			this.readUGenSpec(stream);
		};
		
		def.controlNames = parameters.select {|x| x.name.notNil };
		def.constants = Dictionary.new;
		constants.do {|k,i| def.constants.put(k,i) };
	}
	
	readUGenSpec { arg stream;
		var ugenClass, rateIndex, rate, numInputs, numOutputs, specialIndex;
		var inputSpecs, outputSpecs;
		var bus;
		var ugenInputs, ugen;
		
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
				parameters[i+specialIndex].rate = rate;
			}
		} {
			if (inputSpecs[0] == -1) 
				{ bus = constants[inputSpecs[1]].asInteger }
				{ bus = "?" }; // could do more work to determine if bus is a parameter.
			if (ugenClass.isInputUGen) {
				inputs = inputs.add( IODesc(rate, numOutputs, bus))
			} {
			if (ugenClass.isOutputUGen) {
				outputs = outputs.add( IODesc(rate, numInputs - ugenClass.numFixedArgs, bus))
			}}
		};
	}
}

