
/*
	to do:
		change to triple buffering (rather than buffers for each segment)
		play commands to locate within
*/

AbstractSFP  : AbstractPlayer {

	classvar <dir="SoundFiles/";

	var <>file;

	var segmentBuffers,sched,lastSynth,nextSynth;

	*new { arg file;
		^super.new.file_(loadDocument(file))
	}
	*standardizePath { arg path;
		var pathName;
		pathName = PathName(path.standardizePath);
		^if(pathName.isRelativePath,{
			dir ++ pathName.asRelativePath
		},{
			pathName.fullPath
		})
	}
	*dir_ { arg path;
		dir = path.standardizePath.withTrailingSlash;
	}
	// each subclass should implement timeDuration
	numFrames { ^file.numFrames }

	sampleRate { ^file.sampleRate }
	numChannels { ^file.numChannels }
	preloadData { arg startAt=0,endAt; file.preloadData(startAt,endAt) }
	tempo { ^file.tempo }
	tempo_ { arg tempo; file.tempo_(tempo) }
	firstBeatIsAt { ^file.firstBeatIsAt; }
	firstBeatIsAt_ { arg f; file.firstBeatIsAt_(f); }
	bpm_ { arg b; this.tempo = b / 60.0 }
	fileName { ^file.fileName }
	soundFilePath { ^file.soundFilePath }
	fileDuration { ^file.fileDuration }
	// my own path name if i have it else my sound files path name
	name { ^super.name ?? {this.fileName} }

	timeDuration { ^this.numFrames / this.sampleRate }
	beatDuration { ^this.timeDuration * this.tempo }


	// conversions
	frames2secs { arg frames; ^ frames / this.sampleRate }
	secs2frames { arg secs; ^(secs * this.sampleRate ) }

	// multiple layers ? this will fail !  you'll get multiple translations
	frames2beats { arg frames;  ^(frames - this.firstBeatIsAt / this.sampleRate * this.tempo) }
	beats2frames { arg beats; ^beats * this.tempo.reciprocal  * this.sampleRate + this.firstBeatIsAt }

	relativeBeats2secs { arg beats; ^(beats * (this.tempo.reciprocal )) }
	relativeSecs2beats { arg secs; ^(secs * (this.tempo )) }

	// start beat adjusted (whole track)
	beats2secs { arg beats;
		^this.frames2secs( this.beats2frames(beats) )
	}
	secs2beats { arg secs;
		^this.frames2beats(this.secs2frames(secs) )
	}
	snapSecondToBeat { arg second,beats=4.0;
		^this.beats2secs( this.secs2beats(second).round(beats) )
	}

	// server support
	loadBuffersToBundle { arg bundle;
		// prepare all the preloads
		segmentBuffers = List.new;
		this.preloadData(0,nil,group,bundle,segmentBuffers);
	}
	freeResourcesToBundle { arg bundle;
		segmentBuffers.do({ |b|
			bundle.add( b.freeMsg )
		});
		bundle.addFunction({
			if(file.isOpen,{
				file.close
			})
		})
	}

	asSynthDef {
		^SynthDef(this.defName,{ arg i_bufnum=0,gate=1,out=0;
			Out.ar(out,
				DiskIn.ar(this.numChannels,i_bufnum)
				 * EnvGen.kr(Env.asr(releaseTime:0.1),gate: gate,doneAction: 2)
			)
		})
	}
	defName { ^this.class.name.asString ++ this.numChannels }
	synthDefArgs {
		// first buf to play
		^[\i_bufnum, segmentBuffers.first.bufnum,\gate,1,\out,patchOut.synthArg ]
	}
	didSpawn {
		// start playing the sequence
		// send all the commands now to OSCSched
		var cursor = 0;
		super.didSpawn;
		segmentBuffers.do({ arg sb,i;
			if(i == 0,{
				lastSynth = synth; // the one we started play with
				if(segmentBuffers.size > 1,{
					// the one we will use for the next segment
					nextSynth = Synth.basicNew(this.defName,this.server);
					this.annotate(nextSynth,"synth for segment" + i.asString);
					NodeWatcher.register(nextSynth);
				});
			},{
				sched.xtsched(cursor,this.server,
					[// release previous synth
					[ "/n_set", lastSynth.nodeID,\gate,0],
					// start new synth
					[ nextSynth.addToTailMsg(patchOut.group,
						["i_bufnum",sb,"out",patchOut.synthArg])
					]],{
						// this stepped
						synth = nextSynth;
						synth.isPlaying = true;
				});
				lastSynth = nextSynth;
				nextSynth = Synth.basicNew(this.defName,this.server);
				this.annotate(nextSynth,"synth for segment" + i.asString);
				NodeWatcher.register(nextSynth);
			});
		});
		// ISSUE
		// if we stop before the end, the synths aren't used but they sucked
		// up nodeIDs
	}

	storeArgs {^[file] }
	children { ^[file] }

	guiClass { ^AbstractSFPGui }

}

/**
  * SoundFilePlayer
  * there are (were) other subclasses like SFPLoop, SFPClip that transform an SFP.
  * only SFP has a real concrete sound file
  */
SFP : AbstractSFP  {

	var tempo, <>firstBeatIsAt=0.0;// will be part of the BeatMap
		 // and a real concrete tempo

	var <found = false,<soundFilePath;

	*new { arg path,tempo,firstBeatIsAtFrame=0;
		^super.new.init(path).tempo_(tempo).firstBeatIsAt_(firstBeatIsAtFrame)
	}
	storeArgs { ^[this.soundFilePath,tempo,firstBeatIsAt] }

	*getNew { arg receivingFunction;
		GetFileDialog({ arg ok, path;
			if(ok,{
				{
					if(receivingFunction.notNil,{
						receivingFunction.value(this.new(path));
					},{
						this.new(path).gui;
					});
					nil
				}.defer
			})
		})
	}

	init { arg sfilePath;
		if(sfilePath.isNil,{
			file = SoundFile.new(nil).numChannels_(2);
			name = soundFilePath = nil;
		},{
			// we don't have to check actually
			if(sfilePath.isString,{
				file = SoundFile.new;
				soundFilePath = this.class.standardizePath(sfilePath);
				name = PathName(soundFilePath).fileName;
				found = file.openRead(soundFilePath);
			},{
				if(sfilePath.isKindOf(CXSoundFile),{
					file=sfilePath;
					soundFilePath = file.path = this.class.standardizePath(file.path);
					name = PathName(soundFilePath).fileName;
					found = file.openRead(file.path);
				},{
					die("SFP-init : not a path or a SoundFile " + sfilePath)
				})
			});
			if(found.not,{
				("SFP-init file not found: " + soundFilePath).warn;
			});
		});
	}

	fileNotFound { arg sfilePath;
		// give you a chance to search for it
		ModalDialog({ arg layout;
			CXLabel(layout, "SoundFile not found ! " ++ sfilePath ++ ".  Find it manually ? ");
		},{
			GetFileDialog({ arg ok, sfilePath;
				var it;
				if(ok,{
					this.init(sfilePath);
				})
			})
		});
	}

	preloadData { arg startAt=0,endAt,group,bundle,parentSegmentBuffers;
		var sf;
		// wasteful but...
		// each segment needs its own
		// sharing the preload doesn't work, needs to be reset each time

		// create multiple SoundFiles and put them in the cache

		// if they exist, just repark them

		sf = Buffer.new(group.server,32768,this.numChannels);

		bundle.addPrepare(
			sf.allocMsg(
				sf.cueSoundFileMsg( this.soundFilePath, startAt * file.sampleRate,32768)
			)
		);
		parentSegmentBuffers.add( sf );
	}

	//name { ^this.fileName }
	fileName {
		if(soundFilePath.isNil,{ ^nil });
		^PathName(soundFilePath).fileName
	}
	fileDuration { ^file.duration }
	timeDuration { ^file.duration }

	tempo { ^(tempo ?? {Tempo.tempo}) }
	tempo_ { arg t;
		tempo = t; // ? Tempo.tempo
		if(t.notNil,{
			Tempo.tempo = t;
		});
	}
	children { ^[] }

	guiClass { ^SFPGui }

}

/**
  * Variable speed Sound File Player
  */
VSFP : SFP {

	var tempoClock,myTempo,<pchRatio=1.0;
	var buff,spawnTask;

	pchRatio_ { arg p;
		p = max(p,0.001);
		if(synth.isPlaying,{
			this.server.sendBundle(0.05,
				synth.setMsg(\pchRatio, p);
			);
			SystemClock.sched(0.05,{
				myTempo.tempo = p;
				pchRatio = p;
			});
		},{
			myTempo.tempo = p;
			pchRatio = p;
		})
	}
	init { arg path;
		super.init(path);
		tempoClock = TempoClock.new;
		myTempo = Tempo.new(pchRatio,tempoClock);
		sched = BeatSched.new(SystemClock,myTempo,tempoClock);
	}
	preloadData { arg startAt=0,endAt,group,bundle,parentSegmentBuffers;
		buff = Buffer.new(group.server,44100 * 6,this.numChannels);
		bundle.add(
			buff.allocMsg(
				// not doing startAt yet
				//startAt * file.sampleRate
				buff.readMsg(this.soundFilePath,0,44100 * 6,0,false)
			)
		);
	}
	asSynthDef {
		^SynthDef(this.defName,{ arg out = 0, i_bufnum,pchRatio;
			Out.ar(out,
				PlayBuf.ar(this.numChannels,i_bufnum,pchRatio,loop: 1.0)
			)
		});
	}
	synthDefArgs {
		^[\i_bufnum, buff.bufnum,\pchRatio,myTempo.tempo,\out,patchOut.synthArg ]
	}
	didSpawn {
		//super.didSpawn;
		status = \isPlaying;
		/*
		 * the tank tread algorithm
		 */
		spawnTask = Routine({
			var o;
			var beat = 0,rotator,cursor = 0;
			rotator = [
				//[1,2],
				{
					this.server.sendBundle(0.1,
						buff.readMsg(this.soundFilePath,o, 44100 * 4, 0,false)
					);
				},
				//[3,1],
				{
					this.server.sendBundle(0.1,
						buff.readMsg(this.soundFilePath, o, 44100 * 2, 44100 * 4, false),
						buff.readMsg(this.soundFilePath, o + 88200 , 44100 * 2, 0, false)
					);
				},
				//[2,3]
				{
					this.server.sendBundle(0.1,
						buff.readMsg(this.soundFilePath,o, 44100 * 4, 44100 * 2,false)
					);
				}
			];

			sched.time = 0;
			sched.beat = 0;

			// wait 4 beats
			4.wait;
			beat = beat + 6;

			while({
				o = beat * 44100;
				o < file.numFrames
			},{
				rotator.at(cursor).value;
				cursor = (cursor + 1).wrap(0,2);
				4.wait;
				beat = beat + 4;
			});
			synth.free;
		});
		spawnTask.play(tempoClock)
	}
	didStop {
		super.didStop;
		spawnTask.stop;
		spawnTask = nil;
	}
	currentFrame {
		^(sched.time % 6 * 44100);
	}
	beat { ^sched.beat }
	time { ^sched.time }
}

