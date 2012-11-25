SynthDef {
	var <>name, <>func;

	var <>controls, <>controlNames; // ugens add to this
	var <>allControlNames;
	var <>controlIndex=0;
	var <>children;

	var <>constants;
	var <>constantSet;
	var <>maxLocalBufs;

	// topo sort
	var <>available;
	var <>variants;
	var <>widthFirstUGens;
	var rewriteInProgress;

	var <>desc, <>metadata;

	classvar <synthDefDir;

	*synthDefDir_ { arg dir;
		if (dir.last.isPathSeparator.not )
			{ dir = dir ++ thisProcess.platform.pathSeparator };
		synthDefDir = dir;
	}

	*initClass {
		synthDefDir = Platform.userAppSupportDir ++ "/synthdefs/";
		// Ensure exists:
		synthDefDir.mkdir;
	}

	*new { arg name, ugenGraphFunc, rates, prependArgs, variants, metadata;
		^this.prNew(name).variants_(variants).metadata_(metadata).children_(Array.new(64))
			.build(ugenGraphFunc, rates, prependArgs)
	}
	*prNew { arg name;
		^super.new.name_(name.asString)
	}

	storeArgs { ^[name.asSymbol, func] }

	build { arg ugenGraphFunc, rates, prependArgs;
		protect {
			this.initBuild;
			this.buildUgenGraph(ugenGraphFunc, rates, prependArgs);
			this.finishBuild;
			func = ugenGraphFunc;
		} {
			UGen.buildSynthDef = nil;
		}
	}

	*wrap { arg func, rates, prependArgs;
		if (UGen.buildSynthDef.isNil) {
			"SynthDef.wrap should be called inside a SynthDef ugenGraphFunc.\n".error;
			^0
		};
		^UGen.buildSynthDef.buildUgenGraph(func, rates, prependArgs);
	}
	//only write if no file exists
	*writeOnce { arg name, func, rates, prependArgs, variants, dir, metadata, mdPlugin;
		this.new(name, func, rates, prependArgs, variants, metadata).writeOnce(dir, mdPlugin)
	}
	writeOnce { arg dir, mdPlugin;
		this.writeDefFile(dir, false, mdPlugin)
	}

	initBuild {
		UGen.buildSynthDef = this;
		constants = Dictionary.new;
		constantSet = Set.new;
		controls = nil;
		controlIndex = 0;
		maxLocalBufs = nil;
	}
	buildUgenGraph { arg func, rates, prependArgs;
		var result;
		// save/restore controls in case of *wrap
		var saveControlNames = controlNames;

		controlNames = nil;

		prependArgs = prependArgs.asArray;
		this.addControlsFromArgsOfFunc(func, rates, prependArgs.size);
		result = func.valueArray(prependArgs ++ this.buildControls);

		controlNames = saveControlNames

		^result

	}
	addControlsFromArgsOfFunc { arg func, rates, skipArgs=0;
		var def, names, values,argNames, specs;

		def = func.def;
		argNames = def.argNames;
		if(argNames.isNil) { ^nil };
		names = def.argNames[skipArgs..];
		// OK what we do here is separate the ir, tr and kr rate arguments,
		// create one Control ugen for all of each rate,
		// and then construct the argument array from combining
		// the OutputProxies of these two Control ugens in the original order.
		values = def.prototypeFrame[skipArgs..].extend( names.size );
		if((specs = metadata.tryPerform(\at, \specs)).notNil) {
			values = values.collect { |value, i|
				value ?? {
					(specs[names[i]] ?? { names[i] }).tryPerform(\asSpec).tryPerform(\default) ? 0
				}
			}
		} {
			values = values.collect {|value| value ? 0.0 };
		};
		rates = rates.asArray.extend(names.size, 0).collect {|lag| lag ? 0.0 };
		names.do { arg name, i;
			var c, c2, value, lag;
			#c, c2 = name.asString;
			value = values[i];
			lag = rates[i];
			case { (lag == \ir) or: { c == $i and: { c2 == $_ }}}
			{
				if (lag.isNumber and: { lag != 0 }) {
					Post << "WARNING: lag value "<< lag <<" for i-rate arg '"
						<< name <<"' will be ignored.\n";
				};
				this.addIr(name, value);
			}
			{(lag == \tr) or: { c == $t and: { c2 == $_ }}}
			{
				if (lag.isNumber and: { lag != 0 }) {
					Post << "WARNING: lag value "<< lag <<" for trigger arg '"
						<< name <<"' will be ignored.\n";
				};
				this.addTr(name, value);
			}
			{(lag == \ar) or: { c == $a and: { c2 == $_ }}}
			{
				if (lag.isNumber and: { lag != 0 }) {
					Post << "WARNING: lag value "<< lag <<" for audio arg '"
						<< name <<"' will be ignored.\n";
				};
				this.addAr(name, value);
			}
			{
				if (lag == \kr) { lag = 0.0 };
				this.addKr(name, value, lag);
			};
		};
	}

	addControlName { arg cn;
		controlNames = controlNames.add(cn);
		allControlNames = allControlNames.add(cn);
	}

	// allow incremental building of controls
	addNonControl { arg name, values;
		this.addControlName(ControlName(name, nil, 'noncontrol',
			values.copy, controlNames.size));
	}
	addIr { arg name, values;
		this.addControlName(ControlName(name, controls.size, 'scalar',
			values.copy, controlNames.size));
	}
	addKr { arg name, values, lags;
		this.addControlName(ControlName(name, controls.size, 'control',
			values.copy, controlNames.size, lags.copy));
	}
	addTr { arg name, values;
		this.addControlName(ControlName(name, controls.size, 'trigger',
			values.copy, controlNames.size));
	}
	addAr { arg name, values;
		this.addControlName(ControlName(name, controls.size, 'audio',
			values.copy, controlNames.size))
	}

	buildControls {
		var controlUGens, index, values, lags, valsize;
		var def, argNames;

		var arguments = Array.newClear(controlNames.size);

		var nonControlNames = controlNames.select {|cn| cn.rate == 'noncontrol' };
		var irControlNames = controlNames.select {|cn| cn.rate == 'scalar' };
		var krControlNames = controlNames.select {|cn| cn.rate == 'control' };
		var trControlNames = controlNames.select {|cn| cn.rate == 'trigger' };
		var arControlNames = controlNames.select {|cn| cn.rate == 'audio' };

		if (nonControlNames.size > 0) {
			nonControlNames.do {|cn|
				arguments[cn.argNum] = cn.defaultValue;
			};
		};
		if (irControlNames.size > 0) {
			values = nil;
			irControlNames.do {|cn|
				values = values.add(cn.defaultValue);
			};
			index = controlIndex;
			controlUGens = Control.ir(values.flat).asArray.reshapeLike(values);
			irControlNames.do {|cn, i|
				cn.index = index;
				index = index + cn.defaultValue.asArray.size;
				arguments[cn.argNum] = controlUGens[i];
				this.setControlNames(controlUGens[i], cn);
			};
		};
		if (trControlNames.size > 0) {
			values = nil;
			trControlNames.do {|cn|
				values = values.add(cn.defaultValue);
			};
			index = controlIndex;
			controlUGens = TrigControl.kr(values.flat).asArray.reshapeLike(values);
			trControlNames.do {|cn, i|
				cn.index = index;
				index = index + cn.defaultValue.asArray.size;
				arguments[cn.argNum] = controlUGens[i];
				this.setControlNames(controlUGens[i], cn);
			};
		};
		if (arControlNames.size > 0) {
			values = nil;
			arControlNames.do {|cn|
				values = values.add(cn.defaultValue);
			};
			index = controlIndex;
			controlUGens = AudioControl.ar(values.flat).asArray.reshapeLike(values);
			arControlNames.do {|cn, i|
				cn.index = index;
				index = index + cn.defaultValue.asArray.size;
				arguments[cn.argNum] = controlUGens[i];
				this.setControlNames(controlUGens[i], cn);
			};
		};
		if (krControlNames.size > 0) {
			values = nil;
			lags = nil;
			krControlNames.do {|cn|
				valsize = cn.defaultValue.asArray.size;
				values = values.add(cn.defaultValue);
				lags = lags.addAll(cn.lag.asArray.wrapExtend(valsize));
			};
			index = controlIndex;
			if (lags.any {|lag| lag != 0 }) {
				controlUGens = LagControl.kr(values.flat, lags).asArray.reshapeLike(values);
			}{
				controlUGens = Control.kr(values.flat).asArray.reshapeLike(values);
			};
			krControlNames.do {|cn, i|
				cn.index = index;
				index = index + cn.defaultValue.asArray.size;
				arguments[cn.argNum] = controlUGens[i];
				this.setControlNames(controlUGens[i], cn);
			};
		};
		controlNames = controlNames.reject {|cn| cn.rate == 'noncontrol' };

		^arguments
	}

	setControlNames {arg controlUGens, cn;
		controlUGens.isArray.if({
			controlUGens.do({arg thisCtrl;
				thisCtrl.name_(cn.name);
				})
			}, {
			controlUGens.name_(cn.name)
			});
	}

	finishBuild {

		this.optimizeGraph;
		this.collectConstants;
		this.checkInputs;// will die on error

		// re-sort graph. reindex.
		this.topologicalSort;
		this.indexUGens;
		UGen.buildSynthDef = nil;
	}

	asBytes {
		var stream = CollStream.on(Int8Array.new(256));
		this.asArray.writeDef(stream);
		^stream.collection;
	}
	writeDefFile { arg dir, overwrite(true), mdPlugin;
		var desc, defFileExistedBefore;
		if((metadata.tryPerform(\at, \shouldNotSend) ? false).not) {
			dir = dir ? SynthDef.synthDefDir;
			defFileExistedBefore = File.exists(dir +/+ name ++ ".scsyndef");
			super.writeDefFile(name, dir, overwrite);
			if(overwrite or: { defFileExistedBefore.not }) {
				desc = this.asSynthDesc;
				desc.metadata = metadata;
				SynthDesc.populateMetadataFunc.value(desc);
				desc.writeMetadata(dir +/+ name, mdPlugin);
			};
		} {
			// actual error, not just warning as in .send and .load,
			// because you might try to write the file somewhere other than
			// the default location - could be fatal later, so crash now
			MethodError("This SynthDef (%) was reconstructed from a .scsyndef file. It does not contain all the required structure to write back to disk. File was not written."
				.format(name), this).throw
		}
	}
	
	writeDef { arg file;
		// This describes the file format for the synthdef files.
		var allControlNamesTemp, allControlNamesMap;

		file.putPascalString(name);

		this.writeConstants(file);

		//controls have been added by the Control UGens
		file.putInt32(controls.size);
		controls.do { | item |
			file.putFloat(item);
		};

		allControlNamesTemp = allControlNames.reject { |cn| cn.rate == \noncontrol };
		file.putInt32(allControlNamesTemp.size);
		allControlNamesTemp.do { | item |
			if (item.name.notNil) {
				file.putPascalString(item.name.asString);
				file.putInt32(item.index);
			};
		};

		file.putInt32(children.size);
		children.do { | item |
			item.writeDef(file);
		};

		file.putInt16(variants.size);
		if (variants.size > 0) {
			allControlNamesMap = ();
			allControlNamesTemp.do { |cn|
				allControlNamesMap[cn.name] = cn;
			};
			variants.keysValuesDo {|varname, pairs|
				var varcontrols;

				varname = name ++ "." ++ varname;
				if (varname.size > 32) {
					Post << "variant '" << varname << "' name too long.\n";
					^nil
				};
				varcontrols = controls.copy;
				pairs.pairsDo { |cname, values|
					var cn, index;
					cn = allControlNamesMap[cname];
					if (cn.notNil) {
						values = values.asArray;
						if (values.size > cn.defaultValue.asArray.size) {
							postf("variant: '%' control: '%' size mismatch.\n",
								varname, cname);
							^nil
						}{
							index = cn.index;
							values.do {|val, i|
								varcontrols[index + i] = val;
							}
						}
					}{
						postf("variant: '%' control: '%' not found.\n",
								varname, cname);
						^nil
					}
				};
				file.putPascalString(varname);
				varcontrols.do { | item |
					file.putFloat(item);
				};
			};
		};
	}
	
	writeConstants { arg file;
		var array = FloatArray.newClear(constants.size);
		constants.keysValuesDo { arg value, index;
			array[index] = value;
		};

		file.putInt32(constants.size);
		array.do { | item |
			file.putFloat(item)
		};
	}

	checkInputs {
		var firstErr;
		children.do { arg ugen;
			var err;
			if ((err = ugen.checkInputs).notNil) {
				err = ugen.class.asString + err;
				err.postln;
				ugen.dumpArgs;
				if(firstErr.isNil) { firstErr = err };
			};
		};
		if(firstErr.notNil) {
			("SynthDef" + this.name + "build failed").postln;
			Error(firstErr).throw
		};
		^true
	}



	// UGens do these
	addUGen { arg ugen;
		if (rewriteInProgress.isNil) {
			// we don't add ugens, if a rewrite operation is in progress
			ugen.synthIndex = children.size;
			ugen.widthFirstAntecedents = widthFirstUGens.copy;
			children = children.add(ugen);
		}
	}
	removeUGen { arg ugen;
		// lazy removal: clear entry and later remove all nil enties
		children[ugen.synthIndex] = nil;
	}
	replaceUGen { arg a, b;
		if (b.isKindOf(UGen).not) {
			Error("replaceUGen assumes a UGen").throw;
		};

		b.widthFirstAntecedents = a.widthFirstAntecedents;
		b.descendants = a.descendants;
		b.synthIndex = a.synthIndex;

		children[a.synthIndex] = b;

		children.do { arg item, i;
			if (item.notNil) {
				item.inputs.do { arg input, j;
					if (input === a) { item.inputs[j] = b };
				};
			};
		};
	}
	addConstant { arg value;
		if (constantSet.includes(value).not) {
			constantSet.add(value);
			constants[value] = constants.size;
		};
	}


	// multi channel expansion causes a non optimal breadth-wise ordering of the graph.
	// the topological sort below follows branches in a depth first order,
	// so that cache performance of connection buffers is optimized.

	optimizeGraph {
		var oldSize;
		this.initTopoSort;

		rewriteInProgress = true;
		children.copy.do { arg ugen;
			ugen.optimizeGraph;
		};
		rewriteInProgress = nil;

		// fixup removed ugens
		oldSize = children.size;
		children.removeEvery(#[nil]);
		if (oldSize != children.size) {
			this.indexUGens
		};
	}
	collectConstants {
		children.do { arg ugen;
			ugen.collectConstants;
		};
	}

	initTopoSort {
		available = nil;
		children.do { arg ugen;
			ugen.antecedents = Set.new;
			ugen.descendants = Set.new;
		};
		children.do { arg ugen;
			// this populates the descendants and antecedents
			ugen.initTopoSort;
		};
		children.reverseDo { arg ugen;
			ugen.descendants = ugen.descendants.asArray.sort(
								{ arg a, b; a.synthIndex < b.synthIndex }
							);
			ugen.makeAvailable; // all ugens with no antecedents are made available
		};
	}
	cleanupTopoSort {
		children.do { arg ugen;
			ugen.antecedents = nil;
			ugen.descendants = nil;
			ugen.widthFirstAntecedents = nil;
		};
	}
	topologicalSort {
		var outStack;
		this.initTopoSort;
		while { available.size > 0 }
		{
			outStack = available.pop.schedule(outStack);
		};
		children = outStack;
		this.cleanupTopoSort;
	}
	indexUGens {
		children.do { arg ugen, i;
			ugen.synthIndex = i;
		};
	}

	dumpUGens {
		name.postln;
		children.do { arg ugen, i;
			var inputs, ugenName;
			if (ugen.inputs.notNil) {
				inputs = ugen.inputs.collect {|in|
					if (in.respondsTo(\dumpName)) { in.dumpName }{ in };
				};
			};
			[ugen.dumpName, ugen.rate, inputs].postln;
		};
	}

	// make SynthDef available to all servers
	add { arg libname, completionMsg, keepDef = true;
		var	servers, desc = this.asSynthDesc(libname ? \global, keepDef);
		if(libname.isNil) {
			servers = Server.allRunningServers
		} {
			servers = SynthDescLib.getLib(libname).servers
		};
		servers.do { |each|
			this.doSend(each.value, completionMsg.value(each))
		}
	}

	*removeAt { arg name, libname = \global;
		var lib = SynthDescLib.getLib(libname);
		lib.removeAt(name);
		lib.servers.do { |each|
			each.value.sendMsg("/d_free", name)
		};
	}


	// methods for special optimizations

	// only send to servers
	send { arg server, completionMsg;
		var servers = (server ?? { Server.allRunningServers }).asArray;
		servers.do { |each|
			if(each.serverRunning.not) {
				"Server % not running, could not send SynthDef.".format(server.name).warn
			};
			if(metadata.trueAt(\shouldNotSend)) {
				this.loadReconstructed(each, completionMsg);
			} {
				this.doSend(each, completionMsg);
			}
		}
	}

	doSend { |server, completionMsg|
		var bytes = this.asBytes;
		if (bytes.size < (65535 div: 4)) {
			server.sendMsg("/d_recv", bytes, completionMsg)
		} {
			if (server.isLocal) {
				"SynthDef % too big for sending. Retrying via synthdef file".format(name).warn;
				this.writeDefFile(synthDefDir);
				server.sendMsg("/d_load", synthDefDir ++ name ++ ".scsyndef", completionMsg)
			} {
				"SynthDef % too big for sending.".format(name).warn;
			}
		}
	}

	// send to server and write file
	load { arg server, completionMsg, dir(synthDefDir);
		server = server ? Server.default;
		if(metadata.trueAt(\shouldNotSend)) {
			this.loadReconstructed(server, completionMsg);
		} {
			// should remember what dir synthDef was written to
			dir = dir ? synthDefDir;
			this.writeDefFile(dir);
			server.sendMsg("/d_load", dir ++ name ++ ".scsyndef", completionMsg)
		};
	}

	// write to file and make synth description
	store { arg libname=\global, dir(synthDefDir), completionMsg, mdPlugin;
		var lib = SynthDescLib.getLib(libname);
		var file, path = dir ++ name ++ ".scsyndef";
		if(metadata.falseAt(\shouldNotSend)) {
			protect {
				var bytes, desc;
				file = File(path, "w");
				bytes = this.asBytes;
				file.putAll(bytes);
				file.close;
				lib.read(path);
				lib.servers.do { arg server;
					this.doSend(server.value, completionMsg)
				};
				desc = lib[this.name.asSymbol];
				desc.metadata = metadata;
				SynthDesc.populateMetadataFunc.value(desc);
				desc.writeMetadata(path, mdPlugin);
			} {
				file.close
			}
		} {
			lib.read(path);
			lib.servers.do { arg server;
				this.loadReconstructed(server, completionMsg);
			};
		};
	}

	asSynthDesc { |libname=\global, keepDef = true|
		var	lib, stream = CollStream(this.asBytes);
		libname ?? { libname = \global };
		lib = SynthDescLib.getLib(libname);
		SynthDesc.readFile(stream, keepDef, lib.synthDescs);
		desc = lib[name.asSymbol];
		if(keepDef) { desc.def = this };
		if(metadata.notNil) { desc.metadata = metadata };
		^desc
	}

	// this method warns and does not halt
	// because loading existing def from disk is a viable alternative
	// to get the synthdef to the server

	loadReconstructed { arg server, completionMsg;
			"SynthDef (%) was reconstructed from a .scsyndef file, "
			"it does not contain all the required structure to send back to the server."
				.format(name).warn;
			if(server.isLocal) {
				"Loading from disk instead.".postln;
				server.sendBundle(nil, ["/d_load", metadata[\loadPath], completionMsg]);
			} {
				MethodError("Server is remote, cannot load from disk.", this).throw;
			};

	}

	// this method needs a reconsideration
	storeOnce { arg libname=\global, dir(synthDefDir), completionMsg, mdPlugin;
		var	path = dir ++ name ++ ".scsyndef", lib;
		if(File.exists(path).not) {
			this.store(libname, dir, completionMsg, mdPlugin);
		} {
				// load synthdesc from disk
				// because SynthDescLib still needs to have the info
			lib = SynthDescLib.getLib(libname);
			lib.read(path);
		};
	}

	play { arg target,args,addAction=\addToHead;
		var synth, msg;
//		this.deprecated(thisMethod, Function.findRespondingMethodFor(\play));
		target = target.asTarget;
		synth = Synth.basicNew(name,target.server);
		msg = synth.newMsg(target, args, addAction);
		this.send(target.server, msg);
		^synth
	}

}
