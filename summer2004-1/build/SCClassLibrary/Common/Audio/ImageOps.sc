

XY : MultiOutUGen {
	*ar { arg xscale = 1.0, yscale = 1.0, xoff = 0.0, yoff = 0.0, rot = 0.0, rate = 1.0;
		^this.multiNew('audio', xscale, yscale, xoff, yoff, rot, rate)
	}
	*kr { arg xscale = 1.0, yscale = 1.0, xoff = 0.0, yoff = 0.0, rot = 0.0, rate = 1.0;
		^this.multiNew('control',  xscale, yscale, xoff, yoff, rot, rate)
	}
	init { arg ... argInputs;
		inputs = argInputs;
		channels = [ OutputProxy.kr1(this,0), OutputProxy.kr1(this,1), OutputProxy.kr1(this,2) ];
		^channels
	}
}	

ImageWarp : MultiOutUGen {
	var pic;
	
	*ar { 
		arg pic, x=0.0, y=0.0, interpolationType = 1;
		^this.multiNew('audio', pic, x, y, interpolationType)
	}
	*kr {
		arg pic, x=0.0, y=0.0, interpolationType = 1;
		^this.multiNew('control', pic, x, y, interpolationType)
	}
	
	init { arg aPic ... theInputs;
 		inputs = theInputs;
 		pic = aPic;
		channels = [ OutputProxy.kr1(this,0), OutputProxy.kr1(this,1), OutputProxy.kr1(this,2) ];
		^channels
 	}
}
