LinkPhase : UGen {
	*start {|server=nil|
		{FreeSelf.kr(LinkEnabler.kr)}.play(server ? Server.default);
	}

	*stop {|server=nil|
		{FreeSelf.kr(LinkDisabler.kr)}.play(server ? Server.default);
	}

	*setBPM {|bpm, lag=0.0, curve=\exp, server=nil|
		lag = lag.max(0.0);
		if(lag > 0.0, {
			{
				var startBpm = Latch.kr(in: LinkBPM.kr, trig: 1.0);
				LinkBPM.kr(
					change: 1.0,
					bpm: Env([startBpm, bpm], [lag], curve).kr(
						doneAction: Done.freeSelf,
					).poll;
				);
			}
		}, {
			{
				LinkBPM.kr(1.0, bpm);
				FreeSelf.kr(1.0);
			}
		}).play(server ? Server.default);
	}

	*kr {|quantum=4|
		^this.new1('control', quantum);
	}
}

LinkBPM : UGen {
	*kr {|change=0.0, bpm=60.0|
		^this.new1('control', change, bpm);
	}
}

LinkJump : UGen {
	*kr {|trigger=0.0, beat=0.0, quantum=4.0, force=0.0|
		^this.new1('control', trigger, beat, quantum, force);
	}
}

// Use LinkPhase.start
LinkEnabler : UGen {
	*kr {
		^this.new1('control');
	}
}

// Use LinkPhase.stop
LinkDisabler : UGen {
	*kr {
		^this.new1('control');
	}
}
