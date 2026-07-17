LinkPhase : UGen {
	*kr {|quantum=4|
		^this.multiNew('control', quantum);
	}

	*start {|server|
		server = server ? Server.default;
		server.sendMsg(*this.startMsg(true));
	}

	*stop {|server|
		server = server ? Server.default;
		server.sendMsg(*this.startMsg(false));
	}

	*startMsg {|enable=true|
		^[
			\cmd,
			\linkclock,
			if(enable, 1, 0),
		];
	}

	*setTempo {|tempo, lag=0.0, curve=\exp, server=nil|
		lag = lag.max(0.0);
		if(lag > 0.0, {
			{
				var startTempo = Latch.kr(in: LinkTempo.kr, trig: 1.0);
				LinkTempo.kr(
					gate: 1.0,
					tempo: Env([startTempo, tempo], [lag], curve).kr(
						doneAction: Done.freeSelf,
					);
				);
			}
		}, {
			{
				LinkTempo.kr(1.0, tempo);
				FreeSelf.kr(1.0);
			}
		}).play(server);
	}
}

LinkTempo : UGen {
	*kr {|gate=0.0, tempo=1.0|
		^this.multiNew('control', gate, tempo);
	}
}

LinkJump : UGen {
	*kr {|gate=0.0, beat=0.0, quantum=4.0, force=0.0|
		^this.multiNew('control', gate, beat, quantum, force);
	}
}
