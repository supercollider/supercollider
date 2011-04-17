TempoBusClock : TempoClock {
	var <>control;
	classvar <>default;

	*new { arg control, tempo, beats, seconds;
		^super.new(tempo, beats, seconds).control_(control)
	}

	setTempoAtBeat { arg newTempo, beats;
		control.set(\fadeTime, 0.0, \tempo, newTempo);
		^super.setTempoAtBeat(newTempo, beats)
	}

	setTempoAtSec { arg newTempo, secs;
		control.set(\fadeTime, 0.0, \tempo, newTempo)
		^super.setTempoAtSec(newTempo, secs)
	}

}
