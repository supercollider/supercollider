
Tempo  {

	classvar <default;
	
	var <tempo=1.0;
	
	*initClass { default = this.new; }
		
	bpm { ^tempo * 60.0 }
	bpm_ { arg bpm; tempo = bpm / 60.0; this.changed; }
	tempo_ { arg t; tempo = t; this.changed; }	
	beats2secs { arg beats; ^tempo.reciprocal * beats }
	secs2beats { arg secs; ^tempo * secs }

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

