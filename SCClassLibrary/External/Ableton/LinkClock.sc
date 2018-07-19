// clock with Ableton Link synchronization

LinkClock : TempoClock {

	var <>tempoChanged;

	*newFromTempoClock { |clock|
		^super.new(
			clock.tempo,
			clock.beats,
			clock.seconds,
			clock.queue.maxSize
		).initFromTempoClock(clock)
	}

	initFromTempoClock { |clock|
		var oldQueue;
		//stop TempoClock and save its queue
		clock.stop;
		oldQueue = clock.queue.copy;
		this.setMeterAtBeat(clock.beatsPerBar, 0);

		forBy(1, oldQueue.size-1, 3) { |i|
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

	//override TempoClock primitives
	beats_ { |beats|
		_LinkClock_SetBeats
		^this.primitiveFailed
	}

	setTempoAtBeat { |newTempo, beats|
		_LinkClock_SetTempoAtBeat
		^this.primitiveFailed
	}
	setTempoAtSec { |newTempo, secs|
		_LinkClock_SetTempoAtTime
		^this.primitiveFailed
	}

	setMeterAtBeat { |newBeatsPerBar, beats|
		beatsPerBar = newBeatsPerBar;
		barsPerBeat = beatsPerBar.reciprocal;
		this.prSetQuantum(beatsPerBar);
		this.changed(\meter);
	}

	// PRIVATE
	prStart { |tempo, beats, seconds|
		_LinkClock_New
		^this.primitiveFailed
	}

	prSetQuantum { |quantum|
		_LinkClock_SetQuantum;
		^this.primitiveFailed
	}

	// run tempo changed callback
	prTempoChanged { |tempo, beats, secs, clock|
		tempoChanged.value(tempo, beats, secs, clock);
	}

}

