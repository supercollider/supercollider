IODesc {
	var <>rate, <>numberOfChannels, <>startingChannel, <>type;

	*new { arg rate, numberOfChannels, startingChannel="?", type;
		^super.newCopyArgs(rate, numberOfChannels, startingChannel, type)
	}

	printOn { arg stream;
		stream << rate.asString << " " << type.name << " " << startingChannel << " " << numberOfChannels
	}
}


SynthDesc {
	classvar <>mdPlugin, <>populateMetadataFunc;

	var <>name, <>controlNames, <>controlDict;
	var <>controls, <>inputs, <>outputs;
	var <>metadata;

	var <>constants, <>def;
	var <>msgFunc, <>hasGate = false, <>hasArrayArgs, <>hasVariants, <>canFreeSynth = false;
	var <msgFuncKeepGate = false;

	*newFrom { arg synthdef;
		^synthdef.asSynthDesc
	}

	*initClass {
		mdPlugin = AbstractMDPlugin;	// override in your startup file
	}

	send { arg server, completionMsg;
		def.send(server, completionMsg);
	}

	printOn { arg stream;
		stream << "SynthDesc '" << name << "' \nControls:\n";
		controls.do {|control| control.printOn(stream); $\n.printOn(stream) };
		inputs.do {|input| stream << "   I "; input.printOn(stream); $\n.printOn(stream) };
		outputs.do {|output| stream << "   O "; output.printOn(stream); $\n.printOn(stream) };
	}

	*read { arg path, keepDefs=false, dict;
		dict = dict ?? { IdentityDictionary.new };
		path.pathMatch.do { |filename|
			var file, result;
			file = File(filename, "r");
			protect {
				dict = this.readFile(file, keepDefs, dict, filename);
			}{
				file.close;
			};
		};
		^dict;
	}
		// path is for metadata -- only this method has direct access to the new SynthDesc
		// really this should be a private method -- use *read instead
	*readFile { arg stream, keepDefs=false, dict, path;
		var numDefs, version;
		dict = dict ?? { IdentityDictionary.new };
		stream.getInt32; // 'SCgf'
		version = stream.getInt32; // version
		numDefs = stream.getInt16;
		numDefs.do {
			var desc;
			if(version >= 2, {
				desc = SynthDesc.new.readSynthDef2(stream, keepDefs);
			},{
				desc = SynthDesc.new.readSynthDef(stream, keepDefs);
			});
			dict.put(desc.name.asSymbol, desc);
				// AbstractMDPlugin dynamically determines the md archive type
				// from the file extension
			if(path.notNil) {
				desc.metadata = AbstractMDPlugin.readMetadata(path);
			};
			this.populateMetadataFunc.value(desc);
			if(desc.def.notNil and: { stream.isKindOf(CollStream).not }) {
				desc.def.metadata ?? { desc.def.metadata = () };
				desc.def.metadata.put(\shouldNotSend, true)
					.put(\loadPath, path);
			};
		}
		^dict
	}
	readSynthDef { arg stream, keepDef=false;
		var	numControls, numConstants, numControlNames, numUGens, numVariants;

		protect {

		inputs = [];
		outputs = [];
		controlDict = IdentityDictionary.new;

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
				ControlName('?', i, '?', def.controls[i]);
			};

		numControlNames = stream.getInt16;
		numControlNames.do
			{
				var controlName, controlIndex;
				controlName = stream.getPascalString.asSymbol;
				controlIndex = stream.getInt16;
				controls[controlIndex].name = controlName;
				controlNames = controlNames.add(controlName);
				controlDict[controlName] = controls[controlIndex];
			};

		numUGens = stream.getInt16;
		numUGens.do {
			this.readUGenSpec(stream);
		};

		controls.inject(nil) {|z,y|
			if(y.name=='?') { z.defaultValue = z.defaultValue.asArray.add(y.defaultValue); z } { y }
		};

		def.controlNames = controls.select {|x| x.name.notNil };
		hasArrayArgs = controls.any { |cn| cn.name == '?' };

		numVariants = stream.getInt16;
		hasVariants = numVariants > 0;
			// maybe later, read in variant names and values
			// this is harder than it might seem at first

		def.constants = Dictionary.new;
		constants.do {|k,i| def.constants.put(k,i) };
		if (keepDef.not) {
			// throw away unneeded stuff
			def = nil;
			constants = nil;
		};
		this.makeMsgFunc;

		} {
			UGen.buildSynthDef = nil;
		}

	}
	
	// synthdef ver 2
	readSynthDef2 { arg stream, keepDef=false;
		var	numControls, numConstants, numControlNames, numUGens, numVariants;

		protect {

		inputs = [];
		outputs = [];
		controlDict = IdentityDictionary.new;

		name = stream.getPascalString;

		def = SynthDef.prNew(name);
		UGen.buildSynthDef = def;

		numConstants = stream.getInt32;
		constants = FloatArray.newClear(numConstants);
		stream.read(constants);

		numControls = stream.getInt32;
		def.controls = FloatArray.newClear(numControls);
		stream.read(def.controls);

		controls = Array.fill(numControls)
			{ |i|
				ControlName('?', i, '?', def.controls[i]);
			};

		numControlNames = stream.getInt32;
		numControlNames.do
			{
				var controlName, controlIndex;
				controlName = stream.getPascalString.asSymbol;
				controlIndex = stream.getInt32;
				controls[controlIndex].name = controlName;
				controlNames = controlNames.add(controlName);
				controlDict[controlName] = controls[controlIndex];
			};

		numUGens = stream.getInt32;
		numUGens.do {
			this.readUGenSpec2(stream);
		};

		controls.inject(nil) {|z,y|
			if(y.name=='?') { z.defaultValue = z.defaultValue.asArray.add(y.defaultValue); z } { y }
		};

		def.controlNames = controls.select {|x| x.name.notNil };
		hasArrayArgs = controls.any { |cn| cn.name == '?' };

		numVariants = stream.getInt16;
		hasVariants = numVariants > 0;
			// maybe later, read in variant names and values
			// this is harder than it might seem at first

		def.constants = Dictionary.new;
		constants.do {|k,i| def.constants.put(k,i) };
		if (keepDef.not) {
			// throw away unneeded stuff
			def = nil;
			constants = nil;
		};
		this.makeMsgFunc;

		} {
			UGen.buildSynthDef = nil;
		}

	}

	readUGenSpec { arg stream;
		var ugenClass, rateIndex, rate, numInputs, numOutputs, specialIndex;
		var inputSpecs, outputSpecs;
		var addIO;
		var ugenInputs, ugen;
		var control;

		ugenClass = stream.getPascalString.asSymbol;
		if(ugenClass.asClass.isNil,{
			Error("No UGen class found for" + ugenClass + "which was specified in synth def file: " + this.name ++ ".scsyndef").throw;
		});
		ugenClass = ugenClass.asClass;

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

		addIO = {|list, nchan|
			var b = ugen.inputs[0];
			if (b.class == OutputProxy and: {b.source.isKindOf(Control)}) {
				control = controls.detect {|item| item.index == (b.outputIndex+b.source.specialIndex) };
				if (control.notNil) { b = control.name };
			};
			list.add( IODesc(rate, nchan, b, ugenClass))
		};

		if (ugenClass.isControlUGen) {
			// Control.newFromDesc does not set the specialIndex, since it doesn't call Control-init.
			// Therefore we fill it in here:
			ugen.specialIndex = specialIndex;
			numOutputs.do { |i|
				controls[i+specialIndex].rate = rate;
			}
		} {
			case
			{ugenClass.isInputUGen} {inputs = addIO.value(inputs, ugen.channels.size)}
			{ugenClass.isOutputUGen} {outputs = addIO.value(outputs, ugen.numAudioChannels)}
			{
				canFreeSynth = canFreeSynth or: { ugen.canFreeSynth };
			};
		};
	}
	
	// synthdef ver 2
	readUGenSpec2 { arg stream;
		var ugenClass, rateIndex, rate, numInputs, numOutputs, specialIndex;
		var inputSpecs, outputSpecs;
		var addIO;
		var ugenInputs, ugen;
		var control;

		ugenClass = stream.getPascalString.asSymbol;
		if(ugenClass.asClass.isNil,{
			Error("No UGen class found for" + ugenClass + "which was specified in synth def file: " + this.name ++ ".scsyndef").throw;
		});
		ugenClass = ugenClass.asClass;

		rateIndex = stream.getInt8;
		numInputs = stream.getInt32;
		numOutputs = stream.getInt32;
		specialIndex = stream.getInt16;

		inputSpecs = Int32Array.newClear(numInputs * 2);
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

		addIO = {|list, nchan|
			var b = ugen.inputs[0];
			if (b.class == OutputProxy and: {b.source.isKindOf(Control)}) {
				control = controls.detect {|item| item.index == (b.outputIndex+b.source.specialIndex) };
				if (control.notNil) { b = control.name };
			};
			list.add( IODesc(rate, nchan, b, ugenClass))
		};

		if (ugenClass.isControlUGen) {
			// Control.newFromDesc does not set the specialIndex, since it doesn't call Control-init.
			// Therefore we fill it in here:
			ugen.specialIndex = specialIndex;
			numOutputs.do { |i|
				controls[i+specialIndex].rate = rate;
			}
		} {
			case
			{ugenClass.isInputUGen} {inputs = addIO.value(inputs, ugen.channels.size)}
			{ugenClass.isOutputUGen} {outputs = addIO.value(outputs, ugen.numAudioChannels)}
			{
				canFreeSynth = canFreeSynth or: { ugen.canFreeSynth };
			};
		};
	}

	makeMsgFunc {
		var	string, comma=false;
		var	names = IdentitySet.new,
			suffix = this.hash.asHexString(8);
			// if a control name is duplicated, the msgFunc will be invalid
			// that "shouldn't" happen but it might; better to check for it
			// and throw a proper error
		controls.do({ |controlName|
			var	name;
			if(controlName.name.asString.first.isAlpha) {
				name = controlName.name.asSymbol;
				if(names.includes(name)) {
					"Could not build msgFunc for this SynthDesc: duplicate control name %"
						.format(name).warn;
					comma = true;
				} {
					names.add(name);
				};
			};
		});
			// reusing variable to know if I should continue or not
		if(comma) {
"\nYour synthdef has been saved in the library and loaded on the server, if running.
Use of this synth in Patterns will not detect argument names automatically because of the duplicate name(s).".postln;
			msgFunc = nil;
			^this
		};
		comma = false;
		names = 0;	// now, count the args actually added to the func

		string = String.streamContents {|stream|
			stream << "#{ ";
			if (controlNames.size > 0) {
				stream << "arg " ;
			};
			controls.do {|controlName, i|
				var name, name2;
				name = controlName.name.asString;
				if (name != "?") {
					if (name == "gate") {
						hasGate = true;
						if(msgFuncKeepGate) {
							if (comma) { stream << ", " } { comma = true };
							stream << name;
							names = names + 1;
						}
					}{
						if (name[1] == $_) { name2 = name.drop(2) } { name2 = name };
						if (comma) { stream << ", " } { comma = true };
						stream << name2;
						names = names + 1;
					};
				};
			};
			if (controlNames.size > 0) {
				stream << ";\n" ;
			};
			stream << "\tvar\tx" << suffix << " = Array.new(" << (names*2) << ");\n";
			comma = false;
			controls.do {|controlName, i|
				var name, name2;
				name = controlName.name.asString;
				if (name != "?") {
					if (msgFuncKeepGate or: { name != "gate" }) {
						if (name[1] == $_) { name2 = name.drop(2) } { name2 = name };
						stream << "\t" << name2 << " !? { x" << suffix
							<< ".add('" << name << "').add(" << name2 << ") };\n";
						names = names + 1;
					};
				};
			};
			stream << "\tx" << suffix << "\n}"
		};

			// do not compile the string if no argnames were added
		if(names > 0) { msgFunc = string.compile.value };
	}
	
	msgFuncKeepGate_ { |bool = false|
		if(bool != msgFuncKeepGate) {
			msgFuncKeepGate = bool;
			this.makeMsgFunc;
		}
	}

	writeMetadata { arg path, mdPlugin;
		if(metadata.isNil) { AbstractMDPlugin.clearMetadata(path); ^this };
		(mdPlugin ?? { this.class.mdPlugin }).writeMetadata(metadata, def, path);
	}

	// parse the def name out of the bytes array sent with /d_recv
	*defNameFromBytes { arg int8Array;
		var stream, n, numDefs, size;
		stream = CollStream(int8Array);

		stream.getInt32;
		stream.getInt32;
		numDefs = stream.getInt16;
		size = stream.getInt8;
		n = String.newClear(size);
		^Array.fill(size, {
		  stream.getChar.asAscii
		}).as(String)
	}

	outputData {
		var ugens = def.children;
		var outs = ugens.select(_.writesToBus);
		^outs.collect { |outUgen|
			(rate: outUgen.rate, numChannels: outUgen.numAudioChannels)
		}
	}


}

SynthDescLib {
	classvar <>all, <>global;
	var <>name, <>synthDescs, <>servers;

	*new { arg name, servers;
		if (name.isNil) { "SynthDescLib must have a name".error; ^nil }
		^super.new.name_(name).init(servers);
	}
	init { |inServers|
		all.put(name.asSymbol, this);
		synthDescs = IdentityDictionary.new;
		servers = IdentitySet.with(*inServers ? { Server.default })
	}
	*initClass {
		Class.initClassTree(Server);
		all = IdentityDictionary.new;
		global = this.new(\global);

		ServerBoot.add { |server|
			// tryToLoadReconstructedDefs = false:
			// since this is done automatically, w/o user action,
			// it should not try to do things that will cause warnings
			// (or errors, if one of the servers is not local)
			this.send(server, false)
		}
	}

	*getLib { arg libname;
		^all[libname] ?? {
			Error("library % not found".format(libname)).throw
		};
	}

	*default {
		^global
	}

	*send { |server, tryToLoadReconstructedDefs = true|
		global.send(server, tryToLoadReconstructedDefs);
	}

	*read { arg path;
		global.read(path);
	}

	at { arg i; ^synthDescs.at(i) }

	*at { arg i; ^global.at(i) }

	add { |synthdesc|
		synthDescs.put(synthdesc.name.asSymbol, synthdesc);
	}

	removeAt { |name|
		^synthDescs.removeAt(name.asSymbol);
	}

	addServer { |server|
		servers = servers.add(server); // IdentitySet = one server only once.
	}

	removeServer { |server|
		servers.remove(server);
	}

	match { |key|
		var	keyString = key.asString, dotIndex = keyString.indexOf($.), desc;
		if(dotIndex.isNil) { ^synthDescs.at(key.asSymbol) };
		if((desc = synthDescs[keyString[..dotIndex-1].asSymbol]).notNil
				and: { desc.hasVariants })
			{ ^desc }
			{ ^synthDescs.at(key.asSymbol) }
	}
	*match { |key| ^global.match(key) }

	send { |aServer, tryToLoadReconstructedDefs = true|
		// sent to all
		(aServer ? servers).do { |server|
			server = server.value;
			synthDescs.do { |desc|
				if(desc.def.metadata.trueAt(\shouldNotSend).not) {
					desc.send(server.value)
				} {
					if(tryToLoadReconstructedDefs) {
						desc.def.loadReconstructed(server);
					};
				};
			};
		};
	}

	read { arg path;
		if (path.isNil) {
			path = SynthDef.synthDefDir ++ "*.scsyndef";
		};
		synthDescs = SynthDesc.read(path, true, synthDescs);
	}
}



// Basic metadata plugins

// to disable metadata read/write
AbstractMDPlugin {
	*clearMetadata { |path|
		^thisProcess.platform.clearMetadata(path)
	}

	*writeMetadata { |metadata, synthdef, path|

		this.clearMetadata(path);
		path = this.applyExtension(path);
		this.writeMetadataFile(metadata, synthdef, path);
	}
	*writeMetadataFile {}

		// clearMetadata should ensure that only one MD file ever exists
		// therefore we can check the subclasses in turn
		// and return the first MD found
		// every subclass should have a unique extension
	*readMetadata { |path|
		var	pathTmp, classList, i;
		path = path.splitext[0] ++ ".";
		classList = this.allSubclasses;
			// ensure that SynthDescLib.mdPlugin is preferred for reading,
			// with other plugins as a fallback
			// it will also be possible to use Events or Protos as plugins this way
		if((i = classList.indexOf(SynthDesc.mdPlugin)).notNil and: { i > 0 }) {
			classList = classList.copy.swap(0, i);
		} {
			classList = [SynthDesc.mdPlugin] ++ classList;
		};
		classList.do({ |class|
			if(class.notNil and: { File.exists(pathTmp = path ++ class.mdExtension) }) {
				^class.readMetadataFile(pathTmp)
			}
		});
		^nil
	}
	*readMetadataFile { ^nil }

	*applyExtension { |path|
		^path.splitext[0] ++ "." ++ this.mdExtension
	}
	*mdExtension { ^"" }		// nothing is written anyway
}

// simple archiving of the dictionary
TextArchiveMDPlugin : AbstractMDPlugin {
	*writeMetadataFile { |metadata, synthdef, path|
		metadata.writeArchive(path)
	}

	*readMetadataFile { |path|
		^Object.readArchive(path)
	}
	*mdExtension { ^"txarcmeta" }
}
