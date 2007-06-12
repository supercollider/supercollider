
BufferProxy { // blank space for delays, loopers etc.

	var <buffer,<patchOut,<readyForPlay = false;

	var size=0,<end=0,<>numChannels=1,<>sampleRate=44100.0;

	// move down, no sense up here
	var <>startFrame=0,<>endFrame = -1;

	// while building the synth def...
	var <>forArgi,bufnumControl;


	*new { arg numFrames=44100,numChannels=1,sampleRate=44100.0;
		^super.new
			.startFrame_(0).endFrame_(numFrames - 1)
			.numChannels_(numChannels).sampleRate_(sampleRate)
	}
	storeArgs { ^[this.size,numChannels,sampleRate] }

	size {// actual size loaded on server, not total size
		^if(endFrame == -1,{
			end
		},{
			endFrame - startFrame
		})	
	}
	
	/* server support */
	prepareToBundle { arg group,bundle;
		group = group.asGroup;
		if(buffer.notNil,{
			if(buffer.server != group.server,{
				// this makes me suitable for one server only
				// which is probably always the case
				buffer.free;
				// make buffer
			},{
				// buffer is fine to use
				^this
			})
		});
		
		buffer = Buffer.new(group.asGroup.server,this.size,numChannels);
		buffer.numFrames = this.size;
		buffer.numChannels = numChannels;
		bundle.add( buffer.allocMsg );

		readyForPlay = true;
		patchOut = ScalarPatchOut(this);
	}

	setPatchOut { arg po; patchOut = po }
	free {  
		buffer.free; 
		buffer = nil;
		patchOut = nil;
		readyForPlay = false;
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
	
// no ir yet
//	sampleRateIr {
//		^BufSampleRate.ir(this.bufnumIr)
//	}
		
	bufRateScaleKr {
		^BufRateScale.kr(this.bufnumKr)
	}
	bufFramesKr {
		^BufFrames.kr(this.bufnumKr)
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





Sample : BufferProxy { // a small sound loaded from disk
	
	classvar <soundsDir="sounds/";
	
	var <soundFilePath,<>name,<>soundFile,<beats=4.0,<tempo=1.0;

	var <beatsize;
	
	*new { arg soundFilePath,tempo,startFrame=0,endFrame = -1;
		var new;
		new = super.new;
		new.load(soundFilePath);
		new.tempo_(tempo ? Tempo.tempo);
		if(endFrame.isKindOf(Boolean), { startFrame = 0; endFrame = -1; }); // temp
		new.startFrame_(startFrame).endFrame_(endFrame);
		^new
	}
	storeArgs { ^[ this.class.abrevPath(soundFilePath) ,tempo, startFrame, endFrame ] }
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
			name=PathName(soundFilePath).fileName.asSymbol;
		});
	}
	load { arg thing;
		this.prLoad(thing);
		this.calculate;
		if(buffer.notNil,{ // if already loaded, on server
			// TODO check size and numChannels !!
			if(buffer.numFrames == this.size
				and: {buffer.numChannels == numChannels},{
						buffer.read(this.soundFilePath,startFrame, 
								max(endFrame - startFrame, -1))
			},{
				//discard
				buffer.free;
				buffer = Buffer.read(buffer.server,this.soundFilePath,startFrame);
			});
		});// else wait till prepare
	}
	prLoad { arg thing;
		var pathName;
		
		if(thing.isNil,{ // a blank holder till you load something by gui
			this.soundFile_(SoundFile.new);
			// channels, numFrames...
			size = 44100; // 1 second
			this.calculate;
			this.tempo = Tempo.tempo;
			numChannels = 1;
			sampleRate = 44100.0;
			^this
		});
		if(thing.isKindOf(SoundFile),{
			soundFile=thing;
			this.soundFilePath = this.class.standardizePath( soundFile.path );
			this.calculate;
			this.guessBeats;
			numChannels = soundFile.numChannels;
			sampleRate = soundFile.sampleRate;
			^this
		});
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
			this.guessBeats;
			numChannels = soundFile.numChannels;
			sampleRate = soundFile.sampleRate;
		})
	}
						
	tempo_ { arg tm;
		if(tm.notNil,{ 
			tempo = tm; 
			beats = tempo * (size/soundFile.sampleRate);
			beatsize = size / beats;
		},{ 
			this.guessBeats 
		});
		// invalidates any synth def made off of me
	}

	beats_ { arg bt;
		beats=bt;
		beatsize = size / beats;
		tempo=beats / (size/soundFile.sampleRate);
	}
			
	calculate { 	
		size=soundFile.numFrames;
		end=size-1;
	}
	guessBeats {
		this.beats_(4.0); // this may look wack, but it works very well !
		if(tempo > 4.0,{ this.beats_(beats / 2.0) });
		if(tempo > 4.0,{ this.beats_(beats / 2.0) });
		if(tempo > 4.0,{ this.beats_(beats / 2.0) });
		if(tempo < 0.5,{ this.beats_(beats * 2.0) });
		if(tempo < 0.5,{ this.beats_(beats * 2.0) });		
		if(tempo < 0.5,{ this.beats_(beats * 2.0) });
	}
	
	duration { ^this.size / this.sampleRate }
	asString { ^(name ?? { super.asString }) }
	// yeah but how many doubles ?
	totalMemory { ^numChannels * this.size }
	
	
	pchRatioKr { arg temp;
		// for now, pass in tempo
		
		// InstrSynthDef can support a shared tempo input
		
		// would have to send __mytempo__ when you load buffer and recalc
		// UGen.buildSynthDef.addSecretKr(
		//		("__mytempo__" ++ forArgi.asString).asSymbol,tempo,forArgi,\tempo)
		
		^(this.bufRateScaleKr * temp).madd(tempo.reciprocal)
	}
	
	
	/* server support */
	prepareToBundle { arg group,bundle;
		// TODO keep used Buffers in a map, reuse when path and start/end are identical
		// when start/end are changed, might have to reassign
		
		group = group.asGroup;
		if(buffer.notNil,{
			if(buffer.server != group.server,{
				// this makes me suitable for one server only
				// which is probably always the case
				buffer.free;//.insp("discarding buffer");
				// make buffer
			},{
				// buffer is fine to use
				^this;//.insp("buffer is loaded")
			});
		});

		buffer = Buffer.new(group.asGroup.server,this.size,numChannels);
		if(soundFilePath.notNil,{
			bundle.add( buffer.allocReadMsg(this.soundFilePath,startFrame) )
		},{
			buffer.numFrames = this.size;
			buffer.numChannels = numChannels;
			bundle.add( buffer.allocMsg )
		});
		readyForPlay = true;
		patchOut = ScalarPatchOut(this);
	}
	
	guiClass { ^SampleGui }

}



ArrayBuffer : BufferProxy {

	var <>array;
	
	*new { arg array;
		^super.new(array.size).array_(array.as(Array))
	}
	storeArgs { ^[array] }	

	//this.size wrong ?
	
	prepareToBundle { arg group,bundle;
		buffer = Buffer.new(group.asGroup.server,array.size,numChannels);
		buffer.numFrames = this.size;
		buffer.numChannels = numChannels;
		bundle.add( buffer.allocMsg( buffer.setnMsg(0,array ) ) );
		readyForPlay = true;
		patchOut = ScalarPatchOut(this);
	}

	// gui show it
}
