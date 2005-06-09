
AbstractPlayer : AbstractFunction  { 

	var <path,name;
	
	var <synth,<group,<server,<patchOut,<>readyForPlay = false,<>playIsStarting = false,
		 <status, defName;

	play { arg group,atTime,bus;
		var timeOfRequest;
		if(this.isPlaying,{ ^this });
		timeOfRequest = Main.elapsedTime;
		if(bus.notNil,{ 
			bus = bus.asBus;
			if(group.isNil,{
				server = bus.server;
				this.group = server.asGroup;
			},{	
				this.group = group.asGroup;
				server = this.group.server;
			})
		},{
			this.group = group.asGroup;
			server = this.group.server;
			// leave bus nil
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
				//"prPlay->".debug;
				this.prPlay(atTime,bus,timeOfRequest);
				nil
			});
		},{
			this.prPlay(atTime,bus,timeOfRequest)
		});
		
		CmdPeriod.add(this);
		// this gets removed in stopToBundle
		/*Library.put(AbstractPlayer,\serverDeathWatcher, 
			Updater(server,{ arg s, message;
				if(message == \serverRunning and: {s.serverRunning.not},{
					AppClock.sched(5.0,{ // don't panic to quickly
						if(s.serverRunning.not,{
							"Server dead ?".inform;
							//this.cmdPeriod; // stop everything, she's dead
						})
					})
				});
			})
		);*/
	}
	
	prPlay { arg atTime,bus,timeOfRequest;
		var bundle;
		bundle = MixedBundle.new;
		if(status !== \readyForPlay,{ this.prepareToBundle(group, bundle, false, bus) });
		this.makePatchOut(group,false,bus,bundle);
		this.spawnToBundle(bundle);
		bundle.sendAtTime(this.server,atTime,timeOfRequest);
	}
	
	prepareForPlay { arg agroup,private = false,bus;
		var bundle;
		bundle = MixedBundle.new;
		this.prepareToBundle(agroup,bundle,private,bus);
	
		//^bundle.clumpedSendNow(group.server)
		^bundle.send(group.server)
	}

	prepareToBundle { arg agroup,bundle,private = false, bus, defWasLoaded = false;
		status = \isPreparing;
		bundle.addFunction({
			if(status == \isPreparing,{
				readyForPlay = true;
				status = \readyForPlay;
			})
		});
		group = agroup.asGroup;
		server = group.server;
		if(defWasLoaded.not,{
			this.loadDefFileToBundle(bundle,server);
		});
		this.makePatchOut(group,private,bus,bundle);
		this.makeResourcesToBundle(bundle);
		//this.makeGroupsToBundle(bundle);
		this.prepareChildrenToBundle(bundle);
		
		this.loadBuffersToBundle(bundle);
		this.makePatchOut(group,private,bus,bundle);
		// make extra busses
	}
	makeResourcesToBundle {}
	prepareChildrenToBundle { arg bundle;
		this.children.do({ arg child;
			child.prepareToBundle(group,bundle,true,nil,true);
		});
	}
	loadDefFileToBundle { arg bundle,server;
		var def,bytes,dn;
		// Patch needs to know children numChannels
		// before it can know its own.
		this.children.do({ arg child;
			child.loadDefFileToBundle(bundle,server);
		});
		
		dn = this.defName;
		if(dn.isNil or: {
			Library.at(SynthDef,server,dn.asSymbol).isNil
		},{
			// save it in the archive of the player or at least the name.
			// Patches cannot know their defName until they have built
			def = this.asSynthDef;
			defName = def.name;
			dn = defName.asSymbol;
			bytes = def.asBytes;
			
			bundle.addPrepare(["/d_recv", bytes]);
			
			// InstrSynthDef watches \serverRunning to clear this
			InstrSynthDef.watchServer(server);
			Library.put(SynthDef,server,dn,true);
		});
	}

	loadBuffersToBundle {}
	//makeResourcesToBundle { }
	//freeResourcesToBundle { }

	makePatchOut { arg agroup,private = false,bus,bundle;
		group = agroup.asGroup;
		server = group.server;
		// shouldn't be top unless you are the top out
		this.topMakePatchOut(group,private,bus);
		
		//this.childrenMakePatchOut(group,true,bundle);
	}
	topMakePatchOut { arg agroup,private = false,bus;
		var b;
		this.group = agroup;
		if(patchOut.notNil,{
			if(bus.notNil,{
				this.bus = bus;
				^patchOut
			});
			if(this.rate == \audio,{
				//check if private status changed
				if(private,{
					if(patchOut.hasBus,{
						if(patchOut.bus.isAudioOut,{
							patchOut.bus.free;
						},{
							^patchOut
						});
					});
					patchOut.bus = Bus.audio(group.server,this.numChannels);
					this.annotate(patchOut.bus,"top player but private");
				},{
					if(patchOut.hasBus,{
						if(patchOut.bus.isAudioOut,{ // means is main audio out
							//something is wrong here...
							patchOut.bus.free;
						},{
							^patchOut
						})
					});
					patchOut.bus = Bus(\audio,0,this.numChannels,group.server);
					this.annotate(patchOut.bus,"top player out");
				})
			});
			^patchOut
		},{
			//Patch doesn't know its numChannels or rate until after it makes the synthDef
			if(this.rate == \audio,{// out yr speakers
				if(private,{
					this.setPatchOut(
						AudioPatchOut(this,group,bus 
								?? {
									b = Bus.audio(group.server,this.numChannels);
									this.annotate(b,"top player but private");
									b
								})
						)
				},{
					this.setPatchOut(
						AudioPatchOut(this,group,bus 
								?? {
									b = Bus(\audio,0,this.numChannels,group.server);
									this.annotate(b,"top player out");
									b
								})
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
						this.die("rate is nil");
					});
					this.setPatchOut(
						ScalarPatchOut(this,group,bus)
					)
				});
			});
		});
				
		^patchOut
	}
//	childrenMakePatchOut { arg group,private = true,bundle;
//		this.children.do({ arg child;
//			child.makePatchOut(group,private,nil,bundle)
//		});
//	}
	setPatchOut { arg po; // not while playing
		patchOut = po;
		if(patchOut.notNil,{
			server = patchOut.server;
		});
	}

	spawn { arg atTime,timeOfRequest;
		var bundle;
		bundle = MixedBundle.new;
		this.spawnToBundle(bundle);
		bundle.sendAtTime(this.server,atTime,timeOfRequest);
	}
	spawnOn { arg group,bus, atTime,timeOfRequest;
		var bundle;
		bundle = MixedBundle.new;
		this.spawnOnToBundle(group,bus,bundle);
		bundle.sendAtTime(this.server,atTime,timeOfRequest);
	}


	spawnToBundle { arg bundle;
		bundle.addMessage(this,\didSpawn);
		this.children.do({ arg child;
			child.spawnToBundle(bundle);
		});
		synth = Synth.basicNew(this.defName,server);
		this.annotate(synth,"synth");
		NodeWatcher.register(synth);
		bundle.add(
			synth.addToTailMsg(this.group,this.synthDefArgs)
		);
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
	didSpawn {
		status = \isPlaying;
	}
	
	isPlaying { ^synth.isPlaying ? false }
	cmdPeriod {
		var b;
		CmdPeriod.remove(this);
		b = MixedBundle.new;
		this.stopToBundle(b);
		b.doFunctions;
		// sending the OSC is irrelevant
	}
	// these always call children
	stop { arg atTime,stopSynth = true;
		var b;
		if(server.notNil,{		
			b = MixedBundle.new;
			this.stopToBundle(b,stopSynth);
			b.sendAtTime(server,atTime);
		});
		CmdPeriod.remove(this);
	}
	stopToBundle { arg bundle,stopSynth = true;
		if(status !== \isStopped,{
			this.children.do({ arg child;
				child.stopToBundle(bundle);
			});
			if(stopSynth, { this.freeSynthToBundle(bundle); });
			bundle.addMessage(this,\didStop);
			/*bundle.addFunction({ // actually if somebody else is playing, you shouldn't remove yet
				var sdw;
				sdw = Library.at(AbstractPlayer,\serverDeathWatcher);
				if(sdw.notNil,{
					sdw.remove;
					Library.removeAt(AbstractPlayer,\serverDeathWatcher);
				});
			});*/
			this.freePatchOut(bundle);
			status = \isStopping;
		},{
			//status.debug("already stopped");
		})
	}
	didStop {		
		status = \isStopped;
		NotificationCenter.notify(this,\didStop);
	}
	run { arg flag=true,atTime;
		var msg,b;
		if(synth.notNil,{
			b = MixedBundle.new;
			b.add( synth.runMsg(flag) );
			b.sendAtTime(server,atTime);
		});
		// should call children ?
		// this isn't fully implemented
		// might be quite useful
	}
	release { arg releaseTime,atTime;
		var rb;
		rb = MixedBundle.new;
		this.releaseToBundle(releaseTime,rb);
		rb.sendAtTime(server,atTime);
	}
	releaseToBundle { arg releaseTime,bundle;
		if(synth.notNil,{
			bundle.add(synth.releaseMsg(releaseTime));
		});
		if(releaseTime ? 0.0 > 0.01,{
			bundle.addFunction({
				SystemClock.sched(releaseTime,{ 
					this.stop; 
					nil; 
				})
			});
		},{
			this.stopToBundle(bundle);
		});
	}

	free { arg atTime;
		var bundle;
		bundle = MixedBundle.new;
		this.freeToBundle(bundle);
		bundle.sendAtTime(server,atTime);
	}
	freeToBundle { arg bundle;
		if(status != \freed,{
			if(status == \isPlaying,{
				// sends to all the children
				this.stopToBundle(bundle);
			});
			bundle.addMessage(this,\didFree);
			//bundle.addFunction({ readyForPlay = false; });
			this.freeResourcesToBundle(bundle);
			this.children.do({ arg child;
				child.freeToBundle(bundle);
			});
			status = \isFreeing;
		})
	}
	freeResourcesToBundle {}
	didFree {
		if(status == \isFreeing,{
			readyForPlay = false;
			status = \freed;
			^true
		},{
			"didFree aborted".debug(status);
			^false
		})
	}	
	// these don't call children
	freeSynthToBundle { arg bundle;
		if(synth.isPlaying,{ // ? false
			synth.freeToBundle(bundle);
		});
		//bundle.addFunction({
		//	synth = nil;
		//});
	}
	freePatchOut { arg bundle;
		bundle.addFunction({
			if(status == \isStopping or: {status == \isFreeing } or: {status == \isStopped},{
				patchOut.free; // free the bus
				patchOut = nil;
				group  = nil; 
				//server = nil;
				readyForPlay = false;
			},{
				"aborted free patchout".debug(status)
			})
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
				warn("numChannels mismatch ! this:" + this 
					+ this.numChannels + "vs. supplied bus:" + b);
			});
		});
	}
	group_ { arg g;
		if(g.notNil,{
			group = g.asGroup;
			// if playing, move me
		})
	}
	
	// notifications only needed for top level play
	// or for play inside a socket
	/*stateNotificationsToBundle { arg b;
		b.addFunction({
			playIsStarting = false;
			status = \isSpawning;
			NotificationCenter.notify(this,\didPlay);
		});
	}*/

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
	//for now:  always sending, not writing
	writeDefFile {  arg dir;
		this.asSynthDef.writeDefFile(dir);
		this.children.do({ arg child;
			child.writeDefFile(dir);
		});
	}
	synthDefArgs { 
		^[\out,patchOut.synthArg]		
	}
	defName {
		^defName ?? {this.class.name.asString}
	}
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

	addToSynthDef {  arg synthDef,name;
		// the value doesn't matter, just building the synth def now.
		// value will be passed to the real synth at play time
		synthDef.addIr(name, 0); // \out is an .ir bus index
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
	
	/* UGEN STYLE USAGE */
	ar {
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

	// beat based subclasses need only implement beatDuration
	beatDuration { ^nil } // nil means inf
	timeDuration { var bd;
		bd = this.beatDuration;
		if(bd.notNil,{
			^Tempo.beats2secs(bd)
		},{
			^nil
		});	
	}

	// support Pseq([ aPlayer, aPlayer2],inf) etc.
	// you need to have prepared me and set any busses.
	// i need to have a finite duration.
	embedInStream { arg inval;
		^PlayerEvent(this)
	}
	delta { 	^this.beatDuration	}
	
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

	annotate { arg thing,note;
		if(thing.isKindOf(Node),{
			Library.put(AbstractPlayer, \nodeAnnotations, thing.server, thing.nodeID, this.asString ++ ":" ++ note);
		},{
			if(thing.isKindOf(Bus),{
				Library.put(AbstractPlayer, \busAnnotations, thing.server,thing.rate, thing.index, this.asString + thing.asString ++ ":" ++ note);
			});
		});			
	}
	*getAnnotation { arg thing;
		^if(thing.isKindOf(Node),{
			Library.at(AbstractPlayer, \nodeAnnotations, thing.server, thing.nodeID);
		},{
			if(thing.isKindOf(Bus),{
				Library.at(AbstractPlayer, \busAnnotations, thing.server,thing.rate, thing.index);
			});
		});			
		//^Library.at(AbstractPlayer, node.server, node.nodeID) ? "";
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
		bundle.addMessage(this,\didSpawn);
	}
	didSpawn {
		super.didSpawn;
		isPlaying = true;
	}
	didStop {		
		super.didStop;
		isPlaying = false;
	}
	releaseToBundle { arg releaseTime = 0.1,bundle;
		// children release  ?
		bundle.addMessage(this,\stop);
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

/**
  * this is a basic socket that subclasses can then use to spawn players 
  * on the same bus/group.  This uses a SharedBus which protects itself against
  * getting .freed until the owner class (this) says its okay to free it.
  */
AbstractPlayerProxy : AbstractPlayer { // won't play if source is nil

	var <>source,
		<isPlaying = false, <isSleeping = true, // to be deprec in favor of the higher level state tracking
		sharedBus;
	var <socketStatus=\isSleeping;

	asSynthDef { ^this.source.asSynthDef }
	synthDefArgs { ^this.source.synthDefArgs }
	synthArg { ^this.source.synthArg }
	rate { ^this.source.rate }
	numChannels { ^this.source.numChannels }
	loadDefFileToBundle { arg b,server;
		if(this.source.notNil,{
			this.source.loadDefFileToBundle(b,server)
		})
	}
	defName { ^this.source.defName }
	spawnToBundle { arg bundle; 
		this.source.spawnToBundle(bundle);
		bundle.addMessage(this,\didSpawn);
	}
	didSpawn {
		super.didSpawn;
		isPlaying = true;
		if(this.source.notNil, { 
			socketStatus = \isPlaying;
		});
		//isSleeping = false
	}
	instrArgFromControl { arg control;
		^this.source.instrArgFromControl(control)
	}
	initForSynthDef { arg synthDef,argi;
		// only used for building the synthDef
		this.source.initForSynthDef(synthDef,argi)
	}
	connectToPatchIn { arg patchIn, needsValueSetNow=true;
		this.source.connectToPatchIn(patchIn,needsValueSetNow);
	}
	didStop {
		isPlaying = false;
		isSleeping = true;
		status = \isStopped;
	}
	children { 
		if(this.source.notNil,{
			^[this.source] 
		},{
			^[]
		});
	}
	prepareChildrenToBundle { arg bundle;
		this.children.do({ arg child;
			child.prepareToBundle(group,bundle,true,sharedBus,true);
		});
	}	
	makeResourcesToBundle { arg bundle;
		if(patchOut.hasBus,{ // could be a scalar out
			if(sharedBus.notNil,{ sharedBus.releaseBus(this) });
			sharedBus = SharedBus.newFrom(patchOut.bus,this);
			this.annotate(sharedBus,"sharedBus");
			patchOut.bus = sharedBus;
		});
	}
	freeResourcesToBundle { arg bundle;
		bundle.addFunction({
			if(status == \isStopping or: status == \isFreeing or: status == \isStopped,{
				if(sharedBus.notNil,{
					sharedBus.releaseBus(this);
				});
				sharedBus = nil;
			})
		});
	}		
}

