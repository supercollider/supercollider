
InstrSynthDef : SynthDef {
	
	classvar watchedServers;
	
	var <longName;
	
	// secret because the function doesn't see them
	// but they are needed to pass into the synth
	var secretIrPairs, secretKrPairs;
	
	var <>tempTempoKr;
	
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

		// restart controls in case of *wrap
		controlNames = nil;
		controls = nil;
		
		// OutputProxy In InTrig Float etc.		
		outputProxies = this.buildControlsWithObjects(instr,args);

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
				result = outClass.ar(Control.names([\out]).ir([0]) , result);
				// can still add Out controls if you always use \out, not index
			},{
				if(rate == \control,{
					result = outClass.kr(Control.names([\out]).ir([0]) , result);
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
		if(name.size > 8,{
			name = name.copyRange(0,7) ++ name.copyRange(8,name.size - 1).hash.asFileSafeString;
		});
		name = name ++ outClass.name.asString.first.toUpper;

		/* fixedValues.do({ arg fa,i;
			if(fa.notNil,{
				fixedID = fixedID ++ i ++ fa.asCompileString;
			})
		});
		name = name ++ fixedID.hash.asFileSafeString; */

		longName = name ++ this.class.defNameFromObjects(args);
		name = longName.hash.asFileSafeString;
		("InstrSynthDef built:" + name + longName).inform;
	}
	
	// passed to Instr function but not to synth
	addInstrOnlyArg { arg name,value;
		/*fixedNames = fixedNames.add(name);
		fixedValues = fixedValues.add(value);
		fixedPositions = fixedPositions.add(controls.size);*/
		this.addNonControl(name, value);
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
			synthArgs.add(objects.at(n.at(2)).perform(n.at(3))); // value
		});
		secretKrPairs.do({ arg n,i;
			synthArgs.add(n.at(0)); // name
			synthArgs.add(objects.at(n.at(2)).perform(n.at(3))); // initial value
		});
		^synthArgs
	}

	buildControlsWithObjects { arg instr,objects;
		var argNames,defargs,outputProxies;
		objects.do({ arg obj,argi; obj.initForSynthDef(this,argi) });
		defargs = (argNames = instr.argNames).collect({ arg name,defargi;
			var defarg;
			defarg = (objects.at(defargi) ?? {instr.defArgAt(defargi)});
			defarg.addToSynthDef(this,name);
			defarg
		});
		outputProxies = this.buildControls;
		// wrap them in In.kr etc. if needed
		^outputProxies.collect({ arg outp,i;
			defargs.at(i).instrArgFromControl(outp,i)
		})
	}
	
	tempoKr { arg argi,selector;
		^(tempTempoKr ?? { // first client to request will later be asked to produce the TempoBus
			tempTempoKr = In.kr(
				this.addSecretKr(("__tempo__" ++ argi.asString).asSymbol,1.0,argi,selector),
			1)
		})
	}
	
	*defNameFromObjects { arg objects;
		^String.streamContents({ arg stream;
			var iks=0;
			objects.do({ arg obj,argi;
				// returns 0/i 1/k 2/s tag, adds values if any to stream
				iks = (3 ** argi) *  obj.addToDefName(stream) + iks;			});
			stream << iks;
		});
	}
	*initClass { watchedServers = IdentityDictionary.new }
	*watchServer { arg server;
		if(watchedServers.at(server).isNil,{
			SimpleController(server)
				.put(\serverRunning,{ //clear on quit
					if(server.serverRunning.not,{
						AppClock.sched(3.0,{ // don't panic too quickly
							if(server.serverRunning.not,{ // okay, she's dead
								this.clearCache(server);
								this.loadCacheFromDir(server);
							})
						});
					});
				});
			watchedServers.put(server,Main.elapsedTime);
		});
	}
	*clearCache { arg server;
		"Clearing AbstractPlayer SynthDef cache".inform;
		Library.put(SynthDef,server,nil);		
	}
	*loadCacheFromDir { arg server,dir;
		dir = dir ? SynthDef.synthDefDir;
		(dir++"*").pathMatch.do({ arg p;
			var defName;
			defName = PathName(p).fileNameWithoutExtension;
			Library.put(SynthDef,server,defName.asSymbol,\assumedLoaded);
		})
	}
}
