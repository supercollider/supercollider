//task definition

Tdef {
	var <>task, <key, <isPlaying=false;
	
	*new { arg key, function, clock;
		var t;
		key = key.asSymbol;
		
		t = this.at(key);
		if(function.notNil, {
			if(t.isNil, { 
				t = this.make(function, clock);
				this.put(key, t);
			}, { 
				
				t.init(function, clock ? t.clock);
			})
		 }, {
		 	if(t.isNil, {
				t = this.make(this.default, TempoClock.default);
				this.put(key, t);
			});
		 });
		 
		^t
	} 
	
	*make { arg function, clock;
		^super.new.init(function, clock);
	}

	init { arg function, clock;
		var oldTask;
		oldTask = task;
		task = Task({ arg argList; 
							oldTask.stop;
							function.valueArray(argList);
					}, clock);
					
		if(isPlaying, { task.start });
	}
	
	*default { ^{} }
	clock { ^task.clock }
	
	stream_ { arg argStream; 
			task.stream = argStream;
	}
	
	
	embed { arg inval;
		task.embed(inval)
	}
	
	play { arg inclock, doReset = false;
		schedToCurrentBeat({
					task.stop;
					isPlaying = true;
					task.play(inclock, doReset)
		})
	}
	
	reset { ^task.reset }
	stop { task.stop; isPlaying = false; }

	pause { task.pause }
	resume { task.resume }
	
	start { task.start; isPlaying = true; }
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
	
	//////private implementation
	
	
	prSetKey { arg argKey;
		key = argKey;
	}


}

