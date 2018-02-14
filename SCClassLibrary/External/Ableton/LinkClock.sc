// clock with Ableton Link synchronization

LinkClock : TempoClock {

	var <>tempoChanged;

	*newFromTempoClock { arg clock;
		^super.new(
			clock.tempo,
			clock.beats,
			clock.seconds,
			clock.queue.maxSize
		).initFromTempoClock(clock)
	}

	initFromTempoClock { arg clock;
		var oldQueue;
		//stop TempoClock and save its queue
		clock.stop;
		oldQueue = clock.queue.copy;
		this.setMeterAtBeat(clock.beatsPerBar, 0);

		forBy(1, oldQueue.size-1, 3) {|i|
			var task=oldQueue[i+1];
			//reschedule task with this clock
			this.schedAbs(oldQueue[i], task);
		};

		^this
	}


	numPeers {
		_LinkClock_NumPeers
		^this.primitiveFailed
	}

	//override primitives
	beats_ { arg beats;
		_LinkClock_SetBeats
		^this.primitiveFailed
	}

	// PRIVATE
	prStart { arg tempo, beats, seconds;
		_LinkClock_New
		^this.primitiveFailed
	}

	setTempoAtBeat { arg newTempo, beats;
		_LinkClock_SetTempoAtBeat
		^this.primitiveFailed
	}
	setTempoAtSec { arg newTempo, secs;
		_LinkClock_SetTempoAtTime
		^this.primitiveFailed
	}

	setMeterAtBeat { arg newBeatsPerBar, beats;
		beatsPerBar = newBeatsPerBar;
		barsPerBeat = beatsPerBar.reciprocal;
		this.prSetQuantum(beatsPerBar);
		this.changed(\meter);
	}

	prSetQuantum { arg quantum;
		_LinkClock_SetQuantum;
		^this.primitiveFailed
	}

	//throw tempo changed callback
	prTempoChanged { arg tempo, beats, secs, clock;
		tempoChanged.value(tempo, beats, secs, clock);
	}

}

