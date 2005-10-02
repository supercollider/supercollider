/*
	PatternConductor provides interactive control of patterns. When a Conductor
is stopped it flushes its queue, immediately ending any notes left hanging by 
the pattern.

*/

PatternConductor  {

	var <>clock, <>patterns, <>event, <>quant, <>eventStreamPlayers, <tempo;
	*new { |patterns, event, quant|
		^super.new
			.patterns_(patterns.asArray)
			.event_(event ? Event.default)
			.quant_(quant ? 0).tempo_(1);
	}
	
	play { 
		Routine.run({ this.prPlay }, 64, TempoClock.default, quant)
	}

	prPlay {
		if (clock.notNil) { this.stop };
		clock = TempoClock(tempo);
		eventStreamPlayers = patterns.collect { | p | p.asEventStreamPlayer(event) };
		eventStreamPlayers.do { | p | p.play(clock, false, quant) };
	}
	pause { | pauseTempo = 0.000001| if(clock.notNil) { clock.tempo = pauseTempo } }
	resume { if(clock.notNil) { clock.tempo = tempo } }

	stop { |stopTempo|
		eventStreamPlayers.do { | p | p.stop };
		eventStreamPlayers = nil;
		if (stopTempo.isNil) {
			clock.stop; 
			clock.queue.pairsDo { | t, e| e.value };
			clock.clear;
			clock = nil;
		} {
			clock.tempo = stopTempo;
		};
	}
	
	tempo_ { | temp | 
		tempo = temp; 
		if (clock.notNil) { clock.tempo_(tempo) };
	}
}

