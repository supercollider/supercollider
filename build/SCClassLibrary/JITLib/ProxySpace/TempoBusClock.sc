

TempoBusClock : TempoClock {
	var <>control;
	classvar <>default;
	
	*new { arg control, tempo, beats, seconds;
		^super.new(tempo, beats, seconds).initBus(control, tempo)
	}
	
	initBus { arg ctl, tempo;
		control = ctl;
		control.set(0, tempo ? 1);
	}
	
	setTempoAtBeat { arg newTempo, beats;
		control.set(0, newTempo);
		^super.setTempoAtBeat(newTempo, beats)
	}
	
	setTempoAtSec { arg newTempo, secs;
		control.set(0, newTempo);
		^super.setTempoAtSec(newTempo, secs)
	}

}

