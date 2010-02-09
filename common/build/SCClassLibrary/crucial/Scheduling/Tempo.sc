
Tempo  {

	classvar <default;

	var <tempo=1.0,<beatsPerBar=4.0;
	var tempor,beatsPerBarr,tempoClock,clocks,owners;

	*new { arg tempo=1.0,tempoClock;
		^super.new.init(tempoClock).tempo_(tempo)
	}
	init { arg t;
		tempoClock = t ?? {TempoClock.default};
		clocks = [tempoClock];
		owners = [this];
		CmdPeriod.add(this);
	}

	*initClass {
		Class.initClassTree(TempoClock);
		Class.initClassTree(Server);
		default = this.new;
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

	guiClass { ^TempoGui }

	kr { ^TempoPlayer(this).kr }
	*kr { ^TempoPlayer(default).kr }
	*beats2secsKr { |beats| ^TempoPlayer(default).kr.reciprocal * beats }

	// rare to have more than one tempo,
	// but any you create you will have to call destroy on to get rid of it
	// permanantly.  or re-compile.
	destroy {
		CmdPeriod.remove(this);
	}


	// not using this because the clocks get killed by command-.
	// making them dangerous to hold onto
	*makeClock { arg owner;
		^default.makeClock(owner);
	}
	makeClock { arg owner;
		var t;
		t = TempoClock(tempo);
		clocks = clocks.add(t );
		owners = owners.add( owner );
		^t
	}
	cmdPeriod {
		//would get cleared twice
		// this.clearClocks;
	}
	clearClocks {
		clocks.do({ arg c; c.clear });
		owners.do({ arg o; o.clockDidClear });
	}
	clockDidClear {}

}

