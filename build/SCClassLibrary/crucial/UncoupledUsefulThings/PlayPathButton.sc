
PlayPathButton : ActionButton { // loads the object at loadPath and .plays it

	var <>subject,<isPlaying=false,player;

	*new { arg layout,loadPath,maxx=150;
		var new;
		loadPath = loadPath.asString;
		new=super.new(layout,PathName(loadPath).fileName,nil,maxx,13,3);
		new.backColor_(rgb(228,255,107)).action_({new.doAction}).subject_(loadPath)
		^new
	}
	
	doAction { 
		if(this.isPlaying,{
			isPlaying = false;
			this.backColor_(rgb(228,255,107));
			if(player.notNil,{player.stop });
		},{
			this.backColor_(Color.green);
			this.play;
		})
	}
	
	play { 
		isPlaying = true;
		player = subject.loadPath;
		
		^player.play 
	}

}


// .plays anything that responds to .play
PlayButton : PlayPathButton {
	
	play { subject.play }

}

/*

XPlayButton : PlayButton { // plays exclusively one thing at any time.
					//  all XPlayButtons share a single top level spawn.
					// clicking on any XPlayButton will start that player

	classvar spawn,prev;
 
	doAction {
		var thing;
		if(Synth.isPlaying,{
			if(spawn.notNil and: {spawn.isPlaying} and: {prev !== this},{
				spawn.triggerUgenFunc(thing = targetFunction.value);
				action.value(this,thing);
				this.displayOn;
			},{
				Synth.stop;
				this.displayOff;
			});
		},{
			this.displayOn;
			Synth.play({ arg synth;
				Tempo.setTempo;
				spawn = MonoSpawn(nil,2);
				synth.sched(0,{ spawn.triggerUgenFunc(thing = targetFunction.value); action.value(this,thing) });
				spawn.ar
			});
		});
		if(prev.notNil,{ prev.displayOff });
		prev = this;
	}
	displayOn { this.backColor_(Color.green) }
	displayOff { this.backColor_(rgb(228,255,107)) }

}

*/

/*


XPlayButton : StopStartPlayer {

	classvar tspawn;
	
	doAction {
		if(Synth.isPlaying,{
			if(tspawn.isNil or: { tspawn.state != 3 },{
				this.backColor_(rgb(228,255,107));
				Synth.stop;
			},{
				// start the tspawn
				
				
			});
		},{
			this.backColor_(Color.green);
			player.play;
		})
	}
}


TogglePlayButton // each one can add or drop self from play

*/

