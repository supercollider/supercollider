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
	
	*newFrom { arg proxy, numChannels;
		^this.audio(proxy.server, numChannels ? proxy.numChannels).source_({proxy.ar(numChannels) });
	}
	
	
	record { arg path, headerFormat="aiff", sampleFormat="int16";
		var cmd, n;
		cmd = List.new;
		n = this.numChannels;
		if(this.isPlaying && recSynth.isNil, {
			buffer = Buffer(server, 65536, n);
			if(n <= 2, {
				recSynth = Synth.prNew("system-diskout"++n.asString);
				recSynth.newMsg(group,
					[\i_in, bus.index, \i_bufNum, buffer.bufnum],\addAfter);
			}, {
				"multichannel rec doesn't work yet (no completion command bundles)".inform;
				^this;
				/*
				recSynth = Group.newCommand(cmd, group, \addAfter);
				n.do({ arg i;
					 Synth.newCommand(cmd,  "system-diskout1", 
					[\i_in, bus.index+i, \i_bufNum, buffer.bufnum+i], recSynth, \addToTail);
				})
				*/
			});
			buffer.write(path, headerFormat="aiff", sampleFormat="int16", 0, 0, true, cmd.at(0));
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