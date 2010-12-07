
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

		this.prInit(inevent)
			.prInitNode;

		loop {
			if(this.prDoStreams) {
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
			} {
				~type = \monoNote; 
				~instrument = pattern.synthName;
				~updatePmono = { | argID, argServer | 
					 id = argID;
					 server = argServer;
					 schedBundleArray = ~schedBundleArray;
					 schedBundle = ~schedBundle;
				};
				cleanup.addFunction(event, currentCleanupFunc = { | flag |
					if (flag) { (id: id, server: server, type: \off, 
						hasGate: hasGate,
						schedBundleArray: schedBundleArray,
						schedBundle: schedBundle).play
					}
				}); 
			}
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
		var	sustain, rearticulating = false;
		inevent ?? { ^nil.yield };

		this.prInit(inevent)
			.prInitNode;

		loop {
			if(this.prDoStreams) {
				sustain = event.use { ~sustain.value };
				if(sustain.notNil and: { sustain < event.delta }) {
					event[\removeFromCleanup] = event[\removeFromCleanup].add(currentCleanupFunc);
					thisThread.clock.sched(sustain, {
						currentCleanupFunc.value(true);
						rearticulating = true;
					});
				} { rearticulating = false };
				cleanup.update(event);
				inevent = event.yield;
				this.prSetNextEvent(inevent);	
				if(rearticulating) {
					event[\id] = nil;
					this.prInitNode;
				};
			} {
				^cleanup.exit(inevent)
			}
		}
	}
}
