CheckBadValues : UGen {

	*ar {arg in = 0.0, id = 0, post = 2;
		^this.multiNew('audio', in, id, post);
	}

	*kr {arg in = 0.0, id = 0, post = 2;
		^this.multiNew('control', in, id, post);
	}

	checkInputs {
		if ((rate==\audio) and:{ inputs.at(0).rate != \audio}) {
			^("audio-rate, yet first input is not audio-rate");
		};
		^this.checkValidInputs
	}
}

Sanitize : UGen {

	*ar { |in = 0.0, replace = 0.0|
		^this.multiNew('audio', in, replace);
	}

	*kr { |in = 0.0, replace = 0.0|
		^this.multiNew('control', in, replace);
	}

	checkInputs {
		if ((rate==\audio) and:{ inputs.at(0).rate != \audio}) {
			^("audio-rate, yet first input is not audio-rate");
		};
		^this.checkValidInputs
	}
}