

PlayPathButton : SCButtonAdapter { // loads the object at loadDocument and .plays it

	var <>subject,player,<>action;

	*new { arg layout,path,minWidth=150;
		var new;
		path = path.asString;
		path.dump;
		new=super.new;
		new.makeView(layout,minWidth,GUI.skin.buttonHeight);
		new.view.action_({new.doAction});
		new.path_(path);
		^new
	}
	path_ { arg p;
		subject = p.asString;
		this.initOneState(PathName(subject).fileName,nil,rgb(228,255,107));
	}

	doAction {
		if(player.isPlaying ? false,{
			this.stop;
		},{
			this.play;
		});
		action.value(this);
	}
	stop {
		this.background_(rgb(228,255,107));
		if(player.notNil,{player.stop });
	}
	play {
		player = subject.loadDocument;
		this.background_(rgb(255, 215, 0));
		player.play(atTime:1);
	}
	isPlaying { ^player.isPlaying }

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
	displayOn { this.background_(Color.green) }
	displayOff { this.background_(rgb(228,255,107)) }

}

	doAction {
		if(this.isPlaying,{
			isPlaying = false;
			this.background_(rgb(228,255,107));
			if(player.notNil,{player.stop });
		},{
			this.background_(Color.green);
			this.play;
		})
	}

*/



