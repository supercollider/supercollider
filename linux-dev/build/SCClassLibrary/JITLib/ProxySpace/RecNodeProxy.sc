RecNodeProxy : NodeProxy {
	var <recSynth, <buffer;
	
	isRecording {
		^(recSynth.notNil && recSynth.isPlaying)
	}
	
	
	*initClass {
		SynthDef("system-diskout2", { arg i_in, i_bufNum=0;
			DiskOut.ar(i_bufNum, InFeedback.ar(i_in, 2));
		}).writeDefFile;
		SynthDef("system-diskout1", { arg i_in, i_bufNum=0;
			DiskOut.ar(i_bufNum, InFeedback.ar(i_in, 1));
		}).writeDefFile;
	}
	
	*newFrom { arg proxy, numChannels, onCompletion;
		^this.audio(proxy.server, numChannels ? proxy.numChannels)
			.put({proxy.ar(numChannels) }, 0, true, onCompletion:onCompletion);
	}
	
	
	record { arg path, headerFormat="aiff", sampleFormat="int16";
		var msg, n;
		
		n = this.numChannels;
		if(this.isPlaying && recSynth.isNil, {
			buffer = Buffer.alloc(server, 65536, n);
			if(n <= 2, {
				recSynth = Synth.prNew("system-diskout"++n.asString, group.server);
				msg = recSynth.newMsg(group,\addAfter,
					[\i_in, outbus.index, \i_bufNum, buffer.bufnum]);
			}, {
				"multichannel rec doesn't work yet (no completion command bundles)".inform;
				^this;
				/*
				recSynth = Group.newCommand(cmd, group, \addAfter);
				n.do({ arg i;
					 Synth.newCommand(cmd,  "system-diskout1", 
					[\i_in, outbus.index+i, \i_bufNum, buffer.bufnum+i], recSynth, \addToTail);
				})
				*/
			});
			buffer.write(path, headerFormat="aiff", sampleFormat="int16", 0, 0, true, msg);
		}, { "cannot record".inform })
	}
	
	pause { 
		if(this.isPlaying && recSynth.notNil, {
			recSynth.run(false);
		})
	}
	
	unpause { 
		if(this.isPlaying && recSynth.notNil, {
			recSynth.run(true);
		})
	}
	
	stop {
		recSynth.free;
		recSynth = nil;
		buffer.tryPerform(\close);
		//buffer.free;
	}
	free {
		super.free;
		this.stop;
	}
	

}