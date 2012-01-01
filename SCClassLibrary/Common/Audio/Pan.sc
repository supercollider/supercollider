Pan2 : MultiOutUGen {
	*ar { arg in, pos = 0.0, level = 1.0;
		^this.multiNew('audio', in, pos, level )
	}
	*kr { arg in, pos = 0.0, level = 1.0;
		^this.multiNew('control', in, pos, level )
	}
	init { arg ... theInputs;
		inputs = theInputs;
		channels = [
			OutputProxy(rate, this, 0),
			OutputProxy(rate, this, 1)
		];
		^channels
	}
	checkInputs { ^this.checkNInputs(1) }
}

LinPan2 : Pan2 {}

Pan4 : MultiOutUGen {
	*ar { arg in, xpos = 0.0, ypos = 0.0, level = 1.0;
		^this.multiNew('audio', in, xpos, ypos, level )
	}
	*kr { arg in, xpos = 0.0, ypos = 0.0, level = 1.0;
		^this.multiNew('control', in, xpos, ypos, level )
	}
	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(rate,this, 0), OutputProxy(rate,this, 1),
					OutputProxy(rate,this, 2), OutputProxy(rate,this, 3) ];
		^channels
	}
	checkInputs { ^this.checkNInputs(1) }
}

Balance2 : MultiOutUGen {

	*ar { arg left, right, pos = 0.0, level = 1.0;
		^this.multiNew('audio', left, right, pos, level )
	}
	*kr { arg left, right, pos = 0.0, level = 1.0;
		^this.multiNew('control', left, right, pos, level )
	}
	init { arg ... theInputs;
		inputs = theInputs;
		channels = [
			OutputProxy(rate, this, 0),
			OutputProxy(rate, this, 1)
		];
		^channels
	}
	checkInputs { ^this.checkNInputs(2) }
}

Rotate2 : MultiOutUGen {
	*ar { arg x, y, pos = 0.0;
		^this.multiNew('audio', x, y, pos )
	}
	*kr { arg x, y, pos = 0.0;
		^this.multiNew('control', x, y, pos )
	}
	init { arg ... theInputs;
		inputs = theInputs;
		channels = [
			OutputProxy(rate, this, 0),
			OutputProxy(rate, this, 1)
		];
		^channels
	}
	checkInputs { ^this.checkNInputs(2) }
}

PanB : MultiOutUGen {
	*ar { arg in, azimuth=0, elevation=0, gain=1;
		^this.multiNew('audio', in, azimuth, elevation, gain )
	}
	*kr { arg in, azimuth=0, elevation=0, gain=1;
		^this.multiNew('control', in, azimuth, elevation, gain )
	}
	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(rate,this,0), OutputProxy(rate,this,1),
					OutputProxy(rate,this,2), OutputProxy(rate,this,3) ];
		^channels
	}
	checkInputs { ^this.checkNInputs(1) }
}

PanB2 : MultiOutUGen {
	*ar { arg in, azimuth=0, gain=1;
		^this.multiNew('audio', in, azimuth, gain )
	}
	*kr { arg in, azimuth=0, gain=1;
		^this.multiNew('control', in, azimuth, gain )
	}
	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(rate,this,0), OutputProxy(rate,this,1),
					OutputProxy(rate,this,2) ];
		^channels
	}
	checkInputs { ^this.checkNInputs(1) }
}

BiPanB2 : MultiOutUGen {
	*ar { arg inA, inB, azimuth, gain=1;
		^this.multiNew('audio', inA, inB, azimuth, gain )
	}
	*kr { arg inA, inB, azimuth, gain=1;
		^this.multiNew('control', inA, inB, azimuth, gain )
	}
	init { arg ... theInputs;
		inputs = theInputs;
		channels = [ OutputProxy(rate,this,0), OutputProxy(rate,this,1),
					OutputProxy(rate,this,2) ];
		^channels
	}
	checkInputs { ^this.checkNInputs(2) }
}

DecodeB2 : MultiOutUGen {
	*ar { arg numChans, w, x, y, orientation = 0.5;
		^this.multiNew('audio', numChans, w, x, y, orientation = 0.5 )
	}
	*kr { arg numChans, w, x, y, orientation = 0.5;
		^this.multiNew('control', numChans, w, x, y, orientation = 0.5 )
	}
	init { arg numChans ... theInputs;
		inputs = theInputs;
		channels = Array.fill(numChans, { arg i; OutputProxy(rate,this, i) });
		^channels
	}
	checkInputs { ^this.checkNInputs(3) }
}

PanAz : MultiOutUGen {
	*ar { arg numChans, in, pos = 0.0, level = 1.0, width = 2.0, orientation = 0.5;
		^this.multiNew('audio', numChans, in, pos, level, width, orientation )
	}
	*kr { arg numChans, in, pos = 0.0, level = 1.0, width = 2.0, orientation = 0.5;
		^this.multiNew('control', numChans, in, pos, level, width, orientation )
	}
	init { arg numChans ... theInputs;
		inputs = theInputs;
		channels = Array.fill(numChans, { arg i; OutputProxy(rate,this, i) });
		^channels
	}
	checkInputs { ^this.checkNInputs(1) }
}


XFade2 : UGen {
	// equal power two channel cross fade
	*ar { arg inA, inB = 0.0, pan = 0.0, level = 1.0;
		^this.multiNew('audio', inA, inB, pan, level)
	}
	*kr { arg inA, inB = 0.0, pan = 0.0, level = 1.0;
		^this.multiNew('control', inA, inB, pan, level)
	}
	checkInputs { ^this.checkNInputs(2) }
}

LinXFade2 : UGen {
	// linear two channel cross fade
	*ar { arg inA, inB = 0.0, pan = 0.0, level = 1.0;
		^this.multiNew('audio', inA, inB, pan) * level
	}
	*kr { arg inA, inB = 0.0, pan = 0.0, level = 1.0;
		^this.multiNew('control', inA, inB, pan) * level
	}
	checkInputs { ^this.checkNInputs(2) }
}
