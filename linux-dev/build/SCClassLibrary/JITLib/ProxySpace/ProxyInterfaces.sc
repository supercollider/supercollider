

//lightweight objects that insulate different ways of playing/stopping

EventStreamControl  {
	var <>pattern, <event, <eventstream; 
	
	*new { arg pattern, event;
		^super.newCopyArgs(pattern, event)
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
		eventstream = Pevent(pattern, event).asStream;
		^eventstream.play(SystemClock);
	}
	
	stop {
		if(eventstream.notNil, { eventstream.stream = nil });
		
	}
	
	stopToBundle { arg bundle;
		bundle.addFunction({ this.stop });
	}


	sendDefToBundle {}
	

}


SynthDefControl {
	var <synthDef, <synth;
	
	*new { arg synthDef;
		^super.newCopyArgs(synthDef);
	}
	
	sendDefToBundle { arg bundle;
		bundle.addPrepare(["/d_recv", synthDef.asBytes])
	}	
	sendDef { arg server;
		synthDef.send(server)
	}
	writeDef {
		synthDef.writeDefFile
	}
	
	playToBundle { arg bundle, extraArgs, group;
		synth = Synth.basicNew(synthDef.name,group.server,-1);
		bundle.add(synth.newMsg(group,\addToTail,extraArgs));
		^synth 
	}
	
	//these are more efficiently done by group messages

	stopToBundle { arg bundle;
		//if(synth.isPlaying,{
		//	bundle.add([15, synth.nodeID, \synthGate, 0.0, \gate, 0.0]);
		//});
	}
	stop {} 

}

//todo
AbstractPlayerControl : SynthDefControl {
	
	stop { }

	stopToBundle { arg bundle;
		  //todo?
	}
}



