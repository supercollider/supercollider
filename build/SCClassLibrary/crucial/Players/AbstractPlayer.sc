
AbstractPlayer : AbstractFunction  { 

	var <path,name,<>dirty=true; 
	
	var <synth,<patchOut,<>readyForPlay = false,defName;
		
	play { arg group,atTime,bus;
		var server,bundle;
		
		if(synth.isPlaying,{
			^this
		});  // what if i'm patched to something old ?
			// depend on stop being issued

		if(bus.notNil,{ 
			bus = bus.asBus;
			if(group.isNil,{ 
				server = bus.server;
				group = server.asGroup;
			},{	
				group = group.asGroup;
				server = group.server;
			})
		},{
			group = group.asGroup;
			server = group.server;
			// leave bus nil
		});
					
		bundle = List.new;
		
		if(readyForPlay,{
			this.makePatchOut(group,false,bus);
			this.spawnAtTime(atTime);
		},{
			Routine({
				var limit = 100;
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
					// screwy
					//this.patchOut_(true); // keep it from making one
					
					this.prepareToBundle(group,bundle);
					
					if(bundle.notEmpty,{
						server.listSendBundle(nil,bundle);
					});
					// need some way to track all the preps completion
					// also in some cases the prepare can have a completion
					// tacked on and we can combine with the spawn message
					
					// need a fully fledged OSCMessage that can figure it out
					0.3.wait;
					
					this.makePatchOut(group,false,bus);// public
					
					this.spawnAtTime(atTime);
				});
			}).play;
		});
	}
	prepareForPlay { arg group,private = false;
		group = group.asGroup;
		super.prepareForPlay(group); // sends to server
		this.makePatchOut(group,false);	
	}
	prepareToBundle { arg group,bundle;
		readyForPlay = false;
		
		group = group.asGroup;
		this.loadDefFileToBundle(bundle,group.server);
		// if made, we have secret controls now
		// else we already had them
		
		// has inputs
		this.children.do({ arg child;
			child.prepareToBundle(group,bundle);
		});
		
		// not really until the last confirmation comes from server
		readyForPlay = true;
	}


	makePatchOut { arg group,private = false,bus;
		this.topMakePatchOut(group,private,bus);
		this.childrenMakePatchOut(group,true);
	}
	topMakePatchOut { arg group,private = false,bus;
		//Patch doesn't know its numChannels or rate until after it makes the synthDef
		if(this.rate == \audio,{// out yr speakers
			if(private,{
				this.setPatchOut(
					AudioPatchOut(this,group,bus ? Bus.audio(group.server,this.numChannels))
					)
			},{			
				this.setPatchOut(
					AudioPatchOut(this,group,bus ? Bus(\audio,0,this.numChannels,group.server))
							)
			})
		},{
			if(this.rate == \control,{
				this.setPatchOut(
					ControlPatchOut(this,group,
							bus ? Bus.control(group.server,this.numChannels))
						)
			},{
				(thisMethod.asString + "Wrong output rate: " + this.rate + 
			".  AbstractPlayer cannot prepare this object for play.").error;
			});
		});
		^patchOut
	}
	childrenMakePatchOut { arg group,private = true;
		this.children.do({ arg child;
			child.makePatchOut(group,private)
		});
	}
	setPatchOut { arg po; patchOut = po; }

	
	spawnAtTime { arg atTime;
		var bundle;
		bundle = List.new;
		this.spawnToBundle(bundle);
		
		// atTime.asDeltaTime
		patchOut.server.listSendBundle( atTime, bundle);
		// schedule atTime:
		if(synth.notNil,{
			synth.isPlaying = true;
			synth.isRunning = true;
		});
		this.didSpawn;
	}
	spawnToBundle { arg bundle;
		this.children.do({ arg child;
			child.spawnToBundle(bundle);
		});
		synth = Synth.basicNew(this.defName,patchOut.server);
		bundle.add(
			synth.addToTailMsg(patchOut.group,
				this.synthDefArgs.collect({ arg a,i; [i,a]}).flat 
					//++ this.secretSynthDefArgs
					//++ this.children.collect({ arg child; child.secretSynthDefArgs }).flat,
			)
		);
	}
	/*
		when player saves, save defName and secret args (name -> inputIndex)
			that means you can quickly check, load and execute synthdef
		defName only if not == classname
			
		save it all in InstrSynthDef (patch is only one with secret args so far)
	*/
	loadDefFileToBundle { arg bundle,server;
		var def,bytes,dn;

		// can't assume the children are unchanged
		this.children.do({ arg child;
			child.loadDefFileToBundle(bundle,server);
		});

		dn = this.defName;
		if(dn.isNil or: {
			dn = dn.asSymbol;
			// name creation still has a bug, can't depend yet
			//Library.at(SynthDef,server,dn).isNil
			
			true
		},{
			// save it in the archive of the player
			def = this.asSynthDef;
			bytes = def.asBytes;
			bundle.add(["/d_recv", bytes]);
			// even if name was nil before (Patch), its set now
			defName = def.name;
			
			// TODO on server quit have to clear this
			Library.put(SynthDef,server,defName,true);
		});
	}
	
	//for now:  always sending, not writing
	writeDefFile {
		this.asSynthDef.writeDefFile;
		this.children.do({ arg child;
			child.writeDefFile;
		});
	}
	
	
	/** SUBCLASSES SHOULD IMPLEMENT **/
	rate { ^\audio }
	numChannels { ^1 }
	
	//  this works for simple audio function subclasses
	//  but its probably more complicated if you have inputs
	asSynthDef { 
		^SynthDef(this.defName,{ arg i_outIndex = 0;
			if(this.rate == \audio,{
				Out.ar(i_outIndex,this.ar)
			},{
				Out.kr(i_outIndex,this.kr)
			})
		})
	}
	synthDefArgs { 
		// if children are your args, you won't have to implement this
		// no indices
		^this.children.collect({ arg ag,i; ag.synthArg })  
				 ++ [patchOut.bus.index] // always goes last
	}
	defName {
		^defName ?? {this.class.name.asString}
	}
	didSpawn { arg patchIn,synthArgi;
		if(patchIn.notNil,{
			patchOut.connectTo(patchIn,false); // we are connected now
			patchIn.nodeControl_(NodeControl(synth,synthArgi));
		});
	}


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
	busIndex { ^patchOut.index }
	bus { ^patchOut.bus }
	group { ^patchOut.group }
	server { ^patchOut.server }

/*  RECORDING ETC.
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



/* UGEN STYLE USAGE */

	// only works immediately in  { }.play
	// for quick experimentation, does not encourage reuse
	ar {
		// ideally would add itself as a child to the current InstrSynthDef
		//this.play;
		//^In.ar(this.busIndex,this.numChannels)
		// would break for anyone who overrides .ar
		// could change to asUGenFunc
		// or this could be asInAr
		
		^this.subclassResponsibility(thisMethod)
	}
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
	
/*
	// all that is needed to play inside standard patterns
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
	name { 
		^(name ?? 
		{
			name = if(path.notNil,{ 
						PathName(path).fileName
					},nil);
			name  
		}) 
	}
	asString { ^this.name ?? { super.asString } }

	path_ { arg p; path = PathName(p).asRelativePath }

	// structural utilities
	children { ^[] }
	deepDo { arg function;// includes self
		function.value(this);
		this.children.do({arg c; function.value(c); c.tryPerform(\children).do(function) });
	}	 
	allChildren { 
		var all;
		all = List.new;
		this.deepDo({ arg child; all.add(child) });
		^all
		// includes self
	}
	
	asCompileString { // support arg sensitive formatting
		var stream;
		stream = PrettyPrintStream.on(String(256));
		this.storeOn(stream);
		^stream.contents
	}
	
	guiClass { ^AbstractPlayerGui }

}



MultiplePlayers : AbstractPlayer {

	// manages multiple players sharing the same bus
	// doesn't make a synth

	voices { this.subclassResponsiblity(thisMethod) }
	
	rate { ^this.voices.first.rate }
	numChannels { ^this.voices.first.numChannels }
	
	// prepare  :  making a single bus, may override in subclasses

	childrenMakePatchOut { arg group,private = false;
		this.voices.do({ arg vo;
			// use mine
			vo.setPatchOut(AudioPatchOut(vo,patchOut.group,patchOut.bus.copy));
			// but children make their own
			vo.childrenMakePatchOut(group,false);
		})
	}
	setPatchOut { arg po;
		patchOut = po;
		//everybody plays onto same bus
		this.voices.do({ arg pl;
			// ISSUE if rate is not mine, throw an error
			pl.setPatchOut(po.deepCopy);
		})
	}
	spawnAtTime { arg atTime;
		var bundle;
		bundle = List.new;
		this.voices.do({ arg pl;
			pl.spawnToBundle(bundle)
		});
		
		// atTime.asDeltaTime
		patchOut.server.listSendBundle( atTime, bundle);
		//isPlaying = true;
		this.didSpawn;
	}
	spawnToBundle { arg bundle;
		this.voices.do({ arg pl;
			pl.spawnToBundle(bundle)
		})
	}
	didSpawn { arg patchIn,synthArgi;
		super.didSpawn(patchIn,synthArgi);
		// right ? everybody gets connected.
		this.voices.do({ arg pl;
			pl.didSpawn(patchIn,synthArgi);
		})
	}
	loadDefFileToBundle { arg bundle,server;
		this.voices.do({ arg pl;
			pl.loadDefFileToBundle(bundle,server)
		})
	}		
	free {
		this.voices.do({ arg pl;
			pl.free
		});
		super.free;
	}
}



AbstractPlayerProxy : AbstractPlayer {

	// like  a voice, holds a source that does the actual playing
	// should be switchable
	// duplicates the Patch and Bus to the source
	
	var source;


	
	asSynthDef { ^source.asSynthDef }
	rate { ^source.rate }
	numChannels { ^source.numChannels }
	defName { ^source.defName }
	setPatchOut { arg po;
		super.setPatchOut(po);
		// a copy to the source
		source.setPatchOut(PatchOut(source,patchOut.group,patchOut.bus.copy));
	}
	didSpawn { arg patchIn,synthArgi;
		super.didSpawn(patchIn,synthArgi);
		source.didSpawn(patchIn,synthArgi);
	}
	synthDefArgs { ^source.synthDefArgs }
	children { ^source.children }
	instrArgFromControl { arg control;
		^source.instrArgFromControl(control)
	}
	free {
		source.free;
		super.free;
	}
	
}



