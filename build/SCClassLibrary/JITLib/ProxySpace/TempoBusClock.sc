//for now: set tempo of something that understands set(0, val)
//and plays on the server

//doesn't fully work

TempoBusClock : TempoClock {
	var <>serverControl;
	classvar <>default;
	
	*new { arg serverControl, tempo, beats, seconds;
		^super.new(tempo, beats, seconds).initBus(serverControl, tempo)
	}
	*initClass {
		default = this.new;
	}	
	initBus { arg ctl, tempo;
		serverControl = ctl;
		serverControl.set(0,tempo ? 1);
	}
	setTempoAtBeat { arg newTempo, beats;
		serverControl.set(0, newTempo); // see for latency etc..
		^super.setTempoAtBeat(newTempo, beats)
	}
	setTempoAtSec { arg newTempo, secs;
		serverControl.set(0, newTempo);
		^super.setTempoAtSec(newTempo, secs)
	}

}

