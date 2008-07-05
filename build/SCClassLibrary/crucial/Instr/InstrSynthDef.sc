
InstrSynthDef : SynthDef {
	
	var <longName;
	var instr;
	var inputs; // the inputs supplied the last time this was built
	var outputProxies; // the output proxies those inputs created, primarily used for error reporting
	
	// secret because the function doesn't have them as arg names
	// they are created in the context of the synth def and added
	// magically/secretly.  backdoor access.
	// this is for adding buffers, samples, inline players, subpatches, spawners etc.
	var secretIr, secretKr,stepchildren,synthProxy;
	
	var <>tempTempoKr;
	
	var <rate,<numChannels; // known only after building

	*new {
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
	buildUgenGraph { arg argInstr,args,outClass;
		var result,fixedID="",firstName;
		var isScalarOut;

		outClass = outClass.asClass;
		
		instr = argInstr;
		inputs = args;
		
		// restart controls in case of *wrap
		controlNames = nil;
		controls = nil;
		secretIr = nil;
		secretKr = nil;
		stepchildren = nil;

		{
			// create OutputProxy In InTrig Float etc.
			outputProxies = this.buildControlsWithObjects(instr,inputs);
			result = instr.valueArray(outputProxies);
	
			if(result != 0.0,{
				rate = result.rate;
				numChannels = max(1,result.size);
		
				if(outClass === XOut,{
					"XOut not tested yet.".error;
					//out = outClass.perform(if(this.rate == \audio,\ar,\kr),
					//			inputs.at(0),xfader.value,out)
				});
				
				rate.switch(
					\audio, {
						result = outClass.ar(Control.names([\out]).ir([0]) , result);
						// can still add Out controls if you always use \out, not index
					},
					\control, {
						result = outClass.kr(Control.names([\out]).ir([0]) , result);
					},
					\scalar, { // doesn't make sense for a UGen
						("InstrSynthDef: result of your Instr function was a scalar rate object:" 
							+ result + this.buildErrorString).error;
					},
					\noncontrol,{
					
						("InstrSynthDef: result of your Instr function was a noncontrol rate object:" 
							+ result + this.buildErrorString).error;
					},						
					{
						("InstrSynthDef: result of your Instr function was an object with unknown rate:" 
							+ result + rate + this.buildErrorString).error;
					}
				);
			});
		}.try({ arg err;
			var info;
			err.errorString.postln;
			info = this.buildErrorString;
			info.postln;
			//this.dumpUGens;
			// since we failed while loading synth defs
			// any other previously succesful builds will
			// assume that they finished loading
			// so clear all from the cache
			InstrSynthDef.clearCache(Server.default);

			err.throw;
		});

		name = argInstr.dotNotation ++ "." ++ outClass.name.asString;
		this.allControlNames.do({ |controlName,i|
			name = name ++ ".";
			// the argNames are fixed for the Instr so we don't need to save them
			// but after that comes secret args and the out
			if(i > argInstr.argNames.size,{
				name = name ++ controlName.name.asString;
			},{
				//inputs[i].addToDefName
				// if outputProxy != control
				
				
			});
			switch(controlName.rate,
				\control, {
					name = name ++ "kr";
				},
				\noncontrol, {
					name = name ++ "nc";
				},
				\scalar, {
					name = name ++ "ir";
				});
			// defaultValue is not just what the synth def is built with
			// but also important things like qnty are passed in this way
			// that can completely alter the synth def architecture
			if(controlName.defaultValue.notNil,{
				name = name ++ controlName.defaultValue.asCompileString;
			});
		});
		longName = name;
		firstName = argInstr.name.last.asString;
		if(firstName.size > 20,{
			firstName = "Instr";
		});
		name = firstName ++ "*" ++ longName.hash;
		// name.debug("name");
	}
	
	// passed to Instr function but not to synth
	addInstrOnlyArg { arg name,value;
		this.addNonControl(name, value);
	}
	
	
	// to cache this def, this info needs to be saved
	// argi points to the slot in objects (as supplied to secretDefArgs)
	// selector will be called on that object to produce the synthArg
	// thus sample can indicate itself and be asked for \tempo or \bufnum

	// initialValue is used for building the synth def
	// selector is what will be called on the object to obtain the real value
	// when the synth is sent
	addSecretIr { arg object,initialValue,selector;
		var name;
		name = "__secret_ir__"++(secretIr.size);
		secretIr = secretIr.add([object,name,initialValue,selector]);
		^Control.names([name]).ir([initialValue])
	}
	addSecretKr { arg object,initialValue,selector;
		var name;
		name = "__secret_kr__"++(secretIr.size);
		secretKr = secretKr.add( [object,name,initialValue,selector]);
		^Control.names([name]).kr([initialValue])
	}
	// a player uses this to install itself into the InstrSynthDef
	// so that it gets added to the Patch's stepchildren
	// and gets prepared and spawned when the Patch plays
	playerIn { arg object;
		var bus;
		bus = this.addSecretIr(object,0,\synthArg);
		^object.instrArgFromControl(bus);
	}

//	addSecretAr { arg object,initialValue,selector;
//		var name;
//		name = "__secret_ar__"++(secretIr.size);
//		secretAr = secretAr.add( [object,initialValue,secretKr.size,selector]);
//		// secretIr = secretIr.add([name,value,argi,selector]);
//		^Control.names([name]).ar([initialValue])
//	}
	secretObjects {
		var so;
		so = IdentitySet.new;
		if(secretIr.notNil,{
			secretIr.do({ |list|
				so.add( list[0] );
			})
		});
		if(secretKr.notNil,{
			secretKr.do({ |list|
				so.add( list[0] );
			})
		});
		if(stepchildren.notNil,{
			stepchildren.do({ |stepchild|
				so.add( stepchild )
			})
		});
		^so.as(Array)
	}
	secretDefArgs {
		var synthArgs,size;
		size = secretIr.size * 2 + (secretKr.size * 2);
		if(size == 0, { ^#[] });
		synthArgs = Array(size);
		secretIr.do({ arg n,i;
			var object,name,value,selector;
			#object,name,value,selector = n;
			synthArgs.add(name); // secret arg name
			synthArgs.add(object.perform(selector)); // value
		});
		secretKr.do({ arg n,i;
			var object,name,value,selector;
			#object,name,value,selector = n;
			synthArgs.add(name); // secret arg name
			synthArgs.add(object.perform(selector)); // value
		});
		^synthArgs
	}
	
	buildControlsWithObjects { arg instr,objects;
		var argNames,defargs,outputProxies;
		objects.do({ arg obj,argi; obj.initForSynthDef(this,argi) });
		defargs = (argNames = instr.argNames).collect({ arg name,defargi;
			var defarg;
			defarg = (objects.at(defargi) ?? {instr.initAt(defargi)});
			defarg.addToSynthDef(this,name);
			defarg
		});
		outputProxies = this.buildControls;
		// the objects themselves know how best to be represented in the synth graph
		// they wrap themselves in In.kr In.ar or they add themselves directly eg (Env)
		^outputProxies.collect({ arg outp,i;
			defargs.at(i).instrArgFromControl(outp,i)
		})
	}

	// give more information, relevant to the def function evaluated
	checkInputs {
		var errors,message;
		children.do { arg ugen;
			var err;
			if ((err = ugen.checkInputs).notNil) { 
				errors = errors.add([ugen, err]);
			};
		};
		if(errors.notNil,{
			this.buildErrorString.postln;
			errors.do({ |err|
				var ugen,msg;
				#ugen,msg = err;
				("UGen" + ugen.class.asString + msg).postln;
				ugen.dumpArgs;
			});
			//instr.func.asCompileString.postln;
			//this.dumpUGens;
			Error("SynthDef" + this.name + "build failed").throw;
		});
		^true
	}
	finishBuild {
		super.finishBuild;
		inputs = nil;
		outputProxies = nil;
		instr = nil;
		("InstrSynthDef built:" + name).inform;
	}
	buildErrorString {
		^String.streamContents({ arg stream;
				stream << Char.nl << "ERROR: Instr build failed; " <<< instr << Char.nl;
				stream << "Inputs:" << Char.nl;
				inputs.do({ |in,i|
					stream << Char.tab << instr.argNameAt(i) << ": " << in << Char.nl;
				});
				stream << "Args passed into Instr function:" << Char.nl;
				outputProxies.do({ |op,i|
					stream << Char.tab << instr.argNameAt(i) << ": " << op << Char.nl;
				});
				if(secretIr.notNil or: secretKr.notNil,{
					stream << "Secret args passed in:" << Char.nl;
					secretIr.do({ |op,i|
						stream << Char.tab << op[1] << ": " << op[2] << " (" << op[0] << ":" << op[3] << ")" << Char.nl;
					});
					secretKr.do({ |op,i|
						stream << Char.tab << op[1] << ": " << op[2] << " (" << op[0] << ":" << op[3] << ")" << Char.nl;
					});
				});
			});
	}
	instrName { ^instr.dotNotation }
	tempoKr { arg object,selector;
		// first client to request will later be asked to produce the TempoBus
		^(tempTempoKr ?? { 
			tempTempoKr = In.kr(
				this.addSecretKr(object,1.0,selector)
			)
		})
	}
	
	*defNameFromObjects { arg objects;
		^String.streamContents({ arg stream;
			var iks=0;
			objects.do({ arg obj,argi;
				// returns 0/i 1/k 2/s tag, adds values if any to stream
				iks = (3 ** argi) *  obj.addToDefName(stream) + iks;
				//obj
			});
			stream << iks;
		});
	}
	*watchServer { arg server;
		if(NotificationCenter.registrationExists(server,\didQuit,this).not,{
			NotificationCenter.register(server,\didQuit,this,{
					this.clearCache(server);
					//this.loadCacheFromDir(server);
			});
		});		
			/*SimpleController(server)
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
			*/
	}
	*clearCache { arg server;
		"Clearing AbstractPlayer SynthDef cache".inform;
		Library.global.removeAt(SynthDef,server);
	}
	*loadCacheFromDir { arg server,dir;
		dir = dir ? SynthDef.synthDefDir;
		(dir++"*").pathMatch.do({ arg p;
			var defName;
			defName = PathName(p).fileNameWithoutExtension;
			Library.put(SynthDef,server,defName.asSymbol,\assumedLoaded);
		})
	}
	*buildSynthDef {
		var sd;
		sd = UGen.buildSynthDef;
		if(sd.isNil,{
			Error("Not currently inside a synth def ugenFunc; No synth def is currently being built.").throw;
		});
		if(sd.isKindOf(InstrSynthDef).not,{
			Error("This requires an InstrSynthDef.").throw;
		})
		^sd
	}

	// in the context of an InstrSynthDef
	// InstrSynthDef.buildSynthDef.onTrig(sig,func,pollableValue)
	// this allows to execute the func in the client whenever the trigger goes
	// the mechanics and the adding/removing of the responder are taken care of
	// because Patch knows when it starts and stops the synth
	onTrig { |trig,func,value=0.0|
		// triggerID is the nTh onTrig we have so far added + 9999
		var triggerID,onTrig;
		triggerID = stepchildren.select({|sc|sc.isKindOf(ClientOnTrigResponder)}).size + 9999;
		onTrig = ClientOnTrigResponder(triggerID,func);
		stepchildren = stepchildren.add(onTrig);

		^SendTrig.kr(trig,triggerID,value)
	}

	/*synthProxy {
		^synthProxy ?? { 
			synthProxy = SynthProxy.new;
			stepchildren = stepchildren.add(synthProxy);
			synthProxy
		}
	}*/
}



ClientOnTrigResponder {

	var <>triggerID, <>func,responder;

	*new { |triggerID,func|
		^super.newCopyArgs(triggerID,func)
	}
	didSpawn { |synth|
		var commandpath = ['/tr', synth.nodeID, triggerID];
		responder = OSCpathResponder(synth.server.addr, commandpath, 
			{|time,responder,message| func.value(time,message[3]) });
		responder.add;
	}
	stopToBundle { |b|
		b.addFunction({ responder.remove; responder = nil })
	}
}

// SynthProxy is a way to access the Synth once the SynthDef has started playing
// there is only one SynthProxy per synth def, though there may be multiple synths spawned
// the synthProxy is in stepchildren and in the Patch's stepChildren so it is prepared and spawned.
// it is roughly equivalent to the synth argument in SC2's Spawn
// 
/*SynthProxy  {

	var events,sched,<synth;

	spawnToBundle { |b|
		b.addMessage(this,\didSpawn)
	}
	
	didSpawn { |synth|
		sched = BeatSched.new;
		// sched any events
		events.do({ |df|
			sched.sched(df[0],df[1])
		})	
	}
	sched { |delta, function|
		events = events.add([delta,function]);
	}
	/ *channelOffset_ {
		// shift the Out.ar
	}* /
}*/

