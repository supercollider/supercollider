
Tempo  {

	classvar <default;
	
	var <tempo=1.0,<beatsPerBar=4.0;
	var tempor,beatsPerBarr;
	
	*new { arg tempo=1.0;
		^super.new.tempo_(tempo)
	}
	
	*initClass { default = this.new; }
		
	bpm { ^tempo * 60.0 }
	bpm_ { arg bpm; this.tempo = bpm / 60.0; this.changed; }
	next { ^tempo }
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
	
	/* scheduling.   BeatSched is more flexible */
	// changing tempo after scheduling won't work yet
	sched { arg delta,item; 
		SystemClock.sched(this.beats2secs(delta),item) 
	}
	schedAbs { arg beat,item; 
		SystemClock.schedAbs(this.beats2secs(beat),item) 
	}
	
	*bpm { ^default.bpm }
	*bpm_ { arg bpm; default.bpm_(bpm) }
	*next { ^default.tempo }
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

/*  depreciated.  use BeatSched

BeatClock : Clock {
	
	classvar global;
	var <>tempo;
	
	*initClass { global = this.new }
	*new { arg tempo;
		^super.new.tempo_(tempo ? Tempo.default)
	}
	sched { arg delta,item;
		SystemClock.sched(tempo.beats2secs(delta),
			{ arg time;
				var beat;
				if((beat = item.value(time)).isNumber,{
					this.sched(beat,item)
				})
			})
	}
	schedAbs { arg time,item;
		SystemClock.schedAbs(tempo.beats2secs(time),item)
	}
	*sched { arg delta,item;
		global.sched(delta,item)
	}
	*schedAbs { arg time,item;
		global.schedAbs(time,item)
	}
}

*/

