//task definition

Tdef {
	var <>task, <key;
	
	*new { arg key, function, clock;
		var t;
		key = key.asSymbol;
		
		t = this.at(key);
		
		if(t.isNil, { 
			t = this.make(function, clock);
			this.put(key, t);
		}, { 
			if(function.notNil, {
				t.init(function, clock ? t.clock);
			})
		 });
		 
		^t
	} 
	
	*make { arg function, clock;
		^super.new.init(function, clock);
	}

	init { arg function, clock;
		var playing;
		playing = task.isPlaying;
		task.stop;
		task = Task({ arg argList; 
							function.valueArray(argList);
					}, clock);
		if(playing, { 
			{ task.start }.schedToBeat 
		});
	}
	
	clock { ^task.clock }
	
	stream_ { arg argStream; 
			task.stream = argStream;
	}
	
	
	embed { arg inval;
		task.embed(inval)
	}
	
	play { arg doReset = false;
		schedToCurrentBeat({
					task.stop;
					task.play(doReset)
		})
	}
	
	reset { ^task.reset }
	stop { task.stop }

	pause { task.pause }
	resume { task.resume }
	
	start { task.start }
	next { arg inval;
		^task.next(inval)
	}
	awake { arg beats, seconds, inClock;
		task.awake(beats, seconds, inClock);
	}

		
	
	///////global access
	
	*at { arg key;
		^Library.at(this, key);
	}
	
	*put { arg key, item;
		item.prSetKey(key);
		Library.put(this, key, item);
	}
	
	prSetKey { arg argKey;
		key = argKey;
	}


}

