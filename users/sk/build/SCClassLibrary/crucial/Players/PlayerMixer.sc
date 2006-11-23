
PlayerMixer : MultiplePlayers {  // will become a HasPatchIns

	var <>players;
	
	*new { arg players;
		^super.new.players_(loadDocument(players) ? [])
	}

	voices { ^this.players }
	
	asSynthDef {
		^SynthDef(this.defName,{ arg out=0;
			var inputBuses;
			if(players.size > 1,{
				inputBuses = Control.names(Array.fill(players.size,{ arg i; "in"++i.asString}))
							.ir(Array.fill(players.size,0))
							.collect({ arg in; In.ar(in,this.numChannels) });
				Out.ar(out,
					Mix.new(
						inputBuses
					)
				)
			},{
				Out.ar(out,
					In.ar( Control.names(["in0"]).ir([0]), this.numChannels)
				)
			})
		});
	}
	defName { ^this.class.asString ++ this.players.size.asString ++ this.numChannels.asString }
	synthDefArgs {
		var l;
		l = Array(players.size * 2 + 2);
		l.addAll(["out",this.synthArg]);
		players.do({ arg pl,pli;
				l.addAll(["in" ++ pli.asString,pl.synthArg]);
		});
		^l
	}
	respawnMixerToBundle { arg bundle;
		var dn,bytes;
		if(synth.notNil,{
			bundle.add(synth.freeMsg);
		});
		synth = Synth.basicNew(this.defName,server);
		NodeWatcher.register(synth);
		this.annotate(synth,"synth,respawned");

		dn = this.defName.asSymbol;
		if(Library.at(SynthDef,this.server,dn).isNil,{
			bundle.add(["/d_recv", this.asSynthDef.asBytes,
					synth.addToTailMsg(this.group,this.synthDefArgs) ]);
			Library.put(SynthDef,server,dn,true);
		},{
			bundle.add(
				synth.addToTailMsg(this.group,this.synthDefArgs)
			);
		});
	}
		
	addPlayer { arg player;
		var bundle;
		players = players.add(player);
		if(this.isPlaying,{
			bundle = CXBundle.new;
			//start player
			player.makePatchOut(this.group,true,nil,bundle);
			
			
			
			
			
			//TODO   prepareToBundle
			
			
			
			
			
			
			player.spawnToBundle(bundle);
			this.respawnMixerToBundle(bundle);
			bundle.send(this.server);
		});
	}
	removePlayerAt { arg pli;
		var bundle,player;
		player = players.removeAt(pli);
		if(player.notNil and: {this.isPlaying},{
			bundle = CXBundle.new;
			//stop player
			player.releaseToBundle(0.1,bundle);
			this.respawnMixerToBundle(bundle);
			bundle.send(this.server);
		});
	}
	putPlayer { arg i,pl;
		players.put(i,pl);
		// if playing stop the old one in that spot
		// and spawn the new one on that bus
		if(this.isPlaying,{
			"PlayerMixer-putPlayer while playing not yet implemented.".warn;
		});
	}
	
	beatDuration {  ^players.maxItem({ arg pl; pl.beatDuration ? inf }).beatDuration }
	
	children { ^this.players }
	storeArgs { ^[players] }

	guiClass { ^PlayerMixerGui }

}
