
SynthDefTopologicalSort {
	classvar roots;
	classvar visited;
	classvar out;

	*new { |inRoots|
		var return;
		roots = inRoots.copy;
		visited = IdentitySet();
		out = [];
		roots.do(SynthDefTopologicalSort.prVisit(_));
		// clean up
		return = out;
		roots = nil;
		visited = nil;
		out = nil;
		^return;
	}

	*prVisit { |u|
		if (u.isKindOf(UGen) and: { visited.includes(u).not }){
			visited.add(u);
			u.sortAntecedents;
			u.weakAntecedents.do( SynthDefTopologicalSort.prVisit(_) );
			u.antecedents.do( SynthDefTopologicalSort.prVisit(_) );

			// This case is a little bit of a hack,
			//    because removing weakAntecedents that don't have descendants is costly if done elsewhere.
			if (u.hasObservableEffect or: {u.descendants.isEmpty.not}){
				out = out.add(u)
			}
		}
	}
}

/*
A helper class that lets you see what ugens occur together frequently
PRSynthDefUGenOccurrenceTracker {
	classvar roots;
	classvar visited;
	classvar out;
	classvar rollingArray;
	classvar depth;


// Pass in the roots from the synthdef and a number greater than 1
	*new { |inRoots, inDepth=2|
		var return;
		roots = inRoots.copy;
		visited = Set();
		out = Bag();
		depth = inDepth;
		rollingArray = Array();
		roots.do( PRSynthDefUGenOccurrenceTracker.prVisit(_) );
		return = out;
		roots = nil;
		visited = nil;
		out = nil;
		^return.contents;
	}
	*tally { |inRoots, inDepth|
		^PRSynthDefUGenOccurrenceTracker.new(inRoots, inDepth).asAssociations.sort({ |l, r| l.value > r.value })
	}

	*prVisit { |u|
		if (u.isKindOf(UGen) and: { visited.includes(u).not }){
			visited.add(u);
			rollingArray = rollingArray.add(u);
			if(rollingArray.size >= depth) {
				out.add(
					rollingArray[rollingArray.size - depth .. rollingArray.size]
					.collect(_.nameForDisplay).reverse
				)
			};
			u.antecedents.do( PRSynthDefUGenOccurrenceTracker.prVisit(_) );
			rollingArray.pop;
		}
	}
}
*/

SynthDefOptimisationResult {
	var <>maxDepthOfOperation = 1; // Integer greater than 0.
	var <>newUGens; // Array of UGens.
	var <>observableUGenReplacements; // An Array of Associations, or nil

	addUGen { |u, atDepth|
		newUGens = newUGens.add(u);
		maxDepthOfOperation = max(maxDepthOfOperation, atDepth);
	}

	addConstantsDescendants { |descendants|
		newUGens = (newUGens ?? {[]}).addAll(descendants.asArray);
		maxDepthOfOperation = max(maxDepthOfOperation, 0);
	}

	returnNilIfEmpty { ^newUGens !? { this } }
}

SynthDefOptimizer {
	var deduplicate = false;
	var rewrite = false;

	var finalGraph;
	var toVisit;

	// IdentityDiciontary[
	//   Symbol (current.nameForDisplay) -> Array[ugen inputs, \separator, ugen weakAntecedents] (current.inputsForIdentityCheck)
	// ]
	// Used to deduce if a UGen can be replaced by another.
	var visited;

	*new { |inRoots, deduplicate, rewrite|
		if ( (deduplicate.not) and: { rewrite.not} ){ ^inRoots.copy };
		^super.newCopyArgs(deduplicate.asBoolean, rewrite.asBoolean, inRoots.copy, inRoots.copy, IdentityDictionary(512)).prMain
	}

	prMain {
		while { toVisit.isEmpty.not } {
			this.prOptimise
		};
		^finalGraph
	}

	prDeduplicate { |current|
		if (current.canBeReplacedByIdenticalCall){
			var displayName = current.nameForDisplay;
			var found = visited[displayName];

			if(found.isNil) {
				visited[displayName] = Dictionary(32);
				visited[displayName][current.inputsForIdentityCheck] = current;
			} {
				var idIns = current.inputsForIdentityCheck;
				var possibleReplacement = found.at(idIns);
				if (possibleReplacement.isNil.not) {
					if (possibleReplacement != current) {
						// because we are replacing current with possibleReplacement, we don't need to check current's descendants.
						toVisit = toVisit.addAll(possibleReplacement.descendants);
						toVisit = toVisit.addAll(possibleReplacement.weakDescendants);

						possibleReplacement.replace(current);

						found.put(idIns, current);

						^true // stop handling the current and go to possibleReplacement's descendants.
					}
				} {
					found.put(idIns, current);
				}
			}
		};
		^false
	}

	prRewrite { |current|
		if (current.hasObservableEffect or: { current.descendants.isEmpty.not }) {
			current.optimize !? { |results| // SynthDefOptimisationResult

				if (results.newUGens.isNil.not) {
					var ref = Ref([]);

					results.observableUGenReplacements !? { |replacements|
						// The connections have already been removed, these are now disconnected UGens.
						replacements.do{ |r|
							finalGraph.remove(r.key);
							finalGraph.add(r.value);
						}
					};

					results.newUGens.do{ |u|
						if (u.isKindOf(UGen)){
							u.source.getAllDescendantsAtLevel(results.maxDepthOfOperation, ref)
						}
					};
					toVisit = toVisit.addAll(ref.get);
					^true // recur on newly added
				}
			};
		};
		^false
	}

	prOptimise {
		var current;
		// Although this might look slow, it provides large performance boosts for big graphs.
		toVisit.removeIdenticalDuplicates;
		current = toVisit.pop.source;

		if (deduplicate) {
			if (this.prDeduplicate(current)) { ^nil }
		};

		if (rewrite){
			if (this.prRewrite(current)) { ^nil }
		};

		// no opts
		toVisit = toVisit.addAll(current.antecedents);
	}
}

SynthDefOptimizations {
	var <sorting;
	var <deduplication;
	var <rewrite;

	*all { ^super.newCopyArgs(true, true, true) }
	*none { ^super.newCopyArgs(false, false, false) }
	*onlySorting { ^super.newCopyArgs(true, false, false) }
	*deduplicationAndSorting { ^super.newCopyArgs(true, true, false) }
	*sortingAndRewrite { ^super.newCopyArgs(true, false, true) }
}

SynthDef {
	classvar <synthDefDir;
	classvar <>warnAboutLargeSynthDefs = false;

	classvar <optimizations;

	var <>name;
	var <>func;

	var <>controls, <>controlNames; // ugens add to this
	var <>allControlNames;
	var <>controlIndex=0;

	// This is the final output in topological order.
	// It is also the initial order specified by adding UGens through evaluating the 'func'.
	var <>children;

	// Numbers.
	var <>constants;
	var <>constantSet;
	var <>maxLocalBufs;
	var <>variants;

	// This is nil in normal use, but true when optimisations are being ran.
	var rewriteInProgress;

	var <>desc;
	var <>metadata;

	// These are the ends of the graph, the Outs, BufWrs, et cetera.
	var effectiveUGens;
	// This stores all resource managers in an Event<Class, UGenResourceManager>.
	var resourceManagers;

	*synthDefDir_ { |dir|
		if (dir.last.isPathSeparator.not ) { dir = dir ++ thisProcess.platform.pathSeparator };
		synthDefDir = dir;
	}

	*setOptimizations { |opts|
		if (opts.isKindOf(SynthDefOptimizations).not) { Error("opts should be a SynthDefOptimizations").throw };
		optimizations = opts;
	}

	*initClass {
		synthDefDir = Platform.userAppSupportDir ++ "/synthdefs/";
		synthDefDir.mkdir;
		optimizations = SynthDefOptimizations.all;
	}

	*newForSynthDesc {
		^super.newCopyArgs(
			effectiveUGens: [],
			resourceManagers: UGenResourceManager.createNewInstances,
		)
	}

	*new { |name, ugenGraphFunc, rates, prependArgs, variants, metadata|
		^super.newCopyArgs(
			name: name.asSymbol,
			func: ugenGraphFunc,
			variants: variants,
			metadata: metadata ?? {()},
			children: Array(64),
			effectiveUGens: [],
			resourceManagers: UGenResourceManager.createNewInstances,
		).build(rates, prependArgs)
	}


	// Same as new, but optimises for a fast language side compile time, rather than fast server--side performance.
	*newFast { |name, ugenGraphFunc, rates, prependArgs, variants, metadata|
		^SynthDef.newWithOptimizations(SynthDefOptimizations.sortingAndRewrite, name, ugenGraphFunc, rates, prependArgs, variants, metadata)
	}

	*newWithOptimizations { |newOptimizations, name, ugenGraphFunc, rates, prependArgs, variants, metadata|
		var out;
		var opts = optimizations;
		if (newOptimizations.isKindOf(SynthDefOptimizations).not) { Error("newOptimizations should be a SynthDefOptimizations").throw };
		SynthDef.setOptimizations(newOptimizations);

		out = super.newCopyArgs(
			name: name,
			func: ugenGraphFunc,
			variants: variants,
			metadata: metadata ?? { () },
			children: Array(64),
			effectiveUGens: [],
			resourceManagers: UGenResourceManager.createNewInstances,
		).build(rates, prependArgs);

		SynthDef.setOptimizations(opts);

		^out;
	}

	storeArgs { ^[name, func] }

	build { |rates, prependArgs|
		protect {
			UGen.buildSynthDef = this;
			constants = Dictionary.new;
			constantSet = Set.new;
			controls = nil;
			controlIndex = 0;
			maxLocalBufs = nil;

			this.buildUgenGraph(func, rates, prependArgs);

			rewriteInProgress = true;
			children.do(_.initEdges); // Necessary because of init method in UGen child classes not returning instances of UGen.

			effectiveUGens = SynthDefOptimizer(effectiveUGens, optimizations.deduplication, optimizations.rewrite);

			if (optimizations.sorting) {
				// Does dead code elimination as a side effect.
				children = SynthDefTopologicalSort(effectiveUGens.reverse)
			};

			children.do( _.collectConstants );

			this.checkInputs; // Throws when invalid inputs are found.

			children.do { | ugen, i| ugen.synthIndex = i }; // Reindex UGens.
			children.do(_.onFinalizedSynthDef);

			// Please note, this event should NOT be used inside UGens, instead, onFinalizedSynthDef should be used.
			this.class.changed(\synthDefReady, this);
		} {
			rewriteInProgress = nil;
			UGen.buildSynthDef = nil;
		}
	}

	*wrap { arg ugenGraphFunc, rates, prependArgs;
		if (UGen.buildSynthDef.isNil) {
			"SynthDef.wrap should be called inside a SynthDef ugenGraphFunc.\n".error;
			^0
		};
		^UGen.buildSynthDef.buildUgenGraph(ugenGraphFunc, rates, prependArgs);
	}

	*writeOnce { |name, ugenGraphFunc, rates, prependArgs, variants, dir, metadata, mdPlugin|
		//only write if no file exists
		this.new(name, ugenGraphFunc, rates, prependArgs, variants, metadata).writeOnce(dir, mdPlugin)
	}

	writeOnce { |dir, mdPlugin| this.writeDefFile(dir, false, mdPlugin) }

	buildUgenGraph { |ugenGraphFunc, rates, prependArgs|
		var result;
		// save/restore controls in case of *wrap
		var saveControlNames = controlNames;

		controlNames = nil;

		prependArgs = prependArgs.asArray;
		this.addControlsFromArgsOfFunc(ugenGraphFunc, rates, prependArgs.size);

		result = ugenGraphFunc.valueArray(prependArgs ++ this.buildControls);

		controlNames = saveControlNames

		^result
	}

	addControlsFromArgsOfFunc { arg ugenGraphFunc, rates, skipArgs=0;
		var def, names, values,argNames, specs;

		def = ugenGraphFunc.def;
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

	addControlName { |cn|
		controlNames = controlNames.add(cn);
		allControlNames = allControlNames.add(cn);
	}

	// allow incremental building of controls
	addNonControl { |name, values|
		this.addControlName(ControlName(name, nil, 'noncontrol', values.copy, controlNames.size))
	}
	addIr { |name, values|
		this.addControlName(ControlName(name, controls.size, 'scalar', values.copy, controlNames.size))
	}
	addKr { |name, values, lags|
		this.addControlName(
			ControlName(name, controls.size, 'control', values.copy, controlNames.size, lags.copy))
	}
	addTr { |name, values|
		this.addControlName(
			ControlName(name, controls.size, 'trigger', values.copy, controlNames.size))
	}
	addAr { |name, values|
		this.addControlName(
			ControlName(name, controls.size, 'audio', values.copy, controlNames.size))
	}

	buildControls {
		var controlUGens, index, values, lags, valsize;
		var def, argNames;

		var arguments = Array.newClear(controlNames.size);

		var nonControlNames = controlNames.select { |cn| cn.rate == 'noncontrol' };
		var irControlNames = controlNames.select { |cn| cn.rate == 'scalar' };
		var krControlNames = controlNames.select { |cn| cn.rate == 'control' };
		var trControlNames = controlNames.select { |cn| cn.rate == 'trigger' };
		var arControlNames = controlNames.select { |cn| cn.rate == 'audio' };

		if (nonControlNames.size > 0) {
			nonControlNames.do { |cn| arguments[cn.argNum] = cn.defaultValue };
		};
		if (irControlNames.size > 0) {
			values = nil;
			irControlNames.do { |cn| values = values.add(cn.defaultValue) };
			index = controlIndex;
			controlUGens = Control.ir(values.flat).asArray.reshapeLike(values);
			irControlNames.do { |cn, i|
				cn.index = index;
				index = index + cn.defaultValue.asArray.size;
				arguments[cn.argNum] = controlUGens[i];
				this.setControlNames(controlUGens[i], cn);
			};
		};
		if (trControlNames.size > 0) {
			values = nil;
			trControlNames.do { |cn| values = values.add(cn.defaultValue) };
			index = controlIndex;
			controlUGens = TrigControl.kr(values.flat).asArray.reshapeLike(values);
			trControlNames.do { |cn, i|
				cn.index = index;
				index = index + cn.defaultValue.asArray.size;
				arguments[cn.argNum] = controlUGens[i];
				this.setControlNames(controlUGens[i], cn);
			};
		};
		if (arControlNames.size > 0) {
			values = nil;
			arControlNames.do { |cn| values = values.add(cn.defaultValue) };
			index = controlIndex;
			controlUGens = AudioControl.ar(values.flat).asArray.reshapeLike(values);
			arControlNames.do { |cn, i|
				cn.index = index;
				index = index + cn.defaultValue.asArray.size;
				arguments[cn.argNum] = controlUGens[i];
				this.setControlNames(controlUGens[i], cn);
			};
		};
		if (krControlNames.size > 0) {
			values = nil;
			lags = nil;
			krControlNames.do { |cn|
				valsize = cn.defaultValue.asArray.size;
				values = values.add(cn.defaultValue);
				lags = lags.addAll(cn.lag.asArray.wrapExtend(valsize));
			};
			index = controlIndex;
			if (lags.any { |lag| lag != 0 }) {
				controlUGens = LagControl.kr(values.flat, lags).asArray.reshapeLike(values);
			}{
				controlUGens = Control.kr(values.flat).asArray.reshapeLike(values);
			};
			krControlNames.do { |cn, i|
				cn.index = index;
				index = index + cn.defaultValue.asArray.size;
				arguments[cn.argNum] = controlUGens[i];
				this.setControlNames(controlUGens[i], cn);
			};
		};
		controlNames = controlNames.reject { |cn| cn.rate == 'noncontrol' };
		^arguments
	}

	setControlNames {arg controlUGens, cn;
		if(controlUGens.isArray) {
			controlUGens.do{ |thisCtrl| thisCtrl.name_(cn.name) }
		} {
			controlUGens.name_(cn.name)
		}
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
			MethodError("This SynthDef (%) was reconstructed from a .scsyndef file. "
				"It does not contain all the required structure to write back to disk. "
				"File was not written."
				.format(name), this).throw
		}
	}

	writeDef { arg file;
		// This describes the file format for the synthdef files.
		var allControlNamesTemp, allControlNamesMap;

		try {
			file.putPascalString(name.asString);

			this.writeConstants(file);

			//controls have been added by the Control UGens
			file.putInt32(controls.size);
			controls.do { | item |
				file.putFloat(item);
			};

			allControlNamesTemp = allControlNames.reject { |cn|
				cn.rate == \noncontrol or: { cn.name.isNil }
			};
			file.putInt32(allControlNamesTemp.size);
			allControlNamesTemp.do { | item |
				file.putPascalString(item.name.asString);
				file.putInt32(item.index);
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
		} { // catch
			arg e;
			Error("SynthDef: could not write def: %".format(e.what())).throw;
		}
	}

	writeConstants { |file|
		var array = FloatArray.newClear(constants.size);
		constants.keysValuesDo { |value, index| array[index] = value };
		file.putInt32(constants.size);
		array.do { |item| file.putFloat(item) };
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
			"SynthDef % build failed".format(this.name).postln;
			Error(firstErr).throw
		};
		^true
	}

	addUGen { |ugen|
		ugen.synthIndex = children.size;
		children = children.add(ugen);

		if (ugen.hasObservableEffect){
			effectiveUGens = effectiveUGens.add(ugen)
		};

		// It is the optimizer's responsibility to add the weak edges.
		if (rewriteInProgress.isNil) {
			// If not nil, get each active manager and add the UGen,
			//    otherwise call connectToAll on all resourceManagers, preserving ordering at the expense of a more flexible ordering.
			//ugen.resourceManagers
			//!? { |m| m.asArray.do{ |manager| resourceManagers.at(manager).add(ugen) } }
			//?? { resourceManagers.do(_.connectToAll(ugen)) };

			ugen.resourceDependencies
			!? { |arrayOfArgs| arrayOfArgs.do{ |args| resourceManagers.at(args[0]).add(ugen, *args[1..]) } }
			?? { resourceManagers.do(_.connectToAll(ugen)) }
		}
	}

	addConstant { |value|
		if (constantSet.includes(value).not) {
			constantSet.add(value);
			constants[value] = constants.size;
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
			[ugen.dumpName, ugen.rate, inputs, ugen.antecedents, ugen.descendants, ugen.weakAntecedents, ugen.weakDescendants].postln;
		};
	}

	// make SynthDef available to all servers
	add { arg libname, completionMsg, keepDef = true;
		var	servers, desc = this.asSynthDesc(libname ? \global, keepDef);
		if(libname.isNil) {
			servers = Server.allBootedServers
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

	// only send to servers
	send { arg server, completionMsg;
		var servers = (server ?? { Server.allBootedServers }).asArray;
		servers.do { |each|
			if(each.hasBooted.not) {
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
		var path;
		var resp, syncID;

		if (bytes.size < (65535 div: 4)) {
			server.sendMsg("/d_recv", bytes, completionMsg)
		} {
			if (server.isLocal) {
				if(warnAboutLargeSynthDefs) {
					"SynthDef % too big for sending. Retrying via synthdef file".format(name).warn;
				};
				this.writeDefFile(synthDefDir);
				path = synthDefDir +/+ name ++ ".scsyndef";
				syncID = UniqueID.next;
				resp = OSCFunc({
					resp.remove;
					File.delete(path);
				}, '/synced', srcID: server.addr, argTemplate: [syncID]);
				server.sendBundle(nil,
					["/d_load", path, completionMsg],
					["/sync", syncID]
				);
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
				desc = lib[this.name];
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
		desc = lib.readDescFromDef(stream, keepDef, this, metadata);
		^desc
	}

	findSpecFor { |controlName|
		var specs = metadata[\specs];
		^specs !? { specs[controlName] }
	}

	specs {
		if(metadata[\specs].isNil) { metadata[\specs] = () };
		^metadata[\specs]
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

	play { |target, args, addAction = \addToHead|
		var synth, msg;
		target = target.asTarget;
		synth = Synth.basicNew(name,target.server);
		msg = synth.newMsg(target, args, addAction);
		this.send(target.server, msg);
		^synth
	}
}
