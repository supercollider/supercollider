// originally conceived for LinkClock (Ableton Link support)
// but this may in theory apply to any clock that answers to
// baseBarBeat_ (e.g. MIDISyncClocks, or BeaconClock)

// TODO OSC message prefix with SC_

SCClockMeterSync {
	var <clock, <>id, <ports;
	var addrs, meterChangeResp, meterQueryResp, meterWatcher;

	// normally clock.setMeterAtBeat notifies \meter
	// and this watcher should broadcast the change.
	// but the watcher may need to change baseBarBeat internally.
	// this should not broadcast. flag is off in that case
	var broadcastMeter = true;

	*new { |clock, id, ports|
		^super.new.init(clock, id, ports)
	}

	init { |argClock, argId, argPorts|
		clock = argClock;
		id = argId ?? { 0x7FFFFFFF.rand };
		this.ports = argPorts;  // see ports_ below
		meterQueryResp = OSCFunc({ |msg|
			this.prBroadcast(
				'/LinkClock/meterReply', id, msg[1], this.enabled.asInteger,
				clock.beats, clock.beatsPerBar, clock.baseBarBeat, clock.beatInBar
			);
		}, '/LinkClock/queryMeter');
		// if the 'model' clock's meter changed,
		// this object must relay that over the network
		meterWatcher = SimpleController(clock)
		.put(\meter, {
			if(broadcastMeter) {
				this.prBroadcast(
					'/LinkClock/changeMeter',
					id, clock.beatsPerBar, clock.beats, clock.baseBarBeat
				);
			};
		})
		// if the 'model' clock is stopped, no need to keep monitoring
		.put(\stop, { this.free });
		this.enabled = true;  // assume enabled when creating
	}

	free {
		meterWatcher.remove;
		meterQueryResp.free;
		meterChangeResp.free;
	}

	ports_ { |argPorts|
		ports = (argPorts ?? { (57120 .. 57127) }).asArray;
		addrs = ports.collect { |port| NetAddr("255.255.255.255", port) };
	}

	enabled { ^meterChangeResp.notNil }

	enabled_ { |bool|
		if(bool) {
			if(clock.numPeers == 0) {
				// maybe just started the clock; allow a beat for Link to hook up
				// you would think it should go onto 'this'
				// but the relationship between beats and seconds is unstable initially
				// in practice it works much better to schedule the sync on a stable clock
				SystemClock.sched(1.0, {
					// clock.numPeers: might have changed in the last 2 seconds
					this.resyncMeter(verbose: clock.numPeers > 0)
				});
			} {
				this.resyncMeter;
			};
			meterChangeResp = OSCFunc({ |msg|
				if(msg[1] != id) {  // ignore message that I sent
					// [2] = beatsPerBar, [3] = remote clock's real time, [4] = remote clock's barline
					// also, 5/8 means maybe setting the barline to a half-beat
					// but we have to round the barline because OSC receipt time is inexact
					// -- get the rounding factor from baseBarBeat.asFraction
					// *and* use my utility method because we do not want to broadcast here
					this.setMeterAtBeat(msg[2],
						(clock.beats + msg[4] - msg[3]).round(msg[4].asFraction[1].reciprocal)
					);
				};
			}, '/LinkClock/changeMeter');
		} {
			meterChangeResp.free;
			meterChangeResp = nil;
		};
	}

	setMeterAtBeat { |newBeatsPerBar, beats|
		var saveFlag = broadcastMeter;
		protect {
			broadcastMeter = false;
			clock.setMeterAtBeat(newBeatsPerBar, beats);
		} { broadcastMeter = saveFlag };
	}

	queryMeter { |action, timeout = 0.2|
		var replies = IdentityDictionary.new,
		resp;
		if(clock.numPeers > 0) {
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
			}, '/LinkClock/meterReply');
			{
				this.prBroadcast('/LinkClock/queryMeter', clock.beats);
				timeout.wait;
				resp.free;
				action.value(replies.values);
			}.fork(SystemClock);
		} {
			// no peers, don't bother asking
			// sched is needed to make sure Conditions waiting for this method still work
			SystemClock.sched(0, { action.value(List.new) });
		};
	}

	resyncMeter { |round, verbose = true|
		var replies, cond = Condition.new, bpbs, baseBeats, denom;
		var newBeatsPerBar, newBase;
		fork {
			this.queryMeter { |val|
				replies = val;
				cond.unhang;
			};
			cond.hang;
			replies = replies.select { |reply| reply[\syncMeter] };
			if(replies.size > 0) {
				// verify beatsPerBar, and myBeats - beatInBar, are common across peers
				// (if the network is glitchy, we may have replies from different trials.
				// but the difference between query time and remote beatInBar should be
				// consistent.)
				// also calculate baseBarBeat common denominator
				bpbs = Set.new;
				baseBeats = Set.new;
				denom = 1;
				replies.do { |reply, i|
					denom = denom lcm: reply[\baseBarBeat].asFraction[1];
					bpbs.add(reply[\beatsPerBar]);
					baseBeats.add(reply[\queriedAtBeat] - reply[\beatInBar]);
				};
				if(round.isNil) { round = denom.reciprocal };
				// Now it gets tricky. We need to reduce baseBeats based on beatsPerBar.
				// All 'syncMeter' peers should have the same beatsPerBar (size == 1).
				// But, if something failed, maybe there are more.
				// So we have to do a two-step check.
				if(bpbs.size == 1) {
					// 'pop' = easy way to get one item from an unordered collection
					newBeatsPerBar = bpbs.pop;
					// Sets, by definition, cannot hold multiple items of equivalent value
					// so this automatically collapses to minimum size!
					baseBeats = baseBeats.collect { |x| x.round(round) % newBeatsPerBar };
					if(baseBeats.size == 1) {
						// do not change beats! do not ever change beats here!
						// 'baseBeats.add()' above has calculated baseBarBeat
						// such that my local beatInBar will match theirs
						newBase = baseBeats.pop;
						if(verbose and: { newBeatsPerBar != clock.beatsPerBar }) {
							"syncing meter to %, base = %\n".postf(newBeatsPerBar, newBase)
						};
						clock.setMeterAtBeat(newBeatsPerBar, newBase);  // local only
					} {
						// this should not happen
						Error("LinkClock peers disagree on barline positions; cannot sync barlines").throw;
					};
				} {
					Error("LinkClock peers disagree on beatsPerBar; cannot sync barlines").throw;
				};
			} {
				if(verbose) {
					"Found no SC Link peers synchronizing meter; cannot resync".warn;
				};
			}
		}
	}

	// legit problem: setMeterAtBeat will broadcast meter change
	// but we don't want that here
	// I will do something naughty in this version and discuss later
	adjustMeterBase { |localBeats, remoteBeats, round = 1|
		clock.slotPut(\baseBarBeat,
			clock.baseBarBeat + ((localBeats - remoteBeats) % clock.beatsPerBar).round(round)
		);
	}

	prBroadcast { |... msg|
		var saveFlag = NetAddr.broadcastFlag;
		protect {
			NetAddr.broadcastFlag = true;
			addrs.do { |addr|
				addr.sendMsg(*msg);
			};
		} { NetAddr.broadcastFlag = saveFlag };
	}
}
