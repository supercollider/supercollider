

+ Patch {
}

+ Object {
	
	addToSynthDef {  arg synthDef,name,argi;
		synthDef.addFixed(name,this.synthArg,argi); // has to be an InstrSynthDef
	}
	synthArg { ^this }
	instrArgRate { ^\scalar }
	instrArgFromControl { arg control;
		^this//.insp("Object returns self as instrArg")
	}
}

+ NumberEditor {
	
	addToSynthDef {  arg synthDef,name,argi;
		synthDef.addKr(name,this.synthArg,argi);
	}
	synthArg { ^this.poll }
	instrArgRate { ^\control }
	instrArgFromControl { arg control;
		//control.insp("instrArgFromControl");
		^control
	}	
}

+ TempoBus { // all Busses ?
	addToSynthDef {  arg synthDef,name,argi;
		synthDef.addIr(name,this.synthArg,argi);
	}
	synthArg { ^this.index }
	instrArgRate { ^\control }
	instrArgFromControl { arg control;
		^control
	}	
}

+ AbstractPlayer {

	addToSynthDef {  arg synthDef,name,argi;
		synthDef.addKr(name,this.synthArg,argi); // \out is a .kr bus index
	}

	synthArg { ^patchOut.synthArg }
	instrArgRate { ^\audio }
	instrArgFromControl { arg control;
		^In.ar(control,this.numChannels)
	}	

}

+ KrPlayer {

	addToSynthDef {  arg synthDef,name,argi;
		synthDef.addKr(name,this.synthArg,argi);
	}
	instrArgRate { ^\control }
	instrArgFromControl { arg control;
		^In.kr(control,this.numChannels)
	}	

}

+ Sample {

	instrArgFromControl { arg control,name;
		forArgName = name; // for buffer etc. to build a control
		^this
	}
}

// trig things, supply a InTrig.kr







/*

these are all the crap if statements 
that were cut out by InstrSynthDef


+ Instr {
	//  fixedArgs, irArgs
	asSynthDef { arg fixedArgs,outClass = \Out,xfader,defName;
		//xfader is only if using XOut
		var isScalarOut,nonScalarIndices,controlIndices,ci=0;
		var fixedNames="",inputSpecs;
		
		// must supply an output spec if you are doing scalar
		// otherwise we can determine the spec from the result.
		// eg. SendTrig which returns 0.0
		// other scalars can't be returned in a SynthDef
		isScalarOut = outSpec.notNil and: {outSpec.rate == \scalar};
		if(isScalarOut,{ ci = -1; }); // first arg is NOT \out
		
		outClass = outClass.asClass;
		if(fixedArgs.isNil, { fixedArgs = [] });
		controlIndices = 
				this.specs.collect({ arg spec,i;
					if(spec.rate == \scalar or: {fixedArgs.at(i).notNil},
						nil
					,{
						nonScalarIndices = nonScalarIndices.add(i);
						ci = ci + 1;
						ci
					})
				});
			
		if(defName.isNil,{ // better to let the Patch specify it, 
						// its faster, but no reuseability of synthDef files
			defName = "";
			name.do({ arg part;
				defName = defName ++ part.asString.asFileSafeString;
			});
			defName = defName ++ outClass.name.asString.first.toLower;
			fixedArgs.do({ arg fa,i;
				if(fa.notNil,{
					fixedNames = fixedNames ++ i ++ fa.asCompileString;
				})
			});
			defName = defName ++ fixedNames.hash.asFileSafeString;
		});
		
		if(isScalarOut.not,{
			inputSpecs = 	[[\out,\control,0]]; // first arg is always \out buss index
		},{
			inputSpecs = [];
		});
		inputSpecs = inputSpecs ++
				nonScalarIndices.collect({ arg agi;
					[this.argNameAt(agi),
					specs.at(agi).rate,
					/*  SEEMS RIGHT TO USE THE FIXED ARG FOR THE VALUE */
					fixedArgs.at(agi) ?? {this.defArgAt(agi)}];
				});

		^SynthDef.newFromSpecs(defName,{ arg inputs;
			var outIndex,funcArgs,out,anOutChannel;
			// only gets inputs matching Controls
			
			//fixedArgs.insp("fixedArgs to Instr");
			//inputs.insp("inputs to Instr");
			
			funcArgs = this.specs.collect({ arg spec,i;
				if(spec.rate == \audio,{
					fixedArgs.at(i) ?? 
						{
							In.ar(inputs.at(controlIndices.at(i)), 
								spec.numChannels)
						} 
					// not possible to fix an audio anyway
				},{
					if(spec.rate == \scalar,{
						fixedArgs.at(i) ?? {this.defArgAt(i)}
					},{// control or mixed rate (nil)
						fixedArgs.at(i) ?? {									if(spec.isKindOf(TrigSpec),{ 
								// create a trig, responds to /c_set touching
								InTrig.kr(inputs.at(controlIndices.at(i)))
							},{
								In.kr(inputs.at(controlIndices.at(i)),1 )
								// assumes 1 channel kr for now
							})
						} 
					})
				});
			});
			out = this.func.valueArray(funcArgs);
			// lets guess, look at what the ugenFunc returned
			if(outSpec.isNil,{
				anOutChannel = if(out.isSequenceableCollection,
									{out.first},{out});
				if(anOutChannel.rate == \audio,{
					outSpec = AudioSpec(if(out.isSequenceableCollection,{out.size},1));
					// MultiTrackAudioSpec must be explicitly stated
					// in the Instr def
				},{
					if(anOutChannel.rate == \control,{
						// \bipolar, \unipolar etc.
						outSpec = anOutChannel.signalRange.asSpec;
						// TrigSpec must be explictly stated
						// in the Instr def
					},{ // or scalar
						if(anOutChannel.isFloat,{
							// SendTrig etc.:  no Out
							outSpec =  ScalarSpec.new;//or StaticSpec
						} , { 				
							die("can't handle this scalar output from SynthDef:"
							+ out + "@0: " + anOutChannel);
						});
					})
				})
			});
			if(isScalarOut.not,{ // wrap it in an Out
				if(outClass !== XOut,{
					out = outClass.perform(if(this.rate == \audio,\ar,\kr),
								inputs.at(0),out)
				},{
					out = outClass.perform(if(this.rate == \audio,\ar,\kr),
								inputs.at(0),xfader.value,out)
				});
			});
			out
		},inputSpecs);
	}
}

*/

/*

+ SynthDef {

	*newFromSpecs { arg name, ugenGraphFunc,inputSpecs;
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
			[name,rate,value].insp("nrv from the inputSpecs");
			
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
}	


*/

