
PlayerMixer : MultiplePlayers {  // will become a HasPatchIns

	var <>players;
	
	*new { arg players;
		^super.new.players_(loadDocument(players) ? [])
	}

	voices { ^this.players }
	
	asSynthDef {
		^SynthDef(this.defName,{ arg out=0;
			var inputBusses;
			if(players.size > 1,{
				inputBusses = Control.names(Array.fill(players.size,{ arg i; "in"++i.asString}))
							.ir(Array.fill(players.size,0))
							.collect({ arg in; In.ar(in,this.numChannels) });
				Out.ar(out,
					Mix.ar(
						inputBusses
					)
				)
			},{ // rare that you would use only one but ...
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

	addPlayer { arg apl;
		players = players.add(apl);
		// if playing...
	}
	removePlayerAt { arg pli;
		players.removeAt(pli)
		// if playing change ...
	}
	putPlayer { arg i,pl;
		players.put(i,pl)
		// if playing change ...
	}

	
	beatDuration {  ^players.maxItem({ arg pl; pl.beatDuration ? inf }).beatDuration }
	
	children { ^this.players }
	storeArgs { ^[players] }

	guiClass { ^PlayerMixerGui }

}
