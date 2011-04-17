/*
	PatternConductor provides interactive control of patterns. When a Conductor
is stopped it flushes its queue, immediately ending any notes left hanging by
the pattern.

*/

PatternConductor  {

	var <>patterns, <>event, <>quant, <>eventStreamPlayers;
	var <>clock, <tempo, <>defaultPauseTempo, <>defaultStopTempo;
	*new { |patterns, event, quant|
		^super.new
			.patterns_(patterns.asArray)
			.event_(event ? Event.default)
			.quant_(quant ? 0).tempo_(1).defaultPauseTempo_(1e-8).defaultStopTempo_(1e+8);
	}

	play {

		Routine.run({ this.prPlay }, 64, TempoClock.default, quant)
	}

	prPlay {
		if (clock.notNil) { this.stop };
		CmdPeriod.add(this);
		clock = TempoClock(tempo);
		eventStreamPlayers = patterns.collect { | p | p.asEventStreamPlayer(event) };
		eventStreamPlayers.do { | p | p.play(clock, false, 0) };
	}
	pause { | pauseTempo| clock.tempo = pauseTempo ? defaultPauseTempo }
	resume {  clock.tempo = tempo }

	stop { |stopTempo|
		var oldClock;
		oldClock = clock;
		eventStreamPlayers.do { | p | p.stop };
		clock.tempo = stopTempo ? defaultStopTempo;
		Task({ while {oldClock.queue.size >= 2 } { yield(1) }; oldClock.stop }).play(clock);
		clock = nil;
		eventStreamPlayers = nil;
		CmdPeriod.remove(this);
	}

	tempo_ { | temp |
		tempo = temp;
		if (clock.notNil) { clock.tempo_(tempo) };
	}
	cmdPeriod { clock = nil }
}
