
IODesc {
	var <>rate, <>numberOfChannels, <>startingChannel;
	
	*new { arg rate, numberOfChannels, startingChannel="?";
		^super.newCopyArgs(rate, numberOfChannels, startingChannel)
	}
	
	printOn { arg stream;
		stream << rate.asString << " " << startingChannel.source 
				<< " " << numberOfChannels << "\n"
	}	
}


SynthDesc {
	var <>name, <>controlNames;
	var <>controls, <>inputs, <>outputs; 
	
	var <>constants, <>def;
	var <>msgFunc, <>hasGate = false;
	
	send { arg server, completionMsg; 
		def.send(server, completionMsg);
	}
	
	printOn { arg stream;
		stream << name << " :\n";
		controls.do {|control| control.printOn(stream) };
		inputs.do {|input| stream << "   I "; input.printOn(stream) };
		outputs.do {|output| stream << "   O "; output.printOn(stream) };
	}
	
	*read { arg path, keepDefs=false, dict;
		dict = dict ?? { IdentityDictionary.new };
		path.pathMatch.do { |filename|
			var file, result;
			file = File(filename, "r");
			protect {
				dict = this.readFile(file, keepDefs, dict);
			}{
				file.close;
			};
		};
		^dict;
	}
	*readFile { arg stream, keepDefs=false, dict;
		var numDefs;
		dict = dict ?? { IdentityDictionary.new };
		stream.getInt32; // 'SCgf'
		stream.getInt32; // version
		numDefs = stream.getInt16;
		numDefs.do {
			var desc;
			desc = SynthDesc.new.readSynthDef(stream, keepDefs);
			dict.put(desc.name.asSymbol, desc);
		}
		^dict
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
		};
		this.makeMsgFunc;
		UGen.buildSynthDef = nil;
		
	}
	
	readUGenSpec { arg stream;
		var ugenClass, rateIndex, rate, numInputs, numOutputs, specialIndex;
		var inputSpecs, outputSpecs;
		var bus;
		var ugenInputs, ugen;
		var control;
		
		ugenClass = stream.getPascalString.asSymbol.asClass;
		rateIndex = stream.getInt8;
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

		rate = #[\scalar,\control,\audio][rateIndex];
		ugen = ugenClass.newFromDesc(rate, numOutputs, ugenInputs, specialIndex).source;
		ugen.addToSynth(ugen);
		
		if (ugenClass.isControlUGen) {
			numOutputs.do { |i|
				controls[i+specialIndex].rate = rate;
			}
		} {
			if (ugenClass.isInputUGen) {
				bus = ugen.inputs[0].source;
				if (bus.class.isControlUGen) {
					control = controls.detect {|item| item.index == bus.specialIndex };
					if (control.notNil) { bus = control.name };
				};
				inputs = inputs.add( IODesc(rate, numOutputs, bus))
			} {
			if (ugenClass.isOutputUGen) {
				bus = ugen.inputs[0].source;
				if (bus.class.isControlUGen) {
					control = controls.detect {|item| item.index == bus.specialIndex };
					if (control.notNil) { bus = control.name };
				};
				outputs = outputs.add( IODesc(rate, numInputs - ugenClass.numFixedArgs, bus))
			}}
		};
	}
	
	makeMsgFunc {
		var string, comma=false;
		string = String.streamContents {|stream|
			stream << "#{ ";
			if (controlNames.size > 0) {
				stream << "arg " ;
			};
			controls.do {|controlName, i|
				var name, name2;
				name = controlName.name;
				if (name.asString != "?") {
					if (name == "gate") {
						hasGate = true;
					}{
						if (name[1] == $_) { name2 = name.drop(2) } { name2 = name }; 
						if (comma) { stream << ", " } { comma = true };
						stream << name2 << " = " << controlName.defaultValue.asStringPrec(7); 
					};
				};
			};
			if (controlNames.size > 0) {
				stream << ";\n" ;
			};
			stream << "\t[ ";
			comma = false;
			controls.do {|controlName, i|
				var name, name2;
				name = controlName.name;
				if (name.asString != "?") {
					if (name != "gate") {
						if (name[1] == $_) { name2 = name.drop(2) } { name2 = name }; 
						if (comma) { stream << ", " } { comma = true };
						stream << "'" << name << "', " << name2; 
					};
				};
			};
			stream << " ] }";
		};
		msgFunc = string.interpret;
	}
	
}

SynthDescLib {
	classvar <>all, <>global;
	var <>name, <>synthDescs, <>servers;

	*new { arg name, servers;
		if (name.isNil) { "SynthDescLib must have a name".error; ^nil }
		^super.new.name_(name).servers_(servers ? {Server.default}).init;
	}
	init {
		all.put(name.asSymbol, this);
		synthDescs = IdentityDictionary.new;
	}
	*initClass {
		Class.initClassTree(Server);
		all = IdentityDictionary.new;
		global = this.new(\global);
	}

	*send {
		global.send;
	}
	*read { arg path;
		global.read(path);
	}
	at { arg i; ^synthDescs.at(i) }
	*at { arg i; ^global.at(i) }

	send { 
		servers.do {|server|
			synthDescs.do {|desc| desc.send(server.value) };
		};
	}
	read	{ arg path;
		if (path.isNil) {
			path = SynthDef.synthDefDir ++ "*.scsyndef";
		};
		synthDescs = SynthDesc.read(path, true, synthDescs);
		postf("SynthDescLib '%' read of '%' done.\n", name, path);
	}
}


