
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
		channels = [ OutputProxy(\audio,this, 0), OutputProxy(\audio,this, 1),
					OutputProxy(\audio,this, 2), OutputProxy(\audio,this, 3) ];
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
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2), OutputProxy(\audio,this,3) ];
		^channels
	}
}

PanAz : MultiOutUGen {
	
	*ar { arg numChans, in, pos = 0.0, level = 1.0, width = 2.0;
		^this.multiNew('audio', numChans, in, pos, level, width )
	}
	init { arg numChans ... theInputs;
		inputs = theInputs;		
		channels = Array.fill(numChans, { arg i; OutputProxy(\audio,this, i) });
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
		channels = [ OutputProxy(\audio,this, 0), OutputProxy(\audio,this, 1),
					OutputProxy(\audio,this, 2), OutputProxy(\audio,this, 3),
					OutputProxy(\audio,this, 4), OutputProxy(\audio,this, 5),
					OutputProxy(\audio,this, 6),OutputProxy(\audio,this, 7) 
					];
		^channels
	}
}
*/
