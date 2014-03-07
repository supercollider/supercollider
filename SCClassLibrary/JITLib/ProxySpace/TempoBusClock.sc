TempoBusClock : TempoClock {
	var <>control;
	classvar <>default;

	*new { | control, tempo, beats, seconds |
		^super.new(tempo, beats, seconds).control_(control)
	}

	setTempoAtBeat { | newTempo, beats |
		control.set(\fadeTime, 0.0, \tempo, newTempo);
		^super.setTempoAtBeat(newTempo, beats)
	}

	setTempoAtSec { | newTempo, secs |
		control.set(\fadeTime, 0.0, \tempo, newTempo)
		^super.setTempoAtSec(newTempo, secs)
	}

}
