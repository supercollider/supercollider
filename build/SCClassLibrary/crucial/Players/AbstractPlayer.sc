
AbstractPlayer : AbstractFunction  { 

	var <path,name,<>dirty=true; 
	
	var <synth,<group,<server,<patchOut,<>readyForPlay = false,defName;
		
	play { arg agroup,atTime,bus;
		var bundle,timeOfRequest,sendf;
		
		if(this.isPlaying,{ ^this });
		timeOfRequest = Main.elapsedTime;
		
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

		if(readyForPlay,{
			this.makePatchOut(group,false,bus,bundle);
			this.spawnToBundle(bundle);
			sendf = {	 bundle.sendAtTime(this.server,atTime,timeOfRequest); };
		},{
			sendf = {
				Routine({ //delay till prepared
					(this.prepareForPlay(group,false,bus) / 7.0).wait;
					this.spawn(atTime,timeOfRequest);
				}).play(AppClock)
			};
		});
		if(server.serverRunning.not,{
			server.startAliveThread(0.1,0.4);
			server.waitForBoot({
				if(server.dumpMode != 0,{ 
					server.stopAliveThread;
				});
				InstrSynthDef.clearCache(server);
				if(server.isLocal,{
					InstrSynthDef.loadCacheFromDir(server);
				});
				sendf.value;
				nil
			});
		},sendf);
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
		group = agroup.asGroup;
		this.children.do({ arg child;
			child.prepareToBundle(group,bundle);
		});
		this.loadDefFileToBundle(bundle,group.server);
		
		readyForPlay = true;
	}

	isPlaying { ^synth.isPlaying ? false }
	
	// these always call children
	stop { arg atTime;
		var b;
		if(server.notNil,{		
			b = CXBundle.new;
			this.stopToBundle(b);
			b.sendAtTime(server,atTime);
		})
	}
	stopToBundle { arg b;
		this.children.do({ arg child;
			child.stopToBundle(b);
		});
		if(synth.isPlaying ? false,{
			// what if the bundle is cancelled ?
			b.add(synth.freeMsg);
			synth = nil;
		});
		this.freePatchOut(b);
	}

	free { arg atTime;
		var bundle;
		if(server.notNil,{
			bundle = CXBundle.new;
			this.stopToBundle(bundle);
			this.freeToBundle(bundle);
			bundle.sendAtTime(server,atTime);
		})
	}
	freeToBundle { arg bundle;
		readyForPlay = false;
		this.freeHeavyResources(bundle);
		this.children.do({ arg child;
			child.freeToBundle(bundle);
		});
	}
	
	// these don't call children
	freeSynth {
		if(synth.isPlaying,{
			synth.free;
		});
		synth = nil;
	}
	freePatchOut { arg bundle;
		bundle.addFunction({
			patchOut.free;
			patchOut = nil;
		});
		readyForPlay = false;
	}
	freeHeavyResources { arg bundle; 
		this.children.do({ arg child; child.freeHeavyResources(bundle) })
	}
	
	run { arg flag=true;
		if(synth.notNil,{
			synth.run(flag);
		});
		//should call children ?
	}
	release { arg releaseTime,atTime;
		var rb;
		rb = CXBundle.new;
		this.releaseToBundle(releaseTime,rb);
		rb.sendAtTime(server,atTime);
	}
	releaseToBundle { arg releaseTime,bundle;
		if(synth.notNil,{
			bundle.add(synth.releaseMsg(releaseTime));
		});
		if(releaseTime ? 0.0 > 0.01,{
			bundle.addFunction({
				SystemClock.sched(releaseTime,{ this.stop; nil; })
			});
		},{
			this.stopToBundle(bundle);
		});
	}
		
	busIndex { ^patchOut.index }
	bus { ^patchOut.bus }
	bus_ { arg b;
		if(b.notNil,{
			b = b.asBus(this.rate,this.numChannels,this.server);
			if(patchOut.notNil,{
				if(patchOut.bus != b,{
					patchOut.bus.free; 
				});
				patchOut.bus = b;
			});
			// otherwise we should have had a patchOut
			// and there is nowhere to store this
			
			if(b.numChannels != this.numChannels,{
				warn("numChannels mismatch ! " + this 
					+ this.numChannels + "vs" + b);
			});
		});
	}
	group_ { arg g;
		if(g.notNil,{
			group = g.asGroup;
		})
	}
	
	
	spawn { arg atTime,timeOfRequest;
		var bundle;
		bundle = CXBundle.new;
		this.spawnToBundle(bundle);
		bundle.sendAtTime(this.server,atTime,timeOfRequest);
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
//	spawnOnBus { arg bus,atTime; // deprec.
//		this.spawnOn(bus.server.asGroup,bus,atTime);
//	}
	spawnOn { arg group,bus, atTime,timeOfRequest;
		var bundle;
		bundle = CXBundle.new;
		this.spawnOnToBundle(group,bus,bundle);
		bundle.sendAtTime(this.server,atTime,timeOfRequest);
	}
	spawnOnToBundle { arg agroup,bus,bundle;
		if(patchOut.isNil,{
			this.makePatchOut(agroup,true,bus,bundle);
		},{
			this.bus = bus;
			this.group = agroup;
		});
		this.spawnToBundle(bundle);
	}
	

	/*
		if defName != classname
			when player saves, save defName and secret args (name -> inputIndex)
				that means you can quickly check, load and execute synthdef
					
		save it all in InstrSynthDef (patch is only one with secret args so far)
	*/
	makePatchOut { arg group,private = false,bus,bundle;
		group = group.asGroup;
		server = group.server;
		this.topMakePatchOut(group,private,bus);
		this.childrenMakePatchOut(group,true,bundle);
	}
	topMakePatchOut { arg agroup,private = false,bus;
		this.group = agroup;
		if(patchOut.notNil,{
			if(bus.notNil,{
				this.bus = bus;
				^patchOut
			});
			if(this.rate == \audio,{
				//check if private status changed
				if(private,{
					if(patchOut.bus.notNil,{
						if(patchOut.bus.isAudioOut,{
							patchOut.bus.free;
						},{
							^patchOut
						});
					});
					patchOut.bus = Bus.audio(group.server,this.numChannels);
				},{
					if(patchOut.bus.notNil,{
						if(patchOut.bus.isAudioOut,{
							patchOut.bus.free;
						},{
							^patchOut
						})
					});
					patchOut.bus = Bus(\audio,0,this.numChannels,group.server);
				})
			});
			^patchOut
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
					if(this.rate.isNil,{
						die("Nil rate :",this);
					});
					this.setPatchOut(
						ScalarPatchOut(this,group,bus)
					)
				});
			});
		});
				
		^patchOut
	}
	childrenMakePatchOut { arg group,private = true,bundle;
		this.children.do({ arg child;
			child.makePatchOut(group,private,nil,bundle)
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
			if(Library.at(SynthDef,server,dn).isNil,{
				true
			},{
				//("already loaded:"+dn).debug;
				false
			})
		},{
			// save it in the archive of the player or at least the name.
			// Patches cannot know their defName until they have built
			( "building:" + (this.path ? this) ).debug;
			def = this.asSynthDef;
			defName = def.name;
			dn = defName.asSymbol;
			// for Patch: maybe after building and getting name its already there
			//if(Library.at(SynthDef,server,dn).notNil,{
			//	("already loaded:" + defName).debug;
			//},{			
				bytes = def.asBytes;
				bundle.add(["/d_recv", bytes]);
				// even if name was nil before (Patch), its set now
				("loading def:" + defName).debug;
				// InstrSynthDef watches \serverRunning to clear this
				InstrSynthDef.watchServer(server);
				Library.put(SynthDef,server,dn,true);
				// write for next time
				// def.writeDefFile;
			//});
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
		synthDef.addIr(name,this.synthArg ? 0); // \out is an .ir bus index
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
	didSpawn {}
	rate { ^\audio }
	numChannels { ^1 }
	spec { 
		^if(this.rate == \audio,{
			AudioSpec(this.numChannels)
		},{	
			ControlSpec(-1,1)
			// or trig
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
			this.bus = Bus.alloc(this.rate,this.server,this.numChannels);
		});
		if(patchOut.isNil,{
			"no PatchOut: this object not prepared".error;
			this.dump;
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

	// support Pseq([ aPlayer, aPlayer2],inf) etc.
	// you need to have prepared me and set any busses.
	// i need to have a finite duration.
	embedInStream { arg inval;
		^PlayerEvent(this)
	}
	
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

	save { arg apath;
		var evpath;
		if(File.exists(apath),{
			evpath = apath.escapeChar($ );
			("cp " ++ evpath + evpath ++ ".bak").unixCmd;
		});
		this.asCompileString.write(apath);
		if(path != apath,{ this.didSaveAs(apath); });
	}
	didSaveAs { arg apath;
		path = apath;
		NotificationCenter.notify(AbstractPlayer,\saveAs,[this,path]);
		/* to receive this:
			NotificationCenter.register(AbstractPlayer,\saveAs,you,
			{ arg model,path;
				// do any saveAs handlers you wish
			});
		*/
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
		var args;
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

	guiClass { ^AbstractPlayerGui }

}


SynthlessPlayer : AbstractPlayer { // should be higher

	var <isPlaying=false;

	loadDefFileToBundle { }

	spawnToBundle { arg bundle;
		this.children.do({ arg child;
			child.spawnToBundle(bundle);
		});
		bundle.addAction(this,\didSpawn);
	}
	didSpawn {
		isPlaying = true;
	}
	stopToBundle { arg b;
		super.stopToBundle(b);
		b.addAction(this,\didStop);
	}
	didStop {		
		isPlaying = false;
	}
	releaseToBundle { arg releaseTime = 0.1,bundle;
		// children release  ?
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

//	releaseToBundle { arg releaseTime = 0.1,bundle;
//		this.voices.do({ arg pl;
//			pl.releaseToBundle(releaseTime,bundle)
//		});
//		super.releaseToBundle(releaseTime,bundle);
//	}
}

MultiTrackPlayer : MultiplePlayers { // abstract
	
}

AbstractPlayerProxy : AbstractPlayer { // won't play if source is nil

	var <>source,<isPlaying = false, <isSleeping = true,sharedBus;

	asSynthDef { ^source.asSynthDef }
	synthDefArgs { ^source.synthDefArgs }
	synthArg { ^source.synthArg }
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
	stopToBundle { arg b;
		super.stopToBundle(b);
		b.addAction(this,'didStop');
	}
	didStop {
		isPlaying = false;
		isSleeping = true;
	}

	children { ^[source] }
	
	makePatchOut { arg agroup,private,bus,bundle;
		super.topMakePatchOut(agroup,private,bus,bundle);
		if(patchOut.bus.notNil,{ // could be a scalar out
			sharedBus = SharedBus.newFrom(patchOut.bus,this);
			patchOut.bus = sharedBus;
		});
		this.children.do({ arg child;
			child.makePatchOut(group,private,sharedBus,bundle);
		});
	}
	freePatchOut { arg bundle;
		super.freePatchOut(bundle);
		if(sharedBus.notNil,{
			bundle.addFunction({
				sharedBus.releaseBus(this);
				sharedBus = nil;
			})
		});
	}
}

