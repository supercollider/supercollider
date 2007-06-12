StreamClutch : Stream {
	var <>stream, <>connected, value, >reset=true;
	
	*new { arg pattern, connected=true;
		^super.newCopyArgs(pattern.asStream, connected)
	}
	
	next { arg inval;
		if(reset, { reset = false; value = stream.next(inval) });
		if(connected.value, {
			value = stream.next(inval);
		});
		^value
	}
	lastValue { ^value }
	
	reset { stream.reset; reset = true }
	step { arg inval; value = stream.next(inval ? Event.default); }

}


