LinkUGen : UGen {
	classvar <enabled;
	classvar <>server;

	*initClass {
		Class.initClassTree(Server);
		enabled = false;
		server = Server.default;
	}

	*enable {
		if(server.hasBooted.not, {
			"Server % is not running - can not enable LinkUGen".format(server).warn;
			^this;
		});
		if(enabled, {
			"LinkUGen is already enabled".warn;
			^this;
		});
		{FreeSelf.kr(LinkEnabler.kr())}.play(server);
		enabled = true;
	}

	*disable {
		if(server.hasBooted.not, {
			// this should not happen...
			"Server % is not running - can not disable LinkUGen".format(server).warn;
			^this;
		});
		if(enabled.not, {
			"LinkUGen is already disabled".warn;
			^this;
		});
		{FreeSelf.kr(LinkDisabler.kr)}.play(server);
		enabled = false;
	}

	*enabled_ {|newEnabled|
		newEnabled = newEnabled.asBoolean;
		if(newEnabled, {
			LinkUGen.enable;
		}, {
			LinkUGen.disable;
		});
	}

	*setTempo {|bpm|
		{
			LinkTempo.kr(1.0, bpm);
			FreeSelf.kr(1.0);
		}.play(server);
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

LinkTempo : UGen {
	*kr {|change=0.0, bpm=60.0|
		^this.new1('control', change, bpm);
	}
}

LinkJump : UGen {
	*kr {|trigger=0.0, beat=0.0, quantum=4.0, force=0.0|
		^this.new1('control', trigger, beat, quantum, force);
	}
}
