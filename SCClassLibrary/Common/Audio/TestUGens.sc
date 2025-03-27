CheckBadValues : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^true } // Never delete this.
	canBeReplacedByIdenticalCall { ^true }

	*ar {arg in = 0.0, id = 0, post = 2;
		^this.multiNew('audio', in, id, post);
	}

	*kr {arg in = 0.0, id = 0, post = 2;
		^this.multiNew('control', in, id, post);
	}

	checkInputs {
		if (rate == \audio) {
			^this.checkSameRateAsFirstInput;
		};
		^this.checkValidInputs
	}
}

Sanitize : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	*ar { |in = 0.0, replace = 0.0|
		^this.multiNew('audio', in, replace);
	}

	*kr { |in = 0.0, replace = 0.0|
		^this.multiNew('control', in, replace);
	}

	checkInputs {
		if (rate == \audio) {
			^this.checkSameRateAsFirstInput;
		};
		^this.checkValidInputs
	}
}
