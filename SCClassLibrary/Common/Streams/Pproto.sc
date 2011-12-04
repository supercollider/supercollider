Pfpar : ListPattern {
	initStreams { arg priorityQ;
		list.do({ arg pattern, i;
			priorityQ.put(0.0, pattern.asStream);
		});
	}
	asStream { | cleanup| ^Routine({ arg inval; this.embedInStream(inval, cleanup) }) }

	embedInStream { arg inval, cleanup;
		var assn;
		var priorityQ = PriorityQueue.new;
		cleanup ?? { cleanup = EventStreamCleanup.new };

		repeats.value(inval).do({ arg j;
			var outval, stream, nexttime, now = 0.0;

			this.initStreams(priorityQ);

			// if first event not at time zero
			if (priorityQ.notEmpty and: { (nexttime = priorityQ.topPriority) > 0.0 }, {
				inval = Event.silent(nexttime, inval).yield;
				now = nexttime;
			});

			while({
				priorityQ.notEmpty
			},{
				stream = priorityQ.pop;
				outval = stream.next(inval).asEvent;
				if (outval.isNil, {
					priorityQ.clear;
					^cleanup.exit(inval);
				},{
					cleanup.update(outval);
					// requeue stream
					priorityQ.put(now + outval.delta, stream);
					nexttime = priorityQ.topPriority;
					outval.put(\delta, nexttime - now);

					inval = outval.yield;
					// inval ?? { this.purgeQueue(priorityQ); ^nil.yield };
					now = nexttime;
				});
			});
		});
		^inval;
	}
}

Pproto  : Pattern {

	var <>makeFunction, <>pattern, <>cleanupFunc;

	*new { | makeFunction, pattern, cleanupFunc|
		^super.newCopyArgs( makeFunction, pattern, cleanupFunc)
	}
	storeArgs { ^[makeFunction,pattern,cleanupFunc] }
	embedInStream { | event |
		var stream,  ev, evType;
		var cleanup, cleanupList, eventCleanupFunc;
		var proto;							// temporary proto event used in allocation
		var makeRoutine;						// routine wrapper for function that makes protoEvent
		var protoEvent;						// protoEvent created by function

// Step 1: generate resources from function
		proto = (
			delta: 0, 						// events occur simultaneously
			finish: { ev = currentEnvironment} 	// get copy of event object actually played
		);
		protoEvent = ();
		makeRoutine = Routine({ protoEvent.make (makeFunction) });

		while {
			(ev = makeRoutine.next(ev)).notNil;
		} {
			event = ev.proto_(proto).yield;
			ev.proto = nil;
			cleanupList = cleanupList.add(ev)
		};

		cleanup = EventStreamCleanup.new;
		eventCleanupFunc = { | flag |
			cleanupList.do { | ev |
				EventTypesWithCleanup.cleanup(ev, flag)
			}
		};
		cleanupFunc = eventCleanupFunc ?? { { | flag | eventCleanupFunc.value(proto, flag) } };
		cleanup.addFunction(event, cleanupFunc);

		stream = Pfpar(pattern.asArray).asStream(cleanup);
		loop {
			ev = event.copy.putAll(protoEvent);
			ev = stream.next(ev) ?? { ^cleanup.exit(event) };
			event = ev.yield;
		};
		^event
	}

}
