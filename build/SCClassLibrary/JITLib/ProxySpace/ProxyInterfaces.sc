

//lightweight objects that insulate different ways of playing/stopping

EventStreamContainer  {
	var <>pattern, <eventstream; 
	
	*new { arg pattern;
		^super.newCopyArgs(pattern)
	}
		
		
	sendDef {}
	writeDef {}
	
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
	writeDef {
		synthDef.writeDefFile
	}
	
	playToBundle { arg bundle, extraArgs, group;
		var synth;
		synth = Synth.basicNew(synthDef.name,group.server,-1);
		bundle.add(synth.newMsg(group,\addToTail,extraArgs));
		^synth 
	}
	stopClientProcessToBundle {}
	stop {} //these are more efficiently done by group messages


}