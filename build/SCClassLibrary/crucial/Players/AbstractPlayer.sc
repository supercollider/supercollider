
AbstractPlayer : AbstractFunction  { 

	var <path,<>dirty=false; 
	
	var <synth,<patchOut,<>readyForPlay = false;
	
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
		var server;
		
		if(synth.isPlaying,{
			^patchOut // already playing, return my output object
		}); // what if i'm patched to something old ?
		// depend on stop being issued

		group = group.asGroup;		
		if(patchOut.isNil,{
			if(this.rate == \audio,{
				// public buss
				patchOut = PatchOut(this,group,Bus(\audio,0,this.numChannels,group.server))
			},{
				if(this.rate == \control,{
					patchOut = 
						PatchOut(this,group,
								Bus.next(\control,this.numChannels,group.server))
				},{
					("Wrong output rate: " + this.rate.asString + ".  AbstractPlayer cannot prepare this object for play.").error;
				});
			})
		});
		// a kr player only cares about its patchIn, it doesnt have to
		// run on the server at all.  not really kr then.
		// assuming you directly set the control input of the patchIn
		// rather than set the control output of a bus (more cpu)

		if(readyForPlay,{
			this.spawn(group,atTime);
		},{
			Routine({
				var limit = 100;
				server = patchOut.server;
				if(server.serverRunning.not,{ // TODO only one attempt to boot per server
					if(server.boot.notNil,{ // it is trying
						while({
							server.serverRunning.not and: {(limit = limit - 1) > 0}
						},{
							//would be nice to get a Notification or changed
							"waiting for server to boot...".inform;
							0.2.wait;	
						});
					});
				});

				if(server.serverRunning.not,{
					"server failed to start".error; //die
				},{
					// already have a patchOut
					this.prepareForPlay(group,{
						//"prepared finished, spawning".postln;
						
						// set TempoBus if you need to
						{
							Tempo.changed;
							nil
						}.defer;
						
						this.spawn(group,atTime);
						//"spawned".postln;

					});
				});
			}).play;
		});
		^patchOut
	}

	prepareForPlay { arg group,onComplete;
		// subclasses handle children as they wish to
		
		// might already be playing on a different server

		// how do i specify private or public bus ?

		readyForPlay = false;
		
		// play would have already done this if we were top level
		if(patchOut.isNil,{
			group = group.asGroup;
			patchOut = PatchOut(this,group,
					Bus.alloc(this.rate,group.server,this.numChannels))
					// private buss on this server
		});
		
		fork({		
			var server;
			server = patchOut.server;		
			// boot server if needed
		
			// requires the children to be written already
			this.loadDefFile(server); // if not already loaded, written

			// temporary pause
			0.3.wait;
			
			// Library.put(SynthDef,server.name ? server,this.defName);
			// on server quit have to clear this
			
			readyForPlay = true;
			onComplete.value
		});
	}

	spawn { arg atTime;
		//subclasses should spawn children if they need to
		synth = Synth.tail(patchOut.group,this.defName,
					this.synthDefArgs.collect({ arg a,i; [i,a]}).flat );
		^patchOut
	}

	loadDefFile { arg server;
		var def;
		this.children.do({ arg child;
			child.loadDefFile(server);
		});		
		def = this.asSynthDef;
		def.writeDefFile; // todo: 	check if needed
		server.loadSynthDef(def.name);
	}
	
	writeDefFile {
		this.asSynthDef.writeDefFile; // todo: 	check if needed
		this.children.do({ arg child;
			child.writeDefFile;
		});
	}
	
	// this might work for your subclass, 
	//  but  its probably more complicated if you have inputs
	asSynthDef { 
		^SynthDef(this.defName,{ arg outIndex = 0;
			Out.ar(outIndex,this.ar)
		})
	}
	synthDefArgs { ^[patchOut.bus.index] }

	defName {
		^this.class.name.asString
	}
	
	isPlaying { ^synth.isPlaying }

	stop { ^this.free } // for now
	// do we control all the children ? mostly
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

KrPlayer : AbstractPlayer {

	var <>spec;
	
	rate { ^\control }

	kr {  ^this.subclassResponsibility(thisMethod) }
	ar { ^K2A.ar(this.kr) }
	value { ^this.kr }

	initialValue { ^0.0 }

	//guiClass { ^KrPlayerGui }
	
	asSynthDef { 
		// but its probably more complicated for your subclass
		^SynthDef(this.defName,{ arg outIndex = 0;
			Out.kr(outIndex,this.kr)
		})
	}
	
}





