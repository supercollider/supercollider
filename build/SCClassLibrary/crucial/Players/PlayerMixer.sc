PlayerMixer : AbstractPlayer {	var <>players;
	//,<>timeSplay=0.007;// 0.0029025 is one block		*new { arg players;		^super.new.players_(loadDocument(players) ? [])	}
	prepareForPlay { arg group,bundle;
		//super.prepareForPlay(group,bundle);
		players.do({ arg pl;
			pl.prepareForPlay(group,bundle)
		})	
	}		
	makePatchOut { arg group;
		// this will give me a bus
		super.makePatchOut(group);
		this.patchOut_(patchOut);
	}
	patchOut_ { arg po;
		patchOut = po;
		//everybody plays onto same bus
		players.do({ arg pl;
			pl.patchOut_(AudioPatchOut(pl,po.group,po.bus))
		})
	}
	spawnAtTime { arg atTime;
		var bundle;
		bundle = List.new;
		players.do({ arg pl;
			pl.spawnToBundle(bundle)
		});
		
		// atTime.asDeltaTime
		patchOut.server.listSendBundle( atTime, bundle//.insp("spawn")
											);
		//isPlaying = true;
		this.didSpawn;
	}
	spawnToBundle { arg bundle;
		players.do({ arg pl;
			pl.spawnToBundle(bundle)
		})
	}
	didSpawn { arg patchIn,synthArgi;
		super.didSpawn(patchIn,synthArgi);
		// right ? everybody gets connected.
		players.do({ arg pl;
			pl.didSpawn(patchIn,synthArgi);
		})
	}
	loadDefFileToBundle { arg bundle,server;
		players.do({ arg pl;
			pl.loadDefFileToBundle(bundle,server)
		})
	}		
	free {
		players.do({ arg pl;
			pl.free
		});
		super.free;
	}

	addPlayer { arg apl;		players = players.add(apl);
		// if playing...
	}	removePlayerAt { arg pli;		players.removeAt(pli)
		// if playing ...	}	putPlayer { arg i,pl;		players.put(i,pl)		// if playing ...
	}		storeParamsOn { arg stream;			stream.storeArgs([enpath(players)],1)	}		beatDuration {  ^players.maxItem({ arg pl; pl.beatDuration ? inf }).beatDuration }		children { ^players }	guiClass { ^PlayerMixerGui }}