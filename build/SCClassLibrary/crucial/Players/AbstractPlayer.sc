
AbstractPlayer : AbstractFunction  { 

	var <path,name,<>dirty=true; 
	
	var <synth,<group,<server,<patchOut,<>readyForPlay = false,defName;
		
	play { arg agroup,atTime,bus;
		var bundle;
		if(synth.isPlaying,{ ^this });

		if(bus.notNil,{ 
			bus = bus.asBus;
			if(agroup.isNil,{
				server = bus.server;
				group = server.asGroup;
			},{	
				group = agroup.asGroup;
				server = group.server;
			})
		},{
			group = agroup.asGroup;
			server = group.server;
			// leave bus nil
		});
		
		bundle = CXBundle.new;

		if(readyForPlay and: {Library.at(SynthDef,server,this.defName.asSymbol).notNil},{
			this.makePatchOut(group,false,bus,bundle);
			this.spawnToBundle(bundle);
			server.waitForBoot({
				bundle.send(this.server,atTime);
			});
		},{
			Routine({
				var limit = 100,bsize;
				if(server.serverRunning.not,{
					server.startAliveThread(0.1,0.4);
					if(server.isLocal,{
						InstrSynthDef.loadCacheFromDir(server);
					});
					server.boot(false);
					while({
						server.serverRunning.not 
							and: { (limit = limit - 1) > 0 }
					},{
						"waiting for server to boot...".inform;
						0.4.wait;	
					});
					if(server.dumpMode != 0,{ server.stopAliveThread; });
					//atTime is now bogus
				});
				if(server.serverRunning.not,{
					"server failed to start".error;
				},{
					if(server.dumpMode != 0,{
						server.stopAliveThread;
					});
					bsize = this.prepareForPlay(group,false,bus) / 15.0;
					// need some way to track all the preps completion
					// also in some cases the prepare can have a completion
					// tacked on and we might combine with the spawn message
					
					// need a fully fledged OSCMessage that can figure it out
					bsize.wait;
			
					atTime = atTime ? 0;
					if(atTime > bsize,{ atTime = atTime - bsize });
					this.spawnAtTime(atTime);
				});
			}).play(SystemClock);
		});
	}
	prepareForPlay { arg agroup,private = false,bus;
		var bundle;
		bundle = CXBundle.new;
		group = agroup.asGroup;
		this.prepareToBundle(group,bundle);
		this.makePatchOut(group,private,bus,bundle);
		^bundle.clumpedSendNow(group.server)
	}
	prepareToBundle { arg agroup,bundle;
		readyForPlay = false;
		
		group = agroup.asGroup;
		this.children.do({ arg child;
			child.prepareToBundle(group,bundle);
		});
		this.loadDefFileToBundle(bundle,group.server);
		
		readyForPlay = true;
	}

	isPlaying { ^synth.isPlaying ? false }
	
	// these always call children
	stop {
		this.freeSynth;
		this.freePatchOut;
		this.children.do({ arg child;
			child.stop;
		});
	}
	stopToBundle { arg b;
		if(synth.isPlaying,{
			b.add(synth.freeMsg);
		});
		b.addAction(this,\freePatchOut);
		this.children.do({ arg child;
			child.stopToBundle(b);
		});
	}
	free {
		this.stop;
		this.freeHeavyResources;
		readyForPlay = false;
		this.children.do({ arg child;
			child.free;
		});
	}
	
	// these don't call children
	freeSynth {
		if(synth.isPlaying,{
			synth.free;
		});
		synth = nil;
	}
	freePatchOut {
		if(patchOut.notNil,{
			patchOut.free;
			patchOut = nil;
		});
	}
	freeHeavyResources { arg bundle; }
	
	run { arg flag=true;
		if(synth.notNil,{
			synth.run(flag);
		});
		//should call children ?
	}
	release { arg releaseTime = 0.1,atTime;
		var rb;
		rb = CXBundle.new;
		this.releaseToBundle(releaseTime,rb);
		rb.send(server,atTime);
	}
	releaseToBundle { arg releaseTime,bundle;
		if(synth.notNil,{
			bundle.add(synth.releaseMsg(releaseTime));
		});
		if(releaseTime ? 0.0 > 0.01,{
			bundle.addFunction({
				//if(releaseTime.notNil,{
					SystemClock.sched(releaseTime,{ this.stop; nil; })
				//},{
				//	this.stop; // happening before the gate gets there !
				//})
			});
		},{
			this.stopToBundle(bundle);
			// is same thing...
		});
	}
		
	busIndex { ^patchOut.index }
	bus { ^patchOut.bus }
	bus_ { arg b;
		b = b.asBus(this.rate,this.numChannels,this.server);
		if(patchOut.notNil,{
			patchOut.bus = b;
			if(synth.isPlaying,{
				synth.set(\out,b.index)
			})
		});
		// otherwise we should have had a patchOut
	}

	spawn { this.spawnAtTime(nil) }
	spawnAtTime { arg atTime;
		var bundle;
		bundle = CXBundle.new;
		this.spawnToBundle(bundle);
		bundle.send(this.server,atTime);
	}
	spawnToBundle { arg bundle;
		this.children.do({ arg child;
			child.spawnToBundle(bundle);
		});
		synth = Synth.basicNew(this.defName,server);
		NodeWatcher.register(synth);
		bundle.add(
			synth.addToTailMsg(this.group,this.synthDefArgs)
		);
		bundle.addAction(this,\didSpawn);
	}
	spawnOnBus { arg bus,atTime;
		this.spawnOn(bus.server.asGroup,true,bus);
	}
	spawnOn { arg group,bus, atTime;
		var bundle;
		bundle = CXBundle.new;
		this.spawnOnToBundle(group,bus,bundle);
		bundle.send(this.server,atTime);
	}
	spawnOnToBundle { arg group,bus,bundle;
		if(patchOut.isNil,{
			this.makePatchOut(group,true,bus,bundle);
		},{
			if(patchOut.bus != bus,{ patchOut.bus.free });
			patchOut.bus = bus;
			if(patchOut.group != group,{ patchOut.group = group });
		});
		this.spawnToBundle(bundle);
	}
	

	/*
		if defName != classname
			when player saves, save defName and secret args (name -> inputIndex)
				that means you can quickly check, load and execute synthdef
					
		save it all in InstrSynthDef (patch is only one with secret args so far)
	*/
	makePatchOut { arg group,private = false,bus;
		group = group.asGroup;
		server = group.server;
		this.topMakePatchOut(group,private,bus);
		this.childrenMakePatchOut(group,true);
	}
	topMakePatchOut { arg group,private = false,bus;
		if(patchOut.notNil,{
			if(bus.notNil and: {patchOut.bus != bus},{
				//frees the previous bus it had
				//[this,patchOut.bus,"freeing previous bus :" ].insp;
				patchOut.bus.free;
				patchOut.bus = bus;
			});
			if(group.notNil and: {patchOut.group != group},{
				patchOut.group = group;
			});
			if(private,{
				if(patchOut.bus.isAudioOut,{
					patchOut.bus = Bus.audio(group.server,this.numChannels);
				})
			}/*,{
				if(patchOut.bus.isAudioOut.not,{
					patchOut.bus = Bus(\audio,0,this.numChannels,group.server)
				})
			}*/)
		},{
			//Patch doesn't know its numChannels or rate until after it makes the synthDef
			if(this.rate == \audio,{// out yr speakers
				if(private,{
					this.setPatchOut(
						AudioPatchOut(this,group,bus 
								?? {Bus.audio(group.server,this.numChannels)})
						)
				},{
					this.setPatchOut(
						AudioPatchOut(this,group,bus 
								?? {Bus(\audio,0,this.numChannels,group.server)})
								)
				})
			},{
				if(this.rate == \control,{
					this.setPatchOut(
						ControlPatchOut(this,group,
								bus ?? {Bus.control(group.server,this.numChannels)})
							)
				},{
					this.setPatchOut(
						ScalarPatchOut(this,group,bus)
					)
				});
			});
		});
				
		^patchOut
	}
	childrenMakePatchOut { arg group,private = true;
		this.children.do({ arg child;
			child.makePatchOut(group,private,nil)
		});
	}
	setPatchOut { arg po; // not while playing
		patchOut = po;
		if(patchOut.notNil,{
			server = patchOut.server;
		});
	}

	
	loadDefFileToBundle { arg bundle,server;
		var def,bytes,dn;

		// can't assume the children are unchanged
		this.children.do({ arg child;
			child.loadDefFileToBundle(bundle,server);
		});

		dn = this.defName;
		if(dn.isNil or: {
			dn = dn.asSymbol;
			if(Library.at(SynthDef,server,dn).notNil,{
				("already loaded:"+dn).debug;
				false
			},{
				true
			})
		},{
			// save it in the archive of the player
			// or at least the name.
			// Patches cannot know their defName until they have built
			( "building:" + (this.path ?? {this.name}) ).debug;
			def = this.asSynthDef;
			bytes = def.asBytes;
			bundle.add(["/d_recv", bytes]);
			// even if name was nil before (Patch), its set now
			defName = def.name;
			("loading def:" + defName).debug;
			// InstrSynthDef watches \serverRunning to clear this
			InstrSynthDef.watchServer(server);
			Library.put(SynthDef,server,defName.asSymbol,true);
			// write for next time
			// def.writeDefFile;
		});
	}
	//for now:  always sending, not writing
	writeDefFile {
		this.asSynthDef.writeDefFile;
		this.children.do({ arg child;
			child.writeDefFile;
		});
	}
	addToSynthDef {  arg synthDef,name;
		// value doesn't matter so much, we are going to pass in a real live one
		synthDef.addKr(name,this.synthArg ? 0); // \out is a .kr bus index
	}

	synthArg { ^patchOut.synthArg }
	instrArgFromControl { arg control;
		// a Patch could be either
		^if(this.rate == \audio,{
			In.ar(control,this.numChannels)
		},{
			In.kr(control,this.numChannels)
		})
	}
	
	/** SUBCLASSES SHOULD IMPLEMENT **/
	//  this works for simple audio function subclasses
	//  but its probably more complicated if you have inputs
	asSynthDef { 
		^SynthDef(this.defName,{ arg out = 0;
			if(this.rate == \audio,{
				Out.ar(out,this.ar)
			},{
				Out.kr(out,this.kr)
			})
		})
	}
	synthDefArgs { 
		^[\out,patchOut.synthArg]		
	}
	defName {
		^defName ?? {this.class.name.asString}
	}
	didSpawn {	
		/*if(synth.notNil,{// should always have a synth
			synth.isPlaying = true;
			synth.isRunning = true;
		});*/
	}
	rate { ^\audio }
	numChannels { ^1 }
	spec { 
		^if(this.rate == \audio,{
			AudioSpec(this.numChannels)
		},{ // or trig
			ControlSpec(-1,1)
		})
	}


	/** hot patching **/
	connectTo { arg hasInput;
		this.connectToPatchIn(hasInput.patchIn,this.isPlaying ? false);
	}
	connectToInputAt { arg player,inputIndex=0;
		this.connectToPatchIn(player.patchIns.at(inputIndex),this.isPlaying ? false)
	}
	connectToPatchIn { arg patchIn,needsValueSetNow = true;
		// if my bus is public, change to private
		if(this.isPlaying and: {this.rate == \audio} and: {this.bus.isAudioOut},{
			//"reallocating bus".debug;
			this.bus = Bus.alloc(this.rate,this.server,this.numChannels);
		});
		this.patchOut.connectTo(patchIn,needsValueSetNow)
	}
	disconnect {
		patchOut.disconnect;
	}
	
	
/*  RECORDING ETC.
	scope 	{ Synth.scope({ Tempo.setTempo; this.ar }) }
	fftScope 	{ Synth.fftScope({ Tempo.setTempo; this.ar }) }
*/

/*
	record	{ arg pathName,headerFormat='AIFF',sampleFormat='int24',
				agroup,atTime;
		var bsize,recdef,b;

		if(this.isPlaying.not,{
			bsize = this.prepareForPlay(agroup,true) / 15.0;
			//check if built/loaded
			recdef = SynthDef("_cxrecplayer" ++ this.numChannels,{ arg in,bufnum;
				var inar;
				inar = In.ar(in,this.numChannels);
				DiskOut.ar(bufnum,inar);
				// should be able to decide where the output goes
				Out.ar(0,inar);
			});
			recdef.send(this.server);
			(bsize).wait;
			
			// start play/recording
			b = CXBundle.new;
			this.spawnToBundle(b);
			recsynth = Synth.newToBundle(b,"_cxrecplayer" ++ this.numChannels,
						[0,this.bus.index,1,recbuf.bufnum],
						this.group,\addToTail);
			
			// subtract bsize
			if(atTime.notNil, { atTime = max(0,(atTime - bsize)); });
			b.send(this.server,atTime);

			// something has to hang on to the recbuf and the recsynth
		});
	}
*/

/*
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

	ar {
		^this.subclassResponsibility(thisMethod)
	}
	kr { ^this.ar }
	value {  ^this.ar }
	valueArray { ^this.value }
//	inAr {
//		// only works immediately in  { }.play
//		// for quick experimentation, does not encourage reuse
//		// ideally would add itself as a child to the current InstrSynthDef
//		this.play;
//		^In.ar(this.busIndex,this.numChannels)
//	}
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

	path_ { arg p; 
		path = if(p.isNil,p ,{
			path = PathName(p).asRelativePath
		}) 
	}

	// structural utilities
	children { ^#[] }
	deepDo { arg function;// includes self
		function.value(this);
		this.children.do({arg c; 
			var n;
			n = c.tryPerform(\deepDo,function);
			if(n.isNil,{ function.value(c) });
		});
	}	 
	allChildren { 
		var all;
		all = Array.new;
		this.deepDo({ arg child; all = all.add(child) });
		^all
		// includes self
	}
	
	asCompileString { // support arg sensitive formatting
		var stream;
		stream = PrettyPrintStream.on(String(256));
		this.storeOn(stream);
		^stream.contents
	}
	storeParamsOn { arg stream;
		// anything with a path gets stored as abreviated
		var args,tabs=0;
		args = this.storeArgs;
		if(args.notEmpty,{
			if(stream.isKindOf(PrettyPrintStream),{
				stream.storeArgs( enpath(args) );
			},{
				stream << "(" <<<* enpath(args) << ")"
			});
		})
	}
	// using the arg passing version
	changed { arg what ... moreArgs;
		dependantsDictionary.at(this).do({ arg item;
			item.performList(\update, this, what, moreArgs);
		});
	}

	gui { arg  ... args; 
		^this.guiClass.new(this).performList(\gui,args);
	}
	
	topGui { arg ... args; 
		^this.guiClass.new(this).performList(\topGui,args);
	}
	
	smallGui { arg  ... args;
		^this.guiClass.new(this).performList(\smallGui,args);
	}
	
	guiClass { ^AbstractPlayerGui }

}


SynthlessPlayer : AbstractPlayer { // should be higher

	var <isPlaying;

	loadDefFileToBundle { }

	spawnToBundle { arg bundle;
		this.children.do({ arg child;
			child.spawnToBundle(bundle);
		});
		bundle.addAction(this,\didSpawn);
	}
	didSpawn {
		//super.didSpawn;
		isPlaying = true;
	}
	free {
		super.free;
		isPlaying = false;
	}
	stop {
		super.stop;
		isPlaying = false;
	}
	releaseToBundle { arg releaseTime = 0.1,bundle;
		bundle.addAction(this,\stop);
	}
	connectToPatchIn { arg patchIn,needsValueSetNow = true;
		this.patchOut.connectTo(patchIn,needsValueSetNow)
	}
}


MultiplePlayers : AbstractPlayer { // abstract

	var <>voices;

	children { ^this.voices }

	rate { ^this.voices.first.rate }
	numChannels { ^this.voices.first.numChannels }

	free {
		this.voices.do({ arg pl;
			pl.free
		});
		super.free;
	}
	stop {
		this.voices.do({ arg pl;
			pl.stop;
		});
		super.stop;
	}
	releaseToBundle { arg releaseTime = 0.1,bundle;
		this.voices.do({ arg pl;
			pl.releaseToBundle(releaseTime,bundle)
		})
	}
}

MultiTrackPlayer : MultiplePlayers { // abstract
	
}

AbstractPlayerProxy : AbstractPlayer { // won't play if source is nil

	var <>source,<isPlaying = false, <isSleeping = true,sharedBus;

	asSynthDef { ^source.asSynthDef }
	synthDefArgs { ^source.synthDefArgs }
	rate { ^source.rate }
	numChannels { ^source.numChannels }
	defName { ^source.defName }
	spawnToBundle { arg bundle; 
		source.spawnToBundle(bundle);
		bundle.addAction(this,\didSpawn);
	}
	didSpawn {
		isPlaying = true;
		if(source.notNil, { isSleeping = false });
	}
	instrArgFromControl { arg control;
		^source.instrArgFromControl(control)
	}
	initForSynthDef { arg synthDef,argi;
		// only used for building the synthDef
		source.initForSynthDef(synthDef,argi)
	}
	connectToPatchIn { arg patchIn, needsValueSetNow=true;
		source.connectToPatchIn(patchIn,needsValueSetNow);
	}
	free {
		super.free;
		isPlaying = false;
		isSleeping = true;
	}
	stop {
		isPlaying = false;
		isSleeping = true;
		super.stop;
	}
	releaseToBundle { arg releaseTime=0.2,bundle;
		source.releaseToBundle(releaseTime,bundle)
	}
	children { ^[source] }
	
	makePatchOut { arg group,private,bus;
		super.topMakePatchOut(group,private,bus);
		if(patchOut.bus.notNil,{ // could be a scalar out
			sharedBus = SharedBus.newFrom(patchOut.bus,this);
			patchOut.bus = sharedBus;
		});
		if(source.notNil,{
			source.makePatchOut(group,private,sharedBus);
		});
	}
	freePatchOut {
		super.freePatchOut;
		if(sharedBus.notNil,{
			sharedBus.releaseBus(this);
			sharedBus = nil;
		});
	}
}

