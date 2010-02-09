
AbstractPlayer : AbstractFunction  {

	var <path,>name;
	var <synth,<group,<server,<patchOut,defName;
	var <status;
	 // nil, isPreparing, readyForPlay, isPlaying,isStopped, isStopping, isFreeing, isFreed

	classvar <>bundleClass;

	*bundle { |server,atTime,buildFunction|
		var bundle;
		bundle = AbstractPlayer.bundleClass.new;
		buildFunction.value(bundle);
		bundle.sendAtTime(server,atTime);
	}

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
				this.prPlay(atTime,bus,timeOfRequest);
				nil
			});
		},{
			this.prPlay(atTime,bus,timeOfRequest)
		});

		CmdPeriod.add(this);
	}

	prPlay { arg atTime,bus,timeOfRequest;
		var bundle;
		// status must be in one of these states:
		if(status === \isPlaying,{ "already playing".debug });

		bundle = AbstractPlayer.bundleClass.new;
		if(status !== \readyForPlay,{
			this.prepareToBundle(group, bundle, false, bus)
		});
		this.spawnToBundle(bundle);
		bundle.sendAtTime(this.server,atTime ? this.defaultAtTime,timeOfRequest);
	}
	isPrepared {
		^#[\readyForPlay, \isPlaying,\isStopped, \isStopping].includes(status)
	}
	readyForPlay { ^[\readyForPlay,\isPlaying,\isStopped,\isStopping].includes(status) }
	prepareForPlay { arg group,private = false,bus;
		AbstractPlayer.bundle(group.asTarget.server,nil,{ |bundle|
			this.prepareToBundle(group,bundle,private,bus);
		});
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
		this.prepareChildrenToBundle(bundle);

		this.loadBuffersToBundle(bundle);
	}
	makeResourcesToBundle {}
	prepareChildrenToBundle { arg bundle;
		this.children.do({ arg child;
			// wouldn't it be better if the bundle
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
		// this is the only reason we are forcing the children to load
		// their defs here rather then letting them each do it in their own
		// prepareToBundle
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
			// Patches are discarding their cache on purpose
			// just to be on the safe side.
			// at some point I'll tighten this up again and honor the cache for Patch
			//dn.debug("dn was nil");
			// save it in the archive of the player or at least the name.
			// Patches cannot know their defName until they have built
			def = this.asSynthDef;
			defName = def.name;
			//defName.debug("defname");
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
		this.topMakePatchOut(group,private,bus);
	}
	topMakePatchOut { arg agroup,private = false,bus;
		this.group = agroup;
		bus = bus ?? {BusSpec(\audio,this.numChannels,private)};
		if(patchOut.isNil,{
			patchOut = PatchOut(this,group,bus);
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
		// this was requested : the normal user thinks that stop means stop
		// but they want it to also free its resources
		// so really most of the time you mean .free
		if(andFreeResources,{ ^this.free });

		if(server.notNil,{
			AbstractPlayer.bundle(server,atTime ? this.server.latency,{ |bundle|
				this.stopToBundle(bundle,true);
			})
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
			//this.freePatchOutToBundle(bundle);
		})
	}
	didStop {
		if(status === \isStopping,{
			status = \isStopped;
			NotificationCenter.notify(this,\didStop);
		});
	}
	run { arg flag=true,atTime;
		if(synth.notNil,{
			AbstractPlayer.bundle(server,atTime,{ |bundle|
				bundle.add( synth.runMsg(flag) );
			})
		});
		// should call children ?
		// this isn't fully implemented
		// might be quite useful
	}
	release { arg releaseTime,atTime;
		AbstractPlayer.bundle(server,atTime,{ |rb|
			this.releaseToBundle(releaseTime,rb);
		})
	}
	releaseToBundle { arg releaseTime,bundle;
		releaseTime = max(releaseTime ? 0.1,0.01);
		if(synth.isPlaying,{
			bundle.add(synth.releaseMsg(releaseTime));
			bundle.addFunction({
				SystemClock.sched(releaseTime+0.1,{
					this.stop;
					nil;
				})
			});
		},{
			this.stopToBundle(bundle)
		})
	}

	free { arg atTime;
		if([\isFreed,\isFreeing].includes(status).not,{
			AbstractPlayer.bundle(server,atTime,{ |bundle|
				this.freeToBundle(bundle);
			})
		});
	}
	freeToBundle { arg bundle;
		if([\isFreed,\isFreeing].includes(status).not,{
			if(status === \isPlaying,{
				// sends to all the children
				this.stopToBundle(bundle);
			});
			this.children.do({ arg child;
				child.freeToBundle(bundle);
			});
			// these will be the same thing
			this.freeResourcesToBundle(bundle);
			this.freePatchOutToBundle(bundle);

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
	freePatchOutToBundle { arg bundle;
		bundle.addFunction({
			patchOut.free; // frees the busses
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
			if(patchOut.notNil,{
				patchOut.bus = b;
			});
			// otherwise we should have had a patchOut
			// and there is nowhere to store this
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
		var prev;
		prev = this.getAnnotation(thing);
		if(prev.notNil,{
			note = prev ++ ";;" + note;
		});
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
					note);//thing.asString + ":" +
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
		// and its better to know what it once was annotated as (if it failed to be released etc.)
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

}

MultiTrackPlayer : MultiplePlayers { // abstract

}

/**
  * this is a basic socket that subclasses can then use to spawn players
  * on the same bus/group.  This uses a SharedBus which protects itself against
  * getting .freed until the owner class (this) says its okay to free it.
  */
AbstractPlayerProxy : AbstractPlayer { // won't play if source is nil

	var <>source;
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
		socketStatus = \isSleeping;
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
			child.prepareToBundle(group,bundle,true,this.bus);
		});
	}
/*	makeResourcesToBundle { arg bundle;
		if(patchOut.hasBus,{ // could be a scalar out
			//if(sharedBus.notNil,{ sharedBus.releaseBus(this) });
			sharedBus = patchOut.bus; //SharedBus.newFrom(patchOut.bus,this);
			//if(sharedBus.owner === this,{
			//	this.annotate(sharedBus,"sharedBus");
			//});
			//patchOut.bus = sharedBus;
		});
	}
	freeResourcesToBundle { arg bundle;
		bundle.addFunction({
			if(status == \isStopping or: status == \isFreeing or: status == \isStopped,{
				//if(sharedBus.notNil,{
				//	sharedBus.releaseBus(this);
				//});
				sharedBus = nil;
			})
		});
	}
*/
}

