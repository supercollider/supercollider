

MultiplePlayers : AbstractPlayer {

	// manages multiple players sharing the same bus
	// doesn't make a synth

	voices { this.subclassResponsiblity(thisMethod) }
	
	rate { ^this.voices.first.rate }
	numChannels { ^this.voices.first.numChannels }
	
	// prepare  :  making a single bus, may override in subclasses

	childrenMakePatchOut { arg group,private = false;
		this.voices.do({ arg vo;
			// use mine
			vo.patchOut_(AudioPatchOut(vo,patchOut.group,patchOut.bus.copy));
			// but children make their own
			vo.childrenMakePatchOut(group,false);
		})
	}
	patchOut_ { arg po;
		patchOut = po;
		//everybody plays onto same bus
		this.voices.do({ arg pl;
			// ISSUE if rate is not mine, throw an error
		})
	}
	spawnAtTime { arg atTime;
		var bundle;
		bundle = List.new;
		this.voices.do({ arg pl;
			pl.spawnToBundle(bundle)
		});
		
		// atTime.asDeltaTime
		patchOut.server.listSendBundle( atTime, bundle);
		//isPlaying = true;
		this.didSpawn;
	}
	spawnToBundle { arg bundle;
		this.voices.do({ arg pl;
			pl.spawnToBundle(bundle)
		})
	}
	didSpawn { arg patchIn,synthArgi;
		super.didSpawn(patchIn,synthArgi);
		// right ? everybody gets connected.
		this.voices.do({ arg pl;
			pl.didSpawn(patchIn,synthArgi);
		})
	}
	loadDefFileToBundle { arg bundle,server;
		this.voices.do({ arg pl;
			pl.loadDefFileToBundle(bundle,server)
		})
	}		
	free {
		this.voices.do({ arg pl;
			pl.free
		});
		super.free;
	}
}

PlayerMixer : MultiplePlayers {	var <>players;
	//,<>timeSplay=0.007;// 0.0029025 is one block		*new { arg players;		^super.new.players_(loadDocument(players) ? [])	}
	voices { ^this.players }
		addPlayer { arg apl;		players = players.add(apl);
		// if playing...
	}	removePlayerAt { arg pli;		players.removeAt(pli)
		// if playing change ...	}	putPlayer { arg i,pl;		players.put(i,pl)		// if playing change ...
	}		storeParamsOn { arg stream;			stream.storeArgs([enpath(players)],1)	}		beatDuration {  ^players.maxItem({ arg pl; pl.beatDuration ? inf }).beatDuration }	
	children { ^this.players }

	guiClass { ^PlayerMixerGui }}