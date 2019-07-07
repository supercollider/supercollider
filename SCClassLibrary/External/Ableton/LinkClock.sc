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
		if(syncMeter) {
			if(this.numPeers == 0) {
				// maybe just started the clock; allow a beat for Link to hook up
				this.sched(1, { this.resyncMeter });
			} {
				this.resyncMeter;
			};
			meterChangeResp = OSCFunc({ |msg|
				if(msg[1] != id) {  // ignore message that I sent
					// [2] = beatsPerBar, [3] = remote clock's real time, [4] = remote clock's barline
					// also, 5/8 means maybe setting the barline to a half-beat
					// but we have to round the barline because OSC receipt time is inexact
					this.setMeterAtBeat(msg[2],
						(this.beats + msg[4] - msg[3]).round(msg[4].asFraction[1].reciprocal),
						false  // responding to meter change should not retransmit
					);
				};
			}, '/LinkClock/changeMeter');
		} {
			meterChangeResp.free;
		};
	}

	setMeterAtBeat { |newBeatsPerBar, beats, broadcast = true|
		// phase relative to the old meter
		// phase at the time of the meter change (new meter) is always 0, not useful
		var oldPhase = (beats - baseBarBeat) % beatsPerBar;
		if(broadcast) {
			NetAddr.broadcastFlag = true;
			(57120..57127).do { |port|
				NetAddr("255.255.255.255", port).sendMsg(
					'/LinkClock/changeMeter', id, newBeatsPerBar, this.beats, beats
				);
			};
		};
		super.setMeterAtBeat(newBeatsPerBar, beats);
	}

	queryMeter { |action|
		var replies = IdentityDictionary.new,
		peers = this.numPeers,
		resp, routine;
		if(peers > 0) {
			resp = OSCFunc({ |msg, time, addr|
				if(msg[1] != id) {
					replies.put(msg[1], (
						id: msg[1],
						queriedAtBeat: msg[2],
						syncMeter: msg[3].asBoolean,
						beats: msg[4],
						beatsPerBar: msg[5],
						baseBarBeat: msg[6],
						beatInBar: msg[7]
					));
					if(replies.size == peers) {
						resp.free;
						routine.stop;
						action.value(replies.values);
					};
				};
			}, '/LinkClock/meterReply');
			routine = {
				NetAddr.broadcastFlag = true;
				inf.do {
					// maybe some peers had to take a different port
					(57120 .. 57127).do { |port|
						NetAddr("255.255.255.255", port).sendMsg('/LinkClock/queryMeter', this.beats);
					};
					0.1.wait;
				};
			}.fork(this);
		} {
			action.value(Set.new)
		};
	}

	adjustMeterBase { |localBeats, remoteBeats, round = 1|
		baseBarBeat = baseBarBeat + ((localBeats - remoteBeats) % beatsPerBar).round(round);
	}

	// if there are peers with syncMeter = true, align my clock to their common barline
	// ignore peers with syncMeter = false
	resyncMeter { |round = 1|
		var replies, cond = Condition.new, bpbs, bibs;
		var myBeats, theirPhase, newBase;
		fork {
			this.queryMeter { |val|
				replies = val;
				cond.unhang;
			};
			cond.hang;
			replies = replies.select { |reply| reply[\syncMeter] };
			if(replies.size > 0) {
				// verify beatsPerBar and beatInBar are common across peers
				bpbs = Set.new;
				bibs = Set.new;
				replies.do { |reply|
					bpbs.add(reply[\beatsPerBar]);
					bibs.add(reply[\beatInBar].round(round));
				};
				if(bpbs.size > 1 or: { bibs.size > 1 }) {
					"Discrepancy among 'syncMeter' Link peers; cannot sync barlines".warn;
				} {
					// do not change beats! do not ever change beats here!
					// calculate baseBarBeat such that my local beatInBar will match theirs
					// 'choose' = easy way to get one item from an unordered collection
					myBeats = replies.choose[\queriedAtBeat].round(round);
					theirPhase = bibs.choose;  // should be only one
					newBase = myBeats - theirPhase;
					"syncing meter to %, base = %\n".postf(bpbs.choose, newBase);
					this.setMeterAtBeat(bpbs.choose, newBase, false);  // false = local only
				};
			} {
				"Found no SC Link peers with syncMeter set to true; cannot resync".warn;
			}
		}
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
