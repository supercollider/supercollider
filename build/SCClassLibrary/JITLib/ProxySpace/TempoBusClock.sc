//for now: set tempo of something that understands source_
//and plays on the server

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
		serverControl.source = (tempo ? 1);
	}
	setTempoAtBeat { arg newTempo, beats;
		serverControl.source = newTempo; // see for latency etc..
		^super.setTempoAtBeat(newTempo, beats)
	}
	setTempoAtSec { arg newTempo, secs;
		serverControl.source = newTempo;
		^super.setTempoAtSec(newTempo, secs)
	}

}

