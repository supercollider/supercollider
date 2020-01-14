// originally conceived for LinkClock (Ableton Link support)
// but this may in theory apply to any clock that answers to
// baseBarBeat_ (e.g. MIDISyncClocks, or BeaconClock)

MeterSync {
	var <clock, <id, <ports;
	var <repeats = 4, <delta = 0.01, lastReceived;
	var addrs, meterChangeResp, meterQueryResp;

	// normally clock.setMeterAtBeat notifies \meter
	// and this watcher should broadcast the change.
	// but the watcher may need to change baseBarBeat internally.
	// this should not broadcast. flag is off in that case
	var broadcastMeter = true;

	*new { |clock, id, ports|
		^super.new.init(clock, id, ports)
	}

	init { |argClock, argId, argPorts|
		// these should not be changed after the fact
		// create a new object instead
		clock = argClock;
		id = argId ?? { 0x7FFFFFFF.rand };
		ports = (argPorts ?? { (57120 .. 57127) }).asArray;
		addrs = ports.collect { |port| NetAddr("255.255.255.255", port) };
		meterQueryResp = OSCFunc({ |msg|
			// because of redundancy ('repeats'), we may receive this message
			// multiple times. Respond only once.
			if(msg != lastReceived) {
				lastReceived = msg;
				this.prBroadcast(
					'/SC_LinkClock/meterReply', id, msg[1], this.enabled.asInteger,
					clock.beats, clock.beatsPerBar, clock.baseBarBeat, clock.beatInBar
				);
			};
		}, '/SC_LinkClock/queryMeter');
		clock.addDependant(this);
		this.enabled = true;  // assume enabled when creating
	}

	free {
		clock.removeDependant(this);
		meterQueryResp.free;
		meterChangeResp.free;
	}

	enabled { ^meterChangeResp.notNil }

	enabled_ { |bool|
		var watcher, foundPeers = false;
		if(bool) {
			if(clock.numPeers == 0) {
				// if there are peers, then we get a \numPeers notification
				watcher = SimpleController(clock)
				.put(\numPeers, { |... args|
					watcher.remove;
					foundPeers = true;
					this.resyncMeter(verbose: clock.numPeers > 0)
				})
				.put(\stop, { watcher.remove });
				SystemClock.sched(0.25, {
					if(foundPeers.not) {
						watcher.remove;
						// some clocks other than Link might not broadcast numPeers
						// but they might have peers anyway. If not, this is a no-op
						this.resyncMeter(verbose: clock.numPeers > 0);
					};
				});
			} {
				this.resyncMeter;
			};
			meterChangeResp = OSCFunc({ |msg|
				if(msg[1] != id and: { msg != lastReceived }) {  // ignore message that I sent
					lastReceived = msg;
					// [2] = beatsPerBar, [3] = remote clock's real time, [4] = remote clock's barline
					// also, 5/8 means maybe setting the barline to a half-beat
					// but we have to round the barline because OSC receipt time is inexact
					// -- get the rounding factor from baseBarBeat.asFraction
					// *and* use my utility method because we do not want to broadcast here
					this.prSetMeterAtBeat(msg[2],
						(clock.beats + msg[4] - msg[3]).round(msg[4].asFraction[1].reciprocal)
					);
				};
			}, '/SC_LinkClock/changeMeter');
		} {
			meterChangeResp.free;
			meterChangeResp = nil;
		};
	}

	repeats_ { |value|
		if(value < 1) {
			Error("Invalid number of repeats '%'".format(value)).throw;
		};
		repeats = value
	}

	delta_ { |value|
		if(value <= 0) {
			Error("Invalid messaging delta '%'".format(value)).throw;
		};
		delta = value
	}

	prSetMeterAtBeat { |newBeatsPerBar, beats|
		var saveFlag = broadcastMeter;
		protect {
			broadcastMeter = false;
			clock.setMeterAtBeat(newBeatsPerBar, beats);
		} { broadcastMeter = saveFlag };
	}

	// must always call action after timeout!
	// resyncMeter depends on this
	queryMeter { |action, timeout = 0.2|
		var replies, resp;
		if(timeout.isKindOf(SimpleNumber).not or: { timeout <= 0 }) {
			Error("Invalid timeout '%' provided".format(timeout)).throw;
		};
		if(clock.numPeers > 0) {
			replies = Dictionary.new;
			// no need to check lastReceived here;
			// Dictionary already collapses replies per id
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
				};
			}, '/SC_LinkClock/meterReply');
			{
				protect {
					this.prBroadcast('/SC_LinkClock/queryMeter', clock.beats);
					(timeout + (repeats * delta)).wait;
				} { resp.free };
				action.value(replies.values);
			}.fork(SystemClock);
		} {
			// no peers, don't bother asking
			// sched is needed to make sure Conditions waiting for this method still work
			SystemClock.sched(0, { action.value(List.new) });
		};
	}

	resyncMeter { |round, verbose = true|
		var replies, cond = Condition.new, newBeatsPerBar, newBase;
		fork {
			// queryMeter should never hang (based on default timeout)
			this.queryMeter { |val|
				replies = val;
				cond.unhang;
			};
			cond.hang;
			replies = replies.select { |reply| reply[\syncMeter] };
			if(replies.size > 0) {
				#newBeatsPerBar, newBase = this.prGetMeter(replies, round);
				if(verbose and: { newBeatsPerBar != clock.beatsPerBar }) {
					"syncing meter to %, base = %\n".postf(newBeatsPerBar, newBase)
				};
				this.prSetMeterAtBeat(newBeatsPerBar, newBase);  // local only
				clock.changed(\resynced, true);
			} {
				if(verbose) {
					"(%) Found no SC Link peers synchronizing meter; cannot resync".format(id).warn;
				};
				clock.changed(\resynced, false);  // esp. for unit test
			}
		}
	}

	adjustMeterBase { |localBeats, remoteBeats, round = 1|
		// 'this.prSetMeterAtBeat' to avoid \meter notification
		this.prSetMeterAtBeat(clock.beatsPerBar,
			clock.baseBarBeat + ((localBeats - remoteBeats) % clock.beatsPerBar).round(round)
		);
	}

	update { |obj, what|
		switch(what)
		{ \meter } {
			if(broadcastMeter) {
				this.prBroadcast(
					'/SC_LinkClock/changeMeter',
					id, clock.beatsPerBar, clock.beats, clock.baseBarBeat
				);
			};
		}
		// if the 'model' clock is stopped, no need to keep monitoring
		{ \stop } { this.free }
		{ \disableMeterSync } { this.free };
	}

	prBroadcast { |... msg|
		var saveFlag;
		{
			repeats.do {
				// it is not safe to modify NetAddr.broadcastFlag
				// for this entire loop because we can't block other threads
				// from using it. So, do it atomically for each repeat.
				saveFlag = NetAddr.broadcastFlag;
				protect {
					NetAddr.broadcastFlag = true;
					addrs.do { |addr|
						addr.sendMsg(*msg);
					};
				} { NetAddr.broadcastFlag = saveFlag };
				delta.wait;
			};
		}.fork(SystemClock);
	}

	prGetMeter { |replies, round|
		var bpbs, baseBeats, denom, newBeatsPerBar, newBase;
		bpbs = Set.new;
		baseBeats = Set.new;
		denom = 1;
		replies.do { |reply|
			denom = lcm(denom, reply[\baseBarBeat].asFraction[1]);
			bpbs.add(reply[\beatsPerBar]);
			baseBeats.add(reply[\queriedAtBeat] - reply[\beatInBar]);
		};
		if(round.isNil) { round = denom.reciprocal };
		// All 'syncMeter' peers should have the same beatsPerBar (size == 1).
		// But, if something failed, maybe there are more.
		if(bpbs.size == 1) {
			// 'pop' = easy way to get one item from an unordered collection
			newBeatsPerBar = bpbs.pop;
			// Collapse remote baseBeats to minimum size
			baseBeats = baseBeats.collect { |x| x.round(round) % newBeatsPerBar };
			if(baseBeats.size == 1) {
				// 'baseBeats.add()' above has calculated baseBarBeat
				// such that my local beatInBar will match theirs
				newBase = baseBeats.pop;
			} {
				// this should not happen
				Error("LinkClock peers disagree on barline positions; cannot sync barlines").throw;
			};
		} {
			Error("LinkClock peers disagree on beatsPerBar; cannot sync barlines").throw;
		};
		^[newBeatsPerBar, newBase]
	}

	clock_ { ^this.shouldNotImplement(thisMethod) }
}
