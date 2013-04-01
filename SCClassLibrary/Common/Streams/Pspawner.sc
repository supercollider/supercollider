Spawner : Pattern {
	var <>genStream;
	var <>priorityQ;
	var <>now;
	var <>event;

	*new { | func, stackSize=64 |
		^super.new.init( func, stackSize)
	}

	suspend { | stream |
		priorityQ.removeValue(stream);
	}

	suspendAll {
		priorityQ.clear
	}

	init { | func, stackSize |
		priorityQ = PriorityQueue.new;
		genStream = Routine({func.value(this) }, stackSize);
		now = 0;
		priorityQ.put(now, genStream);
	}

	par { | pattern, delta = 0 |
		var stream = pattern.asStream;
		priorityQ.put(now + delta, stream);
		^stream;
	}

	seq { | pat |
		pat.embedInStream(event)
	}

	wait { | dur |
		Event.silent(dur, event).yield
	}

	embedInStream { | inevent, cleanup|

		var outevent, stream, nexttime;
		event = inevent;					// gives genStream access to the event
		cleanup ?? { cleanup = EventStreamCleanup.new };

		while({
			priorityQ.notEmpty
		},{
			stream = priorityQ.pop;
			outevent = stream.next(event).asEvent;

			if (outevent.isNil, {
				nexttime = priorityQ.topPriority;
				if (nexttime.notNil, {
					// that child stream ended, so rest until next one
					outevent = Event.silent(nexttime - now, event);
					cleanup.update(outevent);
					event = outevent.yield;
					now = nexttime;
				},{
					priorityQ.clear;
					^cleanup.exit(event);
				});
			},{
				cleanup.update(outevent);
				// requeue stream
				priorityQ.put(now + outevent.delta, stream);
				nexttime = priorityQ.topPriority;
				outevent.put(\delta, nexttime - now);

				event = outevent.yield;
				now = nexttime;
			});
		});
		^event;
	}
}

Pspawner : Prout {
	asStream {
		^Routine({ | ev | this.embedInStream(ev) })
	}
	embedInStream { | inevent, cleanup |
		^Spawner(routineFunc).embedInStream(inevent, cleanup ?? { EventStreamCleanup.new });
	}
}

Pspawn : FilterPattern {
	var	<>spawnProtoEvent;

	*new { |pattern, spawnProtoEvent|
		^super.new(pattern)
			.spawnProtoEvent_(spawnProtoEvent ?? { Event.default });
	}

	embedInStream { |inevent, cleanup|
		^Spawner({ |sp|
			var	event, stream = pattern.asStream,
				child;
			while { (event = stream.next(spawnProtoEvent.copy.put(\spawner, sp))).notNil } {
				case
					{ event.method == \wait } {
						event.spawner.wait(event.delta)
					}
					{ #[seq, par].includes(event.method) } {
						child = event[\pattern];
						if(child.isKindOf(Symbol)) {
							child = (event[\dict] ? Pdef).at(child);
						};
						event.spawner.perform(event.method, child.value(event));
						if(event.delta > 0) {
							event.spawner.wait(event.delta)
						}
					}
						// suspend requires access to the specific stream
						// don't know how to get it... maybe implement later
					{ event.method == \suspendAll } {
						event.spawner.suspendAll
					}
					{ "Unrecognized method % in spawner event."
						.format(event.method.asCompileString).warn;
					}
			};
		}).embedInStream(inevent, cleanup ?? { EventStreamCleanup.new })
	}
}

/*
(
	Pseq([
		Pspawner({ | sp |
			sp.postln;
			sp.par(Pbind(*[degree:	Pwhite(0,12), dur: 0.1, db: -30]) );
			sp.seq(Pbind(*[degree:	Pseq((0..4).mirror.mirror, 1) + [-3, 0,2], ctranspose: -12, dur: 0.2 ]) );
			"hi".postln;
			sp.wait(1);
			"bye".postln;
			sp.suspendAll;
		}),

		Pspawner({ | sp |
			sp.postln;
			sp.par(Pbind(*[degree:	Pwhite(0,12), dur: 0.2, ctranspose: -12]) );
			"hi".postln;
			sp.wait(4);
			"bye".postln;
			sp.suspendAll
		}),

	]).play

	a = Spawner({ |sp | 100.do{ sp.wait(1) } });
	a.play;
	b = a.par(Pbind(*[degree: Pwhite(0, 10), dur: 0.2]));
	a.suspend(b)
	a.par(b)

	Pspawner({ | sp |
		5.do {
			sp.par(Pbind(*[
				octave: (5.rand + 3).postln,
				 degree:	Pwhite(0,12), dur: 0.1, db: -30
			]) );
			sp.wait(1);
			sp.clear;
		}
	}).play

)
*/
