
Peep {
	*ar { arg in, label="peep", period=1;
		^this.multiNew('audio', in, label, period)
	}
	*kr { arg in, label="peep", period=1;
		^this.multiNew('control', in, label, period)
	}
}
		