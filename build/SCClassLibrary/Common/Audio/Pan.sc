
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

Balance2 : MultiOutUGen {
	
	*ar { arg left, right, pos = 0.0, level = 1.0;
		^this.multiNew('audio', left, right, pos, level )
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

Rotate2 : MultiOutUGen {
	
	*ar { arg x, y, pos = 0.0;
		^this.multiNew('audio', x, y, pos )
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



PanB : MultiOutUGen {
	
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

PanB2 : MultiOutUGen {
	
	*ar { arg in, azimuth, gain=1;
		^this.multiNew('audio', in, azimuth, gain )
	}
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2) ];
		^channels
	}
}

BiPanB2 : MultiOutUGen {
	
	*ar { arg inA, inB, azimuth, gain=1;
		^this.multiNew('audio', inA, inB, azimuth, gain )
	}
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ OutputProxy(\audio,this,0), OutputProxy(\audio,this,1),
					OutputProxy(\audio,this,2) ];
		^channels
	}
}

DecodeB2 : MultiOutUGen {
	
	*ar { arg numChans, w, x, y, orientation = 0.5;
		^this.multiNew('audio', numChans, w, x, y, orientation = 0.5 )
	}
	init { arg numChans ... theInputs;
		inputs = theInputs;		
		channels = Array.fill(numChans, { arg i; OutputProxy(\audio,this, i) });
		^channels
	}
 	checkInputs {
 		for(0, 2, { arg i;
 			if (inputs.at(i).rate != 'audio', {
 				^("input was not audio rate: " + inputs.at(i));
			});
 		});
 		^nil
 	}
}

PanAz : MultiOutUGen {
	
	*ar { arg numChans, in, pos = 0.0, level = 1.0, width = 2.0, orientation = 0.5;
		^this.multiNew('audio', numChans, in, pos, level, width, orientation )
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

