

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