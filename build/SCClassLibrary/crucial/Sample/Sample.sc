
Sample { 
	// mono or stereo
	
	classvar <soundsDir="sounds/";
	
	var <soundFilePath,<>name,<>soundFile,<tempo=2.0,<beats=4.0;
	
	var <buffer,<>patchOut,<readyForPlay = false;
	
	var <beatsize,<size=0,<end;
	
	// while building the synth def...
	var <>forArgi,bufnumControl;
	
	*new { arg soundFilePath,tempo;
		var new;
		new = super.new;
		new.load(soundFilePath);
		new.tempo_(tempo);
		^new
	}

	*newClear {
		^super.new
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
	soundFilePath_ { arg string;
		soundFilePath = string;
		if(soundFilePath.notNil,{
			name=PathName(soundFilePath).fileName.asSymbol;
		});
	}
	load { arg thing;
		this.prLoad(thing);
		this.calculate;
		// do it right away always ?
		if(buffer.notNil,{
			buffer.read(this.soundFilePath);
		})
	}
	prLoad { arg thing;
		var pathName;
		
		if(thing.isNil,{
			this.soundFile_(SoundFile.new);
			// channels, numFrames...
			size = 44100; // 1 second
			this.calculate;
			this.tempo = Tempo.tempo;
			^this
		});
		if(thing.isNumber,{
			soundFile = SoundFile.new;
			size = thing; // how much to allocate for
					// assumes mono for now !
			this.calculate;
			this.tempo = Tempo.tempo;
			^this
		});
		if(thing.isKindOf(SoundFile),{
			soundFile=thing;
			this.soundFilePath = this.class.standardizePath( soundFile.path );
			this.calculate;
			this.guessBeats;
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
			this.guessBeats
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
	
	sampleRate { ^soundFile.sampleRate }
	duration { ^size / this.sampleRate }
	asString { ^(name ?? { super.asString }) }
	// yeah but how many doubles ?
	totalMemory { ^this.numChannels * this.size }
	numChannels { ^soundFile.numChannels }
	
	/* server support */
	prepareForPlay { arg group,bundle;
		buffer = Buffer.new(group.asGroup.server,-1,this.numChannels);
		if(soundFilePath.notNil,{
			bundle.add( buffer.allocReadMsg(this.soundFilePath,0) )
		},{
			buffer.numFrames = size;
			buffer.numChannels = this.numChannels;
			bundle.add( buffer.allocMsg )
		});
		readyForPlay = true;
		patchOut = ScalarPatchOut(this);
	}
	free {  
		buffer.free; 
		buffer = nil;
		patchOut = nil;
		readyForPlay = false;
	}
	bufnum { ^buffer.bufnum }
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
/*	
	pchRatioKr {
		this.bufRateScaleKr  * this.tempoPlayer * 
		can't kr the tempoPlayer until we have a server.
		can't get a tempo bus until we have a server
		
		can add a secret control, to be fed the tempo bus when
		we have server
	}
	
	tempoPlayer {
		// add a control on the tempo bus
		// return In.kr(
		^(tempoPlayer ?? {
			tempoPlayer = TempoPlayer.new;
		})
	}
*/
	pchRatioKr { arg temp;
		// would have to send __mytempo__ when you load buffer and recalc
		//UGen.buildSynthDef.addSecretKr(
		//		("__mytempo__" ++ forArgi.asString).asSymbol,tempo,forArgi,\tempo)
		
		^this.bufRateScaleKr * temp * tempo.reciprocal
	}
	storeParamsOn { arg stream;
		stream << "(";
		//TODO must abrev the path
		[ soundFilePath ,tempo ].storeItemsOn(stream);
		stream << ")";
	}
	
	guiClass { ^SampleGui }

}

//
//SampleProxy {
//
//	var <sample;
//	
//	*new { arg sample;
//		^super.newCopyArgs(sample)
//	}
//	
//	bufnum {
//		^sample.bufnum
//	}
//	numChannels {
//		^sample.numChannels
//	}
//	numFrames {
//		^BufFrames.ir(sample.bufnum)
//	}
////	sampleRate {
////		
////	}
//
//}


