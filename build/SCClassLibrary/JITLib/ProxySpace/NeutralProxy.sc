NeutralProxy : AbstractFunction {
	var <>proxySpace, <>key, <>numChan;
	
	*new { arg proxySpace,  key, numChan;
		^super.newCopyArgs(proxySpace,  key, numChan);	
	}

	kr { arg nChan;
		var result;
		result = ControlProxy(proxySpace.server, nChan ? numChan ? 1);
		proxySpace.prPut(key, result);
		^result.kr
	}
	
	ar { arg nChan;
		var result;
		result = AudioProxy(proxySpace.server, nChan ? numChan ? 2);
		proxySpace.prPut(key, result);
		^result.ar
	}
	value { ^this.kr }

	play { arg outBus=0, nChan=2; 
		var result;
		result = AudioProxy(proxySpace.server, nChan ? numChan ? 2);
		proxySpace.prPut(key, result);
 		result.play(outBus, nChan) 
	}
	bus {
		"I don't have a bus yet".inform;
		^nil
	}
	obj_ { arg obj;
		^this.notYetImplemented(thisMethod);
	}
}

