
Tempo  {

	classvar <default;
	
	var <tempo=1.0,<beatsPerBar=4.0;
	var tempor,beatsPerBarr;
	
	*initClass { default = this.new; }
		
	bpm { ^tempo * 60.0 }
	bpm_ { arg bpm; tempo = bpm / 60.0; this.changed; }
	tempo_ { arg t; 
		tempo = t; 
		tempor = tempo.reciprocal; 
		this.changed; 
	}	
	beatsPerBar_ { arg b; 
		beatsPerBar = b; 
		beatsPerBarr = beatsPerBar.reciprocal; 
		this.changed; 
	}	

	beats2secs { arg beats; ^tempor * beats }
	secs2beats { arg secs; ^tempo * secs }
	bars2secs { arg bars; ^tempor * (bars * beatsPerBar) }
	secs2bars { arg secs; ^tempo * secs * beatsPerBarr }
	// changing tempo after scheduling won't work yet
	sched { arg delta,item; SystemClock.sched(this.beats2secs(delta),item) }
	schedAbs { arg beat,item; SystemClock.schedAbs(this.beats2secs(beat),item) }
	
	*bpm { ^default.bpm }
	*bpm_ { arg bpm; default.bpm_(bpm) }
	*beats2secs { arg beats; ^default.beats2secs(beats) }
	*secs2beats { arg secs; ^default.secs2beats(secs) }
	*tempo { ^default.tempo }
	*tempo_ { arg tempo; default.tempo_(tempo) }
	*sched { arg delta,item; ^default.sched(delta,item) }
	*schedAbs { arg beat,item; ^default.schedAbs(beat,item) }
		
//	*beats2secsKr { arg beats; ^GetTempo.kr.reciprocal * beats }
//	*secs2beatsKr { arg secs; ^GetTempo.kr * secs }
		
	*gui { arg layout; default.gui(layout) }
	guiClass { ^TempoGui }
}

BeatClock : Clock {
	
	var <>tempo;
	
	*new { arg tempo;
		^super.new.tempo_(tempo ? Tempo.default)
	}

	*sched { arg delta,item;
		SystemClock.sched(tempo.beats2secs(delta),item)
	}
	*schedAbs { arg time,item;
		SystemClock.sched(tempo.beats2secs(time) + beatEpoch,item)
	}
}
