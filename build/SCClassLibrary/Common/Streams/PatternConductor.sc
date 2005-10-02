/*
	PatternConductor provides interactive control of patterns. When a Conductor
is stopped it flushes its queue, immediately ending any notes left hanging by 
the pattern.

*/

PatternConductor  {

	var <>clock, <>pattern, <>event, <>quant, <>eventStream, <tempo;
	*new { |pattern, event, quant|
		^super.new
			.pattern_(pattern).event_(event ? Event.default).quant_(quant ? 0).tempo_(1);
	}
	
	play { 
		if (quant != 0) {
			Routine.run({ this.prPlay }, 64, TempoClock.default, quant)
		} {
			this.prPlay
		}
	}

	prPlay {
		if (clock.notNil) { this.stop };
		clock = TempoClock(tempo);
		eventStream = pattern.play(clock, event, quant);
	}
	pause { | pauseTempo = 0.000001| if(clock.notNil) { clock.tempo = pauseTempo } }
	resume { if(clock.notNil) { clock.tempo = tempo } }

	stop { |stopTempo|
		eventStream.stream.next(nil);
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

