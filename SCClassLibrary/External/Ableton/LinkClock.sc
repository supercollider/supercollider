// clock with Ableton Link synchronization

LinkClock : TempoClock {
	var <syncMeter = false, <id,
	meterChangeResp, meterQueryResp;

	*newFromTempoClock { |clock|
		^super.new(
			clock.tempo,
			clock.beats,
			clock.seconds,
			clock.queue.maxSize
		).prInitFromTempoClock(clock)
	}

	init { |tempo, beats, seconds, queueSize|
		super.init(tempo, beats, seconds, queueSize);
		id = 0x7FFFFFFF.rand;  // to ignore 'self' queries
		// message includes a 'trial' counter
		meterQueryResp = OSCFunc({ |msg|
			NetAddr.broadcastFlag = true;
			// maybe some peers had to take a different port
			(57120 .. 57127).do { |port|
				NetAddr("255.255.255.255", port).sendMsg(
					'/LinkClock/meterReply', id, msg[1], syncMeter.asInteger,
					this.beats, this.beatsPerBar, this.baseBarBeat, this.beatInBar
				);
			};
		}, '/LinkClock/queryMeter');
	}

	syncMeter_ { |bool|
		syncMeter = bool;
		// do sync now?
	}

	queryMeter { |action|
		var replies = IdentityDictionary.new,
		peers = this.numPeers,
		resp = OSCFunc({ |msg, time, addr|
			if(msg[1] != id) {
				replies.put(msg[1], (
					id: msg[1],
					trial: msg[2],
					syncMeter: msg[3].asBoolean,
					beats: msg[4],
					beatsPerBar: msg[5],
					baseBarBeat: msg[6],
					beatInBar: msg[7]
				));
				if(replies.size == peers) {
					routine.stop;
					action.value(replies.values);
				};
			};
		}, '/LinkClock/meterReply'),
		routine = {
			NetAddr.broadcastFlag = true;
			inf.do { |trial|
				// maybe some peers had to take a different port
				(57120 .. 57127).do { |port|
					NetAddr("255.255.255.255", port).sendMsg('/LinkClock/queryMeter', trial);
				};
				0.1.wait;
			};
		}.fork(this);
	}

	stop {
		meterQueryResp.free;
		meterChangeResp.free;
		super.stop;
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
		_LinkClock_GetQuantum;
		^this.primitiveFailed
	}

	quantum_ { |quantum|
		_LinkClock_SetQuantum;
		^this.primitiveFailed
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
