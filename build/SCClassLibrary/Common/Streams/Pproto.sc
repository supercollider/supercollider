
Pproto  : Pattern {
	
	var <>makeFunction, <>pattern, <>cleanupFunc;
	
	*new { | makeFunction, pattern, cleanupFunc|
		^super.newCopyArgs( makeFunction, pattern, cleanupFunc)
	}
		
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

		stream = pattern.asStream;
		loop {
			ev = event.copy.putAll(protoEvent);
			ev = stream.next(ev) ?? { ^cleanup.exit(event) };
			event = ev.yield;
		};
		^event
	}

}
