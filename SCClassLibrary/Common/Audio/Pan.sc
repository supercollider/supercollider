Pan2 : MultiOutUGen {
	*ar { |in, pos = 0.0, level = 1.0|
		^this.multiNew('audio', in, pos, level)
	}
	*kr { |in, pos = 0.0, level = 1.0|
		^this.multiNew('control', in, pos, level)
	}
	init { |... theInputs|
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
	*ar { |in, xpos = 0.0, ypos = 0.0, level = 1.0|
		^this.multiNew('audio', in, xpos, ypos, level)
	}
	*kr { |in, xpos = 0.0, ypos = 0.0, level = 1.0|
		^this.multiNew('control', in, xpos, ypos, level)
	}
	init { |... theInputs|
		inputs = theInputs;
		channels = [
			OutputProxy(rate, this, 0),
			OutputProxy(rate, this, 1),
			OutputProxy(rate, this, 2),
			OutputProxy(rate, this, 3)
		];
		^channels
	}
	checkInputs { ^this.checkNInputs(1) }
}

Balance2 : MultiOutUGen {

	*ar { |left, right, pos = 0.0, level = 1.0|
		^this.multiNew('audio', left, right, pos, level)
	}
	*kr { |left, right, pos = 0.0, level = 1.0|
		^this.multiNew('control', left, right, pos, level)
	}
	init { |... theInputs|
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
	*ar { |x, y, pos = 0.0|
		^this.multiNew('audio', x, y, pos)
	}
	*kr { |x, y, pos = 0.0|
		^this.multiNew('control', x, y, pos)
	}
	init { |... theInputs|
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
	*ar { |in, azimuth = 0, elevation = 0, gain = 1|
		^this.multiNew('audio', in, azimuth, elevation, gain)
	}
	*kr { |in, azimuth = 0, elevation = 0, gain = 1|
		^this.multiNew('control', in, azimuth, elevation, gain)
	}
	init { |... theInputs|
		inputs = theInputs;
		channels = [
			OutputProxy(rate, this, 0),
			OutputProxy(rate, this, 1),
			OutputProxy(rate, this, 2),
			OutputProxy(rate, this, 3)
		];
		^channels
	}
	checkInputs { ^this.checkNInputs(1) }
}

PanB2 : MultiOutUGen {
	*ar { |in, azimuth = 0, gain = 1|
		^this.multiNew('audio', in, azimuth, gain)
	}
	*kr { |in, azimuth = 0, gain = 1|
		^this.multiNew('control', in, azimuth, gain)
	}
	init { |... theInputs|
		inputs = theInputs;
		channels = [
			OutputProxy(rate, this, 0),
			OutputProxy(rate, this, 1),
			OutputProxy(rate, this, 2)
		];
		^channels
	}
	checkInputs { ^this.checkNInputs(1) }
}

BiPanB2 : MultiOutUGen {
	*ar { |inA, inB, azimuth, gain = 1|
		^this.multiNew('audio', inA, inB, azimuth, gain)
	}
	*kr { |inA, inB, azimuth, gain = 1|
		^this.multiNew('control', inA, inB, azimuth, gain)
	}
	init { |... theInputs|
		inputs = theInputs;
		channels = [
			OutputProxy(rate, this, 0),
			OutputProxy(rate, this, 1),
			OutputProxy(rate, this, 2)
		];
		^channels
	}
	checkInputs { ^this.checkNInputs(2) }
}

DecodeB2 : MultiOutUGen {
	*ar { |numChans, w, x, y, orientation = 0.5|
		^this.multiNew('audio', numChans, w, x, y, orientation = 0.5)
	}
	*kr { |numChans, w, x, y, orientation = 0.5|
		^this.multiNew('control', numChans, w, x, y, orientation = 0.5)
	}
	init { |numChans ... theInputs|
		inputs = theInputs;
		channels = Array.fill(numChans, { |i| OutputProxy(rate, this, i) });
		^channels
	}
	checkInputs { ^this.checkNInputs(3) }
}

PanAz : MultiOutUGen {
	*ar { |numChans, in, pos = 0.0, level = 1.0, width = 2.0, orientation = 0.5|
		^this.multiNew('audio', numChans, in, pos, level, width, orientation)
	}
	*kr { |numChans, in, pos = 0.0, level = 1.0, width = 2.0, orientation = 0.5|
		^this.multiNew('control', numChans, in, pos, level, width, orientation)
	}
	init { |numChans ... theInputs|
		inputs = theInputs;
		channels = Array.fill(numChans, { |i| OutputProxy(rate, this, i) });
		^channels
	}
	checkInputs { ^this.checkNInputs(1) }
}


XFade2 : UGen {
	// equal power two channel cross fade
	*ar { |inA, inB = 0.0, pan = 0.0, level = 1.0|
		^this.multiNew('audio', inA, inB, pan, level)
	}
	*kr { |inA, inB = 0.0, pan = 0.0, level = 1.0|
		^this.multiNew('control', inA, inB, pan, level)
	}
	checkInputs { ^this.checkNInputs(2) }
}

LinXFade2 : UGen {
	// linear two channel cross fade
	*ar { |inA, inB = 0.0, pan = 0.0, level = 1.0|
		^this.multiNew('audio', inA, inB, pan) * level
	}
	*kr { |inA, inB = 0.0, pan = 0.0, level = 1.0|
		^this.multiNew('control', inA, inB, pan) * level
	}
	checkInputs { ^this.checkNInputs(2) }
}
