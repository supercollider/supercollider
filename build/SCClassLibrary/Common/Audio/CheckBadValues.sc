CheckBadValues : UGen {

	*ar {arg in = 0.0, id = 0, post = 1;
		^this.multiNew('audio', in, id, post);
	}
	
	*kr {arg in = 0.0, id = 0, post = 1;
		^this.multiNew('control', in, id, post);
	}

}
