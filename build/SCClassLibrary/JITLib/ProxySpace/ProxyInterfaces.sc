

//lightweight objects that insulate different ways of playing/stopping.

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
		^eventstream.play;
	}
	
	stop {
		if(eventstream.notNil, { eventstream.stream = nil });
		
	}
	
	stopToBundle { arg bundle;
		bundle.addFunction({ this.stop });
	}
	
	pause { this.stop } //clear this up.
	unpause { this.start }

	sendDefToBundle {}
	

}


SynthDefControl {
	var <synthDef, <hasGate, <synth;
	
	*new { arg synthDef, hasGate=true;
		^super.newCopyArgs(synthDef, hasGate);
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
		synth = Synth.prNew(synthDef.name,group.server);
		synth.isPlaying = true;
		bundle.add(synth.newMsg(group,\addToTail,extraArgs));
		^synth 
	}
	
	stopToBundle { arg bundle;
		if(synth.isPlaying,{
			if(hasGate, {
				bundle.add([15, synth.nodeID, '#gate', 0.0, \gate, 0.0]); //to be sure.
			}, {
				bundle.add([11, synth.nodeID]); //"/n_free"
			})
		});
	}
	
	stop { arg latency;
		var bundle;
		bundle = List.new;
		this.stopToBundle(bundle);
		synth.server.listSendBundle(latency, bundle)
	} 
	
	pause { synth.run(false) }
	unpause { synth.run(true) }

}

//todo
AbstractPlayerControl  {
	var <player;
	*new { arg player;
		^super.newCopyArgs(player)
	}
	
	playToBundle { arg bundle, extraArgs, group; //mixedbundle
		
	}
	
	stop { 
		player.free
	}

	stopToBundle { arg bundle;
		  //todo?
	}
	writeDef {
		player.asSynthDef.writeDefFile
	}

}



