
BufferProxy { // blank space for delays, loopers etc.

	var <buffer,<patchOut,<readyForPlay = false,server;

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
		if(buffer.notNil,{
			if(buffer.server != server,{
				// this makes me suitable for one server only
				// which is probably always the case
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
		bundle.add( buffer.allocMsg );
		readyForPlay = true;
	}
	makePatchOut {
		patchOut = ScalarPatchOut(this);
	}
	free {
		this.freePatchOut;
		this.freeHeavyResources;
		readyForPlay = false;
	}
	freePatchOut { patchOut.free }
	freeHeavyResources { arg bundle;
		buffer.free;
		buffer = nil;
	}
	
	// each synth def, multiple usage in a synth def ok
	initForSynthDef { arg synthDef,argi;
		forArgi = argi;// for buffer etc. to build a control
		bufnumControl = nil;
		// invalidate any other cached ones
		^this
	}

	bufnum { ^buffer.bufnum }
	
	//ISSUE must invalidate these at start of new synth def !!!!!
	bufnumIr {
		// add a secret ir control
		^bufnumControl ?? {
			bufnumControl = UGen.buildSynthDef.addSecretIr(
				("__bufnum__" ++ forArgi.asString).asSymbol,0,forArgi,\bufnum);
		}
	}
	bufnumKr {
		// add a secret kr control
		^bufnumControl ?? {
			bufnumControl = UGen.buildSynthDef.addSecretKr(
				("__bufnum__" ++ forArgi.asString).asSymbol,0,forArgi,\bufnum);
		}
	}
	sampleRateKr {
		^BufSampleRate.kr(this.bufnumKr)
	}
	sampleRateIr {
		^BufSampleRate.ir(this.bufnumIr)
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
	
	bufDurKr {
		^BufDur.kr(this.bufnumKr)
	}
	
	bufChannelsKr {
		^BufChannels.kr(this.bufnumKr)
	}

	rate { ^\scalar }
}


AbstractSample : BufferProxy {

	classvar <soundsDir="sounds/";

	var <soundFilePath,<>name,<>startFrame,<>endFrame= -1;
	
	size {
		// actual size loaded on server, not total size of file
		^if(endFrame == -1,{
			size
		},{
			endFrame - startFrame + 1
		})	
	}
	
	*soundsDir_ { arg dir;
		soundsDir = dir.standardizePath ++ "/";
	}
	*standardizePath { arg path;
		var pathName;
		pathName = PathName.fromOS9(path); // you can drop the os9 if you like
		^if(pathName.isRelativePath,{
			soundsDir ++ pathName.fullPath;
		},{
			pathName.fullPath
		})
	}
	*abrevPath { arg path;
		if(path.size < soundsDir.size,{ ^path });
		if(path.copyRange(0,soundsDir.size - 1) == soundsDir,{
			^path.copyRange(soundsDir.size, path.size - 1)
		});
		^path
	}
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
		if(buffer.notNil,{
			if(buffer.server != server,{
				// this makes me suitable for one server only
				// which is probably always the case
				buffer.free;
				// make buffer
			},{
				// buffer is fine to use
				^this;
			});
		});

		buffer = Buffer.new(server,this.size,numChannels);
		if(soundFilePath.notNil,{
			// numFrames already set above, no need for an end
			bundle.add( buffer.allocReadMsg(this.soundFilePath,startFrame) )
		},{
			buffer.numFrames = this.size;
			buffer.numChannels = numChannels;
			bundle.add( buffer.allocMsg )
		});
		readyForPlay = true;
	}
	makePatchOut {
		patchOut = ScalarPatchOut(this);
	}
	
}


Sample : AbstractSample { // a small sound loaded from disk
	
	
	var <>soundFile,<beats=4.0,<tempo=1.0;

	var <beatsize;
	
	var <end=0; // last possible frame for looping
		
	*new { arg soundFilePath,tempo,startFrame=0,endFrame = -1;
		var new;
		new = super.new;
		new.load(soundFilePath,tempo);
		// backassward sc2 translation
		if(endFrame.isKindOf(Boolean), { startFrame = 0; endFrame = -1; }); 
		new.startFrame_(startFrame).endFrame_(endFrame);
		^new
	}
	storeArgs { ^[ this.class.abrevPath(soundFilePath) ,tempo, startFrame, endFrame ] }

	load { arg thing,tempo;
		this.prLoad(thing,tempo);
		this.calculate;
		if(buffer.notNil,{ // if already loaded, on server
			// TODO check size and numChannels !!
			if(buffer.numFrames == this.size
				and: {buffer.numChannels == numChannels},{
					if(soundFilePath.notNil,{
						buffer.read(this.soundFilePath,startFrame, 
								max(endFrame - startFrame, -1))
					}); // else check if its allocated yet
			},{
				//discard
				buffer.free;
				buffer = Buffer.read(buffer.server,this.soundFilePath,startFrame);
			});
		});// else wait till prepare
	}
	prLoad { arg thing,t;
		var pathName;
		
		if(thing.isNil,{ // a blank holder till you load something by gui
			this.soundFile_(SoundFile.new);
			// channels, numFrames...
			size = 44100; // 1 second
			this.calculate;
			this.tempo = t ? Tempo.tempo;
			numChannels = 1;
			sampleRate = 44100.0;
			this.guessBeats;
			^this
		});

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
		beatsize = size / beats;
	}

	beats_ { arg bt;
		beats = bt;
		beatsize = size / beats;
		tempo = beats / (size/soundFile.sampleRate);
	}
			
	calculate { 	
		size=soundFile.numFrames;
		end=size-1;
	}
	guessBeats {
		if(tempo > 3.0,{ this.beats_(beats / 2.0) });
		if(tempo > 3.0,{ this.beats_(beats / 2.0) });
		if(tempo > 3.0,{ this.beats_(beats / 2.0) });
		if(tempo < 0.5,{ this.beats_(beats * 2.0) });
		if(tempo < 0.5,{ this.beats_(beats * 2.0) });			if(tempo < 0.5,{ this.beats_(beats * 2.0) });
	}
	
	
	
	pchRatioKr { arg temp;
		// for now, pass in tempo
		
		// InstrSynthDef can support a shared tempo input
		
		// would have to send __mytempo__ when you load buffer and recalc
		// UGen.buildSynthDef.addSecretKr(
		//		("__mytempo__" ++ forArgi.asString).asSymbol,tempo,forArgi,\tempo)
		
		^(this.bufRateScaleKr * temp).madd(tempo.reciprocal)
	}
	
	
	/* server support */

	
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
}

ArrayBuffer : BufferProxy {

	var <>array;
	
	*new { arg array;
		^super.new(array.size).array_(array.as(Array))
	}
	storeArgs { ^[array] }	

	prepareToBundle { arg group,bundle;
		buffer = Buffer.new(group.asGroup.server,array.size,1);
		bundle.add( buffer.allocMsg( buffer.setnMsg(0,array ) ) );
		readyForPlay = true;
	}
	makePatchOut {
		patchOut = ScalarPatchOut(this);
	}
	// gui: show it
}
