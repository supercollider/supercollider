LinkPhase : UGen {
	*start {|server=nil|
		server = server ? Server.default;
		if(server.hasBooted.not, {
			"Server % is not running - can not enable LinkUGen".format(server).warn;
			^this;
		});
		{FreeSelf.kr(LinkEnabler.kr)}.play(server);
	}

	*stop {|server=nil|
		server = server ? Server.default;
		if(server.hasBooted.not, {
			// this should not happen...
			"Server % is not running - can not disable LinkUGen".format(server).warn;
			^this;
		});
		{FreeSelf.kr(LinkDisabler.kr)}.play(server);
	}

	*setTempo {|bpm, lag=0.0, curve=\exp, server=nil|
		server = server ? Server.default;
		if(server.hasBooted.not, {
			"Server % is not running - can not set tempo".format(server).warn;
			^this;
		});
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
		}).play(server);
	}

	*kr {|quantum=4|
		^this.new1('control', quantum);
	}
}

LinkEnabler : UGen {
	*kr {
		^this.new1('control');
	}
}

LinkDisabler : UGen {
	*kr {
		^this.new1('control');
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
