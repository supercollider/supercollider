

+ SynthDef {
	//  cx  Patch support
	//  func = { arg inputs; }
	/* 
		inputSpecs must be supplied : 
		[ 
			['nameOfInput',\audio,0],
			['qnty',\scalar,3],
			['ffreq',\control,440 ]
		]
	*/
	
	// newFromSpecs
	*newList { arg name, ugenGraphFunc,inputSpecs;
		^super.new.name_(name).buildList(ugenGraphFunc,inputSpecs);
	}

	buildList { arg func,inputSpecs;
		var controls;
		
		UGen.buildSynthDef = this;
		constants = Dictionary.new;
		constantSet = Set.new;
		
		controls = this.buildControlsFromInputSpecs(inputSpecs);
		func.value(controls);
		
		if (this.checkInputs.not, { ^nil }); // maybe throw an error
		this.collectConstants;
		
		// re-sort graph. reindex.
		this.topologicalSort;
		this.indexUGens;
	}

	buildControlsFromInputSpecs { arg inputSpecs;
		var controls;
		var irnames, irvalues, ircontrols, irpositions;
		var krnames, krvalues, krcontrols, krpositions;

		// OKee-dokee-doodly what we do here is separate the ir and kr rate arguments,
		// create one multi-channel Control ugen for all of the ir and one for all of 
		// the kr, and then construct the argument array from combining 
		// the OutputProxies of these two Control ugens in the original order.

		inputSpecs.do({ arg nrv,i;
			var name,rate,value;
			#name,rate,value = nrv;
			if (rate == \scalar, {
				irnames = irnames.add(name);
				irvalues = irvalues.add(value);
				irpositions = irpositions.add(i);
			},{
				// the \audio and \control inputs both come in here
				// as both are modulatable kr buss index values
				krnames = krnames.add(name);
				krvalues = krvalues.add(value);
				krpositions = krpositions.add(i);
			});
		});
		if (irnames.size > 0, {
			ircontrols = Control.names(irnames).ir(irvalues);
		});
		if (krnames.size > 0, {
			krcontrols = Control.names(krnames).kr(krvalues);
		});
		controls = Array.newClear(inputSpecs.size);
		ircontrols.asArray.do({ arg control, i; 
			controls.put(irpositions.at(i), control);
		});
		krcontrols.asArray.do({ arg control, i; 
			controls.put(krpositions.at(i), control);
		});
		
		^controls
	}
	/*
	defCode {
	
		file.putString("SCgf");
		file.putInt32(0); // file version
		file.putInt16(this.size); // number of defs in file.
		
		this.do({ arg item; item.writeDef(file); });
	}
	*/

}	
	
