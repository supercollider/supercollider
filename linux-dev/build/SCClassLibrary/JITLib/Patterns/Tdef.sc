//task definition

Tdef : PauseStream {
	var <key, <>repeats;
	
	*new { arg key, function, repeats=inf;
		var t;
		t = this.at(key);
		if(t.isNil, { 
			t = super.new.repeats_(repeats).function_(function);
			this.put(key, t);
		}, { 
			if(function.notNil, {
				t.function = function
			})
		 });
		^t
	} 

	function_ { arg function;
		var playing;
		playing = this.isPlaying;
		this.stop;
		originalStream = stream = Routine({ repeats.do(function) });
		if(playing, { this.start });
	}
	
	isPlaying { ^stream.isNil }
	
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