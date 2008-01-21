
MIDIClockOut {

	var <>sched,<>port,click,<isPlaying = false;
	
	*new { arg deviceName,portName;
		var sched,port;
		if(deviceName.notNil and: portName.notNil,{
			port = MIDIOut.newByName(deviceName,portName);
		},{
			port = MIDIOut.new(0,MIDIClient.destinations[0].uid);
		});
		if(port.isNil,{
			Error("Device not found " + deviceName + portName).throw;
		});
		sched = BeatSched.new;
		^super.newCopyArgs(sched,port)
	}
	
	start {
		var delta,songBeat = 0.0,quantize=4.0;
		if(isPlaying,{
			"MIDIClockOut is already playing".inform;
			^this
		});
		sched.xqsched(quantize,{
			click = 0;
			if(songBeat.notNil,{ sched.beat = songBeat });
			port.songPtr(sched.beat);
			port.start;
			isPlaying = true;
			this.next;
			CmdPeriod.add(this);
		});
	}
	stop {
		sched.clear;
		port.stop;
		isPlaying = false;
	}
	next {
		// calculate the beat delta to the logical time we should be at for the 
		// number of clocks we have sent.  this means we don't slip due to rounding errors
		// or scheduling errors.
		var currentBeat,currentClick,delta,beat;
		currentBeat = (click / 24).floor;
		currentClick = click - (currentBeat * 24);
		
		beat = sched.beat;
		delta = (currentBeat + ((currentClick + 1) / 24.0)) - beat;

		/*(
			currentBeat:  currentBeat,
			currentClick:  currentClick,
			tempo: Tempo.default.bpm, 
			beat:  beat, // current actual beat
			delta: delta
		).debug;*/
		
		port.midiClock;

		sched.xsched( delta,{
			click = click + 1;
			this.next;
		});
	}
	/*songPtr { arg gotoBeat,atTime;
		
	}*/
	cmdPeriod {
		this.stop;
	}
	destroy {
		CmdPeriod.remove(this);
	}
}





