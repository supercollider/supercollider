
Tempo  {

	classvar <default;
	
	var <tempo=1.0,<beatsPerBar=4.0;
	var tempor,beatsPerBarr,tempoClock,clocks;
	
	*new { arg tempo=1.0,tempoClock;
		^super.new.init(tempoClock).tempo_(tempo)
	}
	init { arg t; 
		tempoClock = t ?? {TempoClock.default};
		clocks = [tempoClock];
	}
	
	*initClass { 
		Class.initClassTree(TempoClock);
		Class.initClassTree(Server);
		default = this.new;
		CmdPeriod.add(default);
	}
		
	bpm { ^tempo * 60.0 }
	bpm_ { arg bpm; this.tempo = bpm / 60.0; }
	next { ^tempo }
	tempo_ { arg t; 
		tempo = t; 
		tempor = tempo.reciprocal;
		clocks.do({ arg c; c.tempo = tempo });
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
	
	*bpm { ^default.bpm }
	*bpm_ { arg bpm; 
		default.bpm_(bpm); 
	}
	*next { ^default.tempo }
	*beats2secs { arg beats; ^default.beats2secs(beats) }
	*secs2beats { arg secs; ^default.secs2beats(secs) }
	*tempo { ^default.tempo }
	*tempo_ { arg tempo; 
		default.tempo_(tempo);

	}
	//	*beats2secsKr { arg beats; ^GetTempo.kr.reciprocal * beats }
	//	*secs2beatsKr { arg secs; ^GetTempo.kr * secs }
	
	*makeClock {
		^default.makeClock;
	}
	makeClock {
		var t;
		t = TempoClock(default.tempo);
		clocks = clocks.add(t);
		^t
	}
	cmdPeriod {
		this.clearClocks;
	}
	clearClocks {
		clocks.do({ arg c; c.clear });
	}
	*gui { arg layout; default.gui(layout) }
	guiClass { ^TempoGui }
}

