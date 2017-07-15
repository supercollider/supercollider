// these are internal classes, used by Pmono and PmonoArtic
// they're not likely to work well if you try to use them outside that context

PmonoStream : Stream {
	var	pattern,
		id, server, cleanup, currentCleanupFunc,
		event,
		streamout, name,
		streampairs, endval,
		msgFunc, hasGate, synthLib, desc, schedBundleArray, schedBundle;

	*new { |pattern|
		^super.newCopyArgs(pattern)
	}

	embedInStream { |inevent|
		inevent ?? { ^nil.yield };

		this.prInit(inevent);

		loop {
			if(this.prDoStreams) {
				// always on the first iteration; should not happen thereafter
				if(id.isNil) { this.prInitNode };
				cleanup.update(event);
				inevent = event.yield;
				this.prSetNextEvent(inevent);
			} {
				^cleanup.exit(inevent)
			}
		}
	}

		// private methods abstracted out for the benefit of subclasses
		// you should not use these directly
	prInit { |inevent|
		cleanup = EventStreamCleanup.new;
		streampairs = pattern.patternpairs.copy;
		endval = pattern.patternpairs.size - 1;
		forBy (1, endval, 2) { | i | streampairs[i] = streampairs[i].asStream };

		event = inevent.copy;
		event.use {
			synthLib = ~synthLib ?? { SynthDescLib.global };
			~synthDesc = desc = synthLib.match(pattern.synthName);
			if (desc.notNil) {
				~hasGate = hasGate = desc.hasGate;
				~msgFunc = desc.msgFunc;
			}{
				~msgFunc = ~defaultMsgFunc;
				~hasGate = hasGate = false;
			};
			msgFunc = ~msgFunc;
		}
	}

	prInitNode {
		event.use {
			if (~id.notNil) {
				~type = \monoSet;
				id = ~id;
			} {
				// If the event is a rest, no node would be created
				// so there is no need to add a cleanup function
				// (for a node that will never exist). If a later
				// event is not a rest, ~id will be nil and the
				// cleanup will be set at that time.
				if(event.isRest.not) {
					~type = \monoNote;
					~instrument = pattern.synthName;
					cleanup.addFunction(event, currentCleanupFunc = { | flag |
						if (flag) { (id: id, server: server, type: \off,
							hasGate: hasGate,
							schedBundleArray: schedBundleArray,
							schedBundle: schedBundle).play
						};
						currentCleanupFunc = nil;
					});
				};
			};
			// this should happen whether or not ~id is nil
			~updatePmono = { | argID, argServer |
				id = argID;
				server = argServer;
				schedBundleArray = ~schedBundleArray;
				schedBundle = ~schedBundle;
			};
		};
	}

	prDoStreams {
		forBy (0, endval, 2) { | i |
			name = streampairs[i];
			streamout = streampairs[i+1].next(event);
			streamout ?? { ^false };
			if (name.isSequenceableCollection) {
				name.do { | n, i | event[n] = streamout[i] };
			}{
				event[name] = streamout;
			};
		};
		^true
	}

	prSetNextEvent { |inevent|
		event = inevent.copy;
		event.use{
			~server = server;
			~id = id;
			~type = \monoSet;
			~msgFunc= msgFunc;
		};
	}
}

PmonoArticStream : PmonoStream {
	embedInStream { |inevent|
		var	sustain, rearticulating = true;
		inevent ?? { ^nil.yield };

		this.prInit(inevent);

		loop {
			if(this.prDoStreams) {
				if(rearticulating and: { event.isRest.not }) {
					event[\id] = nil;
					this.prInitNode;
					rearticulating = false;
				};
				sustain = event.use { ~sustain.value };
				if(sustain.notNil and: { sustain < event.delta }) {
					event[\removeFromCleanup] = event[\removeFromCleanup].add(currentCleanupFunc);
					thisThread.clock.sched(sustain, {
						currentCleanupFunc.value(true);
						rearticulating = true;
					});
				};
				cleanup.update(event);
				inevent = event.yield;
				this.prSetNextEvent(inevent);
			} {
				^cleanup.exit(inevent)
			}
		}
	}
}
