

EventStreamContainer  {
	var <>pattern, <eventstream; 
	
	*new { arg pattern;
		^super.newCopyArgs(pattern)
	}
		
		
	sendDef {}
	
	playToBundle { arg bundle, extraArgs, group; //mixedbundle
		bundle.addFunction({ 
			this.play(extraArgs, group) 
		}); //no latency (latency is in stream already)
		^nil
	}
	
	play { arg extraArgs, group;
		this.stop;
		eventstream = Pevent(pattern).asStream;
		^eventstream.play(SystemClock);
	}
	
	stop {
		if(eventstream.notNil, { eventstream.stream = nil });
		
	}
	
	stopClientProcessToBundle { arg bundle;
		bundle.addFunction({ this.stop });
	}


	sendDefToBundle {}
	

}


SynthDefContainer {
	var <synthDef;
	
	*new { arg synthDef;
		^super.newCopyArgs(synthDef);
	}
	
	sendDefToBundle { arg bundle;
		bundle.add(["/d_recv", synthDef.asBytes])
	}	
	sendDef { arg server;
		synthDef.send(server)
	}
	
	playToBundle { arg bundle, extraArgs, group;
		^Synth.newToBundle(bundle, synthDef.name, extraArgs, group, \addToTail); 
	}
	stopClientProcessToBundle {}
	stop {}


}