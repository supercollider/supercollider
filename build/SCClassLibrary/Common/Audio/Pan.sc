
Pan2 : MultiOutUGen {
	
	*ar { arg in, pos = 0.0, level = 1.0;
		^this.multiNew('audio', in, pos, level )
	}
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ 
			OutputProxy(rate, this, 0), 
			OutputProxy(rate, this, 1) 
		];
		^channels
	}
}

LinPan2 : Pan2 {}

Pan4 : MultiOutUGen {
	
	*ar { arg in, xpos = 0.0, ypos = 0.0, level = 1.0;
		^this.multiNew('audio', in, xpos, ypos, level )
	}
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy.ar1(this, 0), OutputProxy.ar1(this, 1),
					OutputProxy.ar1(this, 2), OutputProxy.ar1(this, 3) ];
		^channels
	}
}


PanB : UGen {
	var channels;
	
	*ar { arg in, azimuth, elevation, gain;
		^this.multiNew('audio', in, azimuth, elevation, gain )
	}
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy.ar1(this), OutputProxy.ar1(this),
					OutputProxy.ar1(this), OutputProxy.ar1(this) ];
		^channels
	}
}

PanAz : MultiOutUGen {
	
	*ar { arg numChans, in, pos = 0.0, level = 1.0, width = 2.0;
		^this.multiNew('audio', numChans, in, pos, level, width )
	}
	init { arg numChans ... theInputs;
		inputs = theInputs;		
		channels = Array.fill(numChans, { arg i; OutputProxy.ar1(this, i) });
		^channels
	}
}

XFade2 : UGen {
	// equal power two channel cross fade
	*ar { arg inA, inB = 0.0, pan = 0.0, level = 1.0;
		^this.multiNew('audio', inA, inB, pan, level)
	}
	*kr { arg inA, inB = 0.0, pan = 0.0, level = 1.0;
		^this.multiNew('control', inA, inB, pan, level)
	}
}

LinXFade2 : UGen {
	// linear two channel cross fade
	*ar { arg inA, inB = 0.0, pan = 0.0, level = 1.0;
		^this.multiNew('audio', inA, inB, pan, level)
	}
	*kr { arg inA, inB = 0.0, pan = 0.0, level = 1.0;
		^this.multiNew('control', inA, inB, pan, level)
	}
}

/*
Pan8 : MultiOutUGen {
	
	*ar { arg in, xpos = 0.0, ypos = 0.0, zpos = 0.0, level = 1.0;
		^this.multiNew('audio', in, xpos, ypos, zpos, level )
	}
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy.ar1(this, 0), OutputProxy.ar1(this, 1),
					OutputProxy.ar1(this, 2), OutputProxy.ar1(this, 3),
					OutputProxy.ar1(this, 4), OutputProxy.ar1(this, 5),
					OutputProxy.ar1(this, 6), OutputProxy.ar1(this, 7) ];
		^channels
	}
}
*/
