
Sample { 
	// mono or stereo ... not tested for multichannel, will mostly work fine.
	
	var <soundFilePath,<>name,<>soundFile,<tempo=2.0,<beats=4.0;
	
	var <buffer,<patchOut,<readyForPlay = false;
	
	var <beatsize,<size,<end;
	
	// while building the synth def...
	var <>forArgName,bufnumControl;
	
	*new { arg soundFilePath,tempo;
		var new;
		new = super.new;
		new.load(soundFilePath);
		new.calculate;
		new.tempo_(tempo);
		^new
	}

	*newClear {
		^super.new
	}
	
	soundFilePath_ { arg string;
		soundFilePath = string;
		if(soundFilePath.notNil,{
			name=PathName(soundFilePath).fileName.asSymbol;
		});
	}
	load { arg thing;
		if(thing.isNil,{ 
			^this.soundFile_(SoundFile.new);
		});
		if(thing.isNumber,{
			// a clear buffer
			soundFile = SoundFile.new;
				// todo
		});
		if(thing.isKindOf(SoundFile),{
			soundFile=thing;
			soundFilePath= soundFile.path;
			^this
		});
		if(thing.isString,{
			soundFilePath = thing;
			soundFile = SoundFile.new;
			if(soundFile.openRead(soundFilePath),{
				soundFile.close; // got it
			},{
				("Sample failed to load SoundFile at path: " 
					+ soundFilePath).error;
			});
		})
	}
						
	tempo_ { arg tm;
		if(tm.notNil,{ 
			tempo = tm; 
			beats = tempo * (size/soundFile.sampleRate);
			beatsize = size / beats;
			this.changed;
		},{ 
			this.guessBeats 
		});
		// invalidates any synth def made off of me
	}

	beats_	{arg bt;
		beats=bt;
		beatsize = size / beats;
		tempo=beats / (size/soundFile.sampleRate);
		this.changed;
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
	
	numChannels { ^soundFile.numChannels }	
	sampleRate { ^soundFile.sampleRate }
	duration { ^size / this.sampleRate }
	asString { ^(name ?? { super.asString }) }
	// yeah but how many doubles ?
	totalMemory { ^this.numChannels * this.size }

	/* server support */
	prepareForPlay { arg group,bundle;
		patchOut = ScalarPatchOut(this);
		buffer = Buffer.new(group.asGroup.server,-1,this.numChannels);
		// to do, what if i'm empty ?
		bundle.add( buffer.allocReadMsg(this.soundFilePath,0,-1,0,false) )
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
			UGen.buildSynthDef.addIr('__bufnum__',buffer.bufnum);
			bufnumControl = Control.names(['__bufnum__']).ir([buffer.bufnum]);
		}
	}
	bufnumKr {
		// add a secret kr control
		^bufnumControl ?? {
			UGen.buildSynthDef.addKr('__bufnum__',buffer.bufnum);
			bufnumControl = Control.names(['__bufnum__']).kr([buffer.bufnum]);
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
		//ISSUE fixes the synthDef but doesn't let anyone know
		// could tell the def its fixed
		// or keep a buffer on server with buffnums -> tempos
		^this.bufRateScaleKr * temp * tempo.reciprocal
	}
	storeParamsOn { arg stream;
		stream << "(";
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
