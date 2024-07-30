Spring : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }

	*ar { arg in=0.0, spring=1, damp=0;
		^this.multiNew('audio', in, spring, damp)
	}

	*kr { arg in=0.0, spring=1, damp=0;
		^this.multiNew('control', in, spring, damp)
	}
}

Ball : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }

	*ar { arg in=0.0, g=1, damp=0, friction=0.01;
		^this.multiNew('audio', in, g, damp, friction)
	}

	*kr { arg in=0.0, g=1, damp=0, friction=0.01;
		^this.multiNew('control', in, g, damp, friction)
	}
}

TBall : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }

	*ar { arg in=0.0, g=10, damp=0, friction=0.01;
		^this.multiNew('audio', in, g, damp, friction)
	}

	*kr { arg in=0.0, g=10, damp=0, friction=0.01;
		^this.multiNew('control', in, g, damp, friction)
	}
}
