

//lightweight objects that insulate different ways of playing/stopping.
AbstractPlayControl {
	sendDef {}
	writeDef {}
	prepareForPlay {}
	pause { this.stop } 
	unpause { this.start }

	sendDefToBundle {}
	
	playToBundle { arg bundle; //mixedbundle
		bundle.addFunction({ 
			this.play; 
		}); //no latency (latency is in stream already)
		^nil //return a nil object instead of a synth
	}
	
	stopToBundle { arg bundle;
		bundle.addFunction({ this.stop });
	}
	
	play {
		this.subclassResponsibility(thisMethod);
	}
	
	stop {
		this.subclassResponsibility(thisMethod);
	}
}

EventStreamControl : AbstractPlayControl {
	var <>stream; 
	
	*new { arg stream;
		^super.newCopyArgs(stream)
	}
		
	play {
		stream.stop;
		stream.play(true);
	}
	
	stop {
		stream.stop;
	}
}

NumericalControl : AbstractPlayControl {
	var <>bus, <>value; 
	
	*new { arg bus, value=0;
		^super.newCopyArgs(bus, value)
	}
	
	play {
		bus.set(value);
	}
	
	stop {}

}


SynthDefControl {
	var <synthDef, <hasGate, <synth;
	
	*new { arg synthDef;
		^super.newCopyArgs(synthDef, synthDef.hasGate);
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
	prepareForPlay {
		this.writeDef;
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

SoundDefControl : SynthDefControl {
	sendDef { } //assumes that SoundDef does send to the same server 

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



