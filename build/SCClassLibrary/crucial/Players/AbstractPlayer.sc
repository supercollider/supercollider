
AbstractPlayer : AbstractFunction  { 

	var <path,<>dirty=false; 
	
	var <synth,<patchOut,<>readyForPlay = false, defName;
		
	// subclasses must implement
	ar { ^this.subclassResponsibility(thisMethod) }
	kr { ^this.ar }
	value {  ^this.ar }
	valueArray { ^this.value }
	
	// ugen style syntax
	*ar { arg ... args;
		^this.performList(\new,args).ar
	}
	*kr { arg ... args;
		^this.performList(\new,args).kr
	}

	// function composition
	composeUnaryOp { arg operator;
		^PlayerUnop.new(operator, this)
	}
	composeBinaryOp { arg operator, pattern;
		^PlayerBinop.new(operator, this, pattern)
	}
	reverseComposeBinaryOp { arg operator, pattern;
		^PlayerBinop.new(operator, pattern, this)
	}
	
	rate { ^\audio }
	numChannels { ^1 }

	/*** SC3 SERVER SUPPORT ***/
	play { arg group,atTime;

		// specify patching, boot server, prepare, load, spawn
		var server,bundle;
		bundle = List.new;
		
		if(synth.isPlaying,{
			^this
		}); // what if i'm patched to something old ?
		// depend on stop being issued

		group = group.asGroup;
		if(patchOut.isNil,{
			if(this.rate == \audio,{// out yr speakers
				patchOut = PatchOut(this,
								group,
							Bus(\audio,0,this.numChannels,group.server))
			},{
				if(this.rate == \control,{
					patchOut = 
						PatchOut(this,group,
								Bus.control(this.numChannels,group.server))
				},{
					("Wrong output rate: " + this.rate + 
				".  AbstractPlayer cannot prepare this object for play.").error;
				});
			})
		});

		if(readyForPlay,{
			this.spawnAtTime(atTime);
		},{
			Routine({
				var limit = 100;
				server = patchOut.server;
				if(server.serverRunning.not,{
					server.boot;
					while({
						server.serverRunning.not 
							and: {(limit = limit - 1) > 0}
					},{
						//can watch for Server.changed but we still need to time out
						"waiting for server to boot...".inform;
						0.2.wait;	
					});
				});
				if(server.serverRunning.not,{
					"server failed to start".error;
				},{
					// already have a patchOut
					this.prepareForPlay(group,bundle);
					["-play prepared",bundle].postln;
					if(bundle.notEmpty,{
						server.listSendBundle(nil,bundle);
					});
					// need some way to track all the preps completion
					// also in some cases the prepare can have a completion
					// tacked on
					// need a fully fledged OSCMessage
					0.1.wait;
					
					this.spawnAtTime(atTime);
				});
			}).play;
		});
	}

	prepareForPlay { arg group,bundle;
		
		readyForPlay = false;
		// play would have already done this if we were top level object
		if(patchOut.isNil,{
			group = group.asGroup;
			patchOut = PatchOut(this,group,
						Bus.alloc(this.rate,group.server,this.numChannels))
						// private buss on this server
		});
		patchOut.patchOutsOfInputs = this.children.collect({ arg child;
			child.prepareForPlay(group,bundle);
			child.patchOut
		});

		this.loadDefFileToBundle(bundle);
		
		// not really until the last confirmation comes from server
		readyForPlay = true;
	}
	spawnAtTime { arg atTime;
		var bundle;
		bundle = List.new;
		this.spawnToBundle(bundle);
		
//		"spawnAtTime->".postln;
//		bundle.do({ arg l; 
//			l.postln; 
//		});
		
		// atTime.asDelta
		patchOut.server.listSendBundle( atTime, bundle);
		this.didSpawn;
	}
	spawnToBundle { arg bundle;
		this.children.do({ arg child;
			child.spawnToBundle(bundle);
		});
		synth = Synth.newMsg(bundle, // newToBundle
				this.defName,
				this.synthDefArgs.collect({ arg a,i; [i,a]}).flat,
				patchOut.group,
				\addToTail
				);
	}
	loadDefFileToBundle { arg bundle;
		var def,bytes;
		if(this.defName.isNil or: {
			Library.at(SynthDef,patchOut.server,this.defName.asSymbol).isNil
		},{
			//TODO check path, if a saved version is there
			// save it in the archive of the player
			def = this.asSynthDef;
			bytes = def.asBytes;
			bundle.add(["/d_recv", bytes]);
			defName = def.name;
			//bundle.add(["/d_load", "synthdefs/" ++ defName ++ ".scsyndef"]);

			// on server quit have to clear this
			// but for now at least we know it was written, and therefore
			// loaded automatically on reboot
			//Library.put(SynthDef,patchOut.server,this.defName.asSymbol,true);
		});
		this.children.do({ arg child;
			child.loadDefFileToBundle(bundle);
		});
	}
	
	writeDefFile {
		this.asSynthDef.writeDefFile; // todo: 	check if needed
		this.children.do({ arg child;
			child.writeDefFile;
		});
	}
	//initDefArg { ^patchOut.initDefArg }
	
	
	/** SUBCLASSES SHOULD IMPLEMENT **/
	// this works for simple audio function subclasses
	//  but its probably more complicated if you have inputs
	asSynthDef { 
		^SynthDef(this.defName,{ arg outIndex = 0;
			Out.ar(outIndex,this.ar)
		})
	}
	// if children are your args, you won't have to implement this
	synthDefArgs { 
		^this.children.collect({ arg ag,i; ag.synthArg })  
				 ++ [patchOut.bus.index] // always goes last
	}
	defName {
		^this.class.name.asString
	}
	didSpawn {}
	



	/* status */
	isPlaying { ^synth.isPlaying }

	stop { ^this.free } // for now
	// do we control all the children ?
	// they need to keep track of their own connections via PatchOut
	run { arg flag=true;
		if(synth.notNil,{
			synth.run(flag);
		});
	}	
	free {
		if(synth.notNil,{
			synth.free;
			synth = nil;
		});
		if(patchOut.notNil,{
			patchOut.free;
			patchOut = nil;
		});
		// release any connections i have made
		readyForPlay = false;
	}




/*
	scope 	{ Synth.scope({ Tempo.setTempo; this.ar }) }
	fftScope 	{ Synth.fftScope({ Tempo.setTempo; this.ar }) }
	record	{ arg pathName,headerFormat='SD2',sampleFormat='16 big endian signed';
		 Synth.record({ Tempo.setTempo; this.ar },this.timeDuration, pathName, headerFormat, sampleFormat) 
	}
	write 	{  arg pathName,headerFormat='SD2',sampleFormat='16 big endian signed',duration;
		var dur;
		dur = duration ?? {this.timeDuration};
		if(dur.notNil,{
		 	Synth.write({ Tempo.setTempo; this.ar },dur, pathName, headerFormat, sampleFormat) 
		},{
			(this.name.asString ++ " must have a duration to do Synth.write ").error;
		})
	}
*/	




	// subclasses need only implement beatDuration 
	beatDuration { ^nil } // nil means inf
	timeDuration { var bd;
		bd = this.beatDuration;
		if(bd.notNil,{
			^Tempo.beats2secs(bd)
		},{
			^nil
		});	
	}
	delta { 	^this.beatDuration	}
	
	// all that is needed to play inside standard patterns
/*
	embedInStream { arg inval;
		// i am one event
		^inval.make({
			var dur;
			// needs to protect against inf / nil !!
			~dur = dur = this.beatDuration ? 8192; // arbitrary long ass time 
			~ugenFunc = { 
				~synth.sched(dur,{ thisSynth.release });
				EnvGen.kr(Env.asr) * this.ar
			}
		}).yield
	}
*/	
	
	// if i am saved/loaded from disk my name is my filename
	// otherwise it is "a MyClassName"
	name { ^(if(path.notNil,{ PathName(path).fileName},nil )  ) }
	asString { ^this.name ?? { super.asString } }

	path_ { arg p; path = PathName(p).asRelativePath }

	// structural utilities
	children { ^[] }
	deepDo { arg function;
		function.value(this);
		this.children.do({arg c; function.value(c); c.children.do(function) });
	}	 


	
	asCompileString { // support arg sensitive formatting
		var stream;
		stream = PrettyPrintStream.on(String(256));
		this.storeOn(stream);
		^stream.contents
	}
	
	guiClass { ^AbstractPlayerGui }

}






