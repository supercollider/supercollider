
AbstractPlayer : AbstractFunction  {

	var <path,name;

	var <synth,<group,<server,<patchOut,
		 <status, // nil, isPreparing, readyForPlay, isPlaying,isStopped, isStopping, isFreeing, isFreed
		defName;
	classvar <>bundleClass;

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
			/*
				the hole here is if you manually stop and then restart the server.
				I have no way of knowing that you did that, so the Instr defs are not cleared */
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
		// status must be in one of these states:
		if(status === \isPlaying,{ "already playing".debug });

		bundle = AbstractPlayer.bundleClass.new;
		if(status !== \readyForPlay,{ this.prepareToBundle(group, bundle, false, bus) });
		this.makePatchOut(group,false,bus,bundle);
		this.spawnToBundle(bundle);
		bundle.sendAtTime(this.server,atTime ? this.defaultAtTime,timeOfRequest);
	}
	isPrepared {
		^#[\readyForPlay, \isPlaying,\isStopped, \isStopping].includes(status)
	}
	readyForPlay { ^status === \readyForPlay }
	prepareForPlay { arg group,private = false,bus;
		var bundle;
		bundle = AbstractPlayer.bundleClass.new;
		this.prepareToBundle(group,bundle,private,bus);

		^bundle.send(this.group.server)
	}

	prepareToBundle { arg agroup,bundle,private = false, bus;
		status = \isPreparing;
		bundle.addFunction({
			if(status == \isPreparing,{
				status = \readyForPlay;
			})
		});
		group = agroup.asGroup;
		server = group.server;
		this.loadDefFileToBundle(bundle,server);
		this.makePatchOut(group,private,bus,bundle);
		this.makeResourcesToBundle(bundle);
		//this.makeGroupsToBundle(bundle);
		this.prepareChildrenToBundle(bundle);

		this.loadBuffersToBundle(bundle);
		//this.makePatchOut(group,private,bus,bundle);
		// make extra busses
	}
	makeResourcesToBundle {}
	prepareChildrenToBundle { arg bundle;
		this.children.do({ arg child;
			// and wouldn't it be better if the bundle
			// could just be asked if the def was in there ?
			// this pass thru of defWasLoaded was because of some
			// double loads
			child.prepareToBundle(group,bundle,true,nil);
		});
	}
	loadDefFileToBundle { arg bundle,server;
		var def,bytes,dn;
		// Patch needs to know children numChannels
		// before it can know its own.
		this.children.do({ arg child;
			child.loadDefFileToBundle(bundle,server);
		});
		// yes, I might be producing a stream
		// and I'll pass it to your synthArg
		if(this.rate === 'stream',{ ^this });

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

			// InstrSynthDef watches \serverRunning to clear this if the server severs
			InstrSynthDef.watchServer(server);
			Library.put(SynthDef,server,dn,true);
		});
	}
	// the default behavior for play
	defaultAtTime { ^nil } // immediate
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

	setPatchOut { arg po; // not while playing
		patchOut = po;
		if(patchOut.notNil,{
			server = patchOut.server;
		});
	}

	spawn { arg atTime,timeOfRequest;
		var bundle;
		bundle = AbstractPlayer.bundleClass.new;
		this.spawnToBundle(bundle);
		bundle.sendAtTime(this.server,atTime,timeOfRequest);
	}
	spawnOn { arg group,bus, atTime,timeOfRequest;
		var bundle;
		bundle = AbstractPlayer.bundleClass.new;
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
		b = AbstractPlayer.bundleClass.new;
		this.freeToBundle(b);
		b.doFunctions;
		// sending the OSC is irrelevant since its already root node cleared
	}
	// these always call children
	stop { arg atTime,andFreeResources = true;
		var b;
		// this was requested : the normal user thinks that stop means stop
		// but they want it to also free itself totally
		if(andFreeResources,{ this.free });

		if(server.notNil,{
			b = AbstractPlayer.bundleClass.new;
			this.stopToBundle(b,true);
			b.sendAtTime(server,atTime);
		});
		CmdPeriod.remove(this);
	}
	stopToBundle { arg bundle;
		if([\isStopped,\isStopping,\isFreed,\isFreeing].includes(status).not,{
			status = \isStopping;
			this.children.do({ arg child;
				child.stopToBundle(bundle);
			});
			this.freeSynthToBundle(bundle);
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
		})
	}
	didStop {
		if(status === \isStopping,{
			status = \isStopped;
			NotificationCenter.notify(this,\didStop);
		});
	}
	run { arg flag=true,atTime;
		var msg,b;
		if(synth.notNil,{
			b = AbstractPlayer.bundleClass.new;
			b.add( synth.runMsg(flag) );
			b.sendAtTime(server,atTime);
		});
		// should call children ?
		// this isn't fully implemented
		// might be quite useful
	}
	release { arg releaseTime,atTime;
		var rb;
		rb = AbstractPlayer.bundleClass.new;
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
		if([\isFreed,\isFreeing].includes(status).not,{
			bundle = AbstractPlayer.bundleClass.new;
			this.freeToBundle(bundle);
			bundle.sendAtTime(server,atTime);
		},{
			status.debug("status at free");
		});
	}
	freeToBundle { arg bundle;
		if([\isFreed,\isFreeing].includes(status).not,{
			if(status === \isPlaying,{
				//"freeToBundle, status isPlaying".debug;
				// sends to all the children
				this.stopToBundle(bundle);
			});
			this.children.do({ arg child;
				child.freeToBundle(bundle);
			});
			this.freeResourcesToBundle(bundle);
			status = \isFreeing;
			bundle.addMessage(this,\didFree);
		})
	}
	freeResourcesToBundle {}
	didFree {
		if(status == \isFreeing,{
			status = \isFreed;
			^true
		},{
			^false
		})
	}
	// these don't call children
	freeSynthToBundle { arg bundle;
		if(synth.isPlaying,{ // ? false
			synth.freeToBundle(bundle);
		});
		bundle.addFunction({
			synth = nil;
		});
	}
	freePatchOut { arg bundle;
		bundle.addFunction({
			patchOut.free; // free the bus
			patchOut = nil;
			group  = nil;
			//server = nil;
		});
	}


	busIndex {
		if(patchOut.isNil,{ ^nil });
		^this.patchOut.bus.index
	}
	bus {
		if(patchOut.isNil,{ ^nil });
		^patchOut.bus
	}
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
		if(this.rate.isNil,{ ( this.asString + ": rate is unknown, guessing \control").warn });
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
	// synth arg is the argument value (float,integer) to pass to the synth itself.
	// the synth arg for a player is the index of the bus that it is playing on
	synthArg { ^patchOut.synthArg }
	instrArgFromControl { arg control;
		// a Patch could be either
		this.rate.switch(
			\audio,{
				^In.ar(control,this.numChannels ?? {Error("numChannels is nil"+this).throw})
			},
			\control,{
				^In.kr(control,this.numChannels ?? {Error("numChannels is nil"+this).throw})
			},
			\stream,{
				^control
				//this.synthArg
			},
			{
				Error("AbstractPlayer:instrArgFromControl: rate unknown = "+this.rate).throw
			}
		)
	}

	/* UGEN STYLE USAGE */
	// inside an InstrSynthDef a player can insert itself as a stepchild
	// of the patch and play inline
	ar {
		var dn,synthDef, buildSynthDef;
		// if not built, then build
		dn = this.defName;
		if(dn.isNil or: {Library.at(SynthDef,server,dn.asSymbol).isNil},{
			buildSynthDef = UGen.buildSynthDef;
			synthDef = this.asSynthDef;
			UGen.buildSynthDef = buildSynthDef;
		});
		^UGen.buildSynthDef.playerIn(this)
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
		var b;
		if(this.isPlaying and: {this.rate == \audio} and: {this.bus.isAudioOut},{
			this.bus = b = Bus.alloc(this.rate,this.server,this.numChannels);
			AbstractPlayer.annotate(b,"connected to patch in, made bus");
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
		^thisMethod.notYetImplemented
		//^PlayerEvent(this)
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
		},{
			stream << ".new";
		})
	}

	annotate { arg thing,note;
		this.class.annotate(thing,"owner:" + this.asString + ":" + note);
	}
	*annotate { arg thing,note;
		if(thing.isKindOf(Node),{
			Library.put(AbstractPlayer, \nodeAnnotations,
				thing.server ?? {"node has no server, cannot annotate".die},
				thing.nodeID ?? {"nodeID is nil, cannot annotate".die},
			 	note);
		},{
			if(thing.isKindOf(Bus),{
				Library.put(AbstractPlayer, \busAnnotations,
					thing.server ?? {"Bus has no server, cannot annotate".die},
					thing.rate ?? {"Bus has no rate, cannot annotate".die},
					thing.index ?? {"Bus has no index, cannot annotate".die},
					thing.asString + ":" + note);
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
	*removeAnnotation { arg thing;
		// shouldnt have to do this since its just a lookup by integer indices
/*		if(thing.isKindOf(Node),{
			Library.global.removeAt(AbstractPlayer, \nodeAnnotations,
				thing.server ?? {"node has no server, cannot un-annotate".die},
				thing.nodeID ?? {"nodeID is nil, cannot un-annotate".die}
			 	);
		},{
			if(thing.isKindOf(Bus),{
				Library.global.removeAt(AbstractPlayer, \busAnnotations,
					thing.server ?? {"Bus has no server, cannot un-annotate".die},
					thing.rate ?? {"Bus has no rate, cannot un-annotate".die},
					thing.index ?? {"Bus has no index, cannot un-annotate".die}
					);
			});
		});
*/
	}

	// using the arg passing version
	changed { arg what ... moreArgs;
		dependantsDictionary.at(this).do({ arg item;
			item.performList(\update, this, what, moreArgs);
		});
	}
	*initClass {
		bundleClass = MixedBundle;
	}
	guiClass { ^AbstractPlayerGui }

}


SynthlessPlayer : AbstractPlayer { // should be higher

	var <isPlaying=false;

	loadDefFileToBundle { }
//	prepareToBundle { arg agroup,bundle,private = false, bus, defWasLoaded = false;
//		status = \isPreparing;
//		bundle.addFunction({
//			if(status == \isPreparing,{
//				status = \readyForPlay;
//			})
//		});
//		group = agroup.asGroup;
//		server = group.server;
//		if(defWasLoaded.not,{
//			this.loadDefFileToBundle(bundle,server);
//		});
//		this.makePatchOut(group,private,bus,bundle);
//		this.makeResourcesToBundle(bundle);
//		//this.makeGroupsToBundle(bundle);
//		this.prepareChildrenToBundle(bundle);
//
//		this.loadBuffersToBundle(bundle);
//		//this.makePatchOut(group,private,bus,bundle);
//		// make extra busses
//	}
//
//
//	prepareToBundle { arg agroup,bundle,private = false, bus, defWasLoaded = false;
//		status = \isPreparing;
//		bundle.addFunction({
//			if(status == \isPreparing,{
//				status = \readyForPlay;
//			})
//		});
//		group = agroup.asGroup;
//		server = group.server;
//		if(defWasLoaded.not,{
//			this.loadDefFileToBundle(bundle,server);
//		});
//
//		this.makePatchOut(group,private,bus,bundle);
//		this.makeResourcesToBundle(bundle);
//		//this.makeGroupsToBundle(bundle);
//		this.prepareChildrenToBundle(bundle);
//
//		this.loadBuffersToBundle(bundle);
//		//this.makePatchOut(group,private,bus,bundle);
//		// make extra busses
//	}
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
	isPlaying { ^status == \isPlaying }
	didSpawn {
		super.didSpawn;
		status = \isPlaying;
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
		//isPlaying = false;
		//isSleeping = true;
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
			child.prepareToBundle(group,bundle,true,sharedBus);
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

