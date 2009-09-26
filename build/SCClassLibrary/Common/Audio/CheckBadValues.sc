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
