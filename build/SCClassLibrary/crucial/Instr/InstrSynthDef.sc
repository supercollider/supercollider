
InstrSynthDef : SynthDef {

	var fixedNames,fixedValues,fixedPositions;
	
	*new {
		^super.prNew
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
			some are what the object chose to return
			from addToSynthDef
		*/

		result = instr.valueArray(outputProxies);
		
		if(result != 0.0,{
			outClass = outClass.asClass;
			
			if(outClass === XOut,{
				"XOut not tested yet.".error;
				//out = outClass.perform(if(this.rate == \audio,\ar,\kr),
				//			inputs.at(0),xfader.value,out)
			});
				
			if(result.rate == \audio,{
				this.addKr(\outIndex,0);
				result = outClass.ar(Control.names([\outIndex]).kr([0]) , result);
				// can still add Out controls if you always use \out, not index
			},{
				if(result.rate == \control,{
					this.addKr(\outIndex,0);
					result = outClass.kr(Control.names([\outIndex]).kr([0]) , result);
				},{
					("what is this ?" + result).error;
				})
			});
		});


		//is based on the instr name, ir, kr pattern, fixedValues
		// outClass,numChannels (in case it expanded)
		name = "";
		instr.name.do({ arg part;
				name = name ++ part.asString.asFileSafeString;
		});
		name = name ++ outClass.name.asString.first.toUpper;
		// ir, kr
		fixedValues.do({ arg fa,i;
			if(fa.notNil,{
				fixedID = fixedID ++ i ++ fa.asCompileString;
			})
		});
		name = name ++ fixedID.hash.asFileSafeString;
		("InstrSynthDef made:" + name ).postln;
	}
	
	// fixed arg, NOT addConstant (which is what the UGen will do)
	addFixed { arg name,value;
		fixedNames = fixedNames.add(name);
		fixedValues = fixedValues.add(value);
		fixedPositions = fixedPositions.add(controlsSize);
		controlsSize = controlsSize + 1;
	}

	buildControlsWithObjects { arg instr,objects;
		var defargs,outputProxies;
		defargs = instr.argNames.collect({ arg name,defargi;
			var defarg;
			defarg = (objects.at(defargi) ? {instr.defArgAt(defargi)});
			defarg.addToSynthDef(this,name);
			defarg
		});
		outputProxies = this.buildControls;
		// wrap them in In.kr etc. if needed
		^outputProxies.collect({ arg outp,i;
			defargs.at(i).instrArgFromControl(outp)
		})
	}
	
}

