
InstrSynthDef : SynthDef {

	var fixedNames,fixedValues,fixedPositions;

	// secret because the function doesn't see them
	// but they are needed to pass into the synth
	var secretIrPairs, secretKrPairs;
	
	var <rate,<numChannels; //after build

	*new { // arg name,rate,numChannels,secretIrPairs,secretKrPairs,bytes;
		^super.prNew
	}
	*build { arg instr,args,outClass = \Out;
		^super.prNew.build(instr,args,outClass)
	}
	build { arg instr,args,outClass= \Out;
		this.initBuild
			.buildUgenGraph(instr,args ? #[],outClass)
			.finishBuild
	}
	buildUgenGraph { arg instr,args,outClass;
		var result,fixedID="";
		var isScalarOut;
		var outputProxies;
		
		// need to know the output type now
		
		// OutputProxy In InTrig Float etc.		
		outputProxies = this.buildControlsWithObjects(instr,args);
							
		/*
			not all of the controls are Controls,
			some are what the object chose to add
			in addToSynthDef
		*/

		result = instr.valueArray(outputProxies);
		rate = result.rate;
		numChannels = max(1,result.size);
		
		if(result != 0.0,{
			outClass = outClass.asClass;
			if(outClass === XOut,{
				"XOut not tested yet.".error;
				//out = outClass.perform(if(this.rate == \audio,\ar,\kr),
				//			inputs.at(0),xfader.value,out)
			});
				
			if(rate == \audio,{
				this.addKr(\outIndex,0);
				result = outClass.ar(Control.names([\outIndex]).kr([0]) , result);
				// can still add Out controls if you always use \out, not index
			},{
				if(rate == \control,{
					this.addKr(\outIndex,0);
					result = outClass.kr(Control.names([\outIndex]).kr([0]) , result);
				},{
					("InstrSynthDef: scalar rate ? result of your function:" + result).error;
				})
			});
		});


		//is based on the instr name, ir, kr pattern, fixedValues
		// outClass,numChannels (in case it expanded)
		name = "";
		instr.name.do({ arg part;
				name = name ++ part.asString.asFileSafeString;
		});
		if(name.size > 10,{
			name = name.copyRange(0,9) ++ name.copyRange(10,name.size - 1).hash.asFileSafeString;
		});
		name = name ++ outClass.name.asString.first.toUpper;
		// TODO: ir, kr pattern
		fixedValues.do({ arg fa,i;
			if(fa.notNil,{
				fixedID = fixedID ++ i ++ fa.asCompileString;
			})
		});
		name = name ++ fixedID.hash.asFileSafeString;
		("InstrSynthDef built:" + name ).postln;
	}
	
	// passed to Instr function but not to synth
	addInstrOnlyArg { arg name,value;
		fixedNames = fixedNames.add(name);
		fixedValues = fixedValues.add(value);
		fixedPositions = fixedPositions.add(controlsSize);
		controlsSize = controlsSize + 1;
	}
	
	// to cache this def, this info needs to be saved
	// argi points to the slot in objects (as supplied to secretDefArgs)
	// selector will be called on that object to produce the synthArg
	// thus sample can indicate itself and be asked for \tempo or \bufnum
	addSecretIr { arg name,value,argi,selector;
		secretIrPairs = secretIrPairs.add([name,value,argi,selector]);
		^Control.names([name]).ir([value])
	}
	addSecretKr { arg name,value,argi,selector;
		secretKrPairs = secretKrPairs.add([name,value,argi,selector]);
		^Control.names([name]).kr([value])
	}
	secretDefArgs { arg objects;
		var synthArgs,size;
		size = secretIrPairs.size * 2 + (secretKrPairs.size * 2);
		if(size == 0, { ^#[] });
		synthArgs = Array(size);
		secretIrPairs.do({ arg n,i;
			synthArgs.add(n.at(0)); // secret arg name
			synthArgs.add(objects.at(n.at(2)).perform(n.at(3)));
		});
		secretKrPairs.do({ arg n,i;
			synthArgs.add(n.at(0));
			synthArgs.add(objects.at(n.at(2)).perform(n.at(3)));
		});
		^synthArgs
	}

	buildControlsWithObjects { arg instr,objects;
		var argNames,defargs,outputProxies;
		objects.do({ arg obj,argi; obj.initForSynthDef(this,argi) });
		defargs = (argNames = instr.argNames).collect({ arg name,defargi;
			var defarg;
			defarg = (objects.at(defargi) ? {instr.defArgAt(defargi)});
			defarg.addToSynthDef(this,name);
			defarg
		});
		outputProxies = this.buildControls;//.insp("buildControls");
		// wrap them in In.kr etc. if needed
		^outputProxies.collect({ arg outp,i;
			defargs.at(i).instrArgFromControl(outp,i)
		})
	}
	
	
//	tempoKr {
//		// have to include in cacheing, playing
//		tempoKr ?? {
//			this.addSecretKr('__tempo__',Tempo.tempo, ... )
//		}
//	}
}

