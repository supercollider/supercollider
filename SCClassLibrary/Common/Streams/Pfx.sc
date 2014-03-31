Pfx : FilterPattern {
	var <>fxname, <>pairs;
	*new { arg pattern, fxname ... pairs;
		if (pairs.size.odd, { Error("Pfx should have even number of args.\n").throw });
		^super.new(pattern).fxname_(fxname).pairs_(pairs)
	}
	storeArgs { ^[pattern, fxname] ++ pairs }

	embedInStream { arg inevent;
		var stream, cleanup = EventStreamCleanup.new;
		var server = inevent[\server] ?? { Server.default };
		var id = server.nextNodeID;
		var event = inevent.copy;
		var cleanupEvent;

		pairs.pairsDo {|name, value|
			event[name] = value;
		};
		event[\addAction] = 0; // \addToHead
		event[\instrument] = fxname;
		event[\type] = \on;
		event[\id] = id;
		event[\delta] = 0;

		cleanupEvent = (type: \off, parent: event);
		cleanup.addFunction(event, { |flag|
			if (flag) { cleanupEvent.play }
		});

		inevent = event.yield;
		stream = pattern.asStream;

		loop {
			event = stream.next(inevent) ?? { ^cleanup.exit(inevent) };
			inevent = event.yield;
		};
	}

	isolate {
		^Prout { arg inevent;
			var outputData, synthDesc;

			synthDesc = (inevent[\synthLib] ?? { SynthDescLib.global }).at(fxname);
			if(synthDesc.isNil) { Error("Pfx: SynthDesc not found: %".format(fxname)).throw };
			outputData = synthDesc.outputData;
			if(outputData.size > 1) { Error("Pfx does not support more than one output UGen.").throw };

			if(outputData.isEmpty) {
				this.embedInStream(inevent)
			} {
				outputData = outputData.unbubble;
				if(outputData[\numChannels] > SystemSynthDefs.numChannels) {
					Error("Pfx: SynthDef % has too many channels."
						"Set SystemSynthDefs.numChannels >= %"
						.format(fxname, outputData[\numChannels])).throw
				};

				Pbus(this,
					inevent[\endTime] ? 2.0,
					inevent[\fadeTime] ? 0.02,
					outputData[\numChannels],
					outputData[\rate]
				).embedInStream(inevent)
			}
		}
	}
}

Pfxb : Pfx {
	*new { arg pattern, fxname ... pairs;
		if (pairs.size.odd, { Error("Pfx should have even number of args.\n").throw });
		^super.new(pattern, fxname, *pairs).isolate
	}
}


PAbstractGroup : FilterPattern {
	embedInStream { arg inevent;
		var	server, groupID, event, cleanup;
		var	stream, lag = 0, clock = thisThread.clock,
			groupReleaseTime = inevent[\groupReleaseTime] ? 0.1, cleanupEvent;
		var eventType = this.class.eventType;

		cleanup = EventStreamCleanup.new;
		server = inevent[\server] ?? { Server.default };
		groupID = server.nextNodeID;

		event = inevent.copy;
		event[\addAction] = 0;  // \addToHead
		event[\type] = eventType;
		event[\delta] = 0;
		event[\id] = groupID;

		cleanupEvent = (type: \kill, parent: event);

		cleanup.addFunction(event, { | flag |
			if (flag) { cleanupEvent.lag_(lag - clock.beats + groupReleaseTime).play }
		});
		inevent = event.yield;

		inevent !? { inevent = inevent.copy;
			inevent[\group] = groupID;
		};
		stream = pattern.asStream;
		loop {
			event = stream.next(inevent) ?? { ^cleanup.exit(inevent) };
			lag = max(lag, clock.beats + event.use { ~sustain.value });
			inevent = event.yield;
			inevent.put(\group, groupID);
		}
	}

	*embedLoop { arg inevent, stream, groupID, ingroup, cleanup;
		var event, lag;
		 loop {
			event = stream.next(inevent) ?? { ^cleanup.exit(inevent) };
			lag = event[\dur];
			inevent = event.yield;
			inevent.put(\group, groupID);
		}
	}
}

Pgroup : PAbstractGroup {
	*eventType {
		^\group
	}
}

PparGroup : PAbstractGroup {
	*eventType {
		^\parGroup
	}
}

Pbus : FilterPattern {
	var <>numChannels, <>rate, <>dur, <>fadeTime;

	*new { arg pattern, dur=2.0, fadeTime=0.02, numChannels=2, rate=\audio;
		^super.new(pattern).dur_(dur).numChannels_(numChannels).rate_(rate).fadeTime_(fadeTime)
	}

	storeArgs { ^[ pattern, dur, fadeTime, numChannels, rate ] }

	embedInStream { arg inevent;
		var server, groupID, linkID, bus, ingroup, cleanup;
		var patterns, event, freeBus, stream, cleanupEvent;

		cleanup = EventStreamCleanup.new;
		server = inevent[\server] ?? { Server.default };
		groupID = server.nextNodeID;
		linkID = server.nextNodeID;
		ingroup = inevent[\group];
		dur = dur ?? { server.latency };

		// allocate a new bus number and make a type group event
		// could use a special event type for this:
		if(rate == \audio) {
			bus = server.audioBusAllocator.alloc(numChannels);
			freeBus = { server.audioBusAllocator.free(bus) };
		} {
			bus = server.controlBusAllocator.alloc(numChannels);
			freeBus = { server.controlBusAllocator.free(bus) };
		};

		CmdPeriod.doOnce(freeBus); // also free the bus when cmd-period is run

		event = inevent.copy;
		event[\addAction] = 0; // \addToHead
		event[\type] = \group;
		event[\delta] = 0;
		event[\id] = groupID;
		event[\group] = ingroup;

		// when stream ends, the bus number has to be released
		// so add a function to the cleanup that does this after the link synth has faded out
		cleanup.addFunction(event, { defer(freeBus, fadeTime.abs + dur) });
		// return the group event
		event.yield;

		// allocate a synth that links from the inner bus to the outer bus
		inevent = event = inevent.copy;

		event[\type] = \on;
		event[\group] = groupID;
		event[\addAction] = 3; // \addBefore
		event[\delta] = 0;
		event[\id] = linkID;
		event[\fadeTime] = fadeTime;
		event[\instrument] = format("system_link_%_%", rate, numChannels);
		event[\in] = bus;
		// doneAction = 3; remove and deallocate both this synth and the preceeding node
		// (which is the group).
		event[\msgFunc] = #{ |out, in, fadeTime, gate=1|
			[\out, out, \in, in, \fadeTime, fadeTime, \gate, gate, \doneAction, 3]
		};

		// when stream ends, the synth has to fade out.
		// so add a type-off event to the cleanup
		cleanupEvent = (type: \off, parent: event, fadeTime: fadeTime.abs, hasGate: true, gate: 0);
		cleanup.addFunction(event, { | flag | if(flag) { defer({ cleanupEvent.play }, dur) } });

		// return the link synth event
		inevent = event.yield;

		// now embed the event stream
		stream = Pchain(pattern, (group: groupID, out: bus)).asStream;
		loop {
			// if stream ends, run cleanup
			event = stream.next(inevent) ?? { ^cleanup.exit(inevent) };
			inevent = event.yield;
		}
	}
}
