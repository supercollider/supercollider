
BufferProxy { // blank space for delays, loopers etc.

	var <>buffer,<patchOut,<readyForPlay = false,<server;

	var <>size=0,<>numChannels=1,<>sampleRate=44100.0;

	// while building the synth def...
	var <>forArgi,bufnumControl;

	*new { arg numFrames=44100,numChannels=1,sampleRate=44100.0;
		^super.new.size_(numFrames).numChannels_(numChannels).sampleRate_(sampleRate)
	}
	storeArgs { ^[this.size,numChannels,sampleRate] }

	/* server support */
	prepareToBundle { arg group,bundle;
		group = group.asGroup;
		server = group.server;
		if(readyForPlay,{
			if(buffer.server != server,{
				buffer.free;
				// ...make buffer
			},{
				// buffer is fine to use
				^this
			})
		});

		buffer = Buffer.new(server,this.size,numChannels);
		buffer.numFrames = this.size;
		buffer.numChannels = numChannels;
		bundle.addPrepare( buffer.allocMsg );
		bundle.addFunction({ readyForPlay = true; });
	}
	makePatchOut {
		patchOut = ScalarPatchOut(this);
	}
	free {
		var b;
		b = AbstractPlayer.bundleClass.new;
		this.freeToBundle(b);
		b.sendAtTime(this.server,nil);
	}
	freeToBundle { arg bundle;
		this.freePatchOutToBundle(bundle);
		bundle.addMessage(this,\freeHeavyResources);
		readyForPlay = false;
	}
	freePatchOutToBundle { arg bundle;
		bundle.addFunction({ patchOut.free; patchOut = nil; })
	}
	freeHeavyResources {
		buffer.free; // release bufnum to allocator
		buffer = nil;
	}

	// each synth def, multiple usage in a synth def ok
	initForSynthDef { arg synthDef,argi;
		forArgi = argi;// for buffer etc. to build a control
		bufnumControl = nil;
		// invalidate any other cached ones
		^this
	}
	numFrames { ^size }
	bufnum {
		/*if(InstrSynthDef.buildSynthDef.notNil,{
			("Use bufnumIr, not bufnum to obtain a buffer number inside of a synth def. in:" + InstrSynthDef.buildSynthDef.instrName).warn;
		});*/
		^if(buffer.notNil,{ buffer.bufnum }, nil)
	}

	asUgenInput { ^this.bufnumIr }
	synthArg { ^this.bufnum }

	bufnumIr {
		// add a secret ir control
		^bufnumControl ?? {
			bufnumControl = InstrSynthDef.buildSynthDef.addSecretIr(this,0,\bufnum);
		}
	}

	// the only reason to use the kr version is if you wish to modulate which buffer to use
	bufnumKr {
		// add a secret kr control
		^bufnumControl ?? {
			bufnumControl = InstrSynthDef.buildSynthDef.addSecretKr(this,0,\bufnum);
		}
	}
	sampleRateIr {
		^BufSampleRate.ir(this.bufnumIr)
	}
	// only reason to use kr is if the buffer will be loaded with a new sample
	sampleRateKr {
		^BufSampleRate.kr(this.bufnumKr)
	}

	bufRateScaleKr {
		^BufRateScale.kr(this.bufnumKr)
	}
	bufRateScaleIr {
		^BufRateScale.ir(this.bufnumIr)
	}

	bufFramesKr {
		^BufFrames.kr(this.bufnumKr)
	}
	bufFramesIr {
		^BufFrames.ir(this.bufnumIr)
	}

	bufSamplesKr {
		^BufSamples.kr(this.bufnumKr)
	}
	bufSamplesIr {
		^BufSamples.ir(this.bufnumIr)
	}

	bufDurKr {
		^BufDur.kr(this.bufnumKr)
	}
	bufDurIr {
		^BufDur.kr(this.bufnumIr)
	}

	bufChannelsKr {
		^BufChannels.kr(this.bufnumKr)
	}
	bufChannelsIr {
		^BufChannels.ir(this.bufnumIr)
	}

	rate { ^\noncontrol }
}


AbstractSample : BufferProxy {

	classvar <dir="sounds/";

	var <soundFilePath,<>name,<>startFrame,<>endFrame= -1;

	size {
		// actual size loaded on server, not total size of file
		^if(endFrame == -1,{
			size
		},{
			endFrame - startFrame + 1
		})
	}

	*dir_ { arg p;
		dir = p.standardizePath.withTrailingSlash;
	}
	*standardizePath { arg path;
		var pathName;
		pathName = PathName(path.standardizePath);
		^if(pathName.isRelativePath,{
			dir ++ pathName.fullPath;
		},{
			pathName.fullPath
		})
	}
	*abrevPath { arg path;
		if(path.size < dir.size,{ ^path });
		if(path.copyRange(0,dir.size - 1) == dir,{
			^path.copyRange(dir.size, path.size - 1)
		});
		^path
	}
	abrevPath { ^this.class.abrevPath(this.soundFilePath) }
	soundFilePath_ { arg string;
		soundFilePath = string;
		if(soundFilePath.notNil,{
			name=PathName(soundFilePath).fileName;
		});
	}

	duration { ^this.size / this.sampleRate }
	asString { ^(name ?? { super.asString }) }
	// yeah but how many doubles ?
	totalMemory { ^numChannels * this.size }

	prepareToBundle { arg group,bundle;
		// TODO keep used Buffers in a map, reuse when path and start/end are identical
		// when start/end are changed, would have to reassign
		// use reference counting
		group = group.asGroup;
		server = group.server;
		if(readyForPlay,{
			// buffer is fine to use
			if(buffer.server == server,{ ^this });
			buffer.free;
		});

		buffer = Buffer.new(server,this.size,numChannels);
		if(soundFilePath.notNil,{
			// numFrames already set above, no need for an end
			bundle.addPrepare( buffer.allocReadMsg(this.soundFilePath,startFrame) )
		},{
			//buffer.numFrames = this.size;
			//buffer.numChannels = numChannels;
			bundle.addPrepare( buffer.allocMsg )
		});
		bundle.addFunction({
			readyForPlay = true;
		})
	}

	makePatchOut {
		patchOut = ScalarPatchOut(this);
	}
}

Sample : AbstractSample { // a small sound loaded from disk

	var <>soundFile,<beats=4.0,<tempo=1.0;
	var <beatsize,pchk,beatsizek,tempoi;
	var <end=0; // last possible frame for looping

	*new { arg soundFilePath,tempo,startFrame = 0,endFrame = -1;
		var new;
		new = super.new;
		new.load(soundFilePath,tempo,startFrame,endFrame);
		^new
	}
	// when you intend to .load or record into it later
	*newClear {  arg numFrames=44100,numChannels=1,sampleRate=44100.0;
		^super.new(numFrames,numChannels,sampleRate)
	}

	storeArgs { ^[ this.class.abrevPath(soundFilePath) ,tempo, startFrame, endFrame ] }
	printOn { |stream| this.storeOn(stream) }
	load { arg thing,tempo,argStartFrame = 0,argEndFrame = -1;
		startFrame = argStartFrame;
		endFrame = argEndFrame;
		this.prLoad(thing,tempo);
		this.calculate;
		if(readyForPlay,{ // if already loaded, on server
			if(buffer.server == server
				and: {buffer.numFrames >= this.size}
				and: {buffer.numChannels == numChannels},{
					if(soundFilePath.notNil and: {buffer.server.serverRunning},{
						buffer.read(this.soundFilePath,startFrame,endFrame,0,false);
						readyForPlay = true;
					});
			},{
				buffer.free;
				readyForPlay = false;
				if(buffer.server.serverRunning,{
					buffer = Buffer.alloc(server,size,numChannels);
					{
						buffer.read(this.soundFilePath,startFrame,endFrame,0,false);
						readyForPlay = true;
						nil
					}.defer(0.1);
				}); // else wait for prepare
			});
		},{
			readyForPlay = false;
		});// else wait till prepare
	}
	prLoad { arg thing,t;
		var pathName;
		if(thing.isNil,{ // a blank holder till you load something by gui
			this.soundFile_(SoundFile.new);
			// channels, numFrames...
			this.calculate;
			this.tempo = t ? Tempo.tempo;
			numChannels = 1;
			sampleRate = 44100.0;
			this.guessBeats;
			^this
		});

		// just reading the headers
		// if tempo and endFrame are supplied, avoid loading
		// need sampleRate
		if(thing.isString,{
			this.soundFilePath = this.class.standardizePath( thing );
			soundFile = SoundFile.new;
			if(soundFile.openRead(soundFilePath),{
				soundFile.close; // got it
			},{
				("Sample failed to load SoundFile at path: "
					+ soundFilePath).error;
			});
			this.calculate;
			if(t.notNil,{
				this.tempo = t;
			},{
				this.beats = 4.0;
				this.guessBeats;
			});
			numChannels = soundFile.numChannels;
			sampleRate = soundFile.sampleRate;
			^this
		});

		if(thing.isKindOf(SoundFile),{
			soundFile=thing;
			this.soundFilePath = this.class.standardizePath( soundFile.path );
			this.calculate;
			if(t.notNil,{
				this.tempo = t;
			},{
				this.beats = 4.0;
				this.guessBeats;
			});
			numChannels = soundFile.numChannels;
			sampleRate = soundFile.sampleRate;
			^this
		});
		size = 1;
	}
	reloadBuffer {
		buffer.free;
		buffer = nil;
		if(server.notNil,{
			this.prepareForPlay(server);
		});
	}

	tempo_ { arg tm;
		tempo = tm;
		beats = tempo * (size/soundFile.sampleRate);
		if(beats > 0,{
			beatsize = size / beats;
		},{
			beatsize = 0;
		});
	}
	beats_ { arg bt;
		beats = bt;
		if(beats > 0,{
			beatsize = size / beats;
			tempo = beats / (size/soundFile.sampleRate);
		},{
			beatsize = 0;
			tempo = 0;
		})
	}
	bpm_ { arg bpm; this.tempo_(bpm / 60.0); }
	bpm { ^tempo * 60.0; }
	calculate {
		if(endFrame != -1,{
			size = endFrame - startFrame;
			end = endFrame;
		},{
			size = soundFile.numFrames - startFrame;
			endFrame = end = soundFile.numFrames - 1;
		});
	}
	guessBeats { arg min=80,max=140;
		min = min / 60;
		max = max / 60;
		if(tempo > max,{ this.beats_(beats / 2.0) });
		if(tempo > max,{ this.beats_(beats / 2.0) });
		if(tempo > max,{ this.beats_(beats / 2.0) });
		if(tempo > max,{ this.beats_(beats / 2.0) });
		if(tempo > max,{ this.beats_(beats / 2.0) });
		if(tempo < min,{ this.beats_(beats * 2.0) });
		if(tempo < min,{ this.beats_(beats * 2.0) });
		if(tempo < min,{ this.beats_(beats * 2.0) });
		if(tempo < min,{ this.beats_(beats * 2.0) });
		if(tempo < min,{ this.beats_(beats * 2.0) });
	}
	initForSynthDef { arg synthDef,argi;
		super.initForSynthDef(synthDef,argi);
		beatsizek = pchk = tempoi = nil;
	}
	pchRatioKr {
		^pchk ?? {
			pchk =
				(this.bufRateScaleIr * InstrSynthDef.buildSynthDef.tempoKr(this,\getTempoBus))
					.madd(this.sampleTempoIr.reciprocal)
		}
	}
	getTempoBus {
		^TempoBus(buffer.server).index
	}
	beatsizeIr {
		^beatsizek ?? {
			beatsizek =
				InstrSynthDef.buildSynthDef.addSecretIr( this,beatsize,\beatsize);
		}
	}
	sampleTempoIr {
		^tempoi ?? {
			tempoi = InstrSynthDef.buildSynthDef.addSecretIr(this,tempo,\tempo);
		}
	}
	/*
	sampleTempoKr { // issue: if new sample is loaded, send the tempo to which synths ?
		^samplek ?? {
			samplek = InstrSynthDef.buildSynthDef.addSecretKr(("__stempo__" ++ forArgi.asString).asSymbol,
											tempo,forArgi,\tempo));
		}
	}*/
	prepareToBundle { arg group,bundle;
		super.prepareToBundle(group,bundle);
		if(pchk.notNil,{
			TempoBus(buffer.server).prepareToBundle(group,bundle)
		});
	}

	guiClass { ^SampleGui }

	play { // for testing
		^Patch({ arg sample;
				PlayBuf.ar(sample.numChannels,
						sample.bufnumIr,
						sample.bufRateScaleKr,
						1.0,
						0.0,
						1.0)

			},[
				this
			]).play
	}
	asSignal { arg channel; // nil is stereo interleaved, or 0, 1
		var file,sig;
		file = SoundFile.new;
		file.openRead(this.soundFilePath);
		//Error("Failed to open sound file").throw;
		sig = FloatArray.newClear(file.numFrames * file.numChannels);
		file.readData(sig);
		file.close;
		if(channel.notNil and: {this.numChannels > 1},{
			sig = sig.clump(this.numChannels).flop.at(channel);
		});
		^sig.as(Signal)
	}

}

ArrayBuffer : BufferProxy {

	var <array;

	*new { arg array;
		^super.new(array.size).array_(array.as(Array))
	}
	storeArgs { ^[array] }

	prepareToBundle { arg group,bundle;
		buffer = Buffer.new(group.asGroup.server,array.size,1);
		bundle.add( buffer.allocMsg( buffer.setnMsg(0,array ) ) );
		bundle.addFunction({ readyForPlay = true; });
	}
	makePatchOut {
		patchOut = ScalarPatchOut(this);
	}
	put { arg i,v;
		array.put(i,v);
		if(readyForPlay,{
			buffer.set(i,v);
		})
	}
	at { arg i; ^array.at(i) }
	zero { if(readyForPlay,{ buffer.zero }); array = Array.fill(array.size,0) }
	array_ { arg a;
		var r;
		array = a;
		if(readyForPlay,{
			r = Array.new(array.size * 2);
			array.collect({ arg it,i; r.add(i); r.add(it); });
			buffer.set( *r );
		});
	}
	setArrayToBundle { arg a,b;
		var r;
		array = a;
		r = Array.new(array.size * 2);
		array.collect({ arg it,i; r.add(i); r.add(it); });
		b.add( buffer.setMsg( *r ) );
	}
/*	fill { arg value,bundle;
		array = Array.fill(array.size, value);
		if(bundle.isNil,{ bundle = AbstractPlayer.bundleClass.new; });




	} */

}


