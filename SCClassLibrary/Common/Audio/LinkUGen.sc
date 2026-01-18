LinkPhase : UGen {
	*start {|server=nil|
		{FreeSelf.kr(LinkEnabler.kr)}.play(server ? Server.default);
	}

	*stop {|server=nil|
		{FreeSelf.kr(LinkDisabler.kr)}.play(server ? Server.default);
	}

	*setCPS {|cps, lag=0.0, curve=\exp, server=nil|
		lag = lag.max(0.0);
		if(lag > 0.0, {
			{
				var startCps = Latch.kr(in: LinkCPS.kr, trig: 1.0);
				LinkCPS.kr(
					change: 1.0,
					cps: Env([startCps, cps], [lag], curve).kr(
						doneAction: Done.freeSelf,
					);
				);
			}
		}, {
			{
				LinkCPS.kr(1.0, cps);
				FreeSelf.kr(1.0);
			}
		}).play(server ? Server.default);
	}

	*kr {|quantum=4|
		^this.multiNew('control', quantum);
	}
}

LinkCPS : UGen {
	*kr {|change=0.0, cps=1.0|
		^this.multiNew('control', change, cps);
	}
}

LinkJump : UGen {
	*kr {|trigger=0.0, beat=0.0, quantum=4.0, force=0.0|
		^this.multiNew('control', trigger, beat, quantum, force);
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
