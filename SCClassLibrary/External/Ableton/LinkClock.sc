// clock with Ableton Link synchronization

LinkClock : TempoClock {
	var syncMeter;

	*newFromTempoClock { |clock|
		^super.new(
			clock.tempo,
			clock.beats,
			clock.seconds,
			clock.queue.maxSize
		).prInitFromTempoClock(clock)
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

	latency {
		_LinkClock_GetLatency
		^this.primitiveFailed
	}

	latency_ { |lat|
		_LinkClock_SetLatency
		^this.primitiveFailed
	}

	quantum {
		_LinkClock_GetQuantum
		^this.primitiveFailed
	}

	quantum_ { |quantum|
		_LinkClock_SetQuantum
		^this.primitiveFailed
	}

	isSyncingMeter { ^syncMeter.notNil }
	getMeterSync { ^syncMeter }

	enableMeterSync { |id, ports|
		if(syncMeter.isNil) {
			syncMeter = MeterSync(this, id, ports);
		};
	}

	disableMeterSync {
		// why not 'syncMeter.free'?
		// you might have created additional instances
		// you shouldn't do that! but we should clean them all up anyway
		this.changed(\disableMeterSync);
		syncMeter = nil;
	}

	// PRIVATE
	prStart { |tempo, beats, seconds|
		_LinkClock_New
		^this.primitiveFailed
	}

	// run tempo changed callback
	prTempoChanged { |tempo, beats, secs, clock|
		this.changed(\tempo);
	}

	prStartStopSync { |isPlaying|
		this.changed(if(isPlaying, \linkStart, \linkStop));
	}

	prNumPeersChanged { |numPeers|
		this.changed(\numPeers, numPeers);
	}

	prInitFromTempoClock { |clock|
		var oldQueue;
		//stop TempoClock and save its queue
		clock.stop;
		oldQueue = clock.queue.copy;
		this.setMeterAtBeat(clock.beatsPerBar, clock.baseBarBeat);

		// queue format is grouped into threes:
		// [size, time0, task0, priority0, time1, task1, priority1, ...]
		// below, then, oldQueue[i] == time(j)
		// and oldQueue[i + 1] = task(j) -- schedAbs copies into the new queue
		forBy(1, oldQueue.size-1, 3) { |i|
			var task = oldQueue[i + 1];
			//reschedule task with this clock
			this.schedAbs(oldQueue[i], task);
		};

		^this
	}
}
