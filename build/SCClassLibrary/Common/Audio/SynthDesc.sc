
ParameterDesc {
	var <>index, <>defaultValue, <>name, <>rate;
	
	*new { arg index, defaultValue=0.0, name="", rate='?';
		^super.newCopyArgs(index, defaultValue, name, rate)
	}
	
	printOn { arg stream;
		stream << "P " << index.asString << " " << name << " " << rate << " " << defaultValue << "\n"
	}	
}

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
	var <>name, <>parameters, <>inputs, <>outputs, <>constants; 

	printOn { arg stream;
		stream << name << " :\n";
		parameters.do {|param| param.printOn(stream) };
		inputs.do {|input| stream << "I "; input.printOn(stream) };
		outputs.do {|output| stream << "O "; output.printOn(stream) };
	}
	
	*read { arg path;
		^path.pathMatch.collect { |filename|
			var file, result;
			filename.postln;
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
		var paramValues;
		
		inputs = [];
		outputs = [];
		
		name = stream.getPascalString;
		[\synthdef, name].postln;
		numConstants = stream.getInt16;
		constants = FloatArray.newClear(numConstants);
		stream.read(constants);
		
		numParameters = stream.getInt16;
		paramValues = FloatArray.newClear(numParameters);
		stream.read(paramValues);
		
		parameters = Array.fill(numParameters) 
			{ |i|
				ParameterDesc(i, paramValues[i]);
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
		
		
	}
	
	readUGenSpec { arg stream;
		var ugenClass, rate, numInputs, numOutputs, specialIndex;
		var inputSpecs, outputSpecs;
		var bus;
		
		ugenClass = stream.getPascalString.asSymbol.asClass;
		rate = [\scalar,\control,\audio][stream.getInt8];
		numInputs = stream.getInt16;
		numOutputs = stream.getInt16;
		specialIndex = stream.getInt16;
		inputSpecs = Int16Array.newClear(numInputs * 2);
		outputSpecs = Int8Array.newClear(numOutputs);
		stream.read(inputSpecs);
		stream.read(outputSpecs);
		
		[\ugen, ugenClass, rate, numInputs, numOutputs].postln;
		
		
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

