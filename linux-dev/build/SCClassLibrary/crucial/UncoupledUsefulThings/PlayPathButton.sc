
PlayPathButton : SCButtonAdapter { // loads the object at loadDocument and .plays it

	var <>subject,<isPlaying=false,player;

	*new { arg layout,loadDocument,maxx=150;
		var new;
		loadDocument = loadDocument.asString;
		new=super.new;
		new.makeView(layout,maxx,15);
		new.initOneState(PathName(loadDocument).fileName,nil,rgb(228,255,107));
		new.action_({new.doAction})
			.subject_(loadDocument);
			//.align_(\right);
		^new
	}
	
	doAction { 
		if(this.isPlaying,{
			this.stop;
		},{
			this.play;
		})
	}
	stop {
		isPlaying = false;
		this.backColor_(rgb(228,255,107));
		if(player.notNil,{player.stop });		
	}
	play { 
		isPlaying = true;
		player = subject.loadDocument;
		this.backColor_(Color.green);
		
		^player.play 
	}

}


// .plays anything that responds to .play
PlayButton : PlayPathButton {
	
	play { subject.play }

}


XPlayPathButton : PlayPathButton {

	classvar last;
	
	doAction {
		if(this.isPlaying.not,{
			if(last.notNil, { last.stop });
			this.play;
			last = this;
		},{
			this.stop;
			last = nil;
		})
	}
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
	
*/


