
AbstractPlayer : AbstractFunction  { 

	classvar <>directory="";
	
	var <path,name,<>dirty=true; 
	
	var <synth,<>patchOut,<>readyForPlay = false,defName,patchOutsOfChildren;
		
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
		server = group.server;
		
		if(readyForPlay,{
			this.makePatchOut(group);
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
					patchOut = true; // keep it from making one
					this.prepareForPlay(group,bundle);
					
					//["-play prepared",bundle].postln;
					if(bundle.notEmpty,{
						server.listSendBundle(nil,bundle
												//.insp("prepare")
												);
					});
					// need some way to track all the preps completion
					// also in some cases the prepare can have a completion
					// tacked on and we can combine with the spawn message
					
					// need a fully fledged OSCMessage that can figure it out
					0.1.wait;
					
					this.makePatchOut(group);
					this.spawnAtTime(atTime);
				});
			}).play;
		});
	}
	makePatchOut { arg group;
		//patch doesn't know its numChannels until after it makes the synthDef
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
		});
		^patchOut
	}
	
	prepareForPlay { arg group,bundle;
		//this.insp("prepareForPlay");
		readyForPlay = false;
		
		this.loadDefFileToBundle(bundle);
		// if made, we have secret controls now
		// else we already had them
		
		// play would have already done this if we were top level object
		if(patchOut.isNil,{ // private out
			group = group.asGroup;
			patchOut = PatchOut(this,group,
						Bus.alloc(this.rate,group.server,this.numChannels))
						// private buss on this server
		});
		// has inputs
		patchOutsOfChildren = this.children.collect({ arg child;
			child.prepareForPlay(group,bundle);
			//child.makePatchOut(group)
			child.patchOut
		});
		
		// not really until the last confirmation comes from server
		readyForPlay = true;
	}
	spawnAtTime { arg atTime;
		var bundle;
		bundle = List.new;
		this.spawnToBundle(bundle);
		
		// atTime.asDeltaTime
		patchOut.server.listSendBundle( atTime, bundle//.insp("spawn")
												);
		// schedule atTime:
		synth.isPlaying = true;
		synth.isRunning = true;
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
			
		save it all in InstrSynthDef
	*/
	loadDefFileToBundle { arg bundle;
		var def,bytes;
		if(this.defName.isNil or: {
			true
			// Library.at(SynthDef,patchOut.server,this.defName.asSymbol).isNil
		},{
			this.children.do({ arg child;
				child.loadDefFileToBundle(bundle);
			});

			// save it in the archive of the player
			def = this.asSynthDef;
			bytes = def.asBytes;
			bundle.add(["/d_recv", bytes]);
			defName = def.name;
			
			//secretArgs = def.secretArgs;
			
			// on server quit have to clear this
			// but for now at least we know it was written, and therefore
			// loaded automatically on reboot
			// Library.put(SynthDef,patchOut.server,this.defName.asSymbol,true);
		});
	}
	
	writeDefFile {
		this.asSynthDef.writeDefFile; // todo: 	check if needed
		this.children.do({ arg child;
			child.writeDefFile;
		});
	}
	
	
	/** SUBCLASSES SHOULD IMPLEMENT **/
	// this works for simple audio function subclasses
	//  but its probably more complicated if you have inputs
	asSynthDef { 
		^SynthDef(this.defName,{ arg outIndex = 0;
			if(this.rate == \audio,{
				Out.ar(outIndex,this.ar)
			},{
				Out.kr(outIndex,this.kr)
			})
		})
	}
	// if children are your args, you won't have to implement this
	synthDefArgs { 
		// no indices
		^this.children.collect({ arg ag,i; ag.synthArg })  
				 ++ [patchOut.bus.index] // always goes last
	}
	//secretSynthDefArgs { ^#[] } // requires names
	defName {
		^defName ?? {this.class.name.asString}
	}
	didSpawn { arg patchIn,synthArgi;
		if(patchIn.notNil,{
			patchOut.connectTo(patchIn,false); // we are connected now
			patchIn.nodeControl_(NodeControl(synth,synthArgi));
		});
			
		//node control if you want it
		//synthPatchIns.do({ arg patchIn,synthArgi;
		//	patchIn.nodeControl_(NodeControl(synth,synthArgi));
		//});
		//patchIns.do({ arg patchIn,pti;
		//	// objects hook up 
		//	patchOutsOfChildren.at(pti).connectTo(patchIn,false); 
		//});
		/* probably better
		this.children.do({arg ag,i;
			ag.parentDidSpawn(patchIns.at(i));
		});
		*/
	}
	//parentDidSpawn { arg parentalPatchIn;
	//	patchOut.connectTo(parentalPatchIn,false); // don't send data
	//}




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
	name { ^(name ?? 
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






