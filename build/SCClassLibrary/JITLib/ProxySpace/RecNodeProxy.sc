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
	
	record { arg path, headerFormat="aiff", sampleFormat="int24";
		var cmd, n;
		cmd = List.new;
		n = this.numChannels;
		if(this.isPlaying && recSynth.isNil, {
			buffer = Buffer(server, 65536, n);
			if(n <= 2, {
				recSynth = Synth.newCommand(cmd,  "system-diskout"++n.asString, 
					[\i_in, bus.index, \i_bufNum, buffer.bufnum], group, \addAfter);
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
		buffer.close;
		//buffer.free;
	}
	free {
		super.free;
		this.stop;
	}
	

}