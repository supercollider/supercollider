
SynthDef {	
	var <>name;
	
	var controlsSize=0;
	var irnames, irvalues, ircontrols, irpositions;
	var krnames, krvalues, krcontrols, krpositions, krlags;

	var <>controls,<>controlNames; // ugens add to this
	var <>children;
		
	var <>constants;
	var <>constantSet;
	
	// topo sort
	var <>available;
	
	*new { arg name, ugenGraphFunc, lags, prependArgs;
		^this.prNew(name)
			.build(ugenGraphFunc, lags, prependArgs)
	}
	*prNew { arg name;
		^super.new.name_(name.asString)
	}
	build { arg ugenGraphFunc, lags, prependArgs;
		this.initBuild;
		this.buildUgenGraph(ugenGraphFunc, lags, prependArgs);
		this.finishBuild;
	}
	
	*wrap { arg func, lags, prependArgs;
		if (UGen.buildSynthDef.isNil, { 
			"SynthDef.wrap should be called inside a SynthDef ugenGraphFunc.\n".error; 
			^0 
		});
		^UGen.buildSynthDef.buildUgenGraph(func, lags, prependArgs);
	}
	//use this for system synth defs
	*writeOnce { arg name, func, lags, prependArgs, dir="synthdefs/";
		^pathMatch(dir ++ name ++ ".scsyndef").isEmpty.if({
			this.new(name, func, lags, prependArgs).writeDefFile(dir)
		}, nil);
	}
	
	initBuild {
		UGen.buildSynthDef = this;
		constants = Dictionary.new;
		constantSet = Set.new;
	}
	buildUgenGraph { arg func, lags, prependArgs;
		// clean up.
		irnames = irvalues = ircontrols = irpositions = nil;
		krnames = krvalues = krcontrols = krpositions = krlags = nil;

		prependArgs = prependArgs.asArray;
		this.addControlsFromArgsOfFunc(func, lags, prependArgs.size);
		^func.valueArray(prependArgs.asArray ++ this.buildControls);
	}
	addControlsFromArgsOfFunc { arg func, lags, skipArgs=0;
		var def, names, values,argNames;
		
		def = func.def;
		argNames = def.argNames;
		if(argNames.isNil,{ ^nil }); 
		names = def.argNames.copyToEnd(skipArgs);
		controlsSize = 0;
		// OK what we do here is separate the ir and kr rate arguments,
		// create one Control ugen for all of the ir and one for all of 
		// the kr, and then construct the argument array from combining 
		// the OutputProxies of these two Control ugens in the original order.
		values = def.prototypeFrame.copyToEnd(skipArgs).extend(names.size);
		values = values.collect({ arg value; value ? 0.0 });
		lags = lags.asArray.extend(names.size, 0).collect({ arg lag; lag ? 0.0 });
		names.do({ arg name, i; 
			var c, c2, value, lag;
			#c, c2 = name.asString;
			value = values.at(i);
			lag = lags.at(i);
			if (c == $i && { c2 == $_ }, {
				if (lag != 0, {
					Post << "WARNING: lag value "<< lag <<" for i-rate arg '"
						<< name <<"' will be ignored.\n";
				});
				this.addIr(name, value);
			},{
				this.addKr(name, value, lag);
			});
		});
	}
	
	// allow incremental building of controls
	addIr { arg name, value;
		irnames = irnames.add(name);
		irvalues = irvalues.add(value);
		irpositions = irpositions.add(controlsSize);
		controlsSize = controlsSize + 1;
	}
	addKr { arg name, value, lag=0;
		krnames = krnames.add(name);
		krvalues = krvalues.add(value);
		krpositions = krpositions.add(controlsSize);
		krlags = krlags.add(lag);
		controlsSize = controlsSize + 1;
	}
	buildControls {
		var outputProxies;
		// the Controls add themselves to my controls
		if (irnames.size > 0, {
			ircontrols = Control.names(irnames).ir(irvalues);
		});
		if (krnames.size > 0, {
			if (krlags.any({ arg lag; lag != 0 }), {
				krcontrols = LagControl.names(krnames).kr(krvalues, krlags);
			},{
				krcontrols = Control.names(krnames).kr(krvalues);
			});
		});
		outputProxies = Array.newClear(controlsSize);
		ircontrols.asArray.do({ arg control, i; 
			outputProxies.put(irpositions.at(i), control);
		});
		krcontrols.asArray.do({ arg control, i; 
			outputProxies.put(krpositions.at(i), control);
		});
				
		^outputProxies
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
		var stream;
		stream = CollStream.on(Int8Array.new(256));
		this.asArray.writeDef(stream);
		^stream.collection;
	}
	writeDefFile { arg dir="synthdefs/";
		super.writeDefFile(name,dir);
	}
	writeDef { arg file;
		// This describes the file format for the synthdef files.
		
		file.putPascalString(name);
		
		this.writeConstants(file);

		//controls have been added by the Control UGens
		file.putInt16(controls.size);
		controls.do({ arg item;
			file.putFloat(item);
		});
				
		file.putInt16(controlNames.size);
		controlNames.do({ arg item;
			if (item.name.notNil, {
				file.putPascalString(item.name);
				file.putInt16(item.index);
			});
		});

		file.putInt16(children.size);
		children.do({ arg item;
			item.writeDef(file);
		});
	}
	writeConstants { arg file;
		var array;
		array = Array.newClear(constants.size);
		constants.keysValuesDo({ arg value, index;
			array.put(index, value);
		});

		file.putInt16(constants.size);
		array.do({ arg item; 
			file.putFloat(item) 
		});
	}
	
	checkInputs {
		var seenErr = false;
		children.do({ arg ugen;
			var err;
			if ((err = ugen.checkInputs).notNil, { 
				seenErr = true;
				(ugen.class.asString + err).postln;
			});
		});
		if(seenErr,{ ("SynthDef" + this.name + "build failed").die });
		^true
	}


	
	// UGens do these
	addUGen { arg ugen;
		ugen.synthIndex = children.size;
		children = children.add(ugen);
	}
	removeUGen { arg ugen;
		children.remove(ugen);
		this.indexUGens;
	}
	replaceUGen { arg a, b;
		children.remove(b);
		children.do({ arg item, i;
			if (item === a, { 
				children.put(i, b) 
			});
			item.inputs.do({ arg input, j;
				if (input === a, { item.inputs.put(j, b) });
			});
		});
	}
	addConstant { arg value;
		if (constantSet.includes(value).not, {
			constantSet.add(value);
			constants.put(value, constants.size);
		});
	}

	
	// multi channel expansion causes a non optimal breadth-wise ordering of the graph.
	// the topological sort below follows branches in a depth first order,
	// so that cache performance of connection buffers is optimized.
	
	optimizeGraph {
		this.initTopoSort;
		children.copy.do({ arg ugen;
			ugen.optimizeGraph;
		});
	}
	collectConstants {
		children.do({ arg ugen; 
			ugen.collectConstants;
		});
	}
	
	initTopoSort {
		available = nil;
		children.do({ arg ugen;
			ugen.antecedents = Set.new;
			ugen.descendants = Set.new;
		});
		children.do({ arg ugen;
			// this populates the descendants and antecedents
			ugen.initTopoSort;
		});
		children.reverseDo({ arg ugen;
			ugen.descendants = ugen.descendants.asSortedList(
								{ arg a, b; a.synthIndex < b.synthIndex }
							);
			ugen.makeAvailable; // all ugens with no antecedents are made available
		});
	}
	topologicalSort {
		var outStack;
		this.initTopoSort;
		while ({ available.size > 0 },{
			outStack = available.pop.schedule(outStack);
		});
		
		children = outStack;
	}
	indexUGens {
		children.do({ arg ugen, i;
			ugen.synthIndex = i;
		});
	}
	
	dumpUGens {
		name.postln;
		children.do({ arg ugen, i;
			var inputs;
			if (ugen.inputs.notNil, {
				inputs = ugen.inputs.collect({ arg in; 
					if (in.isKindOf(UGen), { in.source },{ in })
				});
			});
			[i, ugen, ugen.rate, ugen.synthIndex, inputs].postln;
		});
	}
	
	
	send { arg server,completionMsg;
		server.listSendBundle(nil,[["/d_recv", this.asBytes,completionMsg]]);
		//server.sendMsg("/d_recv", this.asBytes,completionMsg);
	}
	load { arg server, completionMsg,dir="synthdefs/";
		// i should remember what dir i was written to
		this.writeDefFile;
		server.listSendMsg(
			["/d_load", dir ++ name ++ ".scsyndef", completionMsg ]
		)
	}
	play { arg target,args,addAction=\addToTail;
		var synth, msg;
		target = target.asTarget;
		synth = Synth.prNew(name,target.server);
		msg = synth.newMsg(target, addAction, args);
		this.send(target.server, msg);
		^synth
	}
}

