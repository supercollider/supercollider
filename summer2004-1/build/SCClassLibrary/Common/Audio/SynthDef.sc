
SynthDef {	
	var <>name;
	
	var <>controls, <>controlNames; // ugens add to this
	var <>allControlNames;
	var <>controlIndex=0;
	var <>children;
		
	var <>constants;
	var <>constantSet;
	
	// topo sort
	var <>available;
	
	
	*new { arg name, ugenGraphFunc, rates, prependArgs;
		^this.prNew(name)
			.build(ugenGraphFunc, rates, prependArgs)
	}
	*prNew { arg name;
		^super.new.name_(name.asString)
	}
	build { arg ugenGraphFunc, rates, prependArgs;
		this.initBuild;
		this.buildUgenGraph(ugenGraphFunc, rates, prependArgs);
		this.finishBuild;
	}
	
	*wrap { arg func, rates, prependArgs;
		if (UGen.buildSynthDef.isNil, { 
			"SynthDef.wrap should be called inside a SynthDef ugenGraphFunc.\n".error; 
			^0 
		});
		^UGen.buildSynthDef.buildUgenGraph(func, rates, prependArgs);
	}
	//only write if no file exists
	*writeOnce { arg name, func, rates, prependArgs, dir="synthdefs/";
		^pathMatch(dir ++ name ++ ".scsyndef").isEmpty.if({
			this.new(name, func, rates, prependArgs).writeDefFile(dir)
		}, nil);
	}
	
	
	initBuild {
		UGen.buildSynthDef = this;
		constants = Dictionary.new;
		constantSet = Set.new;
		controls = nil;
		controlIndex = 0;
	}
	buildUgenGraph { arg func, rates, prependArgs;
		var result, saveControlNames;
		
		// save/restore controls in case of *wrap
		saveControlNames = controlNames;
		controlNames = nil;
		
		prependArgs = prependArgs.asArray;
		this.addControlsFromArgsOfFunc(func, rates, prependArgs.size);
		result = func.valueArray(prependArgs ++ this.buildControls);
		
		controlNames = saveControlNames
		
		^result

	}
	addControlsFromArgsOfFunc { arg func, rates, skipArgs=0;
		var def, names, values,argNames;
		
		def = func.def;
		argNames = def.argNames;
		if(argNames.isNil,{ ^nil }); 
		names = def.argNames[skipArgs..];
		// OK what we do here is separate the ir, tr and kr rate arguments,
		// create one Control ugen for all of each rate, 
		// and then construct the argument array from combining 
		// the OutputProxies of these two Control ugens in the original order.
		values = def.prototypeFrame[skipArgs..].extend( names.size );
		values = values.collect {|value| value ? 0.0 };
		rates = rates.asArray.extend(names.size, 0).collect {|lag| lag ? 0.0 };
		names.do({ arg name, i; 
			var c, c2, value, lag;
			#c, c2 = name.asString;
			value = values[i];
			lag = rates[i];
			if ((lag == \ir) or: { c == $i and: { c2 == $_ }}, {
				if (lag.isNumber and: { lag != 0 }, {
					Post << "WARNING: lag value "<< lag <<" for i-rate arg '"
						<< name <<"' will be ignored.\n";
				});
				this.addIr(name, value);
			},{
			if ((lag == \tr) or: { c == $t and: { c2 == $_ }}, {
				if (lag.isNumber and: { lag != 0 }, {
					Post << "WARNING: lag value "<< lag <<" for trigger arg '"
						<< name <<"' will be ignored.\n";
				});
				this.addTr(name, value);
			},{
				this.addKr(name, value, lag);
			})});
		});
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
	buildControls {
		var arguments;
		var nonControlNames, irControlNames, krControlNames, trControlNames;
		var controlUGens, index, values, lags, valsize;
		var def, argNames;
		
		arguments = Array.newClear(controlNames.size);
		
		nonControlNames = controlNames.select {|cn| cn.rate == 'noncontrol' };
		irControlNames = controlNames.select {|cn| cn.rate == 'scalar' };
		krControlNames = controlNames.select {|cn| cn.rate == 'control' };
		trControlNames = controlNames.select {|cn| cn.rate == 'trigger' };
		
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
			};
		};
		controlNames = controlNames.reject {|cn| cn.rate == 'noncontrol' };
		
		^arguments
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
				
		file.putInt16(allControlNames.size);
		allControlNames.do({ arg item;
			if (item.name.notNil, {
				file.putPascalString(item.name.asString);
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
		array = FloatArray.newClear(constants.size);
		constants.keysValuesDo({ arg value, index;
			array[index] = value;
		});

		file.putInt16(constants.size);
		array.do({ arg item; 
			file.putFloat(item) 
		});
	}
	
	checkInputs {
		var seenErr = false;
		children.do { arg ugen;
			var err;
			if ((err = ugen.checkInputs).notNil) { 
				seenErr = true;
				(ugen.class.asString + err).postln;
			};
		};
		if(seenErr) { Error("SynthDef" + this.name + "build failed").throw };
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
			if (item === a and: { b.isKindOf(UGen) }, { 
				children[i] = b; 
			});
			item.inputs.do({ arg input, j;
				if (input === a, { item.inputs[j] = b });
			});
		});
	}
	addConstant { arg value;
		if (constantSet.includes(value).not, {
			constantSet.add(value);
			constants[value] = constants.size;
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
			ugen.descendants = ugen.descendants.asArray.sort(
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
	
	
	send { arg server,completionMsg;
		server.listSendBundle(nil,[["/d_recv", this.asBytes,completionMsg]]);
		//server.sendMsg("/d_recv", this.asBytes,completionMsg);
	}
	load { arg server, completionMsg,dir="synthdefs/";
		// i should remember what dir i was written to
		var path;
		this.writeDefFile(dir);
		server.listSendMsg(
			["/d_load", dir ++ name ++ ".scsyndef", completionMsg ]
		)
	}
	store { arg libname=\global, path="synthdefs/", completionMsg;
		var lib, bytes, file;
		lib = SynthDescLib.all[libname];
		if(lib.isNil) { Error("library" + libname  + "not found").throw };
		path = path ++ name ++ ".scsyndef";
		file = File(path, "w");
		protect {
			bytes = this.asBytes;
			file.putAll(bytes);
			file.close;
			lib.read(path);
			lib.servers.do { arg server;
				server.sendBundle(nil, ["/d_recv", bytes] ++ completionMsg)
			};
		} { 
			file.close 
		}
	}

	play { arg target,args,addAction=\addToHead;
		var synth, msg;
		target = target.asTarget;
		synth = Synth.basicNew(name,target.server);
		msg = synth.newMsg(target, addAction, args);
		this.send(target.server, msg);
		^synth
	}
}

